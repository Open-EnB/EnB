"""Compiles source-form variants that may edit several tree files and reports each one's gains/losses.

    python tryv2.py VARIANTS.py

VARIANTS.py (keep it in .work/; write it with a file tool, not a heredoc) defines
    V = {name: [(relpath, old, new), ...]}   edits on the current tree files, each `old` once in its file
    COMPILE = [relpath, ...]                 the sources to compile (default: the .cpp files V touches)
    WATCH = ['0x82A470', ...]                optional: addresses to report as exact or not per variant
relpaths are repository-relative with '/' (`app/common/connect.h`, `app/shared/auxdata/lookup.cpp`):
any library, app subdirectories included, headers too. Strings use '\\n'; each is converted to the
file's own line ending (CRLF or LF), so never write '\\r\\n' in a variant.

The files are compiled together (`importer.compile_and_match`, one VM) as they are, then once per
variant, and each variant's byte-exact client addresses are compared with the unmodified run's:
`name: N  gained [...]  lost [...]`. Every touched tree file is restored afterwards, whatever
happens. It holds the importer lock (it edits tree files): don't run it from a script that holds
the lock, and not while another builder verifies the same files.

Use it before `tryvariants.py` (one file, one library, no subdirectories) whenever a form spans
a header and its .cpp or two app files; then record the winner (addpatch.py or the app file
itself) and run a full verify, since a header or inline change can affect other files. Promoted
from builder step 8's .work/step8b/tryv2.py (2026-10-26).
"""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import importer  # noqa: E402


def norm_addr(a):
    return int(a, 16) if isinstance(a, str) else int(a)


def main():
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    spec = runpy.run_path(sys.argv[1])
    V = spec['V']
    touched = sorted({e[0] for v in V.values() for e in v})
    compile_ = spec.get('COMPILE') or [f for f in touched if f.lower().endswith(('.cpp', '.c'))]
    if not compile_:
        raise SystemExit('nothing to compile: give COMPILE')
    watch = [norm_addr(a) for a in spec.get('WATCH', [])]
    for f in set(touched) | set(compile_):
        if not os.path.isfile(os.path.join(REPO, f)):
            raise SystemExit(f'{f}: no such tree file')
    importer.hold_lock()
    paths = [os.path.join(REPO, p) for p in compile_]
    origs = {f: open(os.path.join(REPO, f), encoding='latin1', newline='').read() for f in touched}

    def run(tag):
        found, text = importer.compile_and_match(paths, 'tryv2-' + tag)
        errors = [line for line in text.splitlines() if ' error ' in line]
        return set().union(*found.values()) if found else set(), errors

    def watched(got):
        ints = {norm_addr(x) for x in got}
        return '  ' + ' '.join(f"0x{a:06X}:{'exact' if a in ints else 'no'}" for a in watch) if watch else ''

    def hexes(s):
        return '[' + ', '.join(f'0x{a:06X}' if isinstance(a, int) else str(a) for a in sorted(s)) + ']'
    try:
        base, errors = run('base')
        print(f'as it is: {len(base)} client functions, {len(errors)} errors {errors[:3]}{watched(base)}', flush=True)
        for vname, edits in V.items():
            cur = dict(origs)
            for f, old, new in edits:
                s = cur[f]
                nl = '\r\n' if '\r\n' in s else '\n'
                o, n = old.replace('\r\n', '\n').replace('\n', nl), new.replace('\r\n', '\n').replace('\n', nl)
                if s.count(o) != 1:
                    raise SystemExit(f'{vname}: anchor occurs {s.count(o)} times in {f}: {old!r}')
                cur[f] = s.replace(o, n)
            for f in touched:
                open(os.path.join(REPO, f), 'w', encoding='latin1', newline='').write(cur[f])
            got, errors = run(vname)
            print(f'{vname}: {len(got)}  gained {hexes(got - base)}  lost {hexes(base - got)}{watched(got)}'
                  + (f'  errors {errors[:3]}' if errors else ''), flush=True)
    finally:
        for f in touched:
            open(os.path.join(REPO, f), 'w', encoding='latin1', newline='').write(origs[f])


if __name__ == '__main__':
    main()
