// 0x6AF810 Unknown6AF220Class::~Unknown6AF220Class (app/client/unknown_6af220.cpp), builder L6, 2026-11-10.
// The client: slot +0x5C's body expanded (0x6B0940 called, the vector's erase(begin, end) with its copy folded
// (cmp edi, edi) and _Destroy 0x432C30 called out of line), then the slider's 0x66E460 with an empty RefCountPtr.
// Two states only came out of 17 forms (tryv2, .work/L6/v2.py .. v4.py):
//
// A (the tree's form; explain.py: shape, 27 masked rows, 7 once registers are masked): the copy's end is kept in a
//   stack slot (no fold), _Destroy is called:
//	Unknown_5C();
//	if (Slider) { Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0); }
//   The same state: Unknown6AF220Class::Unknown_5C(); a dead Items.size() before it or two at the end; and form B
//   with a dead Items.size() after Unknown_6B1000(), after InterfaceClass::Unknown_5C() or at the destructor's end.
//
// B (explain.py: relocation, ours expands _Destroy to the strings' ??_G 0x433A10): the copy folds as the client's,
//   _Destroy is expanded:
//	if (Active) {
//		Unknown_6B0940(false);
//		Unknown_6B1000();
//		InterfaceClass::Unknown_5C();
//	}
//	if (Slider) { ... }
//   The same state: Is_Active(); RefCountPtr<RenderObjClass>(NULL); a dead Items.size() before Unknown_6B1000() or
//   at the destructor's start; Unknown_6B1000()'s two statements written out (erase, Unknown104 = -1), alone or
//   with one dead read after them or at the end (two after them: back to the ??_G relocation); slot +0x5C itself
//   holding the erase instead of calling 0x6B1000.
//
// Not tried: a layer between the two (a file-local inline clear helper over the erase called from 0x6B1000, as
// workflow.md's "clear() is one inline layer over erase(begin(), end())"), which would change 0x6AFF60 / 0x6AFDE0
// / 0x6B1000 too (0x6AFF60 and 0x6B1000 are exact with the tree's form).
