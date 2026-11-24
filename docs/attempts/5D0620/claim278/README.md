# 0x5D0620, claim 278

Fresh HEAD 8ed1c946; VC6 SP4 app flags and native objmatch. Four source forms
are the baseline and the three patches here. Only the target body was edited;
the final form blanks its NOT EXACT marker on the same physical line. Source
SHA-256 and raw function body SHA-256 (body hashes are diagnostic, not proof):

| Form | Source SHA-256 | Function bytes | Function SHA-256 | Outcome |
|---|---|---:|---|---|
| baseline | d9a2a49fede6eb1d45b9d73f5fd14ed104822358dec93a01c0dd4699069db467 | 736 | 70b9edb1bb8a2fee1aac31dc9de38c563ff70f8d4d8ac7e529893fce673d951b | 60 exact, target unmatched |
| swap | 65d26069adecbd0a5e6dd5b592f7d4df75a5ba1ea3b10ab333bc750cbfec3fb0 | 736 | cf5ccab21a3004aba9cfd51dec08e8946d112d559f53459f5c351baf7acf1c2b | 60 exact, target unmatched |
| nonempty | d90f0c0168896efb585327d438dbbfffeb9f0267f336f5de2ed547682ce780c8 | 720 | c06e7be33dc65be39483fa914f115a2e2b0eaf49689694b3a1ec28bb225f78c5 | 60 exact, target unmatched |
| swap_nonempty | 948e7a24869af790c1f185137c4d75ea03055875d0db6ce0b9d9d7eaae127d54 | 720 | e25758f8c93e3b06285a51c60e037d19a9a9c095369d7483e4d8f639cc5eb8a6 | 61 exact, +0x5D0620, zero lost |
| final control | 4719cfb32a2ee98f90f17b84674bd93266470a10d9a85980272bb682ce0cbb28 | 720 | same as swap_nonempty | 61 exact, +0x5D0620, zero lost |

All four bodies form separate raw-code groups; every candidate compiled, with
no pair loss, invalid input, or rejected candidate. The final source differs
from the last patch only by the marker. All G_ASSERT and FAIL_IF positions are
unchanged. No helper or header changed. The switch case swap preserves each
key's sound and action; the nonempty guard preserves the empty-text early
return and the nonempty path's option lookup, assignment, update and callback
order, including exceptions and callback timing.

`audit.py`, run with the indicated scratch artifacts and native Python, verifies
708 unpadded executable/table bytes with relocation fields masked, then checks
all 48 relocations including addends, direct call targets, literals, option
singleton and all nine jump table labels against their actual retail addresses.
It also compares old exact object bodies and relocation identity (normalizing
the source-file basename) across all 60 prior symbol/address pairs. The target
has no EH registration or object unwind map, hence no handler, FuncInfo or
cleanup funclets; the final virtual call uses the same receiver and vtable
offset as retail. Full run artifacts stay in `.work/claim-278/5D0620/`; no
retail bytes or object files are published. Maintainer verification remains
required before landing.

Adjacent `*.replay.txt` files contain JSON with the exact unified diff in
the `patch` field. Decode that string before applying; whitespace is preserved.
