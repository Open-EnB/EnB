// Earth & Beyond client (reconstruction)
/*
**	The skill constructors: Skill, SkillAbility, RPGInfo (skill_data, ui_attrb_skills: 0x6A5210, 0x54B680).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_skill.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_SKILL_H
#define __AUX_SKILL_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_SKILL
#include "aux_roots_skill.h"	// the skill classes first, T<float> last (skill_data 0x6A5210)
#include "aux_roots_send.h"

#endif //__AUX_SKILL_H
