// Earth & Beyond client (reconstruction): app\shared\attrib_dir_collective.cpp. Notes at the end of the file.
#include <algorithm>
#include "..\shared\unknown_debug.h"
#include "attrib_dir_collective.h"












RefCountPtr<AttribValueFunctionClass> AttribAverageFunctionClass::Create()
{
	return Create_NEW(new AttribAverageFunctionClass);
}












AttribAverageFunctionClass::AttribAverageFunctionClass() : Changed(false), Unknown1C(0)
{
}









void AttribAverageFunctionClass::Add(float value)
{
	Values.push_back(value);
	Changed = true;
}

void AttribAverageFunctionClass::Remove(float value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), value);
	FAIL_IF(find == Values.end()) return;
	Values.erase(find);
	Changed = true;
}


void AttribAverageFunctionClass::Replace(float old_value, float new_value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), old_value);
	FAIL_IF(find == Values.end()) return;
	*find = new_value;
	Changed = true;
}











float AttribAverageFunctionClass::Calculate()
{
	if (Values.size() == 0) {
		return 0;
	}
	float sum = 0;
	for (std::vector<float>::iterator it = Values.begin(); it != Values.end(); ++it) {
		sum += *it;
	}
	return sum / Values.size();
}

RefCountPtr<AttribValueFunctionClass> AttribMinFunctionClass::Create()
{
	return Create_NEW(new AttribMinFunctionClass);
}

AttribMinFunctionClass::AttribMinFunctionClass() : Sorted(true)
{
}

void AttribMinFunctionClass::Add(float value)
{
	Values.push_back(value);
	Sorted = false;
}

void AttribMinFunctionClass::Remove(float value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), value);
	FAIL_IF(find == Values.end()) return;
	Values.erase(find);
}


void AttribMinFunctionClass::Replace(float old_value, float new_value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), old_value);
	FAIL_IF(find == Values.end()) return;
	*find = new_value;
	Sorted = false;
}










float AttribMinFunctionClass::Calculate()
{
	if (Values.size() == 0) {
		return 0;
	}
	if (!Sorted) {
		std::sort(Values.begin(), Values.end());
	}
	return Values.front();
}

RefCountPtr<AttribValueFunctionClass> AttribMaxFunctionClass::Create()
{
	return Create_NEW(new AttribMaxFunctionClass);
}

AttribMaxFunctionClass::AttribMaxFunctionClass() : Sorted(true)
{
}

void AttribMaxFunctionClass::Add(float value)
{
	Values.push_back(value);
	Sorted = false;
}

void AttribMaxFunctionClass::Remove(float value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), value);
	FAIL_IF(find == Values.end()) return;
	Values.erase(find);
}


void AttribMaxFunctionClass::Replace(float old_value, float new_value)
{
	std::vector<float>::iterator find = std::find(Values.begin(), Values.end(), old_value);
	FAIL_IF(find == Values.end()) return;
	*find = new_value;
	Sorted = false;
}







float AttribMaxFunctionClass::Calculate()
{
	if (Values.size() == 0) {
		return 0;
	}
	if (!Sorted) {
		std::sort(Values.begin(), Values.end());
	}
	return Values.back();
}

AttribFunctionModClass::AttribFunctionModClass(const RefCountPtr<AttribValueFunctionClass> &function, int type, void *owner) : ValueFunction(function), Type(type), Owner(owner)
{
	G_ASSERT(ValueFunction);
}






AttribFunctionModClass::~AttribFunctionModClass()
{
	ValueFunction = 0;
}

float AttribFunctionModClass::Get_Value()
{
	FAIL_IF(!ValueFunction) return 0;
	return ValueFunction->Calculate();
}


int Unknown_4E0700(AttribDir *dir, AttribModIterator mods, void *owner, bool keep);	// 0x4E0700: detaches (and unless keep deletes) owner's mods  // NAME 1: neutral placeholder (D10)
AttribModClass *Unknown_4DFBF0(AttribDir *dir, const std::string &name, AttribDir *member, const std::string &member_name, int type, void *owner);	// 0x4DFBF0: links a member's bound (type 4 / 5) into dir  // NAME 1: neutral placeholder (D10)





AttribDirCollective::AttribDirCollective()
{
}

// NOT EXACT: inline budget (explain.py: 19 vs 20 calls), the client keeps the member loop's test without a zero register and calls more of the tables' helpers out of line
AttribDirCollective::~AttribDirCollective()
{
	FAIL_IF(!Members.empty()) {
		while (!Members.empty()) {
			Remove_Member((AttribDir *)(*Members.begin()).first);
		}
	}
	for (DestinationMap::iterator it = DestinationAttributes.begin(); it != DestinationAttributes.end(); ++it) {
		DestinationStruct *&destination = (*it).second;
		Detach_Owned_Mods(&Dir);
		delete destination;
	}
	DestinationAttributes.clear();
}











// NOT EXACT: shape (explain.py), the client keeps the member loop's iterator in memory and the member and its struct in edi / ebx (a member / info local pair grows our frame instead)
void AttribDirCollective::Add_Destination(const std::string &source, const std::string &destination, const RefCountPtr<AttribValueFunctionClass> &function)
{
	DestinationMap::iterator find = DestinationAttributes.find(source);
	FAIL_IF(find != DestinationAttributes.end()) return;
	DestinationAttributes[source] = new DestinationStruct(destination, function);
	Dir.Attach_Mod(destination.c_str(), new AttribFunctionModClass(function, 0, this));
	MemberMap::iterator it;
	for (it = Members.begin(); it != Members.end(); ++it) {
		MemberStruct *member = (*it).second;
		float value = ((AttribDir *)(*it).first)->Get_Value(source.c_str());
		function->Add(value);
		member->Values[source] = value;
		((AttribDir *)(*it).first)->Attach_Callback(source.c_str(), member->Callback);
	}
	for (it = Members.begin(); it != Members.end(); ++it) {
		Link((AttribDir *)(*it).first, destination);
	}
}




























// NOT EXACT: inline budget (explain.py: frame 0x34 vs 0x28), the client calls MemberStruct's constructor 0x4DD240 out of line; ours inlines it (defined here or in the class alike)
void AttribDirCollective::Add_Member(AttribDir *member)
{
	MemberMap::iterator find = Members.find((unsigned int)member);
	FAIL_IF(find != Members.end()) return;
	MemberCallbackClass *callback = new MemberCallbackClass(*this, member);
	MemberStruct *info = new MemberStruct;
	info->Callback = callback;
	Members[(unsigned int)member] = info;
	for (DestinationMap::iterator it = DestinationAttributes.begin(); it != DestinationAttributes.end(); ++it) {
		const std::string &attribute = (*it).first;
		DestinationStruct *&destination = (*it).second;
		float value = member->Get_Value(attribute.c_str());
		info->Values[attribute] = value;
		destination->Function->Add(value);
		Dir.Recalculate(destination->Name.c_str());
		member->Attach_Callback(attribute.c_str(), info->Callback);
		Link(member, destination->Name);
	}
}














void AttribDirCollective::Remove_Member(AttribDir *member)
{
	MemberMap::iterator find = Members.find((unsigned int)member);
	FAIL_IF(find == Members.end()) return;
	MemberStruct *info = (*find).second;
	Members.erase(find);
	Detach_Owned_Mods(member);
	for (DestinationMap::iterator it = DestinationAttributes.begin(); it != DestinationAttributes.end(); ++it) {
		const std::string &attribute = (*it).first;
		DestinationStruct *&destination = (*it).second;
		member->Detach_Callback(attribute.c_str(), info->Callback);
		float value = info->Values[attribute];
		destination->Function->Remove(value);
		Dir.Recalculate(destination->Name.c_str());
	}
	delete info->Callback;
	delete info;
}





















void AttribDirCollective::Link(AttribDir *member, const std::string &destination)
{
	Unknown_4DFBF0(&Dir, destination, member, destination, 4, this);
	Unknown_4DFBF0(&Dir, destination, member, destination, 5, this);
}

void AttribDirCollective::Detach_Owned_Mods(AttribDir *dir)
{
	Unknown_4E0700(dir, dir->Get_All_Mods(), this, false);
}

AttribDirCollective::MemberStruct::MemberStruct()
{
}

AttribDirCollective::MemberCallbackClass::MemberCallbackClass(AttribDirCollective &parent, AttribDir *member) : Parent(parent), Member(member)
{
}

void AttribDirCollective::MemberCallbackClass::Value_Changed(const char *name, float value)
{
	DestinationMap::iterator attribute_find = Parent.DestinationAttributes.find(name);
	FAIL_IF(attribute_find == Parent.DestinationAttributes.end()) {
	} else {
		const std::string &attribute = (*attribute_find).first;
		DestinationStruct *&destination = (*attribute_find).second;
		MemberMap::iterator member_find = Parent.Members.find((unsigned int)Member);

		FAIL_IF(member_find == Parent.Members.end()) {
		} else {
			MemberStruct *&info = (*member_find).second;
			float old_member_value = info->Values[attribute];

			FAIL_IF(value == old_member_value) {
			} else {
				info->Values[attribute] = value;
				destination->Function->Replace(old_member_value, value);
				Parent.Dir.Recalculate(destination->Name.c_str());
			}
		}
	}
}


/*
**	Reconstruction notes
**
**	client.exe 0x4DA7F0-0x4DBBA0 rebuilt here: the three value functions (average 0xAEC2DC, smallest
**	0xAEC2F8, largest 0xAEC314: Create, the out-of-line constructor, Add, Remove and Replace with their
**	FAIL_IFs on the client's lines 54 / 63, 108 / 116, 159 / 167, Calculate) and the function mod
**	(0xAEC330: G_ASSERT(ValueFunction) on 191, Get_Value's FAIL_IF on 206), with the vector, sort and
**	RefCountPtr instances. Calculate sorts without setting Sorted (as the client does). Then the
**	collective, 0x4DBBC0-0x4DCFF0: constructor, destructor (225), Add_Destination (252), Add_Member
**	(299), Remove_Member (332), Link, Detach_Owned_Mods, the member callback (0xAEC344, 390-401) and
**	the tables' instances. Members are keyed by the member AttribDir's address as an unsigned int (the
**	client converts it into a temporary for every find); the loops keep references to the key and the
**	destination. 0x4DFBF0 (links a member's bound into Dir) and 0x4E0700 (detaches an owner's mods)
**	are another object's: declared here.
*/
