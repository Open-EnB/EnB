// @0x833880
// The tree's form. Residue: the client calls the two strings' constructor basic_string(const allocator &) 0x603BB0 out of line from the first statement on, ours inlines it.
static std::pair<std::string, std::string> Find_Value_Tag(const std::string &text)  // NAME 1: what it does; NAME 1 [tag, format]
{
	static int tag_length = ValueTag.length();
	std::string tag;
	std::string format;
	int match_start = text.find(ValueTag);
	if (match_start == std::string::npos) {
		return(std::pair<std::string, std::string>(tag, format));
	}
	int match_end = text.find('%', match_start + 1);
	FAIL_IF(match_end == std::string::npos) {
		tag.substr(match_start);
		format = "0.1f";
		return(std::pair<std::string, std::string>(tag, format));
	}
	tag = text.substr(match_start, match_end - match_start + 1);
	format = std::string("%") + tag.substr(tag_length).substr(0, tag.substr(tag_length).length() - 1);
	return(std::pair<std::string, std::string>(tag, format));
}
