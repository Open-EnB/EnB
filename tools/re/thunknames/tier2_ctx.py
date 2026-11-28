"""Tier 2 context: a compact text per class for a model to propose retail names from.

Per class C (our qualifier): its open slots' decorated names, the strings its functions reference, the source
files those strings name (asserts.txt), the anchored (retail-exact) classes whose functions reference strings of
the same files, its vtable relatives' anchored classes, and one pseudocode excerpt (constructor first).

Usage: python tools/re/thunknames/tier2_ctx.py --state F CLASS... -> .work/names/tier2/ctx/<CLASS>.txt (paths.work)
"""
import argparse
import collections
import csv
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from gapguess import State, class_of  # noqa: E402

PSEUDO = paths.PSEUDO


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--lines', type=int, default=30)
    ap.add_argument('classes', nargs='+')
    a = ap.parse_args()
    st = State(a.state)
    tgt_of = {i: t for i, (s, t) in enumerate(st.table)}
    file_of = {}
    for line in open(paths.ASSERTS, encoding='latin1'):
        p = line.rstrip('\n').split('\t')
        if len(p) >= 2 and p[0].startswith('0x'):
            file_of[int(p[0], 16)] = p[1].replace('e:\\build\\beta\\G\\', '').replace('\\', '/')
    strs = collections.defaultdict(list)       # target -> [(string va, string)]
    for r in csv.DictReader(open(paths.work('strrefs.tsv')), delimiter='\t'):
        strs[int(r['target'], 16)].append((int(r['string_va'], 16), r['string']))
    # anchored classes per source file
    file_cls = collections.defaultdict(collections.Counter)
    for i in st.onset:
        c = class_of(st.names[i])
        if not c:
            continue
        for sva, _ in strs.get(tgt_of[i], []):
            if sva in file_of:
                file_cls[file_of[sva]][c] += 1
    for C in a.classes:
        slots = [i for i, n in enumerate(st.names) if n and class_of(n) == C and i not in st.onset and st.window(i)]
        out = [f'CLASS (our name, a guess): {C}', f'OPEN SLOTS ({len(slots)}): our decorated names (MSVC 6 '
               f'mangling; method names Unknown_XX are vtable offsets we could not name)']
        for i in slots[:40]:
            out.append('  ' + st.names[i])
        ss, files = [], collections.Counter()
        for i in slots:
            for sva, s in strs.get(tgt_of[i], []):
                if sva in file_of:
                    files[file_of[sva]] += 1
                elif s not in ss:
                    ss.append(s)
        if files:
            out.append('SOURCE FILES named by its asserts: ' + ', '.join(f for f, _ in files.most_common(4)))
            sib = collections.Counter()
            for f in files:
                sib.update(file_cls[f])
            if sib:
                out.append('RETAIL-EXACT classes with code in those files: ' +
                           ', '.join(c for c, _ in sib.most_common(8)))
        if ss:
            out.append('STRINGS its functions use: ' + ' | '.join(x[:70] for x in ss[:12]))
        # pseudocode: constructor first, else the open slot with the longest pseudocode under 4 KB
        pick = None
        for i in slots:
            if st.names[i].startswith('??0'):
                pick = i
                break
        if pick is None:
            best = 0
            for i in slots:
                p = os.path.join(PSEUDO, f'{tgt_of[i]:08X}.c')
                if os.path.exists(p):
                    sz = os.path.getsize(p)
                    if best < sz < 4096:
                        best, pick = sz, i
        if pick is not None:
            p = os.path.join(PSEUDO, f'{tgt_of[pick]:08X}.c')
            if os.path.exists(p):
                lines = [ln.rstrip() for ln in open(p, encoding='latin1') if ln.strip()]
                out.append(f'PSEUDOCODE of {st.names[pick]} (Ghidra, first {a.lines} lines):')
                out += ['  ' + ln[:110] for ln in lines[1:1 + a.lines]]
        txt = '\n'.join(out) + '\n'
        open(paths.work('tier2', 'ctx', f'{C}.txt'), 'w').write(txt)
        print(f'{C}: slots {len(slots)}, {len(txt)} chars')


if __name__ == '__main__':
    main()
