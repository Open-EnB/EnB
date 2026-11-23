# Claim 267: whole-claim synthesis for bounded Sol testing

Analysis-only handoff, gpt-6-astra HIGH, against `9b07cd77`. No compilation, Wine, tracked edit, ledger write, commit, GitHub action or subagent was used. `git status --short` remained empty. The machine-readable recipes, seeds, dependencies, evidence, semantic restrictions and stop conditions are in `hypotheses.json` (32 KB).

The strongest new lead is the manufacturing timer's missing **DurationClass/SecondsClass source layers**, followed by inventory's **input-parameter role reuse** and **real KernelEvent accessor**. These explain specific source semantics and instruction boundaries. They are not another broad local-variable sweep. Start with source-only tests A267-01 through 04. A267-05 through 07 are smaller, lower-confidence tests. A267-08/09 are optional, and the declaration/representation proposals A267-10 through 12 are separate from that queue. Do not execute the conditional retypes automatically.

## What was actually checked

I read `attempts.py show` for all thirteen targets, their current source and relevant headers, the 156-form claim artifacts (including nested `artifacts` and `source_recipes` payloads), and the older inventory stashes. Historical attempts without surviving source remain history-level dedup evidence; I do not claim those missing patches were inspected. All available non-claim stashes for these addresses are the two `docs/attempts/0x5A0740` files. I also regenerated read-only aligned comparisons from saved objects for jobs, log, login, manufacturing ctor/dtor/update. Those compact `*-regions.txt`, `*-retail.txt`, and `*-ours.txt` files are in this directory; `evidence.py` only reads existing objects/retail and writes this scratch directory.

Current baseline remains +2/-0, with exact `59B380` and `5B0BB0` protected. A lower diff row count does not authorize a wrong call, frame or ownership path. Old manufacturing seeds have 140 pairs; current HEAD has 141. Always preserve the new BB0 source when integrating another experiment.

## Individual target assessment

| Target | What the residual really says | New lead / decision |
|---|---|---|
| `59B220` | Inline compound Cast, not a generic register issue. Retail calls `598D80`; the outer lookup and assertion sites otherwise fit. | A267-05: consumed unsigned `Unknown70` reference **inside** the existing filter helper. One form, no further wrapper count sweep. |
| `59B380` | Exact packet temporary, read scheduling, Checked getter and EH graph. | Preserve unchanged. Its useful argument assignment and accessor layer are precedents, not licenses for dead reads. |
| `59BC40` | Same Cast depth problem, plus the aux root reload after NameClass construction. | The other A267-05 form, independently. No template-global or specialization experiment. |
| `59FE30` | Best seed still has the wrong event/slot live-range allocation and frame `18` rather than `14`. | A267-04: real `KernelEventClass::Is_Up()` versus real direct Flags control, on the useful packet-input seed. Existing bitfield trial is not the same accessor layer. |
| `5A0740` | Best seed matches **every fixed byte through offset `41D`**. Tail calls, slots and branch destinations are right; scratch allocation differs. | No new local query/mode/result rewrite. A267-10 is only a low-priority actual declaration-context check, not a promised fix. |
| `5A0D00` | Retail preserves input slot in EBX, spills peek, then overwrites the input argument's stack home with destination slot. | A267-03: explicitly copy source slot then reuse the slot parameter for the destination. Prior copies did not end/reuse the parameter's role. |
| `5A3FB0` | Wrong delete lowering and nested erase boundary cause the register/frame consequences. Retail has direct destructor + global delete, and postfix increment out of line. | A267-06: `::delete job` in the existing helper. No flattening or explicit destructor/deallocator sequence. |
| `5AB030` | Three local scheduling islands: hidden return pointer, OR flags/entry input, tooltip pointer reload. | A267-07: sequence the two useful flag reads **at the new-expression argument**. Optional A267-09 probes lifetime extension of the returned entry smart pointer. |
| `5ACB80` | Whole 1599-byte/456-instruction body and branch graph fit; two isolated scheduling differences remain. Thunk `401D3E -> 5AE9D0` is valid. | **No supported new lead.** Do not fabricate a flags subobject, alter the Add signature or repeat model/manager references. |
| `5AFA00` | Not merely a swapped adjacent pair: ours hoists LEA B80 and the BB8 low store above the entire B90..BB4 initializer run. | A267-02: real DurationClass zero value in the existing scalar initializer. A267-11 is the conditional member-type proposal, with broad checks. |
| `5AFCD0` | Loop is instruction-for-instruction right except EBP/EDI roles; destructor/refcount/null-store order is supported. | Optional A267-08: use the existing same-type smart-pointer reset overload. Do not use Clear(), which fails to null. |
| `5B0BB0` | Exact 999-byte/291-instruction body, packet, sort-value and 256-byte-buffer model; 6 EH states. | Preserve unchanged. This win shows that apparent budget trouble can be a missing useful value lifetime and frame object, not an arbitrary spender. |
| `5B1230` | Baseline `3C` frame is right but several nested call boundaries are wrong. `70`-frame reductions are false progress. | A267-01: typed tick value plus true seconds-to-milliseconds conversion. It is the only new substantial source-layer lead. |

## Why the timer hypothesis is materially different

The ctor stores `+BB8` low at `5AFB30`, computes `+B80` at `5AFB36`, then stores the high half at `+BBC`. In our saved object both the LEA and low store are hoisted before `+B90`. Existing tests changed primitive spellings, body assignments, memsets and array forms; none introduced the project's real inline time constructor. `DurationClass` is an eight-byte value with inline construction, and the workflow documents a real inline member constructor as an initializer scheduling boundary.

The same member is read as a signed 64-bit timestamp in `5B1230`; the current tick is explicitly zero-extended from unsigned 32 bits. A dynamically initialized global contains 64-bit **5**, and the update multiplies it by **1000** before adding the tick. `SecondsClass` to `MillisecondsClass` performs exactly that operation. The current file uses an explicitly interim `Unknown_Filter_Seconds()` helper to manufacture dynamic initialization for a scalar. These facts jointly motivate a time abstraction; the ctor alone would not prove one.

There is also an exact cross-function precedent: `UIMFDClass::Set_SP_Percent` at `5BE120` uses `DurationClass time; time = DurationClass(...)`. Its recorded exact row says the real assignment layer after a lookup sent the nested Cast out of line; constructing the same value directly did not. `docs/data/imports/app/ui_mfd.json` still holds `5BE120`. This is much stronger than adding another generic wrapper around a lookup.

The initial tests keep both manufacturing member and global scalar types. A267-01's three forms compare assigned versus directly constructed `now`, then a genuine Seconds/Milliseconds conversion at the existing delay read. A267-02 puts `DurationClass(0).Time` in the existing ctor initializer. Only if this supplies useful evidence should root consider the separate member/global representation proposals. Retail's bytes do **not** uniquely identify the original C++ type, and broad header changes are not justified merely because a name sounds right.

Required call map for an improving `5B1230`: `5B7210` and `5468D0` Casts out of line; `3640` itself remains inlined; inside it, `5B6DB0`, `5B6CA0`, and `5B3580` stay called. The late top-level `6CA0` expansion is expected inline. Preserve `3C` frame, single reusable NameClass storage, original FP prefix and all existing pairs. Judge these before residual counts.

## Source lifetimes and declarations across inventory

For `5A0D00`, retail loads source slot at `5A0D44` into EBX, spills the first peek at `5A0D4B`, overwrites the original slot home at `5A0DD9`, and later pushes preserved EBX as the source. The proposed recipe is not another source-slot alias: it deliberately reuses the parameter for the destination after preserving the original. That gives the compiler the same two variable roles that the retail stack demonstrates. No extra read or callback is introduced.

For `59FE30`, the real `KernelEventClass` already exposes `Flags` at +2 and `Is_Up()` as bit 12. The source instead casts to a local layout-only struct. The bool is named `down` in this reconstruction, but the proposed accessor must **not** be inverted: it returns the same original bit value. A direct real Flags member is a diagnostic control separating declaration context from the accessor layer. It may still canonicalize; stop after those two forms.

There is an actual declaration mismatch in inventory: the local drag class says slot +0C returns void, while `drag_slot_data.h` and its implementation say `bool Cancel()`. The real class also derives its +10 State from `SMouseDragClass`. This justifies testing the existing real header rather than tuning declarations. However callers discard Cancel's result, so it does not itself explain the 0740 AL inversion. The previous busy-call audit remains valid. A267-10 uses the existing header unchanged, preserves all virtual slots, and is expressly a low-confidence declaration-context diagnostic.

The gadget query returns and busy helper are not retuned. No unknown return type is changed to force ECX/EDX; no shadow pointer reference is used to defeat alias rules. A fully matching 0740 prefix and exact sibling busy users are substantial negative evidence against a broad type, frame or control-flow rewrite.

## Secondary tests and honest stop points

A267-05 borrows the **specific** 5A02E0 helper-count result: a reference to an actually consumed scalar changed its caller's inline boundary while preserving the helper. The prior group reference tests bound the root pointer instead. These two scalar-field tests are still less convincing than the timer model; only a recovered `598D80` call makes them worth following.

A267-06 changes standard deletion lookup, not the destructor implementation. `UIJobsInfoStruct` has no virtual destructor or class-specific delete, so `::delete job` is equivalent while potentially choosing the direct lowering seen in retail. Manually calling the destructor and `operator delete` is not offered as a retained solution because exception deallocation semantics are different unless separately established.

A267-07 keeps both flag reads after allocation by putting the sequential accumulator in the actual constructor argument. This is distinct from a pre-new local. Historical prose mentions a flag local without surviving source, so overlap is honestly marked uncertain. A267-08/09 are one-form optional lifetime/overload probes. They have no authority to introduce different refcounts or EH cleanup. If they canonicalize, stop.

Login is deliberately left without a fabricated experiment. All tested arrays, real model and manager inputs, reference lifetimes, getters and direct initialization forms converge. The retail member layout and actual Add call signature agree. The four-byte LEA and two initializer scheduling instructions alone do not establish a missing class or accessor. That is a reason to park, not to change a header until the registers move.

## Handoff limits

The JSON contains 12 ordered hypotheses and 16 bounded forms, including optional and conditional proposals. Only 11 forms belong to the primary source-only queue A267-01..07. The optional/conditional tail should not be treated as a quota. Do not combine two canonical failures. Preserve the claimed exact pairs and full object baselines, check actual call targets and EH state, then record the source and result once. A267-11 requires all includers, including the direct `ui_manu.cpp`, `ui_manu_util.cpp`, `ui_ptrade.cpp`, `sbaseview.cpp` consumers and any indirect dependents. Root decides whether its evidence warrants that work.

Approximate analysis usage: 30–35k tokens including tool input/output and reasoning; not an instrumented count. All deliverables are scratch-only, and no build VM was used or leased.
