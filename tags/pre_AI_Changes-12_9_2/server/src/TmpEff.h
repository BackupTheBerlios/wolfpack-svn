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

// TmpEff.h: interface for the TmpEff class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__TMPEFF_H__)
#define __TMPEFF_H__ 

//Platform specifics
#include "platform.h"

//System includes
#include <vector>
#include <functional>
#include <algorithm>

//Forward class declarations
class cTempEffect;
class cScriptEffect;
class cTimedSpellAction;
class cTempEffects;

// Wolfpack includes
#include "typedefs.h"
#include "iserialization.h"
#include "singleton.h"

class cTempEffect : public cSerializable
{
	Q_OBJECT
protected:
	SERIAL		sourSer;
	SERIAL		destSer;
	QString		objectid;
	bool		serializable;

public:
	unsigned int expiretime;
	unsigned char dispellable;

	// Fib Heap Node variables 
	cTempEffect*	left;
	cTempEffect*	right;
	cTempEffect*	father;
	cTempEffect*	son;
	unsigned int	rank;
	bool			marker;

public:
//	cTempEffect() { serializable = true; }
	cTempEffect( cTempEffect* left_ = NULL, cTempEffect* right_ = NULL, cTempEffect* father_ = NULL,
				cTempEffect* son_ = NULL, int rank_ = 0, bool marker_ = false )
	{
		serializable = true;
		left = left_;
		right = right_;
		if( !left )
			left = this;
		if( !right )
			right = this;
		father = father_;
		son = son_;
		rank = rank_;
		marker = marker_;
	}

	bool operator<( const cTempEffect &a ) const 
	{ 
		return expiretime < a.expiretime; 
	} 

	virtual				~cTempEffect() {}
	void				setExpiretime_s(int seconds);
	void				setExpiretime_ms(float milliseconds);
	void				setDest(int ser);
	int					getDest();
	void				setSour(int ser);
	int					getSour();
	void				On(P_CHAR pc)  { Q_UNUSED(pc); }
	void				Off(P_CHAR pc) { Q_UNUSED(pc); }
	virtual void		Expire() = 0;
	virtual void		Serialize(ISerialization &archive);
	virtual void		Dispel( P_CHAR pSource, bool silent = false ) { Q_UNUSED(pSource); Q_UNUSED(silent); }
	virtual QString		objectID() const  { return objectid;}
	bool				isSerializable( void ) { return serializable; }
	void				setSerializable( bool data ) { serializable = data; }

	std::vector< cTempEffect* > asVector();
};

class cDelayedHideChar : public cTempEffect
{
	Q_OBJECT
public:
	cDelayedHideChar( SERIAL serial );
	virtual void Expire();
	virtual void Serialize(ISerialization &archive);
	virtual QString		objectID() const  { return "HIDECHAR";}
	SERIAL character;
};

class cTimedSpellAction : public cTempEffect
{
	Q_OBJECT
private:
	SERIAL character;
	UI08 action;
public:
	// Do that as long as we're casting
	cTimedSpellAction( SERIAL serial, UI08 nAction );
	virtual void Expire();
};

/*
class cTmpEffFibHeap
{
public:
	cTmpEffFibHeap() : head(0) {}
	cTmpEffFibHeap( cTempEffect* head_ )	{ head = head_; }

	// methods
	cTempEffect*	accessMin();
	void			deleteMin();
	void			erase( cTempEffect* pT );
	void			insert( cTempEffect* pT );
	cTempEffect*	meld( cTmpEffFibHeap &nFheap );

	std::vector< cTempEffect* >		asVector();

private:
	void			decrease( cTempEffect* pT, int diffTime );

public:
	// variables
	cTempEffect*	head;
};
*/

class cTempEffects
{
private:
	
	struct ComparePredicate : public std::binary_function<cTempEffect*, cTempEffect*, bool>
	{
		bool operator()(const cTempEffect *a, const cTempEffect *b)
		{
			return a->expiretime > b->expiretime;
		}
	};

public:
	cTempEffects()	{ std::make_heap( teffects.begin(), teffects.end(), cTempEffects::ComparePredicate() ); }  // No temp effects to start with
//	cTmpEffFibHeap	teffects;
//	QPtrVector< cTempEffect > vector;
	std::vector< cTempEffect* > teffects;

	void check();
	void serialize(ISerialization &archive);
	void dispel( P_CHAR pc_dest, P_CHAR pSource, bool silent = false );
	void dispel( P_CHAR pc_dest, P_CHAR pSource, const QString &type, bool silent = false, bool onlyDispellable = true );

	void insert( cTempEffect* pT );
	void erase( cTempEffect* pT );

	int	 size()
	{
		return teffects.size();
	}

	int countSerializables()
	{
		int count = 0;
		std::vector< cTempEffect* >::iterator it = teffects.begin();
		while( it != teffects.end() )
		{
			if( (*it)->isSerializable() )
				++count;
			++it;
		}
		return count;
	}
};

typedef SingletonHolder<cTempEffects> TempEffects;

// cRepeatAction
class cRepeatAction: public cTempEffect
{
	Q_OBJECT
private:
	SERIAL _mage;
	UINT8 _anim;
	UINT32 _delay;
public:
	cRepeatAction( P_CHAR mage, UINT8 anim, UINT32 delay );
	virtual void Expire();
	virtual QString objectID() const { return "repeataction"; }
};

class cDelayedHeal: public cTempEffect
{
private:
	UINT16 amount;
public:
	cDelayedHeal( P_CHAR pSource, P_CHAR pTarget, UINT16 _amount );
	virtual void Expire();
};

class cNPC_AI;

class cAIRefreshTimer: public cTempEffect
{
private:
	P_NPC m_npc;
	cNPC_AI* m_interface;
public:
	cAIRefreshTimer( P_NPC m_npc, cNPC_AI* m_interface, UINT32 time );
	virtual void Expire();
};

class cFleeReset: public cTempEffect
{
private:
	P_NPC m_npc;
public:
	cFleeReset( P_NPC m_npc, UINT32 time );
	virtual void Serialize(ISerialization &archive);
	virtual void Expire();
};

#endif
