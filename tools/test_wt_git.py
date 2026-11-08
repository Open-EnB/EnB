"""Tests for tools/vc6/wt.py on temporary git repositories: `rebase` (generated conflicts take the target's side,
any other conflict stops), and (item 5) unlanded commits in `remove` and `list --unlanded`.

    python -m unittest discover -s tools -p "test_*.py"

Standard library and git only; no VMs, no verify (the regeneration step is stubbed).
"""
import argparse
import io
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))

import wt  # noqa: E402


def run(cwd, *args):
    r = subprocess.run(['git', *args], cwd=cwd, capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError(f'git {args}: {r.stderr}')
    return r.stdout.strip()


def write(root, rel, text):
    full = os.path.join(root, *rel.split('/'))
    os.makedirs(os.path.dirname(full), exist_ok=True)
    with open(full, 'w', newline='\n') as f:
        f.write(text)


def rd(path):
    with open(path) as f:
        return f.read()


def commit(cwd, msg, **files):
    for rel, text in files.items():
        write(cwd, rel.replace('__', '/').replace('_dot_', '.'), text)
    run(cwd, 'add', '-A')
    run(cwd, 'commit', '-q', '-m', msg)
    return run(cwd, 'rev-parse', 'HEAD')


class GitRepo(unittest.TestCase):
    """A repo with master and run/x at one base, the main tree on run/x, WTDIR inside it."""

    def setUp(self):
        self.td = tempfile.mkdtemp(prefix='wt_test_')
        self.root = os.path.join(self.td, 'repo')
        os.makedirs(self.root)
        run(self.root, 'init', '-q', '-b', 'master')
        for k, v in (('user.name', 't'), ('user.email', 't@t'), ('core.autocrlf', 'false'),
                     ('commit.gpgsign', 'false')):
            run(self.root, 'config', k, v)
        write(self.root, '.gitignore', '.work/\n')
        self.base = commit(self.root, 'base', **{'docs/imports_dot_md': 'A\n', 'src_dot_txt': 's1\ns2\ns3\n',
                                                 'docs/data/attempts_dot_csv': 'a,b\n1,2\n'})
        run(self.root, 'checkout', '-q', '-b', 'run/x')
        self.wtdir = os.path.join(self.root, '.work', 'wt')
        os.makedirs(self.wtdir)
        for k, v in (('ROOT', self.root), ('WTDIR', self.wtdir)):
            old = getattr(wt, k)
            setattr(wt, k, v)
            self.addCleanup(setattr, wt, k, old)
        self.addCleanup(shutil.rmtree, self.td, ignore_errors=True)

    def worktree(self, name):
        path = os.path.join(self.wtdir, name)
        run(self.root, 'worktree', 'add', '-q', path, '-b', 'wt/' + name, self.base)
        with open(os.path.join(self.wtdir, name + '.base'), 'w') as f:
            f.write(self.base + '\n')
        return path


def rebase_args(name, **kw):
    d = dict(name=name, onto=None, cont=False, lib=[], pool=1, lock_wait=1, trailer=None)
    d.update(kw)
    return argparse.Namespace(**d)


class Rebase(GitRepo):
    def setUp(self):
        super().setUp()
        self.regen = []
        old = wt.regenerate_rebased
        wt.regenerate_rebased = lambda a, path, st: self.regen.append((a.name, st['onto'], sorted(st['took'])))
        self.addCleanup(setattr, wt, 'regenerate_rebased', old)

    def quiet(self, fn, *args):
        with redirect_stdout(io.StringIO()) as out:
            fn(*args)
        return out.getvalue()

    def test_generated_conflict_takes_target_side(self):
        p = self.worktree('T')
        commit(p, 'builder', **{'docs/imports_dot_md': 'B\n', 'src_dot_txt': 's1\ns2\ns3\nmine\n',
                                'docs/data/attempts_dot_csv': 'a,b\n1,2\n3,4\n'})
        onto = commit(self.root, 'run', **{'docs/imports_dot_md': 'C\n', 'docs/data/attempts_dot_csv': 'a,b\n1,2\n5,6\n'})
        self.quiet(wt.cmd_rebase, rebase_args('T'))           # onto: the run branch holding the recorded base
        self.assertFalse(wt.rebase_in_progress(p))
        self.assertEqual(run(p, 'rev-parse', 'HEAD~1'), onto)
        self.assertEqual(rd(os.path.join(p, 'docs', 'imports.md')), 'C\n')
        self.assertEqual(rd(os.path.join(p, 'src.txt')), 's1\ns2\ns3\nmine\n')
        rows = rd(os.path.join(p, 'docs', 'data', 'attempts.csv')).splitlines()
        self.assertEqual(sorted(rows), sorted(['a,b', '1,2', '3,4', '5,6']))
        self.assertEqual(self.regen, [('T', 'run/x', ['docs/imports.md'])])
        self.assertEqual(rd(os.path.join(self.wtdir, 'T.base')).strip(), onto)
        self.assertFalse(os.path.exists(os.path.join(self.wtdir, 'T.rebase')))

    def test_other_conflict_stops_then_continue(self):
        p = self.worktree('T')
        commit(p, 'builder 1', **{'src_dot_txt': 's1\nmine\ns3\n', 'docs/imports_dot_md': 'B\n'})
        commit(p, 'builder 2', **{'docs/imports_dot_md': 'B2\n'})
        commit(self.root, 'run', **{'src_dot_txt': 's1\ntheirs\ns3\n', 'docs/imports_dot_md': 'C\n'})
        with self.assertRaises(SystemExit) as c:
            self.quiet(wt.cmd_rebase, rebase_args('T', onto='run/x'))
        msg = str(c.exception)
        self.assertIn('conflict in src.txt', msg)
        self.assertIn('rebase T --continue', msg)
        self.assertIn('rebase --abort', msg)
        self.assertTrue(wt.rebase_in_progress(p))
        self.assertEqual(self.regen, [])
        # the builder resolves src.txt; --continue resolves the generated rest and regenerates
        write(p, 'src.txt', 's1\nboth\ns3\n')
        run(p, 'add', 'src.txt')
        self.quiet(wt.cmd_rebase, rebase_args('T', cont=True))
        self.assertFalse(wt.rebase_in_progress(p))
        self.assertEqual(rd(os.path.join(p, 'src.txt')), 's1\nboth\ns3\n')
        self.assertEqual(rd(os.path.join(p, 'docs', 'imports.md')), 'C\n')
        self.assertEqual(self.regen, [('T', 'run/x', ['docs/imports.md'])])

    def test_dirty_source_refused(self):
        p = self.worktree('T')
        write(p, 'src.txt', 'dirty\n')
        with self.assertRaises(SystemExit) as c:
            self.quiet(wt.cmd_rebase, rebase_args('T', onto='run/x'))
        self.assertIn('src.txt', str(c.exception))


class SinglePick(GitRepo):
    """bring-over's single form picks with the batch's resolution and reports the generated files it took."""

    def pick(self, commits):
        bo = os.path.join(self.wtdir, 'T.bo')
        run(self.root, 'worktree', 'add', '-q', '--detach', bo, 'run/x')
        took = set()
        with open(bo + '.log', 'w') as log:
            why = wt.cherry_pick(bo, commits, log, took)
        return bo, why, took

    def test_generated_conflict_taken(self):
        p = self.worktree('T')
        c = commit(p, 'builder', **{'docs/imports_dot_md': 'B\n', 'src_dot_txt': 's1\ns2\ns3\nmine\n'})
        commit(self.root, 'run', **{'docs/imports_dot_md': 'C\n'})
        bo, why, took = self.pick([c])
        self.assertIsNone(why)
        self.assertEqual(took, {'docs/imports.md'})
        self.assertEqual(rd(os.path.join(bo, 'docs', 'imports.md')), 'C\n')
        self.assertEqual(rd(os.path.join(bo, 'src.txt')), 's1\ns2\ns3\nmine\n')

    def test_other_conflict_refused(self):
        p = self.worktree('T')
        c = commit(p, 'builder', **{'src_dot_txt': 's1\nmine\ns3\n'})
        commit(self.root, 'run', **{'src_dot_txt': 's1\ntheirs\ns3\n'})
        bo, why, took = self.pick([c])
        self.assertIn('src.txt', why)
        self.assertFalse(os.path.exists(os.path.join(bo, '.git', 'CHERRY_PICK_HEAD')))


def remove_args(name, **kw):
    d = dict(name=name, delete_branch=True, force=False)
    d.update(kw)
    return argparse.Namespace(**d)


class Unlanded(GitRepo):
    def quiet(self, fn, *args):
        with redirect_stdout(io.StringIO()) as out:
            fn(*args)
        return out.getvalue()

    def branch_exists(self, name):
        return subprocess.run(['git', 'rev-parse', '--verify', '-q', 'refs/heads/wt/' + name], cwd=self.root,
                              capture_output=True).returncode == 0

    def test_remove_refuses_unlanded(self):
        p = self.worktree('T')
        c = commit(p, 'builder work', **{'new_dot_txt': 'x\n'})
        with self.assertRaises(SystemExit) as e:
            self.quiet(wt.cmd_remove, remove_args('T'))
        self.assertIn(c[:9], str(e.exception))
        self.assertIn('builder work', str(e.exception))
        self.assertTrue(os.path.isdir(p))
        self.assertTrue(self.branch_exists('T'))
        out = self.quiet(wt.cmd_remove, remove_args('T', force=True))
        self.assertIn('unlanded', out)
        self.assertFalse(self.branch_exists('T'))

    def test_remove_allows_patch_equivalent(self):
        p = self.worktree('T')
        commit(p, 'builder work', **{'new_dot_txt': 'x\n'})
        commit(self.root, 'the same change, landed under another subject', **{'new_dot_txt': 'x\n'})
        self.assertEqual(wt.own_commits('refs/heads/wt/T', wt.landed_refs()), [])
        self.quiet(wt.cmd_remove, remove_args('T'))
        self.assertFalse(os.path.isdir(p))
        self.assertFalse(self.branch_exists('T'))

    def test_pick_with_generated_side_taken_counts_as_landed(self):
        p = self.worktree('T')
        c = commit(p, 'builder work', **{'new_dot_txt': 'x\n', 'docs/imports_dot_md': 'B\n'})
        commit(self.root, 'run', **{'docs/imports_dot_md': 'C\n'})
        r = subprocess.run(['git', 'cherry-pick', c], cwd=self.root, capture_output=True)
        self.assertNotEqual(r.returncode, 0)                  # imports.md conflicts: take the run's side
        run(self.root, 'checkout', '--ours', '--', 'docs/imports.md')
        run(self.root, 'add', 'docs/imports.md')
        subprocess.run(['git', '-c', 'core.editor=true', 'cherry-pick', '--continue'], cwd=self.root,
                       capture_output=True, check=True)
        self.assertEqual(wt.own_commits('refs/heads/wt/T', wt.landed_refs()), [])

    def test_list_unlanded(self):
        a = self.worktree('A')
        commit(a, 'A work', **{'a_dot_txt': 'a\n'})
        b = self.worktree('B')
        commit(b, 'B work', **{'b_dot_txt': 'b\n'})
        run(self.root, 'cherry-pick', 'wt/B')                 # B landed on run/x
        self.worktree('C')                                     # no commits of its own
        g = self.worktree('G')
        commit(g, 'generated only', **{'docs/imports_dot_md': 'G\n'})  # regenerated anyway: not unlanded work
        rows = wt.unlanded_branches()
        self.assertEqual([n for n, _ in rows], ['A'])
        out = self.quiet(wt.cmd_list, argparse.Namespace(unlanded=True))
        self.assertTrue(out.startswith('unlanded    1 wt/* branch(es)'))
        self.assertIn('A (1)', out)
        self.assertIn('A work', out)


if __name__ == '__main__':
    unittest.main()
