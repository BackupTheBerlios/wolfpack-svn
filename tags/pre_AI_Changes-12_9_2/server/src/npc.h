//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
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

#ifndef CNPC_H_HEADER_INCLUDED
#define CNPC_H_HEADER_INCLUDED

// platform includes
#include "platform.h"

// library includes
#include <deque>

// wolfpack includes
#include "basechar.h"

class cNPC_AI;

// Class for Non Player Characters. Implements cBaseChar.
class cNPC : public cBaseChar
{
	Q_OBJECT
public:
	// con-/destructors
    cNPC();
    cNPC(const cNPC& right);
    virtual ~cNPC();
	// operators
    cNPC& operator=(const cNPC& right);

	// type definitions
	struct stWanderType
	{
		// constructors
		stWanderType() : type( enHalt ) {}
		stWanderType( enWanderTypes type_ ) : type( type_ ) {}
		stWanderType( UINT16 x1_, UINT16 x2_, UINT16 y1_, UINT16 y2_ ) : 
			x1( x1_ ), x2( x2_ ), y1( y1_ ), y2( y2_ ), type( enRectangle ) {}
		stWanderType( UINT16 x_, UINT16 y_, UINT16 radius_ ) : 
			x1( x_ ), y1( y_ ), radius( radius_ ), type( enCircle ) {}
		
		// attributes
		enWanderTypes type;
		// rectangles and circles
		UINT16 x1;
		UINT16 x2;
		UINT16 y1;
		UINT16 y2;
		UINT16 radius;
	};

	// implementation of interfaces
	static void registerInFactory();
	void load( char **, UINT16& );
	void save();
	void save( FlatStore::OutputFile*, bool first = false ) throw();
	bool load( unsigned char chunkGroup, unsigned char chunkType, FlatStore::InputFile* ) throw();
	bool postload() throw();
	bool del();

	virtual enCharTypes objectType();
	virtual void update( bool excludeself = false ); 
	virtual void resend( bool clean = true, bool excludeself = false ); 
	virtual void talk( const QString &message, UI16 color = 0xFFFF, UINT8 type = 0, bool autospam = false, cUOSocket* socket = NULL );
	virtual UINT8 notority( P_CHAR pChar );
	virtual void kill();
	virtual void showName( cUOSocket *socket );
	virtual void fight(P_CHAR pOpponent);
	virtual void soundEffect( UI16 soundId, bool hearAll = true );
	virtual void giveGold( Q_UINT32 amount, bool inBank = false );
	virtual UINT32 takeGold( UINT32 amount, bool useBank = false );

	virtual void applyDefinition( const QDomElement& );

	// other public methods
	stError *setProperty( const QString &name, const cVariant &value );
	stError *getProperty( const QString &name, cVariant &value ) const;
	void attackTarget( P_CHAR defender );
	void toggleCombat();
	void setNextMoveTime( void );
	virtual void callGuards(); // overriding
	void makeShop();

	// getters
	UINT32			additionalFlags() const;
    UINT16			maxDamage() const;
    UINT16			minDamage() const;
    UINT32			nextBeggingTime() const;
    UINT32			nextGuardCallTime() const;
    UINT32			nextMoveTime() const;
    UINT32			nextMsgTime() const;
    UINT32			summonTime() const;
    INT16			tamingMinSkill() const;
	P_PLAYER		owner() const;
	QString			carve() const;
	QString			spawnregion() const;
	SERIAL			stablemasterSerial() const;
	QString			lootList() const;
	cNPC_AI*		ai() const;
	UINT32			aiCheckTime() const;
	UINT8			criticalHealth() const;
	// bit flag getters
	bool			hasSpell( UINT8 spell ) const;
	// advanced getters for data structures
	// path finding
	bool			hasPath( void );
	Coord_cl		nextMove();
	Coord_cl		pathDestination( void ) const;
	float			pathHeuristic( const Coord_cl &source, const Coord_cl &destination );
	// wander type
	enWanderTypes	wanderType() const;
	UINT16			wanderX1() const;
	UINT16			wanderX2() const;
	UINT16			wanderY1() const;
	UINT16			wanderY2() const;
	UINT16			wanderRadius() const;

	// setters
    void setMaxDamage(UINT16 data);
    void setAdditionalFlags(UINT32 data);
    void setMinDamage(UINT16 data);
    void setNextBeggingTime(UINT32 data);
    void setNextGuardCallTime(UINT32 data);
    void setNextMoveTime(UINT32 data);
    void setNextMsgTime(UINT32 data);
    void setSummonTime(UINT32 data);
    void setTamingMinSkill(INT16 data);
	void setOwner(P_PLAYER data, bool nochecks = false);
	void setCarve(const QString &data);
	void setSpawnregion(const QString &data);
	void setStablemasterSerial(SERIAL data);
	void setLootList(const QString &data);
    void setGuarding(P_PLAYER data);
	void setAI( cNPC_AI* ai );
	void setAICheckTime( UINT32 data );
	void setCriticalHealth( UINT8 data );
	// bit flag setters
	void setSpell( UINT8 spell, bool data );
	// advanced setters for data structures
	// AI
	void setAI( const QString &data );
	// path finding
	void pushMove( const Coord_cl &move );
	void pushMove( UI16 x, UI16 y, SI08 z );
	void popMove( void );
	void clearPath( void );
	void findPath( const Coord_cl &goal, float sufficient_cost = 0.0f );
	// wander type
	void setWanderType(enWanderTypes data);
	void setWanderX1(UINT16 data);
	void setWanderX2(UINT16 data);
	void setWanderY1(UINT16 data);
	void setWanderY2(UINT16 data);
	void setWanderRadius(UINT16 data);

protected:
	// interface implementation
	static void buildSqlString( QStringList &fields, QStringList &tables, QStringList &conditions );
	virtual void processNode( const QDomElement& Tag );

	// other protected methods

    // Minimum Damage when attacking without weapons.
    // cOldChar::lodamage_
    UINT16 minDamage_;

    // Max Damage value when attacking without weapons.
    // cOldChar::hidamage_
    UINT16 maxDamage_;

    // Time till NPC talks again.
    // cOldChar::antispamtimer_
    UINT32 nextMsgTime_;

    // Time till the NPC calls another guard.
    // cOldChar::antiguardstimer_
    UINT32 nextGuardCallTime_;

    // Time till the NPC handles another begging attempt.
    // cOldChar::begging_timer_
    UINT32 nextBeggingTime_;

    // Time till npc moves next.
    // cOldChar::npcmovetime_
    UINT32 nextMoveTime_;

    // Stores information about how the npc wanders. uses the struct
    // stWanderType with attributes for rectangles, circles and more...
    // 
    // cOldChar::npcWander_
    // cOldChar::fx1_ ...
    stWanderType wanderType_;

    // skill needed to tame this npc
    INT16 tamingMinSkill_;

    // Time till summoned creature disappears.
    // cOldChar::summontimer_
    UINT32 summonTime_;

    // Additional property flags
    // 
    // Bits:
    UINT32 additionalFlags_;

	// Owner of this NPC.
	P_PLAYER owner_;

	// XML def. section which specifies the items that appear when the body
	// of the NPC is carved.
	QString carve_;

	// Spawnregion which has spawned the NPC
	QString spawnregion_;

	// Serial of the stablemaster that stables the NPC.
	SERIAL stablemasterSerial_;

	// XML def. section which specifies the items that appear in the body
	// of the NPC, when it's killed.
	QString lootList_;

	// A* calculated path which the NPC walks on.
	std::deque< Coord_cl > path_;

	// NPC AI State Machine interface
	cNPC_AI* ai_;

	// NPC AI check timer
	UINT32	aiCheckTime_;

	// percentage of maxhitpoints when hitpoints are restored
	UINT8	criticalHealth_;

	// 2 * 32 = 64 bit flags for spells the npc can cast
	UINT32	spellsLow_;
	UINT32	spellsHigh_;
};

inline UINT32 cNPC::additionalFlags() const
{
    return additionalFlags_;
}

inline void cNPC::setAdditionalFlags(UINT32 data)
{
    additionalFlags_ = data;
	changed( SAVE );
}

inline UINT16 cNPC::maxDamage() const
{
    return maxDamage_;
}

inline void cNPC::setMaxDamage(UINT16 data)
{
    maxDamage_ = data;
	changed( SAVE );
}

inline UINT16 cNPC::minDamage() const
{
    return minDamage_;
}

inline void cNPC::setMinDamage(UINT16 data)
{
    minDamage_ = data;
	changed( SAVE );
}

inline UINT32 cNPC::nextBeggingTime() const
{
    return nextBeggingTime_;
}

inline void cNPC::setNextBeggingTime(UINT32 data)
{
    nextBeggingTime_ = data;
	changed( SAVE );
}

inline UINT32 cNPC::nextGuardCallTime() const
{
    return nextGuardCallTime_;
}

inline void cNPC::setNextGuardCallTime(UINT32 data)
{
    nextGuardCallTime_ = data;
	changed( SAVE );
}

inline UINT32 cNPC::nextMoveTime() const
{
    return nextMoveTime_;
}

inline void cNPC::setNextMoveTime(UINT32 data)
{
    nextMoveTime_ = data;
}

inline UINT32 cNPC::nextMsgTime() const
{
    return nextMsgTime_;
}

inline void cNPC::setNextMsgTime(UINT32 data)
{
    nextMsgTime_ = data;
}

inline UINT32 cNPC::summonTime() const
{
    return summonTime_;
}

inline void cNPC::setSummonTime(UINT32 data)
{
    summonTime_ = data;
	changed( SAVE );
}

inline INT16 cNPC::tamingMinSkill() const
{
    return tamingMinSkill_;
}

inline void cNPC::setTamingMinSkill(INT16 data)
{
    tamingMinSkill_ = data;
	changed( SAVE );
}

inline P_PLAYER cNPC::owner() const
{
	return owner_;
}

inline QString cNPC::carve() const
{
	return carve_;
}

inline void cNPC::setCarve(const QString &data)
{
	carve_ = data;
	changed( SAVE );
}

inline QString cNPC::spawnregion() const
{
	return spawnregion_;
}

inline void cNPC::setSpawnregion(const QString &data)
{
	spawnregion_ = data;
	changed( SAVE );
}

inline SERIAL cNPC::stablemasterSerial() const
{
	return stablemasterSerial_;
}

inline void cNPC::setStablemasterSerial(SERIAL data)
{
	stablemasterSerial_ = data;
	changed( SAVE );
}

inline QString cNPC::lootList() const
{
	return lootList_;
}

inline void cNPC::setLootList(const QString &data)
{
	lootList_ = data;
	changed( SAVE );
}

inline cNPC_AI* cNPC::ai() const
{
	return ai_;
}

inline void cNPC::setAI( cNPC_AI* ai )
{
	ai_ = ai;
	changed( SAVE );
}

inline UINT32 cNPC::aiCheckTime() const
{
	return aiCheckTime_;
}

inline void cNPC::setAICheckTime( UINT32 data )
{
	aiCheckTime_ = data;
	changed( SAVE );
}

inline bool cNPC::hasSpell( UINT8 spell ) const
{
	if( spell < 32 )
		return spellsLow_ & ( 1 << spell );
	else if( spell >= 32 && spell < 64 )
		return spellsHigh_ & ( 1 << (spell-32) );
	else
		return false;
}

inline void cNPC::setSpell( UINT8 spell, bool data )
{
	if( data ) 
	{
		if( spell < 32 )
			spellsLow_ |= ( 1 << spell );
		else if( spell >= 32 && spell < 64 )
			spellsHigh_ |= ( 1 << (spell-32) );
	}
	else 
	{
		if( spell < 32 )
			spellsLow_ &= ~( 1 << spell );
		else if( spell >= 32 && spell < 64 )
			spellsHigh_ &= ~( 1 << (spell-32) );
	}
	changed( SAVE );
}

inline UINT8 cNPC::criticalHealth() const
{
	return criticalHealth_;
}

inline void cNPC::setCriticalHealth( UINT8 data )
{
	criticalHealth_ = data;
	changed( SAVE );
}

inline enCharTypes cNPC::objectType()
{
	return enNPC;
}

inline enWanderTypes cNPC::wanderType() const
{
	return wanderType_.type;
}

inline UINT16 cNPC::wanderX1() const
{
	return wanderType_.x1;
}

inline UINT16 cNPC::wanderX2() const
{
	return wanderType_.x2;
}

inline UINT16 cNPC::wanderY1() const
{
	return wanderType_.y1;
}

inline UINT16 cNPC::wanderY2() const
{
	return wanderType_.y2;
}

inline UINT16 cNPC::wanderRadius() const
{
	return wanderType_.radius;
}

inline void cNPC::setWanderType(enWanderTypes data)
{
	wanderType_.type = data;
}

inline void cNPC::setWanderX1(UINT16 data)
{
	wanderType_.x1 = data;
}

inline void cNPC::setWanderX2(UINT16 data)
{
	wanderType_.x2 = data;
}

inline void cNPC::setWanderY1(UINT16 data)
{
	wanderType_.y1 = data;
}

inline void cNPC::setWanderY2(UINT16 data)
{
	wanderType_.y2 = data;
}

inline void cNPC::setWanderRadius(UINT16 data)
{
	wanderType_.radius = data;
}


#endif /* CNPC_H_HEADER_INCLUDED */
