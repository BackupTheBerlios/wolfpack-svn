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

#include "spellbook.h"
#include "prototypes.h"
#include "chars.h"
#include "network/uosocket.h"
#include "newmagic.h"

void cSpellBook::Init( bool mkser )
{
	cItem::Init( mkser );
	spells1_ = 0;
	spells2_ = 0;
}

void cSpellBook::addSpell( UINT8 spell )
{
	// Lower 32 bits
	if( spell < 32 )
	{
		UINT32 bitmask = 1 << spell;
		spells1_ |= bitmask;
	}
	// Upper 32 bits
	else
	{
		UINT32 bitmask = 1 << ( spell - 32 );
		spells2_ |= bitmask;
	}
}

void cSpellBook::removeSpell( UINT8 spell )
{
	// Get a bitmask and reverse it
	UINT32 bitmask;

	if( spell < 32 )
	{
		bitmask = 0xFFFFFFFF - ( 1 << spell );
		spells1_ &= bitmask;
	}
	else if( spell < 64 )
	{
		bitmask = 0xFFFFFFFF - ( 1 << ( spell - 32 ) );
		spells2_ &= bitmask;
	}
}

bool cSpellBook::hasSpell( UINT8 spell )
{
	bool found = false;
	UINT32 bitmask;

	if( spell < 32 )
	{
		bitmask = 1 << spell;
		found = spells1_ &  bitmask;
	}
	else if( spell < 64 )
	{
		bitmask = 1 << ( spell - 32 );
		found = spells2_ & bitmask;
	}

	return found;
}

// abstract cSerializable
void cSpellBook::Serialize( ISerialization &archive )
{
	if( archive.isReading() )
	{
		archive.read( "spells1", spells1_ );
		archive.read( "spells2", spells2_ );
	}
	else if( archive.isWritting() )
	{
		archive.write( "spells1", spells1_ );
		archive.write( "spells2", spells2_ );
	}

	cItem::Serialize( archive );
}

// abstract cDefinable
void cSpellBook::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = this->getNodeValue( Tag );

	// Add a spell
	if( TagName == "addspell" )
		addSpell( hex2dec( Value ).toShort() );

	// Remove a spell
	else if( TagName == "removespell" )
		removeSpell( hex2dec( Value ).toShort() );

	else
		cItem::processNode( Tag );
}

bool cSpellBook::onUse( cUObject *Target )
{
	// Check existing scripts first
	if( cItem::onUse( Target ) )
		return true;

	// We assume that target is a character here
	P_CHAR pChar = dynamic_cast< P_CHAR >( Target );

	if( !pChar || !pChar->socket() )
		return true;

	SERIAL sSerial = 0xFFFFFFFF;

	// First we send a multi-object-to-container packet and then open the gump
	cUOTxItemContent content;

	for( UINT8 i = 0; i < 64; ++i )
	{
		sSerial--;		
		content.addItem( sSerial, NewMagic->calcScrollId( i ), 0, 0, 0, 1, serial );
	}

	pChar->socket()->send( &content );
	
	cUOTxDrawContainer drawcont;
	drawcont.setGump( 0xFFFF );
	drawcont.setSerial( serial );
	pChar->socket()->send( &drawcont );

	return true;
}

bool cSpellBook::onSingleClick( P_CHAR Viewer )
{
	// See if there are scripted events, if not do it our way.
	if( cItem::onSingleClick( Viewer ) )
		return true;
	
	// Show the amount of spells in this spellbook and return false so the normal name is shown as well
	if( !Viewer->socket() )
		return false;

	Viewer->socket()->showSpeech( this, tr( "%1 spells" ).arg( spellCount() ) );

	return false;
}

UINT8 cSpellBook::spellCount()
{
	UINT8 count = 0;

	for( UINT8 i = 0; i < 64; ++i )
		if( hasSpell( i ) )
			++count;

	return count;
}
