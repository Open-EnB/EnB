"""Finds the functions of compiled .obj files in client.exe, byte for byte.

Each function in an object is turned into a byte pattern in which every relocated field (calls,
jumps to other functions, addresses of data and strings) is a wildcard, because those are only
fixed at link time. The pattern is searched for in client.exe's .text. Results per function:

  exact      found once: this is the function, unchanged
  multiple   found more than once (short or common code; not proof on its own). `count` gives
             the number of copies (`addresses` lists the first 5), `identical` whether all
             copies are the same function, relocated fields included (REL32 targets compared)
  none       not found: the client's function differs, is inlined, or was not linked

A `multiple` result is broken by the relocations' targets (D80, `break_tie`): each candidate is
checked against what the object's relocations point at, the string literals (their text against
the client's string at the candidate's relocated field) and the symbols symbols.json knows (their
address against the field's target). A candidate with a contradiction drops out; if one is left
and at least one target confirmed it, the function is `exact` there (`tiebreak` is set).

What D80 leaves `multiple` gets a second tie-break (D88, the "self name"): of the candidates no
target contradicts, the one address symbols.json names with the function's own mangled symbol
from a proof other than an exact match (a vtable slot, a string; an exact entry counts by the
proof it keeps, `proof`, D114(c)) is credited, `exact` with
`tiebreak` and `selfname` set. It runs only once D80 resolves nothing more, so D80 always decides
first, and its credits feed D80's next round like any exact function.

What neither resolves gets the object's client range (D100, P-P6-1): of the survivors, the one
address inside the object's own range (first..last address of its exact functions that are its
alone: not COMDAT copies of inline, template or compiler-generated functions, which the linker
may place with another object) is credited, `exact` with `tiebreak` and `range` set. The range
also covers the object's exact COMDATs that no other object is exact at, within 0x1000 bytes of
either end of that own range (D114(b), P-M1-2; D196, 2026-11-05: at the upper end the widening is
chained through the COMDATs only this object is exact at, each measured from the range the previous
ones widened, so a run of template copies longer than 0x1000 after a file's last own function reaches
the file's static initialisers, ui_ship_cust's 0x620CA0; the lower end and D183's shared COMDATs are
measured from the own range only: chaining them let ui_shipcust's range run down over ui_ship_cust's
copies of a map type the two files shared), and an exact COMDAT that two or more objects are
exact at, at one address under one symbol (D183, P-IS-1), for the holder whose own range is
strictly nearest to it (inside it counts as 0; a tie widens none). Measured 2026-11-01: crediting
it to every holder lets a later object's range reach over the first object's code (inpkeys' range
over inpdsp's basic_string copies at 0x42A820) and two objects' tied functions then propose one
address (inpdsp's 0x42A900, 0x42AAE0 lost). It runs only when D80 and the self name resolve
nothing more; its credits feed D80's next round. A survivor the object's own exact functions
contradict is not inside (P-CGP-1): when their relocated calls to the symbol reach other addresses
only, the symbol is there, not at the survivor (two same-bytes COMDATs of one object would otherwise
both propose one address and neither be credited). In the range round (not D115's) the range is also
widened over the spots where `initialiser_targets` locates the object's own `_$E` static initialisers and
their atexit functions (D209, P-IR3-1 as narrowed by NM1, V7d; D80 amended 2026-11-08), so a tie lying
among them is inside, with three guards, because a header static's initialiser has hundreds of
byte-identical copies (533 of the guard-0xC416FC one) and the one found in a window is often the
neighbour's: (a) a spot two objects locate widens neither; (b) no widening that brings a second copy of
one of the object's still-tied `_$E` functions into the range; (c) in the app region only, a spot below
the range must start a zero-separated run of the client's CRT initialiser table (.CRT$XCU, merged into
.data: the longest run of zero or .text dwords, `xcu_previous`), which lists each object's initialisers
in link order. USE_XCU switches (c). When several symbols propose one address in the range round,
the one whose own evidence there is strictly the strongest is credited and the others stay tied
(P-CW1-1, 2026-11-08; `range_evidence`, compared as a tuple). A proposer is refuted when one of its
calls goes to a function defined in its own object whose bytes do not fullmatch where that call lands
(a template callback's ??_G calling its own inline destructor, against the class's ??_G, ui_chat_panel's
0x562410); if one proposer is left, it is credited. Of several left, the strongest: first whether the
client names the address with the symbol itself (D88 / D171's self names, or an exact function's call
or reference to the symbol reaches it), then how many of its calls go to functions of the object's own
code (not COMDATs) and land inside the object's range there; a tie at the top, or no evidence, still
drops them all. USE_RANGE_EVIDENCE switches it.

Such a COMDAT is `shared` (D183's guard): set on each holder's exact result; importer.py lists it in
the file's `shared` list. A shared result is not placement evidence: a D166 caller tie-break that
needed one (as our caller, or as the function a client call site sits in) is labelled
`shared_callers` (importer.py's `shared_callers` list). Skipping them instead lost five tie-break
pairs (0x4668F0, 0x5199F0 x2, 0x62A5D0 x2), so they are labelled, not skipped.

The self name also comes from the run itself (D171, P-AR1-1): a vtable whose address the run has
learned (an exact function's relocation to `??_7X@@6B@`, e.g. a constructor's vptr store) names its
slots' functions, slot k of our table being the function at the client's table + 4k (through its
thunk); and a short function (too few fixed bytes to search) at an address the run knows (a call's
target) whose pattern holds there teaches its relocations' targets the same way (`??1` of a template
accessor: `mov [ecx], ??_7KVAccessorClass<T>; ret`, whose table then names `??_G`).

What the range leaves gets the static initialiser (D115, P-R2-1 narrowed): a tied constructor or
destructor (`??0` / `??1`) is credited to the one survivor that the object's rebuilt `_$E` static
initialiser calls, or registers through its atexit function (`initialiser_targets`), `exact` with
`tiebreak` and `initialiser` set. Finds and other helpers stay tied. It runs only when the range
resolves nothing more; its credits feed D80's next round.

What the static initialiser leaves gets two D242 rounds (2026-11-12), each feeding D80's next round. First the CRT
table (P-C1-2, amends D94(b); `xcu_position`, USE_XCU_POSITION): the client's initialiser table 0xB6C000-0xB6D700
lists every object's `.CRT$XCU` initialisers in link order, so an object's n-th initialiser is the n-th of the
consecutive table entries in its stretch (each initialiser's window: between its nearest exact own functions in
object order, within the range +- RANGE_GAP, short of the other objects' exact own functions; as many entries as
initialisers, each fullmatching its pattern, none held by another symbol, an exact initialiser at its own entry);
the `_$E` functions those initialisers' bytes reach (the wrapper called, the atexit stub pushed) follow, `exact`
with `xcu` set to 'table' or 'linked'. The `_$E` names stay unused across objects: the position is the proof.
Then object order (P-C1-1; `object_order`, USE_OBJECT_ORDER): tied own functions of one object (not COMDATs:
D189(b)'s order test held for 349 of 353 own groups, 608 of 700 inline ones did not) that share one set of
surviving candidates, as many members as candidates, every candidate inside the object's range, held by no other
symbol and surviving for no tied function outside the group, take the candidates in object order (section, then
offset: with /Gy the linker keeps source order), each between its nearest exact own functions in object order
and not contradicted by the object's own exact calls (P-CGP-1); `exact` with `tiebreak` and `order` set.
wwprofile.obj's Start_Profile 0x9EA930 / Start_Root_Profile 0x9EA9C0, Stop_Profile 0x9EAA50 / Stop_Root_Profile
0x9EAAD0 were the first case.

What every rule above leaves `multiple` gets one funclet round (D246 / C35, USE_FUNCLET_TIES) before the short
round: the one survivor that a matching unwind funclet of an exact function (funclet_targets, the same evidence
as the short `funclet` kind below) jumps or calls to under the function's own symbol; `exact` with `tiebreak`
and `funclet_tie` set. It runs last so that no earlier credit is displaced (in the D80 rounds it moved sclient's
~deque<int> 0x755A10 to emote_data's copy 0x7E6570), and its credits never widen an object's range (the funclet
fixes the symbol's address, not which object's copy the linker kept: dynamic_window_class's
~RefCountPtr<DynamicWindowElementClass> 0x517030 lies in dynamic_window_background's stretch). Mostly shared
COMDAT destructors (~basic_string<char> 0x42A6C0, RefCountPtr / vector instances). What the funclets leave
gets one pushed round (D246 / CR4, USE_PUSHED_TIES) the same way: the one survivor that exact functions'
absolute references under the function's own symbol reach (pushed_targets: an atexit destructor, an eh vector
iterator's element dtor, a qsort / bsearch compare function); `exact` with `tiebreak` and `pushed_tie` set,
never widening an object's range (~RefCountPtr<RenderObjClass> 0x4740B0, IndexClass's search_compfunc 0x9CCC50).
Last, an own-call round (D246 / CR4, USE_CALL_TIES): the one survivor the object's own exact functions' relocated
calls under the function's own symbol reach (obj_called; D166 without its every-caller-is-ours test: it credits
the address, not a placement); `exact` with `tiebreak` and `call_tie` set, never widening an object's range It runs
after D166 has nothing more (run before it, it took D166's credits, which widen ranges, and cost range pairs).
When the object's own calls do not reach one survivor, the one address every exact function's calls reach under
the symbol (D188's `called`, any object's, CR1's evidence) decides (CR7). The same own-call evidence also runs
in the funclet round for a tie the funclets leave (CR5 / CR7): the one unclaimed survivor the object's own exact
functions call; that credit places the object's copy (`call_tie` and `call_placed`) and counts for its range.
(CR1's form, one last round after the loop feeding nothing, adds nothing over these two: measured, dropped.)
The funclet round's evidence reaches further (CR2, run 2026-11-16): (1) USE_FUNCLET_NOTEXACT: the funclets of a parent
that is not exact count as well when client.exe's bytes locate its FuncInfo (the address symbols.json names
with the parent's symbol from a proof other than an exact match, by its EH prologue's pushed stub; else the one
client FuncInfo whose whole unwind map matches the object's), only its matching states; (2) USE_FUNCLET_FOLDED:
several symbols proposed at one address, all COMDAT bodies with one body_sig, are a linker fold (D193 for
non-short bodies: ~RefCountPtr<LegoClass> / <ClientLegoImplClass> 0x700BC0) and are all credited; (3)
USE_FUNCLET_OWN: a symbol reached at several addresses (a template with several client copies) takes, in one
object, the one address that object's own funclets reach; (4) USE_FUNCLET_NAMED: among several reaches, one at
an address symbols.json names with another function (a self name) is dropped (funclet_reach).

Then one short-order round (D246 / CR1, USE_SHORT_ORDER; `short_order`): a gap between two consecutive exact own
functions of one object in object order whose own functions are all `short` is aligned one to one with the client
functions between the two addresses (verified starts, D27) when the counts agree (USE_GAP_ORDER: the gap may
also hold tied `multiple` own functions, `gap_order`), the gap's bytes are only those
functions and their padding, and each fullmatches its counterpart, unclaimed and not contradicted by D80; `exact`
with `order` and `short_order` set. Object order is the proof (D242), the bytes confirm it (D188's never by pattern
alone holds: the pattern decides nothing on its own).

The same round aligns each object's edges (D246 / CR6, USE_EDGE_ORDER; `edge_order`): the own functions before the
object's first exact own function in object order take, nearest first, the client functions right below it, and
those after its last exact one the client functions right after it, each with only padding to its neighbour,
fullmatching, unclaimed and not contradicted by D80; an address two symbols would take stays alone. P-C4-1: the
object's range ends at its exact functions, so an edge function with a byte-identical copy at another object's edge
stayed tied (ui_symbolchat's and sbaseview's option-struct constructors 0x64A330 / 0x69AC20).

The call evidence of D188 may include other libraries' exact functions' calls
(D246 / CR6, USE_EXTERN_CALLED; match_objects' extern_called): verify passes each library the call maps the
previous verify wrote for the others (docs/data/called.json), so no second matching pass is needed.

Last of all, a call-sequence round (D246 / CK, run 2026-11-17, USE_CK_CALLSEQ; `callseq_ties`) for what every rule
above leaves `multiple` or `short`: a function that is not exact but whose address the run knows (the one address
the exact functions' calls give its symbol; with USE_CK_CALLSEQ_NAMED, else D88's self name) is aligned with the
client function there: the object's REL32 relocations in offset order against the client's rel32 calls / jumps that
leave the function (capstone, up to the next verified start; padding after the code), one to one when the counts
agree, one symbol to one address and back, every symbol D80 knows at its known address and at least one so. A tied
or short callee is credited at its aligned target when every aligned caller gives that one address, it fullmatches
there (a `multiple`'s own hit), D80's break_tie leaves it and no other symbol holds or is proposed there (a file
static named in several objects is left alone); `exact` with `ck_callseq` (and `tiebreak`, or `short_known` =
`callseq`), never counted toward ranges. Its credits align like located callers in the next of CK_CALLSEQ_ROUNDS
rounds (their exact calls are not call-tie evidence this late), and (USE_CK_CALLSEQ_DEEP) a not-exact function an
aligned call reaches is located there in turn when every aligned caller gives it that one unheld verified start, so
the alignment follows call chains down (stars.obj's three Create_NEW instances 0x482F00 / 0x482F60 / 0x482FC0). First case: co_data.obj's five hash_map instances
(<ClientObjectClass *, Radar / Gutter / Starmap / Navigation / EmoteDataClass *>), byte-identical: each
ClientObjectDataAccessClass<T>::Get (not exact, located by Get_T_Data's call) gives its find_or_insert, pair
constructor and iterator, and find_or_insert its resize (0x7A0EC0, 0x7A3B50 for Radar).
Vtable slots feed it too (D246 / CK2V, USE_CK_CALLSEQ_VTBL; `callseq_location_vtbl`): a vtable whose client address
the run knows (one address in D80's `known`, an exact function's vptr store), the one address the vptr stores of
located not-exact functions give it (their `??_7` DIR32 relocations in order against the client function's
immediates of stored tables, counts equal, one to one), or (USE_CK_CALLSEQ_VTBL_SLOTS) the one client table (a
`.rdata` address the code stores through `mov [reg], imm32`, as many `.text` slots as ours) that holds every slot the
run knows at one address at its offset, one of them the class's own, places each slot symbol at the client table's
slot (a weak `??_E` read as its `??_G`; one table to one address and back; D80's vtable_check, symbols.json's names
and `known` must not disagree). A not-exact virtual there is a located caller; a tied or short one is a target under
the same conditions as an aligned call's (camctl.obj's CamCtlGadgetClass ??_G / ~ / Unknown_50 0x65BB90..,
sclient.obj's MemberCallback<CameraModuleClass..> Sender_Type / Type 0x7593E0 / 0x759400).

A searched match counts only at a function start (D189(2)): an address the client's thunk table or a
call / jump reaches, or one right after padding (int3 / nop) or after the end of another function (a
`ret`, `ret n`, `jmp`, an indirect `jmp`, or a jump table's last entry), or a 16-byte aligned one (a
function after a switch's byte index table, Hue_Shift 0x94EB20). A hit inside a function (XMP's
Write_Marker without its 0x66 operand-size prefix, 0x9E4F11) is dropped before anything else sees it;
`unanchored` counts the dropped hits. ANCHOR_HITS switches the rule.

A `short` function (fewer than MIN_FIXED fixed bytes: too little to search) is credited only at an
address the run knows independently of its bytes (D188): a slot of a vtable whose address is known
(symbols.json's vtables, or one D171 learned, `vtable`; one an exact function's vptr store gave,
`vptr`), or the target of a call (REL32) in an exact function's relocations (`call`), or (D246 / C21)
the target of a jmp in an unwind funclet of an exact function whose bytes match the object's funclet
for the same unwind state (`funclet`), or (D246 / CR3) the client's immediate of an absolute (DIR32)
reference to the function in an exact function: pushed as an argument (an eh vector iterator's element
ctor / dtor, an atexit destructor, an ostream manipulator), moved into a register or stored (`pushed`,
USE_PUSHED_CALLS, pushed_targets; below call and funclet). Its pattern
must fullmatch there, no relocation target may contradict it (D80's checks), the proofs must name one
address, and no other symbol may hold it. Never by pattern alone. One exception (D193): COMDAT
bodies with the same bytes and the same relocations (a linker-folded inline, such as RefCountPtr's
Peek / operator-> at 0x551D20) that two symbols prove at one address are all credited there, not
dropped. Such a result is `exact` with
`short_known` set to the kind of proof; one in a COMDAT section (an inline, template or
compiler-generated function) is also `shared` (D183's guard: it credits the address, not a file
placement). The round runs when the static initialiser resolves nothing more; its credits feed D80's
next round like any exact function.

    python objmatch.py [--client PATH] [--json OUT] file.obj...
"""
import argparse
import bisect
import collections
import json
import os
import re
import struct
import sys
import threading

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage, DEFAULT_CLIENT  # noqa: E402

MIN_FIXED = 12   # a pattern needs at least this many non-wildcard bytes to be searched
# D189(2): searched hits count only at a function start (function_start). The user accepted its four
# corrections (0x9CEEF7, 0x9F367D, 0x924875, 0x8437A4: mid-function hits, 2026-11-03)
ANCHOR_HITS = True
# D246 / C21 (TK, run 2026-11-15): a short function reached by a jmp from a matching unwind funclet of an
# exact function is credited like a call's target (short_known's `funclet` kind)
USE_FUNCLET_CALLS = True
# D246 / C35 (FE, run 2026-11-15b): the same funclet evidence breaks a D166 tie of a non-short body: of a
# `multiple` function's byte-identical copies, the one a matching unwind funclet of an exact function jumps or
# calls to under the function's own symbol is its address (`funclet_tie`: its own round after every other
# tie rule, before the short round; never widens an object's range)
USE_FUNCLET_TIES = True
# D246 / CR3 (run 2026-11-16): a short function whose address (or its thunk's) an exact function holds as an
# absolute immediate (pushed as an argument, moved into a register, stored) is located by client.exe's bytes
# like a call's target (short_known's `pushed` kind, below call and funclet; pushed_targets)
USE_PUSHED_CALLS = True
# D246 / CR4 (run 2026-11-16): the same pushed evidence breaks a D166 tie of a non-short body, like the funclet
# tie: of a `multiple` function's byte-identical copies, the one exact functions' absolute references under the
# function's own symbol reach (pushed_targets: one client address only) is its address (`pushed_tie`: its own
# round after the funclet round, before the short round; never widens an object's range)
USE_PUSHED_TIES = True
# D246 / CR4: what the pushed round leaves, the object's own exact functions' relocated calls (REL32) under the
# function's own symbol break the tie when they reach one survivor (`call_tie`: its own round once D166's caller
# tie-break resolves nothing more; never widens an object's range). Unlike D166 the address may have callers in other objects too:
# it credits the address (the copy the object's calls reach), not a placement. Only the object's own calls count:
# another object's calls reach its own copy (emote_data / sclient's deque<int>::_Freefront 0x7E7CD0 / 0x756710)
# The same switch, two more forms of the evidence (CR7, folding CR5's and CR1's variants): (1) in the funclet round,
# a tie the funclets leave is the one unclaimed survivor the object's own exact functions call (CR5); such a credit
# places the object's copy and counts for its range (`call_placed`); (2) in the own-call round, when the object's
# own calls do not reach one survivor, the one address every exact function's calls reach under the symbol
# (D188's `called`, CR1's evidence; other libraries' too with USE_EXTERN_CALLED)
USE_CALL_TIES = True
# CR2 (run 2026-11-16): a funclet's reach at an address symbols.json names with another function is not
# evidence for the funclet's symbol (fontdatabase.obj's funclets reach ~StringClass at FontSoundStruct's
# destructor 0x67F660: our member type, not the client's); among several reaches it is dropped and the
# symbol's other reach decides (funclet_reach)
USE_FUNCLET_NAMED = True
# CR2: several symbols the funclet round proposes at one address, all COMDAT bodies with the same bytes and
# relocations (D193's linker fold, for non-short bodies: ~RefCountPtr<LineDisplayTextClass> and
# ~RefCountPtr<FontStyleClass> at 0x51C630), are all credited (funclet_folded)
USE_FUNCLET_FOLDED = True
# CR2: the funclets of a parent whose body is not exact count like an exact parent's (funclet_targets) when
# client.exe's own bytes locate the parent's FuncInfo: the address symbols.json names with the parent's symbol
# from a proof other than an exact match (its EH prologue `push -1; push offset stub`), or the one client
# FuncInfo whose whole unwind map (state count, every state's funclet) matches the object's
# (notexact_funclet_targets)
USE_FUNCLET_NOTEXACT = True
# CR2: a symbol the funclets reach at several client addresses (a template with several copies in client.exe:
# ~RefCountPtr<InterfaceClass> at 0x541A10, 0x54B570, 0x56CE00, 0x643BA0) is, for one object, the one address
# that object's own matching funclets reach (the funclet round only; like P-CGP-1's obj_called)
USE_FUNCLET_OWN = True
# CR5 (run 2026-11-16): a not-exact parent whose whole unwind map matches several client FuncInfos takes the one
# client.exe's placement picks (notexact_funclet_targets): of the candidates pushed once and not by an exact
# function, the one whose push site lies between the client addresses of the parent's exact neighbours in its
# object (the own functions before and after it in object order, D242), else the one whose parent an exact
# function of the same object calls under the parent's symbol (the call target within 0x10 bytes before the
# push); several left, none is used
USE_FUNCLET_PLACED = True
# D246 (CR1, run 2026-11-16): object order places short own functions (D242 / D251's evidence, not the pattern
# alone): between two consecutive exact own functions of one object in object order, when every own function
# between them is short and the client has exactly as many functions between the two addresses, each the same
# length as its counterpart and fullmatching it, unclaimed and uncontradicted, the k-th takes the k-th
# (`short_order`, its own round before the D188 short round)
USE_SHORT_ORDER = True
# D246 (CR1): the same gap alignment when some of the gap's own functions are tied (`multiple`) rather than short
# (`gap_order`); each must still fullmatch the client function at its position, with its length
USE_GAP_ORDER = True
# D246 (CR6): the same alignment at an object's two edges: the own functions before its first exact own function
# (after its last) in object order take the client functions right below it (right after it), padding-only gaps
# (`edge_order`); cross-object byte-identical copies (P-C4-1's range end)
USE_EDGE_ORDER = True
# D246 (EX, run 2026-11-17b): the same alignment at a gap's two edges when the gap also holds a not-exact own
# function (which ends short_order's count test): from the exact P forward and from the exact Q backward, the
# `short` / `multiple` own functions take the client's verified starts next to P's end (Q's start) one by one, each
# its own length, then only padding, fullmatching, unclaimed, D80 not contradicting; the first other one ends that
# side (`gap_edge`; camera_manager's 0x79B4D0 after 0x79B4B0, before the not-exact 0x79B4F0)
USE_EX_GAP_EDGE = True
# ... and a not-exact own function that symbols.json gives one address (D80's known: D88's self name, a proof) is an
# anchor too: the gap members after it take the verified starts from the one after its address, those before it end
# (then padding) at its address; after an object's last such anchor the members follow it the same way
USE_EX_GAP_KNOWN = True
# ... and, forward from an exact anchor's end, a member may start where the padding after the previous function ends
# (16-aligned) when no verified start lies there: functions nothing calls (D27's starts are call targets), the
# padding is the boundary the linker leaves (`gap_edge` too; delta_interpolator's Get_Change 0x4A6360)
USE_EX_GAP_PADDED = True
# D246 (CR6, CR1's EXTERN_CALLED in its cheap form): the relocated calls other libraries' exact functions make (the
# previous verify's call map, docs/data/called.json, passed as match_objects' extern_called) join D188's `called`
USE_EXTERN_CALLED = True
# D246 (CR8): a COMDAT the other rules leave tied (`multiple`) whose one survivor lies in the contribution of an
# object holding the symbol: the linker keeps the first holder's copy in that holder's code, so a copy lies between
# the holder's first exact own function and the next object's first, within RANGE_GAP of the holder's last
# (`contrib`; kv_list's std::_Distance 0x4B7E00, above its range, below kv_tripl's)
USE_CONTRIB_TIES = True
# ... and when several holders' contributions hold a copy, the first holder's in link order (the lowest range)
USE_CONTRIB_FIRST = True
# ... and the `short` COMDATs (too short to search) at the one verified start in the first holder's contribution
# that holds their bytes and length, then padding, D80's break_tie not contradicting
USE_CONTRIB_SHORTS = True
# D246 (CK, run 2026-11-17): a function located by call evidence but not exact (the one address the exact functions'
# calls give its symbol) has the object's relocated calls in the client's order: when the client function there has
# as many outside branches as the object's REL32 relocations, every one under a symbol whose address the run knows
# lands there and at least one does, the others' targets locate the tied / short callees (`ck_callseq`)
USE_CK_CALLSEQ = True
CK_CALLSEQ_ROUNDS = 8
# ... and a caller that symbols.json names from a proof other than an exact match (D88's self name: a vtable slot,
# a string) is located there when no exact function calls it
USE_CK_CALLSEQ_NAMED = True
# ... and a not-exact function an aligned caller's call reaches is located there in turn (one address from every
# aligned caller, a verified start no other symbol holds), so the alignment follows call chains down
USE_CK_CALLSEQ_DEEP = True
# ... and (D246 / CK2V) the vtable slots of the vptrs located constructors store: a vtable the run knows (an exact
# function's vptr store) or one a located not-exact function's vptr stores align with (its `??_7` relocations in
# order against the client function's vtable immediates) gives each slot symbol the client table's slot address,
# a location for a not-exact virtual and a target for a tied / short one (`callseq_location_vtbl`)
USE_CK_CALLSEQ_VTBL = True
# ... and a vtable no vptr store places is at the one client table (a stored vptr, as many `.text` slots as ours) that
# holds every slot whose symbol the run knows at one address, at its offset, at least one of them the class's own
USE_CK_CALLSEQ_VTBL_SLOTS = True
# D246 / VX P1: a candidate address whose `??_7` DIR32 (a vptr store of a table the object defines) holds a client
# table another class owns (a slot whose symbol the run knows elsewhere holds a function symbols.json names as a member
# of a class neither the vtable's nor its slots': `vx_table_foreign`) is not the function, even when the run or
# symbols.json already places the table there (a wrong credit places it: expr_math_float's Create_Binary<float> at
# 0x4CD270 stored 0xAEBF88, ExprMathEvalEqualClass<int>'s table, slot 2 the thunk to Evaluate<int> 0x4CE080).
# Applied to every pattern hit before the D80 rounds (a lone hit included) and in break_tie (`vx_vtable_contradicted`)
USE_VX_VTBL_CONTRA = True
# VX P3: D54's identical-copies check (same_copies) reads an absolute reference as a C string only where the object's
# relocation targets a literal: a vtable's bytes read as one ('se@', the thunk to RefCountClass::Delete_This 0x406573
# little-endian) made the six ~ShareBufferClass<T> copies 0x483600..0x51A7B0, each storing its own table, "identical"
USE_VX_SAME_TARGETS = True
# VX P2: an exact virtual that shares its address with another symbol is not credited there when a client table of its
# own class, placed by the run (one known address, not `vx_table_foreign`), holds another function at its slot
# (client_item_property_special_maker.obj's Get_Lines matched 0x52BC30, ClientItemPropertySpecialEffectClass's; the
# maker's table 0xAEDF00, stored by its exact constructor 0x52C980, holds 0x52CBF0 at that slot): `vx_slot_contradicted`
USE_VX_SLOT_CONTRA = True


def _vx_class(name):
    """VX: the class part of a mangled member / vtable name ('' for a free function): `?Evaluate@?$ExprMathEvalEqualClass@H@@..`
    -> `?$ExprMathEvalEqualClass@H`, `??_7CharacterViewAvatarClass@@6B..` -> `CharacterViewAvatarClass`."""
    if name.startswith(('??_7', '??_G', '??_E')):
        rest = name[4:]
    elif name.startswith(('??0', '??1')):
        rest = name[3:]
    elif name.startswith('?') and not name.startswith('??'):
        rest = name[1:].split('@', 1)[1] if '@' in name[1:] else ''
    else:
        return ''
    return rest.split('@@')[0] if '@@' in rest else ''


_vx_named = None
_vx_virtual = re.compile(r'@@[EFMNUV][AB]E')


def _vx_names():
    """VX: {address: {class keys}} of symbols.json's named functions (thunks left out)."""
    global _vx_named
    if _vx_named is None:
        _vx_named = collections.defaultdict(set)
        for name, addrs in symbol_addresses().items():
            if name.startswith('j_') or name.startswith('??_7'):
                continue
            c = _vx_class(name)
            # only a virtual's name (a non-virtual in a slot is a wrong name: StreakLineClass::Set_Merge_Abort_Factor
            # at 0x974BC0, Bitmap2DObjClass's Set_Aspect slot, a byte-identical setter)
            if c and _vx_virtual.search(name):
                for a in addrs:
                    _vx_named[a].add(c)
    return _vx_named


def vx_table_foreign(obj, sym, raw, known, img):
    """VX P1 / P2: True if the client table at `raw` holds, at a slot whose symbol the run knows elsewhere, a function
    symbols.json names as a member of another class than the vtable's and its slots' (ExprMathEvalEqualClass<float>'s
    Evaluate slot holding Evaluate<int> 0x4CE080): the table is another class's, not ours. A slot holding another of our
    own slots' functions (a slot inserted / removed: CompositeRenderObjClass's Cast_AABox.. one slot off in
    CharacterViewAvatarClass's table) or a member of our own class (an override we lack: ScriptedMovieViewClass's
    0x8023A0 in ViewClass::Unknown_18's slot) is a layout difference, not a contradiction."""
    info = obj.sections[sym['section'] - 1]
    slots = []
    for va, symidx, typ in info['relocs']:
        if typ != 0x06 or va < sym['value']:
            continue
        slot = obj.symbol_at(symidx)
        if slot is not None and slot['class'] in (2, 105):     # 105: a weak `??_E` (read with its `??_G`)
            slots.append((va - sym['value'], slot['name']))
    def at(n):
        # a deleting destructor's slot: VC6 emits `??_G` (scalar) or `??_E` (vector); symbols.json may name either
        twin = ('??_G' if n[3] == 'E' else '??_E') + n[4:] if n.startswith(('??_E', '??_G')) else None
        return known.get(n, set()) | (known.get(twin, set()) if twin else set())
    ours = {_vx_class(sym['name'])} | {_vx_class(n) for _, n in slots}
    mine = set()
    for _, n in slots:
        mine |= at(n)
    named = _vx_names()
    thunk = thunks_of(img)
    for off, n in slots:
        want = at(n)
        if not want:
            continue
        o = img.offset(raw + off)
        if o is None:
            continue
        v = struct.unpack_from('<I', img.data, o)[0]
        have = thunk.get(v, v)
        if have in want or have in mine:
            continue
        cls = named.get(have, set())
        if cls and not cls & ours:
            return True
    return False


def vx_slot_contradicted(img, results, known, thunk):
    """VX P2: the exact results (ids) whose address another symbol also holds while a placed table of the class (one
    known address, not another class's) holds one other address at the symbol's slot."""
    slot_at = collections.defaultdict(set)
    for obj in {id(it[1]): it[1] for it in results}.values():
        for vt, sl in vtable_slots(obj):
            kn = known.get(vt)
            if not kn or len(kn) != 1:
                continue
            raw = next(iter(kn))
            sym = next((x for x in obj.symbols if x['name'] == vt and x['section'] > 0), None)
            if sym is None or vx_table_foreign(obj, sym, raw, known, img):
                continue
            for off, slot in sl:
                o = img.offset(raw + off)
                if o is not None and not generated(slot):
                    v = struct.unpack_from('<I', img.data, o)[0]
                    slot_at[slot].add(thunk.get(v, v))
    holders = collections.defaultdict(set)
    for r, *_ in results:
        if r['status'] == 'exact':
            holders[int(r['addresses'][0], 16)].add(r['symbol'])
    out = set()
    for r, *_ in results:
        if r['status'] != 'exact':
            continue
        a = int(r['addresses'][0], 16)
        want = slot_at.get(r['symbol'])
        if want and len(want) == 1 and a not in want and holders[a] - {r['symbol']}:
            out.add(id(r))
    return out


def vx_vtable_contradicted(obj, sec, start, length, text, base, h, img, known):
    """VX P1: True if a `??_7` DIR32 of the function at `h` (VA; text[0] at `base`) stores a client table whose slots
    contradict the object's vtable with another class's member (`vx_table_foreign`)."""
    o = h - base
    for k, n, typ, sym, addend in obj.reloc_targets(sec, start, length):
        if typ != 0x06 or n != 4 or sym is None or sym['section'] <= 0 or not sym['name'].startswith('??_7') \
                or struct.unpack('<I', addend)[0] != 0 or o + k + 4 > len(text):
            continue
        raw = struct.unpack('<I', text[o + k:o + k + 4])[0]
        if vx_table_foreign(obj, sym, raw, known, img):
            return True
    return False


def generated(name):
    """D94(b): a compiler-generated `_$E<n>` name (a dynamic initialiser or atexit destructor) is
    numbered per translation unit, so the same name is a different function in each object: never
    evidence across objects."""
    return name.startswith('_$E')

RELOC_SIZE = {0x06: 4, 0x07: 4, 0x14: 4, 0x0B: 4, 0x0A: 2, 0x01: 2, 0x02: 2}


class CoffObject:
    """The parts of a COFF .obj that function matching needs."""

    def __init__(self, path):
        d = self.data = open(path, 'rb').read()
        self.path = path
        machine, nsec, _, symptr, nsym, optsize, _ = struct.unpack_from('<HHIIIHH', d, 0)
        if machine != 0x14C:
            raise ValueError(f'{path}: not an x86 object')
        self.sections = []
        for i in range(nsec):
            o = 20 + optsize + 40 * i
            name, vsize, va, rsize, raw, relptr, _, nrel, _, flags = struct.unpack_from('<8sIIIIIIHHI', d, o)
            relocs = [struct.unpack_from('<IIH', d, relptr + 10 * r) for r in range(nrel)]
            self.sections.append({'name': name.rstrip(b'\0').decode('latin1'), 'size': rsize, 'raw': raw,
                                  'relocs': relocs, 'flags': flags})
        strtab = symptr + 18 * nsym
        self.symbols = []
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            raw = d[o:o + 8]
            if raw[:4] == b'\0\0\0\0':
                off = struct.unpack_from('<I', raw, 4)[0]
                name = d[strtab + off:d.index(b'\0', strtab + off)]
            else:
                name = raw.rstrip(b'\0')
            value, secnum, typ, cls, naux = struct.unpack_from('<IhHBB', d, o + 8)
            if cls == 3 and naux and secnum > 0 and value == 0 and typ == 0 \
                    and name.decode('latin1') == self.sections[secnum - 1]['name']:
                # a section definition: its aux record holds the COMDAT selection (D100)
                self.sections[secnum - 1]['select'] = d[o + 18 + 14]
            self.symbols.append({'name': name.decode('latin1'), 'value': value, 'section': secnum,
                                 'type': typ, 'class': cls, 'index': i})
            i += 1 + naux

    def functions(self):
        """[(name, section index, start, end)] for every function defined in a code section."""
        by_sec = {}
        for s in self.symbols:
            if s['section'] > 0 and (s['type'] >> 4) == 2 and s['class'] in (2, 3):
                by_sec.setdefault(s['section'] - 1, []).append(s)
        out = []
        for sec, syms in by_sec.items():
            info = self.sections[sec]
            if not info['flags'] & 0x20:  # IMAGE_SCN_CNT_CODE
                continue
            syms.sort(key=lambda s: s['value'])
            for k, s in enumerate(syms):
                end = syms[k + 1]['value'] if k + 1 < len(syms) else info['size']
                out.append((s['name'], sec, s['value'], end))
        return out

    def own(self, sec):
        """True if the code section holds a function of this translation unit alone (not a COMDAT,
        or a COMDAT with selection NODUPLICATES): not an inline, template or compiler-generated
        copy that the linker may take from another object."""
        info = self.sections[sec]
        return not info['flags'] & 0x1000 or info.get('select') == 1

    def pattern(self, sec, start, end):
        """(regex bytes, fixed byte count, length) for a function, relocations as wildcards."""
        info = self.sections[sec]
        body = bytearray(self.data[info['raw'] + start:info['raw'] + end])
        # Trailing alignment padding is not part of the function.
        while body and body[-1] in (0xCC, 0x90):
            body.pop()
        wild = [False] * len(body)
        for va, _, typ in info['relocs']:
            n = RELOC_SIZE.get(typ, 4)
            for k in range(va - start, va - start + n):
                if 0 <= k < len(body):
                    wild[k] = True
        parts, fixed = [], 0
        for b, w in zip(body, wild):
            if w:
                parts.append(b'.')
            else:
                parts.append(re.escape(bytes([b])))
                fixed += 1
        return b''.join(parts), fixed, len(body)

    def relocated(self, sec, start, length):
        """[(offset in the function, size, type)] of the relocated fields of a function."""
        out = []
        for va, _, typ in self.sections[sec]['relocs']:
            if start <= va < start + length:
                out.append((va - start, RELOC_SIZE.get(typ, 4), typ))
        return out


def _reloc_targets(self, sec, start, length):
    """[(offset in the function, size, type, symbol dict, addend bytes)] of the relocated fields."""
    by_index = getattr(self, '_by_index', None)
    if by_index is None:
        by_index = self._by_index = {s['index']: s for s in self.symbols}
    info = self.sections[sec]
    out = []
    for va, symidx, typ in info['relocs']:
        if start <= va < start + length:
            n = RELOC_SIZE.get(typ, 4)
            out.append((va - start, n, typ, by_index.get(symidx), self.data[info['raw'] + va:info['raw'] + va + n]))
    return out


def _literal(self, sym):
    """The bytes (terminator included) of a string-literal symbol (`??_C@...`), or None."""
    if sym is None or sym['section'] <= 0 or not sym['name'].startswith('??_C@'):
        return None
    info = self.sections[sym['section'] - 1]
    o = info['raw'] + sym['value']
    end = info['raw'] + info['size']
    step = 2 if sym['name'].startswith('??_C@_1') else 1
    while o + step <= end:
        o += step
        if self.data[o - step:o] == b'\0' * step:
            return self.data[info['raw'] + sym['value']:o]
    return None


def _symbol_class(self, name):
    for sym in self.symbols:
        if sym['name'] == name and sym['section'] > 0:
            return sym['class']
    return None


def _symbol_at(self, index):
    if not hasattr(self, '_by_index'):
        self._by_index = {s['index']: s for s in self.symbols}
    return self._by_index.get(index)


CoffObject.symbol_at = _symbol_at
CoffObject.symbol_class = _symbol_class
CoffObject.reloc_targets = _reloc_targets
CoffObject.literal = _literal

_symbol_addresses = None
_self_names = None
_thunk_cache = {}


def thunks_of(img):
    """{thunk VA: target VA} of the client's incremental-link stubs (calls and vtable slots reach
    most functions through them)."""
    if id(img) not in _thunk_cache:
        _thunk_cache[id(img)] = img.incremental_thunks()
    return _thunk_cache[id(img)]



def symbol_addresses():
    """{mangled name: {addresses}} of symbols.json (functions and vtables)."""
    global _symbol_addresses
    if _symbol_addresses is None:
        _symbol_addresses = {}
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data', 'symbols.json')
        try:
            for e in json.load(open(path)):
                if not generated(e['symbol']):
                    _symbol_addresses.setdefault(e['symbol'], set()).add(int(e['address'], 16))
        except (OSError, ValueError, KeyError):
            pass
    return _symbol_addresses


def self_names():
    """D88: {mangled name: {addresses}} of the functions symbols.json names from a proof other than
    an exact match (a vtable slot, a string): the self names that break a `multiple`. An exact entry
    counts when it keeps such a proof (`proof`, D114(c))."""
    global _self_names
    if _self_names is None:
        _self_names = {}
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data', 'symbols.json')
        try:
            for e in json.load(open(path)):
                if e.get('kind') == 'function' and (e.get('source') != 'exact' or e.get('proof')) \
                        and not generated(e['symbol']):
                    _self_names.setdefault(e['symbol'], set()).add(int(e['address'], 16))
        except (OSError, ValueError, KeyError):
            pass
        # a vtable's slot 0 is the class's deleting destructor: symbols.py names it `??_E` (vector),
        # VC6 emits `??_G` (scalar) for a class never new[]'d. Either name is the same slot's proof.
        for k, v in list(_self_names.items()):
            if k.startswith(('??_E', '??_G')):
                _self_names.setdefault(('??_G' if k[3] == 'E' else '??_E') + k[4:], set()).update(v)
    return _self_names


def vtable_check(obj, sym, raw, known, img):
    """For a candidate holding `raw` as the address of the object's vtable `sym` (`??_7...`): True
    if every slot whose function has a known address holds it and at least one does, False if one
    differs, None if no slot is known."""
    info = obj.sections[sym['section'] - 1]
    seen = False
    for va, symidx, typ in info['relocs']:
        if typ != 0x06 or va < sym['value']:
            continue
        slot = obj.symbol_at(symidx)
        if slot is None or slot['class'] != 2 or slot['name'] not in known:
            continue
        o = img.offset(raw + va - sym['value'])
        if o is None:
            return False
        have = thunks_of(img).get(struct.unpack_from('<I', img.data, o)[0], struct.unpack_from('<I', img.data, o)[0])
        if have not in known[slot['name']]:
            return False
        seen = True
    return True if seen else None


def break_tie(obj, sec, start, length, text, base, hits, img, known=None):
    """D80: of the candidate addresses `hits` that match one masked pattern, those the object's
    relocation targets do not contradict. Returns (survivors, confirmed) with `confirmed` the
    survivors that at least one target confirmed: a string literal whose text is the client's
    string at the field, or a symbol symbols.json knows whose address is the field's target."""
    def norm(b):
        # a __FILE__ keeps the file's name only (each E&B project has its own copy of a header's)
        if b'\\' in b or b'/' in b:
            return re.split(rb'[\\/]', b)[-1].lower()
        return b
    known = symbol_addresses() if known is None else known
    fields = obj.reloc_targets(sec, start, length)
    survivors, confirmed = [], []
    for h in hits:
        o = h - base
        ok, sure = True, False
        for k, n, typ, sym, addend in fields:
            if n != 4 or sym is None or o + k + 4 > len(text):
                continue
            raw = struct.unpack('<I', text[o + k:o + k + 4])[0]
            if USE_VX_VTBL_CONTRA and typ == 0x06 and sym['section'] > 0 and sym['name'].startswith('??_7') \
                    and struct.unpack('<I', addend)[0] == 0 \
                    and vx_table_foreign(obj, sym, raw, known, img):
                ok = False      # VX P1: a known table placement the slots contradict confirms nothing
                break
            lit = obj.literal(sym)
            if lit is not None and typ == 0x06:
                add = struct.unpack('<I', addend)[0]
                if add >= len(lit):
                    continue
                want = lit[add:]
                co = img.offset(raw)
                if co is None:
                    ok = False
                    break
                have = img.data[co:co + len(want)]
                # the client's whole string: a header's __FILE__ there can be shorter than ours
                # ('..\shared/auxdata/auxdata.h' against 'e:\build\...\client\..\shared\auxdata\auxdata.h')
                end = img.data.find(b'\0', co)
                whole = img.data[co:end] if end >= 0 else have
                if have == want or norm(whole) == norm(want.rstrip(b'\0')) and want[-1:] == b'\0':
                    sure = True
                else:
                    ok = False
                    break
            elif (typ == 0x06 and sym['section'] > 0 and sym['name'].startswith('??_7') and sym['name'] not in known
                  and struct.unpack('<I', addend)[0] == 0):
                v = vtable_check(obj, sym, raw, known, img)
                if v is False:
                    ok = False
                    break
                sure = sure or v is True
            elif sym['class'] == 2 and sym['name'] in known and typ in (0x06, 0x14):
                target = raw if typ == 0x06 else h + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
                target -= struct.unpack('<I', addend)[0] if typ == 0x06 else 0
                if thunks_of(img).get(target, target) in known[sym['name']]:
                    sure = True
                else:
                    ok = False
                    break
        if ok:
            survivors.append(h)
            if sure:
                confirmed.append(h)
    return survivors, confirmed


def same_copies(text, base, hits, relocs, img=None, strings=None):
    """True if every copy found at `hits` (VAs; text[0] is at VA `base`) is the same function,
    the masked fields included: a relative call/jump (REL32) must reach the same target, any
    other relocated field must hold the same value. Two functions that differ only in their
    callees (or vtables) match one masked pattern but are not identical copies. One exception
    (with `img`): an absolute reference to a C string compares the string, minus any directory
    part, because each E&B project has its own copy of a header's `__FILE__` (0x4A6DF0's is
    `e:\\build\\beta\\G\\app\\shared\\auxdata\\delta_template.h`, 0x7013F0's
    `..\\shared/auxdata/delta_template.h`)."""
    def cstring(va):
        o = img.offset(va)
        if o is None:
            return None
        end = img.data.find(b'\0', o, o + 512)
        s = img.data[o:end] if end > o else b''
        if not s or any(c < 0x20 or c > 0x7E for c in s):
            return None
        return ('str', re.split(rb'[\\/]', s)[-1].lower())

    def fields(h):
        o = h - base
        out = []
        for k, n, typ in relocs:
            raw = text[o + k:o + k + n]
            if typ == 0x14 and n == 4:
                out.append(h + k + 4 + struct.unpack('<i', raw)[0])
            elif typ == 0x06 and n == 4 and img is not None:
                va = struct.unpack('<I', raw)[0]
                # VX P3 (`strings`: the offsets whose target is a literal): any other reference compares its value
                out.append(va if strings is not None and k not in strings else cstring(va) or va)
            else:
                out.append(bytes(raw))
        return out
    first = fields(hits[0])
    return all(fields(h) == first for h in hits[1:])


def demangle(name):
    """Enough of MSVC demangling for reports and source lookup: ?Method@Class@@... -> Class::Method,
    ??0Class@@... -> Class::Class, ??1Class@@... -> Class::~Class, ??4Class@@... -> Class::operator=."""
    if not name.startswith('?'):
        return name.lstrip('_').split('@')[0]
    special = {'0': None, '1': '~', '4': 'operator=', '8': 'operator==', '9': 'operator!=',
               'A': 'operator[]', 'H': 'operator+', 'G': 'operator-', 'D': 'operator*',
               '_G': 'scalar deleting dtor', '_E': 'vector deleting dtor'}
    m = re.match(r'\?\?(_[A-Z]|[0-9A-Z])([^@?]+)@', name)
    if m and m.group(1) in special:
        code, cls = m.group(1), m.group(2)
        method = cls if code == '0' else '~' + cls if code == '1' else special[code]
        return f'{cls}::{method}'
    m = re.match(r'\?([^@?]+)@([^@?]*)@', name)
    if not m:
        return name
    method, cls = m.group(1), m.group(2)
    return f'{cls}::{method}' if cls else method


def vtable_slots(obj):
    """D171: [(vtable symbol, [(offset, slot symbol)])] of the vtables an object defines."""
    if not hasattr(obj, '_vtable_slots'):
        bysec = collections.defaultdict(list)
        for s in obj.symbols:
            if s['section'] > 0:
                bysec[s['section']].append(s['value'])
        out = []
        for sym in obj.symbols:
            if sym['section'] <= 0 or not sym['name'].startswith('??_7'):
                continue
            nxt = min([v for v in bysec[sym['section']] if v > sym['value']] or [1 << 30])
            sl = []
            for va, symidx, typ in obj.sections[sym['section'] - 1]['relocs']:
                if typ == 0x06 and sym['value'] <= va < nxt:
                    slot = obj.symbol_at(symidx)
                    if slot is not None:
                        sl.append((va - sym['value'], slot['name']))
            out.append((sym['name'], sl))
        obj._vtable_slots = out
    return obj._vtable_slots


def learn_short(img, objs, known, thunk):
    """D171: the relocation targets of short functions at addresses the run knows, where their
    pattern holds."""
    for obj in objs:
        if not hasattr(obj, '_shorts'):
            obj._shorts = []
            for name, sec, s, e in obj.functions():
                pat, fixed, length = obj.pattern(sec, s, e)
                if fixed < MIN_FIXED and length >= 5:
                    obj._shorts.append((name, sec, s, length, re.compile(pat, re.S)))
        for name, sec, s, length, pat in obj._shorts:
            addrs = known.get(name)
            if not addrs or len(addrs) != 1:
                continue
            a = next(iter(addrs))
            o = img.offset(a)
            if o is None or not pat.fullmatch(img.data[o:o + length]):
                continue
            for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
                if n != 4 or sym is None or sym['class'] != 2 or obj.literal(sym) is not None or generated(sym['name']):
                    continue
                if typ == 0x06:
                    v = struct.unpack_from('<I', img.data, o + k)[0] - struct.unpack('<I', addend)[0]
                elif typ == 0x14:
                    v = a + k + 4 + struct.unpack_from('<i', img.data, o + k)[0]
                else:
                    continue
                known.setdefault(sym['name'], set()).add(thunk.get(v, v))


def run_self_names(img, objs, known, thunk):
    """D171: self_names() plus the slots of the vtables the run has learned (one address each)."""
    learn_short(img, objs, known, thunk)
    out = {k: set(v) for k, v in self_names().items()}

    def norm(x):
        return '??_G' + x[4:] if x.startswith('??_E') else x
    # an address symbols.json already names (a vtable slot, a string) keeps that name: a learned slot
    # naming another function there adds nothing (WSClassicEnvironmentMapperClass's table points at
    # WSEnvMapperClass's 0x9306E0)
    owners = collections.defaultdict(set)
    for k, v in out.items():
        for a in v:
            owners[a].add(norm(k))
    for obj in objs:
        for name, sl in vtable_slots(obj):
            addrs = known.get(name)
            if not addrs or len(addrs) != 1:
                continue
            vt = next(iter(addrs))
            for off, slot in sl:
                o = img.offset(vt + off)
                if o is None or generated(slot):
                    continue
                v = struct.unpack_from('<I', img.data, o)[0]
                v = thunk.get(v, v)
                if owners.get(v) and norm(slot) not in owners[v]:
                    continue
                if len(symbol_addresses().get(norm(slot), ())) > 1 or len(symbol_addresses().get(slot, ())) > 1:
                    continue    # symbols.json has the symbol at several addresses: which one is this table's?
                out.setdefault(slot, set()).add(v)
                if slot.startswith(('??_E', '??_G')):
                    out.setdefault(('??_G' if slot[3] == 'E' else '??_E') + slot[4:], set()).add(v)
    # a self name is one address: a symbol named at several (two classes of one name, aux_roots' and
    # delta.cpp's DeltaCompoundClass; KVAccessorClass<unsigned> tables) names none of them
    return {k: v for k, v in out.items() if len(v) == 1}


def match_objects(img, paths, region=None, callers=False, extern_called=None, called_out=None):
    """region=(lo, hi): search only [lo, hi) of .text (E&B's app region for app files: a pattern
    that also occurs in the libraries is still exact there). callers=True: D166's caller tie-break
    (caller_tiebreak) for identical copies the other rules leave tied; such a result is 'exact'
    with `caller_tiebreak` set. extern_called ({symbol: addresses}, optional): calls other libraries' exact
    functions make (USE_EXTERN_CALLED); called_out (a dict, optional) receives this run's own {symbol: {addresses}}
    the exact functions call, the extern ones left out."""
    start, end, raw = img.section('.text')
    text = img.data[raw:raw + (end - start)]
    if region:
        lo, hi = max(region[0], start), min(region[1], end)
        text, start = text[lo - start:hi - start], lo
    results, pending = [], []
    thunk = thunks_of(img)
    known = {k: {thunk.get(a, a) for a in v} for k, v in symbol_addresses().items()}
    for path in paths:
        obj = CoffObject(path)
        for name, sec, s, e in obj.functions():
            pat, fixed, length = obj.pattern(sec, s, e)
            r = {'object': os.path.basename(path), 'symbol': name, 'name': demangle(name),
                 'length': length, 'fixed': fixed}
            if fixed < MIN_FIXED:
                r['status'] = 'short'
                r['addresses'] = []
            else:
                hits = [start + m.start() for m in re.finditer(pat, text, re.S)]
                anchored = [h for h in hits if not ANCHOR_HITS or function_start(img, h)]     # D189(2)
                if len(anchored) < len(hits):
                    r['unanchored'] = len(hits) - len(anchored)
                hits = anchored
                if USE_VX_VTBL_CONTRA and hits:
                    kept = [h for h in hits if not vx_vtable_contradicted(obj, sec, s, length, text, start, h, img, known)]
                    if len(kept) < len(hits):
                        r['vx_rejected'] = [f'0x{a:06X}' for a in hits if a not in kept][:5]
                    hits = kept
                r['addresses'] = [f'0x{a:06X}' for a in hits[:5]]
                r['status'] = 'exact' if len(hits) == 1 else 'multiple' if hits else 'none'
                if len(hits) == 1 and r.get('vx_rejected'):
                    # VX P1: a lone survivor of rejected copies is not exact by elimination: D80 must confirm it
                    # (wwlib's Surface::Surface: 0x4DCD10 stores AttribDirCollective's MemberCallbackClass table,
                    # and 0x6AC050's table 0xAFDB28, six slots, is not Surface's either)
                    r['status'] = 'multiple'
                    r['count'] = 1
                    r['identical'] = False     # one copy: not D54's ambiguous column
                    pending.append((r, obj, sec, s, length, hits))
                if len(hits) > 1:
                    # every copy, and whether they are all the same function (D54)
                    r['count'] = len(hits)
                    strs = {k for k, n, typ, sym, add in obj.reloc_targets(sec, s, length)
                            if sym is not None and obj.literal(sym) is not None} if USE_VX_SAME_TARGETS else None
                    r['identical'] = same_copies(text, start, hits, obj.relocated(sec, s, length), img, strs)
                    pending.append((r, obj, sec, s, length, hits))
            results.append((r, obj, sec, s, length))
    # D80: what the exact functions show about the link (an external symbol's address) helps to break
    # the ties of the others; repeat until nothing more resolves (a destructor found once settles the
    # scalar deleting destructors that call it).
    done = set()

    # one client function is one source function (VC6 does not fold identical code): an address two
    # different symbols would take, or that a symbol of another name already holds, is left alone
    claimed = collections.defaultdict(set)
    sole = collections.defaultdict(set)  # symbol -> the addresses it is exact at with no other symbol there
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            claimed[int(r['addresses'][0], 16)].add(r['symbol'])
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact' and claimed[int(r['addresses'][0], 16)] == {r['symbol']}:
            sole[r['symbol']].add(int(r['addresses'][0], 16))

    def learn(r, obj, sec, s, length, addr):
        # A COMDAT copy of an inline function can be byte-identical to another function of its
        # object (dbgconn.obj's inline ~ForwardingConnectionClass and ~StatsConnectionClass 0x8251A0,
        # P1): when the symbol also matches an address of its own elsewhere, the shared one is the
        # other function's and must not become a "known" target that confirms the wrong twin.
        shared = claimed[addr] - {r['symbol']}
        if r['symbol'] and not generated(r['symbol']) and obj.symbol_class(r['symbol']) == 2 \
                and not (shared and sole[r['symbol']] - {addr}):
            known.setdefault(r['symbol'], set()).add(addr)
        o = addr - start
        for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
            if n != 4 or sym is None or sym['class'] != 2 or obj.literal(sym) is not None or generated(sym['name']):
                continue
            if typ == 0x06:
                v = struct.unpack('<I', text[o + k:o + k + 4])[0] - struct.unpack('<I', addend)[0]
                if sym['name'].startswith('??_7'):
                    vptrs[sym['name']].add(v)            # D188: a vptr store (constructor / destructor)
            elif typ == 0x14:
                v = addr + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
                called[sym['name']].add(thunk.get(v, v) & 0xFFFFFFFF)   # D188: a call's target
                own_called[sym['name']].add(thunk.get(v, v) & 0xFFFFFFFF)
                obj_called[(id(obj), sym['name'])].add(thunk.get(v, v) & 0xFFFFFFFF)   # P-CGP-1
            else:
                continue
            known.setdefault(sym['name'], set()).add(thunk.get(v, v))
        if USE_FUNCLET_CALLS:
            for name, t in funclet_targets(img, obj, sec, s, length, addr, thunk):
                funclet_called[name].add(t)      # D246 / C21: a matching unwind funclet's jump
                obj_funclet[(id(obj), name)].add(t)  # CR2: the object's own funclets' reach
        if USE_PUSHED_CALLS:
            for name, t, _ in pushed_targets(img, obj, sec, s, length, addr, text, start, thunk):
                pushed_called[name].add(t)       # D246 / CR3: an absolute function reference's immediate
    called = collections.defaultdict(set)   # D188: {symbol: {addresses}} the exact functions call
    own_called = collections.defaultdict(set)   # the same, this run's own functions only (called_out)
    if USE_EXTERN_CALLED and extern_called:
        for k, v in extern_called.items():
            called[k] |= set(v)
    funclet_called = collections.defaultdict(set)   # D246 / C21: {symbol: {addresses}} their funclets jump to
    pushed_called = collections.defaultdict(set)    # D246 / CR3: {symbol: {addresses}} they push / load / store
    obj_funclet = collections.defaultdict(set)      # CR2: {(object, symbol): {addresses}} its own funclets jump to
    vptrs = collections.defaultdict(set)    # D188: {vtable symbol: {addresses}} their vptr stores give
    obj_called = collections.defaultdict(set)   # P-CGP-1: {(object, symbol): {addresses}} its exact functions call
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            learn(r, obj, sec, s, length, int(r['addresses'][0], 16))
    if USE_FUNCLET_CALLS and USE_FUNCLET_NOTEXACT:
        held = {a for a, v in claimed.items() if v}
        bounds = {}     # CR5: {id(r): (exact own neighbour before, after)} in object order (D242)
        for order in _object_order(results).values():
            for k, a, item in order:
                if a is None:
                    lo, hi = _neighbours(order, k)
                    if lo is not None and hi is not None:
                        bounds[id(item[0])] = (lo, hi)
        for r, obj, sec, s, length in results:
            if r['status'] in ('none', 'multiple'):
                for name, t in notexact_funclet_targets(img, obj, sec, s, length, r['symbol'], thunk, held,
                                                        bounds.get(id(r)), obj_called.get((id(obj), r['symbol']), ())):
                    funclet_called[name].add(t)     # CR2: a not-exact parent's matching funclets
                    obj_funclet[(id(obj), name)].add(t)
    shorts = [x for x in results if x[0]['status'] == 'short' and x[4] > 0]
    folded_at = collections.defaultdict(lambda: collections.defaultdict(set))  # D193: {address: {COMDAT body: {symbols}}}
    # D88: when D80 resolves nothing more, the self name breaks a tie: the one survivor that
    # symbols.json names with the function's own symbol (from a vtable slot or a string, not an
    # exact match); its credits may let D80 resolve more, so the loop goes on
    # D100: when the self name resolves nothing more either, the range breaks a tie: the one survivor
    # inside the object's client range (first..last address of its own exact functions, not COMDAT
    # copies the linker may take from another object); then back to D80
    # D174 (P-TK2-1(b)): the caller tie-break's credits feed D80, the self name and the range like any
    # exact function, and what those credit may let the caller tie-break resolve more: repeat both
    call_tied = set()   # CR5: id(r) of the funclet round's own-call credits
    call_round, call_credited = False, True     # D246 / CR4: the own-call round, after D166 (below)
    while True:
        phase = 'calltie' if call_round else 'd80'
        changed = True
        while changed:
            changed = False
            proposals = []
            selfname = phase == 'selfname'
            if selfname:
                names = run_self_names(img, list({id(x[1]): x[1] for x in results}.values()), known, thunk)
            ranges = {}
            if phase in ('range', 'init', 'xcu', 'order'):
                for r, obj, sec, s, length in results:
                    # D246 / C35: a funclet tie fixes the symbol's address, not which object's copy the linker kept
                    if r['status'] == 'exact' and obj.own(sec) and not r.get('funclet_tie') \
                            and not r.get('pushed_tie') and not (r.get('call_tie') and not r.get('call_placed')):
                        a = int(r['addresses'][0], 16)
                        lo, hi = ranges.get(id(obj), (a, a))
                        ranges[id(obj)] = (min(lo, a), max(hi, a))
                # D114(b): an exact COMDAT that only this object is exact at, within RANGE_GAP of either
                # end of the object's own range, widens it (the linker placed that copy with this object)
                holders = collections.defaultdict(set)
                for r, obj, sec, s, length in results:
                    if r['status'] == 'exact':
                        holders[int(r['addresses'][0], 16)].add(id(obj))
                wide = dict(ranges)
                grown = True
                while grown:    # chained: a COMDAT within RANGE_GAP of the widened range widens it too
                    grown = False
                    for r, obj, sec, s, length in results:
                        if r['status'] == 'exact' and not obj.own(sec) and id(obj) in ranges and not r.get('funclet_tie') \
                                and not r.get('pushed_tie') and not (r.get('call_tie') and not r.get('call_placed')):
                            a = int(r['addresses'][0], 16)
                            wlo, whi = wide[id(obj)]
                            if wlo <= a <= whi:
                                continue
                            # D196: only a COMDAT this object alone is exact at chains, and only upwards (an
                            # object's copies and initialisers follow its own code); D183: one held by several
                            # objects, all under this symbol, when this holder's own range is strictly the
                            # nearest of the holders', and anything below, are measured from the own range
                            # (chained, ui_shipcust's range ran down over ui_ship_cust's copies of their
                            # shared map type on 217fd903: 0x628E40 lost)
                            lo, hi = ranges[id(obj)][0], (whi if holders[a] == {id(obj)} else ranges[id(obj)][1])
                            if (holders[a] == {id(obj)} or claimed[a] == {r['symbol']} and all(
                                    range_distance(ranges[id(obj)], a) < range_distance(ranges[o], a)
                                    for o in holders[a] if o != id(obj) and o in ranges)) and \
                                    lo - RANGE_GAP <= a <= hi + RANGE_GAP:
                                wide[id(obj)] = (min(wlo, a), max(whi, a))
                                grown = True
                ranges = wide
            if phase == 'range':
                # D209 (IR3): the object's static initialisers (and their atexit functions) located by
                # initialiser_targets inside the range widen it, so a tie lying among them is inside
                located = collections.defaultdict(set)
                initialiser_targets(results, pending, ranges, text, start, known, thunk, located)
                # P-IR3-1 narrowed (NM1): (a) a spot two objects locate is a copy, evidence for neither; (b) a
                # widening that takes in a second copy of one of the object's tied initialisers over-reaches
                finders = collections.defaultdict(set)
                for key, spots in located.items():
                    for a in spots:
                        finders[a].add(key)
                tied = collections.defaultdict(list)
                for r, obj, sec, s, length, hits in pending:
                    if id(r) not in done and generated(r['symbol']):
                        tied[id(obj)].append(hits)
                xprev = xcu_previous(img) if USE_XCU and region else {}   # the table marks TU boundaries (zero slots) only in the app stretch
                for key, spots in located.items():
                    if any(len(finders[a]) > 1 for a in spots):
                        continue
                    lo, hi = ranges[key]
                    nlo, nhi = min(lo, min(spots)), max(hi, max(spots))
                    if any(sum(nlo <= h <= nhi for h in hits) >= 2 and
                           sum(nlo <= h <= nhi for h in hits) > sum(lo <= h <= hi for h in hits)
                           for hits in tied[key]):
                        continue
                    if any(a < lo and a in xprev and xprev[a] and xprev[a] not in spots for a in spots):
                        continue
                    ranges[key] = (nlo, nhi)
            registered = initialiser_targets(results, pending, ranges, text, start, known, thunk) if phase == 'init' else {}
            if phase == 'shortorder':
                # D246 / CR1: short own functions placed by object order
                got = short_order(img, results, text, start, claimed, known)
                if USE_EDGE_ORDER:
                    # D246 / CR6: the same alignment at each object's edges, after the gaps
                    taken = {a for _, a in got}
                    edge = [(it, a) for it, a in edge_order(img, results, text, start, claimed, known)
                            if a not in taken]
                    for it, _ in edge:
                        it[0]['edge_order'] = True
                    got = got + edge
                if USE_EX_GAP_EDGE:
                    # D246 (EX): the gaps' edges when a not-exact own function sits inside the gap
                    taken = {a for _, a in got} | {it[0]['symbol'] for it, _ in got}
                    gedge = [(it, a) for it, a in gap_edges(img, results, text, start, claimed, known)
                             if a not in taken and it[0]['symbol'] not in taken]
                    for it, _ in gedge:
                        it[0]['gap_edge'] = True
                    got = got + gedge
                for (r, obj, sec, s, length), a in got:
                    if r['status'] == 'multiple':
                        r['tiebreak'] = True
                        if not r.get('edge_order') and not r.get('gap_edge'):
                            r['gap_order'] = True
                        r.pop('count', None), r.pop('identical', None)
                        done.add(id(r))
                    elif not r.get('edge_order') and not r.get('gap_edge'):
                        r['short_order'] = True
                    r['status'] = 'exact'
                    r['addresses'] = [f'0x{a:06X}']
                    r['order'] = True
                    claimed[a].add(r['symbol'])
                    learn(r, obj, sec, s, length, a)
                shorts = [x for x in shorts if x[0]['status'] == 'short']
                phase = 'd80' if got else 'short'
                changed = True
                continue
            if phase == 'short':
                # D188: a short function at an address the run knows independently of its bytes
                got = short_known(img, shorts, text, start, known, thunk, called, vptrs, claimed, folded_at,
                                  funclet_reach(funclet_called), pushed_called)
                for (r, obj, sec, s, length), a, kind in got:
                    if not obj.own(sec):
                        folded_at[a][body_sig(obj, sec, s, length)].add(r['symbol'])
                    r['status'] = 'exact'
                    r['addresses'] = [f'0x{a:06X}']
                    r['short_known'] = kind
                    claimed[a].add(r['symbol'])
                    learn(r, obj, sec, s, length, a)
                    changed = True
                shorts = [x for x in shorts if x[0]['status'] == 'short']
                phase = 'd80' if changed else 'done'
                continue
            if phase == 'xcu':
                # D242 P-C1-2: the `_$E` initialisers by their CRT table position, and what their bytes reach
                got = xcu_position(img, results, ranges, text, start, claimed, thunk) if USE_XCU_POSITION else []
                for (r, obj, sec, s, length), a, kind in got:
                    if r['status'] == 'multiple':
                        r['tiebreak'] = True
                        r.pop('count', None), r.pop('identical', None)
                        done.add(id(r))
                    r['status'] = 'exact'
                    r['addresses'] = [f'0x{a:06X}']
                    r['xcu'] = kind
                    claimed[a].add(r['symbol'])
                    learn(r, obj, sec, s, length, a)
                    changed = True
                shorts = [x for x in shorts if x[0]['status'] == 'short']
                phase = 'd80' if changed else 'order'
                changed = True
                continue
            if phase == 'order':
                # D242 P-C1-1: a tie among one object's own functions, assigned in object order
                got = []
                if USE_OBJECT_ORDER:
                    surv = {id(it[0]): break_tie(it[1], it[2], it[3], it[4], text, start, it[5], img, known)[0]
                            for it in pending if id(it[0]) not in done}
                    got = object_order(pending, done, surv, ranges, claimed, obj_called, results)
                for (r, obj, sec, s, length, hits), a in got:
                    r['status'] = 'exact'
                    r['addresses'] = [f'0x{a:06X}']
                    r['tiebreak'] = True
                    r['order'] = True
                    r.pop('count', None), r.pop('identical', None)
                    claimed[a].add(r['symbol'])
                    learn(r, obj, sec, s, length, a)
                    done.add(id(r))
                # D246 / C35: what every other tie rule left, by funclet evidence, before the short round
                phase = 'd80' if got else 'funclet' if USE_FUNCLET_TIES else 'pushed' if USE_PUSHED_TIES else 'shortorder' if USE_SHORT_ORDER else 'short'
                changed = True
                continue
            for item in pending:
                r, obj, sec, s, length, hits = item
                if id(r) in done:
                    continue
                survivors, confirmed = break_tie(obj, sec, s, length, text, start, hits, img, known)
                if phase == 'd80':
                    if len(survivors) == 1 and confirmed:
                        proposals.append((item, survivors[0]))
                elif phase == 'funclet':
                    # D246 / C35: one address a matching funclet reaches under this symbol, among the survivors
                    if obj.symbol_class(r['symbol']) == 2 and not generated(r['symbol']):
                        reach = funclet_reach(funclet_called).get(r['symbol'], ())
                        if len(reach) != 1 and USE_FUNCLET_OWN:
                            # CR2: a template with several client copies: the copy the object's own funclets reach
                            # (only when the symbol's reach is not one address: the symbol's evidence comes first,
                            # as in D254; trying the object's own first lost 12 ~RefCountPtr<SceneClass> 0x5C40C0)
                            own = obj_funclet.get((id(obj), r['symbol']), set())
                            reach = funclet_reach({r['symbol']: own}).get(r['symbol'], ())
                        if len(reach) == 1 and next(iter(reach)) in survivors:
                            proposals.append((item, next(iter(reach))))
                        elif USE_CALL_TIES:
                            # CR5 / CR7: the one survivor the object's own exact functions call (~DynamicWindow-
                            # HorizontalClass 0x5187B0, called by its scalar deleting destructor 0x518780)
                            reach = obj_called.get((id(obj), r['symbol']), set())
                            hit = [h for h in survivors if len(reach) == 1 and h in reach
                                   and not claimed[h] - {r['symbol']}]
                            if len(hit) == 1:
                                proposals.append((item, hit[0]))
                                call_tied.add(id(r))
                elif phase == 'calltie':
                    # D246 / CR4: one survivor the object's own exact functions call under this symbol
                    if obj.symbol_class(r['symbol']) == 2 and not generated(r['symbol']):
                        reach = obj_called.get((id(obj), r['symbol']), ())
                        if len(reach) != 1:
                            # CR1 / CR7: else the one address every exact function's calls reach (Create_Peek
                            # 0x55C490, RefCountPtr<RenderObjClass>::operator= 0x5988A0)
                            reach = called.get(r['symbol'], ())
                        if len(reach) == 1 and next(iter(reach)) in survivors:
                            proposals.append((item, next(iter(reach))))
                elif phase == 'pushed':
                    # D246 / CR4: one address exact functions' absolute references reach under this symbol
                    if obj.symbol_class(r['symbol']) == 2 and not generated(r['symbol']):
                        reach = pushed_called.get(r['symbol'], ())
                        if len(reach) == 1 and next(iter(reach)) in survivors:
                            proposals.append((item, next(iter(reach))))
                elif phase == 'selfname':
                    own = [h for h in survivors if h in names.get(r['symbol'], ())]
                    if len(own) == 1:
                        proposals.append((item, own[0]))
                elif phase == 'init':
                    # D115: a tied constructor / destructor is the one the object's static initialiser
                    # calls or registers; finds and other helpers stay tied
                    if r['symbol'].startswith(('??0', '??1')):
                        own = [h for h in survivors if h in registered.get(r['symbol'], ())]
                        if len(own) == 1:
                            proposals.append((item, own[0]))
                elif id(obj) in ranges:
                    lo, hi = ranges[id(obj)]
                    # an address another function is already exact at is not this one's (one client function
                    # is one source function)
                    inside = [h for h in survivors if lo <= h <= hi and not claimed[h] - {r['symbol']}]
                    # P-CGP-1: nor one the object's own exact callers contradict (their relocated calls to
                    # this symbol reach another address: ui_cpit's Create_Peek<FontSys> 0x546510, not the
                    # same-bytes Create_Peek<VertexMaterialClass> 0x584DF0)
                    reach = obj_called.get((id(obj), r['symbol']))
                    if reach:
                        inside = [h for h in inside if h in reach]
                    if len(inside) == 1:
                        proposals.append((item, inside[0]))
            by_addr = collections.defaultdict(set)
            for (r, *_), a in proposals:
                by_addr[a].add(r['symbol'])
            winner = {}
            if phase == 'range' and USE_RANGE_EVIDENCE:
                # P-CW1-1: several symbols proposing one address in the range round: the one whose own evidence
                # there is strictly the strongest is credited (range_evidence), the others stay tied
                best = collections.defaultdict(dict)
                for item, a in proposals:
                    if len(by_addr[a]) > 1:
                        r, obj, sec, s, length, hits = item
                        e = range_evidence(obj, sec, s, length, text, start, a, ranges.get(id(obj)), thunk,
                                           names.get(r['symbol'], ()), known.get(r['symbol'], ()))
                        best[a][r['symbol']] = min(best[a].get(r['symbol'], e), e)   # each copy of the symbol must hold
                for a, ev in best.items():
                    left = {k: e for k, e in ev.items() if e[0]}     # not refuted by a callee's bytes
                    if len(left) == 1:
                        winner[a] = next(iter(left))
                    elif left:
                        top = max(left.values())
                        if top > (True, False, 0) and sum(e == top for e in left.values()) == 1:
                            winner[a] = next(k for k, e in left.items() if e == top)
            folds = funclet_folded(proposals, claimed) if phase == 'funclet' else {}
            for item, a in proposals:
                r, obj, sec, s, length, hits = item
                if r['symbol'] in folds.get(a, ()):
                    pass                                # CR2: a linker-folded COMDAT body, every symbol credited
                elif len(by_addr[a]) > 1 and winner.get(a) != r['symbol'] or claimed[a] - {r['symbol']}:
                    continue
                r['status'] = 'exact'
                r['addresses'] = [f'0x{a:06X}']
                r['tiebreak'] = True
                if selfname:
                    r['selfname'] = True
                if phase == 'range':
                    r['range'] = True
                if phase == 'init':
                    r['initialiser'] = True
                if phase == 'funclet' and id(r) in call_tied:
                    r['call_tie'] = r['call_placed'] = True     # CR5: the object's own call places its copy
                elif phase == 'funclet':
                    r['funclet_tie'] = True
                if phase == 'pushed':
                    r['pushed_tie'] = True
                if phase == 'calltie':
                    r['call_tie'] = True
                r.pop('count'), r.pop('identical')
                claimed[a].add(r['symbol'])
                learn(r, obj, sec, s, length, a)
                done.add(id(r))
                changed = True
            if phase == 'd80':
                if not changed:
                    phase, changed = 'selfname', True   # D80 is done: one self-name round
            elif changed:
                phase = 'd80'                           # back to D80 after a round that credited
            elif phase == 'funclet':
                # nor the funclets (D246 / C35): the pushed references (D246 / CR4), then one short round
                phase, changed = ('pushed' if USE_PUSHED_TIES else 'shortorder' if USE_SHORT_ORDER else 'short'), True
            elif phase == 'pushed':
                phase, changed = ('shortorder' if USE_SHORT_ORDER else 'short'), True   # nor the pushed references (D246 / CR4): short order (CR1), then one short round
            elif phase == 'calltie':
                pass                                    # the own-call round credited nothing (D246 / CR4)
            elif phase == 'selfname':
                phase, changed = 'range', True          # neither D80 nor the self name: one range round
            elif phase == 'range':
                phase, changed = 'init', True           # nor the range: one static initialiser round (D115)
            elif phase == 'init':
                phase, changed = 'xcu', True            # nor the initialiser: the CRT table round (D242), the
                                                        # object-order round (D242), then one short round (D188)
        # D246 / CR4: the own-call round runs only when D166 resolves nothing more (D166's credits widen ranges,
        # an own-call credit never does: run first, it took D166's credits and cost range pairs), and again only
        # after a round of it that credited
        if call_round:
            call_credited = phase != 'calltie'
        credited = caller_tiebreak(img, results, pending, claimed, text, start, thunk, known) if callers else []
        call_round = False
        if not credited:
            if USE_CALL_TIES and call_credited:
                call_round, call_credited = True, False
                continue
            break
        for item in credited:
            r, obj, sec, s, length, hits = item
            done.add(id(r))
            learn(r, obj, sec, s, length, int(r['addresses'][0], 16))
    if USE_CONTRIB_TIES:
        for item, a in contribution_ties(results, pending, done, claimed, text, start, img, known):
            r, obj, sec, s, length, hits = item
            r['status'] = 'exact'
            r['addresses'] = [f'0x{a:06X}']
            r['tiebreak'] = True
            r['contrib'] = True
            r.pop('count', None), r.pop('identical', None)
            claimed[a].add(r['symbol'])
            learn(r, obj, sec, s, length, a)
            done.add(id(r))
    if USE_CK_CALLSEQ:
        located = {}    # USE_CK_CALLSEQ_DEEP: {symbol: address or None} of not-exact functions aligned calls reach
        for _ in range(CK_CALLSEQ_ROUNDS):
            n_located = len(located)
            vslots = callseq_location_vtbl(img, results, claimed, called, text, start, thunk, known, located)                 if USE_CK_CALLSEQ_VTBL else None
            got = callseq_ties(img, results, pending, done, claimed, called, text, start, thunk, known, located,
                               vslots)
            for item, a in got:
                r, obj, sec, s, length = item[:5]
                if r['status'] == 'short':
                    r['short_known'] = 'callseq'
                else:
                    r['tiebreak'] = True
                    done.add(id(r))
                r['status'] = 'exact'
                r['addresses'] = [f'0x{a:06X}']
                r['ck_callseq'] = True
                r.pop('count', None), r.pop('identical', None)
                claimed[a].add(r['symbol'])
                learn(r, obj, sec, s, length, a)
            if not got and len(located) == n_located:
                break
    if USE_VX_SLOT_CONTRA:
        bad = vx_slot_contradicted(img, results, known, thunk)
        for r, *_ in results:
            if id(r) in bad:
                r['vx_slot'] = r['addresses'][0]
                r['status'] = 'none'
                for k in ('tiebreak', 'range', 'selfname', 'order', 'call_tie', 'call_placed', 'funclet_tie',
                          'pushed_tie', 'contrib', 'ck_callseq', 'short_known', 'caller_tiebreak', 'initialiser', 'xcu'):
                    r.pop(k, None)
    if called_out is not None:
        called_out.update({k: set(v) for k, v in own_called.items()})
    for r, *_ in results:
        r.pop('shared', None)
        if r['status'] != 'exact' or not r.get('shared_callers'):
            r.pop('shared_callers', None)
    shared = shared_pairs(results)
    for r, obj, sec, *_ in results:
        if r['status'] == 'exact' and (int(r['addresses'][0], 16), r['symbol']) in shared:
            r['shared'] = True
        if r.get('short_known') and not obj.own(sec):
            r['shared'] = True      # D188: a short COMDAT credits the address, not a file placement
    return [x[0] for x in results]


def contribution_ties(results, pending, done, claimed, text, base, img, known):
    """D246 (CR8): [(pending item, address)] for tied COMDATs (`multiple`, not own code) whose survivors (break_tie)
    include exactly one that lies in the contribution of an object holding the symbol: at or above that object's
    first exact own function, below the next object's, at most RANGE_GAP above its last; the address claimed by no
    other symbol and proposed by no other symbol. Ranges are the exact own functions' (no ties of the tie rounds
    that fix a symbol's address but not the copy: funclet, pushed, unplaced call ties)."""
    ranges = {}
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact' and obj.own(sec) and not r.get('funclet_tie') and not r.get('pushed_tie')                 and not (r.get('call_tie') and not r.get('call_placed')):
            a = int(r['addresses'][0], 16)
            lo, hi = ranges.get(id(obj), (a, a))
            ranges[id(obj)] = (min(lo, a), max(hi, a))
    if not ranges:
        return []
    los = sorted((lo, key) for key, (lo, hi) in ranges.items())
    keys = [lo for lo, _ in los]
    holders = collections.defaultdict(set)
    for r, obj, *_ in results:
        holders[r['symbol']].add(id(obj))

    def owner(h):
        i = bisect.bisect_right(keys, h) - 1
        if i < 0:
            return None
        key = los[i][1]
        if i + 1 < len(los) and keys[i + 1] == keys[i]:
            return None     # two objects start at one address: no single contribution
        return key if h <= ranges[key][1] + RANGE_GAP else None

    props = []
    for item in pending:
        r, obj, sec, s, length, hits = item
        if id(r) in done or r['status'] != 'multiple' or obj.own(sec) or generated(r['symbol']):
            continue
        survivors = break_tie(obj, sec, s, length, text, base, hits, img, known)[0]
        inside = [h for h in survivors if owner(h) in holders[r['symbol']] and not claimed[h] - {r['symbol']}]
        if len(inside) > 1 and USE_CONTRIB_FIRST:
            # several holders' contributions hold a copy: the linker keeps the first holder's in link order
            first = min((ranges[k][0], k) for k in holders[r['symbol']] if k in ranges)[1]
            inside = [h for h in inside if owner(h) == first]
        if len(inside) == 1:
            props.append((item, inside[0]))
    if USE_CONTRIB_SHORTS:
        # a short COMDAT (too short to search): the verified starts in its holders' contributions where its bytes
        # fullmatch, as long as the client function there (then padding), confirmed by D80's break_tie
        starts = _verified_starts(img)
        for r, obj, sec, s, length in results:
            if r['status'] != 'short' or length <= 0 or obj.own(sec) or generated(r['symbol'])                     or obj.symbol_class(r['symbol']) != 2:
                continue
            inside = []
            held = [k for k in holders[r['symbol']] if k in ranges]
            # only the first holder's contribution in link order: a lone copy in a later holder's code may be another
            # function with its bytes (an own getter of the object), the first holder's copy not found
            for k in [min((ranges[k][0], k) for k in held)[1]] if held else []:
                lo = ranges[k][0]
                i = bisect.bisect_right(keys, lo)
                hi = min(ranges[k][1] + RANGE_GAP, keys[i] - 1 if i < len(keys) else ranges[k][1] + RANGE_GAP)
                j = bisect.bisect_left(starts, lo)
                while j < len(starts) and starts[j] <= hi:
                    a = starts[j]
                    e = starts[j + 1] if j + 1 < len(starts) else a + length
                    if owner(a) == k and not claimed[a] - {r['symbol']} and                             _fullmatch_at(obj, sec, s, length, text, base, a) and _padding(text, base, a + length, e)                             and break_tie(obj, sec, s, length, text, base, [a], img, known)[0] == [a]:
                        inside.append(a)
                    j += 1
            if len(inside) == 1:
                props.append(((r, obj, sec, s, length, []), inside[0]))
    by_addr = collections.defaultdict(set)
    for (r, *_), a in props:
        by_addr[a].add(r['symbol'])
    return [(item, a) for item, a in props if len(by_addr[a]) == 1]


def _client_branches(img, a, end, thunk):
    """[target] of the rel32 calls / jumps (E8, E9, 0F 8x) in the client function [a, end) that leave it, in address
    order (thunks resolved); None when the disassembly stops short of END (data in the body)."""
    out = []
    last = a
    for ins in img.disasm(a, end - a):
        last = ins.address + ins.size
        b = ins.bytes
        if (len(b) == 5 and b[0] in (0xE8, 0xE9)) or (len(b) == 6 and b[0] == 0x0F and 0x80 <= b[1] <= 0x8F):
            t = (ins.address + ins.size + struct.unpack_from('<i', b, len(b) - 4)[0]) & 0xFFFFFFFF
            if not a <= t < end:
                out.append(thunk.get(t, t))
        if last >= end:
            break
    return out if last >= end or _padding_img(img, last, end) else None


def _padding_img(img, lo, hi):
    o = img.offset(lo)
    return all(b in (0xCC, 0x90) for b in img.data[o:o + (hi - lo)])


def callseq_ties(img, results, pending, done, claimed, called, text, base, thunk, known, located=None, vslots=None):
    """D246 (CK): [(item, address)] for tied (`multiple`) and `short` functions that a located not-exact caller's
    calls reach. A caller is located when the exact functions' calls give its symbol exactly one address, a verified
    start no other symbol holds. Its object's REL32 relocations (offset order) align one to one with the client
    function's outside rel32 branches (address order, up to the next verified start, padding excepted) when the counts
    agree, one symbol takes one address and distinct symbols distinct ones, every symbol the run knows (D80's
    `known`) is at its known address and at least one is. A callee item is credited at its aligned target when every
    located caller gives the symbol that one address, its pattern fullmatches there (for a `multiple`, the target is
    one of its hits), D80's break_tie leaves it and no other symbol holds or is proposed there."""
    starts = _verified_starts(img)
    startset = set(starts)
    by_sym = collections.defaultdict(list)
    for item in results:
        by_sym[item[0]['symbol']].append(item)
    want = collections.defaultdict(list)     # symbol -> items still to place
    for item in results:
        r = item[0]
        if r['status'] == 'short' and item[4] > 0:
            want[r['symbol']].append(item)
    hits_of = {}
    for item in pending:
        r = item[0]
        if id(r) not in done and r['status'] == 'multiple':
            want[r['symbol']].append(item[:5])
            hits_of[id(r)] = set(item[5])
    if not want:
        return []
    target = collections.defaultdict(set)   # symbol -> addresses the aligned callers give it
    reach = collections.defaultdict(set)    # USE_CK_CALLSEQ_DEEP: the same for the not-exact, not-wanted symbols
    for sym, items in by_sym.items():
        # a function this rule credited aligns like a located one (its exact functions' calls are not call-tie
        # evidence before this round)
        if len(items) > 1 and any(it[1].symbol_class(sym) != 2 for it in items):
            continue    # file statics of one name in several objects: the name is not one function
        mine = {int(it[0]['addresses'][0], 16) for it in items if it[0].get('ck_callseq')}
        if any(it[0]['status'] == 'exact' and not it[0].get('ck_callseq') for it in items):
            continue
        at = mine or called.get(sym, set()) or (self_names().get(sym, set()) if USE_CK_CALLSEQ_NAMED else set())
        if not at and located and located.get(sym) is not None:
            at = {located[sym]}
        if not at and vslots and vslots.get(sym) is not None:
            at = {vslots[sym]}      # USE_CK_CALLSEQ_VTBL: a not-exact virtual at its slot address
        if len(at) != 1:
            continue
        a = next(iter(at))
        if a not in startset or claimed[a] - {sym} or not base <= a < base + len(text):
            continue
        i = bisect.bisect_right(starts, a)
        end = starts[i] if i < len(starts) else base + len(text)
        branches = _client_branches(img, a, end, thunk)
        if branches is None:
            continue
        for r, obj, sec, s, length in items:
            if length <= 0:
                continue
            relocs = [x for x in obj.reloc_targets(sec, s, length) if x[2] == 0x14 and x[1] == 4]
            if len(relocs) != len(branches):
                continue
            fwd, back, anchors, ok = {}, {}, 0, True
            for (k, n, typ, sym2, addend), t in zip(relocs, branches):
                if sym2 is None or sym2['name'].startswith('.') or sym2['class'] not in (2, 3):
                    continue    # a section-relative target: no symbol to check
                name = sym2['name']
                if fwd.setdefault(name, t) != t or back.setdefault(t, name) != name:
                    ok = False
                    break
                kn = known.get(name)
                if kn:
                    if t not in kn:
                        ok = False
                        break
                    anchors += 1
            if not ok or not anchors:
                continue
            for name, t in fwd.items():
                if name in want:
                    target[name].add(t)
                elif located is not None and USE_CK_CALLSEQ_DEEP and name in by_sym and                         not any(it[0]['status'] == 'exact' for it in by_sym[name]):
                    reach[name].add(t)
    for name, t in (vslots or {}).items():
        if t is not None and name in want:
            target[name].add(t)     # USE_CK_CALLSEQ_VTBL: a tied / short virtual at its slot address
    for name, ts in reach.items():
        t = next(iter(ts))
        if len(ts) > 1 or located.get(name, t) != t or t not in startset or claimed[t] - {name}:
            located[name] = None    # two addresses for one symbol: located nowhere
        elif name not in located:
            located[name] = t
    props = []
    for name, ts in target.items():
        if len(ts) != 1:
            continue
        t = next(iter(ts))
        if t not in startset or claimed[t] - {name} or not base <= t < base + len(text):
            continue
        if len(by_sym[name]) > 1 and any(it[1].symbol_class(name) != 2 for it in by_sym[name]):
            continue
        for item in want[name]:
            r, obj, sec, s, length = item
            if r['status'] == 'multiple' and t not in hits_of.get(id(r), ()):
                continue
            if not _fullmatch_at(obj, sec, s, length, text, base, t) or \
                    break_tie(obj, sec, s, length, text, base, [t], img, known)[0] != [t]:
                continue
            props.append((item, t))
    by_addr = collections.defaultdict(set)
    for (r, *_), a in props:
        by_addr[a].add(r['symbol'])
    return [(item, a) for item, a in props if len(by_addr[a]) == 1]


def _client_vtable(img, v):
    """True when client address V is a vtable: a `.rdata` address the code stores through `mov dword ptr [reg],
    imm32` (constructors and destructors write a table's start; vtables.py's stored_vptrs) whose first dword
    points into `.text`. Most of the client's classes carry no RTTI, so the store is the mark."""
    if getattr(img, '_ck2v_stored', None) is None:
        t0, t1, raw = img.section('.text')
        r0, r1, _ = img.section('.rdata')
        text = img.data[raw:raw + (t1 - t0)]
        stored = set()
        for m in re.finditer(rb'\xC7[\x00\x01\x02\x03\x06\x07]', text):
            o = m.start() + 2
            if o + 4 <= len(text):
                x = struct.unpack_from('<I', text, o)[0]
                if r0 <= x < r1 and t0 <= img.u32(x) < t1:
                    stored.add(x)
        img._ck2v_stored = stored
    return v in img._ck2v_stored


def _client_vptrs(img, a, end):
    """[vtable address] of the instructions in the client function [a, end) whose last four bytes are a vtable's
    address (a vptr store `mov [r+d], offset vtable`, or a push / mov / lea of it), in address order; None when the
    disassembly stops short of END."""
    out = []
    last = a
    for ins in img.disasm(a, end - a):
        last = ins.address + ins.size
        if ins.size >= 5:
            v = struct.unpack_from('<I', ins.bytes, ins.size - 4)[0]
            if _client_vtable(img, v):
                out.append(v)
        if last >= end:
            break
    return out if last >= end or _padding_img(img, last, end) else None


def _own_slot(vt, slot):
    """True when SLOT (a mangled method) belongs to the class of vtable VT (`??_7<class>@@6B...`), not a base."""
    if '@@6B' not in vt:
        return False
    cls = vt[4:vt.index('@@6B') + 2]
    if slot.startswith(('??_G', '??_E', '??1')):
        return slot[4 if slot[2] == '_' else 3:].startswith(cls)
    return slot.startswith('?') and not slot.startswith('??') and slot.split('@', 1)[1].startswith(cls)


def _client_vtable_slots(img, thunk):
    """{'at': {function address: [(vtable, offset)]}, 'len': {vtable: slots}} of the client's stored vtables
    in `.rdata` (a table's slots: the dwords from it that point into `.text`, up to the next table)."""
    if getattr(img, '_ck2v_vt', None) is None:
        lo, hi, raw = img.section('.rdata')
        tlo, thi, _ = img.section('.text')
        _client_vtable(img, 0)
        vts = sorted(img._ck2v_stored)
        nxt = dict(zip(vts, vts[1:] + [hi]))
        at, ln = collections.defaultdict(list), {}
        for V in vts:
            k = 0
            while V + 4 * k < nxt[V]:
                f = struct.unpack_from('<I', img.data, img.offset(V + 4 * k))[0]
                if not tlo <= f < thi:
                    break
                at[thunk.get(f, f)].append((V, 4 * k))
                k += 1
            ln[V] = k
        img._ck2v_vt = {'at': at, 'len': ln}
    return img._ck2v_vt


def callseq_location_vtbl(img, results, claimed, called, text, base, thunk, known, located=None):
    """D246 (CK2V): {slot symbol: client address, or None when two tables disagree} from the vtables whose client
    address the run knows: one address in D80's `known` (an exact function's vptr store), or the one address the
    vptr stores of located not-exact functions give it. A not-exact function is located as callseq_ties locates a
    caller (the exact functions' calls, D88's self name, a deep location); its object's `??_7` DIR32 relocations
    (offset order) align one to one with the client function's vtable immediates (_client_vptrs) when the counts
    agree, one table to one address and back, and every table D80 knows is at its known address. A table's slot i
    then gives its slot symbol the client table's slot i (thunks resolved, a `.text` address) unless D80's
    vtable_check finds a known slot elsewhere in the table, symbols.json names another function there, or the
    symbol is known at another address."""
    starts = _verified_starts(img)
    startset = set(starts)
    by_sym = collections.defaultdict(list)
    for item in results:
        by_sym[item[0]['symbol']].append(item)
    tables = collections.defaultdict(set)      # vtable symbol -> client addresses
    for name, v in known.items():
        if name.startswith('??_7'):
            tables[name] |= set(v)
    for sym, items in by_sym.items():
        if any(it[0]['status'] == 'exact' for it in items):
            continue    # an exact function's vptr stores are in `known` already
        if len(items) > 1 and any(it[1].symbol_class(sym) != 2 for it in items):
            continue
        at = called.get(sym, set()) or (self_names().get(sym, set()) if USE_CK_CALLSEQ_NAMED else set())
        if not at and located and located.get(sym) is not None:
            at = {located[sym]}
        if len(at) != 1:
            continue
        a = next(iter(at))
        if a not in startset or claimed[a] - {sym} or not base <= a < base + len(text):
            continue
        i = bisect.bisect_right(starts, a)
        end = starts[i] if i < len(starts) else base + len(text)
        vps = None
        for r, obj, sec, s, length in items:
            if length <= 0:
                continue
            relocs = [x[3]['name'] for x in obj.reloc_targets(sec, s, length)
                      if x[2] == 0x06 and x[1] == 4 and x[3] is not None and x[3]['name'].startswith('??_7')]
            if not relocs:
                continue
            if vps is None:
                vps = _client_vptrs(img, a, end) or []
            if len(relocs) != len(vps):
                continue
            fwd, back = {}, {}
            if all(fwd.setdefault(n, v) == v and back.setdefault(v, n) == n for n, v in zip(relocs, vps)) and \
                    all(not known.get(n) or v in known[n] for n, v in fwd.items()):
                for n, v in fwd.items():
                    tables[n].add(v)
    defs = collections.defaultdict(list)       # vtable symbol -> [(object, its symbol, [(offset, slot)])]
    for obj in {id(it[1]): it[1] for it in results}.values():
        weak = {x['name'] for x in obj.symbols if x['class'] == 105}
        for vt, sl in vtable_slots(obj):
            if vt in tables or USE_CK_CALLSEQ_VTBL_SLOTS:
                sym = next(x for x in obj.symbols if x['name'] == vt and x['section'] > 0)
                # a weak `??_E` (the vector deleting destructor the class never defines) is its `??_G`
                sl = [(off, '??_G' + n[4:] if n.startswith('??_E') and n in weak else n) for off, n in sl]
                defs[vt].append((obj, sym, sl))
    if USE_CK_CALLSEQ_VTBL_SLOTS:
        held = _client_vtable_slots(img, thunk)
        for vt, d in defs.items():
            if tables.get(vt) or len({tuple(sl) for _, _, sl in d}) != 1 or not d[0][2]:
                continue
            sl = d[0][2]
            n = max(off for off, _ in sl) // 4 + 1
            cands, own = None, False
            for off, slot in sl:
                kn = known.get(slot)
                if not kn or len(kn) != 1 or generated(slot):
                    continue
                v = next(iter(kn))
                at = {V for V, o in held['at'].get(v, ()) if o == off and held['len'][V] == n}
                cands = at if cands is None else cands & at
                own = own or _own_slot(vt, slot)
            if cands and len(cands) == 1 and own:
                tables[vt].add(next(iter(cands)))
    names = self_names()
    owners = collections.defaultdict(set)
    for k, v in names.items():
        for x in v:
            owners[x].add(k[4:] if k.startswith(('??_E', '??_G')) else k)
    out = collections.defaultdict(set)
    vt_at = collections.Counter(next(iter(tables[vt])) for vt in defs if len(tables[vt]) == 1)
    for vt, d in defs.items():
        if len(tables[vt]) != 1 or len({tuple(sl) for _, _, sl in d}) != 1 or vt_at[next(iter(tables[vt]))] > 1:
            continue    # one table to one client address and back
        va = next(iter(tables[vt]))
        if any(vtable_check(obj, sym, va, known, img) is False for obj, sym, _ in d):
            continue
        for off, slot in d[0][2]:
            o = img.offset(va + off)
            if o is None or generated(slot) or not slot.startswith('?'):
                continue
            v = struct.unpack_from('<I', img.data, o)[0]
            v = thunk.get(v, v)
            n = slot[4:] if slot.startswith(('??_E', '??_G')) else slot
            if not base <= v < base + len(text) or (owners.get(v) and n not in owners[v]) or \
                    (known.get(slot) and v not in known[slot]):
                continue
            out[slot].add(v)
    return {k: (next(iter(v)) if len(v) == 1 else None) for k, v in out.items()}


def function_start(img, va):
    """D189(2): True if a searched hit at `va` may be a function's start: a thunk's or a call / jump's
    target, the start of .text, or the byte after padding (int3 / nop) or after the end of another
    function (`ret`, `ret n`, `jmp rel32` / `rel8`, an indirect `jmp`, or a jump table's last entry, a
    .text address), or a 16-byte aligned address (a function after a switch's byte index table,
    Hue_Shift 0x94EB20). Never an unaligned hit inside a function."""
    start, end, raw = img.section('.text')
    if va == start:
        return True
    sites, thunks = call_sites(img)
    if va in sites or va in _thunk_targets(img):
        return True
    o = img.offset(va)
    d = img.data
    if o is None or o < 7:
        return False
    if d[o - 1] in (0xCC, 0x90, 0xC3) or d[o - 3] == 0xC2 or d[o - 5] == 0xE9 or d[o - 2] == 0xEB:
        return True
    # an indirect jmp (FF /4): jmp reg, jmp [reg], jmp [reg+disp8], jmp [disp32] / [reg+disp32]
    if d[o - 2] == 0xFF and (0xE0 <= d[o - 1] <= 0xE7 or 0x20 <= d[o - 1] <= 0x27 and d[o - 1] not in (0x24, 0x25)):
        return True
    if d[o - 3] == 0xFF and 0x60 <= d[o - 2] <= 0x67 and d[o - 2] != 0x64:
        return True
    if d[o - 6] == 0xFF and (d[o - 5] == 0x25 or 0xA0 <= d[o - 5] <= 0xA7 and d[o - 5] != 0xA4):
        return True
    # after a jump table (its last entry a .text address), or 16-byte aligned (after a switch's index bytes)
    return start <= struct.unpack_from('<I', d, o - 4)[0] < end or va % 16 == 0


_targets = {}


def _thunk_targets(img):
    if id(img) not in _targets:
        _targets[id(img)] = set(thunks_of(img).values())
    return _targets[id(img)]


def body_sig(obj, sec, s, length):
    """A function's bytes and relocations (offset, size, type, symbol name, addend): equal for copies
    the linker may fold into one client function (D174, D193)."""
    info = obj.sections[sec]
    return (bytes(obj.data[info['raw'] + s:info['raw'] + s + length]),
            tuple((k, n, typ, sym and sym['name'], bytes(add)) for k, n, typ, sym, add in obj.reloc_targets(sec, s, length)))


def short_known(img, shorts, text, base, known, thunk, called, vptrs, claimed, folded_at=None,
                funclet_called=None, pushed_called=None):
    """D188: [(item, address, kind)] for the `short` functions at an address the run knows independently
    of their bytes: a call's target in an exact function (`call`), or a slot of a vtable whose address is
    known (`vptr` when an exact function's vptr store gave it, `vtable` otherwise: symbols.json, D171).
    The pattern must fullmatch there, D80's checks must leave it, the proofs must name one address, and
    neither another symbol already exact there nor another short function may take it.
    D193: except a linker-folded COMDAT: when every claimant of the address (this round's proposals and
    the symbols an earlier short round credited there, `folded_at` {address: {body_sig: {symbols}}}) is
    a COMDAT body with the same bytes and relocations, all are credited.
    D246 / C21: a rel32 jump from an unwind funclet of an exact function, where the funclet matches the
    object's funclet for the same unwind state (`funclet_called`, funclet_targets), is call evidence
    (`funclet`).
    D246 / CR3: a function's address held as an absolute immediate by an exact function (`pushed_called`,
    pushed_targets) is evidence too (`pushed`, below call and funclet)."""
    folded_at = folded_at if folded_at is not None else {}
    objs = list({id(x[1]): x[1] for x in shorts}.values())
    slot_of = collections.defaultdict(lambda: collections.defaultdict(set))   # symbol -> address -> kinds
    names = self_names()
    owners = collections.defaultdict(set)
    for k, v in names.items():
        for a in v:
            owners[a].add(k[4:] if k.startswith(('??_E', '??_G')) else k)
    for obj in objs:
        for vt, sl in vtable_slots(obj):
            addrs = known.get(vt)
            if not addrs or len(addrs) != 1:
                continue
            va = next(iter(addrs))
            kind = 'vptr' if va in vptrs.get(vt, ()) else 'vtable'
            for off, slot in sl:
                o = img.offset(va + off)
                if o is None or generated(slot):
                    continue
                v = struct.unpack_from('<I', img.data, o)[0]
                v = thunk.get(v, v)
                n = slot[4:] if slot.startswith(('??_E', '??_G')) else slot
                if owners.get(v) and n not in owners[v]:
                    continue        # symbols.json names another function there
                if known.get(slot) and v not in known[slot]:
                    continue        # the slot's symbol is known elsewhere: our table differs from the client's
                    # here (ChatPanelInterface's +0x64 is its own empty 0x563260, ours inherits 0x5156B0)
                slot_of[slot][v].add(kind)
    for name, addrs in called.items():
        for a in addrs:
            slot_of[name][a].add('call')
    for name, addrs in (funclet_called or {}).items():
        for a in addrs:
            slot_of[name][a].add('funclet')
    for name, addrs in (pushed_called or {}).items():
        for a in addrs:
            slot_of[name][a].add('pushed')
    props = []
    for item in shorts:
        r, obj, sec, s, length = item
        if obj.symbol_class(r['symbol']) != 2 or generated(r['symbol']):
            continue
        cands = slot_of.get(r['symbol'])
        if not cands or len(cands) != 1:
            continue
        a, kinds = next(iter(cands.items()))
        o = a - base
        if not 0 <= o <= len(text) - length:
            continue
        others = claimed[a] - {r['symbol']}
        if others:
            # D193: only symbols an earlier short round credited there as the same folded COMDAT body
            earlier = folded_at.get(a, {})
            if obj.own(sec) or len(earlier) != 1 or not others <= next(iter(earlier.values())) or \
                    body_sig(obj, sec, s, length) not in earlier:
                continue
        pat = obj.pattern(sec, s, s + length)[0]
        if not re.fullmatch(pat, text[o:o + length], re.S):
            continue
        survivors, _ = break_tie(obj, sec, s, length, text, base, [a], img, known)
        if survivors != [a]:
            continue
        kind = 'call' if 'call' in kinds else 'funclet' if 'funclet' in kinds else             'pushed' if 'pushed' in kinds else 'vptr' if 'vptr' in kinds else 'vtable'
        props.append((item, a, kind))
    by_addr = collections.defaultdict(set)
    bodies = collections.defaultdict(set)       # D193: the COMDAT bodies proposed at each address
    for (r, obj, sec, s, length), a, _ in props:
        by_addr[a].add(r['symbol'])
        bodies[a].add(body_sig(obj, sec, s, length) if not obj.own(sec) else ('own', id(r)))
    for a, v in folded_at.items():
        if a in bodies:
            bodies[a] |= set(v)
    return [p for p in props if len(by_addr[p[1]]) == 1 or len(bodies[p[1]]) == 1 and
            all(b[0] != 'own' for b in bodies[p[1]])]


def range_distance(rng, a):
    """How far address a lies outside the range (lo, hi); 0 inside it."""
    return 0 if rng[0] <= a <= rng[1] else min(abs(a - rng[0]), abs(a - rng[1]))


def shared_pairs(results):
    """D183: the (address, symbol) pairs that two or more objects are exact at."""
    by = collections.defaultdict(set)
    for r, obj, *_ in results:
        if r['status'] == 'exact':
            by[(int(r['addresses'][0], 16), r['symbol'])].add(id(obj))
    return {k for k, v in by.items() if len(v) > 1}


_call_sites = {}
_call_lock = threading.Lock()


def call_sites(img):
    """({target VA: [site VA]} of every E8 / E9 rel32 in .text, the incremental thunk table). A byte
    that only looks like E8 adds a site, which can only make D166 more careful."""
    with _call_lock:
        if id(img) not in _call_sites:
            start, end, raw = img.section('.text')
            body = img.data[raw:raw + (end - start)]
            sites = collections.defaultdict(list)
            for m in re.finditer(rb'[\xE8\xE9]', body):
                o = m.start()
                if o + 5 > len(body):
                    break
                dst = (start + o + 5 + struct.unpack_from('<i', body, o + 1)[0]) & 0xFFFFFFFF
                sites[dst].append(start + o)
            _call_sites[id(img)] = (sites, thunks_of(img))
        return _call_sites[id(img)]


_funclets = {}
FUNCLET_MAX = 0x40   # an unwind funclet is a few instructions (mov ecx, [ebp-x]; jmp dtor)


def funclet_parents(img):
    """D174 (P-TK3-2): ([funclet start VA], {funclet start: (end VA, {parent's push-site VA})}).
    A function with C++ unwinding pushes its handler stub (`push offset __ehhandler$F`; the stub is
    `mov eax, offset FuncInfo; jmp ___CxxFrameHandler`), and FuncInfo (magic 0x19930520, maxState,
    pUnwindMap, ...) lists the unwind funclets as {toState, action} entries. The funclets live apart
    from their parent (client.exe 0xAA8000-0xACD000); a funclet ends at the next funclet or stub, or
    FUNCLET_MAX bytes on. A funclet the linker shares between functions has every parent's push site."""
    with _call_lock:
        if id(img) in _funclets:
            return _funclets[id(img)]
        start, end, raw = img.section('.text')
        body = img.data[raw:raw + (end - start)]
        data = img.data
        stubs = {}
        for m in re.finditer(rb'\xB8(....)\xE9', body, re.S):
            fi = struct.unpack_from('<I', body, m.start() + 1)[0]
            off = img.offset(fi)
            if off is None or off + 12 > len(data) or struct.unpack_from('<I', data, off)[0] != 0x19930520:
                continue
            stubs[start + m.start()] = off
        pushes = collections.defaultdict(set)
        for m in re.finditer(rb'(?=\x68(....))', body, re.S):
            v = struct.unpack_from('<I', body, m.start() + 1)[0]
            if v in stubs:
                pushes[v].add(start + m.start())
        parents = collections.defaultdict(set)
        for stub, off in stubs.items():
            if not pushes[stub]:
                continue
            nstate, pmap = struct.unpack_from('<iI', data, off + 4)
            moff = img.offset(pmap)
            if moff is None or not 0 < nstate < 0x1000:
                continue
            for i in range(nstate):
                if moff + 8 * i + 8 > len(data):
                    break
                act = struct.unpack_from('<I', data, moff + 8 * i + 4)[0]
                if start <= act < end:
                    parents[act] |= pushes[stub]
        bounds = sorted(set(parents) | set(stubs))
        out = {}
        for f in parents:
            i = bisect.bisect_right(bounds, f)
            nxt = bounds[i] if i < len(bounds) else end
            out[f] = (min(nxt, f + FUNCLET_MAX), parents[f])
        _funclets[id(img)] = (sorted(out), out)
        return _funclets[id(img)]


_eh_stubs = {}
EH_MAGIC = 0x19930520


def eh_stubs(img):
    """{EH handler stub VA: file offset of its FuncInfo} of client.exe (`mov eax, offset FuncInfo; jmp`)."""
    with _call_lock:
        if id(img) not in _eh_stubs:
            start, end, raw = img.section('.text')
            body = img.data[raw:raw + (end - start)]
            stubs = {}
            for m in re.finditer(rb'\xB8(....)\xE9', body, re.S):
                off = img.offset(struct.unpack_from('<I', body, m.start() + 1)[0])
                if off is not None and off + 12 <= len(img.data) and \
                        struct.unpack_from('<I', img.data, off)[0] == EH_MAGIC:
                    stubs[start + m.start()] = off
            _eh_stubs[id(img)] = (stubs, body, start)
        return _eh_stubs[id(img)][0]


def _whole_text(img):
    """(.text bytes, its VA): the whole section, whatever region match_objects searches."""
    eh_stubs(img)
    return _eh_stubs[id(img)][1:]


def object_unwind(obj, sec, s, length):
    """(stub reference offset in the function, .text$x section, [(start, end) or None by state]) of an
    object function with an EH frame (it references a `$L` label in .text$x whose code is the handler
    stub `mov eax, offset FuncInfo; jmp ___CxxFrameHandler`), or None. The funclet of a state is its
    unwind map action's label up to the next label of the section (FD, run 2026-11-15)."""
    byidx = None
    for k, n, typ, sym, add in obj.reloc_targets(sec, s, length):
        if n != 4 or sym is None or sym['section'] <= 0 or not sym['name'].startswith('$L'):
            continue
        xsec = sym['section'] - 1
        if obj.sections[xsec]['name'] != '.text$x':
            continue
        fi = [x for x in obj.reloc_targets(xsec, sym['value'], 10) if x[0] == 1]
        if not fi or fi[0][3] is None or fi[0][3]['section'] <= 0:
            continue
        fsym = fi[0][3]
        info = obj.sections[fsym['section'] - 1]
        foff = fsym['value'] + struct.unpack('<I', fi[0][4])[0]
        if foff + 12 > info['size']:
            continue
        magic, nstate = struct.unpack_from('<Ii', obj.data, info['raw'] + foff)
        if magic != EH_MAGIC or not 0 < nstate < 0x1000:
            continue
        if byidx is None:
            byidx = {x['index']: x for x in obj.symbols}
        mapsym = next((byidx.get(si) for va, si, t in info['relocs'] if va == foff + 8), None)
        if mapsym is None or mapsym['section'] <= 0:
            continue
        msec = obj.sections[mapsym['section'] - 1]
        mbase = mapsym['value'] + struct.unpack_from('<I', obj.data, info['raw'] + foff + 8)[0]
        relmap = {va: byidx.get(si) for va, si, t in msec['relocs']}
        xinfo = obj.sections[xsec]
        bounds = sorted({x['value'] for x in obj.symbols if x['section'] == xsec + 1} | {xinfo['size']})
        out = []
        for i in range(nstate):
            lab = relmap.get(mbase + 8 * i + 4)
            if lab is None or lab['section'] != xsec + 1:
                out.append(None)
                continue
            j = bisect.bisect_right(bounds, lab['value'])
            out.append((lab['value'], bounds[j] if j < len(bounds) else xinfo['size']))
        return k, xsec, out
    return None


def funclet_folded(proposals, claimed):
    """CR2: {address: {symbols}} of the funclet round's proposals that a linker fold explains (D193 for
    non-short bodies): two or more symbols at one address, every proposal there a COMDAT (not the object's
    own code) with one body_sig, and nothing else already exact there. USE_FUNCLET_FOLDED switches it."""
    if not USE_FUNCLET_FOLDED:
        return {}
    at = collections.defaultdict(list)
    for (r, obj, sec, s, length, hits), a in proposals:
        at[a].append((r['symbol'], None if obj.own(sec) else body_sig(obj, sec, s, length)))
    out = {}
    for a, props in at.items():
        syms = {x[0] for x in props}
        sigs = {x[1] for x in props}
        if len(syms) > 1 and len(sigs) == 1 and None not in sigs and not claimed[a] - syms:
            out[a] = syms
    return out


_funcinfos = {}
EH_PROLOGUE = re.compile(rb'\x6A\xFF\x68(....)\x64\xA1\x00\x00\x00\x00', re.S)


def client_funcinfos(img):
    """CR2: ({stub VA: [funclet VA by state]}, {state count: [stub VA]}) of client.exe's C++ EH FuncInfos."""
    stubs = eh_stubs(img)                    # takes _call_lock itself (not reentrant)
    with _call_lock:
        if id(img) not in _funcinfos:
            maps, by_n = {}, collections.defaultdict(list)
            for stub, off in stubs.items():
                nstate, pmap = struct.unpack_from('<iI', img.data, off + 4)
                moff = img.offset(pmap)
                if moff is None or not 0 < nstate < 0x1000 or moff + 8 * nstate > len(img.data):
                    continue
                maps[stub] = [struct.unpack_from('<I', img.data, moff + 8 * i + 4)[0] for i in range(nstate)]
                by_n[nstate].append(stub)
            _funcinfos[id(img)] = (maps, by_n)
        return _funcinfos[id(img)]


def _state_pattern(obj, xsec, st):
    """(compiled pattern, length, fixed prefix) of an object funclet (start, end)."""
    pat, _, flen = obj.pattern(xsec, st[0], st[1])
    info = obj.sections[xsec]
    first = min([va - st[0] for va, _, typ in info['relocs'] if st[0] <= va < st[1]] + [flen])
    return re.compile(pat, re.S), flen, bytes(obj.data[info['raw'] + st[0]:info['raw'] + st[0] + first])


def _state_match(sp, cf, text, base):
    co = cf - base
    rx, flen, prefix = sp
    return 0 <= co and co + flen <= len(text) and text.startswith(prefix, co) and \
        rx.fullmatch(text, co, co + flen) is not None


_eh_pushes = {}


def eh_pushes(img):
    """CR5: {EH handler stub VA: [VA of each `push offset stub`]} of client.exe."""
    maps, _ = client_funcinfos(img)          # takes _call_lock itself (not reentrant)
    text, base = _whole_text(img)
    with _call_lock:
        if id(img) not in _eh_pushes:
            out = collections.defaultdict(list)
            for m in re.finditer(rb'(?=\x68(....))', text, re.S):
                v = struct.unpack_from('<I', text, m.start() + 1)[0]
                if v in maps:
                    out[v].append(base + m.start())
            _eh_pushes[id(img)] = out
        return _eh_pushes[id(img)]


def notexact_funclet_targets(img, obj, sec, s, length, symbol, thunk, held, rng=None, calls=()):
    """CR2: [(symbol, client address)] the matching unwind funclets of a function that is not exact reach (the
    same pairs as funclet_targets), its client FuncInfo located by client.exe's bytes alone: (1) the one address
    symbols.json names with its symbol from a proof other than an exact match (self_names), whose EH prologue
    (`push -1; push offset stub; mov eax, fs:[0]`) pushes the stub; else (2) the one client FuncInfo with the
    object's state count whose every state's funclet matches the object's, its stub pushed by no function an
    exact symbol (`held`: their addresses) starts within 0x10 bytes before; (3, CR5, USE_FUNCLET_PLACED) of several
    such FuncInfos, the one whose push site lies strictly between `rng` (lo, hi: the client addresses of the
    exact own functions just before and after the parent in object order), else the one whose parent the
    object's exact functions call under its symbol (`calls`: those call targets, one within 0x10 bytes before
    the push). Only matching states count."""
    ou = object_unwind(obj, sec, s, length)
    if ou is None:
        return []
    k, xsec, states = ou
    text, base = _whole_text(img)
    maps, by_n = client_funcinfos(img)
    stub = None
    named = {thunk.get(a, a) for a in self_names().get(symbol, ())}
    if len(named) == 1:
        o = next(iter(named)) - base
        m = EH_PROLOGUE.search(text, max(o, 0), max(o, 0) + 0x18) if 0 <= o < len(text) else None
        if m:
            v = struct.unpack_from('<I', text, m.start() + 3)[0]
            stub = v if v in maps and len(maps[v]) == len(states) else None
    sps = [None if st is None else _state_pattern(obj, xsec, st) for st in states]
    if stub is None:
        if all(st is None for st in states):
            return []
        good = [c for c in by_n.get(len(states), ())
                if all(sp is None or _state_match(sp, maps[c][i], text, base) for i, sp in enumerate(sps))]
        if len(good) > 1 and USE_FUNCLET_PLACED:
            pushes = eh_pushes(img)
            free = [c for c in good if len(pushes.get(c, ())) == 1 and
                    not any(pushes[c][0] - d in held for d in range(0x11))]
            placed = [c for c in free if rng and rng[0] < pushes[c][0] < rng[1]]
            if len(placed) != 1:
                placed = [c for c in free if any(pushes[c][0] - d in calls for d in range(0x11))]
            good = placed if len(placed) == 1 else good
        if len(good) != 1:
            return []
        stub = good[0]
        push = text.find(b'\x68' + struct.pack('<I', stub))
        if push < 0 or text.find(b'\x68' + struct.pack('<I', stub), push + 1) >= 0 or \
                any(base + push - d in held for d in range(0x11)):
            return []
    out = []
    for i, st in enumerate(states):
        cf = maps[stub][i]
        if st is None or not _state_match(sps[i], cf, text, base):
            continue
        co = cf - base
        for j, n, typ, sym, add in obj.reloc_targets(xsec, st[0], st[1] - st[0]):
            if typ != 0x14 or n != 4 or sym is None or sym['class'] != 2 or generated(sym['name']):
                continue
            t = (cf + j + 4 + struct.unpack_from('<i', text, co + j)[0]) & 0xFFFFFFFF
            out.append((sym['name'], thunk.get(t, t)))
    return out


_funclet_owners = None


def funclet_reach(funclet_called):
    """CR2: funclet_called {symbol: {addresses}} without the addresses symbols.json names with another
    function from a proof other than an exact match (self_names: a vtable slot, a string, a named row;
    USE_FUNCLET_NAMED): a matching funclet's wildcarded target is the client's function,
    whatever our object's symbol there is, so a reach the client names otherwise is our source's type,
    not evidence that the symbol is there. Only to choose among several reaches: a single reach stays, since
    a linker-folded COMDAT is named with one of its symbols only (aux_roots' ~DeltaTemplateClass copies at
    InvSlotStruct's)."""
    global _funclet_owners
    if not USE_FUNCLET_NAMED:
        return funclet_called
    if _funclet_owners is None:
        _funclet_owners = collections.defaultdict(set)
        for k, v in self_names().items():
            for a in v:
                _funclet_owners[a].add(k[4:] if k.startswith(('??_E', '??_G')) else k)
    owners = _funclet_owners
    out = {}
    for name, addrs in funclet_called.items():
        n = name[4:] if name.startswith(('??_E', '??_G')) else name
        keep = {a for a in addrs if not owners.get(a) or n in owners[a]} if len(addrs) > 1 else addrs
        out[name] = keep or addrs
    return out


def funclet_targets(img, obj, sec, s, length, addr, thunk):
    """D246 / C21: [(symbol, client address)] the unwind funclets of an exact function reach: for each
    unwind state whose funclet in the object matches the client's funclet of the same state (the client
    FuncInfo is the one its stub, at the object's stub reference, names), each rel32 call / jmp in it
    pairs the object's target symbol with the client's target (incremental thunks resolved). The
    funclets lie outside the app region (0xAA8000-0xACD000), so this reads the whole .text."""
    ou = object_unwind(obj, sec, s, length)
    if ou is None:
        return []
    text, base = _whole_text(img)
    k, xsec, states = ou
    o = addr - base + k
    if not 0 <= o <= len(text) - 4:
        return []
    off = eh_stubs(img).get(struct.unpack_from('<I', text, o)[0])
    if off is None:
        return []
    nstate, pmap = struct.unpack_from('<iI', img.data, off + 4)
    moff = img.offset(pmap)
    if moff is None or nstate != len(states) or moff + 8 * nstate > len(img.data):
        return []
    out = []
    for i, st in enumerate(states):
        cf = struct.unpack_from('<I', img.data, moff + 8 * i + 4)[0]
        if st is None or not 0 <= cf - base < len(text):
            continue
        ls, le = st
        pat, _, flen = obj.pattern(xsec, ls, le)
        co = cf - base
        if co + flen > len(text) or not re.fullmatch(pat, text[co:co + flen], re.S):
            continue
        for j, n, typ, sym, add in obj.reloc_targets(xsec, ls, le - ls):
            if typ != 0x14 or n != 4 or sym is None or sym['class'] != 2 or generated(sym['name']):
                continue
            t = (cf + j + 4 + struct.unpack_from('<i', text, co + j)[0]) & 0xFFFFFFFF
            out.append((sym['name'], thunk.get(t, t)))
    return out


def pushed_form(body, k):
    """D246 / CR3: the instruction form of an absolute (DIR32) field at offset k of a function's bytes:
    `push` (68 imm32), `mov` (B8+r imm32: mov reg, offset f, then pushed or stored), `lea` (8D /r disp32),
    `store` (C7 /0 ... imm32: mov [mem], offset f) or `other`."""
    if k >= 1 and body[k - 1] == 0x68:
        return 'push'
    if k >= 1 and 0xB8 <= body[k - 1] <= 0xBF:
        return 'mov'
    if k >= 2 and body[k - 2] == 0x8D:
        return 'lea'
    if any(k >= j and body[k - j] == 0xC7 for j in (6, 7, 3, 4, 2)):
        return 'store'
    return 'other'


def pushed_targets(img, obj, sec, s, length, addr, text, base, thunk):
    """D246 / CR3: [(symbol, client address, form)] the absolute function references of an exact function
    reach: each DIR32 relocation whose symbol is an external function (COFF type 0x20; not a vtable, not a
    `_$E` name) pairs the object's symbol with the client's immediate at the same offset, incremental thunks
    resolved, when that lies in client.exe's .text: a function pushed as an argument (an eh vector iterator's
    element ctor / dtor, an atexit destructor, an ostream manipulator), loaded into a register or stored."""
    tlo, thi, _ = img.section('.text')
    info = obj.sections[sec]
    body = obj.data[info['raw'] + s:info['raw'] + s + length]
    o = addr - base
    out = []
    for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
        if typ != 0x06 or n != 4 or sym is None or sym['class'] != 2 or (sym['type'] >> 4) != 2 \
                or generated(sym['name']) or sym['name'].startswith('??_7'):
            continue
        if not 0 <= o + k <= len(text) - 4:
            continue
        v = (struct.unpack_from('<I', text, o + k)[0] - struct.unpack('<I', addend)[0]) & 0xFFFFFFFF
        v = thunk.get(v, v)
        if tlo <= v < thi:
            out.append((sym['name'], v, pushed_form(body, k)))
    return out


def folded(items):
    """D174 (P-TK5-1): True if the tied items are functions of one object with the same bytes and the
    same relocations (symbols and addends): copies the linker may fold into one client function."""
    r0, obj0, sec0, s0, len0, _ = items[0]

    def sig(obj, sec, s, length):
        info = obj.sections[sec]
        return (bytes(obj.data[info['raw'] + s:info['raw'] + s + length]),
                [(k, n, typ, sym and sym['name'], bytes(add)) for k, n, typ, sym, add in obj.reloc_targets(sec, s, length)])
    first = sig(obj0, sec0, s0, len0)
    return all(obj is obj0 and length == len0 and sig(obj, sec, s, length) == first
               for r, obj, sec, s, length, _ in items[1:])


def caller_tiebreak(img, results, pending, claimed, text, base, thunk, known=None):
    """D166: a function found at several client addresses is the one our own callers reach. Our
    caller C (a function of the same object, exact at A) has a relocated call to X's symbol; the
    call at A reaches one of X's addresses. X takes that address only when every client call or
    jump to it comes from a function the same object is exact at (or from X itself) and nothing
    holds the address as data (a vtable slot, a pushed pointer): the linker's merged copy has callers
    in every file and stays tied. No other symbol may be exact there or propose the same address.
    Rounds repeat until nothing changes (an address credited makes its own callees resolvable); a
    round's proposals are applied together, so the result does not depend on the order of the
    objects. Returns the pending items it credited.
    D174: the copies need not be byte-identical (P-TK3-1: they may differ in a relocation's target,
    e.g. an EH handler's address; the callers decide); the candidates are D80's survivors (those no
    relocation target contradicts, P-TK2-1(a)); a call from an EH unwind funclet counts as a call by
    its parent function(s) (P-TK3-2, `funclet_parents`); and when several symbols of one object with
    the same bytes and relocations propose one address (the linker folded them: lookup_table's four
    Create_NEW<...> at 0x4EE9F0), the first by name takes it (P-TK5-1, `folded` set).
    D194: when every proposer is a COMDAT and all share one `body_sig` (any objects), all are credited."""
    todo = []
    for it in pending:
        r, obj, sec, s, length, hits = it
        if r['status'] != 'multiple':
            continue
        survivors, _ = break_tie(obj, sec, s, length, text, base, hits, img, known)
        todo.append((it, set(survivors)))
    if not todo:
        return []
    todo.sort(key=lambda t: (t[0][0]['object'], t[0][0]['symbol']))
    sites, thunks = call_sites(img)
    fstarts, funclets = funclet_parents(img)
    alias = collections.defaultdict(set)
    for th, dst in thunks.items():
        alias[dst].add(th)
    data = img.data

    def inside(site, order, funcs):
        i = bisect.bisect_right(order, site) - 1
        return i >= 0 and site < order[i] + funcs[order[i]]

    def refs_ok(addr, xlen, funcs):
        """funcs: {start: length} of the functions the object is exact at."""
        names = {addr} | alias.get(addr, set())
        if any(data.find(struct.pack('<I', a)) >= 0 for a in names):
            return False
        order = sorted(funcs)
        n = 0
        for a in names:
            for site in sites.get(a, ()):
                if site in thunks:
                    continue                      # the thunk's own jump
                n += 1
                if addr <= site < addr + xlen:
                    continue                      # X calling itself
                if inside(site, order, funcs):
                    continue
                j = bisect.bisect_right(fstarts, site) - 1
                if j >= 0 and site < funclets[fstarts[j]][0] and \
                        all(inside(p, order, funcs) for p in funclets[fstarts[j]][1]):
                    continue                      # an unwind funclet of the object's exact functions
                return False
        return n > 0

    credited = []
    changed = True
    while changed:
        changed = False
        proposals = []
        shared = shared_pairs(results)   # D183: another object holds it too: labelled when it decides
        for item, hits in todo:
            r, obj, sec, s, length, _ = item
            if r['status'] != 'multiple':
                continue
            funcs, callers, unshared = {}, [], {}
            for r2, obj2, sec2, s2, len2 in results:
                if obj2 is obj and r2['status'] == 'exact':
                    a2 = int(r2['addresses'][0], 16)
                    funcs[a2] = max(funcs.get(a2, 0), len2)
                    if (a2, r2['symbol']) not in shared:
                        unshared[a2] = max(unshared.get(a2, 0), len2)
                    if r2['symbol'] != r['symbol']:
                        callers.append((sec2, s2, len2, a2))
            cands, bad, via = set(), False, set()
            for sec2, s2, len2, a2 in callers:
                for k, n, typ, sym, addend in obj.reloc_targets(sec2, s2, len2):
                    if sym is None or sym['name'] != r['symbol']:
                        continue
                    if n != 4 or typ != 0x14:
                        bad = True
                        continue
                    o = a2 - base
                    v = (a2 + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]) & 0xFFFFFFFF
                    cands.add(thunk.get(v, v))
                    via.add(a2)
            if bad or len(cands) != 1:
                continue
            a = next(iter(cands))
            if a not in hits or claimed[a] - {r['symbol']} or not refs_ok(a, length, funcs):
                continue
            # D183: a decision that needed a shared function (every caller shared, or a call site that only
            # a shared function holds) is labelled
            r['shared_callers'] = not (via & set(unshared)) or not refs_ok(a, length, unshared)
            proposals.append((item, a))
        by_addr = collections.defaultdict(list)
        for item, a in proposals:
            by_addr[a].append(item)
        for a, items in by_addr.items():
            if len({it[0]['symbol'] for it in items}) > 1:
                # D194: COMDAT bodies with one body_sig (byte-identical, linker-folded, as D193's
                # short_known): every proposer is credited
                if all(not it[1].own(it[2]) for it in items) and \
                        len({body_sig(*it[1:5]) for it in items}) == 1:
                    for it in items:
                        it[0]['folded'] = True
                elif not folded(items):
                    continue
                else:
                    items = [min(items, key=lambda it: it[0]['symbol'])]
                    items[0][0]['folded'] = True
            for item in items:
                r = item[0]
                if not r.get('shared_callers'):
                    r.pop('shared_callers', None)
                r['status'] = 'exact'
                r['addresses'] = [f'0x{a:06X}']
                r['caller_tiebreak'] = True
                r.pop('count', None)
                r.pop('identical', None)
                claimed[a].add(r['symbol'])
                credited.append(item)
                changed = True
    return credited


def initialiser_targets(results, pending, ranges, text, base, known, thunk, located=None):
    """D115: {symbol: {addresses}} of the functions the objects' rebuilt static initialisers call or
    register. A `_$E` initialiser is usually too short to match (`short`), so it is located by its
    links instead: a pattern hit inside the object's range (widened by RANGE_GAP) where every relocated
    field to a symbol already known points at that symbol's address (atexit, a global), no field points
    at an address known under another name, at least one field is confirmed that way, and the `_$E`
    atexit destructor it pushes matches its own pattern at the pushed address under the same rules (the
    static's address equal in both). An initialiser with exactly one such hit (or an exact one) gives
    its and its destructor's other code targets: the constructor it calls, the destructor the atexit
    function jumps to. A target that is a copy of another of the object's functions and not of the
    symbol's own pattern contradicts the hit (two statics' initialisers of the same shape, inpkeys')."""
    owners = collections.defaultdict(set)
    for name, addrs in known.items():
        for a in addrs:
            owners[a].add(name)
    by_obj = collections.defaultdict(list)
    for item in results:
        if generated(item[0]['symbol']):
            by_obj[id(item[1])].append(item)
    at = collections.defaultdict(lambda: collections.defaultdict(set))   # obj -> address -> its copies' symbols
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            at[id(obj)][int(r['addresses'][0], 16)].add(r['symbol'])
    for r, obj, sec, s, length, hits in pending:
        for h in hits:
            at[id(obj)][h].add(r['symbol'])
    out = collections.defaultdict(set)
    for key, items in by_obj.items():
        if key not in ranges:
            continue
        defs = {r['symbol']: (r, obj, sec, s, length) for r, obj, sec, s, length in items}

        def check(fn, h):
            """None if the function can't be at h, else (confirmations, links, [(symbol, target)])."""
            r, obj, sec, s, length = fn
            o = h - base
            pat = obj.pattern(sec, s, s + length)[0]
            if o < 0 or not re.fullmatch(pat, text[o:o + length], re.S):
                return None
            conf, links, opens = 0, {}, []
            for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
                if n != 4 or sym is None or typ not in (0x06, 0x14):
                    continue
                v = struct.unpack('<I', text[o + k:o + k + 4])[0]
                v = (v - struct.unpack('<I', addend)[0]) & 0xFFFFFFFF if typ == 0x06 \
                    else (h + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]) & 0xFFFFFFFF
                t = thunk.get(v, v)
                name = sym['name']
                if generated(name):
                    if name in defs and name != r['symbol']:
                        links[name] = t
                    continue
                if name in known:
                    if t not in known[name]:
                        return None
                    conf += 1
                    # still the initialiser's target: symbols.json may name it from an earlier run's
                    # match that this very rule made (a regeneration must not take the evidence away)
                    opens.append((name, t))
                elif owners.get(t) or (at[key].get(t) and name not in at[key][t]):
                    return None
                else:
                    opens.append((name, t))
            return conf, links, opens

        for name, fn in defs.items():
            r, obj, sec, s, length = fn
            if r['status'] == 'exact':
                hits = [int(r['addresses'][0], 16)]
            else:
                lo, hi = ranges[key]
                a0 = max(lo - RANGE_GAP - base, 0)
                a1 = min(hi + RANGE_GAP - base + length, len(text))
                pat = obj.pattern(sec, s, s + length)[0]
                hits = [base + a0 + m.start() for m in re.finditer(b'(?=' + pat + b')', text[a0:a1], re.S)]
            good = []
            for h in hits:
                c = check(fn, h)
                if c is None or not (c[0] or r['status'] == 'exact'):
                    continue
                conf, links, opens = c
                ok = True
                spots = [h]
                for ln, la in links.items():
                    lc = check(defs[ln], la)
                    if lc is None:
                        ok = False
                        break
                    mine = dict(opens)
                    if any(mine.get(sym, t) != t for sym, t in lc[2]):
                        ok = False      # the static's address differs between the two
                        break
                    opens = opens + lc[2]
                    spots.append(la)
                if ok:
                    good.append((opens, spots))
            if len(good) == 1:
                for sym, t in good[0][0]:
                    out[sym].add(t)
                if located is not None:
                    located[key].update(good[0][1])
    return out


USE_XCU = True
_xcu = {}


def xcu_previous(img):
    """{initialiser address: the value of the slot before it} of the CRT initialiser table (.CRT$XCU, merged into
    .data: the longest run of dwords that are 0 or .text addresses)."""
    if 'prev' not in _xcu:
        ts, te, _ = img.section('.text')
        ds, de, raw = img.section('.data')
        vals = struct.unpack_from('<%dI' % (min(de - ds, len(img.data) - raw) // 4), img.data, raw)
        best, cur, nz, bnz = (0, 0), 0, 0, 0
        for i, v in enumerate(vals):
            if v == 0 or ts <= v < te:
                cur += 1
                nz += v != 0
                if nz > bnz:
                    best, bnz = (i - cur + 1, i + 1), nz
            else:
                cur, nz = 0, 0
        prev = {}
        for i in range(best[0] + 1, best[1]):
            if vals[i]:
                prev.setdefault(vals[i], vals[i - 1])
        _xcu['prev'] = prev
    return _xcu['prev']


USE_XCU_POSITION = True     # D242 P-C1-2: the `_$E` initialisers credited by their CRT table position
USE_XCU_SUBRUN = True       # C15 (2026-11-16): the unique consecutive sub-run fitting every initialiser
XCU_TABLE = (0xB6C000, 0xB6D700)
USE_OBJECT_ORDER = True     # D242 P-C1-1: an own-function tie inside one object assigned in object order


def xcu_table(img):
    """The non-zero entries of the client's CRT initialiser table (.CRT$XCU merged into .data, XCU_TABLE), in table
    order: the initialisers of every object, in link order."""
    if 'table' not in _xcu:
        out = []
        for va in range(XCU_TABLE[0], XCU_TABLE[1], 4):
            o = img.offset(va)
            v = struct.unpack_from('<I', img.data, o)[0] if o is not None else 0
            if v:
                out.append(v)
        _xcu['table'] = out
    return _xcu['table']


def xcu_initialisers(obj):
    """The symbols an object's .CRT$XCU sections list (its dynamic initialisers), in section and slot order."""
    out = []
    for info in obj.sections:
        if info['name'].startswith('.CRT$XC') and info['name'] not in ('.CRT$XCA', '.CRT$XCZ'):
            for va, symidx, typ in sorted(info['relocs']):
                sym = obj.symbol_at(symidx)
                if sym is not None:
                    out.append(sym['name'])
    return out


def _fullmatch_at(obj, sec, s, length, text, base, a):
    o = a - base
    if length <= 0 or o < 0 or o + length > len(text):
        return False
    return re.fullmatch(obj.pattern(sec, s, s + length)[0], text[o:o + length], re.S) is not None


def _object_order(results):
    """{id(obj): [((sec, start), address or None, item)]} of each object's own functions in object order (section,
    then offset: the order /Gy's linker keeps, library-matching.md 3.6), with the address when the function is
    exact."""
    out = collections.defaultdict(list)
    for item in results:
        r, obj, sec, s, length = item
        if obj.own(sec):
            out[id(obj)].append(((sec, s), int(r['addresses'][0], 16) if r['status'] == 'exact' else None, item))
    for v in out.values():
        v.sort(key=lambda x: x[0])
    return out


def xcu_subrun(run, n, fits):
    """C15 (run 2026-11-16, amends P-C1-2): the n table entries an object's n initialisers take out of RUN (the
    entries lying in their windows, address order): the one consecutive sub-run of length n that FITS (every
    initialiser at its entry), None if none or more than one. An object's first window can also hold the previous
    object's last initialiser (sectorsound's 0x43D2F0 in soundlib's window), so the whole run is not required to be
    the object's. Without USE_XCU_SUBRUN only a run of exactly n entries is tried."""
    subs = [run[i:i + n] for i in range(len(run) - n + 1)] if USE_XCU_SUBRUN else [run] if len(run) == n else []
    subs = [x for x in subs if fits(x)]
    return subs[0] if len(subs) == 1 else None


def _neighbours(order, key):
    """(address of the nearest exact own function before KEY in object order, of the nearest after), None if none."""
    before = [a for k, a, _ in order if k < key and a is not None]
    after = [a for k, a, _ in order if k > key and a is not None]
    return (before[-1] if before else None), (after[0] if after else None)


def xcu_position(img, results, ranges, text, base, claimed, thunk):
    """D242 P-C1-2 (amends D94(b)): [(item, address, kind)] of `_$E` functions credited by the CRT initialiser table.
    The table lists every object's initialisers in link order, so the n-th initialiser an object's .CRT$XCU lists
    is the n-th table entry in the object's stretch. Per object: each initialiser's window is the gap between its
    nearest exact own functions in object order, clipped to the object's range +- RANGE_GAP and to the exact own
    functions of the other objects of the run; the table entries lying in the union of those windows must be
    consecutive in the table, as many as the initialisers, each the n-th initialiser's (inside its own window,
    its pattern fullmatching there, no other symbol there; an initialiser already exact must sit at its entry;
    C15: the one such sub-run of the entries in the windows, `xcu_subrun`).
    Then the `_$E` functions they reference (the constructor wrapper they call, the atexit stub they push or jump
    to, transitively) are credited at the targets the initialisers' bytes give, where their patterns fullmatch.
    kind is 'table' or 'linked'. The names stay unused across objects: the position is the proof."""
    table = xcu_table(img)
    index = {a: i for i, a in enumerate(table)}
    orders = _object_order(results)
    own_exact = sorted((a, k) for k, v in orders.items() for _, a, _ in v if a is not None)
    by_obj = collections.defaultdict(dict)
    for item in results:
        by_obj[id(item[1])][item[0]['symbol']] = item
    out = []
    for key, defs in by_obj.items():
        if key not in ranges:
            continue
        obj = next(iter(defs.values()))[1]
        inits = xcu_initialisers(obj)
        if not inits or any(n not in defs or not generated(n) for n in inits):
            continue
        lo, hi = ranges[key]
        below = [a for a, k in own_exact if a < lo and k != key]
        above = [a for a, k in own_exact if a > hi and k != key]
        blo = max(lo - RANGE_GAP, below[-1] + 1 if below else 0)
        bhi = min(hi + RANGE_GAP, above[0] - 1 if above else 0xFFFFFFFF)
        wins = []
        for n in inits:
            r, _, sec, s, length = defs[n]
            p, q = _neighbours(orders[key], (sec, s))
            wins.append((max(blo, p + 1 if p is not None else blo), min(bhi, q - 1 if q is not None else bhi)))
        run = sorted({t for t in table for w in wins if w[0] <= t <= w[1]})
        def fits(sub):
            if any(index[b] != index[a] + 1 for a, b in zip(sub, sub[1:])):
                return False
            for n, (w0, w1), a in zip(inits, wins, sub):
                r, _, sec, s, length = defs[n]
                if not w0 <= a <= w1 or claimed[a] - {n} or (r['status'] == 'exact' and int(r['addresses'][0], 16) != a) \
                        or r['status'] != 'exact' and not _fullmatch_at(obj, sec, s, length, text, base, a):
                    return False
            return True
        run = xcu_subrun(run, len(inits), fits)
        if run is None:
            continue
        got = {n: a for n, a in zip(inits, run)}
        todo = list(got.items())
        while todo:         # what the initialisers' own bytes reach: the wrapper they call, the atexit stub they push
            n, a = todo.pop()
            r, _, sec, s, length = defs[n]
            o = a - base
            for k, sz, typ, sym, addend in obj.reloc_targets(sec, s, length):
                if sz != 4 or sym is None or sym['name'] not in defs or not generated(sym['name']) \
                        or sym['name'] in got or typ not in (0x06, 0x14) or o + k + 4 > len(text):
                    continue
                v = struct.unpack('<I', text[o + k:o + k + 4])[0]
                v = (v - struct.unpack('<I', addend)[0]) & 0xFFFFFFFF if typ == 0x06 \
                    else (a + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]) & 0xFFFFFFFF
                v = thunk.get(v, v)
                tr, _, tsec, ts, tlen = defs[sym['name']]
                if claimed[v] - {sym['name']} or (tr['status'] == 'exact' and int(tr['addresses'][0], 16) != v) \
                        or tr['status'] != 'exact' and not _fullmatch_at(obj, tsec, ts, tlen, text, base, v):
                    continue
                got[sym['name']] = v
                todo.append((sym['name'], v))
        for n, a in got.items():
            if defs[n][0]['status'] != 'exact':
                out.append((defs[n], a, 'table' if n in inits else 'linked'))
    return out


USE_ORDER_IN_RANGE = True


_starts_cache = {}


def _verified_starts(img):
    if id(img) not in _starts_cache:
        _starts_cache[id(img)] = img.function_starts()[0]
    return _starts_cache[id(img)]


def _padding(text, base, lo, hi):
    return all(b in (0xCC, 0x90) for b in text[lo - base:hi - base])


def short_order(img, results, text, base, claimed, known):
    """D246 / CR1: [(item, address)] for short own functions placed by object order. For each pair of consecutive
    exact own functions P, Q of one object in object order (P's address below Q's) whose own functions between them
    are all `short`: the client's verified starts strictly between P and Q must be as many as those functions, P's
    end, the gaps and the last one's end up to Q only padding, and each start must fullmatch its counterpart (the
    k-th function in object order at the k-th address), unclaimed, with D80's checks leaving it."""
    starts = _verified_starts(img)
    out = []
    for key, order in _object_order(results).items():
        exact = [i for i, (k, a, it) in enumerate(order) if a is not None]
        for i, j in zip(exact, exact[1:]):
            members = [it for _, _, it in order[i + 1:j]]
            p, q = order[i][1], order[j][1]
            kinds = ('short', 'multiple') if USE_GAP_ORDER else ('short',)
            if not members or q <= p or any(it[0]['status'] not in kinds or it[4] <= 0 for it in members):
                continue
            cands = starts[bisect.bisect_right(starts, p):bisect.bisect_left(starts, q)]
            if len(cands) != len(members) or not _padding(text, base, p + order[i][2][4], cands[0]):
                continue
            ok = True
            for (r, obj, sec, s, length), a, e in zip(members, cands, cands[1:] + [q]):
                if claimed[a] or not _fullmatch_at(obj, sec, s, length, text, base, a) or \
                        not _padding(text, base, a + length, e) or \
                        break_tie(obj, sec, s, length, text, base, [a], img, known)[0] != [a]:
                    ok = False
                    break
            if ok:
                out += list(zip(members, cands))
    return out


def edge_order(img, results, text, base, claimed, known):
    """D246 / CR6: [(item, address)] for own functions at an object's edges placed by object order. An object whose
    exact own functions rise in object order: the own functions before its first exact one P, nearest first, take
    the client's verified starts right below P one by one (each start's function, its own length, then only padding
    up to the next one), and the own functions after its last exact one Q, nearest first, the starts right after
    Q's end; each `short` or `multiple`, fullmatching there, unclaimed, with D80's checks leaving it. The first that
    fails ends that edge. An address two symbols take (another object's edge or gap function with the same bytes)
    is left alone. Not a test of other objects' ranges: one stray exact function makes a range span a quarter of
    the app (unknown_7cef30's 0x60A3E0-0x7DC8F0 covers both constructors below). P-C4-1: an object's range ends at its exact
    functions, so a byte-identical copy of an edge function in another object's code tied with it (sbaseview's and
    ui_symbolchat's option-struct constructors 0x69AC20 / 0x64A330)."""
    starts = _verified_starts(img)
    orders = _object_order(results)

    def ok_at(key, item, a, nxt):
        r, obj, sec, s, length = item
        return r['status'] in ('short', 'multiple') and length > 0 and not claimed[a] and             _fullmatch_at(obj, sec, s, length, text, base, a) and _padding(text, base, a + length, nxt) and             break_tie(obj, sec, s, length, text, base, [a], img, known)[0] == [a]

    out = []
    for key, order in orders.items():
        exact = [i for i, (k, a, it) in enumerate(order) if a is not None]
        if not exact or any(order[i][1] >= order[j][1] for i, j in zip(exact, exact[1:])):
            continue
        bound = order[exact[0]][1]
        for j in range(exact[0] - 1, -1, -1):
            k = bisect.bisect_left(starts, bound) - 1
            if k < 0 or not ok_at(key, order[j][2], starts[k], bound):
                break
            out.append((order[j][2], starts[k]))
            bound = starts[k]
        end = order[exact[-1]][1] + order[exact[-1]][2][4]
        for j in range(exact[-1] + 1, len(order)):
            k = bisect.bisect_left(starts, end)
            if k >= len(starts):
                break
            a = starts[k]
            item = order[j][2]
            if not _padding(text, base, end, a) or not ok_at(key, item, a, a + item[4]):
                break
            out.append((item, a))
            end = a + item[4]
    by_addr = collections.defaultdict(set)
    for (r, *_), a in out:
        by_addr[a].add(r['symbol'])
    return [(it, a) for it, a in out if len(by_addr[a]) == 1]


def _after_padding(text, base, end, limit):
    """The first 16-aligned address at or after END below LIMIT whose byte is not padding, with only padding
    (int3 / nop) before it from END; None when there is none or the padding runs past 31 bytes (two alignment
    units: a start nothing calls is taken only that close)."""
    a = (end + 15) & ~15
    while a < limit and a - end <= 31 and a - base < len(text) and text[a - base] in (0xCC, 0x90):
        a += 16
    if a >= limit or a - end > 31 or a - base >= len(text) or not _padding(text, base, end, a):
        return None
    return a


def gap_edges(img, results, text, base, claimed, known):
    """D246 (EX): [(item, address)] for `short` / `multiple` own functions at the edges of a gap between two
    consecutive exact own functions P, Q of one object in object order (P's address below Q's) that short_order cannot
    align (a not-exact member, or a count of verified starts that differs). Forward from P's end: each gap member in object
    order, while it is short or tied, takes the next verified start (only padding before it), fullmatching there with
    its own length and then only padding up to the next start; backward from Q the same with the starts below Q.
    Every address stays strictly between P and Q, unclaimed, D80's break_tie leaving it, and no other member of the
    gap fullmatches there; the two sides may not overlap, and an address two symbols take is left alone.
    USE_EX_GAP_PADDED: forward from an exact anchor, a member may start at the first 16-aligned non-padding byte
    within 31 bytes of the previous end when no verified start lies there. USE_EX_GAP_KNOWN: a not-exact own function that D80's
    known (symbols.json) gives one address, unclaimed by another symbol, anchors the same way (the member after it at
    the very next verified start, no padding test since its client length is unknown), also as an object's last
    anchor (the members after it follow it)."""
    starts = _verified_starts(img)

    def ok_at(item, a, nxt):
        r, obj, sec, s, length = item
        return r['status'] in ('short', 'multiple') and length > 0 and not claimed[a] and \
            _fullmatch_at(obj, sec, s, length, text, base, a) and _padding(text, base, a + length, nxt) and \
            break_tie(obj, sec, s, length, text, base, [a], img, known)[0] == [a]

    out = []
    for key, order in _object_order(results).items():
        # anchors: the exact own functions (address, end) and, USE_EX_GAP_KNOWN, a not-exact own function that
        # symbols.json gives one address (D80's known; its end there is the next verified start)
        anchors = []
        for idx, (k, a, it) in enumerate(order):
            if a is not None:
                anchors.append((idx, a, a + it[4], True))
            elif USE_EX_GAP_KNOWN and it[0]['status'] == 'none' and len(known.get(it[0]['symbol'], ())) == 1:
                a = next(iter(known[it[0]['symbol']]))
                k2 = bisect.bisect_right(starts, a)
                if k2 < len(starts) and not claimed[a] - {it[0]['symbol']}:
                    anchors.append((idx, a, starts[k2], False))
        tail = [(anchors[-1], (len(order), 0xFFFFFFFF, 0, True))] if anchors and not anchors[-1][3] else []
        for (i, p, pend, pexact), (j, q, _, _) in list(zip(anchors, anchors[1:])) + tail:
            members = [it for _, _, it in order[i + 1:j]]
            if not members or q <= p:
                continue        # nothing between, or out of order (what short_order took the hook drops)

            def alone(item, a):
                # no other member of the gap has these bytes there: one symbol to one address (D80)
                return not any(o is not item and o[0]['status'] in ('short', 'multiple') and o[4] > 0 and
                               _fullmatch_at(o[1], o[2], o[3], o[4], text, base, a) for o in members)
            fwd, end, pad = [], pend, pexact
            for item in members:
                if item[0]['status'] not in ('short', 'multiple'):
                    break
                k = bisect.bisect_left(starts, end)
                if pad and USE_EX_GAP_PADDED:
                    a = _after_padding(text, base, end, q)
                    if a is not None and (k >= len(starts) or starts[k] != a):
                        # no verified start there: the member's bytes, then padding to the next 16-byte boundary
                        e = (a + item[4] + 15) & ~15
                        if a < q and e <= q and ok_at(item, a, e) and alone(item, a):
                            fwd.append((item, a))
                            end = a + item[4]
                            continue
                        break
                if k >= len(starts) or starts[k] >= q:
                    break
                a = starts[k]
                nxt = starts[k + 1] if k + 1 < len(starts) else a + item[4]
                if (pad and not _padding(text, base, end, a)) or (not pad and a != end) or \
                        not ok_at(item, a, min(nxt, q)) or not alone(item, a):
                    break
                fwd.append((item, a))
                end, pad = a + item[4], True
            bwd, bound = [], q
            for item in reversed(members[len(fwd):]) if q != 0xFFFFFFFF else ():
                k = bisect.bisect_left(starts, bound) - 1
                if k < 0 or starts[k] < end or (fwd == [] and not pexact and starts[k] <= p) or \
                        item[0]['status'] not in ('short', 'multiple'):
                    break
                a = starts[k]
                if not ok_at(item, a, bound) or not alone(item, a):
                    break
                bwd.append((item, a))
                bound = a
            out += fwd + bwd
    by_addr = collections.defaultdict(set)
    for (r, *_), a in out:
        by_addr[a].add(r['symbol'])
    return [(it, a) for it, a in out if len(by_addr[a]) == 1]


def object_order(pending, done, survivors_of, ranges, claimed, obj_called, results):
    """D242 P-C1-1: [(pending item, address)] for ties inside one object. A group is the tied own functions (not
    COMDATs: D189(b)'s order test held for 349 of 353 own groups, 608 of 700 inline ones did not) of one object that
    share one set of surviving candidates, as many members as candidates, every candidate inside the object's range
    and held by no other symbol, and no tied function outside the group surviving at any of them. The members, in
    object order, take the candidates in address order, when each lands between its nearest exact own functions in
    object order and the object's own exact calls to it (P-CGP-1) do not contradict it. P-SC-1 (C24, 2026-11-15): an
    own function's candidates are first cut to its object's range, unclaimed (USE_ORDER_IN_RANGE): copies in other
    files' code neither enlarge the group's candidate set nor block it."""
    orders = _object_order(results)
    groups = collections.defaultdict(list)
    at = collections.defaultdict(set)
    for item in pending:
        if id(item[0]) in done:
            continue
        surv = survivors_of[id(item[0])]
        rng = ranges.get(id(item[1]))
        if USE_ORDER_IN_RANGE and rng and item[1].own(item[2]):
            # P-SC-1 (C24): an own function's candidates outside its object's range (other files' copies) never
            # take part: two string statics' atexit destructors tied over 20 copies in the image
            surv = [h for h in surv if rng[0] <= h <= rng[1] and not claimed[h]]
        surv = frozenset(surv)
        for h in surv:
            at[h].add(id(item[0]))
        groups[(id(item[1]), surv)].append(item)
    out = []
    for (key, surv), items in groups.items():
        if len(surv) < 2 or len(items) != len(surv) or key not in ranges:
            continue
        lo, hi = ranges[key]
        ids = {id(x[0]) for x in items}
        if any(not x[1].own(x[2]) for x in items) or any(not lo <= h <= hi or claimed[h] or at[h] - ids for h in surv):
            continue
        items = sorted(items, key=lambda x: (x[2], x[3]))
        addrs = sorted(surv)
        ok = True
        for (r, obj, sec, s, length, hits), a in zip(items, addrs):
            p, q = _neighbours(orders[key], (sec, s))
            reach = obj_called.get((key, r['symbol']))
            if (p is not None and a <= p) or (q is not None and a >= q) or (reach and a not in reach):
                ok = False
                break
        if ok:
            out += list(zip(items, addrs))
    return out


USE_RANGE_EVIDENCE = True


def range_evidence(obj, sec, start, length, text, base, a, rng, thunk, own_names, own_known):
    """P-CW1-1: (callees hold, named, own calls) of a range-round proposal at `a`, compared as a tuple.
    `callees hold`: False when one of its relative calls / jumps goes to a function defined in this object
    whose own bytes (relocations masked) do not fullmatch where the call lands at `a` (through the thunk): the
    symbol is not at `a` (a template callback's ??_G calling its own inline destructor, against a class's ??_G
    calling a destructor of another file). `named`: the client names `a` with the function's own symbol
    (symbols.json or a vtable slot the run learned: D88 / D171's self names; or an exact function's call or
    reference to the symbol reaches `a`). `own calls`: how many of its calls go to a function of the object's
    own code (not a COMDAT: the linker keeps it with this object) and land inside the object's range (`rng`)."""
    named = a in own_names or a in own_known
    calls = 0
    o = a - base
    funcs = getattr(obj, '_funcs', None)
    if funcs is None:
        funcs = obj._funcs = {(sc, st): en for nm, sc, st, en in obj.functions()}
    for k, n, typ, sym, addend in obj.reloc_targets(sec, start, length):
        if typ != 0x14 or n != 4 or sym is None or sym['section'] <= 0 or (sym['type'] >> 4) != 2 \
                or o + k + 4 > len(text):
            continue
        t = a + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
        t = thunk.get(t, t) & 0xFFFFFFFF
        end = funcs.get((sym['section'] - 1, sym['value']))
        if end is not None:
            pat, fixed, ln = obj.pattern(sym['section'] - 1, sym['value'], end)
            if ln and (not 0 <= t - base <= len(text) - ln or not re.fullmatch(pat, text[t - base:t - base + ln], re.S)):
                return (False, named, 0)
        if rng and obj.own(sym['section'] - 1) and rng[0] <= t <= rng[1]:
            calls += 1
    return (True, named, calls)


RANGE_GAP = 0x1000   # D114(b): how far beyond its own range an object's lone (or D183 shared) exact COMDAT may lie


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('objects', nargs='+')
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    ap.add_argument('--json')
    ap.add_argument('-q', action='store_true', help='summary only')
    a = ap.parse_args()
    img = ClientImage(a.client)
    results = match_objects(img, a.objects)
    if a.json:
        json.dump(results, open(a.json, 'w', newline='\n'), indent=1)
    counts = {}
    for r in results:
        counts[r['status']] = counts.get(r['status'], 0) + 1
        if not a.q:
            print(f"{r['status']:8s} {r['object']:16s} {r['name'][:48]:48s} len={r['length']:5d} "
                  f"{' '.join(r['addresses'][:3])}")
    print(' '.join(f'{k}={v}' for k, v in sorted(counts.items())), f'total={len(results)}')


if __name__ == '__main__':
    main()
