// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\accel.h (the header's name by convention, D24, from accel.cpp).
*/
#ifndef __ACCEL_H	// NAME 3 [accel.h]: by convention from accel.cpp (D24)
#define __ACCEL_H

// A value that moves towards a target by Accel per unit of time
class AccelClass	// NAME 3: the pair's type name "AccelClass" (kv_encoder.cpp's string)
{
public:
	AccelClass();														// 0x498C10
	void Set_Target(float target, float accel);					// 0x498C30  // NAME 2: what it does
	void Set_Target(float target);									// 0x498CC0  // NAME 2: what it does
	void Set_Value(float value, bool stop);						// 0x498D40  // NAME 2: what it does
	void Adjust_Target(float delta, float accel, float max);	// 0x498D80  // NAME 2: what it does
	float Update(float time);											// 0x498E00  // NAME 2: what it does
	float Get_Value(float time) { return !IsActive ? Current : Update(time); }	// NAME 2: what it does
	float Get_Current() const { return Current; }	// NAME 2: what it does
	bool Update_Changed(float time);									// 0x498E90  // NAME 2: what it does
	void Reset();															// 0x498EE0  // NAME 2: what it does
	void Set(float current, float target, float accel);			// 0x498F00  // NAME 2: what it does
	bool operator==(const AccelClass &that) const;				// 0x498F80
	AccelClass &operator=(const AccelClass &that);				// 0x498FD0
	bool operator!=(const AccelClass &that) const;				// 0x499000

	bool IsActive;															// +0x00 (accel.cpp's assert)
	float Target;															// +0x04  // NAME 3: kv_encoder.cpp's key "Target"
	float Accel;																// +0x08  // NAME 3: kv_encoder.cpp's key "Accel"
	float Current;															// +0x0C  // NAME 3: kv_encoder.cpp's key "Current"
};

char *Value_To_String(char *buf, int size, const AccelClass &value);	// 0x499070  // NAME 2: as strconv_lib's
bool String_To_Value(const char *text, AccelClass *value);				// 0x4990C0  // NAME 2: as strconv_lib's

#endif //__ACCEL_H
