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
product id 4, build 8447, which our test executable doesn't produce; what they are (likely
objects from a library built with that linker's toolset) is open. The 8798 C++ objects (3) are
also unexplained.

The installed VM is now SP4. The VM has a checkpoint with base VC6 (RTM), taken before SP5; SP4 was installed on top of that checkpoint.

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
