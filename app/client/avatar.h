#ifndef AVATAR_H
#define AVATAR_H

#include "composite.h"
#include <list>
#include "..\shared\avatar_desc.h"
#include "..\shared\chat_edit_stream.h"
#include "nstrdup.h"
#include <string.h>
#include <vector>
#include "..\..\wwlib\ref_ptr.h"

class LightClass;
class HAnimClass;
class HMorphAnimClass;
class AudibleSoundClass;
class EnvironmentMapManagerClass;
class PlayerNLPClass;
class MeshClass;

// 0x784700's Value reads: inline (no client copy); with 0x784700's case members the inline budget it
// spends is what leaves the inlined 0x781C50's ??_G 0x781C10 and clear 0x78A510 out of line there
inline unsigned char Unknown_Block_Value(const Unknown_ChatBlockStruct *block) { return block->Value; }	// NAME 1 [block]: neutral (D10)

// an entry of the avatar's animation queue (+0x2A0; 0xC bytes, 0x781880 makes them, 0x781C10 deletes them)
struct Unknown_AvatarQueueStruct	// NAME 1: neutral (D10)
{
	Unknown_AvatarQueueStruct(char anim, const char *text, int mood)
	{
		Anim = anim;
		Text = nstrdup(text);
		strcpy(Text, text);
		Mood = mood;
	}
	~Unknown_AvatarQueueStruct() { delete Text; }	// 0x781C10 (its scalar deleting destructor)

	char Anim;	// +0x00 (0x782200: the body animation's number for 0x781D10)  // NAME 1: neutral (D10)
	char *Text;	// +0x04 (0x782200: the line its lip-sync keys are made for)  // NAME 1: neutral (D10)
	int Mood;	// +0x08 (0x782200: into Desc.Mood_Type unless < 0)  // NAME 1: neutral (D10)
};

// avatar.cpp's model (the view's +0x194, +0x198 and +0x1A4)
class CharacterViewAvatarClass : public CompositeRenderObjClass	// NAME 1: neutral (D10)
{
public:
	CharacterViewAvatarClass(AvatarDescClass *desc, bool a, bool b, EnvironmentMapManagerClass *env_maps);	// 0x77B870 (0x384 bytes)
	virtual ~CharacterViewAvatarClass();				// 0x77C080
	virtual RenderObjClass *Clone() const;				// 0x77C870
	virtual void Set_Transform(const Matrix3D &m);		// 0x77D1E0 (the body too; +0x330 keeps it unless +0x36C)
	virtual void Set_Position(const Vector3 &v);		// 0x77D2B0 (the body too)
	virtual void Render(RenderInfoClass &rinfo);		// 0x77D420
	virtual int Get_Num_Sub_Objects() const { return(1); }	// 0x77C030 (the body)
	virtual RenderObjClass *Get_Sub_Object(int index) const;	// 0x77D460 (the body, index 0)
	virtual void Prepare_LOD(CameraClass &camera);		// 0x77D3E0 (the body's)
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const { sphere = ObjSphere; }	// 0x77BFA0
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const { box = ObjBox; }	// 0x77BFE0
	virtual void Unknown_1C4();							// 0x783B50 (per-frame update)

	void Unknown_77D4A0();								// 0x77D4A0 (the accessories' meshes out of the head prototype, the creation prototypes released, +0x233 cleared)  // NAME 1: neutral
	void Unknown_77D7F0();								// 0x77D7F0 (0x781680, the fidget delay, 0x77DA20, the morph weights at once, 0x781EA0; unreferenced: the constructor inlines it)  // NAME 1: neutral
	void Unknown_77DA20();								// 0x77DA20 (the models from Desc: body, collision, head, mask, face animation, weapons, accessories)  // NAME 1: neutral
	void Unknown_781680();								// 0x781680 (a new PlayerNLP: FAIL_IF 'PlayerNLP != 0'; 0x77D7F0, 0x77E3F0, 0x77E610)  // NAME 1: neutral
	float Unknown_77D9F0(bool a);						// 0x77D9F0 (0x80E260: false): +0x234, plus 1 when a  // NAME 1 [a]: neutral
	void Unknown_77E3D0(int a);							// 0x77E3D0 (0x67A6C0: 1)  // NAME 1 [a]: neutral
	void Unknown_784A50(const char *name, bool a, bool b);	// 0x784A50 ('/walk_on' 1, 1; '/walk_off' 1, 0: the text parsed by the NLP into +0xE8, then 0x784700)  // NAME 1 [name, a, b]: neutral
	void Unknown_784B80(char *data, int size, bool flag);	// 0x784B80 (a chat stream's bytes into +0xE8, then 0x784700; CLIENT_CHAT_EVENT 165, MESSAGE_CHATSTREAM 95)  // NAME 1 [data, size, flag]: what the handler passes
	void Unknown_784700();								// 0x784700 (+0xE8's blocks into the queue +0x2A0 and the states +0x208 / +0x20C / +0x210)
	void Unknown_789FE0(const char *part);				// 0x789FE0 (the head prototype's '<part>0'..'<part>4' removed and released; 0x77F000 / 0x77F960 inline it)  // NAME 1 [part]: neutral
	void Unknown_781760(const char *text);				// 0x781760 (with an NLP: +0xE8 emptied, text parsed into it with Desc.Mood_Type, the end block; the UI's 0x569A00 calls it, 0x784A50 / 0x7860C0 inline it)  // NAME 1 [text]: neutral
	void Unknown_781800(Unknown_ChatStreamClass *stream);	// 0x781800 (+0xE8 emptied, then stream's bytes copied in; unreferenced)  // NAME 1 [stream]: neutral
	void Unknown_781840(const void *data, int size);	// 0x781840 (+0xE8 emptied, then the bytes copied in; 0x784B80 inlines it)  // NAME 1 [data, size]: neutral
	void Unknown_77E370(const char *name);				// 0x77E370 (Desc's First_Name through 0x4C1460, converted; unreferenced)  // NAME 1 [name]: neutral
	void Unknown_77E3A0(const char *name);				// 0x77E3A0 (Desc's Last_Name through 0x4C15F0, converted; unreferenced)  // NAME 1 [name]: neutral
	void Unknown_785080(int kind);						// 0x785080 (+0xE8 emptied, one block by kind, the end block; unreferenced)  // NAME 1 [kind]: neutral
	void Unknown_77E350(int a);							// 0x77E350 (empty; unreferenced)  // NAME 1 [a]: neutral
	void Unknown_7860A0();								// 0x7860A0 (empty; unreferenced)  // NAME 1: neutral
	Matrix3D Unknown_787130();							// 0x787130 (+0x330 set; skel_reference raised, turned by +0x310, 11 back, pitched by +0x30C + 0.3; unreferenced)  // NAME 1: neutral
	void Unknown_785450(float yaw, float pitch);		// 0x785450 (+0x250 / +0x258, the head's look-at angles; unreferenced)  // NAME 1 [yaw, pitch]: 0x7854B0's use of them
	void Unknown_785480();								// 0x785480 (+0x250 / +0x258 / +0x264 / +0x268 cleared; 0x781EA0 inlines it)  // NAME 1: neutral
	void Unknown_7862D0();								// 0x7862D0 (SpeechSound stopped and released; 0x7861B0 inlines it; 0x784E00 writes it out: through it the dtor loses its out-of-line operator= 0x6A2610)  // NAME 1: neutral
	void Unknown_786320();								// 0x786320 (a running (+0x2F4) or walking footstep sound at +0x330 on surface +0x23C; unreferenced)  // NAME 1: neutral
	void Unknown_784ED0();								// 0x784ED0 (chat block 6: +0x20C 2 -> 0 while +0x210 is 0, +0x208 2; 0x784700 / 0x784F60 inline it)  // NAME 1: neutral
	void Unknown_784F20();								// 0x784F20 (chat block 7: +0x20C back to 2, +0x208 0; 0x788900 calls it)  // NAME 1: neutral
	void Unknown_784F60(bool on);						// 0x784F60 (+0x2F4 set; on a change while +0x2F8 & 1, 0x784F20 then 0x784ED0)  // NAME 1 [on]: neutral
	void Unknown_784FF0();								// 0x784FF0 (chat block 4: +0x210 0 -> 1, +0x208 3, +0x20C 2; 0x784700 inlines it)  // NAME 1: neutral
	void Unknown_785040();								// 0x785040 (chat block 5: +0x210 3 -> 2, +0x208 3; 0x784700 inlines it)  // NAME 1: neutral
	void Unknown_785F20(bool on);						// 0x785F20 (the 'avatar_shadow' object +0x2C4 taken off the body, and with on made again onto its 'skel_reference' bone; the UI's 0x634140 calls it)  // NAME 1 [on]: neutral
	void Unknown_781880(char anim, const char *text, int mood);	// 0x781880 (a queue entry pushed)  // NAME 1 [anim, text, mood]: neutral
	Unknown_AvatarQueueStruct *Unknown_781B20();				// 0x781B20 (the queue's first entry or NULL; unreferenced: 0x782200 inlines it)  // NAME 1: neutral
	void Unknown_781B50(Unknown_AvatarQueueStruct *entry);	// 0x781B50 (entry erased from the queue and deleted; unreferenced: 0x782200 inlines it)  // NAME 1 [entry]: neutral
	void Unknown_781C50();								// 0x781C50 (the queue's entries deleted, the queue cleared; unreferenced: 0x77C080 / 0x784700 inline it)  // NAME 1: neutral
	bool Unknown_7835D0();								// 0x7835D0 (no gesture blending, and +0x214 consumed or the gesture done: the next queue step may start; unreferenced: 0x783B50 inlines it)  // NAME 1: neutral
	bool Unknown_789D40();								// 0x789D40 (the playing gesture's +0xDC flags set, i.e. not -1)  // NAME 1: neutral
	void Unknown_789CD0(int a);							// 0x789CD0 (gesture slot a emptied: +0xC0 / +0xC8 / +0xCC / +0xD4 / +0xDC; unreferenced: 0x77C080 inlines it)  // NAME 1 [a]: neutral
	void Unknown_782200();								// 0x782200 (state 1: the queue's first entry played, its face keys made)
	void Unknown_7826A0();								// 0x7826A0 (state 2: the walk / run animation)
	void Unknown_782850();								// 0x782850 (state 3: sit down / stand up)
	bool Unknown_7829B0(const char *name);				// 0x7829B0 (the animation's face keys from its ini; false: none)  // NAME 1 [name]: neutral
	bool Unknown_7860C0();								// 0x7860C0 (a narrator's next tutorial line spoken; inlined in 0x783B50)
	void Unknown_7861B0(const char *speech);			// 0x7861B0 (the speech sound into +0x2E8)  // NAME 1 [speech]: neutral
	void Unknown_784C90(const char *name, bool a);		// 0x784C90 ('Idle0', 1; ui_login.cpp's 'RejectedOnce', 1)  // NAME 1 [name, a]: neutral
	void Unknown_784E00();								// 0x784E00
	void Unknown_786370(bool on);						// 0x786370  // NAME 1 [on]: neutral
	bool Unknown_784EA0(const char *name);				// 0x784EA0 (TutorialManagerClass::Was_Just_Finished of the name; ui_shard.cpp's 'AllGood')  // NAME 1 [name]: Was_Just_Finished's
	bool Unknown_7863E0();								// 0x7863E0
	void Unknown_788EF0();								// 0x788EF0
	void Unknown_788F90(int mode);						// 0x788F90 (the teleport effect: 1..4, 0 none)  // NAME 1 [mode]: neutral
	void Unknown_7896B0(int a);							// 0x7896B0 (0x67B390: 4)  // NAME 1 [a]: neutral
	void Unknown_786B60(bool keep);						// 0x786B60 (flag 1)  // NAME 1 [keep]: neutral
	void Unknown_786BF0(bool keep);						// 0x786BF0 (flag 2)  // NAME 1 [keep]: neutral
	void Unknown_786C70();								// 0x786C70 (flags 1 / 2 off)
	void Unknown_Inline_786C70() { if (Unknown20C != 2) { Unknown208 = 0; Unknown20C = 2; Unknown214 = true; } Unknown2F8 &= 0xFFFC; }	// (0x786B60 / 0x786BF0: 0x786C70 inline)  // NAME 1: neutral (D10)
	void Unknown_786CC0(float rate);					// 0x786CC0 (flag 8)  // NAME 1 [rate]: neutral
	void Unknown_786CF0(float rate);					// 0x786CF0 (flag 4)  // NAME 1 [rate]: neutral
	void Unknown_786D20();								// 0x786D20 (flags 4 / 8 off)
	void Unknown_786D40(float rate);					// 0x786D40 (flag 0x10)  // NAME 1 [rate]: neutral
	void Unknown_786D70(float rate);					// 0x786D70 (flag 0x20)  // NAME 1 [rate]: neutral
	void Unknown_786DA0();								// 0x786DA0 (flags 0x10 / 0x20 off)
	void Unknown_786DC0(float rate);					// 0x786DC0 (flag 0x40)  // NAME 1 [rate]: neutral
	void Unknown_786E00(float rate);					// 0x786E00 (flag 0x80)  // NAME 1 [rate]: neutral
	void Unknown_786E40();								// 0x786E40 (flags 0x40 / 0x80 off)
	void Unknown_786E60();								// 0x786E60 (all movement off)
	Matrix3D Unknown_787DE0();							// 0x787DE0 (+0x330 with this frame's step +0x360 taken back off its translation; the UI's 0x63BC30 calls it)  // NAME 1: neutral
	void Unknown_7885D0(bool second);					// 0x7885D0 (clears +0x318, or +0x324)  // NAME 1 [second]: neutral
	void Unknown_788700(const Matrix3D &goal);			// 0x788700 (the path: goal alone)  // NAME 1 [goal]: neutral
	void Unknown_788730(const Matrix3D &goal, bool trim);	// 0x788730 (goal onto the path; trim: 3 at most)  // NAME 1 [goal, trim]: neutral
	void Unknown_788890();								// 0x788890 (the path's first goal off)
	void Unknown_788900();								// 0x788900 (the movement flags toward the path's first goal)
	void Unknown_7897D0();								// 0x7897D0 (the teleport effect's time)
	void Unknown_781EA0();								// 0x781EA0 (the idle: breathe / bored / sit_breathe; sets +0x220)
	std::string Unknown_781D10(int anim);				// 0x781D10 ('%s.ABANIM%02d-%02d': skeleton, personality, anim)  // NAME 1 [anim]: neutral
	void Unknown_782CA0();								// 0x782CA0 (the face animation's brow and blink keys over the gesture animation)
	void Unknown_782FF0(const char *text);				// 0x782FF0 (the face animation's lip-sync keys for text)  // NAME 1 [text]: neutral
	void Unknown_783470();								// 0x783470 (the face animation onto the head)
	PlayerNLPClass *Get_NLP() { return(PlayerNLP); }	// (inline: 0x7899F0's assert '!Get_NLP()')
	void Unknown_7899F0(const char *anim, float frame, int flags);	// 0x7899F0 (the personality's body animation by tag as the gesture, then 0x789C10; the UI's 0x53CD60)  // NAME 1 [anim]: its assert's; [frame, flags]: 0x789C10's
	void Unknown_789C10(float frame, int flags);		// 0x789C10 (the gesture's frame to stop at and flags; 0x7899F0 inlines it)  // NAME 1 [frame, flags]: neutral
	void Unknown_7831C0(const char *name);				// 0x7831C0 (the gesture animation into +0xC0, or +0xC4 to blend; 'Cannot Find Gesture Animation')  // NAME 1 [name]: neutral
	void Unknown_783620(float time);					// 0x783620 (the frames on, the body's Set_Animation)  // NAME 1 [time]: neutral
void Unknown_780AF0();								// 0x780AF0 (the body morph: ARM / SHIRT / PANT meshes scaled, the HTree altered)
	void Unknown_780F30();								// 0x780F30 (the head morph: every head mesh scaled, the HTree altered)
	void Unknown_7812A0(MeshClass *source_mesh, MeshClass *dest_mesh);	// 0x7812A0 (the source's vertices into the copy's; '!source_mesh || !dest_mesh')
	void Unknown_781410(int now, int steps);			// 0x781410 (steps +0x274 toward Desc.Body_Weight, then 0x780AF0; 'Avatar Morphing')  // NAME 1 [now, steps]: neutral
	void Unknown_781570(int now, int steps);			// 0x781570 (steps +0x288 toward Desc.Head_Weight, then 0x780F30)  // NAME 1 [now, steps]: neutral
	void Unknown_77E0E0(int kind, const char *part, int num, const char *bone, RefCountPtr<RenderObjClass> *slot);	// 0x77E0E0 (a weapon onto the body's bone; kind 0 '%s%s%s%02d', 1 'a%s%s%02d', 2 'a%s%02d')  // NAME 1 [kind, part, num, bone, slot]: neutral
	void Unknown_77FDA0(int num);						// 0x77FDA0 (Weapon_Unique_Num set and its weapon made onto the leg / arm bone, with a body; unreferenced: 0x77DA20 inlines it)  // NAME 1 [num]: neutral
	void Unknown_77FE20(int num);						// 0x77FE20 (Weapon_Hip_Num's, WEAPHIP; unreferenced: 0x77DA20 inlines it)  // NAME 1 [num]: neutral
	void Unknown_77FE70(int num);						// 0x77FE70 (Weapon_Back_Num's, WEAPBAK; unreferenced: 0x77DA20 inlines it)  // NAME 1 [num]: neutral
	void Unknown_780810(EnvironmentMapManagerClass *env_maps);	// 0x780810 (the constructor; ui_avatar_cust / SClient): the shirt and pants colorization regions and the skin colour onto the body and head (shiputils.cpp 0x80FCE0)  // NAME 1 [env_maps]: neutral
	void Unknown_7804A0(const Vector3 &color);			// 0x7804A0 (Eye_Color onto LEYE010..4 / REYE010..4)  // NAME 1 [color]: neutral
	void Unknown_77E830(const float *weights);			// 0x77E830 (the body morph weights, normalised; ui_avatr.cpp's 0x558060)  // NAME 1 [weights]: its assert's
	void Unknown_77E900(const float *weights);			// 0x77E900 (the head morph weights, normalised; ui_avatr.cpp's 0x558060)  // NAME 1 [weights]: its assert's
	void Unknown_77F000(int num);						// 0x77F000 (Head_Type's head loaded again; 0x77DA20, ui_avatr.cpp)  // NAME 1 [num]: neutral
	void Unknown_77E9D0(int body, int pants, bool morph);	// 0x77E9D0 (Body_Type / Pants_Type's body loaded again; 0x77DA20, ui_avatr.cpp)  // NAME 1 [body, pants, morph]: neutral
	void Unknown_77FC20(int num, bool rebuild);			// 0x77FC20 (Hair_Num's 'HAIR' accessory)  // NAME 1 [num, rebuild]: neutral
	void Unknown_77FD30(int num, bool rebuild);			// 0x77FD30 (Beard_Num's 'BEARD' accessory)  // NAME 1 [num, rebuild]: neutral
	void Unknown_780680(int index, Vector3 color);		// 0x780680 (Skin_Color, then the shirt's and pants' colours by index 1 .. 4)  // NAME 1 [index, color]: neutral
	void Unknown_780790(int index, int metal);			// 0x780790 (the shirt's and pants' metals by index 1 .. 4)  // NAME 1 [index, metal]: neutral
	void Unknown_77FC90(int num, bool rebuild);			// 0x77FC90 (Ear_Num's 'EAR' accessory)  // NAME 1 [num, rebuild]: neutral
	void Unknown_77FCE0(int num, bool rebuild);			// 0x77FCE0 (Goggle_Num's 'GOGGLE' accessory)  // NAME 1 [num, rebuild]: neutral
	void Unknown_77F750();							// 0x77F750 (the head object cloned again from the head prototype, onto K_HEAD)
	void Unknown_7857D0(int num);						// 0x7857D0 (Head_Texture_Num's '<gender><race>FACETEX%02d.tga' onto head0..4)  // NAME 1 [num]: neutral
	void Unknown_7859F0(int num);						// 0x7859F0 (Tattoo_Texture_Num's 'ATATTOO%02d.tga' onto HEAD0..4, stage 1)  // NAME 1 [num]: neutral
	void Unknown_785C00(const Vector3 &offset);			// 0x785C00 (Tattoo_Offset: a linear offset mapper on HEAD0..4's stage 1)  // NAME 1 [offset]: neutral
	void Unknown_77F960(const char *part, int num, RefCountPtr<RenderObjClass> *slot);	// 0x77F960 ('%s%s%s%02d': gender, race, part, num; 'HAIR' +0x2C8, 'EAR' +0x2CC, 'GOGGLE' +0x2D0, 'BEARD' +0x2D4)  // NAME 1 [part, num, slot]: neutral
	void Unknown_77FEC0(RefCountPtr<RenderObjClass> *robj, const Vector3 &color, int index);	// 0x77FEC0 (the colour onto vertex material index)  // NAME 1 [robj, color, index]: neutral
	void Unknown_77FFA0(const Vector3 &color, bool rebuild);	// 0x77FFA0 (Hair_Color onto HAIR0..4 and BEARD0..4)  // NAME 1 [color, rebuild]: neutral
	void Unknown_7802F0(const Vector3 &color, bool rebuild);	// 0x7802F0 (Beard_Color onto BEARD0..4)  // NAME 1 [color, rebuild]: neutral
	void Unknown_789D80(RefCountPtr<RenderObjClass> *slot, const char *part);	// 0x789D80 (the accessory's '<part>0..4' meshes into the head prototype's LODs)  // NAME 1 [slot, part]: neutral
		void Unknown_7854B0(float time);					// 0x7854B0 (the head's random look about, K_Head)  // NAME 1 [time]: neutral

	// +0xBC: sizeof(CompositeRenderObjClass)
	RefCountPtr<HMorphAnimClass> Unknown0BC;			// +0xBC (0x77DA20: the face animation, channels 0 mouth, 1 brows, 2 eyelids; a RefCountPtr: 0x77B870 constructs, 0x77C080 destroys it as a member)
	RefCountPtr<HAnimClass> Unknown0C0[2];				// +0xC0 (0x7831C0: the gesture animation, [1] the one blended to; RefCountPtrs: 0x77B870 constructs them through 0x78A440, 0x77C080 destroys them through 0x57B6E0)
	bool Unknown0C8[2];									// +0xC8 (0x783620: the frame reached)
	char Unknown_Pad0CA[0xCC - 0xCA];
	float Unknown0CC[2];								// +0xCC (0x783620: the frame)
	float Unknown0D4[2];								// +0xD4 (0x783620: the frame to stop at)
	int Unknown0DC[2];									// +0xDC (0x783620: flags, -1 none)
	int Unknown0E4;										// +0xE4 (0x783B50: the WW3D frame count of the last update)
	Unknown_ChatStreamClass Unknown0E8;					// +0xE8 (0x77B870: 0x4C4430; the NLP's command stream, 0x784A50)
	PlayerNLPClass *PlayerNLP;							// +0xF4 (0x77B870: 0x78EB50)  // NAME 2: PlayerNLP != 0 assert
	AvatarDescClass *Get_Desc() { return(&Desc); }	// (inline: ui_avatr.cpp's 0x559350 calls an out-of-line copy, 0x559490)  // NAME 1: neutral (D10), as ui_avatar_cust.cpp's view
	AvatarDescClass Desc;								// +0xF8 (0x10C bytes)
	int Unknown204;										// +0x204 (0x77E3D0 sets it)
	int Unknown208;										// +0x208
	int Unknown20C;										// +0x20C
	int Unknown210;										// +0x210
	bool Unknown214;									// +0x214 (0x67A440 sets it)
	bool Unknown215;									// +0x215
	char Unknown_Pad216[0x218 - 0x216];
	float Unknown218;									// +0x218 (0x7831C0: the blend time, 0.5)
	float Unknown21C;									// +0x21C (0x783620: the blend time left)
	bool Is_Unknown220() { return(Unknown220); }		// (ui_login.cpp)  // NAME 1: neutral (D10)
	bool Unknown220;									// +0x220 (ui_login.cpp's 0x5ADA50: narrator welcome waits for it)
	bool Unknown221;									// +0x221
	char Unknown_Pad222[0x224 - 0x222];
	int Unknown224;										// +0x224 (0x781EA0: random part of the fidget delay)
	float Unknown228;									// +0x228
	float Unknown22C;									// +0x22C (0x783B50: the time idle)
	bool Unknown230;									// +0x230 (0x77B870 sets it)
	bool Unknown231;									// +0x231 (0x67A440 clears it)
	bool Unknown232;									// +0x232 (0x67A440 sets it)
	void Unknown_Inline_231() { Unknown231 = false; Unknown214 = true; }	// NAME 1: neutral (D10)
	bool Unknown233;									// +0x233 (0x77B870 sets it)
	float Unknown234;									// +0x234 (0x781410: K_HEAD's height above skel_reference)
	int Unknown238;										// +0x238 (0x783620: footsteps played)
	int Unknown23C;										// +0x23C (0x783620: the surface type)
	int Unknown240;										// +0x240
	int Unknown244;										// +0x244 (0x781EA0: the bored count)
	int Unknown248;										// +0x248
	int Unknown24C;										// +0x24C
	float Unknown250;									// +0x250 (0x7854B0: the head's yaw to look at, degrees; 0x781EA0 clears it)
	float Unknown254;									// +0x254 (0x7854B0: the head's yaw, degrees)
	float Unknown258;									// +0x258 (0x7854B0: the head's pitch to look at; 0x781EA0 clears it)
	float Unknown25C;									// +0x25C (0x7854B0: the head's pitch, degrees)
	float Unknown260;									// +0x260 (0x7854B0: the time not yet stepped, 20 steps a second)
	float Unknown264;									// +0x264 (0x7854B0: random yaw offset -10..10; 0x781EA0 clears it)
	float Unknown268;									// +0x268 (0x7854B0: random pitch offset -5..5; 0x781EA0 clears it)
	unsigned int Unknown26C;							// +0x26C (0x7854B0: the step count)
	unsigned int Unknown270;							// +0x270 (0x7854B0: the step of the next random offsets)
	float Unknown274[5];								// +0x274 (0x781410: the body morph weights shown, stepped toward Desc.Body_Weight)
	float Unknown288[5];								// +0x288 (0x781570: the head morph weights shown, stepped toward Desc.Head_Weight)
	bool Unknown29C;									// +0x29C (0x781410: the body morph still stepping)
	bool Unknown29D;									// +0x29D (0x781570: the head morph still stepping)
	std::vector<Unknown_AvatarQueueStruct *> Unknown2A0;	// +0x2A0 (the animation queue: 0x781880 pushes, 0x782200 plays, 0x784700 / 0x77C080 empty it)
	RefCountPtr<RenderObjClass> HeadSource;	// +0x2B0  // NAME 2: '!HeadSource || !BodyObj' assert
	RefCountPtr<RenderObjClass> BodySource;	// +0x2B4  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> HeadObj;				// +0x2B8  // NAME 2: '!HeadObj' assert (0x786370 sets hidden)
	RefCountPtr<RenderObjClass> BodyObj;	// +0x2BC  // NAME 2: '!HeadSource || !BodyObj' assert (0x786370 / 0x7863E0)
	RefCountPtr<RenderObjClass> CollisionObj;	// +0x2C0  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Unknown2C4;	// +0x2C4
	RefCountPtr<RenderObjClass> HairSource;	// +0x2C8  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Unknown2CC;	// +0x2CC
	RefCountPtr<RenderObjClass> GoggleSource;	// +0x2D0  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> BeardSource;	// +0x2D4  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> WeaponHip;	// +0x2D8 (0x77DA20: Weapon_Unique_Num's, WEAPON01 / WEAPON04)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> WeaponUnique;	// +0x2DC (0x77DA20: Weapon_Hip_Num's, WEAPON02)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> WeaponBack;	// +0x2E0  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> MaskObj;	// +0x2E4  // NAME 1: neutral (D10)
	RefCountPtr<AudibleSoundClass> SpeechSound;			// +0x2E8 (0x784E00 / 0x784C90, 0x7861B0 a RefCountPtr)  // NAME 2: plays speech sound
	char *Unknown2EC;									// +0x2EC (nstrdup in 0x784C90, delete in 0x784E00)
	float Unknown2F0;									// +0x2F0 (60.0f in 0x784C90)
	bool Unknown2F4;									// +0x2F4 (0x77B870 sets it; 0x783620: running footsteps)
	bool Unknown2F5;									// +0x2F5 (0x77B870 sets it; 0x786DC0 / 0x786E00 test it)
	int Unknown2F8;										// +0x2F8 (the movement flags: 1 / 2 forward / back, 4 / 8 turn, 0x10 / 0x20 +0x30C, 0x40 / 0x80 +0x310)
	float Unknown2FC;									// +0x2FC (the turn rate, 1.0: 0x786CC0 / 0x786CF0)
	float Unknown300;									// +0x300 (+0x30C's rate, 1.0: 0x786D40 / 0x786D70)
	float Unknown304;									// +0x304 (+0x310's rate, 1.0: 0x786DC0 / 0x786E00)
	float Unknown308;									// +0x308 (the speed, 0x783B50: -10 .. 10, 25 running)
	float Unknown30C;									// +0x30C (0x783B50: -0.8 .. 0.4 by flags 0x10 / 0x20)
	float Unknown310;									// +0x310 (0x783B50: by flags 0x40 / 0x80)
	int Unknown314;										// +0x314
	std::list<Matrix3D *> Unknown318;					// +0x318 (the path to walk: 0x788730 adds, 0x788900 follows)
	std::list<Matrix3D *> Unknown324;					// +0x324 (0x7885D0 clears it)
	Matrix3D Unknown330;								// +0x330  // NAME 1: neutral (D10)
	Vector3 Unknown360;									// +0x360 (0x783B50: the step moved this frame)
	bool Unknown36C;									// +0x36C (0x77B870 clears it; set: 0x77D1E0 leaves +0x330 alone)
	RefCountPtr<RenderObjClass> Unknown370;			// +0x370 (0x788F90's av_teleNN; a RefCountPtr: 0x77C080 releases it through 0x5988A0, operator =(0))
	RefCountPtr<HAnimClass> Unknown374;				// +0x374 (0x788F90's av_teleNNa animation; a RefCountPtr: 0x77C080 releases it through 0x78A460, operator =(0))
	float Unknown378;									// +0x378 (0x7897D0: the time left at which the body shows / hides; -100 done)
	float Unknown37C;									// +0x37C (0x7897D0: the time left)
	int Unknown380;										// +0x380 (0x7897D0: the teleport effect, 0 none, 2 / 4 show the body)
};

#endif
