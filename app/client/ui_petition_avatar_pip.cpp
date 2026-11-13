// Earth & Beyond client (reconstruction): app\client\ui_petition_avatar_pip.cpp, the petition windows' base with
// the GM avatar's picture-in-picture. The client's asserts name the file. The asserts sit on the client's lines
// (D25). Notes at the end.
#include <string>
#include "ui_petition_avatar_pip.h"
#include "client.h"
#include "avatar.h"
#include "avatar_pip.h"
#include "local_interface_sort.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\g_assert.h"

// 0x5D4150
UIPetitionAvatarPipClass::UIPetitionAvatarPipClass(SClient *client) :
	ChatPanelUIBase(client),
	Pip(NULL)
{
}

// 0x5D41D0
UIPetitionAvatarPipClass::~UIPetitionAvatarPipClass()
{
	if (Pip) {
		Pip->Unknown_7E4350();
		delete Pip;
		Pip = NULL;
	}
	Avatar = NULL;
}

// 0x5D42E0
void UIPetitionAvatarPipClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
	if (Pip) {
		Pip->Unknown_7E4350();
	}
}




























// 0x5D4310
void UIPetitionAvatarPipClass::Unknown_54()
{
	Unknown_10();
	FAIL_IF(!Interface_Mesh()) return;
	RefCountPtr<RenderObjClass> avatar_vp = Create_Get(Interface_Mesh()->Get_Sub_Object_By_Name("Avatar"));
	if (avatar_vp) {
		AvatarDescClass desc(2, "Megan");
		Avatar = Create_NEW(new CharacterViewAvatarClass(&desc, 1, 0, NULL));
		Pip = new AvatarPipClass;
		Pip->Unknown_7E36B0((SClient *)((ClientClass *)Client)->As_Space_Client(), avatar_vp, false);
	} else {



		G_ASSERT(avatar_vp);
	}
}
// 0x5D45D0
void UIPetitionAvatarPipClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	ChatPanelUIBase::Unknown_58();
	if (Pip) {
		Pip->Unknown_7E3A20(NULL, 0, Avatar.Peek());
	}
}

// 0x5D4660
void UIPetitionAvatarPipClass::Unknown_60()
{
	if (Pip) {
		Pip->Unknown_7E3A20(NULL, 0, Avatar.Peek());
	}
}

/*
**	Notes.
**	Object 0x5D4150-0x5D46DF: the constructor 0x5D4150, the deleting destructor 0x5D41A0, the slots in the client's
**	order, the guarded std::ctype<unsigned short>::id initialiser 0x5D4690 (the object's <string>), then the adjustor
**	thunks 0x5D46C0 / 0x5D46D0 of the second and third tables; ui_petition.cpp's thunk 0x5D4140 precedes it,
**	ui_petition_entry.cpp's 0x5D46E0 follows. The ui_petition_entry.cpp pattern (ChatPanelUIBase); the PIP as
**	ui_chat_panel.cpp's (AvatarPipClass over a viewport sub-object).
*/
