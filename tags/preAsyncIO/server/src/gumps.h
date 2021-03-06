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

#ifndef __GUMPS_H__
#define __GUMPS_H__

#include "typedefs.h"

#include "qstringlist.h"

class cGumps
{
public:
	void Button(int s, int button, SERIAL serial, char type);
	void Input(int s);
	void Menu(UOXSOCKET s, int m, P_ITEM it);
	void Open(int s, P_CHAR pc, int num, int num2);

	static cGumps *instance( void )
	{
		static cGumps instance;
		return &instance;
	}
};

class cGump
{
protected:
	SERIAL serial_, type_;
	Q_INT32 x_,y_;
	QStringList layout_, text_;
	bool noMove_, noClose_, noDispose_;
public:

	SERIAL serial( void ) { return serial_; }
	SERIAL type( void ) { return type_; }
	Q_UINT32 x( void ) { return x_; }
	Q_UINT32 y( void ) { return y_; }
	bool noMove( void ) { return noMove_; }
	bool noDispose( void ) { return noDispose_; }
	bool noClose( void ) { return noClose_; }

	void setType( SERIAL data ) { type_ = data; }
	void setSerial( SERIAL data ) { serial_ = data; }
	void setX( Q_UINT32 data ) { x_ = data; }
	void setY( Q_UINT32 data ) { y_ = data; }
	void setNoMove( bool data ) { noMove_ = data; }
	void setNoClose( bool data ) { noClose_ = data; }
	void setNoDispose( bool data ) { noDispose_ = data; }

	void addRawLayout( const QString &data ) { layout_.push_back( data ); }
	Q_UINT32 addRawText( const QString &data );

	cGump(): noMove_( false ), noClose_( false ), noDispose_( false ), x_( -1 ), y_( -1 ) {}
	virtual ~cGump() {}

	// Convenience Constructors
	cGump( SERIAL nSerial, SERIAL nType, bool bNoMove = false, bool bNoClose = false, bool bNoDispose = false, Q_INT32 nX = -1, Q_INT32 nY = -1 ): 
		serial_( nSerial ), type_( nType ), noMove_( bNoMove ), noClose_( bNoClose ), noDispose_( bNoDispose ), x_( nX ), y_( nY ) {}

	// Comfort Setters
	void startPage( Q_UINT32 pageId = 0 ) { layout_.push_back( QString( "{page %1}" ).arg( pageId ) ); }
	void startGroup( Q_UINT32 groupId = 0 ) { layout_.push_back( QString( "{group %1}" ).arg( groupId ) ); }
	void addText( Q_INT32 textX, Q_INT32 textY, const QString &data, Q_UINT16 hue = 0 ) { layout_.push_back( QString( "{text %1 %2 %3 %4}" ).arg( textX ).arg( textY ).arg( hue ).arg( addRawText( data ) ) ); }
	void addBackground( Q_UINT16 gumpId, Q_UINT32 width, Q_UINT32 height ) { layout_.push_back( QString( "{resizepic 0 0 %1 %2 %3}" ).arg( gumpId ).arg( width ).arg( height ) ); }
	void addResizeGump( Q_INT16 gumpX, Q_INT16 gumpY, Q_UINT16 gumpId, Q_UINT32 width, Q_UINT32 height ) { layout_.push_back( QString( "{resizepic %1 %2 %3 %4 %5}" ).arg( gumpX ).arg( gumpY ).arg( gumpId ).arg( width ).arg( height ) ); }
	
	// Buttons
	// TODO: IMPLEMENTATION
	void addButton( Q_INT32 buttonX, Q_INT32 buttonY, Q_UINT16 gumpUp, Q_UINT16 gumpDown, Q_INT32 returnCode );
	void addPageButton( Q_INT32 buttonX, Q_INT32 buttonY, Q_UINT16 gumpUp, Q_UINT16 gumpDown, Q_INT32 pageId );

	// Gump Pictures
	void addGump( Q_INT32 gumpX, Q_INT32 gumpY, Q_UINT16 gumpId, Q_INT16 hue = -1 ) { layout_.push_back( QString( "{gumppic %1 %2 %3%4}" ).arg( gumpY ).arg( gumpY ).arg( gumpId ).arg( ( hue != -1 ) ? QString( " hue=%1" ).arg( hue ) : "" ) ); }
	void addTiledGump( Q_INT32 gumpX, Q_INT32 gumpY, Q_INT32 width, Q_INT32 height, Q_UINT16 gumpId, Q_INT16 hue ) { layout_.push_back( QString( "{gumppictiled %1 %2 %3 %4 %5%6}" ).arg( gumpY ).arg( gumpY ).arg( gumpId ).arg( width ).arg( height ).arg( ( hue != -1 ) ? QString( " hue=%1" ).arg( hue ) : "" ) ); }

	// Art-tile pictures
	void addTilePic( Q_INT32 tileX, Q_INT32 tileY, Q_UINT16 tileId ) { layout_.push_back( QString( "{tilepic %1 %2 %3%4}" ).arg( tileX ).arg( tileY ).arg( tileId ) ); }	

	// Form-fields
	// 7 = x,y,widthpix,widthchars,wHue,TEXTID,startstringindex
	void addInputField( Q_INT32 textX, Q_INT32 textY, Q_UINT32 width, Q_UINT32 height, Q_INT32 textId, const QString &data, Q_INT16 hue = 0 ) { layout_.push_back( QString( "{textentry %1 %2 %3 %4 %5 %6 %7}" ).arg( textX ).arg( textY ).arg( width ).arg( height ).arg( hue ).arg( textId ).arg( addRawText( data ) ) ); }
	void addCheckbox( Q_INT32 checkX, Q_INT32 checkY, Q_UINT16 gumpOff, Q_UINT16 gumpOn, Q_INT32 returnVal, bool checked = false  ) { layout_.push_back( QString( "{checkbox %1 %2 %3 %4 %5 %6}" ).arg( checkX ).arg( checkY ).arg( gumpOff ).arg( gumpOn ).arg( checked ? 1 : 0 ).arg( returnVal ) ); }
	void addRadioButton( Q_INT32 radioX, Q_INT32 radioY, Q_UINT16 gumpOff, Q_UINT16 gumpOn, Q_INT32 returnVal, bool checked = false  ) { layout_.push_back( QString( "{radio %1 %2 %3 %4 %5 %6}" ).arg( radioX ).arg( radioY ).arg( gumpOff ).arg( gumpOn ).arg( checked ? 1 : 0 ).arg( returnVal ) ); }

	// HTML Stuff
	// void addHtmlGump( );
	// void addXmfHtmlGump( );
	// void addXmfHtmlColorGump( );

	// CheckerTrans
	// void addCheckerTrans( );

	void send( UOXSOCKET socket );
};

void entrygump(int s, SERIAL serial, unsigned char type, char index, short int maxlength, char *text1);

#endif