//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//	Copyright 1997, 98 by Marcus Rating (Cironian)
//  Copyright 2001-2003 by holders identified in authors.txt
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

/////////////////////
// Name:	Trade.cpp
// Purpose: functions that are related to trade
// History:	cut from wolfpack.cpp by Duke, 26.10.00
// Remarks:	not necessarily ALL those functions
//

#include "Trade.h"
#include "inlines.h"
#include "srvparams.h"
#include "typedefs.h"
#include "basechar.h"
#include "player.h"
#include "network.h"
#include "network/uorxpackets.h"
#include "network/uotxpackets.h"
#include "network/uosocket.h"

// System Includes
#include <algorithm>

using namespace std;

#undef  DBGFILE
#define DBGFILE "Trade.cpp"

namespace Trade
{

struct MatchItemAndSerial : public std::binary_function<P_ITEM, SERIAL, bool>
{
	bool operator()(P_ITEM pi, SERIAL serial) const
	{
		return pi->serial() == serial;
	}
};


void Trade::buyAction( cUOSocket *socket, cUORxBuy *packet )
{
	P_PLAYER pChar = socket->player();
	P_CHAR pVendor = FindCharBySerial( packet->serial() );

	cUOTxClearBuy clearBuy;
	clearBuy.setSerial( pVendor->serial() );

	if( !pChar || !pVendor || pVendor->free || pChar->free )
	{
		socket->send( &clearBuy );
		return;
	}

	// Is the vendor in range
	if( !pVendor->inRange( pChar, 4 ) )
	{
		socket->sysMessage( tr( "You can't reach the vendor" ) );
		return;
	}

	// Check the LoS (later)

	P_ITEM pPack = pChar->getBackpack();
	if( !pPack )
	{
		socket->send( &clearBuy );
		return;
	}

	UINT32 itemCount = packet->itemCount();

	if( itemCount >= 256 )
	{
		socket->send( &clearBuy );
		return;
	}

	// Get our total gold at once
	UINT32 totalGold = pChar->CountBankGold() + pChar->CountGold();

	P_ITEM pStock = pVendor->GetItemOnLayer( 0x1A );
	cItem::ContainerContent sContent;

	if( pStock )
		sContent = pStock->content();

	P_ITEM pBought = pVendor->GetItemOnLayer( 0x1B );
	cItem::ContainerContent bContent;

	if( pBought )
		bContent = pBought->content();

	UINT32 totalValue = 0;
	UINT32 i;
	map< SERIAL, UINT16 > items;

	for( i = 0; i < itemCount; ++i )
	{
		P_ITEM pItem = FindItemBySerial( packet->iSerial( i ) );

		if( !pItem )
		{
			socket->sysMessage( tr( "Invalid item bought." ) );
			socket->send( &clearBuy );
			return;
		}

		UINT16 amount = packet->iAmount( i );
		UINT8 layer = packet->iLayer( i );

		// First check: is the item on the vendor in the specified layer
		if( layer == 0x1A )
		{
			if( find_if( sContent.begin(), sContent.end(), bind2nd(MatchItemAndSerial(), pItem->serial()) ) == sContent.end() )
			{
				socket->sysMessage( tr( "Invalid item bought." ) );
				socket->send( &clearBuy );
				return;
			}
		}
		else if( layer == 0x1B )
		{
			if( find_if( bContent.begin(), bContent.end(), bind2nd(MatchItemAndSerial(), pItem->serial() ) ) == bContent.end() )
			{
				socket->sysMessage( tr( "Invalid item bought." ) );
				socket->send( &clearBuy );
				return;
			}
		}
		else
		{
			socket->sysMessage( tr( "Invalid item bought." ) );
			socket->send( &clearBuy );
			return;
		}

		// Check amount
		if( pItem->restock() > pItem->amount() )
			pItem->setRestock( pItem->amount() );

		// Nothing of that item left
		if( pItem->restock() == 0 )
			continue;

		totalValue += amount * pItem->buyprice();

		items.insert( make_pair( pItem->serial(), amount ) );
	}

	if( totalValue > totalGold )
	{
		pVendor->talk( tr( "Sorry but you do not possess enough gold." ) );
		socket->send( &clearBuy );
		return;
	}

	// Sanity checks all passed here
	for( map< SERIAL,  UINT16 >::iterator iter = items.begin(); iter != items.end(); ++iter )
	{
		P_ITEM pItem = FindItemBySerial( iter->first );
		UINT16 amount = QMIN( iter->second, pItem->restock() );

		pItem->setRestock( pItem->restock() - amount ); // Reduce the items in stock
		P_ITEM pSold;

		if( pItem->isPileable() )
		{
			pSold = pItem->dupe();
			pSold->setAmount( iter->second );
			pPack->addItem( pSold );
			pSold->update();
		}
		else
		{
			for( UINT16 j = 0; j < amount; ++j )
			{
				pSold = pItem->dupe();
				pSold->setAmount( 1 );
				pPack->addItem( pSold );
				pSold->update();
			}
		}
				
		socket->sysMessage( tr( "You put the %1 into your pack" ).arg( pItem->getName() ) );
	}

	socket->send( &clearBuy );
	pVendor->talk( tr( "Thank you %1, this makes %2 gold" ).arg( pChar->name() ).arg( totalValue ) );

	if( pChar->takeGold( totalValue, true ) < totalValue )
		clConsole.send( QString( "Player 0x%1 payed less than he should have to vendor 0x%2" ).arg( pChar->serial(), 8, 16 ).arg( pVendor->serial(), 8, 16 ) );
}

// this is a q&d fix for 'sell price higher than buy price' bug (Duke, 30.3.2001)
static bool items_match(P_ITEM pi1, P_ITEM pi2)
{
	if (pi1 && pi2 && pi1->id()==pi2->id() &&
		pi1->type()==pi2->type() &&
		!(pi1->id()==0x14F0 && (pi1->morex()!=pi2->morex())) &&			// house deeds only differ by morex
		!(IsShield(pi1->id()) && pi1->name2() == pi2->name2()) &&	// magic shields only differ by name2
		!(IsMetalArmour(pi1->id()) && pi1->color() != pi2->color()) )	// color checking for armour
		return true;
	return false;
}

void Trade::sellAction( cUOSocket *socket, cUORxSell *packet )
{
	P_PLAYER pChar = socket->player();
	P_CHAR pVendor = FindCharBySerial( packet->serial() );

	cUOTxClearBuy clearBuy;
	clearBuy.setSerial( pVendor->serial() );

	if( !pChar || !pVendor || pVendor->free || pChar->free )
	{
		socket->send( &clearBuy );
		return;
	}

	// Is the vendor in range
	if( !pVendor->inRange( pChar, 4 ) )
	{
		socket->sysMessage( tr( "You can't reach the vendor" ) );
		return;
	}

	// Check the LoS (later)

	P_ITEM pPack = pChar->getBackpack();
	if( !pPack )
	{
		socket->send( &clearBuy );
		return;
	}

	UINT32 itemCount = packet->itemCount();

	if( itemCount >= 256 )
	{
		socket->send( &clearBuy );
		return;
	}

	// Get our total gold at once
	UINT32 totalGold = pChar->CountBankGold() + pChar->CountGold();

	P_ITEM pPurchase = pVendor->GetItemOnLayer( 0x1C );
	if( !pPurchase )
	{
		socket->sysMessage( tr( "Invalid item sold." ) );
		socket->send( &clearBuy );
		return;
	}
	cItem::ContainerContent sContent;
	cItem::ContainerContent::const_iterator it;

	UINT32 totalValue = 0;
	UINT32 i;
	map< SERIAL, UINT16 > items;

	for( i = 0; i < itemCount; ++i )
	{
		P_ITEM pItem = FindItemBySerial( packet->iSerial( i ) );

		if( !pItem )
		{
			socket->sysMessage( tr( "Invalid item sold." ) );
			socket->send( &clearBuy );
			return;
		}

		UINT16 amount = packet->iAmount( i );

		// First an equal item with higher amount must be in the vendors sell container!
		sContent = pPurchase->content();

		bool found = false;
		for( it = sContent.begin(); it != sContent.end(); ++it )
		{
			if( !(*it) )
				continue;
	
			if( (*it)->id() == pItem->id() && (*it)->color() == pItem->color() && (*it)->amount() >= pItem->amount() )
			{
				found = true;
				break;
			}
		}

		if( !found )
		{
			socket->sysMessage( tr( "Invalid item sold." ) );
			socket->send( &clearBuy );
			return;
		}

		// now look for the item in the player's pack
		sContent = pPack->content();
		if( find_if( sContent.begin(), sContent.end(), bind2nd(MatchItemAndSerial(), pItem->serial()) ) == sContent.end() )
		{
			socket->sysMessage( tr( "Invalid item sold." ) );
			socket->send( &clearBuy );
			return;
		}

		totalValue += amount * pItem->sellprice();

		items.insert( make_pair( pItem->serial(), amount ) );
	}

	// Sanity checks all passed here
	P_ITEM pBought = pVendor->atLayer( cBaseChar::BuyNoRestockContainer );
	if( pBought )
	{
		for( map< SERIAL,  UINT16 >::iterator iter = items.begin(); iter != items.end(); ++iter )
		{
			P_ITEM pItem = FindItemBySerial( iter->first );
			UINT16 amount = iter->second;

			if( pItem->isPileable() )
			{
				P_ITEM pSold = pItem->dupe();
				pSold->setAmount( iter->second );
				pBought->addItem( pSold );
				pSold->update();
				if( pItem->amount() <= iter->second )
					Items->DeleItem( pItem );
				else
					pItem->setAmount( pItem->amount() - iter->second );
				pItem->update();
			}
			else
			{
				pPack->removeItem( pItem );
				pBought->addItem( pItem );
				pItem->update();
			}
		}
	}

	socket->send( &clearBuy );
	pVendor->talk( tr( "Thank you %1, here are your %2 gold" ).arg( pChar->name() ).arg( totalValue ) );

	pChar->giveGold( totalValue, false );
}

P_ITEM Trade::startTrade( P_CHAR pPlayer, P_CHAR pChar )
{
/*	if( !pChar || !pChar->socket() || !pPlayer || !pPlayer->socket() )
		return NULL;

	// Create a trade-container for both players
	// 0x2AF8 on Layer 0x1F
	SERIAL box1,box2;
	
	// One for our player
	P_ITEM tCont = Items->createScriptItem( "2af8" );
	tCont->setLayer( 0x1f );

//	tCont->setContSerial( pPlayer->serial() );
	tCont->setOwner( pPlayer );
	tCont->tags.set( "tradepartner", cVariant( pChar->serial() ) );
	tCont->update( pPlayer->socket() );
	tCont->update( pChar->socket() );
	box1 = tCont->serial();

	// One for the tradepartner
	tCont = tCont->dupe();
	tCont->setLayer( 0x1f );
//	tCont->setContSerial( pChar->serial() );
	tCont->setOwner( pChar );
	tCont->tags.set( "tradepartner", cVariant( pPlayer->serial() ) );
	tCont->update( pPlayer->socket() );
	tCont->update( pChar->socket() );
	box2 = tCont->serial();

	// Now send the both secure trading packets
	cUOTxTrade trade;
	
	// To us
	trade.setPartner( pChar->serial() );
	trade.setBox1( box1 );
	trade.setBox2( box2 );
	trade.setName( pChar->name.latin1() );
	pPlayer->socket()->send( &trade );

	// To the other
	trade.setPartner( pPlayer->serial() );
	trade.setBox1( box2 );
	trade.setBox2( box1 );
	trade.setName( pPlayer->name.latin1() );
	pChar->socket()->send( &trade );

	return FindItemBySerial( box1 );*/
	return 0;
}

P_ITEM Trade::tradestart(UOXSOCKET s, P_CHAR pc_i)
{
//	P_CHAR pc_currchar = currchar[s];
//	unsigned char msg[90];
//
//	P_ITEM pi_bps = pc_currchar->getBackpack();
//	P_ITEM pi_bpi = pc_i->getBackpack();
//	UOXSOCKET s2 = calcSocketFromChar(pc_i);
//
//	if (pi_bps == NULL) //LB
//	{
//		sysmessage(s, "Time to buy a backpack!");
//		sysmessage(s2, "%s doesnt have a backpack!", pc_currchar->name.latin1());
//		return 0;
//	}
//	if (pi_bpi == NULL)
//	{
//		sysmessage(s2, "Time to buy a backpack!");
//		sysmessage(s, "%s doesnt have a backpack!", pc_i->name.latin1());
//		return 0;
//	}
//
//	P_ITEM pi_ps = Items->SpawnItem(s2, pc_currchar, 1, "#", 0, 0x1E, 0x5E, 0, 0, 0);
//	if(pi_ps == NULL)
//		return 0;
//	pi_ps->pos = Coord_cl(26, 0, 0);
////	pi_ps->setContSerial(pc_currchar->serial());
//	pi_ps->setLayer( 0 );
//	pi_ps->setType( 1 );
//	pi_ps->setDye(0);
////	sendbpitem(s, pi_ps);
////	if (s2 != INVALID_UOXSOCKET)
////		sendbpitem(s2, pi_ps);
//
//	P_ITEM pi_pi = Items->SpawnItem(s2,pc_i,1,"#",0,0x1E,0x5E,0,0,0);
//	if (pi_pi == NULL)
//		return 0;
//	pi_pi->pos = Coord_cl(26, 0, 0);
////	pi_pi->setContSerial(pc_i->serial());
//	pi_pi->setLayer( 0 );
//	pi_pi->setType( 1 );
//	pi_pi->setDye(0);
////	sendbpitem(s, pi_pi);
////	if (s2 != INVALID_UOXSOCKET)
////		sendbpitem(s2, pi_pi);
//
//	pi_pi->setMoreb1( static_cast<unsigned char>((pi_ps->serial()&0xFF000000)>>24) );
//	pi_pi->setMoreb2( static_cast<unsigned char>((pi_ps->serial()&0x00FF0000)>>16) );
//	pi_pi->setMoreb3( static_cast<unsigned char>((pi_ps->serial()&0x0000FF00)>>8) );
//	pi_pi->setMoreb4( static_cast<unsigned char>((pi_ps->serial()&0x000000FF)) );
//	pi_ps->setMore1( static_cast<unsigned char>((pi_pi->serial()&0xFF000000)>>24) );
//	pi_ps->setMore2( static_cast<unsigned char>((pi_pi->serial()&0x00FF0000)>>16) );
//	pi_ps->setMore3( static_cast<unsigned char>((pi_pi->serial()&0x0000FF00)>>8) );
//	pi_ps->setMore4( static_cast<unsigned char>((pi_pi->serial()&0x000000FF)) );
//	pi_ps->setMoreZ(0);
//	pi_pi->setMoreZ(0);
//
//	msg[0] = 0x6F; // Header Byte
//	msg[1] = 0; // Size
//	msg[2] = 47; // Size
//	msg[3] = 0; // Initiate
//	LongToCharPtr(pc_i->serial(),msg+4);
//	LongToCharPtr(pi_ps->serial(),msg+8);
//	LongToCharPtr(pi_pi->serial(),msg+12);
//	msg[16]=1;
//	strcpy((char*)&(msg[17]), pc_i->name.latin1());
//	Xsend(s, msg, 47);
//
//	if (s2 != INVALID_UOXSOCKET)
//	{
//		msg[0]=0x6F; // Header Byte
//		msg[1]=0;    // Size
//		msg[2]=47;   // Size
//		msg[3]=0;    // Initiate
//		LongToCharPtr(pc_currchar->serial(),msg+4);
//		LongToCharPtr(pi_pi->serial(),msg+8);
//		LongToCharPtr(pi_ps->serial(),msg+12);
//		msg[16]=1;
//		strcpy((char*)&(msg[17]), pc_currchar->name.latin1());
//
//		Xsend(s2, msg, 47);
//	}
	return 0;
}

void Trade::clearalltrades()
{
	qWarning("cTrade::clearalltrades() disabled");
/*	AllItemsIterator iterItems;
	for (iterItems.Begin(); !iterItems.atEnd(); iterItems++)
	{
		P_ITEM pi = iterItems.GetData();
		if( pi->type() == 1 && pi->pos.x == 26 && pi->pos.y == 0 && pi->pos.z == 0 && pi->id() == 0x1E5E )
		{
			P_CHAR pc = FindCharBySerial(pi->contserial);
			P_ITEM pBackpack = Packitem(pc);
			SERIAL serial = pi->serial();
			unsigned int ci;
			vector<SERIAL> vecContainer = contsp.getData(serial);
			for (ci = 0; ci < vecContainer.size(); ci++)
			{
				P_ITEM pj = FindItemBySerial(vecContainer[ci]);
				if (pj != NULL)
					if ((pj->contserial==serial))
					{
						if(pBackpack != NULL)
						{
							pBackpack->addItem(pj);
						}
					}
			}
			//iterItems++; // Iterator will became invalid when deletting.
			Items->DeleItem(pi);
			clConsole.send("Trade cleared\n");
		}
	}
*/
}

void Trade::trademsg(int s)
{
/*	P_ITEM cont1, cont2;
	cont1 = cont2 = NULL ;
	switch(buffer[s][3])
	{
	case 0://Start trade - Never happens, sent out by the server only.
		break;
	case 2://Change check marks. Possibly conclude trade
		cont1 = FindItemBySerPtr(&buffer[s][4]);
		if (cont1 != NULL)
			cont2 = FindItemBySerial(calcserial(cont1->moreb1(), cont1->moreb2(), cont1->moreb3(), cont1->moreb4()));
		else
			cont2 = NULL;
		if (cont2 != NULL) // lb crashfix
		{
			cont1->setMoreZ(buffer[s][11]);
			sendtradestatus(cont1, cont2);
			if (cont1->morez() && cont2->morez())
			{
				dotrade(cont1, cont2);
				endtrade(calcserial(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]));
			}
		}
		break;
	case 1://Cancel trade. Send each person cancel messages, move items.
		endtrade(calcserial(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]));
		break;
	default:
		clConsole.send("ERROR: Fallout of switch statement without default. wolfpack.cpp, trademsg()\n"); //Morrolan
	}
*/
}

void Trade::dotrade(P_ITEM cont1, P_ITEM cont2)
{
	qWarning("cTrade::dotrade() is disabled");
/*	int serial;

	P_CHAR p1 = FindCharBySerial(cont1->contserial);
	if(p1 == NULL) return;
	P_CHAR p2 = FindCharBySerial(cont2->contserial);
	if(p2 == NULL) return;
	if(cont1->morez==0 || cont2->morez==0)
	{//not checked - give items to previous owners - AntiChrist
		P_CHAR t;
		t  = p1;
		p1 = p2;
		p2 = t;
	}
	P_ITEM bp1 = Packitem(p1);
	if(bp1 == NULL) return;
	P_ITEM bp2 = Packitem(p2);
	if(bp2 == NULL) return;
	UOXSOCKET s1 = calcSocketFromChar(p1);
	if (s1 ==-1)
		cout << "Error getting socket in trade, calcSocketFromChar for s1" << endl;
	UOXSOCKET s2 = calcSocketFromChar(p2);
    if (s2 ==-1)
		cout << "Error getting socket in trade, calcSocketFromChar for si" << endl;
	serial = cont1->serial();
	unsigned int ci;
	vector<SERIAL> vecContainer = contsp.getData(serial);
	for (ci = 0; ci < vecContainer.size(); ++ci)
	{
		P_ITEM pi = FindItemBySerial(vecContainer[ci]);
		if (pi != NULL)
			if ((pi->contserial==serial))
			{
				bp2->addItem(pi);
				if (s1!=-1)
					pi->update();//AntiChrist
				if (s2!=-1) sendbpitem(s2, pi);
					pi->update();//AntiChrist
			}
	}
	serial = cont2->serial();
	vecContainer.clear();
	vecContainer = contsp.getData(serial);
	for (ci = 0; ci < vecContainer.size(); ++ci)
	{
		P_ITEM pi = FindItemBySerial(vecContainer[ci]);
		if (pi != NULL)
			if ((pi->contserial==serial))
			{
				bp1->addItem(pi);
				if (s2 != INVALID_UOXSOCKET)
					pi->update();//AntiChrist
				if (s1 != INVALID_UOXSOCKET) sendbpitem(s1, pi);
					pi->update();//AntiChrist
			}
	}
*/
}

}