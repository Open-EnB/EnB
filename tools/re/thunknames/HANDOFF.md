# Thunk-order name recovery: handoff (2026-11-26, Mac session)

Two briefs: **A** for a review agent (read-only audit of the method and its results), **B** for the session that
continues the recovery after the review. Background in one paragraph, then the briefs. Method and tools:
`tools/re/thunknames/README.md`; the link rule: `docs/binary.md` §5.1-5.2 (and §5.6 for this work).

## Background
VC6 LINK /INCREMENTAL writes client.exe's thunk table (19,983 `jmp` slots at 0x401005) in the order of its
external-symbol hash table: bucket = f(hash(decorated name)), buckets ascending. A slot whose current name lies
on the longest non-decreasing bucket run is an **anchor** (its name is retail's exact decorated name); every
other slot's true name must hash into the **window** between its neighbouring anchors, and with high probability
into the **near part** (+/- 3 buckets of the position interpolated from the slot index). So a proposed name costs
one hash to check, and a wrong one lands in the near part by chance with about 7 / 15,048. This session raised
the anchors from 2,797 (IM3, 2026-11-25) to **4,720**, and confirmed retail names for **257 class renames**
(1,503 slots) plus 422 other per-slot corrections (signatures, method names, parameter types), all relative to
symbols.json's names; 1,883 of the 1,925 changed anchors are `exact` (our source declarations).

The session's per-step scratch (38 states, logs, the round contexts) exists only on the Mac that ran it
(`.work/names/`, 72 MB, untracked); everything needed to continue is tracked. Data (tracked): `docs/data/thunk-names.tsv` (the state: every named slot, current name, IM3's name, bucket,
on / off, how it was found), `docs/data/thunk-name-hits.tsv` (every accepted hit with its step, chance, null and
window), `docs/data/class-renames.tsv` (our class -> retail class, slots confirmed / open, best chance, source
round, confidence, split suspect). Model proposals: `tools/re/thunknames/proposals/` (pilot, scale, round 3: 12,546
class candidates, about 2.3M Sonnet / Haiku tokens to produce; never regenerate them, reuse them).

---

## A. Review: done 2026-11-26 evening (the brief below is kept for its questions)

Outcome and figures: `docs/binary.md` §5.6 "The review", the progress.md entry of that evening. By question:
1. **Chance model:** the multiplier lacked nothing for the model rounds (the salted null finds the class chance
   conservative, 0.06-0.21x); it lacked the dictionary sizes for round 1 and classsearch2 (rescore.py counts them)
   and the unsplit buckets' double weight (fixed: `gapguess.prob`). Slots of one class are independent enough: the
   salted null keeps their correlation and stays under the model.
2. **Nulls:** the cross-class null is a search (both its hits are probably true names); the salted null replaces
   it; 1e-4 stands (0.008 expected false classes per run of 373).
3. **Cascade / the 8 lost anchors:** all 8 were IM3 base names (no accepted hit is off in the final state): names
   that were anchors only by lying in their window. rescore.py leaves each class's slots out: 35 anchors in 32
   renames are not near without their siblings; only LoginGalaxyClass -> GalaxyInfoClass depends on them (1 of 4).
4. **Weak acceptances:** 9 renames weak (class-renames.tsv `verdict`), 73 single-slot hits probable (the state's
   `grade`). BuffDefStruct -> BuffInfoClass, TargetSummaryText -> TargetSummaryStrings and SubjectClass ->
   ObserverSubjectClass keep.
5-8. **Not settled by this review** (still open for B): the split classes beyond the 2 flagged, tier1c's
   `name.replace(old + '@', new + '@')` over-match, NAME_START against template arguments, the kept kinds' nulls
   at the name's tail. No name in the state spells an identifier twice (no missing back-reference).

Angles not tried yet, in the review's order of expected value: (a) score a class on every slot that spells it,
parameter types included (1,241 off slots name an unconfirmed class only as a parameter type; 31 such classes have
at most 2 qualifier slots); (b) the cross-class pass as a search with its multiplier (12,546); (c) token edits of
our name with retail's own vocabulary (223 of the 257 confirmed renames use only our tokens plus the 792 tokens of
the anchors' identifiers; 158 need at most one new token), for the 249 unconfirmed classes with 3 or more special
member slots; (d) one method name searched jointly over an override group (same vtable index and signature) or a
Get_ / Set_ pair on one field; (e) a second incremental build of the client (a patch, a beta) as an independent
table for the same names: an outside input, the user's call; (f) the order inside a bucket as a one-bit check.

### The brief as given (Opus, read-only; no edits to the state or docs/data)

**Goal:** an independent read of whether each acceptance rule is sound and whether false anchors could have
entered and spread. Report findings ranked by how many anchors they put at risk, each with the exact slots /
classes affected and a test that settles it. Do not re-run the model rounds. Local compute is free (an 18-core
Mac or the Windows workstation); a full re-score of the saved proposals takes under a minute.

**Read:** `tools/re/thunknames/README.md` (method, the steps in order), `docs/binary.md` §5.1-5.2 and §5.6, then the
code in `tools/re/thunknames/`: `gapguess.py` (state, windows, near part, cascade, conflict handling), `tier2_eval.py`
(class scoring and its null), `classsearch2.py` (own variants, dictionary, its null), `tier1c.py` +
`t1c_keep.py` (convention variants, the kinds kept), `propagate.py`, `mangle.py` (the shared name-boundary rule).

**Found while porting (2026-11-26, after the data were produced; the reviewer should weigh these first):**
- **The null has hits.** With the null sample made deterministic (`--seed`, below), 5 seeds x 300 other-class
  candidates x 373 classes gave 2 null hits under 1e-4, both in seed 0: StringValueClass with KVStringAccessorClass
  (1.1e-7) and DeltaClass with DeltaItem (1.9e-6). Either sibling classes' candidates include true names (the null
  is then not a null), or the threshold is not safe. Settle which; the 1,450 accepted hits were all judged against
  the old null.
- **The old null was not reproducible:** it seeded its sample with Python's `hash()` of the class name, which is
  randomized per process. Now `zlib.crc32(name) + --seed`. The data were produced with the old one.
- **8 anchors were lost along the way:** slots on the order in an earlier step's state are off in the final one
  (a later hit reshaped the longest run). List them (`.work/names/gaps/<step>/state.tsv` against the final state,
  on the session's machine; or recompute by replaying the hits in order) and say whether the later hit or the
  earlier anchor was wrong.
- **Two behaviour changes in the ported tools** (not used for the data): `mangle.NAME_START` no longer renames a
  method name equal to the class (`?Foo@Foo@@`: the old copies did), and `propagate.py` skips possible class splits
  (ProjectorManagerClass and RawPacketClass: an anchor still spells the old name). Check both are right.
- **One pending hit:** on the committed state, `classsearch2.py --no-dict` and `tier2_eval.py` both accept
  SlotDataShortcutGadgetClass -> SlotDataShortcutGadget (3 slots, own variants 1.1e-5); not applied, left for B.

**Questions to settle (the author's own doubts, most important first):**
1. **The chance model.** Class score = C(n, k) x prod(forms_i x near_i / 15,048) x candidates tried for the class.
   Is the multiplier complete (all rounds' candidates for that class, own variants, access / const / ??_E forms)?
   The near part is interpolated: are slot hits independent, or do neighbouring slots of one class share a window
   (anchor pairs) so that hits correlate? IM2 / IM3 found the modelled chance ~10x optimistic for random strings.
2. **The nulls.** tier2_eval's null tests 300 other classes' candidates per class (same word distribution); none
   of 415 classes reached 1e-4. Is that the right null for the accept threshold 1e-4 given 415 classes tested
   (family-wise rate)? Recommend a threshold or a per-class correction. classsearch2's dictionary null was one
   random chunk scaled x12: check the scaling.
3. **The cascade** (and the 8 lost anchors above). Every accepted hit becomes an anchor and narrows its neighbours' windows. A false anchor
   would mis-place neighbouring windows and could admit further false hits or reject true ones. Test: for each
   accepted step, drop its anchors and check the later hits still fall in their (re-widened) windows; list hits
   that depend on a single weak anchor.
4. **Weak acceptances.** class-renames.tsv's confidence C (14 renames with 2 anchors or fewer, e.g.
   BuffDefStruct -> BuffInfoClass, TargetSummaryText -> TargetSummaryStrings, SubjectClass ->
   ObserverSubjectClass), round 1's 9 doubtful names (instrings.log: DecorationStruct, StringInfo, ...), the
   `NO_SLOT@InvDataClass` string hit (an enum, not a method?), tier1c's single-slot `suffix` hits.
5. **Propagation over class splits.** propagate.py applied every confirmed rename to every name, qualifiers and
   parameter types (the split guard came after the data). Where our one class is two retail classes (RawPacketClass -> PacketClass confirmed on 3 of 22
   slots, while retail's `?Build@GuildPlayerPermissionsPacketClass@@UAEPAVRawPacketClass@@XZ` keeps
   RawPacketClass; CharacterViewAvatarClass -> AvatarClass 15 of 126), propagation writes wrong names into off
   slots. Off names do not set windows, so anchors are safe, but later guesses start from them. class-renames.tsv
   flags only 2 suspects (CharacterViewAvatarClass is not among them): find the rest and say which renames must
   not propagate.
6. **Substring renames.** tier1c's confirmed-rename path uses `name.replace(old + '@', new + '@')` (no boundary):
   renaming `Packet` would also hit `GuildPacket@`. Check whether any accepted hit came from such an over-match.
7. **The two regex bugs found this session** (a qualifier after `??0` / `??1` / `??_G` / `??_E`, and a class after
   its type code `V` / `U`): `mangle.NAME_START` is now the one rule. Grep for any other boundary logic, and check
   NAME_START against back-references (digits) and template arguments (`?$Name@V...@@@`).
8. **Signature kinds.** tier1c keeps gE, int, ptrcv, const (and suffix+edit) at >= 5x their null; IM3 found case,
   ret, callconv, virtual, static at chance. Are the kept kinds' nulls (window shifted 30-300 buckets) valid
   given the hash's poor mixing at the name's tail?

**Deliverable:** a findings list (rule, risk, affected slots / classes, settling test, recommendation) as the
final report, plus any settling tests run, with their numbers. Proposed changes to the rules go to the
coordinator / user, not into the data.

---

## B. Continuation brief: a runbook (written for a smaller model or a fresh session; follow it in order)

**What you are doing.** Running the local-model rounds that find retail's class names (for the classes still
unconfirmed) and method names (for the `Unknown_XX` methods of classes already confirmed). The local model
(Qwen 27B on this Mac) proposes names; the hash test decides; **the coordinator is a script, `run_local.py`**.
Your job is to start it, wait, read its report, and write the log entry. You do not judge names yourself.

**Rules that are not yours to bend.**
- Never hand-edit `docs/data/*` (the tools write them). Never push `master`; commit only when the user asks.
- A name the tools did not accept is not a finding, however right it looks. Put such ideas under "Candidates to
  look at" in your log entry.
- Do not change a threshold, a stop rule or a tool to get more hits. If you think one is wrong, write why in the
  log entry and tell the user.
- No cloud model rounds (the user approves those per batch). Local compute is free.
- Output discipline: the commands below log to files and print summaries. Do not `cat` logs; use `tail -20` or
  `grep -c`.

**Step 0: setup and checks.**
```
export ENB_CLIENT_EXE=$PWD/.work/client.exe        # Mac; on Windows the default install path is used
git status --short | head                          # know what is uncommitted before you start
python tools/re/thunknames/gapguess.py state       # expect the figures of "Where things stand" below
curl -s -m 5 http://127.0.0.1:8080/v1/models | head -c 200      # the local server (model qwen3.8-27b)
```
If the server does not answer: it does not survive a reboot and another session set it up; tell the user and stop.

**Step 1: start the run** (in the background; it takes hours: a class round about 2 hours at 6 samples, a
method round about 3 hours at 2 samples over 1,833 targets):
```
python tools/re/thunknames/run_local.py --prefix n2 > .work/log/run_n2.log 2>&1
```
What it does, by itself: checks; settles the state (`loop.py`); class rounds `n2c1`, `n2c2`, ... (propose ->
score against the salted null -> apply the accepted classes) until two rounds in a row accept nothing or 6 rounds;
then method rounds `n2m1`, ... until a round accepts no override group and fewer than 10 single hits, or 4 rounds.
It can be re-run with the same prefix after a stop: finished rounds are skipped.

**Step 2: read the report** when it ends: `.work/names/run/n2/report.md` (also the end of the log). Per round it
has the accepted classes, the method lines (`override groups accepted`, `single-slot hits: N ... the salted null
expects F false`), and the state line after each apply. A `FAILED` line names the step; its full output is in
`.work/names/round/<tag>/` or `.work/names/methods/<tag>/`. Report failures; do not patch around them.

**Step 3: the weak renames.** `awk -F'\t' '$13=="weak"' docs/data/class-renames.tsv | cut -f1,2,4,10-13`: list
them in the log entry (a rename that gained anchors may now read `keep`). Do not delete them.

**Step 4: report.** Prepend a dated entry to `docs/progress.md` §1 (copy the shape of the 2026-11-26
thunk-name entries): rounds run, classes accepted per round (the names), method names per round with the null's
expected false count, anchors before / after (`on`, `confirmed`, `probable`), the weak renames, failures,
candidates to look at. Update "Where things stand" below. Then tell the user and wait: the commit and the PR are
the user's call.

**What the names are worth (say this in the entry, do not round it up).**
- A class rename accepted by `tier2_eval` (chance < 1e-4 against the salted null) and an override-group method
  name (`t2-lvmethod`) are `confirmed`.
- A single-slot method name (`t2-lmethod`) is `probable`: the trial below had 13 hits where the null expected
  1.9 false (about 1 in 7 wrong). Probable names bound no window and are not evidence for a source rename by
  themselves; builders use `confirmed` grades and class-renames.tsv's `keep` rows.

**What this session measured (2026-11-26; the settings above come from it).**
- Class pilot, 42 classes round 3 confirmed (a find = the retail name among the candidates): thinking off, 1
  request per class: 13 in 82 s; thinking off, 6 requests at temperature 1.0: 23 in 423 s; thinking `low`, 1
  request: 16 in 968 s. So thinking off, many samples. Repeat with `tier2_local.py --pilot` after a model change.
- Class round `local1` (454 open classes, 3 samples, 23,453 candidates, 58 minutes): 10 classes accepted (8 new:
  GuildPlayerOnlineSectorPacket, GlobalLockBasePacketClass, GameEditorKVModifyPacketClass, HTMLViewer,
  ClosestValueLookupTable, ConnectionSourceClass, MenuViewClass, PacketStatsConnection; and the two pending ones,
  SlotDataShortcutGadget, FontSysCursor), expected false 0.006; applied: +45 anchors.
- Method pilot, 80 anchored methods with their names hidden, 2 requests each: 35 found, 21 of them among the
  first 4 candidates (hence the single-slot rule's `--top 5`). Repeat with `tier2_methods.py pilot`.
- Method trial `mpilot` (the first 240 targets, 2 samples, 25 minutes): 13 single-slot hits, null 1.9; no
  override group. Scored, **not applied**: `run_local.py`'s first method round counts its candidates as tried
  and applies what still hits.

**Beyond this runbook (ask the user before starting; each needs new tool code or a judgment, so a stronger
model):** the review's angles (a)-(f) in section A; `StringValueClass -> KVStringAccessorClass` and `DeltaClass ->
DeltaItem` (cross hits: add them to a proposals file to have them scored); method names for the 1,630
`Unknown_XX` methods in classes not yet confirmed; the 5,700 unnamed slots; brief A's open questions 5-8; a
Q4 / Q6 quantization A/B of the local model (the server session's suggestion; the two pilots are the measure);
single-slot method hits that change the access letter are the likelier false ones (not separated yet).

**Where things stand (2026-11-27, 17:00):** run `n2`'s six class rounds done (11 classes), then steps `n2ko` (the
class test without `Unknown_XX` slots: ShipPositionalStateClass) and `n2t1a` (tier1a on per-index families); on the order 4,834
(confirmed 4,639, probable 195); off named with window 9,441 (`Unknown_XX` 3,460); method rounds `n2m1`- not started
(`n2m1` rebuilds its jobs on resume: 1,874 targets, 98 override groups). The runner's "N classes accepted" count is fixed (it counted one too many). **Before that (2026-11-26, 21:00):** on the order 4,764 (confirmed 4,569, probable 195). Off named slots
9,510 (`Unknown_XX` methods 3,495; 1,833 of them in classes with a confirmed name), unnamed slots 5,700; about 444
class qualifiers unconfirmed. Earlier yields: the dictionary search at chance; Haiku
about 0; Sonnet about 4.7-6.5k tokens per anchor; the two regex fixes gave more than all model rounds together
(re-check tooling before buying more model calls).

**Builder side (Windows, separate items, see progress.md §4, the thunk-order names item):** the confirmed names
reach the code as source renames (class renames from class-renames.tsv's `keep` rows, then per-slot signature
corrections of `confirmed` grade, like N1's 67), each verified name-only (`verify.py` / `pairs_cmp.py`: no pair
should move except int-kind edits), then symbols.json regenerated and `gapguess.py state` re-run on it to confirm
the anchors rise. Non-exact slots (named / vtable source, 42) go through `tools/re/symbols_named.json` with this
evidence. Names into symbols.json only through `tools/re/symbols.py`; client wins over conventions.md (record each
spelling case there: no `Class` suffix, `<X>Interface`, `PIP`, `GPS`); builders never push; nothing reaches
master without the user.
