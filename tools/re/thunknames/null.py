"""Null test for rounds.py's acceptance rule: one round on namecheck.tsv's windows with random identifier
candidates (the same count and length distribution as the real dictionary). Every acceptance is false by
construction; prints how many tokens' best random candidate scores under each threshold, by kind.

Usage: python tools/re/thunknames/null.py [--seed 1]
"""
import argparse
import collections
import csv
import math
import random
import string
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from classsearch import candidates  # noqa: E402
from rounds import Hasher, anchors_windows, tokens  # noqa: E402

THRESH = (1e-2, 1e-4, 1e-6, 1e-8, 1e-10, 1e-12)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--out', default=paths.work('null.tsv'))
    a = ap.parse_args()
    rnd = random.Random(a.seed)
    real = candidates()
    alpha = string.ascii_letters
    fake = set()
    for x in real:
        while True:
            # at least 6 characters: the dictionary holds 1-2 character file names, whose random
            # replacements of the same length run out
            y = rnd.choice(string.ascii_uppercase) + ''.join(rnd.choice(alpha) for _ in range(max(len(x), 6) - 1))
            if y not in real and y not in fake:
                fake.add(y)
                break
    hs = Hasher(fake)
    rows = list(csv.DictReader(open(os.path.join(paths.REPO, '.work', 'im3', 'namecheck.tsv')), delimiter='\t'))
    names = [r['name'] for r in rows]
    b, onset, win = anchors_windows(names)
    nb = 8192 + 6856
    groups = {}
    for i, n in enumerate(names):
        if win[i] is None:
            continue
        for kind, tok, s, e in tokens(n):
            if n.count(tok) == 1:
                groups.setdefault((kind, tok), []).append((i, n[:s], n[e:]))
    todo = [(k, v) for k, v in groups.items() if len(v) >= 2]
    print(f'null: tokens {len(todo)}, candidates {len(hs.cand)}', flush=True)
    best = []
    for gi, ((kind, tok), slots) in enumerate(sorted(todo)):
        if gi % 200 == 0:
            print(f'  {gi}', flush=True)
        count = np.zeros(len(hs.cand), dtype=np.int32)
        logw = np.zeros(len(hs.cand))
        for i, pre, post in slots:
            lo, hi = win[i]
            bk = hs.buckets(pre, post)
            m = (bk >= lo) & (bk <= hi)
            count += m
            logw += np.where(m, math.log((hi - lo + 1) / nb), 0.0)
        ok = np.nonzero(count >= 2)[0]
        if not len(ok):
            continue
        n = len(slots)
        score = [math.lgamma(n + 1) - math.lgamma(count[c] + 1) - math.lgamma(n - count[c] + 1) + logw[c]
                 + math.log(len(hs.cand)) for c in ok]
        k = int(np.argmin(score))
        best.append((kind, tok, n, int(count[ok[k]]), math.exp(score[k]), hs.cand[ok[k]]))
    with open(a.out, 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t')
        w.writerow(['kind', 'ours', 'slots', 'hits', 'expected_by_chance', 'random_candidate'])
        for r in sorted(best, key=lambda r: r[4]):
            w.writerow([r[0], r[1], r[2], r[3], f'{r[4]:.2g}', r[5]])
    c = collections.Counter()
    for kind, _, _, _, ch, _ in best:
        for t in THRESH:
            if ch < t:
                c[(kind, t)] += 1
    for kind in ('class', 'method'):
        print(kind, ' '.join(f'<{t:g}: {c[(kind, t)]}' for t in THRESH), flush=True)


if __name__ == '__main__':
    main()
