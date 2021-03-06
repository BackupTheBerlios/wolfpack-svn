//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//	Copyright 1997, 98 by Marcus Rating (Cironian)
//  Copyright 2001-2003 by holders identified in authors.txt
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
//	Foundation, Inc., 59 Temple Palace - Suite 330, Boston, MA 02111-1307, USA.
//
//	* In addition to that license, if you are running this program or modified
//	* versions of it on a public system you HAVE TO make the complete source of
//	* the version used by you available or provide people with a location to
//	* download it.
//
//
//
//	Wolfpack Homepage: http://wpdev.sf.net/
//==================================================================================


#if !defined(__STORAGE_H__)
#define __STORAGE_H__

// Platform specifics
#include "platform.h"

// System includes

#include <iostream>
#include <map>
#include <vector>

// Forward class definition


// wolfpack includes
#include "typedefs.h"

// Class definition
class Container_cl
{
public:
	Container_cl();
	~Container_cl();
	bool insert(SERIAL serContainer, SERIAL serObject);
	std::vector<SERIAL> getData(SERIAL serContainer);
	bool find(SERIAL serContainer, SERIAL serObject);
	bool remove(SERIAL serContainer, SERIAL serObject);
	
private:
	std::multimap<SERIAL, SERIAL> mapData;
	std::multimap<SERIAL, SERIAL>::iterator iterData;
};

#endif // __STORAGE_H__

