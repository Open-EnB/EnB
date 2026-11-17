"""Builds the generated VC6 workspace (dsp.py) in a build VM with the IDE's own build: msdev /MAKE (D248).

    python tools/vc6/msdev.py make [--project main|clientpoc|LIB] [--rebuild] [--pool K] [--out DIR] [--deploy]
    python tools/vc6/msdev.py fetch-objs --out DIR       every project's objects, for objcmp.py

`make` generates the workspace (dsp.py), sends the tree to the VM (vc6.sync: every source directory
of every project, the SDK headers, the prebuilt import libraries), opens it through the client's own
path (vc6._alias_setup: E:\\build\\beta\\G is the repository, so __FILE__ is the client's string) and
runs `msdev main.dsw /MAKE "<project> - Win32 client_release"`, which builds the project's
dependencies first. Its output (the .plg build logs and msdev's console) lands in DIR/<project>/.
--project clientpoc is the stand-in build in two passes, as build.py's link_program: a first link,
gen_stubs.py's stand-ins for what it leaves unresolved, a second link with them (/force:unresolved
for the rest).

The outputs stay in the VM's tree (each project's client_release\\ folder) until fetched: the exe
and map into DIR (default build/msdev), the objects with fetch-objs.
"""
import argparse
import os
import shutil
import subprocess
import sys
import tarfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import build as B  # noqa: E402
import dsp  # noqa: E402
import libdefs as L  # noqa: E402
import vc6  # noqa: E402

BS = chr(92)
ALIAS = L.APP_LIBS['app']['file_root']          # e:\build\beta\G
DEFAULT_OUT = os.path.join(REPO, 'build', 'msdev')
PREBUILT = [os.path.join(REPO, 'build', B.MSS32), os.path.join(REPO, 'build', B.AUTHLOGIN), B.BINKW32_LIB] + \
    [os.path.join(B.SDK_LIB, n[4:]) for n in B.LINK_SYS if n.startswith('sdk:')]


def alias_path(path):
    """A repository path as the VM's build sees it (under the alias)."""
    return ALIAS + BS + os.path.relpath(os.path.join(REPO, path), REPO)


def sync_tree(projs):
    """Send every project's source and include directories and its .dsp to the VM, and the
    prebuilt libraries to the same paths there (nothing in local mode: the tree is the VM's own)."""
    if vc6.LOCAL:
        return 0
    dirs = set()
    for p in projs:
        dirs.add(os.path.join(REPO, p.dir))
        dirs.update(os.path.join(REPO, os.path.dirname(s)) for s in p.sources)
        dirs.update(p.includes)
    for lib in L.LATE_INCLUDE_LIBS:
        if os.path.isdir(L.dest(lib)):
            dirs.add(L.dest(lib))
    # headers reached by relative paths (app's subfolders, libs\wwonline): every directory under them
    for top in ('app', 'libs', 'programs', B.STUBS_DIR):
        for d, _, _ in os.walk(os.path.join(REPO, top)):
            if dsp.CONFIG not in d.split(os.sep):
                dirs.add(d)
    n = vc6.sync(sorted(dirs))
    missing = [p for p in PREBUILT if not os.path.exists(p)]
    if missing:
        raise SystemExit('missing prebuilt libraries (mkmss32.py lib; mkauthlogin.py; the SDKs, docs/build-your-own.md): '
                         + ', '.join(missing))
    tag = vc6._tag()
    local = os.path.join(os.environ.get('TEMP', '.'), f'enb_msdev_libs_{tag}.tar')
    with tarfile.open(local, 'w') as tar:
        for p in PREBUILT:
            tar.add(p, arcname=vc6.under_root(p)[1].replace(BS, '/'))
    vc6.ensure_work()
    vc6.scp(local, vc6.remote(vc6.WORK + r'\msdev_libs.tar'))
    os.remove(local)
    vc6.ssh(f'cmd /c tar -xf "{vc6.WORK}\\msdev_libs.tar" -C "{vc6.ROOT}" && del "{vc6.WORK}\\msdev_libs.tar"')
    return n


def run_make(project, rebuild=False, out=DEFAULT_OUT):
    """msdev /MAKE one project (its dependencies first). Returns (rc, console text); writes the console
    output and the projects' .plg logs to out/<project>/."""
    setup, _ = vc6._alias_setup([], REPO, ALIAS)
    ws_dir = alias_path(os.path.dirname(dsp.WORKSPACE))
    ws = os.path.basename(dsp.WORKSPACE)
    log = f'{vc6.WORK}\\msdev_{project}.log'
    flag = ' /REBUILD' if rebuild else ''
    lines = setup + [f'if not exist "{ws_dir}\\{ws}" (echo MSDEV_ALIAS_FAILED & exit /b 1)',
                     f'cd /d "{ws_dir}"',
                     f'msdev {ws} /MAKE "{project} - Win32 {dsp.CONFIG}"{flag} /OUT "{log}" > nul 2>&1',
                     'echo MSDEV_RC=%ERRORLEVEL%']
    r = vc6.run_bat(lines)
    if 'MSDEV_ALIAS_FAILED' in (r.stdout or ''):
        raise SystemExit(f'{ws_dir} does not reach the workspace: the subst {ALIAS[:2]} failed (a drive already '
                         f'has that letter in the VM, a DVD drive for one: remove it; vc6._alias_setup)')
    rc = None
    for l in (r.stdout or '').splitlines():
        if l.startswith('MSDEV_RC='):
            rc = int(l.split('=')[1])
    dst = os.path.join(out, project)
    os.makedirs(dst, exist_ok=True)
    local_log = os.path.join(dst, 'msdev.log')
    vc6.scp(vc6.remote(log), local_log)
    text = open(local_log, encoding='latin1').read()
    return rc, text


def fetch(paths, dst):
    """Copy files from the VM's tree (repository-relative paths) into dst, flat."""
    os.makedirs(dst, exist_ok=True)
    tag = vc6._tag()
    remote_tar = f'{vc6.WORK}\\msdev_fetch_{tag}.tar'
    lst = os.path.join(os.environ.get('TEMP', '.'), f'enb_msdev_fetch_{tag}.txt')
    under = os.path.relpath(REPO, vc6.ROOT)
    # LF lines: Windows' tar -T keeps a CR in each name
    open(lst, 'w', newline='').write('\n'.join(under + BS + p.replace('/', BS) for p in paths) + '\n')
    vc6.scp(lst, vc6.remote(f'{vc6.WORK}\\msdev_fetch_{tag}.txt'))
    os.remove(lst)
    vc6.ssh(f'cmd /c tar -cf "{remote_tar}" -C "{vc6.ROOT}" -T "{vc6.WORK}\\msdev_fetch_{tag}.txt"', check=False)
    local = os.path.join(dst, '_fetch.tar')
    vc6.scp(vc6.remote(remote_tar), local)
    vc6.ssh(f'cmd /c del "{remote_tar}" "{vc6.WORK}\\msdev_fetch_{tag}.txt"', check=False)
    got = []
    with tarfile.open(local) as t:
        for m in t.getmembers():
            if m.isfile():
                m.name = os.path.basename(m.name)
                t.extract(m, dst)
                got.append(m.name)
    os.remove(local)
    return got


def fetch_objs(projs, out):
    """Every project's objects but the stand-in build's into out/<lib>/obj/, build.py's layout (the app's
    folder libraries and main together in out/app/obj), for objcmp.py."""
    res = {}
    for p in projs:
        if p.name == dsp.STANDIN:
            continue
        lib = 'app' if p.dir.replace(BS, '/').startswith('app/') else p.name
        objs = [os.path.join(p.dir, dsp.CONFIG, os.path.splitext(os.path.basename(s))[0] + '.obj') for s in p.sources]
        res.setdefault(lib, []).extend(fetch([o.replace(BS, '/') for o in objs], os.path.join(out, lib, 'obj')))
    return res


def make(project, rebuild=False, out=DEFAULT_OUT):
    projs = dsp.projects()
    dsp.write(projs)
    sync_tree(projs)
    if project != dsp.STANDIN:
        rc, text = run_make(project, rebuild, out)
        if project == 'main':
            fetch([f'{dsp.MAIN_DIR}/{dsp.CONFIG}/client.exe'.replace(BS, '/'),
                   f'{dsp.MAIN_DIR}/{dsp.CONFIG}/main.map'.replace(BS, '/')], os.path.join(out, 'main'))
        return rc, text
    # the stand-in build: build.py link_program's two passes
    dst = os.path.join(out, dsp.STANDIN)
    gen_dir = os.path.join(REPO, 'programs', dsp.STANDIN, 'gen')
    gen = os.path.join(gen_dir, 'poc_gen_stubs.cpp')
    if os.path.exists(gen):
        os.remove(gen)
    exe_rel = f'programs/{dsp.STANDIN}/{dsp.CONFIG}/{dsp.STANDIN}.exe'
    # the first pass must link: msdev keeps the last exe when no input is newer (the stand-ins' file
    # dropped from the project does not count), and that exe already has the last run's stand-ins
    vm_exe = vc6.ROOT + BS + os.path.relpath(REPO, vc6.ROOT) + BS + exe_rel.replace('/', BS)
    vc6.ssh(f'cmd /c if exist "{vm_exe}" del "{vm_exe}"', check=False)
    rc, text = run_make(dsp.STANDIN, rebuild, out)
    if ' error(s)' in text and ' 0 error(s)' not in text:
        if 'error C' in text:
            return rc, text                       # a compile error: no stand-ins can help
        sys.path.insert(0, os.path.join(REPO, 'tests', 'clienttest'))
        import gen_stubs
        import coff_alias
        os.makedirs(gen_dir, exist_ok=True)
        pass1 = os.path.join(dst, 'link_pass1.log')
        shutil.copy(os.path.join(dst, 'msdev.log'), pass1)
        gen_stubs.link_stubs(pass1, gen_dir, poc=True)
        os.replace(os.path.join(gen_dir, 'link_stubs.cpp'), gen)
        objs = []
        pairs = [l.strip().split('=', 1) for l in open(os.path.join(gen_dir, 'link_aliases.txt')) if '=' in l]
        if pairs:
            objs.append(os.path.join(gen_dir, 'poc_aliases.obj'))
            coff_alias.write(objs[0], pairs)
        projs = dsp.projects(standin_gen=gen, force_unresolved=True, extra_objs=objs)
        dsp.write(projs)
        sync_tree(projs)
        rc, text = run_make(dsp.STANDIN, False, out)
    fetch([exe_rel, exe_rel[:-4] + '.map'], dst)
    return rc, text


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', choices=['make', 'fetch-objs'])
    ap.add_argument('--project', default='main')
    ap.add_argument('--rebuild', action='store_true', help='/REBUILD: compile everything again')
    ap.add_argument('--out', default=DEFAULT_OUT)
    ap.add_argument('--pool', type=int, metavar='K', help='lease a VM from pool.py, release after')
    ap.add_argument('--deploy', action='store_true',
                    help='--project clientpoc: copy the exe into ClientPOC\\release as client.exe (build.py link\'s deploy)')
    a = ap.parse_args()
    if a.pool:
        import importer as I
        with I.pool_lease(a.pool, None):
            return run(a)
    return run(a)


def deploy(out=DEFAULT_OUT):
    """Copy the stand-in build's exe and map into its deploy folder (build.py PROGRAMS['clientpoc']'s:
    C:\\EnBDevelopment\\ClientPOC\\release as client.exe), as `build.py link` does for its own link."""
    prog = B.PROGRAMS[dsp.STANDIN]
    src = os.path.join(out, dsp.STANDIN, dsp.STANDIN)
    base = os.path.join(prog['deploy'], prog.get('deploy_name', dsp.STANDIN))
    shutil.copy2(src + '.exe', base + '.exe')
    shutil.copy2(src + '.map', base + '.map')
    print('deployed', base + '.exe')


def run(a):
    if a.command == 'fetch-objs':
        got = fetch_objs(dsp.projects(), a.out)
        print('fetched', sum(len(v) for v in got.values()), 'objects from', len(got), 'projects into', a.out)
        return
    rc, text = make(a.project, a.rebuild, a.out)
    tail = [l for l in text.splitlines() if ' error(s)' in l or l.startswith('---')]
    print('\n'.join(tail[-25:]))
    print(f'msdev rc={rc}; log {os.path.join(a.out, a.project, "msdev.log")}')
    if a.deploy and rc == 0 and a.project == dsp.STANDIN:
        deploy(a.out)
    sys.exit(0 if rc == 0 else 1)


if __name__ == '__main__':
    main()
