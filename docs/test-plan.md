# E&B Test Plan

The working copy of the E&B test plan (draft 5, 2026-11-24; D284). The published page is rendered from the same text; sessions change the status columns of §5 and §7 here.

One test suite for two jobs. Now: find and prove behaviour bugs in the rebuild where byte comparison cannot (the functions marked NOT EXACT). After the rebuild is frozen: the oracles that carry the modernization (SDL, OpenAL, FFmpeg), when byte comparison with `client.exe` is gone. Baselines are captured from the retail client on the user's Windows machine.

Companion to: E&B Cross-Platform Port Plan; Tree: master c4dadd26; Working copy: `docs/test-plan.md` (sessions update the status tables there)

## 1. Principles

* **Retail is the truth.** An expected result comes from retail `client.exe` wherever the client's behaviour can be observed. Our own build is the baseline only for things retail can't show: structure layouts, library internals and parser outputs. Those tests are marked as such.
* **Capture once, keep the baseline.** Today's e2e comparison re-runs retail every time and its results go only to `.work/e2e`, with no date in the folder name and no record of the build, the data or the settings; no retail run of the default set, `_play7–10` or sp_* survives (checked 2026-11-24). New baselines are stored in a format-neutral form (PNG, JSON, text) with a manifest that names the retail build, the data version, the settings and the machine that captured them.
* **Two repositories, both in git.** Tools, scenarios and this plan live in `Open-EnB/EnB` (public). Everything that comes from running the game lives in `Open-EnB/baselines` (private): baselines, their manifests, the measured noise, and the inputs a second device needs to repeat a run (replay captures, record tracks, database snapshots). The game's own files go in neither: each device supplies its install and the tools check it against the SHA-256 in the manifest.
* **Usable on any device from the start.** No tool holds a machine's paths: one per-machine config file names the install, the POC folders, antprox, the Net-7 server and the store's checkout. Comparing a run against the store needs neither the retail client nor Windows.
* **Measure retail against itself first.** Retail differs from itself between runs: 153 to 167 of 256 cells in sp_flight, the worst scene that is tolerated, and 207 in `w_entered`, which is skipped for that reason. Capture every baseline at least 3 times, keep all three runs, and set each test's tolerance from that measured noise, not by guessing. The figures are stored as data beside the baseline (the calibration of 2026-11-19 survives only as prose in `shot_mask.txt`).
* **Logs cross devices, frames may not.** Packets, draw tables, camera matrices and audio events are compared across machines as they are. Frames are compared against a baseline of the same capture environment (GPU, driver, OS, resolution) until the difference between two machines has been measured the way run-to-run noise is.
* **Oracles that survive the port.** Every test states its oracle (packets, state dump, frame, draw log, audio log, file snapshot) and how it will be observed once the D3D8 proxy is gone.
* **Target the risk.** 179 entries in `docs/data/not-exact.md` say functional, written from, field-offset or shape. Each P0 or P1 UI and camera test names the risky functions it reaches.

### Five layers of evidence

A function is proven the client's by the lowest layer that can see it. Byte-exact code needs nothing above layer 0. A NOT EXACT function is covered only when a layer 3 or 4 case reaches it with an oracle that would show its difference.

| Layer | What it compares | Tool | What it cannot see |
|---|---|---|---|
| L0 | **Bytes.** Each function against the client's (42,760 exact pairs), and the whole linked image section by section | `verify.py`, `pairs_cmp.py`; `tools/re/imagediff.py` | Which object a relocated field names; anything NOT EXACT |
| L1 | **Static audits.** Relocation targets, argument slots, field offsets, unadopted seed forms, read against the client's code | `relocaudit.py` (D281), the parameter-slot audit, `layoutdiff.py`, `seedcheck.py` | What the reader did not think to compare |
| L2 | **Trace programs.** One subsystem driven with recorded input | udpreplay, auxreplay, kerneltest, clienttest | The UI, the renderer, sound |
| L3 | **Replay scenarios.** The whole client against antprox `/REPLAY` | `e2e.py` (25 scenarios, `_play7-10`) | Anything the recording's server did not send; client input the server must answer |
| L4 | **Live scenarios.** The whole client against the local Net-7 server, retail first | `e2e.py --live` (sb_*, sp_*) | Content the local server lacks |

## 2. What exists today

| Harness | Oracle | Expected result from | Survives the port |
|---|---|---|---|
| **verify** (function pairs) and **imagediff** (whole image) | Bytes against `client.exe`: 42,760 pairs; 1.38 % of the image in place (retail is an incremental `/debug` link, ours is not yet) | Retail `client.exe` | VC6 only |
| **udpreplay** + handshake (54 cases) | EA-UDP datagrams, delivery order, acks, re-encoding | EA's 2004 dumps and the reference decoder's frozen traces | needs CMake |
| **auxreplay**, auxdata_smoke | Aux-data decode and byte round trip | Reference decoder | needs CMake |
| **kerneltest** | Task-kernel trace, 81 lines | Written by hand | portable |
| **clienttest** | SClient packet handling trace (103 checks, about 3,178 lines) | Our first passing run, not retail | needs CMake |
| **movietest** | Movie opened, frames drawn, task removed | Asserts only | Bink-bound |
| **vidtest / drawcheck** | Draw-call group tables for frame 900, pixel diff | Original VIDTEST.EXE, once | D3D8 proxy |
| **e2e default set** (25 scenarios) | Event steps, packets, 16×16-cell shot compare, `expect-ink` | Retail re-run live | D3D8 proxy |
| **_play7–10** recordings | Back home and alive, packets | Retail run | D3D8 proxy |
| **sb_*** live station set (11) | Packet names, `expect-ink` | Retail on local Net-7 | D3D8 proxy |
| **sp_*** live space set (5) | Packets, `expect-pose` (view matrix), draw dumps | Retail on local Net-7 | D3D8 proxy |
| **CI** (`checks.yml`) | Docs, generated files, Python tests | n/a | no C++ build |

The proxy, `tools/harness/d3d8shot.c`, is a fake `d3d8.dll` of about 1,230 lines. It provides back-buffer shots, draw dumps, camera matrices, the file-open log and tagged input injection. All of e2e depends on it.

## 3. Capture tools to build first

These run against retail on Windows. Each one makes a whole family of baselines possible.

| ID | Tool | Records | Unblocks |
|---|---|---|---|
| C1 | **Baseline store and manifest**: `baseline.py capture\|verify\|list`, with the per-machine config file that replaces the paths hard-coded in `clientrun.py` and `e2e.py`. e2e writes a capture straight into the store's checkout under a dated run name. `verify` runs with no retail client present | Per-scenario folders (PNG, JSON, text), all N runs kept. Manifest: retail SHA-256, data version, the client's settings (its registry keys, which are under HKLM's 32-bit view with UAC's per-user VirtualStore copy, and `Data\client\ini`, read only), resolution, GPU, driver, OS, proxy and antprox versions, the scenario's commit, the recording's or database snapshot's hash, measured noise | Everything below |
| C2 | **MSS32.DLL logging proxy** (the same idea as d3d8shot) | Every `AIL_*` call with its sample or stream name, 2D or 3D, position, volume, pan, loop, start and stop, with a frame number | All audio tests; the Miles to OpenAL port |
| C3 | **d3d8shot: draw and camera log per shot**. The proxy already writes the view matrices per frame (`D3D8SHOT_CAMERA`, `camera.txt`) and has a draw dump (`D3D8SHOT_DUMP`); what is left is tying both to each `SHOT` and writing them as JSON | The draw table (FVF, textures, states, prim counts) and the view and projection matrices at every `SHOT` | Render tests with tolerance-free structural checks |
| C4 | **Settings snapshot** (the read-only reading is part of C1's manifest) | The client's registry keys (HKLM's 32-bit view and the per-user VirtualStore copy) and `Data\client\ini`, before and after a scenario, as a diff | Options and settings tests |
| C5 | **Frame-time log** (d3d8shot `Present` timestamps) | Per-frame times for fixed scenes | Performance baselines |
| C6 | **Input: modifier keys and key chords** in the scenario language and the proxy | Shift, Ctrl, Alt, held keys | Vault (Shift+I) and the other screens the scenarios can't reach today |
| C7 | **UI layout query** in place of the hand-kept 87-entry `gadgets.txt` | Gadget rectangles by name from the client | Other resolutions; the port |

Two notes on running the client. When several instances run at once, the harness (`clientrun.py`) renames `enb_mutex_lock` to a name of the same length in each instance's copy of the exe, so `app/main/main_client.cpp` stays as the client has it. A run that does this records the original and the patched exe hashes in its manifest; this recording is to be added. Replay runs use the C# antprox 1.1.0 with antreplay 1.1.0 (machine key `antprox_replay`), because the Rust antprox 1.2 cannot /REPLAY yet. This split is temporary and goes when the Rust proxy replays.

## 4. New test cases

P0 before the port starts; P1 before the phase that needs it; P2 when convenient; Baseline: **R** retail, **O** our build

### UI screens with rewritten functions

| ID | Pri | Scenario | Risky code reached | Oracle | Base |
|---|---|---|---|---|---|
| UI-01 | P0 | Create a character: race, profession, every appearance tab, Random Appearance, ship customisation, name, confirm. Then delete it. The local server's database is restored from a snapshot after the run. | ui_avatr (0x557290, 0x5594B0), ui_shipcust / ui_ship_cust (7), avatar 0x781EA0 / 0x782200 | Packets, shot per tab, avatar draw log | R |
| UI-02 | P0 | Registration and account screens: server list, slot select, every back-out path | ui_registration 0x5FA0B0 (field-offset), 0x5FBD30, 0x5F00C0 | Packets, shots | R |
| UI-03 | P0 | Starbase: enter, walk every room, change rooms, open every terminal, name tags, leave | ui_starbase (9: 0x634140, 0x6366A0, 0x639630, 0x63AC60, 0x63DFE0, 0x640F50, 0x63E930, 0x63E770, 0x63F490) | Shots and draw log per room, camera pose, packets | R |
| UI-04 | P0 | Vault: Shift+I, deposit, withdraw, sort (needs C6) | ui_vault (3) | Packets, shots | R |
| UI-05 | P1 | PDA cargo and equipment: move, stack, equip, unequip, Credits line | ui_pda_cargo (4), ui_pda_equip 0x5D2210, ship_equip_pip | Packets, shots | R |
| UI-06 | P1 | PDA faction page (a known DIFF today: find out why) | pda_faction | Shot | R |
| UI-07 | P1 | Skills, slot data and slot info windows | ui_skills 0x62D730, ui_slot_data, ui_slot_info 0x630070 | Shots | R |
| UI-08 | P1 | Chat: tabs, channels, scroll-back, long lines, symbol chat | chatgadg (4), ui_chat_panel 0x5689D0, ui_symbolchat 0x64A9B0 | Packets, shots | R |
| UI-09 | P1 | The screens scenarios can't reach today: ui_pterm / ptmis / ptnew / ptppl, ui_manu_util, ui_avatar_dialog, ui_interrupt, ui_skill_info, ui_dynamicwindow, ui_scan, ui_shard | ui_scan 0x60D8D0, ui_shard 0x616220 and others | Shots, packets | R |
| UI-10 | P1 | Text rendering: every font, word wrap, clipping, proportional fonts, colour codes | fontsys Update_Text 0x6D64C0, Get_Obj_Space_Bounding_Box, font32bit | Shots of a text test window | R |
| UI-11 | P1 | In-game HTML: a page with GIFs (animated, transparent) and images | html/image/gifload Decode 0x76D990, Set_Data, HtmlImageClass::Draw | Shot; GIF decode hash (unit) | R/O |

### Space, cameras and world

| ID | Pri | Scenario | Risky code reached | Oracle | Base |
|---|---|---|---|---|---|
| SP-01 | P0 | Stargate jump | camctrl_stargate 0x795C80 (field-offset), camctrl_entersector 0x793740 | Camera pose per frame (`D3D8SHOT_CAMERA`), packets. First pass from recording 10 (`/play 10` holds a gate jump), then live | R |
| SP-02 | P0 | Planet landing and take-off | camctrl_land 0x793CE0, camctrl_pland 0x794A90 (field-offset), camctrl_takeoff (shape, field-offset) | Camera pose, packets. First pass from recording 10 (a landing and a take-off), then live; the first exercise of bugs B-01 and B-02 | R |
| SP-03 | P0 | Dock and undock at a station, with pose checks | camctrl_dock 0x792750 | Camera pose, packets | R |
| SP-04 | P1 | Tractor beam on loot; rescue | camctrl_tractor 0x797570 / 0x797770, rescue | Camera pose, packets | R |
| SP-05 | P1 | Galaxy map and starmap: zoom, pan, select, plot a route | galaxy_map_render (5), starmap_pip 0x7C2060, nav_list_line_draw 0x7B9DC0 | Draw log, shots | R |
| SP-06 | P1 | Combat with debris, explosions, camera shake | debrsint (2), cameraeffects, frameint 0x53EB00 | Draw log, shots by brightness, pose | R |
| SP-07 | P1 | Planet view render mode; the darker-sector lead from play1 | mainview Render_Planet_Mode 0x50E6E0, 0x512400; enbscene Apply_Projectors (field-offset) | Draw log, shots | R |
| SP-08 | P1 | Ship decals and environment maps on several hulls | shiputils 0x80EAF0, 0x80EFF0, 0x812390; env_map_manager | Draw log (textures per mesh) | R |
| SP-09 | P2 | Scripted in-game movie. None shipped in the live game (the user), so no scenario can reach it: a load-and-step unit check of the path only | mclient (2), mv3dsnd (2) | Shots, audio log | R |
| SP-10 | P2 | Recordings 2–6 as scenarios | Broad | Packets, alive | R |

### Audio, movies, input and settings

| ID | Pri | Test | Oracle | Base |
|---|---|---|---|---|
| AU-01 | P0 | The audio event log (C2) for every existing scenario: which samples, music themes and streams start and stop, in order. Streams and 3D sounds are compared strictly and 2D UI sounds by count. In station scenes retail's own 3D audio varies between runs (random footstep and greeting samples; streams and 3D sounds identical in 0 of 9 station scenarios, 22 of 25 default, 1 of 4 space), so the strict compare has to be relaxed there by the stored noise (to be built). A missing `_AIL_open_3D_listener@4` import in our exe is an `/incremental:no` pruning artifact: compare import counts against the incremental trial link | Event sequence, with timing tolerance | R |
| AU-02 | P0 | 3D sound: engine, weapons and explosions at known positions in sp_combat and sp_flight | Position, volume and listener per event, with tolerance | R |
| AU-03 | P1 | Music manager: theme changes by sector and combat, silence tails, Load_INI | Event log | R |
| AU-04 | P1 | Bridge and sector ambient sounds | Event log | R |
| MV-01 | P0 | The start-up movie (`eb_sizzle.bik`) played in full: frame shots at fixed frame numbers, length, audio stream | PSNR per frame against retail, frame count | R |
| MV-02 | P1 | Every `.bik` decoded offline by retail Bink and by FFmpeg | Frame hashes or PSNR, audio sample checksums | R |
| IN-01 | P1 | Modifier keys, chords, key repeat, and every hot key in hot_keys.scn with modifiers | Packets, shots | R |
| IN-02 | P2 | Joystick path (`DIRECTX_JOYSTICK`), with a virtual device | Pose, packets | R |
| ST-01 | P1 | Options: change every setting, quit, restart, check that each one is kept | C4 registry and INI diff, shots | R |
| ST-02 | P1 | First run with no settings: the defaults written | C4 diff | R |

### Web panels (replacing BrowserEngine.dll)

The pages will change once authors can use modern standards, so these tests check behaviour, placement and the bridge, not page pixels. Retail is the baseline for where each panel sits and when it opens, not for how pages look.

| ID | Pri | Test | Oracle | Base |
|---|---|---|---|---|
| WB-01 | P0 | Open and close each of the eight panels from its real trigger: `PIT` at the Inter-galactic Net terminal, `HELP`, `CREDITS`, `PRIVACY` in registration, `HTMLDIALOG` from a message box, `RNW`, `SUPPORT` and `BUG` | Opened (yes or no), the panel's name and screen rectangle, the address requested | R |
| WB-02 | P0 | The rectangle follows its UI element at each resolution (the 640×480 UI-space conversion) | Rectangle per resolution | R |
| WB-03 | P0 | The script bridge: a test page reads `window.external.AudioInfo` (IGameDispatch's only member) and calls the new `window.enb` API. Also record the query data each panel sends | A non-empty audio device string; query data equal to retail's for the same panel | R (old) / O (new) |
| WB-04 | P1 | Input: clicks, scrolling, typing into a form, focus moving back to the game when the panel closes, game hot keys disabled while a field has focus | Page-side event log, packets | O |
| WB-05 | P1 | Navigation rules: allowed sites load, others open in the system browser, the bridge is missing on pages from other sites | Navigation log | O |
| WB-06 | P1 | Loading and bad-page behaviour: a slow server, a 404, no network | What the panel shows, and that the game stays responsive | R |
| WB-07 | P1 | The game cursor over a panel, in borderless fullscreen and windowed | Shot from the composited output | O |
| WB-08 | P2 | A standards conformance page (modern CSS layout, ES2020+, fetch, web fonts) on all three engines | Page reports its own results | O |

### Rendering fidelity

| ID | Pri | Test | Oracle | Base |
|---|---|---|---|---|
| RD-01 | P0 | Draw and camera log at every existing e2e shot (C3), kept as committed-format baselines | Draw counts, textures, render states, matrices. Exact on the D3D8 build, structural after the port | R |
| RD-02 | P0 | Frame compare by perceptual metric (SSIM or FLIP) with per-scene thresholds set from retail's own noise, alongside today's 16×16 cell rule | Image metric | R |
| RD-03 | P1 | Render-feature scenes, one per fixed-function feature: fog, specular, alpha test and blend modes, texture stage combiners, environment and bump maps (each `wwshade` shader), lighting | Frame and draw log | R |
| RD-04 | P1 | Resolutions and aspect ratios: 1024×768, 1280×1024, 1920×1080 (needs C7) | Shots, layout query | R |
| RD-05 | P2 | Frame time for fixed scenes (C5) | Median and p95 within a budget | R |

### Build identity (bytes)

| ID | Pri | Test | Oracle | Base |
|---|---|---|---|---|
| IMG-01 | P0 | No exact pair lost: every landing's verify against the baseline (exists, D240) | `pairs_cmp.py`: lost outright is empty | R |
| IMG-02 | P0 | Whole-image diff at every handoff: the one client build linked and compared with retail section by section. The figure only rises; a fall names the landing that caused it | `imagediff.py`: bytes in place, header fields, differing ranges, names in place and in client order | R |
| IMG-03 | P1 | The link reproduced: section set, thunk table, object order, resources, relocations, debug record | `imagediff.py` reports IDENTICAL (the end goal) | R |

### Our-build tests (no retail oracle)

| ID | Pri | Test | Oracle | Base |
|---|---|---|---|---|
| LY-01 | P0 | Packet layout: `static_assert` on the size and every field offset of each packet and aux struct, generated from pkdecode / pkemit. Also covers the 8 `#pragma pack` files | Compile-time asserts | O (VC6) |
| LY-02 | P0 | Serialization round trip: every packet type built, written, read back and compared byte for byte, on 32- and 64-bit builds | Bytes equal to the VC6 build's | O (VC6) |
| UT-01 | P1 | wwmath: vectors, matrices, quaternions, the inline-assembly paths against their C versions | Golden values with float tolerance | O (VC6) |
| UT-02 | P1 | wwlib: strings, hash lists, INI, chunk I/O, RC4, CRC, mix files | Golden values | O (VC6) |
| UT-03 | P1 | ww3d2 loaders: every W3D, DDS and TGA in the data, parsed and hashed | Hash of the parsed result | O (VC6) |
| UT-04 | P1 | wwsaveload: save and load round trip | Bytes | O (VC6) |
| UT-05 | P2 | The HTML view's Parse and the GIF decoder on synthetic files | Golden output | O (VC6) |

The UT tests use synthetic inputs where they can, so they can run on GitHub-hosted runners. Tests that need the retail data run on self-hosted runners.

## 5. Bugs: how one is validated, and the ledger

1. **Found** by an audit (L1), by a failing case, or by the user playing.
2. **Reproduced** with one scenario on retail and on our build. Retail's result is the expected one and goes into the baseline store. A gap the user found is proven live on the local Net-7 server, never on `/REPLAY`. One live client runs on the host at a time.
3. **Fixed** on `client.exe`'s own code, with the address named in the commit, and verified with no pair lost.
4. **Kept**: the scenario joins the set as the bug's regression case and the ledger row gets its case ID. A bug no case reaches yet stays *fixed, not exercised* and names the tool or case it waits for.

A difference between retail and ours is never settled by widening a tolerance. Tolerances come from retail against itself (principle 3).

| ID | Fixed | Where | What the player would see | Found by | Status | Case |
|---|---|---|---|---|---|---|
| B-01 | 2026-11-24 | camctrl_pland 0x794A90 | Planet-landing camera on a wrong path: spline length 1250 for 5000, speeds 0 for 100, start and end tangents, the end key | L1 audit | fixed, not exercised | SP-02 (recording 10) |
| B-02 | 2026-11-24 | camctrl_takeoff 0x796920, 0x796440 | Take-off camera swept the camera's near-clip box, not the ship's model box; two NULL checks the client does not make | L1 audit | fixed, not exercised | SP-02 (recording 10) |
| B-03 | 2026-11-24 | galaxy_map_render 0x7AC130 | The wrong gate lines shown or hidden in every galaxy-map view (two flags swapped into 0x7AD090) | L1 audit | fixed, not exercised | SP-05 |
| B-04 | 2026-11-24 | ui_shipcust constructor 0x622180 | The fourth tutorial voice line (`1492_00_303se.mp3`) never played in ship customisation | L1 audit | fixed, not exercised | UI-01 with AU-01 (needs C2) |
| B-05 | 2026-11-24 | ui_manu 0x5B2480 | A manufacturing-terminal gadget not switched on (`Unknown0CC[1]->Unknown_40(true)` missing) | L1 audit | fixed, not exercised | sb_ manufacturing, with a shot of the gadget |
| B-06 | 2026-11-24 | inv_transaction 0x690D20 | An inventory query left its state set when it was active and not a dump: a later transaction could be refused | L1 audit | fixed, not exercised | UI-05 |
| B-07 | 2026-11-21 | The user's nine client gaps (log entry 2026-11-21) | Six faults in our build fixed, one already working, two the server's | The user | proven live | Name each one's scenario from that log entry (session 1) |
| B-08 | 2026-11-18 | The live station set's three faults (log entry 2026-11-18) | Buy / sell, jobs, refinery, manufacturing | L4 case | proven live on 2026-11-18 (9 of 9, retail and ours); on 2026-11-24 retail failed sb_buysell (0 of 4), sb_dismantle (0 of 3) and sb_emote_dock (0 of 3); the cause was the test state, not the client: avatar 6's cargo drifted between runs (gone with the database state restored before each run) and sb_emote_dock cannot share a session; all pass on retail again (2026-11-25) | sb_*; the three pass on retail again before their baselines are captured |
| B-09 | 2026-11-16 | ui_talkt reference count | A crash on talking to a vendor | L4 case | proven live | sb_ talk |
| B-10 | 2026-11-25 | ui_ptmis / ui_ptnew / ui_ptppl 0x5DED10 / 0x5DFDE0 / 0x5E0E00 | The petition terminal's Missions, News and People rows never found their widgets (widget-name formats had an extra `_`), and a NULL `S_00` was released | Functional entry reading | fixed, not exercised | UI-09 |
| B-11 | 2026-11-25 | ui_chat_panel back-gadget callbacks 0x56DEE0 | Dragging a chat-panel window by its back never laid out the option-font sliders (the callbacks were typed DestroyGadgetEvent, the client's are MovableUIBackGadget(Moved)) | L1 audit (callback type descriptors) | fixed, not exercised | UI-08, with a new step: drag the chat panel by its back |
| B-12 | 2026-11-25 | StarmapDataClass::Create_Mesh 0x7BEF50 | One starmap texture leaked per mesh (Create_Peek where the client calls Create_Get 0x51A6F0) | Functional entry reading | fixed, not exercised | SP-05, with a new case for the leak |
| B-13 | 2026-11-25 | Unknown_466920Class::Render 0x466B20 | The nav map's two scroll-extent lines drew under the UI with wrong blend, cull and z states (IsSortingEnabled tested where the client reads AreStaticSortListsEnabled 0xBFAE50) | Draw-log compare against retail | fixed, e2e starmap shows retail's 193 draws in retail's order | e2e `starmap` |
| B-14 | 2026-11-25 | Random Appearance 0x557290 | The head and body blend pulls were randomised too, four extra RNG draws (`0.0f` passed as `bool random`; the client pushes 0) | L1 audit (parameter slots) | fixed, not exercised | UI-01 |
| B-15 | 2026-11-25 | Talk tree 0x64BCA0 | frame01 freed early (Create_Get where the client's 0x404C8C is Create_Peek, which takes a reference) | Functional entry reading | fixed, not exercised | sb_ talk (as B-09) |
| B-16 | 2026-11-25 | ui_mount 0x5C77A0 | Two undefined externs are Matrix3D::Identity 0xBF9330 and RotateZ90 0xBF92D0 (the matrix initialisers' order) | Functional entry reading | fixed, not exercised | none yet: a new case, the mount window in each view |
| B-17 | 2026-11-25 | /resetship confirmation 0x74ABC0 | Confirmation text differed from the client's string 0xB9A12C | Functional entry reading | fixed, not exercised | none yet: a new case |
| B-18 | 2026-11-25 | Short strings: chat font label 0x56C7E0, ChatFontSys 0x67E670, trade texts 0x68FE10 / 0x6902E0, ephemeral-line ini defaults 0x5A4B50 / 0x5A5DA0, starbase light labels 0x6474C0, a nameless skill 0x817070, "Dest: ?" 0x7C91A0 | `""` where the client passes `" "` (0xB6E948) or `"?"` | L1 audit (literal scan) | fixed, not exercised | UI-08, UI-03, UI-07, SP-05; none yet for the ephemeral-line defaults (a new case) |
| B-19 | 2026-11-25 | Constants as packed bytes: Get_Vertex_Color 0x470EF0 and its 27 copies in 0x470F80 (1/255 as 0x3B808080), 0x473780 / 0x473BC0 (0.7071068f), float-widened tolerances in fp_tt 0x449540 / 0x449EA0, 0x447E20, ui_mount, 0x8494C0, the exact double in ship_pip 0x80CE10 | Land vertex colours and some tolerances differed in the last bits; the ship_pip double is invisible | L1 audit (relocation targets and constants) | fixed, not exercised | none yet: a new case, land vertex colours |

## 6. Porting the harness itself

The proxy is D3D8 and Win32 only. The port needs the same observations from inside the client.

* **Test hook layer** in the port, switched on by an environment variable. It provides frame readback from the SDL_GPU swapchain, the draw and camera log in the same JSON as C3, an audio event log in the same format as C2, the file-open log, and an input queue fed into SDL events.
* **The d3d8shot and MSS32 proxies stay** for capturing retail baselines on Windows. The baselines they write are format-neutral, so the port compares against them directly.
* **tests/ programs** (udpreplay, auxreplay, kerneltest, clienttest) get CMake targets. Their expected outputs are portable text and are reused as they are.
* **antprox and the local Net-7 server** are Windows only (antprox is win-x64). For hosted CI, the harness gets a small cross-platform replay server that plays recordings to the client. The live sb_* and sp_* sets need a real server, so they stay on your Windows machine until Net-7 runs in a container on Linux.
* **Win32 specifics in the Python harness** (window-message posting, process window lookup, netstat, junctions) go behind a small platform module. The machine's paths are already out of the code by then (C1's config file).

## 7. This week's order and status

The unattended week of 2026-11-24 (`docs/week-plan-2026-11-24.md`, kind C). Sessions change the status column in `docs/test-plan.md`; a difference found between retail and ours becomes a behaviour item at once.

| Order | Item | Session | Status |
|---|---|---|---|
| 1 | C1, one scenario end to end: the machine config file, the manifest, the store's layout, `sb_talk` captured three times on retail, pushed to `Open-EnB/baselines`, verified from a fresh clone | interactive, 2026-11-24 | done 2026-11-24: three PASS runs through the Rust antprox 1.2.0 (store `65f2199c`), noise at most 14 of 256 cells a shot, a fresh clone verifies and a fourth retail run is SAME; EnB's changes on `tools/2026-11-24`, not pushed |
| 2 | The run inputs into the store: the replay captures, the record tracks, the test database snapshot | interactive, 2026-11-24 | done 2026-11-24: store `058bbce` `inputs/` (replay set 38 MB in LFS, five record tracks, account 1 and avatars 6-8 as restore scripts over the stock Net-7 dumps), `baseline.py inputs`, `/RECORDINGS:` from machine.py; the proxy pair (antprox-rs 1.2.0.0, antreplay 1.1.0) in the store's `tools/antprox` (`519cfa3`); replay through it does not work yet: antprox-rs has no original profile (P1) and never starts antreplay |
| 3 | C3 draw and camera log per shot | 1 | done 2026-11-25: `D3D8SHOT_DRAWLOG` (e2e sets it) writes `shot_NAME_<frame>.json` per shot (draws, textures by parameters and content hash, view / projection / world); `noise.json` and `verify --run` compare them structurally, per-draw matrix tolerance; proven on replay (live server down): world_entry, play1 and the default set as one session a run (`--set default`, 3 x 300 s) into the store (`7d3406d`); ours: world_entry SAME, the default set 13 of 25 SAME; 2026-11-25: every stored run (default, station, space sets and the two scenarios alone) carries a draw log per shot (store `2efe4b9`); retail's own draw logs are identical in 1 of 55 default shots, 5 of 33 station, 0 of 24 space (draw counts and matrices vary) |
| 4 | sb_buysell, sb_dismantle and sb_emote_dock passing on retail again (B-08) | 1 | done 2026-11-24: each from the restored database state in a client of its own, 3 / 3 PASS each into the store |
| 5 | C6 modifier keys and chords | 1 | open |
| 6 | Recording 10 on retail and on ours with pose capture at the gate, the landing and the take-off (SP-01, SP-02 first pass; B-01, B-02) | 1 | open |
| 7 | C2 the MSS32 logging proxy | 2 | done 2026-11-25: `build/mss32log/mss32.dll` (344 of 344 exports) in every capture (`--audio`), cut per scenario into `audio-NAME.json`; retail's streams / 3D sequence identical run to run in 22 of 25 default scenarios, 0 of 9 station (random footstep and greeting samples), 1 of 4 space; 2D counts equal in 16 of 25, 8 of 9, 4 of 4 |
| 8 | M1's captures: the e2e default set, sb_* and sp_* three times each on retail into the store, retail's own noise measured | 2 | advanced 2026-11-25 (store `2efe4b9`, a fresh clone verifies): default set 3 x 284-308 s (shots at most 254 / 256 cells, play1's flight), station set of 9 3 x 751-793 s (at most 94 / 256) with sb_emote_dock and sb_manufacture_make alone (SESSION_OWN), space set of 4 3 x 227-229 s with a restart each (at most 218 / 256); packets equal in every scenario; sp_combat is not stored because it fails on retail: twice in the set at step 32 (`sent OBJECT_EFFECT` not seen within 15 s after Rally), alone at step 31 (no SKILL_ABILITY sent) and at step 21 (no CREATE after /createmob). It has to be fixed on retail before its baseline is captured |
| 9 | SP-01 to SP-03 live; UI-03; UI-04 | 3 | open |
| 10 | UI-01 and UI-02 with the database snapshot; AU-01 over the existing sets (B-04) | 3 | open |
| 11 | SP-05 (B-03), UI-05 (B-06), the manufacturing shot (B-05); B-07's scenarios named | 5 | open |
| 12 | RD-01, RD-02, AU-02, MV-01, WB-01 to WB-03, LY-01, LY-02 | 5, 6 | open |
| 13 | **Supporting accounts** for the multi-client cases: extra accounts and characters generated from the canonical account scripts in the store (`inputs/db`), with an instance-to-account mapping in the harness | built when the first such case is written | open: needed only for player groups, in-space group formation flying and the proxy's authoritative-positioning test; the baseline is always captured from the admin account's client, and the supporting clients need no shots or draw logs; all accounts are restored once before the case starts, because an in-place restore logs every player out; only as many accounts as the case needs; baselines are not duplicated per account (the user, 2026-11-25) |
| 14 | IMG-02 at every handoff; IMG-03's questions (the week plan's kind D) | every | 1.38 % (2026-11-24) |

Open items from this week's captures:

* The space runs' `messages.log` is 49 to 58 MB a run in plain git, which draws GitHub's large-file warning on push. Move it to LFS or cap its size.
* `baseline.py verify --run` took only a run's `-session` subfolder, and compared what it found there without naming
  the rest (session 0's live station run of ours: 5 of 9; the other 4 ran in the fresh session e2e starts after a
  failure). Fixed 2026-11-25: `--run` takes the e2e `--out` folder (its newest run's session folders; an earlier run's
  are named and left out), a scenario run in a fresh session says so, and every scenario not compared is named with
  its reason (a stored set's the run lacks: `SKIP` under `--set`, exit 1). Session 0's run read that way: 9 of 9
  compared.
* Ours-versus-retail has only been run on a stale exe of 2026-11-21 (the default set 18 SAME and 7 DIFF, the station set 0 of 9 SAME). Those results are not findings until the build at HEAD is compared.

## 8. Three baselines over time

| Stage | Baseline | Where tests run |
|---|---|---|
| Now: reconstruction | Retail `client.exe` on real D3D8 | Capture on a Windows machine with the retail install; comparison against the store on any device with a clone of it |
| Port, phases 1–5 | Retail, for screenshots. The logs that need no GPU (draws, cameras, audio events, packets) are compared exactly | Windows machine for retail comparison; hosted runners for everything else |
| After acceptance | The modernized client, re-captured on lavapipe (software Vulkan), with the retail-derived logs kept | GitHub-hosted runners only |

Every test records which baseline it compares against, so a test can move to the new baseline on its own once it passes against retail on the modernized client.

## 9. Milestones and the freeze

**M1**

### Capture foundation

C1 baseline store (with the machine config file and the run inputs in the store), C3 draw and camera logs, C2 the MSS32 proxy, C6 modifier keys. Capture the existing 25 e2e scenarios, plus sb_* and sp_*, 3 times each, and record retail's noise.

**M2**

### P0 coverage

UI-01 to UI-04, SP-01 to SP-03, AU-01 and AU-02, MV-01, WB-01 to WB-03, RD-01 and RD-02, LY-01 and LY-02. Run each against retail and our build. File every difference as a reconstruction item: these feed the coordinator's field-offset and functional queue.

**M3**

### P1 coverage and CI

The remaining P1 tests. CMake targets for `tests/`. The GitHub Actions jobs that need no game data. Retail comparison stays on your Windows machine for now.

**M4**

### Hook layer

The in-process hook layer, proven on the Windows D3D8 build to produce the same logs as the proxies. Then the port can start.

### When the rebuild is frozen

The modernization fork starts from a commit that meets all of these. They are a record of what was proven, not a demand for a 100 % image.

* Every field-offset and functional entry has been checked for behaviour (`behaviour.tsv` has no open row), and what stays NOT EXACT is codegen only or has a case that reaches it.
* Every P0 case passes on our build against retail's stored baseline, inside retail's own measured noise.
* The ledger has no P0 bug that is fixed but not exercised.
* Every baseline was captured at least three times and is in the store with its manifest.
* M4: the in-process hook layer gives the same logs as the proxies on the Windows D3D8 build.
* The freeze commit records its byte figures: exact pairs, `imagediff.py`'s summary and its list of differing ranges.

## 10. Decisions and open questions

### Decided (the user, 2026-11-24)

* **The baseline store is the private repository `Open-EnB/baselines`**, cloned beside the tree (`C:\EnBDevelopment\baselines` on this machine; the path comes from the machine config file). It holds observations derived from running the client (PNG frames, JSON logs, text, manifests) and the run inputs: the replay captures, the record tracks and the test database snapshots. It never holds the game's files. It is not encrypted: the repository is private and stays private. PNG frames and the captures go through Git LFS. All three runs' frames are kept.
* **Tools and scenarios stay in `Open-EnB/EnB`**, which is public, so nothing derived from running the game is committed there.
* **The local Net-7 database may be snapshotted and restored** around UI-01 and UI-02, with a test account.
* **Baselines are not duplicated per account (2026-11-25).** The multi-client cases (item 13 of section 7) capture their baseline from the admin account's client only; the supporting accounts have no baselines of their own.
* **Recording 10 holds a stargate jump, a planet landing and a take-off.** No scripted in-game movie shipped in the live game, so SP-09 is a unit check only.

### Open questions for you

1. Whether Git LFS's quota on the organisation's plan is enough for PNG frames (about 1 MB a shot, three runs of every scenario); the first full capture will give the real figure.
2. At M3, hosted runners would need read access to that repository (a deploy key). Acceptable, or do retail comparisons stay on your machines for good?
3. How much of the link's reproduction (IMG-03) belongs before the freeze: the image builder's findings of this week come with a proposal.
