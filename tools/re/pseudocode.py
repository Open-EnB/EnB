"""Ghidra pseudocode of client.exe's functions, decompiled ahead of need into .work/pseudo/.

    python pseudocode.py                     every function not yet byte-exact, not yet decompiled
    python pseudocode.py ADDR...             these functions (decompiled again)
    python pseudocode.py --show ADDR...      print the saved pseudocode

Runs Ghidra's headless analyzer itself instead of going through REA: REA can only decompile
functions Ghidra's auto-analysis found, and that misses methods reached only through a vtable
(0x9961E0, the distortion draw). The script tools/re/ghidra/DecompileTargets.java first creates a
function at every function start clientimage.py knows and names them from docs/data/symbols.json
and the gaps reports (client.py's names), then decompiles the targets.

Targets are the function starts minus the incremental thunks and the functions already proven
byte for byte (symbols.json `exact`): about 25,000. The Ghidra project is kept in
%LOCALAPPDATA%/EnB/ghidra (ENB_GHIDRA_PROJECT; Ghidra refuses .work's dotted path), so only the first run pays the analysis (about 5 minutes); every run skips targets that already
have a file unless they are named on the command line. .work/pseudo/index.tsv holds one line per
target: address, ok/failed/no_function, length or error.
"""
import argparse
import bisect
import glob
import hashlib
import json
import os
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import CLIENT_SHA256, DEFAULT_CLIENT, ClientImage  # noqa: E402
from client import DATA, Names  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
WORK = os.path.join(ROOT, '.work', 'ghidra')
OUT = os.path.join(ROOT, '.work', 'pseudo')
GHIDRA = os.environ.get('GHIDRA_INSTALL_DIR', r'C:\tools\ghidra_12.1.4_PUBLIC')
# Ghidra rejects project paths with a '.'-prefixed element, so the project can't live in .work.
PROJECT_DIR = os.environ.get('ENB_GHIDRA_PROJECT', os.path.join(os.environ.get('LOCALAPPDATA', ROOT), 'EnB', 'ghidra'))
PROJECT = 'client_re'
PROGRAM = 'client.exe'


def parse_addr(s):
    return int(s, 16)


def key(va):
    return f'{va:08X}'


def show(addrs):
    for va in addrs:
        path = os.path.join(OUT, key(va) + '.c')
        if os.path.exists(path):
            sys.stdout.write(open(path, encoding='utf-8').read())
        else:
            print(f'// 0x{va:X}: not decompiled yet (python pseudocode.py 0x{va:X})')


def merge_index(new_path):
    """Folds the run's index-new.tsv into index.tsv, newest line per address winning."""
    index = os.path.join(OUT, 'index.tsv')
    rows = {}
    for path in (index, new_path):
        if os.path.exists(path):
            for line in open(path, encoding='utf-8'):
                if line.strip():
                    rows[line.split('\t', 1)[0]] = line.rstrip('\n')
    with open(index, 'w', encoding='utf-8') as f:
        f.writelines(rows[k] + '\n' for k in sorted(rows))
    os.remove(new_path)
    return rows


def false_start_leftovers(img, starts, exact):
    """What runs before clientimage's verified starts left at false ones (an E8 byte inside
    another instruction taken for a call target): those addresses that still have pseudocode
    (DecompileTargets.java created a function there), and the functions containing them, whose
    pseudocode those cut short. The only use of the raw starts (verified=False)."""
    false = sorted(set(img.function_starts(verified=False)[0]) - set(starts))
    index = os.path.join(OUT, 'index.tsv')
    indexed = {int(line.split('\t', 1)[0], 16) for line in open(index, encoding='utf-8')
               if line.strip()} if os.path.exists(index) else set()
    purge = [va for va in false if va in indexed or os.path.exists(os.path.join(OUT, key(va) + '.c'))]
    redo = {starts[bisect.bisect_right(starts, va) - 1] for va in purge} - set(exact)
    if purge:
        print(f'{len(purge)} functions at false starts to remove; {len(redo)} containing functions to decompile again')
    return purge, redo


def remove_outputs(addrs):
    """Deletes these addresses' pseudocode and index lines."""
    gone = {key(va) for va in addrs}
    for k in gone:
        path = os.path.join(OUT, k + '.c')
        if os.path.exists(path):
            os.remove(path)
    index = os.path.join(OUT, 'index.tsv')
    rows = list(open(index, encoding='utf-8'))
    with open(index, 'w', encoding='utf-8') as f:
        f.writelines(line for line in rows if line.split('\t', 1)[0] not in gone)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('addrs', nargs='*', type=parse_addr)
    ap.add_argument('--show', action='store_true', help='print saved pseudocode instead of decompiling')
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    ap.add_argument('--timeout', type=int, default=60, help='decompiler seconds per function')
    ap.add_argument('--limit', type=int, help='decompile at most N targets (trial runs)')
    args = ap.parse_args()
    if args.show:
        return show(args.addrs)

    img = ClientImage(args.client)
    names = Names(img)
    starts, thunks = img.function_starts()
    exact = {int(s['address'], 16) for s in json.load(open(os.path.join(DATA, 'symbols.json')))
             if s['source'] == 'exact'}
    os.makedirs(WORK, exist_ok=True)
    os.makedirs(OUT, exist_ok=True)
    os.makedirs(PROJECT_DIR, exist_ok=True)
    drop = []
    if args.addrs:
        targets = args.addrs
    else:
        drop, redo = false_start_leftovers(img, starts, exact)
        done = {int(os.path.basename(p)[:-2], 16) for p in glob.glob(os.path.join(OUT, '*.c'))}
        targets = sorted({s for s in starts if s not in exact and s not in done} | redo)
    if args.limit:
        targets = targets[:args.limit]
    if not targets:
        return print('nothing to decompile')

    known = sorted(set(starts) | set(thunks) | set(names.proven) | set(names.guessed))
    with open(os.path.join(WORK, 'names.tsv'), 'w', encoding='utf-8') as f:
        for va in known:
            n = names.of(va)
            if n:
                f.write(f'{va:X}\t{n}\n')
    with open(os.path.join(WORK, 'targets.tsv'), 'w', encoding='utf-8') as f:
        f.writelines(f'{key(va)}\t{names.of(va) or ""}\n' for va in targets)
    with open(os.path.join(WORK, 'drop.tsv'), 'w', encoding='utf-8') as f:
        f.writelines(f'{key(va)}\n' for va in drop)

    # A copy under .work: the install path's '&' breaks analyzeHeadless.bat's argument passing.
    exe = os.path.join(WORK, PROGRAM)
    if not os.path.exists(exe) or hashlib.sha256(open(exe, 'rb').read()).hexdigest() != CLIENT_SHA256:
        shutil.copyfile(args.client, exe)
    cmd = [os.path.join(GHIDRA, 'support', 'analyzeHeadless.bat'), PROJECT_DIR, PROJECT]
    if os.path.exists(os.path.join(PROJECT_DIR, PROJECT + '.gpr')):
        cmd += ['-process', PROGRAM, '-noanalysis']
    else:
        cmd += ['-import', exe]
    cmd += ['-scriptPath', os.path.join(HERE, 'ghidra'),
            '-postScript', 'DecompileTargets.java',
            os.path.join(WORK, 'names.tsv'), os.path.join(WORK, 'targets.tsv'), OUT, str(args.timeout),
            os.path.join(WORK, 'drop.tsv')]
    log = os.path.join(WORK, 'headless.log')
    print(f'decompiling {len(targets)} functions; Ghidra log: {log}', flush=True)
    with open(log, 'w', encoding='utf-8', errors='replace') as f:
        rc = subprocess.call(cmd, stdout=f, stderr=subprocess.STDOUT)
    new = os.path.join(OUT, 'index-new.tsv')
    if not os.path.exists(new):
        sys.exit(f'Ghidra exited with {rc} and wrote no results; see {log}')
    run = {line.split('\t', 1)[0]: line.split('\t')[1] for line in open(new, encoding='utf-8') if line.strip()}
    merge_index(new)
    if drop:
        remove_outputs(drop)
    counts = {s: sum(1 for v in run.values() if v == s) for s in ('ok', 'failed', 'no_function')}
    print(f'ok {counts["ok"]}, failed {counts["failed"]}, no function {counts["no_function"]} '
          f'(of {len(targets)}); pseudocode in {OUT}')


if __name__ == '__main__':
    main()
