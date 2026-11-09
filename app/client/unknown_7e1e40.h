// Earth & Beyond client (reconstruction): app\client\unknown_7e1e40.h. PROVISIONAL file name (D137): the
// registration reader's download of the reply through an URL moniker (the object 0x7E1E40-0x7E3160 has no
// __FILE__ or assert) (XR2, 2026-11-09).
#ifndef UNKNOWN_7E1E40_H  // NAME 1 [unknown_7e1e40.h]: PROVISIONAL (D137)
#define UNKNOWN_7E1E40_H

#include <urlmon.h>
#include <comdef.h>
#include "unknown_7cef30.h"

// What the implementation holds at +0x18 (0x48 bytes; tables 0xB073C8 / 0xB073B0, 0x7E1E40 / 0x7E1F10): an
// asynchronous URL moniker download, IBindStatusCallback's eleven slots and IHttpNegotiate's five (a POST of the query)
class Sub14CUnknown18Class : public IBindStatusCallback, public IHttpNegotiate  // NAME 1 [Sub14CUnknown18Class]: neutral (D10)
{
public:
	Sub14CUnknown18Class();		// 0x7E1E40
	~Sub14CUnknown18Class();	// 0x7E1F10

	STDMETHOD(QueryInterface)(REFIID riid, void **ppv);	// 0x7E24B0
	STDMETHOD_(ULONG, AddRef)();						// 0x7E1EB0
	STDMETHOD_(ULONG, Release)();						// 0x7E1ED0
	STDMETHOD(OnStartBinding)(DWORD reserved, IBinding *binding);	// 0x7E2580
	STDMETHOD(GetPriority)(LONG *priority);			// 0x7E2640
	STDMETHOD(OnLowResource)(DWORD reserved);		// 0x7E2660
	STDMETHOD(OnProgress)(ULONG progress, ULONG progress_max, ULONG status, LPCWSTR status_text);	// 0x7E2680
	STDMETHOD(OnStopBinding)(HRESULT result, LPCWSTR error);	// 0x7E2960
	STDMETHOD(GetBindInfo)(DWORD *flags, BINDINFO *info);		// 0x7E2BA0
	STDMETHOD(OnDataAvailable)(DWORD flags, DWORD size, FORMATETC *format, STGMEDIUM *medium);	// 0x7E2CE0
	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown *object);	// 0x7E2DF0
	STDMETHOD(BeginningTransaction)(LPCWSTR url, LPCWSTR headers, DWORD reserved, LPWSTR *additional_headers);	// 0x7E2E90
	STDMETHOD(OnResponse)(DWORD code, LPCWSTR response_headers, LPCWSTR request_headers, LPWSTR *additional_headers);	// 0x7E2FD0

	bool Unknown_7E2080();	// 0x7E2080: the moniker and bind context for Unknown0C (Unknown10 as the POST data)  // NAME 1 [Unknown_7E2080]: neutral
	void Unknown_7E21F0();	// 0x7E21F0: releases everything  // NAME 1 [Unknown_7E21F0]: neutral
	bool Unknown_7E23B0(int verb, const char *url, const char *query, bool async);	// 0x7E23B0 (no callers)  // NAME 1 [verb, url, query, async]: GetBindInfo's use
	bool Unknown_7E2430(int verb, const char *url, const char *query, bool async, RegistrationCallbackClass *callback);	// 0x7E2430 (0x7E1290 starts the request with it)  // NAME 1 [callback]: what 0x7E1290 passes
	const char *Unknown_7E2850(int status);	// 0x7E2850 (slot 0x30's text)  // NAME 1 [status]: OnProgress's
	void Unknown_7E3000();	// 0x7E3000 (no callers; every slot expands it): the message pump  // NAME 1 [Unknown_7E3000]: neutral
	HRESULT Unknown_7E3090();	// 0x7E3090 (no callers; OnStopBinding expands it)  // NAME 1 [Unknown_7E3090]: neutral
	HRESULT Unknown_7E30B0(ULONG progress, ULONG progress_max, ULONG status);	// 0x7E30B0 (no callers; OnProgress expands it)  // NAME 1 [progress, progress_max, status]: OnProgress's
	HRESULT Unknown_7E30E0(HRESULT result, LPCWSTR error);	// 0x7E30E0 (no callers; OnStopBinding expands it)  // NAME 1 [result, error]: OnStopBinding's
	IUnknown *Get_Unknown1C() { if (Unknown1C != NULL) Unknown1C->AddRef(); return Unknown1C; }	// inline (0x7DFED0)  // NAME 1 [Get_Unknown1C]: what it reads

	int Unknown08;					// +0x08: the BINDVERB (1: POST, dropped to GET after a redirect)
	_bstr_t Unknown0C;				// +0x0C: the url
	_bstr_t Unknown10;				// +0x10: the query (the POST data)
	ULONG Unknown14;				// +0x14: the reference count
	IStream *Unknown18;				// +0x18: the download's stream (OnDataAvailable)
	IStream *Unknown1C;				// +0x1C: the copy in memory (what 0x7DFED0 loads the document from)
	IMoniker *Unknown20;			// +0x20
	IBindCtx *Unknown24;			// +0x24
	IBinding *Unknown28;			// +0x28 (OnStartBinding)
	HGLOBAL Unknown2C;				// +0x2C: the POST data's copy
	DWORD Unknown30;				// +0x30: its length
	bool Unknown34;					// +0x34: redirected (BINDSTATUS_REDIRECTING)
	bool Unknown35;					// +0x35: asynchronous (BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE)
	int Unknown38;					// +0x38: the last BINDSTATUS (slot 0x2C)
	HRESULT Unknown3C;				// +0x3C: OnStopBinding's result
	_bstr_t Unknown40;				// +0x40: its error text
	RegistrationCallbackClass *Unknown44;	// +0x44: told of progress (slot 2), the end (slot 1) and errors (slot 3)
};

#endif // UNKNOWN_7E1E40_H
