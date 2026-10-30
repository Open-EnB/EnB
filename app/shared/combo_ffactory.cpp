// Earth & Beyond client (reconstruction): app\shared\combo_ffactory.cpp. Notes at the end of the file.
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <algorithm>
#include "combo_ffactory.h"
#include "unknown_debug.h"
#include "..\..\wwlib\mixfile.h"
#include "..\..\wwlib\wwfile.h"
#include "..\..\ww3d2\texturethumbnail.h"

ComboFileFactoryClass::ComboFileFactoryClass(const char *path) : Simple(0), MixMap(100)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char path_buffer[_MAX_PATH];
	_finddata_t data;
	long handle;







	_splitpath(path, drive, dir, fname, ext);
	G_ASSERT(!fname[0]);
	G_ASSERT(!ext[0]);
	_makepath(path_buffer, drive, dir, NULL, NULL);
	Simple = new SimpleFileFactoryClass;
	Simple->Set_Sub_Directory(path_buffer);
	_makepath(path_buffer, drive, dir, "*", "MIX");
	handle = _findfirst(path_buffer, &data);
	if (handle != -1) {
		do {
			MixFileFactoryClass *mix = new MixFileFactoryClass(data.name, Simple);
			Add_Mix(mix);
			Mixes.push_front(mix);
			StringClass thumbnail_name = data.name;
			thumbnail_name[thumbnail_name.Get_Length() - 3] = 't';
			thumbnail_name[thumbnail_name.Get_Length() - 2] = 'h';
			thumbnail_name[thumbnail_name.Get_Length() - 1] = '6';
			ThumbnailManagerClass::Add_Thumbnail_Manager(thumbnail_name, data.name);
		} while (_findnext(handle, &data) == 0);
		_findclose(handle);
	}
}
// NOT EXACT: shape (explain.py), the client calls the table's _M_delete_node 0x4E6D00 out of line in MixMap.clear() and keeps this in ebx; ours inlines it
ComboFileFactoryClass::~ComboFileFactoryClass()
{
	if (Simple != NULL) {
		delete Simple;
		Simple = NULL;
	}
	for (std::list<MixFileFactoryClass *>::iterator i = Mixes.begin(); i != Mixes.end(); ++i) {
		delete *i;
	}
	Mixes.clear();
	MixMap.clear();
}




















void ComboFileFactoryClass::Add_Mix(MixFileFactoryClass *mix)
{
	FAIL_IF(!mix) return;

	DynamicVectorClass<StringClass> list;
	list.Set_Growth_Step(1000);
	mix->Build_Filename_List(list);
	for (int i = 0; i < list.Count(); i++) {
		std::string temp_string(list[i]);
		std::transform(temp_string.begin(), temp_string.end(), temp_string.begin(), tolower);








		FAIL_IF(MixMap.find(temp_string) != MixMap.end()) continue;
		MixMap[temp_string] = mix;
	}
}

FileClass *ComboFileFactoryClass::Get_File(char const *filename)
{
	FileClass *file = _TheFileFactory->Get_File(filename);
	if (file->Is_Available()) {
		return file;
	}
	std::string temp_string(filename);
	std::transform(temp_string.begin(), temp_string.end(), temp_string.begin(), tolower);
	std::hash_map<std::string, MixFileFactoryClass *>::const_iterator i = MixMap.find(temp_string);
	if (i != MixMap.end()) {
		FileFactoryClass *mix = (*i).second;
		FileClass *mix_file = mix->Get_File(filename);
		if (mix_file->Is_Available()) {
			_TheFileFactory->Return_File(file);
			return mix_file;
		}
		mix->Return_File(mix_file);
	}
	return file;
}

void ComboFileFactoryClass::Return_File(FileClass *file)
{
	delete file;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4E4D10-0x4E5A00 (then the table's and the list's instances to about 0x4E6F10): the
**	constructor 0x4E4D10 (the asserts on the client's lines 29 / 30, D25; the table built as
**	MixMap(100); every *.MIX of the directory added, newest first in Mixes, and its thumbnail file
**	'name.th6' given to ThumbnailManagerClass), FileFactoryClass's destructor 0x4E5110 and the scalar
**	deleting destructor 0x4E5130 (generated), the table's destructor 0x4E5160, the destructor 0x4E5230,
**	Add_Mix 0x4E5440 (the FAIL_IFs on lines 85 / 101), DynamicVectorClass<StringClass>'s destructor
**	0x4E5730, Get_File 0x4E57A0 and Return_File 0x4E5A00 (vtable 0xAEC5D0). Get_File asks the global
**	factory (_TheFileFactory, 0xBC77E0) first, then the mix file named for the lower-case file name.
*/
