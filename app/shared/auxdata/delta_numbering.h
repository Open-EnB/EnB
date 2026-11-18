// Earth & Beyond client (reconstruction)
/*
**	The Delta templates' per-class globals, numbered at start-up from DeltaUnknownCounter (0xBE3B44):
**	DeltaTemplateClass<T>, DeltaArrayClass<T>, DeltaItemArrayClass<T> (float: 0xC416F0, float array: 0xC415DC,
**	the item array: 0xC41624). Only the objects that include this header define (and number) them, as the
**	client's do: mainview, ui_mount, cameraeffects ... look fields up through UnknownRTTIClass<T>::Cast, which
**	reads the global, yet their CRT entries number nothing (their guarded <string> initialisers 0x514F60,
**	0x5C8750, 0x6E4720 hold no numbering), while skill_data 0x6A5210, ui_group 0x598D00 ... number the types
**	their constructors build (workflow.md 3.4). The aux_<family>.h headers include it first.
**
**	// NAME 1 [delta_numbering.h]: provisional file name (no client string names the header)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_NUMBERING_H
#define __DELTA_NUMBERING_H

#include "delta_template.h"
#include "delta_array.h"
#include "delta_item_array.h"

template<class T> int DeltaTemplateClass<T>::UnknownGlobal = DeltaUnknownCounter++;
template<class T> int DeltaArrayClass<T>::UnknownGlobal = DeltaUnknownCounter++;
template<class T> int DeltaItemArrayClass<T>::UnknownGlobal = DeltaUnknownCounter++;

#endif //__DELTA_NUMBERING_H
