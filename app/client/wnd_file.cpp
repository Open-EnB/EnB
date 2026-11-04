// Earth & Beyond client (reconstruction): app\Client\wnd_file.cpp (its asserts' strings 0xB91DF8-0xB91FF0). Notes at the end of the file.
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wnd_file.h"
#include "..\shared\unknown_debug.h"

// 0x6B61B0
StandardFileClass::StandardFileClass( void )
{
	Reset();
}

// 0x6B61D0
StandardFileClass::~StandardFileClass( void )
{










































	G_ASSERT( File_Stream_Ptr == NULL );

	G_ASSERT( Currently_Open == false );
	Reset();
}

// 0x6B6240
bool StandardFileClass::Open( const char *file_name, int open_mode )
{
	int test;





	G_ASSERT( file_name != NULL );
	G_ASSERT( Currently_Open == false );
	G_ASSERT( strlen( file_name ) < MAX_PATH );
















	G_ASSERT( File_Stream_Ptr == NULL );
	if ( open_mode == MODE_READ_ONLY ) {
		File_Stream_Ptr = fopen( file_name, "rb" );
	} else {
		File_Stream_Ptr = fopen( file_name, "wb" );
	}



	G_ASSERT( File_Stream_Ptr != NULL );
	if ( File_Stream_Ptr == NULL ) {
		return( false );
	}

	test = stat( file_name, &File_Statistics );







	G_ASSERT( test == 0 );

	strcpy( File_Name, file_name );
	Currently_Open = true;
	return( true );
}

// 0x6B63B0
bool StandardFileClass::Close( void )
{
	int status;












	G_ASSERT( Currently_Open == true );






































	G_ASSERT( File_Stream_Ptr != NULL );
	if ( File_Stream_Ptr == NULL || Currently_Open == false ) {








		G_ASSERT( false );
		return( false );
	}

	status = fclose( File_Stream_Ptr );



	G_ASSERT( status == 0 );

	File_Stream_Ptr = NULL;
	Currently_Open = false;

	if ( status != 0 ) {
		return( false );
	}
	return( true );
}

// 0x6B6490
int StandardFileClass::Read( void *buffer, unsigned long int bytes_to_read )
{
	int bytes_read;
	int items_read;


















	G_ASSERT( buffer != NULL );
	G_ASSERT( bytes_to_read > 0 );
	G_ASSERT( Currently_Open == true );
























	G_ASSERT( File_Stream_Ptr != NULL );
	if ( File_Stream_Ptr == NULL || Currently_Open == false ) {
		return( 0 );
	}

	items_read = fread( buffer, bytes_to_read, 1, File_Stream_Ptr );













	G_ASSERT( items_read == 1 );
	bytes_read = items_read * bytes_to_read;
	return( bytes_read );
}

// 0x6B65A0
int StandardFileClass::Write( void *buffer, unsigned long int bytes_to_write )
{
	int items_written;
	int bytes_written;












	G_ASSERT( buffer != NULL );
	G_ASSERT( bytes_to_write > 0 );
	G_ASSERT( Currently_Open == true );

























	G_ASSERT( File_Stream_Ptr != NULL );
	if ( File_Stream_Ptr == NULL || Currently_Open == false ) {
		return( 0 );
	}

	items_written = fwrite( buffer, bytes_to_write, 1, File_Stream_Ptr );













	G_ASSERT( items_written == 1 );
	bytes_written = items_written * bytes_to_write;
	return( bytes_written );
}

// 0x6B66B0
bool StandardFileClass::Seek( int distance, int seek_file_position )
{
	bool success;






	G_ASSERT( Currently_Open == true );
	G_ASSERT( seek_file_position == SEEK_SET ||
				seek_file_position == SEEK_CUR ||
				seek_file_position == SEEK_END );























	G_ASSERT( File_Stream_Ptr != NULL );
	if ( File_Stream_Ptr == NULL || Currently_Open == false ) {
		return( false );
	}

	success = fseek( File_Stream_Ptr, distance, seek_file_position ) != 0;










	G_ASSERT( success == false );
	if ( success ) {
		return( false );
	}
	return( true );
}

// 0x6B67A0
int StandardFileClass::Query_Size( void )
{
	int size;






	G_ASSERT( Currently_Open == true );


















	G_ASSERT( File_Stream_Ptr != NULL );
	if ( File_Stream_Ptr == NULL || Currently_Open == false ) {
		return( -1 );
	}

	size = File_Statistics.st_size;







	G_ASSERT( size > -1 );
	return( size );
}

// 0x6B6840
FILE *StandardFileClass::Query_File_Stream_Pointer( void )
{
	return( File_Stream_Ptr );
}

// 0x6B6860
void StandardFileClass::Reset( void )
{
	File_Stream_Ptr = NULL;
	Currently_Open = false;
	File_Name[ 0 ] = '\0';
}

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
**	CnC_Generals_Zero_Hour\GeneralsMD\Code\Tools\Autorun\Wnd_file.cpp), streams only: StandardFileClass
**	0x6B61B0-0x6B686D (the constructor, destructor, Open, Close, Read, Write, Seek, Query_Size,
**	Query_File_Stream_Pointer, Reset, in EA's order) and then Full_Path_File_Exists 0x6B6880, called by
**	tutorial_manager.cpp ("tutorial.dat"). All byte-exact (appfile.py match, 11 of 11).
**	E&B's differs from EA's: no HD_Path / CD_Path (Open fopens the name itself, "rb" for MODE_READ_ONLY and
**	"wb" otherwise, in two calls the compiler merges, and strcpy's the name), the asserts say true / false
**	(not TRUE / FALSE) and file_name; Write lacks EA's early returns; Seek's 'success == false' is fseek's
**	result as a bool; no Tell, Query_Open, Query_Name_String, End_Of_File or Flush. Full_Path_File_Exists
**	fopens "rb" directly (EA's goes through Open_File / Close_File and asserts file_name).
**	The asserts sit on the client's lines (D25): the blank lines above them stand for EA's
**	SUPPORT_HANDLES blocks and comments, which E&B's copy kept in some shorter form.
**	0x6B68C0 has the same 0x25 bytes as Full_Path_File_Exists and no caller (EA's HD_File_Exists /
**	CD_File_Exists sit there; unnamed, not rebuilt).
**	tutorial_manager.cpp still declares its own view of StandardFileClass (same manglings).
*/
