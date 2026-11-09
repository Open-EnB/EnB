// Earth & Beyond client (reconstruction): app\client\unknown_7e1e40.cpp. PROVISIONAL file name (D137).
#include "unknown_7e1e40.h"

Sub14CUnknown18Class::Sub14CUnknown18Class()
	: Unknown18(NULL), Unknown1C(NULL), Unknown20(NULL), Unknown24(NULL), Unknown28(NULL), Unknown2C(NULL),
	  Unknown30(0), Unknown34(false), Unknown35(false), Unknown38(0), Unknown3C(0), Unknown44(NULL)
{
}

STDMETHODIMP_(ULONG) Sub14CUnknown18Class::AddRef()
{
	return Unknown14++;
}

STDMETHODIMP_(ULONG) Sub14CUnknown18Class::Release()
{
	if (--Unknown14 == 0) {
		delete this;
		return 0;
	}
	return Unknown14;
}

// NOT EXACT: the client's has an EH frame (states 1, 0 around the _bstr_t members' destruction); /GX gives none here
// (/EHa gives it, and an EH frame in the constructor the client lacks)
Sub14CUnknown18Class::~Sub14CUnknown18Class()
{
}

bool Sub14CUnknown18Class::Unknown_7E2080()
{
	Unknown_7E21F0();
	if (Unknown10.length() != 0) {
		Unknown30 = Unknown10.length();
		Unknown2C = GlobalAlloc(GPTR, Unknown30 + 1);
		if (Unknown2C == NULL) {
			return false;
		}
		lstrcpyA((char *)Unknown2C, (const char *)Unknown10);
	}
	HRESULT hr = CreateURLMoniker(NULL, (const wchar_t *)Unknown0C, &Unknown20);
	if (SUCCEEDED(hr)) {
		hr = CreateBindCtx(0, &Unknown24);
		if (SUCCEEDED(hr)) {
			hr = RegisterBindStatusCallback(Unknown24, this, NULL, 0);
			if (SUCCEEDED(hr)) {
				BIND_OPTS options;
				options.cbStruct = sizeof(BIND_OPTS);
				Unknown24->GetBindOptions(&options);
				options.dwTickCountDeadline = GetTickCount() + 10000;
				hr = Unknown24->SetBindOptions(&options);
			}
		}
	}
	return SUCCEEDED(hr);
}

void Sub14CUnknown18Class::Unknown_7E21F0()
{
	if (Unknown24 != NULL) {
		RevokeBindStatusCallback(Unknown24, this);
	}
	if (Unknown18 != NULL) {
		Unknown18->Release();
		Unknown18 = NULL;
	}
	if (Unknown1C != NULL) {
		Unknown1C->Release();
		Unknown1C = NULL;
	}
	if (Unknown24 != NULL) {
		Unknown24->Release();
		Unknown24 = NULL;
	}
	if (Unknown20 != NULL) {
		Unknown20->Release();
		Unknown20 = NULL;
	}
	if (Unknown2C != NULL) {
		GlobalFree(Unknown2C);
		Unknown2C = NULL;
	}
	Unknown3C = 0;
	Unknown40 = "";
	Unknown38 = 0;
	Unknown34 = false;
}

bool Sub14CUnknown18Class::Unknown_7E23B0(int verb, const char *url, const char *query, bool async)
{
	Unknown35 = async;
	Unknown08 = verb;
	Unknown0C = url;
	Unknown10 = query;
	Unknown_7E2080();
	return SUCCEEDED(Unknown20->BindToStorage(Unknown24, NULL, IID_IStream, (void **)&Unknown18));
}

bool Sub14CUnknown18Class::Unknown_7E2430(int verb, const char *url, const char *query, bool async, RegistrationCallbackClass *callback)
{
	Unknown44 = callback;
	Unknown35 = async;
	Unknown08 = verb;
	Unknown0C = url;
	Unknown10 = query;
	Unknown_7E2080();
	return SUCCEEDED(Unknown20->BindToStorage(Unknown24, NULL, IID_IStream, (void **)&Unknown18));
}

STDMETHODIMP Sub14CUnknown18Class::QueryInterface(REFIID riid, void **ppv)
{
	*ppv = NULL;
	if (riid == IID_IUnknown || riid == IID_IBindStatusCallback) {
		*ppv = (IBindStatusCallback *)this;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IHttpNegotiate) {
		*ppv = (IHttpNegotiate *)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP Sub14CUnknown18Class::OnStartBinding(DWORD reserved, IBinding *binding)
{
	Unknown_7E3000();
	if (Unknown28 != NULL) {
		Unknown28->Release();
	}
	Unknown28 = binding;
	if (Unknown28 != NULL) {
		Unknown28->AddRef();
	}
	return S_OK;
}

STDMETHODIMP Sub14CUnknown18Class::GetPriority(LONG *priority)
{
	return E_NOTIMPL;
}

STDMETHODIMP Sub14CUnknown18Class::OnLowResource(DWORD reserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP Sub14CUnknown18Class::OnProgress(ULONG progress, ULONG progress_max, ULONG status, LPCWSTR status_text)
{
	char text[255] = "";
	Unknown_7E3000();
	lstrcpyA(text, Unknown_7E2850(status));
	if (status == BINDSTATUS_REDIRECTING) {
		Unknown34 = true;
	}
	Unknown_7E30B0(progress, progress_max, status);
	return S_OK;
}

const char *Sub14CUnknown18Class::Unknown_7E2850(int status)
{
	switch (status) {
		case BINDSTATUS_REDIRECTING: return "Server redirecting client...";
		case BINDSTATUS_FINDINGRESOURCE: return "Finding resource...";
		case BINDSTATUS_CONNECTING: return "Connecting...";
		case BINDSTATUS_BEGINDOWNLOADDATA: return "Beginning to download data...";
		case BINDSTATUS_DOWNLOADINGDATA: return "Downloading data...";
		case BINDSTATUS_ENDDOWNLOADDATA: return "Ending data download...";
		case BINDSTATUS_BEGINDOWNLOADCOMPONENTS: return "Beginning to download components...";
		case BINDSTATUS_INSTALLINGCOMPONENTS: return "Installing components...";
		case BINDSTATUS_ENDDOWNLOADCOMPONENTS: return "Ending component download...";
		case BINDSTATUS_USINGCACHEDCOPY: return "Using cached copy...";
		case BINDSTATUS_SENDINGREQUEST: return "Sending request...";
		case BINDSTATUS_CLASSIDAVAILABLE: return "CLSID available...";
		case BINDSTATUS_MIMETYPEAVAILABLE: return "MIME type available...";
		case BINDSTATUS_CACHEFILENAMEAVAILABLE: return "Cache file name available...";
		default: return "Please Wait...";
	}
}

STDMETHODIMP Sub14CUnknown18Class::OnStopBinding(HRESULT result, LPCWSTR error)
{
	Unknown_7E3000();
	if (result == S_OK && Unknown1C != NULL) {
		LARGE_INTEGER zero;
		zero.QuadPart = 0;
		Unknown1C->Seek(zero, STREAM_SEEK_SET, NULL);
		Unknown_7E3090();
	} else {
		Unknown_7E30E0(result, error);
	}
	Unknown_7E21F0();
	return S_OK;
}

STDMETHODIMP Sub14CUnknown18Class::GetBindInfo(DWORD *flags, BINDINFO *info)
{
	Unknown_7E3000();
	if (Unknown34 && Unknown08 == BINDVERB_POST) {
		Unknown08 = BINDVERB_GET;
	}
	if (Unknown35) {
		*flags = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA;
	} else {
		*flags = BINDF_PULLDATA;
	}
	*flags |= BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;
	info->cbSize = sizeof(BINDINFO);
	info->dwBindVerb = Unknown08;
	info->szExtraInfo = NULL;
	memset(&info->stgmedData, 0, sizeof(STGMEDIUM));
	info->grfBindInfoF = 0;
	info->szCustomVerb = NULL;
	switch (Unknown08) {
		case BINDVERB_GET:
			break;
		case BINDVERB_POST:
			if (Unknown2C != NULL) {
				info->stgmedData.tymed = TYMED_HGLOBAL;
				info->stgmedData.hGlobal = Unknown2C;
				info->stgmedData.pUnkForRelease = (IBindStatusCallback *)this;
				AddRef();
				info->cbstgmedData = Unknown30;
			}
			break;
		default:
			return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP Sub14CUnknown18Class::OnDataAvailable(DWORD flags, DWORD size, FORMATETC *format, STGMEDIUM *medium)
{
	Unknown_7E3000();
	if ((flags & BSCF_FIRSTDATANOTIFICATION) && Unknown18 == NULL && medium->tymed == TYMED_ISTREAM) {
		Unknown18 = medium->pstm;
		if (Unknown18 != NULL) {
			Unknown18->AddRef();
			CreateStreamOnHGlobal(NULL, TRUE, &Unknown1C);
		}
	}
	HRESULT hr;
	do {
		ULARGE_INTEGER count;
		count.QuadPart = 1;
		hr = Unknown18->CopyTo(Unknown1C, count, NULL, NULL);
	} while (hr == S_OK);
	return S_OK;
}

STDMETHODIMP Sub14CUnknown18Class::OnObjectAvailable(REFIID riid, IUnknown *object)
{
	Unknown_7E3000();
	return E_NOTIMPL;
}

STDMETHODIMP Sub14CUnknown18Class::BeginningTransaction(LPCWSTR url, LPCWSTR headers, DWORD reserved, LPWSTR *additional_headers)
{
	Unknown_7E3000();
	if (additional_headers == NULL) {
		return E_POINTER;
	}
	*additional_headers = NULL;
	if (Unknown08 == BINDVERB_POST && Unknown2C != NULL) {
		wchar_t text[] = L"Content-Type: application/x-www-form-urlencoded\r\n";
		LPWSTR copy = (LPWSTR)CoTaskMemAlloc((wcslen(text) + 1) * sizeof(wchar_t));
		if (copy == NULL) {
			return E_OUTOFMEMORY;
		}
		wcscpy(copy, text);
		*additional_headers = copy;
	}
	return S_OK;
}

STDMETHODIMP Sub14CUnknown18Class::OnResponse(DWORD code, LPCWSTR response_headers, LPCWSTR request_headers, LPWSTR *additional_headers)
{
	if (additional_headers == NULL) {
		return E_POINTER;
	}
	*additional_headers = NULL;
	return S_OK;
}

void Sub14CUnknown18Class::Unknown_7E3000()
{
	MSG msg;
	while (PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		GetMessageA(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

HRESULT Sub14CUnknown18Class::Unknown_7E3090()
{
	if (Unknown44 != NULL) {
		Unknown44->Unknown_5F00C0();
	}
	return S_OK;
}

HRESULT Sub14CUnknown18Class::Unknown_7E30B0(ULONG progress, ULONG progress_max, ULONG status)
{
	Unknown38 = status;
	if (Unknown44 != NULL) {
		Unknown44->Unknown_5F00E0();
	}
	return S_OK;
}

HRESULT Sub14CUnknown18Class::Unknown_7E30E0(HRESULT result, LPCWSTR error)
{
	Unknown3C = result;
	if (error != NULL) {
		Unknown40 = error;
	} else {
		Unknown40 = "";
	}
	if (Unknown44 != NULL) {
		Unknown44->Unknown_5F0100();
	}
	return S_OK;
}

/*
** Reconstruction notes (XR2, 2026-11-09)
**
** The registration reader's download (Sub14CImplClass's member at +0x18): an IBindStatusCallback / IHttpNegotiate
** pair over CreateURLMoniker and BindToStorage, the reply copied into a stream on global memory (+0x1C) that
** unknown_7dfed0.cpp's 0x7DFED0 loads the DOMDocument from; a POST of the query (GetBindInfo's TYMED_HGLOBAL,
** BeginningTransaction's form-urlencoded header). Its own object: Sub14CImplClass's slots 0x2C / 0x34 call
** 0x7E2850 / 0x7E2430 out of line though this object expands both, and its literals (0xBA1034-0xBA11D4) follow
** unknown_7dfed0.obj's GUIDs. No __FILE__ or assert: the name is PROVISIONAL (D137), after its first function.
** 0x7E23B0, 0x7E3000, 0x7E3090, 0x7E30B0 and 0x7E30E0 have no callers: the slots expand them (/Ob2), the
** incremental link keeps them. 0x7E2290 / 0x7E2A80 are _bstr_t's operator= (char / wide) out of line;
** 0x7E3130-0x7E3150 the IHttpNegotiate thunks of QueryInterface / AddRef / Release.
*/
