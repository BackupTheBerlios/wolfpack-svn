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
//	Wolfpack Homepage: http://wpdev.sf.net/
//========================================================================================

// WorldMain.cpp: implementation of the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#include "worldmain.h"
#include "progress.h"
#include "charsmgr.h"
#include "corpse.h"
#include "itemsmgr.h"
#include "TmpEff.h"
#include "guildstones.h"
#include "mapobjects.h"
#include "srvparams.h"
#include "chars.h"
#include "pfactory.h"
#include "iserialization.h"
#include "wolfpack.h"
#include "utilsys.h"
#include "maps.h"
#include "territories.h"
#include "accounts.h"
#include "books.h"
#include "multis.h"
#include "spellbook.h"
#include "persistentbroker.h"
#include "network/uopacket.h"

// Library Includes
#include <qsqlcursor.h>
#include <qcstring.h>
#include <mysql.h>

#undef  DBGFILE
#define DBGFILE "worldmain.cpp"


// Items Saver thread
void CWorldMain::cItemsSaver::run() throw()
{
	try
	{
		waitMutex.acquire();
		AllItemsIterator iterItems;
		for (iterItems.Begin(); !iterItems.atEnd(); ++iterItems)
		{
			P_ITEM pi = iterItems.GetData();
			if( pi && pi->objectID() != "RESOURCEITEM" )
				persistentBroker->saveObject(pi);
		}
		waitMutex.release();
	}
	catch( ... )
	{
		waitMutex.release();
	}
}

void CWorldMain::cItemsSaver::wait()
{
	try
	{
		waitMutex.acquire();
		waitMutex.release();
	}
	catch ( ... )
	{
		waitMutex.release();
	}
}

CWorldMain::CWorldMain()
{
	announce(false);

	iWsc = cWsc = NULL;
	Cur = 0;
	PerLoop = -1;
	isSaving = false;
}

CWorldMain::~CWorldMain()
{
	if (iWsc)
		fclose(iWsc);
	if (cWsc)
		fclose(cWsc);
}


void CWorldMain::loadnewworld(QString module) // Load world
{
	ISerialization* archive = cPluginFactory::serializationArchiver(module);

	QString objectID;
	register unsigned int i = 0;

	// Load cItem
	clConsole.send( "Loading World... " );
	
	MYSQL mysql;
	mysql_init(&mysql);
	mysql_options(&mysql,MYSQL_OPT_COMPRESS,0);

	if( !mysql_real_connect(&mysql,"localhost","root","","wolfpack",0,NULL,0))
	{
		fprintf( stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql) );
		return;
	}

	mysql_query( &mysql, UObjectFactory::instance()->findSqlQuery( "cItem" ).latin1() );
	MYSQL_RES *result = mysql_use_result( &mysql );
	MYSQL_ROW row;

	UINT32 sTime = getNormalizedTime();
	UINT16 offset;
	cUObject *object;

	// Fetch row-by-row
	while( ( row = mysql_fetch_row( result ) ) )
	{
		// do something with data
		object = UObjectFactory::instance()->createObject( "cItem" );
		offset = 2; // Skip the first two fields
		object->load( row, offset );
		++i;

		if( i % 1000 == 0 )
			qWarning( QString::number( i ) );
	}

	mysql_free_result( result );
	mysql_close(&mysql);

	printf( "Took: %i msecs", getNormalizedTime() - sTime );

	/*QStringList types = UObjectFactory::instance()->objectTypes();

	QSqlQuery query;
	query.setForwardOnly(true);
	for( INT32 j = 0; j < types.count(); ++j )
	{
		QString type = types[j];
		QSqlQuery query;
			UINT16 offset;
			cUObject *object;
			query.exec( UObjectFactory::instance()->findSqlQuery( type ) );
			while( query.isActive() && query.next() )
			{
				object = UObjectFactory::instance()->createObject( type );
				offset = 2; // Skip the first two fields
				object->load( &query, offset );
			}

	}*/

	clConsole.send( "%i loaded\n", i );

	// Load Temporary Effects
	archive = cPluginFactory::serializationArchiver(module);

	archive->prepareReading( "effects" );
	clConsole.send("Loading Temp. Effects %i...\n", archive->size());
	progress_display progress( archive->size() );
	for ( i = 0; i < archive->size(); ++progress, ++i)
	{
		archive->readObjectID(objectID);

		cTempEffect* pTE = NULL;

		if( objectID == "TmpEff" )
			pTE = new cTmpEff;

		else if( objectID == "PythonEffect" )
			pTE = new cPythonEffect;

		else if( objectID == "HIDECHAR" )
			pTE = new cDelayedHideChar( INVALID_SERIAL );

		else		
			continue; // an error occured..

		archive->readObject( pTE );

		cTempEffects::getInstance()->insert( pTE );
	}
	clConsole.send(" Done.\n");
	archive->close();

	delete archive;
}

//o---------------------------------------------------------------------------o
//|	Class		:	CWorldMain::savenewworld(char x)
//|	Date		:	Unknown
//|	Programmer	:	Unknown		
//o---------------------------------------------------------------------------o
//| Purpose		:	Save current world state. Stores all values in an easily
//|					readable script file "*.wsc". This stores all world items
//|					and NPC/PC character information for a given shard
//o---------------------------------------------------------------------------o
void CWorldMain::savenewworld(QString module)
{
	static unsigned long ocCount, oiCount;
	UI32 savestarttime = getNormalizedTime();

//	AllTmpEff->Off();

	if ( !Saving() )
	{
		//	gcollect();
		if ( announce() )
		{
			sysbroadcast("World data saving....");
			clConsole.send("Worldsave Started!\n" );
			clConsole.send("items  : %i\n", cItemsManager::getInstance()->size());
			clConsole.send("chars  : %i\n", cCharsManager::getInstance()->size());
			clConsole.send("effects: %i\n", cTempEffects::getInstance()->size());
		}
		isSaving = true;
	}


	cItemsSaver ItemsThread(module);
	ItemsThread.run(); // thread disabled for now.

	SrvParams->flush();
	if (SrvParams->serverLog()) savelog("Server data save\n","server.log");

	ISerialization* archive = cPluginFactory::serializationArchiver( module );
	archive->prepareWritting( "chars" );
	AllCharsIterator iterChars;
	for (iterChars.Begin(); !iterChars.atEnd(); ++iterChars)
	{
//		archive->writeObject( iterChars.GetData() );
		persistentBroker->saveObject(iterChars.GetData());
	}
	archive->close();
	delete archive;

	archive = cPluginFactory::serializationArchiver( module );
	archive->prepareWritting( "effects" );
	cTempEffects::getInstance()->serialize( *archive );
	archive->close();
	delete archive;

	// Save the accounts
	clConsole.PrepareProgress( tr( "Saving %1 accounts" ).arg( Accounts->count() ).latin1() );
	Accounts->save();
	clConsole.ProgressDone();
	
//	ItemsThread.join();

	if ( announce() )
	{
		sysbroadcast("Worldsave Done!\n");
		char temp[128];
		sprintf( temp, "World saved in %.03f sec", (float)(((float)getNormalizedTime() - (float)savestarttime) / CLOCKS_PER_SEC ) );
		clConsole.PrepareProgress( temp );
		clConsole.ProgressDone();
	}

	isSaving = false;

	uiCurrentTime = getNormalizedTime();
}

int CWorldMain::announce()
{
	return DisplayWorldSaves;
}

void CWorldMain::announce(int choice)
{
	if(choice<1)
		DisplayWorldSaves=0;
	else
		DisplayWorldSaves=1;
}

bool CWorldMain::Saving( void )
{
	return isSaving;
}

void CWorldMain::SaveChar( P_CHAR pc )
{
}

static void decay1(P_ITEM pi, P_ITEM pItem)
{
	long serial;
	if( pi->corpse() == 1 )
	{
		serial=pi->serial;
		unsigned int ci;
		vector<SERIAL> vecContainer( contsp.getData(pi->serial) );
		for (ci=0;ci<vecContainer.size();ci++)
		{
			P_ITEM pi_j = FindItemBySerial(vecContainer[ci]);
			if( pi_j != NULL )
			{
				if ((pi_j->contserial==pi->serial) &&
					(pi_j->layer()!=0x0B)&&(pi_j->layer()!=0x10))
				{
					pi_j->setContSerial(-1);
					pi_j->moveTo(pi->pos);
					Items->DeleItem(pi_j);
				}
			}
		}
	}
	else
	{
		if ( pi->multis == INVALID_SERIAL )
		{
			cMulti* pi_multi = cMulti::findMulti( pi->pos );
			if( !pi_multi )
			{
				Items->DeleItem(pItem);
			}
		}
	}
}

void CWorldMain::SaveItem( P_ITEM pi, P_ITEM pDefault)
{
}

//o--------------------------------------------------------------------------
//|	Function		-	int CWorldMain::RemoveItemFromCharBody(int charserial, int type1, int type2);
//|	Date				-	Unknown
//|	Programmer	-	Abaddon
//o--------------------------------------------------------------------------
//|	Purpose			-	Remove the specified item from a characters paper doll
//o--------------------------------------------------------------------------
//|	Returns			-	true if removed, false otherwise
//o--------------------------------------------------------------------------
bool CWorldMain::RemoveItemsFromCharBody( int charserial, int type1, int type2 )
{ 
	int serial;
	P_CHAR pc = FindCharBySerial(charserial);
	if (pc == NULL) return false;
 	serial= pc->serial;
 	bool foundMatch = false;
	unsigned int ci;
	vector<SERIAL> vecContainer = contsp.getData(serial);
	for (ci=0;ci<vecContainer.size();ci++)
 	{
		P_ITEM pci = FindItemBySerial(vecContainer[ci]);

 		if (pci != NULL)
 		{
			if( ( pci->layer() == 0x01 || pci->layer() == 0x02 ) && ( pci->contserial == serial ) )
 			{
 				// in first hand, or second hand
 				if( ( pci->id() >> 8 ) == type1 && ( pci->id()&0x00FF ) == type2 )
				{
 					Items->DeleItem( pci );
 					foundMatch = true;
 				}
 			}
 		}
	}
	return foundMatch;
}
