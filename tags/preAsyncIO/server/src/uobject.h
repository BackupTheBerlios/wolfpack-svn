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

#if !defined (__UOBJECT_H__)
#define __UOBJECT_H__

#include "platform.h"
#include "typedefs.h"
#include "coord.h"
#include "iserialization.h"
#include "definable.h"
#include "customtags.h"

// System includes
#include <string>
#include <vector>

// Library includes
#include "qstring.h"
#include "qstringlist.h"
#include "qdom.h"
#include "qfile.h"

// Forward class declarations
//class ISerialization;
class Coord_cl;
class WPDefaultScript;

class cUObject : public cSerializable, public cDefinable
{
// Data Members
public:
	const std::vector< WPDefaultScript* > &getEvents( void );
	void setEvents( std::vector< WPDefaultScript* > List );
	void clearEvents( void );
	void addEvent( WPDefaultScript *Event );
	void removeEvent( QString Name );
	bool hasEvent( QString Name );

	QString eventList( void ); // Returns the list of events
	void recreateEvents( void ); // If the scripts are reloaded call that for each and every existing object

	// Events
	bool onUse( cUObject *Target );
	bool onCollide( cUObject* Obstacle ); // This is called for the walking character first, then for the item walked on

	SERIAL serial;
	SERIAL multis;
	bool free;

	std::string name;
	Coord_cl pos;

	cCustomTags tags;
// Methods
protected:
	virtual void processNode( const QDomElement &Tag ) = 0;

	std::vector< WPDefaultScript* > scriptChain;
	QStringList eventList_; // Important for recreating the scriptChain on reloading
	void init();

public:
	cUObject();
	cUObject( cUObject& ); // Copy constructor
	virtual ~cUObject() {};
	virtual void Serialize(ISerialization &archive);
	virtual QString objectID() const = 0;
	void moveTo( const Coord_cl& );
};

#endif // __UOBJECT_H__
