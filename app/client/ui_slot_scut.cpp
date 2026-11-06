// Earth & Beyond client (reconstruction): app\Client\ui_slot_scut.cpp. Notes at the end of the file.
#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include "sclient.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ref_ptr.h"
#include "ui_slot_scut.h"

// @0x630490
SlotDataShortcutInterface::SlotDataShortcutInterface(SlotDataInterface *slot_iface, const std::string &name, SClient *sclient) :
	UIScutGadgetsClass(name, sclient),
	SlotIFace(slot_iface),
	Updated(false)
{












































	FAIL_IF(!SlotIFace) return;
	SlotDataClass *slotdata = SlotIFace->Get_Slot_Data();

	FAIL_IF(!slotdata) return;
	SlotDataClass::Signal().connect(slotdata, this, &SlotDataShortcutInterface::Slot_Data_Changed);
}

// @0x6307B0
SlotDataShortcutInterface::~SlotDataShortcutInterface()
{
	SlotDataClass *slotdata = SlotIFace->Get_Slot_Data();
	FAIL_IF(!slotdata) return;
	SlotDataClass::Signal().disconnect(slotdata, this, &SlotDataShortcutInterface::Slot_Data_Changed, SlotDataEventDataChange());
}

// @0x630980
void SlotDataShortcutInterface::Set_UI(const RefCountPtr<UIScutClass> &ui)
{
	UIScutGadgetsClass::Set_UI(ui);
	SlotDataClass *slotdata = SlotIFace->Get_Slot_Data();


	FAIL_IF(!slotdata) return;
	Update(slotdata);
}

// @0x6309D0
void SlotDataShortcutInterface::Slot_Data_Changed(SlotDataEventDataChange *event, SlotDataClass *slot_data) { Update(slot_data); }

// NOT EXACT: one out-of-line begin() more than the client in the last set's destructor (attempts.py show 0x6309F0)
// @0x6309F0
void SlotDataShortcutInterface::Update(SlotDataClass *slotdata)
{

	FAIL_IF(!SlotIFace) return;
	FAIL_IF(!slotdata) return;
	int state = Get_State();
	if (state == STATE_NO_UI || !slotdata->Unknown24) return;
	int num = slotdata->Unknown_6A7990();
	if (!num) return;
	bool add = Updated || !ShortcutBeenCreated;
	Updated = true;
	if (state == STATE_NOT_RESTORED) {
		bool restored = Restore();

































		G_ASSERT(restored);
	}
	std::set<int> slots;
	for (int lp = 0; lp < num; lp++) {
		int slot = slotdata->Unknown_6A7A00(lp);
		if (SlotIFace->Unknown_9C(slot, true)) {
			slots.insert(slot);
		}
	}
	if (add) {
		std::set<int> added;
		std::set_difference(slots.begin(), slots.end(), Slots.begin(), Slots.end(), std::inserter(added, added.begin()));
		if (added.size()) {
			std::set<int> unused;
			for (std::vector<RObjGadgetClass *>::iterator it = ShortcutGadgets.begin(); it != ShortcutGadgets.end(); ++it) {
				SlotDataGadgetClass *sdata_gadg = (SlotDataGadgetClass *)(*it)->Unknown_10();

























				FAIL_IF(!sdata_gadg) continue;
				SlotDataGadgetClass *scut_gadg = sdata_gadg->Unknown_A4();

				FAIL_IF(!scut_gadg) continue;
				if (scut_gadg->Unknown_88()) {
					std::set<int>::iterator found = added.find(scut_gadg->Unknown_670910());
					if (found != added.end()) {
						added.erase(found);
						if (!added.size()) break;
					}
				}
			}
			for (std::set<int>::iterator it2 = added.begin(); it2 != added.end(); ++it2) {
				int slot = *it2;
				Create_Shortcut(slot, NULL);
			}
			Save_Current();
		}
	}
	Slots.swap(slots);
	int dead = slots.size();	// (a budget lever: it keeps the set_difference and the destructors as the client has them)
	for (std::vector<RObjGadgetClass *>::iterator it = ShortcutGadgets.begin(); it != ShortcutGadgets.end(); ++it) {
		SlotDataGadgetClass *sdata_gadg = (SlotDataGadgetClass *)(*it)->Unknown_10();
























		FAIL_IF(!sdata_gadg) continue;
		sdata_gadg->Unknown_A8(0);
	}
}

// @0x631000
RObjGadgetClass *SlotDataShortcutInterface::Unknown_0C(int slot)
{













	FAIL_IF(!SlotIFace) return(NULL);
	if (!SlotIFace->Unknown_9C(slot, false)) return(NULL);
	UIViewClass *view = SlotIFace->Get_View();


	FAIL_IF(!view) return(NULL);
	RefCountPtr<SlotDataRenderClass> slot_render = Create_NEW(SlotDataRenderClass::Unknown_6A90A0(1, SlotIFace, 0));


	FAIL_IF(!slot_render) return(NULL);
	slot_render->Unknown_40(slot);
	LayerClass layer((SceneClass *)view->Unknown_50(), (CameraClass *)view->Unknown_54());
	return(new SlotDataShortcutGadgetClass(SlotIFace, slot_render.Peek(), layer));
}

// @0x631270
RObjGadgetClass *SlotDataShortcutInterface::Unknown_14(const char *key)
{




	FAIL_IF(!SlotIFace) return(NULL);
	FAIL_IF(!key) return(NULL);
	UIViewClass *view = SlotIFace->Get_View();

	FAIL_IF(!view) return(NULL);
	RefCountPtr<SlotDataRenderClass> slot_render = Create_NEW(SlotDataRenderClass::Unknown_6A90A0(1, SlotIFace, 0));

	FAIL_IF(!slot_render) return(NULL);
	if (!slot_render->Unknown_10(key)) return(NULL);
	LayerClass layer((SceneClass *)view->Unknown_50(), (CameraClass *)view->Unknown_54());
	return(new SlotDataShortcutGadgetClass(SlotIFace, slot_render.Peek(), layer));
}

// @0x631540
bool SlotDataShortcutInterface::Unknown_10(RObjGadgetClass *gadget, std::string &key)
{
	SlotDataGadgetClass *sdata_gadg = (SlotDataGadgetClass *)gadget->Unknown_10();









	FAIL_IF(!sdata_gadg) return(false);
	SlotDataRenderClass *srend = sdata_gadg->SlotRender;

	FAIL_IF(!srend) return(false);
	srend->Unknown_0C(key);
	return(true);
}

// @0x6315C0
int SlotDataShortcutInterface::Unknown_18()
{
	FAIL_IF(!SlotIFace) return(0);
	return(SlotIFace->Unknown_94() != 0);
}


/*
**	Reconstruction notes. The client's __FILE__ 0xB8A74C names the file; the asserts' lines (61, 64, 72, 83, 95, 96, 138, 179, 182, 226,
**	247, 252, 256, 269, 270, 273, 276, 295, 298, 306) place it. The object starts at the constructor 0x630490 (ui_slot_info.cpp's adjustor
**	thunks 0x630460-0x630480 before it) and its literals at 0xB8A730 ('!slotdata'), and ends after the guarded initialiser 0x632DA0 and the
**	callback's deleting destructor 0x632DD0 (ui_starbase.cpp follows at 0x632E50). The instances follow the file's own functions: set<int> /
**	_Tree 0x630680, 0x631600, 0x631780-0x632AA0 (0xBE5E64 / 0xBE5E68 its _Nilrefs / _Nil), RefCountPtr's destructor 0x631750, Create_NEW
**	0x631720, the callback 0x632740, 0x6328D0-0x632980 (Event_Type 0x62FBA0 folded with ui_slot_data.cpp's).
**	The class's name and the event's are the client's RTTI (0xB8A7C0, 0xB8A690). The set 'unused' in Update has no use the code shows: the
**	client builds and destroys a third set around the loop (0x631780, 0x631600). Get_Slot_Data and Signal are inline layers with no code of
**	their own: both are needed for the constructor to call _Buynode 0x632690 out of line as the client does (budget levers, not evidence).
*/
