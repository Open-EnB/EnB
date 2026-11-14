// Earth & Beyond client (reconstruction): app\Shared\segline_create.h (no string names it; beside
// segline_create.cpp, whose __FILE__ the client has). The holder of a segmented line that co.cpp's Unknown_6F51D0
// and galaxy_map_robj.cpp's Unknown_7B3F00 fill from an INI section.
#ifndef SEGLINE_CREATE_H
#define SEGLINE_CREATE_H

#include "ref_ptr.h"
#include "segline.h"

class INIClass;
class Vector3;

// 4 bytes: a RefCountPtr<SegmentedLineClass> (0x4983A0's FAIL_IF '!Line'); co.cpp's FAIL_IF '!segline' tests it
class Unknown4981E0Class	// NAME 1: neutral (D10)
{
public:
	Unknown4981E0Class(bool create);										// 0x4981E0  // NAME 1 [create]: neutral
	Unknown4981E0Class(const RefCountPtr<SegmentedLineClass> &line);		// 0x4982F0
	~Unknown4981E0Class();													// 0x498320
	void Unknown_498350(const RefCountPtr<SegmentedLineClass> &line);		// 0x498350 (no caller)  // NAME 1 [Unknown_498350]: neutral (D10)
	void Unknown_4983A0(INIClass *ini, const char *section, float scale, const Vector3 &hsv_shift);	// 0x4983A0  // NAME 1 [Unknown_4983A0, hsv_shift]: EA's Recolor argument
	void Unknown_4983A0(INIClass *ini, const char *section, float scale, float *hsv) { Unknown_4983A0(ini, section, scale, *(Vector3 *)hsv); }	// co.cpp's form

	bool operator!() const { return(!Line); }
	RefCountPtr<SegmentedLineClass> Line;									// +0x00  // NAME 3 [Line]: the FAIL_IF '!Line'
};

#endif
