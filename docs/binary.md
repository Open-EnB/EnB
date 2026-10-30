# client.exe: facts about the binary

The target is the retail client as installed (Net-7 era install):

| | |
|---|---|
| Path | `C:\Program Files (x86)\EA GAMES\Earth & Beyond\release\client.exe` |
| SHA-256 | `249f852496f341c00361be95b66e7c0becc2c39e7cba026f9227e38b602651c4` (the same client the earlier protocol research used) |
| Size | 8,642,625 bytes (file date 2022-05-06: the install's date, not the build's) |
| PE timestamp | 0x40575E00 = **2004-03-16 18:46:40 UTC** |
| PDB | `e:\build\beta\G\app\main\client_release\main.pdb` (not available) |
| Image base | 0x400000 |

All addresses in these docs are virtual addresses in this image.

## 1. Toolchain

From the Rich header:

| Product id | Build | Count | Meaning |
|---|---|---|---|
| 11 (C++ compiler, VC6) | 8799 | 1054 | **Visual C++ 6.0**, build 8799 = **Service Pack 4**, confirmed by compiling ([toolchain.md](toolchain.md) §1.2): E&B's and Westwood's C++ |
| 10 (C compiler, VC6) | 8797, 8799 | 202 | VC6 C objects |
| 4 | 8447 | 96 | import-library members made by LIB 6.00.8447 (SP4's): 81 mss32 + 9 binkw32 imports + 2x3 descriptors ([toolchain.md](toolchain.md) §1.2, 2026-10-24) |
| 19 | 8022, 8034 | 35 | import libraries |
| 14 | 7299 | 45 | VC6 resource/old linker objects |
| 28, 29 | 8830 | 1, 66 | objects from a newer toolchain: likely DirectX 8 SDK or Miles/Bink static libraries |
| 6 | 1735 | 1 | CVTRES |

Linker 6.0, **incremental** (/INCREMENTAL): the app's functions are reached through a table of
5-byte `jmp` thunks at the start of `.text` (0x401005–0x41964B, about 20,000 stubs).

## 2. Sections

| Section | VA | Size |
|---|---|---|
| `.text` | 0x401000 | 0x6E58DD |
| `.rdata` | 0xAE7000 | 0x84B41 |
| `.data` | 0xB6C000 | 0xD6F14 |
| `.idata` | 0xC43000 | 0x38D9 |
| `.rsrc` | 0xC47000 | 0x1FDC |
| `.reloc` | 0xC49000 | 0x5CB2E |

**Inside `.text`:**

| Range | Contents |
|---|---|
| 0x401005–0x41964B | incremental-link thunk table |
| ~0x429630–0x8C1B50 | E&B's own objects (`G\app\...`), all reached through thunks |
| ~0x8C1B50–0xAE58DD | statically linked libraries, no thunks. From the string-anchored matches: WWMath near 0x8C8000, WW3D2 ~0x909000–0x9C5000, WWLib ~0x9C7000–0x9E9300, WWDebug ~0x9EA000–0x9EC000, WWAudio ~0x9EE000–0xA04000, WWSaveLoad 0xA049C0, wwshade ~0xA09000–0xA10000, then the CRT and the rest. Rough; see [data/funcmatch.md](data/funcmatch.md) |

| 0xA420AC–0xAA1C00 | incremental-link padding (`int3`), 382 KB |
| 0xAA1C00–0xAE2830 | compiler-generated C++ exception-handling stubs for E&B's code (259 KB) |

What of this is precompiled or generated rather than written: [library-matching.md](library-matching.md) §3.7.

Function discovery: thunk targets (app code), plus `call rel32` targets and padding after
`ret` (library code). This finds about 30,000 functions (`tools/re/clientimage.py`).

## 3. Imports

`d3d8.dll` is not imported: WW3D2's `DX8Wrapper::Init` loads `D3D8.DLL` and `Direct3DCreate8`
by name, as in Renegade and Generals. Imported DLLs:

| DLL | Used for |
|---|---|
| `mss32.dll` (81 imports) | Miles Sound System, under WWAudio |
| `binkw32.dll` | Bink video |
| `WS2_32.dll` | sockets (`app\common\tcpconn.cpp`, `udpconn.cpp`, `app\wwudp`) |
| `authlogin.dll` (`AuthLoginLib_GetComponentFactory`) | the login component (replaced by Net-7's) |
| `WININET`, `urlmon`, `ole32`, `OLEAUT32` | the in-game browser (`BrowserEngine.dll`) and HTTP |
| `DDRAW.dll` (`DirectDrawCreate`) | WWLib's `ddraw.cpp` (video card detection) |
| `AVIFIL32.dll` | WW3D2's `FrameGrabClass` (movie capture) |
| `KERNEL32`, `USER32`, `GDI32`, `ADVAPI32`, `WINMM`, `SHELL32`, `COMCTL32` | Windows |

## 4. Strings and RTTI

* 31,661 printable strings of 5+ characters.
* 472 source paths under `e:\build\beta\G\` ([source-tree.md](source-tree.md)).
* 230 distinct `Class::Method` names in strings ([data/class-methods.txt](data/class-methods.txt)).
* 87 RTTI type descriptors ([data/rtti.txt](data/rtti.txt)): about 30 of them are E&B's own
  (`ClientObjectClass`, `GadgetClass`, `SlotDataClass`, `GClass`, `CameraModuleClass`, the
  `...Event` classes), the rest the C++ library. So the client was compiled with /GR, but RTTI
  descriptors were emitted only for classes used with `dynamic_cast` or `typeid`.
* Assert expressions with their source file: [data/asserts.txt](data/asserts.txt). The file
  attribution is approximate: strings follow object order, and a file's first assert expression
  can come just before its `__FILE__` string, so it gets attributed to the previous file.
