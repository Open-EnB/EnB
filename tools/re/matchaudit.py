"""The match audit (D240): label every exact pair of the ledger by what holds it.

A pair is (library, file, address) of the ledger (docs/data/imports, a file's `in_tree` list). Classes:

  (1) pad-held        the pair's match flips somewhere in a pad sweep: tools/vc6/auxpad.py (SYMBOL_PAD, aux_roots.cpp)
                      or tools/vc6/sendpad.py (SEND_SYMBOL_PAD, sclient.cpp), 0..63. Both save every value's exact
                      address set (.work/log/auxpad-sets.json, sendpad-sets.json); a pair of the swept file exact at
                      some values and not at others is pad-held (one never exact in the lone-file compile does not
                      flip). --send-log / --aux-log add a sweep's printed table (its watched addresses only).
  (2) workaround-held the pair's file carries a known non-client device: D130 dead reads, a `__X_MEMBERS` guard it
                      defines, other code marked "not the client's", a private class view (a class the file defines that
                      another app/ file defines too), an interim forwarder (an inline body on a line marked interim).
                      Static scan: "unconfirmed". `confirm` compiles the file and a variant without the device
                      (appfile.py match, lock-free) and labels the pairs the variant loses "confirmed"; the file's other
                      pairs leave the class.
  (3) contradicted    (not built here yet: a client form not yet applied drops it, D240; after the D237 harness)
  (4) unstable        gained, lost and regained along the ledger's history (the first-parent line of HEAD with each
                      merged run branch's own first-parent commits before its merge; the old docs/data/imports.json
                      before the shards): two or more flips after the first gain.

    python tools/re/matchaudit.py pad [--aux P] [--send P] [--send-log L] [--aux-log L]   class (1)
    python tools/re/matchaudit.py workaround                         class (2), the static scan
    python tools/re/matchaudit.py confirm (LIB FILE VARIANT.cpp --device KIND | --spec JOBS.json) [--pool]
                                                                     class (2) for one file, by build
    python tools/re/matchaudit.py history [--rev REV]                class (4) from git history (~1-3 min)
    python tools/re/matchaudit.py table                              docs/data/match-audit.md and .json

Each class subcommand writes .work/matchaudit/<class>.json (the history also caches per revision under
.work/matchaudit/cache/); `table` combines whatever classes exist with HEAD's working ledger. The table is
generated: never edit it by hand. Standard library only (confirm runs appfile.py, which needs the VMs).
"""
import argparse
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'tools', 'vc6'))

WORK = os.path.join(ROOT, '.work', 'matchaudit')
OUT_MD = os.path.join(ROOT, 'docs', 'data', 'match-audit.md')
OUT_JSON = os.path.join(ROOT, 'docs', 'data', 'match-audit.json')
SHARDS = 'docs/data/imports'
OLD = 'docs/data/imports.json'
PAD_SETS = {'aux': ('app', 'aux_roots', os.path.join(ROOT, '.work', 'log', 'auxpad-sets.json'), 'SYMBOL_PAD'),
            'send': ('app', 'sclient', os.path.join(ROOT, '.work', 'log', 'sendpad-sets.json'), 'SEND_SYMBOL_PAD')}
# source roots per library (a ledger file's `name` is relative to it)
SOURCE_ROOTS = {'app': 'app', 'wlib': 'libs/wwonline/wlib', 'wnet': 'libs/wwonline/wnet'}
CLASSES = ('pad-held', 'workaround-held', 'contradicted', 'unstable')


def norm_addr(a):
    """'0x4A1B2C' for an int or any hex string."""
    return f'0x{a:X}' if isinstance(a, int) else '0x' + a[2:].upper() if a.lower().startswith('0x') else '0x' + a.upper()


def pairs_of(state):
    """{(lib, file key, address)} of a ledger dict (ledger.load's)."""
    out = set()
    for lib, block in state.items():
        if not isinstance(block, dict):
            continue
        for key, v in block.get('files', {}).items():
            for a in v.get('in_tree', []) or []:
                out.add((lib, key, norm_addr(a)))
    return out


def current_ledger():
    import ledger
    return ledger.load()


def save(name, data):
    os.makedirs(WORK, exist_ok=True)
    with open(os.path.join(WORK, name + '.json'), 'w', encoding='utf-8', newline='\n') as f:
        json.dump(data, f, indent=1, sort_keys=True)
        f.write('\n')


def load_class(name):
    p = os.path.join(WORK, name + '.json')
    return json.load(open(p, encoding='utf-8')) if os.path.isfile(p) else None


def git(*args, inp=None):
    r = subprocess.run(['git', '-C', ROOT] + list(args), input=inp, capture_output=True)
    return r.stdout.decode('utf-8', 'replace')


# ---------------------------------------------------------------- (1) pad-held

def pad_rows(sets, lib, key, ledger_pairs, pad_name):
    """Rows for the pairs of (lib, key) exact at some values of a sweep's sets ({"values": {"N": [addr]}}) and not at others.
    Sets read from a sweep's text log hold only its watched addresses ("watched"): only those are judged."""
    values = {int(n): {norm_addr(x) for x in v} for n, v in sets['values'].items()}
    watched = {norm_addr(x) for x in sets['watched']} if sets.get('watched') else None
    rows = []
    for (l, k, x) in sorted(ledger_pairs):
        if (l, k) != (lib, key) or (watched is not None and x not in watched):
            continue
        off = sorted(n for n, got in values.items() if x not in got)
        if off and len(off) < len(values):          # never exact in the sweep: its compile does not hold it, no flip
            rows.append({'lib': l, 'file': k, 'address': x, 'class': 'pad-held',
                         'evidence': f'{pad_name}: exact at {len(values) - len(off)}/{len(values)} values '
                                     f'({compress(sorted(values))}); not at {compress(off)}'
                                     + (f' [{sets["source"]}]' if sets.get('source') else '')})
    return rows


LOG_ROW = re.compile(r'^\s*(\d+)\s+\d+/\d+\s+((?:0x[0-9A-Fa-f]+[+-]\s*)+)', re.M)


def sets_from_log(text, source=''):
    """Sets from a sendpad.log / auxpad sweep's printed table (one line per value: `N  k/m  0xA+ 0xB- ...`):
    the watched addresses only."""
    values, watched = {}, set()
    for m in LOG_ROW.finditer(text):
        marks = m.group(2).split()
        watched |= {t[:-1] for t in marks}
        values[m.group(1)] = sorted(t[:-1] for t in marks if t.endswith('+'))
    return {'values': values, 'watched': sorted(watched), 'source': source}


def merge_pad_rows(rows):
    """One row per pair: the evidence of several sweeps joined."""
    out = {}
    for r in rows:
        k = (r['lib'], r['file'], r['address'])
        if k in out:
            out[k]['evidence'] += '; ' + r['evidence']
        else:
            out[k] = dict(r)
    return [out[k] for k in sorted(out)]


def compress(ns):
    """'0..3, 7, 9..12' for sorted ints."""
    runs, start, prev = [], None, None
    for n in list(ns) + [None]:
        if start is not None and (n is None or n != prev + 1):
            runs.append(f'{start}..{prev}' if prev != start else str(start))
            start = None
        if n is not None and start is None:
            start = n
        prev = n
    return ', '.join(runs)


def cmd_pad(a):
    cur = pairs_of(current_ledger())
    head = git('rev-parse', 'HEAD').strip()
    rows, sweeps = [], {}
    for which, (lib, key, path, pad) in PAD_SETS.items():
        sources = []
        path = getattr(a, which) or path
        if os.path.isfile(path):
            sets = json.load(open(path, encoding='utf-8'))
            sets['source'] = f'sets {os.path.basename(path)} at {sets.get("head", "?")[:8]}'
            sources.append(sets)
        for log in getattr(a, which + '_log') or []:
            sources.append(sets_from_log(open(log, encoding='utf-8', errors='replace').read(),
                                         'log ' + os.path.relpath(log, ROOT).replace(os.sep, '/')))
        if not sources:
            print(f'pad: no {which} sweep sets at {os.path.relpath(path, ROOT)} (run tools/vc6/'
                  f'{"auxpad.py sweep" if which == "aux" else "sendpad.py --dry"} once)')
            continue
        for sets in sources:
            n = len(sets['values'])
            r = pad_rows(sets, lib, key, cur, pad)
            sweeps.setdefault(which, []).append({'source': sets['source'], 'values': compress(sorted(map(int, sets['values']))),
                                                 'watched': len(sets.get('watched') or []) or 'all'})
            print(f'pad: {which} {sets["source"]}: {n} values, {len(r)} of '
                  f'{sum(1 for p in cur if p[:2] == (lib, key))} {key} pairs pad-held (HEAD {head[:8]})')
            rows += r
    save('pad', {'sweeps': sweeps, 'rows': merge_pad_rows(rows)})
    return 0


# ---------------------------------------------------------------- (2) workaround-held

D130 = re.compile(r'(?<![0-9A-Fa-fx_])D130\b')
NOT_CLIENT = re.compile(r"not the client'?s", re.I)
GUARD = re.compile(r'^\s*#\s*define\s+(__[A-Z0-9_]+_MEMBERS)\b', re.M)
CLASS_DEF = re.compile(r'^[ \t]*(?:template\s*<[^;{]*?>\s*)?(?:class|struct)\s+(?:__declspec\([^)]*\)\s+)?([A-Za-z_]\w*)'
                       r'\s*(?::[^;{}()]*)?\{', re.M)
INTERIM_BODY = re.compile(r'\)\s*(?:const\s*)?\{[^{}]*\}')


def strip_comments(text):
    """text with comments blanked (line structure kept), for the class-definition scan."""
    def blank(m):
        return re.sub(r'[^\n]', ' ', m.group(0))
    return re.sub(r'//[^\n]*|/\*.*?\*/', blank, text, flags=re.S)


def class_defs(text):
    """{name: line} of the classes / structs text defines (with a body)."""
    code = strip_comments(text)
    out = {}
    for m in CLASS_DEF.finditer(code):
        out.setdefault(m.group(1), code.count('\n', 0, m.start(1)) + 1)
    return out


def scan_devices(text, others=None, path=''):
    """[(kind, line, detail)] of the non-client devices in one source file's text. others: {class name: [paths]}
    of the classes defined in the other files of the index (the private class view test)."""
    out = []
    lines = text.splitlines()
    for i, l in enumerate(lines, 1):
        if D130.search(l):
            out.append(('d130', i, l.strip()[:80]))
        elif NOT_CLIENT.search(l) and 'symbol pad' not in l.lower():
            out.append(('not-client', i, l.strip()[:80]))
        if re.search(r'interim', l, re.I) and INTERIM_BODY.search(l.split('//')[0]):
            out.append(('interim-forwarder', i, l.strip()[:80]))
    for m in GUARD.finditer(text):
        out.append(('guard', text.count('\n', 0, m.start()) + 1, m.group(1)))
    if others is not None:
        for name, line in class_defs(text).items():
            elsewhere = [p for p in others.get(name, []) if p != path]
            if elsewhere:
                out.append(('class-view', line, f'{name} (also {", ".join(sorted(elsewhere)[:2])})'))
    return sorted(out, key=lambda d: (d[1], d[0]))


def source_path(lib, entry):
    """The repo-relative source path of a ledger file entry, or None."""
    name = entry.get('name')
    if not name:
        return None
    root = SOURCE_ROOTS.get(lib, lib)
    p = root + '/' + name.replace('\\', '/')
    return p if os.path.isfile(os.path.join(ROOT, p)) else None


def class_index():
    """{class name: [repo-relative paths under app/ that define it]}."""
    idx = {}
    for d, _, names in os.walk(os.path.join(ROOT, 'app')):
        for n in names:
            if n.lower().endswith(('.h', '.cpp', '.inl')):
                p = os.path.join(d, n)
                rel = os.path.relpath(p, ROOT).replace(os.sep, '/')
                for c in class_defs(open(p, encoding='latin-1').read()):
                    idx.setdefault(c, []).append(rel)
    return idx


def workaround_rows(state, idx, read=None):
    """(rows, files): every pair of a file with devices, labelled workaround-held (unconfirmed)."""
    read = read or (lambda p: open(os.path.join(ROOT, p), encoding='latin-1').read())
    rows, files = [], []
    for lib, block in sorted(state.items()):
        for key, entry in sorted(block.get('files', {}).items()):
            addrs = [norm_addr(x) for x in entry.get('in_tree', []) or []]
            path = source_path(lib, entry)
            if not addrs or not path:
                continue
            devs = scan_devices(read(path), idx if lib == 'app' else None, path)
            if not devs:
                continue
            kinds = {}
            for k, line, _ in devs:
                kinds.setdefault(k, []).append(line)
            ev = '; '.join(f'{k} x{len(v)} (line {", ".join(map(str, v[:4]))}{", ..." if len(v) > 4 else ""})'
                           for k, v in sorted(kinds.items()))
            files.append({'lib': lib, 'file': key, 'path': path, 'pairs': len(addrs),
                          'devices': [{'kind': k, 'line': n, 'detail': d} for k, n, d in devs]})
            rows += [{'lib': lib, 'file': key, 'address': x, 'class': 'workaround-held',
                      'evidence': 'unconfirmed: ' + ev} for x in sorted(addrs)]
    return rows, files


def cmd_workaround(a):
    rows, files = workaround_rows(current_ledger(), class_index())
    save('workaround', {'rows': rows, 'files': files})
    kinds = {}
    for f in files:
        for d in {d['kind'] for d in f['devices']}:
            kinds[d] = kinds.get(d, 0) + 1
    print(f'workaround: {len(files)} files with devices hold {len(rows)} pairs; files per device: '
          + ', '.join(f'{k} {n}' for k, n in sorted(kinds.items())))
    return 0


EXACT_LINE = re.compile(r'^\s+EXACT\s.*?\b(0x[0-9A-Fa-f]{5,8})\b')
NOMARK_LINE = re.compile(r'^\s+exact\s+.*\(no marker\)\s+(0x[0-9A-Fa-f]{5,8})')


def exact_from_match(text):
    """The addresses appfile.py match reports exact (at a marker, or a markerless function's single hit)."""
    out = set()
    for l in text.splitlines():
        m = EXACT_LINE.match(l) or NOMARK_LINE.match(l)
        if m:
            out.add(norm_addr(m.group(1)))
    return out


def run_match(path):
    cmd = [sys.executable, os.path.join(HERE, 'appfile.py'), 'match', path, '--allow-missing',
           '-I', 'app/client', '-I', 'app/shared', '-I', 'auto']
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, encoding='utf-8', errors='replace')
    return r.returncode, r.stdout + r.stderr


def confirm_one(state, lib, file, variant, device, note=''):
    """Compile (lib, file) and its variant without the device; record the pairs only the original keeps."""
    import shutil
    entry = state.get(lib, {}).get('files', {}).get(file)
    if not entry or not source_path(lib, entry):
        print(f'confirm: no ledger file {lib}/{file} with a source')
        return
    src = source_path(lib, entry)
    d = os.path.join(WORK, 'confirm', file)
    os.makedirs(d, exist_ok=True)
    base = os.path.join(d, os.path.basename(src))
    shutil.copyfile(os.path.join(ROOT, src), base)
    var = os.path.join(d, 'variant_' + os.path.basename(src))
    if os.path.abspath(variant) != os.path.abspath(var):
        shutil.copyfile(variant, var)
    got = {}
    for tag, p in (('base', base), ('variant', var)):
        rc, text = run_match(p)
        with open(os.path.join(d, tag + '.log'), 'w', encoding='utf-8') as f:
            f.write(text)
        if rc:
            print(f'confirm {lib}/{file}: {tag} compile failed (rc {rc}), log {os.path.relpath(d, ROOT)}/{tag}.log')
            return
        got[tag] = exact_from_match(text)
    held = {norm_addr(x) for x in entry.get('in_tree', [])}
    lost = sorted((got['base'] - got['variant']) & held)
    data = load_class('confirm') or {}
    data[f'{lib}/{file}'] = {'device': device, 'lost': lost, 'base_exact': len(got['base'] & held),
                             'variant_exact': len(got['variant'] & held), 'ledger': len(held), 'note': note or ''}
    save('confirm', data)
    print(f'confirm {lib}/{file} without {device}: base {len(got["base"] & held)}/{len(held)} exact, variant '
          f'{len(got["variant"] & held)}; held by the device: {" ".join(lost) or "none"}', flush=True)


def cmd_confirm(a):
    import contextlib
    if a.spec:
        jobs = json.load(open(a.spec, encoding='utf-8'))
    elif a.lib and a.file and a.variant and a.device:
        jobs = [{'lib': a.lib, 'file': a.file, 'variant': a.variant, 'device': a.device, 'note': a.note or ''}]
    else:
        raise SystemExit('confirm: LIB FILE VARIANT --device KIND, or --spec FILE.json')
    state = current_ledger()
    if a.pool:
        import vc6lock
        lease = vc6lock.pool_lease(1)
    else:
        lease = contextlib.nullcontext()
    with lease:
        for j in jobs:
            confirm_one(state, j['lib'], j['file'], j['variant'], j['device'], j.get('note', ''))
    return 0


# ---------------------------------------------------------------- (4) unstable

def linear_revisions(rev='HEAD'):
    """The ledger's line of history, oldest first: HEAD's first-parent commits, each merge preceded by its second
    parent's own first-parent commits since the first parent (a run branch's history)."""
    main = git('rev-list', '--first-parent', '--reverse', '--parents', rev).split('\n')
    out = []
    for line in main:
        parts = line.split()
        if not parts:
            continue
        if len(parts) > 2:
            side = git('rev-list', '--first-parent', '--reverse', parts[2], '^' + parts[1]).split()
            out += side
        out.append(parts[0])
    return out


ZERO = '0' * 40


def first_parents(rev='HEAD'):
    """{commit: its first parent, or None for a root}."""
    out = {}
    for line in git('rev-list', '--parents', rev).splitlines():
        parts = line.split()
        if parts:
            out[parts[0]] = parts[1] if len(parts) > 1 else None
    return out


def ledger_diffs(revs, parents):
    """[(rev, [(path, old blob, new blob)])]: each revision's ledger changes against its own first parent."""
    q = ''.join(f'{r} {parents[r]}\n' if parents.get(r) else f'{r}\n' for r in revs).encode()
    text = git('diff-tree', '--stdin', '-r', '--root', '--', SHARDS, OLD, inp=q)
    out = []
    for line in text.splitlines():
        if not line.startswith(':'):
            if line.strip():
                out.append((line.split()[0], []))
            continue
        meta, _, path = line.partition('\t')
        f = meta.split()
        out[-1][1].append((path, f[2], f[3]))
    return out


def cat_blobs(ids):
    """{id: text} for blob ids, one git cat-file --batch."""
    if not ids:
        return {}
    data = subprocess.run(['git', '-C', ROOT, 'cat-file', '--batch'], input=''.join(i + '\n' for i in ids).encode(),
                          capture_output=True).stdout
    out, i = {}, 0
    for oid in ids:
        nl = data.index(b'\n', i)
        size = int(data[i:nl].split()[2])
        out[oid] = data[nl + 1:nl + 1 + size].decode('utf-8', 'replace')
        i = nl + 1 + size + 1
    return out


def shard_pairs(rel, text):
    """{(lib, key, addr)} of one shard (rel: path below docs/data/imports); None when it is not valid JSON."""
    parts = rel.split('/')
    if len(parts) != 2 or not rel.endswith('.json') or parts[1] == 'library.json':
        return frozenset()
    try:
        obj = json.loads(text)
    except ValueError:
        return None
    if not isinstance(obj, dict):
        return None
    return frozenset((parts[0], parts[1][:-5], norm_addr(a)) for a in obj.get('in_tree', []) or [])


def old_pairs(text):
    """The pairs of an old docs/data/imports.json; None when it is not valid JSON (it was, at times)."""
    try:
        state = json.loads(text)
    except ValueError:
        return None
    return frozenset(pairs_of(state)) if isinstance(state, dict) else None


def scope_of(path):
    """The pairs a ledger path can hold: None (all of them) for the old single file, (lib, key) for a shard."""
    if path == OLD:
        return None
    parts = path[len(SHARDS) + 1:].split('/')
    return (parts[0], parts[1][:-5]) if len(parts) == 2 else ('', '')


def history_changes(diffs):
    """Yield (rev, [(scope, before, after)]) per revision with a ledger diff: each changed path's pairs before and
    after (frozensets; empty for an added / deleted path; None for a file that is not valid JSON)."""
    names = {}
    for _, ch in diffs:
        for p, x, y in ch:
            if p != OLD:
                for oid in (x, y):
                    if oid != ZERO:
                        names.setdefault(oid, p)
    cache = {}
    ids = sorted(names)
    for i in range(0, len(ids), 2000):
        for oid, text in cat_blobs(ids[i:i + 2000]).items():
            cache[oid] = shard_pairs(names[oid][len(SHARDS) + 1:], text)
    old_cache = {}

    def pairs(path, oid):
        if oid == ZERO:
            return frozenset()
        if path != OLD:
            return cache.get(oid, frozenset())
        if oid not in old_cache:
            if len(old_cache) > 4:
                old_cache.pop(next(iter(old_cache)))
            old_cache[oid] = old_pairs(cat_blobs([oid])[oid])
        return old_cache[oid]
    for rev, ch in diffs:
        if ch:
            yield rev, [(scope_of(p), pairs(p, x), pairs(p, y)) for p, x, y in ch]


def flips(changes):
    """{pair: [(rev, '+'|'-')...]}: each pair's transitions in a running state each revision's own changes are applied
    to, from its first gain on. Net over a revision's paths (a pair moving between shards, or the split into shards,
    is no change); a merge re-applying its branch's changes is no transition. A revision whose new file is not valid
    JSON is skipped; an invalid old file stands for the running state's pairs of its scope."""
    out, state = {}, set()
    for rev, ch in changes:
        if any(after is None for _, _, after in ch):
            continue
        before, after = set(), set()
        for scope, b, a in ch:
            if b is None:
                b = state if scope is None else {p for p in state if p[:2] == scope}
            before |= b
            after |= a
        gained, lost = after - before, before - after
        for p in gained - state:
            out.setdefault(p, []).append((rev, '+'))
        for p in lost & state:
            out[p].append((rev, '-'))
        state = (state | gained) - lost
    return out


def unstable_rows(trans, current, short=lambda r: r[:8]):
    rows = []
    for p in sorted(current):
        t = trans.get(p, [])
        n = len(t) - 1            # flips after the first gain
        if n >= 2:
            rows.append({'lib': p[0], 'file': p[1], 'address': p[2], 'class': 'unstable', 'flips': n,
                         'evidence': f'{n} flips: ' + ' '.join(s + short(r) for r, s in t[:8])
                                     + (' ...' if len(t) > 8 else '')})
    return rows


def cmd_history(a):
    revs = linear_revisions(a.rev)
    diffs = ledger_diffs(revs, first_parents(a.rev))
    n = [0]

    def counted():
        for x in history_changes(diffs):
            n[0] += 1
            yield x
    trans = flips(counted())
    cur = pairs_of(current_ledger())
    rows = unstable_rows(trans, cur)
    save('history', {'rev': git('rev-parse', a.rev).strip(), 'revisions': len(revs), 'states': n[0], 'rows': rows})
    print(f'history: {len(revs)} revisions, {n[0]} ledger changes; {len(rows)} of {len(cur)} current pairs '
          f'flipped twice or more')
    return 0


# ---------------------------------------------------------------- the table

def combine(current, pad=None, work=None, confirm=None, hist=None):
    """(rows, summary): the labelled rows of the current pairs, and per class {count, share}."""
    rows = []
    rows += [r for r in (pad or {}).get('rows', []) if (r['lib'], r['file'], r['address']) in current]
    confirm = confirm or {}
    for r in (work or {}).get('rows', []):
        key = (r['lib'], r['file'], r['address'])
        if key not in current:
            continue
        c = confirm.get(f"{r['lib']}/{r['file']}")
        if c is None:
            rows.append(r)
        elif r['address'] in c['lost']:
            rows.append(dict(r, evidence=f"confirmed by build: lost without {c['device']}"
                                         + (f" ({c['note']})" if c.get('note') else '')))
    rows += [r for r in (hist or {}).get('rows', []) if (r['lib'], r['file'], r['address']) in current]
    rows.sort(key=lambda r: (CLASSES.index(r['class']), r['lib'], r['file'], r['address']))
    total = len(current)
    summary = {'pairs': total, 'classes': {}}
    for c in CLASSES:
        n = len({(r['lib'], r['file'], r['address']) for r in rows if r['class'] == c})
        summary['classes'][c] = {'count': n, 'share': round(n / total, 4) if total else 0.0}
    union = len({(r['lib'], r['file'], r['address']) for r in rows})
    summary['not_stable'] = {'count': union, 'share': round(union / total, 4) if total else 0.0}
    return rows, summary


def render(rows, summary, sources):
    pct = lambda x: f'{100 * x:.1f}%'
    out = ['# Match audit (D240)', '',
           'Generated by `tools/re/matchaudit.py table` (never edit by hand). Every exact pair of the ledger '
           '(docs/data/imports) labelled by what holds it; the classes are in the tool\'s docstring. '
           'A pair may carry several classes.', '',
           f"Exact pairs: {summary['pairs']}. Not stable (any class): {summary['not_stable']['count']} "
           f"({pct(summary['not_stable']['share'])}).", '',
           '| Class | Pairs | Share of exact pairs | Source |', '|---|---|---|---|']
    for c in CLASSES:
        s = summary['classes'][c]
        out.append(f"| {c} | {s['count']} | {pct(s['share'])} | {sources.get(c, 'not built')} |")
    out += ['']
    for c in CLASSES:
        rs = [r for r in rows if r['class'] == c]
        if not rs:
            continue
        out += [f'## {c} ({len(rs)})', '']
        each = [r for r in rs if not r['evidence'].startswith('unconfirmed')]
        grouped = {}
        for r in rs:
            if r['evidence'].startswith('unconfirmed'):
                grouped.setdefault((r['lib'], r['file'], r['evidence']), []).append(r['address'])
        if grouped:
            out += ['Unconfirmed (every pair of the file; the addresses are in match-audit.json), files by pairs held:', '',
                    '| Library | File | Pairs | Evidence |', '|---|---|---|---|']
            out += [f"| {k[0]} | {k[1]} | {len(v)} | {k[2].replace('|', '/')} |"
                    for k, v in sorted(grouped.items(), key=lambda kv: (-len(kv[1]), kv[0]))]
            out.append('')
        if each:
            out += ['| Library | File | Address | Evidence |', '|---|---|---|---|']
            out += [f"| {r['lib']} | {r['file']} | {r['address']} | {r['evidence'].replace('|', '/')} |" for r in each]
            out.append('')
    return '\n'.join(out)


def cmd_table(a):
    cur = pairs_of(current_ledger())
    pad, work, conf, hist = (load_class(n) for n in ('pad', 'workaround', 'confirm', 'history'))
    rows, summary = combine(cur, pad, work, conf, hist)
    sources = {}
    if pad:
        sources['pad-held'] = '; '.join(f"{w}: {s['source']}, values {s['values']}, watched {s['watched']}"
                                        for w, ss in sorted(pad.get('sweeps', {}).items()) for s in ss) or 'no sweep sets'
    if work:
        held = sum(len(c['lost']) for c in (conf or {}).values())
        of = sum(c.get('ledger', 0) for c in (conf or {}).values())
        sources['workaround-held'] = (f"static scan, {len(work['files'])} files; {len(conf or {})} confirmed by build "
                                      f"(the device holds {held} of their {of} pairs)")
    if hist:
        sources['unstable'] = f"history to {hist['rev'][:8]}, {hist['states']} ledger states"
    md = render(rows, summary, sources)
    with open(OUT_MD, 'w', encoding='utf-8', newline='\n') as f:
        f.write(md + '\n')
    with open(OUT_JSON, 'w', encoding='utf-8', newline='\n') as f:
        head = json.dumps({'summary': summary, 'sources': sources}, indent=1, sort_keys=True)
        f.write(head[:-2] + ',\n "rows": [\n')      # one row per line: small diffs
        f.write(',\n'.join('  ' + json.dumps(r, sort_keys=True) for r in rows))
        f.write('\n ]\n}\n')
    print(f"table: {summary['pairs']} pairs; " + ', '.join(f"{c} {s['count']}" for c, s in summary['classes'].items())
          + f"; not stable {summary['not_stable']['count']} ({100 * summary['not_stable']['share']:.1f}%)")
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('pad')
    p.add_argument('--aux', help='auxpad.py sweep sets (default .work/log/auxpad-sets.json)')
    p.add_argument('--send', help='sendpad.py sweep sets (default .work/log/sendpad-sets.json)')
    p.add_argument('--aux-log', action='append', help="an auxpad sweep's printed table (watched addresses only)")
    p.add_argument('--send-log', action='append', help='a sendpad.log (watched addresses only)')
    sub.add_parser('workaround')
    c = sub.add_parser('confirm')
    c.add_argument('lib', nargs='?')
    c.add_argument('file', nargs='?', help="the ledger's file key (e.g. ui_starmap)")
    c.add_argument('variant', nargs='?', help='the file without the device (a copy you made)')
    c.add_argument('--device')
    c.add_argument('--note')
    c.add_argument('--spec', help='a JSON list of {lib, file, variant, device, note}: several files, one lease')
    c.add_argument('--pool', action='store_true', help='lease one VM from pool.py for the whole call')
    h = sub.add_parser('history')
    h.add_argument('--rev', default='HEAD')
    sub.add_parser('table')
    a = ap.parse_args(argv)
    return {'pad': cmd_pad, 'workaround': cmd_workaround, 'confirm': cmd_confirm, 'history': cmd_history,
            'table': cmd_table}[a.cmd](a)


if __name__ == '__main__':
    raise SystemExit(main())
