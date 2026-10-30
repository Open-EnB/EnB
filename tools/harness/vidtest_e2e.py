"""M2 gate (c) without E&BConfig: starts vidtest exactly as E&BConfig.exe does, in a copy of EBCONFIG,
and reads results.ini as E&BConfig does (M2.3). Never touches the registry or the install.

    python tools/harness/vidtest_e2e.py [--exe build/programs/vidtest/vidtest.exe]
                                        [--dir build/programs/vidtest/e2e_ebconfig] [--original]
                                        [--resx 800 --resy 600 --depth 32 --windowed true --filter 1]

E&BConfig's launch (0x404E80..0x405035, read with capstone):
  sprintf(cmd, "-device \"%s\" -resx %d -resy %d -depth %d -windowed %s -filter %d", ...)  (%s = true/false)
  CreateProcessA("VIDTEST.EXE", cmd, NULL, NULL, FALSE, 0, NULL, NULL /* its own current directory */, ...)
  poll GetExitCodeProcess every 100 ms while STILL_ACTIVE (0x103)
  exit 1: INIClass ini; Load("results.ini") from its current directory; missing -> "Result file
          results.ini not found."; else Get_Int Results.FillRateFPS / PolyBudget / LandPolyBudget /
          LandLOD / StarBudget / TextureReduction, Info.PeakPolyRate / FreeTextureRam (atoi)
  -3 "Test aborted.", -2 "Error - The animation could not be initialized.", -1 "Error - Unsupported
  video settings.", anything else "Result file results.ini not found."
The application name is relative and lpCurrentDirectory NULL, so VIDTEST.EXE, its working directory
and results.ini are all E&BConfig's current directory: the EBCONFIG folder it was started in.
The command line has no program name, so the CRT's WinMain skips its first token ("-device"):
neither VIDTEST nor ours sees -device, both use device 0.

This script makes --dir a copy of the install's EBCONFIG folder (once), puts our exe in it as
VIDTEST.EXE (the original stays as VIDTEST.orig.exe; --original runs that instead), deletes
RESULTS.INI, changes to --dir and calls CreateProcessA the same way. Our exe gets " -nomsgbox" appended
so a failure is logged instead of shown in a modal box (the original has no such switch).
It prints what E&BConfig would show and the Render values it would then write to the registry.
"""
import argparse
import ctypes
import ctypes.wintypes as wt
import os
import shutil
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
INSTALL = os.path.join(os.environ.get('ENB_RETAIL_INSTALL') or r'C:\Program Files (x86)\EA GAMES\Earth & Beyond', 'EBCONFIG')
k32 = ctypes.windll.kernel32


class STARTUPINFO(ctypes.Structure):
    _fields_ = [('cb', wt.DWORD), ('lpReserved', wt.LPSTR), ('lpDesktop', wt.LPSTR), ('lpTitle', wt.LPSTR),
                ('dwX', wt.DWORD), ('dwY', wt.DWORD), ('dwXSize', wt.DWORD), ('dwYSize', wt.DWORD),
                ('dwXCountChars', wt.DWORD), ('dwYCountChars', wt.DWORD), ('dwFillAttribute', wt.DWORD),
                ('dwFlags', wt.DWORD), ('wShowWindow', wt.WORD), ('cbReserved2', wt.WORD),
                ('lpReserved2', ctypes.c_void_p), ('hStdInput', wt.HANDLE), ('hStdOutput', wt.HANDLE),
                ('hStdError', wt.HANDLE)]


class PROCESS_INFORMATION(ctypes.Structure):
    _fields_ = [('hProcess', wt.HANDLE), ('hThread', wt.HANDLE), ('dwProcessId', wt.DWORD),
                ('dwThreadId', wt.DWORD)]


def atoi(s):
    """VC6's atoi (atol): optional sign, digits, no overflow check (wraps to 32 bits)."""
    s = s.lstrip(' \t')
    neg = s[:1] == '-'
    if s[:1] in '+-' and s:
        s = s[1:]
    v = 0
    for c in s:
        if not c.isdigit():
            break
        v = (v * 10 + int(c)) & 0xFFFFFFFF
    if neg:
        v = (-v) & 0xFFFFFFFF
    return ctypes.c_int32(v).value


def read_ini(path):
    d = {}
    sec = None
    for line in open(path, 'r', encoding='latin-1').read().splitlines():
        line = line.split(';', 1)[0].strip()
        if line.startswith('[') and line.endswith(']'):
            sec = line[1:-1].lower()
        elif '=' in line and sec:
            k, v = line.split('=', 1)
            d.setdefault((sec, k.strip().lower()), v.strip())
    return d


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=os.path.join(REPO, 'build', 'programs', 'vidtest', 'vidtest.exe'))
    ap.add_argument('--dir', default=os.path.join(REPO, 'build', 'programs', 'vidtest', 'e2e_ebconfig'))
    ap.add_argument('--original', action='store_true', help='run the original VIDTEST (no -nomsgbox)')
    ap.add_argument('--device', default='NVIDIA GeForce RTX 3060 Ti')
    ap.add_argument('--resx', type=int, default=800)
    ap.add_argument('--resy', type=int, default=600)
    ap.add_argument('--depth', type=int, default=32)
    ap.add_argument('--windowed', default='true')
    ap.add_argument('--filter', type=int, default=1)
    ap.add_argument('--timeout', type=float, default=120)
    a = ap.parse_args()
    wd = os.path.abspath(a.dir)
    if os.path.normcase(wd).startswith(os.path.normcase(os.path.dirname(INSTALL))):
        raise SystemExit('refusing to run inside the install')
    if not os.path.isdir(wd):
        shutil.copytree(INSTALL, wd)
        print('copied', INSTALL, '->', wd)
    orig = os.path.join(wd, 'VIDTEST.orig.exe')
    target = os.path.join(wd, 'VIDTEST.EXE')
    if not os.path.exists(orig):
        os.replace(target, orig)
    if a.original:
        shutil.copyfile(orig, target)
    else:
        shutil.copyfile(os.path.abspath(a.exe), target)
    for f in os.listdir(wd):
        if f.lower() in ('results.ini', 'vidtest.log', 'stubs.log'):
            os.remove(os.path.join(wd, f))
    cmd = '-device "%s" -resx %d -resy %d -depth %d -windowed %s -filter %d' % (
        a.device, a.resx, a.resy, a.depth, a.windowed, a.filter)
    if not a.original:
        cmd += ' -nomsgbox'
    print('cwd', wd)
    print('CreateProcessA("VIDTEST.EXE", %r)' % cmd)
    os.chdir(wd)
    si = STARTUPINFO()
    si.cb = ctypes.sizeof(si)
    pi = PROCESS_INFORMATION()
    buf = ctypes.create_string_buffer(cmd.encode('latin-1'), 1024)
    if not k32.CreateProcessA(b'VIDTEST.EXE', buf, None, None, False, 0, None, None, ctypes.byref(si),
                              ctypes.byref(pi)):
        print('Unable to launch VIDTEST.EXE to test the chosen video settings! (error %d)' % k32.GetLastError())
        sys.exit(1)
    code = wt.DWORD()
    t0 = time.time()
    while True:
        if not k32.GetExitCodeProcess(pi.hProcess, ctypes.byref(code)):
            print('Unknown')
            sys.exit(1)
        if code.value != 0x103:
            break
        if time.time() - t0 > a.timeout:
            k32.TerminateProcess(pi.hProcess, 99)
            print('timeout: terminated')
            sys.exit(1)
        time.sleep(0.1)
    k32.CloseHandle(pi.hProcess)
    k32.CloseHandle(pi.hThread)
    rc = ctypes.c_int32(code.value).value
    print('exit code', rc, 'after %.1f s' % (time.time() - t0))
    ini = [f for f in os.listdir(wd) if f.lower() == 'results.ini']
    if rc == 1 and ini:
        d = read_ini(os.path.join(wd, ini[0]))
        g = lambda s, k, dv=0: atoi(d[(s, k.lower())]) if (s, k.lower()) in d else dv
        fill = g('results', 'FillRateFPS')
        vals = {k: g('results', k) for k in ('PolyBudget', 'LandPolyBudget', 'LandLOD', 'StarBudget',
                                             'TextureReduction')}
        peak = g('info', 'PeakPolyRate')
        ram = g('info', 'FreeTextureRam')
        print('E&BConfig reads %s:' % ini[0])
        print('  Fill Rate FPS=%d Polygon Budget=%d Star Budget=%d' % (fill, vals['PolyBudget'], vals['StarBudget']))
        print('  Land Polygon Budget =%d Land LOD=%d' % (vals['LandPolyBudget'], vals['LandLOD']))
        print('  Texture Reduction = %d' % vals['TextureReduction'])
        print('  Peak Poly Rate = %d KPolys/s' % peak)
        print('  Free Texture + AGP RAM = %6.3f MB  (FreeTextureRam read as int %d)' % (ram / 1048576.0, ram))
        print('  then writes to ...\\Earth and Beyond\\Render: ' + ', '.join('%s=%d' % kv for kv in vals.items()))
        sys.exit(0)
    msg = {-3: 'Test aborted.', -2: 'Error - The animation could not be initialized.',
           -1: 'Error - Unsupported video settings.'}.get(rc, 'Result file results.ini not found.')
    print('E&BConfig shows:', msg)
    sys.exit(1)


if __name__ == '__main__':
    main()
