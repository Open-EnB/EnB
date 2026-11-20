"""One hypothesis batch (D273): compile N source forms of one function lock-free, in parallel on leased pool VMs,
measure each against the client and print a short summary (at most ~30 lines); the full detail goes to a log.

    python tools/re/trybatch.py run VARIANTS.py [--vms K] [--no-lease] [--slots] [--record [STATUS]] [--only NAME...]
                                [--base] [--file-check]
    python tools/re/trybatch.py base FILE SYMBOL ADDR [--unit CPP] [--file-check]   a base-only measure, no FORMS
    python tools/re/trybatch.py regions OBJ SYMBOL ADDR [--show K]       per-region residue of one compiled object
    python tools/re/trybatch.py slots OBJ LISTING SYMBOL ADDR            frame symbol -> client slot (a /FAs listing)

VARIANTS.py (write it with a file tool in .work/<id>/, never a heredoc) defines
    HYPOTHESIS = 'what this batch tests'           one line; goes to the summary and to --record's evidence
    FILE = 'app/shared/unknown_46c730.cpp'         repository-relative (the run's own tree: run from your worktree)
    SYMBOL = 'LandClass::Unknown_46F120'           as explain.py takes it (demangled, mangled or bare name); also
                                                   with its arguments (`f(char *, int)`, picks an overload), a
                                                   template member (`A<B>::f`), an annotation (`_$E (X)`) or '':
                                                   resolved on the compiled base (below)
    ADDRESS = 0x46F120                             the client's function start
    FORMS = {name: form, ...}                      a form is one of
        [(old, new), ...]                          edit hunks on FILE (each `old` exactly once)
        [(relpath, old, new), ...]                 edit hunks on any file, headers included (tryv2's V format)
        'void C::f(...)\\n{ ... }\\n'                a whole definition: replaces FILE's definition with the same
                                                   signature (matched ignoring whitespace; leading // lines dropped)
    V = {...}                                      accepted instead of FORMS (tryv2.py's variant files run as they are)
    FLAGS = '...'                                  optional: overrides the file's flags (default libdefs.flags_of:
                                                   the library's own, so a library file never gets the app's /GX)
    UNIT = 'app/client/co_data.cpp'                optional, for a header FILE: the source file to compile (default:
                                                   one that includes it and defines the function in the last build,
                                                   by build/<lib>/symbols.txt, else one carrying ADDRESS's marker)
Strings use '\\n' (converted to the file's own line ending). The unmodified file is always compiled too, as `base`;
--base (or the `base` command) compiles only it, FORMS not needed.

Which function. SYMBOL is resolved on the first compiled object, in order: an exact name; the full signature
against the last build's build/<lib>/symbols.txt (argument lists, template members); a name containing SYMBOL
without its arguments; symbols.json's names of ADDRESS; the function at ADDRESS's own marker in FILE, UNIT or their
headers (a comment whose first address is ADDRESS, then the definition; or a declaration with it in its comment).
Several candidates: the one symbols.json places at ADDRESS, else the lowest residue. A resolution other than the
exact name prints as a `symbol ... by the ...` line. A FILE that no longer defines the function (the last build
puts it, or ADDRESS's marker, in another source) is an error naming that file; with --base the batch moves there.

How it runs. Every form becomes an overlay tree .work/trybatch/<stem>-<time>/<form>/ holding only the files the
form edits, at their repository-relative paths; it is compiled from there as verify compiles the file (the same
defines, flags, include list and, for app files, the e:\\build\\beta\\G __FILE__ alias rooted at the overlay), with
the tree file's own folder put on the include list so its relative #includes still resolve. Edited headers are
found when the draft includes them (quoted, from the draft's folder) or through <>: their overlay folders go first
on the include list; a tree header's own quoted #include of its neighbour still finds the tree's copy (VC6 searches
the including file's folder first). The tree, the importer lock and the ledger are never touched.
VMs (D273: the host used fully): --vms K caps them (default: one per compile); they are leased one at a time from
pool.py under $ENB_AGENT (else trybatch-<pid>) until pool.py refuses (its cap, its memory reserve): the first waits
up to 15 minutes (one retry), the others are taken only when free at once, and each starts compiling as soon as it
is ready; all are released at the end, whatever happens. --no-lease uses ENB_VC6_HOSTS / vc6_hosts.json as given
(a lease you already hold). One thread per distinct host (two on one VM collide on sync.tar); forms are queued
over them.

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
    BRANCH(Ns/Mt A/Bb)  shown when res is 0 but an aligned jump is short on one side and near on the other (N), reaches
            another row (M), or the code lengths differ (A/B bytes): the target mask hid a logic difference (SR2)
    TABLES  a switch's jump / byte table differs (explain.py ends the code at the first table on both sides and
            compares the entries apart: jump entries by rank, which cases share a body; the best form's tables print
            as `switch ... table` lines, equal or different, never as residue rows)
    ASSERT-TEXT  the form changes the text of a FAIL_IF / G_ASSERT / WWASSERT / ASSERT / assert condition (a renamed
            identifier inside one): the stringified text is the client's .rdata, which relocation masking hides
    FILE -L/+G  with --file-check: the form loses L / gains G exact functions of the whole object against the base's
            (objmatch's match, a `multiple` credited at the file's markers or symbols.json, as appfile.py match
            credits): a lever that drops another function (a template instance's out-of-line copy) shows
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

def load_spec(path, need_forms=True):
    g = runpy.run_path(path)
    forms = g.get('FORMS', g.get('V')) or {}
    if not forms and need_forms:
        raise SystemExit(f'{path}: no FORMS (or V) dict')
    for k in ('FILE', 'SYMBOL', 'ADDRESS'):
        if k not in g:
            raise SystemExit(f'{path}: {k} missing')
    addr = g['ADDRESS']
    addr = int(addr, 16) if isinstance(addr, str) else int(addr)
    if 'base' in forms:
        raise SystemExit(f'{path}: `base` is the unmodified file; name the form otherwise')
    return {'hypothesis': ' '.join(str(g.get('HYPOTHESIS', '')).split()), 'file': g['FILE'].replace(chr(92), '/'),
            'symbol': g['SYMBOL'], 'address': addr, 'forms': dict(forms), 'flags': g.get('FLAGS'),
            'unit': (g.get('UNIT') or '').replace(chr(92), '/') or None}


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


# ---------------------------------------------------------------- which unit, which symbol

SOURCE_EXT = ('.cpp', '.c')
DEF_NAME = re.compile(r'([~\w]+(?:<[^()]*?>)?(?:::[~\w]+(?:<[^()]*?>)?)*)\s*\(')
CALLCONV = re.compile(r'\b(__thiscall|__cdecl|__stdcall|__fastcall)\b')
ADDR_TOKEN = re.compile(r'0x([0-9A-Fa-f]{6,8})\b')
_index = {}


def main_repo():
    """The main working tree (a worktree's built objects are often only there)."""
    try:
        p = subprocess.run(['git', 'rev-parse', '--path-format=absolute', '--git-common-dir'], cwd=REPO,
                           capture_output=True, text=True)
        return os.path.dirname(p.stdout.strip()) if p.returncode == 0 and p.stdout.strip() else REPO
    except OSError:
        return REPO


def build_file(lib, *parts):
    """A file of the tree's last build (this tree's build/, else the main tree's), or None."""
    for root in dict.fromkeys((REPO, main_repo())):
        p = os.path.join(root, 'build', lib, *parts)
        if os.path.exists(p):
            return p
    return None


def built_object(lib, rel):
    return build_file(lib, 'obj', os.path.splitext(os.path.basename(rel))[0].lower() + '.obj')


def lib_index(lib):
    """{mangled: (full demangled name, [object stems that define it])} of the functions of the last build's
    build/<lib>/symbols.txt (dumpbin /symbols), {} without one."""
    if lib not in _index:
        out = {}
        path = build_file(lib, 'symbols.txt')
        stem = None
        for line in open(path, encoding='latin1') if path else ():
            if line.startswith('Dump of file '):
                stem = os.path.splitext(os.path.basename(line.split()[-1].replace(chr(92), '/')))[0].lower()
                continue
            if ' SECT' not in line or '()' not in line or 'External' not in line or '|' not in line:
                continue
            rest = line.split('|', 1)[1].strip()
            name, _, dem = rest.partition(' ')
            dem = dem.strip()[1:-1] if dem.startswith('(') else name
            out.setdefault(name, (dem, []))[1].append(stem)
        _index[lib] = out
    return _index[lib]


def norm_sig(text):
    """(qualified name, argument list or None) without access, return type, calling convention, class / struct /
    enum keywords and spaces: 'public: void __thiscall A<class B>::f(char *,int)' -> ('A<B>::f', '(char*,int)')."""
    m = CALLCONV.search(text)
    if m:
        text = text[m.end():]
    text = re.sub(r'\b(class|struct|enum|union)\s+', '', text)
    text = re.sub(r'\s+', '', text)
    if '(' not in text:
        return text, None
    k = text.index('(')
    args = text[k:].replace('(void)', '()')
    return text[:k], args


def sig_matches(symbol, demangled):
    n1, a1 = norm_sig(symbol)
    n2, a2 = norm_sig(demangled)
    return bool(n1) and n1 == n2 and (a1 is None or a1 == a2 or a2 is not None and a2.startswith(a1))


def own_mentions(text, addr):
    """Names defined or declared at a mention of addr that is its own marker (the comment's first address is addr):
    a definition after such a comment line, or a declaration carrying it in its own comment (`f(int);  // 0x867250`).
    In the file's order."""
    lines = text.splitlines()
    out = []
    for k, line in enumerate(lines):
        code, _, comment = line.partition('//')
        first = ADDR_TOKEN.search(comment)
        if not first or int(first.group(1), 16) != addr:
            continue
        if '(' in code:
            out += DEF_NAME.findall(code.split('(')[0] + '(')[-1:]
            continue
        if code.strip():
            continue
        for nxt in lines[k + 1:k + 8]:
            s = nxt.strip()
            if not s or s.startswith(('//', '/*', '*', '#', 'template')):
                continue
            if '(' in s:
                out += DEF_NAME.findall(s.split('(')[0] + '(')[-1:]
            break
    return list(dict.fromkeys(out))


def untemplate(name):
    """name without its template arguments: 'A<B<int> >::f' -> 'A::f'."""
    while True:
        new = re.sub(r'<[^<>]*>', '', name)
        if new == name:
            return name
        name = new


def class_mentions(text, addr):
    """(Class::name, hint words) for each own marker of addr on a member declared inside a class / struct body
    (`static T *Get(...)  // 0x79E300 (radar)` in a template's body: own_mentions gives the bare `Get`, which
    every class has); the hint is the marker comment's words after the address (which instance it is)."""
    lines = text.splitlines()
    out = []
    for k, line in enumerate(lines):
        code, _, comment = line.partition('//')
        first = ADDR_TOKEN.search(comment)
        if not first or int(first.group(1), 16) != addr or '(' not in code:
            continue
        names = DEF_NAME.findall(code.split('(')[0] + '(')[-1:]
        if not names or '::' in names[0]:
            continue
        depth, cls = 0, None
        for j in range(k - 1, -1, -1):
            c = lines[j].partition('//')[0]
            depth += c.count('}') - c.count('{')
            if depth < 0:
                for back in range(j, max(-1, j - 4), -1):
                    m = re.search(r'\b(?:class|struct)\s+(\w+)\s*(?::[^{;]*)?\{?\s*$',
                                  lines[back].partition('//')[0])
                    if m:
                        cls = m.group(1)
                        break
                break
        if cls:
            hint = [w.lower() for w in re.findall(r'[A-Za-z]{4,}', comment[first.end():])]
            out.append((f'{cls}::{names[0]}', hint))
    return out


def candidates(names, symbol, addr, texts=(), index=None):
    """(names, how): which of names (an object's mangled function names) SYMBOL means, found in order by an exact
    name (mangled, demangled or bare, as explain.py's pick), by the full signature (the build's symbols.txt: an
    argument list tells overloads apart, a template member is found by its template name), by a name containing
    SYMBOL without its argument list or annotation (`_$E (PermStringInit)`), by symbols.json's names of addr, and by
    the sources' own marker of addr (a stale SYMBOL, a renamed function)."""
    index = index or {}

    def full(n):
        return index[n][0] if n in index else X.demangle(n)

    def exact(sym):
        got = [n for n in names if sym in (n, X.demangle(n)) or (X.demangle(n).rsplit('::', 1)[-1] == sym
                                                                 and '::' not in sym)]
        return got or [n for n in names if sig_matches(sym, full(n))]

    got = exact(symbol)
    if got:
        return got, 'symbol'
    bare = re.sub(r'\s*\(.*\)\s*(const)?\s*$', '', symbol).strip()
    if bare:
        got = [n for n in names if bare in n or bare in X.demangle(n)]
        if got:
            return got, 'name without its arguments'
    at = {n for n, a in X.symbol_addresses().items() if addr in a}
    got = [n for n in names if n in at]
    if got:
        return got, 'symbols.json name of the address'
    for text in texts:
        for name in own_mentions(text, addr):
            got = exact(name)
            if got and ('::' in name or len({X.demangle(n) for n in got}) == 1):
                return got, f"source's marker of 0x{addr:06X} ({name})"
    for text in texts:
        for name, hint in class_mentions(text, addr):
            got = [n for n in names if untemplate(norm_sig(full(n))[0]) == name]
            narrowed = [n for n in got if any(w in full(n).lower() for w in hint)]
            if narrowed or got:
                return narrowed or got, f"source's marker of 0x{addr:06X} in its class ({name})"
    return [], ''


def client_length(addr):
    """Bytes from addr to the next verified or proven start (explain.py's window end)."""
    from clientimage import ClientImage
    starts = sorted(set(ClientImage().function_starts()[0]) | set(X._names_at()))
    return next((x for x in starts if x > addr), addr + 64) - addr


def by_code(obj_path, addr, limit=40, least=0.8):
    """An empty SYMBOL with no symbols.json name or marker at addr (a header static's `_$E` initialiser): the
    object's functions whose size is within half to twice the client's length (up to limit, nearest first)
    that score at least LEAST against addr, ordered by masked residue; [] when none does (a function defined
    inline everywhere has no copy, and a near-length neighbour is no answer)."""
    from objmatch import CoffObject
    want = client_length(addr)
    funcs = [f for f in CoffObject(obj_path).functions() if want / 2 <= f[3] - f[2] <= want * 2 + 16]
    funcs.sort(key=lambda f: abs((f[3] - f[2]) - want))
    scored = []
    for name, _, _, _ in funcs[:limit]:
        try:
            r, _ = X.explain(obj_path, name, f'0x{addr:X}')
        except (SystemExit, Exception):
            continue
        if score_of(r['residue'], r.get('ours', 1), r.get('client', 1)) >= least:
            scored.append((r['residue'], name))
    return [n for _, n in sorted(scored)]


def resolve_symbol(obj_path, symbol, addr, texts=(), lib='app'):
    """(mangled name, how) of the function that SYMBOL means in a compiled object: among several candidates the
    one symbols.json places at addr, else the one with the lowest masked residue against the client."""
    from objmatch import CoffObject
    names = [f[0] for f in CoffObject(obj_path).functions()]
    got, how = candidates(names, symbol, addr, texts, lib_index(lib))
    if not got and not symbol.strip():
        got = by_code(obj_path, addr)
        how = "code (the lowest residue of the object's functions near the client's length, score >= 0.8)"
        if got:
            return got[0], how
    if not got:
        key = re.sub(r'\s*\(.*$', '', symbol).split('::')[-1].strip().lower()
        near = [X.demangle(n) for n in names if key and key in X.demangle(n).lower()][:3]
        raise ValueError(f'{symbol!r} is not in the object (inlined at every call, or a stale name?)'
                         + (f'; near: {", ".join(near)}' if near else ''))
    got = list(dict.fromkeys(got))
    if len(got) > 1:
        placed = [n for n in got if addr in X.symbol_addresses().get(n, ())]
        if len(placed) == 1:
            return placed[0], how + ', the one symbols.json places there'
        scored = []
        for n in placed or got:
            try:
                r, _ = X.explain(obj_path, n, f'0x{addr:X}')
                scored.append((r['residue'], n))
            except (SystemExit, Exception):
                continue
        if scored:
            scored.sort()
            return scored[0][1], how + f', the lowest residue of {len(scored)}'
    return got[0], how


def texts_for(read, *files):
    """The texts of files and of their same-stem headers (a declaration's marker: `f(int);  // 0x867250`)."""
    out = []
    for f in dict.fromkeys(files):
        for rel in [f] + [os.path.splitext(f)[0] + h for h in ('.h', '.hpp') if not f.endswith(h)]:
            try:
                out.append(read(rel))
            except OSError:
                pass
    return out


def sources_of(file):
    """The source files (repository-relative) under FILE's top folder."""
    out = []
    for d, _, fs in os.walk(os.path.join(REPO, file.split('/')[0])):
        for f in fs:
            if f.lower().endswith(SOURCE_EXT):
                out.append(os.path.relpath(os.path.join(d, f), REPO).replace(os.sep, '/'))
    return sorted(out)


def defining_units(spec, read):
    """The source files whose last built object defines the function (by the build's symbols.txt), and failing
    that the ones that carry ADDRESS's own marker at a definition."""
    import libdefs
    lib = libdefs.lib_of(os.path.join(REPO, spec['file']))
    index = lib_index(lib)
    texts = texts_for(read, spec['file'])
    srcs = sources_of(spec['file'])
    by_stem = {}
    for rel in srcs:
        by_stem.setdefault(os.path.splitext(os.path.basename(rel))[0].lower(), rel)
    marked = []
    for rel in srcs:
        text = read(rel)
        if '%X' % spec['address'] in text.upper() and own_mentions(text, spec['address']):
            marked.append(rel)
            texts.append(text)
    names = list(index)
    got, _ = candidates(names, spec['symbol'], spec['address'], texts, index) if index else ([], '')
    units = [by_stem[s] for n in got[:4] for s in index[n][1] if s in by_stem]
    return list(dict.fromkeys(units)), marked


def compile_unit(spec, read):
    """The source file to compile: FILE itself, or for a header FILE (an inline or template member) UNIT when the
    variants file gives one, else a source file that includes the header (directly) and defines the function in
    the last build, else any that defines it, else one that carries ADDRESS's marker."""
    file = spec['file']
    if file.lower().endswith(SOURCE_EXT):
        return file
    if spec.get('unit'):
        return spec['unit']
    units, marked = defining_units(spec, read)
    head = os.path.basename(file).lower()
    incl = re.compile(r'#\s*include\s*[<"]([^>"]+)[>"]')
    direct = [u for u in units if any(os.path.basename(x).lower() == head for x in incl.findall(read(u)))]
    pick = direct + units + marked
    if not pick:
        raise ValueError(f'{file} is a header: no source file defines {spec["symbol"]} in the last build or carries '
                         f'0x{spec["address"]:06X} at a definition; give UNIT = the .cpp to compile')
    return pick[0]


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
         'score': 1.0 if exact else score_of(result['residue'], len(L), len(R)),
         'tables': result.get('tables', []), 'branch': result.get('branch')}
    m['key'] = code_key(L) + tuple((t['kind'], t['ours'], t['diff']) for t in m['tables'])
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

def lease_iter(k, agent, log, more=lambda: True):
    """Lease up to k VMs one at a time, yielding (name, host) as each is ready: the first waits (bounded), the
    others only if free now (pool.py's cap and memory reserve decide how many); stops early once more() is false."""
    pool = [sys.executable, os.path.join(REPO, 'tools', 'vc6', 'pool.py')]
    for n in range(k):
        if n and not more():
            return
        for attempt in range(2):
            p = subprocess.run(pool + ['lease', '--agent', agent, '--count', '1', '--wait', '900' if n == 0 else '0'],
                               capture_output=True, text=True)
            log(p.stdout + p.stderr)
            names = re.findall(r'^(\S+): \S+ ready', p.stderr, re.M)
            line = [l for l in p.stdout.splitlines() if l.startswith('ENB_VC6_HOSTS=')]
            if p.returncode == 0 and line and names:
                yield names[0], line[-1].split('=', 1)[1].split(',')[0]
                break
            release([x for x in names], log)
            if n:      # at the cap or short of memory: no more VMs for this batch
                return
            time.sleep(5)
        else:
            return


def lease(k, agent, log):
    """[(name, host)] of lease_iter."""
    return list(lease_iter(k, agent, log))


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

def table_lines(tables):
    """A switch's tables, apart from the residue (explain.py ends the code at the first table)."""
    return [f'  switch {t["kind"]} table' + (f' @0x{t["at"]:06X}' if t['at'] else '') + f' entries {t["ours"]}/{t["client"]} '
            + ('equal' if not t['diff'] else f'{t["diff"]} different') for t in tables]


def fmt_form(name, m, group=None):
    if 'error' in m:
        return f'{name[:22]:22s} COMPILE FAILED: {m["error"]}'
    g = f'  = {group}' if group else ''
    dl = f' lines {m["dlines"]:+d}' if m.get('dlines') else ''
    b = m.get('branch')
    if b and not m.get('exact') and not m.get('res') and X.branch_differs(b):
        dl += f' BRANCH({b["size"]}s/{b["target"]}t {b["bytes"][0]}/{b["bytes"][1]}b)'
    if m.get('file') and any(m['file']):
        dl += f' FILE -{m["file"][0]}/+{m["file"][1]}'
    if m.get('asserts'):
        dl += ' ASSERT-TEXT ' + '; '.join(m['asserts'])[:40]
    td = [t for t in m.get('tables', ()) if t['diff']]
    if td:
        dl += ' TABLES ' + ','.join(f'{t["kind"]} {t["diff"]}/{t["n"]}' for t in td)
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
        lines += table_lines(results[best].get('tables', ()))
    if slots:
        lines += slots[:6]
    return lines, best


# ---------------------------------------------------------------- whole-file and assert checks

ASSERT_MACRO = re.compile(r'\b(FAIL_IF|G_ASSERT|WWASSERT|ASSERT|assert)\s*\(')


def assert_conditions(text):
    """collections.Counter of the stringified conditions of FAIL_IF / G_ASSERT / WWASSERT / ASSERT / assert in
    text (whitespace runs as one space, as the preprocessor's # makes them): the text the client keeps in .rdata."""
    out = collections.Counter()
    for m in ASSERT_MACRO.finditer(text):
        line_start = text.rfind(NL, 0, m.start()) + 1
        if text[line_start:m.start()].lstrip().startswith(('#', '//')):
            continue
        i, depth = m.end(), 1
        while i < len(text) and depth:
            depth += {'(': 1, ')': -1}.get(text[i], 0)
            i += 1
        if depth == 0:
            out[' '.join(text[m.end():i - 1].split())] += 1
    return out


def assert_renames(read, file, form):
    """The assert conditions a form changes (gone from the tree's text; a renamed identifier inside one shows here):
    relocation masking hides the string, objmatch does not. [] for the base and for forms that keep them all."""
    if not form:
        return []
    try:
        files = apply_form(read, file, form)
    except (ValueError, OSError):
        return []
    gone = []
    for rel, text in files.items():
        before, after = assert_conditions(read(rel)), assert_conditions(text)
        gone += [c for c in before if before[c] > after.get(c, 0)]
    return gone


def exact_set(obj_path, img=None, mentioned=frozenset()):
    """{(symbol, address)} the object's exact functions, as appfile.py match credits them: the bytes match at the
    one place objmatch's match_objects finds, or, for a `multiple`, at a place symbols.json gives the symbol or the
    file's text names (its `// 0xADDR` markers: mentioned); a function too short to search for, at the places
    symbols.json gives it."""
    from objmatch import match_objects, CoffObject
    from clientimage import ClientImage
    img = img or ClientImage()
    known = X.symbol_addresses()
    out, obj = set(), None
    for r in match_objects(img, [obj_path]):
        hits = [int(a, 16) for a in r['addresses']]
        if r['status'] == 'exact' and hits:
            out.add((r['symbol'], hits[0]))
        elif r['status'] == 'multiple':
            out |= {(r['symbol'], h) for h in hits if h in known.get(r['symbol'], ()) or h in mentioned}
        elif r['status'] == 'short' and known.get(r['symbol']):    # too short to search: at its named place only
            obj = obj or CoffObject(obj_path)
            f = next((f for f in obj.functions() if f[0] == r['symbol']), None)
            if f:
                pat, _, length = obj.pattern(*f[1:])
                out |= {(r['symbol'], h) for h in known[r['symbol']]
                        if re.fullmatch(pat, img.data[img.offset(h):img.offset(h) + length], re.S)}
    return out


def regroup(results):
    """Forms with the function's code identical but another whole-file or assert result are not one result."""
    for m in results.values():
        if 'key' in m and (any(m.get('file', ())) or m.get('asserts')):
            m['key'] = (m['key'], ('file', m.get('file')), ('asserts', tuple(m.get('asserts', ()))))


def file_check(objs, results, log, exact=exact_set, mentioned=frozenset()):
    """--file-check (LV2): every form's exact set of the whole file against the base's, so a lever that drops
    another function (another template instance's out-of-line copy, a neighbour's inlining) shows."""
    from clientimage import ClientImage
    img = ClientImage()
    sets = {}
    for name, obj in objs.items():
        try:
            sets[name] = exact(obj, img, mentioned)
        except Exception as e:
            log(f'file check {name}: {type(e).__name__}: {e}')
    base = sets.get('base')
    if base is None:
        return
    for name, got in sets.items():
        if name == 'base' or name not in results or 'error' in results[name]:
            continue
        lost, gained = sorted(base - got), sorted(got - base)
        results[name]['file'] = (len(lost), len(gained))
        if lost or gained:
            log(f'file check {name}: lost {", ".join(f"{X.demangle(s)} 0x{a:06X}" for s, a in lost)}; '
                f'gained {", ".join(f"{X.demangle(s)} 0x{a:06X}" for s, a in gained)}')
    if 'base' in results and 'error' not in results['base']:
        results['base']['file'] = (0, 0)
        log(f'file check base: {len(base)} exact')


# ---------------------------------------------------------------- run

def cmd_run(a):
    if a.cmd == 'base':
        spec = {'hypothesis': 'base only', 'file': a.file.replace(chr(92), '/'), 'symbol': a.symbol,
                'address': int(a.address, 16), 'forms': {}, 'flags': a.flags, 'unit': a.unit}
        stem = f'base-{spec["address"]:06X}'
        variants = None
    else:
        spec = load_spec(a.variants, need_forms=not a.base)
        stem = os.path.splitext(os.path.basename(a.variants))[0]
        variants = os.path.abspath(a.variants)
        if a.base:
            spec['forms'] = {}
    return run_spec(a, spec, stem, variants)


def run_spec(a, spec, stem, variants):
    import vc6
    import libdefs
    file = spec['file']
    if not os.path.exists(os.path.join(REPO, file)):
        raise SystemExit(f'{file} is not in {REPO} (run trybatch from the tree that holds it)')

    def read(rel):
        return open(os.path.join(REPO, rel.replace('/', os.sep)), encoding='latin1', newline='').read()

    try:
        unit = compile_unit(spec, read)
    except ValueError as e:
        raise SystemExit(f'trybatch: {e}')
    moved = None
    if unit == file and not spec.get('unit'):      # a stale FILE: the function now lives in another source file
        units, marked = defining_units(spec, read)
        if (units or marked) and file not in units + marked:
            moved = (units + marked)[0]
            if spec['forms']:
                raise SystemExit(f'trybatch: {file} does not define {spec["symbol"]} (0x{spec["address"]:06X}); '
                                 f'{moved} does: point FILE there')
            file = unit = spec['file'] = moved
    flags = spec['flags'] or libdefs.flags_of(os.path.join(REPO, unit))
    batch = os.path.join(REPO, '.work', 'trybatch', f'{stem}-{time.strftime("%H%M%S")}')
    os.makedirs(batch)
    logf = open(os.path.join(batch, 'trybatch.log'), 'w', encoding='utf-8', newline=NL)
    lock = threading.Lock()

    def log(text):
        with lock:
            logf.write(text.rstrip() + NL)
            logf.flush()

    log(f'variants {variants}; file {file}; unit {unit}; flags {flags}; lib {libdefs.lib_of(os.path.join(REPO, unit))}')
    if moved:
        log(f'FILE moved: the function is defined in {moved}')
    forms = collections.OrderedDict([('base', [])])
    for name, form in spec['forms'].items():
        if not a.only or name in a.only:
            forms[name] = form

    results, todo, dlines, objs = {}, [], {}, {}
    for name, form in forms.items():
        if not re.fullmatch(r'[\w.-]+', name):
            raise SystemExit(f'form name {name!r}: letters, digits, _ . - only')
        try:
            files = apply_form(read, file, form)
        except (ValueError, OSError) as e:
            results[name] = {'error': f'edit: {e}'}
            log(f'== {name}: {e}')
            continue
        files.setdefault(unit, read(unit))
        overlay = os.path.join(batch, name)
        for rel, text in files.items():
            p = os.path.join(overlay, rel.replace('/', os.sep))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            open(p, 'w', encoding='latin1', newline='').write(text)
        dlines[name] = files[file].count(NL) - read(file).count(NL)
        todo.append(name)
    agent = os.environ.get('ENB_AGENT') or f'trybatch-{os.getpid()}'
    leased, threads = [], []
    queue = collections.deque(todo)

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
                rc, text, obj = compile_form(vc6, libdefs, overlay, unit, flags, listing)
                log(f'== {name} on {host}: rc {rc}' + NL + text.strip()[-3000:])
                if not obj:
                    results[name] = {'error': first_error(text)}
                    continue
                if listing:
                    vc6.scp(vc6.remote(listing), os.path.join(overlay, 'listing.asm'))
                    vc6.ssh(f'cmd /c del "{listing}"', check=False)
                objs[name] = obj
            except BaseException as e:      # one form's failure is a result, not the batch's end
                results[name] = {'error': f'{type(e).__name__}: {e}'[:110]}
                log(f'== {name}: ' + traceback.format_exc())

    def start(host):
        t = threading.Thread(target=worker, args=(host,))
        t.start()
        threads.append(t)

    try:
        if a.no_lease:
            hosts = list(dict.fromkeys(vc6.hosts()))[:a.vms or None]   # one thread per VM (sync.tar, the alias)
            for h in hosts:
                start(h)
        else:
            # D273: as many VMs as the pool gives (its cap and memory reserve decide), one per compile at most; each
            # starts compiling as soon as it is leased, one thread per distinct host (two on one VM collide on
            # sync.tar)
            want = a.vms or len(todo)
            hosts = []
            for got in lease_iter(want, agent, log, lambda: bool(queue)):
                leased.append(got)
                if got[1] not in hosts:
                    hosts.append(got[1])
                    start(got[1])
            if not hosts:
                raise SystemExit('trybatch: no VM leased (pool at its cap or short of memory); see ' + logf.name)
        print(f'{len(todo)} compiles on {len(hosts)} VM(s); log {os.path.relpath(logf.name, REPO)}', flush=True)
        for t in threads:
            t.join()
    finally:
        release([n for n, _ in leased], log)
    known = X.symbol_addresses()
    symbol, how = spec['symbol'], 'symbol'
    first = next((objs[n] for n in forms if n in objs), None)
    if first:
        try:
            symbol, how = resolve_symbol(first, spec['symbol'], spec['address'], texts_for(read, file, unit),
                                         libdefs.lib_of(os.path.join(REPO, unit)))
        except ValueError as e:
            for n in objs:
                results[n] = {'error': str(e)[:110]}
            objs = {}
        if how != 'symbol':
            log(f'symbol: {spec["symbol"]} -> {symbol} (by the {how})')
    for name in forms:
        if name in objs:
            try:
                results[name] = measure(objs[name], symbol, spec['address'], known)
                results[name]['dlines'] = dlines[name]
            except BaseException as e:
                results[name] = {'error': f'{type(e).__name__}: {e}'[:110]}
                log(f'== {name}: ' + traceback.format_exc())
    if a.file_check and objs:
        said = {int(m, 16) for t in texts_for(read, file, unit) for m in re.findall(r'0x([0-9A-Fa-f]{6})\b', t)}
        file_check(objs, results, log, mentioned=said)
    for name, form in forms.items():     # AP: a renamed identifier inside an assert's text is a .rdata difference
        hit = assert_renames(read, file, form)
        if hit and name in results and 'error' not in results[name]:
            results[name]['asserts'] = hit
            log(f'{name}: changes the text of an assert condition: {" | ".join(hit)}')
    regroup(results)
    order = list(forms)
    for name in order:
        m = results.get(name, {})
        log(fmt_form(name, m) if m else f'{name}: no result')
        if 'regions' in m:
            for r in m['regions']:
                log(f'   r{r["k"]} @0x{r["at"]:06X} rows {r["ours"]}/{r["client"]} res {r["res"]} regs {r["regs"]} -> {r["label"]}')
    lines, best = summary(spec, results, order)
    if how != 'symbol':
        lines.insert(1, f'symbol {X.demangle(symbol)[:70]} ({symbol[:60]}), by the {how}')
    slots = []
    if best and a.slots:
        lst = os.path.join(batch, best, 'listing.asm')
        try:
            L, R, eL, eR = results[best]['_rows']
            SL = [X.mask_stack(t, eL) for _, t in L]
            SR = [X.mask_stack(t, eR) for _, t in R]
            _, sm = X.residue(SL, SR)
            table, ins = listing_body(open(lst, encoding='latin1').read(), symbol)
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
               ] + (['--stash', variants] if variants else [])
        p = subprocess.run(cmd, capture_output=True, text=True, cwd=REPO)
        print((p.stdout + p.stderr).strip().splitlines()[-1] if (p.stdout + p.stderr).strip() else 'attempts: no output')
    logf.close()
    return {'results': results, 'order': order, 'best': best, 'symbol': symbol, 'how': how, 'file': file, 'unit': unit}


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
    r.add_argument('--vms', type=int, default=0,
                   help='at most this many VMs (default: one per compile, as many as the pool gives)')
    r.add_argument('--no-lease', action='store_true', help='use ENB_VC6_HOSTS / vc6_hosts.json as given')
    r.add_argument('--slots', action='store_true', help='also map the best form\'s frame symbols (/FAs listing)')
    r.add_argument('--record', nargs='?', const='partial', choices=('partial', 'parked', 'blocked'),
                   help='write the attempts.csv row (status, default partial)')
    r.add_argument('--only', nargs='*', help='only these forms (base is always compiled)')
    r.add_argument('--base', action='store_true', help='compile and measure the unmodified file only (no FORMS needed)')
    bs = sub.add_parser('base', help='a base-only measure without a variants file')
    bs.add_argument('file')
    bs.add_argument('symbol')
    bs.add_argument('address')
    bs.add_argument('--unit', help='the .cpp to compile when FILE is a header')
    bs.add_argument('--flags')
    for x in (r, bs):
        x.add_argument('--file-check', action='store_true',
                       help="compare every form's exact functions of the whole file with the base's (appfile.py match's set)")
    bs.add_argument('--vms', type=int, default=1)
    bs.add_argument('--no-lease', action='store_true')
    bs.set_defaults(slots=False, record=None, only=None, base=True)
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
    {'run': cmd_run, 'base': cmd_run, 'regions': cmd_regions, 'slots': cmd_slots}[a.cmd](a)


if __name__ == '__main__':
    main()
