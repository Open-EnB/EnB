#include "chat_window.h"
#include "..\shared\g_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ChatWindowClass::ChatWindowClass(GadgetSlider *owner, int unknown, int index) :
	ChatWindowHistoryClass(unknown),
	Rect(0.0f, 0.0f, 0.0f, 0.0f),
	Unknown74(owner),
	LineHeight(0.0f),
	Unknown90(0),
	Unknown94(0),
	Unknown98(0),
	Unknown9C(true),
	Unknown9D(false),
	Unknown9E(false),
	UnknownA0(0),
	UnknownA4(0),
	UnknownA8(1),
	Name("Chat_Line_")
{
	char buffer[12];
	sprintf(buffer, "%02d", index);
	Name += buffer;
	Unknown_67E670();
}

ChatWindowClass::~ChatWindowClass()
{
	Clear_Fonts();
	Scene = NULL;
	Unknown74 = NULL;
}

void ChatWindowClass::Unknown_67E180(const RefCountPtr<SceneClass> &scene)
{
	Scene = scene;
	for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		const RefCountPtr<ChatFontSys> &font_ptr = *it;










































































































		FAIL_IF(!font_ptr) continue;
		font_ptr->Remove();
	}
	Unknown9C = true;
}

void ChatWindowClass::Unknown_0C(const Vector3 &color, const std::string &text)
{
	Unknown9D = true;
	ChatWindowHistoryClass::Unknown_0C(color, text);
}

int ChatWindowClass::Unknown_08(const Vector3 &color, const std::string &text)
{
	int count = ChatWindowHistoryClass::Unknown_08(color, text);
	if (count == 0) {
		return 0;
	}
	if (Fonts.empty()) {
		return count;
	}
	Unknown9C = true;
	if (Unknown4C != 0) {
		Unknown74->Unknown_6702D0();
		Unknown_680790();
		return count;
	}
	for (int i = 0; i < count; i++) {
		const RefCountPtr<ChatLineClass> &chat_line = Unknown_682770(i);


















		FAIL_IF(chat_line->Is_Null()) break;
		const RefCountPtr<ChatFontSys> &line_ptr = Get_Font(chat_line->Unknown08);

		FAIL_IF(!line_ptr) continue;
		line_ptr->Set_Line_Always(chat_line);
	}
	Unknown90 = Unknown_682730(Unknown_682840())->Unknown08;
	Unknown_6809B0();
	Unknown_680790();
	return count;
}

int ChatWindowClass::Unknown_10(int unknown)
{
	int result = ChatWindowHistoryClass::Unknown_10(unknown);
	if (result != 0) {
		Unknown_67FC60();
		Unknown_680790();
	}
	return result;
}

bool ChatWindowClass::Unknown_14(const char *command)
{
	if (strncmp(command, "/chat render", strlen("/chat render")) == 0) {
		UnknownA0 = atoi(command + strlen("/chat render") + 1);
		return true;
	}
	if (strncmp(command, "/chat extra", strlen("/chat extra")) == 0) {
		int value = atoi(command + strlen("/chat extra") + 1);
		if (UnknownA4 != value) {
			UnknownA4 = value;
			Unknown_04(Unknown48, Unknown54);
		}
		return true;
	}
	if (strncmp(command, "/chat build", strlen("/chat build")) == 0) {
		UnknownA8 = atoi(command + strlen("/chat build") + 1);
		return true;
	}
	return false;
}

bool ChatWindowClass::Unknown_04(unsigned int max_lines, unsigned int width)
{
	bool changed = ChatWindowHistoryClass::Unknown_04(max_lines, width);
	if (Unknown_67F7F0(max_lines)) {
		changed = true;
	}
	if (changed) {
		Unknown_67FC60();
		Unknown_680790();
	}
	return changed;
}

// The clamp is macro-style, 8 / width in each arm: the client tests it against 1 twice (0x67E78C, 0x67E79D)
void ChatWindowClass::Unknown_67E670()
{
	RefCountPtr<ChatFontSys> font = Create_NEW(new ChatFontSys(Name.c_str()));
	Font = *(const RefCountPtr<Font32Class> *)&font->Font;  // FontSys::Font is held like a RefCountPtr here (0x67E71F)
	int width = (int)(Font->Get_Char_Width(' ', 640.0f) < 1.0f ? 1.0f : Font->Get_Char_Width(' ', 640.0f));
	int count = (unsigned int)(8 / width < 1 ? 1 : 8 / width) > 3 ?
		3 : (8 / width < 1 ? 1 : 8 / width);
	memset(UnknownBC, ' ', count);
	UnknownBC[count] = 0;
	Set_Font(Font);
}

void ChatWindowClass::Clear_Fonts()
{
	for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		const RefCountPtr<ChatFontSys> &font_ptr = *it;


























		FAIL_IF(!font_ptr) continue;
		font_ptr->Set_Line(RefCountPtr<ChatLineClass>());
	}
	Fonts.erase(Fonts.begin(), Fonts.end());
}

// NOT EXACT: registers only (max_lines, old_size and the old font rotate; the client divides by the stack copy)
bool ChatWindowClass::Unknown_67F7F0(unsigned int max_lines)
{
	max_lines += UnknownA4 * 2;
	if (max_lines < 4) {
		max_lines = 4;
	}
	unsigned int old_size = Fonts.size();
	if (old_size == max_lines) {
		return false;
	}
	if (old_size < max_lines) {
		max_lines += 4;
	} else if (old_size < max_lines + 4) {
		return false;
	}
	std::vector< RefCountPtr<ChatFontSys> > old;
	old.swap(Fonts);
	Fonts.resize(max_lines);
	for (unsigned int i = 0; i < old_size; i++) {
		RefCountPtr<ChatFontSys> &font = old[i];
		if (font.Peek()) {
			if (font->Line.Peek()) {
				unsigned int index = font->Line->Unknown08 % max_lines;
				if (!Fonts[index].Peek()) {
					Fonts[index] = font;
				} else {
					font->Set_Line(RefCountPtr<ChatLineClass>());
				}
			}
			font = NULL;
		}
	}
	for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		if (!it->Peek()) {
			*it = Create_NEW(new ChatFontSys(Name.c_str()));
		}
	}
	return true;
}

void ChatWindowClass::Unknown_67FC60()
{
	int count = Fonts.size();
	if (count != 0) {
		int pos = Unknown_682840();
		int extra = UnknownA4;
		int line = pos - extra;
		line -= (Unknown4C < extra) ? Unknown4C : extra;
		if (line < 0) {
			line = 0;
		}
		unsigned int id = 0;
		int i = 0;
		for (; i < count; i++, line++) {
			const RefCountPtr<ChatLineClass> &chat_line = Unknown_682730(line);
			if (chat_line->Is_Null()) {
				break;
			}
			id = chat_line->Unknown08;
			if (line == pos) {
				Unknown90 = id;
			}
			const RefCountPtr<ChatFontSys> &line_ptr = Get_Font(id);


















































			FAIL_IF(!line_ptr) continue;
			line_ptr->Set_Line_Always(chat_line);
		}
		id++;
		for (; i < count; i++, id++) {
			const RefCountPtr<ChatFontSys> &line_ptr = Get_Font(id);


			FAIL_IF(!line_ptr) continue;
			line_ptr->Set_Line(RefCountPtr<ChatLineClass>());
		}
		Unknown_6809B0();
		Unknown9C = true;
		Unknown9D = false;
	}
}

void ChatWindowClass::Unknown_67FFD0()
{
	if (Unknown9E) {
		return;
	}
	if (Unknown9D) {
		Unknown_67FC60();
	}
	if (Unknown9C) {
		int limit = UnknownA0 ? UnknownA0 : Unknown50;
		unsigned int id = Unknown90;
		int done = 0;
		unsigned int i = 0;
		for (; i < Unknown50 && done < limit; i++, id++) {
			const RefCountPtr<ChatFontSys> &font_ptr = Get_Font(id);












			FAIL_IF(!font_ptr) continue;
			if (font_ptr->Unknown_680320(UnknownBC, Scene)) {
				done++;
			}
		}
		if (done != 0) {
			return;
		}
		unsigned int size = Fonts.size();
		int extra = UnknownA8 ? UnknownA8 : size;
		done = 0;
		for (; i < size && done < extra; i++, id++) {
			const RefCountPtr<ChatFontSys> &font_ptr = Get_Font(id);



			FAIL_IF(!font_ptr) continue;
			if (font_ptr->Unknown_680320(UnknownBC, RefCountPtr<SceneClass>())) { done++; }
		}
		if (done != 0) { return; }
		Unknown9C = false;
	}
}
bool ChatFontSys::Unknown_680320(const char *indent, RefCountPtr<SceneClass> scene)
{
	bool changed = false;
	if (Line.Peek() && !Line->Unknown29) {
		if (Line->Flags & 1) {
			Line->Flags &= ~1;
			Set_Color(Line->Color);
			const char *text = Line->Text.c_str();
			while (*text == ' ') { text++; }
			if (Line->Is_First()) {
				Unknown_6CE420(0, "%c%s", Line->Is_Bit_3() ? '^' : ' ', text);
			} else {
				Unknown_6CE420(0, "%c%s%s", Line->Is_Bit_3() ? '^' : ' ', indent, text);
			}
			changed = true;
		}
		if (scene.Peek() && Peek_Scene() == NULL) {
			Add(scene.Peek());
			changed = true;
		}
	}
	return changed;
}
// The height and the count are each Top - Bottom: the client stores both (fst [esp+0xC]; fstp [esp+8])
void ChatWindowClass::Unknown_6804E0(const RectClass &rect)
{
	Rect = rect;
	float line_height = Font->Get_Line_Height(480.0f);
	float height = Rect.Top - Rect.Bottom;
	float count = Rect.Top - Rect.Bottom;
	int lines = 1;
	LineHeight = line_height;
	if (count > 0.0f && line_height > 0.0001f) {
		count /= line_height;
		if (count >= 1.0f) { lines = (int)count; }
	}
	Vector3 extent((Rect.Right - Rect.Left) * 0.5f, height * 0.5f, 0.0f);
	Vector3 center(extent.X + Rect.Left, Rect.Top - extent.Y, 0.0f);
	AABoxClass box(center, extent);
	Unknown74->Unknown_670230(box, false);
	int width = (int)((Rect.Right - Rect.Left) - Font->Get_Text_Width(UnknownBC, 640.0f));
	if (width < 1) { width = 1; }
	Unknown_04(lines, width);
}
void ChatWindowClass::Unknown_680670()
{
	if (Unknown_10(Unknown94 - Get_Slider_Value())) { Unknown9C = true; }
	Unknown94 = Get_Slider_Value();
}
void ChatWindowClass::Unknown_6806C0()
{
	Clear_Fonts();
	Unknown_67E670();
	Unknown_67F7F0(Unknown48);
	Unknown_6804E0(Rect);
	Unknown9D = true;
}
void ChatWindowClass::Unknown_680700(bool unknown)
{
	Unknown9E = unknown;
	Unknown_680790();
	if (Unknown9E) {
		for (std::vector< RefCountPtr<ChatFontSys> >::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
			const RefCountPtr<ChatFontSys> &font_ptr = *it;



			FAIL_IF(!font_ptr) continue;
			font_ptr->Remove();
		}
	} else {
		Unknown9C = true;
	}
}

void ChatWindowClass::Unknown_680790()
{
	if (Unknown74 == NULL) {
		return;
	}
	int total = History.size() + Unknown48;
	int pos = Unknown_682840();
	if (Unknown98 != total) {
		Unknown98 = total;
		bool at_end = false;
		if ((float)Get_Slider_Value() == Unknown74->UnknownC0 - Unknown74->UnknownC8) {
			at_end = true;
		}
		Unknown74->Unknown_66FA90((float)total, (float)Unknown48, true);
		if (at_end) {
			Unknown74->Unknown_A4((float)(Unknown98 - Unknown48), true);
		}
	} else if (pos != Get_Slider_Value()) {
		Unknown74->Unknown_A4((float)pos, false);
	}
	Unknown94 = Get_Slider_Value();
	Unknown74->Unknown_8C(Unknown9E);
}

const RefCountPtr<ChatFontSys> &ChatWindowClass::Get_Font(unsigned int index)
{
	static RefCountPtr<ChatFontSys> null_font;
	unsigned int size = Fonts.size();















	FAIL_IF(!size) return null_font;
	return Fonts[index % size];
}

void ChatWindowClass::Unknown_6809B0()
{
	Vector3 pos(Rect.Left, Rect.Top + 3.0f, 0.0f);
	unsigned int id = Unknown90;
	unsigned int i = 0;
	for (; i < Unknown50; i++, id++) {
		const RefCountPtr<ChatFontSys> &font_ptr = Get_Font(id);







		FAIL_IF(!font_ptr) continue;
		font_ptr->Set_Position(pos);
		pos.Y -= LineHeight;
	}
	unsigned int size = Fonts.size();
	for (; i < size; i++, id++) {
		const RefCountPtr<ChatFontSys> &font_ptr = Get_Font(id);



		FAIL_IF(!font_ptr) continue;
		font_ptr->Remove();
	}
}

/*
**	Reconstruction notes (client.exe, the object 0x67DC80-0x68128F; vtable 0xAFBEF0).
**	The client's __FILE__ 0xB8DAC8 and its first literals: 'Chat_Line_' 0xB8DAAC (the constructor), '!font_ptr' 0xB8DABC.
**	The FAIL_IF on line 293 is Clear_Fonts's (the destructor inlines it; its own copy is 0x67F680).
*/
