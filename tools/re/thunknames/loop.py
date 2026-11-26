"""The settle loop after any new confirmed name: propagate -> tier1c variants (with every confirmed class rename)
-> t1c_keep (the kinds kept) -> gapguess, until a pass adds no hit. Each pass's hits are appended to
docs/data/thunk-name-hits.tsv (step `<tag>c<n>`); with --write the final state replaces docs/data/thunk-names.tsv
and export.py refreshes class-renames.tsv.

Usage: python tools/re/thunknames/loop.py [--state F] --tag T [--write]
Scratch: .work/names/loop/<tag>p<n>/, <tag>c<n>/, tier1c_<tag><n>/.
"""
import argparse
import csv
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402

PY = sys.executable
HERE = paths.HERE


def run(*args, log):
    p = subprocess.run([PY, *args], capture_output=True, text=True, cwd=paths.REPO)
    with open(log, 'a') as f:
        f.write(p.stdout + p.stderr)
    if p.returncode:
        raise SystemExit(f'failed: {" ".join(args)} (see {log})')
    return p.stdout


def append_hits(hits_tsv, step):
    rows = list(csv.DictReader(open(hits_tsv), delimiter='\t'))
    if not rows:
        return 0
    new = not os.path.exists(paths.HITS)
    cols = ['step'] + list(rows[0].keys())
    with open(paths.HITS, 'a', newline='') as f:
        w = csv.DictWriter(f, cols, delimiter='\t', extrasaction='ignore')
        if new:
            w.writeheader()
        for r in rows:
            w.writerow(dict(r, step=step))
    return len(rows)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--tag', required=True)
    ap.add_argument('--write', action='store_true')
    a = ap.parse_args()
    log = paths.work('loop', f'{a.tag}.log')
    state = a.state
    n = 0
    while True:
        n += 1
        pdir = paths.workdir('loop', f'{a.tag}p{n}')
        cdir = paths.workdir('loop', f'{a.tag}c{n}')
        tdir = paths.workdir('loop', f'tier1c_{a.tag}{n}')
        print(run(os.path.join(HERE, 'propagate.py'), '--state', state, '--write', os.path.join(pdir, 'state.tsv'),
                  log=log).strip().splitlines()[-1])
        run(os.path.join(HERE, 'tier1c.py'), '--state', os.path.join(pdir, 'state.tsv'), '--out', tdir,
            '--confirmed', paths.HITS, log=log)
        run(os.path.join(HERE, 't1c_keep.py'), os.path.join(tdir, 'guesses.tsv'), os.path.join(tdir, 'guesses_f.tsv'),
            log=log)
        out = run(os.path.join(HERE, 'gapguess.py'), 'test', os.path.join(tdir, 'guesses_f.tsv'), '--state',
                  os.path.join(pdir, 'state.tsv'), '--out', cdir, log=log)
        k = append_hits(os.path.join(cdir, 'hits.tsv'), f'{a.tag}c{n}')
        print(f'pass {n}: hits {k}; {out.strip().splitlines()[-1]}')
        state = os.path.join(cdir, 'state.tsv')
        if k == 0:
            break
    print(f'final {state}')
    if a.write:
        run(os.path.join(HERE, 'export.py'), '--state', state, log=log)
        print(f'wrote {paths.STATE} and {paths.RENAMES}')


if __name__ == '__main__':
    main()
