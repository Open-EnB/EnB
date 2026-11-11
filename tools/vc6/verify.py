"""Compile the repository's libraries and byte-match them against client.exe; record the pairs in
the ledger (split from importer.py; same options as `importer.py verify`).

    python verify.py                          every library
                     [--lib L[,L...] ...]     only these libraries (an unknown name exits)
                     [--pool K [--holder-pid P]]  on K VMs leased from pool.py (agent importer-<pid>),
                                              released afterwards
                     [--lock-wait S] [--agent NAME]
"""
import argparse
import collections
import contextlib
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
if HERE not in sys.path:
    sys.path.insert(0, HERE)
sys.path.insert(1, os.path.join(REPO, 'tools', 're'))
import vc6  # noqa: E402
import vc6lock  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from objmatch import match_objects  # noqa: E402
from libdefs import APP_LIBS, dest, flags_for, flags_of, key_of, late_sync, lib_of, tree_includes  # noqa: E402
from ledger import load_state, save_state  # noqa: E402
from vc6lock import hold_lock, pool_lease, set_agent  # noqa: E402

_img = None


def image():
    global _img
    if _img is None:
        _img = ClientImage()
    return _img



def compile_and_match(paths, tag, ambiguous=None, tiebreak=None, shared=None):
    """Compile files from the repository; {file key: set(addresses)} plus compiler output.
    `ambiguous` (a dict, optional) receives {file key: set(first address)} of the functions found
    at several places in client.exe whose copies are all identical (objmatch `multiple` with
    `identical`; D54's "exact, ambiguous address"). They are never pairs, except those D166's caller
    tie-break resolves (objmatch caller_tiebreak): found like any exact function, and also put in
    `tiebreak` (a dict, optional) as {file key: set(address)}. `shared` (a dict, optional) receives
    {file key: set(address)} of the exact COMDATs two or more of the objects hold at one address
    under one symbol (D183: pairs of each holder, never placement evidence), and, under the key
    'shared_callers', the D166 tie-break pairs whose decision needed such a function, and under the key
    'short_known', the short functions credited at an address the run knows (D188: pairs like any exact
    result; a COMDAT one is also in the file's `shared` list)."""
    hold_lock()
    out = os.path.join(vc6lock.WORK, tag)
    if os.path.isdir(out):
        shutil.rmtree(out)
    app = APP_LIBS.get(lib_of(paths[0])) if paths else None
    if app:
        root = dest(lib_of(paths[0]))
        rc, text = vc6.compile_sources(paths, tree_includes(), defines=app['defines'], out=out,
                                       flags=flags_for(paths), extra_sync=[d for d, _, _ in os.walk(root)] + late_sync(),
                                       source_alias=(REPO, app['file_root']))
    else:
        groups = collections.defaultdict(list)
        for p in paths:
            groups[flags_of(p)].append(p)
        rc, text = 0, ''
        for fl, ps in groups.items():
            r, t = vc6.compile_sources(ps, tree_includes(), out=out, flags=fl, extra_sync=late_sync())
            rc, text = rc or r, text + t
    objs = [os.path.join(out, f) for f in os.listdir(out)] if os.path.isdir(out) else []
    found = collections.defaultdict(set)
    for r in match_objects(image(), objs, app and app['region'], callers=True) if objs else []:
        if r['status'] == 'exact':
            found[key_of(r['object'])].add(r['addresses'][0])
            if r.get('caller_tiebreak') and tiebreak is not None:
                tiebreak.setdefault(key_of(r['object']), set()).add(r['addresses'][0])
            if r.get('shared') and shared is not None:
                shared.setdefault(key_of(r['object']), set()).add(r['addresses'][0])
            if r.get('shared_callers') and shared is not None:
                shared.setdefault(('shared_callers', key_of(r['object'])), set()).add(r['addresses'][0])
            if r.get('short_known') and shared is not None:
                shared.setdefault(('short_known', key_of(r['object'])), set()).add(r['addresses'][0])
        elif r['status'] == 'multiple' and r.get('identical') and ambiguous is not None:
            ambiguous.setdefault(key_of(r['object']), set()).add(r['addresses'][0])
    for p in paths:
        found.setdefault(key_of(p), set())
    return found, text


def verify(state, libs=None):
    """Compile and byte-match the libraries; the libraries are spread over the build VMs."""
    import concurrent.futures as cf
    import queue
    libs = [l for l in (libs or list(state)) if l in state]
    image()                                  # load the client once, before the threads
    free = queue.Queue()
    for h in vc6.hosts():
        free.put(h)

    def one(lib):
        host = free.get()
        try:
            vc6.use_host(host)
            s = state[lib]
            paths = [os.path.join(dest(lib), f['name']) for f in s['files'].values()]
            amb, tie, sha = {}, {}, {}
            found, text = compile_and_match(paths, f'verify-{lib}', amb, tie, sha)
            return lib, found, text, (amb, tie, sha)
        finally:
            free.put(host)

    # biggest first, so the long WW3D2 compile overlaps with the rest
    order = sorted(libs, key=lambda l: -len(state[l]['files']))
    with cf.ThreadPoolExecutor(max(1, len(vc6.hosts()))) as ex:
        results = {lib: (found, text, amb) for lib, found, text, amb in ex.map(one, order)}
    for lib in libs:
        s = state[lib]
        found, text, (amb, tie, sha) = results[lib]
        errors = [l for l in text.splitlines() if ' error ' in l]
        total = set()
        ambiguous = set()
        tied = 0
        shared = set()
        shortk = set()
        short = 0
        for k, f in s['files'].items():
            f['in_tree'] = sorted(found.get(k, set()))
            total |= found.get(k, set())
            # D54: exact, ambiguous address (identical copies); not pairs
            f['ambiguous'] = sorted(amb.get(k, set()))
            ambiguous |= amb.get(k, set())
            # D166: pairs found by their callers (also in in_tree); the key exists only when there are some
            f.pop('tiebreak', None)
            if tie.get(k):
                f['tiebreak'] = sorted(tie[k])
                tied += len(tie[k])
            # D183: pairs at an address other files of the library hold under the same symbol (also in
            # in_tree; in_tree_total counts the address once); the key exists only when there are some
            f.pop('shared', None)
            if sha.get(k):
                f['shared'] = sorted(sha[k])
                shared |= sha[k]
            # D183: D166 tie-break pairs (in `tiebreak`) whose callers include a shared function: labelled
            f.pop('shared_callers', None)
            if sha.get(('shared_callers', k)):
                f['shared_callers'] = sorted(sha[('shared_callers', k)])
            # D188: short functions credited at an address the run knows (also in in_tree)
            f.pop('short_known', None)
            if sha.get(('short_known', k)):
                f['short_known'] = sorted(sha[('short_known', k)])
                shortk |= sha[('short_known', k)]
            if len(f['in_tree']) < f['expected'].get(f['copy'], 0):
                short += 1
        s['in_tree_total'] = len(total)
        s['ambiguous_total'] = len(ambiguous)
        s['compile_errors'] = errors[:50]
        print(f'{lib:11s} in tree: {len(total)} client functions (best copy alone: '
              f'{max(s["totals"].values(), default=0)}), {len(ambiguous)} exact with an ambiguous address, {tied} tie-break by callers, '
              f'{len(shared)} shared addresses, {len(shortk)} short at known addresses; '
              f'{short} files below their copy; {len(errors)} errors')
    return state


def lib_names(values, known):
    """--lib's values as library names: repeated and comma-separated forms both; None when none was given.
    A name the ledger does not hold exits (a typo, or `a,b` before the comma form existed, verified nothing)."""
    names = []
    for v in values or ():
        names += [n.strip() for n in v.split(',') if n.strip() and n.strip() not in names]
    bad = [n for n in names if n not in known]
    if bad:
        raise SystemExit(f'verify: no such library: {", ".join(bad)} (the ledger has: {", ".join(sorted(known))})')
    if values and not names:
        raise SystemExit('verify: --lib names no library')
    return names or None


def main(argv=None):
    ap = argparse.ArgumentParser(description='Compile and byte-match the libraries; record the pairs.')
    ap.add_argument('--lib', action='append', metavar='NAME[,NAME...]',
                    help='limit verify to these libraries (repeatable; commas split; an unknown name exits)')
    ap.add_argument('--lock-wait', type=int, default=vc6lock.LOCK_WAIT,
                    help='seconds to wait for another importer run to finish (default %(default)s)')
    ap.add_argument('--pool', type=int, metavar='K',
                    help='lease K VMs from pool.py for this run (sets ENB_VC6_HOSTS), release after (also on failure)')
    ap.add_argument('--holder-pid', type=int,
                    help="--pool: the lease's long-lived holder pid (default pool.py's: $ENB_HOLDER_PID, $CLAUDE_PID)")
    ap.add_argument('--agent', metavar='NAME',
                    help='agent name: lock owner, ENB_AGENT for children, scratch under .work/NAME/tmp '
                         '(default: the git worktree name, else USERNAME@host)')
    a = ap.parse_args(argv)
    if a.agent:
        set_agent(a.agent)
    # one lease covers the whole run, released on success, failure and interrupt. Lease before taking the
    # importer lock (D234): a lease waiting in pool.py's queue must not hold the lock other verifies need
    # to finish and free their VMs.
    with (pool_lease(a.pool, a.holder_pid) if a.pool else contextlib.nullcontext()):
        hold_lock(a.lock_wait)
        state = load_state()
        libs = lib_names(a.lib, state)
        save_state(verify(state, libs))


if __name__ == '__main__':
    main()
