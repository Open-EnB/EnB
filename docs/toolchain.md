# Toolchain and third-party SDKs

What the rebuild needs to compile, and how each piece was checked against what client.exe was
built with. Decisions D2, D3 and D5 ([progress.md](progress.md)).

## 1. Compilers (D2, D3)

The code builds with **both** Visual C++ 6.0 and a modern MSVC (D3):

| Compiler | Use |
|---|---|
| **Visual C++ 6.0** (the client's) | Proving our work: compile a file and compare its functions with the client's, byte for byte (relocations masked). A tool, not the everyday build |
| Modern MSVC (VS 2022 / VS 18, both installed) | The everyday build |

Required VC6 version: whatever reproduces the client's Rich header entries (see
[binary.md](binary.md) §1): C++ compiler id 11 build **8799** (1,054 objects), C compiler id 10
builds 8797 and 8799, linker 6.0: **Service Pack 4**, confirmed (§1.2).

### 1.1 The build VM (2026-10-22)

VC6 runs in a Hyper-V VM: Windows 10 21H2, host name `DESKTOP-1J2STKG.mshome.net` (Default
Switch), account `Daniel`, reached by SSH with the key `~/.ssh/enb_vc6`. VC6 is in
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

**Several VMs (2026-10-23).** The user added two clones of the VM. Clones keep the computer name
(`DESKTOP-1J2STKG`), so the name resolves to whichever registered last: the tools address VMs by
IP only. `python tools/vc6/vc6.py discover` finds them (SSH on the Default Switch network,
192.168.192.0/20, that accepts the key and has VC6) and writes `.work/vc6_hosts.json`; rerun it
after the VMs restart (their addresses come from DHCP). Each VM has its own sync cache; a missing
file reported by the compiler makes the wrapper resync that VM once. `importer.py verify` spreads
the libraries over the VMs (all seven in about 50 seconds). All three were checked to stamp build
8799 (SP4).

**Now (2026-10-24): Win10 plus the pool.** The two clones (`VC6 SP4 (8799)`) were retired at
07:00 (user decision D30: Hyper-V VMs removed, `Tools\VMClones\1` and `2` deleted, 24.0 + 25.2 GB).
`.work/vc6_hosts.json` lists only Win10 (192.168.193.108), the default host for every tool; more
VMs come from the pool (§1.3): `importer.py verify --pool K`. `vc6.py discover` still rewrites
`.work/vc6_hosts.json` after a restart.

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
8168), VIDTEST.EXE imports neither and has none (2026-10-24, `.work/ebconfig/assessment.md` §1). So
they are the Miles/Bink SDK import libraries, not compiled code. The 8798 C++ objects (3) are
still unexplained; they occur in all three executables (a prebuilt library common to them).

The installed VM is now SP4. The VM has a checkpoint with base VC6 (RTM), taken before SP5; SP4 was installed on top of that checkpoint.

### 1.3 The VM pool (2026-10-24)

**`tools/vc6/pool.py`** runs build VMs on demand, so agents don't share fixed VMs (since
2026-10-24 the only fixed one is Win10, §1.1).
Pool VMs are `enb-vc6-NN`: differencing disks `Tools\VMPool\enb-vc6-NN.vhdx` over the read-only
base `Tools\VMPool\base-vc6.vhdx` (prepared from the `VC6 SP4 (8799)` snapshot: Windows Update,
Defender, SysMain and WSearch off, fixed pagefile; never attach it to a VM or clear its
read-only flag). Hardware is copied from `Win10` (generation 2, Secure Boot `MicrosoftWindows`,
4 GB dynamic memory, 8 vCPU, `Default Switch`); automatic checkpoints are off and the stop
action is shut down.

    python tools/vc6/pool.py lease --agent NAME [--count K]   # prints ENB_VC6_HOSTS=Daniel@ip,...
    ENB_VC6_HOSTS=Daniel@ip python tools/vc6/vc6.py compile ...
    python tools/vc6/pool.py release --agent NAME
    python tools/vc6/pool.py stop-idle [--minutes 10]        # shut down unleased idle VMs
    python tools/vc6/pool.py reclaim [--max-age 8] [--prune [--keep 2]]
    python tools/vc6/pool.py list | create [N] | destroy VM... | destroy --all-free

`lease` reuses free VMs (running ones first), creates clones when short (cap `--max`, default 5),
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

Measured (2026-10-24, `enb-vc6-01`): creating a clone 16 s; `lease` of a stopped clone, start
to SSH and `cl` answering, 20 s; a compile stamps `@comp.id` build 8799; the differencing disk
is 2.2 GB after first boot and a compile (2.17 GB of it from the first boot). Tested (2026-10-24): `reclaim` released fake leases whose holder pid had exited, whose pid
had a different start time, and one 10 h old, and kept one held by the live `claude.exe`;
`lease` then reused the stopped `enb-vc6-01` (SSH and `cl` in 6 s) with `$CLAUDE_PID` as holder;
`reclaim --idle-minutes 0` and `stop-idle --minutes 0` each stopped it once released;
`create 2` made `enb-vc6-02`/`03`, `reclaim --prune` destroyed `03` (keep 2) and `destroy
enb-vc6-02` the other, VM, disk and config folder each gone, non-pool VMs untouched.

## 2. Third-party SDKs (D5)

Cloned to `C:\EnBDevelopment\SDK` (outside the repository, like the EA clones).

| SDK | Source | Version | Client needs | Verdict |
|---|---|---|---|---|
| DirectX 8 | github.com/edgeforce/directx8 (e6fa10f) | **8.0**: `D3D_SDK_VERSION` 120 | `DX8Wrapper::Init` calls `Direct3DCreate8(120)` (`push 0x78` at 0x921390): built against the 8.0 SDK, not 8.1 (220). The client contains `D3DX8 Shader Assembler Version 0.91`, the same string as this SDK's `d3dx8.lib` (D3DX was linked statically) | **Match** |
| Bink | github.com/edgeforce/radtools (f42d146): `include/bink.h`, `lib/binkw32.lib` | `bink.h` 1.0p | The client imports 9 Bink functions; all 9 are in `binkw32.lib` with the same decorated names. The game's `binkw32.dll` is dated 2001-08-29 | **Match** |
| Miles Sound System | same repository: `include/mss.h`, `lib/mss32.lib` | `mss.h` 6.1a (06-Mar-01); `lib/mss32.lib` older (its `tools/mss32.dll` says 6.0i) | The client imports 81 Miles functions. The game's `Mss32.dll` (6.1a, dated 2003-09-22) exports all 81. `mss32.lib` lacks 30 of them; `mss.h` declares all but one, `AIL_open_stream_by_sample`, which Renegade's WWAudio calls | **Usable, with two fixes** (below) |

### Miles fixes

1. **Import library:** generate `mss32.lib` from the game's own `Mss32.dll` exports (a `.def` file
   and `lib /def`), rather than using the SDK's older one. This is exact by construction: it
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

### Consequence for WW3D2

E&B compiled its WW3D2 against DirectX **8.0**. If Zero Hour's WW3D2 uses anything only DirectX
8.1 has, that code can't be what E&B had: the 8.0 headers will show it at compile time, and each
such place is a point where E&B's copy differs from Zero Hour's.
