"""M4: false-credit audit of the evidence-pin kinds: callers/refs (client.py's logic) and placement."""
import os
import json, random, re, struct, sys, collections, bisect
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from clientimage import ClientImage
from client import Names
img = ClientImage(); names = Names(img)
start, end, raw = img.section('.text')
body = img.data[raw:raw + (end - start)]
calls = collections.defaultdict(list)
TH = collections.defaultdict(list)

for m in re.finditer(rb'[\xE8\xE9]', body):
    o = m.start()
    if o + 5 > len(body): break
    dst = (start + o + 5 + struct.unpack_from('<i', body, o + 1)[0]) & 0xFFFFFFFF
    if (start + o) in names.thunks: continue
    calls[names.thunks.get(dst, dst)].append(start + o)
def sites(a):
    out = list(calls.get(a, []))
    needles = [struct.pack('<I', a)] + [struct.pack('<I', th) for th, d in names.thunks.items() if d == a]
    for sec in img.sections:
        s, e, r = img.section(sec)
        blob = img.data[r:r + (e - s)]
        for needle in needles:
            i = blob.find(needle)
            while i >= 0:
                out.append(s + i); i = blob.find(needle, i + 1)
    return out
census = {(e['lib'], e['object'], e['symbol']): e for e in json.load(open('.work/m4/census.json'))}
kinds = json.load(open('.work/m4/kinds_caller_callerlib_place1000_place4000.json'))
try: kinds.update(json.load(open('.work/m4/kinds_pin.json')))
except OSError: pass
def dist(h, lo, hi): return 0 if lo <= h <= hi else (lo - h if h < lo else h - hi)
def support(a, lo, hi):
    own = [s for s in sites(a) if lo - 0x1000 <= s <= hi + 0x1000]
    return own
SYM = collections.defaultdict(set)
for x in json.load(open('docs/data/symbols.json')):
    if x.get('kind') == 'function': SYM[int(x['address'], 16)].add(x['symbol'])
random.seed(1)
allconf = {}
summary = {}
for kind, v in kinds.items():
    cr = v['credits']
    allconf[kind] = sum(1 for c in cr if [x for x in SYM.get(int(c['address'], 16), ()) if x != c['symbol']])
    samp = cr if len(cr) <= 20 else random.sample(cr, 20)
    doubt = 0
    print('==', kind, len(cr), 'credits; sample', len(samp))
    for c in samp:
        e = census[(c['lib'], c['object'], c['symbol'])]
        lo, hi = (int(x, 16) for x in e['range']) if e['range'] else (0, 0)
        a = int(c['address'], 16)
        cands = [int(x, 16) for x in e['hits']]
        sup = support(a, lo, hi)
        others = [b for b in cands if b != a]
        o_sup = [b for b in others[:8] if support(b, lo, hi)]
        nearer = [b for b in others if dist(b, lo, hi) < dist(a, lo, hi)]
        conflict = [x for x in SYM.get(a, ()) if x != c['symbol']]
        if kind.startswith('caller'):
            ok = not conflict and (kind == 'caller' or sup or dist(a, lo, hi) <= 0x4000)
        else:
            ok = not conflict and (sup or not o_sup) and not nearer
        why = f"conflict {conflict[:1]} own-sites {len(sup)} d={dist(a,lo,hi):#x} other-supported {len(o_sup)} nearer-others {len(nearer)}"
        if not ok: doubt += 1
        print(('  ok  ' if ok else '  DOUBT'), c['lib'], c['object'], c['name'][:40], c['address'], f"n={c['count']}", 'twinid' if c['twin_identical'] else 'distinct', why)
    summary[kind] = doubt
    print('doubtful', doubt, 'of', len(samp))
# cross-kind agreement
idx = collections.defaultdict(dict)
for kind, v in kinds.items():
    for c in v['credits']:
        idx[(c['lib'], c['object'], c['symbol'])][kind] = c['address']
dis = [(k, d) for k, d in idx.items() if len(set(d.values())) > 1]
print('credited by >1 kind', sum(1 for d in idx.values() if len(d) > 1), 'disagreeing', len(dis))
for k, d in dis[:15]: print('  ', k, d)
print('doubtful in sample', summary)
print('symbols.json name conflicts over all credits', allconf)
