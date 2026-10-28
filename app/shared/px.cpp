// Earth & Beyond client (reconstruction): app\shared\px.cpp. Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "px.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>

// 0x44B760
PxEnd::PxEnd(PxBox *box, int type) :
	Box(box),
	Pos(0.0f, 0.0f, 0.0f),
	Type(type)
{
	memset(Next, 0, sizeof(Next));
	memset(Prev, 0, sizeof(Prev));
}

// 0x44B7B0
PxEnd::~PxEnd()
{
}

// 0x44B7D0
PxBox::PxBox(void *owner) :
	List(NULL),
	Owner(owner),
	High(NULL),
	Low(NULL),
	Self(0),
	Test(-1),
	Mask(0)
{
	High = new PxEnd(this, PxEnd::HIGH);
	Low = new PxEnd(this, PxEnd::LOW);
}

// 0x44B930
PxBox::PxBox(void *owner, const PxBox &src) :
	List(NULL),
	Owner(owner),
	High(NULL),
	Low(NULL),
	Self(src.Self),
	Test(src.Test),
	Mask(src.Mask)
{
	High = new PxEnd(this, PxEnd::HIGH);
	Low = new PxEnd(this, PxEnd::LOW);
}

// 0x44BA20
PxBox::~PxBox()
{
	delete High;
	delete Low;
	Low = NULL;
	High = NULL;
}



















































































































// 0x44BA70
void PxBox::Get_Corners(Vector3 *low, Vector3 *high)
{
	FAIL_IF(!(low && high)) return;
	*low = Low->Pos;
	*high = High->Pos;
}

// 0x44BAF0
void PxBox::On_Enter(PxBox *b)
{
	printf("PxBox::On_Enter(this=%p, b=%p)\n", this, b);
}

// 0x44BB20
void PxBox::On_Exit(PxBox *b)
{
	printf("PxBox::On_Exit (this=%p, b=%p)\n", this, b);
}

// 0x44BB50
void PxBox::Remove()
{
	FAIL_IF(!List) return;
	List->Remove(this);
}

// 0x44BB90
void PxBox::Set_Self(int self)
{
	if (List) List->Set_Self(this, self);
	else Self = self;
}

// 0x44BBD0
// NOT EXACT: scheduling (the client reads Mask before testing List; attempts.py show 0x44BBD0)
void PxBox::Set_Test(int test)
{
	if (List) List->Set_Test_Mask(this, test, Mask);
	else Test = test;
}

// 0x44BC10
// NOT EXACT: scheduling (the client reads Test before testing List; attempts.py show 0x44BC10)
void PxBox::Set_Mask(int mask)
{
	if (List) List->Set_Test_Mask(this, Test, mask);
	else Mask = mask;
}

// 0x44BC50
void PxBox::Set_Test_Mask(int test, int mask)
{
	if (List) List->Set_Test_Mask(this, test, mask);
	else { Test = test; Mask = mask; }
}







// 0x44BC90
// NOT EXACT: inline budget (the client inlines Is_Valid_Float for low.Y and high.Y too; attempts.py show 0x44BC90)
void PxBox::Set_Corners(const Vector3 &low, const Vector3 &high)
{
	FAIL_IF(!low.Is_Valid()) return;
	FAIL_IF(!high.Is_Valid()) return;

	Vector3 new_low = low;
	Vector3 new_high = high;
	for (int i = 0; i < 3; i++) {


		FAIL_IF(new_low[i] > new_high[i]) {
			float temp = new_low[i];
			new_low[i] = new_high[i];
			new_high[i] = temp;
		}
	}

	if (List) {
		List->Set_Corners(this, new_low, new_high);
	} else {
		Low->Pos = new_low;
		High->Pos = new_high;
	}
}

static bool Overlap(PxBox *a, PxBox *b)  // NAME 2: what it tests
{
	for (int i = 0; i < 3; i++) {
		if (b->High->Pos[i] < a->Low->Pos[i] || a->High->Pos[i] < b->Low->Pos[i]) {
			return(false);
		}
	}
	return(true);
}

// 0x44BE90
PxList::PxList() :
	PxEnd(NULL, LOW),
	Unknown2C(0),
	Updating(false)
{
	for (int i = 0; i < 3; i++) {
		Next[i] = this;
		Prev[i] = this;
	}
}

// 0x44BF00
PxList::~PxList()
{
	FAIL_IF(!Is_Empty()) {
		Remove_All();
	}
	for (int i = 0; i < 3; i++) {
		Prev[i] = NULL;
		Next[i] = NULL;
	}
}


// 0x44BFC0
void PxList::Insert(PxBox *box)
{
	WWPROFILE("PxList::Insert");
	FAIL_IF(!box) return;


	FAIL_IF(box->List) return;

	Updating = true;
	for (int i = 0; i < 3; i++) {
		PxEnd *low = box->Low;
		PxEnd *end = Next[i];
		float pos = low->Pos[i];
		while (end != this && end->Pos[i] < pos) {
			end = end->Next[i];
		}
		low->Prev[i] = end->Prev[i];
		low->Next[i] = end;
		end->Prev[i]->Next[i] = low;
		end->Prev[i] = low;

		PxEnd *high = box->High;
		pos = high->Pos[i];
		while (end != this && end->Pos[i] <= pos) {
			end = end->Next[i];
		}
		high->Prev[i] = end->Prev[i];
		high->Next[i] = end;
		end->Prev[i]->Next[i] = high;
		end->Prev[i] = high;
	}
	box->List = this;

	for (PxEnd *end = Next[0]; end != box->High; end = end->Next[0]) {
		if (end->Type == LOW) {
			PxBox *other = end->Box;
			if (other != box) {
				bool box_wants = box->Wants(other);
				bool other_wants = other->Wants(box);
				if (box_wants || other_wants) {
					if (Overlap(box, other)) {
						if (box_wants) box->On_Enter(other);
						if (other_wants) other->On_Enter(box);
					}
				}
			}
		}
	}
	Updating = false;
}





















































// 0x44C2E0
// NOT EXACT: register (base and index swapped in one address of the overlap test; attempts.py show 0x44C2E0)
void PxList::Remove(PxBox *box)
{
	FAIL_IF(!box) return;


	FAIL_IF(box->List != this) return;

	Updating = true;
	for (PxEnd *end = box->High->Prev[0]; end != this; end = end->Prev[0]) {
		if (end->Type == LOW) {
			PxBox *other = end->Box;
			if (box != other) {
				bool box_wants = box->Wants(other);
				bool other_wants = other->Wants(box);
				if (box_wants || other_wants) {
					if (Overlap(box, other)) {
						if (box_wants) box->On_Exit(other);
						if (other_wants) other->On_Exit(box);
					}
				}
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		PxEnd *low = box->Low;
		low->Prev[i]->Next[i] = low->Next[i];
		low->Next[i]->Prev[i] = low->Prev[i];
		low->Prev[i] = NULL;
		low->Next[i] = NULL;
		PxEnd *high = box->High;
		high->Prev[i]->Next[i] = high->Next[i];
		high->Next[i]->Prev[i] = high->Prev[i];
		high->Prev[i] = NULL;
		high->Next[i] = NULL;
	}
	box->List = NULL;
	Updating = false;
}

// 0x44C4E0
void PxList::Remove_All()
{
	Updating = true;
	while (Next[0] != this) {
		Remove(Next[0]->Box);
	}
	Updating = false;
}





























// 0x44C520
// NOT EXACT: register (as 0x44C2E0; attempts.py show 0x44C520)
void PxList::Set_Self(PxBox *box, int self)
{
	FAIL_IF(!box) return;
	if (box->Self == self) return;

	Updating = true;
	int old_self = box->Self;
	for (PxEnd *end = box->High->Prev[0]; end != this; end = end->Prev[0]) {
		if (end->Type == LOW) {
			PxBox *other = end->Box;
			if (other != box) {
				bool wanted = (old_self & other->Mask) == other->Test;
				bool now_wanted = (self & other->Mask) == other->Test;
				if (wanted != now_wanted) {
					if (Overlap(box, other)) {
						if (wanted) {
							other->On_Exit(box);
						} else {
							box->Self = self;
							other->On_Enter(box);
							box->Self = old_self;
						}
					}
				}
			}
		}
	}
	box->Self = self;
	Updating = false;
}










































// 0x44C690
// NOT EXACT: register (as 0x44C2E0; attempts.py show 0x44C690)
void PxList::Set_Test_Mask(PxBox *box, int test, int mask)
{
	FAIL_IF(!box) return;
	if (box->Test == test && box->Mask == mask) return;

	Updating = true;
	int old_test = box->Test;
	int old_mask = box->Mask;
	for (PxEnd *end = box->High->Prev[0]; end != this; end = end->Prev[0]) {
		if (end->Type == LOW) {
			PxBox *other = end->Box;
			if (other != box) {
				bool wanted = (other->Self & old_mask) == old_test;
				bool now_wanted = (other->Self & mask) == test;
				if (wanted != now_wanted) {
					if (Overlap(box, other)) {
						if (wanted) {
							box->On_Exit(other);
						} else {
							box->Test = test;
							box->Mask = mask;
							box->On_Enter(other);
							box->Test = old_test;
							box->Mask = old_mask;
						}
					}
				}
			}
		}
	}
	box->Test = test;
	box->Mask = mask;
	Updating = false;
}

static std::vector<PxBox *> Notified;  // NAME 2: the boxes a corner change has already reported

// 0x44C900
// NOT EXACT: inline budget (the client calls vector::insert(it, x) out of line; attempts.py show 0x44C900)
void PxList::Set_Corners(PxBox *box, const Vector3 &low, const Vector3 &high)
{
	WWPROFILE("PxList::Set_Corners");
	Updating = true;
	Vector3 old_low = box->Low->Pos;
	Vector3 old_high = box->High->Pos;
	std::vector<PxBox *>::iterator it = std::lower_bound(Notified.begin(), Notified.end(), box);
	if (it == Notified.end() || *it != box) {
		Notified.insert(it, box);
	}

	for (int i = 0; i < 3; i++) {
		PxEnd *end;
		PxEnd *low_end = box->Low;
		if (low[i] < old_low[i]) {
			end = low_end->Prev[i];
			if (end == this || end->Pos[i] < low[i]) {
				low_end->Pos[i] = low[i];
			} else {
				for (; end != this && low[i] <= end->Pos[i]; end = end->Prev[i]) {
					if (end->Type == HIGH) {
						PxBox *other = end->Box;
						bool other_wants = other->Wants(box);
						bool box_wants = box->Wants(other);
						if (other_wants || box_wants) {
							int j;
							for (j = 0; j < 3; j++) {
								if (high[j] < other->Low->Pos[j] || other->High->Pos[j] < low[j]) break;
							}
							if (j == 3) {
								it = std::lower_bound(Notified.begin(), Notified.end(), other);
								if (it == Notified.end() || *it != other) {
									Notified.insert(it, other);
									if (box_wants) box->On_Enter(other);
									if (other_wants) other->On_Enter(box);
								}
							}
						}
					}
				}
				low_end = box->Low;
				PxEnd *next = end->Next[i];
				low_end->Prev[i]->Next[i] = low_end->Next[i];
				low_end->Next[i]->Prev[i] = low_end->Prev[i];
				low_end->Prev[i] = next->Prev[i];
				low_end->Next[i] = next;
				next->Prev[i]->Next[i] = low_end;
				next->Prev[i] = low_end;
				box->Low->Pos[i] = low[i];
			}
		} else {
			end = low_end->Next[i];
			if (end == this || low[i] <= end->Pos[i]) {
				low_end->Pos[i] = low[i];
			} else {
				float value = low[i];
				for (; end != this && end->Pos[i] < value; end = end->Next[i]) {
					if (end->Type == HIGH) {
						PxBox *other = end->Box;
						bool other_wants = other->Wants(box);
						bool box_wants = box->Wants(other);
						if (other_wants || box_wants) {
							int j;
							for (j = 0; j < 3; j++) {
								if (other->High->Pos[j] < old_low[j] || old_high[j] < other->Low->Pos[j]) break;
							}
							if (j == 3) {
								it = std::lower_bound(Notified.begin(), Notified.end(), other);
								if (it == Notified.end() || *it != other) {
									Notified.insert(it, other);
									if (box_wants) box->On_Exit(other);
									if (other_wants) other->On_Exit(box);
								}
							}
						}
					}
				}
				low_end = box->Low;
				low_end->Prev[i]->Next[i] = low_end->Next[i];
				low_end->Next[i]->Prev[i] = low_end->Prev[i];
				low_end->Prev[i] = end->Prev[i];
				low_end->Next[i] = end;
				end->Prev[i]->Next[i] = low_end;
				end->Prev[i] = low_end;
				box->Low->Pos[i] = value;
			}
		}

		if (high[i] < old_high[i]) {
			end = box->High->Prev[i];
			if (end == this || end->Pos[i] <= high[i]) {
				box->High->Pos[i] = high[i];
			} else {
				float value = high[i];
				for (; end != this && value < end->Pos[i]; end = end->Prev[i]) {
					if (end->Type == LOW) {
						PxBox *other = end->Box;
						bool other_wants = other->Wants(box);
						bool box_wants = box->Wants(other);
						if (other_wants || box_wants) {
							int j;
							for (j = 0; j < 3; j++) {
								if (other->High->Pos[j] < old_low[j] || old_high[j] < other->Low->Pos[j]) break;
							}
							if (j == 3) {
								it = std::lower_bound(Notified.begin(), Notified.end(), other);
								if (it == Notified.end() || *it != other) {
									Notified.insert(it, other);
									if (box_wants) box->On_Exit(other);
									if (other_wants) other->On_Exit(box);
								}
							}
						}
					}
				}
				PxEnd *high_end = box->High;
				PxEnd *next = end->Next[i];
				high_end->Prev[i]->Next[i] = high_end->Next[i];
				high_end->Next[i]->Prev[i] = high_end->Prev[i];
				high_end->Prev[i] = next->Prev[i];
				high_end->Next[i] = next;
				next->Prev[i]->Next[i] = high_end;
				next->Prev[i] = high_end;
				box->High->Pos[i] = value;
			}
		} else {
			end = box->High->Next[i];
			if (end == this || high[i] < end->Pos[i]) {
				box->High->Pos[i] = high[i];
			} else {
				for (; end != this && end->Pos[i] <= high[i]; end = end->Next[i]) {
					if (end->Type == LOW) {
						PxBox *other = end->Box;
						bool other_wants = other->Wants(box);
						bool box_wants = box->Wants(other);
						if (other_wants || box_wants) {
							int j;
							for (j = 0; j < 3; j++) {
								if (high[j] < other->Low->Pos[j] || other->High->Pos[j] < low[j]) break;
							}
							if (j == 3) {
								it = std::lower_bound(Notified.begin(), Notified.end(), other);
								if (it == Notified.end() || *it != other) {
									Notified.insert(it, other);
									if (box_wants) box->On_Enter(other);
									if (other_wants) other->On_Enter(box);
								}
							}
						}
					}
				}
				PxEnd *high_end = box->High;
				high_end->Prev[i]->Next[i] = high_end->Next[i];
				high_end->Next[i]->Prev[i] = high_end->Prev[i];
				high_end->Prev[i] = end->Prev[i];
				high_end->Next[i] = end;
				end->Prev[i]->Next[i] = high_end;
				end->Prev[i] = high_end;
				box->High->Pos[i] = high[i];
			}
		}
	}
	Notified.clear();
	Updating = false;
}

// 0x44D210
void PxList::Dump()
{
	for (int i = 0; i < 3; i++) {
		printf("Axis %d\n", i);
		for (PxEnd *end = Next[i]; end != this; end = end->Next[i]) {
			printf("  Type  = %s\n", (end->Type == HIGH) ? "HIGH" : "LOW");
			printf("  Box   = %p\n", end->Box);
			printf("  Self  = %d\n", end->Box->Self);
			printf("  Test  = %d\n", end->Box->Test);
			printf("  Mask  = %d\n", end->Box->Mask);
			printf("  Pos   = %f\n", end->Pos[i]);
			printf("\n");
		}
	}
}

// 0x44D320
bool PxList::Is_Sorted()
{
	for (int i = 0; i < 3; i++) {
		for (PxEnd *end = Next[i]; end != this; end = end->Next[i]) {
			PxEnd *next = end->Next[i];
			if (next != this && next->Pos[i] < end->Pos[i]) {
				return(false);
			}
		}
	}
	return(true);
}

/*
** Notes
**
** px.cpp is client 0x44B760-0x44D740, after dynamic_obj.cpp and before px_box_list.cpp (0x44D740 on). Its __FILE__
** is pushed by every FAIL_IF here (asserts.txt files the first one, '!(low && high)' 0x44BA70, under
** dynamic_obj.cpp). The class names PxBox and PxList are the client's (their profile and debug strings); PxEnd, the
** member names other than List and the header name are provisional (the dump's labels name Box, Self, Test, Mask,
** Pos and the LOW / HIGH type).
**
** A sweep and prune: PxList is the sentinel of three sorted, doubly linked lists of box ends (one per axis). Insert
** links a box's two ends into each axis, then reports every overlapping box along x; Remove reports the overlaps
** ending and unlinks it. Set_Corners moves each end past its neighbours one axis at a time: a low end moving down
** or a high end moving up can start an overlap (On_Enter), the opposite moves can end one (On_Exit); the static
** Notified vector keeps each other box to one report per call. Set_Self / Set_Test_Mask report the overlaps whose
** filter result changes. A box reports b only when (b->Self & Mask) == Test.
**
** The vector<PxBox *> members 0x44D370-0x44D6D0 (insert, lower_bound, clear and their helpers) are this file's
** instances. 0x44D040, between Set_Corners and Dump, is not px.cpp's (PX2): it is the implicit copy constructor of
** a 0x5C-byte structure (std::strings at +0x00, +0x10, +0x24, +0x34, ints at +0x20, +0x44, +0x48, a bool at +0x4C,
** a vector of 4-byte elements at +0x50), called only from 0x4708F0 (a template instance in the no-file stretch
** after rs_track.cpp), whose callers 0x5FA0B0, 0x5FA5F0 and 0x600000 are ui_registration.cpp's. Both carry an
** unusual tail of padding (0x94 bytes after 0x44D040, 0x25C after 0x4708F0, against px.cpp's usual 0x0E): the
** incremental linker (the client's thunks show /INCREMENTAL) put them in free space left by a relink.
*/
