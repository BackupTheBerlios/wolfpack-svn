//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//  Copyright 2001-2004 by holders identified in authors.txt
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
//	Foundation, Inc., 59 Temple Palace - Suite 330, Boston, MA 02111-1307, USA.
//
//	* In addition to that license, if you are running this program or modified
//	* versions of it on a public system you HAVE TO make the complete source of
//	* the version used by you available or provide people with a location to
//	* download it.
//
//
//
//	Wolfpack Homepage: http://wpdev.sf.net/
//==================================================================================

#if !defined(__MULTIS_H__)
#define __MULTIS_H__

// Platform specifics
#include "platform.h"

// Library includes
#include <qvaluelist.h>
#include <qptrvector.h>
#include <qstringlist.h>

#include "uobject.h"
#include "items.h"
#include "gumps.h"
#include "basechar.h"

// Beware! This is abstract (more or less)
class cMulti : public cItem
{
protected:
	static void buildSqlString( QStringList &fields, QStringList &tables, QStringList &conditions );

public:
	const char *objectID() const
	{
        return "cMulti";
	}

	cMulti();
	virtual void toDeed( cUOSocket* socket ) = 0;

	// DB Serialization
	void load( char **, UINT16& );
	void save();
	bool del();

	QString deedSection( void ) { return deedsection_; }
	bool	itemsdecay( void ) { return itemsdecay_; }
	void	setName( const QString nValue );

	P_CHAR	coOwner( void );
	void	setCoOwner( P_CHAR pc );

	void	addItem( P_ITEM pi );
	void	removeItem( P_ITEM pi );
	void	checkItems();
	void	addChar( P_CHAR pc );
	void	removeChar( P_CHAR pc );
	void	checkChars();

	bool	isBanned( P_CHAR pc );
	void	addBan( P_CHAR pc );
	void	removeBan( P_CHAR pc );
	void	removeBan( SERIAL serial );
	bool	isFriend( P_CHAR pc );
	void	addFriend( P_CHAR pc );
	void	removeFriend( P_CHAR pc );
	void	removeFriend( SERIAL serial );

	QValueList< SERIAL >	chars()		const { return chars_; }
	QValueList< SERIAL >	items()		const { return items_; }
	std::vector<SERIAL>	friends()	const { return friends_; }
	std::vector<SERIAL>	bans()		const { return bans_; }

	P_ITEM	findKey( P_CHAR pc );
	void	createKeys( P_PLAYER pc, const QString &name );
	void	removeKeys( void );

	bool	authorized( P_PLAYER pc );

	bool	inMulti( const Coord_cl &srcpos );

	static cMulti*	findMulti( const Coord_cl &pos );
	static bool		inMulti( const Coord_cl &srcpos, const Coord_cl &multipos, UI16 id );

	virtual bool	ishouse()	{ return false; }
	virtual bool	isboat()		{ return false; }
	virtual UINT32	revision() { return 0; }

	virtual void	addCHTile( UINT16 model, Coord_cl pos) { Q_UNUSED(model); Q_UNUSED(pos); }
	virtual void	clearTiles( void ) {}
	virtual void	sendCH( cUOSocket* socket ) {}


protected:
	virtual void processNode( const cElement *Tag );

protected:
	QString deedsection_;
	bool	itemsdecay_;
	SERIAL	coowner_;
	std::vector<SERIAL>	friends_;
	std::vector<SERIAL>	bans_;
	QValueList< SERIAL >	items_;
	QValueList< SERIAL >	chars_;
};

class cMultiGump : public cGump
{
private:
	SERIAL char_;
	SERIAL multi_;
	QPtrVector< cBaseChar > friends;
	QPtrVector< cBaseChar > bans;

public:
	cMultiGump( SERIAL charSerial, SERIAL multiSerial );

	virtual void handleResponse( cUOSocket* socket, const gumpChoice_st& choice );
};

#endif
