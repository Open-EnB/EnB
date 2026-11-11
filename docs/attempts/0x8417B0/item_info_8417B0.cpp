// @0x8417B0
// The tree's form (0.78, the same 287 instructions; 4 rows differ at the second substr: the client keeps
// field_separator in esi and takes field_separator + 1 into eax (lea), ours increments esi in place).
void ItemBaseInfoClass::Apply_Delta(const std::string &delta)
{
	Reset_Fields();
	std::vector<std::string> fields;
	Unknown_4FC2F0(fields, delta, std::string(DeltaSeparator));
	for (std::vector<std::string>::iterator it = fields.begin(); it != fields.end(); ++it) {
		int field_separator = it->find(':');
#line 543
		FAIL_IF(field_separator == std::string::npos) continue;
		std::string id = it->substr(0, field_separator);
		int item_info = atoi(id.c_str());
#line 555
		FAIL_IF(item_info <= ITEM_INFO_NONE) {
			continue;
		}

		FAIL_IF(item_info >= NUM_ITEM_INFO) {
			continue;
		}
		std::string value = it->substr(field_separator + 1);
		if (_ItemField.Is_String(item_info)) {
			Set_String(item_info, value.c_str());
			continue;
		}
		if (_ItemField.Is_Float(item_info)) {
			Set_Float(item_info, atof(value.c_str()), false);
			continue;
		}
		if (_ItemField.Is_Int(item_info)) {
			Set_Int(item_info, atoi(value.c_str()), false);
		}
	}
}

// Forms tried (all with the loop above):
//  1. a switch over the field's type with Set_Int / Set_Float / Set_String cases: 0.61 (the client tests the
//     type as an if chain, string, float, int: Is_String / Is_Float / Is_Int).
//  2. if (Is_String) ... else if (Is_Float) ... else if (Is_Int) ... without the continues: 0.65 (one copy of
//     the scope's destructors; the client has two, the string / float one shared with the FAIL_IFs' exits and
//     an inline-expanded one after the int branch: the continues above give exactly that).
//  3. form of the tree with substr(field_separator + 1, it->size()): 0.62.
// What is left is the register of field_separator + 1: something after the substr still reads
// field_separator in the client's source (dead in the object), or the + 1 is a separate unsigned local.
