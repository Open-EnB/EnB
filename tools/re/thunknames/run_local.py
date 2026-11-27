"""The unattended local run (HANDOFF.md brief B): the coordinator is this script, the local model only proposes.

  python tools/re/thunknames/run_local.py --prefix n1 [--class-rounds 6] [--method-rounds 4]

  0. checks   the state loads, the self-tests pass, the local server answers
  1. settle   loop.py on the current state (skipped when step `<prefix>rv` is already in the hits file)
  2. classes  rounds `<prefix>c1`, c2, ...: round_local.py propose, then apply when a class is accepted;
              a round proposed earlier and not applied (--pending TAG, e.g. local1) is applied first;
              stops after two rounds in a row with no accepted class, or --class-rounds
  3. methods  rounds `<prefix>m1`, m2, ...: tier2_methods.py jobs / propose / score, then apply;
              stops after a round with no accepted override group and fewer than --min-singles single hits,
              or --method-rounds
Every step's lines go to .work/names/run/<prefix>/report.md (append; the run's record for the log entry) and to
stdout. A failed step is written there and the run goes on with the next phase. Nothing is committed or pushed.
Re-running with the same prefix continues: finished rounds (their `applied` mark or their proposals file) are
skipped by number.
"""
import argparse
import csv
import datetime
import io
import os
import subprocess
import sys
import urllib.request
from contextlib import redirect_stdout

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402
import round_local  # noqa: E402

PY = sys.executable
HERE = paths.HERE
REPORT = None


def say(text):
    print(text, flush=True)
    with open(REPORT, 'a') as f:
        f.write(text.rstrip('\n') + '\n')


def tool(args, log):
    """Runs a tool; whole output to `log`; -> (ok, stdout)."""
    p = subprocess.run([PY, os.path.join(HERE, args[0]), *args[1:]], capture_output=True, text=True, cwd=paths.REPO)
    open(log, 'w').write(p.stdout + p.stderr)
    return p.returncode == 0, p.stdout + (p.stderr[-800:] if p.returncode else '')


def state_line():
    ok, out = tool(['gapguess.py', 'state'], os.path.join(os.path.dirname(REPORT), 'state.log'))
    return out.strip().splitlines()[-1] if ok and out.strip() else 'state failed: ' + out[-300:]


def steps_done():
    if not os.path.exists(paths.HITS):
        return set()
    return {r['step'] for r in csv.DictReader(open(paths.HITS), delimiter='\t')}


def apply(tag, guesses, wdir):
    buf = io.StringIO()
    try:
        with redirect_stdout(buf):
            n = round_local.apply_guesses(tag, guesses, wdir)
    except SystemExit as e:
        say(f'  apply {tag} FAILED: {e}')
        return -1
    for line in buf.getvalue().splitlines():
        say('  ' + line)
    return n


def main():
    global REPORT
    ap = argparse.ArgumentParser()
    ap.add_argument('--prefix', required=True)
    ap.add_argument('--pending', default='', help='a class round already proposed and scored, to apply first')
    ap.add_argument('--class-rounds', type=int, default=6)
    ap.add_argument('--method-rounds', type=int, default=4)
    ap.add_argument('--samples', type=int, default=6, help='requests per class')
    ap.add_argument('--method-samples', type=int, default=2, help='requests per method')
    ap.add_argument('--min-singles', type=int, default=10)
    ap.add_argument('--url', default='http://127.0.0.1:8080/v1/chat/completions')
    ap.add_argument('--model', default='qwen3.8-27b')
    a = ap.parse_args()
    rdir = paths.workdir('run', a.prefix)
    REPORT = os.path.join(rdir, 'report.md')
    say(f'\n## run {a.prefix} started {datetime.datetime.now():%Y-%m-%d %H:%M}')
    # 0. checks
    say('state: ' + state_line())
    for t in ('mangle.py', 'permnull.py'):
        ok, out = tool([t], os.path.join(rdir, t + '.log'))
        say(f'{t}: {out.strip().splitlines()[-1] if out.strip() else "no output"}')
        if not ok:
            say('a self-test failed: stopping')
            return 1
    try:
        urllib.request.urlopen(a.url.rsplit('/chat', 1)[0] + '/models', timeout=10).read()
        server = True
        say('local server: answers')
    except Exception as e:
        server = False
        say(f'local server: NO ANSWER ({e}); only the steps without a model run')
    # 1. settle
    if a.prefix + 'rv' not in steps_done() and not os.path.exists(os.path.join(rdir, 'settled')):
        ok, out = tool(['loop.py', '--tag', a.prefix + 'rv', '--write'], os.path.join(rdir, 'settle.log'))
        for line in out.strip().splitlines()[-4:]:
            say('  settle: ' + line)
        if ok:
            open(os.path.join(rdir, 'settled'), 'w').write('settled\n')
        say('state: ' + state_line())
    # 2. classes
    srv = ['--url', a.url, '--model', a.model]
    if a.pending:
        wdir = paths.workdir('round', a.pending)
        g = os.path.join(wdir, 'guesses.tsv')
        if os.path.exists(os.path.join(wdir, 'applied')):
            say(f'class round {a.pending}: already applied')
        elif os.path.exists(os.path.join(paths.PROPOSALS, a.pending, 'L00.tsv')):
            # the state moved since it was scored: score again, then apply
            ok, out = tool(['tier2_eval.py', '--const', '--no-cross', '--perm', '1000', '--out', wdir],
                           os.path.join(wdir, 'eval.log'))
            acc = [x for x in out.splitlines() if 'ACCEPT' in x]
            say(f'class round {a.pending} (pending): {len(acc)} classes accepted' + ('' if ok else ' (eval FAILED)'))
            for x in acc:
                say('  ' + ' '.join(x.split()))
            if ok and acc:
                apply(a.pending, g, wdir)
                say('state: ' + state_line())
    empty = 0
    for r in range(1, a.class_rounds + 1):
        if not server or empty >= 2:
            break
        tag = f'{a.prefix}c{r}'
        wdir = paths.workdir('round', tag)
        if os.path.exists(os.path.join(wdir, 'applied')) or os.path.exists(os.path.join(wdir, 'empty')):
            continue
        if not os.path.exists(os.path.join(paths.PROPOSALS, tag, 'L00.tsv')):
            ok, out = tool(['round_local.py', 'propose', '--tag', tag, '--samples', str(a.samples), *srv],
                           os.path.join(wdir, 'propose.log'))
        else:
            ok, out = tool(['tier2_eval.py', '--const', '--no-cross', '--perm', '1000', '--out', wdir],
                           os.path.join(wdir, 'eval.log'))
        acc = [x for x in out.splitlines() if 'ACCEPT' in x]
        say(f'class round {tag}: {len(acc)} classes accepted' + ('' if ok else ' (FAILED: ' + out[-300:] + ')'))
        for x in acc:
            say('  ' + ' '.join(x.split()))
        if not ok:
            break
        if acc:
            empty = 0
            apply(tag, os.path.join(wdir, 'guesses.tsv'), wdir)
            say('state: ' + state_line())
        else:
            empty += 1
            open(os.path.join(wdir, 'empty'), 'w').write('no class accepted\n')
    # 3. methods
    for r in range(1, a.method_rounds + 1):
        if not server:
            break
        tag = f'{a.prefix}m{r}'
        wdir = paths.workdir('methods', tag)
        if os.path.exists(os.path.join(wdir, 'applied')) or os.path.exists(os.path.join(wdir, 'empty')):
            continue
        ok = True
        out = ''
        if not os.path.exists(os.path.join(paths.PROPOSALS, tag, 'M00.tsv')):
            for cmd in (['jobs'], ['propose', '--samples', str(a.method_samples), *srv]):
                ok, out = tool(['tier2_methods.py', cmd[0], '--tag', tag, *cmd[1:]],
                               os.path.join(wdir, cmd[0] + '.log'))
                say(f'method round {tag} {cmd[0]}: ' + (out.strip().splitlines()[-1] if out.strip() else ''))
                if not ok:
                    break
        elif not os.path.exists(os.path.join(wdir, 'jobs.json')):
            ok, out = tool(['tier2_methods.py', 'jobs', '--tag', tag], os.path.join(wdir, 'jobs.log'))
        if not ok:
            say(f'method round {tag} FAILED: {out[-300:]}')
            break
        ok, out = tool(['tier2_methods.py', 'score', '--tag', tag], os.path.join(wdir, 'score.log'))
        for line in out.strip().splitlines()[:4]:
            say(f'  {line}')
        if not ok:
            say(f'method round {tag} score FAILED: {out[-300:]}')
            break
        g = os.path.join(wdir, 'guesses.tsv')
        rows = list(csv.DictReader(open(g), delimiter='\t'))
        groups = sum(x['tier'] == 't2-lvmethod' for x in rows)
        singles = sum(x['tier'] == 't2-lmethod' for x in rows)
        if rows:
            apply(tag, g, wdir)
            say('state: ' + state_line())
        if not groups and singles < a.min_singles:
            if not rows:
                open(os.path.join(wdir, 'empty'), 'w').write('nothing accepted\n')
            say(f'method round {tag}: {groups} group slots, {singles} single hits: stopping the method rounds')
            break
    say(f'run {a.prefix} finished {datetime.datetime.now():%Y-%m-%d %H:%M}; state: ' + state_line())
    say('not done by this script: the progress.md log entry, any commit (the user\'s call)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
