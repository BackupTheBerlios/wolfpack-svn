
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

#include "engine.h"

#include "../territories.h"
#include "../skills.h"
#include "../party.h"
#include "../combat.h"
#include "../srvparams.h"
#include "../chars.h"
#include "../walking.h"
#include "../commands.h"
#include "../scriptmanager.h"
#include "../makemenus.h"
#include "../npc.h"
#include "../guilds.h"
#include "../basechar.h"
#include "../player.h"
#include "../singleton.h"

#include "utilities.h"
#include "content.h"
#include "tempeffect.h"
#include "objectcache.h"
#include "skills.h"

/*
	\object char
	\inherit object
	\description This object type represents players and npcs in the world.
	To determine whether the object instance is for a player or a npc, use the
	<i>npc</i> and <i>player</i> properties.
*/
typedef struct {
    PyObject_HEAD;
	P_CHAR pChar;
} wpChar;

// Note: Must be of a different type to cause more then 1 template instanciation
class cCharObjectCache : public cObjectCache< wpChar, 50 >
{
};

typedef SingletonHolder< cCharObjectCache > CharCache;

static void FreeCharObject( PyObject *obj )
{
	CharCache::instance()->freeObj( obj );
}

PyObject *wpChar_getAttr( wpChar *self, char *name );
int wpChar_setAttr( wpChar *self, char *name, PyObject *value );
int wpChar_compare( PyObject*, PyObject* );

long wpChar_hash(wpChar *self) {
	return self->pChar->serial();
}

/*!
	The typedef for Wolfpack Python chars
*/
static PyTypeObject wpCharType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "wpchar",
    sizeof(wpCharType),
    0,
//	FreeCharObject,
	wpDealloc,
	0,
    (getattrfunc)wpChar_getAttr,
    (setattrfunc)wpChar_setAttr,
	wpChar_compare,
	0,
	0,
	0,
	0,
	(hashfunc)wpChar_hash
};

PyObject* PyGetCharObject( P_CHAR pChar )
{
	if( !pChar )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

//	wpChar *returnVal = CharCache::instance()->allocObj( &wpCharType );
	wpChar *returnVal = PyObject_New( wpChar, &wpCharType );
	returnVal->pChar = pChar;
	return (PyObject*)returnVal;
}

// Methods

/*
	\method char.update
	\description Resend the character.
	\param clean Defaults to 0.
	If true, the character will be removed from all connected sockets before resending him.
*/
static PyObject* wpChar_update(wpChar* self, PyObject* args)
{
	Q_UNUSED(args);
	unsigned char clean = 0;

	if (!PyArg_ParseTuple(args, "|B:char.update(clean)", &clean))
	{
		return 0;
	}

	self->pChar->resend(clean != 0);

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.removefromview
	\description Remove the character from all clients who can currently see it.
	\param clean Defaults to 0.
	If true, the character will be removed from all connected sockets, even if they
	can't see him.
*/
static PyObject* wpChar_removefromview( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgInt( 0 ) || getArgInt( 0 ) == 0 )
		self->pChar->removeFromView( false );
	else
		self->pChar->removeFromView( true );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.message
	\description Send an overhead message to a player. This method does nothing for NPCs.
	\param message The message to be sent.
	\param color Optional parameter.
	The color of the overhead message.
*/
/*
	\method char.message
	\description Send an overhead localized message to a player. This method does nothing for NPCs.
	\param message The numeric id of the localized message.
	\param params Optional parameter. 
	Parameters that should be parsed into the localized message.
	\param affix Optional parameter.
	If specified, the affix will be appended to the localized message.
*/
static PyObject* wpChar_message( wpChar* self, PyObject* args )
{
	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );
	
	if( !player || !player->socket() )
		return PyFalse;

	if (checkArgStr(0)) {
		QString message = getArgStr( 0 );

		if( ( player->body() == 0x3DB ) && message.startsWith( SrvParams->commandPrefix() ) )
			Commands::instance()->process( player->socket(), message.right( message.length()-1 ) );
		else if( message.startsWith( SrvParams->commandPrefix() ) )
			Commands::instance()->process( player->socket(), message.right( message.length()-1 ) );
		else if( PyTuple_Size( args ) == 2 && PyInt_Check( PyTuple_GetItem( args, 1 ) ) )
			player->message( message, PyInt_AsLong( PyTuple_GetItem( args, 1 ) ) );
		else
			player->message( message );
	}
	else if( checkArgInt( 0 ) )
	{
		// Affix?
		unsigned int id;
		char *clilocargs = 0;
		char *affix = 0;

		if( !PyArg_ParseTuple( args, "i|ss:char.message( clilocid, [args], [affix] )", &id, &clilocargs, &affix ) )
			return 0;		

		// Cliloc Message
		if( affix )
			player->socket()->clilocMessageAffix( id, clilocargs, affix, 0x3b2, 3, player, false, false );
		else
			player->socket()->clilocMessage( id, clilocargs, 0x3b2, 3, player );
	}
	else
	{
		PyErr_BadArgument();
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.moveto
	\description Change the position of this character.
	\param pos The coord object representing the new position.
*/
/*
	\method char.moveto
	\description Change the position of this character.
	\param x The new x coordinate of this character.
	\param y The new y coordinate of this character.
	\param z Defaults to the current z position of the character.
	The new z coordinate of this character.
	\param map Defaults to the current map the character is on.
	The new map coordinate of this character.
*/
static PyObject* wpChar_moveto( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( PyTuple_Size( args ) == 1 && checkWpCoord( PyTuple_GetItem( args, 0 ) ) )
	{
		self->pChar->moveTo( getWpCoord( PyTuple_GetItem( args, 0 ) ) );
		return PyTrue;
	}

	// Gather parameters
	Coord_cl pos = self->pChar->pos();

	if( PyTuple_Size( args ) <= 1 )
	{
		PyErr_BadArgument();
		return NULL;
	}

	// X,Y
	if( PyTuple_Size( args ) >= 2 )
	{
		if( !PyInt_Check( PyTuple_GetItem( args, 0 ) ) || !PyInt_Check( PyTuple_GetItem( args, 1 ) ) )
			return PyFalse;

		pos.x = PyInt_AsLong( PyTuple_GetItem( args, 0 ) );
		pos.y = PyInt_AsLong( PyTuple_GetItem( args, 1 ) );
	}

	// Z
	if( PyTuple_Size( args ) >= 3 )
	{
		if( !PyInt_Check( PyTuple_GetItem( args, 2 ) ) )
			return PyFalse;

		pos.z = PyInt_AsLong( PyTuple_GetItem( args, 2 ) );
	}

	// MAP
	if( PyTuple_Size( args ) >= 4 )
	{
		if( !PyInt_Check( PyTuple_GetItem( args, 3 ) ) )
			return PyFalse;

		pos.map = PyInt_AsLong( PyTuple_GetItem( args, 3 ) );
	}

	self->pChar->moveTo( pos );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.sound
	\description Play a creature dependant soundeffect.
	\param sound One of the following constants from <library id="wolfpack.consts">wolfpack.consts</library>:
	<code>SND_STARTATTACK = 0
	SND_IDLE = 1
	SND_ATTACK = 2
	SND_DEFEND = 3
	SND_DIE = 4</code>
*/
static PyObject* wpChar_sound( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgInt( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	unsigned char arg = getArgInt( 0 );

	if( arg > cBaseChar::Bark_Death )
		return PyFalse;

	self->pChar->bark( (cBaseChar::enBark)arg );
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.soundeffect
	\description Play a soundeffect originating from the character.
	\param sound The id of the soundeffect.
	\param hearall Defaults to true.
	If set to false, only the character can hear the sound. Has no 
	effect for NPCs.
*/
static PyObject* wpChar_soundeffect( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( PyTuple_Size( args ) < 1 || !PyInt_Check( PyTuple_GetItem( args, 0 ) ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	if( PyTuple_Size( args ) > 1 && PyInt_Check( PyTuple_GetItem( args, 1 ) ) && !PyInt_AsLong( PyTuple_GetItem( args, 1 ) ) )
		self->pChar->soundEffect( PyInt_AsLong( PyTuple_GetItem( args, 0 ) ), false );
	else
		self->pChar->soundEffect( PyInt_AsLong( PyTuple_GetItem( args, 0 ) ) );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.distanceto
	\description Measure the distance between the character and another object.
	\param object The target object. May be another character, item or a coord 
	object.
	\return The distance in tiles towards the given target.
*/
/*
	\method char.distanceto
	\description Measure the distance between the character and a coordinate.
	\param x The x component of the target coordinate.
	\param y The y component of the target coordinate.
	\return The distance in tiles towards the given coordinate.
*/
static PyObject* wpChar_distanceto( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyInt_FromLong( -1 );

	// Probably an object
	if( PyTuple_Size( args ) == 1 )
	{
		PyObject *pObj = PyTuple_GetItem( args, 0 );

		if( checkWpCoord( PyTuple_GetItem( args, 0 ) ) )
			return PyInt_FromLong( self->pChar->pos().distance( getWpCoord( pObj ) ) );

		// Item
		P_ITEM pItem = getWpItem( pObj );
		if( pItem )
			return PyInt_FromLong( pItem->dist( self->pChar ) );

		P_CHAR pChar = getWpChar( pObj );
        if( pChar )
			return PyInt_FromLong( pChar->dist( self->pChar ) );
	}
	else if( PyTuple_Size( args ) >= 2 ) // Min 2 
	{
		Coord_cl pos = self->pChar->pos();

		if( !PyInt_Check( PyTuple_GetItem( args, 0 ) ) || !PyInt_Check( PyTuple_GetItem( args, 1 ) ) )
			return PyInt_FromLong( -1 );

		pos.x = PyInt_AsLong( PyTuple_GetItem( args, 0 ) );
		pos.y = PyInt_AsLong( PyTuple_GetItem( args, 1 ) );  

		return PyInt_FromLong( self->pChar->pos().distance( pos ) );
	}

	PyErr_BadArgument();
	return NULL;
}

/*
	\method char.action
	\description Play an animation for the character.
	The animation id is automatically translated if the character is on a horse or 
	if the current body id of the character doesn't	support the animation.
	\param id The id of the animation that should be played.
*/
static PyObject* wpChar_action( wpChar* self, PyObject* args )
{
	if( PyTuple_Size( args ) < 1 || !PyInt_Check( PyTuple_GetItem( args, 0 ) ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	self->pChar->action( PyInt_AsLong( PyTuple_GetItem( args, 0 ) ) );
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.directionto
	\description Return the direction from the character to another object.
	\param object The target object. May be another character, item or a coord 
	object.
	\return The direction towards the target.
*/
/*
	\method char.directionto
	\description Return the direction from the character to a coordinate.
	\param x The x component of the target coordinate.
	\param y The y component of the target coordinate.
	\return The direction towards the target.
*/
static PyObject* wpChar_directionto( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyInt_FromLong( -1 );

	// Probably an object
	if( PyTuple_Size( args ) == 1 )
	{
		PyObject *pObj = PyTuple_GetItem( args, 0 );

		if( checkWpCoord( pObj ) )
		{
			Coord_cl pos = getWpCoord( pObj );
			return PyInt_FromLong( self->pChar->pos().direction( Coord_cl( pos.x, pos.y ) ) );
		}

		// Item
		P_ITEM pItem = getWpItem( pObj );
		if( pItem )
			return PyInt_FromLong( self->pChar->pos().direction( pItem->pos() ) );

		P_CHAR pChar = getWpChar( pObj );
        if( pChar )
			return PyInt_FromLong( self->pChar->direction(pChar) );
	}
	else if( PyTuple_Size( args ) >= 2 ) // Min 2 
	{
		Coord_cl pos = self->pChar->pos();

		if( !PyInt_Check( PyTuple_GetItem( args, 0 ) ) || !PyInt_Check( PyTuple_GetItem( args, 1 ) ) )
			return PyInt_FromLong( -1 );

		pos.x = PyInt_AsLong( PyTuple_GetItem( args, 0 ) );
		pos.y = PyInt_AsLong( PyTuple_GetItem( args, 1 ) );  

		return PyInt_FromLong( self->pChar->pos().direction( pos ) );
	}

	PyErr_BadArgument();
	return NULL;
}

/*
	\method char.checkskill
	\description Make a skillcheck with a given difficulty and allow the 
	character to gain in that skill.
	\param skill The id of the skill that should be checked.
	\param min The lower boundary of the difficulty range.
	\param max The upper boundary of the difficulty range.
	\return True if the skillcheck succeeded, false otherwise.
*/
static PyObject* wpChar_checkskill( wpChar* self, PyObject* args )
{
	if( self->pChar->free )
		return PyFalse;

	unsigned short skill;
	unsigned short min, max;

	if( !PyArg_ParseTuple( args, "hhh|char.checkskill( skill, min, max )", &skill, &min, &max ) )
		return 0;
	
	bool success = self->pChar->checkSkill( skill, min, max );

	return success ? PyTrue : PyFalse;
}

/*
	\method char.itemonlayer
	\description Find an item on a given layer.
	\param layer The layer id.
	\return The item on the given layer or None.
*/
static PyObject* wpChar_itemonlayer( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgInt( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	return PyGetItemObject( self->pChar->atLayer( (cBaseChar::enLayer)getArgInt( 0 ) ) );
}

/*
	\method char.combatskill
	\description Get the id of the combat skill the character would use in a fight.
*/
static PyObject* wpChar_combatskill( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_ITEM pi = self->pChar->rightHandItem();
	if( !pi )
		pi = self->pChar->leftHandItem();

	return PyInt_FromLong( Combat::instance()->weaponSkill( pi ) );
}

/*
	\method char.useresource	
	\description Remove items from the characters backpack recursively.
	\param amount The amount if items that should be deleted.
	\param id The display id of the items that should be deleted.
	\param color Defaults to 0.
	The color of the items that should be deleted.
	\return The amount of items that were deleted.
*/
static PyObject* wpChar_useresource( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyInt_FromLong( 0 );
	
	if( PyTuple_Size( args ) < 2 || !PyInt_Check( PyTuple_GetItem( args, 0 ) ) || !PyInt_Check( PyTuple_GetItem( args, 1 ) ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

    UINT16 amount = PyInt_AsLong( PyTuple_GetItem( args, 0 ) );
	UINT16 id = PyInt_AsLong( PyTuple_GetItem( args, 1 ) );
	UINT16 color = 0;

	if( PyTuple_Size( args ) > 2 && PyInt_Check( PyTuple_GetItem( args, 2 ) ) )
		color = PyInt_AsLong( PyTuple_GetItem( args, 2 ) );

	P_ITEM pPack = self->pChar->getBackpack();
	UINT16 deleted = 0;

	if( pPack )
		deleted = amount - pPack->DeleteAmount( amount, id, color );

	return PyInt_FromLong( deleted );
}

/*
	\method char.resurrect
	\description Resurrect the character.
*/
static PyObject* wpChar_resurrect( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	self->pChar->resurrect();

	return PyTrue;
}

/*
	\method char.kill
	\description Kill the character. This is not forced, but instead 
	it tries to damage the character so he dies normally.
*/
static PyObject* wpChar_kill( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	self->pChar->damage( DAMAGE_GODLY, self->pChar->hitpoints() );

	return PyTrue;
}

/*
	\method char.damage
	\description Deal damage to the character.
	\param type The damage type. One of the following constants from <library id="wolfpack.consts">wolfpack.consts</library>:
	DAMAGE_PHYSICAL = 0
	DAMAGE_MAGICAL = 1
	DAMAGE_GODLY = 2
	DAMAGE_HUNGER = 3
	\param amount The amount of damage that should be dealt.
	\param source Defaults to None.
	The source of the damage. May either be a character or an item.
	\return The amount of damage really dealt.
*/
static PyObject* wpChar_damage( wpChar* self, PyObject* args )
{
	if( self->pChar->free )
		return PyFalse;

	int type, amount;
	PyObject *source = Py_None;
	
	if( !PyArg_ParseTuple( args, "ii|O:char.damage( type, amount, source )", &type, &amount, &source ) )
		return 0;

	cUObject *pSource = 0;
	if (source != Py_None) {
		PyConvertObject(source, &pSource);
	}

	return PyInt_FromLong( self->pChar->damage( (eDamageType)type, amount, pSource ) );
}

/*
	\method char.emote
	\description Show an emote above the characters head. The emote will be
	visible for everyone in range. The *...* will be automatically put in place.
	\param text The text for the emote.
*/
static PyObject* wpChar_emote( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if (!checkArgStr(0)) {
		PyErr_BadArgument();
		return NULL;
	}

	QString message = QString( "*%1*" ).arg(getArgStr(0));
	self->pChar->emote(message);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.say
	\description Let the character say a text visible for everyone in range.
	\param text The text the character should say.
	\param color Defaults to the normal speech color of the character.
	The color for the text.
*/
/*
	\method char.say
	\description Let the character say a localized text message.
	\param clilocid The id of the localizd message the character should say.
	\param params Defaults to an empty string.
	The parameters that should be parsed into the localized message.
	\param affix Defaults to an empty string.
	Text that should be appended or prepended (see the prepend parameter) to the
	localized message.
	\param prepend Defaults to false.
	If this boolean parameter is set to true, the affix is prepended rather than
	appended.
	\param color Defaults to the characters speech color.
	The color of the message.
	\param socket Defaults to None.
	If a socket object is given here, the message will only be seen by the given socket.
*/
static PyObject* wpChar_say( wpChar* self, PyObject* args, PyObject *keywds )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgStr( 0 ) )
	{
		P_NPC npc = dynamic_cast<P_NPC>( self->pChar );

		if ( !npc )
			return PyFalse;

		uint id;
		char *clilocargs = 0;
		char *affix = 0;
		char prepend;
		uint color = self->pChar->saycolor();
		cUOSocket* socket = 0;

		static char *kwlist[] = { "clilocid", "args", "affix", "prepend", "color", "socket", NULL};

		if( !PyArg_ParseTupleAndKeywords( args, keywds, "i|ssbiO&:char.say( clilocid, [args], [affix], [prepend], [color], [socket] )", kwlist, &id, &clilocargs, &affix, &prepend, &color, PyConvertSocket, &socket ) )
			return 0;		

		npc->talk( id, clilocargs, affix, prepend, color, socket );
	}
	else
	{
		ushort color = self->pChar->saycolor();
		
		if( checkArgInt( 1 ) )
			color = getArgInt( 1 );

		self->pChar->talk( getArgStr( 0 ), color );

	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.countresource
	\description Counts a certain type of item in the characters backpack.
	\param itemid The display id of the items to count.
	\param color Defaults to 0.
	The color of the items to count.
	\return The amount of items found.
*/
static PyObject* wpChar_countresource( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;
	
	if( PyTuple_Size( args ) < 1 || !checkArgInt( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	UINT16 id = getArgInt( 0 );
	INT16 color = -1;

	if( PyTuple_Size( args ) > 1 && checkArgInt( 1 ) )
		color = getArgInt( 1 );

	P_ITEM pPack = self->pChar->getBackpack();
	UINT16 avail = 0;

	if( pPack )
		avail = pPack->CountItems( id, color );

	return PyInt_FromLong( avail );
}

/*
	\method char.isitem
	\return False.
*/
static PyObject* wpChar_isitem( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	Q_UNUSED(self);
	return PyFalse;
}

/*
	\method char.ischar
	\return True.
*/
static PyObject* wpChar_ischar( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	Q_UNUSED(self);
	return PyTrue;
}

/*
	\method char.gettag
	\description Get a custom tag attached to the character.
	\return None if there is no such tag, the tag value otherwise.
	Possible return types are: unicode (string), float, integer.
	\param name The name of the tag.
*/
static PyObject* wpChar_gettag( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	if( PyTuple_Size( args ) < 1 || !checkArgStr( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	QString key = getArgStr( 0 );
	cVariant value = self->pChar->getTag( key );

	if( value.type() == cVariant::String )
		return PyUnicode_FromUnicode((Py_UNICODE*)value.toString().ucs2(), value.toString().length());
	else if( value.type() == cVariant::Int )
		return PyInt_FromLong( value.asInt() );
	else if( value.type() == cVariant::Double )
		return PyFloat_FromDouble( value.asDouble() );		

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.settag
	\description Set a custom tag on the object.
	\param name The name of the tag.
	\param value The value of the tag. Possible value types
	are string, unicode, float and integer.
*/
static PyObject* wpChar_settag( wpChar* self, PyObject* args )
{
	if( self->pChar->free )
		return PyFalse;

	char *key;
	PyObject *object;

	if (!PyArg_ParseTuple( args, "sO:char.settag( name, value )", &key, &object ))
		return 0;

	if (PyString_Check(object)) {
		self->pChar->setTag(key, cVariant(PyString_AsString(object)));
	} else if (PyUnicode_Check(object)) {
		self->pChar->setTag(key, cVariant(QString::fromUcs2((ushort*)PyUnicode_AsUnicode(object))));
	} else if (PyInt_Check(object)) {
		self->pChar->setTag(key, cVariant((int)PyInt_AsLong(object)));
	} else if (PyFloat_Check(object)) {
		self->pChar->setTag(key, cVariant((double)PyFloat_AsDouble(object)));
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.hastag
	\description Check if the character has a certain custom tag attached to it.
	\return True if the tag is present. False otherwise.
	\param name The name of the tag.
*/
static PyObject* wpChar_hastag( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( PyTuple_Size( args ) < 1 || !checkArgStr( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	QString key = getArgStr( 0 );
	
	return self->pChar->getTag( key ).isValid() ? PyTrue : PyFalse;
}

/*
	\method char.deltag
	\description Deletes a tag attached to the character.
	\param name The name of the tag.
*/
static PyObject* wpChar_deltag( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( PyTuple_Size( args ) < 1 || !checkArgStr( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	QString key = getArgStr( 0 );
	self->pChar->removeTag( key );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.addfollower
	\description Add a follower to this characters follower list.
	This method only has an effect for players.
	Please note that setting the owner of a npc automatically adds the npc to the
	owners follower list.
	\param pet The pet you want to add to this characters follower list.
*/
static PyObject* wpChar_addfollower( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );
	if (!player)
		return PyFalse;

	if (!checkArgChar(0)) {
		PyErr_BadArgument();
		return NULL;
	}

	P_NPC pPet = dynamic_cast<P_NPC>( getArgChar( 0 ) );
	
	if (pPet) {
		player->addPet( pPet );
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.removefollower
	\description Remove a follower from this players follower list.
	Please note that setting the owner of a npc automatically removes the npc from the
	previous owners follower list.
	\param pet The pet you want to remove from this characters follower list.
*/
static PyObject* wpChar_removefollower( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (!player)
		return PyFalse;

	if (!checkArgChar(0)) {
		PyErr_BadArgument();
		return NULL;
	}
  
	P_NPC pPet = dynamic_cast<P_NPC>( getArgChar( 0 ) );
	
	if (pPet) {
		player->removePet(pPet);
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.hasfollower
	\description Checks if a certain pet is in the follower list of this
	character. 
	Please note that the pet is automatically in the follower list of this
	character if it's owned by him.
	\param pet The pet you want to check for.
	\return True if the pet is in the characters follower list. False otherwise.
*/
static PyObject* wpChar_hasfollower( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );
	if ( !player )
		return PyFalse;

	if( !checkArgChar( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	P_NPC pPet = dynamic_cast<P_NPC>( getArgChar( 0 ) );
	
	if( pPet )
	{
		cBaseChar::CharContainer::const_iterator iter = player->pets().begin();
		cBaseChar::CharContainer::const_iterator end  = player->pets().end();
		return std::binary_search( iter, end, (cBaseChar*)pPet ) ? PyTrue : PyFalse;
	}

	return PyFalse;
}

/*
	\method char.updatehealth
	\description Update the healthbar of this character. The update will also be sent to 
	anyone in range and the partymembers of the character.
*/
static PyObject* wpChar_updatehealth( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	self->pChar->updateHealth();
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.updatemana
	\description Update the minimum and maximum mana of this character. 
	This method only has an effect for connected players. The change will 
	also be visible to party members of the player.
*/
static PyObject* wpChar_updatemana( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (player && player->socket()) {
		player->socket()->updateMana();
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.updatestamina
	\description Update the minimum and maximum stamina of this character. 
	This method only has an effect for connected players. The change will 
	also be visible to party members of the player.
*/
static PyObject* wpChar_updatestamina( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (player && player->socket()) {	
		player->socket()->updateStamina();
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.updatestats
	\description Resend the status information to this player.
*/
static PyObject* wpChar_updatestats( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (player && player->socket()) {	
		player->socket()->sendStatWindow();
	}

	Py_INCREF(Py_None);
	return Py_None;
}


/*
	\method char.getweapon
	\description Get the weapon the character has currently equipped.
	\return None if the character has no weapon equipped. The item object for the weapon otherwise.
*/
static PyObject* wpChar_getweapon( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	return PyGetItemObject( self->pChar->getWeapon() ); 
}

/*
	\method char.turnto
	\description Let the character turn toward another object or coordinate.
	\param target What the character should turn toward to. This may either be another character, an item or a 
	coordinate object. If the character should turn toward equipped or contained items, he will turn to the wearer 
	or outmost container.
*/
static PyObject* wpChar_turnto( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( checkArgCoord( 0 ) )
	{
		Coord_cl pos = getArgCoord( 0 );
		self->pChar->turnTo( pos );
		Py_INCREF(Py_None);
		return Py_None;
	}

	if( !checkArgObject( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	cUObject *object = 0;

	if( checkArgChar( 0 ) )
		object = getArgChar( 0 );
	else if( checkArgItem( 0 ) )
	{
		P_ITEM pItem = getArgItem( 0 );
		
		pItem = pItem->getOutmostItem();

		if( pItem->container() && pItem->container()->isChar() )
			object = pItem->container();
		else
			object = pItem;
	}

	if( object && object != self->pChar )
		self->pChar->turnTo( object );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.mount
	\description Forces the character to mount a mountable npc.
	This method only works for players as the mounter and npcs 
	as the mounted object.
	\param pet The pet the player should be mounted on.
*/
static PyObject* wpChar_mount( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgChar( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (!player) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	P_NPC pChar = dynamic_cast<P_NPC>( getArgChar( 0 ) );

	if (pChar) {
		player->mount(pChar);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.unmount
	\description Forces the character to unmount a mounted npc.
	This method only works for players.
	\return The previously mounted npc or None.
*/
static PyObject* wpChar_unmount( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;
	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (!player) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyGetCharObject(player->unmount());
}

/*
	\method char.equip
	\description Forces the character to equip a given item.
	\param item The item that should be equipped.
*/
static PyObject* wpChar_equip( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgItem( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	P_ITEM pItem = getArgItem( 0 );
	
	if( pItem )
		self->pChar->wear( pItem );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.getbankbox
	\description Get the bankbox of the character and autocreate it if
	neccesary.
	\return The bankbox item object. If this method returns None, 
	something went <b>really</b> wrong.
*/
static PyObject* wpChar_getbankbox( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if ( !player )
		return PyFalse;

	return PyGetItemObject( player->getBankbox() );
}

/*
	\method char.getbackpack
	\description Get the backpack of the character and autocreate it if
	neccesary.
	\return The backpack item object. If this method returns None, 
	something went <b>really</b> wrong.
*/
static PyObject* wpChar_getbackpack( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	return PyGetItemObject( self->pChar->getBackpack() );
}

/*
	\method char.movingeffect
	\description Shows an effect that moves from the character toward another object or location.
	\param id The effect item id.
	\param target The effect target. May either be an item, a character or a coord object.
	\param fixeddirection Defaults to true.
	If false, the animated object will turn towards the target. This is used
	for arrows, fireballs and similar effects.
	\param explodes Defaults to false.
	If this is true, an explosion will be shown when the effect reaches 
	its target.
	\param speed Defaults to 10.
	This is the speed of the moving effect.
	\param color Defaults to 0.
	This is the color of the moving effect.
	\param rendermode Defaults to 0.
	This is a special rendermode for the moving effect.
	Valid values are unknown.
*/
static PyObject* wpChar_movingeffect(wpChar* self, PyObject* args) {
	PyObject *target;
	unsigned short id;
	
	// Optional Arguments	
	unsigned char fixedDirection = 1;
	unsigned char explodes = 0;
	unsigned char speed = 10;
	unsigned int hue = 0;
	unsigned int renderMode = 0;

	if (!PyArg_ParseTuple(args, 
		"HO|BBBII:char.movingeffect(id, target, [fixedDirection], [explodes], [speed], [hue], [rendermode])", 
		&id, &target, &fixedDirection, &explodes, &speed, &hue, &renderMode)) {
		return 0;
	}

	// Coordinates or Object accepted
	if (checkWpCoord(target)) {
		Coord_cl coord = getWpCoord(target);
		self->pChar->effect(id, coord, fixedDirection, explodes, speed, hue, renderMode);
	} else if (checkWpChar(target)) {
		self->pChar->effect(id, getWpChar(target), fixedDirection, explodes, speed, hue, renderMode);
	} else if (checkWpItem(target)) {
		self->pChar->effect(id, getWpItem(target), fixedDirection, explodes, speed, hue, renderMode);
	} else {
		PyErr_SetString(PyExc_TypeError, "First argument has to be an UO object or a position.");
		return 0;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.effect
	\description Show an effect that moves along with the character.
	\param id The effect item id.
	\param speed Defaults to 5.
	This is the animation speed of the effect.
	\param duration Defaults to 10.
	This is how long the effect should be visible.
	\param hue Defaults to 0.
	This is the color for the effect.
	\param rendermode Defaults to 0.
	This is a special rendermode for the effect.
	Valid values are unknown.
*/
static PyObject* wpChar_effect( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgInt( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}
	
	UINT16 id = getArgInt( 0 );

	// Optional Arguments
	UINT8 speed = 5;
	UINT8 duration = 10;
	UINT16 hue = 0;
	UINT16 renderMode = 0;

	if (!PyArg_ParseTuple(args, 
		"H|BBHH:char.effect(id, [speed], [duration], [hue], [rendermode])", 
		&id, &speed, &duration, &hue, &renderMode)) {
		return 0;
	}
	
	self->pChar->effect( id, speed, duration, hue, renderMode );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.dispel
	\description Dispels all effects on this character marked as dispellable.
*/
/*
	\method char.dispel
	\description Dispel a certain kind of effect.
	\param source The character who is responsible for dispelling.
	\param force Defaults to false.
	If this is true, all effects matching the dispelid will be dispelled,
	even if they're not marked as dispellable.
	\param dispelid Defaults to an empty string.
	If this string is empty, all effects will be affected. Otherwise dispel 
	will only affect effects that have the given dispel id.
	\param dispelargs Defaults to an empty list.
	This list of parameters will be passed on to the dispel callback specified
	when applying the effect to this character.
*/
static PyObject* wpChar_dispel( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	// We now have the list of effects applied to pChar
	// Now check if we force dispelling and only have two arguments
	if( PyTuple_Size( args ) == 0 )
	{
		TempEffects::instance()->dispel( self->pChar, 0, false );
	}
	else
	{
		// Iterate trough the list of tempeffects
		cBaseChar::EffectContainer effects = self->pChar->effects();

		P_CHAR pSource = getArgChar( 0 );
		bool force = checkArgInt( 1 ) && ( getArgInt( 1 ) != 0 );
		QString dispelid;
		PyObject *dispelargs = 0;

		if( checkArgStr( 2 ) )
			dispelid = getArgStr( 2 );

		if( PyTuple_Size( args ) > 3 && PyList_Check( PyTuple_GetItem( args, 3 ) ) )
			dispelargs = PyList_AsTuple( PyTuple_GetItem( args, 3 ) );
		
		if( !dispelargs )
			dispelargs = PyTuple_New( 0 );

		if (cPythonScript::canChainHandleEvent(EVENT_DISPEL, self->pChar->getEvents())) {
			PyObject *source;
			if (pSource) {
				source = pSource->getPyObject();
			} else {
				Py_INCREF(Py_None);
				source = Py_None;
			}

			const char *ptype = "";
			if (!dispelid.isEmpty()) {
				ptype = dispelid.latin1();
			}

			PyObject *args = Py_BuildValue("(NNBBsN", self->pChar->getPyObject(), source, 
				0, force ? 1 : 0, ptype, dispelargs);
			bool result = cPythonScript::callChainedEventHandler(EVENT_DISPEL,self->pChar->getEvents(), args);
			Py_DECREF(args);

			if (result) {
				Py_INCREF(Py_None);
				return Py_None;
			}
		}

		for( uint i = 0; i < effects.size(); ++i )
		{
			// No python effect, but we are forcing.
			if( ( force || effects[i]->dispellable ) && dispelid.isNull() && effects[i]->objectID() != "cPythonEffect" )
			{
				effects[i]->Dispel( pSource, false );
				self->pChar->removeEffect( effects[i] );
				TempEffects::instance()->erase( effects[i] );
			}
			// We are dispelling everything and this is a python effect
			else if( ( force || effects[i]->dispellable ) && dispelid.isNull() && effects[i]->objectID() == "cPythonEffect" )
			{
				// At least use the specific dispel function
				cPythonEffect *pEffect = dynamic_cast< cPythonEffect* >( effects[i] );
				if( pEffect )
				{
					pEffect->Dispel( pSource, dispelargs );
					self->pChar->removeEffect( effects[i] );
					TempEffects::instance()->erase( effects[i] );
				}
			}
			// We are dispelling specific python effects
			else if( ( force || effects[i]->dispellable ) && effects[i]->objectID() == "cPythonEffect" && !dispelid.isNull() )
			{
				cPythonEffect *pEffect = dynamic_cast< cPythonEffect* >( effects[i] );
				if( pEffect && pEffect->dispelId() == dispelid )
				{
					pEffect->Dispel( pSource, dispelargs );
					self->pChar->removeEffect( effects[i] );
					TempEffects::instance()->erase( effects[i] );
				}
			}
			
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.addtimer
	\description Add a timed effect to this character.
	\param expiretime The time in miliseconds until this effect expires and the given expire function is called.
	\param expirecallback The full name of the function (preceding the name of the script and modules it is in) that should be called 
	when the effect expires. The prototype for this function is:
	<code>def expire_callback(char, args):
		&nbsp;&nbsp;pass</code>
	Char is the character the effect was applied to. Args is the list of custom arguments you passed to addtimer.
	\param arguments A list of arguments that should be passed on to the effect. Please note that you should only pass on 
	strings, integers and floats because they are the only objects that can be saved to the worldfile. If you want to pass on
	items or characters, please pass the serial instead and use the findchar and finditem functions in the <library id="wolfpack">wolfpack</library>
	library.
	\param serializable Defaults to false. 
	If this is true, the effect will be saved to the worldfile as well. Otherwise the effect will be lost when the server is
	shutted down or crashes.
	\param dispellable Defaults to false.
	This flag indicates that the effect can be dispelled by a normal dispel spell.
	\param dispelname Defaults to an empty string.
	This string is an identifier you need to specify if you later want to dispel this specific effect. It should be an identifier
	unique to this type of effect.
	\param dispelcallback Defaults to an empty string.
	Like the expirecallback this string is the full name of a function including script and modules it may be in. It's called when the 
	effect is dispelled by any means. The prototype for this function is:	
	<code>def dispel_callback(char, args, source, dispelargs):
		&nbsp;&nbsp;pass</code>
	Char is the character the effect was applied to, args are the custom arguments you passed to addtimer. 
	Source is the character responsible for the dispelling of the effect, but may also be None. For a 
	description of dispelargs see the description of the dispel method.
*/
static PyObject* wpChar_addtimer( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	// Three arguments
	if( PyTuple_Size( args ) < 3 || !checkArgInt( 0 ) || !checkArgStr( 1 ) || !PyList_Check( PyTuple_GetItem( args, 2 ) ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	UINT32 expiretime = getArgInt( 0 );
	QString function = getArgStr( 1 );
	PyObject *py_args = PyList_AsTuple( PyTuple_GetItem( args, 2 ) );

	cPythonEffect *effect = new cPythonEffect( function, py_args );
	
	// Should we save this effect?
	if( checkArgInt( 3 ) && getArgInt( 3 ) != 0 ) 
		effect->setSerializable( true );
	else
		effect->setSerializable( false );
	
	// dispellable
	if( checkArgInt( 4 ) && getArgInt( 4 ) != 0 )
		effect->dispellable = true;
	else
		effect->dispellable = false;

	// dispelname
	if( checkArgStr( 5 ) )
		effect->setDispelId( getArgStr( 5 ) );

	// dispelfunc
	if( checkArgStr( 6 ) )
		effect->setDispelFunc( getArgStr( 6 ) );

	effect->setDest( self->pChar->serial() );
	effect->setExpiretime_ms( expiretime );
	TempEffects::instance()->insert( effect );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.maywalk
	\description Check if the character could walk from his present location to the given coordinates.
	\param x The x component of the coordinate.
	\param y The y component of the coordinate.
	\param z The z component of the coordinate.
	\param map The map component of the coordinate.
	\return True if the character can walk there, false otherwise.
*/
static PyObject* wpChar_maywalk( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	// Four Arguments: x,y,z,map
	if( !checkArgInt( 0 ) || !checkArgInt( 1 ) || !checkArgInt( 2 ) || !checkArgInt( 3 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	//if( !mayWalk( self->pChar, Coord_cl( getArgInt( 0 ), getArgInt( 1 ), getArgInt( 2 ), getArgInt( 3 ) ) ) )
	int argx = getArgInt( 0 );
	int argy = getArgInt( 1 );
	int argz = getArgInt( 2 );
	int argmap = getArgInt( 3 );
	Coord_cl argcoord( argx, argy, argz, argmap );
	if( !mayWalk( self->pChar, argcoord ) )
		return PyFalse;
	else
		return PyTrue;
}

/*
	\method char.iscriminal
	\description Check if the character is a criminal.
	\return True if the character is criminal, false otherwise.
*/
static PyObject* wpChar_iscriminal( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	return self->pChar->isCriminal() ? PyTrue : PyFalse;
}

/*
	\method char.delete
	\description Delete the character.
*/
static PyObject* wpChar_delete( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	
	cCharStuff::DeleteChar(self->pChar);

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.ismurderer
	\description Check if the character is a murderer.
	\return True if the character is a murderer, false otherwise.
*/
static PyObject* wpChar_ismurderer( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	return self->pChar->isMurderer() ? PyTrue : PyFalse;
}

/*
	\method char.criminal
	\description Flag the player as a criminal. This method only works for players and does nothing for NPCs.
*/
static PyObject* wpChar_criminal( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (!player) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	player->makeCriminal();

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.reveal
	\description Reveal the character if he's currently hidden.
*/
static PyObject* wpChar_reveal( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);

	self->pChar->unhide();

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.fight
	\description Initiate a fight between this character and another.
	\param target Another character.
*/
static PyObject* wpChar_fight( wpChar* self, PyObject* args ) {
	if (!checkArgChar(0)) {
		PyErr_BadArgument();
		return 0;
	}

	P_CHAR pChar = getArgChar(0);

	if (self->pChar == pChar) {
		return PyFalse;
	}

	if (self->pChar->fight(pChar)) {
		return PyTrue;
	} else {
		return PyFalse;
	}
}

/*
	\method char.follow
	\description This method only works for NPCs. Let this character
	follow another player.
	\param player The player who should be followed.
*/
static PyObject* wpChar_follow( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgChar( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	P_PLAYER pChar = dynamic_cast<P_PLAYER>( getArgChar( 0 ) );

	if( !pChar || pChar == self->pChar )
		return PyFalse;

	P_NPC npc = dynamic_cast<P_NPC>( self->pChar );
	if ( !npc )
		return PyFalse;

	pChar->addPet( npc );
	npc->setWanderType( enFollowTarget );
	npc->setWanderFollowTarget( pChar );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.disturb
	\description This method only works for players. 
	Disturb the player at whatever he is doing. Meditation for instance.
*/
static PyObject* wpChar_disturb( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

	if (!player) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	player->disturbMed();

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.goto
	\description This method only works for NPCs. Let the character
	go to a given position.
	\param target A coordinate object giving the position the NPC should move to.
*/
static PyObject* wpChar_goto( wpChar* self, PyObject* args )
{
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	if( !checkArgCoord( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	Coord_cl pos = getArgCoord( 0 );

	if( pos.map != self->pChar->pos().map )
	{
		PyErr_Warn( PyExc_Warning, "Cannot move to a different map using goto." );
		return PyFalse;
	}

	P_NPC npc = dynamic_cast<P_NPC>( self->pChar );

	if (!npc) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	npc->setWanderType( enDestination );
	npc->setWanderDestination( pos );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.updateflags
	\description This method will resend the flags for this character. Please be sure 
	to only use this if you are sure that the flags have changed. If the flags didn't change
	and you use this method, all clients in range will see the character walking one 
	cell forward, although he didn't do serverside.
*/
static PyObject* wpChar_updateflags( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pChar || self->pChar->free )
		return PyFalse;

	self->pChar->update();
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.notoriety
	\description Calculate the notoriety value toward another character or a generic 
	notoriety value if no target is given.
	\param target The character you want to calculate the notoriety value with.
	\return The notoriety, which is one of the following values:
	<code>0x01 - Innocent
	0x02 - Guild ally
	0x03 - Attackable but not criminal
	0x04 - Criminal
	0x05 - Guild enemy
	0x06 - Murderer
	0x07 - Invulnerable</code>
*/
static PyObject* wpChar_notoriety( wpChar* self, PyObject* args )
{
	if (self->pChar->free) {
		return PyFalse;
	}

	P_CHAR target = 0;

	if (!PyArg_ParseTuple( args, "|O&:char.notoriety([char])", PyConvertChar, &target))
		return 0;

	if (!target) {
		target = self->pChar;
	}

	return PyInt_FromLong(self->pChar->notoriety(target));
}

/*
	\method char.canreach
	\description Check if the character can reach the given object or coordinate and if he is in
	within a given range.
	\param target The object the character wants to reach. This can either be an item, a character or a 
	coordinate object.
	\param range The range the character needs to be within.
	\return True if the character can reach the given object, false otherwise.
*/
static PyObject* wpChar_canreach( wpChar* self, PyObject* args )
{
	Q_UNUSED(args);
	if( self->pChar->free || ( !checkArgObject( 0 ) && !checkArgCoord( 0 ) ) || !checkArgInt( 1 ) )
		return PyFalse;

	P_PLAYER pPlayer = dynamic_cast< P_PLAYER >( self->pChar );

	if( pPlayer && pPlayer->isGM() )
		return PyTrue;

	Coord_cl pos;

	P_ITEM pItem = 0;
	P_CHAR pChar = 0;

	// Parameter 1: Coordinate
	if( checkArgCoord( 0 ) )
	{
		pos = getArgCoord( 0 );
	
	// Parameter1: Item/Char
	} else {	
		pChar = getArgChar( 0 );

		if(!pChar) {
			pItem = getArgItem( 0 );
			if (!pItem) {
				Py_INCREF(Py_False);
				return Py_False;
			}

			if (pItem->getOutmostChar() == self->pChar) {
				Py_INCREF(Py_True);
				return Py_True;
			}

			pos = pItem->pos();
		} else {
			pos = pChar->pos();
		}		
	}

	UINT32 range = getArgInt( 1 );

	if( self->pChar->pos().map != pos.map )
		return PyFalse;

	if( self->pChar->pos().distance( pos ) > range )
		return PyFalse;

	if (pItem) {
		if (!self->pChar->lineOfSight(pItem, true)) {
			Py_INCREF(Py_False);
			return Py_False;
		}
	} else if (pChar) {
		if (!self->pChar->lineOfSight(pChar, true)) {
			Py_INCREF(Py_False);
			return Py_False;
		}
	} else {
		if (!self->pChar->lineOfSight(pos, true)) {
			Py_INCREF(Py_False);
			return Py_False;
		}
	}
	return PyTrue;
}

/*
	\method char.canpickup
	\description Check if the character can pick up a given item. This
	works only for players.
	\param item The item the player wants to pick up.
	\return True if the character can pick up the item. False otherwise.
*/
static PyObject* wpChar_canpickup( wpChar* self, PyObject* args )
{
	if( self->pChar->free )
		return PyFalse;

	if( PyTuple_Size( args ) != 1 )
	{
		PyErr_BadArgument();
		return 0;
	}

	P_ITEM pItem = getArgItem( 0 );

	P_PLAYER pPlayer = dynamic_cast< P_PLAYER >( self->pChar );

	if( !pPlayer )
		return PyFalse;

	return pPlayer->canPickUp( pItem ) ? PyTrue : PyFalse;
}

/*
	\method char.cansee
	\description Check if the character can see another character or an item.
	This is for checking if the character can see an invisible or hidden object.
	\param object The object you want to check. This is either an item or a character.
	\return True if the object can be seen by the player, false otherwise.
*/
static PyObject* wpChar_cansee( wpChar *self, PyObject *args )
{
	if( self->pChar->free )
		return PyFalse;

	PyObject *object = 0;

	if (!PyArg_ParseTuple(args, "O:char.cansee([char,item])", &object))
		return 0;

	bool result = false;

	// Item
	if (checkWpItem(object)) {
		result = self->pChar->canSee(getWpItem(object));

	// Char
	} else if (checkWpChar(object)) {
		result = self->pChar->canSee(getWpChar(object));
	} else {
		PyErr_SetString(PyExc_TypeError, "Argument types required: char, item");
		return 0;
	}

	return result ? PyTrue : PyFalse;
}

/*
	\method char.lightning
	\description Show a lightning animation striking the character.
	\param hue Defaults to 0.
	The color of the lightning bolt.
*/
static PyObject* wpChar_lightning( wpChar *self, PyObject *args )
{
	unsigned short hue = 0;
	
	if( !PyArg_ParseTuple( args, "|h:char.lightning( [hue] )", &hue ) )
		return 0;

	self->pChar->lightning( hue );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.resendtooltip
	\description Resend the tooltip of this character to surrounding clients.
*/
static PyObject* wpChar_resendtooltip( wpChar *self, PyObject *args )
{
	Q_UNUSED(args);
	if( self->pChar->free )
		return PyFalse;

	self->pChar->resendTooltip();
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.additem
	\description Add an item to a certain layer of this player.
	Normally you should use equip instead.
	\param layer The layer you want to add the item to.
	\param item The item you want to add there.
	\param handleweight Defaults to true.
	Should the totalweight of the player be modified accordingly.
	\param noremove Defaults to false.
	If this is true, the item should not be removed from the previous container. Handle this with extreme care!
*/
static PyObject* wpChar_additem( wpChar *self, PyObject *args )
{
	if( self->pChar->free )
		return PyFalse;

	if( !checkArgInt( 0 ) && !checkArgItem( 1 ) )
	{
		PyErr_BadArgument();
		return 0;
	}
	bool handleWeight = true;
	bool noRemove = false;

	if( PyTuple_Size( args ) > 2 && checkArgInt( 2 ) )
		handleWeight = getArgInt( 2 ) > 0?true:false;

	if( PyTuple_Size( args ) > 2 && checkArgInt( 3 ) )
		noRemove = getArgInt( 3 ) > 0?true:false;

	int layer = getArgInt( 0 );
	P_ITEM pItem = getArgItem( 1 );
	
	if (!pItem) {
		Py_INCREF(Py_None);
		return Py_None;		
	}

	self->pChar->addItem( (cBaseChar::enLayer)layer, pItem, handleWeight, noRemove );

	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.vendorbuy
	\description Show the buy dialog for this NPC to a player.
	\param player The player you want to show the dialog to.
*/
static PyObject* wpChar_vendorbuy( wpChar *self, PyObject* args )
{
	if ( self->pChar->free )
		return PyFalse;
	if ( !checkArgChar( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	P_PLAYER player = dynamic_cast<P_PLAYER>( getArgChar( 0 ) );
	P_NPC npc = dynamic_cast<P_NPC>( self->pChar );
	
	if ( !player || !npc ) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	npc->vendorBuy( player );
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.vendorsell
	\description Show the sell dialog for this NPC to a player.
	\param player The player you want to show the dialog to.
*/
static PyObject* wpChar_vendorsell( wpChar *self, PyObject* args )
{
	if ( self->pChar->free )
		return PyFalse;
	if ( !checkArgChar( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	P_PLAYER player = dynamic_cast<P_PLAYER>( getArgChar( 0 ) );
	P_NPC npc = dynamic_cast<P_NPC>( self->pChar );
	
	if (!player || !npc) {
		Py_INCREF(Py_None);
		return Py_None;		
	}

	npc->vendorSell( player );
		Py_INCREF(Py_None);
		return Py_None;
}

/*
	\method char.aiengine
	\description Get the ai engine associated with this NPC.
	This only works for NPCs and returns None otherwise
	\return The Ai object used by this npc.
*/
static PyObject* wpChar_aiengine( wpChar* self, PyObject* args )
{
	Q_UNUSED( args );
	P_NPC npc = dynamic_cast<P_NPC>( self->pChar );
	if ( npc )
	{
		return PyGetAIObject( npc->ai() );
	}
	Py_INCREF( Py_None );
	return Py_None;
}

/*
	\method char.log
	\description Log an event associated with this character.
	If the character is a connected player, his socket id will be 
	prepended to the message.
	\param level The loglevel to use for the message. Possible constants from <library id="wolfpack.consts">wolfpack.consts</library> are:
	<code>0x00 LOG_MESSAGE
0x01 LOG_ERROR
0x02 LOG_PYTHON
0x03 LOG_WARNING
0x04 LOG_NOTICE
0x05 LOG_TRACE
0x06 LOG_DEBUG</code>
	\param message The message you want to send to the logfile and console. Please make sure to include a newline (\n) at the end.
*/
static PyObject *wpChar_log(wpChar *self, PyObject *args) {	
	char *message;
	unsigned int loglevel;

	if (!PyArg_ParseTuple(args, "ies:char.log(level, message)", &loglevel, "utf-8", &message)) {
		return 0;
	}

	self->pChar->log((eLogLevel)loglevel, message);
	PyMem_Free(message);
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.removeevent
	\description Remove a python script from the event chain for this object.
	\param event The id of the python script you want to remove from the event chain.
*/
static PyObject *wpChar_removeevent(wpChar *self, PyObject *args) {	
	char *event;
	if (!PyArg_ParseTuple(args, "s:char.removeevent(name)", &event)) {
		return 0;
	}
	self->pChar->removeEvent(event);
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.addevent
	\description Add a pythonscript to the event chain of this object.
	Does nothing if the object already has that event.
	\param event The id of the python script you want to add to the event chain.
*/
static PyObject *wpChar_addevent(wpChar *self, PyObject *args) {	
	char *event;
	if (!PyArg_ParseTuple(args, "s:char.addevent(name)", &event)) {
		return 0;
	}

	cPythonScript *script = ScriptManager::instance()->find(event);

	if (!script) {
		PyErr_Format(PyExc_RuntimeError, "No such script: %s", event);
		return 0;
	}

	self->pChar->addEvent(script);
	Py_INCREF(Py_None);
	return Py_None;
}

/*
	\method char.hasevent
	\description Check if this object has a python script in its event chain.
	\param event The id of the python script you are looking for.
	\return True of the script is in the chain. False otherwise.
*/
static PyObject *wpChar_hasevent(wpChar *self, PyObject *args) {	
	char *event;
	if (!PyArg_ParseTuple(args, "s:char.hasevent(name)", &event)) {
		return 0;
	}

	if (self->pChar->hasEvent(event)) {
		Py_INCREF(Py_True);
		return Py_True;
	} else {
		Py_INCREF(Py_False);
		return Py_False;
	}
}

/*
	\method char.callevent
	\description Call a python event chain for this object. Ignore global hooks.
	\param event The id of the event you want to call. See <library id="wolfpack.consts">wolfpack.consts</library> for constants.
	\param args A tuple of arguments you want to pass to this event handler.
	\return The result of the first handling event.
*/
static PyObject *wpChar_callevent(wpChar *self, PyObject *args) {	
	unsigned int event;
	PyObject *eventargs;

	if (!PyArg_ParseTuple(args, "IO!:char.callevent(event, args)", &event, &PyTuple_Type, &eventargs)) {
		return 0;
	}

	if (cPythonScript::canChainHandleEvent((ePythonEvent)event, self->pChar->getEvents())) {
		bool result = cPythonScript::callChainedEventHandler((ePythonEvent)event, self->pChar->getEvents(), eventargs);

		if (result) {
			Py_INCREF(Py_True);
			return Py_True;
		}
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyMethodDef wpCharMethods[] = 
{
	{ "moveto",			(getattrofunc)wpChar_moveto,			METH_VARARGS, "Moves the character to the specified location." },
	{ "resurrect",		(getattrofunc)wpChar_resurrect,			METH_VARARGS, "Resurrects the character." },
	{ "kill",			(getattrofunc)wpChar_kill,				METH_VARARGS, "This kills the character." },
	{ "damage",			(getattrofunc)wpChar_damage,			METH_VARARGS, "This damages the current character." },
  { "update",			(getattrofunc)wpChar_update,			METH_VARARGS, "Resends the char to all clients in range." },
	{ "resendtooltip",	(getattrofunc)wpChar_resendtooltip,		METH_VARARGS, "Resends the tooltip for this character." },
	{ "updateflags",	(getattrofunc)wpChar_updateflags,		METH_VARARGS, "Resends the character if flags have changed (take care, this might look like a move)." },
	{ "removefromview", (getattrofunc)wpChar_removefromview,	METH_VARARGS, "Removes the char from all surrounding clients." },
	{ "message",		(getattrofunc)wpChar_message,			METH_VARARGS, "Displays a message above the characters head - only visible for the player." },
	{ "soundeffect",	(getattrofunc)wpChar_soundeffect,		METH_VARARGS, "Plays a soundeffect for the character." },
	{ "distanceto",		(getattrofunc)wpChar_distanceto,		METH_VARARGS, "Distance to another object or a given position." },
	{ "action",			(getattrofunc)wpChar_action,			METH_VARARGS, "Lets the char perform an action." },
	{ "directionto",	(getattrofunc)wpChar_directionto,		METH_VARARGS, "Distance to another object or a given position." },
	{ "checkskill",		(getattrofunc)wpChar_checkskill,		METH_VARARGS, "Performs a skillcheck for the character." },
	{ "itemonlayer",	(getattrofunc)wpChar_itemonlayer,		METH_VARARGS, "Returns the item currently weared on a specific layer, or returns none." },
	{ "combatskill",	(getattrofunc)wpChar_combatskill,		METH_VARARGS, "Returns the combat skill the character would currently use." },
	{ "getweapon",		(getattrofunc)wpChar_getweapon,			METH_VARARGS, "What weapon does the character currently wear." },
	{ "useresource",	(getattrofunc)wpChar_useresource,		METH_VARARGS, "Consumes a resource posessed by the char." },
	{ "countresource",	(getattrofunc)wpChar_countresource,		METH_VARARGS, "Counts the amount of a certain resource the user has." },
	{ "emote",			(getattrofunc)wpChar_emote,				METH_VARARGS, "Shows an emote above the character." },
	{ "say",			(getattrofunc)wpChar_say,				METH_VARARGS|METH_KEYWORDS, "The character begins to talk." },
	{ "turnto",			(getattrofunc)wpChar_turnto,			METH_VARARGS, "Turns towards a specific object and resends if neccesary." },
	{ "equip",			(getattrofunc)wpChar_equip,				METH_VARARGS, "Equips a given item on this character." },
	{ "maywalk",		(getattrofunc)wpChar_maywalk,			METH_VARARGS, "Checks if this character may walk to a specific cell." },
	{ "sound",			(getattrofunc)wpChar_sound,				METH_VARARGS, "Play a creature specific sound." },
	{ "disturb",		(getattrofunc)wpChar_disturb,			METH_VARARGS, "Disturbs whatever this character is doing right now." },
	{ "canreach",		(getattrofunc)wpChar_canreach,			METH_VARARGS, "Checks if this character can reach a certain object." },
	{ "notoriety",		(getattrofunc)wpChar_notoriety,			METH_VARARGS, "Returns the notoriety of a character toward another character." },
	{ "canpickup",		(getattrofunc)wpChar_canpickup,			METH_VARARGS, NULL },
	{ "cansee",			(getattrofunc)wpChar_cansee,			METH_VARARGS, NULL },
	{ "lightning",		(getattrofunc)wpChar_lightning,			METH_VARARGS, NULL },
	{ "log",			(getattrofunc)wpChar_log,				METH_VARARGS, NULL },
	{ "additem",		(getattrofunc)wpChar_additem,			METH_VARARGS, "Creating item on specified layer."},
	
	// Mostly NPC functions
	{ "fight",			(getattrofunc)wpChar_fight,			METH_VARARGS, "Let's the character attack someone else." },
	{ "goto",			(getattrofunc)wpChar_goto,				METH_VARARGS, "The character should go to a coordinate." },
	{ "follow",			(getattrofunc)wpChar_follow,			METH_VARARGS, "The character should follow someone else." },
	{ "vendorbuy",		(getattrofunc)wpChar_vendorbuy,			METH_VARARGS, 0 },
	{ "vendorsell",		(getattrofunc)wpChar_vendorsell,		METH_VARARGS, 0 },
	{ "aiengine",		(getattrofunc)wpChar_aiengine,			METH_VARARGS, 0	},

	{ "addtimer",		(getattrofunc)wpChar_addtimer,			METH_VARARGS, "Adds a timer to this character." },
	{ "dispel",			(getattrofunc)wpChar_dispel,			METH_VARARGS, "Dispels this character (with special options)." },

	// Event handling functions
	{ "addevent",		(getattrofunc)wpChar_addevent,			METH_VARARGS, 0},
	{ "removeevent",	(getattrofunc)wpChar_removeevent,		METH_VARARGS, 0},
	{ "hasevent",		(getattrofunc)wpChar_hasevent,			METH_VARARGS, 0},
	{ "callevent",		(getattrofunc)wpChar_callevent,			METH_VARARGS, 0},

	// Update Stats
	{ "updatestats",	(getattrofunc)wpChar_updatestats,		METH_VARARGS, "Resends other stats to this character." },
	{ "updatemana",		(getattrofunc)wpChar_updatemana,		METH_VARARGS, "Resends the manabar to this character." },
	{ "updatestamina",	(getattrofunc)wpChar_updatestamina,		METH_VARARGS, "Resends the stamina bar to this character." },
	{ "updatehealth",	(getattrofunc)wpChar_updatehealth,		METH_VARARGS, "Resends the healthbar to the environment." },

	// Mount/Unmount
	{ "unmount",		(getattrofunc)wpChar_unmount,			METH_VARARGS, "Unmounts this character and returns the character it was previously mounted." },
	{ "mount",			(getattrofunc)wpChar_mount,				METH_VARARGS, "Mounts this on a specific mount." },

	// Effects
	{ "movingeffect",	(getattrofunc)wpChar_movingeffect,		METH_VARARGS, "Shows a moving effect moving toward a given object or coordinate." },
	{ "effect",			(getattrofunc)wpChar_effect,			METH_VARARGS, "Shows an effect staying with this character." },

	// Bank/Backpack
	{ "getbankbox",		(getattrofunc)wpChar_getbankbox,		METH_VARARGS,	"Gets and autocreates a bankbox for the character." },
	{ "getbackpack",	(getattrofunc)wpChar_getbackpack,		METH_VARARGS, "Gets and autocreates a backpack for the character." },

	// Follower System
	{ "addfollower",	(getattrofunc)wpChar_addfollower,		METH_VARARGS, "Adds a follower to the user." },
	{ "removefollower",	(getattrofunc)wpChar_removefollower,	METH_VARARGS, "Removes a follower from the user." },
	{ "hasfollower",	(getattrofunc)wpChar_hasfollower,		METH_VARARGS, "Checks if a certain character is a follower of this." },
	{ "reveal", (getattrofunc)wpChar_reveal, METH_VARARGS, 0 },

	// Tag System
	{ "gettag",			(getattrofunc)wpChar_gettag,			METH_VARARGS, "Gets a tag assigned to a specific char." },
	{ "settag",			(getattrofunc)wpChar_settag,			METH_VARARGS, "Sets a tag assigned to a specific char." },
	{ "hastag",			(getattrofunc)wpChar_hastag,			METH_VARARGS, "Checks if a certain char has the specified tag." },
	{ "deltag",			(getattrofunc)wpChar_deltag,			METH_VARARGS, "Deletes the specified tag." },

	// Reputation System
	{ "iscriminal",		(getattrofunc)wpChar_iscriminal,		METH_VARARGS, "Is this character criminal.." },
	{ "ismurderer",		(getattrofunc)wpChar_ismurderer,		METH_VARARGS, "Is this character a murderer." },
	{ "criminal",		(getattrofunc)wpChar_criminal,			METH_VARARGS, "Make this character criminal." },
	{ "delete",			(getattrofunc)wpChar_delete,			METH_VARARGS, 0 },

	// Is*? Functions
	{ "isitem",			(getattrofunc)wpChar_isitem,			METH_VARARGS, "Is this an item." },
	{ "ischar",			(getattrofunc)wpChar_ischar,			METH_VARARGS, "Is this a char." },

    { NULL, NULL, 0, NULL }
};

// Getters & Setters
PyObject *wpChar_getAttr( wpChar *self, char *name )
{
	// Python specific stuff
	/*
		\rproperty char.gm Indicates whether the player is a gm or not. 
		Always false for NPCs.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	if ( !strcmp( "gm", name ) )
	{
		P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );
	
		if (!player)
			return PyFalse;

		return player->isGM() ? PyTrue : PyFalse;
		
	/*
		\rproperty char.tags This property is a list of names for all tags attached to this character.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	} else if (!strcmp("tags", name)) {
		// Return a list with the keynames
		PyObject *list = PyList_New(0);

		QStringList tags = self->pChar->getTags();
		for (QStringList::iterator it = tags.begin(); it != tags.end(); ++it) {
			QString name = *it;
			if (!name.isEmpty()) {
				PyList_Append(list, PyString_FromString(name.latin1()));
			}
		}

		return list;
		
	/*
		\rproperty char.party A <object id="PARTY">PARTY</object> object for the party the player belongs to.
		None for NPCs or if the player is not in a party.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	} else if (!strcmp("party", name)) {
		P_PLAYER player = dynamic_cast<P_PLAYER>(self->pChar);

		if (player && player->party()) {
			return player->party()->getPyObject();
		}

		Py_INCREF(Py_None);
		return Py_None;
		
	/*
		\rproperty char.guild A <object id="GUILD">GUILD</object> object for the guild the player belongs to.
		None for NPCs or if the player is not in a guild.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	} else if (!strcmp("guild", name)) {
		P_PLAYER player = dynamic_cast<P_PLAYER>(self->pChar);

		if (player && player->guild()) {
			return player->guild()->getPyObject();
		}

		Py_INCREF(Py_None);
		return Py_None;
		
	/*
		\rproperty char.rank The rank for the players account.
		NPCs and players without accounts always have rank 1.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	} else if( !strcmp("rank", name)) {
		P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );
	
		if (!player)
			return PyInt_FromLong(1);

		cAccount *account = player->account();

		if (account) {
			return PyInt_FromLong(account->rank());
		} else {
			return PyInt_FromLong(1);
		}
		
	/*
		\rproperty char.region A <object id="REGION">REGION</object> object for the region the character is in.
		May be None if the region the character is in is undefined.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	} else if( !strcmp( "region", name ) )
		return PyGetRegionObject( self->pChar->region() );

	/*
		\rproperty char.account An <object id="ACCOUNT">ACCOUNT</object> object for the players account.
		None for NPCs and players without accounts.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "account", name ) )
	{
		P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );
		if ( !player )
		{
			Py_INCREF( Py_None );
			return Py_None;
		}
		return PyGetAccountObject( player->account() );
	}
	/*
		\rproperty char.socket The <object id="SOCKET">SOCKET</object> object for this character.
		None for disconnected players and NPCs.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "socket", name ) )
	{
		P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

		if ( !player )
		{
			Py_INCREF( Py_None );
			return Py_None;
		}
		return PyGetSocketObject( player->socket() );
	}
	/*
		\rproperty char.skill Returns a <object id="SKILL">SKILL</object> object in value mode for the character.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "skill", name ) )
	{
		wpSkills *skills = PyObject_New( wpSkills, &wpSkillsType );
		skills->pChar = self->pChar;
		skills->type = 0;
		return (PyObject*)( skills );
	}
	
	/*
		\rproperty char.skillcap Returns a <object id="SKILL">SKILL</object> object in cap mode for the character.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "skillcap", name ) )
	{
		wpSkills *skills = PyObject_New( wpSkills, &wpSkillsType );
		skills->pChar = self->pChar;
		skills->type = 1;
		return (PyObject*)( skills );
	}
	/*
		\rproperty char.skilllock Returns a <object id="SKILL">SKILL</object> object in lock mode for the character.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "skilllock", name ) )
	{
		wpSkills *skills = PyObject_New( wpSkills, &wpSkillsType );
		skills->pChar = self->pChar;
		skills->type = 2;
		return (PyObject*)( skills );
	}
	/*
		\rproperty char.followers Returns the list of followers for this player.
		This property is None for NPCs, not an empty list.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "followers", name ) )
	{
		P_PLAYER player = dynamic_cast<P_PLAYER>( self->pChar );

		if ( !player )
		{
			Py_INCREF( Py_None );
			return Py_None;
		}

		cBaseChar::CharContainer followers = player->pets();
		PyObject *rVal = PyTuple_New( followers.size() );

		for( uint i = 0; i < followers.size(); ++i )
			PyTuple_SetItem( rVal, i, PyGetCharObject( followers[i] ) );

		return rVal;
	}
	/*
		\rproperty char.guards Returns a list of NPCs who are guarding this character.
		This property is valid for NPCs and players.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "guards", name ) )
	{
		cBaseChar::CharContainer guards = self->pChar->guardedby();
		PyObject *rVal = PyTuple_New( guards.size() );

		for( uint i = 0; i < guards.size(); ++i )
			PyTuple_SetItem( rVal, i, PyGetCharObject( guards[i] ) );

		return rVal;
	}
	/*
		\property char.events The names of the scripts attached to this character.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	else if( !strcmp( "events", name ) )
	{
		QStringList events = QStringList::split( ",", self->pChar->eventList() );
		PyObject *list = PyList_New( events.count() );
		for( uint i = 0; i < events.count(); ++i )
			PyList_SetItem( list, i, PyString_FromString( events[i].latin1() ) );
		return list;
	/*
		\rproperty char.npc True if this character is a npc, false otherwise.
		This property is exclusive to python scripts and overrides normal properties with the same name.
	*/
	} else if (!strcmp("npc", name)) {
    return self->pChar->objectType() == enNPC ? PyTrue : PyFalse;
  /*
  	\rproperty char.player True if this character is a player, false otherwise.
  	This property is exclusive to python scripts and overrides normal properties with the same name.
  */
  } else if (!strcmp("player", name)) {
    return self->pChar->objectType() == enPlayer ? PyTrue : PyFalse;
  } else {
		cVariant result;
		stError *error = self->pChar->getProperty( name, result );

		if( !error )
		{
			PyObject *obj = 0;

			switch( result.type() )
			{
			case cVariant::BaseChar:
				obj = PyGetCharObject( result.toChar() );
				break;
			case cVariant::Item:
				obj = PyGetItemObject( result.toItem() );
				break;
			case cVariant::Long:
			case cVariant::Int:
				obj = PyInt_FromLong( result.toInt() );
				break;
			case cVariant::String:
				if( result.toString().isNull() )
					obj = PyUnicode_FromWideChar(L"", 0);
				else
					obj = PyUnicode_FromUnicode((Py_UNICODE*)result.toString().ucs2(), result.toString().length() );
				break;
			case cVariant::Double:
				obj = PyFloat_FromDouble( result.toDouble() );
				break;
			case cVariant::Coord:
				obj = PyGetCoordObject( result.toCoord() );
				break;
			}

			if( !obj )
			{
				PyErr_Format( PyExc_ValueError, "Unsupported Property Type: %s", result.typeName() );
				return 0;
			}

			return obj;
		}
		else
			delete error;
	}

	// If no property is found search for a method
	return Py_FindMethod( wpCharMethods, (PyObject*)self, name );
}

int wpChar_setAttr( wpChar *self, char *name, PyObject *value )
{
	// Special Python things.
	if( !strcmp( "events", name ) )
	{
		if( !PyList_Check( value ) )
		{
			PyErr_BadArgument();
			return -1;
		}

		self->pChar->clearEvents();
		int i;
		for( i = 0; i < PyList_Size( value ); ++i )
		{
			if( !PyString_Check( PyList_GetItem( value, i ) ) )
				continue;

			cPythonScript *script = ScriptManager::instance()->find( PyString_AsString( PyList_GetItem( value, i ) ) );
			if( script )
				self->pChar->addEvent( script );
		}
	}
	else
	{
		cVariant val;
		if( PyString_Check( value ) )
			val = cVariant( PyString_AsString( value ) );
		else if( PyUnicode_Check( value ) )
			val = cVariant(QString::fromUcs2((ushort*)PyUnicode_AsUnicode(value)));
		else if( PyInt_Check( value ) )
			val = cVariant( PyInt_AsLong( value ) );
		else if( checkWpItem( value ) )
			val = cVariant( getWpItem( value ) );
		else if( checkWpChar( value ) )
			val = cVariant( getWpChar( value ) );
		else if( checkWpCoord( value ) )
			val = cVariant( getWpCoord( value ) );
		else if( PyFloat_Check( value ) )
			val = cVariant( PyFloat_AsDouble( value ) );

		if( !val.isValid() )
		{
			if( value->ob_type )
				PyErr_Format( PyExc_TypeError, "Unsupported object type: %s", value->ob_type->tp_name );
			else
				PyErr_Format( PyExc_TypeError, "Unknown object type" );
			return 0;
		}

		stError *error = self->pChar->setProperty( name, val );

		if( error )
		{
			PyErr_Format( PyExc_TypeError, "Error while setting attribute '%s': %s", name, error->text.latin1() );
			delete error;
			return 0;
		}
	}

	return 0;
}

P_CHAR getWpChar( PyObject *pObj )
{
	if( pObj->ob_type != &wpCharType )
		return 0;

	wpChar *item = (wpChar*)( pObj );
	return item->pChar;
}

bool checkWpChar( PyObject *pObj )
{
	if( pObj->ob_type != &wpCharType )
		return false;
	else
		return true;
}

int wpChar_compare( PyObject *a, PyObject *b )
{
	// Both have to be characters
	if( a->ob_type != &wpCharType || b->ob_type != &wpCharType ) 
		return -1;

	P_CHAR pA = getWpChar( a );
	P_CHAR pB = getWpChar( b );

	return !( pA == pB );
}

int PyConvertChar(PyObject *object, P_CHAR* character) {
	if (object->ob_type != &wpCharType) {
		PyErr_BadArgument();
		return 0;
	}

	*character = ((wpChar*)object)->pChar;
	return 1;
}

int PyConvertPlayer(PyObject *object, P_PLAYER* player) {
	if (object->ob_type != &wpCharType) {
		PyErr_BadArgument();
		return 0;
	}

	P_PLAYER temp = dynamic_cast<P_PLAYER>(((wpChar*)object)->pChar);

	if (!temp) {
		return 0;
	}
    
	*player = temp;
	return 1;
}
