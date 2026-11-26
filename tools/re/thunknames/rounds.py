"""Iterated name recovery through LINK's thunk hash (exploratory; binary.md 5.1-5.2).

Each round: anchors = the longest non-decreasing bucket run over the current names (slot order); every other
slot's window = [previous anchor's bucket, next anchor's bucket]. Then a token search: for each token of ours
(a class qualifier, or a method name) that occurs in at least --min-slots off slots, every candidate X replaces
it, and the slots whose new name hashes into its window count. A token's best candidate is accepted when its
expected false hits (C(n, k) * prod of the hit slots' window fractions * candidates) is under --chance; each slot
takes the new name only when it hashes into its own window. Rounds stop when nothing is accepted.

Usage: python tools/re/thunknames/rounds.py [--rounds 4] [--chance 1e-4]
Writes .work/names/rounds/<n>.tsv (slot, target, name, original, bucket, on_order, how) and accepted.tsv.
"""
import argparse
import csv
import math
import os
import re
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from linklayout import lhash, bucket, lnds_members, LHASH_PRIME  # noqa: E402
from classsearch import candidates, raw, MAXP, P, NB  # noqa: E402

IDENT = r'[A-Za-z_][A-Za-z0-9_]*'


def tokens(name):
    """[(kind, token, start, end)] of the replaceable identifiers: the class qualifier and the method name."""
    out = []
    m = re.match(r'\?\?(?:0|1|_G|_E|4)(' + IDENT + ')@@', name)
    if m:
        out.append(('class', m.group(1), m.start(1), m.end(1)))
        return out
    m = re.match(r'\?(' + IDENT + ')@(' + IDENT + ')@@', name)
    if m:
        out.append(('method', m.group(1), m.start(1), m.end(1)))
        out.append(('class', m.group(2), m.start(2), m.end(2)))
    return out


class Hasher:
    def __init__(self, cand):
        self.cand = sorted(cand, key=lambda x: (-len(x), x))
        L = max(len(x) for x in self.cand)
        self.mat = np.zeros((L, len(self.cand)), dtype=np.uint32)
        lens = np.array([len(x) for x in self.cand])
        self.active = [int(np.sum(lens > j)) for j in range(L)]
        for i, x in enumerate(self.cand):
            self.mat[:len(x), i] = np.frombuffer(x.encode('latin1'), dtype=np.uint8)
        self.cache = {}

    def after(self, pre):
        if pre in self.cache:
            return self.cache[pre]
        h = np.full(len(self.cand), raw(0, pre), dtype=np.uint32)
        for j, k in enumerate(self.active):
            c = self.mat[j, :k]
            hk = h[:k]
            h[:k] = hk + (c << 18) + c + ((hk >> 1) ^ hk)
        if len(self.cache) < 64 and len(pre) <= 4:
            self.cache[pre] = h
        return h

    def buckets(self, pre, post):
        h = self.after(pre).copy()
        for ch in post.encode('latin1'):
            c = np.uint32(ch)
            h = h + (c << np.uint32(18)) + c + ((h >> np.uint32(1)) ^ h)
        h = h % np.uint32(LHASH_PRIME)
        b = h % np.uint32(MAXP)
        return np.where(b < P, h % np.uint32(2 * MAXP), b)


def anchors_windows(names):
    b = [bucket(lhash(n), MAXP, P) for n in names]
    on = sorted(lnds_members(b))
    onset = set(on)
    win = [None] * len(names)
    j = 0
    for i in range(len(names)):
        while j < len(on) and on[j] < i:
            j += 1
        if i in onset:
            continue
        if j == 0 or j >= len(on):
            continue
        win[i] = (b[on[j - 1]], b[on[j]])
    return b, onset, win


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--rounds', type=int, default=4)
    ap.add_argument('--chance', type=float, default=1e-4)
    ap.add_argument('--min-slots', type=int, default=2)
    ap.add_argument('--namecheck', default=os.path.join(paths.REPO, '.work', 'im3', 'namecheck.tsv'))
    a = ap.parse_args()
    os.makedirs(paths.workdir('rounds'), exist_ok=True)

    rows = list(csv.DictReader(open(a.namecheck), delimiter='\t'))
    names = [r['name'] for r in rows]
    how = [''] * len(rows)
    hs = Hasher(candidates())
    print(f'candidates {len(hs.cand)}', flush=True)
    accepted = []
    b, onset, win = anchors_windows(names)
    print(f'round 0: on {len(onset)}', flush=True)
    for rnd in range(1, a.rounds + 1):
        groups = {}
        for i, n in enumerate(names):
            if win[i] is None:
                continue
            for kind, tok, s, e in tokens(n):
                if n.count(tok) != 1:
                    continue
                groups.setdefault((kind, tok), []).append((i, n[:s], n[e:]))
        todo = [(k, v) for k, v in groups.items() if len(v) >= a.min_slots]
        print(f'round {rnd}: tokens {len(todo)}, slots {sum(len(v) for _, v in todo)}', flush=True)
        changes = {}
        for gi, ((kind, tok), slots) in enumerate(sorted(todo)):
            if gi % 200 == 0:
                print(f'  {gi} {kind} {tok}', flush=True)
            count = np.zeros(len(hs.cand), dtype=np.int32)
            logw = np.zeros(len(hs.cand))
            masks = []
            for i, pre, post in slots:
                lo, hi = win[i]
                bk = hs.buckets(pre, post)
                m = (bk >= lo) & (bk <= hi)
                count += m
                logw += np.where(m, math.log((hi - lo + 1) / NB), 0.0)
                masks.append(m)
            ok = np.nonzero(count >= 2)[0]
            if not len(ok):
                continue
            # expected false hits for each candidate with k hits: C(n,k) * prod(w_i/NB) * candidates
            n = len(slots)
            score = [math.lgamma(n + 1) - math.lgamma(count[c] + 1) - math.lgamma(n - count[c] + 1) + logw[c]
                     + math.log(len(hs.cand)) for c in ok]
            c = ok[int(np.argmin(score))]
            chance = math.exp(min(score))
            x = hs.cand[c]
            if x == tok or chance >= a.chance:
                continue
            hit = [slots[k] for k in range(n) if masks[k][c]]
            accepted.append((rnd, kind, tok, x, n, len(hit), f'{chance:.2g}'))
            for i, pre, post in hit:
                if i not in changes:
                    changes[i] = (pre + x + post, f'r{rnd} {kind} {tok}->{x}')
        for i, (nn, hw) in changes.items():
            names[i] = nn
            how[i] = (how[i] + '; ' if how[i] else '') + hw
        b, onset, win = anchors_windows(names)
        print(f'round {rnd}: accepted tokens {sum(1 for t in accepted if t[0] == rnd)}, slots renamed '
              f'{len(changes)}, on {len(onset)}', flush=True)
        with open(paths.work('rounds', f'{rnd}.tsv'), 'w', newline='') as f:
            w = csv.writer(f, delimiter='\t')
            w.writerow(['slot', 'target', 'name', 'original', 'bucket', 'on_order', 'how'])
            for i, r in enumerate(rows):
                w.writerow([r['slot'], r['target'], names[i], r['name'] if names[i] != r['name'] else '', b[i],
                            'on' if i in onset else 'off', how[i]])
        with open(paths.work('rounds', 'accepted.tsv'), 'w', newline='') as f:
            w = csv.writer(f, delimiter='\t')
            w.writerow(['round', 'kind', 'ours', 'candidate', 'slots', 'hits', 'expected_by_chance'])
            w.writerows(accepted)
        if not changes:
            break


if __name__ == '__main__':
    main()
