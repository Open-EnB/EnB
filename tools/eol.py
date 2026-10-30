#!/usr/bin/env python3
"""Line endings: the tree is LF everywhere (D147, .gitattributes). Finds and fixes working files
that are not.

    python tools/eol.py check [PATH...]     list tracked text files with a CR on disk (exit 1 if any)
    python tools/eol.py fix [PATH...]       rewrite them with LF (CRLF -> LF; a lone CR is reported, not changed)

Git normalizes what it stores, so a CRLF working file never reaches a commit; but until it is fixed,
byte comparisons (importer.py replay) and edit scripts' anchors see the CRs. Windows tools that write
CRLF: Python's text mode without newline='\\n', some editors. Files marked -text in .gitattributes
(docs/data/asserts.txt) are skipped. Standard library only.
"""
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def tracked_text(paths):
    """Tracked files git treats as text (not -text, not binary), as repository-relative paths."""
    out = subprocess.run(['git', 'ls-files', '--eol', '-z', '--'] + list(paths), cwd=ROOT,
                         capture_output=True, check=True).stdout.decode('utf-8', 'replace')
    files = []
    for rec in out.split('\0'):
        if '\t' not in rec:
            continue
        meta, path = rec.split('\t', 1)
        attr = meta.split()[2] if len(meta.split()) > 2 else ''
        if '-text' in meta.split()[0] or attr.startswith('attr/-text') or 'i/none' in meta:
            continue
        files.append(path)
    return files


def scan(paths):
    bad = []
    for p in tracked_text(paths):
        full = os.path.join(ROOT, p)
        if not os.path.isfile(full):
            continue
        b = open(full, 'rb').read()
        if b'\r' in b:
            crlf = b.count(b'\r\n')
            bad.append((p, crlf, b.count(b'\r') - crlf))
    return bad


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ('check', 'fix'):
        print(__doc__.split('\n\n')[1])
        return 2
    bad = scan(sys.argv[2:])
    if sys.argv[1] == 'check':
        for p, crlf, lone in bad:
            print(f'{p}: {crlf} CRLF' + (f', {lone} lone CR' if lone else ''))
        return 1 if bad else 0
    for p, crlf, lone in bad:
        full = os.path.join(ROOT, p)
        b = open(full, 'rb').read()
        open(full, 'wb').write(b.replace(b'\r\n', b'\n'))
        print(f'{p}: {crlf} CRLF -> LF' + (f'; {lone} lone CR left (fix by hand)' if lone else ''))
    return 0


if __name__ == '__main__':
    sys.exit(main())
