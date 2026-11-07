// Earth & Beyond client (reconstruction): unknown_75b7b0.h, PROVISIONAL header name (D137) beside unknown_75b7b0.cpp
#ifndef UNKNOWN_75B7B0_H
#define UNKNOWN_75B7B0_H

class RenderObjClass;
class Vector3;

void Unknown_75B7B0(RenderObjClass *obj, float phase);	// 0x75B7B0 (OrbitalManager 0x4F45A0)  // NAME 1 [obj, phase]: neutral (D10); what 0x75BB50 does with it
void Unknown_75B8F0(RenderObjClass *obj, const Vector3 &sun, const Vector3 &camera);	// 0x75B8F0 (OrbitalManager 0x4F45A0, enbscene.cpp 0x75F690)  // NAME 1 [obj, sun, camera]: neutral (D10); what 0x75F690 passes
void Unknown_75BB50(RenderObjClass *obj, float phase);	// 0x75BB50  // NAME 1 [obj, phase]: neutral (D10); the 'plt_dn' textures' U offset

#endif
