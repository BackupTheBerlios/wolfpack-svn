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
//	GNU General Public Li678cense for more details.
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

/* House code for deed creation by Tal Strake, revised by Cironian */

//#include "wolfpack.h"
#include "house.h"
#include "iserialization.h"
#include "mapobjects.h"
#include "srvparams.h"
#include "mapstuff.h"
#include "debug.h"
#include "tilecache.h"
#include "utilsys.h"
#include "network.h"
#include "network/uosocket.h"

#include "customtags.h"
#include "territories.h"

// System Include Files
#include <algorithm>

// Namespaces for this module
using namespace std;

#undef  DBGFILE
#define DBGFILE "house.cpp"

void cHouse::processHouseItemNode( const QDomElement &Tag )
{
	P_CHAR pOwner = FindCharBySerial( ownserial );
	P_ITEM nItem = Items->MemItemFree();

	if( !nItem )
		return;
	
	nItem->Init( true );
	cItemsManager::getInstance()->registerItem( nItem );

	nItem->applyDefinition( Tag );
	if( nItem->type() == 222 )
		nItem->setName( name() );

	nItem->SetOwnSerial( this->ownserial );
	addItem( nItem );
	Coord_cl npos = this->pos;

	QDomNode childNode = Tag.firstChild();
	while( !childNode.isNull() )
	{
		if( childNode.isElement() )
		{
			QDomElement childTag = childNode.toElement();
			QString TagName = childTag.nodeName();
			QString Value = this->getNodeValue( childTag );

			// <pack />
			if( TagName == "pack" && pOwner )
			{
				P_ITEM pBackpack = Packitem( pOwner );
				if( pBackpack )
					pBackpack->AddItem( nItem );
			}

			// <lock />
			else if( TagName == "lock" )
			{
				nItem->magic = 4;
			}

			// <secure />
			else if( TagName == "secure" )
				nItem->magic = 3;

			// <position x="1" y="5" z="20" />
			else if( TagName == "position" )
			{
				npos.x = npos.x + childTag.attribute( "x" ).toShort();
				npos.y = npos.y + childTag.attribute( "y" ).toShort();
				npos.z = npos.z + childTag.attribute( "z" ).toShort();
			}
		}
		
		childNode = childNode.nextSibling();
	}

	nItem->moveTo( npos );
	nItem->update();
}

bool cHouse::onValidPlace()
{
    cTerritory* Region = cAllTerritories::getInstance()->region( pos.x, pos.y );
	if( Region != NULL && Region->isGuarded() && SrvParams->houseInTown() == 0 )
		return false;

	UI32 multiid = this->id() - 0x4000;

	int j;
	SI32 length;
	st_multi multi;
	UOXFile *mfile;
	Map->SeekMulti(multiid, &mfile, &length);
	length=length/sizeof(st_multi);
	if (length == -1 || length>=17000000)//Too big...
	{
		clConsole.log( tr( "cHouse::onValidPlace: Bad length in multi file. Avoiding stall.\n" ).latin1() );
		length = 0;
	}

	SI08 mapz = 0;
	tile_st tile;
	for( j = 0; j < length; j++ )
	{
		mfile->get_st_multi(&multi);
		Coord_cl multipos = Coord_cl( multi.x + pos.x, multi.y + pos.y, pos.z, pos.map );

		mapz = Map->MapElevation( multipos );
		if( pos.z < mapz )
			return false;

		land_st mapTile = cTileCache::instance()->getLand( Map->SeekMap( multipos ).id );
		if( mapTile.flag1 & 0x40 || mapTile.flag1 & 0x80 )
			return false;
		
		MapStaticIterator msi( multipos );
		staticrecord *stat = msi.Next();
		while( stat != NULL )
		{
			msi.GetTile( &tile );
			if( multi.z > stat->zoff && multi.z < (stat->zoff + tile.height) )
				return false;
			stat = msi.Next();
		}
		
		RegionIterator4Items ri( multipos );
		for( ri.Begin(); !ri.atEnd(); ri++ ) 
		{
			P_ITEM pi = ri.GetData();
			if( pi && pi->multis != serial )
			{
				tile = cTileCache::instance()->getTile( pi->id() );
				if( multi.z > pi->pos.z && multi.z < ( pi->pos.z + tile.height ) )
					return false;
			}
		}
	}
	return true;
}

void cHouse::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = this->getNodeValue( Tag );

	// <id>16572</id>
	if( TagName == "id" )
		this->setId( Value.toUShort() );

	// <movechar x="0" y="0" z="8" />
	else if( TagName == "movechar" )
	{
		charpos_.x = Tag.attribute( "x" ).toInt();
		charpos_.y = Tag.attribute( "y" ).toInt();
		charpos_.z = Tag.attribute( "z" ).toInt();
	}

	// <item>
	//     <inherit id="houseitem_a" />
	// </item>
	else if( TagName == "item" )
	{
		processHouseItemNode( Tag );
	}

	// <nokey />
	else if( TagName == "nokey" )
		nokey_ = true;

	else
		cMulti::processNode( Tag );
}

void cHouse::build( const QDomElement &Tag, UI16 posx, UI16 posy, SI08 posz, SERIAL senderserial, SERIAL deedserial )
{
	P_CHAR pc_currchar = FindCharBySerial( senderserial );
	cUOSocket* socket = pc_currchar->socket();

	this->serial = cItemsManager::getInstance()->getUnusedSerial();
	cItemsManager::getInstance()->registerItem( this );
	this->SetOwnSerial( senderserial );
	this->priv = 0;
	this->MoveTo( posx, posy, posz );

	this->applyDefinition( Tag );
	if( !this->onValidPlace() )
	{
		if( socket )
			socket->sysMessage( tr("Can not build a house at this location!") );
		this->remove();
		Items->DeleItem( this );
		return;
	}
	this->update();

	P_ITEM pDeed = FindItemBySerial( deedserial );
	if( pDeed != NULL )
		Items->DeleItem( pDeed );

	if( !nokey_ )
	{
		createKeys( pc_currchar, tr("house key") );
	}

	RegionIterator4Items ri(this->pos);
	for(ri.Begin(); !ri.atEnd(); ri++)
	{
		P_ITEM si = ri.GetData();
		si->update();
	}
		
	pc_currchar->MoveTo( this->pos.x + charpos_.x, this->pos.y + charpos_.y, this->pos.z + charpos_.z );
	pc_currchar->resend();
}

void cHouse::remove( void )
{
	removeKeys();
	RegionIterator4Chars ri(this->pos);
	for (ri.Begin(); !ri.atEnd(); ri++)
	{
		P_CHAR pc = ri.GetData();
		if(pc->npcaitype() == 17 && pc->multis == this->serial)
			Npcs->DeleteChar(pc);
	}
	RegionIterator4Items rii(this->pos);
	for(rii.Begin(); !rii.atEnd(); rii++)
	{
		P_ITEM pi = rii.GetData();
		if(pi->multis == this->serial && pi->serial != this->serial && pi->type() != 202)
			Items->DeleItem(pi);
	}
}

void cHouse::toDeed( cUOSocket* socket )
{
	P_CHAR pc_currchar = socket->player();
	if( !pc_currchar )
		return;
	P_ITEM pBackpack = pc_currchar->getBackpack();

	RegionIterator4Chars ri(this->pos);
	for (ri.Begin(); !ri.atEnd(); ri++)
	{
		P_CHAR pc = ri.GetData();
		if( pBackpack && pc->npcaitype() == 17 && pc->multis == this->serial )
		{
			P_ITEM pPvDeed = Items->createScriptItem( "14f0" );
			if( pPvDeed )
			{
				pPvDeed->setName( tr("A vendor deed for %1").arg( pc->name.c_str() ) );
				pPvDeed->setType( 217 );
				pPvDeed->value = 2000;
				pPvDeed->update();
				socket->sysMessage( tr("Packed up vendor %1.").arg(pc->name.c_str()) );
			}
		}
	}

	this->remove();
	
	if( this->deedsection_.isNull() || this->deedsection_.isEmpty() || !pBackpack )
	{
		Items->DeleItem( this );
		return;
	}

	P_ITEM pDeed = Items->createScriptItem( this->deedsection_ );
	if( pDeed ) 
	{
		pBackpack->AddItem( pDeed );
		pDeed->update();
	}
	Items->DeleItem( this );
	socket->sysMessage( tr("You turned the boat into a deed.") );
}

void cHouse::Serialize(ISerialization &archive)
{
	if (archive.isReading())
	{
		archive.read( "nokey", nokey_ );
		archive.read( "charpos.x", charpos_.x );
		archive.read( "charpos.y", charpos_.y );
		archive.read( "charpos.z", charpos_.z );
	}
	else if ( archive.isWritting())
	{
		archive.write( "nokey", nokey_ );
		archive.write( "charpos.x", charpos_.x );
		archive.write( "charpos.y", charpos_.y );
		archive.write( "charpos.z", charpos_.z );
	}
	cMulti::Serialize(archive); // Call base class method too.
}

QString cHouse::objectID() const
{
	return "HOUSE";
}




