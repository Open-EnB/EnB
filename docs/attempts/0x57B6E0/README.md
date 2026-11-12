# 0x57B6E0: the combo box's animations, RefCountPtr<HAnimClass> or raw (proposal for the user, D119)

Builder L10, run 2026-11-11. Two forms of `UIComboBoxClass::WidthAnim` / `DropAnim` (+0xA4 / +0xA8) were verified
against the run's baseline (`imports.0185dca9.json`); each loses one address outright, so neither lands without the
user (D66).

## The tree's form (55ae80ba): `RefCountPtr<HAnimClass>` members

The client's own form: Init 0x5790C0 assigns `Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(...))`, the
destructor 0x578D50 clears both with `= NULL` and then destroys them as members. 25 functions of 0x578120-0x57BAC0
are exact, among them the destructor and `Create_Get<HAnimClass>` 0x57B6B0 (app: 14389 functions in tree).

Lost outright: **0x57B6E0** (`~RefCountPtr<RenderObjClass>`). The object then also holds `~RefCountPtr<HAnimClass>`
out of line (the constructors', the destructor's and Init's unwinding), byte for byte the same, and the matcher
cannot credit 0x57B6E0 to one of the two: it moves to the ambiguous column (group 0x46BE90). In the client the
`HAnimClass` instance is another object's COMDAT copy. explain.py: identical.

## `ui_combobox_rawptr_L10.cpp` / `.h`: raw `HAnimClass *` members

Explicit Release_Ref (`Set_Anim`, `REF_PTR_RELEASE`), no `RefCountPtr<HAnimClass>` in the object. 0x57B6E0 stays a
pair. Verified 2026-11-11 16:10: 17 functions of the range exact (app: 14381). Not exact any more: the destructor
0x578D50 (its members' destructors and EH states are gone) and 0x57B6B0 (not compiled); and with the destructor
goes the only exact function that names the class's tables, so the short slots 0x579D80 / 0x579DA0, the adjustor
thunks 0x57BAB0 / 0x57BAC0, 0x57B750 and 0x57BA40 lose their credit too.

Lost outright: **0x57BA80**, the object's static initialiser (the guard-0xC416FC header static, a pair in the
baseline). Why (objmatch.py's range round, D209): the object's own range now starts at 0x578120, so the window in
which its initialiser is looked for (the range widened by 0x1000) reaches ui_class.cpp's byte-identical initialiser
at 0x5780A0: two hits, none located. In the baseline the range started at 0x57A330 and 0x57BA80 was the only hit;
in the RefCountPtr form the thunks at 0x57BAB0 / 0x57BAC0 put 0x57BA80 inside the range itself.
A destructor without `pop_back` (no `std::copy` in the object) changes nothing (appfile.py match).

## Proposal

Accept the RefCountPtr form under D66 (vi) / D119: it is the client's form, gains 25 functions in the range and
8 more than the raw form, and its one lost address is a byte-identical template destructor that the object does
reproduce. The raw form trades it for the loss of the object's initialiser and eight functions.
A matcher lead instead of a source one: P-CGP-1 could let an exact function's unwinding funclets (the destructor's
jumps to `~RefCountPtr<HAnimClass>`, which land on another object's copy in the client) decide the tie.
