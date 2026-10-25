// Earth & Beyond client (reconstruction): the aux-data root's object, PROVISIONAL file name (M1.3).
// Notes at the end of the file.
// NAME 1 [unknown_root.cpp]: provisional file name: the object 0x4A3990-0x4A4780 has no assert and no __FILE__
#include <windows.h>
#include <mmsystem.h>
#include "unknown_root.h"
#include "encode.h"
#include "linearbu.h"
#include "policy.h"
#include "unknown_changes.h"

#pragma comment(lib, "winmm.lib")

static bool TicksFirst = true;			// 0xB72FF8
static DWORD TicksStart;				// 0xBE3A6C

unsigned int Unknown_Get_Ticks()
{
	if (TicksFirst) {
		TicksStart = timeGetTime();
		TicksFirst = false;
		return 0;
	}
	return timeGetTime() - TicksStart;
}

UnknownRootClass::UnknownRootClass(int unknown, const char *name) : DeltaCompoundClass(name), m_Unknown9C(unknown)
{
}

UnknownRootClass::~UnknownRootClass()
{
}

LinearBufferClass *UnknownRootClass::Encode(PolicyClass *policy, ChangeTableClass *changes)
{
	DeltaCollectorClass collector(policy, changes);
	Accept(collector);
	if (collector.Updated.Count == 0 && collector.Defaulted.Count == 0 && collector.Cleared.Count == 0) {
		return 0;
	}
	LinearBufferClass *buf = new LinearBufferClass(0x400, 0x400);
	DeltaListSetClass set(&collector.Updated, &collector.Defaulted, &collector.Cleared);
	int count = m_Unknown98;
	RecordTableClass records(count, &collector.Updated, &collector.Defaulted, &collector.Cleared);
	int updated = set.Lists[0]->Count;
	int defaulted = set.Lists[1]->Count;
	int cleared = set.Lists[2]->Count;
	if (4 * (updated + defaulted + cleared) + 12 <= records.Unknown_Sum()) {
		char encoding = 0;
		if (buf->Write(&encoding, 1) != 1 || !set.Write(buf)) {
			delete buf;
			return 0;
		}
	} else {
		char encoding = 1;
		if (buf->Write(&encoding, 1) != 1 || !records.Unknown_All(buf)) {
			delete buf;
			return 0;
		}
	}
	return buf;
}

bool DeltaCompoundClass::Decode(LinearBufferClass &buf, const DurationClass &base)
{
	char encoding;
	buf.Read(&encoding, 1);
	DeltaCodecClass *codec;
	if (encoding == 0) {
		codec = &RecordCodec;
	} else {
		codec = Get_Tree_Codec();
	}
	return codec->Decode(this, buf, base);
}

UnknownSendRootClass::UnknownSendRootClass(int unknown, const char *) : UnknownRootClass(unknown, "Hull"), Policy(0), Changes(0)
{
}

// The send root's policy (Unknown_48): a rule set enabled always, with one rule, "*" INCLUDE.
// vtable 0xAEACC0: a condition that is always true
class PolicyAlwaysClass : public PolicyConditionClass  // NAME 2: describes what it does
{
public:
	virtual bool Evaluate(int, int) { return true; }					// 0x4A4400
	virtual void Attach(PolicyStateClass *, int) {}					// 0x4A42D0
};

// vtable 0xAEACA8: a rule set with a condition of its own
class PolicyConditionalSetClass : public PolicyRuleSetClass  // NAME 2: describes what it does
{
public:
	PolicyConditionalSetClass(RefCountPtr<PolicyConditionClass> condition) : Condition(condition) {}
	virtual RefCountPtr<PolicyConditionClass> Get_Condition(int, int) { return Condition; }	// 0x4A4320

	RefCountPtr<PolicyConditionClass> Condition;		// +0x18  // NAME 2: describes what it does
};

// vtable 0xAEAC9C: every field INCLUDE, always
class PolicyIncludeAllClass : public PolicyConditionalSetClass  // NAME 2: describes what it does
{
public:
	PolicyIncludeAllClass() : PolicyConditionalSetClass(Create_NEW((PolicyConditionClass *)new PolicyAlwaysClass))
	{
		Add_Rule(NameClass("*"), INCLUDE, -1);
	}
};

void UnknownSendRootClass::Unknown_48()
{
	DeltaCompoundClass::Unknown_48();
	PolicyIncludeAllClass *rule_set = new PolicyIncludeAllClass;
	Policy = new PolicyClass;
	Policy->Add(rule_set);
	m_UnknownA4 = Policy->Make_State(0, 0);
	Policy->Bind(m_UnknownA4, this);
	Changes = new ChangeTableClass(this);
}

UnknownSendRootClass::~UnknownSendRootClass()
{
	delete Policy;
	delete m_UnknownA4;
	delete Changes;
}

LinearBufferClass *UnknownSendRootClass::Encode_Changes(unsigned int time)
{
	Changes->Time = time;
	if (Changes->Root->Dirty || Changes->Delta->m_Unknown70 != Changes->Root->Stamp) {
		Changes->Scan();
	}
	if (Changes->Entries[m_Unknown7C].Changed > 0) {
		return Encode(Policy, Changes);
	}
	return 0;
}

// 0x4B0860: in another object (0x4AF.../0x4B0..., not attributed either); here until it has a file
void DeltaVisitorClass::Visit_Children(DeltaCompoundClass *delta)
{
	int count = delta->Children.size();
	for (int i = 0; i < count; i++) {
		delta->Children[i]->Accept(*this);
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A3990-0x4A4780 (no assert, no __FILE__: the file name is provisional, as
**	unknown_root.cpp; the user decides the name). Rebuilt here:
**	* Unknown_Get_Ticks 0x4A3B40, the aux-data clock (TicksFirst 0xB72FF8, TicksStart 0xBE3A6C).
**	* UnknownRootClass (vtable 0xAEAB88, size 0xA0): constructor 0x4A3BD0 (int, name: the int at
**	  +0x9C), destructor 0x4A3D30 (deleting 0x4A3D00). The 10 roots of the CREATE factory 0x6F6000
**	  and the Player root 0x752510 derive from it (app/client/aux_roots.cpp, generated).
**	* DeltaCompoundClass::Decode 0x4A3FA0: the encoding byte picks the record form (0) or the tree
**	  form (encode.cpp). The packet entry 0x7364C0 calls it on the root.
**	* DeltaVisitorClass::Visit_Children 0x4B0860 (another object, unattributed).
**	The same object also has DeltaClass's inline virtuals 0x4A3C50-0x4A3CB0 and DeltaCompoundClass's
**	vtable 0xAEABE4 and deleting destructor 0x4A3CD0 (emitted here first: compiler-generated).
**
**	Not rebuilt:
**	* 0x4A3990-0x4A3B10: value conversions (sscanf of 12 floats into a 3x4 matrix, __int64 text
**	  conversions through 0x8B44E0 / 0x8B44C0): strconv_lib.h's, inlined or instanced here?
**	* 0x4A3D50, the root's encode (the update collector 0x4A8AF0 / 0x4A8B40, RecordTableClass or
**	  DeltaListSetClass, whichever is smaller: Unknown_Sum < 4 * (ids) + 12 gives the tree form),
**	  needs the collector (encode.cpp's 0x4A8990-0x4A8FC0, not rebuilt).
**	* A second root class, vtable 0xAEAC40, size 0xAC: constructor 0x4A3FF0 (name at 0xB72FFC),
**	  its Unknown_48 0x4A40B0 (makes the collector +0xA0, a policy object +0xA4 and the change
**	  table +0xA8, 0x4AB910), destructor 0x4A4590, 0x4A4660 (the encode driver), and the ref-counted
**	  helpers 0x4A42D0-0x4A4780 (vtables 0xAEAC9C-0xAEACD4).
*/
