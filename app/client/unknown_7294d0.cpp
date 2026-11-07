// Earth & Beyond client (reconstruction): app\client\unknown_7294d0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "client_lego_impl.h"
#include "co.h"
#include "client.h"
#include "rendobj.h"

// A base asset's model as 0x729770 returns it (8 bytes; constructors 0x729990 / 0x7299B0, destructor 0x729740)
struct Unknown729990Struct					// NAME 1: neutral (D10)
{
	Unknown729990Struct(void) : AssetId(-1), Model(NULL) {}
	Unknown729990Struct(int asset_id, const RefCountPtr<RenderObjClass> &model) : AssetId(asset_id), Model(model) {}

	int AssetId;							// +0x00  // NAME 2: what 0x7294D0 passes to Create as the asset id
	RefCountPtr<RenderObjClass> Model;		// +0x04  // NAME 2: what it holds
};

Unknown729990Struct Unknown_729770(ClientClass *client, int base_asset, float scale);	// NAME 1 [client, base_asset, scale]: what 0x7294D0 passes

// 0x7294D0 (cdecl): a lego for a base asset; kind 3 is kind 2's
RefCountPtr<LegoClass> Unknown_7294D0(const LegoIdentifierClass &id, int kind, int value, ClientClass *client, float scale)
{
	switch (kind) {
		case 0:
			return(NULL);
		case 1:
			{
				Unknown729990Struct model = Unknown_729770(client, value, scale);
				if (model.Model && model.AssetId > 0) {
					return(ClientLegoImplClass::Create(id, 1, model.AssetId, model.Model));
				}
				return(NULL);
			}
		case 2:
			{
				Unknown729990Struct model = Unknown_729770(client, value, scale);
				if (model.Model && model.AssetId > 0) {
					return(ClientLegoImplClass::Create(id, 2, model.AssetId, model.Model));
				}
				return(NULL);
			}
		case 3:
			return(Unknown_7294D0(id, 2, value, client, scale));
	}
	return(NULL);
}

// 0x729770 (cdecl): the base asset's model (ClientObjectClass::Unknown_6F5140, no colour shift), or
// a static {-1, NULL} when there is none
Unknown729990Struct Unknown_729770(ClientClass *client, int base_asset, float scale)
{
	static Unknown729990Struct _null;
	if (base_asset <= 0) {
		return(_null);
	}
	Unknown729990Struct model;
	model.AssetId = base_asset;
	float hsv[3] = {0.0f, 0.0f, 0.0f};
	model.Model = Create_Get(ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv));
	if (model.Model == NULL) {
		return(_null);
	}
	return(model);
}

/*
** Reconstruction notes (WE6, 2026-11-07)
**
** The object: no __FILE__ or assert; client 0x7294D0-0x7299C8: 0x7294D0, the struct's destructor 0x729740,
** 0x729770, its static's atexit destructor 0x729970, the struct's constructors 0x729990 / 0x7299B0. After
** client.cpp's HashListClass instances (Add 0x729320), before client_lego_impl.cpp's bone maps' hash_map
** destructor 0x729B00. Callers: co.cpp's SUBPARTS 0x6FEF70 (kind 1) and 0x732880 (0x7294D0 and 0x729770).
*/
