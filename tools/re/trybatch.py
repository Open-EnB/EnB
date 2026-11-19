"""One hypothesis batch (D273): compile N source forms of one function lock-free, in parallel on leased pool VMs,
measure each against the client and print a short summary (at most ~30 lines); the full detail goes to a log.

    python tools/re/trybatch.py run VARIANTS.py [--vms K] [--no-lease] [--slots] [--record [STATUS]] [--only NAME...]
    python tools/re/trybatch.py regions OBJ SYMBOL ADDR [--show K]       per-region residue of one compiled object
    python tools/re/trybatch.py slots OBJ LISTING SYMBOL ADDR            frame symbol -> client slot (a /FAs listing)

VARIANTS.py (write it with a file tool in .work/<id>/, never a heredoc) defines
    HYPOTHESIS = 'what this batch tests'           one line; goes to the summary and to --record's evidence
    FILE = 'app/shared/unknown_46c730.cpp'         repository-relative (the run's own tree: run from your worktree)
    SYMBOL = 'LandClass::Unknown_46F120'           as explain.py takes it (demangled, mangled or bare name)
    ADDRESS = 0x46F120                             the client's function start
    FORMS = {name: form, ...}                      a form is one of
        [(old, new), ...]                          edit hunks on FILE (each `old` exactly once)
        [(relpath, old, new), ...]                 edit hunks on any file, headers included (tryv2's V format)
        'void C::f(...)\\n{ ... }\\n'                a whole definition: replaces FILE's definition with the same
                                                   signature (matched ignoring whitespace; leading // lines dropped)
    V = {...}                                      accepted instead of FORMS (tryv2.py's variant files run as they are)
    FLAGS = '...'                                  optional: overrides the file's flags (default libdefs.flags_of:
                                                   the library's own, so a library file never gets the app's /GX)
Strings use '\\n' (converted to the file's own line ending). The unmodified file is always compiled too, as `base`.

How it runs. Every form becomes an overlay tree .work/trybatch/<stem>-<time>/<form>/ holding only the files the
form edits, at their repository-relative paths; it is compiled from there as verify compiles the file (the same
defines, flags, include list and, for app files, the e:\\build\\beta\\G __FILE__ alias rooted at the overlay), with
the tree file's own folder put on the include list so its relative #includes still resolve. Edited headers are
found when the draft includes them (quoted, from the draft's folder) or through <>: their overlay folders go first
on the include list; a tree header's own quoted #include of its neighbour still finds the tree's copy (VC6 searches
the including file's folder first). The tree, the importer lock and the ledger are never touched.
VMs: --vms K (default min(forms + 1, 3)) leased one at a time from pool.py under $ENB_AGENT (else trybatch-<pid>):
the first waits up to 15 minutes, the others are taken only when free at once (pool.py keeps its memory reserve),
each with one retry; all are released at the end, whatever happens. --no-lease uses ENB_VC6_HOSTS / vc6_hosts.json
as given (a lease you already hold). One thread per VM; forms are queued over them.

Measures, per form (explain.py's masks; ours vs the client from ADDRESS to the next known start):
    res     masked residue rows (stack offsets, targets, relocations masked): the progress measure
    regs    the same with registers also masked
    raw     rows that differ with only targets / relocations masked (stack offsets kept)
    frame   our frame size / the client's
    score   1 - res / max(our rows, client rows), two decimals: the masked fraction that matches
            (attempts.csv's score column, so rounds stay comparable)
    class   explain.py's class
    lines   shown when the form changes the file's line count: every later __LINE__ (asserts, FAIL_IF) shifts, which
            explain.py calls `literal`; keep the count (blank lines) as variants.py try does
Forms whose code is identical (instruction text, call targets and relocations) are grouped: VC6 canonicalises
declaration and operand order and names, so such forms are one result, not several. Regions: both sides are cut at
their calls, the call sequences aligned (our callee's symbols.json address against the client's target; by index
when the counts agree), and the rows between two aligned calls form a region; the best form's regions with a
residue are printed. --slots compiles with /FAs as well and, for the best form, maps each frame symbol of our
listing onto the client's slot on the aligned rows (`*`: some row puts it elsewhere).

--record [STATUS] (default partial; exact when a form is exact) writes the batch's attempts.csv row through
attempts.py record: the hypothesis, the forms count, the best form's score and class, and `residue N` (plus regs,
raw, the baseline's residue) in the evidence, and stashes the variants file (docs/attempts/<ADDR>/): attempts.py's
D271 progress test reads that figure before the score.
"""
import argparse
import collections
import difflib
import os
import re
import runpy
import subprocess
import sys
import threading
import time
import traceback

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
import explain as X  # noqa: E402

NL = chr(10)
CRLF = chr(13) + chr(10)
MAX_FORM_LINES = 18
MAX_REGION_LINES = 8
BLOCKER = {'register-only': 'register', 'order-only': 'order', 'identical': '', 'exact': ''}


# ---------------------------------------------------------------- the variants file and the edits

def load_spec(path):
    g = runpy.run_path(path)
    forms = g.get('FORMS', g.get('V'))
    if not forms:
        raise SystemExit(f'{path}: no FORMS (or V) dict')
    for k in ('FILE', 'SYMBOL', 'ADDRESS'):
        if k not in g:
            raise SystemExit(f'{path}: {k} missing')
    addr = g['ADDRESS']
    addr = int(addr, 16) if isinstance(addr, str) else int(addr)
    if 'base' in forms:
        raise SystemExit(f'{path}: `base` is the unmodified file; name the form otherwise')
    return {'hypothesis': ' '.join(str(g.get('HYPOTHESIS', '')).split()), 'file': g['FILE'].replace(chr(92), '/'),
            'symbol': g['SYMBOL'], 'address': addr, 'forms': dict(forms), 'flags': g.get('FLAGS')}


def _squeeze(text):
    """text without whitespace, and for each kept character its index in text."""
    keep = [i for i, c in enumerate(text) if not c.isspace()]
    return ''.join(text[i] for i in keep), keep


def _block_end(text, i):
    """Index just past the brace block opening at text[i] == '{' (comments, strings and char literals skipped)."""
    depth, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith('//', i):
            j = text.find(NL, i)
            i = n if j < 0 else j
            continue
        if text.startswith('/*', i):
            j = text.find('*/', i + 2)
            i = n if j < 0 else j + 2
            continue
        if c in '"' "'":
            i += 1
            while i < n and text[i] != c:
                i += 2 if text[i] == chr(92) else 1
        elif c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise ValueError('unbalanced braces')


def replace_function(text, body):
    """text with the definition whose signature (up to its first '{', whitespace ignored) is body's replaced by body."""
    lines = body.split(NL)
    while lines and (not lines[0].strip() or lines[0].lstrip().startswith('//')):
        lines.pop(0)
    body = NL.join(lines)
    if '{' not in body:
        raise ValueError('a whole-function form needs a body in braces')
    sig, _ = _squeeze(body[:body.index('{')])
    flat, keep = _squeeze(text)
    hits = [m.start() for m in re.finditer(re.escape(sig + '{'), flat)]
    if len(hits) != 1:
        raise ValueError(f'the signature occurs {len(hits)} times in the file: {body[:body.index("{")].strip()[:80]!r}')
    start = keep[hits[0]]
    brace = keep[hits[0] + len(sig)]
    end = _block_end(text, brace)
    new = body.rstrip(NL)
    if CRLF in text:
        new = new.replace(CRLF, NL).replace(NL, CRLF)
    return text[:start] + new + text[end:]


def apply_form(read, file, form):
    """{relpath: new text} for one form; read(relpath) gives a tree file's text (newline='')."""
    out = {}

    def get(rel):
        if rel not in out:
            out[rel] = read(rel)
        return out[rel]

    if isinstance(form, str):
        out[file] = replace_function(get(file), form)
        return out
    for edit in form:
        if len(edit) == 2:
            rel, (old, new) = file, edit
        else:
            rel, old, new = edit
        rel = rel.replace(chr(92), '/')
        text = get(rel)
        if CRLF in text:
            old, new = (s.replace(CRLF, NL).replace(NL, CRLF) for s in (old, new))
        n = text.count(old)
        if n != 1:
            raise ValueError(f'anchor occurs {n} times in {rel}: {old[:80]!r}')
        out[rel] = text.replace(old, new)
    out.setdefault(file, get(file))
    return out


# ---------------------------------------------------------------- measuring

def call_key(ins, known):
    """What a call goes to: the client address (ours through symbols.json when it names one place), else a name."""
    t = ins.target
    if not t:
        return None
    if t[0] == 'addr':
        return t[1]
    if t[0] == 'sym':
        addrs = known.get(t[1], ())
        return next(iter(addrs)) if len(addrs) == 1 else t[1]
    return None


def call_label(ins):
    t = ins.target
    if not t:
        return '?'
    if t[0] == 'sym':
        name = X.demangle(t[1]) if hasattr(X, 'demangle') else t[1]
        return name[:40]
    return f'0x{t[1]:06X}' if isinstance(t[1], int) else str(t[1])


def regions(L, R, known=None):
    """[(i0, i1, j0, j1, label)]: the rows of ours L[i0:i1] and the client's R[j0:j1] between aligned calls."""
    known = X.symbol_addresses() if known is None else known
    cl = [i for i, (ins, _) in enumerate(L) if ins.mn == 'call']
    cr = [j for j, (ins, _) in enumerate(R) if ins.mn == 'call']
    kl = [call_key(L[i][0], known) for i in cl]
    kr = [call_key(R[j][0], known) for j in cr]
    if len(cl) == len(cr):
        pairs = list(zip(cl, cr))
    else:
        sm = difflib.SequenceMatcher(None, kl, kr, autojunk=False)
        pairs = [(cl[b.a + k], cr[b.b + k]) for b in sm.get_matching_blocks() for k in range(b.size)]
    out, i0, j0 = [], 0, 0
    for i, j in pairs:
        out.append((i0, i + 1, j0, j + 1, call_label(R[j][0]) if R[j][0].target else call_label(L[i][0])))
        i0, j0 = i + 1, j + 1
    out.append((i0, len(L), j0, len(R), 'end'))
    return out


def region_rows(L, R, eL, eR, known=None):
    rows = []
    for k, (i0, i1, j0, j1, label) in enumerate(regions(L, R, known)):
        SA = [X.mask_stack(t, eL) for _, t in L[i0:i1]]
        SB = [X.mask_stack(t, eR) for _, t in R[j0:j1]]
        rs, _ = X.residue(SA, SB)
        rr, _ = X.residue([X.mask_regs(x) for x in SA], [X.mask_regs(x) for x in SB])
        at = R[j0][0].addr if j0 < len(R) else (R[-1][0].addr if R else 0)
        rows.append({'k': k, 'ours': i1 - i0, 'client': j1 - j0, 'res': rs, 'regs': rr, 'at': at, 'label': label,
                     'span': (i0, i1, j0, j1)})
    return rows


def show_region(L, R, eL, eR, span):
    i0, i1, j0, j1 = span
    SA = [X.mask_stack(t, eL) for _, t in L[i0:i1]]
    SB = [X.mask_stack(t, eR) for _, t in R[j0:j1]]
    _, sm = X.residue(SA, SB)
    out = []
    for tag, a1, a2, b1, b2 in sm.get_opcodes():
        for m in range(max(a2 - a1, b2 - b1)):
            x = SA[a1 + m] if a1 + m < a2 else ''
            y = SB[b1 + m] if b1 + m < b2 else ''
            ya = f'{R[j0 + b1 + m][0].addr:06x} ' if b1 + m < b2 else ''
            out.append(f"{' ' if tag == 'equal' else '*'} {x:44.44s} | {ya}{y}")
    return out


def score_of(res, n_ours, n_client):
    """The batch score: the masked fraction that matches, 1 - res / max(rows)."""
    n = max(n_ours, n_client, 1)
    return max(0.0, 1 - res / n)


def code_key(L):
    """Identical code: the same instructions (as text), call targets and relocations."""
    return tuple((t, ins.target, tuple(ins.refs)) for ins, t in L)


def is_exact(obj_path, symbol, addr):
    from objmatch import CoffObject
    from clientimage import ClientImage
    obj = CoffObject(obj_path)
    cands = X.pick(obj.functions(), symbol)
    if not cands:
        return False
    _, sec, s, e = cands[0]
    pat, _, length = obj.pattern(sec, s, e)
    img = ClientImage()
    o = img.offset(addr)
    return bool(re.fullmatch(pat, img.data[o:o + length], re.S))


def measure(obj_path, symbol, addr, known=None):
    """The numbers of one compiled form (dict), plus its rows for regions."""
    result, (L, R, SL, SR, sm, at) = X.explain(obj_path, symbol, f'0x{addr:X}')
    raw, _ = X.residue([t for _, t in L], [t for _, t in R])
    eL, eR = X.ebp_frame(L), X.ebp_frame(R)
    exact = is_exact(obj_path, symbol, addr)
    m = {'res': result['residue'], 'regs': result['residue_regs'], 'raw': raw,
         'frame': X.frame_of(L)[0], 'frame_client': X.frame_of(R)[0],
         'ours': len(L), 'client': len(R), 'class': 'exact' if exact else result['class'],
         'reason': result.get('reason', ''), 'exact': exact,
         'score': 1.0 if exact else score_of(result['residue'], len(L), len(R)), 'key': code_key(L)}
    m['regions'] = region_rows(L, R, eL, eR, known)
    m['_rows'] = (L, R, eL, eR)
    return m


# ---------------------------------------------------------------- the frame-slot map (BG2's symmap)

LIST_REF = re.compile(r'([_$?][\w$@?]*)\$?\[(esp|ebp)([+-]\d+)?\]')
CLIENT_REF = re.compile(r'\[(esp|ebp)(?: ([+-]) (0x[0-9a-f]+|\d+))?\]')


def listing_body(text, symbol):
    """(frame symbol table {name: offset}, instruction lines) of SYMBOL's PROC in a VC6 /FAs listing."""
    procs = list(re.finditer(r'^(\S+)\s+PROC NEAR[^\n]*$', text, re.M))
    last = symbol.split('::')[-1]
    pick = [p for p in procs if p.group(1) == symbol
            or p.group(0).split(';', 1)[-1].split(', COMDAT')[0].strip() == symbol]
    pick = pick or [p for p in procs if p.group(1).startswith('?' + last + '@')]
    if not pick:
        raise ValueError(f'{symbol} not in the listing')
    p = pick[0]
    end = text.index('ENDP', p.end())
    head = text[:p.start()]
    seg = max(head.rfind('_TEXT' + chr(9) + 'SEGMENT'), head.rfind('_TEXT SEGMENT'), 0)
    table = {}
    for line in head[seg:].splitlines():
        m = re.match(r'^(\S+)\s*=\s*(-?\d+)$', line.strip())
        if m:
            table[m.group(1)] = int(m.group(2))
    ins = []
    for line in text[p.end():end].splitlines():
        s = line.split(';')[0].strip()
        if not s or s.endswith(':') or s.startswith('npad') or re.match(r'^\$\w+:', s):
            continue
        if re.match(r'^[a-z]', s):
            ins.append(s)
    return table, ins


def slot_map(table, ins, L, R, sm):
    """[(our offset, name, Counter(client slot))]: each listing symbol's client slot on the aligned (equal) rows."""
    seen = collections.defaultdict(collections.Counter)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != 'equal':
            continue
        for k in range(i2 - i1):
            i = i1 + k
            if i >= len(ins) or ins[i].split()[0] != L[i][0].mn:
                continue
            ours = LIST_REF.findall(ins[i])
            theirs = CLIENT_REF.findall(R[j1 + k][0].raw)
            for (name, reg, delta), (creg, sign, val) in zip(ours, theirs):
                if reg != creg:
                    continue
                d = int(delta or 0)
                v = int(val, 0) if val else 0
                seen[name + '$' if name + '$' in table else name][(-v if sign == '-' else v) - d] += 1
    rows = [(off, name, seen.get(name, collections.Counter())) for name, off in table.items()]
    return sorted(rows, key=lambda r: (-r[0], r[1]))


def slot_lines(rows):
    out = []
    for off, name, c in rows:
        if not c:
            continue
        where = ', '.join(f'{y:+d}:{n}' for y, n in c.most_common(3))
        out.append(f'  {off:+5d} {name[:28]:28s} -> {where}{"" if set(c) == {off} else "   *"}')
    return out


# ---------------------------------------------------------------- VMs

def lease(k, agent, log):
    """Lease up to k VMs one at a time: [(name, host)]. The first waits (bounded), the others only if free now."""
    got = []
    pool = [sys.executable, os.path.join(REPO, 'tools', 'vc6', 'pool.py')]
    for n in range(k):
        for attempt in range(2):
            p = subprocess.run(pool + ['lease', '--agent', agent, '--count', '1', '--wait', '900' if n == 0 else '0'],
                               capture_output=True, text=True)
            log(p.stdout + p.stderr)
            names = re.findall(r'^(\S+): \S+ ready', p.stderr, re.M)
            line = [l for l in p.stdout.splitlines() if l.startswith('ENB_VC6_HOSTS=')]
            if p.returncode == 0 and line and names:
                got.append((names[0], line[-1].split('=', 1)[1].split(',')[0]))
                break
            release([x for x in names], log)
            if n:      # at the cap or short of memory: no more VMs for this batch
                return got
            time.sleep(5)
        else:
            return got
    return got


def release(names, log):
    if names:
        p = subprocess.run([sys.executable, os.path.join(REPO, 'tools', 'vc6', 'pool.py'), 'release'] + list(names),
                           capture_output=True, text=True)
        log(p.stdout + p.stderr)


# ---------------------------------------------------------------- compiling

def compile_form(vc6, libdefs, overlay, file, flags, listing):
    """Compile overlay/<file> as verify compiles FILE; returns (rc, compiler text, object path or None)."""
    lib = libdefs.lib_of(os.path.join(REPO, file))
    src = os.path.join(overlay, file.replace('/', os.sep))
    tree_dir = os.path.dirname(os.path.join(REPO, file.replace('/', os.sep)))
    edited_dirs = sorted({d for d, _, fs in os.walk(overlay) if any(f.lower().endswith(libdefs.HEADER_EXT) for f in fs)})
    includes = edited_dirs + [tree_dir] + [d for d in libdefs.tree_includes()
                                           if os.path.normcase(d) != os.path.normcase(tree_dir)]
    sync = [d for d, _, _ in os.walk(overlay) if os.path.basename(d) != 'obj'] + libdefs.late_sync()
    out = os.path.join(overlay, 'obj')
    if listing:
        flags = f'{flags} /FAs /Fa"{listing}"'
    app = libdefs.APP_LIBS.get(lib)
    if app:
        root = libdefs.dest(lib)
        sync += [d for d, _, _ in os.walk(root)]
        rc, text = vc6.compile_sources([src], includes, defines=app['defines'], out=out, flags=flags,
                                       extra_sync=sync, source_alias=(overlay, app['file_root']))
    else:
        rc, text = vc6.compile_sources([src], includes, out=out, flags=flags, extra_sync=sync)
    stem = os.path.splitext(os.path.basename(file))[0].lower()
    objs = [os.path.join(out, f) for f in os.listdir(out) if f.lower() == stem + '.obj'] if os.path.isdir(out) else []
    return rc, text, (objs[0] if objs else None)


def first_error(text):
    errs = [l.strip() for l in text.splitlines() if ' error ' in l or 'fatal error' in l]
    return errs[0][-110:] if errs else (text.strip().splitlines() or ['no object'])[-1][-110:]


# ---------------------------------------------------------------- the summary

def fmt_form(name, m, group=None):
    if 'error' in m:
        return f'{name[:22]:22s} COMPILE FAILED: {m["error"]}'
    g = f'  = {group}' if group else ''
    dl = f' lines {m["dlines"]:+d}' if m.get('dlines') else ''
    return (f'{name[:22]:22s} res {m["res"]:4d} regs {m["regs"]:4d} raw {m["raw"]:4d} frame {m["frame"]}/{m["frame_client"]}'
            f' rows {m["ours"]}/{m["client"]} score {m["score"]:.2f} {m["class"]}{dl}{g}')


def summary(spec, results, order, slots=None):
    """The printed lines: header, base, forms by residue, identical groups, the best form's regions."""
    lines = [f'trybatch {spec["symbol"]} 0x{spec["address"]:06X} [{spec["file"]}]: {spec["hypothesis"][:150]}']
    groups = collections.OrderedDict()
    for name in order:
        m = results.get(name, {})
        if 'key' in m:
            groups.setdefault(m['key'], []).append(name)
    leader = {}
    for names in groups.values():
        for n in names[1:]:
            leader[n] = names[0]
    base = results.get('base')
    if base:
        lines.append(fmt_form('base', base))
    ok = sorted((n for n in order if n != 'base' and 'res' in results.get(n, {}) and n not in leader),
                key=lambda n: (results[n]['res'], results[n]['regs'], results[n]['raw']))
    bad = [n for n in order if n != 'base' and 'error' in results.get(n, {})]
    shown = ok[:MAX_FORM_LINES]
    for n in shown:
        same = [x for x in groups[results[n]['key']] if x != n]
        lines.append(fmt_form(n, results[n], ', '.join(same)[:40] if same else None))
    if len(ok) > len(shown):
        lines.append(f'... {len(ok) - len(shown)} more distinct forms in the log')
    for n in bad[:4]:
        lines.append(fmt_form(n, results[n]))
    if len(bad) > 4:
        lines.append(f'... {len(bad) - 4} more failed compiles in the log')
    n_forms = len([n for n in order if n != 'base'])
    lines.append(f'{n_forms} forms, {len(ok)} distinct codes (identical forms grouped), {len(bad)} failed')
    if base and 'key' in base:
        same_as_base = [n for n in groups.get(base['key'], []) if n != 'base']
        if same_as_base:
            lines.append(f'identical to base: {", ".join(same_as_base)[:120]}')
    best = ok[0] if ok else ('base' if base and 'regions' in base else None)   # every form identical to base
    if best:
        regs = results[best]['regions']
        bad_regs = [r for r in regs if r['res']]
        lines.append(f'best {best}: {len(regs)} regions, {len(bad_regs)} with residue'
                     + (f' (base: {sum(1 for r in base["regions"] if r["res"])})' if base and 'regions' in base else ''))
        for r in sorted(bad_regs, key=lambda r: -r['res'])[:MAX_REGION_LINES]:
            lines.append(f'  r{r["k"]:<3d} @0x{r["at"]:06X} rows {r["ours"]:3d}/{r["client"]:3d} res {r["res"]:3d} '
                         f'regs {r["regs"]:3d}  -> {r["label"]}')
    if slots:
        lines += slots[:6]
    return lines, best


# ---------------------------------------------------------------- run

def cmd_run(a):
    import vc6
    import libdefs
    spec = load_spec(a.variants)
    file = spec['file']
    if not os.path.exists(os.path.join(REPO, file)):
        raise SystemExit(f'{file} is not in {REPO} (run trybatch from the tree that holds it)')
    flags = spec['flags'] or libdefs.flags_of(os.path.join(REPO, file))
    stem = os.path.splitext(os.path.basename(a.variants))[0]
    batch = os.path.join(REPO, '.work', 'trybatch', f'{stem}-{time.strftime("%H%M%S")}')
    os.makedirs(batch)
    logf = open(os.path.join(batch, 'trybatch.log'), 'w', encoding='utf-8', newline=NL)
    lock = threading.Lock()

    def log(text):
        with lock:
            logf.write(text.rstrip() + NL)
            logf.flush()

    log(f'variants {os.path.abspath(a.variants)}; file {file}; flags {flags}; lib {libdefs.lib_of(os.path.join(REPO, file))}')
    forms = collections.OrderedDict([('base', [])])
    for name, form in spec['forms'].items():
        if not a.only or name in a.only:
            forms[name] = form

    def read(rel):
        return open(os.path.join(REPO, rel.replace('/', os.sep)), encoding='latin1', newline='').read()

    results, todo, dlines = {}, [], {}
    for name, form in forms.items():
        if not re.fullmatch(r'[\w.-]+', name):
            raise SystemExit(f'form name {name!r}: letters, digits, _ . - only')
        try:
            files = apply_form(read, file, form)
        except (ValueError, OSError) as e:
            results[name] = {'error': f'edit: {e}'}
            log(f'== {name}: {e}')
            continue
        overlay = os.path.join(batch, name)
        for rel, text in files.items():
            p = os.path.join(overlay, rel.replace('/', os.sep))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            open(p, 'w', encoding='latin1', newline='').write(text)
        dlines[name] = files[file].count(NL) - read(file).count(NL)
        todo.append(name)
    agent = os.environ.get('ENB_AGENT') or f'trybatch-{os.getpid()}'
    leased = []
    try:
        if a.no_lease:
            hosts = list(dict.fromkeys(vc6.hosts()))[:a.vms or None]   # one thread per VM (sync.tar, the alias)
        else:
            leased = lease(a.vms or min(len(todo), 3), agent, log)
            hosts = [h for _, h in leased]
            if not hosts:
                raise SystemExit('trybatch: no VM leased (pool at its cap or short of memory); see ' + logf.name)
        print(f'{len(todo)} compiles on {len(hosts)} VM(s); log {os.path.relpath(logf.name, REPO)}', flush=True)
        queue = collections.deque(todo)
        known = X.symbol_addresses()

        def worker(host):
            vc6.use_host(host)
            while True:
                with lock:
                    if not queue:
                        return
                    name = queue.popleft()
                overlay = os.path.join(batch, name)
                listing = f'{vc6.WORK}\\trybatch_{os.getpid()}_{name}.asm' if a.slots else None
                try:
                    rc, text, obj = compile_form(vc6, libdefs, overlay, file, flags, listing)
                    log(f'== {name} on {host}: rc {rc}' + NL + text.strip()[-3000:])
                    if not obj:
                        results[name] = {'error': first_error(text)}
                        continue
                    if listing:
                        vc6.scp(vc6.remote(listing), os.path.join(overlay, 'listing.asm'))
                        vc6.ssh(f'cmd /c del "{listing}"', check=False)
                    with lock:
                        results[name] = measure(obj, spec['symbol'], spec['address'], known)
                        results[name]['dlines'] = dlines[name]
                except BaseException as e:      # one form's failure is a result, not the batch's end
                    results[name] = {'error': f'{type(e).__name__}: {e}'[:110]}
                    log(f'== {name}: ' + traceback.format_exc())

        threads = [threading.Thread(target=worker, args=(h,)) for h in hosts]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
    finally:
        release([n for n, _ in leased], log)
    order = list(forms)
    for name in order:
        m = results.get(name, {})
        log(fmt_form(name, m) if m else f'{name}: no result')
        if 'regions' in m:
            for r in m['regions']:
                log(f'   r{r["k"]} @0x{r["at"]:06X} rows {r["ours"]}/{r["client"]} res {r["res"]} regs {r["regs"]} -> {r["label"]}')
    lines, best = summary(spec, results, order)
    slots = []
    if best and a.slots:
        lst = os.path.join(batch, best, 'listing.asm')
        try:
            L, R, eL, eR = results[best]['_rows']
            SL = [X.mask_stack(t, eL) for _, t in L]
            SR = [X.mask_stack(t, eR) for _, t in R]
            _, sm = X.residue(SL, SR)
            table, ins = listing_body(open(lst, encoding='latin1').read(), spec['symbol'])
            slots = slot_lines(slot_map(table, ins, L, R, sm))
            log('slots of ' + best + NL + NL.join(slots))
            lines.append(f'slots of {best} (ours -> client:count; all in the log):')
            lines += [s for s in slots if s.endswith('*')][:6] or slots[:4]
        except (OSError, ValueError) as e:
            lines.append(f'slots: {e}')
    if best:
        L, R, eL, eR = results[best]['_rows']
        for r in results[best]['regions']:
            if r['res']:
                log(f'-- {best} region r{r["k"]} -> {r["label"]}' + NL + NL.join(show_region(L, R, eL, eR, r['span'])))
    for line in lines:
        log(line)
    print(NL.join(lines))
    if a.record is not None and best:
        m, b = results[best], results.get('base', {})
        status = 'exact' if m['exact'] else a.record
        ev = (f'trybatch {stem} ({len(order) - 1} forms): {spec["hypothesis"]}; best {best} residue {m["res"]} regs {m["regs"]}'
              f' raw {m["raw"]} frame {m["frame"]}/{m["frame_client"]}'
              + (f'; base residue {b["res"]}' if 'res' in b else ''))
        cmd = [sys.executable, os.path.join(HERE, 'attempts.py'), 'record', f'0x{spec["address"]:06X}', '--symbol',
               spec['symbol'], '--file', file, '--status', status, '--score', f'{m["score"]:.2f}', '--forms',
               str(len(order) - 1), '--blocker', BLOCKER.get(m['class'], m['class']), '--evidence', ev,
               '--stash', os.path.abspath(a.variants)]
        p = subprocess.run(cmd, capture_output=True, text=True, cwd=REPO)
        print((p.stdout + p.stderr).strip().splitlines()[-1] if (p.stdout + p.stderr).strip() else 'attempts: no output')
    logf.close()


def cmd_regions(a):
    m = measure(a.obj, a.symbol, int(a.address, 16))
    L, R, eL, eR = m['_rows']
    print(fmt_form(os.path.basename(a.obj), m))
    for r in m['regions']:
        print(f'r{r["k"]:<3d} @0x{r["at"]:06X} rows {r["ours"]:3d}/{r["client"]:3d} res {r["res"]:3d} regs {r["regs"]:3d}'
              f'  -> {r["label"]}')
        if a.show == r['k']:
            print(NL.join(show_region(L, R, eL, eR, r['span'])))


def cmd_slots(a):
    result, (L, R, SL, SR, sm, at) = X.explain(a.obj, a.symbol, a.address)
    table, ins = listing_body(open(a.listing, encoding='latin1').read(), a.symbol)
    print(f'listing instructions {len(ins)}, object rows {len(L)}')
    print(NL.join(slot_lines(slot_map(table, ins, L, R, sm))))


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split(NL)[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('run')
    r.add_argument('variants')
    r.add_argument('--vms', type=int, default=0, help='VMs to lease (default min(compiles, 3))')
    r.add_argument('--no-lease', action='store_true', help='use ENB_VC6_HOSTS / vc6_hosts.json as given')
    r.add_argument('--slots', action='store_true', help='also map the best form\'s frame symbols (/FAs listing)')
    r.add_argument('--record', nargs='?', const='partial', choices=('partial', 'parked', 'blocked'),
                   help='write the attempts.csv row (status, default partial)')
    r.add_argument('--only', nargs='*', help='only these forms (base is always compiled)')
    g = sub.add_parser('regions')
    g.add_argument('obj')
    g.add_argument('symbol')
    g.add_argument('address')
    g.add_argument('--show', type=int)
    s = sub.add_parser('slots')
    s.add_argument('obj')
    s.add_argument('listing')
    s.add_argument('symbol')
    s.add_argument('address')
    a = ap.parse_args(argv)
    {'run': cmd_run, 'regions': cmd_regions, 'slots': cmd_slots}[a.cmd](a)


if __name__ == '__main__':
    main()
