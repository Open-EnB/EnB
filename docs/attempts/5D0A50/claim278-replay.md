# Claim 278 — 0x5D0A50 replay evidence

HEAD 79dc642f; one baseline control plus 25 valid candidates; no invalid or uncompiled inputs. All builds use original app flags and original basename. Physical G_ASSERT / FAIL_IF lines verified before each batch. Each candidate below is a complete patch against the tracked baseline; duplicate output hashes are canonical compiler duplicates. Hash covers target bytes with relocation fields zeroed, including alignment. Relocation identities and EH were not audited because no target matched.

| form | source SHA-256 | masked target SHA-256 | object bytes | objdiff similarity | symbol gains / losses |
|---|---|---|---:|---:|---|
| baseline | `d9a2a49fede6eb1d45b9d73f5fd14ed104822358dec93a01c0dd4699069db467` | `3b8aa113a881e189d5dc09ae150c2bc32c9342efe59b7351c39e34ac3ce31b6f` | 592 | 0.28 | 0 / 0 |
| branch1 | `ce2953dc8862942d12ed7384a959d2a5821d863e10e05ba7df877a6d2d71f569` | `adda3b46554819f5029be53e4dbf2d8e121b5d5558fa4093b23899b96e451f25` | 592 | 0.30 | 0 / 0 |
| branch2 | `a94acaf88022d124b0a94e987219c1e6cd7f2e3a79c9f416a001d5e71c81b161` | `66d822fa6df65d464ab7ec4f93d9f72681fcf91f1167814c20bf72ba0d63aef7` | 544 | 0.29 | 0 / 0 |
| branch3 | `818f638aebfc81931861b38ce5db17ef9c738d2285b474493247d319857b770a` | `66d822fa6df65d464ab7ec4f93d9f72681fcf91f1167814c20bf72ba0d63aef7` | 544 | 0.29 | 0 / 0 |
| helper000 | `1122bac4e9e8eb9c0a918cf36ff6d7ca68b55e141f0c2b569597fd7b19960203` | `9fac03852f5755f2af2f1b2edba8f7235c0bf2fda8f540d9272f82812e7f1047` | 544 | 0.65 | 0x5466A0,0x5481C0 / 0 |
| helper001 | `ad3b57812212915e754ef42f7b9aafb684f507fae914779d8b0fbac6ef58859c` | `f9f5e1d3c4d44dd527d0fc7ef6dfb172667d5eb8784978322bc37ad2e11920d2` | 544 | 0.62 | 0x5466A0,0x5481C0 / 0 |
| helper010 | `93b577ca939eb9a0f38f99f139a486beae8d996ac0de2176984aaf2aa51a702a` | `352ef4220c23e5fbe05bb029a76e6e9a77445072651bee1552e0505aece898a4` | 592 | 0.30 | 0x5481C0 / 0 |
| helper011 | `2effb023f24d01138cbbedd4a31585a3679ab24fd074a3b05f5e11745b224297` | `ee646adbd47a865e94c62888f4414e5aa3219af785fa32eda2b16d795cd3b41c` | 576 | 0.33 | 0x5481C0 / 0 |
| helper100 | `4c3b19a377219fc07fda992823df38de8bf40f0ffedff6fbb01c1697ba58441d` | `43ea77377107a7f601ca24323e8085fea838905eb9889ee23f0c231f5084a3ca` | 560 | 0.66 | 0x5466A0 / 0 |
| helper101 | `c05591d26f63c5d3448ec658c54d16aa9c91426799594b154b13a57dfdf5146c` | `b1b082895aa720fcb3d579a4acb4ad335329ab18c7154ef1414711f2494d5938` | 544 | 0.65 | 0x5466A0 / 0 |
| order0 | `e21c513e02731ffd94a865534b950c7a3b6d5c02db2002ee8e41e774d2006a92` | `0fafa564ffe6b708a1adb6c4a921694d1aca87109653337293af6500c1e44342` | 560 | 0.33 | 0x5481C0 / 0 |
| order1 | `ded9038971cb1f5d9be60a8d444b16953a61714f5275dcd6be4def0bb2473c41` | `480fbd92eeaf188d3a3f72d857b06ed276e757d68922eb9ec11e91f4e08b5a84` | 560 | 0.33 | 0x5481C0 / 0 |
| order2 | `975a8ab0eca86eddc7daaecef4802b4021a174817d9b97d3d7d16ccad50f9622` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |
| order3 | `63ad7e5baf49484e88dd9b775d28f11e2aad520f825076adf8717d8eae9337b9` | `4dc1a75051a25d4c2eaeae6548eadd798a2883f60eab912ffda81c40ae228bda` | 576 | 0.68 | 0x5466A0,0x5481C0 / 0 |
| order4 | `1d146d4ca272efd2c9f66f148e36294b639cfa8406955ece592ec7f94fe25eaf` | `689d6bc571dddddf4fe56f6f074f770ba3ecdfd63682742601a3eb816c76880e` | 560 | 0.32 | 0x5481C0 / 0 |
| order5 | `b01d64c7f87361f807806250198b3df0a5e0691ba7124e812ce82777bde9c6fc` | `d3b451913d3ece5c4ab9e447f500a280db132b423565324bd07ba840af2320d7` | 560 | 0.31 | 0x5481C0 / 0 |
| order6 | `8afe02d20d4d0708cf3eb231e074e052fce9e275f148e9cd33cbb8a8e55851e2` | `d9746461bf126364b8ec0731f48f1c7406989dca65f82b5d0df5b3966691180f` | 592 | 0.34 | 0x5481C0 / 0 |
| order7 | `3babb6485c11e78fe20163feae5fe0b5d96c51477ffa7e714067f77d0dd9b8b2` | `2c36ecb036ba462fac1039e89d1f105be4939069a8f88351c3f079b934e566b5` | 576 | 0.68 | 0x5466A0 / 0 |
| valid0 | `1ba3f77f6c9bc73e695e656ae3d1a49707903aa72778b4769b4db07fc93ce85b` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |
| valid1 | `f0cc59dcbd09bca97e6d634493d403079d3aeef389700792009634f9af0088a8` | `48ec5de7617882bca6dfa161c4c74b3c938418ef171d9de5989f3a159e031432` | 560 | 0.61 | 0x5466A0,0x5481C0 / 0 |
| valid2 | `b96a5449e6d297a889cf4f06412f9d05403666e4a0fa6af00351f5c5f6c1db3a` | `48ec5de7617882bca6dfa161c4c74b3c938418ef171d9de5989f3a159e031432` | 560 | 0.61 | 0x5466A0,0x5481C0 / 0 |
| valid3 | `e85e1b949210043f58d9dff5169ec56fc1f15332b2f213539cb489316a74ea25` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |
| valid4 | `2a39476b19c5cb27cf4edd844afab12804a40930705a80fe99bd6186a41e0ba3` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |
| valid5 | `e93320339b77da3c6c287841dc71324394f27e09848e6b393f70fcf1fd939831` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |
| valid6 | `9a608e2226f61500cbd06bc98e47f9a971d6de21e3ab2ce02a74110d99c5da49` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |
| valid7 | `b5911a434ef4bfe0f6776110bc85fc21bb52f1e2bcbc1cbc11e77987f7c34a87` | `37282d8fa233a944e63bbdaee90add8c4bcba56abfb65ca8714a7d75b2be8fc4` | 544 | 0.71 | 0x5466A0,0x5481C0 / 0 |

Retail target executable span 569 bytes (0x5D0A50..0x5D0C88 inclusive); candidate object lengths include COFF alignment padding. The best order2 form has matching 0x5c frame, register setup, money-copy, string assembly, and text update; valid lookup is inlined where retail calls Unknown_Find_Field and changed lookup is called where retail inlines it. Similarity is a heuristic; it does not prove relocation or EH identity. Helper-emission gains are outside claim and were not retained. No production edit was kept. Native comparison is local; maintainer VC6 verification remains authoritative.

### branch1

```diff
--- baseline/ui_pda_cargo.cpp
+++ branch1/ui_pda_cargo.cpp
@@ -296,22 +296,22 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) {
+        NameClass name("Money");
+        field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
+        if (!field || !field->Unknown70) field = NULL;
+    } else {
+        NameClass name("Money");
+        if (!aux) return;
+        field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
+        if (!field || field->Unknown70 < 2) return;
+    }
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### branch2

```diff
--- baseline/ui_pda_cargo.cpp
+++ branch2/ui_pda_cargo.cpp
@@ -296,22 +296,22 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) {
+        NameClass name("Money");
+        field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
+        if (field && field->Unknown70) money = field->Value;
+    } else {
+        NameClass name("Money");
+        if (!aux) return;
+        field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
+        if (!field || field->Unknown70 < 2) return;
+        money = field->Value;
+    }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### branch3

```diff
--- baseline/ui_pda_cargo.cpp
+++ branch3/ui_pda_cargo.cpp
@@ -296,22 +296,21 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) {
+        NameClass name("Money");
+        field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
+        if (field && field->Unknown70) money = field->Value;
+    } else {
+        NameClass name("Money");
+        field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
+        if (!field || field->Unknown70 < 2) return;
+        money = field->Value;
+    }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### helper000

```diff
--- baseline/ui_pda_cargo.cpp
+++ helper000/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,13 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (!field) return; }
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### helper001

```diff
--- baseline/ui_pda_cargo.cpp
+++ helper001/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,12 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (field) money = field->Value; } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (!field) return; money = field->Value; }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### helper010

```diff
--- baseline/ui_pda_cargo.cpp
+++ helper010/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,13 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && field->Unknown70 <= 1) field = NULL; if (!field) return; }
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### helper011

```diff
--- baseline/ui_pda_cargo.cpp
+++ helper011/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,12 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (field) money = field->Value; } else { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && field->Unknown70 <= 1) field = NULL; if (!field) return; money = field->Value; }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### helper100

```diff
--- baseline/ui_pda_cargo.cpp
+++ helper100/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,13 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && !field->Unknown70) field = NULL; } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (!field) return; }
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### helper101

```diff
--- baseline/ui_pda_cargo.cpp
+++ helper101/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,12 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && !field->Unknown70) field = NULL; if (field) money = field->Value; } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (!field) return; money = field->Value; }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order0

```diff
--- baseline/ui_pda_cargo.cpp
+++ order0/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,13 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (!all) { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (!field) return; } else { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order1

```diff
--- baseline/ui_pda_cargo.cpp
+++ order1/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,12 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (!all) { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (!field) return; money = field->Value; } else { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (field) money = field->Value; }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order2

```diff
--- baseline/ui_pda_cargo.cpp
+++ order2/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order3

```diff
--- baseline/ui_pda_cargo.cpp
+++ order3/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (!all) { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order4

```diff
--- baseline/ui_pda_cargo.cpp
+++ order4/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,13 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (!all) { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && field->Unknown70 <= 1) field = NULL; if (!field) return; } else { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order5

```diff
--- baseline/ui_pda_cargo.cpp
+++ order5/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,12 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (!all) { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && field->Unknown70 <= 1) field = NULL; if (!field) return; money = field->Value; } else { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); if (field) money = field->Value; }
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order6

```diff
--- baseline/ui_pda_cargo.cpp
+++ order6/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && field->Unknown70 <= 1) field = NULL; }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### order7

```diff
--- baseline/ui_pda_cargo.cpp
+++ order7/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (!all) { NameClass name("Money"); field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name); if (field && field->Unknown70 <= 1) field = NULL; } else { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid0

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid0/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *const &root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70) ? t : NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid1

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid1/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); if (!t) return NULL; if (!t->Unknown70) return NULL; return t; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid2

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid2/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *const &root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); if (!t) return NULL; if (!t->Unknown70) return NULL; return t; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid3

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid3/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return t; return NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid4

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid4/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *const &root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return t; return NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid5

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid5/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); DeltaCompoundClass *r = root; C *t = Unknown_Find_Field<C>(r, n); if (t && t->Unknown70) return t; return NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid6

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid6/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *const &root, const char *str) { NameClass n(str); DeltaCompoundClass *r = root; C *t = Unknown_Find_Field<C>(r, n); if (t && t->Unknown70) return t; return NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```

### valid7

```diff
--- baseline/ui_pda_cargo.cpp
+++ valid7/ui_pda_cargo.cpp
@@ -292,3 +292,3 @@
 
-// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
+template<class C> inline C *Cargo_Find_Valid(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); if (t) { if (t->Unknown70) return t; } return NULL; } template<class C> inline C *Cargo_Find_Changed(DeltaCompoundClass *root, const char *str) { NameClass n(str); C *t = Unknown_Find_Field<C>(root, n); return (t && t->Unknown70 > 1) ? t : NULL; } // NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
 // 0x5D0A50
@@ -296,22 +296,14 @@
 {
-	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
-	NameClass name("Money");
-	DeltaTemplateClass<MoneyClass> *field = NULL;
-	if (all) {
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-	} else {
-		if (!aux) return;
-		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
-		if (!field || field->Unknown70 < 2) return;
-	}
-	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
-	if (font) {
-		MoneyClass money;
-		if (field) {
-			money = field->Value;
-		}
-		std::string text = "Credits: ";
-		text += money.Amount_As_Text(true);
-		font->Change_Text(text.c_str(), 0);
-	}
+    DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
+    MoneyClass money;
+    DeltaTemplateClass<MoneyClass> *field;
+    if (all) { field = Cargo_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money"); } else { field = Cargo_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money"); }
+    if (!all && !field) return;
+    if (field) money = field->Value;
+    FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
+    if (font) {
+        std::string text = "Credits: ";
+        text += money.Amount_As_Text(true);
+        font->Change_Text(text.c_str(), 0);
+    }
 }
```
