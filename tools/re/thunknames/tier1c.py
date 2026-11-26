"""Tier 1c: convention variants of our off names against the current (narrower) windows.

Variants: thunkname.variants() (IM3's: access, const, int kinds, pointer cv, pointer <-> reference, virtual,
static, callconv, ret, case, ??_G / ??_E) plus
  access+const  both edits at once
  underscore    method name CamelCase <-> Under_Score (GetValue <-> Get_Value)
  suffix        class qualifier +/- Class / Struct (a class's name changes everywhere it is spelled)
  stl           std:: _Construct / _Destroy <-> construct / destroy
Per kind: near hits against a null (each slot's near part shifted by a random +/-30..300 buckets, --null times).
A slot with exactly one near hit, of a kind with hits >= 2 x null, is a guess -> tier1c/guesses.tsv (gapguess.py
applies it with the cascade). Prints the kind table.

Usage: python tools/re/thunknames/tier1c.py [--state F] [--null 20]
"""
import argparse
import collections
import csv
import os
import random
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from mangle import rename, contains, class_of  # noqa: E402
from gapguess import State, DEFAULT_STATE, bk  # noqa: E402
import thunkname  # noqa: E402

NBK = 15048


def camel_to_under(m):
    parts = re.findall(r'[A-Z][a-z0-9]*|[a-z0-9]+', m)
    return '_'.join(p[:1].upper() + p[1:] for p in parts) if len(parts) > 1 else None


def under_to_camel(m):
    return ''.join(p[:1].upper() + p[1:] for p in m.split('_') if p) if '_' in m.strip('_') else None


CONFIRMED = {}   # our class -> retail class (suffix edits confirmed in an earlier run): combined with one more edit


def extra(name):
    out = []
    for c0, c1 in CONFIRMED.items():
        if class_of(name) == c0:
            sx = rename(name, c0, c1)
            out.append(('suffix', sx))
            out += [('suffix+' + k, x) for k, x in thunkname.variants(sx) if k in ('access', 'const', 'int', 'ptrref')]
    v = thunkname.variants(name)
    acc = [x for k, x in v if k == 'access']
    for a in acc:
        for k, x in thunkname.variants(a):
            if k == 'const':
                out.append(('access+const', x))
    m = re.match(r'^\?([A-Za-z_][A-Za-z0-9_]*)@', name)
    if m and not name.startswith('??'):
        meth = m.group(1)
        for f in (camel_to_under, under_to_camel):
            r = f(meth)
            if r and r != meth:
                out.append(('underscore', '?' + r + name[m.end(1):]))
    m = re.match(r'\?\?(?:0|1|_G|_E)([A-Za-z_][A-Za-z0-9_]*)@@', name) or \
        re.match(r'\?[A-Za-z_][A-Za-z0-9_]*@([A-Za-z_][A-Za-z0-9_]*)@@', name)
    if m:
        c = m.group(1)
        alts = set()
        for suf in ('Class', 'Struct'):
            if c.endswith(suf) and len(c) > len(suf):
                alts.add(c[:-len(suf)])
            else:
                alts.add(c + suf)
        if c.endswith('Struct'):
            alts.add(c[:-6] + 'Class')
        if c.endswith('Class'):
            alts.add(c[:-5] + 'Struct')
        for x in alts:
            out.append(('suffix', rename(name, c, x)))
    for a, b in (('_Construct', 'construct'), ('_Destroy', 'destroy')):
        if name.startswith(f'?{a}@std@@'):
            out.append(('stl', name.replace(f'?{a}@', f'?{b}@', 1)))
    return v + out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=DEFAULT_STATE)
    ap.add_argument('--out', default=paths.workdir('tier1c'))
    ap.add_argument('--null', type=int, default=20)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--confirmed', help='hits.tsv of an earlier run: its t1c-suffix class renames')
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    st = State(a.state)
    print(st.summary(), flush=True)
    rnd = random.Random(a.seed)
    if a.confirmed:
        cnt = collections.Counter()
        for r in csv.DictReader(open(a.confirmed), delimiter='\t'):
            if r['tier'] in ('r1', 't1c-suffix', 't2-class', 'cs2-own', 'cs2-dict'):
                m0, m1 = class_of(r['before'] or ''), class_of(r['name'])
                if m0 and m1 and m0 != m1:
                    cnt[(m0, m1)] += 1
        CONFIRMED.update({c0: c1 for (c0, c1), v in cnt.items() if v >= 2})
        print(f'confirmed class renames: {len(CONFIRMED)}')
    hits = collections.Counter()
    nulls = collections.Counter()
    per_slot = {}
    nvar = 0
    for i in range(st.n):
        nm = st.names[i]
        w = st.window(i)
        if not nm or w is None or 'Unknown' in nm:
            continue
        vs = extra(nm)
        seen = set()
        vs = [(k, x) for k, x in vs if not (x in seen or seen.add(x))]
        nvar += len(vs)
        lo, hi, nlo, nhi = w
        bs = [(k, x, bk(x)) for k, x in vs]
        near = [(k, x) for k, x, b in bs if nlo <= b <= nhi]
        for k, _ in near:
            hits[k] += 1
        for _ in range(a.null):
            d = rnd.choice((-1, 1)) * rnd.randint(30, 300)
            for k, x, b in bs:
                if nlo + d <= b <= nhi + d:
                    nulls[k] += 1.0 / a.null
        if near:
            per_slot[i] = near
    print(f'variants {nvar}')
    print('kind          near  null')
    real = set()
    for k in sorted(set(hits) | set(nulls), key=lambda k: -hits[k]):
        flag = hits[k] >= 2 * nulls[k] and hits[k] >= 2
        if flag:
            real.add(k)
        print(f'{k:13s} {hits[k]:5d} {nulls[k]:5.1f}{"  real" if flag else ""}')
    rows = []
    for i, near in per_slot.items():
        if len(near) == 1 and near[0][0] in real:
            k, x = near[0]
            rows.append([f'0x{st.table[i][0]:X}', x, f't1c-{k}', f'ours {st.names[i]}'])
    with open(os.path.join(a.out, 'guesses.tsv'), 'w', newline='') as fo:
        w = csv.writer(fo, delimiter='\t', lineterminator='\n')
        w.writerow(['slot', 'name', 'tier', 'note'])
        w.writerows(rows)
    exp = sum(nulls[k] for k in real)
    print(f'guesses (one near hit, real kind) {len(rows)}; expected false about {exp:.1f} '
          f'(the null count of the real kinds)')


if __name__ == '__main__':
    main()
