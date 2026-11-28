# Tier 2 class-name proposals (Earth & Beyond client.exe, Westwood, 2002, MSVC 6)

We are recovering the original C++ class names in the game client. A hash oracle checks each proposal exactly
(the linker's symbol order), so a wrong guess costs nothing and a right one is confirmed. Breadth matters more than
certainty: give many plausible guesses per class.

Your batch file holds about 20 classes, each starting with `===== <our class name> =====`, with our decorated
method names (MSVC mangling), source files from asserts, retail-exact classes in the same files, strings, and a
pseudocode excerpt. Our class name is a guess; retail's often differs by wording, abbreviation or suffix.

For each class propose up to **15 candidate class names**, most likely first. Include our own name's close
variants only if they differ (spelling out abbreviations, adding / dropping `Class`, `Struct`, `Manager`,
`Interface`, `RenderObj`, `Object`), and names suggested by the source file, strings and sibling classes.

Retail naming style (confirmed): Westwood C++ `FooClass`, `FooStruct`, `FooManagerClass`; some have no suffix
(`GuildDeductFeePacket`, `LinearBuffer`, `BitSet`, `SoundLibrary`, `CordChannel`, `FontDatabase`?). Abbreviations
are often spelled out in retail: `ProxyRObj` -> `ProxyRenderObjClass`, `SClient` -> `SpaceClientClass`,
`MsgServer` -> `MessageServerClass`, `ProjectorManagerClass` -> `ProjectionManagerClass`, `UIGroupClass` ->
`GroupInterface`, `UIScanClass` -> `ScannerInterface`, `ClientSoundPacketClass` -> `SoundPacketClass`,
`Packet_00A8Class` -> `ChatProxyLoginPacketClass`.

Output: write one TSV file (path in your task), no header, one line per candidate:

    CLASS<TAB><our class name><TAB><candidate class name>

Use only the Read tool (on your batch file) and the Write tool (for the output). No other tools.
Reply with one line: the number of classes covered and lines written.
