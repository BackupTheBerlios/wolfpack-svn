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

// library includes
#include <qvaluelist.h>
#include <math.h>

// wolfpack includes
#include "npc.h"
#include "network/uotxpackets.h"
#include "network/uosocket.h"
#include "network.h"
#include "player.h"
#include "globals.h"
#include "world.h"
#include "persistentbroker.h"
#include "dbdriver.h"
#include "sectors.h"
#include "srvparams.h"
#include "log.h"
#include "console.h"
#include "spawnregions.h"
#include "corpse.h"
#include "wpdefmanager.h"
#include "combat.h"
#include "walking.h"
#include "skills.h"
#include "ai/ai.h"
#include "inlines.h"
#include "basics.h"

cNPC::cNPC()
{
	minDamage_			= 0;
	maxDamage_			= 0;
	nextMsgTime_		= 0;
	nextGuardCallTime_	= 0;
	nextBeggingTime_	= 0;
	nextMoveTime_		= 0;
	tamingMinSkill_		= 0;
	summonTime_			= 0;
	additionalFlags_	= 0;
	owner_				= NULL;
	stablemasterSerial_	= INVALID_SERIAL;
	wanderType_			= stWanderType();
	aiid_				= "Monster_Aggressive_L1";
	ai_					= new Monster_Aggressive_L1( this );
	aiCheckInterval_	= (UINT16)floor(SrvParams->checkAITime() * MY_CLOCKS_PER_SEC);
	aiCheckTime_		= uiCurrentTime + aiCheckInterval_;
	criticalHealth_		= 10; // 10% !
	spellsLow_			= 0;
	spellsHigh_			= 0;
	controlSlots_		= 1;
}

cNPC::cNPC(const cNPC& right)
{
}

cNPC::~cNPC()
{
	delete ai_;
}

cNPC& cNPC::operator=(const cNPC& right)
{
	return *this;
}

static cUObject* productCreator()
{
	return new cNPC;
}

void cNPC::registerInFactory()
{
	QStringList fields, tables, conditions;
	buildSqlString( fields, tables, conditions ); // Build our SQL string
	QString sqlString = QString( "SELECT %1 FROM uobjectmap,%2 WHERE uobjectmap.type = 'cNPC' AND %3" ).arg( fields.join( "," ) ).arg( tables.join( "," ) ).arg( conditions.join( " AND " ) );
	UObjectFactory::instance()->registerType( "cNPC", productCreator );
	UObjectFactory::instance()->registerSqlQuery( "cNPC", sqlString );
}

void cNPC::buildSqlString( QStringList &fields, QStringList &tables, QStringList &conditions )
{
	cBaseChar::buildSqlString( fields, tables, conditions );
	fields.push_back( "npcs.mindamage,npcs.maxdamage,npcs.tamingminskill" );
	fields.push_back( "npcs.summontime,npcs.additionalflags,npcs.owner" );
	fields.push_back( "npcs.carve,npcs.spawnregion,npcs.stablemaster" );
	fields.push_back( "npcs.lootlist,npcs.ai,npcs.wandertype" );
	fields.push_back( "npcs.wanderx1,npcs.wanderx2,npcs.wandery1,npcs.wandery2" );
	fields.push_back( "npcs.wanderradius,npcs.fleeat,npcs.spellslow,npcs.spellshigh,npcs.controlslots" );
	tables.push_back( "npcs" );
	conditions.push_back( "uobjectmap.serial = npcs.serial" );
}

static void npcRegisterAfterLoading( P_NPC pc );

void cNPC::load( char **result, UINT16 &offset )
{
	cBaseChar::load( result, offset );
	SERIAL ser;

	minDamage_ = atoi( result[offset++] );
	maxDamage_ = atoi( result[offset++] );
	tamingMinSkill_ = atoi( result[offset++] );
	summonTime_ = atoi( result[offset++] ) + uiCurrentTime;
	if( summonTime_ )
		summonTime_ += uiCurrentTime;
	additionalFlags_ = atoi( result[offset++] );
	ser = atoi( result[offset++] );
	owner_ = dynamic_cast<P_PLAYER>(FindCharBySerial( ser ));
	carve_ = result[offset++];
	spawnregion_ = result[offset++];
	stablemasterSerial_ = atoi( result[offset++] );
	lootList_ = result[offset++];
	setAI( result[offset++] );
	setWanderType( (enWanderTypes)atoi( result[offset++] ) );
	setWanderX1( atoi( result[offset++] ) );
	setWanderX2( atoi( result[offset++] ) );
	setWanderY1( atoi( result[offset++] ) );
	setWanderY2( atoi( result[offset++] ) );
	setWanderRadius( atoi( result[offset++] ) );
	setCriticalHealth( atoi( result[offset++] ) );
	spellsLow_ = atoi( result[offset++] );
	spellsHigh_ = atoi( result[offset++] );
	controlSlots_ = atoi( result[offset++] );

	npcRegisterAfterLoading( this );
	changed_ = false;

	if (!spawnregion_.isEmpty()) {
		cSpawnRegion *region = SpawnRegions::instance()->region(spawnregion_);
		if (region) {
			region->add(serial_);
		} else {
			spawnregion_ = QString::null;
		}
	}
}

void cNPC::save()
{
	if ( changed_ )
	{
		initSave;
		setTable( "npcs" );

		addField( "serial", serial() );
		addField( "mindamage", minDamage_);
		addField( "maxdamage", maxDamage_);
		addField( "tamingminskill", tamingMinSkill_);
		addField( "summontime", summonTime_ ? summonTime_ - uiCurrentTime : 0 );
		addField( "additionalflags", additionalFlags_ );
		addField( "owner", owner_ ? owner_->serial() : INVALID_SERIAL );
		addStrField( "carve", carve_);
		addStrField( "spawnregion", spawnregion_);
		addField( "stablemaster", stablemasterSerial_ );
		addStrField( "lootlist", lootList_);
		addStrField( "ai", aiid_ );
		addField( "wandertype", (UINT8)wanderType() );
		addField( "wanderx1", wanderX1() );
		addField( "wanderx2", wanderX2() );
		addField( "wandery1", wanderY1() );
		addField( "wandery2", wanderY2() );
		addField( "wanderradius", wanderRadius() );
		addField( "fleeat", criticalHealth() );
		addField( "spellslow", spellsLow_ );
		addField( "spellshigh", spellsHigh_ );
		addField( "controlslots", controlSlots_ );

		addCondition( "serial", serial() );
		saveFields;
	}
	cBaseChar::save();
}

bool cNPC::del()
{
	if( !isPersistent )
		return false; // We didn't need to delete the object

	persistentBroker->addToDeleteQueue( "npcs", QString( "serial = '%1'" ).arg( serial() ) );
	changed_ = true;
	return cBaseChar::del();
}

static void npcRegisterAfterLoading( P_NPC pc )
{
	if (pc->stablemasterSerial() == INVALID_SERIAL) {
		MapObjects::instance()->add(pc);
	}
}

void cNPC::setOwner(P_PLAYER data, bool nochecks)
{
	// We CANT be our own owner
	if( data && ( data->serial() == this->serial() ) )
		return;

	if( !nochecks && owner_ )
	{
		owner_->removePet(this, true);
	}

	owner_ = data;
	changed( TOOLTIP );
	changed_ = true;

	if( !nochecks && owner_ )
	{
		owner_->addPet(this, true);
	}
}

void cNPC::setNextMoveTime()
{
	unsigned int interval;

	if (isTamed()) {
		interval = SrvParams->tamedNpcMoveTime() * MY_CLOCKS_PER_SEC;
	} else {
		interval = SrvParams->npcMoveTime() * MY_CLOCKS_PER_SEC;
	}

	// Wander slowly if wandering freely.
	if (wanderType() == enFreely || wanderType() == enCircle || wanderType() == enRectangle) {
		interval *= 3;
	}

	setNextMoveTime(uiCurrentTime + interval);
}

// Update flags etc.
void cNPC::update( bool excludeself )
{
	cUOTxUpdatePlayer update;
	update.fromChar(this);

	for (cUOSocket *socket = Network::instance()->first(); socket; socket = Network::instance()->next()) {
		if (socket->canSee(this)) {
			update.setHighlight(notoriety(socket->player()));
			socket->send(&update);
		}
	}
}

// Resend the char to all sockets in range
void cNPC::resend( bool clean)
{
	// We are stabled and therefore we arent visible to others
	if( stablemasterSerial() != INVALID_SERIAL )
		return;

	cUOTxRemoveObject remove;
	remove.setSerial(serial_);

	for (cUOSocket *socket = Network::instance()->first(); socket; socket = Network::instance()->next()) {
		if (socket->canSee(this)) {
			cUOTxDrawChar drawChar;
			drawChar.fromChar(this);
			drawChar.setHighlight(notoriety(socket->player()));
			socket->send(&drawChar);

			sendTooltip(socket);

			for (ItemContainer::const_iterator it = content_.begin(); it != content_.end(); ++it) {
				it.data()->sendTooltip(socket);
			}
		} else if (clean) {
			socket->send(&remove);
		}
	}
}

void cNPC::talk( const QString &message, UI16 color, UINT8 type, bool autospam, cUOSocket* socket )
{
	if( autospam )
	{
		if( nextMsgTime_ < uiCurrentTime )
			nextMsgTime_ = uiCurrentTime + MY_CLOCKS_PER_SEC*10;
		else
			return;
	}

	if( color == 0xFFFF )
		color = saycolor_;

	cUOTxUnicodeSpeech::eSpeechType speechType;

	switch( type )
	{
	case 0x01:		speechType = cUOTxUnicodeSpeech::Broadcast;		break;
	case 0x06:		speechType = cUOTxUnicodeSpeech::System;		break;
	case 0x09:		speechType = cUOTxUnicodeSpeech::Yell;			break;
	case 0x02:		speechType = cUOTxUnicodeSpeech::Emote;			break;
	case 0x08:		speechType = cUOTxUnicodeSpeech::Whisper;		break;
	case 0x0A:		speechType = cUOTxUnicodeSpeech::Spell;			break;
	default:		speechType = cUOTxUnicodeSpeech::Regular;		break;
	};

	cUOTxUnicodeSpeech* textSpeech = new cUOTxUnicodeSpeech();
	textSpeech->setSource( serial() );
	textSpeech->setModel( body_ );
	textSpeech->setFont( 3 ); // Default Font
	textSpeech->setType( speechType );
	textSpeech->setLanguage( "" );
	textSpeech->setName( name() );
	textSpeech->setColor( color );
	textSpeech->setText( message );

	if( socket )
	{
		socket->send( textSpeech );
	}
	else
	{
		// Send to all clients in range
		for( cUOSocket *mSock = Network::instance()->first(); mSock; mSock = Network::instance()->next() )
		{
			if( mSock->player() && ( mSock->player()->dist( this ) < 18 ) )
			{
				mSock->send( new cUOTxUnicodeSpeech( *textSpeech ) );
			}
		}
		delete textSpeech;
	}
}

void cNPC::talk( const UINT32 MsgID, const QString& params /*= 0*/, const QString& affix /*= 0*/, bool prepend /*= false*/, UI16 color /*= 0xFFFF*/, cUOSocket* socket /*= 0*/ )
{
	if ( color == 0xFFFF )
		color = saycolor_;

	if ( socket )
	{
		if ( affix.isEmpty() )
			socket->clilocMessage( MsgID, params, color, 3, this );
		else
			socket->clilocMessageAffix( MsgID, params, affix, color, 3, this, false, prepend );
	}
	else
	{
		// Send to all clients in range
		for( cUOSocket *mSock = Network::instance()->first(); mSock; mSock = Network::instance()->next() )
		{
			if( mSock->player() && ( mSock->player()->dist( this ) < 18 ) )
			{
				if ( affix.isEmpty() )
					mSock->clilocMessage( MsgID, params, color, 3, this );
				else
					mSock->clilocMessageAffix( MsgID, params, affix, color, 3, this, false, prepend );
			}
		}
	}
}

UINT8 cNPC::notoriety( P_CHAR pChar ) // Gets the notoriety toward another char
{
	if (isIncognito()) {
		return 0x03;
	}

	/*
		Hard to tell because the ai-types are now string based
		0 = invalid/across server line
		1 = innocent (blue)
		2 = guilded/ally (green)
		3 = attackable but not criminal (gray)
		4 = criminal (gray)
		5 = enemy (orange)
		6 = murderer (red)
		7 = invulnerable (yellow)
		//7 = unknown use (translucent (like 0x4000 hue))
	*/
	UINT8 result;

	if (isInvulnerable()) {
		return 7;
	}

	// Check for Guild status + Highlight
//	UINT8 guildStatus = GuildCompare( this, pChar );

//	if( npcaitype() == 0x02 )
//		return 0x06; // 6 = Red -> Monster

	if( ai_ && ai_->notorietyOverride() )
		return ai_->notorietyOverride();

	if( !pChar )
		return 3;

	if( pChar->kills() > SrvParams->maxkills() )
		result = 0x06; // 6 = Red -> Murderer

//	else if( guildStatus == 1 )
//		result = 0x02; // 2 = Green -> Same Guild

//	else if( guildStatus == 2 )
//		result = 0x05; // 5 = Orange -> Enemy Guild

	else
	{
		// Monsters are always bad
//		if( npcaitype_ == 4 )
//			return 0x01;

		if( isHuman() )
		{
			if( karma_ >= 0 )
				result = 0x01;
			else
				result = 0x06;
		}

		// Everything else
		else
		{
			return 3;
		}
	}

	return result;
}

/*!
	If this character is in a guarded area, it checks the surroundings for criminals
	or murderers and spawns a guard if one is found.
*/
void cNPC::callGuards()
{
	if( nextGuardCallTime() < uiCurrentTime )
	{
		setNextGuardCallTime( uiCurrentTime + (MY_CLOCKS_PER_SEC*10) );
	} else
		return;

	cBaseChar::callGuards();
}

void cNPC::applyDefinition( const cElement *sectionNode )
{
	cBaseChar::applyDefinition( sectionNode );

	// Let's try to assume some unspecified values
	if ( this->strength() && !this->hitpoints() ) // we don't want to instantly die, right?
	{
		if ( !this->maxHitpoints() )
			setMaxHitpoints( strength() );
		setHitpoints( maxHitpoints() );
	}
}

void cNPC::showName( cUOSocket *socket )
{
	if( !socket->player() )
		return;

	QString charName = name();

	// apply titles
	if( SrvParams->showNpcTitles() && !title_.isEmpty() )
		charName.append( ", " + title_ );

	// Append serial for GMs
	if( socket->player()->showSerials() )
		charName.append( QString( " [0x%1]" ).arg( serial(), 4, 16 ) );

	// Frozen
	if (isFrozen())
		charName.append( tr(" [frozen]") );

	// Guarded
	if( guardedby_.size() > 0 )
		charName.append( tr(" [guarded]") );

	// Guarding
	if( isTamed() && guarding_ )
		charName.append( tr(" [guarding]") );

	Q_UINT16 speechColor;

	// 0x01 Blue, 0x02 Green, 0x03 Grey, 0x05 Orange, 0x06 Red
	switch( notoriety( socket->player() ) )
	{
		case 0x01:	speechColor = 0x59;		break; //blue
		case 0x02:	speechColor = 0x3F;		break; //green
		case 0x03:	speechColor = 0x3B2;	break; //grey
		case 0x05:	speechColor = 0x90;		break; //orange
		case 0x06:	speechColor = 0x22;		break; //red
		default:	speechColor = 0x3B2;	break; // grey
	}

	if (isInvulnerable()) {
		speechColor = 0x35;
	}

	// Show it to the socket
	socket->showSpeech( this, charName, speechColor, 3, cUOTxUnicodeSpeech::System );
}

void cNPC::soundEffect( UI16 soundId, bool hearAll )
{
	if( !hearAll )
		return;

	cUOTxSoundEffect pSoundEffect;
	pSoundEffect.setSound( soundId );
	pSoundEffect.setCoord( pos() );

	// Send the sound to all sockets in range
	for( cUOSocket *s = Network::instance()->first(); s; s = Network::instance()->next() )
		if( s->player() && s->player()->inRange( this, s->player()->visualRange() ) )
			s->send( &pSoundEffect );
}

void cNPC::giveGold( Q_UINT32 amount, bool inBank )
{
	P_ITEM pCont = getBackpack();

	if( !pCont )
		return;

	// Begin Spawning
	Q_UINT32 total = amount;

	while( total > 0 )
	{
		P_ITEM pile = cItem::createFromScript( "eed" );
		pile->setAmount( QMIN( total, static_cast<Q_UINT32>(65535) ) );
		pCont->addItem( pile );
		total -= pile->amount();
	}
}

UINT32 cNPC::takeGold( UINT32 amount, bool useBank )
{
	P_ITEM pPack = getBackpack();

	UINT32 dAmount = 0;

	if( pPack )
		dAmount = pPack->DeleteAmount( amount, 0xEED, 0 );

	return dAmount;
}

void cNPC::processNode( const cElement *Tag )
{
	changed_ = true;
	QString TagName = Tag->name();
	QString Value = Tag->value();

	//<attack min=".." max= "" />
	//<attack>10</attack>
	if( TagName == "attack" )
	{
		if( Tag->hasAttribute("min") && Tag->hasAttribute("max") )
		{
			minDamage_ = hex2dec( Tag->getAttribute("min") ).toInt();
			maxDamage_ = hex2dec( Tag->getAttribute("max") ).toInt();
		}
		else
		{
			minDamage_ = Value.toInt();
			maxDamage_ = minDamage_;
		}
	}

	//<npcwander type="rectangle" x1="-10" x2="12" y1="5" y2="7" />
	//<......... type="rect" ... />
	//<......... type="3" ... />
	//<......... type="circle" radius="10" />
	//<......... type="2" ... />
	//<......... type="free" (or "1") />
	//<......... type="none" (or "0") />
	else if( TagName == "npcwander" )
	{
		if( Tag->hasAttribute("type") )
		{
			QString wanderType = Tag->getAttribute("type");
			if( wanderType == "rectangle" || wanderType == "rect" || wanderType == "3" )
				if( Tag->hasAttribute("x1") &&
					Tag->hasAttribute("x2") &&
					Tag->hasAttribute("y1") &&
					Tag->hasAttribute("y2") )
				{
					wanderType_ = stWanderType( pos().x + Tag->getAttribute("x1").toInt(),
						pos().x + Tag->getAttribute("x2").toInt(),
						pos().y + Tag->getAttribute("y1").toInt(),
						pos().y + Tag->getAttribute("y2").toInt() );
				}
			else if( wanderType == "circle" || wanderType == "4" )
			{
				wanderType_ = stWanderType( pos().x, pos().y, 5 );
				if( Tag->hasAttribute("radius") )
					setWanderRadius( Tag->getAttribute("radius").toInt() );
			}
			else if( wanderType == "free" || wanderType == "2" )
				wanderType_ = stWanderType( enFreely );
			else
				wanderType_ = stWanderType();
		}
	}

	//<shopkeeper>
	//	<sellable>...handled like item-<contains>-section...</sellable>
	//	<buyable>...see above...</buyable>
	//	<restockable>...see above...</restockable>
	//</shopkeeper>
	else if( TagName == "shopkeeper" )
	{
		makeShop();

		for( unsigned int i = 0; i < Tag->childCount(); ++i )
		{
			const cElement *currNode = Tag->getChild( i );

			if( !currNode->childCount() )
				continue;

			unsigned char contlayer = 0;
			if( currNode->name() == "restockable" )
				contlayer = BuyRestockContainer;
			else if( currNode->name() == "buyable" )
				contlayer = BuyNoRestockContainer;
			else if( currNode->name() == "sellable" )
				contlayer = SellContainer;
			else
				continue;

			P_ITEM contItem = this->GetItemOnLayer( contlayer );
			if( contItem != NULL )
				contItem->processContainerNode( currNode );
		}
	}

	else if( TagName == "inherit" )
	{
		QString inheritID;
		if( Tag->hasAttribute( "id" ) )
			inheritID = Tag->getAttribute( "id" );
		else
			inheritID = Value;

		const cElement *element = DefManager->getDefinition( WPDT_NPC, inheritID );
		if( element )
			applyDefinition( element );
	}

	else
		cBaseChar::processNode( Tag );

}

// Simple setting and getting of properties for scripts and the set command.
stError *cNPC::setProperty( const QString &name, const cVariant &value )
{
	changed( TOOLTIP );
	changed_ = true;
	/*
		\property char.nextmsgtime This integer value is the next time the npc will say something.
		This property is exclusive to NPC objects.
	*/
	SET_INT_PROPERTY( "nextmsgtime", nextMsgTime_ )

	/*
		\property char.controlslots The amount of follower slots this npc will consume when owned
		by a player.
		This property is exclusive to player characters.
	*/
	else SET_INT_PROPERTY( "controlslots", controlSlots_ )

	/*
		\property char.nextguardcalltime This integer value is the next time the npc will call for a guard.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "nextguardcalltime", nextGuardCallTime_ )
	/*
		\property char.carve This string property is the id of the carve XML definition used for the corpse this NPC will leave behind.
		This property is exclusive to NPC objects.
	*/
	else SET_STR_PROPERTY( "carve", carve_ )
	/*
		\property char.spawnregion This string property is the id of the spawnregion this NPC was spawned by.
		This property is exclusive to NPC objects.
	*/
	else SET_STR_PROPERTY( "spawnregion", spawnregion_ )
	/*
		\property char.stablemaster If this integer property is not -1, the NPC won't be visible. Is is something like a container
		value for NPCs. Item and character serials are valid here. If you set the stablemaster serial, the NPC will be automatically
		removed from the map.
		This property is exclusive to NPC objects.
	*/
	else if (name == "stablemaster") {
		setStablemasterSerial(value.toInt());
		return 0;
	}

	/*
		\property char.lootlist This string is the id of the XML definition list for the loot in the corpse this NPC will leave behind.
		This property is exclusive to NPC objects.
	*/
	else SET_STR_PROPERTY( "lootlist", lootList_ )

	/*
		\property char.maxdamage The maximum damage the natural weapon of this npc can deal.
		This property is only used if the NPC doesn't use a weapon.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "maxdamage", maxDamage_ )

	/*
		\property char.mindamage The minimum damage the natural weapon of this npc can deal.
		This property is only used if the NPC doesn't use a weapon.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "mindamage", minDamage_ )

	/*
		\property char.nextmovetime This integer is the time the npc will move next.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "nextmovetime", nextMoveTime_ )

	/*
		\property char.summoned This boolean flag indicates whether the NPC was summoned and doesn't leave a corpse behind.
		This property is exclusive to NPC objects.
	*/
	else if (name == "summoned") {
		setSummoned(value.toInt() == 1);
		return 0;

	/*
		\property char.wandertype This integer is the type of wander algorithm used for this character. One of the following values is possible:
		<code>0x00 Standing
		0x01 Rectangle
		0x02 Circle
		0x03 FollowTarget
		0x04 Destination</code>
		This property is exclusive to NPC objects.
	*/
	} else if( name == "wandertype" )
	{
		setWanderType( (enWanderTypes)value.toInt() );
		return 0;
	}

	/*
		\property char.wanderx1 This integer property is either the x component of the upper left corner of the wander rectangle or the x component of the coordinate used by the Circle wandertype.
		This property is exclusive to NPC objects.
	*/
	else if( name == "fx1" || name == "wanderx1" )
	{
		setWanderX1( value.toInt() );
		return 0;
	}
	/*
		\property char.wanderx2 This integer property is the x component of the lower right corner of the wander rectangle.
		This property is exclusive to NPC objects.
	*/
	else if( name == "fx2" || name == "wanderx2" )
	{
		setWanderX2( value.toInt() );
		return 0;
	}
	/*
		\property char.wandery1 This integer property is either the y component of the upper left corner of the wander rectangle or the y component of the coordinate used by the Circle wandertype.
		This property is exclusive to NPC objects.
	*/
	else if( name == "fy1" || name == "wandery1" )
	{
		setWanderY1( value.toInt() );
		return 0;
	}
	/*
		\property char.wandery2 This integer property is the y component of the lower right corner of the wander rectangle.
		This property is exclusive to NPC objects.
	*/
	else if( name == "fy2" || name == "wandery2" )
	{
		setWanderY2( value.toInt() );
		return 0;
	}
	/*
		\property char.wanderradius This integer property is the radius of the circle used by the Circle wandertype.
		This property is exclusive to NPC objects.
	*/
	else if( name == "fz1" || name == "wanderradius" )
	{
		setWanderRadius( value.toInt() );
		return 0;
	}
	/*
		\property char.totame The required taming skill to tame this NPC.
		Please note that this skill is the float value multiplied by 10 (100.0% = 1000).
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "totame", tamingMinSkill_ )
	/*
		\property char.summontime The servertime when this summoned creature will disappear.
		If this integer value is 0, the NPC will never disappear.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "summontime", summonTime_)
	/*
		\property char.owner This is the owner of this NPC. It returns a character object or None.
		Please note that you automatically tame the NPC by setting a new owner.
		This property is exclusive to NPC objects.
	*/
	else if( name == "owner" )
	{
		P_PLAYER pOwner = dynamic_cast<P_PLAYER>(value.toChar());
		setOwner( pOwner );
		return 0;
	}

	// skill.
	else if( name.left( 6 ) == "skill." )
	{
		QString skill = name.right( name.length() - 6 );
		INT16 skillId = Skills->findSkillByDef( skill );

		if( skillId != -1 )
		{
			setSkillValue( skillId, value.toInt() );
			return 0;
		}
	}
	/*
		\property char.ai This string property is the name of the AI used by this NPC.
		This property is exclusive to NPC objects.
	*/
	else if( name == "ai" )
	{
		setAI( value.toString() );
		return 0;
	}
	else SET_INT_PROPERTY( "fleeat", criticalHealth_ )
	/*
		\property char.criticalhealth If the NPCs hitpoints fall below the percentage given in this integer property,
		he flees from his attacker if its ai supports it.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "criticalhealth", criticalHealth_ )
	/*
		\property char.spellslow This integer bitfield indicates the spells of the first 4 circles this NPC is able to
		cast without a spellbook.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "spellslow", spellsLow_ )
	/*
		\property char.spellshigh This integer bitfield indicates the spells of the last 4 circles this NPC is able to
		cast without a spellbook.
		This property is exclusive to NPC objects.
	*/
	else SET_INT_PROPERTY( "spellshigh", spellsHigh_ )
	else if( name == "spell" )
	{
		UINT8 spell = name.right( name.length() - 6 ).toShort();

		setSpell( spell, value.toInt() );
		return 0;
	// skillcap.
	} else if( name.left( 9 ) == "skillcap." ) {
		QString skill = name.right( name.length() - 9 );
		INT16 skillId = Skills->findSkillByDef( skill );

		if( skillId != -1 )
		{
			setSkillCap(skillId, value.toInt());
			return 0;
		}
	} else {
		INT16 skillId = Skills->findSkillByDef(name);

		if (skillId != -1) {
			setSkillValue(skillId, value.toInt());
			return 0;
		}
	}

	return cBaseChar::setProperty( name, value );
}

stError *cNPC::getProperty( const QString &name, cVariant &value ) const
{
	GET_PROPERTY( "nextmsgtime", (int)nextMsgTime_ )
	else GET_PROPERTY( "controlslots", (int)controlSlots_ )
	else GET_PROPERTY( "antispamtimer", (int)nextMsgTime_ )
	else GET_PROPERTY( "nextguardcalltime", (int)nextGuardCallTime_ )
	else GET_PROPERTY( "antiguardstimer", (int)nextGuardCallTime_ )
	else GET_PROPERTY( "carve", carve_ )
	else GET_PROPERTY( "spawnregion", spawnregion_ )
	else GET_PROPERTY( "stablemaster", stablemasterSerial_ )
	else GET_PROPERTY( "lootlist", lootList_ )
	else GET_PROPERTY( "loot", lootList_ )
	else GET_PROPERTY( "maxdamage", maxDamage_ )
	else GET_PROPERTY( "mindamage", minDamage_ )
	else GET_PROPERTY( "npc", true )
	else GET_PROPERTY( "nextmovetime", (int)nextMoveTime_ )
	else GET_PROPERTY( "npcmovetime", (int)nextMoveTime_ )
	else GET_PROPERTY( "wandertype", (int)wanderType() )
	else GET_PROPERTY( "wanderx1", wanderX1() )
	else GET_PROPERTY( "fx1", wanderX1() )
	else GET_PROPERTY( "wanderx2", wanderX2() )
	else GET_PROPERTY( "fx2", wanderX2() )
	else GET_PROPERTY( "wandery1", wanderY1() )
	else GET_PROPERTY( "fy1", wanderY1() )
	else GET_PROPERTY( "wandery2", wanderY2() )
	else GET_PROPERTY( "fy2", wanderY2() )
	else GET_PROPERTY( "wanderradius", wanderRadius() )
	else GET_PROPERTY( "fz1", wanderRadius() )
	else GET_PROPERTY( "totame", tamingMinSkill_ )
	else GET_PROPERTY( "summontime", (int)summonTime_)
	else GET_PROPERTY( "summontimer", (int)summonTime_)
	else GET_PROPERTY( "owner", owner_ )
	else GET_PROPERTY( "ai", aiid_ )
	else GET_PROPERTY( "fleeat", criticalHealth_ )
	else GET_PROPERTY( "criticalhealth", criticalHealth_ )
	else GET_PROPERTY( "spellslow", (int)spellsLow_ )
	else GET_PROPERTY( "spellshigh", (int)spellsHigh_ )
	else GET_PROPERTY( "summoned", (int)(summoned() ? 1 : 0))
	else if(name == "spell" )
	{
		UINT8 spell = name.right( name.length() - 6 ).toShort();

		value = cVariant( hasSpell( spell ) );
		return 0;
	}

	return cBaseChar::getProperty( name, value );
}

Coord_cl cNPC::nextMove()
{
	Coord_cl ret;
	if( !path_.empty() )
	{
		ret = path_.front();
	}
	else
		ret = Coord_cl( 0xFFFF, 0xFFFF, 0xFF, 0 );

	return ret;
}

void cNPC::pushMove( UI16 x, UI16 y, SI08 z )
{
	path_.push_back( Coord_cl( x, y, z, 0 ) );
}

void cNPC::pushMove( const Coord_cl &move )
{
	path_.push_back( move );
}

void cNPC::popMove( void )
{
	path_.pop_front();
}

void cNPC::clearPath( void )
{
	path_.clear();
}

bool cNPC::hasPath( void )
{
	return !path_.empty();
}

Coord_cl cNPC::pathDestination( void ) const
{
	if( path_.empty() )
		return Coord_cl( 0xFFFF, 0xFFFF, 0xFF, 0 );
	else
		return path_.back();
}

/*!
	A* pathfinding algorithm implementation. consult me (sereg) for changes!
	I've used this paper as a base: http://www.wpdev.org/docs/informed-search.pdf
	A*>>
*/

/*!
	We'll need to build a list of pathnodes. Each pathnode contains
	coordinates, iteration step, cost and a pointer to its predecessor.
*/
class pathnode_cl
{
public:
	pathnode_cl( UI16 x_, UI16 y_, SI08 z_, UI16 step_, float cost_ ) : prev( NULL ), x( x_ ), y( y_ ), z( z_ ), step( step_ ), cost( cost_ ) {}
	pathnode_cl() : prev( NULL ) {}
	UI16	x;
	UI16	y;
	SI08	z;
	UI16	step;
	float	cost;

	pathnode_cl *prev;
};

/*!
	Compare predicate for pathnodes using cost and step
	Two times because we need more weight for the costs.
*/
struct pathnode_comparePredicate : public std::binary_function<pathnode_cl, pathnode_cl, bool>
{
	bool operator()(const pathnode_cl *a, const pathnode_cl *b)
	{
		return (a->step + 2 * a->cost) > (b->step + 2 * b->cost);
	}
};

/*!
	Compare predicate for pathnodes using coordinates
*/
struct pathnode_coordComparePredicate : public std::binary_function<pathnode_cl, pathnode_cl, bool>
{
	bool operator()(const pathnode_cl *a, const pathnode_cl *b)
	{
		return ( a->x * a->x + a->y * a->y + a->z * a->z ) <
				( b->x * b->x + b->y * b->y + b->z * b->z );
	}
};

/*!
	Heuristic function for A*
	We use simple 3-dim. euclid distance: d = sqrt( (x1-x2) + (y1-y2) + (z1-z2) )
*/
float cNPC::pathHeuristic( const Coord_cl &source, const Coord_cl &destination )
{
	return (float)( sqrt( pow( source.x - destination.x, 2 ) + pow( source.y - destination.y, 2 ) + pow( (source.z - destination.z) / 5.0f, 2 ) ) );
}

/*!
	The algorithm..
	currently works in x,y,z direction. no idea how to implement map jumping yet.
*/
void cNPC::findPath( const Coord_cl &goal, float sufficient_cost /* = 0.0f */ )
{
	if( path_.size() > 0 )
		path_.clear();

	if( pos_.map != goal.map )
		return;

	/*
		For A* we use a priority queue to store the unexamined path nodes
		in a way, that the node with lowest cost lies always at the beginning.
		I'll use std::vector combined with the stl heap functions for it.
	*/
	std::vector< pathnode_cl* >	unvisited_nodes;
	std::vector< pathnode_cl* >	visited_nodes;
	std::make_heap( unvisited_nodes.begin(), unvisited_nodes.end(), pathnode_comparePredicate() );

	/*
		We also need a vector for the calculated list.
		And a temporary vector of neighbours.
	*/
	std::vector< pathnode_cl* >	allnodes;
	std::vector< Coord_cl >		neighbours;
	std::vector< pathnode_cl* >::iterator	pit;
	std::vector< Coord_cl >::iterator		nit;

	/*
		So let's start :)
		The initial thing we'll have to do is to push the current char position
		into the priority queue.
	*/
	pathnode_cl *baseNode = new pathnode_cl( pos_.x, pos_.y, pos_.z, 0, pathHeuristic( pos_, goal ) );
	unvisited_nodes.push_back( baseNode );
	// pushing the heap isnt needed here..
	allnodes.push_back( baseNode );

	/*
		Each iteration of the following loop will take the first element
		out of the priority queue and calculate the neighbour nodes
		(sourrounding coordinates). The nodes have prev pointers which let
		us get the whole path in the end.

		All neighbours that are reachable will be pushed into the priority
		queue.

		If no neighbour is reachable we ran into a dead end. Nothing will
		be done in this iteration, because we'll get a better node out of the
		priority queue next iteration.
	*/

	pathnode_cl *currentNode = NULL;
	pathnode_cl *newNode = NULL;
	pathnode_cl *prevNode = NULL;
	int iterations = 0;
	while( !unvisited_nodes.empty() )
	{
		// get the first element of the queue
		bool visited;
		do
		{
			visited = false;
			currentNode = *unvisited_nodes.begin();

			// remove it from the queue
			std::pop_heap( unvisited_nodes.begin(), unvisited_nodes.end(), pathnode_comparePredicate() );
			unvisited_nodes.pop_back();

			// dont calculate loops !

			visited = std::binary_search( visited_nodes.begin(), visited_nodes.end(), currentNode, pathnode_coordComparePredicate() );

			// steps > step depth
		} while( ( visited || currentNode->step > SrvParams->pathfindMaxSteps() ) && !unvisited_nodes.empty() );

		if( iterations > SrvParams->pathfindMaxIterations() || currentNode->step > SrvParams->pathfindMaxSteps() )
		{
			// lets set the pointer invalid if we have an invalid path
			currentNode = NULL;
			break;
		}

		// we reached the goal
		if( currentNode->cost <= sufficient_cost )
			break;

		// the neighbours can be all surrounding x-y-coordinates
		neighbours.clear();
		int i = 0;
		int j = 0;
		Coord_cl pos;
		for( i = -1; i <= 1; ++i )
		{
			for( j = -1; j <= 1; ++j )
			{
				if( i != 0 || j != 0 )
					neighbours.push_back( Coord_cl( currentNode->x + i, currentNode->y + j, currentNode->z, pos_.map ) );
			}
		}

		// check walkability and set z offsets for neighbours
		nit = neighbours.begin();
		while( nit != neighbours.end() )
		{
			pos = *nit;
			// this should change z coordinates also if there are stairs
			if( mayWalk( this, pos ) && !Movement::instance()->CheckForCharacterAtXYZ( this, pos ) )
			{
				// push a path node into the priority queue
				newNode = new pathnode_cl( pos.x, pos.y, pos.z, currentNode->step + 1, pathHeuristic( pos, goal ) );
				newNode->prev = currentNode;
				unvisited_nodes.push_back( newNode );
				allnodes.push_back( newNode );
				std::push_heap( unvisited_nodes.begin(), unvisited_nodes.end(), pathnode_comparePredicate() );
			}
			++nit;
		}

		visited_nodes.push_back( currentNode );
		std::sort( visited_nodes.begin(), visited_nodes.end(), pathnode_coordComparePredicate() );
		++iterations;
	}

	// finally lets set the char's path
	if( currentNode )
	{
		path_.clear();
		while( currentNode->prev )
		{
			path_.push_front( Coord_cl( currentNode->x, currentNode->y, currentNode->z, pos_.map ) );
			currentNode = currentNode->prev;
		}
	}

	/* debug...
	Console::instance()->send( QString( "Pathfinding: %1 iterations\n" ).arg( iterations ) );
	std::deque< Coord_cl >::const_iterator it = path_.begin();
	while( it != path_.end() )
	{
		Console::instance()->send( QString( "%1,%2\n" ).arg( (*it).x ).arg( (*it).y ) );
		++it;
	}
	*/

	// lets free the memory assigned to the nodes...
	pit = allnodes.begin();
	while( pit != allnodes.end() )
	{
		delete *pit;
		++pit;
	}
}

/*!
	<<A*
*/

void cNPC::setAI( const QString &data )
{
	QString tmp = QStringList::split( ",", data )[0];
	aiid_ = tmp;

	if( ai_ )
		delete ai_;

	ai_ = NULL;

	AbstractAI* ai = AIFactory::instance()->createObject( tmp );
	if( !ai )
		return;

	ScriptAI* sai = dynamic_cast< ScriptAI* >( ai );
	if( sai )
	{
		sai->setName( tmp );
	}
	ai->init( this );

	setAI( ai );
}

void cNPC::makeShop()
{
	P_ITEM currCont = GetItemOnLayer( BuyRestockContainer );
	if( !currCont )
	{
		currCont = cItem::createFromScript( "e75" );
		currCont->setOwner( this );
		addItem( BuyRestockContainer, currCont );
		currCont->update();
	}

	currCont = GetItemOnLayer( BuyNoRestockContainer );
	if( !currCont )
	{
		currCont = cItem::createFromScript( "e75" );
		currCont->setOwner( this );
		addItem( BuyNoRestockContainer, currCont );
		currCont->update();
	}

	currCont = GetItemOnLayer( SellContainer );
	if( !currCont )
	{
		currCont = cItem::createFromScript( "e75" );
		currCont->setOwner( this );
		addItem( SellContainer, currCont );
		currCont->update();
	}
}

void cNPC::awardKarma( P_CHAR pKilled, short amount )
{
	int nCurKarma = 0, nChange = 0, nEffect = 0;

	nCurKarma = karma();

	if( nCurKarma < amount && amount > 0 )
	{
		nChange=((amount-nCurKarma)/75);
		setKarma(nCurKarma+nChange);
		nEffect=1;
	}

	if( ( nCurKarma > amount ) && ( !pKilled ) )
	{
		nChange = ( ( nCurKarma - amount ) / 50 );
		setKarma( nCurKarma - nChange );
		nEffect = 0;
	}
	else if( ( nCurKarma > amount ) && ( pKilled->karma() > 0 ) )
	{
		nChange= ( ( nCurKarma - amount ) / 50 );
		setKarma( nCurKarma - nChange );
		nEffect=0;
	}

	// Cap at 10000 or -10000
	if( karma_ > 10000 )
		karma_ = 10000;
	else if( karma_ < -10000 )
		karma_ = -10000;
}

void cNPC::awardFame( short amount )
{
	int nCurFame, nChange=0;

	setFame( QMIN( 10000, fame() ) );

	nCurFame = fame();

	// We already have more than that.
	if( nCurFame > amount )
		return;

	// Loose Fame when we died
	if( isDead() )
	{
		// Fame / 25 is our loss
		nChange = nCurFame / 25;
		setFame( QMAX( 0, nCurFame - nChange ) );
		setDeaths( deaths() + 1 );
	}
	else
	{
		nChange = ( amount - nCurFame ) / 75;
		setFame( nCurFame+nChange );
	}
}

void cNPC::vendorBuy( P_PLAYER player )
{
	P_ITEM pContA = GetItemOnLayer( cBaseChar::BuyRestockContainer );
	P_ITEM pContB = GetItemOnLayer( cBaseChar::BuyNoRestockContainer );

	if ( player->isDead() )
		return;

	if( !pContA && !pContB )
	{
		talk( tr( "Sorry but i have no goods to sell" ) );
		return;
	}

	talk( 500186, 0, 0, false, saycolor(), player->socket() ); // Greetings.  Have a look around.
	player->socket()->sendBuyWindow( this );
}

void cNPC::vendorSell( P_PLAYER player )
{
	P_ITEM pContC = GetItemOnLayer( cBaseChar::SellContainer );

	if (!pContC || pContC->content().size() == 0) {
		talk(501550, 0, 0, false, saycolor_, player->socket());
		return;
	}

	player->socket()->sendSellWindow( this, player );
}

void cNPC::log( eLogLevel loglevel, const QString &string )
{
	// NPC's usually don't have sockets
	Log::instance()->print( loglevel, string );
}

void cNPC::log( const QString &string )
{
	log( LOG_NOTICE, string );
}

PyObject *cNPC::getPyObject() {
	return PyGetCharObject(this);
}

const char *cNPC::className() const {
	return "npc";
}

bool cNPC::inWorld() {
	return stablemasterSerial() == INVALID_SERIAL;
}

void cNPC::createTooltip(cUOTxTooltipList &tooltip, cPlayer *player) {
	cUObject::createTooltip(tooltip, player);

	QString affix;

	if (!title_.isEmpty()) {
		affix = " " + title_;
	} else {
		affix = " ";
	}

	// Append the (frozen) tag
	if (isFrozen()) {
		if (affix.endsWith(" ")) {
			affix.append(tr("(frozen)"));
		} else {
			affix.append(tr(" (frozen)"));
		}
	}

	tooltip.addLine(1050045, QString(" \t%1\t%2").arg(name_).arg(affix));
	onShowTooltip(player, &tooltip);
}

void cNPC::setStablemasterSerial(SERIAL data)
{
	stablemasterSerial_ = data;
	changed_ = true;

	if (data == INVALID_SERIAL) {
		MapObjects::instance()->add(this);
	} else {
		MapObjects::instance()->remove(this);
	}
}

cNPC *cNPC::createFromScript(const QString &section, const Coord_cl &pos) {
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

void cNPC::remove() {
	// If a player is mounted on us, unmount him.
	if (stablemasterSerial_ != INVALID_SERIAL) {
		P_PLAYER player = dynamic_cast<P_PLAYER>(World::instance()->findChar(stablemasterSerial_));
		if (player) {
			P_ITEM mount = player->atLayer(cBaseChar::Mount);

			if (mount && mount->getTag("pet").toInt() == serial_) {
				mount->remove();
			}
		}
	}

	setOwner(0);
	cBaseChar::remove();
}
