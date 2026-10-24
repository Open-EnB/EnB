// Earth & Beyond client (reconstruction): app\shared\auxdata\linearbu.cpp. Notes at the end of the file.
#include <string.h>
#include "linearbu.h"
#include "..\unknown_debug.h"







LinearBufferClass::LinearBufferClass(int capacity, int grow_by) :
	Buffer(0),
	Size(0),
	Capacity(0),
	Pos(0),
	OwnsBuffer(false)
{
	GrowBy = grow_by;
	if (capacity != 0) {
		Buffer = new char[capacity];
		G_ASSERT(Buffer);
		if (Buffer == 0) {
			return;
		}
		OwnsBuffer = true;
	}
	Capacity = capacity;
}




LinearBufferClass::LinearBufferClass(const void *data, int size, int grow_by, bool copy) :
	Buffer(0),
	Size(0),
	Capacity(0),
	Pos(0),
	OwnsBuffer(false)
{
	GrowBy = grow_by;
	G_ASSERT(data != 0 && size > 0);
	if (data != 0 && size > 0) {
		if (!copy) {
			Buffer = (char *)data;
			Capacity = size;
			Size = size;
		} else {
			Buffer = new char[size];
			Capacity = size;
			Size = size;
			OwnsBuffer = true;
			memcpy(Buffer, data, size);
		}
	}
}

LinearBufferClass::~LinearBufferClass()
{
	if (OwnsBuffer) {
		delete [] Buffer;
	}
}









int LinearBufferClass::Read(void *buf, int len)
{
	G_ASSERT(buf != 0 && len >= 0);
	if (buf == 0 || len < 0) {
		return -1;
	}
	if (Pos > Size) {
		return 0;
	}
	if (len == 0) {
		return 0;
	}
	int count = Size - Pos;
	if (len < count) {
		count = len;
	}
	memcpy(buf, Buffer + Pos, count);
	Pos += count;
	return count;
}

int LinearBufferClass::Read(int pos, void *buf, int len)
{
	if (Seek(pos) == -1) {
		return -1;
	}
	return Read(buf, len);
}










char *LinearBufferClass::Read_String()
{
	int end = Pos;
	if (end <= Size) {
		while (Buffer[end] != 0) {
			end++;
			if (end == Size) {
				G_ASSERT(false);
				end = Size - 1;
				break;
			}
		}
		int size = end - Pos + 1;
		if (size > 0) {
			char *buf = new char[size + 1];
			int read = Read(buf, size);



			G_ASSERT(read == size);
			buf[size] = 0;
			return buf;
		}
	}
	return 0;
}



int LinearBufferClass::Write(const void *buf, int len)
{
	G_ASSERT(buf != 0 && len >= 0);
	if (buf == 0 || len < 0) {
		return -1;
	}
	if (Pos + len > Capacity) {
		if (Grow(Pos + len) == -1) {
			return -1;
		}
	}
	if (len == 0) {
		return 0;
	}
	memcpy(Buffer + Pos, buf, len);
	int end = Pos + len;
	Size = (Size > end) ? Size : end;
	Pos = end;
	return len;
}

int LinearBufferClass::Write(int pos, const void *buf, int len)
{
	if (Seek(pos) == -1) {
		return -1;
	}
	return Write(buf, len);
}

int LinearBufferClass::Write_String(const char *string)
{
	return Write(string, strlen(string) + 1);
}

int LinearBufferClass::Append(const void *buf, int len)
{
	if (Seek(Size) == -1) {
		return -1;
	}
	return Write(buf, len);
}

int LinearBufferClass::Seek_Relative(int offset)
{
	return Seek(Pos + offset);
}











int LinearBufferClass::Truncate(int len)
{
	if (len < 0) {
		G_ASSERT(len >= 0);
		return -1;
	}
	Size -= len;
	if (Pos > Size) {
		Pos = Size;
	}
	return Pos;
}

int LinearBufferClass::Get_Pos() const
{
	return Pos;
}






int LinearBufferClass::Seek(int pos)
{
	if (pos < 0) {
		G_ASSERT(pos >= 0);
		return -1;
	}
	Pos = pos;
	return pos;
}

int LinearBufferClass::Skip(int offset)
{
	Seek(Pos + offset);
	return Pos;
}





int LinearBufferClass::Grow(int len)
{
	G_ASSERT(len > Capacity);
	if (len <= Capacity) {
		return -1;
	}
	int capacity = Capacity;
	while (capacity < len) {
		capacity += GrowBy;
	}
	return Set_Capacity(capacity);
}

int LinearBufferClass::Get_Size() const
{
	return Size;
}




int LinearBufferClass::Set_Size(int size)
{
	if (size < 0) {
		G_ASSERT(size >= 0);
		return -1;
	}
	Size = size;
	if (Pos > size) {
		Pos = size;
	}
	return size;
}

int LinearBufferClass::Get_Capacity() const
{
	return Capacity;
}

int LinearBufferClass::Unknown_Set_Size(int size)
{
	Capacity = size;
	Size = size;
	if (Pos >= size) {
		Pos = size - 1;
	}
	return size;
}








int LinearBufferClass::Set_Capacity(int capacity)
{
	if (capacity < 0) {
		G_ASSERT(capacity >= 0);
		return -1;
	}
	bool owns = false;
	char *buf = 0;
	if (capacity != 0) {
		buf = new char[capacity];



		if (buf == 0) {
			G_ASSERT(buf);
			return -1;
		}
		owns = true;
	}
	if (Size > capacity) {
		Size = capacity;
		if (Pos > capacity) {
			Pos = capacity;
		}
	}
	if (buf != 0 && Buffer != 0) {
		memcpy(buf, Buffer, Size);
	}
	if (OwnsBuffer) {
		delete [] Buffer;
	}
	Buffer = buf;
	Capacity = capacity;
	OwnsBuffer = owns;
	return capacity;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4AAB60-0x4AB536: LinearBufferClass (D24), 21 functions, asserts on the client's
**	lines 22-310 (D25). Seek (line 223) and Read (line 75) are inlined where the client inlines them,
**	Grow (line 242) in Write, although each is defined after its callers. The forms the bytes
**	decide: GrowBy is assigned in the constructors' bodies (stored after the members set to 0), the
**	uncopied case first in the second constructor, Read_String's work inside `if (end <= Size)`,
**	Write's `Size = (Size > end) ? Size : end` before `Pos = end`, `Pos > size` with Pos first in
**	the comparisons, and Unknown_Set_Size returning its argument.
*/
