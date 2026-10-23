# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

### 2026-10-22: byte matching

* `tools/vc6/vc6.py` compiles in the VM; `tools/re/objmatch.py`, `objdiff.py`, `matchreport.py`
  match compiled functions against the client.
* The client's libraries were compiled with `/MT /O2 /Ob2`, no `/GX`, no `/G6`: Renegade's
  release flags.
* **2,708 client functions proven byte-identical** to EA source (WW3D2 1,502, WWLib 553, WWMath
  369, WWAudio 175, WWSaveLoad 80, WWDebug 66, wwshade 46): [library-matching.md](library-matching.md) §3.5.
  First pass compared Renegade and Zero Hour only (2,650) and compiled Zero Hour against Renegade's
  `wwdebug` headers because Zero Hour's `wwdebug.h` includes its game engine's `debug.h`, which the
  wrapper didn't copy to the VM. The user pointed out the Generals and Zero Hour trees are both in
  the clone; rerun with all three copies, each against its own libraries (`bytematch_all.sh`).
* Correction: WW3D2 is mostly **Renegade's** core with some later features that Zero Hour also
  has, not Zero Hour's (the string census only saw the new features). WWMath and WWAudio are
  Renegade's; WWSaveLoad and wwshade Zero Hour's; WWLib is mixed per file.
* `RC4Class` is E&B's own variant with inline assembly (§3.6).

### 2026-10-22: VC6 build VM

* VM up (Windows 10, SSH with key `~/.ssh/enb_vc6`), base VC6 then SP5 installed and tested
  ([toolchain.md](toolchain.md) §1.1). Both compile, link and run a test program.
* Neither is the client's compiler: RTM stamps build 8168, SP5 stamps 8966, the client has 8799.
* **SP4 is the client's compiler**: reverted to the RTM checkpoint, installed SP4, and it stamps
  8799 like the client, with the same CRT build mix ([toolchain.md](toolchain.md) §1.2).

### 2026-10-22: decisions D1–D5; SDKs checked

* D1–D5 settled (§2).
* DirectX 8 and RAD SDKs cloned to `C:\EnBDevelopment\SDK` and checked against the client
  ([toolchain.md](toolchain.md)): the client used DirectX **8.0** (`Direct3DCreate8(120)`), as the
  SDK; Bink's 9 imports all match; Miles' header is right but its import library is older than the
  game's `Mss32.dll` (30 of 81 imports missing), so we generate the import library from the DLL.

### 2026-10-22: project started

* Repository created at `C:\EnBDevelopment\EnB`, laid out after the original tree
  ([source-tree.md](source-tree.md)).
* EA's Renegade and Generals/Zero Hour repositories cloned to `C:\EnBDevelopment\EA`.
* Facts about the binary recorded ([binary.md](binary.md)): VC6 SP4 (build 8799, toolchain.md), incremental link, built
  2004-03-16, PDB `G\app\main\client_release\main.pdb`.
* The original source tree recovered from `__FILE__` strings: 472 paths
  ([source-tree.md](source-tree.md), [data/source-paths.txt](data/source-paths.txt)).
* Coding conventions written from the client's own strings and EA's Westwood code
  ([conventions.md](conventions.md)).
* First library matching ([library-matching.md](library-matching.md)): WW3D2, WWLib, WWDebug and
  wwshade follow **Zero Hour**; WWAudio follows **Renegade**; wwphys, wwnet and wwui are not
  in the client. 300 client functions matched by strings (about 170 that are not duplicates).
  Zero Hour's WW3D2 contains E&B code (`..\data\client\mixfiles\`, `global.th6`).
* WWLib did drift between the titles (about one file in five has code changes); the client's
  `rc4.cpp` is Zero Hour's.
* Tools in `tools/re` ([tools/re/README.md](../tools/re/README.md)).

## 2. Decisions

| # | Date | Decision | Why |
|---|---|---|---|
| A1 | 2026-10-22 | The repository root stands for `G\`; directories as in the original, lower case | the original layout is known; mirroring it keeps include paths (`..\..\wwlib\ref_ptr.h`) meaningful |
| A2 | 2026-10-22 | Names come from the client first (strings, RTTI, asserts), then from Westwood convention | the client is the only source of E&B's own names |
| A3 | 2026-10-22 | EA clones live outside the repository, at `C:\EnBDevelopment\EA`; SDKs at `C:\EnBDevelopment\SDK` | they are references; what we import is copied in with its origin recorded |
| D1 | 2026-10-22 | The repository is GPL v3 (EA's code, with EA's additional terms) | user |
| D2 | 2026-10-22 | Install Visual C++ 6.0 as a tool for proving our work (byte-for-byte comparison) | user. Not installed yet: media needed ([toolchain.md](toolchain.md) §1) |
| D3 | 2026-10-22 | The code builds with both VC6 and modern MSVC | user |
| D4 | 2026-10-22 | Import EA code file by file, as each file is verified against the client | user |
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |

## 3. Open decisions

None. D1–D5 were settled on 2026-10-22 (table §2).

## 4. Next steps

1. Start importing, file by file (D4): take the files where every function matches, from the
   copy that matches, into `wwlib/`, `ww3d2/`, ...; record each in `docs/imports.md` with its
   origin and match counts.
2. Files with some unmatched functions: diff them (`objdiff.py`) and reconstruct E&B's version
   until byte-exact. First candidate: `rc4.cpp` (library-matching.md §3.6).
3. Vtable naming: find the vtables of the matched WW3D2/WWLib/WWAudio classes and name every slot
   from the EA headers (library-matching.md §5.1).
4. Call-graph propagation from the matched functions (§5.2).
5. Start E&B's own code where prior work is strongest: `app\shared\auxdata` (the Delta classes;
   the the reference decoder research has the address catalogue, field model and wire format),
   `app\packets` (the packet classes, against the reference decoder's message models), and the task
   kernel (`app\kernel`, `app\client\tasks`), which gives the program's skeleton.
