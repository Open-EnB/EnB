// Earth & Beyond client (reconstruction): app\Client\client_lego_root.cpp. Notes at the end of the file.
#include "client_lego_root.h"
#include "co.h"
#include "aux_roots.h"
#include "..\shared\auxdata\auxdata.h"
// The asserts name DeltaClass::Is_Valid(), not declared in delta.h (a declaration there moves aux_roots.cpp's pad window: interim, D137)
#define Is_Valid() Unknown70
// Get_Size() is DeltaCompoundClass's member (delta.h), as the FAIL_IF text 0xB95290 names it
// unknown_7294d0.cpp's (0x7294D0 / 0x729770; that file has no header yet)
struct Unknown729990Struct					// NAME 1: neutral (D10)
{
	Unknown729990Struct(void) : AssetId(-1), Model(NULL) {}
	Unknown729990Struct(int asset_id, const RefCountPtr<RenderObjClass> &model) : AssetId(asset_id), Model(model) {}

	int AssetId;							// +0x00  // NAME 2: what 0x7294D0 passes to Create as the asset id
	RefCountPtr<RenderObjClass> Model;		// +0x04  // NAME 2: what it holds
};
Unknown729990Struct Unknown_729770(ClientClass *client, int base_asset, float scale);	// 0x729770  // NAME 1 [client, base_asset, scale]: what 0x7294D0 passes
RefCountPtr<LegoClass> Unknown_7294D0(const LegoIdentifierClass &id, int kind, int value, ClientClass *client, float scale);	// 0x7294D0  // NAME 1 [id, kind, value, client, scale]: what 0x6FEF70 passes

// 0x731FA0 (cdecl, co.cpp's constructor): the object's lego root
RefCountPtr<ClientLegoRootClass> Unknown_731FA0(ClientObjectClass *object)
{
	return(Create_NEW(new ClientLegoRootClass(*object)));
}

// 0x732690 (co.cpp's destructor): every lego off the root
void ClientLegoRootClass::Unknown_732690()
{
	std::vector<RefCountPtr<LegoClass> > legos;
	ClientLegoImplClass::Detach_All(legos);
}

ClientLegoRootClass::ClientLegoRootClass(ClientObjectClass &client_object) :
	ClientLegoImplClass(LegoIdentifierClass(), 1),
	Object(&client_object),
	AuxData(NULL)
{
	G_ASSERT(client_object.AuxData);
	if (client_object.AuxData) {
		NameClass name("Legos");
		AuxData = Unknown_Find_Field<DeltaLegoClass>((DeltaCompoundClass *)client_object.AuxData, name);
		if (AuxData) {
			unsigned int count = AuxData->Attachments.Get_Size();
			Identifiers.resize(count);
		}
	}
}

ClientLegoRootClass::~ClientLegoRootClass(void)
{
	{
		std::vector<RefCountPtr<LegoClass> > legos;
		Detach_All(legos);
	}
	AuxData = NULL;
}

void ClientLegoRootClass::Unknown_1C(void)
{
	if (AuxData && AuxData->Unknown70 > 1) {
		Update();
	}
	ClientLegoImplClass::Unknown_1C();
}

void ClientLegoRootClass::Notify_Parent(void)
{
	Object->Unknown_34(Get_Radius());
}






void ClientLegoRootClass::Update(void)
{
	FAIL_IF(!AuxData) return;
	FAIL_IF(!AuxData->Is_Valid()) return;
	if (AuxData->Scale.Unknown70 > 1) {
		Set_Scale(AuxData->Scale.Value);
	}
	if (AuxData->Attachments.Unknown70 > 1) {
		unsigned int count = AuxData->Attachments.Get_Size();
		for (unsigned int i = 0; i < count; i++) {
			if (AuxData->Attachments.Children[i]->Unknown70 > 1) {
				Update_Attachment(i);
			}
		}
		Object->Unknown_6FCB70();
	}
}








// NOT EXACT: shape (similarity 0.86, every row equal but one: the client's null test of lego at the end jumps past its release to the epilogue, ours to the release; attempts.py show 0x732880)
void ClientLegoRootClass::Update_Attachment(unsigned int aux_data_index)
{
	FAIL_IF(!AuxData) return;
	FAIL_IF(aux_data_index >= AuxData->Attachments.Get_Size()) return;
	bool create = false;
	bool data_changed = false;
	DeltaAttachmentClass *attachment = (DeltaAttachmentClass *)AuxData->Attachments.Children[aux_data_index];
	LegoIdentifierClass &id = Identifiers[aux_data_index];
	RefCountPtr<LegoClass> lego;



















	if (attachment->BoneName.Unknown70 > 1) {
		LegoIdentifierClass old_id(id);
		if (!old_id.Is_Empty()) {
			Remove(old_id);
		}
		LegoIdentifierClass new_id(std::string(attachment->BoneName.Get()));
		id = new_id;
		create = true;
	} else if (attachment->Type.Unknown70 > 1) {
		if (!id.Is_Empty()) {
			Remove(id);
		}
		create = true;
	} else if (attachment->Assets.Unknown70 > 1) {
		lego = Find(id);
		if (lego) {
			ClientClass *client = Object->Client;
			FAIL_IF(!client) return;
			Unknown729990Struct model = Unknown_729770(client, attachment->Assets.Value, Get_Scale());
			if (!model.Model || model.AssetId < 0) {
				Remove(id);
				return;
			}
			lego->Init(model.AssetId, model.Model);
		} else {
			create = true;
		}
	}
	if (attachment->Data.Unknown70 > 1) {
		data_changed = true;
	}
	if (create) {
		ClientClass *client = Object->Client;










		G_ASSERT(client);
		if (client) {
			lego = Unknown_7294D0(id, attachment->Type.Value, attachment->Assets.Value, client, Get_Scale());
			if (!lego) {
				return;
			}
			Add(lego);
			data_changed = true;
		}
	} else {
		lego = Find(id);
	}
	if (lego) {
		if (data_changed) {
			std::string data(attachment->Data.Get());
			lego->Unknown_3C((int)&data);	// the client passes a std::string (client_lego_impl.h declares the slot with an int: symbols.json's name)
		}
	}
}

/*
** Reconstruction notes (WE8, 2026-11-07)
**
** The object: client 0x731FA0-0x7333B0 (asserts on lines 39-174, D25): 0x731FA0 (the root's maker), the
** constructor 0x732030, the destructor 0x7324F0 (scalar deleting 0x7324C0), Unknown_1C 0x732650, 0x732690,
** Notify_Parent 0x732750, Update 0x732790, Update_Attachment 0x732880, then the instances: RefCountPtr's
** Create_NEW 0x732FB0, the identifier vector's size 0x732FE0, erase 0x733020, uninitialized_copy 0x733090,
** uninitialized_fill_n 0x7330E0, fill 0x733120, copy_backward 0x733160.
** The vtable 0xB030A0 is ClientLegoImplClass's 0xB02F1C but for the destructor, +0x1C and +0x48 (Notify_Parent).
** co.h's +0xB4 member (UnknownB4) is a ClientLegoRootClass: 0x731FA0 returns it and
** 0x732690 is its member.
*/
