"""One class-name round with the local model, as two commands (HANDOFF.md brief B):

  python tools/re/thunknames/round_local.py propose --tag local2 [--samples 6]
      1. the open classes: qualifiers with open slots and no anchor            -> proposals/<tag>/classes.txt
      2. their contexts (tier2_ctx3.py, the names already tried included)        -> .work/names/tier2/ctx3/
      3. the local model's candidates (tier2_local.py, thinking off)             -> proposals/<tag>/L00.tsv
      4. the score against the hash with the salted null (tier2_eval.py --perm)  -> .work/names/round/<tag>/
         and the ACCEPT lines printed. Nothing in docs/data changes.
  python tools/re/thunknames/round_local.py apply --tag local2
      5. gapguess.py test on the round's guesses (the cascade), then loop.py --write (propagate, variants,
         export): docs/data's three files change. Look at the ACCEPT lines of step 4 first.

The proposals are tracked (they are the record of what was tried: tier2_eval counts them in every later chance,
tier2_ctx3 lists them as tried). A tag is used once: propose refuses an existing proposals/<tag>/L00.tsv.
Every step's output goes to .work/names/round/<tag>/<step>.log; only the summary lines are printed.
"""
import argparse
import collections
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402

PY = sys.executable
HERE = paths.HERE


def step(name, args, wdir, show):
    """Runs one tool; its whole output to <wdir>/<name>.log; prints the lines for which show(line) is true."""
    log = os.path.join(wdir, name + '.log')
    p = subprocess.run([PY, os.path.join(HERE, args[0]), *args[1:]], capture_output=True, text=True, cwd=paths.REPO)
    open(log, 'w').write(p.stdout + p.stderr)
    if p.returncode:
        raise SystemExit(f'{name} failed: see {log}\n' + (p.stdout + p.stderr)[-1500:])
    for line in p.stdout.splitlines():
        if show(line):
            print(line)
    return p.stdout


def open_classes():
    from gapguess import State, class_of
    st = State()
    on = collections.Counter(class_of(st.names[i]) for i in st.onset)
    op = collections.Counter(class_of(n) for i, n in enumerate(st.names)
                             if n and i not in st.onset and st.window(i))
    return sorted(c for c in op if c and not on[c])


def apply_guesses(tag, guesses, wdir):
    """gapguess's cascade on `guesses`, the hits appended as step `tag`, then loop.py --write. -> hits applied."""
    gdir = os.path.join(wdir, 'gaps')
    step('gapguess', ['gapguess.py', 'test', guesses, '--out', gdir], wdir, lambda s: s.startswith(('hits', 'slots')))
    loop = __import__('loop')
    n = loop.append_hits(os.path.join(gdir, 'hits.tsv'), tag)
    print(f'{n} hits appended to {paths.HITS} as step {tag}')
    step('loop', ['loop.py', '--state', os.path.join(gdir, 'state.tsv'), '--tag', tag, '--write'], wdir,
         lambda s: s.startswith(('pass', 'final', 'wrote')))
    step('state', ['gapguess.py', 'state'], wdir, lambda s: True)
    open(os.path.join(wdir, 'applied'), 'w').write('applied\n')
    return n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=('propose', 'apply'))
    ap.add_argument('--tag', required=True)
    ap.add_argument('--samples', type=int, default=6)
    ap.add_argument('--temperature', type=float, default=1.0)
    ap.add_argument('--parallel', type=int, default=16)
    ap.add_argument('--perm', type=int, default=1000)
    ap.add_argument('--url', default='http://127.0.0.1:8080/v1/chat/completions')
    ap.add_argument('--model', default='qwen3.8-27b')
    a = ap.parse_args()
    pdir = os.path.join(paths.PROPOSALS, a.tag)
    wdir = paths.workdir('round', a.tag)
    props = os.path.join(pdir, 'L00.tsv')
    guesses = os.path.join(wdir, 'guesses.tsv')
    if a.cmd == 'propose':
        if os.path.exists(props):
            raise SystemExit(f'{props} exists: a tag is used once (score it again with tier2_eval.py, or pick a new tag)')
        os.makedirs(pdir, exist_ok=True)
        classes = open_classes()
        clist = os.path.join(pdir, 'classes.txt')
        open(clist, 'w', newline='\n').write('\n'.join(classes) + '\n')
        print(f'1. open classes: {len(classes)} -> {clist}')
        step('ctx', ['tier2_ctx3.py', '--classes', clist, '--batches', '1'], wdir, lambda s: False)
        print('2. contexts written')
        step('local', ['tier2_local.py', '--classes', clist, '--out', props, '--mode', 'off', '--samples',
                       str(a.samples), '--temperature', str(a.temperature), '--parallel', str(a.parallel),
                       '--url', a.url, '--model', a.model], wdir, lambda s: 'candidates' in s)
        print('3. candidates written')
        step('eval', ['tier2_eval.py', '--const', '--no-cross', '--perm', str(a.perm), '--out', wdir],
             wdir, lambda s: 'ACCEPT' in s or s.startswith(('accept threshold', 'accepted classes', '  1e-4')))
        print(f'4. scored: guesses in {guesses}; the whole table in {os.path.join(wdir, "eval.log")}')
        print(f'next: look at the ACCEPT lines, then: python tools/re/thunknames/round_local.py apply --tag {a.tag}')
    else:
        if not os.path.exists(guesses):
            raise SystemExit(f'{guesses} missing: run propose first')
        if os.path.exists(os.path.join(wdir, 'applied')):
            raise SystemExit(f'{a.tag} is already applied')
        apply_guesses(a.tag, guesses, wdir)


if __name__ == '__main__':
    main()
