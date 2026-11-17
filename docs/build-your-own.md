# Building your own client

This page takes you from a fresh clone to a `client.exe` compiled from this repository's source,
with the compiler Westwood used: **Visual C++ 6.0 Service Pack 4**, running in a Hyper-V virtual
machine on your PC. One script does every step:

```powershell
python tools\build_client.py
```

The first run takes an hour or two, most of it making the build VM. After that, a full rebuild of
every library takes about three minutes in the VM. A run after `git pull` compiles only what changed,
and the script skips the steps that are already done.

Contents: [What you need](#what-you-need) · [What the script does](#what-the-script-does) ·
[What you get](#what-you-get) · [Running what you built](#running-what-you-built) ·
[Building by hand](#building-by-hand) · [How the workspace is laid out](#how-the-workspace-is-laid-out) ·
[Troubleshooting](#troubleshooting)

## What you need

| | |
|---|---|
| Windows | Windows 10 or 11 **Pro, Enterprise or Education** (Hyper-V), 64-bit, virtualization enabled in the firmware |
| Memory and disk | 16 GB of memory; about 60 GB of free disk |
| Software | [Python](https://www.python.org/) 3.10 or later, [git](https://git-scm.com/), [7-Zip](https://www.7-zip.org/), the OpenSSH client (Settings > System > Optional features) |
| The clone | at **`C:\EnBDevelopment\EnB`**: `git clone <this repository> C:\EnBDevelopment\EnB`. The VM sees `C:\EnBDevelopment` at the same path, so include paths and the client's `__FILE__` strings come out the same |
| Internet | for the downloads below; nothing is uploaded |

Prepare Hyper-V once, in a PowerShell window opened with **Run as administrator** (the only
steps that need it):

```powershell
Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All   # then restart
Add-LocalGroupMember -Group 'Hyper-V Administrators' -Member $env:USERNAME  # then sign out and in
```

Then, in a normal PowerShell window:

```powershell
cd C:\EnBDevelopment\EnB
python tools\build_client.py
```

The project never redistributes anyone else's software (decision D150). The script downloads
Microsoft's and RAD's files from where the project found them, and checks each one against the
hash recorded in the docs before using it.

## What the script does

Each step prints a `== N.` line. If a step fails, the script stops and names the log to read.
Fix the problem and run the script again: it picks up where it stopped.

1. **Checks** that the clone is at `C:\EnBDevelopment\EnB` and that Python, git and OpenSSH are
   installed.
2. **The build VM.** If there is none yet, the script asks, then runs
   `tools\vc6\setup\New-BuildVM.ps1`. That script runs unattended for 30-60 minutes. It downloads
   Visual Studio 6.0 and its Service Pack 4 from the Internet Archive, and a Windows 10 evaluation
   image from Microsoft. Then it makes a VM, installs the compiler and checks that `cl` is build
   8799, the one that compiled the client. Last, it keeps the VM's disk as the base for a pool of
   copies. [setup-vc6.md](setup-vc6.md) explains every part, and has a manual route if you want to
   use your own Windows. Pass `--yes` to skip the question.
3. **The SDKs.** The DirectX 8.0 SDK and RAD's Bink / Miles headers go into `C:\EnBDevelopment\SDK`,
   from the copies the project used ([toolchain.md](toolchain.md) §2). Every file the build reads is
   checked against its SHA-256. Then `tools\vc6\sdk_patch.py` makes the patched Miles headers the
   client was built with (`SDK\radtools\include_enb`).
4. **The import libraries.** `mss32.lib` for Miles, built from the committed `tools\vc6\mss32.def`
   (the exports of the game's own `Mss32.dll`). `authlogin.lib` for EA.Com's login component, built
   from `tools\vc6\authlogin.def`.
5. **The workspace.** `tools\vc6\dsp.py` writes a Visual C++ 6.0 workspace: `app\main\main.dsw`
   with one project per library. The script sends the tree to a VM from the pool and builds the
   workspace with the IDE's own command-line build (`msdev main.dsw /MAKE`), through
   `tools\vc6\msdev.py`. That builds every library first, then **`main`**, the client itself. `main`
   holds only reconstructed source. While the reconstruction is unfinished, its link reports
   the functions nobody has rebuilt yet as *unresolved externals*, and it writes no exe. That list
   is the project's remaining work: 98 symbols on 2026-11-16.
6. **The stand-in build** (`clientpoc`). This is the same libraries and `app\main` sources as `main`,
   plus a *stand-in* for each unresolved function. Some stand-ins are written by hand
   (`programs\clientpoc`, `stubs\`). The rest are generated from the first link's unresolved list
   (`tests\clienttest\gen_stubs.py`), and then the project is linked a second time. On 2026-11-16
   there were 48 generated stand-ins. A generated stand-in records that it was called and returns,
   so the client runs as far as the rebuilt code takes it. Skip this step with `--no-standin`.
7. **The results** are copied to `build\client\`.

The VM is leased from the pool for the run and released at the end. Use `--pool 2` to lease
two VMs. To use a VM you already have, set `ENB_VC6_HOSTS=user@<ip>`.

## What you get

| `build\client\` | |
|---|---|
| `client.exe`, `client.map` | the stand-in build, and its linker map |
| `msdev\main\msdev.log` | the IDE build of every library and of `main`: compiler output, then the unresolved list |
| `msdev\clientpoc\msdev.log` | the stand-in build's log |
| `summary.json` | the unresolved count |
| `logs\` | the import-library steps |

Every project's objects and libraries stay in the tree, in a `client_release\` folder beside the
project (git ignores these folders). Open `app\main\main.dsw` in Visual C++ 6.0 inside the VM to
browse and build the projects in the IDE.

The IDE build gives the same code as the project's byte-matching build (`tools\vc6\build.py`).
Measured on 2026-11-16 over all 1,019 objects: each library's set of exactly matched
`(object, address)` pairs is the same in both builds, 41,062 in all. 998 objects have the same
sections and relocations. 19 differ only in the order of their COMDAT sections. In `tcpqueue.obj` the
IDE build has the client's own `__FILE__` string (`e:\build\beta\G\libs\wwonline\wnet\tcpqueue.cpp`)
where build.py has the clone's path. `ui_avatr.obj` differs in two functions that match the client
in neither build. So what you built is what the progress card measures. To repeat the comparison,
see [Building by hand](#building-by-hand).

## Running what you built

The client needs the game's data: the retail install of Earth & Beyond. The project does not
distribute it. It also needs a server to log in to, such as a Net-7 emulator server. The project's
own tests also replay recorded sessions with a proxy that is not published yet.

1. Make a separate folder for the run, for example `C:\EnBDevelopment\ClientRun\release`. Copy the
   retail install's `release` folder into it, then copy `build\client\client.exe` over the copy's
   `client.exe`. Never build or run inside the game's install folder.
2. Start the server or the replay, then start `client.exe` from that folder.

Where the client reaches a stand-in, it logs the stand-in's name to `stubs.log` in its folder. The
stand-in build keeps the old test hooks out of the exe (D248): the project's tests drive the client
through a `d3d8.dll` proxy (`tools\harness\d3d8shot.c`), and that proxy also drives the retail client.

## Building by hand

The script only chains these tools. You can run each one yourself, for example while working on the
source:

```powershell
python tools\vc6\pool.py lease --agent me            # prints ENB_VC6_HOSTS=user@<ip>
$env:ENB_VC6_HOSTS = 'user@<ip>'
python tools\vc6\dsp.py generate                      # the workspace (dsp.py list: what is in it)
python tools\vc6\msdev.py make --project main         # libraries + main, in the VM
python tools\vc6\msdev.py make --project clientpoc    # the stand-in build
python tools\vc6\msdev.py make --project wwlib        # any one project and what it depends on
python tools\vc6\pool.py release --agent me
```

`--rebuild` compiles everything again. Without it, the IDE compiles only what changed since the
last build in that VM.

The project's own verification still runs on `tools\vc6\build.py` and `tools\vc6\verify.py`
([workflow.md](workflow.md)). To check that the IDE build gives the same objects:

```powershell
python tools\vc6\build.py --out .work\dsp\base               # build.py's objects
python tools\vc6\msdev.py fetch-objs --out .work\dsp\msdev   # the IDE build's objects
python tools\vc6\objcmp.py .work\dsp\base .work\dsp\msdev
```

## How the workspace is laid out

The workspace is generated: `dsp.py` writes it from the tables `build.py` and `verify.py` use
(`tools\vc6\libdefs.py`), so the compile options can't drift apart. The generated files are not
committed. A new source file is in the workspace the next time it is generated.

The layout is our best reconstruction of Westwood's. The client's own evidence:

* The client's debug-information path is `e:\build\beta\G\app\main\client_release\main.pdb`. So
  the exe project is called `main`, lives in `app\main`, and its configuration is `client_release`.
  Every generated project has that one configuration (`Win32 client_release`).
* The client's `__FILE__` strings are `e:\build\beta\G\app\...` and
  `e:\build\beta\G\libs\wwonline\wnet\...`. The repository root stands for `G\`. The VM opens the
  workspace through that same path (`E:` is mapped to it), and the IDE passes it to the compiler,
  so the strings come out the same.
* Renegade's published `commando.dsw` has one static-library project per library folder, and an
  exe project whose link line lists the `.lib` files. The generated workspace follows that: one
  project per Westwood library (`wwlib`, `ww3d2`, ...; `wlib` and `wnet` under `libs\wwonline`),
  one per folder of E&B's own code (`app\client`, `app\shared`, `app\packets`, ...), and `main`
  with `app\main`'s own files.

How E&B's code was split into libraries is a guess. So is each project's file order (alphabetical
in each folder). Neither changes the compiled objects, only the order inside the exe.

## Troubleshooting

* **`the clone must be at C:\EnBDevelopment\EnB`**: clone it there. The VM mirrors that path.
* **The VM step fails**: [setup-vc6.md](setup-vc6.md) §6.
* **`these SDK files are missing or differ`**: a different copy of the SDK is in
  `C:\EnBDevelopment\SDK`. Move it aside and run the script again, or get the copies named in
  [toolchain.md](toolchain.md) §2.
* **A compile error in step 5**: the log names the file. On a clean checkout of `master`, please
  open an issue with the log.
* **The IDE warns `Could not find the file osdep.h`** (or `hash_map`): harmless. The IDE's
  dependency scanner follows `#include` lines that the preprocessor skips.
