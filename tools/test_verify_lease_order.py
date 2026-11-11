"""D234: verify.py --pool leases its VMs before it takes the importer lock.

A lease waiting in pool.py's queue (D232) must not hold the lock that other verifies need
to finish and release their VMs. No VM, no importer: every call is replaced.
"""
import contextlib
import os
import sys
import unittest
from unittest import mock

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'vc6'))
import verify  # noqa: E402


class LeaseOrder(unittest.TestCase):
    def run_main(self, argv):
        calls = []

        @contextlib.contextmanager
        def lease(count, holder_pid=None):
            calls.append('lease')
            yield 'ip'
            calls.append('release')

        with mock.patch.object(verify, 'pool_lease', lease), \
                mock.patch.object(verify, 'hold_lock', lambda *a, **k: calls.append('lock')), \
                mock.patch.object(verify, 'load_state', lambda: calls.append('load') or {}), \
                mock.patch.object(verify, 'lib_names', lambda v, s: []), \
                mock.patch.object(verify, 'verify', lambda s, l: calls.append('verify') or s), \
                mock.patch.object(verify, 'save_state', lambda s: calls.append('save')):
            verify.main(argv)
        return calls

    def test_pool_leases_before_lock(self):
        self.assertEqual(self.run_main(['--pool', '1']),
                         ['lease', 'lock', 'load', 'verify', 'save', 'release'])

    def test_no_pool_still_locks(self):
        self.assertEqual(self.run_main([]), ['lock', 'load', 'verify', 'save'])


if __name__ == '__main__':
    unittest.main()
