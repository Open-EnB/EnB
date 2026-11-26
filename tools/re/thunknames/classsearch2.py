"""Step 2 (no model): class-name search over every class with open slots and no anchor, on the current state.

Per class C (our qualifier), templates = its open slots (off or unnamed-with-window are not used: only named slots
with C as qualifier); each slot is tested with every access of its kind (Q A I / U M E / S K C; a slot counts
once). A candidate X scores k near hits of n slots: chance = C(n, k) * prod(3 w_i / NB) * candidates tried for C.
Two candidate sources, scored separately (their own multiplier):
  own   variants generated from C itself (prefix UI / Unknown / C stripped, suffix toggles, abbreviation expansion,
        `UI<X>Class` -> `<X>Interface`): about 100-300 per class
  dict  a large dictionary (identifiers in client.exe, EA / app class and file names, camel forms) x SUFFIXES,
        vectorised, chunked over a process pool; slots where C occurs more than once are left out here
Null (--null): the dict search with random identifiers of the same length distribution (chunk 0 only, scaled).
Writes .work/names/cs2/{own.tsv, dict.tsv, guesses.tsv} (paths.work).

Usage: python tools/re/thunknames/classsearch2.py --state F [--workers 18] [--chunk 200000] [--null] [--no-dict]
"""
import argparse
import collections
import csv
import glob
import math
import multiprocessing as mp
import os
import random
import re

import string
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from mangle import rename, spelling  # noqa: E402
from gapguess import State, NB, bk, class_of, client_ids  # noqa: E402
from linklayout import LHASH_PRIME  # noqa: E402
import classsearch as cs  # noqa: E402

MAXP, P = 8192, 6856
ACC = {c: g for g in ('QAI', 'UME', 'SKC') for c in g}
SUFFIXES = ['', 'Class', 'Struct', 'Manager', 'ManagerClass', 'Mgr', 'MgrClass', 'Interface', 'InterfaceClass',
            'View', 'ViewClass', 'Window', 'WindowClass', 'Object', 'ObjectClass', 'Obj', 'ObjClass', 'RObj',
            'RenderObj', 'RenderObjClass', 'Gadget', 'GadgetClass', 'Packet', 'PacketClass', 'Data', 'DataClass',
            'Info', 'InfoClass', 'Record', 'RecordClass', 'List', 'ListClass', 'System', 'SystemClass', 'Handler',
            'HandlerClass', 'Base', 'BaseClass', 'Def', 'DefClass', 'Node', 'NodeClass']
ABBR = {'RObj': ['RenderObj', 'RenderObject'], 'Obj': ['Object'], 'Msg': ['Message'], 'Mgr': ['Manager'],
        'Desc': ['Description', 'Descriptor'], 'Pos': ['Position', 'Positional'], 'Info': ['Information'],
        'Btn': ['Button'], 'Wnd': ['Window'], 'Win': ['Window'], 'Dlg': ['Dialog'], 'Ctrl': ['Control'],
        'Cfg': ['Config', 'Configuration'], 'Pkt': ['Packet'], 'Req': ['Request'], 'Resp': ['Response'],
        'Attr': ['Attribute'], 'Attrib': ['Attribute'], 'Attrb': ['Attribute'], 'Inv': ['Inventory'],
        'Num': ['Number'], 'Char': ['Character'], 'Manu': ['Manufacturing', 'Manufacture'], 'Opt': ['Options',
        'Option'], 'Cpit': ['Cockpit'], 'Prox': ['Proximity'], 'SBase': ['Starbase'], 'Sb': ['Starbase'],
        'Dyn': ['Dynamic'], 'Gfx': ['Graphics'], 'Snd': ['Sound'], 'Tex': ['Texture'], 'Img': ['Image'],
        'Anim': ['Animation'], 'Ani': ['Animation'], 'Tgt': ['Target'], 'Def': ['Definition'],
        'Mfd': ['MFD'], 'MFD': ['Mfd'], 'Cam': ['Camera'], 'Ctx': ['Context'], 'Str': ['String'],
        'Buf': ['Buffer'], 'Mat': ['Material'], 'Vec': ['Vector'], 'Mtx': ['Matrix'], 'Lst': ['List'],
        'Mgmt': ['Management'], 'Util': ['Utility', 'Utilities'], 'Conn': ['Connection'], 'Svr': ['Server'],
        'Srv': ['Server'], 'Cli': ['Client'], 'Grp': ['Group'], 'Pl': ['Player'], 'Plr': ['Player'],
        'Ship': ['Ship'], 'Wpn': ['Weapon'], 'Eff': ['Effect'], 'Fx': ['Effect', 'Effects'], 'Txt': ['Text'],
        'Lbl': ['Label'], 'Sel': ['Selection'], 'Ctl': ['Control'], 'Disp': ['Display'], 'Coll': ['Collision'],
        'Proj': ['Projectile', 'Projection'], 'Hud': ['HUD'], 'HUD': ['Hud'], 'Nav': ['Navigation'],
        'Tex2': ['Texture'], 'Dir': ['Directory'], 'Ref': ['Reference'], 'Res': ['Resource'], 'Pt': ['Point']}
STRIP_PRE = ('UI', 'Unknown', 'C', 'Client', 'Ui')


def camel_parts(w):
    return re.findall(r'[A-Z]+(?=[A-Z][a-z])|[A-Z]?[a-z]+|[A-Z]+|[0-9]+', w)


def own_variants(c):
    base = set()
    core = c
    for suf in ('Class', 'Struct'):
        if core.endswith(suf) and len(core) > len(suf):
            core = core[:-len(suf)]
    base.add(core)
    for pre in STRIP_PRE:
        if core.startswith(pre) and len(core) > len(pre) + 2 and core[len(pre)].isupper():
            base.add(core[len(pre):])
    base |= {b.replace('_', '') for b in base}
    # abbreviation expansion, one part or all parts
    more = set()
    for b in base:
        parts = camel_parts(b)
        if not parts:
            continue
        alts = [[p] + ABBR.get(p, []) for p in parts]
        for k, p in enumerate(parts):
            for x in alts[k][1:]:
                more.add(''.join(parts[:k] + [x] + parts[k + 1:]))
        full = ''.join(a[-1] for a in alts)
        more.add(full)
    base |= more
    out = set()
    for b in base:
        if not b or not re.match(r'[A-Za-z_]', b):
            continue
        for suf in SUFFIXES:
            out.add(b + suf)
    out.discard(c)
    return sorted(out)


def slot_variants(name):
    k = name.find('@@')
    p = k + 2 if k >= 0 and k + 2 < len(name) and name[k + 2] in ACC else None
    if name.startswith(('??_G', '??_E')):
        out = [name[:p] + a + name[p + 1:] for a in 'UME'] if p else [name]
        return out + [('??_E' if v.startswith('??_G') else '??_G') + v[4:] for v in out]
    if p is None:
        return [name]
    return [name[:p] + a + name[p + 1:] for a in ACC[name[p]]]


def logchance(n, k, logw, ntries):
    return math.lgamma(n + 1) - math.lgamma(k + 1) - math.lgamma(n - k + 1) + logw + math.log(max(1, ntries))


# ---- dict search (vectorised) --------------------------------------------------------------------------------

def raw_np(h, s):
    for ch in s.encode('latin1'):
        c = np.uint32(ch)
        h = h + (c << np.uint32(18)) + c + ((h >> np.uint32(1)) ^ h)
    return h


def final_bucket(h):
    h = h % np.uint32(LHASH_PRIME)
    b = h % np.uint32(MAXP)
    return np.where(b < P, h % np.uint32(2 * MAXP), b)


_W = {}


def worker_init(cands, jobs):
    cands = sorted(cands, key=lambda x: (-len(x), x))
    L = max(len(x) for x in cands)
    mat = np.zeros((L, len(cands)), dtype=np.uint32)
    lens = np.array([len(x) for x in cands])
    for i, x in enumerate(cands):
        mat[:len(x), i] = np.frombuffer(x.encode('latin1'), dtype=np.uint8)
    _W.update(cands=cands, mat=mat, active=[int(np.sum(lens > j)) for j in range(L)], jobs=jobs)


def after(pre):
    h0 = cs.raw(0, pre)
    h = np.full(len(_W['cands']), h0, dtype=np.uint32)
    for j, k in enumerate(_W['active']):
        c = _W['mat'][j, :k]
        hk = h[:k]
        h[:k] = hk + (c << 18) + c + ((hk >> 1) ^ hk)
    return h


def search_chunk(job_ids):
    """For each class job: (class, [(k, logw, cand)] best few by score)."""
    out = []
    cache = {}
    for ji in job_ids:
        c, slots = _W['jobs'][ji]       # slots: [(pre, [posts], nlo, nhi, w)]
        count = np.zeros(len(_W['cands']), dtype=np.int32)
        logw = np.zeros(len(_W['cands']))
        for pre, posts, nlo, nhi, w in slots:
            if pre not in cache:
                if len(cache) > 32:
                    cache.clear()
                cache[pre] = after(pre)
            h = cache[pre]
            m = np.zeros(len(_W['cands']), dtype=bool)
            for post in posts:
                b = final_bucket(raw_np(h, post))
                m |= (b >= nlo) & (b <= nhi)
            count += m
            logw += np.where(m, math.log(3 * w / NB), 0.0)
        n = len(slots)
        ok = np.nonzero(count >= 2)[0]
        best = []
        if len(ok):
            sc = np.array([logchance(n, int(count[i]), logw[i], 1) for i in ok])
            for j in np.argsort(sc)[:3]:
                i = ok[j]
                best.append((float(sc[j]), int(count[i]), _W['cands'][i]))
        out.append((ji, best))
    return out


def dictionary():
    words = set()
    data = open(cs.CLIENT_EXE, 'rb').read()
    for m in cs.ID.finditer(data):
        words.add(m.group().decode())
    for root in (os.path.join(cs.EA_ROOT, 'CnC_Renegade'), os.path.join(cs.EA_ROOT, 'CnC_Generals_Zero_Hour'),
                 os.path.join(paths.REPO, 'app')):
        for f in glob.glob(os.path.join(root, '**', '*.[hc]*'), recursive=True):
            try:
                txt = open(f, 'rb').read()
            except OSError:
                continue
            for m in re.finditer(rb'\b(?:class|struct|enum|union)\s+([A-Za-z_][A-Za-z0-9_]*)', txt):
                words.add(m.group(1).decode())
            words.add(os.path.splitext(os.path.basename(f))[0])
    base = set()
    for w in words:
        if len(w) > 40:
            continue
        camel = ''.join(p[:1].upper() + p[1:] for p in re.split(r'[_\W]+', w) if p)
        base.add(w)
        base.add(camel)
        for suf in ('Class', 'Struct'):
            if w.endswith(suf) and len(w) > len(suf):
                base.add(w[:-len(suf)])
    return {b + s for b in base for s in SUFFIXES if b}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--workers', type=int, default=18)
    ap.add_argument('--chunk', type=int, default=150000)
    ap.add_argument('--null', action='store_true')
    ap.add_argument('--no-dict', action='store_true')
    ap.add_argument('--out', default=paths.workdir('cs2'))
    ap.add_argument('--own-chance', type=float, default=1e-4)
    ap.add_argument('--dict-chance', type=float, default=1e-7)
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    st = State(a.state)
    print(st.summary(), flush=True)
    by = collections.defaultdict(list)
    on = collections.Counter()
    for i, n in enumerate(st.names):
        c = n and class_of(n)
        if not c:
            continue
        if i in st.onset:
            on[c] += 1
        elif st.window(i):
            by[c].append(i)
    classes = sorted(c for c, v in by.items() if len(v) >= 2 and on[c] == 0)
    print(f'classes (>= 2 open slots, no anchor) {len(classes)}, slots {sum(len(by[c]) for c in classes)}', flush=True)

    # own variants
    own_rows, guesses = [], []
    accepted = {}
    for c in classes:
        cands = own_variants(c)
        best = None
        for x in cands:
            k, logw, hits = 0, 0.0, []
            for i in by[c]:
                lo, hi, nlo, nhi = st.window(i)
                vs = slot_variants(rename(st.names[i], c, x))
                for v in vs:
                    if nlo <= bk(v) <= nhi:
                        k += 1
                        logw += math.log(len(vs) * (nhi - nlo + 1) / NB)
                        hits.append((i, v))
                        break
            if k >= 2:
                lc = logchance(len(by[c]), k, logw, len(cands))
                if best is None or lc < best[0]:
                    best = (lc, k, x, hits)
        if best:
            ch = math.exp(best[0])
            own_rows.append([c, best[2], best[1], len(by[c]), len(cands), f'{ch:.2g}',
                             int(best[2] in client_ids())])
            if ch < a.own_chance:
                accepted[c] = best[2]
                for i, v in best[3]:
                    guesses.append([f'0x{st.table[i][0]:X}', v, 'cs2-own', f'{c}->{best[2]} chance {ch:.2g}'])
    own_rows.sort(key=lambda r: float(r[5]))
    with open(os.path.join(a.out, 'own.tsv'), 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['ours', 'candidate', 'hits', 'slots', 'tried', 'chance', 'in_client'])
        w.writerows(own_rows)
    print(f'own variants: accepted {len(accepted)} classes, {len(guesses)} slots (chance < {a.own_chance:g})',
          flush=True)

    if not a.no_dict:
        rest = [c for c in classes if c not in accepted]
        jobs = []
        for c in rest:
            slots = []
            for i in by[c]:
                nm = st.names[i]
                if len(spelling(c).findall(nm)) != 1:
                    continue
                m = spelling(c).search(nm)
                pre, post = nm[:m.start()], nm[m.end() - 1:]
                lo, hi, nlo, nhi = st.window(i)
                slots.append((pre, [v[len(pre) + len(c):] for v in slot_variants(nm) if v.startswith(pre)], nlo,
                              nhi, nhi - nlo + 1))
            if len(slots) >= 2:
                jobs.append((c, slots))
        if a.null:
            rnd = random.Random(1)
            real = sorted(dictionary())
            cand = set()
            for x in real[:a.chunk]:
                cand.add(rnd.choice(string.ascii_uppercase) + ''.join(rnd.choice(string.ascii_letters)
                                                                      for _ in range(max(len(x), 4) - 1)))
            chunks = [sorted(cand)]
            total = len(real)
        else:
            cand = sorted(dictionary())
            total = len(cand)
            rnd = random.Random(2)
            rnd.shuffle(cand)
            chunks = [cand[k:k + a.chunk] for k in range(0, len(cand), a.chunk)]
        print(f'dict: {total} candidates in {len(chunks)} chunks; class jobs {len(jobs)}', flush=True)
        best = collections.defaultdict(list)
        for ci, ch in enumerate(chunks):
            # one chunk at a time, the class jobs spread over the pool
            parts = [list(range(len(jobs)))[k::a.workers] for k in range(a.workers)]
            with mp.Pool(a.workers, initializer=worker_init, initargs=(ch, jobs)) as pool:
                for res in pool.imap_unordered(search_chunk, parts):
                    for ji, b in res:
                        best[ji] += b
            print(f'  chunk {ci + 1}/{len(chunks)} done', flush=True)
        rows = []
        for ji, b in best.items():
            if not b:
                continue
            lc, k, x = min(b)
            c, slots = jobs[ji]
            lc += math.log(total)
            rows.append([c, x, k, len(slots), f'{math.exp(lc):.2g}', int(x in client_ids())])
        rows.sort(key=lambda r: float(r[4]))
        with open(os.path.join(a.out, 'null.tsv' if a.null else 'dict.tsv'), 'w', newline='') as f:
            w = csv.writer(f, delimiter='\t', lineterminator='\n')
            w.writerow(['ours', 'candidate', 'hits', 'slots', 'chance', 'in_client'])
            w.writerows(rows)
        for t in (1e-4, 1e-6, 1e-7, 1e-8, 1e-10):
            print(f'  dict best under {t:g}: {sum(float(r[4]) < t for r in rows)}', flush=True)
        if not a.null:
            for r in rows:
                if float(r[4]) < a.dict_chance:
                    c, x = r[0], r[1]
                    for i in by[c]:
                        lo, hi, nlo, nhi = st.window(i)
                        for v in slot_variants(rename(st.names[i], c, x)):
                            if nlo <= bk(v) <= nhi:
                                guesses.append([f'0x{st.table[i][0]:X}', v, 'cs2-dict', f'{c}->{x} chance {r[4]}'])
                                break
    if not a.null:
        with open(os.path.join(a.out, 'guesses.tsv'), 'w', newline='') as f:
            w = csv.writer(f, delimiter='\t', lineterminator='\n')
            w.writerow(['slot', 'name', 'tier', 'note'])
            w.writerows(guesses)
        print(f'guesses {len(guesses)}')


if __name__ == '__main__':
    main()
