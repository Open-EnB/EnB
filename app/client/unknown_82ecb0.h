// Earth & Beyond client (reconstruction): app\client\unknown_82ecb0.h (PROVISIONAL name, D137)
#ifndef UNKNOWN_82ECB0_H	// NAME 1: provisional header name (D10, D137)
#define UNKNOWN_82ECB0_H

class Matrix3D;
class Vector3;
class TextureClass;
class INIClass;
class RenderObjClass;
class IntersectionClass;
class LayerClass;
class IntersectionResultClass;

Matrix3D Unknown_82ECB0(const Vector3 &direction, const Vector3 &position);	// 0x82ECB0  // NAME 1 [all]: neutral (D10)
void Unknown_82EF50();	// 0x82EF50  // NAME 1: neutral (D10)
TextureClass *Unknown_82F000(INIClass &ini, const char *section, float *hsv);	// 0x82F000  // NAME 1 [all]: neutral (D10)
RenderObjClass *Unknown_82F090(IntersectionClass *inter, float screen_x, float screen_y, const LayerClass &layer, RenderObjClass *robj, IntersectionResultClass *final_result);	// 0x82F090  // NAME 1 [all]: neutral (D10)

#endif
