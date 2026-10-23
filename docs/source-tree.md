# The original source tree

client.exe keeps the paths of the source files it was built from. Release builds kept their
asserts, so every file with an assert left its `__FILE__` string behind: 472 paths under
`e:\build\beta\G\`, plus a few relative header paths. The full list is
[data/source-paths.txt](data/source-paths.txt) (regenerate with `tools/re/srctree.py`).

Files with no assert left no string, so the real tree is bigger than this. Directories
that aren't here (the WW libraries, for example) are known some other way, given in the
"Evidence" column.

## 1. Layout

| Original path | Files seen | What it is | Evidence |
|---|---|---|---|
| `G\app\main` | 1 (`main_client.cpp`) | WinMain; the project that links the client. The PDB was `G\app\main\client_release\main.pdb`, so the project was `main` and the configuration `client_release` | `__FILE__`, PDB path |
| `G\app\kernel` | 1 (`kernel.cpp`) | `KernelClass::Run_One_Cycle`: the task loop (`Current_Task->Run()`, `Background_Task`, `Pending_Delete`) | `__FILE__`, profile labels |
| `G\app\client` | ~290 (with `html`, `item`, `tasks`) | The game client: UI screens (`ui_*.cpp`), gadgets (`gd_*.cpp`), camera controllers (`camctrl_*.cpp`), effects (`eff_*.cpp`), radar, nav, starmap, galaxy map, fonts, client objects (`co.cpp`, `co_data.cpp`), tasks (`tasks\tsk_login.cpp`, `tsk_game.cpp`, `tsk_movie.cpp`, `tsk_smovie.cpp`), item property display (`item\`), an HTML viewer (`html\`) | `__FILE__` (spelled both `Client` and `client`: two projects, or two include paths) |
| `G\app\common` | 37 | Code shared by client and server: connections (`tcpconn.cpp`, `udpconn.cpp`, `memconn.cpp`, `connect.cpp`), game definitions (`race.cpp`, `prof.cpp`, `itemtype.cpp`, `weapontype.cpp`, `faction_and_reputation.cpp`, `guild_defs.cpp`, `manufacturing_defs.cpp`, `starbase_defs.cpp`), `galaxy_map.cpp` | `__FILE__` |
| `G\app\shared` | 71 | Shared engine-level code: `attrib_dir*.cpp` (attributes), the `kv_*` key/value encoders, `px*.cpp` (physics), `rs*.cpp` (render states / effects), `spline*`, `orbitalmanager.cpp`, `enbassetmgr.cpp` (`ENBAssetManager`), `callback_manager.cpp`, `timer_manager.cpp`, `money.cpp`, `observer.cpp` | `__FILE__` |
| `G\app\shared\auxdata` | 16 | AuxData: `delta.cpp`, `delta_template.h`, `delta_array.h`, `delta_item_array.h`, `delta_interpolator.*`, `delta_string.cpp`, `delta_money.cpp`, `delta_time.cpp`, `bitset.cpp`, `encode.cpp`, `linearbu.cpp`, `lookup.cpp`, `name.cpp`, `permstr.cpp`, `policy.cpp`, `auxdata.h`, `rtti.h`. Already worked out in the reference decoder ([prior-work.md](prior-work.md)) | `__FILE__` |
| `G\app\shared\expr` | 10 | An expression lexer/parser/evaluator (`expr_math_*`) | `__FILE__` |
| `G\app\shared\message` | 1 | `msgserv.cpp` | `__FILE__` |
| `G\app\packets` | 23 (with `gameedit`) | Packet classes: `pk.h`, `pk.cpp`, `pkupdate.cpp`, `pkitemba.cpp`, `pktask.cpp`, `pkflag.cpp`, `pkjoblist.cpp`, ...; `gameedit\pkge*.cpp` are the game editor's packets. `clientstub.cpp`. Each packet is `XxxPacketClass` with `Process_Packet` | `__FILE__`, ~100 `XxxPacketClass::Process_Packet` strings |
| `G\app\wwudp` | 3 | E&B's reliable UDP: `cordmgr.cpp` (`CordMgr`), `cordcon.cpp` (`CordCon`), `bandwidthaccounting.cpp` | `__FILE__` |
| `G\app\audio` | 10 | Music and ambient sound on top of WWAudio (`musicmanager.cpp`, `sectorsound.cpp`, `starbasesound.cpp`, ...) | `__FILE__` |
| `G\app\input` | 5 | Input: `input.cpp`, `inpdsp.cpp`, `w32kbd.cpp`, `w32mouse.cpp` (`Win32MouseHandlerClass`) | `__FILE__` |
| `G\app\display` | 1 | `display.cpp` | `__FILE__` |
| `G\app\dbgmgr` | 1 | `dbgmgr.cpp` | `__FILE__` |
| `G\libs\wwonline\wnet` | 1 | `tcpqueue.cpp`: Westwood Online networking | `__FILE__` |
| `G\wwlib` | (`ref_ptr.h`) | WWLib. `app\Client\ui_item_examination.cpp` includes `..\..\wwlib\ref_ptr.h`, so WWLib was at `G\wwlib` | relative include |
| `G\ww3d2`, `G\wwmath`, `G\wwaudio`, `G\wwdebug`, `G\wwsaveload`, `G\wwshade` (inferred) | none | The WWVegas libraries, statically linked ([library-matching.md](library-matching.md)). That they sat beside `wwlib` is inferred, not seen | string matches against EA's code |

## 2. How the repository mirrors it

The repository root stands for `G\`:

```
EnB/
  app/        G\app (E&B's own code; same subdirectories, lower case)
  libs/       G\libs (wwonline)
  wwlib/ ...  the Westwood libraries, once imported (library-matching.md §5)
  docs/       this documentation
  tools/      our reverse-engineering tools (not part of the original tree)
```

Directory names are lower case. The original mixes `Client`/`client`, `Common`/`common` and
`Packets`/`packets`; Windows didn't care, and we can't tell which spelling was the
directory's own.

## 3. What the tree tells us

* **E&B is not built on Generals' game engine.** No `GameEngine`, `GameLogic` or `GameClient`
  paths. Matching Zero Hour's `GameEngine` and `GameEngineDevice` strings against the client finds
  only common words (`Priority`, `Animation`, `TurnRate`), with one exception:
  `GameEngineDevice/.../W3DAssetManager.cpp`'s texture-recolouring code (`#%s!%gH%gS%gV%g`,
  `WW3DAssetManager::Create_Render_Obj`), a WW3D asset manager extension that E&B's
  `ENBAssetManager` (`app\shared\enbassetmgr.cpp`) probably shares. Otherwise E&B shares only
  the WWVegas libraries with Generals.
* **There was a server in the same tree.** `app\common` and `app\shared` hold code meant for both
  sides (`distance_calc_client.cpp` implies a server counterpart; `pk_sub_parts.cpp`,
  `gameedit\pkge*.cpp` are editor and server packets compiled into the client).
* **The task kernel** (`app\kernel`, `app\client\tasks`) is how the client is structured:
  `LoginTaskClass` (`tsk_login.cpp`), the game task (`tsk_game.cpp`), movies.
