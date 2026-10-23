/*
**	Earth & Beyond client (reconstruction)
**
**	Release-build definitions of the Generals debug macros that Zero Hour's wwdebug.h expects
**	from GameEngine\Include\Common\debug.h. In Generals' release configuration (no
**	ALLOW_DEBUG_UTILS) every one of them compiles to nothing; these are those definitions.
**	E&B's own header is not known.
*/

#ifndef __GAMEDEBUG_H
#define __GAMEDEBUG_H

#define DEBUG_INIT(f)					((void)0)
#define DEBUG_SHUTDOWN()				((void)0)
#define DEBUG_LOG(m)					((void)0)
#define DEBUG_ASSERTLOG(c, m)			((void)0)
#define DEBUG_CRASH(m)					((void)0)
#define DEBUG_ASSERTCRASH(c, m)		((void)0)
#define BEGIN_PROFILE(uniqueid)
#define END_PROFILE(uniqueid, msg, howoftentolog, howoftentoreset)

#endif // __GAMEDEBUG_H
