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

#include "utilities.h"
#include "../network/uosocket.h"
#include "../network/uotxpackets.h"
#include "target.h"
#include "gump.h"

/*!
	Struct for WP Python Sockets
*/
typedef struct {
    PyObject_HEAD;
	cUOSocket *pSock;
} wpSocket;

// Forward Declarations
static PyObject *wpSocket_getAttr( wpSocket *self, char *name );
static int wpSocket_setAttr( wpSocket *self, char *name, PyObject *value );

/*!
	The typedef for Wolfpack Python chars
*/
static PyTypeObject wpSocketType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "wpsocket",
    sizeof(wpSocketType),
    0,
    wpDealloc,
    0,
    (getattrfunc)wpSocket_getAttr,
    (setattrfunc)wpSocket_setAttr,
};

PyObject* PyGetSocketObject( cUOSocket *socket )
{
	if( !socket )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	wpSocket *rVal = PyObject_New( wpSocket, &wpSocketType );
	rVal->pSock = socket;

	if( rVal )
		return (PyObject*)rVal;

	Py_INCREF( Py_None );
	return Py_None;
}

static PyObject* wpSocket_disconnect(wpSocket* self, PyObject* args) {
	Q_UNUSED(args);
	self->pSock->socket()->close();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_sysmessage(wpSocket* self, PyObject* args) {
	char *message;
	unsigned short color = 0x3b2;
	unsigned short font = 3;

	if (!PyArg_ParseTuple(args, "es|HH:socket.sysmessage(message, color, font)",
		"utf-8", &message, &color, &font)) {
		return 0;
	}

	self->pSock->sysMessage(QString::fromUtf8(message), color, font);
	PyMem_Free(message);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_clilocmessage(wpSocket* self, PyObject* args) {
	unsigned int clilocid;
	char *params = 0;
	unsigned short color = 0x3b2;
	unsigned short font = 3;
	PyObject *psource = Py_None;
	char *affix = 0;
	unsigned char dontmove = 0;
	unsigned char prepend = 0;

	if (!PyArg_ParseTuple(args, "I|esHHOesBB:socket.clilocmessage"
		"(messageid, [params], [color], [font], [source], [affix], [dontmove], [prepend])",
		&clilocid, "utf-8", &params, &color, &font, &psource, 
		"utf-8", &affix, &dontmove, &prepend)) {
		return 0;
	}

	cUObject *source = 0;

	if (psource != Py_None) {
		PyConvertObject(psource, &source);
	}

	if (affix != 0) {
		self->pSock->clilocMessageAffix(clilocid, QString::fromUtf8(params), 
			QString::fromUtf8(affix), color, font, source, dontmove, prepend);
		PyMem_Free(affix);
	} else {
		self->pSock->clilocMessage(clilocid, QString::fromUtf8(params), color, font, source);
	}

	if (params != 0) {
		PyMem_Free(params);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_showspeech(wpSocket* self, PyObject* args) {
	cUObject *object;
	char *message;
	unsigned short color = 0x3b2;
	unsigned short font = 3;
	unsigned char type = 0;
	
	if (!PyArg_ParseTuple(args, "O&es|HHB:socket.showspeech"
		"(source, message, [color], [font], [type])", 
		PyConvertObject, &object, "utf-8", &message, &color, &font, &type)) {
		return 0;
	}

	self->pSock->showSpeech(object, QString::fromUtf8(message), color, font, 
		(cUOTxUnicodeSpeech::eSpeechType)type);
	PyMem_Free(message);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_attachtarget(wpSocket* self, PyObject* args) {
	char *responsefunc;
	PyObject *targetargs = 0;
	char *cancelfunc = 0;
	char *timeoutfunc = 0;
	unsigned int timeout = 0;	

	if (!PyArg_ParseTuple(args, "s|O!ssI:socket.attachtarget"
		"(callback, [args], [cancelcallback], [timeoutcallback], [timeout])", 
		&responsefunc, &PyList_Type, &targetargs, &cancelfunc, &timeoutfunc, &timeout)) {
		return 0;
	}
	
	if (targetargs) {
		targetargs = PyList_AsTuple(targetargs);
	} else {
		targetargs = PyTuple_New(0);
	}

	cPythonTarget *target = new cPythonTarget(responsefunc, timeoutfunc, cancelfunc, targetargs);
	
	if (timeout) {
		target->setTimeout(uiCurrentTime + timeout);
	}

	self->pSock->attachTarget(target);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_attachitemtarget(wpSocket* self, PyObject* args) {
	char *responsefunc;
	PyObject *items;
	PyObject *targetargs = 0;	
	char *cancelfunc = 0;
	char *timeoutfunc = 0;
	unsigned int timeout = 0;
	int xoffset, yoffset, zoffset;

	if (!PyArg_ParseTuple(args, "sO!iii|O!ssI:socket.attachitemtarget"
		"(callback, [items], [args], [cancelcallback], [timeoutcallback], [timeout])", 
		&responsefunc, &PyList_Type, &items, &xoffset, &yoffset, &zoffset, &PyList_Type, 
		&targetargs, &cancelfunc, &timeoutfunc, &timeout)) {
		return 0;
	}
	
	if (targetargs) {
		targetargs = PyList_AsTuple(targetargs);
	} else {
		targetargs = PyTuple_New(0);
	}

	std::vector<stTargetItem> targetitems;

	// Evaluate the given items
	for (int i = 0; i < PyList_Size(items); ++i) {
		PyObject *listitem = PyList_GetItem(items, i);

		// Has to be another list
		if (PyList_Check(listitem)) {			
			// id, xoffset, yoffset, zoffset, hue
			if (PyList_Size(listitem) == 5) {
				PyObject *id = PyList_GetItem(listitem, 0);
				PyObject *ixoffset = PyList_GetItem(listitem, 1);
				PyObject *iyoffset = PyList_GetItem(listitem, 2);
				PyObject *izoffset = PyList_GetItem(listitem, 3);
				PyObject *hue = PyList_GetItem(listitem, 4);

				if (PyInt_Check(id) && PyInt_Check(ixoffset) && PyInt_Check(iyoffset) &&
					PyInt_Check(izoffset) && PyInt_Check(hue)) {
					stTargetItem targetitem;
					targetitem.id = PyInt_AsLong(id);
					targetitem.xOffset = PyInt_AsLong(ixoffset);
					targetitem.yOffset = PyInt_AsLong(iyoffset);
					targetitem.zOffset = PyInt_AsLong(izoffset);
					targetitem.hue = PyInt_AsLong(hue);
					targetitems.push_back(targetitem);
				}
			}
		}
	}

    cPythonTarget *target = new cPythonTarget(responsefunc, timeoutfunc, cancelfunc, targetargs);	
	
	if (timeout) {
		target->setTimeout(uiCurrentTime + timeout);
	}

	self->pSock->attachTarget(target, targetitems, xoffset, yoffset, zoffset);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_attachmultitarget(wpSocket* self, PyObject* args) {
	char *responsefunc;
	unsigned short multiid;
	PyObject *targetargs;
	char *cancelfunc = 0;
	char *timeoutfunc = 0;
	unsigned int timeout = 0;	

	if (!PyArg_ParseTuple(args, "sHO!|ssI:socket.attachmultitarget"
		"(callback, multi, args, [cancelcallback], [timeoutcallback], [timeout])", 
		&responsefunc, &multiid, &PyList_Type, &targetargs, &cancelfunc, &timeoutfunc, &timeout)) {
		return 0;
	}
	
	targetargs = PyList_AsTuple(targetargs);

	cPythonTarget *target = new cPythonTarget(responsefunc, timeoutfunc, cancelfunc, targetargs);
	
	if (timeout) {
		target->setTimeout(uiCurrentTime + timeout);
	}

	self->pSock->attachTarget(target, 0x4000 + multiid);
	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Begins CH customization
*/
static PyObject* wpSocket_customize( wpSocket* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pSock )
		return PyFalse;

	if( !checkArgItem( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}
	P_ITEM signpost = getArgItem( 0 );

	cUOTxStartCustomHouse custom;
	custom.setSerial( signpost->getTag( "house" ).toInt() ); // Morex of signpost contain serial of house
	self->pSock->send( &custom );
	Py_INCREF(Py_None);
	return Py_None;
}


/*!
	Sends a gump to the socket. This function is used internally only.
*/
static PyObject* wpSocket_sendgump(wpSocket* self, PyObject* args) {
	// Parameters:
	// x, y, nomove, noclose, nodispose, serial, type, layout, text, callback, args
	int x, y;
	bool nomove, noclose, nodispose;
	unsigned int serial, type;
	PyObject *layout, *texts, *py_args;
	char *callback;

	if (!PyArg_ParseTuple(args, "iiBBBIIO!O!sO!:socket.sendgump", &x, &y, &nomove, 
		&noclose, &nodispose, &serial, &type, &PyList_Type, &layout, &PyList_Type, &texts, 
		&callback, &PyList_Type, &py_args)) {
		return 0;
	}

	// Convert py_args to a tuple
	py_args = PyList_AsTuple(py_args);

	cPythonGump *gump = new cPythonGump( callback, py_args );
	if( serial )
		gump->setSerial( serial );

	if( type )
		gump->setType( type );

	gump->setX( x );
	gump->setY( y );
	gump->setNoClose( noclose );
	gump->setNoMove( nomove );
	gump->setNoDispose( nodispose );

	INT32 i;
	for (i = 0; i < PyList_Size( layout ); ++i) {
        PyObject *item = PyList_GetItem(layout, i);

		if (PyString_Check(item)) {
			gump->addRawLayout(PyString_AsString(item));
		} else if (PyUnicode_Check(item)) {
			gump->addRawLayout(QString::fromUcs2((ushort*)PyUnicode_AS_UNICODE(item)));
		} else {
			gump->addRawLayout("");
		}
	}

	for (i = 0; i < PyList_Size(texts); ++i) {
        PyObject *item = PyList_GetItem(texts, i);

		if (PyString_Check(item)) {
			gump->addRawText(PyString_AsString(item));
		} else if (PyUnicode_Check(item)) {
			gump->addRawText(QString::fromUcs2((ushort*)PyUnicode_AS_UNICODE(item)));
		} else {
			gump->addRawText("");
		}
	}

	self->pSock->send( gump );

	return PyInt_FromLong( gump->serial() );
}

/*!
	Closes a gump that has been sent to the client using it's
	serial.
*/
static PyObject* wpSocket_closegump( wpSocket* self, PyObject* args )
{
	unsigned int type;
	unsigned int button = 0;

	if (!PyArg_ParseTuple(args, "i|i:socket.closegump(type, [button])", &type, &button)) {
		return 0;
	}

	cUOTxCloseGump closeGump;
	closeGump.setButton(button);
	closeGump.setType(type);
	self->pSock->send(&closeGump);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Resends the world around this socket.
*/
static PyObject* wpSocket_resendworld( wpSocket* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pSock )
		return PyFalse;
	self->pSock->resendWorld( false );
	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Resends the player only.
*/
static PyObject* wpSocket_resendplayer( wpSocket* self, PyObject* args )
{
	Q_UNUSED(args);
	if( !self->pSock )
		return PyFalse;
	self->pSock->resendPlayer( false );
	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Sends a container and it's content to a socket.
*/
static PyObject* wpSocket_sendcontainer( wpSocket* self, PyObject* args )
{
	if( !self->pSock )
		return PyFalse;
	
	if( !checkArgItem( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	if( !getArgItem( 0 ) )
		return PyFalse;

	self->pSock->sendContainer( getArgItem( 0 ) );

	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Sends a packet to this socket.
*/
static PyObject* wpSocket_sendpacket( wpSocket* self, PyObject* args )
{
	if( PyTuple_Size( args ) != 1 )
	{
		PyErr_BadArgument();
		return 0;
	}

	PyObject *list = PyTuple_GetItem( args, 0 );

	if( !PyList_Check( list ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	// Build a packet
	int packetLength = PyList_Size( list );

	QByteArray buffer( packetLength );

	for( int i = 0; i < packetLength; ++i )
		buffer[i] = PyInt_AsLong( PyList_GetItem( list, i ) );

	cUOPacket packet( buffer );
	self->pSock->send( &packet );

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* wpSocket_sendpaperdoll( wpSocket* self, PyObject* args )
{
	if( !self->pSock )
		return PyFalse;
	
	if( !checkArgChar( 0 ) )
	{
		PyErr_BadArgument();
		return 0;
	}

	if( !getArgChar( 0 ) )
		return PyFalse;

	self->pSock->sendPaperdoll( getArgChar( 0 ) );

	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Returns the custom tag passed
*/
static PyObject* wpSocket_gettag( wpSocket* self, PyObject* args )
{
	if( PyTuple_Size( args ) < 1 || !checkArgStr( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	QString key = PyString_AsString( PyTuple_GetItem( args, 0 ) );
	cVariant value = self->pSock->tags().get(key);

	if( value.type() == cVariant::String )
		return PyUnicode_FromUnicode((Py_UNICODE*)value.toString().ucs2(), value.toString().length());
	else if( value.type() == cVariant::Int )
		return PyInt_FromLong( value.asInt() );
	else if( value.type() == cVariant::Double )
		return PyFloat_FromDouble( value.asDouble() );

	Py_INCREF( Py_None );
	return Py_None;
}

/*!
	Sets a custom tag
*/
static PyObject* wpSocket_settag( wpSocket* self, PyObject* args )
{
	char *key;
	PyObject *object;

	if (!PyArg_ParseTuple( args, "sO:char.settag( name, value )", &key, &object ))
		return 0;

	if (PyString_Check(object)) {
		self->pSock->tags().set(key, cVariant(PyString_AsString(object)));
	} else if (PyUnicode_Check(object)) {
		self->pSock->tags().set(key, cVariant(QString::fromUcs2((ushort*)PyUnicode_AsUnicode(object))));
	} else if (PyInt_Check(object)) {
		self->pSock->tags().set(key, cVariant((int)PyInt_AsLong(object)));
	} else if (PyFloat_Check(object)) {
		self->pSock->tags().set(key, cVariant((double)PyFloat_AsDouble(object)));
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Checks if a certain tag exists
*/
static PyObject* wpSocket_hastag( wpSocket* self, PyObject* args )
{
	if( !self->pSock )
		return PyFalse;

	if( PyTuple_Size( args ) < 1 || !checkArgStr( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	QString key = getArgStr( 0 );
	
	return self->pSock->tags().has( key ) ? PyTrue : PyFalse;
}

/*!
	Deletes a given tag
*/
static PyObject* wpSocket_deltag( wpSocket* self, PyObject* args )
{
	if( !self->pSock )
		return PyFalse;

	if( PyTuple_Size( args ) < 1 || !checkArgStr( 0 ) )
	{
		PyErr_BadArgument();
		return NULL;
	}

	QString key = getArgStr( 0 );
	self->pSock->tags().remove( key );

	Py_INCREF(Py_None);
	return Py_None;
}

/*!
	Resends the status window to this client.
*/
static PyObject *wpSocket_resendstatus( wpSocket *self, PyObject *args )
{
	Q_UNUSED(args);
	self->pSock->sendStatWindow();
	Py_INCREF(Py_None);
	return Py_None;
}

// Resend lightlevel
static PyObject *wpSocket_updatelightlevel( wpSocket *self, PyObject *args )
{
	Q_UNUSED(args);
	self->pSock->updateLightLevel();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *wpSocket_questarrow( wpSocket *self, PyObject *args )
{
	int show;
	int x = 0;
	int y = 0;

	if( !PyArg_ParseTuple( args, "i|ii:socket.questarrow( show, x, y )", &show, &x, &y ) )
		return 0;

	self->pSock->sendQuestArrow( show, x, y );
	return PyTrue;
}

static PyObject *wpSocket_log( wpSocket *self, PyObject *args )
{
	char loglevel;
	char *text;

	if( !PyArg_ParseTuple( args, "bs:socket.log( loglevel, text )", &loglevel, &text ) )
		return 0;

	self->pSock->log( (eLogLevel)loglevel, text );
	return PyTrue;
}

static PyObject *wpSocket_updateplayer(wpSocket *self, PyObject *args) {
	self->pSock->updatePlayer();
	return PyTrue;
}

static PyObject *wpSocket_updateskill(wpSocket *self, PyObject *args) {
	unsigned short skill;

	if (!PyArg_ParseTuple( args, "H:socket.updateskill(skill)", &skill)) {
		return 0;
	}

	self->pSock->sendSkill(skill);
	return PyTrue;
}

static PyMethodDef wpSocketMethods[] = 
{
	{ "updateskill",		(getattrofunc)wpSocket_updateskill, METH_VARARGS, NULL },
	{ "updateplayer",		(getattrofunc)wpSocket_updateplayer, METH_VARARGS, NULL },
	{ "questarrow",			(getattrofunc)wpSocket_questarrow, METH_VARARGS, NULL },
    { "sysmessage",			(getattrofunc)wpSocket_sysmessage, METH_VARARGS, "Sends a system message to the char." },
    { "clilocmessage",		(getattrofunc)wpSocket_clilocmessage, METH_VARARGS, "Sends a localized message to the socket." },
	{ "showspeech",			(getattrofunc)wpSocket_showspeech, METH_VARARGS, "Sends raw speech to the socket." },
	{ "disconnect",			(getattrofunc)wpSocket_disconnect, METH_VARARGS, "Disconnects the socket." },
	{ "attachtarget",		(getattrofunc)wpSocket_attachtarget,  METH_VARARGS, "Adds a target request to the socket" },
	{ "attachmultitarget",	(getattrofunc)wpSocket_attachmultitarget,  METH_VARARGS, "Adds a multi target request to the socket" },
	{ "attachitemtarget",	(getattrofunc)wpSocket_attachitemtarget,  METH_VARARGS, "Adds a target request to the socket" },
	{ "sendgump",			(getattrofunc)wpSocket_sendgump,	METH_VARARGS, "INTERNAL! Sends a gump to this socket." },
	{ "closegump",			(getattrofunc)wpSocket_closegump,	METH_VARARGS, "Closes a gump that has been sent to the client." },
	{ "resendworld",		(getattrofunc)wpSocket_resendworld,  METH_VARARGS, "Sends the surrounding world to this socket." },
	{ "resendplayer",		(getattrofunc)wpSocket_resendplayer,  METH_VARARGS, "Resends the player only." },
	{ "sendcontainer",		(getattrofunc)wpSocket_sendcontainer,  METH_VARARGS, "Sends a container to the socket." },
	{ "sendpacket",			(getattrofunc)wpSocket_sendpacket,		METH_VARARGS, "Sends a packet to this socket." },
	{ "sendpaperdoll",		(getattrofunc)wpSocket_sendpaperdoll,	METH_VARARGS,	"Sends a char's paperdool to this socket."	},
	{ "gettag",				(getattrofunc)wpSocket_gettag,	METH_VARARGS,	"Gets a tag from a socket." },
	{ "settag",				(getattrofunc)wpSocket_settag,	METH_VARARGS,	"Sets a tag to a socket." },
	{ "hastag",				(getattrofunc)wpSocket_hastag,	METH_VARARGS,	"Checks if a socket has a specific tag." },
	{ "deltag",				(getattrofunc)wpSocket_deltag,	METH_VARARGS,	"Delete specific tag." },
	{ "resendstatus",		(getattrofunc)wpSocket_resendstatus, METH_VARARGS,	"Resends the status windows to this client." },
	{ "customize",			(getattrofunc)wpSocket_customize, METH_VARARGS,	"Begin house customization." },
	{ "log",				(getattrofunc)wpSocket_log, METH_VARARGS, NULL },
	{ "updatelightlevel",   (getattrofunc)wpSocket_updatelightlevel, METH_VARARGS, NULL },
	{ NULL, NULL, 0, NULL }
};

// Getters & Setters
static PyObject *wpSocket_getAttr( wpSocket *self, char *name )
{
	if( !strcmp( name, "player" ) )
		return PyGetCharObject( self->pSock->player() );
	else if (!strcmp(name, "screenwidth")) {
		return PyInt_FromLong(self->pSock->screenWidth());
	} else if (!strcmp(name, "screenheight")) {
		return PyInt_FromLong(self->pSock->screenHeight());
	} else if (!strcmp(name, "walksequence")) {
		return PyInt_FromLong(self->pSock->walkSequence());
	} else {
		return Py_FindMethod( wpSocketMethods, (PyObject*)self, name );
	}
}

static int wpSocket_setAttr( wpSocket *self, char *name, PyObject *value )
{
	Q_UNUSED(self);
	if (!strcmp(name, "walksequence") && PyInt_Check(value)) {
		self->pSock->setWalkSequence(PyInt_AsLong(value));
		return 0;
	}
	return 1;
}

int PyConvertSocket( PyObject *object, cUOSocket** sock )
{
	if( object->ob_type != &wpSocketType )
	{
		PyErr_BadArgument();
		return 0;
	}

	*sock = ( (wpSocket*)object )->pSock;
	return 1;
}
