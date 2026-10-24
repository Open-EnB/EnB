/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\linearbu.h (name assumed from linearbu.cpp)
**
**	The buffer the aux-data fields read from and write to. Only what the delta files use so far: the
**	functions are linearbu.cpp's (not rebuilt yet; its asserts say "read == size"). The client
**	doesn't name the class (D24: "linear buffer", after the file name).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __LINEARBU_H
#define __LINEARBU_H

class LinearBufferClass
{
public:
	int Read(void *data, int size);					// 0x4AACE0, returns the bytes read
	int Write(const void *data, int size);			// 0x4AAFC0, returns the bytes written
};

#endif //__LINEARBU_H
