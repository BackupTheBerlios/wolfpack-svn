//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//	Copyright 1997, 98 by Marcus Rating (Cironian)
//  Copyright 2001 by holders identified in authors.txt
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//	* In addition to that license, if you are running this program or modified
//	* versions of it on a public system you HAVE TO make the complete source of
//	* the version used by you available or provide people with a location to
//	* download it.
//
//
//
//	Wolfpack Homepage: http://wpdev.sf.net/
//========================================================================================

#if !defined(__SERTEXTFILE_H__)
#define __SERTEXTFILE_H__

#include "iserialization.h"

#include <fstream>

/*!
CLASS
    

    This class provides an implementation for Text file serialization implementation.


USAGE
    <More detailed description of the class and
        a summary of it's (public) operations>.

*/
class serTextFile : public ISerialization
{
protected:
	std::fstream file;
	unsigned int _version;
public:
	serTextFile() : _version(0) {}
	virtual ~serTextFile() {}

	virtual void prepareReading(std::string ident);
	virtual void prepareWritting(std::string ident);
	virtual void close();	

	virtual unsigned int getVersion();
	virtual void setVersion(unsigned int);

	// Write Methods
	virtual void writeObjectID(std::string data);

	virtual void write(std::string Key, std::string &data);
	virtual void write(std::string Key, unsigned int data);
	virtual void write(std::string Key, signed int data);
	virtual void write(std::string Key, signed short data);
	virtual void write(std::string Key, unsigned short data);
	virtual void write(std::string Key, unsigned char data);
	virtual void write(std::string Key, signed char data);
	virtual void write(std::string Key, bool data);

	virtual void doneWritting();

	// Read Methods
	virtual void readObjectID(std::string &data);

	virtual void read(std::string Key, std::string    &data);
	virtual void read(std::string Key, unsigned int   &data);
	virtual void read(std::string Key, signed   int   &data);
	virtual void read(std::string Key, unsigned short &data);
	virtual void read(std::string Key, signed short   &data);
	virtual void read(std::string Key, unsigned char  &data);
	virtual void read(std::string Key, signed   char  &data);
	virtual void read(std::string Key, bool           &data);
};

#endif // __SERTEXTFILE_H__
