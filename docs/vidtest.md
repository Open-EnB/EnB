# VIDTEST.EXE: functional spec (M2.0, 2026-10-24)

What the original "Video Mode Tester" 1.0 does, precise enough to rebuild a program that behaves
the same on the tree's 2004 Westwood libraries (D36, D45, D46). Target: `EBCONFIG\VIDTEST.EXE`
(2,093,056 bytes, SHA-256 `c66596842b53…0fa8`, PE time 2002-09-03, VC6 SP4, `/INCREMENTAL`).
Its art and ini files are in the same folder.

Sources: Ghidra 12.1.4 (headless and REA) on a copy of the exe, capstone disassembly, and the
earlier read-only assessment `.work/ebconfig/assessment.md`. Scratch, scripts and the per-function
pseudocode are in `.work/milestones/m2/`:

* `pseudo/XXXXXXXX.c`: Ghidra pseudocode of all 475 app functions (0x401000-0x4288A0). The library
  functions are named after the tree's objects where they match exactly (`names.tsv`, 3,133 names
  from `.work/ebconfig/libmatch.json`).
* `app.json`, `fsum.tsv`, `fuzzy_*.json`: the app functions' sizes, client.exe matches and callees.
* `vdis.py ADDR [LEN]`: VIDTEST disassembly with names and strings. `w3dinfo.py FILE...`: W3D chunks.
* `VidDecompile.java` with `ghidra_run2.py`: re-runs the decompile. The project is in
  `%LOCALAPPDATA%\EnB\ghidra_vidtest`, so later runs take about a minute.

Conventions in this document: addresses are VIDTEST's unless they say "client". "= 0x…" means
byte-identical in client.exe once relocations are masked; "~ 0x… (0.9)" is a mnemonic-sequence
similarity to a client function. Names in *italics* are names I gave from the evidence; the
binary has no symbols.

**Corrections from running the original (M2.1, 2026-10-24).** A d3d8.dll proxy in a copy of
EBCONFIG (`tools/harness/run_reference.py`, `d3d8shot.c`) logged the original's frames and file
opens. Where this document says otherwise, these win:
* O1 settled: `Build_Scenes`' first store, `mov byte [TheInstance+0x4074],1` at 0x423F44, is
  `Set_WW3D_Load_On_Demand(true)` (not the asset manager's +0x101D). mu02, MU02A, about 50 sub-files
  and `BST29A-1.w3d` load on demand, with `..\NAME.w3d` retries for the missing `astro01`,
  `PPVT01A`, `jvc-p01`. Our build loads the same chain.
* O4 settled: the original opens `e&bologo_low.dds` and the logo shows.
* `river4_256.pld` IS opened by the original (§2.1 says no `.pld` is referenced).
* O3 settled (M2.2): `Build_Scenes`' library switches are 0x446080(false) = `WW3D::Enable_Sorting`,
  0x5EFFD8 `Enable_Static_Sort_Lists(true)`, 0x5EFFD9 `Enable_Munge_Sort_On_Load(true)`, 0x5C60AC
  `Set_Thumbnail_Enabled(false)`, 0x5EFFD7 `Set_Screen_UV_Bias(true)`, 0x5EFFDA
  `Enable_Overbright_Modify_On_Load(true)` (turned on after the loading screen). The caps byte
  +0xDD is `Support_DXTC`.
* The stars add 2,000 polygons to Max Polys at star budget 200 (30,729 in the original's frame 300
  against 28,729 without stars).
* O7: on the host the original wrote `FreeTextureRam=-3145728`, `TextureReduction=2`; ours
  fixes it (D53).

## 1. Program flow

### 1.1 WinMain (0x4235C0)

1. Saves `hInstance`. Installs the WWDebug message and assert handlers (`WWDebug_Install_Message_Handler`
   0x46C840 and `WWDebug_Install_Assert_Handler` 0x46C850; the app handlers are thunks 0x401140 and
   0x40169F).
2. `LoadStringA` 1 = title `"E&B Video Test"` and 2 = class `"VidTestWndClass"`. It registers the
   class (0x423930): `CS_HREDRAW|CS_VREDRAW`, WndProc 0x4239D0, icon 0x65, cursor 0x66, brush
   `COLOR_WINDOW+1`. Then `CreateWindowExA(0, class, title, WS_OVERLAPPEDWINDOW, 0,0,0,0)`,
   `ShowWindow(nCmdShow)` and `UpdateWindow`. WW3D sizes the window later.
3. `WWMath::Init()`.
4. `new WW3DAssetManager` (0x408C bytes). It sets byte +0x4075 = 1, which is
   `Set_Activate_Fog_On_Load(true)`: +0x4074 is `WW3D_Load_On_Demand`, which stays false (checked in
   `Create_Render_Obj` 0x43E083). It registers the particle emitter loader:
   `Register_Prototype_Loader(&_ParticleEmitterLoader)` (vtable +0x58, object 0x60C9BC, built by
   part_ldr's static initialiser 0x469510).
   **No file factory and no path are set.** Assets load from the current directory through the
   default factory. The string `..\data\client\mixfiles\` belongs to the library's
   `ThumbnailManagerClass` (texture thumbnails, `*.mix` scan), not to the app.
5. `WW3D::Init(hwnd, NULL, false)`.
6. `ArgvClass::Init(lpCmdLine, "@")`, then `new ArgvClass(0,0)` into global 0x5E5E28 (0x402200).
7. *Setup_Device* (0x423BE0): on failure `MessageBoxA(NULL, "Your hardware does not support the
   requested video mode settings.", MB_ICONINFORMATION)`, then shutdown and **return -1**.
8. *Build_Scenes* (0x423F20): on failure shutdown, then `MessageBoxA("Failed to build the scenes
   required for the test.", MB_ICONEXCLAMATION)` and **return -2**.
9. `new SplineDatabase(NULL)` (0x850 bytes, 0x41C660) into global 0x5EF19C, then `Load("splines.wsd")`
   (0x41CB20).
10. Main loop:
    ```
    for (;;) {
        do { Frame(); } while (!PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE));
        if (!GetMessage(&msg, 0, 0, 0)) break;
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
    ```
11. Shutdown: *Release_Scenes* (0x424C10), delete the text renderer, delete the spline database,
    release the font, `asset_manager->Free_Assets()` (+0xC) and delete it,
    `LookupTableMgrClass::Shutdown` (0x511B50), `DestroyWindow`, `WW3D::Shutdown()`, delete the
    ArgvClass. **Returns `msg.wParam`**.

Exit codes: 1 = all tests finished and `results.ini` written; -3 = aborted by a key or window close
before the end (nothing written); -1 = video mode unsupported; -2 = scene build failed.

### 1.2 WndProc (0x4239D0)

* `WM_KEYDOWN`, `WM_CLOSE` and `WM_DESTROY`: if the tests have not finished (flag 0x5EF188 = 0),
  `PostQuitMessage(-3)`. Otherwise *Write_Results* (0x427FC0), which posts quit with code 1.
* `WM_ACTIVATEAPP`: `wParam` 0 calls `WW3D::On_Deactivate_App()`, otherwise
  `WW3D::On_Activate_App()` (0x445FD0, an empty stub that returns 0). Both then fall through to
  `DefWindowProc`.
* Everything else goes to `DefWindowProc`.

### 1.3 Command line and device (0x423BE0)

The arguments go through ArgvClass (`Find_Value(name)` returns the token after the name). There are
three helpers: *Arg_String* 0x4022C0, *Arg_Int* 0x4022F0 (atoi) and *Arg_Bool* 0x402330, where the
first character T, t, 1, Y or y means true and anything else means false.

| Argument | Default | Effect |
|---|---|---|
| `-device "name"` | `WW3D::Get_Render_Device_Name(0)` | device name for `WW3D::Set_Render_Device(const char*, …)` |
| `-resx N` | 640 | width, kept in 0x5EF194 |
| `-resy N` | 480 | height, kept in 0x5EF198 |
| `-depth N` | 16 | bit depth |
| `-windowed B` | true | windowed |
| `-sync B` | false | `WW3D::Set_Ext_Swap_Interval(sync)` (0x445280 → `DX8Wrapper::Set_Swap_Interval`) |
| `-filter N` | 0 | `WW3D::Set_Texture_Filter(N)` |

E&BConfig starts it as `VIDTEST.EXE -device "%s" -resx %d -resy %d -depth %d -windowed %s -filter %d`.
It never passes `-sync`.

The sequence:
```
if (WW3D::Set_Render_Device(dev, resx, resy, depth, windowed, true) != WW3D_ERROR_OK) return false;
WW3D::Set_Ext_Swap_Interval(sync); WW3D::Set_Texture_Filter(filter);
int ram = DX8Wrapper::Get_Free_Texture_RAM();          // 0x44C020: IDirect3DDevice8::GetAvailableTextureMem
ini.Put_Int("Info", "FreeTextureRam", ram);
reduction = ram < 8000000 ? 2 : ram < 16000000 ? 1 : 0;   // compared as signed int -> double
ini.Put_Int("Results", "TextureReduction", reduction);
WW3D::Set_Texture_Reduction(reduction);
WW3D::Enable_Large_Texture_Extra_Reduction(!(caps && caps->byte_0xDD));   // DX8Wrapper current caps (0x5F57C8)
ini.Put_String("Info","Device",dev); Put_Int Width, Height, BitDepth; Put_Bool("Info","Windowed",windowed);
```
`ini` is a global INIClass, saved only by *Write_Results*.

Note for modern hardware: `ram` is a signed int. A device that reports 2 GB or more of texture
memory gives a negative value, so the program picks `TextureReduction=2`. To match the original,
keep the signed comparison.

### 1.4 Frame (0x424E90)

`WW3D::Sync(GetTickCount())` (0x445260). It then pushes the current `timeGetTime()` into the
frame-time ring (0x423E70, a global at 0x5ED1E0 with 2,000 slots; entries older than 1 s are dropped,
so the slot count is the frames in the last second). Then it dispatches on the test state (0x5EF15C):
0 Fill Rate (0x426D80), 1 Polygon Budget (inline here), 2 Land LOD (0x4267E0), anything else
"finished": it sets flag 0x5EF188 = 1 and calls *Write_Results*.

## 2. Scenes (Build_Scenes 0x423F20)

`WW3D::Enable_…(false)` (0x446080: sets a WW3D bool at 0x5C609C and invalidates
`TheDX8MeshRenderer`; which flag it is remains an open question). The asset manager's byte +0x101D
is set to 1 first (open question).

| Object | Global | Setup |
|---|---|---|
| Sun light | 0x5EF150 | `new LightClass(1)` (directional). Fields: [0x26] intensity 1.0, ambient (0,0,0), diffuse (1,1,1), specular (1,1,1), attenuation range 1e6/1e6, [0x37..0x39] = (0,0,-1); `Set_Force_Visible(1)`; `Set_Position(0,0,6000)` |
| 3D camera | 0x5EF148 | `new CameraClass`; `Set_Transform(identity)`; `Set_Clip_Planes(10, 10000)`; `Set_View_Plane(hfov 0.7853982, vfov 0.5759587)` (45°, 33°) |
| 2D camera | 0x5EF14C | `new CameraClass`; `Set_View_Plane(min (-1,-0.75), max (1,0.75))`; `Set_Position(0,0,320)`; `Set_Clip_Planes(300, 1000)` |
| Font | 0x5EF158 | `asset_mgr->Get_Font3DInstance("font.tga")` (vtable +0x48) |
| Text | 0x5EF154 | `new Render2DTextClass(font)`; `Set_Screen_Resolution(0,0,resx,resy)`; `Set_Coordinate_Range(same)`; `Enable_Additive(false)`; `Enable_Alpha(true)` |
| Overlay scene | 0x5EF144 | `new SimpleSceneClass`; `Load_3D_Assets("mu01a.w3d")`; `Create_Render_Obj("mu01a")` (0x5EF170) added |

Loading screen: `Reset()`, then the text location is set to (resx/2-50, resy/2) and it draws
`"Loading..."` in white. `WW3D::Sync(GetTickCount())`, then
`Begin_Render(true, true, (0.2,0.2,0.2))`, render the overlay scene with the 2D camera
(0x424E40: `WW3D::Render(overlay, cam2, false, false, black)`), `Render2DClass::Render()` on the
text, and `End_Render(true)`.

`Load_3D_Assets` (vtable +8) runs, return values ignored, on: `back01.w3d ths01.w3d jvc01.w3d
bst29a.w3d pvs01.w3d pvs02.w3d pvs03.w3d pvs04.w3d`, then the 18 player ships `p{j,t,p}v{e,f,t}01a.w3d`
and their `…01a-1.w3d` animations (order pj, pt, pp × e, f, t). **The 18 ship files are not in
EBCONFIG**, so those loads fail without an error.

| Scene | Global | Contents |
|---|---|---|
| Background | 0x5EF13C | `back01` (0x5EF164). Its `Get_Cost()` starts the poly total |
| Stars | 0x5EF138 | *StarField* (0x5EF16C, `new` 0xA4 bytes, ctor 0x4228A0 with budget 200), `Init(0)` (0x422B20); `Get_Num_Polys()` added to the poly total |
| Space | 0x5EF134 | ambient (0.5,0.5,0.5); sun light; `Create_Render_Obj("mu02")` (0x5EF160) |
| Land | 0x5EF140 | ambient (0.5,0.5,0.5); sun light; *Land* `Create("land.ini", true)` (0x4068F0, 0x5EF168), `Create_Decorations(0)` (0x415C40); the land; **the same `mu02` object** |

*Max_LOD_And_Cost(mu02)* (0x424AA0) walks the object. For an HLOD (class id 0x19) it calls
`Set_LOD_Level(Get_LOD_Count()-1)` (finest LOD) and takes `Get_Cost()`, then recurses into the
additional models (`Get_Additional_Model_Count` / `Peek_Additional_Model`) and sums. Any object named
`BST29A` gets `Get_HAnim("BST29A.BST29A-1")` (format `"%s.%s-1"`) and
`Set_Animation(anim, 0, ANIM_MODE_LOOP)`. The sum goes to 0x5EF184 ("Max Polys" of the land test).
0x5EF180 = the sum + cost(back01) + star polys ("Max Polys" of the poly test).

Release_Scenes (0x424C10) releases, in order: light, both cameras, the space, background, land,
stars and overlay scenes, back01, land, star field, mu01a and mu02.

### 2.1 The art

`w3dinfo.py` output:

* **mu01a.w3d**: HLOD `MU01A` with bones CAMERA, BOX01-04, CIRC, CIRC01-04, RING and SYSTEM, plus
  12 two-polygon meshes. Textures `bg05 bg_circ bg_circ2 bg_circ3 bg_frame2 bg_ring bg_system
  bologo_low`. This is the frame and logo overlay drawn on every frame through the 2D camera. Note
  `bologo_low.tga`; the folder has `E&BOLogo_low.dds`, so the logo is probably missing (check
  against the original).
* **back01.w3d**: HLOD `BACK01`, mesh `SPHERE01` with 120 polygons, textures `neb_blue neb_grey`.
  This is the nebula sky sphere. (BACK01A.W3D, `neb0401`, is not referenced.)
* **mu02.w3d**: aggregate `mu02` = HLOD `MU02A` with `PVS04` attached to bones PVS04…PVS13
  (10 copies).
* **mu02a.w3d**: HLOD `MU02A` with a `CAMERA` bone and sub-objects `BST29A JVC01 THS01 PVS01 PVS02
  PVS03 PPVT01A WARRIOR EXPLORER DEFENDER PRIVATEER SCOUT ENFORCER SEEKER TRADESMAN SENTINEL`. This
  is the whole space scene.
  * BST29A.W3D (station, 145+ meshes, textures `bst-ring bst2904 bst2906 elec01-j elec02-j flicker
    gas lf`) with BST29A-1.W3D, its looping animation.
  * JVC01 and THS01 are aggregates of JVC01A/B and THS01A-F.
  * PVS01-04 are models with LOD chains.
  * Each ship name (DEFENDER.W3D …) is an aggregate of three parts, for example
    `explorer = SJH31 SJW31 SJP31` and `privateer = SPH11 SPP11 SPW11`. These are the
    SJ*/ST*/SP* files with `sjh0x, grid_env01, namedecal, noseart` textures.
  * PPVT01A is absent.
* **land**: `land.ini` → `river4_256.pgm` (height), `river_color4_256.tga` (colour),
  `hm03_det.tga` → HM03_DET.DDS (detail). Decorations `TDtree01a-04a.w3d` (`tdtree01.tga`).
  **RIVER4.PLD and river4_256.pld are not referenced** (no `.pld` string in the exe).
* **stars**: `stars.ini` texture `hy_stars.tga` → HY_STARS.DDS (WW3D's texture loader substitutes
  `.dds` for `.tga`).
* **font.tga**: the Font3D font.
* Not referenced by VIDTEST: MC01*.W3D (dated 2000), ROAD.TGA, BACK01A.W3D, RIVER4*.PLD.

**Open question O1: MU02.W3D is never named in a `Load_3D_Assets` call, and load-on-demand is off**,
yet `Create_Render_Obj("mu02")` has to succeed: a NULL would crash `Add_Render_Object` and
0x424AA0. One of the earlier loads must register the `mu02` prototype, perhaps through a dependency
load in the 2002 aggregate or HLOD loaders. To settle it, log the original's file opens (Procmon)
in M2.1.

### 2.2 Cameras and camera paths

* **Polygon test** (0x425A80). The 3D camera transform is `mu02->Get_Bone_Transform("camera")`
  with the columns rotated by -90° twice. This is the usual Max camera-bone conversion,
  `Rotate_Y(-90°)` then `Rotate_Z(-90°)`; to be verified in M2.1. back01 gets a +90° rotation about Z,
  translated to the camera's position (a sky sphere that follows the camera). No animation file
  exists for MU02A, so **the camera is static unless the HLOD animates it**. The movement comes from
  the BST29A loop animation.
* **Land LOD test** (0x4269A0). First `mu02->Set_Transform(T)` with T = identity rotation and
  translation (0, -20000, 500), so the station complex sits above the terrain. The camera is then
  derived from mu02's `camera` bone the same way.
* **Fill rate** and the overlay use no camera path.

### 2.3 Per-frame rendering

| State | Begin_Render | Scenes (camera, clip planes) |
|---|---|---|
| Fill rate (0x426E20) | clear colour+Z, black | 10 × *Draw_Fullscreen_Quad*(0.04,0.04,0.05); overlay; text |
| Polygon (0x425A80) | Z only (no colour clear), black | `PredictiveLODOptimizerClass::Clear()`; `space->Visibility_Check(cam)`; `Optimize_LODs(PolyBudget)`; background (cam, clip 20..100000); stars (clip 50..150, camera depth range [0x2F]=1,[0x30]=1); space (clip 100..100000, depth range 0..1); overlay; text |
| Land (0x4269A0) | colour+Z, colour = sky colour from *Update_Lighting* | *Update_Lighting(out,false)*; `Land::Update_Patches(cam, LandLOD)` (0x4078F0); land scene `Set_Fog_Enable(true)`, `Set_Fog_Color(sky)`, `Set_Fog_Range(1000, 80000)`; LOD clear; `land->Visibility_Check(cam)`; `Optimize_LODs(LandBudget)`; render land; overlay; text |

All three skip rendering while the window is iconic (`IsIconic`).

*Draw_Fullscreen_Quad(colour)* (0x426EC0) saves the world and view transforms and sets them to
identity. The shader is the global at 0x5EFC9C (= 0x00084433: depth LEQUAL, no Z write, src ONE /
dst ONE, primary gradient MODULATE, texturing off, cull on, i.e. the additive-solid preset) with
depth compare forced to ALWAYS (`|7`), and `VertexMaterialClass::Get_Preset(PRELIT_DIFFUSE=0)`. It
builds a `DynamicVBAccessClass(BUFFER_TYPE_DYNAMIC_DX8=2, FVF 0x252, 4)` quad at z=1 over (±1,±1),
all four vertices diffuse = colour × 255, and a `DynamicIBAccessClass(2, 6)` with indices
0,2,1,0,3,2. Then `Set_Vertex_Buffer`, `Set_Index_Buffer(ib,0)`, `Draw_Triangles(0,2,0,4)`, and the
transforms are restored. Ten additive passes leave the screen at about (0.4,0.4,0.5).

### 2.4 Lighting (*Update_Lighting* 0x4250B0, `(Vector3 *out, bool full)`)

```
a = fmod(GetTickCount() * 5e-5, 2π); if (a > π) a = 2π - a;  // one cycle every 125.7 s
sun = normalize(cos a, 0, sin a);  e = asin(sun.z); if (e < 0) { sun = -sun; e = -e; }
t = (π/2 - e) / π + 0.5;                                       // time of day, global 0x5EF190
```
If the spline database exists, for each of `LightColor.Earth`, `LightIntensity.Sun`,
`SkyColor.Earth`, `SkyIntensity.Sun` and `SkyEmissive.Default` it fetches the `_R/_G/_B` curves
(0x41D030) and evaluates each at t (curve vtable +0x14). Light colour = LightColor × LightIntensity,
sky = SkyColor × SkyIntensity (+ SkyEmissive). Without splines, the defaults are a base of
(0.1,0.1,0.1), an ambient offset of (0,0,0.3) and a light of (1,1,0.2). These defaults are read from
pseudocode; re-check them against the asm before relying on them.

The outputs:
* the sun light's direction field ([0x37..0x39]) = -sun;
* its diffuse = clamp01(light colour);
* the land scene's ambient = the sky term (`Set_Ambient_Light`);
* a lighting record (sun direction, colours, 0.005 bias) goes to the Land's lightmap: `full`
  rebuilds all of it (0x40A9F0), otherwise `Land::Update_Lightmap_ManualThreaded` (0x40AB10) does a
  time-sliced update;
* `*out` = clamp01(sky colour), which becomes the fog and clear colour.

The full rebuild is called once at the polygon → land transition; the incremental update runs on
every land-test frame.

### 2.5 Text overlay (*Draw_Overlay* 0x425F80, every frame of every state)

`Debug_Statistics::Record_Texture_Mode(0)`, then it builds a string (every format ends in `\n`):

| State | Lines |
|---|---|
| 0 | `Fill Rate Test` |
| 1 | `Polygon Budget Test`, `Budget = %d` (PolyBudget), `Max Polys = %6.0f` (0x5EF180), `Star Budget = %d` |
| 2 | `Land LOD Test`, `LOD = %d`, `Budget = %d` (LandBudget), `Max Polys = %6.0f` (0x5EF184) |
| done | `Fill Rate FPS= %d`, `Space Budget= %d`, `Star Budget= %d`, `Land LOD Level= %d`, `Land Budget= %d` |
| always | `%d KPoly/s %d FPS`: KPoly/s = `Debug_Statistics::Get_DX8_Polygons()` (0x474410) × frames-in-last-second / 1000 |

**PeakPolyRate** (0x5EF174) = the largest KPoly/s value seen. The text is drawn at (0, resy-150) in
white. When finished, `Press a key to continue...` is drawn first in red (0xFFFF0000), but the same
frame already posts the quit, so the prompt never waits. Then `Render2DClass::Render()`.

## 3. Tests and results.ini

State and data (initialised data at 0x5C5508): budgets byte[0] Space = 9, byte[1] Land = 9,
byte[2] Star = 2, then the frame counters poly 0x5C550C = -1, land 0x5C5510 = -1 and fill
0x5C5514 = -1. The LOD level 0x5EF17C starts at 0. Units: Space ×1000 polygons, Land ×1000, Star ×200
stars.

**Fill Rate** (state 0, 0x426D80). The first frame stores `timeGetTime()`. It counts frames; after
the 61st (counter > 60):
`FillRateFPS = ftol(60 / ((timeGetTime() - t0) * 0.001))`, then state 1.

**Polygon Budget** (state 1, 0x424F00). The first entry renders 10 warm-up frames and sets the
counter to 0, then starts the timer (GetTickCount). Every 15 frames:
```
fps = 15 * 1000 / (now - t0)
if (fps >= 30 && Space != 0xFF && Space*1000 <= MaxPolys(0x5EF180)) {
    if (Star < 11) Star++;  starfield->budget(+0xA0) = Star*200;
    Space++; counter = 0; render 2 frames; t0 = now;
} else {
    Space--; Star--; state = 2; Update_Lighting(&tmp, true);   // land lightmap full build
}
```
Every frame then renders once (0x425A80) with `Optimize_LODs(Space*1000)`.

**Land LOD** (state 2, 0x4267E0). Same structure: 10 warm-up frames, then every 15 frames
```
if (fps >= 30 && Land != 0xFF && Land*1000 <= MaxPolys(0x5EF184)) {
    if (!toggle && LOD < 7) { LOD++; toggle = 1; land->dirty(+0x136) = 1; }
    else { Land++; toggle = 0; }
    counter = 0; 2 frames; t0 = now;
} else {
    if (!toggle) Land--; else { LOD--; land->dirty = 1; }
    state = 3;
}
```
So it raises LOD and the polygon budget in turn, and steps back the last raise on failure.

**Results** (*Write_Results* 0x427FC0), into the global INIClass, then `Save("results.ini")` (to
the current directory) and `PostQuitMessage(1)`:

| Section | Key | Value |
|---|---|---|
| Info | FreeTextureRam | at setup, GetAvailableTextureMem (signed) |
| Info | Device, Width, Height, BitDepth, Windowed | at setup, from the command line (Windowed is written yes/no) |
| Info | PeakPolyRate | max KPoly/s |
| Results | TextureReduction | at setup: 2 (< 8 MB), 1 (< 16 MB), 0 |
| Results | FillRateFPS | §3 Fill Rate |
| Results | PolyBudget | Space × 1000 |
| Results | LandPolyBudget | Land × 1000 |
| Results | LandLOD | LOD (0..7) |
| Results | StarBudget | Star × 200 |

The sample `RESULTS.INI` in EBCONFIG (2005, RADEON 9500, 800×600×32 windowed) fits this:
FillRateFPS 144, PolyBudget 30000, LandPolyBudget 28000, LandLOD 7, StarBudget 2000, PeakPolyRate
10651, TextureReduction 0. The ceilings: Star reaches its cap of 11 and is then decremented, which
gives 2,000; Space stops at the MaxPolys or 30 fps limit.

## 4. E&B scene classes

All of them are 2002 copies of E&B's `G\app\shared` code. The client has the same keys and strings,
in the same order in .rdata (heightfield/land, decorations, spline, stars). The class names come from
client asserts (`LandClass`; `app\shared\stars.cpp`, `spline.cpp`, `spline.h`, `spline.inl`). The
land and decoration source file names are not known; the client's nearest assert file
(`ui_registration.cpp`) is not it.

### 4.1 HeightField (land.ini [HEIGHTMAP])

*HeightField::Load(ini, flag)* 0x402420 ~ client 0x4634F0 (0.90). It reads `HEIGHT_MAP` (string),
`HSCALE` (1.0), `VSCALE` (1.0), `WRAPPING` (false) and `FLIP_PGM` (true), and loads the PGM (magic
`P2`, ASCII) through 0x4026B0 = client 0x4637C0. It stores the size, size-1, log2 (`Find_POT_Log2`),
half the size, the horizontal scale HSCALE/size and the vertical scale VSCALE/maxval. The ctor is
0x4023D0 = client 0x4634A0; *Sample(u,v)→Vector3* is 0x402AE0. The ray and box collision code
(0x402DD0-0x404B30) is ~ client 0x463F00-0x465D30 (0.62-0.83) and uses `CollisionMath::Collide`.

### 4.2 Land (RenderObjClass, 0x1228 bytes)

| VIDTEST | Role | Client |
|---|---|---|
| 0x4068F0 | *Land::Create(ini, flag)*: new, `Load_Assets`, flag stored at +0x135 | (inlined in the client's mainview.cpp: `land%04d.ini`) |
| 0x405410 | ctor | = 0x468E90 |
| 0x405750 | *Load_Assets(ini)*: [GENERAL] `HASSEA` (false); [LAND] `COLOR_MAP`, `DETAIL_MAP`, `XDETAIL_SCALE`/`YDETAIL_SCALE` (1.0), `FLIP_TGA` (false), `Patch_Grid_Width` (16), `LIGHTMAP_CHUNK` (8); if HASSEA, [SEA] `SWATCH` ("water_swatch", a render object), `SEA_LEVEL` (0), `DETAIL_SCALE`..`DETAIL_SCALE4` (1.0). It creates TextureClass objects for the colour and detail maps, builds the lightmap and `TextureLoader::Request_Foreground_Loading` | ~ 0x4691D0 `LoadAssets` (0.99) |
| 0x4069D0 | dtor | ~ 0x46A480 (1.00) |
| 0x4078F0 | *Update_Patches(camera, lod)*: rebuilds the patch grid around the camera cell when the cell moves or the dirty flag +0x136 is set | ~ 0x46B3C0 (0.96) |
| 0x40A9F0 | lightmap full rebuild | ~ 0x46D900 (0.76) |
| 0x40AB10 | `Land::Update_Lightmap_ManualThreaded` (WWPROFILE label) | ~ 0x46DA70 (0.72), the same label |
| 0x40AD70, 0x40BE00, 0x40BFB0, 0x40D830, 0x40F290-0x40FD50 | patch tessellation and LOD | ~ 0x46DDD0, 0x46EF60, 0x46F120, 0x470F80, 0x472F80-0x473BC0 (0.72-0.89) |
| 0x410B90 (10,350 B) | render: DX8 dynamic VB/IB, render states, material | ~ 0x474B30 (**0.65**, the largest 2002/2004 difference) |

The **case-sensitive INI trap**: WW `INIClass` looks entries up by a case-sensitive `CRC::String`.
EBCONFIG's `land.ini` writes `PATCH_GRID_WIDTH=16`, which does not match `Patch_Grid_Width`, so the
default (16) is used and the result happens to be the same. `FOGNEAR`, `FOGFAR` and `DETAIL_FUNC`
are never read (no such strings in VIDTEST; the client has `FOGNEAR`/`FOGFAR` strings without
code references). The land's fog comes from the hard-coded 1000/80000.

### 4.3 Decorations (decoration.ini)

*Land::Create_Decorations(set)* 0x415C40 ~ client 0x479D00 (0.78). It reads section `[set]` (`[0]`):
`NUM_DECOS` (5000), `MIN_SCALE`, `MAX_SCALE`, `NUM_PROTOS` and `PROTO%d` (TDTREE01A..04A). The
prototypes come from 0x414B50 ~ client 0x478B50 (`DECO%d`, `DecID`, `Prototypes`, `Scale`, `Asset`,
`SHADOW%d`; default `TDTREE01A`). With `Random4Class(0x1105)` each decoration takes a random
prototype, a random scale in [MIN,MAX] and random (u,v) → `HeightField::Sample`. If the land has a
sea, positions at or below `SEA_LEVEL` are retried. It also gets a random rotation about Z. Building
is done by 0x416170 = client 0x47A2F0 (byte-identical, 7 KB). There is also a `rb`/`wb` file pair
near these strings (a decoration cache?), not seen in use.

### 4.4 Stars (stars.cpp)

*StarField* is a RenderObjClass, 0xA4 bytes. Its ctor(budget) 0x4228A0 stores the budget at +0xA0
and +0x8C (byte), and the layer vector is at +0x90/+0x94. *Init(system)* 0x422B20 ~ client 0x4829E0
(0.78) loads `stars.ini` and reads the layers from sections `STAR%.4d\%d` (system, i), then
`(system/100*100)`, then `Default\%d`. Each layer is read by *StarLayer::Load(ini, section)* 0x421F90
~ client 0x481DB0 (0.99): `Texture`, `NumStars`, `TextureLog2Width`, `MinSize`, `MaxSize`, `QMCOffset`,
`PickRandomly`, `Hemisphere` and `Quads`. It builds `PointGroupClass` (frame row/column log2, point
colours) and places the points with `QMCSamplingClass`. Total stars go in +0x9C. The polygon test
changes the rendered count through +0xA0. 0x421E30 (uses `CameraClass::Get_View_Matrix`) ~ client
0x481B70; dtor 0x422950 ~ client 0x47FCD0.

### 4.5 Spline database (spline.cpp; splines.wsd)

* ctor 0x41C660 (0x850 bytes; a 0x200-entry table, list heads, name).
* *Load(file)* 0x41CB20: RawFileClass, then ChunkLoadClass through 0x41F620 and 0x41EC80; it uses
  `Curve1DClass` and `HermiteSpline1DClass` templates.
* *Get_Color_Splines(name, &r, &g, &b)* 0x41D030: looks up `name_R`, `_G`, `_B` with 0x41CFF0.
* dtor 0x41C880 ~ client 0x489C70.
* Chunk save code (0x41F170-0x41F430) is linked.
* Client counterparts sit at 0x485000-0x48A000 (0x41D380/0x41D5B0 ~ 0x485990 0.91, 0x41D7E0 ~
  0x486040 0.97, 0x41E820 ~ 0x487190 0.94, 0x420230 ~ 0x489050, 0x4207C0 ~ 0x489660, 0x420930 ~
  0x489830). The client loads `splines.wsd` from mousetbl.cpp (call site 0x726412 in client 0x726100).

The **mailslot** `\\.\mailslot\G\SplineDatabaseUpdate`: 0x41E2E0 does `CreateMailslotA`,
`GetMailslotInfo` and `ReadFile`, then reloads through 0x41EE30. It is a live-edit hook for a
spline editor, but **nothing references its thunk (0x401834)**, so VIDTEST never opens the
mailslot. The client keeps the string with no code reference. A rebuild can omit it.

## 5. Library API used by the app

These are the direct calls from the 475 app functions (names of exact-matched tree functions), the
virtual calls resolved through the vtables, and the unnamed callees identified here. Templates that
were instantiated in app objects (VectorClass, DynamicVectorClass, std::string, the
`RenderObjClass::Set_*` inlines) are not listed.

* **WW3D**: `Init`, `Shutdown`, `Set_Render_Device(const char*,int,int,int,int,bool)` (0x444820),
  `Get_Render_Device_Name` (0x444900), `Set_Ext_Swap_Interval` (0x445280), `Set_Texture_Filter`,
  `Set_Texture_Reduction`, `Enable_Large_Texture_Extra_Reduction`, `Sync` (0x445260), `Begin_Render`,
  `Render(SceneClass*,CameraClass*,bool,bool,const Vector3&)`, `End_Render`, `On_Activate_App`
  (0x445FD0), `On_Deactivate_App`, an unidentified bool setter 0x446080 (O3).
* **DX8Wrapper**: `Get_Free_Texture_RAM` (0x44C020), `Set_Swap_Interval`, `Set_Transform`,
  `Set_Vertex_Buffer(DynamicVBAccessClass&)`, `Set_Index_Buffer(DynamicIBAccessClass&,int)`,
  `Draw_Triangles(4 args)`, `Set_DX8_Render_State`, `Get_DX8_Render_State_Value_Name`,
  `Set_Material`, the inline `Set_Shader`, and the current-caps pointer. Also
  `DynamicVBAccessClass` and `DynamicIBAccessClass` (ctor, dtor, `WriteLockClass`),
  `DX8VertexBufferClass`, `DX8IndexBufferClass`, `VertexMaterialClass::Get_Preset`,
  `VertexMaterialClass` (ctor, `Compute_CRC`), `MaterialPassClass::Install_Materials`,
  `ShaderClass` (the preset global 0x5EFC9C).
* **Scene and objects**: `SimpleSceneClass` (ctor, `Add_Render_Object`, `Visibility_Check`),
  `SceneClass` (`Set_Ambient_Light`, `Set_Fog_Enable`, `Set_Fog_Color`, `Set_Fog_Range`),
  `CameraClass` (ctor, `Set_Transform`, `Set_Position`, `Set_Clip_Planes`, `Set_View_Plane` in both
  the fov and min/max forms, `Get_View_Matrix`, `Cull_Box`, the depth-range fields [0x2F]/[0x30]),
  `LightClass(type)` and its fields, and `RenderObjClass`: ctor, dtor, `Class_ID`, `Get_Name`,
  `Get_Num_Polys`, `Set_Transform`, `Set_Position`, `Get_Position`, `Get_Bone_Transform(const char*)`,
  `Set_Animation(HAnimClass*,float,int)`, `Get_Cost`, `Set_LOD_Level`, `Get_LOD_Count`,
  `Set_Force_Visible`, `Get_Bounding_Box`, `Get_Bounding_Sphere`, `Set_User_Data`.
  `HLodClass::Get_Additional_Model_Count`/`Peek_Additional_Model` (vtable +0x1F0/+0x1F4).
  `PredictiveLODOptimizerClass::Clear`/`Optimize_LODs(float)`. `PointGroupClass` (ctor,
  `Set_Frame_Row_Column_Count_Log2`, `Set_Point_Color`, render). `MeshClass::Get_Model`,
  `MeshGeometryClass::Get_Vertex_Normal_Array`, `MeshMatDescClass::Get_Shader`/`Get_Single_Texture`,
  `MaterialInfoClass::Get_Texture`.
* **Assets and textures**: `WW3DAssetManager` (ctor, dtor, `Load_3D_Assets(const char*)`,
  `Create_Render_Obj`, `Get_HAnim`, `Get_Font3DInstance`, `Register_Prototype_Loader`,
  `Free_Assets`, the `Activate_Fog_On_Load` field) and `_ParticleEmitterLoader`. `TextureClass` ctor
  (file name) and `Get_Surface_Level`, `SurfaceClass::Copy`, `TextureLoader::Request_Foreground_Loading`.
* **2D**: `Render2DTextClass(Font3DInstanceClass*)` with `Reset` (vtable +4), the location fields and
  `Draw_Text(const char*,unsigned)`. `Render2DClass` (`Set_Screen_Resolution`,
  `Set_Coordinate_Range`, `Enable_Additive`, `Enable_Alpha`, `Render`).
* **Statistics and debug**: `Debug_Statistics::Get_DX8_Polygons` (0x474410), `Record_Texture_Mode`
  (0x474110), `WWProfileSampleClass` / `WWProfileManager::Start_Profile`/`Stop_Profile` (0x4569C0,
  0x456AE0), `WWDebug_Install_Message_Handler`/`WWDebug_Install_Assert_Handler`.
* **wwlib**: `ArgvClass` (`ArgvClass(bool,bool)`, `Init`, `Find_Value`), `INIClass` (ctor, dtor,
  `Load`, `Is_Loaded`, `Find_Section`, `Get_Bool/Int/Float/String`, `Put_Int/Bool/String/Float`,
  `Save`), `RawFileClass` (ctor, dtor, `Open`, `Close`), `ChunkLoadClass` (ctor 0x464C30,
  `Open_Chunk`, `Close_Chunk`, `Read`), `ChunkSaveClass` (`Begin_Chunk`, `End_Chunk`, `Write`),
  `StringClass` (`Format`, `operator+=`, `Get_Length`, `Resize`, `Get_String`, `Free_String`),
  `Random4Class`, `GenericNode` (`Link`/`Unlink`), `MultiListObjectClass` dtor, `RefCountClass`,
  `FileClass::Printf_Indented`, `CRC_Stringi`, `LookupTableMgrClass::Shutdown` (0x511B50).
* **wwmath**: `WWMath::Init`, `WWMath::Inv_Sqrt`, `Matrix3D::Rotate_Vector`, `CollisionMath::Collide`
  (box and ray forms), `QMCSamplingClass` (ctor, `Sample`), `Curve1DClass` (ctor 0x505390,
  `operator=`), `HermiteSpline1DClass` (template instances), `Find_POT_Log2`.
* **CRT**: atoi, sprintf/_snprintf, fmod, asin, sin/cos, rand, operator new/delete, `_ftol`, timeGetTime.

For L2/M2.1 the link needs, besides `ww3d2` and `wwlib`: wwmath, wwdebug (profile and handlers),
and wwshade, which comes in through WW3D's shader and material code. No wwaudio, wwui, network,
Miles or Bink. Imports: WINMM, AVIFIL32 (from the libraries), KERNEL32, USER32 (30), GDI32 (16),
ADVAPI32, ole32 and OLEAUT32, with D3D8 loaded at run time.

## 6. Function table

Every app function in 0x401000-0x4288A0 (475 of them, from Ghidra; 435 are incremental-link thunk
targets ("T"), 40 are statics called directly ("s"); Ghidra did not give the other 2 of the 437 thunk
targets a function of their own). The client column is "=" for byte-identical
(255) or "~" for the best mnemonic similarity at 0.6 or above within client 0x460000-0x490000 and
0x720000-0x730000 (48). Several byte-identical small functions occur many times in the client
("+N more"). The names that `docs/data/symbols.json` or the gaps reports give for these client
addresses are often wrong guesses (for example 0x4026B0 = client 0x4637C0 is listed as
`~SoundRenderObjClass::Set_Hidden?`), so only addresses are given here. Sizes are Ghidra body sizes
(byte spans for the 10 whose bodies Ghidra missed).

**command line (ArgvClass wrapper)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x402200 | 101 | T | ~ 0x466E70 (0.93) | create ArgvClass (global 0x5E5E28) |
| 0x402290 | 29 | T | = 0x75de83 | delete ArgvClass |
| 0x4022C0 | 32 | T |  | Arg_String(name, default) |
| 0x4022F0 | 42 | T |  | Arg_Int(name, default) (atoi) |
| 0x402330 | 60 | T |  | Arg_Bool(name, default): T/t/1/Y/y = true |
| 0x402380 | 58 | T | = 0x4a1590 |  |

**heightfield: PGM height map (land.ini [HEIGHTMAP])**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x4023D0 | 49 | T | = 0x4634a0 | HeightField ctor |
| 0x402420 | 467 | T | ~ 0x4634F0 (0.90) | HeightField::Load(ini [HEIGHTMAP]: HEIGHT_MAP HSCALE VSCALE WRAPPING FLIP_PGM) |
| 0x402680 | 28 | T | = 0x463790 (+1 more) | HeightField dtor/reset |
| 0x4026B0 | 835 | T | = 0x4637c0 | PGM file reader (flip) |
| 0x402AE0 | 99 | T | = 0x463bf0 | HeightField::Sample(u,v) -> Vector3 |
| 0x402B70 | 50 | T | = 0x463c80 (+1 more) |  |
| 0x402BC0 | 50 | T | = 0x463c80 (+1 more) |  |
| 0x402C10 | 72 | T | = 0x463d20 (+1 more) |  |
| 0x402C80 | 72 | T | = 0x463d20 (+1 more) |  |
| 0x402CF0 | 70 | T | = 0x463e00 |  |
| 0x402D60 | 68 | T | ~ 0x463E70 (0.61) |  |

**heightfield: ray/box collision and sampling**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x402DD0 | 610 | T | ~ 0x463F00 (0.76) |  |
| 0x4030F0 | 621 | T | ~ 0x464290 (0.77) |  |
| 0x403400 | 264 | T |  |  |
| 0x403560 | 1750 | T | ~ 0x4647C0 (0.83) |  |
| 0x403E00 | 82 | T | = 0x447c40 |  |
| 0x403E80 | 886 | T | ~ 0x465030 (0.70) |  |
| 0x4042E0 | 1694 | T | ~ 0x4654E0 (0.80) |  |
| 0x404B30 | 922 | T | ~ 0x465D30 (0.62) |  |
| 0x404FC0 | 34 | T | = 0x466140 |  |

**heightfield**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x405000 | 733 | T | = 0x4663e0 |  |
| 0x4053A0 | 29 | T | = 0x4668f0 (+2 more) |  |
| 0x4053D0 | 42 | T | = 0x42f250 |  |

**Land: construction and land.ini loading**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x405410 | 618 | T | = 0x468e90 | Land ctor (RenderObjClass, 0x1228 bytes) |
| 0x405720 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x405750 | 3490 | T | ~ 0x4691D0 (0.99) | Land::Load_Assets(ini): [GENERAL]/[LAND] keys, colour/detail maps, lightmap |
| 0x406870 | 84 | T | = 0x46a320 |  |
| 0x4068F0 | 164 | T | = 0x46a3a0 | Land::Create(ini_name, flag) factory |

**Land: patch grid, LOD, lightmap**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x4069D0 | 1148 | T | ~ 0x46A480 (1.00) | Land dtor |
| 0x406F70 | 1754 | T | = 0x46aa40 |  |
| 0x407810 | 162 | T | = 0x46b2e0 |  |
| 0x4078F0 | 1297 | T | ~ 0x46B3C0 (0.96) | Land::Update_Patches(camera, lod) |
| 0x407F60 | 152 | T | = 0x46bb20 |  |
| 0x408030 | 149 | T | = 0x46bbf0 |  |
| 0x408100 | 354 | T | = 0x46bcc0 |  |
| 0x4082D0 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x408300 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x408330 | 93 | T | = 0x46bef0 |  |
| 0x4083B0 | 751 | T | = 0x46bf70 |  |
| 0x408760 | 345 | T | = 0x438ba0 (+4 more) |  |
| 0x408920 | 102 | T | = 0x439050 (+4 more) |  |
| 0x4089B0 | 16 | T | = 0x46c570 (+4 more) |  |
| 0x4089D0 | 34 | T | = 0x439540 (+4 more) |  |
| 0x408A10 | 23 | T | = 0x42b2c0 (+4 more) |  |
| 0x408A40 | 8 | T |  |  |
| 0x408A50 | 8 | T |  |  |
| 0x408A60 | 12 | T |  |  |
| 0x408A80 | 7 | T |  |  |
| 0x408AA0 | 32 | T | = 0x42f060 (+4 more) |  |
| 0x408AD0 | 1 | T |  |  |
| 0x408AF0 | 32 | T | = 0x42f060 (+4 more) |  |
| 0x408B20 | 137 | T | = 0x4627f0 |  |
| 0x408BE0 | 4 | T |  |  |
| 0x408C00 | 6 | T |  |  |
| 0x408C20 | 3 | T |  |  |
| 0x408C40 | 3 | T |  |  |
| 0x408C60 | 3 | T |  |  |
| 0x408C80 | 3 | T |  |  |
| 0x408CA0 | 3 | T |  |  |
| 0x408CC0 | 1 | T |  |  |
| 0x408CE0 | 1 | T |  |  |
| 0x408D00 | 4 | T |  |  |
| 0x408D20 | 3 | T |  |  |
| 0x408D40 | 5 | T |  |  |
| 0x408D60 | 5 | T |  |  |
| 0x408D80 | 5 | T |  |  |
| 0x408DA0 | 5 | T |  |  |
| 0x408DC0 | 5 | T |  |  |
| 0x408DE0 | 5 | T |  |  |
| 0x408E00 | 5 | T |  |  |
| 0x408E20 | 1 | T |  |  |
| 0x408E40 | 3 | T |  |  |
| 0x408E60 | 3 | T |  |  |
| 0x408E80 | 3 | T |  |  |
| 0x408EA0 | 3 | T |  |  |
| 0x408EC0 | 3 | T |  |  |
| 0x408EE0 | 5 | T |  |  |
| 0x408F00 | 5 | T |  |  |
| 0x408F20 | 15 | T |  |  |
| 0x408F40 | 15 | T |  |  |
| 0x408F60 | 3 | T |  |  |
| 0x408F80 | 3 | T |  |  |
| 0x408FA0 | 5 | T |  |  |
| 0x408FC0 | 3 | T |  |  |
| 0x408FE0 | 3 | T |  |  |
| 0x409000 | 5 | T |  |  |
| 0x409020 | 5 | T |  |  |
| 0x409040 | 5 | T |  |  |
| 0x409060 | 5 | T |  |  |
| 0x409080 | 5 | T |  |  |
| 0x4090A0 | 1 | T |  |  |
| 0x4090C0 | 1 | T |  |  |
| 0x4090E0 | 1 | T |  |  |
| 0x409100 | 1 | T |  |  |
| 0x409120 | 7 | T |  |  |
| 0x409140 | 7 | T |  |  |
| 0x409160 | 3 | T |  |  |
| 0x409180 | 3 | T |  |  |
| 0x4091A0 | 6 | T |  |  |
| 0x4091C0 | 3 | T |  |  |
| 0x4091E0 | 6 | T |  |  |
| 0x409200 | 3 | T |  |  |
| 0x409220 | 3 | T |  |  |
| 0x409240 | 3 | T |  |  |
| 0x409260 | 13 | T |  |  |
| 0x409280 | 7 | T |  |  |
| 0x4092A0 | 3 | T |  |  |
| 0x4092C0 | 3 | T |  |  |
| 0x4092E0 | 3 | T |  |  |
| 0x409300 | 3 | T |  |  |
| 0x409320 | 3 | T |  |  |
| 0x409340 | 3 | T |  |  |
| 0x409360 | 21 | T | = 0x463030 |  |
| 0x409390 | 21 | T | = 0x463060 |  |
| 0x4093C0 | 9 | T |  |  |
| 0x4093E0 | 29 | T | = 0x4630b0 |  |
| 0x409410 | 12 | T |  |  |
| 0x409430 | 29 | T | = 0x463100 |  |
| 0x409460 | 12 | T |  |  |
| 0x409480 | 29 | T | = 0x463150 |  |
| 0x4094B0 | 9 | T |  |  |
| 0x4094D0 | 29 | T | = 0x4631a0 |  |
| 0x409500 | 9 | T |  |  |
| 0x409520 | 29 | T | = 0x4631f0 |  |
| 0x409550 | 9 | T |  |  |
| 0x409570 | 24 | T | = 0x463240 |  |
| 0x4095A0 | 3 | T |  |  |
| 0x4095C0 | 11 | T |  |  |
| 0x4095E0 | 4 | T |  |  |
| 0x409600 | 10 | T |  |  |
| 0x409620 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x409650 | 7 | T |  |  |
| 0x409670 | 24 | T | = 0x463340 |  |
| 0x4096A0 | 24 | T | = 0x463370 |  |
| 0x4096D0 | 7 | T |  |  |
| 0x4096F0 | 3 | T |  |  |
| 0x409710 | 6 | T |  |  |
| 0x409730 | 19 | T |  |  |
| 0x409760 | 19 | T |  |  |
| 0x409790 | 19 | T |  |  |
| 0x4097C0 | 8 | T |  |  |
| 0x4097D0 | 8 | T |  |  |
| 0x409820 | 737 | T | = 0x46c730 |  |
| 0x409BD0 | 1858 | T | = 0x46cae0 |  |
| 0x40A500 | 527 | T | = 0x46d410 |  |
| 0x40A7A0 | 355 | T | = 0x46d6b0 |  |
| 0x40A970 | 21 | T |  |  |
| 0x40A9A0 | 63 | T | = 0x46d8b0 |  |
| 0x40A9F0 | 212 | T | ~ 0x46D900 (0.76) | Land lightmap: full rebuild from sun light |
| 0x40AB10 | 474 | T | ~ 0x46DA70 (0.72) | Land::Update_Lightmap_ManualThreaded (incremental) |
| 0x40AD70 | 449 | T | ~ 0x46DDD0 (0.82) |  |
| 0x40AFC0 | 1087 | T | = 0x46e120 |  |
| 0x40B510 | 1002 | T | = 0x46e670 |  |
| 0x40BA00 | 318 | T | = 0x46eb60 |  |
| 0x40BB90 | 282 | T | = 0x46ecf0 |  |
| 0x40BD00 | 50 | T | = 0x46ee60 |  |
| 0x40BD50 | 31 | T | = 0x46eeb0 |  |
| 0x40BD80 | 56 | T | = 0x46eee0 |  |
| 0x40BDD0 | 31 | T | = 0x46ef30 |  |

**Land: patch tessellation (ROAM-style)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x40BE00 | 232 | T | ~ 0x46EF60 (0.86) |  |
| 0x40BF30 | 86 | T | = 0x46f0a0 |  |
| 0x40BFB0 | 4891 | T | ~ 0x46F120 (0.72) |  |
| 0x40D7A0 | 105 | T | = 0x470ef0 |  |
| 0x40D830 | 5380 | T | ~ 0x470F80 (0.78) |  |
| 0x40F290 | 582 | T | ~ 0x472F80 (0.89) |  |
| 0x40F580 | 238 | T | = 0x473290 |  |
| 0x40F6B0 | 670 | T | ~ 0x4733C0 (0.78) |  |
| 0x40FA00 | 669 | T | ~ 0x473780 (0.84) |  |
| 0x40FD50 | 665 | T | ~ 0x473BC0 (0.84) |  |
| 0x4100A0 | 84 | T | = 0x474000 (+4 more) |  |
| 0x410120 | 21 | T | = 0x474080 (+4 more) |  |
| 0x410150 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x410180 | 693 | T | = 0x4740e0 (+4 more) |  |
| 0x4104F0 | 24 | T | = 0x474450 (+4 more) |  |
| 0x410520 | 47 | T | = 0x474480 (+4 more) |  |
| 0x410560 | 23 | T |  |  |
| 0x410590 | 31 | T | = 0x474530 |  |
| 0x4105C0 | 30 | T | = 0x474560 (+3 more) |  |
| 0x4105F0 | 47 | T | = 0x474590 |  |
| 0x410630 | 179 | T | = 0x4745d0 |  |
| 0x410720 | 145 | T | = 0x4746c0 |  |
| 0x4107F0 | 395 | T | = 0x474790 |  |
| 0x4109F0 | 34 | T | = 0x474990 |  |
| 0x410A30 | 258 | T | = 0x4749d0 (+1 more) |  |

**Land: rendering (DX8 dynamic VB/IB)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x410B90 | 10350 | T | ~ 0x474B30 (0.65) |  |
| 0x413E20 | 707 | s |  |  |
| 0x414200 | 45 | T | = 0x45d360 (+4 more) |  |
| 0x414240 | 811 | T | = 0x478240 |  |
| 0x414640 | 25 | T | = 0x478640 |  |
| 0x414670 | 25 | T | = 0x478670 |  |
| 0x4146A0 | 415 | T | = 0x4786a0 |  |
| 0x4148B0 | 142 | T | = 0x4788b0 |  |

**decorations (decoration.ini)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x414970 | 370 | T | = 0x478970 |  |
| 0x414B50 | 1156 | T | ~ 0x478B50 (0.87) | decoration prototypes (DECO%d, DecID, Prototypes, Scale) |
| 0x415110 | 809 | T | = 0x4791d0 |  |
| 0x415510 | 906 | T | = 0x4795d0 |  |
| 0x415990 | 541 | T | = 0x479a50 |  |
| 0x415C40 | 1051 | T | ~ 0x479D00 (0.78) | Land::Create_Decorations(set): decoration.ini [n] |

**decorations: build/render, mesh helpers**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x416170 | 7059 | T | = 0x47a2f0 | decoration batches build |
| 0x418400 | 209 | s |  |  |
| 0x418520 | 1269 | s |  |  |
| 0x418B60 | 104 | T | = 0x4340a0 (+4 more) |  |
| 0x418BF0 | 20 | T | = 0x434070 (+4 more) |  |
| 0x418C20 | 104 | T | = 0x4340a0 (+4 more) |  |
| 0x418CB0 | 20 | T | = 0x434070 (+4 more) |  |

**templates (std::string, VectorClass, DynamicVectorClass) and inlines**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x418CE0 | 62 | T | = 0x42a6c0 (+4 more) |  |
| 0x418D30 | 21 | T | = 0x474080 (+4 more) |  |
| 0x418D60 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x418D90 | 21 | T | = 0x474080 (+4 more) |  |
| 0x418DC0 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x418DF0 | 21 | T | = 0x474080 (+4 more) |  |
| 0x418E20 | 21 | T | = 0x474080 (+4 more) |  |
| 0x418E50 | 34 | T | = 0x42a900 (+4 more) |  |
| 0x418E90 | 32 | T | = 0x466780 (+4 more) |  |
| 0x418EC0 | 42 | T | = 0x45cb00 (+4 more) |  |
| 0x418F00 | 36 | T | = 0x47d0f0 (+4 more) |  |
| 0x418F40 | 96 | T | = 0x432b80 |  |
| 0x418FC0 | 34 | T | = 0x42a900 (+4 more) |  |
| 0x419000 | 35 | T | = 0x47d170 (+1 more) |  |
| 0x419040 | 34 | T | = 0x42a900 (+4 more) |  |
| 0x419080 | 34 | T | = 0x42a900 (+4 more) |  |
| 0x4190C0 | 18 | T | = 0x47d230 |  |
| 0x4190F0 | 84 | T | = 0x474000 (+4 more) |  |
| 0x419170 | 21 | T | = 0x474080 (+4 more) |  |
| 0x4191A0 | 21 | T | = 0x474080 (+4 more) |  |
| 0x4191D0 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x419200 | 21 | T | = 0x474080 (+4 more) |  |
| 0x419230 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x419260 | 72 | T | = 0x42a820 |  |
| 0x4192D0 | 69 | T | = 0x47d3d0 |  |
| 0x419340 | 56 | T | = 0x4667d0 (+1 more) |  |
| 0x419390 | 3 | T |  |  |
| 0x4193B0 | 391 | T | = 0x47d690 (+4 more) |  |
| 0x4195B0 | 51 | T | = 0x47d890 (+4 more) |  |
| 0x419600 | 17 | T | = 0x42a940 |  |
| 0x419630 | 282 | T | = 0x42a970 |  |
| 0x4197A0 | 60 | T | = 0x47d8e0 |  |
| 0x4197F0 | 676 | T | = 0x432d50 |  |
| 0x419B50 | 512 | T | = 0x42cab0 |  |
| 0x419DD0 | 986 | T | = 0x47d930 |  |
| 0x41A2B0 | 651 | T | = 0x47de10 |  |
| 0x41A5F0 | 522 | T | = 0x47e150 |  |
| 0x41A890 | 6 | T |  |  |
| 0x41A8B0 | 89 | T | = 0x42ab30 |  |
| 0x41A909 | 33 | s |  |  |
| 0x41A930 | 136 | s |  |  |
| 0x41AA10 | 201 | T | = 0x42cd30 |  |
| 0x41AB20 | 56 | T | = 0x47e3f0 (+4 more) |  |
| 0x41AB70 | 41 | T | = 0x47e440 (+4 more) |  |
| 0x41ABB0 | 56 | T | = 0x47e510 (+1 more) |  |
| 0x41AC00 | 41 | T | = 0x47e560 (+1 more) |  |
| 0x41AC40 | 48 | T | = 0x47e5a0 |  |
| 0x41AC80 | 39 | T | = 0x47e5e0 |  |
| 0x41ACC0 | 19 | T | = 0x42aae0 (+4 more) |  |
| 0x41ACF0 | 56 | T | = 0x47e3f0 (+4 more) |  |
| 0x41AD40 | 41 | T | = 0x47e440 (+4 more) |  |
| 0x41AD80 | 3 | T |  |  |
| 0x41ADA0 | 100 | T | = 0x47e6e0 (+1 more) |  |
| 0x41AE30 | 227 | T | = 0x47e770 |  |
| 0x41AF60 | 39 | T | = 0x45cdc0 (+4 more) |  |
| 0x41AFA0 | 94 | T | = 0x47e8e0 (+1 more) |  |
| 0x41B020 | 36 | T | = 0x47d0f0 (+4 more) |  |
| 0x41B060 | 49 | T | = 0x45d0a0 (+4 more) |  |
| 0x41B0B0 | 80 | T | = 0x47e9f0 (+1 more) |  |
| 0x41B120 | 74 | T | = 0x47ea60 |  |
| 0x41B190 | 87 | T | = 0x47ead0 |  |
| 0x41B210 | 116 | T | = 0x47eec0 |  |
| 0x41B2B0 | 133 | T | = 0x47ef60 |  |
| 0x41B370 | 62 | T | = 0x47f020 (+4 more) |  |
| 0x41B3C0 | 75 | T | = 0x47f070 (+4 more) |  |
| 0x41B430 | 27 | T | = 0x47f0e0 |  |
| 0x41B460 | 355 | T | = 0x433700 |  |
| 0x41B630 | 1 | T |  |  |
| 0x41B650 | 95 | T | = 0x47f110 |  |
| 0x41B6D0 | 24 | T | = 0x474450 (+4 more) |  |
| 0x41B700 | 73 | T | = 0x45d2c0 (+4 more) |  |
| 0x41B770 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x41B7A0 | 83 | T | = 0x433a10 |  |
| 0x41B820 | 113 | T | = 0x47f260 |  |
| 0x41B8E0 | 191 | T | = 0x47f300 |  |
| 0x41B9D0 | 81 | T | = 0x47f3f0 |  |
| 0x41BA50 | 70 | T | = 0x47f470 |  |
| 0x41BAC0 | 47 | T | = 0x47f4e0 |  |
| 0x41BB00 | 39 | T | = 0x47f520 |  |
| 0x41BB40 | 47 | T | = 0x4837b0 |  |
| 0x41BB80 | 21 | T |  |  |
| 0x41BBB0 | 81 | T | ~ 0x483820 (0.82) |  |
| 0x41BC30 | 169 | T |  |  |
| 0x41BD10 | 130 | T |  |  |
| 0x41BDD0 | 144 | T | = 0x483af0 |  |
| 0x41BE90 | 144 | T |  |  |

**spline database (spline.cpp, splines.wsd)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x41BF50 | 104 | T | ~ 0x483CF0 (0.86) |  |
| 0x41BFE0 | 236 | T | = 0x483db0 |  |
| 0x41C110 | 236 | T | = 0x483db0 |  |
| 0x41C240 | 231 | T |  |  |
| 0x41C370 | 97 | T | ~ 0x481960 (0.83) |  |
| 0x41C400 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x41C430 | 290 | T | ~ 0x481610 (0.62) |  |
| 0x41C5B0 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x41C5E0 | 96 | T | = 0x481840 |  |
| 0x41C660 | 381 | T | = 0x4845f0 | SplineDatabase ctor |
| 0x41C840 | 45 | T | = 0x45d360 (+4 more) |  |
| 0x41C880 | 524 | T | ~ 0x489C70 (0.88) | SplineDatabase dtor |
| 0x41CB20 | 290 | T |  | SplineDatabase::Load(file) (chunked splines.wsd) |
| 0x41CCA0 | 365 | T |  |  |
| 0x41CE70 | 66 | T |  |  |
| 0x41CEE0 | 35 | T |  |  |
| 0x41CF20 | 160 | T |  |  |
| 0x41CFF0 | 35 | T |  |  |
| 0x41D030 | 263 | T | ~ 0x486040 (0.79) | Get colour splines name_R/_G/_B |
| 0x41D190 | 146 | T |  |  |
| 0x41D260 | 222 | T | ~ 0x485790 (0.71) |  |
| 0x41D380 | 434 | T | ~ 0x485990 (0.91) |  |
| 0x41D5B0 | 434 | T | ~ 0x485990 (0.91) |  |
| 0x41D7E0 | 311 | T | ~ 0x486040 (0.97) |  |
| 0x41D970 | 311 | T | ~ 0x486040 (0.97) |  |
| 0x41DB00 | 465 | T | ~ 0x485990 (0.79) |  |
| 0x41DD60 | 478 | T |  |  |
| 0x41DFC0 | 191 | T | = 0x4867b0 |  |
| 0x41E0B0 | 107 | T | = 0x4868a0 |  |
| 0x41E140 | 39 | T | = 0x486930 |  |
| 0x41E180 | 61 | T | = 0x486970 |  |
| 0x41E1D0 | 87 | T | = 0x4869c0 |  |
| 0x41E250 | 110 | T | = 0x486a40 |  |
| 0x41E2E0 | 165 | T | = 0x486ad0 | mailslot \\.\mailslot\G\SplineDatabaseUpdate poll (not referenced) |
| 0x41E3C0 | 30 | T |  |  |
| 0x41E3F0 | 57 | T | = 0x486c40 |  |
| 0x41E440 | 60 | T | = 0x486c90 |  |
| 0x41E490 | 25 | T |  |  |
| 0x41E4C0 | 58 | T |  |  |
| 0x41E510 | 63 | T |  |  |
| 0x41E560 | 56 | T |  |  |
| 0x41E5B0 | 41 | T |  |  |
| 0x41E5F0 | 435 | T |  |  |
| 0x41E820 | 327 | T | ~ 0x487190 (0.94) |  |
| 0x41E9D0 | 94 | T | = 0x487360 |  |
| 0x41EA50 | 16 | T |  |  |
| 0x41EA70 | 402 | T | ~ 0x487400 (0.87) |  |
| 0x41EC80 | 324 | T | ~ 0x487660 (0.75) |  |
| 0x41EE30 | 281 | T |  |  |
| 0x41EFA0 | 113 | T |  |  |
| 0x41F040 | 106 | T | = 0x487e70 |  |
| 0x41F0D0 | 126 | s |  |  |
| 0x41F170 | 218 | s |  |  |
| 0x41F290 | 82 | s |  |  |
| 0x41F320 | 198 | s |  |  |
| 0x41F430 | 268 | s |  |  |
| 0x41F590 | 3 | s |  |  |
| 0x41F5B0 | 70 | s | ~ 0x48CAF0 (0.65) |  |
| 0x41F620 | 315 | T |  |  |
| 0x41F7B0 | 87 | T | = 0x4887f0 |  |
| 0x41F830 | 225 | s |  |  |
| 0x41F960 | 554 | s |  |  |
| 0x41FC20 | 82 | s |  |  |
| 0x41FCB0 | 192 | s |  |  |
| 0x41FDA0 | 260 | s |  |  |
| 0x41FF00 | 175 | s |  |  |
| 0x41FFE0 | 213 | s |  |  |
| 0x420100 | 105 | s |  |  |
| 0x420190 | 127 | s |  |  |
| 0x420230 | 547 | T | ~ 0x489050 (0.85) |  |
| 0x420500 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x420530 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x420580 | 105 | T | = 0x4893f0 |  |
| 0x420610 | 207 | T | = 0x489480 |  |
| 0x420720 | 49 | T | = 0x489590 |  |
| 0x420770 | 60 | T |  |  |
| 0x4207C0 | 267 | T | ~ 0x489660 (0.83) |  |
| 0x420930 | 578 | T | ~ 0x489830 (0.86) |  |
| 0x420C30 | 33 | T | = 0x489bc0 |  |
| 0x420C70 | 42 | T | = 0x45cb00 (+4 more) |  |
| 0x420CB0 | 27 | T | = 0x45cbe0 (+4 more) |  |
| 0x420CE0 | 397 | T | = 0x489c70 |  |
| 0x420EE0 | 42 | T | = 0x45cb00 (+4 more) |  |
| 0x420F20 | 27 | T | = 0x45cb40 (+4 more) |  |
| 0x420F50 | 157 | T | = 0x45c0e0 (+4 more) |  |
| 0x421020 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x421050 | 51 | T | = 0x457ca0 (+4 more) |  |
| 0x4210A0 | 17 | T |  |  |
| 0x4210D0 | 51 | T | = 0x457ca0 (+4 more) |  |
| 0x421120 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x421150 | 45 | T | = 0x48a0e0 |  |
| 0x421190 | 75 | T | = 0x48a120 |  |
| 0x421200 | 69 | T | = 0x457fa0 (+3 more) |  |
| 0x421270 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x4212A0 | 157 | T | = 0x45c0e0 (+4 more) |  |
| 0x421370 | 87 | T | = 0x48a190 |  |
| 0x4213F0 | 186 | T | = 0x48a210 (+2 more) |  |
| 0x4214E0 | 39 | T | = 0x45cdc0 (+4 more) |  |
| 0x421520 | 81 | T | = 0x48a340 |  |
| 0x4215A0 | 27 | T | = 0x45cbe0 (+4 more) |  |
| 0x4215D0 | 192 | T | = 0x48a3f0 (+2 more) |  |
| 0x4216C0 | 67 | T | = 0x48a4e0 |  |
| 0x421730 | 219 | T | = 0x48a550 |  |
| 0x421850 | 62 | T | = 0x48a7f0 (+4 more) |  |
| 0x4218A0 | 174 | T | = 0x45cce0 (+4 more) |  |
| 0x421980 | 39 | T | = 0x45cdc0 (+4 more) |  |
| 0x4219C0 | 51 | T | = 0x48a960 (+4 more) |  |
| 0x421A10 | 27 | T | = 0x45cb40 (+4 more) |  |
| 0x421A40 | 176 | T | = 0x45cf20 (+4 more) |  |
| 0x421B20 | 38 | T | = 0x48ab30 (+4 more) |  |
| 0x421B60 | 73 | T | = 0x45d2c0 (+4 more) |  |
| 0x421BD0 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x421C00 | 45 | T | = 0x45d360 (+4 more) |  |
| 0x421C40 | 73 | T | = 0x45d2c0 (+4 more) |  |
| 0x421CB0 | 30 | T | = 0x42f010 (+4 more) |  |

**stars (stars.cpp, stars.ini)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x421CE0 | 28 | T | = 0x481a20 |  |
| 0x421D10 | 213 | T | = 0x481a50 |  |
| 0x421E30 | 258 | T | ~ 0x481B70 (0.88) |  |
| 0x421F90 | 1809 | T | ~ 0x481DB0 (0.99) | StarLayer::Load(ini section) |
| 0x422880 | 7 | T |  |  |
| 0x4228A0 | 89 | T | = 0x4826e0 | StarField ctor(budget) |
| 0x422920 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x422950 | 207 | T | ~ 0x47FCD0 (0.88) | StarField dtor |
| 0x422A60 | 138 | T | = 0x482920 |  |
| 0x422B20 | 623 | T | ~ 0x4829E0 (0.78) | StarField::Init(system): stars.ini STAR%.4d\%d, then Default\%d |
| 0x422E30 | 81 | T | = 0x482d80 |  |
| 0x422EB0 | 7 | T |  |  |
| 0x422ED0 | 54 | T | = 0x482e20 |  |
| 0x422F20 | 34 | T | = 0x42a900 (+4 more) |  |
| 0x422F60 | 21 | T | = 0x474080 (+4 more) |  |
| 0x422F90 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x422FC0 | 21 | T | = 0x474080 (+4 more) |  |
| 0x422FF0 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x423020 | 21 | T | = 0x474080 (+4 more) |  |
| 0x423050 | 30 | T | = 0x46be90 (+4 more) |  |
| 0x423080 | 521 | T | = 0x436420 (+4 more) | StarField layer list add |
| 0x423320 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x423350 | 40 | T | = 0x483600 (+4 more) |  |
| 0x423390 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x4233C0 | 40 | T | = 0x483600 (+4 more) |  |
| 0x423400 | 30 | T | = 0x42f010 (+4 more) |  |
| 0x423430 | 40 | T | = 0x483600 (+4 more) |  |
| 0x423470 | 8 | T |  |  |
| 0x423480 | 8 | T |  |  |
| 0x423490 | 3 | T |  |  |
| 0x4234B0 | 1 | T |  |  |
| 0x4234D0 | 32 | T | = 0x42f060 (+4 more) |  |
| 0x423560 | 10 | s |  |  |
| 0x423580 | 12 | s |  |  |

**vidtest main (WinMain, scenes, tests, results)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x4235C0 | 695 | T |  | WinMain |
| 0x423930 | 121 | s |  | register window class |
| 0x4239D0 | 134 | s |  | WndProc |
| 0x423BE0 | 495 | s |  | parse command line, set render device, texture reduction, results [Info] |
| 0x423E50 | 13 | T |  | frame-rate ring reset |
| 0x423E70 | 102 | T |  | frame-rate ring: push timeGetTime, drop >1 s old |
| 0x423F00 | 7 | T |  | frames in last second |
| 0x423F20 | 2346 | s |  | Build_Scenes |
| 0x424AA0 | 280 | s |  | Max_LOD_And_Cost(robj): max LOD, sum Get_Cost, loop BST29A anim |
| 0x424C10 | 447 | s |  | Release_Scenes |
| 0x424E40 | 59 | s |  | Render overlay scene (mu01a, 2D camera) |
| 0x424E90 | 398 | s |  | Frame: test state machine |
| 0x4250B0 | 1999 | s |  | Update_Lighting(out, full): sun from time, splines, land lightmap |
| 0x425A80 | 1015 | s |  | Poly test frame |
| 0x425F80 | 1700 | s |  | Draw text overlay |
| 0x4267E0 | 349 | s |  | Land LOD test step |
| 0x4269A0 | 769 | s |  | Land LOD test frame |
| 0x426D80 | 116 | s |  | Fill rate test step |
| 0x426E20 | 120 | s |  | Fill rate test frame |
| 0x426EC0 | 3409 | s |  | Draw full-screen additive quad(colour) |
| 0x427F80 | 12 | T |  |  |
| 0x427FA0 | 14 | T |  |  |
| 0x427FC0 | 240 | s |  | Write results.ini, PostQuitMessage(1) |

**inline copies (StringClass, DX8Wrapper)**

| Address | Size | Kind | Client | Role |
|---|---|---|---|---|
| 0x4280F0 | 166 | T | = 0x510be0 |  |
| 0x4281D0 | 39 | T | = 0x510cc0 |  |
| 0x428210 | 22 | T | = 0x48e910 (+1 more) |  |
| 0x428240 | 92 | T | = 0x42a100 (+2 more) |  |
| 0x4282C0 | 14 | T |  |  |
| 0x4282E0 | 1161 | T | = 0x511e00 |  |

## 7. Open questions and build order

### 7.1 Open questions

* **O1** (settled, see the corrections at the top) How `mu02` gets loaded (§2.1). Find out with Procmon on the original, or by stepping
  through the 2002 `Load_3D_Assets`. It decides whether our build needs an explicit
  `Load_3D_Assets("mu02.w3d")`.
* **O2** The static camera: does MU02A's `CAMERA` bone move? There is no anim file, so probably not.
  Check against screenshots of the original's polygon test.
* **O3** (settled, see the corrections at the top) 0x446080: which WW3D bool setter it is (it invalidates `TheDX8MeshRenderer`). Also asset
  manager +0x101D, and the caps byte +0xDD used for `Enable_Large_Texture_Extra_Reduction`. All
  three need a look at the 2002 WW3D layout against the tree's.
* **O4** (settled: the logo loads as `e&bologo_low.dds`) `bologo_low.tga` against `E&BOLogo_low.dds`: is the logo missing in the original too?
* **O5** The default lighting without splines (§2.4) and the exact spline sum for the sky/ambient:
  read the asm (0x4252C5-0x4257B8) when M2.4 implements it.
* **O6** The 2002 vs 2004 differences in Land render (0x410B90 ~ 0.65), decorations and star init:
  M2.4 has to choose the client's 2004 code (D45/D46 direction) or the 2002 behaviour. The visible
  result should be the same, since the same art and keys are used.
* **O7** The signed `FreeTextureRam` on GPUs reporting 2 GB or more: replicate it (TextureReduction=2)
  or fix it? Proposal: replicate, since the user decides (it changes E&BConfig's registry values).

### 7.2 Suggested build order

* **M2.1, one model** (after L2). WinMain, window, `WW3D::Init`, `Set_Render_Device`, an asset
  manager on the current directory, one camera, the sun light, `Load_3D_Assets("bst29a.w3d")` and
  `Create_Render_Obj("BST29A")` with the BST29A-1 loop, rendered in a loop (the brief's ship
  `p?v?01a` is not in EBCONFIG, so the station is the model to use). Screenshot. Settle O1 here with
  Procmon on the original.
* **M2.2, scenes and text.** The command line (ArgvClass, §1.3), all five scenes (§2), mu02 with
  `Max_LOD_And_Cost`, the camera from the bone (§2.2), back01 following the camera, the overlay
  through the 2D camera, Font3D/Render2DText and the loading screen. Then windowed and fullscreen.
  The star field and the land can be stubs (empty scenes) at this point.
* **M2.3, tests.** The frame-time ring, the state machine (§1.4, §3), *Draw_Fullscreen_Quad*,
  PredictiveLODOptimizer budgets, the overlay text (§2.5), and the INIClass results and exit codes.
  The Land LOD test runs against the stub land (no LOD effect) until M2.4. With that, (b) is met for
  FillRate and PolyBudget, and (c) end to end.
* **M2.4, E&B classes.** StarField (stars.cpp, mostly ~0.99 to the client) first, then the
  SplineDatabase and lighting, then HeightField + Land (load, patches, lightmap, render) and finally
  decorations. Reconstruct from the client's 2004 functions listed in §4 (they are also client work)
  and diff the behaviour against VIDTEST's 2002 pseudocode in `.work/milestones/m2/pseudo/`.
