"""Builds the tree's libraries into .lib files with VC6 (L1, docs/milestones.md 3).

    python tools/vc6/build.py [--pool K] [--lib X]... [--out DIR] [--allow-dirty]
    python tools/vc6/build.py unresolved [--out DIR] [--report FILE]
    python tools/vc6/build.py link [--program vidtest] [--pool K] [--out DIR]

`build` (the default command) compiles every committed source file of each library (git
ls-files; NOT_LINKED files stay out), not only the ones that match, with the flags, defines,
include paths and __FILE__ alias that `importer.py verify` uses (LIB_FLAGS, APP_LIBS,
tree_includes, imported from importer.py), then runs LIB in the VM. Per library, DIR/<lib>/
(default build/, git-ignored) receives <lib>.lib, obj/*.obj, cl.log (the compiler output),
symbols.txt (`dumpbin /symbols` over the objects), members.txt (`lib /list`) and summary.json
(files compiled, files without an object and their first error, warning count, .lib size,
member count). It also saves the public symbols of the system libraries the link step will use
(DIR/_sys/*.txt, `dumpbin /linkermember:1`); the DirectX 8 SDK's libraries are copied to the VM
for that, since the VM has only VC98's.

Like verify, it compiles the working tree's files, so it refuses to run while a library has
uncommitted changes (`git status`), unless --allow-dirty. It never assembles anything.

`unresolved` reads DIR and lists the external symbols the objects reference that none of the
libraries and none of the system libraries define, grouped by library and by likely cause.

`link` (L2) compiles a program (PROGRAMS: programs/<name>/*.cpp) and the stubs (stubs/*.cpp,
into stubs.lib, D44) in the VM and links them against DIR/<lib>/<lib>.lib, DIR/mss32/mss32.lib
(tools/vc6/mkmss32.py, D51) and the system libraries (LINK_SYS: the CRT, Win32, DirectX 8,
vfw32, comsupp, ddraw: D50), into DIR/programs/<name>/ (exe, map, cl.log, link.log, summary.json).

Checked build (M2 gate (d)): ENB_CHECKED=1 python tools/vc6/build.py --out build/chk, then link
with the same variable and --out, compiles everything with tools/vc6/checked/enb_checked.h forced
in, so WWASSERT and WWDEBUG_SAY/WARNING/ERROR reach WWDebug's message handler (copy build/mss32 to
build/chk first). Never used for matching.

VMs: --pool K leases K VMs from pool.py (agent build-<pid>) and releases them afterwards, as
`importer.py verify --pool K`; without it, vc6.hosts() (ENB_VC6_HOSTS) is used.
"""
import argparse
import collections
import concurrent.futures as cf
import json
import os
import queue
import re
import subprocess
import sys
import tarfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import importer as I  # noqa: E402
import vc6  # noqa: E402

ORDER = I.LIBS + list(I.APP_LIBS)
BS = chr(92)
SDK_LIB = r'C:\EnBDevelopment\SDK\directx8\lib'
VC_LIB = r'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Lib'
# The system libraries L2's link will use (the brief's list: the CRT, DirectX 8, Win32 import
# libraries), and a second tier of other system libraries (a symbol found only there means the
# link needs that library too, not a missing function).
SYS_LIBS = ['LIBCMT.LIB', 'LIBCPMT.LIB', 'OLDNAMES.LIB', 'KERNEL32.LIB', 'USER32.LIB', 'GDI32.LIB',
            'ADVAPI32.LIB', 'OLE32.LIB', 'OLEAUT32.LIB', 'UUID.LIB', 'WINMM.LIB', 'WS2_32.LIB',
            'sdk:d3d8.lib', 'sdk:d3dx8.lib', 'sdk:dxguid.lib',
            'VFW32.LIB', 'COMSUPP.LIB', 'sdk:ddraw.lib']     # D50 (L2)
SYS_LIBS_EXTRA = ['IMM32.LIB', 'VERSION.LIB', 'SHELL32.LIB', 'COMCTL32.LIB', 'COMDLG32.LIB',
                  'WSOCK32.LIB', 'MSACM32.LIB', 'sdk:dsound.lib',
                  'sdk:dinput8.lib', 'sdk:dxerr8.lib', 'sdk:d3dx.lib']
# Miles (D51): mss32.lib generated from the game's Mss32.dll by tools/vc6/mkmss32.py into
# DIR/mss32/ (its linkermember.txt lists the publics for `unresolved`).
MSS32 = os.path.join('mss32', 'mss32.lib')


def git(*args):
    return subprocess.run(['git', '-C', REPO] + list(args), capture_output=True, text=True,
                          check=True).stdout


def sources(lib):
    """The committed .cpp/.c files of a library (paths under the repository), NOT_LINKED dropped."""
    out = []
    for f in git('ls-files', '--', lib + '/').splitlines():
        if f.lower().endswith(('.cpp', '.c')) and not I.not_linked(lib, os.path.basename(f)):
            if lib in I.APP_LIBS or f.count('/') == 1:
                out.append(os.path.join(REPO, f.replace('/', os.sep)))
    return sorted(out)


def dirty(libs):
    text = git('status', '--porcelain', '--', *[l + '/' for l in libs])
    return [l for l in text.splitlines() if l.strip()]


# A checked build (M2 gate (d)): with ENB_CHECKED=1 every compile (libraries, program, stubs) also
# force-includes tools/vc6/checked/enb_checked.h with DEBUG_CRASHING and DEBUG_LOGGING, so WWASSERT,
# WWDEBUG_SAY/WARNING/ERROR report through WWDebug's message handler instead of compiling to nothing
# as in the client. Build it into its own folder (--out build_chk; copy mss32/ there); never for matching.
CHECKED_DIR = os.path.join(REPO, 'tools', 'vc6', 'checked')


def checked_flags():
    """(directories to sync, extra cl options) for a checked build, or ([], [])."""
    if os.environ.get('ENB_CHECKED') != '1':
        return [], []
    return [CHECKED_DIR], [f'/I "{vc6.under_root(CHECKED_DIR)[0]}"', '/FI"enb_checked.h"',
                           '/D "DEBUG_CRASHING"', '/D "DEBUG_LOGGING"']


def compile_lines(lib, paths):
    """Batch lines that compile `paths` the way importer.compile_and_match does, into objdir."""
    app = I.APP_LIBS.get(lib)
    incs = [vc6.under_root(i) for i in I.tree_includes()]
    srcs = [vc6.under_root(p) for p in paths]
    dirs = [os.path.dirname(f) for f, _ in srcs] + [f for f, _ in incs] + checked_flags()[0]
    setup = []
    if app:
        dirs += [d for d, _, _ in os.walk(I.dest(lib))]
        setup, srcs = vc6._alias_setup(srcs, REPO, app['file_root'])
        defines = app['defines']
    else:
        defines = vc6.DEFAULT_DEFINES
    flags = I.flags_for(paths)
    cmd = [flags] + [f'/I "{f}"' for f, _ in incs] + [f'/D "{d}"' for d in defines] + checked_flags()[1]
    return dirs, setup, cmd, srcs


def build_lib(lib, out):
    paths = sources(lib)
    dirs, setup, cmd, srcs = compile_lines(lib, paths)
    vc6.sync(dirs)
    work = f'{vc6.WORK}\\build_{lib}'
    rsp = f'{work}\\cl.rsp'
    # the command line goes into a response file (ww3d2's is long); cl expands it as typed
    rsp_text = '\r\n'.join(cmd + [f'/Fo"{work}\\obj\\\\"'] + [f'"{f}"' for f, _ in srcs]) + '\r\n'
    local_rsp = os.path.join(os.environ.get('TEMP', '.'), f'enb_build_{lib}_{vc6._tag()}.rsp')
    open(local_rsp, 'w', newline='').write(rsp_text)
    vc6.run_bat([f'if exist "{work}" rmdir /s /q "{work}"', f'mkdir "{work}\\obj"'])
    vc6.scp(local_rsp, vc6.remote(rsp))
    p = vc6.run_bat(list(setup) + [
        f'cl @"{rsp}" > "{work}\\cl.log" 2>&1',
        f'cd /d "{work}\\obj"',
        f'dir /b *.obj > ..\\objs.rsp 2>nul',
        f'lib /nologo /out:..\\{lib}.lib @..\\objs.rsp > ..\\lib.log 2>&1',
        f'cd /d "{work}"',
        f'lib /nologo /list {lib}.lib > members.txt 2>&1',
        f'dumpbin /nologo /symbols obj\\*.obj > symbols.txt 2>&1',
        f'tar -cf ..\\build_{lib}.tar *.lib *.log *.txt obj',
    ])
    dst = os.path.join(out, lib)
    if os.path.isdir(dst):
        import shutil
        shutil.rmtree(dst)
    os.makedirs(dst)
    tar = os.path.join(dst, 'build.tar')
    vc6.scp(vc6.remote(f'{vc6.WORK}\\build_{lib}.tar'), tar)
    with tarfile.open(tar) as t:
        t.extractall(dst)
    os.remove(tar)
    vc6.ssh(f'cmd /c rmdir /s /q "{work}" & del "{vc6.WORK}\\build_{lib}.tar"', check=False)
    return summarize(lib, paths, dst, p.stdout + p.stderr)


DIAG = re.compile(r'^(.*?)\((\d+)\) ?: (fatal error|error|warning) (C\d+): (.*)$')


def summarize(lib, paths, dst, bat_text):
    log = open(os.path.join(dst, 'cl.log'), encoding='latin1').read()
    names = {os.path.basename(p).lower(): p for p in paths}
    current = None
    per = collections.OrderedDict((os.path.basename(p), {'errors': [], 'warnings': 0}) for p in paths)
    other = []
    for line in log.splitlines():
        s = line.strip()
        if s.lower() in names:
            current = os.path.basename(names[s.lower()])
            continue
        m = DIAG.match(s)
        if m and current:
            if m.group(3) == 'warning':
                per[current]['warnings'] += 1
            else:
                per[current]['errors'].append(s)
        elif m:
            other.append(s)
        elif ' error ' in s or s.startswith('Command line'):
            other.append(s)
    objs = {os.path.splitext(f)[0].lower() for f in os.listdir(os.path.join(dst, 'obj'))} \
        if os.path.isdir(os.path.join(dst, 'obj')) else set()
    failed = {}
    for name, d in per.items():
        if os.path.splitext(name)[0].lower() not in objs:
            failed[name] = d['errors'][0] if d['errors'] else '(no object, no error listed)'
    libfile = os.path.join(dst, f'{lib}.lib')
    members = [l.strip() for l in open(os.path.join(dst, 'members.txt'), encoding='latin1')
               if l.strip().lower().endswith('.obj')] if os.path.exists(os.path.join(dst, 'members.txt')) else []
    summary = {
        'lib': lib, 'files': len(paths), 'compiled': len(objs), 'failed': failed,
        'errors': sum(len(d['errors']) for d in per.values()),
        'files_with_errors': {n: d['errors'][0] for n, d in per.items() if d['errors']},
        'warnings': sum(d['warnings'] for d in per.values()),
        'warnings_per_file': {n: d['warnings'] for n, d in per.items() if d['warnings']},
        'other_diagnostics': other[:20],
        'lib_size': os.path.getsize(libfile) if os.path.exists(libfile) else 0,
        'members': len(members),
        'lib_log': open(os.path.join(dst, 'lib.log'), encoding='latin1').read().strip()
        if os.path.exists(os.path.join(dst, 'lib.log')) else '',
    }
    json.dump(summary, open(os.path.join(dst, 'summary.json'), 'w'), indent=1)
    return summary


def sys_symbols(out):
    """Save `dumpbin /linkermember:1` of each system library to out/_sys/<name>.txt (once)."""
    d = os.path.join(out, '_sys')
    want = [n for n in SYS_LIBS + SYS_LIBS_EXTRA
            if not os.path.exists(os.path.join(d, n.split(':')[-1].lower() + '.txt'))]
    if not want:
        return
    os.makedirs(d, exist_ok=True)
    sdk = f'{vc6.WORK}\\build_sdk'
    vc6.run_bat([f'if not exist "{sdk}" mkdir "{sdk}"'])
    lines = [f'cd /d "{sdk}"']
    for n in want:
        name = n.split(':')[-1]
        if n.startswith('sdk:'):
            vc6.scp(os.path.join(SDK_LIB, name), vc6.remote(f'{sdk}\\{name}'))
            src = f'{sdk}\\{name}'
        else:
            src = f'{VC_LIB}\\{name}'
        lines.append(f'dumpbin /nologo /linkermember:1 "{src}" > "{name.lower()}.txt" 2>&1')
    lines.append('tar -cf ..\\build_sdk.tar *.txt')
    vc6.run_bat(lines)
    tar = os.path.join(d, 'sys.tar')
    vc6.scp(vc6.remote(f'{vc6.WORK}\\build_sdk.tar'), tar)
    with tarfile.open(tar) as t:
        t.extractall(d)
    os.remove(tar)
    vc6.ssh(f'cmd /c rmdir /s /q "{sdk}" & del "{vc6.WORK}\\build_sdk.tar"', check=False)


def build(libs, out):
    free = queue.Queue()
    for h in vc6.hosts():
        free.put(h)

    def one(lib):
        host = free.get()
        try:
            vc6.use_host(host)
            return build_lib(lib, out)
        finally:
            free.put(host)

    order = sorted(libs, key=lambda l: -len(sources(l)))
    with cf.ThreadPoolExecutor(max(1, len(vc6.hosts()))) as ex:
        results = {s['lib']: s for s in ex.map(one, order)}
    vc6.use_host(vc6.hosts()[0])
    sys_symbols(out)
    for lib in libs:
        s = results[lib]
        print(f"{lib:11s} {s['compiled']:3d}/{s['files']:3d} compiled, {len(s['failed'])} failed, "
              f"{s['errors']} errors, {s['warnings']} warnings; {lib}.lib {s['lib_size']:,} bytes, "
              f"{s['members']} members")
        for name, err in s['failed'].items():
            print(f'    FAILED {name}: {err[:200]}')
    return results


# ------------------------------------------------------------------------------------ unresolved

SYM = re.compile(r'^[0-9A-F]{3,} [0-9A-F]{8} (\S+)\s+\S+\s+(?:\(\)\s+)?(External|WeakExternal)\s+\| (\S+)(?: \((.*)\))?$')


def obj_symbols(path):
    """{obj: (defined set, undefined set)}, and {symbol: demangled}, from dumpbin /symbols."""
    per = {}
    demangled = {}
    obj = None
    for line in open(path, encoding='latin1'):
        line = line.rstrip('\n')
        if line.startswith('Dump of file '):
            obj = os.path.basename(line[len('Dump of file '):].strip())
            per[obj] = (set(), set())
            continue
        m = SYM.match(line)
        if not m or obj is None:
            continue
        sect, kind, sym, dem = m.groups()
        if dem:
            demangled[sym] = dem
        value = line.split()[1]
        if sect == 'UNDEF' and kind == 'External' and int(value, 16) == 0:
            per[obj][1].add(sym)
        else:
            per[obj][0].add(sym)   # defined, a common (UNDEF with a size) or a weak external
    return per, demangled


def lib_publics(path):
    pubs = set()
    started = False
    for line in open(path, encoding='latin1'):
        if 'public symbols' in line:
            started = True
            continue
        if started:
            parts = line.split()
            if len(parts) == 2 and re.fullmatch(r'[0-9A-F]+', parts[0]):
                pubs.add(parts[1])
    return pubs


FUNC_DEF = re.compile(r'^(?=[^\s/#])[^;{}()=\n]*?(?<![\w:~])((?:\w+::)*~?\w+)\s*\([^;\n]*$', re.M)
OPERATOR_DEF = re.compile(r'^(?=[^\s/#])[^;{}=\n]*?(?<![\w:~])((?:\w+::)+operator)\s*([^\s(]*|\(\))\s*\(', re.M)
AUTO_POOL = re.compile(r'^\s*DEFINE_AUTO_POOL\s*\(\s*(\w+)', re.M)
DATA_DEF = re.compile(r'^(?!return|typedef|extern|using|#)[A-Za-z_][\w \t*&:<>,]*?[ \t*&]((?:\w+::)*\w+)[ \t]*'
                      r'(?:\[[^\]]*\])?\s*(?:=|;)', re.M)


def definitions():
    """{'Class::Method' or name: set of 'copy:lib/file'} over the EA copies' and the tree's .cpp
    and .h files (copy 'tree' for the repository). A crude column-0 scan: a function head without
    a ';' on its line, or a data definition."""
    defs = collections.defaultdict(set)

    def scan(path, tag):
        text = open(path, encoding='latin1').read()
        for pat in (FUNC_DEF, DATA_DEF):
            for m in pat.finditer(text):
                defs[m.group(1)].add(tag)
        for m in OPERATOR_DEF.finditer(text):
            defs[m.group(1) + m.group(2)].add(tag)
        for m in AUTO_POOL.finditer(text):           # AutoPoolClass<X, N>::Allocator
            defs['AutoPoolClass<' + m.group(1) + '>::Allocator'].add(tag)
    for lib in I.LIBS:
        for c, sub in I.LIB_DIR[lib].items():
            d = os.path.join(I.COPY_ROOT[c], sub)
            if os.path.isdir(d):
                for f in os.listdir(d):
                    if f.lower().endswith(('.cpp', '.h', '.inl')):
                        scan(os.path.join(d, f), f'{c}:{lib}/{f}')
    for lib in ORDER:
        for f in git('ls-files', '--', lib + '/').splitlines():
            if f.lower().endswith(('.cpp', '.h', '.inl')):
                scan(os.path.join(REPO, f), f'tree:{f}')
    return defs


def qualified(dem):
    """'Class::Method' (or 'function') of a demangled name, without arguments."""
    if not dem:
        return None
    s = re.sub(r'\(.*$', '', dem)                          # cut the argument list
    s = re.sub(r'<[^<>]*(?:<[^<>]*>[^<>]*)*>', '', s)      # drop template arguments
    s = s.replace('__thiscall ', '').replace('__cdecl ', '').replace('__stdcall ', '').replace('__fastcall ', '')
    s = s.split()[-1] if s.split() else s
    return s.strip('*&')


def unresolved(out, report):
    libs = [l for l in ORDER if os.path.exists(os.path.join(out, l, 'symbols.txt'))]
    objsyms, dem = {}, {}
    defined_by = collections.defaultdict(set)
    for lib in libs:
        per, d = obj_symbols(os.path.join(out, lib, 'symbols.txt'))
        dem.update(d)
        objsyms[lib] = per
        for o, (defs, _) in per.items():
            for s in defs:
                defined_by[s].add(f'{lib}/{o}')
    sysd = os.path.join(out, '_sys')
    tier1, tier2 = set(), {}
    for n in SYS_LIBS:
        tier1 |= lib_publics(os.path.join(sysd, n.split(':')[-1].lower() + '.txt'))
    mss = os.path.join(out, 'mss32', 'linkermember.txt')
    if os.path.exists(mss):
        tier1 |= lib_publics(mss)
    stubbed = set()      # defined by stubs/ (D44): `link` saves their symbols in out/stubs/symbols.txt
    if os.path.exists(os.path.join(out, 'stubs', 'symbols.txt')):
        for _, (sdefs, _) in obj_symbols(os.path.join(out, 'stubs', 'symbols.txt'))[0].items():
            stubbed |= sdefs
    for n in SYS_LIBS_EXTRA:
        for s in lib_publics(os.path.join(sysd, n.split(':')[-1].lower() + '.txt')):
            tier2.setdefault(s, n.split(':')[-1])
    # symbol -> {lib: set(objs referencing it)}
    refs = collections.defaultdict(lambda: collections.defaultdict(set))
    for lib, per in objsyms.items():
        for o, (_, undefs) in per.items():
            for s in undefs:
                if s not in defined_by and s not in tier1:
                    refs[s][lib].add(o)
    defs = definitions()
    failed = set()
    for lib in libs:
        try:
            failed |= {f.lower() for f in json.load(open(os.path.join(out, lib, 'summary.json')))['failed']}
        except (OSError, ValueError, KeyError):
            pass
    tree_cpp = {os.path.basename(p).lower() for l in ORDER for p in sources(l)}
    nl_names = {n for (_, n) in I.NOT_LINKED}
    nl_stems = {os.path.splitext(n)[0] for n in nl_names}

    def cause(sym):
        """(group, detail): the likely reason nothing defines sym."""
        d = dem.get(sym, sym)
        q = qualified(d) or sym
        if sym in stubbed:
            return 'stubbed (stubs/, D44)', ''
        if sym in tier2:
            return 'system library not on the list', tier2[sym]
        if re.match(r'^(__imp_)?_?_?AIL_', sym):
            return 'Miles (mss32.lib)', ''
        if re.match(r'^(__imp_)?_?_?(Bink|RAD)', sym):
            return 'Bink (binkw32.lib)', ''
        cls = q.split('::')[-2] if '::' in q else ''
        stem = re.sub(r'class$', '', cls.lower())
        pool = re.match(r'^\?Allocator@\?\$AutoPoolClass@V(\w+)@@', sym)
        if pool:
            q = 'AutoPoolClass<' + pool.group(1) + '>::Allocator'
        elif 'operator' in q:
            q = re.sub(r'operator\s+', 'operator', q)
        where = defs.get(q, set())
        if not where and q.startswith('Int::'):      # int.cpp defines Int<64>'s statics through `bignum`
            where = defs.get('bignum::' + q[5:], set())
        files = {w.split('/')[-1].lower() for w in where}
        if (stem and stem in nl_stems) or files & nl_names:
            return 'NOT_LINKED file', ', '.join(sorted(files & nl_names) or [stem + '.cpp'])
        tree = sorted(w[5:] for w in where if w.startswith('tree:'))
        tree_c = [w for w in tree if w.lower().endswith('.cpp')]
        if tree_c:
            bad = [w for w in tree_c if os.path.basename(w).lower() in failed]
            if bad:
                return 'defined in a tree file that failed to compile', ', '.join(bad)
            if not any(w.split('/')[0].lower() in libs for w in tree_c):
                return 'defined in a library not built in this run', ', '.join(tree_c[:3])
            return 'defined in a tree .cpp but not emitted (#if / inline / signature differs)', ', '.join(tree_c[:3])
        ea_c = sorted(w for w in where if not w.startswith('tree:') and w.lower().endswith('.cpp'))
        if ea_c:
            names = sorted({w.split('/')[-1] for w in ea_c})
            if any(n.lower() in tree_cpp for n in names):
                return 'EA copy defines it, the tree\'s copy of the file does not', ', '.join(ea_c[:3])
            return 'EA file not in the tree (no evidence, never imported)', ', '.join(ea_c[:3])
        if tree:
            return 'inline in a tree header, body not emitted', ', '.join(tree[:2])
        if where:
            return 'inline in an EA header only', ', '.join(sorted(where)[:2])
        if sym.startswith('??_7') or sym.startswith('??_R') or 'vftable' in d:
            return 'vtable/RTTI of a class whose first virtual is missing', ''
        if '::' in q:
            return 'declared-only member (no definition in the tree or EA copies)', ''
        return 'declared-only or E&B app hook (free function/global, no definition anywhere)', ''

    groups = collections.defaultdict(lambda: collections.defaultdict(list))
    for s in sorted(refs):
        c, detail = cause(s)
        for lib in refs[s]:
            groups[lib][c].append((s, detail, sorted(refs[s][lib])))
    lines = [f'# Unresolved externals over {", ".join(l + ".lib" for l in libs)}',
             f'# (not defined by any of them nor by {", ".join(n.split(":")[-1] for n in SYS_LIBS)})',
             f'# {len(refs)} symbols ({len(set(refs) & stubbed)} defined by stubs/). Generated by tools/vc6/build.py '
             'unresolved; causes are heuristics.', '']
    for lib in libs:
        if lib not in groups:
            continue
        n = sum(len(v) for v in groups[lib].values())
        lines.append(f'== {lib}: {n} symbols')
        for c in sorted(groups[lib], key=lambda c: -len(groups[lib][c])):
            lines.append(f'  -- {c}: {len(groups[lib][c])}')
            for s, extra, objs in groups[lib][c]:
                d = dem.get(s, '')
                lines.append(f'     {s}' + (f'  [{d}]' if d and d != s else '') + f'  <- {", ".join(objs[:4])}'
                             + (f' (+{len(objs) - 4})' if len(objs) > 4 else '') + (f'  ({extra})' if extra else ''))
        lines.append('')
    os.makedirs(os.path.dirname(os.path.abspath(report)), exist_ok=True)
    open(report, 'w', encoding='utf-8', newline='\n').write('\n'.join(lines))
    print(f'{len(refs)} unresolved symbols ({len(set(refs) & stubbed)} defined by stubs/, '
          f'{len(set(refs) - stubbed)} not) -> {report}')
    for lib in libs:
        if lib in groups:
            print(f'  {lib:11s} ' + ', '.join(f'{c} {len(v)}' for c, v in
                                              sorted(groups[lib].items(), key=lambda x: -len(x[1]))))
    return refs


# ------------------------------------------------------------------------------------------ link

# L2: programs linked from the tree's libraries. Each program's sources are every .cpp in its
# directory, compiled with `flags` (WW3D2's: its headers' layouts depend on its defines) and
# vc6.DEFAULT_DEFINES. stubs/*.cpp (D44) go into stubs.lib, so a stub is linked only when
# something references it; the map file shows which were.
PROGRAMS = {
    'vidtest': {'dir': os.path.join('programs', 'vidtest'), 'flags': I.LIB_FLAGS['ww3d2'],
                'subsystem': 'windows', 'runtime': []},
}
STUBS_DIR = 'stubs'
LINK_LIBS = ['app', 'wwshade', 'ww3d2', 'wwaudio', 'wwsaveload', 'wwmath', 'wwlib', 'wwdebug']
LINK_SYS = ['LIBCMT.LIB', 'LIBCPMT.LIB', 'OLDNAMES.LIB', 'KERNEL32.LIB', 'USER32.LIB', 'GDI32.LIB',
            'ADVAPI32.LIB', 'OLE32.LIB', 'OLEAUT32.LIB', 'UUID.LIB', 'WINMM.LIB', 'WS2_32.LIB',
            'VFW32.LIB', 'COMSUPP.LIB',                                   # D50
            'sdk:d3d8.lib', 'sdk:d3dx8.lib', 'sdk:dxguid.lib', 'sdk:ddraw.lib']
UNRES = re.compile(r'error LNK20(?:01|19): unresolved external symbol (\S+)(?: referenced in function (\S+))?')


def link_program(name, out):
    """Compile a program and the stubs in the VM and link them against out/<lib>/<lib>.lib,
    out/mss32/mss32.lib and the system libraries. Writes out/programs/<name>/: <name>.exe,
    <name>.map, cl.log, link.log, summary.json (unresolved symbols, stubs linked)."""
    prog = PROGRAMS[name]
    pdir = os.path.join(REPO, prog['dir'])
    srcs = sorted(os.path.join(pdir, f) for f in os.listdir(pdir) if f.lower().endswith(('.cpp', '.c')))
    sdir = os.path.join(REPO, STUBS_DIR)
    stubs = sorted(os.path.join(sdir, f) for f in os.listdir(sdir) if f.lower().endswith(('.cpp', '.c'))) \
        if os.path.isdir(sdir) else []
    libs = [os.path.join(out, l, l + '.lib') for l in LINK_LIBS]
    libs.append(os.path.join(out, MSS32))
    missing = [l for l in libs if not os.path.exists(l)]
    if missing:
        raise SystemExit('build the libraries first (build.py; mkmss32.py lib): missing ' + ', '.join(missing))
    app_dirs = [d for d, _, _ in os.walk(I.dest('app'))]
    incs = [vc6.under_root(i) for i in I.tree_includes() + app_dirs + [pdir, sdir]]
    vc6.sync([pdir, sdir] + [f for f, _ in incs] + checked_flags()[0])
    work = f'{vc6.WORK}\\link_{name}'
    vc6.run_bat([f'if exist "{work}" rmdir /s /q "{work}"', f'mkdir "{work}\\obj"', f'mkdir "{work}\\stubobj"',
                 f'mkdir "{work}\\libs"'])
    # the libraries go over as one tar
    tag = vc6._tag()
    local_tar = os.path.join(os.environ.get('TEMP', '.'), f'enb_link_{name}_{tag}.tar')
    with tarfile.open(local_tar, 'w') as tar:
        for l in libs:
            tar.add(l, arcname=os.path.basename(l))
        for n in LINK_SYS:
            if n.startswith('sdk:'):
                tar.add(os.path.join(SDK_LIB, n[4:]), arcname=n[4:])
    vc6.scp(local_tar, vc6.remote(f'{work}\\libs.tar'))
    os.remove(local_tar)

    def rsp(flags, files, objdir):
        cmd = [flags] + [f'/I "{f}"' for f, _ in incs] + [f'/D "{d}"' for d in vc6.DEFAULT_DEFINES] + checked_flags()[1]
        return '\r\n'.join(cmd + [f'/Fo"{work}\\{objdir}\\\\"'] + [f'"{vc6.under_root(f)[0]}"' for f in files]) + '\r\n'

    lines = [f'cd /d "{work}\\libs"', 'tar -xf ..\\libs.tar', 'del ..\\libs.tar', f'cd /d "{work}"']
    for tag2, flags, files, objdir in (('cl', prog['flags'], srcs, 'obj'), ('stubs', I.LIB_FLAGS['ww3d2'] + ' /GX', stubs, 'stubobj')):
        if not files:
            continue
        local = os.path.join(os.environ.get('TEMP', '.'), f'enb_link_{name}_{tag2}_{tag}.rsp')
        open(local, 'w', newline='').write(rsp(flags, files, objdir))
        vc6.scp(local, vc6.remote(f'{work}\\{tag2}.rsp'))
        os.remove(local)
        lines.append(f'cl @"{work}\\{tag2}.rsp" >> cl.log 2>&1')
    if stubs:
        lines += ['dir /b stubobj\\*.obj > stubobjs.txt 2>nul',
                  'cd stubobj', 'lib /nologo /out:..\\libs\\stubs.lib *.obj > ..\\stubslib.log 2>&1', 'cd ..',
                  'dumpbin /nologo /symbols stubobj\\*.obj > stubsyms.txt 2>&1']
    sys_names = [n.split(':')[-1] for n in LINK_SYS]
    link = ['/nologo', f'/subsystem:{prog["subsystem"]}', '/machine:ix86', '/incremental:no', '/nodefaultlib',
            f'/out:{name}.exe', f'/map:{name}.map', f'/libpath:"{work}\\libs"', 'obj\\*.obj']
    link += [os.path.basename(l) for l in libs] + (['stubs.lib'] if stubs else []) + sys_names
    local = os.path.join(os.environ.get('TEMP', '.'), f'enb_link_{name}_link_{tag}.rsp')
    open(local, 'w', newline='').write('\r\n'.join(link) + '\r\n')
    vc6.scp(local, vc6.remote(f'{work}\\link.rsp'))
    os.remove(local)
    lines += [f'link @link.rsp > link.log 2>&1',
              f'tar -cf ..\\link_{name}.tar *.log *.map *.exe *.txt 2>nul']
    vc6.run_bat(lines)
    dst = os.path.join(out, 'programs', name)
    os.makedirs(dst, exist_ok=True)
    for f in os.listdir(dst):
        if f.lower().endswith(('.exe', '.map', '.log')) and f.lower() not in ('vidtest.log', 'stubs.log'):
            os.remove(os.path.join(dst, f))
    tar = os.path.join(dst, 'link.tar')
    vc6.scp(vc6.remote(f'{vc6.WORK}\\link_{name}.tar'), tar)
    with tarfile.open(tar) as t:
        t.extractall(dst)
    os.remove(tar)
    vc6.ssh(f'cmd /c rmdir /s /q "{work}" & del "{vc6.WORK}\\link_{name}.tar"', check=False)
    if os.path.exists(os.path.join(dst, 'stubsyms.txt')):
        os.makedirs(os.path.join(out, 'stubs'), exist_ok=True)
        os.replace(os.path.join(dst, 'stubsyms.txt'), os.path.join(out, 'stubs', 'symbols.txt'))
    return link_summary(name, dst, stubs)


def link_summary(name, dst, stubs):
    log = open(os.path.join(dst, 'link.log'), encoding='latin1').read()
    cl = open(os.path.join(dst, 'cl.log'), encoding='latin1').read() if os.path.exists(os.path.join(dst, 'cl.log')) else ''
    unres = collections.OrderedDict()
    for m in UNRES.finditer(log):
        unres.setdefault(m.group(1), set())
        if m.group(2):
            unres[m.group(1)].add(m.group(2))
    # stub objects the linker pulled: the map's per-section contributions name `stubs.lib:x.obj`
    mapf = os.path.join(dst, name + '.map')
    pulled = set()
    if os.path.exists(mapf):
        for line in open(mapf, encoding='latin1'):
            for m in re.finditer(r'stubs:(\w+)\.obj', line):
                pulled.add(m.group(1) + '.obj')
    exe = os.path.join(dst, name + '.exe')
    summary = {'program': name, 'exe': os.path.exists(exe), 'exe_size': os.path.getsize(exe) if os.path.exists(exe) else 0,
               'cl_errors': [l.strip() for l in cl.splitlines() if ' error ' in l][:30],
               'link_errors': [l.strip() for l in log.splitlines() if 'error' in l.lower() and 'LNK2001' not in l
                               and 'LNK2019' not in l][:30],
               'link_warnings': [l.strip() for l in log.splitlines() if 'warning' in l.lower()][:30],
               'unresolved': {s: sorted(v)[:5] for s, v in unres.items()},
               'stub_objects': [os.path.basename(s) for s in stubs], 'stub_objects_linked': sorted(pulled)}
    json.dump(summary, open(os.path.join(dst, 'summary.json'), 'w'), indent=1)
    print(f"{name}: exe {'written, ' + format(summary['exe_size'], ',') + ' bytes' if summary['exe'] else 'NOT written'}; "
          f"{len(unres)} unresolved; {len(summary['cl_errors'])} compile errors; stub objects linked: "
          f"{', '.join(sorted(pulled)) or 'none'}")
    for e in summary['cl_errors'][:10]:
        print('  cl:', e[:200])
    for s in list(unres)[:60]:
        print('  unresolved:', s, ' <- ', ', '.join(sorted(unres[s])[:2]))
    for e in summary['link_errors'][:10]:
        print('  link:', e[:200])
    return summary


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', nargs='?', default='build', choices=['build', 'unresolved', 'link'])
    ap.add_argument('--program', action='append', choices=list(PROGRAMS), help='link: the programs (default all)')
    ap.add_argument('--lib', action='append', choices=ORDER, help='build only these libraries')
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    ap.add_argument('--report', default=os.path.join(REPO, '.work', 'milestones', 'l1', 'unresolved.txt'))
    ap.add_argument('--pool', type=int, metavar='K', help='lease K VMs from pool.py, release after')
    ap.add_argument('--holder-pid', type=int)
    ap.add_argument('--allow-dirty', action='store_true',
                    help='build even if a library has uncommitted changes')
    a = ap.parse_args()
    if a.command == 'unresolved':
        unresolved(a.out, a.report)
        return
    if a.command == 'link':
        progs = a.program or list(PROGRAMS)
        if a.pool:
            with I.pool_lease(a.pool, a.holder_pid):
                for p in progs:
                    link_program(p, a.out)
        else:
            for p in progs:
                link_program(p, a.out)
        return
    libs = a.lib or ORDER
    changed = dirty(libs)
    if changed and not a.allow_dirty:
        raise SystemExit('uncommitted changes in the libraries (build the committed tree, or --allow-dirty):\n'
                         + '\n'.join(changed[:20]))
    if a.pool:
        with I.pool_lease(a.pool, a.holder_pid):
            build(libs, a.out)
    else:
        build(libs, a.out)
    unresolved(a.out, a.report)


if __name__ == '__main__':
    main()
