# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

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
exchange 0x829180, memconn.cpp. D84 records P-B1 (`~TCPMgr() throw()`); P-B2 (per-file flags) declined
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
| D4 | 2026-10-22 | Import EA code file by file, as each file is verified against the client | user |
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools (the copies this project used; it does not distribute them, D150; [toolchain.md](toolchain.md) §2 lists their SHA-256 hashes) | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |
| D6 | 2026-10-23 | E&B's own code starts with `app\shared\auxdata`, then `app\wwudp`, then the task kernel (`app\kernel`: `KernelClass::Run_One_Cycle`, the loop running the current, background and pending-delete tasks; with `app\main` and `app\client\tasks`), then `app\packets` ([app-plan.md](app-plan.md) §3) | user |
| D7 | 2026-10-23 | E&B's own code is held to byte-exactness everywhere, like the libraries. Check in with the user periodically if this becomes too costly. Trigger: revisit with the user when docs/data/not-exact.md passes 400 entries (269 on 2026-10-27) (amended 2026-10-27, D110) | user |
| D8 | 2026-10-23 | No hybrid client (reconstructed functions patched into the retail one) for now: its results would need the user to validate them. Open to it again if it can be shown to verify results better than byte matching, without the user | user |
| D9 | 2026-10-23 | E&B follows Zero Hour's KM/KJM (shader system) line through 08/01/02, without the 08/05/02+ texture redesign and without the Generals team's MW/GTH changes. So `TextureFilterClass` (08/05/02 KM) is dropped: `TextureClass` has Renegade's five filter members (texturefilter.cpp/.h `NOT_LINKED`) | user; overnight finding: Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks, and it unlocks wwshade. Reworded by the user 2026-10-23 after step 1 showed the client has the 06/27/02 abstraction but not the 08/05/02 redesign (library-matching.md §3.6.1), and again 2026-10-24 after the dated-comment sweep (`.work/night/item6i/report.md`: bytes place every dated ZH edit; MW 0/3, GTH 10/12 absent); filter members folded in by the user 2026-10-24 (all four constructors exact, 0b8f6ce) |
| D10 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D11 | 2026-10-23 | Compiler-generated code (STL template instances): extend the corpus as each app file needs it, not ahead of the app work | user |
| D12 | 2026-10-23 | Compile each library with the flags E&B used, wherever byte evidence shows them (wwshade: `/GX /Ob1`; importer `LIB_FLAGS`, `bytematch_all.sh` `LIBFLAGS`) | user: "match how E&B built things whenever you have evidence to support it" |
| D13 | 2026-10-23 | EA files the client does not link are dropped from the tree (importer `NOT_LINKED`, with the evidence): first `shdlegacyw3d.cpp`/`.h`; then WWAudio's `AudioSaveLoad.cpp`/`.h` (2026-10-23) | user |
| D14 | 2026-10-23 | `ENABLE_WWPROFILE` in WWDebug too, although its bytes are neutral (verify: 67 before and after, none lost): consistent with a project-wide define. Soft: drop it if it ever costs a match. Also (D26, merged 2026-10-27): `USE_WWSHADE`: WW3D2 built with it (under D12, evidence: the `SHD_*` wrapper calls); wwshade too, as a soft consistency setting like the above (neutral in the bytes); done e278d81, d2955f0. Also (D74, merged 2026-10-27): `ENABLE_WWPROFILE` goes into the app library's defines, kept if a verify changes no pair (evidence since: WWAudio +32, WWMath +1, WW3D2 +20 pairs) | user |
| D15 | 2026-10-23 | `G_CODE_BASE` in WWAudio stays an importer patch removing its two `#ifdef`s, not a `/D` in `LIB_FLAGS` (same bytes) | user: only `WWAudio.cpp` uses it, so the bytes can't show a project-wide define |
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
| D37 | 2026-10-24 | App class names from the auxdata work: `DeltaClass`, `DeltaCompoundClass`, `DeltaTemplateClass`, `NameClass`, `LookupClass`, `LinearBufferClass`, `MoneyClass`, `DurationClass`; `PermString` keeps the client's own name (its debug message at 0x4ADFF0), without the suffix; the header of the assert functions is `app/shared/unknown_debug.h` (`Unknown_Assert_Failed`, `Unknown_Fail_If_Report`, D10) | user (accepted builders' proposals) |
| D38 | 2026-10-24 | Names chosen from behaviour stand: `Is_Parent_Of`, `Is_Parent_Or_Same`, `Match_Parts` (name.cpp), `Unknown_Set_Size` (linearbu.cpp), `StringValueClass`, `Add_String` (delta_string.cpp, delta.h), `PermStringInitClass` (permstr.h), `Unknown_Debug_Printf` 0x509DC0 (dbgmgr.cpp) | user |
| D39 | 2026-10-24 | The time and money field instances `DeltaTemplateClass<DurationClass>` / `<MoneyClass>` are explicit specialisations (`template<>` constructor, Read, Write) in delta_time.cpp / delta_money.cpp, declared `extern template` where the roots use them (aux_roots.h for money). Reworded 2026-10-25 to the form the tree has: the first wording (`template class ...` instantiations next to the header's `extern template`) doesn't compile in VC6 (C2949, C2909) | user |
| D40 | 2026-10-24 | `DeltaCompoundClass::Add` 0x4A5670 (D11): check the client's other `vector<T*>::insert` copies for the out-of-line `size()` 0x4A58C0 before changing the STL corpus | user |
| D41 | 2026-10-24 | Header `__FILE__` strings in app code: leave them for now (ours include `..\duration.h`; the string differs, bytes and lines match through the relocation); run app compiles in `e:\build\beta\G\app\<dir>` with `/I ..` and the client's include spellings when strings are compared (whole-image check) or the project directory's name is known | user |
| D42 | 2026-10-24 | Milestones M1 (capture harness on the rebuilt auxdata and wwudp classes) and M2 (working VIDTEST on the tree's libraries) drive the work met 2026-10-25 (D63); the NOT EXACT rule continues as D64 ([milestones.md](milestones.md)). Functional code on their path that isn't byte-exact yet is allowed, marked `// NOT EXACT:` (P1) (amended 2026-10-27, D110) | user |
| D43 | 2026-10-24 | The 11 AuxData root constructors (first written as 13) are generated from the extracted schemas in the client's code form, then verified like any app file (P2) | user |
| D44 | 2026-10-24 | Missing functions at link time are stubbed in `stubs/` only, never in library files; each stub logs its first call (P3) | user |
| D45 | 2026-10-24 | New programs: `tests/capharness/`, `tools/harness/` (Python side), `programs/vidtest/` for VIDTEST's own code (WinMain, tests, `results.ini`: no source path survives in VIDTEST.EXE). VIDTEST's E&B scene classes follow the client's layout (`app/shared/stars.cpp`, `spline.cpp`, ...): they share the client's strings, including `\\.\mailslot\G\...` (P4) | user |
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
| D66 | 2026-10-25 | Pair losses. Not losses, named in the commit message: (i) a COMDAT duplicate kept matched in another file (D20); (ii) the same source compiled into two files (D55); (iii) a false match exposed by a type fix (first: 0x8C1520, `KVPairTemplateClass<__int64>`'s string setter, matched by the money instance only while `MoneyClass` was a bare `__int64`). A change that gains and loses nothing but models the client's own classes (07af3e1: Ship's BuffArray and Buff elements with the client's vtables and globals) is allowed, stated in the commit (2026-10-25, evening). (v) a net-positive change where every lost pair's address is re-credited under another file in the same verify (pairs_cmp's address-level 'lost outright' list empty), named in the commit; an address lost outright still waits for the user; (vi) a pair that becomes ambiguous because the client's own form adds a byte-identical instance (D119). (vii) a caller tie-break pair (D166) lost with its caller's pair. Anything else is a loss and waits for the user before the commit. D54's ambiguous column stays as is: the 17 identical template leaf Reads are not pairs (amended 2026-10-27, D110; 2026-10-28, D119) | user |
| D67 | 2026-10-25 | Test programs live under `tests/` (D45 extended: kerneltest and movietest in tests/capharness). The wlib stand-ins (app/common/dictionary.h, critsec.h, app/wwudp/wtime.h, parts of cordcon.h) stay until D73's import replaces them with the imported files at the client's paths (`libs\wwonline\wlib`), then are deleted Done: the wlib stand-ins are deleted (D73's import is done), so the "stay until" clause is complete (2026-10-27) | user |
| D68 | 2026-10-25 | [kernel-map.md](kernel-map.md)'s 10-step reconstruction order is adopted (extends D6); steps 1-5 and 7 done or partly done on 2026-10-24 | user (backfilled) |
| D69 | 2026-10-25 | Safety rules for every run of our programs and every session (amends D48): never a modal dialog (`-nomsgbox` or its equivalent in every program); full screen allowed; no screen grabs, images only from the program's own back buffer; never the game's install folder as working directory; no registry changes (the original E&BConfig only in a supervised run with a registry backup); never push (amended by D148 / D157: push only a run branch, never master); never `importer.py assemble` on a tree with work; `git add` by name, `.claude/settings.json` stays out | user (each given on 2026-10-24; numbered 2026-10-25) |
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
| D80 | 2026-10-25 | `objmatch.py` breaks `multiple` ties by each data relocation's target (the string or known symbol it points at) against the object's own literals, so byte-identical functions that differ only in their strings count (the assert reporters, the twin constructors 0x4A7080/0x4A8150, `Set_Render_Device`, the one-int packet constructors and Prints: builders B and D). Adopted after a snapshot comparison of every library's pair set with no losses, the counts before and after recorded in library-matching.md §3.6.1. The objmatch order (merged 2026-10-27 from D88, D94(b), D95(b), D100): D80 rounds to a fixed point; then the self-name round (D88: the one candidate address that symbols.json names with the function's own mangled symbol from a non-exact proof, a vtable slot, is credited when D80 finds no contradiction there; measured by builder W3 at the objmatch level: +337 pairs, 0 lost, ww3d2 188, wwmath 60, wwaudio 45, wwshade 31, wwlib 10, wwdebug 2, wwsaveload 1; prototype `.work/w3/objmatch_sn.py`; adopted after a full verify snapshot of a quiet tree, with pairs_cmp proving the gain, not while builders are verifying; carried out 2026-10-27 04:50, 0d70c67 the pre-D88 snapshot, efc3cbb the rule: +368 pairs, 0 lost, ww3d2 188, wwmath 60, wwaudio 45, app 31, wwshade 31, wwlib 10, wwdebug 2, wwsaveload 1; `tiebreak`/`selfname` do not reach imports.json, left as is until something needs it); then D80 again; then the range round (D100: a tie goes to the object whose client range contains the address, P-P6-1, carried out on a quiet tree with a full verify of every library and pairs_cmp gains-only, like D88, then 45 from `.work/p6/park45` and 117 from `.work/p5/park117`; refinement d8b8153: an address another function is exact at is held out, and the range is the object's own non-COMDAT or NODUPLICATES exact functions); compiler-generated `_$E<n>` names are skipped (D94(b), P-H7-2, carried out the same way). D95(b) (the D80 tie-break not crediting an implicit destructor to an identical twin when the function's own address is free, P-P5-2) is not implemented: superseded by D100 and its refinement, close pending as P-M1-1 | user |
| D81 | 2026-10-25 | D75 corrected: 0xBDA908 is the client's pooled `""` string, not a log object; dbgmgr.cpp calls the reporter 0x4BC130 as `("", "", file, ...)`. The `UnknownLogClass`/`UnknownLog` stand-ins in app/wwudp (cordmgr.cpp, bandwidthaccounting.cpp, udpreplay) become plain calls with `""` (follow-up after builder A leaves app/wwudp) (D75, merged here 2026-10-27: the assert functions' reporter 0x4BC130 (the logger) is a reconstructed stand-in with D10 names and D56 tags; its premise, a log object at 0xBDA908, is what this row corrects) | user |
| D82 | 2026-10-25 | `app/shared/unknown_debug.cpp` (the assert reporters 0x4E9410/0x4E9450) and `unknown_new.cpp` (E&B's operator new/new[]/delete over FastAllocatorGeneral) stay two files although one object in the client, so kerneltest and movietest can define their own reporters without LNK2005 | user |
| D83 | 2026-10-25 | Builder A's proposals accepted: BinkMovie lives at the top-level `binkmovie/` beside ww3d2 (the client has no path for it); wlib's wstypes.h takes sint8/sint32/uint32 from WWLib's bittype.h so one definition holds program-wide (same bytes, needed for linking); E&B's own tcpcon.cpp (0x8BBA70-0x8BCA50) and tcpqueue.cpp are rebuilt in wnet next, before connect.cpp's factories (RenegadeGR's tcpcon.cpp matches nothing: E&B's TCPCon is an OutputDevice with a lock writing through queues); build.py and `LINK_LIBS` learn the new libraries when a program first needs wlib | user |
| D84 | 2026-10-26 | wnet's `~TCPMgr()` is declared `throw()`: wnet must build with /GX (TCPMgr::wait, TCPCon and TCPQueue have exception frames) but E&B's destructor has none, and `throw()` is the only form found that makes VC6 omit it. Compiler evidence, not a known source line (P-B1); kept because it gains and loses nothing. Per-file compile flags (P-B2) not added until a library really mixes /GX and none | user |
| D85 | 2026-10-26 | Open-BFME-1 (a byte-exact BFME rebuild under MSVC 7.1) is not a source and not a root: its ww3d2 is the DX9 port with BFME's layouts and reverse-engineered names, so nothing of it is copied (the assessment is summarised in the 2026-10-26 21:25 log entry). Its method is taken: the attempt log (`attempts.py`, read before any retry), the near-miss classifier (`explain.py`) with §3.1's triage order, the one-shot switch sweep (`flagsweep.py`, never looped) and the two-attempt stop rule for register-only and order-only residues (a third attempt is a Fable job under D72) | user |
| D86 | 2026-10-26 | merged into D56 (2026-10-27) | user |
| D87 | 2026-10-26 | SGI STL 3.3's `stl_hashtable.h` / `stl_hash_map.h` (the permissive SGI/HP notice, compatible with D1) are imported, adapted for VC6, as the client's hash_map: the string hash `__stl_hash_string` 0x42DB00 (h*5+c, 65 callers), `__stl_prime_list` once per translation unit (336 copies, the first 0xAE701C), the buckets a VC6 `std::vector` at +4. First users: dbgconn's counters 0x825390 / 0x8255B0 (keyed by std::string, bucket function 0x825B30) and kv_list's index 0x4B62B0 (replacing its std::map stand-in). Builder S3's P-S3-1. Carried out 2026-10-26/27 (H3, H4): the source lives with the other SDKs at `C:\EnBDevelopment\SDK\stl-3.3` (user), the VC6 headers in `libs/sgistl/` (generated by `tools/vc6/sgistl_adapt.py`, outside the importer's libraries); the client's out-of-line hashtable constructors take three arguments (no allocator), so the adapter drops it | user |
| D88 | 2026-10-26 | merged into D80 (2026-10-27) | user |
| D89 | 2026-10-26 | Fable near-miss limits, so Fable loops no more than Opus: (1) entry only after two recorded Opus rounds with a stashed best form and an explain.py class of inline-budget, frame or shape; register-only and order-only residues never enter (VC6 collapses them to a few canonical outputs: Make_Screen_Shot's 110 forms), they stay parked until a lever proven elsewhere names a cause; (2) one Fable round per function, ever, ending exact or with a written hypothesis in attempts.csv; a second needs a new §3.1 lever filed since, cited in the row; (3) at most 8 forms in that round, each from a stated hypothesis, batched through tryv2.py; (4) at most 4 functions per Fable session; at most 6 active queue entries by unlock value (a group taken as one round, as D97's, is one entry); further requests wait in §4 item 7's parked list; (5) one Fable builder at a time, under about 15% of a run's tokens; (6) `attempts.py show` prints LIMIT REACHED for such functions and `attempts.py check` refuses a retry of one unless `--lever` cites the new §3.1 row (amended 2026-10-27, D110) | user |
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
| D114 | 2026-10-27 | (a) D95(b) is closed, superseded by D100 and its refinement (P-M1-1); (b) P-M1-2's wider range (the object's exact COMDATs only it is exact at, within 0x1000 of either end) and (c) P-M1-3's regeneration of symbols.json each get a full verify of every library on a quiet tree, like D88, and are kept only gains-only | user |
| D115 | 2026-10-27 | objmatch credits a tied constructor / destructor of a file-static to the address its rebuilt static initialiser registers or calls (the `_$E` initialiser's own relocation names it: co_player_relationship's 0x709EF0 registers 0x709F40's atexit), as D88's self name does from a vtable slot; finds and other tied helpers stay tied. On a quiet tree after D114, with a full verify of every library, kept only gains-only (P-R2-1 narrowed) | user |
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
| D148 | 2026-10-29 | P-GH-4: option C. Once the GitHub remote exists, `main` changes only through a pull request: each run works on its own branch (`run/<date>`), the coordinator pushes that branch and opens one PR at handoff, and the user reviews and merges (no auto-merge: a checkpoint before work becomes public). Contributors use PRs from forks. D69 amended: "never push" becomes "push only your run branch, never `main`" (builders still never push; the coordinator pushes); every push passes `tools/prepush_check.py` in a pre-push hook; the agents' credential is a token for this one repository allowed to push branches and open PRs, nothing more. Before the remote exists nothing changes ([github-plan.md](github-plan.md) §3.1) Amended by D157 (verification before master, contributors' landings, the run PR as a draft). | user |
| D149 | 2026-10-29 | Three of Open-BFME's practices adopted: (1) the public README welcomes AI-generated PRs, one verified function per commit; (2) an `AGENTS.md` (the public form of CLAUDE.md / builders.md) that agents reread when 24 hours have passed or a commit reports it changed; (3) address claims, so contributors' agents do not work on the same function (built when outside contributors arrive; ours coordinate through the coordinator meanwhile). (1) and (3) amended by D157 and D156: one PR per claim, squashed; claims as issues. | user |
| D150 | 2026-10-29 | P-GH-5: the project ships only binaries built from its own sources, never a third party's (MSS32.DLL, Bink, DirectX, the game's files): users get those from other sources, like the game art (D143). EA's art and trademarks are not used publicly or in anything distributed; the README states the non-affiliation | user |
| D151 | 2026-10-29 | P-GH-6: contribution lanes. Without VC6: reverse-engineering notes (names under D56, layouts, evidence), Python tools and tests (hosted CI), documentation, and C++ written without VC6, which a maintainer's VM verify (label-gated) proves before merge; with VC6: C++ with the contributor's own pairs result, then the same maintainer verify; later the playable modern-MSVC build. Issues and PRs carry `needs-vc6` / `no-vc6`; CONTRIBUTING describes the lanes. The maintainer's verify is the verify service (D157). | user |
| D152 | 2026-10-29 | P-GH-8: the repository is `Open-EnB/EnB`, default branch `master`; created private, pushed, reviewed on GitHub, then made public; security reports through GitHub's private vulnerability reporting; the Contributor Covenant as code of conduct. P-GH-7 applied the same day: 2FA required for the org, members cannot create repositories, Actions limited to GitHub-owned and verified ones with read-only tokens, outside contributors' workflows need approval, secret scanning with push protection and Dependabot alerts on for new repositories, a `maintainers` team | user |
| D153 | 2026-10-30 | The private reference decoder's project is not part of this repository: no tool reads it and no file names it (the tree reworded, and history rewritten before the repository goes public). The opcode names and aux-data class names taken from the Net-7 lineage are final (pkfactory.py keeps its own table; auxnames.py and n7aux.py retired); the decoder is not extracted as a tool of its own (too much work for the value) | user |
| D154 | 2026-10-30 | The EA captures' reference data is frozen: the five traces and their reference readings (regenerated so each pair is consistent) ship as a release asset of this repository, `ea-reference-2026-10-30.tar.xz` (6.7 MB, SHA-256 checked by tools/harness/fetch_reference.py into roots.EA_REFERENCE); the raw captures stay public at trunk/capturedPackets of https://github.com/therealKyp/Earth-and-Beyond-server. Westwood's RSA key, published with that source, is committed (tools/harness/westwood_keys.py) | user |
| D155 | 2026-10-30 | The test programs are siblings under `tests/`, one folder each (tests/udpreplay, auxreplay, clienttest, auxdata_smoke, kerneltest, movietest), no longer under `tests/capharness/` (D45, D67). The M1.0 skeleton `capharness.cpp` and `tools/harness/build.py`, which built it, are retired: udpreplay and auxreplay carry M1's gates. | user |
| D156 | 2026-10-30 | P-GH-10, claims: contributors' agents and the maintainer's runs claim the functions they rebuild in one register, GitHub issues titled `claim: UNIT` kept by the Claims workflow (`tools/claims.py`); a claim is at most 30 not-yet-exact functions in one file's window (or a bare range), never a lock on editing the file; it lives 8 h without a PR (a draft counts; `/renew` +8 h twice), 3 days without the contributor's activity on its PR, 7 days in all, all counted only while it is the contributor's turn; 3 live claims per account, maintainers excepted. Option B (contributors with write access pushing branches) not adopted. [github-plan.md](github-plan.md) §4.5.1-4.5.3 | user |
| D157 | 2026-10-30 | P-GH-10, verification and merging (amends D148, D149(1), extends D146): every PR is verified before it reaches master, the maintainer's run PRs included, by a polling service on the maintainer's host (`tools/vc6/verify_watch.py`, a scheduled task running master's code; no self-hosted runner), which posts the required check `vc6-verify`; a PR that changes anything that would run on the host waits for a maintainer's `/verify-approved SHA`. Contributors' PRs land as one commit (the test merge plus the verified record, the PR's author kept) through a `verified/prN` landing PR the service pushes and merges: by itself (automerge on) when gain-only, CI green and only sources, headers and docs/data changed, otherwise on a maintainer's `/land SHA`; the maintainer's own PRs and any D66 case are merged by hand. One PR per claim, any number of commits; "one verified function per commit" retired. The run's PR opens as a draft at run start (`tools/run_branch.py`). [github-plan.md](github-plan.md) §4.5.4-4.5.5 | user |
| D158 | 2026-10-30 | File headers say where the file came from: our own files (app/, programs/, stubs/, tests/, the tools' C/C++, files written from the client inside a library) start with `// Earth & Beyond client (reconstruction)`, without Westwood's banner box or an EA copyright line; Westwood's banner and EA's licence header stay only on files imported from EA's C&C releases, and an imported file changed for E&B says so on the line under EA's copyright, written by importer.py itself (`MODIFIED_NOTE`, on assemble, add and replay); third-party code that is not EA's keeps its own notice (libs/sgistl). Header changes never move a code line (`__LINE__`). [conventions.md](conventions.md) §4 | user |
| D159 | 2026-10-30 | Unknown members follow §1's patterns like any member: `Unknown7C` (CamelCase, no prefix), `UnknownFlag`, `Unknown_7C` for a method; Generals' `GameEngine` code (`m_` members, camelCase methods, `AsciiString`) is the counterexample, not E&B's style. Inside an imported library its own style holds: WWAudio's members are `m_`-prefixed, so its unknowns stay `m_Unknown7C`. [conventions.md](conventions.md) §1 | user |
| D160 | 2026-10-30 | P-CSC-1: the Character and Starship Creator (2002-09-13) is a reference, not a rebuild target. It is the 2002 witness for NOT EXACT functions (98 tree functions are exact in CSC and absent from the client: source matching 2002, client changed later, so code-form retries won't help) and a second reference for code the client lacks or drops. Names still come from the client (CSC folds identical functions). [CSC.md](CSC.md) §1.1, §6.3 | user |
| D161 | 2026-10-30 | P-CSC-2: CSC's source and toolchain facts are recorded in [source-tree.md](source-tree.md) (`C:\Projects\G`, `Tools\Avatar Creator`, `chatint` / `textwin` / `usermsg`, `wnet\tcpmgr.cpp`, 0x836960 in galaxy_map.cpp) and [toolchain.md](toolchain.md) §1.2 (VC6 RTM throughout, no 8798 objects, `/OPT:REF,ICF`) | user |
| D162 | 2026-10-30 | P-CSC-3: a functional CSC becomes a milestone once the client's character-creation screens run on the tree; not before ([CSC.md](CSC.md) §6.2) | user |
| D163 | 2026-10-30 | P-API-1: the user's $200 of Anthropic API credits go to (1) a pilot builder run in Claude Code on a separate API key (spend limit set in the Console; the coordinator stays on the subscription; model and tokens logged in the plan beside subscription runs, for the cost per pair gained), then (2) a one-off Batch API triage of the not-yet-exact functions' saved pseudocode (`.work/pseudo/`) with a cheap model: STL / template instance, Westwood library or E&B code, and the class or file its callers suggest; hints for planning only, never names (D56), objmatch / verify decide; (3) the rest as overflow builder capacity (work the weekly window turns away, the D89 Fable queue). Next session (2026-10-31 morning) | user |
| D164 | 2026-10-31 | P-MU1-1: explicit instantiations (`template C *Unknown_Find_Field<C>(...);`) are an accepted matching form for emitting the client's out-of-line template copies (first: ui_manu's 0x5B6DB0, 0x5B70C0, 0x5B7210, 0x5B72B0, probably 0x5B7050), each marked in the source as not the client's form (like aux_roots' SYMBOL_PAD); callers stay unchanged | user |
| D165 | 2026-10-31 | P-SM2-1: FAIL_IF in app/shared/unknown_debug.h becomes `if ((expr) && (Unknown_Fail_If_Report(#expr, __FILE__, __LINE__), 1))` (the app-wide trial: +6, scriptedmovie 0x803310, co 0x6FEC40, ini_manager 0x4EB1B0, px 0x44C2E0 / 0x44C520 / 0x44C690; -3, client_lego_impl 0x72D5F0, co 0x6FFFF0, sclient 0x750260). A builder fixes the three losses in their own files first; the macro changes in the commit that loses nothing | user |
| D166 | 2026-10-31 | Caller tie-break: when objmatch finds one of our functions at several byte-identical client addresses (`multiple` + `identical`, D54's ambiguous column), verify pairs it with the address whose callers are all already paired exactly, in the same file, with the functions that call it in our object (first: ui_cpit's 0x584C80, called only from 0x57DD20; land's 0x47CF50 from 0x47A2F0). An address with callers in several files (the linker's merged copy, 0x474080 with 247 callers) or with an unpaired caller stays ambiguous. Deterministic (passes until nothing changes); `pairs_cmp.py` lists tie-break pairs separately. D66 (vii): a tie-break pair lost because its caller lost its pair is not a loss of its own, named in the commit. The switch lands as one commit with a full verify of all 11 libraries (the one-time gain), apart from any builder's work | user |
| D167 | 2026-10-31 | P-API-2, D163's Batch API triage: one tool, `tools/re/triage.py` (`build`, `submit`, `fetch`, `ingest`, `plan`, `show`, `score`), run by the coordinator only, one batch at a time. Each request holds one function: its pseudocode from `.work/pseudo/` (rebuilt in full first), the strings it uses, its callers' and callees' names and files, the nearest functions with a file above and below it, and its region. `custom_id` is `fn-<ADDR>`, every request stores a hash of its input, and a rerun resends only changed inputs. The batch id is recorded in `.work/triage/batches.json` at submit (results expire after 29 days); raw results are kept unchanged in `.work/triage/raw/`; tokens and cost go to the run's plan log. The answers follow a JSON schema: kind (STL / template instance, Westwood library, E&B, CRT, unknown), suggested library / file / class, confidence, the evidence cited. Rows that contradict a deterministic fact (`__FILE__`, gaps windows) are flagged. A calibration batch comes first: about 300 functions already byte-exact (pairs from imports.json, D164's template copies, LD6's std::sort 0x4708F0 / 0x472910 and its helpers), decompiled with their own name and `__FILE__` strings removed, run on Haiku 5.5 and Sonnet 5.5. The full batch runs on Haiku 5.5 at effort `low` if it gets the kind right ≥ 85% of the time and the file ≥ 50% at confidence ≥ medium; Sonnet 5.5 only as a second opinion on the disputed or low-confidence rows. Hints only (D56): never in symbols.json, the sources, NAME tags or `docs/data/`; objmatch / verify decide | user |
| D168 | 2026-10-31 | P-API-3: the triage table stays under `.work/triage/` (git-ignored, read by builders on this tree), not in `docs/data/`. Revisited once `triage.py score` shows whether the hints hold up (§4's D163 item) | user |
| D169 | 2026-10-31 | D66 moves past `vc6-verify` (amends D157's "any D66 case is merged by hand": the master ruleset requires `vc6-verify` and has no bypass actors, so that path did not exist; first hit PR #39, the pilot's scriptedmovieline.cpp with 0x7FFD30 / 0x7FFDC0 moved from scriptedmovie). A PR whose every lost pair is a move (the address held by another file in the same verify, none lost outright) fails until a maintainer comments `/accept-d66 SHA` naming its head (or runs `verify_pr.py PR --accept-d66`). The service then verifies it again and posts success, the moves and who accepted them named in the status and the comment, and the PR merges or lands like one that lost nothing. A pair lost outright still fails and waits for the user; no command passes it. The command is the user's: agents list the moves for the user and never post it (comments from this host carry the user's login). No bypass actor on the ruleset. `tools/vc6/verify_pr.py`, `verify_watch.py`, [github-plan.md](github-plan.md) §4.5.5 | user |
| D170 | 2026-10-31 | D163's full triage runs on Sonnet 5.5, over the 14,753 functions in the application range only (amends D167's "the full batch on Haiku 5.5"). The calibration (`triage.py calib`, 300 byte-exact functions with their own name, class and paths removed, no neighbours or files): Haiku 5.5 kind 79.7 %, file 40.0 % at medium / high (6 of 15 right even at high), failing D167's gate; Sonnet 5.5 kind 89.3 %, file 63.2 % (19 of 24 at high), passing it. Sonnet costs about $0.0027 a function, so all 20,588 would be about $53; the 5,835 library-range functions (Westwood libraries, which gaps.py already locates from EA's source, and the runtime) are left out, estimate $41, inside the `enb-d163` workspace's $60 limit | user |
| D171 | 2026-10-31 | P-AR1-1: objmatch's D88 self-name round also uses the slots of vtables learned in the same run (from the exact constructors that store them, e.g. 0x702890 stores 0xB01844) and learns the relocations of short functions at addresses the run already knows, breaking the relocation-only ties of template instances whose vtables symbols.json never names (aux_roots' accessor / KVPair deleting destructors and destructors). Measured on objmatch alone (AR1's prototype `.work/ar1/proto_om.py`): aux_roots.obj 102 -> 335 exact, all 241 app objects +318, 0 lost; shipcust_desc's three range credits move to vtable addresses (0x4F7000 -> 0x4484C0, 0x4F7520 -> 0x42F010, 0x4F7550 -> 0x4488E0; 0x42F010 to be checked). Order (the user, 2026-10-31): once the tree is quiet, symbols.json is regenerated first (it lagged imports.json: 10,005 exact against 13,341 pairs), with the full verify workflow.md §3.4 requires; then this lands as one commit with a full verify of all 11 libraries, the moves listed for the user | user |
| D172 | 2026-10-31 | P-API-4 (D168's revisit; replaces D168's ".work only"): the triage table is tracked, written by `triage.py ingest full`: `docs/data/triage.json` (one row per line: address, kind, library, file, class, template_of, confidence, contradicts; 1.2 MB), `docs/data/triage-evidence.tsv` (the evidence each answer cited; 3.6 MB) and `docs/data/triage.md` (counts, the score, the reading rule), each under the pre-push check's 5 MB; `plan`, `score` and `show` read it. Still hints, never evidence (D56, D167): no matching tool reads it. Reading rule: a low-confidence hint, or one for a function at a file boundary (the nearest known functions before and after it in different files), is a lead; confirm the object's boundaries (statics, literals, padding) before placing. Evidence: `score` against what is exact, high 494/494, medium 1,219/1,271, low 60/83; the builders' tests against pairs, both neighbours the hinted file 23 of 23, a file boundary 18 of 23, every miss one object no evidence names (unknown_screenfade.cpp: the hint said a neighbour's file) | user |

## 3. Open decisions

* **P-VM-1** (2026-10-29, coordinator; recommend yes): the build host's VMs need replacing by **2027-01-20**: the
  template `Win10` and the pool's base made from it stop working then, and every clone inherits the date. Rebuild the
  pool with `tools/vc6/setup/New-BuildVM.ps1 -ReplaceBase` ([setup-vc6.md](setup-vc6.md) §5; tested 2026-10-29: 5.5
  minutes, same pairs) on a quiet tree before then, and retire `Win10` as the template (`.work/vc6_config.json` then
  names `enb-vc6-template`)? Alternative: a Windows image without an end date (`-WindowsVhdx`).
* **P-TG3-1** (2026-10-31, TG3; recommend yes): CordCon's three harness counters (AcksMatched_, ResendRequests_,
  Rejected_) move out of the class (a side table in udpreplay), and its constructor takes the key as one `__int64`:
  the client allocates 0xC0, ours 0xC8; this unblocks ACK2 0x85ABB0 (0.61 with both fixed in a draft).
* **P-TG4-2** (2026-10-31, TG4; recommend keep): D130's dead spenders in a NOT EXACT function: ui_avatar_cust
  0x550A10 (NOT EXACT 0.71) holds five dead finds (four until TK6 added 0x5513E0-0x551C80, 2026-10-31 t4) that exist
  only to send matrix3d.h's Rotate_Y 0x551270 out of line, gaining that pair; each is needed (one fewer inlines it). D130 was written for functions made exact; does
  it cover spenders kept for another function's pair?
* **P-TK3-1** (2026-10-31, TK3; recommend yes): extend D166's caller tie-break to `multiple` copies that are not
  identical (same rule: our exact caller reaches one address and every client caller of it is exact in the same
  object; copies differing only in relocation targets are pinned harder by the caller). Measured at objmatch level
  over the verify-app objects: **+82 app pairs, 0 lost** (`.work/tk3/measure.log`; fontdatabase's 0x6C6ED0, 0x6C88D0,
  0x6C9170, 0x6C9920, 0x6C9AD0, 0x6C9C80, 0x6C9E30 among them). One line in `tools/re/objmatch.py` `caller_tiebreak`
  (drop the `get('identical')` filter; `.work/tk3/objmatch_x.py`), landed as its own commit after a full verify of
  all 11 libraries, as D166 was.
* **P-TK3-2** (2026-10-31, TK3 and TK1 independently; recommend yes, lower value): D166 counts an EH unwind funclet
  as part of the function whose FuncInfo references it (unblocks fontdatabase's ~vector 0x6C6E20, called by three
  funclets, and control_keys_manager's 0x71EC90 now / 0x71EDC0 once Load_Definitions 0x71B1F0 is exact).
* **P-TK2-1** (2026-10-31, TK2; recommend yes, with P-TK3-1): objmatch's caller tie-break (a) also runs on D80's
  survivors when they are identical, and (b) the D80 / self-name / range rounds repeat after it, its credits fed back
  into D80. Dry run over every app object: **+45 pairs, 0 lost** ((a) alone +12; attrib_dir's string-hashtable finds
  0x4D8950 / 0x4D8D20 / 0x4D8DF0 among them). Patch `.work/tk2/objmatch_d166x.diff`, gains
  `.work/tk2/objmatch_d166x_gains.log`. Changes D166 / D100; land after a full verify of every library (measured
  separately from P-TK3-1's +82: the overlap is not yet known).
* **P-TK7-1** (2026-10-31, TK7; recommend yes): commit E&B's configfile.cpp + xtime.cpp revision (configfile without
  sectionList and MDC's writers, readFile keeping empty values and joining backslash lines, a new clear 0x8B2690; xtime
  without FormatTime / ParseDate / Print* / getYDay; one change, wdebug.h's ERRMSG ties them). Measured wlib +11 exact
  (130 -> 137 in tree) for **4 pairs lost outright**: 0x77AA10, 0x77B030, 0x77B690, 0x850820, app-object template /
  iostream instances only matchbot's sectionList / writeFile credited to configfile (the client's configfile object
  has none); not a D66 kind. They come back as their app files are built. Stash `docs/attempts/8B2810/`, diff
  `.work/tk7/proposal/`.
* **P-TK5-1** (2026-10-31, TK5; recommend yes, with P-TK3-1 / P-TK2-1): objmatch's caller_tiebreak credits one of an
  object's byte-identical symbols when several propose the same address (the linker folded the four
  Create_NEW<Floor / Ceiling / Closest / Interpolated> into lookup_table's 0x4EE9F0; all four callers exact). Not yet
  measured over the app; land with the other tie-break changes after a full verify of every library.

## 4. Next steps

Outstanding work only (rewritten 2026-10-31: what is done went to the log entries, which hold each item's
history). Milestones M1 and M2 are met (D42, D63; [milestones.md](milestones.md)). Order and detail of the
client's own code: [kernel-map.md](kernel-map.md) §6 and, for step 9's dependencies,
[kernel-map-step9.md](kernel-map-step9.md). Adding a file: [workflow.md](workflow.md) §2.1. Every function
written but not byte-exact, with its cause: `docs/data/not-exact.md`; every abandoned attempt, its blocker and
LIMIT REACHED: `python tools/re/attempts.py list|show` (a parked row waits for a new §3.1 lever). The items
below name only the work items, the blocked work, the leads and the queues.

* **The triage plan's runs (D163; the log entry of 2026-10-31 15:40).** `.work/triage/plan.md` (rerun `triage.py
  plan` after each run): 161 runs of 3+ neighbouring functions with no pair and one suggested file, 72 agreeing with
  both neighbours (the hint's file was right 23 of 23 in TG1 / TG2's six), 76 naming a file not in the tree (new
  files: ui_registration 0x5FFFC0, htmlview 0x7761E0, ui_starbase 0x646F70, ui_ship_cust 0x61E8B0, ...); 2,735 template
  instances; 379 library kinds in the app range to check against gaps. Each run is a bounded builder item: claim the
  bare range (`run_branch.py claim 0xLO-0xHI`). The table is tracked in `docs/data/triage.*` (D172). The $200's
  remaining ~$156 is D163 part (3)'s overflow capacity.
* **symbols.json again** (quiet tree, full verify of every library, workflow.md §3.4) to record D171's +841, then
  `.work/pseudo/` rebuilt.

0. **GitHub (P-GH-10):** the claims and verify service are live on master; the rehearsal waits for the first
   outside contributor, then the public switch: [github-plan.md](github-plan.md) §4.5.7. Runs start with
   `python tools/run_branch.py start` (coordinator-pacing skill).

1. **Work items** (each at its file's next edit, unless it says otherwise):
   * app/packets' "(not rebuilt)" comments for 0x735FC0 / 0x739FF0 / 0x734740 are stale (all three exact); 0x738970
     (CLIENT_DAMAGE) is still not rebuilt.
   * fontdatabase.cpp's stand-in FontTextFileClass is SplitStringClass and ui_line_display.cpp's LineDisplayWrapClass
     is FontSplitStringClass; 0x4FA8F0's 9 other callers and Set_Font 0x4FAAC0's 2 can include
     unknown_font_split_string.h.
   * Client names (client wins): FontStyleClass is the client's `FontSys` (0x6D0A20 calls 'FontSys::Change_Text'
     0x6CDBC0; rename when loadingview.cpp's stand-in `class FontSys` merges into fontmanager.h, P-FU1-1); the global
     Unknown_BDADC0 is the client's AssetManager (its assert; scriptedmovie.cpp's #define goes with the rename);
     SClient::Unknown1338 is EnvironmentMapManager (the client's assert; SClientUnknown1338Class); intrface.h's
     ControlKeysManagerClass is the client's ControlKeysManager. (TK1: 12 files, sclient.h and seven ui_* pairs; no bytes;
     a quiet window.)
   * Merges into real headers: tsk_login.h's CharacterViewUnknown1B4Class is InterfaceClass (intrface.h); P-H9-3 /
     P-H11-1 / P-H12-3 / P-H13-2 / P-T11-1 / P-H18-1 (stand-in names and twins switched to the real headers; Model ->
     RenderObject; LoginUnknown0CClass from inpdsp.h); P-MU1-2 (SClient 0x744560, PDAInterfaceClass 0x695A00 /
     0x695780, UIMeshClass 0x5B9930 declared in their headers instead of ui_manu.cpp's local cast classes); rs_track.h's
     Unknown45D9E0Class / Unknown45DC80Class become ProjectorCasterClass / ProjectorReceiverClass subclasses from
     proj_man.h, the receiver's inline ctor on proj_man.h line 207 when rs_proj_sup.cpp is built (PM1).
   * Types: P-FU1-2 (ManagedFontClass::Id / FontManagerClass's keys as RenderObjClass * instead of unsigned, with a
     full app verify); `ClientClass::Unknown1144` as `HeightFieldClass *`; P-SM1-2 (OrbitalManager's ctor takes a
     shared callback base and ClientClass); P-T12-2 (aux_roots.h's bool `Type_Name` / `String_To_Value` overloads into
     strconv_lib.h through the aux_roots generator).
   * Data and records: the .bss int Unknown_BE71D4 (in postate.cpp) moves to its owner, CLIENT_SET_TIME 0x737960's
     file or sclient 0x7472A0's, when 0x737960 is rebuilt; RenderHookClass's 7-byte destructor 0x6F36F0 wants a D99
     exact row; docs/attempts/726100's stash still says `new PxClass` (PxList); asserts.txt files px.cpp's
     `!(low && high)` 0x44BA70 under dynamic_obj.cpp (a file-boundary error in its generator); 92's sets named Drops
     (+0x10) / Adds (+0x20) from the client's Print: confirm when the handler 0x81F810 is read; P-MV2-3 (anything
     added to duration.h re-sweeps auxpad.py).
   * From the 2026-10-31 t4 run (TK14): ui_avatr.cpp (19 pairs): the window's vtable 0xAEF9BC holds 0x554D60 at +0x60
     and 0x554E80 at +0x68, InterfaceClass's Unknown_60 / Unknown_68: declare them as those overrides when 0x554E80 (the
     50-case gadget handler, 0x10E0 bytes) is rebuilt, then rerun every marker. Open: the ctor 0x553AF0 (0x1030
     bytes), 0x556F80-0x55BAC0's 21 functions (leads 0x55A060, reusing 0x558060's weights pattern, and 0x55B9A0),
     0x55BFB0.
   * From the 2026-10-31 t4 run (TK17): cordcon's write 0x850C70 / buildHeader onto Cord_Put_Small / Cord_Put_Seq;
     pump 0x853560 starts with a test of the byte 0xC40488 before WWPROFILE's string call (the profile macro's form
     differs); examinePacket 0x8514D0 0.73 (ntohs / ntohl arguments through eax, the channel kept in ebp); handlePacket
     0x851AE0 0.21 (the resend path's `CordMessageClass m = *it` copy in another stack slot).
   * From the 2026-10-31 t4 run (TK15): ui_avatar_cust.cpp starts at 0x54B760 (PermStringInitClass static 0xBE5B34;
     0x54B370 is ui_attrb_skills.cpp's, line 577; 0x54B740 / 0x54B750 are thunks into its 0x549800); ctor 0x54B7B0,
     dtors 0x54BA80 / 0x54BAB0. The window's real base is Unknown5BB320Class (sclient.h; mesh at +0x84, dtor 0x5BB3B0),
     not InterfaceClass: rebasing it unlocks the per-frame 0x54CAE0, the ctor and dtors. One register residue (the
     view's Client in edx, not eax) holds 0x54E690 / 0x54E7F0 / 0x54EAD0 / 0x54E950 (0.69-0.85); 0x54EC30 0.77.
   * From the 2026-10-31 t4 run (TK8, TK12): cordcon's pump 0x853560 (0.46), examinePacket 0x8514D0 (0.62 register),
     write 0x850C70 (0.39), close 0x850870 (0.43); the CordCon ctor 0x84F640 (0.15) needs P-TG3-1's 0xC0 layout (it
     inlines Add_Channels(0x40) and builds Stats_ after the channels). galaxy_map.cpp (new, app/common): Add_System
     0x836F30 (lines 159 / 160), Add_Starbase 0x837650 (189 / 190), lookups 0x8378B0-0x837B30, the header from
     0x838050. claims.py resolve / check should count attempts.csv `exact` (D99) rows as done (mainview's #111 was
     handed out though LD4 finished it); LD4's D99 rows 0x514D30 / 0x514D50 / 0x514D70 carry stale names (As_Main_View,
     UIViewClass::Unknown_44 / _58) at the mainview rows' next touch.
   * From the 2026-10-31 t4 run (TK9): landdmap 0x468920 (builds the cells; its corner if-chain on k looks like an
     inline Get_Corner(k)), then 0x467A50; 0x468D50 register 0.88, 0x468760 0.46 (generators in docs/attempts/468760/).
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
     delta_template.h members in a draft but cost aux_roots 6 (workflow.md 3.3).
   * From the 2026-10-31 evening run: ui_chat_panel.h's UIChatComboClass is ui_combobox.cpp's UIComboBoxClass (it
     declares Unknown_57AD50): move the class into a ui_combobox.h, the chat panel using it (P-TG4-1, D137); AvatarUI /
     ShipUI as RefCountPtrs with `.Peek()` in cpacket.cpp's lines 3423 / 3436, dropping 0x69CDE0's cast (TG3). Leads:
     sbaseview 0x69D030 (0x724 bytes) and SBaseView's slots 0x69B5E0 / 0x69C0D0 / 0x69C5A0 / 0x69C5E0 / 0x69C8F0 and
     ctor / dtor; CordMgr's ACK2 0x85ABB0 after P-TG3-1, ACK1 0x85A430, Compute_Ticks 0x85A220 (needs a stub for
     WWOnline's hash 0x8B4A90); ui_avatar_cust 0x550A10's frame-time prologue (register).
   * From the 2026-10-31 day run: UIGroupMemberUnknown2CClass (ui_group.h) merges into ui_cpit.h's
     UICpitUnknownD0Class (one class, ctor 0x5DBFC0; D137); aux_roots' colour 0xB00A00 is probably wwmath's Vector3 (its
     KVPair's Get_Type_Name 0x4B24F0 returns 'Vector3'; keep BLOB_NE's dword operator!=); free leftovers 0x709630 /
     0x709650 (vtable 0xB01FD0) and 0x7099F0 / 0x709A40 (rtti.h casts); D99 candidates 0x463410 / 0x463440
     (Unknown462690Class's deleting dtor / dtor). Leads: ui_manu's money find out of line first in 0x5B4B10 /
     0x5B5320 (DeltaTemplateClass<MoneyClass>'s instantiation state in the client's TU); ui_group 0x5946D0's
     Action2PacketClass string `_Tidy` (case 35-39 or pkaction.h), then 0x593180 out of line; SClient::Shutdown
     0x741BB0 (implicit, inline-shaped dtors of SClientUnknown1334 / 1338 / ProjDecalManager); landdmap 0x466F60's
     spacing temporaries.
   * Blocked: 0x42C330 waits for 0x7D3CC0's file (the account / subscription XML reader 0x7C9000-0x7E3000, between
     ui_starmap.cpp and avatar_pip.cpp, no __FILE__: a provisional file, D137, when someone builds it); `Send_Data`
     0x82D420 (the client tests channel >= 0 first; wlib's dictionary.h needs a wlib + app verify).

2. **Step 9 (D90)**, left in its files (history: the log entries of 2026-10-28 and 2026-10-29):
   * co.cpp: the thunks 0x70E2A0-0x70E2D0 (too short for verify to count); type 28's aim 0x70E850 (frame / shape);
     the base's slot +0x04 0x7251C0 (a pure placeholder) and its attach / detach rows; the damage state 0x6FE1E0;
     0x6FCB70, 0x6FEAC0, 0x6FC9A0, 0x6F3FA0, 0x6F4610, Process_AI 0x744260; 0x6FD310 parked for good (IB3).
     Leads: P-DF1-1 (the shared inline-budget cause: auxdata.h's Unknown_Find_Field against 0x7079B0's size), P-DF1-2
     (delta_array.h's bounds accessor stays co.cpp-local until the 0x6FB1E0 canary is understood), `Unknown_Find_Valid`
     and the inline-member form on 0x6F3FA0 / 0x6FBEF0 / Get_Name. SUBPARTS 180 is NOT_DRIVEN until clienttest's fake
     models get slot +0xF8 and an asset-manager stand-in (P-DM1-2). eff_o's 0x6E8890 frame (E&B's lineseg.h may
     differ: P1 - P0 and the len2 test stay in the FPU in the client).
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
     P-SC3-2 (0x7511A0: one tryv2 round of vector forms, low priority); 0x750C20 is dead code (parked, P-SC3-1).
   * Parked after two rounds: Unknown_50, the fast start 0x76A370, Pre_Run, Run, Start_Game, Go_Next / Back_Step,
     SClient's destructor / Init / Shutdown. D92's Fable round: ClientClass's constructor and Find_Object /
     Remove_Object (item 7).
   * Lead for the rest: the auto_inline lever on other NOT EXACT app functions that call a same-file function out of
     line.

3. **The packet classes:** 260 of 260 declared, about 255 rebuilt. Left: 45 and 117; 0x867250 register (LIMIT
   REACHED); Reset_Keys 0x71C710 parked (inline budget, 4 element forms). Lead (F9): the shared-helper reading for
   the other app/packets files parked at inline budget whose ctors / Builds / Prints loop over containers.

4. **The SGI hash_map's users (D87):** client_lego_impl's Get_Lego_Transform 0x72B000, Child_Moved 0x72CE40, Attach
   0x72D0E0 (Fable-eligible, P-H17-2) and Add 0x72B7F0 (one or two EH states off); control_keys_manager's P-H11-2
   pair and Load_Definitions 0x71B1F0 (Fable-eligible); inpdsp 0x429F60 (register); kv_list's 0x4B6B60 (why
   0x4B7030 is a one-caller find instance); fontmanager's 0x6D0A70 (inline budget); co_player_relationship and
   option.cpp after the eval.

5. **Land and the views (M2.4, D46):**
   * land.cpp: the big functions 0x46F120 / 0x470F80 (a session each); Render 0x474B30 is a structural gap (the
     client's second dynamic VB / IB pass, inlined render-state debug names); Get_Downhill's case 2 / 3 tail merge;
     Get_Triangles 0x465D30 (its second push_back's end() load; Fable-eligible); the decoration appender 0x47C6A0
     (inline budget, 13 forms: Fable-eligible), 0x479D00 / 0x47C580 shape, 0x478B50 budget, Build 0x47A2F0 (parked;
     it can only gain itself, its helpers are `multiple`; its RefCountPtr form parked: the client calls insert(it, n,
     x) 0x47D690 out of line), Load_Assets 0x4691D0 frame, Update_Patches 0x46B3C0 shape, the ini writer 0x4791D0
     frame; the stars' Init 0x4829E0 register, Load 0x481DB0 order; landdmap 0x466780-0x469000 (Matrox only, never
     runs) untouched; the global byte 0xBFAE52 set during the prototype load is unknown.
   * mainview.cpp: ctor 0x50B360 order, Init 0x50BA60 register (`docs/attempts/50BA60`), 0x50D060 register, Update
     0x50F1B0 frame (DurationClass::operator+ out of line costs aux_roots' pad), targeting 0x512400 budget (the client
     calls its find copies 0x514B60 / 0x514BD0), picking 0x512BB0 shape, overlays 0x50D520 budget, sea 0x510D40
     shape, 0x5146D0 shape (the client keeps * 0.01 and * scale apart; same at 0x5C25C0), 0x514010 / 0x6B4480 shape,
     the three Render_*_Mode; 0x513CD0 / 0x513D20 are ambiguous twins.
   * The views: the loading view's 0x691830 (frame 0x548), 0x6912A0 / 0x691320 (the +0x58..+0x64 class, the font
     globals); the other views' constructors / destructors (mv3dview.cpp's ctor, 2.7 KB, and mv3dsnd.cpp);
     sbaseview.cpp (leads 0x6A0590, 0x69CDE0 with +0x184..+0x1A0 as interface RefCountPtrs; 0x69F710 order);
     scriptedmovie.cpp (the dtor 0x8008B0, the loader 0x8099B0, 0x802930 / 0x8059A0 shape, 0x802B20 register).

6. **Step 8's tail:** `Send_Data` 0x82D420 (item 1, blocked); everything else is at LIMIT (Key_Exchange 0x829180,
   Rate Count, the UDP constructor 0x82CBF0 register, Listen 0x82A470 register) or in the Fable queue (TimedLog
   0x824540).

7. **The Fable queue** under D89 / D97 (by unlock value; D136: not pruned, rounds scheduled more often):
   client_lego_impl's Add 0x72B7F0 (P-H20-1's round; Find 0x72ABB0 and Remove 0x72C1D0 are exact since);
   CLIENT_DAMAGE 0x738970 (P-T19-3; FORMATION 0x734740 is exact since); D92's three (item 2); Get_Lego_Transform
   then Child_Moved / Attach (P-H17-2); Load_Definitions 0x71B1F0; P-F8-3 (0x6FBEF0 with a per-field inline
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
   its own statics, no __FILE__; NAME 1).

9. **Tools:** P-LD3-2 (`wt.py bring-over` accepts named D119 ambiguous addresses, checked against the ambiguous
   column) with P-CS1-2 (the same flag for D66 (i) / (v) moves when pairs_cmp's lost-outright list is empty); promote
   P9's worktree scripts (`.work/p9/wt.py`'s leftovers, `checks.sh`, `commit_wt.py`; commit_json.py must keep
   imports.json's maps sorted); the compare-and-swap commit rule (`update-ref master NEW OLD`) into commit_mine
   (H17's race); gen_stubs' by-value returns (P-T18-2); H16's `.work/h16/fm_gen.py` / `commit_h16.py` pattern for
   generated files.

10. **Parked leftovers** (each waits for a lever; `attempts.py show ADDR`): fontdatabase's Load_String 0x6C0430
   register (4 rows with auto_inline(off) around the word parsers), Set_Font 0x6C38F0 register, Load 0x6C52B0
   frame; the inlined StringClass::operator+=(TCHAR)'s base / index swap (LIMIT REACHED; lead: how the client's
   wwstring.h writes `m_Buffer[cur_len] = ch`); Read_Completed_File 0x6AD4B0 order (LIMIT REACHED); the tutorial
   ctor 0x6AC600 / Reload 0x6AD7B0; SoundDefClass 0x43D3E0 and Load 0x43FE30; attrib_dir's Notify 0x4D71D0, Unlock
   0x4D8690, Attach_Mod 0x4D6820 / 0x4D4810; spln_db's Create_Hermite / Create_Color 0x485C10 / 0x487400 (only the
   `new HashNodeClass`'s EH state differs: when VC6 compiles GenericNode's out-of-line ctor, a reasoning lead),
   Get_Color_Curves 0x485440, Create_Blend 0x486200, ~SplineDatabaseClass 0x484810, Resolve 0x487660 (24 masked rows,
   `docs/attempts/487660/`: only the alpha lookup's HashListClass::Find is still inlined); the shaders' `multiple`
   functions (D88), Compute_Caps 0x9B0160, the meshmatdesc noise (D18 / D28); `CreateBrowser`'s app callers 0x7CA5C0
   / 0x7CAD00 (E&B's `querydata`); P-H11-2 (the _Instances destructor frees nodes without element destructors: leave
   unbuilt until another SGI user shows it); `selfname` into imports.json only when something needs it (D88 row);
   same-shape template instances stay `multiple` until their relocation targets have names in symbols.json (P9).

11. **Reminders:** D7's (revisit byte-exactness of E&B's code with the user if it becomes too costly;
   `docs/data/not-exact.md` shows how much functional code waits); Key_Exchange 0x829180 (LIMIT REACHED after three
   Opus rounds; F3's two untested leads are in its attempts.csv row, P-S3-2); which bool-return form gives a bare
   `setcc al` without the `xor`; the inline budget blocks most parked UI rows: a study of VC6's /Ob2 budget (F8 /
   IB3's levers, workflow.md §3.1) is the shared lead.

12. **The 2026-10-30 night's files**, left in each: rs_track.cpp's ctors 0x45FDA0 / 0x45FFB0 (order), 0x460440
   (register; `auto_inline(off)` around it is a lever, P-RT1-1), 0x460E80; 0x462610 / 0x462650 / 0x462690 are
   rs_track's (unclaimed); proj_man's 0x493620 (one register row). ui_group.cpp: 0x5946D0, 0x595530, 0x596770,
   0x597E40 (best: from 0x5979A0 / 0x597A60's forms), 0x592C50 and its NOT EXACT rows. ui_cpit.cpp: 8 parked, mostly
   inline budget (out-of-line Cast / Find_Field / _Tidy). ui_manu.cpp: 0x5B4B10, 0x5B5320 unbuilt, 11 inline-budget
   parked. ui_scut.cpp: 6 parked (the bar's ctor 0x610F80 register, the control keys 0x611300 / 0x611710, 0x6120E0's
   float loads (best lead), 0x612820, _Sort_0 0x6129C0).
