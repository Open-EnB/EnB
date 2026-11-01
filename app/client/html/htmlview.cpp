// Earth & Beyond client (reconstruction): app\client\html\htmlview.cpp (PARTIAL). Notes at the end of the file.
#include <windows.h>
#include "htmlview.h"
#include "..\..\shared\unknown_debug.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

HtmlViewClass::HtmlViewClass(void) :
	Surf(NULL),
	Loader(NULL)
{
	Url[0] = '\0';
	Links.clear();
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	ScrollX = 0;
	ScrollY = 0;
	Dirty = 0;
}

HtmlViewClass::~HtmlViewClass(void)
{
}

bool HtmlViewClass::Clear(void)
{
	Links.clear();
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	ScrollX = 0;
	ScrollY = 0;
	return true;
}

bool HtmlViewClass::Open(char *url)
{
	Clear();
	strcpy(Url, url);
	if (Loader == NULL)
		return false;
	Loader->Open(url);
	return true;
}

bool HtmlViewClass::Set_Loader(HtmlLoaderClass *loader)
{
	Loader = loader;
	return true;
}

bool HtmlViewClass::Update(void)
{
	if (Loader != NULL)
		Loader->Update();
	bool dirty = false;
	if (Dirty)
		dirty = true;
	Dirty = 0;
	return dirty;
}

bool HtmlViewClass::Set_Surface(DSurface *surface)
{
	Surf = surface;
	Layout();
	return true;
}

bool HtmlViewClass::Clear_Links(void)
{
	Links.clear();
	return true;
}

bool HtmlViewClass::Clear_Blocks(void)
{
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	return true;
}

























































bool HtmlViewClass::Add_Data(char *name, char *url, char *data, int length, bool render)
{
	if (name != NULL && url != NULL && data != NULL && length != 0) {
		HtmlLinkStruct *link;
		if (!Find_Link(name, &link)) {
			HtmlLinkStruct entry;
			strcpy(entry.Url, url);
			strcpy(entry.Name, name);
			Links.addTail(entry);
			if (!Find_Link(name, &link))
				G_ASSERT(0);
		}
		if (link->Data.length() != 0)
			link->Data.remove(link->Data.length() - 1);
		link->Data.addMany(data, link->Data.length(), length);
		link->Data.addMany("", link->Data.length(), 1);
	}
	if (render) {
		Layout();
		Render(0, 0, 0, 0, Surf->Get_Width(), Surf->Get_Height());
	}
	return true;
}

void HtmlViewClass::Layout(void)
{
	HtmlBlockClass *block;
	while (Blocks.removeTail(block)) {
		delete block;
	}

	for (int i = 0; i < Links.length(); i++) {
		HtmlLinkStruct *link;
		if (!Links.getPointer(&link, i))
			link = NULL;
		if (strcmp(link->Name, Url) == 0) {
			HtmlAttributeStackClass attributes;
			char buffer[64];
			sprintf(buffer, "%d", 10);
			attributes.Set("Horizontal_Padding", buffer);
			sprintf(buffer, "%d", 16);
			attributes.Set("Vertical_Padding", buffer);
			block = new HtmlBlockClass(NULL, Surf->Get_Width(), Surf->Get_Height(), &attributes, NULL);
			block->AbsX = 0;
			block->AbsY = 0;
			Blocks.addTail(block);
			Parse(link, block, 0);
			Dirty = 1;
			return;
		}
	}
}

static inline bool Has_Graph(char *str)
{
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (isgraph(str[i]))
			return true;
	}
	return false;
}





































































































































































































































































static int TableDepth;  // NAME 1: 0xBE83F0, counted up by <table>, down by </table>; nothing reads it
static int RowDepth;  // NAME 1: 0xBE83F4, the same for <tr>
static int CellDepth;  // NAME 1: 0xBE83F8, the same for <td>

static int Get_Value(int offset, Wstring &tag, Wstring &value);  // 0x7784D0

// An HTML colour (#RRGGBB) as a COLORREF (0x00BBGGRR), its digits read three times as the client does.
#define HTML_COLORREF(str, end) \
	(((strtoul((str) + 1, &(end), 16) & 0xFF) << 16) | (strtoul((str) + 1, &(end), 16) >> 16) | \
	(strtoul((str) + 1, &(end), 16) & 0xFF00))

// NOT EXACT: 0x7710D0, functional (D64): written from the pseudocode; 18 KB, not yet compared form by form
int HtmlViewClass::Parse(HtmlLinkStruct *link, HtmlBlockClass *block, int offset)
{
	char *start;
	if (!link->Data.getPointer(&start, offset))
		start = NULL;
	char *cptr = start;
	int consumed = 0;
	ArrayList<HtmlBlockClass *> rowcells;
	ArrayList<int> colwidths;
	int column = 0;
	HtmlAttributeStackClass tableattrs;
	HtmlAttributeStackClass rowattrs;
	char buffer[128];
	char *end;

	if (block == NULL)
		return 0;

	if (strcmp(link->Url, "image/gif") != 0) {
		for (;;) {
			char *tagend = NULL;
			consumed = cptr - start;
			if (*cptr == '\0')
				break;
			char *tagstart = strchr(cptr, '<');
			if (tagstart != NULL) {
				tagend = tagstart;
				while ((tagend = strchr(tagend, '>')) != NULL) {
					if (strncmp(tagstart, "<!--", 4) != 0 || strncmp(tagend - 2, "-->", 3) == 0)
						break;
					tagend++;
				}
			}
			if (tagend == NULL) {
				if (strlen(cptr) != 0) {
					Add_Text(block, cptr, strlen(cptr), block->Width - block->HPadding, -1);
				}
				break;
			}
			if (cptr < tagstart && strlen(cptr) != 0) {
				Add_Text(block, cptr, tagstart - cptr, block->Width - block->HPadding, -1);
				cptr = tagstart;
				continue;
			}

			Wstring tag;
			Wstring name;
			tag.set(tagend - tagstart - 1, tagstart + 1);
			int pos = tag.getToken(0, " \t", name);
			name.toLower();
			int fontheight = block->Get_Font_Height(NULL);

			if (strcmp(name.get(), "p") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(20, false);
				while (block->Pop_Style("Align", "p.left")) {}
				while (block->Pop_Style("Align", "p.right")) {}
				while (block->Pop_Style("Align", "p.center")) {}
				Wstring key;
				Wstring value;
				while ((pos = tag.getToken(pos, " =\t\r\n", key)) != -1 && (pos = Get_Value(pos, tag, value)) != -1) {
					key.toLower();
					if (strcmp(key.get(), "align") == 0) {
						char align[64];
						sprintf(align, "p.%s", value.get());
						block->Push_Style("Align", align);
					}
				}
			} else if (strcmp(name.get(), "/p") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(20, false);
				while (block->Pop_Style("Align", "p.left")) {}
				while (block->Pop_Style("Align", "p.right")) {}
				while (block->Pop_Style("Align", "p.center")) {}
			} else if (strcmp(name.get(), "font") == 0) {
				Wstring key;
				Wstring value;
				while ((pos = tag.getToken(pos, " =\t\r\n", key)) != -1 && (pos = Get_Value(pos, tag, value)) != -1) {
					key.toLower();
					if (strcmp(key.get(), "face") == 0) {
						block->Set_Style("Font.Face", value.get());
					} else if (strcmp(key.get(), "size") == 0) {
						block->Set_Style("Font.BaseSize", value.get());
					} else if (strcmp(key.get(), "color") == 0) {
						end = NULL;
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						block->Set_Style("Font.Colorref", buffer);
					}
				}
			} else if (strcmp(name.get(), "/font") == 0) {
				block->Remove_Style("Font.Face");
				block->Remove_Style("Font.BaseSize");
				block->Remove_Style("Font.Colorref");
			} else if (strcmp(name.get(), "br") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(5, false);
			} else if (strcmp(name.get(), "a") == 0) {
				Wstring key;
				pos = tag.getToken(pos, " =\t\r\n", key);
				if (strcmp(key.get(), "href") == 0) {
					Wstring value;
					if (Get_Value(pos, tag, value) != -1) {
						block->Set_Style("Anchor.HREF", value.get());
					}
				}
			} else if (strcmp(name.get(), "/a") == 0) {
				block->Remove_Style("Anchor.HREF");
			} else if (strcmp(name.get(), "title") == 0) {
				block->Set_Attribute("TitleFlag", "1");
			} else if (strcmp(name.get(), "/title") == 0) {
				block->Remove_Attribute("TitleFlag");
			} else if (strcmp(name.get(), "i") == 0) {
				block->Set_Style("Font.Italic", "true");
			} else if (strcmp(name.get(), "/i") == 0) {
				block->Remove_Style("Font.Italic");
			} else if (strcmp(name.get(), "b") == 0 || strcmp(name.get(), "strong") == 0) {
				block->Set_Style("Font.Bold", "true");
			} else if (strcmp(name.get(), "/b") == 0 || strcmp(name.get(), "/strong") == 0) {
				block->Remove_Style("Font.Bold");
			} else if (strcmp(name.get(), "center") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(0, false);
				block->Set_Style("Align", "center");
			} else if (strcmp(name.get(), "/center") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(0, false);
				block->Remove_Style("Align");
			} else if (strcmp(name.get(), "table") == 0) {
				TableDepth++;
				colwidths.clear();
				tableattrs.Add_All(block->Attributes);
				tableattrs.Remove("BackgroundPointer");
				tableattrs.Remove("BackgroundName");
				tableattrs.Remove("BackgroundColorref");
				tableattrs.Remove("Horizontal_Padding");
				tableattrs.Remove("Vertical_Padding");
				block->CursorX = block->RightPadding;
				block->Advance_Y(20, false);
				int spacing = 0;
				Wstring key;
				Wstring value;
				while ((pos = tag.getToken(pos, " =\t\r\n", key)) != -1 && (pos = Get_Value(pos, tag, value)) != -1) {
					key.toLower();
					if (strcmp(key.get(), "bgcolor") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						tableattrs.Set("BackgroundColorref", buffer);
					}
					if (strcmp(key.get(), "text") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						tableattrs.Set("TextColorref", buffer);
					}
					if (strcmp(key.get(), "link") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						tableattrs.Set("LinkColorref", buffer);
					}
					if (strcmp(key.get(), "width") == 0) {
						int width = atol(value.get());
						if (value.get()[value.length() - 1] == '%')
							width = (block->Width - block->HPadding * 2) * width / 100;
						sprintf(buffer, "%d", width);
						tableattrs.Set("Width", buffer);
					}
					if (strcmp(key.get(), "cellpadding") == 0) {
						tableattrs.Set("Horizontal_Padding", value.get());
						tableattrs.Set("Vertical_Padding", value.get());
					}
					if (strcmp(key.get(), "cellspacing") == 0) {
						spacing = atol(value.get()) / 2 * 2;
						sprintf(buffer, "%d", spacing);
						tableattrs.Set("CellSpacing", buffer);
					}
					if (strcmp(key.get(), "border") == 0) {
						tableattrs.Set("Border", value.get());
					}
				}
				char *str = tableattrs.Get("Width");
				int avail = block->Width - block->HPadding * 2;
				if (str == NULL || atol(str) > avail) {
					sprintf(buffer, "%d", avail);
					tableattrs.Set("Width", buffer);
					str = tableattrs.Get("Width");
				}
				int width = atol(str);
				int align = block->Get_Align();
				if (align == 0) {
					block->Advance_X((block->Width - width) / 2, true);
					block->RightPadding += (block->Width - width) / 2;
				} else if (align == 1) {
					block->Advance_X(block->Width - width, true);
					block->RightPadding += block->Width - width;
				}
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = block->CursorX;
				item->Y = block->CursorY;
				item->Width = 0;
				item->Height = 0;
				item->Attributes.Add_All(tableattrs);
				item->Attributes.Set("Type", "Table");
				block->Items.add(item, block->Items.length());
				sprintf(buffer, "%p", item);
				tableattrs.Set("ElementPointer", buffer);
				str = tableattrs.Get("Border");
				if (str != NULL) {
					int border = atol(str);
					int inset = spacing / 2 + border;
					block->Advance_Y(inset, true);
					block->Advance_X(inset, true);
					block->RightPadding += spacing / 2 + border;
					str = tableattrs.Get("Width");
					if (str != NULL) {
						sprintf(buffer, "%d", atol(str) - border * 2 - spacing / 2);
						tableattrs.Set("Width", buffer);
					}
				}
			} else if (strcmp(name.get(), "/table") == 0) {
				TableDepth--;
				int border = 0;
				char *str = tableattrs.Get("Border");
				if (str != NULL) {
					border = atol(str);
					block->RightPadding -= border;
					block->Advance_X(-border, true);
					block->Advance_Y(border, true);
				}
				int spacing = 0;
				str = tableattrs.Get("CellSpacing");
				if (str != NULL)
					spacing = atol(str);
				HtmlItemStruct *item = NULL;
				str = tableattrs.Get("ElementPointer");
				if (str != NULL) {
					sscanf(str, "%p", &item);
					spacing /= 2;
					item->Width = spacing + atol(tableattrs.Get("Width")) + border * 2;
					item->Height = spacing - item->Y + block->CursorY;
					tableattrs.Clear();
					colwidths.clear();
					block->Advance_Y(0, true);
					block->RightPadding -= spacing;
					int align = block->Get_Align();
					if (align == 0) {
						block->RightPadding -= (block->Width - item->Width) / 2;
					} else if (align == 1) {
						block->RightPadding -= block->Width - item->Width;
					}
					block->CursorX = block->RightPadding;
				}
			} else if (strcmp(name.get(), "tr") == 0) {
				RowDepth++;
				rowcells.clear();
				rowattrs.Clear();
				column = 0;
				char *str = tableattrs.Get("CellSpacing");
				if (str != NULL)
					atol(str);
				tableattrs.Get("Border");
			} else if (strcmp(name.get(), "/tr") == 0) {
				RowDepth--;
				int height = 1;
				int width = 0;
				int spacing = 0;
				int border = 0;
				char *str = tableattrs.Get("CellSpacing");
				if (str != NULL)
					spacing = atol(str);
				str = tableattrs.Get("Border");
				if (str != NULL && atol(str) != 0)
					border = 1;
				HtmlBlockClass *cell;
				int i;
				for (i = 0; i < rowcells.length(); i++) {
					rowcells.get(cell, i);
					if (cell->Unknown1C4 > height)
						height = cell->Unknown1C4;
					width += cell->Width + spacing + border * 2;
				}
				for (i = 0; i < rowcells.length(); i++) {
					rowcells.get(cell, i);
					cell->Height = height;
					HtmlItemStruct *item = NULL;
					sscanf(cell->Get_Style("ElementPointer"), "%p", &item);
					item->Height = height + border * 2;
				}
				block->RightPadding -= width;
				block->CursorX = block->RightPadding;
				block->Advance_Y(spacing + border * 2 + height, true);
				rowcells.clear();
				rowattrs.Clear();
			} else if (strcmp(name.get(), "td") == 0) {
				CellDepth++;
				HtmlAttributeStackClass cellattrs;
				Wstring key;
				Wstring value;
				while ((pos = tag.getToken(pos, " =\t\r\n", key)) != -1 && (pos = Get_Value(pos, tag, value)) != -1) {
					key.toLower();
					if (strcmp(key.get(), "bgcolor") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						cellattrs.Set("BackgroundColorref", buffer);
					}
					if (strcmp(key.get(), "width") == 0) {
						int width = atol(value.get());
						if (value.get()[value.length() - 1] == '%') {
							int full = block->Width;
							char *str = tableattrs.Get("Width");
							if (str != NULL)
								full = atol(str);
							width = full * width / 100;
						}
						sprintf(buffer, "%d", width);
						cellattrs.Set("Width", buffer);
					}
					if (strcmp(key.get(), "height") == 0) {
						cellattrs.Set("Height", value.get());
					}
					if (strcmp(key.get(), "colspan") == 0) {
						cellattrs.Set("ColSpan", value.get());
					}
				}
				int spacing = 0;
				int border = 0;
				char *str = tableattrs.Get("CellSpacing");
				if (str != NULL)
					spacing = atol(str);
				str = tableattrs.Get("Border");
				if (str != NULL && atol(str) > 0) {
					sprintf(buffer, "%d", 1);
					cellattrs.Set("Border", buffer);
					border = 1;
				}
				HtmlAttributeStackClass attributes;
				attributes.Add_All(tableattrs);
				attributes.Add_All(rowattrs);
				attributes.Add_All(cellattrs);
				int width = 50;
				str = tableattrs.Get("Width");
				if (str != NULL)
					width = atol(str);
				str = cellattrs.Get("Width");
				if (str != NULL)
					width = atol(str);
				int colspan = 1;
				str = cellattrs.Get("ColSpan");
				if (str != NULL)
					colspan = atol(str);
				int colwidth = width / colspan;
				int i;
				for (i = 0; i < colspan; i++) {
					if (column + i < 0 || column + i >= colwidths.length())
						colwidths.add(colwidth, column + i);
				}
				int cellwidth = 0;
				int w = 0;
				for (i = 0; i < colspan; i++) {
					colwidths.get(w, column + i);
					cellwidth += w;
				}
				column += colspan;
				HtmlBlockClass *cell = new HtmlBlockClass((void *)1, cellwidth - border * 2 - spacing, 100, &attributes, NULL);
				block->Add_Child(cell);
				int half = spacing / 2;
				cell->AbsX = block->CursorX + border + half;
				cell->AbsY = border + half + block->CursorY;
				Blocks.addTail(cell);
				rowcells.addTail(cell);
				int length = Parse(link, cell, tagend - start + offset + 1);
				cell->Height = cell->Unknown1C4;
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = half + block->CursorX;
				item->Y = half + block->CursorY;
				item->Width = cell->Width + border * 2;
				item->Height = cell->Height + border * 2;
				item->Attributes.Add_All(cellattrs);
				item->Attributes.Set("Type", "Cell");
				block->Items.addTail(item);
				sprintf(buffer, "%p", item);
				cell->Set_Style("ElementPointer", buffer);
				block->RightPadding += cellwidth;
				block->Advance_X(cellwidth, false);
				tagend += length;
			} else if (strcmp(name.get(), "/td") == 0) {
				CellDepth--;
				consumed = tagend + 1 - start;
				break;
			} else if (strcmp(name.get(), "h1") == 0) {
				if (block->CursorX != block->RightPadding)
					block->Advance_Y(0, false);
				block->CursorX = block->RightPadding;
				block->Push_Style("Font.HSize", "1");
			} else if (strcmp(name.get(), "h2") == 0) {
				if (block->CursorX != block->RightPadding)
					block->Advance_Y(0, false);
				block->CursorX = block->RightPadding;
				block->Push_Style("Font.HSize", "2");
			} else if (strcmp(name.get(), "h3") == 0) {
				if (block->CursorX != block->RightPadding)
					block->Advance_Y(0, false);
				block->CursorX = block->RightPadding;
				block->Push_Style("Font.HSize", "3");
			} else if (strcmp(name.get(), "h4") == 0) {
				if (block->CursorX != block->RightPadding)
					block->Advance_Y(0, false);
				block->CursorX = block->RightPadding;
				block->Push_Style("Font.HSize", "4");
			} else if (strncmp(name.get(), "/h", 2) == 0 && isdigit(name.get()[2])) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(20, false);
				block->Pop_Style("Font.HSize", NULL);
			} else if (strcmp(name.get(), "body") == 0) {
				Wstring key;
				Wstring value;
				while ((pos = tag.getToken(pos, " =\t\r\n", key)) != -1 && (pos = Get_Value(pos, tag, value)) != -1) {
					key.toLower();
					if (strcmp(key.get(), "bgcolor") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						block->Set_Attribute("BackgroundColorref", buffer);
					} else if (strcmp(key.get(), "text") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						block->Set_Attribute("TextColorref", buffer);
					} else if (strcmp(key.get(), "link") == 0) {
						sprintf(buffer, "%d", HTML_COLORREF(value.get(), end));
						block->Set_Attribute("LinkColorref", buffer);
					} else if (strcmp(key.get(), "background") == 0) {
						block->Set_Attribute("BackgroundName", value.get());
						HtmlImageClass *image;
						if (Find_Image(value.get(), &image)) {
							sprintf(buffer, "%p", image);
							block->Set_Attribute("BackgroundPointer", buffer);
							continue;
						}
						image = new HtmlGifImageClass(value.get(), Surf->Bytes_Per_Pixel() << 3);
						sprintf(buffer, "%p", image);
						block->Set_Attribute("BackgroundPointer", buffer);
						Images.addTail(image);
						Loader->Open(value.get());
						HtmlLinkStruct *data;
						if (Find_Link(value.get(), &data)) {
							if (data->Data.length() > 0) {
								char *bytes;
								data->Data.getPointer(&bytes, 0);
								image->Set_Data(bytes, data->Data.length());
							}
							continue;
						}
						G_ASSERT(0);
					}
				}
			} else if (strcmp(name.get(), "hr") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(5, false);
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = block->CursorX;
				item->Y = block->CursorY;
				item->Height = 4;
				item->Width = block->Width - block->HPadding - item->X;
				item->Attributes.Set("Type", "HR");
				block->Items.addTail(item);
				block->CursorX = block->RightPadding;
				block->Advance_Y(5, false);
			} else if (strcmp(name.get(), "ul") == 0) {
				block->RightPadding += 32;
				block->Advance_X(32, false);
				if (block->RightPadding == block->HPadding + 32)
					block->Advance_Y(5, false);
			} else if (strcmp(name.get(), "/ul") == 0) {
				if (block->RightPadding >= block->HPadding + 32) {
					block->RightPadding -= 32;
					if (block->CursorX >= block->HPadding + 32)
						block->Advance_X(-32, false);
				}
				block->CursorX = block->RightPadding;
				if (block->RightPadding == block->HPadding)
					block->Advance_Y(20, false);
			} else if (strcmp(name.get(), "li") == 0) {
				block->CursorX = block->RightPadding;
				block->Advance_Y(4, false);
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = block->CursorX - 10;
				item->Y = fontheight / 2 + block->CursorY;
				item->Width = 4;
				item->Height = 4;
				item->Attributes.Set("Type", "LI");
				block->Items.addTail(item);
			} else if (strcmp(name.get(), "img") == 0) {
				Wstring key;
				Wstring value;
				while ((pos = tag.getToken(pos, " =\t\r\n", key)) != -1 && (pos = Get_Value(pos, tag, value)) != -1) {
					key.toLower();
					if (strcmp(key.get(), "src") == 0) {
						HtmlImageClass *image;
						if (!Find_Image(value.get(), &image)) {
							image = new HtmlGifImageClass(value.get(), Surf->Bytes_Per_Pixel() << 3);
							Images.addTail(image);
							Loader->Open(value.get());
							HtmlLinkStruct *data;
							if (Find_Link(value.get(), &data) && data->Data.length() > 0) {
								char *bytes;
								data->Data.getPointer(&bytes, 0);
								image->Set_Data(bytes, data->Data.length());
							}
						}
						HtmlItemStruct *item = new HtmlItemStruct;
						int width = 32;
						int height = 32;
						item->X = block->CursorX;
						item->Y = block->CursorY;
						if (image->Get_Surface() != NULL) {
							width = image->Get_Surface()->Get_Width();
							height = image->Get_Surface()->Get_Height();
						}
						item->Height = height;
						item->Width = width;
						item->Attributes.Add_All(block->Styles);
						item->Attributes.Set("Type", "Image");
						item->Attributes.Set("URL", value.get());
						block->Items.addTail(item);
						block->Advance_X(width, true);
					}
					if (strcmp(key.get(), "alt") == 0) {
						block->Set_Style("Img.Alt", value.get());
					}
				}
				block->Remove_Style("Img.Alt");
			}
			cptr = tagend + 1;
		}
		block->Advance_X(0, false);
		block->Advance_Y(0, false);
	}

	int bottom = 0;
	int right = 0;
	int i;
	for (i = 0; i < block->Items.length(); i++) {
		HtmlItemStruct *item = block->Get_Item(i);
		if (item->Height + item->Y > bottom)
			bottom = item->Height + item->Y;
	}
	for (i = 0; i < block->Items.length(); i++) {
		HtmlItemStruct *item = block->Get_Item(i);
		if (item->Width + item->X > right)
			right = item->Width + item->X;
	}
	block->Unknown1C4 = block->VPadding + 2 + bottom;
	block->Unknown1C0 = block->HPadding + right;
	return consumed;
}















































































// NOT EXACT: 0x7759E0, register allocation (widest spilled, one more frame slot than the client's 0x34)
bool HtmlViewClass::Add_Text(HtmlBlockClass *block, char *cptr_in, int maxlength, int maxwidth, int)
{
	int startx = block->CursorX;
	int x = startx;
	int y = block->CursorY;
	G_ASSERT(maxlength > 0);
	G_ASSERT(cptr_in);

	if (startx == block->RightPadding) {
		while (*cptr_in == ' ' || *cptr_in == '\n' || *cptr_in == '\t' || *cptr_in == '\r') {
			cptr_in++;
			maxlength--;
		}
	}
	if (maxlength == 0)
		return false;

	Wstring text;
	text.set(maxlength, cptr_in);
	text.replace('\n', ' ');
	text.replace('\r', ' ');
	text.replace('\t', ' ');
	char *p = text.get();
	for (p = strstr(p, "  "); p != NULL; p = strstr(p, "  ")) {
		int pos = p - text.get();
		int count = 0;
		while (p[count + 1] == ' ')
			count++;
		text.remove(pos, count);
	}

	if (!Has_Graph(text.get()))
		return false;

	text.replace("&nbsp;", " ");
	cptr_in = text.get();
	maxlength = text.length();
	if (block->Get_Attribute("TitleFlag") != NULL) {
		block->Set_Attribute("Title", cptr_in);
		return false;
	}
	if (maxlength == 0)
		return false;

	HDC hdc = Surf->GetDC();
	if (hdc == NULL)
		return false;

	int height = block->Get_Font_Height(NULL);
	int flags = 0;
	if (block->Get_Style("Font.Bold") != NULL)
		flags = 1;
	if (block->Get_Style("Font.Italic") != NULL)
		flags |= 2;
	char *face = block->Get_Style("Font.Face");
	if (face == NULL)
		face = "Times New Roman";
	HFONT font = Get_Font(hdc, face, 0, height - 2, flags);
	int widest = 0;
	if (font != NULL)
		SelectObject(hdc, font);

	int lines = 0;
	char *line = cptr_in;
	SIZE size;
	while (true) {
		int n = 1;
		while (true) {
			GetTextExtentPoint32(hdc, cptr_in, n, &size);
			if (widest < size.cx && size.cx + x < maxwidth)
				widest = size.cx;
			if (size.cx + x >= maxwidth || n >= maxlength)
				break;
			n++;
		}
		if (size.cx + x >= maxwidth || n != maxlength)
			n--;
		if (n < 1)
			n = 1;
		int found = -1;
		for (int k = n; k > 1 && n != maxlength; k--) {
			if (!isgraph(cptr_in[k - 1])) {
				found = k;
				break;
			}
		}
		if (found != -1)
			n = found;
		GetTextExtentPoint32(hdc, cptr_in, n, &size);
		widest = size.cx;
		lines++;
		if (lines == 1 || n == maxlength || startx != block->RightPadding) {
			HtmlItemStruct *item = new HtmlItemStruct;
			item->X = x;
			item->Width = widest;
			item->Height = lines * height;
			item->Y = y;
			item->Attributes.Add_All(block->Styles);
			item->Attributes.Set("Type", "Text");
			item->Attributes.Set("Text", line, cptr_in + n - line);
			block->Items.addTail(item);
			if (n == maxlength) {
				x = block->Advance_X(size.cx, false);
				if (lines > 1)
					y = block->Advance_Y(0, false);
			} else {
				x = block->RightPadding;
				block->CursorX = x;
				y = block->Advance_Y(0, false);
			}
			lines = 0;
			line = cptr_in + n;
		}
		cptr_in += n;
		maxlength -= n;
		if (maxlength == 0)
			break;
	}
	Surf->ReleaseDC(hdc);
	return true;
}

// NOT EXACT: 0x7761E0, the parent bounds' stores scheduled later than the client's (the clip RECT, bottom first, matches)
bool HtmlViewClass::Render(int scrollx, int scrolly, int x, int y, int width, int height)
{
	if (Surf == NULL)
		return false;

	ScrollX = scrollx;
	ScrollY = scrolly;

	RECT clip;
	clip.bottom = y + height;
	clip.top = y;
	clip.left = x;
	clip.right = x + width;

	HtmlBlockClass *block = NULL;
	for (int i = 0; i < Blocks.length(); i++) {
		Blocks.get(block, i);
		if (i == 0) {
			block->Set_X(scrollx);
			block->Set_Y(scrolly);
		}
		RECT rect = clip;
		HtmlBlockClass *parent = block->Parent;
		if (parent != NULL) {
			RECT bounds;
			bounds.left = parent->AbsX;
			bounds.top = parent->AbsY;
			bounds.right = parent->Get_Width() + bounds.left;
			bounds.bottom = parent->Get_Height() + bounds.top;
			if (!IntersectRect(&rect, &clip, &bounds))
				continue;
		}
		Draw_Block(block, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	return true;
}

bool HtmlViewClass::Draw_Block(HtmlBlockClass *block, int x, int y, int width, int height)
{
	if (Surf == NULL)
		return false;

	RECT clip;
	clip.bottom = y + height;
	clip.top = y;
	clip.left = x;
	clip.right = x + width;
	RECT bounds;
	bounds.bottom = block->Get_Height() + block->Get_Abs_Y();
	bounds.top = block->Get_Abs_Y();
	bounds.left = block->Get_Abs_X();
	bounds.right = block->Get_Width() + block->Get_Abs_X();
	RECT draw;
	if (!IntersectRect(&draw, &clip, &bounds))
		return false;

	HtmlImageClass *image = NULL;
	char *pointer = block->Get_Attribute("BackgroundPointer");
	if (pointer != NULL)
		sscanf(pointer, "%p", &image);
	if (image == NULL || image->Get_Surface() == NULL) {
		COLORREF color = 0xC0C0C0;
		char *colorref = block->Get_Attribute("BackgroundColorref");
		if (colorref != NULL || block->Owner == NULL) {
			HDC hdc = Surf->GetDC();
			if (colorref != NULL)
				color = atol(colorref);
			HBRUSH brush = CreateSolidBrush(color);
			FillRect(hdc, &draw, brush);
			DeleteObject(brush);
			Surf->ReleaseDC(hdc);
		}
	} else {
		int tilew = image->Get_Surface()->Get_Width();
		int tileh = image->Get_Surface()->Get_Height();
		for (int tx = bounds.left - tilew; tx <= bounds.right + tilew; tx += tilew) {
			for (int ty = bounds.top - tileh; ty <= bounds.bottom + tileh; ty += tileh) {
				image->Draw(Surf, tx - block->X % tilew, ty - block->Y % tileh, draw.left, draw.top, draw.right - draw.left, draw.bottom - draw.top);
			}
		}
	}

	for (int i = 0; i < block->Items.length(); i++) {
		HtmlItemStruct *item = block->Get_Item(i);
		RECT rect;
		rect.bottom = item->Y + item->Height - block->Get_Y() + block->Get_Abs_Y();
		rect.top = item->Y - block->Get_Y() + block->Get_Abs_Y();
		rect.left = item->X - block->Get_X() + block->Get_Abs_X();
		rect.right = item->X + item->Width - block->Get_X() + block->Get_Abs_X();
		RECT visible;
		if (IntersectRect(&visible, &draw, &rect)) {
			Draw_Item(block, item, block->Get_X() - block->Get_Abs_X(), block->Get_Y() - block->Get_Abs_Y(), draw.left, draw.top, draw.right - draw.left, draw.bottom - draw.top);
		}
	}
	Unknown13C = 4000;
	Unknown138 = 1000;
	return true;
}

// NOT EXACT: 0x7767A0, the client saves ebx / esi only after the Type test (its early returns pop two
// registers); the text loop's loads are scheduled differently
bool HtmlViewClass::Draw_Item(HtmlBlockClass *block, HtmlItemStruct *item, int offx, int offy, int clipx, int clipy, int clipw, int cliph)
{
	if (Surf == NULL)
		return false;

	char *type = item->Attributes.Get("Type");
	if (type == NULL)
		return true;

	if (strcmp(type, "Text") == 0) {
		char *text = item->Attributes.Get("Text");
		HDC hdc = Surf->GetDC();
		COLORREF color = 0;
		char *value = item->Attributes.Get("Font.Colorref");
		if (value != NULL || (value = block->Get_Attribute("TextColorref")) != NULL)
			color = atol(value);
		int flags = 0;
		if (item->Attributes.Get("Anchor.HREF") != NULL) {
			color = 0xB00000;
			char *link = block->Get_Attribute("LinkColorref");
			if (link != NULL)
				color = atol(link);
			flags = 4;
		}
		SetTextColor(hdc, color);
		SetBkMode(hdc, TRANSPARENT);
		if (item->Attributes.Get("Font.Bold") != NULL)
			flags |= 1;
		if (item->Attributes.Get("Font.Italic") != NULL)
			flags |= 2;
		int height = block->Get_Font_Height(item);
		char *face = item->Attributes.Get("Font.Face");
		if (face == NULL)
			face = "Times New Roman";
		HFONT font = Get_Font(hdc, face, 0, height - 2, flags);
		if (font != NULL)
			SelectObject(hdc, font);
		HRGN region = CreateRectRgn(clipx, clipy, clipw + clipx, clipy + cliph);
		SelectClipRgn(hdc, region);
		DeleteObject(region);

		int length = strlen(text);
		int y = item->Y - offy;
		int itemh = item->Height;
		int x = item->X - offx;
		int right = item->Width + x;
		int widest = 0;
		int dy = 0;
		while (length != 0) {
			SIZE size;
			int n = 1;
			while (true) {
				GetTextExtentPoint32(hdc, text, n, &size);
				if (size.cx > widest && size.cx + x < right)
					widest = size.cx;
				if (size.cx + x > right || n >= length)
					break;
				n++;
			}
			if (size.cx + x > right || n != length)
				n--;
			if (n < 1)
				n = 1;
			int found = -1;
			for (int k = n; k > 1 && n != length; k--) {
				if (!isgraph(text[k - 1])) {
					found = k;
					break;
				}
			}
			if (found != -1)
				n = found;
			SetTextAlign(hdc, TA_BASELINE);
			TextOut(hdc, x, dy + y - 2 + itemh, text, n);
			dy += height;
			text += n;
			length -= n;
		}
		Surf->ReleaseDC(hdc);
		return true;
	}

	if (strcmp(type, "HR") == 0) {
		Draw_Bevel(item->X - offx, item->Y - offy, item->Width, item->Height, clipx, clipy, clipw, cliph, 1, true);
		return true;
	}
	if (strcmp(type, "LI") == 0) {
		HDC hdc = Surf->GetDC();
		COLORREF color = 0;
		char *value = item->Attributes.Get("Font.Colorref");
		if (value != NULL || (value = block->Get_Attribute("TextColorref")) != NULL)
			color = atol(value);
		HBRUSH brush = CreateSolidBrush(color);
		RECT rect;
		rect.left = item->X - offx;
		rect.right = item->Width - offx + item->X;
		rect.top = item->Y - offy;
		rect.bottom = item->Height - offy + item->Y;
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);
		Surf->ReleaseDC(hdc);
		return true;
	}
	if (strcmp(type, "Table") == 0) {
		char *border = item->Attributes.Get("Border");
		if (border != NULL) {
			Draw_Bevel(item->X - offx, item->Y - offy, item->Width, item->Height, clipx, clipy, clipw, cliph, atol(border), true);
			return true;
		}
	} else if (strcmp(type, "Cell") == 0) {
		char *border = item->Attributes.Get("Border");
		if (border != NULL) {
			Draw_Bevel(item->X - offx, item->Y - offy, item->Width, item->Height, clipx, clipy, clipw, cliph, atol(border), false);
			return true;
		}
	} else if (strcmp(type, "Image") == 0) {
		char *url = item->Attributes.Get("URL");
		for (int i = 0; i < Images.length(); i++) {
			HtmlImageClass *image;
			Images.get(image, i);
			if (strcmp(image->Get_Name(), url) == 0) {
				image->Draw(Surf, item->X - offx, item->Y - offy, clipx, clipy, clipw, cliph);
				return true;
			}
		}
	}
	return true;
}

char *HtmlViewClass::Click(int x, int y, int)
{
	static char href[256];

	for (int i = Blocks.length() - 1; i >= 0; i--) {
		HtmlBlockClass *block = NULL;
		Blocks.get(block, i);
		int bx = block->Get_X() + x - block->Get_Abs_X();
		int by = block->Get_Y() + y - block->Get_Abs_Y();
		for (int j = 0; j < block->Items.length(); j++) {
			HtmlItemStruct *item = block->Get_Item(j);
			if (bx >= item->X && bx <= item->X + item->Width && by >= item->Y && by <= item->Y + item->Height) {
				char *value = item->Attributes.Get("Anchor.HREF");
				if (value == NULL)
					return NULL;
				strcpy(href, value);
				Open(href);
				return href;
			}
		}
	}
	return NULL;
}

// NOT EXACT: 0x7772C0, the loop top reloads this before i (the client i first); Click's twin
char *HtmlViewClass::Hit_Test(int x, int y, int *type, int *index)
{
	for (int i = Blocks.length() - 1; i >= 0; i--) {
		HtmlBlockClass *block = NULL;
		Blocks.get(block, i);
		int bx = block->Get_X() + x - block->Get_Abs_X();
		int by = block->Get_Y() + y - block->Get_Abs_Y();
		for (int j = 0; j < block->Items.length(); j++) {
			HtmlItemStruct *item = block->Get_Item(j);
			if (bx >= item->X && bx <= item->X + item->Width && by >= item->Y && by <= item->Y + item->Height) {
				*index = j;
				char *value = item->Attributes.Get("Anchor.HREF");
				if (value != NULL) {
					*type = 2;
					return value;
				}
				*type = 1;
				return NULL;
			}
		}
	}
	*type = 1;
	*index = -1;
	return NULL;
}

bool HtmlViewClass::Find_Link(char *name, HtmlLinkStruct **link)
{
	for (int i = 0; i < Links.length(); i++) {
		Links.getPointer(link, i);
		if (strcmp((*link)->Name, name) == 0)
			return true;
	}
	*link = NULL;
	return false;
}

bool HtmlViewClass::Find_Image(char *name, HtmlImageClass **image)
{
	for (int i = 0; i < Images.length(); i++) {
		HtmlImageClass *entry;
		Images.get(entry, i);
		if (strcmp(entry->Get_Name(), name) == 0) {
			*image = entry;
			return true;
		}
	}
	*image = NULL;
	return false;
}

int HtmlViewClass::Get_Unknown13C(void)
{
	return Unknown13C;
}

int HtmlViewClass::Get_Unknown138(void)
{
	return Unknown138;
}

char *HtmlViewClass::Get_Title(void)
{
	char *title = "";
	if (Blocks.length() != 0) {
		HtmlBlockClass *block = NULL;
		Blocks.get(block, 0);
		title = block->Get_Attribute("Title");
		if (title == NULL)
			title = "";
	}
	return title;
}

void HtmlViewClass::Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised)
{
	int light = 0xFFFFFF;
	int dark = 0;
	if (!raised) {
		light = 0;
		dark = 0xFFFFFF;
	}

	for (int i = 0; i < thickness; i++) {
		Draw_Line(x + i - 1, y + i, x + width - 1 - i - 1, y + i, clipx, clipy, clipw, cliph, light, 0x7F);
		Draw_Line(x + i, y + height - 1 - i, x + width - 1 - i, y + height - 1 - i, clipx, clipy, clipw, cliph, dark, 0x7F);
		Draw_Line(x + i, y + i, x + i, y + height - 1 - i, clipx, clipy, clipw, cliph, light, 0x7F);
		Draw_Line(x + width - 1 - i, y + i - 1, x + width - 1 - i, y + height - 1 - i - 1, clipx, clipy, clipw, cliph, dark, 0x7F);
	}
}

void HtmlViewClass::Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha)
{
	if (Surf->Bytes_Per_Pixel() == 4) {
		Draw_Line_32(x1, y1, x2, y2, clipx, clipy, clipw, cliph, color, alpha);
	} else if (Surf->Bytes_Per_Pixel() == 3) {
		Draw_Line_24(x1, y1, x2, y2, clipx, clipy, clipw, cliph, color, alpha);
	} else if (Surf->Bytes_Per_Pixel() == 2) {
		Draw_Line_16(x1, y1, x2, y2, clipx, clipy, clipw, cliph, color, alpha);
	}
}

#pragma auto_inline(off)
void HtmlViewClass::Draw_Line_24(int, int, int, int, int, int, int, int, int, int)
{
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// A tag attribute's value from offset on: quoted ("...") or up to the next delimiter; the offset after it, or -1.
static int Get_Value(int offset, Wstring &tag, Wstring &value)
{
	char *cptr = tag.get() + offset;
	while (*cptr == ' ' || *cptr == '=' || *cptr == '\t' || *cptr == '\r' || *cptr == '\n')
		cptr++;
	if (*cptr == '"') {
		cptr++;
		char *end = strchr(cptr, '"');
		if (end == NULL)
			return -1;
		value.set(end - cptr, cptr);
		return end - tag.get() + 1;
	}
	return tag.getToken(offset, " =\t\r\n", value);
}
#pragma auto_inline(on)


/*
**	Reconstruction notes (HV1, 2026-11-01).
**	* The object (htmlview.cpp's __FILE__ 0xB9CA28: asserts in 0x770860 line 158, 0x7710D0 line 926 and
**	  0x7759E0 lines 1114 / 1115): its own functions from after image.cpp's (that object's vtable 0xB043F0
**	  holds 0x76E910 / 0x76E9D0 / 0x76E990) to 0x7778E0's line drawer, then its template copies
**	  0x778590-0x779120 (DynamicVectorClass instances called by 0x770860 / 0x7710D0); 0x7791F0 starts the
**	  next object, unknown_htmlattrib.cpp. Literals 0xB9C9A4-0xB9CD20 ('%s\%s' and 'Vertical_Padding' first
**	  used by 0x76E9D0 / 0x76F0F0, before the __FILE__ string) sit in this run of .rdata too.
**	* So far: the frame drawing the client calls from 0x7767A0, on an object whose +0 is a wwlib Surface
**	  (slots +0x34 Lock, +0x38 Unlock, +0x40 Bytes_Per_Pixel, +0x44 Stride): Draw_Bevel 0x777630 and
**	  Draw_Line 0x7777C0 exact, the empty 24-bit line 0x777EC0 exact at its marker (3 bytes; kept out of
**	  line by auto_inline(off), as the client calls it). Not yet: the 32-bit and 16-bit alpha line
**	  drawers 0x7778E0 / 0x777EE0 (alpha factors in the statics 0xC41550 / 0xC41554 and 0xC4153C /
**	  0xC4154C), the view and its parser. The class sits here until the view's header exists.
**	* The functions with asserts sit on the client's lines (D25): Add_Data's G_ASSERT(0) on 158, Add_Text's
**	  two on 1114 / 1115; the blank lines before Add_Text keep the parser 0x7710D0 (its assert on 926) a gap.
**	* HV2 (2026-11-01): the view, HtmlViewClass (htmlview.h), 0x76FFD0-0x770720, every function exact. The
**	  file starts at 0x76FFD0: 0x76F0F0-0x76FFCF is HtmlBlockClass's object (unknown_htmlblock.cpp, ended by
**	  its ArrayList copies), and the view's ArrayList<HtmlLinkStruct> destructor is this file's template
**	  copy 0x7787C0. 0x770430 (the 0x210-byte link's destructor, a COMDAT inside the run) pairs with several.
**	  Forms: the constructor writes Clear's body out (its link destructors inline, Clear's call them);
**	  the block lists empty as `while (getTail(b)) { delete b; remove(length() - 1); }` (remove inline,
**	  its result unused); Update tests the byte Dirty into a bool set in a branch.
**	  Its constructor, destructor, Clear, Open and Clear_* have no callers in the client; image.cpp's
**	  0x76E9D0 calls 0x770860, which calls Layout and Render. HV1's UnknownHtmlDrawClass is the view (+0 its
**	  wwlib Surface: Get_Width / Get_Height size Layout's block). Draw_Line_32 0x7778E0: parked at 0.18
**	  (attempts.csv): the form of Draw_Line_16 compiles to the same shape but keeps alpha, not x1, cached.
**	* HtmlBlockClass's inline getters (Get_X / Get_Y / Get_Width / Get_Height) are why the client reads a
**	  field before an out-of-line call in one expression (Click, Draw_Block); the RECTs are filled bottom,
**	  top, left, right. NOT EXACT: Render (the parent bounds' stores), Hit_Test (its loop top), Add_Text
**	  (one frame slot more than the client's), Draw_Item (the client saves ebx / esi after the Type test).
**	* HV3 (2026-11-01): the parser Parse 0x7710D0, NOT EXACT (functional, D64, from the pseudocode): an
**	  if / else chain of tag names (p, font, br, a, title, i, b / strong, center, table, tr, td, h1-h4, /hN,
**	  body, hr, ul, li, img); <td> lays its cell out in a child block by recursing, and </td> returns the
**	  characters used. Its assert (body background with no link of that name) on line 926. Get_Value
**	  0x7784D0 exact: defined after Draw_Line_24 under auto_inline(off) (VC6 inlines it otherwise, also
**	  when it is defined last). Not yet: the client keeps ArrayList<HtmlItemStruct *>::add 0x778CF0 and four
**	  growVector copies (0x778EB0 int, 0x778F80 HtmlBlockClass *, 0x779050 HtmlImageClass *, 0x779120
**	  HtmlItemStruct *, by their callers) out of line where this build inlines them; the client's frame is
**	  0x284 bytes against this one's 0x27C. The statics 0xBE83F0 / 0xBE83F4 / 0xBE83F8 count table, row and
**	  cell depth and are never read. Block +0x1BC (RightPadding) is the line's start x (tables and lists
**	  move it), +0x1C0 / +0x1C4 the laid-out content's width / height (set at the parser's end).
**	* Not yet: Draw_Line_16 / Draw_Line_32. The font cache 0x76EB70 (Get_Font, declared in htmlview.h) is not this file's: it and
**	  0x76E9D0 (the object before, image.cpp's vtable slot) share its static ArrayList at 0xBE82E0.
*/
