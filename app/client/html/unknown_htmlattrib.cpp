// Earth & Beyond client (reconstruction): app\client\html\unknown_htmlattrib.cpp. Notes at the end of the file.
#include "unknown_htmlattrib.h"

static uint32 Wstring_Hash(const Wstring &string);

char *Get_Mime_Type(char *filename)
{
	char *ext = filename + strlen(filename) - 1;
	while ((*ext != '.') && (ext != filename))
		ext--;
	if (stricmp(ext, ".gif") == 0)
		return("image/gif");
	return("text/html");
}

HtmlAttributeStackClass::HtmlAttributeStackClass(void) : Values(Wstring_Hash)
{
}

HtmlAttributeStackClass::~HtmlAttributeStackClass(void)
{
}

void HtmlAttributeStackClass::Add_All(HtmlAttributeStackClass &other)
{
	int index = 0;
	int offset = 0;
	Wstring key;
	ArrayList<Wstring> value;

	while (other.Values.iterate(index, offset, key, value))
		Values.add(key, value);
}

void HtmlAttributeStackClass::Set(char *key, char *value, int length)
{
	Wstring k;
	Wstring v;

	k.set(key);
	if (length == -1)
		v.set(value);
	else
		v.set(length, value);

	Values.remove(k);
	ArrayList<Wstring> list;
	list.addTail(v);
	Values.add(k, list);
}

char *HtmlAttributeStackClass::Get(char *key)
{
	Wstring k;
	k.set(key);

	ArrayList<Wstring> *list;
	if (Values.getPointer(k, &list)) {
		Wstring *s;
		list->getPointer(&s, list->length() - 1);
		return(s->get());
	}
	return(NULL);
}

void HtmlAttributeStackClass::Push(char *key, char *value, int length)
{
	Wstring k;
	Wstring v;

	k.set(key);
	if (length == -1)
		v.set(value);
	else
		v.set(length, value);

	ArrayList<Wstring> *list = NULL;
	if (!Values.getPointer(k, &list)) {
		ArrayList<Wstring> empty;
		Values.add(k, empty);
		Values.getPointer(k, &list);
	}
	list->addTail(v);
}

bit8 HtmlAttributeStackClass::Pop(char *key, char *value, int length)
{
	Wstring k;
	Wstring v;

	k.set(key);
	if (value != NULL) {
		if (length == -1)
			v.set(value);
		else
			v.set(length, value);
	}

	ArrayList<Wstring> *list;
	if (!Values.getPointer(k, &list))
		return(FALSE);

	if (value == NULL) {
		bool result = list->remove(list->length() - 1);
		if (list->length() == 0)
			Values.remove(k);
		return(result);
	}

	for (int i = list->length() - 1; i >= 0; i--) {
		Wstring *s;
		list->getPointer(&s, i);
		if (*s == v) {
			bool result = list->remove(i);
			if (list->length() == 0)
				Values.remove(k);
			return(result);
		}
	}
	return(FALSE);
}

bool HtmlAttributeStackClass::Remove(char *key)
{
	Wstring k;
	k.set(key);
	return(Values.remove(k));
}

void HtmlAttributeStackClass::Clear(void)
{
	Values.clear();
}

static uint32 Wstring_Hash(const Wstring &string)
{
	uint32 length = string.length();
	uint32 retval = length;
	for (uint32 i = 0; i < ((length < 20) ? length : 20); i++) {
		retval += *(string.get() + i) + i;
		retval = (retval << 8) ^ (retval >> 24);
	}
	return(retval);
}

/*
**	Reconstruction notes (HV1, 2026-11-01).
**	* The object: client 0x7791F0-0x77B726, between htmlview.cpp (its own functions to 0x7778E0, then its
**	  template copies 0x778590-0x779120, DynamicVectorClass instances called by 0x770860 / 0x7710D0) and
**	  0x77B760 / 0x77B7D0 (the random and ease helpers before avatar.cpp 0x77B870). Own code after a run
**	  of template copies starts a new object: Get_Mime_Type 0x7791F0 (called by image.cpp's slot
**	  0x76E9D0), the class's members 0x779260-0x77A5B0, the static Wstring_Hash 0x77A710 (stored without
**	  an incremental thunk, defined last as configfile.cpp's is), then the wlib templates 0x77A780-0x77B690
**	  (Dictionary<Wstring, ArrayList<Wstring> > and ArrayList<Wstring>, from libs\wwonline\wlib).
**	  No __FILE__ or assert names the file: unknown_htmlattrib.cpp is a neutral name (D137 placement).
**	* Its literals ('image/gif', 'text/html', '.gif') follow htmlview.cpp's in .rdata, before avatar.cpp's.
**	* Every function exact, the 16 template copies with them (0x77AA10, 0x77B030, 0x77B690: D175's
**	  decided losses, paired again here).
**	* Forms: Get / Pop read an entry with a bare getPointer (wlib's E&B arraylist.h NULLs it on a miss: a
**	  select, not a NULL initialiser); Pop's two removals are written out in each branch with the index local to the loop
**	  (one shared `i` moves `value` from edi to ebx); Remove returns bool (setne after Dictionary::remove).
*/
