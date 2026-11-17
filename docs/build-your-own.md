# Building your own client

This page takes you from a fresh copy of the repository to a `client.exe` compiled from its source,
with the compiler Westwood used: **Visual C++ 6.0 Service Pack 4**, running in a Hyper-V virtual
machine on your PC. One PowerShell script does every step:

```powershell
powershell -ExecutionPolicy Bypass -File tools\Build-Client.ps1
```

Your PC needs only Windows with Hyper-V. The build tools are Python, but they run inside the VM, and
the script puts a Python there; nothing is installed on your PC. The first run takes an hour or two,
most of it making the build VM. After that, a full rebuild of every library takes about three
minutes, and a run after updating the source compiles only what changed.

Contents: [What you need](#what-you-need) · [Downloads and licences](#downloads-and-licences) ·
[What the script does](#what-the-script-does) · [What you get](#what-you-get) ·
[Running what you built](#running-what-you-built) · [Building by hand](#building-by-hand) ·
[How the workspace is laid out](#how-the-workspace-is-laid-out) · [Troubleshooting](#troubleshooting)

## What you need

| | |
|---|---|
| Windows | Windows 10 or 11 **Pro, Enterprise or Education** (Hyper-V), 64-bit, virtualization enabled in the firmware |
| Memory and disk | 16 GB of memory; about 60 GB of free disk |
| Software | Windows PowerShell and the OpenSSH client (both part of Windows: Settings > System > Optional features > OpenSSH Client), and [7-Zip](https://www.7-zip.org/) for unpacking the compiler's disc images |
| The source | at **`C:\EnBDevelopment\EnB`**: `git clone https://github.com/Open-EnB/EnB C:\EnBDevelopment\EnB`, or GitHub's "Download ZIP" unpacked there. The VM sees `C:\EnBDevelopment` at the same path, so include paths and the client's `__FILE__` strings come out the same |
| Internet | for the downloads below |

Prepare Hyper-V once, in a PowerShell window opened with **Run as administrator** (the only
steps that need it):

```powershell
Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All   # then restart
Add-LocalGroupMember -Group 'Hyper-V Administrators' -Member $env:USERNAME  # then sign out and in
```

Then, in a normal PowerShell window:

```powershell
cd C:\EnBDevelopment\EnB
powershell -ExecutionPolicy Bypass -File tools\Build-Client.ps1
```

## Downloads and licences

Open-EnB distributes only its own source (decision D150). Everything else the build needs is
someone else's software, which you download yourself. Before each download, the script lists what
it will fetch and from where, shows this notice, and asks you to answer `y`:

> These packages are third-party software, downloaded from the addresses shown. Open-EnB does not
> distribute them, grants no licence to them, and makes no promise about them. Make sure you have
> the right to download and use them before you continue.

| What | From | Checked by | Asked |
|---|---|---|---|
| Visual Studio 6.0 Professional (disc 1) and its Service Pack 4 | the Internet Archive, items X04-14444 and X05-86594 | SHA-1 of each archive, then every installed file ([setup-vc6.md](setup-vc6.md) §7) | when the VM is made |
| Windows 10 Enterprise Evaluation (Hyper-V Quick Create's MSIX packaging environment) | Microsoft | SHA-256 | when the VM is made (not with your own Windows disk, setup-vc6.md §4) |
| Python 3.12 embeddable package | python.org | SHA-256 | the first time the VM needs it; it goes into the VM only |
| DirectX 8.0 SDK, RAD's Bink and Miles headers and libraries | GitHub (`edgeforce/directx8`, `edgeforce/radtools`), at the commits the project used | SHA-256 of every file the build reads ([toolchain.md](toolchain.md) §2) | when `C:\EnBDevelopment\SDK` lacks them |

Answer `n` and nothing is downloaded; the script stops and says how to supply that item yourself.
For example, put your own copy of the SDKs in `C:\EnBDevelopment\SDK\directx8` and `...\radtools`:
the script checks their hashes, not where they came from. `-AcceptDownloads` answers yes to every
question, for a run nobody watches.

## What the script does

Each step prints a `== N.` line. If a step fails, the script stops and says why. Fix the problem
and run the script again: finished steps are skipped.

1. **Checks** that the source is at `C:\EnBDevelopment\EnB`, Hyper-V is on, and OpenSSH is there.
2. **The build VM.** If there is none yet, `tools\vc6\setup\New-BuildVM.ps1` makes one (30-60
   minutes, unattended after its question). It downloads the compiler and Windows, makes a VM,
   installs Visual C++ 6.0 SP4 and checks that `cl` is build 8799, the one that compiled the client.
   [setup-vc6.md](setup-vc6.md) explains every part, and has a manual route for your own Windows.
   Then the script starts the VM and reaches it over SSH.
3. **The SDKs** in `C:\EnBDevelopment\SDK`: downloaded if missing (asked first), then every file the
   build reads is checked against its SHA-256.
4. **Python in the VM**, in `C:\EnBTools\Python` there (asked first, the first time).
5. **The source and the SDKs are copied into the VM**, at the same paths. The VM's build outputs stay,
   so a later run compiles only what changed.
6. **The build, in the VM** (`tools\build_client.py --local`):
   * the import libraries: `mss32.lib` for Miles, from the committed `tools\vc6\mss32.def` (the
     exports of the game's own `Mss32.dll`), and `authlogin.lib` for EA.Com's login component, from
     `tools\vc6\authlogin.def`;
   * **the workspace**: `tools\vc6\dsp.py` writes a Visual C++ 6.0 workspace, `app\main\main.dsw`
     with one project per library, and `tools\vc6\msdev.py` builds it with the IDE's own
     command-line build (`msdev main.dsw /MAKE`): every library, then **`main`**, the client itself.
     `main` holds only reconstructed source. While the reconstruction is unfinished, its link reports
     the functions nobody has rebuilt yet as *unresolved externals*, and it writes no exe. That list
     is the project's remaining work: 98 symbols on 2026-11-16;
   * **the stand-in build** (`clientpoc`): the same libraries and `app\main` sources as `main`, plus
     a *stand-in* for each unresolved function. Some stand-ins are written by hand
     (`programs\clientpoc`, `stubs\`). The rest are generated from the first link's unresolved list
     (`tests\clienttest\gen_stubs.py`), and then the project is linked a second time. On 2026-11-16
     there were 48 generated stand-ins. A generated stand-in records that it was called and returns,
     so the client runs as far as the rebuilt code takes it. `-NoStandin` skips it.
7. **The results** are copied back to `build\client\` on your PC. If the script started the VM, it
   shuts it down again (`-KeepRunning` leaves it on).

## What you get

| `build\client\` | |
|---|---|
| `client.exe`, `client.map` | the stand-in build, and its linker map |
| `msdev\main\msdev.log` | the IDE build of every library and of `main`: compiler output, then the unresolved list |
| `msdev\clientpoc\msdev.log` | the stand-in build's log |
| `summary.json` | the unresolved count |
| `logs\` | the import-library steps |

Every project's objects and libraries stay in the VM's copy of the tree, in a `client_release\`
folder beside each project. To browse and build the projects in the IDE, open
`C:\EnBDevelopment\EnB\app\main\main.dsw` in Visual C++ 6.0 inside the VM (`vmconnect localhost
enb-vc6-template`).

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

The script only chains these tools. With Python 3.10+ and git on your PC (the maintainer's set-up,
[setup-vc6.md](setup-vc6.md) §2 with the pool), you can run each one from there, for example while
working on the source; they then reach the VM over SSH:

```powershell
python tools\build_client.py                          # what Build-Client.ps1 runs in the VM, from the host
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

* **`cannot be loaded because running scripts is disabled`**: start it as shown, with
  `powershell -ExecutionPolicy Bypass -File ...`.
* **`the clone must be at C:\EnBDevelopment\EnB`**: move the source there. The VM mirrors that path.
* **The VM step fails**: [setup-vc6.md](setup-vc6.md) §6.
* **`these SDK files are missing or differ`**: a different copy of the SDK is in
  `C:\EnBDevelopment\SDK`. Move it aside and run the script again, or get the copies named in
  [toolchain.md](toolchain.md) §2. Line endings don't matter: a header's hash is taken with CRLF
  line ends.
* **A compile error in step 6**: `build\client\msdev\main\msdev.log` names the file. On an
  unchanged copy of `master`, please open an issue with the log.
* **The IDE warns `Could not find the file osdep.h`** (or `hash_map`): harmless. The IDE's
  dependency scanner follows `#include` lines that the preprocessor skips.
