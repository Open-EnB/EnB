// Earth & Beyond client (reconstruction): link-time stubs (D44, docs/milestones.md 3.2)
/*
**	A stub stands for a function the client has and the tree does not yet. Every stub logs its
**	first call: one line to OutputDebugString, stderr and the stub log (the file named by the
**	environment variable ENB_STUB_LOG, else stubs.log in the current directory). Stub_Log_Mark writes a
**	marker line and lets every stub log its next call as a first call again.
*/


#ifndef STUB_LOG_H
#define STUB_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void Stub_Log_First_Call(const char *name, int *seen);
void Stub_Log_Line(const char *format, ...);
__declspec(dllexport) void Stub_Log_Mark(const char *name);

#ifdef __cplusplus
}
#endif

#define STUB_CALLED(name)	do { static int _stub_seen = 0; if (!_stub_seen) Stub_Log_First_Call(name, &_stub_seen); } while (0)

#endif // STUB_LOG_H
