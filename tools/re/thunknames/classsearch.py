"""Class-name recovery through LINK's thunk hash (binary.md 5.1-5.2), exploratory.

For every class name C of ours that occurs (once, as the qualifier) in thunk slots that are off LINK's order and
have a bucket window (namecheck.tsv), substitute candidate names X and count the slots whose new decorated name
hashes into its window. A candidate that fits k slots by chance has probability about prod(width_i / nbuckets).

Usage: python tools/re/thunknames/classsearch.py [--min-slots 2] [--out .work/names/classsearch.tsv]
"""
import argparse
import collections
import csv
import glob
import math
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from linklayout import lhash, bucket, LHASH_PRIME  # noqa: E402
import numpy as np  # noqa: E402


def raw(h, s):
    """LINK's hash loop over s from state h, without the final modulus (ASCII only)."""
    for c in s.encode('latin1'):
        h = (h + ((c << 18) & 0xFFFFFFFF) + c + ((h >> 1) ^ h)) & 0xFFFFFFFF
    return h
CLIENT_EXE = paths.CLIENT_EXE
EA_ROOT = paths.EA_ROOT

MAXP, P = 8192, 6856
NB = MAXP + P

ID = re.compile(rb'[A-Za-z_][A-Za-z0-9_]{2,60}')


def qualifier(name):
    """(class, start, end) of the simple class qualifier in a decorated member name, or None."""
    m = re.match(r'\?\?(?:0|1|_G|_E|4)([A-Za-z_][A-Za-z0-9_]*)@@', name)
    if m:
        return m.group(1), m.start(1), m.end(1)
    m = re.match(r'\?[A-Za-z_][A-Za-z0-9_]*@([A-Za-z_][A-Za-z0-9_]*)@@', name)
    if m:
        return m.group(1), m.start(1), m.end(1)
    return None


def candidates():
    words = set()
    data = open(CLIENT_EXE, 'rb').read()
    for m in ID.finditer(data):
        words.add(m.group().decode())
    for root in (os.path.join(EA_ROOT, 'CnC_Renegade'), os.path.join(EA_ROOT, 'CnC_Generals_Zero_Hour'),
                 os.path.join(paths.REPO, 'app')):
        for f in glob.glob(os.path.join(root, '**', '*.[hc]*'), recursive=True):
            try:
                txt = open(f, 'rb').read()
            except OSError:
                continue
            for m in re.finditer(rb'\b(?:class|struct|enum|union)\s+([A-Za-z_][A-Za-z0-9_]*)', txt):
                words.add(m.group(1).decode())
            base = os.path.splitext(os.path.basename(f))[0]
            words.add(base)
    out = set(words)
    for w in words:
        camel = ''.join(p[:1].upper() + p[1:] for p in re.split(r'[_\W]+', w) if p)
        for s in (w, camel):
            for suf in ('', 'Class', 'Struct', 'Manager', 'ManagerClass', 'Mgr', 'MgrClass'):
                out.add(s + suf)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--min-slots', type=int, default=2)
    ap.add_argument('--out', default=paths.work('classsearch.tsv'))
    a = ap.parse_args()

    rows = list(csv.DictReader(open(os.path.join(paths.REPO, '.work', 'im3', 'namecheck.tsv')), delimiter='\t'))
    # calibration: on-order names hash to their own bucket column
    bad = sum(1 for r in rows if r['on_order'] == 'on' and bucket(lhash(r['name']), MAXP, P) != int(r['bucket']))
    print(f'calibration: on-order names with a different bucket: {bad}')

    by_class = collections.defaultdict(list)
    on_by_class = collections.Counter()
    for r in rows:
        q = qualifier(r['name'])
        if not q:
            continue
        cls, s, e = q
        if r['on_order'] == 'on':
            on_by_class[cls] += 1
            continue
        if r['on_order'] != 'off' or not r['window'] or r['name'].count(cls) != 1:
            continue
        lo, hi = map(int, r['window'].split('-'))
        by_class[cls].append((r['name'][:s], r['name'][e:], lo, hi, r['slot']))

    cand = sorted(candidates(), key=lambda x: (-len(x), x))
    print(f'classes with off slots: {len(by_class)}; candidates: {len(cand)}', flush=True)
    L = max(len(x) for x in cand)
    mat = np.zeros((L, len(cand)), dtype=np.uint32)
    lens = np.array([len(x) for x in cand])
    active = [int(np.sum(lens > j)) for j in range(L)]
    for i, x in enumerate(cand):
        mat[:len(x), i] = np.frombuffer(x.encode('latin1'), dtype=np.uint8)
    cache = {}

    def after(pre):
        """Hash state of pre + candidate, for every candidate (before the final modulus)."""
        if pre in cache:
            return cache[pre]
        h0 = raw(0, pre)
        h = np.full(len(cand), h0, dtype=np.uint32)
        for j in range(L):
            k = active[j]
            c = mat[j, :k]
            hk = h[:k]
            h[:k] = hk + (c << 18) + c + ((hk >> 1) ^ hk)
        if pre.startswith('??'):
            cache[pre] = h
        return h

    def buckets(pre, post):
        h = after(pre)
        for ch in post.encode('latin1'):
            c = np.uint32(ch)
            h = h + (c << np.uint32(18)) + c + ((h >> np.uint32(1)) ^ h)
        h = h % np.uint32(LHASH_PRIME)
        b = h % np.uint32(MAXP)
        return np.where(b < P, h % np.uint32(2 * MAXP), b)

    # self-test: the vector hash against lhash / bucket
    pre, post = '?Get_Value@', '@@QAEPBDPBD@Z'
    bk = buckets(pre, post)
    for i in range(0, len(cand), max(1, len(cand) // 200)):
        assert bk[i] == bucket(lhash(pre + cand[i] + post), MAXP, P), cand[i]
    print('vector hash self-test passed')

    results = []
    todo = [c for c, s in by_class.items() if len(s) >= a.min_slots]
    print(f'classes to search: {len(todo)}, slots {sum(len(by_class[c]) for c in todo)}', flush=True)
    for ci, (cls, slots) in enumerate(sorted(by_class.items())):
        if len(slots) < a.min_slots:
            continue
        if ci % 100 == 0:
            print(f'  {ci} {cls}', flush=True)
        count = np.zeros(len(cand), dtype=np.int32)
        hitmask = []
        for (pre, post, lo, hi, sl) in slots:
            bk = buckets(pre, post)
            m = (bk >= lo) & (bk <= hi)
            count += m
            hitmask.append(m)
        idx = np.nonzero(count >= 2)[0]
        best = []
        for i in idx:
            if cand[i] == cls:
                continue
            hits = [slots[k][4] for k in range(len(slots)) if hitmask[k][i]]
            best.append((len(hits), cand[i], hits))
        best.sort(key=lambda t: -t[0])
        widths = [hi - lo + 1 for (_, _, lo, hi, _) in slots]
        for n, x, hits in best[:3]:
            # chance of n hits among the class's slots (median width), times the candidates tried
            w = sorted(widths)[len(widths) // 2]
            pc = math.comb(len(slots), n) * (w / NB) ** n * len(cand)
            results.append((cls, len(slots), on_by_class[cls], x, n, f'{pc:.2g}', ' '.join(hits)))
    with open(a.out, 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t')
        w.writerow(['ours', 'off_slots', 'on_slots', 'candidate', 'hits', 'expected_by_chance', 'slots'])
        w.writerows(results)
    strong = [r for r in results if float(r[5]) < 0.01]
    print(f'classes searched: {sum(1 for s in by_class.values() if len(s) >= a.min_slots)}; '
          f'candidate rows: {len(results)}; strong (chance < 0.01): {len(strong)}')
    for r in sorted(strong, key=lambda r: float(r[5]))[:40]:
        print(*r[:6], sep='\t')


if __name__ == '__main__':
    main()
