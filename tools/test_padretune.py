"""Tests for the 2026-11-11 pad-retune changes (D227):

  D227  wt.py bring-over's pad retune commits both of sendpad.py's outputs (aux_roots_send.h and auxroots.py's
        SEND_SYMBOL_PAD), so the regeneration after it no longer refuses with "regenerating changed other files";
        any other changed file is still left out (and still refuses there)

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


class Retune(Repo):
    """D227: the retune commit takes both of sendpad's outputs."""

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

    def test_unchanged_pad_commits_nothing(self):
        commit(self.td, 'Change x.h', {'app/client/x.h': 'int y;\n',
                                       'app/client/aux_roots_send.h': 'enum P { A, B };\n',
                                       'tools/re/auxroots.py': 'SEND_SYMBOL_PAD = 49\n'})
        head = run(self.td, 'rev-parse', 'HEAD')
        self.assertFalse(self.retune())
        self.assertEqual(run(self.td, 'rev-parse', 'HEAD'), head)


if __name__ == '__main__':
    unittest.main()
