"""The signature audit: retail's decorated name of every thunk-order slot against the name our object gives the
same address, field by field (progress.md 2026-11-27 night plan, task 2).

    python tools/re/sigaudit.py [--objs build] [--state docs/data/thunk-names.tsv] [--refresh] [--out docs/data]

For every slot on the thunk order with a confirmed or probable name (docs/data/thunk-names.tsv) retail's name is the
state's; ours is the symbol our object gives the slot's target:
  exact    the ledger's (file, address) pair (docs/data/imports): the object's function the byte match puts there
           (objmatch over build/<lib>/obj, cached in .work/sigaudit/; --refresh after a rebuild)
  not      symbols.json's name at the target (a vtable, named or strings entry), the file the object defining that
           symbol, when one does
Both are decoded (thunkname.parse, the one decorated-name grammar of tools/re) into access, static / virtual,
const `this`, calling convention, return type and each parameter (argument back-references resolved), and every
field that differs is a row of signature-audit.tsv. Then:
  families  vt.families (client vtables, per index): a confirmed on-order virtual against every other entry of its
            family at that index (the state's name and ours): rows where the override disagrees (method, const,
            callconv, return, parameters)
  census    the W4 enum names, struct (U) against class (V), the parameter / return classes retail names that our
            objects never spell or that ours spells differently at the same place, each with its slot count
Writes signature-audit.tsv (rows), signature-audit-families.tsv, signature-audit-census.tsv and signature-audit.md
(the summary; generated: never edit by hand). Rerun after the renames land: one command, after a build.
"""
import argparse
import collections
import csv
import glob
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
import thunkname  # noqa: E402

DATA = os.path.join(REPO, 'docs', 'data')
STATE = os.path.join(DATA, 'thunk-names.tsv')
CACHE = os.path.join(REPO, '.work', 'sigaudit')
PURE = 0x9CCD10

# ---- decoding ------------------------------------------------------------------------------------------------

ACCESS = {'A': ('private', 'normal'), 'E': ('private', 'virtual'), 'C': ('private', 'static'),
          'I': ('protected', 'normal'), 'M': ('protected', 'virtual'), 'K': ('protected', 'static'),
          'Q': ('public', 'normal'), 'U': ('public', 'virtual'), 'S': ('public', 'static'),
          'Y': ('global', 'global')}
CV = {'A': '', 'B': 'const', 'C': 'volatile', 'D': 'const volatile'}
CONV = {'A': '__cdecl', 'E': '__thiscall', 'G': '__stdcall', 'I': '__fastcall'}
PRIM = {'C': 'signed char', 'D': 'char', 'E': 'unsigned char', 'F': 'short', 'G': 'unsigned short', 'H': 'int',
        'I': 'unsigned int', 'J': 'long', 'K': 'unsigned long', 'M': 'float', 'N': 'double', 'O': 'long double',
        'X': 'void', 'Z': '...'}
PRIM2 = {'_J': '__int64', '_K': 'unsigned __int64', '_N': 'bool', '_W': 'wchar_t'}
KEY = {'V': 'class', 'U': 'struct', 'T': 'union', 'W': 'enum'}


def _type_end(s, i):
    p = thunkname.Parser(s)
    p.i = i
    p.type()
    return p.i


def _qual_end(s, i):
    p = thunkname.Parser(s)
    p.i = i
    p.qualname()
    return p.i


def qual_text(q):
    """A qualname's text ('Foo@Bar@@' or '?$T@VX@@@@' ...) as C++ (Bar::Foo, T<class X>)."""
    parts, i = [], 0
    while i < len(q) and q[i] != '@':
        if q[i].isdigit():
            parts.append('#' + q[i])
            i += 1
        elif q.startswith('?$', i):
            p = thunkname.Parser(q)
            p.i = i + 2
            j = q.index('@', i + 2)
            name = q[i + 2:j]
            p.i = j + 1
            args = []
            while p.peek() != '@':
                a = p.i
                p.template_arg()
                args.append(type_text(q[a:p.i]) if not q.startswith('$', a) else q[a:p.i])
            p.take()
            parts.append(f'{name}<{",".join(args)}>')
            i = p.i
        else:
            j = q.index('@', i)
            parts.append(q[i:j])
            i = j + 1
    return '::'.join(reversed(parts))


def type_text(t):
    """One decorated type as C++ (best effort, display only; the comparison uses the decorated text)."""
    try:
        return _type_text(t)
    except Exception:
        return t


def _type_text(t):
    c = t[0]
    if c in PRIM:
        return PRIM[c]
    if t[:2] in PRIM2:
        return PRIM2[t[:2]]
    if c.isdigit():
        return '#' + c
    if c in 'PQRS' and t[1:2] == '6':
        return 'function pointer ' + t[2:]
    if c in 'PQRSAB':
        cv = CV.get(t[1], '?')
        inner = _type_text(t[2:])
        mark = '&' if c in 'AB' else '*'
        return (cv + ' ' if cv else '') + inner + ' ' + mark
    if c in 'TUV':
        return KEY[c] + ' ' + qual_text(t[1:])
    if c == 'W':
        return f'enum({t[1]}) ' + qual_text(t[2:])
    if c == '?':
        cv = CV.get(t[1], '?')
        return (cv + ' ' if cv else '') + _type_text(t[2:])
    return t


def resolve_args(args):
    """Argument texts with MSVC's back-references (a digit = the n-th multi-character argument) spelled out."""
    table, out = [], []
    for a in args:
        if len(a) == 1 and a.isdigit():
            n = int(a)
            out.append(table[n] if n < len(table) else a)
            continue
        if len(a) > 1 and a not in table and len(table) < 10:
            table.append(a)
        out.append(a)
    return out


def decode(name):
    """{qual, method, cls, access, kind, const, callconv, ret, args, ellipsis} of a decorated function name, or
    None (not decorated, or outside thunkname's grammar: an adjustor thunk, a template function ...)."""
    if not name:
        return None
    if name.startswith('j_'):
        name = name[2:]
    p = thunkname.parse(name)
    if not p:
        return None
    code = name[p['code']]
    access, kind = ACCESS[code]
    const = CV.get(name[p['cv']], '?') if p['cv'] is not None else None
    conv = CONV.get(name[p['conv']], name[p['conv']])
    ret = name[slice(*p['ret'])] if p['ret'] else '@'
    args = resolve_args([name[a:b] for a, b in p['args']])
    method = name[p['first'][0]:p['first'][1]]
    if method.startswith('?'):
        method = '?' + method[1:].rstrip('@')
    else:
        method = method.rstrip('@')
    qual = name[p['name'][0]:p['name'][1]]
    cls = qual[len(name[p['first'][0]:p['first'][1]]):]
    return {'qual': qual, 'method': method, 'cls': qual_text(cls) if cls else '', 'access': access, 'kind': kind,
            'const': const, 'callconv': conv, 'ret': ret, 'args': args, 'ellipsis': p['end'] == 'Z'}


def show(field, v):
    if field in ('ret',) or field.startswith('param'):
        return v if v in ('@', '-') else f'{v} ({type_text(v)})'
    if isinstance(v, bool):
        return 'yes' if v else 'no'
    return '' if v is None else str(v)


def type_kind(ours, retail):
    """What a differing type text is: int, ptrcv, ptrref, classkey, class (only a spelled name differs), other."""
    for kind, new in thunkname.type_edits(ours):
        if new == retail:
            return kind
    for kind, mid in thunkname.type_edits(ours):
        for kind2, new in thunkname.type_edits(mid):
            if new == retail and {kind, kind2} <= {'ptrcv', 'ptrref'}:
                return 'ptrref+cv'
    if _CLS.sub(r'N@', ours) == _CLS.sub(r'N@', retail):
        return 'class'
    return 'other'


_CLS = re.compile(r'((?:^|(?<=[A-Z0-9@?$]))(?:W[0-7]|[VUT]))[A-Za-z_][A-Za-z0-9_]*@')


def compare(o, r):
    """[(field, ours, retail, kind)] of decoded names o (ours) and r (retail)."""
    rows = []
    if o['qual'] != r['qual']:
        if o['method'] == r['method']:
            kind = 'class'
        elif {o['method'], r['method']} == {'?_G', '?_E'}:
            kind = 'gE'
        else:
            kind = 'method'
        rows.append(('name', o['qual'], r['qual'], kind))
    for f in ('access', 'kind', 'const', 'callconv'):
        if o[f] != r[f]:
            rows.append((f, o[f], r[f], f))
    if o['ret'] != r['ret']:
        rows.append(('ret', o['ret'], r['ret'], type_kind(o['ret'], r['ret'])))
    oa, ra = o['args'], r['args']
    if oa == ['X']:
        oa = []
    if ra == ['X']:
        ra = []
    if len(oa) != len(ra):
        rows.append(('nparams', len(oa), len(ra), 'nparams'))
    for i in range(min(len(oa), len(ra))):
        if oa[i] != ra[i]:
            rows.append((f'param{i + 1}', oa[i], ra[i], type_kind(oa[i], ra[i])))
    if o['ellipsis'] != r['ellipsis']:
        rows.append(('ellipsis', o['ellipsis'], r['ellipsis'], 'ellipsis'))
    return rows


def class_refs(t):
    """[(key letter, name)] of the classes / structs / unions / enums a decorated type text spells out."""
    out = []
    for m in re.finditer(r'(?:(?<=^)|(?<=[A-Z0-9@?$]))(W[0-7]|[VUT])([A-Za-z_][A-Za-z0-9_]*)@', t):
        out.append((m.group(1)[0], m.group(2)))
    return out


def class_refs_all(name):
    """class_refs over a whole decorated name (its qualifiers' classes included)."""
    out = class_refs(name)
    d = decode(name)
    if d and d['cls']:
        out += [('V', x) for x in re.findall(r'[A-Za-z_][A-Za-z0-9_]*', d['cls'])]
    return out


# ---- inputs --------------------------------------------------------------------------------------------------

def load_state(path):
    rows = []
    with open(path, newline='') as f:
        for r in csv.DictReader(f, delimiter='\t'):
            g = (r.get('grade') or '').split(':')[0].strip()
            rows.append({'slot': int(r['slot'], 16), 'target': int(r['target'], 16), 'name': r['name'],
                         'on': r['on_order'] == 'on', 'grade': g})
    return rows


def objmatch_results(objroot, libs, refresh):
    """{lib: objmatch results} over build/<lib>/obj (cached per library by the objects' newest mtime)."""
    from clientimage import ClientImage
    from objmatch import match_objects, DEFAULT_CLIENT
    import libdefs
    os.makedirs(CACHE, exist_ok=True)
    img, out = None, {}
    for lib in libs:
        objs = sorted(glob.glob(os.path.join(objroot, lib, 'obj', '*.obj')))
        if not objs:
            continue
        stamp = max(os.path.getmtime(p) for p in objs)
        cp = os.path.join(CACHE, f'objmatch-{lib}.json')
        if not refresh and os.path.exists(cp):
            c = json.load(open(cp))
            if c.get('stamp') == stamp and c.get('n') == len(objs):
                out[lib] = c['results']
                continue
        if img is None:
            img = ClientImage(DEFAULT_CLIENT)
        app = libdefs.APP_LIBS.get(lib)
        print(f'objmatch {lib}: {len(objs)} objects', flush=True)
        res = match_objects(img, objs, app and app['region'])
        keep = [{'object': r['object'], 'symbol': r['symbol'], 'status': r['status'],
                 'addresses': r['addresses'][:1]} for r in res if r['status'] == 'exact']
        json.dump({'stamp': stamp, 'n': len(objs), 'results': keep}, open(cp, 'w'), indent=0)
        out[lib] = keep
    return out


def object_defs(objroot, libs):
    """{symbol: 'lib/key'} of every function our objects define (the first object, by library order)."""
    from objmatch import CoffObject
    defs = {}
    for lib in libs:
        for p in sorted(glob.glob(os.path.join(objroot, lib, 'obj', '*.obj'))):
            key = os.path.splitext(os.path.basename(p))[0].lower()
            try:
                o = CoffObject(p)
            except Exception:
                continue
            for n, *_ in o.functions():
                defs.setdefault(n, f'{lib}/{key}')
    return defs


def ours_by_address(objroot, refresh):
    """{address: (symbol, file, exact)}: the ledger's exact pairs with the object's symbol there, then
    symbols.json's other names (not exact)."""
    import ledger
    from libdefs import key_of
    led = ledger.load()
    libs = list(led)
    res = objmatch_results(objroot, libs, refresh)
    out = {}
    for lib in libs:
        pairs = {}
        for key, f in led[lib].get('files', {}).items():
            for a in f.get('in_tree', []):
                pairs[int(a, 16)] = (key, f.get('name', key))
        for r in res.get(lib, []):
            a = int(r['addresses'][0], 16)
            k = key_of(r['object'])
            if a in pairs and pairs[a][0] == k:
                out.setdefault(a, (r['symbol'], f'{lib}/{pairs[a][1]}', True))
    defs = object_defs(objroot, libs)
    for x in json.load(open(os.path.join(DATA, 'symbols.json'))):
        if x.get('kind') != 'function' or x['source'] == 'thunk':
            continue
        a = int(x['address'], 16)
        if a in out:
            continue
        if x['source'] == 'exact':
            # an exact name outside our ledger: the Microsoft / EA libraries' byte matches (bytematch-*.json)
            out[a] = (x['symbol'], 'lib:' + x['source'], True) if x['symbol'] not in defs else \
                (x['symbol'], defs[x['symbol']], False)
        else:
            out[a] = (x['symbol'], defs.get(x['symbol'], '-'), False)
    out[None] = set(defs)
    return out


# ---- the three reports ---------------------------------------------------------------------------------------

def audit(state, ours):
    rows, slots, undecoded = [], [], collections.Counter()
    for s in state:
        if not s['on'] or s['grade'] not in ('confirmed', 'probable'):
            continue
        r = decode(s['name'])
        o = ours.get(s['target'])
        rec = {'slot': s['slot'], 'target': s['target'], 'grade': s['grade'], 'retail': s['name'],
               'ours': o[0] if o else None, 'file': o[1] if o else '-', 'exact': bool(o and o[2]), 'diff': []}
        slots.append(rec)
        if not r:
            undecoded['retail'] += 1
            continue
        if not o:
            rec['diff'] = [('ours', '-', s['name'], 'missing')]
            continue
        d = decode(o[0])
        if not d:
            undecoded['ours'] += 1
            rec['diff'] = [('ours', o[0], s['name'], 'undecoded')]
            continue
        rec['diff'] = compare(d, r)
        rec['dec'] = (d, r)
    for rec in slots:
        for field, ov, rv, kind in rec['diff']:
            rows.append({'address': f"0x{rec['target']:06X}", 'file': rec['file'],
                         'exact': 'exact' if rec['exact'] else 'not', 'field': field, 'ours': show(field, ov),
                         'retail': show(field, rv), 'grade': rec['grade'], 'kind': kind,
                         'slot': f"0x{rec['slot']:06X}", 'ours_name': rec['ours'] or '-',
                         'retail_name': rec['retail']})
    return slots, rows, undecoded


SIG = ('const', 'callconv', 'ret', 'args', 'ellipsis')


def families_audit(state, ours, vts=None):
    """Rows where an entry of a vtable family at index k disagrees with a confirmed on-order virtual there."""
    from thunknames.vt import vtables, families
    if vts is None:
        vts = [(a, s, e) for a, s, e in vtables() if s.endswith('@@6B@')]
    by_slot = {s['slot']: s for s in state}
    fam = collections.defaultdict(list)
    for (vi, k), root in families(vts, PURE).items():
        fam[k, root].append(vi)
    rows, seen = [], set()
    for (k, root), members in sorted(fam.items()):
        ents = sorted({vts[v][2][k] for v in members} - {PURE})
        if len(ents) < 2:
            continue
        refs = [e for e in ents if e in by_slot and by_slot[e]['on'] and by_slot[e]['grade'] == 'confirmed']
        for ref in refs:
            rd = decode(by_slot[ref]['name'])
            if not rd or rd['kind'] != 'virtual':
                continue
            for e in ents:
                if e == ref:
                    continue
                st = by_slot.get(e)
                cands = []
                if st and st['name']:
                    cands.append(('retail' if st['on'] else 'state-off', st['name'], st['grade'] or '-'))
                o = ours.get(st['target']) if st else None
                if o:
                    cands.append(('ours', o[0], 'exact' if o[2] else 'not'))
                for side, nm, g in cands:
                    od = decode(nm)
                    if not od:
                        continue
                    for field, ov, rv, kind in compare(od, rd):
                        if field == 'name':
                            if od['method'] == rd['method'] or (od['method'] in ('?_G', '?_E', '?1')
                                                                 and rd['method'] in ('?_G', '?_E', '?1')):
                                continue
                            field, ov, rv = 'method', od['method'], rd['method']
                        if field in ('access',):
                            continue           # an override may change its access
                        key = (e, side, field, str(ov), str(rv))
                        if key in seen:
                            continue
                        seen.add(key)
                        rows.append({'index': k, 'ref_slot': f'0x{ref:06X}', 'ref_name': by_slot[ref]['name'],
                                     'slot': f'0x{e:06X}', 'target': f"0x{st['target']:06X}" if st else '-',
                                     'side': side, 'grade': g, 'field': field, 'override': show(field, ov),
                                     'confirmed': show(field, rv), 'name': nm})
    return rows


def census(slots, ours_all=None):
    """[(section, name, retail slots, ours slots, note)]; `ours_all` (optional): every decorated name our objects
    define, so `lacking` means no symbol of ours spells the class at all (else: no audited slot of ours)."""
    enum_r, enum_o = collections.Counter(), collections.Counter()
    key_r = collections.defaultdict(collections.Counter)      # name -> {key letter: slots}
    key_o = collections.defaultdict(collections.Counter)
    spelled = collections.Counter()                           # (ours name, retail name) at the same place
    keydiff = collections.Counter()                           # (name, ours key, retail key)
    retail_cls = collections.Counter()
    ours_names = set()
    for rec in slots:
        r = decode(rec['retail'])
        if not r:
            continue
        rtypes = [r['ret']] + r['args']
        seen = set()
        for t in rtypes:
            for k, n in class_refs(t):
                if (k, n) in seen:
                    continue
                seen.add((k, n))
                key_r[n][k] += 1
                if k == 'W':
                    enum_r[n] += 1
                retail_cls[n] += 1
        o = decode(rec['ours']) if rec['ours'] else None
        if not o:
            continue
        otypes = [o['ret']] + o['args']
        seen = set()
        for t in otypes:
            for k, n in class_refs(t):
                ours_names.add(n)
                if (k, n) in seen:
                    continue
                seen.add((k, n))
                key_o[n][k] += 1
                if k == 'W':
                    enum_o[n] += 1
        if len(otypes) != len(rtypes):
            continue
        for ot, rt in zip(otypes, rtypes):
            if ot == rt:
                continue
            oc, rc = class_refs(ot), class_refs(rt)
            if len(oc) == len(rc):
                for (ok, on), (rk, rn) in zip(oc, rc):
                    if on != rn:
                        spelled[on, rn] += 1
                    elif ok != rk:
                        keydiff[on, ok, rk] += 1
            elif not oc and len(rc) == 1 and rc[0][0] == 'W':
                spelled[f'({type_text(ot)})', rc[0][1]] += 1
    out = []
    for n, c in sorted(enum_r.items(), key=lambda x: (-x[1], x[0])):
        out.append(('enum', n, c, enum_o.get(n, 0), ''))
    for n, c in sorted(enum_o.items(), key=lambda x: (-x[1], x[0])):
        if n not in enum_r:
            out.append(('enum-ours-only', n, 0, c, ''))
    for (n, ok, rk), c in sorted(keydiff.items(), key=lambda x: (-x[1], x[0])):
        out.append(('struct-class', n, c, c, f'ours {KEY[ok]}, retail {KEY[rk]}'))
    for n, ks in sorted(key_r.items()):
        if 'U' in ks:
            out.append(('struct-retail', n, ks['U'], key_o[n].get('U', 0),
                        'retail struct' + (f', also class {ks["V"]}' if ks.get('V') else '')))
    for (on, rn), c in sorted(spelled.items(), key=lambda x: (-x[1], x[0])):
        out.append(('spelled', rn, c, c, f'ours {on}'))
    if ours_all is not None:
        for nm in ours_all:
            for k, n in class_refs_all(nm):
                ours_names.add(n)
    for n, c in sorted(retail_cls.items(), key=lambda x: (-x[1], x[0])):
        if n not in ours_names:
            out.append(('lacking', n, c, 0, 'no symbol of ours spells it' if ours_all is not None
                        else 'no slot of ours spells it'))
    return out


# ---- output --------------------------------------------------------------------------------------------------

def write_tsv(path, rows, cols):
    with open(path, 'w', newline='\n', encoding='utf-8') as f:
        f.write('\t'.join(cols) + '\n')
        for r in rows:
            f.write('\t'.join(str(r[c]).replace('\t', ' ') for c in cols) + '\n')


def folder(file):
    if file in ('-',) or file.startswith('lib:'):
        return file
    lib, _, rest = file.partition('/')
    d = os.path.dirname(rest)
    return f'{lib}/{d}' if d else lib


def summary(slots, rows, frows, crows, undecoded, head):
    n = len(slots)
    ex = sum(1 for s in slots if s['exact'])
    missing = sum(1 for s in slots if not s['ours'])
    same = sum(1 for s in slots if s['ours'] and not s['diff'])
    differ = [s for s in slots if s['diff']]
    lines = ['# Signature audit', '',
             f'Generated by `python tools/re/sigaudit.py` at {head}; never edit by hand. Retail: the thunk-order state\'s '
             'confirmed and probable on-order names (docs/data/thunk-names.tsv); ours: the symbol our object gives the '
             'slot\'s target (the ledger\'s exact pair) or symbols.json\'s name there (not exact). Rows: '
             '[signature-audit.tsv](signature-audit.tsv), the families [signature-audit-families.tsv]'
             '(signature-audit-families.tsv), the census [signature-audit-census.tsv](signature-audit-census.tsv).', '',
             '## Slots', '',
             '| | slots |', '|---|---|',
             f'| on the order, confirmed or probable | {n} |',
             f'| ours exact (a ledger pair) | {ex} |',
             f'| ours not exact (symbols.json) | {n - ex - missing} |',
             f'| no name of ours at the target | {missing} |',
             f'| retail name outside the grammar | {undecoded["retail"]} |',
             f'| ours outside the grammar | {undecoded["ours"]} |',
             f'| equal | {same} |',
             f'| differing (any field) | {len(differ)} |', '']
    byf = collections.Counter((r['field'] if not r['field'].startswith('param') else 'param', r['exact'])
                              for r in rows)
    fields = sorted({f for f, _ in byf}, key=lambda f: -sum(v for (g, _), v in byf.items() if g == f))
    lines += ['## Rows by field', '', '| field | exact | not | of which name-only kinds |', '|---|---|---|---|']
    name_only = collections.Counter(r['field'] if not r['field'].startswith('param') else 'param'
                                    for r in rows if r['kind'] in thunkname.NAME_ONLY or r['kind'] == 'class')
    for f in fields:
        lines.append(f'| {f} | {byf[f, "exact"]} | {byf[f, "not"]} | {name_only[f]} |')
    byk = collections.Counter((r['kind'], r['exact']) for r in rows)
    lines += ['', '## Rows by kind', '', '| kind | exact | not |', '|---|---|---|']
    for k in sorted({k for k, _ in byk}, key=lambda k: -(byk[k, 'exact'] + byk[k, 'not'])):
        lines.append(f'| {k} | {byk[k, "exact"]} | {byk[k, "not"]} |')
    bydir = collections.Counter((folder(r['file']), r['exact']) for r in rows)
    sdir = collections.Counter(folder(s['file']) for s in differ)
    lines += ['', '## Rows by folder', '', '| folder | slots differing | rows exact | rows not |', '|---|---|---|---|']
    for d in sorted({d for d, _ in bydir}, key=lambda d: -(bydir[d, 'exact'] + bydir[d, 'not'])):
        lines.append(f'| {d} | {sdir[d]} | {bydir[d, "exact"]} | {bydir[d, "not"]} |')
    fby = collections.Counter((r['field'] if not r['field'].startswith('param') else 'param', r['side'])
                              for r in frows)
    lines += ['', '## Vtable families', '',
              'An entry at index k of a family (vt.families) whose name disagrees with a confirmed on-order virtual '
              'at k (access left out: an override may change it). `retail`: the entry\'s own on-order name; '
              '`state-off`: its off-order state name; `ours`: our name at its target.', '',
              '| field | retail | state-off | ours |', '|---|---|---|---|']
    for f in sorted({f for f, _ in fby}):
        lines.append(f'| {f} | {fby[f, "retail"]} | {fby[f, "state-off"]} | {fby[f, "ours"]} |')
    cby = collections.Counter(r[0] for r in crows)
    lines += ['', '## Type census', '', '| section | names | retail slots |', '|---|---|---|']
    for sec in ('enum', 'enum-ours-only', 'struct-class', 'struct-retail', 'spelled', 'lacking'):
        lines.append(f'| {sec} | {cby[sec]} | {sum(r[2] for r in crows if r[0] == sec)} |')
    lines += ['', 'Sections: `enum` W4 names retail spells (retail slots, ours slots); `struct-class` a name ours and '
              'retail key differently at the same place; `struct-retail` names retail spells as struct (U); '
              '`spelled` a class (or an enum ours spells as a plain type) that ours spells differently at the same '
              'place (`ours ...`); `lacking` classes retail names that no slot of ours spells.', '']
    top = [r for r in crows if r[0] == 'spelled'][:15]
    if top:
        lines += ['Most frequent spelled-differently classes:', '', '| retail | ours | slots |', '|---|---|---|']
        lines += [f'| {r[1]} | {r[4][5:]} | {r[2]} |' for r in top]
        lines.append('')
    return '\n'.join(lines)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--objs', default=os.path.join(REPO, 'build'), help='the build root (build/<lib>/obj)')
    ap.add_argument('--state', default=STATE)
    ap.add_argument('--out', default=DATA)
    ap.add_argument('--refresh', action='store_true', help='rerun objmatch (else cached per library by mtime)')
    a = ap.parse_args()
    state = load_state(a.state)
    ours = ours_by_address(a.objs, a.refresh)
    slots, rows, undecoded = audit(state, ours)
    frows = families_audit(state, ours)
    crows = census(slots, ours.pop(None))
    cols = ['address', 'file', 'exact', 'field', 'ours', 'retail', 'grade', 'kind', 'slot', 'ours_name',
            'retail_name']
    write_tsv(os.path.join(a.out, 'signature-audit.tsv'), rows, cols)
    write_tsv(os.path.join(a.out, 'signature-audit-families.tsv'), frows,
              ['index', 'ref_slot', 'ref_name', 'slot', 'target', 'side', 'grade', 'field', 'override', 'confirmed',
               'name'])
    write_tsv(os.path.join(a.out, 'signature-audit-census.tsv'),
              [dict(zip(('section', 'name', 'retail_slots', 'ours_slots', 'note'), r)) for r in crows],
              ['section', 'name', 'retail_slots', 'ours_slots', 'note'])
    import subprocess
    try:
        head = subprocess.run(['git', 'rev-parse', '--short', 'HEAD'], cwd=REPO, capture_output=True,
                              text=True).stdout.strip()
    except OSError:
        head = '?'
    with open(os.path.join(a.out, 'signature-audit.md'), 'w', newline='\n', encoding='utf-8') as f:
        f.write(summary(slots, rows, frows, crows, undecoded, head) + '\n')
    print(f'slots {len(slots)}, differing {sum(1 for s in slots if s["diff"])}, rows {len(rows)}, '
          f'family rows {len(frows)}, census rows {len(crows)}')


if __name__ == '__main__':
    main()
