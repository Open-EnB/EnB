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

Still open: wwshade (138 client functions in the tree, Zero Hour's copy alone 61). `shdlegacyw3d.cpp`/`.h` are not in the client and were dropped (D13). Not rebuilt yet: `ShdMeshClass::Render` (0xA09430, a static guard), the `ShdMeshClass` ctor (0xA08F80). The client's `MeshClass::Cast_Ray` (0x940530) has no translucency test at all (a WW3D2 near-miss). In the texture files,
`TextureClass`'s constructors, `Init`, `Apply` and `Get_Reduction` and `DX8Wrapper`'s device code
are near-misses still (`Get_Reduction` has Renegade's body, unconfirmed; Renegade's
`Set_Render_Target(TextureClass *)` is kept without evidence). The remaining near-misses (`TexProjectClass`, `MeshClass`,
`GridTextureMapperClass`, `WW3DAssetManager`, the particle buffer's constructor and line-group
rendering, `CPUDetectClass::Init_Processor_Speed`: one swapped register pair) are code
differences rather than layouts. Gaps split into linked and unlinked: docs/data/gaps-*-classified.md.

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

**Running total:** 4,208 client functions (841 KB) proven to be EA's Westwood libraries or
Microsoft's libraries, plus about 800 KB of linker and compiler output. Of the ~2.2 MB library
region (0x8C1B50 on), 829 KB is proven; most of what remains is the Westwood libraries' unmatched
functions (the gaps, §3.6) and the incremental padding.

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
