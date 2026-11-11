// Earth & Beyond client (reconstruction): app\client\ui_experience.h, the experience bars (the client's ui_experience.cpp
// per its asserts; this header's name is provisional, NAME 3). Laid out from the constructor 0x58C450, the destructor
// 0x58C6E0 and the table 0xAF16BC; the PDA (pdaint.cpp's set-up 0x693230) and the character info window's attributes
// panel (ui_attrb.cpp's 0x546DD0) make it.
#ifndef UI_EXPERIENCE_H  // NAME 3 [ui_experience.h]: provisional header name for ui_experience.cpp
#define UI_EXPERIENCE_H

#include "ui_cpit.h"		// UnknownObjectWatchClass (interim placement, D137)

class RenderObjClass;
class InterfaceClass;
class ClientObjectClass;
class UIExperienceBarClass;

// The combat, trade and explore bars of a mesh (0x28 bytes, table 0xAF16BC): a watcher of the avatar whose slot +0x10
// fills the bars from its RPGInfo every frame
class UIExperienceClass : public UnknownObjectWatchClass	// NAME 3: class-from-file (D24) for app\client\ui_experience.cpp
{
public:
	UIExperienceClass(RenderObjClass *mesh, InterfaceClass *parent);	// 0x58C450 ('combat_bar', 'trade_bar', 'explore_bar'; tooltips UI_EXPERIENCE)  // NAME 1 [mesh, parent]: what its callers pass
	virtual ~UIExperienceClass();						// +0x00 0x58C6E0 (deleting 0x58C6B0)
	virtual void Unknown_08(ClientObjectClass *object);	// +0x08 0x58CA10 (the bars emptied, then the base's)  // NAME 1 [object]: the base's
	virtual void Unknown_Watch_0C();					// +0x0C 0x58C8E0 (the bars emptied)
	virtual void Unknown_10();							// +0x10 0x58CB50 (RPGInfo.CombatExp / TradeExp / ExploreExp, every frame)
	void Unknown_58C7B0();								// 0x58C7B0 (the bars emptied: slots +0x08 / +0x0C inline it)
	void Unknown_58CF60();								// 0x58CF60 (the bars out of the parent's +0x64, before its delete)

	ClientObjectClass *Get_Object() { return(Object); }	// (inline: 0x693CA0 loads +0x04 before the client's slot +0x28 call)  // NAME 2: what it returns

	InterfaceClass *Parent;								// +0x18  // NAME 1: what its callers pass
	UIExperienceBarClass *CombatBar;					// +0x1C ('combat_bar')  // NAME 2: the mesh object's name
	UIExperienceBarClass *TradeBar;						// +0x20 ('trade_bar')  // NAME 2: the mesh object's name
	UIExperienceBarClass *ExploreBar;					// +0x24 ('explore_bar')  // NAME 2: the mesh object's name
};

#endif
