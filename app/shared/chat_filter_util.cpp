// Earth & Beyond client (reconstruction): app\shared\chat_filter_util.cpp. Notes at the end of the file.
#include "chat_filter_util.h"
#include "chat_filter.h"
#include "g_assert.h"

// 0x4E42C0
ChatFilterClass *Create_Chat_Filter()
{
	ChatWordFilterClass *filter = new ChatWordFilterClass;
	FAIL_IF(!filter) return(NULL);
	filter->Add_Word("fuck");
	filter->Add_Word("shit");
	filter->Add_Word("cunt");
	filter->Add_Word("asshole");
	filter->Add_Word("cocksucker");
	filter->Add_Word("motherfucker");
	filter->Add_Word("bitch");
	filter->Add_Word("fag");
	filter->Add_Word("gay");
	filter->Add_Word("nigger");
	filter->Add_Word("piss");
	filter->Add_Word("prick");
	filter->Add_Word("pussy");
	filter->Add_Word("twat");
	return(filter);
}

/*
**	Reconstruction notes (chat_filter_util.cpp)
**
**	* Client 0x4E42C0-0x4E4AF0 (<string>'s guarded ctype initialiser at 0x4E4290 ends the object
**	  before, 0x4E4AD0 ends this one): the file from 0x4E42C0's '!filter' FAIL_IF (line 10;
**	  e:\build\beta\G\app\shared\chat_filter_util.cpp). The object also holds ChatWordFilterClass's
**	  implicit destructor (0x4E4A10, 0x4E49E0) and vector<string>'s allocator constructor (0x4E4AA0).
**	* Nothing in the client calls 0x4E42C0.
*/
