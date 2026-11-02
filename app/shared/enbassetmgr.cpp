// Earth & Beyond client (reconstruction): app\Shared\enbassetmgr.cpp (__FILE__ in the assert at 0xB721AC). Notes at the end of the file.
#include "assetmgr.h"

// E&B's asset manager, 0x408C bytes (ENBAssetManager in the client's strings, table 0xAEA5C8; the tree's files call it
// UnknownBDADC0Class until the rename P-MC-3). Only the constructor and the destructor are written: the table's own
// slots (+0x10 0x48F2C0, +0x14 0x48B0F0 Create_Render_Obj, +0x38 0x48F1A0, +0x78 0x48B220) are not, so this table
// keeps WW3DAssetManager's there and has no +0x78 yet.
class UnknownBDADC0Class : public WW3DAssetManager	// NAME 1: neutral (D10)
{
public:
	UnknownBDADC0Class();
	virtual ~UnknownBDADC0Class();
};

// @0x48B070
UnknownBDADC0Class::UnknownBDADC0Class()
{
}

// @0x48B0D0
UnknownBDADC0Class::~UnknownBDADC0Class()
{
}

/*
**	Notes
**
**	0x48B070 is the constructor WinMain (0x526DE0) calls after new (0x408C bytes). The destructor 0x48B0D0 is declared (it stores
**	the table before the base's; the compiler's would only jump); the deleting destructor 0x48B0A0 is the compiler's.
*/
