"""Per-file workflow for E&B's own code (docs/app-plan.md section 2): window -> draft -> match.

    python appfile.py show FILE                 the file's window: every function, its size, callees,
                                                strings and asserts (with their __LINE__)
    python appfile.py draft FILE [--out DIR]    also write DIR/<name>.cpp: one stub per function with
                                                all of the above and Ghidra's saved pseudocode
    python appfile.py match DRAFT.cpp [-I DIR|auto]... [--pool] [--flags F] [--diff NAME]
                                                compile the draft with VC6 (tools/vc6/vc6.py) and
                                                match every function against its `// @0xADDR` marker

FILE is the source path as the client's __FILE__ strings give it, below e:\\build\\beta\\G
(`app\\shared\\auxdata\\bitset.cpp`; `/` works too). The window is the functions app_inventory.py
attributes to FILE (docs/data/app-functions.json: `assert` = the function references the file's
__FILE__, `between` = it lies between two that do), plus unattributed neighbours before and after
(marked `?`: they may belong to the next object). Sizes run to the next verified function start,
trailing int3/nop padding dropped.

Asserts are E&B's assert(): `push __LINE__; push __FILE__; push "expr"; call _assert`, so each one
gives a source line of FILE. Callees are named as client.py names them (symbols.json, gaps reports,
thunks followed) and otherwise by app-functions.json (`name`, or `[file]` when attributed).

`match` compiles with the app's presumed flags (/MT /O2 /Ob2 /GX, docs/app-plan.md section 1) and
with NDEBUG (D61: E&B's own asserts, G_ASSERT/FAIL_IF, don't test it). In the draft, put `// @0xADDR` on the line
before a function's definition; each compiled function whose demangled name is that definition's
is reported EXACT when its bytes (relocations masked, as objmatch.py) are the client's at ADDR,
otherwise with its objmatch status and objdiff similarity to ADDR. `--diff
NAME` prints the full objdiff of one function.

Include directories (D112(5)): with no -I (or `-I auto`, which may be mixed with explicit ones) the
draft's #includes are resolved recursively as VC6 resolves them, quoted ones from the including
file's folder first, against every app/ folder with a tracked header, importer.py's tree_includes
(libraries, SDK_INCLUDES), the libs/ folders and the wwlib-style top-level folders; the folders actually used
go to cl as -I in that order and every folder a resolved header sits in is sent to the VM (a stale
VM copy is what the long hand-written lists were for). `--pool` leases one VM from pool.py under
$ENB_AGENT and releases it after; otherwise ENB_VC6_HOSTS / vc6_hosts.json choose the VM. Objects
land in .work/<$ENB_AGENT or appfile>/appfile/obj, never in the draft's folder. A stale result is
refused: the draft changed during the compile, the object is older than the draft, or it lacks a
non-inline function the draft marks (after one retry with the VM's sync cache reset;
--allow-missing reports anyway). `draft` and `match` run only from inside their own tree (rootguard).
"""
import argparse
import bisect
import difflib
import json
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', 'vc6'))
from clientimage import ClientImage  # noqa: E402
from client import DATA, Names, section_of  # noqa: E402
import rootguard  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
PSEUDO = os.path.join(ROOT, '.work', 'pseudo')
SOURCE = re.compile(r'^e:\\build\\beta\\G\\(.+\.(cpp|h|inl))$', re.I)
APP_FLAGS = '/nologo /c /MT /W3 /O2 /Ob2 /GX'
IMM = re.compile(r'0x[0-9a-f]{5,8}')


def norm_file(path):
    return path.replace('/', '\\').lower().strip('\\')


class App:
    def __init__(self):
        self.img = ClientImage()
        self.names = Names(self.img)
        self.starts = self.img.function_starts()[0]
        d = json.load(open(os.path.join(DATA, 'app-functions.json')))
        self.funcs = {int(r['address'], 16): r for r in d['functions']}
        self.order = sorted(self.funcs)

    def size(self, va):
        i = bisect.bisect_right(self.starts, va)
        end = self.starts[i] if i < len(self.starts) else va + 0x1000
        o = self.img.offset(va)
        body = self.img.data[o:o + (end - va)]
        n = len(body)
        while n and body[n - 1] in (0xCC, 0x90):
            n -= 1
        return n

    def window(self, fname, neighbours=3):
        """[(address, evidence)] of FILE's functions, with up to `neighbours` unattributed `?` ones
        on each side."""
        own = [a for a in self.order if self.funcs[a].get('file') == fname]
        if not own:
            return []
        out = [(a, self.funcs[a]['file_evidence']) for a in own]
        lo, hi = self.order.index(own[0]), self.order.index(own[-1])
        for step, i in ((-1, lo - 1), (1, hi + 1)):
            n = 0
            while 0 <= i < len(self.order) and 'file' not in self.funcs[self.order[i]] and n < neighbours:
                n += 1
                out.append((self.order[i], '?'))
                i += step
        return sorted(out)

    def is_vtable(self, va):
        if section_of(self.img, va) != '.rdata' or self.img.offset(va) is None:
            return False
        return section_of(self.img, self.img.u32(va)) == '.text'

    def string_at(self, va):
        if section_of(self.img, va) not in ('.rdata', '.data') or self.img.offset(va) is None:
            return None
        if self.is_vtable(va):
            return None
        s = self.img.cstr(va, 200)
        if s and len(s) >= 2 and all(32 <= ord(c) < 127 or c in '\t\r\n' for c in s):
            return s
        return None

    def name(self, va):
        n = self.names.of(va)
        target = self.names.thunks.get(va, va)
        if n and not re.fullmatch(r'j_0x[0-9A-F]+', n):
            return n
        r = self.funcs.get(target)
        if r:
            if r.get('name'):
                return r['name']
            if r.get('file'):
                return f'0x{target:X} [{r["file"].rsplit(chr(92), 1)[-1]}]'
        return f'0x{target:X}'

    def analyse(self, va):
        size = self.size(va)
        ins = list(self.img.disasm(va, size))
        callees, strings, asserts, vtables = [], [], [], []
        for k, i in enumerate(ins):
            if i.mnemonic == 'call':
                m = IMM.fullmatch(i.op_str)
                if m:
                    callees.append(self.name(int(m.group(0), 16)))
                continue
            for m in IMM.findall(i.op_str):
                if self.is_vtable(int(m, 16)):
                    vtables.append(f'0x{int(m, 16):X}')
                    continue
                s = self.string_at(int(m, 16))
                if s is None:
                    continue
                if SOURCE.match(s) and i.mnemonic == 'push':
                    line = expr = None
                    if k and ins[k - 1].mnemonic == 'push' and re.fullmatch(r'0x[0-9a-f]+|\d+', ins[k - 1].op_str):
                        line = int(ins[k - 1].op_str, 0)
                    if k + 1 < len(ins) and ins[k + 1].mnemonic == 'push':
                        e = IMM.fullmatch(ins[k + 1].op_str)
                        expr = self.string_at(int(e.group(0), 16)) if e else None
                    asserts.append((line, expr, norm_file(SOURCE.match(s).group(1))))
                elif not SOURCE.match(s) and not any(s == a[1] for a in asserts[-1:]):
                    strings.append(s)
        # an assert's expression is pushed after the file: drop it from the plain strings
        exprs = {a[1] for a in asserts}
        strings = [s for s in strings if s not in exprs]
        return {'size': size, 'callees': list(dict.fromkeys(callees)), 'strings': list(dict.fromkeys(strings)),
                'asserts': asserts, 'vtables': list(dict.fromkeys(vtables))}


def describe(app, fname, neighbours=3):
    rows = []
    for va, ev in app.window(fname, neighbours):
        r = app.funcs[va]
        info = app.analyse(va)
        info.update(address=va, evidence=ev, name=r.get('name') or app.names.of(va),
                    names=r.get('names'), file=r.get('file'))
        rows.append(info)
    return rows


def print_rows(rows, fname):
    print(f'{fname}: {len(rows)} functions '
          f'({sum(r["evidence"] == "assert" for r in rows)} assert, {sum(r["evidence"] == "between" for r in rows)} between, '
          f'{sum(r["evidence"] == "?" for r in rows)} unattributed neighbours), '
          f'{sum(r["size"] for r in rows if r["evidence"] != "?")} bytes, '
          f'{sum(len(r["asserts"]) for r in rows)} asserts')
    for r in rows:
        print(f'\n0x{r["address"]:06X} size 0x{r["size"]:X} [{r["evidence"]}]' +
              (f' {r["name"]}' if r['name'] else '') + (f' (names: {", ".join(r["names"])})' if r.get('names') else '') +
              (f' (file {r["file"]})' if r['evidence'] == '?' and r.get('file') else ''))
        for line, expr, f in r['asserts']:
            other = '' if f == r.get('file') else f' ({f})'
            print(f'  assert  line {line}: {expr!r}{other}')
        for s in r['strings']:
            print(f'  string  {s!r}')
        if r['vtables']:
            print(f'  vtables {", ".join(r["vtables"])}')
        if r['callees']:
            print(f'  calls   {", ".join(r["callees"])}')


def pseudocode(va):
    path = os.path.join(PSEUDO, f'{va:08X}.c')
    if os.path.exists(path):
        return open(path, encoding='utf-8').read().rstrip()
    return f'not decompiled yet (python tools/re/pseudocode.py 0x{va:X})'


def write_draft(rows, fname, out):
    os.makedirs(out, exist_ok=True)
    path = os.path.join(out, fname.rsplit('\\', 1)[-1])
    lines = [f'// Draft of {fname} (tools/re/appfile.py draft): one stub per function of the client\'s window.',
             '// Put the C++ under each `// @0xADDR` marker (the marker names the function for `appfile.py match`).',
             '#include <assert.h>', '']
    for r in rows:
        lines.append(f'// ---- 0x{r["address"]:06X} size 0x{r["size"]:X} [{r["evidence"]}]' + (f' {r["name"]}' if r['name'] else ''))
        for line, expr, f in r['asserts']:
            lines.append(f'// assert line {line}: {expr}' + ('' if f == r.get('file') else f' ({f})'))
        for s in r['strings']:
            lines.append(f'// string {s!r}')
        if r['vtables']:
            lines.append(f'// vtables {", ".join(r["vtables"])}')
        if r['callees']:
            lines.append(f'// calls {", ".join(r["callees"])}')
        lines.append('/*')
        lines += [l.replace('*/', '* /') for l in pseudocode(r['address']).splitlines()]
        lines.append('*/')
        lines.append(f'// @0x{r["address"]:06X}')
        lines.append('')
    open(path, 'w', encoding='utf-8', newline='\n').write('\n'.join(lines) + '\n')
    return path


MARKER = re.compile(r'^\s*//\s*@(0x[0-9A-Fa-f]+)\s*$')
DEFN = re.compile(r'([A-Za-z_][\w]*(?:::~?[A-Za-z_]\w*|::operator\s*\S+?)*)\s*\(')


def markers(path, inline=None):
    """{qualified name: [address]} from `// @0xADDR` lines followed by a definition; also the
    number of markers (filled or not). `inline` (a set, optional) receives the names whose
    definition is inline or a template (the compiler may leave them out of the object)."""
    out = {}
    lines = open(path, encoding='latin1').read().splitlines()
    for k, l in enumerate(lines):
        m = MARKER.match(l)
        if not m:
            continue
        for j, nxt in enumerate(lines[k + 1:k + 6]):
            if not nxt.strip() or nxt.lstrip().startswith('//'):
                continue
            d = DEFN.search(nxt)
            if d:
                name = d.group(1).replace(' ', '')
                out.setdefault(name, []).append(int(m.group(1), 16))
                if inline is not None and re.search(r'\b(inline|__inline|__forceinline|template)\b',
                                                    nxt + ' ' + lines[k + j]):
                    inline.add(name)
            break
    return out, sum(1 for l in lines if MARKER.match(l))


# ------------------------------------------------------------------ match: -I auto (D112(5))

INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*([<"])([^">\r\n]+)[">]', re.M)
NOT_LIBRARY = {'.git', '.work', 'build', 'docs', 'tools', 'tests', 'programs', 'stubs'}


def candidate_dirs():
    """-I auto's search list, in order: every app/ folder holding a tracked header (a draft away from
    its home folder finds app/packets/pk.h before wwlib/pk.h, as the home folder would), the app
    build's own list (importer.tree_includes: the libraries, importer.SDK_INCLUDES, libs/wwonline,
    libs/sgistl), the libs/ folders holding tracked headers, other top-level folders holding headers
    (wwlib style), the repository root last."""
    import subprocess
    import importer
    base = [os.path.normpath(d) for d in importer.tree_includes()]
    repo = os.path.normpath(importer.REPO)
    tracked = subprocess.run(['git', '-C', repo, 'ls-files', '--', 'app', 'libs'], capture_output=True,
                             text=True).stdout.split('\n')
    extra = sorted({os.path.normpath(os.path.join(repo, os.path.dirname(f))) for f in tracked
                    if f.lower().endswith(('.h', '.hpp', '.inl'))})
    tops = sorted(os.path.join(repo, d) for d in os.listdir(repo)
                  if d not in NOT_LIBRARY and os.path.isdir(os.path.join(repo, d))
                  and any(f.lower().endswith('.h') for f in os.listdir(os.path.join(repo, d))))
    out = []
    app = [d for d in extra if os.path.relpath(d, repo).lower().startswith('app')]
    for d in app + [b for b in base if b != repo] + extra + tops + [repo]:
        if os.path.normcase(d) not in {os.path.normcase(x) for x in out}:
            out.append(d)
    return out


def auto_includes(draft, cands=None):
    """Resolve the draft's #includes recursively as VC6 does (quoted: the including file's folder,
    then the folders of the files including it, innermost first, then the -I list; angle: the -I
    list) against candidate_dirs(). Returns (the candidates used, in their order; the other
    folders the resolved headers sit in, to send to the VM; the headers resolved by name).
    Conditional compilation is ignored: every #include counts (a superset)."""
    cands = cands or candidate_dirs()
    key = os.path.normcase
    used, found_dirs, seen, names = set(), set(), set(), 0
    stack = [(os.path.abspath(draft), ())]
    while stack:
        path, chain = stack.pop()
        if key(path) in seen:
            continue
        seen.add(key(path))
        try:
            text = open(path, encoding='latin1').read()
        except OSError:
            continue
        here = os.path.dirname(path)
        for kind, name in INCLUDE.findall(text):
            name = name.strip().replace('/', os.sep)
            local = [here] + list(reversed(chain)) if kind == '"' else []
            hit = None
            for d in local + cands:
                p = os.path.normpath(os.path.join(d, name))
                if os.path.isfile(p):
                    hit = p
                    if d not in local:
                        used.add(key(d))
                    break
            if hit:
                names += 1
                found_dirs.add(os.path.dirname(hit))
                stack.append((hit, chain + (here,)))
    inc = [d for d in cands if key(d) in used]
    sync = sorted(d for d in found_dirs if key(d) not in used)
    return inc, sync, names


def agent_name():
    return os.environ.get('ENB_AGENT') or 'appfile'


def lease_vm():
    """--pool: lease one VM from pool.py under $ENB_AGENT (else appfile-<pid>) and point vc6 at
    it; returns the VM names to release (only the one leased here, never the agent's others)."""
    import subprocess
    pool = [sys.executable, os.path.join(ROOT, 'tools', 'vc6', 'pool.py')]
    agent = os.environ.get('ENB_AGENT') or f'appfile-{os.getpid()}'
    p = subprocess.run(pool + ['lease', '--agent', agent, '--count', '1'], capture_output=True, text=True)
    sys.stderr.write(p.stderr)
    names = re.findall(r'^(\S+): \S+ ready', p.stderr, re.M)
    line = [l for l in p.stdout.splitlines() if l.startswith('ENB_VC6_HOSTS=')]
    if p.returncode or not line:
        release_vm(names)
        raise SystemExit(f'pool.py lease failed (exit {p.returncode}): {p.stdout.strip()}')
    os.environ['ENB_VC6_HOSTS'] = line[-1].split('=', 1)[1]
    print(f'pool: {", ".join(names)} {os.environ["ENB_VC6_HOSTS"]} for {agent}', flush=True)
    return names


def release_vm(names):
    import subprocess
    if names:
        p = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'vc6', 'pool.py'), 'release'] + names,
                           capture_output=True, text=True)
        print(f'pool: {p.stdout.strip()}', flush=True)


def match(a):
    names = lease_vm() if a.pool else []
    try:
        return _match(a)
    finally:
        release_vm(names)


def compile_draft(a, includes, sync, out):
    import vc6
    shutil.rmtree(out, ignore_errors=True)
    os.makedirs(out)
    defines = list(vc6.DEFAULT_DEFINES) + ['ENABLE_WWPROFILE'] + a.D  # with NDEBUG (D61) and ENABLE_WWPROFILE (D74), as APP_LIBS
    before = os.path.getmtime(a.draft)
    rc, log = vc6.compile_sources([a.draft], includes, defines, a.flags, out, extra_sync=sync)
    objs = [os.path.join(out, f) for f in os.listdir(out) if f.lower().endswith('.obj')]
    return rc, log, objs, before


def _match(a):
    import vc6
    from objmatch import CoffObject, match_objects
    import objdiff
    a.draft = os.path.abspath(a.draft)
    if not a.I or a.I == ['auto']:
        includes, sync, n = auto_includes(a.draft)
        print(f'-I auto: {n} headers from {len(includes)} directories: '
              + ' '.join(os.path.relpath(d, ROOT) if not os.path.relpath(d, ROOT).startswith('..') else d
                         for d in includes))
    else:
        includes, sync = [d for d in a.I if d != 'auto'], []
        if 'auto' in a.I:
            more, sync, _ = auto_includes(a.draft)
            includes += [d for d in more if os.path.normcase(os.path.abspath(d))
                         not in {os.path.normcase(os.path.abspath(x)) for x in includes}]
    # never into the draft's folder (a shared app folder's obj/ was cleared under other builders)
    out = os.path.join(ROOT, '.work', agent_name(), 'appfile', 'obj')
    inline = set()
    want, total = markers(a.draft, inline)
    app = App()
    img = app.img
    for attempt in (0, 1):
        rc, log, objs, before = compile_draft(a, includes, sync, out)
        errors = [l for l in log.splitlines() if ' error ' in l or ' warning ' in l]
        print('\n'.join(errors[:30]))
        if rc or not objs:
            raise SystemExit(f'compile failed (rc {rc})')
        # a stale result: the draft changed meanwhile, the object predates it, or it lacks a
        # function the draft defines (the VM compiled an old copy: forget the sync cache, once)
        now = os.path.getmtime(a.draft)
        if now != before or os.path.getmtime(objs[0]) < now - 2:
            raise SystemExit(f'stale result: {objs[0]} is older than {a.draft} (or the draft changed during the compile)')
        compiled = {r['name'] for r in match_objects(img, objs)}
        lacking = sorted(n for n in want if n not in inline and n not in compiled)
        if not lacking or a.allow_missing:
            break
        if attempt == 0:
            print(f'the object lacks {", ".join(lacking)}: resending the sources and compiling again', flush=True)
            vc6.reset_sync_cache()
            continue
        raise SystemExit(f'stale result: the object lacks {", ".join(lacking)}, defined in the draft '
                         f'(--allow-missing to report anyway)')
    obj = CoffObject(objs[0])
    funcs = {name: (sec, s, e) for name, sec, s, e in obj.functions()}
    exact = 0
    for r in match_objects(img, objs):
        addrs = want.get(r['name'], [])
        addr = addrs.pop(0) if addrs else None
        hits = [int(x, 16) for x in r['addresses']]
        if addr is None:
            print(f"  {r['status']:8s} {r['name']:50s} len {r['length']:4d} (no marker) {' '.join(r['addresses'][:3])}")
            continue
        sec, s, e = funcs[r['symbol']]
        pat, _, length = obj.pattern(sec, s, e)
        o = img.offset(addr)
        if re.fullmatch(pat, img.data[o:o + length], re.S):
            exact += 1
            client_size = app.size(addr)
            if client_size != length:
                print(f"  PREFIX   {r['name']:50s} len {r['length']:4d} 0x{addr:06X}: the client's function is {client_size} bytes")
                continue
            note = {'exact': '', 'short': ' (too short to search for: matched at its marker only)',
                    'multiple': f" (the pattern also occurs at {' '.join(x for x in r['addresses'] if int(x, 16) != addr)})"}
            print(f"  EXACT    {r['name']:50s} len {r['length']:4d} 0x{addr:06X}{note.get(r['status'], '')}")
            continue
        left = objdiff.obj_lines(obj, sec, s, s + length)
        right = objdiff.client_lines(img, addr, length)
        sm = difflib.SequenceMatcher(None, [objdiff.norm(t) for _, _, t in left],
                                     [objdiff.norm(t) for _, _, t in right], autojunk=False)
        where = f" (pattern at {' '.join(r['addresses'][:3])})" if hits else ''
        print(f"  {r['status']:8s} {r['name']:50s} len {r['length']:4d} 0x{addr:06X} similarity {sm.ratio():.2f}{where}")
        if a.diff and a.diff in (r['name'], r['symbol']):
            for op, i1, i2, j1, j2 in sm.get_opcodes():
                for k in range(max(i2 - i1, j2 - j1)):
                    lft = left[i1 + k] if i1 + k < i2 else None
                    rgt = right[j1 + k] if j1 + k < j2 else None
                    lt = f'{lft[0]:5x} {lft[2]}' if lft else ''
                    rt = f'{rgt[0]:06x} {rgt[2]}' if rgt else ''
                    print(f"    {' ' if op == 'equal' else '*'} {lt:44.44s} | {rt}")
    missing = [f'{n}@0x{x:X}' for n, xs in want.items() for x in xs]
    print(f'{exact} of {total} markers exact; {len(missing)} defined but not compiled'
          + (f': {", ".join(missing)}' if missing else ''))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('show')
    s.add_argument('file')
    s.add_argument('--json')
    d = sub.add_parser('draft')
    d.add_argument('file')
    d.add_argument('--out', default=os.path.join(ROOT, '.work', 'app'))
    m = sub.add_parser('match')
    m.add_argument('draft')
    m.add_argument('-I', action='append', default=[],
                   help="include directory; 'auto' (the default when no -I is given) resolves the draft's #includes")
    m.add_argument('--pool', action='store_true', help='lease one VM from pool.py for this run ($ENB_AGENT), release after')
    m.add_argument('--allow-missing', action='store_true',
                   help='report even when the object lacks a function the draft defines (else refused as stale)')
    m.add_argument('-D', action='append', default=[], help='extra define')
    m.add_argument('--flags', default=APP_FLAGS)
    m.add_argument('--diff', help='print the objdiff of this function (demangled or mangled name)')
    for p in (d, m):
        p.add_argument('--root', help=rootguard.HELP)
    for p in (s, d):
        p.add_argument('--neighbours', type=int, default=3, help='unattributed functions shown on each side (default 3)')
    a = ap.parse_args()
    if a.cmd in ('draft', 'match'):
        rootguard.check(__file__, a.root)   # D112(4): they write .work/ (show only reads)
    if a.cmd == 'match':
        return match(a)
    fname = norm_file(a.file)
    app = App()
    rows = describe(app, fname, a.neighbours)
    if not rows:
        raise SystemExit(f'{fname}: no function attributed to it in app-functions.json')
    print_rows(rows, fname)
    if a.cmd == 'show' and a.json:
        json.dump(rows, open(a.json, 'w', newline='\n'), indent=1)
    if a.cmd == 'draft':
        print('\n' + write_draft(rows, fname, a.out))


if __name__ == '__main__':
    main()
