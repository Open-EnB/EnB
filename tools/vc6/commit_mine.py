"""Commit only your own hunks when the git index is shared with other builders (D62).

    python tools/vc6/commit_mine.py MSGFILE SPEC.json [--base HEAD] [--file NAME ...] [--dry]

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

docs/data/imports.json is special (P-H14-1): an address key such as 0x42D050 matches identical lines in other
files' blocks, so a key that is a block name there (`"control_keys_manager": {` ... `}`), or a `--file NAME`
(repeatable, applies to imports.json), selects the hunks inside that block's line span instead (the new-file
span in the working copy or the old-file span in BASE; a hunk straddling a block's edge is an error). Other keys
of that path keep the keyword behaviour. Hunks with "in_tree_total" are dropped from imports.json (counters
that every verify rewrites; H16) unless a key names in_tree_total. `--dry` prints the chosen hunks and stops.

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
IMPORTS = 'docs/data/imports.json'


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


def block_spans(lines, name):
    """1-based (first, last) lines of every `"name": {` ... matching close in the pretty-printed JSON."""
    pat = re.compile(r'^(\s*)' + re.escape(json.dumps(name)) + r': \{\s*$')
    spans = []
    for i, ln in enumerate(lines):
        m = pat.match(ln.rstrip('\r\n'))
        if not m:
            continue
        close = re.compile(r'^' + m[1] + r'\},?\s*$')
        for j in range(i + 1, len(lines)):
            if close.match(lines[j].rstrip('\r\n')):
                spans.append((i + 1, j + 1))
                break
    return spans


def where(lo, hi, spans):
    """0 outside, 1 inside, 2 straddling. hi < lo is an empty range: lines go after line `lo`."""
    if hi < lo:
        return 1 if any(a <= lo < b for a, b in spans) else 0
    if any(a <= lo and hi <= b for a, b in spans):
        return 1
    return 2 if any(lo <= b and a <= hi for a, b in spans) else 0


def block_hunks(hunks, names, old, new, p):
    """Hunks of p that lie inside one of the named blocks (old = BASE's lines, new = working copy's)."""
    keep = []
    for name in names:
        so, sn = block_spans(old, name), block_spans(new, name)
        if not so and not sn:
            sys.exit(f'{p}: no block named {name!r} in the base or the working copy')
        if len(so) > 1 or len(sn) > 1:
            print(f'warning: {name!r} is a block name {max(len(so), len(sn))} times in {p}')
        for h in hunks:
            m = HUNK.match(h[0].rstrip('\r\n'))
            a, b = int(m[1]), int(m[2]) if m[2] is not None else 1
            c, d = int(m[3]), int(m[4]) if m[4] is not None else 1
            ro = where(a, a + b - 1, so)
            rn = where(c, c + d - 1, sn)
            if 2 in (ro, rn):
                sys.exit(f'{p}: hunk {h[0].strip()} straddles the edge of block {name!r}; take it by another key')
            if 1 in (ro, rn) and not any(h is k for k in keep):
                keep.append(h)
    return keep


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('msgfile')
    ap.add_argument('spec')
    ap.add_argument('--base', default='HEAD')
    ap.add_argument('--file', action='append', default=[], help='imports.json file block to take (repeatable)')
    ap.add_argument('--dry', action='store_true', help='show the chosen hunks and stop before committing')
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
    hunk_specs = dict(spec.get('hunks', {}))
    if a.file:
        hunk_specs.setdefault(IMPORTS, [])
    for p, keys in hunk_specs.items():
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
        keep = []
        if p == IMPORTS:
            old = git('show', f'{a.base}:{p}').decode('utf-8', 'surrogateescape').splitlines(keepends=True)
            with open(os.path.join(ROOT, p), newline='', encoding='utf-8', errors='surrogateescape') as f:
                new = f.read().splitlines(keepends=True)
            names = list(a.file) + [k for k in keys if block_spans(old, k) or block_spans(new, k)]
            keys = [k for k in keys if k not in names]
            if not any('in_tree_total' in k for k in keys):
                n = len(hunks)
                hunks = [h for h in hunks if not any('"in_tree_total"' in x for x in h[1:] if x[:1] in '+-')]
                if n != len(hunks):
                    print(f'{p}: dropped {n - len(hunks)} in_tree_total hunk(s)')
            keep = block_hunks(hunks, names, old, new, p) if names else []
        keep += [h for h in hunks if not any(h is k for k in keep) and any(
            k in ''.join(x for x in h[1:] if x[:1] in '+-') for k in keys)]
        keep = [h for h in hunks if any(h is k for k in keep)]
        print(f'{p}: {len(keep)} of {len(hunks)} hunks are yours')
        if a.dry:
            for h in keep:
                print('   ', h[0].strip(), '|', ' '.join(x.strip() for x in h[1:])[:120])
        if keep:
            patch = ''.join(head) + ''.join(''.join(h) for h in renumber(keep))
            git('apply', '--cached', '--unidiff-zero', '-', inp=patch.encode('utf-8', 'surrogateescape'))
    stat = git('diff', '--cached', '--stat', a.base).decode(errors='replace')
    if not stat.strip():
        sys.exit('nothing to commit: no whole path changed and no hunk matched')
    print(stat)
    if a.dry:
        os.remove(INDEX)
        return
    git('commit', '-q', '-F', a.msgfile)
    print(git('log', '-1', '--format=%h %s').decode(errors='replace').strip())
    # the shared index still holds the old blobs for these paths: point them at the new HEAD
    git('reset', '-q', 'HEAD', '--', *paths, shared=True)
    os.remove(INDEX)


if __name__ == '__main__':
    main()
