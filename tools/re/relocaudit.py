"""Relocation-target audit of exact functions against the client's own map (PRESET, run 2026-11-21).

Byte matching masks every relocated field (objmatch.py: calls, data addresses, strings are wildcards), so a
function is credited exact while it reads another global than the client's: GMAP's case, StarmapDataClass::
Update_Transform 0x7BEAD0 naming ShaderClass::_PresetAlpha2DShader where the client copies 0xC01180, the
Additive2D preset (an opaque square on the galaxy map). relaudit.py finds a symbol read at two client addresses by
majority; this tool compares each relocation with what the symbol's client address is known or derived to be, so a
symbol named wrongly everywhere, or wrongly in the only function that reads it, shows too.

    python tools/re/relocaudit.py [--objs DIR ...] [--only TEXT] [--log PATH] [--json PATH]

Exact functions: symbols.json's `exact` entries (the ledger's crediting), each one taken where the object's masked
pattern fullmatches at that address (a symbol in several objects: every object whose copy matches). For each
relocated 32-bit field (DIR32 and REL32) the client's value is read back and compared with:

  function / vtable   symbols.json's addresses for the symbol (REL32 through the incremental-link thunks)
  data (external)     the address the symbol's defining objects read it at (their exact functions: ShaderClass's
                      initialisers 0x93B240.. for the presets), else the majority of all exact readers
  static (per object) the other exact functions of the same object (one static, one address)
  string literal      the client's string at the field (text compared; a __FILE__ by its file name)
  __real@ constant    the client's bytes at the field

Each disagreement is a row: function, its object, our symbol, the client's value with the name the client's map
gives it (symbols.json, or the data map derived above: `= ShaderClass::_PresetAdditive2DShader`), the expected
value and its basis. A value within 0x40 of the expected one is marked `member` (another member / offset of the
right object). Prints one summary line; rows to --log (default .work/log/relocaudit.log), all of it to --json.
It changes no crediting: a report for the coordinator (D246) and for behaviour bugs like gap 8 / GMAP's.
Run on fresh objects (build.py build in the tree audited, or verify's .work/<agent>/tmp/enb_import/verify-<lib>
with --objs); a stale object reports its old form.

Classes: `fold` (the client calls a byte-identical copy of our callee: the linker folds identical COMDATs, so
Create_Peek<X> of every X is one address; benign), `assert-text` (a literal pushed right after a __FILE__: an assert's
expression, debug only, but the client's variable names), `literal` / `constant` (the text or bytes differ), `data`
(another global), `call` (another callee), `address` (another function's or vtable's address), `static`.

Found on run/2026-11-21 (fixed by PRESET unless noted): proj_man Get_Shadow_Shader 0x492470 OpaqueSolid preset,
unknown_6bb7e0 AreStaticSortListsEnabled, skillnam '\\' section joiner, gd_attrb_skills' tooltip newlines,
avatar_desc 'FacialFeature-%02d', skill_render's operand order, gutter_data's case order, mousetbl's AssetManager,
SimpleSceneClass::Register's Add_Tail, cpacket's profile names, w32kbd's trace format; GMAP's starmap_data /
galaxy_map_robj Alpha2D (GMAP). Left: co 0x6FD7A0 TheInstance for AssetManager (co.cpp: pad-sensitive), ui_ping
0x5D7D60 Add_Object's overload, ui_faction 0x58E6F0 GadgetClass::Unknown_40, galaxy_map_robj 0x7B3F00 Set_Color,
LandClass::Get_Vertex_Color 0x470EF0 (1/255 one ulp off), soundrobj's PrototypeClass vtable, pkmessageline's
duration.h __FILE__, and symbols.json's stale VertexMaterialClass::Set_Ambient_Color_Source at 0x92F060 (that is
Set_Emissive: it writes +0x14) behind meshmatdesc's rows.
"""
import argparse
import collections
import glob
import json
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
DIR32, REL32 = 0x06, 0x14
LIBS = ('app', 'ww3d2', 'wwlib', 'wwmath', 'wwsaveload', 'wwaudio', 'wwshade', 'wwdebug', 'wlib', 'wnet', 'binkmovie')


def _demangle(name):
    try:
        from objmatch import demangle
        return demangle(name) or name
    except Exception:
        return name


def load_exact(path):
    """({mangled symbol: {addresses}} of exact entries, {mangled: {addresses}} of every entry, {address: name})"""
    exact, known, rev = collections.defaultdict(set), collections.defaultdict(set), {}
    for e in json.load(open(path, encoding='utf-8')):
        a = int(e['address'], 16)
        known[e['symbol']].add(a)
        if e.get('kind') != 'thunk':
            rev.setdefault(a, e.get('name') or e['symbol'])
        if e.get('kind') == 'function' and e.get('source') == 'exact':
            exact[e['symbol']].add(a)
    return exact, known, rev


def scan(paths, img, exact):
    """[(obj path, function symbol, function address, offset, type, symbol dict, addend int, field value)]"""
    from objmatch import CoffObject, _reloc_targets
    tstart, tend, traw = img.section('.text')
    text, tbase = img.data[traw:traw + (tend - tstart)], tstart
    rows = []
    for p in paths:
        try:
            o = CoffObject(p)
        except (ValueError, OSError, struct.error):
            continue
        for name, sec, start, end in o.functions():
            addrs = exact.get(name)
            if not addrs and not name.startswith('_$E'):
                continue
            pat, fixed, length = o.pattern(sec, start, end)
            hits = []
            for a in sorted(addrs or ()):
                off = img.offset(a)
                if off is not None and re.fullmatch(pat, img.data[off:off + length], re.S):
                    hits.append(a)
            if name.startswith('_$E') and len(hits) != 1:
                hits = []
                if fixed >= 6 and length <= 0x80:
                    # an uncredited initialiser of its object (ShaderClass's presets, 0x93B240..): one copy in .text
                    found = [m.start() for m in re.finditer(pat, text, re.S)][:2]
                    if len(found) == 1:
                        hits = [tbase + found[0]]
                if not hits:
                    continue    # D94(b): an initialiser's name is per object; only an unambiguous copy
            for a in hits:
                for k, n, typ, sym, addend in _reloc_targets(o, sec, start, length):
                    if n != 4 or sym is None or typ not in (DIR32, REL32):
                        continue
                    raw = struct.unpack_from('<I', img.data, img.offset(a + k))[0]
                    add = struct.unpack('<i', addend)[0]
                    if typ == REL32:
                        v = (a + k + 4 + struct.unpack('<i', struct.pack('<I', raw))[0]) & 0xFFFFFFFF
                    else:
                        v = (raw - add) & 0xFFFFFFFF
                    rows.append((p, name, a, k, typ, sym, add, v, raw, o.own(sec)))
    return rows


def norm_file(b):
    if b'\\' in b or b'/' in b:
        return re.split(rb'[\\/]', b)[-1].lower()
    return b


def audit(paths, img=None, symbols=None):
    from clientimage import ClientImage
    from objmatch import CoffObject, thunks_of
    img = img or ClientImage()
    exact, known, rev = load_exact(symbols or os.path.join(REPO, 'docs', 'data', 'symbols.json'))
    thunk = thunks_of(img)
    rows = scan(paths, img, exact)
    objs = {}

    def obj(p):
        if p not in objs:
            objs[p] = CoffObject(p)
        return objs[p]

    def defines(p, name):
        return any(s['name'] == name and s['section'] > 0 for s in obj(p).symbols)

    # the data map: external data symbols not in symbols.json
    data_obs = collections.defaultdict(lambda: collections.defaultdict(set))    # sym -> value -> {(func addr, definer)}
    static_obs = collections.defaultdict(lambda: collections.defaultdict(set))  # (obj, sym) -> value -> {func addr}
    for p, fn, a, k, typ, sym, add, v, raw, own in rows:
        nm = sym['name']
        if typ != DIR32 or nm in known or nm.startswith(('??_C@', '__real@')):
            continue
        if sym['class'] == 2:
            data_obs[nm][v].add((a, defines(p, nm)))
        elif sym['class'] == 3 and own:   # a COMDAT copy may be another object's (one stl_prime_list)
            static_obs[(p, nm)][v].add(a)
    data_map, data_basis = {}, {}
    for nm, vals in data_obs.items():
        dv = {v for v, fs in vals.items() if any(d for _, d in fs)}
        if len(dv) == 1:
            data_map[nm] = dv.pop()
            data_basis[nm] = 'definer'
            continue
        count = {v: len({x for x, _ in fs}) for v, fs in vals.items()}
        best = sorted(count, key=lambda v: (-count[v], v))
        if len(best) == 1 or count[best[0]] > count[best[1]]:
            data_map[nm] = best[0]
            data_basis[nm] = 'majority %d/%d' % (count[best[0]], sum(count.values()))
    rev_data = {}
    for nm, v in data_map.items():
        rev_data.setdefault(v, _demangle(nm))

    def client_name(v):
        t = thunk.get(v, v)
        if t in rev:
            return rev[t]
        if v in rev_data:
            return rev_data[v]
        return None

    # where our functions are defined: a call that reaches a byte-identical copy of our callee is a fold (the
    # linker's identical-COMDAT folding: Create_Peek<X> for every X at one address), not another callee
    defs = {}
    for q in paths:
        try:
            for name, sec, start, end in obj(q).functions():
                defs.setdefault(name, (q, sec, start, end))
        except (ValueError, OSError, struct.error):
            pass

    def folded(nm, v):
        d = defs.get(nm)
        if not d:
            return False
        pat, fixed, length = obj(d[0]).pattern(*d[1:])
        off = img.offset(thunk.get(v, v))
        return off is not None and re.fullmatch(pat, img.data[off:off + length], re.S) is not None

    # the relocated fields of each function by offset: an assert's expression text is pushed right after its
    # __FILE__ (push line; push file; push text)
    fields = collections.defaultdict(dict)
    for p, fn, a, k, typ, sym, add, v, raw, own in rows:
        fields[(p, a)][k] = sym

    def assert_text(p, a, k):
        prev = fields[(p, a)].get(k - 5)
        lit = prev and obj(p).literal(prev)
        return bool(lit) and (b'\\' in lit or b'/' in lit) and re.search(rb'\.(cpp|h)\0$', lit.lower()) is not None

    out = []
    for p, fn, a, k, typ, sym, add, v, raw, own in rows:
        nm = sym['name']
        exp, basis, bad = None, None, False
        if nm in known:
            want = known[nm]
            if thunk.get(v, v) not in want and v not in want and not any(thunk.get(w) == thunk.get(v, v) for w in want):
                exp, basis, bad = min(want), 'symbols.json', True
        elif typ != DIR32:
            continue
        elif nm.startswith('??_C@'):
            lit = obj(p).literal(sym)
            if lit is None or add >= len(lit):
                continue
            want = lit[add:]
            co = img.offset(raw)
            if co is None:
                bad, basis = True, 'literal outside the image'
            else:
                end = img.data.find(b'\0' * (2 if nm.startswith('??_C@_1') else 1), co)
                have = img.data[co:co + len(want)]
                whole = img.data[co:end] if end >= 0 else have
                if not (have == want or norm_file(whole) == norm_file(want.rstrip(b'\0'))):
                    bad, basis = True, 'literal %r, client %r' % (want.rstrip(b'\0')[:60], whole[:60])
        elif nm.startswith('__real@'):
            m = re.match(r'__real@([48])@', nm)
            if m and sym['section'] > 0:
                size = int(m.group(1))
                info = obj(p).sections[sym['section'] - 1]
                want = obj(p).data[info['raw'] + sym['value']:info['raw'] + sym['value'] + size]
                co = img.offset(raw)
                if co is None or img.data[co:co + size] != want:
                    bad, basis = True, 'constant %s, client %s' % (
                        want[::-1].hex(), img.data[co:co + size][::-1].hex() if co is not None else '?')
        elif sym['class'] == 2:
            if nm in data_map and data_map[nm] != v:
                exp, basis, bad = data_map[nm], data_basis[nm], True
        elif sym['class'] == 3 and own:
            vals = static_obs.get((p, nm), {})
            if len(vals) > 1:
                count = {x: len(fs) for x, fs in vals.items()}
                major = max(count, key=lambda x: (count[x], -x))
                if v != major:
                    exp, basis, bad = major, 'static, same object %d/%d' % (count[major], sum(count.values())), True
        if not bad:
            continue
        if nm in known and typ == REL32 and folded(nm, v):
            kind = 'fold'
        elif nm.startswith('??_C@'):
            kind = 'assert-text' if assert_text(p, a, k) else 'literal'
        elif nm.startswith('__real@'):
            kind = 'constant'
        elif exp is not None and abs(v - exp) <= 0x40 and not client_name(v):
            kind = 'member'
        elif nm in known:
            kind = 'call' if typ == REL32 else 'address'
        else:
            kind = 'data' if sym['class'] == 2 else 'static'
        out.append({'function': a, 'symbol': fn, 'name': _demangle(fn), 'object': os.path.relpath(p, REPO),
                    'offset': k, 'type': 'REL32' if typ == REL32 else 'DIR32', 'reloc': nm, 'reloc_name': _demangle(nm),
                    'client': v, 'client_name': client_name(v), 'expected': exp, 'basis': basis, 'kind': kind})
    return out, data_map, data_basis


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--objs', nargs='*', help='object directories (default build/<lib>/obj of every library)')
    ap.add_argument('--root', default=REPO, help='the tree whose build/ is read (default this one)')
    ap.add_argument('--only', help='rows whose relocation symbol contains this text')
    ap.add_argument('--log', default=os.path.join(REPO, '.work', 'log', 'relocaudit.log'))
    ap.add_argument('--json', default=os.path.join(REPO, '.work', 'log', 'relocaudit.json'))
    a = ap.parse_args(argv)
    dirs = a.objs or [os.path.join(a.root, 'build', lib, 'obj') for lib in LIBS]
    paths = sorted(p for d in dirs for p in glob.glob(os.path.join(d, '*.obj')))
    out, data_map, basis = audit(paths)
    if a.only:
        out = [r for r in out if a.only in r['reloc'] or a.only in r['reloc_name']]
    out.sort(key=lambda r: (r['function'], r['offset']))
    for f in (a.log, a.json):
        os.makedirs(os.path.dirname(os.path.abspath(f)), exist_ok=True)
    with open(a.log, 'w', encoding='utf-8') as f:
        for r in out:
            f.write('0x%06X  %-40s %s+0x%X %s  %s  client 0x%06X%s  expected %s (%s) [%s]\n' % (
                r['function'], r['name'][:40], os.path.basename(r['object']), r['offset'], r['type'], r['reloc_name'],
                r['client'], ' = ' + r['client_name'] if r['client_name'] else '',
                '0x%06X' % r['expected'] if r['expected'] is not None else '-', r['basis'], r['kind']))
    with open(a.json, 'w', encoding='utf-8') as f:
        json.dump({'rows': out, 'data_map': {k: '0x%06X' % v for k, v in sorted(data_map.items())},
                   'data_basis': basis}, f, indent=1)
    funcs = sorted({r['function'] for r in out})
    kinds = collections.Counter(r['kind'] for r in out)
    kfuncs = {k: len({r['function'] for r in out if r['kind'] == k}) for k in kinds}
    benign = ('fold', 'assert-text')
    real = sorted({r['function'] for r in out if r['kind'] not in benign})
    print('relocaudit: %d objects, %d rows in %d exact functions (%s); %d functions outside %s; %d data symbols '
          'mapped  (log %s)' % (len(paths), len(out), len(funcs),
                               ', '.join('%s %d/%d' % (k, kinds[k], kfuncs[k]) for k in sorted(kinds)),
                               len(real), '/'.join(benign), len(data_map), a.log))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
