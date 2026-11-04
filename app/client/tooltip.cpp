// Earth & Beyond client (reconstruction): app\Client\tooltip.cpp (__FILE__ 0xB91938). Notes at the end of the file.
#include "tooltip.h"
#include "sclient.h"
#include "client.h"
#include "dynamic_window_class.h"
#include "dynamic_window_render_object.h"
// (fontdatabase.h's 0xBE67B8 clashes with tsk_game.h's: renamed while it is included, as unknown_msgbox.cpp does)
#define Unknown_BE67B8 Unknown_BE67B8_FontDatabase
#include "fontmanager.h"
#include "..\shared\unknown_font_split_string.h"
#undef Unknown_BE67B8
#include "gadget.h"
#include "local_interface_sort.h"
#include "unknown_signalbase.h"
#include "option.h"
#include "tasks\tsk_login.h"
#include "..\packets\pkgadget.h"
#include "..\shared\unknown_debug.h"
#include "aabox.h"

unsigned int Unknown_Get_Ticks();				// 0x4A3B40, the aux-data clock
bool Unknown_42EC60(int button, int arg);		// 0x42EC60 (input.cpp: a button or key down)  // NAME 1 [button, arg]: neutral

// 0x51C800's element: unknown_51c800.h (the line count kept for the FAIL_IFs below)
#include "unknown_51c800.h"





// The SignalMap (0x4F9740 / 0x4F98D0), as intrface.cpp's InterfaceSignalMapClass
class TooltipSignalMapClass : public SignalMap  // NAME 1: neutral (D10)
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback) { connect(sender, callback); }
	void Remove(void *sender, const SignalBaseClass::CallbackClass &callback) { disconnect(sender, callback); }
	template <class SENDER, class OBJECT, class EVENT>
	void Add(SENDER *sender, OBJECT *object, void (OBJECT::*function)(EVENT *, SENDER *))
	{
		Add(sender, new SignalBaseClass::MemberCallback<SENDER, OBJECT, void (OBJECT::*)(EVENT *, SENDER *), EVENT>(object, function));
	}
	template <class SENDER, class OBJECT, class EVENT>
	void Remove(SENDER *sender, OBJECT *object, void (OBJECT::*function)(EVENT *, SENDER *), const EVENT &event)
	{
		Remove(sender, SignalBaseClass::MemberCallback<SENDER, OBJECT, void (OBJECT::*)(EVENT *, SENDER *), EVENT>(object, function));
	}
};

inline void UIViewUnknown30Class::Unknown_6ABC10()
{
	if (Unknown0C) {
		Window->Remove_All(false);
		Window->Remove_From_Scene();
		if (CurGadget) {
			CurGadget->Unknown2C_08 = false;
		}
		Unknown0C = false;
		Unknown0D = false;
	}
}


UIViewUnknown30Class::UIViewUnknown30Class(UIViewClass *view) :
	View(view),
	CurGadget(NULL),
	Unknown08(0),
	Unknown0C(false),
	Unknown0D(false),
	Unknown0E(false),
	Unknown0F(false),
	Hides(0),
	Window(),
	Unknown18(0.0f, 0.0f)
{
	memset(Lines, 0, sizeof(Lines));
	Window = Create_NEW(new DynamicWindowClass("TOOLTIPS", MAX_LINES));
	Window->Set_Level(2);
	for (int i = 0; i < MAX_LINES; i++) {
		Lines[i] = new FontSys("TOOLTIP_FONT", 0, NULL);
	}
}


UIViewUnknown30Class::~UIViewUnknown30Class()
{
	Unknown_6AB930(NULL);
	for (int i = 0; i < MAX_LINES; i++) {
		REF_PTR_RELEASE(Lines[i]);
	}
	Window = NULL;
}

// NOT EXACT: register (0.80): the view's loads for the second 0x65F5F0 call and the sort object go to eax / ecx the other way round

void UIViewUnknown30Class::Unknown_6AB5B0()
{
	if (Hides != 0 || View == NULL || View->Unknown2C == NULL) {
		return;
	}
	GadgetClass *gadget = GadgetClass::Unknown_65F5F0(View->Unknown2C, false, true);
	if (gadget || (gadget = GadgetClass::Unknown_65F5F0(View->Unknown2C, false, false)) != NULL) {
		const char *text = Unknown0E ? gadget->Unknown24 : gadget->Get_Unknown14();
		if (text == NULL) {
			gadget = NULL;
		}
	}
	if (gadget != CurGadget) {
		Unknown_6AB930(gadget);
		return;
	}
	if (CurGadget == NULL) {
		return;
	}
	if (!Unknown0D && (Unknown_BE5F24 == gadget || Unknown_BE5F28 == gadget)) {
		Unknown_6ABC10();
		Unknown08 = Unknown_Get_Ticks();
		int delay = (gadget->Is_Unknown2C_06() ? 30 : 1200) * 2;
		if (!Unknown0F) {
			delay = OptionClass::Instance->Get_Float("Interface_Tooltip_Delay") * delay * 0.01f;
		}
		Unknown08 += delay;
	}
	if (Unknown0C) {
		CurGadget->Unknown_20(0);
		if (CurGadget->Is_Unknown2C_07()) {
			Unknown_6ABC10();
			Unknown_6ABC60();
		} else {
			Unknown_6ABE90();
		}
		InterfaceSortClass sort(View, true);
		if (Window->Peek_Scene() == NULL) {
			Window->Add_To_Scene((SceneClass *)View->Unknown_50());
		}
	} else if (Unknown08 <= Unknown_Get_Ticks()) {
		Unknown_6ABC60();
	}
	CurGadget->Unknown2C_07 = false;
}







































bool UIViewUnknown30Class::Unknown_6AB850()
{
	FAIL_IF(!Hides) {
		return(false);
	}
	return(--Hides == 0);
}


bool UIViewUnknown30Class::Unknown_6AB890()
{
	Hides++;
	Unknown_6AB930(NULL);
	return(Hides == 1);
}


void UIViewUnknown30Class::Unknown_6AB8C0()
{
	if (CurGadget) {
		Unknown08 = Unknown_Get_Ticks();
		Unknown0D = true;
	}
}





void UIViewUnknown30Class::Unknown_6AB8F0(DestroyGadgetEvent *event, GadgetClass *gadget)
{
	FAIL_IF(CurGadget != gadget) {
		return;
	}
	Unknown_6AB930(NULL);
}

// NOT EXACT: inline-budget (0.75): the client calls the DestroyGadgetEvent callback's constructor 0x6AC050 out of line for the disconnect (inlined here) and loads the gadget's name before the avatar id

void UIViewUnknown30Class::Unknown_6AB930(GadgetClass *gadget)
{
	if (CurGadget) {
		if (Unknown0E) {
			UIViewClass *view = View->Unknown_20();
			if (view && ((Unknown65B810ViewClass *)view)->Client) {
				SClient *client = ((Unknown65B810ViewClass *)view)->Client;
				client->Send_Packet(&Packet_0068Class(client->AvatarID, 0x10, CurGadget->Unknown24));

			}
		}
		Unknown_6ABC10();
		((TooltipSignalMapClass &)GadgetClass::Get_Signal_Map()).Remove(CurGadget, this, &UIViewUnknown30Class::Unknown_6AB8F0, DestroyGadgetEvent());
	}
	Unknown0D = false;
	Unknown0E = false;
	CurGadget = gadget;
	if (gadget) {
		gadget->Unknown_20(1);
		LoginUnknown18Class *mouse = (LoginUnknown18Class *)View->Unknown_44();
		if (mouse) {
			Vector2 pos(mouse->Unknown14, mouse->Unknown18);
			if (Unknown18 != pos) {
				Unknown18 = pos;
				CurGadget = NULL;
				Unknown08 = 0;
				return;
			}
		}
		Unknown08 = Unknown_Get_Ticks();
		int delay = gadget->Is_Unknown2C_06() ? 30 : 1200;
		if (!Unknown0F) {
			delay = OptionClass::Instance->Get_Float("Interface_Tooltip_Delay") * delay * 0.01f;
		}
		Unknown08 += delay;
		((TooltipSignalMapClass &)GadgetClass::Get_Signal_Map()).Add(CurGadget, this, &UIViewUnknown30Class::Unknown_6AB8F0);
		if (Unknown_42EC60(0x12, 0)) {
			if (CurGadget->Unknown24) {
				UIViewClass *view = View->Unknown_20();
				if (view && ((Unknown65B810ViewClass *)view)->Client) {
					Unknown0E = ((Unknown65B810ViewClass *)view)->Client->DebugMode;
				}
			}
			Unknown_6AB8C0();
		}
	} else {
		Unknown08 = 0;
	}
}

// NOT EXACT: shape (0.48): the client keeps the split loop whole (width from the stack, 'lines = MAX_LINES' on the latch); VC6 peels the first pass here















































void UIViewUnknown30Class::Unknown_6ABC60()
{
	if (Unknown0C || CurGadget == NULL || View == NULL) {
		return;
	}
	FontSplitStringClass split(*(RefCountPtr<Font32Class> *)&Lines[0]->Font);
	int width = 100;
	const char *text = Unknown0E ? CurGadget->Unknown24 : CurGadget->Get_Unknown14();
	int lines;
	int tries = 0;
	do {
		split.Split(text, (float)width, 0);
		lines = split.Count();
		tries++;
		if (lines <= tries || split.Get_New_Lines() + 1 >= lines) {
			FAIL_IF(lines > MAX_LINES) {
				lines = MAX_LINES;
			}
			goto fits;
		}
		if (width < 600) {
			width += 50;
		}
	} while (tries < MAX_LINES);
	lines = MAX_LINES;
fits:
	for (int i = 0; i < lines; i++) {
		Lines[i]->Change_Text(split.Get(i), 2);
		Lines[i]->Unknown_6CE910(!CurGadget->Unknown_60());
		Unknown51C800Class *element = new Unknown51C800Class(Lines[i], 6);
		Window->Add_Object(element, i);
		element->Release_Ref();
	}
	Unknown_6ABE90();
	Unknown0C = true;
	if (CurGadget) {
		CurGadget->Unknown2C_08 = true;
	}
}


void UIViewUnknown30Class::Unknown_6ABE90()
{
	LoginUnknown18Class *mouse = (LoginUnknown18Class *)View->Unknown_44();
	if (mouse) {
		Vector2 coords;
		View->Unknown_65B650(Vector2(mouse->Unknown14, mouse->Unknown18), &coords);
		Vector2 size = Window->Get_Min_Size();
		Window->Layout_Size(size);
		coords.X = size.X * 0.5f + coords.X;
		coords.Y = size.Y * 0.5f + coords.Y;
		coords.X -= 3.0f;
		coords.Y -= 3.0f;
		Window->Set_Center(coords);
		return;
	}
	if (Window->Peek_Scene() == NULL) {
		AABoxClass box;
		CurGadget->Unknown_64(box);
		Vector2 size = Window->Get_Min_Size();
		Window->Layout_Size(size);
		Vector2 center(box.Center.X, size.Y * 0.5f + box.Center.Y + box.Extent.Y);
		Window->Set_Center(center);
	}
}

/*
**	Reconstruction notes
**
**	The file: tooltip.cpp's __FILE__ (0xB91938) is cited by 0x6AB850 ('!Hides', line 181), 0x6AB8F0
**	('CurGadget != gadget', line 210) and 0x6ABC60 ('lines > MAX_LINES', line 331); the constructor
**	0x6AB320 and the destructor 0x6AB4D0 come before them, with the EH handlers 0xAC14A9 / 0xAC14CB in
**	the same block as 0x6AB930's and 0x6ABC60's (textgadg.cpp's end at 0xAC13xx).
**	Every frame (0x6AB5B0) the gadget under the pointer (gadget.cpp's 0x65F5F0) with a tooltip text
**	(its +0x14, or its name +0x24 in the debug mode) becomes CurGadget (0x6AB930: the DestroyGadgetEvent
**	callback 0x6AB8F0 connected, the delay 1200 ms (30 with the gadget's bit 6) scaled by the option
**	'Interface_Tooltip_Delay' in percent); after the delay 0x6ABC60 splits the text into up to 20 lines
**	(widths 100 to 600 by 50) and 0x6ABE90 places the window at the pointer. 0x6ABBF0 is Packet_0068Class's
**	inline destructor (0x6AB930 sends it in the debug mode), 0x6ABC10 the inline hide.
*/
