/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\co_data.h (name assumed from co_data.cpp)
**
**	Data objects attached to a client object (co.cpp's ClientObjectClass): the base class
**	(vtable 0xB056CC, 8 bytes: the object at +4, the client's FAIL_IF name 'ClientObject') and the
**	access to the objects of each data type through data_binder.h's registry, keyed by the client
**	object. The client binds five types: radar, gutter, starmap, navigation and emote data; the
**	object's destructor (co.cpp 0x6F2FE0) deletes all five through Remove_All 0x79D390.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CO_DATA_H  // NAME 3 [co_data.h]: header name assumed from the client's co_data.cpp
#define __CO_DATA_H

#include "..\shared/data_binder.h"

class ClientObjectClass;
class ClientClass;

// The registry's key: the client object's address
__STL_BEGIN_NAMESPACE
__STL_TEMPLATE_NULL struct hash<ClientObjectClass *> {
	size_t operator()(ClientObjectClass *co) const { return((size_t)co); }
};
__STL_END_NAMESPACE

class ClientObjectDataClass  // NAME 3: by convention (D24) from the client's co_data.cpp
{
public:
	ClientObjectDataClass(ClientObjectClass *co);			// 0x79D0F0 (the assert names 'co')
	virtual ClientObjectClass *Get_Client_Object() { return(ClientObject); }	// +0x00 0x79D130  // NAME 2: what it does
	virtual ~ClientObjectDataClass() {}						// +0x04 0x79D150 (0x79D180)

	ClientClass *Get_Client();								// 0x79D1A0: the object's client (+0x7C)  // NAME 2: what it does
	int Unknown_18();										// 0x79D1E0: the client's slot +0x18, else 0  // NAME 1: the client's slot

	static void Remove_All(ClientObjectClass *co);			// 0x79D390: deletes the five types' data of co  // NAME 2: what it does

protected:
	ClientObjectClass *ClientObject;						// +0x04 (the FAIL_IF's name)
};

// The data of type T bound to a client object, made on request (T's constructor takes the object)
template<class T>
class ClientObjectDataAccessClass  // NAME 1: neutral (D10), the per-type access
{
public:
	// NOT EXACT: inline budget (the client calls the type map's constructor 0x79E6E0 and the value map's pair out of line)
	static T *Get(ClientObjectClass *co, bool create)  // 0x79E300 (radar) ...  // NAME 2: what it does
	{
		DataBinderClass<ClientObjectClass *> &binder = DataBinderClass<ClientObjectClass *>::Instance();
		if (binder.Has(co, DataBinderType<T>())) {
			return(binder.Get(co, DataBinderType<T>()));
		}
		if (!create) {
			return(0);
		}
		T *data = new T(co);
		binder.Bind(co, data);
		return(data);
	}

	static bool Remove(ClientObjectClass *co)  // inline in 0x79D700 (radar) ...  // NAME 2: what it does
	{
		DataBinderClass<ClientObjectClass *> &binder = DataBinderClass<ClientObjectClass *>::Instance();
		if (binder.Has(co, DataBinderType<T>())) {
			T *data = binder.Get(co, DataBinderType<T>());
			binder.Remove(co, DataBinderType<T>());
			delete data;
			return(true);
		}
		return(false);
	}
};

class RadarDataClass;
class GutterDataClass;
class StarmapDataClass;
class NavigationDataClass;
class EmoteDataClass;

RadarDataClass *Get_Radar_Data(ClientObjectClass *co, bool create);				// 0x79D6D0  // NAME 2: what it does
bool Remove_Radar_Data(ClientObjectClass *co);										// 0x79D700  // NAME 2: what it does
GutterDataClass *Get_Gutter_Data(ClientObjectClass *co, bool create);				// 0x79D840  // NAME 2: what it does
bool Remove_Gutter_Data(ClientObjectClass *co);									// 0x79D870  // NAME 2: what it does
StarmapDataClass *Get_Starmap_Data(ClientObjectClass *co, bool create);			// 0x79D9B0  // NAME 2: what it does
bool Remove_Starmap_Data(ClientObjectClass *co);									// 0x79D9E0  // NAME 2: what it does
NavigationDataClass *Get_Navigation_Data(ClientObjectClass *co, bool create);		// 0x79DB20  // NAME 2: what it does
bool Remove_Navigation_Data(ClientObjectClass *co);								// 0x79DB50  // NAME 2: what it does
EmoteDataClass *Get_Emote_Data(ClientObjectClass *co, bool create);				// 0x79DC90  // NAME 2: what it does
bool Remove_Emote_Data(ClientObjectClass *co);										// 0x79DCC0  // NAME 2: what it does

#endif //__CO_DATA_H
