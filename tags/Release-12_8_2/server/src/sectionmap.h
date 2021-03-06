//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//	Copyright 1997, 98 by Marcus Rating (Cironian)
//      Copyright 2001 by holders identified in authors.txt
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
//	Wolfpack Homepage: http://www.wpdev.com/
//========================================================================================
#if !defined(__SECTIONMAP_H__)
#define __SECTIONMAP_H__

// Platform specifics
#include "platform.h"


// System Includes
#include <iostream>
#include <map>
#include <vector>
#include <string>



using namespace std ;


// Third Party includes



//Forward class declaration

class Sectionmap_cl ;

//Wolfpack Includes
#include "mstring.h"


//Class definitions
class Sectionmap_cl
{
public:
        /// Constructor
        Sectionmap_cl()   ;
        /// Cosntructor of itself
        Sectionmap_cl(const Sectionmap_cl& clData) ;
        /// Desctructor
        ~Sectionmap_cl() ;

        // Return the number in the list
        UI32 size() ;


		// Return a value based on a key

		string	get(string sKey) ;

		// See if a key is present

		bool	isPresent(string sKey) ;

        /// Operator=
        Sectionmap_cl&    operator= (const Sectionmap_cl& clSectionmap_cl) ;

        /// Clear out the map
        void clear() ;

		/// Insert an element into the Section (we have to parse it)
		void insert( string sInput) ;

		// Pop of the first element (returns a vector[0] being the key, vector[1] being the value;
		vector<mstring>   pop()  ;
		void offset(UI32 siOffset) ;

private:
        //
        map<string,string>    mapData ;

        map<string,string>::iterator iterData ;

		map<string,string>::iterator iterOffset ;


};
//==========================================================================================

#endif



