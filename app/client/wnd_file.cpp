// Earth & Beyond client (reconstruction): app\Client\wnd_file.cpp (its asserts' strings 0xB91E60-0xB91EE8). Notes at the end of the file.
#include <stdio.h>

// 0x6B6880
bool Full_Path_File_Exists(char const *file_name)
{
	FILE *file = fopen(file_name, "rb");
	if (file != NULL) {
		fclose(file);
		return true;
	}
	return false;
}

/*
**	Reconstruction notes
**
**	client.exe: wnd_file.cpp is E&B's copy of Westwood's Autorun Wnd_file.cpp (EA's
**	CnC_Generals_Zero_Hour\GeneralsMD\Code\Tools\Autorun\Wnd_file.cpp): StandardFileClass
**	0x6B61D0-0x6B6810 (Open, Close, Read, Write, Seek, Query_Size by their asserts, on stdio streams
**	only) and then Full_Path_File_Exists 0x6B6880, called by tutorial_manager.cpp ("tutorial.dat").
**	E&B's Full_Path_File_Exists opens with fopen(file_name, "rb") directly (EA's goes through
**	Open_File / Close_File and asserts file_name).
**	0x6B68C0 has the same 0x25 bytes and no caller (EA's HD_File_Exists / CD_File_Exists sit there; unnamed).
**	So far only 0x6B6880, byte-exact (appfile.py match; too short for verify's count).
*/
