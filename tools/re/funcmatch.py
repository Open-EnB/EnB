"""Names client.exe functions after EA's published Westwood sources, anchored on shared strings.

A client function that references the same string literals / assert expressions as one EA
function is that function (or its descendant). Strings shared by many EA functions are weighted
down. Output: a JSON of every match and a Markdown summary.

    python funcmatch.py [--client PATH] [--out DIR]
"""
import argparse
import bisect
import collections
import json
import os

from clientimage import ClientImage, DEFAULT_CLIENT
from easource import LIBRARIES, strings_by_function, walk_library

MIN_LEN = 6


def index_ea():
    """{(lib, file, function): {'tags': set, 'strings': set}}"""
    idx = {}
    for tag, path, lib in LIBRARIES:
        for rel, full in walk_library(path):
            fname = rel.split('/')[-1].lower()
            for func, strs in strings_by_function(full).items():
                if not func:
                    continue
                key = (lib, fname, func)
                e = idx.setdefault(key, {'tags': set(), 'strings': set()})
                e['tags'].add(tag)
                e['strings'] |= {s for s in strs if len(s) >= MIN_LEN}
    return idx


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    ap.add_argument('--out', default=os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data')))
    args = ap.parse_args()
    os.makedirs(args.out, exist_ok=True)

    img = ClientImage(args.client)
    cstrings = img.strings(4)
    by_text = collections.defaultdict(list)
    for va, s in cstrings.items():
        by_text[s].append(va)

    ea = index_ea()
    owners = collections.defaultdict(set)  # string -> EA function keys
    for key, e in ea.items():
        for s in e['strings']:
            if s in by_text:
                owners[s].add(key)
    print(f'EA functions: {len(ea)}; strings shared with the client: {len(owners)}')

    starts, thunks = img.function_starts()
    print(f'client functions (incremental thunks): {len(starts)}')
    vas = {va: s for s, l in by_text.items() if s in owners for va in l}
    refs = img.dword_refs(vas.keys())

    per_client = collections.defaultdict(lambda: collections.defaultdict(float))
    evidence = collections.defaultdict(lambda: collections.defaultdict(list))
    for va, sites in refs.items():
        s = vas[va]
        keys = owners[s]
        w = 1.0 / len(keys)
        for site in sites:
            i = bisect.bisect_right(starts, site) - 1
            if i < 0:
                continue
            fn = starts[i]
            for key in keys:
                if s not in evidence[fn][key]:
                    per_client[fn][key] += w
                    evidence[fn][key].append(s)

    matches = []
    for fn, scores in per_client.items():
        ranked = sorted(scores.items(), key=lambda kv: (-kv[1], kv[0]))   # ties: by name, so reruns agree
        (best, score), second = ranked[0], (ranked[1][1] if len(ranked) > 1 else 0.0)
        n = len(evidence[fn][best])
        if score >= 2 or (score >= 1 and n >= 1 and second < score):
            conf = 'high' if score >= 2 and second <= score / 2 else 'medium' if second < score else 'ambiguous'
        else:
            conf = 'low'
        matches.append({
            'client': f'0x{fn:06X}', 'lib': best[0], 'file': best[1], 'function': best[2],
            'tags': ''.join(sorted(ea[best]['tags'])), 'score': round(score, 2), 'second': round(second, 2),
            'confidence': conf, 'strings': evidence[fn][best][:6],
            'alternatives': [f'{k[0]}/{k[1]}:{k[2]}' for k, v in ranked[1:4] if v >= score / 2],
        })
    # One EA function claimed by several client functions: the strings came with inlined code
    # (or a helper the client duplicated). The best-scoring claimant stays primary.
    claims = collections.defaultdict(list)
    for m in matches:
        claims[(m['lib'], m['file'], m['function'])].append(m)
    rank = {'high': 0, 'medium': 1, 'ambiguous': 2, 'low': 3}
    for group in claims.values():
        # ties: the claimant whose own match is clearer, then the lowest address (reruns agree)
        group.sort(key=lambda m: (-m['score'], rank[m['confidence']], m['client']))
        for m in group:
            m['claimants'] = len(group)
        for m in group[1:]:
            if m['score'] <= group[0]['score']:
                m['confidence'] = 'shared'
    matches.sort(key=lambda m: (m['lib'], m['file'], m['function']))
    json.dump(matches, open(os.path.join(args.out, 'funcmatch.json'), 'w'), indent=1)

    by_lib = collections.Counter((m['lib'], m['confidence']) for m in matches)
    print('\nmatches by library and confidence:')
    for lib in sorted({m['lib'] for m in matches}):
        print(f"  {lib:14s} " + '  '.join(f"{c}={by_lib[(lib, c)]}" for c in CONFIDENCES))
    print(f"total {len(matches)}")
    write_markdown(matches, os.path.join(args.out, 'funcmatch.md'))


CONFIDENCES = ('high', 'medium', 'ambiguous', 'low', 'shared')


def write_markdown(matches, path):
    with open(path, 'w', encoding='utf-8') as f:
        f.write('# String-anchored function matches (generated by tools/re/funcmatch.py)\n\n')
        f.write('Tags: which EA copies contain the function (R Renegade, G Generals, Z Zero Hour). '
                '`shared`: another client function claims the same EA function with a higher score '
                '(inlined copy or duplicated helper).\n\n')
        f.write('| Client | Library | File | Function | Tags | Score | Confidence | Evidence |\n')
        f.write('|---|---|---|---|---|---|---|---|\n')
        for m in matches:
            ev = '; '.join(s.replace('|', '/').replace('\n', ' ').replace('\r', '')[:40] for s in m['strings'][:2])
            f.write(f"| {m['client']} | {m['lib']} | {m['file']} | `{m['function']}` | {m['tags']} | "
                    f"{m['score']} | {m['confidence']} | {ev} |\n")


if __name__ == '__main__':
    main()
