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

## A. Review brief (Opus, read-only; no edits to the state or docs/data)

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

## B. Continuation brief (after the review's findings are applied)

**Start:** `git switch names/2026-11-26` (or master once merged), then
`python tools/re/thunknames/gapguess.py state` must print `on 4720` (or the post-review figure in the log entry) and
`python tools/re/thunknames/mangle.py` its self-test. `.work/names/` is scratch: a fresh clone has none and needs none
(run `strrefs.py` once before tier1b or the contexts).
Setup: `ENB_CLIENT_EXE` = retail client.exe (SHA-256 249f8524...51c4; default: clientimage's install path),
`ENB_EA` = the EA tree (default `C:\EnBDevelopment\EA`); Python 3 with numpy. `.work/pseudo/` only for
`tier2_ctx*.py` (Ghidra output; `pseudocode.py` rebuilds it on Windows). Outputs go to `.work/names/`.

**The pipeline after any new confirmed name:** `python tools/re/thunknames/loop.py` (propagate -> tier1c variants ->
gapguess, until a pass adds nothing), then `export.py` to refresh docs/data. Model proposals are scored with
`tier2_eval.py --props <dir> --null-sample 300 --const`; accept at chance < 1e-4 (or the review's threshold).

**Where things stand:** 1,074 class qualifiers: 623 confirmed, 451 not. Off named slots 9,554 (Unknown_XX methods
3,514), unnamed slots 5,700. Yield so far: dictionary search at chance (dead end); Haiku ~0; Sonnet ~4.7-6.5k
tokens per anchor; the two regex fixes gave more than all model rounds together (re-check tooling before buying
more model calls).

**Next avenues, in expected value order:**
1. **The review's fixes** (re-score, drop weak or split-propagated names, re-run the loop), then the pending
   SlotDataShortcutGadget hit.
2. **Unknown_XX methods of confirmed classes** (3,514 slots): class and signature are known from the code (vtable
   index, byte-exact signature), only the method name is open. Sources: the same vtable index in a confirmed
   relative (tier1a, now with far more anchors), `Class::Method` strings (tier1b), then a model asked for method
   names with the class's confirmed methods as context. Test each name with access / const forms; per-slot chance
   is ~0.1 % x forms, so require class consistency or a string.
3. **The 451 unconfirmed classes:** 66 had two rounds without a hit; the rest have no destructor slot or few named
   methods. Use what failed (the destructor test says the name is none of the candidates) and the confirmed
   family style; consider Opus for the 66 only if the user approves.
4. **Unnamed slots (5,700):** class, method and signature all open; find the class from the function's vtable or
   its callers first, then they become item 2.
5. **A tried-names ledger** (`.work/names/tried.tsv`: slot, name, window at the time) so later runs skip repeats.

**Builder side (Windows, separate items, see progress.md §4, the thunk-order names item):** the confirmed names reach the code as
source renames (class renames, then per-slot signature corrections, like N1's 67), each verified name-only
(`verify.py` / `pairs_cmp.py`: no pair should move except int-kind edits), then symbols.json regenerated and
`gapguess.py state` re-run on it to confirm the anchors rise. Non-exact slots (named / vtable source, 42) go
through `tools/re/symbols_named.json` with this evidence.

**Rules that still apply:** names into symbols.json only through `tools/re/symbols.py`; generated files only by
their tools; client wins over conventions.md (record each spelling case there: no `Class` suffix, `<X>Interface`,
`PIP`, `GPS`); builders never push; nothing reaches master without the user.
