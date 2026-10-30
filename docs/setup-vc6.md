# Setting up the VC6 build VM

The project proves its work by compiling with the compiler that built client.exe, **Visual C++ 6.0
Service Pack 4** (C++ back end build 8799; [toolchain.md](toolchain.md) §1.2), and comparing the
objects with the client byte for byte. VC6 runs in a Hyper-V virtual machine on your Windows PC; the
tools in `tools/vc6` reach it over SSH. This page sets that VM up, by one script (§2) or by hand
(§4). The project never redistributes Microsoft's software: you download it from the Internet
Archive and Microsoft, and the scripts check every download against its published hash (D146).

Contents: §1 requirements · §2 the scripted route · §3 what you get · §4 the manual route ·
§5 renewing the Windows evaluation · §6 troubleshooting · §7 how the install was derived.

## 1. Requirements

| | |
|---|---|
| Host | Windows 10 or 11 **Pro, Enterprise or Education** (Hyper-V), 64-bit, virtualization enabled in the firmware |
| Memory | 16 GB minimum; 32 GB for a pool of several VMs (each clone starts with 2 GB and settles near 1.1 GB) |
| Disk | about 60 GB: 4.7 GB Windows image download, 1.3 GB VC6 media (plus 1.2 GB of ISOs), 15-20 GB per VM disk |
| Account | a member of the **Hyper-V Administrators** group (no elevation needed after that) |
| Software | Python 3.10+, [7-Zip](https://www.7-zip.org/), the OpenSSH client (Settings > Optional features), Windows PowerShell 5.1 (built in), git |
| Repository | cloned to **`C:\EnBDevelopment\EnB`**: the VM mirrors `C:\EnBDevelopment` at the same path, so include paths and `__FILE__` strings match |
| Network | internet access for the host and the VM (the VM installs the OpenSSH Server capability from Windows Update) |

One-time host preparation, in an **elevated** PowerShell (the only steps that need it):

```powershell
Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All   # then restart
Add-LocalGroupMember -Group 'Hyper-V Administrators' -Member $env:USERNAME  # then sign out and in
```

## 2. The scripted route (recommended; an agent can run it)

From the repository root, in a normal (not elevated) PowerShell window:

```powershell
powershell -ExecutionPolicy Bypass -File tools\vc6\setup\New-BuildVM.ps1
```

It runs unattended, 30-60 minutes (mostly downloads), and stops with an error message at the first
problem. What it does:

1. **The VC6 media** (`tools/vc6/setup/media.py`, into `C:\EnBDevelopment\Tools\Media`): downloads
   the two Internet Archive items, checks their SHA-1, unpacks them with 7-Zip and turns each disc
   image (MDF) into an ISO that Hyper-V attaches directly; no mounting tool is needed.

   | Disc | Internet Archive item | File | SHA-1 |
   |---|---|---|---|
   | Visual Studio 6.0 Professional, disc 1 | [X04-14444](https://archive.org/details/X04-14444) | `1999-02_X04-14444_X04-14444_VSP600ENU1.7z` | `50a0674f8e881410c3f35fc7e09bb39213167797` |
   | Visual Studio 6.0 Service Pack 4 | [X05-86594](https://archive.org/details/X05-86594) | `2000-08_X05-86594_X05-86829_vs6sp4.7z` | `7ba4001a8e6a3af8f9807d85471b5ea83000eae8` |

   Already downloaded them? Put the two `.7z` files in the media folder first; they are used if
   their hashes match.
2. **Windows**: Microsoft's *Windows 10 MSIX packaging environment* image, the one Hyper-V's Quick
   Create gallery offers, downloaded directly from Microsoft (4.7 GB, SHA-256 checked). It is
   Windows 10 Enterprise **Evaluation**: 90 days from first boot (§5). Your own generalized
   Windows 10 disk can replace it: `-WindowsVhdx PATH`.
3. **A seed ISO**: an answer file (`autounattend.xml`: the VM account, no out-of-box screens, a
   one-time logon that starts the setup) and the guest scripts. The account's password is random;
   the script stores it for you, encrypted to your Windows account, in `.work\<vm>.cred.xml`, and
   deletes the seed when setup is done.
4. **The VM** `enb-vc6-template` (generation 2, Secure Boot, 8 virtual processors, dynamic memory,
   Default Switch) with the Windows disk and three DVDs: seed, VC6, SP4. On first boot
   `guest\Setup-Guest.ps1` installs the OpenSSH Server with your key (`~\.ssh\enb_vc6`, created if
   missing), installs VC6 SP4 (`guest\Install-VC6.ps1`, §7), and applies the build VM settings
   (Windows Update, SysMain and Search off, Defender exclusions, fixed page file).
5. **The check** (`tools/vc6/setup/check_vm.py`): `cl` answers *12.00.8804*, `link` *6.00.8447*,
   and a compiled object carries `@comp.id` product 11, **build 8799**. Anything else stops here.
6. **The pool's base**: the VM is shut down, its DVDs removed, and its disk copied to
   `C:\EnBDevelopment\Tools\VMPool\base-vc6.vhdx` (read-only). Pool clones are differencing disks
   over it ([toolchain.md](toolchain.md) §1.3). Skip with `-NoBase` to keep a single VM only.
7. **Settings**: `.work\vc6_config.json` (account, key, pool folder, template VM; read by
   `tools/vc6/vc6config.py`).

Options: `-Name`, `-User` (default `user`), `-PoolDir`, `-MediaDir`, `-Prefix` (pool VM names),
`-Cpu`, `-StartupGB`, `-ConfigFile`, `-ReplaceBase` (rebuild the base: destroy the pool's clones
first), `-TimeoutMinutes`. A second, separate pool (for a test) takes its own `-Name`, `-PoolDir`,
`-Prefix` and `-ConfigFile`; point the tools at it with `ENB_VC6_CONFIG`.

Then try it:

```powershell
python tools\vc6\pool.py lease --agent me          # starts a clone, prints ENB_VC6_HOSTS=user@ip
$env:ENB_VC6_HOSTS = 'user@<ip>'
python tools\vc6\setup\check_vm.py                  # PASS
python tools\vc6\importer.py verify --lib wwdebug   # a library's pairs, as in docs/workflow.md
python tools\vc6\pool.py release --agent me
```

## 3. What you get

* **One template VM** (`enb-vc6-template`, normally off): its disk holds Windows and VC6; its
  hardware settings are what `pool.py` copies into each clone. You can start it and use it as a
  single build VM (`python tools\vc6\vc6.py discover` finds it); the base is a separate copy of
  its disk, so the clones are not affected.
* **The pool**: `pool.py lease` makes clones on demand (`enb-vc6-01`, ...), up to `max_vms` in
  `.work\vc6_config.json` (default 7) and only while the host keeps `reserve_gb` (default 4 GB)
  free after a clone's 2 GB start. Lower `max_vms` on a 16 GB host (2-3).
* **SSH**: `ssh -i ~\.ssh\enb_vc6 user@<ip>`; the VM's console: `vmconnect localhost enb-vc6-template`
  (the password: `(Import-Clixml .work\enb-vc6-template.cred.xml).GetNetworkCredential().Password`).

## 4. The manual route

For a Windows you install yourself, or if the image download is no longer offered.

1. **Windows 10 in a Hyper-V VM.** Any 64-bit Windows 10 works (the existing project VMs run
   21H2, build 19044). Options:
   * Microsoft's [Evaluation Center](https://www.microsoft.com/en-us/evalcenter/): *Windows 10
     Enterprise LTSC 2021*, ISO, 90-day evaluation (a free Microsoft account sign-in).
   * Hyper-V Manager > Quick Create > *Windows 10 MSIX packaging environment* (the same image §2
     downloads).
   * Your own licensed Windows 10 media.
   Create a local administrator account (any name; tell the tools with `"user"` in
   `.work\vc6_config.json`). Give the VM internet access (Default Switch).
2. **The media**: `python tools\vc6\setup\media.py --media C:\EnBDevelopment\Tools\Media --download`,
   then in Hyper-V Manager attach `vc6pro-disc1.iso` and `vs6sp4.iso` as DVD drives (VM Settings >
   SCSI Controller > DVD Drive). *Alternative*: mount the original MDF files inside the VM with
   [WinCDEmu](https://wincdemu.sysprogs.org/); to eject a WinCDEmu disc use Disk Management
   (`diskmgmt.msc`, right-click the drive > Eject): Explorer's Eject did not work in our tests.
3. **Copy the setup folder** into the VM as `C:\EnBSetup`: `tools\vc6\setup\guest\*.ps1`,
   `tools\vc6\setup\vc6-files.tsv`, and your public key as `enb_vc6.pub` (make the key with
   `ssh-keygen -t ed25519 -N "" -f $HOME\.ssh\enb_vc6`). Hyper-V's `Copy-VMFile` or an enhanced
   session's clipboard both work.
4. **Run it** in the VM, in an elevated PowerShell:
   `powershell -ExecutionPolicy Bypass -File C:\EnBSetup\Setup-Guest.ps1`. It ends with
   `build VM setup done` (or writes `C:\EnBSetup\failed.txt`).
   *The original installers instead*: VC6's `SETUP.EXE` (Custom; clear everything except Visual
   C++ 6.0; no product key was asked for), then SP4's `SETUPSP4.EXE`. That is how the reference VM
   was made; §7 shows the file copy gives the same compiler.
5. **On the host**: `python tools\vc6\vc6.py discover`, then `python tools\vc6\setup\check_vm.py`
   must print PASS. For a pool, shut the VM down and copy its disk to
   `C:\EnBDevelopment\Tools\VMPool\base-vc6.vhdx` (read-only), and set `"template_vm"` to its name.

## 5. Renewing the Windows evaluation

The evaluation image runs 90 days from its first boot (`slmgr /xpr` in the VM shows the date);
after that Windows shuts itself down every hour. Every clone inherits the base's date. Renewing is
a rebuild, not a repair:

```powershell
python tools\vc6\pool.py destroy --all-free                     # clones are children of the base
Remove-VM enb-vc6-template -Force                               # and delete its folder in the pool dir
powershell -ExecutionPolicy Bypass -File tools\vc6\setup\New-BuildVM.ps1 -ReplaceBase
```

The media and the Windows image stay in the media folder, so a rebuild takes 20-30 minutes. A
licensed Windows (`-WindowsVhdx`, or §4) does not expire.

## 6. Troubleshooting

| Symptom | Cause and fix |
|---|---|
| `media.py`: SHA-1 differs | an interrupted or changed download: delete the `.7z` and rerun |
| The script waits at "waiting for Windows setup" for more than 20 minutes | watch the console: `vmconnect localhost enb-vc6-template`. An out-of-box screen means the answer file was not read (a different Windows image): finish the screens by hand with the account `user` and the password from §3, then run `C:\EnBSetup\Setup-Guest.ps1` (§4 step 4) |
| `guest setup failed: ... OpenSSH` | the VM had no internet: check the Default Switch, rerun `C:\EnBSetup\Setup-Guest.ps1` in the VM |
| `Install-VC6.ps1`: disc not attached | attach both ISOs (§4 step 2); the labels must be `VSP600ENU1` and `VS6SP4` |
| `check_vm.py`: build 8798 or 8966 | another VC6 (an Introductory edition's back end, or SP5) is first on the VM's PATH |
| `ssh: connect ... timed out` after a host restart | the Default Switch changed subnet: `python tools\vc6\vc6.py discover` |
| `pool.py`: host memory | raise `reserve_gb`/lower `max_vms` in `.work\vc6_config.json`, or stop other VMs |

## 7. How the install was derived

The reference VM (2026-10-22) was installed with VC6's own setup (Custom, Visual C++ only) and SP4's
setup. Its `C:\Program Files (x86)\Microsoft Visual Studio` holds 2,100 files. Each was matched to
its source on the two discs by content, or through the setup's INF files where setup renames a file
(SP4's `c2.dll`, the back end that stamps build 8799, is `enu\os\system\msvcep.dll` on the disc):

* 2,024 files are byte-identical to their source;
* 74 differ only because setup *binds* executables to the system DLLs it finds (the PE checksum,
  the import hints and the bound-import table: load-time bookkeeping). Unbound, 71 are identical;
  the other three are IDE files the build never runs (`MSDEV.EXE` and `DEVCPP.PKG` carry setup's
  registration stamp, `MAPSYM.EXE` one section flag). `Install-VC6.ps1` copies the unbound source;
* 2 are written by setup: `VCVARS32.BAT` (the script writes the same file) and setup's own
  `setup.stf` (not needed).

The list is `tools/vc6/setup/vc6-files.tsv` (paths and hashes only). The compiler, linker,
headers and libraries a build uses are therefore the same bytes as the reference VM's, and
`check_vm.py` proves the result per VM.

**Tested end to end on 2026-10-29** (a separate test pool, `-Prefix enb-vc6t-`): `New-BuildVM.ps1`
ran unattended in 5.5 minutes with the downloads already in the media folder (Windows 10 21H2, build
19044.1288; cl 12.00.8804, `@comp.id` build 8799); `pool.py lease` made a clone of the new base, and
`importer.py verify` on it gave exactly the recorded results: wwdebug 107 client functions (65 by
the best copy alone, 3 ambiguous), ww3d2 2,983 (1,323, 87), no file changed.
