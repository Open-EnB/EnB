// @0x8335D0
// The tree's form (0.57). Before: find / replace written in Fill_Values (0.33; the client calls them out of line);
// match.first.empty() for length() == 0 changed nothing. Residue: the client keeps ebx (an allocator byte) and edi,
// a frame 8 bytes larger, and its loop is not rotated (one call of Find_Value_Tag).
// The first tag in text replaced by value_text, false when text has none (an inline layer: Replace_Value and Fill_Values
// call string::replace 0x834650 out of line through it)
inline bool Replace_Tag(std::string &text, const std::string &tag, const std::string &value_text)  // NAME 1: what it does
{
	int pos = text.find(tag);
	if (pos == std::string::npos) {
		return(false);
	}
	text.replace(pos, tag.length(), value_text);
	return(true);
}
static std::string Fill_Values(const std::string &text, const std::vector<float> &values)  // NAME 1: what it does
{
	std::string result = text;
	std::vector<float>::const_iterator value_iterator = values.begin();
	for (;;) {
		std::pair<std::string, std::string> match = Find_Value_Tag(result);
		if (match.first.empty()) {
			G_ASSERT(value_iterator == values.end());
			return(result);
		}


		G_ASSERT(value_iterator != values.end());
		float value = (value_iterator != values.end()) ? *value_iterator++ : 0.0f;
		char buffer[32];
		sprintf(buffer, match.second.c_str(), value);
		std::string value_text = buffer;
		Replace_Tag(result, match.first, value_text);
	}
}
