"""machine.py's resolution order and baseline.py's store checks on a synthetic store (no game, any OS):
python -m unittest discover -s tools -p "test_*.py" (or run this file). Also the draw logs' structural compare
(d3d8shot D3D8SHOT_DRAWLOG, test-plan C3) on synthetic logs."""
import contextlib
import io
import json
import os
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'harness'))
import baseline  # noqa: E402
import bmp2png  # noqa: E402
import machine  # noqa: E402


class Resolution(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.cfg = os.path.join(self.tmp.name, 'machine.json')
        with open(self.cfg, 'w', encoding='utf-8') as f:
            json.dump({'net7_bin': 'srv', 'label': 'box'}, f)

    def tearDown(self):
        self.tmp.cleanup()

    def test_env_wins(self):
        env = {'ENB_MACHINE': self.cfg, 'ENB_NET7_BIN': r'X:\n7'}
        self.assertEqual(machine.resolve('net7_bin', env), (r'X:\n7', 'env ENB_NET7_BIN'))

    def test_config_then_default(self):
        env = {'ENB_MACHINE': self.cfg}
        v, src = machine.resolve('net7_bin', env)
        self.assertEqual(v, os.path.join(self.tmp.name, 'srv'))        # relative to the config file
        self.assertTrue(src.startswith('config '))
        self.assertEqual(machine.resolve('label', env)[0], 'box')
        v, src = machine.resolve('baselines', env)
        self.assertEqual((v, src), (os.path.join(machine.base(), 'baselines'), 'default'))

    def test_old_env_names(self):
        env = {'ENB_MACHINE': self.cfg, 'ENB_RETAIL_INSTALL': r'Y:\EnB', 'ANTPROX_EXE': r'Y:\a.exe'}
        self.assertEqual(machine.resolve('retail_install', env)[0], r'Y:\EnB')
        self.assertEqual(machine.resolve('antprox', env)[0], r'Y:\a.exe')
        self.assertEqual(machine.resolve('antprox_replay', env)[0], r'Y:\a.exe')     # it overrides both

    def test_replay_proxy_split(self):
        env = {'ENB_MACHINE': self.cfg, 'ENB_ANTPROX_REPLAY': r'Y:\cs.exe'}
        self.assertEqual(machine.resolve('antprox_replay', env), (r'Y:\cs.exe', 'env ENB_ANTPROX_REPLAY'))
        env['ANTPROX_EXE'] = r'Y:\one.exe'
        self.assertEqual(machine.resolve('antprox_replay', env), (r'Y:\one.exe', 'env ANTPROX_EXE'))

    def test_recordings_follow_baselines(self):
        env = {'ENB_MACHINE': self.cfg, 'ENB_BASELINES': r'Z:\store'}
        self.assertEqual(machine.resolve('recordings', env), (os.path.join(r'Z:\store', 'inputs', 'captures'),
                                                             'default'))
        env['ENB_RECORDINGS'] = r'Z:\recs'
        self.assertEqual(machine.resolve('recordings', env), (r'Z:\recs', 'env ENB_RECORDINGS'))

    def test_proxies_from_antprox_dir_else_fallback(self):
        store = os.path.join(self.tmp.name, 'store')
        env = {'ENB_MACHINE': self.cfg, 'ENB_BASELINES': store}
        d = os.path.join(store, 'tools', 'antprox')
        self.assertEqual(machine.resolve('antprox_dir', env)[0], d)
        v, src = machine.resolve('antprox', env)                     # no folder: the old default, said so
        self.assertEqual(v, machine.defaults()['antprox'])
        self.assertIn('fallback', src)
        os.makedirs(d)
        self.assertEqual(machine.resolve('antprox', env)[0], os.path.join(d, 'antprox-rs.exe'))
        self.assertEqual(machine.resolve('antreplay', env)[0], os.path.join(d, 'antreplay.exe'))
        self.assertEqual(machine.resolve('antprox_replay', env)[0], os.path.join(d, 'antprox.exe'))
        env['ENB_ANTREPLAY'] = r'Z:\r.exe'                           # each stays overridable on its own
        self.assertEqual(machine.resolve('antreplay', env)[0], r'Z:\r.exe')
        self.assertNotIn('antprox_live', machine.KEYS)

    def test_clientrun_passes_replayexe_only_when_not_beside(self):
        try:
            import clientrun
        except (ImportError, AttributeError, OSError):
            self.skipTest('clientrun needs Windows')
        from unittest import mock
        self.assertEqual((clientrun.antprox_exe(), clientrun.antprox_exe(['/SERVER:x'])),
                         (clientrun.ANTPROX_REPLAY, clientrun.ANTPROX))
        beside = os.path.join(os.path.dirname(os.path.abspath(clientrun.ANTPROX_REPLAY)), 'antreplay.exe')
        with mock.patch.dict(os.environ, {'ENB_ANTREPLAY': beside}):
            self.assertEqual(clientrun.replay_exe_args(), [])
        with mock.patch.dict(os.environ, {'ENB_ANTREPLAY': r'Z:\elsewhere\antreplay.exe'}):
            self.assertEqual(clientrun.replay_exe_args(), [r'/REPLAYEXE:Z:\elsewhere\antreplay.exe'])

    def test_clientrun_passes_recordings_only_when_present(self):
        try:
            import clientrun
        except (ImportError, AttributeError, OSError):
            self.skipTest('clientrun needs Windows')
        from unittest import mock
        with mock.patch.dict(os.environ, {'ENB_RECORDINGS': self.tmp.name}):
            self.assertEqual(clientrun.recordings_folder(), self.tmp.name)
        with mock.patch.dict(os.environ, {'ENB_RECORDINGS': os.path.join(self.tmp.name, 'absent')}):
            self.assertIsNone(clientrun.recordings_folder())

    def test_unknown_key_in_config(self):
        with open(self.cfg, 'w', encoding='utf-8') as f:
            json.dump({'bogus': 1}, f)
        with self.assertRaises(SystemExit):
            machine.resolve('net7_bin', {'ENB_MACHINE': self.cfg})

    def test_worktree_follows_gitdir(self):
        main = os.path.join(self.tmp.name, 'dev', 'EnB')
        gitdir = os.path.join(main, '.git', 'worktrees', 'wt1')
        os.makedirs(gitdir)
        with open(os.path.join(gitdir, 'commondir'), 'w') as f:
            f.write('../..\n')
        wt = os.path.join(self.tmp.name, 'elsewhere', 'wt1')
        os.makedirs(wt)
        with open(os.path.join(wt, '.git'), 'w') as f:
            f.write('gitdir: ' + gitdir.replace('\\', '/') + '\n')
        self.assertEqual(os.path.normcase(machine.main_checkout(wt)), os.path.normcase(os.path.abspath(main)))
        self.assertEqual(os.path.normcase(machine.base(wt)), os.path.normcase(os.path.join(self.tmp.name, 'dev')))
        self.assertEqual(machine.main_checkout(main), os.path.abspath(main))


def png(path, w, h, value=0, box=None):
    """A w x h RGB PNG of grey value, with box (x0, y0, x1, y1) white."""
    rows = []
    for y in range(h):
        row = bytearray([value] * (3 * w))
        if box and box[1] <= y < box[3]:
            for x in range(box[0], box[2]):
                row[3 * x: 3 * x + 3] = b'\xff\xff\xff'
        rows.append(bytes(row))
    bmp2png.write_png(path, w, h, rows)


def make_run(scen_dir, run, scenario='scn', box=None, packets=('A 4', 'B 8'), drawlog=None):
    d = os.path.join(scen_dir, run)
    os.makedirs(d)
    png(os.path.join(d, 'shot_one_00010.png'), 64, 64, 10, box)
    if drawlog is not None:
        baseline.write_json(os.path.join(d, 'shot_one_00010.json'), drawlog)
    with open(os.path.join(d, 'e2e.log'), 'w') as f:
        f.write(f'00:00:00 scenario {scenario}\n00:00:01   shot one taken\n')
    with open(os.path.join(d, f'result-{scenario}.json'), 'w') as f:
        json.dump({'name': scenario, 'passed': True, 'expected_fail': None}, f)
    baseline.write_json(os.path.join(d, 'packets.json'), {scenario: list(packets)})
    files = {f: {'sha256': baseline.sha256_file(os.path.join(d, f)), 'bytes': os.path.getsize(os.path.join(d, f))}
             for f in sorted(os.listdir(d))}
    baseline.write_json(os.path.join(d, 'manifest.json'),
                        {'schema': 1, 'scenario': scenario, 'verdict': 'PASS', 'client': {'name': 'retail-00000000'},
                         'env': 'box-64x64', 'shots': [['one', 'shot_one_00010.png']], 'files': files,
                         **({'drawlogs': [['one', 'shot_one_00010.json']]} if drawlog is not None else {})})
    return d


class Store(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.store = self.tmp.name
        self.scen = os.path.join(self.store, 'runs', 'retail-00000000', 'box-64x64', 'scn')
        self.runs = [make_run(self.scen, '20261124-000001'),
                     make_run(self.scen, '20261124-000002', box=(0, 0, 8, 8)),
                     make_run(self.scen, '20261124-000003', box=(0, 0, 16, 8))]
        self.noise = baseline.write_noise(self.scen)

    def tearDown(self):
        self.tmp.cleanup()

    def verify(self, *extra):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc = baseline.main(['verify', '--store', self.store] + list(extra))
        return rc, out.getvalue()

    def test_noise(self):
        s = self.noise['shots']['one']
        self.assertEqual(len(s['pairs']), 3)
        self.assertEqual(s['max_differ'], 8)        # 16x16 cells of 4x4 pixels: the 16x8 box covers 4 x 2
        self.assertTrue(self.noise['packets_equal'] and self.noise['verdicts_equal'])

    def test_clean_store_verifies(self):
        rc, out = self.verify()
        self.assertEqual(rc, 0, out)
        self.assertIn('3 runs', out)

    def test_corrupt_file(self):
        with open(os.path.join(self.runs[0], 'e2e.log'), 'a') as f:
            f.write('x')
        rc, out = self.verify()
        self.assertEqual(rc, 1)
        self.assertIn('SHA-256 differs', out)

    def test_lfs_pointer(self):
        with open(os.path.join(self.runs[1], 'shot_one_00010.png'), 'wb') as f:
            f.write(b'version https://git-lfs.github.com/spec/v1\noid sha256:00\nsize 1\n')
        rc, out = self.verify()
        self.assertEqual(rc, 1)
        self.assertIn('git lfs pull', out)
        self.assertNotIn('corrupt', out)

    def test_missing_and_extra(self):
        os.remove(os.path.join(self.runs[2], 'packets.json'))
        with open(os.path.join(self.runs[2], 'stray.txt'), 'w') as f:
            f.write('x')
        rc, out = self.verify()
        self.assertEqual(rc, 1)
        self.assertIn('packets.json: missing', out)
        self.assertIn('stray.txt: not in the manifest', out)

    def test_verify_run_same_and_diff(self):
        other = os.path.join(self.tmp.name, 'other')
        same = make_run(other, 'a', box=(0, 0, 4, 4))
        rc, out = self.verify('--run', same, '--client', 'retail-00000000', '--env', 'box-64x64')
        self.assertEqual(rc, 0, out)
        self.assertIn('scn: SAME', out)
        diff = make_run(other, 'b', box=(0, 0, 64, 64), packets=('A 4',))
        rc, out = self.verify('--run', diff)
        self.assertEqual(rc, 1)
        self.assertIn('scn: DIFF', out)
        self.assertIn('packets', out)

    def test_verify_run_audio_only(self):
        # AU-01: --audio-only prints the audio verdict alone; packets and shots that differ do not count
        other = os.path.join(self.tmp.name, 'other')
        diff = make_run(other, 'b', box=(0, 0, 64, 64), packets=('A 4',))
        rc, out = self.verify('--run', diff, '--audio-only')
        self.assertEqual(rc, 0, out)
        self.assertIn('scn: AUDIO NONE no audio stored', out)
        self.assertNotIn('packets', out)
        self.assertIn('--audio-only: 0 SAME, 0 RETAIL-UNSTABLE, 0 DIFF, 1 NONE', out)
        self.assertEqual(baseline.AUDIO_ONLY_WORD[baseline.RETAIL_UNSTABLE], 'RETAIL-UNSTABLE')


IDENT = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]


def drawlog(n=4, fog=0, view_x=0.0, extra_at=None, cull=1, tex_hash='00000000000000aa'):
    """A synthetic d3d8shot draw log of n draws (d3d8shot.c Log_Draw's schema)."""
    view = list(IDENT)
    view[12] = view_x
    draws = []
    for i in range(n):
        draws.append({'i': i, 'call': 'DIP', 'prim': 4, 'prims': 10 + i, 'verts': 30 + i, 'stride': 32,
                      'fvf': 0x152, 'tex': [0, -1], 'view': 0, 'proj': 0, 'world': IDENT,
                      'rs': [1, cull, fog], 'ts': [[4, 2, 0], [1, 2, 1]]})
        if extra_at == i:
            draws.append({'i': i, 'call': 'DPUP', 'prim': 5, 'prims': 2, 'verts': 4, 'stride': 28, 'fvf': 0x144,
                          'tex': [-1, -1], 'view': 0, 'proj': 0, 'world': IDENT, 'rs': [0, 1, fog],
                          'ts': [[1, 2, 0], [1, 2, 1]]})
    return {'format': 'd3d8shot-drawlog 1', 'shot': 'one', 'frame': 10, 'counts': {'draws': len(draws)},
            'render_states': ['ZENABLE', 'CULLMODE', 'FOGCOLOR'], 'stage_states': ['COLOROP', 'COLORARG1', 'TEXCOORDINDEX'],
            'textures': [{'id': 0, 'type': 3, 'w': 64, 'h': 64, 'format': 21, 'levels': 1, 'usage': 0, 'pool': 1,
                          'hash': tex_hash}],
            'views': [view], 'projections': [IDENT], 'sets': [{'draw': 0, 'view': 0}, {'draw': 0, 'proj': 0}],
            'draws': draws}


class DrawLogs(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.store = self.tmp.name
        self.scen = os.path.join(self.store, 'runs', 'retail-00000000', 'box-64x64', 'scn')

    def tearDown(self):
        self.tmp.cleanup()

    def stored(self, *logs):
        for k, lg in enumerate(logs):
            make_run(self.scen, f'20261124-00000{k + 1}', drawlog=lg)
        return baseline.write_noise(self.scen)['shots']['one'].get('drawlog')

    def check(self, log):
        other = os.path.join(self.tmp.name, 'other')
        mine = make_run(other, f'r{len(os.listdir(other)) if os.path.isdir(other) else 0}', drawlog=log)
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc = baseline.main(['verify', '--store', self.store, '--run', mine, '--client', 'retail-00000000',
                                '--env', 'box-64x64'])
        return rc, out.getvalue()

    def test_identical_runs(self):
        nz = self.stored(drawlog(), drawlog(), drawlog())
        self.assertTrue(nz['identical'])
        self.assertEqual((nz['draws'], nz['varying'], nz['unmatched_max']), ([4, 4], [], 0))
        self.assertEqual(nz['matrix_spread']['view'], 0.0)
        rc, out = self.check(drawlog())
        self.assertEqual(rc, 0, out)
        self.assertIn('draw logs 1 of 1 the same', out)

    def test_frame_number_is_not_a_difference(self):
        a, b = drawlog(), drawlog()
        b['frame'] = 99
        self.assertTrue(self.stored(a, b)['identical'])

    def test_varying_field_allowed_other_field_not(self):
        nz = self.stored(drawlog(fog=0), drawlog(fog=0), drawlog(fog=7))
        self.assertFalse(nz['identical'])
        self.assertEqual(nz['varying'], ['rs.FOGCOLOR'])
        rc, out = self.check(drawlog(fog=9))            # a value no stored run had, in a field that varies
        self.assertEqual(rc, 0, out)
        rc, out = self.check(drawlog(cull=3))
        self.assertEqual(rc, 1, out)
        self.assertIn('draw log one', out)
        self.assertIn('first difference draw 0', out)
        self.assertIn('rs.CULLMODE', out)
        self.assertIn('1 fields differ in 4 of 4 paired draws', out)
        self.assertIn('as a multiset: 1 draw states not stored, 1 stored ones missing', out)

    def test_multiset_draw_order_and_lod(self):
        def lod(log, i, prims):                         # draw i at another LOD (asteroids of a playback)
            log['draws'][i]['prims'], log['draws'][i]['verts'] = prims, 3 * prims
            return log

        def tex(log, i):                                # draw i with a texture of its own (a separate object)
            log['textures'].append(dict(log['textures'][0], id=1, hash=f'{i:016x}'))
            log['draws'][i]['tex'] = [len(log['textures']) - 1, -1]
            return log
        base = lambda: tex(tex(tex(drawlog(), 1), 2), 3)        # noqa: E731
        self.stored(base(), base(), base())
        moved = base()
        moved['draws'] = moved['draws'][::-1]           # the same draws in another order
        rc, out = self.check(moved)
        self.assertEqual(rc, 0, out)
        self.assertIn('draw logs 1 of 1 the same (1 as a multiset)', out)
        rc, out = self.check(lod(base(), 2, 99))
        self.assertEqual(rc, 0, out)
        self.assertIn('(1 as a multiset)', out)
        ok, text = baseline.drawlog_verdict(baseline.drawlog_path(os.path.join(self.tmp.name, 'other', 'r1',
                                                                               'shot_one_00010.png')),
                                            [os.path.join(self.scen, r, 'shot_one_00010.json')
                                             for r in baseline.stored_runs(self.scen)], None)
        self.assertTrue(ok, text)
        self.assertIn('in order: first difference draw 2', text)
        self.assertIn('2 fields differ in 1 of 4 paired draws', text)

    def test_multiset_counts_and_still_draws(self):
        self.stored(drawlog(), drawlog())
        rc, out = self.check(drawlog(n=6))               # two draws more of a state: no stored run varies so
        self.assertEqual(rc, 1, out)
        self.assertIn('draw counts per state 2 apart (stored runs up to 0)', out)
        log = drawlog()
        log['draws'] = log['draws'][::-1]
        log['draws'][0]['world'] = IDENT[:12] + [5.0, 0, 0, 1]
        rc, out = self.check(log)
        self.assertEqual(rc, 1, out)
        self.assertIn('world matrices of 1 draws out of the stored spread', out)

    def test_texture_identity(self):
        self.stored(drawlog(), drawlog())
        rc, out = self.check(drawlog(tex_hash='00000000000000bb'))
        self.assertEqual(rc, 1, out)
        self.assertIn('tex0.hash', out)

    def test_matrix_tolerance_from_the_spread(self):
        nz = self.stored(drawlog(view_x=0.0), drawlog(view_x=0.01), drawlog(view_x=0.005))
        self.assertAlmostEqual(nz['matrix_spread']['view'], 0.01, places=6)
        self.assertEqual(nz['varying'], [])             # matrices are not fields
        rc, out = self.check(drawlog(view_x=0.024))     # 0.014 from the nearest stored run: within 1.5 x 0.01
        self.assertEqual(rc, 0, out)
        rc, out = self.check(drawlog(view_x=0.5))
        self.assertEqual(rc, 1, out)
        self.assertIn('view in 4 draws from draw 0', out)

    def test_matrix_tolerance_is_per_draw(self):
        def moved(log, i, x):                           # draw i's world translated by x (an object that moves)
            log['draws'][i]['world'] = IDENT[:12] + [x, 0, 0, 1]
            return log
        nz = self.stored(moved(drawlog(), 2, 0.0), moved(drawlog(), 2, 100.0), moved(drawlog(), 2, 300.0))
        self.assertEqual(nz['matrix_spread']['world'], 300.0)
        self.assertEqual(sorted(nz['matrix_draws']['20261124-000001']), ['2'])
        rc, out = self.check(moved(drawlog(), 2, 500.0))     # the moving draw within its own spread
        self.assertEqual(rc, 0, out)
        rc, out = self.check(moved(moved(drawlog(), 2, 0.0), 1, 1.0))   # a still draw moved: the floor holds it
        self.assertEqual(rc, 1, out)
        self.assertIn('matrices world in 1 draws from draw 1', out)

    def test_extra_draw_aligned_not_shifted(self):
        self.stored(drawlog(), drawlog())
        rc, out = self.check(drawlog(extra_at=1))
        self.assertEqual(rc, 1, out)
        self.assertIn('5 draws, stored 4', out)
        self.assertIn('1 without a partner from draw 2', out)
        self.assertNotIn('first difference', out)       # the draws after the extra one still pair up

    def test_draw_count_varying_in_the_store_allowed(self):
        nz = self.stored(drawlog(), drawlog(extra_at=1), drawlog())
        self.assertEqual((nz['draws'], nz['unmatched_max']), ([4, 5], 1))
        rc, out = self.check(drawlog(extra_at=1))
        self.assertEqual(rc, 0, out)

    def test_old_runs_without_draw_logs(self):
        self.stored(None, None)
        rc, out = self.check(drawlog())
        self.assertEqual(rc, 0, out)
        self.assertIn('no draw log stored for 1', out)
        self.assertNotIn('drawlog', json.dumps(baseline.read_json(os.path.join(self.scen, 'noise.json'))))

    def test_run_without_its_draw_log(self):
        self.stored(drawlog(), drawlog())
        rc, out = self.check(None)
        self.assertEqual(rc, 1, out)
        self.assertIn('no draw log in the run', out)

    def test_retail_unstable_when_retail_fails_its_own_leave_one_out_the_same_way(self):
        # brief CS2 (c2): one stored run has two draws more of the one state; left out, it fails the others' draw
        # counts. A run far out in the draw counts only is RETAIL-UNSTABLE (exit 4), never SAME, never DIFF
        self.stored(drawlog(4), drawlog(4), drawlog(6))
        loo = baseline.drawlog_stability([os.path.join(self.scen, r, 'shot_one_00010.json')
                                          for r in sorted(os.listdir(self.scen)) if r.startswith('2026')])
        self.assertEqual([(r, w) for r, _, w in loo], [('20261124-000003', {'draws'})])
        rc, out = self.check(drawlog(12))
        self.assertEqual(rc, 4, out)
        self.assertIn('scn: RETAIL-UNSTABLE draw log one', out)
        self.assertIn('leave one out, for draws: 20261124-000003', out)
        self.assertIn('1 RETAIL-UNSTABLE', out)
        rc, out = self.check(drawlog(6))                # within what two stored runs differ by: SAME
        self.assertEqual(rc, 0, out)

    def test_retail_stable_mismatch_stays_diff(self):
        self.stored(drawlog(4), drawlog(4), drawlog(4))
        self.assertEqual(baseline.drawlog_stability(
            [os.path.join(self.scen, r, 'shot_one_00010.json') for r in sorted(os.listdir(self.scen))
             if r.startswith('2026')]), [])
        rc, out = self.check(drawlog(12))
        self.assertEqual(rc, 1, out)
        self.assertIn('scn: DIFF draw log one', out)
        self.assertNotIn('RETAIL-UNSTABLE', out)

    def test_unstable_elsewhere_does_not_cover_another_difference(self):
        # retail's third run differs in FOGCOLOR (left out it fails), the run in CULLMODE, which retail agrees on
        self.stored(drawlog(fog=0), drawlog(fog=0), drawlog(fog=7))
        rc, out = self.check(drawlog(cull=3))
        self.assertEqual(rc, 1, out)
        self.assertIn('scn: DIFF draw log one', out)
        self.assertIn('the stored runs, leave one out, differ only in draws, states:rs.FOGCOLOR)', out)
        self.assertFalse(baseline.drawlog_unstable(set(), [('r', '', {'draws'})]))
        self.assertTrue(baseline.drawlog_unstable({'draws'}, [('r', '', {'draws', 'cams'})]))
        self.assertFalse(baseline.drawlog_unstable({'draws', 'textures'}, [('r', '', {'draws'})]))


LOOT = ['SET_TARGET 12', 'LOOT_HULK_PERMISSION 8']


def loot(takes, size=20):
    return LOOT + [f'INVENTORY_MOVE {size}'] * takes + ['OPEN_INTERFACE 6']


class Packets(unittest.TestCase):
    """Brief CS3 (c9): a packet difference of the kind the stored retail runs differ in among themselves, leave one
    out (sp_combat's random loot: the number of INVENTORY_MOVE takes), is RETAIL-UNSTABLE; any other is DIFF."""

    def test_reasons(self):
        self.assertEqual(baseline.packet_reasons(loot(3), loot(3)), set())
        self.assertEqual(baseline.packet_reasons(loot(3), loot(5)), {'count:INVENTORY_MOVE'})
        self.assertEqual(baseline.packet_reasons(loot(3), loot(3, 24)), {'size:INVENTORY_MOVE'})
        swapped = [LOOT[1], LOOT[0]] + loot(3)[2:]
        self.assertEqual(baseline.packet_reasons(swapped, loot(3)), {'order'})
        # a count difference does not hide another packet's order
        self.assertEqual(baseline.packet_reasons([LOOT[1], LOOT[0]] + loot(2)[2:], loot(3)),
                         {'count:INVENTORY_MOVE', 'order'})
        self.assertEqual(baseline.packet_reasons(loot(3), loot(3)[:-1]), {'count:OPEN_INTERFACE'})

    def test_stability(self):
        stored = [('r1', loot(3)), ('r2', loot(3)), ('r3', loot(5))]
        self.assertEqual(baseline.packet_stability(stored), [('r3', {'count:INVENTORY_MOVE'}, 'r1')])
        self.assertEqual(baseline.packet_stability([('r1', loot(3)), ('r2', loot(3))]), [])
        self.assertEqual(baseline.packet_stability([('r1', loot(3))]), [])

    def test_verdict(self):
        stored = [('r1', loot(3)), ('r2', loot(4)), ('r3', loot(5))]
        self.assertIs(baseline.packet_verdict(loot(4), stored)[0], True)
        ok, text = baseline.packet_verdict(loot(7), stored)        # more takes: the kind retail varies in
        self.assertEqual(ok, baseline.RETAIL_UNSTABLE, text)
        self.assertIn('INVENTORY_MOVE x7 (x5)', text)
        self.assertIn('3 of the 3 stored runs, leave one out', text)
        ok, text = baseline.packet_verdict(loot(4, 24), stored)    # another size: retail agrees on it
        self.assertIs(ok, False, text)
        self.assertIn('INVENTORY_MOVE sizes 24 (20)', text)
        self.assertIn('differ only in count:INVENTORY_MOVE', text)
        ok, text = baseline.packet_verdict(loot(7) + ['CLIENT_CHAT 30'], stored)  # a packet retail never sends
        self.assertIs(ok, False, text)
        self.assertIn('CLIENT_CHAT x1 (x0)', text)
        stable = [('r1', loot(3)), ('r2', loot(3)), ('r3', loot(3))]
        ok, text = baseline.packet_verdict(loot(4), stable)         # retail always took three
        self.assertIs(ok, False, text)
        self.assertIn('the stored runs agree', text)
        self.assertIs(baseline.packet_verdict(loot(4), [('r1', loot(3))])[0], False)

    def test_verify_run(self):
        with tempfile.TemporaryDirectory() as tmp:
            scen = os.path.join(tmp, 'runs', 'retail-00000000', 'box-64x64', 'scn')
            for k, n in enumerate((3, 3, 5)):
                make_run(scen, f'20261124-00000{k + 1}', packets=loot(n))
            baseline.write_noise(scen)

            def check(pk, name):
                run = make_run(os.path.join(tmp, 'other'), name, packets=pk)
                out = io.StringIO()
                with contextlib.redirect_stdout(out):
                    rc = baseline.main(['verify', '--store', tmp, '--run', run])
                return rc, out.getvalue()
            rc, out = check(loot(6), 'a')
            self.assertEqual(rc, 4, out)
            self.assertIn('scn: RETAIL-UNSTABLE packets 9, stored 6/6/8: against 20261124-000003 only in INVENTORY_MOVE x6 (x5)', out)
            rc, out = check(loot(6) + ['CLIENT_CHAT 30'], 'b')
            self.assertEqual(rc, 1, out)
            self.assertIn('scn: DIFF packets 10', out)
            rc, out = check(loot(5), 'c')
            self.assertEqual(rc, 0, out)


def make_set_run(sdir, run, order=('a', 'b'), box=None, packets=None, stored=True, logs=None):
    """A run of a set: one session folder holding every scenario's result, shots and draw logs; stored=True adds the
    store's manifest (set, order, scenarios) and packets.json."""
    d = os.path.join(sdir, run)
    os.makedirs(d)
    lines, scen = [], []
    for k, n in enumerate(order):
        f = f'shot_{n}1_{10 + k:05d}.png'
        png(os.path.join(d, f), 64, 64, 10, box if n == 'b' else None)
        lines += [f'00:00:0{k} scenario {n}', f'00:00:0{k}   shot {n}1 taken']
        with open(os.path.join(d, f'result-{n}.json'), 'w') as fh:
            json.dump({'name': n, 'passed': True, 'expected_fail': None}, fh)
        e = {'name': n, 'verdict': 'PASS', 'shots': [[f'{n}1', f]], 'drawlogs': []}
        if logs is not None:
            baseline.write_json(os.path.join(d, f[:-4] + '.json'), logs)
            e['drawlogs'] = [[f'{n}1', f[:-4] + '.json']]
        scen.append(e)
    with open(os.path.join(d, 'e2e.log'), 'w') as fh:
        fh.write('\n'.join(lines) + '\n')
    baseline.write_json(os.path.join(d, 'packets.json'), packets or {n: [f'{n.upper()} 4'] for n in order})
    if stored:
        files = {f: {'sha256': baseline.sha256_file(os.path.join(d, f)), 'bytes': os.path.getsize(os.path.join(d, f))}
                 for f in sorted(os.listdir(d))}
        baseline.write_json(os.path.join(d, 'manifest.json'),
                            {'schema': 1, 'scenario': 'default', 'set': 'default', 'verdict': 'PASS',
                             'order': list(order), 'scenarios': scen, 'client': {'name': 'retail-00000000'},
                             'env': 'box-64x64', 'files': files})
    return d


class Sets(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.store = self.tmp.name
        base = os.path.join(self.store, 'runs', 'retail-00000000', 'box-64x64')
        self.sdir = os.path.join(base, 'default')
        for k in range(3):
            make_set_run(self.sdir, f'20261125-00000{k}', box=(0, 0, 4 * k, 4), logs=drawlog())
        self.nz = baseline.write_noise(self.sdir)
        make_run(os.path.join(base, 'a'), '20261124-000001', scenario='a')     # a scenario stored alone too
        baseline.write_noise(os.path.join(base, 'a'))

    def tearDown(self):
        self.tmp.cleanup()

    def run_verify(self, run, *extra):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc = baseline.main(['verify', '--store', self.store, '--run', run] + list(extra))
        return rc, out.getvalue()

    def test_noise_per_set_by_scenario_then_shot(self):
        self.assertEqual(self.nz['set'], 'default')
        self.assertEqual(sorted(self.nz['scenarios']), ['a', 'b'])
        self.assertEqual(self.nz['scenarios']['b']['shots']['b1']['max_differ'], 2)
        self.assertTrue(self.nz['scenarios']['a']['shots']['a1']['drawlog']['identical'])
        self.assertTrue(self.nz['order_equal'])
        self.assertIn('set of 2 scenarios', baseline.noise_summary(self.nz))
        rc, out = io.StringIO(), None
        with contextlib.redirect_stdout(rc):
            code = baseline.main(['verify', '--store', self.store])
        self.assertEqual(code, 0, rc.getvalue())

    def test_verify_run_by_scenario_inside_the_set(self):
        mine = make_set_run(os.path.join(self.tmp.name, 'mine'), 's1', stored=False, logs=drawlog())
        rc, out = self.run_verify(mine)
        self.assertEqual(rc, 0, out)
        self.assertIn('a: SAME', out)
        self.assertIn('b: SAME', out)
        self.assertIn('/default, 3 runs', out)          # the set, not a's own folder: the run has two scenarios
        self.assertNotIn('order differs', out)

    def test_order_differs_is_said(self):
        mine = make_set_run(os.path.join(self.tmp.name, 'mine'), 's2', order=('b', 'a'), stored=False, logs=drawlog())
        rc, out = self.run_verify(mine)
        self.assertEqual(rc, 0, out)
        self.assertIn('scenario order differs', out)

    def test_order_differs_with_differences_is_not_comparable(self):
        mine = make_set_run(os.path.join(self.tmp.name, 'mine'), 's3', order=('b', 'a'), stored=False,
                            logs=drawlog(cull=3), packets={'a': ['A 4', 'X 1'], 'b': ['B 4']})
        rc, out = self.run_verify(mine)
        self.assertEqual(rc, 3, out)
        self.assertIn('a: NOT COMPARABLE (scenario order differs', out)
        self.assertIn('packets 2, stored 1/1/1', out)
        self.assertIn('0 of 2 scenarios the same as the baseline, 2 not comparable', out)
        with open(os.path.join(mine, 'result-a.json'), 'w') as fh:     # a verdict is not the order's
            json.dump({'name': 'a', 'passed': False, 'expected_fail': None, 'failure': 'x'}, fh)
        rc, out = self.run_verify(mine)
        self.assertEqual(rc, 1, out)
        self.assertIn('a: DIFF verdict FAIL', out)
        same_order = make_set_run(os.path.join(self.tmp.name, 'mine'), 's4', stored=False, logs=drawlog(cull=3))
        rc, out = self.run_verify(same_order)          # the stored order: a difference is a DIFF
        self.assertEqual(rc, 1, out)
        self.assertIn('a: DIFF draw log a1', out)

    def test_runs_of_an_older_scenario_text_are_left_out(self):
        for k, run in enumerate(sorted(os.listdir(self.sdir))):
            mp = os.path.join(self.sdir, run, 'manifest.json')
            if not os.path.isfile(mp):
                continue
            man = baseline.read_json(mp)
            man['scenario_files'] = {'tools/harness/e2e/b.scn': 'old' if k < 2 else 'new',
                                     'tools/harness/e2e/a.scn': 'same'}
            baseline.write_json(mp, man)
        self.assertEqual([e['run'] for e in baseline.run_entries(self.sdir, 'b')], ['20261125-000002'])
        self.assertEqual(len(baseline.run_entries(self.sdir, 'a')), 3)
        mine = make_set_run(os.path.join(self.tmp.name, 'mine'), 's5', stored=False, logs=drawlog())
        rc, out = self.run_verify(mine)
        self.assertIn('/default, 1 runs; 2 of an older b.scn left out]', out)
        self.assertIn('/default, 3 runs]', out)

    def test_single_scenario_run_prefers_its_own_folder(self):
        mine = make_run(os.path.join(self.tmp.name, 'one'), 'r', scenario='a')
        rc, out = self.run_verify(mine)
        self.assertIn('box-64x64/a, 1 runs', out)
        rc, out = self.run_verify(mine, '--set', 'default')
        self.assertIn('box-64x64/default, 3 runs', out)


def make_session(out, stamp, order, listed, failed=()):
    """An e2e session folder <stamp>-session under an e2e --out folder: make_set_run's, with e2e's head line
    ('session of N: ...; exe ...', the scenarios it was given) and FAILED's results failed."""
    d = make_set_run(out, f'{stamp}-session', order=order, stored=False, logs=drawlog())
    with open(os.path.join(d, 'e2e.log')) as fh:
        body = fh.read()
    with open(os.path.join(d, 'e2e.log'), 'w') as fh:
        fh.write(f'00:00:00 session of {len(listed)}: {", ".join(listed)}; exe C:/x/client.exe, instance t, '
                 f'out {d}\n' + body)
    for n in failed:
        with open(os.path.join(d, f'result-{n}.json'), 'w') as fh:
            json.dump({'name': n, 'passed': False, 'expected_fail': None, 'failure': 'x'}, fh)
    return d


class OutFolders(unittest.TestCase):
    """verify --run with e2e's --out folder (a run continued in a fresh session after a failure: a session folder
    each) and the set's scenarios a run leaves out (2026-11-25: the station set, 5 of 9 compared, 4 unnamed)."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.store = self.tmp.name
        self.sdir = os.path.join(self.store, 'runs', 'retail-00000000', 'box-64x64', 'default')
        for k in range(3):
            make_set_run(self.sdir, f'20261125-00000{k}', order=('a', 'b', 'c'), logs=drawlog())
        baseline.write_noise(self.sdir)
        self.out = os.path.join(self.tmp.name, 'out')
        make_session(self.out, '20261125-010000', ('a', 'b'), ('a', 'b', 'c'), failed=('b',))
        make_session(self.out, '20261125-010500', ('c',), ('c',))
        os.makedirs(os.path.join(self.out, '20261125-010000-db-snapshot'))     # e2e --db-snapshot's: no e2e.log

    def tearDown(self):
        self.tmp.cleanup()

    def run_verify(self, run, *extra):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc = baseline.main(['verify'] + list(extra) + ['--store', self.store, '--run', run, '--set', 'default',
                                                           '--allow-stale'])
        return rc, out.getvalue()

    def test_out_folder_compares_every_session(self):
        rc, out = self.run_verify(self.out)
        self.assertIn('an e2e --out folder: 2 session(s)', out)
        self.assertIn('a: SAME', out)
        self.assertIn('b: DIFF verdict FAIL', out)
        self.assertIn('c: SAME', out)
        self.assertIn('run after the login (a fresh session), stored after b', out)
        self.assertIn('2 of 3 scenarios the same', out)
        self.assertNotIn('SKIP', out)

    def test_an_earlier_run_in_the_same_out_folder_is_left_out(self):
        make_session(self.out, '20261124-230000', ('a', 'b', 'c'), ('a', 'b', 'c'))
        rc, out = self.run_verify(self.out)
        self.assertIn('1 older session folder(s) of an earlier run left out (20261124-230000-session', out)
        self.assertIn('2 of 3 scenarios the same', out)

    def test_one_session_names_the_scenarios_it_lacks(self):
        rc, out = self.run_verify(os.path.join(self.out, '20261125-010000-session'))
        self.assertEqual(rc, 1)
        self.assertIn('SKIP: c (1 of the set\'s 3: listed in session 20261125-010000-session, which ended after b '
                      'failed', out)
        self.assertIn('1 of 2 scenarios the same as the baseline; 1 stored scenario(s) of the set not in the run', out)

    def test_never_listed_and_not_named(self):
        rc, out = self.run_verify(os.path.join(self.out, '20261125-010500-session'))
        self.assertIn("SKIP: a, b (2 of the set's 3: never listed in the run's sessions", out)
        rc, out = self.run_verify(self.out, 'a')
        self.assertIn('not compared: b, c (in the run, not named on the command line)', out)
        self.assertEqual(rc, 0, out)


def write(path, data=b'x'):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'wb') as f:
        f.write(data)


class Inputs(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.store = self.tmp.name
        os.makedirs(os.path.join(self.store, 'runs'))
        top = os.path.join(self.store, 'inputs')
        write(os.path.join(top, 'captures', 'ea', 'alex3.pcapng'), b'pcap')
        write(os.path.join(top, 'captures', 'ea', 'alex3.json'), b'{}')
        write(os.path.join(top, 'records', '20261115-202650', 'track.txt'), b'track')
        write(os.path.join(top, 'db', 'avatar6.sql'), b'-- six')
        write(os.path.join(top, 'db', 'account1.sql'), b'-- account')
        write(os.path.join(top, 'db', 'content', 'jobs.sql'), b'-- jobs')
        self.top = top

    def tearDown(self):
        self.tmp.cleanup()

    def run_main(self, *argv):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc = baseline.main(list(argv) + ['--store', self.store])
        return rc, out.getvalue()

    def test_manifest_written_and_checked(self):
        rc, out = self.run_main('inputs', '--write')
        self.assertEqual(rc, 0, out)
        man = baseline.read_json(os.path.join(self.store, baseline.INPUTS_MANIFEST))
        cap = man['files']['captures/ea/alex3.pcapng']
        self.assertEqual(cap['replay_ids'], [1])
        self.assertIn('play1', cap['scenarios'])                    # the real scenarios name /play 1
        self.assertIn('sb_buysell', man['files']['records/20261115-202650/track.txt']['scenarios'])
        self.assertEqual(man['sets']['captures']['sha256'],
                         baseline.folder_set_hash(os.path.join(self.top, 'captures')))
        self.assertEqual(man['sets']['db']['files'], 3)
        rc, out = self.run_main('verify')
        self.assertEqual(rc, 0, out)
        self.assertIn('inputs/captures: 2 files', out)

    def test_changed_missing_and_extra_inputs_fail_verify(self):
        self.run_main('inputs', '--write')
        write(os.path.join(self.top, 'captures', 'ea', 'alex3.pcapng'), b'changed')
        os.remove(os.path.join(self.top, 'records', '20261115-202650', 'track.txt'))
        write(os.path.join(self.top, 'db', 'avatar9.sql'), b'-- nine')
        rc, out = self.run_main('verify')
        self.assertEqual(rc, 1)
        self.assertIn('alex3.pcapng: SHA-256 differs', out)
        self.assertIn('track.txt: missing', out)
        self.assertIn('avatar9.sql: not in the manifest', out)
        rc, _ = self.run_main('inputs')
        self.assertEqual(rc, 1)

    def test_tools_folder_checked(self):
        src = os.path.join(self.tmp.name, 'src', 'antreplay.exe')
        write(src, b'MZ replay')
        rc, out = self.run_main('tools', '--add', src)
        self.assertEqual(rc, 0, out)
        man = baseline.read_json(os.path.join(self.store, baseline.TOOLS_MANIFEST))
        self.assertEqual(man['files']['antreplay.exe']['bytes'], 9)
        self.run_main('inputs', '--write')
        rc, out = self.run_main('verify')
        self.assertEqual(rc, 0, out)
        self.assertIn('tools/antprox: 1 files ok', out)
        write(os.path.join(self.store, 'tools', 'antprox', 'antreplay.exe'), b'MZ other')
        rc, out = self.run_main('verify')
        self.assertEqual(rc, 1)
        self.assertIn('antreplay.exe: SHA-256 differs', out)

    def test_db_state_order(self):
        names = [os.path.basename(p) for p in baseline.db_state_files(self.store)]
        self.assertEqual(names, ['account1.sql', 'avatar6.sql'])      # content/ is applied once by hand



def _ev(i, tick, ev, h, snd=None, **kw):
    e = {'i': i, 't': tick - 1000, 'tick': tick, 'f': tick // 10, 'ev': ev, 'h': h}
    if snd:
        e['snd'] = snd
    e.update(kw)
    return e


def _session(hover=1):
    """A two-scenario session: a stream and a 3D sample in each, HOVER 2D hover sounds in the second."""
    ev = [{'i': 0, 't': 0, 'tick': 1000, 'f': 1, 'ev': 'log'},
          _ev(1, 1100, 'open', 'm1', file='music.mp3'), _ev(2, 1100, 'loop', 'm1', 'music.mp3', v=0),
          _ev(3, 1101, 'start', 'm1', 'music.mp3'), _ev(4, 1150, 'volume', 'm1', 'music.mp3', v=90),
          _ev(5, 1190, 'load', 't1', type='door.wav', size=10, hash='aa'), _ev(6, 1191, 'start', 't1', 'aa:10'),
          _ev(7, 1200, 'position', 't1', 'aa:10', x=1)]
    i = 8
    for k in range(hover):
        ev += [_ev(i, 1300 + k, 'start', 's1', 'bb:5'), _ev(i + 1, 1301 + k, 'end', 's1', 'bb:5')]
        i += 2
    ev += [_ev(i, 1400, 'stop', 'm1', 'music.mp3'), _ev(i + 1, 1400, 'close', 'm1', 'music.mp3'),
           _ev(i + 2, 2500, 'shutdown', 'd1')]
    res = [('a', {'tick_end': 1250}), ('b', {'tick_end': 1450})]
    return ev, res


class Audio(unittest.TestCase):
    def summ(self, hover=1):
        ev, res = _session(hover)
        names = baseline.audio_names(ev)
        return {n: baseline.audio_summary(e, names) for n, e in baseline.audio_slices(ev, res).items()}

    def test_slices_by_scenario_ticks(self):
        a = self.summ()
        self.assertEqual([x[0] for x in a['a']['sequence']], ['open', 'start', 'start'])
        self.assertEqual([x[0] for x in a['b']['sequence']], ['start', 'end', 'stop', 'close'])
        self.assertEqual(a['a']['sequence'][1][3], 0)                   # the stream's loop count at its start
        self.assertEqual(a['a']['names'], {'aa:10': 'door.wav'})
        self.assertEqual(a['a']['counts']['music.mp3'], {'loop': 1, 'open': 1, 'start': 1, 'volume': 1})
        self.assertNotIn('volume', [x[0] for x in a['a']['sequence']])  # the per-frame stream is not in it
        self.assertTrue(a['a']['frames'])

    def test_one_scenario_without_ticks_takes_the_whole_log(self):
        ev, _ = _session()
        out = baseline.audio_slices(ev, [('x', {})])
        self.assertEqual(len(out['x']), len(ev) - 1)

    def test_noise_and_verdict(self):
        r1, r2, r3 = self.summ(1)['b'], self.summ(2)['b'], self.summ(4)['b']
        d = tempfile.mkdtemp()
        ents = []
        for k, a in enumerate((r1, r2)):
            path = os.path.join(d, f'a{k}.json')
            baseline.write_json(path, a)
            ents.append({'run': f'r{k}', 'audio': path})
        nz = baseline.audio_noise(ents)
        self.assertTrue(nz['strict_identical'])
        self.assertFalse(nz['loose_equal'])
        self.assertEqual(nz['loose']['start s bb:5'], [1, 2])
        self.assertTrue(baseline.audio_verdict(r2, [r1, r2], nz)[0])
        ok, text = baseline.audio_verdict(r3, [r1, r2], nz)              # four hovers: out of the 1-2 range
        self.assertFalse(ok)
        self.assertIn('2D', text)
        other = json.loads(json.dumps(r1))
        other['sequence'][-1][2] = 'other.mp3'                           # a stream event differs: strict
        ok, text = baseline.audio_verdict(other, [r1, r2], nz)
        self.assertFalse(ok)
        self.assertIn('streams/3D item', text)
        self.assertEqual(baseline.audio_verdict(r1, [], None), (None, 'no audio stored'))
        self.assertIsNone(baseline.audio_noise([{'run': 'r', 'audio': None}]))

    @staticmethod
    def seq(*items):
        """An audio summary of strict items (event, kind, sound): a stream 'm' or a 3D sample 't'."""
        return {'sequence': [[ev, k, snd, 0 if ev == 'start' else None, 1, 1] for ev, k, snd in items],
                'names': {'aa:10': 'door.wav', 'cc:7': 'bell.wav'}, 'events': len(items)}

    def test_retail_unstable_per_stream(self):
        # brief CS2 (c2): the stored runs disagree on the 3D door (stopped in one, ended in the other two): a run that
        # differs there only is RETAIL-UNSTABLE, named by its sound; one differing in the music retail agrees on: DIFF
        music = [('open', 'm', 'music.mp3'), ('start', 'm', 'music.mp3')]
        s1 = self.seq(*music, ('start', 't', 'aa:10'), ('end', 't', 'aa:10'))
        s2 = self.seq(*music, ('start', 't', 'aa:10'), ('stop', 't', 'aa:10'))
        stored = [s1, s1, s2]
        stab = baseline.audio_stability(stored)
        self.assertEqual(stab['shapes'], {'m music.mp3': True, 't door.wav': False})
        self.assertFalse(stab['order'])
        run = self.seq(*music, ('start', 't', 'aa:10'))                 # the door never ends: retail varies there
        ok, text = baseline.audio_verdict(run, stored, None)
        self.assertEqual(ok, baseline.RETAIL_UNSTABLE, text)
        self.assertIn('t door.wav differ, and so do the 3 stored runs', text)
        run = self.seq(('open', 'm', 'music.mp3'), ('start', 't', 'aa:10'), ('end', 't', 'aa:10'))
        ok, text = baseline.audio_verdict(run, stored, None)            # the music never starts: retail agrees on it
        self.assertIs(ok, False)
        self.assertIn('stable in the 3 stored runs and different: m music.mp3', text)
        new = self.seq(*music, ('start', 't', 'aa:10'), ('end', 't', 'aa:10'), ('start', 't', 'cc:7'))
        ok, text = baseline.audio_verdict(new, stored, None)            # a sound no stored run has
        self.assertIs(ok, False)
        self.assertIn('t bell.wav', text)
        self.assertIs(baseline.audio_verdict(run, [s1], None)[0], False)  # one stored run: nothing measured, DIFF

    def test_random_variants_by_family(self):
        # sp_combat 2026-11-25: the space music is a random variant (retail: TerranWander07-2 twice, 04-2 once; a
        # fourth retail run and ours: 01-5): another variant where retail's own runs vary is RETAIL-UNSTABLE; where
        # every stored run played the same variant, another one is DIFF; another shape (a stop) is DIFF either way
        def music(track, end='stop'):
            return self.seq(('open', 'm', track), ('start', 'm', track), (end, 'm', track))
        stored = [music('Wander07-2.mp3'), music('Wander07-2.mp3'), music('Wander04-2.mp3')]
        self.assertEqual(baseline.audio_family(stored[0], 'Wander07-2.mp3'), 'Wander#-#.mp3')
        self.assertEqual(baseline.audio_family(stored[0], 'aa:10'), 'door.wav')
        self.assertEqual(baseline.audio_family(stored[0], 'ee:3'), 'ee:3')            # no name: the sample itself
        ok, text = baseline.audio_verdict(music('Wander01-5.mp3'), stored, None)
        self.assertEqual(ok, baseline.RETAIL_UNSTABLE, text)
        self.assertIn('m Wander#-#.mp3 (Wander01-5.mp3) differ, and so do the 3 stored runs', text)
        ok, text = baseline.audio_verdict(music('Wander01-5.mp3'), stored[:2], None)
        self.assertIs(ok, False)                                       # the stored runs always played 07-2
        self.assertIn('stable in the 2 stored runs and different: m Wander#-#.mp3', text)
        ok, text = baseline.audio_verdict(music('Wander01-5.mp3', 'end'), stored, None)
        self.assertIs(ok, False)                                       # every stored run stops it, the run's ends

    def test_samples_named_from_the_data(self):
        import audiolog
        d = tempfile.mkdtemp()
        body = b'RIFF' + bytes(range(40))
        with open(os.path.join(d, 'sfx_foot_glass_4.wav'), 'wb') as f:
            f.write(body)
        key = f'{audiolog.fnv64(body)}:{len(body)}'
        old, audiolog.NAMES = audiolog.NAMES, os.path.join(d, 'names.json')
        try:
            a = self.seq(('start', 't', key), ('end', 't', key))
            named = baseline.name_samples([a, None], d)
        finally:
            audiolog.NAMES = old
        self.assertEqual(named[0]['names'][key], 'sfx_foot_glass_4.wav')
        self.assertIsNone(named[1])
        self.assertNotIn(key, a['names'])                               # a copy: the stored summary is untouched
        self.assertEqual(baseline.audio_family(named[0], key), 'sfx_foot_glass_#.wav')

    def test_retail_unstable_order_only(self):
        a, b = ('start', 'm', 'music.mp3'), ('start', 't', 'aa:10')
        ok, text = baseline.audio_verdict(self.seq(b, a), [self.seq(a, b), self.seq(a, b), self.seq(b, a, b)], None)
        self.assertEqual(ok, baseline.RETAIL_UNSTABLE, text)           # the third stored run starts the door twice
        ok, text = baseline.audio_verdict(self.seq(b, a), [self.seq(a, b), self.seq(a, b)], None)
        self.assertIs(ok, False)                                       # order only, retail's order stable: DIFF
        self.assertIn('the order differs, the 2 stored runs agree on it', text)
        c = ('start', 't', 'cc:7')
        stored = [self.seq(a, b, c), self.seq(a, b, c), self.seq(b, a, c), self.seq(a, c, b)]
        self.assertFalse(baseline.audio_stability(stored)['order'])
        ok, text = baseline.audio_verdict(self.seq(c, a, b), stored, None)
        self.assertEqual(ok, baseline.RETAIL_UNSTABLE, text)
        self.assertIn('the order differs, and so do the 4 stored runs', text)

    def test_slices_cut_at_the_next_scenarios_first_input(self):
        # brief CS2 (c3): what scenario a's last input started and the client played after a's end (sp_galaxymap's
        # message_1.wav, 1 ms into sp_warp's slice) stays with a when b's first input came later
        ev, res = _session()
        ev.insert(-3, _ev(99, 1260, 'start', 't2', 'cc:7'))             # after a's end (1250), before b's input
        res[1][1]['tick_input'] = 1280
        self.assertEqual(baseline.audio_cut_of(res), 'input')
        self.assertEqual(baseline.audio_cut_of([res[0], ('b', {'tick_end': 1450})]), 'end')
        end = baseline.audio_slices(ev, res, 'end')
        inp = baseline.audio_slices(ev, res, 'input')
        self.assertNotIn(1260, [e['tick'] for e in end['a']])
        self.assertIn(1260, [e['tick'] for e in end['b']])
        self.assertIn(1260, [e['tick'] for e in inp['a']])
        self.assertNotIn(1260, [e['tick'] for e in inp['b']])
        self.assertEqual(len(inp['a']) + len(inp['b']), len(end['a']) + len(end['b']))
        self.assertEqual(baseline.audio_cut_stored([{'cut': 'input'}, {'cut': 'input'}]), 'input')
        self.assertEqual(baseline.audio_cut_stored([{'cut': 'input'}, {}]), 'end')     # older stored runs: 'end'


class LiveSets(unittest.TestCase):
    def test_session_first_and_scripts_hash(self):
        self.assertIn('sb_emote_dock', baseline.SESSION_OWN)
        h = baseline.scripts_hash({'inputs/db/a.sql': '1', 'inputs/db/b.sql': '2'})
        self.assertNotEqual(h, baseline.scripts_hash({'inputs/db/b.sql': '2', 'inputs/db/a.sql': '1'}))

    def test_names_after_the_set_option(self):
        # brief CS3: 'capture --set space sp_flight ...' (the README's form) stopped at argparse's
        # 'unrecognized arguments' before any client started; the names now parse after an option too
        with tempfile.TemporaryDirectory() as tmp:
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                rc = baseline.main(['capture', '--store', tmp, '--set', 'space', 'sp_flight', 'sp_warp'])
            self.assertEqual(rc, 2)
            self.assertIn('is not a checkout of the store', out.getvalue())


class Messages(unittest.TestCase):
    """messages.log stored filtered above MESSAGES_RAW_MAX (a space session's is 49-58 MB of asserts a frame)."""
    LOG = ('00:00:01.000000 INFO  -: starting  main_client.cpp(596)\n'
           '00:00:02.000000 STAT  Render: Frame Rate =  239  frameint.cpp(269) @0,0\n'
           '00:00:02.001000 ERROR G_ASSERT: ASSERTION FAILED: a == b  effect_description.cpp(348)\n'
           '00:00:02.001000 ERROR G_ASSERT: ASSERTION FAILED: c == d  effect_description.cpp(318)\n'
           '00:00:02.005000 STAT  Render: Frame Rate =  241  frameint.cpp(269) @0,0\n'
           '00:00:02.006000 ERROR G_ASSERT: ASSERTION FAILED: a == b  effect_description.cpp(348)\n'
           '00:00:02.006000 ERROR G_ASSERT: ASSERTION FAILED: c == d  effect_description.cpp(318)\n'
           '00:00:02.009000 ERROR G_ASSERT: ASSERTION FAILED: a == b  effect_description.cpp(348)\n'
           '00:00:03.000000 INFO  input: ( 100, 45, 120001) => ( 45 0) : ------ w32kbd.cpp(270)\n'
           '00:00:03.001000 ERROR G_ASSERT: ASSERTION FAILED: a == b  effect_description.cpp(348)\n')

    def setUp(self):
        self.d = tempfile.mkdtemp()
        self.src = os.path.join(self.d, 'messages.log')
        write(self.src, self.LOG.encode())

    def test_filter_drops_frame_rate_and_folds_repeats(self):
        dst = os.path.join(self.d, 'out.log')
        info = baseline.filter_messages(self.src, dst)
        lines = open(dst, encoding='utf-8').read().splitlines()
        self.assertEqual(info['lines'], 10)
        self.assertEqual(info['dropped'], 2)
        self.assertEqual(info['folded'], 3)
        self.assertFalse(info['cut'])
        self.assertEqual(info['sha256'], baseline.sha256_file(self.src))   # the full log's hash, not the copy's
        self.assertEqual(info['stored_bytes'], os.path.getsize(dst))
        self.assertFalse(any('Frame Rate' in ln for ln in lines))
        self.assertEqual(lines[3], '00:00:02.009000 [repeated 2 x] ERROR G_ASSERT: ASSERTION FAILED: a == b  '
                                   'effect_description.cpp(348)')
        self.assertEqual(lines[4], '00:00:02.006000 [repeated 1 x] ERROR G_ASSERT: ASSERTION FAILED: c == d  '
                                   'effect_description.cpp(318)')
        self.assertTrue(lines[5].endswith('w32kbd.cpp(270)'))
        self.assertTrue(lines[6].startswith('00:00:03.001000 ERROR G_ASSERT'))   # after another line: kept again
        self.assertEqual(len(lines), 7)

    def test_cut_at_the_limit(self):
        dst = os.path.join(self.d, 'out.log')
        info = baseline.filter_messages(self.src, dst, limit=120)
        text = open(dst, encoding='utf-8').read()
        self.assertTrue(info['cut'])
        self.assertTrue(text.splitlines()[-1].startswith('[cut: '))
        self.assertEqual(len(text.splitlines()), 2)                      # the one line that fits, then the cut

    def test_store_keeps_a_small_log_whole(self):
        dest = os.path.join(self.d, 'dest')
        os.makedirs(dest)
        m = baseline.store_messages(self.d, dest)
        self.assertEqual(m['kept'], 'full')
        self.assertEqual(open(os.path.join(dest, 'messages.log'), 'rb').read(), self.LOG.encode())
        old = baseline.MESSAGES_RAW_MAX
        baseline.MESSAGES_RAW_MAX = 10
        try:
            m = baseline.store_messages(self.d, dest)
        finally:
            baseline.MESSAGES_RAW_MAX = old
        self.assertEqual(m['kept'], 'filtered')
        self.assertLess(m['stored_bytes'], m['bytes'])
        self.assertIsNone(baseline.store_messages(dest + '-none', dest))


def fake_pe(path, stamp):
    """A file with an MZ header, e_lfanew 0x80 and a PE signature whose COFF TimeDateStamp is STAMP."""
    import struct
    data = bytearray(0x100)
    data[:2] = b'MZ'
    struct.pack_into('<I', data, 0x3C, 0x80)
    data[0x80:0x84] = b'PE\0\0'
    struct.pack_into('<I', data, 0x88, stamp)
    write(path, bytes(data))


class StaleGuard(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.run = os.path.join(self.tmp.name, 'run')
        self.exe = os.path.join(self.tmp.name, 'client.exe')
        fake_pe(self.exe, 1_000_000)

    def tearDown(self):
        self.tmp.cleanup()

    def log(self, retail=False):
        write(os.path.join(self.run, 'e2e.log'),
              (f'23:16:59 session of 2: a, b; exe {self.exe}, instance bl, out {self.run}\n'
               f'23:17:03 antprox pid 1, client pid 2' + (' (the retail client)' if retail else '') + '\n').encode())

    def guard(self, times, allow=False, retail=False):
        self.log(retail)
        exe, ret = baseline.run_exe([self.run])
        lines = []
        ok = baseline.build_guard(exe, ret, allow, times=times, out=lines.append)
        return ok, lines

    def test_pe_stamp_read(self):
        self.assertEqual(baseline.pe_link_time(self.exe), (1_000_000, 'pe'))
        plain = os.path.join(self.tmp.name, 'plain.bin')
        write(plain, b'not a pe')
        self.assertEqual(baseline.pe_link_time(plain)[1], 'mtime')

    def test_exe_from_log(self):
        self.log()
        self.assertEqual(baseline.run_exe([self.run]), (self.exe, False))

    def test_stale_fails_first_line(self):
        ok, lines = self.guard([1_000_300, 1_000_200, 999_000])
        self.assertFalse(ok)
        self.assertTrue(lines[0].startswith('STALE BUILD: exe linked '))
        self.assertIn('(2 commits since)', lines[0])
        self.assertIn('sha256 ', lines[1])

    def test_allow_stale_still_prints(self):
        ok, lines = self.guard([1_000_300], allow=True)
        self.assertTrue(ok)
        self.assertTrue(lines[0].startswith('STALE BUILD:'))

    def test_fresh_passes(self):
        ok, lines = self.guard([999_000])
        self.assertTrue(ok)
        self.assertFalse(any(x.startswith('STALE') for x in lines))
        self.assertIn('linked', lines[0])

    def test_retail_exempt(self):
        ok, lines = self.guard([2_000_000], retail=True)
        self.assertTrue(ok)
        self.assertIn('(retail)', lines[0])

    def test_missing_exe_fails_closed(self):
        os.remove(self.exe)
        ok, lines = self.guard([1])
        self.assertFalse(ok)
        self.assertIn('not found', lines[0])


if __name__ == '__main__':
    unittest.main()
