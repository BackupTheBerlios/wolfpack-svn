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
//	Wolfpack Homepage: http://www.wpdev.sf.net/
//========================================================================================

// Client.h: interface for the cClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__34C6E2C3_782D_4442_A4CC_8EC2B7DDD228__INCLUDED_)
#define AFX_CLIENT_H__34C6E2C3_782D_4442_A4CC_8EC2B7DDD228__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "typedefs.h"

// Forward Class declaration
class cChar;

class cClient  
{
protected:
	int		socket;
//	bool	dragging;
public:
	cClient(int s);
	~cClient();
	UOXSOCKET GetSocket();
	cChar* getPlayer();
	bool IsDragging();
	void SetDragging();
	void ResetDragging();
};

typedef cClient* P_CLIENT;

#endif // !defined(AFX_CLIENT_H__34C6E2C3_782D_4442_A4CC_8EC2B7DDD228__INCLUDED_)
