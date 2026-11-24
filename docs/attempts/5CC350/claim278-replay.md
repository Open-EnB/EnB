# Claim 278: 0x5CC350 replay

Patch blocks encode exact unified-diff text as JSON; decode the `patch` field before applying.

Fresh HEAD a5f1534b, 26 valid candidates in four analyzed batches. Baseline 52 exact symbol/address pairs; final 53, +0x5CC350 and no losses. All candidates compiled and preserved all 52. Original app flags, basename and assertion lines 130, 136, 371 and 374 preserved. The retail and local audit use a private client and VC6; maintainer verification remains required. Masked hashes group code, not relocation or EH proof.

| form | source SHA-256 | masked target SHA-256 | span | similarity | gains / losses |
|---|---|---|---:|---:|---|
| baseline | `7da4bd2b5c9591a70973163376738447f83365951f89744ef746bd1968749b37` | `1240be770f864820822fda1e134c118a8de58845e8b13eab62dd600a8f014e81` | 832 | 0.51 | 0 / 0 |
| named_font | `e91000bfc39828ef3f31bc01267f164e50e95dc160648b9cdfb60eaf93f5a28a` | `1240be770f864820822fda1e134c118a8de58845e8b13eab62dd600a8f014e81` | 832 | 0.51 | 0 / 0 |
| named_label | `174dcb52474bccb609cccaf2891eba0592bd79d18531cd0e098996c8949fe5e0` | `1240be770f864820822fda1e134c118a8de58845e8b13eab62dd600a8f014e81` | 832 | 0.51 | 0 / 0 |
| named_both | `2cb81a008e082ae4fd7f9a5729481eb8eb5ffdcafa935ac4f6405acd5ab7a7f1` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| implicit_both | `40708854a16b7dbec74b3de9fb7f1c0a9bc276117d9e5e85999686152227d26f` | `1240be770f864820822fda1e134c118a8de58845e8b13eab62dd600a8f014e81` | 832 | 0.51 | 0 / 0 |
| ptr_post | `614eea283639e80d344135266dc67018e589151f8b91e958b41837ddf5ead767` | `c8737e0e141958e22fcddc24f2cb88df288d72a0e83c132da5ed6d75a8d22622` | 816 | 0.47 | 0 / 0 |
| ptr_pre | `3b38ff64284b918a602e2a24d285d1b4d69ba5046a09623ce3c12966096ed697` | `c8737e0e141958e22fcddc24f2cb88df288d72a0e83c132da5ed6d75a8d22622` | 816 | 0.47 | 0 / 0 |
| label_ptr | `edebd801b7c47533265d426093ea3f74b56da046b06baa321bb8b867286b34e8` | `4ff442298d565f363dc852bdea54d8895f3b08c1106f51af4dd41a704fe9fc38` | 832 | 0.49 | 0 / 0 |
| font_ptr | `72256257a7818695f5eee8c6c3916be117169aaa4403f713e6800098c6eb234c` | `4bf4e624abfaace5f1284639d2228b0235bfe976f363a1027c9c61196d203aeb` | 816 | 0.46 | 0 / 0 |
| label_p | `2e0be3ae0a0312ecbc07b11cf4c470dde7d051ee48bc26fe46c14a4010d409f4` | `4450711f001c34066e79937221565ce4e6b5c77ae9df7e5653fb48e60a7f9371` | 832 | 0.53 | 0 / 0 |
| font_p | `c34a1549d0087d3a2564c75c76d4762ba127dcd6d108432e08e2b223c2e1aad9` | `6c5b571768c24ace17645dd8d0d33199d5d049616a1b0bb74b06cd8e82d62fc8` | 832 | 0.64 | 0 / 0 |
| zero_loops | `52902886e0abf804499777a6185c43837678f5cd50701fcd843369bd022af314` | `af275c3e2db90dfd8bfb2fb7b25497f822a42d24035d58b82a55cab03a7b545d` | 880 | 0.37 | 0 / 0 |
| zero_button_loop | `baf7742dad2dfdb897d417d64449612cec9ad3e4046ba25b5b247996a895a675` | `c1309073755c3ecab202d82617ff6bad083bbed0ac4126f8314cacf4157d62e8` | 832 | 0.63 | 0 / 0 |
| zero_slider_loop | `c5d0bef21e15354fb52067ad574b93bed188652dc567199ba61ff518223e524b` | `b196ad43580844dd5c06ffe4724ff61517345687c5ac7412ecc2b5a431461292` | 832 | 0.65 | 0 / 0 |
| zero_button_assign | `37f9d7969260676e383aecfc35d1bb96f11d907b339b4fd49875d7620190a45a` | `f869439bf23a4ca78c21d9fa3b4281d0e154fd8e36cb8bdfa07f25916462f73f` | 832 | 0.65 | 0 / 0 |
| zero_slider_assign | `e3e8519f013643339324aa272f2a5376618e85b1467b84099978cdc574b01d79` | `3e646a76370ef363ce381c254a3283c12a636b2b2d60ed0374bbe212a6c723b3` | 832 | 0.65 | 0 / 0 |
| zero_both_assign | `1b937077c756d5f38628aaab6ae1d0030ee6ba78a96a62ab8569d8ce03264901` | `28d9d22b6b3f5d7db735c436c363ee955bed300cff972d849b0a3b802e6d838f` | 880 | 0.38 | 0 / 0 |
| zero_fill | `a57d473a703bed5460a1810d4f1f5589ec9cbc5595b93faeb423af07f7414f89` | `32a28d09a125c87b02db6bc67f61f1b48fee3a21a8be7f09efdaa2af50a93c6f` | 832 | 0.58 | 0 / 0 |
| zero_elements | `87cd33f1bce0a2ffe38bf72ab919fcf9f6378cbec5b35a565d19220b1a5dd55f` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| ne_null | `038bd07e05b33299719a2e2bb662f27312847837acfb8321b81019a67a4b3f4e` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| ne_zero | `bc8b79671ebe96f8290e350f8232e86ddcfafee34e12fb64d630d1ace92820a7` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| reverse_cmp | `ce38fd6e4f506579d9fd5bd76bfcb7cb4711532cf9c89809d1390edc1039992c` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| negated | `59e3695b3b6b911d85ce9d0b1438c538c41d0e2264a5ea9c369743c81a9fc19a` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| double_not | `e7563cfa53035ba0a410ce933c324048e066001703719dc41bd4e9f4eb1d23be` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| indexed_chars | `754afe996d076b285928a163383bf33d3ee01784f3179870483dba956453031d` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| char_ne | `3d2a70521c62355530626ecfd745e1763a5e21cb474ef7f9056e26f8b9e3f61a` | `39fdafd4153d87f85647f164332480cde36ffc5685d0c0f9dd674eece51bf95e` | 832 | 0.68 | 0 / 0 |
| ternary_checks | `c5cbb6d5e14e6c82b0aa9129c8add01214b695aabe9cf12395874e03e6d25b4b` | `750605ff32861c903f1e012dfde989d73e31698e9a6ec48c8524e228271851ce` | 816 | 0.91 | 1 / 0 |
| final | `5b1d5dd1608f3f521ddf21000a7d70f281737b5e44a8c098a17aefea331b2017` | `750605ff32861c903f1e012dfde989d73e31698e9a6ec48c8524e228271851ce` | 816 | 0.91 | 1 / 0 |

Retail and final executable body: 807 bytes including return and branches; frame 0x34. Retail verifies font pointer, font first byte, label pointer, label first byte in that order, then constructs, inserts and destroys each separate string before the four getters (Unknown98, Spacing, Scale, Index) and settings insertion. Final nested conditional preserves short-circuit and null checks and the lexical string scopes preserve destruction and exceptions. Full body fixed bytes match and every relocation was resolved with zero addend; three vtable references checked against their class tables and representative entry, global fontDatabase at 0xBE67B8, npos value 0xffffffff. Indirect receivers are the same because executable instructions and vtable relocation targets match. EH handler, FuncInfo, predecessor states and each cleanup funclet/relocation were checked independently. Earlier 52 exact symbol bodies and relocations including recursively resolved compiler-renumbered locals were compared before/after; source-path literals agree by basename. Scripts and local proof artifacts are under .work/claim-278/5CC350, not in git.

Masked code groups:
- `1240be770f86`: baseline, named_font, named_label, implicit_both
- `39fdafd4153d`: named_both, zero_elements, ne_null, ne_zero, reverse_cmp, negated, double_not, indexed_chars, char_ne
- `c8737e0e1419`: ptr_post, ptr_pre
- `4ff442298d56`: label_ptr
- `4bf4e624abfa`: font_ptr
- `4450711f001c`: label_p
- `6c5b571768c2`: font_p
- `af275c3e2db9`: zero_loops
- `c1309073755c`: zero_button_loop
- `b196ad435808`: zero_slider_loop
- `f869439bf23a`: zero_button_assign
- `3e646a76370e`: zero_slider_assign
- `28d9d22b6b3f`: zero_both_assign
- `32a28d09a125`: zero_fill
- `750605ff3286`: ternary_checks, final

### named_font

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ named_font/ui_option_font.cpp\n@@ -49,3 +49,3 @@\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n \t\tLabels.push_back(std::string(labels[i]));\n"}
```

### named_label

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ named_label/ui_option_font.cpp\n@@ -50,3 +50,3 @@\n \t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### named_both

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ named_both/ui_option_font.cpp\n@@ -49,4 +49,4 @@\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### implicit_both

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ implicit_both/ui_option_font.cpp\n@@ -49,4 +49,4 @@\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\tFontNames.push_back(name);\n+\t\tLabels.push_back(labels[i]);\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### ptr_post

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ ptr_post/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n-\t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\tfor (; *font_names && **font_names && *labels && **labels; font_names++, labels++) {\n+\t\tconst char *name = *font_names;\t\t\t\t// NAME 1: neutral (D10)\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(*labels); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### ptr_pre

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ ptr_pre/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n-\t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\tfor (; *font_names && **font_names && *labels && **labels; ++font_names, ++labels) {\n+\t\tconst char *name = *font_names;\t\t\t\t// NAME 1: neutral (D10)\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(*labels); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### label_ptr

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ label_ptr/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; font_names[i] && *font_names[i] && *labels && **labels; i++, labels++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(*labels); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### font_ptr

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ font_ptr/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n-\t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\tfor (int i = 0; *font_names && **font_names && labels[i] && *labels[i]; font_names++, i++) {\n+\t\tconst char *name = *font_names;\t\t\t\t// NAME 1: neutral (D10)\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### label_p

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ label_p/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n-\t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\tfor (const char **p = labels; font_names[p-labels] && *font_names[p-labels] && *p && **p; ++p) {\n+\t\tconst char *name = font_names[p-labels];\t\t\t\t// NAME 1: neutral (D10)\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(*p); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### font_p

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ font_p/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n-\t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\tfor (const char **p = font_names; *p && **p && labels[p-font_names] && *labels[p-font_names]; ++p) {\n+\t\tconst char *name = *p;\t\t\t\t// NAME 1: neutral (D10)\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[p-font_names]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_loops

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_loops/ui_option_font.cpp\n@@ -45,8 +45,8 @@\n {\n-\tmemset(Buttons, 0, sizeof(Buttons));\n-\tmemset(Sliders, 0, sizeof(Sliders));\n+\tfor (int j = 0; j < 4; j++) Buttons[j] = NULL;\n+\tfor (int k = 0; k < OPTION_FONT_SLIDERS; k++) Sliders[k] = NULL;\n \tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_button_loop

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_button_loop/ui_option_font.cpp\n@@ -45,3 +45,3 @@\n {\n-\tmemset(Buttons, 0, sizeof(Buttons));\n+\tfor (int j = 0; j < 4; j++) Buttons[j] = NULL;\n \tmemset(Sliders, 0, sizeof(Sliders));\n@@ -49,4 +49,4 @@\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_slider_loop

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_slider_loop/ui_option_font.cpp\n@@ -46,7 +46,7 @@\n \tmemset(Buttons, 0, sizeof(Buttons));\n-\tmemset(Sliders, 0, sizeof(Sliders));\n+\tfor (int j = 0; j < OPTION_FONT_SLIDERS; j++) Sliders[j] = NULL;\n \tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_button_assign

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_button_assign/ui_option_font.cpp\n@@ -45,3 +45,3 @@\n {\n-\tmemset(Buttons, 0, sizeof(Buttons));\n+\tButtons[0] = Buttons[1] = Buttons[2] = Buttons[3] = NULL;\n \tmemset(Sliders, 0, sizeof(Sliders));\n@@ -49,4 +49,4 @@\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_slider_assign

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_slider_assign/ui_option_font.cpp\n@@ -46,7 +46,7 @@\n \tmemset(Buttons, 0, sizeof(Buttons));\n-\tmemset(Sliders, 0, sizeof(Sliders));\n+\tSliders[0] = Sliders[1] = Sliders[2] = Sliders[3] = Sliders[4] = NULL;\n \tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_both_assign

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_both_assign/ui_option_font.cpp\n@@ -45,8 +45,8 @@\n {\n-\tmemset(Buttons, 0, sizeof(Buttons));\n-\tmemset(Sliders, 0, sizeof(Sliders));\n+\tButtons[0] = Buttons[1] = Buttons[2] = Buttons[3] = NULL;\n+\tSliders[0] = Sliders[1] = Sliders[2] = Sliders[3] = Sliders[4] = NULL;\n \tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_fill

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_fill/ui_option_font.cpp\n@@ -45,8 +45,8 @@\n {\n-\tmemset(Buttons, 0, sizeof(Buttons));\n-\tmemset(Sliders, 0, sizeof(Sliders));\n+\tstd::fill(Buttons, Buttons + 4, (GadgetClass *)NULL);\n+\tstd::fill(Sliders, Sliders + OPTION_FONT_SLIDERS, (GadgetSlider *)NULL);\n \tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### zero_elements

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ zero_elements/ui_option_font.cpp\n@@ -45,8 +45,8 @@\n {\n-\tmemset(Buttons, 0, sizeof(Buttons));\n-\tmemset(Sliders, 0, sizeof(Sliders));\n+\tmemset(Buttons, 0, 4 * sizeof(*Buttons));\n+\tmemset(Sliders, 0, OPTION_FONT_SLIDERS * sizeof(*Sliders));\n \tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### ne_null

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ ne_null/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; font_names[i] != NULL && *font_names[i] != 0 && labels[i] != NULL && *labels[i] != 0; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### ne_zero

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ ne_zero/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; font_names[i] != 0 && *font_names[i] != 0 && labels[i] != 0 && *labels[i] != 0; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### reverse_cmp

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ reverse_cmp/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; NULL != font_names[i] && 0 != *font_names[i] && NULL != labels[i] && 0 != *labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### negated

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ negated/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; !(!font_names[i] || !*font_names[i] || !labels[i] || !*labels[i]); i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### double_not

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ double_not/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; !!font_names[i] && !!*font_names[i] && !!labels[i] && !!*labels[i]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### indexed_chars

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ indexed_chars/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; font_names[i] && font_names[i][0] && labels[i] && labels[i][0]; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### char_ne

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ char_ne/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; font_names[i] && font_names[i][0] != 0 && labels[i] && labels[i][0] != 0; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```

### ternary_checks

```json
{"patch": "--- baseline/ui_option_font.cpp\n+++ ternary_checks/ui_option_font.cpp\n@@ -47,6 +47,6 @@\n \tmemset(Sliders, 0, sizeof(Sliders));\n-\tfor (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {\n+\tfor (int i = 0; font_names[i] ? (*font_names[i] ? (labels[i] ? *labels[i] != 0 : false) : false) : false; i++) {\n \t\tconst char *name = font_names[i];\t\t\t\t// NAME 1: neutral (D10)\n-\t\tFontNames.push_back(std::string(name));\n-\t\tLabels.push_back(std::string(labels[i]));\n+\t\t{ std::string font(name); FontNames.push_back(font); }\n+\t\t{ std::string label(labels[i]); Labels.push_back(label); }\n \t\tSettings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));\n"}
```
