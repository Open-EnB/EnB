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

**Except wwshade** (2026-10-23): built with `/MT /O2 /Ob1 /GX`. Its functions have EH frames
(handlers in the 0xAE0630 region) and call functions that `/Ob2` inlines (`ShdSubMeshClass::read_chunks`
0xA0E310 calls all ten readers). On the tree's wwshade: `/GX /Ob2` gains 16 of its own functions over
no `/GX` (it loses only out-of-line copies of other libraries' inline functions); `/GX /Ob1` gains 15
more, loses none. Zero Hour's copy alone: 46 -> 61. `importer.py LIB_FLAGS` and
`bytematch_all.sh LIBFLAGS` carry it.

**wwshade and WWAudio were built with `_MBCS`** (2026-10-23): `StringClass(const TCHAR*)`'s
`_tcsclen` is a call to `_mbslen` (0xA2BCAB) in the client, and its only four callers are
`ShdDefClass(classid)` 0xA0EC30, the `ShdMeshClass` ctor 0xA08F80 and `WWAudioClass` 0x9F1BA0,
0x9F3700 (every other library inlines `strlen` there). WWAudio with `/D _MBCS`: +2 (those two),
none lost; wwshade: nothing gained or lost yet (0xA0EC30 then differs only in a null test the
client keeps after the call).

**wwshade was built with `ENABLE_WWPROFILE`** (2026-10-23): `ShdMeshClass::Render` 0xA09430 opens a `WWProfileSampleClass` (`IsRoot` 0, `Enabled` from the manager; `Start_Profile` 0x9EA930, dtor 0x444160, which E&B's own code calls 212 times). E&B's `ShdDX8RendererClass::Flush` 0xA08060 has no sampler (patched out). +2, none lost. **So was WW3D2**: with `/D ENABLE_WWPROFILE` it gains 23 (asset manager, dazzle, sorting renderer, texture thumbnails, `WW3D`...); E&B's `ParticleEmitterClass::On_Frame_Update` 0x97DEF0 has no sampler (patched out). Lost: the pairs (dx8renderer, 0x53F1C0) and (ww3d, 0x53F1C0), an app-region copy of `StringClass(const char*, bool)` still matched by 14 other files. **And WWAudio and WWMath**: WWAudio 266 -> 298 (`WWAudioClass`, `SoundSceneClass`, sound buffers and handles), WWMath +1 (`cullsys` 0x8C8DD0), none lost; WWDebug neutral, WWLib and WWSaveLoad have no samplers. `ENABLE_WWPROFILE` looks project-wide (E&B's own code calls the sampler's dtor 212 times).

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

### 3.6.1 Layout differences (2026-10-23)

Most of the remaining near-misses are functions that differ from the client only in member
offsets: E&B's headers declare a class differently. `tools/re/layoutdiff.py` aligns each located
gap with the client's function and tallies the displacement pairs (`[reg+X]` here, `[reg+Y]` in
the client) per class; `tools/vc6/layout.py` prints a class's offsets as VC6 lays it out. Found
and fixed so far (importer `PATCHES`, see docs/imports.md):

| Fix | Evidence | Gained |
|---|---|---|
| `MeshBuilderClass::MAX_STAGES` = 8 (Zero Hour's value; Renegade and Generals: 2) | `VertexMaterialClass::CRCDirty` at +0x6C, not +0x3C: `Mapper[]` and `UVSource[]` are sized by it | 36 |
| `RenderObjClass` gets Zero Hour's render hook (`RenderHookClass`, the `RenderHook` member, `Get/Set_Render_Hook`, deleted in the destructor), but not Zero Hour's `ObjectScale`/`ObjectColor` | every member after +0x88 shifted by 4 in `HLodClass`, `SphereRenderObjClass`, `RingRenderObjClass`, ... | 287 |
| `Matrix3D` gets Zero Hour's `preMul`/`postMul`/`mul`/`mulVector3`/`mulVector3Array` helpers (not its operator rework) | Zero Hour's `streak.cpp` needs them; let the last WW3D2 files compile | 1, then 7 more after a refine pass |
| `RenderObjClass` vtable as in the client (113 slots, 0xAE84F8): Renegade's four user-lighting virtuals are not virtual (as in Zero Hour), and Zero Hour's `Remove_Sub_Objects_From_Bone(int)` overload is there (slot 35, 0x941F90; first taken for an unknown empty virtual at slot 40, which is `Set_Animation(void)`: VC6 lists overloads in reverse order) | `tools/re/vtables.py`: the client's table, slot by slot | 125 + 18 |
| `ParticleEmitterDefClass` and its file data: Zero Hour's `W3dEmitterExtraInfoStruct` (`FutureStartTime`), plus an E&B flag in its padding (`UnknownFlag`, a `uint8`) with a virtual setter after `Set_Future_Start_Time` and a getter | +0x28 from `m_LineProperties`; vtable slot 19 at 0xB10920 | 39 + 11 |
| `SceneClass`/`SimpleSceneClass`: Zero Hour's `Get_Scene_ID` virtuals (not its virtual `Render`) | vtable at 0xB120FC, slots 2 and 23 | 8 |
| Miles API declared `dllimport` (SDK header patched outside the repository) | the client calls Miles through the import table | 30 (WWAudio) |
| `AudibleSoundClass`: two E&B ints after `m_LoopsLeft` (4 and -1 at construction; `m_Unknown7C`, `m_Unknown80`) | +8 from `m_Length`; both constructors | 31 (WWAudio) |
| `Sound3DClass`: E&B fields after `m_CurrentVelocity` (12 bytes) and after `m_LastUpdate` (four ints) | +12 from `m_MaxVolRadius`; the constructors zero them (the copy constructor at 0x9F7C90 up to +0x16C) | 9 + 10 (WWAudio) |
| `RingRenderObjClass`, `SphereRenderObjClass` from Zero Hour (LOD support); `Matrix4x4`/`Matrix3x3` as aliases of Renegade's `Matrix4`/`Matrix3` | +0x34/+0x40 shifts | 28 + 31 |
| `ParticleBufferClass`: Zero Hour's line-group `GroupID` buffer and `CurrentGroupID`, not its `FutureStartTime`; `NewParticleStruct::GroupID` | +4 from `PingPongPosition` | 19 + 1 |
| `ParticleEmitterClass`: Zero Hour's code, but E&B keeps the future start time in the emitter (`FutureStartTime`, ms; `Reset()` starts `EmitRemain` from it) and takes and keeps the definition's flag (`UnknownFlag`) | the constructor (0x97D180, `ret 0x78`), copy constructor and `Create_From_Definition` | 10 |
| `LightEnvironmentClass`: Renegade's light record (no point lights) | E&B's class is 0xB0 smaller (4 lights x 44 bytes) | net 2 |
| `MeshModelClass` keeps `PolygonRendererList` (as Generals/Zero Hour) | `GapFiller` 24 bytes later | 6 |
| `DazzleRenderObjClass` from Zero Hour (`current_scale`) | +4 at 0x124 | 25 |
| `DX8Caps`: Zero Hour's twelve extra capability flags, not its `MaxSimultaneousTextures` | +12 from `VertexShaderVersion`, `DeviceId` too | 13 + 5 (wwshade) |
| `RenderInfoClass`: Zero Hour's `Texture_Projector` after `light_environment` (not its alpha overrides) | the client's constructor (0x930F80) zeroes one more member; `SpecialRenderInfoClass` members from 0x124 | 14 |
| `GridTextureMapperClass`: Zero Hour's `Offset` and five-argument constructors | +4 from 0x20 | 4, -2 in `meshmatdesc.cpp` (inlining flip, documented) |
| `ShaderClass`: Zero Hour's `SSCAT_SCREEN`; `Get_SS_Category`, `Guess_Sort_Level` spliced from Zero Hour | the two functions | 2 |
| `RenderObjClass` class IDs end as Zero Hour's (`CLASSID_SHDMESH` = 0x24) | `ShdMeshClass::Class_ID` at 0xA0A7E0 returns 0x24 | 0 (one more wwshade file compiles) |
| WWLib `Pick_Random_Number`: unsigned, with a full-range mask guard (reconstructed) | the four `operator()(min, max)` compare unsigned | 4 (WWLib) |
| Six more WW3D2 files from Zero Hour by batch trials (`tools/vc6/trial_all.py`): `aabtree`, `composite`, `dx8fvf`, `dynamesh`, `hanim`, `surfaceclass` | kept only because they gain and lose nothing | 26 |
| Texture system (D9): Zero Hour's 06/27/02 "texture class abstraction" without its 08/05/02 redesign. `texture`, `texproject`, `dx8texman`, `bitmaphandler`, `texturethumbnail` (+ `colorspace.h`, `dx8list.h`, `formconv.h`) from Zero Hour, minus cube/volume textures, `IsReducible`/`allow_reduction`, the `Get_Asset_Type`/`Get_Texture_Memory_Usage` virtuals and Generals' `#if 0` around thumbnail `Load`/`Save`. `DX8Wrapper`, `textureloader`, `ddsfile` stay Renegade's with Zero Hour's 06/27/02 parts patched in (`KJM` blocks: Z textures, depth render targets, shader state, draw-call statistics; loader on `TextureBaseClass` with HSV shift; HSV in DDS copies). Renegade-API callers renamed (`MipCountType`, `TextureFilterClass`, `Peek_D3D_Texture`). Importer `TEXTURE_API_PATCHES` | vtables at 0xB119A0 (base, trackers, `TextureClass`, `ZTextureClass`: 6 slots each, no cube/volume tables); constructors 0x94B020 (`ret 0x14`, `D3DTexture` at +0x34), 0x94B240, 0x94B3D0; no caller of `D3DXCreateCubeTexture`/`D3DXCreateVolumeTexture`; no vtable for the load task; Zero Hour's dated change logs; `Peek_Thumbnail_Instance_From_Any_Manager` at 0x91AD40 with a static flag at 0xBFAEA5 | 88 (WW3D2) + 7 (wwshade), none lost |
| wwshade's WW3D2 (D9, importer `WWSHADE_API_PATCHES`): Zero Hour's `IS_SELF_SHADOWED` bit and accessors, `LightEnvironmentClass::Get_Max_Lights`, `DX8Wrapper::Apply_Default_State` in an earlier form (`Set_Material(NULL)`, no light loop, no `ShaderClass::Invalidate`, no opening `SNAPSHOT_SAY`), `Set_Ambient`/`Get_Ambient`/`Ambient_Color` (used by `WW3D::Render`), `Set_Vertex_Shader` keeping its redundancy check, two vertex streams (`vertex_buffers[2]`), non-FVF vertex buffers (`vertex_size`, without W3DMPO), `Light_Environment`/`Render_Info`, `MAX_TEXTURE_STAGES` = 8 with dx8renderer on `MeshMatDescClass::MAX_TEX_STAGES`. E&B's own: `Set_Light_Environment(env, flag)` (lights set only when the flag is set) and a `Render_Info` setter (names unknown, D10). wwshade's `ShdDX8RendererClass::Flush` order and `ShdMeshClass::Cast_Ray` as the client has them | `ShdMeshClass::Render` 0xA09430 (bit 0x80000), `Apply_Default_State` 0x9270D0, `WW3D::Render` 0x917E2F (ambient to 0xC00530), `Set_Vertex_Shader` 0x9275E0, `Set_Vertex_Buffer(vb, stream)` 0x924E20 (`vertex_buffers` 0xBFFEE0), `DX8VertexBufferClass` 0x94F330 (`ret 0x10`), `Set_Light_Environment` 0x926460 (callers push 1, the shader renderer 0), `render_state.Textures` 0xBFFC88-0xBFFCA8 (8), `ShdDX8RendererClass::Flush` 0xA08060, `ShdMeshClass::Cast_Ray` 0xA09580 (Renegade's `IgnoreTranslucentMeshes` at +0x40) | 25 (WW3D2) + 36 (wwshade), none lost |
| E&B's earlier wwshade (D9, importer `WWSHADE_EARLY_PATCHES`, compiled `/GX /Ob1`): renderer node on `MultiListObjectClass` only, a visible list per pass in `MeshContainerClass`, no `RendererListContainerClass`, sorting, skins or node comparisons; `ShdInterfaceClass(def)` without class id, `Greater_Than`/`Similar_Enough`/`Is_Opaque`; `ShdSubMeshClass` without visible-polygon range, sorting, legacy conversion, skin deformation or vertex-influence chunk; no LegacyW3D shader (class ids end at CUBEMAP, one prototype loader); E&B's "Cube Map (unsupported)" shader on a D3DX cube texture. With them: Zero Hour's fill light in `LightEnvironmentClass` (no Generals point lights, no near-black tests) and Zero Hour's const `RefCountClass` | node ctor 0xA082D0 (vtable 0xB16AA8), `MeshContainerClass` 0x68 bytes (0xA08005), Shd6SimpleClass vtable 0xB16F0C (12 slots), `ShdInterfaceClass` ctor 0xA08E70 (`ret 4`), `SHD_Force_Links` 0xA08C70 (5), `Init_Shaders` 0xA07E00 (ids 1..5), `read_chunks` jump table 0xA0E3D0, cube map ctor 0xA0AD80 (`D3DXCreateCubeTextureFromFileA`), `LightEnvironmentClass` ctor 0x9853B0 (+0x144), `Calculate_Fill_Light` 0x985840 | 7 (WW3D2) + 62 (wwshade). The 20 wwshade pairs lost were mispairings: copies of other libraries' inline functions (still matched there) and `0x93ED00` (Renegade's `MeshClass::Clone`, matched before by the no-`/GX` `ShdMeshClass::Clone`; the real one, 0xA092C0, now matches) |
| E&B's fixed-function lights (importer `WWSHADE_EARLY_PATCHES`): `ShdHWVertexShader::Unknown_Set_Lights` (name not known, D10) copies `DX8Wrapper`'s render state and sets each enabled D3D light, light 0 with a white specular and `Ambient_Color` as its ambient (set before the ambient, or it doesn't match); called at the end of the Simple, Gloss Mask and Cube Map DX6 `Apply_Shared`. Simple sets its material there, and neither Simple nor Gloss Mask sets it in `Apply_Instance` | 0xA0FC60 (after `Light` in shdhwshader.obj; `ShdHWShader`'s vtable stored at 0xA0FE60); callers 0xA0AF40, 0xA0CD90, 0xA0D910; `Apply_Instance` sets only the textures: Simple 0xA0DA00, Gloss Mask 0xA0CF10 | 6 (wwshade), none lost |
| E&B's shader definition defaults (importer `WWSHADE_EARLY_PATCHES`): `Ambient` starts at 0,0,0 in Simple, Gloss Mask, Bump Diffuse and Bump Specular; the cube map's `Ambient` stays 1,1,1 and its `Diffuse` starts at 0,0,0. Every copy constructor initialises the members to those defaults and assigns them from `that` in its body, after the texture names (Zero Hour's `ShdSimpleDefClass` copy constructor already does) | default/copy constructors: cube map 0xA0A840/0xA0A900, Bump Diffuse 0xA0B370/0xA0B450, Bump Specular 0xA0BCF0/0xA0BDE0, Gloss Mask 0xA0C680/0xA0C740, Simple 0xA0D290 (found by their vtable stores; gaps.py paired several elsewhere). `Create` 0xA0AD00 and the dtor 0xA0AA30 differ only in relocations (twins of the other definitions', vtable-placed) | 9 (wwshade), none lost |
| Shader versions: Zero Hour's `DX8Caps::Get_Pixel_Shader_Minor_Version` (reads `PixelShaderVersion`; Renegade's reads the vertex shader's), and E&B's Bump Specular always on its DX6 version (`ShdBumpSpecDefClass::Init`'s selection under `#if 0`) | `ShdBumpDiffDefClass::Init` 0xA0B9B0 takes major and minor from +0x12C; `ShdBumpSpecDefClass::Init` 0xA0C3B0 stores 1 (`SHDVER_6`) to 0xC40730 and jumps to `Shd6BumpSpecClass::Init` (ours is the same 15 bytes, too short to be counted) | 1 (wwshade). The pair (shdbumpspec, 0xA0B9B0) is lost: a twin of Bump Diffuse's `Init`, still matched there |
| `ShdMeshClass` (importer `WWSHADE_EARLY_PATCHES`): `Update_Cached_Bounding_Volumes` transforms the sphere centre with `operator *` (as `MeshClass`'s), not `Transform_Vector`; `Load_W3D` assigns `Name` inline and does not name its sub-meshes | `Update_Cached_Bounding_Volumes` 0xA09ED0 (temporaries, no aliasing test); `Load_W3D` 0xA0A380 (`StringClass::operator=` at 0xA0A3E2, `i++` right after the sub-mesh's `Load_W3D`, frame 0x16C). `Get_Num_Vertices`, `Get_Sub_Mesh_Count`, `Peek_Sub_Mesh` are not in the client (unreferenced; gaps.py paired them with app-region code) | 2 (wwshade), none lost |
| `Sound3DClass::Set_Velocity` takes a second float, the time since the last update (importer `PATCHES`), so the one-argument `Set_Velocity` of `SoundPseudo3DClass` and `Listener3DClass` hide it instead of overriding it | 0x9F8920 ends `ret 8` and reads `[ebp+0xC]`; `On_Frame_Update` 0x9F7E80 passes two arguments. `SoundPseudo3DClass`'s vtable 0xB15EE4: slot 97 is 0x9F8920, slot 105 (its first own slot) is the one-argument 0x9FFA90 (`ret 4`), so the later slots move up one (`Update_Pseudo_Volume` at 0x1A8/0x1AC). `Listener3DClass`'s vtable 0xB163D0: the one-argument 0xA02DE0 is its last slot (121), so it is declared after `Set_Sample_Playback_Rate`. (First recorded as a non-const `Vector3 &` in `SoundPseudo3DClass`; replaced, same bytes) | 4 (wwaudio: 0x9FFBD0, 0x9FFCC0, 0x9FFF70, `FilteredSoundClass::Update_Volume` 0xA036F0), none lost |
| `Sound3DClass` (importer `PATCHES`): E&B takes the Miles lock (`MMSLockClass`) in `Update_Miles_Transform`, `Set_DropOff_Radius` and `Set_Max_Vol_Radius`; `Update_Edge_Volume` restores the full volume inside the falloff start | AIL_lock/AIL_unlock imports at the start and end of 0x9F8680, 0x9F8D00, 0x9F8D70; `Internal_Set_Volume(m_RealVolume)` at 0x9F8337 | 4 (wwaudio), none lost |
| `WWAudioClass` (importer `PATCHES`, `sdk_patch.py`): two E&B bools after `m_ForceDisable`, `m_Unknown19A5` (pause-all: `AudibleSoundClass::Play` queues sounds while set; cleared by `Unknown_Resume_All`) and `m_Unknown19A6` (set by `Temp_Disable_Audio`, which zeroes the effect, music and dialog volumes; the four volume setters do nothing while set); `Unknown_Resume_All` (D10); `AIL_startup()` calls `MSS_auto_cleanup()` (toolchain.md §2, fix 4) | ctor 0x9EE9B0 (zeroes +0x19A5/+0x19A6 at 0x9EEC2E; `MSS_auto_cleanup` 0xA03A30 at 0x9EEC54); setters 0x9F2240, 0x9F22F0, 0x9F23A0, 0x9F2480; `Unknown_Resume_All` 0x9F4720 (callers 0x5AD123, 0x744852); `Temp_Disable_Audio` 0x9F4C50; the app sets +0x19A5 at 0x6918D7, 0x7409A3, 0x7447F1. gaps.py had paired the ctor with 0x9DAE10 | 7 (wwaudio), none lost (WW3D2 2,202 unchanged) |
| `AudibleSoundClass::Play` (importer `PATCHES`): while `WWAudioClass::m_Unknown19A5` is set, a sound other than music is added to the playlist paused and returns | 0x9FD3E0 (flag read at 0x9FD419, `STATE_PAUSED` stored at 0x9FD430) | 1 (wwaudio), none lost |
| `Sound3DClass` Doppler (importer `PATCHES`): `Set_Velocity` computes a pitch from the velocity relative to the listener (constants 0.85, 3.4, 1/33.3, names not known); `On_Frame_Update` re-enables Renegade's `#if 0` auto-velocity, gated by `_UnknownDopplerFlag` (D10; the app's "Doppler_Enabled"), and calls `Update_Miles_Transform`; `Set_Listener_Transform` tracks the listener velocity (`m_Unknown148`, timed by `m_Unknown160`) and no longer calls `Update_Miles_Transform`; `m_Unknown164/168/16C` are floats (pitch range, velocity scale) | `Set_Velocity` 0x9F8920, `On_Frame_Update` 0x9F7E80, `Set_Listener_Transform` 0x9F84A0 (its `!=` keeps `operator==` out of line: 0x9F8630); flag 0xC404F8 set by the app at 0x51461C; `WWMath::Inv_Sqrt` 0x447C40 out of line from `Normalize` | 5 (wwaudio), none lost (WW3D2 2,202 unchanged) |
| `Sound3DClass::Set_Transform` and `Set_Position` (importer `PATCHES`): no longer keep `m_PrevTransform` or update Miles (`On_Frame_Update` does both now); `Set_Position` takes the Miles lock | `Set_Transform` 0x9F8350, `Set_Position` 0x9F8760 | 2 (wwaudio), none lost |
| `SoundSceneObjClass` deferred release (importer `PATCHES`; names not known, D10): the destructor queues `m_UserObj` and `m_AttachedObject` (`Unknown_Release_Later`) instead of releasing them; `WWAudioClass::On_Frame_Update` and `Shutdown` release the queue (`Unknown_Flush_Release_List`). Statics `m_UnknownReleaseListMutex`, `m_UnknownReleaseList` defined after `m_IDListMutex` | dtor 0xA00460; 0xA01280, 0xA01300 (callers 0x9F11CA, 0x9F29F6); statics 0xC40590, 0xC40598 (destructor guard bits 2 and 4, after `m_IDListMutex`'s 1; list destructor 0xA00230); 0xA007D0; `On_Frame_Update` 0x9F1150, `Shutdown` 0x9F2940 | 7 (wwaudio), none lost (WW3D2 2,202 unchanged) |
| `WWAudioClass::Add_To_Playlist` (importer `PATCHES`): a sound already in the playlist is removed from `m_CompletedSounds` | 0x9F0D90 (search loop 0x9F0E39, `Delete(index)` 0x9F0E59) | 1 (wwaudio), none lost |
| `WWAudioClass::Is_Disabled` (importer `PATCHES`): the `-NOAUDIO` argument check under `#ifdef G_CODE_BASE` (also in Renegade's copy) is compiled in E&B; defining `G_CODE_BASE` for WWAudio would give the same bytes (open question) | 0x9F26A0 (`ArgvClass` 0x9F26BE); with it `Is_Disabled` is no longer inlined, and the six `Create_*` (0x9EFD90, 0x9EFEC0, 0x9F0090, 0x9F01C0, 0x9F0350, 0x9F0540), both `Initialize` (0x9F2740, 0x9F2840), both `Simple_Play_2D_Sound_Effect` (0x9F3390, 0x9F3500), `Fade_Background_Music` 0x9F41B0, `Set_Background_Music` 0x9F4420 and 0x94C000 match | 14 (wwaudio), none lost |
| `AudibleSoundClass` (importer `PATCHES`): `Update_Volume` passes `m_RealVolume`; `operator=` copies `m_Unknown7C` and `m_Unknown80` | `Update_Volume` 0x9F7910 (slot 54; reads +0x6C); `operator=` 0x9FD090 (copies +0x7C, +0x80 after `m_LoopsLeft`) | 2 (wwaudio), none lost (WW3D2 2,202 unchanged) |
| `WWAudioClass::Reprioritize_Playlist` (importer `PATCHES`): E&B gives a sample handle only to a sound with fewer than `m_Unknown7C` (4 by default) instances holding handles, counted by the new `Unknown_Count_Instances` (D10) over sounds with the same `m_Unknown80` | `Reprioritize_Playlist` 0x9F0FE0 (nested test with a temporary: `m_Unknown7C` loaded at 0x9F1042), `Unknown_Count_Instances` 0x9F1090 | 2 (wwaudio), none lost (WW3D2 2,202 unchanged) |
| `WWAudioClass::Get_2D_Sample`, `Get_3D_Sample` (importer `PATCHES`): the same instance limit when handing out a handle (at the limit, the least important instance's handle is taken); the scan covers every handle; in 2D only sound effects give up handles, and other types start from priority 99999.9 | `Get_2D_Sample` 0x9F1360, `Get_3D_Sample` 0x9F15E0 | 2 (wwaudio), none lost |
| `WWAudioClass::Build_3D_Driver_List` (importer `PATCHES`): providers tried in the order D3DSOUND, EAX, PSEUDO, A3D, DOLBY | 0x9F1850 (`Find_3D_Device` 1, 2, 5, 3, 6) | 1 (wwaudio), none lost |
| `WWAudioThreadsClass::Delayed_Release_Thread_Proc` (importer `PATCHES`): skips a pass while `AIL_background()`, holds `AIL_lock_mutex` inside the list lock | 0x9FBA30 (0x9FBA63, 0x9FBA7F, 0x9FBB1D) | 1 (wwaudio), none lost |
| Mesh rendering per mesh model (importer `MESH_MODEL_RENDERER_PATCHES`): polygon renderers live in `MeshModelClass` (+0x84, already declared) and are registered once per model, as in Generals (`Register_Mesh_Type`/`Unregister_Mesh_Type` take the model, `_RegisteredMeshList`, `Define_FVF` without user lighting); `MeshClass` loses Renegade's user lighting and its own renderer list; `MeshModelClass` unregisters in its destructor, `operator=` and `Reset` (Zero Hour's order) and registers in `Register_For_Rendering`. Kept from Renegade: the sort-level test, the delayed material passes and the gap-filler removal. `MeshClass`'s one new virtual (slot 113, returns 0x750) is `Unknown_Virtual_113` (D10) | `DX8PolygonRendererClass` constructor 0x9C4820 adds to `[mmc+0x84]`; `MeshClass::Clone` 0x93ED00 allocates 0xA8; `Register_Mesh_Type` 0x97AC10, `Unregister_Mesh_Type` 0x97ABA0, `MeshClass::Render` 0x93F390, `MeshModelClass::Reset` 0x9586B0; no user-lighting functions in the client | 30 (ww3d2), none lost; wwshade 138 unchanged |
| No user-lighting persistence in `RenderObjClass` (importer `MESH_MODEL_RENDERER_PATCHES`): Generals' `RenderObjPersistFactoryClass::Save`/`Load` (object created after the chunk loop), `Save_User_Lighting`/`Load_User_Lighting` and the sub-object helpers dropped | `Save` 0x942CF0 (no second chunk), `Load` 0x942B20 | 2 (ww3d2), none lost |
| `MeshClass::Cast_Ray` (importer `PATCHES`): no `IgnoreTranslucentMeshes` test | 0x940530 (collision type, `Is_Animation_Hidden`, `StartBad`) | 1 (ww3d2), none lost |
| `MeshModelClass::compute_static_sort_levels` (importer `PATCHES`): Zero Hour's `SSCAT_SCREEN_BF` case | 0x9BD260 (switch cases 3, 4, 8) | 1 (ww3d2), none lost |
| `SegLineRendererClass::Render` (importer `PATCHES`): Zero Hour's sixth argument `Vector4 * rgbas = 0` | callers `SegmentedLineClass::Render_Seg_Line` 0x969C50 and `Render` 0x9695A0 push 0 | 2 (ww3d2), none lost |
| `MeshClass::Make_Unique` (importer `PATCHES`): Zero Hour's `bool force_meshmdl_clone` | 0x940300 tests the argument | 1 (ww3d2), none lost |
| `MeshGeometryClass::operator=` (importer `PATCHES`): Zero Hour's: the cull tree is cloned, not shared | 0x92A200 (`new` of 0x1C bytes, `Set_Mesh`) | 1 (ww3d2), none lost |
| `TextureLoadTaskClass` constructor (importer `PATCHES`): Zero Hour's `HSVShift(0,0,0)` | inlined in `TextureLoadTaskClass::Create` 0x94A120 (+0x28..+0x30 zeroed) | 2 (ww3d2), none lost |
| `ParticleBufferClass` line groups from Zero Hour (importer `PATCHES`): the `GroupID` buffer initialised, allocated and released, `CurrentGroupID`, `Get_New_Particles` storing group ids, Zero Hour's `Render_Line` (runs per group, per-point colours); E&B's line end point at the emitter comes from `ParticleEmitterClass::Unknown_Get_Position` (origin for emitter-space particles; name not known, D10) | constructors 0x95BA90/0x95C360 (+0x204, +0x2B4), destructor 0x95D430, `Get_New_Particles` 0x962070, `Render_Line` 0x95DDE0 (0x95E290), 0x97E260 | 12 (ww3d2, with template instances), none lost |
| `ParticleEmitterClass` (importer `PATCHES`): with `UnknownFlag` (+0xFD) set, `Emit` emits in emitter space through two E&B functions (`Unknown_Create_New_Particles`, `Unknown_Initialize_Particle`: `Create_New_Particles`/`Initialize_Particle` without the transform, names not known, D10); the constructor keeps `FutureStartTime` at 0 unless positive and starts `EmitRemain` from it; `Create_New_Particles` multiplies by `1/frametime` and lerps the origin by value; `Build_Definition` passes `UnknownFlag` on | `Emit` 0x97E330 (0x97EB80, 0x97ECB0), constructor 0x97D180 (0x97D2CA, 0x97D368), `Create_New_Particles` 0x97E530, `Build_Definition` 0x97EDE0 (slot 0x4C after `Set_Shader`) | 6 (ww3d2), none lost |
| E&B's screen-distortion module (D19; reconstructed `ww3d2/unknown_distortion.cpp`/`.h`, names neutral, D10): the per-frame list (reset, add, draw), `UnknownDistortionClass` (a `RenderObjClass` with a `RefCountPtr` to its distortion function and a radius) and `UnknownRippleClass` | 0x995DC0, 0x995E80; constructors 0x995ED0/0x995FD0, destructors 0x995F10/0x996040, `Clone` 0x9960F0, bounds 0x997D10/0x997D30, ripple constructor 0x997D60 | 10 (ww3d2), none lost. Not yet: the draw 0x9961E0 (6.7 KB), the ripple 0x997DE0, `Unknown_Add_To_Distortion_List` 0x995E20 and `Render` 0x996180 (register allocation) |
| `TextureBaseClass::Invalidate_Old_Unused_Textures` (E&B; importer `PATCHES`): keeps last frame's applied-texture count and zeroes it before the thumbnail test; `Unknown_Get_Textures_Applied_Last_Frame` returns it (name not known, D10) | 0x94B770 (0xC01294), getter 0x94B910 (app callers 0x53EC23, 0x6A1317) | 1 (ww3d2; the getter is too short to count), none lost |
| `TextureBaseClass::Invalidate` as E&B has it (importer `PATCHES`): the inactivation-time extension (Zero Hour's commented-out "battlefield version") first, then the thumbnail task before the load task | 0x94BA10 (0x94BA13, 0x94BA3A); inlined in `Set_HSV_Shift` 0x94C150 | 2 (ww3d2), none lost |
| `TextureLoader::_UnknownInvalidationTime` (E&B; importer `PATCHES`; name not known, D10): set by the application, zeroed by `Init`, passed to `Invalidate_Old_Unused_Textures` by `Update` | 0xC01290 (0x80055E: 300000 ms); `Init` 0x948C40, `Update` 0x949990, `Flush_Pending_Load_Tasks` 0x9497B0 (`Update` inlined) | 3 (ww3d2), none lost |
| `SphereRenderObjClass::Init_Material` keeps backface culling on (E&B; importer `PATCHES`) | 0x90BF80 (0x90C037: the alpha preset copied unchanged) | 1 (ww3d2), none lost |
| `TextureBaseClass::Peek_D3D_Base_Texture` does not set `LastAccessed` (E&B; importer `PATCHES`) | its inlined copies in `ZTextureClass::Apply` 0x94D190, `TextureClass::Init` 0x94B920, `Get_Surface_Level` 0x94BB50, `Get_D3D_Surface_Level` 0x94BBB0 | 4 (ww3d2), none lost |
| `TextureLoadTaskClass` (importer `PATCHES`): `Get_Texture_Information` takes a `const StringClass &` path; `Begin_Uncompressed_Load` keeps the validated `dest_format` for an unknown format | 0x94A890, 0x94A6F0, 0x94AA80 (0x94AB03) | 3 (ww3d2); 0x48CA70 (`StringClass::operator=`, an app-region copy) no longer emitted by this file, still matched in 18 others |
| `WW3D::Set_Render_Device` (both overloads; importer `PATCHES`) calls an E&B function on success (`Unknown_Reset_Distortion_List`, name not known, D10; module D19) | 0x917790, 0x9177E0 call 0x995DC0 | 0 counted: the overloads are now twins (each found at both addresses, `multiple`), none lost |
| `ParticleEmitterDefClass::Save_Extra_Info` writes `m_ExtraInfo` itself (E&B's flag included; importer `PATCHES`) | 0x916A60 (0x28 bytes from +0x1E0) | 1 (ww3d2), none lost |
| `ParticleBufferClass::Update_Cached_Bounding_Volumes` transforms its sphere and box by `Get_Transform()` (E&B, for emitter-space particles); `ParticleBufferClass::Scale` and `SegLineRendererClass::Scale` from Zero Hour (importer `PATCHES`) | 0x960DB0 (two virtual `Get_Transform` calls, `Transform_Center_Extent_AABox` 0x960EED); 0x95E8B0 (calls 0x9C2B50) | 3 (ww3d2), none lost |
| `LineGroupClass::Render` as E&B has it (importer `PATCHES`): it takes the object's transform (passed by `Render_Line_Group`) and works through three E&B members (names not known, D10): `Unknown_Set_Transforms` (WORLD from the transform; offsets turned by the transpose of `Matrix3(view) * Matrix3(transform)`; or VIEW and WORLD to a static identity), `Unknown_Fill_Indices` and `Unknown_Fill_Vertices` (Renegade's blocks) | 0x97BAF0 `ret 8`, 0x97C900, 0x97C1D0, 0x97C360; callers 0x95E885, 0x6BAEDF | 5 (`Render_Line_Group` 0x95E500 and the four), none lost |
| `PointGroupClass::Render` takes the object's transform (E&B; importer `PATCHES`): the points are transformed by the view matrix's first three rows times the transform; `Render_Particles` passes its `Transform` | 0x954960 `ret 8`, `operator*` at 0x9554D6; callers 0x95DDC6 (`&Transform`, +0x1C), 0x481D08 and 0x6BAE8C (static matrices) | 1 (`Render_Particles` 0x95DC90), none lost; `Render` itself still differs in its stack-slot placement |
| Texture mappers from Zero Hour (`mapper.cpp`/`.h`, `matrixmapper.cpp`/`.h`; importer `MAPPER_PATCHES`): the texture-matrix mappers (`Calculate_Texture_Matrix`, `Apply` setting `D3DTS_TEXTURE0+Stage`), `ScaleTextureMapperClass` as the base of the offset mappers, the WS, grid-WS, random, edge and bump-env mappers, `MatrixMapperClass::Calculate_Texture_Matrix`; Zero Hour's 16-float `Matrix4` constructor and `Init`; `VertexMaterialClass::Load_W3D` keeps Renegade's form but builds the WS mappers from the Args INI and has the `GRID_WS` cases | vtables 0xB10EAC/0xB10F00/0xB10F24 in Zero Hour's slot order (slot 8 `Calculate_Texture_Matrix`); `ScaleTextureMapperClass::Apply` 0x9322F0 (slot 5, inherited by `LinearOffsetTextureMapperClass`); constructor 0x9329F0 (`ret 0x14`); `MatrixMapperClass` 0xB12EAC slot 8 0x980960; `Load_W3D` 0x92F2A0 (0x936070 called for both WS cases, 0x130000/0x140000) | 68 (ww3d2): 56 mapper, 10 matrixmapper, 3 vertmaterial, net +1 meshmatdesc; -1 `MeshMatDescClass::Make_UV_Array_Unique` 0x947310 (register allocation, `meshmatdesc.cpp`'s fragility: including `matrix4.h` first in that file wins it back, no EA copy does so; not applied, D18) |

Still open (2026-10-23): wwshade (138 client functions in the tree): the two `"UnNamed"` constructors, left as near-misses (D17), and an inline `Set_DX8_Texture_Stage_State` copy. `shdlegacyw3d.cpp`/`.h` are not in the client and were dropped (D13); so were WWAudio's `AudioSaveLoad.cpp`/`.h` (no save/load subsystem in the client; accepted loss: its three pairs 0x42F250, 0x909500, 0x9DA8C0, shared helpers still matched by other files). WW3D2 (2,242 in the tree; 407 gaps in docs/data/gaps-ww3d2): in the texture files, `TextureClass`'s constructors, `Init`, `Apply` and `Get_Reduction` and `DX8Wrapper`'s device code
are near-misses still (`Get_Reduction` has Renegade's body, unconfirmed; Renegade's
`Set_Render_Target(TextureClass *)` is kept without evidence). Clusters located but not started (progress.md, 2026-10-23 near-miss entry): the particle system (`ParticleBufferClass`, `PointGroupClass::Render`'s stack frame), E&B's screen-distortion module (0x995D20-0x998200, D19: `ww3d2/unknown_distortion.cpp`) that `WW3D::Set_Render_Device` calls into. Other near-misses (`MeshClass::Render_Material_Pass`, `WW3DAssetManager`, `GapFillerClass`, `CPUDetectClass::Init_Processor_Speed`: one swapped register pair) are code differences rather than layouts. Gaps split into linked and unlinked: docs/data/gaps-*-classified.md.

### 3.7 Precompiled libraries and generated code: out of scope

Code in client.exe that nobody at Westwood wrote, and that the rebuild gets back by linking the
same libraries or by compiling with the same flags. Byte-matched with `tools/vc6/runtime_match.sh`
(the objects inside the `.lib` files are the code; nothing to compile):

| What | Where it comes from | Client functions proven | KB | Report |
|---|---|---|---|---|
| C runtime (`memcpy`, `printf`, `malloc`, startup, EH runtime `__CxxFrameHandler`, ...) | VC6 SP4 `LIBCMT.LIB` | 681 | 102 | [bytematch-libcmt](data/bytematch-libcmt.md) |
| C++ standard library (iostreams, locale) | VC6 SP4 `LIBCPMT.LIB` | 181 | 36 | [bytematch-libcpmt](data/bytematch-libcpmt.md) |
| COM support (`_com_error`, `_bstr_t`) | VC6 SP4 `COMSUPP.LIB` | 13 | 2 | [bytematch-comsupp](data/bytematch-comsupp.md) |
| D3DX (matrix maths, shader assembler, textures) | DirectX 8.0 SDK `d3dx8.lib` | 627 | 147 | [bytematch-d3dx8](data/bytematch-d3dx8.md) |

Not code we have to write either:

* **Generated by the linker:** the incremental-link thunk table (0x401005–0x419650, 97 KB, 19,983
  thunks) and incremental padding: `int3` runs at 0x419650 (63 KB) and 0xA420AC–0xAA1C00 (382 KB).
* **Generated by the compiler:** C++ exception-handling stubs at 0xAA1C00–0xAE2830 (259 KB: about
  7,400 unwind funclets and 4,400 `mov eax, funcinfo / jmp __CxxFrameHandler` thunks). They belong
  to E&B's own functions and come back when those are compiled with `/GX`. So **E&B's app code
  was compiled with `/GX`**, unlike the libraries (§3.5).
* **Templates from VC6's headers** (`std::vector`, `std::map`, `std::string`, ...) instantiated in
  E&B's code: compiled from the headers whenever our code uses them. Not yet measured.
* **DLLs:** Windows, DirectX 8 (`d3d8.dll`, loaded at run time), Miles (`mss32.dll`), Bink
  (`binkw32.dll`), `authlogin.dll`, the browser (`BrowserEngine.dll`): only import entries in the
  exe ([binary.md](binary.md) §3).

**Running total:** see [coverage.md](coverage.md), regenerated by `symbols.py` at each handoff (2026-10-23: 4,428 library-region functions proven byte for byte, 5,025 named in all, about 1,082 KB; 120 proven in the app region). The byte-match results above (2,708 EA functions, 2026-10-22) count only what an unmodified EA copy matches; the tree, with E&B's changes, now has 3,805 library functions (docs/imports.md). About 800 KB more is linker and compiler output (§3.7).

### 3.6 Not matched: E&B's own versions

Functions found in neither copy are where E&B's code differs. The first one examined:

* **`RC4Class`** (WWLib, 0x9E0210–0x9E04B8). `Print_State` and `operator=` are Zero Hour's,
  byte for byte. The rest is a third version. The constructor fills a static initial key
  (`State[i] = i`, X = Y = 0, at 0xC1ED10) once, guarded by a flag at 0xC1EE12; Zero Hour has a
  static `RC4_Table_Init[]` array. `Prepare_Key` (0x9E0290) copies that 258-byte key and then
  branches on lengths 8 and 16, with the 8- and 16-byte versions inlined. The 16-byte setup and
  `RC4()` (0x9E0420) contain hand-written inline assembly (`push ebp` / `mov ebp, eax` inside the
  loop). **Reconstructed** (2026-10-22, `wwlib/rc4.cpp`): all five functions byte-exact. The
  source order is E&B's (constructor, `operator=`, `Prepare_Key`, `Print_State`, `RC4`: with
  `/Gy` each function is a COMDAT placed in source order, so the client's layout gives the order).
  Byte swaps use a local, not Zero Hour's static `RC4_Temp_Byte`; the loop counter is an `int`.

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
