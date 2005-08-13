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

#if !defined(__ASYNCNETIO_H__)
#define __ASYNCNETIO_H__

#include <qthread.h>
#include <qmutex.h>

#include <qmap.h>

class Q3SocketDevice;
class cAsyncNetIOPrivate;
class cUOPacket;

class cAsyncNetIO : public QThread
{
	QMap<Q3SocketDevice*, cAsyncNetIOPrivate*> buffers;

	typedef QMap<Q3SocketDevice*, cAsyncNetIOPrivate*>::iterator iterator;
	typedef QMap<Q3SocketDevice*, cAsyncNetIOPrivate*>::const_iterator const_iterator;

	QMutex mapsMutex;
	QWaitCondition waitCondition;

	bool volatile canceled_;

public:
	cAsyncNetIO() : canceled_( false )
	{
	}
	~cAsyncNetIO() throw();

	bool registerSocket( Q3SocketDevice*, bool loginSocket );
	bool unregisterSocket( Q3SocketDevice* );
	Q_ULONG bytesAvailable( Q3SocketDevice* ) const;

	cUOPacket* recvPacket( Q3SocketDevice* );
	void pushfrontPacket( Q3SocketDevice*, cUOPacket* packet );
	void sendPacket( Q3SocketDevice*, cUOPacket*, bool );

	void flush( Q3SocketDevice* );
	bool canceled() const
	{
		return canceled_;
	}
	void cancel()
	{
		canceled_ = true; waitCondition.wakeAll();
	}

protected:
	virtual void run() throw();

private:
	void buildUOPackets( cAsyncNetIOPrivate* );
	void flushWriteBuffer( cAsyncNetIOPrivate* );
};

#endif //__ASYNCNETIO_H__

