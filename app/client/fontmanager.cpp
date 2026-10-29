// Earth & Beyond client (reconstruction): app\Client\fontmanager.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "..\..\wwlib\wwstring.h"
ManagedFontClass::ManagedFontClass(RenderObjClass *object)
{
	Styles.clear();
	Name = object->Get_Name();
	Id = (unsigned int)object;
	Unknown08 = 0;
	for (std::hash_map<std::string, FontStringStruct *>::iterator it = Unknown_BE67B8->Strings.begin(); it != Unknown_BE67B8->Strings.end(); ++it) {
		FontStringStruct *def = it->second;
		if (stricmp(def->RenderObj, Name) == 0) {
			if (def->StringID[0] == 'W') {
				Unknown_6CF930(def, (int)object);
			} else {
				Unknown_6D1220(new FontStyleClass(*def, Id, 0));
			}
		}
	}
}

ManagedFontClass::ManagedFontClass(const char *name, int unknown)
{
	FontStringStruct *def = Unknown_BE67B8->Find_String(name);
	Styles.clear();
	if (def) {
		Unknown_6CF930(def, unknown);
	}
}

ManagedFontClass::~ManagedFontClass()
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
		style->second->Unknown_6D5140();
	}
	Styles.clear();
	Id = 0;
}

void ManagedFontClass::Unknown_6D01C0(int level, const char *name)
{
	if (name == NULL) {
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			style->second->Set_Sort_Level(level);
		}
	} else {
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			if (strcmp(name, style->second->Info.StringID) == 0) {
				style->second->Set_Sort_Level(level);
			}
		}
	}
}

void ManagedFontClass::Unknown_6D0390(const char *name)
{
	if (name == NULL) {
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			style->second->Set_Hidden(false);
		}
	} else {
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			if (strcmp(name, style->second->Info.StringID) == 0) {
				style->second->Set_Hidden(false);
			}
		}
	}
}

void ManagedFontClass::Unknown_6D0550(const char *name)
{
	if (name == NULL) {
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			style->second->Set_Hidden(true);
		}
	} else {
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			if (strcmp(name, style->second->Info.StringID) == 0) {
				style->second->Set_Hidden(true);
			}
		}
	}
}

void ManagedFontClass::Unknown_Set_Styles(int value)
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
		style->second->Unknown250 = value;
	}
}

void ManagedFontClass::Unknown_6D07D0(int unknown)
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
		style->second->Unknown_6CE7D0(unknown);
	}
}

void ManagedFontClass::Unknown_6D0890()
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
		style->second->Unknown_6D5D40();
	}
}

void ManagedFontClass::Unknown_All_Styles(int unknown1, int unknown2)
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
		style->second->Unknown_6CD950(unknown1, unknown2);
	}
}

void ManagedFontClass::Unknown_6D0A20(const char *name, const char *text, int unknown)
{
	Unknown_6D1600(name)->Change_Text(text, unknown);
}

// NOT EXACT: inline budget, 58 rows: the client inlines std::string(const char *) for the key, ours calls it (and two of the six 4-string loops swap lea operands; attempts.csv)
void ManagedFontClass::Unknown_6D0A70(const char *name, const char *text)
{
	FontStringStruct def;
	std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.begin();
	while (it != Styles.end()) {
		FontStyleClass *style = (*it).second;
		std::hash_map<std::string, FontStyleClass *>::iterator next = ++it;
		if (strcmp(name, style->Info.StringID) == 0) {
			def = style->Info;
			style->Unknown_6D5140();
			std::string key = "";
			key = def.Unknown04.Is_Empty() ? (const char *)def.StringID : (const char *)def.Unknown04;
			Styles.erase(key);
		}
		it = next;
	}
	def.Text = text;
	Unknown_6CF930(&def, Id);
}

// NOT EXACT: inline budget: the client calls the std::string(const char *, allocator) constructor out of line, ours inlines it (attempts.csv)
void ManagedFontClass::Unknown_6D1010(const char *name, TPoint3D<int> color)
{
	FontStyleClass *style = Unknown_6D1600(name);
	style->Unknown494.Color = color;
	style->Unknown4A4 = style->Unknown494;
}

// NOT EXACT: shape, 18 rows: the client stores the color through [ebp+0x494] directly, ours through the member's address (attempts.csv)
void ManagedFontClass::Unknown_6D10B0(const char *name, TPoint3D<int> color)
{
	for (std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.begin(); it != Styles.end(); ++it) {
		FontStyleClass *style = it->second;
		if (strcmp(name, style->Info.StringID) == 0) {
			style->Set_Color(color);
		}
	}
}




























































































































































































void ManagedFontClass::Unknown_6D1220(FontStyleClass *style)
{
	if (style && style->Info.StringID) {
		std::string key = "";
		if (!style->Info.Unknown04.Is_Empty()) {
			key = style->Info.Unknown04;
		} else {
			key = style->Info.StringID;
		}
		if (Unknown_6D1600(key)) {





			G_ASSERT(0);
		} else {
			Styles[key] = style;
		}
	}
}

// NOT EXACT: shape, 38 rows: the key copy and its destruction differ (attempts.csv)
FontStyleClass *ManagedFontClass::Unknown_6D1480(std::string name)
{
	std::string key = name;
	key += "_0";
	std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.find(key);
	if (it == Styles.end()) return NULL;
	return it->second;
}

// NOT EXACT: shape, 39 rows: the client destroys the name in one shared tail, ours in each return (attempts.csv)
FontStyleClass *ManagedFontClass::Unknown_6D1600(std::string name)
{
	std::hash_map<std::string, FontStyleClass *>::iterator it = Styles.find(name);
	if (it == Styles.end()) return Unknown_6D1480(name);
	return it->second;
}
















FontManagerClass::FontManagerClass()
{
	Fonts.clear();
	DebugFont = Create_NEW(new FontStyleClass("DebugFont", 0, 0));
}

FontManagerClass::~FontManagerClass()
{
	DebugFont = NULL;
	Remove_All();
}

void FontManagerClass::Remove_All()
{
	for (std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		delete it->second;
	}
	Fonts.clear();
}

void FontManagerClass::Unknown_6D1C10(int unknown1, int unknown2)
{
	for (std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.begin(); it != Fonts.end(); ++it) {
		it->second->Unknown_All_Styles(unknown1, unknown2);
	}
}

// NOT EXACT: register-only, 3 rows: the client loads the id into ecx and keeps the font in eax (attempts.csv)
void FontManagerClass::Add(RenderObjClass *object)
{
	ManagedFontClass *font = new ManagedFontClass(object);
	if (font && font->Id) {
		Fonts[font->Id] = font;
	}
}

// NOT EXACT: register-only, 2 rows: the client reloads the buffer into eax, ours into edx (attempts.csv)
void FontManagerClass::Add(const RefCountPtr<RenderObjClass> &object)
{
	Add(object.Peek());
}

ManagedFontClass *FontManagerClass::Create(const char *name, int unknown)
{
	ManagedFontClass *font = new ManagedFontClass(name, unknown);
	Add(font);
	return font;
}

void FontManagerClass::Remove(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it != Fonts.end()) {
		delete it->second;
		Fonts.erase(it);
	}
}

void FontManagerClass::Unknown_6D2310(const unsigned int &id)
{
	unsigned int key = id;
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(key);
	if (it != Fonts.end()) {
		delete it->second;
		Fonts.erase(it);
	}
}

void FontManagerClass::Unknown_6D2400(unsigned int id)
{
	Peek_Font(id)->Unknown_6D0390(NULL);
}

void FontManagerClass::Unknown_6D2480(unsigned int id)
{
	Peek_Font(id)->Unknown_6D0550(NULL);
}

void FontManagerClass::Unknown_6D2500(const char *name, const char *text, unsigned int id, int unknown)
{
	Peek_Font(id)->Unknown_6D0A20(name, text, unknown);
}

// Find 0x6D2A70 out of line here and in 0x6D2700 (auto_inline off); Peek_Font is its inlined twin elsewhere (H19)
void FontManagerClass::Unknown_6D2580(const char *name, TPoint3D<int> color, unsigned int id)
{
	Find(id)->Unknown_6D1010(name, color);
}

inline ManagedFontClass *FontManagerClass::Peek_Font(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it == Fonts.end()) return NULL;
	return it->second;
}
























// NOT EXACT: order-only, 1 row: the inlined std::string(name) stores its terminator through [ebx+eax], ours [eax+ebx] (attempts.csv)
FontStyleClass *FontManagerClass::Get_Style(const char *name, unsigned int id)
{
	ManagedFontClass *font = Peek_Font(id);
	if (!font) {
		G_ASSERT(0);
		return DebugFont.Peek();
	}

	FontStyleClass *style = font->Unknown_6D1600(name);
	if (!style) {



		G_ASSERT(0);
		return DebugFont.Peek();
	}
	return style;
}
// The style test after the if (font) block (the font-less path skips it); the key copy keeps ecx = this for Find (H21)
RefCountPtr<FontStyleClass> FontManagerClass::Unknown_6D2700(const char *name, const unsigned int &id)
{
	RefCountPtr<FontStyleClass> style;
	unsigned int key = id;
	ManagedFontClass *font = Find(key);
	if (font) {
		style = Create_Peek(font->Unknown_6D1600(name));
	}
	if (style) {
		return style;
	}
	G_ASSERT(0);
	return DebugFont;
}

// NOT EXACT: inline budget, 75 rows: the client calls the styles table's begin and bucket helpers out of line, ours inlines them (attempts.csv)
void FontManagerClass::Unknown_6D28C0(int value, unsigned int id)
{
	Peek_Font(id)->Unknown_Set_Styles(value);
}

void FontManagerClass::Add(ManagedFontClass *font)
{
	if (font && font->Id) {
		Fonts[font->Id] = font;
	}
}

#pragma auto_inline(off)
ManagedFontClass *FontManagerClass::Find(unsigned int id)
{
	std::hash_map<unsigned int, ManagedFontClass *>::iterator it = Fonts.find(id);
	if (it == Fonts.end()) return NULL;
	return it->second;
}
#pragma auto_inline(on)
/*
**	Reconstruction notes
**
**	client.exe: the window 0x6CE990-0x6D2A70 (asserts on lines 360, 524, 533 and 550), then the
**	containers' instances. 0x6D0A70 is this file's: it calls the table instances 0x6D2D70, 0x6D3350,
**	0x6D3730 and 0x6D3DB0, which the linker placed after 0x6D2A70, so its object is this one (so is
**	ManagedFontClass's block from 0x6CE990: its constructor calls 0x6D2BA0 / 0x6D3F40, 0x6CF520 calls
**	0x6D3350). 0x6D0F70 is FontSoundStruct's implicit operator=, emitted after its first caller
**	0x6D0A70 (FU1): 0x6D0A70 inlines FontStringStruct's copy only through the explicit inline operator=
**	in fontdatabase.h; with the implicit one it is called out of line and fontdatabase.cpp's Load_Init
**	0x6BF330 moves one load (H21's order-only knock-on; attempts.csv, docs/attempts/6D0F70). 0x6D2700 exact (H21: a flat style test and a key copy). 0x6D2400 / 0x6D2480 / 0x6D2500 exact once their callees 0x6D0390 / 0x6D0550 / 0x6D0A20 are
**	defined (FU1). FontStyleClass is a DynamicMeshClass (0x6D47C0 builds the base, vtable 0xAFECB4): its
**	+0x16C is Set_Sort_Level (0x6D01C0), its +0x184 Set_Hidden (0x6D0390 / 0x6D0550; its override 0x67EF40).
**	The font is keyed by its render object's address: the constructor 0x6CE990 takes the object (its Get_Name
**	is the font's name, its address the id) and loads the database's templates whose RenderObj is that name;
**	FontStringStruct's implicit copy constructor 0x6CECC0 and the styles table's destructor 0x6CEBF0 follow it.
**	The depth lever (workflow.md 3.1) made the destructor and 0x6D1C10 exact: the client calls the
**	nested table helpers out of line when they sit one inline level deeper.
**	Find 0x6D2A70 is never inlined (#pragma auto_inline(off)): the client calls it out of line in
**	0x6D2580 and 0x6D2700 and inlines the same body in the trio, Get_Style and 0x6D28C0, written as
**	the inline twin Peek_Font (builder H19: 0x6D2580 exact; no budget form inside 0x6D2580 sent
**	Find out of line: dead reads, a depth wrapper, a constructed color, a RefCountPtr local).
*/
