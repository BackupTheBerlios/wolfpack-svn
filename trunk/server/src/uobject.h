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
#include "persistentobject.h"
#include "definable.h"
#include "customtags.h"
#include "factory.h"

// System includes
#include <string>
#include <vector>
#include <map>

// Library includes
#include "qstring.h"
#include "qstringlist.h"
//#include "qdom.h"
#include "qmap.h"

// Forward class declarations
class QDomElement;
class Coord_cl;
class WPDefaultScript;
class cUOSocket;
class QSqlQuery;
class cItem;

struct stError 
{
	INT8 code;
	QString text;
};

#define PROPERTY_ERROR( errno, data ) { stError *errRet = new stError; errRet->code = errno; errRet->text = data; return errRet; }

#define GET_PROPERTY( id, getter ) if( name == id ) {\
	value = cVariant( getter ); \
	return 0; \
}

#define SET_STR_PROPERTY( id, setter ) if( name == id ) {\
	QString text = value.toString(); \
	if( text == QString::null )	\
		PROPERTY_ERROR( -2, "String expected" ) \
	setter = text; \
	return 0; \
	}

#define SET_INT_PROPERTY( id, setter ) if( name == id ) {\
	bool ok; \
	INT32 data = value.toInt( &ok ); \
	if( !ok ) \
		PROPERTY_ERROR( -2, "Integer expected" ) \
	setter = data; \
	return 0; \
	}

#define SET_BOOL_PROPERTY( id, setter ) if( name == id ) {\
	bool ok; \
	INT32 data = value.toInt( &ok ); \
	if( !ok ) \
		PROPERTY_ERROR( -2, "Boolean expected" ) \
	setter = data == 0 ? false : true; \
	return 0; \
	}

#define SET_CHAR_PROPERTY( id, setter ) if( name == id ) {\
	setter = value.toChar(); \
	return 0; \
	}

#define SET_ITEM_PROPERTY( id, setter ) if( name == id ) {\
	setter = value.toItem(); \
	return 0; \
	}

class cUObject : public virtual PersistentObject, public virtual cDefinable
{
	Q_OBJECT
// Data Members
private:
	QString bindmenu_;
	QString name_;
	Coord_cl pos_;

protected:
	// Things for building the SQL string
	static void buildSqlString( QStringList &fields, QStringList &tables, QStringList &conditions );

public:
	const std::vector< WPDefaultScript* > &getEvents( void );
	void setEvents( std::vector< WPDefaultScript* > List );
	void clearEvents( void );
	void addEvent( WPDefaultScript *Event );
	void removeEvent( const QString& Name );
	void removeFromView( bool clean = true );
	bool hasEvent( const QString& Name ) const;
	
	// New Load (Query: result, offset: current field offset)
	void load( char **, UINT16& );
	void save();
	bool del();

	QString eventList( void ) const; // Returns the list of events
	void recreateEvents( void ); // If the scripts are reloaded call that for each and every existing object
	bool inRange( const cUObject *object, UINT32 range ) const;

	// Multiple quick-effect methods
	void effect( UINT16 id, UINT8 speed = 10, UINT8 duration = 5, UINT16 hue = 0, UINT16 renderMode = 0 ); // Moving with this character
	void effect( UINT16 id, cUObject *target, bool fixedDirection = true, bool explodes = false, UINT8 speed = 10, UINT16 hue = 0, UINT16 renderMode = 0 );
	void effect( UINT16 id, const Coord_cl &target, bool fixedDirection = true, bool explodes = false, UINT8 speed = 10, UINT16 hue = 0, UINT16 renderMode = 0 );

	// Events
	virtual bool onUse( cUObject *Target );
	virtual bool onCreate( const QString &definition );
	virtual bool onCollide( cUObject* Obstacle ); // This is called for the walking character first, then for the item walked on
	
	SERIAL serial;
	SERIAL multis;
	bool free;

	cCustomTags tags;

// Methods
public:
	virtual void talk( const QString &message, UI16 color = 0xFFFF, UINT8 type = 0, bool autospam = false, cUOSocket* socket = NULL ) = 0;

protected:
	virtual void processNode( const QDomElement &Tag ) = 0;


	std::vector< WPDefaultScript* > scriptChain;
	QStringList eventList_; // Important for recreating the scriptChain on reloading
	void init();

public:
	cUObject();
	cUObject( cUObject& ); // Copy constructor
	virtual ~cUObject() {};
	virtual QString objectID() const = 0;
	void moveTo( const Coord_cl& );
	unsigned int dist(cUObject* d) const;
	QString bindmenu() const	{ return bindmenu_; }
	QString name() const		{ return name_;		}
	Coord_cl pos() const		{ return pos_;		}
	void setBindmenu( const QString& d )	{ bindmenu_ = d;	}
	void setName( const QString& d )		{ name_ = d;		}	
	void setPos( const Coord_cl& d )		{ pos_ = d;			}

	bool isItem() { return (serial != INVALID_SERIAL && serial > 0 && serial >= 0x40000000); }
	bool isChar() { return (serial != INVALID_SERIAL && serial > 0 && serial <  0x40000000); }

	virtual stError *setProperty( const QString &name, const cVariant &value );
	virtual stError *getProperty( const QString &name, cVariant &value ) const;
};


class UObjectFactory : public Factory<cUObject, QString>
{
public:
	static UObjectFactory* instance()
	{
		static UObjectFactory factory;
		return &factory;
	}

	void registerSqlQuery( const QString &type, const QString &query )
	{
		sql_queries.insert( std::make_pair( type, query ) );
		sql_keys.push_back( type );
	}

	QString findSqlQuery( const QString &type ) const
	{
		std::map< QString, QString >::const_iterator iter = sql_queries.find( type );

		if( iter == sql_queries.end() )
			return QString::null;
		else
			return iter->second;
	}

	QStringList objectTypes() const
	{
		return sql_keys;
	}

private:
	std::map< QString, QString > sql_queries;
	QStringList sql_keys;
};

#endif // __UOBJECT_H__
