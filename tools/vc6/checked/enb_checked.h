/*
**	A checked build's debug macros (M2 gate (d)): tools/vc6/build.py force-includes this header into
**	every compile when ENB_CHECKED=1 (with /D DEBUG_CRASHING /D DEBUG_LOGGING), so Zero Hour's
**	wwdebug.h maps WWASSERT, WWASSERT_PRINT, W3D_DIE, WWDEBUG_ERROR, WWDEBUG_SAY and WWDEBUG_WARNING
**	onto these definitions instead of wwdebug/gamedebug.h's release ones (all ((void)0), as the
**	client). Failed asserts and errors reach the WWDebug message handler as WWDEBUG_TYPE_ERROR, the
**	logs as WWDEBUG_TYPE_INFORMATION (vidtest logs both: "[wwdebug N]"). Not part of the
**	reconstruction: the tree's code is unchanged and never built this way for matching.
*/

#ifndef __GAMEDEBUG_H
#define __GAMEDEBUG_H

#define DEBUG_INIT(f)					((void)0)
#define DEBUG_SHUTDOWN()				((void)0)
#ifdef __cplusplus
void WWDebug_Printf(const char * format,...);
void WWDebug_Printf_Error(const char * format,...);
#define DEBUG_LOG(m)					WWDebug_Printf m
#define DEBUG_ASSERTLOG(c, m)			((c) ? (void)0 : WWDebug_Printf m)
#define DEBUG_CRASH(m)					WWDebug_Printf_Error m
#define DEBUG_ASSERTCRASH(c, m)		((c) ? (void)0 : WWDebug_Printf_Error m)
#else	/* C files: the release definitions */
#define DEBUG_LOG(m)					((void)0)
#define DEBUG_ASSERTLOG(c, m)			((void)0)
#define DEBUG_CRASH(m)					((void)0)
#define DEBUG_ASSERTCRASH(c, m)		((void)0)
#endif
#ifdef __cplusplus
// asserts compiled for the first time read private members (DX8Wrapper::_Update_Texture's
// WWASSERT(system->Pool == ...)): open them for this diagnostic build only
#define private public
#endif
#define BEGIN_PROFILE(uniqueid)
#define END_PROFILE(uniqueid, msg, howoftentolog, howoftentoreset)

#endif // __GAMEDEBUG_H
