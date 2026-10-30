/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//
// rc4.cpp
// RC4 encryption / decryption
//
#include "rc4.h"
#include <memory.h>
#include <stdio.h>


//
// The key state every key setup starts from (State[i] = i, X = Y = 0), built by the first
// constructor.
//
RC4Class::RC4Key	RC4Class::InitialKey;
bool					RC4Class::InitialKeyBuilt = false;


RC4Class::RC4Class()
{
	if (!InitialKeyBuilt) {
		InitialKeyBuilt = true;
		for (unsigned int i = 0; i < 256; i++) {
			InitialKey.State[i] = (unsigned char)i;
		}
		InitialKey.X = 0;
		InitialKey.Y = 0;
	}
}


//
// Copy state & key
//
RC4Class &RC4Class::operator=(const RC4Class &other) {
	if (this == &other)
		return(*this);

	Key.X=other.Key.X;
	Key.Y=other.Key.Y;
	memcpy(Key.State, other.Key.State, 256);

	return(*this);
}


//
// Setup the encryption key.  This must be called before you encrypt/decrypt!
//
void RC4Class::Prepare_Key(const unsigned char *key_data_ptr, int key_data_len)
{
	unsigned char	*state;
	unsigned char	index1;
	unsigned char	index2;
	unsigned char	temp;
	int				counter;

	Key = InitialKey;
	state = &Key.State[0];
	index1 = 0;
	index2 = 0;

	switch (key_data_len) {
		case 8:
			for (counter = 0; counter < 256; counter++) {
				index2 = (unsigned char)(key_data_ptr[index1] + state[counter] + index2);
				temp = state[counter];
				state[counter] = state[index2];
				state[index2] = temp;
				++index1;
				index1 &= 0x07;
			}
			break;

		case 16:
			//
			//	C equivalent (the same key schedule as the other cases, with index1 wrapping at 16):
			//
			//		for (counter = 0; counter < 256; counter++) {
			//			index2 = (unsigned char)(key_data_ptr[index1] + state[counter] + index2);
			//			swap(state[counter], state[index2]);
			//			index1 = (index1 + 1) & 0x0F;
			//		}
			//
			//	Registers: esi = key, edi = state, ecx (cl) = counter, edx (dl) = index2,
			//	ebx = index1. The loop ends when cl wraps from 255 to 0. The index1/index2
			//	variables in memory are not updated (nothing reads them afterwards).
			//	Why assembly: all the state kept in registers; see RC4().
			//
			__asm {
				mov		esi, key_data_ptr
				mov		edi, state
				xor		ecx, ecx
				xor		edx, edx
				xor		ebx, ebx
				jmp		key16_loop
				align		16
key16_loop:
				mov		al, [esi+ebx]
				add		dl, [edi+ecx]
				inc		ebx
				add		dl, al
				mov		al, [edi+ecx]
				mov		ah, [edi+edx]
				and		ebx, 0x0f
				mov		[edi+ecx], ah
				inc		cl
				mov		[edi+edx], al
				jnz		key16_loop
			}
			break;

		default:
			for (counter = 0; counter < 256; counter++) {
				index2 = (unsigned char)(key_data_ptr[index1] + state[counter] + index2);
				temp = state[counter];
				state[counter] = state[index2];
				state[index2] = temp;
				index1 = (unsigned char)((index1 + 1) % key_data_len);
			}
			break;
	}
}


void RC4Class::Print_State(void) {
	unsigned char	*state;
	state = &Key.State[0];
	for (int i=0; i<256; i+=5) {
		printf("	%3d	%3d	%3d	%3d	%3d\n",state[i], state[i+1], state[i+2], state[i+3], state[i+4]);
	}
	printf("X = %d		Y = %d\n\n",Key.X,Key.Y);
}


//
// RC4 in standard mode.
//
// This will XOR the buffer with the RC4 stream (like a one time pad).
//
void RC4Class::RC4(unsigned char *buffer_ptr, int buffer_len)
{
	unsigned char	x;
	unsigned char	y;
	unsigned char	*state;

	x = Key.X;
	y = Key.Y;
	state = &Key.State[0];

	//
	//	C equivalent:
	//
	//		do {
	//			x++;
	//			y += state[x];
	//			swap(state[x], state[y]);
	//			*buffer_ptr++ ^= state[(state[x] + state[y]) & 0xFF];
	//		} while (--buffer_len > 0);
	//
	//	Note: the loop tests its count at the bottom, so a call with buffer_len == 0 still
	//	encrypts one byte (a quirk of the original).
	//	Registers: esi = state, edi = buffer, ecx = position, bl = x, dl = y, al/ah = the two
	//	state bytes being swapped. EBP is borrowed as the loop limit (pushed and popped around
	//	the loop), so no local variable can be used inside it.
	//	Why assembly: RC4 runs over every network packet; the compiler of the day kept x, y and
	//	the position in memory and could not use EBP as a register.
	//
	__asm {
		xor		ecx, ecx
		mov		esi, state
		mov		edi, buffer_ptr
		xor		ebx, ebx
		xor		edx, edx
		mov		bl, x
		mov		dl, y
		inc		bl
		mov		eax, buffer_len
		push		ebp
		mov		ebp, eax
		add		dl, [esi+ebx]
		mov		ah, [esi+edx]
		jmp		rc4_loop
		align		16
rc4_loop:
		mov		al, [esi+ebx]
		mov		[esi+ebx], ah
		add		ah, al
		mov		[esi+edx], al
		shr		eax, 8
		inc		bl
		mov		al, [esi+eax]
		add		dl, [esi+ebx]
		xor		[edi+ecx], al
		inc		ecx
		mov		ah, [esi+edx]
		cmp		ecx, ebp
		jl			rc4_loop
		pop		ebp
		sub		dl, [esi+ebx]
		dec		bl
		mov		y, dl
		mov		x, bl
	}

	Key.X = x;
	Key.Y = y;
}
