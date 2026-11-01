/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
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

/****************************************************************************\
*        C O N F I D E N T I A L --- W E S T W O O D   S T U D I O S         *
******************************************************************************
Project Name: Carpenter  (The RedAlert ladder creator)
File Name   : configfile.cpp
Author      : Neal Kettler
Start Date  : June 9, 1997
Last Update : June 17, 1997  


This class will read in a config file and store the key value pairs for
later access.  This is a fairly simple class, the config file is assumed
to be of the form:

#comment
key = value

The value can then be retrieved as a string or an integer.  The key on
the left is used for retrieval and it must be specified in uppercase
for the 'get' functions. E.g. getString("KEY",valWstring);
\***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "configfile.h"
#include "wdebug.h"

static uint32 Wstring_Hash(const Wstring &string);
static char  *Eat_Spaces(char *string);

ConfigFile::ConfigFile() : Dictionary_(Wstring_Hash)
{ }

ConfigFile::~ConfigFile()
{ }

// E&B: clear every entry, under the lock (0x8B2690, between ~ConfigFile and readFile); no caller in the client
void ConfigFile::clear(void)
{
  Critsec_.lock();
  Dictionary_.clear();
  Critsec_.unlock();
}

// Read and parse the config file.  The key value pairs will be stored
//   for later access by the getString/getInt functions.
bit8 ConfigFile::readFile(FILE *in)
{
  char    string[256];
  char    sectionname[256];  // section name like '[user parameters]'
  Wstring  key;
  Wstring  value;
  char   *cptr;

  memset(string,0,256);
  memset(sectionname,0,256);

  while (fgets(string,256,in))
  {
    cptr=Eat_Spaces(string);
    if ((*cptr==0)||(*cptr=='#'))  // '#' signals a comment
      continue;

    if (*cptr=='[')  // new section
    {
      key=cptr;
      key.truncate(']');    // remove after & including the ]
      key.cat("]");         // put the ] back
      strcpy(sectionname,key.get());    // set the current section name
      if (strlen(sectionname)==2) // clear section with a "[]"
        sectionname[0]=0;
      continue;
    }

    if (strchr(cptr,'=')==NULL)   // All config entries must have a '='
      continue;
    key=cptr;
    key.truncate('=');
    key.removeSpaces();  // No spaces allowed in the key
    key.toUpper();       // make key all caps

    // Add the section name to the end of the key
    if (strlen(sectionname))
      key.cat(sectionname);

    cptr=Eat_Spaces(strchr(cptr,'=')+1); // Jump to after the '='
    value=cptr;
    value.truncate('\r');
    value.truncate('\n');
    value.truncate('#');

    if (value.length())  // E&B: an empty value is stored as it is (0x8B2810)
    {
      // Remove trailing spaces
      while(isgraph(value.get()[strlen(value.get())-1])==0)
        value.get()[strlen(value.get())-1]=0;

      // E&B: a trailing backslash continues the value on the next line (0x8B2810)
      while(value.get()[strlen(value.get())-1]=='\\')
      {
        value.get()[strlen(value.get())-1]=0;
        if (fgets(string,256,in)==NULL)
          break;
        cptr=Eat_Spaces(string);
        value+=cptr;
        value.truncate('\r');
        value.truncate('\n');
        value.truncate('#');
      }
    }

    Critsec_.lock();
    Dictionary_.add(key,value);
    Critsec_.unlock();
  }
  return(TRUE);
}


//
// Enum through the config strings.  To start, index & offset should be 0
// If retval is false you're done, ignore whatever's in key & value.
//
// Section specifies the configfile section.  Set to NULL if you don't care.
//
bit8 ConfigFile::enumerate(int &index, int &offset, Wstring &key, Wstring &value, IN char *section) const
{
	int seclen = strlen(section);
  while(1)
  {
    Critsec_.lock();
    if (Dictionary_.iterate(index,offset,key,value)==FALSE)   // out of keys?
    {
      Critsec_.unlock();
      return(FALSE);
    }
    Critsec_.unlock();

    if (section==NULL)  // no specified section, so any will do...
      break;

    if (strlen(section)+2 >= strlen(key.get()))  // key should have form: X[section]
      continue;

    // Is this key part of our section?
	const char *keystr = key.get() + strlen(key.get())-seclen-1;
    if (strncmp(keystr,section,strlen(section))==0)
      break;
  }
  key.truncate('[');  // remove the section name
  return(TRUE);
}



// Get a config entry as a string
bit8 ConfigFile::getString(IN Wstring &_key, Wstring &value, IN char *section) const
{
  Wstring key(_key);
  key.toUpper();

  if (section)  // append section name to key
  {
    key+="[";
    key+=section;
    key+="]";
  }

  Critsec_.lock();
  bit8 retval=Dictionary_.getValue(key,value);
  Critsec_.unlock();

  if (retval==FALSE)
  { 
	DBGMSG("Config entry missing: "<<key.get()); 
  }

  return(retval);
} 

// Get a config entry as a string
bit8 ConfigFile::getString(IN char *key,Wstring &value, IN char *section) const
{
  Wstring sKey;
  sKey.set(key);
  return(getString(sKey,value,section));
}   

// Get a config entry as an integer
bit8 ConfigFile::getInt(IN Wstring &_key,sint32 &value, IN char *section) const
{
  Wstring key(_key);
  key.toUpper();
 
  if (section)  // append section name to key
  {
    key+="[";
    key+=section;
    key+="]";
  }

  Wstring svalue;
  Critsec_.lock();
  bit8 retval=Dictionary_.getValue(key,svalue);
  Critsec_.unlock();

  if (retval==FALSE)
  { DBGMSG("Config entry missing: "<<key.get()); }

  if (retval==FALSE)
    return(FALSE);
  value=atol(svalue.get());
  return(TRUE);
}

// Get a config entry as an integer
bit8 ConfigFile::getInt(IN char *key,sint32 &value, IN char *section) const
{
  Wstring sKey;
  sKey.set(key);

  return(getInt(sKey,value,section));
}    



// Get a config entry as an integer
bit8 ConfigFile::getInt(IN Wstring &_key,sint16 &value, IN char *section) const
{
  Wstring key(_key);
  key.toUpper();
 
  if (section)  // append section name to key
  {
    key+="[";
    key+=section;
    key+="]";
  }

  Wstring svalue;
  Critsec_.lock();
  bit8 retval=Dictionary_.getValue(key,svalue);
  Critsec_.unlock();

  if (retval==FALSE)
  { DBGMSG("Config entry missing: "<<key.get()); }

  if (retval==FALSE)
    return(FALSE);
  value=atoi(svalue.get());
  return(TRUE);
}
 
// Get a config entry as an integer
bit8 ConfigFile::getInt(IN char *key,sint16 &value, IN char *section) const
{
  Wstring sKey;
  sKey.set(key);

  return(getInt(sKey,value,section));
}



/************* Static functions below **************/

// Given a Wstring, return a 32 bit integer that has a good numeric
//   distributation for the purposes of indexing into a hash table.
static uint32 Wstring_Hash(const Wstring &string)
{
  uint32 retval=0;
  retval=string.length();
  for (uint32 i=0; i<string.length(); i++)
  {
    retval+=*(string.get()+i);
    retval+=i;
    retval=(retval<<8)^(retval>>24);  // ROL 8
  }
  return(retval);
}         

static char *Eat_Spaces(char *string)
{
  char *retval=string;
  while (isspace(*retval))
    retval++;
  return(retval);
}
