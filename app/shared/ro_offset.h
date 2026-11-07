// Earth & Beyond client (reconstruction): app\shared\ro_offset.h (the client's __FILE__ 0xB71AA0; the asserts sit
// on its lines 49, 50 and 54). Notes at the end.
#ifndef __RO_OFFSET_H
#define __RO_OFFSET_H

#include "always.h"
#include "ro_proxy.h"
#include "vector3.h"
#include "g_assert.h"

// A proxy render object that keeps its proxied object at a fixed offset from this one's own transform.
// Table 0xAE9B48; 0xD0 bytes.
class OffsetRenderObjClass : public ProxyRObj	// the client's Get_Name, 0xB71AD0
{
public:
	OffsetRenderObjClass(void);								// 0x47F560
	virtual ~OffsetRenderObjClass(void);					// 0x47F5E0

	virtual void Set_Transform(const Matrix3D &m);			// 0x47F610
	virtual void Set_Position(const Vector3 &v);			// 0x47F750
	virtual void Render(RenderInfoClass &rinfo);			// 0x47F7E0

	void Set_Offset(const Vector3 &offset);					// 0x47F860  // NAME 2: the offset it stores, and the class's name

	Vector3 Offset;											// +0xC4  // NAME 2: the class's name; Set_Transform and Set_Position add it

	// 0x47FA00, 0x47FA20
	virtual int Class_ID(void) const { return(0x10003); }
	virtual const char *Get_Name(void) const { return("OffsetRenderObjClass"); }

	// The client's asserts fix the layout from here on: Add_Sub_Object and Remove_Sub_Object on lines 49 and 50,
	// Clone on line 54. What the original had in between is unknown; these lines only keep those numbers.















	// 0x47F960, 0x47F9B0: on the client's lines 49 and 50
	virtual int Add_Sub_Object(RenderObjClass *subobj) { G_ASSERT(Proxied); return(Proxied->Add_Sub_Object(subobj)); }
	virtual int Remove_Sub_Object(RenderObjClass *robj) { G_ASSERT(Proxied); return(Proxied->Remove_Sub_Object(robj)); }

	//
	// 0x47FA40: on the client's line 54
	virtual RenderObjClass *Clone(void) const { G_ASSERT(false); return(NULL); }
};

#endif //__RO_OFFSET_H
