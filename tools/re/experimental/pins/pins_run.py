"""M4: offline objmatch reruns on a full verify's objects: census of ties and the evidence-pin kinds."""
import os
import collections, csv, json, os, sys, glob, time
sys.path.insert(0, r'C:/EnBDevelopment/EnB/.work/m4_dummy')
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objmatch_pins as om
ROOT = r'C:/EnBDevelopment/EnB/.work/wt/pins'
WORK = os.path.join(ROOT, '.work/pins/tmp/enb_import')
OUT = os.path.join(ROOT, '.work/m4')
REGION = {'app': (0x401000, 0x8C1B50)}

def key_of(f):
    return os.path.splitext(os.path.basename(f))[0].lower()

state = json.load(open(os.path.join(ROOT, 'docs/data/imports.json')))
libs = [l for l in state if os.path.isdir(os.path.join(WORK, f'verify-{l}'))]
img = om.ClientImage()

def run_all(extra=None, **ctx):
    om.EXTRA = extra
    allres, ranges = {}, []
    for lib in libs:
        om.CTX.update(ctx); om.CTX['lib'] = lib
        objs = sorted(glob.glob(os.path.join(WORK, f'verify-{lib}', '*')))
        allres[lib] = om.match_objects(img, objs, REGION.get(lib))
        for o, (lo, hi) in om.CTX['last_ranges'].items():
            ranges.append((lo, hi, (lib, o)))
    return allres, ranges

def pairs(allres):
    out = set()
    for lib, rs in allres.items():
        for r in rs:
            if r['status'] == 'exact':
                out.add((lib, key_of(r['object']), r['addresses'][0]))
    return out

t = time.time()
base, ranges = run_all(None)
bp = pairs(base)
vp = set()
for lib in libs:
    for k, f in state[lib]['files'].items():
        for a in f.get('in_tree', []):
            vp.add((lib, k, a))
print('baseline', len(bp), 'verify', len(vp), 'only offline', len(bp - vp), 'only verify', len(vp - bp), f'{time.time()-t:.0f}s', flush=True)
claimed = collections.defaultdict(set)
for lib, rs in base.items():
    for r in rs:
        if r['status'] == 'exact':
            claimed[int(r['addresses'][0], 16)].add(r['symbol'])
# census
census = []
rmap = {(lib, o): (lo, hi) for lo, hi, (lib, o) in ranges}
for lib, rs in base.items():
    for r in rs:
        if r['status'] == 'multiple':
            hits = r['_hits']
            lo, hi = rmap.get((lib, r['object']), (None, None))
            census.append({'lib': lib, 'object': r['object'], 'symbol': r['symbol'], 'name': r['name'],
                           'count': len(hits), 'identical': r['identical'], 'hits': [f'0x{h:06X}' for h in hits],
                           'range': [f'0x{lo:06X}', f'0x{hi:06X}'] if lo else None,
                           'claimed': [f'0x{h:06X}' for h in hits if claimed.get(h, set()) - {r['symbol']}]})
json.dump(census, open(os.path.join(OUT, 'census.json'), 'w'), indent=1)
print('census', len(census), flush=True)
pins = {}
pf = os.path.join(ROOT, 'docs/data/pins.csv')
if os.path.exists(pf):
    for row in csv.DictReader(open(pf)):
        k = (row['file'].lower(), row['symbol'])
        if k in pins:
            print('pin refused (second row for one symbol):', row['symbol'], row['address']); continue
        pins[k] = int(row['address'], 16)
kinds = [('caller', {}), ('callerlib', {}), ('place1000', {'N': 0x1000}), ('place4000', {'N': 0x4000}), ('pin', {})]
if len(sys.argv) > 1:
    kinds = [k for k in kinds if k[0] in sys.argv[1:]]
report = {}
for name, extra in kinds:
    t = time.time()
    kind = 'place' if name.startswith('place') else name
    res, _ = run_all(kind, claimed=claimed, ranges=ranges, pins=pins, **extra)
    p = pairs(res)
    credits = []
    for lib, rs in res.items():
        for r in rs:
            if r.get('extra'):
                credits.append({'lib': lib, 'object': r['object'], 'symbol': r['symbol'], 'name': r['name'],
                                'address': r['addresses'][0], 'count': len(r['_hits']),
                                'twin_identical': r.get('_twin_identical'),
                                'cands': [f'0x{h:06X}' for h in r['_hits']]})
    gained, lost = sorted(p - bp), sorted(bp - p)
    report[name] = {'gained': gained, 'lost': lost, 'credits': credits}
    per = collections.Counter(g[0] for g in gained)
    print(name, 'gained', len(gained), dict(per), 'lost', len(lost), 'direct credits', len(credits),
          'twin-identical', sum(1 for c in credits if c['twin_identical']), f'{time.time()-t:.0f}s', flush=True)
    json.dump(report, open(os.path.join(OUT, 'kinds_' + '_'.join(k for k, _ in kinds) + '.json'), 'w'), indent=1)
