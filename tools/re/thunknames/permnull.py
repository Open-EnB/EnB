"""The salted null: the same class-rename test with names that cannot be retail's (review, 2026-11-26).

The cross-class null (another class's candidates) is not a null: sibling classes share a vocabulary, and its two
"hits" (StringValueClass with KVStringAccessorClass, DeltaClass with DeltaItem) read like true names. Here every
candidate X is tested as X + two salt characters, K salts at once: the slots, their windows, the method names and
signatures around the class, the access / const forms and the candidate's own letters all stay, so whatever
correlation the hash leaves between one class's slots is in the null too; only the truth is gone.

  salted(st, slots, c, x, K, seed) -> (k[K], chance[K]): per salt, the near hits and tier2_eval's modelled chance
                                      (C(n, k) x prod(forms x prob(near)), WITHOUT the candidates-tried factor)

Used by tier2_eval.py (--perm K) and rescore.py. Needs numpy.
"""
import math
import os
import sys
import zlib

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402,F401  (puts tools/re on sys.path)
from linklayout import LHASH_PRIME  # noqa: E402
from gapguess import MAXP, P, prob  # noqa: E402
from mangle import rename  # noqa: E402

MARK = '\x01'
ALPHA = np.frombuffer(b'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_', dtype=np.uint8)
M32 = np.uint64(0xFFFFFFFF)


def salts(k, seed):
    """k distinct two-character salts as a (k, 2) array of character codes."""
    rng = np.random.RandomState(seed & 0x7FFFFFFF)
    pick = rng.choice(len(ALPHA) ** 2, size=k, replace=False)
    return np.stack([ALPHA[pick // len(ALPHA)], ALPHA[pick % len(ALPHA)]], axis=1).astype(np.uint64)


def _step(h, c):
    return (h + ((c << np.uint64(18)) & M32) + c + ((h >> np.uint64(1)) ^ h)) & M32


def buckets(template, salt):
    """The bucket of `template` with every MARK replaced by each salt: an array over the salts."""
    h = np.zeros(len(salt), dtype=np.uint64)
    for ch in template:
        if ch == MARK:
            h = _step(_step(h, salt[:, 0]), salt[:, 1])
        else:
            h = _step(h, np.uint64(ord(ch)))
    h = h % np.uint64(LHASH_PRIME)
    b = h % np.uint64(MAXP)
    return np.where(b < P, h % np.uint64(2 * MAXP), b).astype(np.int64)


def log_comb(n):
    return np.array([math.lgamma(n + 1) - math.lgamma(k + 1) - math.lgamma(n - k + 1) for k in range(n + 1)])


def salted(windows, names, c, x, variants, salt):
    """windows: [(nlo, nhi)] per slot; names: the slots' current names; the class c is renamed to x + salt in
    every spelling, each form of variants() tried, a slot counts once. -> (k, chance) arrays over the salts."""
    k = np.zeros(len(salt), dtype=np.int64)
    logw = np.zeros(len(salt))
    for (nlo, nhi), nm in zip(windows, names):
        vs = variants(rename(nm, c, x + MARK))
        if not any(MARK in v for v in vs):
            continue
        hit = np.zeros(len(salt), dtype=bool)
        for v in vs:
            b = buckets(v, salt)
            hit |= (b >= nlo) & (b <= nhi)
        k += hit
        logw += np.where(hit, math.log(len(vs) * prob(nlo, nhi)), 0.0)
    lc = log_comb(len(names))[k] + logw
    return k, np.where(k > 0, np.exp(np.minimum(lc, 0.0)), 1.0)


def seed_of(name, seed):
    return zlib.crc32(name.encode()) + seed


def _selftest():
    from linklayout import lhash, bucket
    s = salts(50, 1)
    t = '??1Foo' + MARK + '@@UAE@XZ'
    got = buckets(t, s)
    for j in range(len(s)):
        nm = t.replace(MARK, chr(int(s[j, 0])) + chr(int(s[j, 1])))
        assert got[j] == bucket(lhash(nm), MAXP, P), nm
    return len(s)


if __name__ == '__main__':
    print(f'permnull self-test: {_selftest()} salted names hash as linklayout.lhash does')
