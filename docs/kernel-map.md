# The client kernel, its tasks and the packet layer: a map

The next item in D6's order after the milestones: `app\kernel`, `app\main`, `app\client\tasks`
(the program's skeleton), with the packet layer between the transports and the packet handlers
(`app\common`). Mapped read-only on 2026-10-24: classes, vtables, functions, the call structure,
and a reconstruction order (§6), adopted as D68 (extends D6). Since then steps 1-5 and 7 are done
or partly done (§6's rows): kernel.cpp, bandwidthaccounting.cpp, the connection layer (circbuff.cpp,
connect.cpp's base and stream connection, udpconn.cpp), the packet factory with its switch over
263 generated classes, MenuTask and the movie tasks (`git log --oneline` from b5f5c26).

Evidence tags: **S** a client string naming it (profile label, log text, assert, `__FILE__`),
**C** what the code does (a descriptive name, not the client's), **N** the maintainer's earlier,
unpublished protocol research ([prior-work.md](prior-work.md)); its findings are restated here where used. Sizes are to the next verified function start (`appfile.py show`).
Addresses are the retail client.exe's (SHA-256 `249f8524…51c4`); the tools that give them:
`appfile.py show FILE`, `client.py dis|callers|refs`, `pseudocode.py --show`; vtable slot dumps
below came from a small reader over `clientimage.py` (follows the incremental-link thunks).

## 1. From WinMain to the task loop

`WinMain` is the only function attributed to `app\main\main_client.cpp`: **0x526DE0**, 0xBDD bytes
(S: `__FILE__` in its assert, line 596). Its steps are logged through dbgmgr.cpp (0x509EF0 opens a
step with the text, 0x50A1E0 closes it), so the order is in the strings:

| Step (the client's log text) | Calls (selection) |
|---|---|
| command line, `-PURCHASE`, the `enb_mutex_lock` mutex ("Earth & Beyond is already running."), the EULA / ROC texts | ArgvClass, 0x5FADE0 / 0x5FAD50 (ui_registration.cpp) |
| "Main Thread": `Register_Thread_ID`; `DisableExceptions` (HKLM `SOFTWARE\Westwood Studios\Earth and Beyond`) | |
| file factories (`..\data\client\mixfiles\`, `..\data\client\output\`), WWMath::Init, "Build: %s %s %s" | 0x4E4D10 / 0x4E5230 (combo_ffactory.cpp) |
| "initializing the asset manager", "initializing WW3D" (`-MENU`, `-TEST_PATH`) | 0x48B070, WW3D::Init |
| "starting message server" (`-ENABLE_MESSAGES`) | 0x8B20D0 / 0x8B2230 (app\shared\message\msgserv.cpp): a task, run once at once (slot +0x30) |
| "initializing input handler", "starting winsock services", "initializing render state class", "display handler", "options manager", "sound library", "sound manager" | 0x42E4A0 / 0x42EE00 (input.cpp), 0x4581E0, 0x442AC0 (display.cpp), 0x43E8D0 (soundlib.cpp), 0x75AB10 |
| **the kernel**: `new KernelClass` (0x443100); the intro movie task (0x76BF40, `..//data//client//mixfiles//eb_sizzle.bik` via 0x76C2C0) added and given the focus (Add_Task 0x443380, Set_Focus 0x443CD0) | |
| "adding message server to kernel" (with `-ENABLE_MESSAGES`) | Add_Task |
| "starting up font system", the scripted movie (`moviescript1.txt`, `moviescript`: 0x76C670 + 0x76C8B0, added), "Personality manager", "Buff Database", "Tutorial Manager", "starbase lights manager", DX8WebBrowser::Initialize | 0x6BE5C0, 0x6D16E0, 0x6C4C80, 0x790560, 0x676DB0, 0x6AC600, 0x764480 |
| "starting menu task": MenuTask (0x76BC50) added; LoginTask (0x7675D0) added and given the focus | Add_Task, Set_Focus |
| "Server initialized & ready to go" (0x4BC000), then **KernelClass::Run 0x4436C0** until the kernel stops | |
| "shutting down kernel" (`~KernelClass` 0x4431C0, delete), then every subsystem in reverse ("freeing assets", "shutting down winsock services", ...), `Unregister_Thread_ID` | 0x82DF60 (udpconn.cpp) last |

The loop (all in kernel.cpp):

```
KernelClass::Run 0x4436C0:  while (Process_Messages() && Run_One_Cycle() && Is_Running()) {}
    Process_Messages 0x443710 (vtable +0x08): PeekMessage / GetMessage / Translate / Dispatch;
        WM_QUIT would set +0x0C, but the pump (0x443E90, inlined) tests for it only after
        GetMessage returned nonzero, which it never does for WM_QUIT: a posted quit doesn't
        stop the kernel (tests set `Quit` directly); then the input queue (0x42EA80 takes an event, kernel +0x14 Handle_Key
        0x443E20 first: Esc with no task focused shuts down, key 0x6A cycles the focus; else 0x42E8A0
        hands it on)
    Run_One_Cycle 0x443820 (+0x0C, S 'KernelClass::Run_One_Cycle'):
        now = 0x4A3B40 (the ms clock: auxdata's unknown_root.cpp)
        sleep = min over tasks of (task->Last_Run + task->Get_Interval() - now); Sleep(sleep) if > 0
            (S 'Kernel::Sleep')
        for each task in the list (+0x14) whose Last_Run + Get_Interval() <= now:
            Last_Run = now
            the focused task (+0x04): Pre_Run (+0x38), Run (+0x30, S 'Current_Task->Run()'), then,
                when rendering is on (+0x0D) and WW3D's device check 0x9179D0 returns 0 (+0x0E),
                Render (+0x3C, S 'Current_Task->Render()')
            any other task: Run (S 'Background_Task->Run()')
            a task removed while running (+0x08): Shutdown (+0x40) and delete
                (S 'Pending_Delete->Shutdown()')
    Is_Running 0x443B90 (+0x10): false once `Quit` (+0x0C) is set, else the task list isn't empty
```

## 2. KernelClass (app\kernel\kernel.cpp)

One instance (S: assert `Instance == NULL`, line 74; the pointer at **0xBDADBC**). 0x24 bytes,
vtable **0xAE766C** (6 slots, no virtual destructor):

| Offset | Member | Evidence |
|---|---|---|
| +0x00 | vtable 0xAE766C | |
| +0x04 | `Current_Task`: the focused task | S (profile label) |
| +0x08 | `Pending_Delete`: a task removed while it was running | S (assert `!Pending_Delete`, label) |
| +0x0C | bool `Quit`: meant for WM_QUIT, never set by it in practice (§1) | C |
| +0x0D | bool: render the focused task | C |
| +0x0E | bool: the last device check passed | C |
| +0x10 | byte (constructor) | |
| +0x14 | `std::list<Task *>` head node; +0x18 its size | C (VC6 list: next, prev, value) |
| +0x1C | the task being run | C |
| +0x20 | the list iterator Run_One_Cycle walks | C |

| Slot | Address | Size | Name | Evidence |
|---|---|---|---|---|
| +0x00 | 0x443270 | 0x14 | Cycle_Focus (Next_Focus, then a flag at [0xBDADC0]+0x4074) | C |
| +0x04 | 0x4432A0 | 0xA3 | Shutdown: the focused task yields, every task Shutdown (+0x40) and, if Delete_On_Remove, deleted; then a heap check (`_heapchk`, DebugBreak) | C |
| +0x08 | 0x443710 | 0xCA | Process_Messages | C |
| +0x0C | 0x443820 | 0x2BA | Run_One_Cycle | S |
| +0x10 | 0x443B90 | 0x13 | Is_Running (no `Quit`, tasks left) | C |
| +0x14 | 0x443E20 | 0x50 | Handle_Key (Esc, 0x6A) | C |

Other functions (kernel.cpp, 0x443020-0x444150):

| Address | Size | Name | Evidence |
|---|---|---|---|
| 0x443100 | 0x8D | KernelClass::KernelClass | S (`Instance == NULL`) |
| 0x4431C0 | 0x63 | KernelClass::~KernelClass | C |
| 0x443250 | 0x6 | Get_Instance (static: returns 0xBDADBC) | C |
| 0x443380 | 0x9B | Add_Task(task): appends, sets task->Kernel; focuses it if none is focused and it accepts the focus; assert `false` (line 157) when already in | C, S (assert) |
| 0x443450 | 0x22 | Contains_Task | S (assert text `Contains_Task(task)`) |
| 0x443490 | 0x145 | Remove_Task(task): yields its focus, unlinks it, Shutdown + delete if Delete_On_Remove (deferred to Pending_Delete when it is the running task); assert `false` (196) | C, S |
| 0x443640 | 0x28 | Get_Task(index) | C |
| 0x443680 | 0x30 | (unlinks the iterator's node) | C |
| 0x4436C0 | 0x31 | Run | C |
| 0x443BC0 | 0x13 | Is_Focus(task) | C |
| 0x443BF0 | 0x4 | Get_Focus | C |
| 0x443C10 | 0x84 | Next_Focus: the focused task yields, the next one that accepts it gets it | C |
| 0x443CD0 | 0x7A | Set_Focus(task) (assert `Contains_Task(task)`, line 474) | S, C |
| 0x443D90 | 0x68 | the input step of Process_Messages without the message pump | C |
| 0x443E90, 0x443F30 | 0x77, 0x4D | (list helpers) | |

## 3. The task base class

No RTTI and no string names it; the tasks' own strings call them `ClientTask`, `LoginTask`,
`MenuTask` (S) and the asserts `LoginTaskClass` (S). Constructor **0x443050** (vtable, +0x08 = 0),
destructor 0x4430A0, scalar deleting destructor 0x443070; 0x443020 Is_Focused (the kernel's
+0x04 is this), 0x4430C0 Set_Kernel. Layout: +0x00 vtable, **+0x04 the kernel** (Add_Task sets
it), **+0x08 Last_Run** (ms). Vtable **0xAE7614**, 18 slots; every task's vtable starts with these:

| Slot | Default | Name | Evidence |
|---|---|---|---|
| +0x00 | 0x4430E0 `ret 4` | Describe(table): the message server writes `TaskType = MessageServer` | C |
| +0x04 | 0x443070 | scalar deleting destructor | C |
| +0x08 .. +0x24 | 0x443FA0 .. 0x444080 (`return 0`) | eight input handlers (the input queue's events) | C |
| +0x28 | 0x4440A0 (false) | Accept_Focus | S ('LoginTask: accepting focus.') |
| +0x2C | 0x4440C0 (true) | Yield_Focus | S ('... yielding focus.', 'F* that! Not yielding focus.') |
| +0x30 | 0x4440E0 | Run | S ('ClientTask::Run', 'LoginTask::Run', 'CordMgr::Run') |
| +0x34 | 0x9CCD10 (0) | Get_Interval: ms between runs (the UDP task: 100) | C |
| +0x38 | 0x444100 | Pre_Run (the focused task only, before Run) | C |
| +0x3C | 0x444120 | Render | S (label) |
| +0x40 | 0x444140 (true) | Shutdown | S (label) |
| +0x44 | 0x9CCD10 (false) | Delete_On_Remove: the kernel deletes it | C |

The task classes (every vtable in .rdata that starts with the base's slot 0 and has the base's
layout; overridden slots listed):

| Class | Vtable | File | Constructor | Overrides | Role |
|---|---|---|---|---|---|
| ClientTask (S) | 0xB03FF4 | tsk_game.cpp (18 functions, 0xAF4 bytes) | 0x766350 (assert `kernel && ticket`) | +04 0x766190, +0C 0x766150, +28 0x7665A0, +2C 0x7665F0, +30 Run 0x766630, +34 0x7671E0, +3C 0x766F10, +40 0x766F80, +44 0x766170 | the game: modes `MODE_JOIN` (0x7667F0: the `Master_Connection`, connect.cpp 0x821B40 / 0x821000 / 0x821400), `MODE_WAIT_FOR_REDIRECT` (0x7669F0, 0x766AB0: client.cpp 0x727F00 / 0x728150), `MODE_SECTOR` (0x766BD0); a redirect (sector change) makes a new ClientTask (0x7670A0, `RedirectCount <= 50`), a lost connection a new LoginTask |
| LoginTask / LoginTaskClass (S) | 0xB040D4 (29 slots to +0x70) | tsk_login.cpp (54, 13,768 bytes) | 0x7675D0 (0x36E) | +04, +08, +28, +2C, +30 Run 0x767F50, +34, +38, +3C, +44; own: +48 0x7689E0 (`Galaxies.empty()`), +4C 0x768B00 (the ticket step, N), +50 0x7683F0, +54 Go_Next_Step 0x769500 (S), +58 Go_Back_Step 0x769680 (S), +5C .. +70 | login, galaxy list, character select and creation; Go_Next_Step's state 5 makes the ClientTask (0x769E70 calls 0x766350) and removes itself ('LoginTask: removing self.'); the version / ticket / character-list replies arrive through static handlers 0x76B4A0-0x76BBF0 (asserts `task`, '!"Unhandled status in version response packet"') |
| MenuTask (S) | 0xB04228 | between tsk_login.cpp and tsk_movie.cpp, no `__FILE__` (0x76BC50-0x76BF30) | 0x76BC50 | +04, +28 0x76BD10, +2C 0x76BD30, +34, +38 0x76BD70 (hands event 0x43 to its handler 0x76BDB0), +44 | 'MenuTask: Starting Login Task.' (the handler 0x76BDB0 makes a LoginTask), 'MenuTask: killing kernel.' |
| the movie task | 0xB04280 | tsk_movie.cpp (5, 279 bytes; the class 0x76BF40-0x76C5A0) | 0x76BF40; the movie 0x76C2C0 (assert `!movie_name`) | +04, +28, +2C, +30 0x76C430 (removes itself at the end), +34, +38 0x76C4D0 (skip), +3C 0x76C230 (WW3D::Begin_Render / End_Render), +44 | the intro movie (Bink: 0xA16xxx) |
| the scripted movie task | 0xB04360 | tsk_smovie.cpp (19, 1,731 bytes) | 0x76C670 (asserts `moviename`), load 0x76C8B0 (`kernel == NULL`) | +04, +18, +28 .. +44 | `moviescript1.txt`; its own clients (`Clients.size() == 0`, mclient.cpp, client_lego_impl.cpp) |
| the message server | 0xB0C418 | app\shared\message\msgserv.cpp | 0x8B20D0 | +00 Describe ('TaskType' 'MessageServer'), +04, +30 0x8B22C0, +34, +40, +44 | `-ENABLE_MESSAGES` only ('Task Hello', 'Task Ping' ...) |
| the UDP connection task | 0xB0889C | app\common\udpconn.cpp | (§5) | +04 0x82DD70, +30 Run 0x82DF90 (S 'CordMgr::Run'), +34 0x82E020 (100 ms), +44 0x82DE30 | pumps CordMgr (§5) |
| the bandwidth accounting | 0xB08CF0 | app\wwudp\bandwidthaccounting.cpp | 0x84C760 (adds itself: Get_Instance, Add_Task; registers the wnet callback 0x84C8B0 with 0x8C05E0) | +04 0x84C800, +30 0x84C970 (the rates), +34 0x84C950, +44 0x84C7E0 (false) | per-IP byte and packet counts (0x84C8B0 counts each datagram +28 header bytes; a DynamicVectorClass of 0x3C-byte entries at 0xBF4048, qsorted by 0x84D150); printed by 0x84CC50 / 0x84CB80 ('Bandwidth Accounting', 'Bandwidth Accounting Per IP') |

The task flow: intro movie (focused) -> scripted movie -> MenuTask -> LoginTask (focused at
start-up) -> ClientTask (join, redirects, sector) -> back to a new LoginTask when the connection
drops; the UDP task and the bandwidth accounting run in the background.

Correction to the earlier research (N): LoginTaskClass's vtable starts at **0xB040D4**, not
0xB040CC (0xB040CC holds two zero dwords before it). Its slot offsets are 8 lower than those notes
give: the ticket step 0x768B00 is +0x4C (notes: +0x54), Go_Next_Step 0x769500 +0x54 (notes:
+0x5C). The login screen's "vfunc 0x4C" (asks the task to log in) is this +0x4C.

## 4. The connection classes (app\common: connect.h, connect.cpp, udpconn.cpp, tcpconn.cpp)

None of these classes has RTTI; the names below are descriptive (C) unless marked S. A shared
`return 0` (0x9CCD10: `xor eax, eax; ret`) fills several base slots.

**The connection base** (connect.h): vtable **0xB0850C**, constructor 0x820F90 (inlined: vtable,
+0x04 a bool = 1), scalar deleting destructor 0x820FB0. Its slots 4-12 are small defaults at
0x726AF0-0x726BF0 that app-functions.json puts in client.cpp (inline header virtuals emitted
there). The slots, from what the UDP and TCP classes put in them:

| Slot | Name (C) | UDP (0xB088F4) | stream / TCP |
|---|---|---|---|
| +0x00 | destructor | | 0x8233B0 |
| +0x04 | Send(raw message) | 0x82D980 (assert `packet->Header.Size > UDP_DATABUFFER_SIZE`, l.489) | 0x823570 (header, then payload, through slot +0x34) |
| +0x08 | Send(packet) | 0x82DA20 (the packet's slots +0x18..+0x24 give the channel and flags) | 0x8234D0 (packet slot +0x04 Build, then +0x04) |
| +0x0C | Receive() -> packet | 0x82DB30 | 0x8236D0 |
| +0x10 | (an id: the UDP CordCon, TCP 0x8299D0) | 0x82D190 | 0x8299D0 |
| +0x14 | Connect(wait) | 0x82D1B0 | 0x828F90 |
| +0x18 | Is_Connected | 0x82D320 (CordCon 0x8501D0: state 4; then Disconnect) | 0x829930 |
| +0x1C | Disconnect | 0x82D3B0 (CordMgr 0x8595B0, Release 0x8596F0) | 0x829960 |
| +0x20 | Is_Dead | 0x82D390 | 0x8238C0 (closed, nothing buffered) |
| +0x24, +0x28 | Set_Flag / Get_Flag (+0x04) | | |
| +0x2C | Get_Stats (7 out parameters) | 0x82D060 (CordCon 0x84FDE0) | |
| +0x30 | (2 arguments, false) | | |
| +0x34, +0x38 | (stream only) Transport_Write / Transport_Read | | TCP 0x829A30 (RC4, TCPCon::write 0x8BBF00) / 0x829BC0 (TCPCon::read 0x8BC280, RC4) |

The raw message is 0x22C bytes: {s16 size including the 4-byte header, s16 opcode,
LinearBufferClass at +0x04} (destructor 0x823550, connect.cpp).

* **The stream connection** (connect.cpp 0x823320-0x8238C0, vtable **0xB085F4**, constructor
  0x823320): a 100,000-byte CircBuff (app\common\circbuff.cpp 0x830BF0) and a 0x1000-byte read
  buffer at +0x20. Receive 0x8236D0 reads the transport into the ring (assert
  `bytes_written == bytes_from_transport`, l.1138), takes the 4-byte header (size < 4:
  Disconnect), waits for the whole message, copies it out (`readbytes == bytes`, l.1104), calls
  the factory 0x8696C0, frees the copy, sets the packet's +0x04 to the connection.
* **Wrappers** (connect.cpp / connect.h): a forwarding wrapper (vtable 0xB08590, the inner
  connection at +0x08, slots 0x821BB0-0x821E40); the peek connection (vtable 0xB0854C,
  constructor 0x821C30, assert `connection` connect.h l.74, 0x10 bytes; slot +0x34 Peek 0x821FB0
  keeps the last packet at +0x0C, Receive 0x821FF0 = Peek then clear, assert `Last_Packet`
  l.470); two logging wrappers with `-PACKET_STATS` (vtables 0xB086C4 / 0xB0870C, constructors
  0x823F70 / 0x824540; 'netlog', 'LogPath', `c:\temp\glog`; 'Sent' / 'Received', the second with a
  timestamp); a statistics wrapper 0x824A90 (two average.h samples) and 0x8250B0.
* **ConnectionManager** (S: 'ConnectionManager: probing port %hd...'; vtable 0xB085D0, constructor
  0x822070): connections with a flag each (+0x08/+0x0C), round robin (+0x14), the current one
  (+0x18), a listener (+0x24); Peek 0x8223D0, Get_Packet 0x822490 (assert
  `retval == testpacket`), Receive 0x822500, Broadcast 0x822550, Mark 0x8225C0, Clear_Marks
  0x822630, Accept_New 0x822300, Add_Connection 0x822CC0. Mostly the server's.
* **The factories**: Create_Connection **0x821000**(server type, flag) reads network.ini
  (MasterServer, ChatServer, GlobalServer_*, SectorServer ...; `-SERVER_ADDR`, `-GLOBAL_ADDR`) and
  calls **0x821450** (0x357 bytes): `-PROTOCOL` picks the transport (MEMORY: memconn.cpp
  0x8277C0 / 0x827A80; UDP: `new` 0xD0 -> 0x82C680; otherwise TCP, `new` 0x248 -> 0x828C80, and
  ChatServer and GlobalServer_Client are always TCP), then the optional wrappers (asserts
  `new_connection` l.196 / 209 / 223 / 230; 'connecting to %s on port %d'). 0x821400: the same for
  127.0.0.1. Callers: ClientTask's MODE_JOIN 0x7668E0 (the `Master_Connection`), LoginTask
  0x7698E0 (`GlobalConnection`), client.cpp 0x727E10 (SectorServer).
* **UDPConnection** (udpconn.cpp, 41 functions, 6,025 bytes; vtable **0xB088F4**, 0xD0 bytes):
  constructors 0x82C680 (by host name), 0x82C980 (by address), 0x82CBF0 (an accepted CordCon);
  destructor 0x82C950 / 0x82CDC0 (CordMgr::Release). Members: a channel hash (+0x08..+0x20),
  two statistics objects, byte counters (+0x68 / +0x6C), the CordMgr (+0x74), the CordCon (+0x78),
  16 channel ids (+0x80..+0xBC), the default channel (+0xC0), name / IP / port (+0xC4..+0xCC).
  Try_Connect 0x82D240: CordMgr::Connect 0x858EC0, Pump 0x857F20, 0x859440, then openc on the
  default channel (mode 3) and CordStatsClass::Set_Rate_Limit(0x10000). The send core 0x82D420
  (0x375 bytes) opens channels lazily (openc mode 3 reliable / 4 sequenced; assert `open_ok`
  l.391) and calls CordCon::write. Every CordCon / CordMgr call is bracketed by wlib's critical
  section (0x8B4330 / 0x8B4350). **One CordMgr per process**: 0x82DE50 makes it (0x95C bytes,
  constructor 0x856970; globals 0xBF16C0 / 0xBF16BC) and the 12-byte pump task (vtable
  0xB0889C, global 0xBF16C4): Run 0x82DF90 (S 'CordMgr::Run') calls CordMgr::Pump every 100 ms
  (Get_Interval 0x82E020). UDPConnectionManager (vtable 0xB08934, constructor 0x82E040) and the
  UDP listener (vtable 0xB08958, constructor 0x82E200; Listen 0x82E470, Stop 0x82E530, Accept
  0x82E5B0) are the server's side.
* **TCPConnection** (tcpconn.cpp, 25 functions, 5,350 bytes; vtable **0xB08838**, 0x248 bytes, a
  stream connection with two RC4Class): constructors 0x828C80 (host), 0x828DE0 (address),
  **0x829C80** (an accepted socket, assert `socket != INVALID_SOCKET` l.564: symbols.json's
  `PacketManagerClass::Clear_Socket_Error` there is wrong); Connect 0x828F90 (TCPMgr::connect
  0x8BD680, getOutgoingConnection 0x8BD8E0, the key exchange 0x829180); a global TCPMgr (static
  init 0x828A10 -> 0x8BCA50). WWOnline wnet's TCPMgr / TCPCon (0x8BB0C0-0x8BF1C0, with
  tcpqueue.cpp's 4) is called only from TCPCon, never from tcpconn.cpp directly.

## 5. From a datagram to its handler

```
UDP:  [kernel] the CordMgr task's Run 0x82DF90 -> CordMgr::Pump 0x857F20 -> Process_CORD_Packet
      -> CordCon::handlePacket (app\wwudp, rebuilt)
      UDPConnection::Receive 0x82DB30 -> CordCon::read 0x850FD0 (len 0x8000, buffer 0xBE96B8)
TCP:  stream Receive 0x8236D0 -> Transport_Read 0x829BC0 -> TCPCon::read 0x8BC280 + RC4 -> ring
both: -> the factory 0x8696C0 (a LinearBufferClass over the payload: linearbu.cpp, in the tree)
      -> 0x869790 (pk.cpp, 15,995 bytes: a switch on the s16 opcode; each case
         `new XxxPacketClass(LinearBuffer &)`; assert l.1078 for an unknown opcode)
      -> packet->+0x04 = the connection
read loop: SClient::Process_Packets 0x7427C0 (S, 0xE1 bytes): its connection (SClient +0x1124) is
      a peek connection over a statistics wrapper over Create_Connection("SectorServer", 1)
      (client.cpp 0x727E10):
          while ((p = conn->Peek()) != 0) {
              if (!p->Process_Packet(this)) break;     // false: stays queued for the next frame
              delete conn->Receive();
          }
```

**The packet base class** (pk.h; asserts pk.h l.483 / 490 / 496 / 502), vtable **0xAEDE88**, 10
slots; layout +0x00 vtable, +0x04 the source connection, +0x08 a serial number (the global
0xBF4128): +0x00 destructor 0x52B350, +0x04 Build -> raw message, **+0x08 Process_Packet(SClient
*)** (base 0x52B240 asserts), +0x0C Get_Data_Size, +0x10 Print, +0x14 0x86F010 (assert pk.cpp
l.1344), +0x18..+0x24 the UDP send parameters (channel -1, 0, true, false). **263 opcodes** (262
cases, 259 constructors): 0-237 (57 absent), 5000-5011 (10), 30000-30027 (16); the names agree
with the Net-7 opcode table (0 VERSION_REQUEST: 'VersionRequest { Major=%d Minor=%d }';
4 CREATE). The constructors and Build/Print/size slots live at 0x85C5F0-0x8B1EB0 (app\packets
pk*.cpp, gameedit\pkge*.cpp; mostly unattributed). Process_Packet by file: app\client\cpacket.cpp
99 (+4 unattributed neighbours; 97 with a profile label 'X::Process_Packet()'),
app\packets\clientstub.cpp 149 (0x19-byte `assert; return true` for server-only packets, one per
line 14, 17, 20 ...), tsk_login.cpp 6 (opcodes 1, 111, 112, 117, 118, 120), tsk_game.cpp 1 (54),
and 32-34's vtables hold cpacket.cpp's message-line handlers 0x736A20 / 0x736AE0 / 0x736BA0, not the base's
(correction from PF1, 2026-10-28: 107 + 149 + 6 + 1 = 263; [packet-flow.md](packet-flow.md)).

Not traced yet: who calls SClient::Process_Packets (a vtable or pointer), the login and game
tasks' own read loops, the exact roles of the connection slots +0x10 / +0x2C / +0x30 and the
packet slots +0x14..+0x24; 0x856970 (CordMgr's constructor) lies before cordmgr.cpp's recorded
range 0x857590.

## 6. Proposed reconstruction order

Sizes: functions attributed or in the file's window, bytes from `appfile.py show` (to the next
verified start). Each step names what it needs that isn't in the tree.

| # | What | Size | Needs | Test |
|---|---|---|---|---|
| 1 | **kernel.cpp**: KernelClass and the task base (§2, §3), the 14 one-line default slots 0x443FA0-0x444140 (**done** 2026-10-24: 18 functions exact; Remove_Task, Is_Running, TaskClass::Is_Focused functional, the one-line defaults too short to search for; `tools/harness/kerneltest.py`: 35 of 35) | ~45 functions, ~4.4 KB (0x443020-0x444150) | the ms clock 0x4A3B40 (auxdata's unknown_root.cpp, in the tree), the input queue 0x42EA80 / 0x42E8A0 (input.cpp: declare), WW3D's device check 0x9179D0 (WW3D, in the tree?) | a program with dummy tasks: focus, intervals, Pending_Delete, Shutdown order |
| 2 | **bandwidthaccounting.cpp** (M1.4's last file; **done** 2026-10-24, exact, on kernel.h's declared base): the task 0x84C760-0x84CC50, the per-IP table 0x84D150-0x84D730, the `DynamicVectorClass<entry>` instantiation 0x84D960-0x84DF00 (WWLib's vector.h) | ~30 functions, ~4.4 KB (0x84C720-0x84DFB0) | (1); the wnet hook 0x8C05E0 / 0x8C0600 (UDP::Write / Read call it: `(out, len, ip, port)`); E&B's debug print 0x4BC460 (cdecl: window, 0xBDA908, `__FILE__`, `__LINE__`, column, row, format; lines 281-282, 310-361) | udpreplay counting its datagrams |
| 3 | connect.h (the base, the wrappers, the peek connection) and the stream half of connect.cpp, circbuff.cpp (**partly done** 2026-10-24, 8a6066b: circbuff.cpp all 7 exact; the base's constructor / destructor and the stream connection 0x823320-0x8238C0 all exact (the destructor since 1ea47fd); the wrappers, the peek connection, ConnectionManager and the factories not yet; app/packets/pk.h declares the packet base, the header, the raw message and the factory) | 11 + ~10 + ~8 functions, ~3.6 KB | INIClass (in the tree) | a memory transport |
| 4 | **udpconn.cpp**: UDPConnection, the CordMgr task, the listener (**partly done** 2026-10-24, 89d1db0 / 32d336c: UDPConnection by address and over a CordCon, its slots, the send core and the pump task, 17 exact with the CordCon / CordMgr members it calls added to app/wwudp (5938ed4: getStats and closec exact), the rest functional on a wlib Dictionary stand-in and CritSec / rate declarations; the constructor by name 0x82C680, UDPConnectionManager and the listener not yet; tested: `udpreplay.py conn`) | 41, 6.0 KB | (1), (3), app\wwudp (in the tree), wlib's critical section 0x8B42F0-0x8B4350 and HashTemplate (wlib: open decision; stand-ins as for Wtime) | udpreplay's loop test through UDPConnection::Send / Receive with a stub factory |
| 5 | pk.h's base and pk.cpp's helpers; the factory 0x869790 with the 259 classes declared (cases: sizes and constructors) (**partly done** 2026-10-24, 1ea47fd and after: the factory 0x8696C0 exact, pk.h's base with its inline defaults on lines 483-502, the raw message's helpers 0x86E870-0x86EFE0 and slot +0x14 0x86F010 in pk.cpp, all exact; the switch 0x869790 exact, generated by tools/re/pkclasses.py over app/packets/pk_classes.h's 263 declared classes (their constructors and members not rebuilt; the harness's udpreplay conn runs every Alex message through it); the map: docs/data/packet-factory.txt (tools/re/pkfactory.py: 263 opcodes, their sizes, constructors and 263 vtables; 4 classes inline their constructor over a shared base's); 2026-10-25: the packet classes started, 91 of the 263 rebuilt in 56 app/packets files (pklogoff.cpp by hand, the rest drafted by tools/re/pkdecode.py / pkemit.py), every function compiling to the client's bytes; the 172 others (char * buffers and arrays, vectors and aux-data members, intermediate bases, Print through tables or helpers) still declared in pk_classes.h: `pkdecode.py --report` says why) | 45, 17.6 KB (the factory 16 KB) | LinearBufferClass (in the tree); the packet classes (0x85C5F0-0x8B1EB0) for an exact factory | decode the captures' messages into packet objects |
| 6 | clientstub.cpp (**done** 2026-10-25: all 149 exact, generated by tools/re/pkclasses.py with the Process_Packet declarations in pk_classes.h) | 149, 4.1 KB | (5)'s declarations | trivial, each on its `__LINE__` |
| 7 | MenuTask, the movie tasks (tsk_movie.cpp, tsk_smovie.cpp) (**done** 2026-10-24: app\client\tasks\tsk_menu.cpp (MenuTask: no `__FILE__`, the name from the link order, NAME 1), tsk_movie.cpp, tsk_smovie.cpp, tsk_login.h declared; 24 client functions exact, the scripted task's Init functional (its loop isn't rotated in the client); 0xA161B0-0xA162D0 is Renegade's BinkMovie library (BINKMovie, E&B's Play takes a volume), declared only; `kerneltest.py`: 81 of 81 with the start-up chain intro -> scripted movie -> MenuTask -> LoginTask; `movietest.py` plays eb_ws_logo.bik / eb_sizzle.bik through the task with Renegade's player) | ~8 + 5 + 19 functions, ~3 KB | (1); Bink (0xA16xxx), mclient.cpp / client_lego_impl.cpp for the scripted movie | VIDTEST-style program playing the intro |
| 8 | tcpconn.cpp, connect.cpp's factories and ConnectionManager, memconn.cpp (**started** 2026-10-25: app/common/tcpconn.cpp PARTIAL, TCPConnection in the client's layout, 8 exact: the constructor by address, destructor, Connect, Is_Connected, Disconnect, Get_Address, Transport_Write / Read; Try_Connect functional; the constructors by host 0x828C80 and over a socket 0x829C80 exact; the RSA key exchange 0x829180 and the listener not yet. E&B's TCPCon and TCPQueue rebuilt byte-exact in libs\wwonline\wnet (tcpcon.cpp, tcpqueue.cpp: c0e19b6); ConnectionManager's constructor, destructor, Get_Port, Set_Name, Peek, Get_Packet, Receive, Broadcast, Mark, Clear_Marks, Set_Listener, Set_Unknown05, Remove_Connection, Destroy_Connection, Destroy_All, Is_Managed exact (connect.h); Accept_New, Add_Connection and the factories not yet; 2026-10-26: connect.cpp complete but Remove_Dead 0x822B50 (NOT EXACT): the forwarding and peek wrappers, Accept_New, Add_Unwrapped, Add_Connection, the factories 0x821000 / 0x821400 / 0x821450 with the host helpers and Network_INI, Create_Connection_Manager 0x822F10; Try_Connect exact; memconn.cpp (memory connection, pipes, listener; the option slot 0x826D00 exact, S1) and its registry by port (PROVISIONAL unknown_memregistry.cpp); dbgconn.cpp's packet logs, rate wrapper (on the new average.h) and statistics wrapper slots (its counters 0x825390 / 0x8255B0, a hash map keyed by std::string, not yet); the UDP listener (udpconn.cpp), the TCP listener's Stop / Accept (Listen 0x82A470 NOT EXACT, register, LIMIT; its constructor with the RSA key file not yet), the composite listener (PROVISIONAL unknown_listener.cpp), the rate average (PROVISIONAL unknown_rate.cpp); app 1028 -> 1133. Not yet: the RSA exchange 0x829180, the constructor by name 0x82C680) | 25 + ~35 + 23 functions, ~17 KB | WWOnline wnet's TCPMgr / TCPCon (libs\wwonline\wnet, D73, D83), ArgvClass, dbgmgr.cpp | the Net-7 recordings' TCP streams (M1.5's data) |
| 9 | tsk_game.cpp (ClientTask), tsk_login.cpp (LoginTask) | 18 + 54 functions, 2.8 + 13.8 KB | (4)-(8); client.cpp, the login and character select screens (0xAF2C30, 0xAEFB60), kcom auth (`authFactory`, `iauth`), SClient | a login against a recorded server |
| 10 | main_client.cpp's WinMain | 1 function, 3 KB | every subsystem it starts (the table in §1) | last; a reduced WinMain for the programs before |

Recommendation (a proposal for the user): (1) next (app/kernel/kernel.h already declares its
interface, for (2), which is done); it needs nothing from WWOnline and gives the client its loop; then (3) and (4), which put the
rebuilt UDP stack under a kernel task and the game's connection class and can be tested with
udpreplay's loop test. (5) is the gate to the packet classes and is large; (8) and (9) wait for
the wnet decision.

## 7. Traps and corrections found while mapping

* The earlier research (N) gives LoginTaskClass's vtable as 0xB040CC with slot offsets 8 too
  high: the table starts at 0xB040D4 (§3).
* app-functions.json attributes the connection base's inline slots 0x726AF0-0x726BF0 to
  client.cpp: they are connect.h's inline virtuals, emitted there.
* asserts.txt's "All data sent, queueing close packet." under bandwidthaccounting.cpp is
  cordcon.cpp's pumpClose log (the nearest-path rule); bandwidthaccounting.cpp's own strings are
  the debug window's.
* The task base's vtable has no virtual names in the client; the slot names in §3 come from the
  tasks' strings (focus, Run, Render, Shutdown) and the kernel's use of them.
