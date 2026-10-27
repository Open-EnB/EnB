"""Commit only your own hunks when the git index is shared with other builders (D62).

    python tools/vc6/commit_mine.py MSGFILE SPEC.json [--base HEAD]

A bare `git commit` takes whatever the other builders have staged, and `git commit -- <paths>` takes
the whole working-tree copy of a shared file (importer.py, an app header, docs/data/*) including their
uncommitted hunks. This script builds the commit in a private index instead: it reads BASE (default
HEAD) into `.work/commit_mine.index`, adds the `whole` paths as they are in the working tree, applies
only the matching hunks of each `hunks` path, commits, and then points the shared index's entries for
those paths at the new HEAD so `git status` doesn't show a staged revert (other builders' unstaged
edits of the same files stay in the working tree, unstaged).

SPEC.json:
    {"whole": ["ww3d2/textureloader.cpp", "docs/library-matching.md"],
     "hunks": {"tools/vc6/importer.py": ["textureloader", "WWSHADE_API_PATCHES"]}}
A hunk of `git diff -U0 BASE -- path` is yours when any of its +/- lines contains one of the keys.
MSGFILE holds the full commit message (subject, body, the Co-Authored-By line).
Written by builder C on 2026-10-25 (.work/c/commit_mine.py), promoted by the coordinator.
"""
import argparse
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
INDEX = os.path.join(ROOT, '.work', 'commit_mine.index')


HUNK = re.compile(r'^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@(.*)$')


def renumber(hunks):
    """Rewrite the kept hunks' headers so their new-file positions follow the kept hunks only.

    `git apply` places a hunk at its new-file line (`+c`) and searches outward for the preimage; a
    -U0 insertion hunk has no preimage, so it lands exactly at `c`. When hunks before it were
    skipped (other builders'), `c` still counts their lines and the insertion lands inside another
    entry (139c184 broke imports.json this way, 2026-10-26). So `c` is recomputed from the old-file
    position and the running delta of the kept hunks alone.
    """
    out, delta = [], 0
    for h in hunks:
        header = h[0].rstrip('\r\n')
        m = HUNK.match(header)
        if not m:
            sys.exit(f'unparsed hunk header: {header!r}')
        a = int(m[1])
        b = int(m[2]) if m[2] is not None else 1
        d = int(m[4]) if m[4] is not None else 1
        if b == 0:
            c = a + delta + 1      # insertion after old line a
        elif d == 0:
            c = a + delta - 1      # deletion of old lines a..a+b-1
        else:
            c = a + delta
        eol = h[0][len(header):]
        out.append([f'@@ -{a},{b} +{c},{d} @@{m[5]}{eol}'] + h[1:])
        delta += d - b
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('msgfile')
    ap.add_argument('spec')
    ap.add_argument('--base', default='HEAD')
    a = ap.parse_args()
    spec = json.load(open(a.spec))
    os.makedirs(os.path.dirname(INDEX), exist_ok=True)
    if os.path.exists(INDEX):
        os.remove(INDEX)
    env = dict(os.environ, GIT_INDEX_FILE=INDEX)

    def git(*args, inp=None, shared=False):
        r = subprocess.run(['git', *args], cwd=ROOT, env=None if shared else env, input=inp, capture_output=True)
        if r.returncode:
            sys.exit(f'git {" ".join(args)}: {r.stderr.decode(errors="replace")}')
        return r.stdout

    git('read-tree', a.base)
    paths = list(spec.get('whole', []))
    for p in paths:
        git('add', '--', p)
    for p, keys in spec.get('hunks', {}).items():
        paths.append(p)
        d = git('diff', '-U0', a.base, '--', p).decode('utf-8', 'surrogateescape')
        head, hunks, cur = [], [], None
        for line in d.splitlines(keepends=True):
            if line.startswith('@@'):
                cur = [line]
                hunks.append(cur)
            elif cur is None:
                head.append(line)
            else:
                cur.append(line)
        keep = [h for h in hunks if any(k in ''.join(x for x in h[1:] if x[:1] in '+-') for k in keys)]
        print(f'{p}: {len(keep)} of {len(hunks)} hunks are yours')
        if keep:
            patch = ''.join(head) + ''.join(''.join(h) for h in renumber(keep))
            git('apply', '--cached', '--unidiff-zero', '-', inp=patch.encode('utf-8', 'surrogateescape'))
    stat = git('diff', '--cached', '--stat', a.base).decode(errors='replace')
    if not stat.strip():
        sys.exit('nothing to commit: no whole path changed and no hunk matched')
    print(stat)
    git('commit', '-q', '-F', a.msgfile)
    print(git('log', '-1', '--format=%h %s').decode(errors='replace').strip())
    # the shared index still holds the old blobs for these paths: point them at the new HEAD
    git('reset', '-q', 'HEAD', '--', *paths, shared=True)
    os.remove(INDEX)


if __name__ == '__main__':
    main()
