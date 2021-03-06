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

// Platform include
#include "platform.h"

// Wolfpack includes
#include "chars.h"
#include "basechar.h"
#include "items.h"
#include "party.h"
#include "guilds.h"
#include "player.h"
#include "npc.h"
#include "globals.h"
#include "multis.h"
#include "world.h"
#include "sectors.h"
#include "wpdefmanager.h"
#include "console.h"
#include "log.h"
#include "combat.h"

// Qt Includes
#include <qstringlist.h>

// Libary Includes
#include <math.h>

void cCharStuff::DeleteChar( P_CHAR pc_k ) // Delete character
{
	if( !pc_k )
		return;

	// Cancel any ongoing fight.
	QPtrList<cFightInfo> fights = pc_k->fights();
	for (cFightInfo *info = fights.first(); info; info = fights.next()) {
		delete info;
	}

	// Call the onDelete event.
	PyObject *args = Py_BuildValue("(O&)", PyGetCharObject, pc_k);
	cPythonScript::callChainedEventHandler(EVENT_DELETE, pc_k->getEvents(), args);
	Py_DECREF(args);

	pc_k->free = false;

	P_NPC pn_k = dynamic_cast<P_NPC>(pc_k);

	if(pn_k) {
		// If a player is mounted on us, unmount him.
		if (pn_k->stablemasterSerial() != INVALID_SERIAL) {
			P_PLAYER player = dynamic_cast<P_PLAYER>(World::instance()->findChar(pn_k->stablemasterSerial()));

			if (player) {
				P_ITEM mount = player->atLayer(cBaseChar::Mount);

				if (mount && mount->getTag("pet").toInt() == pn_k->serial()) {
					mount->remove();
				}
			}
		}

		pn_k->setOwner(0);
	}

	pc_k->setGuarding( 0 );
	
	// We need to remove the equipment here.
	cBaseChar::ItemContainer container(pc_k->content());
	cBaseChar::ItemContainer::const_iterator it (container.begin());
	cBaseChar::ItemContainer::const_iterator end(container.end());
	for (; it != end; ++it )
	{
		P_ITEM pItem = *it;
		if( !pItem )
			continue;

		pItem->remove();
	}

	// multi check
	if( pc_k->multis() != INVALID_SERIAL )
	{
		cMulti* pMulti = dynamic_cast< cMulti* >( FindItemBySerial( pc_k->multis() ) );
		if( pMulti )
		{
			pMulti->removeChar( pc_k );
		}
	}

	pc_k->removeFromView( false ); // Remove the character from all in-range sockets view
	MapObjects::instance()->remove( pc_k ); // taking it out of mapregions BEFORE x,y changed

	P_PLAYER pp_k = dynamic_cast<P_PLAYER>(pc_k);
	if (pp_k) {
		if (pp_k->account()) {
			pp_k->account()->removeCharacter(pp_k);
		}

		if (pp_k->party()) {
			pp_k->party()->removeMember(pp_k);
		} else {
			TempEffects::instance()->dispel(pp_k, 0, "cancelpartyinvitation", false, false);
		}

		if (pp_k->guild()) {
			pp_k->guild()->removeMember(pp_k);
		}
	}
	
	World::instance()->deleteObject( pc_k );
}

P_NPC cCharStuff::createScriptNpc( const QString &section, const Coord_cl &pos )
{
	if( section.isNull() || section.isEmpty() )
		return NULL;

	const cElement* DefSection = DefManager->getDefinition( WPDT_NPC, section );

	if( !DefSection )
	{
		Console::instance()->log( LOG_ERROR, QString( "Unable to create unscripted npc: %1\n" ).arg( section ) );
		return NULL;
	}

	P_NPC pChar = new cNPC;
	pChar->Init();

	pChar->setMinDamage(1);
	pChar->setMaxDamage(1);

	pChar->moveTo( pos );

	pChar->setRegion( AllTerritories::instance()->region( pChar->pos().x, pChar->pos().y, pChar->pos().map ) );

	pChar->applyDefinition( DefSection );

	// OrgBody and OrgSkin are often not set in the scripts
	pChar->setOrgBody(pChar->body());
	pChar->setOrgSkin(pChar->skin());

	// Now we call onCreate
	pChar->onCreate( section );

	pChar->resend( false );

	return pChar;
}

