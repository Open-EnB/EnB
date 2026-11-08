"""Transition shim (docs/pipeline-plan.md 3.3): importer.py was split into

    libdefs.py   the libraries, flags, include paths, dest(), lib_of, NOT_LINKED, APP_LIBS
    ledger.py    load / save the sharded ledger (docs/data/imports), RECONSTRUCTED (computed from the
                 shards' descriptions), load_state / save_state, export / split
    verify.py    `python verify.py [--lib L] [--pool K] ...`: compile, byte-match, record
    report.py    `python report.py`: docs/imports.md
    vc6lock.py   the importer lock, the agent and its scratch folder WORK, pool_lease

`import importer` keeps working: every attribute resolves to the module that now holds it (looked up
at each access, so importer.WORK follows set_agent). `python importer.py verify|report ...` forwards
to verify.py / report.py with a deprecation note on stderr.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
if HERE not in sys.path:
    sys.path.insert(0, HERE)
import libdefs  # noqa: E402
import vc6lock  # noqa: E402
import ledger  # noqa: E402
import verify as _verify  # noqa: E402
import report as _report  # noqa: E402

_MODULES = (vc6lock, libdefs, ledger, _verify, _report)


def __getattr__(name):
    for m in _MODULES:
        if name in vars(m):
            return getattr(m, name)
    raise AttributeError(f"module 'importer' has no attribute {name!r}")


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ('verify', 'report'):
        raise SystemExit('usage: importer.py verify|report [options] (now tools/vc6/verify.py and report.py)')
    cmd, rest = sys.argv[1], sys.argv[2:]
    print(f'importer.py {cmd} is deprecated: use tools/vc6/{cmd}.py', file=sys.stderr, flush=True)
    if cmd == 'verify':
        _verify.main(rest)
    else:
        _report.main(rest)


if __name__ == '__main__':
    main()
