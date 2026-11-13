// Earth & Beyond client (reconstruction): app\Client\distance_calc_client.cpp (__FILE__ 0xBA137C). Notes at the end.
#include "distance_calc_client.h"
#include "co.h"
#include "..\common\distance_calc.h"
#include "..\shared\g_assert.h"
// 0x7E6060
float Unknown_7E6060(ClientObjectClass *obj_a, ClientObjectClass *obj_b)
{
	FAIL_IF(!obj_a) return(0.0f);
	FAIL_IF(!obj_b) return(0.0f);
	return(Unknown_831E90(obj_a->Unknown_24(), obj_a->Unknown_30(), obj_b->Unknown_24(), obj_b->Unknown_30()));
}




// 0x7E6130
float Unknown_7E6130(ClientObjectClass *obj_a, ClientObjectClass *obj_b)
{
	FAIL_IF(!obj_a) return(0.0f);
	FAIL_IF(!obj_b) return(0.0f);
	return(Unknown_831F10(obj_a->Unknown_24(), obj_a->Unknown_30(), obj_b->Unknown_24(), obj_b->Unknown_30()));
}




// 0x7E6200
char Unknown_7E6200(ClientObjectClass *obj_a, ClientObjectClass *obj_b, float range)
{
	FAIL_IF(!obj_a) return(0x54);
	FAIL_IF(!obj_b) return(0x60);
	return((char)Unknown_831F90(obj_a->Unknown_24(), obj_a->Unknown_30(), obj_b->Unknown_24(), obj_b->Unknown_30(), range));
}

/*
**	Reconstruction notes
**
**	The object runs from 0x7E6060 to its guarded <string> initialiser 0x7E62C0 (emote_data.cpp's constructor run
**	starts at 0x7E62F0). Each function passes two client objects' position (slot +0x24) and radius (slot +0x30) to
**	distance_calc.h's sphere distances, obj_b's first (VC6's right-to-left arguments). The FAIL_IFs pin lines 9-10,
**	20-21 and 31-32. 0x7E6200 returns a byte: its FAIL_IFs return 0x54 and 0x60 (mov al, the client's own
**	constants), and 0x831F90's int result goes out in al unconverted.
*/
