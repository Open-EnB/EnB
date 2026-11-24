# Claim 278 — 0x5CFE40 replay evidence

Patch blocks encode exact unified-diff text as JSON; decode the `patch` field before applying.

Fresh HEAD 90ebf551. Initial and final baseline controls both 60 exact symbol/address pairs. 27 valid forms; initial Wine socket setup failure excluded. Whole-unit native VC6 SP4 matching used original app flags, original basename and fixed assert lines. Masked target SHA groups compiler duplicates; similarity is heuristic, not proof.

| form | source SHA-256 | masked target SHA-256 | object span | similarity | symbol gains / losses |
|---|---|---|---:|---:|---|
| baseline | `d9a2a49fede6eb1d45b9d73f5fd14ed104822358dec93a01c0dd4699069db467` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| slot_ref | `409c782a53df4b2cc429be6a2f9730369946c041995d219742a4d21aadddc354` | `92b263af441a58a05118b0b3e13586cbfab900ec053fe8581460cf533ed6358d` | 1152 | 0.84 | 0 / 0 |
| slot_ptr | `3e6d8a61ab0d7b70fdf0238e4fc2d4d2238738a70c11a903b481ce45fd1540a9` | `92b263af441a58a05118b0b3e13586cbfab900ec053fe8581460cf533ed6358d` | 1152 | 0.84 | 0 / 0 |
| slot_index | `5ac80b5a9f4d67e4da5077dc427f380d55a83d177cc0f6bcecc0d77c44cf7ed9` | `92b263af441a58a05118b0b3e13586cbfab900ec053fe8581460cf533ed6358d` | 1152 | 0.84 | 0 / 0 |
| name_local | `5d02e7ab0da2b85e96e6867381a5df76c6f2af6b944d347e497da31a073ce446` | `4f7ad3ac70f0e61e6d76c121ad53864e33b7dc80d208215cb1c3983cb33284a7` | 1152 | 0.71 | 0 / 0 |
| test_i | `0051bcc38ca4ff467d9154b5236c5e4e83aacb730b78c81e6a9e4b58559a070c` | `a7083e5112fa748f8476998733c998ff94b6a94ed50adf6ccd66efe45f8975a9` | 1200 | 0.57 | 0 / 0 |
| test_i_unsigned | `41a5f6abedf91816434edbd99f76aea4b5c9e5b897ac6bedecccddd803b0bb10` | `a7083e5112fa748f8476998733c998ff94b6a94ed50adf6ccd66efe45f8975a9` | 1200 | 0.57 | 0 / 0 |
| test_diff | `cb9141c43dfb52f90b720a34abf2f8f3894a1c2ef4ecbbb0fdee18e1913594e5` | `d0fb82473651e2e8e6e0c8ee76ccbaded597834eb281db3d0a795fb6129a12de` | 1152 | 0.81 | 0 / 0 |
| test_offset | `b42189a7fc014f59e94e72a66ca4c0bd0bc19d45d2276869a60355fcea96c1ed` | `70270b92f0c4d606e3e2592843061e33075817856ab2cccc91f3864776f815f1` | 1136 | 0.66 | 0 / 0 |
| post_ref | `0eb1055894b9ab8ffdeec3c739340d14bd4397545e2a24ee315113d71011259f` | `92b263af441a58a05118b0b3e13586cbfab900ec053fe8581460cf533ed6358d` | 1152 | 0.84 | 0 / 0 |
| post_ptr | `b54adb8ec632fcb7dceab8eb4d02e01c310bfb6957c53dfcffa1ef4a880dc316` | `92b263af441a58a05118b0b3e13586cbfab900ec053fe8581460cf533ed6358d` | 1152 | 0.84 | 0 / 0 |
| post_val | `15c4e56d04af45f45b820edd3771276b130337489a3a4454b44a744a24266a61` | `3138e35058ad05354686d72b8e447b7b8b9b9765424eee4f4e2e7ec3ad12655b` | 1168 | 0.69 | 0 / 0 |
| count_lt | `e54ec0fa48958e0cbbdfb866847f2ffcfe6894f78cb6750eda9e66a3452573a4` | `a10a6e2a49a72708352e6ce51a17edb5b163d298464b90f1d549c9551c95fc92` | 1152 | 0.84 | 0 / 0 |
| count_const | `d2c323da7f824550a44750be781b15c1c69f028a9b16c03a83ccedcc2d7cde6e` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| count_le | `e919a34a3a617d7bb4dc847f70b36ba3e9bc7eb706bd746ffa6118dbfdb597f8` | `e206a9b8033073d904dc5d10aee126af3e0eb1a102f3a9965a1eb65cad1bab73` | 1152 | 0.83 | 0 / 0 |
| index_pre | `ad06433aa7456ec4784bf4fe427f53c4bb7af0b8cb5d096a9e47bb70a76b4ff1` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| index_add | `80006fc863c85023b7a98c8768acfda5da2d0d61e038ee0ef81a4d42a3627ef4` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| index_assign | `c507cc03a6cec7842969581777f79e625f4fa02c1056c4de40399e9ff986d036` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| index_sub | `ffcfef61c43e5b5f683489498e3654cbc880181892ff6e0aeb496928ee6b3728` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| index_unsigned | `a0d1b1f7b1a3c5bc6e7814b2262aa4b69b5b9d835e538e883a7efb222ab99a81` | `31546d32e200cb47e6474ea004107de4b46c305d71573519b772ae028478d1d4` | 1152 | 0.84 | 0 / 0 |
| id_reverse | `83052efb950573380b9dfb1609239667ac66188241f0b7f0322495e8454e3143` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| id_temp | `df52c59c06478784dd0e60b55765c76c82c544ac7be1a2a4333b92f9dd1cdf98` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| id_subtract | `48fa1c27f41aeb201628ced6358abb4b85c16353936d559d9e5647e8ef2b1e34` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| id_inc | `16f5e90ca277b3c59647e962a5656764a75cc72d6add0797c2afecb8f2167c0c` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| name_deref | `9a4e38b37429fd0ebadcd1af254d7f1fbd5e39d375f3c5339d2ac709b9a4d1b5` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| slot_deref | `ea9e142e12e0ed895b36dd91f51d167e6462a848e4dee5b9c44fe7f5183afb4c` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| name_index | `42ba4566c92238176a07b774a1a4ec80ae990c05c16a24a688f9cb91fe9c61ba` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |
| slot_index_commute | `24a9e78331ec625fda245564337d4ee5faea0fc3120831c84aa95e25dcb6840a` | `a1496db8fbf6e7cc79a4e24594235446290b5843cddea00623e1801da3a38ec7` | 1152 | 0.84 | 0 / 0 |

Baseline span 1152 bytes including padding, stack reservation 0x18. The principal mismatch is pointer update order: retail increments the gadget pointer before the name pointer and uses EDI for the latter; baseline updates the name through EDX first. Caching the name changes EDI allocation but worsens overall similarity. No candidate changed the exact set, no production edit retained. Relocations and EH are not asserted exact. Maintainer verification remains authoritative.

Compiler code groups (masked target bytes):
- `a1496db8fbf6`: baseline, count_const, index_pre, index_add, index_assign, index_sub, id_reverse, id_temp, id_subtract, id_inc, name_deref, slot_deref, name_index, slot_index_commute
- `92b263af441a`: slot_ref, slot_ptr, slot_index, post_ref, post_ptr
- `4f7ad3ac70f0`: name_local
- `a7083e5112fa`: test_i, test_i_unsigned
- `d0fb82473651`: test_diff
- `70270b92f0c4`: test_offset
- `3138e35058ad`: post_val
- `a10a6e2a49a7`: count_lt
- `e206a9b80330`: count_le
- `31546d32e200`: index_unsigned

### slot_ref

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ slot_ref/ui_pda_cargo.cpp\n@@ -135,15 +135,15 @@\n \t\tint id = i + 13;\n-\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n+\t\tButtonGadgetClass *&button = Buttons5[i]; RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n \t\tUnknown_496AE0(robj, 0);\n \t\tif (id == 15) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n+\t\t\tbutton = new ButtonGadgetClass(robj, 5, 3);\n \t\t} else if (id == 16) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n+\t\t\tbutton = new ButtonGadgetClass(robj, 0x50, 3);\n \t\t} else {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x55, 3);\n-\t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\t\tbutton = new ButtonGadgetClass(robj, 0x55, 3);\n+\t\t}\n+\t\tGadgetClass::Set_Tooltip(button, \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tbutton->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0(button, id, View, 1, 0);\n \t}\n"}
```

### slot_ptr

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ slot_ptr/ui_pda_cargo.cpp\n@@ -135,15 +135,15 @@\n \t\tint id = i + 13;\n-\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n+\t\tButtonGadgetClass **button = &Buttons5[i]; RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n \t\tUnknown_496AE0(robj, 0);\n \t\tif (id == 15) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n+\t\t\t(*button) = new ButtonGadgetClass(robj, 5, 3);\n \t\t} else if (id == 16) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n+\t\t\t(*button) = new ButtonGadgetClass(robj, 0x50, 3);\n \t\t} else {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x55, 3);\n-\t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\t\t(*button) = new ButtonGadgetClass(robj, 0x55, 3);\n+\t\t}\n+\t\tGadgetClass::Set_Tooltip((*button), \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\t(*button)->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0((*button), id, View, 1, 0);\n \t}\n"}
```

### slot_index

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ slot_index/ui_pda_cargo.cpp\n@@ -135,15 +135,15 @@\n \t\tint id = i + 13;\n-\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n+\t\tButtonGadgetClass *&button = *(Buttons5 + i); RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n \t\tUnknown_496AE0(robj, 0);\n \t\tif (id == 15) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n+\t\t\tbutton = new ButtonGadgetClass(robj, 5, 3);\n \t\t} else if (id == 16) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n+\t\t\tbutton = new ButtonGadgetClass(robj, 0x50, 3);\n \t\t} else {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x55, 3);\n-\t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\t\tbutton = new ButtonGadgetClass(robj, 0x55, 3);\n+\t\t}\n+\t\tGadgetClass::Set_Tooltip(button, \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tbutton->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0(button, id, View, 1, 0);\n \t}\n"}
```

### name_local

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ name_local/ui_pda_cargo.cpp\n@@ -135,3 +135,3 @@\n \t\tint id = i + 13;\n-\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n+\t\tconst char *button_name = CargoButtonNames[i]; RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(button_name);\n \t\tUnknown_496AE0(robj, 0);\n@@ -144,3 +144,3 @@\n \t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", button_name);\n \t\tButtons5[i]->Unknown_65FFA0(first);\n"}
```

### test_i

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ test_i/ui_pda_cargo.cpp\n@@ -137,5 +137,5 @@\n \t\tUnknown_496AE0(robj, 0);\n-\t\tif (id == 15) {\n+\t\tif (i == 2) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n-\t\t} else if (id == 16) {\n+\t\t} else if (i == 3) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n"}
```

### test_i_unsigned

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ test_i_unsigned/ui_pda_cargo.cpp\n@@ -137,5 +137,5 @@\n \t\tUnknown_496AE0(robj, 0);\n-\t\tif (id == 15) {\n+\t\tif ((unsigned)i == 2) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n-\t\t} else if (id == 16) {\n+\t\t} else if ((unsigned)i == 3) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n"}
```

### test_diff

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ test_diff/ui_pda_cargo.cpp\n@@ -137,5 +137,5 @@\n \t\tUnknown_496AE0(robj, 0);\n-\t\tif (id == 15) {\n+\t\tif (id - 13 == 2) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n-\t\t} else if (id == 16) {\n+\t\t} else if (id - 13 == 3) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n"}
```

### test_offset

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ test_offset/ui_pda_cargo.cpp\n@@ -137,5 +137,5 @@\n \t\tUnknown_496AE0(robj, 0);\n-\t\tif (id == 15) {\n+\t\tif (i + 1 == 3) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n-\t\t} else if (id == 16) {\n+\t\t} else if (i + 1 == 4) {\n \t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n"}
```

### post_ref

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ post_ref/ui_pda_cargo.cpp\n@@ -144,6 +144,6 @@\n \t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\tButtonGadgetClass *&button = Buttons5[i]; GadgetClass::Set_Tooltip(button, \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tbutton->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0(button, id, View, 1, 0);\n \t}\n"}
```

### post_ptr

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ post_ptr/ui_pda_cargo.cpp\n@@ -144,6 +144,6 @@\n \t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\tButtonGadgetClass **button = &Buttons5[i]; GadgetClass::Set_Tooltip((*button), \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\t(*button)->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0((*button), id, View, 1, 0);\n \t}\n"}
```

### post_val

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ post_val/ui_pda_cargo.cpp\n@@ -144,6 +144,6 @@\n \t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\tButtonGadgetClass *button = Buttons5[i]; GadgetClass::Set_Tooltip(button, \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tbutton->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0(button, id, View, 1, 0);\n \t}\n"}
```

### count_lt

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ count_lt/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i != NUM_BUTTONS; i++) {\n \t\tint id = i + 13;\n"}
```

### count_const

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ count_const/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i < 5; i++) {\n \t\tint id = i + 13;\n"}
```

### count_le

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ count_le/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i <= 4; i++) {\n \t\tint id = i + 13;\n"}
```

### index_pre

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ index_pre/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i < NUM_BUTTONS; ++i) {\n \t\tint id = i + 13;\n"}
```

### index_add

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ index_add/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i < NUM_BUTTONS; i += 1) {\n \t\tint id = i + 13;\n"}
```

### index_assign

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ index_assign/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i < NUM_BUTTONS; i = i + 1) {\n \t\tint id = i + 13;\n"}
```

### index_sub

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ index_sub/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (int i = 0; i < NUM_BUTTONS; i -= -1) {\n \t\tint id = i + 13;\n"}
```

### index_unsigned

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ index_unsigned/ui_pda_cargo.cpp\n@@ -133,3 +133,3 @@\n \tint first = ((GadgetClass *)first_bttn)->Unknown0C;\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n+\tfor (unsigned i = 0U; i < NUM_BUTTONS; i++) {\n \t\tint id = i + 13;\n"}
```

### id_reverse

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ id_reverse/ui_pda_cargo.cpp\n@@ -134,3 +134,3 @@\n \tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tint id = i + 13;\n+\t\tint id = 13 + i;\n \t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n"}
```

### id_temp

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ id_temp/ui_pda_cargo.cpp\n@@ -134,3 +134,3 @@\n \tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tint id = i + 13;\n+\t\tint id = (i + 12) + 1;\n \t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n"}
```

### id_subtract

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ id_subtract/ui_pda_cargo.cpp\n@@ -134,3 +134,3 @@\n \tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tint id = i + 13;\n+\t\tint id = i - (-13);\n \t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n"}
```

### id_inc

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ id_inc/ui_pda_cargo.cpp\n@@ -134,3 +134,3 @@\n \tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tint id = i + 13;\n+\t\tint id = i + 0xD;\n \t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n"}
```

### name_deref

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ name_deref/ui_pda_cargo.cpp\n@@ -135,3 +135,3 @@\n \t\tint id = i + 13;\n-\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n+\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name((*(CargoButtonNames + i)));\n \t\tUnknown_496AE0(robj, 0);\n@@ -144,3 +144,3 @@\n \t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", (*(CargoButtonNames + i)));\n \t\tButtons5[i]->Unknown_65FFA0(first);\n"}
```

### slot_deref

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ slot_deref/ui_pda_cargo.cpp\n@@ -138,12 +138,12 @@\n \t\tif (id == 15) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n+\t\t\t(*(Buttons5 + i)) = new ButtonGadgetClass(robj, 5, 3);\n \t\t} else if (id == 16) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n+\t\t\t(*(Buttons5 + i)) = new ButtonGadgetClass(robj, 0x50, 3);\n \t\t} else {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x55, 3);\n-\t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\t\t(*(Buttons5 + i)) = new ButtonGadgetClass(robj, 0x55, 3);\n+\t\t}\n+\t\tGadgetClass::Set_Tooltip((*(Buttons5 + i)), \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\t(*(Buttons5 + i))->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0((*(Buttons5 + i)), id, View, 1, 0);\n \t}\n"}
```

### name_index

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ name_index/ui_pda_cargo.cpp\n@@ -135,3 +135,3 @@\n \t\tint id = i + 13;\n-\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);\n+\t\tRenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(i[CargoButtonNames]);\n \t\tUnknown_496AE0(robj, 0);\n@@ -144,3 +144,3 @@\n \t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", i[CargoButtonNames]);\n \t\tButtons5[i]->Unknown_65FFA0(first);\n"}
```

### slot_index_commute

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ slot_index_commute/ui_pda_cargo.cpp\n@@ -138,12 +138,12 @@\n \t\tif (id == 15) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 5, 3);\n+\t\t\ti[Buttons5] = new ButtonGadgetClass(robj, 5, 3);\n \t\t} else if (id == 16) {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x50, 3);\n+\t\t\ti[Buttons5] = new ButtonGadgetClass(robj, 0x50, 3);\n \t\t} else {\n-\t\t\tButtons5[i] = new ButtonGadgetClass(robj, 0x55, 3);\n-\t\t}\n-\t\tGadgetClass::Set_Tooltip(Buttons5[i], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n-\t\tButtons5[i]->Unknown_65FFA0(first);\n+\t\t\ti[Buttons5] = new ButtonGadgetClass(robj, 0x55, 3);\n+\t\t}\n+\t\tGadgetClass::Set_Tooltip(i[Buttons5], \"UI_PDA_CARGO\", CargoButtonNames[i]);\n+\t\ti[Buttons5]->Unknown_65FFA0(first);\n \t\trobj->Release_Ref();\n-\t\tUnknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);\n+\t\tUnknown64.Unknown_6870E0(i[Buttons5], id, View, 1, 0);\n \t}\n"}
```
