"""Name hints for the generated aux-data classes (M1.3, D56): matches each client compound
(by vtable) to Net-7's C++ AuxClasses (C:/EnBDevelopment/Net7/Net7/AuxClasses) and
the reference decoder's typed C# classes (src/Shared/the reference game model/Objects) by structure, never by guess:
each root is walked top-down; a child compound at index i takes the class of the Net-7 member
`X.Init(i, this, ...)` and of the C# property `[AuxField(i)]`; a match is confirmed when the child
counts agree (Net-7's Construct(.., N, ..), the C# field list) and, for C#, the field names and
their order agree. Read-only on both repositories.

    python tools/re/auxnames.py      (after auxroots.py has written .work/auxroots.json)

Writes tools/re/auxroots_names.json (vtable -> Net-7 name, C# name, evidence), which
auxroots.py uses for the class names (D24 Westwood style: Delta + Net-7's name without Aux +
Class; roots Delta + the client's root string + Class) and their `// NAME n:` tags.
"""
import glob, json, os, re, collections

N7 = r'C:\EnBDevelopment\Net7\Net7\AuxClasses'
CS = r'C:\EnBDevelopment\the reference decoder\src\Shared\the reference game model\Objects'
EXT = r'C:\EnBDevelopment\EnB\.work\auxroots.json'

# ---- Net-7
n7 = {}
text = {}
for p in glob.glob(os.path.join(N7, '*.h')) + glob.glob(os.path.join(N7, '*.cpp')):
    text[p] = open(p, encoding='latin-1').read()
classes = {}
for p, t in text.items():
    for m in re.finditer(r'class\s+(Aux\w+)\s*:\s*public\s+AuxBase', t):
        classes[m.group(1)] = {'count': None, 'members': {}, 'init': {}, 'file': os.path.basename(p)}
for name, c in classes.items():
    base = c['file'][:-2]
    body = ''.join(t for p, t in text.items() if os.path.basename(p)[:-2].split('.')[0] in (base, base.replace('.', '')))
    body = text[os.path.join(N7, c['file'])] + text.get(os.path.join(N7, c['file'][:-2] + '.cpp'), '')
    m = re.search(r'Construct\(\s*Flags\s*,\s*ExtendedFlags\s*,\s*(\d+)', body)
    c['count'] = int(m.group(1)) if m else None
    for m in re.finditer(r'class\s+(Aux\w+)\s+(\w+)\s*(\[\s*\d+\s*\])?\s*;', body):
        c['members'][m.group(2)] = (m.group(1), bool(m.group(3)))
    for m in re.finditer(r'(\w+)(\[\w+\])?\.Init\(\s*(\w+)\s*,\s*this', body):
        mem, arr, idx = m.groups()
        if idx.isdigit():
            c['init'][int(idx)] = mem
        elif arr:
            c['init']['*'] = mem

# ---- C#
cs = {}
for p in glob.glob(os.path.join(CS, '*.cs')):
    t = open(p, encoding='utf-8').read()
    for m in re.finditer(r'\[AuxClass(?:\("(\w+)"\))?\]\s*public\s+(?:abstract\s+|sealed\s+)?partial\s+class\s+(\w+)(.*?)(?=\n\[AuxClass|\Z)', t, re.S):
        root, cname, body = m.groups()
        fields = {}
        for f in re.finditer(r'\[AuxField\((\d+)([^\]]*)\)[^\]]*\]\s*public\s+partial\s+([\w<>\[\]?]+)\s+(\w+)', body):
            idx, extra, typ, prop = f.groups()
            nm = re.search(r'Name\s*=\s*"(\w+)"', extra)
            fields[int(idx)] = (typ, nm.group(1) if nm else prop)
        cs[cname] = {'root': root, 'fields': fields}
cs_root = {v['root']: k for k, v in cs.items() if v['root']}

# ---- walk
doc = json.load(open(EXT))
rows = collections.OrderedDict()
N7_ROOT = {'Ship': 'AuxShipIndex', 'Player': 'AuxPlayerIndex', 'Hulk': 'AuxHulkIndex',
           'ManufacturingLab': 'AuxManufacturingIndex', 'Harvestable': 'AuxHarvestable'}


def leafnames(n):
    return [f['name'] for f in n.get('fields', [])]


def walk(node, path, n7c, csc):
    t = node['type']
    vt = node.get('vt') or '-'
    kids = node['fields'] if t == 'struct' else [node['element']]
    count = len(node['fields']) if t == 'struct' else node.get('count')
    ev = []
    n7info = classes.get(n7c) if n7c else None
    if n7info:
        ok = n7info['count'] == count
        ev.append(f'Net-7 Construct {n7info["count"]} children' + (' = ours' if ok else f' != ours {count}'))
    csinfo = cs.get(csc) if csc else None
    if csinfo and t == 'struct':
        names = [csinfo['fields'].get(i, (None, None))[1] for i in range(len(kids))]
        same = names == [k['name'] for k in kids] and len(csinfo['fields']) == len(kids)
        ev.append('C# fields ' + ('= ours (same names, same order)' if same else f'differ ({len(csinfo["fields"])} vs {len(kids)})'))
    key = (vt, path.split('.')[0] if vt == '-' else vt)
    r = rows.setdefault(vt if vt != '-' else path, {'paths': [], 'n7': set(), 'cs': set(), 'ev': set(), 'count': count, 'type': t})
    r['paths'].append(path)
    if n7c: r['n7'].add(n7c)
    if csc: r['cs'].add(csc)
    r['ev'].update(ev)
    if t == 'array':
        el = node['element']
        if el['type'] in ('struct', 'array'):
            n7e = None
            if n7info and '*' in n7info['init']:
                n7e = n7info['members'].get(n7info['init']['*'], (None,))[0]
            cse = None
            walk(el, path + '.#', n7e, csc_elem.get(path))
        return
    for i, k in enumerate(kids):
        if k['type'] not in ('struct', 'array'):
            continue
        n7k = None
        if n7info and i in n7info['init']:
            n7k = n7info['members'].get(n7info['init'][i], (None,))[0]
        csk = None
        if csinfo and i in csinfo['fields']:
            typ = csinfo['fields'][i][0]
            mm = re.match(r'AuxArray<(\w+)\??>', typ)
            if mm:
                csc_elem[path + '.' + k['name']] = mm.group(1) if mm.group(1) in cs else None
                csk = None
            else:
                csk = typ.rstrip('?') if typ.rstrip('?') in cs else None
        walk(k, path + '.' + k['name'], n7k, csk)


csc_elem = {}
for rname, r in doc['roots'].items():
    walk(r, rname, N7_ROOT.get(rname), cs_root.get(rname))

MANUAL = {   # Net-7 elements declared without a numbered Init (checked by hand, 2026-10-24)
    '0xb01444': ('AuxPrimaryCategory', 'Net-7 Construct 2 = ours; AuxPrimaryCategories holds PrimaryCategory[2] (Init(0), Init(1))'),
    '0xb014fc': ('AuxCategories', 'Net-7 Construct 5 = ours; AuxPrimaryCategory.Categories.Init(1) = our child 1'),
    '0xb014a0': ('AuxCategory', 'Net-7 Construct 3 = ours; AuxCategories holds Category[5]'),
    '0xb01558': ('AuxSubCategories', 'Net-7 Construct 5 = ours; AuxCategory.SubCategories.Init(1) = our child 1'),
    '0xb01be8': ('AuxSubCategory', 'Net-7 Construct 3 = ours; AuxSubCategories holds SubCategory[5]'),
    '0xb01f74': ('AuxElement', 'Net-7 Construct 5 = ours; the element of AuxElements (Net-7 sizes that 4, the client 5)'),
}
table = {}
for vt, r in rows.items():
    n7 = '/'.join(sorted(r['n7']))
    ev = sorted(r['ev'])
    if vt in MANUAL:
        n7 = MANUAL[vt][0]
        ev = [MANUAL[vt][1]] + ev
    table[vt] = {'net7': n7 or None, 'cs': '/'.join(sorted(r['cs'])) or None, 'kind': r['type'],
                 'children': r['count'], 'path': r['paths'][0], 'evidence': '; '.join(ev)}
OUTJ = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'auxroots_names.json')
with open(OUTJ, 'w', newline='\n') as f:
    json.dump(table, f, indent=1, sort_keys=True)
    f.write('\n')
print(len(table), 'compounds ->', OUTJ)
