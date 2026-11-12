"""Tests for the 2026-11-11 pad-retune changes (D227, D228):

  D227  wt.py bring-over's pad retune commits both of sendpad.py's outputs (aux_roots_send.h and auxroots.py's
        SEND_SYMBOL_PAD), so the regeneration after it no longer refuses with "regenerating changed other files";
        any other changed file is still left out (and still refuses there)
  D228  sendpad.py takes its watch set from the onto commit's ledger when HEAD is a `wt.py rebase` record commit
        (--watch-from REV names it outright; wt.py passes its target when the landing carries such a commit)

    python -m unittest discover -s tools -p "test_*.py"

Standard library and git only; no VM, no verify (verify and sendpad's sweep are stubbed).
"""
import argparse
import contextlib
import io
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
for d in ('vc6', 're'):
    sys.path.insert(0, os.path.join(HERE, d))

import sendpad  # noqa: E402
import wt  # noqa: E402

RECORD = ('Record the verified pairs of wt/O5 after rebasing it onto run/2026-11-10 6cfb06e15: generated files '
          'regenerated, not merged, no source change, 96 gained, 0 D66 moves, 2 lost outright')


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


def commit(cwd, msg, files):
    for rel, text in files.items():
        write(cwd, rel, text)
    run(cwd, 'add', '-A')
    run(cwd, 'commit', '-q', '-m', msg)
    return run(cwd, 'rev-parse', 'HEAD')


class Repo(unittest.TestCase):
    def setUp(self):
        self.td = tempfile.mkdtemp(prefix='padretune_')
        self.addCleanup(shutil.rmtree, self.td, ignore_errors=True)
        run(self.td, 'init', '-q', '-b', 'master')
        for k, v in (('user.name', 't'), ('user.email', 't@t'), ('core.autocrlf', 'false'),
                     ('commit.gpgsign', 'false')):
            run(self.td, 'config', k, v)
        self.base = commit(self.td, 'base', {'app/client/aux_roots_send.h': 'enum P { };\n',
                                             'tools/re/auxroots.py': 'SEND_SYMBOL_PAD = 44\n',
                                             'app/client/x.h': 'int x;\n'})


class WatchSource(Repo):
    """D228: the record commit's subject names the onto commit; its ledger still holds what the rebase lost."""

    def test_record_commit_gives_onto(self):
        commit(self.td, RECORD, {'docs/data/imports/x.json': '{}\n'})
        self.assertEqual(sendpad.watch_source(self.td), '6cfb06e15')

    def test_other_commit_gives_head(self):
        commit(self.td, 'Rebuild Foo::Bar from the client', {'app/client/x.h': 'int y;\n'})
        self.assertEqual(sendpad.watch_source(self.td), 'HEAD')

    def test_regex_matches_record_subject(self):
        subj = wt.record_subject('of wt/O5 after rebasing it onto run/2026-11-10 6cfb06e15: generated files '
                                 'regenerated, not merged', {'gained': [], 'gained_tiebreak': [], 'moves': []})
        self.assertEqual(sendpad.REBASE_RECORD.match(subj).group(2), '6cfb06e15')
        self.assertTrue(wt.REBASE_RECORD.match(subj))
        self.assertIsNone(wt.REBASE_RECORD.match(wt.record_subject('of wt/O5 after the SEND_SYMBOL_PAD retune', {
            'gained': [], 'gained_tiebreak': [], 'moves': []})))

    def test_carries_rebase_record(self):
        commit(self.td, RECORD, {'docs/data/imports/x.json': '{}\n'})
        commit(self.td, 'a later commit', {'app/client/x.h': 'int z;\n'})
        self.assertTrue(wt.carries_rebase_record(self.td, self.base))
        self.assertFalse(wt.carries_rebase_record(self.td, run(self.td, 'rev-parse', 'HEAD~1')))


class SendpadMainSource(unittest.TestCase):
    """D228: main() reads the watch set from --watch-from, else from watch_source()."""

    def go(self, argv, source):
        td = tempfile.mkdtemp(prefix='sendpadmain_')
        self.addCleanup(shutil.rmtree, td, ignore_errors=True)
        asked = []
        saved = {k: getattr(sendpad, k) for k in ('REPO', 'watch_source', 'watch_list', 'sweep')}
        self.addCleanup(lambda: [setattr(sendpad, k, v) for k, v in saved.items()])
        sendpad.REPO = td
        sendpad.watch_source = lambda repo=None: source
        sendpad.watch_list = lambda src='HEAD': asked.append(src) or list(sendpad.WATCH)
        sendpad.sweep = lambda a, log, watch: (44, None, [])
        with contextlib.redirect_stdout(io.StringIO()):
            sendpad.main(argv)
        return asked

    def test_watch_from(self):
        self.assertEqual(self.go(['--dry', '--watch-from', 'abc1234'], 'HEAD'), ['abc1234'])

    def test_auto(self):
        self.assertEqual(self.go(['--dry'], '6cfb06e15'), ['6cfb06e15'])


class Retune(Repo):
    """D227: the retune commit takes both of sendpad's outputs; D228: --watch-from TARGET after a rebase record."""

    def setUp(self):
        super().setUp()
        self.calls = []
        names = ('rev', 'run_logged', 'tool', 'corrections_at', 'verify_finished')
        saved = {k: getattr(wt, k) for k in names}
        saved_cmp, saved_load = wt.pairs_cmp.compare, wt.ledger.load
        self.addCleanup(lambda: [setattr(wt, k, v) for k, v in saved.items()])
        self.addCleanup(setattr, wt.pairs_cmp, 'compare', saved_cmp)
        self.addCleanup(setattr, wt.ledger, 'load', saved_load)
        wt.rev = lambda r, cwd=None: self.base
        wt.tool = lambda bo, new, old: ['tools/vc6/' + new]
        wt.corrections_at = lambda old: None
        wt.verify_finished = lambda log, libs: []
        wt.ledger.load = lambda src=None, repo=None: {}
        wt.pairs_cmp.compare = lambda a, b, c: {'lost': [('app', 'sclient', '0x758210')],
                                                'lost_outright': ['0x758210']}
        write(self.td, 'tools/vc6/sendpad.py', '# accepts --watch-from\n')

        def run_logged(args, cwd, log):
            self.calls.append(args)
            if '--includes' in args:
                return 0, ['app/client/x.h']
            if any(str(x).endswith('sendpad.py') for x in args):
                write(self.td, 'app/client/aux_roots_send.h', 'enum P { A, B };\n')
                write(self.td, 'tools/re/auxroots.py', 'SEND_SYMBOL_PAD = 49\n')
                return 0, ['sendpad: window 46..53; was 44; pad set to 49 (1 s)']
            return 0, ['verify ok']
        wt.run_logged = run_logged
        self.a = argparse.Namespace(pool=1, lock_wait=10, trailer='Co-Authored-By: t <t@t>')

    def retune(self):
        with contextlib.redirect_stdout(io.StringIO()):
            return wt.sendpad_retune(self.a, self.td, ['app'], 'run/x')

    def test_commits_both_outputs_only(self):
        commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n', 'tools/vc6/sendpad.py': '# --watch-from\n'})
        write(self.td, 'docs/other.md', 'stray\n')
        run(self.td, 'add', 'docs/other.md')
        run(self.td, 'commit', '-q', '-m', 'add other')
        write(self.td, 'docs/other.md', 'changed by something else\n')
        self.assertTrue(self.retune())
        files = run(self.td, 'show', '--name-only', '--format=', 'HEAD').split()
        self.assertEqual(sorted(files), sorted(wt.PAD_OUTPUTS))
        self.assertEqual(wt.dirty_tracked(self.td), ['docs/other.md'])   # regen_and_commit still refuses this
        sweep = [c for c in self.calls if any(str(x).endswith('sendpad.py') for x in c) and '--includes' not in c]
        self.assertEqual(sweep[0][-2:], ['--watch-from', self.base])   # P-L18-3: always, not only after a record

    def test_unchanged_pad_commits_nothing(self):
        commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n',
                                       'app/client/aux_roots_send.h': 'enum P { A, B };\n',
                                       'tools/re/auxroots.py': 'SEND_SYMBOL_PAD = 49\n'})
        head = run(self.td, 'rev-parse', 'HEAD')
        self.assertFalse(self.retune())
        self.assertEqual(run(self.td, 'rev-parse', 'HEAD'), head)

    def test_watch_from_target_after_own_verify_commit(self):
        # P-L18-3: a builder's own verify commit (no rebase record) may have recorded the loss already
        commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n'})
        commit(self.td, 'Rebuild x, verify recorded', {'docs/data/imports/x.json': '{}\n'})
        self.assertTrue(self.retune())
        sweep = [c for c in self.calls if any(str(x).endswith('sendpad.py') for x in c) and '--includes' not in c]
        self.assertEqual(sweep[0][-2:], ['--watch-from', self.base])

    def test_old_sendpad_without_watch_from(self):
        write(self.td, 'tools/vc6/sendpad.py', '# predates D228\n')
        commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n'})
        self.assertTrue(self.retune())
        self.assertNotIn('--watch-from', [x for c in self.calls for x in c])

    def test_watch_from_target_after_rebase_record(self):
        commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n'})
        commit(self.td, RECORD, {'docs/data/imports/x.json': '{}\n'})
        self.assertTrue(self.retune())
        sweep = [c for c in self.calls if any(str(x).endswith('sendpad.py') for x in c) and '--includes' not in c]
        self.assertEqual(sweep[0][-2:], ['--watch-from', self.base])


if __name__ == '__main__':
    unittest.main()
