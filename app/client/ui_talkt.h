// Earth & Beyond client (reconstruction): app\client\ui_talkt.cpp's talk tree (0x180 bytes), declared as far as
// its users call it; ui_talkt.cpp itself (client 0x64B6E0-0x64EF40) is not rebuilt yet. sclient.h includes it after
// the class's base. SClient's +0x1298 holds it or ui_talkt_space.cpp's (derived) +0x129C; the starbase view's +0x190
// is one (0x69F8F0 hands &+0x190 straight to 0x74C9D0). Base: 0x64B6E0 calls 0x5BB320, and the slots +0x88 / +0x8C of
// its table 0xAF8B4C are Unknown5BB320Class's 0x543750 / 0x543770.
#ifndef UI_TALKT_H
#define UI_TALKT_H

class TalkTreePacketClass;
class UITalkTreeClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_talkt.cpp
{
public:
	UITalkTreeClass(SClient *client, int arg);	// 0x64B6E0 (the starbase view's 0x69F8F0 passes its client and 1; ui_talkt_space.cpp's 0x64F310 its client and 0)  // NAME 1 [client, arg]: neutral
	~UITalkTreeClass();							// 0x64B940 (table 0xAF8B4C; slot 0x64B910)
	virtual void Unknown_54();					// +0x54 0x64BCA0
	virtual void Unknown_58();					// +0x58 0x64C800
	virtual void Unknown_5C();					// +0x5C 0x64C8D0
	virtual void Unknown_60();					// +0x60 0x64D770
	virtual void Unknown_64();					// +0x64 0x64DA40
	virtual bool Unknown_68(int key);			// +0x68 0x64D400
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x64CC40
	virtual void Unknown_90(TalkTreePacketClass *packet);	// +0x90 0x64CC90 (TALK_TREE 84)  // NAME 1 [packet]: the handler's packet
	int Unknown_64CF90(); void Unknown_64CFF0(); bool Unknown_64D3E0(); bool Unknown_64EEA0(); void Unknown_64EED0();	// the starbase view's 0x69C8F0 / 0x69CCD0; 0x64EED0 (0x742530)
	void Unknown_64D230(int action);	// 0x64D230 (TALK_TREE_ACTION 86)  // NAME 1 [action]: the packet's Action
	void Unknown_64C7B0(const std::string &name);	// 0x64C7B0 (G_ASSERT '!NPCName'; ui_talkt_space.cpp's 0x64F630 passes the avatar's full name)  // NAME 1 [name]: what 0x64F630 passes
	char UnknownAC[0x160 - 0xAC];	// +0xAC
	int Unknown160;	// +0x160 (the starbase view's 0x69CCD0 tests it with Active)
	char Unknown164[0x180 - 0x164];	// +0x164
};

#endif
