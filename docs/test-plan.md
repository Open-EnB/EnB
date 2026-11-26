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
| UI-06 | P1 | PDA faction page (SAME against retail since 2026-11-25: 0x58E6F0 calls GadgetClass::Unknown_40 as the client) | pda_faction | Shot | R |
| UI-07 | P1 | Skills, slot data and slot info windows | ui_skills 0x62D730, ui_slot_data, ui_slot_info 0x630070 | Shots | R |
| UI-08 | P1 | Chat: tabs, channels, scroll-back, long lines, symbol chat | chatgadg (4), ui_chat_panel 0x5689D0, ui_symbolchat 0x64A9B0 | Packets, shots | R |
| UI-09 | P1 | The screens scenarios can't reach today: ui_pterm / ptmis / ptnew / ptppl, ui_manu_util, ui_avatar_dialog, ui_interrupt, ui_skill_info, ui_dynamicwindow, ui_scan, ui_shard | ui_scan 0x60D8D0, ui_shard 0x616220 and others | Shots, packets | R |
| UI-10 | P1 | Text rendering: every font, word wrap, clipping, proportional fonts, colour codes | fontsys Update_Text 0x6D64C0, Get_Obj_Space_Bounding_Box, font32bit | Shots of a text test window | R |
| UI-11 | P1 | In-game HTML: a page with GIFs (animated, transparent) and images | html/image/gifload Decode 0x76D990, Set_Data, HtmlImageClass::Draw | Shot; GIF decode hash (unit) | R/O |
| UI-12 | P2 | A subscription request with an empty first parameter (the client's `Add_Param` reads the string's refcount byte, B-23): the request's bytes equal retail's | unknown_7cef30 0x7D4E30 (SubsRequestClass) | Packets, byte for byte | R |
| UI-13 | P2 | The guild list's order with member names that differ in the second letter and with odd and even lengths (the client sorts with `wcscoll` on narrow strings, B-23): the list shown equals retail's | ui_guild 0x81AEB0 | Shots, the list's rows in order | R |

### Space, cameras and world

| ID | Pri | Scenario | Risky code reached | Oracle | Base |
|---|---|---|---|---|---|
| SP-01 | P0 | Stargate jump | camctrl_stargate 0x795C80 (field-offset), camctrl_entersector 0x793740 | Camera pose per frame (`D3D8SHOT_CAMERA`), packets. First pass from recording 10 (`/play 10` holds a gate jump), then live | R |
| SP-02 | P0 | Planet landing and take-off | camctrl_land 0x793CE0, camctrl_pland 0x794A90 (field-offset), camctrl_takeoff (shape, field-offset) | Camera pose, packets. First pass from recording 10 (a landing and a take-off), then live; the first exercise of bugs B-01 and B-02 | R |
| SP-03 | P0 | Dock and undock at a station, with pose checks | camctrl_dock 0x792750 | Camera pose, packets | R |
| SP-04 | P1 | Tractor beam on loot; rescue | camctrl_tractor 0x797570 / 0x797770, rescue | Camera pose, packets | R |
| SP-05 | P1 | Galaxy map and starmap: zoom, pan, select, plot a route | galaxy_map_render (5), starmap_pip 0x7C2060, nav_list_line_draw 0x7B9DC0 | Draw log, shots | R |
| SP-06 | P1 | Combat with debris, explosions, camera shake | debrsint (2), cameraeffects, frameint 0x53EB00 | Draw log, shots by brightness, pose | R |
| SP-07 | P1 | Planet view render mode (play1's 'darker sector 4505' was the shot's phase, not the client: at one point of the playback ours and retail draw the same, 2026-11-25) | mainview Render_Planet_Mode 0x50E6E0, 0x512400; enbscene Apply_Projectors (field-offset) | Draw log, shots | R |
| SP-08 | P1 | Ship decals and environment maps on several hulls | shiputils 0x80EAF0, 0x80EFF0, 0x812390; env_map_manager | Draw log (textures per mesh) | R |
| SP-09 | P2 | Scripted in-game movie. None shipped in the live game (the user), so no scenario can reach it: a load-and-step unit check of the path only | mclient (2), mv3dsnd (2) | Shots, audio log | R |
| SP-10 | P2 | Recordings 2–6 as scenarios | Broad | Packets, alive | R |
| SP-11 | P1 | Next Target after a kill (`sp_nexttarget`, `tools/harness/e2e/sp_nexttarget.scn`: sp_combat cut after the kill, two N presses): per run, the first press picks the hulk first iff its centre is within ~218 of our ship, else our own ship (the client's behaviour: 37 of 37 runs, retail and ours; the rates differ because the mob dies at another distance, a server-side matter) | TargetingClass 0x7C61F0 / 0x7C62B0, TargetFindClass 0x7C3FF0 / 0x7C4530 (Build_List), 0x7C4E30 (_Sort), 0x7E6130 | Packets (SET_TARGET) against the hulk's and our positions, a rule per run, not a rate | R |

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
| B-01 | 2026-11-24 | camctrl_pland 0x794A90 | Planet-landing camera on a wrong path: spline length 1250 for 5000, speeds 0 for 100, start and end tangents, the end key | L1 audit | fixed, not exercised | Not reached live (CS5 2026-11-26): state 7 comes only from CAMERA_CONTROL 1 (request 13), which Net-7 never sends; the Land verb runs state 5 (camctrl_land 0x793CE0, `sp_land`: the same as retail) and GM `/ccamera 1` beside the planet changed nothing. Needs a server that sends CAMERA_CONTROL 1 at a landing |
| B-02 | 2026-11-24 | camctrl_takeoff 0x796920, 0x796440 | Take-off camera swept the camera's near-clip box, not the ship's model box; two NULL checks the client does not make | L1 audit | fixed, not exercised | Not reached live (CS5 2026-11-26): state 9 comes only from the cockpit's warp button in view mode 1 (ui_cpit.cpp 0x583250, request 17); on Endriago's surface the button does nothing (clicks at 640,922, 640,912, 690,920 and Q: no camera change, no packet), and the Launchpad's CAMERA_CONTROL 4 runs the stargate camera (state 3; `sp_land`: the same as retail). Needs a planet where the warp button is live |
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
| B-20 | 2026-11-25 | FontSys::Update_Text 0x6D64C0 | Line height computed as ((float)U * Scale) * 480 / H where the client computes (Scale * 480) * U / H (0x6D664D); under D3D8's single-precision FPU a scaled font's line could round one pixel taller or shorter | Opus max round on a parked function; behaudit fpu check | fixed, not exercised | UI-10 and a new case: a font with Scale != 1, several lines, against retail |
| B-21 | 2026-11-25 | SClient::Unknown_749ED0 0x749ED0 | The channel name of /gen, /ooc, /mkt, /new, /jen, /ter, /pro, /exp, /tra, /war, /sen, /def, /enf and /rad went into the chat request's +0x14 string (the tell / reply recipient) where the client stores it in +0x24 (0x74A789) and leaves +0x14 empty: channel lines went out with an empty channel field | Fable round on a parked function, trybatch --slots (a frame symbol at a second client offset) | fixed, not exercised | UI-08, a new step: type `/gen hi` live and compare the ClientChatRequest bytes with retail's |
| B-22 | 2026-11-26 | ShipEquipPipClass 0x542A10 | The equipment pip's orbit camera, when the ship is dragged: the six Translate rows summed (m2z + m1y) + m0x where the client sums (m0x + m2z) + m1y: a last-bit rounding of the camera, not a visible jump | Fable round on a parked function (FB6, W3DView GraphicView.cpp's to_object rotation read as reference) | fixed in run 2026-11-25-evening (7afb8511d), not exercised | UI-05 with a drag of the equipment pip and a shot of it |
| B-23 | 2026-11-26 | Faithful client bugs kept: 0x7D4E30 `Add_Param` reads `*Query.rend()`, the VC6 string's refcount byte `_Ptr[-1]`; 0x81AEB0 sorts guild members with `wcscoll` on narrow strings | A subscription request with an empty first parameter: `Add_Param` reads the string's refcount byte there (our build reproduces it); the guild list is ordered two bytes at a time (and can read one past an odd-length name), so names differing in the second letter, or of odd and even lengths, are not in plain alphabetical order | Fable / Opus rounds (B10: 0x7D4E30; B13: 0x81AEB0) | kept: the client's own behaviour (coordinator calls c14, c26); a fork that fixes either does it on purpose | UI-12 (the request, byte for byte against retail's) and UI-13 (the guild list order) |

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
| 5 | C6 modifier keys and chords | 1 | done 2026-11-25: `key shift+i` (names, letters, VKs; chords with `+`), `keydown` / `keyup` held across steps, posted as a keyboard sends them (the modifier's own key-down first, Alt without Ctrl as WM_SYSKEY* with lParam bit 29); client.exe reads Shift / Ctrl / Alt only through GetKeyState / GetAsyncKeyState (0x4307A0, 0x430CA0, 0x4314C0; no DirectInput), which the proxy answers from the posted keys and logs (`key 0x49 +shift`); 17 unit tests (`tools/harness/test_keys.py`). `sb_vault` (live) opens the Vault with Shift+I, closes it with the held form, toggles Ctrl+I's ping read-out: PASS on retail and on ours (the run's HEAD client), twice each, the same packets, the vault and the ping drawn alike (expect-ink 435 / 436 and 260 / 260); the remaining shot cells (4-36 of 256, the station's background, in vault_before too) are within retail's own station noise |
| 6 | Recording 10 on retail and on ours with pose capture at the gate, the landing and the take-off (SP-01, SP-02 first pass; B-01, B-02) | 1 | done 2026-11-25 (R10): `e2e.py --camera-track` (the camera's views about every 2nd frame) on `_play10`, retail 3 + 5 runs, ours (HEAD a9fa08ee3) 1: every place the camera holds still is the same on both to 0.1 unit / 0.1 degree (the dock camera of CAMERA_CONTROL 5, the warp camera, the starbase cameras, each sector entry; the seeded generator 0xBDB1C0 makes them repeat exactly on retail), no pose difference. The recording exercises neither the stargate camera (CAMERA_CONTROL 4 comes 0.2 s before Net-7's handoff: no frame drawn) nor the landing (the user's Land verb; on the replay the verb icon sends nothing) nor the take-off (the warp button on Endriago's surface does nothing): SP-01's stargate and SP-02, with B-01 and B-02, stay for the live runs (item 9) |
| 7 | C2 the MSS32 logging proxy | 2 | done 2026-11-25: `build/mss32log/mss32.dll` (344 of 344 exports) in every capture (`--audio`), cut per scenario into `audio-NAME.json`; retail's streams / 3D sequence identical run to run in 22 of 25 default scenarios, 0 of 9 station (random footstep and greeting samples), 1 of 4 space; 2D counts equal in 16 of 25, 8 of 9, 4 of 4 (2026-11-26, CS2: a verdict RETAIL-UNSTABLE where retail's own runs disagree in the audio, slices cut at the next scenario's first input, audio compared by sound family with digits as #) |
| 8 | M1's captures: the e2e default set, sb_* and sp_* three times each on retail into the store, retail's own noise measured | 2 | done 2026-11-26 (CS, CS2, CS3): the default, station, space and sp_combat sets captured with `tick_input` into the store's working tree, uncommitted (the user commits; the replaced runs are under `.work/cs3/store-replaced/`): space 5 runs, sp_combat 5, station 5, sb_emote_dock 3, sb_manufacture_make 3, verify 27 runs / 2085 files matching. sp_combat passes on retail (hotbar slots 4 / 5 are Biocessation / Enrage Group, answered by a PRIORITY_MESSAGE; the click repeats until SKILL_ABILITY). Ours at HEAD: the default set 25 / 25 SAME (hc6, b2630a8bd, 311 s); station and space: every DIFF is retail noise or timing (random footsteps and greetings, the server's job board, animation phase, loot), 6 RETAIL-UNSTABLE, sp_galaxymap SAME (CS3, at 84c6a89e8) |
| 9 | SP-01 to SP-03 live; UI-03; UI-04 | 3 | UI-03 and UI-04 done 2026-11-25 (U4): `sb_rooms` (every room of Jove's Fury with a pose and a shot per spot, every terminal opened and closed, the name tags, the launch and the dock back, ~470 s) and `sb_vault_ops` (Shift+I, deposit, withdraw, five sorts spaced by the server's 10 s sort timer, the exit box; the cargo put back) pass 3 / 3 on retail and on ours at a9fa08ee3 with the SAME packets and the same server answers; the terminal shots, the vault and the cargo windows are identical (0 cells) on all four, the room shots inside retail's spread (up to 95 of 256 cells); no difference between retail and ours; runs under `.work/u4/e2e/set2`, not in the store. SP-01 to SP-03 done 2026-11-26 (CS5): `sp_gate` (warp to the Saturn gate, its verb, the stargate camera, Saturn), `sp_land` (`/wormhole 2210`, warp to Endriago, Land: the landing camera, state 5; the surface; the Launchpad: the stargate camera on a planet) and `sp_dock` (warp to Jove's Fury, Dock: the docking camera, state 6; the hangar walk; the launch) with `--camera-track`, retail 3 runs each in the store (`baseline.py capture --camera-track`), ours (CSC at cacc2b687) 7 / 2 / 2 runs on a restarted Net-7: the SAME packets (26 / 43 / 34), and `camtrack.py` finds the landing camera's held place, the ship's place on its spline (draw logs), the docking camera, each sector entry and the stargate camera's offset from the ship the same as retail's spread; no difference between retail and ours. The gate warp's end has two kinds on both clients (the warp camera 0x797B50 cuts 4.1-4.2 s or 3.0-3.6 s before WARP_INDEX: retail 5 / 3 of 8 runs, ours 3 / 6 of 9), and against retail's runs of its own kind ours' stargate camera (its offset from the ship) and Saturn's entry camera are the same; where the ship stops after the warp varies on both (retail 102286-102845, ours 101939-102830 in x). Not reachable on the local Net-7: state 7 (camctrl_pland, B-01) and state 9 (camctrl_takeoff, B-02) |
| 10 | UI-01 and UI-02 with the database snapshot; AU-01 over the existing sets (B-04) | 3 | open |
| 11 | SP-05 (B-03), UI-05 (B-06), the manufacturing shot (B-05); B-07's scenarios named | 5 | open |
| 12 | RD-01, RD-02, AU-02, MV-01, WB-01 to WB-03, LY-01, LY-02 | 5, 6 | open |
| 13 | **Supporting accounts** for the multi-client cases: extra accounts and characters generated from the canonical account scripts in the store (`inputs/db`), with an instance-to-account mapping in the harness | built when the first such case is written | open: needed only for player groups, in-space group formation flying and the proxy's authoritative-positioning test; the baseline is always captured from the admin account's client, and the supporting clients need no shots or draw logs; all accounts are restored once before the case starts, because an in-place restore logs every player out; only as many accounts as the case needs; baselines are not duplicated per account (the user, 2026-11-25) |
| 14 | IMG-02 at every handoff; IMG-03's questions (the week plan's kind D) | every | 1.37 % (2026-11-26: 118,585 / 8,642,625 bytes at b2630a8bd; 1.38 % on 2026-11-24) |

Open items from this week's captures:

* The space runs' `messages.log` is 49 to 58 MB a run in plain git, which draws GitHub's large-file warning on push. Capped 2026-11-25 (CS): `baseline.py capture` stores a log over 1 MB filtered (the frame-rate lines dropped, repeated asserts folded, cut at 4 MB; 48.8 MB -> 0.3 MB), the full log left in `.staging` and named in the manifest's `messages_log`. Open: the space set's three stored runs still hold the full logs (rewrite them with `filter_messages` before the store's next push, or leave them).
* `baseline.py verify --run` took only a run's `-session` subfolder, and compared what it found there without naming
  the rest (session 0's live station run of ours: 5 of 9; the other 4 ran in the fresh session e2e starts after a
  failure). Fixed 2026-11-25: `--run` takes the e2e `--out` folder (its newest run's session folders; an earlier run's
  are named and left out), a scenario run in a fresh session says so, and every scenario not compared is named with
  its reason (a stored set's the run lacks: `SKIP` under `--set`, exit 1). Session 0's run read that way: 9 of 9
  compared.
* sb_jobs_accept's failure on ours in session 0 ('sent JOB_DESCRIPTION' not seen at the Explore row) is the board,
  not the client: that JOB_LIST's payload (antprox's length 0x1EA less its 4-byte header: 486 bytes) is exactly the
  nine jobs the Combat and Trade shots show (Looter Sweep x2, Manifest Delivery x7: Net-7's GetJobList, 16 bytes,
  title, sponsor and '500 XP' a job), so the Explore tab was empty and its row click rightly sent nothing. The
  terminal's code in ours is byte-exact (ui_jobs.cpp's row click 0x5A2730, list 0x5A2C50 / 0x5A3BC0, tabs
  0x5A3440). At HEAD (a9fa08ee3) ours and retail both pass it live, one after the other (2026-11-25 07:30-07:37).
  Open: the scenario still fails on a board without an Explore job (RefreshJobs draws each of up to 9 slots from
  four job missions at random; one accepted job is gone until the next 5-minute refresh).
* **Recorded-session shots at one point of the playback, and a draw-log compare a moving scene passes** (2026-11-25,
  PH): antreplay plays a sector session at the recorded pace from the client's LOGIN whatever the client's load takes
  (6-25 s), so play1's `alive 20` after the load put its shot in the recording's flight or its rest. `e2e.py`'s
  `wait-since` waits for a time after a line; play1's sector shot is `play1_p52` (52 s into the playback, where the
  recording's second camera is at rest). Three retail runs and two of ours agree on every view at each second of
  33-44 s and at 30-60 s by fours (the main view to 1 unit, the second camera to 32); the brightness and draw counts
  of ours and retail match at every point (51-53 of 255, 543-726 draws). Between 38 and 44 s a planet at (-42794,
  -83151) switches from three passes to two and back on both clients (ours at 37-38 s in one run of three, retail
  at 38-39 s: within the shots' timing). The default set was captured again on retail three times for the new shot
  (store runs 20261125-172233, -172745, -173302; the older three stay for the other 24 scenarios and are left out
  for play1, whose text changed). `baseline.py verify --run` compares a draw log that fails in order again as a
  multiset (states, textures, counts per state, worlds within retail's own leave-one-out spread), and a scenario run
  after another than stored is NOT COMPARABLE (exit 3), not DIFF. Ours at HEAD (a9fa08ee3's client) on the default
  set: 23 of 25 SAME (19 on the earlier compare and store); left: play1's p52 shot 17 cells against retail's 9 (3 runs; the mean 53.3
  against 50.8-52.8, draw log SAME) and world_entry's turning home planet (draw 7, 420 against an allowed 210: the
  wall clock, `shot_mask.txt` masks it in shots).
* play1's `play1_entered` (wait-quiet proxy 2 after the session line) is the loading screen in most runs (13 draws,
  the 2D camera only): the load has a 2 s quiet in its middle. `_play10` waits for the loading screen's close
  instead (its fc_map_load00.ini is read again then). play1_windows' w_* shots are not at fixed playback points
  either (they read SAME now by the multiset compare).
* The Jupiter note (sp_flight's space_entered 49.8 on ours against retail's 55.9 / 57.5, 2026-11-21) is not a
  difference: from the store's database state (Net7 restarted, as the space set's capture), ours at HEAD (a9fa08ee3's
  client) draws space_entered at the same camera with the same 187 draws, mean 49.0 against the stored retail runs'
  49.2-49.3 and a retail run in the same hold 49.3; the draw log is SAME against the three stored runs (as a
  multiset: 12 world matrices within 0.64 unit, as many as a retail run has against the others), the shot 2-3 cells
  of 256, the packets of the whole scenario the same. The 2026-11-21 numbers came from another database state.
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
