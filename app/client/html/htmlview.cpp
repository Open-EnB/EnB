// Earth & Beyond client (reconstruction): app\client\html\htmlview.cpp (PARTIAL). Notes at the end of the file.
#include <windows.h>
#include "htmlview.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

HTMLViewer::HTMLViewer(void) :
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

HTMLViewer::~HTMLViewer(void)
{
}

bool HTMLViewer::Clear(void)
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

bool HTMLViewer::Open(char *url)
{
	Clear();
	strcpy(Url, url);
	if (Loader == NULL)
		return false;
	Loader->Open(url);
	return true;
}

bool HTMLViewer::Set_Loader(HtmlLoaderClass *loader)
{
	Loader = loader;
	return true;
}

bool HTMLViewer::Update(void)
{
	if (Loader != NULL)
		Loader->Update();
	bool dirty = false;
	if (Dirty)
		dirty = true;
	Dirty = 0;
	return dirty;
}

bool HTMLViewer::Set_Surface(DSurface *surface)
{
	Surf = surface;
	Layout();
	return true;
}

bool HTMLViewer::Clear_Links(void)
{
	Links.clear();
	return true;
}

bool HTMLViewer::Clear_Blocks(void)
{
	HtmlBlockClass *block;
	while (Blocks.getTail(block)) {
		delete block;
		Blocks.remove(Blocks.length() - 1);
	}
	return true;
}

























































bool HTMLViewer::Add_Data(char *name, char *mimetype, char *data, int length, bool render)
{
	if (name != NULL && mimetype != NULL && data != NULL && length != 0) {
		HtmlLinkStruct *link;
		if (!Find_Link(name, &link)) {
			HtmlLinkStruct entry;
			strcpy(entry.MimeType, mimetype);
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

void HTMLViewer::Layout(void)
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
#define Get_Colorref(str) (((strtoul((str) + 1, &end, 16) & 0xFF) << 16) | (strtoul((str) + 1, &end, 16) >> 16) | (strtoul((str) + 1, &end, 16) & 0xFF00))





// NOT EXACT: 0x7710D0, register allocation (<li> / <body> add clamps, the <img> link count, the end loops, the <table> inset: each client form alone rotates block / fontheight / tagend from ebp / ebx / edi); budget and frame as the client's
int HTMLViewer::Parse(HtmlLinkStruct *link, HtmlBlockClass *block, int offset)
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


	if (block == NULL)
		return 0;

	if (strcmp(link->MimeType, "image/gif") != 0) {
		while (true) {
			char *tagend = NULL;
			consumed = cptr - start;
			if (*cptr == '\0')
				break;
			char *tagstart = strchr(cptr, '<');
			if (tagstart != NULL) {
				char *search = tagstart;
				while (true) {
					tagend = strchr(search, '>');
					if (tagend == NULL)
						break;
					if (strncmp(tagstart, "<!--", 4) != 0 || strncmp(tagend - 2, "-->", 3) == 0)
						break;
					search = tagend + 1;
				}
			}
			if (tagend == NULL && strlen(cptr) != 0) {
				Add_Text(block, cptr, strlen(cptr), block->Get_Width() - block->Get_HPadding(), -1);
				break;
			}
			if (tagstart > cptr && strlen(cptr) != 0) {
				Add_Text(block, cptr, tagstart - cptr, block->Get_Width() - block->Get_HPadding(), -1);
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
				block->New_Line();
				block->Advance_Y(20, false);
				while (block->Pop_Style("Align", "p.left")) {}
				while (block->Pop_Style("Align", "p.right")) {}
				while (block->Pop_Style("Align", "p.center")) {}
				Wstring key;
				Wstring value;
				while (true) { if ((pos = tag.getToken(pos, " =\t\r\n", key)) == -1) break; if ((pos = Get_Value(pos, tag, value)) == -1) break;
					key.toLower();
					if (strcmp(key.get(), "align") == 0) {
						char align[64];
						sprintf(align, "p.%s", value.get());
						block->Push_Style("Align", align);
					}
				}
			} else if (strcmp(name.get(), "/p") == 0) {
				block->New_Line();
				block->Advance_Y(20, false);
				while (block->Pop_Style("Align", "p.left")) {}
				while (block->Pop_Style("Align", "p.right")) {}
				while (block->Pop_Style("Align", "p.center")) {}
			} else if (strcmp(name.get(), "font") == 0) {
				Wstring key;
				Wstring value;
				while (true) { if ((pos = tag.getToken(pos, " =\t\r\n", key)) == -1) break; if ((pos = Get_Value(pos, tag, value)) == -1) break;
					key.toLower();
					if (strcmp(key.get(), "face") == 0) {
						block->Set_Style("Font.Face", value.get());
					} else if (strcmp(key.get(), "size") == 0) {
						block->Set_Style("Font.BaseSize", value.get());
					} else if (strcmp(key.get(), "color") == 0) {
						char *end = NULL; sprintf(buffer, "%d", Get_Colorref(value.get()));
						block->Set_Style("Font.Colorref", buffer);
					}
				}
			} else if (strcmp(name.get(), "/font") == 0) {
				block->Remove_Style("Font.Face");
				block->Remove_Style("Font.BaseSize");
				block->Remove_Style("Font.Colorref");
			} else if (strcmp(name.get(), "br") == 0) {
				block->New_Line();
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
				block->New_Line();
				block->Advance_Y(0, false);
				block->Set_Style("Align", "center");
			} else if (strcmp(name.get(), "/center") == 0) {
				block->New_Line();
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
				block->New_Line();
				block->Advance_Y(20, false);
				int spacing = 0;
				Wstring key;
				Wstring value; char *end;
				while (true) { if ((pos = tag.getToken(pos, " =\t\r\n", key)) == -1) break; if ((pos = Get_Value(pos, tag, value)) == -1) break;
					key.toLower();
					if (strcmp(key.get(), "bgcolor") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
						tableattrs.Set("BackgroundColorref", buffer);
					}
					if (strcmp(key.get(), "text") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
						tableattrs.Set("TextColorref", buffer);
					}
					if (strcmp(key.get(), "link") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
						tableattrs.Set("LinkColorref", buffer);
					}
					if (strcmp(key.get(), "width") == 0) {
						int width = atol(value.get());
						if (value.get()[value.length() - 1] == '%')
							width = (block->Get_Width() - block->Get_HPadding() * 2) * width / 100;
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
				int avail = block->Get_Width() - block->Get_HPadding() * 2;
				if (str == NULL || atol(str) > avail) {
					sprintf(buffer, "%d", avail);
					tableattrs.Set("Width", buffer);
					str = tableattrs.Get("Width");
				}
				int width = atol(str);
				int align = block->Get_Align();
				if (align == 0) {
					block->Advance_X((block->Get_Width() - width) / 2, true);
					block->LineStartX += (block->Get_Width() - width) / 2;
				} else if (align == 1) {
					block->Advance_X(block->Get_Width() - width, true);
					block->LineStartX += block->Get_Width() - width;
				}
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = block->Get_Cursor_X();
				item->Y = block->Get_Cursor_Y();
				item->Width = 0;
				item->Height = 0;
				item->Attributes.Add_All(tableattrs);
				item->Attributes.Set("Type", "Table");
				block->Add_Item(item);
				sprintf(buffer, "%p", item);
				tableattrs.Set("ElementPointer", buffer);
				str = tableattrs.Get("Border");
				if (str != NULL) {
					int border = atol(str);
					int inset = spacing / 2 + border;
					block->Advance_Y(inset, true);
					block->Advance_X(inset, true);
					block->LineStartX += inset;
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
					block->Set_Line_Start_X(block->Get_Line_Start_X() - border);
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
					item->Height = spacing - item->Y + block->Get_Cursor_Y();
					tableattrs.Clear();
					colwidths.clear();
					block->Advance_Y(0, true);
					block->LineStartX -= spacing;
					int align = block->Get_Align();
					if (align == 0) {
						int shift = (block->Get_Width() - item->Width) / 2; block->LineStartX -= shift;
					} else if (align == 1) {
						int shift = block->Get_Width() - item->Width; block->LineStartX -= shift;
					}
					block->New_Line();
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
					if (cell->ContentHeight > height)
						height = cell->ContentHeight;
					width += cell->Get_Width() + spacing + border * 2;
				}
				for (i = 0; i < rowcells.length(); i++) {
					rowcells.get(cell, i);
					cell->Height = height;
					HtmlItemStruct *item = NULL;
					sscanf(cell->Get_Style("ElementPointer"), "%p", &item);
					item->Height = height + border * 2;
				}
				block->LineStartX -= width;
				block->New_Line();
				block->Advance_Y(spacing + border * 2 + height, true);
				rowcells.clear();
				rowattrs.Clear();
			} else if (strcmp(name.get(), "td") == 0) {
				CellDepth++;
				HtmlAttributeStackClass cellattrs;
				Wstring key;
				Wstring value; char *end;
				while (true) { if ((pos = tag.getToken(pos, " =\t\r\n", key)) == -1) break; if ((pos = Get_Value(pos, tag, value)) == -1) break;
					key.toLower();
					if (strcmp(key.get(), "bgcolor") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
						cellattrs.Set("BackgroundColorref", buffer);
					}
					if (strcmp(key.get(), "width") == 0) {
						int width = atol(value.get());
						if (value.get()[value.length() - 1] == '%') {
							int full = block->Get_Width();
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
				cell->AbsX = block->Get_Cursor_X() + border + half;
				cell->AbsY = border + half + block->Get_Cursor_Y();
				Blocks.addTail(cell);
				rowcells.addTail(cell);
				int length = Parse(link, cell, tagend - start + offset + 1);
				cell->Height = cell->ContentHeight;
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = half + block->Get_Cursor_X();
				item->Y = half + block->Get_Cursor_Y();
				item->Width = cell->Get_Width() + border * 2;
				item->Height = cell->Get_Height() + border * 2;
				item->Attributes.Add_All(cellattrs);
				item->Attributes.Set("Type", "Cell");
				block->Items.addTail(item);
				sprintf(buffer, "%p", item);
				cell->Set_Style("ElementPointer", buffer);
				block->LineStartX += cellwidth;
				block->Advance_X(cellwidth, false);
				tagend += length;
			} else if (strcmp(name.get(), "/td") == 0) {
				CellDepth--;
				consumed = tagend + 1 - start;
				break;
			} else if (strcmp(name.get(), "h1") == 0) {
				if (block->Get_Cursor_X() != block->Get_Line_Start_X())
					block->Advance_Y(0, false);
				block->New_Line();
				block->Push_Style("Font.HSize", "1");
			} else if (strcmp(name.get(), "h2") == 0) {
				if (block->Get_Cursor_X() != block->Get_Line_Start_X())
					block->Advance_Y(0, false);
				block->New_Line();
				block->Push_Style("Font.HSize", "2");
			} else if (strcmp(name.get(), "h3") == 0) {
				if (block->Get_Cursor_X() != block->Get_Line_Start_X())
					block->Advance_Y(0, false);
				block->New_Line();
				block->Push_Style("Font.HSize", "3");
			} else if (strcmp(name.get(), "h4") == 0) {
				if (block->Get_Cursor_X() != block->Get_Line_Start_X())
					block->Advance_Y(0, false);
				block->New_Line();
				block->Push_Style("Font.HSize", "4");
			} else if (strncmp(name.get(), "/h", 2) == 0 && isdigit(name.get()[2])) {
				block->New_Line();
				block->Advance_Y(20, false);
				block->Pop_Style("Font.HSize", NULL);
			} else if (strcmp(name.get(), "body") == 0) {
				Wstring key;
				Wstring value; char *end;
				while (true) { if ((pos = tag.getToken(pos, " =\t\r\n", key)) == -1) break; if ((pos = Get_Value(pos, tag, value)) == -1) break;
					key.toLower();
					if (strcmp(key.get(), "bgcolor") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
						block->Set_Attribute("BackgroundColorref", buffer);
					} else if (strcmp(key.get(), "text") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
						block->Set_Attribute("TextColorref", buffer);
					} else if (strcmp(key.get(), "link") == 0) {
						sprintf(buffer, "%d", Get_Colorref(value.get()));
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
						Images.add(image, Images.length());
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
				block->New_Line();
				block->Advance_Y(5, false);
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = block->Get_Cursor_X();
				item->Y = block->Get_Cursor_Y();
				item->Width = block->Get_Width() - block->Get_HPadding() - item->X;
				item->Height = 4;
				item->Attributes.Set("Type", "HR");
				block->Items.addTail(item);
				block->New_Line();
				block->Advance_Y(5, false);
			} else if (strcmp(name.get(), "ul") == 0) {
				block->Set_Line_Start_X(block->Get_Line_Start_X() + 32);
				block->Advance_X(32, false);
				if (block->Get_Line_Start_X() == block->Get_HPadding() + 32)
					block->Advance_Y(5, false);
			} else if (strcmp(name.get(), "/ul") == 0) {
				if (block->Get_Line_Start_X() >= block->Get_HPadding() + 32) {
					block->Set_Line_Start_X(block->Get_Line_Start_X() - 32);
					if (block->Get_Cursor_X() >= block->Get_HPadding() + 32)
						block->Advance_X(-32, false);
				}
				block->New_Line();
				if (block->Get_Line_Start_X() == block->Get_HPadding())
					block->Advance_Y(20, false);
			} else if (strcmp(name.get(), "li") == 0) {
				block->New_Line();
				block->Advance_Y(4, false);
				HtmlItemStruct *item = new HtmlItemStruct;
				item->X = block->Get_Cursor_X() - 10;
				item->Y = fontheight / 2 + block->Get_Cursor_Y();
				item->Width = 4;
				item->Height = 4;
				item->Attributes.Set("Type", "LI");
				block->Items.add(item, block->Items.length());
			} else if (strcmp(name.get(), "img") == 0) {
				Wstring key;
				Wstring value;
				while (true) { if ((pos = tag.getToken(pos, " =\t\r\n", key)) == -1) break; if ((pos = Get_Value(pos, tag, value)) == -1) break;
					key.toLower();
					if (strcmp(key.get(), "src") == 0) {
						HtmlImageClass *image;
						if (!Find_Image(value.get(), &image)) {
							image = new HtmlGifImageClass(value.get(), Surf->Bytes_Per_Pixel() << 3);
							Images.addTail(image);
							Loader->Open(value.get());
							HtmlLinkStruct *data;
							if (Find_Link(value.get(), &data)) {
								if (data->Data.length() > 0) {
									char *bytes;
									data->Data.getPointer(&bytes, 0);
									image->Set_Data(bytes, data->Data.length());
								}
							}
						}












						HtmlItemStruct *item = new HtmlItemStruct;
						int width = 32;
						int height = 32;
						item->X = block->Get_Cursor_X();
						item->Y = block->Get_Cursor_Y();
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
	int i;
	for (i = 0; i < block->Items.length(); i++) {
		HtmlItemStruct *item = block->Get_Item(i);
		if (item->Height + item->Y > bottom)
			bottom = item->Height + item->Y;
	}
	int right = 0;
	for (i = 0; i < block->Items.length(); i++) {
		HtmlItemStruct *item = block->Get_Item(i);
		if (item->Width + item->X > right)
			right = item->Width + item->X;
	}
	block->ContentHeight = block->VPadding + 2 + bottom;
	block->ContentWidth = block->Get_HPadding() + right;
	return consumed;
}

































































// 0x7759E0
bool HTMLViewer::Add_Text(HtmlBlockClass *block, char *cptr_in, int maxlength, int maxwidth, int)
{
	int x = block->CursorX;
	int y = block->CursorY;
	int startx = x;
	G_ASSERT(maxlength > 0);
	G_ASSERT(cptr_in);
	{
		int pad = block->LineStartX;
		if (startx == pad) {
			while (*cptr_in == ' ' || *cptr_in == '\n' || *cptr_in == '\t' || *cptr_in == '\r') {
				cptr_in++;
				maxlength--;
			}
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
	char *line = text.get();
	maxlength = text.length();
	if (block->Get_Attribute("TitleFlag") != NULL) {
		block->Set_Attribute("Title", line);
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
	cptr_in = line;
	SIZE size;
	while (true) {
		int n = 1;
		while (true) {
			GetTextExtentPoint32(hdc, cptr_in, n, &size);
			if (size.cx > widest && size.cx + x < maxwidth)
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
		if (lines == 1 || n == maxlength || startx != block->LineStartX) {
			HtmlItemStruct *item = new HtmlItemStruct;
			item->X = x;
			item->Y = y;
			item->Width = widest;
			item->Height = lines * height;
			item->Attributes.Add_All(block->Styles);
			item->Attributes.Set("Type", "Text");
			item->Attributes.Set("Text", line, cptr_in + n - line);
			block->Items.addTail(item);
			if (n != maxlength) {
				x = block->CursorX = block->LineStartX;
				y = block->Advance_Y(0, false);
			} else {
				x = block->Advance_X(size.cx, false);
				if (lines > 1)
					y = block->Advance_Y(0, false);
			}
			lines = 0;
			widest = 0;
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

// 0x7761E0
bool HTMLViewer::Render(int scrollx, int scrolly, int x, int y, int width, int height)
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
		RECT bounds;
		HtmlBlockClass *parent = block->Parent;
		if (parent != NULL) {
			bounds.left = parent->AbsX;
			bounds.top = parent->AbsY;
			bounds.right = bounds.left + parent->Get_Width();
			bounds.bottom = bounds.top + parent->Get_Height();
			if (!IntersectRect(&rect, &clip, &bounds))
				continue;
		}
		Draw_Block(block, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	return true;
}

bool HTMLViewer::Draw_Block(HtmlBlockClass *block, int x, int y, int width, int height)
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

// NOT EXACT: 0x7767A0, the text loop's setup loads are scheduled differently (the client holds item
// in esi before strlen)
bool HTMLViewer::Draw_Item(HtmlBlockClass *block, HtmlItemStruct *item, int offx, int offy, int clipx, int clipy, int clipw, int cliph)
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
		int itemh = item->Height;
		int x = item->X - offx;
		int right = item->Width + x;
		int widest = 0;
		int dy = 0;
		int y = item->Y - offy;
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

	}

	else if (strcmp(type, "HR") == 0) {
		Draw_Bevel(item->X - offx, item->Y - offy, item->Width, item->Height, clipx, clipy, clipw, cliph, 1, true);

	} else if (strcmp(type, "LI") == 0) {
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

	} else if (strcmp(type, "Table") == 0) {
		char *border = item->Attributes.Get("Border");
		if (border != NULL) {
			int width = atol(border);
			Draw_Bevel(item->X - offx, item->Y - offy, item->Width, item->Height, clipx, clipy, clipw, cliph, width, true);

		}
	} else if (strcmp(type, "Cell") == 0) {
		char *border = item->Attributes.Get("Border");
		if (border != NULL) {
			int width = atol(border);
			Draw_Bevel(item->X - offx, item->Y - offy, item->Width, item->Height, clipx, clipy, clipw, cliph, width, false);

		}
	} else if (strcmp(type, "Image") == 0) {
		char *url = item->Attributes.Get("URL");
		for (int i = 0; i < Images.length(); i++) {
			HtmlImageClass *image;
			Images.get(image, i);
			if (strcmp(image->Get_Name(), url) == 0) {
				image->Draw(Surf, item->X - offx, item->Y - offy, clipx, clipy, clipw, cliph);
				break;
			}
		}
	}
	return true;
}

char *HTMLViewer::Click(int x, int y, int)
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
char *HTMLViewer::Hit_Test(int x, int y, int *type, int *index)
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

bool HTMLViewer::Find_Link(char *name, HtmlLinkStruct **link)
{
	for (int i = 0; i < Links.length(); i++) {
		Links.getPointer(link, i);
		if (strcmp((*link)->Name, name) == 0)
			return true;
	}
	*link = NULL;
	return false;
}

bool HTMLViewer::Find_Image(char *name, HtmlImageClass **image)
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

int HTMLViewer::Get_Unknown13C(void)
{
	return Unknown13C;
}

int HTMLViewer::Get_Unknown138(void)
{
	return Unknown138;
}

char *HTMLViewer::Get_Title(void)
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

void HTMLViewer::Draw_Bevel(int x, int y, int width, int height, int clipx, int clipy, int clipw, int cliph, int thickness, bool raised)
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

void HTMLViewer::Draw_Line(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha)
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
static unsigned int Alpha32;  // NAME 1: 0xC41550, the 32-bit blend's alpha & 0xFF (written, then read back by the products)
static unsigned int InvAlpha32;  // NAME 1: 0xC41554, 0xFF - Alpha32 (assigned first)

#define BLEND32(dst, col, alph) \
	{ \
		c = *(dst); \
		InvAlpha32 = 0xFF - ((alph) & 0xFF); \
		Alpha32 = (alph) & 0xFF; \
		*(dst) = ((((c & 0xFF0000) * InvAlpha32 + ((col) & 0xFF0000) * Alpha32) & 0xFF0000FF) | \
			(((c & 0xFF00) * InvAlpha32 + ((col) & 0xFF00) * Alpha32) & 0xFF0000) | \
			((c & 0xFF) * InvAlpha32 + ((col) & 0xFF) * Alpha32)) >> 8; \
	}

void HTMLViewer::Draw_Line_32(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha)
{
	int count, right, bottom, pitch, dx, dy, sx, sy, err;
	unsigned int *buffer, *p;
	unsigned int c;

	count = 0;
	right = clipx + clipw;
	bottom = clipy + cliph;

	buffer = (unsigned int *)Surf->Lock();
	if (buffer == NULL)
		return;

	pitch = Surf->Stride() / 4;
	p = buffer + (y1 * pitch + x1);
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx < 0) {
		dx = -dx;
		sx = -1;
	} else {
		sx = 1;
	}
	if (dy < 0) {
		dy = -dy;
		sy = -1;
	} else {
		sy = 1;
	}

	if (dy < dx) {
		err = dx >> 2;
		while (x1 != x2) {
			if (count == 1) {
				if (x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
					BLEND32(p, color, alpha);
				}
				count = 0;
			}
			count++;
			err += dy;
			if (err > dx) {
				err -= dx;
				y1 += sy;
				p += sy * pitch;
			}
			x1 += sx;
			p += sx;
		}
		if (count == 1 && x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
			BLEND32(p, color, alpha);
		}
	} else {
		err = dy >> 1;
		while (y1 != y2) {
			if (count == 1) {
				if (x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
					BLEND32(p, color, alpha);
				}
				count = 0;
			}
			count++;
			err += dx;
			if (err > dy) {
				err -= dy;
				x1 += sx;
				p += sx;
			}
			y1 += sy;
			p += sy * pitch;
		}
		if (count == 1 && x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
			BLEND32(p, color, alpha);
		}
	}

	Surf->Unlock();
}
#pragma auto_inline(on)

#pragma auto_inline(off)
void HTMLViewer::Draw_Line_24(int, int, int, int, int, int, int, int, int, int)
{
}
#pragma auto_inline(on)

#pragma auto_inline(off)
static unsigned int Alpha16;  // NAME 1: 0xC4153C, the 16-bit (555) blend's alpha & 0xFF
static unsigned int InvAlpha16;  // NAME 1: 0xC4154C, 0xFF - Alpha16 (assigned first)

#define BLEND16(dst, col, alph) \
	{ \
		c = *(dst); \
		InvAlpha16 = 0xFF - ((alph) & 0xFF); \
		Alpha16 = (alph) & 0xFF; \
		*(dst) = (unsigned short)((((((unsigned short)c & 0x3E0) * InvAlpha16 + ((col) & 0x3E0) * Alpha16) >> 8) & 0x3E0) | \
			((((c & 0x7C00) * InvAlpha16 + ((col) & 0x7C00) * Alpha16) >> 8) & 0x7C00) | \
			(((c & 0x1F) * InvAlpha16 + ((col) & 0x1F) * Alpha16) >> 8)); \
	}

void HTMLViewer::Draw_Line_16(int x1, int y1, int x2, int y2, int clipx, int clipy, int clipw, int cliph, int color, int alpha)
{
	int count, right, bottom, pitch, dx, dy, sx, sy, err;
	unsigned short *buffer, *p;
	unsigned int c;

	count = 0;
	right = clipx + clipw;
	bottom = clipy + cliph;

	buffer = (unsigned short *)Surf->Lock();
	if (buffer == NULL)
		return;

	pitch = Surf->Stride() / 2;
	p = buffer + (y1 * pitch + x1);
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx < 0) {
		dx = -dx;
		sx = -1;
	} else {
		sx = 1;
	}
	if (dy < 0) {
		dy = -dy;
		sy = -1;
	} else {
		sy = 1;
	}

	if (dy < dx) {
		err = dx >> 2;
		while (x1 != x2) {
			if (count == 1) {
				if (x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
					BLEND16(p, (unsigned int)color, alpha);
				}
				count = 0;
			}
			count++;
			err += dy;
			if (err > dx) {
				err -= dx;
				y1 += sy;
				p += sy * pitch;
			}
			x1 += sx;
			p += sx;
		}
		if (count == 1 && x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
			BLEND16(p, (unsigned int)color, alpha);
		}
	} else {
		err = dy >> 1;
		while (y1 != y2) {
			if (count == 1) {
				if (x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
					BLEND16(p, (unsigned int)color, alpha);
				}
				count = 0;
			}
			count++;
			err += dx;
			if (err > dy) {
				err -= dy;
				x1 += sx;
				p += sx;
			}
			y1 += sy;
			p += sy * pitch;
		}
		if (count == 1 && x1 >= clipx && x1 < right && y1 >= clipy && y1 < bottom) {
			BLEND16(p, (unsigned int)color, alpha);
		}
	}

	Surf->Unlock();
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
**	  line by auto_inline(off), as the client calls it). The 32-bit and 16-bit alpha line drawers
**	  0x7778E0 / 0x777EE0 (alpha factors in the statics 0xC41550 / 0xC41554 and 0xC4153C / 0xC4154C)
**	  exact since B2 (2026-11-25), see below. The class sits here until the view's header exists.
**	* The functions with asserts sit on the client's lines (D25): Add_Data's G_ASSERT(0) on 158, Add_Text's
**	  two on 1114 / 1115; the blank lines before Add_Text keep the parser 0x7710D0 (its assert on 926) a gap.
**	* HV2 (2026-11-01): the view, HTMLViewer (htmlview.h), 0x76FFD0-0x770720, every function exact. The
**	  file starts at 0x76FFD0: 0x76F0F0-0x76FFCF is HtmlBlockClass's object (unknown_htmlblock.cpp, ended by
**	  its ArrayList copies), and the view's ArrayList<HtmlLinkStruct> destructor is this file's template
**	  copy 0x7787C0. 0x770430 (the 0x210-byte link's destructor, a COMDAT inside the run) pairs with several.
**	  Forms: the constructor writes Clear's body out (its link destructors inline, Clear's call them);
**	  the block lists empty as `while (getTail(b)) { delete b; remove(length() - 1); }` (remove inline,
**	  its result unused); Update tests the byte Dirty into a bool set in a branch.
**	  Its constructor, destructor, Clear, Open and Clear_* have no callers in the client; image.cpp's
**	  0x76E9D0 calls 0x770860, which calls Layout and Render. HV1's UnknownHtmlDrawClass is the view (+0 its
**	  wwlib Surface: Get_Width / Get_Height size Layout's block). Draw_Line_32 0x7778E0 / Draw_Line_16
**	  0x777EE0 (B2, 2026-11-25): a dotted Bresenham (every second pixel) clipped to clipx..clipx + clipw,
**	  alpha-blended into the locked surface (32-bit: 8:8:8, the alpha byte cleared; 16-bit: 555 masks).
**	  Both exact once the blend assigns InvAlpha before Alpha (else alpha stays cached in a register and
**	  the whole function's allocation rotates); the 16-bit one also needs its 0x3E0 term's pixel read as
**	  (unsigned short)c, which alone puts that term first (an unsigned short c instead adds and 0xffff).
**	* HtmlBlockClass's inline getters (Get_X / Get_Y / Get_Width / Get_Height) are why the client reads a
**	  field before an out-of-line call in one expression (Click, Draw_Block); the RECTs are filled bottom,
**	  top, left, right. NOT EXACT: Render (the parent bounds' stores), Hit_Test (its loop top), Add_Text
**	  (one frame slot more than the client's), Draw_Item (its Text loop setup; one exit gives the late pushes).
**	* HV3 (2026-11-01): the parser Parse 0x7710D0, NOT EXACT (functional, D64, from the pseudocode): an
**	  if / else chain of tag names (p, font, br, a, title, i, b / strong, center, table, tr, td, h1-h4, /hN,
**	  body, hr, ul, li, img); <td> lays its cell out in a child block by recursing, and </td> returns the
**	  characters used. Its assert (body background with no link of that name) on line 926. Get_Value
**	  0x7784D0 exact: defined after Draw_Line_24 under auto_inline(off) (VC6 inlines it otherwise, also
**	  when it is defined last). Not yet: the client keeps ArrayList<HtmlItemStruct *>::add 0x778CF0 (the
**	  table's item) and growVector copies (0x778EB0 int, 0x778F80 HtmlBlockClass *, 0x779050 HtmlImageClass *,
**	  0x779120 HtmlItemStruct *, by their callers) out of line: an inline budget (explain.py).
**	  PT2 (2026-11-19, the client's bytes): every loop is top-tested with one call (while (true) with the
**	  tests as breaks: the six attribute loops, the '>' search, the tag loop); Get_Colorref is a macro on
**	  Parse's own end (value.get() three times per colour: 16 get calls more); the <body> / <img> lookups
**	  are Find_Image / Find_Link inlined (no index test after the loop: the found block jumps to the
**	  attribute loop's top). The inline budget cuts the deepest nested expansions first, then the earliest
**	  sites: <td>, <hr> and <img> add with addTail (no pos > Entries_ clamp in the client's copies; <td>'s
**	  growVectors both out of line), the block's cursor, line start and padding are read through inline
**	  accessors (New_Line, Get_Cursor_X / Y, Get_Line_Start_X, Set_Line_Start_X, Get_HPadding): every
**	  client add / growVector call out of line, the table's add 0x778CF0 included; <body>'s image add
**	  keeps its clamp test in ours (the client's has none: open). The <table> item's zero width / height
**	  are the constructor's; right is declared before the second end loop. Frame 0x284 as the client's: <font>'s colour sets its own end to NULL
**	  first (mov [end], ebx), <td> has its own end; <table>'s and </table>'s alignment, </table>'s spacing,
**	  <td> and </tr> move LineStartX with += / -= (add / sub [mem], reg; </table>'s alignment through a
**	  shift local: both arms end in one tail-merged sub), the rest with the setter. The statics 0xBE83F0 / 0xBE83F4 / 0xBE83F8 count table, row and
**	  cell depth and are never read. Block +0x1BC (LineStartX) is the line's start x (tables and lists
**	  move it), +0x1C0 / +0x1C4 the laid-out content's width / height (set at the parser's end).
**	* The font cache 0x76EB70 (Get_Font, declared in htmlview.h) is not this file's: it and
**	  0x76E9D0 (the object before, image.cpp's vtable slot) share its static ArrayList at 0xBE82E0.
*/
