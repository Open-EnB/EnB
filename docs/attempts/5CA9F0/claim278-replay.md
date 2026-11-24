# Claim 278: 0x5CA9F0 replay

Patch blocks encode exact unified-diff text as JSON; decode the `patch` field before applying.

Fresh HEAD bd393322; 27 valid candidates in four analyzed batches. Original and final unchanged controls each 55 exact symbol/address pairs. All variants have zero pair changes. One initial Wine socket failure and two source-generation assertion failures occurred before any valid compilation; excluded. No compiler failures or uncompiled candidate forms. Original app flags, basename and physical assertion lines preserved. Masked hashes identify output groups, not full proof.

| form | source SHA-256 | masked target SHA-256 | object span | similarity | gains / losses |
|---|---|---|---:|---:|---|
| baseline | `91b141f734b163abd9242741939edd393729b93027f8edb8606eb11b7515377e` | `d956950e1e5fefad966aacd174c3275c4f993a7ac24e65fab6d241867f481250` | 1232 | 0.69 | 0 / 0 |
| key_sub | `dfbdeb76f86b6345d710ad020046e831953dfc289e94ef766a05ca2f43f5727e` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| key_assign | `2450bc75a2d1d15d75aca7030cd5c18e531fc9b110cdb0fd1e41ab4d1ae495b2` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| key_plus | `d64c23553bc820ddfb8f6d8c79e32f31ee4a6e2cce1d05ebc6391606e73f6a37` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| equal_null | `266a4f87c357d942ac4b002b36f8f17f98acb6fbaeac1d44e8d0d4fea5ea34f5` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| equal_zero | `d31997e348f09be62ffcad7043b22ab3087a4ae28f34a7b74e4e89c570f34584` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| not_equal_else | `e3eabbe9b77d46b01eeb3a85ed755191d0e4a401bafecb56dc5f143b06cf0752` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| positive_else | `25f8045d638e82a4ccefaa8c9417403236f4eb10cdcc04bcba006fe3039152c5` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| negated_comparison | `fcc8043d48dea298963ce7d7bcc5b409ec41b77171fb1bf880be4dce772af71f` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| equal_reversed | `0316952f44ff7c4a26af58be6dad0039a4962fcbc5bb8a4cb354796038371b84` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| boolean_cast | `a98ddd6739bfc503e3fe71bc6c7a9277ae069c497956eeb0039f7ae501ef2c25` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| conditional_return | `389e19b852304c70cfe13239496a98440d396feea388d48aacca45631a55d187` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| late_ternary | `56aff1dc1cbba6bf4e43c07f7f22cc72b2899c48f3819c690e3b1872a87a1ab5` | `2aa8376f1a2bd907b3a8cfbd9a7ceb12e4d9bdab2d1342b697cbc972b2ad6f40` | 1232 | 0.68 | 0 / 0 |
| late_bool_math | `a806a633ad4c139c4320985f85d242b544bd9f61a09d95f4f9997ec78d674542` | `a26b3359ff59bbfb6b5ec6bcd9327fbf37fdcb9a3595d17277cba44a0731b5f3` | 1232 | 0.68 | 0 / 0 |
| late_range | `1e2830a3a975f5a1461304be6207223db429f117a050dbeff3f9f4a403994def` | `0d07106e434dd11e12d6a1c0095eb3a8d2870157ea2208e0fcb51988d9094d77` | 1232 | 0.67 | 0 / 0 |
| late_range2 | `f7899705e0e7dc436301aed4f68eecc35753bbece2ca48b9367177f52d194a49` | `c0454b307e3e4a5f4e0f7c2bcc41d5b549a139dce7d21cd6b0e700776fcc832e` | 1232 | 0.68 | 0 / 0 |
| late_ternary_row | `40a387aa5687ee2f5f226e566d00fa5422c10faa98fc61d9677c0cd58dc269a0` | `1756645c60a8917accc035e41a3c8b30d8ccbdc3649498df354b370dcb33ddb8` | 1232 | 0.69 | 0 / 0 |
| late_range2_row | `4af4c15110d65ac57d13fbf9cbfbaad93e108c0da64b74a61522af2552869e5f` | `6c0e775b08895b08e99618c78b1e87bb03321dbcb0eac176db025d4dea87951f` | 1232 | 0.69 | 0 / 0 |
| late_conditional_row | `352bdf1955da8e58cb97019ff8dbc70e0f3ed9aa513ed88281c7f6944a346bd2` | `eb47baa56477b8ba70b1d5e87cd1005800b26a1009781091fca5daf327a5abe4` | 1232 | 0.69 | 0 / 0 |
| post_local | `c663bcc876a7b6d7c07d23a62897898564e909c25c6e64b67078338471499e3d` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| assignment_condition | `d4e0320a043ec31a41e29b39b34059f5502654e28afd30b6257f6375d774281f` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| assignment_nonnull | `b45d4be24706a1816c91188eb62c3fd2f40151c70b895706e2305aa00fdcc6b0` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| iterator_check | `84acbd61e2d2fb56c2e1b5b49206ad91e9454abf69e831e2c26ee9c121166f15` | `25d4de9b7dee32a60867d51d735912776a284a8017788deddf2b4b6386bb4e5b` | 1232 | 0.66 | 0 / 0 |
| local_mapping | `a930120f01d72edcd595449f780cfc16c1bda794f101cbe1554cd0392ae4fd75` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| local_member_check | `c10dedd25e35ba77dfa2a224ce0b20e986c63d43120072fd9453fb2d8dda4cef` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| local_assignment_check | `c6132a6e146931b66340dd45349651ef75efc4cefd77f8301477d9b606ac8f67` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| compare_reversed | `c6a107eab924eedf379b1485c8ed3a96ddbea7ce0727a9d377404fe367134fef` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |
| compare_boolean | `7ad649393fb583d66aee3ea7307f9da934f2ead0d7d652663fef4a1a1bf8b641` | `0e07042d934b1937e78eeca5eed4a701c14230b6d88dc44872718c24e7748ab7` | 1232 | 0.68 | 0 / 0 |

Retail uses LEA EAX,[EBX-15/26], stores row into the key argument stack slot, tests EBP, and clears EBX after loading the mapping. key_sub instead subtracts in EBX and clears it before testing EBP; it also prepares Create_Peek temporary before the first push. Baseline and key_sub stack reservation is 8 bytes; calls remain present, though placement differs. No full byte, relocation, or EH identity claimed; maintainer verification remains authoritative.

Masked code groups:
- `d956950e1e5f`: baseline
- `0e07042d934b`: key_sub, key_assign, key_plus, equal_null, equal_zero, not_equal_else, positive_else, negated_comparison, equal_reversed, boolean_cast, conditional_return, post_local, assignment_condition, assignment_nonnull, local_mapping, local_member_check, local_assignment_check, compare_reversed, compare_boolean
- `2aa8376f1a2b`: late_ternary
- `a26b3359ff59`: late_bool_math
- `0d07106e434d`: late_range
- `c0454b307e3e`: late_range2
- `1756645c60a8`: late_ternary_row
- `6c0e775b0889`: late_range2_row
- `eb47baa56477`: late_conditional_row
- `25d4de9b7dee`: iterator_check

### key_sub

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ key_sub/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### key_assign

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ key_assign/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey = key - 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey = key - 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### key_plus

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ key_plus/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey += -15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey += -26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### equal_null

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ equal_null/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (bttn == NULL) return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### equal_zero

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ equal_zero/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (bttn == 0) return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### not_equal_else

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ not_equal_else/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (bttn != NULL) {} else return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### positive_else

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ positive_else/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (bttn) {} else return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### negated_comparison

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ negated_comparison/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (!(bttn != NULL)) return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### equal_reversed

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ equal_reversed/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (NULL == bttn) return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### boolean_cast

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ boolean_cast/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (bool(bttn) == false) return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### conditional_return

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ conditional_return/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -273,3 +273,3 @@\n \t}\n-\tif (!bttn) return(false);\n+\tif (bttn ? false : true) return(false);\n \n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n"}
```

### late_ternary

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_ternary/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = key - (EditPrimary ? 15 : 26);\n \n"}
```

### late_bool_math

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_bool_math/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = key - (26 - 11 * (int)EditPrimary);\n \n"}
```

### late_range

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_range/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = key - (key <= 25 ? 15 : 26);\n \n"}
```

### late_range2

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_range2/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = key - (key < 26 ? 15 : 26);\n \n"}
```

### late_ternary_row

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_ternary_row/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = key <= 25 ? key - 15 : key - 26;\n \n"}
```

### late_range2_row

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_range2_row/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = key < 26 ? key - 15 : key - 26;\n \n"}
```

### late_conditional_row

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ late_conditional_row/ui_option_control_keys.cpp\n@@ -264,3 +264,3 @@\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\t\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\t\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tlp = EditPrimary ? key - 15 : key - 26;\n \n"}
```

### post_local

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ post_local/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey = key - 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey = key - 26;\n \t\tEditPrimary = false;\n@@ -274,3 +274,3 @@\n \tif (!bttn) return(false);\n-\n+\tint lp = key;\n \n"}
```

### assignment_condition

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ assignment_condition/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -297,4 +297,3 @@\n \tif (keys_it != CurrentKeys.end()) {\n-\t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tif ((EditKey = *keys_it)) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n@@ -303,2 +302,3 @@\n \t\t}\n+\n \t\tG_ASSERT(false);\n"}
```

### assignment_nonnull

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ assignment_nonnull/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -297,4 +297,3 @@\n \tif (keys_it != CurrentKeys.end()) {\n-\t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tif ((EditKey = *keys_it) != NULL) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n@@ -303,2 +302,3 @@\n \t\t}\n+\n \t\tG_ASSERT(false);\n"}
```

### iterator_check

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ iterator_check/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -298,3 +298,3 @@\n \t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tif (*keys_it) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n"}
```

### local_mapping

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ local_mapping/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -295,6 +295,6 @@\n \t}\n-\n \tif (keys_it != CurrentKeys.end()) {\n-\t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tKeyMappingStruct *mapping = *keys_it;\n+\t\tEditKey = mapping;\n+\t\tif (mapping) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n"}
```

### local_member_check

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ local_member_check/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -295,5 +295,5 @@\n \t}\n-\n \tif (keys_it != CurrentKeys.end()) {\n-\t\tEditKey = *keys_it;\n+\t\tKeyMappingStruct *mapping = *keys_it;\n+\t\tEditKey = mapping;\n \t\tif (EditKey) {\n"}
```

### local_assignment_check

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ local_assignment_check/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -297,4 +297,4 @@\n \tif (keys_it != CurrentKeys.end()) {\n-\t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tKeyMappingStruct *mapping = *keys_it;\n+\t\tif ((EditKey = mapping)) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n"}
```

### compare_reversed

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ compare_reversed/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -298,3 +298,3 @@\n \t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tif (NULL != EditKey) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n"}
```

### compare_boolean

```json
{"patch": "--- baseline/ui_option_control_keys.cpp\n+++ compare_boolean/ui_option_control_keys.cpp\n@@ -261,6 +261,6 @@\n \tGadgetClass *bttn;\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n-\tint lp;\t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n+\t       \t\t\t\t\t\t\t\t\t\t\t\t// NAME 1: neutral (D10)\n \tif (key >= 15 && key <= 25) {\n \t\tbttn = Primary[key - 15];\n-\t\tlp = key - 15;\n+\t\tkey -= 15;\n \t\tEditPrimary = true;\n@@ -268,3 +268,3 @@\n \t\tbttn = Secondary[key - 26];\n-\t\tlp = key - 26;\n+\t\tkey -= 26;\n \t\tEditPrimary = false;\n@@ -290,3 +290,3 @@\n \tstd::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();\n-\tint count = (int)(Slider->Get_Value() + lp);\t\t// NAME 1: neutral (D10)\n+\tint count = (int)(Slider->Get_Value() + key);\t\t// NAME 1: neutral (D10)\n \twhile (count--) {\n@@ -298,3 +298,3 @@\n \t\tEditKey = *keys_it;\n-\t\tif (EditKey) {\n+\t\tif (!!EditKey) {\n \t\t\tKeyPress->Set_Text(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));\n"}
```
