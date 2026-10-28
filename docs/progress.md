# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

### 2026-10-27 (20:30, night): decisions D96-D115, the audit, six tools, objmatch's range / initialiser rounds; +568 pairs, 0 lost

Coordinator Opus 5.5 (D96). Base 6bcc001; HEAD +568 pairs against it (+811 against a76b92e, the day's start), no address
lost outright (one D66(v) move: dbgconn 0x821E40, still matched in connect). Plan log: `.work/run-2026-10-27b/plan.md`.

* **Matching (quiet tree, every library verified in full each time):** D100's range round +192 (ab7656d) and its held-address
  refinement +25 (9339007); D94(b) `_$E` names skipped (fcc5f49, 0/0); D95(b) closed unimplemented (D114(a)); D114(b) the
  range widened by an object's lone exact COMDATs within 0x1000, +245 (0de0e6c); D114(c) symbols.json regenerated
  (10,422 -> 17,300 symbols) with symbols.py keeping a name's vtable / string proof beside `exact`, +72 (ae85fb4: the
  plain regeneration measured +63 / -323, it erased D88's evidence); D115's initialiser round +7 (092776b: 0x709F40,
  inpkeys 0x42B420 / 0x42B540, permstr 0x4AE2D0, 0x729B00, 0xA06000 / 0xA06020). objmatch's order: D80, self name, D80,
  range, D80, initialiser, D80 (tools/re/README.md). `.work/pseudo/` is deleted: rerun `python tools/re/pseudocode.py`.
* **Builds:** packets 45 (2d2dba7) and 117 (32da0be); F6's Fable round (D97) found E&B's ref_ptr.h has no
  `G_ASSERT(dummy == 0)` (964f95e: client_lego_impl Find exact; every library 0 lost); F7 trimmed ref_ptr.h's comments so
  operator*'s assert sits on the client's line 356 (92e0e2d, 0/0: its two users are not built yet) and made Remove
  0x72C1D0 exact (595c6b1, +5). Add 0x72B7F0 stays parked.
* **Tools (D112, D113):** private scratch per agent and `importer.py --agent` (999f780); commit_mine by entry and by row,
  `--refresh-totals`, compare-and-swap (3b6d6fc); the root guard (420f76e); `appfile.py match -I auto` / `--pool`
  (3ed6d2b); names.py over untracked files, refreshing not-exact.md (cdf3e23); `tools/vc6/wt.py` new / bring-over /
  remove / list (67c2b7d; a real landing on master is still untested); pacing.py's lost-outright check counts an address
  still matched anywhere as a move (5b9d86f). About 30 trap bullets retired (42ff582).
* **Decisions:** D96-D115 (§2); the audit's amendments are D110. Two audits (decisions; traps -> tools) and the Open-BFME
  comparison (they claim an address per ledger row and verify bytes there; ties never arise) are in the plan log.
* **Late (20:40-22:30), all decided:** D116 (D115 credits by symbol across objects), D117 (pairs_cmp prints
  `lost outright` and `moves`: ed1945d), D118 (evidence-backed pins measured by M4 in a wt.py worktree, caller in
  object +655 / 0 lost, placement 2.4 % wrong; kept in tools/re/experimental/pins/, not adopted, e4e5ae2). Project
  settings committed (3bc25eb): claude.ai connectors and Claude in Chrome off, 120 non-PE REA tools denied. §3 is
  empty. HEAD +811 pairs against a76b92e, the day's start, no address lost outright. The eval branches and
  worktrees (`.work/eval/*`) go after 2026-11-03 (`wt.py remove` / `git worktree remove`).

### 2026-10-27 (17:30, evening): the D93 model evaluation, eight runs scored; A-opus and B-opus merged (+113 pairs, 0 lost); P-E1 proposes the D72 revision

Coordinator Opus 5.5 (the user, 14:50: changes eval-d93.md's "the coordinator stays on Fable"; noted in D93's row).
Base a76b92e; eight runs in worktrees `.work/eval/<run>` on branches `eval/<run>` (`.work/eval/setup.py`: junctions
for locks / pool / pseudo, dgrams, libraries). Every branch rescored by the coordinator (`.work/eval/score.sh`: verify
`--lib app --pool 1` in the worktree, pairs_cmp against the base and against the branch's own imports.json, replay,
udpreplay `conn alex3/alex7` for task A). Logs: `.work/eval/log.md`, `.work/run-2026-10-27/plan.md`. Memory, not
usage, limited the run (VS Code's cpptools held ~4 GB indexing the worktrees): wave A started together; wave B
started in three steps (B-opus + B-sonnet-1 15:55, B-sonnet-2 16:10, B-haiku 16:30) with the same brief and base.
Usage at 17:29: session 27 %, week 8 % (HEAD +185 pairs against a76b92e, 0 lost).

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
  replay 0 differences everywhere, udpreplay conn alex3 / alex7 PASS on all four A branches. The dishonesty is in
  the self-reported token counts (off by -35 % to +110 %, both directions) and one wrong harness claim (A-haiku).
* **Task A:** Opus and one Sonnet run reached the same 26 addresses; Opus in two thirds of the time with a little
  more tokens; the second Sonnet run missed one destructor; Haiku 19 addresses, two parked and an object split.
  **Merged: master fast-forwarded to eval/A-opus (ae9a0a2);** the other branches gain nothing A-opus lacks.
* **Task B:** B-opus +86 (Load, Save, the 20 KB Set_Defaults exact; extent 0x7F0F60-0x7FEBE0); B-sonnet-1 +36 for 1.3x
  the tokens. **Merged: B-opus cherry-picked onto master (85400d9, 62e999e, 57f25cc)**; master verify after both merges: app 3,388
  functions, +122 / -9 rename moves against a76b92e (112 addresses = 26 + 86), 0/0 against the committed imports.json,
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

* **Addendum (19:00): after the eval, nine Opus builders on master, 42 commits since a76b92e, app +243 pairs / -9 rename moves
  (194 addresses), 0 lost; a quiet-tree verify at 7480bea matched the committed imports.json (0/0), replay 0, vidtest
  links, totals refreshed (693e7fc: 3,463 in tree).** R1 co_player_relationship.cpp 0 -> 31 (handlers 137-139, the
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

Coordinator (Fable), 04:30 to 12:30, the user online 04:35-09:30 and then away. The API session cap stopped
five builders at 06:05 (the user chose to spend the session window); all five resumed from their transcripts
at 08:11 with their leases intact and lost nothing. The pool grew to seven slots with 2 GB startup clones
(the host's memory, user); the day ran up to seven builders at a time. Totals since the 04:40 entry:
270 commits, app +1,941 (file, address) pairs with no address lost (the 36 pairs_cmp "lost" entries are the gameedit renames' moves, D95(c)), app client functions ~2,350 -> ~4,300 in 201 files (client/ 16 .cpp, packets/ 124); the whole run from 684cec2: 393 commits, +3,009 pairs, only the two D66 land copies lost. The run plan and log: `.work/run-2026-10-26/plan.md`.

* **D88 and the quiet window (04:24-04:31):** the self-name tie-break, +368 pairs, 0 lost (cbd1292 the
  snapshot, 3364636 the rule); the item 5 tools (D78 pointers, tryv2's EXTRA_INC; D74/D77/`--pool` were done).
* **Step 9 (D90), T5/T6/T7/T8/T9/T10/T11:** 9a kcom (authlogin.lib from a .def, AuthLoginResponseClass 2 of 2),
  9b the login data objects (the ticket and character slot 13 of 13, the galaxy entry, avatar_desc ~38 pairs,
  shipcust_desc 12 of 12, the character creator), 9c ClientClass (client.cpp, 0x1150 bytes, 23 slots, 22 of 28
  exact), 9d SClient's skeleton (sclient.cpp on its 0x14F8-byte layout, SClient : ClientClass, SOptionsClass;
  the constructor, Process_Packets, the pump and three overrides exact; Init / Shutdown functional), soptions.cpp
  7 of 7, cpacket.cpp's handlers (T8: 30 handlers, 29 exact; T10: see below), co.h (ClientObjectClass out of
  client.h; H13) and co.cpp (T11, below), and **the clienttest harness** (T9, `tools/harness/clienttest.py`:
  client.cpp, sclient.cpp, cpacket.cpp and the packet objects with logging stand-ins; 38 checks; alex3's first
  1,000 messages drive the real handlers into a trace checked against expected.txt). kerneltest 81 of 81
  throughout; VIDTEST's link, broken by ee8fa6f's stub, restored in 0f87385 (stand-ins live under
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
* **Tools:** the pool's 2 GB startup and cap 7 (bf45d2d), vc6.py's junction fix (ab8355a), build.py's UNRES regex
  (6b4c7ee), udpreplay's D91 and D95(a) rules, clienttest. Every builder's levers and traps are filed in
  workflow.md §3 (about 40 new rows and bullets today).
* **Decisions:** D90 (9a-9d), D91 (udpreplay's size rule; Clamp_Int declined; P-L4-2), D92 (P-T6-1's Fable round
  after F5; clienttest deferred then accepted), D93 (**the model evaluation: the next session's first item**,
  [eval-d93.md](eval-d93.md)), D94 (0x85CB00 as D66(iii); the `_$E` name rule; clienttest), D95 (the
  recorded-bytes list; the implicit-destructor twin rule; renames as D66(iv)). Open for the user: §3.
* **The last hour (11:00-12:05):** T11's co.cpp (+42: 34 members, the vtable through +0x44, the 0x188-byte layout), T12's
  objmatch fix (f308ceb: D80 read the client's `__FILE__` only to our literal's length; relative header paths never tied,
  +16) and its aux-data lookup template (T13 landed it with clienttest's link restored, 2207aa8, +4), P7's seven small
  packets (+63; 129-133 complete the object 0x89FB50-0x8A14D0), P8's P-P7-1 trial (the implicit ShipCustDescClass
  assignment kept at +0/-0; the char[] layout would trade +3 for tsk_login's constructor: P-P8-1), H15's const_iterator
  batch (4 of 4), H14's fontmanager.cpp (+23) and control_keys remainder (+16), the tools fix P-T9-1 (6b4c7ee).
  Incidents, all resolved: a builder's edit script truncated the shared attempts.csv (restored from HEAD, trap filed);
  clienttest's link at HEAD depended on a stale build/app/app.lib and on P7's files pulling shipcust_desc.obj (T13:
  `build.py build --lib app` first; udpreplay's shipcust stand-in staged for clienttest). Every VM is released; the
  working copy's uncommitted imports.json totals hunk (commit_mine never commits the totals lines) was discarded: the
  next quiet-tree verify refreshes them. Usage at handoff: see the plan log's last heartbeat (weekly ~73%).

* **The afternoon (12:20-14:05, the user: "use all our quota before reset", then "don't start anything new"):**
  seven more builders on disjoint files, 81 commits, app +234 pairs, 0 lost (final totals since the 04:40 entry: 352
  commits, +2,175 pairs; since 684cec2: 475 commits, +3,243 pairs, only the two D66 land copies lost). T16: ten more
  cpacket handlers (64, 17, 37, 62 exact; the position-update family NOT EXACT on one Is_Valid_Float residue, P-T16-3);
  clienttest drives 26 handlers over alex3's first 1,000 messages, trace 1,952 lines. T17: rtti.h's type test (a
  class-static cast; the nesting-depth budget lever) made the parked co.cpp lookups exact, and the client object's
  0x4C-byte base UnknownB02C90Class is in client.cpp (constructor / destructor exact). T18: sclient.cpp 22 -> 52 pairs;
  Do_Frame / Render / chat wait for their clienttest stubs to go (P-T18-1). H17: client_lego_impl.cpp 0 -> 57 pairs
  (LegoClass bases, bone maps, knobs, orphans; three Fable-eligible). H18: app/input/inpdsp.cpp 10 of 11 exact (+17;
  the registry read only, unreachable from tests). P9: the login packets 111, 119, 2, 118, 112, 113, 114, 145 (+90) and
  most of P-T5-2 (the visitor merge is P-P9-2). Tools: commit_mine selects imports.json hunks by file block
  (`--file`, 3772169), attempts.py writes the csv's own line ending (e7995bb). Two incidents, both repaired by the
  builders and filed as traps: H18's zero-context imports.json patch (89315ee) and H17's private-index commit racing
  T18's (a6e1eb1; the compare-and-swap rule). New for the user in §3: P-P6-1, P-T12-2, P-P9-1, P-P9-2, P-T16-3,
  P-T18-3. Every VM is stopped; the tree is clean.

### 2026-10-27 (04:40, overnight): twelve builders in parallel; app +1,018 pairs, 0 lost; step 9's tasks, the SGI hash_map, land, 190 packet classes; D85-D89

Coordinator (Fable), 2026-10-26 21:30 to 2026-10-27 04:30, the user away from 23:05. Tree 684cec2 -> HEAD before
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
  0x87F320 (shape), 219's two ctors (order-only; P4's lever "Unknown68 as two ints" fixed 108 in 7d3e7fc).
  59 Server_Ready is 5 of 5 exact but parked in `.work/p4/park59/` behind P-P4-1 (§3).
* **Step 8's tail, S3** (Opus, ~450k): `Pump_Pending` 0x82AA10 exact through an inline RSA decrypt, the UDP
  constructors and listener exact once `CordMgr` got its key pair (+0x5C, 0x95C bytes; `unknown_rsakey.h`),
  `Remove_Dead`, the rate destructor; +19. Key_Exchange parked at 0.87 (inline budget, LIMIT under D89).
  Seven code-generation levers filed in workflow.md §3.1.
* **Step 9, T3 and T4** (Opus, ~545k + ~475k): tsk_game.cpp complete (ClientTaskClass, 16 of 16; Init through
  D40's dead-call lever), tsk_login.cpp 48 functions exact (the login packet handlers, both constructors with
  the five 0x160-byte ship-customisation objects, the avatar copy, the galaxy list), the kcom ticket and fast
  start NOT EXACT; +86. Six LoginTaskClass functions parked after two rounds (register-only or inline budget).
  The mapper's `docs/kernel-map-step9.md` (7695c98) sizes what step 9 still needs: client.cpp's base class,
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
* **Tools:** commit_mine.py's -U0 hunk placement fixed (af0a8a5: a skipped earlier hunk shifted an insertion into
  another imports.json entry; 139c184 had broken the file), explain.py's window ends at the nearest verified or
  proven start (e2e8e7c: Create_Device reads identical), `attempts.py` LIMIT REACHED and `check --lever` (D89).
* **Decisions:** D85 Open-BFME's method (not its code), D86 step 8's provisional names kept, D87 SGI hash_map,
  D88 the self-name tie-break (carried out in the quiet window after this entry), D89 the Fable limits.
  Open for the user: §3.

### 2026-10-26 (21:25): Open-BFME's method adopted (D85); P-P1, P-P2 accepted, P2 kept; packet round 3 and step 8's tail launched

Coordinator (Fable). The Open-BFME-1 assessment of 2026-10-25 (a read-only look at github.com/Open-BFME/Open-BFME-1
@ 7b8a0f5: BFME's DX9 ww3d2 under MSVC 7.1, so its bytes, layouts and names prove nothing about E&B's client;
its two register-allocation levers tested on Make_Screen_Shot gave our U form again) is recorded as D85: nothing
of theirs enters the sources or roots, their method does. Carried out by Opus early on 2026-10-26, before the night's
builders ran: the attempt log `tools/re/attempts.py` + `docs/data/attempts.csv` (b49ff3f), the near-miss classifier
`tools/re/explain.py` (70784d3), the switch sweep `tools/vc6/flagsweep.py` with tryv2.py (996b68a), and workflow.md
§3.1 as a symptom -> lever -> proof table with explain.py's triage order and the two-attempt stop rule (b23c08a).
Builders P and S already used it (one attempts.csv row each, cited in their reports). The scratch clone is deleted.
builders.md §3 now asks each report for the attempt rows written and explain.py's class per NOT EXACT function.
User decisions: P-P1 and P-P2 accepted (packet round 3: 62 ADVANCED_POSITIONAL_UPDATE, effects 9 and 11, the
udpreplay stubs for RELATIONSHIP 137); P2 kept (D86); D7 stays enforced (byte-exactness is the goal and the
measure). Launched: the packet builder (P-P1, P-P2), then step 8's tail (`Pump_Pending` 0x82AA10) once the packet
builder's first commit lands, to keep the shared-record window short (§3.8's imports.json trap).

### 2026-10-26 (07:57): packet classes round 2: 91 -> 114 classes, +159 pairs, 0 lost; udpreplay alex3 7,408 -> 19,084 messages

Builder P (Opus, ~400k tokens over two runs, the first cut at 01:10; six commits dd0ca0e, 1b5dda9, da95d03,
6ce1700, c1bd9fe, d95d5e8, each gaining only, replay 0 differences, `udpreplay conn` PASS on alex3 and alex7).
By hand: DirLock 223-226 over their base (vtable 0xB0A290), GlobalLock 227-229 (0xB094F4), AUX_DATA 27 with
PLAYER_VAR_AUX_DATA 28, GALAXY_MAP 151/152, CLIENT_SOUND 106, WARP_INDEX 156, and the client's pkflag.cpp
(`FlagPacketClass`, vtable 0xB0A3D4: the base of effects 9, 11-14 and ADVANCED_POSITIONAL_UPDATE 62).
Decoder: pkdecode/pkemit read a Build whose opcode comes from `Get_Type` (slot +0x28; 10 classes in 7 files,
da95d03), reach the last packet object (the old bound 0x8B1EB0 was exclusive), and keep a `HAND` set of
files pkemit never rewrites. Round trip: alex3 7,408 -> 19,084 messages, alex7 1,622 -> 3,618. NOT EXACT:
AUX_DATA's Print 0x8770A0 (register order, two forms, `attempts.py show 0x8770A0`). Pair counts include
PacketClass's inline duplicates 0x52B210-0x52B350 (five per new file, D20). dd0ca0e's and 1b5dda9's
subjects overstate the class count by two (real: 91 -> 95 -> 100; da95d03 records the correction).
Not started: 62 ADVANCED_POSITIONAL_UPDATE (13,833 messages, the largest gain left: needs the flag builder
0x8AD300, the size function 0x8AD220, asserts on pkupdate.cpp lines 277-285), 137 RELATIONSHIP (1,544
messages: a Reaction class 0x848C80 and strconv helpers 0x835890 / 0x4A30A0 that udpreplay would need
stubs for), 173-177 GroupServer. 146 classes still don't decode (33 fail on the destructor, 18 have an
intermediate base, 16 store unknowns in the constructor, 13 fail on Print, 19 have length-counted buffers
through `movsx` / `mov [reg]`: the most regular decoder extension). Proposals P-P1, P-P2 in §3. Traps filed
in workflow.md §3.6 and §3.8.

### 2026-10-26 (07:54): step 8's tail: P1 applied, the TCP listener's key exchange built; app 1133 -> 1252 pairs with P's packets

Builder S (Opus, ~1.1M tokens over two runs, the first cut by the session limit at 01:10; seven commits
6afe590 ... a57949f, each gaining only, replay 0 differences, `udpreplay conn alex3` PASS). P1 applied
(6afe590): `~ForwardingConnectionClass` inline in connect.h, dbgconn 0x825170 / 0x8251A0 gained, 0x821E40
kept thanks to 78f5f28's tie-break. e7e39b7 refreshed imports.json from a full verify (+4 stale pairs).
tcpconn.cpp: `Key_Exchange` 0x829180 functional, NOT EXACT (one `_Tree::_Inc` the client inlines, a
cumulative inline budget; `attempts.py show 0x829180`, 16 forms, `.work/s1/vars1-7.py` + `vk.py`); the
listeners' `std::set` 0xBE91A8, `Pump_All` 0x82A990, the listener destructor 0x82A2C0 / 0x82A2F0 (695da57);
the listener constructor 0x829E50 with `RSAKeyClass`'s SSH1 key-file loader `Read_Number` 0x82C260 /
`Compute` 0x82C330 (828c5a5, the matching form is `KeyPair.Load(file_auto_ptr(...))`); `Send_Key` 0x82A830
(5e7f96d, its record completed by a57949f). Two shared-record sweeps happened (a hand-built imports.json
left in the working copy was taken by P's da95d03; a verify's rewrite was taken by 5e7f96d): both repaired
in later commits with nothing lost (695da57 restored P's 68 pairs; P's c1bd9fe gives the pkflag pairs their
source). Traps filed in workflow.md §3.1 and §3.8. Left of step 8: `Pump_Pending` 0x82AA10 (the same key
class, best lead), dbgconn's counters, the UDP constructor 0x82C680, the ArrayList copies, the NOT EXACT
list. `RSAKeyClass`'s `Unknown200..Unknown800` members stay NAME 2 until a string names them (P2's rule).

### 2026-10-26 (00:05): kernel-map step 8 nearly done: app 1028 -> 1133 client functions, +113 pairs, 0 lost

Builder (Opus, ~720k tokens; 13 commits 1a20cd8 ... 11ecd72, each gaining only, replay 0 differences,
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

Builder (Opus, ~450k tokens, cut once by the session limit and resumed): 9843588 E&B's own `tcpcon.cpp`
and `tcpqueue.cpp` rebuilt in `libs/wwonline/wnet` (TCPCon 18 of 18 exact, TCPQueue 7 exact plus one too
short to count; wnet 8 -> 41 functions, wlib 76 -> 78, app 1008 -> 1009; E&B's `CONNRESET` -16 state,
wlib's `msleep` 0x8B8270 and `growVector(howmany)`; wnet builds with /GX), then step 8 in app/common:
5f8ba3c tcpconn.cpp's two remaining constructors (by host name 0x828C80, over an accepted socket
0x829C80); 916b335, 1c44a78, 08e4ab3 `ConnectionManager` declared in connect.h with 17 methods exact
(app 1009 -> 1028). Every commit gains only; replay 0 differences; `udpreplay conn alex3` PASS.
Not exact: `Try_Connect` 0x8290A0 (the client repeats the `Handle = 0; return true` tail; 0.75 after
three forms). Not started: `Add_Connection` 0x822CC0 (needs the peek/stats wrapper classes, vtables
0xB08590/0xB0854C, and ArgvClass), `Accept_New` 0x822300, the factories 0x821000/0x821450, the RSA
exchange 0x829180, memconn.cpp. D84 records P-B1 (`~TCPMgr() throw()`); P-B2 (per-file flags) declined
as not needed. Earlier the same night: the screenshot residue parked (4246727).

### 2026-10-25 (night): four parallel builders; +654 pairs, 0 lost; D73 imported; 91 packet classes; D80-D82

The first D62 run with four Opus builders at once on disjoint file sets (`.work/run-2026-10-25/plan.md`:
models, tokens, results), started from d530e88. `pairs_cmp` d530e88 -> HEAD: **+654 pairs, 0 lost**
(app 545, wlib 75, binkmovie 24, wnet 8, ww3d2 2). About 1.75M builder tokens.

* **A, D73 imports** (ffb80d4 ... 9f2cff1): WWOnline's wlib at `libs/wwonline/wlib` (74 then 75 pairs:
  E&B's `Xtime::getTime_t` 0x8BA510 rebuilt), wnet (matchbot's udp.cpp, RenegadeGR's tcpmgr; 8),
  Renegade's BinkMovie at `binkmovie/` (21 functions, 24 pairs). The critsec, dictionary, CordArrayClass
  and Wtime stand-ins are gone (D67; every Wtime address was wlib's Xtime). Kernel-map step 8 started:
  `app/common/tcpconn.cpp` with 8 exact. RenegadeGR's tcpcon.cpp matches nothing: E&B's TCPCon is its own
  (an OutputDevice with a lock, writing through tcpqueue.cpp), so it is rebuilt next (P-A3).
* **B, packet classes** (f356388 ... ab76321): clientstub.cpp's 149 stubs exact in one commit; pklogoff by
  hand; then a decoder, `tools/re/pkdecode.py` / `pkemit.py`, drafted 91 of the 263 classes into 56
  app/packets files, 395 of 396 functions exact (one NOT EXACT constructor 0x867250). udpreplay is now a
  round trip through the rebuilt classes: alex3 7,408 and alex7 1,622 messages byte-identical. 251 exact
  functions are uncounted as `multiple` (one-int constructors and Prints look alike): D80.
* **C, WW3D2** (f9ae6b9, 95f350d): `PointGroupClass::Render` 0x954960 and `Process_Foreground_Load`
  0x949C60 exact (ww3d2 2475 -> 2477). Make_Screen_Shot and Update_Movie_Capture share one 4-instruction
  load-order residue (a Fable puzzle under D72 after two Opus rounds); the distortion draw is an
  inline-budget effect with no plausible source form yet.
* **D, app leftovers** (27812b5, c0ae2f0, 2c12eca, b7cf527): operator delete 0x4E9D10,
  `DeltaCompoundClass::Add` 0x4A5670 (a dead `size()` read spends the inline budget: D40 closed), the
  Mounts array constructor 0x706F60 exact; the assert reporters exact but `multiple`; Ship's BuffArray and
  Buff elements modelled as the client's classes (neutral, D66). D75's premise corrected (D81).
* **Coordinator:** D80 (objmatch tie-break by relocation target), D81, D82, D66's neutral-fidelity clause,
  D18/D28 confirmed as operand-order noise (cface4a); traps filed in workflow.md §3.1/3.3/3.4/3.8;
  `tools/vc6/commit_mine.py` promoted from builder C (95aa10f) after the shared git index swept staged
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
`FreeTextureRam=4284481536` (a 4 GB card) to -10485760 and dividing by 10^6: the 2004 tool's limit, not
a defect of ours (the original VIDTEST reports the same figure on this card; `vidtest_e2e.py` emulates
the wrap). The user cancelled at "Press OK to save settings". Registry: the WOW6432Node, VirtualStore
and 64-bit `Westwood Studios` keys were exported to `.work/regbackup/` before the run and are
byte-identical after it. No message box appeared. M2 is complete on all four gates.

**D53 seen in the game.** The user's client had `TextureReduction=2` in both Render keys (the real
WOW6432Node key and its VirtualStore copy), written by the original VIDTEST's signed comparison on a 4 GB
card: every texture loaded at a quarter of its size. With the value set to 0 (the user, by `reg add`,
backups in `.work/regbackup/`) and the original client relaunched, screenshots of the same views
(`.work/texdetail/`, local) show the planet in Endriago at about twice the fine detail (edge energy
2.58 -> 5.01 on the cropped planet), the ship hull and the login-screen avatar modestly crisper (1.09x,
1.11x: their textures are small to begin with). So the original tool's bug cost players most of the
planet and nebula detail on any card over 2 GB; our vidtest's fix (D53) is the setting to ship.

### 2026-10-25 (later): the C# proxy audited, renamed the reference decoder (the reference decoder), its test-harness repo retired; EnB paths follow

The D79 question was settled as an audit of `the reference decoder` plus the first housekeeping there (that
repository's `docs/progress.md` has the detail). Four read-only audits (architecture and code quality,
docs vs code vs EnB's findings, harness-merge completeness, rename inventory; Opus 214k + 259k, Sonnet
126k + 99k) then: its 2026-10-22 work committed as found (d41ff27, 277 tests), `the reference traces`
retired (its 12 commits were already subtree-merged; 22 recordings hash-checked into the proxy's
`captures/`, 217 MB of replay runs discarded; c4a48c3), the rename to **the reference decoder** (folder
`C:\EnBDevelopment\the reference decoder`, `the reference decoder.slnx`, `the reference decoder.exe`, `the reference decoder.Proxy.*`, `antlink`, pipe
`the reference decoder.Link.v1`; f84a591, d16bf94, ccf8567; `the reference decoder`, `Net7.Protocol` and every citation of the
original 2.46 unchanged), its design.md split into seven docs with EnB's corrections applied (bbfc74e,
16c9dc6, a792172), and a private tool's coordinated change (edc103e its own 2026-10-22 work, 1036d23
the new exe, pipe, log names and the old launcher id read as an alias).

* **EnB's findings that superseded the proxy's docs** (now cited there): the EA-UDP datagram layout
  (`app/wwudp/cordcon.h`: CRC-32 at 0x8B3EC0, flags byte, ack count, per-datagram RC4 key, the ack list
  the proxy's `the reference importer` never decoded), the packet factory's 263 opcodes above 0x0FFF, the client's
  class names for 0x21/0xB7/0xB8 and the 0x9E/0xA0 split, LoginTaskClass's vtable at 0xB040D4 (its
  login offsets were 8 too high), 11 AuxData roots not 13, difference 4 in library-matching.md §3.8.
* **What that project knows and EnB's docs don't** (for later): the login and character-select screens
  (`docs/research/client-ui.md` there), the action message layouts, authlogin.dll's flags and offsets,
  the MVAS addresses, the 2.46 opcode census, byte-order fixes, AuxData behaviour per ship kind.
* **EnB fixes from its audit:** prior-work.md no longer claims an EA-UDP transport exists in the proxy
  (only the `the reference importer` importer does); tools/harness/README.md's EA-UDP table says flags (not type) and
  names the ack entry's first byte as cordcon.h does (kind 0/1/2), and cites auxdata.md §7 items 1-3
  (not §7.1-§7.3); toolchain.md §3, `roots.py` (`REF_TRACES_ROOT` is now the proxy's `captures/`),
  `pkfactory.py` and `refs.sh` point at `the reference decoder`.
* The proxy's re-architecture (server profiles, client-side TCP and EA-UDP, patch manifests) is a
  proposal with decisions A1-A12 pending in that repository (`.work/audit/architecture-proposal.md`);
  its EA-UDP transport will be built from `app/wwudp`. Not EnB work.

### 2026-10-25: housekeeping audit; D62-D79; builders.md; traps 100 -> 74; milestones split; tools promoted

Four read-only audits (`.work/audit/traps.md`, `decisions.md`, `tools.md`, `startup.md`; two on Opus,
two on Sonnet) then three edit agents on disjoint files. Found: D32 (serial builders) contradicted the
approved parallel runs; D23 (NDEBUG off) contradicted D61; D36 vs the E&BConfig check; D7 vs `NOT EXACT`
code beyond D42's scope; D39 as written didn't compile; D10/D16 vs D56; the builder brief, the pairs
comparer and several standing safety rules existed only in the git-ignored `.work/`; a builder read
about 105 KB (27k tokens) at startup, 82% of it milestones.md and workflow.md §3.

* **Decisions** D62-D79 (6477bb8), including yesterday's proposals: D62 parallel builders (replaces D32),
  D63 the E&BConfig check is one-time, D64 `NOT EXACT` general with a tracker, D66 pair-loss classes
  (0x8C1520 is class iii), D69 safety rules numbered, D71 attribution from the harness, D72 subagent
  models, D73 imports (wlib/wnet, tcpmgr/tcpcon, BinkMovie; CordArrayClass's remove on wlib), D74-D78
  tool and define items, D79 the the reference decoder note. D10, D16, D23, D32, D36, D39, D43 reworded.
* **[builders.md](builders.md)** (75920e0): the tracked builder brief: startup set per kind, rules, the
  model table, report format. CLAUDE.md names the pool, `pairs_cmp.py`, D27, D69, D71.
* **workflow.md §3** (a0121ec, b619ec3): 100 traps / 37 KB -> 74 / 18 KB in §3.0 Everyone plus nine
  topic buckets; 24 duplicate groups merged; §2.1 fixed for D41/D61; tool quirks moved to the three
  READMEs (0386ac4). kernel-map.md: the four "WM_QUIT sets +0x0C" claims corrected, header current.
* **milestones.md** 51 -> 20 KB (c826e08): one heading per item; results in library-matching.md §3.8,
  the decode-path table in [decode-path.md](decode-path.md); §4 here collapsed (76e8472); the overnight
  reports archived under `docs/archive/` (65956fb).
* **Tools** (79a79ec ... cba6d0d): `tools/vc6/pairs_cmp.py`, `tools/re/roots.py` (every external root;
  toolchain.md §3), the rerunnable night scripts promoted (zhall, dated, strscan, mark), `notexact.py`
  -> `docs/data/not-exact.md` (38 functions in 15 files), `tools/vc6/README.md`; `compare.py` retired
  (its staged deletion landed in 6477bb8). vidtest_e2e/exitcodes kept separate (little shared code).
* Startup set per builder now about 43 KB (app), 39 KB (library), 35 KB (harness).
* the reference decoder `docs/research/auxdata.md` §7 item 9 added in that repository's working tree (D79).
* Models and tokens: audits Opus 107k + 127k, Sonnet 96k + 78k; edits Opus 134k, Sonnet 168k + 108k.
* Open: the Net7 repositories question (D79) before housekeeping closes; the supervised E&BConfig
  acceptance check (D63).

### 2026-10-24 (evening, unsupervised): M1 and M2 met; transport, kernel, connections, packet factory; app 251 -> 458

Plan and log: `.work/milestones/plan-7h.md`. Two builders in parallel, coordinator heartbeat.
Both builders were cut off by the API usage limit 19:40-21:10 and resumed.

* **M1 met**, gates (a)-(d) ([milestones.md](milestones.md) §2): M1.4 rebuilt `app\wwudp` (CordCon,
  CordMgr, bandwidthaccounting.cpp; 29c3245 ... e0e82f8, addc2ad): the Alex datagrams re-encrypted
  with their keys go through our CordCon: alex3 26,106 messages in order, all 14,812 client ack lists
  accepted, the peer accepts all our datagrams, 54/54 handshakes (WWLib's XMP at run time); the
  `loop` test over a lossy link delivers everything. M1.5 (26e8ed7, bcdb31d): every Net-7 AUX_DATA
  decodes and equals the reference decoder's aux-log, Net-7's known differences counted, a fourth one found.
  The client's TCP transport is app\common\tcpconn.cpp on WWOnline's TCPMgr/TCPCon (D47 finding).
* **M2 met** apart from the supervised E&BConfig run: land matched from the original's draw calls
  (a d3d8 proxy dump: 3a37637, 2e216a9, c00bf3c), frames 500-900 within 1.7-3.2%; gate (d) clean in
  a checked build (78644e0). D61 NDEBUG (2fade9a).
* M1.3 leftovers (f2f2a08 ... c6867b3): the update collector, change table, root encode, second root
  class, policy.cpp; client-path re-encode of the EA captures 39,716 / 43,978 byte for byte (the rest
  differ only in server-policy fields). One accepted loss, 0x8C1520 (a false match while MoneyClass
  was a bare __int64; library-matching.md row): for the user to confirm. Root constructors (d64612e
  ... a67e4b6): Player 0x752510 and the factory 0x6F6000 exact (VC6's inline budget model, workflow.md).
* Kernel ([kernel-map.md](kernel-map.md), 8ab13d1): kernel.cpp (cba72d7, 18 exact) with kerneltest;
  the connection layer circbuff/connect/udpconn (3742a73, 9026165, ff8123e); MenuTask and the movie
  tasks (faecc74; movietest plays the game's Westwood logo through BinkMovie); pk.cpp's helpers, the
  factory 0x8696C0 and its 16 KB switch 0x869790 byte for byte with 263 generated packet classes
  (38f055c, c105a36, e6fea6e, `tools/re/pkfactory.py`, `pkclasses.py`). udpreplay `conn` runs the Alex
  messages through UDPConnection, the stream framing and the client's factory.
* Names: pre-D56 names tagged (f727a18); the review list is `docs/data/names-review.md`.
* Proposals waiting for the user: import WWOnline wlib/wnet (Zero Hour matchbot) plus Renegade's
  RenegadeGR tcpmgr/tcpcon; import Renegade's BinkMovie; ENABLE_WWPROFILE as an app define; the logger
  stand-in; count the 17 template leaf Reads as pairs; names for unknown_changes.* and UnknownCOClass;
  CordMgr::Connect returning the session id; CordArrayClass's remove with wlib; funcmatch's string
  evidence to check `__FILE__`; the draw dump as M2's regression check; Net-7 difference 4 into
  the reference decoder's auxdata.md.

### 2026-10-24 (afternoon): milestones M1 and M2 (D42-D61); app 136 -> 251, first linked programs

Plan: [milestones.md](milestones.md) (M1 capture harness, M2 working VIDTEST); VIDTEST spec:
[vidtest.md](vidtest.md). Coordinator plus builders, two in parallel from M2.1 on.

* M1.0 (5af559a): `tools/harness/convert.py` turns the Alex dumps into datagram files and decodes the
  EA-UDP ack list (type bit 0); it holds all 26,078 / 2,779 the reference decoder messages plus 28 / 12 the reference decoder lost
  (D52). the reference decoder references saved in `.work/harness/ref/`. `capharness.exe`, the first VC6 program.
* M1.1 (ff1db4f): the decode path mapped (milestones.md §2.3). M1.2 (7ca6a71 ... 795b23d): codecs,
  lookup build, encoder records, kv_pair_string, the time field: app 136 -> 164. D54's ambiguous
  column (a666c27). M1.3 (a35535c, 8303cf4): the 11 roots and 42 nested classes generated
  (`tools/re/auxroots.py`), names matched to Net-7's AuxClasses by structure (D59): app 164 -> 202;
  `auxreplay`: all 43,978 EA AUX_DATA decode, 43,977 re-encode byte for byte (D60), every field
  change equals the reference decoder's aux-log (gate (c)), Alex 4,289 / 366 pass.
* L1 (12d7347): `tools/vc6/build.py` builds all eight libraries. L2 (d7c38ab ... fbe117b): the 12
  never-imported EA files (D49, wwlib 565 -> 586, wwaudio 351 -> 360), mss32.lib from the game's DLL
  (D51, `mkmss32.py`), `stubs/` (D44), the first executable `programs/vidtest`. D55: ww3d2/pot.cpp
  NOT_LINKED (9d6635d, WW3D2 2477 -> 2475, D20 duplicates).
* M2.0 (fae8e7a): VIDTEST's spec. M2.1-M2.3 (25c0008 ... 066ace7): BST29A, every W3D scene, sky,
  overlay, text, the three tests, results.ini, exit codes, E&BConfig's launch (no registry), a d3d8.dll
  proxy capturing the original's frames (frame 300 within 3.2%). M2.4 (e47e935, 5c182eb): stars (8
  exact), splines, land, decorations: app 202 -> 251; Max Polys 30729 / PolyBudget 30000 as the
  original; land frames 35-40% different (orientation open). D61 NDEBUG (2fade9a, pairs unchanged).
* Names: D56 certainty tags and `tools/re/names.py` (b61f6af); pre-D56 names tagged (f727a18).
* Lessons (workflow.md §3): never a modal dialog or a screen grab in unattended runs (two
  message boxes reached the user); never run a program in the game's install folder; two builders
  waiting for each other's clean tree deadlocked for an hour (rule replaced).
* Open: M1.4 (wwudp, gate (a)) and M1.5; M2 gate (d), land orientation; the supervised E&BConfig
  run with the user once all features are done.

### 2026-10-24: the kv encoder, the interpolator's Unknown_00, encode.cpp and policy.cpp starts; app 92 -> 136

* `app/shared/kv_encoder_base.cpp`/`.h` (22cfb49, with the interpolator below: 33): `KVEncoderBaseClass`
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
  (0xAEAF08: Get 0x4A6B30 exact, Clone 0x4A6AF0 exact but a twin). `delta_template.h` (e0952d6):
  the generic Write/Read (lines 91/110) and `Unknown_00` with `DeltaTemplateAccessorClass<T>`
  (Set 0x4A6F00); the float Read/Write/Unknown_00 are exact but `multiple` (int twins in game
  code), so uncounted. MoneyClass got `operator!=` (VC6 instantiates a template's virtuals with the
  class, so Set_Value is compiled for money).
* name.cpp `Get_Rest` 0x4ADBB0 (c410fef); lookup.cpp `Find` 0x4AC9F0 (3383224): the map at +0x10 is
  `std::map<const char *, DeltaClass *>` (the key copied to a temporary). The name tests
  0x4ACA90/0x4ACB20 call find 0x4ACBD0 out of line; every variant tried inlines it.
* `encode.cpp` (be443af, 4): `RecordTableClass` (`Records` +0, `Count` +4; ctor line 512, dtor, two
  loops over the records' slots 4/8). `policy.cpp` (6065697, 3): `CompiledPolicyClass` dtor 0x4AE850,
  `Get_Action(int id, int *value)` 0x4AED50 (line 334), `PolicyClass::Get_Action` 0x4AF490 (444).
* Not done: the time/money constructors 0x4A8150/0x4A7080 (byte-identical to each other: `multiple`,
  0 countable; and `template class X<DurationClass>;` in delta_time.cpp is error C2949 next to
  delta_template.h's `extern template`: §3), the time field's Unknown_00
  0x4A8330/0x4A8630 and accessor 0xAEB244 (ticks to wall-clock time, a guarded static).
  Drafts and scripts: `.work/day/D/` (`layout.py` lays a tree file out so asserts land on given
  lines; `m.sh` runs appfile match with the mirror's include dirs).

### 2026-10-24: auxdata continued: interpolator, string, name, permstr, linearbu; app 39 -> 92

* `delta_interpolator.cpp`/`.h` (83c2f38, 7): `DeltaInterpolatorClass` (vtable 0xAEAE54, size 0x250;
  `EndTime` +0xA0, `ChangePerTick` +0x130, `StartValue` +0x1B8, `LastTime` +0x240, `LastValue` +0x248,
  a flag +0x24C). Set 0x4A6590 needed a user-defined `DurationClass::operator=` (duration.h: the
  implicit copy schedules the two stores after the vtable load). The time field's constructor
  is out of line (0x4A8150) where the float fields' inline: `extern template class
  DeltaTemplateClass<DurationClass>;` in delta_template.h (VC6 extension, C4231) gives it; an explicit
  specialisation declaration of the constructor is ignored by VC6. Unknown_00 0x4A6930 left.
* `delta_string.cpp`/`.h` (bb3b4cf, 9): `DeltaStringClass` (0xAEB0E4, `String` +0x84), its value
  object for the encoder (`DeltaStringValueClass` 0xAEB13C on `StringValueClass` 0xAEB150) and
  `DeltaChildVisitorClass::Add_String` (kv_encoder_base.cpp's 0x4B48A0) in delta.h.
* `name.cpp` + `permstr.h` (28c1e60, 12): `NameClass` = `Name`, `Parts[MAX_NAME_LENGTH]` (10),
  `Count` (-1 until `Split`), `HasWildcard`; `PermString` (the client's name, from 0x4ADFF0's message)
  is one interned pointer. `Append` adds `rhs.Count` inside its copy loop (the client's bug, kept).
* `permstr.cpp` (cc9dd7b, 6): the hash table (`s_Table`/`s_Size` 1511/`s_Count`), a counted
  `PermStringInitClass` static in permstr.h (every including object has one: the client's 0x4A60A0,
  0x4ADC50 ...), `Find` (one-at-a-time hash).
* `linearbu.cpp` (300dff7, 18 + 3 four-byte getters at markers): `LinearBufferClass` size 0x1C
  (`Buffer`, +4 unknown, `Size`, `Capacity`, `GrowBy`, `Pos`, `OwnsBuffer`).
* delta.cpp's `Unknown_20` 0x4A53A0 (cba68ae): the base call goes after the `if`. `Add` 0x4A5670 is
  still not exact: the client calls `size()` 0x4A58C0 out of line in `_Last = S + size() + M` and
  lays out the `_Last - P < M` branch's calls differently: a different `<vector>` form (D11).
* VC6 inlines functions defined later in the same file (NameClass::Match_Parts, permstr's
  Is_Prime and entry constructor, linearbu's Seek/Read/Grow). Drafts: `.work/day/C/`.

### 2026-10-24: the Delta object model (app\shared\auxdata); app 11 -> 39

* `delta.cpp`/`delta.h` (845c0c9): `DeltaClass` (the reference decoder's DeltaBase; D24 name, D10
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
* `delta_money.cpp` (5ced1fe, 2: the 8-byte money at +0x88), `delta_time.cpp` (6641861, 2: 4 bytes on
  the wire; `duration.h`'s `As_Int` with `FAIL_IF(Time > INT_MAX)` on line 190), `lookup.cpp`
  (8588371, 3 index accessors at lines 67/78/90). New headers holding only what is used: name.h,
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
  unchanged, none lost (249c365).
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
  instructions with the macro names substituted (e8f39a8).
* `ProjectorClass::Peek_Mapper` comes from Zero Hour's projector.h (0xA126D0 reads Mapper at +0xC8).
  The six .cpp are copied from Zero Hour with E&B's `ShdInterfaceClass(def)` base call (new block
  `SHD_BUMP_PATCHES`). Gains +21: the 19 from 6i plus both `Setup_Self_Shadow_Info` (0xA126D0,
  0xA15240). None lost, replay 0 differences (0de15b9).
* No near-misses left in the new files. Init, destructors, Copy_Vertex_Stream and Peek_Texture are
  exact but `multiple`. The files' out-of-line `DX8Wrapper::Set_DX8_Texture_Stage_State` differs
  from ww3d2's 0x9217E0 (0.47), as in the older wwshade files.

### 2026-10-24 (overnight): Zero Hour's dated edits, D9 finished, wwlib's E&B allocator; 3,925 -> 4,062

Unattended run 00:02-07:40 PDT: a coordinator and one builder subagent at a time (no parallel
builds: `verify` shares `%TEMP%\enb_import` and the VMs' scratch), queue and log in
`.work/night/plan.md`, a heartbeat every 30 min, each builder's report in
`.work/night/item*/report.md`. 65 commits `2f6c80c..e88f4e9`, none pushed. Morning report:
[overnight-report-2.md](archive/overnight-report-2.md).

* D9's texture leftovers (items 1, 6f): `Get_Reduction` 0x94BC20, `TextureClass::Apply` E&B's
  form 0x94BC80, `Get_Texture_Memory_Usage` one non-virtual base function with Generals' split
  body 0x94C1D0, `Set_Mip_Mapping` compiled in texture.cpp 0x94CA20 (a658d43..11e300b); Zero Hour's
  08/05/02 `TextureFilterClass` dropped for Renegade's five filter members, all four constructors
  0x94B240/0x94B3D0/0x94B5B0/0x94B660 exact, texturefilter.cpp/.h `NOT_LINKED` (8f10486, c35cc34,
  new block `TEXTURE_FILTER_PATCHES`).
* `DX8Wrapper` device code (item 2, 4ae710d..854e5f5, +25): WW3D2 built `/D USE_WWSHADE`,
  `Reset_Device` E&B's body, E&B's lost-device restore `Unknown_Restore_Device` 0x921C50 (D10),
  transform cache, `Init` zeroing; `Create_Device` without `DX8Wrapper_PreserveFPU` 0x921EF0 (9f11db9).
* Zero Hour-only edits in Renegade-based files (items 3, 6b, 6d, 6e): `//DMS`, ZH-tagged and
  commented-out code found by diffing every EA copy and by scanning the client's log/profile
  strings (`.work/night/item6b/strscan.py`, `fdiff.py`; `item6d/zhall.py` splices every
  differing donor function and reports gained/lost). `GapFillerClass` 0x959A80, animobj, scenes,
  sorting loops to `Get_Max_Textures_Per_Pass`, texture loader HSV shift, DDS guards, dx8caps
  (Matrox Parhelia), `Alter_Avatar_HTree` 0x98A2D0, `ShaderClass::Apply` with `GRADIENT_MODULATE2X`
  0x93B510, `post_process` 0x9BCC40, WW3D's static sort lists and `SHD_FLUSH` (fdd0a81),
  `Post_Load_Process` 0x9475E0 (b02b85e), `WW3D::Init` 0x917660.
* Dated-comment sweep (item 6i, `.work/night/item6i/dated.py`: 1,116 dated comments + 1,770 VSS
  `$Modtime`s against the client's bytes) fixed D9's cutoff: Zero Hour's KM/KJM (shader system)
  line is in through 08/01/02; the 08/05/02+ texture redesign and the Generals team's MW/GTH
  changes are out, whatever their date. D9 reworded accordingly (§2). Gain: the asset manager's
  07/01/02 shader-loader registration 0x91C2C0 (a2a0fcc).
* `dx8webbrowser.cpp` from Zero Hour (item 6k, 761d6ac, +19): `BrowserEngine.tlh`/`.tli` generated
  by VC6 from E&B's own `BrowserEngine.dll` (same GUIDs as EA's, plus `querydata` and
  `Post_MultiPart_Form`) as reconstructed files (D21); vc6.py syncs `.tlh`/`.tli`.
* Distortion (item 5, 11f65aa): external linkage of the list and an inline `expf` give 0x995E20,
  0x996180, 0x997DE0; the draw 0x9961E0 is still 369 masked rows.
* wwdebug/wwlib (item 7b): the memory log's 23 categories (client name table 0xBCB228, c7fc28c,
  +11), `FastAllocatorGeneral`'s 0xE30-byte E&B layout with out-of-line `Alloc` 0x9DAE10, `Free`
  0x9DAF60, `Unknown_Reset_Statistics` 0x9DADA0 (9850f75..debed6b), `Read_Frame` 0x9EB4C0.
* Item 4: the meshmatdesc "inlining flip" did not exist: 0x4E9CB0 is E&B's `::operator new`
  (`FastAllocatorGeneral::Get_Allocator()->Alloc`), never inlined; the three fragile functions
  differ in index load order that moves with the number of symbols declared before the classes
  (noise). Only 0x947310 is lost (D18).
* Tools: `gaps.py` and every tool use verified function starts (98a0280, 78b0a74; symbols 6,736 ->
  6,858, gaps rows 652 -> 593); importer lock `.work/locks/importer.lock` (8c0516a); VM pool
  `tools/vc6/pool.py` (87f38c3, orphan reclaim 6e298b5) and `importer.py verify --pool K`
  (c7b8bcc); `tools/re/appfile.py` for E&B's own files (5df8a3e: `app\shared\auxdata\bitset.cpp`
  12/12 exact as a draft in `.work/night/item7/try/`, not in the tree).
* Old build VMs retired (07:00): the two `VC6 SP4 (8799)` clones removed, `Tools\VMClones\1,2`
  deleted; `.work/vc6_hosts.json` lists only Win10 (192.168.193.108); `enb-vc6-01/02` stopped, free.
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
  disassembly (0a4514e): the camera-facing square projected to whole pixels, the on-screen part
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

* REA's raised timeouts (3d44d68) work: the first `batch_decompile` came back after Ghidra's
  analysis (285 s), moved to a background task at Claude Code's 120 s mark. Correction to the
  entry below: the distortion session's retries did **not** work. All four REA calls (line groups
  0x97BAF0/0x97C900/0x97C360; distortion 0x9961E0 and 0x997EA0..0x9981A0) failed with
  `provider_timeout`, hidden because the ~85 KB-per-item error payloads overflowed the result.
* Then 0x9961E0 failed differently: Ghidra defines no function there (reached only from the
  vtable at `.rdata` 0xB13984) and REA can't create one. So: `tools/re/pseudocode.py` +
  `tools/re/ghidra/DecompileTargets.java` (20541ad) run Ghidra headless, create a function at
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
  (dbdb1d6..7eaf120); each fix is a row in library-matching.md §3.6.1. Full verify at the end: all
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
  Five commits, 0941037..c5fcd0e; each fix a row in library-matching.md §3.6.1.
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
  (condensed objdiffs of the top gaps), `.work/fn.py` (print one function). VMs at
  192.168.193.108, .202.231, .207.76 (`vc6.py check` passed). REA not opened this session.

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
* Commits: d32f1b3..(this handoff) on master. Nothing pushed.
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
* Commits: 23bd311..66236bf on master. Nothing pushed.
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
  `.work/vc6_hosts.json` (192.168.193.108, .202.231, .207.76).

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
* Commits: 60e66ab (wwshade's WW3D2, merged into master by fast-forward), 8501576 (tools),
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
* Commits 8c83880, 13ea6b4 on branch `d9-texture` (master unchanged, nothing pushed).
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
  `.work/d9_base_imports.json` (pre-D9 state). VMs 192.168.193.108, .202.231, .207.76.

### 2026-10-23: importer replay; decisions D6-D11

* `importer.py replay` rebuilds every library in `.work/replay` from the EA copies plus the
  decisions in `docs/data/imports.json` (header origins, source copies, splices, then PATCHES,
  annotations, APPENDS, NEW_FILES, RECONSTRUCTED), and compares the result with the repository
  file by file. It now reproduces the git tree exactly: 7 libraries, 0 files differ.
* The first run found records that disagreed with the tree; the records were fixed, not the tree
  (no source file changed, so no compiled byte changed): `lightenvironment` kept a stale
  `headers_from: Z` after f8cad49 returned its header to Renegade's; seven header origins swapped
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
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |
| D6 | 2026-10-23 | E&B's own code starts with `app\shared\auxdata`, then `app\wwudp`, then the task kernel (`app\kernel`: `KernelClass::Run_One_Cycle`, the loop running the current, background and pending-delete tasks; with `app\main` and `app\client\tasks`), then `app\packets` ([app-plan.md](app-plan.md) §3) | user |
| D7 | 2026-10-23 | E&B's own code is held to byte-exactness everywhere, like the libraries. Check in with the user periodically if this becomes too costly. Trigger: revisit with the user when docs/data/not-exact.md passes 400 entries (269 on 2026-10-27) (amended 2026-10-27, D110) | user |
| D8 | 2026-10-23 | No hybrid client (reconstructed functions patched into the retail one) for now: its results would need the user to validate them. Open to it again if it can be shown to verify results better than byte matching, without the user | user |
| D9 | 2026-10-23 | E&B follows Zero Hour's KM/KJM (shader system) line through 08/01/02, without the 08/05/02+ texture redesign and without the Generals team's MW/GTH changes. So `TextureFilterClass` (08/05/02 KM) is dropped: `TextureClass` has Renegade's five filter members (texturefilter.cpp/.h `NOT_LINKED`) | user; overnight finding: Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks, and it unlocks wwshade. Reworded by the user 2026-10-23 after step 1 showed the client has the 06/27/02 abstraction but not the 08/05/02 redesign (library-matching.md §3.6.1), and again 2026-10-24 after the dated-comment sweep (`.work/night/item6i/report.md`: bytes place every dated ZH edit; MW 0/3, GTH 10/12 absent); filter members folded in by the user 2026-10-24 (all four constructors exact, 8f10486) |
| D10 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D11 | 2026-10-23 | Compiler-generated code (STL template instances): extend the corpus as each app file needs it, not ahead of the app work | user |
| D12 | 2026-10-23 | Compile each library with the flags E&B used, wherever byte evidence shows them (wwshade: `/GX /Ob1`; importer `LIB_FLAGS`, `bytematch_all.sh` `LIBFLAGS`) | user: "match how E&B built things whenever you have evidence to support it" |
| D13 | 2026-10-23 | EA files the client does not link are dropped from the tree (importer `NOT_LINKED`, with the evidence): first `shdlegacyw3d.cpp`/`.h`; then WWAudio's `AudioSaveLoad.cpp`/`.h` (2026-10-23) | user |
| D14 | 2026-10-23 | `ENABLE_WWPROFILE` in WWDebug too, although its bytes are neutral (verify: 67 before and after, none lost): consistent with a project-wide define. Soft: drop it if it ever costs a match. Also (D26, merged 2026-10-27): `USE_WWSHADE`: WW3D2 built with it (under D12, evidence: the `SHD_*` wrapper calls); wwshade too, as a soft consistency setting like the above (neutral in the bytes); done 4ae710d, 7ecb645. Also (D74, merged 2026-10-27): `ENABLE_WWPROFILE` goes into the app library's defines, kept if a verify changes no pair (evidence since: WWAudio +32, WWMath +1, WW3D2 +20 pairs) | user |
| D15 | 2026-10-23 | `G_CODE_BASE` in WWAudio stays an importer patch removing its two `#ifdef`s, not a `/D` in `LIB_FLAGS` (same bytes) | user: only `WWAudio.cpp` uses it, so the bytes can't show a project-wide define |
| D16 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D17 | 2026-10-23 | merged into D56 (2026-10-27) | user |
| D19 | 2026-10-23 | E&B's screen-distortion module in WW3D2 (0x995D20-0x998200, between dx8texman.cpp and boxrobj.cpp; first described as a cube-map list) is rebuilt as `ww3d2/unknown_distortion.cpp`/`.h` (a reconstructed file), neutral names (D10) until E&B's own turn up; the function `WW3D::Set_Render_Device` calls is `Unknown_Reset_Distortion_List` | user (name chosen after the module turned out not to be about cube maps) |
| D18 | 2026-10-23 | Don't include `matrix4.h` first in `ww3d2/meshmatdesc.cpp` (it would win back `Make_UV_Array_Unique` 0x947310, but no EA copy has that order). Keep it on record until a better fix is found; revisit if it becomes WW3D2's last outstanding item | user; kept 2026-10-24 after item 4 showed the cause is symbol-count noise, not an inlining flip; confirmed 2026-10-25 (evening, builder C): the difference is only which of `pass` and `stage` is loaded first, operand-order tie-breaking, not inlining; closed 2026-10-27 (D110): the noise is understood (operand-order tie-breaking, 2026-10-25); kept as is, no revisit |
| D20 | 2026-10-24 | Losing a COMDAT-duplicate `(file, address)` pair, where the helper stays matched in another file, is not a loss. Name each such pair in the commit message | user (03:40) |
| D21 | 2026-10-24 | `dx8webbrowser.cpp`'s type library: `BrowserEngine.tlh`/`.tli` generated by VC6 from E&B's own `BrowserEngine.dll` are committed as reconstructed files, `#include` instead of `#import` (original kept as a comment); no binaries in the repository (option A) | user (04:40); done 761d6ac |
| D22 | 2026-10-24 | Import Zero Hour's shd6/shd7/shd8 bump shaders into wwshade, with the shader headers rebuilt from the client's data (0xBCB8FC-0xBCEBB7) as reconstructed files, in `shdpp.exe`'s format: `DWORD` arrays holding the shader text, NUL, zero padding, 0 terminator (PS; `DWORD` approved by the user 2026-10-24, since `ShdHW*Shader::Create(DWORD*)` takes them and `char[]` would pick `Create(char *file_name)`) | user (06:25); done 2026-10-24 (e8f39a8, 0de15b9: headers as shdpp's DWORD arrays, since `Create(DWORD*)` takes them) |
| D23 | 2026-10-24 | E&B's own files enter the importer as a non-EA library `app` (all reconstructed, `/GX`, `NDEBUG` on since D61 (first written as off), verify limited to the app region) | user (06:25); done 249c365 (importer `APP_LIBS`; sources compiled as `e:\build\beta\G\...` for the client's `__FILE__`) |
| D24 | 2026-10-24 | App classes the client doesn't name get Westwood `XxxClass` names (`BitSetClass`) | user (06:25); conventions.md §1 |
| D25 | 2026-10-24 | App asserts sit on their real `__LINE__` through the file layout (blank lines, comments) in tree files; `#line` only in drafts | user (06:38) |
| D26 | 2026-10-24 | merged into D14 (2026-10-27) | user (03:47, 06:25); done 4ae710d, 7ecb645 |
| D27 | 2026-10-24 | Verified function starts (decoded call targets) are the default for every tool | user (03:47); done 78b0a74 |
| D28 | 2026-10-24 | Keep ww3d.h's unused Renegade `MinStaticSortLevel`/`MaxStaticSortLevel` declarations (noise compensation for meshmatdesc after fdd0a81, not evidence); revisit when the noise is understood | user (06:25); kept 2026-10-25 (evening): the noise is operand-order tie-breaking (builder C), D18 likewise; closed 2026-10-27 (D110): the noise is understood (operand-order tie-breaking, 2026-10-25); kept as is, no revisit |
| D29 | 2026-10-24 | Keep 62b895b's protected `D3DTexture` (needed by c35cc34, which writes it as 0x94B660 does); patch reasons reworded. Its comment (D35, merged 2026-10-27) cites 0x94B660 (c35cc34), not `Apply` 0x94BC80 | user (06:38); done e88f4e9 |
| D30 | 2026-10-24 | Build VMs come from a pool of differencing clones of `Tools\VMPool\base-vc6.vhdx`, no warm sync cache (each leasee syncs its tree), cap set by host memory (`pool.py create --max`, default 7; one VM per 2 GB free above a 3 GB floor); the old fixed clones are retired (amended 2026-10-27, D110) | user (04:40, 06:25); done 87f38c3, c7b8bcc; clones retired 07:00 ([toolchain.md](toolchain.md) §1.3) |
| D31 | 2026-10-24 | Sessions release their leased VMs in the handoff skill; `pool.py lease`/`create` reclaim orphaned leases first | user (06:25); done 6e298b5 |
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
| D56 | 2026-10-24 | Name certainty: every name chosen without the client's own evidence carries a tag comment where it is declared, `// NAME n: evidence`, scored 5 = the client names it (RTTI, assert, `__FILE__`, `Class::Method` string), 4 = EA's Westwood source names it, or a Net-7 / the reference decoder name confirmed by matching structure, 3 = built by convention from a client-named file or sibling (D24 class-from-file, a `.h` paired with a known `.cpp`), 2 = describes what the code does, 1 = neutral placeholder (`Unknown_xx`, D10). 4-5 are settled; 3 and below are listed for review by `tools/re/names.py` in the generated `docs/data/names-review.md` (untagged `Unknown*` names count as 1). When better evidence appears the tag is raised and the name leaves the list ([conventions.md](conventions.md) §1). Merged 2026-10-27, with D65 (neutral names are NAME 1): D10 (E&B's unknown additions keep neutral names, `m_UnknownXX`, evidence in a comment, until the code that uses them is rebuilt); D16 (members whose use is now rebuilt, `m_Unknown148`, `m_Unknown164/168/16C`, `m_Unknown19A5/19A6`, keep their names with the comments in the headers until the app code shows E&B's names); D17 (the two wwshade `"UnNamed"` constructors stay near-misses; revisit only on new evidence); D33 (kept: `MEM_CLIENTOBJECT`..`MEM_PARTICLES` from the client's name table 0xBCB228, `FastAllocatorGeneral::m_UnknownE00`..`m_UnknownE20`, `Unknown_Reset_Statistics`); D57 (`encode.h` NAME 2 and `strconv_lib.h` NAME 3 stay; `KVPairClass`'s virtuals keep descriptive names `Clone`, `Get_Type_Name`, `Get_Value_String`, NAME 2; the file for the compound/root functions 0x4A3990-0x4A4780 and 0x4B0860 waits for M1.3, using Net-7's `AuxClasses` and the reference decoder's `the reference game model` names as structure-matched hints); D58 (provisional file names stay: `app/shared/auxdata/unknown_root.*`, the compound/root object with no `__FILE__`; `app/client/aux_roots.*`, the generated roots, the client's code being in co.cpp and an unattributed object at 0x751BE0, the generator writing into co.cpp once that is rebuilt; `app/shared/heightfield.*`, `app/shared/land.*`); D86 (step 8's `unknown_memregistry.cpp`, `unknown_listener.cpp`, `unknown_rate.cpp` and `RSAKeyClass`'s `Unknown200..Unknown800` members, NAME 2, stay until a string or `__FILE__` gives the real one, P2) | user |
| D57 | 2026-10-24 | merged into D56 (2026-10-27) | user |
| D58 | 2026-10-24 | merged into D56 (2026-10-27) | user |
| D59 | 2026-10-24 | AuxData class names: nested compounds `Delta<Net-7 name without Aux>Class` (NAME 4, matched child by child against Net-7's `AuxClasses` and `the reference game model`, `tools/re/auxroots_names.json`), roots `Delta<client string>Class` (NAME 3); `DeltaInterpolatorClass` keeps its name (Net-7's AuxPercent / C# Ramp; the client interpolates) | user |
| D60 | 2026-10-24 | M1 gate (b) is met with one documented exception: capture_1 #24128 (a Hulk, header byte 0x55 under a parent marking that child cleared) decodes but can't be re-encoded by the client's encoder; the harness doesn't keep header bytes as read | user |
| D61 | 2026-10-24 | The app library compiles with NDEBUG (in APP_LIBS' defines; G_ASSERT / FAIL_IF don't depend on it), confirmed by a verify that changes no pair; files stop defining it themselves | user |
| D62 | 2026-10-25 | Builders may run in parallel on disjoint items (replaces D32), under the shared-tree rule: never compile the other builder's changes into a verify (check `git diff` of shared files: importer.py, app headers, stubs, docs/data), commit only your own hunks, keep the uncommitted window short, never wait for a clean tree (that deadlocked both builders for an hour on 2026-10-24). Each builder uses a private scratch folder (TEMP/TMP under .work/<agent>/tmp; automatic once the importer does it) or its own worktree. The importer lock serialises `verify` ([builders.md](builders.md)) (amended 2026-10-27, D110) | user (approved for the 2026-10-24 unsupervised run; recorded 2026-10-25) |
| D63 | 2026-10-25 | M2 gate (c)'s repeatable check is `tools/harness/vidtest_e2e.py` (E&BConfig's command line, results.ini). A supervised run of the original E&BConfig with the user, registry backed up first, is a one-time acceptance check, not a gate. D36 reworded: E&BConfig.exe isn't rebuilt. **Passed 2026-10-25** (log entry of that evening): every displayed value as our results.ini, registry unchanged | user |
| D64 | 2026-10-25 | `// NOT EXACT:` is a general interim marker for app code written functionally from pseudocode, not only on the M1/M2 path (D42); D7 stays the target. `tools/re/names.py` lists every marked function in the generated `docs/data/not-exact.md` | user |
| D65 | 2026-10-25 | D10/D16's neutral names are NAME 1 on D56's scale, replaced whenever better evidence appears (conventions.md §1 reworded) | user |
| D66 | 2026-10-25 | Pair losses. Not losses, named in the commit message: (i) a COMDAT duplicate kept matched in another file (D20); (ii) the same source compiled into two files (D55); (iii) a false match exposed by a type fix (first: 0x8C1520, `KVPairTemplateClass<__int64>`'s string setter, matched by the money instance only while `MoneyClass` was a bare `__int64`). A change that gains and loses nothing but models the client's own classes (b7cf527: Ship's BuffArray and Buff elements with the client's vtables and globals) is allowed, stated in the commit (2026-10-25, evening). (v) a net-positive change where every lost pair's address is re-credited under another file in the same verify (pairs_cmp's address-level 'lost outright' list empty), named in the commit; an address lost outright still waits for the user; (vi) a pair that becomes ambiguous because the client's own form adds a byte-identical instance (D119). Anything else is a loss and waits for the user before the commit. D54's ambiguous column stays as is: the 17 identical template leaf Reads are not pairs (amended 2026-10-27, D110; 2026-10-28, D119) | user |
| D67 | 2026-10-25 | Test programs live under `tests/` (D45 extended: kerneltest and movietest in tests/capharness). The wlib stand-ins (app/common/dictionary.h, critsec.h, app/wwudp/wtime.h, parts of cordcon.h) stay until D73's import replaces them with the imported files at the client's paths (`libs\wwonline\wlib`), then are deleted Done: the wlib stand-ins are deleted (D73's import is done), so the "stay until" clause is complete (2026-10-27) | user |
| D68 | 2026-10-25 | [kernel-map.md](kernel-map.md)'s 10-step reconstruction order is adopted (extends D6); steps 1-5 and 7 done or partly done on 2026-10-24 | user (backfilled) |
| D69 | 2026-10-25 | Safety rules for every run of our programs and every session (amends D48): never a modal dialog (`-nomsgbox` or its equivalent in every program); full screen allowed; no screen grabs, images only from the program's own back buffer; never the game's install folder as working directory; no registry changes (the original E&BConfig only in a supervised run with a registry backup); never push; never `importer.py assemble` on a tree with work; `git add` by name, `.claude/settings.json` stays out | user (each given on 2026-10-24; numbered 2026-10-25) |
| D70 | 2026-10-25 | Inline assembly stays wherever the client has it, with a C-equivalent comment | user (2026-10-23; backfilled) |
| D71 | 2026-10-25 | A commit's `Co-Authored-By` line names the model that wrote the commit (the line that session's harness gives); the docs carry no literal model name | user |
| D72 | 2026-10-25 | Subagent models: the coordinator runs Opus (D96; Fable until 2026-10-27); builders (verify loops, byte matching, RE, programs) run Opus by default; Fable for a subagent only when the expected total cost is lower: a small-input reasoning puzzle (inline budget, codegen form) after two failed Opus attempts, or a one-shot analysis whose wrong answer would cost a builder run; Sonnet for read-only audits, cross-referencing, doc moves, log digging and generator reruns (anything a tool checks afterwards; Explore agents too); Haiku for grep-level searches and log triage, never tree code. Plan logs record model and tokens per builder ([builders.md](builders.md) §4) (amended 2026-10-27, D110) | user |
| D73 | 2026-10-25 | Import WWOnline's wlib and wnet (Zero Hour's matchbot), Renegade's `Code\Tools\RenegadeGR` tcpmgr/tcpcon and Renegade's BinkMovie, under D4 (file by file, each verified). `CordArrayClass`'s remove is rebuilt on the imported wlib and its stand-in deleted (D67). Unblocks kernel-map steps 8-9 | user |
| D74 | 2026-10-25 | merged into D14 (2026-10-27) | user |
| D75 | 2026-10-25 | merged into D81 (2026-10-27) | user |
| D76 | 2026-10-25 | `CordMgr::Connect` keeps the client's signature; the harness reads the session id another way, unless the client's bytes return it | user |
| D77 | 2026-10-25 | funcmatch's string evidence requires the client function's `__FILE__` to match the EA function's file; symbols.json regenerated (0x829C80 is tcpconn.cpp's constructor by accepted socket, not `PacketManagerClass::Clear_Socket_Error`) | user |
| D78 | 2026-10-25 | The d3d8 draw dump (`tools/harness/run_reference.py`, `drawdump.py`) is M2's standard regression check, beside the frame comparison | user |
| D79 | 2026-10-25 | Net-7 difference 4 (fields named in two sets) is a one-line note in the reference decoder's `docs/research/auxdata.md` §7 for that project's coordinator. the reference decoder and a private tool need their own audit and re-architecture later, outside this project's scope; their design docs are now likely less complete than this project's for transport and auxdata. The user revisits this before housekeeping closes | user Settled 2026-10-25: the audit and first housekeeping were done (the proxy is now the reference decoder, `C:\EnBDevelopment\the reference decoder`); the rest is that project's.; settled 2026-10-25; retired 2026-10-27 |
| D80 | 2026-10-25 | `objmatch.py` breaks `multiple` ties by each data relocation's target (the string or known symbol it points at) against the object's own literals, so byte-identical functions that differ only in their strings count (the assert reporters, the twin constructors 0x4A7080/0x4A8150, `Set_Render_Device`, the one-int packet constructors and Prints: builders B and D). Adopted after a snapshot comparison of every library's pair set with no losses, the counts before and after recorded in library-matching.md §3.6.1. The objmatch order (merged 2026-10-27 from D88, D94(b), D95(b), D100): D80 rounds to a fixed point; then the self-name round (D88: the one candidate address that symbols.json names with the function's own mangled symbol from a non-exact proof, a vtable slot, is credited when D80 finds no contradiction there; measured by builder W3 at the objmatch level: +337 pairs, 0 lost, ww3d2 188, wwmath 60, wwaudio 45, wwshade 31, wwlib 10, wwdebug 2, wwsaveload 1; prototype `.work/w3/objmatch_sn.py`; adopted after a full verify snapshot of a quiet tree, with pairs_cmp proving the gain, not while builders are verifying; carried out 2026-10-27 04:50, cbd1292 the pre-D88 snapshot, 3364636 the rule: +368 pairs, 0 lost, ww3d2 188, wwmath 60, wwaudio 45, app 31, wwshade 31, wwlib 10, wwdebug 2, wwsaveload 1; `tiebreak`/`selfname` do not reach imports.json, left as is until something needs it); then D80 again; then the range round (D100: a tie goes to the object whose client range contains the address, P-P6-1, carried out on a quiet tree with a full verify of every library and pairs_cmp gains-only, like D88, then 45 from `.work/p6/park45` and 117 from `.work/p5/park117`; refinement 9339007: an address another function is exact at is held out, and the range is the object's own non-COMDAT or NODUPLICATES exact functions); compiler-generated `_$E<n>` names are skipped (D94(b), P-H7-2, carried out the same way). D95(b) (the D80 tie-break not crediting an implicit destructor to an identical twin when the function's own address is free, P-P5-2) is not implemented: superseded by D100 and its refinement, close pending as P-M1-1 | user |
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
| D94 | 2026-10-27 | (a) T5's drop of (tsk_login, 0x85CB00), the avatar's implicit assignment, when account_id became `__int64` is a D66(iii) non-loss (c0091e0); the address returns with the packet file that carries the client's copy. (b) merged into D80 (2026-10-27). (c) The clienttest program (P-T7-1) is built once the first cpacket.cpp handlers exist, about one builder session | user |
| D95 | 2026-10-27 | (a) udpreplay sends the recorded bytes for a rebuilt class whose client Build writes a size field larger than its payload, as an explicit opcode list (97 AVATAR_DESCRIPTION, 183), so 97 can be committed from `.work/p5/park97` (P-P5-1). (b) merged into D80 (2026-10-27): not implemented, superseded (P-P5-2; 117 GLOBAL_ERROR was unparked under D100). (c) D66 gains kind (iv): a file rename whose pairs reappear under the new name, named in the commit, is not a loss (P-P5-3: pkunknown_85e640 / 85f870 / 860d30 to gameedit/pkgekv, pkgeobj, pkgeplan) | user |
| D96 | 2026-10-27 | D72 revised from D93's scores (P-E1): coordinators run Opus 5.5 instead of Fable; every other D72 row stands (builders Opus by default, Fable under D89, Sonnet for audits and tool-checked work, Haiku for searches, never tree code) | user |
| D97 | 2026-10-27 | The D89 Fable queue takes the evening's four requests: client_lego_impl's Find 0x72ABB0 + Add 0x72B7F0 + Remove 0x72C1D0 as one round on their common end-of-function inlining (P-H20-1) first, then CLIENT_DAMAGE 0x738970 (P-T19-3; FORMATION 0x734740 after it), then TimedLog 0x824540 (P-S1-1) and CLIENT_CHAT_EVENT 0x73B150 (P-T20-1) at low priority; D89's limits unchanged (one Fable builder at a time, 4 functions per session) | user |
| D98 | 2026-10-27 | Before a function is marked LIMIT REACHED on a register residue, a tryv2 sweep of loop forms in the inlined helpers it calls is part of its second round (P-H22-2; how 0x4B3850 fell after F3's rounds) | user |
| D99 | 2026-10-27 | Functions too short for the matcher but byte-identical to the client (objdiff 0 rows) are recorded as exact rows in attempts.csv (P-S1-2; 0x82E1D0); no matcher change | user |
| D100 | 2026-10-27 | merged into D80 (2026-10-27) | user |
| D101 | 2026-10-27 | VIDTEST on LandClass (P-L6-1 with P-L6-2 as its first step): one probe places the four undefined land symbols (~LandPatchClass 0x474560, ~LandPatchRecordClass, Unknown_477E20, LandUnknownClass::Peek_Material_Pass 0x93D6C0); then vidland's Create_Decorations / Render_Decorations move to programs/vidtest/viddeco.* (VIDTEST's own 2002 code, D46) as a render object after the land, the rest of vidland.* is deleted and `.work/l6/`'s switch diff applied; land test, drawcheck and sbs within +-1.5% | user |
| D102 | 2026-10-27 | One whole-app trial of SOptionsClass +0x1C as a float (`[Lights] GlobalZ` through INIClass::Put_Float, P-T18-3); kept only if soptions.cpp's 7 of 7 hold and pairs_cmp is gains-only | user |
| D103 | 2026-10-27 | udpreplay compares LOGIN 2's clock field (byte 0x44) zeroed, as for 52 and 68 (P-P9-1, D91's family; f41cef7) | user |
| D104 | 2026-10-27 | LoginKVVisitorClass merges into DeltaChildVisitorClass / KVEncoderBaseClass (kv_encoder_base.h, delta.h; the canary 0x6FB1E0) together with prof.cpp / race.cpp (~30 functions 0x4F6A80-0x4F7820), as one item (P-P9-2, the rest of P-T5-2) | user |
| D105 | 2026-10-27 | ADVANCED 62 (0x734130) gets one more Opus round, the second recorded one D89(1) requires; 62 enters the Fable queue only after it, and then gets a D89 Fable round if still not exact (P-T16-3; SIMPLE 8 shares its residue) (amended 2026-10-27, D110) | user |
| D106 | 2026-10-27 | tsk_game.cpp keeps its one-line view of ServerRedirectPacketClass; its comment is refreshed at the file's next edit (P-A1) | user |
| D107 | 2026-10-27 | The four `Unknown_BE8B38Class` views (tsk_game.h, sclient.cpp, fontdatabase.cpp, soundlib.cpp) become option.h's OptionClass as an item of its own with its own verify (P-Bopus-1) | user |
| D108 | 2026-10-27 | option.cpp's Get_String 0x7F9700 joins the D89 Fable queue at low priority (P-Bopus-2) | user |
| D109 | 2026-10-27 | SClient's Init 0x741180 and Shutdown 0x741BB0 are held until the UI base classes they call exist (P-T7-2) | user |
| D110 | 2026-10-27 | The decisions audit's changes: D89(4), D105, D72 amended (conflicts); D66 (v), D30, D62, D42, D18, D28, D7 amended (holding back); D67, D75, D79, D32 and the D14 / D29 / D56 / D80 families merged (housekeeping) | user |
| D111 | 2026-10-27 | E&B's ref_ptr.h: the three EA copies are compared for the lines E&B's version lacks (P-F6-2, after F6 found G_ASSERT(dummy == 0) absent: 964f95e); then client_lego_impl's Remove 0x72C1D0 gets 1-2 more forms (the named lego_id reference, Find's exact form) as a D89(2) retry citing that lever (P-F6-1); Add 0x72B7F0 waits for P-F6-2 | user |
| D112 | 2026-10-27 | Tools from the traps audit: (1) the importer and tryv2 put scratch under .work/<agent>/tmp when an agent id is set, `--agent` defaults from the worktree name; (2) commit_mine commits a file's `headers` rows, splits attempts.csv and the generated .md files by row, gains `--refresh-totals`, and commits by compare-and-swap (`update-ref` with the old value); (4) tools that write the tree refuse when the working directory isn't their root; (5) `appfile.py match -I auto`, its own lease, objects outside the draft folder; (6) names.py scans untracked files and refreshes not-exact.md. Tool (3), a worktree helper: see the coordinator's comparison with worktrunk | user |
| D113 | 2026-10-27 | Worktrees get a tool of our own, `tools/vc6/wt.py` (new / bring-over / remove: junctions, captures, libraries, app.lib; bring-over refuses unless master equals the base, merges the row files by row, re-verifies and lands through commit_mine's compare-and-swap), not worktrunk: its setup would still be our hook script, its copy-on-write sharing needs APFS / btrfs / XFS (this host is NTFS), and its squash merge with generated messages conflicts with the one-commit-per-verified-gain history (D112(3)) | user |
| D114 | 2026-10-27 | (a) D95(b) is closed, superseded by D100 and its refinement (P-M1-1); (b) P-M1-2's wider range (the object's exact COMDATs only it is exact at, within 0x1000 of either end) and (c) P-M1-3's regeneration of symbols.json each get a full verify of every library on a quiet tree, like D88, and are kept only gains-only | user |
| D115 | 2026-10-27 | objmatch credits a tied constructor / destructor of a file-static to the address its rebuilt static initialiser registers or calls (the `_$E` initialiser's own relocation names it: co_player_relationship's 0x709EF0 registers 0x709F40's atexit), as D88's self name does from a vtable slot; finds and other tied helpers stay tied. On a quiet tree after D114, with a full verify of every library, kept only gains-only (P-R2-1 narrowed) | user |
| D116 | 2026-10-27 | D115's initialiser round credits by symbol across objects (one mangled symbol is one address in the linked image), as builder M3 built it (092776b: permstr 0x4AE2D0, definitionfactory 0xA06000 / 0xA06020); D115 reads accordingly (P-M3-1) | user |
| D117 | 2026-10-27 | pairs_cmp prints the addresses lost outright (lost pairs whose address no file holds afterwards) and the moves, the list D66(v) depends on (P-M3-2) | user |
| D118 | 2026-10-27 | Evidence-backed pins (Open-BFME's ledger) are measured and not adopted: the matching stays objmatch's rounds in D80's order. Builder M4's measurement (wt/pins 8db8c42): a caller in the same object +655 pairs, 0 lost; a caller in the library +812; placement +321 / +336 with about 2.4 % wrong where checkable; six hand pins +8; 82-88 % of every kind's credits land on byte-identical twins. The prototype and its numbers are kept in tools/re/experimental/pins/ (README) for a later decision | user |
| D119 | 2026-10-28 | P-LD2-1 accepted: a pair that moves to D54's ambiguous column because the change writes the client's own form, which has several byte-identical instances (land.obj's `Create_*` RefCountPtr setters; 0x474080 when Build_Decorations 0x47A2F0 uses them), is not a loss under D66 (vi). Named in the commit with the address and the identical instances; an address lost outright still waits for the user. Work item: land.cpp's Land_Set_Ref helper goes back to the client's `Create_*` calls | user (LD2's proposal) |
| D120 | 2026-10-28 | P-LD1-2 accepted (a D69 safety default): drawcheck, vidtest_sbs and run_vidtest default to `--art build/art` whenever that folder exists, so VIDTEST's Save_PLD never writes into the install folder | user (LD1's proposal) |
| D121 | 2026-10-28 | P-CR1-1 accepted: co.h's `UnknownA8` (+0xA8) is `BaseAsset` (CREATE 4 and CHANGE_BASE_ASSET 38 store the packet's BaseAsset there; 0x6F39F0 reads it as BASE-n) | user (CR1's proposal) |
| D122 | 2026-10-28 | P-CR1-3 accepted: the object subclasses' run 0x70DC00-0x70F1D0 (constructors 0x70DBC0 / 0x70E2E0 type 24, 0x70E5A0 type 28; no `__FILE__`) gets the provisional file `app/client/co_subtypes.cpp` (NAME 1), renamed if an assert or string names it | user (CR1's proposal) |
| D123 | 2026-10-28 | P-PF1-1 accepted: `ClientClass::DebugObjectID` (+0x112C) is the player's avatar id, `AvatarID` / `Get_Avatar` (CLIENT_AVATAR 55 sets it; ADVANCED 62, FORMATION 65, 0x6F3C60, DAMAGE 100 use it) | user (PF1's proposal) |
| D124 | 2026-10-28 | P-PX2-1 accepted: the string-less object 0x451C00-0x452770 (`PxBoxCallback`) keeps the neutral file name `app/shared/unknown_px_callbacks.cpp` (D10) until a string names it | user (PX2's proposal) |
| D125 | 2026-10-28 | P-PX2-2 accepted: PxObject's functions 0x4557F0-0x455860 stay in px_collision.cpp until evidence places them | user (PX2's proposal) |

## 3. Open decisions

* **P-IB1-1** (2026-10-28, IB1; recommend accept for up to 4 per function): dead root-NULL finds
  (`Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this)`, no code emitted) as an inline-budget
  spender, like 0x6FFC80's dead 0x6F5A40 calls. IB1 used 4 in 0x6FDBD0 (fc348e7); 0x6FBEF0 would need 20 (not
  committed).
  P-DM1-1 (DM1; recommend): count only dead finds toward the 4; DM1 also used 1 dead type test in 0x6FDBD0 (without
  it acd5e81 loses 0x531590 outright) and a dead `UnknownB8.size()` in 0x6FEF70, all marked.
* **bada317's matching rule** (2026-10-28, PK1, landed without a decision; coordinator recommends keep): objmatch's
  self-name round files every vtable slot 0 `??_E` name under `??_G` too (VC6 emits the scalar `??_G` for a class never
  new[]'d; symbols.py names slot 0 `??_E`). +293 pairs (276 addresses: app 173, ww3d2 72, wwmath 26, wwlib 11, ...),
  0 lost, whole tree verified, replay 0, udpreplay PASS. Keep it, or revert it and treat it like D94(b) / D95(b)
  (a quiet-tree round)? PK1 (P-PK1-1, recommends keep): slot 0 holds the class's only deleting destructor, so both
  names are one proof; bytes still match with relocations masked. M4's D118 numbers do not include it (its README
  lists it unmeasured: "an alias would credit 14 functions from vtable proof alone").
* **P-CS1-1** (2026-10-28, CS1; recommend accept): D123 renamed the getter; rename the setter at slot +0x24,
  `Set_Debug_Object`, to `Set_Avatar_ID` too (it sends the DEBUG packet; expected 0/0 pairs).
* **P-CS1-3** (2026-10-28, CS1; recommend one file for now): the client never inlines type 24's base constructor,
  destructor or update into type 24 though they come first in the run, which suggests two objects (the base
  0x70DBC0-0x70E2D0, types 24 / 28 0x70E2E0-0x70F030). Keep D122's one provisional file until a string names them,
  or split now.
* **P-HX1-1** (2026-10-28, HX1; recommend keep): a build aid that is not the client's code in tree code:
  `tools/re/auxroots.py`'s `SYMBOL_PAD` emits an enum of 16 enumerators at the end of the generated aux_roots.cpp,
  the only lever found for VC6's declaration-count tie-breaks (workflow.md §3.3). It carries e062fb2 (D104's members
  ungated, P-PK1-2) and 45bf5c0 (P-PK1-3's Clone form, now gains-only), +16 pairs, 0 lost; reverting both loses
  the 16. (P-PK1-3's D66 trade is no longer needed.)

* **D101's sbs criterion** (2026-10-28, LD1; recommend accept as met; since LD2's sun fix the means are 2.03-2.09 %,
  inside vidland's 1.9-2.4 %, frames 600-900 1.4-2.0 %): the means moved 0.4-0.7 points (2.6-2.8 %
  against vidland's 1.9-2.4 %), within ±1.5 points; frames 600-900 are 2.8-3.0 % against 1.5-2.4 %. LD1 reads
  "within ±1.5 %" as points of difference; the user confirms or asks for the lighting fix first.


## 4. Next steps

Milestones M1 and M2 are met (D42, D63; [milestones.md](milestones.md)). Order and detail of the
client's own code: [kernel-map.md](kernel-map.md) §6 and, for step 9's dependencies,
[kernel-map-step9.md](kernel-map-step9.md). Adding a file: [workflow.md](workflow.md) §2.1.

1. **The quiet tree's items** (the D93 evaluation is done, 2026-10-27 evening: P-E1 for the user; the eval branches
   `eval/*` stay a week, then `git branch -D` and `git worktree remove` under `.work/eval/`). Left from it: P-Bopus-1 if
   accepted (0x603D70 is not option.cpp's, O1). The quiet tree carries the matching-rule
   changes D94(b) (skip `_$E` names), D95(b) (no implicit-destructor credit to a twin when the own address is
   free) and P-P6-1 if accepted (a tie goes to the object whose client range holds the address), each with a
   full verify of every library and pairs_cmp gains-only, like D88; then 117 from `.work/p5/park117` and 45 from
   `.work/p6/park45`. Also on that quiet tree: P-H7-1 (sgistl_adapt.py drops `typename` from out-of-class return
   types; remove ini_manager's and attrib_dir's explicit specializations), P-F5-1 / the regenerated names-review.md,
   and a full verify to refresh imports.json's `in_tree_total` lines (commit_mine never commits them).
   Small follow-ups from the 2026-10-27 evening (work items, no decision): symbols.json's 0xB006F4 is `??_7ClientObjectClass@@6BMultiFixedPoolDecalSystemClass@@@` (slot 0 is the thunk
   0x709E90 to ClientObjectClass's destructor; CO1, 2026-10-28), and 0xAEDE88 is PacketClass's
   vtable (`??_7PacketClass@@6B@`, through symbols.py; then rebuild `.work/pseudo/`); app/packets' "(not rebuilt)" comments
   for 0x735FC0 / 0x739FF0 / 0x734740 / 0x738970; control_keys_manager.cpp to include inpkeys.h at its next edit (0x42B7E0's
   mangled name); fontmanager 0x6D0A70 with 0x6D0F70 and ManagedFontClass's block 0x6CE990-0x6D0A20; CLIENT_CHAT_LIST
   0x73CC90; Send_Data 0x82D420 after wlib's Dictionary layout; 0x42C330 with 0x7D3CC0's file.
2. Step 9 (D90) continues. co.cpp: derive ClientObjectClass from UnknownB02C90Class (client.cpp) and
   MultiFixedPoolDecalSystemClass (layout and vtable 0xB006F4 in co.cpp's notes), then the constructor 0x6F2820 and
   destructor 0x6F2FE0, with T16's co.h list in the same step (Unknown98Class's slots +0x18..+0x2C replacing
   cpacket's Unknown98StateClass, a `std::vector<ColorizationRegionStruct>` at +0xC8, declarations for 0x6F2820 /
   0x6F5140 / 0x6F33F0; clienttest's `new ClientObjectClass` stand-in changes with them), which unblocks CREATE 4; (2026-10-28, CS1: D121 / D123 renames done, co_subtypes.cpp new (D122, +33: types 24 / 28
   and type 24's base with dynamic_obj.cpp's proximity object `Unknown44AC70Class` at +0x188), clienttest 66 checks;
   0x6F3FA0 0.62 with `auto_inline(off)` around 0x6F3ED0; left: type 28's aim 0x70E850 frame / shape, retype
   `ClientClass::Unknown1144` as `HeightFieldClass *`; dynamic_obj.cpp done by DO1 (f413f9f, +11: the client's
   `DynObj`, 0x44AC70-0x44B740; NOT EXACT ctor 0x44AC70 shape, copy 0x44ADA0 register; its `ProximityObject` is a
   `PxWrapper`, px_wrapper.h provisional); px.cpp and px_wrapper.cpp done by PX1 (fea8f8a, 8ad641d: +39; the client's
   PxBox / PxList / PxCollision, client.h's PxClass is PxList; NOT EXACT 0x44BBD0 / 0x44BC10 order, 0x44BC90 / 0x44C900
   budget, 0x44C2E0 / 0x44C520 / 0x44C690 register, Update 0x456B60 shape); the other px files done by PX2 (4a00c53..
   f4e0414: +80; px_box_list, px_process, px_callback_type / poll / accurate, px_collision, unknown_px_callbacks.cpp;
   packet-flow.md §2.6: ClientClass builds the PxList / PxProcessClass and SClient pushes / pops it, but
   PxProcessClass::Process 0x455CC0 has no reference and nothing inserts a box: the proximity / collision system never
   runs in retail; NOT EXACT Push 0x455D10 shape, Pop 0x455F60 register, 0x453B10 frame, the accurate callbacks'
   constructors / Clone budget (`__stl_next_prime` and the hash_map setup out of line), 0x450AC0 order). Left:
   px_callback_accurate's overlap handling 0x44E470-0x44F200, On_Enter / On_Exit 0x44F5D0 / 0x44F6E0, its hash_map /
   vector instances 0x450B40-0x451B60 (FAIL_IF line 170 inlined into five: definition order); the factories
   0x452140 / 0x452460; px_collision's 0x453C20, 0x453F10, 0x454730 (OBBox separating axes); sclient.cpp's
   Unknown_455D10 / _455F60 calls become PxProcessClass::Push / Pop (sent to SC1); 0x44D040 is ui_registration.cpp's
   (113f8cf); docs/attempts/726100's stash still says `new PxClass`; asserts.txt files px.cpp's `!(low && high)` 0x44BA70
   under dynamic_obj.cpp (a file-boundary error in its generator); the
   auto_inline lever on other NOT EXACT app functions that call a same-file function out of line; clienttest's
   StarField / OrbitalManager stand-ins for P-LD3-1) (2026-10-28: co.cpp's every function outside aux_roots' range 0x6F6000-0x6FBEF0 is in source (DM1, +15); NOT EXACT 0x6FE1E0 40 rows, 0x6FCB70 47, 0x6FEAC0 register, parked 0x6FEC40 order / 0x6FC9A0 shape; next lead: `Unknown_Find_Valid` and the inline-member form on 0x6F3FA0, 0x6FBEF0, Get_Name; SUBPARTS 180 is NOT_DRIVEN until clienttest's fake models get slot +0xF8 and an asset-manager stand-in (P-DM1-2); effects 9-14 exact, eff_o.cpp new (EF1) and its factory 0x6E9490, pass helper 0x6EB9B0, 0x6E7AA0 / 0x6E82A0 and P-EF1-1..3 done (EF2; Unknown_725AD0 declared on UnknownB02C90Class, whose region 0x725050-0x7260FF holds it); left: 0x6E8890 frame (E&B's lineseg.h may differ: P1 - P0 and the len2 test stay in the FPU in the client)); CREATE 4 0x733700, CHANGE_BASE_ASSET 38, Set_Model 0x6F33F0, the loader 0x6F5140 / 0x6F51D0 exact (CR1); work item: RsTrackClass is declared in both cpacket.cpp and co.cpp, move it to app/shared/rs_track.h line-neutral; RenderHookClass's 7-byte destructor 0x6F36F0 wants a D99 exact row; co bases, ctor / dtor exact (CO1); postate.cpp 0 -> 96 pairs, 13 states (PS1); work items: cpacket.cpp to include postate.h and drop its own copies of the state classes, with a verify for line shifts; the .bss int Unknown_BE71D4 now defined in postate.cpp moves to its owner, CLIENT_SET_TIME 0x737960's file or sclient 0x7472A0's, when 0x737960 is rebuilt; postate's NOT EXACT 0x718830 shape, 0x7179A0 / 0x717430 inline budget: tryv2 sweep; extrap.cpp written whole by EX1, 0 -> 63 pairs, 11 NOT EXACT, first lead the negation / flag form shared by 0x7102B0 and 0x710530; the per-frame update 0x6F3C60 exact, 0x6F4610 inline budget and Process_AI 0x744260 shape (UP1); next on that path: 0x6FBEF0, 0x6FD310, 0x6FDBD0, 0x6FE1E0, Do_Frame 0x742B60, P-UP1-1 and Do_Frame 0x742B60 done (DF1: clienttest section (4) runs packet -> state -> transform for real, 45 checks); 0x6FD310 / 0x6FDBD0 / 0x6FBEF0 written NOT EXACT; next: the shared inline-budget cause (P-DF1-1, compare auxdata.h's Unknown_Find_Field with 0x7079B0's size), 0x6FE1E0 the damage state; delta_array.h's bounds accessor stays co.cpp-local until the 0x6FB1E0 canary is understood (P-DF1-2))
   the base's slot +0x04 0x7251C0 (pure placeholder) and its attach / detach rows. cpacket.cpp: CREATE 4, the effect
   family 10/11/... (9's pattern), FORMATION 65, NAME_DECAL 178, CAMERA_CONTROL 146, CLIENT_DAMAGE 100,
   CLIENT_CHAT_EVENT 165; (2026-10-28, CP1: 6f52820..4cb9352, +12: every object-model handler in source, GADGET 103,
   INCAPACITANCE_DETAILS 144, FORMATION 65, VERB_LIST 91, STARBASE_ROOM_CHANGE 160, recustomize 129 / 131 / 133, GM
   234 / 235, CTA_RESPONSE 189 exact; NOT EXACT START 5 / STARBASE_AVATAR_CHANGE 158 register, CLIENT_SET_TIME 52
   shape, ADVANCED 62 / SIMPLE 8 drift, DAMAGE 100 frame; P-T16-2 and P-SC1-1 (sbaseview.h) done; cpacket.cpp 89
   exact / 8 NOT EXACT / 10 not rebuilt: TRADE 31, MessageChatstream 95, FIND_MEMBER 83, NoteContent 184,
   CLIENT_CHAT_LIST / ERROR 164 / 166, guild 200 / 204 / 208 / 216; clienttest 87 checks) (2026-10-28, SC1: 00de1a9..cf59a64, +18: P-MV2-2, P-T18-2 (gen_stubs writes `const
   RefCountPtr<T> &` / by-value RefCountPtr stand-ins), PxProcessClass::Push / Pop, the steering inputs
   0x745AA0-0x745E40, the GroupInfo tests, 0x74B600 / 0x74B640 / 0x74D730, 0x751660, 0x74E580, 0x750B60 exact;
   NOT EXACT 0x744C10 shape, 0x7429B0 register, 0x74C3B0 / 0x745750 / 0x7436A0 / 0x74EB00 budget (0x728300 was an
   inline member of client.h); left in sclient.cpp: 0x74C8D0, the chat commands 0x74ABC0, the dialogs 0x74E710,
   0x74EDB0, 0x74F5C0, 0x74FA30, 0x74FEC0, 0x750260, 0x750800, 0x750C20-0x751460, Print 0x74DF70 (its hand stub feeds
   clienttest's trace), CHANGE_BASE_ASSET's 0x74D950 after P-SC1-1 (move cpacket.cpp's SBaseViewClass into its own
   header, line-neutral); Init could call the inline Unknown_744D80) P-T16-2 (Unknown831340Class's 4th constructor parameter is `const RefCountPtr<ItemBaseInfoClass> &`,
   with client.cpp's "Null Item" call); P-T16-3 (one Opus round on 62, then Fable). sclient.cpp: P-T18-1 (the
   rebuilding commit deletes the member's hand-written clienttest stub: Do_Frame 0x742B60, Render 0x744690, the chat
   commands 0x74ABC0, Print, Unknown_34 / _64, ...), P-T18-2 (gen_stubs: class-by-value returns and template
   parameters, unblocks 0x744C10 / 0x744D80), 0x7429B0 (line 749), 0x7436A0, 0x745750, 0x745AA0-0x745E40 (need
   ClientClass +0x10D4's type), 0x74C3B0, 0x74D570 / 0x74D690 (aux data; auxdata.h in sclient.cpp breaks kv_pair.h's
   Type_Name overloads, C2665), the dialogs 0x74E580-0x751660, the destructor's `++it` loop once the UI base exists
   (P-T7-2). P-H9-3 / P-H11-1 / P-H12-3 / P-H13-2 / P-T11-1 / P-H18-1 (stand-in names and twins switched to the real
   headers at the files' next edit; Model -> RenderObject; LoginUnknown0CClass from inpdsp.h); the Fable round D92
   for ClientClass's constructor and Find_Object / Remove_Object. Parked after two rounds: Unknown_50, the fast start
   0x76A370, Pre_Run, Run, Start_Game, Go_Next/Back_Step, SClient's destructor / Init / Shutdown.
3. The packet classes: 260 of 260 declared, ~255 rebuilt (2026-10-28: PK1 emptied pk_classes.h, VERB_LIST 91, 82's raw
   ctor / Build, P-P7-2, P-P9-2, prof.cpp / race.cpp; PK2: the value constructors 0x8A0380 / 0x89FD80 / 0x85C800 /
   0x864090 exact with the union kept (ShipCustDescClass's `operator=` written out inline, ccd455d), GLOBAL_ERROR
   0x864A10, 230 / 231's sizes, AuxData Print 0x8770A0: +9; 181 / 182 were already exact (P10, 9e54d08); the
   decoder's destructor failures are all hand-rebuilt; `.work/pk1/tv.py` / `hv.py` duplicate tryv2.py and are not
   promoted). Left: 45 and 117 (item 1); the inline-budget group where the client inlines a large callee but calls
   its deepest small callees out of line (VerbList ctor 0x8AF150 0.82 and Print 0x8AF6D0, VerbUpdate 0x8AFD10, DECAL's
   raw ctor 0x8807B0; neither `inline_depth` nor the declaration count explains it: a Fable candidate); COLORIZATION
   0x87F320 / DECAL 0x8805C0 shape, 0x867250 register (SIB order of `_Eos`); P-PK2-2: pkverbrequest.cpp and
   pkverblist.cpp are one client object 0x8AEAE0-0x8B15C0, merge them with a D66 (v) move. Tools to promote:
   `.work/p9/wt.py`'s leftovers, `checks.sh`, `commit_wt.py`; commit_json.py must keep imports.json's maps sorted.
4. The SGI hash_map's users (D87): left are the key-name object after inpdsp (0x42B610 / 0x42B7E0, init 0x42BC90),
   client_lego_impl's seven NOT EXACT (Get_Lego_Transform 0x72B000, Child_Moved 0x72CE40, Attach 0x72D0E0
   Fable-eligible, P-H17-2; Add / Remove one or two EH states off), fontmanager's 0x6D2700 (a block ending before the
   G_ASSERT) and 0x6D2580 (the RefCountPtr-local lever), 0x6D0F70 (probably 0x6D0A70's file), control_keys_manager's
   P-H11-2 pair and Load_Definitions 0x71B1F0 (Fable-eligible), inpdsp 0x429F60 (register), the hash-loop residues
   (P-H10-1: 0x5A7E80, 0x5A8100 with 0x677410, 0x6ACD10 as one batch when a lever names the loop-register cause),
   kv_list's 0x4B6B60 (why 0x4B7030 is a one-caller find instance), co_player_relationship and option.cpp after the
   eval. P-H17-1: note in libs/sgistl's README that `insert_unique_noresize` fails in VC6 (C2899); the client inserts
   through `operator[]`.
5. Land (M2.4, D46): P-L6-2 and P-L6-1 done 2026-10-28 (LD1: 30431ad, 84a3871, 614854d; VIDTEST on LandClass,
   vidland.* deleted, viddeco.* draws the trees; drawcheck PASS at frame 900, sbs mean 2.6-2.8 % against vidland's
   1.9-2.4 %, frames 600-900 2.8-3.0 %; LandLodDistances has 8 entries, not 6). LD2 (7a79042, 3457856, 7916522,
   06166a9: app +25): the lighting difference was VIDTEST's sun input (acos, the lightmap now the original's), P-LD1-1
   done (Create_Decorations 0x479D00 / Build_Decorations 0x47A2F0 fill the patch records, viddeco.* deleted,
   drawcheck f900 every group the original's, sbs mean 2.03-2.09 %); NOT EXACT Save_Decorations 0x479A50 frame,
   0x479D00 / 0x47C580 / 0x47C6A0 shape, 0x478B50 budget, Build 0x47A2F0 parked shape; the global byte 0xBFAE52 set
   during the prototype load is unknown. Next: D119's work item (Land_Set_Ref back to the client's `Create_*`), D120
   (the `--art build/art` default), the mainview zone loader 0x5137F0, the ini writer 0x4791D0 (all done by LD3,
   17a366f, 029e4f9, fa4255e NOT EXACT frame, 3551b97 mainview.cpp new with 0x5137F0 exact, 2530d1f wind 0x513B90
   exact; the stars' 0x513700 is exact in `docs/attempts/513700/` and lands once clienttest's fake main view has
   StarField / OrbitalManager stand-ins (P-LD3-1); Build's RefCountPtr form parked: the client calls
   insert(it, n, x) 0x47D690 out of line); mainview.cpp (MV1: ea1f012, bd26d4b, 275d5a2, a7ba945:
   +32, MainViewClass typed +0x48..+0x1E8, Add_Object / Remove_Object, the render path; NOT EXACT ctor 0x50B360 order,
   0x50D060 register, the three Render_*_Mode; left ~55: Update 0x50F1B0, targeting 0x512400, picking +0x60 0x512BB0,
   overlays 0x50D520, profile text 0x510440, sea 0x510D40; Init 0x50BA60 is exact-but-registers in
   `docs/attempts/50BA60`; HN2 (5756e57, 3485e3c, b5f4b4c, 5e05b58, 8a15feb, 91c6ad9, 33d0524: app +4, ww3d2 +1):
   stars 0x513700, 0x513540, 0x513600, Shutdown 0x50C2F0, WW3D::Set_Gamma 0x919090 (WW3D_API_PATCHES) exact,
   0x513CD0 / 0x513D20 ambiguous twins; NOT EXACT Init 0x50BA60 / slot +0x08 0x50C690 register, 0x5146D0 shape (the
   client keeps * 0.01 and * scale apart; same at 0x5C25C0), 0x514010 shape; UIGroupClass in ui_group.h; the
   interface objects share a 0x38-byte UIUnknownBaseClass plus a RefCountClass at +0x38; clienttest's fake main view
   carries StarField, OrbitalManager and traced fakes. MV2 (9ecbfee..207c00c: +13): P-HN2-1 / P-HN2-2 done
   (symbols.py's hand-named source `tools/re/symbols_named.json`, `--apply-named`), 0x512A00 and the profile text
   0x510440 exact; NOT EXACT Update 0x50F1B0 frame (DurationClass::operator+ out of line costs aux_roots' pad),
   targeting 0x512400 budget (the client calls its find copies 0x514B60 / 0x514BD0), picking 0x512BB0 shape,
   overlays 0x50D520 budget, sea 0x510D40 shape; every mainview function has been tried. Left: P-MV2-1 (+0x178..
   +0x1A4 is a LandLightingStruct: a real member once its own header reaches sclient.h, with the ctor 0x50B360),
   P-MV2-2 (SClientUnknownWindowClass and UIUnknownBaseClass are one 0x38-byte base: merge), P-MV2-3 (anything
   added to duration.h re-sweeps auxpad.py); 0x50B930 untouched; the Doppler flag 0xC404F8 lives in mainview.cpp until its file is known
   (P-MV1-3)); Load_Assets 0x4691D0 (the pragma keeps the patch ctor / Init
   out of line, frame left), Update_Patches 0x46B3C0 (frame, PatchSize's unsigned conversion), Get_Triangles
   0x465D30 (Fable-eligible); the
   parked rows (Compute_Lighting order-only, Update_Patches / Compute_Horizons shape, the scanners' frame,
   Cast_OBBox / Cast_Ray frame, Get_Triangles budget, Render shape, Load_Assets budget); the stars' Load / Init.
6. Step 8's tail: Key_Exchange (LIMIT), the UDP constructor over a CordCon 0x82CBF0 (register), Open_Fixed_Channel
   0x82D8C0 (field offset), `Send_Data` (wlib's Dictionary layout), 0x82E1D0 (needs CordMgr 0x8598F0), the
   TimedLog ctor, the memconn option slot, Rate Count (LIMIT), Listen.
7. The Fable queue under D89 / D97 (six at most, by unlock value): co.cpp's 0x6FD310, 0x6FBEF0 and the roll 0x6F3FA0 as one round (2026-10-28, IB1: two Opus rounds each, inline budget; the question: what in the client's source spends more budget per find; 0x6F4610 / 0x6FDBD0 left the queue, now register / shape), client_lego_impl's Find 0x72ABB0 + Add 0x72B7F0 +
   Remove 0x72C1D0 as one round (P-H20-1), CLIENT_DAMAGE 0x738970 then FORMATION 0x734740 (P-T19-3), D92's three
   (item 2), Get_Lego_Transform then Child_Moved / Attach (P-H17-2), cpacket 62 with SIMPLE 8 as one round (both
   rounds done, CP1 2026-10-28: the Is_Valid_Float cut follows the inline budget; left the drift divisor's CSE and the
   state pointer in ebp, one fix likely gains both),
   Load_Definitions 0x71B1F0; low priority: TimedLog 0x824540 (P-S1-1), option.cpp Get_String 0x7F9700 (D108), CLIENT_CHAT_EVENT 0x73B150 (P-T20-1), the kv_list
   destructor's twin 0x4B6B60 if a lever appears, the distortion draw 0x9961E0 (LIMIT), the ShdMesh ctor 0xA08F80
   (LIMIT). Out for good (register/order): Make_Screen_Shot, Update_Movie_Capture's LockRect residue, Rate Count,
   Run/Start_Game/Go_*.
8. Tools: done 2026-10-27 (commit_mine `--file`, attempts.py line endings, build.py's UNRES regex). Left: P-LD3-2
   (`wt.py bring-over` accepts named D119 ambiguous addresses, checked against the ambiguous column) with P-CS1-2
   (the same flag for D66 (i) / (v) moves when pairs_cmp's lost-outright list is empty); promote
   P9's worktree scripts (item 3); the compare-and-swap commit rule (`update-ref master NEW OLD`) into commit_mine
   (H17's race); gen_stubs' by-value returns (P-T18-2); H16's `.work/h16/fm_gen.py` / `commit_h16.py` pattern for
   generated files.
9. Leftovers: the shaders' `multiple` functions (D88 credits what it can), Compute_Caps 0x9B0160 (parked), the
   meshmatdesc noise (D18/D28), `CreateBrowser`'s app callers 0x7CA5C0 / 0x7CAD00 (E&B's `querydata`); P-H11-2
   (the _Instances destructor frees nodes without element destructors: leave unbuilt until another SGI user
   shows it); `tiebreak` / `selfname` into imports.json only when something needs it (D88 row); same-shape template
   instances stay `multiple` until their relocation targets have names in symbols.json (P9).
10. Moved from §3 on 2026-10-27 (work items and reminders, not decisions): P-T5-2 (LoginPacketDataClass /
   LoginInterfaceAEADC8Class into pk.h / delta.h, tsk_login.h's typedefs dropped at tsk_login.cpp's next edit; its
   visitor part is D104); P-T12-2 (aux_roots.h's bool `Type_Name` / `String_To_Value` overloads to strconv_lib.h through
   the aux_roots generator, when it is next touched); Key_Exchange 0x829180 (LIMIT REACHED, three Opus rounds; waits
   for a new §3.1 lever; F3's two untested leads are in its attempts.csv row, P-S3-2); D7's reminder (revisit
   byte-exactness of E&B's code with the user if it becomes too costly; `docs/data/not-exact.md` shows how much
   functional code waits); which bool-return form gives a bare `setcc al` without the `xor`.
