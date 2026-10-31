# Toolchain and third-party SDKs

What the rebuild needs to compile, and how each piece was checked against what client.exe was
built with. Decisions D2, D3 and D5 ([progress.md](progress.md)).

## 1. Compilers (D2, D3)

The code builds with **both** Visual C++ 6.0 and a modern MSVC (D3):

| Compiler | Use |
|---|---|
| **Visual C++ 6.0** (the client's) | Proving our work: compile a file and compare its functions with the client's, byte for byte (relocations masked). A tool, not the everyday build |
| Modern MSVC (VS 2022 or later) | The everyday build |

Required VC6 version: whatever reproduces the client's Rich header entries (see
[binary.md](binary.md) §1): C++ compiler id 11 build **8799** (1,054 objects), C compiler id 10
builds 8797 and 8799, linker 6.0: **Service Pack 4**, confirmed (§1.2).

### 1.1 The build VM (2026-10-22)

A new machine sets its VM up with [setup-vc6.md](setup-vc6.md) (2026-10-29: one script,
`tools/vc6/setup/New-BuildVM.ps1`, VC6 by file copy, proven equal to the setup-installed VM below).
Machine-specific settings (account, key, pool folder, template VM) are in `.work/vc6_config.json`
(`tools/vc6/vc6config.py`).

VC6 runs in a Hyper-V VM: Windows 10 21H2 on the Default Switch, addressed by IP, the VM account
(`user` in `.work/vc6_config.json`), reached by SSH with the key `~/.ssh/enb_vc6`. VC6 is in
`C:\Program Files (x86)\Microsoft Visual Studio\VC98` (environment variables not registered;
call `VC98\Bin\VCVARS32.BAT`). `\\tsclient\C` (the desktop session's drive redirection) is not
visible to SSH sessions, so files go over SSH (`scp`, `tar`).

**`tools/vc6/vc6.py`** drives it: `compile` packs the needed source directories (source files
only) into a tar, copies it to the VM, where `C:\EnBDevelopment` is mirrored at the same path,
compiles with VCVARS32 set, and brings the objects back (`--sync DIR` also copies a directory
reached only by a relative `#include`, such as Generals' `GameEngine\Include\Common`); `run` runs any command; `check` prints
the compiler and linker versions. Default flags `/nologo /c /MT /W3 /O2 /Ob2`, defines `WIN32
_WINDOWS WIN32_LEAN_AND_MEAN WINVER=0x400 NDEBUG`: the client's (library-matching.md §3.5).
Compiling WW3D2 (115 files) takes about 30 seconds.

**Several VMs.** Clones keep the template's computer name, so the name resolves to whichever
registered last: the tools address VMs by IP only. `python tools/vc6/vc6.py discover` probes the
addresses Hyper-V reports for the running VMs (SSH that accepts the key and has VC6) and writes
`.work/vc6_hosts.json`; rerun it after the VMs restart (their addresses come from DHCP, and the
Default Switch's private subnet changes when the host restarts; `--network CIDR` scans a range
instead). Each VM has its own sync cache; a missing file reported by the compiler makes the
wrapper resync that VM once. `importer.py verify` spreads the libraries over the VMs (all seven in
about 50 seconds). Every VM was checked to stamp build 8799 (SP4).

**Now: one fixed host plus the pool.** `.work/vc6_hosts.json` lists the fixed VM, the default host
for every tool; more VMs come from the pool (§1.3): `importer.py verify --pool K`. (The early
fixed clones were retired 2026-10-24, D30.)

### 1.2 Which VC6 built the client: SP4

Every object records the build of the compiler's back end (`C2.DLL`) in `@comp.id`, and the
linker copies these into the Rich header. Measured with a test file (C++ and C, `/O2 /GX /MT`):

| Toolchain | `cl.exe` | `C1.DLL` / `C1XX.DLL` | `C2.DLL` | Objects stamped | Its CRT's objects (Rich) |
|---|---|---|---|---|---|
| VC6 RTM | 12.00.8168 | | | 8168 | 8168 |
| VC6 SP4 | 12.00.8804 | 12.00.8867 | **12.00.8799** | **8799** | C 8797, C++ 8797 and 8168 |
| VC6 SP5 | 12.00.8804 | 12.00.8867 / 12.00.8964 | | 8966 | 8047 |
| **client.exe** | | | | **8799** (C++ 1,054, C some), 8798 (C++ 3) | C 8797 (201), C++ 8797 (13), 8168 (C++ 17, C 7) |

**The client was built with Visual C++ 6.0 Service Pack 4.** The compiler stamp matches, and so
does the CRT mix. `LINK.EXE` in SP4 is 6.00.8447. The client's Rich header has 96 objects with
product id 4, build 8447, which our test executable doesn't produce. They are import-library
members (Linker600 import objects): the client's mss32 imports (81) and binkw32 imports (9) plus
2x3 descriptors = 96; E&BConfig.exe has 80 mss32 imports + 3 descriptors = 83 id-4 entries (build
8168), VIDTEST.EXE imports neither and has none (2026-10-24). So
they are the Miles/Bink SDK import libraries, not compiled code. The 8798 C++ objects (3) are
still unexplained; they occur in all three executables (a prebuilt library common to them).

The Character and Starship Creator (2002-09-13, [CSC.md](CSC.md) §1) is the exception: every object,
its CRT included, is **VC6 RTM** (C++ 8168 x920, C 8168 x185), its Miles import library is build 8168
(as E&BConfig's), cvtres is 1720, and it has **no 8798 objects**. So the 8798 objects are not part
of every E&B link: they came with something CSC didn't link, or only with the build server's SP4
environment. CSC was linked non-incrementally with `/OPT:REF,ICF` (CSC.md §1.1); the client
incrementally.

### 1.3 The VM pool (2026-10-24)

**`tools/vc6/pool.py`** runs build VMs on demand, so agents don't share fixed VMs (one fixed host
remains, §1.1).
Pool VMs are `enb-vc6-NN`: differencing disks `Tools\VMPool\enb-vc6-NN.vhdx` over the read-only
base `Tools\VMPool\base-vc6.vhdx` (prepared from the `VC6 SP4 (8799)` snapshot: Windows Update,
Defender, SysMain and WSearch off, fixed pagefile; never attach it to a VM or clear its
read-only flag). Hardware is copied from the template VM (`template_vm` in `.work/vc6_config.json`:
`enb-vc6-template` for a VM made by setup-vc6.md; generation 2, Secure Boot `MicrosoftWindows`,
dynamic memory, 8 vCPU, `Default Switch`) except the startup memory: clones get 2 GB (`STARTUP_GB`,
2026-10-27: a 4 GB startup failed clone starts on a loaded host while the VMs run at ~1.1 GB; an
older clone with 4 GB takes `Set-VMMemory -StartupBytes 2GB` once it is off); automatic checkpoints are
off and the stop action is shut down. The cap (`--max`, default 7) is set by host memory: check
`Get-CimInstance Win32_OperatingSystem` free memory before raising it.

    python tools/vc6/pool.py lease --agent NAME [--count K]   # prints ENB_VC6_HOSTS=<user>@ip,...
    ENB_VC6_HOSTS=<user>@ip python tools/vc6/vc6.py compile ...
    python tools/vc6/pool.py release --agent NAME
    python tools/vc6/pool.py stop-idle [--minutes 10]        # shut down unleased idle VMs
    python tools/vc6/pool.py reclaim [--max-age 8] [--prune [--keep 2]]
    python tools/vc6/pool.py list | create [N] | destroy VM... | destroy --all-free

`lease` reuses free VMs (running ones first), creates clones when short (cap `--max`, default 7),
starts them, waits for the address (Hyper-V's KVP), SSH with the key and `cl`, and clears
vc6.py's sync cache for that IP, so the first compile sends the tree (each leasee syncs its own;
no warm cache, user decision). The host key of a pool IP is accepted on first sight
(`accept-new`). Leases are by agent name in `.work/pool/leases.json` (lock `leases.lock`,
taken over when its process is dead or it is older than 15 minutes, as a live holder may stop
or create several VMs under it).

**Orphans (user decision 2026-10-24).** Sessions release their VMs in the handoff skill
(`release --agent NAME`, then `stop-idle --minutes 0`). For sessions that never hand off, each
lease records its holder: a long-lived pid with its start time, the machine, the Claude Code
session id (`CLAUDE_CODE_SESSION_ID`) and the lease time. The holder is `--holder-pid`, else
`$ENB_HOLDER_PID`, else `$CLAUDE_PID` (Claude Code's `claude.exe`, set in every agent shell); not
the `python pool.py lease` process, which exits at once, nor its parent shell, which Claude Code
starts per command. Subagents share the session's `claude.exe`, so a subagent's lease outlives
the subagent until its release, the session's exit or the age limit. A lease is orphaned when
the holder pid is gone, names a process started later (pid reuse), or the lease is older than
`--max-age` hours (8; also the only test for leases without a holder pid). `reclaim`, which
`lease` and `create` run first, releases orphaned leases; reports free VMs that fail the safety
check and never touches them; stops free running VMs idle `--idle-minutes` (10; `lease` spares
the ones it just took); with `--prune` destroys free stopped VMs beyond the `--keep`
lowest-numbered (2); and drops free entries of VMs that no longer exist. `lease` then reuses
free VMs, running ones first, stopped ones next, before creating any clone. `list` shows who
holds what and the holder pid. Every start, stop or delete checks the name prefix and that the
VM's only disk is a differencing child of the base in `Tools\VMPool`, and `destroy` refuses
leased VMs. The pool never touches `.work/vc6_hosts.json` or vc6.py's defaults.

**`importer.py verify --pool K [--holder-pid P]`** (opt-in; without it verify uses the default
hosts as before) leases K VMs as agent `importer-<pid>`, sets `ENB_VC6_HOSTS` for the run and
releases the lease in a `finally` (it leaves stopping idle VMs to `stop-idle`/`reclaim`). The
holder is `--holder-pid`, else pool.py's own default (`$ENB_HOLDER_PID`, `$CLAUDE_PID`). pool.py
runs as a child process, which is safe under the importer lock because pool.py never takes it.
Tested (2026-10-24): `verify --lib wwdebug --pool 1` on `enb-vc6-01` (81, pairs unchanged) and
`verify --lib ww3d2 --pool 2` on `enb-vc6-01`/`02` (2,477, pairs unchanged, about 100 s); both
leases released (`pool.py list`: free).

`importer.py refine --pool K` and `tools/vc6/bytematch_all.sh --pool K [WORKDIR]` (2026-10-25) work the
same way (agents `importer-<pid>` / `bytematch-<pid>`; the script leases in front of its loop and
releases in an EXIT trap, so also after a failure or a kill). Tested: `refine --pool 1 --lib wnet`
(gained 0, released) and the script killed 40 s into its first library (released).

Measured (2026-10-24, `enb-vc6-01`): creating a clone 16 s; `lease` of a stopped clone, start
to SSH and `cl` answering, 20 s; a compile stamps `@comp.id` build 8799; the differencing disk
is 2.2 GB after first boot and a compile (2.17 GB of it from the first boot). Tested (2026-10-24): `reclaim` released fake leases whose holder pid had exited, whose pid
had a different start time, and one 10 h old, and kept one held by the live `claude.exe`;
`lease` then reused the stopped `enb-vc6-01` (SSH and `cl` in 6 s) with `$CLAUDE_PID` as holder;
`reclaim --idle-minutes 0` and `stop-idle --minutes 0` each stopped it once released;
`create 2` made `enb-vc6-02`/`03`, `reclaim --prune` destroyed `03` (keep 2) and `destroy
enb-vc6-02` the other, VM, disk and config folder each gone, non-pool VMs untouched.

## 2. Third-party SDKs (D5)

Cloned to `C:\EnBDevelopment\SDK` (outside the repository, like the EA clones). The sources below
are the copies this project used; the SDKs are Microsoft's and RAD's, and the project does not
distribute them (D150): bring your own copy and check it against these SHA-256 hashes.

| File (under `SDK\`) | SHA-256 |
|---|---|
| `directx8\include\d3d8.h` | `3ca75d0512eb882de054bf8d93c1a25dc356e3681d83eaf4e15e7d46d2eb80b0` |
| `directx8\include\d3dx8.h` | `4dda934ffc04ca0a7ee7551ee524c1eb9e5ec85cb345202ea0661c71feff122c` |
| `directx8\lib\d3d8.lib` | `3c9f0dc543c41c4b03d380ae47591133db4718dd4bd6c0b739c97a4aa9a493dd` |
| `directx8\lib\d3dx8.lib` | `32148094cebbbe9b55f7769787cdde5926ca01014e072844f5152d25072e1f44` |
| `directx8\lib\dxguid.lib` | `70a74a5e9c195f73db8d6a9bd082611825d07605b67fc7bceb10171d2c194cce` |
| `radtools\include\bink.h` | `25c9a43ae85386cd0d9e00fe5d1f3670814c7a6be2e7c5d8e9952a5fa8674f99` |
| `radtools\include\mss.h` | `721bac19260c63445524acc8a027e25307c4ddaa8e47990a72b7109567047377` (before `sdk_patch.py`) |
| `radtools\include\rad.h` | `528d9fea30775f34af20c8779b44435e1d7e3ad1ad6c24e879aa81ac40719b69` |
| `radtools\lib\binkw32.lib` | `4ef062e19174050cf251454c92670bca256e5be3dfee0ba6ae4aa2b54ce907d1` |

| SDK | Source | Version | Client needs | Verdict |
|---|---|---|---|---|
| DirectX 8 | github.com/edgeforce/directx8 (e6fa10f) | **8.0**: `D3D_SDK_VERSION` 120 | `DX8Wrapper::Init` calls `Direct3DCreate8(120)` (`push 0x78` at 0x921390): built against the 8.0 SDK, not 8.1 (220). The client contains `D3DX8 Shader Assembler Version 0.91`, the same string as this SDK's `d3dx8.lib` (D3DX was linked statically) | **Match** |
| Bink | github.com/edgeforce/radtools (f42d146): `include/bink.h`, `lib/binkw32.lib` | `bink.h` 1.0p | The client imports 9 Bink functions; all 9 are in `binkw32.lib` with the same decorated names. The game's `binkw32.dll` is dated 2001-08-29 | **Match** |
| Miles Sound System | same repository: `include/mss.h`, `lib/mss32.lib` | `mss.h` 6.1a (06-Mar-01); `lib/mss32.lib` older (its `tools/mss32.dll` says 6.0i) | The client imports 81 Miles functions. The game's `Mss32.dll` (6.1a, dated 2003-09-22) exports all 81. `mss32.lib` lacks 30 of them; `mss.h` declares all but one, `AIL_open_stream_by_sample`, which Renegade's WWAudio calls | **Usable, with two fixes** (below) |
| SGI STL | SGI STL 3.3 (2000, the last SGI release), the 91 files of github.com/steveLauwh/SGI-STL `SGI-STL V3.3/` (a mirror of the sgi.com distribution), fetched 2026-10-26 to `SDK\stl-3.3`; read as data, nothing in it is run. Licence: the SGI (1996-1998) and Hewlett-Packard (1994) notices, "Permission to use, copy, modify, distribute and sell this software and its documentation for any purpose is hereby granted without fee, provided that the above copyright notice appear in all copies and that both that copyright notice and this permission notice appear in supporting documentation ... provided "as is" without express or implied warranty" (compatible with D1; kept in every copied file) | 3.3 | The client's hash_map (D87): the string hash `__stl_hash_string` (h*5+c), `__stl_prime_list` once per translation unit (0xAE701C, 0xAEB764, ...), the hashtable layout (functors, the bucket vector, the count). `stl_hashtable.h`, `stl_hash_map.h`, `stl_hash_set.h`, `stl_hash_fun.h` are adapted for VC6 in `libs\sgistl` (on the include path after the SDKs) by `tools/vc6/sgistl_adapt.py`'s edits, each marked `VC6 (E&B, D87)` | **Match** (kv_list.cpp, dbgconn.cpp) |

### Miles fixes

1. **Import library:** generate `mss32.lib` from the game's own `Mss32.dll` exports, rather than
   using the SDK's older one: `tools/vc6/mkmss32.py` writes a `.def` and links a dummy DLL, keeping
   LINK's import library (`lib /def` in VC6 drops the leading underscore of `_AIL_x@n`, so its
   imports don't bind; D51). This is exact by construction: it
   lists what the DLL the client runs against exports.
2. **`AIL_open_stream_by_sample`:** one declaration to add, from its decorated name
   (`_AIL_open_stream_by_sample@16`: four 32-bit arguments) and Renegade's call site in WWAudio.

The 30 functions the old import library lacks are: `AIL_3D_sample_length`, `AIL_3D_sample_offset`,
`AIL_3D_user_data`, `AIL_background`, `AIL_end_3D_sample`, `AIL_end_sample`,
`AIL_enumerate_filters`, `AIL_lock`, `AIL_lock_mutex`, `AIL_open_stream_by_sample`,
`AIL_pause_stream`, `AIL_release_timer_handle`, `AIL_resume_3D_sample`, `AIL_resume_sample`,
`AIL_sample_user_data`, `AIL_serve`, `AIL_set_3D_sample_effects_level`,
`AIL_set_3D_sample_offset`, `AIL_set_3D_speaker_type`, `AIL_set_3D_user_data`,
`AIL_set_file_callbacks`, `AIL_set_filter_sample_preference`, `AIL_set_sample_ms_position`,
`AIL_set_sample_processor`, `AIL_set_sample_user_data`, `AIL_set_stream_loop_block`,
`AIL_set_stream_ms_position`, `AIL_stop_timer`, `AIL_unlock`, `AIL_unlock_mutex`.

3. **`dllimport`:** E&B's build declared the Miles API `__declspec(dllimport)` (the client calls
   Miles through the import table, `call dword ptr [__imp__AIL_...]`); this 6.1a `mss.h`
   hard-codes plain `extern` (`#if 1 /*def __BORLANDC__*/`). `tools/vc6/sdk_patch.py` writes a
   patched copy of the headers to `SDK\radtools\include_enb` (outside the repository: the RAD
   headers aren't ours to publish), and the build uses that. Worth 30 WWAudio functions.

4. **`MSS_auto_cleanup`:** E&B's `mss.h` had `#define AIL_startup() (MSS_auto_cleanup(),AIL_startup())`
   active; this one has it commented out ("ODCODENOTE Remove"). The client's `WWAudioClass`
   constructor calls `MSS_auto_cleanup` (0xA03A30) before `AIL_startup` (0x9EEC54). `sdk_patch.py`
   restores the macro (worth the constructor). `MSS_auto_cleanup` is a static member of the SDK's
   `mss32.lib` (it sits just before the Miles import thunks: `atexit` of a wrapper at 0xA03A40 that
   tail-calls `AIL_shutdown` through the thunk, then `return 0`); the import library generated from
   the `.def` (fix 1) lacks it, so a full link needs a small object providing it.

A Miles SDK whose `mss.h` declares `AIL_open_stream_by_sample` (6.1 or later, contemporary with
Renegade, 2001–2002) would remove both fixes, but isn't required.

### EA.Com's login component (authlogin.dll)

`release\authlogin.dll` (114,688 bytes, linked 2002-07-17, 3.3.0.6) has no SDK or published source:
it exports `AuthLoginLib_GetAuthorize` and `AuthLoginLib_GetComponentFactory`, undecorated, and the
client imports only the factory (`LoginTaskClass::Fetch_EA_Ticket` 0x76A7B0). `tools/vc6/authlogin.def`
lists the two names; `tools/vc6/mkauthlogin.py` turns it into `build/authlogin/authlogin.lib` with
VC6's `lib /def` (undecorated names import correctly that way, unlike Miles' stdcall ones), and
`build.py link` adds that library when it exists. The interfaces are declared in
`app/client/unknown_authlogin.h` (D90 9a). No program here loads the real DLL: a test supplies a
stand-in exporting the factory.

### Consequence for WW3D2

E&B compiled its WW3D2 against DirectX **8.0**. If Zero Hour's WW3D2 uses anything only DirectX
8.1 has, that code can't be what E&B had: the 8.0 headers will show it at compile time, and each
such place is a point where E&B's copy differs from Zero Hour's.

## 3. External repositories and data (2026-10-25)

Everything outside this repository that the tools read lives under `C:\EnBDevelopment` (except the
retail install). The Python tools take the paths from one module, `tools/re/roots.py`
(`python tools/re/roots.py` prints each and whether it exists); the shell scripts
(`tools/vc6/bytematch_all.sh`, `runtime_match.sh`) and `tools/vc6/importer.py`,
`vc6.py`, `layout.py`, `build.py` carry the same few paths in their own constants (the VM mirrors
`C:\EnBDevelopment` at the same path, so they stay absolute).

| Path | What | Used by |
|---|---|---|
| `C:\EnBDevelopment\EA` | EA's published Westwood source (Renegade, Generals, Zero Hour, ...) | `easource.py`, `importer.py`, `bytematch_all.sh`, `libdiff.py`, `movietest.py` (BinkMovie) |
| `C:\EnBDevelopment\SDK` | DirectX 8 SDK (`directx8`) and RAD/Miles (`radtools`; `include_enb` derived by `sdk_patch.py`, §2) | `importer.py`, `build.py`, `layout.py`, `bytematch_all.sh` |
| `C:\EnBDevelopment\Tools\VMPool` | The VM pool's disks (§1.3; `ENB_VC6_POOL_DIR` overrides) | `pool.py` |
| `C:\EnBDevelopment\Net7` | Net-7 server emulator, C++ (`Net7\AuxClasses`: the aux-data classes) | `auxnames.py` (removed, D153) |
| `C:\EnBDevelopment\Tools\Media\ea-reference` (`EA_REFERENCE`; `ENB_EA_REFERENCE` overrides) | The EA captures' frozen reference (D154), unpacked by `tools/harness/fetch_reference.py`: `ea\NAME.trace.jsonl` (the reference traces, one game message per line) and `ref\NAME.*.txt` (the reference decoder's readings); fixed, never regenerated | `auxreplay.py`, `udpreplay.py`, `clienttest.py` |
| `C:\EnBDevelopment\Live-Era-Data` | EA's live-era packet dumps (`enbserver3\ENB-Packets-Live\Alex3-fixed.txt.txt`, `Alex7-...`; public at trunk/capturedPackets of github.com/therealKyp/Earth-and-Beyond-server, D154) | `convert.py`, `auxreplay.py`, `udpreplay.py` |
| `C:\Program Files (x86)\EA GAMES\Earth & Beyond` | The retail install: `release\client.exe` (SHA-256 `249f8524...51c4`), `EBCONFIG`, game data and movies. Read only; no tool uses it as a working directory (they copy what they need under `build/` or `.work/`) | `clientimage.py`, `vidtest_e2e.py`, `run_reference.py`, `movietest.py`, `mkmss32.py` |

Inside this repository, git-ignored: `.work/` (scratch, pseudocode, locks, pool state), `build/`.
To move an external repository, change `roots.py` and the shell scripts' variables listed above.
