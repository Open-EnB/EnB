// clienttest: a stand-in Direct3D device behind DX8Wrapper (ww3d2's dx8wrapper.obj is linked, so its members can't
// be replaced). sclient.cpp's 0x7511A0 (CLIENT_TYPE 60 through the main view's 0x514010) ends with
// DX8Wrapper::Flush_DX8_Resource_Manager(0), a DX8CALL through D3DDevice, which is NULL without a device. The
// device's table logs ResourceManagerDiscardBytes (slot 5); any other slot logs and exits 4, so a new device call
// shows at once. DX8_Assert's WWASSERT on the D3D8 object is empty in this build (no WWDEBUG).
#include "dx8wrapper.h"
#include <stdlib.h>
#include "clienttest.h"

static HRESULT __stdcall Fake_D3D_Discard_Bytes(void *, DWORD bytes) { Trace("d3d ResourceManagerDiscardBytes %u", (unsigned)bytes); return(D3D_OK); }
static HRESULT __stdcall Fake_D3D_Other(void *) { Trace("d3d: a device call clienttest does not expect"); exit(4); return(D3D_OK); }

static void *Fake_D3D_Table[128];
static struct FakeD3DDeviceStruct { void **Table; } Fake_D3D_Device = { Fake_D3D_Table };

// D3DDevice is protected: a derived class reaches it (a redefined 'protected' would change the member's mangled name)
class FakeDX8AccessClass : public DX8Wrapper
{
public:
	static void Set_Device(IDirect3DDevice8 *device) { D3DDevice = device; }
};

// clienttest.cpp's main calls it once the SClient and the asset manager are made
void Install_Fake_D3D_Device()
{
	for (int i = 0; i < sizeof(Fake_D3D_Table) / sizeof(Fake_D3D_Table[0]); i++) {
		Fake_D3D_Table[i] = (void *)Fake_D3D_Other;
	}
	Fake_D3D_Table[5] = (void *)Fake_D3D_Discard_Bytes;	// IDirect3DDevice8::ResourceManagerDiscardBytes
	FakeDX8AccessClass::Set_Device((IDirect3DDevice8 *)&Fake_D3D_Device);
}
