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
 * Wolfpack Homepage: http://developer.berlios.de/projects/wolfpack/
 */

#include "serverconfig.h"

#include "log.h"
#include "verinfo.h"
#include "basedef.h"
#include "inlines.h"
#include "preferences.h"

// Library Includes
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qhostaddress.h>
#include <qdatetime.h>

#include <qglobal.h>
#if defined( Q_OS_WIN32 )
# include <winsock.h>
#elif defined ( Q_OS_UNIX )
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
#endif

const char preferencesFileVersion[] = "1.0";

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

Q_INT32 resolveName( const QString& data )
{
	if ( data.isEmpty() )
		return INADDR_NONE;

	// we do a dns lookup on this

	Q_UINT32 uiValue = inet_addr( ( char* ) data.latin1() ) ;
	if ( uiValue == INADDR_NONE )
	{
		hostent* ptrHost = gethostbyname( ( char* ) data.latin1() );
		if ( ptrHost != 0 )
		{
			char** ptrPtr = ptrHost->h_addr_list;
			//We only use the first one
			if ( *ptrPtr != NULL )
			{
				//  	I can think of no other way
				memcpy( &uiValue, *ptrPtr, sizeof( in_addr ) ) ;
			}
		}
	}

	// inet_addr returns the ip in reverse order
	return ntohl( uiValue );
}

cConfig::cConfig() : Preferences( "wolfpack.xml", "Wolfpack", "1.0" )
{
}

void cConfig::readData()
{
	Preferences::readData();

	// Account Group
	autoAccountCreate_ = getBool( "Accounts", "Auto Create", false, true );
	accountsDriver_ = getString( "Accounts", "Database Driver", "sqlite", true );
	accountsHost_ = getString( "Accounts", "Database Host", "", true );
	accountsName_ = getString( "Accounts", "Database Name", "accounts.db", true );
	accountsUsername_ = getString( "Accounts", "Database Username", "", true );
	accountsPassword_ = getString( "Accounts", "Database Password", "", true );
	hashAccountPasswords_ = getBool( "Accounts", "Use MD5 Hashed Passwords", false, true );
	convertUnhashedPasswords_ = getBool( "Accounts", "Automatically Hash Loaded Passwords", false, true );
	maxCharsPerAccount_ = wpMin<unsigned char>( 6, getNumber( "Accounts", "Maximum Number of Characters", 6, true ) );

	// AI
	checkAITime_ = getDouble( "AI", "Default AI Check Time", 0.5, true );
	animalWildFleeRange_ = getNumber( "AI", "Wild animals flee range", 8, true );
	guardDispelTime_ = getNumber( "AI", "Guard dispel time (sec)", 25, true );

	// Persistency
	databasePassword_ = getString( "Database", "Database Password", "", true );
	databaseUsername_ = getString( "Database", "Database Username", "", true );
	databaseHost_ = getString( "Database", "Database Host", "", true );
	databaseDriver_ = getString( "Database", "Database Driver", "binary", true );
	databaseName_ = getString( "Database", "Database Name", "world.db", true );

	// Repsys
	murderdecay_ = getNumber( "Repsys", "Murder Decay", 28800, true );
	maxkills_ = getNumber( "Repsys", "Max Kills", 5, true );
	crimtime_ = getNumber( "Repsys", "Criminal Time", 120, true );

	// Game Speed Group
	objectDelay_ = getNumber( "Game Speed", "ObjectDelay", 1, true );
	checkNPCTime_ = getDouble( "Game Speed", "NPCs Check Time", 0.8, true );
	checkTamedTime_ = getDouble( "Game Speed", "Tamed Check Time", 0.6, true );
	niceLevel_ = getNumber( "Game Speed", "Nice Level", 2, true );
	skillDelay_ = getNumber( "Game Speed", "SkillDelay", 7, true );
	skillLevel_ = getNumber( "Game Speed", "SkillLevel", 3, true );
	maxStealthSteps_ = getNumber( "Game Speed", "Max Stealth Steps", 10, true );
	runningStamSteps_ = getNumber( "Game Speed", "Running Stamina Steps", 15, true );
	hungerRate_ = getNumber( "Game Speed", "Hunger Rate", 120, true );
	hungerDamageRate_ = getNumber( "Game Speed", "Hunger Damage Rate", 10, true );
	boatSpeed_ = getDouble( "Game Speed", "Boat Speed", 0.750000, true );
	snoopdelay_ = getNumber( "Game Speed", "Snoop Delay", 7, true );
	housedecay_secs_ = getNumber( "Game Speed", "House Decay-Sec.", 604800, true );
	spawnRegionCheckTime_ = getNumber( "Game Speed", "SpawnRegion Check Time", 5, true );
	itemDecayTime_ = getNumber( "Game Speed", "Item Decay Time", 300, true );
	corpseDecayTime_ = getNumber( "Game Speed", "Corpse Decay Time", 600, true );

	secondsPerUOMinute_ = getNumber( "Game Speed", "Seconds Per UO Minute", 5, true );
	npcMoveTime_ = getDouble( "Game Speed", "NPC Move Time", 0.8, true );
	tamedNpcMoveTime_ = getDouble( "Game Speed", "Tamed NPC Move Time", 0.6, true );

	// General Group
	enableFeluccaSeason_ = getBool( "General", "Enable Felucca Season", true, true );
	enableTrammelSeason_ = getBool( "General", "Enable Trammel Season", true, true );
	showSkillTitles_ = getBool( "General", "ShowSkillTitles", true, true );
	skillcap_ = getNumber( "General", "SkillCap", 700, true );
	statcap_ = getNumber( "General", "StatsCap", 225, true );
	commandPrefix_ = getString( "General", "Command Prefix", "'", true );
	skillAdvanceModifier_ = getNumber( "General", "Skill Advance Modifier", 1000, true );
	statsAdvanceModifier_ = getNumber( "General", "Stats Advance Modifier", 500, true );
	stealing_ = getBool( "General", "Stealing Enabled", true, true );
	guardsActive_ = getBool( "General", "Guards Enabled", true, true );
	saveSpawns_ = getBool( "General", "Save Spawned Regions", true, true );
	lootdecayswithcorpse_ = getBool( "General", "Loot Decays With Corpse", true, true );
	invisTimer_ = getDouble( "General", "InvisTimer", 60, true );
	poisonTimer_ = getNumber( "General", "PoisonTimer", 180, true );
	hungerDamage_ = getNumber( "General", "Hunger Damage", 0, true );
	tamedDisappear_ = getNumber( "General", "Tamed Disappear", 0, true );
	houseInTown_ = getNumber( "General", "House In Town", 0, true );
	shopRestock_ = getNumber( "General", "Shop Restock", 60, true );
	quittime_ = getNumber( "General", "Char Time Out", 300, true );
	cacheMulFiles_ = getBool( "General", "Cache Mul Files", true, true );
	categoryTagAddMenu_ = getBool( "General", "Build AddMenu by Category Tags", true, true );
	showNpcTitles_ = getBool( "General", "Show Npc Titles", true, true );
	logMask_ = getNumber( "General", "Logging Mask", LOG_ALL, true );
	overwriteDefinitions_ = getBool( "General", "Overwrite Definitions", false, true );
	dontStackSpawnedObjects_ = getBool( "General", "Don't Stack Spawned Objects", true, true );
	antiSpeedHack_ = getBool( "General", "Anti Speed Hack", true, true );
	antiSpeedHackDelay_ = getNumber( "General", "Anti Speed Hack Delay", 175, true );
	antiSpeedHackDelayMounted_ = getNumber( "General", "Anti Speed Hack Delay Mounted", 75, true );
	refreshMaxValues_ = getBool( "General", "Refresh Characters Maximum Values", true, true );

	saveInterval_ = getNumber( "General", "Save Interval", 900, true );
	mulPath_ = QDir::convertSeparators( getString( "General", "MulPath", "./muls/", true ) );
	logPath_ = QDir::convertSeparators( getString( "General", "LogPath", "./logs/", true ) );
	logRotate_ = getBool( "General", "LogRotate", true, true );
	mountRange_ = getNumber( "General", "Mount Range", 2, true );

	// Network
	loginPort_ = getNumber( "Network", "Loginserver Port", 2593, true );
	gamePort_ = getNumber( "Network", "Gameserver Port", 2592, true );
	enableLogin_ = getBool( "Network", "Enable Loginserver", true, true );
	enableGame_ = getBool( "Network", "Enable Gameserver", true, true );
	allowUnencryptedClients_ = getBool( "Network", "Allow Unencrypted Clients", true, true );
	allowStatRequest_ = getBool( "Network", "Allow Stat Request", true, true );

	// Combat
	attackstamina_ = getNumber( "Combat", "Attack Stamina", -2, true );
	attack_distance_ = getNumber( "Combat", "Attack Distance", 13, true );

	// Resources 60*60*12
	resourceitemdecaytime_ = getNumber( "Resources", "ResourceItem Decay Time (not empty)", 43200, true );

	// Light
	worldBrightLevel_ = getNumber( "Light", "World Bright Level", 1, true );
	worldFixedLevel_ = getNumber( "Light", "World Fixed Level", 0, true );
	worldDarkLevel_ = getNumber( "Light", "World Dark Level", 18, true );
	dungeonLightLevel_ = getNumber( "Light", "Dungeon Level", 18, true );

	// Path Finding
	pathfind4Follow_ = getBool( "Path Finding", "Activate for Following", true, true );
	pathfind4Combat_ = getBool( "Path Finding", "Activate for Combat", false, true );
	pathfindMaxIterations_ = getNumber( "Path Finding", "Maximum Iterations during Calculation", 45, true );
	pathfindMaxSteps_ = getNumber( "Path Finding", "Maximum Steps for Calculation", 15, true );
	pathfindFollowRadius_ = getNumber( "Path Finding", "Follow Radius", 10, true );
	pathfindFollowMinCost_ = getDouble( "Path Finding", "Follow min. estimated Cost", 1.5, true );
	pathfindFleeRadius_ = getNumber( "Path Finding", "Flee Radius", 10, true );

	// Binary Saves
	binarySavepath_ = getString( "Binary Save Driver", "Save Path", "world.bin", true );
	binaryBackups_ = getNumber( "Binary Save Driver", "Number Of Backups", 5, true );
	binaryCompressBackups_ = getBool( "Binary Save Driver", "Compress Backups", false, true );
}

void cConfig::load()
{
	// Load data into binary format
	// If value not found, create key.
	readData();

	if ( !containGroup( "StartLocation" ) )
		setDefaultStartLocation();

	if ( !containGroup( "LoginServer" ) )
		setDefaultServerList();

	flush(); // if any key created, save it.

	// Reload Body Info
	CharBaseDefs::instance()->loadBodyInfo();

	cComponent::load();
}

void cConfig::unload()
{
	serverList_.clear();
	startLocation_.clear();
	cComponent::unload();
}

void cConfig::reload()
{
	unload();
	load();
}

std::vector<StartLocation_st>& cConfig::startLocation()
{
	if ( startLocation_.empty() ) // Empty? Try to load
	{
		bool bKeepLooping = true;
		unsigned int i = 1;
		do
		{
			QString tmp = getString( "StartLocation", QString( "Location %1" ).arg( i++ ), "" ).simplifyWhiteSpace();
			bKeepLooping = ( tmp != "" );
			if ( bKeepLooping ) // valid data.
			{
				QStringList strList = QStringList::split( "=", tmp );
				if ( strList.size() == 2 )
				{
					StartLocation_st loc;
					loc.name = strList[0];
					QStringList strList2 = QStringList::split( ",", strList[1].stripWhiteSpace() );
					if ( strList2.size() == 4 )
					{
						loc.pos.x = strList2[0].toUShort();
						loc.pos.y = strList2[1].toUShort();
						loc.pos.z = strList2[2].toShort();
						loc.pos.map = strList2[3].toUShort();
						startLocation_.push_back( loc );
					}
				}
			}
		}
		while ( bKeepLooping );
	}
	return startLocation_;
}

void cConfig::setDefaultStartLocation()
{
	setString( "StartLocation", "Location 1", "Yew=567,978,0,1" );
	setString( "StartLocation", "Location 2", "Minoc=2477,407,15,1" );
	setString( "StartLocation", "Location 3", "Britain=1496,1629,10,1" );
	setString( "StartLocation", "Location 4", "Moonglow=4404,1169,0,1" );
	setString( "StartLocation", "Location 5", "Trinsic=1844,2745,0,1" );
	setString( "StartLocation", "Location 6", "Magincia=3738,2223,20,1" );
	setString( "StartLocation", "Location 7", "Jhelom=1378,3817,0,1" );
	setString( "StartLocation", "Location 8", "Skara Brae=594,2227,0,1" );
	setString( "StartLocation", "Location 9", "Vesper=2771,977,0,1" );
	flush(); // save
}

void cConfig::setDefaultServerList()
{
	setString( "LoginServer", "Shard 1", "Your Shard Name=127.0.0.1,2592" );
	flush(); // save.
}

void cConfig::setSecondsPerUOMinute( unsigned int data )
{
	secondsPerUOMinute_ = data;
	setNumber( "Game Speed", "Seconds Per UO Minute", secondsPerUOMinute_ );
	flush();
}

void cConfig::setWorldBrightLevel( unsigned char data )
{
	worldBrightLevel_ = data;
	setNumber( "Light", "World Bright Level", data );
	flush();
}

void cConfig::setWorldFixedLevel( unsigned char data )
{
	worldFixedLevel_ = data;
	setNumber( "Light", "World Fixed Level", data );
	flush();
}

void cConfig::setWorldDarkLevel( unsigned char data )
{
	worldDarkLevel_ = data;
	setNumber( "Light", "World Dark Level", data );
	flush();
}

void cConfig::setDungeonLightLevel( unsigned char data )
{
	dungeonLightLevel_ = data;
	setNumber( "Light", "Dungeon Level", data );
	flush();
}

void cConfig::setMulPath( const QString& data )
{
	mulPath_ = data;
	setString( "General", "MulPath", data );
	flush();
}

void cConfig::setLogPath( const QString& data )
{
	logPath_ = data;
	setString( "General", "LogPath", data );
	flush();
}

unsigned char& cConfig::worldCurrentLevel()
{
	return worldCurrentLevel_;
}

void cConfig::guardsActive( bool enabled )
{
	guardsActive_ = enabled;
	setBool( "General", "Guards Enabled", enabled );
}

struct stGroupDoc
{
	const char* group;
	const char* documentation;
};

QString cConfig::getGroupDoc( const QString& group )
{
	static stGroupDoc group_doc[] =
	{
	{"AI", QT_TR_NOOP( "This group configures the NPC AI." )},
	{"Accounts", QT_TR_NOOP( "This group configures the account management." )},
	{"Database", QT_TR_NOOP( "This group configures access to the worldsave database." )},
	{0, 0}
	};

	// Try to find documentation for a group in our table
	unsigned int i = 0;

	while ( group_doc[i].group )
	{
		if ( group == group_doc[i].group )
		{
			return tr( group_doc[i].documentation );
		}
		++i;
	}

	return Preferences::getGroupDoc( group );
}

struct stEntryDoc
{
	const char* group;
	const char* entry;
	const char* documentation;
};

QString cConfig::getEntryDoc( const QString& group, const QString& entry )
{
	static stEntryDoc entry_doc[] =
	{
	{"Accounts", "Auto Create", QT_TR_NOOP( "If active login attempts with non-existing login names will create a new account automatically\n"
	"This is very usefull for new shards without account policy\n" )},
	{"Accounts", "Database Driver", QT_TR_NOOP( "Possible values are: sqlite, mysql" )},
	{"Accounts", "Maximum Number of Characters", QT_TR_NOOP( "Should not be more than 6, due to client restrictions" )},
	{"Accounts", "Use MD5 Hashed Passwords", QT_TR_NOOP( "This will store hashed passwords, increasing password security." )},
	{"Accounts", "Automatically Hash Loaded Passwords", QT_TR_NOOP( "If active, will convert older plain text passwords into MD5 hash" )},
	{0, 0, 0}
	};

	// Try to find documentation for an entry in our table
	unsigned int i = 0;

	while ( entry_doc[i].group && entry_doc[i].entry )
	{
		if ( group == entry_doc[i].group && entry == entry_doc[i].entry )
		{
			return tr( entry_doc[i].documentation );
		}
		++i;
	}

	return Preferences::getEntryDoc( group, entry );
}
