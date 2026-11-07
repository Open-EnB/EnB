// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\client_lego_root.h (name assumed from
**	client_lego_root.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\client_lego_root.cpp')
**
**	The lego root of a client object (co.cpp's +0xB4, made by 0x731FA0): a ClientLegoImplClass
**	(0xB8 bytes, vtables 0xB030A0 / 0xB03094) that also hangs the object's 'Legos' aux data
**	attachments on itself, one lego per attachment, and tells the object its radius.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CLIENT_LEGO_ROOT_H  // NAME 3 [client_lego_root.h]: header name assumed from the client's client_lego_root.cpp
#define __CLIENT_LEGO_ROOT_H

#include "client_lego_impl.h"

class ClientObjectClass;
class DeltaLegoClass;

class ClientLegoRootClass : public ClientLegoImplClass  // NAME 3 [ClientLegoRootClass]: by convention (D24) from the client's client_lego_root.cpp
{
public:
	ClientLegoRootClass(ClientObjectClass &client_object);			// 0x732030: the client's assert 'client_object.AuxData'
	virtual ~ClientLegoRootClass(void);								// 0x7324F0 (scalar deleting 0x7324C0)

	virtual void Unknown_1C(void);									// +0x1C 0x732650: the aux data's changes first
	virtual void Notify_Parent(void);								// +0x48 0x732750: the object gets the radius (its slot +0x34)

	void Update(void);												// 0x732790: the client's asserts '!AuxData', '!AuxData->Is_Valid()'  // NAME 2: what it does
	void Update_Attachment(unsigned int aux_data_index);			// 0x732880: the client's assert 'aux_data_index >= ...'  // NAME 2: what it does

	ClientObjectClass *Object;										// +0xA0: the constructor's client_object  // NAME 2: what it holds
	DeltaLegoClass *AuxData;										// +0xA4: the object's 'Legos' (the client's asserts)
	std::vector<LegoIdentifierClass> Identifiers;					// +0xA8: one per attachment  // NAME 2: what it holds
};

#endif
