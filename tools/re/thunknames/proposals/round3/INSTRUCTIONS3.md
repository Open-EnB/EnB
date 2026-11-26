# Round 3 class-name proposals (Earth & Beyond client.exe, Westwood, 2002, MSVC 6)

We recover the original C++ class names of the game client. A hash oracle checks every proposal exactly against
the linker's symbol order, chiefly on the DESTRUCTOR SLOTS, whose signature the compiler fixes, so the class name is
the only unknown there. A wrong guess costs nothing; a right one is confirmed. Give breadth.

Your batch file holds about 26 classes, each starting with `===== <our class name> =====`. For each class you get:
the destructor slots, our decorated method names, every name ALREADY TRIED (all wrong: do not repeat them or trivial
variants of them), confirmed retail classes with the same name start, in its vtable family, and in its source files
(these show retail's actual spelling style), source files and strings.

Propose up to **20 new candidate class names** per class, most likely first. Think about:
- the source file name (dynamic_window_vertical.cpp, gd_text_editor.cpp -> TextEditorGadget): retail names often
  follow the file name, spelled in the family's style;
- the family's style: whether siblings carry `Class` (DynamicWindowClass) or not (DynamicWindowElement,
  TextEditorGadget, LinearBuffer, BitSet), `Struct`, `Interface` (UI screens are `<X>Interface`: MFDInterface,
  VerbInterface, ShortcutInterface), `Packet` / `PacketClass`, `Manager` / `ManagerClass`, `Data`, `Node`, `RObj`;
- abbreviations spelled out or kept (ProxyRObj -> ProxyRenderObjClass, Gps -> GPS, Inv -> Inventory, CircBuff ->
  CircularBuffer, Rect3D -> Rectangle3D) and reordered words (InventorySortPacket -> InvSortPacketClass,
  KVPairString -> KVStringPairClass, MissionDismissal -> MissionDismiss);
- a placeholder name (Unknown..., Packet_XXXX) says nothing: use the methods, strings, file and relatives.

Output: write one TSV file (path in your task), no header, one line per candidate:

    CLASS<TAB><our class name><TAB><candidate class name>

Use only the Read tool (on your batch file) and the Write tool (for the output). No other tools.
Reply with one line: the number of classes covered and lines written.
