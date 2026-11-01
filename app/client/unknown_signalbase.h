// Earth & Beyond client (reconstruction): SignalBaseClass and its member callbacks, as far as the windows'
// gadget-event callbacks use them. The header's name is neutral (no __FILE__ names it; interim placement, D137).
#ifndef UNKNOWN_SIGNALBASE_H  // NAME 1 [unknown_signalbase.h]: neutral header name (D10)
#define UNKNOWN_SIGNALBASE_H

#include <typeinfo.h>

// The client's RTTI names the callbacks SignalBaseClass::MemberCallback<SENDER, OBJECT, FUNC, EVENT> (type names
// '.PBV?$MemberCallback@...@SignalBaseClass@@': typeid(this) in a const member, 0xB7C068, 0xB817B0, about 20 more)
class SignalBaseClass	// NAME 3: the client's RTTI (the scope of MemberCallback)
{
public:
	// The callbacks' base (vtable 0xAEDB58: the deleting destructor 0x51E220, five pure slots)
	class CallbackClass	// NAME 1: neutral (D10)
	{
	public:
		virtual ~CallbackClass() {}
		virtual const type_info &Sender_Type() const = 0;		// +0x04 (typeid(GadgetClass): 0x560270, 0x5AF810)  // NAME 1: what it returns
		virtual const type_info &Event_Type() const = 0;		// +0x08 (typeid(FlashingGadgetEvent): 0x53D130, shared)  // NAME 1: what it returns
		virtual const type_info &Type() const = 0;			// +0x0C (typeid(this): 0x560290, 0x5AF830)  // NAME 1: what it returns
		virtual bool Is_Equal(const CallbackClass &that) const = 0;	// +0x10 (0x5602B0, 0x5AF850)  // NAME 1: what it does
		virtual void Call(void *sender, void *event) = 0;		// +0x14 (0x560330, 0x5AF8D0: the handler gets (event, sender))  // NAME 1: what it does
	};

	// An object and a member function (0x10 bytes: the function pointer is 8, the windows have two bases)
	template <class SENDER, class OBJECT, class FUNC, class EVENT>
	class MemberCallback : public CallbackClass	// NAME 5: the client's RTTI
	{
	public:
		MemberCallback(OBJECT *object, FUNC function) : Object(object), Function(function) {}
		virtual const type_info &Sender_Type() const { return(typeid(SENDER)); }
		virtual const type_info &Event_Type() const { return(typeid(EVENT)); }
		virtual const type_info &Type() const { return(typeid(this)); }
		virtual bool Is_Equal(const CallbackClass &that) const
		{
			if (!(Type() == that.Type())) {
				return(false);
			}
			const MemberCallback &other = (const MemberCallback &)that;
			return(Is_Same(other.Object, other.Function));	// (the other's members in registers: through a call)
		}
		bool Is_Same(OBJECT *object, FUNC function) const { return(Object == object && Function == function); }	// NAME 1: what it does
		virtual void Call(void *sender, void *event) { (Object->*Function)((EVENT *)event, (SENDER *)sender); }

		OBJECT *Object;									// +0x04  // NAME 1: what it holds
		FUNC Function;									// +0x08  // NAME 1: what it holds
	};
};

#endif
