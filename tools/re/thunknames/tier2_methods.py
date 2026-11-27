"""Method names from the local model, for `Unknown_XX` methods of classes whose retail name is confirmed
(HANDOFF.md brief B; the class and the signature are known, the method name is the only unknown).

One slot gives about 11 bits (a near part is about 3e-4), too few to find a name among many, so:
  override groups   slots at the same vtable index of one vtable family with the same signature share ONE name:
                    a candidate hitting k >= 2 of them at C(n, k) x prod(forms x prob) x candidates < --chance
                    is accepted as `t2-lvmethod` (a confirmed anchor);
  single slots      exactly one of the slot's first --top candidates of each round near (the pilot: 21 of 35
                    finds were in the first 4), at those candidates x forms x prob <= --single:
                    accepted as `t2-lmethod`, which gapguess grades PROBABLE (it bounds no window and is no
                    evidence for a source rename by itself).
The salted null (every candidate + two salt characters, K salts) gives the expected false single-slot hits:
`score` prints it next to the real count. Read that line before trusting a round.

  jobs     the targets and their contexts (class, confirmed methods of the class, signature, strings, Ghidra
           pseudocode from .work/pseudo/)                        -> .work/names/methods/<tag>/jobs.json
  propose  the local model's candidates, one request per target  -> proposals/<tag>/M00.tsv (METHOD lines)
  score    the hash test and the null                            -> .work/names/methods/<tag>/guesses.tsv
  pilot    recall on anchored methods (their names hidden): is the model worth a round?

Usage: python tools/re/thunknames/tier2_methods.py jobs|propose|score --tag T [--samples 2] [--limit N]
       python tools/re/thunknames/tier2_methods.py pilot [--n-pilot 80] [--samples 2]
"""
import argparse
import collections
import concurrent.futures
import csv
import json
import math
import os
import random
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402
import tier2_eval  # noqa: E402
import tier2_local  # noqa: E402
from gapguess import State, bk, class_of, prob  # noqa: E402

SIMPLE = re.compile(r'^\?(?P<m>[A-Za-z_][A-Za-z0-9_]*)@(?P<c>[A-Za-z_][A-Za-z0-9_]*)@@(?P<acc>[A-Z])(?P<rest>.*)$')
PURE = 0x9CCD10
PSEUDO_MAX = 2600

SYSTEM = """We recover the original C++ method names of the game Earth & Beyond's client.exe (Westwood Studios,
2002, MSVC 6). A hash oracle checks every proposal exactly, so a wrong guess costs nothing: give breadth.

You get one method whose name is unknown (shown as Unknown_...): its class (the real retail name), the class's
confirmed retail method names (they show the style), the method's decorated signature, strings it references and
its decompiled body (Ghidra; other functions in it may carry placeholder names).

Propose {n} candidate names for the method, most likely first. Westwood's style: Words_With_Underscores, each word
capitalised (Get_Render_State_List, Set_Visible, Is_Enabled, On_Frame_Update, Add_Object, Process_Packet,
Find_Item, Update, Reset, Init, Shutdown); accessors are Get_X / Set_X / Is_X / Has_X / Peek_X for a member X;
handlers On_X / Handle_X; never MFC's style (GetFoo, OnFoo, m_foo). Vary the verb and the noun: give synonyms
(Get_Count, Get_Num, Count, Num_Items), shorter and longer forms.

Output only the candidate names, one per line, no numbering, no commentary."""


def pseudo(target, hide=()):
    p = os.path.join(paths.PSEUDO, f'{target:08X}.c')
    if not os.path.exists(p):
        return ''
    s = open(p, errors='replace').read()
    s = s.split('\n', 1)[1] if s.startswith('//') else s
    for h in hide:
        s = s.replace(h, 'Unknown_Method')
    s = re.sub(r'\n\s*\n', '\n', s).strip()
    return s[:PSEUDO_MAX] + ('\n... (cut)' if len(s) > PSEUDO_MAX else '')


def load_strs():
    strs = collections.defaultdict(list)
    f = paths.work('strrefs.tsv')
    if os.path.exists(f):
        for r in csv.DictReader(open(f), delimiter='\t'):
            if r['string'] not in strs[int(r['target'], 16)]:
                strs[int(r['target'], 16)].append(r['string'])
    return strs


def class_methods(st):
    """{class: [confirmed method names]} from the confirmed anchors."""
    out = collections.defaultdict(list)
    for i in st.ai:
        m = SIMPLE.match(st.names[i])
        if m and m.group('m') not in out[m.group('c')]:
            out[m.group('c')].append(m.group('m'))
    return out


def all_tried(names=None, top=0):
    """{slot's current name: [every method candidate in proposals/, in file order]}; with `top`, only the
    first `top` candidates each file gives the slot (the single-slot rule's list: every round adds `top`)."""
    import glob
    out = collections.defaultdict(list)
    for f in sorted(glob.glob(os.path.join(paths.PROPOSALS, '**', '*.tsv'), recursive=True)):
        seen = collections.Counter()
        for line in open(f):
            p = line.rstrip('\n').split('\t')
            if len(p) == 4 and p[0] == 'METHOD' and (names is None or p[2] in names):
                seen[p[2]] += 1
                if (not top or seen[p[2]] <= top) and p[3] not in out[p[2]]:
                    out[p[2]].append(p[3])
    return out


def context(cls, name, shown, target, methods, strs, hide=(), tried=()):
    lines = [f'CLASS: {cls}',
             'CONFIRMED METHODS OF THE CLASS: ' + (', '.join(methods[:40]) if methods else '(none)'),
             f'THE METHOD (decorated, name unknown): {shown}']
    if tried:
        lines.append('ALREADY TRIED, ALL WRONG (do not repeat): ' + ', '.join(tried[-80:]))
    ss = [s[:70] for s in strs.get(target, [])[:8] if not any(h in s for h in hide)]
    if ss:
        lines.append('STRINGS IT REFERENCES: ' + ' | '.join(ss))
    ps = pseudo(target, hide)
    lines.append('DECOMPILED:\n' + ps if ps else 'DECOMPILED: (not available)')
    return '\n'.join(lines)


def override_groups(st):
    """{slot index: group id} for open slots at the same vtable index of one family (vt.py; tier1a's families)."""
    from vt import vtables
    vts = [(a, s, e) for a, s, e in vtables() if s.endswith('@@6B@')]
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
    grp = {}
    for vi, (_, _, ents) in enumerate(vts):
        for k, e in enumerate(ents):
            i = st.index.get(e)
            if i is not None and i not in grp:
                grp[i] = f'{find(vi)}:{k}'
    return grp


def targets(st):
    """Open slots named ?Unknown...@C@@... whose class C has a confirmed anchor: [(slot index, match)]."""
    known = collections.Counter(class_of(st.names[i]) for i in st.ai)
    out = []
    for i, n in enumerate(st.names):
        if not n or i in st.onset or not st.window(i):
            continue
        m = SIMPLE.match(n)
        if m and m.group('m').startswith('Unknown') and known[m.group('c')] and 'Unknown' not in m.group('c'):
            out.append((i, m))
    return out


def cmd_jobs(a):
    st = State()
    strs, meths, grp = load_strs(), class_methods(st), override_groups(st)
    tried = all_tried()
    jobs = []
    for i, m in targets(st):
        tgt = st.table[i][1]
        g = grp.get(i)
        jobs.append(dict(slot=f'0x{st.table[i][0]:X}', name=st.names[i], cls=m.group('c'),
                         group=f'{g}:{m.group("acc")}{m.group("rest")}' if g else '',
                         ctx=context(m.group('c'), st.names[i], st.names[i], tgt, meths[m.group('c')], strs,
                                     tried=tried.get(st.names[i], ()))))
    gs = collections.Counter(j['group'] for j in jobs if j['group'])
    out = os.path.join(paths.workdir('methods', a.tag), 'jobs.json')
    json.dump(jobs, open(out, 'w'))
    print(f'{out}: {len(jobs)} targets; override groups with >= 2 targets: {sum(v >= 2 for v in gs.values())} '
          f'({sum(v for v in gs.values() if v >= 2)} slots); with pseudocode '
          f'{sum("(not available)" not in j["ctx"] for j in jobs)}')


def ask_all(a, jobs, key):
    """jobs: dicts with `ctx`; -> {job[key]: [names]}, totals."""
    work = [(j, s) for j in jobs for s in range(a.samples)]
    out = collections.defaultdict(list)
    tot = dict(tokens_in=0, tokens_out=0, failed=0, requests=len(work))
    t0 = time.time()

    def one(w):
        j, s = w
        return j[key], tier2_local.ask(a.url, a.model, 'off', a.n, j['ctx'], a.temperature, a.seed + s,
                                       system=SYSTEM)
    with concurrent.futures.ThreadPoolExecutor(a.parallel) as ex:
        for f in concurrent.futures.as_completed([ex.submit(one, w) for w in work]):
            try:
                k, (names, pi, po, _) = f.result()
            except Exception as e:
                tot['failed'] += 1
                if tot['failed'] <= 5:
                    print(f'  request failed: {e}', flush=True)
                continue
            tot['tokens_in'] += pi
            tot['tokens_out'] += po
            for x in names:
                if x not in out[k] and not x.startswith('Unknown'):
                    out[k].append(x)
    tot['seconds'] = round(time.time() - t0)
    return out, tot


def cmd_propose(a):
    jobs = json.load(open(os.path.join(paths.workdir('methods', a.tag), 'jobs.json')))
    if a.limit:
        jobs = jobs[:a.limit]
    out_f = os.path.join(paths.PROPOSALS, a.tag, 'M00.tsv')
    if os.path.exists(out_f):
        raise SystemExit(f'{out_f} exists: a tag is used once')
    os.makedirs(os.path.dirname(out_f), exist_ok=True)
    out, tot = ask_all(a, jobs, 'slot')
    by = {j['slot']: j for j in jobs}
    with open(out_f, 'w', newline='') as f:
        for s, names in out.items():
            for x in names:
                f.write(f'METHOD\t{by[s]["cls"]}\t{by[s]["name"]}\t{x}\n')
    print(f'{out_f}: {len(out)} of {len(jobs)} targets, {sum(len(v) for v in out.values())} candidates; {tot}')


def renamed(name, m):
    mm = SIMPLE.match(name)
    return '?' + m + name[mm.end('m'):]


def cmd_score(a):
    import numpy as np
    import permnull
    tier2_eval.CONST = True
    st = State()
    wdir = paths.workdir('methods', a.tag)
    jobs = {j['name']: j for j in json.load(open(os.path.join(wdir, 'jobs.json')))}
    cands = all_tried(set(jobs))        # every round's candidates: all of them were tried, all of them count
    first = all_tried(set(jobs), a.top)  # the single-slot rule's shorter list
    slot_of = {st.names[i]: i for i in range(st.n) if st.names[i] in cands and i not in st.onset and st.window(i)}
    salt = permnull.salts(a.perm, a.seed)
    rows, null_single, n_single = [], 0.0, 0
    near = {}           # name -> {candidate: variant that is near}
    for nm, i in slot_of.items():
        lo, hi, nlo, nhi = st.window(i)
        near[nm] = {}
        nullhits = np.zeros(len(salt), dtype=np.int64)
        for m in first[nm]:
            vs = tier2_eval.variants(renamed(nm, m))
            for v in vs:
                if nlo <= bk(v) <= nhi:
                    near[nm][m] = v
                    break
            h = np.zeros(len(salt), dtype=bool)
            for v in tier2_eval.variants(renamed(nm, m + permnull.MARK)):
                b = permnull.buckets(v, salt)
                h |= (b >= nlo) & (b <= nhi)
            nullhits += h
        forms = len(tier2_eval.variants(nm))
        ch = len(first[nm]) * forms * prob(nlo, nhi)
        if ch <= a.single:
            null_single += float((nullhits == 1).mean())
            if len(near[nm]) == 1:
                n_single += 1
                (m, v), = near[nm].items()
                rows.append([f'0x{st.table[i][0]:X}', v, 't2-lmethod', f'{nm} -> {m}; chance {ch:.2g} {a.tag}'])
    # override groups: one name on >= 2 slots
    groups = collections.defaultdict(list)
    for nm in slot_of:
        if jobs[nm]['group']:
            groups[jobs[nm]['group']].append(nm)
    n_group = 0
    taken = set()
    for g, members in groups.items():
        if len(members) < 2:
            continue
        union = []
        for nm in members:
            union += [m for m in cands[nm] if m not in union]
        best = None
        for m in union:
            hit, logw = [], 0.0
            for nm in members:
                i = slot_of[nm]
                lo, hi, nlo, nhi = st.window(i)
                vs = tier2_eval.variants(renamed(nm, m))
                v = next((v for v in vs if nlo <= bk(v) <= nhi), None)
                if v:
                    hit.append((i, v))
                    logw += math.log(len(vs) * prob(nlo, nhi))
            n, k = len(members), len(hit)
            if k >= 2:
                lc = math.lgamma(n + 1) - math.lgamma(k + 1) - math.lgamma(n - k + 1) + logw + math.log(len(union))
                if best is None or lc < best[0]:
                    best = (lc, m, hit)
        if best and math.exp(best[0]) < a.chance:
            n_group += 1
            for i, v in best[2]:
                taken.add(f'0x{st.table[i][0]:X}')
                rows.append([f'0x{st.table[i][0]:X}', v, 't2-lvmethod',
                             f'override group {g} -> {best[1]}; chance {math.exp(best[0]):.2g} {a.tag}'])
    rows = [r for r in rows if not (r[2] == 't2-lmethod' and r[0] in taken)]
    out = os.path.join(wdir, 'guesses.tsv')
    with open(out, 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['slot', 'name', 'tier', 'note'])
        w.writerows(rows)
    singles = sum(r[2] == 't2-lmethod' for r in rows)
    print(f'scored {len(slot_of)} slots, {sum(len(cands[n]) for n in slot_of)} candidates')
    print(f'override groups accepted: {n_group} ({sum(r[2] == "t2-lvmethod" for r in rows)} slots, confirmed grade)')
    print(f'single-slot hits: {singles} (probable grade); the salted null expects {null_single:.1f} false among '
          f'slots judged the same way: about {100 * min(1.0, null_single / max(1, n_single)):.0f} % of them')
    print(f'guesses: {out}')
    for r in rows[:a.show]:
        print('  ' + r[2] + '  ' + r[1] + '   (' + r[3].split(';')[0] + ')')


def cmd_pilot(a):
    """Recall on confirmed anchors: hide the method name, ask, look for it among the candidates."""
    st = State()
    strs, meths = load_strs(), class_methods(st)
    pool = []
    for i in st.ai:
        m = SIMPLE.match(st.names[i])
        if m and not m.group('m').startswith('Unknown') and len(meths[m.group('c')]) >= 3 \
                and os.path.exists(os.path.join(paths.PSEUDO, f'{st.table[i][1]:08X}.c')):
            pool.append((i, m))
    random.Random(a.seed).shuffle(pool)
    jobs = []
    for i, m in pool[:a.n_pilot]:
        meth, cls = m.group('m'), m.group('c')
        shown = '?Unknown_Method' + st.names[i][m.end('m'):]
        jobs.append(dict(key=st.names[i], truth=meth,
                         ctx=context(cls, st.names[i], shown, st.table[i][1], [x for x in meths[cls] if x != meth],
                                     strs, hide=(meth,))))
    out, tot = ask_all(a, jobs, 'key')
    ranks = sorted(out[j['key']].index(j['truth']) + 1 for j in jobs if j['truth'] in out[j['key']])
    ncand = sum(len(v) for v in out.values())
    print(f'method pilot: {len(jobs)} anchored methods with their names hidden, {a.samples} request(s) each, '
          f'{a.n} names asked')
    print(f'  found {len(ranks)} of {len(jobs)} (ranks {ranks}); candidates per method {ncand / max(1, len(jobs)):.0f}; '
          f'{tot}')
    for j in jobs[:a.show]:
        print(f'  {"FOUND" if j["truth"] in out[j["key"]] else "miss "} {j["key"][:60]:60s} {", ".join(out[j["key"]][:5])}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=('jobs', 'propose', 'score', 'pilot'))
    ap.add_argument('--tag')
    ap.add_argument('--url', default='http://127.0.0.1:8080/v1/chat/completions')
    ap.add_argument('--model', default='qwen3.8-27b')
    ap.add_argument('--parallel', type=int, default=16)
    ap.add_argument('--n', type=int, default=15, help='names asked per request')
    ap.add_argument('--samples', type=int, default=2)
    ap.add_argument('--temperature', type=float, default=1.0)
    ap.add_argument('--seed', type=int, default=0)
    ap.add_argument('--limit', type=int, default=0, help='propose: only the first N targets (a trial)')
    ap.add_argument('--chance', type=float, default=1e-4, help='override groups')
    ap.add_argument('--single', type=float, default=0.03, help='single slots: candidates x forms x prob at most this')
    ap.add_argument('--top', type=int, default=5, help='single slots: the first N candidates of each round')
    ap.add_argument('--perm', type=int, default=200)
    ap.add_argument('--n-pilot', type=int, default=80)
    ap.add_argument('--show', type=int, default=12)
    a = ap.parse_args()
    if a.cmd != 'pilot' and not a.tag:
        ap.error('--tag is required')
    dict(jobs=cmd_jobs, propose=cmd_propose, score=cmd_score, pilot=cmd_pilot)[a.cmd](a)


if __name__ == '__main__':
    main()
