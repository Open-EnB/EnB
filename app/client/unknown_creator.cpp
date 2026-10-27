/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): CharacterCreatorClass and
**	LoginNameDataClass (unknown_creator.h), client 0x78CEE0-0x78E190, partly rebuilt.
*/

#include "unknown_creator.h"
#include "..\shared\auxdata\linearbu.h"
#include "registry.h"
#include "rawfile.h"
#include "..\shared\unknown_login_data.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\shipcust_desc.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

// 0x78CEE0
CharacterCreatorClass::CharacterCreatorClass()
{
	RegistryClass creator("SOFTWARE\\Westwood Studios\\Earth and Beyond\\Character and Starship Creator", false);
	if (creator.Is_Valid()) {
		RegistryClass folders("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", false);
		StringClass folder;
		if (folders.Is_Valid()) {
			folders.Get_String("Common AppData", folder, "");
		}
		if (!folder.Is_Empty()) {
			folder += "\\";
			folder += "Westwood Studios";
			if (Make_Folder(folder)) {
				folder += "\\";
				folder += "Earth and Beyond";
				if (Make_Folder(folder)) {
					folder += "\\";
					folder += "Character and Starship Creator";
					if (Make_Folder(folder)) {
						folder += "\\";
						Folder = folder;
					}
				}
			}
		} else {
			char path[MAX_PATH];
			char drive[_MAX_DRIVE];
			char directory[_MAX_DIR];
			GetModuleFileName(NULL, path, MAX_PATH);
			_splitpath(path, drive, directory, NULL, NULL);
			folder = drive;
			folder += directory;
			Folder = folder;
		}
	}
}

// 0x78D450
int __stdcall Unknown_78D450(const unsigned char *data, unsigned int size)
{
	int sum = 0;
	for (unsigned int i = 0; i < size; i++) {
		sum += *data + i;
	}
	return(sum);
}

// NOT EXACT: inline budget (the client calls StringClass::operator+= out of line, ours inlines the two string ones; dead inline reads, a top-declared checksum tried: attempts.py show 0x78D480)
// 0x78D480
void CharacterCreatorClass::Unknown_78D480(int slot, LoginCharacterClass *character, AvatarDescClass *avatar, ShipCustDescClass *ship, LoginNameDataClass *name)
{
	int size = avatar->Get_Short_Size() + ship->Get_Short_Size() + name->Get_Size();
	LinearBufferClass buffer(size, 0x400);
	StringClass path;
	RawFileClass file;

	*character = LoginCharacterClass();
	if (!Folder.Is_Empty()) {
		path = Folder;
		path += "Avatar";
		path += (char)(slot + '1');
		path += ".dat";
		file.Open(path, RawFileClass::READ);
		if (file.Is_Open()) {
			int checksum;
			if (file.Read(&checksum, 4)) {
				unsigned char *data = new unsigned char[size];
				if (file.Read(data, size)) {
					if (checksum == Unknown_78D450(data, size)) {
						character->ID = -1;
						character->Galaxy_ID = -1;
						buffer.Write(data, size);
						buffer.Seek(0);
						avatar->Read_Short(&buffer);
						ship->Read_Short(&buffer);
						name->Read(&buffer);
						ship->Tech_Level = 0;
					}
				}
				delete [] data;
			}
			file.Close();
		}
	}
}

// NOT EXACT: inline budget (as 0x78D480: the client calls StringClass::operator+= (const char *) out of line, ours inlines it)
// 0x78D8A0
void CharacterCreatorClass::Unknown_78D8A0(int slot, AvatarDescClass *avatar, ShipCustDescClass *ship, LoginNameDataClass *name)
{
	StringClass path;
	int size = avatar->Get_Short_Size() + ship->Get_Short_Size() + name->Get_Size();
	LinearBufferClass buffer(size, 0x400);
	RawFileClass file;

	if (!Folder.Is_Empty()) {
		path = Folder;
		path += "Avatar";
		path += (char)(slot + '1');
		path += ".dat";
		file.Open(path, RawFileClass::WRITE);
		if (file.Is_Open()) {
			avatar->Write_Short(&buffer);
			ship->Write_Short(&buffer);
			name->Write(&buffer);
			int checksum = Unknown_78D450((unsigned char *)buffer.Buffer, size);
			file.Write(&checksum, 4);
			file.Write(buffer.Buffer, size);
			file.Close();
		}
	}
}

// NOT EXACT: inline budget (as 0x78D480: the client calls StringClass::operator+= (const char *) out of line, ours inlines it)
// 0x78DAF0
void CharacterCreatorClass::Unknown_78DAF0(int slot)
{
	StringClass path;
	RawFileClass file;
	int size = AvatarDescClass().Get_Short_Size() + LoginNameDataClass().Get_Size() + ShipCustDescClass().Get_Short_Size();

	if (!Folder.Is_Empty()) {
		path = Folder;
		path += "Avatar";
		path += (char)(slot + '1');
		path += ".dat";
		file.Open(path, RawFileClass::READ | RawFileClass::WRITE);
		if (file.Is_Open()) {
			int checksum;
			if (file.Read(&checksum, 4)) {
				unsigned char *data = new unsigned char[size];
				if (file.Read(data, size)) {
					file.Close();
					if (checksum == Unknown_78D450(data, size)) {
						file.Delete();
					}
				}
			}
		}
	}
}

// 0x78E060
int Make_Folder(const char *path)
{
	if (!CreateDirectory(path, NULL)) {
		DWORD error = GetLastError();
		if (error != 0 && error != ERROR_ALREADY_EXISTS) {
			return(false);
		}
	}
	return(true);
}

// 0x78E0A0
void LoginNameDataClass::Set_Name(const char *name)
{
	strncpy(Name, name, sizeof(Name) - 1);
	Name[sizeof(Name) - 1] = 0;
}

// 0x78E0E0
bool LoginNameDataClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(Name, sizeof(Name));
	return(true);
}

// 0x78E110
bool LoginNameDataClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(Name, sizeof(Name));
	return(true);
}

// 0x78E140
int LoginNameDataClass::Get_Size()
{
	return(sizeof(Name));
}

/*
**	Reconstruction notes
**
**	Not rebuilt yet: the avatar files' functions 0x78D480 (reads 'Avatar<slot>.dat' through
**	RawFileClass into a LinearBufferClass and checks it), 0x78D8A0 (writes one), 0x78DAF0,
**	0x78DE40; 0x78D810 is WWLib's StringClass::operator+= (char) emitted here out of line.
**	0x78D450 and 0x78E060 have no callers: kept out of line, inlined where used.
*/
