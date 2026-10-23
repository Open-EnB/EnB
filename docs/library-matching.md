# Matching EA's published Westwood code to client.exe

Goal: find the code in client.exe that EA has published (Renegade, Generals, Zero Hour), so we can
import it instead of reverse engineering it. This page is the method, the results so far, and
the plan for importing.

## 1. Sources

| Copy | Tag | Repository | Commit | Libraries used for matching |
|---|---|---|---|---|
| Renegade (2002) | R | github.com/electronicarts/CnC_Renegade | 3e00c3a (2025-03-03) | `Code/wwlib`, `ww3d2`, `WWMath`, `WWAudio`, `wwdebug`, `wwsaveload`, `wwnet`, `wwphys`, `wwutil`, `wwui`, `wwbitpack`, `wwtranslatedb`, `WWOnline`, `BinkMovie` |
| Generals (2003) | G | github.com/electronicarts/CnC_Generals_Zero_Hour (`Generals/`) | 0a05454 (2025-02-27) | `Code/Libraries/Source/WWVegas/*` |
| Zero Hour (2003) | Z | same repository (`GeneralsMD/`) | 0a05454 | `Code/Libraries/Source/WWVegas/*` (adds `wwshade`) |

Clones are at `C:\EnBDevelopment\EA\` (outside this repository). Both are GPL v3 with EA's
additional terms (§6). Third-party SDK code (Miles, Bink, DirectX, GameSpy, ...) is not in them.

## 2. Method

1. **String census** (`tools/re/libversions.py`). Every string literal, `#define` string and
   assert expression in each EA file is looked up among client.exe's strings. Release builds of
   both E&B and Renegade kept their asserts, so assert expressions like
   `WWMath::Is_Valid_Float(radius)` survive, and they are long and specific.
2. **Version vote.** Where a file exists in more than one copy, the strings only one copy has, found
   in the client, show which copy the client's file descends from
   ([data/library-versions.md](data/library-versions.md)).
3. **String-anchored function matching** (`tools/re/funcmatch.py`). Each EA string is attributed
   to the function it's in (`tools/re/easource.py`'s rough C++ parser). Each reference to the
   string in client.exe's code is attributed to the client function containing it. A client
   function that uses the same strings as one EA function is matched to it, scored by how specific
   the strings are ([data/funcmatch.md](data/funcmatch.md), [data/funcmatch.json](data/funcmatch.json)).

Limitations, which decide the next steps (§5):

* Only functions with a string are anchored. Most library functions have none.
* `shared` matches are several client functions claiming one EA function: inlined copies, or
  helpers duplicated by the compiler. Only the best is a candidate for the function itself.
* Matches in the app region (below ~0x8C1B50) against generic strings (`Default`, `render`,
  `System`) are noise. The library region is where these matches mean something.

## 3. Results (2026-10-22)

### 3.1 Which libraries are in the client

Strings gave the first answer (2026-10-22, morning); byte matching (§3.5) corrected it the same
day. The "Closest copy" column is the byte-matching answer.

| Library | In client.exe? | Closest copy | Evidence |
|---|---|---|---|
| **WW3D2** | yes, ~0x909000–0x9C5000 | **Renegade's core, with later features that Zero Hour also has**: per file (§3.5) | Bytes: 1,476 functions; 585 only Renegade's copy explains (`meshgeometry`, `aabtree`, `htree`, `motchan`, `dx8vertexbuffer`, `render2dsentence`, ...), 154 only Zero Hour's (`dx8wrapper`, `dazzle`, `textureloader`, `vertmaterial`, ...). Strings: Z-only strings in `htree.cpp` (`HTreeClass::Alter_Avatar_HTree` and its bone names), `texture.cpp`, `textureloader.cpp`, `texturethumbnail.cpp`, `dx8caps.cpp` (Parhelia), `dx8fvf.cpp`, `ww3dformat.cpp`. G-only: `dx8wrapper.cpp`'s `RenderDeviceName`, `RenderDeviceWidth`, ... registry values. 36 Z votes, 6 G, 1 R |
| **WWLib** | yes, ~0x9C7000–0x9E9300 | **mixed, per file**: `ini`, `mixfile` follow Zero Hour; `rawfile`, `multilist`, `wwstring`, `widestring` follow Renegade; `rc4` is a third version (§3.6) | Bytes: 548 functions (25 only R, 15 only Z). Strings: `cpudetect.cpp` (82 strings), `except.cpp` (91), `ini.cpp`, `mixfile.cpp`, `ddraw.cpp`, `registry.cpp`; `RC4Class::Print_State` (0x9E03B0), which only Zero Hour's `rc4.cpp` has |
| **WWAudio** | yes, ~0x9EE000–0xA04000 | **Renegade** | Bytes: 175 functions, 94 only R, 4 only Z. Strings: G and Z gutted WWAudio (Generals used its own audio). The client has Renegade's full `WWAudioClass` (`Create_3D_Sound`, `Cache_Buffer`, `WWAudio.ini`, volume settings): 27 R-only strings |
| **WWDebug** | yes, ~0x9EA000–0x9EC000 | **Zero Hour** (66 functions, 32 only Z, 1 only R) | `wwprofile.cpp`'s frame logging (`FRAME: %d %2.2f %s`), `wwmemlog.cpp` |
| **WWMath** | yes (few strings; `CullableClass::Set_Cull_Box` at 0x8C8DD0) | **Renegade** (369 functions, 51 only R, 3 only Z) | `WWMath::Is_Valid_Float`, `WWMATH_EPSILON` asserts throughout E&B's code |
| **WWSaveLoad** | yes (`DefinitionMgrClass` at 0xA049C0) | **Zero Hour** leaning (80 functions, 31 only Z, 5 only R) | E&B also has `app\shared\definition_manager.h` |
| **wwshade** | yes, ~0xA09000–0xA10000 | **Zero Hour** (only copy; 46 functions byte-exact) | `ShdHWShader::Shell_Run` (`\shaders\`, `Shader Asset Error`), `ShdMeshClass::Render`. wwshade exists only in Zero Hour |
| wwphys | **probably not** | | The hits (`PhysicsSceneClass` texture projectors, `RigidBodyClass::Assert_State_Valid`) are in the app region. E&B has its own physics (`app\shared\px*.cpp`) and scene (`enbscene.cpp`); its projector code may be adapted from wwphys's `pscene` |
| wwnet, wwui, wwutil, wwtranslatedb, wwbitpack | **no** | | only generic strings. E&B has its own UDP layer (`app\wwudp`) and UI (gadgets) |
| WWOnline | partly | | E&B has `libs\wwonline\wnet\tcpqueue.cpp`, not Renegade's `WWOnline` (`Product::Product` is a generic hit) |

(The address ranges are approximate, from where the matched functions are.)

### 3.2 E&B and Zero Hour share WW3D2 code

Zero Hour's `WW3D2/texturethumbnail.cpp` contains the string `..\data\client\mixfiles\`. That is
an E&B path (the install's data is in `Data\client\mixfiles`), and the client has the same
string. Zero Hour's `texturethumbnail.h` names `global.th6`, and `Data\client\mixfiles\global.th6`
is in the E&B install, beside a `.th6` for each `.mix`. So
Zero Hour's WW3D2 carries code written for E&B, or both took it from a WWVegas branch they shared.
Byte matching (§3.5) shows the sharing is selective: E&B's WW3D2 is Renegade's core plus
features that also reached Zero Hour (texture thumbnails, `HTreeClass::Alter_Avatar_HTree`,
`DX8Caps` updates). Zero Hour's later changes to the core (meshes, animation, vertex buffers)
are not in E&B's.

### 3.3 How much the EA copies differ from each other

`tools/re/libdiff.py` compares two copies file by file, ignoring comments, whitespace and
banners. WWLib:

| Pair | Same code | Changed | Only in first | Only in second | Biggest changes (lines) |
|---|---|---|---|---|---|
| Renegade → Generals | 157 | 65 | 39 | 1 | `registry.cpp` 251, `ini.cpp` 189, `widestring.h` 91, `cpudetect.cpp` 76 |
| Generals → Zero Hour | 177 | 45 | 1 | 44 | `registry.cpp` 251, `ini.cpp` 183, `rc4.cpp` 155, `widestring.h` 89 |
| Renegade → Zero Hour | 203 | 57 | 1 | 6 | `rc4.cpp` 155, `cpudetect.cpp` 76, `targa.cpp` 67, `convert.cpp` 58 |

So WWLib was mostly stable, but not frozen: Generals' copy is the odd one out (an older branch
point; `registry.cpp` and `ini.cpp` differ most), and Renegade and Zero Hour are close. "WWLib
didn't change" holds for about four files in five. The rest have to be checked against the
client file by file; the crypto E&B depends on (`rc4.cpp`) is one of them, and the client has
Zero Hour's.

### 3.4 Functions matched by strings

300 client functions matched by strings; by confidence (as of this writing): WW3D2 29 high,
61 medium; WWLib 11 high, 8 medium; WWAudio 2 high, 21 medium; WWDebug 1 high, 5 medium;
wwshade 1 high, 2 medium. Examples:

| Client | Function | Strings |
|---|---|---|
| 0x9211C0 | `DX8Wrapper::Init` | `D3D8.DLL`, `Direct3DCreate8` |
| 0x918120 | `WW3D::End_Render` | `WW3D::End_Render`, `DX8Wrapper::End_Scene` |
| 0x927700 | `DX8Wrapper::Get_DX8_Render_State_Name` | 74 `D3DRS_*` names |
| 0x9B0160 | `DX8Caps::Compute_Caps` | `Video Card: %s` ... |
| 0x98A2D0 | `HTreeClass::Alter_Avatar_HTree` (Z only) | bone names |
| 0x9D64B0 | `CPUDetectClass::Init_Processor_Log` | `Operating System: `, `Windows NT` |
| 0x9CCD50 | `Dump_Exception_Info` | `IMAGEHLP.DLL`, ... (E&B writes `_except.txt`, as Renegade) |
| 0x9E03B0 | `RC4Class::Print_State` | the state dump format |

The full table: [data/funcmatch.md](data/funcmatch.md).

### 3.5 Byte matching with the client's compiler

Each library copy (Renegade, Generals and Zero Hour, each compiled against its own sibling
libraries) was compiled with VC6 SP4 in the build VM and every function searched for in
client.exe with relocated fields masked. `tools/vc6/bytematch_all.sh` reruns all of it (about
4 minutes).
A function found exactly once is that function, unchanged. Per-file reports:
[WW3D2](data/bytematch-ww3d2.md), [WWLib](data/bytematch-wwlib.md),
[WWMath](data/bytematch-wwmath.md), [WWAudio](data/bytematch-wwaudio.md),
[WWSaveLoad](data/bytematch-wwsaveload.md), [WWDebug](data/bytematch-wwdebug.md),
[wwshade](data/bytematch-wwshade.md); each has a `.json` mapping client address to function.

**Flags.** The client's libraries were built with `/MT /O2 /Ob2` and **no `/GX`** (no C++
exception handling) and no `/G6`: Renegade's release configuration, not Zero Hour's
(`/G6 /MD /GX`). Tested on `ini.cpp`, `cpudetect.cpp`, `rc4.cpp` (byte-exact functions found):

| Flags | Exact |
|---|---|
| `/MT /O2 /Ob2` (no `/GX`) | **88** |
| `/MT /GX /O2 /Ob2` | 69 |
| `/MT /GX /O2` | 37 |
| `/G6 /MT /GX /O2 /Ob1`, `/G6 ... /Ox` | 10 |
| `/G6 /MT /GX /O2 /Ob2` | 9 |

**Results (2026-10-22): 2,708 distinct client functions are byte-identical to EA source**,
about 568 KB of code. Of the first 2,650 counted: 325 under 32 bytes, 461 of 32–63, 698 of
64–127, 568 of 128–255, 598 of 256 or more. Short functions are weaker evidence on their own, but
they sit among long ones in the same file order.

| Library | Client functions | Only Renegade's copy | Only Generals' | Only Zero Hour's |
|---|---|---|---|---|
| WW3D2 | 1,502 | 566 | 17 | 153 |
| WWLib | 553 | 25 | 3 | 15 |
| WWMath | 369 | 27 | 0 | 2 |
| WWAudio | 175 | 94 | 0 | 3 |
| WWSaveLoad | 80 | 5 | 0 | 31 |
| WWDebug | 66 | 1 | 0 | 32 |
| wwshade | 46 | (none) | (none) | 46 |

Generals' copies add little: 17 WW3D2 functions (`hlod` 10, `streakrender` 5, `dazzle`,
`render2d` 3 each, ...) and 3 in WWLib. Zero Hour's `wwdebug.h` includes
`GameEngine\Include\Common\debug.h` from its own game engine; the wrapper's `--sync` copies
that directory, and the copies of Generals and Zero Hour compile against their own WWDebug.

(Inline helpers from headers match from several libraries, so the rows add up to more than
2,650.) A few files didn't compile: they need GNU regex (`regexpr.cpp`), Generals' debug macros
(`dx8renderer.cpp` and `sortingrenderer.cpp` in Zero Hour), the in-game browser
(`dx8webbrowser.cpp`) or `mmsystem.h` (`wwmouse.cpp`).

### 3.6 Not matched: E&B's own versions

Functions found in neither copy are where E&B's code differs. The first one examined:

* **`RC4Class`** (WWLib, 0x9E0210–0x9E04B8). `Print_State` and `operator=` are Zero Hour's,
  byte for byte. The rest is a third version. The constructor fills a static initial key
  (`State[i] = i`, X = Y = 0, at 0xC1ED10) once, guarded by a flag at 0xC1EE12; Zero Hour has a
  static `RC4_Table_Init[]` array. `Prepare_Key` (0x9E0290) copies that 258-byte key and then
  branches on lengths 8 and 16, with the 8- and 16-byte versions inlined. The 16-byte setup and
  `RC4()` (0x9E0420) contain hand-written inline assembly (`push ebp` / `mov ebp, eax` inside the
  loop). To be reconstructed and proven byte-exact.

## 4. What this means for the rebuild

* Import **file by file** (D4), each from the copy whose bytes match (§3.5 reports): mostly
  Renegade for WW3D2's core, WWMath and WWAudio; Zero Hour for wwshade, WWSaveLoad and the WWLib
  and WW3D2 files the reports show. Build with `/MT /O2 /Ob2`.
* The earlier finding that WWLib didn't change between the titles holds for most files, but
  not all (§3.3). Each imported WWLib file needs checking against the client.
* E&B's own code (`G\app`, ~0x429630–0x8C1B50, about 18,700 functions) is not in EA's code and has
  to be reverse engineered. EA's code still helps there: it gives the class layouts of every
  WW3D/WWMath/WWLib object E&B's code touches, so those calls and fields can be named.

## 5. Next steps for matching

1. **Vtables.** For every matched class with virtual methods, find its vtable in the client
   (constructors store it) and name each slot from the EA header's declaration order. This names
   functions that have no strings, and checks the class's version (a slot added or removed shows
   up at once).
2. **Call-graph propagation.** A matched function's callees, in order, against the calls in the
   EA function's body. Names the helpers matched functions call.
3. **Compile and compare.** Done for the first pass (§3.5). Next: per function, choose the copy;
   for functions neither copy matches, diff against the client (`tools/re/objdiff.py`) to find
   E&B's changes, starting with files where most functions match and a few don't.
4. **Constants and tables.** CRC tables, `DX8Caps` vendor tables, `FVFInfoClass` tables: exact
   data matches for data structures.

## 6. Licence

EA's code is GPL v3 with additional terms (modified versions must be marked as such and not
identified as the original; no use of EA's trademarks beyond attribution). Anything that
includes it is GPL v3 too, and so is this repository (decision D1 in
[progress.md](progress.md)).
