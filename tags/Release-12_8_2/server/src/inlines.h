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

#if !defined(_INLINES_H__)
#define _INLINES_H__

#undef  DBGFILE
#define DBGFILE "inlines.h"

// library Includes
#include "qapplication.h"

// Wolfpack includes
#include "globals.h" // uiCurrentTime

inline void SetTimerSec(unsigned int *timer, const short seconds)
{
	*timer=seconds * MY_CLOCKS_PER_SEC + uiCurrentTime;
}

inline bool isCharSerial(long ser) {return (ser != INVALID_SERIAL && ser >= 0 && ser <  0x40000000);}
inline bool isItemSerial(long ser) {return (ser != INVALID_SERIAL && ser >= 0 && ser >= 0x40000000);}

inline QString tr( const char* text, const char* comment = 0, const char* context = "@default" )
{
	return qApp->translate( context, text, comment );
}


#endif

