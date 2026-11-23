0x5DC4A0 UIPowerClass::Unknown_10 — EXACT

Base: 51c7769c; model: gpt-6.1-sol (high). No tracked edits by child.
20 new valid forms: 11 target-only, 9 helper-overlay trials; 0 semantically invalid diagnostics;
0 dead spenders; 22 successful compiles including baseline and exact repeat. One sandbox denial
occurred before compilation. Root's production confirmation is excluded.

Apply best.patch to the base. It changes only the target function, preserves physical line count,
and uses existing helpers without overlays: Unknown_Power_Find for MaxEnergyPower, one-layer
Unknown_Power_Field for MaxShieldPower/MaxHullPoints/HullPoints. Shield max is captured once
before >0, as retail 0x5DC7D5. Existing helpers at ui_power.cpp:30-31 derive from ui_mfd.cpp:885
and ui_analyze.cpp:70-71; all state predicates remain Unknown70 != 0. No alias casts or new types.

Fresh full-unit native comparison: 47 -> 50 exact symbol/address pairs; +0x5DC4A0,
+0x546710 (new float Find), +0x5BF2B0 (unchanged interpolator Find now unambiguous), zero loss.
Independent repeat agrees. Existing outside-target emitted functions and relocations are unchanged.
The new float helper and unchanged interpolator helper have independent masked-byte/call/data audits.

Target [0x5DC4A0,0x5DC998): 1272 bytes, 373 reachable instructions, 62 branches, 39 calls,
47 x87 instructions, sole ret 0x5DC997, no EH frame. Raw normalized/masked/register residues all 0.
All call targets, branch targets/widths, FP constant bits/widths, rounding stores and conversion order
match retail. Audit covers NaN/unordered and signed-zero branches, field reads and callback timing.
Header __FILE__ literal prefixes differ in scratch compilation; basenames delta_interpolator.h and
auxdata.h match. All other literal bytes match. No executable link or runtime test by child.

forms.json records every new source SHA, exact-pair delta and body metrics; forms/*.patch replay
all 20 forms from the git base. Run `python replay-script.txt /tmp/power-replay` to reconstruct and SHA-check
them. audit.json carries independent full-boundary/call/FP/helper evidence; audit-script.txt is the local
reproducible audit script (paths reference the original immutable scratch tree/runs).
