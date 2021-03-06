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

#include "makemenus.h"
#include "wolfpack.h"
#include "network/uosocket.h"
#include "skills.h"
#include "targetrequests.h"
#include "wpdefmanager.h"
#include "basechar.h"
#include "resources.h"
#include "srvparams.h"
#include "python/utilities.h"

#include "debug.h"

// System Includes
#include <math.h>

using namespace std;

#undef  DBGFILE
#define DBGFILE "makemenus.cpp"

/*****************************************************************************
  cMakeItem member functions
 *****************************************************************************/

cMakeItem::cMakeItem( const QString &name, const QString &section, UINT16 amount )
{
	name_ = name;
	section_ = section;
	amount_ = amount;
}

cMakeItem::cMakeItem( const QDomElement &Tag )
{
	name_ = Tag.attribute( "name" );
	section_ = Tag.attribute( "section" );
	amount_ = hex2dec( Tag.attribute( "amount" ) ).toUShort();
	if( Tag.hasAttribute( "inherit" ) )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_MAKEITEM, Tag.attribute( "inherit" ) );
		applyDefinition( *DefSection );
	}
	applyDefinition( Tag );
}

void cMakeItem::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "name" )
		name_ = Value;

	else if( TagName == "section" )
		section_ = Value;

	else if( TagName == "amount" )
		amount_ = Value.toUShort();
}

/*****************************************************************************
  cUseItem member functions
 *****************************************************************************/

cUseItem::cUseItem( const QDomElement &Tag )
{
	name_ = Tag.attribute( "name" );
	if( !Tag.attribute( "itemid" ).isNull() )
	{
		QString idstr = Tag.attribute( "itemid" );
		QStringList ids;
		ids.push_back( idstr );
		if( idstr.contains(",") )
		{
			ids = QStringList::split( ",", idstr );
		}

		QStringList::const_iterator it = ids.begin();
		while( it != ids.end() )
		{
			if( (*it).contains("-") )
			{
				QStringList idspan = QStringList::split( "-", idstr );
				UINT16 min = hex2dec(idspan[0]).toUShort();
				UINT16 max = hex2dec(idspan[1]).toUShort();
				while( min <= max )
				{
					id_.push_back( min );
					++min;
				}
			}
			else
				id_.push_back( hex2dec( (*it) ).toUShort() );

			++it;
		}
	}
	amount_ = hex2dec( Tag.attribute( "amount" ) ).toUShort();
	if( Tag.hasAttribute( "inherit" ) )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_USEITEM, Tag.attribute( "inherit" ) );
		applyDefinition( *DefSection );
	}
	applyDefinition( Tag );
	if( colors_.size() == 0 )
		colors_.push_back( 0 );
}

cUseItem::cUseItem( const QString& name, const QValueVector< UINT16 >& ids, const QValueVector< UINT16 >& colors, UINT16 amount ) :
amount_(amount), colors_(colors), id_(ids), name_(name)
{
}

void cUseItem::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "name" )
		name_ = Value;

	else if( TagName == "id" )
	{
		QString idstr = Tag.attribute( "itemid" );
		QStringList ids;
		ids.push_back( idstr );
		if( idstr.contains(",") )
		{
			ids = QStringList::split( ",", idstr );
		}

		QStringList::const_iterator it = ids.begin();
		while( it != ids.end() )
		{
			if( (*it).contains("-") )
			{
				QStringList idspan = QStringList::split( "-", idstr );
				UINT16 min = hex2dec(idspan[0]).toUShort();
				UINT16 max = hex2dec(idspan[1]).toUShort();
				while( min <= max )
				{
					id_.push_back( min );
					++min;
				}
			}
			else
				id_.push_back( hex2dec( (*it) ).toUShort() );

			++it;
		}
	}

	else if( TagName == "amount" )
		amount_ = Value.toUShort();

	else if( TagName == "color" )
	{
		QString Value = Tag.text();
		QDomNode chNode = Tag.firstChild();
		while( !chNode.isNull() )
		{
			if( chNode.isElement() )
			{
				QDomElement chTag = chNode.toElement();
				QString chTagName = chTag.nodeName();
				QString chValue = getNodeValue( chTag );
				if( chTagName == "getlist" && chTag.hasAttribute( "id" ) )
				{
					QStringList list = DefManager->getList( chTag.attribute( "id" ) );
					QStringList::const_iterator it = list.begin();
					while( it != list.end() )
					{
						colors_.push_back( hex2dec((*it)).toUShort() );
						++it;
					}
				}
			}
			chNode = chNode.nextSibling();
		}

		QStringList colstr;
		if( Value.contains( "," ) )
		{
			colstr = QStringList::split( ",", Value );
		}
		else
			colstr.push_back( Value );
		
		QStringList::const_iterator it = colstr.begin();
		while( it != colstr.end() )
		{
			if( (*it).contains( "-" ) )
			{
				QStringList cols = QStringList::split( "-", (*it) );
				UINT16 mincol = hex2dec(cols[0]).toUShort();
				UINT16 maxcol = hex2dec(cols[1]).toUShort();
				while( mincol <= maxcol )
				{
					colors_.push_back( mincol );
					++mincol;
				}
			}
			else
				colors_.push_back( hex2dec((*it)).toUShort() );
			++it;
		}
	}
}

bool cUseItem::hasEnough( const cItem* pBackpack ) const
{
	// the next loop will search for a the item in a range of colors.
	// it is a do-while, cause it shall run once through the loop if
	// colormin holds the one color and colormax == 0!
	QValueVector< UINT16 >::const_iterator color = colors_.begin();
	QValueVector< UINT16 >::const_iterator end(colors_.end());
	UINT16 amount = 0;
	while( color != end )
	{
		QValueVector< UINT16 > ids = this->id();
		QValueVector< UINT16 >::iterator it = ids.begin();
		while( it != ids.end() )
		{
			amount += pBackpack->CountItems( (*it), (*color) );
			++it;
		}
		++color;
	}

	return ( amount >= this->amount() );
}

/*****************************************************************************
  cSkillCheck member functions
 *****************************************************************************/

cSkillCheck::cSkillCheck( const QDomElement &Tag )
{
	skillid_ = hex2dec( Tag.attribute( "skillid" ) ).toUShort();
	min_ = hex2dec( Tag.attribute( "min" ) ).toUShort();
	max_ = hex2dec( Tag.attribute( "max" ) ).toUShort();
	if( Tag.hasAttribute( "inherit" ) )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_SKILLCHECK, Tag.attribute( "inherit" ) );
		applyDefinition( *DefSection );
	}
	applyDefinition( Tag );
}

void cSkillCheck::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "skillid" )
		skillid_ = Value.toUShort();

	else if( TagName == "min" )
		min_ = Value.toUShort();

	else if( TagName == "max" )
		max_ = Value.toUShort();
}

bool cSkillCheck::skilledEnough( P_CHAR pChar )
{
	return ( pChar->skillValue( skillid() ) >= minimum() );
}

void cSkillCheck::applySkillMod( float skillmod )
{
	min_ = (UINT16)ceil( float(min_) * skillmod );
	max_ = (UINT16)ceil( float(max_) * skillmod );

	if( min_ > max_ )
	{
		UINT16 temp = max_;
		max_ = min_;
		min_ = temp;
	}
}

/*****************************************************************************
  cMakeSection member functions
 *****************************************************************************/

cMakeSection::~cMakeSection() 
{
}

cMakeSection::cMakeSection( const QString &name, cMakeAction* baseaction )
{
	baseaction_ = baseaction;
	name_ = name;
}

cMakeSection::cMakeSection( const QDomElement &Tag, cMakeAction* baseaction )
{
	baseaction_ = baseaction;
	name_ = Tag.attribute( "name" );
	if( Tag.hasAttribute( "inherit" ) )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_MAKESECTION, Tag.attribute( "inherit" ) );
		applyDefinition( *DefSection );
	}
	applyDefinition( Tag );
}

void cMakeSection::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );
	
	if( TagName == "name" )
		name_ = Value;
}

void cMakeSection::execute( cUOSocket* const socket )
{
}


cMakeCustomSection::~cMakeCustomSection()
{
	makeitems_.setAutoDelete( true );
	makeitems_.clear();

	useitems_.setAutoDelete( true );
	useitems_.clear();

	skillchecks_.setAutoDelete( true );
	skillchecks_.clear();
}

cMakeCustomSection::cMakeCustomSection( const QString &name, cMakeAction* baseaction ) : cMakeSection( name, baseaction )
{
}

cMakeCustomSection::cMakeCustomSection( const QDomElement &Tag, cMakeAction* baseaction ) : cMakeSection( Tag, baseaction )
{
}

void cMakeCustomSection::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "makeitem" )
	{
		cMakeItem* pMakeItem = new cMakeItem( Tag );
		makeitems_.append( pMakeItem );
	}

	else if( TagName == "useitem" )
	{
		cUseItem* pUseItem = new cUseItem( Tag );
		useitems_.append( pUseItem );
	}

	else if( TagName == "skillcheck" )
	{
		cSkillCheck* pSkillCheck = new cSkillCheck( Tag );
		skillchecks_.append( pSkillCheck );
	}

	else
		cMakeSection::processNode( Tag );
}

bool	cMakeCustomSection::hasEnough( cItem* pBackpack )
{
	bool hasEnough = true;
	QPtrList< cUseItem > useitems = useitems_;
	QPtrListIterator< cUseItem > uiit( useitems );
	while( (uiit.current()) && hasEnough )
	{
		// this one is in here because a GM doesnt need a backpack if he uses the add menu!
		if( !pBackpack )
		{
			hasEnough = false;
			break;
		}

		hasEnough = uiit.current()->hasEnough( pBackpack );

		++uiit;
	}
	return hasEnough;
}

void	cMakeCustomSection::useResources( cItem* pBackpack )
{
	QPtrList< cUseItem > useitems = useitems_;
	QPtrListIterator< cUseItem > uiit( useitems );
	while( uiit.current() )
	{
		if( !pBackpack )
			return;

		// the next loop will use the items up in a range of colors.
		// it is a do-while, cause it shall run once through the loop if
		// colormin holds the one color and colormax == 0!
		QValueVector< UINT16 > colors = uiit.current()->colors();
		QValueVector< UINT16 >::iterator color = colors.begin();
		UINT16 amount = uiit.current()->amount();
		UINT16 curramount = 0;
		while( color != colors.end() && amount > 0 )
		{
			QValueVector< UINT16 > ids = uiit.current()->id();
			QValueVector< UINT16 >::iterator it = ids.begin();
			while( it != ids.end() && amount > 0 )
			{
				// remove all available items or just the amount thats left
				curramount = pBackpack->CountItems( (*it), (*color) );
				if( curramount > amount )
				curramount = amount;
				pBackpack->DeleteAmount( curramount, (*it), (*color) );

				amount -= curramount;
				++it;
			}
			++color;
		}

		++uiit;
	}
}

// if any of foreach(skill) : skill < min => false
bool	cMakeCustomSection::skilledEnough( P_CHAR pChar )
{
	bool skilledEnough = true;
	QPtrListIterator< cSkillCheck > skit( skillchecks_ );
	while( skit.current() && skilledEnough )
	{
		skilledEnough = skit.current()->skilledEnough( pChar );
		++skit;
	}
	return skilledEnough;
}

// calcRank checks the skill and may raise it! (==0) => failed, (>0) => success
UINT32	cMakeCustomSection::calcRank( P_CHAR pChar )
{
	bool hasSuccess = true;
	UINT32 ranksum = 1;
	QPtrListIterator< cSkillCheck > skit( skillchecks_ );
	while( skit.current() && hasSuccess )
	{
		if( !pChar->checkSkill( skit.current()->skillid(), skit.current()->minimum(), skit.current()->maximum() ) )
			hasSuccess = false;
		else
		{
			UINT16 skill = pChar->skillValue( skit.current()->skillid() );
			if( skill > skit.current()->maximum() )
				skill = skit.current()->maximum();
			if( skill < skit.current()->minimum() )
				skill = skit.current()->minimum();

			ranksum += (UINT16)( ceil( (double)skill / (double)(skit.current()->maximum() - skit.current()->minimum()) * 10.0f ) );
		}
		++skit;
	}
	if( !hasSuccess )
		return 0;
	else
		return ranksum;
}

void cMakeCustomSection::execute( cUOSocket* const socket )
{
	P_PLAYER pChar	 = socket->player();
	P_ITEM pBackpack = pChar->getBackpack();

	if( !socket || !pChar || !baseaction_ )
		return;

	// first check for necessary items
	if( !hasEnough( pBackpack ) )
	{
		socket->sysMessage( tr("You do not have enough resources to create this!") );
		return;
	}

	// now we have to use the resources up!
	useResources( pBackpack );

	// then lets check the skill and calculate the item rank!
	UINT32 ranksum = calcRank( pChar );
	if( ranksum == 0 )
	{
		if( !baseaction_->failMsg().isNull() )
		socket->sysMessage( baseaction_->failMsg() );
		if( baseaction_->failSound() > 0 )
			pChar->soundEffect( baseaction_->failSound() );
		return;
	}
	else if( !baseaction_->succMsg().isNull() )
	{
		socket->sysMessage( baseaction_->succMsg() );
		if( baseaction_->succSound() > 0 )
			pChar->soundEffect( baseaction_->succSound() );
	}

	// lets calculate the rank for the item now
	UINT16 rank;
	if( skillchecks_.count() == 0 )
		rank = 10;
	else
		rank = static_cast<UINT16>(( ranksum / skillchecks_.count() ));
	if( rank > 10 )
		rank = 10;
	if( rank < 1 )
		rank = 1;

	// finally lets create the items/npcs!

	// items:
	QPtrList< cMakeItem > makeitems = makeitems_;
	QPtrListIterator< cMakeItem > miit( makeitems_ );
	while( miit.current() )
	{
		if( miit.current()->section().isNull() )
		{
			++miit;
			continue;
		}
		P_ITEM pItem = Items->createScriptItem( miit.current()->section() );
		if( pItem )
		{
			if( pItem->isPileable() )
				pItem->setAmount( miit.current()->amount() );
			pItem->applyRank( rank );
			pBackpack->addItem( pItem );
			pItem->update();
		}

		// if the item is not pileable create amount-1 items more
		if( pItem && !pItem->isPileable() )
		{
			for( UINT16 i = 1; i < miit.current()->amount(); ++i )
			{
				pItem = Items->createScriptItem( miit.current()->section() );
				if( pItem )
				{
					pItem->applyRank( rank );
					pBackpack->addItem( pItem );
					pItem->update();
				}
			}
		}
		++miit;
	}

	if( baseaction_->charAction() > 0 )
		pChar->action( baseaction_->charAction() );

	if( makeitems_.count() > 0 )
	{
		QString Message;
		switch( rank )
		{
			case 1: 
				Message = tr("You made an item with no quality!");
				break;

			case 2: 
				Message = tr("You made an item very below standard quality!");
				break;

			case 3: 
				Message = tr("You made an item below standard quality!");
				break;

			case 4: 
				Message = tr("You made a weak quality item!");
				break;

			case 5: 
				Message = tr("You made a standard quality item!");
				break;

			case 6: 
				Message = tr("You made a nice quality item!");
				break;

			case 7: 
				Message = tr("You made a good quality item!");
				break;

			case 8: 
				Message = tr("You made a great quality item!");
				break;

			case 9: 
				Message = tr("You made a beautiful quality item!");
				break;

			case 10: 
				Message = tr("You made a perfect quality item!");
				break;
		}
		socket->sysMessage( Message );
	}
}


cMakeNpcSection::~cMakeNpcSection()
{
}

cMakeNpcSection::cMakeNpcSection( const QString &name, cMakeAction* baseaction ) : cMakeSection( name, baseaction )
{
}

cMakeNpcSection::cMakeNpcSection( const QDomElement &Tag, cMakeAction* baseaction ) : cMakeSection( Tag, baseaction )
{
}

void cMakeNpcSection::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "makenpc" )
	{
		makenpc_.name = Tag.attribute( "name" );
		makenpc_.section = Tag.attribute( "id" );
	}

	else
		cMakeSection::processNode( Tag );
}

void cMakeNpcSection::execute( cUOSocket* const socket )
{
	P_PLAYER pChar	 = socket->player();
	P_ITEM pBackpack = pChar->getBackpack();

	if( !socket || !pChar || !baseaction_ )
		return;

	// npcs:
	if( !makenpc_.section.isEmpty() )
		socket->attachTarget( new cAddNpcTarget( makenpc_.section ) );

	if( baseaction_->charAction() > 0 )
		pChar->action( baseaction_->charAction() );
}


cMakeResourceSection::cMakeResourceSection( const QString &name, cMakeAction* baseaction ) : cMakeCustomSection( name, baseaction )
{
}

cMakeResourceSection::cMakeResourceSection( const QDomElement &Tag, cMakeAction* baseaction ) : cMakeCustomSection( Tag, baseaction )
{
}

void cMakeResourceSection::processNode( const QDomElement &Tag )
{
	cMakeCustomSection::processNode( Tag );
}

void cMakeResourceSection::addMakeItemSectionPrefixes( const QString& prefix )
{
	QPtrListIterator< cMakeItem > it( makeitems_ );
	while( it.current() )
	{
		it.current()->setSection( QString( "%1_%2" ).arg( prefix.lower() ).arg( it.current()->section() ) );
		++it;
	}
}

void cMakeResourceSection::applySkillMod( float skillmod )
{
	QPtrListIterator< cSkillCheck > it( skillchecks_ );
	while( it.current() )
	{
		it.current()->applySkillMod( skillmod );
		++it;
	}
}

void cMakeResourceSection::execute( cUOSocket* const socket )
{
	cMakeCustomSection::execute( socket );
}


cMakeAmountSection::cMakeAmountSection( const QString &name, cMakeAction* baseaction ) : cMakeCustomSection( name, baseaction )
{
}

cMakeAmountSection::cMakeAmountSection( const QDomElement &Tag, cMakeAction* baseaction ) : cMakeCustomSection( Tag, baseaction )
{
}

void cMakeAmountSection::processNode( const QDomElement &Tag )
{
	cMakeCustomSection::processNode( Tag );
}

void cMakeAmountSection::setMakeItemAmounts( UINT16 amount )
{
	QPtrListIterator< cMakeItem > it( makeitems_ );
	while( it.current() )
	{
		it.current()->setAmount( amount );
		++it;
	}
}

void cMakeAmountSection::execute( cUOSocket* const socket )
{
	cMakeCustomSection::execute( socket );
}


cDoCodeAction::~cDoCodeAction()
{
}

cDoCodeAction::cDoCodeAction( const QString &name, cMakeAction* baseaction ) : cMakeSection( name, baseaction )
{
}

cDoCodeAction::cDoCodeAction( const QDomElement &Tag, cMakeAction* baseaction ) : cMakeSection( Tag, baseaction )
{
}

void cDoCodeAction::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	cMakeSection::processNode( Tag );
}

void cDoCodeAction::execute( cUOSocket* const socket )
{
	P_PLAYER pChar	 = socket->player();
	P_ITEM pBackpack = pChar->getBackpack();

	if( !socket || !pChar || !baseaction_ )
		return;

	if( name_ == "repair" )
	{
		socket->sysMessage( tr("Choose item to repair!") );
		cSkRepairItem* pTargetRequest = new cSkRepairItem( this );
		socket->attachTarget( pTargetRequest );
		return;
	}
}


cDoScriptAction::~cDoScriptAction()
{
}

cDoScriptAction::cDoScriptAction( const QString &name, cMakeAction* baseaction ) : cMakeSection( name, baseaction )
{
}

cDoScriptAction::cDoScriptAction( const QDomElement &Tag, cMakeAction* baseaction ) : cMakeSection( Tag, baseaction )
{
	if( Tag.hasAttribute( "script" ) )
		functionName = Tag.attribute( "script" );
}

void cDoScriptAction::processNode( const QDomElement &Tag )
{
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "script" )
		functionName = Value;
	else
		cMakeSection::processNode( Tag );
}

void cDoScriptAction::execute( cUOSocket* const socket )
{
	P_PLAYER pChar	 = socket->player();

	if( !pChar || !baseaction_ )
		return;

	// Get everything before the last dot
	if( functionName.contains( "." ) )
	{
		// Find the last dot
		INT32 position = functionName.findRev( "." );
		QString sModule = functionName.left( position );
		QString sFunction = functionName.right( functionName.length() - (position+1) );

		PyObject *pModule = PyImport_ImportModule( const_cast< char* >( sModule.latin1() ) );

		if( pModule )
		{
			PyObject *pFunc = PyObject_GetAttrString( pModule, const_cast< char* >( sFunction.latin1() ) );
			if( pFunc && PyCallable_Check( pFunc ) )
			{
				// Create our Argument list
				PyObject *p_args = PyTuple_New( 1 );
				PyTuple_SetItem( p_args, 0, PyGetCharObject( pChar ) );

				PyEval_CallObject( pFunc, p_args );
				
				if( PyErr_Occurred() )
					PyErr_Print();
			}
		}
	}
}


cMakeAction::cMakeAction( const QString &name, UINT16 model, const QString &description, WPACTIONTYPE type, cMakeMenu *basemenu )
{
	basemenu_ = basemenu;
	name_ = name;
	model_ = model;
	description_ = description;
	failmsg_ = (char*)0;
	succmsg_ = (char*)0;
	charaction_ = 0;
	succsound_ = 0;
	failsound_ = 0;
	type_ = type;
}

cMakeAction::cMakeAction( const QDomElement &Tag, cMakeMenu* basemenu )
{
	basemenu_ = basemenu;
	name_ = Tag.attribute( "id" );
	description_ = (char*)0;
	failmsg_ = (char*)0;
	succmsg_ = (char*)0;
	charaction_ = 0;
	succsound_ = 0;
	failsound_ = 0;
	type_ = CUSTOM_SECTIONS;
	if( Tag.hasAttribute( "type" ) )
	{
		QString Value = Tag.attribute( "type" );
		if( Value == "resource" || Value.toUShort() == 1 )
			type_ = RESOURCE_SECTIONS;
		else if( Value == "amount" || Value.toUShort() == 2 )
			type_ = AMOUNT_SECTIONS;
		else if( Value == "npc" || Value.toUShort() == 3 )
			type_ = NPC_SECTION;
		else if( Value == "delayed" || Value.toUShort() == 4 )
			type_ = DELAYED_SECTIONS;
		else if( Value == "code" || Value.toUShort() == 5 )
			type_ = CODE_ACTION;
		else if( Value == "script" || Value.toUShort() == 6 )
			type_ = SCRIPT_ACTION;
	}
	if( Tag.hasAttribute( "inherit" ) )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_ACTION, Tag.attribute( "inherit" ) );
		applyDefinition( *DefSection );
	}
	applyDefinition( Tag );
}

void cMakeAction::processNode( const QDomElement &Tag )
{
	// CAUTION: the base tag attributes are evaluated in the constructor!!
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	if( TagName == "do" )
	{
		if( type_ == CODE_ACTION )
		{
			cDoCodeAction* pMakeSection = new cDoCodeAction( Tag, this );
			if( pMakeSection )
				makesections_.push_back( pMakeSection );
		}
		else if( type_ == SCRIPT_ACTION )
		{
			cDoScriptAction* pMakeSection = new cDoScriptAction( Tag, this );
			if( pMakeSection )
				makesections_.push_back( pMakeSection );
		}
	}
	else if( TagName == "make" )
	{
		if( type_ == CUSTOM_SECTIONS )
		{
			cMakeCustomSection* pMakeSection = new cMakeCustomSection( Tag, this );
			if( pMakeSection )
				makesections_.push_back( pMakeSection );
		}
		else if( type_ == NPC_SECTION )
		{
			cMakeNpcSection* pMakeSection = new cMakeNpcSection( Tag, this );
			if( pMakeSection )
				makesections_.push_back( pMakeSection );
		}
		else if( type_ == AMOUNT_SECTIONS && Tag.hasAttribute( "amounts" ) )
		{
			QString amountstr = Tag.attribute("amounts");
			QStringList amounts = QStringList::split( ",", amountstr );
			QStringList::const_iterator it = amounts.begin();
			while( it != amounts.end() )
			{
				if( (*it).toUShort() > 0 )
				{
					cMakeAmountSection* pMakeSection = new cMakeAmountSection( Tag, this );
					if( pMakeSection )
					{
						pMakeSection->setMakeItemAmounts( (*it).toUShort() );
						pMakeSection->setName( QString("(%1)").arg( (*it).toUShort() ) );
						makesections_.push_back( pMakeSection );
					}
				}
				++it;
			}
		}
		else if( type_ == RESOURCE_SECTIONS && Tag.hasAttribute( "resource" ) )
		{
			cAllResources::iterator found = Resources::instance()->find( Tag.attribute( "resource" ) );
			if( found != Resources::instance()->end() )
			{
				cResource* pResource = found->second;
				if( pResource )
				{
					QValueVector< cResource::resourcespec_st > resourcespecs = pResource->resourceSpecs();
					QValueVector< cResource::resourcespec_st >::iterator it = resourcespecs.begin();
					while( it != resourcespecs.end() )
					{
						cResource::resourcespec_st item = (*it);
						QString name;
						UINT16 amount = 1;
						QValueVector< UINT16 > id;
						QDomNode childNode = Tag.firstChild();
						while( !childNode.isNull() )
						{
							if( childNode.isElement() )
							{
								QDomElement childTag = childNode.toElement(); 
								if( childTag.nodeName() == "useitem" )
								{
									if( childTag.hasAttribute( "amount" ) )
										amount = hex2dec( childTag.attribute( "amount" ) ).toUShort();

									if( childTag.hasAttribute( "itemid" ) )
									{
										QString idstr = childTag.attribute( "itemid" );
										QStringList ids;
										ids.push_back( idstr );
										if( idstr.contains(",") )
										{
											ids = QStringList::split( ",", idstr );
										}
	
										QStringList::const_iterator it = ids.begin();
										while( it != ids.end() )
										{
											if( (*it).contains("-") )
											{
												QStringList idspan = QStringList::split( "-", idstr );
												UINT16 min = hex2dec(idspan[0]).toUShort();
												UINT16 max = hex2dec(idspan[1]).toUShort();
												while( min <= max )
												{
													id.push_back( min );
													++min;
												}
											}
											else
												id.push_back( hex2dec( (*it) ).toUShort() );

											++it;
										}
									}
								}
							}
							childNode = childNode.nextSibling();
						}

						cMakeResourceSection* pMakeSection = new cMakeResourceSection( Tag, this );
						if( pMakeSection->name().isEmpty() )
							name = pResource->name();
						else
							name = pMakeSection->name();
						
						if( !item.name.isNull() )
							name = QString("%1 %2").arg( item.name ).arg( name );

						if( id.empty() )
							id = item.ids;

						amount = (UINT16)ceil( (float)amount * item.makeuseamountmod );

						cUseItem* pUseItem = new cUseItem( name, id, item.colors, amount );
						if( pMakeSection )
						{
							if( !pUseItem )
								delete pMakeSection;
							else
							{
								pMakeSection->setName( item.name );
								pMakeSection->appendUseItem( pUseItem );
								pMakeSection->addMakeItemSectionPrefixes( item.name );
								pMakeSection->applySkillMod( item.makeskillmod );
								makesections_.push_back( pMakeSection );
							}
						}
						else
						{
							if( pUseItem )
								delete pUseItem;
						}


						++it;
					}
				}
			}
		}
		else if( type_ == SCRIPT_ACTION && Tag.hasAttribute( "script" ) )
		{

		}
	}

	else if( TagName == "fail" )
		failmsg_ = Value;

	else if( TagName == "success" )
		succmsg_ = Value;

	else if( TagName == "description" )
		description_ = Value;

	else if( TagName == "charaction" )
		charaction_ = Value.toUShort();

	else if( TagName == "name" )
		name_ = Value;

	else if( TagName == "sound" )
	{
		succsound_ = Value.toUShort();
		failsound_ = succsound_;
	}
	
	else if( TagName == "succsound" )
		succsound_ = Value.toUShort();

	else if( TagName == "failsound" )
		failsound_ = Value.toUShort();

	else if( TagName == "model" )
		model_ = Value.toUShort();

	else if( TagName == "bodymodel" )
		model_ = creatures[ Value.toUShort() ].icon;
}

void cMakeAction::execute( cUOSocket* socket, UINT32 makesection )
{
	if( makesection >= makesections_.size() )
		return;

	cMakeSection* pSection = makesections_[ makesection ];
	if( !pSection )
		return;

	pSection->execute( socket );
}

/*****************************************************************************
  cMakeMenu member functions
 *****************************************************************************/

cMakeMenu::cMakeMenu( const QString &name, cMakeMenu* previous )
{
	name_ = name;
	link_ = (char*)0;

	prev_ = previous;
}

// cross linking will lead to an infinite loop and stack overflow
// we avoid this by adding a special attribute for the menu tags
// instead of the inherit attributes: link
cMakeMenu::cMakeMenu( const QDomElement &Tag, cMakeMenu* previous )
{
	name_ = Tag.attribute( "id" );
	link_ = (char*)0;
	if( Tag.hasAttribute( "link" ) )
	{
		link_ = Tag.attribute( "link" );
		const QDomElement* DefSection = DefManager->getSection( WPDT_MENU, Tag.attribute( "link" ) );
		applyDefinition( *DefSection );
	}
	if( Tag.hasAttribute( "inherit" ) )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_MENU, Tag.attribute( "inherit" ) );
		applyDefinition( *DefSection );
	}
	prev_ = previous;
	applyDefinition( Tag );
}

void cMakeMenu::processNode( const QDomElement &Tag )
{
	// CAUTION: the base tag attributes are evaluated in the constructor!!
	QString TagName = Tag.nodeName();
	QString Value = getNodeValue( Tag );

	bool recurse = link_.isNull();

	if( TagName == "menu" && recurse )
	{
		cMakeMenu* pMakeMenu = new cMakeMenu( Tag, this );

		if( pMakeMenu )
			submenus_.push_back( pMakeMenu );
	}

	else if( TagName == "action" && recurse )
	{
		cMakeAction* pMakeAction = new cMakeAction( Tag, this );
		actions_.push_back( pMakeAction );
	}

	else if( TagName == "name" )
		name_ = Value;
}

/*****************************************************************************
  cMakeMenuGump member functions
 *****************************************************************************/

cMakeMenuGump::cMakeMenuGump( cMakeMenu* menu, cUOSocket* socket, QString notices )
{
	prev_ = menu->prevMenu();
	// link attribute:
	if( !menu->link().isNull() )
	{
		cMakeMenu* pMenu = MakeMenus::instance()->getMenu( menu->link() );
		if( pMenu )
			menu = pMenu;
		else
		{
			pMenu = menu->prevMenu();
			if( pMenu )
				menu = menu->prevMenu();
		}
	}

	QString htmlmask = "<basefont color=\"#FFFFFF\">%1";
	QString htmlmaskcenter = "<basefont color=\"#FFFFFF\"><div align=\"center\">%1</div>";

	action_ = NULL;
	menu_ = menu;

	startPage();
	addResizeGump( 0, 0, 0x13BE, 530, 417 );

	addTiledGump( 10, 10, 510, 22, 0xA40, -1 );
	addTiledGump( 10,292, 150, 45, 0xA40, -1 );
	addTiledGump( 165, 292, 355, 45, 0xA40, -1 );
	addTiledGump( 10, 342, 510, 65, 0xA40, -1 );
	addTiledGump( 10, 37, 200, 250, 0xA40, -1 );
	addTiledGump( 215, 37, 305, 250, 0xA40, -1 );
	addCheckertrans( 10, 10, 510, 397 );

	addHtmlGump( 10, 12, 510, 20, htmlmaskcenter.arg( menu_->name() ) ); 
	addHtmlGump( 10, 39, 200, 20, htmlmaskcenter.arg( tr("CATEGORIES") ) ); 
	addHtmlGump( 215, 39, 305, 20, htmlmaskcenter.arg( tr("SELECTIONS") ) ); 
	addHtmlGump( 10, 294, 150, 20, htmlmaskcenter.arg( tr("NOTICES") ) );

	if( !notices.isEmpty() )
		addHtmlGump( 170, 292, 345, 41, htmlmask.arg( notices ) );

	// EXIT button , return value: 0
	addButton( 15, 382, 0xFB1, 0xFB3, 0 );
	addHtmlGump( 50, 385, 150, 18, htmlmask.arg( tr("EXIT") ) );

	// LAST TEN button , return value: 1
	addButton( 15, 60, 4011, 4012, 1 );
	addHtmlGump( 50, 63, 150, 18, htmlmask.arg( tr("LAST TEN") ) );

	// MARK ITEM switch , switch index: 1
	addCheckbox( 270, 362, 4005, 4007, 1, false );
	addHtmlGump( 305, 365, 150, 18, htmlmask.arg( tr("MARK ITEM") ) );

	// MAKE LAST button , return value: 2
	addButton( 270, 382, 4005, 4007, 2 );
	addHtmlGump( 305, 385, 150, 18, htmlmask.arg( tr("MAKE LAST") ) );

	// PREVIOUS MENU button, return value: -1
	if( prev_ )
	{
		addButton( 15, 362, 0xFAE, 0xFB0, -1 );
		addHtmlGump( 50, 365, 150, 18, htmlmask.arg( tr("PREVIOUS MENU") ) );
	}

	cMakeMenu::SubMenuContainer submenus = menu_->subMenus();
	cMakeMenu::ActionContainer  actions  = menu_->actions();
	cMakeMenu::SubMenuContainer::iterator mit = submenus.begin();
	cMakeMenu::ActionContainer::iterator ait = actions.begin();

	UINT32 page_;
	UINT32 menupages = ((UINT32)ceil( (double)submenus.size() / 9.0f ));
	UINT32 actionpages = ((UINT32)ceil( (double)actions.size() / 10.0f ));
	UINT32 pages = ( menupages > actionpages ) ? menupages : actionpages;

	for( page_ = 1; page_ <= pages; ++page_ )
	{
		startPage( page_ );
		UINT32 yoffset = 80;
		UINT32 i = (page_-1) * 9;
		if( i  <= submenus.size() )
		{
			while( mit != submenus.end() && i <= submenus.size() && i < (page_ * 9) )
			{
				// category buttons, return values: 3 -> submenus.size()+3-1
				addButton( 15, yoffset, 4005, 4007, i+3 );
				addHtmlGump( 50, yoffset+3, 150, 18, htmlmask.arg( (*mit)->name() ) );
				yoffset += 20;
				++i;
				++mit;
			}
		}

		yoffset = 60;
		i = (page_-1) * 10;
		if( i  <= actions.size() )
		{
			while( ait != actions.end() && i <= actions.size() && i < (page_ * 10) )
			{
				// selection buttons, return values: submenus.size()+3 -> actions.size()+submenus.size()+3-1
				addButton( 220, yoffset, 4005, 4007, i + submenus.size() + 4 );
				addButton( 480, yoffset, 4011, 4012, i + submenus.size() + 4 + 1000 );
				addHtmlGump( 255, yoffset+3, 220, 18, htmlmask.arg( (*ait)->name() ) );
				yoffset += 20;
				++i;
				++ait;
			}
		}

		// page switch buttons
		if( page_ > 1 )
		{
			addPageButton( 15, 342, 0xFAE, 0xFB0, page_-1 );
			addHtmlGump( 50, 345, 150, 18, htmlmask.arg( tr("PREVIOUS PAGE") ) );
		}
		if( page_ < pages )
		{
			addPageButton( 270, 342, 4005, 4007, page_+1 );
			addHtmlGump( 305, 345, 150, 18, htmlmask.arg( tr("NEXT PAGE") ) );
		}

	}
}

cMakeMenuGump::cMakeMenuGump( cMakeAction* action, cUOSocket* socket )
{
	P_PLAYER pChar = socket->player();
	action_ = action;
	menu_ = action->baseMenu();
	prev_ = action->baseMenu();

	QString htmlmask = "<basefont color=\"#FFFFFF\">%1";
	QString htmlmaskcenter = "<basefont color=\"#FFFFFF\"><div align=\"center\">%1</div>";

	startPage();
	addResizeGump( 0, 0, 5054, 530, 417 );
	addTiledGump( 10, 10, 510, 22, 2624, -1 );
	addTiledGump( 165,	37, 355, 88, 2624, -1 );
	addTiledGump( 165, 130, 355, 80, 2624, -1 );
	addTiledGump( 165, 215, 355, 80, 2624, -1 );
	addTiledGump( 165, 300, 355, 80, 2624, -1 );
	addTiledGump( 10, 385, 510, 22, 2624, -1 );
	addTiledGump( 10,  37, 150, 88, 2624, -1 );
	addTiledGump( 10, 130, 150, 22, 2624, -1 );
	addTiledGump( 10, 215, 150, 22, 2624, -1 );
	addTiledGump( 10, 300, 150, 22, 2624, -1 );
	addCheckertrans( 10, 10, 510, 397 );

	addHtmlGump( 10, 12, 510, 20, htmlmaskcenter.arg( menu_->name() ) ); 
	if( action->model() > 0 )
		addTilePic( 15, 42, action->model() );
	addHtmlGump( 10, 132, 150, 20, htmlmaskcenter.arg( tr("SKILLS") ) );
	addHtmlGump( 10, 217, 150, 20, htmlmaskcenter.arg( tr("MATERIALS") ) );
	addHtmlGump( 10, 302, 150, 20, htmlmaskcenter.arg( tr("OTHER") ) );

	if( !action->description().isNull() )
		addHtmlGump( 170, 302, 345, 76, htmlmask.arg( action->description() ), false, true );

	addHtmlGump( 170, 39, 70, 20, htmlmask.arg( tr("ITEM") ) );

	addButton( 15, 387, 0xFAE, 0xFB0, -1 );
	addHtmlGump( 50, 389, 80, 18, htmlmask.arg( tr("BACK") ) );

	UINT32 page = 1;
	cMakeAction::SectionContainer makesections = action->makesections();
	cMakeAction::SectionContainer sections;
	std::vector< UINT32 > offsets;
	cMakeAction::SectionContainer::iterator it = makesections.begin();
	cItem* pBackpack = 0;
	if( pChar )
		 pBackpack = pChar->getBackpack();
	UINT32 i = 1;

	while( it != makesections.end() )
	{
		if( pChar && pBackpack )
		{
			if( action->type() == cMakeAction::CUSTOM_SECTIONS || 
				action->type() == cMakeAction::AMOUNT_SECTIONS ||
				action->type() == cMakeAction::RESOURCE_SECTIONS )
			{
				cMakeCustomSection* pMCS = dynamic_cast< cMakeCustomSection* >( *it );
				if( pMCS->hasEnough( pBackpack ) && pMCS->skilledEnough( pChar ) )
				{
					sections.push_back( (*it) );
					offsets.push_back( i );
				}
			}
			else
			{
				sections.push_back( (*it) );
				offsets.push_back( i );
			}
		}
		++i;
		++it;
	}
	it = sections.begin();
	std::vector< UINT32 >::iterator button = offsets.begin();
	cMakeAction::SectionContainer::iterator next = sections.begin();
	++next;

	while( it != sections.end() )
	{
		QString content;

		startPage( page );

		addHtmlGump( 245, 39, 270, 20, htmlmask.arg( QString("%1 %2").arg(action->name()).arg((*it)->name()) ) );

		if( sections.size() > 1 )
		{
			content = "";
			addPageButton( 135, 387, 4005, 4007, (page < sections.size()) ? (page+1) : 1 );
			if( next != sections.end() )
				content = (*next)->name();
			else
				content = (*(sections.begin()))->name();

			if( content.isEmpty() )
				content = tr("NEXT");
			addHtmlGump( 170, 389, 200, 18, htmlmask.arg( content.upper() ) );
		}

		addButton( 375, 387, 4005, 4007, (*button) );
		addHtmlGump( 410, 389, 95, 18, htmlmask.arg( tr("MAKE NOW") ) );

		cMakeCustomSection* pMCS = dynamic_cast<cMakeCustomSection*>( *it );
		if( pMCS )
		{
			content = "";
			QPtrList< cSkillCheck > skillchecks = pMCS->skillchecks();
			QPtrListIterator< cSkillCheck > sit( skillchecks );
			while( sit.current() )
			{
				content += QString("%2% %1<br>").arg( Skills->getSkillName( sit.current()->skillid() ) ).arg( QString::number( (double)sit.current()->minimum() / 10.0f, 'f', 1 ).lower() );
				++sit;
			}
			content = htmlmask.arg( content );
			addHtmlGump( 170, 132, 345, 76, content, false, (skillchecks.count() > 4) );

			content = "";
			QPtrList< cUseItem > useitems = pMCS->useitems();
			QPtrListIterator< cUseItem > uit( useitems );
			while( uit.current() )
			{
				content += QString("%2 %1<br>").arg( uit.current()->name() ).arg( uit.current()->amount() );
				++uit;
			}
			content = htmlmask.arg( content );
			addHtmlGump( 170, 217, 345, 76, content, false, (useitems.count() > 4) );
		}

		++page;
		++button;
		++it;
		++next;
	}
	// not enough resources or not skilled enough
	if( page == 1 )
	{
		startPage( page );
		addHtmlGump( 245, 39, 270, 20, htmlmask.arg( action->name() ) );

		cMakeCustomSection* pMCS = dynamic_cast< cMakeCustomSection* >( makesections[0] );
		if( ( makesections.size() > 0 ) && ( pMCS ) )
		{
			QString content = "";
			QPtrList< cSkillCheck > skillchecks = pMCS->skillchecks();
			QPtrListIterator< cSkillCheck > sit( skillchecks );
			while( sit.current() )
			{
				if( pChar && sit.current()->skilledEnough( pChar ) )
					content += QString("%2% %1<br>").arg( Skills->getSkillName( sit.current()->skillid() ) ).arg( QString::number( (double)sit.current()->minimum() / 10.0f, 'f', 1 ).lower() );
				else
					content += QString("<basefont color=\"#FF0000\">%2% %1<br><basefont color=\"#FFFFFF\">").arg( Skills->getSkillName( sit.current()->skillid() ) ).arg( QString::number( (double)sit.current()->minimum() / 10.0f, 'f', 1 ).lower() );

				++sit;
			}
			content = htmlmask.arg( content );
			addHtmlGump( 170, 132, 345, 76, content, false, (skillchecks.count() > 4) );

			content = "";
			QPtrList< cUseItem > useitems = pMCS->useitems();
			QPtrListIterator< cUseItem > uit( useitems );
			while( uit.current() )
			{
				if( pBackpack && uit.current()->hasEnough( pBackpack ) )
					content += QString("%2 %1<br>").arg( uit.current()->name() ).arg( uit.current()->amount() );
				else
					content += QString("<basefont color=\"#FF0000\">%2 %1<br><basefont color=\"#FFFFFF\">").arg( uit.current()->name() ).arg( uit.current()->amount() );

				++uit;
			}
			content = htmlmask.arg( content );
			addHtmlGump( 170, 217, 345, 76, content, false, (useitems.count() > 4) );
		}	
	}
}

void cMakeMenuGump::handleResponse( cUOSocket* socket, gumpChoice_st choice )
{
	if( choice.button == 0 || !socket || !menu_ )
		return;

	if( !MakeMenus::instance()->contains( menu_ ) )
	{
		socket->sysMessage( tr("Menu lost because of script reload. Try again please!") );
		return;
	}

	cMakeMenu::SubMenuContainer submenus = menu_->subMenus();
	cMakeMenu::ActionContainer  actions  = menu_->actions();

	if( choice.button == -1 && prev_ && MakeMenus::instance()->contains( prev_ ) )
		socket->send( new cMakeMenuGump( prev_, socket ) );
	else if( action_ ) // we have response of a detail menu
	{
		cMakeAction::SectionContainer sections = action_->makesections();
		if( choice.button <= sections.size() )
		{
			cMakeSection* section = sections[ choice.button-1 ];
			cMakeMenu* basemenu = menu_->baseMenu();
			if( socket->player() )
				socket->player()->setLastSection( basemenu, section );
			section->execute( socket );
		}
	}
	else if( choice.button == 1 )
	{
		P_PLAYER pChar = socket->player();
		if( !pChar )
			return;
		QPtrList< cMakeSection > sections = pChar->lastSelections( menu_->baseMenu() );
		socket->send( new cLastTenGump( sections, menu_ ) ); 
	}
	else if( choice.button == 2 )
	{
		P_PLAYER pChar = socket->player();
		if( !pChar )
			return;
		if( pChar->lastSection( menu_->baseMenu() ) )
			pChar->lastSection( menu_->baseMenu() )->execute( socket );
	}
	else if( choice.button < submenus.size()+3 )
	{
		socket->send( new cMakeMenuGump( submenus[ choice.button-3 ], socket ) );
	}
	else if( choice.button < (actions.size()+submenus.size()+4) )
	{
		P_PLAYER pChar = socket->player();
		if( !pChar )
			return;
		cItem* pBackpack = pChar->getBackpack();
		cMakeCustomSection* pMCS = NULL;
		cMakeAction::SectionContainer sections = actions[ choice.button - submenus.size() - 4 ]->makesections();
		if( sections.empty() )
		{
			socket->send( new cMakeMenuGump( menu_, socket, tr("There is nothing to make") ) );
		}
		else if( pMCS = dynamic_cast< cMakeCustomSection* >( sections[0] ) )
		{
			if( pMCS->hasEnough( pBackpack ) )
			{
				if( pMCS->skilledEnough( pChar ) )
				{
					cMakeAction* action = actions[ choice.button - submenus.size() - 4 ];
					cMakeAction::SectionContainer makesections = action->makesections();
					cMakeSection* section = makesections[0];
					cMakeMenu* basemenu = menu_->baseMenu();
					pChar->setLastSection( basemenu, section );
					section->execute( socket );
				}
				else
				{
					socket->send( new cMakeMenuGump( menu_, socket, tr("You are not skilled enough to make this item") ) );
				}
			}
			else
			{
				socket->send( new cMakeMenuGump( menu_, socket, tr("You do not have enough resources to make this item") ) );
			}
		}
		else
		{
			cMakeAction* action = actions[ choice.button - submenus.size() - 4 ];
			cMakeAction::SectionContainer makesections = action->makesections();
			cMakeSection* section = makesections[0];
			cMakeMenu* basemenu = menu_->baseMenu();
			pChar->setLastSection( basemenu, section );
			section->execute( socket );
		}
	}
	else if( choice.button < (actions.size()+submenus.size()+4+1000) )
	{
		P_PLAYER pChar = socket->player();
		if( !pChar )
			return;
		cMakeAction* action = actions[ choice.button - submenus.size() - 4 - 1000 ];
		std::vector< UINT32 > switches = choice.switches;
		std::vector< UINT32 >::iterator it = switches.begin();
		if( !action->makesections().empty() )
		{
			cMakeMenu* basemenu = menu_->baseMenu();
			cMakeSection* section = action->makesections()[0];
			while( it != switches.end() )
			{
				if( (*it) == 1 )
					pChar->setLastSection( basemenu, section );
				it++;
			}
		}
		socket->send( new cMakeMenuGump( action, socket ) );
	}
}

/*****************************************************************************
  cLastTenGump member functions
 *****************************************************************************/

cLastTenGump::cLastTenGump( QPtrList< cMakeSection > sections, cMakeMenu* prev, QString notices )
{
	QString htmlmask = "<basefont color=\"#FFFFFF\">%1";
	QString htmlmaskcenter = "<basefont color=\"#FFFFFF\"><div align=\"center\">%1</div>";

	sections_ = sections;
	prev_ = prev;

	setX( 50 );
	setY( 50 );

	startPage();
	addResizeGump( 0, 0, 0x13BE, 530, 417 );
	addTiledGump( 10, 10, 510, 22, 0xA40, -1 );
	addTiledGump( 10,292, 150, 45, 0xA40, -1 );
	addTiledGump( 165, 292, 355, 45, 0xA40, -1 );
	addTiledGump( 10, 342, 510, 65, 0xA40, -1 );
	addTiledGump( 10, 37, 200, 250, 0xA40, -1 );
	addTiledGump( 215, 37, 305, 250, 0xA40, -1 );
	addCheckertrans( 10, 10, 510, 397 );

	addHtmlGump( 10, 12, 510, 20, htmlmaskcenter.arg( tr("LAST TEN") ) ); 
	addHtmlGump( 10, 39, 200, 20, htmlmaskcenter.arg( tr("CATEGORIES") ) ); 
	addHtmlGump( 215, 39, 305, 20, htmlmaskcenter.arg( tr("SELECTIONS") ) ); 
	addHtmlGump( 10, 294, 150, 20, htmlmaskcenter.arg( tr("NOTICES") ) );

	if( !notices.isEmpty() )
		addHtmlGump( 170, 292, 345, 41, htmlmask.arg( notices ) );

	// EXIT button , return value: 0
	addButton( 15, 382, 0xFB1, 0xFB3, 0 );
	addHtmlGump( 50, 385, 150, 18, htmlmask.arg( tr("EXIT") ) );

	// PREVIOUS MENU button, return value: -1
	if( prev_ )
	{
		addButton( 15, 362, 0xFAE, 0xFB0, -1 );
		addHtmlGump( 50, 365, 150, 18, htmlmask.arg( tr("PREVIOUS MENU") ) );
	}

	QPtrListIterator< cMakeSection > it( sections );

	startPage( 1 );
	UINT32 yoffset = 60;
	UINT32 i = 1;
	while( it.current() )
	{
		if( it.current()->baseAction() && it.current()->baseAction()->baseMenu() )
		{
			// category buttons, return values: 1 -> 10
			addButton( 15, yoffset, 4005, 4007, i );
			addHtmlGump( 50, yoffset+3, 150, 18, htmlmask.arg( it.current()->baseAction()->baseMenu()->name() ) );
		}
		// selection buttons, return values: 11 -> 20
		addButton( 220, yoffset, 4005, 4007, i+10 );
		// detail buttons, return values: 21 -> 30
		addButton( 480, yoffset, 4011, 4012, i+20 );
		addHtmlGump( 255, yoffset+3, 295, 18, htmlmask.arg( QString("%1 %2").arg(it.current()->baseAction()->name()).arg(it.current()->name()) ) );
		yoffset += 20;
		++i;
		++it;
	}
}

void cLastTenGump::handleResponse( cUOSocket* socket, gumpChoice_st choice )
{
	if( choice.button == 0 || !socket )
		return;

	// script reload will lose any object, so it is enough to search for the prev menu.
	if( !MakeMenus::instance()->contains( prev_ ) )
	{
		socket->sysMessage( tr("Menu lost because of script reload. Try again please!") );
		return;
	}

	if( choice.button == -1 && prev_ )
		socket->send( new cMakeMenuGump( prev_, socket ) );
	else if( choice.button > 0 && choice.button <= 10 )
	{
		cMakeSection* section = sections_.at( choice.button-1 );
		socket->send( new cMakeMenuGump( section->baseAction()->baseMenu(), socket ) );
	}
	else if( choice.button > 10 && choice.button <= 20 )
	{
		cMakeSection* section = sections_.at( choice.button-11 );
		P_PLAYER pChar = socket->player();
		if( !pChar )
			return;
		cItem* pBackpack = pChar->getBackpack();
		cMakeCustomSection* pMCS = NULL;
		if( section && ( pMCS = dynamic_cast< cMakeCustomSection* >( section ) ) && pMCS->hasEnough( pBackpack ) )
		{
			section->execute( socket );
		}
		else
			socket->send( new cLastTenGump( sections_, prev_, tr("You do not have enough resources to make this item") ) );
	}
	else if( choice.button > 20 && choice.button <= 30 )
	{
		cMakeSection* section = sections_.at( choice.button-21 );
		cMakeAction* action = section->baseAction();
		socket->send( new cMakeMenuGump( action, socket ) ); 
	}
}

/*****************************************************************************
  cAllMakeMenu member functions
 *****************************************************************************/

cAllMakeMenus::~cAllMakeMenus()
{
	std::map< QString, cMakeMenu* >::iterator iter(menus_.begin());
	std::map< QString, cMakeMenu* >::iterator end (menus_.end());
	while( iter != end )
	{
		delete iter->second;
		++iter;
	}
	menus_.clear();
}

QString	cAllMakeMenus::getValue( const QDomElement &Tag ) const
{
	QString Value;
	if( !Tag.hasChildNodes() )
		return "";
	else
	{
		QDomNode childNode = Tag.firstChild();
		while( !childNode.isNull() )
		{
			if( !childNode.isElement() )
			{
				if( childNode.isText() )
					Value += childNode.toText().data();
				childNode = childNode.nextSibling();
				continue;
			}
			QDomElement childTag = childNode.toElement();
			if( childTag.nodeName() == "random" )
			{
				if( childTag.attributes().contains("min") && childTag.attributes().contains("max") )
					if( childTag.attribute( "min", "0" ).contains( "." ) || childTag.attribute( "max", "0" ).contains( "." ) )
						Value += QString::number( RandomNum( childTag.attributeNode("min").nodeValue().toFloat(), childTag.attributeNode("max").nodeValue().toFloat() ) );
					else
						Value += QString::number( RandomNum( childTag.attributeNode("min").nodeValue().toInt(), childTag.attributeNode("max").nodeValue().toInt() ) );
				else if( childTag.attributes().contains("valuelist") )
				{
					QStringList RandValues = QStringList::split(",", childTag.attributeNode("list").nodeValue());
					Value += RandValues[ RandomNum(0,RandValues.size()-1) ];
				}
				else if( childTag.attributes().contains( "list" ) )
				{
					Value += DefManager->getRandomListEntry( childTag.attribute( "list" ) );
				}
				else if( childTag.attributes().contains("dice") )
					Value += QString::number(rollDice(childTag.attributeNode("dice").nodeValue()));
				else if( childTag.attributes().contains( "value" ) )
				{
					QStringList parts = QStringList::split( "-", childTag.attribute( "value", "0-0" ) );
					if( parts.count() >= 2 )
					{
						QString min = parts[0];
						QString max = parts[1];

						if( max.contains( "." ) || min.contains( "." ) )
							Value += QString::number( RandomNum( min.toFloat(), max.toFloat() ) );
						else
							Value += QString::number( RandomNum( min.toInt(), max.toInt() ) );

					}
				}
				else
					Value += QString( "0" );
			}

			// Process the childnodes
			QDomNodeList childNodes = childTag.childNodes();

			for( int i = 0; i < childNodes.count(); i++ )
			{
				if( !childNodes.item( i ).isElement() )
					continue;

				Value += this->getValue( childNodes.item( i ).toElement() );
			}
			childNode = childNode.nextSibling();
		}
	}
	return hex2dec( Value );
}

UINT16 cAllMakeMenus::getModel( const QDomElement &Tag )
{
	UINT16 model = 0;
	QDomNode childNode = Tag.firstChild();
	while( !childNode.isNull() )
	{
		if( childNode.isElement() )
		{
			QDomElement childTag = childNode.toElement();
			if( childTag.nodeName() == "id" )
				model = getValue( childTag ).toUShort();
/*			else if( childTag.nodeName() == "inherit" )
			{
				QString section = (char*)0;
				if( childTag.hasAttribute( "id" ) )
					section = childTag.attribute( "id" );
				else
					section = childTag.text();

				QDomElement inhSection = *DefManager->getSection( WPDT_ITEM, section );
				if( !inhSection.isNull() )
					model = getModel( inhSection );
			}*/
		}
		childNode = childNode.nextSibling();
	}
	
	return model;
}

void cAllMakeMenus::load()
{
	QStringList sections = DefManager->getSections( WPDT_MENU );
	QStringList::const_iterator it = sections.begin();
	while( it != sections.end() )
	{
		const QDomElement* DefSection = DefManager->getSection( WPDT_MENU, (*it) );
		if( !DefSection->isNull() )
		{
			cMakeMenu* pMakeMenu = new cMakeMenu( *DefSection );
			if( pMakeMenu && !( (*it) == "ADD_MENU" && SrvParams->addMenuByCategoryTag() ) )
			{
				menus_.insert( make_pair< QString, cMakeMenu* >( (*it), pMakeMenu ) );
			}
		}
		++it;
	}

	// build addmenu by category tags
	if( SrvParams->addMenuByCategoryTag() )
	{
		// create base menu
		cMakeMenu* pAddMenu = new cMakeMenu( tr( "Add Menu" ) );
		if( !pAddMenu )
			return;
		
		menus_.insert( make_pair< QString, cMakeMenu* >( "ADD_MENU", pAddMenu ) );

		// item menu
		cMakeMenu* pItemMenu = new cMakeMenu( tr( "Items" ), pAddMenu );
		pAddMenu->addSubMenu( pItemMenu );

		if( pItemMenu )
		{
			QStringList sections = DefManager->getSections( WPDT_ITEM );
			
			QStringList::const_iterator it = sections.begin();
			while( it != sections.end() )
			{
				QDomElement defSection = *DefManager->getSection( WPDT_ITEM, (*it) );
				if( !defSection.isNull() )
				{
					QStringList category;
					QString description;
					UINT16 model = 0;

					QDomNode childNode = defSection.firstChild();
					while( !childNode.isNull() )
					{
						if( childNode.isElement() )
						{
							QDomElement childTag = childNode.toElement();
							if( childTag.nodeName() == "category" )
							{
								category.push_back( childTag.text() );
							}
							else if( childTag.nodeName() == "description" )
							{
								description = childTag.text();
							}
							else if( childTag.nodeName() == "id" )
							{
								model = getValue( childTag ).toUShort();
							}
							else if( childTag.nodeName() == "inherit" )
							{
								QString section;
								if( childTag.hasAttribute( "id" ) )
									section = childTag.attribute( "id" );
								else
									section = childTag.text();

								QDomElement inhSection = *DefManager->getSection( WPDT_ITEM, section );
								if( !inhSection.isNull() )
									model = getModel( inhSection );
							}
						}

						childNode = childNode.nextSibling();
					}

					description += "\nDef.-section: "+(*it);
					
					if( !category.isEmpty() )
					{
						QStringList::const_iterator catIt = category.begin();
						for ( ; catIt != category.end(); ++catIt )
						{
							cMakeMenu* currentBaseMenu = pItemMenu;
							
							QStringList categorization = QStringList::split( "\\", *catIt );
							QStringList::const_iterator cit = categorization.begin();
							while( cit != categorization.end() )
							{
								QString current = (*cit);
								++cit;
								if( cit != categorization.end() )
								{
									bool menuExists = false;
									cMakeMenu::SubMenuContainer submenus = currentBaseMenu->subMenus();
									cMakeMenu::SubMenuContainer::const_iterator sit = submenus.begin();
									while( sit != submenus.end() )
									{
										if( (*sit)->name() == current )
										{
											menuExists = true;
											currentBaseMenu = (*sit);
											break;
										}
										++sit;
									}
	
									if( !menuExists )
									{
										// generate submenu, set current basemenu and continue
										cMakeMenu* pNewMenu = new cMakeMenu( current, currentBaseMenu );
										currentBaseMenu->addSubMenu( pNewMenu );
										currentBaseMenu = pNewMenu;
									}
								}
								else // last item means name
								{
									// generate cMakeAction object for the item definition...
									cMakeAction* pItem = new cMakeAction( current, model, description, cMakeAction::CUSTOM_SECTIONS, currentBaseMenu );
									if( pItem )
									{
										currentBaseMenu->addAction( pItem );
										cMakeCustomSection* pMakeSection = new cMakeCustomSection( "", pItem );
										if( pMakeSection )
										{
											pItem->appendSection( pMakeSection );
											cMakeItem* pMakeItem = new cMakeItem( "", (*it), 1 );
											if( pMakeItem )
												pMakeSection->appendMakeItem( pMakeItem );
										}
									}
								}
							}
						}
					}
				}
				++it;
			}
		}

		// npc menu
		cMakeMenu* pNpcMenu = new cMakeMenu( tr( "NPCs" ), pAddMenu );
		pAddMenu->addSubMenu( pNpcMenu );

		if( pNpcMenu )
		{
			QStringList sections = DefManager->getSections( WPDT_NPC );
			
			QStringList::const_iterator it = sections.begin();
			while( it != sections.end() )
			{
				QDomElement defSection = *DefManager->getSection( WPDT_NPC, (*it) );
				if( !defSection.isNull() )
				{
					QStringList category;
					QString description;
					UINT16 model = 0;

					QDomNode childNode = defSection.firstChild();
					while( !childNode.isNull() )
					{
						if( childNode.isElement() )
						{
							QDomElement childTag = childNode.toElement();
							if( childTag.nodeName() == "category" )
							{
								category.push_back( childTag.text() );
							}
							else if( childTag.nodeName() == "description" )
							{
								description = childTag.text();
							}
							else if( childTag.nodeName() == "id" )
							{
								model = getValue( childTag ).toUShort();
							}
							else if( childTag.nodeName() == "inherit" )
							{
								QString section = (char*)0;
								if( childTag.hasAttribute( "id" ) )
									section = childTag.attribute( "id" );
								else
									section = childTag.text();

								QDomElement inhSection = *DefManager->getSection( WPDT_ITEM, section );
								if( !inhSection.isNull() )
									model = getModel( inhSection );
							}
						}

						childNode = childNode.nextSibling();
					}
					
					description += "\nDef.-section: "+(*it);

					if( !category.isEmpty() )
					{
						QStringList::const_iterator catIt = category.begin();
						for ( ; catIt != category.end(); ++catIt )
						{
							cMakeMenu* currentBaseMenu = pNpcMenu;
		
							QStringList categorization = QStringList::split( "\\", *catIt );
							QStringList::const_iterator cit = categorization.begin();
							while( cit != categorization.end() )
							{
								QString current = (*cit);
								++cit;
								if( cit != categorization.end() )
								{
									bool menuExists = false;
									cMakeMenu::SubMenuContainer submenus = currentBaseMenu->subMenus();
									cMakeMenu::SubMenuContainer::const_iterator sit = submenus.begin();
									while( sit != submenus.end() )
									{
										if( (*sit)->name() == current )
										{
											menuExists = true;
											currentBaseMenu = (*sit);
											break;
										}
											++sit;
									}
	
									if( !menuExists )
									{
										// generate submenu, set current basemenu and continue
										cMakeMenu* pNewMenu = new cMakeMenu( current, currentBaseMenu );
										currentBaseMenu->addSubMenu( pNewMenu );
										currentBaseMenu = pNewMenu;
									}
								}	
								else // last item means name
								{
									// generate cMakeAction object for the npc definition...
									cMakeAction* pNpc = new cMakeAction( current, creatures[model].icon, description, cMakeAction::NPC_SECTION, currentBaseMenu );
									if( pNpc )
									{
										currentBaseMenu->addAction( pNpc );
										cMakeNpcSection* pMakeSection = new cMakeNpcSection( "", pNpc );
										if( pMakeSection )
										{
											pNpc->appendSection( pMakeSection );
											pMakeSection->setNpcProperties( current, (*it) );
										}
									}
								}
							}
						}
					}
				}
				++it;
			}
		}

		// resources menu
		cMakeMenu* pResMenu = new cMakeMenu( tr( "Resources" ), pAddMenu );
		pAddMenu->addSubMenu( pResMenu );

		if( pResMenu )
		{
			cAllResources::iterator it = Resources::instance()->begin();
			while( it != Resources::instance()->end() )
			{
				cResource* pResource = it->second;
				if( pResource )
				{
					cMakeMenu* pMakeMenu = new cMakeMenu( pResource->name(), pResMenu );
					if( pMakeMenu )
					{
						pResMenu->addSubMenu( pMakeMenu );
						QValueVector< cResource::resourcespec_st > resourcespecs = pResource->resourceSpecs();
						QValueVector< cResource::resourcespec_st >::iterator rit = resourcespecs.begin();
						while( rit != resourcespecs.end() )
						{
							cResource::resourcespec_st item = (*rit);
							cMakeAction* pMakeAction = new cMakeAction( item.name + " " + pResource->name(), item.ids[0], "", cMakeAction::CUSTOM_SECTIONS, pMakeMenu ); 
							if( pMakeAction )
							{
								UINT16 model = 0;
								if( item.ids.count() > 0 )
									model = item.ids[0];

								pMakeMenu->addAction( pMakeAction );

								QValueVector< UINT16 > amounts( 10 );
								amounts[0] = 1;		amounts[1] = 2;		amounts[2] = 5;
								amounts[3] = 10;	amounts[4] = 20;	amounts[5] = 50;
								amounts[6] = 100;	amounts[7] = 200;	amounts[8] = 500;
								amounts[9] = 1000;

								QValueVector< UINT16 >::iterator ait = amounts.begin();
								while( ait != amounts.end() )
								{
									cMakeCustomSection* pMakeSection = new cMakeCustomSection( "("+QString::number( (*ait) )+")", pMakeAction );
									if( pMakeSection )
									{
										pMakeAction->appendSection( pMakeSection );
										cMakeItem* pMakeItem = new cMakeItem( "", item.name.lower() + "_" + it->first, (*ait) );
										if( pMakeItem )
											pMakeSection->appendMakeItem( pMakeItem );
									}
									++ait;
								}
							}
							++rit;
						}
					}
				}
				++it;
			}
		}
	}
}

void cAllMakeMenus::reload()
{
	std::map< QString, cMakeMenu* >::iterator iter = menus_.begin();
	while( iter != menus_.end() )
	{
		delete iter->second;
		++iter;
	}
	menus_.clear();

	for ( cUOSocket* mSock = cNetwork::instance()->first(); mSock; mSock = cNetwork::instance()->next()	)
	{
		mSock->player()->clearLastSelections();
	}
	load();
}

void cAllMakeMenus::callMakeMenu( cUOSocket* socket, const QString& section )
{
	cMakeMenu* pMenu = getMenu( section );
	if(	pMenu )
	{
		cMakeMenuGump* pGump = new cMakeMenuGump( pMenu, socket );
		socket->send( pGump );
	}
	else
		clConsole.send( tr("WARNING: Missing %1 menu definition!").arg(section) );
}



