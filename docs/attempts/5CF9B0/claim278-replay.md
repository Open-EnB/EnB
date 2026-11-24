# Claim 278 — 0x5CF9B0 replay evidence

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

```diff
--- baseline/ui_pda_cargo.cpp
+++ assign/ui_pda_cargo.cpp
@@ -80,3 +80,3 @@
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+		FilterName.assign(options->Get_String("Inv_Filter_Cargo"));
 	}
```

### assign_range

```diff
--- baseline/ui_pda_cargo.cpp
+++ assign_range/ui_pda_cargo.cpp
@@ -80,3 +80,3 @@
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+		FilterName.assign(options->Get_String("Inv_Filter_Cargo"), 0, std::string::npos);
 	}
```

### array_memset

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_memset/ui_pda_cargo.cpp
@@ -74,5 +74,5 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
-	}
+	memset(Buttons5, 0, sizeof(Buttons5));
+
+
 	OptionClass *options = OptionClass::Instance; if (options) {
```

### array_chain

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_chain/ui_pda_cargo.cpp
@@ -74,5 +74,5 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
-	}
+	Buttons5[0] = Buttons5[1] = Buttons5[2] = Buttons5[3] = Buttons5[4] = NULL;
+
+
 	OptionClass *options = OptionClass::Instance; if (options) {
```

### named

```diff
--- baseline/ui_pda_cargo.cpp
+++ named/ui_pda_cargo.cpp
@@ -80,3 +80,3 @@
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+		std::string value = options->Get_String("Inv_Filter_Cargo"); FilterName = value;
 	}
```

### init_default

```diff
--- baseline/ui_pda_cargo.cpp
+++ init_default/ui_pda_cargo.cpp
@@ -72,3 +72,3 @@
 	SortIndex(0),
-	SortReverse(false), FilterName("")
+	SortReverse(false)
 {
```

### init_temp

```diff
--- baseline/ui_pda_cargo.cpp
+++ init_temp/ui_pda_cargo.cpp
@@ -72,3 +72,3 @@
 	SortIndex(0),
-	SortReverse(false), FilterName("")
+	SortReverse(false), FilterName(std::string(""))
 {
```

### init_allocator

```diff
--- baseline/ui_pda_cargo.cpp
+++ init_allocator/ui_pda_cargo.cpp
@@ -72,3 +72,3 @@
 	SortIndex(0),
-	SortReverse(false), FilterName("")
+	SortReverse(false), FilterName("", std::allocator<char>())
 {
```

### if_instance

```diff
--- baseline/ui_pda_cargo.cpp
+++ if_instance/ui_pda_cargo.cpp
@@ -77,6 +77,6 @@
 	}
-	OptionClass *options = OptionClass::Instance; if (options) {
-		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
-		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+	if (OptionClass::Instance) {
+		SortIndex = OptionClass::Instance->Get_Bool("Inv_Sort_Cargo");
+		SortReverse = OptionClass::Instance->Get_Bool("Inv_Sort_Cargo_Reverse");
+		FilterName = OptionClass::Instance->Get_String("Inv_Filter_Cargo");
 	}
```

### if_ne_null

```diff
--- baseline/ui_pda_cargo.cpp
+++ if_ne_null/ui_pda_cargo.cpp
@@ -77,3 +77,3 @@
 	}
-	OptionClass *options = OptionClass::Instance; if (options) {
+	OptionClass *options = OptionClass::Instance; if (options != NULL) {
 		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
```

### if_notnull

```diff
--- baseline/ui_pda_cargo.cpp
+++ if_notnull/ui_pda_cargo.cpp
@@ -77,3 +77,3 @@
 	}
-	OptionClass *options = OptionClass::Instance; if (options) {
+	OptionClass *options = OptionClass::Instance; if (!!options) {
 		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
```

### if_early

```diff
--- baseline/ui_pda_cargo.cpp
+++ if_early/ui_pda_cargo.cpp
@@ -77,3 +77,3 @@
 	}
-	OptionClass *options = OptionClass::Instance; if (options) {
+	OptionClass *options = OptionClass::Instance; if (!options) { Unknown_BE67BC->Load("fa_pdainv.ini"); return; } {
 		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
```

### array_index_down

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_index_down/ui_pda_cargo.cpp
@@ -74,4 +74,4 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
+	for (int i = NUM_BUTTONS; i > 0; i--) {
+		Buttons5[i - 1] = NULL;
 	}
```

### array_ptr

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_ptr/ui_pda_cargo.cpp
@@ -74,4 +74,4 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
+	for (ButtonGadgetClass **p = Buttons5; p != Buttons5 + NUM_BUTTONS; p++) {
+		*p = NULL;
 	}
```

### array_do

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_do/ui_pda_cargo.cpp
@@ -74,5 +74,5 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
-	}
+	int i = 0; do {
+		Buttons5[i++] = NULL;
+	} while (i < NUM_BUTTONS);
 	OptionClass *options = OptionClass::Instance; if (options) {
```

### array_while

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_while/ui_pda_cargo.cpp
@@ -74,4 +74,4 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
+	int i = 0; while (i < NUM_BUTTONS) {
+		Buttons5[i++] = NULL;
 	}
```

### array_post

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_post/ui_pda_cargo.cpp
@@ -74,4 +74,4 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
+	for (int i = 0; i != NUM_BUTTONS;) {
+		Buttons5[i++] = NULL;
 	}
```

### array_fill

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_fill/ui_pda_cargo.cpp
@@ -74,5 +74,5 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
-	}
+	std::fill(Buttons5, Buttons5 + NUM_BUTTONS, (ButtonGadgetClass *)NULL);
+
+
 	OptionClass *options = OptionClass::Instance; if (options) {
```

### array_size

```diff
--- baseline/ui_pda_cargo.cpp
+++ array_size/ui_pda_cargo.cpp
@@ -74,4 +74,4 @@
 {
-	for (int i = 0; i < NUM_BUTTONS; i++) {
-		Buttons5[i] = NULL;
+	for (unsigned int i = 0; i < sizeof(Buttons5) / sizeof(Buttons5[0]); ++i) {
+		Buttons5[i] = 0;
 	}
```

### string_bound

```diff
--- baseline/ui_pda_cargo.cpp
+++ string_bound/ui_pda_cargo.cpp
@@ -80,3 +80,3 @@
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+		const std::string &value = options->Get_String("Inv_Filter_Cargo"); FilterName = value;
 	}
```

### string_named_assign

```diff
--- baseline/ui_pda_cargo.cpp
+++ string_named_assign/ui_pda_cargo.cpp
@@ -80,3 +80,3 @@
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+		std::string value = options->Get_String("Inv_Filter_Cargo"); FilterName.assign(value);
 	}
```

### string_bound_assign

```diff
--- baseline/ui_pda_cargo.cpp
+++ string_bound_assign/ui_pda_cargo.cpp
@@ -80,3 +80,3 @@
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
-		FilterName = options->Get_String("Inv_Filter_Cargo");
+		const std::string &value = options->Get_String("Inv_Filter_Cargo"); FilterName.assign(value);
 	}
```

### sort_ternary

```diff
--- baseline/ui_pda_cargo.cpp
+++ sort_ternary/ui_pda_cargo.cpp
@@ -78,3 +78,3 @@
 	OptionClass *options = OptionClass::Instance; if (options) {
-		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
+		SortIndex = options->Get_Bool("Inv_Sort_Cargo") ? 1 : 0;
 		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
```

### sort_reverse_bool

```diff
--- baseline/ui_pda_cargo.cpp
+++ sort_reverse_bool/ui_pda_cargo.cpp
@@ -79,3 +79,3 @@
 		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
-		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse");
+		SortReverse = options->Get_Bool("Inv_Sort_Cargo_Reverse") != false;
 		FilterName = options->Get_String("Inv_Filter_Cargo");
```

### options_declare

```diff
--- baseline/ui_pda_cargo.cpp
+++ options_declare/ui_pda_cargo.cpp
@@ -77,3 +77,3 @@
 	}
-	OptionClass *options = OptionClass::Instance; if (options) {
+	OptionClass *options; if ((options = OptionClass::Instance) != NULL) {
 		SortIndex = options->Get_Bool("Inv_Sort_Cargo");
```

### init_range

```diff
--- baseline/ui_pda_cargo.cpp
+++ init_range/ui_pda_cargo.cpp
@@ -72,3 +72,3 @@
 	SortIndex(0),
-	SortReverse(false), FilterName("")
+	SortReverse(false), FilterName("", (std::string::size_type)0)
 {
```
