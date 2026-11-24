# Claim 278 — 0x5CF9B0 replay evidence

Patch blocks encode exact unified-diff text as JSON; decode the `patch` field before applying.

Fresh HEAD 14fd2e8a. One initial and one final baseline control, 26 valid candidates, one compiler failure (ambiguous zero-length constructor), and one sandbox setup failure. Native VC6 SP4 compilation uses original app flags and basename; physical assert lines verified. All successful candidates matched against the same whole-unit baseline. Hash covers target bytes with relocation fields zeroed including alignment; identical hashes are compiler duplicates. Similarity is heuristic, not byte proof.

| form | source SHA-256 | masked target SHA-256 | object bytes | similarity | symbol gains / losses |
|---|---|---|---:|---:|---|
| baseline | `d9a2a49fede6eb1d45b9d73f5fd14ed104822358dec93a01c0dd4699069db467` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| assign | `efce4359108d32877f1ec7356e559da4b44c5c7c150ce040bba2756d7c11c612` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| assign_range | `1eedc63fe0d10ae75ae3efa5d9434dec4ee87e64b01370e58869ee35e15fd31c` | `f3e3e321b14f0b8df332ab96f8c35e213ce4b0c985105459d635ff89d61ff00b` | 688 | 0.50 | 0x42CD30 ?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ / 0 |
| array_memset | `958db90590d0e83fc22f4e37ac3b8c720aa951f14c6e5b76eccc613f2390dd3d` | `85ff6f4c880107da2cd89ce9ab728186839ff2a492e608d558d4ff98cc8aa45e` | 592 | 0.74 | 0 / 0 |
| array_chain | `17d1f4625a89091b97fe5bfba344ead6d185cbc7e83cbf3056287c26c88862c8` | `6ba2345ef4a936ab72f46d53df5f7298ac41f769672e32bb2e96c302bddffd95` | 608 | 0.74 | 0 / 0 |
| named | `713a29cc906b2864c565170081cf953abf431e2d856f02c1cb3c5cbf5adf36f8` | `6a844307e7228d83d4207dfa08a589a03bf77c3cc9fa704d85a2278914576224` | 608 | 0.39 | 0 / 0 |
| init_default | `a4f97208fc743ea5c4711eaf786f7f6f97e2be5df0aafd958d92a99f18c36ede` | `2dff6c91f80c6e89297d4373aac30e65e7e64cb9462335deb30ae60df9aca0ce` | 528 | 0.37 | 0 / 0 |
| init_temp | `136a9366b2c75462b0e398d7c4e2c078020fd331493586b0422d903c42edd0a8` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| init_allocator | `c781c2958261839be063829ebef2263506a65557f29d99eee9db8ac3094c2a02` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| if_instance | `0e60375e605ce8ae7e74998d9ae6b3c2a54b597aaab88e3c5f3ecdfb61841fc2` | `861cc5e09e0fe1677798396a47bb8da485f06b93e3e8da10595eaf7e79153db5` | 608 | 0.70 | 0 / 0 |
| if_ne_null | `1bc62e51cd820c7d791661fa2f270e6a5165e767e648908b6cf72e6a8bdd556b` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| if_notnull | `637ccfa35e10a00d8be2035fa52ce2151e2f2c76e246656b2c9471e4e4c68540` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| if_early | `2ee57009b646c5114de3d6135c4f6fac2ab6a5a566f67e1faf23fe9bf000b3da` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| array_index_down | `a516b24883f386c8dd7e765801d1b31cf7c1593669a453a8133f727dc55e7d3e` | `9a4bdfe86b97b4a2a8f653f788b781642d060ad9a2079fe8254b6d6f160cf67c` | 592 | 0.72 | 0 / 0 |
| array_ptr | `6f5a71c79c9b5f92f35c30d023f364f3f3d0d0f8556393426d6b5eb8449cfe56` | `cc325d5d94510187b8798ad5474e22b39584a282d0aa511664a946b601031685` | 592 | 0.73 | 0 / 0 |
| array_do | `3a0e2f21d6d9e0d204cafcccf6c59e54ca9f8196c50922ea6c9b293a4b2b932e` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| array_while | `f41ba320bcaea7803ae89cf595c25b97359e95be1d112dff2fab82ba67b6b76b` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| array_post | `5fecf9131d72f94e6ff4b2ba123abc0c8f1f39768b8b2d8e8f31fccb4979eaa5` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| array_fill | `e49c0f9b1b18d6168207d8a2d1efc59c1968fa9aa7248ff4401f6ad3eea2653b` | `09817af6dc40a3fdc0452409b145a110830c69fbd6ae2b4225f30c9a48f7d764` | 592 | 0.76 | 0 / 0 |
| array_size | `46da0dc5a0e19c766a474676981fa6803cfd23530c5bf4be641f74c164ff9ec0` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| string_bound | `e660bafc0486fee34735ed0fbcfa46ce1b6e85c796e8a3a245098f51b80f9628` | `6a844307e7228d83d4207dfa08a589a03bf77c3cc9fa704d85a2278914576224` | 608 | 0.39 | 0x5D1000 _$E278 / 0x5D1000 _$E277 |
| string_named_assign | `a1faec76bfc626f6adb8ff1a19727d250006c25584acb87325dca5976db05686` | `6a844307e7228d83d4207dfa08a589a03bf77c3cc9fa704d85a2278914576224` | 608 | 0.39 | 0 / 0 |
| string_bound_assign | `545a798c84dca6eaa9b0c22e2abdce3479d9b23d3872c67b7bc924f42759c8d1` | `6a844307e7228d83d4207dfa08a589a03bf77c3cc9fa704d85a2278914576224` | 608 | 0.39 | 0x5D1000 _$E278 / 0x5D1000 _$E277 |
| sort_ternary | `c5e72f97eb5c4ad4e6558379ba56100366d4e85b646916013d3cf4d477189840` | `0153863810ef4fd38e0810cf4e095223a2c3b0be03150a7c7a4183ddca3190a7` | 608 | 0.68 | 0 / 0 |
| sort_reverse_bool | `f02e86080b3ec3e94bf1327b2d1a2577342356713432ace354c0ab5eba16998b` | `52eff64b0265458ca100ef5ce4af785f2eeebea6969a870d140726dbf6493639` | 608 | 0.74 | 0 / 0 |
| options_declare | `4b29da324fac8131444e7a3bc9f9f9d2d459b01907dba00a7ca0787dcb512545` | `e4d2704f0db719d55b1b55df8a958f1e27c8ca80004d5f7e27a54850b68e0654` | 608 | 0.73 | 0 / 0 |
| init_range | `8b5a1c705691301a9bbab0a5c2573bcbdd0225e397f71b130f1b895adefde286` | `35f1a548178cb245d6ebbaa1673208d63320207a31884479ed6c39720b2aea23` | 560 | 0.40 | 0 / 0 |

Retail calls string::_Eos during initial empty-string construction and inlines its later assignment; baseline does the opposite. Retail zeros five buttons using EDX, while the baseline uses ECX. The fill form (0.76) retains an actual loop unlike retail and does not solve either string inversion. Explicit assign range emits unrelated 0x42CD30; no production code retained. `init_nullcount` failed with VC6 C2668 and is not counted. EH/relocation identity audits are not claims of exactness because no target matched. Maintainer verification remains authoritative.

### assign

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ assign/ui_pda_cargo.cpp\n@@ -80,3 +80,3 @@\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\t\tFilterName.assign(options->Get_String(\"Inv_Filter_Cargo\"));\n \t}\n"}
```

### assign_range

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ assign_range/ui_pda_cargo.cpp\n@@ -80,3 +80,3 @@\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\t\tFilterName.assign(options->Get_String(\"Inv_Filter_Cargo\"), 0, std::string::npos);\n \t}\n"}
```

### array_memset

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_memset/ui_pda_cargo.cpp\n@@ -74,5 +74,5 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n-\t}\n+\tmemset(Buttons5, 0, sizeof(Buttons5));\n+\n+\n \tOptionClass *options = OptionClass::Instance; if (options) {\n"}
```

### array_chain

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_chain/ui_pda_cargo.cpp\n@@ -74,5 +74,5 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n-\t}\n+\tButtons5[0] = Buttons5[1] = Buttons5[2] = Buttons5[3] = Buttons5[4] = NULL;\n+\n+\n \tOptionClass *options = OptionClass::Instance; if (options) {\n"}
```

### named

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ named/ui_pda_cargo.cpp\n@@ -80,3 +80,3 @@\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\t\tstd::string value = options->Get_String(\"Inv_Filter_Cargo\"); FilterName = value;\n \t}\n"}
```

### init_default

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ init_default/ui_pda_cargo.cpp\n@@ -72,3 +72,3 @@\n \tSortIndex(0),\n-\tSortReverse(false), FilterName(\"\")\n+\tSortReverse(false)\n {\n"}
```

### init_temp

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ init_temp/ui_pda_cargo.cpp\n@@ -72,3 +72,3 @@\n \tSortIndex(0),\n-\tSortReverse(false), FilterName(\"\")\n+\tSortReverse(false), FilterName(std::string(\"\"))\n {\n"}
```

### init_allocator

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ init_allocator/ui_pda_cargo.cpp\n@@ -72,3 +72,3 @@\n \tSortIndex(0),\n-\tSortReverse(false), FilterName(\"\")\n+\tSortReverse(false), FilterName(\"\", std::allocator<char>())\n {\n"}
```

### if_instance

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ if_instance/ui_pda_cargo.cpp\n@@ -77,6 +77,6 @@\n \t}\n-\tOptionClass *options = OptionClass::Instance; if (options) {\n-\t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n-\t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\tif (OptionClass::Instance) {\n+\t\tSortIndex = OptionClass::Instance->Get_Bool(\"Inv_Sort_Cargo\");\n+\t\tSortReverse = OptionClass::Instance->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n+\t\tFilterName = OptionClass::Instance->Get_String(\"Inv_Filter_Cargo\");\n \t}\n"}
```

### if_ne_null

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ if_ne_null/ui_pda_cargo.cpp\n@@ -77,3 +77,3 @@\n \t}\n-\tOptionClass *options = OptionClass::Instance; if (options) {\n+\tOptionClass *options = OptionClass::Instance; if (options != NULL) {\n \t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n"}
```

### if_notnull

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ if_notnull/ui_pda_cargo.cpp\n@@ -77,3 +77,3 @@\n \t}\n-\tOptionClass *options = OptionClass::Instance; if (options) {\n+\tOptionClass *options = OptionClass::Instance; if (!!options) {\n \t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n"}
```

### if_early

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ if_early/ui_pda_cargo.cpp\n@@ -77,3 +77,3 @@\n \t}\n-\tOptionClass *options = OptionClass::Instance; if (options) {\n+\tOptionClass *options = OptionClass::Instance; if (!options) { Unknown_BE67BC->Load(\"fa_pdainv.ini\"); return; } {\n \t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n"}
```

### array_index_down

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_index_down/ui_pda_cargo.cpp\n@@ -74,4 +74,4 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n+\tfor (int i = NUM_BUTTONS; i > 0; i--) {\n+\t\tButtons5[i - 1] = NULL;\n \t}\n"}
```

### array_ptr

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_ptr/ui_pda_cargo.cpp\n@@ -74,4 +74,4 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n+\tfor (ButtonGadgetClass **p = Buttons5; p != Buttons5 + NUM_BUTTONS; p++) {\n+\t\t*p = NULL;\n \t}\n"}
```

### array_do

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_do/ui_pda_cargo.cpp\n@@ -74,5 +74,5 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n-\t}\n+\tint i = 0; do {\n+\t\tButtons5[i++] = NULL;\n+\t} while (i < NUM_BUTTONS);\n \tOptionClass *options = OptionClass::Instance; if (options) {\n"}
```

### array_while

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_while/ui_pda_cargo.cpp\n@@ -74,4 +74,4 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n+\tint i = 0; while (i < NUM_BUTTONS) {\n+\t\tButtons5[i++] = NULL;\n \t}\n"}
```

### array_post

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_post/ui_pda_cargo.cpp\n@@ -74,4 +74,4 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n+\tfor (int i = 0; i != NUM_BUTTONS;) {\n+\t\tButtons5[i++] = NULL;\n \t}\n"}
```

### array_fill

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_fill/ui_pda_cargo.cpp\n@@ -74,5 +74,5 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n-\t}\n+\tstd::fill(Buttons5, Buttons5 + NUM_BUTTONS, (ButtonGadgetClass *)NULL);\n+\n+\n \tOptionClass *options = OptionClass::Instance; if (options) {\n"}
```

### array_size

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ array_size/ui_pda_cargo.cpp\n@@ -74,4 +74,4 @@\n {\n-\tfor (int i = 0; i < NUM_BUTTONS; i++) {\n-\t\tButtons5[i] = NULL;\n+\tfor (unsigned int i = 0; i < sizeof(Buttons5) / sizeof(Buttons5[0]); ++i) {\n+\t\tButtons5[i] = 0;\n \t}\n"}
```

### string_bound

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ string_bound/ui_pda_cargo.cpp\n@@ -80,3 +80,3 @@\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\t\tconst std::string &value = options->Get_String(\"Inv_Filter_Cargo\"); FilterName = value;\n \t}\n"}
```

### string_named_assign

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ string_named_assign/ui_pda_cargo.cpp\n@@ -80,3 +80,3 @@\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\t\tstd::string value = options->Get_String(\"Inv_Filter_Cargo\"); FilterName.assign(value);\n \t}\n"}
```

### string_bound_assign

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ string_bound_assign/ui_pda_cargo.cpp\n@@ -80,3 +80,3 @@\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n-\t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n+\t\tconst std::string &value = options->Get_String(\"Inv_Filter_Cargo\"); FilterName.assign(value);\n \t}\n"}
```

### sort_ternary

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ sort_ternary/ui_pda_cargo.cpp\n@@ -78,3 +78,3 @@\n \tOptionClass *options = OptionClass::Instance; if (options) {\n-\t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n+\t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\") ? 1 : 0;\n \t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n"}
```

### sort_reverse_bool

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ sort_reverse_bool/ui_pda_cargo.cpp\n@@ -79,3 +79,3 @@\n \t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n-\t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\");\n+\t\tSortReverse = options->Get_Bool(\"Inv_Sort_Cargo_Reverse\") != false;\n \t\tFilterName = options->Get_String(\"Inv_Filter_Cargo\");\n"}
```

### options_declare

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ options_declare/ui_pda_cargo.cpp\n@@ -77,3 +77,3 @@\n \t}\n-\tOptionClass *options = OptionClass::Instance; if (options) {\n+\tOptionClass *options; if ((options = OptionClass::Instance) != NULL) {\n \t\tSortIndex = options->Get_Bool(\"Inv_Sort_Cargo\");\n"}
```

### init_range

```json
{"patch": "--- baseline/ui_pda_cargo.cpp\n+++ init_range/ui_pda_cargo.cpp\n@@ -72,3 +72,3 @@\n \tSortIndex(0),\n-\tSortReverse(false), FilterName(\"\")\n+\tSortReverse(false), FilterName(\"\", (std::string::size_type)0)\n {\n"}
```
