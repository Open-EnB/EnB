// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\item_timer_gauge.h (PROVISIONAL, D137: item_timer_gauge.cpp's header,
**	after its __FILE__ 'e:\build\beta\G\app\Client\item_timer_gauge.cpp' 0xB907F0; the client holds no string of it)
**
**	An item's timer gauge: the 'ItemTime' model attached beside a render object, its 'timer' bone turned by the time
**	left (0x18 bytes: the constructor 0x691170, the destructor 0x6912A0, the update 0x691320).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ITEM_TIMER_GAUGE_H  // NAME 3 [item_timer_gauge.h]: the header of item_timer_gauge.cpp
#define ITEM_TIMER_GAUGE_H

#include "vector3.h"

class RenderObjClass;

class ItemTimerGaugeClass  // NAME 3 [ItemTimerGaugeClass]: class-from-file (D24) for item_timer_gauge.cpp
{
public:
	ItemTimerGaugeClass(RenderObjClass *attach);	// 0x691170 (FAIL_IF '!attach' l.63, '!RObj' l.67, '!TimerRObj' l.71)  // NAME 3 [attach]: the FAIL_IF's
	~ItemTimerGaugeClass();							// 0x6912A0
	void Update(unsigned int time);					// 0x691320 (the time left in ms: minutes up to 60000, else hours)  // NAME 2 [all]: what it does

	RenderObjClass *RObj;							// +0x00 ('ItemTime')  // NAME 3: the FAIL_IF '!RObj'
	RenderObjClass *TimerRObj;						// +0x04 (its 'timer' sub object)  // NAME 3: the FAIL_IF '!TimerRObj'
	int TimerBone;									// +0x08 (RObj's 'timer' bone, captured)  // NAME 2: what it holds
	Vector3 Last;									// +0x0C (the last colour given 0x496110)  // NAME 2: what it holds
};

#endif
