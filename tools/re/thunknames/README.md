# Thunk-order name recovery

Recovers retail's exact decorated names (class, method, signature) from the order of client.exe's incremental
thunk table. The rule: [docs/binary.md](../../../docs/binary.md) §5.1-5.2; this work and its results: §5.6.
Status, review and next steps: [HANDOFF.md](HANDOFF.md) (delete it when its two sessions are done).

## The idea in four lines
VC6 LINK /INCREMENTAL writes the 19,983 thunks in its symbol hash table's bucket order. A named slot on the longest
non-decreasing bucket run is an **anchor** (retail's exact name). Every other slot's real name hashes into the
**window** between its neighbouring anchors, and almost always into its **near part** (+/- 3 buckets of the bucket
interpolated by slot position). So any proposed name costs one hash; a wrong one lands in the near part with
about 7 / 16,384 (`gapguess.prob`: the buckets [6856, 8192) are unsplit, hold double and count twice). Every
acceptance therefore needs a chance figure and a null (the same test with names known to be wrong).

**Anchor grades (the review, 2026-11-26).** Only **confirmed** anchors bound windows. An anchor is **probable**
(kept as the slot's name, never a bound, not evidence for a source rename by itself) when it is a single-slot hit
of a kind with a high null (access, const, int, pointer cv / reference, callconv, underscore), a rename whose class
has one anchor, a rename `rescore.py` judged weak, or a name never tested (IM3's, a propagated one) that lies
outside its near part. Names with `Unknown` are never anchors. The rules: the comment above `WEAK` in gapguess.py.

## Data
| File | What |
|---|---|
| `docs/data/thunk-names.tsv` | **The state**: every named slot (slot, target, name, original = IM3's name where it differs, bucket, on / off, how it was found, grade = confirmed / probable: why). Every tool's default input. |
| `docs/data/thunk-name-hits.tsv` | Every accepted hit, in order: step, slot, name, before, tier, chance, window, near part. |
| `docs/data/class-renames.tsv` | Our class -> retail class: slots, anchors, destructor anchor, best per-slot chance, first step, confidence A / B / C, split suspect, and rescore's `near_alone`, `tries`, `chance_alone`, `verdict` (keep / weak). Builders draw on the `keep` rows. |
| `proposals/{pilot,scale,round3}/` | The model proposals (12,546 class candidates; ~2.3M Sonnet / Haiku tokens to make: reuse, never regenerate), their instructions and class lists. |

All three docs/data files are written by `export.py` / `loop.py --write`; never edit them by hand.
Scratch goes to `.work/names/` (`paths.py`).

## Setup
Python 3 with numpy (and the repo's `tools/re` modules). `ENB_CLIENT_EXE` = retail client.exe (SHA-256
249f8524...51c4; default: clientimage's install path), `ENB_EA` = the EA tree (default `C:\EnBDevelopment\EA`; only
the dictionary tools read it). `.work/pseudo/` only for `tier2_ctx.py`. Run from anywhere; paths are repo-relative.
Check: `python tools/re/thunknames/gapguess.py state` prints the anchor count (4,720 at the 2026-11-26 handoff) and
`python tools/re/thunknames/mangle.py` its self-test (`permnull.py` has one too). 4,764 on, 4,569 confirmed after the
review and the first local round.

## Tools, in pipeline order
| Tool | Does |
|---|---|
| `mangle.py` | The one rule for finding / renaming a class inside a decorated name (after `??0` / `??_G`, after a type code `V` / `U`, never the method name). Two bugs of 2026-11-26 came from copies of a weaker pattern: use these functions, never a new regex. |
| `gapguess.py` | The state (full thunk table, unnamed slots included), anchors and their grades, windows, near parts, the density model (`prob`, `near_part`: use them, never `width / 15048`); `test GUESSES.tsv` checks whole decorated names and cascades (each hit becomes an anchor and narrows its neighbours; conflicting hits are retried). Its per-hit `chance` counts only the guesses in the file: a tool that picked its guess among variants is judged by its kind's null (the grades). |
| `vt.py`, `strrefs.py` | Vtables from client.exe at symbols.json's `??_7` addresses; string references per function without a disassembler (`.work/names/strrefs.tsv`, input to tier1b and the contexts). |
| `tier1a.py` | Vtable relatives: an anchored method at the same vtable index in a relative gives an override's method and signature. |
| `tier1b.py` | `Class::Method` strings a function references (on anchors: right 104 of 110). |
| `tier1c.py`, `t1c_keep.py` | Convention variants of our names (access, const, int kind, pointer cv / reference, `??_G` / `??_E`, class +/- Class / Struct, a confirmed class rename + one edit), each kind against a null (window shifted); keeps the kinds at >= 5x their null. |
| `classsearch2.py` | Class-name search without a model: variants of our own name (abbreviations, prefixes, suffixes) and a 1.86M-word dictionary (18 processes, ~70 s; at chance so far). |
| `tier2_ctx.py`, `tier2_ctx3.py` | Compact per-class contexts for a model (round 3's: destructor slots marked, names already tried, confirmed relatives). |
| `tier2_eval.py` | Scores model proposals (every file under `proposals/`): each class's open slots renamed, every access / const / `??_E` form, chance = C(n, k) x prod(forms x prob(near)) x candidates tried. `--perm 1000` runs the salted null and sets the threshold (`--fwer`, expected false classes per run). The cross-class pass (300 other classes' candidates per class, `--seed`) is a search: its `CROSS HIT` lines are candidates to look at. METHOD lines are ignored unless `--methods` (the cloud rounds' rule, no threshold): `tier2_methods.py` scores method names. |
| `permnull.py` | The salted null: a candidate + two salt characters, K salts at once (numpy). A salt can spell a true name (`HTMLView` + `er`): look at any salted draw far under the rest. |
| `rescore.py` | Every accepted rename with its own slots left out: still near without its siblings? chance with the candidates tried (round 1: 291k tokens, the dictionary 1.86M); keep / weak. `export.py` runs it. |
| `tier2_local.py` | Class-name candidates from a local model (OpenAI-compatible server, one class per request, 16 in parallel, thinking off); `--pilot` measures recall on classes already confirmed. |
| `tier2_methods.py` | Method names for `Unknown_XX` methods of confirmed classes from the local model (class, its confirmed methods, signature, strings, pseudocode): `jobs`, `propose`, `score` (override groups: confirmed; single slots: probable, with the salted null's expected false count), `pilot`. |
| `round_local.py` | One class round as two commands: `propose` (classes, contexts, candidates, score) and `apply` (cascade, loop, export). |
| `run_local.py` | **The unattended run**: checks, settle, class rounds, method rounds, each with its stop rule; report in `.work/names/run/<prefix>/report.md`. HANDOFF.md brief B is its runbook. |
| `propagate.py` | Writes every confirmed class rename into every off name (qualifiers and parameter types); skips possible class splits (two retail targets, or an anchor still spelling our class). |
| `loop.py` | propagate -> tier1c -> t1c_keep -> gapguess until a pass adds nothing; appends the hits; `--write` updates the docs/data files. |
| `export.py` | Writes the state and class-renames.tsv (and once, the hits file from a session's per-step hits). |
| `classsearch.py`, `rounds.py`, `null.py` | Round 1 (2026-11-26 morning): the dictionary and the iterated token search with its random-name null. Kept for the dictionary and for reproducing round 1 from IM3's `namecheck.tsv`. |

## A typical round
1. Propose: write a GUESSES.tsv (slot, name, tier, note), or model proposals as `CLASS<TAB>ours<TAB>candidate`
   under `proposals/<round>/` (contexts from `tier2_ctx3.py`, instructions as in `proposals/round3/`).
2. Score: `tier2_eval.py --null-sample 300 --const --perm 1000` (proposals) or `gapguess.py test GUESSES.tsv` (names).
3. Accept the class hits under the salted null's threshold (1e-4: 0.008 expected false classes per run), apply with
   `gapguess.py test <guesses> --out DIR`, then `loop.py --state DIR/state.tsv --tag T --write`.
4. Record the step in the progress.md log entry.

## How the names reach the code
Not here: builders rename the source declarations (class renames by file, then per-slot signature corrections,
like N1's 67), verify name-only on a pool VM (no pair should move except int-kind edits), regenerate symbols.json
with `tools/re/symbols.py`, and re-run `gapguess.py state` on the new names (progress.md §4). Names of slots that are
not `exact` go through `tools/re/symbols_named.json` with this evidence. Client wins: record each spelling case in
docs/conventions.md, and tag each name per D56 (the thunk order is the client's own evidence: NAME 5, the
review may grade weak acceptances lower).
