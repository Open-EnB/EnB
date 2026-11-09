// Earth & Beyond client (reconstruction): app\client\dynamic_window_icon.h, a texture shown as a dynamic window element
// (the client's dynamic_window_icon.cpp, FAIL_IF 'texture_name.empty()' at 0x519A20; this header's name is provisional,
// NAME 1). Declarations only (its code is not in the tree yet).
#ifndef DYNAMIC_WINDOW_ICON_H  // NAME 1 [dynamic_window_icon.h]: provisional header name for dynamic_window_icon.cpp
#define DYNAMIC_WINDOW_ICON_H

#include "dynamic_window_class.h"
#include <string>
class TextureClass;

// dynamic_window_icon.cpp's element (0x54 bytes, made by its factory 0x519A20)
class DynamicWindowIconClass : public DynamicWindowClass  // NAME 3: class-from-file (D24) for dynamic_window_icon.cpp (base interim: what DynamicWindowGadgetClass takes)
{
public:
	static RefCountPtr<DynamicWindowIconClass> Create(const std::string &texture_name, const Vector2 &size, unsigned int flags);	// 0x519A20  // NAME 2 [texture_name]: the FAIL_IF's; NAME 1 [Create, size, flags]: as DynamicWindowTextClass's
	void Unknown_519FE0(const RefCountPtr<TextureClass> &texture);							// 0x519FE0 (the item examination title's icon changed; 0x51A190 reads a TextureClass reference)  // NAME 1: neutral (D10); NAME 1 [texture]: neutral
};

#endif
