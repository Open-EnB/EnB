"""Compares two EA copies of a library file by file, ignoring comments, whitespace and banners.

    python libdiff.py <dirA> <dirB>

Prints, per file present in both, the number of code lines that differ (0 = same code), and
the files only one side has. Used to check how much a library drifted between Renegade,
Generals and Zero Hour.
"""
import difflib
import os
import sys

from easource import strip_comments, SOURCE_EXT


def code_lines(path):
    text = open(path, encoding='latin1').read()
    # strip_comments also blanks preprocessor lines; keep them, they are code here.
    lines = []
    for raw, clean in zip(text.splitlines(), strip_comments(text).splitlines()):
        s = raw if raw.lstrip().startswith('#') else clean
        s = ''.join(s.split())
        if s:
            lines.append(s)
    return lines


def main():
    a, b = sys.argv[1], sys.argv[2]
    fa = {f.lower(): f for f in os.listdir(a) if f.lower().endswith(SOURCE_EXT)}
    fb = {f.lower(): f for f in os.listdir(b) if f.lower().endswith(SOURCE_EXT)}
    same, changed = [], []
    for name in sorted(set(fa) & set(fb)):
        la, lb = code_lines(os.path.join(a, fa[name])), code_lines(os.path.join(b, fb[name]))
        if la == lb:
            same.append(name)
            continue
        n = sum(1 for d in difflib.ndiff(la, lb) if d[:2] in ('- ', '+ '))
        changed.append((n, name))
    print(f'same code: {len(same)}; changed: {len(changed)}; only in A: {len(set(fa) - set(fb))}; '
          f'only in B: {len(set(fb) - set(fa))}')
    for n, name in sorted(changed, reverse=True):
        print(f'  {n:5d}  {name}')
    if set(fa) - set(fb):
        print('only in A: ' + ' '.join(sorted(set(fa) - set(fb))))
    if set(fb) - set(fa):
        print('only in B: ' + ' '.join(sorted(set(fb) - set(fa))))


if __name__ == '__main__':
    main()
