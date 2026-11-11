// NOT EXACT: 0x58AE90, inline-budget: the client calls std::string::_Eos out of line for the font name's string where this inlines it (docs/attempts/58AE90)
void UIDynamicWindowClass::Add_Text(DynamicWindowClass *window, const char *font_name, int row_index, unsigned int flags, const char *format, ...)
{
	FAIL_IF(!window) return;
	char text[512];
	text[0] = '\0';
	if (format) {
		va_list args;
		va_start(args, format);
		_vsnprintf(text, 511, format, args);
		va_end(args);
	}
	RefCountPtr<DynamicWindowTextClass> element = DynamicWindowTextClass::Create(font_name, text, flags | 6);
	window->Add_Element(element, row_index);
}
