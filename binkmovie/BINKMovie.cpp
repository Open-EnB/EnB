/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "binkmovie.h"
#include "dx8wrapper.h"
#include "formconv.h"
#include "render2d.h"
#include "Bink.h"
#include "rect.h"
#include "subtitlemanager.h"
#include "dx8caps.h"

class BINKMovieClass
{
	private:
		StringClass Filename;
		HBINK Bink;
		bool FrameChanged;
		unsigned TextureCount;
		unsigned MaxWidth;	// NAME 2 [MaxWidth]: the textures' width (Renegade's local max_width)
		unsigned MaxHeight;	// NAME 2 [MaxHeight]: the textures' height (Renegade's local max_height)

		struct TextureInfoStruct {
			TextureClass* Texture;
			int TextureWidth;
			int TextureHeight;
			int TextureLocX;
			int TextureLocY;
			RectClass UV;
			RectClass Rect;
		};

		TextureInfoStruct* TextureInfos;
		TextureInfoStruct* TextureInfos1;	// NAME 1 [TextureInfos1]: the first texture set
		TextureInfoStruct* TextureInfos2;	// NAME 1 [TextureInfos2]: the second texture set
		unsigned char* TempBuffer;
		Render2DClass* Renderer;
		SubTitleManagerClass* SubTitleManager;

		void Create_Textures();	// NAME 2 [Create_Textures]: news TextureInfos and its textures
		void Free_Textures();	// NAME 2 [Free_Textures]: releases TextureInfos' textures and deletes it

	public:
		BINKMovieClass(const char* filename,const char* subtitlename);
		~BINKMovieClass();

		void Update();
		void Render();
		bool Is_Complete();
		void Set_Volume(int volume);	// NAME 2 [Set_Volume]: BinkSetVolume
};


static BINKMovieClass* CurrentMovie;


void BINKMovie::Play(const char* filename,const char* subtitlename, int volume)
{
	if (CurrentMovie) {
		delete CurrentMovie;
		CurrentMovie = NULL;
	}

	CurrentMovie = new BINKMovieClass(filename,subtitlename);
	CurrentMovie->Set_Volume(volume);
}


void BINKMovie::Stop()
{
	if (CurrentMovie) {
		delete CurrentMovie;
		CurrentMovie = NULL;
	}
}


void BINKMovie::Update()
{
	if (CurrentMovie) {
		CurrentMovie->Update();
	}
}


void BINKMovie::Render()
{
	if (CurrentMovie) {
		CurrentMovie->Render();
	}
}


void BINKMovie::Init()
{
	BinkSoundUseDirectSound(0);
}


void BINKMovie::Shutdown()
{
	Stop();
}


bool BINKMovie::Is_Complete()
{
	if (CurrentMovie) {
		return CurrentMovie->Is_Complete();
	}

	return true;
}


// ----------------------------------------------------------------------------

BINKMovieClass::BINKMovieClass(const char* filename, const char* subtitlename)
	:
	Filename(filename),
	Bink(0),
	FrameChanged(true),
	SubTitleManager(NULL)
{
	Bink = BinkOpen(Filename, 0);

	if (Bink == NULL) {
		return;
	}

	TempBuffer = new unsigned char[Bink->Width * Bink->Height*2];

	const D3DCAPS8& dx8caps = DX8Wrapper::Get_Current_Caps()->Get_DX8_Caps();
	unsigned poweroftwowidth = 1;

	while (poweroftwowidth < Bink->Width) {
		poweroftwowidth <<= 1;
	}

	unsigned poweroftwoheight = 1;

	while (poweroftwoheight < Bink->Height) {
		poweroftwoheight <<= 1;
	}

	if (poweroftwowidth > dx8caps.MaxTextureWidth) {
		poweroftwowidth = dx8caps.MaxTextureWidth;
	}

	if (poweroftwoheight > dx8caps.MaxTextureHeight) {
		poweroftwoheight = dx8caps.MaxTextureHeight;
	}

	TextureCount = 0;
	MaxWidth = poweroftwowidth;
	MaxHeight = poweroftwoheight;
	unsigned x, y;

	for (y = 0; y < Bink->Height; y += poweroftwoheight-2) {		// Two pixels are lost due to duplicated edges to prevent bilinear artifacts
		for (x = 0; x < Bink->Width; x += poweroftwowidth-2) {
			++TextureCount;
		}
	}

	Create_Textures();
	TextureInfos1 = TextureInfos;
	TextureInfos = NULL;
	Create_Textures();
	TextureInfos2 = TextureInfos;

	Renderer = new Render2DClass;

	if (subtitlename) {
		SubTitleManager = SubTitleManagerClass::Create(subtitlename);
		if (SubTitleManager) {
			SubTitleManager->Reset();
		}
	}
}


BINKMovieClass::~BINKMovieClass()
{
	if (Bink) {
		BinkClose(Bink);
		Bink = NULL;

		delete[] TempBuffer;
		TempBuffer = NULL;

		TextureInfos = TextureInfos1;
		TextureInfos1 = NULL;
		Free_Textures();

		TextureInfos = TextureInfos2;
		TextureInfos2 = NULL;
		Free_Textures();

		if (Renderer) {
			delete Renderer;
			Renderer = NULL;
		}
	}
}


void BINKMovieClass::Create_Textures()
{
	TextureInfos = new TextureInfoStruct[TextureCount];
	unsigned cnt = 0;
	unsigned x, y;

	for (y = 0; y < Bink->Height; y += MaxHeight-1) {
		for (x = 0; x < Bink->Width; x += MaxWidth-1) {
			TextureInfos[cnt].Texture = new TextureClass(
				MaxWidth, MaxHeight, D3DFormat_To_WW3DFormat(D3DFMT_R5G6B5),
				MIP_LEVELS_1, TextureClass::POOL_MANAGED, false);

			TextureInfos[cnt].TextureLocX = x;
			TextureInfos[cnt].TextureLocY = y;
			TextureInfos[cnt].TextureWidth = MaxWidth;
			TextureInfos[cnt].UV.Right = float(MaxWidth) / float(MaxWidth);

			if ((TextureInfos[cnt].TextureWidth + x) > Bink->Width) {
				TextureInfos[cnt].TextureWidth = Bink->Width - x;
				TextureInfos[cnt].UV.Right = float(TextureInfos[cnt].TextureWidth - 1) / float(MaxWidth);
			}

			TextureInfos[cnt].TextureHeight = MaxHeight;
			TextureInfos[cnt].UV.Bottom = float(MaxHeight) / float(MaxHeight);

			if ((TextureInfos[cnt].TextureHeight + y) > Bink->Height) {
				TextureInfos[cnt].TextureHeight = Bink->Height - y;
				TextureInfos[cnt].UV.Bottom = float(TextureInfos[cnt].TextureHeight + 1) / float(MaxHeight);
			}

			TextureInfos[cnt].UV.Left = 1.0f / float(MaxWidth);
			TextureInfos[cnt].UV.Top = 1.0f / float(MaxHeight);

			TextureInfos[cnt].Rect.Left = float(TextureInfos[cnt].TextureLocX) / float(Bink->Width);
			TextureInfos[cnt].Rect.Top = float(TextureInfos[cnt].TextureLocY) / float(Bink->Height);
			TextureInfos[cnt].Rect.Right = float(TextureInfos[cnt].TextureLocX + TextureInfos[cnt].TextureWidth) / float(Bink->Width);
			TextureInfos[cnt].Rect.Bottom = float(TextureInfos[cnt].TextureLocY + TextureInfos[cnt].TextureHeight) / float(Bink->Height);

			++cnt;
		}
	}
}


void BINKMovieClass::Free_Textures()
{
	if (TextureInfos) {
		for (unsigned t = 0; t < TextureCount; ++t) {
			REF_PTR_RELEASE(TextureInfos[t].Texture);
		}

		delete[] TextureInfos;
		TextureInfos = NULL;
	}
}


void BINKMovieClass::Update()
{
	if (!Bink) {
		return;
	}

	FrameChanged |= !BinkWait(Bink);
}


static unsigned char* Get_Tex_Address(unsigned char* buffer, int x, int y, int w, int h)
{
	if (x < 0) {
		x = 0;
	} else if (x >= w) {
		x = w - 1;
	}

	if (y < 0) {
		y = 0;
	} else if (y >= h) {
		y = h - 1;
	}

	return buffer + x * 2 + y * 2 * w;
}


void BINKMovieClass::Render()
{
	if (!Bink) {
		return;
	}

	// decompress a frame
	if (FrameChanged) {
		if (DX8Wrapper::Get_Current_Caps()->Get_Vendor() == DX8Caps::VENDOR_POWERVR) {
			if (TextureInfos == TextureInfos1) {
				TextureInfos = TextureInfos2;
				Free_Textures();
				Create_Textures();
				TextureInfos2 = TextureInfos;
			} else {
				TextureInfos = TextureInfos1;
				Free_Textures();
				Create_Textures();
				TextureInfos1 = TextureInfos;
			}
		}

		BinkDoFrame(Bink);
		FrameChanged = false;

		BinkCopyToBuffer(Bink, TempBuffer, Bink->Width * 2, Bink->Height, 0, 0, BINKSURFACE565|BINKCOPYNOSCALING);

		for (unsigned t = 0; t < TextureCount; ++t) {
			IDirect3DTexture8* d3d_texture = TextureInfos[t].Texture->Peek_D3D_Texture();

			if (d3d_texture) {
				unsigned char* cur_tex_ptr = Get_Tex_Address(TempBuffer, TextureInfos[t].TextureLocX,
					TextureInfos[t].TextureLocY, Bink->Width, Bink->Height);

				unsigned w = TextureInfos[t].TextureWidth;
				unsigned h = TextureInfos[t].TextureHeight;

				if (w > Bink->Width-TextureInfos[t].TextureLocX) {
					w = Bink->Width-TextureInfos[t].TextureLocX;
				}

				if (h > Bink->Height-TextureInfos[t].TextureLocY) {
					h = Bink->Height-TextureInfos[t].TextureLocY;
				}

				D3DSURFACE_DESC d3d_surf_desc;
				D3DLOCKED_RECT locked_rect;

				DX8_ErrorCode(d3d_texture->GetLevelDesc(0, &d3d_surf_desc));

				RECT rect;
				rect.left = 0;
				rect.top = 0;
				rect.right = w;
				rect.bottom = h;
				DX8_ErrorCode(d3d_texture->LockRect(0,&locked_rect,&rect,0));

				for (unsigned y = 0; y < h; ++y) {
					unsigned char* dest = (unsigned char*)locked_rect.pBits + y * locked_rect.Pitch;
					memcpy(dest, cur_tex_ptr, w * 2);
					cur_tex_ptr += Bink->Width * 2;
				}

				DX8_ErrorCode(d3d_texture->UnlockRect(0));
			}
		}

		if (Bink->FrameNum < Bink->Frames) // goto the next if not on the last
			BinkNextFrame(Bink);
	}

	for (unsigned t = 0; t < TextureCount; ++t) {
		Renderer->Reset();
		Renderer->Set_Texture(TextureInfos[t].Texture);
		Renderer->Set_Coordinate_Range(RectClass(0.0f, 0.0f, 1.0f, 1.0f));//Bink->Width,Bink->Height));

		RectClass rect(TextureInfos[t].TextureLocX, TextureInfos[t].TextureLocY, TextureInfos[t].TextureWidth, TextureInfos[t].TextureHeight);
		Renderer->Add_Quad(TextureInfos[t].Rect, TextureInfos[t].UV, 0xffffffff);
		Renderer->Render();
	}

	if (SubTitleManager) {
		unsigned long hundredths = (unsigned long)((Bink->FrameNum * 100) / ((float)Bink->FrameRate / Bink->FrameRateDiv));
		unsigned long seconds = hundredths / 100;
		unsigned long minutes = seconds / 60 / 60;
		unsigned long hours = minutes / 60;
		unsigned long movieTime = (((hours * 60) + minutes) * 60 + (seconds % 60)) * 60 + ((hundredths % 100) * 60) / 100;
		SubTitleManager->Process(movieTime);
		SubTitleManager->Render();
	}
}


bool BINKMovieClass::Is_Complete()
{
	if (!Bink) return true;
	return (Bink->FrameNum>=Bink->Frames);
}


void BINKMovieClass::Set_Volume(int volume)
{
	if (Bink) {
		BinkSetVolume(Bink, volume);
	}
}


// Earth & Beyond client (reconstruction): changes to Renegade's BINKMovie.cpp (frozen 2026-11-07 from importer.py)
// EnB: BINKMovieClass::BINKMovieClass (line 180 at the freeze): E&B's WW3D2 is Zero Hour's here: MipCountType is a
//       file-scope enum in texture.h (not TextureClass's)
// EnB: BINKMovieClass::Render (line 292 at the freeze): E&B's WW3D2 is Zero Hour's here:
//       TextureClass::Peek_D3D_Texture (texture.h), Renegade's Peek_DX8_Texture
// EnB: BINKMovieClass: 0x30 bytes (Play's new 0x30): no TicksPerFrame; the texture size kept (MaxWidth +0x10,
//       MaxHeight +0x14) and two texture sets (TextureInfos1 +0x1C, TextureInfos2 +0x20) beside the current one (+0x18);
//       the renderer a pointer (+0x28, new 0x5B4, Render2DClass's constructor); members Create_Textures,
//       Free_Textures and Set_Volume
// EnB: BINKMovie::Play (0xA161B0): a volume in place of the font; BinkSetVolume through Set_Volume (inlined)
// EnB: BINKMovieClass::BINKMovieClass (0xA162F0): no font argument; the texture loop moved to Create_Textures,
//       called twice (two sets); the subtitle manager is created from the subtitle name alone and reset
//       (0xA16444 calls SubTitleManagerClass::Create 0xA16E20 with one argument when it is not NULL, 0xA16455 Reset
//       0xA17150 on the result)
// EnB: BINKMovieClass::~BINKMovieClass (0xA16470): Bink, the buffer, both texture sets and the renderer released
//       and cleared; the subtitle manager is not deleted
// EnB: BINKMovieClass::Create_Textures (0xA16560) and Free_Textures (0xA167F0): new; the constructor's texture
//       loop and the destructor's release loop of Renegade's
// EnB: BINKMovieClass::Render (0xA16870): on a PowerVR card (DX8Caps VendorId +0x13C == 5) each new frame swaps to
//       the other texture set and recreates it; the renderer through its pointer; the subtitle time is the frame's
//       hundredths of a second turned into ticks (60 a second) as subtitleparser.cpp's Decode_Time_String counts them
// EnB: BINKMovieClass::Set_Volume (0xA16C90): new, BinkSetVolume (binkw32 import 0xC44460)
