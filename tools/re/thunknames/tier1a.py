"""Tier 1a: vtable relatives. Vtables sharing an entry at the same index are one family (inheritance: a derived
vtable repeats its base's entries it does not override). For two relatives A, B and an index k where A[k] is an
anchor (retail's exact name ?m@CA@@sig) and B[k] is another slot that is off or unnamed, B[k]'s name is ?m@CB@@sig
(the same method and signature), with B's class CB:
  known    CB from B's own anchors (entries only B has, on the order, simple qualifier): direct guesses, access
           variants U / M / E (public / protected / private virtual) -> guesses.tsv for gaps.py
  unknown  no own anchor: B's templates (?m@ X @@sig per known k, ??_G X @@UAEPAXI@Z for the destructor) go to a
           class search over the dictionary (classsearch.candidates()) -> per vtable the best X and its chance.
Only primary vtables (??_7C@@6B@) and simple qualifiers. Writes .work/names/tier1a/{guesses.tsv, search.tsv}.

Usage: python tools/re/thunknames/tier1a.py [--state F] [--search] [--chance 1e-6]
"""
import argparse
import collections
import csv
import math
import os
import re
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from gapguess import State, NB, DEFAULT_STATE, class_of, in_client  # noqa: E402
from vt import vtables  # noqa: E402

PURE = 0x9CCD10
SIMPLE = re.compile(r'^\?(?P<m>[A-Za-z_][A-Za-z0-9_]*)@(?P<c>[A-Za-z_][A-Za-z0-9_]*)@@(?P<acc>[A-Z])(?P<rest>.*)$')
DTOR = re.compile(r'^\?\?_[GE](?P<c>[A-Za-z_][A-Za-z0-9_]*)@@(?P<acc>[A-Z])(?P<rest>AEPAXI@Z)$')
VACC = 'UME'   # virtual __thiscall: public, protected, private


def split(name):
    """('m', method, class, rest-after-access) or ('dtor', None, class, rest) or None."""
    m = SIMPLE.match(name)
    if m and m.group('acc') in VACC:
        return 'm', m.group('m'), m.group('c'), m.group('rest')
    m = DTOR.match(name)
    if m and m.group('acc') in VACC:
        return 'dtor', None, m.group('c'), m.group('rest')
    return None


def build(name_kind, meth, cls, acc, rest):
    if name_kind == 'dtor':
        return f'??_G{cls}@@{acc}{rest}'
    return f'?{meth}@{cls}@@{acc}{rest}'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=DEFAULT_STATE)
    ap.add_argument('--out', default=paths.workdir('tier1a'))
    ap.add_argument('--search', action='store_true', help='class search for the vtables without an own anchor')
    ap.add_argument('--chance', type=float, default=1e-6)
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    st = State(a.state)
    print(st.summary(), flush=True)

    vts = [(addr, sym, ents) for addr, sym, ents in vtables() if sym.endswith('@@6B@')]
    holders = collections.defaultdict(set)          # entry -> vtables holding it (at any index)
    at = collections.defaultdict(set)               # (entry, k) -> vtables
    for vi, (_, _, ents) in enumerate(vts):
        for k, e in enumerate(ents):
            if e != PURE:
                holders[e].add(vi)
                at[(e, k)].add(vi)
    # families: union-find over a shared entry at the same index
    parent = list(range(len(vts)))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x
    for vs in at.values():
        vs = sorted(vs)
        for v in vs[1:]:
            parent[find(v)] = find(vs[0])
    fam = collections.defaultdict(list)
    for vi in range(len(vts)):
        fam[find(vi)].append(vi)

    def name_on(e):
        i = st.index.get(e)
        return st.names[i] if i is not None and i in st.onset else None

    def open_slot(e):
        i = st.index.get(e)
        return i is not None and i not in st.onset and st.window(i) is not None

    # B's own class from its own anchors
    own_cls = {}
    for vi, (_, sym, ents) in enumerate(vts):
        c = collections.Counter()
        for e in ents:
            if e != PURE and len(holders[e]) == 1:
                n = name_on(e)
                if n and split(n):
                    c[split(n)[2]] += 1
        if c:
            own_cls[vi] = c.most_common(1)[0][0]

    # templates per vtable: (k, entry, kind, method, rest) known from an anchored relative at the same index
    tmpl = collections.defaultdict(dict)
    for root, members in fam.items():
        if len(members) < 2:
            continue
        for va in members:
            for k, e in enumerate(vts[va][2]):
                n = name_on(e)
                sp = n and split(n)
                if not sp:
                    continue
                for vb in members:
                    if vb == va or k >= len(vts[vb][2]):
                        continue
                    f = vts[vb][2][k]
                    if f == e or f == PURE or not open_slot(f):
                        continue
                    # f belongs to the base-most vtable holding it (the shortest); a tie is ambiguous
                    hs_ = sorted(holders[f], key=lambda v: len(vts[v][2]))
                    if len(hs_) > 1 and len(vts[hs_[0]][2]) == len(vts[hs_[1]][2]):
                        continue
                    tmpl[hs_[0]].setdefault(k, (f, sp[0], sp[1], sp[3], vts[va][1]))
    n_t = sum(len(v) for v in tmpl.values())
    print(f'primary vtables {len(vts)}, families with >= 2 members {sum(len(m) >= 2 for m in fam.values())}; '
          f'open slots with a relative\'s template {n_t} in {len(tmpl)} vtables; with an own class '
          f'{sum(len(v) for vb, v in tmpl.items() if vb in own_cls)} in {sum(vb in own_cls for vb in tmpl)}',
          flush=True)

    rows = []
    for vb, ks in tmpl.items():
        cls = own_cls.get(vb)
        if not cls:
            continue
        for k, (f, kind, meth, rest, src) in ks.items():
            for acc in VACC:
                rows.append([f'0x{f:X}', build(kind, meth, cls, acc, rest), 't1a-vt',
                             f'{vts[vb][1]}[{k}] from {src}'])
    with open(os.path.join(a.out, 'guesses.tsv'), 'w', newline='') as fo:
        w = csv.writer(fo, delimiter='\t', lineterminator='\n')
        w.writerow(['slot', 'name', 'tier', 'note'])
        w.writerows(rows)
    print(f'known-class guesses {len(rows)} for {len(set(r[0] for r in rows))} slots', flush=True)

    if not a.search:
        return
    from rounds import Hasher
    from classsearch import candidates
    hs = Hasher(candidates())
    print(f'class search: dictionary {len(hs.cand)}', flush=True)
    res = []
    todo = [vb for vb in tmpl if vb not in own_cls]
    for n_done, vb in enumerate(todo):
        if n_done % 100 == 0:
            print(f'  {n_done}/{len(todo)}', flush=True)
        ks = tmpl[vb]
        # each template with each access; a slot counts once (any access)
        count = np.zeros(len(hs.cand), dtype=np.int32)
        logw = np.zeros(len(hs.cand))
        masks = {}
        for k, (f, kind, meth, rest, src) in ks.items():
            i = st.index[f]
            lo, hi, nlo, nhi = st.window(i)
            hitk = np.zeros(len(hs.cand), dtype=bool)
            for acc in VACC:
                pre = '??_G' if kind == 'dtor' else f'?{meth}@'
                post = f'@@{acc}{rest}'
                b = hs.buckets(pre, post)
                hitk |= (b >= nlo) & (b <= nhi)
            masks[k] = hitk
            count += hitk
            logw += np.where(hitk, math.log(3 * (nhi - nlo + 1) / NB), 0.0)
        n = len(ks)
        ok = np.nonzero(count >= (2 if n >= 2 else 1))[0]
        if not len(ok):
            continue
        score = np.array([math.lgamma(n + 1) - math.lgamma(count[c] + 1) - math.lgamma(n - count[c] + 1)
                          + logw[c] + math.log(len(hs.cand)) for c in ok])
        j = int(np.argmin(score))
        c = ok[j]
        x = hs.cand[c]
        hit_k = [k for k in ks if masks[k][c]]
        res.append(dict(vtable=vts[vb][1], ours=vts[vb][1][4:-6], candidate=x, slots=n, hits=len(hit_k),
                        chance=math.exp(score[j]), in_client=int(in_client('?a@' + x + '@@')),
                        ks=' '.join(str(k) for k in hit_k)))
    res.sort(key=lambda r: r['chance'])
    with open(os.path.join(a.out, 'search.tsv'), 'w', newline='') as fo:
        w = csv.DictWriter(fo, list(res[0].keys()) if res else ['vtable'], delimiter='\t', lineterminator='\n')
        w.writeheader()
        for r in res:
            r = dict(r, chance=f'{r["chance"]:.2g}')
            w.writerow(r)
    strong = [r for r in res if r['chance'] < a.chance]
    print(f'class search: vtables {len(todo)}, best candidates {len(res)}, under {a.chance:g}: {len(strong)} '
          f'(in client.exe {sum(r["in_client"] for r in strong)})')
    # guesses for the strong ones (every template, every access) so gaps.py applies them with the cascade
    rows2 = []
    for r in strong:
        vb = next(v for v in todo if vts[v][1] == r['vtable'])
        for k, (f, kind, meth, rest, src) in tmpl[vb].items():
            for acc in VACC:
                rows2.append([f'0x{f:X}', build(kind, meth, r['candidate'], acc, rest), 't1a-search',
                              f'{r["vtable"]}[{k}] X={r["candidate"]} chance {r["chance"]:.2g}'])
    with open(os.path.join(a.out, 'search_guesses.tsv'), 'w', newline='') as fo:
        w = csv.writer(fo, delimiter='\t', lineterminator='\n')
        w.writerow(['slot', 'name', 'tier', 'note'])
        w.writerows(rows2)
    print(f'search guesses {len(rows2)}')


if __name__ == '__main__':
    main()
