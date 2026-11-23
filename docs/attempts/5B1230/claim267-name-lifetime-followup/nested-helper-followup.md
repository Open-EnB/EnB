# Focused nested-helper handoff

Against production `84ea059a`, there is **one modest caller-only hypothesis left**, with at most two forms. It uses the experimentally demonstrated Name lifetime while keeping the original `Unknown_5B3640(false)` context. The exact recipes and stop criteria are in `nested-helper-followup.json`.

The positive baseline has 145 pairs, a `3C` frame, the recovered components/validity Cast calls, and the desired later `6CA0`/`3580` calls. Its one substantive inline boundary is wrong: it calls `Unknown_Find_Valid_List` whole. Expanding 3640 at the caller removes one real source layer. Keeping `Unknown_Get_List` within that expansion recovers inline Name construction plus `5B6DB0`, but also causes the later helpers to inline. That is evidence for a depth/context tradeoff, not evidence that another inert wrapper would represent original source.

The new experiment separates the two factors which the latest test combined:

1. Start from the **current positive source**, with the actual `Unknown_5B3640(false)` call intact. Make only the `Validity` Name an explicit `NameClass("Validity")` argument temporary to the existing `NameClass const&` overload.
2. If not exact, independently use a small scope around the existing named Validity object and lookup, assigning the resulting pointer to an outer `validity` local. This retains the original constructor-before-root-read order while ending the Name lifetime before success processing.

The first standalone argument-temporary test was on the 144-pair seed. The subsequent 145-pair lifetime test removed the outer 3640 source context. Thus the proposed combination has not been measured. The older scoped-name forms also altered a shared helper or expanded the call; they are not this isolated scope-only source. This is not a claim that lifetime alone governs inline selection. It tests whether the now-correct components/time source context can satisfy both the established lifetime requirement and the original outer call boundaries.

The result must have all of the following: inline PreviousAttempts Name construction; a call to exact `5B6DB0`; 3640 itself still inlined; nested `6CA0` and `3580` still called; `3C` frame; unchanged components/validity Cast targets; all 145 pairs retained. Reduced rows with the whole lookup still out of line are not success. The argument form must use explicit `NameClass(...)`, since a bare string selects a different existing overload and adds source layers. The generated root read must still occur after Name construction.

I checked the current helper definitions/declarations and the supplied exact-relative comparisons. `3850` deliberately has different PreviousAttempts/KnownFormula Cast expansion, and its nested copy in exact `1ED0` supplies another constraint. Removing `Unknown_Previous_Attempts` globally therefore is not a local repair. `3580`'s exact source directly constructs a Name and uses SetList, but substituting that source inside 3640 requires changing the out-of-claim callee and reproduces a historically explored family. It is a coordination question, not a new caller-only recipe.

No further bool-argument or receiver permutation is supported. Passing `!Unknown0BAD` for the known false argument would be a gratuitous inliner perturbation. A private clone/wrapper of 3640 added solely to recreate lost depth is similarly unsupported. Reusing the existing Name through assignment is contradicted by the repeated retail constructor call. I found no signature or header error that would justify a new declaration change here.

If these two lifetime forms fail the complete call-map criterion, **stop this caller-only branch**. Further work needs actual evidence for a coordinated reconstruction of 3640 or its helper topology, not more source permutations in 1230.

Read-only analysis only; no compilation, Wine, tracked edit, subagent or GitHub action. Approximate focused usage: 5–7k tokens, uninstrumented.
