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

#if !defined( __ITEMS_H )
#define __ITEMS_H

// Wolfpack Includes
#include "uobject.h"
#include "defines.h"
#include "network/uotxpackets.h"

// Library Includes
#include <qvaluevector.h>

// Forward Class declarations
class ISerialization;
class cUOSocket;


class cItem : public cUObject
{
	Q_OBJECT
	Q_PROPERTY ( ushort		id			READ id				WRITE setId				)
	Q_PROPERTY ( ushort		color		READ color			WRITE setColor			)
	Q_PROPERTY ( ushort		amount		READ amount			WRITE setAmount			)
	Q_PROPERTY ( ushort		restock		READ restock		WRITE setRestock		)
	Q_OVERRIDE ( QString	name		READ name			WRITE setName			)
	Q_PROPERTY ( uchar		layer		READ layer			WRITE setLayer			)
	Q_PROPERTY ( QString	murderer	READ murderer		WRITE setMurderer		)
	Q_PROPERTY ( QString	spawnregion	READ spawnregion	WRITE setSpawnRegion	)
	Q_PROPERTY ( int		totalweight READ totalweight	WRITE setTotalweight	)
	Q_PROPERTY ( QString	carve		READ carve			WRITE setCarve			)
	Q_PROPERTY ( ushort		accuracy	READ accuracy		WRITE setAccuracy		)
	Q_PROPERTY ( int		sellprice	READ sellprice		WRITE setSellprice		)
	Q_PROPERTY ( int		buyprice	READ buyprice		WRITE setBuyprice		)
	Q_PROPERTY ( int		price		READ price			WRITE setPrice			)
	Q_PROPERTY ( uchar		moreb1		READ moreb1			WRITE setMoreb1			)
	Q_PROPERTY ( uchar		moreb2		READ moreb2			WRITE setMoreb2			)
	Q_PROPERTY ( uchar		moreb3		READ moreb3			WRITE setMoreb3			)
	Q_PROPERTY ( uchar		moreb4		READ moreb4			WRITE setMoreb4			)
	Q_PROPERTY ( uchar		more1		READ more1			WRITE setMore1			)
	Q_PROPERTY ( uchar		more2		READ more2			WRITE setMore2			)
	Q_PROPERTY ( uchar		more3		READ more3			WRITE setMore3			)
	Q_PROPERTY ( uchar		more4		READ more4			WRITE setMore4			)
	Q_PROPERTY ( uchar		morex		READ morex			WRITE setMoreX			)
	Q_PROPERTY ( uchar		morey		READ morey			WRITE setMoreY			)
	Q_PROPERTY ( uchar		morez		READ morez			WRITE setMoreZ			)
	Q_PROPERTY ( uchar		doordir		READ doordir		WRITE setDoorDir		)
	Q_PROPERTY ( uchar		dooropen	READ dooropen		WRITE setDoorOpen		)
	Q_PROPERTY ( uchar		dye			READ dye			WRITE setDye			)
	Q_PROPERTY ( uint		att			READ att			WRITE setAtt			)
	Q_PROPERTY ( uint		def			READ def			WRITE setDef			)
	Q_PROPERTY ( short		st			READ st				WRITE setSt				)
	Q_PROPERTY ( short		st2			READ st2			WRITE setSt2			)
	Q_PROPERTY ( short		dx			READ dx				WRITE setDx				)
	Q_PROPERTY ( short		dx2			READ dx2			WRITE setDx2			)
	Q_PROPERTY ( short		in			READ in				WRITE setIn				)
	Q_PROPERTY ( short		in2			READ in2			WRITE setIn2			)
	Q_PROPERTY ( uchar		magic		READ magic			WRITE setMagic			)
	Q_PROPERTY ( uint		gatetime	READ gatetime		WRITE setGateTime		)
	Q_PROPERTY ( int		gatenumber	READ gatenumber		WRITE setGateNumber		)
	Q_PROPERTY ( uint		decaytime	READ decaytime		WRITE setDecayTime		)
	Q_PROPERTY ( uint		disabled	READ disabled		WRITE setDisabled		)
	Q_PROPERTY ( uint		poisoned	READ poisoned		WRITE setPoisoned		)
	Q_PROPERTY ( uint		murdertime	READ murdertime		WRITE setMurderTime		)
	Q_PROPERTY ( int		rank		READ rank			WRITE setRank			)
	Q_PROPERTY ( uchar		direction	READ direction		WRITE setDirection		)
	Q_PROPERTY ( QString	description	READ description	WRITE setDescription	)
	Q_PROPERTY ( QString	creator		READ creator		WRITE setCreator		)
	Q_PROPERTY ( int		ownserial	READ ownSerial		WRITE SetOwnSerial		)
	Q_PROPERTY ( uchar		visible		READ visible		WRITE setVisible		)
	Q_PROPERTY ( uchar		priv		READ priv			WRITE setPriv			)
	Q_PROPERTY ( int		good		READ good			WRITE setGood			)
	Q_PROPERTY ( int		rndvaluerate READ rndvaluerate	WRITE setRndValueRate	)
	Q_PROPERTY ( uchar		madewith	READ madewith		WRITE setMadeWith		)

//	friend class cChar; // temporary
public:
	typedef QValueVector<cItem*> ContainerContent;
public:

	virtual void	talk( const QString &message, ushort color = 0xFFFF, UINT8 type = 0, bool autospam = false, cUOSocket* socket = NULL );
	void load( char **, UINT16& );
	void save();
	bool del();

	void	processContainerNode( const QDomElement &Tag );
	virtual void update( cUOSocket *mSock = NULL );
	P_ITEM	dupe();
	void	soundEffect( UINT16 sound );

	// Getters
	ushort			id()			const { return id_; }			// The graphical id of the item
	ushort			color()			const { return color_; }		// The Color of the item
	ushort			amount()		const { return amount_; }		// Amount of items in pile
	ushort			restock()		const { return restock_; }		// Amount of items a vendor will respawn this item to.
	ushort			amount2()		const { return amount2_; }		// Used to track things like number of yards left in a roll of cloth
	const QString	&name2()		const { return name2_; }		// The identified name of the item
	const QString	&name()			const { return name_; }			// The identified name of the item
	uchar			layer()			const { return layer_; }		// Layer if equipped on paperdoll
	bool			twohanded()		const { return priv_&0x20; }		// Is the weapon twohanded ?
	const QString	&murderer()		const { return murderer_; }		// If it's a corpse, this holds the name of the murderer
	UI32			type()			const { return type_; }			// Used for hardcoded behaviour
	UI32			type2()			const { return type2_; }
	uchar			offspell()		const { return offspell_; } 
	bool			secured()		const { return priv_&0x08; }		// Is the container secured (houses)
	SI16			speed()			const { return speed_; }		// Weapon speed
	SI16			lodamage()		const { return lodamage_; }		// Minimum damage weapon inflicts
	SI16			hidamage()		const { return hidamage_; }		// Maximum damage weapon inflicts
	bool			wipe()			const { return priv_&0x10; }		// Should the item be wiped when affected by /WIPE
	SI16			weight()		const { return weight_; }
	SI16			stones()		const { return (SI16)( weight_ / 10 ); } // Weight transformed to UO Stones
	SI16			hp()			const { return hp_; }			// Number of hitpoints an item has
	SI16			maxhp()			const { return maxhp_; }		// Maximum number of hitpoints an item has
	QString			spawnregion()	const { return spawnregion_; }
	uchar			moreb1()		const { return moreb1_; }
	uchar			moreb2()		const { return moreb2_; }
	uchar			moreb3()		const { return moreb3_; }
	uchar			moreb4()		const { return moreb4_; }
	bool			corpse()		const { return priv_&0x40; }		// Is the item a corpse
	bool			newbie()		const { return priv_&0x02; }		// Is the Item Newbie
	P_CHAR			owner() const;
	int				totalweight()	const { return totalweight_; }
	QString			carve()			const { return carve_; }
	uint			antispamtimer() const { return antispamtimer_;}
	ushort			accuracy()		const { return accuracy_; }		// for weapons, could be used for certain tools too.
	cUObject		*container()    const { return container_; }
	int				sellprice()		const { return sellprice_; } // Price this item is being bought at by normal vendors
	int				buyprice()		const { return buyprice_; } // Price this item is being sold at by normal vendors
	int				price()			const { return price_; } // Price this item is being sold at by player vendors

	uchar			more1()			const { return more1_; }
	uchar			more2()			const { return more2_; }
	uchar			more3()			const { return more3_; }
	uchar			more4()			const { return more4_; }
	uint			morex()			const { return morex_; }
	uint			morey()			const { return morey_; }
	uint			morez()			const { return morez_; }	
	uchar			doordir()		const { return doordir_; }
	uchar			dooropen()		const { return dooropen_; }
	uchar			dye()			const { return dye_; }
	uint			att()			const { return att_; }
	uint			def()			const { return def_; }
	short			st()			const { return st_; }
	short			st2()			const { return st2_; }
	short			dx()			const { return dx_; }
	short			dx2()			const { return dx2_; }
	short			in()			const { return in_; }
	short			in2()			const { return in2_; }
	uchar			magic()			const { return magic_; }
	uint			gatetime()		const { return gatetime_; }
	int				gatenumber()	const { return gatenumber_; }
	uint			decaytime()		const { return decaytime_; }
	uint			disabled()		const { return disabled_; } 
	uint			poisoned()		const { return poisoned_; }
	uint			murdertime()	const { return murdertime_; } 
	int				rank()			const { return rank_; } 
	uchar			direction()		const { return dir_;  }
	QString			description()	const { return desc_; }
	QString			creator()		const { return creator_;}
	uchar			visible()		const { return visible_;}
	uchar			priv()			const { return priv_;	}
	int				good()			const { return good_;	}
	int				rndvaluerate()  const { return rndvaluerate_; }
	int				madewith()		const { return madewith_;	}

//***************************END ADDED GETTERS************


	// Setters
	void	setId( ushort nValue ) { id_ = nValue; changed( SAVE );};
	void	setColor( ushort nValue ) { color_ = nValue; changed( SAVE );};
	void	setAmount( ushort nValue );
	void	setRestock( ushort nValue ) { restock_ = nValue; changed( SAVE );}
	void	setAmount2( ushort nValue ) { amount2_ = nValue; changed( SAVE+TOOLTIP );}; //Used to track things like number of yards left in a roll of cloth
	void	setName( const QString& nValue ) { name_ = nValue; changed( SAVE+TOOLTIP );};
	void	setName2( const QString& nValue ) { name2_ = nValue; changed( SAVE+TOOLTIP );};
	void	setLayer( uchar nValue ) { layer_ = nValue; changed( SAVE );};
	void	setTwohanded( bool nValue ) { nValue ? priv_ &= 0x20 : priv_ |= 0xDF; changed( SAVE+TOOLTIP );};
	void	setMurderer( const QString& nValue ) { murderer_ = nValue; changed( SAVE );};
	void	setType( UI32 nValue ) { type_ = nValue; changed( SAVE );};
	void	setType2( UI32 nValue ) { type2_ = nValue; changed( SAVE );};	
	void	setOffspell( uchar nValue ) { offspell_ = nValue; changed( SAVE );};
	void	setSecured( bool nValue ) { ( nValue ) ? priv_ &= 0x08 : priv_ |= 0xF7; changed( SAVE+TOOLTIP );};
	void	setSpeed( SI16 nValue ) { speed_ = nValue; changed( SAVE+TOOLTIP );};
	void	setHidamage( SI16 nValue ) { hidamage_ = nValue; changed( SAVE+TOOLTIP );};
	void	setLodamage( SI16 nValue ) { lodamage_ = nValue; changed( SAVE+TOOLTIP );};
	void	setWipe( bool nValue ) { ( nValue ) ? priv_ &= 0x10 : priv_ |= 0xEF; changed( SAVE );};
	void	setWeight( SI16 nValue );
	void	setHp( SI16 nValue ) { hp_ = nValue; changed( SAVE+TOOLTIP );};
	void	setMaxhp( SI16 nValue ) { maxhp_ = nValue; changed( SAVE+TOOLTIP );};
	void	setSpawnRegion( const QString& nValue ) { spawnregion_ = nValue; changed( SAVE );};
	void	setMoreb1( uchar nValue ) { moreb1_ = nValue; changed( SAVE );};
	void	setMoreb2( uchar nValue ) { moreb2_ = nValue; changed( SAVE );};
	void	setMoreb3( uchar nValue ) { moreb3_ = nValue; changed( SAVE );};
	void	setMoreb4( uchar nValue ) { moreb4_ = nValue; changed( SAVE );};
	void	setCorpse( bool nValue ) { ( nValue ) ? priv_ |= 0x40 : priv_ &= 0xBF; changed( SAVE+TOOLTIP );}
	void	setNewbie( bool nValue ) { ( nValue ) ? priv_ |= 0x02 : priv_ &= 0xFD; changed( SAVE+TOOLTIP );}
	void	setOwner( P_CHAR nOwner );
	void	setTotalweight( int data );
	void	setCarve( const QString& data ) { carve_ = data; changed( SAVE );}
	void	setAntispamtimer ( uint data ) { antispamtimer_ = data; changed( SAVE );}
	void	setAccuracy( ushort data ) { accuracy_ = data; changed( SAVE );}
	void	setDirection( uchar d )	 { dir_ = d; changed( SAVE );}
	void	setDescription( const QString& d ) { desc_ = d; changed( SAVE+TOOLTIP );}
	void	setCreator( const QString& d )	{ creator_ = d;	changed( SAVE+TOOLTIP );}

	cItem();
	cItem( const cItem& src); // Copy constructor
	static void registerInFactory();

	bool wearOut(); // The item wears out and true is returned if it's destroyed
	P_ITEM	getCorpse( void ); // Get the corpse this item is in
	void	toBackpack( P_CHAR pChar );
	void	showName( cUOSocket *socket );
	void	applyRank( uchar rank );
//*****************************************ADDED SETTERS ***************
	void	setMore1( uchar data ) { more1_ = data; changed( SAVE );}
	void	setMore2( uchar data ) { more2_ = data; changed( SAVE );}
	void	setMore3( uchar data ) { more3_ = data; changed( SAVE );}
	void	setMore4( uchar data ) { more4_ = data; changed( SAVE );}
	void	setMoreX( uint data ) { morex_ = data; changed( SAVE );}
	void	setMoreY( uint data ) { morey_ = data; changed( SAVE );}
	void	setMoreZ( uint data ) { morez_ = data; changed( SAVE );}
	void	setDoorDir( uchar data ) { doordir_ = data; changed( SAVE );}
	void	setDoorOpen( uchar data ) { dooropen_ = data; changed( SAVE );}
	void	setDye( uchar data ) { dye_ = data; changed( SAVE );}
	void	setAtt(	uint data ) { att_ = data; changed( SAVE );}
	void	setDef( uint data ) { def_ = data; changed( SAVE+TOOLTIP );}
	void	setSt( short data ) { st_ = data; changed( SAVE+TOOLTIP );}
	void	setSt2( short data ) { st2_ = data; changed( SAVE+TOOLTIP );}
	void	setDx( short data ) { dx_ = data; changed( SAVE+TOOLTIP );}
	void	setDx2( short data ) { dx2_ = data; changed( SAVE+TOOLTIP );}
	void	setIn( short data ) { in_ = data; changed( SAVE+TOOLTIP );}
	void	setIn2( short data ) { in2_ = data; changed( SAVE+TOOLTIP );}
	void	setMagic( uchar data ) { magic_ = data; changed( SAVE+TOOLTIP );}
	void	setGateTime( uint data ) { gatetime_ = data; changed( SAVE );}
	void	setGateNumber( int data ) { gatenumber_ = data; changed( SAVE );}
	void	setDecayTime( uint data ) { decaytime_ = data; changed( SAVE );}
	void	setPrice( int data ) { price_ = data; changed( SAVE+TOOLTIP );}
	void	setBuyprice( int data ) { buyprice_ = data; changed( SAVE+TOOLTIP );}
	void	setSellprice( int data ) { sellprice_ = data; changed( SAVE+TOOLTIP );}

	void	setDisabled(uint data) { disabled_ = data; changed( SAVE );}
	void	setPoisoned(uint data) { poisoned_ = data; changed( SAVE );}
	void	setMurderTime(uint data) { murdertime_ = data; changed( SAVE );}
	void	setRank(int data) { rank_ = data; changed( SAVE );} 
	void	setVisible( uchar d ) { visible_ = d; changed( SAVE );}
	void	setPriv( uchar d ) { priv_ = d; changed( SAVE+TOOLTIP );}
	void	setGood( int d ) { good_ = d;	changed( SAVE );}
	void	setRndValueRate( int d ) { rndvaluerate_ = d; changed( SAVE );}
	void	setMadeWith( int d )	{ madewith_ = d; changed( SAVE+TOOLTIP );}
	void	setContainer( cUObject* d ) { container_ = d; changed( SAVE ); }

//*******************************************END ADDED SETTERS**********

	SERIAL spawnserial;

	// Bit | Hex | Description
	//===================
	//   0 |  01 | Decay
	//   1 |  02 | Newbie
	//   2 |  04 | Dispellable
	//   3 |  08 | Secured (Chests)
	//   4 |  10 | Wipeable (/WIPE affects the item)
	//   5 |  20 | Twohanded
	//   6 |  40 | Corpse
	//   7 |  80 | <unused>
	
	bool incognito; //AntiChrist - for items under incognito effect
	// ^^ NUTS !! - move that to priv

	uint time_unused;     // LB -> used for house decay and possibly for more in future, gets saved
	uint timeused_last; // helper attribute for time_unused, doesnt get saved
	
	virtual void Init( bool mkser = true );
	void setSerial(SERIAL ser);
	bool isInWorld()			{ return (!container_); }
	bool isMulti()				{ return ( id_ >= 0x4000 ); }
	bool isPileable();
	
	void setOwnSerialOnly(int ownser);
	void SetOwnSerial(int ownser);
	int ownSerial() const			{return ownserial_;}
	
	void SetSpawnSerial(long spawnser);
	void SetMultiSerial(long mulser);
	
	bool isShield() { return type_ == 1009; }
	UINT16 getWeaponSkill();

	void MoveTo(int newx, int newy, signed char newz);
	long ReduceAmount(const short amount = 1);
	short GetContGumpType();
	void SetRandPosInCont(cItem* pCont);
	bool PileItem(cItem* pItem);
	bool ContainerPileItem(cItem* pItem);	// try to find an item in the container to stack with
	void addItem(cItem* pItem, bool randomPos = true, bool handleWeight = true, bool noRemove = false ); // Add Item to container
	void removeItem(cItem*, bool handleWeight = true );
	void removeFromCont( bool handleWeight = true );
	ContainerContent content() const;
	bool contains( const cItem* ) const;
	int  CountItems(short ID, short col= -1) const;
	int  DeleteAmount(int amount, ushort _id, ushort _color = 0);
	QString getName( bool shortName = false );
	void startDecay();
	void setAllMovable()		{this->magic_=1; changed( SAVE );} // set it all movable..
	bool isAllMovable()         {return (magic_==1);}
	void setGMMovable()		    {this->magic_=2; changed( SAVE );} // set it GM movable.
	bool isGMMovable()          {return (magic_==2);}
	void setOwnerMovable()		{this->magic_=3; changed( SAVE );} // set it owner movable.
	bool isOwnerMovable()       {return (magic_==3);}
	void setLockedDown()        {this->magic_=4; changed( SAVE );} // set it locked down.
	bool isLockedDown()			{return (magic_==4);}

	// Public event wrappers added by darkstorm
	virtual bool onSingleClick( P_CHAR Viewer );
	bool onDropOnChar( P_CHAR pChar );
	bool onDropOnItem( P_ITEM pItem );
	bool onDropOnGround( const Coord_cl &pos );
	bool onPickup( P_CHAR pChar );
	bool onShowTooltip( P_CHAR sender, cUOTxTooltipList* tooltip ); // Shows a tool tip for specific object
	
	QPtrList< cItem > getContainment() const;

	P_ITEM getOutmostItem();
	P_CHAR getOutmostChar();

	stError *setProperty( const QString &name, const cVariant &value );
	stError *getProperty( const QString &name, cVariant &value ) const;

////
	void flagUnchanged() { changed_ = false; cUObject::flagUnchanged(); }

protected:
	// Methods
	static void buildSqlString( QStringList &fields, QStringList &tables, QStringList &conditions );
	virtual void	processNode( const QDomElement &Tag );
	void	processModifierNode( const QDomElement &Tag );

	// Data
	ushort		id_;
	ushort		color_;
	ushort		amount_; 
	ushort		restock_;
	ushort		amount2_; 
	QString		name_;
	QString		name2_;
	uchar		layer_;
	QString		murderer_;
	SI16		lodamage_; 
	SI16		hidamage_; 
	ushort		type_;
	ushort		type2_;
	uchar		offspell_; // Whats that for ?!
	SI16		speed_;
	SI16		weight_;
	SI16		hp_;
	SI16		maxhp_;
	QString		spawnregion_;
	int			totalweight_;
	QString		carve_;
	uint		antispamtimer_;
	ushort		accuracy_;	// for weapons, could be used for certain tools too.
	int			sellprice_;
	int			buyprice_;
	int			price_; // This price is only used for player vendor items
	
	//Charges
	ushort		chrg_count_;	//Charges count
	ushort		chrg_spell_;	//Charge spell

	//Damage %
	SI08		dmg_increase_;
	SI08		dmg_physical_;
	SI08		dmg_cold_;
	SI08		dmg_fire_;
	SI08		dmg_poison_;
	SI08		dmg_energy_;
	SI08		dmg_spell_;		//Spell damage increase

	//Other modifiers
	SI08		dfn_chance_;	//Defence chance increase %
	short		dxt_bonus_;		//Dexterity bonus
	short		int_bonus_;		//Intelligence bonus
	short		str_bonus_;		//Strength bonus

	SI08		gld_increase_;	//Gold increase %
	short		mana_increase_;	//Mana increase
	short		stam_increase_; //Stamina increase
	SI08		swing_increase_;//Swing speed increase %

	SI08		nhn_potions_;	//Enhance potions %
	short		cst_recovery_;	//Faster cast recovery
	short		cst_speed_;		//Faster casting
	SI08		self_repair_;	//Self repair
	
	//Hit modifiers
	SI08		hit_chance_;	//Hit chance increase %
	SI08		hit_cold_;		//Hit cold %
	SI08		hit_dispel_;	//Hit dispel %
	SI08		hit_energy_;	//Hit energy %
	SI08		hit_fire_;		//Hit fire %
	SI08		hit_fireball_;	//Hit fireball %
	SI08		hit_harm_;		//Hit harm %
	SI08		hit_lifeleech_;	//Hit life leech %
	SI08		hit_lighting_;	//Hit lighting %
	SI08		hit_lattack_;	//Hit lower atack %
	SI08		hit_ldefence_;	//Hit lower defence %
	SI08		hit_marrow_;	//Hit magic arrow %
	SI08		hit_manaleech_;	//Hit mana leech %
	SI08		hit_physical_;	//Hit physical area %
	SI08		hit_poisoon_;	//Hit poison area %
	SI08		hit_stamleech_;	//Hit stamina leech %
	short		hit_point_;		//Hit point increase (amount)

	//Lower requirements for resources and stats consumption
	SI08		low_mana_;		//Low mana cost %
	SI08		low_reagent_;	//Low reagent cost %
	SI08		low_global_;	//Low requirements %
	
	//Durability
	ushort		drb_base_;		//Durability base
	ushort		drb_current_;   //Durability current
	ushort		uss_base_;		//Uses base
	ushort		uss_current_;	//Uses current


	//Regenerations
	ushort		mana_regen_;	//Mana regeneration
	ushort		stam_regen_;	//Stamina regeneration
	ushort		hit_regen_;		//Hit point regeneration


	//Reflecting and resisting
	SI08		rfl_physical_;	//Reflect physical %
	SI08		rss_cold_;		//Cold resist %
	SI08		rss_energy_;	//Energy resist %
	SI08		rss_fire_;		//Fire resist %
	SI08		rss_physical_;	//Physical resist %
	SI08		rss_poison_;	//Poison resist %

	// More values
	uchar		moreb1_;
	uchar		moreb2_;
	uchar		moreb3_;
	uchar		moreb4_;
	ContainerContent content_;
	cUObject*	container_;
	uchar		more1_; // For various stuff
	uchar		more2_;
	uchar		more3_;
	uchar		more4_;
	uint		morex_;
	uint		morey_;
	uint		morez_;
	uchar		doordir_; // Reserved for doors
	uchar		dooropen_;
	uchar		dye_; // Reserved: Can item be dyed by dye kit
	uint		att_; // Item attack
	uint		def_; // Item defense
	short		st_; // The strength needed to equip the item
	short		st2_; // The strength the item gives
	short		dx_; // The dexterity needed to equip the item
	short		dx2_; // The dexterity the item gives
	short		in_; // The intelligence needed to equip the item
	short		in2_; // The intelligence the item gives
	uchar		magic_; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable, 4=Locked Down
	uint		gatetime_;
	int			gatenumber_;
	uint		decaytime_;
	uint		disabled_; //Item is disabled, cant trigger.
	uint		poisoned_; //AntiChrist -- for poisoning skill
	uint		murdertime_; //AntiChrist -- for corpse -- when the people has been killed
	int			rank_; //Magius(CHE) --- for rank system, this value is the LEVEL of the item from 1 to 10. Simply multiply the rank*10 and calculate the MALUS this item has from the original.
	// for example: RANK 5 ---> 5*10=50% of malus
	//   this item has same values decreased by 50%..
	// RANK 1 ---> 1*10=10% this item has 90% of malus!
	// RANK 10 --> 10*10=100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.
	// Vars: LODAMAGE,HIDAMAGE,ATT,DEF,HP,MAXHP
	uchar		dir_;
	QString		desc_;
	QString		creator_; // Store the name of the player made this item
	SERIAL		ownserial_;
	uchar		visible_; // 0=Normally Visible, 1=Owner & GM Visible, 2=GM Visible
	uchar		priv_;
	int			good_; // Store type of GOODs to trade system! (Plz not set as UNSIGNED)  --- Magius(CHE)
	int			rndvaluerate_; // Store the value calculated base on RANDOMVALUE in region.scp. ---- MAgius(CHE) (2)
	uchar		madewith_; // Store the skills used to make this item -- Magius(CHE)
private:
	bool changed_;
};


//forward declaration
class QDomElement;

class cAllItems
{
public:
	// Added by DarkStorm
	P_ITEM createScriptItem( const QString& Section ); // Creates an item from an item-section
	P_ITEM createListItem( QString Section ); // Creates an Item from an item-list

	void DeleItem(P_ITEM pi);
	char isFieldSpellItem(P_ITEM pi);
	P_ITEM SpawnItem(P_CHAR pc_ch,int nAmount, const char* cName, bool pileable, short id, short color, bool bPack);
	P_ITEM SpawnItemBank(P_CHAR pc_ch, QString nItem);
	P_ITEM  SpawnItemBackpack2(UOXSOCKET s, QString nItem, int nDigging);
	void DecayItem(uint currenttime, P_ITEM pi);
	void RespawnItem(uint Currenttime, P_ITEM pi);
	void AddRespawnItem(P_ITEM pItem, QString itemSect, bool spawnInItem);
	void CheckEquipment(P_CHAR pc_p); //AntiChrist
	void GetScriptItemSetting(P_ITEM pi); // by Magius(CHE)
};

#endif
