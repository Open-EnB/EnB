// Earth & Beyond client (reconstruction)
// udpreplay: what E&B's INFMSG in app\wwudp\cordcon.cpp needs from wlib's wdebug.cpp (the program doesn't
// link wlib; CritSec comes from conn_replay.cpp's critsec.cpp). The info stream stays NULL, as in the
// client, where nothing calls MsgManager's set*Stream (0x8B5560-0x8B5A80): INFMSG prints nothing.
#include "..\..\..\libs\wwonline\wlib\wdebug.h"

CritSec DebugLibSemaphore;

ostream *MsgManager::infoStream(void)
{
	return(NULL);
}
