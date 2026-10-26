"""Per-file workflow for E&B's own code (docs/app-plan.md section 2): window -> draft -> match.

    python appfile.py show FILE                 the file's window: every function, its size, callees,
                                                strings and asserts (with their __LINE__)
    python appfile.py draft FILE [--out DIR]    also write DIR/<name>.cpp: one stub per function with
                                                all of the above and Ghidra's saved pseudocode
    python appfile.py match DRAFT.cpp [-I DIR]... [--flags F] [--diff NAME]
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
NAME` prints the full objdiff of one function. Objects land in DRAFT's directory under obj/.
"""
import argparse
import bisect
import difflib
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', 'vc6'))
from clientimage import ClientImage  # noqa: E402
from client import DATA, Names, section_of  # noqa: E402

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
    open(path, 'w', encoding='utf-8', newline='\r\n').write('\n'.join(lines) + '\n')
    return path


MARKER = re.compile(r'^\s*//\s*@(0x[0-9A-Fa-f]+)\s*$')
DEFN = re.compile(r'([A-Za-z_][\w]*(?:::~?[A-Za-z_]\w*|::operator\s*\S+?)*)\s*\(')


def markers(path):
    """{qualified name: [address]} from `// @0xADDR` lines followed by a definition; also the
    number of markers (filled or not)."""
    out = {}
    lines = open(path, encoding='latin1').read().splitlines()
    for k, l in enumerate(lines):
        m = MARKER.match(l)
        if not m:
            continue
        for nxt in lines[k + 1:k + 6]:
            if not nxt.strip() or nxt.lstrip().startswith('//'):
                continue
            d = DEFN.search(nxt)
            if d:
                out.setdefault(d.group(1).replace(' ', ''), []).append(int(m.group(1), 16))
            break
    return out, sum(1 for l in lines if MARKER.match(l))


def match(a):
    import vc6
    from objmatch import CoffObject, match_objects, demangle
    import objdiff
    out = os.path.join(os.path.dirname(os.path.abspath(a.draft)), 'obj')
    for f in os.listdir(out) if os.path.isdir(out) else ():
        if f.lower().endswith('.obj'):
            os.remove(os.path.join(out, f))
    defines = list(vc6.DEFAULT_DEFINES) + ['ENABLE_WWPROFILE'] + a.D  # with NDEBUG (D61) and ENABLE_WWPROFILE (D74), as APP_LIBS
    rc, log = vc6.compile_sources([a.draft], a.I, defines, a.flags, out)
    errors = [l for l in log.splitlines() if ' error ' in l or ' warning ' in l]
    print('\n'.join(errors[:30]))
    objs = [os.path.join(out, f) for f in os.listdir(out) if f.lower().endswith('.obj')] if os.path.isdir(out) else []
    if rc or not objs:
        raise SystemExit(f'compile failed (rc {rc})')
    app = App()
    img = app.img
    want, total = markers(a.draft)
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
    m.add_argument('-I', action='append', default=[], help='include directory')
    m.add_argument('-D', action='append', default=[], help='extra define')
    m.add_argument('--flags', default=APP_FLAGS)
    m.add_argument('--diff', help='print the objdiff of this function (demangled or mangled name)')
    for p in (s, d):
        p.add_argument('--neighbours', type=int, default=3, help='unattributed functions shown on each side (default 3)')
    a = ap.parse_args()
    if a.cmd == 'match':
        return match(a)
    fname = norm_file(a.file)
    app = App()
    rows = describe(app, fname, a.neighbours)
    if not rows:
        raise SystemExit(f'{fname}: no function attributed to it in app-functions.json')
    print_rows(rows, fname)
    if a.cmd == 'show' and a.json:
        json.dump(rows, open(a.json, 'w'), indent=1)
    if a.cmd == 'draft':
        print('\n' + write_draft(rows, fname, a.out))


if __name__ == '__main__':
    main()
