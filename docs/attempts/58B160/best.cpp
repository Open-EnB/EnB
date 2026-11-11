// 0x58B160
void UIDynamicWindowClass::Add_Text(const char *font_name, int row_index, unsigned int flags, const char *format, ...)
{
	FAIL_IF(!MainWindow) return;
	char text[512];
	text[0] = '\0';
	if (format) {
		va_list args;
		va_start(args, format);
		_vsnprintf(text, 511, format, args);
		va_end(args);
	}
	RefCountPtr<DynamicWindowTextClass> element = DynamicWindowTextClass::Create(font_name, text, flags | 6);
	MainWindow->Add_Element(element, row_index);
}
