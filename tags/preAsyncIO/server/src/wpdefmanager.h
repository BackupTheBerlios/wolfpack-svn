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

#if !defined(AFX_WPDEFMANAGER_H__59377C90_C75A_4AC7_8B5B_517354DC8E76__INCLUDED_)
#define AFX_WPDEFMANAGER_H__59377C90_C75A_4AC7_8B5B_517354DC8E76__INCLUDED_

// Library Includes
#include "qdom.h"
#include "qmap.h"
#include "qstring.h"
#include "qstringlist.h"

typedef QMap< QString, QDomElement > DefSections;

enum WPDEF_TYPE 
{
	WPDT_ITEM = 0,
	WPDT_SCRIPT,
	WPDT_NPC,
	WPDT_LIST,
	WPDT_MENU,
	WPDT_SPELL,
	WPDT_PRIVLEVEL,
	WPDT_SPAWNREGION,
	WPDT_REGION,
	WPDT_MULTI,
	WPDT_TEXT
};

class WPDefManager  
{
private:
	// Maps
	DefSections Items;
	DefSections Scripts;
	DefSections NPCs;
	DefSections StringLists;
	DefSections Menus;
	DefSections Spells;
	DefSections PrivLevels;
	DefSections SpawnRegions;
	DefSections Regions;
	DefSections Multis;
	DefSections Texts;

	bool ImportSections( const QString& FileName );
	void ProcessNode( QDomElement Node );

public:
	WPDefManager() {};
	virtual ~WPDefManager() {};

	void reload( void );
	void load( void );
	void unload( void );

	QDomElement *getSection( WPDEF_TYPE Type, QString Section );
	QStringList getSections( WPDEF_TYPE Type );
	QString		getRandomListEntry( QString ListSection );
	QStringList	getList( QString ListSection );
	QString		getText( QString TextSection );
};

const QString processNode( QDomElement Node );

#endif // !defined(AFX_WPDEFMANAGER_H__59377C90_C75A_4AC7_8B5B_517354DC8E76__INCLUDED_)
