// Earth & Beyond client (reconstruction): app\client\unknown_7dfed0.cpp. PROVISIONAL file name (D137).
#include "unknown_7dfed0.h"
#include <stdlib.h>

void Sub14CImplClass::Unknown_5F00C0()
{
	IUnknown *stream = Unknown18.Get_Unknown1C();
	if (stream != NULL) {
		Unknown10->load(_variant_t(stream));
	}
	if (Unknown08 != NULL) {
		Unknown08->Unknown_5F00C0();
	}
	stream->Release();
}

void Sub14CImplClass::Unknown_5F00E0()
{
	if (Unknown08 != NULL) {
		Unknown08->Unknown_5F00E0();
	}
}

void Sub14CImplClass::Unknown_5F0100()
{
	if (Unknown08 != NULL) {
		Unknown08->Unknown_5F0100();
	}
}

void Sub14CImplClass::Slot1()
{
	CoInitialize(NULL);
	Unknown0C = Unknown10.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (SUCCEEDED(Unknown0C)) {
		Unknown10->async = VARIANT_FALSE;
		Unknown10->validateOnParse = VARIANT_FALSE;
	}
}

void Sub14CImplClass::Slot2()
{
	Unknown10 = NULL;
	CoUninitialize();
}

std::string Sub14CImplClass::Slot3(const char *path, int index, const char *name)
{
	_bstr_t value("");
	MSXML2::IXMLDOMNodeListPtr list = Unknown10->selectNodes(_bstr_t(path));
	if (list) {
		MSXML2::IXMLDOMNodePtr node = list->Getitem(index);
		if (node) {
			if (name != NULL) {
				if (node->GetnodeType() == MSXML2::NODE_ELEMENT) {
					MSXML2::IXMLDOMNodePtr child = node->selectSingleNode(_bstr_t(name));
					if (child) {
						value = child->Gettext();
					}
				}
			} else {
				value = node->Gettext();
			}
		}
	}
	return std::string((const char *)value);
}

int Sub14CImplClass::Slot4(const char *path, int index, const char *name)
{
	return atoi(Slot3(path, index, name).c_str());
}

float Sub14CImplClass::Slot5(const char *path, int index, const char *name)
{
	return (float)atof(Slot3(path, index, name).c_str());
}

int Sub14CImplClass::Slot6(const char *path)
{
	MSXML2::IXMLDOMNodeListPtr list = Unknown10->selectNodes(_bstr_t(path));
	int count = 0;
	if (list != NULL) {
		count = list->Getlength();
	}
	return count;
}

std::string Sub14CImplClass::Slot7(const char *path, const char *name, int index, const char *element)
{
	_bstr_t value("");
	MSXML2::IXMLDOMNodeListPtr list = Unknown10->selectNodes(_bstr_t(path));
	if (list) {
		MSXML2::IXMLDOMNodePtr node = list->Getitem(index);
		if (node) {
			if (element != NULL) {
				if (node->GetnodeType() == MSXML2::NODE_ELEMENT) {
					MSXML2::IXMLDOMNodePtr child = node->selectSingleNode(_bstr_t(element));
					if (child) {
						MSXML2::IXMLDOMNamedNodeMapPtr attributes = child->Getattributes();
						MSXML2::IXMLDOMNodePtr attribute = attributes->getNamedItem(_bstr_t(name));
						value = attribute->Gettext();
					}
				}
			} else {
				MSXML2::IXMLDOMNamedNodeMapPtr attributes = node->Getattributes();
				MSXML2::IXMLDOMNodePtr attribute = attributes->getNamedItem(_bstr_t(name));
				value = attribute->Gettext();
			}
		}
	}
	return std::string((const char *)value);
}

int Sub14CImplClass::Slot8(const char *path, const char *name, int index, const char *element)
{
	return atoi(Slot7(path, name, index, element).c_str());
}

bool Sub14CImplClass::Slot13(SubsRequestClass *request, int callback)
{
	Slot14();
	Unknown08 = (RegistrationCallbackClass *)callback;
	Unknown18.Unknown_7E2430(1, request->Url.c_str(), request->Query.c_str(), request->Unknown04, this);
	return true;
}

std::string Sub14CImplClass::Slot15()
{
	_bstr_t xml;
	xml = Unknown10->Getxml();
	return std::string((const char *)xml);
}

std::string Sub14CImplClass::Slot16()
{
	std::string name = "";
	MSXML2::IXMLDOMDocumentTypePtr type = Unknown10->Getdoctype();
	if (type != NULL) {
		name = (const char *)type->Getname();
	}
	return name;
}

int Sub14CImplClass::Slot9()
{
	MSXML2::IXMLDOMParseError *error = NULL;
	long code = 0;
	if (!Unknown10->Getparsed()) {
		if (SUCCEEDED(Unknown10->get_parseError(&error)) && error != NULL) {
			if (FAILED(error->get_errorCode(&code))) {
				code = -1;
			}
			error->Release();
		}
	}
	return code;
}

void Sub14CImplClass::Slot14()
{
	_bstr_t empty("");
	Unknown10->loadXML(empty);
}

int Sub14CImplClass::Slot11()
{
	return Unknown18.Unknown38;
}

const char *Sub14CImplClass::Slot12(int arg)
{
	return Unknown18.Unknown_7E2850(Unknown18.Unknown38);
}

/*
** Reconstruction notes (XR2, 2026-11-09)
**
** The registration screen's reader (ui_registration.h's Sub14CImplClass at +0x14C): MSXML's DOMDocument
** (CLSID 2933BF90, the IIDs 2933BF80 / 81 / 82 / 83 / 8B in .rdata 0xBA0F78-0xBA1008) through #import's smart pointers
** and wrappers: msxml3.tlh / msxml3.tli, VC6's #import output, stand in for the client's #import (its type library,
** msxml.dll or msxml3.dll, is not known; the interfaces' slots used here are the same in both). No __FILE__ or assert:
** the name is PROVISIONAL (D137), after its first function. Its out-of-line wrapper copies (0x7E04C0-0x7E1DF0: _bstr_t's
** reference counting, the selectNodes / selectSingleNode / Getitem / Gettext / Getattributes / getNamedItem /
** Getxml / Getdoctype / Getname / loadXML wrappers, _com_ptr_t's operator-> and destructors) come from those headers.
** The object goes on with the download callback at +0x18 (Sub14CUnknown18Class, 0x7E1E40-; not rebuilt here).
*/
