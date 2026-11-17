# Record of the week of 2026-11-10 (usage week Tue 11-10 12:00 to Tue 11-17 12:00)

Sources: archive/week-plan-2026-11-10.md; progress.md log entries 2026-11-10..16; `.work/run-2026-11-*/plan.md` heartbeats
(`weekly_all used N%`); `docs/data/progress-history.csv` (one row per day: exact functions, pairs, per-library fn counts).
`docs/coverage.md` is NOT usable for the deltas (identical at fecd64c81, c657ef372 and a5a3fde4d), the history CSV is.
"Pairs" = the log entries' "+N pairs over the run's baseline".

## 1. Counts at the week's start and end
| | Start: CSV row 11-10 (f4a1453a, run 11-10 head 13:05; master c657ef372 at the 12:00 reset) | Row 11-09 (c7b83f48; master fecd64c81, before the two old-week runs) | End: row 11-16 (5d1d72af; master 0877e3301, same counts at a7e711730 / 71d8ed112) | Delta start -> end |
|---|---|---|---|---|
| Exact functions, all | 20,414 | 20,143 | 25,034 | +4,620 |
| app exact (fn_app) | 13,955 | 13,682 | 17,893 | +3,938 |
| libraries exact (10 libs) | 6,888 | 6,888 | 7,667 | +779 (ww3d2 +322, wwshade +109, wwmath +87, wwlib +63, wwaudio +57, wlib +51, wwsaveload +33, wwdebug +26, binkmovie +23, wnet +8) |
| Pairs (file, address) | 30,896 | 30,303 | 41,260 | +10,364 (+10,957 from the 11-09 row) |
| Bytes exact / not exact | 3,584,384 / 640,172 | 3,529,440 / 601,052 | 4,371,376 / 823,798 | +786,992 / +183,626 |
| Unattributed bytes | 1,852,865 | 1,951,329 | 886,295 | -966,570 |
| NOT EXACT functions | 758 | 719 | 846 | +88 |

README card: 49.25 % byte exact (20,333 fn) on 11-10 -> 60.45 % (25,034 fn) on 11-16; .text rebuilt 65.13 % -> 76.35 %.
The 12 sessions' "+pairs" sum to 10,539 (section 2); CSV delta 10,364: the 175 gap is tools PRs, accepted-corrections and
rows outside run baselines. Two runs sit before the reset and are not in the week: 11-09b (+416) and 11-10 day (+178), meter 96-98 %.
The 11-15b row first read 24,291 / 39,107 (73787f085); the 11-16 row is 25,034 / 41,260.

## 2. Per session (meter = weekly_all % in the run's plan.md; pts = last minus first)
| # | Run (date) | Coordinator / builders | Meter | Pts | Pairs | Pairs/pt | Kind of work |
|---|---|---|---|---|---|---|---|
| 1 | 10b (11-10 night) | Fable coord; 11 Fable builders (3.04M tok), then 10 Opus (2.1M) from 23:05 at the floor; Sonnet 0.9M | 4 -> 19 | 15 | 813 | 54 | map/queue (574 rows), 3 stuck fns, 18 new files; Fable 360 pairs/11 pts, Opus 456/3 pts |
| 2 | 11 (11-11 day) | Fable coord; 8 Fable (2.4M), 5 Opus (1.9M), Sonnet 0.6M | 19 -> 35 | 16 | 619 | 39 | 8 layouts + shared headers, 28 files drafted; ~6 pts/h |
| 3 | 11b (11-11 night) | Fable coord; 9 Fable (3.8M), 21 Opus (4.6M); cut off 03:05 at 64 % by credits, drained 08:15 | 36 -> 64 | 28 | 969 | 35 | D236-240, 3 kept branches, 5 layouts, ~30 files, library classification |
| - | reset 11-12 08:15 | Fable usage subtracted | 64 -> 16-17 | | | | |
| 4 | 12 (11-12 pm) | Opus coord; 11 Opus + 1 Sonnet builder, no Fable | 17 -> 20 | 3 | 853 | 284 | kept branches landed; D242 crediting +646 (~1 pt); D243/D244 renames (0 pairs); files/finishing ~207 |
| 5 | 12b (11-12 night) | Opus coord; F1 Fable (248k) + 19 Opus | 20 -> 30 | 10 | 535 | 54 | layout b-9; new files 449 (5.5 pts), finishing 84 (4 pts) |
| 6 | 13 (11-13 day) | Opus; 17 Opus builders | 30 -> 38 | 8 | 864 | 108 | 32 new files ~830, sclient B9 items 35, cl limit fix |
| 7 | 13b (11-13 night) | Opus; 30 Opus builders | 38 -> 46 | 8 | 1,529 | 191 | 30 new files ~1,446 (7.5 pts), finishing ~83 (1.5 pts) |
| 8 | 14 (11-14 day) | Fable coordinator (user's /model); 17 Opus builders | 47 -> 53 | 6 | 112 | 19 | e2e harness: 25 scenarios, proxy hooks, recorded session; 7 files ~85 pairs |
| 9 | 14 night (plan-s9.md) | Opus; 23 Opus + 18 agy Gemini 4 Hydrogen (13.9M stream tok) | 53 -> 61 | 8 | 417 | 52 | last client-name/vtable/hint rows, 23 files; agy ~255 of the 417 |
| 10 | 15 (11-15 day) | Opus; 2 Fable (LV1 195k, LV2 190k), 20 Opus (4.3M) | 61 -> 72 (user's other project on the meter from ~12:00) | 11 | 956 | 87 | crediting + names ~827, new files ~101, levers/finishing ~30 |
| 11 | 15b (11-15 night) | Opus; 12 Opus (3.0M) + Sonnet; no Fable | 75 -> 80 (72 -> 75 gap = other project) | 5 | 716 | 143 | FE funclet round 423, queue rows 273 (QD 163 = one Get_Priority credit), levers/finishing 16, names/tools 4 |
| 12 | 16 (11-16 day) | Opus; 15 Opus (~2.95M); 3 agy (A3 stopped at the agy floor); antprox coordinator shared the meter from 09:10 | 80 -> 90 | 10 | 2,156 | 216 | crediting CR1-CR8 + XC ~2,100 (5.5 pts), queue rows +52, live harness 0 pairs |
| | Week | | | | 10,539 | | |

Phase A (S1-3, Fable coordinators, 4 -> 64 = 60 pts): 2,401 pairs = 40/pt. Phase B (S4-12, 17 -> 90 = 73 pts): 8,138 = 111/pt;
105/pt over S4-9 + S11 only (the sessions with an unshared meter: 5,026 pairs / 48 pts).
agy/Gemini: spent in S9 (agy week 0 -> ~45 %, cap 50 %; ~1.2 % of a 5 h window per 100k stream tokens) and S12 (A1-A3; 56 -> 31 % left;
A1 + A2 gave 29 pairs). API-key builders: none logged this week. Fable ration of phase B: 2.25 of 6 points (F1 0.9, LV1 + LV2 1.35).
S10 and S12's meters mix other sessions, so their pairs per point are low estimates.

## 3. Pairs per point by kind (log entries' own splits; points split by builder tokens, so every row is an estimate)
| Kind | Pairs | Points (est.) | Pairs/pt | Check against the 11-17 plan's figure |
|---|---|---|---|---|
| Crediting (objmatch rules, ties) | ~3,996: S4 T1 646, S10 ~827 (with names), S11 FE 423, S12 ~2,100 | ~9.3 (1 / ~2.5 / ~0.3 / 5.5) | ~430 overall; 646, ~330, ~1,400 (FE, tiny token share), ~380 | plan 380-720: same low end, mine ~430 aggregate; FE and T1 give the high end |
| New files from the queue | ~3,500 (phase B): S5 449, S6 830, S7 1,446, S8 ~85, S9 417 (255 agy), S10 ~101, S11 ~110, S12 52 | ~35 | ~100 (S5 80, S6 104, S7 190; S9 ~20 Claude-only, ~52 with agy; S10-S12 30-110) | plan ~115: close; S7 pulls it up |
| Finishing drafts | S5 84, S6 35, S7 ~83, a few more | ~8 | 20 (S5, S6), 55 (S7), ~33 overall | plan ~33: matches |
| Names / renames | S4 RN 0, S11 4, S12 NM +3 (AR +33 unlanded); S10's names sit inside crediting | ~3 | ~1-2 alone | plan ~105: NOT reproduced; names land at 0 / 0 and their pairs appear as crediting. ~105 only if it counted S10's names + crediting row |
| Harness / e2e / live | S8 112; S12 live harness 0 | 6 + 1.2 | ~19 (S8), 0 (S12) | plan ~19: matches (S8); both found behaviour bugs byte matching cannot see |
| Levers / stuck functions | S10 23 (LV1 + DU 13, LV2 5, FR2 5); S11 ~16; Fable P1/P2 28 | ~2.5, ~1.5, 3.7 | ~9, ~10, 8 | plan ~5: mine 8-10 |

By model (not tracked per builder; from the entries' splits and token counts):
- Fable builders, phase A: S1 33/pt (360 pairs, 11 pts), S2 39/pt, S3 35/pt (the last two mixed with Opus builders; S2 tokens Fable 2.4M vs Opus 1.9M). F1 layout b-9: 5 pairs / 0.9 pt (value is layout_b9.md). Levers LV1 + LV2: ~9/pt. Parked leftovers P1/P2: 8/pt.
- Fable coordinator S8: 19/pt (harness work).
- Opus builders: S1 Opus phase 150/pt (456 pairs / 3 pts); phase B 105-111/pt; best sessions S4 284, S12 216, S7 191, S11 143 (crediting or known-pattern new files).
- Sonnet (landers, checkers, doc-filer): no pairs of its own; 45-145k tokens a run, 15-25 runs a night.
- agy Gemini: ~255 pairs in S9 for 0 Claude points (client-name rows ~165, hints ~68 by the helper's mapping), outside the Claude meter.

## 4. Facts for D72 (docs/builders.md section 4; D72 / D96 rows in progress.md section 2)
- D72 / D96: Opus coordinator and builders by default; Fable only after two failed Opus attempts under D89; Sonnet for lander, checker, doc-filer, audits; agy Hydrogen for bounded items (D187). This week departed from it: S1-3 Fable coordinators and builders (D225), S8 a Fable coordinator at the user's choice.
- Fable cost measured 11-10 night (S1): 11 pts in 3.35 h = 3.3 pts/h with 2-6 Fable builders (plan assumed 2.4-3.0); ~3.5 pts per million Fable builder tokens incl. coordinator; ~1.2 pts a Fable builder (250-500k tokens); S2 ~6 pts/h with six Fable builders. S3 hit Fable's share cap at 03:05 (a model cap, not the meter).
- Opus cost: ~1.0 pt/h with 2-4 Opus builders (S1 Opus phase 1.0, S5 1.0, S6 1.4, S7 0.85, S8 0.9, S11 ~1.0); coordinator ~0.6/h + 0.1 per builder; ~1.2 (S7, 6.6M tok, 8 pts) to 2.6 (S10, shared meter) pts per million builder tokens; items 100-420k tokens each.
- Pairs per point: Opus phase B 105-111 vs Fable phase A 40 (Fable builders 33-39; Opus in the same night 150): about 3x. Order of worth: crediting ~430, known-pattern new files 80-190, finishing 20-55, harness ~19, levers ~9, Fable stuck functions 8 (all Opus except the levers and P1/P2).
- Yield decay: the client-names rows ran out in S9 (new files 190/pt in S7, ~20-50 in S9, ~30 in S12); crediting carried S10-S12.
- Limits that bound concurrency were not the meter: S5's cap, S12's importer lock and memory, S11's safety-classifier outage (00:05-00:28), S5's API 403 (both resumed).
