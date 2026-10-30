"""D112(4): a tool that writes build/ or the tree runs only from inside its own tree.

The tool's root is the git tree holding the script (the nearest ancestor with a `.git` entry:
worktrees under .work/ count as their own trees). The current directory must lie in that same
tree (its own nearest `.git` ancestor is the root), so a `cd` into a worktree that failed and
left the shell in the main tree no longer lets the worktree's script (or the main tree's, run
from inside a worktree) write the other tree's build/. `--root DIR` replaces the cwd test: the
caller names the tree it means, and the tool refuses unless that is its own root.

    import rootguard
    ap.add_argument('--root', help=rootguard.HELP)
    ...
    rootguard.check(__file__, a.root)
"""
import os

HELP = ("the tree this run is meant for (default: the current directory's tree); "
        "the tool refuses unless it is the tree holding the script")


def tree_of(path):
    """The nearest ancestor of path (itself included) holding a .git entry, or None."""
    p = os.path.abspath(path)
    while True:
        if os.path.exists(os.path.join(p, '.git')):
            return p
        up = os.path.dirname(p)
        if up == p:
            return None
        p = up


def same(a, b):
    return a is not None and b is not None and \
        os.path.normcase(os.path.normpath(a)) == os.path.normcase(os.path.normpath(b))


def check(script, root=None):
    """Refuse (SystemExit) unless the current directory (or `root`) is the script's tree; returns it."""
    mine = tree_of(os.path.dirname(os.path.abspath(script)))
    name = os.path.basename(script)
    if root is not None:
        if not same(os.path.abspath(root), mine):
            raise SystemExit(f'{name}: refusing: --root {os.path.abspath(root)} is not this script\'s tree {mine} '
                             f'(run {os.path.join(os.path.abspath(root), os.path.relpath(os.path.abspath(script), mine))} instead)')
        return mine
    cwd = os.getcwd()
    if not same(tree_of(cwd), mine):
        raise SystemExit(f'{name}: refusing: the current directory {cwd} is not inside the tree '
                         f'{mine} that holds this script (it is in {tree_of(cwd) or "no git tree"}); '
                         f'cd into {mine}, run the other tree\'s copy, or pass --root {mine}')
    return mine
