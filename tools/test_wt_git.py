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


class Land(GitRepo):
    """Item 3: a landing tolerates commits on the target that touch none of its paths and no generated file."""

    def setUp(self):
        super().setUp()
        locks = os.path.join(self.root, '.work', 'locks')
        os.makedirs(locks)
        old = wt.LOCK
        wt.LOCK = os.path.join(locks, 'importer.lock')
        self.addCleanup(setattr, wt, 'LOCK', old)
        commit(self.root, 'notes', **{'notes_dot_txt': 'n1\n'})
        self.old = run(self.root, 'rev-parse', 'HEAD')
        p = self.worktree('T')
        run(p, 'reset', '-q', '--hard', self.old)
        commit(p, 'builder 1', **{'src_dot_txt': 's1\ns2\ns3\nmine\n'})
        self.new = commit(p, 'builder 2', **{'lib/b_dot_txt': 'b\n'})

    def land(self, refuse_dirty=False):
        with redirect_stdout(io.StringIO()) as out:
            why, landed = wt.land('run/x', self.old, self.new, wait=1, refuse_dirty=refuse_dirty)
        return why, landed, out.getvalue()

    def test_unrelated_commit_lands_both(self):
        theirs = commit(self.root, 'coordinator', **{'other_dot_txt': 'o\n'})
        write(self.root, 'notes.txt', 'n1\nuncommitted\n')         # the coordinator's edit survives the landing
        why, landed, out = self.land()
        self.assertIsNone(why)
        self.assertIn('replayed', out)
        self.assertEqual(run(self.root, 'rev-parse', 'run/x'), landed)
        self.assertEqual(run(self.root, 'rev-parse', 'run/x~2'), theirs)
        self.assertEqual(run(self.root, 'log', '--format=%s', '-3', 'run/x').splitlines(),
                         ['builder 2', 'builder 1', 'coordinator'])
        self.assertEqual(run(self.root, 'show', 'run/x:other.txt'), 'o')
        self.assertEqual(run(self.root, 'show', 'run/x:lib/b.txt'), 'b')
        self.assertEqual(rd(os.path.join(self.root, 'src.txt')), 's1\ns2\ns3\nmine\n')
        self.assertEqual(rd(os.path.join(self.root, 'notes.txt')), 'n1\nuncommitted\n')
        self.assertEqual(run(self.root, 'status', '--porcelain', '-uno'), 'M notes.txt')

    def test_not_moved_fast_forwards(self):
        why, landed, out = self.land()
        self.assertIsNone(why)
        self.assertEqual(landed, self.new)
        self.assertNotIn('replayed', out)

    def test_shared_path_refused(self):
        theirs = commit(self.root, 'coordinator', **{'src_dot_txt': 's1\ns2\ns3\n\ntheirs\n', 'other_dot_txt': 'o\n'})
        why, landed, _ = self.land()
        self.assertIn('src.txt', why)
        self.assertNotIn('other.txt', why)
        self.assertIsNone(landed)
        self.assertEqual(run(self.root, 'rev-parse', 'run/x'), theirs)

    def test_generated_file_refused(self):
        theirs = commit(self.root, 'regenerated', **{'docs/imports_dot_md': 'R\n'})
        why, landed, _ = self.land(refuse_dirty=True)
        self.assertIn('generated files: docs/imports.md', why)
        self.assertEqual(run(self.root, 'rev-parse', 'run/x'), theirs)


def bo_args(*names, **kw):
    d = dict(names=list(names), batch_name=None, target='run/x', lib=[], pool=1, trailer=None, dry=False,
             picks_only=False, resume=False, keep=False, lock_wait=1)
    d.update(kw)
    return argparse.Namespace(**d)


class Stacked(GitRepo):
    """Item 4: a branch made on another builder's branch records its parent and lands after it, its own commits only."""

    def setUp(self):
        super().setUp()
        locks = os.path.join(self.root, '.work', 'locks')
        os.makedirs(locks)
        stubs = {'LOCK': os.path.join(locks, 'importer.lock'), 'importer_agent': lambda path: 'x',
                 'libs_touched': lambda *a: [], 'regen_and_commit': lambda *a, **k: None,
                 'say_losses': lambda r: None, 'judge_losses': lambda *a: dict(gained=[], gained_tiebreak=[], moves=[], lost_outright=[]),
                 'regenerate_rebased': lambda a, path, st: None,
                 # no junctions (cmd /c mklink is Windows-only; CI runs Linux): these tests are about git
                 'furnish': lambda path, libs=True, pseudo=True: None}
        for k, v in stubs.items():
            old = getattr(wt, k)
            setattr(wt, k, v)
            self.addCleanup(setattr, wt, k, old)
        old = wt.ledger.load
        wt.ledger.load = lambda *a, **k: {}
        self.addCleanup(setattr, wt.ledger, 'load', old)
        self.quiet(wt.cmd_new, argparse.Namespace(name='P', base='run/x'))
        self.pp = os.path.join(self.wtdir, 'P')
        self.p1 = commit(self.pp, 'parent work', **{'p_dot_txt': 'p\n'})
        self.quiet(wt.cmd_new, argparse.Namespace(name='C', base='wt/P'))
        self.cp = os.path.join(self.wtdir, 'C')
        self.c1 = commit(self.cp, 'child work', **{'c_dot_txt': 'c\n'})

    def quiet(self, fn, *args):
        with redirect_stdout(io.StringIO()) as out:
            fn(*args)
        return out.getvalue()

    def land_parent_differently(self):
        """P's commit lands on run/x with the same author, time and subject but another diff (a conflict resolved
        by hand): not patch-equivalent, so a plain rebase of the child would replay it and conflict."""
        run(self.root, 'cherry-pick', '-n', self.p1)
        write(self.root, 'p.txt', 'p\nresolved\n')
        run(self.root, 'add', 'p.txt')
        run(self.root, 'commit', '-q', '-C', self.p1)
        return run(self.root, 'rev-parse', 'HEAD')

    def test_new_records_parent(self):
        self.assertEqual(wt.read_base('C'), (self.p1, 'P'))             # P's tip when C was made, and P
        self.assertEqual(wt.read_base('P'), (self.base, None))
        with open(os.path.join(self.wtdir, 'OLD.base'), 'w') as f:      # an old one-line record still reads
            f.write(self.base + '\n')
        self.assertEqual(wt.read_base('OLD'), (self.base, None))

    def test_list_marks_parent_landed(self):
        out = self.quiet(wt.cmd_list, argparse.Namespace(unlanded=False))
        self.assertIn('on wt/P (unlanded)', out)
        self.land_parent_differently()
        out = self.quiet(wt.cmd_list, argparse.Namespace(unlanded=False))
        self.assertIn('on wt/P (parent landed: rebase)', out)

    def test_rebase_takes_child_commits_only(self):
        commit(self.root, 'run moves', **{'r_dot_txt': 'r\n'})
        with self.assertRaises(SystemExit) as e:                        # the parent not landed: refused
            self.quiet(wt.cmd_rebase, rebase_args('C', onto='run/x'))
        self.assertIn('stacked on wt/P', str(e.exception))
        tip = self.land_parent_differently()
        self.quiet(wt.cmd_rebase, rebase_args('C', onto='run/x'))
        self.assertEqual(run(self.cp, 'rev-parse', 'HEAD~1'), tip)
        self.assertEqual(run(self.cp, 'log', '--format=%s', f'{tip}..HEAD'), 'child work')
        self.assertEqual(rd(os.path.join(self.cp, 'p.txt')), 'p\nresolved\n')
        self.assertEqual(wt.read_base('C'), (tip, None))

    def test_bring_over_child_alone_refused(self):
        with self.assertRaises(SystemExit) as e:
            self.quiet(wt.cmd_bring_over, bo_args('C'))
        self.assertIn('stacked on wt/P', str(e.exception))
        with self.assertRaises(SystemExit) as e:
            self.quiet(wt.cmd_bring_over, bo_args('C', 'P'))
        self.assertIn('put it before', str(e.exception))
        self.assertEqual(run(self.root, 'rev-parse', 'run/x'), self.base)

    def test_batch_parent_first_lands(self):
        write(self.root, 'other.txt', 'uncommitted\n')                  # untracked: no effect
        self.quiet(wt.cmd_bring_over, bo_args('P', 'C'))
        self.assertEqual(run(self.root, 'log', '--format=%s', f'{self.base}..run/x').splitlines(),
                         ['child work', 'parent work'])

    def test_child_after_parent_landed_brings_own_commits(self):
        tip = self.land_parent_differently()
        self.quiet(wt.cmd_bring_over, bo_args('C'))
        self.assertEqual(run(self.root, 'log', '--format=%s', f'{tip}..run/x').splitlines(), ['child work'])
        self.assertEqual(rd(os.path.join(self.root, 'p.txt')), 'p\nresolved\n')


if __name__ == '__main__':
    unittest.main()
