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

#ifndef __RESOURCES_H
#define __RESOURCES_H

#include "definable.h"
#include "items.h"
#include "wptargetrequests.h"
#include "typedefs.h"
#include "singleton.h"

// Library includes
#include <qvaluevector.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qobject.h>
#include <set>

class cResource : public cDefinable
{
public:
	cResource( const cElement *Tag );

	struct convertspec_st
	{
		QValueVector< UINT16 > sourcecolors;
		QValueVector< UINT16 > sourceids;
		float rate;
	};

	struct resourcespec_st
	{
		QString name;
		QString definition;
		UINT32 minamount_per_attempt;
		UINT32 maxamount_per_attempt;
		QValueVector< UINT16 > ids;
		QValueVector< UINT16 > colors;
		QValueVector< UINT16 > artids;
		QValueVector< UINT16 > mapids;
		UINT32 vein_minamount;
		UINT32 vein_maxamount;
		UINT32 vein_quota;
		UINT16 minskill;
		UINT16 maxskill;
		UINT32 quota;
		convertspec_st conversion;
		float makeuseamountmod;
		float makeskillmod;
	};

	// implements cDefinable
	virtual void processNode( const cElement *Tag );

	// Getters
	bool	deleteSource()	const { return deletesource_; }
	QValueVector< resourcespec_st > resourceSpecs() const { return resourcespecs_; }
	QString	name()	const { return name_; }

	bool	hasArtId( UINT16 id );
	bool	hasMapId( UINT16 id );

	void	handleFindTarget( cUOSocket* socket, Coord_cl pos, UINT16 mapid, UINT16 artid );
	void	handleConversionTarget( cUOSocket* socket, Coord_cl pos, cItem* pSource, UINT16 mapid, UINT16 artid );

private:
	UINT32	amountmin_;
	UINT32	amountmax_;
	UINT32	veinchance_;
	UINT8	skillid_;
	UINT32	totalquota_;
	UINT32	totalveinquota_;
	UINT32	refreshtime_;
	QString section_;
	QString	name_;
	bool	deletesource_;
	UINT32	staminamin_;
	UINT32	staminamax_;
	UINT8	charaction_;
	UINT16	sound_;
	QString failmsg_;
	QString succmsg_;
	QString emptymsg_;

	QValueVector< resourcespec_st > resourcespecs_;
	QValueVector< UINT16 > mapids_;
	QValueVector< UINT16 > artids_;
	std::set< UINT16 > sourceids_;
	std::set< UINT16 > sourcecolors_;
};

class cResourceItem : public cItem
{
public:
	cResourceItem( const QString& resource, UINT32 amount, UINT32 vein );
	cResourceItem() {} // Used for Loading

	static void registerInFactory();
	
	// Getters
	QString	resource()	const	{ return resource_; }

private:
	QString		resource_;
};

class cAllResources : public std::map< QString, cResource* >
{
public:
	cAllResources();
	~cAllResources();
	void load();
	void unload();
	void reload();
};

typedef SingletonHolder<cAllResources> Resources;

class cFindResource : public cTargetRequest
{
public:
	cFindResource( QString resname ) : resourcename_( resname ), tool_( INVALID_SERIAL ) {}
	virtual bool responsed( cUOSocket *socket, cUORxTarget *target );

	void setTool( SERIAL data ) { tool_ = data; }
private:
	QString resourcename_;
	SERIAL tool_;
};

class cConvertResource : public cTargetRequest
{
public:
	cConvertResource( QString resname, cItem* pi ) : resourcename_( resname ), sourceserial_( pi->serial() ) {}
	virtual bool responsed( cUOSocket *socket, cUORxTarget *target );

private:
	QString resourcename_;
	SERIAL	sourceserial_;
};

#endif

