# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

### 2026-11-10 (13:30 PST, day): the week's last 4 %: ui_friends, ui_stringentry, the dynamic window toggle and the item examination rows, the chat panel stand-ins retired, sendpad.py; +178 pairs over the run's baseline, 0 lost outright

- **Run** run/2026-11-10 (PR #185), coordinator Opus 5.5 on the subscription, base 041ad8cb, baseline `.work/imports.041ad8cb.json`,
  plan `.work/run-2026-11-10/plan.md`. The user: open progress.md items until the week's usage is spent (96 % at 09:00, reset
  12:00); at 10:40 "launch 2 more builders". Weekly 96 % -> 98 % at 11:14; the week reset at 12:00 (the old week ended at ~98-99 %);
  no launches into the new week, the last builders finished in it (~1 %). Builders: Opus FR2 ~272k, TC1 ~206k, DW1 ~236k, AP2 ~97k,
  CP2 ~230k, IE3 ~222k, SU1 ~305k; Sonnet SP1 ~107k, CA1 ~51k, HC1 ~52k.
- **HEAD against the baseline:** gained 178, 0 lost outright (pacing.py heartbeat at 8155b207); every landing passed wt.py's
  link checks.
- **Files:** FR2: ui_friends.cpp's ctor 0x58F210, slots +0x54 0x58F6B0 / +0x68 0x590020 (the 11-case key switch), 0x590E50,
  0x591C30 exact (+34); +0xB0 is a UIPTradeCreditsClass (AP2: 0x58F6B0's new(0xD0) and 0x649890), so ui_group.h's UISymbolChatClass
  lost 0x64A220 / 0x64A240. TC1: app/client/ui_stringentry.cpp (UIPTradeCreditsClass 0x649890-0x64A32F, the text entry window
  the guild / ignore / friends / trade windows use; file name from the tooltip section UI_STRINGENTRY; all 12 exact, +33), the class
  out of sclient.h. DW1: dynamic_window_toggle.cpp (ToggleButtonDynamicWindow, client RTTI and __FILE__, 0x51D790-0x51EADF),
  gd_dynamicelement.cpp (DynamicElementGadgetClass 0x6686F0, interim) and gadget_collection_interface.h (GadgetCollectionInterface /
  GadgetEvent, RTTI; was UIDynamicWindowUnknown84Class): 13 of 14 exact (+49). IE3: the item examination rows on them (attributes,
  options, specials: ctors, dtors, the toggle callback, MemberCallback slots, ToggleButtonStateEvent() 0x51E7F0, the three sorts):
  all exact (+49). SU1: ui_starbase.cpp's Init 0x634140 (9.2 KB) functional, NOT EXACT 0.55 (frame 0x1D0 vs 0x1A0), with the window's layouts (+0xCC entries with their FontSys texts, +0x1F8 / +0xB8 local classes, +0xB4 NPC slot / terminal lookups, +0xBC ExtraBones NAME 3), the lights block moved to the file's end (the client's order), 0x636520 inline exact; its first bring-over was refused by the link check (a header struct against the gen stubs' class), fixed (+13).
- **Stand-ins retired:** AP2: ui_chat_panel / ui_ptrade include avatar_pip.h. CP2: cpacket.cpp's ChatPanelInterface and four
  chat window stand-ins onto ui_chat_panel.h / ui_friends.h / ui_playeropt.h / ui_radio_list.h / ui_radio_admin.h; ui_guild.h
  (D137) for UIGuildClass (sclient's 0x81BF40 now links to ui_guild.cpp's real member); mainview.cpp's and sclient.cpp's
  ChatPanelInterface stand-ins onto ui_chat_panel.h; sclient.h's UIChatPanelUnknown138Class deleted; 0 / 0 each, four
  link_baseline symbols now resolved.
- **Tools:** SP1: tools/vc6/sendpad.py (the SEND_SYMBOL_PAD sweep through verify's path, ~5 min for 0..63) and
  `wt.py bring-over --sendpad` (opt-in; it ran on three bring-overs, none needed a retune: each builder had swept already).
  SEND_SYMBOL_PAD today: 4 -> 56 (TC1) -> 13 (CP2) -> SU1's value. CA1: the Claims Action's startup failures were 11 runs on
  2026-11-09 16:02Z-18:11Z only; no repository cause; §4 item 9 keeps a check at the next merge.
- **D182 harness:** HC1 at 89be45e6 (FR2 landed): build 542 / 542, clienttest 103 checks, 0 failed, trace equal (2510 lines),
  udpreplay alex3 24771 equal, handshake 54 / 54; no new asserts. HC2 at 8155b207 (the run's head): build 545 / 545, clienttest
  103 checks, 0 failed, trace equal (2510), udpreplay alex3 24771 equal, handshake 54 / 54; no new asserts. The POC was not run
  this run (tools/agy/poc_check.sh at the next run's start: TC1 / CP2 / SU1 changed POC-linked code).
- **Traps filed** (workflow.md §3.1, §3.3, §3.4, §3.6): byte-array member stand-ins break on a retype (grep the run branch's head
  too), struct vs class mangling against gen stubs, inline ctors with RefCountPtr members in shared headers, sendpad's watch set
  from a ledger that already lost 0x758210, split pad windows, the templated disconnect, operator< int locals for sorts,
  a shrunk callee getting inlined, MSYS `//` paths.
- **Proposal (D137, the coordinator's call, not done):** rename UIPTradeCreditsClass to UIStringEntryClass in one cross-cutting pass.

### 2026-11-10 (06:30 PST, night, 2026-11-09b): the world's windows react (mouse to gadgets, chat entry and keyboard, the PDA's character page like retail, cargo and equipment windows); +416 pairs over the run's baseline, 0 lost outright beyond D66 (vi)'s 0x65B900

- **Run** run/2026-11-09b (PR #183), coordinator Opus 5.5 on the subscription, base fecd64c8, baseline
  `.work/imports.fecd64c8.json`, plan `.work/run-2026-11-09b/plan.md`. The user: first whatever blocks interacting with the
  on-screen items after world entry (ship, chat windows, PDA visible but not clickable), then items that unlock stuck work or
  large unexplored functions with many callers, weekly usage 100 % by 07:00, subscription only (no agy, no API-key builders),
  hourly heartbeats, no blocking questions. Weekly usage 90 % -> 96 % (05:10; resets 11:59). Builders (Opus): IN1 ~194k tokens, PD1 ~96k
  (resumed twice for the pad sweeps), SB1 ~457k, SK1 ~450k, CE1 ~420k, AV1 ~150k, TX1 ~150k, PW1 ~400k, GS1 ~400k, ID1 ~250k,
  UV1 ~130k, ST1 ~150k, IT1 ~200k, SG1 ~140k; Sonnet: HC1-HC5 (D182 harness and POC checks), DOC (this entry's draft), BS1
  (a bisect, stopped once SG1 had the cause). Burn: about 0.5 % of the week per hour with two Opus builders and the coordinator.
- **HEAD against the baseline:** gained 416, 0 lost outright beyond D66 (vi)'s accepted 0x65B900 (pacing.py heartbeat at
  0c009e1c); every landing passed wt.py's link checks.
- **The in-world interaction chain** (each cause found by running the POC in the world and reading what the click or key
  reached): IN1 found why nothing was clickable: sclient.cpp's Do_Frame 0x742B60 called GadgetClass::Process_Input 0x65F780
  through a free declaration `Unknown_65F780`, which linked to a gen stand-in the POC had stubbed empty; PD1 applied it by name
  (the user agreed: sclient.cpp includes gadget.h and calls GadgetClass::Process_Input; +1 shared pair, sclient 0x702240), with
  the pad re-sweep. CE1: chatgadg.cpp ChatGadgetClass (0x65BFD0, 13 functions, client `__FILE__`): typed text shows in the chat
  entry and the second Return sends CLIENT_CHAT_REQUEST 0x00A3 (+37); the POC's 0x42A1A0 stand-in returned no key for
  control_keys.ini, so typed letters hit hotkeys. SK1: ui_skills.cpp (UISkillsClass, was Unknown62D1F0Class), ui_slot_data.cpp
  (SlotDataInterface), ui_slot_info.cpp, drag_mouse.cpp (SMouseDragClass), drag_slot_data.cpp (+166): the PDA's character button
  no longer crashes. AV1: SClientUnknown1394Class and Unknown77E3F0Class are both CharacterViewAvatarClass (ctor 0x77B870), the
  avatar's +0xE8 is Unknown_ChatStreamClass (0x7817E0 rebuilt): the ui_attrb.cpp(237) assert is gone (+3). TX1: Do_Frame called
  free `Unknown_918200`, a gen stub; the client calls WW3D::Sync there, so Get_Frame_Time stayed 0 in the world and every FontSys
  WIPE / FADE effect froze at step 0: the character page's level, class, rank, credits and bar labels draw as retail's
  (+1, ui_scut 0x612820; Set_Tooltip 0x65EFB0 / 0x65F340 and SoundLibraryClass::Apply_Volumes 0x43FD70 by name). GS1 swept the
  free `Unknown_` declarations that link to gen stand-ins (in-world hits 48 -> 35, the POC link's gen stand-ins 408 -> 380):
  SClient 0x743380 (keyboard events were dropped without a gadget source), 0x744B60, 0x744E50, 0x744560 (NOT EXACT) rebuilt,
  unknown_764fd0.cpp (chat colours at SClient +0x13CC), about 25 call sites onto real names (+8). PW1: ui_pda_cargo.cpp
  (UIPdaCargoClass), ui_pda_equip.cpp (UIPdaEquipClass), gd_inv_slider.cpp (+112): both windows open, Equip>> works. ID1:
  inv_data.cpp / inv_data.h, InvDataClass 0x688410-0x689100 (exact but 0x688B90), its POC stand-in dropped, ui_inv /
  inv_render / inv_manu_render / inv_trade_render / client_item_property_builder.h onto the real names (a stand-in class name
  `Unknown688830Class` had linked a NULL gen stub that crashed the POC's cargo window) (+30). HC1 / HC2 / HC3 at the heads of
  PD1, SK1 and GS1: the click toggles the chat split, the character window opens with its labels, the cargo and equipment windows
  show Equip>> / Credits / Sort, typed text shows. The replay's cargo is empty (retail too), so slots stay empty.
- **Other files:** SB1: ui_starbase.cpp's rest (+31; 21 functions exact in 0x633180-0x6437A1), sbaseview.h retypes (+0xEC list,
  +0xB8 / +0xF8 / +0x118 gadgets, +0xD4 SGI hash stand-in), pkstarbase_avatar_change.h ctor 0x8A3600, clienttest.py #defines and
  clienttest.cpp fakes renamed (Python: the PR needs /verify-approved). UV1: app/shared/uvcontrl.cpp / uvcontrl.h (D137): the
  texture mapper controllers, all 12 functions 0x4A0FB0-0x4A1550 exact (+13); the POC's UVController gen stand-ins are no longer
  hit. ST1: ui_avatar_cust.h (NAME 1) holds StarbaseAvatarCreationInterface (RTTI 0xB7AC88); sbaseview.h's
  SBaseViewUnknown19CClass stand-in is gone, AvatarUI retyped, so the starbase's avatar recustomization window is built from the
  real (exact) 0x54B7B0 / 0x551C40 (0 / 0; no replay reaches it yet). IT1: app/common/item_template.cpp (D137) the item templates
  (0x831340 ctor, 0x8316D0 dtor, 0x831840 icon, 0x8318B0 lookup, 0x831950 Mount parser, all exact, +14). The real lookup returns
  the Null Item for empty shortcut slots, and so does ID1's real inventory: the shortcut bar then built gadgets through a gen
  stand-in and every in-world replay crashed at 15d2b82d (HC4). SG1: gd_slot_data's exact 0x670E30 class was named
  Unknown_670E30Class while the callers used SlotDataShortcutGadgetClass (the client's name, ui_inv's FAIL_IF): renamed, the real
  ctor signature in ui_slot_scut.h (three symbols_named replaces rows keep its shorts), and the POC proxy-button stand-in gets
  slot +0xA4: no crash.
- **SEND_SYMBOL_PAD tonight:** 40 -> 12 (PD1: `#include "gadget.h"` in sclient.cpp, window 9..14) -> 37 (SB1's sbaseview.h,
  window 36..38) -> 41 (AV1's sclient.h names, 40..42) -> 43 (TX1, 42..44) -> 30 (GS1: ui_scut.h dropping its duplicate 0x5417D0, window 29..31) -> 47 (GS1's 0x744560, window 46..49) -> 4 (ST1's sbaseview.h, window 1..7; IT1 re-swept after rebasing: holds). Every edit to a
  header sclient.cpp includes moves the window; the bring-over refused SB1 once (cherry-picked onto the next head it lost sclient
  0x758210 outright), CE1's chatgadg.cpp (not included by sclient.cpp) needed no change. IN1's "byte-identical twin" reading of
  the 0x758210 loss was wrong (0x218 B vs 0x702240's 0x31C B). The sweep script
  (`.work/wt/GS1/.work/gs1/sendpad_imp.py`) should move into tools/ (§4 item 9).
- **D66 (vi):** CE1 loses uiview 0x65B900 (guarded atexit for 0x42B320) outright: the client's own chatgadg copy 0x65EC80 makes
  it ambiguous; accepted-corrections.json carries the entry (751cf3e6; precedent 0x5495A0, 0x604150), so the PR needs the
  maintainer's /accept-d66 (D202), and /verify-approved for clienttest.py.
- **D182 harness:** HC1 (ae75446d), HC2 (79edb986), HC3 (99168736): clienttest 103 checks, 0 failed, exit 0, trace equal (2605
  lines at HC3; GS1 updated expected.txt because the real code now runs); udpreplay alex3 as before (24771 equal in order),
  handshake 54; the POC no CRASH / STUB_CALLED. Asserts seen every run: soundlib(838) x4, interface_static_sort_list(503) (GS1:
  radar adds robjs before a current interface; harmless); new at HC3: dynamic_window_background.cpp(147) AnimationData x4; ID1:
  Mounts inv_data_peek.cpp(104) on the equipment list. HC4 (15d2b82d): harness passed, every in-world replay crashed (above);
  HC5 (0c009e1c, SG1's fix): the POC no CRASH (character page, cargo + equipment windows, typed text), udpreplay as before,
  ClientPOC\release\client.exe deployed from it; clienttest crashed after 38 checks: the real Mount parser 0x831950 (IT1) read
  the test data's short `Mount0=3 0,0,0 1 2`. CT1 (9f2c24fc, tests/ only): cbasset.ini's Mount0 is 12 floats, expected.txt
  regenerated by removals only (92 item-template stub lines and 3 `gen Unknown_831950` lines: the real code now runs): 103
  checks, 0 failed, exit 0, trace equal (2510 lines).
- **Traps filed** (workflow.md §3.1, §3.3, §3.4, §3.7, §3.8): free `Unknown_<ADDR>` declarations and callers' local stand-in class
  names link silently to gen stand-ins (the night's class), the SEND pad history and the re-sweep rule, D66 (vi) twins in
  pairs_cmp, POC shot timing (IN1's ~59 frames/s against the virtual-time counts), tail padding and auto_inline forms.

### 2026-11-09 (15:10 PST, day): §4 item 00 done (fontsys 0x6CE4E0 / 0x6CD6F0, ui_mfd 0x5BF350 exact; D221 revisited as P-FN3-1); twelve new window files (inv_data_peek, ui_mount, ui_option_account / _channel / _check, ui_refinery, ui_inv, ui_radio_main / _join / _entry / _list / _admin); +576 pairs over the run's baseline, 0 lost outright

- **Run** run/2026-11-09 (PR #182), coordinator Opus 5.5 on the subscription, base f0a28c6f (#180 merged) with c26bd2a9 /
  b983c485 cherry-picked (82210501, ca8f529b), baseline `.work/imports.f0a28c6f.json`, plan `.work/run-2026-11-09/plan.md`.
  The user: weekly usage at least 90 % by 16:00 (+/-1), subscription only, §4 item 00 first, then items that unlock stuck
  work or large unexplored functions with many callers, hourly heartbeats, no blocking questions. Weekly usage 86 % ->
  90 % (15:03). Builders (all Opus): FN3, FN4, MF1, RF1, MT1, OP1, UI1, CH1, CW1, EB1, SW1 (~3.5M tokens together). Burn: about 1 %
  of the week per hour with 2-3 builders and the coordinator (86 % flat for the first 70 minutes).
- **HEAD against the baseline:** gained 576 (364 shared), 0 lost outright, 0 moves; two addresses lost a shared holder,
  each still held by other files (D66 (i)): ui_cpit 0x6B2070 / 0x73F950 (EB1; cpacket and lego_identifier hold
  0x73F950); the PR needs the maintainer's /accept-d66 (D202). It touches Python (tools/harness/clienttest.py, SW1): it
  also needs /verify-approved. Every landing passed wt.py's link checks; D182's harness after each (clienttest 103 checks, 0 failed,
  trace equal; udpreplay alex3 as before; handshake 54); the POC at 33403b0b ran 40 s to the intro movie (startup only).
- **Item 00 (the user's):** FN3: 0x6CD6F0 and 0x6CDAB0 exact (the +0x303 clear as a byte loop: lea-first rep stos),
  0x6CE4E0 exact through a file-local inline layer Clip_Text (kept, the coordinator: the client's out-of-line += shows the
  deeper level, ~20 budget forms failed, D220's two-layer precedent); D221's evidence: still no sub-object (P-FN3-1, §3).
  FN4: 0x6CE7D0 exact, 0x6CD950 0.59 -> 0.71 (register, parked). MF1: ui_mfd Set_SP_Percent 0x5BE120 and Cast 0x5BF350
  exact in the two-layer find (DurationClass assigned, not constructed: a user operator= as a zero-code spender).
- **New files** (the 2026-11-08d ranking's top untouched pieces, then their neighbours): inv_data_peek.cpp (client
  asserts, +27; 0x68AFF0's 19 callers), ui_mount.cpp (+33), ui_option_account.cpp (+32) (MT1); ui_refinery.cpp (client
  assert, 29 exact, +72; RF1); ui_option_channel.cpp (+44), ui_option_check.cpp (+34), InvDataPeekClass declared once
  (OP1); ui_inv.cpp (client __FILE__, +61) with ui_inv.h's UIInvClass replacing ui_trade.h's / pdaint.h's two stand-ins
  and ui_slot_data.h's base (UI1); ui_radio_main.cpp / ui_radio_join.cpp (+91, CH1); ui_radio_entry.cpp (client FAIL_IF,
  16/16), ui_radio_list.cpp, ui_radio_admin.cpp (+163, CW1). Provisional names (D137) are in §4 item 8.
- **EB1 (the shared vector-erase blocker):** the client's vector<string>::erase 0x4FC7C0 call sits one inline layer below the call site (dtor -> own Unknown_5C -> clear helper -> clear(); push_back; ++it in sorted-insert loops): chat window dtors 0x5E76B0 / 0x5E4300, 0x5E8240 / 0x5E4F10, ui_cpit 0x584690 (parked since 10-31), ui_playeropt 0x5D9D10 exact, +11.
- **SW1 (the chat stand-ins onto the real headers):** ui_chat_panel.cpp's six window stand-ins and cpacket.cpp's three replaced by the real classes (UIRadioMain / Join / Entry / Admin / List, UIOptionCheck / Account, UIPlayerOpt) with the client's signatures; the POC and clienttest links kept (clienttest.py, poc_data.cpp, ui_avsel_stubs.cpp); 0 gained, 0 lost; cpacket 0x73CC90 0.48 -> 0.73.
- **Claims Action:** pull_request_target runs end in startup_failure since 2026-11-09 16:02Z, so a merged run PR's claims
  stay live until the hourly schedule or a manual `gh workflow run claims.yml` reconciles (09:10 today: #180's 39). §4 item 9.
- **Traps filed** (workflow.md §3.1, §3.3): fill and layer forms (FN3), assignment as a spender (MF1), loop / clamp / while
  forms (FN4, MT1), delta_array.h reaches aux_roots.h (MT1), spender and table forms (RF1), stopping an appfile loop
  (RF1), option page forms (OP1), include lines, temporaries and mangled names (UI1), chat window forms (CH1, CW1).

### 2026-11-09 (09:30 PST, morning): P-XR2-1 carried out on XP SP3's msxml.dll (D222); D218-D221 recorded

- **The user's decisions on last night's proposals:** P-IP1-1 yes (D218), P-IP1-2 yes (D219), P-IB2-2 no (D220),
  P-FN1-1 no for now (D221), with fontsys 0x6CE4E0 / 0x6CD6F0 and ui_mfd 0x5BF350 (two-layer form only) first in §4
  (item 00). D218 / D219 are not carried out yet (§4, client_item_property_builder.cpp).
- **P-XR2-1 (D222, c26bd2a9):** msxml.dll 8.00.7002.0 extracted from the user's Windows XP SP3 ISO
  (`I386\MSXML.DL_`, 7z + expand.exe; `.work/msxml/xp_sp3/`, also msxml2.dll 8.30 / msxml3.dll 8.90, never in the tree);
  VC6's `#import "msxml.dll"` on a pool VM (`.work/msxml/gentlh.py`), placed whole as app/client/msxml.tlh / .tli
  (`place.py`), namespace MSXML2 -> MSXML in unknown_7dfed0.*. Evidence (`.work/msxml/guids.py`): client.exe holds
  MSXML 2.0's DOMDocument CLSID 2933BF90 (0xBA0F90) and IIDs 2933BF80 / 81 / 82 / 83 / 8B, none of MSXML 3.0's; the
  hand-cut msxml3 header had compiled F6D90F11 (a different object than retail, invisible to pairs); the new object holds
  2933BF90. verify app against HEAD: 0 gained, 0 lost (0x5FA5F0 kept with the whole header); linkgate passed. Not run:
  the POC through the registration screen, clienttest's harness (D182).
- **Aside:** linkgate reports `Sub14CImplClass::Slot15` now resolved in the POC's baseline (likely XR2's, not this
  change); `linkgate.py check --write-baseline` drops it.
- **Trap filed** (workflow.md §3.1): `#import` the client's own type library, found by its GUIDs. Heredocs mangle
  backslashes in Python (CLAUDE.md's rule, hit again): write `.work/` scripts with the editor.

### 2026-11-09 (05:00 PST, night, 2026-11-08d): the inline-budget rule measured and applied; client_item_property_builder and the item property files, the subs-interface account client and the registration reader over MSXML, ui_jobs, ui_trade, ui_log, ui_guild and ui_guild_list new; 0x752170 back (D217); +735 pairs over the run's baseline, 0 lost outright beyond D66 (vi)'s 0x604150

- **Run** run/2026-11-08d (PR #180), coordinator Opus 5.5 on the subscription, base 3869e813 (#178 / #179 merged), baseline
  `.work/imports.3869e813.json`, plan `.work/run-2026-11-08d/plan.md`. The user: weekly usage at least 84 % by 07:00 (a floor,
  slightly over fine), subscription only (no agy, no API-key builders), items that unlock stuck work or large unexplored
  functions with many callers, hourly heartbeats, no blocking questions. Weekly usage 78 % -> 86 % (04:50). Builders (all
  Opus): SC2, IP1, XR1, FN1, IB1, JB1, TM1, IB2, XR2, CP2, GL1, FN2, IP2, IP3, TR1 (~5.3M tokens together); Sonnet: the item ranking
  (109k), DOC (the traps / §3 / §4 pass, 104k).
- **HEAD against the baseline:** gained 735 (338 shared) + 18 tie-break, 0 lost outright, 0 moves; 6 addresses lost a shared holder, each still held by another file (D66 (i)): 0x6CF410 (fontsys / unknown_6cce60; fontmanager, assetmgr, dazzle, rendobj hold it), 0x702240 (sclient; aux_roots), 0x585040 (skill_data, ui_scan; ui_cpit), 0x546710 (ui_mfd; ui_cpit, ui_manu, ui_scan), 0x818E90 (ui_registration; skillnam), 0x5469C0 (ui_scan; co, ui_analyze, ui_attrb, ui_starmap). One accepted loss in accepted-corrections.json (PR #181 to master,
  cherry-picked onto the run): 0x604150 (D66 (vi), the coordinator: ui_registration's ~_com_ptr_t<IXMLDOMDocument> is
  byte-identical to the client's own 0x7E1CA0 / 0x7E1CF0, now emitted by unknown_7dfed0). #181 merges before #180. The PR needs the maintainer's /accept-d66 for the six shared-holder rows (D202).
- **The ranking (the user's two criteria):** a scan of client.exe's direct call sites over the 3,727 app functions not in
  the tree (712 KB) ranked them by size x callers and grouped them by client file; §4 gave the unlock leads (.work/log/a5.log,
  a6.log). Tonight took the top of both lists.
- **The inline-budget rule (IB1, then IB2 / CP2 applied it):** each caller has its own budget, raised by its own non-inline
  code; a callee costs its source size (dead code included); top-level sites spend it in source order; inline calls after a
  site take that site's nested budget. Two levers reproduce the client's out-of-line finds (two inline layers with the
  NameClass in the inner one; one inline call after an inlined helper); Casts need the root by `char *const &`. Lab and
  census in `.work/ib1/`, IB2's variant runner `.work/ib2/vr.py`; workflow.md §3.1 has the rule. Exact by it: ui_analyze
  0x545E40, ui_manu 0x5B4680, skill_data 0x6A4C20, ui_cpit 0x583790, find / Cast copies in ui_buff, ui_scan, ui_mfd,
  ui_manu, ui_cpit; P-IB2-1 (ui_scan 0x60E6A0) carried out once ui_cpit held 0x585040; D130 dead spenders in 0x7C5690 /
  0x54A640 replaced by the lever.
- **New files:** client_item_property_builder.cpp (IP1, the client's `__FILE__`; 0x522860, the biggest unbuilt function, 17.8 KB,
  written functional with its 37 asserts in order; the property types and attribute ids are in the plan log);
  unknown_7cef30.cpp (XR1, provisional: the 'subs-interface' account-server client, 21 methodCall requests, +129);
  unknown_7dfed0.cpp (XR2, provisional: the registration reader over MSXML's DOMDocument, all 18 slots exact; msxml3.tlh /
  .tli cut to the DOM core, P-XR2-1) and unknown_7e1e40.cpp (the reply download over URL monikers, 23 of 24; build.py links
  URLMON.LIB); ui_jobs.cpp (JB1, `__FILE__`, 29 of 30); ui_log.cpp and ui_guild.cpp (TM1, provisional: 'UI_LOG' / 'UI_GUILD';
  the brief's "ui_login rest" and "time_mgr tail" were neither); ui_guild_list.cpp (GL1, provisional, 15 of 16).
- **Unlocked:** 0x752170 pairs again (SC2: UnknownB016DCType's copy constructor, MountBoneNames as DeltaItemArrayClass
  <DeltaStringClass>: 0x751BE0, 0x6F8880 and five more; aux_roots repadded SYMBOL_PAD 58, SEND_SYMBOL_PAD 40, auxroots.py
  DEF_ORDER); 0x42C330 and 0x7D3CC0 (XR1); the Sub14C stand-ins gone (XR2); FontSys's " " literal (FN1: 0x6D6390 exact and
  Change_Text's five sites, a functional fix for the POC), Play by value (+14 in unknown_fontsound).
- **Late items (03:00-05:30):** GL1: ui_guild_list.cpp 16 of 16 and ui_guild's 0x81C060 / 0x81C1F0 / 0x81CB90 (an Active
  test through the inline Is_Active() fixed three register swaps); IP2: the property files 0x529810-0x52B38F (icon by its
  `__FILE__`, its +0x08 a RefCountPtr<TextureClass>; option, name, description, auto fire), every own function exact, +94;
  IP3: the property base ClientItemPropertyClass and the attribute base with its six subclasses 0x51F900-0x521B1F, all exact
  or tie-break, +86 with shared; FN2: 0x6CD180 exact and a real bug fixed (FontSys's own Bits +0x2BC shadowed
  RenderObjClass::Bits in 0x6D60C0: the POC wrote the wrong field); TR1: ui_trade.cpp (`__FILE__`), 9 of 14, its base
  Unknown59F3D0Class = ui_inv.cpp's base (interim, merge with PDAInventoryWindowClass).
- **Burn rate:** 78 % -> 86 % over 9.3 h with 1-5 Opus builders (~0.8 %/h all in; ~2 %/h for the first hour with five
  builders and the ranking, ~0.5 %/h with two). The handoff drafts and §4 / traps pass went to one Sonnet agent.
- **Harness (D182):** at 95917583 (everything landed): clienttest 103 checks, 0 failed, trace equal to expected.txt (2,653 lines); udpreplay alex3 exit 0; handshakes 54 / 54 (also at 89a50e0c and a245e9a2).
- **Traps filed** (workflow.md §3.1-§3.8): the inline-budget rule; ctor / dtor and EH-state forms; argument and loop forms;
  two objects when one class inlines what another calls; large #import headers; gen_stubs and by-value std::string returns;
  pad windows (appfile vs the importer path; a header member for one caller moves sclient's window); appfile's overload and
  marker quirks; MSYS `//` arguments.

### 2026-11-08 (20:25 PST, evening, 2026-11-08c): D206 carried out; event.cpp and unknown_78a910.cpp whole, optdlg.cpp written, ui_attrb's attributes panel, Player's aux tree in sclient.cpp (D217) with clienttest's effect database, P-H13-2; section 4 audited and refiled; the real burn rate; +320 pairs and 10 tie-break over the run's baseline, 0 lost outright

- **Run** run/2026-11-08c (PR #178), coordinator Opus 5.5 on the subscription, base 3c0af6d0 (#176 merged), baseline
  `.work/imports.3c0af6d0.json`, plan `.work/run-2026-11-08c/plan.md`. The user: weekly usage to ~80 % by 20:00 (a
  target), agy may spend its whole Gemini 5-hour window, no API-key builders; land the items already in §4 and prune its
  stale ones. Weekly usage 73 % -> 78 %; agy's Gemini 5h window 100 % -> 9 % remaining. Builders: Opus K1, C1, W1,
  C2, K2, AT3, W2, SC1, OD1, G3F, LV1, OD2, AT4, LV2, HF1, AN2, SB3, PH1, WF1, VM1, OD3 (~4.0M tokens together); Flash (agy) G1-G8 (~9.4M stream
  tokens);
  Sonnet S4A (the §4 audit), RF1 (the refile), PF1 (pacing.py), SF1 (SC1's clienttest stand-ins), BQ1 (the
  progress chart's step).
- **HEAD against the baseline:** gained 320 + 10 tie-break (137 shared), 0 lost outright; 82 moves (SC1's aux functions aux_roots ->
  sclient, D66 (v)); 18 shared holders less (each still held by other files: D66 (i) / (v)). Two accepted losses, recorded in accepted-corrections.json (PR #179 to
  master, cherry-picked onto the run so bring-over accepts them): 0x752170 (D217, P-SC1-1, the user: a byte-identical
  copy constructor aux_roots held only by the range tie-break; it returns when sclient's 0x751BE0 is exact) and 0x5495A0
  (D66 (vi), the coordinator: two byte-identical static initialisers in the client's ui_attrb object, ours now credited
  to 0x548190). #179 merges before #178.
- **The burn rate (the user's question):** the skill's 2 % of the week per Opus builder-hour came from one run on
  2026-10-27; 22 runs' heartbeat lines (2026-10-30..11-08b) give 0.47 % per builder-hour with the coordinator included
  (`scripts/burnrate.py`; a fit gives the coordinator ~0.6 %/h, each builder ~0.1 %/h, loosely). pacing.py's default is now
  0.5 (29164c62); its projection read 0 % whenever the usage reply had only the weekly window (fixed, 5b0e11bf). This
  run: ~1 % per hour with 4-7 Opus builders.
- **§4 (the user):** S4A's audit found ~26 entries already done by 11-07 / 11-08 commits that no log entry named; removed
  after checking each in the ledger's in_tree lists (a02949ef): 0x55A1F0, 0x590DB0, 0x591A30, 0x4C7D80, 0x4CBCD0,
  0x442650, 0x4BCD30, 0x66FD80, 0x575D50, 0x77D7F0, 0x77D9F0, 0x495990, 0x5B9410, 0x814430 / 0x814620 / 0x814A30 /
  0x815720, 0x693230, TE1's 0x673EE0 / 0x84BCB0 / 0x84BDF0 / 0x84BE40 / 0x84C060, ro_funcs 0x495FD0-0x496620,
  starmap_pip's 0x7C3A90. Absence from not-exact.md is not proof (CH2's six had no marker and weren't paired). The 19
  dated per-run bullets above the numbered categories were refiled into them (b8d040d1, every address / D / P / file
  token kept), and the coordinator-pacing skill now says to file leads under their category.
- **Decisions carried out:** D206 (K1, 9022414b): DeltaChildVisitorClass -> KVEncoderClass with the client's pure slots
  (vtable 0xAEB460; the bodies are KVFilterEncoderClass's, 0xAEB6B8), aux_roots repadded (52, the only pad with 6 / 6),
  +1. CM2 was already done by D204.
- **New and finished files:** event.cpp (G3 drafted on agy, G3F finished: Listener / Subject 0x4E81B0-0x4E8E40 all exact,
  +24 with 8 tie-breaks); unknown_78a910.cpp's ctor / dtor / 0x78ACE0-0x78B660 all exact (OD1, +104); optdlg.cpp new
  (OD1 / OD2, the client's `__FILE__`): 0x846B80, 0x846CD0, 0x846D10, 0x8472E0, 0x847CD0 exact; 0x846FC0 (register),
  0x847630 (shape, 30 forms: its first test holds 0x847BB0 / 0x847C40 too), 0x8476C0 mapped, not written. WinMain's
  stand-ins 0x846C90 / 0x847D20 untouched (D69). ui_attrb's attributes panel Unknown546A80Class (AT3: 16 of 18 exact,
  +25; ui_attrb_skills.cpp on ui_attrb.h, AT4).
- **Near misses closed:** chat window split set-up 0x5723B0 rebuilt (register, 2 rows; POC stand-in gone) and 0x572EE0
  exact (W1); chat_window / _history 0x6821F0, 0x67E670, 0x6804E0, 0x6812C0 exact and FontSys::Font a
  RefCountPtr<Font32Class> (W2, +9); ui_starbase 0x63BDB0 (G4), 0x63C080 / 0x63D2F0 (G7), 0x646A00 (LV1); gd_buffgadg
  0x665940 / 0x6656A0 (LV1); ui_avatr 0x5582A0 (G5); 0x563260 (K2), 6 D99 rows (G1), 0x6F36F0 D99 (K2).
- **Evening addendum (the user: fix the effect database and its harness this session; 20:00 was the stop time, soft):**
  SC1 (D217) landed (df07a240): Player's aux tree compiled in sclient.cpp (D137 interim, auxroots.py
  SEND_HEADER_ROOTS) makes Player's dtor 0x7551D0 and RPGInfo's 0x753C60 exact, the testeffects loader 0x74CA90
  functional. Its loader links eff_o.obj into clienttest: SF1 dropped the colliding stand-ins, HF1 (098acdb9) gave
  clienttest an effect.ini fixture (EA's shipped EFFECT sections for alex3's 11 descs and their links), eff_ base
  assets in basset.ini, stand-ins for RObjEffect / WarpEffect / LineEffect on their real ends and the fake view's
  scene / camera in every section: trace equal again (expected.txt: 109 diff lines, each a removed stand-in line or
  the real loader / constructor lines). PH1 carried out P-H13-2 (UnknownCOClass is ClientObjectClass::
  Unknown_Create_Aux_Root 0x6F6000 in co.h) and P-H11-1's rest (sclient.cpp on control_keys_manager.h) with SB3's
  starbase work in one pad round: SYMBOL_PAD 16, SEND_SYMBOL_PAD 51 (window 41..61). WF1: FontSys's destructor
  implicit now holds with nothing lost (+4: SC1's declarations kept 0x74D990), so P-W2-1 closed without a loss.
  SB3: UIStarbaseClass's dtor 0x633B50, UIStarbaseUnknownCCClass's 0x632E50, money's operator>> 0x4F0780 exact (+6).
  AN2 / VM1: ui_analyze's six slots functional, the client's shared Mode / Validity types
  (DeltaTemplateClass<UnknownB013ECEnum> / <UnknownB01394Enum>, 0x6FA6F0) for ui_analyze and ui_manu, 0x546090
  exact (+6). OD3: optdlg's 0x8476C0 / 0x847BB0 / 0x847C40 functional with their tables. G8: gutter_radar 0x7B71D0.
- **Stand-ins and headers (0 / 0):** C1: TR1's skill_render / inv_render, LineDisplayWrapClass -> FontSplitStringClass,
  fontmanager.h in nine views, FontSys ctors take RenderObjClass *; C2: unknown_4be9d0.h, P-MU1-2, P-H11-1 in part, rs.h
  and rs_proj_sup.h, ui_popupmsg.h, ui_cpit on ui_scan.h; K2: aux_roots' Write / deleting pairs swapped by the client's
  vtables (+1), the root object from 0x4A3B40, srctree.py files asserts by the nearest `__FILE__` (249 rows),
  Unknown10BC unsigned. Not held: P-H13-2 and P-H11-1's rest (lose aux_roots ctors and sclient 0x74D990; one quiet-queue
  pad round), the strconv move (no pad 6 / 6: left where they are, the coordinator's call), P-H9-3 (its text is lost).
- **The progress history** is rebuilt at this handoff (`progress.py --history --rebuild`, the user): #176's 1.69-point
  step in bytes rebuilt was symbols.json's 4,145 new function lengths ending the old over-charge, not lost work.
- **Throughput:** the importer lock is global and not FIFO: with 6-9 builders verifies waited 20-50 minutes, three agy
  builders timed out in the queue (G3 / G4 / G5; their work was saved: G3F finished event.cpp, G4 / G5's commits landed
  and their unverified edits are patches in the run folder), and two batch landings gave up before `--lock-wait 6000`.
  More than ~5 builders adds queue time, not landings.
- **Harness (D182):** at 64cfeed3 (everything landed): the POC runs its videos and exits 0; clienttest 103 checks, 0 failed,
  trace equal to expected.txt (2,653 lines, HF1's effect fixture); udpreplay alex3 exit 0; handshakes 54 / 54.
- **Traps filed** (workflow.md §3.1, §3.3, §3.4, §3.6): dead size() reads and default-construct-then-assign; VC6's loop
  rewrites; inline copies in definition order and the explicit empty destructor's vptr stores; sclient 0x74D990's pad
  sensitivity; the lock (`--lock-wait 6000`, ENB_LOCK_WAIT for auxpad), accepted losses on the target branch, shared
  verify scratch, renames alone changing pairs; appfile.py's marker, stdin, include and line-count quirks.

### 2026-11-08 (13:35 PST, day, 2026-11-08b): P-IR3-1, P-CW1-1..3 carried out; a second Batch API round (b1108) applied; ChatPanelUIBase; symbols.json regenerated with two tools fixes; +247 pairs over the run's baseline, 0 lost outright

- **Run** run/2026-11-08b (PR #176), coordinator Opus 5.5 on the subscription, base 05a28def (#175 merged), baseline
  `.work/imports.05a28def.json`, plan `.work/run-2026-11-08b/plan.md`. The user (09:00): P-IR3-1 as recommended (V7d +
  (b)), P-CW1-1, P-CW1-2, P-CW1-3 approved (D212-D215); P-SB1-1 is the coordinator's call (D137: kept as proposed, the +0xD4
  stand-in stays until 0x633B50); P-CW2-1 not decided; then a single Batch API round on the $67 left (D216). Weekly usage
  71 % -> 73 %. Builders, all Opus: OM1, CW4, RT1, RN1, VB1, VB2, VB3, SY1 (~1.7M tokens together).
- **HEAD against the baseline:** gained 243 + 4 tie-break, 0 lost outright; 6 D66 moves (0x68FB60 / 0x68FC50 / 0x68FC80
  inv_render -> inv_trade_render, D209 (b); 0x4A8600 delta_time, 0x4F7700 / 0x4F7750 shipcust_desc -> aux_roots, with
  the symbols fixes); 6 shared holders less (0x68FC30 inv_render; 0x4B5740 chat_window_history; 0x42A970 / 0x42D050
  ui_chat_panel_base; 0x4B2AA0 kv_encoder; 0x703F40 sclient: each still held by other files); 0x68FAE0's loss accepted
  (D209 / D212: accepted-corrections.json, landed first as PR #177 because verify_pr.py reads the list from master). The
  merge needs the maintainer's `/accept-d66` on the head.
- **objmatch (OM1):** V7d (fb07b848, +35: app 28, ww3d2 4, wnet / wwaudio / wwmath 1; none of the unnarrowed rule's false
  gains came back); P-CW1-1 as two rules: the strongest of several range proposers (9c6cac86, +9, decided by the self-name
  test) and the refutation of a proposer whose call into its own object lands on other bytes (f69ae7e4, 0 / 0); with them
  CW1's ui_chat_panel SignalMap / MemberCallback retype holds (ed7d12da, +18). Switches `USE_XCU`, `USE_RANGE_EVIDENCE`.
- **Chat (CW4, RN1, SY1):** SClient 0x74D990 exact (74ed6384); clienttest traces the chat lines at its call of 0x563760
  through a `#define` seam in sclient's wrapper (a2d280d1; expected.txt's 55 chat lines now carry the client's prefixes,
  NOTE_CONTENT 184 counts 10); both stand-ins of 0x74D990 gone. 0x744E70 exact, 0x42C4E0 gained. Renames on client
  evidence, pairs unchanged: ChatPanelInterface (af0f630d; UIChatIntClass was already ChatWindowInterface) and
  ChatPanelUIBase (706e01e8: its MemberCallback type descriptor 0xB7D5E8 <- 0x56DF50 <- thunk 0x40535D <- table 0xAF0174,
  stored by our destructor 0x56D330). File names stay (the client's own `__FILE__`).
- **D185's leftovers (RT1):** the 11-01 `try` dropped blank lines inside a variant, so a right variant failed on its
  asserts' `__LINE__`; `variants.py try --fit` aligns them. Of 20 functions whose best variant lost pairs: 113 losses in
  15 were that line shift, 5 in 4 an out-of-line helper copy (0x44E470 / 0x450D60, 0x500760 / 0x42DB80, 0x584690 /
  0x47E4D0 + 0x73F950, 0x5B1ED0 / 0x5B7260), 1 placement (0x5B5320 / 0x5B70C0). +10 (0x44BC90, 0x712780, 0x738970,
  0x7A5400 exact; 0x726100's 'Null Item' gains three string copies). 33 only-closer variants are attempts rows with stashes.
  New: `try --fit / --answers / --keep-losers`, `pick --exclude-sent RUN --add FILE`, `build --clues FILE`.
- **Batch b1108 (D216):** `pick --all --exclude-sent full --add .work/rt1/rerun.json` 404 (30 re-runs with clues), 91
  skipped at build, 313 requests, **$35.76** (expected $31.31, ceiling $64.77); `try --fit`: 50 of 296 exact (48 losing
  nothing), 23 more gaining a pair. VB1 +37, VB2 +28, VB3 +39: about 60 functions exact. Not applied (attempts rows):
  invented helpers / forwarders / inline asm (0x7A6DD0, 0x546090, 0x583DC0, 0x6CE4E0, 0x455590 (a wrong pick:
  Vector3() at that address), 0x576AC0, 0x561030), gains that drop the function's own similarity by more than 0.1
  (0x7AD090, 0x75B8F0, 0x75FB00, 0x534CA0, 0x5C0350, 0x80CA30), and 0x5913E0 (its gain lost 0x592350 in the full verify).
  API credit left: about $31 (to 11-28).
- **symbols.json (SY1, P-RN1-1; 62f3a30b, tools):** the first regeneration (4cad4d7a, kept in history) lost 0x5156B0
  (InterfaceClass::Unknown_64, ~40 files) and 0x466BA0 outright in the landing's verify: SY1 had verified only before
  regenerating. Causes: symbols.py placed tables by slot alignment where an exact function's vptr store says otherwise
  (StarmapPipSupportClass at 0xAE8754, not 0xB06D60; also DeltaTemplateClass<B00A00>, DeltaCompoundClass, LightClass,
  DynamicMeshClass, DynamicWindowRenderObjectClass); objmatch's short_known took ChatPanelInterface's own empty +0x64
  (0x563260) as a second address of ?Unknown_64@InterfaceClass; and a symbol at several client functions (two classes of
  one name; linker-folded addresses, D193) got a self name. Fixes: symbols.py places a table by the vptr stores and gives
  no `proof` at such symbols; objmatch skips a vtable slot whose symbol is known at another address and keeps self names
  with one address only (as its docstring said). symbols.json 26,931 -> 36,217; all 11 libraries verified after it:
  +56, 0 lost outright (3 moves, 2 shared holders less, above). `.work/pseudo` refreshed (`--all`, `--check` 0).
- **Harness (D182):** at cffba3c0, 32893875 and 62f3a30b: clienttest 103 / 103 equal to expected.txt (2658 lines); udpreplay
  alex3 26336 of 26465, 7 of 8; handshakes 54. The POC was not replayed this run (P-OM1-1: the chat panel now registers
  its handler 0x56C730 for real); CW4's replay crashes after world entry in ViewClass::Unknown_14+0x44 (0x88), as the
  baseline exe does (§4).
- **Records:** traps in workflow.md §3.3, §3.4, §3.6, §3.7, §3.8. The earlier plan-log times before 11:42 were estimates.
- **Waiting on the user:** `/accept-d66` on PR #176's head (6 moves, 6 shared holders less); the merge. P-CW2-1 (0x56FD30 0.545 -> 0.606 in b1108, not
  exact).
- **Next session:** §4: SY1's retype lead (+0x9C MovableUIBackGadget*, 0x56DEE0's MovableUIBackGadgetMoved), the POC's
  world-entry crash, then the batch's leads (0x510D40 frame, 0x5913E0, 0x561030).

### 2026-11-08 (06:45 PST, night): the POC's PDA, chat window, PDA star map and the scanner's target lines on real code; rs_hanim and the 'link' render state exact; +298 pairs over the run's baseline, 0 lost outright

- **Run** run/2026-11-08 (PR #175), the user's goals: weekly usage 75% by 07:00 (66% at the start, 71% at the end) and the agy 5-hour window spent in full (it opened at 01:06 with the first launches and was at 17% remaining by 02:45; A1-A8 spent it: ~9.4M Flash stream tokens). Plan log `.work/run-2026-11-08/plan.md`. Claude builders (Opus): PD1, CW1, TL1, RH1, CW2, RH2, PD2, TL2, SB1, RH3, PD3, TL3; agy (Flash): A1-A8.
- **Setup fixes** (81c60f8d): `wt.py`'s junction removal passed `cmd rmdir` a forward-slash path (cleanup refused every worktree with junctions); `run_branch.py start` shadowed the repo name (crash opening the PR). **Unlanded branches** dropped, each tagged `archive/wt-<name>` locally: RS1 (landed as d0f9fee1), CC1 (superseded by 7fbc1220), HF2 (RT1's fix, landed), pins (an M4 measurement, not for master).
- **POC world entry:** pdaint.cpp PDAInterfaceClass (PD1, +42: 19 of 24 exact; the POC draws the PDA in world), ui_chat_panel's chat-line path and ui_combobox (CW1, +15; the classes' RTTI names: ChatWindowInterface, ChatPanelInterface, OptionFontInterface), ui_chat_window.cpp ChatWindowInterface (CW2, +51, 18 of 30 exact; the chat window draws both panes with the replay server's lines, a POC FontSys 0x6CE420 stand-in fills formatted text), ui_chat_window_split.cpp UIChatWindowSplitClass (CW3, +37: ctor and 9 more exact, the dtor and 0x572EE0 functional; the set-up 0x5723B0 still a POC stand-in), starmap_pip.cpp StarmapPipClass (PD2, +35, 23 of 27 exact; PDAInterfaceUnknownA0Class is StarmapPipClass), ui_scan's target lines 0x60EFE0 exact / 0x60F1A0 functional and five more exact (TL1, +19), the interrupt display 0x60E6A0 functional + setters (TL2, +5), 0x60FEE0 (TL3, +1), rs_hanim.cpp all exact + enbscene's land projectors 0x75F8B0 functional (RH1, +28), the 'link' state unknown_45a090.cpp all exact + 0x75FB00 functional (RH2, +22), UIStarbaseClass's ctor 0x6333E0 exact (SB1, +8) and its destructor 0x633B50 functional over an SGI-shaped +0xD4 table (SB2, +3: 0x633AC0, 0x6440F0, 0x644AE0), the 'sound' state unknown_45f390.cpp all exact (RH3, +27). New POC stand-ins for what the real set-ups create: poc_world3.cpp / poc_world4.cpp. starmap_pip 0x7C1350 exact, 0x7C1770 functional (PD3, +1), 0x7C3A90 exact, +0x58 ScaleMult (client name), +0x68 one Vector3 (PD4, +1).
- **agy near misses:** of ~45 parked functions retried, 3 went exact (ui_mfdm 0x5C2470, rs_list 0x45C430, galaxy_map_render 0x7A99F0); rows for the rest (0x75B8F0, 0x5DD3C0, 0x640810 now LIMIT REACHED). A6's behaviour rewrite of unknown_7979a0's four functions (0 pairs) was reverted by the coordinator (diff kept at `.work/run-2026-11-08/A6/`). Residues are mostly STL inline-budget and register / order.
- **Landing:** every landing through `wt.py bring-over` (single and batch), the link gate on each app landing (no refusals). The importer lock queued 15-25 minutes per verify with 4-5 builders. A landing outlived a 600 s shell timeout and kept running after its task was stopped (trap filed).
- **Harness (D182):** at 054e53fb (after PD1 / CW1): POC links (10 known unresolved), runs 40 s; clienttest 103 checks, 0 failed (2653 trace lines); udpreplay alex3 26106 vs 26078, reference's undelivered 0; handshakes 54. At the handoff: at ef6f30c0 (before CW3's landing): POC links (9 unresolved; link_baseline.json refreshed to 9 at the final HEAD, Unknown_765DF0 resolved), runs 40 s, exit 0; clienttest 103 checks, 0 failed (2653 trace lines); udpreplay alex3 26106 vs 26078, reference's undelivered 0; handshakes 54. CW3's landing and the final baseline refresh linked clientpoc and clienttest at the final HEAD.
- **Waiting on the user:** P-CW1-1, P-CW1-2, P-CW1-3, P-CW2-1, P-SB1-1 (§3); the merge of #175. The interim name unknown_45a090.cpp (RH2 proposes rs_link.cpp; no client evidence: D137 list).
- **Next session:** §4's POC world entry item (the chat split window's set-up 0x5723B0, the PDA's windows behind poc_world3.cpp, mainview's +0xA8 retype), then the near misses.

### 2026-11-07 (evening, 2026-11-07b): the EA freeze (D211) and the sharded ledger; pipeline-plan §3.0-§3.3 and items 1-6 landed; 0 gained, 0 lost at every step

- **Run** run/2026-11-07b (PR #174), the plan [pipeline-plan.md](pipeline-plan.md), serial work on a quiet tree: the coordinator, then one bounded builder at a time (docs, then §3.3). Baseline `.work/imports.9d9b2201.json`; every step's pairs_cmp against it: 0 gained, 0 lost.
- **§3.0:** `importer.py replay` 0 problems, 0 files differ at the base; no builder running (worktrees CC1, CC1b, RD1, RS1 left, unlanded).
- **Item 1** (8af20bf5): attempts.csv merges by line union (.gitattributes); docs_check.py flags a (date, address) twice (17 earlier same-day pairs allowed by name); `attempts.py record` refuses one.
- **§3.1 final refine pass** (evidence refreshed first, 7ec023ce: wwlib explains 725 client functions, up from 553; a run without `--pool` compiles nothing, all VMs being off): refine in a throwaway worktree proposed 3 splices, all ww3d2 (BoxRenderObjClass::Set_Color, DazzleRenderObjClass::Clone, SegLineRendererClass::Set_Texture from R); their text equals the tree's and a full ww3d2 verify gave 0 gained: single-file artifacts (0x48F6F0 claimed by two files). Kept nothing; the other six WW libraries proposed nothing; wlib, wnet, binkmovie have no evidence. Reported to the user, who gave the go-ahead.
- **§3.2 the freeze (D211):** 952f432f, one end-of-file block in each of 202 files, 1,012 `// EnB:` entries (.work/freeze/annotate.py: patches located by replaying each file's patches and mapping to the final text, since 44 had their text changed by later edits; 32 EA files without a final newline got one; one dead PATCHES entry on the RECONSTRUCTED unknown_distortion.cpp skipped); banner note reworded in 178 files; full verify of all 11 libraries 0 / 0, imports.json byte-identical. 323f258d deleted the machinery (also trial.py, trial_all.py, refresh_expected.py, mkspec.py); 9eda2a30 the docs (CLAUDE.md, conventions.md, workflow.md fix loop and traps, library-matching.md §7, READMEs).
- **§3.3** (builder): 7cf943ae importer.py split into libdefs.py, vc6lock.py, ledger.py, verify.py, report.py, importer.py a forwarding shim; 1ed1517c docs/data/imports.json becomes docs/data/imports/<lib>/ (1,714 shards; RECONSTRUCTED's 976 descriptions in them; `ledger.py export` equals the old file, stored counts equal the computed ones), consumers on `ledger.load`, full verify 0 / 0 with every shard byte-identical after it; c7ca0c35 docs and `ledger.py describe`. 69 tool tests pass.
- **Verify nondeterminism (trap filed, workflow.md §3.0):** one full verify credited 10 wwaudio/sound3dhandle pairs (ListenerHandleClass 0xA029B0-0xA02C00) with no text change; a library and a full rerun gave the baseline again.
- **Harness at the handoff (D182):** at c7ca0c35 (tools/agy/poc_check.sh, CHECK_HARNESS=1; the next commit is this log only): the POC links (10 known unresolved) and runs 40 s (the sizzle movie plays, shots at 60 s / 120 s); clienttest 103 checks, 0 failed (2654 trace lines); udpreplay alex3 26106 messages vs 26078, 0 of the reference's undelivered; handshakes 54. A first try was stopped by Claude Code for low host memory; its relative log path also wrote nowhere (poc_check.sh cds into its worktree: keep the default log).
- **Items 2-5** (one builder each for 2+5 and 3+4, the user's go-ahead after the D182 check): 046ebbc0 item 2 (single bring-over takes the target's side on generated files and regenerates; `wt.py rebase ID [--onto B] [--continue]`; rebase_gen.sh calls it); ef2a99fe item 5 (`wt.py remove` refuses unlanded commits without --force; `wt.py list --unlanded`, also in the heartbeat and the drain checklist; landed = on master / a run branch, the same author, author time and subject, or patch-equivalent; 4 unlanded today: CC1, HF2, RS1, pins); cbbe5b33 item 3 (a landing replays onto run-branch commits that touch none of its paths and no generated file, else refuses naming them); 89922f6d item 4 (`wt.py new C --base wt/P` records the parent; bring-over refuses C before P; `list` marks "parent landed: rebase"; `rebase C` replays only C's commits). 88 tool tests pass; the regeneration in a real rebase / landing has run only with the verify stubbed.
- **Item 6** (69c900c0, 520de67a): bring-over's pre-landing gate. (a) `tools/vc6/standins.py check [--rev A..B]`: a new app definition (or new pair) that a hand stand-in (poc_standins.cpp, poc_world*.cpp, poc_gadget_*.cpp, app_stubs.cpp, client_stubs.cpp) still defines refuses; over the 234 commits of PRs #168, #170-#173 it refused 5, 4 real (later fixed by hand) and 1 false (`Unknown_731FA0`, client_lego_root.cpp vs client_stubs.cpp:82; `// standins: keep` exempts one). (b) `tools/vc6/linkgate.py check`: links clientpoc and clienttest in the scratch worktree when app/, stubs/, programs/clientpoc/ or tests/clienttest/ changed; refuses on LNK2005 or an unresolved symbol not in `tools/vc6/link_baseline.json` (the POC's 10; refresh with `--write-baseline` when the set changes on purpose); `--no-link` skips it. Adds 2-5 min to an app landing. 107 tool tests pass.
- **.work/pseudo emptied, rebuilt:** this run's first `run_branch.py cleanup` (18:27) removed old worktrees with plain `git worktree remove`, which deletes .work/pseudo through the junction; cleanup now uses wt.py's drop_worktree (ef2a99fe). Rebuilt with `pseudocode.py --all`: 29,804 of 29,817 starts, `--check` clean.
- **Hosted verify on #174:** marking it ready started the service with master's tools; it crashed twice on the missing imports.json (no verdict posted), after the coordinator had released its two VM leases by mistake (`importer-<pid>` with no holder pid: check the pid before releasing). #174 is back in draft.
- **Merged:** PR #174 as ddba2306 (2026-11-08 00:38 PST) by the user. The hosted verify could not judge it (master's tools read docs/data/imports.json; it crashed four times, posting nothing), so the user ran `verify_pr.py 174 --pool 6 --approved --no-post` from the run branch: success, +0 pairs, 0 lost, NOT EXACT 624 -> 624, all 11 libraries, test merge 7136ea102; posted `vc6-verify` by hand with `gh api .../statuses/<head>` (master's ruleset requires `checks` and `vc6-verify`) and merged. CI first failed: six item-4 tests called `wt.py new`, whose junctions use `cmd /c mklink`, absent on CI's Linux; a3ae8405 stubs `furnish`. The verifier checkout moved to ddba2306 at the next scheduled launch. D182 rerun at 68450ecc: as before.
- **Waiting on the user:** two coordinator choices to confirm: libs_touched still picks libraries from changed importer.py / libdefs.py lines (flag changes need a verify); library fixes no longer add a library-matching.md §3.6.1 row (the `// EnB:` blocks replace it). Four unlanded branches (`wt.py list --unlanded`): CC1, HF2, RS1, pins: land or drop.
- **Next session:** §4's POC world entry item and the near misses, with the new landing tools (the first real `wt.py rebase`, regenerating bring-over and link gate on real data).

### 2026-11-07 (14:05 PST, day): D203-D210 carried out; the POC's world view past the loading screen, the player's ship drawn as retail's (50 draws on 18 transforms), the radar panel, the orbitals, the real scene and lights; +338 pairs over the run's baseline, 0 lost outright beyond D203's three

- **Run** run/2026-11-07 (PR #173), the user's goals: weekly usage 66% by 16:00 (60% at the start, 65% at the end), and at 10:25 the reset agy window spent in full (it ran out at 12:35: ~9.8M Flash tokens over eight items, about 1.0-1.2% per 100k). Plan log `.work/run-2026-11-07/plan.md`. Claude builders: IR3, GR3, GD2, GD3, CM2, WE2-WE9, TG3, NM1, GN1, GD4, HF1, HF2, AF1, RT1; agy (Flash): A1-A8.
- **Decisions:** D203 (coordinator: attempts.csv rows, a workflow.md 3.0 rule, the three in accepted-corrections.json when a batch landing dropped them); D204 / D205 (CM2: the six CameraManagerClass stand-ins in camera_manager.h, sclient.cpp on CameraModuleClass::Get_Signal_Map, 0 / 0 each); D207 (GR3, clientpoc.py's self-exit); D208 (GD3: UIBackGadget, unknown_674260.h folded into gd_uiback.h); D209 measured (IR3) and investigated at the user's request (NM1): P-IR3-1; P-GD3-1 investigated (GN1: two classes, 0x60 / 0x68 bytes) and decided as D210 (GD4: BoundedUIBackGadget). D206 waits for aux_roots' repad.
- **POC world entry (the user's item, SClient::Unknown_750B60):** 0x750B60 is the incapacitated test, not the loading gate: START drops the loading view as retail does; the "map on top" was the login's one-off splash left in the back buffer (space mode clears no colour) over an empty backdrop scene (WE2: backint.cpp, BackInterfaceClass, puts SERVER_PARAMETERS' backdrop in). Then: the chase camera 0x79BFC0 and modules 0x79C4A0 / 0x79C8E0 (WE3, the ship behind the camera; a gen Pre_Render answering false had hidden every hooked model); orbitalmanager.cpp (WE4: the Sun and the Galilean moons draw; Jupiter itself needs a CREATE + PLANET_POSITIONAL_UPDATE that the replay never sends: antprox coverage); co.h's AccelClass (WE5: an uninitialised stand-in rolled the ship 188 deg; the camera matched retail to 3 decimals), enbscene.cpp's planet day / night, the render hook 0x6BB7E0; the lego factory 0x7294D0 and LegoSplitStruct (WE6, the ship's parts); enbscene.cpp's MainViewSceneClass on ProjSceneClass (WE7: the POC light gone, the ship lit by mainview's +0x144 lights from OrbitalManager); client_lego_root.cpp (WE8: the ship's 50 draws on 18 transforms, group for group as retail, wings included; Init_Bone_Maps / Shutdown_Bone_Maps by name). Shots: WE2-090018, WE3-093842, WE5P-111307, WE7-122821, WE8-124356 (.work/poc/shots, or the builders' worktrees' .work/poc/shots). Sector draws: retail ~170, POC ~60 at WE7: the planet (data), the radar, target box, left bars and chat window (stand-ins).
- **Other app work:** TG3's five triage runs (money.cpp's operators, ui_popup_browser.cpp, circbuff's five, ui_starbase's lights, unknown_78a910.cpp the starbase avatar, PROVISIONAL), agy A1 (unknown_root policy helpers +10), A2 (bridgesounds.cpp +3), A4 (rs_eff_p.cpp +33), A5 (the camctrl stubs of states 3, 7, 9, functional), A7 (ui_popup_browser's dtors +4), A6 + AF1 (money.cpp's rest +41; MoneyClass's member declarations behind `__MONEY_MEMBERS`: with them visible aux_roots' pad lost 8). A3 and A8 landed nothing (A3: COMDAT copies tied to non-exact callers; A8 stopped on the agy 429).
- **Landing:** generated-file conflicts on almost every landing (imports.json); branches built on another builder's unlanded branch carried its commits again (rebase `--onto run <old tip>` the own commits only); two refusals by the coordinator's slips (a progress.md commit during a landing; a hand rebase taking the run's side of imports.json without regenerating). The importer lock queued 5-15 minutes per verify with 5-7 builders. Health: the POC link broke twice on landed code (MoneyClass stand-ins in three places, HF1; poc_world.cpp's Unknown458C10Class signature after rs_track.h, HF2) and clienttest once (shiputils / ui_talkt compiled twice, HF2).
- **Harness at the handoff (D182):** at 13d234bc (tools/agy/poc_check.sh, CHECK_HARNESS=1): the POC links (10 known unresolved) and runs 40 s; clienttest 103 checks, 0 failed (2654 trace lines); udpreplay alex3 26336 of 26465 first sendings, 7 of 8 sets; handshake 54. poc_check.sh now leases its VM before clienttest (it had ssh'ed to a stale IP).
- **Late results:** WE9 (ui_scan.cpp UIScanClass, the radar panel TSI08A: 4 exact, 5 NOT EXACT; +43 with D203's three back; POC draws 92 -> 114, shot WE9-133023 frame_320000); RT1 (co.h's +0xB4 is RefCountPtr<ClientLegoRootClass>, 0 / 0, and the stale Unknown458C10Class stand-ins that broke the POC compile and clienttest); HF2 duplicated RT1's fix, not landed (wt/HF2 kept). App 12,373 -> 12,544 in tree before A7, which landed last (8f4468ad), after the harness check.
- **Waiting on the user:** P-IR3-1 (V7d: +82 / 0 lost; recommend adopting it), the move of 0x68FB60 / 0x68FC50 / 0x68FC80 (P-IR3-1 (b)); `/accept-d66` on PR #173 for D203's three if the hosted check lists them; the merge.
- **Next session:** §4's POC world entry item (the HUD stand-ins, the planet's data), then the near misses.

### 2026-11-07 (06:30 PST, night): the POC client enters the world; galaxy_map_render / _robj, the camera (manager, modules, ten controllers), rs_list, ui_mesh, kv_encoder and neighbours, skillnam, time_mgr; app 12,373 in tree, +1,728 pairs over the run's baseline (+493 at the evening's close), 0 lost outright

- **Run** run/2026-11-06 (PR #172), continued from the evening; goal the user's: weekly usage at 56%, raised to 60% at 23:35 ("to help with" world entry), by 07:00 (54% at 23:35, 60% at 04:32). Plan log: `.work/run-2026-11-06c/plan.md`. Builders tonight: KV1, RD1, WE1, SN1, GM3, KV2, CM1, RS1, UM1, GM4, UP1, CS1, CC1 (all Opus).
- **POC world entry (WE1, the user's interest; memory replay-world-login):** login, Karen, Enter, GLOBAL_TICKET, the real MASTER_JOIN and LOGIN packets, the redirect, the sector connection, START / START_ACK; the main view draws space mode under the cockpit HUD (gutter, Warp, both shortcut bars) and the client stays in `SClient::Do_Frame` 90 s without a crash (shot `.work/poc/shots/WE1-010334/frame_344000.png`). The first crash was a gen null global: 0xC404D8 is `WWAudioClass::_theInstance` and the ctor's +0x19A5 its `m_Unknown19A5` (public in WWAudio.h's importer patch). The loading view's galaxy map stays on top until `SClient::Unknown_750B60` answers; no ship draws. RS1's rs_list stopped rs_track.cpp(876)'s per-frame `!primary`. The POC's link broke with UM1's ui_mesh.cpp (poc_world.cpp's UIMeshClass block): first item of §4.
- **New files, by client evidence (`__FILE__` unless PROVISIONAL):** projscen, kv_encoder, kv_acc, kv_time_adds (KV1, +162, several PROVISIONAL); radar_data, starmap_data, nav_list_builder's rest (RD1, +52); skillnam, skill_ability_names complete, time_mgr (SN1, +37); ui_chat_panel_base and WE1's world path (+15 plus functional code); galaxy_map_render 0x7A6020-0x7B2B30 (GM3, +148), galaxy_map_robj (GM4, +25); accel, delta_visitor (KV2, +15); camera_manager, cammod_chase, cammod, two controllers (CM1, +37 functions); rs_list, unknown_45d840, unknown_763e00 (RS1, +99); ui_mesh (UM1, +87); ui_proc, observer (UP1, +38 pairs); the state machine 0x799E40 and the warp controller functional, not exact (CS1); the camctrl_* controllers for states 1, 3-11 (CC1, +69 alone, +118 measured on the merge with CS1's retyped slot +0x08: states 1 and 10 fully real, 4, 5, 6, 8, 11 working with functional helpers, 3, 7, 9 still reach stubs 0x795C80, 0x794A90, 0x796440 / 0x796920). Counts are each builder's own; the measured total is in the title.
- **Landing:** builders based on another builder's unlanded branch (RS1, UM1, GM4, UP1, CS1, CC1) replayed that builder's commits on a plain rebase: landed with `.work/coord/rebase_onto.sh` (ONTO = the old tip) or as one net-diff commit on a fresh worktree (RS1b, CC1b) when their own commits conflicted. WE1 hand-edited wwaudio/WWAudio.h: moved into its importer patch. RS1 and UP1 lost nav_list_builder's 0x7B7EF0, 0x7B8ED0, 0x7B8F10 outright: 0x7B7EF0 is exact only with the exact text of the headers nav_list_builder.cpp includes (P-RS1-1); both kept those headers byte-equal and landed with 0 lost. clienttest broke three times on landed code (KV1's real Add against a stand-in, 12cdd42e; KV2's stage edit asserting the removed stand-in, 8d2a9124; CM1's real 0x79B2B0 on a NULL CameraManager, CS1's fake manager).
- **Harness at the handoff (D182):** see the last plan-log line; clienttest 103 / 103 equal to expected.txt, udpreplay alex3 26336 of 26465 first sendings, 7 of 8 sets, handshake 54 at 078ccdcf.
- **The coordinator's slips:** a `--split` claim repeated after a granted one (refused, harmless); read a two-branch landing log as both refused when CM1 had landed (corrected within minutes); an apostrophe in importer.py's patch text broke the file once (reverted before any commit).
- **Waiting on the user:** P-RS1-1, P-CM1-1, P-CM1-2, P-KV1-1, P-KV1-2 (§3); `/accept-d66` on PR #172's head if the hosted check lists the evening's D66 entries; the merge.
- **Next session:** §4's POC world entry item (the link, then 0x750B60 so the map drops and the ship can be looked for), then the near misses and leads.

### 2026-11-06 (16:55 PST, afternoon): kerneltest builds again, the data audit's first pass, 30 new app source files from the triage's leads; app 10,560 -> 11,265, +1,373 pairs, 0 lost outright

- **Run** run/2026-11-06 (PR #170, from master b9d8203b), goal the user's: weekly usage at 50% by 17:00 (42% at the start; 50% at 16:45). Plan log: `.work/run-2026-11-06b/plan.md`. Verify at 140d2c9c against the baseline: gained 1,373 (749 shared, 535 short at known addresses), 9 tie-breaks, **lost outright 0**, 5 D66 (v) moves and 21 shared-holder entries, all TE1's (below). Replay clean.
- **Harness at the handoff (D182):** clienttest 103 / 103, trace equal to expected.txt; udpreplay alex3 the same figures as this morning (26336 of 26465 first sendings, 7 of 8 sets); handshake 54, all at 140d2c9c. Link and run: kerneltest as above.
- **kerneltest (KT1, 66119f58):** builds and runs, 81 of 81 rerun by the coordinator. Two lines in taskstest.cpp; the BINKMovie and LNK2005 errors were fallout.
- **Data the pairs can't see (AU1, AU2):** a byte audit of 481 tables and 371 float constants against the client (`.work/au1/tablecmp.py`). Fixed: `MusicTypeNames` 0xBAF63C reversed after its first entry, `UIAvatarCustParts` 0xB7A324's three 15-character names, 1e-5f at 0x8ADB20 (three comparisons), 0.0001f widened at six sites. All read from the client's bytes by the coordinator; AU2's "exact double absent" check used wrong bytes, redone correctly. P-AU1-1 proposes the audit as a tool.
- **New files, placed by client evidence** (the triage hint was a lead; every file below has its __FILE__ or boundaries in its notes): galaxy_map_pip (GP1, +19), map_pip, galaxy_map_focal_point (MP1, +27), ui_mfd (MF1, +85), ui_mfd_extended_context + _harvestable + _strings, ui_mfdm (MM1, +111), verbmanager (VB1, +34), ui_verb (VB2, +84), chat_window, chat_window_history, unknown_67d430 (CH1, +135; CH2, +37), pip_scene, starmap_pip_support (PS1, +95), ui_scut_gadgets (SG1, +43), ui_slot_scut (SL1, +39), ui_starmap (ST1, +43), gd_attrb_skills (AS1, +43), gd_slot_data (GS1, +46), skill_render (SK1, +28), slot_data_render (SR1, +49), target_find (TF1, +23), ro_proxy (RP1, +142), ro_offset (RO1, +68), gd_text_editor (TE1, +70), ui_talkt_space (TS1, +28), ui_talkt (TK1, +64), stredit's rest, target_generator_summary (SE1, +17), inv_render's start (IR1, +35 verified alone). Counts are each builder's own, many at shared addresses; the total above is the measured one. XR1 put gd_attrb_skills on the real headers (+1).
- **Client facts:** VB1 and VB2 confirm 92's Drops (+0x10) / Adds (+0x20): a drop greys its button. SClient +0x12A4's class was misnamed UIStarmapClass: now TargetingClass (ST1, NAME 2), base TargetFindClass (TF1). ProxyRObj's Cast_* / Intersect_* call their own slot, not the proxied object's (kept: exact). 0xB9F7AC is target_find.cpp's, asserts.txt attributes it to starmap_pip_support.cpp.
- **The hint's boundaries were wrong more often than its file:** 0x67DB40 is not chat_window (CH1), 0x7C3F10-0x7C4211 not starmap_pip_support (PS1), 0x671300-0x672690 not gd_slot_data (GS1), 0x6A7420-0x6A7B50 not skill_render (SK1), 0x658090 / 0x6580A0 not ui_verb (VB2). `<string>`'s initialiser at each object's end (CH1) settled most.
- **Landing (workflow.md §3.3, the bullet of that name):** per-branch landings took 20-30 minutes behind the lock, so the coordinator batched: eight branches on one verify, +561. A batch lost `ui_group` 0x597080 outright where no builder had: order-only noise from a header state; LB1 swapped the compare's operands, stable across five header states. clienttest broke twice on landed code (HF1, one C2027; HF2, the real 0x64EED0 on a fake object), both fixed in tests / clienttest.py with expected.txt unchanged. `wt.py bring-over` refuses TE1's moves, so the last batches fast-forwarded after their own verify, replay and an empty `lost outright`.
- **The coordinator's slips:** removed CH1's worktree before reading a failed landing's exit (branch intact, nothing lost); overwrote an older `.work/coord/hc.sh` unread.
- **Host:** cpptools (VS Code's C++ extension) held 10.2 GB at 13:17 and memory fell to 0.8 GB free; it was gone by 13:30.
- **Waiting on the user:** `/accept-d66` on PR #170's head for TE1's 5 moves and 21 shared-holder entries, `/verify-approved` if the Python changes need it (tools/harness/clienttest.py, HF1 and HF2), then the merge. P-AU1-1, P-LAND-1 (§3).
- **Not run:** the POC, on any of this code (§4). Claims: ranges granted to builders whose work landed end with the PR.
- **After the PR (17:50, local commits on top of 0dbd837, for a PR of their own once #170 merges):** D201 and D202, the user's. `tools/re/datacmp.py` (DC1, 09921b13): first full run 739 of 932 symbols compared, 3 mismatches; it is run when wanted, not per run. Two of the mismatches were real and in this afternoon's exact ui_mfdm.cpp: 180.0 at 0xAF3BF0 where the source had 90.0, and 0.0001f widened at 0xAE7850 (c035711f, read from the client and the rebuilt object). `wt.py bring-over A B C..D` (LD1, 7dc74f7b): one verify over a batch, the D66 loss rule for batch and single forms, `--resume` after a hand fix; `rebase_gen.sh` passes `--lock-wait`; tested on throwaway branches with real verifies, `tools/test_wt.py` passes; `.work/coord/land_*.sh` are superseded. The single form keeps its 3-way merge over a dirty target (no work lost, conflicts refuse) where the batch form refuses one: LD1's call, kept. These tools have not yet run on a real run's branches.
- **Next session:** the POC link and replay over this code (§4), then §4's leads; `.work/triage/plan.md` has 33 runs left.

### 2026-11-06 (10:55 PST, morning): chartemplate.cpp whole, D198's message names, EA's final data proven and the POC shows EA's six classes; +48 pairs, 0 lost outright

- **Run** run/2026-11-06 (PR #168, from master 769dc288), builders CT1, RN1, EA1 and CARD1. Plan log: `.work/run-2026-11-06/plan.md`. Quiet-tree verify at dad1b5a6:
  app 10560 in tree, +48 vs the baseline, 0 lost outright, 1 corrected (D197, PR #169 merged), 37 moves and 8 shared-holder entries, all D198's file renames.
  Harness at dad1b5a6 (D182): clienttest 103 / 103 trace equal; udpreplay alex3 the same figures as CT1's run (26336 of 26465 first sendings, 7 of 8 sets), handshake 54.
- **Client-exact class screen (CT1, EA1):** chartemplate.cpp whole, all exact, with chartemplate.h (+38; 2c8b5e84 D197's patch, +10; 1452406b the ui_class flag tables 0xB7DC8C / 0xB7DC98 at the client's bytes; f7b6c290). The three hidden classes are the client's own bytes, NULL names and flags. Net-7's memory patch undoes them and its data adds the buttons: "retail shows nine" (my report during the run) described Net-7's data, not EA's.
- **EA's final data, without installing (EA1):** `eandb_demo.exe` (2002, Wise, unpacked by `.work/eadata/wise.py`) plus `enb-up.exe` (RTPatch through its own DLL on pool VM enb-vc6-01: 1281 patches, 0 ignored, 0 invalid) gives `.work/eadata/ea2004/`. Against EA's manifest of 2004-12-28: 1177 of 1187 files hash-exact, rechecked independently by `.work/coord/check_ea1.py`. Not proven: FA_PUB6.ini, fa_ptnews.ini, orbital4025.ini, dimple.dll (Castanet never updated them, so the 2002 bytes); fa_opt01a.ini, rs.ini, sounds.ini, mixfile_art_19.mix (same size, hash differs, unexplained); RESULTS.INI (local output); enb.exe (missing). Details in `.work/eadata/ea2004_provenance.tsv`.
- **D199, D200 (the user):** 249f8524 stays the target though it is EA's 82663b2f plus Net-7's URL edits. `C:\EnBDevelopment\ClientPOC_EA` is EA's set beside ClientPOC, used with `clientpoc.py --ea --instance NAME` (6fd9ac35). Copied from ea2004: 1015 files, 0 differing; three key files rechecked against the manifest.
- **The result, seen in the POC:** the last-built POC exe over ClientPOC_EA, antprox /REPLAY, `.work/ct1/in_class.txt`: one back-buffer shot, frame 300000 (`.work/poc/shots/EA1-105041/`), shows Enforcer, Tradesman, Sentinel / Warrior, Defender, Explorer: six buttons, the three absent, no code change. Login, character select and the class screen ran without a crash on EA's authlogin.dll; `combo_ffactory.cpp(101)`'s FAIL_IF fired x25 (x29 on ClientPOC in CT1's run, so not new). Only that path was run: the rest of EA's data, including sounds.ini and art_19, is untested.
- **D198 (RN1, 7082af70):** Message_Debug / Info / Warn / Error / Stat / Common (NAME 5), Message_Critical, G_Assert_Report, Fail_If_Report (NAME 2), g_assert.h/.cpp, message*.h/.cpp and the message classes. app 10560 = 10560, no lines moved.
- **Tools:** the progress card's caption split in two (CARD1, d41587cd). Traps filed in workflow.md §3.
- **Waiting on the user:** `/accept-d66` on PR #168's head for D198's moves, then the merge. P-EA1-1's remaining part (§3).
- **Next session:** kerneltest's build (§4, the first item below it). Game data never goes in git: ClientPOC_EA and `.work/eadata/` exist only on this machine.

### 2026-11-06 (04:30 PST, night): the expression library (10 files), the item examination window (7), ui_starbase / ui_friends / ui_ptrade / ui_ping / ui_playeropt / avatar_pip / ui_analyze / ui_attrb; +870 pairs, 0 lost

- **Run** run/2026-11-05-night (PR #167, from master ab992dba), 15 Opus builders (ST1, EX1-EX4, IE1 / IE2, FR1, PT1 / PT2, PG1,
  CP1, AP1, AN1, AT2) and 3 Sonnet (HC1-HC3) in the shared tree, one to three at a time; Claude subscription only. Items: the triage
  plan's runs naming files not in the tree (`.work/triage/plan.md`), each builder continuing into its object's free
  neighbours (claims recorded on the PR). Weekly usage 36% -> 42% (the user's goal 42% by 07:00; window closed at 41% at 02:18, reopened at 02:58 for AN1 / AT2 as
  nothing was running; 42% at 04:18);
  ~0.35% of the week per Opus builder-hour. Plan log: `.work/run-2026-11-05-night/plan.md`. Quiet-tree app verify at HEAD:
  10529 in tree, +870 vs the baseline (+8 tie-break, +360 shared), 0 lost, 0 lost outright, 0 moves.
- **The expression library (EX1-EX4, +346; app/shared/expr/):** expr_math_parser_impl.cpp, expr_math_tree.cpp,
  expr_math_parser_factory.cpp, expr_lexer_impl.cpp, expr_lexer_match_list.cpp (SGI hash_map), expr_math_lexer_factory.cpp,
  expr_lexer_matchers.cpp, expr_math_token.cpp, expr_math_float.cpp, expr_math_int.cpp, with their headers (expr_math_lexer.h /
  expr_math_tree.h real now; expr_math_eval_impl.h / expr_math_builder.h named by the client's __FILE__). 0x4C4980-0x4D42C0
  all exact but 9 (§4).
- **The item examination window (IE1 / IE2, +162):** ui_item_examination.cpp, _options, _attributes, _description, _title,
  _property_router, _specials; declaration-only ui_slot_info.h / ui_dynamicwindow.h / dynamic_window_icon.h; partial
  item/client_item_property_option / _attribute / _special.h. Row classes wait for dynamic_window_toggle and gadget 0x6686F0.
- **UI windows:** ui_starbase.cpp (ST1, +42: the starbase lights editor; the rest waits for P-ST1-1), ui_friends.cpp (FR1, +39),
  ui_ptrade.cpp (PT1 / PT2, +25 own: all exact but 0x5E2CE0; the TRADE 31 handlers landed with clienttest checking the real
  ones), ui_ping.cpp (PG1, +30, all exact), ui_playeropt.cpp (CP1, +30, ui_friends' twin, all but 0x5D9D10), avatar_pip.cpp
  (AP1, +12), ui_analyze.cpp (AN1, +15 own: ui_manu's near-copy), ui_attrb.cpp (AT2, +36: the skill panel; the
  attributes panel left).
- **Harnesses (D182):** HC1 found clienttest broken by FR1's real 0x5913E0 (LNK2005 against a stand-in): HC2 fixed it
  (97949a2f); PT2 replaced the TRADE 31 stand-ins (3490538c / 9572676a) and stubbed money.cpp's operators for clientpoc
  (84a95404: 24 -> 21 unresolved). HC3 at ef68e0c1: app 386/386 (one VM-sync flake on the first build), clienttest 103 / 103 trace equal, udpreplay alex3 ok, handshake 54/54, clientpoc 21 unresolved; AN1 / AT2 after it added no tests/ stand-in addresses.
- **Proposal for the user:** P-ST1-1 (§3: a duplicated ctype id initialiser flips 0x648380 to 0x646120 when ui_starbase.cpp's
  range moves; recommend accepting it as a D66 non-loss). Interim placements (D137, the coordinator's; listed in §4):
  ui_item_examination_attributes / _description / _specials.cpp, expr_lexer_matchers.cpp, expr_math_token.cpp,
  UIStarbaseLightsClass in ui_starbase.cpp, UIPTradeClass in sclient.h, ui_friends.h, ui_analyze.h. Traps filed in workflow.md §3.1 / §3.4 /
  §3.5 (one bullet per report). Section 4 gained one item per file with what is left.

### 2026-11-05 (21:45 PST, evening): PR #163's vc6-verify fixed: 0x6209D0 back in ui_ship_cust, D196 (objmatch's chained range) brings 0x620CA0 back; PRs #163 / #164 / #165 merged

- **Why #163 failed:** 13 lost against master: 11 D66 moves and 2 outright (ui_shipcust 0x6209D0 / 0x620CA0), false
  pairs master held while ui_ship_cust and ui_shipcust shared one map type. The coordinator had accepted them under
  D66 (iii) during the run (`--accept`); that ruling is the user's, and the verify service passes no outright loss
  (ruleset "Protect master (D148)" requires vc6-verify, no bypass; github-plan.md 4.5.5).
- **0x6209D0** (`_Tree<..., StarbaseShipCustGadgetStruct>::_Min`, called out of line from 0x61A4D0): SC3 (Opus)
  rewrote 0x61A4D0's group-0 loop (`while` with `it++` last, 4 dead size() reads; c3bd14d8), now in ui_ship_cust.
  Also 0x61A4D0 0.52 -> 0.76, ui_shipcust's twin 0x624270 0.46 -> 0.79, 0x61F250 0.34 -> 0.78, 0x61BD80 0.54 -> 0.64,
  0x619470 0.34 -> 0.49, +1 shared 0x55D8A0 (b745aeed); quiet-tree verify f59aaf04.
- **0x620CA0** (`<string>`'s guarded `std::ctype<unsigned short>::id` initialiser, one byte-identical copy per
  includer): no include was missing; objmatch's D114(b) range stopped 0x1000 short of it, after ui_ship_cust's map
  copies. D196 (the user's): the widening chains upwards through COMDATs a file alone is exact at (SC4; the two-way
  first version lost ui_shipcust 0x620CA0 / 0x628E40 on master's pre-split tree, RW1). Landed in three PRs, in order:
  #164 (accepted-corrections.json: ui_shipcust 0x620CA0 a decided loss, D196), #163 (+438, the user's `/accept-d66`
  for 12 moves), #165 (objmatch.py; full verify against a1a1d1dd: app +84, ww3d2 +7 shared, 0 lost, 0 moves;
  0x620CA0 in ui_ship_cust, ui_shipcust's own 0x62A6F0; app's tie-break by callers 433 -> 171). master ab992dba.
- Builders: SC3, SC4 (Opus), RW1 (Sonnet); claims ended with #163. The verify service once failed "compile errors?"
  on host memory (SC4's stopped verify kept two VMs leased; the service needs 6 GB free). Traps filed in workflow.md
  §3.1 / §3.4 / §3.5. Cleanup: worktrees rw1 / sc4 / corr removed, 21 merged branches deleted (wt/pins kept: a
  commit of its own; remote `claims-state` listed, not checked).

### 2026-11-05 (14:51 PST, day): avatar.cpp's out-of-line helpers explained (D195's unref.py), ui_ship_cust's ctor / dtor, the character info window's five files; +436 pairs, 0 lost outright

- **Run** run/2026-11-05-day (PR #163, from master 217fd903), 17 Opus builders (AR1-AR4, SC1 / SC2, BF1, LD7, SB2-SB4, TB1 /
  TB2, IM1, GS1, AT1) and 5 Sonnet (HC1-HC4, UR1) in the shared tree; Claude subscription only. The first two items (avatar.cpp's
  retypes, ui_ship_cust's ctor / dtor with decal.h) ran in series (shared headers), then three builders at a time. Weekly
  usage 27% -> 35% (the user's goal 35% by 16:00, reached 14:47; the window reopened twice at 34% for short items);
  ~0.5% of the week per Opus builder-hour. Plan log:
  `.work/run-2026-11-05-day/plan.md`. HEAD's imports.json: +436 vs the baseline, 0 lost outright (accepted under D66:
  0x6209D0 / 0x620CA0, false matches exposed by ui_ship_cust's own map instantiation, (iii); 0x618500 and avatar's copy
  of 0x6A2610, (i)).
- **avatar.cpp (AR1-AR4, ~+66 with nlp; copy ctor 0x77C8F0 via a pure-virtual base of the chat stream, avatar.h's pads dropped):** BodyObj / Unknown0C0[2] retyped as the client's RefCountPtrs; the out-of-line
  ??_G / clear / erase in 0x77C080 / 0x782200 / 0x784700 come from unreferenced members VC6 auto-inlines into callers
  (0x781C50, 0x781B20 / 0x781B50): with them in the tree AR1's interim inline_depth(0) went; ctor 0x77B870 / dtor 0x77C080
  exact; PlayerNLPClass's ctor / dtor out of line in nlp.cpp; 7 near misses left. D195 (P-AR2-1, the user): `tools/re/unref.py`
  lists a file's unreferenced members not in the tree (UR1, bd7bab67).
- **ui_ship_cust.cpp (SC1 / SC2, +83; SignalMap's connect<> as an inline layer):** ctor 0x6181B0 / dtor 0x6186A0 exact, 27 exact; SBaseViewUnknown1A0Class ->
  StarbaseShipCustomizeInterface in ui_ship_cust.h; decal.h's inline copies now emitted by ui_ship_cust.cpp (shared with
  ui_shipcust.cpp's identical COMDATs, nothing lost); 0x618CA0 is slot +0x54, not the ctor.
- **The character info window (BF1, TB1, TB2, IM1, AT1, +107; attrb Init 0x549AA0 exact):** ui_buff.cpp on gd_buffgadg.h; new ui_cinfo.cpp (UICinfoClass),
  ui_attrb_skills.cpp, ui_missn.cpp, ui_faction.cpp, each placed by the client's own strings (triage's hint was wrong for
  ui_cinfo); interim unknown_674260.h; Unknown5BB320Class::Mesh -> InterfaceMesh (the client's FAIL_IFs, conventions.md §5).
- **sbaseview.cpp / mainview.cpp (SB2-SB4, +15 and 13 tie-break):** 0x6A2120 / 0x69E6E0 / 0x69EF00 / 0x69DC80 / 0x69DFC0 /
  0x6A1C90, Init 0x69B5E0, the interface switcher 0x69F8F0, 0x69F710 and mainview's Init 0x50BA60 exact (two LIMIT REACHED
  rows cleared by `Get_Client()` in constructor arguments); ViewClass +0x18 a bool; MainViewUnknown1E4Class = UIPTradeClass;
  interim unknown_490900.h; UITalkTreeClass in sclient.h (interim until ui_talkt.h); UIManuClass's stray pack(4) dropped.
- **land.cpp (LD7):** no pair; 0x46F120 1784 -> 688 and 0x470F80 2298 -> 2042 masked rows, forms in docs/attempts.
- **gd_slider.cpp (GS1):** 0x66F330 / 0x66EF00 exact (18 of 19; +2), 0x66FD80 parked 0.59.
- **Harnesses (D182):** HC1 found clienttest broken by the ui_ship_cust rename (the real 0x61F250 on the test's 4-byte fake;
  money.cpp's operator>=): HC2 fixed it (fd2ddbf8; the test no longer checks 0x61F250's arguments); HC3 fixed SB2 / SB3's
  fallout (c79cd2d8: UIPTradeClass redefinition, GadgetClass void stand-ins, expected.txt's UITalkTreeClass line). HC4 at the end (5f66efb5): clienttest 103 / 103, trace equal; udpreplay alex3 ok, handshake 54; a final quiet-tree verify added the D188 shorts 0x462E30 / 50 / 70 in six files (bb6a86cd, +18 shared, 0 lost, replay 0).
- Interim placements (D137, the coordinator's): avatar.h's `Unknown_Block_Value`, 0x782FF0's raw tail, unknown_674260.h,
  unknown_490900.h, UITalkTreeClass in sclient.h, Unknown62D1F0Class in ui_attrb_skills.h. Traps filed in workflow.md
  §3.1 / §3.7 / §3.8 (one bullet per report). Section 4 trimmed to what is left.

### 2026-11-05 (06:30 PST, night): character select and creation toward exact; the class screen's effects and slider in the POC; D193 / D194; +203 pairs, 0 lost outright

- **Run** run/2026-11-05 (PR #162, from master 538199c6), 20 Opus builders (CS1-CS20) on disjoint files in the shared tree,
  one Opus research pass (RS1), two Sonnet harness passes (HC, HF); Claude subscription only (no API-key or agy builders).
  Weekly usage 19% -> 27% (the user's target 28-29%); ~0.7% of the week per Opus builder-hour. Plan log:
  `.work/run-2026-11-05/plan.md`. HEAD's imports.json: +203 pairs vs the baseline, 0 lost outright (moves and D66 (i)
  shares named in the commits).
- **Files:** ui_shipcust.cpp 43 -> 7 NOT EXACT (CS3 / CS7 / CS9 / CS13: Unknown620D20Class is ShipCustomizeInterface's
  second base, by-value Vector3 / Vector2 signatures, while-loop map walks); ui_shard.cpp 3 -> 18 of 20 exact (CS5);
  ui_class.cpp (CS2, CS10); ui_avsel.cpp's window rest exact up to ui_bio (CS2); ui_avatr.cpp / ui_avatar_cust.cpp (CS1,
  CS6, CS17: +20 in the last round, twins' forms ported; 0x5502F0 was a functional misread, now fixed); avatar.cpp
  (CS12 / CS15 / CS18: the file in the client's address order, padded to its `__LINE__`s; the client's RefCountPtr members
  and slot parameters; +39); tsk_login.cpp (CS10); unknown_585920.cpp all exact (CS13); decal.cpp 0x75CB10 exact and its
  interim inlines into decal.h (CS8 / CS19); gd_slider.cpp (CS14 / CS16); new ui_ship_cust.cpp, the starbase buy-ship
  window (CS21, 17 exact).
- **POC (the user's report, RS1):** the class screen lacked the avatar's teleport sparkle and the planet / ship turn.
  Causes: the ease 0x77B7D0 linked as a generated stand-in returning 0 (the angle jumped), and the teleport chain 0x788F90 /
  0x7896B0 / 0x788EF0 (av_tele04, W3DAnimSound.ini) empty or missing. CS12 rebuilt both; the user confirmed the sparkle, the
  turn and the race fade work. CS14's sweep: the only stand-ins called after login were the class screen's skill slider
  (gd_slider, never drawn): rebuilt, drawn (CS16's shot). The full creation flow calls no generated stand-in now.
- **Decisions:** D193 (P-CS1-1: folded short COMDATs credited to every proving file), D194 (P-CS4-1: the same in the caller
  tie-break). D195 (ClientPOC's release exe at handoff) was withdrawn the same night: not a new decision, `clientpoc.py
  --instance` / `--exe` already isolate builders' runs; builders.md's POC rule now states the check once and clientpoc.py
  refuses a missing `--exe` (a9acd2fe).
- **Harness check (D182) at HEAD:** udpreplay builds, alex3 rc 0, handshake 54/54. clienttest first failed to link
  (Unknown_B08C9C, defined only in the POC: 4f94141c), then faulted at GADGET 103: afa1bba5's correct
  Unknown_6603A0(bool, UIViewClass *) callers reached the real gadget.cpp function on the harness's zeroed gadget; d3f654e6
  fakes the gadget's slot +0x24 (the check now logs `6603A0(1)`): 103 checks, exit 0, trace equal.
- **Last builders (CS20 / CS21):** CS20 made four near-exact functions exact (0x785C00, 0x781570, 0x54EAD0, 0x55BFB0); CS21 started app/client/ui_ship_cust.cpp, which turned out to be the starbase's buy-ship window (StarbaseShipCustomizeInterface), with 17 exact; its ctor / dtor take over decal.h's inline copies next.
- Traps filed in workflow.md §3.1 / §3.4 / §3.7 / §3.8 (one bullet per builder report). Section 4 trimmed to what is left.
- **Next session:** after #162 merges, one item: avatar.cpp's parked BodyObj / Unknown0C0 RefCountPtr retypes (0x781410 / 0x782FF0
  block them; 0x7831C0 / 0x782200 wait on Unknown0C0), or ui_ship_cust.cpp's ctor 0x618CA0 / dtor 0x6186A0 (the new file, CS21),
  which take over decal.h's inline copies (a D66 move).

### 2026-11-04 (night): kerneltest links again (81 of 81); build.py builds wwlib again

- **Cause (`/VERBOSE` link):** c759a57b pointed kernel.cpp's Shutdown at `WW3DAssetManager::Get_Instance()`
  (`TheInstance`, 0xBFAEE0); kerneltest.cpp still defined the old `Unknown_BFAEE0`, so `TheInstance` pulled
  ww3d2.lib(assetmgr.obj), whose loaders and textures reach `WW3D::SyncTime`, i.e. ww3d.obj, which clashes with
  taskstest.cpp's WW3D::Begin_Render / End_Render / Unknown_Restore_Device (LNK2005).
- **Fix (tests/ only):** kerneltest.cpp defines `WW3DAssetManager::TheInstance` as its traced slot-0x10 object (Release_Unused_Assets);
  no ww3d2 object is linked now. `kerneltest.py run`: 81 of 81 expected lines in order.
- **build.py:** library builds stopped at wwlib since P-B2's FILE_FLAGS (rawfile.cpp `/GX`, 2026-11-02): one `cl` per
  flag set now. All eleven libraries built (0 failed). No library source changed, so no verify / pairs_cmp.
- Traps filed (workflow.md 3.7). PR on branch `kerneltest-link`.
- **Next session:** after #161 merges, one section 4 item: POC character creation's leftovers, starting with the smallest
  (ui_avatar_cust.cpp:63 declares 0x495FD0 `void`, ro_funcs's is `bool`: retype so it links to the real function, as
  c759a57b did for ui_avsel's 0x496110; check with `.work/coord/pocfull.sh`). The Create_Get item is done (#159).

### 2026-11-04 (evening): P-CGP-1 applied (D192); +58 pairs, +2 tie-break, 1 accepted loss

- **objmatch (D192):** the D100 range round keeps only survivors the object's own exact functions' relocated calls to the
  symbol reach, when they reach any (`obj_called`). ui_cpit's Create_Peek<FontSys> 0x546510 no longer competes with the
  same-bytes Create_Peek<VertexMaterialClass> for 0x584DF0, which stays held.
- **ui_cpit:** 0x584110 (WARP / DISTRESS / SPACE_CPIT) and 0x5845A0 (WARP_CPIT) peek Get_Style's FontSys as the client
  does (Create_Peek 0x546510): Create_Get released a reference it never took.
- **Pairs** (full verify vs d9b9a17b): +58 (app, ww3d2: attrib_dir, client_lego_impl, co_player_relationship, fontmanager,
  ui_line_display, ringobj and others), tie-break +2 (avatar 0x62A5D0, client_lego_impl 0x730090); lost outright 0x780AC0
  (avatar), as the proposal measured and D192 accepts. The proposal's +91 was measured on an earlier base. The CGP
  worktree's patches were gone (worktree removed); both were rewritten from the proposal's text.
- **Verified:** full `importer.py verify --pool 1` + pairs_cmp (above). Not run: clienttest, udpreplay, POC (the ui_cpit
  change is a runtime refcount fix in the cockpit's warp / distress text, not reached by the POC's flow). PR #159
  (branch `pcgp1`, ff0c5213 and this entry) waits for `vc6-verify` and the user's merge.
- **Next session:** after #159 merges, one section 4 item: kerneltest's link failure (ww3d.obj pulled in; `/VERBOSE` link
  to find the symbol), the smallest open item.

### 2026-11-04 (15:45 PST, day): nineteen builders on NOT EXACT code and strong-hint files; five new files; +570 pairs (+801 in PR #158), 0 lost outright

Coordinator Opus 5.5 (the user away; weekly 12 % -> 19 %, goal 21 % by 16:00); branch run/2026-11-04 (PR #158, back in
draft for the day, `ready` at the handoff), plan log `.work/run-2026-11-04/plan.md` (Day session section). The user approved
P-LIGHT-1 and P-QUIT-2 (D190, D191) and set no focus: cheap NOT EXACT refinements and new functions with strong hints.
Builders (Opus unless noted): TOOLS (Sonnet), CGP, UAC, AUXR, REG, CLS, REG2, EAM, AVS2, NET, SBV, BIO, SCI, BUFF, GADG, STRC,
PIP, HTML, LINES (Sonnet).

- **Tools (D190 / D191):** `clientpoc.py replay --retail` (a copy of the retail client under d3d8shot's user32 hooks, BM_CLICK
  on I Agree: retail character select on the POC's replay input), `tools/harness/dumpcmp.py` (draw dumps), and `replay`'s
  `client exit:` line. d3d8shot must be rebuilt for the hooks (`run_reference.py build`).
- **New files:** ui_buff.cpp (the client's __FILE__), gd_buffgadg.cpp (the client's __FILE__), strconv_lib.cpp (the client's
  __FILE__) and, interim (D137): ui_bio.cpp (the character info window's bio tab), unknown_strconv.cpp (the conversions
  0x4A30A0-0x4A3640), headers delta_money.h and aux_roots_send.h.
- **Toward exact:** ui_avatar_cust.cpp rebased on Unknown5BB320Class (ctor / dtors / per-frame exact, UAC); ui_registration
  (REG / REG2: second base at +0x84, 0x5FA900 / 0x5F8FD0 / combo fillers exact); ui_class 0x576360 / 0x5767F0 D64 -> exact
  and the planet turn (CLS); aux_roots gaps and SClient 0x7518A0 / 0x7555B0 with the Hull root (AUXR), USER_DTOR /
  delta_money (SCI); ENBAssetManager Create_Render_Obj / 0x48ECA0 (EAM); sbaseview's ctor / dtor / Shutdown / render (SBV);
  cordmgr Pump_Releases, cordcon pumpInternal 0.18 -> 0.76 (NET); ShipPip 0x80E260 / 0x80E530 (PIP); ui_avsel 0x55F140
  (AVS2); htmlview renames (HTML).
- **Create_Get / Create_Peek:** ui_manu / ui_chat_panel fixed (CGP); ui_cpit's waits for P-CGP-1 (§3).
- **Pairs:** +801 vs a302d4d3 for the PR (+570 today), 0 lost outright; D66 moves: 0x4A3CD0 to unknown_root alone (AUXR, D66 (v): the PR needs
  `/accept-d66`, D169), 0x4A7100 into delta_money (SCI, D66 (i)). Harness: udpreplay alex3 (messages equal) and handshake 54 at b62513b9; clienttest 103, trace equal after CTDAY's stand-ins (c366a207: ui_buff.cpp pulled aux_roots.obj and three symbols of its local gadget names). POC full creation flow at
  b62513b9: antprox receives GLOBAL_CREATE_CHARACTER 0x0072 (539 bytes; `.work/poc/shots/coord-152245`).
- **Proposals:** P-CGP-1 (objmatch: drop a D100 candidate an exact caller's call contradicts; +91 / 1 loss).
- **Coordinator traps:** plain `git worktree remove` on a wt.py worktree deleted the main tree's `.work/pseudo` through its
  junction twice (restored from `.work/pseudo-backup/`; use `wt.py remove`); builders' worktree POC links deploy to the shared
  ClientPOC\release (REG overwrote it); landing a branch by fast-forward skips bring-over's replay check (SCI's delta_money.h
  was unregistered and blocked every later landing); an importer.py SyntaxWarning reads as a replay difference; two builders
  on aux_roots' inputs move the SYMBOL_PAD window (STRC re-swept after SCI); builders finished in 0.5-1.5 h, faster than
  briefed: plan a second item per brief.
- **Next session:** PR #158 (`/accept-d66`, then merge); P-CGP-1; then one section 4 item.

### 2026-11-04 (05:15 PST, night): POC character creation end to end, the avatar lit like retail, the quit fault fixed; +231 pairs, 0 lost

Coordinator Opus 5.5 (the user away; weekly 8 % -> 12 %, cap 14 %); branch run/2026-11-04 (PR #158, draft), plan log
`.work/run-2026-11-04/plan.md` (every report, landing and decision). Builders: LIGHT, CRE1, CRE2, SHIP, AVS, DEC, QUIT
(Opus), CTFIX, AUD (Sonnet). The user's goals: the avatars' lighting, and an empty slot opening character creation.

- **POC:** login -> character select -> empty slot -> class screen (profession, skills, stats) -> avatar customization
  (the avatar draws; gender, head, hair, beard, shirt / pants styles, colours, random, body pull, height / weight) -> ship
  customization (hull / wing, paint, random, decals, palette drag, ship name) -> galaxy select -> Play Game: antprox
  receives GLOBAL_CREATE_CHARACTER 0x0072 (539 bytes; ShipCustDesc with the choices, decal 7). Checked by the coordinator at
  72203519 with no hook (`.work/coord/pocfull.sh`, input `.work/run-2026-11-04/in_full.txt`, shots
  `.work/poc/shots/coord-044407`). Quitting from character select or from creation exits cleanly.
- **LIGHT:** the avatar at character select matches retail in all 63 draws of a draw dump (render / stage states, textures,
  material colours, lights; the POC drew 60): avatar.cpp set the gen stand-in Unknown_BFAE52 instead of
  WW3D::OverbrightModifyOnLoad (MODULATE vs MODULATE2X); shiputils.cpp (new) colours the '~S1' / '~S2' / '~SK' materials
  (0x80FCE0 / 0x80FD10 / 0x812390); ro_funcs 0x496DB0 (highest LOD). Retail reference: a copy of the retail client on the
  same replay (P-LIGHT-1). Shots `.work/poc/shots/light-retail-004551` vs `light-011030`.
- **Creation:** CRE1 (class / avatar screens, AvatarCreationInterface 0x553AF0, ship_pip.cpp, chartemplate.cpp,
  unknown_585920.cpp interim; the reported slot-click crash was the class screen's Next into a gen ctor), CRE2 (ui_shard.cpp,
  the galaxy select and the create packet path), SHIP (ui_shipcust.cpp, ShipCustomizeInterface 0x620D20-0x628BF0, 14 of 43
  exact; shiputils), AVS (the avatar draws: CharacterViewAvatarClass lacked Get_Num_Sub_Objects / Get_Sub_Object, so its
  bounds were radius 0; ui_avatr's controls), DEC (decal.cpp's library, 0x80EAF0, palette drag 0x671570 / 0x671630 exact).
- **QUIT:** ViewClass::Unknown_10 0x6B46F0 called Create_Get where the client calls Create_Peek (identical bytes, so it
  matched exact): the vector freed the quit box. AUD's audit of every Create_Get / Create_Peek: 10 mismatches off the POC
  path (section 4).
- **Pairs:** +231 vs a302d4d3, 0 lost outright. Harness: clienttest (103, trace equal; clienttest.py now compiles
  shiputils.cpp with 0x80FF20 stood in: CTFIX, a tools change for the maintainer's review; expected.txt loses the stand-in
  lines the real functions replace), udpreplay alex3 equal, handshake 54.
- **Proposals:** P-LIGHT-1 (`clientpoc.py --retail`), P-QUIT-2 (replay reports the client's exit). Interim placements
  (D137): unknown_585920.cpp, decal.cpp's foreign functions, chartemplate.cpp (listed in section 4).
- **Coordinator traps:** the first 40 minutes of three Opus builders read as ~3 %/h on the weekly meter (start-up), later
  ~0.5 %/h per builder: measure twice before cutting builders; a message saying "merge when X lands" was read as "hand
  back now" (SHIP stopped early): say "keep working"; two builders rebuilding the same functions (DEC / AVS 0x671570) cost
  a hand-resolved rebase (`.work/coord/resolve_avs2.py`, `land_avs.sh`): name shared helper files in both briefs.
- **Next session:** PR #158 (review clienttest.py's change, then merge); P-LIGHT-1 / P-QUIT-2; then one section 4 item:
  the Create_Get mismatches, or POC creation's leftovers.

### 2026-11-04 (early): #153-#156 merged; CLAUDE.md's context section made rules

Coordinator Opus 5.5 with the user. PRs #153 (renames, FontSys, B2 merges), #154 (P-FU1-2), #155 (symbols.json
regenerated, pseudocode.py `--all` / `--check`) and #156 merged; no app change since the late-night entry.

- **Context rules (#156):** the session reached 525k tokens (Read 440k, Bash 154k) because CLAUDE.md's "Keeping context
  small" was worded as tips with a judgment-call trigger. It is now rules: named commands always log to `.work/log/` and
  print only their summary (verify, pairs_cmp, build, harness, client.py callers/refs, recursive grep); Reads use
  offset/limit; one back-buffer image per check; a third search or fourth file goes to a subagent; one item per session,
  with `/handoff` after each PR.
- **Next session:** pick one §4 item. Open from this run: the POC's quit fault (on master too), FontSys's constructors
  taking the render object as an integer, kerneltest's link failure, P-VM-1 (quiet tree; the user's call), P-T12-2
  (with the aux_roots generator's next edit), P-HF-1 (left).

### 2026-11-03 (late night): P-FU1-2, the font manager keyed by render objects; app 9,039 = 9,039, 0 lost

Coordinator Opus 5.5 with the user; branch `types/fontmanager-keys` from 1ff6d912 (PR #153 merged). P-T12-2 left for
the aux_roots generator's next edit (the user).

- **P-FU1-2:** FontManagerClass's map, ManagedFontClass::Id and the lookups take `RenderObjClass *` (Add 0x6D1D80 files
  a font under its object's address); the by-reference ones 0x6D2310 / 0x6D2700 / 0x6D2AE0 take
  `const RefCountPtr<RenderObjClass> &` (loadingview / ui_popupmsg / ui_login pass a RefCountPtr member). hash<RenderObjClass *>
  moves from interface_static_sort_list.h into a new unknown_robj_hash.h (D137 interim) both include. The callers'
  `(unsigned int)` / `(const unsigned int &)` casts go; ui_chat_panel / ui_cpit / ui_group / ui_manu's local views take
  the new types, and loadingview / ui_avatar_cust / ui_avatr's views carry the real class and method names (Get_Style,
  Remove, Unknown_6D2310), so their calls reach fontmanager.cpp instead of generated stand-ins; ui_registration's
  FontId +0x120 is a RenderObjClass *. app verify 9,039 = 9,039, pairs_cmp empty (the renamed hash_map instances lost
  nothing), replay 0. clienttest 103 / 0: its 0xBE67C4 is an empty FontManagerClass (members only), ui_stubs.cpp's
  second definition gone, expected.txt loses `gen LoadingViewFontManagerClass::Unknown_6D2310`. POC: character select,
  no crash, the same first calls.
- **symbols.json regenerated (quiet tree, stacked on P-FU1-2; 43ac8a37):** full verify of all eleven libraries before
  (equal to the record) and after, pairs_cmp empty in every category, replay 0; 127 entries take tonight's renamed
  mangled names, function/exact 16,094 -> 16,134.
- **.work/pseudo/ (the user: never lose its files again):** found empty (wiped 18:52, before this session; worktrees
  reach it through wt.py's junction). The old refresh ("delete, rerun") also lost every function that had become exact,
  since a plain run skips those. pseudocode.py gains `--all` (every function start, exact ones included, in place) and
  `--check` (starts without a file, triage requests without one; exit 1 if a start was never decompiled); CLAUDE.md,
  tools/re/README.md and workflow.md §3.6 say so. `--all` took 83 s: 29,804 of 29,817 starts decompiled (13 no
  function), triage's full.jsonl 14,753 requests all covered. Zip copy `.work/pseudo-backup/pseudo-2026-11-03.zip`
  (13 MB).
- **Quit at character select (the user, run 4):** the shutdown faults: ~LoginTaskClass -> CharacterViewClass::Unknown_10
  -> UIViewClass::Unknown_10 reads 0x29282726 (string bytes as a pointer). **Master 1ff6d912 does the same** (its POC
  built in a worktree, linked outside ClientPOC\release): not P-FU1-2's; §4 item. Scripted runs never quit before (killed
  at 60 s): `.work/run-2026-11-03/input3-quit.txt` is input3 plus Escape at 40 s and the confirm dialog's Yes (505, 543)
  at 44 s (the user: Escape opens the quit dialog from login and character select; Quit / Back are bottom left).

### 2026-11-03 (night): the quiet tree's client-name renames (AssetManager, EnvironmentMapManager, ControlKeysManager) and two retypes; app 9,039 = 9,039, 0 gained, 0 lost

Coordinator Opus 5.5 with the user; no builders. Branch `names/client-names-1103`, base c9403bfe; scratch `.work/cn1/`
(apply.py, apply2.py, named.py, the verifies' imports.json copies). The user reviewed §4's quiet-tree items: group A
(client-evidenced renames and types, no code change) now; P-HF-1 left as it is.

- **Renames (client wins, conventions.md §5):** the global 0xBDADC0 `Unknown_BDADC0` -> `AssetManager`
  (scriptedmovie.cpp's G_ASSERT 'AssetManager' 0x802D30; the user's call over MC3's neutral name; 7 app files and the
  four test programs' definitions); SClient +0x1338 `Unknown1338` -> `EnvironmentMapManager` (option.cpp's and
  scriptedmovie.cpp's 'sclient->EnvironmentMapManager'; its class SClientUnknown1338Class -> `EnvironmentMapManagerClass`, the
  user's call: NAME 3 by convention from env_map_manager.cpp); intrface.h's
  stand-in `ControlKeysManagerClass` -> control_keys_manager.h's `ControlKeysManager` (RTTI): the interface slots
  +0x70 / +0x74 and their overrides take it, the windows' casts are gone, ui_line_display.cpp's local stand-in class
  gives way to the header (calls Add_Keys / Remove_Keys; its 6 lines held for the FAIL_IFs). scriptedmovie.cpp's two
  macros are gone.
- **Types:** ClientClass +0x1144 is `HeightFieldClass *` (sclient.h's land view +0x90 too: LandClass::Field;
  co_subtypes.cpp's cast gone). ui_class.cpp's 0x496AE0 takes bool and gd_robj's 0x495FD0 / slot +0x90 a
  `TextureClass *`, as ro_funcs.cpp defines them (ui_chat_panel.cpp calls the slot with a texture). §4's
  ui_login / ui_cpit / eff_o declarations already matched (stale bullet deleted).
- **Verify (app, pool 3):** the first verify lost 4 outright, 0x515710 / 0x515730 (InterfaceClass's empty slots,
  3 bytes) and 0x610780 / 0x6107C0 (UIScutClass): short functions credited by their symbols.json name, which the
  renamed parameter type changed (AR2's trap, workflow.md §3.3). `replaces` entries in symbols_named.json for them
  and gd_robj's 0x66C470, `symbols.py --apply-named`: the second verify 9,039 = baseline, pairs_cmp empty; replay
  0 files differ. The long renamed functions keep their old mangled names in symbols.json until the next full
  regeneration (§4).
- **SClientUnknown1338Class -> EnvironmentMapManagerClass** (the user: NAME 3 by convention from env_map_manager.cpp;
  e96390a3): app 9,039 = 9,039, clienttest 103 / 0.
- **B1, FontStyleClass -> FontSys** (NAME 5: Get_Name 0x67EF20 "FontSys", 'FontSys::Change_Text' /
  'FontSys::Update_Text'; 34 files, 177 uses, lines held). The nine files' local `class FontSys` views now share the
  real class's mangled names, so their calls reach fontsys.cpp's members. The first verify lost 4 outright
  (Is_Complete 0x67EEE0, Class_ID 0x67EF00, Get_Name 0x67EF20, ??_E 0x67EF80: vtable-named short slots);
  symbols_named.json renames every vtable-sourced FontSys entry (8, the vtable 0xAFBF28 with `"kind": "vtable"`,
  else apply-named makes it a function). Second verify 9,039 = baseline, pairs_cmp empty, replay 0. clienttest 103 / 0;
  POC (`build.py link --out .work/cn1/pocbuild`, not deployed; instance cn1, /REPLAY input3, 60 s): no crash,
  character select drawn at frame 190000 (Karen idling, the list, the buttons' text), no FontSys stand-in generated.
- **B2 (the stand-in merges, one verify each):** B2.1 P-H18-1, tsk_login.h includes app/input/inpdsp.h instead of its
  opaque LoginUnknown0CClass: app 9,039 = 9,039. B2.2 P-T11-1, ClientObjectClass::Model -> RenderObject (co.cpp's
  G_ASSERT 'RenderObject != 0'; 7 files, only ClientObjectClass's uses): 9,039 = 9,039. B2.3 P-H12-3, the font
  globals 0xBE67B8 / 0xBE67BC as FontDatabaseClass * / FontStringFilesClass * everywhere (19 files: tsk_game.h /
  sclient.h forward declarations, fontdatabase.h included by tsk_game.cpp / sclient.cpp / loadingview.cpp /
  ui_line_display.cpp on held lines, the stand-ins' Unknown_6C52B0 / 6C55B0 as Load / Unload, the casts and the six
  `#define Unknown_BE67B8` workarounds gone): 9,039 = 9,039, +1 shared (ui_line_display 0x42F250), 0 lost. Before it,
  each global was two data symbols in a linked program (the type is in the mangled name). clienttest: the loading
  view's Unload now reaches the real FontStringFilesClass::Unload, so tsk_game_stubs.cpp gives 0xBE67BC an empty
  FontStringFilesClass (member constructed, not the constructor: it would load fc_default.ini through the null
  0xBE67B8) and expected.txt loses the one `gen SClientFontDatabaseClass::Unknown_6C55B0` line: 103 checks, 0 failed.
  B2.2 had missed clienttest.cpp's TestObjectClass destructor (`Model = 0`): fixed here (the app verify compiles no
  tests). POC: character select drawn, no crash, the same first calls. B2.4: tsk_login.h's unused
  `typedef InterfaceClass CharacterViewUnknown1B4Class` deleted (no user left): 9,039 = 9,039. B2.5: ui_group.h's
  stand-in UIGroupMemberUnknown2CClass is ui_cpit.h's UICpitUnknownD0Class (same slots; ui_cpit.h gains 0x5DC390 /
  0x5DCAE0, ui_group.cpp's cast gone): 9,039 = 9,039. B2.6 P-TG4-1: UIComboBoxClass moves into a new ui_combobox.h
  (D137 interim name, NAME 3; forward declarations for ui_combobox.cpp's local stand-ins, Unknown_Show_Selected
  defined inline in the .cpp before its callers), ui_chat_panel.h's UIChatComboClass gives way to it: 9,039 = 9,039.
  At the branch head (d3aa1bd1, committed tree): clienttest 103 / 0; POC to character select, no crash, the same three
  first-call stand-ins.
- **Harness:** clienttest 103 checks, 0 failed; udpreplay alex3 as before, handshake 54; movietest links. A first
  clienttest run crashed (0xC0000005, output ending at line 87): it linked the main tree's app.lib from before the
  rename, whose objects still named `Unknown_BDADC0`, and the second link pass made a NULL stand-in for it (§3.7's
  "build app before a clienttest link"; for a renamed global it shows as a crash, not a link error). **kerneltest
  does not link on master either** (c9403bfe, checked in a worktree): ww3d2.lib(ww3d.obj) is pulled in again and
  clashes with taskstest.cpp's WW3D::Begin_Render / End_Render / Unknown_Restore_Device stand-ins (56d6c356's fix
  no longer holds; §4).

### 2026-11-03 (evening, after the handoff): PR #147 verified and merged; prepush_check scans only what a push adds; verify_pr runs only from master

Coordinator Opus 5.5; no builders. Plan log `.work/run-2026-11-03-pm/plan.md`.

- **PR #147 (the run) merged** after vc6-verify success on 0f9cb094c: +5,666 pairs, 6 D66 moves accepted by the
  maintainer (`/accept-d66 0f9cb094`), D189(2)'s four losses corrected through master's accepted-corrections.json,
  0 lost outright. Its earlier failures were the host, not the PR: free memory (5.5 GB; MSBuild node-reuse
  `.NET Host` workers, not antprox), two verifies of #147 at once (the second drops the first's `pr147.vf`
  worktree), and a run from the main tree on the run branch, whose corrections list lacked the D189 entries.
- **PR #148 (merged):** the four D189(2) entries in `docs/data/accepted-corrections.json`; prepush_check.py lets
  docs/data's generated tables reach 25 MB (5 MB elsewhere) and scans only the blobs no remote-tracking ref has
  (`--base origin/<base>` in CI on a PR, `--full` otherwise): 4 min 47 s -> 4 s. symbols.py writes symbols.json
  one entry per line (5.93 -> 5.59 MB).
- **PR #149 (merged):** verify_pr.py refuses unless its checkout is origin/master with no local change under
  tools/ or to the corrections list (`.work/wt/verifier`); `--no-post` still runs anywhere, printing only, under a
  `pr<N>.test` record verify_watch.py never lands from.
- Repo upkeep: `git gc` (.git 283 -> 114 MB), worktrees 51 -> 7 (kept: hc2, mrc, pocin with uncommitted work;
  poc1, verifier, corr).

### 2026-11-03 (18:45 PST, afternoon): the POC's character select lit, Karen with hair, blinking and idling, the narrator walks off on Accept; D188 / D189 (+5,421 pairs); multi-instance POC; +2,321 in-tree, lost outright only D189's four accepted

Coordinator Opus 5.5 (Claude weekly 0 % -> ~7 %, the user's soft goal); 21 Claude builders (Opus, two Sonnet), branch
run/2026-11-03 (PR #147, draft), HEAD 20550a64; plan log `.work/run-2026-11-03-pm/plan.md` (every report, merge and decision).

- POC: login -> avatar list -> character select works against antprox /REPLAY with no crash (clienttest 103/0 equal).
  LTS: the login was never stuck (shots count presented frames: frame 45000 is ~20 s at ~3,500 fps); characters did not
  draw because CharacterViewAvatarClass lacked Set_Transform / Set_Position 0x77D1E0 / 0x77D2B0 (exact). POS: per-frame
  update 0x783B50 + idle animations; NLP: personality manager 0x790560, PlayerNLPClass, phonemes (+28); FACE: morph
  weights, head look 0x7854B0; BLINK: the face HMorphAnim (blink, brows, lip sync); HAIR: hair / ears / goggles / beard /
  weapons / eye colour (0x77DA20's player branch); CHAT: chat_edit_stream.cpp + the avatar's command path (interpreter
  0x784700, queue 0x782200, walk / sit, narrator lines): the narrator's '/walk_off' on Accept; PATH: movement code;
  LIGHT: the flat look was WW3D statics 0xBFAE4F-0xBFAE52 written into gen stand-in globals (OverbrightModifyOnLoad never
  reached ww3d2); CUR4: the cursor's black square was a stale build/ww3d2/ww3d2.lib (all main-tree libs rebuilt);
  STB / STB2: sound_manager.cpp, wnd_file.cpp (StandardFileClass), env_map_manager.cpp, starbase_lights.cpp,
  Unknown_BFAEE0 = WW3DAssetManager::TheInstance; MSG: msgserv.cpp (the client's -ENABLE_MESSAGES debug server, binds
  0.0.0.0:9999) + tools/harness/msglisten.py + clientpoc.py --messages; NEX / LGX: ui_class 0 -> 7 exact, gd_slider +1,
  login path LIMIT REACHED (the /O2 constant-in-ebx region). MRC: concurrent-replay crash = shared auth.ini (per-instance
  Data\client\ini now) + DynamicWindowHorizontalClass's element tests (5 D66 moves the user accepted).
- Multi-instance POC (the user's request): poc_multi.cpp (CreateMutexA enb_mutex_lock -> _<pid>, SetCursorPos no-op
  without ENB_POC_INPUT), `clientpoc.py run|replay --instance NAME --exe EXE` (ClientPOC\inst\NAME\release, client by
  antprox's descendant PID, antprox starts serialized), `build.py link --out` no longer deploys.
- Libraries: MAPR (mapper twins named: +12), XMP (E&B's JPEG writer, wwlib/unknown_jpeg.cpp, +20), TW3 (ww3d2 +39),
  TSH / TLIB triage (most leftover runs are objmatch `short`). D188 / D189 (SHRT): short functions credited at
  independently known addresses (vptr 4690, call 628, vtable 33; COMDATs shared), nested exact starts dropped, matches
  anchored to function starts: +5,421 pairs, in-tree +2,321 (app 7672 -> 9039, ww3d2 3303 -> 3739, wwaudio 574 -> 780,
  wwshade 278 -> 487 ...), lost outright only 0x8437A4 / 0x924875 / 0x9CEEF7 (-> 0x9CEFD0) / 0x9F367D (accepted), one
  D66(v) move 0x9306E0.
- AVX: Karen's head / tattoo textures 0x7857D0 / 0x7859F0 / 0x785C00 (functional) called from 0x77F750; the dtor 0x77C080 releases the gesture queue and the EAR clone; +0x2B8 / +0x2BC renamed HeadObj / BodyObj (the client's asserts). Harness at 20550a64: clienttest 103/0 equal, udpreplay alex3 + handshake 54. Open leads (section 4): RefCountPtr avatar members; gd_robj's 0x495FD0 declared int (ro_funcs: bool); ChatStream rename (D137).
- Decisions: D188, D189 (the user; (b) source-order tie-break left open). User calls: MRC's 5 D66 moves accepted;
  ANCHOR_HITS on; message server option (a) + a listener.
- Coordinator traps: never keep-both a C++ conflict (gd_slider duplicated, caught by verify, redone); rebase_gen.sh takes the
  run's imports.json and re-verifies only app (a library's new file drops out of the record: verify --lib reads its file
  list from imports.json); symbols.py needs --objs and a good symbols.json as input; TaskStop of a background shell leaves
  its children running; a worktree reset needs its wt base file updated; poc_check.sh / rebase_gen.sh paths must be absolute.

### 2026-11-03 (11:45 PST, morning): the POC gets past the avatar list without a crash; agy Flash window spent; +45 pairs, 0 lost

Coordinator Opus 5.5 on the subscription (Claude weekly 99 %: hourly heartbeats, minimal); builders Gemini 3.8 Flash high
through agy (D187), 7 items, the 5-hour window 100 % -> 0 % in 2h50m (about 1.4 % per 100k tokens; 7.5M tokens in all).
Branch run/2026-11-03, PR #147 (draft), base 7cf59deb (PR #146 merged); HEAD cd74e1a8, +45 pairs vs the baseline, 0 lost.
Plan log `.work/run-2026-11-03-agy/plan.md`.

- STORE (0.45M; resumed from the night's Flash + Sonnet edits): settings_store.cpp 0x7C97B0-0x7CCF20 (accessor 0x7CA2D0,
  ctor, 0x7CB610, Set_String / Set_Int / Set_Int64 / Get_String) +23, 6 NOT EXACT; POC store stand-in and clienttest's
  settings stubs gone.
- A574 (1.30M): ClassSelectInterface, new app/client/ui_class.cpp/.h (0x574710-0x577DA0) +12, 9 NOT EXACT.
- LOG (1.52M): Unknown_Log 0x7E5F90 / 0x7E5F70 (unknown_log.cpp), 0x8B4560 / 0x8B4510 (shared/unknown_8b4560.cpp), ro_funcs
  0x496730 / 0x496AE0 retyped bool in 8 callers (39 fewer gen stand-ins): +3, all exact.
- CUR3 (1.89M, no commit): the cursor's black square is _PresetOpaque2DShader chosen at 0x974CD0 while the cursor texture's
  format is still UNKNOWN (Request_Foreground_Loading off the DX8 thread queues it); smouse 0x515120 never re-creates it.
  ARROW.dds is DXT5 with alpha.
- SLD (0.96M): GadgetSlider, new app/client/gd_slider.cpp/.h (0x128 bytes, vtable 0xAFAF78) +4, 0x66DFC0 / 0x66E460 NOT EXACT.
- CVW (1.13M): the crash at 0x400003 was ui_class's undeclared UIClassNotifierClass method (/FORCE:UNRESOLVED points it at the
  image base); ClassSelectInterface connects FlashingGadgetEvent through SignalMap 0x4F9740 / 0x4F98D0.
- AVS (0.66M): POC forwarders in poc_ui.cpp for UIAvsel* -> the real CharacterViewClass / CharacterViewAvatarClass (the gen
  no-op 0x67AB20 left Model NULL: crash 0x4F592E in FontManagerClass::Add).
- TIP (1.02M, timed out before committing; the coordinator committed its verified work): intrlist.cpp 0x687150 / 0x6872A0 /
  0x687360 +3, exact.
- POC via antprox /REPLAY (D181) + input3, per landing: at 9ca5bfd9 a crash after Accept in GadgetClass::Unknown_65F5F0
  (0x42AC5F, garbage pointer) <- tooltip 0x6AB5B0 <- UIViewClass::Unknown_14 <- LoginTaskClass::Run; at cd74e1a8 (with
  0x6872A0 real) no crash in 50 s, login + avatar list received, but the screen stays the login screen (shots
  .work/poc/shots/replay-113603).
- Coordinator traps: a worktree reset onto a new base needs its wt base file updated (bring-over refused 'merge commits');
  poc_check.sh's LOG and rebase_gen.sh's BASELINE must be absolute paths.
- Proposals (user): build.py:568 skip the clientpoc deploy when `--out` is given (CVW: worktree links deploy over
  ClientPOC\release); D66-free. prepush_check.py's cat-file pipe can deadlock on Windows (CVW).

### 2026-11-02 (23:10 PST, night): the POC's mouse and keyboard log in against antprox /REPLAY; agy quota measured; +85 pairs, 0 lost

Coordinator Opus 5.5 on the subscription (Claude weekly 98-99 % all run: lean); builders Gemini 3.8 Flash high through agy
(D187), one Sonnet test. Base 25d96676 (#145 merged; `.work/imports.25d96676.json`); run branch `run/2026-11-02`, PR #146.
Plan log: `.work/run-2026-11-03/plan.md`. The user (19:55): 4h45m, spend agy's 5-hour window on the POC: the mouse on the
login screen's buttons and text boxes.

* **Input (coordinator):** `clientpoc.py --input FILE` posts timed window messages; programs/clientpoc/poc_input.cpp
  (ENB_POC_INPUT=1) redirects the exe's GetCursorPos / GetMessagePos / SetCursorPos / GetForegroundWindow / GetKeyState /
  GetAsyncKeyState / GetMessageA imports to a virtual cursor and keyboard (the client reads the cursor, not the message,
  and takes input only in the foreground): the real desktop is never touched (2a22360b). Traced (uncommitted trace build):
  clicks reached GadgetClass::Process_Input, but RObjGadgetClass's hit test called the gen pick 0x496960 (always false),
  and the buttons stayed disabled until the narrator avatar's +0x220 (POC stand-in false; 2518a681 made it true).
* **Builders (Flash):** PICK 5a83ed09 ro_funcs.cpp, the pick 0x496960 exact, +7; SIG 94b2360f SignalMap / SignalList, +5
  (12 NOT EXACT); KEYS 819dcbde key list / intrlist, +43; HOV a1cf3c59 InterfaceSceneClass::Get_Scene_ID const (it never
  overrode the slot: the hover FAIL_IF local_interface_sort.cpp(148) every frame); CUR c3427718 bmp2d 0x974CD0, ww3d2 +1
  (the cursor's black square stays); AV a69808c6 avatar.cpp, +5 (+23 shared); AV2 4f190458 the avatar's constructor makes
  ANPCBODY000 (+0x2BC) / ANPCHEAD000, Render 0x77D420 exact, +1. CSEL's 2d1b3969 (ui_avsel forwarders) never linked:
  reverted (cb329d99). CUR2 / STORE stopped for quota (STORE's partial edits uncommitted in .work/wt/store).
* **POC (coordinator, D181 replay with `.work/coord/replay.py`):** at cb329d99 the mouse clicks the edit boxes (focus),
  typing shows ("tester", ******), Accept logs in: AuthLogin accepted, GLOBAL_LOGIN, GLOBAL_AVATAR_LIST (Karen); the task
  goes on (AV2), makes the avatar selection interface (gen 0x574710 ctor) and crashes in CharacterViewClass::Unknown_67A370
  (+0x54 of it). Without a server (the default 40 s run) no crash. Harness check at cb329d99 (worktree hc2): POC 40 s no crash; clienttest 103 checks, 0 failed, expected.txt updated (413 stand-in trace lines gone: SignalMap, key list, intrlist now real); udpreplay alex3 ran, handshake 54.
* **agy quota (the user's `agy -p /usage --output-format json`; Git Bash needs MSYS_NO_PATHCONV=1;
  `.work/coord/agyquota.py sample|items`, samples in .work/agy-quota.csv; stream usage is per turn, summed):** Gemini 5h
  window 7.8-11M stream tokens (100 % -> 11.3 % for ~6.9M; AV2 alone 0.84M = 7.5 %: the charge is by cost, cache reads
  included, so it varies by item): 6-9 Flash items; Gemini weekly fell 2.28 %
  (48.91 -> 46.63) while the 5h fell 7.48 % (22:04 -> 22:48): the week is ~3.3 five-hour windows, ~25-36M stream tokens
  (~20-28 Flash items a week; refine with more samples). Claude/GPT group via agy: Sonnet 5.5 high used 170k stream tokens (2.9M cache reads) =
  36.8 % of its 5h and ~16-18 % of its week: 5h ~460k, week ~1M tokens: not a builder pool.
* **Totals:** +85 pairs (27 shared, 2 tie-break), 0 lost against 25d96676. 11 Flash launches (8 with commits), 1 Sonnet.
* **Traps (workflow.md §3.7):** a builder's POC-only edit must be linked, not just app.lib built (CSEL); the agy usage
  stream line is per turn. **Open for the user:** P-API-5 (a) / (c), P-VM-2, P-VM-1 (unchanged); CLAUDE.md's safety line
  already names D181 (the user's edit tonight).

### 2026-11-02 (19:00 PST, evening): agy (Antigravity) builders on the user's Google subscription; P-B2 adopted (D186); +39 pairs, 0 lost

Coordinator Opus 5.5 on the subscription; builders through the Antigravity CLI (`agy -p`, the user's Google Pro
subscription, no API key). Base 3436a036 (#144 merged; `.work/imports.3436a036.json`); run branch `run/2026-11-02`,
PR #145. Plan log: `.work/run-2026-11-02-agy/plan.md`; launcher `.work/run-2026-11-02-agy/launch-agy.sh ID MODEL TIMEOUT`
(one `wt.py` worktree per builder, stream-json to `ID.jsonl`). The user (16:05): spend the remaining 90 % of agy's
5-hour window (reset ~19:15) on the POC.

* **Pilots (afternoon):** P-LG2-1 through the agy MCP bridge (Gemini 4 Argon): re-measured, both forms lose (the item was
  already closed; its stale section-4 bullet is deleted). RF (Opus 5.5 high via agy): wwlib rawfile's four functions; the
  ctor / dtor need /GX (RawFileClass lies in the app range): **P-B2 adopted (D186)**, `importer.py FILE_FLAGS`, wwlib +4.
  Opus via agy hit `RESOURCE_EXHAUSTED (429) Individual quota reached` after 8 min (214k tokens). HF (Gemini 3.8 Flash
  high): heightfield's byte-identical Get / Set twins named by their land.cpp callers (symbols_named.json, D88), app +4.
* **Run (Gemini 3.8 Flash high, 3-4 at a time):** EAM4 (75624fe0): ENBPrototypeClass 0x48AED0-0x48B050 exact,
  Create_Render_Obj 0x48B220 functional (NOT EXACT shape), the Unknown_ members renamed to EA's names, +4. UREG (f82efe6b):
  new ui_registration.cpp, demo checks 0x5FAD50 / 0x5FADE0 exact, their POC stand-ins gone, +2. EULA (8b6cf858): stopped
  by the coordinator (WinMain calls 0x847D20 for the ROC text in retail too: its POC run would show the real dialog; my
  brief said otherwise; its run 65 reached DialogBoxParamA, which returned -1, no dialog shown); finished by the
  coordinator with the POC stand-in kept: 0x847D20 exact, 0x847FE0 NOT EXACT (inline-budget), provisional
  unknown_847d20.cpp, +1 (+2 shared). EAM5 (32a43c5d): Recolor_ParticleEmitter / Recolor_Sphere / Recolor_Ring /
  Post_Process exact, +4. ROF (f2652478): ro_funcs 0x496730 / 0x496AE0 exact, +2. AUD (d14272b7): new surfacetypes.cpp
  (0x442870 / 0x4428E0 / 0x442930 exact, 0x442650 NOT EXACT frame 0.51), tsk_login calls the real Flush_Playlist 0x43FA80,
  +1 (+2 shared). UREG2 (6dd03470): ui_registration 0x5FAE70 / 0x5FAE90 / 0x5FAFF0 / 0x5FB330 / 0x5FB540 exact,
  0x5FAF60 NOT EXACT (register), +4 (+11 shared).
* **Totals:** +39 pairs, +13 shared, 0 lost against 3436a036. Final check at HEAD (coordinator): POC 40 s, no crash (24 first-call stand-ins, gen 0x5FAE70 / 0x43FA80 / 0x442650 gone; 0x496730 / 0x496AE0 still gen: their callers declare int parameters, section 4); clienttest 103 checks, 0 failed; udpreplay alex3 ran, handshake 54.
* **agy as a builder host (the evidence for D187):** every builder followed builders.md (baseline, verify, pairs_cmp, attempts
  rows, VM release, no push). Tokens from the stream's result line (`usage`): HF 0.62M, EAM4 1.29M, UREG 1.30M, EULA ~1.58M
  (killed), EAM5 1.12M, ROF 0.59M, AUD 0.66M, UREG2 0.72M, plus 17-43M cache-read tokens each; builders' own token figures
  were off by 10x or more. The user's readings: HF = 10 % of the 5-hour window, 4 % of the week; 60 % at 18:05: about 1 %
  of the window per 100k tokens. Per-model quotas: Opus via agy ran out in one short item, Flash ran nine. POC runs started
  by agy builders all crash in WW3D::Set_Gamma at startup (runs 63-69) while the coordinator's runs of the same code do not:
  builders' POC checks prove nothing (workflow.md §3.7).
* **Decided:** P-AGY-1 approved (D187; tools/agy/). **Open for the user:** P-HF-1 (the builder's: heightfield's `_2` twins as real overloads, when land.cpp is
  next edited; no proposal needed now), P-API-5 (a) / (c), P-VM-2, P-VM-1.

### 2026-11-02 (06:45 PST, night): D185's exact variants applied, the POC's login screen drawn with its text; +476 pairs, 0 lost outright

Coordinator Opus 5.5 on the subscription only. Base 6a08fe5d (#143 merged; `.work/imports.6a08fe5d.json`); run branch
`run/2026-11-02-night`, PR #144. Plan log: `.work/run-2026-11-02-night/plan.md`. The user (00:30): spend the week to
98 % by 07:00, subscription usage only; merge D185's exact Batch API variants, then the POC client; accepted
(`/accept-d66`) the move of the ctor 0x6D8B90 and its statics into fonttexture.cpp (done, 037f86ae: no exact pair changed
file); declined P-VM-2 for tonight. HEAD against the base (quiet-tree verify at 9f59bf88: 7,456 in tree, 0 errors): **+476 pairs (441 + 35 by caller tie-break; 85 of them shared copies), 0 lost outright; lost 3, all D66 (i): the moves 0x4481E0 (eff_o, held by ww3d2 / wwmath) and 0x77B030 (udpconn, held by unknown_htmlattrib) and 0x510CC0 (fontsys, held by 28 files)**. about 100 commits;
33 builder launches (28 Opus, 5 Sonnet; several resumed for a second item); weekly usage 90 % -> 98 %.

* **D185 applied (VC, VO, PF, PI):** 72 of the 75 functions with an exact variant are exact in the tree (+102 pairs with
  their tie-break / inline copies). Most "LOSES" entries were later asserts' `__LINE__` shifts (the variant is longer):
  absorbed in blank padding. Dropped: 0x78D8A0 (needs an invented forwarding helper), 0x8020B0 (a fake derived struct),
  0x6E2290 (loses rect3d 0x6B71E0 / 0x6D80A0).
* **The POC (D179 / D181):** the login screen draws with its text: the welcome header, both labels and edit boxes (the
  stored name dimmed), Quit Game / Accept, Account Management (`clientpoc.py run --shots`, PI f9b90803: the client's own
  back buffer through d3d8shot). The invisible text was fontsys.cpp's own externs for WW3D::SyncTime / PreviousSyncTime
  0xBFAE3C / 40 (zero stand-ins: the FADE clock never moved); PF read WW3D::Get_Frame_Time() (450c321c), FT / MV the same
  in 8 more files. Real on the path now: the FontStyleClass stand-ins, the click query 0x42EC60, textgadg (14 / 15),
  tooltip.cpp, the message box (unknown_msgbox.cpp, gd_dynamicwindow.cpp, unknown_51c800.cpp, intrlist's list),
  ui_popupmsg.cpp, the character view (charview.cpp has no NOT EXACT left), InterfaceClass 0x540A90, smouse.cpp (11 / 11),
  dbgmgr.cpp (the step log), display.cpp's device 0x442AC0 and RenderStateClass (rs.cpp), mousetbl.cpp / .h (the cursor
  table: the stand-in had wrong slots), enbassetmgr.cpp / .h (Create_Render_Obj, the texture release loop, the mesh HSV recolour; EAM3 found EA's own source of it in the Renegade code: §4.1), avatar_desc's
  KV adds, the message reporters / server / filter, TCPConnection::Key_Exchange and UDPConnection::Send_Data byte-exact.
  Final harness check (HC3, 6d207582): no crash in 40 s, 30 first-call stand-ins (50 at run 22). The 29
  combo_ffactory.cpp(101) FAIL_IF lines are retail behaviour (MIX: 29 names in two .mix files each). Not run: against
  antprox /REPLAY (its launcher writes HKCU / HKLM keys; CLAUDE.md forbids registry changes unattended).
* **New files named by the client's strings:** ctrngadg.cpp, rs_robj.cpp, rs_proj_sup.cpp, mousetbl.cpp, enbassetmgr.cpp,
  gd_dynamicwindow.cpp, ro_funcs.cpp, dbgmgr.cpp, display.cpp, rs.cpp, ui_popupmsg.cpp, tooltip.cpp. Interim (D137):
  unknown_msgbox, unknown_51c800, unknown_515640, unknown_458710, unknown_45e3f0, unknown_message{,_record,_server,
  _filter}, mousetbl.h, enbassetmgr.h. P-MC-3 done (MC3 a0343ac0): UnknownBDADC0Class -> ENBAssetManager (conventions.md).
* **Levers (workflow.md §3: 42637fb9, 46affcce):** a file-static inline wrapper one level down around an STL call
  (pop_front, push_back, getPointer: exact three times), `it._Inc()`, a static inline accessor for a member argument
  (register order, twice), a member pointer read into a local, `bool more` loops against rotation, constructor stores in
  declaration order, a dead `x = y = 0;` for lea operand order, loop-bound read order before dead reads.
* **Health:** HC / HC2 / LD1 / LF / MSG / KT closed every link gap the night opened (Tooltip SignalMap, Wstring / Wtime
  operators, the 4BE9D0 stage strip in clienttest.py 04968dfa, the message box's fake sizes); kerneltest, broken already at
  the base (taskstest.cpp's WW3D render pair), links and runs again (KT 56d6c356: 81 of 81). Final check at 6d207582:
  clienttest 103/103 trace equal, udpreplay alex3 / handshake 54/54, POC no crash; later commits checked by their builders
  the same way. One POC crash landed and was fixed within the hour (d07356a2 -> d043eeba). Python changed: clienttest.py
  (04968dfa), clientpoc.py (f9b90803 --shots): the maintainer's `/verify-approved`.
* **Pacing:** 1 % steps of the weekly figure misled the 02:03 heartbeat (stops cut, reopened at 02:18); the true rate was
  ~0.33 % per Opus builder-hour, and most builders finished well inside their stops.
* **Open for the user:** P-API-5 (a) / (c), P-VM-2, P-VM-1 (unchanged). Tools ideas logged, not proposals yet: objmatch
  crediting a tied function by a symbol learnt from an exact caller's relocation (MV); a layout helper for assert lines in
  appfile.py (RS); d3d8shot saving A4R4G4B4 textures (PI).

### 2026-11-01 (23:55 PST, night): POC text path and login activation on API-key builders; D185 variants for the whole backlog; +29 pairs, 0 lost

Coordinator Opus 5.5 on the subscription. Base a86efb86 (#141 merged; `.work/imports.a86efb86.json`); run branch
`run/2026-11-02`, PR #142. Plan log: `.work/run-2026-11-02/plan.md`. The user: scale D184's pilot to the whole NOT EXACT
backlog (D185), and POC items 1-3 on API credits through headless builders (`claude -p`, Opus 5.5, the key
`ENB_ANTHROPIC_API_KEY`, `--max-budget-usd` each; launcher `.work/run-2026-11-02/launch.sh`). The `enb-d163` workspace's
limit is monthly ($140, then $160 by the user); November's spend at 23:30 was the night's alone. HEAD against the base:
**+29 pairs (+20 shared), 0 lost outright, no moves** (quiet-tree verify a4b9e198). Weekly subscription usage 90 % throughout.

* **API spend tonight $92.67:** D184 pilot $4.66; D185 round 1 $39.89 (367 requests), round 2 $25.43 (201 near misses);
  five headless builders $22.69 (TXT1 6.36, FM 2.94, TXT2 5.65, ACT 5.23, TXT3 2.51; the stream's `total_cost_usd`).
* **D185 (VS1):** variants.py `--run full`: 437 of 479 entries resolve, 367 requests (8 / 5 / 3 / 2 variants by source
  length); `try --pool K` unattended (detached Start-Process; resumable results.d/). Both rounds: **75 of 355 functions
  have an exact variant, 47 lose no other pair of their file** (`.work/variants/full/results.json`): to apply through
  verify / pairs_cmp (D66), as VA1 did for the pilot (+13 for 7). Some are stale already (TXT1 changed 0x6D5170).
* **POC (FM, TXT1, TXT2, TXT3, ACT):** ui_login / ui_avsel on the real FontManagerClass (81968b85; exact sets unchanged);
  Update_Text 0x6D64C0, fonttexture.cpp, the effect step 0x6CA0C0 (the client's EffectType switch, names from
  Get_Effect_Type) and Update_Text's callees 0x6CCE60 / 0x6CD180 / 0x6CD6F0 / 0x6D87D0, functional (D64) in fontsys.cpp
  and the interim unknown_6ca0c0.cpp / unknown_6cce60.cpp (D137); the login activation / update functions (intrlist.cpp
  0x6871D0 exact, control_key_list.cpp, tasks/settings_store.cpp: the lkey poll and its WinInet thread) and smouse.cpp's
  software mouse (5 exact); kernel.cpp calls WW3D's real 0x9179D0. POC run 23: no crash in 40 s, the '!text' FAIL_IF gone,
  Update_Text runs every frame; whether text reaches the screen is unseen (clientpoc has no back-buffer shot yet). Next
  stand-ins: Unknown_42EC60 (the button-down query: clicks), Change_Text 0x6CDBC0, 0x6D8650, 0x6CE910.
* **Health:** clienttest relinked (CT4 2ed61121: WININET.LIB in clienttest.py and build.py's clientpoc, a Get_String
  stand-in, a zeroed software mouse for the now-real smouse 0x515340; expected.txt -1 stub line): 103/103 equal;
  udpreplay run alex3 / handshake 54/54; replay 0.
* **Host memory was the limit:** Claude Code's reaper killed the four launcher shells at 22:52 (the builders' claude
  processes kept running); leases waited ~25 minutes. Headless builders: 2-3 at once on this host (workflow.md §3.0).
* **Records:** TXT3's subject says POC run 21 (it is 23). Python changed: variants.py (D185), clienttest.py, build.py
  (WININET.LIB): the maintainer's `/verify-approved`. Open for the user: P-API-5 (a) / (c), P-VM-2, P-VM-1.

### 2026-11-01 (21:45 PST, evening): D183 (shared COMDATs) landed and D184's Batch API variant pilot applied; +27 pairs, 0 lost

Coordinator Opus 5.5. Base 7fb89dbb (#140 merged; `.work/imports.7fb89dbb.json`); run branch `run/2026-11-01-eve`, PR #141.
Plan log: `.work/run-2026-11-01-eve/plan.md`. The user approved P-IS-1 (D183, on subscription usage) and the P-API-5 (b)
pilot (D184) for tonight. HEAD against the base: **+27 pairs (app +26 incl. 1 tie-break, ww3d2 +1), 0 lost outright, no
moves.** 3 builders (2 Opus, 1 Sonnet); weekly usage 89 % -> 90 %.

* **D183 (IS1):** objmatch credits an exact COMDAT several objects hold at one address under one symbol to the holder
  whose range is strictly nearest, and marks such pairs `shared` in imports.json (3,661 pairs at 537 addresses, all
  already paired at the base); D166's tie-break labels decisions that needed a shared caller (`shared_callers`); gaps.py
  and triage.py's neighbour lookup skip shared pairs; pairs_cmp lists shared gains / lost holders and never counts them as
  moves. The as-worded form ("each holder") lost inpdsp's 0x42A900 / 0x42AAE0 outright: the user amended D183 (P-IS-2).
  Full verify of all 11 libraries: +14 (winmsg 0x42FBF0-0x430340 x12, kv_encoder_base 0x4B56F0, ringobj 0x910490)
  (928fe104).
* **D184 pilot (VP1, VA1):** `tools/re/variants.py` (pick / build / submit / fetch / try; the key ENB_ANTHROPIC_API_KEY).
  Batch, claude-opus-5-5: round 1 40 functions ($3.08), round 2 the 20 near misses ($1.59): **$4.66**; 11 of 39 exact in
  `try`, 7 losing nothing else. VA1 applied all 7 through verify / pairs_cmp (+ 0x596130 Add_Member, exact with its own
  source, and ui_group's COMDAT helpers): **+13 pairs (da66757d)** in 13 minutes. Cost: **$0.36 of API credit per pair
  landed**, plus one short Opus builder to apply. not-exact.md 487 -> 479. The 4 variants that lose other pairs (0x460440
  -9, 0x5B10A0 -7, 0x6D1480 -1, 0x7451D0 -17) are not looked into. Scale-up is P-API-5 (b)'s open part.
* **Health:** clienttest 103/103 equal to expected.txt, udpreplay handshake 54/54, replay 0 differences at HEAD (D182).
* **Records:** `wt.py bring-over` lands on local master by default: it did (nothing pushed; reset, cherry-picked as
  928fe104). Traps filed in workflow.md §3.1 / §3.4. Python changed: objmatch.py, importer.py, pairs_cmp.py, gaps.py,
  triage.py, variants.py (new): the maintainer's `/verify-approved`.

### 2026-11-01 (16:30 PST, day): the POC client links, builds the login screen and runs without a crash; +789 pairs, 0 lost

Coordinator Opus 5.5. Base c27a1f0c (#139 merged; `.work/imports.c27a1f0c.json`); run branch `run/2026-11-01-day`, PR #140.
Plan log: `.work/run-2026-11-01-day/plan.md` (every report, trap and decision of the run). The user's goals: weekly usage to
~90 % by 19:00 (79 % at 11:18, 89 % at the end); wrap up items 1-4, then start the proof-of-concept client (videos ->
login -> character select against antprox's /REPLAY). HEAD against the base: **+789 pairs (app +787, wlib +2), 0 lost
outright.** 23 builders (20 Opus, 3 Sonnet; 27 rounds with GAD4 and DW1 resumed), never more than 7 at once; usage ran ~4 %/h at 6-7 builders, so stop times
were pulled in at 12:26 and the window reopened in small steps.

* **Items 1-4 (network, login, html):** P-CC2-1 landed (b36dec11: wlib takes matchbot's wdebug.cpp with E&B's revision and
  E&B's INFMSG; **0x850820, D175's last decided loss, pairs again**), CordCon::pumpClose and CordMgr::Compute_Ticks exact,
  ACK1 / Pump_Connections 0.37 / 0.38 -> 0.75 / 0.74 (CC2, NW); udpreplay byte-identical throughout (handshake 54/54). Login /
  character select: the windows' gadget callbacks are the client's SignalBaseClass::MemberCallback (new
  unknown_signalbase.h), Start_Game in as D64, every member of LoginInterface / AvatarSelectionInterface / LoginTaskClass
  defined; P-LG2-1 measured and closed (the written operator= stays) (LG). htmlview's tag parser 0x7710D0 written
  functional (D64, NOT EXACT 0.34, untested), Get_Value exact (HV).
* **The POC client (items 5-9):** WinMain 0x526DE0 byte-exact in the new `app\main\main_client.cpp` and its 14 helpers in
  `app\main\main.cpp` (MC). The ledger `.work/poc/ledger.md` (LEDGER): 5,274 functions on the core path, 1,792 missing at
  the start, a 37-item queue, antprox's /REPLAY read (it starts the client in its folder with `-PROTOCOL UDP -SERVER_ADDR a
  -GLOBAL_ADDR a`; authlogin.dll's GET /AuthLogin goes to antreplay; the exe imports authlogin.dll statically).
  `build.py link --program clientpoc` links every library with gen_stubs `--link --poc` stand-ins and deploys client.exe
  into `ClientPOC\release`; `tools/harness/clientpoc.py run` runs it and digests stubs.log (LINK). New today on the path,
  each with its own builder: intrface.cpp, gadget.cpp, gadget_list_manager.cpp (UI), gclass.cpp (GC: GClass, the client's
  name), deffont.cpp, font32bit.cpp (the real Font32Class), unknown_fontsound.cpp (FNT1), fontsys.cpp (FontStyleClass, FS),
  input.cpp, w32kbd.cpp, w32mouse.cpp, winmsg.cpp, w32window.cpp (INP), textgadg.cpp + stredit.cpp (TG), splash.cpp,
  gd_robj.cpp, gd_resize.cpp, unknown_66b770.cpp (GAD4), charview.cpp (CV: CharacterViewClass, +60),
  interface_static_sort_list.cpp (SORT1), interface_scene.cpp (IS), rect3d.cpp, fontsys_cursor.cpp, unknown_675120.cpp
  (BTN: the buttons and the text cursor), dynamic_window_background / class / render_object / horizontal / vertical.cpp
  (DW1); dynamic_window_text / multi_line_text.cpp (DW1, +177 over four rounds); local_interface_sort.cpp and the shared InterfaceSortScopeClass (LS); ui_login / ui_avsel on the real gadget, button, text-box, scope and signal-map classes. **Where it stands:** POC run 19 (16:02, d00a9ff6): the client starts, opens the real window and display, runs the font system, the menu task hands over to LoginTaskClass, CharacterViewClass builds its scenes, cameras, music and the login models, and LoginInterface is fully constructed; it runs 40 s without a crash and is killed. No screen is drawn yet: the login window's activation and update run on gen stand-ins, and FAIL_IF '!text' fontsys_cursor.cpp(86) fires twice (ui_login's / ui_avsel's local font-manager stand-ins). The startup movie completes at once (P-LINK-1); the scripted movie is skipped by its data (P-MC-1). Item 10 (antprox, D181) is not started.
* **Health:** clienttest links and passes again (103/103; CT, CT2: its LNK2005 stand-ins deleted, fakes rebuilt, expected.txt
  regenerated with every change explained); it had been broken two days unnoticed (P-CT-1), and broke again within two hours
  of the fix (the coordinator's check at HEAD 414513c5: 23 unresolved); CT3 (f1e3d0dc) links it at HEAD again (103/103,
  3,168 lines; clienttest.py takes udpreplay's Xtime / wdebug stand-ins and LIBCIMT, the exe no longer imports
  authlogin.dll); udpreplay build / run alex3 / handshake 54/54 pass at the end. The POC run found a real bug
  byte matching had hidden: combo_ffactory asked `_TheFileFactory` instead of `_TheSimpleFileFactory` and recursed (937c3ab9).
* **Decisions:** D179 (the POC's folder and modal-dialog rules), D180 (P-LG1-1 kept), D181 (P-POC-4, the user at ~12:40:
  antprox may run; HKCU edits to E&B keys allowed; unelevated). The coordinator's calls (client evidence, D137 / the
  evidence rule): P-POC-1 / P-MC-1 (no scripted movie: WinMain adds it only when moviescript1.txt exists, and ClientPOC has
  none), P-POC-2 (the 3-D avatar a stand-in), P-POC-3 (message box / dynamic windows stand-ins first); renames queued for a
  quiet window: SClient -> SpaceClientClass (P-LG4-1, the client's RTTI), InterfaceClass -> InterfaceMemberClass (P-UI-1),
  UnknownBDADC0Class -> ENBAssetManager (P-MC-3), FontStyleClass -> FontSys (§4.1); interim placements (D137): main.cpp's
  folder, unknown_signalbase.h, unknown_fontsound.cpp, winmsg.cpp, w32window.cpp, splash.cpp, unknown_66b770.cpp,
  unknown_675120.cpp, gclass.cpp's relation table, deffont.cpp's 0x684E00, fontsys.cpp's texture holder statics, the dynamic
  window row / column / element headers. New open: P-IS-1 (§3); after the handoff the user approved P-CT-1 (D182) and restored the intro movies' names (P-LINK-1 closed).
* **Records:** dee4187a's subject says +28, it is +26 (INP's verify counted stredit's pairs); ad4d5e36's subject was mangled
  by sed ("Add appientdynamic_window_..."; 6bf3c5fb gives the text). Python changes needing `/verify-approved`: build.py,
  gen_stubs.py, coff_alias.py, clientpoc.py, clienttest.py, udpreplay.py.
* **Traps:** ~90 from the reports, filed in workflow.md §3.0-§3.7 (a69a16fe and the handoff commit).
* **Next session first:** the POC's next failure (below and `.work/poc/runs.md`); then text drawing (Update_Text 0x6D64C0, 11.7
  KB 0x6CA0C0's layout core, fonttexture.cpp 0x6D8DC0); then item 10 against antprox (D181). The queue: `.work/poc/ledger.md`
  and §4.1's POC bullet.
* **Outside git:** `.work/run-2026-11-01-day/` (plan log, briefs, traps/), `.work/poc/` (ledger, runs.md, antlog), builders'
  scratch `.work/<id>/`.

### 2026-11-01 (09:30 PST, night): the network stack round, login and character select started; +314 pairs, 0 lost

Coordinator Opus 5.5. Base a230790d (#138 merged; `.work/imports.a230790d.json`); run branch `run/2026-11-01`, PR #139.
Plan log: `.work/run-2026-11-01/plan.md`. The user's goals: weekly usage to 80 % by 07:00 (69 % at 00:53, 79 % at
08:55); a soft goal of the client's network stack exact, then start-up, login and character selection. HEAD against
the base: **+314 pairs (app +294, wnet +20), 0 lost outright, no moves.** Daylight saving ended at 02:00 PDT: times
below are PST from then on.

* **Network stack (CC1, CC2, NW1-NW7, FB1; about 55 functions not exact at the start):** D176 landed (CordCon's 0xC0
  layout, an `__int64` key, the harness counters in `tests/udpreplay/cord_counters.h`); CordCon::write takes the
  client's `(channel, len, data, priority)`; cordcon.cpp in the client's function order with FAIL_IF / INFMSG lines
  in place; the CordMgr constructors, Init, Listen, Wait, Unlisten, ~CordMgr, Find_Connection, Cancel_Connect and
  the go-away sender exact; app\wwudp on wnet's E&B udp.h (P-NW4-1); wnet's E&B udp.cpp complete and tcpmgr.cpp's
  revision (+20); E&B's ArrayList::getPointer sets NULL on a miss (P-NW2-2) and tcpmgr.h's `FAILED = -1` (P-NW2-1).
  Two functional fixes from the client: CordCon clears `GoneAway_` on receive, and pumpClose gives up after 90
  seconds. udpreplay output unchanged throughout (run / loop / conn alex3, handshake 54/54). **Not met:** about 25
  functions remain (D99 rows aside), all partial or parked: cordcon's write / read / handlePacket / examinePacket /
  buildHeader / pumpClose / close / openc / pumpInternal / Cord_Get_Seq / Copy, CordMgr's Pump / Pump_Connections /
  Go_Away / ACK1 / ACK2 / Compute_Ticks, udpconn's Send_Data and 0x82CBF0, tcpconn's Key_Exchange and Listen, wnet's
  pumpConnections. FB1's Fable round (D89) parked Send_Data and pumpConnections (LIMIT REACHED).
* **Start-up, login, character select (LG1-LG3):** new `ui_login.cpp` (LoginInterface, 15 of 16 exact, +31), new
  `ui_avsel.cpp` / `ui_avsel.h` (AvatarSelectionInterface, base InterfaceClass, 20 written, 17 exact, +23);
  kernel.cpp / tsk_login.cpp +6 and 6 D99 rows; the rest of tsk_login is register / shape residue.
* **Other files:** galaxy_map.cpp complete (35 -> 136 with its iterators; GM1, GM2) and the new galaxy_map_client.cpp
  (+15); html: new `unknown_htmlattrib.cpp` (HtmlAttributeStackClass, all exact: **D175's three Wstring losses
  0x77AA10 / 0x77B030 / 0x77B690 pair again**), `htmlview.cpp` / `htmlview.h` (HtmlViewClass) and
  `unknown_htmlblock.cpp` (HtmlBlockClass) (+76; HV1, HV2); vendor_defs.cpp, weapontype.cpp, unknown_verb.cpp (NW4).
  D175's fourth loss 0x850820 comes back with E&B's INFMSG (P-CC2-1, drafted outside the tree).
* **Decisions:** D178 (P-NW3-1, the user at ~08:50). New open: P-LG1-1, P-LG2-1, P-VM-2 (§3). Interim placements
  (D137, the coordinator's): unknown_htmlattrib.cpp, unknown_htmlblock.cpp, galaxy_map_client.h, ui_avsel.h,
  wnet udp.cpp's direct includes of <string> and permstr.h.
* **Process:** the coordinator's question to the user (P-NW3-1) held the session from ~02:40 to ~08:50: heartbeats and
  launches stopped while every builder finished, and the 07:00 handoff slipped to ~09:30. Ask through the run's log
  and the report instead of a blocking dialog during an unattended run. Seven VMs for nine or ten builders saturated
  the pool and the importer lock from ~01:30 (20-30 minute waits; workflow.md §3.5). The coordinator fixed HEAD's
  udpreplay link (79859ca7: the Value_To_String(VerbEnum) stub). commit_mine swept neighbouring rows between
  builders several times (documents only; not repaired).
* **D178 landed (NW8):** NW3's wt/nw3-p1 brought onto HEAD (da2d4d2d): CordMgr +0x58 is MaxPerSocket_, the harness count in udpreplay; Connect 0x858EC0 and the new Set_Buffers 0x8578C0 (NAME 2) exact, growVector 0x85BC80: app +3, 0 lost, replay 0, udpreplay byte-identical. P-CC2-2: udpreplay.py links unknown_verb.cpp (1e7082c3, a Python change: the PR needs `/verify-approved`). Worktree `.work/wt/nw3` and branch `wt/nw3-p1` are fully landed and left for the user to remove (`python tools/vc6/wt.py remove nw3 --delete-branch`; the builder's removal was refused by the permission check).
* **Traps:** about 60 from the reports, filed in workflow.md §3.0-§3.9 (2026-11-01 bullets).
* **Next session first:** P-CC2-1 (E&B's INFMSG and MsgManager streams in wlib, with stubs for udpreplay / clienttest:
  brings 0x850820 back and unblocks cordcon's four log-bearing functions); update D175's notes in
  `docs/data/accepted-corrections.json` (three of the four pair again); the user's calls on P-LG1-1 / P-LG2-1 /
  P-VM-2; then §4's network and login bullets.
* **Outside git:** `.work/run-2026-11-01/` (plan log, briefs, traps), `.work/<builder>/` scratch (cc1, cc2, nw1-nw8,
  hv1, hv2, gm1, gm2, lg1-lg3, fb1).

### 2026-11-01 (D175): the decided-loss tools PR (#137) and wlib's configfile / xtime revision: +11, 4 decided losses

Opus 5.5, with the user. Base edbe036d (#137 merged); branch `run/2026-11-01-wlib`.

* **#137 (tools/decided-losses, merged):** an accepted-corrections entry with `moved_to: null` is a decided loss:
  pairs_cmp.py lists it under `corrected (accepted: D…)` with no gain required, verify_pr.py names it `(decided loss)`;
  D175's four wlib `configfile` entries (0x77AA10, 0x77B030, 0x77B690, 0x850820) are in the list.
* **wlib (D175):** E&B's configfile.cpp + xtime.cpp revision from `docs/attempts/8B2810/` (8 patches in
  WWONLINE_PATCHES); verify --lib wlib --lib wnet on 2 pool VMs: **+11 (configfile 10, xtime 0x8BAA50), 0 lost,
  4 corrected (accepted: D175)**; replay 0 differences (configfile's banner copied from `.work/replay`, workflow.md
  §3.9's trap). Only wlib's own files include configfile.h / xtime.h.
* **Where the four come back:** 0x850820 (`ostream::operator<<(manip)`) with cordcon.cpp's callers (pumpClose
  0x850260, handlePacket 0x851AE0, pumpInternal 0x853A30; cordmgr's Pump_Releases 0x859940); 0x77AA10 / 0x77B030 /
  0x77B690 (`ArrayList<Wstring>` / `Dictionary<Wstring,Wstring>` instances) with an unattributed app file at
  0x7772C0-0x77B7D0 (callers 0x779520, 0x779860; just before avatar.cpp 0x77B870; no assert or name evidence yet).
* **Next:** D176 (CordCon's 0xC0 layout, udpreplay's counters outside), then §4's t4 leads; the 0x7772C0 file is a
  new lead.

### 2026-11-01 (00:15): P-GH-11 landed (claims in the run PR body), symbols.json regenerated, D174's tie-break: +211, 0 lost

Coordinator Opus 5.5, the 2026-10-31 night session continued after the user's three asks: resolve PR #78, cut the
GitHub API volume and the claim emails, and settle the open proposals. Base d2df79bb (#78 and #135 merged;
`.work/imports.d2df79bb.json`); run branch `run/2026-11-01`, PR #136.

* **PR #78** (the night run, +212) merged after `/accept-d66` (TK5's move); the symbols.json regeneration was taken
  out of it (vc6-verify counted D173's corrected pair as lost outright).
* **P-GH-11 (#135, GH1):** maintainer runs claim in the run PR's body (`<!-- enb-run-claims -->`, `run_branch.py claim`
  / `release`; no issues, no emails); the Claims Action writes the live state to `claims-state` and `list` / `check` read
  it in one call (5-minute cache); claim waits back off; a 500-call budget floor; Checks run on master pushes and ready
  PRs only, a newer push cancelling the older run; `docs/data/accepted-corrections.json` (decisions only) read by
  pairs_cmp / verify_pr from master. Live-tested: a claim and its release in #136's body, no issue. The 2026-10-31 run
  burned the 5,000/hour budget (claims.py's per-issue scans) and opened 46 claim issues.
* **symbols.json** regenerated (79c13952): +6, 0x723AB0 -> 0x4B2AA0 `corrected (accepted: D173)`, 0 lost.
* **D174 (TB1, e94b3b1e):** objmatch's caller tie-break takes non-identical copies, D80's survivors, repeats the
  rounds until nothing more is credited, credits one of an object's folded symbols, and counts an EH funclet's call as
  its parents' (13,088 funclets mapped from the FuncInfo stubs): **+205 (app +191, ww3d2 +13, wnet +1), 0 lost**.
  Funclet-called copies whose parent is not exact yet wait for it (fontdatabase 0x6C6E20 on 0x6BEA90).
* **Decisions:** D173-D177 (the user). Open: P-VM-1 (later; deadline 2027-01-20).
* **Next session first:** D175 (P-TK7-1) in two PRs: a tools PR letting an accepted-corrections entry with
  `moved_to: null` pass as a decided loss (with D175's four addresses), then wlib's configfile / xtime revision from
  `docs/attempts/8B2810/` (+11). Then D176 (CordCon's 0xC0 layout, udpreplay's counters outside), then the leads in
  §4's t4 bullets. Builders: claims are in the run PR body now (builders.md, coordinator-pacing).

### 2026-10-31 (22:05, night): the triage plan's runs, 17 Opus builders; +212 pairs, 0 lost; symbols.json regeneration ready (D173)

Coordinator Opus 5.5. Base 20f0bf21 (PR #65 merged; `.work/imports.20f0bf21.json`); run branch `run/2026-10-31-t4`,
PR #78. Plan log: `.work/run-2026-10-31-t4/plan.md`. The user's goal: keep building, weekly usage to 70 % within five
hours (63 % at 17:50, 68 % at the handoff). HEAD against the base: **+212 pairs (app +143 written, +23 by tie-break;
wlib +46), 0 lost outright**; the one move, 0x8C0FD0 delta_money -> unknown_login_data, is D66 (iii) / (v) (TK5).

* **The previous session's handoff (the user's request at 18:05):** its 18:45 entry was an uncommitted hunk in the
  tree, not on master (first misread as landed): committed here (e3f6a984). The rest of it was clean.
* **Builders (TK1-TK19, Opus, each a few of the triage plan's runs or a lead from an earlier report):**
  control_keys_manager (TK1, +7 and +13 tie-break; P-H11-2 solved: a hash_map value's private destructor with the map
  as friend), attrib_dir (TK2, +6), fontdatabase / fontmanager (TK3, +7), wlib wtime.cpp complete from RenegadeGR's
  copy and wlib built with /GX (TK4, +27), lookup_table / delta_money / policy (TK5, +16), ui_avatar_cust and the new
  ui_avatr.cpp (TK6 / TK14 / TK15: 4 -> 32 and 0 -> 19; class names from the client's RTTI), wstring.cpp's E&B
  revision (TK7, +19), cordcon (TK8 27 -> 48; TK17 forms only), landdmap / pkupdate (TK9 +9, TK18 +1), ui_line_display /
  tutorial_manager (TK10, +13), dbgconn (TK11, +5), the new galaxy_map.cpp (TK12 / TK16, 0 -> 22), pkgadget /
  client_lego_impl (TK13, +1 and 16 D99 rows), proj_man (TK19, +3). Hint verdicts: right in every run but one
  (ui_avatar_dialog was ui_avatr.cpp by its __FILE__).
* **Proposals for the user (§3):** P-TK3-1 / P-TK2-1 / P-TK5-1 (objmatch's caller tie-break over non-identical copies,
  D80's survivors and folded symbols: +82 and +45 measured separately, 0 lost; land together after a full verify),
  P-TK3-2 (EH funclets count as their parent's callers), P-TK7-1 (E&B's configfile / xtime revision: wlib +11 for 4
  app template instances lost). P-TG4-2 updated (five dead finds).
* **symbols.json (the previous handoff's first item):** on the quiet tree, a full verify recorded (292e9aa6: ui_avatr's
  15 pairs were missing from imports.json). The regeneration with every library's objects gave +6 and lost 0x723AB0
  outright (KVAccessorClass<UnknownB016DCType>'s ??_G, paired by D171's learned-slot self name until symbols.json
  names it exact: AR1's class). SY1 found the loss is a false pair moving to its true address 0x4B2AA0 (the client's vtable layout); the user accepted it (D173). The regeneration (+6, 0x723AB0 corrected; dc424b62) was then taken out of the run's PR
  (b980c8e9, the user's choice): vc6-verify counts 0x723AB0 as lost outright and runs master's tools, so it lands in a
  follow-up PR once master's verify reads accepted corrections (P-GH-11's tools PR). `.work/pseudo/` was rebuilt from it. A regeneration feeds a run's late phases into its early ones (SY1's trap, workflow.md §3.4). symbols.py without `--objs` reads no objects (TK13's trap, hit
  again here).
* **Process:** GitHub's API rate limit (20:22-20:32) blocked claims and the PR sync; claims hang after it. Claims
  released unused: #98, #99, #111, #116, #124, #128, #133, #134.
* **Traps:** about 30 from the builders, in workflow.md §3.1-§3.9 (LC1's literal trap corrected: literals are merged
  across objects).
* **Next session first:** the user's calls on P-TK3-1 / P-TK2-1 / P-TK5-1 (one tie-break change, +82 / +45 measured,
  landed after a full verify) and P-TK7-1; then the leads in §4's t4 bullets (ui_avatr / ui_avatar_cust, galaxy_map,
  cordcon, landdmap) and the triage plan's remaining runs.
* **Outside git:** `.work/run-2026-10-31-t4/` (plan log), `.work/sym2/` (the regeneration's verifies and objects),
  `.work/tk*/`, `.work/sy1/` (builders' scratch: forms, patches, measurements named in §3 / §4).

### 2026-10-31 (18:45): handoff; D172 landed (the triage table tracked); master 20f0bf21

* **State:** master 20f0bf21 holds the day's three runs (PR #41 +1,102, #64 +30, #65 D172); the tree is clean,
  no VM leased, the pool stopped. Local branches cleaned with `run_branch.py cleanup` (wt/pins kept: its own M4
  measurement commit). Claims #58-#63 close at the Claims Action's next reconcile (their PR merged).
* **Decisions today:** D167-D172 (the user). Open for the user (§3): P-VM-1 (the pool's VM base before 2027-01-20),
  P-TG3-1 (CordCon's harness counters out of the class: recommend yes), P-TG4-2 (D130's dead spenders kept for
  another function's pair: recommend keep).
* **Usage:** weekly 63 % at the handoff; D163's API credits about $43.40 of $200 spent (`ENB_ANTHROPIC_API_KEY`,
  workspace `enb-d163`, limit $60).
* **Next session first:** regenerate symbols.json on the quiet tree to record D171's +841 (workflow.md §3.4: full
  verify of every library before and after, pairs_cmp gain-only, then `.work/pseudo/` rebuilt), then the triage
  plan's runs (§4) under D172's reading rule.
* **Outside git:** `.work/triage/` (the batch ids, raw results, plan.md; the results expire 29 days after
  2026-10-31), `.work/sym/` (the first regeneration's objects and copies), `.work/run-2026-10-31-api/` and
  `.work/run-2026-10-31-t2/` (plan logs). No processes running.

### 2026-10-31 (18:05, evening): the triage's second test (boundary runs, 18 of 23); +30 pairs, 0 lost; branch cleanup

Coordinator Opus 5.5, two Opus builders. Base a581d2cd (PR #41 merged; `.work/imports.a581d2cd.json`); run branch
`run/2026-10-31-t2`, PR #64. Plan log: `.work/run-2026-10-31-t2/plan.md`. The user asked for one more run to firm up
the triage's score (D168, P-API-4) before finalizing it. HEAD against the baseline: +30 pairs, 0 lost. Weekly 62 % ->
63 %.

* **The test:** six triage runs between two *different* files (the hint had to choose), 23 functions; the builders
  decided each file from the client's own evidence before reading the hint. Result: right 18 of 23 (15 proven by a
  pair); the 5 misses are one object no evidence names (the screen fade, now unknown_screenfade.cpp). Accepted with
  its reading rule as D172 (the table tracked in `docs/data/triage.*`).
* **TG3** (#58-#60, +16): CordMgr's SYN1 0x85A3B0 / SYN2 0x85AB30 exact (the hint right 5/5 for the run, wrong
  kind on two); new app/client/unknown_screenfade.cpp, 10 exact (0x7FF010-0x7FFBE0, 0x42F250; the hint said
  scriptedmovie: wrong 5/5); sbaseview 0x69CBE0 / 0x69CC10 / 0x69CCD0 / 0x69CDE0 exact (right 3/3). P-TG3-1 open.
* **TG4** (#61-#63, +14): three files the client names by its asserts started: ui_combobox.cpp (0x57A330,
  0x57AAB0, 0x57AC70, 0x57AD50 exact), cameraeffects.cpp (0x6E3710, 0x6E37F0, 0x6E3B60 exact), ui_avatar_cust.cpp
  (0x550670, 0x550970 exact, 0x550A10 NOT EXACT register); the hint right 10/10. P-TG4-2 open; P-TG4-1 (ui_chat_panel.h's
  UIChatComboClass is UIComboBoxClass: move it into a ui_combobox.h) is a placement, the coordinator's (D137): accepted,
  at the files' next edit (§4 item 1).
* **Branch cleanup (the user's request):** 16 stale local branches deleted (all merged or the same patch as a commit
  in master), local master fast-forwarded, wt/pins kept (its own M4 measurement); the remote had nothing left over.
  `run_branch.py cleanup` (6e3fb718) does this at each handoff now (coordinator-pacing skill).
* **Traps:** TG3's and TG4's in workflow.md §3.3.

### 2026-10-31 (15:40, day): D163's triage done (D167-D170), D169's /accept-d66, D171 landed; +1,102 pairs, 0 lost

Coordinator Opus 5.5, up to three Opus builders at a time. Base d290dff0 (PRs #39 and #40 merged;
`.work/imports.d290dff0.json`); run branch `run/2026-10-31-api`, PR #41. Plan log: `.work/run-2026-10-31-api/plan.md`.
The user's goal: work §4's open items until the weekly usage reached 70 %; at 14:20 the user accepted P-AR1-1 (D171)
and stopped new work, asking for the symbols.json regeneration and then D171. HEAD against the baseline: **+1,102
pairs** (app 423, ww3d2 403, wwaudio 142, wwlib 53, wwsaveload 29, binkmovie 24, wwmath 12, wwshade 9, wwdebug 6), 0
lost, 0 moves. Weekly usage 59 % -> 62 %.

* **Decisions:** D169 (`/accept-d66 SHA` passes a PR's D66 moves through vc6-verify; PR #40, the first use on #39),
  D170 (the triage's full run on Sonnet 5.5 over the application range), D171 (P-AR1-1, objmatch's self-name round
  with the vtables it learns). P-API-4 (D168's revisit) is open in §3.
* **D163's triage (D167, D170):** `.work/pseudo/` rebuilt (20,588 functions); `tools/re/triage.py` (build, submit,
  status, fetch, ingest, calib, plan, score, show). Calibration on 300 redacted known answers: Haiku 5.5 kind 79.7 %,
  file 40.0 % (failed D167's gate), Sonnet 5.5 89.3 % / 63.2 % (passed). The full run: 14,753 answers, 0 errors, about
  $40.70 (with the calibration $41.57, with the pilot about $43.40 of the $200). `.work/triage/plan.md`: 161 runs of
  3+ neighbouring functions with no pair and one suggested file (72 agreeing with both neighbours), 2,735 template
  instances, 379 library kinds in the app range. TG1 and TG2 built six of the runs: the hinted file right 23 of 23;
  `score` after the run: 1,824 hinted functions exact, file right high 494/494, medium 1,206/1,251, low 58/79.
  builders.md points at `triage.py show` (a hint, not evidence).
* **Builders** (all Opus; traps filed in workflow.md §3.1-§3.6):
  * UG2 (ui_group.cpp, claims #42 / #43): +3: 0x598370 exact, 0x596770 / 0x592C50 / 0x595530 / 0x5946D0 written NOT
    EXACT; 0x597E40 was exact through 13 dead reads, reverted under D130 (afaac70a, the limit is 6 per function).
  * MU2 (ui_manu.cpp, rs_track.cpp, #44 / #45): +17: rs_track 0x462610 / 0x462650 / 0x462690 exact (Unknown462690Class,
    a CullableClass), ui_manu 0x5B4B10 / 0x5B5320 / 0x5B5CD0 NOT EXACT (inline budget), 0x5B6060 exact.
  * TG1 (triage runs avatar_desc 0x4C1B80-0x4C2BB0, sclient 0x741BB0-0x7427C0, #46 / #47): +9: avatar_desc's seven
    Get_*_Offset (Vector2 by value) and Save_Defaults 0x4C2360 exact, SClient 0x742530 exact; Shutdown 0x741BB0 NOT EXACT.
  * AR1 (aux_roots' unpaired small functions, #48-#50): the 68 split as 0 not emitted, 4 different, 64 identical but
    uncounted (46 relocation-only ties of unnamed template vtables, 18 short `??1`); +4 (0xB01A40 = ProjectedDecalClass's
    conversions, pad 15) and P-AR1-1. Round 2 (the user's order): symbols.json regenerated gain-only, +116
    (3021179b: symbols.py's `same()` treats `??_E` / `??_G` of one class as one symbol, D115 registers known
    initialiser targets; my first regeneration had lost 268 outright and was not kept), then D171, +841 (69f21f4b; a
    learned slot never names an address symbols.json gives another function). `.work/pseudo/` rebuilt (18,981).
  * AR2 (aux_roots' leads, #55-#57): +103: 0x709A90, the client's names for eight generated value types (NAME 5 from
    their KVPairs' Get_Type_Name), out-of-line enum conversions (ENUM_EXTERN, new generated stubs/aux_roots_stubs.cpp),
    22 identical functions named by vtable slot; pad 10.
  * CT1 (UG2's proposal (b), #51): ~CTA_Request_PacketClass 0x86F1C0 out of line, byte-identical (D99 row);
    0x5946D0 still NOT EXACT 0.55 (inline budget).
  * TG2 (triage runs land / heightfield / landdmap, #52-#54): +9: landdmap's helpers 0x4676D0-0x467780 and 0x4677B0,
    Apply_Colors 0x46D410, Get_Triangles 0x465D30 (was Fable-eligible), 0x466140 moved to heightfield.cpp
    (Get_Height_Byte) exact; 0x466F60 / 0x46D6B0 / 0x4654E0 NOT EXACT.
* **Coordinator:** progress.md §4 rewritten to outstanding work only (9f9ac26d); vidtest.cpp's stale viddeco.h include
  dropped; the D163 key setup (`ENB_ANTHROPIC_API_KEY`, never `ANTHROPIC_API_KEY`).
* **Placement (D137, coordinator):** UIGroupMemberUnknown2CClass (ui_group.h) and ui_cpit.h's UICpitUnknownD0Class are
  one class (ctor 0x5DBFC0, 0x28 bytes): merge into ui_cpit.h at either file's next edit (§4 item 1).
* **Usage:** weekly 59 % -> 62 % (about 2.2 M builder tokens across nine builder rounds).

### 2026-10-31 (10:20): D163 (a), the API-key pilot: LC1 +2 pairs for $1.85; scriptedmovieline.cpp

Coordinator Opus 5.5 on the subscription; one builder headless (`claude -p`, Opus 5.5, `--permission-mode auto`,
`--max-budget-usd 25`) on the separate key `ENB_ANTHROPIC_API_KEY`. PR #37 had merged, so a new run branch
`run/2026-10-31-pm` from 84c61e2c (`run_branch.py start --date 2026-10-31-pm`), draft PR #39, claim #38. Plan log:
`.work/run-2026-10-31-pm/plan.md` (launcher `launch_LC1.sh`, stream `LC1.jsonl`). f3c15d2a (D167, D168), committed
on the local run/2026-10-31 after PR #37 merged, cherry-picked here.

* **LC1 (ff4ffbbe, P-SM1-3 and P-SM1-1):** the line class 0x7FFC50-0x7FFE9F in its own file
  app/client/scriptedmovieline.cpp (NAME 1, provisional: no asserts), linked just before scriptedmovie.cpp;
  ScriptedMovieParamsClass merged into ScriptedMovieLineClass. Against 84c61e2c: +2 (0x7FFC70, 0x7FFCB0), two
  D95(c) moves (0x7FFD30, 0x7FFDC0), lost outright none; replay 0. 0x7FFC50 exact but its 13 bytes occur at five
  other addresses (D99). 0x7FFEA0 stays in scriptedmovie.cpp (it shares the 'CREATE_OBJECT' literal with 0x803310).
  0x803310 still NOT EXACT (inline budget 0.63): the next lead, under scriptedmovie's claim.
* **The pilot's numbers (D163 (a)):** 63 turns, 9.5 minutes, $1.85 at list price, about $0.93 per pair gained.
  Tokens: input 118, cache write 96,845, cache read 4,327,669, output 25,169 (thinking 8,857). The builder's own
  estimate was "~400k tokens", about a tenth of the 4.45M with cache reads: the self-reported token figures in
  earlier plan logs (LD6 ~400k for +19, LD7 ~250k for +5) are the same kind of estimate, so compare runs by the
  stream's `total_cost_usd` / `modelUsage`, not by builders' reports. A headless builder needs no Agent tool:
  the launcher script, the brief file and the stream's result line were enough.
* **Traps:** LC1's in workflow.md §3.3 (62269192).
* **Usage:** the subscription's week stayed at 59 %.

### 2026-10-31 (09:15, morning): D164-D166; FAIL_IF's && form (+6), the caller tie-break (+317 pairs), the progress card fixed

Coordinator Opus 5.5 with two builders, one at a time. Base 56b6c79e (PR #13 merged; `.work/imports.56b6c79e.json`); run
branch `run/2026-10-31`, PR #37. Plan log: `.work/run-2026-10-31/plan.md`. HEAD against the baseline: +323 pairs, nothing
lost outright.

* **Decisions:** D164 (P-MU1-1, explicit instantiations for out-of-line template copies), D165 (P-SM2-1, FAIL_IF's
  `&&` form), D166 (the caller tie-break; D66 (vii)). D163 (the API credits) was not started: still first in §4.
* **FI1 (Opus, 2943f972, D165):** the `&&` FAIL_IF in unknown_debug.h, app 5,127 -> 5,133 (+6: scriptedmovie 0x803310,
  co 0x6FEC40, ini_manager 0x4EB1B0, px 0x44C2E0 / 0x44C520 / 0x44C690), 0 lost. Four knock-ons fixed in their own files
  (no per-file macro): client_lego_impl 0x72D5F0 (`break` for `return`), co 0x6FFFF0 and sclient 0x750260 (one dead
  inline call each to restore the inline budget the cheaper macro freed), ui_cpit 0x57F240 (one more implicit key name).
* **TB1 (Sonnet, cf19989a + 89b35501, D166):** objmatch's `caller_tiebreak` (verify / compile_and_match only, not
  `appfile.py match`), a `tiebreak` list per file in imports.json (also in `in_tree`), pairs_cmp's tie-break lines. Full
  verify of all 11 libraries twice, identical output: +317 pairs (app 259, ww3d2 40, wwaudio 7, wwlib 5, wnet 3, wwmath 2,
  wwsaveload 1), ambiguous 340 -> 315, replay 0 files differ. Bytes unchanged (identical copies already counted as exact
  bytes). 0x584C80 / 0x47CF50 stay ambiguous until their callers 0x57DD20 (ui_cpit) / 0x47A2F0 (land) are exact.
  0x62A5D0 is held by pkcolorization and scriptedmovie (D20; kept, TB1's stricter variant not taken). pacing.py's
  pairs_cmp parser fixed for the new lines (d7a7b262).
* **Progress card (the user's request):** the daily job was dropped in cff55ff and nothing replaced it, so the card stood
  at 2026-10-29. `run_branch.py ready` now regenerates and commits it (`--co-author` for D71). 'Bytes rebuilt' = byte-exact
  + CRT / third-party + linker output, those three in greens first (db82df8d): 46.50% rebuilt (30.62% byte-exact),
  34.34% written in C++ (52c8e4d4). The user's question answered in the session: the outstanding work is unattributed +
  NOT EXACT + stubbed (~53.5% of .text); runtime and linker output come back with the toolchain (EH stubs with their
  functions).
* **Traps:** FI1's in workflow.md §3.1 / §3.3 (88460e4a), TB1's in §3.4 (cf19989a, d7a7b262).
* **Usage:** weekly 58 % -> 59 %.

### 2026-10-31 (03:45, overnight): the quiet tree closed, land item 5's next round, seven new files; app 4,761 -> 5,127 in tree, +475 pairs, 0 lost

Coordinator Opus 5.5, one to three Opus builders at a time (pacing to the user's ~60 % of the week by 07:00: 50 % at
18:40). Base c0913c2b (`.work/imports.c0913c2b.json`); run branch `run/2026-10-30`, draft PR #13. Plan log:
`.work/run-2026-10-30/plan.md`. Every library verified gains-only throughout; no address lost outright against master.

* **Quiet tree (§4 item 1) closed.** Most of it was already done on 10-27 (D94(b), D95(b), D100, park45 / park117, the
  packets' comments, names-review). New: symbols.json's 0xB006F4 is ClientObjectClass's MultiFixedPoolDecalSystemClass
  vtable (379c29da; symbols_named.json entries take `kind` and `replaces`); D107 done (QT1 bb502e83: the
  `Unknown_BE8B38Class` views are OptionClass, 0 / 0); P-H7-1 (f918e190): sgistl_adapt.py drops `typename` from the
  pair<> return types, so `hash_map::insert` compiles, but the two explicit specializations stay (removing them loses 13
  app pairs); a full verify of all 11 libraries left imports.json unchanged.
* **Land (§4 item 5).** LD6 (dd60da4c..a1a48eb6, +19 pairs): the unreferenced out-of-line copies of inlined helpers
  0x46EE60..0x46F0A0 / 0x470EF0 / 0x474590, 0x46E120, Build_Index_Buffers 0x478240, Save_Decorations 0x479A50, the
  decoration ini reader's string helpers (+7). LD7 (55f1d2c5, +5): Render's sea Adds as a macro and its world
  transforms as `Set_Transform(D3DTS_WORLD, Matrix4(true))`: 0x47D3D0, 0x47F300 (one of the two land copies accepted
  as lost on 10-27, back), 0x47F3F0, 0x47F470, 0x47F4E0; Render NOT EXACT (2,622 -> 2,222 rows; the client's is ~5,600
  instructions to our 2,400: structural, LD8). P-LD6-1's inline-helper lever gained nothing on 0x47A2F0 (its insert
  0x47D690 is byte-identical but `multiple` in the client) or 0x47C6A0 (7 forms). LD8 (eb8fdfe9..fce1f069, +7):
  heightfield's Get_Height 0x464600, Load_PGM 0x4637C0, insert 0x466180 and the vector helpers 0x4667D0-0x4668A0;
  Get_Downhill / Get_Normal / Get_Triangles / Cast_OBBox residues down; Load 0x4634F0 a LIMIT candidate. VIDTEST
  drawcheck PASS at frame 900 each time, sbs 1.74-1.97 %.
* **New files from the client's __FILE__ asserts:** rs_track.cpp (RT1, 0 -> 102, 83 of 86; RsTrackClass moved into
  rs_track.h from co.cpp / cpacket.cpp); proj_man.cpp (PM1, 0 -> 61: Renegade's pscene_projectors.cpp code in a
  ProjectorManagerClass, StaticShadowTexMgrClass; only 0x493620 parked); ui_group.cpp (UG1, 0 -> 52); ui_cpit.cpp (CP1,
  0 -> 62, UICpitClass into ui_cpit.h, Unknown5BB320Class moved to sclient.h); ui_manu.cpp (MU1, 0 -> 34); ui_scut.cpp (SC2, 0 -> 40, UIScutClass out of ui_cpit.h); ui_chat_panel.cpp (CH1, 39 exact, UIChatPanelBaseClass in its header for now).
  scriptedmovie.cpp continued: SM1 +28 (19 functions, two of UI1's parked rows), SM2 +12 (ctor 0x8000C0, Create_Light
  0x805B60, Play_Audio, the three effect commands).
* **GitHub:** the user made EnB and `.github` public; the coordinator applied, at the user's request, the master
  ruleset (`Protect master (D148)`, `checks` + `vc6-verify` required), private vulnerability reporting and secret
  scanning with push protection (github-plan.md §4.5.7). The Claims Action refused a fourth maintainer claim while
  the user's org membership was private (the token saw no MEMBER); the user made it public and later claims passed.
* **Decisions:** D163 (P-API-1, the API credits: pilot builder, then a Batch API triage; first item of the next
  session). P-SM2-1 accepted 2026-10-31 as D165 (FAIL_IF's form). P-MU1-1 accepted 2026-10-31 as D164
  (explicit instantiations for out-of-line template copies).
* **For the user to confirm:** 0x584C80 (CP1's 7f6f84e5 credited it to ui_cpit, 9d9b074c unattributed it: our
  Create_Get<FontStyleClass> is byte-identical to the client's Create_NEW there). Not in master's baseline, so nothing
  is lost against master; CP1 called it D66(vi), but it is not in any ambiguous list.
* **Placements (D137, coordinator):** Unknown5BB320Class in sclient.h (CP1); ui_group's +0xE8 object 0x5927E0-0x593180
  in ui_group.cpp (UG1); UnknownAEEA88Class in ui_scut.h (SC2); rs_track.h's interim rs_list / rs_proj_sup classes (RT1). P-SM1-3 / SM2: the line class
  0x7FFC50-0x7FFDC0 (with 0x7FFD30 / 0x7FFDC0) gets its own file between option.cpp and scriptedmovie.cpp at
  scriptedmovie's next edit (removes the auto_inline pair around 0x7FFD30).
* **Traps:** about 45 rows and bullets filed in workflow.md §3.3, 3.4, 3.5, 3.6, 3.9 (b58b4cd1, d07ef55b, 76d90b1d,
  ff6451d2, b013383b, 95699838, 373934e4, 080b1a6b, c38ea9e3, ef6397da, f341b852, c5724b6f). Incident: more than four parallel
  `appfile.py match --pool` jobs half-created the clone enb-vc6-07 (destroyed).
* **Usage:** the week 50 % -> 58 % (the user's target ~60 % by 07:00; the coordinator's own work and the session window
  on top); 13 builder runs, all Opus, ~0.1-1M tokens each (the plan log has each). Final verify of all 11 libraries at
  15c75b71: 0 / 0 against the committed imports.json, replay 0 files differ.

### 2026-10-30 (evening): the Character and Starship Creator analysed (CSC.md); D160-D162

Interactive session with the user, no builders, no tree code changed. EA's promotional Character and Starship
Creator (2002-09-13) analysed: [CSC.md](CSC.md). It holds about 2.4 MB of the client's own front end as it stood
at launch, built from `C:\Projects\G` with VC6 RTM and linked non-incrementally with `/OPT:REF,ICF` (§1.1).
Byte-exact rebuild: no (RTM toolchain, 2002 libraries, 267 of its 343 app files not in the tree); functional:
later, once the client's character-creation screens exist. Findings: four new source file names and the
`Tools\Avatar Creator` folder; 98 tree functions exact in CSC but absent from the client (e.g. `DynObj::DynObj()`
0x44AC70, NOT EXACT); the creator file writer that the client's `unknown_creator.cpp` reads; 2,143 masked 1:1
function pairs (701 file attributions agree, 4 disagree, 22 new). No provisional `unknown_*` file settled. The
user accepted P-CSC-1..3 (D160-D162); D161 applied to source-tree.md and toolchain.md §1.2. Scratch in
`.work/csc/` (Ghidra project `%LOCALAPPDATA%\EnB\ghidra_csc`).
* **Left:** D160(a), listing the 98 against not-exact.md / the attempts log so builders see the 2002 evidence
  before a retry.

### 2026-10-30 (day): the repository on GitHub (Open-EnB/EnB, private); claims and the verify service; the final public read; D153-D159

Interactive session with the user, no builders. The repository went up to GitHub private (D152), after the
history rewrite that removed the private reference decoder (D153); EA's reference traces froze into a release
asset (D154); the test programs became siblings under tests/ and capharness retired (D155).

* **P-GH-10, coordinating outside contributors' agents** ([github-plan.md](github-plan.md) §4): claims are
  GitHub issues kept by the Claims workflow (`tools/claims.py`, D156); every PR is verified before master by a
  polling service on the maintainer's host (`tools/vc6/verify_watch.py` as the scheduled task "EnB verify watch",
  `verify_pr.py`; no self-hosted runner), posting the `vc6-verify` check; contributors' PRs land through
  `verified/prN` landing PRs (D157); the coordinator's run branch, draft PR and claims go through
  `tools/run_branch.py`. Live on master since PR #1; the first automatic verify ran on PR #8. The rehearsal waits
  for the first outside contributor; automerge stays off until it has passed.
* **The final read before going public** (PR #9): every hand-written doc read; personal and machine detail
  removed, the earlier unpublished research described generically, conduct reports through GitHub's private
  reporting, the SDK copies recorded with SHA-256 hashes (D5), stale statements updated.
* **File headers (D158)** and **unknown members (D159)**: about 330 of our files now start with `// Earth &
  Beyond client (reconstruction)`; 157 imported files changed for E&B carry importer.py's note; 590 uses of
  `m_UnknownXX` became `UnknownXX` (WWAudio's kept). Full verify gained [] lost [], imports.json unchanged, replay
  0 files differ.
* **Org profile:** `Open-EnB/.github` created private (profile README with the non-affiliation notice, default
  SECURITY and code of conduct); it goes public with EnB.
* **Trap** (filed in workflow.md §3.8): app code can name an imported library's own members
  (soundlib.cpp's `using AudibleSoundClass::m_Unknown7C`): a rename across our files must leave those alone. The
  first verify caught it as compile errors.
* **Left (github-plan.md §4.5.7):** the public switch (`.github` first, then EnB; the ruleset with `checks` and
  `vc6-verify` required; private reporting; secret scanning), the rehearsal, a token scoped to EnB for the
  coordinator, the Actions version bump. P-VM-1 (the build VMs need replacing by 2027-01-20) stays open.

### 2026-10-29 (14:55, day): slow pace, two Fable rounds, D126-D141; app 4,549 -> 4,761 in tree, +289 pairs, 52 D66 moves, 0 lost outright

Coordinator Opus 5.5, 08:40 to 14:55 (launches closed 14:30; IB3 at the user's word). Base ad6562e (the history
rewrite's new master: every commit re-authored, old tip aeeb5bc). 19 builders, up to five at a time (17 Opus; F8 / F9
Fable); weekly usage 38 % -> 47 % (the Fable rounds about 2.5 points of it). Plan log: `.work/run-2026-10-29/plan.md`.

* **History rewrite (D126):** tools resolve refs at run time and are unaffected; the tracked files' old short hashes
  were rewritten through the old -> new map at this handoff (same length, no line moves; full verify 0 / 0), and the
  map is kept as `docs/commit-map.txt` for the old hashes in commit messages.
* **Fable (D135, outside the day's budget):** F9 made the verb object's five inline-budget functions exact (+6: the
  client's three static set helpers, a new §3.1 lever); F8 narrowed co.cpp's per-find spender (0 pairs; the
  'non-tail nested multi-return expansion' lever, 0x6FBEF0's frame). Measured cost: ~1.6 % of the week per hour per
  Fable builder, F8 ~1.1 % in all. D136: the queue is not pruned; rounds are scheduled more often.
* **Builds:** sclient's leftovers (SC3, 0x750260), the packet classes (PK3: DECAL / COLORIZATION, the verb merge),
  views (LD4: view.cpp / uiview.cpp / loadingview.cpp, +54; UI1: intrface.h, sbaseview.cpp, scriptedmovie.cpp),
  co_subtype_base.cpp (CS2, D132), land's vertex struct (LD5), fontmanager (FU1 +35, LS1 split_string.cpp +28), the
  proximity leftovers (PX3 +30), the shared-helper / while-loop rounds (HL1, HL2, HL3). Last: PX4 (0x454730's OBBox helpers +4), HL3 (Load_String in the client's form, 2 rows), IB3 (0x6FD310 parked for good, D140), UI1 (+62 in all).
* **Decisions:** D126-D141 (§2): the hash rewrite, D101's sbs criterion met, 0x750C20 parked, the ??_E / ??_G rule
  kept (D129), D130's one limit of 6 dead inline-budget spenders per function, D131 / D132, the aux_roots pad kept
  (D133), the two Fable rounds (D134 / D135), D136, interim placements delegated to the coordinator (D137; §4 item 8
  lists them), D138 / D139 under it, D140 (0x6FD310's last round: parked for good), D141 (every variant compile
  counts as a form; F9's, HL1's and HL2's overruns accepted). Open for the user (§3): P-UI1-1 (clienttest needs
  build/art/MSS32.DLL), accepted afterwards as D142; D143 (read the install folder in place, copy from it) the same
  day.
* **Coordinator lesson:** S8's five items were already done; check a §4 item's addresses against imports.json and
  attempts.py before briefing it (workflow.md §3.6).
* **Next:** §4 items 2, 5, 10 carry the builders' leads (sclient 0x74CA90 / 0x740050, P-LD4-1's follow-ups, the
  stand-in switches to SplitStringClass / FontSplitStringClass, P-FU1-1 FontSys, Resolve 0x487660's stash); one
  Fable round per run from item 7 (D136); the symbols.json names 0xB006F4 / 0xAEDE88 on a quiet tree.

### 2026-10-28 (17:00, day): the 24-hour run; packets -> object model; app 3,797 -> 4,549 pairs, 0 lost; D119-D125

Coordinator Opus 5.5, 2026-10-27 21:55 to 2026-10-28 17:00 (launches closed at 15:50; a hold 07:20-08:45).
Base c756cbf; HEAD +940 pairs against it (app 3,797 -> 4,549; all libraries 8,914 -> 9,773), no address lost outright
(D119's 0x474080 moved to ambiguous; D66 (i) / (v) moves only). 29 Opus builders, two at a time; weekly usage 15 % ->
37 %. Plan log: `.work/run-2026-10-28/plan.md` (every launch, report and heartbeat).

* **Packets -> object model (the user's interest; docs/packet-flow.md is the reference):** PF1 wrote packet-flow.md;
  co.cpp's bases / ctor / dtor (CO1), CREATE 4 / CHANGE_BASE_ASSET 38 / Set_Model (CR1), postate.cpp 13 states (PS1),
  extrap.cpp (EX1), the per-frame update and Do_Frame (UP1, DF1), the effects factory eff_o.cpp (EF1, EF2), co.cpp's
  rest (IB1, DM1), co_subtypes.cpp for CREATE's types 24 / 28 (CS1, D122), dynamic_obj.cpp's DynObj (DO1) and the whole
  proximity system px_* (PX1, PX2: built but never run in retail, §2.6), cpacket.cpp's every handler in source (CP1, CP2:
  97 exact / 10 NOT EXACT / 0 not rebuilt), sclient.cpp's object-facing members (SC1, SC2). clienttest drives the real
  CREATE / REMOVE / SUBPARTS through a fake asset manager (HN1) and grew 38 -> 103 checks.
* **Land and the main view (Next Step 5):** the lighting difference was VIDTEST's sun input (LD2: acos), the client's
  decorations replace viddeco (P-LD1-1), D119's Create_* form, D120's `--art build/art` default (LD3), mainview.cpp new
  (LD3, MV1, HN2, MV2: zone / wind / stars loaders, Add_Object / Remove_Object, the render path, Shutdown; every
  function tried), WW3D::Set_Gamma through WW3D_API_PATCHES (HN2).
* **Packet classes (Next Step 3):** PK1's self-name round (8370176, +293, for the user's decision in §3), PK2's value
  constructors with the union kept (T14 solved).
* **Tools:** gen_stubs writes `const RefCountPtr<T> &` / by-value RefCountPtr stand-ins (SC1, P-T18-2);
  `tools/re/symbols_named.json` with `symbols.py --apply-named` (MV2); `tools/harness/artdir.py` (LD3);
  `tools/vc6/auxpad.py` (HX1); wt.py bring-over landed every builder (refuses D66 / D119 moves: P-LD3-2 / P-CS1-2).
* **Decisions:** D119-D125 (§2). Open for the user (§3): 8370176's rule, P-HX1-1, P-IB1-1 with P-DM1-1, D101's sbs
  criterion (now met: 2.03-2.09 %), P-CS1-1, P-CS1-3. Traps from every report are in workflow.md §3.
* **Next:** §4 items 2, 3, 5 carry each builder's leftovers; the Fable queue (item 7) gained cpacket 62 / 8 as one round.
  Pool VM enb-vc6-06 has 6.2 GB of disk left. docs/imports.md in the main tree is stale: `importer.py report`.

### 2026-10-27 (20:30, night): decisions D96-D115, the audit, six tools, objmatch's range / initialiser rounds; +568 pairs, 0 lost

Coordinator Opus 5.5 (D96). Base e144a62; HEAD +568 pairs against it (+811 against c692c69, the day's start), no address
lost outright (one D66(v) move: dbgconn 0x821E40, still matched in connect). Plan log: `.work/run-2026-10-27b/plan.md`.

* **Matching (quiet tree, every library verified in full each time):** D100's range round +192 (38e745f) and its held-address
  refinement +25 (d8b8153); D94(b) `_$E` names skipped (9df983a, 0/0); D95(b) closed unimplemented (D114(a)); D114(b) the
  range widened by an object's lone exact COMDATs within 0x1000, +245 (86fd568); D114(c) symbols.json regenerated
  (10,422 -> 17,300 symbols) with symbols.py keeping a name's vtable / string proof beside `exact`, +72 (0253f43: the
  plain regeneration measured +63 / -323, it erased D88's evidence); D115's initialiser round +7 (c422c85: 0x709F40,
  inpkeys 0x42B420 / 0x42B540, permstr 0x4AE2D0, 0x729B00, 0xA06000 / 0xA06020). objmatch's order: D80, self name, D80,
  range, D80, initialiser, D80 (tools/re/README.md). `.work/pseudo/` is deleted: rerun `python tools/re/pseudocode.py`.
* **Builds:** packets 45 (166aed3) and 117 (bd9c2f2); F6's Fable round (D97) found E&B's ref_ptr.h has no
  `G_ASSERT(dummy == 0)` (4f2a8b1: client_lego_impl Find exact; every library 0 lost); F7 trimmed ref_ptr.h's comments so
  operator*'s assert sits on the client's line 356 (ce64a51, 0/0: its two users are not built yet) and made Remove
  0x72C1D0 exact (368912d, +5). Add 0x72B7F0 stays parked.
* **Tools (D112, D113):** private scratch per agent and `importer.py --agent` (f8a697c); commit_mine by entry and by row,
  `--refresh-totals`, compare-and-swap (f99519d); the root guard (2836859); `appfile.py match -I auto` / `--pool`
  (d8a8b2b); names.py over untracked files, refreshing not-exact.md (43b8144); `tools/vc6/wt.py` new / bring-over /
  remove / list (f584ed0; a real landing on master is still untested); pacing.py's lost-outright check counts an address
  still matched anywhere as a move (0e925c0). About 30 trap bullets retired (0b7aabd).
* **Decisions:** D96-D115 (§2); the audit's amendments are D110. Two audits (decisions; traps -> tools) and the Open-BFME
  comparison (they claim an address per ledger row and verify bytes there; ties never arise) are in the plan log.
* **Late (20:40-22:30), all decided:** D116 (D115 credits by symbol across objects), D117 (pairs_cmp prints
  `lost outright` and `moves`: b6439b3), D118 (evidence-backed pins measured by M4 in a wt.py worktree, caller in
  object +655 / 0 lost, placement 2.4 % wrong; kept in tools/re/experimental/pins/, not adopted, 5c8e998). Project
  settings committed (82704a1): claude.ai connectors and Claude in Chrome off, 120 non-PE REA tools denied. §3 is
  empty. HEAD +811 pairs against c692c69, the day's start, no address lost outright. The eval branches and
  worktrees (`.work/eval/*`) go after 2026-11-03 (`wt.py remove` / `git worktree remove`).

### 2026-10-27 (17:30, evening): the D93 model evaluation, eight runs scored; A-opus and B-opus merged (+113 pairs, 0 lost); P-E1 proposes the D72 revision

Coordinator Opus 5.5 (the user changed eval-d93.md's "the coordinator stays on Fable"; noted in D93's row).
Base c692c69; eight runs in worktrees `.work/eval/<run>` on branches `eval/<run>` (`.work/eval/setup.py`: junctions
for locks / pool / pseudo, dgrams, libraries). Every branch rescored by the coordinator (`.work/eval/score.sh`: verify
`--lib app --pool 1` in the worktree, pairs_cmp against the base and against the branch's own imports.json, replay,
udpreplay `conn alex3/alex7` for task A). Logs: `.work/eval/log.md`, `.work/run-2026-10-27/plan.md`. Memory, not
usage, limited the run (an editor's indexer held several GB indexing the worktrees): wave A started together; wave B
started in three steps (B-opus + B-sonnet-1 15:55, B-sonnet-2 16:10, B-haiku 16:30) with the same brief and base.
Usage at 17:29: session 27 %, week 8 % (HEAD +185 pairs against c692c69, 0 lost).

| Run | Exact / built | Pairs vs base (net, addresses) | NOT EXACT | Tokens (notification) | Min | Claims re-verified | Report issues |
|---|---|---|---|---|---|---|---|
| A-opus | 23 of 23 claimed (21 proven) | +36 / -9 rename moves (+27, 26) | 0 | 248,707 | 19 | exact (0/0) | tokens said ~160k; "23 of 23" in the subject, corrected in the report |
| A-sonnet-1 | all | +36 / -9 (+27, 26) | 0 | 215,777 | 29 | exact | tokens said ~400K; headers rows not committed; counts not in subjects |
| A-sonnet-2 | 5 of 6 for 83 | +33 / -9 (+24, 23) | 1 (0x886890) | 213,638 | 29 | exact | tokens said ~450k; tsk_game.cpp view replaced by an include (unasked) |
| A-haiku | 2 parked | +29 / 0 (no rename: object split in two files, 19) | 2 (0x886710, 0x862300 prefix) | 344,774 | 34 | exact | alex7 "fails at HEAD" wrong (used `run`, baseline its own commit); built udpreplay in the main tree |
| B-opus | all but the String getters | +86 / 0 (86) | 2 (0x7F9700, 0x7F9970) | 384,644 | 44 | exact | tokens said ~260k |
| B-sonnet-1 | 7 not exact | +36 / 0 (36; 35 real: 0x603D70 is a side effect, see the addendum) | 7 (Set_Defaults, dtor, Clear, Load, Save, 2 getters) | 502,880 | 59 | exact | extent 0x7FF010 vs B-opus 0x7FEBE0 |
| B-sonnet-2 | 8 NOT EXACT (Load, Save, Set_Defaults, dtor, 0x7F81A0, 2 getters; Set_Defaults literal) | +33 / 0 (33; 32 real: 0x603D70, see the addendum) | 7 | 487,646 | 71 | exact | tokens said 600-700K; extent to ~0x7FFFxx (scriptedmovie at 0x8000C0) |
| B-haiku | readers and finds only | +21 / 0 (21, all within B-opus) | 1 parked (0x7F9700) | 241,902 | 16 | exact | stopped by choice at 16 min of 150; kept Unknown_BE8B38Class; first write landed in the main tree (removed) |

* **Every claim held:** each branch's committed imports.json equals the coordinator's re-verify (0 gained, 0 lost),
  replay 0 differences everywhere, udpreplay conn alex3 / alex7 PASS on all four A branches. The inaccuracies are
  the self-reported token counts (off by -35 % to +110 %, both directions) and one wrong harness claim (A-haiku).
* **Task A:** Opus and one Sonnet run reached the same 26 addresses; Opus in two thirds of the time with a little
  more tokens; the second Sonnet run missed one destructor; Haiku 19 addresses, two parked and an object split.
  **Merged: master fast-forwarded to eval/A-opus (3f7fdc0);** the other branches gain nothing A-opus lacks.
* **Task B:** B-opus +86 (Load, Save, the 20 KB Set_Defaults exact; extent 0x7F0F60-0x7FEBE0); B-sonnet-1 +36 for 1.3x
  the tokens. **Merged: B-opus cherry-picked onto master (a784725, 5e8b4df, 4e9fc19)**; master verify after both merges: app 3,388
  functions, +122 / -9 rename moves against c692c69 (112 addresses = 26 + 86), 0/0 against the committed imports.json,
  replay 0. (0x603D70, which both Sonnet B runs paired, is not option.cpp's: see the addendum.)
* **After the eval (17:00-):** memory freed, four Opus builders on master, disjoint files, stop 18:20-18:50: R1
  co_player_relationship.cpp (3 commits by 17:09), P10 packets 181/182 (and udpreplay's main-tree rebuild), T19 cpacket's
  65/178/146/100/165, H19 the key-name object 0x42B610. Their results: the addendum below this entry.
* **Traps:** ten bullets filed in workflow.md §3.1, 3.2, 3.4, 3.6, 3.7, 3.9 (dead reads as expansions; auto_inline(off);
  the list destructor's forms; C1001 on hash_map deduction; commit_mine's headers keys; private TEMP; appfile -I;
  udpreplay's `conn` gate; absolute paths in worktrees; pkemit / pkclasses keys).
* **Incident:** A-haiku's failed `cd` ran `udpreplay.py build` in the main tree (build/programs/udpreplay overwritten
  with a failed link, `.work/harness/udpreplay_src` re-staged; untracked only): rebuild before the next udpreplay check.
* **Open for the user (§3):** P-E1 (the D72 revision), P-A1, P-Bopus-1, P-Bopus-2.

* **Addendum (19:00): after the eval, nine Opus builders on master, 42 commits since c692c69, app +243 pairs / -9 rename moves
  (194 addresses), 0 lost; a quiet-tree verify at 8f83933 matched the committed imports.json (0/0), replay 0, vidtest
  links, totals refreshed (16501c3: 3,463 in tree).** R1 co_player_relationship.cpp 0 -> 31 (handlers 137-139, the
  gutter colours, the checks; Neutral 0x70B450 parked) and R2 +3 (Group 0x70BC30 / 0x70BAC0 through a dead-read wrapper,
  0x70B3D0); P10 packets 181/182 and 21/22, 15 of 15 (+22; udpreplay rebuilt in the main tree, conn alex3 / alex7 PASS);
  T19 cpacket NAME_DECAL 178 and CAMERA_CONTROL 146 exact, FORMATION 65 / CLIENT_DAMAGE 100 written NOT EXACT; T20
  CLIENT_CHAT_EVENT 165 written NOT EXACT (inline-budget) and clienttest's expected.txt refreshed for RELATIONSHIP 137 only
  (38 of 38); H19 the key-name object as app/input/inpkeys.cpp (provisional name, +18) and fontmanager 0x6D2580; H21
  fontmanager 0x6D2700 (0x6D0F70 parked: its exact form costs fontdatabase's Load_Init); H22 **P-H10-1 closed**: the
  declare-then-assign iterator from the eval made the four hash-loop residues exact, plus two destructors and the LIMIT row
  0x4B3850 (touched against the brief; gain-only, kept); H20 client_lego_impl Add / Remove to the client's EH states (still
  inline-budget); S1 step 8's Open_Fixed_Channel, memconn's slot, both Pending_Messages (+4). O1 found **0x603D70 is not
  option.cpp's**: `std::string::assign(const char *)` with 17 callers in 0x5FBD30; the Sonnet B branches paired it through a
  non-exact Set_Defaults, so their B scores are 35 and 32 addresses, and B-opus covers every real gain. Traps: a second
  batch in workflow.md §3.1, 3.4, 3.6, 3.8, 3.9. Usage at the end: session 34 % before the 18:10 reset, week ~10 %.
  New for the user (§3): P-H22-2, P-S1-2, P-R2-1 and the Fable queue (P-T19-3, P-H20-1, P-S1-1, P-T20-1).

### 2026-10-27 (14:05, day): the session cap and the reset; seven builders at a time; 352 commits, app +2,175 pairs, 0 lost; D90-D95; the eval D93 is next

Coordinator (Fable), 04:30 to 12:30. The API session cap stopped
five builders at 06:05 (the user chose to spend the session window); all five resumed from their transcripts
at 08:11 with their leases intact and lost nothing. The pool grew to seven slots with 2 GB startup clones
(the host's memory, user); the day ran up to seven builders at a time. Totals since the 04:40 entry:
270 commits, app +1,941 (file, address) pairs with no address lost (the 36 pairs_cmp "lost" entries are the gameedit renames' moves, D95(c)), app client functions ~2,350 -> ~4,300 in 201 files (client/ 16 .cpp, packets/ 124); the whole run from aa66709: 393 commits, +3,009 pairs, only the two D66 land copies lost. The run plan and log: `.work/run-2026-10-26/plan.md`.

* **D88 and the quiet window (04:24-04:31):** the self-name tie-break, +368 pairs, 0 lost (0d70c67 the
  snapshot, efc3cbb the rule); the item 5 tools (D78 pointers, tryv2's EXTRA_INC; D74/D77/`--pool` were done).
* **Step 9 (D90), T5/T6/T7/T8/T9/T10/T11:** 9a kcom (authlogin.lib from a .def, AuthLoginResponseClass 2 of 2),
  9b the login data objects (the ticket and character slot 13 of 13, the galaxy entry, avatar_desc ~38 pairs,
  shipcust_desc 12 of 12, the character creator), 9c ClientClass (client.cpp, 0x1150 bytes, 23 slots, 22 of 28
  exact), 9d SClient's skeleton (sclient.cpp on its 0x14F8-byte layout, SClient : ClientClass, SOptionsClass;
  the constructor, Process_Packets, the pump and three overrides exact; Init / Shutdown functional), soptions.cpp
  7 of 7, cpacket.cpp's handlers (T8: 30 handlers, 29 exact; T10: see below), co.h (ClientObjectClass out of
  client.h; H13) and co.cpp (T11, below), and **the clienttest harness** (T9, `tools/harness/clienttest.py`:
  client.cpp, sclient.cpp, cpacket.cpp and the packet objects with logging stand-ins; 38 checks; alex3's first
  1,000 messages drive the real handlers into a trace checked against expected.txt). kerneltest 81 of 81
  throughout; VIDTEST's link, broken by 969f244's stub, restored in 9ba36c5 (stand-ins live under
  tests/capharness/udpreplay/).
* **Packets, P5/P6/P7:** ~190 -> ~245 of 260 classes (+~550 pairs): JOB_LIST 147, D91's rule with Server_Ready 59,
  220 with MoneyClass, 219's constructors through the filed lever, 5000/5001/5010, 95, 230/231, ITEM_BASE 37,
  94, 173, the game-editor packets 30002-30028 (app/packets/gameedit/, the client's file names, D95(c)),
  97 under D95(a)'s recorded-bytes list, 183, 138, 84, 176/177, 168-172, 82, 53, 58, 8/70/65. udpreplay alex3
  51,800 -> 52,166 Build-written messages, alex7 4,516 -> 5,514 (+38 / 32 sent as received). Parked: 45
  (P-P6-1), 117 (D95(b)), 82's raw constructor and Build.
* **The SGI hash_map's users (D87), H6-H15:** H6's header lever (`_M_destroy_val`: the class template's own
  destructor call, +10 whole-app; out-of-class members, pragmas and find forms do nothing); then by file:
  lookup_table +41, ini_manager / definition_manager +27, attrib_dir +58, attrib_dir_collective +51,
  symbol_substitution +27, boolpars +38, unique_design_id_lookup +119 (the first iostream user: VC6's
  `<sstream>` byte-exact), soundlib +41 (app/audio), ui_line_display +31, control_keys_manager +53 (H11) and its
  remainder (H14, below), co_data +18 (five-way twins elsewhere), fontdatabase +85 (H12), and **the caller-budget
  lever** the table's parked finds waited for: a `const_iterator` local makes VC6 call find out of line with the
  key inline (H12, 7 of 7; H15 applied it as a batch, 4 of 4 std::string-keyed callers; not const char * keys).
* **Land, L5/L6/F5:** Load_Assets, Update_Patches, Compute_Lighting, Render, the collision code, spline
  evaluation (L5; land 47 -> 60 pairs), the horizon chain and the Matrox displacement-map companion
  (landdmap.cpp, L6), the lightmap threads exact through F5's Fable round (the depth lever: a one-line inline
  helper pushes the deepest expansion out of line). VIDTEST on LandClass in a worktree trial: the terrain and
  lighting match (frames 1-550 within 0.9-4%), the decorations are missing (P-L6-1, user).
* **Fable rounds under D89:** F4 (SegLineRenderer's Render exact after one move; tsk_login +1; three parked with
  explanations, ~330k) and F5 (the lighting record, 6 of 8 forms, ~260k). The limits held.
* **Tools:** the pool's 2 GB startup and cap 7 (546973a), vc6.py's junction fix (096a45b), build.py's UNRES regex
  (2618ede), udpreplay's D91 and D95(a) rules, clienttest. Every builder's levers and traps are filed in
  workflow.md §3 (about 40 new rows and bullets today).
* **Decisions:** D90 (9a-9d), D91 (udpreplay's size rule; Clamp_Int declined; P-L4-2), D92 (P-T6-1's Fable round
  after F5; clienttest deferred then accepted), D93 (**the model evaluation: the next session's first item**,
  [eval-d93.md](eval-d93.md)), D94 (0x85CB00 as D66(iii); the `_$E` name rule; clienttest), D95 (the
  recorded-bytes list; the implicit-destructor twin rule; renames as D66(iv)). Open for the user: §3.
* **The last hour (11:00-12:05):** T11's co.cpp (+42: 34 members, the vtable through +0x44, the 0x188-byte layout), T12's
  objmatch fix (b4c8a64: D80 read the client's `__FILE__` only to our literal's length; relative header paths never tied,
  +16) and its aux-data lookup template (T13 landed it with clienttest's link restored, 07e9718, +4), P7's seven small
  packets (+63; 129-133 complete the object 0x89FB50-0x8A14D0), P8's P-P7-1 trial (the implicit ShipCustDescClass
  assignment kept at +0/-0; the char[] layout would trade +3 for tsk_login's constructor: P-P8-1), H15's const_iterator
  batch (4 of 4), H14's fontmanager.cpp (+23) and control_keys remainder (+16), the tools fix P-T9-1 (2618ede).
  Incidents, all resolved: a builder's edit script truncated the shared attempts.csv (restored from HEAD, trap filed);
  clienttest's link at HEAD depended on a stale build/app/app.lib and on P7's files pulling shipcust_desc.obj (T13:
  `build.py build --lib app` first; udpreplay's shipcust stand-in staged for clienttest). Every VM is released; the
  working copy's uncommitted imports.json totals hunk (commit_mine never commits the totals lines) was discarded: the
  next quiet-tree verify refreshes them. Usage at handoff: see the plan log's last heartbeat (weekly ~73%).

* **The afternoon (12:20-14:05, the user: "use all our quota before reset", then "don't start anything new"):**
  seven more builders on disjoint files, 81 commits, app +234 pairs, 0 lost (final totals since the 04:40 entry: 352
  commits, +2,175 pairs; since aa66709: 475 commits, +3,243 pairs, only the two D66 land copies lost). T16: ten more
  cpacket handlers (64, 17, 37, 62 exact; the position-update family NOT EXACT on one Is_Valid_Float residue, P-T16-3);
  clienttest drives 26 handlers over alex3's first 1,000 messages, trace 1,952 lines. T17: rtti.h's type test (a
  class-static cast; the nesting-depth budget lever) made the parked co.cpp lookups exact, and the client object's
  0x4C-byte base UnknownB02C90Class is in client.cpp (constructor / destructor exact). T18: sclient.cpp 22 -> 52 pairs;
  Do_Frame / Render / chat wait for their clienttest stubs to go (P-T18-1). H17: client_lego_impl.cpp 0 -> 57 pairs
  (LegoClass bases, bone maps, knobs, orphans; three Fable-eligible). H18: app/input/inpdsp.cpp 10 of 11 exact (+17;
  the registry read only, unreachable from tests). P9: the login packets 111, 119, 2, 118, 112, 113, 114, 145 (+90) and
  most of P-T5-2 (the visitor merge is P-P9-2). Tools: commit_mine selects imports.json hunks by file block
  (`--file`, 9263c0c), attempts.py writes the csv's own line ending (571dba0). Two incidents, both repaired by the
  builders and filed as traps: H18's zero-context imports.json patch (ebad207) and H17's private-index commit racing
  T18's (684476f; the compare-and-swap rule). New for the user in §3: P-P6-1, P-T12-2, P-P9-1, P-P9-2, P-T16-3,
  P-T18-3. Every VM is stopped; the tree is clean.

### 2026-10-27 (04:40, overnight): twelve builders in parallel; app +1,018 pairs, 0 lost; step 9's tasks, the SGI hash_map, land, 190 packet classes; D85-D89

Coordinator (Fable), 2026-10-26 21:30 to 2026-10-27 04:30. Tree aa66709 -> HEAD before
the quiet window: **121 commits, +1,019 pairs, 0 lost** (`pairs_cmp`; app 1,018, ww3d2 1; the only "lost"
entries are three inline COMDAT copies land.cpp no longer emits, still matched in other files: D66(i)).
App went 1,256 -> 1,997 client functions. Up to five builders at a time on disjoint file sets (D62), ~9.5M
builder tokens, the run plan and log in `.work/run-2026-10-26/plan.md`. A three-hour API outage (DNS, ~00:11
to 03:00) killed three builders mid-item; all three were resumed from their transcripts with their leases
intact and lost nothing.

* **Packets, P3 and P4** (Opus, ~650k + ~2.5M; 52 commits): 114 -> ~190 of 260 classes. ADVANCED_POSITIONAL_UPDATE 62
  over FlagPacketClass (19 of 19 exact), OBJECT_EFFECT 9, RELATIONSHIP 137 with udpreplay stubs (P-P1, P-P2),
  the chat packets, the guild object's 18 classes (pkguildplayeronline.cpp, +110), the GM packets, VERB_UPDATE
  92 with `std::set`, the navigation list file (app/common/unknown_navlist.cpp) with WARP 155, the time packets
  52/68 with udpreplay comparing their clock fields masked, 5004-5009 in pktask.cpp with `#line`. udpreplay's
  alex3 round trip 19,084 -> 51,800 messages, alex7 3,618 -> 4,516. NOT EXACT: Colorization's value ctor
  0x87F320 (shape), 219's two ctors (order-only; P4's lever "Unknown68 as two ints" fixed 108 in 27bcb40).
  59 Server_Ready is 5 of 5 exact but parked in `.work/p4/park59/` behind P-P4-1 (§3).
* **Step 8's tail, S3** (Opus, ~450k): `Pump_Pending` 0x82AA10 exact through an inline RSA decrypt, the UDP
  constructors and listener exact once `CordMgr` got its key pair (+0x5C, 0x95C bytes; `unknown_rsakey.h`),
  `Remove_Dead`, the rate destructor; +19. Key_Exchange parked at 0.87 (inline budget, LIMIT under D89).
  Seven code-generation levers filed in workflow.md §3.1.
* **Step 9, T3 and T4** (Opus, ~545k + ~475k): tsk_game.cpp complete (ClientTaskClass, 16 of 16; Init through
  D40's dead-call lever), tsk_login.cpp 48 functions exact (the login packet handlers, both constructors with
  the five 0x160-byte ship-customisation objects, the avatar copy, the galaxy list), the kcom ticket and fast
  start NOT EXACT; +86. Six LoginTaskClass functions parked after two rounds (register-only or inline budget).
  The mapper's `docs/kernel-map-step9.md` (040b035) sizes what step 9 still needs: client.cpp's base class,
  SClient (0x14F8 bytes; 111 functions, 62 KB; cpacket.cpp's 99 handlers depend on it), the login and
  character-select screens (the kernel map's 0xAF2C30/0xAEFB60 are slot +0x40 of the real vtables), kcom
  (authlogin.dll, an import library from a .def), the login data objects (~43 KB); recommends 9a-9d (§3).
* **SGI hash_map, D87: H3, H4, H5** (Opus, ~330k + ~450k + ~280k): SGI STL 3.3 fetched into
  `C:\EnBDevelopment\SDK\stl-3.3` (github.com/steveLauwh/SGI-STL, the SGI/HP notice kept; roots.py `SGI_STL`,
  toolchain.md §2), VC6-adapted headers generated into `libs/sgistl/` by `tools/vc6/sgistl_adapt.py` (the
  client's hashtable ctor takes 3 arguments: the allocator dropped). On it, byte-exact: kv_list's index and
  `Keep_Only`, dbgconn's counters, kv_tripl.cpp, cfaction_database.cpp, buff_database.cpp, tutorial_manager.cpp,
  combo_ffactory.cpp; +134. Every parked miss there is the client inlining fewer hashtable/string helpers than
  our headers (H5's P-H5-1): a header-level lever to test on a quiet tree (§4 item 3), not per-function puzzles.
  H4's report lists the 20 remaining caller clusters of the string hash 0x42DB00 by file.
* **Land and splines, L3 and L4** (Opus, ~650k + ~450k): spln_db.cpp as the client's spline database (30 of 50
  exact, +45), heightfield.cpp on the client's 0x98-byte layout, LandClass on its 0x1228-byte layout with 18
  exact (land 21 -> 44); VIDTEST draws through `programs/vidtest/vidland.*` until LandClass can render (P-L4-1,
  then Load_Assets, Compute_Lighting, Update_Patches, Render 0x474B30). VIDTEST.EXE's height sampling is the
  client's two-triangle one (its Sample 0x402AE0 is byte-identical to 0x463BF0): the trees switched, every
  compared frame closer to the original.
* **WW3D2, W3** (Opus, ~480k): `subdivision_util` 0x9C2390 exact through a per-function Zero Hour hybrid (+1), the
  3-argument `Make_Screen_Shot` declared; the distortion draw to 0.92 (the sixth `??_H` is an inline spend after
  the view restore), Compute_Caps, SegLineRenderer's Render and the ShdMesh ctor parked with stashes.
* **Fable, F3** (~540k, 0 pairs): Ship's zeroed members are floats (residue -9 rows); the lookup tests, the kv ctor
  and Key_Exchange (57 -> 43 rows) recorded with written explanations. The cost of that round is what D89 limits.
* **Tools:** commit_mine.py's -U0 hunk placement fixed (9f627f6: a skipped earlier hunk shifted an insertion into
  another imports.json entry; e5661c2 had broken the file), explain.py's window ends at the nearest verified or
  proven start (ffd32e5: Create_Device reads identical), `attempts.py` LIMIT REACHED and `check --lever` (D89).
* **Decisions:** D85 Open-BFME's method (not its code), D86 step 8's provisional names kept, D87 SGI hash_map,
  D88 the self-name tie-break (carried out in the quiet window after this entry), D89 the Fable limits.
  Open for the user: §3.

### 2026-10-26 (21:25): Open-BFME's method adopted (D85); P-P1, P-P2 accepted, P2 kept; packet round 3 and step 8's tail launched

Coordinator (Fable). The Open-BFME-1 assessment of 2026-10-25 (a read-only look at github.com/Open-BFME/Open-BFME-1
@ 7b8a0f5: BFME's DX9 ww3d2 under MSVC 7.1, so its bytes, layouts and names prove nothing about E&B's client;
its two register-allocation levers tested on Make_Screen_Shot gave our U form again) is recorded as D85: nothing
of theirs enters the sources or roots, their method does. Carried out by Opus early on 2026-10-26, before the night's
builders ran: the attempt log `tools/re/attempts.py` + `docs/data/attempts.csv` (5f0b059), the near-miss classifier
`tools/re/explain.py` (08ffb04), the switch sweep `tools/vc6/flagsweep.py` with tryv2.py (f4a950d), and workflow.md
§3.1 as a symptom -> lever -> proof table with explain.py's triage order and the two-attempt stop rule (affd582).
Builders P and S already used it (one attempts.csv row each, cited in their reports). The scratch clone is deleted.
builders.md §3 now asks each report for the attempt rows written and explain.py's class per NOT EXACT function.
User decisions: P-P1 and P-P2 accepted (packet round 3: 62 ADVANCED_POSITIONAL_UPDATE, effects 9 and 11, the
udpreplay stubs for RELATIONSHIP 137); P2 kept (D86); D7 stays enforced (byte-exactness is the goal and the
measure). Launched: the packet builder (P-P1, P-P2), then step 8's tail (`Pump_Pending` 0x82AA10) once the packet
builder's first commit lands, to keep the shared-record window short (§3.8's imports.json trap).

### 2026-10-26 (07:57): packet classes round 2: 91 -> 114 classes, +159 pairs, 0 lost; udpreplay alex3 7,408 -> 19,084 messages

Builder P (Opus, ~400k tokens over two runs, the first cut at 01:10; six commits 1739545, efcbe24, 25480bb,
d3203c5, 3fb7a54, 0df698f, each gaining only, replay 0 differences, `udpreplay conn` PASS on alex3 and alex7).
By hand: DirLock 223-226 over their base (vtable 0xB0A290), GlobalLock 227-229 (0xB094F4), AUX_DATA 27 with
PLAYER_VAR_AUX_DATA 28, GALAXY_MAP 151/152, CLIENT_SOUND 106, WARP_INDEX 156, and the client's pkflag.cpp
(`FlagPacketClass`, vtable 0xB0A3D4: the base of effects 9, 11-14 and ADVANCED_POSITIONAL_UPDATE 62).
Decoder: pkdecode/pkemit read a Build whose opcode comes from `Get_Type` (slot +0x28; 10 classes in 7 files,
25480bb), reach the last packet object (the old bound 0x8B1EB0 was exclusive), and keep a `HAND` set of
files pkemit never rewrites. Round trip: alex3 7,408 -> 19,084 messages, alex7 1,622 -> 3,618. NOT EXACT:
AUX_DATA's Print 0x8770A0 (register order, two forms, `attempts.py show 0x8770A0`). Pair counts include
PacketClass's inline duplicates 0x52B210-0x52B350 (five per new file, D20). 1739545's and efcbe24's
subjects overstate the class count by two (real: 91 -> 95 -> 100; 25480bb records the correction).
Not started: 62 ADVANCED_POSITIONAL_UPDATE (13,833 messages, the largest gain left: needs the flag builder
0x8AD300, the size function 0x8AD220, asserts on pkupdate.cpp lines 277-285), 137 RELATIONSHIP (1,544
messages: a Reaction class 0x848C80 and strconv helpers 0x835890 / 0x4A30A0 that udpreplay would need
stubs for), 173-177 GroupServer. 146 classes still don't decode (33 fail on the destructor, 18 have an
intermediate base, 16 store unknowns in the constructor, 13 fail on Print, 19 have length-counted buffers
through `movsx` / `mov [reg]`: the most regular decoder extension). Proposals P-P1, P-P2 in §3. Traps filed
in workflow.md §3.6 and §3.8.

### 2026-10-26 (07:54): step 8's tail: P1 applied, the TCP listener's key exchange built; app 1133 -> 1252 pairs with P's packets

Builder S (Opus, ~1.1M tokens over two runs, the first cut by the session limit at 01:10; seven commits
d63ba29 ... 1d80e3c, each gaining only, replay 0 differences, `udpreplay conn alex3` PASS). P1 applied
(d63ba29): `~ForwardingConnectionClass` inline in connect.h, dbgconn 0x825170 / 0x8251A0 gained, 0x821E40
kept thanks to 9cceabd's tie-break. f95b6a6 refreshed imports.json from a full verify (+4 stale pairs).
tcpconn.cpp: `Key_Exchange` 0x829180 functional, NOT EXACT (one `_Tree::_Inc` the client inlines, a
cumulative inline budget; `attempts.py show 0x829180`, 16 forms, `.work/s1/vars1-7.py` + `vk.py`); the
listeners' `std::set` 0xBE91A8, `Pump_All` 0x82A990, the listener destructor 0x82A2C0 / 0x82A2F0 (12dcede);
the listener constructor 0x829E50 with `RSAKeyClass`'s SSH1 key-file loader `Read_Number` 0x82C260 /
`Compute` 0x82C330 (d7d979b, the matching form is `KeyPair.Load(file_auto_ptr(...))`); `Send_Key` 0x82A830
(7ae0040, its record completed by 1d80e3c). Two shared-record sweeps happened (a hand-built imports.json
left in the working copy was taken by P's 25480bb; a verify's rewrite was taken by 7ae0040): both repaired
in later commits with nothing lost (12dcede restored P's 68 pairs; P's 3fb7a54 gives the pkflag pairs their
source). Traps filed in workflow.md §3.1 and §3.8. Left of step 8: `Pump_Pending` 0x82AA10 (the same key
class, best lead), dbgconn's counters, the UDP constructor 0x82C680, the ArrayList copies, the NOT EXACT
list. `RSAKeyClass`'s `Unknown200..Unknown800` members stay NAME 2 until a string names them (P2's rule).

### 2026-10-26 (00:05): kernel-map step 8 nearly done: app 1028 -> 1133 client functions, +113 pairs, 0 lost

Builder (Opus, ~720k tokens; 13 commits 968f7d0 ... 7606eaa, each gaining only, replay 0 differences,
`udpreplay conn alex3` PASS): connect.cpp's forwarding and peek wrappers, `Accept_New`, `Add_Unwrapped`
0x822790, `Add_Connection` 0x822CC0; the factories 0x821000/0x821400/0x821450, the host helpers,
`Network_INI`, `Create_Connection_Manager` 0x822F10; `Try_Connect` 0x8290A0; memconn.cpp with the
registry by port (provisional `unknown_memregistry.cpp`); dbgconn.cpp's packet logs; the rate wrapper
(`average.h`), the statistics wrapper slots, the UDP and TCP listeners, the composite listener
(provisional `unknown_listener.cpp`) and `Unknown_RateClass` (`unknown_rate.cpp`). Three gains are D20
COMDAT duplicates. NOT EXACT: `Remove_Dead` 0x822B50, the memconn option slot 0x826D00, Rate Count
0x824EC0 and its four inlining slots, the timed log constructor 0x824540, TCP Listen 0x82A470, the UDP
listener and manager constructors. `.work/step8b/tryv2.py` (variants across several files and app
subdirectories) is promoted with item 4's tooling. Open for the user: P1 and P2 (§3).

### 2026-10-26 (early): E&B's TCPCon and TCPQueue exact in wnet; ConnectionManager in connect.cpp; +56 pairs

Builder (Opus, ~450k tokens, cut once by the session limit and resumed): c0e19b6 E&B's own `tcpcon.cpp`
and `tcpqueue.cpp` rebuilt in `libs/wwonline/wnet` (TCPCon 18 of 18 exact, TCPQueue 7 exact plus one too
short to count; wnet 8 -> 41 functions, wlib 76 -> 78, app 1008 -> 1009; E&B's `CONNRESET` -16 state,
wlib's `msleep` 0x8B8270 and `growVector(howmany)`; wnet builds with /GX), then step 8 in app/common:
53f6aa2 tcpconn.cpp's two remaining constructors (by host name 0x828C80, over an accepted socket
0x829C80); 5434b6d, 00678ec, cb58b86 `ConnectionManager` declared in connect.h with 17 methods exact
(app 1009 -> 1028). Every commit gains only; replay 0 differences; `udpreplay conn alex3` PASS.
Not exact: `Try_Connect` 0x8290A0 (the client repeats the `Handle = 0; return true` tail; 0.75 after
three forms). Not started: `Add_Connection` 0x822CC0 (needs the peek/stats wrapper classes, vtables
0xB08590/0xB0854C, and ArgvClass), `Accept_New` 0x822300, the factories 0x821000/0x821450, the RSA
exchange 0x829180, memconn.cpp. D84 records P-B1 (`~TCPMgr() throw()`); P-B2 (per-file flags) declined then, adopted 2026-11-02 (D186)
as not needed. Earlier the same night: the screenshot residue parked (85d3506).

### 2026-10-25 (night): four parallel builders; +654 pairs, 0 lost; D73 imported; 91 packet classes; D80-D82

The first D62 run with four Opus builders at once on disjoint file sets (`.work/run-2026-10-25/plan.md`:
models, tokens, results), started from d8a7dd9. `pairs_cmp` d8a7dd9 -> HEAD: **+654 pairs, 0 lost**
(app 545, wlib 75, binkmovie 24, wnet 8, ww3d2 2). About 1.75M builder tokens.

* **A, D73 imports** (1cbd027 ... 642256d): WWOnline's wlib at `libs/wwonline/wlib` (74 then 75 pairs:
  E&B's `Xtime::getTime_t` 0x8BA510 rebuilt), wnet (matchbot's udp.cpp, RenegadeGR's tcpmgr; 8),
  Renegade's BinkMovie at `binkmovie/` (21 functions, 24 pairs). The critsec, dictionary, CordArrayClass
  and Wtime stand-ins are gone (D67; every Wtime address was wlib's Xtime). Kernel-map step 8 started:
  `app/common/tcpconn.cpp` with 8 exact. RenegadeGR's tcpcon.cpp matches nothing: E&B's TCPCon is its own
  (an OutputDevice with a lock, writing through tcpqueue.cpp), so it is rebuilt next (P-A3).
* **B, packet classes** (33dedee ... 9084412): clientstub.cpp's 149 stubs exact in one commit; pklogoff by
  hand; then a decoder, `tools/re/pkdecode.py` / `pkemit.py`, drafted 91 of the 263 classes into 56
  app/packets files, 395 of 396 functions exact (one NOT EXACT constructor 0x867250). udpreplay is now a
  round trip through the rebuilt classes: alex3 7,408 and alex7 1,622 messages byte-identical. 251 exact
  functions are uncounted as `multiple` (one-int constructors and Prints look alike): D80.
* **C, WW3D2** (2cfad68, f9e4a92): `PointGroupClass::Render` 0x954960 and `Process_Foreground_Load`
  0x949C60 exact (ww3d2 2475 -> 2477). Make_Screen_Shot and Update_Movie_Capture share one 4-instruction
  load-order residue (a Fable puzzle under D72 after two Opus rounds); the distortion draw is an
  inline-budget effect with no plausible source form yet.
* **D, app leftovers** (66535af, 33a4c97, 65991e8, 07af3e1): operator delete 0x4E9D10,
  `DeltaCompoundClass::Add` 0x4A5670 (a dead `size()` read spends the inline budget: D40 closed), the
  Mounts array constructor 0x706F60 exact; the assert reporters exact but `multiple`; Ship's BuffArray and
  Buff elements modelled as the client's classes (neutral, D66). D75's premise corrected (D81).
* **Coordinator:** D80 (objmatch tie-break by relocation target), D81, D82, D66's neutral-fidelity clause,
  D18/D28 confirmed as operand-order noise (eb46e3d); traps filed in workflow.md §3.1/3.3/3.4/3.8;
  `tools/vc6/commit_mine.py` promoted from builder C (6134002) after the shared git index swept staged
  work across builders twice (repaired, nothing lost: checked by file). builders.md: own files by path,
  shared files only through commit_mine.py.
* Open for the user: A's proposals P-A1 (binkmovie at the top level), P-A2 (wstypes.h takes WWLib's
  bittype.h types), P-A3 (E&B's tcpcon.cpp and tcpqueue.cpp next), P-A4 (build.py/LINK_LIBS when a
  program first needs wlib). The tools item (§4 item 5, D80, D74, D77, D78, D81) runs next on Sonnet.

### 2026-10-25 (evening): D63's supervised E&BConfig run passed; M2 complete

The one-time acceptance check of M2 gate (c): the user ran the original `E&BConfig.exe` (build 8168,
2004) unelevated from the EBCONFIG copy at `build/programs/vidtest/e2e_ebconfig/`, with our vidtest
(build 2026-10-24 22:43) in place as `VIDTEST.EXE`, and pressed Performance Test. Our vidtest ran the
three tests to the end at the configured 1280x1024x32 windowed and exited 1; E&BConfig read its
`results.ini` and displayed Fill Rate FPS 3529, Polygon Budget 30000, Star Budget 2000, Land Polygon
Budget 28000, Land LOD 7, Texture Reduction 0, Peak Poly Rate 57655 KPolys/s, every value as written
(`vidtest.log` line 358). "Free Texture + AGP RAM = -10.486 MB" is E&BConfig's own `atoi` wrapping our
`FreeTextureRam` (a card over 2 GB) to a negative value and dividing by 10^6: the 2004 tool's limit, not
a defect of ours (the original VIDTEST reports the same figure; `vidtest_e2e.py` emulates
the wrap). The run was cancelled at "Press OK to save settings". E&BConfig's registry settings were
byte-identical before and after the run. No message box appeared. M2 is complete on all four gates.

**D53 seen in the game.** An install configured by the original VIDTEST on a card over 2 GB has
`TextureReduction=2`, written by its signed comparison: every texture loads at a quarter of its size.
With the value set to 0 and the original client relaunched, screenshots of the same views
(local) show the planet in Endriago at about twice the fine detail (edge energy
2.58 -> 5.01 on the cropped planet), the ship hull and the login-screen avatar modestly crisper (1.09x,
1.11x: their textures are small to begin with). So the original tool's bug cost players most of the
planet and nebula detail on any card over 2 GB; our vidtest's fix (D53) is the setting to ship.

### 2026-10-25 (later): the earlier research audited (D79)

An audit of the maintainer's earlier, unpublished protocol research (outside this project, D153) corrected
prior-work.md, tools/harness/README.md's EA-UDP table and the reference-trace paths here.

### 2026-10-25: housekeeping audit; D62-D79; builders.md; traps 100 -> 74; milestones split; tools promoted

Four read-only audits (`.work/audit/traps.md`, `decisions.md`, `tools.md`, `startup.md`; two on Opus,
two on Sonnet) then three edit agents on disjoint files. Found: D32 (serial builders) contradicted the
approved parallel runs; D23 (NDEBUG off) contradicted D61; D36 vs the E&BConfig check; D7 vs `NOT EXACT`
code beyond D42's scope; D39 as written didn't compile; D10/D16 vs D56; the builder brief, the pairs
comparer and several standing safety rules existed only in the git-ignored `.work/`; a builder read
about 105 KB (27k tokens) at startup, 82% of it milestones.md and workflow.md §3.

* **Decisions** D62-D79 (beb98df), including yesterday's proposals: D62 parallel builders (replaces D32),
  D63 the E&BConfig check is one-time, D64 `NOT EXACT` general with a tracker, D66 pair-loss classes
  (0x8C1520 is class iii), D69 safety rules numbered, D71 attribution from the harness, D72 subagent
  models, D73 imports (wlib/wnet, tcpmgr/tcpcon, BinkMovie; CordArrayClass's remove on wlib), D74-D78
  tool and define items, D79 the reference decoder note. D10, D16, D23, D32, D36, D39, D43 reworded.
* **[builders.md](builders.md)** (2592b1b): the tracked builder brief: startup set per kind, rules, the
  model table, report format. CLAUDE.md names the pool, `pairs_cmp.py`, D27, D69, D71.
* **workflow.md §3** (af0588b, 6d472f2): 100 traps / 37 KB -> 74 / 18 KB in §3.0 Everyone plus nine
  topic buckets; 24 duplicate groups merged; §2.1 fixed for D41/D61; tool quirks moved to the three
  READMEs (9fa4de5). kernel-map.md: the four "WM_QUIT sets +0x0C" claims corrected, header current.
* **milestones.md** 51 -> 20 KB (b62091d): one heading per item; results in library-matching.md §3.8,
  the decode-path table in [decode-path.md](decode-path.md); §4 here collapsed (338be7e); the overnight
  reports archived under `docs/archive/` (116d671).
* **Tools** (626e375 ... 429ea4e): `tools/vc6/pairs_cmp.py`, `tools/re/roots.py` (every external root;
  toolchain.md §3), the rerunnable night scripts promoted (zhall, dated, strscan, mark), `notexact.py`
  -> `docs/data/not-exact.md` (38 functions in 15 files), `tools/vc6/README.md`; `compare.py` retired
  (its staged deletion landed in beb98df). vidtest_e2e/exitcodes kept separate (little shared code).
* Startup set per builder now about 43 KB (app), 39 KB (library), 35 KB (harness).
* Models and tokens: audits Opus 107k + 127k, Sonnet 96k + 78k; edits Opus 134k, Sonnet 168k + 108k.
* Open: the earlier research's audit (D79) before housekeeping closes; the supervised E&BConfig
  acceptance check (D63).

### 2026-10-24 (evening, unsupervised): M1 and M2 met; transport, kernel, connections, packet factory; app 251 -> 458

Plan and log: `.work/milestones/plan-7h.md`. Two builders in parallel, coordinator heartbeat.
Both builders were cut off by the API usage limit 19:40-21:10 and resumed.

* **M1 met**, gates (a)-(d) ([milestones.md](milestones.md) §2): M1.4 rebuilt `app\wwudp` (CordCon,
  CordMgr, bandwidthaccounting.cpp; 8fd795a ... 6d1e607, cdc8e0b): the Alex datagrams re-encrypted
  with their keys go through our CordCon: alex3 26,106 messages in order, all 14,812 client ack lists
  accepted, the peer accepts all our datagrams, 54/54 handshakes (WWLib's XMP at run time); the
  `loop` test over a lossy link delivers everything. M1.5 (2c5d257, 96ff38a): every Net-7 AUX_DATA
  decodes and equals the reference decoder's field-change log, Net-7's known differences counted, a fourth one found.
  The client's TCP transport is app\common\tcpconn.cpp on WWOnline's TCPMgr/TCPCon (D47 finding).
* **M2 met** apart from the supervised E&BConfig run: land matched from the original's draw calls
  (a d3d8 proxy dump: f844701, 138acbb, 6d5960e), frames 500-900 within 1.7-3.2%; gate (d) clean in
  a checked build (de51d56). D61 NDEBUG (c9a9a2c).
* M1.3 leftovers (6b687cb ... c2dadc6): the update collector, change table, root encode, second root
  class, policy.cpp; client-path re-encode of the EA captures 39,716 / 43,978 byte for byte (the rest
  differ only in server-policy fields). One accepted loss, 0x8C1520 (a false match while MoneyClass
  was a bare __int64; library-matching.md row): for the user to confirm. Root constructors (b568424
  ... fa2fd99): Player 0x752510 and the factory 0x6F6000 exact (VC6's inline budget model, workflow.md).
* Kernel ([kernel-map.md](kernel-map.md), b5f5c26): kernel.cpp (b4d60a6, 18 exact) with kerneltest;
  the connection layer circbuff/connect/udpconn (8a6066b, 89d1db0, 5938ed4); MenuTask and the movie
  tasks (63bcecd; movietest plays the game's Westwood logo through BinkMovie); pk.cpp's helpers, the
  factory 0x8696C0 and its 16 KB switch 0x869790 byte for byte with 263 generated packet classes
  (1ea47fd, c031316, f1b49de, `tools/re/pkfactory.py`, `pkclasses.py`). udpreplay `conn` runs the Alex
  messages through UDPConnection, the stream framing and the client's factory.
* Names: pre-D56 names tagged (3d6e067); the review list is `docs/data/names-review.md`.
* Proposals waiting for the user: import WWOnline wlib/wnet (Zero Hour matchbot) plus Renegade's
  RenegadeGR tcpmgr/tcpcon; import Renegade's BinkMovie; ENABLE_WWPROFILE as an app define; the logger
  stand-in; count the 17 template leaf Reads as pairs; names for unknown_changes.* and UnknownCOClass;
  CordMgr::Connect returning the session id; CordArrayClass's remove with wlib; funcmatch's string
  evidence to check `__FILE__`; the draw dump as M2's regression check; Net-7 difference 4 recorded
  (library-matching.md §3.8).

### 2026-10-24 (afternoon): milestones M1 and M2 (D42-D61); app 136 -> 251, first linked programs

Plan: [milestones.md](milestones.md) (M1 capture harness, M2 working VIDTEST); VIDTEST spec:
[vidtest.md](vidtest.md). Coordinator plus builders, two in parallel from M2.1 on.

* M1.0 (f192045): `tools/harness/convert.py` turns the Alex dumps into datagram files and decodes the
  EA-UDP ack list (type bit 0); it holds all 26,078 / 2,779 messages of the reference decoder's import plus 28 / 12 that import lost
  (D52). The reference decoder's readings saved in `.work/harness/ref/`. `capharness.exe`, the first VC6 program.
* M1.1 (fc533b5): the decode path mapped (milestones.md §2.3). M1.2 (985ee8e ... 49cbe67): codecs,
  lookup build, encoder records, kv_pair_string, the time field: app 136 -> 164. D54's ambiguous
  column (5e83b12). M1.3 (bf8ac0b, 5afdaeb): the 11 roots and 42 nested classes generated
  (`tools/re/auxroots.py`), names matched to Net-7's AuxClasses by structure (D59): app 164 -> 202;
  `auxreplay`: all 43,978 EA AUX_DATA decode, 43,977 re-encode byte for byte (D60), every field
  change equals the reference decoder's field-change log (gate (c)), Alex 4,289 / 366 pass.
* L1 (160f88d): `tools/vc6/build.py` builds all eight libraries. L2 (326acec ... 8c57559): the 12
  never-imported EA files (D49, wwlib 565 -> 586, wwaudio 351 -> 360), mss32.lib from the game's DLL
  (D51, `mkmss32.py`), `stubs/` (D44), the first executable `programs/vidtest`. D55: ww3d2/pot.cpp
  NOT_LINKED (2033c04, WW3D2 2477 -> 2475, D20 duplicates).
* M2.0 (8c511b2): VIDTEST's spec. M2.1-M2.3 (4be6086 ... 4b32fdb): BST29A, every W3D scene, sky,
  overlay, text, the three tests, results.ini, exit codes, E&BConfig's launch (no registry), a d3d8.dll
  proxy capturing the original's frames (frame 300 within 3.2%). M2.4 (646aa86, 43b30f0): stars (8
  exact), splines, land, decorations: app 202 -> 251; Max Polys 30729 / PolyBudget 30000 as the
  original; land frames 35-40% different (orientation open). D61 NDEBUG (c9a9a2c, pairs unchanged).
* Names: D56 certainty tags and `tools/re/names.py` (6f8e791); pre-D56 names tagged (3d6e067).
* Lessons (workflow.md §3): never a modal dialog or a screen grab in unattended runs (two
  message boxes reached the user); never run a program in the game's install folder; two builders
  waiting for each other's clean tree deadlocked for an hour (rule replaced).
* Open: M1.4 (wwudp, gate (a)) and M1.5; M2 gate (d), land orientation; the supervised E&BConfig
  run with the user once all features are done.

### 2026-10-24: the kv encoder, the interpolator's Unknown_00, encode.cpp and policy.cpp starts; app 92 -> 136

* `app/shared/kv_encoder_base.cpp`/`.h` (88a52a3, with the interpolator below: 33): `KVEncoderBaseClass`
  (vtable 0xAEB62C, the destructor only; `Data` +4, the pair list +8) and its Data (0x3C bytes:
  `PrefixList` list<string> +0, `FlagList` list<int> +0xC, `Flags` +0x18, the joined prefix +0x1C, the
  last key +0x2C). Exact: dtor, Make_Key, Get/Set/Push/Pop_Flags, Push/Pop/Set_Prefix, the static join
  0x4B43E0 (called directly, no thunk; inlined in Push_Prefix and Set_Prefix), Unknown_Test 0x4B4820
  (the even 0x155 and odd 0x2AA bits of `Flags & mask`), Add_String 0x4B48A0, plus the STL instances
  they use. The constructor 0x4B3850 differs in register allocation only (`this` in ebx).
* `kv_pair.cpp`/`.h` (`KVPairClass`, vtable 0xAEB7EC: dtor + 6 pure; the key a `PermString`, two
  flags), `kv_pair_string.h` (`StringValueClass` moved here from delta_string.h, `KVPairStringClass`),
  `kv_list.h`; the typed accessor/pair templates `KVAccessorClass<T>`/`KVPairTemplateClass<T>`
  (float: 0xAE78D0/0xAE7860) and the member template `Add_Value<T>` in the encoder header.
  `DeltaChildVisitorClass` now derives from `KVEncoderBaseClass` (slot 0 is the base's destructor).
* `delta_interpolator.cpp`: Unknown_00 0x4A6930 (EndTime/ChangePerTick through their vtables via
  `((DeltaClass &)EndTime).Unknown_00`, then "CurrentValue") and `DeltaInterpolatorValueClass`
  (0xAEAF08: Get 0x4A6B30 exact, Clone 0x4A6AF0 exact but a twin). `delta_template.h` (49bfa5b):
  the generic Write/Read (lines 91/110) and `Unknown_00` with `DeltaTemplateAccessorClass<T>`
  (Set 0x4A6F00); the float Read/Write/Unknown_00 are exact but `multiple` (int twins in game
  code), so uncounted. MoneyClass got `operator!=` (VC6 instantiates a template's virtuals with the
  class, so Set_Value is compiled for money).
* name.cpp `Get_Rest` 0x4ADBB0 (a7c449c); lookup.cpp `Find` 0x4AC9F0 (3ceecb0): the map at +0x10 is
  `std::map<const char *, DeltaClass *>` (the key copied to a temporary). The name tests
  0x4ACA90/0x4ACB20 call find 0x4ACBD0 out of line; every variant tried inlines it.
* `encode.cpp` (9d651e5, 4): `RecordTableClass` (`Records` +0, `Count` +4; ctor line 512, dtor, two
  loops over the records' slots 4/8). `policy.cpp` (6dd05ad, 3): `CompiledPolicyClass` dtor 0x4AE850,
  `Get_Action(int id, int *value)` 0x4AED50 (line 334), `PolicyClass::Get_Action` 0x4AF490 (444).
* Not done: the time/money constructors 0x4A8150/0x4A7080 (byte-identical to each other: `multiple`,
  0 countable; and `template class X<DurationClass>;` in delta_time.cpp is error C2949 next to
  delta_template.h's `extern template`: §3), the time field's Unknown_00
  0x4A8330/0x4A8630 and accessor 0xAEB244 (ticks to wall-clock time, a guarded static).
  Drafts and scripts: `.work/day/D/` (`layout.py` lays a tree file out so asserts land on given
  lines; `m.sh` runs appfile match with the mirror's include dirs).

### 2026-10-24: auxdata continued: interpolator, string, name, permstr, linearbu; app 39 -> 92

* `delta_interpolator.cpp`/`.h` (7fad8a2, 7): `DeltaInterpolatorClass` (vtable 0xAEAE54, size 0x250;
  `EndTime` +0xA0, `ChangePerTick` +0x130, `StartValue` +0x1B8, `LastTime` +0x240, `LastValue` +0x248,
  a flag +0x24C). Set 0x4A6590 needed a user-defined `DurationClass::operator=` (duration.h: the
  implicit copy schedules the two stores after the vtable load). The time field's constructor
  is out of line (0x4A8150) where the float fields' inline: `extern template class
  DeltaTemplateClass<DurationClass>;` in delta_template.h (VC6 extension, C4231) gives it; an explicit
  specialisation declaration of the constructor is ignored by VC6. Unknown_00 0x4A6930 left.
* `delta_string.cpp`/`.h` (cafc4f4, 9): `DeltaStringClass` (0xAEB0E4, `String` +0x84), its value
  object for the encoder (`DeltaStringValueClass` 0xAEB13C on `StringValueClass` 0xAEB150) and
  `DeltaChildVisitorClass::Add_String` (kv_encoder_base.cpp's 0x4B48A0) in delta.h.
* `name.cpp` + `permstr.h` (83f8fc8, 12): `NameClass` = `Name`, `Parts[MAX_NAME_LENGTH]` (10),
  `Count` (-1 until `Split`), `HasWildcard`; `PermString` (the client's name, from 0x4ADFF0's message)
  is one interned pointer. `Append` adds `rhs.Count` inside its copy loop (the client's bug, kept).
* `permstr.cpp` (e3209af, 6): the hash table (`s_Table`/`s_Size` 1511/`s_Count`), a counted
  `PermStringInitClass` static in permstr.h (every including object has one: the client's 0x4A60A0,
  0x4ADC50 ...), `Find` (one-at-a-time hash).
* `linearbu.cpp` (4849fba, 18 + 3 four-byte getters at markers): `LinearBufferClass` size 0x1C
  (`Buffer`, +4 unknown, `Size`, `Capacity`, `GrowBy`, `Pos`, `OwnsBuffer`).
* delta.cpp's `Unknown_20` 0x4A53A0 (844659a): the base call goes after the `if`. `Add` 0x4A5670 is
  still not exact: the client calls `size()` 0x4A58C0 out of line in `_Last = S + size() + M` and
  lays out the `_Last - P < M` branch's calls differently: a different `<vector>` form (D11).
* VC6 inlines functions defined later in the same file (NameClass::Match_Parts, permstr's
  Is_Prime and entry constructor, linearbu's Seek/Read/Grow). Drafts: `.work/day/C/`.

### 2026-10-24: the Delta object model (app\shared\auxdata); app 11 -> 39

* `delta.cpp`/`delta.h` (8e0b409): `DeltaClass` (the earlier research's DeltaBase; D24 name, D10
  `Unknown_XX` virtuals/`m_UnknownXX` members) and `DeltaCompoundClass`. Layout from the ctor
  0x4A4D40: vtable 0xAEADC8 (a one-slot interface, `_purecall`) then 0xAEAD70 (18 slots); +0x04 a
  per-class global (0xBE3A78; the compound's 0xBE3A74), +0x08 `Name` and +0x3C `FullName` (NameClass,
  0x34 bytes, ctor 0x4AD380), +0x70 a 0/1/2 state, +0x74 `Parent`, +0x78 the slot in the parent,
  +0x7C a running index, +0x80 = 0x3FF; size 0x84. Compound: vtable 0xAEABE4 (19 slots, +0x48 new),
  `Children` std::vector at +0x84, `Lookup` +0x94 (the assert's name, made by `Get_Lookup`, line
  227), +0x98 the subtree's index count; size 0x9C; its ctor is inline (0x4A6110 inlines it).
  0xAEAB88 is a class derived from it (dtor 0x4A3D00), not DeltaCompound itself. 21 functions; not
  exact: `Unknown_20` 0x4A53A0 (register-save placement) and `Add` 0x4A5670 (the client calls
  vector::size 0x4A58C0 out of line inside the inlined insert).
* Slots 0x2C/0x30 are `Read`/`Write` (the leaves' overrides use linearbu's 0x4AACE0/0x4AAFC0);
  `Read`'s second parameter is an 8-byte base the time field subtracts (`const DurationClass &`).
  Leaves are `DeltaTemplateClass<T>` (delta_template.h, whose `__FILE__` is absolute).
* `delta_money.cpp` (1832936, 2: the 8-byte money at +0x88), `delta_time.cpp` (b5cf75d, 2: 4 bytes on
  the wire; `duration.h`'s `As_Int` with `FAIL_IF(Time > INT_MAX)` on line 190), `lookup.cpp`
  (a791677, 3 index accessors at lines 67/78/90). New headers holding only what is used: name.h,
  lookup.h, linearbu.h, delta_template.h, `app/shared/money.h`, `app/shared/duration.h`.
* Header `__FILE__`: `..\shared/duration.h` is an include directory `..` plus
  `"shared/duration.h"` (VC6 joins them with `\`), so E&B ran cl in a directory one below `app`
  (e.g. `app\client`). Ours include `..\duration.h` relative to the .cpp: the string differs, the
  bytes don't (relocation). Open: see §3 (header `__FILE__` strings).
* Drafts in `.work/day/B/`: `i1.cpp` (`DeltaInterpolatorClass::Set` 0x4A6590, same length as the
  client's, one `pop esi` scheduled differently; fields `EndTime` +0xA0, `ChangePerTick` +0x130,
  `StartValue` +0x1B8 from the ctor's strings, `LastTime` +0x240), `d2.cpp` (delta.cpp's misses),
  `vary.py` (variant runner for appfile drafts).

### 2026-10-24: D23, E&B's own code in the importer (`app`); bitset.cpp, app 0 -> 11

* The importer has a non-EA library `app` (`APP_LIBS`): every file `RECONSTRUCTED`, paths under
  `app/` as in `e:\build\beta\G\app`, `/MT /W3 /O2 /Ob2 /GX` without `NDEBUG`, objmatch searching
  only the app region 0x401000-0x8C1B50. `load_state` builds its entry from `RECONSTRUCTED`;
  assemble/refine skip it; replay and report handle its subdirectories. Every other library is
  unchanged, none lost (86757d4).
* `__FILE__`: the client's strings are absolute, and VC6 takes the source path as cl was given
  it. verify names app sources `e:\build\beta\G\...` through a `subst E:` and a junction in the VM
  (vc6.py `source_alias`), so bitset.obj's string is the client's byte for byte.
* `app/shared/auxdata/bitset.cpp`/`.h`: `BitSetClass` (D24), asserts on lines 8-113 through the
  layout (D25), the reconstruction notes at the end of the file. verify counts 11 of the 12; the
  10-byte destructor is too short to search for and is exact at its appfile marker.
* `app/shared/unknown_debug.h`: G_ASSERT 0x4E9410 and FAIL_IF 0x4E9450. Their object lies in
  app\shared between event.cpp and ini_manager.cpp (alphabetical link order), with E&B's operator
  new/delete, and has no `__FILE__` string: the directory is known, the name neutral (D10).

### 2026-10-24: D22 (PS), Zero Hour's bump shaders in wwshade; wwshade 138 -> 159

* The 14 shdpp.exe headers (`*.vsh_code.h`/`*.psh_code.h`) EA's release lacks are reconstructed
  from the client's .data 0xBCB8FC-0xBCEBB7 (`.work/d22/gen_headers.py`). They are shdpp's own
  form, DWORD arrays (text, NUL padded to a DWORD, a 0 terminator), not `char[]`:
  `ShdHW*Shader::Create(DWORD*)` (0xA0F3E0, 0xA0F5A0) takes them, and a `char[]` would pick the
  file-name overload. Each object's .data equals the client's bytes. The text has Zero Hour's
  instructions with the macro names substituted (4c0fbf6).
* `ProjectorClass::Peek_Mapper` comes from Zero Hour's projector.h (0xA126D0 reads Mapper at +0xC8).
  The six .cpp are copied from Zero Hour with E&B's `ShdInterfaceClass(def)` base call (new block
  `SHD_BUMP_PATCHES`). Gains +21: the 19 from 6i plus both `Setup_Self_Shadow_Info` (0xA126D0,
  0xA15240). None lost, replay 0 differences (523467f).
* No near-misses left in the new files. Init, destructors, Copy_Vertex_Stream and Peek_Texture are
  exact but `multiple`. The files' out-of-line `DX8Wrapper::Set_DX8_Texture_Stage_State` differs
  from ww3d2's 0x9217E0 (0.47), as in the older wwshade files.

### 2026-10-24 (overnight): Zero Hour's dated edits, D9 finished, wwlib's E&B allocator; 3,925 -> 4,062

Unattended run 00:02-07:40: a coordinator and one builder subagent at a time (no parallel
builds: `verify` shares `%TEMP%\enb_import` and the VMs' scratch), queue and log in
`.work/night/plan.md`, a heartbeat every 30 min, each builder's report in
`.work/night/item*/report.md`. 65 commits `b9b0c61..a1cbc31`, none pushed. Morning report:
[overnight-report-2.md](archive/overnight-report-2.md).

* D9's texture leftovers (items 1, 6f): `Get_Reduction` 0x94BC20, `TextureClass::Apply` E&B's
  form 0x94BC80, `Get_Texture_Memory_Usage` one non-virtual base function with Generals' split
  body 0x94C1D0, `Set_Mip_Mapping` compiled in texture.cpp 0x94CA20 (b71c5dc..508d352); Zero Hour's
  08/05/02 `TextureFilterClass` dropped for Renegade's five filter members, all four constructors
  0x94B240/0x94B3D0/0x94B5B0/0x94B660 exact, texturefilter.cpp/.h `NOT_LINKED` (0b8f6ce, 5534f64,
  new block `TEXTURE_FILTER_PATCHES`).
* `DX8Wrapper` device code (item 2, e278d81..8ee8171, +25): WW3D2 built `/D USE_WWSHADE`,
  `Reset_Device` E&B's body, E&B's lost-device restore `Unknown_Restore_Device` 0x921C50 (D10),
  transform cache, `Init` zeroing; `Create_Device` without `DX8Wrapper_PreserveFPU` 0x921EF0 (cc535c7).
* Zero Hour-only edits in Renegade-based files (items 3, 6b, 6d, 6e): `//DMS`, ZH-tagged and
  commented-out code found by diffing every EA copy and by scanning the client's log/profile
  strings (`.work/night/item6b/strscan.py`, `fdiff.py`; `item6d/zhall.py` splices every
  differing donor function and reports gained/lost). `GapFillerClass` 0x959A80, animobj, scenes,
  sorting loops to `Get_Max_Textures_Per_Pass`, texture loader HSV shift, DDS guards, dx8caps
  (Matrox Parhelia), `Alter_Avatar_HTree` 0x98A2D0, `ShaderClass::Apply` with `GRADIENT_MODULATE2X`
  0x93B510, `post_process` 0x9BCC40, WW3D's static sort lists and `SHD_FLUSH` (694a02b),
  `Post_Load_Process` 0x9475E0 (4fc992a), `WW3D::Init` 0x917660.
* Dated-comment sweep (item 6i, `.work/night/item6i/dated.py`: 1,116 dated comments + 1,770 VSS
  `$Modtime`s against the client's bytes) fixed D9's cutoff: Zero Hour's KM/KJM (shader system)
  line is in through 08/01/02; the 08/05/02+ texture redesign and the Generals team's MW/GTH
  changes are out, whatever their date. D9 reworded accordingly (§2). Gain: the asset manager's
  07/01/02 shader-loader registration 0x91C2C0 (d0a68f5).
* `dx8webbrowser.cpp` from Zero Hour (item 6k, b8d3dd7, +19): `BrowserEngine.tlh`/`.tli` generated
  by VC6 from E&B's own `BrowserEngine.dll` (same GUIDs as EA's, plus `querydata` and
  `Post_MultiPart_Form`) as reconstructed files (D21); vc6.py syncs `.tlh`/`.tli`.
* Distortion (item 5, 22ae97e): external linkage of the list and an inline `expf` give 0x995E20,
  0x996180, 0x997DE0; the draw 0x9961E0 is still 369 masked rows.
* wwdebug/wwlib (item 7b): the memory log's 23 categories (client name table 0xBCB228, 06cca35,
  +11), `FastAllocatorGeneral`'s 0xE30-byte E&B layout with out-of-line `Alloc` 0x9DAE10, `Free`
  0x9DAF60, `Unknown_Reset_Statistics` 0x9DADA0 (ac56d39..b9f8783), `Read_Frame` 0x9EB4C0.
* Item 4: the meshmatdesc "inlining flip" did not exist: 0x4E9CB0 is E&B's `::operator new`
  (`FastAllocatorGeneral::Get_Allocator()->Alloc`), never inlined; the three fragile functions
  differ in index load order that moves with the number of symbols declared before the classes
  (noise). Only 0x947310 is lost (D18).
* Tools: `gaps.py` and every tool use verified function starts (cd18170, 5d7079f; symbols 6,736 ->
  6,858, gaps rows 652 -> 593); importer lock `.work/locks/importer.lock` (fa14686); VM pool
  `tools/vc6/pool.py` (b156da0, orphan reclaim 92ca150) and `importer.py verify --pool K`
  (4b1f45b); `tools/re/appfile.py` for E&B's own files (c4d34c9: `app\shared\auxdata\bitset.cpp`
  12/12 exact as a draft in `.work/night/item7/try/`, not in the tree).
* Old build VMs retired (07:00): the two `VC6 SP4 (8799)` clones removed and deleted;
  `.work/vc6_hosts.json` lists only Win10; `enb-vc6-01/02` stopped, free.
* E&BConfig.exe / VIDTEST.EXE assessment (`.work/ebconfig/assessment.md`): neither is a quick
  win; open question in §3.
* Verified: each commit by its builder (verify of the affected libraries, pair sets vs
  `.work/night/imports.start.json`, `replay` 0 differences), cumulative pairs checked by the
  coordinator after each item. In-tree counts: WW3D2 2,362 -> 2,477, WWDebug 67 -> 81, WWLib 557
  -> 565; WWAudio 351, WWMath 370, WWSaveLoad 80, wwshade 138 unchanged; total 3,925 -> 4,062
  (+137). Lost pairs, all COMDAT duplicates still matched elsewhere (D20): (animatedsoundmgr,
  0x6CF410), (meshmatdesc, 0x497330), (meshmdl, 0x909500), (ww3d, 0x909500), (texturefilter,
  0x42F250/0x53F1C0/0x9217E0; file dropped; 0x94CA30 moved to texture), (wwmemlog and wwprofile,
  0x9CEFF0).
* Decisions: D20-D32 (user, during the night; §2), D9 reworded, D18 kept. Open: §3.
* Next step: D22 (PS, §4 item 1), from `.work/night/item6i/report.md` (PS) and its trial script
  `.work/night/item6i/shdtry.py`.

### 2026-10-23: D19's distortion draw rebuilt (not byte-exact); its neighbours are uncountable

* `UnknownDistortionClass::Unknown_Render_Distortion` (0x9961E0) written from the client's
  disassembly (d665c29): the camera-facing square projected to whole pixels, the on-screen part
  of the back buffer copied to a `TextureClass` (clamp, no filtering), world/view/projection set to
  identity with `_PresetOpaque2DShader` (depth always) and PRELIT_NODIFFUSE, a grid of 8-pixel
  cells in three function-static `DynamicVectorClass` lists with each texture coordinate sent
  through `Unknown_Distort`, drawn through the dynamic VB/IB, view and projection restored. Kept
  as the client has them: uv0 scaled by the width only, uv1 by the height only; the early returns
  for fewer than 2 cells leave the view and projection unrestored.
* Not byte-exact: 6,672 bytes vs 6,759; 369 of ~1,530 instructions differ with stack offsets and
  relocations masked. Left: the view restore (the client builds a `??_H` Matrix4 temporary, ours
  doesn't), the index loop's rotation and registers, the static guards' registers, a frame 0x18
  bytes bigger in the client (0x208 vs 0x1F0: locals not found). Forms that did help: early
  returns with their own cleanup copies; `Get_Translation(Vector3 *)`; `y1 >= height ? ... : y1`;
  `sw`/`sh` before the clamped size; a `Vector2 offset` temporary; `i0`/`i1` index locals.
  Tried and worse: `RefCountPtr` locals (the client doesn't null-check texture or back buffer),
  `Vector2` sum for the vertex, `for (int ...)` loops, hoisted `v`, index-count orderings.
* Neighbours 0x997EA0..0x9981A0: no source needed. They are
  `DynamicVectorClass<UnknownDistortionClass *>`'s template members (plus static cleanups
  0x997C50/90/D0, 0x997D90, 0x998120); our object compiles each byte-identical, but the client has
  33-140 identical copies, so `multiple`, 0 counted (new trap in workflow.md §3). 0x9981F0 is an
  8-byte `this`-adjusting thunk (too short to count).
* `Unknown_Add_To_Distortion_List` 0x995E20: only `add ecx, eax` vs `add eax, ecx` (registers).
* Verified: `importer.py verify --lib ww3d2` 0 errors, 2,362 in tree (unchanged); pair sets +12
  (template instances in unknown_distortion), 0 lost; `report`; `replay` 0 differences.
* Tools: none in tools/. Scratch in `.work/`: `dist_compile.py` (compile the file alone, objdiff the
  draw), `dist_mask.py` (diff with esp offsets/relocations/immediates masked), `dist_try.py`
  (score variants), `dist_head.py` (exact sets vs HEAD), `objmatch_all.py` (objmatch without the
  5-address cap), `dist_v*.py` variant specs, `imports.before-draw.json`.
* Decisions: none. Open: item 1 rewritten.
* Next step: another open item (WW3D2 near-misses, `GapFillerClass`, `_Get_Total_*_Size`), or the
  draw's frame size if a lead turns up.

### 2026-10-23: saved Ghidra pseudocode (`pseudocode.py`); REA timeouts confirmed

* REA's raised timeouts (ad3cf4a) work: the first `batch_decompile` came back after Ghidra's
  analysis (285 s), moved to a background task at Claude Code's 120 s mark. Correction to the
  entry below: the distortion session's retries did **not** work. All four REA calls (line groups
  0x97BAF0/0x97C900/0x97C360; distortion 0x9961E0 and 0x997EA0..0x9981A0) failed with
  `provider_timeout`, hidden because the ~85 KB-per-item error payloads overflowed the result.
* Then 0x9961E0 failed differently: Ghidra defines no function there (reached only from the
  vtable at `.rdata` 0xB13984) and REA can't create one. So: `tools/re/pseudocode.py` +
  `tools/re/ghidra/DecompileTargets.java` (eb64985) run Ghidra headless, create a function at
  every `clientimage.py` start, name them as `client.py` does, and save pseudocode for every start
  not byte-exact to `.work/pseudo/`: 25,063 ok, 0 failed, 119 refused (false starts
  mid-instruction, all in the library region). Full run 80 s once the project exists.
* CLAUDE.md: the saved pseudocode is now step 3 of the tool order, ahead of REA (user).
  workflow.md §3: the vtable-only trap updated, one new (Ghidra's dotted-path and `&` limits).
* Noticed, not followed up: gaps-ww3d2 places `~MeshModelClass::Init_For_NPatch_Rendering?` at
  0x95A482, mid-instruction; the location is probably wrong.
* Verified: the run's counts above; spot checks of 0x9961E0 (28 KB) and 0x97BAF0 (callees named).
  No library changed, no `importer.py verify` needed.
* Next step: rebuild the distortion draw 0x9961E0 (D19) from
  `python tools/re/pseudocode.py --show 0x9961E0`, then its neighbours 0x997EA0..0x9981A0.
* Outside git: `.work/pseudo/` (59 MB, `index.tsv`), `.work/ghidra/` (inputs, a copy of
  client.exe, `headless.log`), the Ghidra project in `%LOCALAPPDATA%\EnB\ghidra`.

### 2026-10-23: WW3D2 particles, textures, line groups (after the mapper swap)

* WW3D2 2,310 -> 2,352 (+42); gaps 347 -> 316; `symbols.json` 6,703 -> 6,736 (none gone). Every step
  verified by pair sets (`importer.py verify`), `replay` (0 differences) and committed on its own
  (a45b407..01c469f); each fix is a row in library-matching.md §3.6.1. Full verify at the end: all
  seven libraries 0 errors, WW3D2 the only one changed.
* D18 taken (user): meshmatdesc's byte-only include-order fix is not applied; kept on record.
* Particles (cluster 2 of the near-miss entry, done but for one function):
  * `PointGroupClass::Render` and `LineGroupClass::Render` take the object's transform (E&B; the
    app passes static matrices). `LineGroupClass::Render` rebuilt with three E&B members (transform
    setup, index fill, vertex fill; names not known, D10): all four match.
  * `ParticleEmitterClass`: emitter-space emission when `UnknownFlag` (+0xFD) is set (two E&B
    members), constructor (`FutureStartTime` and `EmitRemain`), `Create_New_Particles`
    (reciprocal frame time, by-value lerp), `Build_Definition` passing the flag,
    `Unknown_Get_Position` (0x97E260).
  * `ParticleBufferClass`: Zero Hour's line groups (`GroupID`, `Render_Line`), E&B's bounds in
    object space (`Update_Cached_Bounding_Volumes`), Zero Hour's `Scale` (+
    `SegLineRendererClass::Scale`). `ParticleEmitterDefClass::Save_Extra_Info` writes `m_ExtraInfo`.
  * Left: `PointGroupClass::Render` (0x954960) matches structurally but its block-scope stack
    slots are placed differently throughout; no lead.
* `WW3D::Set_Render_Device` (cluster 3): both overloads call E&B's `Unknown_Reset_Distortion_List`
  (0x995DC0). Matched in fact but not counted: the overloads are now twins (objmatch `multiple`).
  The E&B module behind it (a screen-distortion effect) is D19.
* Textures (cluster 4, D9): `Peek_D3D_Base_Texture` without `LastAccessed` (+4, `TextureClass::Init`
  among them), E&B's `Invalidate` (Zero Hour's commented-out "battlefield version" plus the
  load-task test), `Invalidate_Old_Unused_Textures` keeping last frame's count (+ an E&B getter),
  `Get_Texture_Information` taking a `StringClass &`, `TextureLoader::_UnknownInvalidationTime`
  (0xC01290, set by the app). Left: the four `_Get_Total_*_Size` functions inline two different
  memory-usage bodies split on `Initialized` (not found in any EA copy); the `TextureClass`
  constructors, `Apply`, `Get_Reduction` and `DX8Wrapper`'s device code as before.
* Not started: `GapFillerClass` (cluster 5).
* Tools: none new. One trap added to workflow.md §3 (twin overloads after an added call).
* Verified: as above. Nothing run live. REA opened once (Ghidra hit its 330 s startup deadline on
  the first batch; the retry worked but was not needed in the end); closed.
* Decisions: D18 (user). Open: D19 (proposed below).
* Next step: D19's module if the user agrees; else `GapFillerClass`, the `_Get_Total_*_Size`
  family, or the remaining D9 texture items.
* Outside git: `.work/*_edit.py`, `*_variants*.py`, `*_spec.py`, `*_reasons.py`, `*_edited/` for
  each step; `.work/imports.mapper-base.json` (the session's baseline), `.work/pairs_cmp.py`.

### 2026-10-23: WW3D2 texture mappers from Zero Hour

* WW3D2 2,242 -> 2,310 (+68 client functions); gaps 407 -> 347; mapper.obj, matrixmapper.obj and
  vertmaterial.obj now have no gaps. Other libraries unchanged (full `verify`, 0 errors).
* The previous entry was wrong about the cluster: the client *has* Zero Hour's texture-matrix
  mappers. The vtables (`vtables.py --seed`, 0xB10EAC/0xB10F00/0xB10F24) are in Zero Hour's slot
  order with `Calculate_Texture_Matrix` at slot 8, and `ScaleTextureMapperClass::Apply` 0x9322F0
  (inherited by `LinearOffsetTextureMapperClass`) sets `D3DTS_TEXTURE0+Stage`. Zero Hour's copy
  matched only 2 functions in the evidence because of its own headers; against the tree it
  matches everything. Trap added to workflow.md §3.
* Done: `mapper.cpp`/`.h` and `matrixmapper.cpp`/`.h` swapped to Zero Hour's (`trial.py`); the
  Renegade-copy Grid patches in `PATCHES` dropped (Zero Hour has them). New importer block
  `MAPPER_PATCHES`: Zero Hour's 16-float `Matrix4` constructor/`Init` (11/2001), and
  `VertexMaterialClass::Load_W3D` (0x92F2A0) building the WS mappers from the Args INI plus the
  `GRID_WS` cases (Renegade's single function otherwise).
* Pair sets against the session's baseline: 71 gained; 3 lost, of which 2 are not real
  (`LinearOffsetTextureMapperClass::Reset` 0x932CC0 stays matched in mapper.obj, only
  vertmaterial's twin went; `operator*(Matrix3D)` 0x7E6E40 is an app-region duplicate still matched
  in 4 files). The real loss: `MeshMatDescClass::Make_UV_Array_Unique` 0x947310 (register
  allocation only; meshmatdesc.cpp's known fragility, now that `mapper.h` includes `matrix4.h`).
  meshmatdesc.cpp gained 2 at the same time (0x946EB0, 0x947490), net +1. Accepted and
  written down (library-matching.md §3.6.1), as for the Grid change.
* `symbols.json` 6,580 -> 6,703 (none gone); `gaps-ww3d2`, `imports.md`, `coverage.md` regenerated.
* Verified: `importer.py verify` (all libraries), pair sets, `report`, `replay` (0 differences).
  Nothing run live.
* D18 (user): don't include `matrix4.h` first in meshmatdesc.cpp, though `tryvariants.py` shows it
  wins 0x947310 back and loses nothing in that file (no EA copy has that include order). Kept on
  record; revisit if it becomes WW3D2's last outstanding item.
* Next step: one of the remaining clusters (particles, `Set_Render_Device`'s E&B call, textures
  under D9, `GapFillerClass`).
* Outside git: `.work/m4_16f.py`, `vm_ws_edit.py` (the edits as made), `mapper_spec.py`,
  `mapper_reasons.py`, `mapper_drop_patches.py`, `mapper_docs.py`; `.work/mapper_edited/` (edited
  files, matched the addpatch result); `.work/imports.mapper-base.json` (the baseline);
  `.work/mmd_variants.py` (the meshmatdesc variants). REA not opened.

### 2026-10-23: WW3D2 near-misses: mesh rendering per mesh model

* WW3D2 2,202 -> 2,242, none lost (pair sets compared at every step); wwshade 138 unchanged.
  Five commits, 3cb45a0..5c61281; each fix a row in library-matching.md §3.6.1.
* Decisions D15-D17 recorded (the three open questions of the previous entry, all as recommended).
* The main find: E&B's mesh rendering is **per mesh model**, as in Generals: polygon renderers live
  in `MeshModelClass` (+0x84; the member was already declared, the code still used `MeshClass`'s
  list) and `DX8MeshRendererClass::Register_Mesh_Type`/`Unregister_Mesh_Type` take the model and
  keep a `_RegisteredMeshList` (0xC07618). `MeshClass` has no user lighting and no list of its own
  (size 0xA8); nor has `RenderObjClass` any user-lighting persistence (Generals' factory
  `Save`/`Load`). Kept from Renegade: the sort-level test, the delayed material passes, the
  gap-filler removal. New importer block `MESH_MODEL_RENDERER_PATCHES` (91 patches, written with
  the new `tools/vc6/mkspec.py`). +34.
* `MeshClass`'s vtable has one virtual of its own (slot 113, 0x941310, returns 0x750) where
  Renegade has `Save_User_Lighting`/`Load_User_Lighting`: `Unknown_Virtual_113` (D10).
* Smaller Zero Hour pieces (importer `PATCHES`): `compute_static_sort_levels`' screen case,
  `SegLineRendererClass::Render`'s `rgbas` argument, `MeshClass::Make_Unique(bool)`,
  `MeshGeometryClass::operator=` cloning the cull tree, `TextureLoadTaskClass`'s `HSVShift`
  zeroed; `MeshClass::Cast_Ray` without `IgnoreTranslucentMeshes`. +10.
* Clusters found, not started (each a session):
  1. `mapper.cpp` (~50 gaps): the client's `LinearOffsetTextureMapperClass` constructor 0x9329F0 is
     Zero Hour's (start offset +0x28, clamp flag +0x30, five arguments), yet Zero Hour's
     `mapper.cpp` matches only 2 functions (Renegade's 18): E&B has ZH's members without its
     texture-matrix `Apply`. `vtables.py` mis-seeds the class (0xB10E4C): seed it.
  2. Particles: `PointGroupClass::Render` takes a second argument (the transform, `[esi+0x1C]`,
     from `ParticleBufferClass::Render_Particles` 0x95DC90); `ParticleEmitterClass` has E&B fields
     at +0xE4 and +0xFD (`Emit` 0x97E330, `Build_Definition` 0x97EDE0).
  3. `WW3D::Set_Render_Device` (0x917790, 0x9177E0) calls an unknown E&B function 0x995DC0 on
     success (reads the device resolution, compares with 0x400).
  4. Texture (D9): the client's `Peek_D3D_Base_Texture` seems not to set `LastAccessed`
     (`ZTextureClass::Apply` 0x94D190); `TextureLoadTaskClass::Begin_*_Load` differ in argument
     evaluation only (Zero Hour's versions are the excluded 08/05/02 redesign).
  5. `GapFillerClass` (3 functions, 0.75-0.78): register scheduling only; cause not found.
* Tools: `tools/vc6/mkspec.py` (tree edits -> addpatch spec). Three traps added to workflow.md §3.
* `symbols.json` 6,551 -> 6,580; `gaps-ww3d2` regenerated (407 gaps, 461 at the start).
* Verified: every step by `importer.py verify` for ww3d2 and wwshade (pair sets before/after),
  `report`, and `replay` (0 differences). Nothing run live; nothing waits on the user.
* Decisions: D15-D17 taken by the user this session (as recommended).
* Open questions: none new.
* Next step: one of the clusters above; mapper.cpp has the most functions. Start by seeding its
  vtables (`vtables.py --class LinearOffsetTextureMapperClass --seed ...=0x9329F0 --list`) and
  comparing the constructors against Zero Hour's member list.
* Outside git: `.work/mmr_*.py`, `rendobj_edit.py`, `*_spec.py`, `*_reasons.py` (edits and specs
  as applied); `.work/mmr_edited/` (edited files, matched the replayed tree); `.work/odbatch.py`
  (condensed objdiffs of the top gaps), `.work/fn.py` (print one function). Three
  build VMs (`vc6.py check` passed). REA not opened this session.

### 2026-10-23: WWAudio complete (0 gaps); wwshade's ShdMeshClass

* WWAudio 298 -> 351, every function our source compiles now byte-exact (`gaps-wwaudio`: 0);
  wwshade 136 -> 138; WW3D2 2,202 unchanged in every verify. All in importer `PATCHES` (wwaudio)
  and `WWSHADE_EARLY_PATCHES` (wwshade), one commit per step, each in library-matching.md §3.6.1.
* E&B's WWAudio changes found (names not known keep D10 names):
  * `Sound3DClass`: a software Doppler effect. `Set_Velocity` takes a second float (elapsed ms)
    and sets the pitch from the velocity relative to the listener (constants 0.85, 3.4, 1/33.3);
    `On_Frame_Update` re-enables Renegade's `#if 0` auto-velocity behind `_UnknownDopplerFlag`
    (0xC404F8, the app's "Doppler_Enabled", 0x51461C) and alone updates Miles;
    `Set_Listener_Transform` tracks the listener velocity (`m_Unknown148`, `m_Unknown160`);
    `m_Unknown164/168/16C` are floats. The one-argument `Set_Velocity` of `SoundPseudo3DClass` and
    `Listener3DClass` therefore hide it (new vtable slots): the session's first guess, a non-const
    `Vector3 &` in `SoundPseudo3DClass`, was replaced (same bytes).
  * `WWAudioClass`: a pause-all flag (`m_Unknown19A5`, set by the app; `AudibleSoundClass::Play`
    queues sounds paused; `Unknown_Resume_All` 0x9F4720 resumes) and a volume lock
    (`m_Unknown19A6`, set by the new `Temp_Disable_Audio`); a per-kind instance limit
    (`AudibleSoundClass::m_Unknown80` kind, `m_Unknown7C` maximum, default 4) in
    `Reprioritize_Playlist` (`Unknown_Count_Instances` 0x9F1090), `Get_2D_Sample`, `Get_3D_Sample`;
    the `-NOAUDIO` check (`#ifdef G_CODE_BASE` compiled: it stops `Is_Disabled` being inlined, +13
    callers); `Add_To_Playlist` drops replayed sounds from `m_CompletedSounds`; providers tried
    D3DSOUND, EAX, PSEUDO, A3D, DOLBY.
  * `SoundSceneObjClass`: a deferred release (destructor queues `m_UserObj`/`m_AttachedObject`;
    flushed by `WWAudioClass::On_Frame_Update` and `Shutdown`).
  * Miles: E&B's `mss.h` had `AIL_startup()` call `MSS_auto_cleanup()` (`sdk_patch.py` restores
    it; toolchain.md §2 fix 4: a full link needs `MSS_auto_cleanup`, a static member of the SDK's
    mss32.lib, which the generated import library lacks). Also the Miles lock or mutex in several
    functions, and `Delayed_Release_Thread_Proc` skips passes while `AIL_background()`.
* `AudioSaveLoad.cpp`/`.h` dropped (D13: no save/load subsystem in the client). Accepted loss:
  its three pairs, shared helpers still matched by other files.
* wwshade: `ShdMeshClass::Update_Cached_Bounding_Volumes` (`operator *`, as `MeshClass`) and
  `Load_W3D` (`Name` set inline, no sub-mesh names). `Render` already matched (stale note fixed).
  `Get_Num_Vertices`, `Get_Sub_Mesh_Count`, `Peek_Sub_Mesh` are not in the client.
* The `"UnNamed"` null test (open question below): ruled out `/GF` off, `/Gf`, `/Oa`, `/Ow`, `/Op`,
  `_NO_INLINING`, and the literal as a const pointer, pointer variable, array, cast or with an
  explicit hint. The client threads the same jump in WWAudio's `Add_Logical_Type` (a variable
  string), not with the literal.
* Tools: `tools/vc6/trydiff.py` (compile one variant, keep its object for `objdiff.py`),
  `tools/re/objrel.py` (a function's relocations by symbol). Five traps added to workflow.md §3.
* `symbols.json` 6,494 -> 6,551; `gaps-wwaudio` (0) and `gaps-wwshade` (5) regenerated.
* Commits: ca3e4c1..(this handoff) on master. Nothing pushed.
* Verified: every step by `importer.py verify` for each library including the changed header
  (pair sets compared before and after; WW3D2 whenever a WWAudio header changed) and `replay`
  (0 differences). Nothing run live; nothing waits on the user.
* Decisions: none taken by the user this session; D10 and D13 applied.
* Open questions:
  1. `G_CODE_BASE`: kept as a patch removing the two `#ifdef`s in `WWAudio.cpp`; defining it for
     WWAudio (`LIB_FLAGS`) gives the same bytes. Recommendation: keep the patch (only this file uses
     it, so the bytes can't tell a project-wide define from a local change).
  2. Names for the E&B members now understood (`m_Unknown148` the listener's velocity,
     `m_Unknown164/168` the pitch range, `m_Unknown19A5` pause-all, ...): D10 allows renaming once
     their code is rebuilt. Recommendation: keep the neutral names with the comments now in the
     headers, since E&B's real names are still unknown; rename only if the app code shows them.
  3. The `"UnNamed"` null test (above). Recommendation: leave it; the only two functions affected
     are these constructors.
* Next step: WW3D2 near-misses and the texture ones (D9: `TextureClass` constructors, `Init`,
  `Apply`, `Get_Reduction`; `DX8Wrapper` device code). Start by regenerating
  `docs/data/gaps-ww3d2` (`gaps.py --title ww3d2 --out docs/data/gaps-ww3d2
  %TEMP%\enb_import\verify-ww3d2\*.obj`) and take its highest-similarity entries, checking each
  pairing against the vtable first (the WWAudio pairings were often wrong).
* Outside git: the REA session (client.exe, analysed) may still be open; `.work/*_spec.py`,
  `*_v.py` as applied; `.work/pc_*.c` pseudocode; `.work/pairs_cmp.py`.

### 2026-10-23: wwshade's E&B code; `_MBCS` and `ENABLE_WWPROFILE` builds

* Last session's regenerated evidence had gone to `%TEMP%\enb_bytematch`, not `.work/bm2`
  (trap added); new `tools/vc6/refresh_expected.py` refreshes plan counts from new evidence
  (wwshade best copy alone 46 -> 61, 0 files below their copy).
* wwshade 118 -> 136, none lost (importer `WWSHADE_EARLY_PATCHES`): E&B's fixed-function light
  setup `ShdHWVertexShader::Unknown_Set_Lights` 0xA0FC60 (name not known, D10) and its calls at the
  end of the Simple, Gloss Mask and Cube Map DX6 `Apply_Shared`; materials set there, not in
  `Apply_Instance`; definition defaults (`Ambient` 0,0,0; cube map `Diffuse` 0,0,0; copy ctors assign
  in their bodies); Bump Specular always DX6; Zero Hour's `Get_Pixel_Shader_Minor_Version`
  (`WWSHADE_API_PATCHES`).
* Library flags (D12; importer `LIB_FLAGS`, `bytematch_all.sh LIBFLAGS`):
  * `_MBCS` in wwshade and WWAudio: `_mbslen` 0xA2BCAB has four callers, all there. WWAudio +2.
  * `ENABLE_WWPROFILE` in wwshade, WW3D2, WWAudio, WWMath (`WWProfileSampleClass`; its dtor
    0x444160 is called 212 times by E&B's own code, so probably project-wide). WW3D2 2,179 -> 2,202,
    WWAudio 266 -> 298, WWMath +1, wwshade +2. E&B dropped two samplers: `ShdDX8RendererClass::Flush`
    0xA08060, `ParticleEmitterClass::On_Frame_Update` 0x97DEF0 (patched out). Accepted losses:
    twin pairings only (library-matching.md §3.5, §3.6.1).
* Open: `ShdDefClass(classid)` 0xA0EC30 and the `ShdMeshClass` ctor 0xA08F80 differ only in the
  `StringClass("UnNamed")` inline: the client tests the pointer again after the `_mbslen` call, ours
  folds the test (not `/O1`, not `_MB_MAP_DIRECT`, all EA copies of the ctor the same).
* Evidence regenerated with the new flags (`.work/bm2`), plan counts refreshed; refine: WWMath +2
  (`matrix4` 0x8DEE60, 0x8DEEC0 from Zero Hour); its wwshade splice of Zero Hour's
  `ShdBumpSpecDefClass::Init` was a twin match undoing an E&B patch, reverted (trap added).
* `docs/data/gaps-wwshade.*` generated for the first time (7 left), `gaps-wwaudio` regenerated (48);
  `symbols.json` 6,438 -> 6,494.
* Commits: c56f895..69501fb on master. Nothing pushed.
* Verified: every step by `importer.py verify` (pair sets compared before/after) and `replay`
  (0 differences). Nothing run live; nothing waits on the user.
* Decisions: D14 (user): `ENABLE_WWPROFILE` in WWDebug too (neutral, none lost; soft). The other
  flag changes apply D12 (user); the helper's neutral name applies D10.
* Open questions:
  1. The `"UnNamed"` null test (above). Recommendation: leave it until `ShdMeshClass`'s ctor is
     worked on; it is the same issue in both functions.
* Next step: `ShdMeshClass::Update_Cached_Bounding_Volumes` 0xA09ED0, then `Load_W3D` 0xA0A380:
  `python tools/re/objdiff.py %TEMP%\enb_import\verify-wwshade\shdmesh.obj
  "?Update_Cached_Bounding_Volumes@ShdMeshClass@@MBEXXZ" 0xA09ED0`
  (gaps-wwshade pairs `ShdMeshClass` with app-region addresses; use the vtable/ctor addresses).
  After that, the 48 WWAudio gaps (`docs/data/gaps-wwaudio.md`, regenerated this session).
* Outside git: `.work/bm2` is the current evidence (new flags); `.work/bm2_prev`,
  `.work/bm2_old_noGX` older sets (deletable). Specs as applied: `.work/{lights,cdef,defs,
  gm_inst,caps,bsinit,prof,w3prof}_spec.py`; `.work/pairs_cmp.py` (pair-set diff of two
  `imports.json`), `.work/sub_flags.py` (compile a library with given flags). VMs:
  `.work/vc6_hosts.json` (three VMs).

### 2026-10-23: D9 step 3, E&B's earlier wwshade; wwshade's flags

* **wwshade was built `/GX /Ob1`** (the other libraries `/Ob2`, no `/GX`): EH frames in its
  functions; on the tree, `/GX` +16 own functions, `/Ob1` +15 more, none lost; Zero Hour's copy
  alone 46 -> 61. `importer.py` `LIB_FLAGS`/`flags_for` and `bytematch_all.sh` `LIBFLAGS` carry it;
  bm2 evidence regenerated. Kept by the user (D12).
* The client's wwshade rebuilt where it is earlier than Zero Hour's (importer
  `WWSHADE_EARLY_PATCHES`, 69 patches, each with its address): renderer node on
  `MultiListObjectClass` only, `MeshContainerClass` with a visible list per pass, no list
  containers, sorting or skins; `ShdInterfaceClass(def)` without class id or comparison/opacity
  virtuals; `ShdSubMeshClass` without visible range, sorting, legacy conversion, skins, influence
  chunk; no LegacyW3D shader or legacy loader (class ids 1..5, 5 force links, one loader);
  E&B's "Cube Map (unsupported)" shader on a D3DX cube texture. WW3D2: Zero Hour's fill light
  (`lightenvironment.obj` now complete). WWLib: Zero Hour's const `RefCountClass`.
* Byte-exact: wwshade 56 -> 118, WW3D2 2,173 -> 2,179 (+7 pairs), others unchanged, against
  `.work/sub_base_imports.json`. wwshade's 20 lost pairs are mispairings (other libraries' inline
  copies, still matched there; `0x93ED00` Renegade's `MeshClass::Clone`, which only the old
  no-`/GX` `ShdMeshClass::Clone` matched; the real 0xA092C0 now matches). `replay`: 0 differences.
  `report` regenerated.
* `shdlegacyw3d.cpp`/`.h` dropped (D13): importer `NOT_LINKED` keeps them out of plan, assemble and
  the state. wwshade now compiles with 0 errors; nothing gained or lost.
* `symbols.json` regenerated (6,317 -> 6,438 names; 0x93ED00 is now `MeshClass::Clone`).
* Not yet refreshed: the plan-time "best copy alone" totals and per-file `expected` counts in
  `imports.json` still come from the old no-`/GX` wwshade evidence (verify prints 46, and 3 files
  "below their copy"); a `refine --lib wwshade` against the new bm2 should update them.
* Commit: this entry's work, on master. Nothing pushed.
* Next session: E&B helper 0xA0FC60 (lights from the render state, end of three `Apply_Shared`);
  cube map definition functions (0xA0A840-0xA0AD60, a `.dds` path); `ShdMeshClass` ctor, `Render`,
  `Load_W3D`.
* Outside git: `.work/sub_*.py` (edits as applied, variants, `sub_mkspec.py` diff -> addpatch spec,
  `sub_cmp.py` pair comparison, `sub_flags.py` compile with given flags).

### 2026-10-23: tools for the next sessions (client.py, vtables.py, addpatch.py, tryvariants.py)

* New `tools/re/client.py` (`dis`, `name`, `refs`, `callers`): the lookups the wwshade step
  wrote as throwaway scripts, with addresses named from symbols.json, the gaps reports and
  the thunk table.
* `vtables.py`: anchors also from symbols.json (by plain name for string-proven ones), through
  incremental-link thunks; the class's own and rare slots weigh more; a table on slots that
  most tables share is "not placed" instead of a base class's table; tables end at the next
  stored vtable; `--seed Class=ADDR` and `--list`. Checked on ww3d2 against the old version:
  no primary table moved except `XSurface` (now 0xAFC29C, 16 own anchors) and
  `TexProjectClass` (0xB11120, all 8 slots); `SoundRenderObjClass`/`StreakLineClass` no longer
  land on `RenderObjClass`'s table; reported differences 214 -> 57.
* `symbols.json` regenerated (6,100 -> 6,317 names, this session's matches included).
  Removed: the names of an E&B class derived from `TexProjectClass` (table 0xAEA764, its
  `Pre_Render_Update` 0x4941A0), which the old alignment had given `TexProjectClass`'s, and
  `StreakLineClass::Clone` on a shared stub (0x9CCD10).
* Promoted from `.work`: `tools/vc6/addpatch.py --block NAME SPEC.py` (apply patches and record
  them in an importer block; replaces the `d9_`/`ws_addpatch.py` copies) and
  `tools/vc6/tryvariants.py LIB FILE V.py` (one file's variants compiled alone). Both tested:
  a new block replays with 0 differences; a no-op variant gains and loses nothing.
* REA tested: its first request runs Ghidra's full analysis (over 2 minutes), every session.
  A `close_binary` snapshot (14 KB) only caches answers already given; reopening with it, a new
  address waited over 2 minutes again. Names set in REA are lost on close. CLAUDE.md's tool
  order now says so (client.py first, REA only for pseudocode, opened early). CLAUDE.md is not
  tracked by git.
* Commits: 400ef2c (wwshade's WW3D2, merged into master by fast-forward), 8f8719c (tools),
  this entry. Nothing pushed.
* Next session: start with the step-2 entry's "Next" (the client's older `ShdSubMeshClass` and
  renderer node). Find `shdsubmesh.obj` in the client with `client.py refs`/`callers` from the
  node constructor's calls (0xA082D0), and the classes' tables with `vtables.py --seed`.

### 2026-10-23: D9 step 2, wwshade's WW3D2

* Each wwshade compile error checked against the client before adding anything: the Zero Hour
  WW3D2 features it confirms were added (importer `WWSHADE_API_PATCHES`, 61 patches, each with
  its address). Self-shadow bit, `Get_Max_Lights`, `Apply_Default_State` (an earlier form),
  `Set_Ambient`/`Ambient_Color`, two vertex streams, non-FVF vertex buffers, `Light_Environment`/
  `Render_Info`, `MAX_TEXTURE_STAGES` = 8 (dx8renderer on `MeshMatDescClass::MAX_TEX_STAGES`, as
  Zero Hour). This reverses an earlier note that it stays 2: `render_state.Textures` has 8 slots
  in the client. Details and addresses: library-matching.md §3.6.1.
* E&B's own additions found: `Set_Light_Environment(env, flag)` (0x926460; the shader renderer
  passes 0 to only record the environment) and a setter for `Render_Info`. Neutral names
  (`unknown_set_lights`, `Unknown_Set_Render_Info`) per D10.
* E&B's wwshade is older than Zero Hour's in places: no skin/sorting/visible-polygon code in the
  renderer node or `ShdSubMeshClass`, `Register_Mesh` only creates the node, `Cast_Ray` keeps
  Renegade's `IgnoreTranslucentMeshes`. Zero Hour features asked for only by that code
  (`CheckTranslucent`, `MultiListIterator::Last`, `MeshGeometryClass::get_deformed_vertices`)
  were not added.
* Byte-exact client functions: WW3D2 2,148 -> 2,173, wwshade 20 -> 56, WWAudio unchanged; sets
  of `(file, address)` compared against the session's starting `imports.json`
  (`.work/ws_base_imports.json`): none lost. `importer.py replay`: 0 files differ. `report` regenerated.
* Committed on `d9-texture` and merged into master. Three new traps in workflow.md §3.
* Decided by the user: commit and merge `d9-texture` into master; the neutral names stay (D10).
* Open: as before, Renegade's `Set_Render_Target(TextureClass *)` and `Get_Reduction`.
* Next: rebuild the client's older wwshade code from its disassembly. `ShdSubMeshClass` without
  `FirstVisiblePolygon`/`VisiblePolygonCount`/`Sorting` (and `Init_From_Legacy_Mesh_Model`,
  `ShdMeshClass::Init_From_Legacy_Mesh`/`Load_W3D` uses), `MeshContainerClass::Register_Mesh`
  (0xA081D0), the node constructor (0xA082D0: streams, then index buffer, polygons 0..count) and
  `Flush` (0xA08790), dropping `Get_Deformed_Vertices`. First find where `shdsubmesh.obj` sits
  in the client (gaps.py misplaces it). Then `TEX_CUBEMAP`, `const Add_Ref`, the
  `VertexMaterialClass` parsers. The client's `MeshClass::Cast_Ray` (0x940530) has no
  translucency test: a WW3D2 near-miss.
* Outside git: `.work/ws_spec*.py` (the patches as applied), `ws_verify.sh` (verify ww3d2,
  wwaudio, wwshade and compare with the baseline `.work/ws_base_imports.json`). The other
  `ws_*.py` helpers are superseded by `tools/re/client.py`, `tools/vc6/addpatch.py` and
  `tryvariants.py` (next entry).

### 2026-10-23: D9 step 1, the texture system

* E&B's texture system is Zero Hour's as of its 06/27/02 "texture class abstraction" (Kenny
  Mitchell's change logs in Zero Hour's files), without the 08/05/02 "redesign (revisited)".
  Client evidence: the vtables at 0xB119A0 (`TextureBaseClass`, the two trackers, `TextureClass`,
  `ZTextureClass`, 6 slots each, no cube or volume tables); the base constructor 0x94B020 (5
  arguments, `D3DTexture` at +0x34: no `IsReducible`); nothing calls `D3DXCreateCubeTexture`/
  `D3DXCreateVolumeTexture`; the load task has no vtable; Zero Hour's byte-exact Z textures,
  depth render targets, shader setters and draw-call statistics. Details: library-matching.md §3.6.1.
* Done as Zero Hour's files where the client is Zero Hour's (`texture`, `texproject`,
  `dx8texman`, `bitmaphandler`, `texturethumbnail` and four headers) and as Renegade's files with
  Zero Hour's 06/27/02 parts patched in where it is Renegade's (`DX8Wrapper`, `textureloader`,
  `ddsfile`). 122 patches (19 files) in importer.py's new `TEXTURE_API_PATCHES` block, each with its evidence.
* Byte-exact client functions: WW3D2 2,060 -> 2,148, wwshade 13 -> 20; none lost (two inline
  instances moved from `texture.obj` to other objects). `importer.py replay`: 0 files differ.
* Commits 2b6d26d, 09d4130 on branch `d9-texture` (master unchanged, nothing pushed).
  `trial.py --errors` writes every compile error. Four new traps in workflow.md §3.
* Verified: VC6 verify of ww3d2, wwaudio, wwshade (sets compared by address against the
  pre-D9 imports.json), `importer.py replay` 0 differences, `report` regenerated. Not run:
  the other libraries' verify (their headers didn't change), `symbols.py`/coverage regeneration.
* Decision: D9 reworded by the user (§2) after this step.
* Open: (1) merge `d9-texture` into master? Recommend yes once the user has looked.
  (2) Renegade's `Set_Render_Target(TextureClass *)` kept without evidence; recommend checking
  for an app-region caller before removing. (3) `Get_Reduction` has Renegade's body, not
  byte-exact; recommend trying Zero Hour's (needs `WW3D::Is_Large_Texture_Extra_Reduction_Enabled`).
* Next: wwshade. Run `python tools/vc6/vc6.py discover`, then
  `python tools/vc6/importer.py verify --lib wwshade` and work through its compile errors
  (`Apply_Default_State`, `Get_Ambient`, two vertex streams, `Get_Max_Lights`, self-shadowing),
  each checked against the client before adding. Then the texture near-misses.
* Outside git: `.work/d9_*.py` (patch builders: `d9_lines.py` Editor, `d9_addpatch.py`,
  `d9_rmpatch.py` undo, `d9_verify.sh`, `d9_cmp.py`/`d9_addr.py` compare), baseline
  `.work/d9_base_imports.json` (pre-D9 state). Three build VMs.

### 2026-10-23: importer replay; decisions D6-D11

* `importer.py replay` rebuilds every library in `.work/replay` from the EA copies plus the
  decisions in `docs/data/imports.json` (header origins, source copies, splices, then PATCHES,
  annotations, APPENDS, NEW_FILES, RECONSTRUCTED), and compares the result with the repository
  file by file. It now reproduces the git tree exactly: 7 libraries, 0 files differ.
* The first run found records that disagreed with the tree; the records were fixed, not the tree
  (no source file changed, so no compiled byte changed): `lightenvironment` kept a stale
  `headers_from: Z` after dfd1831 returned its header to Renegade's; seven header origins swapped
  by refine had never been written to the `headers` map; `predlod.h` (Z) and `streak.h` (G) were
  recorded under the wrong copy (they differ only in the banner line); `INI.H`'s spelling.
* The `headers` map is now the one authority for header origins; `trial.py` and refine's header
  swaps keep it and `headers_from` in step. Replay is part of the fix loop
  ([workflow.md](workflow.md) §2 step 5).
* The user answered the overnight report's questions: D6-D11 in §2 (start order auxdata ->
  wwudp -> task kernel -> packets; byte-exact everywhere; no hybrid client for now; move the
  texture system to Zero Hour's; unknowns keep neutral names; STL corpus grown on demand).
* Next session: D9, WW3D2's texture classes and `DX8Wrapper` from Zero Hour (§4 step 1). Start
  with `python tools/vc6/vc6.py discover` if the VMs restarted; `.work/bm2` holds the evidence.
  Keep `importer.py replay` at 0 differences before each commit.

### 2026-10-23 (overnight): layouts, vtables, symbol map, app survey

Unattended session, 01:20-04:40, plan agreed beforehand; report: [overnight-report.md](archive/overnight-report.md).

* Byte-exact client functions in the tree 3,121 -> 3,408 (WW3D2 1,862 -> 2,060, WWAudio 184 -> 264,
  WWLib 553 -> 557, wwshade 8 -> 13). Every fix with its evidence: [library-matching.md](library-matching.md)
  §3.6.1. One loss accepted and documented (`meshmatdesc.cpp`, 2 functions, an inlining flip).
* New tools: `tools/re/vtables.py` (client vtables aligned slot by slot; note VC6 orders overloaded
  virtuals in reverse declaration order), `symbols.py` (docs/data/symbols.json, a Ghidra script,
  [coverage.md](coverage.md): 6,100 symbols), `classify_gaps.py`, `app_inventory.py`
  ([app-inventory.md](app-inventory.md)), `tools/vc6/trial.py`, `trial_all.py`, `layout.py`,
  `sdk_patch.py`; three build VMs used in parallel, addressed by IP.
* `importer.py refine` fixed: whole-file and header swaps had been accepted on a net gain; now no
  previously matched function may be lost.
* E&B's own code surveyed; proposal for where to start: [app-plan.md](app-plan.md) (decisions D6-D8).
* Found: E&B's `DX8Wrapper` and texture classes follow Zero Hour's (vertex/pixel shader state, Z
  textures): Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks. Moving the
  texture system to Zero Hour's is the largest remaining WW3D2 step and unlocks wwshade.
* Found: `importer.py assemble` restarts from the base copies (it is the pipeline's first stage),
  so the tree cannot yet be rebuilt from the EA sources and the recorded decisions. The tree in
  git is the authoritative result until a replay step exists.

### 2026-10-23: gaps (step 4), assembly comments

* Gap locator (`tools/re/gaps.py`) and layout-difference finder (`tools/re/layoutdiff.py`,
  `tools/vc6/layout.py`). Three header fixes found from member offsets
  ([library-matching.md](library-matching.md) §3.6.1): WW3D2 in tree 1,354 -> 1,681.
* Decided (user): inline assembly stays wherever the client has it (an earlier "no assembly" rule
  was withdrawn), and every assembly block gets a comment with its C equivalent
  ([conventions.md](conventions.md), "Inline assembly"). 37 comments inserted into imported files
  by `tools/vc6/annotations.py` (re-applied on import; verified: no bytes changed), and the
  RC4Class blocks commented by hand.

### 2026-10-22: import (step 2) and RC4Class (step 3)

* Libraries imported into the repository by `tools/vc6/importer.py` ([imports.md](imports.md)):
  file by file from the best-matching EA copy, whole-file swaps and single-function splices from
  the other copies where they add byte-exact functions and lose none (checked tree-wide when a
  header changes). The repository now reproduces more client functions than any single EA copy:
  WW3D2 1,354, WWLib 553, WWMath 367, WWAudio 182, WWSaveLoad 80, WWDebug 67; wwshade only 6 of
  46 (needs Zero Hour's WW3D2 interfaces).
* `RC4Class` reconstructed from the client, all five functions byte-exact; the importer keeps
  reconstructed files (`RECONSTRUCTED`).
* Out of scope, proven: the VC6 SP4 C runtime, C++ library and COM support, D3DX 8.0; linker
  padding and compiler EH stubs. E&B's own code was compiled with `/GX`
  ([library-matching.md](library-matching.md) §3.7).

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
| D2 | 2026-10-22 | Install Visual C++ 6.0 as a tool for proving our work (byte-for-byte comparison) | user. Installed: VC6 SP4 in Hyper-V VMs (three, 2026-10-23; since 2026-10-24 Win10 plus the pool, D30), the client's exact compiler build ([toolchain.md](toolchain.md) §1) |
| D3 | 2026-10-22 | The code builds with both VC6 and modern MSVC | user |
| D4 | 2026-10-22 | Import EA code file by file, as each file is verified against the client. Superseded in part by D211 (2026-11-07): the EA-derived libraries are frozen as plain source, edited in place; importer.py's add / assemble are retired | user |
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools (the copies this project used; it does not distribute them, D150; [toolchain.md](toolchain.md) §2 lists their SHA-256 hashes) | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |
| D6 | 2026-10-23 | E&B's own code starts with `app\shared\auxdata`, then `app\wwudp`, then the task kernel (`app\kernel`: `KernelClass::Run_One_Cycle`, the loop running the current, background and pending-delete tasks; with `app\main` and `app\client\tasks`), then `app\packets` ([app-plan.md](app-plan.md) §3) | user |
| D7 | 2026-10-23 | E&B's own code is held to byte-exactness everywhere, like the libraries. Check in with the user periodically if this becomes too costly. Trigger: revisit with the user when docs/data/not-exact.md passes 400 entries (269 on 2026-10-27) (amended 2026-10-27, D110). Revisited 2026-11-10 at 742 entries (D224): byte-exact stays the goal, the work continues toward it | user |
| D8 | 2026-10-23 | No hybrid client (reconstructed functions patched into the retail one) for now: its results would need the user to validate them. Open to it again if it can be shown to verify results better than byte matching, without the user | user |
| D9 | 2026-10-23 | E&B follows Zero Hour's KM/KJM (shader system) line through 08/01/02, without the 08/05/02+ texture redesign and without the Generals team's MW/GTH changes. So `TextureFilterClass` (08/05/02 KM) is dropped: `TextureClass` has Renegade's five filter members (texturefilter.cpp/.h `NOT_LINKED`) | user; overnight finding: Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks, and it unlocks wwshade. Reworded by the user 2026-10-23 after step 1 showed the client has the 06/27/02 abstraction but not the 08/05/02 redesign (library-matching.md §3.6.1), and again 2026-10-24 after the dated-comment sweep (`.work/night/item6i/report.md`: bytes place every dated ZH edit; MW 0/3, GTH 10/12 absent); filter members folded in by the user 2026-10-24 (all four constructors exact, 0b8f6ce) |
| D10 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D11 | 2026-10-23 | Compiler-generated code (STL template instances): extend the corpus as each app file needs it, not ahead of the app work | user |
| D12 | 2026-10-23 | Compile each library with the flags E&B used, wherever byte evidence shows them (wwshade: `/GX /Ob1`; importer `LIB_FLAGS`, `bytematch_all.sh` `LIBFLAGS`) | user: "match how E&B built things whenever you have evidence to support it" |
| D13 | 2026-10-23 | EA files the client does not link are dropped from the tree (importer `NOT_LINKED`, with the evidence): first `shdlegacyw3d.cpp`/`.h`; then WWAudio's `AudioSaveLoad.cpp`/`.h` (2026-10-23) | user |
| D14 | 2026-10-23 | `ENABLE_WWPROFILE` in WWDebug too, although its bytes are neutral (verify: 67 before and after, none lost): consistent with a project-wide define. Soft: drop it if it ever costs a match. Also (D26, merged 2026-10-27): `USE_WWSHADE`: WW3D2 built with it (under D12, evidence: the `SHD_*` wrapper calls); wwshade too, as a soft consistency setting like the above (neutral in the bytes); done e278d81, d2955f0. Also (D74, merged 2026-10-27): `ENABLE_WWPROFILE` goes into the app library's defines, kept if a verify changes no pair (evidence since: WWAudio +32, WWMath +1, WW3D2 +20 pairs) | user |
| D15 | 2026-10-23 | `G_CODE_BASE` in WWAudio stays an importer patch removing its two `#ifdef`s, not a `/D` in `LIB_FLAGS` (same bytes). Superseded by D211 (2026-11-07): PATCHES are retired; the change is a direct edit of WWAudio.cpp recorded in its end-of-file `// EnB:` block (still no `/D`) | user: only `WWAudio.cpp` uses it, so the bytes can't show a project-wide define |
| D16 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D17 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D19 | 2026-10-23 | E&B's screen-distortion module in WW3D2 (0x995D20-0x998200, between dx8texman.cpp and boxrobj.cpp; first described as a cube-map list) is rebuilt as `ww3d2/unknown_distortion.cpp`/`.h` (a reconstructed file), neutral names (D10) until E&B's own turn up; the function `WW3D::Set_Render_Device` calls is `Unknown_Reset_Distortion_List` | user (name chosen after the module turned out not to be about cube maps) |
| D18 | 2026-10-23 | Don't include `matrix4.h` first in `ww3d2/meshmatdesc.cpp` (it would win back `Make_UV_Array_Unique` 0x947310, but no EA copy has that order). Keep it on record until a better fix is found; revisit if it becomes WW3D2's last outstanding item | user; kept 2026-10-24 after item 4 showed the cause is symbol-count noise, not an inlining flip; confirmed 2026-10-25 (evening, builder C): the difference is only which of `pass` and `stage` is loaded first, operand-order tie-breaking, not inlining; closed 2026-10-27 (D110): the noise is understood (operand-order tie-breaking, 2026-10-25); kept as is, no revisit |
| D20 | 2026-10-24 | Losing a COMDAT-duplicate `(file, address)` pair, where the helper stays matched in another file, is not a loss. Name each such pair in the commit message | user |
| D21 | 2026-10-24 | `dx8webbrowser.cpp`'s type library: `BrowserEngine.tlh`/`.tli` generated by VC6 from E&B's own `BrowserEngine.dll` are committed as reconstructed files, `#include` instead of `#import` (original kept as a comment); no binaries in the repository (option A) | user; done b8d3dd7 |
| D22 | 2026-10-24 | Import Zero Hour's shd6/shd7/shd8 bump shaders into wwshade, with the shader headers rebuilt from the client's data (0xBCB8FC-0xBCEBB7) as reconstructed files, in `shdpp.exe`'s format: `DWORD` arrays holding the shader text, NUL, zero padding, 0 terminator (PS; `DWORD` approved by the user 2026-10-24, since `ShdHW*Shader::Create(DWORD*)` takes them and `char[]` would pick `Create(char *file_name)`) | user; done 2026-10-24 (4c0fbf6, 523467f: headers as shdpp's DWORD arrays, since `Create(DWORD*)` takes them) |
| D23 | 2026-10-24 | E&B's own files enter the importer as a non-EA library `app` (all reconstructed, `/GX`, `NDEBUG` on since D61 (first written as off), verify limited to the app region) | user; done 86757d4 (importer `APP_LIBS`; sources compiled as `e:\build\beta\G\...` for the client's `__FILE__`) |
| D24 | 2026-10-24 | App classes the client doesn't name get Westwood `XxxClass` names (`BitSetClass`) | user; conventions.md §1 |
| D25 | 2026-10-24 | App asserts sit on their real `__LINE__` through the file layout (blank lines, comments) in tree files; `#line` only in drafts | user |
| D26 | 2026-10-24 | merged into D14 (2026-10-27) | user; done e278d81, d2955f0 |
| D27 | 2026-10-24 | Verified function starts (decoded call targets) are the default for every tool | user; done 5d7079f |
| D28 | 2026-10-24 | Keep ww3d.h's unused Renegade `MinStaticSortLevel`/`MaxStaticSortLevel` declarations (noise compensation for meshmatdesc after 694a02b, not evidence); revisit when the noise is understood | user; kept 2026-10-25 (evening): the noise is operand-order tie-breaking (builder C), D18 likewise; closed 2026-10-27 (D110): the noise is understood (operand-order tie-breaking, 2026-10-25); kept as is, no revisit |
| D29 | 2026-10-24 | Keep 8f5cbfe's protected `D3DTexture` (needed by 5534f64, which writes it as 0x94B660 does); patch reasons reworded. Its comment (D35, merged 2026-10-27) cites 0x94B660 (5534f64), not `Apply` 0x94BC80 | user; done a1cbc31 |
| D30 | 2026-10-24 | Build VMs come from a pool of differencing clones of a base VHDX (`vc6_config`), no warm sync cache (each leasee syncs its tree), cap set by host memory (`pool.py create --max`, default 7; one VM per 2 GB free above a 3 GB floor); the old fixed clones are retired (amended 2026-10-27, D110) | user; done b156da0, 4b1f45b; clones retired 2026-10-24 ([toolchain.md](toolchain.md) §1.3) |
| D31 | 2026-10-24 | Sessions release their leased VMs in the handoff skill; `pool.py lease`/`create` reclaim orphaned leases first | user; done 92ca150 |
| D32 | 2026-10-24 | ~~Builders stay serial (one at a time); parallel worktrees not needed yet~~ Superseded by D62 (2026-10-25) (retired 2026-10-27) | user |
| D33 | 2026-10-24 | merged into D56 (2026-10-27) | user |
| D34 | 2026-10-24 | Keep evidence-backed changes that gain nothing countable when the client's form is shown: first `DazzlePrototypeClass`'s Renegade virtual destructor (0x96EF00 has the client's form but is too short to count; vtable 0xB12598 has 4 slots) | user |
| D35 | 2026-10-24 | merged into D29 (2026-10-27) | user |
| D36 | 2026-10-24 | VIDTEST.EXE is rebuilt as a working program, not byte-exact: proof that the tree links into a small app that loads E&B's original art and renders a 3D scene (the 2002 library variant and incremental link aren't needed for that). E&BConfig.exe isn't rebuilt (reworded 2026-10-25 with D63: the original only launches our vidtest) | user |
| D37 | 2026-10-24 | App class names from the auxdata work: `DeltaClass`, `DeltaCompoundClass`, `DeltaTemplateClass`, `NameClass`, `LookupClass`, `LinearBufferClass`, `MoneyClass`, `DurationClass`; `PermString` keeps the client's own name (its debug message at 0x4ADFF0), without the suffix; the header of the assert functions is `app/shared/unknown_debug.h` (`Unknown_Assert_Failed`, `Unknown_Fail_If_Report`, D10). The assert header and functions superseded by D198 (2026-11-06): `app/shared/g_assert.h`, `G_Assert_Report`, `Fail_If_Report` | user (accepted builders' proposals) |
| D38 | 2026-10-24 | Names chosen from behaviour stand: `Is_Parent_Of`, `Is_Parent_Or_Same`, `Match_Parts` (name.cpp), `Unknown_Set_Size` (linearbu.cpp), `StringValueClass`, `Add_String` (delta_string.cpp, delta.h), `PermStringInitClass` (permstr.h), `Unknown_Debug_Printf` 0x509DC0 (dbgmgr.cpp) | user |
| D39 | 2026-10-24 | The time and money field instances `DeltaTemplateClass<DurationClass>` / `<MoneyClass>` are explicit specialisations (`template<>` constructor, Read, Write) in delta_time.cpp / delta_money.cpp, declared `extern template` where the roots use them (aux_roots.h for money). Reworded 2026-10-25 to the form the tree has: the first wording (`template class ...` instantiations next to the header's `extern template`) doesn't compile in VC6 (C2949, C2909) | user |
| D40 | 2026-10-24 | `DeltaCompoundClass::Add` 0x4A5670 (D11): check the client's other `vector<T*>::insert` copies for the out-of-line `size()` 0x4A58C0 before changing the STL corpus | user |
| D41 | 2026-10-24 | Header `__FILE__` strings in app code: leave them for now (ours include `..\duration.h`; the string differs, bytes and lines match through the relocation); run app compiles in `e:\build\beta\G\app\<dir>` with `/I ..` and the client's include spellings when strings are compared (whole-image check) or the project directory's name is known | user |
| D42 | 2026-10-24 | Milestones M1 (capture harness on the rebuilt auxdata and wwudp classes) and M2 (working VIDTEST on the tree's libraries) drive the work met 2026-10-25 (D63); the NOT EXACT rule continues as D64 ([milestones.md](milestones.md)). Functional code on their path that isn't byte-exact yet is allowed, marked `// NOT EXACT:` (P1) (amended 2026-10-27, D110) | user |
| D43 | 2026-10-24 | The 11 AuxData root constructors (first written as 13) are generated from the extracted schemas in the client's code form, then verified like any app file (P2) | user |
| D44 | 2026-10-24 | Missing functions at link time are stubbed in `stubs/` only, never in library files; each stub logs its first call (P3) | user |
| D45 | 2026-10-24 | New programs: `tests/capharness/`, `tools/harness/` (Python side), `programs/vidtest/` for VIDTEST's own code (WinMain, tests, `results.ini`: no source path survives in VIDTEST.EXE). VIDTEST's E&B scene classes follow the client's layout (`app/shared/stars.cpp`, `spline.cpp`, ...): they share the client's strings, including `\\.\mailslot\G\...` (P4). `tests/capharness/` superseded by D155 (2026-10-30): each test program has its own folder under `tests/`, capharness.cpp retired | user |
| D46 | 2026-10-24 | VIDTEST's E&B classes (Land, stars, decorations, splines) are rebuilt from the client's 2004 versions where the client has them, byte-exact there; VIDTEST's 2002 code only where the client lacks it (P5) | user |
| D47 | 2026-10-24 | M1's transport gate uses the EA captures; the Net-7 TCP recordings are tested at message level unless their transport is in `app\wwudp` (P6) | user |
| D48 | 2026-10-24 | Build on pool VMs; run vidtest on the host (the VMs have no 3D hardware). Running the tree's own programs unattended is allowed, as E&B's own code is (P7) | user |
| D49 | 2026-10-24 | L2: the 12 EA files never imported (wwlib TARGA.CPP, int.cpp, blwstraw.cpp, blowfish.cpp, stimer.cpp, slnode.cpp, win.cpp; wwsaveload definitionfactory.cpp; wwaudio soundstreamhandle.cpp, Utils.cpp; wwshade shddeffactory.cpp, shdforcelinks.cpp) are imported as they are, each checked with a verify ([milestones.md](milestones.md) §3.1) | user |
| D50 | 2026-10-24 | L2's link line adds vfw32, comsupp and ddraw | user |
| D51 | 2026-10-24 | Miles: `mss32.lib` is generated from the game's own `Mss32.dll` exports by `tools/vc6/mkmss32.py`: a generated `.def` linked into a dummy DLL, keeping LINK's import library (VC6's `lib /def` drops the stdcall names' leading underscore; reworded 2026-10-24, user), not stubbed and not the SDK's older lib | user |
| D52 | 2026-10-24 | M1's AUX_DATA totals for the Alex sessions come from `tools/harness/convert.py`'s datagram files (4,289 / 366), the 4 messages the reference decoder lost reported as "not in the reference decoder" | user |
| D53 | 2026-10-24 | VIDTEST O7: our vidtest fixes the signed `FreeTextureRam` comparison (GPUs reporting 2 GB or more), rather than copying the original's `TextureReduction=2` | user |
| D54 | 2026-10-24 | A function whose compiled bytes occur at several places in client.exe, all identical copies (objmatch `multiple`, e.g. template leaf Reads), counts as "exact, ambiguous address" in a separate column beside each library's pair count; the `(file, address)` pair counts and the before/after pair rule are unchanged | user |
| D55 | 2026-10-24 | `ww3d2/pot.cpp` is NOT_LINKED; wwmath's identical `pot.cpp` supplies `Find_POT` / `Find_POT_Log2` (both copies matched the same two client functions, and a program linking both fails on the duplicate). Its 2 ww3d2 pairs are dropped as D20 duplicates | user |
| D56 | 2026-10-24 | Name certainty: every name chosen without the client's own evidence carries a tag comment where it is declared, `// NAME n: evidence`, scored 5 = the client names it (RTTI, assert, `__FILE__`, `Class::Method` string), 4 = EA's Westwood source names it, or a Net-7 name or the reference game model's, confirmed by matching structure, 3 = built by convention from a client-named file or sibling (D24 class-from-file, a `.h` paired with a known `.cpp`), 2 = describes what the code does, 1 = neutral placeholder (`Unknown_xx`, D10). 4-5 are settled; 3 and below are listed for review by `tools/re/names.py` in the generated `docs/data/names-review.md` (untagged `Unknown*` names count as 1). When better evidence appears the tag is raised and the name leaves the list ([conventions.md](conventions.md) §1). Merged 2026-10-27, with D65 (neutral names are NAME 1): D10 (E&B's unknown additions keep neutral names, `m_UnknownXX`, evidence in a comment, until the code that uses them is rebuilt); D16 (members whose use is now rebuilt, `m_Unknown148`, `m_Unknown164/168/16C`, `m_Unknown19A5/19A6`, keep their names with the comments in the headers until the app code shows E&B's names); D17 (the two wwshade `"UnNamed"` constructors stay near-misses; revisit only on new evidence); D33 (kept: `MEM_CLIENTOBJECT`..`MEM_PARTICLES` from the client's name table 0xBCB228, `FastAllocatorGeneral::m_UnknownE00`..`m_UnknownE20`, `Unknown_Reset_Statistics`); D57 (`encode.h` NAME 2 and `strconv_lib.h` NAME 3 stay; `KVPairClass`'s virtuals keep descriptive names `Clone`, `Get_Type_Name`, `Get_Value_String`, NAME 2; the file for the compound/root functions 0x4A3990-0x4A4780 and 0x4B0860 waits for M1.3, using Net-7's `AuxClasses` and the reference game model's names as structure-matched hints); D58 (provisional file names stay: `app/shared/auxdata/unknown_root.*`, the compound/root object with no `__FILE__`; `app/client/aux_roots.*`, the generated roots, the client's code being in co.cpp and an unattributed object at 0x751BE0, the generator writing into co.cpp once that is rebuilt; `app/shared/heightfield.*`, `app/shared/land.*`); D86 (step 8's `unknown_memregistry.cpp`, `unknown_listener.cpp`, `unknown_rate.cpp` and `RSAKeyClass`'s `Unknown200..Unknown800` members, NAME 2, stay until a string or `__FILE__` gives the real one, P2) | user |
| D57 | 2026-10-24 | merged into D56 (2026-10-27) | user |
| D58 | 2026-10-24 | merged into D56 (2026-10-27) | user |
| D59 | 2026-10-24 | AuxData class names: nested compounds `Delta<Net-7 name without Aux>Class` (NAME 4, matched child by child against Net-7's `AuxClasses` and the reference game model, `tools/re/auxroots_names.json`), roots `Delta<client string>Class` (NAME 3); `DeltaInterpolatorClass` keeps its name (Net-7's AuxPercent / C# Ramp; the client interpolates) | user |
| D60 | 2026-10-24 | M1 gate (b) is met with one documented exception: capture_1 #24128 (a Hulk, header byte 0x55 under a parent marking that child cleared) decodes but can't be re-encoded by the client's encoder; the harness doesn't keep header bytes as read | user |
| D61 | 2026-10-24 | The app library compiles with NDEBUG (in APP_LIBS' defines; G_ASSERT / FAIL_IF don't depend on it), confirmed by a verify that changes no pair; files stop defining it themselves | user |
| D62 | 2026-10-25 | Builders may run in parallel on disjoint items (replaces D32), under the shared-tree rule: never compile the other builder's changes into a verify (check `git diff` of shared files: importer.py, app headers, stubs, docs/data), commit only your own hunks, keep the uncommitted window short, never wait for a clean tree (that deadlocked both builders for an hour on 2026-10-24). Each builder uses a private scratch folder (TEMP/TMP under .work/<agent>/tmp; automatic once the importer does it) or its own worktree. The importer lock serialises `verify` ([builders.md](builders.md)) (amended 2026-10-27, D110) | user (approved for the 2026-10-24 unsupervised run; recorded 2026-10-25) |
| D63 | 2026-10-25 | M2 gate (c)'s repeatable check is `tools/harness/vidtest_e2e.py` (E&BConfig's command line, results.ini). A supervised run of the original E&BConfig with the user, registry backed up first, is a one-time acceptance check, not a gate. D36 reworded: E&BConfig.exe isn't rebuilt. **Passed 2026-10-25** (log entry of that evening): every displayed value as our results.ini, registry unchanged | user |
| D64 | 2026-10-25 | `// NOT EXACT:` is a general interim marker for app code written functionally from pseudocode, not only on the M1/M2 path (D42); D7 stays the target. `tools/re/names.py` lists every marked function in the generated `docs/data/not-exact.md` | user |
| D65 | 2026-10-25 | D10/D16's neutral names are NAME 1 on D56's scale, replaced whenever better evidence appears (conventions.md §1 reworded) | user |
| D66 | 2026-10-25 | Pair losses. Not losses, named in the commit message: (i) a COMDAT duplicate kept matched in another file (D20); (ii) the same source compiled into two files (D55); (iii) a false match exposed by a type fix (first: 0x8C1520, `KVPairTemplateClass<__int64>`'s string setter, matched by the money instance only while `MoneyClass` was a bare `__int64`). A change that gains and loses nothing but models the client's own classes (07af3e1: Ship's BuffArray and Buff elements with the client's vtables and globals) is allowed, stated in the commit (2026-10-25, evening). (iv) a file rename whose pairs reappear under the new name, named in the commit (D95(c)); (v) a net-positive change where every lost pair's address is re-credited under another file in the same verify (pairs_cmp's address-level 'lost outright' list empty), named in the commit; an address lost outright still waits for the user; (vi) a pair that becomes ambiguous because the client's own form adds a byte-identical instance (D119). (vii) a caller tie-break pair (D166) lost with its caller's pair. Anything else is a loss and waits for the user before the commit. D54's ambiguous column stays as is: the 17 identical template leaf Reads are not pairs (amended 2026-10-27, D110; 2026-10-28, D119) | user |
| D67 | 2026-10-25 | Test programs live under `tests/` (D45 extended: kerneltest and movietest in tests/capharness). The wlib stand-ins (app/common/dictionary.h, critsec.h, app/wwudp/wtime.h, parts of cordcon.h) stay until D73's import replaces them with the imported files at the client's paths (`libs\wwonline\wlib`), then are deleted Done: the wlib stand-ins are deleted (D73's import is done), so the "stay until" clause is complete (2026-10-27) | user |
| D68 | 2026-10-25 | [kernel-map.md](kernel-map.md)'s 10-step reconstruction order is adopted (extends D6); steps 1-5 and 7 done or partly done on 2026-10-24 | user (backfilled) |
| D69 | 2026-10-25 | Safety rules for every run of our programs and every session (amends D48): never a modal dialog (`-nomsgbox` or its equivalent in every program); full screen allowed; no screen grabs, images only from the program's own back buffer; never the game's install folder as working directory; no registry changes (the original E&BConfig only in a supervised run with a registry backup); never push (amended by D148 / D157: push only a run branch, never master); never `importer.py assemble` on a tree with work (retired with assemble by D211, 2026-11-07); `git add` by name, `.claude/settings.json` stays out | user (each given on 2026-10-24; numbered 2026-10-25) |
| D70 | 2026-10-25 | Inline assembly stays wherever the client has it, with a C-equivalent comment | user (2026-10-23; backfilled) |
| D71 | 2026-10-25 | A commit's `Co-Authored-By` line names the model that wrote the commit (the line that session's harness gives); the docs carry no literal model name | user |
| D72 | 2026-10-25 | Subagent models: the coordinator runs Opus (D96; Fable until 2026-10-27); builders (verify loops, byte matching, RE, programs) run Opus by default; Fable for a subagent only when the expected total cost is lower: a small-input reasoning puzzle (inline budget, codegen form) after two failed Opus attempts, or a one-shot analysis whose wrong answer would cost a builder run; Sonnet for read-only audits, cross-referencing, doc moves, log digging and generator reruns (anything a tool checks afterwards; Explore agents too); Haiku for grep-level searches and log triage, never tree code. Plan logs record model and tokens per builder ([builders.md](builders.md) §4) (amended 2026-10-27, D110) | user |
| D73 | 2026-10-25 | Import WWOnline's wlib and wnet (Zero Hour's matchbot), Renegade's `Code\Tools\RenegadeGR` tcpmgr/tcpcon and Renegade's BinkMovie, under D4 (file by file, each verified). `CordArrayClass`'s remove is rebuilt on the imported wlib and its stand-in deleted (D67). Unblocks kernel-map steps 8-9 | user |
| D74 | 2026-10-25 | merged into D14 (2026-10-27) | user |
| D75 | 2026-10-25 | merged into D81 (2026-10-27) | user |
| D76 | 2026-10-25 | `CordMgr::Connect` keeps the client's signature; the harness reads the session id another way, unless the client's bytes return it | user |
| D77 | 2026-10-25 | funcmatch's string evidence requires the client function's `__FILE__` to match the EA function's file; symbols.json regenerated (0x829C80 is tcpconn.cpp's constructor by accepted socket, not `PacketManagerClass::Clear_Socket_Error`) | user |
| D78 | 2026-10-25 | The d3d8 draw dump (`tools/harness/run_reference.py`, `drawdump.py`) is M2's standard regression check, beside the frame comparison | user |
| D79 | 2026-10-25 | Net-7 difference 4 (fields named in two sets) is a one-line note in the earlier research's notes, outside this project. That research's own audit is outside this project's scope; for transport and auxdata this project's docs are now the more complete. The user revisits this before housekeeping closes | user. Settled 2026-10-25: the audit was done; retired 2026-10-27; the earlier research retired from this project (D153) |
| D80 | 2026-10-25 | `objmatch.py` breaks `multiple` ties by each data relocation's target (the string or known symbol it points at) against the object's own literals, so byte-identical functions that differ only in their strings count (the assert reporters, the twin constructors 0x4A7080/0x4A8150, `Set_Render_Device`, the one-int packet constructors and Prints: builders B and D). Adopted after a snapshot comparison of every library's pair set with no losses, the counts before and after recorded in library-matching.md §3.6.1. The objmatch order (merged 2026-10-27 from D88, D94(b), D95(b), D100): D80 rounds to a fixed point; then the self-name round (D88: the one candidate address that symbols.json names with the function's own mangled symbol from a non-exact proof, a vtable slot, is credited when D80 finds no contradiction there; measured by builder W3 at the objmatch level: +337 pairs, 0 lost, ww3d2 188, wwmath 60, wwaudio 45, wwshade 31, wwlib 10, wwdebug 2, wwsaveload 1; prototype `.work/w3/objmatch_sn.py`; adopted after a full verify snapshot of a quiet tree, with pairs_cmp proving the gain, not while builders are verifying; carried out 2026-10-27 04:50, 0d70c67 the pre-D88 snapshot, efc3cbb the rule: +368 pairs, 0 lost, ww3d2 188, wwmath 60, wwaudio 45, app 31, wwshade 31, wwlib 10, wwdebug 2, wwsaveload 1; `tiebreak`/`selfname` do not reach imports.json, left as is until something needs it); then D80 again; then the range round (D100: a tie goes to the object whose client range contains the address, P-P6-1, carried out on a quiet tree with a full verify of every library and pairs_cmp gains-only, like D88, then 45 from `.work/p6/park45` and 117 from `.work/p5/park117`; refinement d8b8153: an address another function is exact at is held out, and the range is the object's own non-COMDAT or NODUPLICATES exact functions); compiler-generated `_$E<n>` names are skipped (D94(b), P-H7-2, carried out the same way). D95(b) (the D80 tie-break not crediting an implicit destructor to an identical twin when the function's own address is free, P-P5-2) is not implemented: superseded by D100 and its refinement, close pending as P-M1-1 (closed by D114(a), 2026-10-27). Amended by D212 (2026-11-08): the range round also spans an object's located static initialisers | user |
| D81 | 2026-10-25 | D75 corrected: 0xBDA908 is the client's pooled `""` string, not a log object; dbgmgr.cpp calls the reporter 0x4BC130 as `("", "", file, ...)`. The `UnknownLogClass`/`UnknownLog` stand-ins in app/wwudp (cordmgr.cpp, bandwidthaccounting.cpp, udpreplay) become plain calls with `""` (follow-up after builder A leaves app/wwudp) (D75, merged here 2026-10-27: the assert functions' reporter 0x4BC130 (the logger) is a reconstructed stand-in with D10 names and D56 tags; its premise, a log object at 0xBDA908, is what this row corrects) | user |
| D82 | 2026-10-25 | `app/shared/unknown_debug.cpp` (the assert reporters 0x4E9410/0x4E9450) and `unknown_new.cpp` (E&B's operator new/new[]/delete over FastAllocatorGeneral) stay two files although one object in the client, so kerneltest and movietest can define their own reporters without LNK2005. `unknown_debug.cpp` renamed by D198 (2026-11-06): `app/shared/g_assert.cpp`; `unknown_new.cpp` stays | user |
| D83 | 2026-10-25 | Builder A's proposals accepted: BinkMovie lives at the top-level `binkmovie/` beside ww3d2 (the client has no path for it); wlib's wstypes.h takes sint8/sint32/uint32 from WWLib's bittype.h so one definition holds program-wide (same bytes, needed for linking); E&B's own tcpcon.cpp (0x8BBA70-0x8BCA50) and tcpqueue.cpp are rebuilt in wnet next, before connect.cpp's factories (RenegadeGR's tcpcon.cpp matches nothing: E&B's TCPCon is an OutputDevice with a lock writing through queues); build.py and `LINK_LIBS` learn the new libraries when a program first needs wlib | user |
| D84 | 2026-10-26 | wnet's `~TCPMgr()` is declared `throw()`: wnet must build with /GX (TCPMgr::wait, TCPCon and TCPQueue have exception frames) but E&B's destructor has none, and `throw()` is the only form found that makes VC6 omit it. Compiler evidence, not a known source line (P-B1); kept because it gains and loses nothing. Per-file compile flags (P-B2) not added until a library really mixes /GX and none. The P-B2 clause superseded by D186 (2026-11-02): per-file flags adopted (`FILE_FLAGS`, first wwlib rawfile.cpp) | user |
| D85 | 2026-10-26 | Open-BFME-1 (a byte-exact BFME rebuild under MSVC 7.1) is not a source and not a root: its ww3d2 is the DX9 port with BFME's layouts and reverse-engineered names, so nothing of it is copied (the assessment is summarised in the 2026-10-26 21:25 log entry). Its method is taken: the attempt log (`attempts.py`, read before any retry), the near-miss classifier (`explain.py`) with §3.1's triage order, the one-shot switch sweep (`flagsweep.py`, never looped) and the two-attempt stop rule for register-only and order-only residues (a third attempt is a Fable job under D72) | user |
| D86 | 2026-10-26 | merged into D56 (2026-10-27) | user |
| D87 | 2026-10-26 | SGI STL 3.3's `stl_hashtable.h` / `stl_hash_map.h` (the permissive SGI/HP notice, compatible with D1) are imported, adapted for VC6, as the client's hash_map: the string hash `__stl_hash_string` 0x42DB00 (h*5+c, 65 callers), `__stl_prime_list` once per translation unit (336 copies, the first 0xAE701C), the buckets a VC6 `std::vector` at +4. First users: dbgconn's counters 0x825390 / 0x8255B0 (keyed by std::string, bucket function 0x825B30) and kv_list's index 0x4B62B0 (replacing its std::map stand-in). Builder S3's P-S3-1. Carried out 2026-10-26/27 (H3, H4): the source lives with the other SDKs at `C:\EnBDevelopment\SDK\stl-3.3` (user), the VC6 headers in `libs/sgistl/` (generated by `tools/vc6/sgistl_adapt.py`, outside the importer's libraries); the client's out-of-line hashtable constructors take three arguments (no allocator), so the adapter drops it | user |
| D88 | 2026-10-26 | merged into D80 (2026-10-27) | user |
| D89 | 2026-10-26 | Fable near-miss limits, so Fable loops no more than Opus: (1) entry only after two recorded Opus rounds with a stashed best form and an explain.py class of inline-budget, frame or shape; register-only and order-only residues never enter (VC6 collapses them to a few canonical outputs: Make_Screen_Shot's 110 forms), they stay parked until a lever proven elsewhere names a cause; (2) one Fable round per function, ever, ending exact or with a written hypothesis in attempts.csv; a second needs a new §3.1 lever filed since, cited in the row; (3) at most 8 forms in that round, each from a stated hypothesis, batched through tryv2.py; (4) at most 4 functions per Fable session; at most 6 active queue entries by unlock value (a group taken as one round, as D97's, is one entry); further requests wait in §4 item 7's parked list; (5) one Fable builder at a time, under about 15% of a run's tokens; (6) `attempts.py show` prints LIMIT REACHED for such functions and `attempts.py check` refuses a retry of one unless `--lever` cites the new §3.1 row (amended 2026-10-27, D110). (4)'s cap of six queue entries lifted by D136 (2026-10-29): the queue is not pruned to fit; Fable rounds are scheduled more often instead | user |
| D90 | 2026-10-27 | Kernel-map step 9 continues in the order of `kernel-map-step9.md`: 9a kcom auth (an import library from authlogin.dll's .def; AuthLoginResponseClass), 9b the login data objects (the 0x90-byte character slot and TicketClass, avatar_desc.cpp's 0x10C-byte objects, shipcust_desc.cpp's 0x160-byte objects, the galaxy entry), 9c client.cpp's client base class, 9d SClient's skeleton (constructor, Init, Shutdown, Process_Packets: what cpacket.cpp's handlers need, tested with udpreplay); the UI front end (the login and character-select screens, intrface.cpp's 25 screens) is deferred | user |
| D91 | 2026-10-27 | udpreplay's `same()` skips a message's 2-byte size field only when the recorded message's size field disagrees with its length (alex7 session 625d message #89, a cut dump message with size 0x1E0A; the rebuilt Build writes the true size), so 59 Server_Ready (5 of 5 exact) can be committed (P-P4-1). Declined the same day: moving `Clamp_Int` into duration.h, +3/-2 at aux_roots through a D80 tie-break swap (P-P4-3): a swap is not a D66 non-loss. Accepted for after Load_Assets: one D89 Fable round on land's lighting-record struct form, the constructor with the lightmap threads 0x46DA70 / 0x46DDD0 together (P-L4-2). D88's `tiebreak`/`selfname` markers stay out of imports.json until something needs them | user |
| D92 | 2026-10-27 | One D89 Fable round for ClientClass's constructor 0x726100 (inline budget) and Find_Object 0x727070 / Remove_Object 0x727600 (shape; the untried `goto` to the found return), after the land lighting round F5 (P-T6-1). The clienttest program (P-T6-2) is deferred to step 9d and subsumed by P-T7-1 | user |
| D93 | 2026-10-27 | A model evaluation on two fixed reconstruction tasks (A: three small packet classes; B: option.cpp's SGI hash_map file), the same brief for Opus 5.5, Sonnet 5.5 (twice) and Haiku 5.5, each in its own worktree and branch at one base commit, scored by the coordinator's re-verify (exact functions, pairs gained with nothing lost, tokens and relative dollars per pair, process compliance, report honesty, levers found); the best branch merges afterwards; D72 revised from the scores. Fable is not a candidate; the Opus-coordinator trial is a later session. Design and the brief: [eval-d93.md](eval-d93.md). The next coordinator session runs it first, after the weekly reset of 12:59 (coordinator's P-C1) The coordinator was Opus 5.5, not Fable (user, 2026-10-27 14:50); results: the 2026-10-27 evening entry. | user |
| D94 | 2026-10-27 | (a) T5's drop of (tsk_login, 0x85CB00), the avatar's implicit assignment, when account_id became `__int64` is a D66(iii) non-loss (d23195d); the address returns with the packet file that carries the client's copy. (b) merged into D80 (2026-10-27). (c) The clienttest program (P-T7-1) is built once the first cpacket.cpp handlers exist, about one builder session | user |
| D95 | 2026-10-27 | (a) udpreplay sends the recorded bytes for a rebuilt class whose client Build writes a size field larger than its payload, as an explicit opcode list (97 AVATAR_DESCRIPTION, 183), so 97 can be committed from `.work/p5/park97` (P-P5-1). (b) merged into D80 (2026-10-27): not implemented, superseded (P-P5-2; 117 GLOBAL_ERROR was unparked under D100). (c) D66 gains kind (iv): a file rename whose pairs reappear under the new name, named in the commit, is not a loss (P-P5-3: pkunknown_85e640 / 85f870 / 860d30 to gameedit/pkgekv, pkgeobj, pkgeplan) | user |
| D96 | 2026-10-27 | D72 revised from D93's scores (P-E1): coordinators run Opus 5.5 instead of Fable; every other D72 row stands (builders Opus by default, Fable under D89, Sonnet for audits and tool-checked work, Haiku for searches, never tree code) | user |
| D97 | 2026-10-27 | The D89 Fable queue takes the evening's four requests: client_lego_impl's Find 0x72ABB0 + Add 0x72B7F0 + Remove 0x72C1D0 as one round on their common end-of-function inlining (P-H20-1) first, then CLIENT_DAMAGE 0x738970 (P-T19-3; FORMATION 0x734740 after it), then TimedLog 0x824540 (P-S1-1) and CLIENT_CHAT_EVENT 0x73B150 (P-T20-1) at low priority; D89's limits unchanged (one Fable builder at a time, 4 functions per session) | user |
| D98 | 2026-10-27 | Before a function is marked LIMIT REACHED on a register residue, a tryv2 sweep of loop forms in the inlined helpers it calls is part of its second round (P-H22-2; how 0x4B3850 fell after F3's rounds) | user |
| D99 | 2026-10-27 | Functions too short for the matcher but byte-identical to the client (objdiff 0 rows) are recorded as exact rows in attempts.csv (P-S1-2; 0x82E1D0); no matcher change | user |
| D100 | 2026-10-27 | merged into D80 (2026-10-27) | user |
| D101 | 2026-10-27 | VIDTEST on LandClass (P-L6-1 with P-L6-2 as its first step): one probe places the four undefined land symbols (~LandPatchClass 0x474560, ~LandPatchRecordClass, Unknown_477E20, LandUnknownClass::Peek_Material_Pass 0x93D6C0); then vidland's Create_Decorations / Render_Decorations move to programs/vidtest/viddeco.* (VIDTEST's own 2002 code, D46) as a render object after the land, the rest of vidland.* is deleted and `.work/l6/`'s switch diff applied; land test, drawcheck and sbs within +-1.5% | user |
| D102 | 2026-10-27 | One whole-app trial of SOptionsClass +0x1C as a float (`[Lights] GlobalZ` through INIClass::Put_Float, P-T18-3); kept only if soptions.cpp's 7 of 7 hold and pairs_cmp is gains-only | user |
| D103 | 2026-10-27 | udpreplay compares LOGIN 2's clock field (byte 0x44) zeroed, as for 52 and 68 (P-P9-1, D91's family; d63e864) | user |
| D104 | 2026-10-27 | LoginKVVisitorClass merges into DeltaChildVisitorClass / KVEncoderBaseClass (kv_encoder_base.h, delta.h; the canary 0x6FB1E0) together with prof.cpp / race.cpp (~30 functions 0x4F6A80-0x4F7820), as one item (P-P9-2, the rest of P-T5-2) | user |
| D105 | 2026-10-27 | ADVANCED 62 (0x734130) gets one more Opus round, the second recorded one D89(1) requires; 62 enters the Fable queue only after it, and then gets a D89 Fable round if still not exact (P-T16-3; SIMPLE 8 shares its residue) (amended 2026-10-27, D110) | user |
| D106 | 2026-10-27 | tsk_game.cpp keeps its one-line view of ServerRedirectPacketClass; its comment is refreshed at the file's next edit (P-A1) | user |
| D107 | 2026-10-27 | The four `Unknown_BE8B38Class` views (tsk_game.h, sclient.cpp, fontdatabase.cpp, soundlib.cpp) become option.h's OptionClass as an item of its own with its own verify (P-Bopus-1) | user |
| D108 | 2026-10-27 | option.cpp's Get_String 0x7F9700 joins the D89 Fable queue at low priority (P-Bopus-2) | user |
| D109 | 2026-10-27 | SClient's Init 0x741180 and Shutdown 0x741BB0 are held until the UI base classes they call exist (P-T7-2) | user |
| D110 | 2026-10-27 | The decisions audit's changes: D89(4), D105, D72 amended (conflicts); D66 (v), D30, D62, D42, D18, D28, D7 amended (holding back); D67, D75, D79, D32 and the D14 / D29 / D56 / D80 families merged (housekeeping) | user |
| D111 | 2026-10-27 | E&B's ref_ptr.h: the three EA copies are compared for the lines E&B's version lacks (P-F6-2, after F6 found G_ASSERT(dummy == 0) absent: 4f2a8b1); then client_lego_impl's Remove 0x72C1D0 gets 1-2 more forms (the named lego_id reference, Find's exact form) as a D89(2) retry citing that lever (P-F6-1); Add 0x72B7F0 waits for P-F6-2 | user |
| D112 | 2026-10-27 | Tools from the traps audit: (1) the importer and tryv2 put scratch under .work/<agent>/tmp when an agent id is set, `--agent` defaults from the worktree name; (2) commit_mine commits a file's `headers` rows, splits attempts.csv and the generated .md files by row, gains `--refresh-totals`, and commits by compare-and-swap (`update-ref` with the old value); (4) tools that write the tree refuse when the working directory isn't their root; (5) `appfile.py match -I auto`, its own lease, objects outside the draft folder; (6) names.py scans untracked files and refreshes not-exact.md. Tool (3), a worktree helper: see the coordinator's comparison with worktrunk | user |
| D113 | 2026-10-27 | Worktrees get a tool of our own, `tools/vc6/wt.py` (new / bring-over / remove: junctions, captures, libraries, app.lib; bring-over refuses unless master equals the base, merges the row files by row, re-verifies and lands through commit_mine's compare-and-swap), not worktrunk: its setup would still be our hook script, its copy-on-write sharing needs APFS / btrfs / XFS (this host is NTFS), and its squash merge with generated messages conflicts with the one-commit-per-verified-gain history (D112(3)) | user |
| D114 | 2026-10-27 | (a) D95(b) is closed, superseded by D100 and its refinement (P-M1-1); (b) P-M1-2's wider range (the object's exact COMDATs only it is exact at, within 0x1000 of either end) and (c) P-M1-3's regeneration of symbols.json each get a full verify of every library on a quiet tree, like D88, and are kept only gains-only. (b) amended by D183 (2026-11-01: a COMDAT several objects hold goes to the strictly nearest holder) and D196 (2026-11-05: the widening is chained) | user |
| D115 | 2026-10-27 | objmatch credits a tied constructor / destructor of a file-static to the address its rebuilt static initialiser registers or calls (the `_$E` initialiser's own relocation names it: co_player_relationship's 0x709EF0 registers 0x709F40's atexit), as D88's self name does from a vtable slot; finds and other tied helpers stay tied. On a quiet tree after D114, with a full verify of every library, kept only gains-only (P-R2-1 narrowed). Amended by D116 (2026-10-27): the round credits by symbol across objects | user |
| D116 | 2026-10-27 | D115's initialiser round credits by symbol across objects (one mangled symbol is one address in the linked image), as builder M3 built it (c422c85: permstr 0x4AE2D0, definitionfactory 0xA06000 / 0xA06020); D115 reads accordingly (P-M3-1) | user |
| D117 | 2026-10-27 | pairs_cmp prints the addresses lost outright (lost pairs whose address no file holds afterwards) and the moves, the list D66(v) depends on (P-M3-2) | user |
| D118 | 2026-10-27 | Evidence-backed pins (Open-BFME's ledger) are measured and not adopted: the matching stays objmatch's rounds in D80's order. Builder M4's measurement (wt/pins 8db8c42): a caller in the same object +655 pairs, 0 lost; a caller in the library +812; placement +321 / +336 with about 2.4 % wrong where checkable; six hand pins +8; 82-88 % of every kind's credits land on byte-identical twins. The prototype and its numbers are kept in tools/re/experimental/pins/ (README) for a later decision | user |
| D119 | 2026-10-28 | P-LD2-1 accepted: a pair that moves to D54's ambiguous column because the change writes the client's own form, which has several byte-identical instances (land.obj's `Create_*` RefCountPtr setters; 0x474080 when Build_Decorations 0x47A2F0 uses them), is not a loss under D66 (vi). Named in the commit with the address and the identical instances; an address lost outright still waits for the user. Work item: land.cpp's Land_Set_Ref helper goes back to the client's `Create_*` calls | user (LD2's proposal) |
| D120 | 2026-10-28 | P-LD1-2 accepted (a D69 safety default): drawcheck, vidtest_sbs and run_vidtest default to `--art build/art` whenever that folder exists, so VIDTEST's Save_PLD never writes into the install folder | user (LD1's proposal) |
| D121 | 2026-10-28 | P-CR1-1 accepted: co.h's `UnknownA8` (+0xA8) is `BaseAsset` (CREATE 4 and CHANGE_BASE_ASSET 38 store the packet's BaseAsset there; 0x6F39F0 reads it as BASE-n) | user (CR1's proposal) |
| D122 | 2026-10-28 | P-CR1-3 accepted: the object subclasses' run 0x70DC00-0x70F1D0 (constructors 0x70DBC0 / 0x70E2E0 type 24, 0x70E5A0 type 28; no `__FILE__`) gets the provisional file `app/client/co_subtypes.cpp` (NAME 1), renamed if an assert or string names it | user (CR1's proposal) |
| D123 | 2026-10-28 | P-PF1-1 accepted: `ClientClass::DebugObjectID` (+0x112C) is the player's avatar id, `AvatarID` / `Get_Avatar` (CLIENT_AVATAR 55 sets it; ADVANCED 62, FORMATION 65, 0x6F3C60, DAMAGE 100 use it) | user (PF1's proposal) |
| D124 | 2026-10-28 | P-PX2-1 accepted: the string-less object 0x451C00-0x452770 (`PxBoxCallback`) keeps the neutral file name `app/shared/unknown_px_callbacks.cpp` (D10) until a string names it | user (PX2's proposal) |
| D125 | 2026-10-28 | P-PX2-2 accepted: PxObject's functions 0x4557F0-0x455860 stay in px_collision.cpp until evidence places them | user (PX2's proposal) |
| D126 | 2026-10-29 | The history rewrite (every commit re-authored, hashes changed; old master ad6562e -> ad6562e): tracked files' old commit hashes are rewritten through the old -> new map, same length so no line moves (`.work/coord/rehash.py`; generated files regenerated, not edited), and the map is kept as `docs/commit-map.txt` for the old hashes in commit messages; done at the 2026-10-29 handoff with a full verify, pairs 0 / 0 | user |
| D127 | 2026-10-29 | D101's sbs criterion is met: since LD2's sun fix the means are 2.03-2.09 % against vidland's 1.9-2.4 %, frames 600-900 1.4-2.0 % ("within ±1.5 %" read as points of difference) | user (LD1's proposal) |
| D128 | 2026-10-29 | P-SC3-1: 0x750C20 (testeffects.ini's player, 0.54, register / stack slots, no caller) stays parked; no exception to D89's rule for register-only residues | user (SC3's proposal) |
| D129 | 2026-10-29 | The ??_E / ??_G matching rule (8370176; bada317 before the D126 rewrite) is kept as an extension of D88's self-name round: a vtable slot 0's `??_E` name in symbols.json counts for the object's `??_G` and the reverse (symbols.py names slot 0 `??_E`; VC6 emits `??_G` for a class never new[]'d); bytes still match with relocations masked, so a vector deleting destructor or a slot-0 thunk cannot be credited. +293 pairs (276 addresses), 0 lost. D118's numbers predate it (M4's README notes the alias unmeasured) | user (PK1's P-PK1-1, coordinator) |
| D130 | 2026-10-29 | P-IB1-1 with P-DM1-1: dead code that spends VC6's inline budget without emitting code (a root-NULL find, an unused type test, an unused accessor read) may stand in for budget the client's source spent invisibly. Every kind counts toward one limit of 6 per function; each is marked in the source as not the client's; each must be needed (removing any one loses a pair, checked at commit). A function that needs more stays NOT EXACT and parked, no exception: 0x6FBEF0 needs 20 and goes to the Fable round on the per-find cost (§4 item 7). When a real source form for the missing budget is found, it replaces the dead spenders it explains | user (IB1's and DM1's proposals) |
| D131 | 2026-10-29 | P-CS1-1 accepted: the setter at slot +0x24, `Set_Debug_Object`, becomes `Set_Avatar_ID` like D123's getter (it sends the DEBUG packet; expected 0 / 0 pairs); a work item in §4 item 2 | user (CS1's proposal) |
| D132 | 2026-10-29 | P-CS1-3: split co_subtypes.cpp now: type 24's base (0x70DBC0-0x70E2D0: constructor, destructor, update, thunks) goes to a file of its own with a neutral name (D10), types 24 / 28 (0x70E2E0-0x70F030) stay; the `#pragma auto_inline(off)` around the base, which stood in for the file boundary, goes; a D66 (v) move, 0 lost; a work item in §4 item 2 | user (CS1's evidence: the client never inlines the base into type 24) |
| D133 | 2026-10-29 | P-HX1-1: auxroots.py's `SYMBOL_PAD` enum stays in the generated aux_roots.cpp for now (marked as not the client's; it carries 84eacca and 8102bcf, +16 pairs); revisit when client code (a real declaration the file sees) can take its place | user (HX1's proposal) |
| D134 | 2026-10-29 | P-PK3-1, widened: the verb object's five inline-budget functions (VerbList ctor 0x8AF150, Print 0x8AF6D0, VerbUpdate ctor 0x8AFD10, Build 0x8AFF40, Print 0x8B0150; one cause on the same two sets) go to the Fable queue (§4 item 7) as one round; the object's other functions are exact or are the five's out-of-line helpers | user |
| D135 | 2026-10-29 | Two D89 Fable rounds run at once on 2026-10-29, outside the day's usage budget: co.cpp's per-find budget round (0x6FD310, 0x6FBEF0, the roll 0x6F3FA0) and D134's verb round with all five functions in one session (an exception to D89's 4 functions per session and D97's one Fable builder at a time); their measured cost then raises the rest of the day's plan | user |
| D136 | 2026-10-29 | The Fable queue (§4 item 7) is not pruned or merged to fit D89's six: Fable rounds are scheduled more often instead, so the queue stays short (the coordinator plans at least one round per run while the queue holds eligible items) | user |
| D137 | 2026-10-29 | Interim placement is the coordinator's to decide: where a function, class or global sits (which file, which header) while the evidence for its real home is missing. Each such placement is marked where it sits (`// PLACEMENT: provisional (D137): <the evidence that would place it>`) and listed in §4's provisional placements item; it is corrected when new evidence (a __FILE__, a string, an assert's line, an inlining or static-initialiser boundary) allows a better one. Placements with evidence on both sides still go to the user | user |
| D138 | 2026-10-29 | P-LD4-2 (under D137): the view at SClient +0x1358 (0x514CC0's class) stays at mainview.cpp's end, provisional, until a __FILE__ or a static initialiser places it | coordinator (D137) |
| D139 | 2026-10-29 | P-PX3-1 (under D137): the factories 0x452140-0x452770 are an object of their own, `app/shared/unknown_px_factories.cpp` (provisional, neutral name, D10), split from D124's unknown_px_callbacks.cpp: compiled together the factories inline PxBoxCallback's constructor 0x451C30, which the client calls; apart both are exact | coordinator (D137) |
| D140 | 2026-10-29 | P-IB2-1 accepted: one more short round on 0x6FD310 past LIMIT REACHED, at most 2 forms (IB2's form (1) plus a find-if-changed helper with the mark test inside, for all three finds and for Spot / Line only); neither exact: parked for good | user (IB2's proposal) |
| D141 | 2026-10-29 | P-F9-2: every compile of a variant counts as a form toward a round's cap, scratch-copy probes included, for Fable (D89) and Opus rounds alike; today's overruns (F9's verb round, HL1's Resolve 0x487660, HL2's two 4-form functions) are accepted after the fact | user (F9's proposal) |
| D142 | 2026-10-29 | P-UI1-1 accepted: clienttest depends on build/art/MSS32.DLL (the Miles imports of the rebuilt loading view), a copy of the game's DLL in the project's art folder (D120); never run from the install folder (D69) | user (UI1's proposal) |
| D143 | 2026-10-29 | The game's install folder (`C:\Program Files (x86)\EA GAMES\Earth & Beyond`) may be read in place by any session, and any of its files copied out as needed (for programs, into build/art or another folder under build/). Not granted: writing into it, or using it as a working directory (D69 stands). The final product uses the original game art once it is playable | user |
| D144 | 2026-10-29 | P-GH-2: one repository for the reconstruction (`Open-EnB/EnB`: the libraries, app/, every executable of the original tree under programs/, tools, docs) plus satellite repositories split by codebase or trust (`.github`, a future server); no repositories for third-party material, which stays bring-your-own ([github-plan.md](github-plan.md) §1) | user |
| D145 | 2026-10-29 | P-GH-3: the commit email stays as it is; no history rewrite for it | user |
| D146 | 2026-10-29 | P-GH-6, the first part: contributors build their own VC6 SP4 environment from public sources (Hyper-V; the VC6 and SP4 media from the Internet Archive, X04-14444 and X05-86594) with detailed user documents and scripts from the project; the project links to the media and checks their hashes but never redistributes them. Which contributions need no VC6 stays open | user |
| D147 | 2026-10-29 | P-GH-1 accepted as revised: LF everywhere, enforced by `.gitattributes` (`* text=auto eol=lf`, which overrides any `core.autocrlf`; `*.bat`/`*.cmd` CRLF; `docs/data/asserts.txt -text`); every working tree converted once; `tools/eol.py check|fix` the guard (run by commit_mine.py and the handoff once step 3 wires it); tools write and compare LF (step 3, a verified round); the line-ending traps in workflow.md / builders.md collapse into one rule (workflow.md §3.0). No Claude Code hook. Evidence: VC6 compiles LF sources to the same bytes (full verify, 12,541 pairs unchanged); ~25-35 incidents and ~3,000 defensive commands in 245 transcripts ([github-plan.md](github-plan.md) P-GH-1) | user |
| D148 | 2026-10-29 | P-GH-4: option C. Once the GitHub remote exists, `master` (the default branch, D152) changes only through a pull request: each run works on its own branch (`run/<date>`), the coordinator pushes that branch and opens one PR at handoff, and the user reviews and merges (no auto-merge: a checkpoint before work becomes public). Contributors use PRs from forks. D69 amended: "never push" becomes "push only your run branch, never `master`" (builders still never push; the coordinator pushes); every push passes `tools/prepush_check.py` in a pre-push hook; the agents' credential is a token for this one repository allowed to push branches and open PRs, nothing more. Before the remote exists nothing changes ([github-plan.md](github-plan.md) §3.1) Amended by D157 (verification before master, contributors' landings, the run PR as a draft). | user |
| D149 | 2026-10-29 | Three of Open-BFME's practices adopted: (1) the public README welcomes AI-generated PRs, one verified function per commit; (2) an `AGENTS.md` (the public form of CLAUDE.md / builders.md) that agents reread when 24 hours have passed or a commit reports it changed; (3) address claims, so contributors' agents do not work on the same function (built when outside contributors arrive; ours coordinate through the coordinator meanwhile). (1) and (3) amended by D157 and D156: one PR per claim, squashed; claims as issues. | user |
| D150 | 2026-10-29 | P-GH-5: the project ships only binaries built from its own sources, never a third party's (MSS32.DLL, Bink, DirectX, the game's files): users get those from other sources, like the game art (D143). EA's art and trademarks are not used publicly or in anything distributed; the README states the non-affiliation | user |
| D151 | 2026-10-29 | P-GH-6: contribution lanes. Without VC6: reverse-engineering notes (names under D56, layouts, evidence), Python tools and tests (hosted CI), documentation, and C++ written without VC6, which a maintainer's VM verify (label-gated) proves before merge; with VC6: C++ with the contributor's own pairs result, then the same maintainer verify; later the playable modern-MSVC build. Issues and PRs carry `needs-vc6` / `no-vc6`; CONTRIBUTING describes the lanes. The maintainer's verify is the verify service (D157). | user |
| D152 | 2026-10-29 | P-GH-8: the repository is `Open-EnB/EnB`, default branch `master`; created private, pushed, reviewed on GitHub, then made public; security reports through GitHub's private vulnerability reporting; the Contributor Covenant as code of conduct. P-GH-7 applied the same day: 2FA required for the org, members cannot create repositories, Actions limited to GitHub-owned and verified ones with read-only tokens, outside contributors' workflows need approval, secret scanning with push protection and Dependabot alerts on for new repositories, a `maintainers` team | user |
| D153 | 2026-10-30 | The private reference decoder's project is not part of this repository: no tool reads it and no file names it (the tree reworded, and history rewritten before the repository goes public). The opcode names and aux-data class names taken from the Net-7 lineage are final (pkfactory.py keeps its own table; auxnames.py and n7aux.py retired); the decoder is not extracted as a tool of its own (too much work for the value) | user |
| D154 | 2026-10-30 | The EA captures' reference data is frozen: the five traces and their reference readings (regenerated so each pair is consistent) ship as a release asset of this repository, `ea-reference-2026-10-30.tar.xz` (6.7 MB, SHA-256 checked by tools/harness/fetch_reference.py into roots.EA_REFERENCE); the raw captures stay public at trunk/capturedPackets of https://github.com/therealKyp/Earth-and-Beyond-server. Westwood's RSA key, published with that source, is committed (tools/harness/westwood_keys.py) | user |
| D155 | 2026-10-30 | The test programs are siblings under `tests/`, one folder each (tests/udpreplay, auxreplay, clienttest, auxdata_smoke, kerneltest, movietest), no longer under `tests/capharness/` (D45, D67). The M1.0 skeleton `capharness.cpp` and `tools/harness/build.py`, which built it, are retired: udpreplay and auxreplay carry M1's gates. | user |
| D156 | 2026-10-30 | P-GH-10, claims: contributors' agents and the maintainer's runs claim the functions they rebuild in one register, GitHub issues titled `claim: UNIT` kept by the Claims workflow (`tools/claims.py`); a claim is at most 30 not-yet-exact functions in one file's window (or a bare range), never a lock on editing the file; it lives 8 h without a PR (a draft counts; `/renew` +8 h twice), 3 days without the contributor's activity on its PR, 7 days in all, all counted only while it is the contributor's turn; 3 live claims per account, maintainers excepted. Option B (contributors with write access pushing branches) not adopted. [github-plan.md](github-plan.md) §4.5.1-4.5.3. The maintainer's runs' claims superseded by P-GH-11 (log entry 2026-11-01 00:15): they are lines in the run PR's body, not issues; contributors' claims stay issues | user |
| D157 | 2026-10-30 | P-GH-10, verification and merging (amends D148, D149(1), extends D146): every PR is verified before it reaches master, the maintainer's run PRs included, by a polling service on the maintainer's host (`tools/vc6/verify_watch.py`, a scheduled task running master's code; no self-hosted runner), which posts the required check `vc6-verify`; a PR that changes anything that would run on the host waits for a maintainer's `/verify-approved SHA`. Contributors' PRs land as one commit (the test merge plus the verified record, the PR's author kept) through a `verified/prN` landing PR the service pushes and merges: by itself (automerge on) when gain-only, CI green and only sources, headers and docs/data changed, otherwise on a maintainer's `/land SHA`; the maintainer's own PRs and any D66 case are merged by hand. One PR per claim, any number of commits; "one verified function per commit" retired. The run's PR opens as a draft at run start (`tools/run_branch.py`). [github-plan.md](github-plan.md) §4.5.4-4.5.5. "Any D66 case merged by hand" replaced by D169 (2026-10-31): a PR whose only losses are moves passes `vc6-verify` after the maintainer's `/accept-d66 SHA` | user |
| D158 | 2026-10-30 | File headers say where the file came from: our own files (app/, programs/, stubs/, tests/, the tools' C/C++, files written from the client inside a library) start with `// Earth & Beyond client (reconstruction)`, without Westwood's banner box or an EA copyright line; Westwood's banner and EA's licence header stay only on files imported from EA's C&C releases, and an imported file changed for E&B says so on the line under EA's copyright, written by importer.py itself (`MODIFIED_NOTE`, on assemble, add and replay); third-party code that is not EA's keeps its own notice (libs/sgistl). Header changes never move a code line (`__LINE__`). [conventions.md](conventions.md) §4. The importer-written note (`MODIFIED_NOTE` on assemble, add and replay) retired by D211 (2026-11-07): an EA-derived file's banner line is D211's fixed text, its changes listed in the end-of-file `// EnB:` block | user |
| D159 | 2026-10-30 | Unknown members follow §1's patterns like any member: `Unknown7C` (CamelCase, no prefix), `UnknownFlag`, `Unknown_7C` for a method; Generals' `GameEngine` code (`m_` members, camelCase methods, `AsciiString`) is the counterexample, not E&B's style. Inside an imported library its own style holds: WWAudio's members are `m_`-prefixed, so its unknowns stay `m_Unknown7C`. [conventions.md](conventions.md) §1 | user |
| D160 | 2026-10-30 | P-CSC-1: the Character and Starship Creator (2002-09-13) is a reference, not a rebuild target. It is the 2002 witness for NOT EXACT functions (98 tree functions are exact in CSC and absent from the client: source matching 2002, client changed later, so code-form retries won't help) and a second reference for code the client lacks or drops. Names still come from the client (CSC folds identical functions). [CSC.md](CSC.md) §1.1, §6.3 | user |
| D161 | 2026-10-30 | P-CSC-2: CSC's source and toolchain facts are recorded in [source-tree.md](source-tree.md) (`C:\Projects\G`, `Tools\Avatar Creator`, `chatint` / `textwin` / `usermsg`, `wnet\tcpmgr.cpp`, 0x836960 in galaxy_map.cpp) and [toolchain.md](toolchain.md) §1.2 (VC6 RTM throughout, no 8798 objects, `/OPT:REF,ICF`) | user |
| D162 | 2026-10-30 | P-CSC-3: a functional CSC becomes a milestone once the client's character-creation screens run on the tree; not before ([CSC.md](CSC.md) §6.2) | user |
| D163 | 2026-10-30 | P-API-1: the user's $200 of Anthropic API credits go to (1) a pilot builder run in Claude Code on a separate API key (spend limit set in the Console; the coordinator stays on the subscription; model and tokens logged in the plan beside subscription runs, for the cost per pair gained), then (2) a one-off Batch API triage of the not-yet-exact functions' saved pseudocode (`.work/pseudo/`) with a cheap model: STL / template instance, Westwood library or E&B code, and the class or file its callers suggest; hints for planning only, never names (D56), objmatch / verify decide; (3) the rest as overflow builder capacity (work the weekly window turns away, the D89 Fable queue). Next session (2026-10-31 morning) | user |
| D164 | 2026-10-31 | P-MU1-1: explicit instantiations (`template C *Unknown_Find_Field<C>(...);`) are an accepted matching form for emitting the client's out-of-line template copies (first: ui_manu's 0x5B6DB0, 0x5B70C0, 0x5B7210, 0x5B72B0, probably 0x5B7050), each marked in the source as not the client's form (like aux_roots' SYMBOL_PAD); callers stay unchanged | user |
| D165 | 2026-10-31 | P-SM2-1: FAIL_IF in app/shared/unknown_debug.h becomes `if ((expr) && (Unknown_Fail_If_Report(#expr, __FILE__, __LINE__), 1))` (the app-wide trial: +6, scriptedmovie 0x803310, co 0x6FEC40, ini_manager 0x4EB1B0, px 0x44C2E0 / 0x44C520 / 0x44C690; -3, client_lego_impl 0x72D5F0, co 0x6FFFF0, sclient 0x750260). A builder fixes the three losses in their own files first; the macro changes in the commit that loses nothing | user |
| D166 | 2026-10-31 | Caller tie-break: when objmatch finds one of our functions at several byte-identical client addresses (`multiple` + `identical`, D54's ambiguous column), verify pairs it with the address whose callers are all already paired exactly, in the same file, with the functions that call it in our object (first: ui_cpit's 0x584C80, called only from 0x57DD20; land's 0x47CF50 from 0x47A2F0). An address with callers in several files (the linker's merged copy, 0x474080 with 247 callers) or with an unpaired caller stays ambiguous. Deterministic (passes until nothing changes); `pairs_cmp.py` lists tie-break pairs separately. D66 (vii): a tie-break pair lost because its caller lost its pair is not a loss of its own, named in the commit. The switch lands as one commit with a full verify of all 11 libraries (the one-time gain), apart from any builder's work | user |
| D167 | 2026-10-31 | P-API-2, D163's Batch API triage: one tool, `tools/re/triage.py` (`build`, `submit`, `fetch`, `ingest`, `plan`, `show`, `score`), run by the coordinator only, one batch at a time. Each request holds one function: its pseudocode from `.work/pseudo/` (rebuilt in full first), the strings it uses, its callers' and callees' names and files, the nearest functions with a file above and below it, and its region. `custom_id` is `fn-<ADDR>`, every request stores a hash of its input, and a rerun resends only changed inputs. The batch id is recorded in `.work/triage/batches.json` at submit (results expire after 29 days); raw results are kept unchanged in `.work/triage/raw/`; tokens and cost go to the run's plan log. The answers follow a JSON schema: kind (STL / template instance, Westwood library, E&B, CRT, unknown), suggested library / file / class, confidence, the evidence cited. Rows that contradict a deterministic fact (`__FILE__`, gaps windows) are flagged. A calibration batch comes first: about 300 functions already byte-exact (pairs from imports.json, D164's template copies, LD6's std::sort 0x4708F0 / 0x472910 and its helpers), decompiled with their own name and `__FILE__` strings removed, run on Haiku 5.5 and Sonnet 5.5. The full batch runs on Haiku 5.5 at effort `low` if it gets the kind right ≥ 85% of the time and the file ≥ 50% at confidence ≥ medium; Sonnet 5.5 only as a second opinion on the disputed or low-confidence rows. Hints only (D56): never in symbols.json, the sources, NAME tags or `docs/data/`; objmatch / verify decide. Superseded in part: the full batch's model by D170 (Sonnet 5.5; Haiku failed the gate), "never in `docs/data/`" by D172 (the table is tracked there) | user |
| D168 | 2026-10-31 | P-API-3: the triage table stays under `.work/triage/` (git-ignored, read by builders on this tree), not in `docs/data/`. Revisited once `triage.py score` shows whether the hints hold up (§4's D163 item). Superseded by D172 (2026-10-31): the table is tracked in `docs/data/` | user |
| D169 | 2026-10-31 | D66 moves past `vc6-verify` (amends D157's "any D66 case is merged by hand": the master ruleset requires `vc6-verify` and has no bypass actors, so that path did not exist; first hit PR #39, the pilot's scriptedmovieline.cpp with 0x7FFD30 / 0x7FFDC0 moved from scriptedmovie). A PR whose every lost pair is a move (the address held by another file in the same verify, none lost outright) fails until a maintainer comments `/accept-d66 SHA` naming its head (or runs `verify_pr.py PR --accept-d66`). The service then verifies it again and posts success, the moves and who accepted them named in the status and the comment, and the PR merges or lands like one that lost nothing. A pair lost outright still fails and waits for the user; no command passes it. The command is the user's: agents list the moves for the user and never post it (comments from this host carry the user's login). No bypass actor on the ruleset. `tools/vc6/verify_pr.py`, `verify_watch.py`, [github-plan.md](github-plan.md) §4.5.5 | user |
| D170 | 2026-10-31 | D163's full triage runs on Sonnet 5.5, over the 14,753 functions in the application range only (amends D167's "the full batch on Haiku 5.5"). The calibration (`triage.py calib`, 300 byte-exact functions with their own name, class and paths removed, no neighbours or files): Haiku 5.5 kind 79.7 %, file 40.0 % at medium / high (6 of 15 right even at high), failing D167's gate; Sonnet 5.5 kind 89.3 %, file 63.2 % (19 of 24 at high), passing it. Sonnet costs about $0.0027 a function, so all 20,588 would be about $53; the 5,835 library-range functions (Westwood libraries, which gaps.py already locates from EA's source, and the runtime) are left out, estimate $41, inside the `enb-d163` workspace's $60 limit | user |
| D171 | 2026-10-31 | P-AR1-1: objmatch's D88 self-name round also uses the slots of vtables learned in the same run (from the exact constructors that store them, e.g. 0x702890 stores 0xB01844) and learns the relocations of short functions at addresses the run already knows, breaking the relocation-only ties of template instances whose vtables symbols.json never names (aux_roots' accessor / KVPair deleting destructors and destructors). Measured on objmatch alone (AR1's prototype `.work/ar1/proto_om.py`): aux_roots.obj 102 -> 335 exact, all 241 app objects +318, 0 lost; shipcust_desc's three range credits move to vtable addresses (0x4F7000 -> 0x4484C0, 0x4F7520 -> 0x42F010, 0x4F7550 -> 0x4488E0; 0x42F010 to be checked). Order (the user, 2026-10-31): once the tree is quiet, symbols.json is regenerated first (it lagged imports.json: 10,005 exact against 13,341 pairs), with the full verify workflow.md §3.4 requires; then this lands as one commit with a full verify of all 11 libraries, the moves listed for the user | user |
| D172 | 2026-10-31 | P-API-4 (D168's revisit; replaces D168's ".work only"): the triage table is tracked, written by `triage.py ingest full`: `docs/data/triage.json` (one row per line: address, kind, library, file, class, template_of, confidence, contradicts; 1.2 MB), `docs/data/triage-evidence.tsv` (the evidence each answer cited; 3.6 MB) and `docs/data/triage.md` (counts, the score, the reading rule), each under the pre-push check's 5 MB; `plan`, `score` and `show` read it. Still hints, never evidence (D56, D167): no matching tool reads it. Reading rule: a low-confidence hint, or one for a function at a file boundary (the nearest known functions before and after it in different files), is a lead; confirm the object's boundaries (statics, literals, padding) before placing. Evidence: `score` against what is exact, high 494/494, medium 1,219/1,271, low 60/83; the builders' tests against pairs, both neighbours the hinted file 23 of 23, a file boundary 18 of 23, every miss one object no evidence names (unknown_screenfade.cpp: the hint said a neighbour's file) | user |
| D173 | 2026-10-31 | P-SY1-1: the symbols.json regeneration's one loss, aux_roots' `??_G KVAccessorClass<UnknownB016DCType>` 0x723AB0, is a false pair corrected (D66 (iii)-like): it moves to 0x4B2AA0. Evidence: our object's `??_7` has 4 slots like the client's 0xAEB4BC (slot 0 thunk 0x405EB6 -> 0x4B2AA0, then three purecalls); 0x723AB0 is slot 0 of 0xB02C84, a 2-slot vtable of another class. HEAD's pair came from D100's range tie-break; the regenerated symbols.json lets D171's learned-slot self name resolve it first. Lands with the regeneration (+5 net; dc424b62, taken out of PR #78 to a follow-up PR) | user |
| D174 | 2026-10-31 | P-TK3-1 + P-TK2-1 + P-TK5-1 + P-TK3-2 as one objmatch change to D166's caller tie-break: copies differing only in relocation targets count (P-TK3-1, +82 app measured); the tie-break also runs on D80's identical survivors and the D80 / self-name / range rounds repeat after it (P-TK2-1, +45); one of an object's linker-folded identical symbols is credited (P-TK5-1); an EH unwind funclet's call counts as its parent's (the function whose FuncInfo references it, P-TK3-2). Landed together, measured together, after a full verify of every library, gain-only (any loss back to the user). The user's sequencing: after PR #78 and P-GH-11's PR #135 are resolved, in the same session | user |
| D175 | 2026-10-31 | P-TK7-1: wlib takes E&B's own configfile.cpp + xtime.cpp revision (configfile without sectionList and MDC's writers, readFile keeping empty values and joining backslash lines, clear 0x8B2690; xtime without FormatTime / ParseDate / Print* / getYDay): wlib +11; the 4 pairs lost outright (0x77AA10, 0x77B030, 0x77B690, 0x850820: app template / iostream instances only matchbot's sectionList / writeFile credited to configfile) are accepted (into accepted-corrections as decided losses) and come back as their app files are built | user |
| D176 | 2026-10-31 | P-TG3-1: CordCon takes the client's 0xC0 layout: its three harness counters (AcksMatched_, ResendRequests_, Rejected_) move to a side table in udpreplay, and the constructor takes the key as one `__int64` (unblocks the ctor 0x84F640 and ACK2 0x85ABB0) | user |
| D177 | 2026-10-31 | P-TG4-2: D130 covers dead spenders in a NOT EXACT function kept for another function's pair: ui_avatar_cust 0x550A10 keeps its five dead finds (Rotate_Y 0x551270's pair), marked, until 0x550A10 is rebuilt | user |
| D178 | 2026-11-01 | P-NW3-1 / P-NW4-2: CordMgr's +0x58 takes the client's meaning (MaxPerSocket_, the per-socket connection limit); the harness's Unexpected_ count moves to udpreplay's side table, as D176 did for CordCon (unblocks Connect 0x858EC0 and the buffer setter 0x8578C0) | user |
| D179 | 2026-11-01 | The proof-of-concept client (videos -> login -> character select against antprox's /REPLAY): its data is `C:\EnBDevelopment\ClientPOC` (a copy of the game data in EA's layout, not the install folder); the POC exe is deployed into `ClientPOC\release` (the third-party DLLs are already there), and running it there and writing there is allowed. Modal dialogs are allowed in the POC client only; unattended test programs keep D69's `-nomsgbox`. D69 / D143 still govern the real install folder (never its working directory, never written) | user |
| D180 | 2026-11-01 | P-LG1-1 kept: LoginTicketInfoClass is 0xA0 bytes (its +0x20 an `__int64`, 8-aligned; 9c0a0162, +1, 0 lost); GlobalLoginPacketClass +0xAC and LoginTaskClass +0x1134 become tail padding | user |
| D181 | 2026-11-01 | P-POC-4: builders may run antprox (`/REPLAY /CLIENT:` the POC exe in `C:\EnBDevelopment\ClientPOC\release`) for the POC, and HKCU registry edits to E&B-related keys / values are allowed (amends D69's "no registry changes" for this case only): antprox's AppCompatFlags\Layers entry for client.exe and its `Westwood Studios\Earth and Beyond\Registration` `Registered=1`. Run unelevated (the session is), so the latter lands in HKCU's VirtualStore, never in HKLM itself; no other registry changes | user |
| D182 | 2026-11-01 | P-CT-1: the coordinator runs clienttest (build + run: 103 checks, trace equal to expected.txt) and udpreplay (build, run alex3, handshake 54/54) at HEAD after any landed commit that rebuilds a file they link, and once more before every handoff (coordinator-pacing skill, "Harness check"); a failure is a health item fixed by one bounded builder before further launches. P-LINK-1 closed: the user restored ClientPOC's intro movies' names (eb_sizzle.bik, eb_ws_logo.bik) | user |
| D183 | 2026-11-01 | P-IS-1: objmatch's D114(b) range widening credits a COMDAT held by two or more objects at the same client address with the same symbol to the holder whose own range is strictly nearest (a tie credits none; amends D114(b)'s "only if one object holds it"; first: 0x6DAF80, credited to nobody until named in symbols_named.json). Amended by the user 2026-11-01 (P-IS-2): "each holder" lost inpdsp's 0x42A900 / 0x42AAE0 outright (one holder's widened range reached over the other's code); D166's tie-break labels a decision that needed a shared caller (`shared_callers`) instead of skipping it (skipping lost 5 tie-break pairs). Landed 05b691f4: +14, 0 lost, no moves. Guard: such pairs are marked shared, counted once in the totals (imports.json, coverage), and never used as placement or file-boundary evidence (gaps, triage's neighbours; D166's tie-break labels them); the function's home stays an interim placement (D137) until other evidence decides it. Lands as one objmatch commit in its own PR with a full verify of all 11 libraries against master 7fb89dbb, the moves and shared pairs listed for the user; any pair lost outright back to the user | user |
| D184 | 2026-11-01 | P-API-5 (b) piloted: a Batch API variant pilot for NOT EXACT functions (one new tool on triage.py's pattern: build / submit / fetch / try; the key `ENB_ANTHROPIC_API_KEY`; batch ids, raw results and cost in `.work/`, cost in the run's plan log). 50 functions from docs/data/not-exact.md, at most $10; each request holds the function's source, the client's and our disassembly, explain.py's class and the matching workflow.md §3.1 traps and asks for up to 8 variants; the try step compiles each with appfile.py on the pool and reports which are exact. The pilot changes no app source: exact variants are applied afterwards like any builder's change (pairs_cmp, D66). Its result (cost per function made exact) decides P-API-5 (b)'s scale-up; (a) and (c) stay open | user |
| D185 | 2026-11-01 | P-API-5 (b) scaled up: `tools/re/variants.py` runs over the whole NOT EXACT backlog (docs/data/not-exact.md, 479 entries: every function an entry resolves to one client address, the functional (D64) rewrites and long functions included) on the Batch API (claude-opus-5-5), round 1 for all, round 2 for the near misses. The whole batch's cost is counted (token counting) before submitting and the `enb-d163` workspace's spend limit is the user's to raise; `try` runs unattended on the pool. Exact variants are applied afterwards by builders through verify / pairs_cmp (D66), as in D184. Pilot: $4.66, +13 pairs (da66757d) | user |
| D186 | 2026-11-02 | P-B2 adopted (D84's condition met): per-file compile flags, `importer.py FILE_FLAGS` keyed (lib, file). First entry wwlib `rawfile.cpp` with /GX: every RawFileClass function lies in the app range (0x683BE0-0x684DD0, none in wwlib's 0x9C7000-0x9E9300) and its ctor / dtor carry EH frames (handlers 0xABF368 / 0xABF393), so E&B compiled it with the app's /GX. wwlib +4 (0x683BE0, 0x683E10, 0x683EA0, 0x683FB0), 0 lost; found by the agy pilot builder RF | user |
| D187 | 2026-11-02 | P-AGY-1 approved: Gemini 3.8 Flash (high) through the Antigravity CLI (`agy`, the user's Google subscription) is a builder model next to D72's, for bounded POC and triage items (the functions of one file, 30-60 minutes). Launched by `tools/agy/launch.sh` in a `wt.py` worktree, landed with `wt.py bring-over --target run/<date>`; paced by the user's agy window (about 1 % of the 5-hour window per 100k stream tokens, the user's reading at the start and mid-run; a 429 stops launches); the coordinator runs every POC check itself (`tools/agy/poc_check.sh`: agy-started processes get no D3D device). Not Opus through agy (its quota covers one short item). Evidence: run 2026-11-02 evening, nine Flash items, +39 pairs and 0 lost after the coordinator's verify of each | user |
| D188 | 2026-11-03 | TW3 / TSH proposal (a) accepted with conditions: objmatch credits a `short` function (fewer than MIN_FIXED fixed bytes) as exact only at an address the run knows independently of its bytes: a learned vtable slot (D171), the target of a call from an exact function, or a constructor's vptr store; its pattern must hold there and its relocations' targets must agree (D80). Never by pattern alone. A short function that is a COMDAT (header inline, template, compiler-generated) is marked `shared` (D183's guard): it credits the address, not a file placement. | user |
| D189 | 2026-11-03 | TW3 proposal (c) accepted: symbols.py drops an exact entry that lies inside another exact entry of the same symbol (stale mid-function matches such as WW3D::Render 0x917C5C, RegistryClass::Set_Bin 0x9D18D9, Delete_Value 0x9D1C29, left by versions without E&B's early-out guard); and objmatch credits a match only at a known function start or right after padding / another function's end, never inside a function. Proposal (b) (source-order tie-break for byte-identical overload twins) is left open; checked 2026-11-10 against tools/re/objmatch.py: no round uses source order. Later rounds settle the twins something else tells apart (D166 / D174: their callers, relocation targets, and one credit for copies the linker folds; D88 / D171: a self name; P-CW1-1: the stronger evidence in the range round); twins of one object that nothing references differently stay in D54's ambiguous column. | user |
| D190 | 2026-11-04 | P-LIGHT-1 approved: `clientpoc.py replay --retail` runs a copy of the retail client.exe (copied out of the install folder, D143) with a d3d8shot build that has poc_input-style user32 hooks (D3D8SHOT_INPUT=1) and a BM_CLICK on the terms box's "I Agree" (the dialog's own input, D69), so any builder can take a retail reference shot or draw dump on the same replay. From LIGHT's `.work/wt/light/.work/light/` (agree.py, build_d3d8in.py, cpoc_retail.py, cmp2.py / cmp3.py) | user |
| D191 | 2026-11-04 | P-QUIT-2 approved: `clientpoc.py replay` prints whether the client exited and when (antprox.out's "Client exited" line against the last input's time), as `run` does | user |
| D192 | 2026-11-04 | P-CGP-1 approved: objmatch's D100 range round drops a survivor the object's own exact functions contradict (their relocated calls to the symbol reach other addresses only), so two same-bytes COMDATs of one object no longer both propose one address; with it ui_cpit 0x584110 / 0x5845A0 take the client's Create_Peek (0x546510). The one loss, avatar 0x780AC0 (ColorizationHalfStruct::operator=) moving to the shared 0x62A5D0 that the exact `vector<ColorizationRegionStruct>::insert` 0x62A0A0 calls, is accepted | user |
| D193 | 2026-11-05 | P-CS1-1 approved, option (a): a byte-identical linker-folded COMDAT that two symbols claim (objmatch.short_known, D188 short pairs) is credited to every file that proves the address instead of being dropped; then avatar.h's inline `CharacterViewAvatarClass::Get_Desc()` lands, making ui_avatr 0x559350 / 0x559490 exact without losing ui_avatar_cust's 0x551D20. | user |
| D194 | 2026-11-05 | P-CS4-1 approved, option (a): D193's folded-COMDAT test (objmatch `body_sig`) also applies in D166's caller tie-break (`caller_tiebreak`'s by_addr check), so a byte-identical linker-folded body that several symbols propose (lookup_table's four Create_NEW at 0x4EE9F0) is credited instead of dropped; a narrow tool change with a full verify. | user |
| D195 | 2026-11-05 | P-AR2-1 approved: `tools/re/unref.py` lists, per app file, the client function starts in the file's range with no call / thunk reference in .text and no .rdata (vtable) reference that are not yet in the tree (promoted from `.work/ar2/unref.py` / `vt.py`): unreferenced members VC6 auto-inlines into callers, the lever that decides which nested helpers the client calls out of line (avatar.cpp, AR2: 18 found, 16 exact, +31). | user |
| D196 | 2026-11-05 | objmatch's D114(b) range widening is chained: each COMDAT a file is credited with widens the range the next one is measured from, until it stops growing (same holder and D183 nearest-holder conditions). Cause (SC4): ui_ship_cust.cpp's `<string>` initialiser 0x620CA0 (`std::ctype<unsigned short>::id`, guard 0xC416FC) sits after more than 0x1000 of the file's map / MemberCallback copies, so the unwidened range stopped at 0x620640 and the pair was lost when ui_shipcust's false match went (PR #163's vc6-verify). Measured on the run branch: app +85, ww3d2 +7 shared, 0 lost, replay 0; 'tie-break by callers' 433 -> 171 (the same pairs credited by the range first). Narrowed by SC4 (wt/sc4 00c32265): the chain runs only upwards, through COMDATs the object alone is exact at (the full chain lost ui_shipcust's 0x620CA0 / 0x628E40 on master). The false pair ui_shipcust 0x620CA0 is a decided loss in accepted-corrections.json (PR #164). Order: #164, then #163 (its 12 moves by `/accept-d66`), then the objmatch PR from the new master | user |
| D197 | 2026-11-06 | P-ST1-1 accepted: ui_starbase 0x648380 moving to 0x646120 is a D66 non-loss (the kind of (vi)). The two are the client's byte-identical copies of `std::ctype<unsigned short>::id`'s guarded initialiser in one object; ours compiles one and D100 credits it to whichever copy the file's range reaches first. `.work/st1/helpers_640CC0.patch` is applied as it stands (+10) and the rest of ui_starbase.cpp may proceed. The entry in accepted-corrections.json lands first (PR #169) | user |
| D198 | 2026-11-06 | Names for the assert and message objects. The client's (NAME 5, each pushes the string to its WWPROFILE): reporters 0x4BBED0 / 0x4BC000 / 0x4BC130 / 0x4BC260 / 0x4BC460 / 0x4BC5A0 = `Message_Debug` / `_Info` / `_Warn` / `_Error` / `_Stat` / `_Common`. Chosen (NAME 2; the client's are not recoverable from it: no __FILE__, no label, no RTTI): `Message_Critical` 0x4BC3A0 (level 0x20; no caller, no label), `G_Assert_Report` 0x4E9410, `Fail_If_Report` 0x4E9450, `g_assert.h/.cpp`, `message.h/.cpp`, `message_filter / _record / _server.cpp`, `MessageRecordClass`, `MessageFilterClass`, `MessageServerProtocolClass`, `MessageServerProtocolEntryStruct`. `unknown_new.cpp` stays. A real name found later is one more scripted rename | user |
| D199 | 2026-11-06 | P-EA1-2, the user: client.exe 249f8524 stays the matching target, recorded as what it is: EA's final client 82663b2f (same size, `.text` and PE stamp identical) with 720 bytes of Net-7's URL edits in `.data`, 0xB84710-0xBA013F (`auth.east.ea.com,auth.west.ea.com,www.ea.com` -> `play.net-7.org`, the store / manual / support links). No function's bytes differ. The project began against Net-7's copy and continues against it, so the tree keeps those strings. | user |
| D200 | 2026-11-06 | P-EA1-1 (a), the user: `C:\\EnBDevelopment\\ClientPOC_EA` (`Data`, `release`, EA's final set only, local, never committed) beside `ClientPOC`, which stays as it is. `clientpoc.py run|replay --ea --instance NAME` uses its Data and DLLs; the exe stays the POC build, `--exe`'s or `--retail`'s. Its `release\\client.exe` is EA's 82663b2f, kept as data, not run by the tool. | user |
| D201 | 2026-11-06 | P-AU1-1, the user: keep the data audit as a tool, `tools/re/datacmp.py`, but it does **not** run once per run: it is run when wanted. | user |
| D202 | 2026-11-06 | P-LAND-1, the user: batched landing moves into `wt.py` (one verify over several branches), `tools/agy/rebase_gen.sh`'s importer calls get `--lock-wait`, and a batch whose only losses are D66 moves and shared holders lands when `lost outright` is empty (the merge still waits for the maintainer's `/accept-d66`). | user |
| D203 | 2026-11-07 | P-RS1-1, the user: "if 0x7B7EF0 is a miss, it's a miss": a pair that holds only with the exact text of headers other work must edit is a near miss, not a pair to protect. nav_list_builder's 0x7B7EF0 and its two vector destructors 0x7B8ED0 / 0x7B8F10 are dropped: their loss is accepted under D66 (named in the commit, `--accept` in the heartbeat) and they go to attempts.csv. Builders never work around other code to keep a pair won earlier; the aim is both changes exact at once. | user |
| D204 | 2026-11-07 | P-CM1-2, the user: yes: the six local stand-in CameraManagerClass declarations (co, cpacket, ctrngadg, sclient, ui_cpit, ui_verb) move to camera_manager.h, one file per verify. | user |
| D205 | 2026-11-07 | P-CM1-1, the user: yes: sclient.cpp's `Unknown_79B9E0()` becomes `CameraModuleClass::Get_Signal_Map()` from cammod.h, with a verify; nav_list_builder's header text no longer binds it (D203). | user |
| D206 | 2026-11-07 | P-KV1-1 and P-KV1-2, the user: yes to both: DeltaChildVisitorClass becomes KVEncoderClass, with the client's slots (vtable 0xAEB6B8: +0x08 Accel, +0x0C Matrix3D, +0x10 Vector3, +0x14 / +0x18 per frame), when aux_roots is next repadded. | user |
| D207 | 2026-11-07 | P-GR2-1, the user: yes: clientpoc.py's `exit_report` clears `killed` when it breaks on antprox.out's "Client exited" (Python, `/verify-approved`). | user |
| D208 | 2026-11-07 | P-GD1-1, the user: rename Unknown_674260Class to `UIBackGadget` now (a second string may never come): a Haiku builder first searches briefly for better evidence (strings, RTTI, asserts near vtable 0xAFB5F4); its name wins if it finds one. | user |
| D209 | 2026-11-07 | P-IR2-1, the user: measure first whether D100's range counting an object's short initialisers at known addresses gains pairs anywhere besides inv_render's 0x68FAE0 (a full verify on a quiet tree, pairs_cmp, like D88 / D100). If it does, propose the rule change; if not, accept 0x68FAE0's loss under D66, move 0x68FB60 / 0x68FC80 to inv_trade_render.cpp, and work out how to make 0x68FAE0 exact there too. Resolved by D212 (2026-11-08): the range widened over static initialisers, 0x68FAE0's loss accepted, the three functions moved to inv_trade_render.cpp. | user |
| D210 | 2026-11-07 | P-GD3-1, the user: rename 0x674C10's class (0x68 bytes, vtable 0xAFB7AC; derived from UIBackGadget like MovableUIBackGadget, GN1) from UIBackGadgetClass to `BoundedUIBackGadget` (NAME 2: it answers the pointer only inside its back_tl / back_br rectangle). | user |
| D211 | 2026-11-07 | The user (docs/pipeline-plan.md, agreed 2026-11-07; carried out run 2026-11-07b): the EA freeze. The EA-derived libraries (ww3d2, wwlib, wwmath, wwdebug, wwsaveload, wwaudio, wwshade, binkmovie, libs/wwonline) are plain source: edit them directly. Each E&B change to an EA-derived file is described in the file's one end-of-file block of `// EnB:` entries (conventions.md), never inline above code; the banner line under EA's copyright reads "Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file." PATCHES, APPENDS, NEW_FILES, EXTRA_FILES, annotations.py and importer.py's plan / assemble / add / replay / refine are retired, as are addpatch.py, mkspec.py and the EA-copy swap tools; the bytematch evidence is reference only. EA's copies (C:\EnBDevelopment\EA) and imports.json's copy / best / merged stay as provenance. Preceded by a final refine pass that kept nothing (its 3 ww3d2 proposals were single-file artifacts). | user; done 952f432f (202 files, 1,012 entries), 323f258d (machinery); full verify 0 gained, 0 lost |
| D212 | 2026-11-08 | P-IR3-1, the user: as recommended: (a') objmatch's range widened over an object's located static initialisers, narrowed as V7d (D80 amended; fb07b848, +35, 0 lost), and (b) 0x68FAE0's loss accepted under D66 with 0x68FB60 / 0x68FC50 / 0x68FC80 moved to inv_trade_render.cpp (e98a6fce; accepted-corrections.json as D209) | user |
| D213 | 2026-11-08 | P-CW1-1, the user: in objmatch's range round, of several symbols proposing one address the one with strictly the strongest evidence is credited (the client's self name or an exact function's call / reference first, then calls into the object's own range), and a proposer whose call into its own object lands on other bytes is refuted; a remaining tie still drops all (9c6cac86, f69ae7e4) | user |
| D214 | 2026-11-08 | P-CW1-2, the user: clienttest traces the chat lines at the real SClient 0x74D990's call of 0x563760 (a `#define` seam in sclient's wrapper) instead of a stub of 0x74D990; the real function lands, both stand-ins go (a2d280d1) | user |
| D215 | 2026-11-08 | P-CW1-3, the user: UIChatPanelClass -> ChatPanelInterface, UIChatIntClass -> ChatWindowInterface everywhere (the client's MemberCallback RTTI, 0xB7D308 / 0xB7D408; af0f630d) | user |
| D216 | 2026-11-08 | P-API-5 (b) again, the user: one single Batch API round (claude-opus-5-5) on the $67 left: today's NOT EXACT functions not sent on 11-01 plus the re-runs RT1's re-test justified, each with its clue (`variants.py pick --exclude-sent / --add`, `build --clues`); results applied by builders through verify / pairs_cmp (D66). b1108: 313 requests, $35.76 | user |
| D217 | 2026-11-08 | P-SC1-1, the user: accept the loss of 0x752170 (UnknownB00A00Type's copy constructor, byte-identical at 0x499720 / 0x661D70 / 0x752170, held by aux_roots only through the range tie-break) and land SC1's placement of Player's aux tree in sclient.cpp (D137 interim; 0x7551D0 / 0x753C60 exact). Recorded in accepted-corrections.json (PR #179). It returns when sclient's 0x751BE0 is exact. 0x752170 pairs again (run 2026-11-08d, SC2 43ae9d86: 0x751BE0 exact). | user |
| D218 | 2026-11-09 | P-IP1-1, the user: as recommended: shared headers make UISlotInfoItem = InvDataClass and UISlotInfoItemUnknown54Class = InvDataPeekClass; ui_item_examination and inv_render move onto the real classes (0x521B80 pairs through the tie-break until then). Not yet carried out. | user |
| D219 | 2026-11-09 | P-IP1-2, the user: as recommended: merge ItemTemplateInfoClass into ItemBaseInfoClass (pkitemba.h); client_item_property_builder.cpp treats them as one class (attribute ids 1-37, flags at +0x58). Not yet carried out. | user |
| D220 | 2026-11-09 | P-IB2-2, the user: no, as recommended: ui_mfd keeps the client's two-layer find; 0x5BF350 stays not exact rather than gained by a one-layer form the client does not have. | user |
| D221 | 2026-11-09 | P-FN1-1, the user: no for now, as recommended: no FontSys sub-object for +0x2B4..+0x2CC on access patterns alone; revisit once 0x6CE4E0 and 0x6CD6F0 are exact (0x6CD180 is, FN2). The user put both at the top of §4 for the next session. | user |
| D222 | 2026-11-09 | P-XR2-1, the user: instead of the hand-cut msxml3.tlh / .tli, VC6's whole #import output of msxml.dll, MSXML 2.0's type library, extracted from the Windows XP SP3 ISO's I386\MSXML.DL_ (8.00.7002.0; the DLL stays in `.work/msxml/xp_sp3/`, never in the tree): app/client/msxml.tlh / .tli, namespace MSXML. The client's .rdata holds this library's DOMDocument CLSID 2933BF90 (0xBA0F90) and IIDs 2933BF80 / 81 / 82 / 83 / 8B and none of MSXML 3.0's; the hand-cut header had compiled msxml3's DOMDocument CLSID F6D90F11 (a functional difference pairs cannot see). verify: app 0 gained, 0 lost against HEAD (0x5FA5F0 kept); linkgate passed; the object holds 2933BF90. Scripts `.work/msxml/`. | user |
| D223 | 2026-11-10 | The workflow review, the user: (a) the traps of workflow.md §3 are rewritten on their own branch (stale bullets deleted, repeats merged, §3.1 and §3.3 split into a part read up front and a part looked up by symptom, coordinator-only bullets moved to the coordinator-pacing skill); (b) superseded and amended rows of this table are marked in place, no archive split, no id renumbered or removed; (c) a **lander** subagent on Sonnet (`.claude/agents/lander.md`) runs `wt.py bring-over` for the coordinator and returns one line per branch; (d) not adopted: Claude Managed Agents' multiagent orchestration and Claude Code's advisor (no trial) | (a) an app builder's startup set had grown to about 260 KB against the 43 KB builders.md stated; (b) 187 of 225 ids are cited in live docs, tools or source and nobody reads the table whole, so pruning saves nothing and marking keeps a followed citation true; (c) the last five runs logged about 25 bring-over refusals and 15 builder resumes in the coordinator's context, which costs about six times a builder's per hour; (d) Managed Agents run in a hosted sandbox at API rates and cannot reach the VM pool or the worktrees; an advisor call re-reads the builder's whole transcript, the pairs check already does its "before done" review, and D89's limits could not be enforced on it |
| D224 | 2026-11-10 | The user, after the audit of this table: (a) D7's trigger (not-exact.md past 400; 742 entries) revisited: E&B's own code stays held to byte-exactness and the work continues toward it; (b) P-API-5 is closed: its pilot (D163 (a)) and the Batch API rounds (D184, D185, D216) were run; Batch variant rounds are the use of the API credit, the next one when about 300 new not-exact functions have gathered since the last; (c) rows without a Why cell (D188, D189, D193-D195, D199-D210) record "user"; D199 no longer cites D1 | user |

## 3. Open decisions

* **P-FN3-1** (2026-11-09, FN3; D221 revisited, recommend still no): no FontSys sub-object for +0x2B4..+0x2CC. 0x6CE4E0 and 0x6CD6F0 became exact without one. A byte scan of 0x6C0000-0x6E0000 (`.work/fn3/scan2b4.py`) finds the range's address taken only in the constructor 0x6D5170 (`lea edi, [esi+0x2B4]`, edi..edi+0xC pushed to WW3D::Get_Device_Resolution, then +0x2C4 / +0x2C8 / Scale stored); no call passes it as `this`, nothing copies it as a block, and every other function (Render, Change_Text, 0x6CE4E0, Update_Text, 0x6CE7D0) uses the fields one by one. Revisit with a sub-object whose inline init is that block when 0x6D5170 is worked (fontmanager.h, with the sclient pad check 0x74D990 / 0x758210 / 0x702240).
* **P-CW2-1** (2026-11-08, CW2; recommend not yet): the form stashed in docs/attempts/5719A0 (`Vector3 &f(Vector3 &color, entry)`) makes 0x5719A0 and 0x570FF0 exact but loses 0x571B70 (vector::size, then inlined into 0x56FD30): net +1 with one loss. Retry once 0x56FD30 is exact. (2026-11-08b: b1108 sent 0x56FD30, best variant 0.606 against 0.545, not exact.)
* **P-EA1-1** (2026-11-06, EA1; recommend not yet, after more POC paths have run on it): make `ClientPOC_EA` (D200) the POC's default instead of `--ea`? One replay reached the class screen and shows EA's six; `ClientPOC` still has what antprox's Net-7 sessions may need, including 401 installed files outside EA's set. If it becomes a project input, the scripts in `.work/eadata/` move to `tools/` (never the data).
* **P-VM-2** (2026-11-01, NW7 / FB1 / GM2 / LG3; recommend yes): `pool.py lease` (and `verify --pool`, `appfile.py
  --pool`) wait in a fair queue at the pool's cap instead of exiting with "pool cap: 7 VMs", and a lease process does
  not hold `leases.lock` past its own save (one held it 15 minutes on 2026-11-01). A tools change (Python: the
  maintainer's `/verify-approved`).
* **P-VM-1** (2026-10-29, coordinator; recommend yes): the build host's VMs need replacing by **2027-01-20**: the
  template `Win10` and the pool's base made from it stop working then, and every clone inherits the date. Rebuild the
  pool with `tools/vc6/setup/New-BuildVM.ps1 -ReplaceBase` ([setup-vc6.md](setup-vc6.md) §5; tested 2026-10-29: 5.5
  minutes, same pairs) on a quiet tree before then, and retire `Win10` as the template (`.work/vc6_config.json` then
  names `enb-vc6-template`)? Alternative: a Windows image without an end date (`-WindowsVhdx`).
## 4. Next steps

Outstanding work only (rewritten 2026-10-31: what is done went to the log entries, which hold each item's
history). Milestones M1 and M2 are met (D42, D63; [milestones.md](milestones.md)). Order and detail of the
client's own code: [kernel-map.md](kernel-map.md) §6 and, for step 9's dependencies,
[kernel-map-step9.md](kernel-map-step9.md). Adding a file: [workflow.md](workflow.md) §2.1. Every function
written but not byte-exact, with its cause: `docs/data/not-exact.md`; every abandoned attempt, its blocker and
LIMIT REACHED: `python tools/re/attempts.py list|show` (a parked row waits for a new §3.1 lever). The items
below name only the work items, the blocked work, the leads and the queues.

0. **GitHub (P-GH-10):** the claims and verify service are live on master; the rehearsal waits for the first
   outside contributor, then the public switch: [github-plan.md](github-plan.md) §4.5.7. Runs start with
   `python tools/run_branch.py start` (coordinator-pacing skill).

1. **Work items** (each at its file's next edit, unless it says otherwise):
   * Types: P-SM1-2 (OrbitalManager's ctor takes a shared callback base and ClientClass). strconv: D57's range and
     milestones.md still give the root object 0x4A3990 (it starts at 0x4A3B40).
   * Data and records: the .bss int Unknown_BE71D4 (in postate.cpp) moves to its owner, CLIENT_SET_TIME 0x737960's
     file or sclient 0x7472A0's, when 0x737960 is rebuilt; P-MV2-3 (anything
     added to duration.h re-sweeps auxpad.py).
   * From the 2026-10-31 t4 run (TK19): sclient's testeffects.ini loader 0x74CA90 NOT EXACT (inline budget 0.42:
     the client inlines the record's push_back; SC2 tried nine forms: the vector insert stays out of line); sclient's hash_map instance 0x756820 is still unnamed (no symbol) and unpaired.
   * From the 2026-10-31 t4 run (TK14): ui_avatr.cpp (19 pairs): the window's vtable 0xAEF9BC holds 0x554D60 at +0x60
     and 0x554E80 at +0x68, InterfaceClass's Unknown_60 / Unknown_68: declare them as those overrides if 0x554E80's
     functional rebuild (2026-11-04) does not, then rerun every marker; its functions toward exact: the POC creation item.
   * SClient's aux code (SCI, 2026-11-04): Player's aux tree compiles in sclient.cpp (D137 interim, SC1; auxroots.py
     SEND_HEADER_ROOTS, DEF_ORDER, aux_roots.cpp SYMBOL_PAD 58 (window 57..59), aux_roots_send.h SEND_SYMBOL_PAD 47 (window 46..49): each a narrow
     window, re-sweep with any header change, through the importer path: workflow.md 3.4).
     SClient::Init 0x741180 (inline budget 0.69; 0x758970 itself is exact now): the client calls RefCountPtr operator= 0x758970
     out of line in 0x74C9D0's else branch (stash docs/attempts/741180/); 0x755600 the same pattern (SC2 did not attempt either). Retyping PlayerVarAuxData loses 41 of cpacket.cpp's functions (aux_roots_send.h): dropped.
     delta_money.h (interim, D137, NAME 1).
   * The character info window (BIO / AVS2 / BF1, 2026-11-05): ui_cinfo.cpp's UICinfoClass (0x5734D0-0x5745AF, 12 exact;
     0x573E80 / 0x5741A0 NOT EXACT, register) makes four tabs: ui_bio.cpp's UIBioClass and 0x549640 (0x124, string
     ATTRIBUTES), 0x5C4170 (0xEC, MISSIONS), 0x58D080 (0xE4, FACTION_MENU), placed by TB1 (2026-11-05) as
     ui_attrb_skills.cpp (0x549640-0x54B75F, UIAttrbSkillsClass over UISkillsClass (ui_skills.cpp, SK1; was the interim skill base Unknown62D1F0Class)),
     ui_missn.cpp (0x5C4170-0x5C735F, UIMissnClass), ui_faction.cpp (0x58D080-0x58EB4F, UIFactionClass); round 2 by TB2 (+41). Left: attrb's
     Create_NEW copies 0x54B490 / 0x54B540 (Init 0x549AA0 calls them out of line; exact since AT1, but they are in none
     of the file's imports.json lists: probably byte-identical to other Create_NEW copies), 0x54A200 0.71 (register: ecx / edx
     swapped in the inlined 0x54B370), 0x54A640 0.68 (frame 0x8C vs 0x54: one NameClass slot, the font in the argument's
     slot); missn 0x5C5DB0 0.58 (register, late edi push), 0x5C66D0 0.77 (register: load order); faction 0x58DC20 0.65
     (register: reputation in esi, its slot +0x28 vs ebx / +0x14).
     ui_buff.cpp (0x5608D0-0x561DC0, on gd_buffgadg.h since BF1): 0x561030 parked (inline budget, ~29 forms: auxdata.h's
     find stays inline where the client calls 0x561C40 out of line; inline_depth(0) also outlines 0x560FA0 / operator[]),
     0x5615C0 partial (not a verified start; frame 0x74 vs 0x64; an early `continue` for an empty source_object outlines
     the scrub name's assign as the client does but scored worse). ui_cpit.cpp's UICpitGadget674260Class should switch to
     the interim unknown_674260.h (needs unknown_675120.h there): with the next ui_cpit builder. gd_buffgadg NOT EXACT: 0x667A60 / 0x667230 (one inline-budget
     cause: basic_string::assign out of line), 0x6661F0 (register), 0x665F20 (frame). Leads: `operator>` for
     duration.h (0x667950 / 0x6661F0 compare `.Time >`; re-sweep auxpad.py after, P-MV2-3); Unknown_671300Class's own header
     for ui_avatr / ui_avatar_cust / gd_buffgadg once its file is found (near gd_slot_data.cpp).
     ui_avsel 0x55F140 (register: the profession index, prof + race * 2 then + race).
   * ui_avatar_cust.cpp (UAC, 2026-11-04, on Unknown5BB320Class now): Init 0x54BD30 (slot +0x54, ui_avatr's ctor's twin:
     gadget loops, an inline AvatarDescClass copy, the palette helper inlined twice, `new MemberCallback`), key handler
     0x54CBC0 (slot +0x68, NOT EXACT 0.67: the inlined 0x54EAD0 copies call the converting ctor 0x551D40 out of line
     with the dtor inline, ours the reverse; try the getter chain inside the inlined copies); ui_avatr's key handler
     0x554E80 (0.27; ~18 dead reads reach 0.64, over D130); NOT EXACT 0x54F390 0.91 (shape: the client
     re-tests +0x64 after setting +0x08; even `volatile` doesn't make VC6 re-read it: the source structure differs), 0x557290 0.67 (VC6
     hoists the gender branches' shared opening), 0x54E950 0.75 (LIMIT REACHED) / 0x54EC30 register; 0x553AF0 (frame 0.61, parked),
     0x55BAC0 (inline-budget 0.28), 0x550A10 (register 0.71), Init 0x54BD30 (EH-state order; keep 0x551320's FAIL_IF
     lines 2210 / 2214 when inserting above it). ui_avatr.cpp's file-static Set_Rate (interim) is probably ShipPipClass's
     inline setter: move it into ship_pip.h with a full verify; the commented-out UIAvatrFlash block goes in a neutral
     pass that absorbs the line shift (0x55C280's FAIL_IF lines sit below it). The EventCallbackClass + Remove lever for
     dtors should fit the starbase windows' 0x6186A0 / 0x622B90.
   * The network stack (NET, 2026-11-04; cordmgr Pump_Releases 0x859940 exact with E&B's INFMSG): cordcon pumpInternal
     0x853A30 0.76 (the resend copy's EH state 9 vs the client's 10: one more numbered object between the full-datagram
     Write and the copy), handlePacket 0x851AE0 0.31 (frame 0xF8 vs 0x100, Incoming scans through getPointer), cordmgr Pump
     0x857F20 0.30; read 0x850FD0 0.71 (the second Ready_.erase out of line), ACK2 0x85ABB0 0.58 / Pump_Connections
     0x858A60 0.74 (frame: declaration order does not move it), Go_Away 0x8586D0 0.65 (lport / lip in dead parameter slots),
     Copy 0x84E260; register lotteries write 0x850C70, examinePacket 0x8514D0, buildHeader 0x8551F0, ACK1 0x85A430, openc
     0x84FF70 (three LIMIT REACHED). P-FB1-2 (pumpConnections 0x8BE270, LIMIT REACHED: try UDP's thin-forwarder lever from Send_Data
     6e78f015 before a third round). P-NW6-1: the
     remaining call-site NULL forms (Unlisten, ~CordMgr, Connect) are redundant; leave them until a neutral pass.
   * From the 2026-11-01 night run (login / character select; LG1-LG3): ui_login's ctor 0x5ACB80 0.88 (order,
     LIMIT REACHED); ui_avsel 0x55E070 (register), 0x55D060 (blocked: calls tsk_login.h's Get_Selected /
     Get_Avatar_Descs 0x55D860 / 0x55D880 out of line; case 7's FE01C tail merged into case 8's), 0x55F140 (parked; its
     iterator copies 0x560090 / 0x5600B0 come out of line only with it; the rest up to ui_bio's 0x5603E0 is exact); tsk_login (CS10, 2026-11-05): Go_Next_Step 0x769500 / Go_Back_Step 0x769680 have one residue
     each, the client's `push ebx; mov ebx,1` before the jump table (parked: 8 local / flag forms fail; needs a lever proven
     elsewhere first); Fetch_EA_Ticket
     0x76A7B0 (shape: ours tail-duplicates `ok=false; pop ebp`, 2 rows), 0x7698E0 (register: Major in edx vs eax),
     Unknown_50 0x7683F0 (register: the client recomputes the slot index); LIMIT REACHED 0x767F50 / 0x769E70; Cycle_Focus 0x443270 is probably
     `Set_WW3D_Load_On_Demand(true)` (+0x4074). cpacket.cpp / sclient.cpp include galaxy_map_client.h at their next
     edit (their partial GalaxyMapClientClass declarations).
   * From the 2026-11-01 night run (html, galaxy map; HV2, GM2): htmlview's parser 0x7710D0 (18 KB, assert line
     926, gap left for it; its template copies 0x778CF0-0x779120; frame 0x298 vs the client's 0x284 since D185's bef21977:
     find the inlined copies adding 0x14, then fix line 469's stale "0x27C" comment), Hit_Test 0x7772C0 0.77 (order) and
     Draw_Item 0x7767A0 0.72 (shrink-wrap, workflow.md §3.1.2) both at the D89 limit, Draw_Line_32 0x7778E0 parked (stash); 0x76EB70 (the font cache) is 0x76E9D0's object's; 0x77B760 /
     0x77B7D0 probably avatar.cpp's. galaxy_map's four iterator Next (0.75 register), galaxy_map_client 0x7A52F0 /
     0x7A5400; 0x7A8B70 (the galaxy map's renderer) walks the iterators.
   * ui_attrb.cpp (AT2, 2026-11-06 night; the client's own file, object 0x546A80-0x54962F): the attributes panel's NOT EXACT
     0x547C20 (layout 0.61: the client leaves the valid branch's null result unmerged) and 0x547FD0 (inline budget 0.48:
     only the valid branch's Cast out of line); the skill panel's 0x548D60 (0.73, register in the title's colour stores,
     9 forms).
   * optdlg.cpp (OD1 / OD2 / LV2; the client's file, WinMain's stand-ins 0x846C90 / 0x847D20 stay, D69): NOT EXACT
     0x846FC0 (register 0.66: case 2 keeps Choices in edi, the client the dialog) and 0x847630 (shape 0.77, 30 forms:
     the client loads Type into ecx and the table into eax before the inlined 0x846CD0's first test); the page
     procedures 0x847BB0 / 0x847C40 (0.77) and the servers.ini page 0x8476C0 (0.78) are functional with the same
     first-test residue (~34 forms: it needs a new lever). 0x84BE60 (target_generator_summary, register 0.68): an inline operator|= gives the siblings'
     bytes (0x4F58A0 / 0x835390 / 0x83D290), not its ebx form.
   * ui_starbase.cpp: UIStarbaseClass::Unknown_54 0x634140 NOT EXACT 0.55 (SU1: frame 0x1D0 vs ours 0x1A0, every later stack
     offset differs; the first entry's `std::string("")` is out of line in the client; zero in ebx vs eax): try one RefCountPtr
     per Create_Get / Create_NEW block and three char[32] name buffers. Unrebuilt callees: 0x644F80 (+0x1F8's 0xA4 class),
     0x6B6050 (+0xB8's gadget), 0x6976C0 / 0x636520 / 0x698150 / 0x6980E0 (the +0xB4 NPC slot and terminal lookups).
   * ui_analyze.cpp (AN1, 2026-11-06 night; UIAnalyzeClass 0x11C over Unknown5BB320Class, ui_manu.cpp's near-copy; ui_analyze.h
     interim NAME 1): slots +0x54 0x5439F0 (register 0.80) / +0x60 0x5443F0, 0x544D80, 0x545190, 0x545610, 0x5458B0 functional (AN2;
     inline budget: the client calls the finds / Casts out of line); Validity (0xC41640) and Mode (0xC41634) are one
     field type each, shared with ui_manu (0x5468D0 is called from ui_manu's 0x5B1230); NOT EXACT
     0x545610 (0.58: both Casts inline), 0x5443F0 (0.27), dtor 0x5437C0 (one relocation target differs). The file-wide question, why the client calls
     Find_Field / Cast out of line, also covers ~10 of ui_manu's NOT EXACT functions.
   * ui_playeropt.cpp (CP1, 2026-11-05 night; UIPlayerOptClass 0x114, ui_friends' twin, 0x5D8C40-0x5DAE8F): NOT EXACT only
     0x5D9D10 (inline budget, 8 forms). Its levers for ui_friends.cpp: 0x590E50 with `X.Peek() != NULL && X->Is_Active()`,
     0x591A30 taking its target from its own 0x592150 (as 0x5DAB50), perhaps 0x58FBA0 by calling the file's own helper.
     clienttest.cpp:1857's stand-in UIChatPanelUnknownD8Class::Unknown_5DA600 now has a real definition (no clash): retire
     it with the next tests/ edit. Next object ui_popup_browser.cpp 0x5DAE90.
   * avatar_pip.cpp (AP1, 2026-11-05 night; AvatarPipClass 0x3C, 0x7E31B0-0x7E49AF): NOT EXACT 0x7E4520 (shape: the client
     converts `* 0.1f` before Get_Sort_Level), 0x7E3A20 0.83 (shape: the light translation folded into zeroed floats, a release
     tail-merge, Rotate_Y's operand order); the model carrier Unknown47FA70Class (ctor 0x47FA70, table 0xAE9D88; ro_proxy.cpp
     by the triage, not in the tree).
   * ui_ptrade.cpp (PT1 / PT2, 2026-11-05 night; UIPTradeClass in sclient.h, interim D137, base Unknown5BB320Class, 0xE8): NOT
     EXACT only 0x5E2CE0 (0.84, register: key 3 reads the client through edx, the client eax; 7 forms). 0x5E1B80 / 0x5E1B90 are the previous file's.
     money.cpp's operators 0x4F0500 / 0x4F0700 / 0x4F0740 are stubs in clienttest / clientpoc until money.cpp links there.
   * Chat panel stand-ins (CP2, 2026-11-10): cpacket.cpp keeps UIChatPanelUnknownF8Class / F890Class and UIGroupFindClass for
     FIND_MEMBER 83 (no ui_groupfind file; 0x59A400 unwritten); ui_chat_panel.h's UIChatPanelWindowClass (+0xF8 / +0x104) is
     really ChatPanelUIBase: retyping it lets slot +0x90 be called without the cast (every `(UIChatPanelWindowClass *)new` in
     ui_chat_panel.cpp and POC_WINDOW_CTOR). ChatPanelInterface's ~ 0x562410, 0x565F60, 0x569A00, 0x56C7E0 are still POC gen
     stand-ins (ui_chat_panel.cpp lacks them).
   * ui_friends.cpp (FR1 / FR2; UIFriendsClass 0x14C over UIChatPanelBaseClass, 0x58F0F0-0x5921F0): NOT EXACT dtor 0x58F450
     (inline budget: ours inlines 0x5906C0 inside the inlined slot +0x5C, the client calls it; 11 forms; 12 dead reads push it
     out but lose 0x592300: try a deeper inline level for 0x5906C0's delete), 0x58FBA0 (the inlined player dtor's EH state
     between its two strings: client 2, ours 1), 0x5908B0 (register, 6 loop forms), 0x5913E0 (inline budget). cpacket.cpp's
     stand-in `class UIFriendsClass` waits for its stand-in ChatPanelInterface to go (ui_friends.h brings the real one).
     ui_friends.h interim (NAME 1).
   * The item examination window (IE1 / IE2, 2026-11-05 night; ui_item_examination*.cpp, 0x531430-0x53AB4F): NOT EXACT the
     description ctor 0x534CA0 (frame 0x2C vs 0x24; a named Vector2 gets the frame but inlines the second string's `_Eos`),
     the attributes ctor 0x532BA0 / dtor 0x533000 forms in attempts.csv; title 0x53A140 (the name's std::string by value);
     0x5378C0 (UIItemExaminationPropertyRouterClass's inline ctor, called by Unknown_94: probably a D99 short row).
     dynamic_window_toggle.cpp's ctor 0x51DEB0 NOT EXACT 0.47 (inline budget: the client calls RefCountPtr's copy 0x51E950
     out of line inside the StateRenderObjects vector copy). ui_item_examination.cpp's
     local Unknown537830Class / LineDisplayClientClass onto the real headers in a line-neutral edit. Next object 0x53AA30.
     Interim names (D137): ui_item_examination_attributes / _description / _specials.cpp; declaration-only ui_slot_info.h /
     ui_dynamicwindow.h / dynamic_window_icon.h wait for their files.
   * The expression library (app/shared/expr/, EX1-EX4, 2026-11-05 night): NOT EXACT expr_math_parser_impl Add_Similar 0x4D0DF0
     (register), Parse_Level 0x4D10E0 (inline budget: a token scope closing before the returns, untried); expr_math_float
     0x4C8920 / 0x4C9010 (frame: no EH frame for `new` in float's copy, int's exact),
     Create_Unary 0x4CC120 (inline budget), Visit_Binary 0x4CB260 / 0x4CFB00 (shape: a merged tail, 28 calls vs 24);
     std::fill 0x4D3CF0 / copy_backward 0x4D3EC0 parked (npos as an immediate -1); 0x4C7B70's guarded initialiser wants a D99
     row. Lead (tools): a vtable-slot pairing for the operator classes' byte-identical dtors / deleting dtors (~60 addresses
     in 0x4C9EC0-0x4CAC60 and 0x4CE2A0-0x4CF500, "multiple" today). Interim names (D137): expr_lexer_matchers.cpp,
     expr_math_token.cpp, the expr_math_float / int headers.
   * **From the 2026-11-09 day run (CH1, CW1, EB1):** the unwritten callers of vector<string>::erase 0x4FC7C0, to start
     from EB1's rule (workflow.md §3.1): the string-split helper 0x4FC2F0 (lego_identifier), ui_chat_window's dtor
     0x562440, ui_combobox 0x578D50, sclient 0x74DFF0; other sorted-insert loops with `it++` (ui_guild's member lists,
     ui_friends) may want `++it`; chat window near misses 0x5E85F0
     (basic_string::erase out of line in operator='s self-assign branch), 0x5E5360 (cases 5 / 6 expansion and frame);
     UIChatPanelWindowClass retired for ChatPanelUIBase (both hold the int at +0x98; the client has one Create_Get
     0x56CCB0 for the window base): removes ui_chat_panel.cpp's casts, renames 0x56CCB0 and pdaint's 0x694AA0, the five
     remaining stand-ins (5DAE90, 5CE240, 5CE700, 5CEDD0, 5CA010: no headers yet) pad from 0xA0; a ui_chat_panel.h edit,
     so with the sclient pad sweep (0x74D990 / 0x758210 / 0x702240) and the POC's POC_WINDOW_CTOR (SW1); cpacket
     0x73CC90 (0.73, 328 vs 327 instructions: the FAIL_IF and !panel returns keep their own profile tails, CP2 / FU1's
     shape), 0x73B150 (0.42, the range ctor 0x73F680 out of line); 0x569A00 (0x5C8DB0's caller) unwritten; 0x5EB2F0 (0.51, inline-budget:
     the client's three std::string(const char *) temporaries call _Tidy(false) out of line and inline assign(s, n));
     symbols.json's name for the thunk 0x4105DC ('DynamicScreenMeshClass::Set_Aspect') is wrong: it is
     ChatPanelInterface::Unknown_565F60's.
   * **From the 2026-11-09 day run (RF1):** sbaseview.h's SBaseViewUnknown184Class -> `class
     UIRefineryClass;` with the SEND_SYMBOL_PAD sweep in the same change (a plain forward declaration gained 0x702240 and
     lost 0x758210), then drop sbaseview.cpp case 5's cast; ui_refinery near misses 0x5EDCC0 (0.77, inline-budget: the
     'NegotiatedCost' Cast out of line), 0x5ECEF0 (0.76, register / flag-test order), 0x5EDFD0 (0.38, a probe reached
     0.48), 0x5EE460 (0.34, register, ui_manu 0x5B5CD0's residue).
   * **From the 2026-11-09 day run (UI1):** the rest of ui_inv.cpp: 0x59E970-0x59F2C0, three window classes over
     UIInvClass (vtables 0xAF2414 / 0xAF2644, a `vector<RenderObjClass*>` at +0xD4, a shared inline Add at line 108 with
     the '!robj' FAIL_IF, slot +0x54 0x59F2C0) and their vector helpers 0x5A0FC0-0x5A1060; near misses 0x59FE30 (0.62,
     register), 0x5A0740 (0.37, shape: the client's returns share one `mov al,1` exit; a form stashed), 0x5A0D00 (0.58,
     register).
   * **From the 2026-11-09 day run (OP1):** the options' next page from 0x5CA010 (0x1B0 bytes, request 8), after
     ui_option_check.cpp; ui_chat_panel.cpp's local Unknown5C8870Class / Unknown5C9910Class onto ui_option_account.h /
     ui_option_check.h (ctor (client, page, mode)), and 0x5C8DB0's caller 0x569B5A (in 0x569A00) onto
     ui_option_channel.h; ui_chat_panel's ChatChannelStruct initialiser 0x568700 (reads the channel-name globals
     0xBA8C20-0xBA8C50); ui_chat_panel 0x56B700's NOT EXACT may be request 6's fields (the client: channel in Unknown24,
     " " in Unknown34; ours Unknown14 / Unknown24).
   * **From the 2026-11-09 day run (MT1, FN4):** ui_mount Build 0x5C77A0 (0.53, inline-budget: rtti.h's Cast out of line in the client,
     frame 0x164 vs 0x1A8) and the uncalled lookup 0x5C8710 (0.58, order); fontsys 0x6CCE60 (Update_Text's countdowns,
     0.48, no attempts yet).
   * **Font types:** Font32Class's XSpacing / MinSpacing reachable; unknown_6ca0c0.cpp and unknown_6cce60.cpp merged
     when evidence names the object (D137); FontSys::Font +0x24C as `RefCountPtr<Font32Class>` (0x588DC0 passes
     its address as one); sclient.h's `SClientFontDatabaseClass::Unknown_6C52B0(int)` is
     `FontStringFilesClass::Load(const char *)`. FontColorStruct / FontEffectStruct stay in their .cpp files (in
     fontdatabase.h they cost Load_Init its match).
   * **The POC client (from the 2026-11-01 day run; queue `.work/poc/ledger.md`, run log `.work/poc/runs.md`):** build with
     `build.py build --lib app --allow-dirty`, mkauthlogin, `build.py link --program clientpoc`, run `clientpoc.py run
     --seconds 40 --shots 60000,120000` (the client's own back buffer, PNGs in `.work/poc/shots/`; `git add -N` new files
     first; build in a `wt.py` worktree when others' edits are in the main tree: workflow.md §3.0). State (run
     2026-11-02 agy night): the mouse and keyboard log in (`clientpoc.py --input FILE` with poc_input.cpp's virtual
     cursor; against antprox /REPLAY, allowed unattended by D181, with `.work/coord/replay.py INPUT SECONDS SHOTS`); the
     29 combo_ffactory.cpp(101) FAIL_IF lines are retail behaviour. Next, in order: avatar.cpp's functional NOT EXACT functions toward exact (RefCountPtr members +0x2B0..+0x2E4 /
     +0xBC as the client has them; 0x782200 / 0x7826A0 / 0x782850); the dtor 0x77C080's queue release if AVX did not
     land it; gen globals the client starts at 1 (0xBB9654, 0xBBB084) and TextureLoader's 0xC01290; the /O2
     constant-in-ebx residue blocking 0x769500 / 0x769680 / 0x76A370 (.work/wt/lgx/.work/lgx/find_ebx1.py); the
     the in-game users of the movement setters (ui_starbase 0x63A100);
     never the 0x847D20 / 0x846C90 dialog stand-ins: they keep the POC free of modal dialogs);
     ui_registration.cpp's rest: 0x5FA900 (make 0x5FB330 / 0x5FB540 / 0x5FB750 RegistrationInterface members first),
     0x5F8FD0 (INI load), 0x5FA0B0, 0x5FA5F0, 0x5FBD30 and the reply handler 0x600000 (~14 KB each, 0x600000 shifts every later __LINE__ in the file: a session of its own, XR2; the getters 0x5FFFC0 /
     0x5FFFE0 pair with 0x5FBD30), functional toward exact 0x5FB9F0 / 0x5FB0C0 / 0x5F9460 (attempts.csv rows),
     ro_funcs 0x4967F0 (frame) / 0x496960,
     unknown_847d20 0x847FE0 (inline-budget);
     ENBAssetManager (enbassetmgr.cpp: EA published it, `C:\EnBDevelopment\EA\CnC_Renegade\Code\ww3d2\hueshift\
     enbassetmgr.cpp`): Create_Render_Obj 0x48B220 to exact (NOT EXACT shape: 40 calls vs the client's 35, a merged tail),
     Recolor_Texture 0x48CB40 (register), 0x48ECA0 (EH states); the ledger's queue: BINK (E&B's
     BINKMovie gaps 0xA161B0-0xA17330), SORT2-3, LD2 (shipcust_desc), DW's icon / move / toggle, operator new 0x4E9CB0.
     Leads by file: dbgmgr End_Step 0x50A1E0 (a dead store our compiler drops), ctrngadg 0x50ABC0 (block layout), tooltip
     0x6AB5B0 / 0x6AB930 / 0x6ABC60, textgadg 0x6AAD10, gclass 0x4A17B0, filter match
     0x4BE2C0 (register), udpconn's constructor, mainview 0x514B00 (needs a duration.h member) / 0x514BD0 (caller 0x512400),
     clientpoc.py's docstring: ENB_POC_WINDOWED /
     --fullscreen no longer act (the real 0x442AC0 reads RenderDeviceWindowed from the registry, read-only). Stand-ins to
     delete as code lands: programs\clientpoc\poc_standins.cpp, poc_link.cpp, poc_ui.cpp, poc_data.cpp (its flag constants
     0xB08CAC-0xB08CC4 want their owner).
   * From the 2026-11-01 day run (leftovers by file; attempts.csv rows for each): cordcon read 0x850FD0 (m's home in k's
     slot), handlePacket / pumpInternal (match the timebufs' frame offsets +0xE8 / +0xC0 first), openc / write (register);
     cordmgr Pump / Go_Away / ACK2, cordmgr's INFMSG 0x859940 can use wdebug.h's 40-byte timebuf; gadget 0x6609A0;
     gclass 0x4A17B0 (order); font32bit Make_Proportional 0x6BC120; fontsound 0x6BCFB0 / 0x6BD060; fontsys
     Render 0x6D8270 (compare order + floor / scale form), 0x6D47C0 (EH state), 0x6D5170, Change_Text 0x6CDBC0 / 0x6CDAB0
     (functional); stredit Process_Key 0x84AAD0; rect3d ctor 0x6E2290 (its D185 variant needs a helper and loses
     0x6B71E0 / 0x6D80A0); dynamic windows' Set_Alignment; ui_avsel 0x55F140 (erase inline on the FAIL_IF path), 0x55E070
     (register); htmlview Parse exact (frame: 16 Wstrings, 4 attribute stacks; `.work/HV3`) and a harness test for it.
     Files next to today's: gd_proxy.cpp (settles unknown_66b770.cpp), 0x66C8F0-0x66C9A0 (a 640x480 gadget,
     vtable 0xAFAB64), interface_scene's 0x6DAD40. Header merges: gadget.h's GadgetUnknown08Struct and ui_scut's local GadgetClass are RObjGadgetClass;
     ui_manu / ui_group / ui_scut button stand-ins onto unknown_675120.h (P-BTN-1; ui_manu / ui_group first need gadget.h);
     ui_avsel's UIAvselEditClass onto textgadg.h; P-UI-2 (intrface.h's __INTRFACE_MEMBERS guard); P-MC-2 / P-LINK-2 (the font
     globals 0xBE67B8 / BC / C4 and Unknown_496110 one type each).
   * gd_slider.cpp (CS14 / CS16 / GS1, 2026-11-05; the class screen's skill slider, all 19 exact). ui_combobox.cpp's stand-in GadgetClass -> the real gadget.h (then gd_slider.h can be included; the rows and the
     text field +0x5C bit 4 / +0x61 / +0x68 FontSys * belong to an unidentified derived gadget); ui_manu.cpp holds the
     same kind of stand-in.
   * ui_ship_cust.cpp (CS21 / SC1, 2026-11-05: the starbase's buy-ship window StarbaseShipCustomizeInterface, RTTI 0xB89BB0,
     declared in ui_ship_cust.h; ctor 0x6181B0 / dtor 0x6186A0 exact; 28 exact): NOT EXACT (attempts rows): slot +0x54
     0x618CA0 0.73 (SC2: every call matches; registers left: Selected_Decal via [ebp+0xF0] where the client reuses ebx,
     eax / ecx / edx swaps), 0x619470 0.49 (lead: write each of the 26 cases out, as the client's jump table has them),
     0x61A4D0 0.76 (the client calls _Tree::begin 0x61FBE0 out of line; routing through 0x61BC50 does it but puts _Inc
     out of line and loses 0x6209D0; tail registers), 0x61BD80 0.64 (lookup mix: the client's operator[] / map::insert /
     _Tree::insert / pair ctor out of line 4 / 6 / 9 / 4, ours 8 / 8 / 3 / 0), 0x61F250 0.78 (register: the inlined
     0x61AD30's hull / wing loops),
     0x61B170 0.89 (register: Page's load / `mov ebx, 1` scheduled into the u arithmetic, 7 more forms; the unreferenced
     0x61B0F0 is exactly its Page == 1 body), 0x61DF80 0.47 (0.60 stashed: the client copies block 1's Matrix3D inline,
     blocks 2 / 3 through ??_H), 0x61E8B0 0.62 (LIMIT REACHED), 0x61E8E0 0.42 (0.64 stashed; 0x50C vs 0x4AC: the client
     inlines one string release); the part's operator= 0x619320
     emitted, not exact (its loop pointer starts at +0x08, the client's at +0x18).
   * POC character creation (the 2026-11-04 night run; full-flow input `.work/run-2026-11-04/in_full.txt`, coordinator
     check `.work/coord/pocfull.sh`): the avatar screen's 0x55BAC0 (0.28, inline budget; a 2-6 dead-read sweep peaked at 0.42); swatch (negative) and secret decals untested; the functional
     NOT EXACT functions toward exact: decal Load 0x75BE00 (0.46, shape: the client spills &Unknown30 ([esp+0x24], reloaded
     into edi each swatch iteration) and &SwatchArray where ours spills `this`; the swatch section looks like an inlined
     helper taking both by reference, on the asserts' lines 132-144); Load's two base materials use Create_NEW for 0x75CFA0
     (same body as Create_Get 0x47D040; the name is a guess until another caller of 0x75CFA0 is read: proj_decal_manager
     0x7633E0, galaxy map 0x7B2B30); decal.cpp's 0x82EF70
     stays interim (it sits near udpconn / chartemplate); ui_shipcust's 7 (CS3 / CS7 / CS9, 2026-11-05; attempts rows for each): 0x627200 0.86 (LIMIT REACHED;
     by-value signatures landed 5e63a2c2: frame 0x30 vs 0x2C, ebp pushed per case, case 0's Unknown054 test branches the
     other way), 0x624E50 0.68;
     0x623080 (inline budget, over D130 only), ctor 0x622180 0.37 (the client calls ShipCustPartClass's operator= 0x619320
     out of line), 0x625A60 0.65, 0x624270 0.79 (SC3's forms; LoginTask's two vtable calls not tail-merged, tail registers), 0x621390
     0.65 (register; push_back drops 0x8810F0-0x881250's duplicates, still held by pknamedecal: D66 (i)); LIMIT REACHED
     0x621530, 0x624E50),
     ui_shard's 0x616220 (register: the inlined 0x617660's eax vs ecx), shiputils 0x80EAF0 0.38 (asserts on client lines
     41 / 45 / 59: lay the file out first), AvatarCreationInterface 0x553AF0 / 0x554E80, ui_class's planet turn 0x576AC0 (prologue registers only, Get_Frame_Time's: masked
     98 -> 28, form in docs/attempts/576AC0/) and ctor 0x574710 (masked 313 -> 28 but with ~40 dead reads, over D130; frame 0x34 matched: the +0x134 / +0x138
     store order and the first dialog string's inline `_Eos`; docs/attempts/574710/), ShipPip (PIP, 2026-11-04; CS5 2026-11-05) 0x80CE10
     0.84 (7 bytes: one `mov ecx, edi` in the inlined Translate(-center), docs/attempts/80CE10/; no other exact
     client copy of the inlined Translate(-v) exists to compare with), 0x80D5E0 0.65 (the inlined
     Unknown_80E7D0 returns in eax, the client's in ecx; likely 0x80CE10's blocker: wwmath's Matrix3D::Translate inline body),
     0x80CA30 0.62 (frame 0x48 vs 0x44: the client spills the clamp's max, keeps min on the FPU), 0x80C640 0.50 (frame 0xA0 vs 0x74: the client copies Set(Vector3)'s components
     through stack temps; four Set / Matrix3D-temp forms compile identically), avatar.cpp (CS12 / CS15, 2026-11-05: the file follows the client's address order,
     padded to its `__LINE__`s; re-pad after edits with `.work/cs12/reorder.py SRC OUT`): every member the client holds
     as a RefCountPtr is one now (AR1); AR2 / AR3 (2026-11-05): all but 20 of the 138 functions from 0x77B870 exact
     (ctor 0x77B870, dtor 0x77C080, the LOD overrides; PlayerNLPClass's ctor / dtor 0x78EB50 / 0x78EB80 out of line in
     nlp.cpp). Interim (D137): 0x782FF0's tail reads Unknown0C0 through a raw `HAnimClass **`; avatar.h's free inline
     `Unknown_Block_Value` (NAME 1, inferred from 0x784700's inline-site count). Left (AR4 made the copy ctor 0x77C8F0 and 0x783620 exact): 0x786410 0.94 (register: the client's eax / ecx / edx
     rotation fits the else branch generated first but placed second), 0x787450 0.92 (order: y*y / z*z in the second
     Normalize's first Length2), 0x782200 0.69, 0x780AF0 / 0x782FF0 (LIMIT REACHED); functional for the POC: 0x780810, 0x781EA0, 0x783B50,
     0x788900, 0x788F90; 0x7817E0 (a 7-byte getter of +0xE8) not built. `.work/cs12/reorder.py` uses only the client's
     first N lines when the tree has fewer anchors (AR3's rule, folded in 2026-11-05); the
     teleport loader 0x788F90 functional (NOT EXACT); ctor / dtor; ship_pip's `Unknown_Zoom` is a provisional file-static helper (NAME 1); the server's answer to GLOBAL_CREATE_CHARACTER (LoginTask +0x20 approved,
     +0x1090 errors 8-13) once antprox /REPLAY answers opcode 0x0072 (it logs it as ignored). Interim placements (D137):
     unknown_585920.cpp (0x585920 .. 0x585D10, 0x5860B0, 0x671300 / 0x6714F0 / 0x671570 / 0x671630; all exact since
     2026-11-05), decal.cpp's 0x82EF70: revisit when objmatch
     places them.
   * ui_chat_panel / ui_manu leads (CGP, 2026-11-04): 0x56B700 register only (178 = 178), 0x5AFFC0
     frame only (0xAC vs 0x9C), inline budget 0x567DF0 / 0x567760 / 0x563280 / 0x565390, register 0x5630B0 / 0x5AFCD0 /
     0x5B4850, order 0x5AFA00; 0x563910 calls AvatarPipUnknown2CClass::Unknown_77E3D0, the client's symbol is
     CharacterViewAvatarClass's (a naming fix).
   * Tools (from the 2026-10-31 t4 run, TK12, still open): claims.py resolve / check should count attempts.csv
     `exact` (D99) rows as done (mainview's #111 was handed out though LD4 finished it; on 2026-11-01 its per-file
     counts still include the night's D99 rows); LD4's D99 rows 0x514D30 / 0x514D50 / 0x514D70 carry stale names
     (As_Main_View, UIViewClass::Unknown_44 / _58) at the mainview rows' next touch.
   * From the 2026-10-31 t4 run (TK9, TK18): landdmap 0x468920 (the cell builder, NOT EXACT, frame 0x8C vs 0x90 after
     TK18's 22 forms: the client keeps X on the FPU and Y in a spare slot for the upper point, perhaps one inline
     building both points from floats by value); 0x468760 0.46 (generators in docs/attempts/468760/).
     0x466920-0x466BC0 is its own object between heightfield and landdmap (an unnamed Line3DClass subclass, member at
     +0x108, vtable 0xAE8754, no RTTI; ctor called from 0x7B99C0 / 0x7C3AC0): a new unknown_*.cpp when built.
     heightfield's Get_Height_2 / Set_Height_2 0x463CD0 / 0x463D90 are twins of 0x463C80 / 0x463D20 called from the land
     code: ambiguous, no tie-break reaches them.
   * From the 2026-10-31 t4 run (TK6, TK11): ui_avatr.cpp (new, AvatarCreationInterface) is ui_avatar_cust.cpp's twin:
     leads 0x553AF0 (ctor), 0x554E80, 0x556F80 onward, and ui_avatar_cust's 0x54B370-0x550670 (a function rebuilt in
     one is the template for the other); 0x55C350 sits at ui_avatr.cpp's end (interim, D137: in the client after line
     2207). ui_avatr 0x556B20 NOT EXACT 0.54 (inline budget; more dead spenders than D130's 6: left parked).
     scriptedmovie's 0x8099B0 (register 0.82) and Create_Object 0x804290 (0.58) block five vector copies.
   * From the 2026-10-31 t4 run (TK5): lookup_table's hash_map() 0x4EE810 and 12 other out-of-line hash_map() copies
     go exact with _M_initialize_buckets written out in sgistl's three-argument hashtable ctor (sgistl_adapt.py;
     `.work/tk5/sgistl_g.patch`: +19 and +11 tie-break) but lose 66 (the hashtable(n,hf,eql) copies inline into their
     class ctors; `.work/tk5/cmp_g.log`): a lead, not landable. delta_money 0x4A7120 / 0x4A71A0 exact as
     delta_template.h members in a draft but cost aux_roots 6 (workflow.md §3.4).
   * sbaseview.cpp (SBV / SB2 / SB3 / SB4, 2026-11-05; Init 0x69B5E0, the switcher 0x69F8F0, 0x69F710 exact since SB4): NOT
     EXACT, parked with attempts rows: the lights loader 0x6A0590 0.72 (frame; lead: a file-static rotate writing Z, Y, X
     cut rows 119 -> 92; the client puts `up` at +0x34 and a scalar at +0x40, ours the other way round), 0x69D030 0.87
     (literal: Rotate_Z's row 0 still does s*tmp2 first); 0x6A1180 not started (the 0x700-byte debug memory display on
     FastAllocator internals; frameint.cpp's 0x53EB00 is the same code). Interim (D137): UITalkTreeClass (0x64B6E0, 0x180) declared in
     sclient.h until ui_talkt.cpp / .h is built; +0x184 / +0x188 / +0x198's neutral stand-ins.
     unknown_490900.h (SBaseViewUnknown68Class = ScriptedMovieViewerSettingsStruct, +0x00 the scene LightClass) is interim
     (D137): revisit when 0x4909D0's file is found (its lights may be Vector4s, as 0x6A0590's are).
     ui_avatar_cust 0x550A10's frame-time prologue (register).
   * From the 2026-10-31 day run: aux_roots' colour 0xB00A00 is probably wwmath's Vector3 (its
     KVPair's Get_Type_Name 0x4B24F0 returns 'Vector3'; keep BLOB_NE's dword operator!=); free leftovers 0x709630 and
     0x7099F0 / 0x709A40 (rtti.h casts); D99 candidate 0x463440 (Unknown462690Class's dtor). Leads: ui_manu's money find out of line first in 0x5B4B10 /
     0x5B5320 (DeltaTemplateClass<MoneyClass>'s instantiation state in the client's TU); ui_group 0x5946D0's
     Action2PacketClass string `_Tidy` (case 35-39 or pkaction.h), then 0x593180 out of line; SClient::Shutdown
     0x741BB0 (implicit, inline-shaped dtors of SClientUnknown1334 / 1338 / ProjDecalManager); landdmap 0x466F60's
     spacing temporaries.
   * SY1 / chat panel (ChatPanelUIBase): +0x9C is a MovableUIBackGadget * and 0x56DEE0's slot takes
     (MovableUIBackGadgetMoved *, MovableUIBackGadget *) by the mangled callback type (our source: DestroyGadgetEvent *,
     GadgetClass *, an `int Unknown9C`); 0x56D970 connects it. The POC replay (WE1's input) crashes after world entry in
     ViewClass::Unknown_14+0x44 reading 0x88 (SClient::Do_Frame -> MainViewClass::Unknown_14), with the baseline exe
     too; replay the POC once for the chat panel's real handler registration (P-OM1-1). Batch b1108's leads
     (attempts.csv): 0x510D40 (frame: only the DynamicIBAccessClass slot differs), 0x5913E0 (inline the list clear, keep
     0x592350 out of line), 0x561030 (auxdata.h's Unknown_Find_Field<DeltaBuffsClass> out of line by the inline budget,
     not a forwarder), 0x788900 / 0x6A7730 (shape, 0.59), 0x6CE4E0, and RT1's 33 only-closer stashes; D185's helper-copy
     cases 0x44E470, 0x500760, 0x584690, 0x5B1ED0. 0x56C180's NOT EXACT note predates the SignalMap retype: re-check.
   * ui_registration.cpp (RG1): 0x5FBD30 (Next, 17 KB) and 0x600000 (reply handler, 14 KB): no exact prospect, write
     them only if the registration flow is wanted; the likely yield is the out-of-line STL copies at 0x604xxx-0x60Cxxx,
     which need the real 0xF4-byte entry type (dtor 0x449CC0). Sub14C's slots 0x7DFED0-0x7E15B0 are real code now
     (unknown_7dfed0.cpp, XR2); the 0xB08C90-range flags may read 0. Function starts for 0x5EED20-0x5EF070's `$E`
     initialisers would let them count (D99). 16 NOT EXACT (attempts.csv).
   * target_reticle / skill_render (TR1): Select_Next_Target's caller 0x59DB10 has no source in the tree yet. `Unknown_466920Class` (Line3DClass
     with a sort level, ctor 0x466920, table 0xAE8754) stays under a D137 mark in starmap_pip_support.h until its file
     is known.
   * money 0x4F0320 MoneyClass::Div (shape 0.99: the client keeps a dead store of 0 to q's low dword before __alldiv),
     ui_popup_browser's ctor, the camctrl stubs of states 3, 7, 9 (A5, functional). Leads: LegoClass::Unknown_3C to
     `const std::string &` at the next symbols.py refresh (WE8: loses 0x72CBF0 now); the enum-statics header 0xBE3B44 /
     0xC416A8 / 0xC41658 (0x68FAE0, 0x54B6F0; NM1).
   * radar_data / kv_encoder, not rebuilt (RD1, KV1): 0x4973E0, 0x4976B0, 0x497700 (RObj2Texture); 0x832010, 0x79B6B0
     declared only.
   * Functional, unplaced (WE1; from the near-miss pass): 0x561DD0, 0x56D510, 0x5629E0, 0x744690 (WE1's functional);
     unplaced: 0x8192C0, 0x819420 (after time_mgr.cpp), 0x5DCB50-0x5DCBE0 (a gauge object before ui_proc.cpp).
   * The triage plan's runs (D163; the log entry of 2026-10-31 15:40): `.work/triage/plan.md` (rerun `triage.py plan`
     after each run): 33 runs of 3+ neighbouring functions with no pair and one suggested file, 10 agreeing with both
     neighbours (the hint's file was right 23 of 23 in TG1 / TG2's six; the 2026-11-05 night run found it wrong for
     ui_item_examination's panels; on 2026-11-06 the file was right more often than its boundaries, see that entry), 16
     naming a file not in the tree; 2,735 template instances; 379 library
     kinds in the app range to check against gaps. Each run is a bounded builder item: claim the bare range
     (`run_branch.py claim 0xLO-0xHI`). The table is tracked in `docs/data/triage.*` (D172). The $200's
   * chat_window (CH2): `unknown_67d430.*` is interim (D137): no `__FILE__`. 
   * ui_registration.cpp: the reply handler 0x600000 (14 KB; every reply getter it calls exists; writing it shifts the
     `__LINE__` of every later assert: a session of its own).
   * ui_guild.cpp (provisional): 0x81C380 (the member list rebuilt from a vector of strings) and its string helper
     0x81D290 (calls 0x4FB810 / 0x4FC990).
   * client_item_property_builder.cpp: 0x522860 / 0x521C10 / 0x527F40 (NOT EXACT); ui_item_examination's router /
     builder stand-ins (need D218 carried out first: UISlotInfoItem = InvDataClass, UISlotInfoItemUnknown54Class =
     InvDataPeekClass in shared headers, ui_item_examination and inv_render onto them); D219: ItemTemplateInfoClass merged
     into ItemBaseInfoClass (pkitemba.h). client_item_property_attribute.h:
     the client's inline code the tree lacks (13 functions, 18 asserts, used by its includers); the visitor's slot +0x20
     takes a ClientItemPropertyClass * (the base's Accept passes `this`: retype with symbols_named.json `replaces` rows);
     0x51F8E0 (7 bytes, no callers, stores RefCountClass's vtable, between dynamic_window_vertical and the base: an
     out-of-line inline destructor copy of one of the two).
   * ui_jobs.cpp: 0x5A3FB0 (the client expands the job's delete and list::erase three inline levels down).
   * ui_trade.cpp: the insertion sort 0x653450 (by price or by name; inline-budget 0.23, stash docs/attempts/0x653450/:
     ours makes 41 calls against the client's 31, compare which before trying assign forms); the hash-set users
     0x652030 / 0x6522D0 (likely an SGI hash container local's dtor) / 0x652670; the ctor 0x650560, Init 0x650A40,
     0x651390, 0x651830; 0x651F60 (register 0.68: the client pushes a zeroed edx twice on the -1 call).
     ui_talkt.cpp's own UITradeClass stand-in (over Unknown5BB320Class) retypes onto ui_trade.h at its next edit.
   * ui_cpit.cpp: 0x584000's expansions in 0x5821C0 (needs F) and 0x5806E0's last two sites (the client keeps their
     finds / Casts out of line); 0x57D6C0.
   * The inline-budget census (any file): its `o` entries in small callers, lever (a) or (b) (workflow.md §3.1; lab paths
     under Tools).
   * fontsys.cpp: 0x6CE4E0 (Change_Text's store order, 0.58: operator+= out of line) and Update_Text 0x6D64C0 (a round of
     its own, Fable-eligible per D72: the client calls Convert_Color_Clamp and ShaderClass::Reset where ours calls
     Clamp_Color); 0x6D60C0 (spend a little budget between the two Play copies).
   * sclient.cpp: 0x74CA90 / 0x741180 / 0x755600 (above); unknown_7cef30.cpp: the request readers 0x7D6A60 / 0x7DAD50 /
     0x7DB2D0 (Parked leftovers).
   * **From the 2026-11-09b night run (GS1, ID1, SB1, TX1, SK1, CE1):** gen stand-ins still hit in the world (35 at GS1's
     close, 34 after UV1; list them with the POC's stubs.log `gen` lines against `client.py name`): 0x53DFB0 / 0x53E030,
     BridgeInterfaceClass, acticon 0x6766E0, kv_util 0x4BB3F0, MusicManager / 0x74BFB0, the chat colours 0x765540 / 0x765BE0 /
     0x765E40, TX1's 0x58B160, 0x810CF0, the 'if_targ' range gauge Unknown6A3D60Class (0x6A3D60 / 0x6A4020 / dtor 0x6A3F10,
     never rebuilt; its stubs are inert); ItemBaseInfoClass (pkitemba.h) and ItemTemplateInfoClass
     (client_item_property_builder.h) are one class (both declare 0x83F490): merge the names, then rebuild the ctor 0x83F490 (a
     gen stub: every item template's `new ItemBaseInfoClass` has a garbage table), 0x83DB00 and the dtor behind table 0xB08C34
     (SG1); the 0x42A410 key-binding writer's retype (ControlKeyComboStruct against KernelEventClass); Unknown_671300Class's
     second ctor argument as RenderObjClass * (UV1: ui_cpit passes `(int)uv`; ui_avatr / ui_avatar_cust's local declarations
     must change with it); the chat panel's
     missing tabs and dynamic_window_background.cpp(147)'s AnimationData assert; money.cpp 0x4F00C0 (a hand stub groups
     thousands in the POC) / 0x4FAEE0; SB1's next ui_starbase leads 0x63CA50 (needs the +0xCC ctor and operator[] 0x644A20),
     0x63E930 / 0x63F490, 0x640360, 0x6417D0; CE1's ClientChatRequestPacketClass 0x878D90; symbols.json misnames 0x58A640
     (it is UIDynamicWindowClass::Get_Center, SK1); antprox's replay does not echo CLIENT_CHAT_REQUEST 0x00A3 (antprox repo).

2. **Step 9 (D90)**, left in its files (history: the log entries of 2026-10-28 and 2026-10-29):
   * co.cpp: the thunks 0x70E2A0-0x70E2D0 (too short for verify to count); type 28's aim 0x70E850 (frame / shape);
     the base's slot +0x04 0x7251C0 (a pure placeholder) and its attach / detach rows; the damage state 0x6FE1E0;
     0x6FCB70, 0x6FEAC0, 0x6FC9A0, 0x6F3FA0, 0x6F4610, Process_AI 0x744260; 0x6FD310 parked for good (IB3).
     Leads: P-DF1-1 (the shared inline-budget cause: auxdata.h's Unknown_Find_Field against 0x7079B0's size), P-DF1-2
     (delta_array.h's bounds accessor stays co.cpp-local until the 0x6FB1E0 canary is understood), `Unknown_Find_Valid`
     and the inline-member form on 0x6F3FA0 / 0x6FBEF0 / Get_Name. SUBPARTS 180 is NOT_DRIVEN until clienttest's fake
     models get slot +0xF8 and an asset-manager stand-in (P-DM1-2).
   * postate.cpp: 0x718830 shape, 0x7179A0 / 0x717430 inline budget (a tryv2 sweep). extrap.cpp: its NOT EXACT rows,
     first lead the negation / flag form shared by 0x7102B0 and 0x710530.
   * px: parked Overlap 0x44E470 (the client inlines in reverse source order; lead: a nested helper for the enter
     half), On_Enter 0x44F5D0, On_Exit 0x44F6E0 (a kept `&it->second`); 0x454730 order (leads: an explicit
     Matrix3D::Transform_Vector + Matrix3::Multiply, or A's box built after B's); 0x453F10 (the client keeps self /
     other in ecx / edi from the start; lead: a per-box struct or named Get_*_Vector locals); 0x454590 is a {int,
     string, string} _Construct with callers in three other files, not a px function.
   * cpacket.cpp: ADVANCED 62 / SIMPLE 8 drift (one fix likely gains both: the drift divisor's CSE, the state pointer
     in ebp), CLIENT_CHAT_LIST 164 (each case's return repeats the profile tail; lead: a `break` into one exit),
     START 5 / STARBASE_AVATAR_CHANGE 158 / MESSAGE_CHATSTREAM 95 register, CLIENT_SET_TIME 52 shape, DAMAGE 100 frame.
   * sclient.cpp: P-T18-1 (a rebuilding commit deletes the member's hand-written clienttest stub: Render 0x744690, the
     chat commands 0x74ABC0, Print, Unknown_34 / _64, ...); P-T18-2 (gen_stubs: class-by-value returns and template
     parameters; unblocks 0x744C10 / 0x744D80); the destructor's `++it` loop once the UI base exists (P-T7-2); next
     lead the testeffects.ini loader 0x74CA90 that fills 0xBE7D80 and the 0x740050 clear (TestEffectStruct exists);
     0x750C20 is dead code (parked, P-SC3-1).
   * Parked after two rounds: Unknown_50, the fast start 0x76A370, Pre_Run, Run, Start_Game, Go_Next / Back_Step,
     SClient's Init / Shutdown. D92's Fable round: ClientClass's constructor (item 7).
   * Lead for the rest: the auto_inline lever on other NOT EXACT app functions that call a same-file function out of
     line.

3. **The packet classes:** 260 of 260 declared, about 255 rebuilt. Left: 45 and 117; 0x867250 register (LIMIT
   REACHED). Lead (F9): the shared-helper reading for
   the other app/packets files parked at inline budget whose ctors / Builds / Prints loop over containers.

4. **The SGI hash_map's users (D87):** client_lego_impl's Get_Lego_Transform 0x72B000 (Fable-eligible, P-H17-2) and
   Add 0x72B7F0 (one or two EH states off); control_keys_manager's P-H11-2
   pair and Load_Definitions 0x71B1F0 (Fable-eligible); inpdsp 0x429F60 (register); kv_list's 0x4B6B60 (why
   0x4B7030 is a one-caller find instance); fontmanager's 0x6D0A70 (inline budget); co_player_relationship and
   option.cpp after the eval.

5. **Land and the views (M2.4, D46):**
   * land.cpp: the big functions (LD7, 2026-11-05; best forms in docs/attempts/46F120/ld7_46F120_c3.cpp.txt and
     docs/attempts/470F80/ld7_470F80_carry.cpp.txt): 0x46F120 688 masked rows (left: the client keeps y + 1 in a frame
     slot per x step and y - 1 as that minus 2, and does all eight height fetches before the z differences; ~45 forms;
     untested: a real reference use, a helper taking (xp, yp)), 0x470F80 2042 (left: frame 0x7C vs 0x6C, the loops'
     pointer counters); Render 0x474B30 is a structural gap (the
     client's second dynamic VB / IB pass, inlined render-state debug names); Get_Downhill's case 2 / 3 tail merge;
     Get_Triangles 0x465D30 (its second push_back's end() load; Fable-eligible); the decoration appender 0x47C6A0
     (inline budget, 13 forms: Fable-eligible), 0x479D00 / 0x47C580 shape, 0x478B50 budget, Build 0x47A2F0 (parked;
     it can only gain itself, its helpers are `multiple`; its RefCountPtr form parked: the client calls insert(it, n,
     x) 0x47D690 out of line), Load_Assets 0x4691D0 frame, Update_Patches 0x46B3C0 shape; the stars' Init 0x4829E0
     register, Load 0x481DB0 order; landdmap 0x466780-0x469000's rest (Matrox only, never runs); the global byte 0xBFAE52 set during the prototype load is unknown.
   * mainview.cpp: ctor 0x50B360 order (try SB4's `Get_Client()` lever that made Init 0x50BA60 exact), 0x50D060 register, Update
     0x50F1B0 frame (DurationClass::operator+ out of line costs aux_roots' pad), targeting 0x512400 budget (the client
     calls its find copies 0x514B60 / 0x514BD0), picking 0x512BB0 shape, overlays 0x50D520 budget, sea 0x510D40
     shape, 0x5C25C0 shape (the client keeps * 0.01 and * scale apart), 0x514010 / 0x6B4480 shape,
     the three Render_*_Mode; 0x513CD0 / 0x513D20 are ambiguous twins.
   * The views: the loading view's 0x691830 (frame 0x548), 0x6912A0 / 0x691320 (the +0x58..+0x64 class, the font
     globals); the other views' constructors / destructors (mv3dview.cpp's ctor, 2.7 KB, and mv3dsnd.cpp);
     sbaseview.cpp (leads 0x6A0590, +0x184..+0x1A0 as interface RefCountPtrs; 0x69F710 order);
     scriptedmovie.cpp (the dtor 0x8008B0, the loader 0x8099B0, 0x8059A0 shape, 0x8020B0: its D185 variant needs a
     fake derived struct).
   * POC world entry (WE1's path; run `clientpoc.py replay --input .work/WE1/input.txt --seconds 140 --shots 1` with
     `D3D8SHOT_EVERY=2000 D3D8SHOT_MAX=500`; in-world frames from ~280000): the POC enters the sector and draws the
     backdrop, the orbitals, the player's ship as retail's, lit by the real lights, under the cockpit HUD with the radar
     panel (ui_scan.cpp). Open (retail ~170 draws per frame, the POC ~114 at WE9; WE7's `.work/we7/drawsum.py`): the
     chat window's split class's NOT EXACT set-up 0x5723B0 (register-only, 2 rows) and dtor 0x572040 (inline-budget;
     a 0.77 form with dead reads stashed);
     ChatWindowInterface's NOT EXACT (attempts.csv, CW2); the chat reply list 0x56E2A0 (fc_symbolchat.ini: likely
     ui_symbolchat.cpp) and the combo box's ctor 0x5789E0 / 0x5790C0 / 0x578210 / 0x579DC0 / 0x57A060 / 0x5689D0; the
     PDA's NOT EXACT 0x693E70, 0x695780, 0x695A00, 0x695FE0 and its windows' stand-ins (poc_world3.cpp: vault, experience,
     and PW1's D179 stand-ins under the cargo and equipment windows: DynamicWindowIconClass::Create 0x519A20, 0x51A9C0,
     0x66CC00, combo 0x5789E0 / 0x57B480, ship PIP 0x541DE0 / 0x66DA50, stats 0x6A2ED0; poc_world4.cpp: the star map
     set-up's class, GadgetPipSceneClass 0x66ACE0, GadgetStarmapClass 0x7B4700); in the world the chat panel's tabs are
     missing and the chat text colour differs from retail's (red against green: SClient +0x13CC, 0x765540 / 0x765BE0 /
     0x765E40), the cargo window shows no items (the replay's cargo is empty: a replay with items is the next check of item_template.cpp) and
     drag is untested; asserts every run: soundlib(838) x4, interface_static_sort_list(503) once, dynamic_window_background.cpp(147)
     AnimationData x4, Mounts inv_data_peek.cpp(104) on the equipment list; starmap_pip.cpp's rest (0x7C0440, 0x7C0620, 0x7C0910, 0x7C1200, 0x7C1C50, 0x7C2340,
     0x7C2850, 0x7C2DA0-0x7C30E0; NOT EXACT 0x7C0C80, 0x7C1770 (frame, 0.88: retry the slot order now that +0x68 is a
     Vector3), 0x7C2060, 0x7C3830) and sclient.h's +0xA8 as UIStarmapClass (mainview including ui_starmap.h lost
     0x512A00 / 0x514960: read its objdiff first); ui_scan's 0x60ECD0 and 0x610370 (inline-budget; a delta_template.h
     edit costs aux_roots' 8 pairs), UIScanThreatClass::Unknown_Refresh (declared, inline in 0x60DF90 / 0x60F1A0), NOT
     EXACT 0x60E300, 0x60E580, 0x60D8D0, 0x60E6A0, 0x60F1A0; the target lines need a replay that selects a target;
     Jupiter needs its CREATE + PLANET_POSITIONAL_UPDATE in antprox's replay (antprox repo); the render states:
     rs_list's twins 0x45A750 / 0x45A7E0; enbscene's NOT EXACT 0x75F8B0 (field-offset) and 0x75FB00 (FP order);
     camera_manager.cpp / camctrl_takeoff.cpp's local MainViewSceneClass -> enbscene.h where lines hold; the stand-ins
     listed in poc_world.cpp / poc_world2.cpp / poc_world3.cpp / poc_world4.cpp / poc_gadget_*.cpp. Evidence-backed
     cleanups: the 0x755AF0-0x755C30 COMDAT copies are sclient.cpp's, tied to SClient::Init 0x741180 (NOT EXACT).

6. **Step 8's tail:** at LIMIT (Rate Count, the UDP constructor 0x82CBF0 register, Listen 0x82A470 register: try
   tonight's thin-forwarder / `it._Inc()` levers first) or in the Fable queue (TimedLog 0x824540).

7. **The Fable queue** under D89 / D97 (by unlock value; D136: not pruned, rounds scheduled more often):
   client_lego_impl's Add 0x72B7F0 (P-H20-1's round);
   CLIENT_DAMAGE 0x738970 (P-T19-3); ClientClass's constructor (D92, item 2); Get_Lego_Transform (P-H17-2);
   Load_Definitions 0x71B1F0; P-F8-3 (0x6FBEF0 with a per-field inline
   helper, low priority). Low priority: TimedLog 0x824540 (P-S1-1), option.cpp Get_String 0x7F9700 (D108),
   CLIENT_CHAT_EVENT 0x73B150 (P-T20-1), the kv_list destructor's twin 0x4B6B60 if a lever appears, the distortion
   draw 0x9961E0 (LIMIT), the ShdMesh ctor 0xA08F80 (LIMIT). Out for good (register / order): Make_Screen_Shot,
   Update_Movie_Capture's LockRect residue, Rate Count, Run / Start_Game / Go_*, 0x6FD310.

8. **Provisional placements (D137;** revisit when evidence appears; new ones are added here and marked
   `// PLACEMENT: provisional (D137)` in the source at each file's next edit, three files so far):
   co_subtypes.cpp / co_subtype_base.cpp (D122, D132: no __FILE__); unknown_px_callbacks.cpp (D124) and
   unknown_px_factories.cpp (D139); PxObject 0x4557F0-0x455860 in px_collision.cpp (D125); px_wrapper.h (PX1); the
   view at SClient +0x1358 at mainview.cpp's end (D138); the Doppler flag 0xC404F8 in mainview.cpp (P-MV1-3); the
   0x514CC0-class and Unknown65B810ViewClass files (LD4); step 8's unknown_memregistry.cpp / unknown_listener.cpp /
   unknown_rate.cpp (D86); inpkeys.cpp (H19); unknown_font_split_string.cpp (LS1: between split_string.obj and
   symbol_substitution.obj in link order, so its real name sorts between those); Unknown5BB320Class in sclient.h
   (CP1); ui_group's +0xE8 object 0x5927E0-0x593180 in ui_group.cpp (UG1); UnknownAEEA88Class in ui_scut.h (SC2;
   also bridge_int.cpp's 0x53B040 and the 3-D movie view 0x7EEF90, until bridge_int.cpp is rebuilt); rs_track.h's
   interim rs_list / rs_proj_sup classes (RT1); scriptedmovieline.cpp / .h (LC1, 2026-10-31: the object
   0x7FFC50-0x7FFE9F between option.cpp and scriptedmovie.cpp, no __FILE__); unknown_screenfade.cpp (TG3,
   2026-10-31: the screen fade / letterbox object 0x7FF010-0x7FFC4F between option.obj and scriptedmovieline.obj,
   its own statics, no __FILE__; NAME 1); from the 2026-11-02 runs: unknown_6ca0c0.cpp / unknown_6cce60.cpp (font
   layout), unknown_msgbox.cpp, unknown_51c800.cpp, unknown_515640.cpp, unknown_458710.cpp, unknown_45e3f0.cpp,
   unknown_message.cpp / _record / _server / _filter (and Unknown_4BE9D0 in the filter's), mousetbl.h, enbassetmgr.h.
   Added by later runs: delta_visitor.cpp (KV2), unknown_45d840.cpp, unknown_763e00.cpp/.h (RS1), cammod.cpp, camctrl.h,
   unknown_798ae0.cpp, unknown_7979a0.cpp (CM1), ui_proc.h, observer.h (UP1; the .cpp names are the client's),
   TimeMgrClass (SN1, D24 NAME 3), DeltaVisitorClass::Visit_Children 0x4B0860 left in unknown_root.cpp (moving it loses
   0x4A46F0), kv_encoder.cpp, kv_acc.cpp, kv_acc_string.h, kv_acc_time.h, kv_time_adds.cpp (no `__FILE__`;
   KVFilterEncoderClass's 0x4B5AD0-0x4B5D70 belong in kv_encoder_base.cpp's object by position, `<string>` initialiser
   at 0x4B5E80), 0x7BE5A0 as closing radar_system_data_cache.cpp and the PermStringInitClass static at 0x7BB180's owner
   (position and shape only, RD1), starmap_data.cpp's local `RObj2Texture` stand-in, ui_registration.h's classes under
   `__UI_REGISTRATION_MEMBERS` (they left the .cpp to hold the ctor's assert on line 369), slot_data.h's name (RTTI's
   class, link order only; ui_slot_scut.h and three others keep SlotDataClass stand-ins), skill_ability_names'
   `Unknown_815320`-style free functions, radar_system.h against sclient.h's same-layout stand-in, radar_system.cpp's
   `PDAInterfaceUnknownA0Class : MapPipClass` (base unverified), targeting.cpp's name (no client string; the asserts say
   'targeting'; sorts between target_reticle and ui_starmap), 0x68FB60 / 0x68FC80 at inv_render.cpp's end (P-IR2-1),
   unknown_67d430.* (no __FILE__), unknown_671300.h, ro_offset.cpp, ro_proxy.h's name, unknown_45a090.cpp and
   unknown_45f390.cpp (the 'link' and 'sound' render states; rs_link.cpp / rs_sound.cpp would fit the link order: no
   client string), ui_mfd_extended_context.cpp's name (NAME 1), target_generator_summary.cpp's name, the Unknown_B0804C
   tooltip width in gd_attrb_skills.cpp, ui_starmap.h's StarmapPipClass.
   No __FILE__, or a name from a window string: unknown_7cef30.cpp / .h (XR1: the
   subs-interface account client, RegistrationUnknown1ACClass, object 0x7CECC0-0x7DFE70), unknown_7dfed0.cpp / .h (XR2:
   Sub14CImplClass, the registration reader over MSXML), unknown_7e1e40.cpp / .h (XR2: Sub14CUnknown18Class, the reply
   download; the account / subscription XML reader range 0x7C9000-0x7E3000 between ui_starmap.cpp and avatar_pip.cpp), ui_log.cpp ('UI_LOG', the log book window) and ui_guild.cpp ('UI_GUILD'), both TM1's, ui_jobs.h (JB1:
   UIJobsClass over Unknown5BB320Class, interim), client_item_property_builder.h (IP1), ui_guild_list.cpp / .h (GL1:
   'UI_GUILD_LIST', object 0x81E030-0x81F0FF), client_item_property_unknown_529810 / _description / _name /
   _option_auto_fire.cpp (IP2: the property classes 0x529810-0x52B380 without a `__FILE__`), client_item_property.cpp /
   _attribute.cpp / _attribute_distance / _float / _integer / _percent / _seconds / _string.cpp (IP3: 0x51F900-0x521B1F,
   no asserts), ui_inv.h (UI1: UIInvClass, named from the file, D24; no RTTI) and ui_slot_data.h (UI1: its base
   UISlotDataClass 0x62DE80, declared only; ui_attrb_skills.h's Unknown62D1F0Class may move onto it),
   ui_option_account.cpp / .h (MT1: the options' account
   page 'opt05a', UIOptionAccountClass, no string names the file), ui_option_channel.cpp / .h ('opt06a') and
   ui_option_check.cpp / .h ('opt03a') (OP1: named after the tooltip sections UI_OPTION_CHANNEL / UI_OPTION_CHECK),
   ui_radio_main.cpp / .h ('cht01', tooltip 'UI_RADIO_MAIN') and ui_radio_join.cpp / .h ('cht03', 'Join Private
   Channel') (CH1: no asserts in either), ui_radio_list.cpp / .h ('cht02') and ui_radio_admin.cpp / .h ('cht05',
   'UI_RADIO_ADMIN') (CW1: no strings name them).

   From the 2026-11-09b night run: inv_data.cpp / .h (ID1: InvDataClass, client `__FILE__` 0xB8FDC4; the header placement is D137), ui_pda_cargo.h
   and ui_pda_equip.h (PW1: the .cpps follow the client's `__FILE__`, the headers are D137), unknown_764fd0.cpp (GS1: the chat
   colours at SClient +0x13CC, no `__FILE__`), gd_inv_slider.cpp's class Unknown669FB0Class (PW1: the name is interim),
   drag_mouse.cpp (SMouseDragClass) and drag_slot_data.cpp, ui_skills.cpp, ui_slot_info.cpp (SK1: names to confirm at their next
   edit; ui_slot_data.cpp's SlotDataInterface is RTTI), ui_slot_data.h (the UI1 entry above: now defined). 2026-11-10: ui_stringentry.cpp / .h (TC1: UIPTradeCreditsClass
   0x649890-0x64A32F, file named from the tooltip section UI_STRINGENTRY, NAME 2; the class name should become
   UIStringEntryClass in a cross-cutting rename: it serves the guild, ignore and friends entries too), gd_dynamicelement.cpp
   (DW1: DynamicElementGadgetClass 0x6686F0, NAME 1, placed between gd_buffgadg and gd_dynamicwindow), gadget_collection_interface.h
   (DW1: GadgetCollectionInterface / GadgetEvent are RTTI names, the header's name is interim), ui_guild.h (CP2).

9. **Tools:** the Claims Action: check that the next merged run PR's pull_request_target (closed) run succeeds and drops
   its claims from `claims-state` (11 startup_failure runs 2026-11-09 16:02Z-18:11Z only, none since, workflow unchanged;
   treated as transient; on a recurrence capture the run's annotations in the UI: `--log-failed` and the jobs API are
   empty for startup_failure). P-LD3-2 (`wt.py bring-over` accepts named D119 ambiguous addresses, checked against the ambiguous
   column) with P-CS1-2 (the same flag for D66 (i) / (v) moves when pairs_cmp's lost-outright list is empty); promote
   P9's worktree scripts (`.work/p9/wt.py`'s leftovers, `checks.sh`, `commit_wt.py`; commit_json.py must keep
   imports.json's maps sorted); the compare-and-swap commit rule (`update-ref master NEW OLD`) into commit_mine
   (H17's race); gen_stubs' by-value returns (P-T18-2; it also skips by-value std::string returns, XR1); H16's
   `.work/h16/fm_gen.py` / `commit_h16.py` pattern for generated files; promote the inline-budget lab (IB1's
   `.work/ib1/e1-e11.py` and `census.txt`, IB2's variant runner `.work/ib2/vr.py`: its marker insertion
   shifts `__LINE__`) into tools/re if the census walk goes on; auxroots.py's pad comment is stale (SYMBOL_PAD 58,
   SEND_SYMBOL_PAD 47, DEF_ORDER); `appfile.py match` mixes overloads and its pad windows differ from the importer's
   (workflow.md 3.4, 3.6). `wt.py bring-over --sendpad` (tools/vc6/sendpad.py) has not yet run on a real bring-over that loses sclient
   pairs: check it at the first one; a cheaper pre-sweep of the current pad +-8 would cut its ~5 min.
   Data and generated files: symbols.json / ghidra_symbols.py keep 15 old D198 names (and vtable 0xAFB5F4 reads
   `??_7Unknown_675120Class@@6B@`: it is UIBackGadget's, D208): run `symbols.py` with every library's `--objs` at the
   next refresh (without them it rewrites the file wholesale).
   symbols.json: the next full regeneration (workflow.md §3.4, every `lost outright` checked against the client's vtable
   layout first) also renews the 2026-11-03 renames' long functions (still under `ControlKeysManagerClass` mangled
   names; they match by bytes). SY1's lead: D100's range (and the self name) could reject a `??_G` / `??_E` whose
   vtable's slot count differs from the object's `??_7` (it would have avoided 0x723AB0).
   datacmp (`tools/re/datacmp.py`, D201): ui_manu `_Unknown_BE5C20`: 104 bytes in our .data, first byte 5; the client's
   0xBE5C20 is outside its raw data (BSS), so it holds nothing initialised there: either that symbol is not at that
   address, or the object initialises what the client sets at run time. Less certain than the two constants datacmp
   found first (fixed).
   The rest of the static table audit (`tools/re/datacmp.py`, D201: run when wanted): 739 of 932 symbols compared at its
   first full run, 193 not placed (`--list-unplaced`); left: 4-byte scalars (BridgeNear / BridgeFar, ...), constants
   read only by non-exact functions, tables the compiler inlined or emitted no symbol for, 8 masked pointers in 7 tables
   (ui_attrb 0xB79F04, ui_analyze 0xB7967C, ui_manu 0xB81868, ...).

10. **Parked leftovers** (each waits for a lever; `attempts.py show ADDR`): inv_data_peek 0x68A090 / 0x68A270 /
   0x68A450 / 0x68A8C0 (register: the failure path's inlined string ctor `[ecx+ebx]` vs `[ebx+ecx]`, skill_data_peek
   0x6A5730's residue); fontsys 0x6CD950 (0.71, register: index edi / slot ebp swapped by every clamp form, ~120 forms; a
   copy at the top fixes the registers but hoists the mov); fontdatabase's Set_Font 0x6C38F0
   register, Load 0x6C52B0 frame; the inlined StringClass::operator+=(TCHAR)'s base / index swap (LIMIT REACHED; lead:
   how the client's wwstring.h writes `m_Buffer[cur_len] = ch`); SoundDefClass 0x43D3E0; attrib_dir's Notify 0x4D71D0, Unlock
   0x4D8690, Attach_Mod 0x4D6820 / 0x4D4810; spln_db's Create_Hermite / Create_Color 0x485C10 / 0x487400 (only the
   `new HashNodeClass`'s EH state differs: when VC6 compiles GenericNode's out-of-line ctor, a reasoning lead),
   Get_Color_Curves 0x485440, Create_Blend 0x486200, ~SplineDatabaseClass 0x484810, Resolve 0x487660 (24 masked rows,
   `docs/attempts/487660/`: only the alpha lookup's HashListClass::Find is still inlined); the shaders' `multiple`
   functions (D88), Compute_Caps 0x9B0160, the meshmatdesc noise (D18 / D28); `CreateBrowser`'s app callers 0x7CA5C0
   / 0x7CAD00 (E&B's `querydata`); P-H11-2 (the _Instances destructor frees nodes without element destructors: leave
   unbuilt until another SGI user shows it); `selfname` into imports.json only when something needs it (D88 row);
   same-shape template instances stay `multiple` until their relocation targets have names in symbols.json (P9).
   * Near misses by file (attempts.csv rows; the parenthesis is the closeness where recorded and the cause):
     - accel 0x4990C0 (shape)
     - camera_manager 0x79B4F0, 0x799E40 (frame), 0x7998B0 (order)
     - cammod_chase 0x79BFC0 (0.42, inline-budget)
     - chat_window / chat_window_history (CH2): 0x67F7F0 (register 0.68: the client spills this, max_lines, i),
       0x682770 (order 0.38: VC6 duplicates the Is_First test); attempts.csv.
     - client_lego_root 0x732880 (0.86, shape)
     - galaxy_map_pip 0x5404E0
     - galaxy_map_render 0x7A8B70 (inline-budget; holds 0x7B18A0-0x7B1EC0's 11 template members), 0x7A6DD0, 0x7AA660,
       0x7ADFC0, 0x7AB6D0, 0x7AC130, 0x7AD090, 0x7AF400 / 0x7AF4A0 (inline-budget), 0x7A72C0 (register: fdivrp),
       0x7A7560, 0x7A7D70 (field-offset), 0x7A83D0, 0x7AD5B0 (frame, untried), 0x7AD850 (blocked: colour part only)
     - galaxy_map_robj 0x7B2B30 (register), 0x7B43F0 (order)
     - gd_text_editor 0x671CB0, 0x6732F0, 0x6735A0
     - gutter_data 0x7B54E0, 0x7B63C0 (register); gutter_radar 0x7B75E0 (shape: `return 1.0f;` cross-jumped)
     - inv_render and neighbours (IR2; don't spend heavily): 0x68C6B0, 0x68F510 (inline-budget), 0x68CCC0, 0x68E360
       (register), 0x68FE10, 0x690AA0, 0x690D20 (order). Stand-ins left in the new files: `Unknown66CC00Class` (HUD
       gadget), `Unknown688830Class`, `Item` (inv_data)
     - kv_encoder 0x4B5D70 (0.43, inline-budget; 0x4B5E40 and 0x4B5E60 stay unbuilt with it)
     - nav_list_builder 0x7B7EF0 (0.84, order)
     - orbitalmanager 0x4F2CB0 (0.53, order), 0x4F45A0 (0.57, layout)
     - radar_data 0x7BB570 (0.54, register; the most bytes left, case 1 and case 0's copy registers are the lead),
       0x7BBC00 (0.76, inline-budget)
     - radar_system 0x7BCA70 (barely tried), 0x7BE3A0
     - rs_list 0x45AB20, 0x45AD90, 0x45B550
     - skill_data 0x6A4F40; skill_data_peek 0x6A5730; slot_data 0x6A7730, 0x6A7860
     - starmap_data 0x7BEF50 (0.50, register)
     - target_generator_summary 0x84BE60
     - target_reticle 0x7C5690 TargetReticleClass::Update (register, parked at about 30 forms; its 16 no-code finds are
       not the client's source)
     - ui_mesh 0x5B8330, 0x5B8850, 0x5B8EE0, 0x5B8FF0
     - ui_mfd 0x5BECF0, 0x5BCE50, 0x5BD540, 0x5BDC20, 0x5BDDF0, 0x5BDED0; ui_mfd_extended_context_harvestable
       0x5C0350; ui_mfdm 0x5C25C0
     - ui_proc 0x5DD3C0 (order 0.81)
     - ui_slot_scut 0x6309F0 (about 300 forms: stop); ui_verb 0x658C90 (about 80 forms)
     - ui_talkt 0x64B6E0, 0x64C8D0, 0x64CC90, 0x64D400, 0x64D770, 0x64E7A0 (0x64D770 and 0x64E7A0 have better draft
       forms, TK1's rows)
     - unknown_75b7b0 0x75B8F0 (0.83, order), 0x75BB50 (0.37, inline-budget); unknown_79c8e0 0x79C970 (0.52, shape);
       unknown_7979a0 0x797B50, 0x7981B0, 0x798570, 0x7987F0
   * Near misses by file (attempts.csv rows where recorded):
     - client_item_property_builder 0x522860 (0.61, 17.7 KB, all 37 asserts in order), 0x521C10 (0.70), 0x527F40 (0.78,
       frame), 0x5293B0 (inline-budget)
     - unknown_7cef30 0x7D6A60 / 0x7DAD50 / 0x7DB2D0 (0.71, inline-budget: the request dtor 0x7D4D60 stays out of line),
       0x7DA760 (0.66, register), 0x7D4E30 (0.56), 0x7D5600 (0.50), 0x7D9790 (0.60, frame), 0x7D6180 (0.26, shape);
       unknown_7e1e40 0x7E1F10 (frame: the EH states around the _bstr_t members, six forms)
     - ui_jobs 0x5A3FB0 (0.53, 14 forms: inline-budget / shape)
     - ui_log 0x5AB030 / 0x5ABB10 (register); ui_guild 0x81AEB0 (order), 0x819FB0 / 0x81A420
       (inline-budget)
     - ui_cpit 0x5821C0 (0.62), 0x5806E0 (0.66, whole-function budget), 0x57D6C0 (0.58); ui_manu 0x5B5320 (0.50),
       0x5B4B10 (0.59), 0x5B2480 (0.72); ui_mfd 0x5BDED0 (0.74: Set_SP_Max's Cast<float> out of line needs a
       spender after the MaxShieldPower find that keeps 0x5BE330 exact, e.g. NameClass assigned if it has a user
       operator=; MF1's six forms listed), 0x5BD540 (0.72: ours emits Unknown_MFD_Field / DeltaInterpolatorClass::
       Get_Value copies the client lacks)
     - fontsys 0x6D47C0 (register, 1 row), 0x6D4C70 (order, 4 rows), 0x6D0A70 (inline-budget), 0x6D5C10 (FPU copy),
       0x6D60C0; 0x6C2890 / 0x6C2A10 parked (FN1)
   * Near misses from the 2026-11-09b night run (attempts.csv rows where recorded):
     - ui_starbase (SB1) 0x641FA0 (0.80, register), 0x63E2B0 (0.72), 0x63A100 (0.63), 0x63E770 (0.57), 0x643DF0 (functional)
     - chatgadg (CE1) 0x65BFD0, 0x65CCD0, 0x65DE90, 0x65E250, 0x65E5E0, 0x65E840
     - ui_skills / ui_slot_data / drag (SK1) 0x62D730, 0x62DE80, 0x62E320, 0x630070, 0x684F20, 0x685060, 0x685380, 0x685500
     - ui_pda_cargo / ui_pda_equip / gd_inv_slider (PW1): twelve (`attempts.py list`)
     - inv_data (ID1) 0x688B90 (0.87, one load order)
     - sclient (GS1) 0x744560 (NOT EXACT); Do_Frame 0x742B60 is still relocation-class (PD1's lead)

11. **Reminders:** D7's (revisit byte-exactness of E&B's code with the user if it becomes too costly;
   `docs/data/not-exact.md` shows how much functional code waits); which bool-return form gives a bare
   `setcc al` without the `xor`; the inline budget blocks most parked UI rows: a study of VC6's /Ob2 budget (F8 /
   IB3's levers, workflow.md §3.1) is the shared lead.
   antprox's build table (AntaresProxy repo) calls 249f8524 EA's last client (D199): it is EA's 82663b2f plus Net-7's
   URL edits; correct the label there, and add 82663b2f if EA's exe should be recognised.

12. **The 2026-10-30 night's files**, left in each: rs_track.cpp's ctors 0x45FDA0 / 0x45FFB0 (order), 0x460E80;
   proj_man's 0x493620 (one register row). ui_group.cpp: 0x5946D0, 0x595530, 0x596770,
   0x597E40 (best: from 0x5979A0 / 0x597A60's forms), 0x592C50 and its NOT EXACT rows. ui_cpit.cpp: 8 parked, mostly
   inline budget (out-of-line Cast / Find_Field / _Tidy). ui_manu.cpp: 0x5B4B10, 0x5B5320 unbuilt, 11 inline-budget
   parked. ui_scut.cpp: 6 parked (the bar's ctor 0x610F80 register, the control keys 0x611300 / 0x611710, 0x6120E0's
   float loads (best lead), 0x612820, _Sort_0 0x6129C0).
