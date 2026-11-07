// Earth & Beyond client (reconstruction): the object at client 0x84B360 (INTERIM file name, see the notes at the end).
#include <stdio.h>
#include <string>

struct TargetSummaryText	// NAME 2: what it holds
{
	std::string Scope;		// +0x00 ("to group", "to targets", ...)  // NAME 2: what it holds
	std::string Radius;		// +0x10 ("Effect Radius : %d")  // NAME 2: what it holds
	int Unknown20;			// +0x20 (only ever copied here, never initialised)
	int Unknown24;			// +0x24 (likewise)
};

// 0x84B360 (the implicit destructor is 0x84B8A0, the implicit copy constructor 0x84B930)
TargetSummaryText Target_Summary_Text(unsigned char flags, int radius)	// NAME 2: what it does; NAME 1 [flags, radius]: neutral
{
	TargetSummaryText text;
	char buffer[128];
	sprintf(buffer, "Effect Radius : %d", radius);
	if (flags & 2) {
		text.Scope = "to group";
		text.Radius.assign(buffer);
		return(text);
	}
	if (flags & 0x10) {
		if (flags & 4) {
			text.Scope = "to targets";
			text.Radius.assign(buffer);
			return(text);
		}
		if (flags & 8) {
			text.Scope = "to targets";
			return(text);
		}
		text.Scope = "to target";
		return(text);
	}
	if (flags & 0x20) {
		if (flags & 4) {
			text.Scope = "to enemies";
			text.Radius.assign(buffer);
			return(text);
		}
		if (flags & 8) {
			text.Scope = "to enemies";
			text.Radius.assign(buffer);
			return(text);
		}
		text.Scope = "to enemy";
		return(text);
	}
	return(text);
}

/*
**	Reconstruction notes. The client holds no __FILE__ for this object: it starts after stredit.cpp's <string>
**	initialiser (0x84B330) and sits before vendor_defs.cpp (0x84C270), and it has no caller. Its neighbours 0x84BE40-0x84C060
**	are the converters of TargetGeneratorSummaryBF (aux_roots.h; stubs/aux_roots_stubs.cpp), whose 'self / group / aoe /
**	pointblank / friend / enemy' names are the bits read here (2 group, 4 aoe, 8 pointblank, 0x10 friend, 0x20 enemy): hence the
**	interim file name, D137, NAME 3. `text.Radius.assign(buffer)` against `= buffer`: the operator's extra inline level
**	put the seventh return's destructor out of line and merged it with the last return (0.82); assign on Scope instead of
**	Radius is exact as well, so which of the two the original used is not decided by the bytes.
*/
