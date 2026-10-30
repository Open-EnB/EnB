/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: ..\shared/kv_acc_array.h (the asserts' __FILE__, relative as E&B's headers are)
**
**	A string accessor over an array of values (StringValueClass, kv_pair_string.h): the values as
**	one text, separated by Separator, for KVEncoderBaseClass::Add_String. avatar_desc.cpp's
**	body and head weights are its float instance (0x4C3110-0x4C3BE0). The asserts sit on the
**	client's lines 66, 72 and 93 (D25); notes at the end of the file.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_ACC_ARRAY_H  // NAME 5 [kv_acc_array.h]: the asserts' __FILE__
#define __KV_ACC_ARRAY_H

#include "kv_pair_string.h"
#include "unknown_debug.h"
#include "strconv_lib.h"
#include <string>
#include <string.h>

// vtable 0xAEBA6C (float), 0x20 bytes
template<class T> class KVArrayAccessorClass : public StringValueClass  // NAME 3: by convention (D24) from kv_acc_array.h
{
public:
	KVArrayAccessorClass(T *array, unsigned int capacity, char separator) :
		Array(array), Capacity(capacity), Separator(separator), Text("")
	{
	}
	virtual const char *Get() const;								// +0x04 0x4C32D0 (float)
	virtual void Set(const char *value);							// +0x08 0x4C34D0 (float)
	virtual StringValueClass *Clone() const { return new KVArrayAccessorClass<T>(Array, Capacity, Separator); }	// +0x0C 0x4C37E0 (float)

	T *Array;								// +0x04  // NAME 2: what it holds
	unsigned int Capacity;				// +0x08  // NAME 5: the FAIL_IF 'index >= Capacity'
	char Separator;						// +0x0C  // NAME 5: the assert 'strchr(buffer, Separator) == 0'
	mutable std::string Text;			// +0x10 what Get returns  // NAME 2: what it holds
};

// The accessor for an array (0x4C3110 for float: avatar_desc.cpp's weights)
template<class T> StringValueClass *Make_Array_Accessor(T *array, unsigned int capacity, char separator)  // NAME 2: what it does
{
	return new KVArrayAccessorClass<T>(array, capacity, separator);
}




// NOT EXACT: literal (the client compares the result of find with std::string::npos loaded from memory, ours folds it to -1; float: 0x4C34D0)
template<class T> void KVArrayAccessorClass<T>::Set(const char *value)
{
	Text = value;
	for (unsigned int i = 0; i < Capacity; i++) {
		Array[i] = 0;
	}
	if (Text.length() != 0) {
		unsigned int start = 0;
		std::string::size_type end = Text.find(Separator);
		unsigned int index = 0;
		while (end != std::string::npos) {
			FAIL_IF(index >= Capacity) return;
			std::string token = Text.substr(start, end - start);
			T item;


			bool converted = String_To_Value(token.c_str(), &item);
			G_ASSERT(converted);
			start = end + 1;
			Array[index] = item;
			index++;
			end = Text.find(Separator, start);
		}
	}
}






// (float: 0x4C32D0, exact but for the __FILE__ of this header string: the client has it relative, D41)
template<class T> const char *KVArrayAccessorClass<T>::Get() const
{
	Text = "";
	for (unsigned int i = 0; i < Capacity; i++) {
		char buffer[128];
		Text += Value_To_String(buffer, sizeof(buffer), Array[i]);
		G_ASSERT(strchr(buffer, Separator) == 0);
		Text += Separator;
	}
	return Text.c_str();
}

#endif //__KV_ACC_ARRAY_H

/*
**	Reconstruction notes
**
**	Set clears the array, then converts each separated token (std::string::substr out of line,
**	0x4C39B0); the last token needs a trailing separator, which Get always writes. Make_Array_Accessor
**	is called out of line by the visitor (0x4BFED0).
*/
