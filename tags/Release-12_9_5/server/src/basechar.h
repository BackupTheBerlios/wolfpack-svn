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

#ifndef CBASECHAR_H_HEADER_INCLUDED
#define CBASECHAR_H_HEADER_INCLUDED

// platform includes
#include "platform.h"

// library includes
#include <qmap.h>
#include <qvaluevector.h>
#include <qdatetime.h>

// wolfpack includes
#include "typedefs.h"
#include "uobject.h"
#include "TmpEff.h"
#include "territories.h"
#include "log.h"
#include "multis.h"

// include for ceilf()
#include <math.h>

class cFightInfo;
class cMulti;
class cUOTxTooltipList;


// This class is the base interface for all char objects. 
class cBaseChar : public cUObject
{
public:
	const char *objectID() const
	{
        return "cBaseChar";
	}

	// con-/destructors
    cBaseChar();
    cBaseChar(const cBaseChar& right);
    virtual ~cBaseChar();
 
	// operators
    cBaseChar& operator=(const cBaseChar& right);

	// type definitions
	typedef QMap<ushort, cItem*> ItemContainer;
	typedef QValueVector< cBaseChar* > CharContainer;
	typedef QValueVector< cTempEffect* > EffectContainer;
	enum enLayer { TradeWindow = 0, SingleHandedWeapon, DualHandedWeapon, Shoes, Pants, Shirt, Hat, Gloves,
	Ring, Neck = 0xA, Hair, Waist, InnerTorso, Bracelet, FacialHair = 0x10,  MiddleTorso, 
	Earrings, Arms, Back, Backpack, OuterTorso, OuterLegs, InnerLegs, Mount, BuyRestockContainer,
	BuyNoRestockContainer, SellContainer, BankBox, Dragging };

	enum enBark {
		Bark_Attacking= 0,
		Bark_Idle,
		Bark_Hit,
		Bark_GetHit,
		Bark_Death
	};

	// implementation of interfaces
	void load( char **, ushort& );
	void save();
	bool del();

	// interface methods
	// object type specific methods
	virtual enCharTypes objectType() = 0;
	// network related methods
	virtual void update( bool excludeself = false ) = 0; 
	virtual void resend(bool clean = true) = 0; 
	// other methods
	virtual uchar notoriety( P_CHAR pChar ) = 0; // Gets the notoriety towards another char

	bool canSee(cUObject *object);
	virtual bool canSeeChar(P_CHAR character);
	virtual bool canSeeItem(P_ITEM item);
	virtual void moveTo(const Coord_cl &pos, bool noremove = false);
	bool lineOfSight(P_ITEM item, bool touch = false);
	bool lineOfSight(Coord_cl position, bool touch = false);
	bool lineOfSight(P_CHAR character, bool touch = false);

	/*!
		\brief This enumeration contains values that specify what events should be processed
			for the given character.
	*/
	enum TimingEvents {
		EventCombat = 0x01,
		EventLight = 0x02,
		EventTime = 0x04,
	};

	/*!
		\brief This function give this character time to process timing events.
		\param time The current servertime.
		\param events A bitfield of \a TimingEvents values to determine what exactly should be checked.
	*/
	virtual void poll(unsigned int time, unsigned int events);

	/*!
		\brief Kills the character.
		\returns True if the character was really killed and false if nothing changed.
	*/
	virtual bool kill(cUObject *source);

	/*!
		\brief This method tries to find an ongoing fight between this character and another.
		\param enemy The other character who participates in the fight.
		\returns A pointer to a fight object if found, a null pointer otherwise.
	*/
	cFightInfo *findFight(P_CHAR enemy);

	/*!
		\brief Enumeration of fight status codes that are returned by the fight functions.
	*/
	enum FightStatus {
		FightDenied = 0x00,
		FightStarted,
		FightContinued
	};

	/*!
		\brief Let this creature fight another creature or cancel an ongoing fight.
		\param enemy The enemy this creature should fight. May be null to cancel a fight.
		\returns A returncode of the \a eFightStatus enumeration.
	*/
	virtual FightStatus fight(P_CHAR enemy);

	/*!
		\brief Sends a system message to this character. The message is only sent to logged
			in players.
		\param message The text of the message.
		\param color The color of the sysmessage. The default is 0x3b2.
		\param font The font for the sysmessage. The default is 0x03.
		\returns True if the message was really sent. False otherwise.
	*/
	bool sysmessage(const QString &message, unsigned short color = 0x3b2, unsigned short font = 0x03);

	/*!
		\brief Sends a localized system message to this character. The message is only sent to logged
			in players.
		\param message The id of the localized message.
		\param params Parameters to parse into the localized message. May be QString::null if no parameters
			should be parsed in.
		\param color The color of the sysmessage. The default is 0x3b2.
		\param font The font for the sysmessage. The default is 0x03.
		\returns True if the message was really sent. False otherwise.
	*/
	bool sysmessage(unsigned int message, const QString &params = QString::null, unsigned short color = 0x3b2, unsigned short font = 0x03);

	/*!
		\brief Send this character an overhead message. This does nothing for logged out characters
			or npcs.
		\param message The text of message to send to this character.
		\param color The color of the message.
		\param source The object the message should be displayed on.
		\param font The font to show this message in. Default is 0x03.
		\param mode The speech mode of the message. 0x00 is default and means regular speech.
		\returns True if the message was sent or False otherwise.
	*/
	bool message(const QString &message, 
				 unsigned short color = 0x3b2, 
				 cUObject *source = 0, 
				 unsigned short font = 0x03, 
				 unsigned char mode = 0x00);

	/*!
		\brief Sends this character a packet if he is a player and logged in.
		Does nothing for npcs and logged out characters.
		\param packet The packet you want to send to this character.
		\returns True if the packet was sent, False otherwise.
	*/
	virtual bool send(cUOPacket *packet);

	/*!
		\brief Checks if the character is available in world to take action.
		\returns True if the character is in the world or False if otherwise.
	*/
	virtual bool inWorld() = 0;

	virtual void showName( cUOSocket *socket ) = 0;	
	virtual void soundEffect( UI16 soundId, bool hearAll = true ) = 0;
	virtual void giveGold( Q_UINT32 amount, bool inBank = false ) = 0;
	virtual uint takeGold( uint amount, bool useBank = false ) = 0;
	virtual void log( eLogLevel, const QString &string ) = 0;
	virtual void log( const QString &string ) = 0;
	unsigned int damage(eDamageType type, unsigned int amount, cUObject *source = 0);	
	
	// other public methods
	// Simple Property setting and getting for script engines.
	stError *setProperty( const QString &name, const cVariant &value );
	stError *getProperty( const QString &name, cVariant &value ) const;	
	void updateHealth( void );
	void action(uchar id, uchar speed = 1, bool reverse = false); // Do an action
	P_ITEM getWeapon() const;
	P_ITEM getShield() const;
	void setHairColor( ushort d); 
	void setHairStyle( ushort d); 
	void setBeardColor( ushort d); 
	void setBeardStyle( ushort d); 
	void playDeathSound();
	double getHitpointRate();
	double getStaminaRate();
	double getManaRate();
	void resurrect();
	void turnTo( cUObject *object );
	void turnTo( const Coord_cl &pos );
	void wear( P_ITEM );
	bool isHuman() const;
	bool isMurderer() const;
	bool isCriminal() const;
	virtual bool isInnocent();
	void unhide();
	int  CountItems(short ID, short col= -1);
	int  CountGold();
	P_ITEM GetItemOnLayer(unsigned char layer);
	P_ITEM getBackpack();
	P_ITEM getBankbox();
	void SetMultiSerial(long mulser);
	void setSerial(SERIAL ser);
	void MoveTo(short newx, short newy, signed char newz);
	bool Wears(P_ITEM pi);
	unsigned int getSkillSum() const;
	void Init(bool ser = true);
	bool isSameAs(P_CHAR pc);
	bool inGuardedArea();
	void emote( const QString &emote, UI16 color = 0xFFFF );
	P_ITEM rightHandItem() const;
	P_ITEM leftHandItem() const;
	void bark( enBark );	// Play a body dependant sound
	void goldSound( unsigned short amount, bool hearall = true ); // Play a sound for dropping goldcoins depending on the amount
	void showPaperdoll( cUOSocket *source, bool hotkey );
	virtual bool checkSkill( UI16 skill, SI32 min, SI32 max, bool advance = true );
	cItem* atLayer( enLayer layer ) const;
	bool Owns( P_ITEM pi ) const;
	virtual void callGuards();
	virtual void flagUnchanged();
	virtual void awardFame( short amount ) = 0;
	virtual void awardKarma( P_CHAR pKilled, short amount ) = 0;

	// Wrapper events
	virtual bool onWalk( unsigned char dir, unsigned char sequence ); // Walks in a specific Direction
	virtual bool onTalk( unsigned char speechType, UI16 speechColor, UI16 speechFont, const QString &Text, const QString &Lang ); // The character says something
	virtual bool onWarModeToggle( bool War ); // The character switches warmode
	virtual bool onShowPaperdoll( P_CHAR pOrigin ); // The paperdoll of this character is requested, there is no vice-versa call
	virtual bool onShowSkillGump(); //Show Skillgump
	virtual bool onSkillUse( UI08 Skill ); // The character uses %Skill
	virtual bool onDeath(cUObject *source, P_ITEM corpse);
	virtual bool onDropOnChar( P_ITEM pItem );
	virtual QString onShowPaperdollName( P_CHAR pOrigin ); // only change the viewed name
//	virtual bool onShowTooltip( P_PLAYER sender, cUOTxTooltipList* tooltip ); // Shows a tool tip for specific object
	virtual bool onCHLevelChange( uint level ); // Fired when player moving trough levels
	virtual bool onSkillGain( unsigned char skill, unsigned short min, unsigned short max, bool success );

	// Combat
	inline P_CHAR attackTarget() const {
		return attackTarget_;
	}

	inline void setAttackTarget(P_CHAR target) {
		attackTarget_ = target;
	}
	
	inline unsigned int nextSwing() const {
		return nextSwing_;
	}

	inline void setNextSwing(unsigned int data) {
		nextSwing_ = data;
	}

	void refreshMaximumValues();

	// getters
    ushort			body() const;    
    QDateTime		creationDate() const;
    uint			criminalTime() const;
    ushort			deaths() const;
    short			dexterityMod() const;
    short			dexterity() const;
    ushort			emoteColor() const;
    short			fame() const;
    uchar			flag() const;
    bool			gender() const;
    P_CHAR			guarding() const;
    short			hitpoints() const;
    int				hunger() const;
    uint			hungerTime() const;
    short			intelligence() const;
    short			intelligenceMod() const;
    short			karma() const;
    ushort			kills() const;
    short			mana() const;
    ushort			maxHitpoints() const;
    ushort			maxMana() const;
    ushort			maxStamina() const;
    SERIAL			murdererSerial() const;
    uint			murdererTime() const;
    short			orgBody() const;
    QString			orgName() const;
    ushort			orgSkin() const;
    signed char poison() const;
    uint			propertyFlags() const;
	uint			regenHitpointsTime() const;
	uint			regenStaminaTime() const;
	uint			regenManaTime() const;
    cTerritory*		region() const;
    uint			runningSteps() const;
    ushort			saycolor() const;
    uint			skillDelay() const;
    ushort			skin() const;
    short			stamina() const;
    int				stealthedSteps() const;
    short			strength() const;
    short			strengthMod() const;
	unsigned int	lastMovement() const;
    QString			title() const;
    float			weight() const;
	short			hitpointsBonus() const;
	short			staminaBonus() const;
	short			manaBonus() const;
	// bit flag getters
	bool			isIncognito() const;
	bool			isPolymorphed() const;
	bool			isTamed() const;
	bool			isCasting() const;
	bool			isHidden() const;
	bool			isInvisible() const;
	bool			isMeditating() const;
	bool			isFrozen() const;
	bool			showSkillTitles() const;
	bool			isDead() const;
	bool			isAtWar() const;
	bool			isInvulnerable() const;
	// advanced getters for data structures
	// skills
	ushort			skillValue( ushort skill ) const;
	ushort			skillCap( ushort skill ) const;
	uchar			skillLock( ushort skill ) const;
	// effects
	EffectContainer	effects() const;
	// guards
	CharContainer	guardedby() const;
	// content
	ItemContainer	content() const;

	// setters
    void setBody(ushort data);
    void setCreationDate(const QDateTime &data);
    void setCriminalTime(uint data);
    void setDeaths(ushort data);
    void setDexterityMod(short data);
    void setDexterity(short data);
    void setEmoteColor(ushort data);
    void setFame(short data);
    void setFlag(uchar data);
    void setGender(bool data);
	void setGuarding(P_CHAR data);
    void setHitpoints(short data);
    void setHunger(int data);
    void setHungerTime(uint data);
    void setIntelligence(short data);
    void setIntelligenceMod(short data);
    void setKarma(short data);
    void setKills(ushort data);
    void setMana(short data);
    void setMaxHitpoints(ushort data);
    void setMaxMana(ushort data);
    void setMaxStamina(ushort data);
    void setMurdererSerial(SERIAL data);
    void setMurdererTime(uint data);
    void setOrgBody(short data);
    void setOrgName(const QString &data);
    void setOrgSkin(ushort data);
	void setLastMovement(unsigned int data);
    void setPoison(signed char data);
    void setPropertyFlags(uint data);
	void setRegenHitpointsTime(uint data);
	void setRegenStaminaTime(uint data);
	void setRegenManaTime(uint data);
    void setRegion(cTerritory* data);
    void setRunningSteps(uint data);
    void setSaycolor(ushort data);
    void setSkillDelay(uint data);
    void setSkin(ushort data);
    virtual void setStamina(short data, bool notify = true );
    void setStealthedSteps(int data);
    void setStrength(short data);
    void setStrengthMod(short data);
    void setTitle(const QString &data);
    void setWeight(float data);
	// bit flag setters
	void setIncognito(bool data);
	void setPolymorphed(bool data);
	void setTamed(bool data);
	void setCasting(bool data);
	void setHidden(bool data);
	void setInvisible(bool data);
	void setMeditating(bool data);
	void setFrozen(bool data);
	void setShowSkillTitles(bool data);
	void setDead(bool data);
	void setAtWar(bool data);
	void setInvulnerable(bool data);
	void setHitpointsBonus(short data);
	void setStaminaBonus(short data);
	void setManaBonus(short data);
	unsigned char			statCap() const;
	void setStatCap(unsigned char data);

	unsigned char strengthCap() const;
	unsigned char dexterityCap() const;
	unsigned char intelligenceCap() const;
	void setStrengthCap(unsigned char data);
	void setDexterityCap(unsigned char data);
	void setIntelligenceCap(unsigned char data);
	unsigned char statCap_;
	unsigned char strengthCap_;
	unsigned char dexterityCap_;
	unsigned char intelligenceCap_;

	// advanced setters for data structures
	// skills
	void setSkillValue( ushort skill, ushort value );
	void setSkillCap( ushort skill, ushort cap );
	void setSkillLock( ushort skill, uchar lock );
	// effects
	void addEffect( cTempEffect *effect );
	void removeEffect( cTempEffect *effect );
	// guards
	void addGuard( P_CHAR pPet, bool noGuardingChange = false );
	void removeGuard( P_CHAR pPet, bool noGuardingChange = false );
	// content
	void addItem( enLayer layer, cItem*, bool handleWeight = true, bool noRemove = false );
	void removeItem( enLayer layer, bool handleWeight = true );

	/*!
		Return a reference to the list of ongoing fights.
	*/
	QPtrList<cFightInfo> &fights() {
		return fights_;
	}
private:
	bool changed_;

protected:
	/*!
		\brief The target we are currently attacking.
	*/
	P_CHAR attackTarget_;

	/*!
		\brief This indicates the time when the character will be able to swing for the next time.
	*/
	unsigned int nextSwing_;

	/*!
		\brief Collection of information about ongoing fights.
	*/
	QPtrList<cFightInfo> fights_;

	// type definitions
	struct stSkillValue
	{
		bool changed;
		ushort value; // Skill Value (Default: 0)
		ushort cap; // Special Cap Value (Default: 1000)
		uchar lock; // 0: Up, 1: Down, 2: Locked (Default: 0)

		stSkillValue(): value(0), cap(1000), lock(0), changed(false) {}
	};

	// other protected methods
	static void buildSqlString( QStringList &fields, QStringList &tables, QStringList &conditions );
	virtual void processNode( const cElement *Tag );

    // The body ID for this character. cOldChar::id_
    ushort body_;

    // The original body id, when the char is affected by magic.
    ushort orgBody_;

	// The last time this character moved
	unsigned int lastMovement_;

    // The gender of the character. cOldChar::sex_
    bool gender_;
    
    // The original skin color hue of the char. Is needed after applying
    // magical/temporal effects which change skin color.
    // cOldChar::xskin_
    ushort orgSkin_;

    // Flag storage for magical/temporal/skill effects like incognito,
    // polymorph, ... . 
    // Bits:
    // 01 - incognito. cOldChar::incognito_
    // 02 - polymorph. cOldChar::polymorph_
    // 03 - tamed, cOldChar::tamed_
    // 04 - casting, cOldChar::casting_
    // 05 - hidden, cOldChar::hidden_, Bit 1 & 2
    // 06 - invisible, cOldChar::invisible_, Bit 3
    // 07 - dontuse
    // 08 - meditating, cOldChar::med_
    // 09 - frozen, cOldChar::priv2_, Bit 2
    // 10 - show skill titles, cOldChar::priv2, Bit 4
    // 11 - dead, cOldChar::dead
    // 12 - war, cOldChar::war
		// 13 - invulnerable, cOldChar::priv2 Bit 3
		// UPPER WORD:
		// 17 - ReactiveArmor (0x20000)
		// 18 - Protection (0x40000)
		// 19 - Magic Reflect (0x8000)
		// 20 - Mana Drain (0x100000)
    uint propertyFlags_;

    // Weight of the char, including worn items.
    float weight_;

	// Bonus values for stamina, health and mana (maximum values)
	short hitpointsBonus_;
	short staminaBonus_;
	short manaBonus_;

    // Dexterity of the char
    // cOldChar::dx
    short dexterity_;

    // dex modifier for influencing equipped items.
    // cOldChar::dx2
    short dexterityMod_;

    // maximum stamina the character can fill up
    ushort maxStamina_;

    // current stamina of the char.
    // cOldChar::stm_
    short stamina_;

    // strength of the char
    // cOldChar::st_
    short strength_;

    // temporal strength addons.
    // cOldChar::st2_
    short strengthMod_;

    // Maximum hitpoints the char can fill up to.
    ushort maxHitpoints_;

    // current hitpoints of the char.
    // cOldChar::hp_
    short hitpoints_;

    // Intelligence of the char.
    // cOldChar::in_
    short intelligence_;

    // Modifier for intelligence. cOldChar::in2_
    short intelligenceMod_;

    // Maximum mana the char can fill up.
    ushort maxMana_;

    // current mana of the char.
    // cOldChar::mn_
    short mana_;

    // Karma of the char.
    short karma_;

    // Fame of the char.
    short fame_;

    // Kills the char has made
    ushort kills_;

    // Times the char has died.
    ushort deaths_;

    // The hunger value of the char. 6 means not hungry, 0 means starving.
    // cOldChar::hunger_
    int hunger_;

    // Server clocks when next hunger check will be made.
    // cOldChar::hungertime_
    uint hungerTime_;

    // Ingame name-color flag.
    // Bits:
    // 01 - red
    // 02 - grey
    // 03 - blue
    // 04 - green
    // 05 - orange
    uchar flag_;

    // Color for emote messages.
    ushort emoteColor_;

    // Saves the date of creation. cOldChar::creationday_
    QDateTime creationDate_;

    // Saves the number of steps that were stealthed. value -1 indicates that
    // the char will be revealed
    int stealthedSteps_;

    // Saves the number of steps the char ran.
    uint runningSteps_;

    // Time, till murderer flag disappears. cOldChar::murderrate_
    uint murdererTime_;

    // Time, till criminal flag wears off. value -1 indicates not criminal!
    uint criminalTime_;

    // time till next skill usage is possible
    uint skillDelay_;

    // Poison level applied to this character. -1 for none.
    signed char poison_;

    // Title of the char.
    QString title_;

    // Original name of the char.
    // cOldChar::orgname_
    QString orgName_;

    // Skin color hue of the char.
    ushort skin_;

    // Temporal effects that affect this character.
    EffectContainer effects_;

    // Item contents of the char (i.e. equipment).
    ItemContainer content_;

    // Skill properties of this char.
    QValueVector< stSkillValue > skills_;

    // Region the char is in.
    cTerritory* region_;

    // Color hue the char speeks with.
    ushort saycolor_;

    // Serial of the last char which murdered this character.
    // cOldChar::murdererSer_
    SERIAL murdererSerial_;

    // Chars that guard this character.
    CharContainer guardedby_;

	// Time, when next hitpoint will be regenerated.
	// cOldChar::regen_
	uint regenHitpointsTime_;

	// Time, when next stamina point will be regenerated.
	// cOldChar::regen2_
	uint regenStaminaTime_;

	// Time, when next mana point will be regenerated.
	// cOldChar::regen3_
	uint regenManaTime_;

    // Char which the character guards.
    P_CHAR guarding_;
};

inline void cBaseChar::flagUnchanged()
{
	cBaseChar::changed_ = false;
	cUObject::flagUnchanged();
}

inline P_CHAR cBaseChar::guarding() const
{
    return guarding_;
}

inline void cBaseChar::setGuarding(P_CHAR data)
{
	if( data == guarding_ )
		return;

	if( guarding_ )
		guarding_->removeGuard( this );

	guarding_ = data;
	changed_ = true;
	changed( TOOLTIP );

	if( guarding_ )
		guarding_->addGuard( this );		
}

inline ushort cBaseChar::body() const
{
    return body_;
}

inline void cBaseChar::setBody(ushort data)
{
    body_ = data;

	if (data == 0x190) {
		gender_ = 0;
	} else if (data == 0x191) {
		gender_ = 1;
	}

	changed_ = true;
}

inline QDateTime cBaseChar::creationDate() const
{
    return creationDate_;
}

inline void cBaseChar::setCreationDate(const QDateTime &data)
{
    creationDate_ = data;
	changed_ = true;
}

inline uint cBaseChar::criminalTime() const
{
    return criminalTime_;
}

inline void cBaseChar::setCriminalTime(uint data)
{
    criminalTime_ = data;
	changed_ = true;
}

inline ushort cBaseChar::deaths() const
{
    return deaths_;
}

inline void cBaseChar::setDeaths(ushort data)
{
    deaths_ = data;
	changed_ = true;
}

inline short cBaseChar::dexterityMod() const
{
    return dexterityMod_;
}

inline void cBaseChar::setDexterityMod(short data)
{
    dexterityMod_ = data;
	changed_ = true;
	refreshMaximumValues();
}

inline short cBaseChar::dexterity() const
{
    return dexterity_;
}

inline void cBaseChar::setDexterity(short data)
{
    dexterity_ = data;
	changed_ = true;
	refreshMaximumValues();
}

inline ushort cBaseChar::emoteColor() const
{
    return emoteColor_;
}

inline void cBaseChar::setEmoteColor(ushort data)
{
    emoteColor_ = data;
	changed_ = true;
}

inline short cBaseChar::fame() const
{
    return fame_;
}

inline void cBaseChar::setFame(short data)
{
    fame_ = data;
	changed_ = true;
}

inline uchar cBaseChar::flag() const
{
    return flag_;
}

inline void cBaseChar::setFlag(uchar data)
{
    flag_ = data;
	changed_ = true;
}

inline bool cBaseChar::gender() const
{
    return gender_;
}

inline void cBaseChar::setGender(bool data)
{
    gender_ = data;
	changed_ = true;
}

inline short cBaseChar::hitpoints() const
{
    return hitpoints_;
}

inline void cBaseChar::setHitpoints(short data)
{
    hitpoints_ = data;
	changed_ = true;
}

inline int cBaseChar::hunger() const
{
    return hunger_;
}

inline void cBaseChar::setHunger(int data)
{
    hunger_ = data;
	changed_ = true;
}

inline uint cBaseChar::hungerTime() const
{
    return hungerTime_;
}

inline void cBaseChar::setHungerTime(uint data)
{
    hungerTime_ = data;
}

inline short cBaseChar::intelligence() const
{
    return intelligence_;
}

inline void cBaseChar::setIntelligence(short data)
{
    intelligence_ = data;
	changed_ = true;
	refreshMaximumValues();
}

inline short cBaseChar::intelligenceMod() const
{
    return intelligenceMod_;
}

inline void cBaseChar::setIntelligenceMod(short data)
{
    intelligenceMod_ = data;
	changed_ = true;
	refreshMaximumValues();
}

inline short cBaseChar::karma() const
{
    return karma_;
}

inline void cBaseChar::setKarma(short data)
{
    karma_ = data;
	changed_ = true;
}

inline ushort cBaseChar::kills() const
{
    return kills_;
}

inline void cBaseChar::setKills(ushort data)
{
    kills_ = data;
	changed_ = true;
}

inline short cBaseChar::mana() const
{
    return mana_;
}

inline void cBaseChar::setMana(short data)
{
    mana_ = data;
	changed_ = true;
}

inline ushort cBaseChar::maxHitpoints() const
{
    return maxHitpoints_;
}

inline void cBaseChar::setMaxHitpoints(ushort data)
{
    maxHitpoints_ = data;
	changed_ = true;
}

inline ushort cBaseChar::maxMana() const
{
    return maxMana_;
}

inline void cBaseChar::setMaxMana(ushort data)
{
    maxMana_ = data;
	changed_ = true;
}

inline ushort cBaseChar::maxStamina() const
{
    return maxStamina_;
}

inline void cBaseChar::setMaxStamina(ushort data)
{
    maxStamina_ = data;
	changed_ = true;
}

inline uint cBaseChar::murdererTime() const
{
    return murdererTime_;
}

inline void cBaseChar::setMurdererTime(uint data)
{
    murdererTime_ = data;
	changed_ = true;
}

inline short cBaseChar::orgBody() const
{
    return orgBody_;
}

inline void cBaseChar::setOrgBody(short data)
{
    orgBody_ = data;
	changed_ = true;
}

inline ushort cBaseChar::orgSkin() const
{
    return orgSkin_;
}

inline void cBaseChar::setOrgSkin(ushort data)
{
    orgSkin_ = data;
	changed_ = true;
}

inline signed char cBaseChar::poison() const
{
    return poison_;
}

inline void cBaseChar::setPoison(signed char data)
{
  poison_ = data;
	changed_ = true;
}

inline uint cBaseChar::propertyFlags() const
{
    return propertyFlags_;
}

inline void cBaseChar::setPropertyFlags(uint data)
{
    propertyFlags_ = data;
	changed_ = true;
}

inline uint cBaseChar::runningSteps() const
{
    return runningSteps_;
}

inline void cBaseChar::setRunningSteps(uint data)
{
    runningSteps_ = data;
}

inline uint cBaseChar::skillDelay() const
{
    return skillDelay_;
}

inline void cBaseChar::setSkillDelay(uint data)
{
    skillDelay_ = data;
}

inline short cBaseChar::stamina() const
{
    return stamina_;
}

inline int cBaseChar::stealthedSteps() const
{
    return stealthedSteps_;
}

inline void cBaseChar::setStealthedSteps(int data)
{
    stealthedSteps_ = data;
}

inline short cBaseChar::strength() const
{
    return strength_;
}

inline void cBaseChar::setStrength(short data)
{
    strength_ = data;
	changed_ = true;
	refreshMaximumValues();
}

inline short cBaseChar::strengthMod() const
{
    return strengthMod_;
}

inline void cBaseChar::setStrengthMod(short data)
{
    strengthMod_ = data;
	changed_ = true;
	refreshMaximumValues();
}

inline float cBaseChar::weight() const
{
    return ( ceilf( weight_ * 100 ) / 100 );
}

inline void cBaseChar::setWeight(float data)
{
	weight_ = ceilf( data * 100 ) / 100;
}

inline SERIAL cBaseChar::murdererSerial() const
{
    return murdererSerial_;
}

inline void cBaseChar::setMurdererSerial(SERIAL data)
{
    murdererSerial_ = data;
	changed_ = true;
}

inline QString cBaseChar::orgName() const
{
    return orgName_;
}

inline void cBaseChar::setOrgName(const QString &data)
{
    orgName_ = data;
	changed_ = true;
}

inline cTerritory* cBaseChar::region() const
{
    return region_;
}

inline void cBaseChar::setRegion(cTerritory* data)
{
    region_ = data;
}

inline ushort cBaseChar::saycolor() const
{
    return saycolor_;
}

inline void cBaseChar::setSaycolor(ushort data)
{
    saycolor_ = data;
	changed_ = true;
}

inline ushort cBaseChar::skin() const
{
    return skin_;
}

inline void cBaseChar::setSkin(ushort data)
{
    skin_ = data;
	changed_ = true;
}

inline QString cBaseChar::title() const
{
    return title_;
}

inline void cBaseChar::setTitle(const QString &data)
{
    title_ = data;
	changed_ = true;
}

inline uint cBaseChar::regenHitpointsTime() const
{
    return regenHitpointsTime_;
}

inline void cBaseChar::setRegenHitpointsTime(uint data)
{
    regenHitpointsTime_ = data;
}

inline uint cBaseChar::regenStaminaTime() const
{
    return regenStaminaTime_;
}

inline void cBaseChar::setRegenStaminaTime(uint data)
{
    regenStaminaTime_ = data;
}

inline uint cBaseChar::regenManaTime() const
{
    return regenManaTime_;
}

inline void cBaseChar::setRegenManaTime(uint data)
{
    regenManaTime_ = data;
}

inline bool cBaseChar::isIncognito() const
{
	return propertyFlags_ & 0x0001;
}

inline bool cBaseChar::isPolymorphed() const
{
	return propertyFlags_ & 0x0002;
}

inline bool cBaseChar::isTamed() const
{
	return propertyFlags_ & 0x0004;
}

inline bool cBaseChar::isCasting() const
{
	return propertyFlags_ & 0x0008;
}

inline bool cBaseChar::isHidden() const
{
	return propertyFlags_ & 0x0010;
}

inline bool cBaseChar::isInvisible() const
{
	return propertyFlags_ & 0x0020;
}

inline bool cBaseChar::isMeditating() const
{
	return propertyFlags_ & 0x0080;
}

inline bool cBaseChar::isFrozen() const
{
	return propertyFlags_ & 0x0100;
}

inline bool cBaseChar::showSkillTitles() const
{
	return propertyFlags_ & 0x0200;
}

inline bool cBaseChar::isDead() const
{
	return propertyFlags_ & 0x0400;
}

inline bool cBaseChar::isAtWar() const
{
	return propertyFlags_ & 0x0800;
}

inline bool cBaseChar::isInvulnerable() const
{
	return propertyFlags_ & 0x1000;
}

inline void cBaseChar::setIncognito(bool data)
{
	if( data ) propertyFlags_ |= 0x0001; else propertyFlags_ &= ~0x0001; 
	changed_ = true;
}

inline void cBaseChar::setPolymorphed(bool data)
{
	if( data ) propertyFlags_ |= 0x0002; else propertyFlags_ &= ~0x0002; 
	changed_ = true;
}

inline void cBaseChar::setTamed(bool data)
{
	if( data ) propertyFlags_ |= 0x0004; else propertyFlags_ &= ~0x0004; 
	changed_ = true;
}

inline void cBaseChar::setCasting(bool data)
{
	if( data ) propertyFlags_ |= 0x0008; else propertyFlags_ &= ~0x0008; 
	changed_ = true;
}

inline void cBaseChar::setHidden(bool data)
{
	if( data ) propertyFlags_ |= 0x0010; else propertyFlags_ &= ~0x0010; 
	changed_ = true;
}

inline void cBaseChar::setInvisible(bool data)
{
	if( data ) propertyFlags_ |= 0x0020; else propertyFlags_ &= ~0x0020; 
	changed_ = true;
}

inline void cBaseChar::setMeditating(bool data)
{
	if( data ) propertyFlags_ |= 0x0080; else propertyFlags_ &= ~0x0080; 
	changed_ = true;
}

inline void cBaseChar::setFrozen(bool data)
{
	if( data ) propertyFlags_ |= 0x0100; else propertyFlags_ &= ~0x0100; 
	changed_ = true;
}

inline void cBaseChar::setShowSkillTitles(bool data)
{
	if( data ) propertyFlags_ |= 0x0200; else propertyFlags_ &= ~0x0200; 
	changed_ = true;
}

inline void cBaseChar::setDead(bool data)
{
	if( data ) propertyFlags_ |= 0x0400; else propertyFlags_ &= ~0x0400; 
	changed_ = true;
}

inline void cBaseChar::setAtWar(bool data)
{
	if( data ) propertyFlags_ |= 0x0800; else propertyFlags_ &= ~0x0800; 
	changed_ = true;
}

inline void cBaseChar::setInvulnerable(bool data)
{
	if( data ) propertyFlags_ |= 0x1000; else propertyFlags_ &= ~0x1000; 
	changed_ = true;
}

inline cBaseChar::CharContainer cBaseChar::guardedby() const
{
	return guardedby_;
}

inline cBaseChar::ItemContainer cBaseChar::content() const
{
	return content_;
}

inline bool cBaseChar::isHuman() const 
{ 
	return (body_ == 0x190 || body_ == 0x191); 
}

inline bool cBaseChar::isInnocent()
{
	return !isMurderer() && !isCriminal();
}

inline bool cBaseChar::isSameAs(P_CHAR pc)
{
	return ( pc && pc->serial() == serial() );
}

inline P_ITEM cBaseChar::rightHandItem() const
{
	return atLayer( SingleHandedWeapon );
}

inline P_ITEM cBaseChar::leftHandItem() const
{
	return atLayer( DualHandedWeapon );
}

inline cBaseChar::EffectContainer cBaseChar::effects() const
{
	return effects_;
}

inline unsigned int cBaseChar::lastMovement() const {
	return lastMovement_;
}

inline void cBaseChar::setLastMovement(unsigned int data) {
	lastMovement_ = data;
}

inline short cBaseChar::hitpointsBonus() const {
	return hitpointsBonus_;
}

inline short cBaseChar::manaBonus() const {
	return manaBonus_;
}

inline short cBaseChar::staminaBonus() const {
	return staminaBonus_;
}

inline void cBaseChar::setHitpointsBonus(short data) {
	hitpointsBonus_ = data;
	refreshMaximumValues();
	changed_ = true;
}

inline void cBaseChar::setManaBonus(short data) {
	manaBonus_ = data;
	refreshMaximumValues();
	changed_ = true;
}

inline void cBaseChar::setStaminaBonus(short data) {
	staminaBonus_ = data;
	refreshMaximumValues();
	changed_ = true;
}

inline unsigned char cBaseChar::strengthCap() const {
	return strengthCap_;
}

inline unsigned char cBaseChar::dexterityCap() const {
	return dexterityCap_;
}

inline unsigned char cBaseChar::intelligenceCap() const {
	return intelligenceCap_;
}

inline void cBaseChar::setStrengthCap(unsigned char data) {
	strengthCap_ = data;
	changed_ = true;
}

inline void cBaseChar::setDexterityCap(unsigned char data) {
	dexterityCap_ = data;
	changed_ = true;
}

inline void cBaseChar::setIntelligenceCap(unsigned char data) {
	intelligenceCap_ = data;
	changed_ = true;
}

inline void cBaseChar::setStatCap(unsigned char data) {
	statCap_ = data;
	changed_ = true;
}

inline unsigned char cBaseChar::statCap() const {
	return statCap_;
}

#endif /* CBASECHAR_H_HEADER_INCLUDED */
