/*
 *     Wolfpack Emu (WP)
 * UO Server Emulation Program
 *
 * Copyright 2001-2004 by holders identified in AUTHORS.txt
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Palace - Suite 330, Boston, MA 02111-1307, USA.
 *
 * In addition to that license, if you are running this program or modified
 * versions of it on a public system you HAVE TO make the complete source of
 * the version used by you available or provide people with a location to
 * download it.
 *
 * Wolfpack Homepage: http://wpdev.sf.net/
 */

#include "multi.h"
#include "definitions.h"

#include "muls/multiscache.h"
#include "world.h"
#include "log.h"
#include "console.h"
#include "sectors.h"
#include "timers.h"

void cMulti::remove()
{
	cUObject* object;
	for ( object = objects.first(); object; object = objects.next() )
	{
		object->setMulti( 0 );
	}
	objects.clear();

	cItem::remove();
}

cMulti::cMulti()
{
	serial_ = INVALID_SERIAL;
}

cMulti::~cMulti()
{
}

PyObject* cMulti::getPyObject()
{
	return cItem::getPyObject();
}

void cMulti::buildSqlString( const char *objectid, QStringList& fields, QStringList& tables, QStringList& conditions ) {
	cItem::buildSqlString(objectid, fields, tables, conditions);
}

const char* cMulti::className() const
{
	return "multi";
}

stError* cMulti::setProperty( const QString& name, const cVariant& value )
{
	return cItem::setProperty( name, value );
}

PyObject* cMulti::getProperty( const QString& name )
{
	return cItem::getProperty( name );
}

void cMulti::moveTo( const Coord_cl& pos, bool noRemove )
{
	cUObject::moveTo( pos, noRemove );
}

cMulti* cMulti::createFromScript( const QString& id )
{
	cMulti* multi = 0;

	const cElement* section = Definitions::instance()->getDefinition( WPDT_MULTI, id );

	if ( section )
	{
		multi = new cMulti;
		multi->setSerial( World::instance()->findItemSerial() );
		multi->setBaseid( id.latin1() );
		multi->applyDefinition( section );
		cDelayedOnCreateCall* onCreateCall = new cDelayedOnCreateCall( multi, id );
		Timers::instance()->insert( onCreateCall );
	}
	else
	{
		Console::instance()->log( LOG_ERROR, tr( "Unable to create unscripted item: %1\n" ).arg( id ) );
	}

	return multi;
}

void cMulti::addObject( cUObject* object )
{
	objects.remove( object );
	objects.append( object );
}

void cMulti::removeObject( cUObject* object )
{
	objects.remove( object );
}

bool cMulti::inMulti( const Coord_cl& pos )
{
	// Seek tiles with same x,y as pos
	// Seek for tile which z value <= pos.z + 5 && z value >= pos.z - 5
	MultiDefinition* multi = MultiCache::instance()->getMulti( id_ - 0x4000 );

	if ( !multi )
	{
		return false;
	}

	QValueVector<multiItem_st> items = multi->getEntries();
	QValueVector<multiItem_st>::iterator it;
	for ( it = items.begin(); it != items.end(); ++it )
	{
		if ( !it->visible )
		{
			continue;
		}

		if ( pos_.x + it->x != pos.x || pos_.y + it->y != pos.y )
		{
			continue;
		}

		if ( pos_.z + it->z >= pos.z - 5 && pos_.z + it->z <= pos.z + 5 )
		{
			return true;
		}
	}

	return false;
}

static FactoryRegistration<cMulti> registration("cMulti");

unsigned char cMulti::classid;

cMulti* cMulti::find( const Coord_cl& pos )
{
	cMulti* multi = 0;

	// Multi Range = BUILDRANGE
	cItemSectorIterator* iter = SectorMaps::instance()->findMultis( pos, BUILDRANGE );

	for ( cItem*item = iter->first(); item; item = iter->next() )
	{
		multi = dynamic_cast<cMulti*>( item );

		if ( multi && multi->inMulti( pos ) )
		{
			break;
		}
		else
		{
			multi = 0;
		}
	}

	delete iter;
	return multi;
}

void cMulti::save( cBufferedWriter& writer )
{
	cItem::save( writer );

	// Save objects within this multi *after* the multi
	cUObject* object;
	for ( object = objects.first(); object; object = objects.next() )
	{
		object->save( writer );
	}
}
