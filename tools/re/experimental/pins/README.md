# Evidence-backed pins (experimental, not in the workflow)

Saved 2026-10-27 from builder M4's measurement (branch `wt/pins`, commit 8db8c42). The user's decision
D118: keep the tool, don't adopt it; the matching stays objmatch's rounds in D80's order.

## Background

objmatch credits a compiled function only when its bytes point at one client address. Template and inline
copies are often byte-identical at several addresses, so they stay tied and count nowhere (D54). Rules D80,
D88, D100, D114 and D115 break some ties from evidence (relocations, vtable names, the object's range, the
static initialiser). Open-BFME (github.com/Open-BFME/Open-BFME-1) takes the other approach: a ledger row
claims one address per function and the tool verifies the bytes there ("a wrong pin still matches").

M4 measured four evidence kinds as an extra objmatch round after the initialiser round, on the tree at
179426c (11,366 pairs, 2,906 tied entries / 2,294 symbols):

| Kind | Pairs | Lost | Audit (20 sampled) | Credits on byte-identical twins |
|---|---|---|---|---|
| (a) an exact function of the same object calls exactly one candidate | +655 (full verify) | 0 | 0 doubtful | 87 % |
| (a') a caller anywhere in the library | +812 | 0 | 4 doubtful (far template COMDATs) | 88 % |
| (b) placement next to the object's range (N = 0x1000 / 0x4000) | +321 / +336 | 0 | 2 of 85 wrong where (a) could check | 82-84 % |
| (c) hand-written pins (`pins.csv`) | +8 from 6 pins | 0 | heuristic | 4 of 6 |

Findings worth keeping: (a) is a link proof (the client's own call instruction), the strongest kind; (b)
is measurably wrong at about 2.4 % and should never be automatic; most credits of every kind land on a
candidate whose bytes equal another's, so the pair is right by bytes but the identity can be a twin;
symbols.json names a vtable slot `??_E` where our builds produce the equivalent `??_G` (an alias would credit
14 functions from vtable proof alone). That alias is now objmatch's rule (bada317 / 32fd4b3, D129, +293
pairs): the table above predates it.

## Files

- `objmatch_pins.py`: objmatch.py at 179426c plus the pin rounds (`--kinds caller,callerlib,place1000,place4000,pin`).
- `pins_run.py`: runs the kinds offline over a verify's objects (M4's `.work/m4/` layout; adjust the paths).
- `audit.py`: samples credits and checks them with callers and placement.
- `kinds_pin.json`: the per-kind credit lists of the (c) run.
- `pins.csv`: the draft ledger format (symbol, address, file, evidence, note), six pins and one negative row.

The scripts are a snapshot: objmatch.py has moved on since 179426c, so rerunning them needs the matching
objmatch.py from that commit and a fresh verify's objects.
