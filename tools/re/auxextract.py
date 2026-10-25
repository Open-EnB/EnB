"""Run the client's AuxData root constructors symbolically (M1.3, D43): the schema trees with each
node's vtable, constructor calls and position in its allocation (`vt`, `ctors`, `at`).

A copy of the reference decoder's research/tools/auxschema/extract_auxschema.py (same author, same
client), extended only to record `ctors`, `vt` and `at` per node; with those stripped its output
equals the reference decoder's src/Shared/the reference game model/Schema/client-auxdata.json (checked by
`tools/re/auxroots.py check`). Used by tools/re/auxroots.py; standalone:

    python tools/re/auxextract.py CLIENT.exe OUT.json

The original's docstring follows.
Extract the client's AuxData schemas from client.exe (design §8.2, docs/research/auxdata.md).

The client builds every AuxData tree in C++ constructors: each field is a named Delta object,
containers add their children in wire order with Compound::Add, and arrays add N unnamed
elements in a loop. This script runs those constructors symbolically (x86, just enough to follow
`this` pointers, the stack, heap allocations and loop bounds) and writes the trees as JSON.

    pip install capstone
    python extract_auxschema.py "C:/Program Files (x86)/EA GAMES/Earth & Beyond/release/client.exe" out.json

Addresses are for the retail client with SHA-256 249f8524...51c4; the script refuses others.
"""
import hashlib, json, re, struct, sys
import capstone

CLIENT_SHA256 = '249f852496f341c00361be95b66e7c0becc2c39e7cba026f9227e38b602651c4'

BASE_CTOR = 0x4a4d40        # DeltaBase::DeltaBase(const char *name)
ADD_CHILD = 0x4a5670        # DeltaCompound::Add(DeltaBase *child)
SHIP_ADD = 0x6f8c70         # the ship's Add(bool narrow, DeltaBase *child): narrow sets the child's mask to 0xCF
MASK, DEFAULT_MASK = 0x80, 0x3ff    # a Delta's visibility mask (docs/research/auxdata.md §3)
ALLOC = 0x4e9cb0            # operator new
SKIP = {0x4e9410, 0x4e9d10, 0xa266da}   # assert, operator delete, sprintf
COMPOUND_READ = 0x4a3c70
VALUE_CTORS = {0x700b80, 0x700b40, 0x4a7080}   # DeltaTemplateClass<int> / <float> / <MoneyClass>(name, const T &) out of line (M1.3)
STRING_CTOR = 0x4a78f0       # DeltaStringClass(name, default) (M1.3)    # DeltaCompound's raw read: reads nothing
RDATA = (0xae7000, 0xb6c000)
REGS = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')
MEM_RE = re.compile(r'^(?:dword|word|byte) ptr \[(\w+)(?: ([+-]) (0x[0-9a-f]+|\d+))?\]$')
NUM_RE = re.compile(r'0x[0-9a-f]+|\d+')

# Root classes: constructor (or the factory case that builds it inline) by name.
ROOTS = {
    'Ship': 0x6f66e0, 'Component': 0x6fbda0, 'Light': 0x6f9870, 'Decoration': 0x6f9c30,
    'Harvestable': 0x6f9ed0, 'ManufacturingLab': 0x6fa6f0, 'Player': 0x752510, 'Hulk': 0x6f6076,
}
# Planet, Starbase and Stargate are built inline in the factory and hold only Name.
NAME_ONLY = ('Planet', 'Starbase', 'Stargate')
FACTORY_INDEX, FACTORY_TABLE, FACTORY_TYPES = 0x6f6518, 0x6f64ec, 0x2a

# Leaf types by vtable; other leaves are fixed-size blobs named by their size.
LEAF_TYPES = {
    0xaeaeb0: 'float', 0xb00d34: 'int32', 0xb00d8c: 'bool', 0xaeb0e4: 'string',
    0xaeb1ec: 'time', 0xaeafb8: 'money', 0xb00a00: 'color',
}

class Image:
    def __init__(self, path):
        self.d = open(path, 'rb').read()
        sha = hashlib.sha256(self.d).hexdigest()
        if sha != CLIENT_SHA256:
            sys.exit(f'{path}: SHA-256 {sha} is not the client these addresses are for')
        d = self.d
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        n, oh = struct.unpack_from('<H', d, pe + 6)[0], struct.unpack_from('<H', d, pe + 20)[0]
        self.base = struct.unpack_from('<I', d, pe + 52)[0]
        self.secs = [struct.unpack_from('<8sIIII', d, pe + 24 + oh + 40 * i) for i in range(n)]
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self._ret = {}

    def off(self, a):
        for _, vs, va, rs, ra in self.secs:
            if va + self.base <= a < va + self.base + rs:
                return a - va - self.base + ra
        return None

    def u32(self, a): return struct.unpack_from('<I', self.d, self.off(a))[0]

    def cstr(self, a):
        o = self.off(a)
        if o is None: return None
        e = self.d.find(b'\0', o, o + 100)
        s = self.d[o:e] if e >= 0 else b''
        return s.decode('latin1') if 2 <= len(s) < 80 and all(32 <= c < 127 for c in s) else None

    def thunk(self, a):
        o = self.off(a)
        return a + 5 + struct.unpack_from('<i', self.d, o + 1)[0] if self.d[o] == 0xe9 else a

    def disasm(self, a, size=0x8000):
        o = self.off(a)
        return self.md.disasm(self.d[o:o + size], a)

    def ret_size(self, fn):
        if fn not in self._ret:
            self._ret[fn] = next((int(i.op_str, 16) if i.op_str else 0
                                  for i in self.disasm(fn, 0x6000) if i.mnemonic == 'ret'), 0)
        return self._ret[fn]

    def read_size(self, vt):
        """Bytes a leaf's read method takes from the buffer (the push before its read call)."""
        rd = self.thunk(self.u32(vt + 0x2c))
        if rd == COMPOUND_READ: return None
        last = None
        for ins in self.disasm(rd, 200):
            if ins.mnemonic == 'push' and NUM_RE.fullmatch(ins.op_str): last = int(ins.op_str, 0)
            if ins.mnemonic == 'call': return last
            if ins.mnemonic == 'ret': break
        return None

class Node:
    def __init__(self, addr): self.addr = addr; self.name = None; self.vt = None; self.children = []; self.count = None; self.ctors = []; self.value = None; self.ivalue = None

class Interp:
    """Symbolic values: ('T', a) object pointer, ('C', n) constant, ('E', o) stack slot."""
    def __init__(self, img): self.img = img; self.nodes = {}; self.masks = {}; self.heap = 0x10000000; self.stores = {}

    def node(self, a):
        return self.nodes.setdefault(a, Node(a))

    def run(self, fn, this, args=(), depth=0):
        img = self.img
        if depth > 8 or fn in SKIP: return
        regs = {r: None for r in REGS}; regs['ecx'] = this
        sp = 0; mem = {4 + 4 * j: a for j, a in enumerate(args)}
        snap = None; last_add = None; recent = []

        def addr_of(m):
            mm = MEM_RE.match(m)
            if not mm: return None
            r, sign, k = mm.groups(); k = int(k, 0) if k else 0
            if sign == '-': k = -k
            if r == 'esp': return ('E', sp + k)
            b = regs.get(r)
            return (b[0], b[1] + k) if b and b[0] in ('E', 'T') else None

        def val(x):
            if x in regs: return regs[x]
            if NUM_RE.fullmatch(x): return ('C', int(x, 0))
            a = addr_of(x)
            return mem.get(a[1]) if a and a[0] == 'E' else None

        def stream():
            pc = fn; steps = 0
            while steps < 20000:
                restart = None
                for ins in img.disasm(pc):
                    steps += 1
                    yield ins
                    if ins.mnemonic == 'jmp' and ins.op_str.startswith('0x') and int(ins.op_str, 16) > ins.address:
                        restart = int(ins.op_str, 16); break      # follow forward jumps
                if restart is None: return
                pc = restart

        for ins in stream():
            m, ops = ins.mnemonic, ins.op_str
            parts = ops.split(', ') if ops else []
            recent = (recent + [ins])[-6:]
            # a loop's back edge: its bound is the immediate in the compare just before it
            if m.startswith('j') and ops.startswith('0x') and int(ops, 16) < ins.address and last_add is not None:
                for p in reversed(recent[:-1]):
                    if p.mnemonic == 'cmp':
                        k = p.op_str.split(', ')[1]
                        if NUM_RE.fullmatch(k) and self.node(last_add).count is None:
                            self.node(last_add).count = int(k, 0)
                        break
            if m == 'push':
                sp -= 4; mem[sp] = val(parts[0])
            elif m == 'pop':
                v = mem.get(sp); sp += 4
                if parts[0] in regs: regs[parts[0]] = v
            elif m == 'mov' and len(parts) == 2:
                dst, src = parts
                if dst in regs: regs[dst] = val(src)
                elif dst != 'esp':
                    a = addr_of(dst)
                    if a and a[0] == 'E': mem[a[1]] = val(src)
                    elif a and a[0] == 'T' and dst.startswith('dword') and not NUM_RE.fullmatch(src):
                        v = val(src)
                        if v and v[0] == 'C': self.stores[a[1]] = v[1]          # (M1.3) a value from a register
                    elif a and a[0] == 'T' and NUM_RE.fullmatch(src) and dst.startswith('byte'):
                        self.stores[a[1]] = int(src, 0)                            # (M1.3) a bool leaf's value at +0x84
                    elif a and a[0] == 'T' and NUM_RE.fullmatch(src) and dst.startswith('dword'):
                        imm = int(src, 0)
                        self.stores[a[1]] = imm                                    # (M1.3) a leaf's value at +0x84
                        if RDATA[0] <= imm < RDATA[1]: self.node(a[1]).vt = imm   # last store = most derived
                        elif imm <= DEFAULT_MASK: self.masks[a[1] - MASK] = imm     # a mask store, if a[1] - MASK is a field
            elif m == 'lea' and len(parts) == 2 and parts[0] in regs:
                regs[parts[0]] = addr_of('dword ptr ' + parts[1])
            elif m in ('add', 'sub') and len(parts) == 2 and parts[0] == 'esp' and NUM_RE.fullmatch(parts[1]):
                n = int(parts[1], 0); sp += n if m == 'add' else -n
            elif m == 'xor' and len(parts) == 2 and parts[0] == parts[1] and parts[0] in regs:
                regs[parts[0]] = ('C', 0)
            elif m == 'or' and len(parts) == 2 and parts[0] in regs and parts[1] == '0xffffffff':
                regs[parts[0]] = ('C', 0xFFFFFFFF)                                   # (M1.3) -1 in a register
            elif m == 'call':
                if not parts[0].startswith('0x'):
                    regs['eax'] = regs['ecx'] = regs['edx'] = None; continue
                callee = img.thunk(int(parts[0], 16))
                nargs = img.ret_size(callee) // 4
                cargs = [mem.get(sp + 4 * j) for j in range(6)]
                ecx = regs['ecx']; result = None
                if callee == ALLOC:
                    self.heap += 0x100000; result = ('T', self.heap)
                elif callee == BASE_CTOR and ecx and ecx[0] == 'T':
                    if cargs[0] and cargs[0][0] == 'C': self.node(ecx[1]).name = img.cstr(cargs[0][1])
                    result = ecx
                elif callee == ADD_CHILD and ecx and ecx[0] == 'T':
                    if cargs[0] and cargs[0][0] == 'T':
                        self.node(ecx[1]).children.append(cargs[0][1]); last_add = ecx[1]
                elif callee == SHIP_ADD and ecx and ecx[0] == 'T':
                    if cargs[1] and cargs[1][0] == 'T':
                        self.node(ecx[1]).children.append(cargs[1][1]); last_add = ecx[1]
                        if cargs[0] == ('C', 1): self.masks[cargs[1][1]] = 0xcf
                elif ecx and ecx[0] == 'T' and callee not in SKIP:
                    nd = self.node(ecx[1])
                    nd.ctors.append(callee)
                    if callee == STRING_CTOR and cargs[1] and cargs[1][0] == 'C':   # (name, default)
                        o = img.off(cargs[1][1])
                        nd.value = img.cstr(cargs[1][1]) if o is not None and img.d[o] else ''
                    if callee in VALUE_CTORS and cargs[1] and cargs[1][0] == 'E':  # (name, const T &value)
                        v = mem.get(cargs[1][1])
                        if v and v[0] == 'C': nd.ivalue = v[1]
                    a = cargs[1]   # array constructors take (name, count, ...)
                    if a and a[0] == 'C' and a[1] < 10000 and nd.count is None: nd.count = a[1]
                    self.run(callee, ecx, cargs[:max(nargs, 4)], depth + 1)
                    result = ecx
                sp += 4 * nargs
                regs['eax'] = result; regs['ecx'] = regs['edx'] = None
            elif m == 'ret':
                # an early return on an error path: resume from the last branch
                o = img.off(ins.address) + ins.size
                if img.d[o] in (0xcc, 0x90) or snap is None: break
                regs, sp, mem = dict(snap[0]), snap[1], dict(snap[2])
            elif m.startswith('j') and m != 'jmp':
                snap = (dict(regs), sp, dict(mem))

def to_json(img, it, a, name=None):
    n = it.nodes[a]
    name = name or n.name
    return with_mask(it, a, json_of(img, it, n, name))

def with_mask(it, a, out):
    mask = it.masks.get(a, DEFAULT_MASK)
    if mask != DEFAULT_MASK: out['mask'] = mask
    return out

def json_of(img, it, n, name):
    out = json_of0(img, it, n, name)
    out['vt'] = f'{n.vt:#x}' if n.vt else None
    out['ctors'] = [f'{c:#x}' for c in n.ctors]
    out['at'] = f'{n.addr:#x}'
    if n.value is not None:
        out['default'] = n.value
    if not n.children and out['type'] in ('float', 'int32', 'enum', 'money', 'bool'):   # (M1.3) money: its low dword
        v = n.ivalue if n.ivalue is not None else it.stores.get(n.addr + 0x84)
        if v:
            out['value'] = v & 0xFFFFFFFF
    return out

def json_of0(img, it, n, name):
    if n.children:
        kids = [it.nodes[c] for c in n.children]
        if n.count is not None and all(k.name is None for k in kids) and len(kids) == 1:
            return {'name': name, 'type': 'array', 'count': n.count, 'element': to_json(img, it, n.children[0], 'item')}
        return {'name': name, 'type': 'struct', 'fields': [to_json(img, it, c) for c in n.children]}
    t = LEAF_TYPES.get(n.vt)
    if t is None:
        size = img.read_size(n.vt)
        if size is None: raise ValueError(f'{name}: no type for vtable {n.vt:#x}')
        t = 'enum' if size == 4 else f'bytes{size}'
    return {'name': name, 'type': t}

def main(client, out):
    img = Image(client)
    roots = {}
    for name, entry in ROOTS.items():
        it = Interp(img)
        if name == 'Hulk':   # built inline in the factory: the first allocation is the root
            it.run(entry, None); root = min(a for a, n in it.nodes.items() if n.children)
        else:
            it.run(entry, ('T', 0)); root = 0
        roots[name] = to_json(img, it, root, name)
    for name in NAME_ONLY:
        roots[name] = {'name': name, 'type': 'struct', 'fields': [{'name': 'Name', 'type': 'string'}]}

    # create type -> root, from the factory's switch
    index = img.d[img.off(FACTORY_INDEX):img.off(FACTORY_INDEX) + FACTORY_TYPES]
    by_entry = {}
    for name, entry in list(ROOTS.items()):
        by_entry[entry] = name
    inline = {'Planet': 'Planet', 'Starbase': 'Starbase', 'Stargate': 'Stargate', 'Hulk': 'Hulk'}
    create_types = {}
    for t in range(FACTORY_TYPES):
        case = img.u32(FACTORY_TABLE + 4 * index[t])
        names = [img.cstr(int(i.op_str, 16)) for i in img.disasm(case, 64) if i.mnemonic == 'push' and i.op_str.startswith('0x')]
        calls = [img.thunk(int(i.op_str, 16)) for i in img.disasm(case, 64) if i.mnemonic == 'call']
        root = next((inline[n] for n in names if n in inline), None) or next((by_entry[c] for c in calls if c in by_entry), None)
        if root: create_types[str(t)] = root

    doc = {
        'source': f'client.exe sha256 {CLIENT_SHA256}',
        'generator': 'research/tools/auxschema/extract_auxschema.py',
        'createTypes': create_types,
        'playerRoot': 'Player',
        'roots': roots,
    }
    with open(out, 'w', newline='\n') as f:
        json.dump(doc, f, indent=1)
        f.write('\n')

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
