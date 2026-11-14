// Earth & Beyond client (reconstruction): app\Common\chat_defs.cpp (__FILE__ 0xBA9030). Notes at the end of the file.
#include <string.h>
#include "chat_defs.h"
#include "..\shared\unknown_4a1590.h"
#include "..\shared\g_assert.h"

struct ChatEventString {	// NAME 2: the client's FAIL_IF text 'str.Event != event' (its members); the type's name is assumed
	ChatEventString(ChatEventType event, const char *name) : Event(event), Name(name) {}
	ChatEventType Event;
	const char *Name;
};
#define CHAT_EVENT_STRING(e)	ChatEventString(e, #e)

static ChatEventString _ChatEventStrings[] = {	// 0xBF3090, built by the dynamic initialiser 0x830460  // NAME 1: what it holds
	CHAT_EVENT_STRING(CHAT_EVENT_INVALID),
	CHAT_EVENT_STRING(CHAT_EVENT_LOGIN),
	CHAT_EVENT_STRING(CHAT_EVENT_LOGOUT),
	CHAT_EVENT_STRING(CHAT_EVENT_SAY_CHANNEL),
	CHAT_EVENT_STRING(CHAT_EVENT_SAY_PERSON),
	CHAT_EVENT_STRING(CHAT_EVENT_SAY_LOCAL),
	CHAT_EVENT_STRING(CHAT_EVENT_SAY_ALL),
	CHAT_EVENT_STRING(CHAT_EVENT_ENTER_CHANNEL),
	CHAT_EVENT_STRING(CHAT_EVENT_EXIT_CHANNEL),
	CHAT_EVENT_STRING(CHAT_EVENT_INSERT_CHANNEL),
	CHAT_EVENT_STRING(CHAT_EVENT_REMOVE_CHANNEL),
	CHAT_EVENT_STRING(CHAT_EVENT_REMOVE_CHANNEL_BY_PERSON),
	CHAT_EVENT_STRING(CHAT_EVENT_INVITE),
	CHAT_EVENT_STRING(CHAT_EVENT_UNINVITE),
	CHAT_EVENT_STRING(CHAT_EVENT_GAG),
	CHAT_EVENT_STRING(CHAT_EVENT_UNGAG),
	CHAT_EVENT_STRING(CHAT_EVENT_BAN),
	CHAT_EVENT_STRING(CHAT_EVENT_UNBAN),
	CHAT_EVENT_STRING(CHAT_EVENT_KICK),
	CHAT_EVENT_STRING(CHAT_EVENT_IGNORE),
	CHAT_EVENT_STRING(CHAT_EVENT_UNIGNORE),
	CHAT_EVENT_STRING(CHAT_EVENT_FRIEND),
	CHAT_EVENT_STRING(CHAT_EVENT_UNFRIEND),
	CHAT_EVENT_STRING(CHAT_EVENT_INSERT_OWNER),
	CHAT_EVENT_STRING(CHAT_EVENT_REMOVE_OWNER),
	CHAT_EVENT_STRING(CHAT_EVENT_SET_RESTRICTION_ON),
	CHAT_EVENT_STRING(CHAT_EVENT_SET_RESTRICTION_OFF),
	CHAT_EVENT_STRING(CHAT_EVENT_GM_GAG),
	CHAT_EVENT_STRING(CHAT_EVENT_GM_UNGAG),
	CHAT_EVENT_STRING(CHAT_EVENT_SET_TITLE_HIDDEN_ON),
	CHAT_EVENT_STRING(CHAT_EVENT_SET_TITLE_HIDDEN_OFF),
};





























// 0x830770
char *Value_To_String(char *buf, int size, ChatEventType event)
{
	const size_t n = sizeof(_ChatEventStrings) / sizeof(_ChatEventStrings[0]);
	FAIL_IF((size_t)event >= n) { Bounded_Strcpy(buf, size, "CHAT_EVENT_ERROR1"); return(buf); }
	const ChatEventString &str = _ChatEventStrings[event];

	FAIL_IF(str.Event != event) { Bounded_Strcpy(buf, size, "CHAT_EVENT_ERROR2"); return(buf); }
	Bounded_Strcpy(buf, size, str.Name);
	return(buf);
}

// 0x830830
bool String_To_Value(const char *buf, ChatEventType *event)
{
	const size_t n = sizeof(_ChatEventStrings) / sizeof(_ChatEventStrings[0]);
	for (size_t i = 0; i < n; i++) {
		const ChatEventString &str = _ChatEventStrings[i];
		if (strcmp(buf, str.Name) == 0) {
			*event = str.Event;
			return(true);
		}
	}
	return(false);
}

struct ChatErrorString {	// NAME 2: the client's FAIL_IF text 'str.Error != error' (its members); the type's name is assumed
	ChatErrorString(ChatErrorType error, const char *name) : Error(error), Name(name) {}
	ChatErrorType Error;
	const char *Name;
};

static ChatErrorString _ChatErrorStrings[] = {	// 0xBF3188, built by the dynamic initialiser 0x8308C0  // NAME 1: what it holds
	ChatErrorString((ChatErrorType)0, "ok"),
	ChatErrorString((ChatErrorType)1, "unknown_error"),
	ChatErrorString((ChatErrorType)2, "invalid channel"),
	ChatErrorString((ChatErrorType)3, "invalid parameter"),
	ChatErrorString((ChatErrorType)4, "invalid person"),
	ChatErrorString((ChatErrorType)5, "duplicate name"),
	ChatErrorString((ChatErrorType)6, "not a member"),
	ChatErrorString((ChatErrorType)7, "already"),
	ChatErrorString((ChatErrorType)8, "is ignored"),
	ChatErrorString((ChatErrorType)9, "is gagged"),
	ChatErrorString((ChatErrorType)10, "is banned"),
	ChatErrorString((ChatErrorType)11, "is system channel"),
	ChatErrorString((ChatErrorType)12, "need password"),
	ChatErrorString((ChatErrorType)13, "wrong password"),
	ChatErrorString((ChatErrorType)14, "no permission"),
	ChatErrorString((ChatErrorType)15, "bad name"),
	ChatErrorString((ChatErrorType)16, "no memory"),
	ChatErrorString((ChatErrorType)17, "yourself"),
	ChatErrorString((ChatErrorType)18, "reached max limit"),
};











// 0x830AA0
char *Value_To_String(char *buf, int size, ChatErrorType error)
{
	const size_t n = sizeof(_ChatErrorStrings) / sizeof(_ChatErrorStrings[0]);
	FAIL_IF((size_t)error >= n) { Bounded_Strcpy(buf, size, "bad chat error"); return(buf); }
	const ChatErrorString &str = _ChatErrorStrings[error];

	FAIL_IF(str.Error != error) { Bounded_Strcpy(buf, size, "bad error table"); return(buf); }
	Bounded_Strcpy(buf, size, str.Name);
	return(buf);
}

// 0x830B60
bool String_To_Value(const char *buf, ChatErrorType *error)
{
	const size_t n = sizeof(_ChatErrorStrings) / sizeof(_ChatErrorStrings[0]);
	for (size_t i = 0; i < n; i++) {
		const ChatErrorString &str = _ChatErrorStrings[i];
		if (strcmp(buf, str.Name) == 0) {
			*error = str.Error;
			return(true);
		}
	}
	return(false);
}

/*
**	Reconstruction notes
**
**	The object: 0x830460-0x830BC3, after the guarded <string> initialiser 0x830430 of the file before it
**	(this one has none; circbuff.cpp's CircBuffClass constructor 0x830BF0 follows). Both tables are built
**	at start-up (their entries in the initialiser list 0xB6CF2C / 0xB6CF30 sit together between two
**	zero separators), so their structs have constructors: the tables live in .bss (0xBF3090, 0xBF3188),
**	the names in .data with the FAIL_IF texts and the __FILE__ string 0xBA9030. The event names are the
**	enumerators stringised (CHAT_EVENT_STRING); the error codes have no names in the client. The
**	FAIL_IFs pin lines 80 / 83 and 144 / 147. String_To_Value 0x830830 is exact; the client's function
**	list runs it into the initialiser after it (0x8308C0 has no start of its own there).
*/
