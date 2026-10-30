# Coding conventions

The rebuild is written the way Westwood wrote Earth & Beyond. We don't have E&B's own source, so
these rules come from two places:

* **What client.exe says about itself.** Class and method names in assert, profile and log
  strings (`docs/data/class-methods.txt`), assert expressions with member names
  (`docs/data/asserts.txt`), RTTI names (`docs/data/rtti.txt`), and the source tree
  (`docs/data/source-paths.txt`; see [source-tree.md](source-tree.md)).
* **The Westwood code EA published.** Renegade (`Code/`) and the WWVegas libraries Generals and Zero Hour
  shipped with. E&B's naming matches these libraries and Renegade's game code (`Combat`,
  `Commando`): the patterns in §1.

Generals' own game code (`GameEngine`), written by a different team, is the counterexample: its
`m_` members, camelCase methods and `AsciiString` are **not** E&B's style; don't copy them.

When a rule here and a string from the client disagree, the client wins. Record the case in §5.

## 1. Evidence from the client

| Pattern | Examples from client.exe |
|---|---|
| `XxxClass` class names | `LoginTaskClass`, `InvDataClass`, `SlotDataClass`, `GadgetClass`, `ClientObjectClass`, `UIViewClass`, `StarbaseRoomClass` |
| Packets: `XxxPacketClass` with `Process_Packet` | `CreatePacketClass::Process_Packet`, `DamagePacketClass::Process_Packet`, `ClientShipPacketClass::Process_Packet`, `StarbaseSetPacketClass` (~100 more) |
| Methods: `Capitalised_Words_With_Underscores` | `KernelClass::Run_One_Cycle`, `MainView::Render_Planet_Mode`, `AttribDir::Attach_Update`, `CordMgr::Pump_Connections`, `Land::Update_Lightmap_ManualThreaded` |
| Members: `CamelCase`, no prefix | `Position`, `Orientation`, `CurrentSpeed`, `ImpartedVelocity`, `RemainingTime`, `SkyIntensity.Sun`, `RPGInfo.Profession`, `Parent.Members`, `Data->Listeners` |
| Some members `Capitalised_Underscore` | `Pending_Delete->Shutdown()`, `Background_Task->Run()`, `Current_Task->Render()` (the task kernel) |
| Locals and parameters: `lower_case_with_underscores` | `!level_aux_data`, `!rpg_aux_data`, `!main_gadget`, `!space_client`, `member_find`, `attribute_find`, `find_result`, `set_speed` |
| Enum constants: `UPPER_CASE` | `State != UI_CLOSED`, `InvDataClass::NO_SLOT` |
| Virtual "V" prefix for abstract interfaces (inferred) | `VGadgetClass`, `VSlotDataClass`, `VClientObjectClass`, `VRadarDataClass`, `VCameraModuleClass`, `VToolTipClass` |
| `PAV` prefix (meaning unknown) | `PAVRadarDataClass`, `PAVNavigationDataClass`, `PAVStarmapDataClass`, `PAVGutterDataClass`, `PAVEmoteDataClass` |
| STL is used freely | `Data->FlagList.size() > 0`, `find_result == Data->Listeners.end()`, `!INI.Load(filename.c_str())`, `i->second` |
| Not every class has the suffix | `MainView`, `AttribDir`, `DynObj`, `CordMgr`, `CordCon`, `ENBAssetManager`, `CallbackManager`, `ControlKeysManager`, `GuildRecruitConfirmSectorPacket`, `PermString` (D37) |
| Some modules use camelCase methods | `CordCon::handlePacket`, `CordCon::buildMsgHeader`, `CordCon::pump` (`app/wwudp`, the UDP layer: a different author) |
| Asserts on | Release build keeps assert expressions and `__FILE__` paths; `WWMath::Is_Valid_Float(...)` checks are common |

**Rule:** where the client names a class, method or member, use exactly that name, including
its inconsistencies (`MainView`, not `MainViewClass`; `CordCon::handlePacket`, not
`Handle_Packet`). Where it doesn't, follow §2.

**Unknowns** (decision D10): a member, flag or virtual whose purpose isn't known gets a neutral
name in the §1 patterns (`Unknown7C`, after its offset, like any member: `CamelCase`, no prefix;
`UnknownFlag`; `Unknown_7C` for a method) with its evidence in a comment. Inside an imported
library, follow that library's own style: WWAudio's members are `m_`-prefixed, so its unknowns
are too (`m_Unknown7C`). It is NAME 1 on
D56's scale (D65): replaced as soon as the code that uses it, or a structure-matched Net-7 name
(D59), gives better evidence, the tag recording that evidence.

**Name certainty** (decision D56): a name chosen without the client's own evidence gets a tag
comment where it is declared: `// NAME n: evidence`. 5 = the client names it (RTTI, assert,
`__FILE__`, a `Class::Method` string); 4 = EA's Westwood source names it, or a Net-7 name or one
from the earlier protocol research (prior-work.md), confirmed by matching structure; 3 = built by convention from a client-named
file or sibling (D24, a `.h` paired with a known `.cpp`); 2 = describes what the code does;
1 = neutral placeholder. Names scoring 3 or below (and untagged `Unknown*` names, counted as 1)
are listed in the generated `docs/data/names-review.md` (`python tools/re/names.py`); raise the tag
when better evidence turns up. Example: `// NAME 2: encode.cpp is the client's; header name assumed`.

**App classes the client doesn't name** (D24, user 2026-10-24): Westwood `XxxClass` style, as
the client's own named classes are (`BitSetClass` for `app\shared\auxdata\bitset.cpp`, members
from its asserts: `Buffer`, `NumBits`). A name the client later shows replaces it.

## 2. Westwood (WWVegas / Renegade) style

### Files

* File names are lower case, short, often abbreviated (`ui_cpit.cpp`, `pdaint.cpp`, `camctl.cpp`,
  `dx8wrapper.cpp`). E&B's own files are snake_case (`chat_window_history.cpp`) or 8.3-style
  abbreviations (`ctrngadg.cpp`, `debrsint.cpp`). For a file we know only from its class, take the
  name from `docs/data/source-paths.txt` if it's there.
* One class (or a closely related group) per `.h`/`.cpp` pair.
* Header guard: `#pragma once` under `_MSC_VER` plus a classic guard:

```cpp
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SOUND2DHANDLE_H
#define __SOUND2DHANDLE_H
...
#endif //__SOUND2DHANDLE_H
```

  (WWLib uses `INI_H`, without underscores; WW3D/WWAudio use `__NAME_H`. Copy the style of the
  library the file belongs to; use `__NAME_H` in E&B's own code.)

* File header: our own files start with `// Earth & Beyond client (reconstruction)`; Westwood's
  boxed banner and EA's licence header appear only in files imported from EA's releases (§4).
* Forward-declare classes in headers instead of including headers when you can.

### Classes

```cpp
//////////////////////////////////////////////////////////////////////
//
//	Sound2DHandleClass
//
//////////////////////////////////////////////////////////////////////
class Sound2DHandleClass : public SoundHandleClass
{
public:

	///////////////////////////////////////////////////////////////////
	//	Public constructors/destructors
	///////////////////////////////////////////////////////////////////
	Sound2DHandleClass (void);
	~Sound2DHandleClass (void);

	///////////////////////////////////////////////////////////////////
	//	Public methods
	///////////////////////////////////////////////////////////////////
	HSAMPLE		Get_HSAMPLE (void)		{ return SampleHandle; }
	void			Initialize (SoundBufferClass *buffer);

protected:

	///////////////////////////////////////////////////////////////////
	//	Protected member data
	///////////////////////////////////////////////////////////////////
	HSAMPLE		SampleHandle;
};
```

* `public:`, then `protected:`, then `private:`; methods before data. Sections are marked with the
  `////` banners shown above.
* Accessors: `Get_Xxx` / `Set_Xxx` / `Is_Xxx` / `Peek_Xxx` (a pointer the caller doesn't own) /
  `Add_Ref` / `Release_Ref` (`RefCountClass`). Getters are usually inline in the header.
* `(void)` for empty parameter lists. A space before the `(` in WW3D/WWAudio code; none in WWLib.
  E&B's own code: no space (unknown; choose one per file and keep it).
* Downcasting by virtual `As_XxxClass()` methods that return `this` or `NULL`, not
  `dynamic_cast` (Westwood's own RTTI). E&B also has its own RTTI in `app/shared/auxdata/rtti.h`.
* Singletons: `static XxxClass *Get_Instance (void)` (e.g. `WWAudioClass::Get_Instance()`).
* Reference counting: `RefCountClass`, `REF_PTR_SET`, `REF_PTR_RELEASE`, `RefCountPtr<T>`
  (`wwlib/ref_ptr.h`, used by E&B).

### Types and values

* WWLib's types (`bittype.h`): `uint8`, `uint16`, `uint32`, `sint8`, `sint16`, `sint32`, `uint`,
  `sint`, `float32`. Plain `int`, `float` and `bool` are common too.
* `NULL`, not `nullptr` or `0`, for pointers.
* Strings: WWLib's `StringClass` / `WideStringClass` in library code; E&B's own code also uses
  `std::string` (see §1).
* Math: `Vector3`, `Matrix3D`, `Quaternion`, `AABoxClass` from WWMath; `WWMATH_EPSILON`,
  `WWMath::Is_Valid_Float`.

### Statements and comments

* Braces: K&R in function bodies (`if (x) {`); the function's own opening brace on its own line.
  Tabs for indentation; members aligned in columns with tabs.
* Function header comment blocks, Westwood style:

```cpp
/***********************************************************************************************
 * INIClass::Clear -- Clears out a section (or all sections) of the INI data.                  *
 *                                                                                             *
 * INPUT:   section  -- Pointer to the section to clear out [pass NULL to clear all].          *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/02/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
```

  WW3D/WWAudio code more often uses the lighter form:

```cpp
////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Initialize
//
////////////////////////////////////////////////////////////////////////////////////////////////
```

  A reconstructed function has a comment above it giving the client address its body came from
  and what it does, e.g. `// 0x801860: the Unknown120 scene through the scripted camera ...`.
* Within a body, comment blocks use the Westwood double-slash box:

```cpp
	//
	// Make sure this handle is fresh
	//
```

* Asserts: `WWASSERT(expr)` in library code. E&B's app code uses `G_ASSERT(expr)` and
  `FAIL_IF(expr)` (`app/shared/unknown_debug.h`, over the client's report functions 0x4E9410 and
  0x4E9450; the macro names are ours, the client's are not known). Reproduce each assert where the
  binary has it, with the expression exactly as stringified in the client (`docs/data/asserts.txt`)
  and on the client's line: the macros compile `__FILE__` and `__LINE__` ([workflow.md](workflow.md) §3).
* Profiling: `WWPROFILE("Class::Method")` at the top of hot functions. The label strings in the
  client tell you where they were.

### Inline assembly

Westwood used inline assembly (`__asm`) where VC6 couldn't express something or couldn't do it
fast enough: CPU instructions with no C equivalent (`CPUID`, `RDTSC`, `int 3`), fast float to int
conversion (`fistp` instead of the slow `_ftol`), atomic locks, and hand-tuned hot loops (RC4,
LCW compression, blitters). Where the client has assembly, the reconstruction has the same
assembly: it is the only way to reproduce those bytes (decided 2026-10-22; an earlier "no
assembly" rule was withdrawn).

**Every assembly block carries a comment giving its C equivalent**, so the code can be read
without decoding the assembly. The comment goes immediately before the block (or the
function) in this form:

```cpp
	//
	//	C equivalent:
	//
	//		do {
	//			x++;
	//			y += state[x];
	//			swap(state[x], state[y]);
	//			*buffer++ ^= state[(state[x] + state[y]) & 0xFF];
	//		} while (--buffer_len > 0);		// note: runs once even when buffer_len == 0
	//
	//	Why assembly: ...
	//
	__asm {
```

* Say what the block computes, in C, and anything surprising about it (side effects,
  original bugs, registers it relies on).
* Add a line on why it is assembly when that is not obvious.
* Comments never change the compiled bytes. In imported EA files they do move the lines
  below them, so they are added through `tools/vc6/annotations.py` (re-applied on every
  import), and the library is re-verified afterwards.

## 3. Toolchain the conventions assume

The client was built with **Visual C++ 6.0** **Service Pack 4** (compiler build 8799, confirmed; [toolchain.md](toolchain.md)) and linked by LINK 6.0
(see [binary.md](binary.md)). The code should be valid VC6 C++: no `nullptr`, `auto`,
`override`, range-for, `<cstdint>`; `for`-loop variables leak into the enclosing scope under VC6
(write code that works either way); STL is VC6's Dinkumware (or STLport, as Generals used; which
one E&B used isn't known yet).

The code is meant to build with a modern MSVC too (the playable build, not set up yet), so it must
also be valid standard C++ (the `for`-scope rule above is why). Where VC6 and the standard disagree, use a macro in a
compatibility header, not `#ifdef`s in every file.

## 4. File headers

The header says where the file came from (the user, 2026-10-30).

**Our own files** (app/, programs/, stubs/, tests/, the tools' C/C++, and files written from the
client inside a library, such as `ww3d2/unknown_distortion.cpp` or `libs/wwonline/wnet/tcpqueue.cpp`)
start with this line, the original path after the colon when the client gives it
(`docs/data/source-paths.txt`):

```cpp
// Earth & Beyond client (reconstruction): app\client\ui_login.cpp
```

A longer description follows as a block comment:

```cpp
// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_login.cpp
*/
```

No Westwood banner box and no EA copyright line: the file is written from the client's bytes,
under the repository's GPL v3 (D1, [LICENSE](../LICENSE)). The generators (`auxroots.py`,
`pkclasses.py`, `pkemit.py`, importer.py's `NEW_FILES`) write the same line.

**Files imported from EA's C&C releases** (Renegade, Generals, Zero Hour: the `ww*` libraries,
`binkmovie`, `libs/wwonline`) keep EA's licence header and Westwood's banner unchanged. Which copy
and commit each came from is recorded in `docs/data/imports.json` ([imports.md](imports.md)), not in
the file. A file changed to match the client (PATCHES, annotations, APPENDS, functions spliced from
another copy, or an EA file reworked in place such as `wwlib/rc4.cpp`) says so on the line under EA's
copyright:

```cpp
/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
```

importer.py writes that line itself (`MODIFIED_NOTE`) on every assemble and replay, in place of
the banner's bare `**` line: don't edit imported files by hand.

**Third-party code that is not EA's** keeps its own notice (`libs/sgistl`, SGI STL 3.3, D87).

**A header change never moves a code line.** The asserts compile `__LINE__`, so a line added or
removed above code changes the bytes. Replace lines one for one, or make up the difference with
blank lines after the comment block.

## 5. Client-wins cases

Where the client's names disagree with these conventions, the client's are used; each case:

* **SoundLibraryBridge (2026-10-27, H9):** the client's bridge vtable 0xAE74EC has two slots; the tree's `ww3d2/soundlibrarybridge.h` declares three pure virtuals (`Stop_Playing_Audio` is Zero Hour's). app/audio/soundlib.cpp implements the third as an empty slot; ww3d2 stays as it is (no pair depends on it).

* **ControlKeysManager (2026-10-27, H11):** the RTTI names it `ControlKeysManager` (no `Class` suffix); ui_line_display.cpp's `ControlKeysManagerClass` (NAME 3) and its Unknown_71A960 / 71AB40 (Add_Keys / Remove_Keys), sclient.cpp's Unknown_719D50 / 719E10 (Create / Destroy) switch to control_keys_manager.h's names at their next edit, once SClient replaces the file's ControlKeysSClientClass stand-in (P-H11-1).

* **ClientObjectClass (2026-10-27, H13):** declared in app/client/co.h (vtable 0xB0070C, constructor 0x6F2820, destructor 0x6F2FE0). aux_roots.h's `UnknownCOClass` (the factory 0x6F6000) is the same class: merge it into co.h at aux_roots.h's next edit (P-H13-2). co_data's Remove_All 0x79D390 keeps a dead `Get_Game_ID()` call (the D40 budget lever, P-H13-1).

* **FontDatabaseClass (2026-10-27, H12):** app/client/fontdatabase.h; the globals 0xBE67B8 / 0xBE67BC are declared under stand-in names in tsk_game.h, sclient.cpp and ui_line_display.cpp: point them at fontdatabase.h at those files' next edit (P-H12-3).

* **ClientObjectClass's render object (2026-10-27, T11):** co.h names the member `Model`; co.cpp's assert texts say `RenderObject`, the client's name. Rename at cpacket.cpp's next edit (it and clienttest use it; P-T11-1). The aux_roots.h UnknownCOClass merge waits for the CREATE factory to move into co.cpp (aux_roots.cpp is generated; P-T11-2).

* **The input dispatcher (2026-10-27, H18):** the client's path is `app\input\inpdsp.cpp` (client wins: app/input/, not app/client/). `LoginUnknown0CClass` is defined in app/input/inpdsp.h; tsk_login.h still carries its opaque `char[0x18]` twin with the same layout and mangled names: replace it with the include at tsk_login.h's next edit and verify tsk_login, sclient and kerneltest (P-H18-1).

* **DynObj and PxWrapper (2026-10-28, DO1):** the client's profile strings name them (`DynObj::Update_Proximity`, `PxWrapper::Update(LooseBox)`), without a `Class` suffix: app/shared/dynamic_obj.h and px_wrapper.h keep them. CS1's `Unknown44AC70Class` (co_subtypes.h) and its forward-declared `ProximityObjectClass` (the FAIL_IF's member name, not a type) are gone.

* **PxBox, PxList, PxCollision and _Collision_Callback (2026-10-28, PX1):** the client's strings name them (`PxBox::On_Enter`, `PxList::Insert`, `PxCollision::Process`, the profile name `_Collision_Callback`), without a `Class` suffix; px.h / px_wrapper.h keep them, and the provisional PxEnd, PxObject, PxCallback, PxBoxList and PxCollisionBox follow that style. client.h's `PxClass` (a D24 name for px.cpp's object) is PxList. The profile string `PxWrapper::Update(LooseBox` lacks its closing parenthesis in the client; px_wrapper.cpp keeps it.
