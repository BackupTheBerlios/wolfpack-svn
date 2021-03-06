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

#ifndef __UOSOCKET__
#define __UOSOCKET__

// Library Includes
#include <qcstring.h>
#include <qsocketdevice.h>
#include <qobject.h>
#include <qmap.h>
#include <qbitarray.h>
#include <vector>

// Forward Declarations
class cUOPacket;
class cAccount;
class cTargetRequest;
class cGump;
class cUObject;
class cCustomTags;
class cContextMenu;

// Too many Forward Declarations
#include "uorxpackets.h"
#include "../log.h"
#include "../typedefs.h"
#include "../python/engine.h"
#include "../targetrequest.h"
#include "../customtags.h"
#include "../objectdef.h"

struct stTargetItem
{
	Q_UINT16 id;
	Q_INT16 xOffset;
	Q_INT16 yOffset;
	Q_INT16 zOffset;
	Q_UINT16 hue;
};

class cUOSocket
{
	OBJECTDEF( cUOSocket )

public:
	enum eSocketState
	{
		Connecting		= 0,
		LoggingIn,
		LoggedIn,
		InGame
	};

public:
	/*!
		\brief This static function registers a python function to handle
		a specific type of packet.
		\param packet The id of the packet to handle.
		\param handler The python function for handling the packet.
	*/
	static void registerPacketHandler( unsigned char packet, PyObject* handler );

	/*!
		\brief This function clears all installed packet handlers.
	*/
	static void clearPacketHandlers();

	/*!
		\brief Let this socket use a given item.
	*/
	bool useItem( P_ITEM item );

	inline unsigned short screenWidth() const
	{
		return _screenWidth;
	}

	inline unsigned short screenHeight() const
	{
		return _screenHeight;
	}

	cCustomTags tags() const
	{
		return tags_;
	}
	cCustomTags& tags()
	{
		return tags_;
	}

	QBitArray* toolTips() const
	{
		return tooltipscache_;
	}
	bool haveTooltip( Q_UINT32 data ) const
	{
		return tooltipscache_->testBit( data );
	}
	void delTooltip( Q_UINT32 data )
	{
		tooltipscache_->setBit( data, false );
	}
	void addTooltip( Q_UINT32 );

	cUOSocket( QSocketDevice* sDevice );
	virtual ~cUOSocket( void );

	Q_UINT8 walkSequence( void ) const;
	void setWalkSequence( Q_UINT8 data );


	QSocketDevice* socket( void ) const;
	void setSocket( QSocketDevice* data );

	eSocketState state( void ) const;
	void setState( eSocketState data );

	QString version( void ) const;
	QString lang( void ) const;
	QString ip( void ) const;

	P_PLAYER player( void ) const;
	P_ITEM dragging() const;

	cAccount* account( void ) const
	{
		return _account;
	}

	void setAccount( cAccount* data )
	{
		_account = data;
	}

	unsigned int lastActivity() const;
	unsigned int rxBytes() const;
	unsigned int txBytes() const;
	unsigned char viewRange() const;
	void setRxBytes( unsigned int data );
	void setTxBytes( unsigned int data );

	Q_UINT32 uniqueId( void ) const;

	void recieve(); // Tries to recieve one packet and process it
	void send( cUOPacket* packet ) const;
	void send( cGump* gump );

	// Handler
	void handleSecureTrading( cUORxSecureTrading* packet );
	void handleLoginRequest( cUORxLoginRequest* packet );
	void handleHardwareInfo( cUORxHardwareInfo* packet );
	void handleSelectShard( cUORxSelectShard* packet );
	void handleServerAttach( cUORxServerAttach* packet );
	void handleDeleteCharacter( cUORxDeleteCharacter* packet );
	void handlePlayCharacter( cUORxPlayCharacter* packet );
	void handleCreateChar( cUORxCreateChar* packet );
	void handleUpdateRange( cUORxUpdateRange* packet );
	void handleQuery( cUORxQuery* packet );
	void handleRequestLook( cUORxRequestLook* packet );
	void handleMultiPurpose( cUORxMultiPurpose* packet );
	void handleCastSpell( cUORxCastSpell* packet );
	void handleContextMenuRequest( cUORxContextMenuRequest* packet );
	void handleContextMenuSelection( cUORxContextMenuSelection* packet );
	void handleToolTip( cUORxRequestToolTip* packet );
	void handleCustomHouseRequest( cUORxCustomHouseRequest* packet );
	void handleWalkRequest( cUORxWalkRequest* packet );
	void handleSetLanguage( cUORxSetLanguage* packet );
	void handleSpeechRequest( cUORxSpeechRequest* packet );
	void handleDoubleClick( cUORxDoubleClick* packet );
	void handleGetTip( cUORxGetTip* packet );
	void handleChangeWarmode( cUORxChangeWarmode* packet );
	void handleResurrectionMenu( cUORxResurrectionMenu* packet );
	void handleTarget( cUORxTarget* packet );
	void handleRequestAttack( cUORxRequestAttack* packet );
	void handleExtendedStats( cUORxExtendedStats* packet );
	void handleAction( cUORxAction* packet );
	void handleAllNames( cUORxAllNames* packet );
	void handleGumpResponse( cUORxGumpResponse* packet );
	void handleHelpRequest( cUORxHelpRequest* packet );
	void handleSkillLock( cUORxSkillLock* packet );
	void handleBuy( cUORxBuy* packet );
	void handleSell( cUORxSell* packet );
	void handleProfile( cUORxProfile* packet );
	void handleRename( cUORxRename* packet );
	void handleAosMultiPurpose( cUORxAosMultiPurpose* packet );
	void handleCHBackup( cUORxAosMultiPurpose* packet );
	void handleCHRestore( cUORxAosMultiPurpose* packet );
	void handleCHCommit( cUORxAosMultiPurpose* packet );
	void handleCHDelete( cUORxCHDelete* packet );
	void handleCHAddElement( cUORxCHAddElement* packet );
	void handleCHClose( cUORxAosMultiPurpose* packet );
	void handleCHStairs( cUORxCHStairs* packet );
	void handleCHSync( cUORxAosMultiPurpose* packet );
	void handleCHClear( cUORxAosMultiPurpose* packet );
	void handleCHLevel( cUORxCHLevel* packet );
	void handleCHRevert( cUORxAosMultiPurpose* packet );
	void handleParty( cUOPacket* packet );
	void handleChat( cUOPacket* packet );

	// Utilities
	void updateChar( P_CHAR pChar );
	void sendChar( P_CHAR pChar );
	void showSpeech( const cUObject* object, const QString& message, Q_UINT16 color = 0x3B2, Q_UINT16 font = 3, Q_UINT8 speechType = 0x00 ) const;
	void sysMessage( const QString& message, Q_UINT16 color = 0x3b2, Q_UINT16 font = 3 ) const;
	void sendCharList();
	void removeObject( cUObject* object );
	void disconnect(); // Call this whenever the socket should disconnect
	void sendPaperdoll( P_CHAR pChar );
	void playMusic( void );
	void sendContainer( P_ITEM pCont );
	void bounceItem( P_ITEM pItem, Q_UINT8 reason );
	void updatePlayer();
	void resendPlayer( bool quick = true );
	void poll();
	void soundEffect( Q_UINT16 soundId, cUObject* source = NULL );
	void attachTarget( cTargetRequest* request );
	void attachTarget( cTargetRequest* request, Q_UINT16 multiid, unsigned short xoffset, unsigned short yoffset, unsigned short zoffset );
	void attachTarget( cTargetRequest* request, std::vector<stTargetItem>& items, Q_INT16 xOffset = 0, Q_INT16 yOffset = 0, Q_INT16 zOffset = 0 );
	void cancelTarget();
	void resendWorld( bool clean = true );
	void resync();
	void updateStamina( P_CHAR pChar = NULL );
	void updateMana( P_CHAR pChar = NULL );
	void updateHealth( P_CHAR pChar = NULL );
	void sendStatWindow( P_CHAR pChar = NULL );
	bool inRange( cUOSocket* ) const;
	void sendSkill( Q_UINT16 skill );
	void sendBuyWindow( P_NPC pVendor );
	void sendSellWindow( P_NPC pVendor, P_CHAR pSeller );
	void sendVendorCont( P_ITEM pItem );
	void clilocMessage( const Q_UINT32 MsgID, const QString& params = 0, const Q_UINT16 color = 0x3b2, const Q_UINT16 font = 3, cUObject* object = 0, bool system = false );
	void clilocMessageAffix( const Q_UINT32 MsgID, const QString& params = 0, const QString& affix = 0, const Q_UINT16 color = 0x3b2, const Q_UINT16 font = 3, cUObject* object = 0, bool dontMove = false, bool prepend = false, bool system = false );
	void updateLightLevel();
	void sendQuestArrow( bool show, Q_UINT16 x, Q_UINT16 y );
	void closeGump( Q_UINT32 type, Q_UINT32 returnCode );
	void log( eLogLevel loglevel, const QString& message );
	void log( const QString& message );

	bool canSee( cUOSocket* socket );
	bool canSee( P_ITEM item );
	bool canSee( P_CHAR character );
	bool canSee( cUObject* object );

	void allowMove( Q_UINT8 sequence );
	void denyMove( Q_UINT8 sequence );

private:
	void updateCharList();
	void setPlayer( P_PLAYER player );
	void playChar( P_PLAYER player );

private:
	QValueVector<cUORxWalkRequest> packetQueue;
	unsigned int _uniqueId;
	unsigned int _lastActivity;
	Q_UINT8 _walkSequence;
	Q_UINT8 lastPacket;
	eSocketState _state;
	QString _lang;
	cTargetRequest* targetRequest;
	cAccount* _account;
	P_PLAYER _player;
	unsigned int _rxBytes;
	unsigned int _txBytes;
	QSocketDevice* _socket;
	unsigned short _screenWidth;
	unsigned short _screenHeight;
	Q_UINT8 _viewRange;
	QString _version;
	cCustomTags tags_;
	QString _ip; // IP used to connect
	QBitArray* tooltipscache_;
	QPtrList<cContextMenu> contextMenu_;
	QMap<SERIAL, cGump*> gumps;

	bool authenticate( const QString& username, const QString& password );

	/*!
		\brief This array contains all registered packet handlers known
		to all sockets.
	*/
	static PyObject* handlers[255];
};

// Inline members
inline Q_UINT8 cUOSocket::walkSequence( void ) const
{
	return _walkSequence;
}

inline void cUOSocket::setWalkSequence( Q_UINT8 data )
{
	_walkSequence = data;
}

inline QSocketDevice* cUOSocket::socket( void ) const
{
	return _socket;
}

inline void cUOSocket::setSocket( QSocketDevice* data )
{
	_socket = data;
}

inline cUOSocket::eSocketState cUOSocket::state( void ) const
{
	return _state;
}

inline void cUOSocket::setState( cUOSocket::eSocketState data )
{
	_state = data;
}

inline QString cUOSocket::version( void ) const
{
	return _version;
}

inline QString cUOSocket::lang( void ) const
{
	return _lang;
}

inline QString cUOSocket::ip( void ) const
{
	return _ip;
}

inline P_PLAYER cUOSocket::player( void ) const
{
	return _player;
}

inline unsigned int cUOSocket::lastActivity() const
{
	return _lastActivity;
}

inline unsigned int cUOSocket::rxBytes( void ) const
{
	return _rxBytes;
}

inline Q_UINT32 cUOSocket::txBytes( void ) const
{
	return _txBytes;
}

inline Q_UINT8 cUOSocket::viewRange( void ) const
{
	return _viewRange;
}

inline void cUOSocket::setRxBytes( Q_UINT32 data )
{
	_rxBytes = data;
}

inline void cUOSocket::setTxBytes( Q_UINT32 data )
{
	_txBytes = data;
}

inline Q_UINT32 cUOSocket::uniqueId( void ) const
{
	return _uniqueId;
}


#endif
