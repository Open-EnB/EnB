"""Compiles variants of one tree file and reports which client functions each gains or loses.

    python tryvariants.py LIB FILE VARIANTS.py

VARIANTS.py (keep it in .work/) defines V = {name: [(old, new), ...]}: edits on the current tree
file, each `old` occurring exactly once. The file is compiled alone (one VM, a few minutes for a
big file) first as it is, then once per variant, and each variant's byte-exact client addresses
are compared with the unmodified file's. The tree file is restored afterwards, whatever happens.

Use it to find the form of one function quickly (e.g. which SNAPSHOT_SAY changes VC6's inlining,
workflow.md §3); then record the winner with addpatch.py and run a full verify, since a header
or inline change can affect other files.
"""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import importer  # noqa: E402


def main():
    if len(sys.argv) != 4:
        raise SystemExit(__doc__)
    lib, name, spec = sys.argv[1:4]
    variants = runpy.run_path(spec)['V']
    real = [x for x in os.listdir(importer.dest(lib)) if x.lower() == name.lower()]
    if not real:
        raise SystemExit(f'{lib}/{name}: no such file in the tree')
    path = os.path.join(importer.dest(lib), real[0])
    orig = open(path, encoding='latin1', newline='').read()
    nl = '\r\n' if '\r\n' in orig else '\n'

    def run(tag):
        found, text = importer.compile_and_match([path], 'try-' + tag)
        errors = [line for line in text.splitlines() if ' error ' in line]
        return set().union(*found.values()), errors

    try:
        base, errors = run('base')
        print(f'as it is: {len(base)} client functions, {len(errors)} errors')
        for vname, edits in variants.items():
            s = orig
            for old, new in edits:
                o, n = old.replace('\n', nl), new.replace('\n', nl)
                if s.count(o) != 1:
                    raise SystemExit(f'{vname}: anchor occurs {s.count(o)} times: {old!r}')
                s = s.replace(o, n)
            open(path, 'w', encoding='latin1', newline='').write(s)
            got, errors = run(vname)
            print(f'{vname}: {len(got)}  gained {sorted(got - base)}  lost {sorted(base - got)}'
                  + (f'  errors {errors[:3]}' if errors else ''))
    finally:
        open(path, 'w', encoding='latin1', newline='').write(orig)


if __name__ == '__main__':
    main()
