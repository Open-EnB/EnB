"""Tier 1b: names from the strings a function references (strrefs.tsv): `Class::Method` in a log / assert /
profile string. Calibration first: for functions whose thunk slot is an anchor, how often the string's class and
method are the anchor's. Guesses for open slots (off or unnamed, with a window):
  named   our signature kept, class and method from the string, access variants (members: Q A I U M E, the
          return-and-args part kept; a static member S / K / C)
  unnamed common signatures (COMMON) with each access
Writes .work/names/tier1b/guesses.tsv.

Usage: python tools/re/thunknames/tier1b.py [--state F]
"""
import argparse
import collections
import csv
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from gapguess import State, DEFAULT_STATE  # noqa: E402

CM = re.compile(r'(?<![A-Za-z0-9_:])([A-Za-z_][A-Za-z0-9_]*)::(~?[A-Za-z_][A-Za-z0-9_]*)')
MEMBER = re.compile(r'^\?(?P<m>[A-Za-z_][A-Za-z0-9_]*)@(?P<c>[A-Za-z_][A-Za-z0-9_]*)@@(?P<acc>[A-Z])(?P<rest>.*)$')
CTOR = re.compile(r'^\?\?(?P<k>[01])(?P<c>[A-Za-z_][A-Za-z0-9_]*)@@(?P<acc>[A-Z])(?P<rest>.*)$')
# access letter -> the part after it starts with the this-cv (A / B) for non-static members
ACC_THIS = 'QAIUME'
ACC_STATIC = 'SKC'
# frequent signatures after the access letter (this-cv + calling convention + return + args)
COMMON = ['AEXXZ', 'AE_NXZ', 'AEHXZ', 'AEXH@Z', 'AEXPAX@Z', 'AE_NH@Z', 'AEXM@Z', 'AEMXZ', 'AEPBDXZ',
          'AEXPBD@Z', 'BEHXZ', 'BE_NXZ', 'BEMXZ', 'AEHH@Z', 'AEX_N@Z', 'AEKXZ', 'BEKXZ', 'AEXK@Z']


def parse(name):
    m = MEMBER.match(name)
    if m:
        return 'm', m.group('m'), m.group('c'), m.group('acc'), m.group('rest')
    m = CTOR.match(name)
    if m:
        return m.group('k'), None, m.group('c'), m.group('acc'), m.group('rest')
    return None


def build(kind, meth, cls, acc, rest):
    if kind in '01':
        return f'??{kind}{cls}@@{acc}{rest}'
    return f'?{meth}@{cls}@@{acc}{rest}'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=DEFAULT_STATE)
    ap.add_argument('--out', default=paths.workdir('tier1b'))
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    st = State(a.state)
    print(st.summary(), flush=True)
    refs = collections.defaultdict(set)
    for r in csv.DictReader(open(paths.work('strrefs.tsv')), delimiter='\t'):
        if not r['slot']:
            continue
        for c, m in CM.findall(r['string']):
            refs[int(r['slot'], 16)].add((c, m))
    # calibration on anchors
    cal = collections.Counter()
    for s, cms in refs.items():
        i = st.index.get(s)
        if i is None or i not in st.onset:
            continue
        p = parse(st.names[i])
        if not p:
            cal['anchor not a simple member'] += 1
            continue
        kind, meth, cls, *_ = p
        meth = meth if kind == 'm' else (cls if kind == '0' else '~' + cls)
        best = 'neither'
        for c, m in cms:
            if c == cls and m == meth:
                best = 'class+method'
                break
            if c == cls:
                best = 'class only'
            elif m == meth and best == 'neither':
                best = 'method only'
        cal[best] += 1
    print('calibration (anchored slots with a Class::Method string):', dict(cal), flush=True)

    rows = []
    for s, cms in refs.items():
        i = st.index.get(s)
        if i is None or i in st.onset or st.window(i) is None:
            continue
        cur = st.names[i]
        p = parse(cur) if cur else None
        for c, m in cms:
            if m.startswith('~'):
                kinds = [('1', None, m[1:])] if m[1:] == c else []
            elif m == c:
                kinds = [('0', None, c)]
            else:
                kinds = [('m', m, c)]
            for kind, meth, cls in kinds:
                if p and p[0] == kind or (p and kind == 'm' and p[0] == 'm'):
                    acc0, rest = p[3], p[4]
                    if acc0 in ACC_THIS:
                        for acc in ACC_THIS:
                            rows.append([f'0x{s:X}', build(kind, meth, cls, acc, rest), 't1b-str-named',
                                         f'{c}::{m}; ours {cur}'])
                    elif acc0 in ACC_STATIC:
                        for acc in ACC_STATIC:
                            rows.append([f'0x{s:X}', build(kind, meth, cls, acc, rest), 't1b-str-named',
                                         f'{c}::{m}; ours {cur}'])
                    else:
                        rows.append([f'0x{s:X}', build(kind, meth, cls, acc0, rest), 't1b-str-named',
                                     f'{c}::{m}; ours {cur}'])
                elif kind == 'm':
                    for sig in COMMON:
                        for acc in 'QAIU':
                            rows.append([f'0x{s:X}', build(kind, meth, cls, acc, sig), 't1b-str-common',
                                         f'{c}::{m}; ours {cur or "-"}'])
                elif kind == '1':
                    for acc in 'QU':
                        rows.append([f'0x{s:X}', f'??1{cls}@@{acc}AE@XZ', 't1b-str-common',
                                     f'{c}::{m}; ours {cur or "-"}'])
    rows = [list(r) for r in dict.fromkeys(tuple(r[:2]) + tuple(r[2:]) for r in rows)]
    with open(os.path.join(a.out, 'guesses.tsv'), 'w', newline='') as fo:
        w = csv.writer(fo, delimiter='\t')
        w.writerow(['slot', 'name', 'tier', 'note'])
        w.writerows(rows)
    by = collections.Counter(r[2] for r in rows)
    print(f'guesses {len(rows)} for {len(set(r[0] for r in rows))} slots: {dict(by)}')


if __name__ == '__main__':
    main()
