# Coding conventions

The rebuild is written the way Westwood wrote Earth & Beyond. We don't have E&B's own source, so
these rules come from two places:

* **What client.exe says about itself.** Class and method names in assert, profile and log
  strings (`docs/data/class-methods.txt`), assert expressions with member names
  (`docs/data/asserts.txt`), RTTI names (`docs/data/rtti.txt`), and the source tree
  (`docs/data/source-paths.txt`; see [source-tree.md](source-tree.md)).
* **The Westwood code EA published.** Renegade (`Code/`) and the WWVegas libraries Generals and Zero Hour
  shipped with. E&B's naming matches these libraries and Renegade's game code (`Combat`,
  `Commando`). It does **not** match Generals' own game code (`GameEngine`: `m_` members,
  camelCase methods, `AsciiString`), which was written by a different team.

When a rule here and a string from the client disagree, the client wins. Record the case in this
file.

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
name (`m_Unknown7C`, after its offset; `UnknownFlag`) with its evidence in a comment. It is NAME 1 on
D56's scale (D65): replaced as soon as the code that uses it, or a structure-matched Net-7 name
(D59), gives better evidence, the tag recording that evidence.

**Name certainty** (decision D56): a name chosen without the client's own evidence gets a tag
comment where it is declared: `// NAME n: evidence`. 5 = the client names it (RTTI, assert,
`__FILE__`, a `Class::Method` string); 4 = EA's Westwood source names it, or a Net-7 /
the reference decoder name confirmed by matching structure; 3 = built by convention from a client-named
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

* File banner: Westwood's boxed header. Our banner keeps the box and says plainly that the
  file is a reconstruction (see the template in §4). Don't fill in `$Archive::`, `$Author::`,
  `$Modtime::` or `$Revision::`: those are source-control fields we don't have.
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

  In reconstructed functions, the HISTORY line records the reconstruction (date, `RE`) and the
  client address the body came from, e.g. `10/22/2026 RE : Reconstructed from client.exe 0x5AD540.`
* Within a body, comment blocks use the Westwood double-slash box:

```cpp
	//
	// Make sure this handle is fresh
	//
```

* Asserts: `WWASSERT(expr)` in library code. E&B's app code asserts too (expressions and
  `__FILE__` are in the binary); its macro name is not known yet (`docs/data/asserts.txt`
  shows the expressions). Reproduce each assert where the binary has it, with the expression
  exactly as stringified in the client.
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

The project builds with a modern MSVC too, so the code must also be valid standard C++ (the
`for`-scope rule above is why). Where VC6 and the standard disagree, use a macro in a
compatibility header, not `#ifdef`s in every file.

## 4. File banner template

The repository is GPL v3 (decision D1, [progress.md](progress.md)), hence the licence lines.

```cpp
/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_login.cpp
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Earth & Beyond                                               *
 *                                                                                             *
 *                    File Name : ui_login.cpp                                                 *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
```

Files imported from EA's repositories keep their original banners and licence header unchanged;
we add a line saying which EA copy (Renegade, Generals, Zero Hour) and commit it came from, and
note any change made to match the client.

* **SoundLibraryBridge (2026-10-27, H9):** the client's bridge vtable 0xAE74EC has two slots; the tree's `ww3d2/soundlibrarybridge.h` declares three pure virtuals (`Stop_Playing_Audio` is Zero Hour's). app/audio/soundlib.cpp implements the third as an empty slot; ww3d2 stays as it is (no pair depends on it).

* **ControlKeysManager (2026-10-27, H11):** the RTTI names it `ControlKeysManager` (no `Class` suffix); ui_line_display.cpp's `ControlKeysManagerClass` (NAME 3) and its Unknown_71A960 / 71AB40 (Add_Keys / Remove_Keys), sclient.cpp's Unknown_719D50 / 719E10 (Create / Destroy) switch to control_keys_manager.h's names at their next edit, once SClient replaces the file's ControlKeysSClientClass stand-in (P-H11-1).

* **ClientObjectClass (2026-10-27, H13):** declared in app/client/co.h (vtable 0xB0070C, constructor 0x6F2820, destructor 0x6F2FE0). aux_roots.h's `UnknownCOClass` (the factory 0x6F6000) is the same class: merge it into co.h at aux_roots.h's next edit (P-H13-2). co_data's Remove_All 0x79D390 keeps a dead `Get_Game_ID()` call (the D40 budget lever, P-H13-1).

* **FontDatabaseClass (2026-10-27, H12):** app/client/fontdatabase.h; the globals 0xBE67B8 / 0xBE67BC are declared under stand-in names in tsk_game.h, sclient.cpp and ui_line_display.cpp: point them at fontdatabase.h at those files' next edit (P-H12-3).

* **ClientObjectClass's render object (2026-10-27, T11):** co.h names the member `Model`; co.cpp's assert texts say `RenderObject`, the client's name. Rename at cpacket.cpp's next edit (it and clienttest use it; P-T11-1). The aux_roots.h UnknownCOClass merge waits for the CREATE factory to move into co.cpp (aux_roots.cpp is generated; P-T11-2).
