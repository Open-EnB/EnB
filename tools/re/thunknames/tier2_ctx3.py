"""Round 3 contexts: per unconfirmed class, the destructor slots (name the only unknown), its methods, every class
name already tried and missed, confirmed relatives (name prefix, vtable family, same source file) and the strings /
source files. Writes .work/names/tier2/ctx3/<CLASS>.txt and batch files .work/names/tier2/batch3/R<nn>.txt (paths.work).

Usage: python tools/re/thunknames/tier2_ctx3.py --state F --classes LIST --batches N
"""
import argparse
import collections
import csv
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from gapguess import State, class_of  # noqa: E402
from vt import vtables  # noqa: E402
from classsearch2 import own_variants  # noqa: E402

PURE = 0x9CCD10


def parts(c):
    for pre in ('UI', 'Unknown'):
        if c.startswith(pre) and len(c) > len(pre) and c[len(pre)].isupper():
            c = c[len(pre):]
    return re.findall(r'[A-Z]+(?=[A-Z][a-z])|[A-Z]?[a-z]+|[A-Z]+|[0-9]+', c)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--classes', required=True)
    ap.add_argument('--batches', type=int, default=16)
    a = ap.parse_args()
    st = State(a.state)
    classes = open(a.classes).read().split()
    known = collections.Counter(class_of(st.names[i]) for i in st.onset)
    known.pop(None, None)
    open_by = collections.defaultdict(list)
    for i, n in enumerate(st.names):
        c = n and class_of(n)
        if c and i not in st.onset and st.window(i):
            open_by[c].append(i)
    tried = collections.defaultdict(list)
    for f in glob.glob(os.path.join(paths.PROPOSALS, '**', '*.tsv'), recursive=True):
        for line in open(f):
            p = [x.strip() for x in line.rstrip('\n').split('\t')]
            if len(p) >= 3 and p[0] == 'CLASS' and p[2] not in tried[p[1]]:
                tried[p[1]].append(p[2])
    # source files and strings
    file_of = {}
    for line in open(paths.ASSERTS, encoding='latin1'):
        p = line.rstrip('\n').split('\t')
        if len(p) >= 2 and p[0].startswith('0x'):
            file_of[int(p[0], 16)] = p[1].replace('e:\\build\\beta\\G\\', '').replace('\\', '/')
    strs = collections.defaultdict(list)
    for r in csv.DictReader(open(paths.work('strrefs.tsv')), delimiter='\t'):
        strs[int(r['target'], 16)].append((int(r['string_va'], 16), r['string']))
    tgt = {i: t for i, (s, t) in enumerate(st.table)}
    file_cls = collections.defaultdict(collections.Counter)
    for i in st.onset:
        c = class_of(st.names[i])
        if c:
            for sva, _ in strs.get(tgt[i], []):
                if sva in file_of:
                    file_cls[file_of[sva]][c] += 1
    # vtable families -> confirmed classes
    vts = [(addr, sym, ents) for addr, sym, ents in vtables() if sym.endswith('@@6B@')]
    at = collections.defaultdict(set)
    for vi, (_, _, ents) in enumerate(vts):
        for k, e in enumerate(ents):
            if e != PURE:
                at[(e, k)].add(vi)
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
    vt_cls = {}
    for vi, (_, sym, ents) in enumerate(vts):
        cnt = collections.Counter()
        for e in ents:
            i = st.index.get(e)
            if i is not None and st.names[i]:
                c = class_of(st.names[i])
                if c:
                    cnt[(c, i in st.onset)] += 1
        vt_cls[vi] = cnt
    fam_conf = collections.defaultdict(set)    # our class -> confirmed classes in its vtable family
    fam_of = collections.defaultdict(set)
    for vi in range(len(vts)):
        fam_of[find(vi)].add(vi)
    for root, members in fam_of.items():
        conf = {c for v in members for (c, on), _ in vt_cls[v].items() if on and c in known}
        ours = {c for v in members for (c, on), _ in vt_cls[v].items()}
        for c in ours:
            fam_conf[c] |= conf - {c}

    texts = []
    for C in classes:
        slots = open_by.get(C, [])
        if not slots:
            continue
        names = [st.names[i] for i in slots]
        dt = [n for n in names if n.startswith(('??_G', '??_E', '??1'))]
        meth = [n for n in names if n not in dt]
        out = [f'===== {C} =====']
        if dt:
            out.append('DESTRUCTOR SLOTS (the class name is the ONLY unknown here; signature fixed by the compiler): '
                       + ' '.join(sorted(set(dt))))
        out.append(f'METHODS ({len(meth)}, our decorated names): ' + ' '.join(meth[:20]))
        t = tried.get(C, [])
        out.append(f'ALREADY TRIED, ALL WRONG ({len(t)} model names + ~{len(own_variants(C))} mechanical variants '
                   f'of our name: +/- Class/Struct/Manager/Interface, UI/Unknown prefix dropped, abbreviations '
                   f'expanded): ' + (', '.join(t) if t else '(model: none)'))
        p = parts(C)
        rel = set()
        if p:
            pre = p[0] if len(p[0]) > 3 or len(p) == 1 else ''.join(p[:2])
            rel = {k for k in known if k.startswith(pre) and k != C}
        fc = fam_conf.get(C, set())
        files = collections.Counter()
        ss = []
        for i in slots:
            for sva, s in strs.get(tgt[i], []):
                if sva in file_of:
                    files[file_of[sva]] += 1
                elif s not in ss:
                    ss.append(s)
        sib = collections.Counter()
        for f in files:
            sib.update(file_cls[f])
        if rel:
            out.append('CONFIRMED RETAIL CLASSES WITH THE SAME NAME START: ' + ', '.join(sorted(rel)[:12]))
        if fc:
            out.append('CONFIRMED RETAIL CLASSES IN ITS VTABLE FAMILY (base / derived): ' + ', '.join(sorted(fc)[:12]))
        if files:
            out.append('SOURCE FILES: ' + ', '.join(f for f, _ in files.most_common(3)))
        if sib:
            out.append('CONFIRMED RETAIL CLASSES IN THOSE FILES: ' + ', '.join(c for c, _ in sib.most_common(8)))
        if ss:
            out.append('STRINGS: ' + ' | '.join(x[:60] for x in ss[:8]))
        txt = '\n'.join(out) + '\n'
        open(paths.work('tier2', 'ctx3', f'{C}.txt'), 'w').write(txt)
        texts.append(txt)
    for b in range(a.batches):
        part = texts[b::a.batches]
        open(paths.work('tier2', 'batch3', f'R{b:02d}.txt'), 'w').write('\n'.join(part))
        print(f'R{b:02d}: {len(part)} classes, {sum(len(x) for x in part)} chars')


if __name__ == '__main__':
    main()
