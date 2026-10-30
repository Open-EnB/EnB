# Kernel-map step 9's dependencies: SClient, the login screens, kcom auth, the login's data objects

A read-only map made on 2026-10-27 (the run plan's Q6) of what `docs/kernel-map.md` §6 row 9
names as its needs besides steps 4-8: client.cpp and SClient, the login and character-select
screens (0xAF2C30, 0xAEFB60), kcom auth (`authFactory`, `iauth`), and the objects tsk_login.cpp
holds. Step 9 itself stands (2026-10-26, builders T3 and T4): tsk_game.cpp complete (17 marked
functions, none NOT EXACT), tsk_login.cpp 45 of its 52 attributed functions written, 9 of them
NOT EXACT (Run, Go_Next/Back_Step, Pre_Run, Unknown_50, the fast start, Start_Game,
Handle_Global_Error, Fetch_EA_Ticket; `attempts.py show`). Everything the two task files
declare without defining is listed by subsystem in §1; §2-§5 map the subsystems; §6 is a
reconstruction order like kernel-map.md §6, §7 a recommendation, §8 the traps found.

Evidence tags as in kernel-map.md: **S** a client string (assert, `__FILE__`, log text),
**C** what the code does (descriptive, not the client's name), **N** the maintainer's earlier,
unpublished protocol research ([prior-work.md](prior-work.md)); its findings are restated where used. Sizes: "attributed" counts and bytes are
`appfile.py show`'s; a **window** is the run of verified function starts from the first
function to the next file's first attributed function (`.work/map9/rsz.py`, padding included,
so a window can be up to ~30 % above appfile.py's bytes and can hold a neighbour's
unattributed functions). Addresses are the retail client.exe's (SHA-256 `249f8524…51c4`).
Tools used: `client.py dis|callers|refs`, `pseudocode.py --show`, `appfile.py show`, and four
scratch readers in `.work/map9/` (vt.py: vtable slots through the thunks; vrefs.py: which
functions store a vtable; deps.py: a window's callees grouped by file; fwin.py / rsz.py: windows).

## 1. What tsk_game.obj and tsk_login.obj import (declared, not in the tree)

From `build/app/symbols.txt` (the UNDEF externals no app object defines), grouped:

| Subsystem | Symbols (client addresses) | Section |
|---|---|---|
| SClient | `SClient(ClientTaskClass *)` 0x7405E0, `~SClient` 0x740BB0, the redirect connection 0x727F00, the send 0x728150 (both client.cpp), its virtuals +0x00 / +0x04 / +0x14 / +0x64 | §2 |
| the login front end | `CharacterViewClass(LoginTaskClass *, int)` 0x678450, 0x67A300, 0x67A370 (charview.cpp); the message box 0x588A70 / 0x5890B0 (ui_dynamicwindow.cpp's neighbourhood); the input dispatcher 0x429630 / 0x429800 / 0x429A20 (inpdsp.cpp) and the input queue 0x42E8A0 / 0x42EA80 / 0x42EDB0 (input.cpp); the software mouse 0x514FE0-0x515440 (LoginTask +0x18, §3.4); the gadget event 0x65F780 (gadget.cpp) | §3 |
| kcom auth | `AuthLoginLib_GetComponentFactory` (authlogin.dll import), `AuthLoginResponseClass(void *)` 0x792040, `Get_Error_Text` 0x7922B0 | §4 |
| the login's data objects | `TicketClass` 0x8C15F0 / 0x8C16D0 / 0x8C16F0; the character slot 0x8C06E0 / 0x8C0750; the avatar descriptor 0x4BEB50 / 0x4C1460; the ship customisation descriptor (0x160) 0x4F6210 / 0x4F6230 / 0x4F62F0 / 0x4F6330 / 0x4F6750, its assignment 0x55D8A0 and its 0x44-byte part 0x6191F0; the galaxy entry 0x83C730 / 0x83C7C0 / 0x83C850; the name 0x78E0E0 / 0x78E110 / 0x78E140; the creator's registry record 0x78CEE0 / 0x78D480 / 0x78DAF0 | §5 |
| singletons | the browser / URL manager 0x7CA2D0 (`Set_String` / `Set_Int` / `Set_Int64` / 0x7CB610 / 0x7CB6F0); the options object 0xBE8B38 (option.cpp 0x7F84F0, 0x7F8EF0); the font database 0xBE67B8 (fontdatabase.cpp 0x6C47B0); sound 0x43FA80 / 0x43FD70 (soundlib.cpp); the demo check 0x5FADE0 (ui_registration.cpp); 0x7228C0 / 0x723150 (a reference-counted global set before client.cpp); 0x7E5F70 (once per process); the version 0x4456C0; the UDP pending count 0x82E1D0 (udpconn.cpp, in the tree but not this function) | §5.6 |
| packets | sending constructors not in pk_classes.h: MasterJoin 53 (0x8974C0), Login 2 (0x89A740), GlobalConnect 109 (0x863400), GlobalCreateCharacter 114 (0x864090), GlobalDeleteCharacter, 119's Build / Print / size | the packet builder's (P4's merge of the task files' packet views into pk_classes.h) |
| E&B's debug output | 0x4BC000 / 0x4BC130 (the message reporter), 0x509E30 (dbgmgr.cpp), `Unknown_Log` | kernel-map.md §6 row 8's dbgmgr.cpp |

## 2. client.cpp and SClient

### 2.1 The client base class (app\client\client.cpp)

client.cpp (S: `__FILE__` in 10 asserts, lines 165-443) holds the **base class both clients
derive from**: SClient (sclient.cpp) and the movie script's client (mclient.cpp, `MovieClientClass`
in tsk_smovie.cpp, 0x1178 bytes). No RTTI or string names the base. **0x1150 bytes**
(SClient's second base starts at +0x1150; the movie client is 0x1150 + 0x28).
Constructor **0x726100** (0x7D0 bytes, the one function before client.cpp's first assert; callers
0x7405E0 SClient and 0x7EC3E0 the movie client), destructor **0x726C60** (0x309), vtable
**0xB02D7C**, 23 slots:

| Slot | Address | Size | What (C) |
|---|---|---|---|
| +0x00 .. +0x14 | 0x9CCD10 | 3 | `return 0` (the shared default): SClient fills all six (Init, Shutdown, ..., the frame) |
| +0x18, +0x1C, +0x20 | 0x7268E0, 0x726900, 0x726920 | 3 each | `return 0` |
| +0x24 | 0x728010 | 0x93 | sets the watched game id (+0x112C) and sends DebugPacketClass(id, 0x21, +0x11) (assert `Connection`, l.330) |
| +0x28 | 0x726940 | 0x87 | finds the object with id +0x112C in the first table (assert l.165) |
| +0x2C | 0x726A00 | 0x13 | |
| +0x30 | 0x726A30 | 0x87 | (assert l.165) |
| +0x34 | 0x7281F0 | 5 | `return 0` |
| +0x38 | 0x728210 | 0x13 | toggles +0x10 through +0x3C |
| +0x3C | 0x728240 | 0x86 | sets +0x10 and sends DebugPacketClass(+0x112C, 0x1E, flag) (assert `Connection`, l.402) |
| +0x40 .. +0x58 | 0x7274D0, 0x727600, 0x727780, 0x7278F0, 0x727AD0, 0x727B40, 0x727CA0 | 0xF0 .. 0x172 | add / find / remove in the object tables, keyed by game id modulo 257 (assert `ObjectList.Find(object->Get_Game_ID())`, l.222) |

Layout (constructor, C):

| Offset | Member |
|---|---|
| +0x00 | vtable 0xB02D7C |
| +0x08, +0x0C | 0 |
| +0x10, +0x11 | bools (+0x10: the debug flag of slots +0x38 / +0x3C) |
| +0x14 | the kernel (ClientTaskClass::Init sets it; sclient.h's `Kernel`) |
| +0x18 | a 0x10-byte object (constructor 0x763E00: between proj_decal_manager.cpp and tsk_game.cpp) |
| +0x1C, +0x444, +0x86C, +0xC94 | four object tables, 0x428 bytes each: a GenericList (WWLib) + 257 buckets + two counts; vtables 0xB02DF8 / 0xB02DF4 / 0xB02DF0 / 0xB02DEC (slots 0x729030, 0x729060, ...) |
| +0x10BC .. +0x1114 | floats (±2500, ±1e6, 10000, 1.2217 = 70° in radians, 1.0) and flags |
| **+0x1124** | **the connection**: at construction a peek connection (connect.h 0x821C30) over a do-nothing connection (vtable **0xB02D3C**: slot 0 0x726C10, three connect.cpp slots 0x8232C0-0x823300, the base's inline slots 0x726AF0-0x726BF0) |
| +0x112C | the game id the client watches (slots +0x24, +0x28) |
| +0x1138 | `rand()` after `srand` of a clock (the import at 0xC44340) |
| +0x1140 | byte |

The connection setters: **0x727F00** (redirect: `Create_Connection(ip, port)` 0x821400 -> the
statistics wrapper 0x824A90 -> a peek connection; called by ClientTaskClass 0x766AB0) and
**0x727E10** (the same through `Create_Connection("SectorServer", 1)` 0x821000): **0x727E10 has no
caller and no reference** (§8). 0x728150 sends a packet on +0x1124 (slot +0x08). Other client.cpp
functions: 0x727130 (0x224, strings, co.cpp 0x6FF4B0), 0x7281D0 ('Debug_Dump_Texture_Data - not
yet implemented'), 0x728330-0x7283F0 (four `FAIL_IF(!ship)`, l.419-443), 0x728180.

Window: **0x726100-0x728430, 48 functions, 9.0 KB** (appfile.py: 43 attributed, 4,844 bytes),
plus **0x728430-0x729BF0, 29 unattributed functions, 6.1 KB** before client_lego_impl.cpp: the
object tables' helpers (0x728450-0x7290F0, called by the destructor). Callees outside the tree:
px.cpp 0x44BE90 / 0x44BF00, px_process.cpp 0x455A90 / 0x455AB0, spline.cpp 0x4845F0 / 0x484810 /
0x484AB0 (the mouse table's `splines.wsd`, vidtest.md), co.cpp, eff_o.cpp 0x6E6ED0 / 0x6E74B0,
control_keys_manager.cpp 0x71BCC0, client_lego_impl.cpp 0x729E00 / 0x72E530, 0x831340 / 0x8316D0
(after circbuff.cpp), 0x763E00 / 0x763E30; in the tree: connect.cpp / connect.h / dbgconn.cpp,
pkdebug.cpp (DebugPacketClass 0x8802C0), permstr.cpp, new / delete. 21 app files in all.

### 2.2 SClient (app\client\sclient.cpp)

S: 'SClient::Process_Packets', 'SClient::Process_AI'; sclient.cpp's `__FILE__` in 65 asserts.
**0x14F8 bytes**, two bases: the client base (§2.1) at +0x00 and a one-slot class at **+0x1150**
(constructor 0x75A280, vtable **0xB0323C**, slot 0 0x75A530 in soptions.cpp: the client's options,
C). Constructor **0x7405E0** (0x3D8; ClientTaskClass::Init 0x766350), destructor **0x740BB0**
(0x2EA; 0x7661C0). Members the constructor builds: two 2 x 0x30-byte arrays (+0x11AC, +0x120C,
element constructor 0x740AE0 / destructor 0x755A10), a 6-slot object at +0x12C4 (vtable 0xB032BC,
0x757410 / 0x758D60: in the Delta template run, §2.2's tail), **TicketClass +0x12D8**, a
`VectorClass<Vector3>` at +0x1340 (vtable 0xAE9B0C, WWLib), an INIClass. From the frame and the
tasks: +0x1124 the connection (base), +0x1350 / +0x1354 objects called every frame, +0x1368 the
object tsk_game's Render reads (+0x68), **+0x136C a view mode** (3: the 3-D movie view,
assert `Movie3DView` l.4104; 6: the scripted movie view, `ScriptedMovieView` l.4127).

Vtable **0xB03240**, 26 slots (the base's 23 plus 3):

| Slot | Address | Size | What | Evidence |
|---|---|---|---|---|
| +0x00 | 0x741180 | 0x7E8 | Init: metals.ini, rs.ini, the decal and envmap INIs, `Bridge_Mode`, font_med10.tga (ClientTaskClass::Init calls it once the kernel is set) | C (symbols.json's name is wrong, §8) |
| +0x04 | 0x741BB0 | 0x78E | Shutdown (ClientTaskClass::Shutdown) | C |
| +0x08 | 0x7427A0 | 1 | `ret` | |
| **+0x0C** | **0x7427C0** | 0xE1 | **Process_Packets** (kernel-map.md §5's loop) | S |
| +0x10 | 0x7428F0 | 0x85 | | |
| **+0x14** | **0x742B60** | 0x5B6 | the frame: returns 0..5 to ClientTaskClass's MODE_SECTOR 0x766BD0; Is_Dead on the connection -> 'Lost connection to server.' box and a do-nothing connection; the input queue (0x42EDB0, 0x42EA80, 0x42E8A0, inpdsp 0x429A20, the key handler 0x7436A0); **calls Process_Packets through its own slot +0x0C at 0x742F25**; Process_AI 0x744260 | S (strings), C |
| +0x18 | 0x740AC0 | 3 | | |
| +0x24 | 0x745730 | | (overrides the base's id setter) | |
| +0x34, +0x3C, +0x40, +0x44 | 0x74ABC0, 0x743330 ('Debug mode turned %s.'), 0x744B90, 0x744BD0 | | overrides | S (+0x3C) |
| +0x1C, +0x20, +0x28 .. +0x30, +0x38, +0x48 .. +0x58 | the base's | | inherited | |
| +0x5C | 0x740F60 | 0x1AB | loads action.ini, cskill_*.ini, testeffects.ini, gutter_color.ini, cbasset.ini | C |
| +0x60 | 0x741B70 | 0x2C | | |
| +0x64 | 0x744690 | 0x3CF | Render (ClientTaskClass::Render, Wait_For_Redirect) | C |

sclient.h's `Unknown_00 .. Unknown_64` are these slots (its +0x5C..+0x64 are the three new ones).

Windows:
* **0x73F680-0x7405E0**, 11 functions, 3.9 KB: sclient.cpp's file statics (a sound-name vector,
  'megan_wlc.wav', atexit) and a small class (vtables 0xAE7D54, 0xAE8014, 0xB03194).
* **0x7405E0-0x7429B0**, 13 functions, 9.2 KB: constructor, destructor, Init, Shutdown, the
  options loader, Process_Packets, 0x7428F0. No asserts; the first assert is 0x7429B0 (l.749).
* **0x7429B0-0x751BE0**: sclient.cpp's attributed run, **111 functions, 48.7 KB attributed /
  62 KB window**, 65 asserts (lines 749-4127, not in address order: §8): the frame, the camera keys
  ('Camera set to 1st person mode with bridge.', screenshots), the chat panel, Process_AI,
  the 'ClientSendUITriggers' aux data.
* **0x751BE0-0x75A530**, 200 unattributed functions, 35 KB: the aux-data classes' inline
  constructors and templates emitted here (DeltaPlayerClass 0x752510, DeltaRPGInfoClass,
  DeltaSkillClass, DeltaSkillAbilityClass, DeltaMissionClass, DeltaGroupInfoClass,
  DeltaGroupMemberClass, `DeltaTemplateClass<...>`, `KVPairTemplateClass<...>`: symbols.json),
  then soptions.cpp's constructor 0x75A280.

**Reach**: 0x73F680-0x751BE0 calls 281 functions in 92 app files (camera_manager 13,
mainview 12, ui_chat_panel 11, client.cpp 10, the aux-data templates 42, avatar, co,
control_keys_manager, musicmanager, lego_identifier, soptions, input, inpdsp, smouse, packets
pkdebug / pkjoblist / pkflag ...). **Its users**: ClientTaskClass (constructor, destructor, the redirect, the send, four virtuals), cpacket.cpp's 99
`Process_Packet(SClient *)` handlers (109 functions, 0x733700-0x73F680, 49 KB window), and
client_lego_impl.cpp (38 attributed, 0x729BF0-0x732790, 137 functions, 35.7 KB window), which
read and write SClient's members: SClient's layout is their gate, not its code.

The movie client (mclient.cpp): constructor 0x7EC3E0 (assert l.83, ArgvClass), vtable 0xB07864
(23 slots: +0x00..+0x14 and +0x1C its own 0x7EC550-0x7ECAA0, the rest the base's); window
0x7EC3E0-0x7ED410, 21 functions, 4.1 KB. Called by the scripted movie task (tsk_smovie.cpp
0x76C8B0, rebuilt with the class declared).

## 3. The login and character-select screens

### 3.1 Correction: 0xAF2C30 and 0xAEFB60 are not vtable starts

They are slot **+0x40** of the screens' tables, which start at **0xAF2BF0** (login, 34 slots) and
**0xAEFB20** (character select, 36 slots). The constructors store 0xAF2BF0 / 0xAEFB20 at +0x00
(nothing references +0x40). So the earlier research's (N) "slot 0x20 per-frame, slot 0x28
event handler" are slots **+0x60 and +0x68**, as kernel-map.md's row 9 inherited them.

| Screen | File (S) | Constructor | Bytes | Vtables (+0x00 / +0x04 / +0x38; a 0x10-byte helper) | Own slots | Window |
|---|---|---|---|---|---|---|
| login | ui_login.cpp (7 asserts) | **0x5ACB80** (`(view, view +0x74)`: 'Name_fe01a', 'Password_fe01a', 'fa_fe01a.ini', 'FE01A'); destructor 0x5AD350 | 0xDC | 0xAF2BF0 / 0xAF2BE4 / 0xAF2BD8; 0xAF2BBC | +0x00 0x5AD350, +0x60 0x5AD540 (per frame), +0x68 0x5AD610 (events) | 0x5ACB80-0x5AFCD0, 33 functions, 12.6 KB (appfile.py: 12 attributed, 4,000 bytes) |
| character select | ui_avsel.cpp (15 asserts) | **0x55C650** ('FE12A', 'FE01C', 'FE_ROOM', 'FA_FE12A.ini', 'click_me'); destructor 0x55CC90 | 0x124 | 0xAEFB20 / 0xAEFB14 / 0xAEFB08; 0xAEFAEC | +0x00, +0x60 0x55CEA0, +0x68 0x55D060 (also the referrer: 'Sorry, you cannot refer yourself.'), new +0x88 0x55EBD0, +0x8C 0x55ECB0 | 0x55C650-0x560BC0, 55 functions, 17.8 KB (appfile.py: 14 attributed, 5,782 bytes) |

N's field tables for the two screens hold for these objects; their offsets are from the object's
start.

### 3.2 The UI base classes and how far they reach

```
[an inline base: vtables 0xAEAB48 (+0x00), 0xAEAB3C (+0x04)]
  -> gclass.cpp's class: constructor 0x4A1880 (vtables 0xAEAAD8 / 0xAEAACC; members +0x08 byte,
     +0x0C, a member built by 0x4F9350); asserts 'Get_Targeting_List() != NULL' (l.325),
     'Get_Target_Node()' (l.344); its only derived class is intrface.cpp's
  -> intrface.cpp's class: constructors 0x540A90 / 0x540D30, vtable 0xAEEF98 (34 slots), a
     third table at +0x38 (0xAEEF80); asserts 'ssl', '!View', 'PartialDeactivatedGadget',
     '!Active' (l.84-333)
  -> 25 screens: login, character select, ui_avatr, ui_class, ui_shard, ui_shipcust,
     ui_registration, ui_popupmsg (the front end, §3.3) and ui_mfd, ui_ptmis / ptnew / ptppl,
     ui_string_list, ui_treem, ui_verb, ui_line_display, ui_talkt_space, ui_skills, backint,
     bridge_int, debrsint, frameint, smouse's 0x515640, ...
```

The interface base's slots (0xAEEF98): +0x00 0x540D00 (destructor), +0x04..+0x40 and +0x4C,
+0x50 the gclass run 0x4A1800-0x4A2010, +0x44 / +0x48 gclass.cpp's two asserted functions, +0x54,
+0x64..+0x78 **0x515690-0x515750** (one-line defaults emitted next to smouse.cpp, §8), +0x58 /
+0x5C / +0x60 / +0x7C / +0x80 / +0x84 intrface.cpp (activate / deactivate with
`PartialDeactivatedGadget`). The two screens override only +0x00, +0x60, +0x68 (and avsel adds two).
tsk_login.h's `CharacterViewUnknown1B4Class` (31 slots, +0x78 handles the global error) is this
base: CharacterViewClass +0x1B4 is the current screen.

Windows: gclass.cpp with the inline base 0x4A1590-0x4A2100, 35 functions, 2.9 KB (2 attributed);
intrface.cpp 0x540940-0x541DE0, 43 functions, 5.3 KB (19 attributed, 2,808 bytes). Their callees
outside the tree: gadget.cpp (0x65ECE0, 0x65EED0), intrlist.cpp (0x686B50, 0x686FA0),
interface_static_sort_list.cpp (0x6DB380), view.cpp, mousetbl's neighbour 0x721D50, 0x4F9350 /
0x4F9740 (between shipcust_desc.cpp and split_string.cpp).

### 3.3 CharacterViewClass and the front end it builds

CharacterViewClass (charview.cpp, constructor 0x678450, 0x514 bytes, vtable 0xAFBD5C, 24 slots)
derives from uiview.cpp's class (constructor 0x65A6C0; uiview.cpp 8 attributed, 2,477 bytes) and
owns an avatar descriptor and a ship customisation descriptor of its own (§5). **0x679160**
builds the screen for its step (+0x1AC) into **+0x1B4**, then calls the screen's +0x54, view.cpp's
0x6B4940 and the task's mouse (+0x14 is the LoginTaskClass, whose slot +0x08 returns itself;
its +0x18 is the mouse, §3.4: 0x515400, 0x515120(2)):

| Step | Constructor | Bytes | File |
|---|---|---|---|
| 0 | 0x5DB670 | 0xD4 | ui_popupmsg.cpp (the window before its first assert) |
| 1 | 0x5ACB80 | 0xDC | ui_login.cpp |
| 2 | 0x55C650 | 0x124 | ui_avsel.cpp |
| 3 | 0x574710 | 0x154 | ui_class.cpp (window) |
| 4 | 0x553AF0 (, 0) | 0x2F8 | ui_avatr.cpp (window) |
| 5 | 0x615A50 | 0xEC | ui_shard.cpp (the galaxy select) |
| 6 | 0x622180 (, step +0x1B0 == 5, 0) | 0x26C | ui_shipcust.cpp |
| 9, 10, 11, 12 | 0x5EF0B0 (, 3 / 20 / 3 / 0) | 0x5B8 | ui_registration.cpp |
| 7, 8 | none | | |

charview.cpp: 36 attributed (6,027 bytes); window 0x677340-0x67DF50, 100 functions, 27.7 KB.
Its callees: avatar.cpp (9), the screens' constructors, soundlib, env_map_manager, smouse,
the message box, uiview.

The front end's screens, windows (constructor to the next file's first attributed function):

| File | Window | Functions | KB |
|---|---|---|---|
| ui_popupmsg.cpp | 0x5DB670-0x5DCD20 | 23 | 5.8 |
| ui_login.cpp | 0x5ACB80-0x5AFCD0 | 33 | 12.6 |
| ui_avsel.cpp | 0x55C650-0x560BC0 | 55 | 17.8 |
| ui_class.cpp | 0x574710-0x5790C0 | 44 | 18.9 |
| ui_avatr.cpp | 0x553AF0-0x55C350 | 42 | 34.9 |
| ui_shard.cpp | 0x615A50-0x61A4D0 | 38 | 19.1 |
| ui_shipcust.cpp | 0x6211D0-0x62B2E0 | 85 | 41.2 |
| ui_registration.cpp | 0x5EF0B0-0x5FFFC0 (attributed run; up to 0x60D8D0) | 88 (234) | 69.4 (125) |
| charview.cpp, uiview.cpp, intrface.cpp, gclass.cpp | §3.2, §3.3 | ~185 | ~39 |

**About 290-345 KB in all**, before the UI kit they call. The login screen's callees by file:
charview.cpp 13, intrface.cpp 4, fontmanager.cpp 4, textgadg.cpp 4 (the edit boxes 0x6AA570 /
0x6AA6C0 / 0x6AA810), the browser manager 3, tsk_login.cpp 3 (Set_Username 0x7697A0,
Set_Password 0x7697E0, Close_Global_Connection 0x769CE0), avatar.cpp 3, fontdatabase.cpp,
intrlist.cpp, ui_registration.cpp's demo checks 0x5FADE0 / 0x5FAE70, the message box,
tutorial_manager.cpp, gadget.cpp, soundlib.cpp, env_map_manager.cpp (33 app groups for avsel,
24 for login).

### 3.4 The task's other UI members

* **+0x0C** (`LoginUnknown0CClass`, 0x18 bytes): inpdsp.cpp's input dispatcher (constructor
  0x429630, destructor 0x429800, 0x429A20 takes an event); inpdsp.cpp window 0x429630-0x429F60,
  9 functions, 2.3 KB.
* **+0x18** (`LoginUnknown18Class`, 0x30 bytes): the software mouse (C: 0x515440 reads the cursor
  position 0x42ECF0 each frame and moves its cursor object, +0x2C); constructor 0x514FE0, in the
  run 0x514400-0x515510 before smouse.cpp's first attributed function; 0x514FE0-0x515510, 9
  functions, 1.3 KB. CharacterViewClass reaches it through the task (its +0x14).
* CharacterViewClass +0x34 (`CharacterViewUnknown34Class`): the message box object, methods
  0x588A70 / 0x5890B0 in the run 0x585780-0x58A080 before ui_dynamicwindow.cpp (73 unattributed).

## 4. kcom auth (authlogin.dll)

* **The DLL**: `release\authlogin.dll` (Net-7's client folder; 114,688 bytes, linked 2002-07-17,
  version 3.3.0.6) exports two functions,
  `AuthLoginLib_GetAuthorize` and `AuthLoginLib_GetComponentFactory`, and imports KERNEL32,
  ADVAPI32, WININET. client.exe imports only `AuthLoginLib_GetComponentFactory` (binary.md §3).
* **The interfaces** (tsk_login.h, from Fetch_EA_Ticket 0x76A7B0 and 0x792040): C++ objects
  with COM-style slots, `Release` at +0x08. The factory (`authFactory`) creates the auth
  interface with id 0x67 at slot +0x1C (`kr`, `kcom::ResultSucceeded(kr)`: S, the assert);
  `iauth` +0x28 `Login(name, password, "2184", base, url, 5)` returns a result object whose
  slots +0x1C (success), +0x20 (code), +0x24 (message), +0x28 (ticket), +0x2C (a third string)
  AuthLoginResponseClass's constructor copies.
* **AuthLoginResponseClass** (S: `response`, `response.Get_Ticket()`): constructor **0x792040**
  (0x1EE; sscanf '%*4c%d' for the code), `Get_Error_Text` **0x7922B0** (0x60: a text per code, 'Unknown Error'
  for the rest), destructor inline (0x76AC60 in tsk_login.cpp). They lie
  in a run of 66 unattributed functions after nlp.cpp (0x78F690-0x792590) with no `__FILE__`;
  their neighbours 0x792320-0x792590 store vtables 0xB04B1C / 0xB04B40 (not identified).
* **No EA source**: `kcom`, `IComponentFactory` and `AuthLoginLib_*` occur nowhere in EA's
  published code under `C:\EnBDevelopment\EA` or `SDK` (searched whole words).
* **What the client needs**: declared (tsk_login.h has it: the two interfaces, the slots used,
  the response) and, to link a program, an import library: a two-line `authlogin.def` turned into
  `authlogin.lib` with VC6's `LIB /DEF`. Nothing of authlogin.dll is compiled. A test program can
  load a stand-in DLL exporting the factory (never the real one against EA's dead servers;
  Net-7's patched copy talks to Net-7's auth server; out of scope here).
* Size: 2 functions, 0.7 KB, plus the .def.

## 5. The login's data objects

LoginTaskClass is 0x1148 bytes (tsk_login.h). Its members by class:

| Member | Class (tsk_login.h) | Bytes | Constructor / vtables | Where its code is | Other users |
|---|---|---|---|---|---|
| +0x38 `Ticket` (and ClientTaskClass's, SClient +0x12D8) | TicketClass | 0x48 | 0x8C15F0, destructor 0x8C16D0, `=` 0x8C16F0; vtable 0xB0C608 | the module after wnet's udp.cpp (§5.1) | MasterJoin 53, Login 2 |
| +0x80 `Characters[5]` | LoginCharacterClass (the character slot) | 0x90 | 0x8C06E0, destructor 0x8C0750; vtables 0xB0C560 (Write 0x8C0770, Read 0x8C0940, Get_Size 0x8C0AC0) / 0xB0C55C (visitor 0x8C0AE0) | same module | the avatar list packet 112, Go_Next_Step |
| +0x350 `Unknown0350[5]` | LoginUnknown350Class | 0x10C | 0x4BEB50 (+0x4C1460 sets the name); vtables 0xAEB9D0 / 0xAEB9CC | **avatar_desc.cpp** (C: the character's looks; N: the names at +0x424 + slot x 0x10C) | charview.cpp, the create packet 114 |
| +0x88C `Unknown088C[5]` (T4's 0x160-byte objects) | LoginUnknown88CClass | 0x160 | inline (0x7688F0 out of line); 0x4F6210 / 0x4F6230 / 0x4F62F0; vtables 0xAF78A0 / 0xAF789C (Write 0x4F6330, visitor 0x4F6750); `=` 0x55D8A0 (in ui_avsel.cpp's window); four 0x44-byte parts 0x6191F0 (after ui_shard.cpp) | **shipcust_desc.cpp** (C: the ship's customisation; N: 'the avatar's looks ... for the 3-D model') | charview.cpp (owns one), the create packet 114 |
| +0xF6C `Galaxies` | `std::list<LoginGalaxyClass>` | 0x60 each | vtable 0xAEFBD0 (Write 0x83C730, Read 0x83C7C0, Get_Size 0x83C850); destructor 0x76BBF0 | app\common, in the run 0x838E50-0x83D490 after galaxy_map.h (102 unattributed) | ui_avsel.cpp, ui_shard.cpp (galaxy select), the avatar list packet 0x862D90, 0x863360 |
| (stack) | LoginNameDataClass | 0x84 | vtable 0xB04160 (0x78E0E0 / 0x78E110 / 0x78E140) | the run between avatar.cpp and nlp.cpp | |
| (stack) | LoginUnknown78CEE0Class (the creator's registry record: 'SOFTWARE\Westwood Studios\Earth and Beyond\Character and Starship Creator') | StringClass | 0x78CEE0, 0x78D480, 0x78DAF0 | same run | |
| +0x1098 `Ticket_Info` | LoginTicketInfoClass (11 strings) | 0x9C | inline | tsk_login.cpp | GlobalLogin 118 |

### 5.1 Where they live

* **avatar_desc.cpp** (app\shared): one asserted function (0x4C05D0); window from the constructor
  0x4BEB50 to chat_edit_stream.cpp 0x4C45B0, 78 functions, 23 KB (shares the run 0x4BB8A0-0x4C05A0
  with kv_util.cpp's tail).
* **shipcust_desc.cpp** (app\shared): 47 attributed (4,032 bytes); window 0x4F6160-0x4F9740, 94
  functions, 13.8 KB.
* **The module after WWOnline's wnet** (0x8BF5F0-0x8C1B30, 70 unattributed between udp.cpp's
  `UDP::Wait` and WWMath::Init; no `__FILE__`): the character slot (0x8C06E0..), TicketClass
  (0x8C15F0..) and a `KVPairTemplateClass<MoneyClass>` member (0x8C0FD0): KV-encoded data objects
  linked after the libraries; 0x8C0600-0x8C1B30, 45 functions, 5.4 KB.
* **The galaxy entry**: 3-4 functions in app\common's run after galaxy_map.h (the run is 102
  functions, 18 KB, mostly not the galaxy's).
* All of them are the packet layer's types too: they are the aux-data / KV members that keep
  the avatar list (112), create character (114) and global login (118) packet classes out of
  `pkdecode.py`'s simple set (kernel-map.md §6 row 5), with app/shared's kv_*.cpp already in the tree.

### 5.2 Singletons the tasks call

| Address | What (C) | File | Users here |
|---|---|---|---|
| 0x7CA2D0 -> 0xBE8890 (constructor 0x7C97B0, function-local static) | the browser / URL manager: 'HTMLDIALOG', 'HELP', 'SUPPORT', 'BUG', 'PRIVACY', 'CREDITS', the URLs, 'ShellExecute Error: %X'; `Set_String("lkey")`, `Set_Int64("characterid")` | the run 0x7C91A0-0x7E35D0 between ui_starmap.cpp and avatar_pip.cpp (228 unattributed); 0x7C97B0-0x7CCB30 ~28 functions, 13 KB | ClientTaskClass, LoginTaskClass (0x7CB610 the LKey URL), the login screen |
| 0xBE8B38 | the options manager (WinMain's 'options manager') | option.cpp 0x7F84F0, 0x7F8EF0 | ClientTaskClass::Init |
| 0xBE67B8 | the font database | fontdatabase.cpp 0x6C47B0 | ClientTaskClass::Init |
| 0x7228C0 / 0x723150 | a reference-counted global table (0xBE7C70, count 0xBE7CD4) | before client.cpp (0x722890-0x726100, 53 unattributed) | LoginTaskClass constructor / destructor |

## 6. Proposed reconstruction order

Sizes are windows (functions, bytes to the next verified start) unless marked attributed.

| # | What | Size | Needs | Test |
|---|---|---|---|---|
| 9a | **kcom auth**: AuthLoginResponseClass 0x792040 / 0x7922B0 (file unknown: a PROVISIONAL unknown_*.cpp under D10) and `authlogin.def` -> `authlogin.lib` | 2 functions, 0.7 KB | nothing new; std::string (VC6) | Fetch_EA_Ticket links; a stand-in DLL's factory in a test program |
| 9b | **the login's data objects**: the character slot and TicketClass module (0x8C0600-0x8C1B30), shipcust_desc.cpp, avatar_desc.cpp, the galaxy entry, the name data and the creator's record | 45 + 94 + 78 + ~10 functions, ~43 KB (5.4 + 13.8 + 23 + ~1) | kv_*.cpp, LinearBufferClass, delta.h's visitor (in the tree); the registry record writes HKLM/HKCU (D69: never run against the real registry) | round-trip each through LinearBuffer against the recordings' avatar list (112) and global login (118) messages; unblocks those packet classes and tsk_login's Unknown_50 0x7683F0 / 0x768CB0 |
| 9c | **client.cpp**: the client base (constructor, destructor, 23 slots, object tables, the do-nothing connection, the connection setters) | 48 functions, 9.0 KB + 29, 6.1 KB (the tables' helpers) | connect.h / pkdebug (in the tree); px.cpp, px_process.cpp, spline.cpp, co.cpp, eff_o.cpp, control_keys_manager.cpp, client_lego_impl.cpp, 0x763E00 (declare) | a program with a derived client: add / find / remove by game id; the redirect setter over a memory connection |
| 9d | **SClient's skeleton**: the class with its 0x14F8 layout, constructor, destructor, Init / Shutdown / the options loader, Process_Packets, the frame 0x742B60, Render; the rest declared; soptions.cpp's base | 13 + 11 functions, 13 KB; soptions.cpp 7 functions, 1.2 KB | (9c); camera_manager, mainview, ui_chat_panel, the aux-data Delta classes (declare; app/shared/auxdata in the tree) | udpreplay: Process_Packets over a recorded sector stream with cpacket's handlers stubbed |
| 9e | **sclient.cpp's body** and the Delta template run after it | 111 functions, 62 KB; 200 functions, 35 KB | (9d) and 92 app files (281 callees) | as (9d); then cpacket.cpp (109 functions, 49 KB) and client_lego_impl.cpp (137, 35.7 KB) on SClient's layout |
| 9f | **the UI base**: gclass.cpp with its inline base, intrface.cpp, the defaults by smouse.cpp, smouse.cpp's mouse, inpdsp.cpp | 35 + 43 + 9 + 9 functions, ~12 KB | gadget.cpp, intrlist.cpp, interface_static_sort_list.cpp, view.cpp, 0x4F9350 (declare) | a dummy screen through Activate / Deactivate |
| 9g | **CharacterViewClass** (charview.cpp) and uiview.cpp's base | 100 + ~8 functions, ~31 KB | (9b), (9f); the screens' constructors (declare); avatar.cpp, env_map_manager, soundlib | build the view with stub screens: step changes (0x679160) |
| 9h | **ui_login.cpp and ui_avsel.cpp** | 33 + 55 functions, 30.4 KB | (9f), (9g); fontmanager, fontdatabase, textgadg, the message box, tutorial_manager | the earlier research's (N) fast-login events against the screens |
| 9i | the rest of the front end: ui_popupmsg, ui_class, ui_avatr, ui_shard, ui_shipcust, ui_registration | 23 + 44 + 42 + 38 + 85 + 88 (234) functions, ~190-245 KB | (9f), (9g); the 3-D avatar (avatar.cpp), the ship customisation | each screen built and driven by events |

Step 9's own leftovers (Run, Go_Next / Back_Step, Pre_Run, Start_Game: register / shape
residues under D89; Unknown_50 and the fast start: inline budget, in Fable's queue) don't wait on
any of these: their callees are declared.

## 7. Recommendation (a proposal; the user decides)

Open **SClient's side next: 9a, 9b, then 9c and 9d** (about 70 KB in four small steps), and leave
the UI front end (9f-9i, ~290-345 KB with a reach into the whole UI kit) for later.

* 9a is a one-sitting item that closes tsk_login's last undeclared import and gives
  Fetch_EA_Ticket its real declarations.
* 9b's objects are shared with the packet layer: they are what keeps packets 112, 114 and 118
  (and the other aux-member classes) out of the generated set, and the parked Unknown_50 /
  0x768CB0 copy them. The kv_* encoders they build on are already in the tree.
* 9c is small, has nearly all its needs in the tree (connect.h, dbgconn, pkdebug) and is
  shared by SClient and the already-rebuilt scripted movie task's client.
* 9d gives SClient a real layout, which is the gate to the next big app block: cpacket.cpp's 99
  handlers and client_lego_impl.cpp (~85 KB together), testable with udpreplay on the sector
  recordings, as the packet classes were.
* The front end can be bypassed for the step 9 test (kernel-map.md §6 row 9's "a login against a
  recorded server"): a test program links LoginTaskClass with a stub CharacterViewClass and
  drives it the way the earlier research's (N) fast login does (Set_Username, Set_Password, slot +0x4C, then the
  slot choice and Go_Next_Step).

Alternative: if the goal is to see the login screen drawn (a VIDTEST-style milestone), 9f -> 9g
-> 9h is the path, about 75 KB before fonts, text gadgets and the 3-D avatar are counted.

## 8. Traps and corrections found while mapping

* **0xAF2C30 / 0xAEFB60** (kernel-map.md §6 row 9, from the earlier research, N) are slot +0x40
  of the vtables **0xAF2BF0 / 0xAEFB20**; those notes' "slot 0x20 / 0x28" are +0x60 / +0x68.
  Walk back to the dword the constructor stores (`.work/map9/vrefs.py`) before taking any
  quoted vtable address as a start.
* **symbols.json's `WW3DAssetManager::Load_Procedural_Textures` at 0x741180** (source
  `strings`: 'metals.ini') is SClient's slot +0x00, Init: sclient.cpp's window, called by
  ClientTaskClass::Init. Renegade's asset manager reads the same INI; E&B moved that into SClient.
  `pseudocode.py` and `client.py` print the wrong name.
* **0x727E10** (`Create_Connection("SectorServer", 1)` into SClient +0x1124; kernel-map.md §4 / §5)
  has no caller and no reference, through its thunk or otherwise: the connection is the base
  constructor's do-nothing one until ClientTaskClass's redirect calls 0x727F00. Dead code the
  incremental link kept.
* **SClient::Process_Packets' caller** (kernel-map.md §5 "not traced"): SClient's frame slot
  +0x14 0x742B60 calls it through its own vtable +0x0C at 0x742F25.
* **The client base's default slots are 0x9CCD10** (`xor eax, eax; ret`) for +0x00..+0x14:
  `client.py refs` on a slot address finds nothing because vtables point at thunks; use the
  thunk-aware refs (`.work/map9/vt.py refs`, worth folding into client.py).
* **Inline header virtuals land next to their first user**: connect.h's in client.cpp
  (kernel-map.md §7) and the interface base's +0x54 / +0x64..+0x78 defaults 0x515690-0x515750 in
  smouse.cpp's run, right after the first interface-derived constructor 0x515640.
* **sclient.cpp's asserts are not in address order** (0x7429B0 l.749, 0x742B60 l.4104 / 4127,
  0x743290 l.940): the frame was moved, or it is defined late in the file and emitted early;
  `#line` per function when drafting.
* **appfile.py's `__FILE__` windows miss the constructors**: every screen's constructor
  (0x5ACB80, 0x55C650, 0x574710, 0x553AF0, 0x615A50, 0x5DB670), SClient's (0x7405E0) and the client
  base's (0x726100) lie in the unattributed run before their file's first assert. Use
  `appfile.py show FILE --neighbours N` and the vtable stores to place them.
* **0x8C06E0 (the character slot) and 0x8C15F0 (TicketClass)** sit after WWOnline's wnet in the
  link order, among KV templates: app code linked as a library after wnet, not wnet itself.
* The earlier research (N) calls +0x88C + slot x 0x160 "the avatar's looks"; the code puts it
  in shipcust_desc.cpp's class (0x4F6210..), and the avatar's looks are the 0x10C avatar_desc
  records at +0x350.
