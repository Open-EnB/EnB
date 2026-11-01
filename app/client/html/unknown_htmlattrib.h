// Earth & Beyond client (reconstruction): app\client\html\unknown_htmlattrib.h. Notes in unknown_htmlattrib.cpp.
#ifndef UNKNOWN_HTMLATTRIB_H
#define UNKNOWN_HTMLATTRIB_H

#include "..\..\..\libs\wwonline\wlib\wstypes.h"
#include "..\..\..\libs\wwonline\wlib\wstring.h"
#include "..\..\..\libs\wwonline\wlib\arraylist.h"
#include "..\..\..\libs\wwonline\wlib\dictionary.h"

// A Dictionary of value stacks: each key keeps an ArrayList<Wstring> whose last entry is its current
// value. 0x38 bytes (the Dictionary alone); htmlview's view keeps two (+0x28, +0x60).
class HtmlAttributeStackClass  // NAME 2: htmlview's tag handlers set / push / pop per key ('Font.Bold', 'p.%s', 'Anchor.HREF')
{
public:
	HtmlAttributeStackClass(void);  // 0x779260
	~HtmlAttributeStackClass(void);  // 0x779320

	void Add_All(HtmlAttributeStackClass &other);  // 0x779520  // NAME 2: every key of other, added here (no result, not an operator=)
	void Set(char *key, char *value, int length = -1);  // 0x779860  // NAME 2: the key's stack becomes value alone
	char *Get(char *key);  // 0x779B70  // NAME 2: the top of the key's stack, or NULL
	void Push(char *key, char *value, int length = -1);  // 0x779CA0  // NAME 2: value added on top of the key's stack
	bit8 Pop(char *key, char *value = NULL, int length = -1);  // 0x779F90  // NAME 2: removes the top (or the topmost equal to value)
	bool Remove(char *key);  // 0x77A300  // NAME 2: the key and its whole stack
	void Clear(void);  // 0x77A5B0  // NAME 2: Dictionary::clear

private:
	Dictionary<Wstring, ArrayList<Wstring> > Values;  // +0x00  // NAME 2: the stacks by key
};

char *Get_Mime_Type(char *filename);  // 0x7791F0  // NAME 2: 'image/gif' for a .gif, else 'text/html'

#endif
