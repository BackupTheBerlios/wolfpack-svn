/*
 *     Wolfpack Emu (WP)
 * UO Server Emulation Program
 *
 * Copyright 2001-2005 by holders identified in AUTHORS.txt
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

// System Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

// Wolfpack Includes
#include "../accounts.h"
#include "../console.h"
#include "../exportdefinitions.h"
#include "resource.h"
#include "../python/engine.h"
#include "../network/network.h"
#include "../network/uosocket.h"
#include "../player.h"
#include "../server.h"
#include "../inlines.h"

#include <qthread.h>
//Added by qt3to4:
#include <Q3MemArray>
#include "../verinfo.h"

#include "../sqlite/sqlite.h"
#include "../sqlite3/sqlite3.h"
#if defined(MYSQL_DRIVER)
#if defined(Q_OS_WIN32)
#include <winsock.h>
#endif
#include <mysql.h>
#endif

void cConsole::start()
{
}

void cConsole::poll()
{
	// Poll for new Commands
	commandMutex.lock();
	QStringList commands = commandQueue;
	commandQueue.clear();
	commandMutex.unlock();

	while ( commands.count() > 0 )
	{
		handleCommand( commands.front() );
		commands.pop_front();
	}
}

void cConsole::stop()
{
}

void cConsole::rollbackChars( unsigned int count )
{

	// Remove from the end of the linebuffer
	linebuffer_.truncate( linebuffer_.length() - count );
}

void cConsole::send( const QString& sMessage )
{
	// If a progress message is waiting, remove it.
	if ( !progress.isEmpty() )
	{
		QString temp = progress;
		progress = QString::null;
		rollbackChars( temp.length() );
		progress = temp;
	}

	unsigned int textLength = sMessage.length();

	// Append to the linebuffer
	linebuffer_.append( sMessage );

	// Resend the Progress message if neccesary.
	if ( !progress.isEmpty() )
	{
		QString temp = progress;
		progress = QString::null;
		send( temp );
		progress = temp;
	}
	qDebug( qPrintable( sMessage ) );
}

void cConsole::changeColor( enConsoleColors color )
{

	switch ( color )
	{
	case WPC_GREEN:
		break;

	case WPC_RED:
		break;

	case WPC_YELLOW:
		break;

	case WPC_BROWN:
		break;

	case WPC_NORMAL:
		break;

	case WPC_WHITE:
		break;
	};
}

void cConsole::setConsoleTitle( const QString& data )
{
}

void cConsole::setAttributes( bool bold, bool italic, bool underlined, unsigned char r, unsigned char g, unsigned char b, unsigned char size, enFontType font )
{
	if ( bold )
	{
	}

	if ( italic )
	{
	}

	if ( underlined )
	{
	}

	if ( size )
	{
	}

	switch ( font )
	{
	case FONT_SERIF:
		break;

	case FONT_NOSERIF:
		break;

	case FONT_FIXEDWIDTH:
		break;
	}
}

void cConsole::notifyServerState( enServerState newstate )
{
}

int main( int argc, char** argv )
{
	QApplication app( argc, argv );
	Server::instance()->start();
	return app.exec();
}
