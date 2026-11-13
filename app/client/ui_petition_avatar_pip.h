// Earth & Beyond client (reconstruction): app\client\ui_petition_avatar_pip.h, the petition windows' base with the
// GM avatar's picture-in-picture (an avatar 'Megan' over the model's 'Avatar' viewport). The client's asserts name the
// file (e:\build\beta\G\app\Client\ui_petition_avatar_pip.cpp); this header's name follows it (NAME 1, provisional).
#ifndef UI_PETITION_AVATAR_PIP_H  // NAME 1 [ui_petition_avatar_pip.h]: provisional header name for ui_petition_avatar_pip.cpp
#define UI_PETITION_AVATAR_PIP_H

#include "ui_chat_panel_base.h"

class AvatarPipClass;
class CharacterViewAvatarClass;

// The window (0xA8 bytes, tables 0xAF4F74 / 0xAF4F68 / 0xAF4F5C; no slots of its own): ChatPanelUIBase with an
// avatar drawn in a picture-in-picture over the model's 'Avatar' viewport. ui_petition.cpp's window (constructor
// 0x5D33B0) and ui_petition_player.cpp's (0x5D5740) derive from it.
class UIPetitionAvatarPipClass : public ChatPanelUIBase	// NAME 3: class-from-file (D24) for ui_petition_avatar_pip.cpp
{
public:
	UIPetitionAvatarPipClass(SClient *client);			// 0x5D4150 (deleting 0x5D41A0)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIPetitionAvatarPipClass();				// 0x5D41D0
	virtual void Unknown_54();							// +0x54 0x5D4310 (FAIL_IF '!Interface_Mesh()', G_ASSERT 'avatar_vp': the avatar 'Megan' and its PIP over 'Avatar')
	virtual void Unknown_58();							// +0x58 0x5D45D0 (the base's, then the PIP shows the avatar)
	virtual void Unknown_5C();							// +0x5C 0x5D42E0 (InterfaceClass's, then the PIP's model off)
	virtual void Unknown_60();							// +0x60 0x5D4660 (the PIP shows the avatar)

	RefCountPtr<CharacterViewAvatarClass> Avatar;		// +0xA0 (the avatar 'Megan' Unknown_54 makes)  // NAME 2: what it holds
	AvatarPipClass *Pip;								// +0xA4 (its picture-in-picture over the model's 'Avatar')  // NAME 2: what it holds
};

#endif // UI_PETITION_AVATAR_PIP_H
