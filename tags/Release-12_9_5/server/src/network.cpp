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

// Platform Includes
#include "platform.h"

//Wolfpack Includes
#include "network.h"
#include "srvparams.h"
#include "globals.h"
#include "console.h"
#include "inlines.h"
#include "network/asyncnetio.h"
#include "network/uosocket.h"
#include "network/listener.h"
#include "basechar.h"
#include "player.h"

// Library Includes
#include <qstringlist.h>

cNetwork *cNetwork::instance_;

cNetwork::cNetwork( void )
{
	loginSockets.setAutoDelete( true );
	uoSockets.setAutoDelete( true );	

	if( SrvParams->enableLogin() )
	{
		loginServer_ = new cListener( SrvParams->loginPort() );
		loginServer_->start();
	}
	else
		loginServer_ = 0;

	if( SrvParams->enableGame() )
	{
		gameServer_ = new cListener( SrvParams->gamePort() );
		gameServer_->start();
	}
	else
		gameServer_ = 0;

	netIo_ = new cAsyncNetIO;
	netIo_->start();
}

cNetwork::~cNetwork( void )
{
	if( loginServer_ )
	{
		loginServer_->cancel();
		loginServer_->wait();
		delete loginServer_;
	}

	if( gameServer_ )
	{
		gameServer_->cancel();
		gameServer_->wait();
		delete gameServer_;
	}

	netIo_->cancel();
	netIo_->wait();
	delete netIo_;
}

void cNetwork::poll( void )
{
	lock();

	// Check for new Connections (LoginServer)
	if( loginServer_ && loginServer_->haveNewConnection() )
	{
		QSocketDevice *socket = loginServer_->getNewConnection(); 
		netIo_->registerSocket( socket, true );
		cUOSocket *uosocket = new cUOSocket(socket);
		loginSockets.append( uosocket );

		// Notify the admin
		uosocket->log( QString( "Client connected to login server (%1).\n" ).arg( socket->peerAddress().toString() ) );
	}

	// Check for new Connections (GameServer)
	if( gameServer_ && gameServer_->haveNewConnection() )
	{
		QSocketDevice *socket = gameServer_->getNewConnection(); 
		netIo_->registerSocket( socket, false );
		cUOSocket *uosocket = new cUOSocket(socket);
		loginSockets.append( uosocket );

		// Notify the admin
		uosocket->log( QString( "Client connected to game server (%1).\n" ).arg( socket->peerAddress().toString() ) );
	}

	// fast return
	if( !uoSockets.isEmpty() || !loginSockets.isEmpty() )
	{
		// Check for new Packets
		cUOSocket* uoSocket = 0;
		for ( uoSocket = uoSockets.first(); uoSocket; uoSocket = uoSockets.next() )
		{
			// Check for disconnected sockets
			if ( uoSocket->socket()->error() != QSocketDevice::NoError || !uoSocket->socket()->isValid() || !uoSocket->socket()->isWritable() || uoSocket->socket()->isInactive() || !uoSocket->socket()->isOpen() )
			{
				uoSocket->log( "Client disconnected.\n" );
				uoSocket->disconnect();
				netIo_->unregisterSocket( uoSocket->socket() );			
				uoSockets.remove( uoSocket );
			}
			else
			{
				uoSocket->recieve();

				if( uiCurrentTime % 500 == 0 ) // Once every 0.5 Seconds
					uoSocket->poll();
			}
		}

		for ( uoSocket = loginSockets.first(); uoSocket; uoSocket = loginSockets.next())
		{
			if( uoSocket->socket()->error() != QSocketDevice::NoError || !uoSocket->socket()->isValid() || !uoSocket->socket()->isOpen() )
			{
				uoSocket->log( "Client disconnected.\n" );
				netIo_->unregisterSocket( uoSocket->socket() );
				loginSockets.remove();
				continue;
			}
			else
				uoSocket->recieve();

			if ( uoSocket->state() == cUOSocket::InGame )
			{
				uoSockets.append( loginSockets.take() );
			}
		}
	}

	unlock();
}

// Load IP Blocking rules
void cNetwork::load( void )
{
}

// Reload IP Blocking rules
void cNetwork::reload( void )
{
	unload();
	load();
}

// Unload IP Blocking rules
void cNetwork::unload( void )
{
//	hosts_deny.clear();
}

void cNetwork::broadcast(const QString &message, UINT16 color, UINT16 font) 
{
	for (cUOSocket *socket = uoSockets.first(); socket; socket = uoSockets.next()) 
	{
		socket->sysMessage(message, color, font);
	}
}
