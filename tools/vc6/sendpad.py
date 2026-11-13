"""Sweep and set SEND_SYMBOL_PAD, the symbol pad in app/client/aux_roots_send.h (workflow.md 3.4).

Every declaration sclient.cpp sees moves the window of pad values that keep its padded constructors and
0x74D990 exact (sclient 0x758210, 0x702240, 0x74D990 ...). Any edit to a header sclient.cpp includes can
shift it. This compiles sclient.cpp alone through verify's own path (importer.compile_and_match, not
appfile match: the two pad through different windows) with each pad value, prints the window, and leaves
the pad at the middle of it.

  sendpad.py [--range LO..HI] [--pool K] [--lock-wait S] [--agent NAME] [--holder-pid P] [--watch-from REV] [--dry]
             [--hold] [--first-window]
  sendpad.py --includes [PATH ...]    the headers sclient.cpp includes (transitively, within app/);
                                      with paths: those of them among PATH (exit 1 when none)
  --hold          take the importer lock once, at the first value, and keep it to the end of the sweep (given back on
                  every exit, exceptions and Ctrl-C included): a sweep queues once on the first-come lock (D238)
                  instead of once per value. wt.py bring-over's retune passes it, with --first-window.
  --first-window  try the current pad first, then outward (cur, cur+1, cur-1, cur+2, ...) and stop once a run of at
                  least 2 full values has both neighbours known (failed, or outside the range; 2, not 3, since P-L18-4: a
                  2-wide window such as 46..47 swept all 64 values); the pad goes to its (lower) middle. That is the full sweep's pick whenever the run nearest the current pad is the longest; with
                  no such run every value is tried and the pick is the full sweep's. Per-value output covers only
                  the values tried.

The window is the run of consecutive values at which every address of WATCH that HEAD's ledger records for
sclient.cpp is exact (the longest run when there are several; HEAD's, not the working tree's: the verify that
lost the pairs has already taken them out of the working ledger, which then watched nothing it lost); the pad is written to its middle (lower middle); --dry restores the file instead.
When HEAD is a `wt.py rebase` record commit ("Record the verified pairs of wt/NAME after rebasing it onto BRANCH
SHA: ..."), HEAD's ledger has recorded the loss already: the watch set comes from that onto commit's ledger (D228).
--watch-from REV names the ledger outright (wt.py bring-over passes its target when the landing carries such a commit).
The importer lock is taken per variant, not for the whole sweep. K VMs (default 1) are leased from pool.py and
released at the end (also on failure). Log: .work/log/sendpad.log (one line per value). Prints one summary line.
Every value's exact address set (all of sclient.cpp's, not only WATCH) goes to .work/log/sendpad-sets.json (--sets PATH),
with HEAD's commit: tools/re/matchaudit.py's pad-held class (D240) reads it, so a sweep need not be run twice.
Exit 1: no value keeps all of them (the header is left as it was).
It works on the tree holding the script (wt.py bring-over runs the scratch worktree's copy).
"""
import argparse
import os
import re
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)

H = os.path.join(REPO, 'app', 'client', 'aux_roots_send.h')
AUXROOTS = os.path.join(REPO, 'tools', 're', 'auxroots.py')      # the generator holds SEND_SYMBOL_PAD too
CPP = os.path.join(REPO, 'app', 'client', 'sclient.cpp')
APP = os.path.join(REPO, 'app')
WATCH = ['0x74D990', '0x758210', '0x702240', '0x742B60', '0x743380', '0x744B60', '0x744560']
LINE = re.compile(r'^enum AuxRootsSendSymbolPadType \{[^}]*\};', re.M)
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
# wt.py's record_subject for a rebase: group 1 the onto branch, group 2 its commit (abbreviated)
REBASE_RECORD = re.compile(r'^Record the verified pairs of wt/\S+ after rebasing it onto (\S+) ([0-9a-f]{7,40}):')


def with_pad(text, n):
    """text (aux_roots_send.h, latin-1) with the pad enum holding n enumerators."""
    assert LINE.search(text), 'aux_roots_send.h has no AuxRootsSendSymbolPadType line'
    enums = ', '.join(f'AUX_ROOTS_SEND_PAD_{i:02d}' for i in range(n))
    body = 'enum AuxRootsSendSymbolPadType { ' + enums + ' };' if n else 'enum AuxRootsSendSymbolPadType { };'
    return LINE.sub(lambda m: body, text, count=1)


def set_generator(n):
    """SEND_SYMBOL_PAD = n in tools/re/auxroots.py, so a regeneration of aux_roots_send.h keeps the pad."""
    src = open(AUXROOTS, 'rb').read().decode('utf-8')
    new, k = re.subn(r'^SEND_SYMBOL_PAD = \d+', f'SEND_SYMBOL_PAD = {n}', src, flags=re.M)
    if k == 1 and new != src:
        open(AUXROOTS, 'wb').write(new.encode('utf-8'))


def current_pad(text):
    m = LINE.search(text)
    return len(re.findall(r'AUX_ROOTS_SEND_PAD_\d+', m.group(0))) if m else None


def include_set():
    """The files under app/ that sclient.cpp includes, transitively, as repo-relative '/' paths. "x\\y.h" is looked
    up beside the includer, then by its tail name anywhere under app/ (an ambiguous tail name counts all of them,
    which only widens the set)."""
    idx = {}
    for d, _, names in os.walk(APP):
        for n in names:
            idx.setdefault(n.lower(), []).append(os.path.join(d, n))
    seen, todo = set(), [CPP]
    while todo:
        f = todo.pop()
        try:
            text = open(f, encoding='latin-1').read()
        except OSError:
            continue
        for inc in INCLUDE.findall(text):
            inc = inc.replace('\\', '/')
            near = os.path.normpath(os.path.join(os.path.dirname(f), inc))
            cands = [near] if os.path.isfile(near) else idx.get(os.path.basename(inc).lower(), [])
            for c in cands:
                c = os.path.normpath(c)
                if c not in seen and c.startswith(APP):
                    seen.add(c)
                    todo.append(c)
    return {os.path.relpath(p, REPO).replace(os.sep, '/') for p in seen}


def windows(ok):
    runs, start, prev = [], None, None
    for n in ok + [None]:
        if start is not None and (n is None or n != prev + 1):
            runs.append((start, prev))
            start = None
        if n is not None and start is None:
            start = n
        prev = n
    return runs


def watch_source(repo=None):
    """The revision whose ledger holds the watch set: HEAD, or the onto commit when HEAD is a `wt.py rebase` record
    commit (its ledger has recorded the loss already, D228)."""
    import subprocess
    r = subprocess.run(['git', 'log', '-1', '--format=%s', 'HEAD'], cwd=repo or REPO, capture_output=True, text=True)
    m = REBASE_RECORD.match(r.stdout.strip()) if r.returncode == 0 else None
    return m.group(2) if m else 'HEAD'


def watch_list(src='HEAD'):
    """WATCH addresses that HEAD's ledger records for sclient.cpp (an address not matched yet constrains nothing;
    the working ledger is the wrong source: a verify that just lost them has rewritten it);
    all of WATCH when that ledger has no sclient.cpp entry."""
    try:
        import ledger
        held = {str(x) for x in ledger.load(src, repo=REPO)['app']['files']['sclient']['in_tree']}
    except Exception:
        return list(WATCH)
    return [w for w in WATCH if w in held] or list(WATCH)


HOLD = False          # --hold: measure() keeps the lock it takes; run_sweep() gives it back


def measure(importer, n, orig, lock_wait):
    """Compile sclient.cpp alone with pad n under the importer lock (taken and given back here; with --hold kept)."""
    importer.hold_lock(lock_wait)
    try:
        with open(H, 'wb') as f:
            f.write(with_pad(orig, n).encode('latin-1'))
        found, text = importer.compile_and_match([CPP], f'sendpad-{n}')
        got = set().union(*found.values()) if found else set()
        errs = [l for l in text.splitlines() if ' error ' in l][:3]
        return got, errs
    finally:
        with open(H, 'wb') as f:
            f.write(orig.encode('latin-1'))
        if not HOLD:
            importer.release_lock()


def save_sets(path, file, sets, errors, repo=None):
    """Write the per-value exact address sets of one sweep (sendpad.py / auxpad.py) as JSON:
    {"file", "head", "values": {"N": [addr, ...]}, "errors": {"N": [line, ...]}}."""
    import json
    import subprocess
    r = subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=repo or REPO, capture_output=True, text=True)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    data = {'file': file, 'head': r.stdout.strip(), 'values': {str(n): sorted(s) for n, s in sorted(sets.items())},
            'errors': {str(n): e for n, e in sorted(errors.items()) if e}}
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(data, f, indent=1)
        f.write('\n')


def sweep(a, log, watch):
    """(pad before, chosen pad or None, [(lo, hi) windows])."""
    import importer
    if a.agent:
        importer.set_agent(a.agent)
    orig = open(H, 'rb').read().decode('latin-1')
    ok, sets, errors = [], {}, {}
    try:
        with importer.pool_lease(a.pool, a.holder_pid):
            for n in range(a.range[0], a.range[1] + 1):
                got, errs = measure(importer, n, orig, a.lock_wait)
                sets[n], errors[n] = got, errs
                save_sets(a.sets, 'app/client/sclient.cpp', sets, errors)
                hit = [w for w in watch if w in got]
                print(f'{n:3d}  {len(hit)}/{len(watch)}  ' + ' '.join(w + ('+' if w in got else '-') for w in WATCH)
                      + f'  total {len(got)}' + (f'  ERRORS {errs}' if errs else ''), file=log, flush=True)
                if len(hit) == len(watch):
                    ok.append(n)
    finally:
        with open(H, 'wb') as f:
            f.write(orig.encode('latin-1'))
    runs = windows(ok)
    if not runs:
        return current_pad(orig), None, runs
    best = max(runs, key=lambda r: (r[1] - r[0], -r[0]))
    return current_pad(orig), (best[0] + best[1]) // 2, runs


def outward(lo, hi, cur):
    """lo..hi from cur outward: cur, cur+1, cur-1, cur+2, ... (cur clamped into the range)."""
    cur = min(max(lo if cur is None else cur, lo), hi)
    order = [cur]
    for d in range(1, hi - lo + 1):
        order += [n for n in (cur + d, cur - d) if lo <= n <= hi]
    return order


def settled(ok, tried, lo, hi):
    """The first run of 2 or more full values (P-L18-4) whose two neighbours are known (tried and not full, or outside lo..hi),
    or None."""
    for x, y in windows(sorted(ok)):
        if y - x >= 1 and (x - 1 < lo or x - 1 in tried) and (y + 1 > hi or y + 1 in tried):
            return x, y
    return None


def sweep_first_window(a, log, watch):
    """--first-window: sweep()'s result from the values tried outward from the current pad, stopping at settled()."""
    import importer
    if a.agent:
        importer.set_agent(a.agent)
    orig = open(H, 'rb').read().decode('latin-1')
    ok, tried = [], set()
    try:
        with importer.pool_lease(a.pool, a.holder_pid):
            for n in outward(a.range[0], a.range[1], current_pad(orig)):
                got, errs = measure(importer, n, orig, a.lock_wait)
                tried.add(n)
                hit = [w for w in watch if w in got]
                print(f'{n:3d}  {len(hit)}/{len(watch)}  ' + ' '.join(w + ('+' if w in got else '-') for w in WATCH)
                      + f'  total {len(got)}' + (f'  ERRORS {errs}' if errs else ''), file=log, flush=True)
                if len(hit) == len(watch):
                    ok.append(n)
                done = settled(ok, tried, a.range[0], a.range[1])
                if done:
                    print(f'first window {done[0]}..{done[1]}: stopped after {len(tried)} values', file=log, flush=True)
                    break
    finally:
        with open(H, 'wb') as f:
            f.write(orig.encode('latin-1'))
    runs = windows(sorted(ok))
    if not runs:
        return current_pad(orig), None, runs
    best = settled(ok, tried, a.range[0], a.range[1]) or max(runs, key=lambda r: (r[1] - r[0], -r[0]))
    return current_pad(orig), (best[0] + best[1]) // 2, runs


def run_sweep(a, log, watch):
    """sweep() or sweep_first_window(); with --hold the lock the first value takes is given back here, on every exit."""
    global HOLD
    HOLD = bool(getattr(a, 'hold', False))
    try:
        return (sweep_first_window if getattr(a, 'first_window', False) else sweep)(a, log, watch)
    finally:
        if HOLD:
            HOLD = False
            import importer
            importer.release_lock()


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--range', default='0..63', metavar='LO..HI', help='pad values to try (default %(default)s)')
    ap.add_argument('--pool', type=int, default=1, metavar='K', help='VMs to lease from pool.py (default %(default)s)')
    ap.add_argument('--lock-wait', type=int, default=None, metavar='S',
                    help="seconds to wait for another importer run, per variant (default the importer's)")
    ap.add_argument('--agent', metavar='NAME', help='importer agent: lock owner, scratch under .work/NAME/tmp')
    ap.add_argument('--holder-pid', type=int, help="the lease's holder pid (pool.py's default otherwise)")
    ap.add_argument('--hold', action='store_true', help='take the importer lock once and hold it for the whole sweep')
    ap.add_argument('--first-window', action='store_true',
                    help='current pad first, then outward; stop at the first settled window of 2 or more values')
    ap.add_argument('--watch-from', metavar='REV',
                    help="the ledger the watch set comes from (default HEAD's, or the onto commit's when HEAD is a "
                         "wt.py rebase record commit)")
    ap.add_argument('--dry', action='store_true', help='sweep and print, leave the header as it was')
    ap.add_argument('--sets', default=os.path.join(REPO, '.work', 'log', 'sendpad-sets.json'), metavar='PATH',
                    help="every value's exact address set, as JSON (default %(default)s)")
    ap.add_argument('--includes', nargs='*', metavar='PATH', help='print the include set, or which PATHs are in it')
    a = ap.parse_args(argv)
    if a.includes is not None:
        inc = include_set()
        if a.includes:
            hit = sorted(p.replace('\\', '/') for p in a.includes if p.replace('\\', '/') in inc)
            print('\n'.join(hit))
            return 0 if hit else 1
        print('\n'.join(sorted(inc)))
        return 0
    m = re.fullmatch(r'(\d+)\.\.(\d+)', a.range)
    if not m or int(m.group(1)) > int(m.group(2)):
        ap.error('--range LO..HI')
    a.range = (int(m.group(1)), int(m.group(2)))
    t0 = time.time()
    os.makedirs(os.path.join(REPO, '.work', 'log'), exist_ok=True)
    with open(os.path.join(REPO, '.work', 'log', 'sendpad.log'), 'w', encoding='utf-8') as log:
        src = a.watch_from or watch_source()
        watch = watch_list(src)
        print(f"watch set from {src}'s ledger: {' '.join(watch)}", file=log, flush=True)
        was, pick, runs = run_sweep(a, log, watch)
        wtxt = ', '.join(f'{x}..{y}' if x != y else str(x) for x, y in runs) or 'none'
        if pick is None:
            line = f'sendpad: no value in {a.range[0]}..{a.range[1]} keeps all {len(watch)} addresses ({' '.join(watch)}); pad left at {was}'
        else:
            if not a.dry:
                text = open(H, 'rb').read().decode('latin-1')
                with open(H, 'wb') as f:
                    f.write(with_pad(text, pick).encode('latin-1'))
                set_generator(pick)
            line = (f'sendpad: window {wtxt}; was {was}; ' + (f'dry run, pad stays {was}' if a.dry else f'pad set to {pick}')
                    + f' ({time.time() - t0:.0f} s)')
        print(line, file=log)
    print(line + '  (log .work/log/sendpad.log)')
    return 0 if pick is not None else 1


if __name__ == '__main__':
    raise SystemExit(main())
