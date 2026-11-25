"""The standing behaviour audit (AU, run 2026-11-25): what byte matching masks, compared by value, ours vs client.exe.

Byte matching masks every relocated field and not-exact functions differ anyway, so a wrong string, float, global,
callee, case target, immediate or call argument hides behind "exact" or behind codegen residue. Session 0 of the
week plan (run 2026-11-24-night) found 22 real behaviour differences this way with one-off scripts (P1 objscan,
R5 immscan, LD relcheck, R6 relscan, SC sem, P2 strscan / realscan, PS slots_all); this is them as one tool.

    python tools/re/behaudit.py CHECK[,CHECK...] [--objs DIR ...] [--tree DIR] [--libobjs ROOT] [--listings DIR]
                                [--only ADDR ...] [--out DIR] [--known TSV] [--no-stale]

CHECK (each writes OUT/<check>.tsv, every row; `all` runs them all and writes OUT/report.md, the open rows):
  lit    literal contents at every relocated field: string literals (text; "" included; a __FILE__ by its file
         name), __real@ float / double constants (bytes, never a tolerance); exact functions at the same offset,
         not-exact ones at the aligned row, plus the multiset rows (lit-missing / real-missing / lit-extra)
  rel    relocation targets: callees and vtables (symbols.json, through the incremental-link thunks; a call to a
         byte-identical copy is `fold`), data (relocaudit's data map: the definer's reads, else the majority of exact
         readers), statics (the same object's exact readers), `named-other` (the client's value is another named
         symbol), `inconsistent` (not-exact readers disagree); switches: every case value's target, through the
         byte index table, mapped through the alignment (`switch`); local labels (`selfref`); multiset rows
         (sym-missing / data-missing)
  imm    immediates of aligned not-exact rows: imm (same instruction, other value), immreg / regimm (one side an
         immediate, the other a register: the register's constant traced back, reg=same is codegen; P1's 0.0f
         pushed as a bool)
  args   pushed argument slots of every aligned call of the not-exact functions (behaudit_args.py, from PS); with
         --listings DIR (the /FAs .asm next to each object) frame slots compare by name
  fpu    x87 chains of three or more operands whose operands and operations are the client's in another order
         (VC6 reassociates an unparenthesised `a * K * b`: B3's 0x6D64C0); int / float loads alike
  data   initialised data (datacmp.py: tables, pointers to strings / functions, by target; a pointer through a
         thunk is the same, two targets holding the same bytes are codegen)
  seeds  stashed forms the tree has not adopted (behaudit_seeds.py: D185 variant stashes scoring above the tree,
         seed-patch hunks never applied; SD's varcheck and R3's seedcheck2), not a behaviour check
  all    every check above

Pairing: by mangled name (symbols.json), by the tree's `// @0xADDR` markers, not-exact.md's rows naming an address
and attempts.csv's (address, symbol) rows (demangled names), and an `Unknown_XXXXXX` name's own address. An address
symbols.json names for another mangled symbol is not a candidate. A candidate that the masked pattern fullmatches
makes the function exact there (a credited exact entry first; identical uncredited copies of several functions,
String_To_Value overloads reading their own tables, are assigned one to one, the fewest rows and reads of statics
consistent with the untied functions first); otherwise the best-aligned candidate not taken by an exact match, one
function per address (overloads of one name: the best aligned), none under MIN_RATIO. Skipped: deleting destructors
(??_G / ??_E), stale objects (an object older than its source's last commit in --tree, unless --no-stale).

Filters (the scripts' false positives, each row keeps its class in column `auto`): `noise` = a hit whose expected
value occurs elsewhere in the client's function (`shifted`), fold and static rows, lit-extra, stl_prime_list / $E /
Random consistency rows, a string datacmp could not read; `names` = an assert's expression text (a literal pushed
right after a __FILE__: D56's naming work); `codegen` = esp cleanup immediates, reg=same, an argument difference the
resolver could not show different, a data pointer whose target holds the client's target's bytes. Rows matching an entry of the known file (default
tools/re/behaudit_known.tsv: check, address, kind, ours, client, result, reason; `*` matches anything) carry its
result. Open rows = neither filtered nor known: each is a candidate behaviour difference to check on the client's
instructions (docs/workflow.md: handoff checks). Prints one summary line per check; rows go to the TSVs.

Objects: a fresh build (`build.py build --lib app` or a checker's `.work/wt/<id>/build/app/obj`); library objects
for the data map from --libobjs (default the objects' own build root when it holds every library, else the main
tree's build/: a partial root leaves most data symbols unplaced). Fixtures: test_behaudit.py. Run 2026-11-25 at
run/2026-11-25's head (a9fa08ee3, .work/wt/hc1/build): ~14,400 exact and ~440 not-exact functions; after the filters
and session 0's classified rows, 25 functions had open rows, all codegen but 0x6D64C0's fpu row (B3's fix).
"""
import argparse
import bisect
import collections
import csv
import difflib
import glob
import os
import re
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import capstone  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from objmatch import CoffObject, RELOC_SIZE, demangle, thunks_of  # noqa: E402
import appfile  # noqa: E402
import relocaudit as RA  # noqa: E402

DIR32, REL32 = 0x06, 0x14
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True
NUM = re.compile(r'-?\b(0x[0-9a-f]+|\d+)\b')
CHECKS = ('lit', 'rel', 'imm', 'fpu', 'args', 'data', 'seeds')
KNOWN_TSV = os.path.join(HERE, 'behaudit_known.tsv')
COLS = ('check', 'kind', 'obj', 'addr', 'function', 'status', 'ratio', 'how', 'off', 'ours', 'client_va', 'client',
        'ours_val', 'client_val', 'expected', 'basis', 'note', 'auto', 'known')
JUMP = re.compile(r'^jmp dword ptr \[e\w\w\*4 \+ (0x[0-9a-f]+|\d+)\]$')
BYTE_LOAD = re.compile(r'^(?:mov|movzx) \w+, byte ptr \[e\w\w \+ (0x[0-9a-f]+|\d+)\]$')
SKIP_PREFIX = ('??_G', '??_E')
MIN_RATIO = 0.1     # a not-exact pair aligning fewer rows is a wrong pair
SIZE = {'byte': 1, 'word': 2, 'dword': 4, 'qword': 8, 'tbyte': 10}
DATA_RANGE = (0xA90000, 0xC50000)     # the client's .rdata and .data (R6's bounds)
HEXNAME = re.compile(r'(?:_|\bx)([0-9A-F]{6})$')
LOOSE = re.compile(r'^\s*//\s*@?(0x[0-9A-Fa-f]{6,})\b')


# ---------------------------------------------------------------- the context: client, names, markers, data map
def loose_markers(path):
    """{qualified name: [address]}: a `// @0xADDR` or `// 0xADDR...` line followed by a definition (PS's form)."""
    out = {}
    lines = open(path, encoding='latin1').read().splitlines()
    for k, line in enumerate(lines):
        m = LOOSE.match(line)
        if not m:
            continue
        for nxt in lines[k + 1:k + 6]:
            if not nxt.strip() or nxt.lstrip().startswith('//') or appfile.PRAGMA.match(nxt) \
                    or nxt.lstrip().startswith('template'):
                continue
            d = appfile.DEFN.search(nxt)
            if d and not nxt.rstrip().endswith(';'):
                out.setdefault(d.group(1).replace(' ', ''), []).append(int(m.group(1), 16))
            break
    return out


def last_commits(tree, since):
    """{path relative to tree (lower case, '/'): commit time} of app/ files changed after `since` (epoch)."""
    out = {}
    try:
        r = subprocess.run(['git', '-C', tree, 'log', '--format=C %ct', '--name-only', '--since=%d' % int(since),
                            '--', 'app'], capture_output=True, text=True, timeout=120)
    except (OSError, subprocess.SubprocessError):
        return out
    t = 0
    for line in r.stdout.splitlines():
        if line.startswith('C '):
            t = int(line[2:])
        elif line.strip():
            out.setdefault(line.strip().lower(), t)
    return out


class Context:
    def __init__(self, objdirs, tree=REPO, libroot=None, stale=True, log=None):
        self.tree = tree
        self.log = log or (lambda *a: None)
        self.img = ClientImage()
        self.thunk = thunks_of(self.img)
        self.exact_syms, self.known, self.rev = RA.load_exact(os.path.join(REPO, 'docs', 'data', 'symbols.json'))
        self.starts = sorted(self.img.function_starts()[0])
        self.paths = sorted(p for d in objdirs for p in glob.glob(os.path.join(d, '*.obj')))
        self.markers = collections.defaultdict(list)
        self.src_of = {}
        for src in glob.glob(os.path.join(tree, 'app', '**', '*.cpp'), recursive=True) \
                + glob.glob(os.path.join(tree, 'app', '**', '*.h'), recursive=True):
            rel = os.path.relpath(src, tree).replace('\\', '/')
            if src.endswith('.cpp'):
                self.src_of.setdefault(os.path.splitext(os.path.basename(src))[0].lower(), rel)
            for n, al in loose_markers(src).items():
                self.markers[n].extend(al)
        # functions without a marker line: not-exact.md's rows naming an address, attempts.csv's (address, symbol)
        ne = os.path.join(tree, 'docs', 'data', 'not-exact.md')
        if os.path.exists(ne):
            for line in open(ne, encoding='utf-8', errors='replace'):
                m = re.match(r'\| `[^`]+` \| `([^`]+)` \| \w+ \| (.*)', line)
                a = m and re.search(r'(?:attempts\.py show |\(|@)(0x[0-9A-Fa-f]{6})\b', m.group(2))
                if a and '::' in m.group(1):
                    self.markers[m.group(1)].append(int(a.group(1), 16))
        at = os.path.join(tree, 'docs', 'data', 'attempts.csv')
        if os.path.exists(at):
            with open(at, encoding='utf-8', errors='replace') as fh:
                for r in csv.DictReader(fh):
                    try:
                        a = int(r.get('address') or '', 16)
                    except ValueError:
                        continue
                    if r.get('symbol') and not r['symbol'].startswith(('$', '_$', '?')) \
                            and a not in self.markers.get(r['symbol'], ()):
                        self.markers[r['symbol']].append(a)
        self.stale = set()
        if stale and self.paths:
            oldest = min(os.path.getmtime(p) for p in self.paths)
            commits = last_commits(tree, oldest - 1)
            for p in self.paths:
                rel = self.src_of.get(os.path.splitext(os.path.basename(p))[0].lower())
                if rel and commits.get(rel.lower(), 0) > os.path.getmtime(p):
                    self.stale.add(p)
        self.objs = {}
        self._data = None
        self._defs = None
        if libroot is None:     # the build the objects come from (all libraries), else the main tree's
            up = os.path.dirname(os.path.dirname(os.path.abspath(objdirs[0]))) if objdirs else ''
            libroot = up if os.path.isdir(os.path.join(up, 'ww3d2', 'obj')) else os.path.join(REPO, 'build')
        self.libroot = libroot

    def obj(self, p):
        if p not in self.objs:
            self.objs[p] = CoffObject(p)
        return self.objs[p]

    def u32(self, va):
        off = self.img.offset(va)
        return None if off is None or off + 4 > len(self.img.data) else struct.unpack_from('<I', self.img.data, off)[0]

    # relocaudit's data map, from the exact functions of these objects and of the other libraries' objects
    def data(self):
        if self._data is not None:
            return self._data
        dirs = {os.path.normcase(os.path.dirname(p)) for p in self.paths}
        lib_paths = list(self.paths)
        for lib in RA.LIBS:
            d = os.path.join(self.libroot, lib, 'obj')
            if lib != 'app' and os.path.normcase(d) not in dirs:
                lib_paths += sorted(glob.glob(os.path.join(d, '*.obj')))
        ex_rows = RA.scan(lib_paths, self.img, self.exact_syms)
        data_obs = collections.defaultdict(lambda: collections.defaultdict(set))
        static_obs = collections.defaultdict(lambda: collections.defaultdict(set))
        any_obs = collections.defaultdict(collections.Counter)
        for p, fn, a, k, typ, sym, add, v, raw, own in ex_rows:
            nm = sym['name']
            if typ == DIR32 and not nm.startswith(('??_C@', '__real@', '$L')):
                any_obs[nm if sym['class'] == 2 else (os.path.basename(p).lower(), nm)][v] += 1
            if typ != DIR32 or nm in self.known or nm.startswith(('??_C@', '__real@')):
                continue
            if sym['class'] == 2:
                data_obs[nm][v].add((a, any(s['name'] == nm and s['section'] > 0 for s in self.obj(p).symbols)))
            elif sym['class'] == 3 and own:
                static_obs[(os.path.basename(p).lower(), nm)][v].add(a)
        data_map, basis = {}, {}
        for nm, vals in data_obs.items():
            dv = {v for v, fs in vals.items() if any(d for _, d in fs)}
            if len(dv) == 1:
                data_map[nm] = dv.pop()
                basis[nm] = 'definer'
                continue
            count = {v: len({x for x, _ in fs}) for v, fs in vals.items()}
            best = sorted(count, key=lambda v: (-count[v], v))
            if len(best) == 1 or count[best[0]] > count[best[1]]:
                data_map[nm] = best[0]
                basis[nm] = 'majority %d/%d' % (count[best[0]], sum(count.values()))
        rev_data = {}
        for nm, v in data_map.items():
            rev_data.setdefault(v, demangle(nm) or nm)
        defs = {}
        for q in lib_paths:
            try:
                for name, sec, start, end in self.obj(q).functions():
                    defs.setdefault(name, (q, sec, start, end))
            except (ValueError, OSError, struct.error):
                pass
        self._defs = defs
        self._data = (data_map, basis, static_obs, any_obs, rev_data)
        self.log('data map: %d exact reloc rows, %d data symbols mapped' % (len(ex_rows), len(data_map)))
        return self._data

    def client_name(self, v):
        t = self.thunk.get(v, v)
        if t in self.rev:
            return self.rev[t]
        return self.data()[4].get(v)

    def folded(self, nm, v):
        self.data()
        d = self._defs.get(nm)
        if not d:
            return False
        pat, fixed, length = self.obj(d[0]).pattern(*d[1:])
        off = self.img.offset(self.thunk.get(v, v))
        return off is not None and re.fullmatch(pat, self.img.data[off:off + length], re.S) is not None

    def masked_exact(self, o, sec, s, e, addr):
        pat, _, n = o.pattern(sec, s, e)
        off = self.img.offset(addr)
        return off is not None and re.fullmatch(pat, self.img.data[off:off + n], re.S) is not None


# ---------------------------------------------------------------- rows of both sides (R5 / R6's forms)
def norm(text):
    return re.sub(r'\b(j\w+) 0x[0-9a-f]+', r'\1 <branch>', text)


def shape(text):
    t = re.sub(r'\b(j\w+|call) \S+', r'\1 X', text)
    return NUM.sub('#', t.replace('REL', '#'))


def imm_of(ins):
    for o in ins.operands:
        if o.type == capstone.x86.X86_OP_IMM:
            enc = ins.encoding
            return o.imm & 0xffffffff, enc.imm_offset, enc.imm_offset + enc.imm_size
    return None


def self_relocs(o, sec, start, end):
    """{field offset in the section: target offset in the section} of the relocations to this section's own symbols."""
    info = o.sections[sec]
    secsym = {sm['index'] for sm in o.symbols if sm['section'] == sec + 1}
    out = {}
    for va, symidx, typ in info['relocs']:
        if start <= va < end and symidx in secsym and RELOC_SIZE.get(typ, 4) == 4 and typ == DIR32:
            sm = o.symbol_at(symidx)
            out[va] = struct.unpack_from('<I', o.data, info['raw'] + va)[0] + sm['value']
    return out


def code_end(o, sec, start, end, selfrel=None):
    """Where the code of a function ends: the first switch table (a self relocation's target inside the function
    that is itself a relocated field, or a byte table one reads), else the function's end. R6's fix of R5's cut."""
    selfrel = self_relocs(o, sec, start, end) if selfrel is None else selfrel
    data_start = end
    for va, add in selfrel.items():
        if start < add < end and add in selfrel:
            data_start = min(data_start, add)
    return data_start - start


def obj_rows(o, sec, start, end):
    info = o.sections[sec]
    selfrel = self_relocs(o, sec, start, end)
    body = o.data[info['raw'] + start:info['raw'] + end]
    rel = {}
    for va, symidx, typ in info['relocs']:
        if start <= va < end:
            rel[va - start] = (symidx, typ, RELOC_SIZE.get(typ, 4))
    cov = set()
    for off, (_, _, n) in rel.items():
        cov.update(range(off, off + n))
    body = body[:code_end(o, sec, start, end, selfrel)]
    rows = []
    for ins in MD.disasm(body, 0):
        raw = f'{ins.mnemonic} {ins.op_str}'
        covered = any(x in cov for x in range(ins.address, ins.address + ins.size))
        text = raw
        im = imm_of(ins)
        enc = ins.encoding
        imm_rel = im is not None and any(x in cov for x in range(ins.address + im[1], ins.address + im[2]))
        disp_rel = enc.disp_size and any(x in cov for x in range(ins.address + enc.disp_offset,
                                                                    ins.address + enc.disp_offset + enc.disp_size))
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5:
            text = f'{ins.mnemonic} REL'
        elif covered:
            ops = ins.op_str.split(', ')
            if imm_rel:
                ops[-1] = 'REL'
            if disp_rel:
                for k, op in enumerate(ops):
                    if '[' in op:
                        inner = op[op.index('[') + 1:op.rindex(']')]
                        # the displacement is a trailing number after + / - (or the whole operand); not the
                        # scale of `[eax*4]`, whose zero displacement capstone leaves out (R5 / R6's form read the
                        # 4 as the displacement, so no switch's jump row ever aligned)
                        if re.search(r'(?:^|[+-] )(0x[0-9a-f]+|\d+)$', inner):
                            inner = re.sub(r'(0x[0-9a-f]+|\d+)$', 'REL', inner)
                        else:
                            inner += ' + REL'
                        ops[k] = op[:op.index('[') + 1] + inner + op[op.rindex(']'):]
            text = f'{ins.mnemonic} ' + ', '.join(ops)
        rels = [(x, rel[x]) for x in sorted(rel) if ins.address <= x < ins.address + ins.size]
        rows.append({'off': ins.address, 'raw': raw, 'text': text, 'mn': ins.mnemonic, 'size': ins.size,
                     'imm': None if im is None or imm_rel else im[0], 'rels': rels,
                     'ioff': enc.imm_offset if im else None, 'doff': enc.disp_offset if enc.disp_size == 4 else None,
                     'ops': [x.type for x in ins.operands]})
    return rows


def client_rows(ctx, addr, length):
    """The client's rows from addr to its next verified start (at most 3x ours + 64), not to our length."""
    i = bisect.bisect_right(ctx.starts, addr)
    if i < len(ctx.starts):
        length = max(length, min(ctx.starts[i] - addr, 3 * length + 64))
    rows = []
    o = ctx.img.offset(addr)
    for ins in MD.disasm(ctx.img.data[o:o + length + 64], addr):
        raw = f'{ins.mnemonic} {ins.op_str}'
        text = raw
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5:
            text = f'{ins.mnemonic} REL'
        elif re.search(r'0x[0-9a-f]{6,}', ins.op_str):
            text = re.sub(r'0x[0-9a-f]{6,}', 'REL', text)
        im = imm_of(ins)
        enc = ins.encoding
        rows.append({'off': ins.address, 'raw': raw, 'text': text, 'mn': ins.mnemonic, 'size': ins.size,
                     'imm': None if im is None else im[0], 'ops': [x.type for x in ins.operands],
                     'ioff': enc.imm_offset if im and enc.imm_size == 4 else None,
                     'doff': enc.disp_offset if enc.disp_size == 4 else None})
        if ins.address - addr >= length:
            break
    return rows


def exact_client_rows(ctx, addr, left):
    """The client's rows at our offsets (an exact function: the same instructions)."""
    rows = []
    for L in left:
        va = addr + L['off']
        o = ctx.img.offset(va)
        ins = next(MD.disasm(ctx.img.data[o:o + 16], va), None)
        if ins is None:
            continue
        im = imm_of(ins)
        enc = ins.encoding
        rows.append({'off': va, 'raw': f'{ins.mnemonic} {ins.op_str}', 'text': '', 'mn': ins.mnemonic,
                     'size': ins.size, 'imm': None if im is None else im[0], 'ops': [x.type for x in ins.operands],
                     'ioff': enc.imm_offset if im and enc.imm_size == 4 else None,
                     'doff': enc.disp_offset if enc.disp_size == 4 else None})
    return rows


def align(left, right):
    """[(ours, client, how)]: objdiff's difflib on the masked text, then each differing block on the shape."""
    sm = difflib.SequenceMatcher(None, [norm(r['text']) for r in left], [norm(r['text']) for r in right],
                                 autojunk=False)
    pairs = []
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op == 'equal':
            pairs.extend((left[i1 + k], right[j1 + k], 'eq') for k in range(i2 - i1))
            continue
        if op != 'replace':
            continue
        ls = [shape(r['text']) for r in left[i1:i2]]
        rs = [shape(r['text']) for r in right[j1:j2]]
        sm2 = difflib.SequenceMatcher(None, ls, rs, autojunk=False)
        for op2, a1, a2, b1, b2 in sm2.get_opcodes():
            if op2 == 'equal':
                pairs.extend((left[i1 + a1 + k], right[j1 + b1 + k], 'shape') for k in range(a2 - a1))
            elif op2 == 'replace' and a2 - a1 == b2 - b1:
                pairs.extend((left[i1 + a1 + k], right[j1 + b1 + k], 'rep') for k in range(a2 - a1))
    return sm.ratio(), pairs


FAM = {}
for _big, _rest in (('eax', 'ax al ah'), ('ebx', 'bx bl bh'), ('ecx', 'cx cl ch'), ('edx', 'dx dl dh'),
                    ('esi', 'si'), ('edi', 'di'), ('ebp', 'bp')):
    for _r in [_big] + _rest.split():
        FAM[_r] = _big
WRITES = {'mov', 'xor', 'or', 'and', 'add', 'sub', 'inc', 'dec', 'lea', 'pop', 'movzx', 'movsx', 'neg', 'not',
          'shl', 'shr', 'sar', 'imul', 'xchg', 'sbb', 'adc', 'cdq'} | {'set' + c for c in
          ('e', 'ne', 'l', 'le', 'g', 'ge', 'a', 'ae', 'b', 'be', 's', 'ns')}


def reg_value(rows, idx, reg):
    """The constant a register holds before rows[idx], by the last linear write to it, or None."""
    fam = FAM.get(reg)
    if fam is None:
        return None
    for k in range(idx - 1, -1, -1):
        r = rows[k]
        mn = r['mn']
        if mn == 'call':
            if fam in ('eax', 'ecx', 'edx'):
                return None
            continue
        ops = r['raw'][len(mn) + 1:].split(', ')
        if mn == 'cdq' and fam == 'edx':
            return None
        if mn not in WRITES or not ops or FAM.get(ops[0]) != fam:
            continue
        if mn in ('xor', 'sub') and len(ops) == 2 and ops[0] == ops[1]:
            return 0
        if mn == 'mov' and r['imm'] is not None:
            return r['imm']
        if mn == 'or' and r['imm'] == 0xffffffff:
            return 0xffffffff
        return None
    return None


# ---------------------------------------------------------------- pairing
class Func:
    """One of our functions paired with a client address."""

    def __init__(self, path, o, name, sec, s, e, addr, exact, ratio=1.0):
        self.path, self.o, self.name, self.sec, self.s, self.e = path, o, name, sec, s, e
        self.dn = demangle(name) or name
        self.addr, self.exact, self.ratio = addr, exact, ratio
        self.left = self.right = self.pairs = None

    def rows(self, ctx):
        if self.left is None:
            self.left = obj_rows(self.o, self.sec, self.s, self.e)
            if self.exact:
                self.right = exact_client_rows(ctx, self.addr, self.left)
                self.pairs = list(zip(self.left, self.right, ['eq'] * len(self.right)))
            else:
                self.right = client_rows(ctx, self.addr, self.e - self.s)
                self.ratio, self.pairs = align(self.left, self.right)
        return self.left, self.right, self.pairs


def pair_functions(ctx, only=None):
    """[Func]: every function of ctx.paths with a client address, exact ones first; and the stats."""
    stats = collections.Counter()
    exact, work, taken = [], [], set()
    owner = collections.defaultdict(set)     # symbols.json's names of an address (any kind)
    for n, addrs in ctx.known.items():
        for a in addrs:
            owner[a].add(n)
    for p in ctx.paths:
        if p in ctx.stale:
            stats['stale_objects'] += 1
            continue
        try:
            o = ctx.obj(p)
            funcs = o.functions()
        except (ValueError, OSError, struct.error) as ex:
            ctx.log('skip %s: %s' % (p, ex))
            continue
        for name, sec, s, e in funcs:
            if name.startswith(SKIP_PREFIX):
                continue
            dn = demangle(name) or name
            cands = set(ctx.known.get(name, ())) | set(ctx.markers.get(dn, ()))
            m = HEXNAME.search(dn.split('::')[-1])
            if m:
                cands.add(int(m.group(1), 16))      # Unknown_473780: the address in the name (PS's address_of)
            cands = sorted(a for a in cands if ctx.img.offset(a) is not None)
            if not cands:
                continue
            hit = [a for a in cands if ctx.masked_exact(o, sec, s, e, a)]
            if hit:
                taken.update(hit)
                credited = [a for a in hit if a in ctx.exact_syms.get(name, ())]
                if not credited:
                    hit = [a for a in hit if (not owner.get(a) or name in owner[a])]
                    if not hit:
                        stats['exact_elsewhere'] += 1     # its copies are other symbols' credited addresses
                        continue
                exact.append((p, o, name, sec, s, e, credited or hit, bool(credited)))
                continue
            work.append((p, o, name, sec, s, e, [a for a in cands if (not owner.get(a) or name in owner[a])]))
    out, seen = [], set()
    for p, o, name, sec, s, e, hits, credited in exact:
        group = []
        for a in hits:
            f = Func(p, o, name, sec, s, e, a, True)
            if (f.name, a) in seen:
                continue
            seen.add((f.name, a))
            group.append(f)
        if not credited and len(group) > 1:
            for f in group:
                f.tied = group      # identical masked copies: the one with the fewest rows is ours (strscan's rule)
        out += group
    for p, o, name, sec, s, e, cands in work:
        free = [a for a in cands if a not in taken]
        if not free:
            stats['no_free_address'] += 1
            continue
        best = None
        left = obj_rows(o, sec, s, e)
        for a in free:
            f = Func(p, o, name, sec, s, e, a, False)
            f.left = left
            f.right = client_rows(ctx, a, e - s)
            f.ratio, f.pairs = align(left, f.right)
            if best is None or f.ratio > best.ratio:
                best = f
        if best.ratio < MIN_RATIO:
            stats['unaligned'] += 1
            continue
        if (best.name, best.addr) in seen:
            continue
        seen.add((best.name, best.addr))
        out.append(best)
    best = {}
    for f in out:
        if not f.exact and (f.addr not in best or f.ratio > best[f.addr].ratio):
            best[f.addr] = f
    stats['overload_dropped'] = sum(1 for f in out if not f.exact and best[f.addr] is not f)
    out = [f for f in out if f.exact or best[f.addr] is f]
    if only:
        out = [f for f in out if f.addr in only]
    stats['exact'] = sum(1 for f in out if f.exact)
    stats['notexact'] = sum(1 for f in out if not f.exact)
    return out, stats


# ---------------------------------------------------------------- lit / rel / imm rows of one function
def norm_file(b):
    return RA.norm_file(b)


def our_literal(o, sym):
    if sym['name'].startswith('??_C@_00'):
        return b'\0'            # objmatch's literal() gives None for "" (P2's trap): every "" site was skipped
    return o.literal(sym)


def client_text(ctx, va, wide):
    co = ctx.img.offset(va)
    if co is None:
        return None
    end = ctx.img.data.find(b'\0' * (2 if wide else 1), co)
    return ctx.img.data[co:end] if end >= 0 else ctx.img.data[co:co + 256]


def judge(ctx, f, sym, typ, add, v, raw, width=16):
    """(check, kind, expected, basis) when the client's value disagrees with what our symbol is known to be, else
    None; kind 'unknown' / 'unknown-call' when nothing is known about the symbol (the consistency pass)."""
    o, nm = f.o, sym['name']
    data_map, basis, static_obs, any_obs, _ = ctx.data()
    if nm in ctx.known:
        want = ctx.known[nm]
        t = ctx.thunk
        if t.get(v, v) in want or v in want or any(t.get(w) == t.get(v, v) for w in want):
            return None
        if typ == REL32 and ctx.folded(nm, v):
            return ('rel', 'fold', min(want), 'symbols.json')
        return ('rel', 'call' if typ == REL32 else 'address', min(want), 'symbols.json')
    if typ != DIR32:
        if ctx.folded(nm, v):
            return None
        return ('rel', 'unknown-call', None, '')
    if nm.startswith('??_C@'):
        lit = our_literal(o, sym)
        if lit is None or add >= len(lit):
            return None
        wide = nm.startswith('??_C@_1')
        want = lit[add:]
        whole = client_text(ctx, raw, wide)
        if whole is None:
            return ('lit', 'literal', None, 'literal outside the image')
        co = ctx.img.offset(raw)
        have = ctx.img.data[co:co + len(want)]
        if have == want or norm_file(whole) == norm_file(want.rstrip(b'\0')):
            return None
        kind = 'literal-case' if whole.lower() == want.rstrip(b'\0').lower() else 'literal'
        return ('lit', kind, None, 'ours %r client %r' % (want.rstrip(b'\0')[:70], whole[:70]))
    if nm.startswith('__real@'):
        m = re.match(r'__real@([48a])@', nm)
        if m and sym['section'] > 0:
            size = {'4': 4, '8': 8, 'a': 10}[m.group(1)]
            info = o.sections[sym['section'] - 1]
            want = o.data[info['raw'] + sym['value']:info['raw'] + sym['value'] + size]
            co = ctx.img.offset(raw)
            have = ctx.img.data[co:co + size] if co is not None else b''
            if have != want:
                if size in (4, 8):
                    fmt = '<f' if size == 4 else '<d'
                    ov = struct.unpack(fmt, want)[0]
                    cv = struct.unpack(fmt, have)[0] if len(have) == size else None
                else:
                    ov, cv = want.hex(), have.hex()
                return ('lit', 'constant', None, 'ours %r client %r' % (ov, cv))
        return None
    if nm in data_map:
        if data_map[nm] != v:
            return ('rel', 'data', data_map[nm], basis[nm])
        return None
    if sym['class'] == 3:
        vals = static_obs.get((os.path.basename(f.path).lower(), nm), {})
        if vals:
            count = {x: len(fs) for x, fs in vals.items()}
            major = max(count, key=lambda x: (count[x], -x))
            if v != major and v not in vals:
                return ('rel', 'static', major, 'static, same object %d/%d' % (count[major], sum(count.values())))
            return None
    key = nm if sym['class'] == 2 else (os.path.basename(f.path).lower(), nm)
    if nm.startswith('$L'):
        return None     # an EH state table / funclet label: per object, its address is the layout's
    if key in any_obs:
        c = any_obs[key]
        major = max(c, key=lambda x: (c[x], -x))
        if v not in c:
            return ('rel', 'data', major, 'exact readers %d/%d' % (c[major], sum(c.values())))
        return None
    got = content_match(ctx, o, sym, add, v, width)
    if got is True:
        return None
    if got is not None:
        return ('rel', 'data-content', None, got)
    cn = ctx.client_name(v)
    if cn and (demangle(nm) or nm) != cn:
        return ('rel', 'named-other', None, 'client value named %s' % cn)
    return ('rel', 'unknown', None, '')


def content_match(ctx, o, sym, add, v, n=16):
    """True when our initialised data at sym + add (n bytes, to the section's end) is the client's at v + add: plain
    bytes equal, a relocated pointer to a literal reaching the same text, to a known function / vtable the same
    address; a description of the first difference; None when it cannot be told (no data, an uninitialised section,
    pointers to other unknown symbols only)."""
    if sym['section'] <= 0:
        return None
    sec = sym['section'] - 1
    info = o.sections[sec]
    if info['flags'] & 0x20 or info['flags'] & 0x80 or not info['raw']:
        return None         # code, or .bss
    if 'stl_prime_list' in sym['name']:
        return None
    lo = sym['value'] + max(add, 0)
    nxt = [x['value'] for x in o.symbols if x['section'] == sym['section'] and x['value'] > sym['value']]
    hi = min(lo + n, min(nxt) if nxt else info['size'])     # to the symbol's end: what follows is laid out per object
    if hi <= lo:
        return None
    rel = {va: (symidx, typ) for va, symidx, typ in info['relocs'] if lo <= va < hi}
    ours = o.data[info['raw'] + lo:info['raw'] + hi]
    co = ctx.img.offset((v + max(add, 0)) & 0xffffffff)
    if co is None:
        return None
    theirs = ctx.img.data[co:co + len(ours)]
    told = False
    k = 0
    while k < len(ours):
        if lo + k in rel and k + 4 <= len(ours):
            t = o.symbol_at(rel[lo + k][0])
            cv = struct.unpack_from('<I', theirs, k)[0] if len(theirs) >= k + 4 else None
            if t is not None and cv is not None:
                lit = our_literal(o, t)
                if lit is not None:
                    have = client_text(ctx, cv - struct.unpack_from('<i', ours, k)[0], t['name'].startswith('??_C@_1'))
                    if have is None or have != lit.rstrip(b'\0'):
                        return 'entry +%d: ours %r client %r' % (k, lit.rstrip(b'\0')[:40], (have or b'')[:40])
                    told = True
                elif t['name'] in ctx.known:
                    if ctx.thunk.get(cv, cv) not in ctx.known[t['name']] and cv not in ctx.known[t['name']]:
                        return 'entry +%d: ours %s client 0x%06X' % (k, (demangle(t['name']) or t['name'])[:40], cv)
                    told = True
            k += 4
            continue
        if k < len(theirs) and ours[k] != theirs[k]:
            return 'byte +%d: ours %02x client %02x' % (k, ours[k], theirs[k])
        if ours[k]:
            told = True
        k += 1
    return True if told else None


def row(f, check, kind, L=None, R=None, how='', ours_val='', client_val='', expected='', basis='', note=''):
    return {'check': check, 'kind': kind, 'obj': os.path.basename(f.path), 'addr': '0x%06X' % f.addr,
            'function': f.dn[:90], 'status': 'exact' if f.exact else 'notexact', 'ratio': '%.3f' % f.ratio,
            'how': how, 'off': '+%x' % L['off'] if L else '', 'ours': L['raw'][:70] if L else '',
            'client_va': '%06x' % R['off'] if R else '', 'client': R['raw'][:70] if R else '',
            'ours_val': str(ours_val)[:120], 'client_val': str(client_val)[:120],
            'expected': expected, 'basis': basis, 'note': note, 'auto': '', 'known': ''}


def client_fields(ctx, right):
    """The values of the client's 32-bit fields anywhere in its function (with call targets and their thunks)."""
    cvals, cimms = set(), set()
    for r in right:
        for fo in (r['ioff'], r['doff']):
            if fo is None:
                continue
            x = ctx.u32(r['off'] + fo)
            if x is None:
                continue
            cvals.add(x)
            if r['mn'] in ('call', 'jmp') and fo == r['ioff'] and r['size'] == 5:
                t = (r['off'] + 5 + struct.unpack('<i', struct.pack('<I', x))[0]) & 0xffffffff
                cvals.add(t)
                cvals.add(ctx.thunk.get(t, t))
        if r['imm'] is not None:
            cimms.add(r['imm'])
    return cvals, cimms


def is_assert_text(o, left, L):
    """E&B's assert pushes __LINE__, __FILE__, then the expression's text: a literal pushed right after a path."""
    k = left.index(L)
    if k == 0 or left[k - 1]['mn'] != 'push':
        return False
    for _, (symidx, typ, sz) in left[k - 1]['rels']:
        sym = o.symbol_at(symidx)
        lit = sym and our_literal(o, sym)
        if lit and (b'\\' in lit or b'/' in lit) and re.search(rb'\.(cpp|h)\0$', lit.lower()):
            return True
    return False


def function_rows(ctx, f, uncheck):
    """lit / rel / imm rows of one paired function (aligned rows, switches, multiset rows)."""
    left, right, pairs = f.rows(ctx)
    o, info = f.o, f.o.sections[f.sec]
    out = []
    cvals, cimms = client_fields(ctx, right)
    selfrel = self_relocs(o, f.sec, f.s, f.e)
    for L, R, how in pairs:
        if how == 'rep' and L['mn'] != R['mn']:
            continue
        for fo_abs, (symidx, typ, sz) in L['rels']:
            if sz != 4 or typ not in (DIR32, REL32):
                continue
            sym = o.symbol_at(symidx)
            if sym is None:
                continue
            pos = fo_abs - L['off']
            cpos = R['ioff'] if pos == L['ioff'] else R['doff'] if pos == L['doff'] else None
            if f.exact:
                cpos = pos
            if cpos is None:
                continue
            raw = ctx.u32(R['off'] + cpos)
            if raw is None:
                continue
            add = struct.unpack_from('<i', o.data, info['raw'] + f.s + fo_abs)[0]
            if f.s + fo_abs in selfrel:
                continue        # a local label (switch tables: switch_rows)
            if typ == REL32:
                v = (R['off'] + cpos + 4 + struct.unpack('<i', struct.pack('<I', raw))[0]) & 0xffffffff
            else:
                v = (raw - add) & 0xffffffff
            ms = re.search(r'(byte|word|dword|qword|tbyte) ptr \[', L['raw']) if pos == L['doff'] else None
            res = judge(ctx, f, sym, typ, add, v, raw, SIZE[ms.group(1)] if ms else 16)
            if res is None:
                continue
            check, kind, exp, basis = res
            ours_val = demangle(sym['name']) or sym['name']
            client_val = '0x%06X' % (ctx.thunk.get(v, v) if typ == REL32 else v)
            if check == 'lit':
                m = re.match(r"ours (.*) client (.*)$", basis)
                if m:
                    ours_val, client_val = m.group(1), m.group(2)
            r = row(f, check, kind, L, R, how, ours_val, client_val, '0x%06X' % exp if exp is not None else '',
                    basis if check != 'lit' else '', '')
            if kind in ('unknown', 'unknown-call'):
                uncheck.append((sym['name'] if sym['class'] == 2 else (r['obj'].lower(), sym['name']), v, r))
                continue
            if exp is not None and not f.exact and (exp in cvals or any(ctx.thunk.get(c) == exp for c in cvals)):
                r['note'] = 'shifted'
            if check == 'lit' and sym['name'].startswith('??_C@') and not f.exact:
                lit = our_literal(o, sym)
                if lit and any(ctx.img.offset(x) is not None and ctx.img.data.find(lit, ctx.img.offset(x))
                               == ctx.img.offset(x) for x in cvals):
                    r['note'] = 'shifted'
            if check == 'lit' and kind.startswith('literal') and is_assert_text(o, left, L):
                r['note'] = (r['note'] + ' assert-text').strip()
            if check == 'rel' and kind in ('call', 'address'):
                cn = ctx.client_name(v)
                r['note'] = (r['note'] + ' client=' + cn[:60]).strip() if cn else r['note']
            out.append(r)
        # immediates (immscan's classes); an exact function's immediates are not masked
        if f.exact or L['mn'].startswith('j') or L['mn'] in ('call', 'loop'):
            continue
        hit = None
        if how in ('shape', 'rep') and L['imm'] is not None and R['imm'] is not None and L['imm'] != R['imm'] \
                and shape(L['text']) == shape(R['text']):
            hit = ('imm', '0x%x' % L['imm'], '0x%x' % R['imm'])
        elif how == 'rep' and len(L['ops']) == len(R['ops']) and L['ops'] and \
                shape(L['text']).rsplit(' ', 1)[0] == shape(R['text']).rsplit(' ', 1)[0]:
            if L['imm'] is not None and R['imm'] is None and L['ops'][-1] == 2 and R['ops'][-1] == 1:
                hit = ('immreg', '0x%x' % L['imm'], R['raw'].rsplit(' ', 1)[-1])
            elif R['imm'] is not None and L['imm'] is None and R['ops'][-1] == 2 and L['ops'][-1] == 1:
                hit = ('regimm', L['raw'].rsplit(' ', 1)[-1], '0x%x' % R['imm'])
        if hit:
            note = 'shifted' if hit[0] == 'imm' and L['imm'] in cimms else ''
            if hit[0] in ('immreg', 'regimm'):
                if hit[0] == 'regimm':
                    v_, want_, reg_ = reg_value(left, left.index(L), hit[1]), R['imm'], hit[1]
                else:
                    v_, want_, reg_ = reg_value(right, right.index(R), hit[2]), L['imm'], hit[2]
                if v_ is None:
                    note = 'reg=?'
                else:
                    m_ = 0xff if reg_.endswith(('l', 'h')) else 0xffffffff
                    note = 'reg=same' if (v_ & m_) == (want_ & m_) else 'reg=0x%x' % v_
            if L['mn'] in ('add', 'sub') and L['raw'].startswith(L['mn'] + ' esp,'):
                note = (note + ' esp').strip()
            out.append(row(f, 'imm', hit[0], L, R, how, hit[1], hit[2], note=note))
    out += switch_rows(ctx, f, selfrel)
    out += fpu_rows(ctx, f)
    out += multiset_rows(ctx, f, cvals)
    return out


def body_sig(ctx, f, rows, at, where, client):
    """What a case body's first row reaches (later rows are often a tail another case shares; mov / lea and
    registers are codegen): the value of its relocated (ours) / absolute (client's) field, a string's text, a
    function's address, a table entry's string; None off the rows, with no field, or when it cannot be told."""
    k = at.get(where)
    if k is None:
        return None
    v = field_value(ctx, f, rows[k], client)
    return v or None


def field_value(ctx, f, r, client):
    """The value an instruction's 32-bit field reaches (None: no field, False: not told)."""
    mem = '[' in r['raw']
    if client:
        fo = r['doff'] if mem and r['doff'] is not None else r['ioff']
        x = ctx.u32(r['off'] + fo) if fo is not None else None
        if x is None or not (0x401000 <= x < 0xC50000):
            return None
        if mem:
            x = ctx.u32(x)
            if x is None:
                return False
        if ctx.thunk.get(x, x) in ctx.rev:
            return ('a', ctx.thunk.get(x, x))
        s = ctx.img.cstr(x, 200) if DATA_RANGE[0] <= x < DATA_RANGE[1] else None
        return ('s', s.encode('latin1')) if s else False
    if not r['rels']:
        return None
    fo_abs, (symidx, typ, sz) = r['rels'][0]
    sym = f.o.symbol_at(symidx)
    if sym is None or typ != DIR32:
        return False
    info = f.o.sections[f.sec]
    add = struct.unpack_from('<i', f.o.data, info['raw'] + f.s + fo_abs)[0]
    if not mem:
        lit = our_literal(f.o, sym)
        if lit is not None:
            return ('s', lit.rstrip(b'\0'))
        if sym['name'] in ctx.known:
            return ('a', min(ctx.known[sym['name']]))
        return False
    if sym['section'] <= 0:
        return ('a', min(ctx.known[sym['name']])) if sym['name'] in ctx.known else False
    ti = f.o.sections[sym['section'] - 1]
    at = sym['value'] + add
    for va, idx, t in ti['relocs']:
        if va == at:
            tgt = f.o.symbol_at(idx)
            lit = tgt and our_literal(f.o, tgt)
            if lit is not None:
                return ('s', lit.rstrip(b'\0'))
            if tgt and tgt['name'] in ctx.known:
                return ('a', min(ctx.known[tgt['name']]))
            return False
    return False


def switch_rows(ctx, f, selfrel):
    """Each switch's case values compared by where they go: ours through our byte index table and jump table, the
    client's through its own, our target carried through the alignment (identity in an exact function). Masking
    hides every jump-table entry, so an exact function can send a case to another body."""
    left, right, pairs = f.rows(ctx)
    o, info = f.o, f.o.sections[f.sec]
    to_client = {}
    for L, R, how in pairs:
        if how in ('eq', 'shape'):
            to_client[L['off']] = R['off']
    if f.exact:
        to_client = {L['off']: f.addr + L['off'] for L in left}
    body = o.data[info['raw'] + f.s:info['raw'] + f.e]
    ours_tables = sorted({t - f.s for t in selfrel.values() if f.s < t < f.e and t in selfrel})
    at_o = {L['off']: k for k, L in enumerate(left)}
    at_c = {R['off']: k for k, R in enumerate(right)}
    out = []
    plist = list(pairs)
    for k, (L, R, how) in enumerate(plist):
        mo, mc = JUMP.match(L['text'].replace('REL', '0')), JUMP.match(R['raw'])
        if not (mo and mc) or L['doff'] is None:
            continue
        fld = f.s + L['off'] + L['doff']
        if fld not in selfrel:
            continue
        t_o = selfrel[fld] - f.s
        t_c = int(mc.group(1), 0)
        n = 0
        while f.s + t_o + 4 * n in selfrel and (n == 0 or t_o + 4 * n not in ours_tables):
            n += 1
        jt_o = [selfrel[f.s + t_o + 4 * i] - f.s for i in range(n)]
        jt_c = [ctx.u32(t_c + 4 * i) for i in range(n)]
        # the byte index table read a few rows before (a two-level switch), on both sides
        idx_o = idx_c = None
        for L2, R2, _ in plist[max(0, k - 4):k][::-1]:
            bo, bc = BYTE_LOAD.match(L2['text'].replace('REL', '0')), BYTE_LOAD.match(R2['raw'])
            if bo and bc and L2['doff'] is not None and f.s + L2['off'] + L2['doff'] in selfrel:
                i_o = selfrel[f.s + L2['off'] + L2['doff']] - f.s
                i_c = int(bc.group(1), 0)
                nxt = min([t for t in ours_tables if t > i_o] + [len(body)])
                idx_o = list(body[i_o:nxt].rstrip(b'\xcc\x90'))
                co = ctx.img.offset(i_c)
                idx_c = list(ctx.img.data[co:co + len(idx_o)])
                break
        values = range(len(idx_o)) if idx_o is not None else range(n)
        bad, unmapped = [], 0
        for v in values:
            if idx_o is not None:
                a, b = idx_o[v], idx_c[v] if v < len(idx_c) else None
                tgt_o = jt_o[a] if a < n else None
                tgt_c = jt_c[b] if b is not None and b < n else None
            else:
                tgt_o, tgt_c = jt_o[v], jt_c[v]
            if tgt_o is None or tgt_c is None:
                if (tgt_o is None) != (tgt_c is None):
                    bad.append(v)
                continue
            if to_client.get(tgt_o) == tgt_c:
                continue
            # not where the alignment put it: the same body reached elsewhere (case bodies laid out in another
            # order) compares by its first rows' shape
            so, sc = body_sig(ctx, f, left, at_o, tgt_o, False), body_sig(ctx, f, right, at_c, tgt_c, True)
            if so is None or sc is None:
                if f.exact:
                    bad.append(v)       # an exact function's case bodies sit at our offsets: another one is wrong
                else:
                    unmapped += 1
            elif so != sc:
                bad.append(v)
        if bad:
            out.append(row(f, 'rel', 'switch', L, R, how, 'cases ' + ','.join(map(str, bad[:20])),
                           'table 0x%06X' % t_c, note='%d of %d values%s' % (
                               len(bad), len(values), ', %d unmapped' % unmapped if unmapped else '')))
    return out


def multiset_rows(ctx, f, cvals):
    """Every relocation, aligned or not: our known targets / literals / constants the client's function never
    uses, and the client's literals ours never uses (not-exact functions only)."""
    if f.exact:
        return []
    left, right, _ = f.rows(ctx)
    o, info = f.o, f.o.sections[f.sec]
    data_map = ctx.data()[0]
    img = ctx.img
    cnorm = {ctx.thunk.get(x, x) for x in cvals} | cvals
    cstrs = {}
    creals = set()
    lo, hi = DATA_RANGE
    for x in cvals:
        if lo <= x < hi:
            b = img.cstr(x, 4096)
            if b:
                cstrs[b.encode('latin1')] = x
            off_ = img.offset(x)
            if off_ is not None:
                creals.add(bytes(img.data[off_:off_ + 4]))
                creals.add(bytes(img.data[off_:off_ + 8]))
    our_lits = set()
    out = []
    for L in left:
        for fo_abs, (symidx, typ, sz) in L['rels']:
            if sz != 4 or typ not in (DIR32, REL32):
                continue
            sym = o.symbol_at(symidx)
            if sym is None:
                continue
            nm = sym['name']
            add = struct.unpack_from('<i', o.data, info['raw'] + f.s + fo_abs)[0] if typ == DIR32 else 0
            miss = None
            if nm.startswith('??_C@'):
                lit = our_literal(o, sym)
                if lit is None or add or nm.startswith('??_C@_1'):
                    continue
                t = lit.rstrip(b'\0')
                our_lits.add(t)
                if t and t not in cstrs and not any(norm_file(c) == norm_file(t) for c in cstrs):
                    miss = ('lit', 'lit-missing', repr(t[:70]))
            elif nm.startswith('__real@'):
                m_ = re.match(r'__real@([48])@', nm)
                if m_ and sym['section'] > 0:
                    size = int(m_.group(1))
                    ti = o.sections[sym['section'] - 1]
                    want = bytes(o.data[ti['raw'] + sym['value']:ti['raw'] + sym['value'] + size])
                    if want not in creals:
                        miss = ('lit', 'real-missing', repr(struct.unpack('<f' if size == 4 else '<d', want)[0]))
            elif nm in ctx.known:
                want = ctx.known[nm]
                if not any((w in cnorm) or (w + add in cnorm) or (ctx.thunk.get(w, w) in cnorm) for w in want):
                    if not (typ == REL32 and any(ctx.folded(nm, x) for x in cnorm if img.offset(x) is not None
                                                 and 0x401000 <= x < 0xA90000)):
                        miss = ('rel', 'sym-missing', (demangle(nm) or nm)[:80])
            elif nm in data_map:
                if data_map[nm] + add not in cnorm:
                    miss = ('rel', 'data-missing', (demangle(nm) or nm)[:80])
            if miss:
                out.append(row(f, miss[0], miss[1], L, None, '', miss[2], ''))
    for b, x in sorted(cstrs.items()):
        if len(b) >= 2 and b not in our_lits and not any(norm_file(b) == norm_file(t) for t in our_lits):
            out.append(row(f, 'lit', 'lit-extra', None, None, '', '', repr(b[:70]), note='0x%06X' % x))
    return out


def consistency_rows(uncheck):
    """A symbol nothing else knows, read at several client values by the not-exact functions: the minority rows."""
    byv = collections.defaultdict(collections.Counter)
    for nm, v, r in uncheck:
        byv[nm][v] += 1
    out = []
    for nm, v, r in uncheck:
        c = byv[nm]
        if len(c) > 1:
            major = max(c, key=lambda x: (c[x], -x))
            if v != major:
                r = dict(r, kind='inconsistent', expected='0x%06X' % major,
                         basis='not-exact readers %d/%d' % (c[major], sum(c.values())))
                out.append(r)
    return out


# ---------------------------------------------------------------- fpu: x87 evaluation order
FOPS = {'fmul': 'mul', 'fimul': 'mul', 'fadd': 'add', 'fiadd': 'add', 'fdiv': 'div', 'fidiv': 'div',
        'fdivr': 'divr', 'fidivr': 'divr', 'fsub': 'sub', 'fisub': 'sub', 'fsubr': 'subr', 'fisubr': 'subr'}
FLOADS = ('fld', 'fild')
FMEM = re.compile(r'^(byte|word|dword|qword|tbyte) ptr (?:\w+:)?\[([^\]]*)\]$')


def fpu_operand(ctx, f, row, client):
    """A comparable name for an x87 row's memory operand: a constant by its value, a member by its size and offset,
    a stack slot as `stack`, another global as `global`; int loads (fild / fimul ...) marked `i`; None otherwise."""
    m = FMEM.match(row['raw'].split(' ', 1)[1]) if ' ' in row['raw'] else None
    if not m:
        return None
    size, inner = SIZE[m.group(1)], m.group(2)
    kind = 'i' if row['mn'].startswith('fi') else 'f'
    if re.match(r'^(esp|ebp)\b', inner) and (inner.startswith('esp') or False):
        return (kind, size, 'stack')
    if not client and row.get('rels'):
        sym = f.o.symbol_at(row['rels'][0][1][0])
        if sym is not None and sym['name'].startswith('__real@') and sym['section'] > 0:
            info = f.o.sections[sym['section'] - 1]
            b = f.o.data[info['raw'] + sym['value']:info['raw'] + sym['value'] + size]
            return (kind, size, 'k' + b.hex())
        return (kind, size, 'global')
    if client:
        g = re.fullmatch(r'0x([0-9a-f]{6,})', inner)
        if g:
            va = int(g.group(1), 16)
            off = ctx.img.offset(va)
            lo, hi = ctx.img.section('.rdata')[:2]
            if off is not None and lo <= va < hi:      # .rdata: a constant
                return (kind, size, 'k' + ctx.img.data[off:off + size].hex())
            return (kind, size, 'global')
    mm = re.match(r'^(\w+)(?: ([+-]) (0x[0-9a-f]+|\d+))?$', inner)
    if not mm:
        return (kind, size, 'mem?')
    off = int(mm.group(3), 0) * (-1 if mm.group(2) == '-' else 1) if mm.group(3) else 0
    return (kind, size, 'm%+d' % off)


def fpu_chains(ctx, f, rows, client):
    """[(first row, [(op, operand)])]: a load (fld / fild of memory) and the memory-operand arithmetic that follows it
    on the stack top, until a store, compare, another load, a call or a jump; other instructions are skipped."""
    out, cur = [], None
    for r in rows:
        mn = r['mn']
        if mn in FLOADS:
            if cur and len(cur[1]) >= 3:
                out.append(cur)
            opnd = fpu_operand(ctx, f, r, client)
            cur = (r, [('ld', opnd)]) if opnd else None
            continue
        if not mn.startswith('f') and mn not in ('call', 'ret') and not mn.startswith('j'):
            continue
        if cur and mn in FOPS and ',' not in r['raw']:
            opnd = fpu_operand(ctx, f, r, client)
            if opnd:
                cur[1].append((FOPS[mn], opnd))
                continue
        if cur and len(cur[1]) >= 3:
            out.append(cur)
        cur = None
    if cur and len(cur[1]) >= 3:
        out.append(cur)
    return out


def canon(ch):
    """(op, operand) pairs, the load and the first operation swapped into order when they commute."""
    seq = [(op, x[1:]) for op, x in ch]
    if seq[1][0] in ('mul', 'add') and seq[1][1] < seq[0][1]:
        seq[0], seq[1] = ('ld', seq[1][1]), (seq[1][0], seq[0][1])
    return seq


def chain_key(ch):
    return (tuple(sorted(op for op, _ in ch[1:])), tuple(sorted(x[1:] for _, x in ch)))


def fpu_rows(ctx, f):
    """A chain of three or more operands whose operands and operations are the client's but taken in another order
    (VC6 reassociates `a * K * b`: B3's 0x6D64C0); the int / float form of a load is not a difference (exact)."""
    if f.exact:
        return []
    left, right, _ = f.rows(ctx)
    ours, theirs = fpu_chains(ctx, f, left, False), fpu_chains(ctx, f, right, True)
    seq = {}
    for r, ch in theirs:
        seq.setdefault(chain_key(ch), []).append((r, canon(ch)))
    out = []
    for r, ch in ours:
        k = chain_key(ch)
        if 'stack' in str(k) or 'mem?' in str(k) or k not in seq:
            continue
        mine = canon(ch)
        if any(c == mine for _, c in seq[k]):
            continue
        rc, c = seq[k][0]
        out.append(row(f, 'fpu', 'order', r, rc, '', ' '.join('%s %s' % (op, x[1]) for op, x in mine)[:120],
                       ' '.join('%s %s' % (op, x[1]) for op, x in c)[:120]))
    return out



# ---------------------------------------------------------------- args and data
def args_rows(ctx, funcs, listings=None, log=None):
    import behaudit_args as A
    out = []
    for f in funcs:
        if f.exact:
            continue
        lst = None
        for d in ([listings] if listings else []) + [os.path.dirname(f.path)]:
            p = os.path.join(d, os.path.splitext(os.path.basename(f.path))[0] + '.asm')
            if os.path.exists(p):
                lst = open(p, encoding='latin1').read()
                break
        try:
            result, cls, diffs, proven, slot_rows, compared, ncalls, notes = A.audit(f.path, f.name, f.addr, lst)
        except BaseException as ex:  # noqa: B902 (explain raises SystemExit on a symbol it cannot pick)
            if isinstance(ex, KeyboardInterrupt):
                raise
            if log:
                log('args %s 0x%06X: %r' % (f.dn, f.addr, ex))
            continue
        for w, kind, o_, c_, why in diffs:
            r = row(f, 'args', kind, None, None, '', o_, c_, note=why[:200])
            m = re.search(r'@0x([0-9A-F]+)', w)
            r['client_va'] = m.group(1).lower() if m else ''
            r['client'] = w[:70]
            r['basis'] = 'listing' if lst else 'no listing'
            if why.startswith('codegen'):
                r['auto'] = 'codegen'
            elif kind == 'count' and (w, kind, o_, c_, why) not in proven:
                r['auto'] = 'codegen'
            out.append(r)
    return out


def content_equal(ctx, a, b, n=16):
    oa, ob = ctx.img.offset(a), ctx.img.offset(b)
    return oa is not None and ob is not None and ctx.img.data[oa:oa + n] == ctx.img.data[ob:ob + n]


def data_rows(ctx, log=None):
    import datacmp
    from objmatch import match_objects
    paths = [p for p in ctx.paths if p not in ctx.stale]
    results = match_objects(ctx.img, paths, datacmp.APP_REGION, callers=True)
    objs = {}
    for p in paths:
        try:
            objs[os.path.basename(p)] = ctx.obj(p)
        except (ValueError, OSError, struct.error):
            pass
    out = []
    for r in datacmp.run(ctx.img, objs, results):
        if r['status'] != 'mismatch':
            continue
        for off, kind, ours, theirs in r['diffs']:
            note, auto = '', ''
            if kind == 'ptr':
                try:
                    o_, c_ = int(ours, 16), int(theirs, 16)
                except (TypeError, ValueError):
                    o_ = c_ = None
                if o_ is not None and ctx.thunk.get(c_, c_) == ctx.thunk.get(o_, o_):
                    continue            # a function through its incremental-link thunk (datacmp compares raw)
                if o_ is not None and content_equal(ctx, o_, c_):
                    note, auto = 'the targets hold the same 16 bytes (another copy)', 'codegen'
            elif ours in (None, 'None') and theirs in (None, 'None'):
                note, auto = 'datacmp read neither string', 'noise'
            out.append({'check': 'data', 'kind': kind, 'obj': r['obj'], 'addr': '0x%06X' % r['client'],
                        'function': (demangle(r['sym']) or r['sym'])[:90], 'status': r['how'] or '', 'ratio': '',
                        'how': '', 'off': '+%x' % off, 'ours': '', 'client_va': '%06x' % (r['client'] + off),
                        'client': '', 'ours_val': str(ours)[:120], 'client_val': str(theirs)[:120], 'expected': '',
                        'basis': '', 'note': note, 'auto': auto, 'known': ''})
    return out


# ---------------------------------------------------------------- filters, known rows, output
def auto_class(r):
    kind, note = r['kind'], r['note']
    if r['auto']:
        return r['auto']
    if 'assert-text' in note.split():
        return 'names'
    if 'shifted' in note or kind in ('fold', 'static', 'lit-extra'):
        return 'noise'
    if 'esp' in note.split() or note == 'reg=same':
        return 'codegen'
    if kind in ('inconsistent', 'data') and any(x in r['ours_val'] for x in ('stl_prime_list', '$E', 'Random')):
        return 'noise'
    return ''


def load_known(path):
    out = []
    if path and os.path.exists(path):
        for line in open(path, encoding='utf-8').read().splitlines()[1:]:
            if not line.strip() or line.startswith('#'):
                continue
            f = line.split('\t')
            f += [''] * (7 - len(f))
            out.append(f[:7])
    return out


def known_of(r, known):
    vals = (r['check'], r['addr'], r['kind'], r['ours_val'], r['client_val'])
    for k in known:
        if all(p == '*' or p == v or (p.endswith('*') and v.startswith(p[:-1])) for p, v in zip(k[:5], vals)):
            return k[5]
    return ''


def write_tsv(path, rows):
    with open(path, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('\t'.join(COLS) + '\n')
        for r in rows:
            fh.write('\t'.join(str(r.get(c, '')).replace('\t', ' ').replace('\n', ' ') for c in COLS) + '\n')


def run(checks, objdirs, tree=REPO, libroot=None, listings=None, only=None, out=None, known_path=KNOWN_TSV,
        stale=True, quiet=False):
    """{check: [row]} for the checks asked; writes OUT/<check>.tsv (and report.md for more than one check)."""
    t0 = time.time()
    logf = open(os.path.join(out, 'behaudit.log'), 'w', encoding='utf-8', newline='\n') if out else None

    def log(*a):
        if logf:
            logf.write(' '.join(str(x) for x in a) + '\n')
    ctx = Context(objdirs, tree, libroot, stale, log)
    funcs, stats = pair_functions(ctx, only) if set(checks) - {'data', 'seeds'} else ([], collections.Counter())
    log('pairing: %s; stale objects: %s' % (dict(stats), sorted(os.path.basename(p) for p in ctx.stale)))
    rows = {c: [] for c in checks}
    if set(checks) & {'lit', 'rel', 'imm', 'fpu'}:
        allr, uncheck, per = [], [], {}
        for f in funcs:
            unc = []
            try:
                per[id(f)] = (function_rows(ctx, f, unc), unc)
            except (ValueError, IndexError, KeyError, struct.error, TypeError) as ex:
                log('rows %s 0x%06X: %r' % (f.dn, f.addr, ex))
        # a tie's copies also count their reads of statics that the untied functions read elsewhere (chat_defs'
        # String_To_Value for events / errors: masked-identical, each reading its own table)
        base = collections.defaultdict(set)
        for f in funcs:
            if not getattr(f, 'tied', None):
                for key, v, _ in per.get(id(f), ([], []))[1]:
                    base[key].add(v)

        def cost(g):
            got = per.get(id(g), ([], []))
            return len(got[0]) + sum(1 for key, v, _ in got[1] if key in base and v not in base[key])
        for f in keep_tied(funcs, cost):
            got = per.get(id(f), ([], []))
            allr += got[0]
            uncheck += got[1]
        allr += consistency_rows(uncheck)
        for r in allr:
            if r['check'] in rows:
                rows[r['check']].append(r)
    if 'args' in checks:
        rows['args'] = args_rows(ctx, funcs, listings, log)
    if 'seeds' in checks:
        import behaudit_seeds
        rows['seeds'] = behaudit_seeds.rows(tree)
    if 'data' in checks:
        rows['data'] = data_rows(ctx, log)
    known = load_known(known_path)
    for c, rs in rows.items():
        for r in rs:
            r['auto'] = auto_class(r)
            r['known'] = known_of(r, known)
    if out:
        for c, rs in rows.items():
            write_tsv(os.path.join(out, c + '.tsv'), rs)
        if len(checks) > 1:
            write_report(os.path.join(out, 'report.md'), rows, stats, ctx, objdirs)
    for c, rs in rows.items():
        cnt = collections.Counter(r['auto'] or ('known ' + r['known'].split()[0] if r['known'] else 'open') for r in rs)
        if not quiet:
            print('%-4s %5d rows in %4d functions: %s' % (c, len(rs), len({r['addr'] for r in rs}),
                                                          ', '.join('%s %d' % kv for kv in sorted(cnt.items()))))
    if not quiet:
        print('functions: %d exact, %d not exact, %d stale objects; %.0f s%s' % (
            stats['exact'], stats['notexact'], len(ctx.stale), time.time() - t0,
            ' -> ' + out if out else ''))
    if logf:
        logf.close()
    return rows, stats, ctx


def keep_tied(funcs, cost):
    """funcs without the losing copies of a tie: the tied candidates (a function's identical masked copies, uncredited
    and unnamed) are assigned one function per address and one address per function, the fewest rows first."""
    out, tied = [], []
    for f in funcs:
        (tied if getattr(f, 'tied', None) else out).append(f)
    used_f, used_a = set(), set()
    for f in sorted(tied, key=lambda g: (cost(g), g.addr, g.name)):
        if (f.path, f.name) in used_f or f.addr in used_a:
            continue
        used_f.add((f.path, f.name))
        used_a.add(f.addr)
        out.append(f)
    return out


def is_open(r):
    return not r['auto'] and not r['known']


def write_report(path, rows, stats, ctx, objdirs):
    with open(path, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('# Behaviour audit (generated by tools/re/behaudit.py all; do not edit)\n\n')
        fh.write('Objects: %s. Functions: %d exact, %d not exact; stale objects skipped: %d.\n\n' % (
            ', '.join(objdirs), stats['exact'], stats['notexact'], len(ctx.stale)))
        fh.write('| check | rows | open | known | filtered |\n|---|---|---|---|---|\n')
        for c, rs in rows.items():
            fh.write('| %s | %d | %d | %d | %d |\n' % (c, len(rs), sum(map(is_open, rs)),
                                                      sum(1 for r in rs if r['known'] and not r['auto']),
                                                      sum(1 for r in rs if r['auto'])))
        fh.write('\nOpen rows (neither filtered nor in tools/re/behaudit_known.tsv): check each on the client\'s '
                 'instructions, then add it to the known file with its result.\n')
        for c, rs in rows.items():
            op = [r for r in rs if is_open(r)]
            if not op:
                continue
            fh.write('\n## %s (%d open)\n\n| address | function | kind | ours | client | note |\n|---|---|---|---|---|---|\n'
                     % (c, len(op)))
            for r in op:
                fh.write('| %s | %s | %s | %s | %s | %s |\n' % (
                    r['addr'], r['function'][:50].replace('|', '/'), r['kind'], r['ours_val'][:60].replace('|', '/'),
                    r['client_val'][:60].replace('|', '/'), (r['note'] + ' ' + r['ours'] + ' / ' + r['client'])[:90]
                    .replace('|', '/')))


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('check', help='lit, rel, imm, args, data, all, or a comma list (lit,rel,imm)')
    ap.add_argument('--objs', nargs='+', default=[os.path.join(REPO, 'build', 'app', 'obj')])
    ap.add_argument('--tree', default=REPO, help='the tree whose markers pair functions and whose git log dates them')
    ap.add_argument('--libobjs', help='root of the libraries\' build folders for the data map (default build/)')
    ap.add_argument('--listings', help='folder of the /FAs .asm listings (args; default next to each object)')
    ap.add_argument('--only', nargs='+', help='client addresses to audit (hex)')
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'behaudit'))
    ap.add_argument('--known', default=KNOWN_TSV)
    ap.add_argument('--no-stale', action='store_true', help='keep objects older than their source\'s last commit')
    a = ap.parse_args(argv)
    os.makedirs(a.out, exist_ok=True)
    checks = CHECKS if a.check == 'all' else tuple(a.check.split(','))
    if set(checks) - set(CHECKS):
        ap.error('unknown check: ' + a.check)
    only = {int(x, 16) for x in a.only} if a.only else None
    run(checks, a.objs, a.tree, a.libobjs, a.listings, only, a.out, a.known, not a.no_stale)
    return 0


if __name__ == '__main__':
    sys.exit(main())
