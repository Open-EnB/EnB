// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\wnd_file.h beside wnd_file.cpp, after EA's Autorun
**	Wnd_File.h, D137)
**
**	StandardFileClass: E&B's copy of Westwood's Autorun file class (CnC_Generals_Zero_Hour\GeneralsMD\
**	Code\Tools\Autorun\Wnd_File.h), streams only (SUPPORT_STREAMS). 0x130 bytes: the FILE * at +0x00, the
**	struct stat at +0x04 (Query_Size reads st_size at +0x18), the name at +0x28, Currently_Open at +0x12C.
**	The client has no Tell, Query_Open, Query_Name_String, End_Of_File or Flush (EA's has them).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef WND_FILE_H  // NAME 3 [wnd_file.h]: EA's Wnd_File.h beside the client's wnd_file.cpp
#define WND_FILE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MODE_READ_ONLY				0
#define MODE_WRITE_ONLY				1

#define MAX_PATH_SIZE				_MAX_PATH

class StandardFileClass
{
	public:
		StandardFileClass();
		~StandardFileClass();

		bool 	Open 				( const char *file_name, int open_mode );
		bool 	Close				( void );
		int 	Read 				( void *buffer, unsigned long int bytes_to_read );
		int 	Write				( void *buffer, unsigned long int bytes_to_write );
		bool 	Seek 				( int distance, int seek_file_position );
		int 	Query_Size			( void );
		FILE *Query_File_Stream_Pointer( void );

	private:
		void Reset( void );

		FILE *File_Stream_Ptr;
		struct stat File_Statistics;
		char	File_Name[ MAX_PATH_SIZE ];
		bool	Currently_Open;
};

bool Full_Path_File_Exists( char const *file_name );

#endif // WND_FILE_H
