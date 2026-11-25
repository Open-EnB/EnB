"""tools/harness/audiolog.py on synthetic audio logs (test-plan C2)."""
import json
import os
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'harness'))
import audiolog  # noqa: E402


def ev(i, t, name, **kw):
    e = {'i': i, 't': t, 'tick': 1000 + t, 'ev': name}
    e.update(kw)
    return e


def sample_log(shift=0, vol=100, x=1.0):
    return [ev(0, 0, 'log', version=1, real='mss32r.dll', exe='client.exe', frames=0, missing=0),
            ev(1, 5, 'driver_open', api='waveOut', h='d1', device=-1, rate=22050, ch=2, bits=16, ret=0),
            ev(2, 9, 'alloc', h='s1', driver='d1'),
            ev(3, 10, 'load', h='s1', type='.wav', size=4, hash='00000000000000aa', block=-1, ret=1),
            ev(4, 11 + shift, 'volume', h='s1', snd='00000000000000aa:4', v=vol),
            ev(5, 12 + shift, 'start', h='s1', snd='00000000000000aa:4'),
            ev(6, 20 + shift, 'open', h='m1', driver='d1', sample='-', file='music/theme.mp3', mem=0),
            ev(7, 21 + shift, 'start', h='m1', snd='music/theme.mp3'),
            ev(8, 30 + shift, 'position', h='t1', snd='x:1', x=x, y=2.0, z=3.0),
            ev(9, 40 + shift, 'stop', h='s1', snd='00000000000000aa:4', pos_ms=30 + shift, len_ms=100),
            ev(10, 50 + shift, 'close', h='m1', snd='music/theme.mp3', pos_ms=29 + shift, len_ms=99999)]


class AudioLogTest(unittest.TestCase):
    def test_fnv64_matches_the_c_constants(self):
        self.assertEqual(audiolog.fnv64(b''), 'cbf29ce484222325')
        self.assertEqual(audiolog.fnv64(b'a'), 'af63dc4c8601ec8c')

    def test_sequence_strips_timing_and_names_sounds(self):
        seq = audiolog.sequence(sample_log(), {'00000000000000aa:4': 'click.wav'})
        self.assertEqual([s[:3] for s in seq],
                         [('start', 's', 'click.wav'), ('start', 'm', 'music/theme.mp3'),
                          ('stop', 's', 'click.wav'), ('close', 'm', 'music/theme.mp3')])

    def test_diff_same_sequence_despite_timing(self):
        self.assertIsNone(audiolog.diff(sample_log(), sample_log(shift=7)))
        self.assertIsNotNone(audiolog.diff(sample_log(), sample_log(shift=7), time_tol=5))
        self.assertIsNone(audiolog.diff(sample_log(), sample_log(shift=7), time_tol=7))

    def test_diff_reports_the_first_differing_item(self):
        b = sample_log()
        b[7]['snd'] = 'music/other.mp3'
        k, x, y, why = audiolog.diff(sample_log(), b)
        self.assertEqual(k, 1)
        self.assertEqual(x[2], 'music/theme.mp3')
        self.assertEqual(y[2], 'music/other.mp3')
        k, x, y, why = audiolog.diff(sample_log(), sample_log()[:-1])
        self.assertEqual((k, y), (3, None))

    def test_diff_all_fields_with_float_tolerance(self):
        self.assertIsNone(audiolog.diff(sample_log(), sample_log(shift=3), all_fields=True))
        r = audiolog.diff(sample_log(), sample_log(vol=90), all_fields=True)
        self.assertIn('v:', r[3])
        self.assertIsNotNone(audiolog.diff(sample_log(), sample_log(x=1.01), all_fields=True))
        self.assertIsNone(audiolog.diff(sample_log(), sample_log(x=1.01), all_fields=True, float_tol=0.02))

    def test_summary_counts_kinds_and_sounds(self):
        kinds, sounds = audiolog.summary(sample_log(), {'00000000000000aa:4': 'click.wav'})
        self.assertEqual(kinds['start'], 2)
        self.assertEqual(sounds['00000000000000aa:4']['name'], 'click.wav')
        self.assertEqual(sounds['00000000000000aa:4']['starts'], 1)
        self.assertEqual(sounds['music/theme.mp3']['loads'], 1)

    def test_load_and_name_from_a_mix(self):
        with tempfile.TemporaryDirectory() as d:
            body = b'RIFF\x04\x00\x00\x00WAVE'
            names = b'\x0aclick.wav\x00'
            head_off = 16 + len(body)
            name_off = head_off + 4 + 12
            mix = (b'MIX1' + struct.pack('<III', head_off, name_off, 0) + body + struct.pack('<I', 1) +
                   struct.pack('<III', 0x1234, 16, len(body)) + struct.pack('<I', 1) + names)
            with open(os.path.join(d, 'sound.mix'), 'wb') as f:
                f.write(mix)
            log = os.path.join(d, 'audio.jsonl')
            evs = [ev(1, 0, 'load', h='s1', size=len(body), hash=audiolog.fnv64(body))]
            with open(log, 'w') as f:
                for e in evs:
                    f.write(json.dumps(e) + '\n')
                f.write('{"i":2,"t":1,"ev":"sta')       # a line cut by the kill
            got = audiolog.load(d)
            self.assertEqual(got[-1]['ev'], 'bad-line')
            old = audiolog.NAMES
            audiolog.NAMES = os.path.join(d, 'names.json')
            try:
                found = audiolog.namer(got, d)
            finally:
                audiolog.NAMES = old
            self.assertEqual(found, {f'{audiolog.fnv64(body)}:{len(body)}': 'click.wav'})

    def test_c_forms_from_decorated_names(self):
        self.assertEqual(audiolog.c_form('_AIL_set_3D_position@16'), ('__stdcall', 'AIL_set_3D_position', 16))
        self.assertEqual(audiolog.c_form('@stream_background@0'), ('__fastcall', 'stream_background', 0))
        self.assertEqual(audiolog.c_form('AIL_sprintf'), ('__cdecl', 'AIL_sprintf', 0))


if __name__ == '__main__':
    unittest.main()
