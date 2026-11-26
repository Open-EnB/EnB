# Tier 2 name proposals (Earth & Beyond client.exe, Westwood, 2002, MSVC 6)

We are recovering the original C++ names of classes and methods in the game client. A hash oracle checks each
proposal exactly (one hash per name against the linker's symbol order), so wrong guesses cost nothing but a wasted
line; a right guess is confirmed. Breadth matters more than certainty: give your best several guesses.

For each class context file you are given (`.work/names/tier2/ctx/<CLASS>.txt`), propose:
1. Up to 10 candidate **class names** for the retail class (most likely first). Our name is often a placeholder
   (Unknown..., UI...Class) or a near miss.
2. For up to 15 of its methods named `Unknown_XX`, up to 3 candidate **method names** each (just the identifier).

Retail naming style seen so far (confirmed names):
- Westwood C++ style: classes `FooClass`, `FooStruct`, `FooManagerClass`; methods `Get_Value`, `Set_Position`,
  `Is_Visible`, `Process_Packet`, `Render`, `Update`, `Init`, `Shutdown`, `On_Enter`, `Run_One_Cycle`.
- Not everything has the suffix: `GuildDeductFeePacket`, `LinearBuffer`, `BitSet`, `SoundLibrary`, `CordChannel`.
- Our `UI<X>Class` interface screens are retail `<X>Interface`: UIGroupClass -> `GroupInterface`, UIScanClass ->
  `ScannerInterface`; `StarbaseAvatarCreationInterface` exists. Our `SClient` is `SpaceClientClass`,
  `ProjectorManagerClass` is `ProjectionManagerClass`, `MsgServer` is `MessageServerClass`,
  `CharacterViewAvatarClass` is partly `AvatarClass`.
- Source file names (in the context) often give the class name (ui_manu.cpp -> a manufacturing interface).

Output: write one TSV file (the path is given in your task), no header, one proposal per line:

    CLASS<TAB><our class name><TAB><candidate class name>
    METHOD<TAB><our class name><TAB><our decorated name exactly as listed><TAB><candidate method name>

Use only the Read tool on the context files and the Write tool for the output file; no other tools, no searching.
Reply with one line: the number of CLASS and METHOD lines written.
