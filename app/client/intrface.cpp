// Earth & Beyond client (reconstruction): app\client\intrface.cpp. Notes at the end of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members typed (the windows still cast the +0x4C array)
#include "intrface.h"
#include "sclient.h"
#include "gadget.h"
#include "control_keys_manager.h"
#include "unknown_signalbase.h"
#include "..\shared\unknown_debug.h"

#include "interface_static_sort_list.h"	// the view's scene's +0x9C (the asserts' 'ssl')








// the views' scene (slot +0x50: uiview.h's ViewUnknown1CClass), as far as its +0x9C
class InterfaceSceneClass  // NAME 1: neutral (D10)
{
public:
	char Unknown00[0x9C];
	InterfaceStaticSortListClass *SSL;				// +0x9C (the asserts' 'ssl')  // NAME 2: the asserts'
};

// local_interface_sort.cpp's sort object (8 bytes; constructor 0x6E0E80 over an interface, destructor 0x6E18B0)
class InterfaceSortClass  // NAME 1: neutral (D10)
{
public:
	InterfaceSortClass(InterfaceClass *iface);		// 0x6E0E80  // NAME 1 [iface]: neutral
	~InterfaceSortClass();							// 0x6E18B0

	char Unknown00[8];
};

// The SignalMap (0x4F9740 / 0x4F98D0), as far as the interfaces connect to it
class InterfaceSignalMapClass  // NAME 1: neutral (D10)
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback);			// 0x4F9740 (the client's 'SignalMap::connect')  // NAME 1 [sender, callback]: neutral
	void Remove(void *sender, const SignalBaseClass::CallbackClass &callback);	// 0x4F98D0 (the client's 'SignalMap::disconnect')  // NAME 1 [sender, callback]: neutral
	template <class SENDER, class OBJECT, class EVENT>
	void Add(SENDER *sender, OBJECT *object, void (OBJECT::*function)(EVENT *, SENDER *))	// NAME 1 [sender, object, function]: neutral
	{
		Add(sender, new SignalBaseClass::MemberCallback<SENDER, OBJECT, void (OBJECT::*)(EVENT *, SENDER *), EVENT>(object, function));
	}
	template <class SENDER, class OBJECT, class EVENT>
	void Remove(SENDER *sender, OBJECT *object, void (OBJECT::*function)(EVENT *, SENDER *), const EVENT &event)	// 0x541A50 (KeysSavedEvent)  // NAME 1 [sender, object, function, event]: neutral
	{
		Remove(sender, SignalBaseClass::MemberCallback<SENDER, OBJECT, void (OBJECT::*)(EVENT *, SENDER *), EVENT>(object, function));
	}
};












// NOT EXACT: register (shape: 0.62; VC6 keeps the constant 1 of the two reference counts in edi and the view in ebp, the client the view in edi)
InterfaceClass::InterfaceClass(UIViewClass *view, int unknown) :
	Client(NULL),
	Unknown44(unknown),
	View(view),
	Unknown70(NULL),
	Unknown74(0),
	Unknown78(this),
	Active(false),
	PartialDeactivatedGadget(NULL)
{
	if (Unknown44) {
		((RefCountClass *)Unknown44)->Add_Ref();
	}
	Unknown70 = new InterfaceKeyHandlerClass(this);
	if (View) {
		InterfaceSceneClass *scene = (InterfaceSceneClass *)View->Unknown_50();
		InterfaceStaticSortListClass *ssl = scene ? scene->SSL : NULL;
		G_ASSERT(ssl);
		if (ssl) {
			ssl->Add_Interface(this);
		}
	}
	if (view && view->Unknown_20()) {
		Client = (LineDisplayClientClass *)((Unknown65B810ViewClass *)view->Unknown_20())->Client;
	} else {
		Client = NULL;
	}
	((InterfaceSignalMapClass &)GadgetClass::Get_Signal_Map()).Add((void *)this, this, &InterfaceClass::Unknown_5418C0);
}

void InterfaceClass::Unknown_60()
{
}

InterfaceClass::InterfaceClass(LineDisplayClientClass *client) :
	Client(client),
	Unknown44(0),
	View(NULL),
	Unknown70(NULL),
	Unknown74(0),
	Unknown78(this),
	Active(false),
	PartialDeactivatedGadget(NULL)
{
	Unknown70 = new InterfaceKeyHandlerClass(this);
	((InterfaceSignalMapClass &)GadgetClass::Get_Signal_Map()).Add((void *)this, this, &InterfaceClass::Unknown_5418C0);
}

























InterfaceClass::~InterfaceClass()
{
	G_ASSERT(!View);
	if (Active) {
		Unknown_5C();
	}
	Unknown70->Release_Ref();
	if (Unknown44) {
		((RefCountClass *)Unknown44)->Release_Ref();
		Unknown44 = 0;
	}
	((InterfaceSignalMapClass &)GadgetClass::Get_Signal_Map()).Remove((void *)this, this, &InterfaceClass::Unknown_5418C0, GadgetGroupToFrontEvent(NULL));
}























void InterfaceClass::Unknown_58()
{
	G_ASSERT(!PartialDeactivatedGadget);
	if (!Active && View && Unknown44) {
		InterfaceSortClass sort(this);
		InterfaceSceneClass *scene = View ? (InterfaceSceneClass *)View->Unknown_50() : NULL;
		InterfaceStaticSortListClass *issl = scene ? scene->SSL : NULL;









		FAIL_IF(!issl) {
			return;
		}
		issl->Interface_To_Front(this);
		Unknown58.Unknown_686E90();
		Unknown64.Unknown_6871D0(View->Get_Unknown04(), Unknown70, Unknown78);
		ControlKeysManager *manager = ControlKeysManager::Get((ControlKeysSClientClass *)(Client ? ((ClientClass *)Client)->Unknown_18() : 0));
		Unknown_70((ControlKeysManagerClass *)manager);
		if (manager) {
			((InterfaceSignalMapClass &)ControlKeysManager::Get_Signal_Map()).Add(manager, this, &InterfaceClass::Unknown_541830);
		}
		Unknown4C.Unknown_721F40(View->Get_Unknown04(), Unknown70, this);
		Active = true;
	}
}


















void InterfaceClass::Unknown_5C()
{
	G_ASSERT(!PartialDeactivatedGadget);
	if (Active) {
		ControlKeysManager *manager = ControlKeysManager::Get((ControlKeysSClientClass *)(Client ? ((ClientClass *)Client)->Unknown_18() : 0));
		Unknown4C.Unknown_721F80(View->Get_Unknown04(), this);
		if (manager) {
			((InterfaceSignalMapClass &)ControlKeysManager::Get_Signal_Map()).Remove(manager, this, &InterfaceClass::Unknown_541830, KeysSavedEvent());
		}
		Unknown_74((ControlKeysManagerClass *)manager);
		Unknown64.Unknown_6872A0(View->Get_Unknown04(), Unknown78);
		Unknown58.Unknown_686EE0();
		Active = false;
	}
}



























// (arg is a bool: the client tests it as a byte; intrface.h and its eight overriders say so, P-UI-3)
void InterfaceClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (view == View) {
		if (View && arg) {
			Unknown_58();
		}
		return;
	}
	if (Active) {
		Unknown_5C();
	}
	RefCountPtr<InterfaceClass> keep = Create_Peek(this);
	if (View) {
		InterfaceSceneClass *scene = (InterfaceSceneClass *)View->Unknown_50();
		InterfaceStaticSortListClass *ssl = scene ? scene->SSL : NULL;
		G_ASSERT(ssl);
		if (ssl) ssl->Remove_Interface(this);
		View->Unknown_6B4990(this);
	}
	View = view;
	if (View) {
		InterfaceSceneClass *scene = (InterfaceSceneClass *)View->Unknown_50();
		InterfaceStaticSortListClass *ssl = scene ? scene->SSL : NULL;

		G_ASSERT(ssl);
		if (ssl) {
			ssl->Add_Interface(this);
		}
		if (!Client && View->Unknown_20()) {
			Client = (LineDisplayClientClass *)((Unknown65B810ViewClass *)View->Unknown_20())->Get_Client();
		}
		REF_PTR_SET(((RefCountClass *&)Unknown44), ((RefCountClass *)View->Unknown_50()));
		Unknown64.Unknown_687360(View);
		Unknown58.Unknown_686F10(View->Unknown_50());
		View->Unknown_6B4940(this);
		if (arg) {
			Unknown_58();
		}
	} else {
		REF_PTR_RELEASE(((RefCountClass *&)Unknown44));
	}
}









void InterfaceClass::Unknown_80()
{
	FAIL_IF(PartialDeactivatedGadget) return;
	FAIL_IF(!Active) return;
	PartialDeactivatedGadget = new InterfaceDeactivationGadgetClass(0, 0, 0, 0, 0);
	PartialDeactivatedGadget->Unknown_54(View->Unknown2C, Unknown78);
	Unknown64.Unknown_6872A0(View->Get_Unknown04(), Unknown78);
	Unknown58.Unknown_686EE0();
}


// NOT EXACT: register (0.77; the view's +0x2C in eax and the table in edx in the client, the other way round here)
void InterfaceClass::Unknown_84()
{
	FAIL_IF(!PartialDeactivatedGadget) return;
	FAIL_IF(!Active) return;
	InterfaceSortClass sort(this);
	Unknown58.Unknown_686E90();
	Unknown64.Unknown_6871D0(View->Get_Unknown04(), Unknown70, Unknown78);
	PartialDeactivatedGadget->Unknown_58(View->Unknown2C);
	GadgetClass::Destroy(PartialDeactivatedGadget);
	PartialDeactivatedGadget = NULL;
}

void InterfaceClass::Unknown_5417D0(InterfaceClass *parent)
{
	if (parent) {
		Unknown78 = parent->Unknown78;
		return;
	}
	Unknown78 = this;
}

InterfaceStaticSortListClass *InterfaceClass::Unknown_541800()
{
	if (!View) {
		return(NULL);
	}
	InterfaceSceneClass *scene = (InterfaceSceneClass *)View->Unknown_50();
	if (!scene) {
		return(NULL);
	}
	return(scene->SSL);
}





































void InterfaceClass::Unknown_541830(KeysSavedEvent *event, ControlKeysManager *key_manager)
{
	FAIL_IF(!key_manager) return;
	Unknown4C.Unknown_721F80(View->Get_Unknown04(), this);
	Unknown_74((ControlKeysManagerClass *)key_manager);
	Unknown_70((ControlKeysManagerClass *)key_manager);
	Unknown4C.Unknown_721F40(View->Get_Unknown04(), Unknown70, this);
}












void InterfaceClass::Unknown_5418C0(GadgetGroupToFrontEvent *event, void *sender)
{
	if (sender != this) {
		return;
	}
	InterfaceSceneClass *scene = View ? (InterfaceSceneClass *)View->Unknown_50() : NULL;
	InterfaceStaticSortListClass *ssl = scene ? scene->SSL : NULL;
	FAIL_IF(!ssl) return;
	ssl->Interface_To_Front(this);
}














void InterfaceClass::Unknown_541930()
{
	InterfaceSceneClass *scene = View ? (InterfaceSceneClass *)View->Unknown_50() : NULL;
	InterfaceStaticSortListClass *ssl = scene ? scene->SSL : NULL;
	FAIL_IF(!ssl) return;
	ssl->Interface_To_Front(this);
}


/*
**	Reconstruction notes
**
**	The client's window (app-functions.json): the constructors 0x540A90 (the view and its scene; the asserts'
**	line 84) and 0x540D30 (a client, no view), the destructor 0x540E90 (line 141), the slots +0x58 activate
**	(0x541010, lines 177 / 191), +0x5C deactivate (0x541210, 226), +0x60 (0x540C70, empty), +0x7C onto a view
**	(0x5412F0, 282 / 291), +0x80 / +0x84 the partial deactivation (0x541550 / 0x541690, 320-333), then 0x5417D0,
**	0x541800, the callbacks 0x541830 (KeysSavedEvent, 401) and 0x5418C0 (GadgetGroupToFrontEvent, 426), 0x541930
**	(447). Compiled with them: the key handler's slot 0x540C90 and its destructors 0x540CB0 / 0x540CE0, the
**	deleting destructor 0x540D00, the two MemberCallback instances 0x541AD0-0x541D90, Create_Peek 0x5419E0, the
**	RefCountPtr destructor 0x541A10, the template disconnect 0x541A50 and the adjustor thunks 0x541DC0 / 0x541DD0.
**	0x5411F0 (a jmp to 0x6E18B0) and 0x541980 / 0x5419B0 (local_interface_sort.cpp's constructor over an
**	interface or a RefCountPtr) are not rebuilt.
**
**	The class: the callbacks' RTTI names it InterfaceMemberClass (SignalBaseClass::MemberCallback<void,
**	InterfaceMemberClass, ..., GadgetGroupToFrontEvent> and <ControlKeysManager, InterfaceMemberClass, ...,
**	KeysSavedEvent>, both over `this`): kept as InterfaceClass here (intrface.h's, used by every window) until the
**	rename is decided.
*/
