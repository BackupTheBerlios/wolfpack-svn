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

#include "accounts.h"
#include "ai/ai.h"
#include "basedef.h"
#include "basics.h"
#include "commands.h"
#include "console.h"
#include "getopts.h"
#include "contextmenu.h"
#include "corpse.h"
#include "inlines.h"
#include "log.h"
#include "muls/maps.h"
#include "muls/multiscache.h"
#include "network/network.h"
#include "npc.h"
#include "player.h"
#include "scriptmanager.h"
#include "sectors.h"
#include "action.h"
#include "skills.h"
#include "spawnregions.h"
#include "serverconfig.h"
#include "territories.h"
#include "muls/tilecache.h"
#include "timing.h"
#include "verinfo.h"
#include "world.h"
#include "definitions.h"
#include "multi.h"
#include "persistentbroker.h"

// System Includes
#include <qwaitcondition.h>
#include <qptrvector.h>
#include <qapplication.h>
#include "python/engine.h"
#include "verinfo.h"
#include "network/network.h"

#include "sqlite/sqlite.h"
#if defined(MYSQL_DRIVER)
#if defined(Q_OS_WIN32)
#include <winsock.h>
#endif
#include <mysql.h>
#endif

cComponent::cComponent()
{
	this->loaded = false;
}

cComponent::~cComponent()
{
}

void cComponent::reload()
{
	unload();
	load();
}

void cComponent::load()
{
	loaded = true;
}

void cComponent::unload()
{
	loaded = false;
}

class cServer::Private
{
public:
	QPtrList<cComponent> components;
	bool running;
	enServerState state;
	bool secure;
	QMutex actionMutex;
	unsigned int time;
	QValueVector<cAction*> actionQueue;
	Private() : running( true ), state( STARTUP ), secure( true ), time( 0 )
	{
	}
};

void cServer::queueAction( enActionType type ) {
	switch (type) {
	case RELOAD_SCRIPTS:
		queueAction( new cActionReloadScripts );
		break;
	case RELOAD_PYTHON:
		queueAction( new cActionReloadPython );
		break;
	case RELOAD_ACCOUNTS:
		queueAction( new cActionReloadAccounts );
		break;
	case RELOAD_CONFIGURATION:
		queueAction( new cActionReloadConfiguration );
		break;
	case SAVE_WORLD:
		queueAction( new cActionSaveWorld );
		break;
	case SAVE_ACCOUNTS:
		queueAction( new cActionSaveAccounts );
		break;
	default:
		break;
	}
}

void cServer::queueAction( cAction *action )
{
	if ( d->state == RUNNING )
	{
		QMutexLocker lock( &d->actionMutex );
		d->actionQueue.push_back( action );
	} else {
		delete action; // Delete it right away
	}
}

void cServer::pollQueuedActions()
{
	// Perform Threadsafe Actions
	if ( !d->actionQueue.empty() )
	{
		d->actionMutex.lock();
		while ( d->actionQueue.begin() != d->actionQueue.end() )
		{
			cAction *action = *(d->actionQueue.begin());
			d->actionQueue.erase(d->actionQueue.begin());

			try {
				action->execute();
			} catch ( wpException e ) {
				Console::instance()->log( LOG_PYTHON, e.error() + "\n" );
			}			

			delete action;
		}
		d->actionMutex.unlock();
	}
}

#if defined(CRASHHANDLER)
#include "bugreport/crashhandler.h"

LONG CALLBACK exceptionCatcher( _EXCEPTION_POINTERS* exception )
{
	QString message = GetFaultReason( exception );
	message += "\n";
	message += "Stack Trace:\n";

	DWORD options = GSTSO_PARAMS | GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE;
	const char* buff = GetFirstStackTraceString( options, exception );
	do
	{
		message.append( buff );
		message.append( "\n" );
		buff = GetNextStackTraceString( options, exception );
	}
	while ( buff );

	throw wpException( " " + message );
}
#endif

cServer::cServer()
{
#if defined(CRASHHANDLER)
	SetUnhandledExceptionFilter( exceptionCatcher );
#endif

	d = new Private;

	// Register Components
	registerComponent( Config::instance(), "configuration", true, false );

	// We want to start this independently
	//registerComponent(PythonEngine::instance(), "python", false, true, "configuration");

	registerComponent( Definitions::instance(), "definitions", true, false, "configuration" );
	registerComponent( ScriptManager::instance(), "scripts", true, false, "definitions" );
	registerComponent( ContextMenus::instance(), "contextmenus", true, false, "scripts" );
	registerComponent( SpawnRegions::instance(), "spawnregions", true, false, "definitions" );
	registerComponent( Territories::instance(), "territories", true, false, "definitions" );

	registerComponent( Maps::instance(), "maps", true, false, "configuration" );
	registerComponent( SectorMaps::instance(), "sectormaps", false, true, "maps" );
	registerComponent( TileCache::instance(), "tiledata", true, false, "configuration" );
	registerComponent( MultiCache::instance(), "multis", true, false, "configuration" );

	// Accounts come before world
	registerComponent( Accounts::instance(), "accounts", true, false );
	registerComponent( World::instance(), "world", false, true );

	registerComponent( Network::instance(), "network", true, false );
}

cServer::~cServer()
{
	delete d;
}

bool cServer::isRunning()
{
	return d->running;
}

void cServer::cancel()
{
	d->running = false;
}

void cServer::setSecure( bool secure )
{
	d->secure = secure;
}

bool cServer::getSecure()
{
	return d->secure;
}

bool cServer::run( int argc, char** argv )
{
	bool error = false;
	
	setState( STARTUP );

	QApplication app( argc, argv, false );
	QTranslator translator( 0 );

	// Start Python
	PythonEngine::instance()->load();

	// Parse the parameters.
	Getopts::instance()->parse_options( argc, argv );

	// Print a header and useful version informations
	setupConsole();

	// Load all subcomponents
	load();

	// Start the QT translator
	QString languageFile = Config::instance()->getString( "General", "Language File", "", true );
	if ( !languageFile.isEmpty() )
	{
		if ( !translator.load( languageFile, "." ) )
		{
			Console::instance()->log( LOG_ERROR, "Couldn't load translator.\n" );
			return false;
		}
		qApp->installTranslator( &translator );
	}

	try {
		// Open the Worldsave and Account Database drivers.
		if ( Config::instance()->databaseDriver() != "binary" && !PersistentBroker::instance()->openDriver( Config::instance()->databaseDriver() ) )
		{
			Console::instance()->log( LOG_ERROR, QString( "Unknown Worldsave Database Driver '%1', check your wolfpack.xml" ).arg( Config::instance()->databaseDriver() ) );
			return 1;
		}

		if ( !PersistentBroker::instance()->openDriver( Config::instance()->accountsDriver() ) )
		{
			Console::instance()->log( LOG_ERROR, QString( "Unknown Account Database Driver '%1', check your wolfpack.xml" ).arg( Config::instance()->accountsDriver() ) );
			return 1;
		}

		setState( RUNNING );

		ScriptManager::instance()->onServerStart();	// Notify all scripts about server startup
		Console::instance()->start(); // Notify the console about the server startup

		PyThreadState* _save;
		QWaitCondition niceLevel;
		unsigned char cycles = 0;

		while ( isRunning() )
		{
			// Every 10th cycle we sleep for a while and give other threads processing time.
			if ( ++cycles == 10 )
			{
				cycles = 0;
				_save = PyEval_SaveThread(); // Python threading - start
				switch ( Config::instance()->niceLevel() )
				{
				case 0:
					break;	// very unnice - hog all cpu time
				case 1:
					if ( Network::instance()->count() != 0 )
						niceLevel.wait( 10 );
					else
						niceLevel.wait( 100 ); break;
				case 2:
					niceLevel.wait( 10 ); break;
				case 3:
					niceLevel.wait( 40 ); break;// very nice
				case 4:
					if ( Network::instance()->count() != 0 )
						niceLevel.wait( 10 );
					else
						niceLevel.wait( 4000 ); break; // anti busy waiting
				case 5:
					if ( Network::instance()->count() != 0 )
						niceLevel.wait( 40 );
					else
						niceLevel.wait( 5000 ); break;
				default:
					niceLevel.wait( 10 ); break;
				}
				qApp->processEvents( 40 );
				PyEval_RestoreThread( _save ); // Python threading - end
			}

			pollQueuedActions();

			d->time = getNormalizedTime(); // Update our currenttime

			try
			{
				Network::instance()->poll();
				Timing::instance()->poll();
				Console::instance()->poll();
			}
			catch ( wpException e )
			{
				Console::instance()->log( LOG_PYTHON, e.error() + "\n" );
			}
		}

	} catch (wpException &exception) {
		Console::instance()->log(LOG_ERROR, exception.error() + "\n" );
		error = true;
	} /*catch (...) {
		error = true;
	}*/

	setState( SHUTDOWN );
	Console::instance()->stop();
	ScriptManager::instance()->onServerStop(); // Notify python scripts about shutdown
	Network::instance()->broadcast( "The server is shutting down." );
	unload();

	// Stop Python
	PythonEngine::instance()->unload();

	return !error;
}

void cServer::setupConsole()
{
	Console::instance()->setAttributes( true, false, true, 60, 140, 70, 12, FONT_NOSERIF );
	Console::instance()->send( QString( "\n%1 %2 %3\n\n" ).arg( productString(), productBeta(), productVersion() ) );
	Console::instance()->setAttributes( false, false, false, 0xAF, 0xAF, 0xAF, 0, FONT_FIXEDWIDTH );

	Console::instance()->send( "Copyright (C) 2000-2004 Wolfpack Development Team\n" );
	Console::instance()->send( "Wolfpack Homepage: http://www.wpdev.org/\n" );
	Console::instance()->send( "By using this software you agree to the license accompanying this release.\n" );
	Console::instance()->send( "Compiled on " __DATE__ " " __TIME__ "\n" );
	Console::instance()->send( "Compiled for QT " QT_VERSION_STR " (Using: " );
	Console::instance()->send( qVersion() );
	Console::instance()->send( qSharedBuild() ? " Shared" : " Static" );
	Console::instance()->send( ")\n" );
	QString pythonBuild = Py_GetVersion();
	pythonBuild = pythonBuild.left( pythonBuild.find( ' ' ) );

#if defined(Py_ENABLE_SHARED)
	pythonBuild += " Shared";
#else
	pythonBuild += " Static";
#endif

	Console::instance()->send( "Compiled for Python " PY_VERSION " (Using: " );
	Console::instance()->send( pythonBuild + ")\n" );
	Console::instance()->send( "Compiled with SQLite " SQLITE_VERSION "\n" );
#if defined (MYSQL_DRIVER)
	Console::instance()->send( QString( "Compiled for MySQL " MYSQL_SERVER_VERSION " (Using: %1)\n" ).arg( mysql_get_client_info() ) );
#else
	Console::instance()->send( "MySQL Support: disabled\n" );
#endif
	Console::instance()->send( "\n" );
	QString consoleTitle = QString( "%1 %2 %3" ).arg( productString(), productBeta(), productVersion() );
	Console::instance()->setConsoleTitle( consoleTitle );
}

void cServer::load()
{
	// NPC AI types
	Monster_Aggressive_L0::registerInFactory();
	Monster_Aggressive_L1::registerInFactory();
	Monster_Berserk::registerInFactory();
	Human_Vendor::registerInFactory();
	Human_Stablemaster::registerInFactory();
	Human_Guard::registerInFactory();
	Human_Guard_Called::registerInFactory();
	Normal_Base::registerInFactory();
	Animal_Wild::registerInFactory();
	Animal_Domestic::registerInFactory();

	// Script NPC AI types
	QStringList aiSections = Definitions::instance()->getSections( WPDT_AI );
	QStringList::const_iterator aiit( aiSections.begin() );
	for ( ; aiit != aiSections.end(); ++aiit )
	{
		ScriptAI::registerInFactory( *aiit );
	}

	QPtrList<cComponent>::iterator it( d->components.begin() );
	for ( ; it != d->components.end(); ++it )
	{
		cComponent* component = *it;
		load( component->getName() );
	}

	d->time = getNormalizedTime();
	srand( d->time );
}

void cServer::unload()
{
	QPtrVector<cComponent> vector;
	d->components.toVector( &vector );
	int i;

	for ( i = vector.size() - 1; i >= 0; --i )
	{
		unload( vector[i]->getName() );
	}
}

cComponent* cServer::findComponent( const QString& name )
{
	QPtrList<cComponent>::iterator it( d->components.begin() );
	for ( ; it != d->components.end(); ++it )
	{
		cComponent* component = *it;
		if ( component->getName() == name )
		{
			return component;
		}
	}

	return 0;
}

void cServer::registerComponent( cComponent* component, const QString& name, bool reloadable, bool silent, const QString& depends )
{
	component->setName( name );
	component->setRelodable( reloadable );
	component->setSilent( silent );
	component->setDepends( depends );

	if ( component->getName().isEmpty() )
	{
		throw wpException( "Trying to register a component without a name." );
	}

	if ( findComponent( component->getName() ) != 0 )
	{
		throw wpException( QString( "There already is a component with this name: %1" ).arg( component->getName() ) );
	}

	d->components.append( component );
}

void cServer::load( const QString& name )
{
	cComponent* component = findComponent( name );

	if ( !component )
	{
		throw wpException( QString( "Trying to load an unknown component: %1" ).arg( name ) );
	}

	if ( component->isLoaded() )
	{
		return;
	}

	if ( !component->isSilent() )
	{
		Console::instance()->sendProgress( QString( "Loading %1" ).arg( component->getName() ) );
	}

	component->load();

	if ( !component->isSilent() )
	{
		Console::instance()->sendDone();
	}

	// Find all components depending on this one and load them.
	QPtrList<cComponent>::iterator it( d->components.begin() );
	for ( ; it != d->components.end(); ++it )
	{
		cComponent* subcomponent = *it;
		if ( subcomponent->getDepends() == component->getName() )
		{
			load( subcomponent->getName() );
		}
	}
}

void cServer::unload( const QString& name )
{
	cComponent* component = findComponent( name );

	if ( !component )
	{
		throw wpException( QString( "Trying to unload an unknown component: %1" ).arg( name ) );
	}

	if ( !component->isLoaded() )
	{
		return;
	}

	// Find all components depending on this one and unload them.
	QPtrList<cComponent>::iterator it( d->components.begin() );
	for ( ; it != d->components.end(); ++it )
	{
		cComponent* subcomponent = *it;
		if ( subcomponent->getDepends() == component->getName() )
		{
			unload( subcomponent->getName() );
		}
	}

	if ( !component->isSilent() )
	{
		Console::instance()->sendProgress( QString( "Unloading %1" ).arg( component->getName() ) );
	}

	component->unload();

	if ( !component->isSilent() )
	{
		Console::instance()->sendDone();
	}
}

void cServer::reload( const QString& name )
{
	Server::instance()->setState( SCRIPTRELOAD );

	cComponent* component = findComponent( name );

	if ( !component )
	{
		throw wpException( QString( "Trying to reload an unknown component: %1" ).arg( name ) );
	}

	if ( component->isReloadable() )
	{
		if ( !component->isSilent() )
		{
			Console::instance()->sendProgress( QString( "Reloading %1" ).arg( component->getName() ) );
		}

		component->reload();

		if ( !component->isSilent() )
		{
			Console::instance()->sendDone();
		}
	}

	// Find all components depending on this one and reload them.
	QPtrList<cComponent>::iterator it( d->components.begin() );
	for ( ; it != d->components.end(); ++it )
	{
		cComponent* subcomponent = *it;
		if ( !subcomponent->isReloadable() )
		{
			continue;
		}

		if ( subcomponent->getDepends() == component->getName() )
		{
			reload( subcomponent->getName() );
		}
	}

	Server::instance()->setState( RUNNING );
}

void cServer::setState( enServerState state )
{
	Console::instance()->notifyServerState( state );
	d->state = state;
}

unsigned int cServer::time()
{
	return d->time;
}

void cServer::refreshTime()
{
	d->time = getNormalizedTime();
}

enServerState cServer::getState()
{
	return d->state;
}
