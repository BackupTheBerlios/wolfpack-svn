//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//	Copyright 1997, 98 by Marcus Rating (Cironian)
//  Copyright 2001 by holders identified in authors.txt
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
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//	* In addition to that license, if you are running this program or modified
//	* versions of it on a public system you HAVE TO make the complete source of
//	* the version used by you available or provide people with a location to
//	* download it.
//
//
//
//	Wolfpack Homepage: http://wpdev.sf.net/
//========================================================================================

#ifndef __WPPYTHONSCRIPT_H__
#define __WPPYTHONSCRIPT_H__

#include "wpdefaultscript.h"
#include "Python.h"
#include "pyerrors.h"

class WPPythonScript : public WPDefaultScript  
{
private:
	PyObject *codeModule; // This object stores the compiled Python Module

public:
	virtual const QString Type( void ) {
		return "python";
	};
	
	virtual ~WPPythonScript() {};

	virtual void load( const QDomElement &Data );
	virtual void unload( void );

	// Normal Events
	virtual bool onUse( P_CHAR User, P_ITEM Used );

	virtual bool onShowItemName( P_ITEM Item, P_CHAR Viewer );
	virtual bool onShowCharName( P_CHAR Character, P_CHAR Viewer );

	virtual bool onCollideItem( P_CHAR Character, P_ITEM Obstacle );
	virtual bool onCollideChar( P_CHAR Character, P_CHAR Obstacle );
	virtual bool onWalk( P_CHAR Character, UI08 Direction, UI08 Sequence );

	// if this events returns true (handeled) then we should not display the text
	virtual bool onTalk( P_CHAR Character, char speechType, UI16 speechColor, UI16 speechFont, const QString &Text, const QString &Lang );

	virtual bool onTalkToNPC( P_CHAR Talker, P_CHAR Character, const QString &Text );
	virtual bool onTalkToItem( P_CHAR Talker, P_ITEM Item, const QString &Text );

	virtual bool onWarModeToggle( P_CHAR Character, bool War );

	// Is the Client version already known to us here ???
	virtual bool onConnect( UOXSOCKET Socket, const QString &IP, const QString &Username, const QString &Password );

	virtual bool onDisconnect( UOXSOCKET Socket, QString IP );

	virtual bool onEnterWorld( P_CHAR Character );

	virtual bool onHelp( P_CHAR Character );

	virtual bool onChat( P_CHAR Character );

	virtual bool onSkillUse( P_CHAR Character, UI08 Skill );
};

//////////////
// We will define our Python extensions here (!)
void reloadPython( void );
void stopPython( void );
void startPython( int argc, char* argv[] );
void initPythonExtensions( void );

inline void Py_WPDealloc( PyObject* self )
{
    PyObject_Del( self );
}

//////////////
// wolfpack.console
//	print( String )
//	progress( String )
//	printDone()
//	printFail()
//	printSkip()
PyObject* PyWPConsole_send( PyObject* self, PyObject* args );
PyObject* PyWPConsole_progress( PyObject* self, PyObject* args );
PyObject* PyWPConsole_printDone( PyObject* self, PyObject* args );
PyObject* PyWPConsole_printFail( PyObject* self, PyObject* args );
PyObject* PyWPConsole_printSkip( PyObject* self, PyObject* args );

PyObject* PyWPServer_shutdown( PyObject* self, PyObject* args );
PyObject* PyWPServer_save( PyObject* self, PyObject* args );

PyObject* PyWPItems_findbyserial( PyObject* self, PyObject* args );
PyObject* PyWPItems_add( PyObject* self, PyObject* args );

PyObject* PyWPChars_findbyserial( PyObject* self, PyObject* args );

PyObject* PyWPMovement_deny( PyObject* self, PyObject* args );
PyObject* PyWPMovement_accept( PyObject* self, PyObject* args );

PyObject* PyWP_clients( PyObject* self, PyObject* args );

PyObject* PyWPMap_gettile( PyObject* self, PyObject* args );
PyObject* PyWPMap_getheight( PyObject* self, PyObject* args );

static PyMethodDef WPGlobalMethods[] = 
{
    { "console_send",		PyWPConsole_send,		METH_VARARGS, "Prints something to the wolfpack console" },
	{ "console_progress",	PyWPConsole_progress,	METH_VARARGS, "Prints a .....[xxxx] block" },
	{ "console_printDone",	PyWPConsole_printDone,	METH_VARARGS, "Prints a [done] block" },
	{ "console_printFail",	PyWPConsole_printFail,	METH_VARARGS, "Prints a [fail] block" },
	{ "console_printSkip",	PyWPConsole_printSkip,	METH_VARARGS, "Prints a [skip] block" },

	// .map
	{ "map_gettile",		PyWPMap_gettile,		METH_VARARGS, "Get's a maptile" },
	{ "map_getheight",		PyWPMap_getheight,		METH_VARARGS, "Get's the height of the map at the specified point" },

	// .server
	{ "server_shutdown",	PyWPServer_shutdown,	METH_VARARGS, "Shuts the server down" },
	{ "server_save",		PyWPServer_save,		METH_VARARGS, "Saves the worldstate" },
	
	// .items
	{ "items_add",			PyWPItems_add,			METH_VARARGS, "Adds an item by it's ID specified in the definition files" },
	{ "items_findbyserial",	PyWPItems_findbyserial,	METH_VARARGS, "Finds an item by it's serial" },

	// .chars	
	{ "chars_findbyserial",	PyWPChars_findbyserial,	METH_VARARGS, "Finds an char by it's serial" },

	// .movement
	{ "movement_accept",	PyWPMovement_accept,	METH_VARARGS, "Accepts the movement of the character" },
	{ "movement_deny",		PyWPMovement_deny,		METH_VARARGS, "Denies the movement of a character" },

	// .clients
	{ "clients",			PyWP_clients,			METH_VARARGS, "Retrieves the clientS object" },

    { NULL, NULL, 0, NULL }
};

//========================================= WPItem

typedef struct {
    PyObject_HEAD;
	P_ITEM Item;
} Py_WPItem;

PyObject *Py_WPItemGetAttr( Py_WPItem *self, char *name );
int Py_WPItemSetAttr( Py_WPItem *self, char *name, PyObject *value );
PyObject* Py_WPItembySerial( PyObject* self, PyObject* args );

// WPItem
PyObject* Py_WPItem_update( Py_WPItem* self, PyObject* args );
PyObject* Py_WPItem_delete( Py_WPItem* self, PyObject* args );

static PyTypeObject Py_WPItemType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPItem",
    sizeof(Py_WPItemType),
    0,
    Py_WPDealloc,				/*tp_dealloc*/
    0,								/*tp_print*/
    (getattrfunc)Py_WPItemGetAttr,	/*tp_getattr*/
    (setattrfunc)Py_WPItemSetAttr,								/*tp_setattr*/
    0,								/*tp_compare*/
    0,								/*tp_repr*/
    0,								/*tp_as_number*/
    0,								/*tp_as_sequence*/
    0,								/*tp_as_mapping*/
    0,								/*tp_hash */
};

static PyMethodDef Py_WPItemMethods[] = 
{
    { "update", (getattrofunc)Py_WPItem_update, METH_VARARGS, "Sends the item to all clients in range." },
	{ "delete", (getattrofunc)Py_WPItem_delete, METH_VARARGS, "Deletes the item and the underlying reference." },
    { NULL, NULL, 0, NULL }
};

//========================================= WPChar
typedef struct {
    PyObject_HEAD;
	P_CHAR Char;
} Py_WPChar;

PyObject *Py_WPCharGetAttr( Py_WPChar *self, char *name );
int Py_WPCharSetAttr( Py_WPChar *self, char *name, PyObject *value );

// WPChar
PyObject* Py_WPChar_update( Py_WPChar* self, PyObject* args );
PyObject* Py_WPChar_message( Py_WPChar* self, PyObject* args );
PyObject* Py_WPChar_sysmessage( Py_WPChar* self, PyObject* args );
PyObject* Py_WPChar_requesttarget( Py_WPChar* self, PyObject* args );
PyObject* Py_WPChar_lineofsight( Py_WPChar* self, PyObject* args );
PyObject* Py_WPChar_distance( Py_WPChar* self, PyObject* args );

static PyTypeObject Py_WPCharType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPChar",
    sizeof(Py_WPCharType),
    0,
    Py_WPDealloc,
    0,
    (getattrfunc)Py_WPCharGetAttr,
    (setattrfunc)Py_WPCharSetAttr,
    0,
    0,
    0,
    0,
    0,
    0,
};

static PyMethodDef Py_WPCharMethods[] = 
{
	{ "sysmessage",    (getattrofunc)Py_WPChar_sysmessage, METH_VARARGS, "Sends a systemmessage to the characters 'console'." },
    { "update",		   (getattrofunc)Py_WPChar_update, METH_VARARGS, "Sends the char to all clients in range." },
	{ "requesttarget", (getattrofunc)Py_WPChar_requesttarget, METH_VARARGS, "Requests a target-cursor from the client." },
	{ "message",	   (getattrofunc)Py_WPChar_message, METH_VARARGS, "Displays a message above the characters head - only visible for the player." },
	{ "lineofsight",   (getattrofunc)Py_WPChar_lineofsight, METH_VARARGS, "Checks if a specific object (or location) is in the line of sight of our character" },
	{ "distance",	   (getattrofunc)Py_WPChar_distance, METH_VARARGS, "Calculates the distance between a character and an item, a char or a location" },
    { NULL, NULL, 0, NULL }
};

//========================================= WPClients

typedef struct {
    PyObject_HEAD;
} Py_WPClients;

int Py_WPClientsLength( Py_WPClients *self );
PyObject *Py_WPClientsGet( Py_WPClients *self, int Num );

static PySequenceMethods Py_WPClientsSequence = {
	(inquiry)Py_WPClientsLength,	/* sq_length */
	0,		/* sq_concat */
	0,		/* sq_repeat */
	(intargfunc)Py_WPClientsGet,			/* sq_item */
	0,		/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,		/* sq_contains */
};

static PyTypeObject Py_WPClientsType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPClients",
    sizeof(Py_WPClientsType),
    0,
    Py_WPDealloc,						/*tp_dealloc*/
    0,									/*tp_print*/
    0,									/*tp_getattr*/
    0,									/*tp_setattr*/
    0,									/*tp_compare*/
    0,									/*tp_repr*/
    0,									/*tp_as_number*/
    &Py_WPClientsSequence,				/*tp_as_sequence*/
    0,									/*tp_as_mapping*/
    0,	/*tp_hash */
};

//================================= WPClient
typedef struct {
    PyObject_HEAD;
	UOXSOCKET Socket;
} Py_WPClient;

PyObject *Py_WPClientGetAttr( Py_WPClient *self, char *name );

static PyTypeObject Py_WPClientType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPClient",
    sizeof(Py_WPClientType),
    0,
    Py_WPDealloc,						/*tp_dealloc*/
    0,									/*tp_print*/
    (getattrfunc)Py_WPClientGetAttr,		/*tp_getattr*/
    0,									/*tp_setattr*/
    0,									/*tp_compare*/
    0,									/*tp_repr*/
    0,									/*tp_as_number*/
    0,				/*tp_as_sequence*/
    0,									/*tp_as_mapping*/
    0,	/*tp_hash */
};

PyObject* Py_WPClient_disconnect( Py_WPClient* self, PyObject* args );
PyObject* Py_WPClient_send( Py_WPClient* self, PyObject* args );

static PyMethodDef Py_WPClientMethods[] = 
{
    { "disconnect",		(getattrofunc)Py_WPClient_disconnect,	METH_VARARGS, "Disconnects this socket." },
	{ "send",			(getattrofunc)Py_WPClient_send,			METH_VARARGS, "Sends a buffer to the socket." },
    { NULL, NULL, 0, NULL }
};

//================================= WPContent

typedef struct {
    PyObject_HEAD;
	cItem *Item;
} Py_WPContent;

int Py_WPContentLength( Py_WPContent *self );
PyObject *Py_WPContentGet( Py_WPContent *self, int Num );

static PySequenceMethods Py_WPContentSequence = {
	(inquiry)Py_WPContentLength,
	0,
	0,
	(intargfunc)Py_WPContentGet,
	0,
	0,
	0,
	0,
};

static PyTypeObject Py_WPContentType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPContent",
    sizeof(Py_WPContentType),
    0,
    Py_WPDealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    &Py_WPContentSequence,
    0,
    0,
};

//================================= WPEquipment

typedef struct {
    PyObject_HEAD;
	cChar *Char;
} Py_WPEquipment;

int Py_WPEquipmentLength( Py_WPEquipment *self );
PyObject *Py_WPEquipmentGet( Py_WPEquipment *self, int Num );

static PySequenceMethods Py_WPEquipmentSequence = {
	(inquiry)Py_WPEquipmentLength,
	0,
	0,
	(intargfunc)Py_WPEquipmentGet,
	0,
	0,
	0,
	0,
};

static PyTypeObject Py_WPEquipmentType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPEquipment",
    sizeof(Py_WPEquipmentType),
    0,
    Py_WPDealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    &Py_WPEquipmentSequence,
    0,
    0,
};

//================== TARGETTING
#include "wptargetrequests.h"
#include "targeting.h"

typedef struct {
    PyObject_HEAD;
	PKGx6C targetInfo;
} Py_WPTarget;

PyObject *Py_WPTargetGetAttr( Py_WPTarget *self, char *name );

static PyTypeObject Py_WPTargetType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "WPTarget",
    sizeof(Py_WPTargetType),
    0,
    Py_WPDealloc,
    0,
    (getattrfunc)Py_WPTargetGetAttr,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

class cPythonTarget: public cTargetRequest
{
protected:
	PyObject *callback_;
	PyObject *arguments_;

public:
	cPythonTarget( PyObject *callback, PyObject *arguments );
	virtual ~cPythonTarget() {};

	virtual void responsed( UOXSOCKET socket, PKGx6C targetInfo );
	virtual void timedout( UOXSOCKET socket );
};

#endif // __WPPYTHONSCRIPT_H__
