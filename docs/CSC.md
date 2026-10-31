# Character and Starship Creator: what it tells us, and could the tree rebuild it (2026-10-30)

EA's promotional "Character and Starship Creator" (CSC) let players make up to five characters and
their starships before buying the game; the retail client imports them. Target:
`Character and Starship Creator\Character and Starship Creator.exe` (4,485,120 bytes, SHA-256
`6320397b24ec…f7ae`, PE time 2002-09-13 08:14:05 UTC, version 1.0.4.0, internal name "Avatar
Creator", original file name `Avatar Creator.exe`, Westwood Studios (C) 2002). Its data
(`Data\Mixfiles\*.mix`, `Data\Client\Nlp`, `Data\Client\Personalities`) and the Miles runtime
(`Mss32.dll`, `Mp3dec.asi`, `Mssfast.m3d`) are in the same folder.

Sources: a copy of the exe in `.work/csc/` (the install folder only read, D143); Ghidra 12.1.4
headless (project `%LOCALAPPDATA%\EnB\ghidra_csc`); the E&BConfig/VIDTEST assessment scripts
(`.work/ebconfig/`). Scratch and scripts are in `.work/csc/`:

* `peinfo.log`, `str_csc.txt` / `str_client.txt` (all strings), `csc_only_str.txt` (820 strings
  CSC has and the client lacks), `csc_paths.txt` / `client_paths.txt`, `pathcmp.py`.
* `libmatch.py` -> `libmatch.json`: every function of the tree's build objects (and SP4
  LIBCMT/LIBCPMT/COMSUPP/d3dx8) pattern-matched in client and CSC. `summ.py`, `cscexact.py`.
* `ghidra/run.py`, `CscFuncs.java` -> `ghidra/funcs.tsv` (21,370 functions); `names.py` +
  `VidDecompile.java` -> `pseudo/` (the tool's 94 functions), `pseudo_creator/`.
* `cscmap.py` -> `cscmap.json`: client app functions paired 1:1 with CSC functions, and CSC's
  `__FILE__` attribution; `provisional.py`, `runs.txt` (CSC's object order), `layout.py`, `need.py`,
  `treecmp.py`, `mixlist.py` -> `mix_csc.tsv` / `mix_client.tsv`, `mixcmp.py` (contents).

Metric caveat as in the VIDTEST assessment: "exact" is objmatch semantics (relocations wildcarded,
found once in .text), looser than the project's verified pair counts; use the numbers
comparatively.

Licence note: CSC's EULA ("EA Tools End User License") forbids reverse engineering, like the
game's own licence. This analysis reads the program for interoperability with our reconstruction;
nothing from it is redistributed.

## 0. Answers

* **What is it?** E&B's own client front end, built from the client's source tree as it stood on
  launch day (2002-09-13), plus a small tool layer (`G\Tools\Avatar Creator`, 7 files, 94
  functions, 29 KB). About 2.4 MB of its 3.8 MB of code is client app code (§2).
* **What does it tell us about the client?** Not much that is new about the 2004 client's code,
  but several useful facts (§4). It is a third dated 2002 build, after VIDTEST (09-03) and
  E&BConfig (09-04), and the only one that contains the client's app layer. It confirms 358 of the
  client's source files and adds four: `app\client\chatint.cpp`, `textwin.cpp`, `usermsg.cpp`,
  `libs\wwonline\wnet\tcpmgr.cpp`. It shows the client's own build was service-packed while the
  developers' build was not (CSC is VC6 RTM). 98 tree functions are byte-exact against CSC's
  2002 code but not against the client; one is `DynObj::DynObj()`, marked NOT EXACT. It is the
  writer of the `Avatar<N>.dat` files the client's `CharacterCreatorClass` reads. It names no
  provisional (`unknown_*`) file.
* **Could we rebuild it from our tree?** Byte-exact: no, and not realistically (§6.1). It needs
  the VC6 RTM compiler and CRT, the September 2002 version of the seven libraries and of ~343
  client app files (the tree has 76 of those files, at their 2004 versions), and the tool's own
  code. Functionally, as D36 did for VIDTEST: possible in principle, but it needs most of the
  client's front end (UI gadgets, avatar rendering, the ship and avatar customisation screens),
  which the tree hasn't reconstructed. So it is a milestone after the client's login and
  character-creation screens work, not before (§6.2).

## 1. The program

| | CSC | client.exe (ref) | VIDTEST.EXE (ref) |
|---|---|---|---|
| Size | 4,485,120 | 8,642,625 | 2,093,056 |
| PE timestamp | 2002-09-13 08:14:05 UTC | 2004-03-17 | 2002-09-03 21:22:12 UTC |
| Version info | "Earth & Beyond Character and Starship Creator" 1.0.4.0, Westwood Studios (C) 2002, internal name "Avatar Creator" | | "Video Mode Tester" 1.0 |
| Built on | `C:\Projects\G\...` (`__FILE__`) | `e:\build\beta\G\...` | |
| C++ compiler (Rich id 11) | **8168 (VC6 RTM) x920**, nothing else | 8799 (SP4) x1054, 8797 x13, 8798 x3, 8168 x17 | 8799 x203, 8797 x13, 8798 x3, 8168 x1 |
| C / CRT (id 10) | **8168 x185** (RTM CRT) | 8797 x201, 8168 x7, 8799 x1 | 8797 x145 |
| Import libraries (id 4) | 8168 x83 = mss32 (80) + 3 descriptors, as E&BConfig | 8447 x96 = mss32 (81) + binkw32 (9) + 6 | none |
| Other Rich | D3DX 8830 (id 28 x1, 29 x66); MASM 7291 x8, 7299 x43; id 19 8022 x14, 8034 x19; cvtres **1720** | D3DX 8830; MASM 7291 x11, 7299 x45; id 19 8022 x14, 8034 x21; cvtres 1735 | |
| Link | LINK 6.0, **not incremental**: no thunk table, functions padded to 16 with `nop` (0x90), no .reloc | /INCREMENTAL, ~20,000 thunks | /INCREMENTAL, 460 thunks |
| Sections | .text 0x3A5AF0, .rdata 0x545E2, .data 0xB92F0 (raw 0x48000), .rsrc 0x2A80 | | |
| Imports | KERNEL32 (120), USER32 (43), GDI32 (11), ADVAPI32 (7), SHELL32 (`ShellExecuteExA`, `SHGetSpecialFolderPathA`), urlmon (3), ole32 (7), OLEAUT32 (8), AVIFIL32 (1), WINMM (2), mss32 (80), WS2_32 (26). No binkw32, WININET, authlogin, DDRAW | + binkw32, WININET, authlogin, DDRAW... | |
| D3D | D3D8 through WW3D2's `LoadLibrary` (none imported) | same | same |
| Resources | 6 icons (group 101), 1 string table, version. No dialogs | | |
| RTTI | 26 type names (client 87). The only one the client lacks: `WideStringClass` | | |

The compiler is the striking difference. Every object in CSC, its CRT included, is VC6 without a
service pack. The client is SP4 (toolchain.md §1.2). CSC also lacks the three unexplained C++ 8798
objects that the client, VIDTEST and E&BConfig all carry (toolchain.md §1.2). So they aren't part
of every E&B link: they came with something CSC didn't link, or were only in the build server's
(SP4) environment.

### 1.1 The two link methods

| | client.exe (`/INCREMENTAL`) | CSC (`/INCREMENTAL:NO`) |
|---|---|---|
| Calls | to external functions through ~20,000 `jmp` thunks; to `static` functions direct | all direct |
| Identical functions | kept apart: app bodies that are only `ret` 110, `xor eax,eax; ret` 148, `ret 4` 67, `mov al,1; ret` 33, `xor al,al; ret` 23 | folded: 2, 1, 1, 0, 0 in the whole program |
| Unreferenced functions | kept (dormant code stays, e.g. the creator writer in §4) | dropped |
| Padding | `int3` after every function, plus 382 KB | `nop` to 16 bytes: 7,485 of 7,491 app functions start on a 16-byte boundary |

An incremental link implies `/OPT:NOREF` and no folding. The counts (`.work/csc/icf.py`) show CSC was
linked with the release defaults `/OPT:REF` and `/OPT:ICF`. What follows:

* **Matching is unaffected.** The link doesn't change a function's code, and objmatch wildcards call
  targets; hence the 2,143 masked 1:1 pairs (§4) and the matches of §3.
* **CSC is the weaker naming reference.** Under folding one CSC address can stand for several
  source functions (different classes' empty virtuals, identical template instances); this is most of
  §3's "CSC only". Names come from the client.
* **Only the client shows `static`.** workflow.md §3's rule (a function called directly, not through
  a `j_` thunk, is `static`) has no CSC equivalent.
* **Neither layout marks file boundaries:** both pad every function.
* **CSC witnesses 2002 liveness.** Everything in it was referenced in that build, so the 98
  functions of §3 that are exact in CSC and absent from the client are real 2002 code, not leftovers.
  One way only: a function missing from CSC may just be unused by CSC's program.
* **A CSC rebuild** links without thunks or padding to reproduce, but needs `/OPT:REF,ICF`, and its
  layout then depends on exactly which bodies are identical.

## 2. What is inside

### 2.1 Layout (.text)

| Region | Functions | What |
|---|---|---|
| 0x401000-0x4081C0 | 94 (29 KB) | The tool: `G\Tools\Avatar Creator\` `AvatarCreator.cpp`, `AvatarView.cpp`, `CreatorTask.cpp`, `UI_EditAvatarSelection.cpp`, `UI_Introduction.cpp`, `UI_Poster.cpp`, `WinMain.cpp` (in that order) |
| 0x4081C0-~0x4C0000 | ~2,100 | WW3D2, WWMath, WWLib, WWDebug, WWSaveLoad |
| ~0x4C0000-~0x720000 | ~7,500 (~2.4 MB) | Client app code (G\app), 343 `.cpp` files by `__FILE__`, starting with `kernel.cpp` |
| ~0x720000-0x740000 | ~500 | WWAudio, WWShade, more WW3D2/WWSaveLoad |
| 0x740000-0x760000 | ~700 | LIBCMT (RTM), WWMath |
| 0x760000-0x780000 | ~1,200 | D3DX8 |
| 0x780000-0x7A6AF0 | 9,405 (100 KB) | `Unwind@` funclets |

The app code's order is library-pull order, not the client's object order. The tool's objects
come first, then each library member as the linker resolved a reference to it. So the app layer
was linked from a library (an `app.lib`-like build of G\app), not object by object as in the
client. For example, `CharacterCreatorClass` (CSC 0x4DD9E0) follows `tsk_login.obj`, its first
user. In the client it sits between `avatar.cpp` and `nlp.cpp`. A file name can therefore be
inferred from CSC neighbours only within one object, and COMDATs (inline and template code) sit
with whichever object pulled them first.

### 2.2 Source files (`__FILE__` strings)

369 `C:\Projects\G\` paths (client: 469 `e:\build\beta\G\` paths); `pathcmp.log`:

* **358 shared** with the client.
* **11 CSC only:** the 7 tool files, plus `app\client\chatint.cpp`, `app\client\textwin.cpp`,
  `app\client\usermsg.cpp` and `libs\wwonline\wnet\tcpmgr.cpp`. The first three hold assert
  texts the client lacks (`!ChatGadget`, `!user_msg`, `TextLine && TextLine->Peek_Font32Ptr()`):
  the 2004 client either dropped them or compiled them without asserts. The second `wnet` file
  completes `G\libs\wwonline\wnet` beside `tcpqueue.cpp` (source-tree.md §1).
* **111 client only:** files CSC didn't link (`app\main\main_client.cpp`, `app\display\display.cpp`,
  `app\client\html\*`, the movie tasks) or that came later (`app\shared\expr\*`,
  `chat_window*.cpp`, `buff_database.cpp`, `attrib_dir*.cpp`, `ini_manager.cpp`, `money.cpp`,
  `timer_manager.cpp`, `wwudp\bandwidthaccounting.cpp`...). CSC alone can't tell which.
* One relative include differs: CSC has `..\Main\../packets/pk.h` where the client has
  `.\../packets/pk.h`, so in 2002 a file in `G\app\Main` included `pk.h`.

`G\Tools\` is a new top-level directory of the original tree, beside `G\app` and `G\libs`.

### 2.3 Data

The 8 mix files (Renegade `MIX1`; `mixlist.py`) hold 1,946 files. **1,889 are in the retail
install too (by name), and 1,840 of those are byte-identical** (`mixcmp.py`: 1,461 in retail mix
files, 379 loose). The 49 that differ were changed by later patches: avatar meshes and skeletons
(`abanim09-*`, `amtbody*`, `anpc*`, `avcollision.w3d`), decals, `font_small9.tga`, and the INIs
`avatardef`, `decal_textures`, `secret_decal_textures`, `fa_fe_class00`, `fontsys`, `sounds`,
`w3danimsound`, `dialogbox`, `fc_default`, `fc_dialogbox`. The remaining 57 are the creator's own
front end: `fe_front*`, `fe_end*`, `fe_endscreen*`, the posters
(`fe_poster0?.w3d`), `fe_narrator01.w3d`, `fe01b.w3d`, `fe12b*.w3d`, the race portraits
(`fe_terran*`, `fe_jenqui*`, `fe_progen*`), `fa_fe01b/12b/14a.ini`, `fe_front.ini`, `fe_end.ini`,
seven `1492_00_85?se.mp3` voice files, `co0101.dds`, `csr0000.dds`, `fe_c07.dds`, `if_main02.dds`.
The search path (`.\;.\Data\Client\;...\Art\;...\Avatar\;...\Ini\;...\Sound\;.\Data\Common\;
.\Data\Assets\Avatar Creator\{Art,Ini,Sound}\`) is the client's layout plus one assets folder.

## 3. Library and app content (pattern-exact functions of the tree's build objects)

| lib | tree fns | testable | exact in client | exact in CSC | both | CSC only | client only |
|---|---|---|---|---|---|---|---|
| ww3d2 | 6,917 | 4,717 | 3,083 | 2,986 | 2,227 | 759 | 856 |
| wwaudio | 1,246 | 762 | 482 | 496 | 353 | 143 | 129 |
| wwmath | 845 | 724 | 435 | 431 | 300 | 131 | 135 |
| wwlib | 1,069 | 945 | 638 | 413 | 287 | 126 | 351 |
| wwshade | 762 | 374 | 189 | 212 | 157 | 55 | 32 |
| wwsaveload | 522 | 406 | 98 | 125 | 61 | 64 | 37 |
| wwdebug | 146 | 121 | 83 | 50 | 31 | 19 | 52 |
| app | 10,951 | 8,287 | 4,343 | 2,938 | 1,947 | 991 | 2,396 |
| LIBCMT (SP4 objects) | 1,465 | 1,256 | 710 | 511 | 493 | 18 | 217 |
| LIBCPMT | 950 | 732 | 256 | 17 | 10 | 7 | 246 |
| d3dx8 | 2,115 | 1,841 | 658 | 467 | 438 | 29 | 220 |

* Our SP4-compiled objects match CSC's RTM-compiled code almost as well as the client's. RTM and
  SP4 generate the same code for nearly everything (the 2002 VIDTEST/E&BConfig showed the same for
  SP5). The `@comp.id` stamp is what differs.
* "CSC only" is mostly COMDAT copies that occur more than once in the client (incremental link, no
  folding): 2,263 of 2,361. **98 are exact in CSC and absent from the client**: ww3d2 60, wwlib 26,
  wwmath 8, app 2, wwsaveload 2 (`cscexact.py`). Examples: `DynObj::DynObj()` (app, client
  0x44AC70, NOT EXACT in `app/shared/dynamic_obj.cpp`), `WW3D::Update_Movie_Capture`,
  `DynamicMeshClass::Change_Vertex_Color` / `Reset`, `RawFileClass::RawFileClass`,
  `Windows_Message_Handler`, `VectorClass<StringClass>::Resize` instances.
* App code paired directly, client function to CSC function (masked bytes equal, unique on both
  sides): 2,143 of the client's 18,690. The 2002-to-2004 changes and inlining differences keep the
  rest apart.

## 4. What it tells us about the client

1. **The client's source at launch.** CSC is the client's app layer as of 2002-09-13, with asserts
   and some debug code the 2004 build no longer has (820 strings): QA automation
   (`Analyze_Automation_Script.txt`, `Manufacture_Automation_Script.txt`, `QA_AnalyzeLog%s.txt`),
   switches (`-UITEST`, `-NET_DEBUG`, `-TRUEVISION-XFILE`, `-FONTDUMP`, `-LOG_ASSETS`,
   `-GALAXY_MAP_DEBUG`), developer slash commands (`/colmesh`, `/latency`, `/texfilter`,
   `/clearmem`, `/profile_log_begin|end`, `/time get|correct`, `/gc list`), "LoginTask: Entering
   Fast Start Mode", and the beta's web endpoints (`test.earthandbeyond.ea.com/embedded/...`,
   `ebweb.westwood.ea.com/staging/...`, `jump-subs.ea.com/...prodID=REG-EB-BETA`). It is version
   evidence of the same kind as Zero Hour's dated comments: where a tree function matches CSC
   but not the client, the source changed between September 2002 and March 2004.
2. **NOT EXACT leads.** The 98 functions in §3 are tree code that compiles byte-exact to the
   2002 build. For a function marked NOT EXACT (e.g. `DynObj::DynObj()`: "the client duplicates
   the epilogue into the allocation-failed path"), that says our source is the 2002 source and
   the client's differs by a later edit. It is not our codegen and not a compiler option, since
   SP4 reproduces RTM's code here. Such a function is a 2002-vs-2004 delta, worth recording
   instead of retrying forms.
3. **Source files:** four new names (§2.2), and assert-level confirmation of
   `libs\wwonline\wnet\tcpmgr.cpp`. The tree names it after Westwood's source (imports.md, getStatus
   0x8BDD40); CSC's code around its twins of 0x8BDD40/0x8BDE20/0x8BDF80 sits in that file.
4. **Attribution for app-functions.json:** of the 2,143 paired functions, 727 have a file in CSC.
   701 agree with the client's attribution and 4 disagree, all "between" evidence (COMDAT
   placement). 22 are new. Two of those come from an assert in the function itself:
   client 0x8250B0 is in `app\common\dbgconn.cpp` (the tree already puts `StatsConnectionClass`'s
   constructor there) and **client 0x836960 is in `app\common\galaxy_map.cpp`** (new). The other 20
   are "between" evidence only.
5. **Provisional files:** CSC settles none. In CSC the creator's object (§5.3) follows
   `tsk_login.obj` and is otherwise unattributed: no assert, as in the client. `unknown_navlist`,
   `unknown_cordmsg`, `unknown_rate` and `unknown_login_data` have twins in CSC, all between other
   files' runs with no assert of their own. Lead only: two functions of the `unknown_root.cpp`
   object (0x4A40B0 `Unknown_48`, 0x4A4590) are emitted inside CSC's `sclient.cpp` object. The
   client's `sclient.cpp` is far away (0x7429B0-0x7518A0), so in CSC they are probably COMDATs
   (inline code from a header) pulled with sclient.obj, which may also be their nature in the
   client.
6. **The character import, both sides.** The client's `CharacterCreatorClass`
   (`app/client/unknown_creator.cpp`) reads what CSC writes. CSC has the same object: constructor
   0x4DD9E0 (registry key `SOFTWARE\Westwood Studios\Earth and Beyond\Character and Starship
   Creator`, folder `<Common AppData>\Westwood Studios\Earth and Beyond\Character and Starship
   Creator\`), read 0x4DDE30, write 0x4DE100, check-and-delete 0x4DE2D0, `LoginNameDataClass::
   Set_Name` 0x4DE550. The client's writer (0x78D8A0) and `Set_Name` (0x78E0A0) have no caller in
   the client; CSC calls the writer from its `CreatorTask` (0x4025F0). This confirms what the tree
   reconstructed: five slots ("Character #1".."#5" are CSC's default names), and per slot a
   0x84-byte `LoginNameDataClass` (the task's array stride), `AvatarDescClass` and
   `ShipCustDescClass`, saved with the byte-sum checksum. Our `unknown_creator.cpp` could be
   checked against CSC's twins as a second reference.
7. **Toolchain** (§1): developer builds were VC6 RTM on `C:\Projects\G`; the shipped client came
   from a build machine (`e:\build\beta\G`) with SP4. The 8798 objects aren't part of every link.

## 5. What the program does (outline)

Names in *italics* are mine; addresses are CSC's.

### 5.1 WinMain (*WinMain* 0x407E70, `WinMain.cpp`)

`ArgvClass` init; registers the window class (icon 101, `WndProc` 0x408090: passes messages to the
client's input/window handler 0x4CECA0, posts quit on `WM_DESTROY`, swallows `SC_CLOSE`,
`SC_KEYMENU` and `SC_SCREENSAVE`); creates a `WS_CAPTION | WS_SYSMENU` window. A failed create
passes `GetLastError` with `__FILE__`/line 0x62 to *Fatal* 0x402F30. The program imports
`MessageBoxA`/`MessageBoxExW`, so under D69 it is never run unattended. Then *Run* 0x4015D0.

### 5.2 Run and initialisation (`AvatarCreator.cpp`)

* *Run* 0x4015D0: the client's file factories (the mix files in `.\Data\Mixfiles\` through
  `ComboFileFactoryClass` and the search path above); *Init* 0x401080; a `KernelClass` with a
  `CreatorTask` (0x1218 bytes, 0x4020C0) as its task; `KernelClass::Run`; *Shutdown* 0x401260.
* *Init* 0x401080: input and mouse; `WWMath::Init`; *Init_WW3D* 0x401410; `SoundLibraryClass::Init`;
  the client's font manager, font string files, tutorial manager and the other singletons; the
  kernel.
* *Init_WW3D* 0x401410: the asset manager (0x408C bytes; it sets bytes +0x4074, VIDTEST's
  `Set_WW3D_Load_On_Demand(true)` (vidtest.md O1), and +0x4075); `WW3D::Init`; N-patches level 4. It tries each device at **800x600**,
  depth 32 then 16, windowed if `-W` is given, and renders three empty frames.
* 0x401840 opens a document or web page with `ShellExecuteExA`. The links in the strings are the
  website and the retailers (`www.earthandbeyond.com`, EA store SKU 14354E, Amazon, GameStop,
  EB Games); I haven't traced which button opens which.

### 5.3 Screens

Each is the client's interface machinery (`InterfaceClass`, gadgets, `.ini` layouts) with a tool
class on top:

| Screen | Layout | Code |
|---|---|---|
| Welcome | `FA_FE01B.INI`, `FE01B.w3d`, narrator `FE_NARRATOR01`, "WelcomeAvatarCreator" | `UI_Introduction.cpp` |
| Slot selection (5 slots: location, level, profession, race; New / Next / Quit) and the import message "When you install the retail version of Earth & Beyond, your characters and starships will be automatically imported into the full game." | `FE12B` | `UI_EditAvatarSelection.cpp` |
| Race, profession, avatar and ship customisation | `fa_fe_class00`, `fa_fe_avatar00`, `fa_ship01-04` | the client's `ui_class.cpp`, `ui_avatr.cpp`, `ui_avsel.cpp`, `ui_shipcust.cpp`, `avatar.cpp`, `charview.cpp` (§2.1) |
| Poster: the character and ship with decals (`decal_textures.ini`, `decal_swatches.ini`, `secret_decal_textures.ini`); Edit label, Save (to the desktop: `SHGetSpecialFolderPathA(CSIDL_DESKTOPDIRECTORY)`), Email, Previous, Exit | `FA_FE14A.INI`, `FE_POSTER00/02-04` | `UI_Poster.cpp` |
| Front and end screens: website button, logos, "Exit in %d sec" | `FE_Front.ini`, `FE_End.ini` | (no assert; after `UI_Poster.cpp`) |

Saving a slot: *CreatorTask::Save* 0x4025F0 builds a `CharacterCreatorClass`, then a
`LoginCharacterClass`. It writes slot `+0xF68` through the writer 0x4DE100 (avatar, ship, name at
`+0xF80 + slot*0x84`) and copies the result into the task's slot tables (`+0x78 + slot*0x90`,
`+0x348 + slot*0xFC`).

The NLP lexicons and the ten `Personality0?.ini` are the client's (`nlp.cpp`: the avatar's
personality, `INITNLP`); CSC ships them because the avatar code loads them.

## 6. Could we rebuild CSC from our tree?

### 6.1 Byte-exact: no

What it would need, against what we have:

* **Compiler:** VC6 RTM (C2 8168) and its RTM LIBCMT for every object, and cvtres 1720. The pool
  VMs are SP4 (8799). An RTM VM would be a new base image (user decision, like E&BConfig's SP5).
  Codegen is nearly the same (§3), but every `@comp.id` and the Rich header differ.
* **Libraries:** WW3D2, WWMath, WWLib, WWShade, WWSaveLoad, WWDebug and WWAudio at their
  **September 2002** versions. This is the same second-variant problem as VIDTEST/E&BConfig
  (assessment §3): about 1,000 functions differ or aren't linked.
* **App code:** 343 client `.cpp` files at their 2002 versions. The tree has 76 of them, at their
  2004 versions; the client functions attributed to the other 267 number 2,987 (`need.py`; the
  largest are `galaxy_map_render.cpp` 151, `avatar.cpp` 134, `ui_registration.cpp` 88,
  `rs_track.cpp` 86, `ui_starbase.cpp` 79). Three app files exist only in CSC (§2.2), and
  `tcpmgr.cpp` would be E&B's version of Westwood's.
* **The tool:** 94 functions (29 KB), about 13 of them template or inline instances the tree
  already has. The rest is new code to reverse engineer.
* **Link:** easier than the client's: no incremental-link thunks or padding to reproduce. But the
  layout is library-pull order, which needs the original libraries' member order and the tool's
  object order. Also the 2002 Miles import library (LINK 8168, as E&BConfig), D3DX8 (have),
  and a tiny `.rc` (icons, one string table, version).

Verdict: further away than VIDTEST or E&BConfig (assessment §3). It depends on nearly the whole
client front end at a version we have no binary for except CSC itself.

### 6.2 Functional (D36's sense for VIDTEST): later, not now

The tool layer is small: WinMain, init, a task, three screens and a poster. A working CSC on the
tree's 2004 libraries would load the original `Data` folder (nearly all retail data, §2.3) and
write `Avatar<N>.dat` files the client reads (the format is already in `unknown_creator.cpp`).
What it can't do without is the client front end it runs on:

* have: kernel, `tsk_login` (partly), `uiview`, `view`, `avatar_desc`, `shipcust_desc`,
  `combo_ffactory`, `fontmanager`, `fontdatabase`, `soundlib`, `tutorial_manager`, `inpdsp`,
  `unknown_creator`;
* missing: `gadget.cpp`, `intrface.cpp`, `gadget_list_manager.cpp`, `textgadg.cpp`, `tooltip.cpp`,
  `interface_scene.cpp`, `ui_class.cpp`, `ui_avatr.cpp`, `ui_avsel.cpp`, `ui_shipcust.cpp`,
  `avatar.cpp`, `charview.cpp`, `ship_pip.cpp`, `decal.cpp`, `env_map_manager.cpp`, `fontsys.cpp`,
  `font32bit.cpp`, `nlp.cpp`, `input.cpp`, `smouse.cpp`, `mousetbl.cpp`, `wnd_file.cpp`...

Those are the same files the client's own character creation needs. A working CSC therefore
falls out of reconstructing the client's login and character-creation screens: the extra cost
is then the tool layer (~80 functions). Building it earlier would mean writing stand-ins for the
UI system, which the client would discard. Unlike VIDTEST (D36), it wouldn't prove anything the
client's own screens won't.

### 6.3 Proposals: all three accepted 2026-10-30 (D160-D162)

* **P-CSC-1 (D160):** keep CSC as a reference, not a target. Use it (a) as the 2002
  witness for NOT EXACT functions (§4.2: list the 98 in not-exact.md's notes or the attempts log)
  and (b) as a second reference for code the client lacks or drops (the creator writer,
  `chatint`/`textwin`/`usermsg`, the developer commands).
* **P-CSC-2 (D161, applied):** record the new source facts in source-tree.md: `G\Tools\Avatar Creator`, the four
  file names, `wnet\tcpmgr.cpp` by `__FILE__`, the developer root `C:\Projects\G`. Also the
  attribution of client 0x836960 to `galaxy_map.cpp`. Record the RTM toolchain and the missing
  8798 objects in toolchain.md §1.2.
* **P-CSC-3 (D162, later):** a functional CSC as a milestone once the client's character-creation
  screens run on the tree (§6.2). Not before.
