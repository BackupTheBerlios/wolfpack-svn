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

// Client.cpp: implementation of the cClient class.
//
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "Client.h"
#include "junk.h"
#include "chars.h"
#include "SndPkg.h"
#include "items.h"
#include "debug.h"

#undef  DBGFILE
#define DBGFILE "client.cpp"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cClient::cClient( UOXSOCKET nSocket )
{
	if( nSocket < 0 || nSocket > MAXCLIENT )
	{
		char tmp[50];
		sprintf( tmp, "Bad socket # <%d>", nSocket );
		LogCritical(tmp);
	}
	else
		socket_ = nSocket;
}

void cClient::sysMessage( UI16 color, const QString &text )
{
	sysmessage( socket_, color, text );
}

void cClient::sysMessage( const QString &text )
{
	sysmessage( socket_, text );
}

// Returns the item dragged by the current character
P_ITEM cClient::dragging( void )
{
	P_CHAR pChar = player();

	if( !pChar )
		return NULL;

	vector< SERIAL > equipment = contsp.getData( pChar->serial );

	for( UI32 i = 0; i < equipment.size(); i++ )
	{
		P_ITEM pItem = FindItemBySerial( equipment[ i ] );

		if( pItem && ( pItem->layer() == 0x1E ) )
			return pItem;
	}

	return NULL;
}