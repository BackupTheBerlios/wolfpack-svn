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

// Library Includes
#include <qxml.h>
#include <qfile.h>
#include <qptrstack.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qvaluevector.h>

// Reloading
#include "ai/ai.h"
#include "spawnregions.h"
#include "territories.h"
#include "resources.h"
#include "makemenus.h"
#include "contextmenu.h"
#include "skills.h"
#include "items.h"
#include "world.h"
#include "skills.h"
#include "srvparams.h"
#include "basechar.h"
#include "network.h"

#include "wpdefmanager.h"
#include "globals.h"
#include "basics.h"
#include "inlines.h"
#include "console.h"
#include "commands.h"
#include "encryption.h"
#include "basedef.h"
#include "log.h"

#include <stdlib.h>

struct stCategory
{
	const char *name;
	eDefCategory key;
};

stCategory categories[] = {
	{ "item",			WPDT_ITEM },
	{ "script",			WPDT_SCRIPT },
	{ "npc",			WPDT_NPC },
	{ "list",			WPDT_LIST },
	{ "menu",			WPDT_MENU },
	{ "spell",			WPDT_SPELL },
	{ "acl",			WPDT_PRIVLEVEL },
	{ "spawnregion",	WPDT_SPAWNREGION },
	{ "region",			WPDT_REGION },
	{ "multi",			WPDT_MULTI },
	{ "text",			WPDT_TEXT },
	{ "startitems",		WPDT_STARTITEMS },
	{ "location",		WPDT_LOCATION },
	{ "skill",			WPDT_SKILL },
	{ "action",			WPDT_ACTION },
	{ "make",			WPDT_MAKESECTION },
	{ "makeitem",		WPDT_MAKEITEM },
	{ "useitem",		WPDT_USEITEM },
	{ "skillcheck",		WPDT_SKILLCHECK },
	{ "define",			WPDT_DEFINE },
	{ "resource",		WPDT_RESOURCE },
	{ "contextmenu",	WPDT_CONTEXTMENU },
	{ "ai",				WPDT_AI },
	{ "charbase",		WPDT_CHARBASE },
	{ 0,				WPDT_COUNT },
};

class cDefManagerPrivate
{
public:
	QMap< QString, cElement* > unique[WPDT_COUNT];
	QMap< unsigned int, QValueVector< cElement* > > nonunique;
	QStringList imports;
};

class cXmlHandler : public QXmlDefaultHandler
{
private:
	cDefManagerPrivate *impl;
	QPtrStack< cElement > stack;
	QString filename;
	QXmlLocator *locator;

public:
	cXmlHandler( cDefManagerPrivate *impl, const QString &filename )
	{
		this->impl = impl;
		this->filename = filename;
	};

	void setDocumentLocator( QXmlLocator * locator )
	{
		this->locator = locator;
	}

	bool endDocument()
	{
		while( stack.count() > 1 )
			stack.pop(); // The parent node should take care of deleting the children

		if( stack.count() == 1 )
			delete stack.pop();

		return true;
	}

	bool startElement( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )
	{
		// Some Parent Elements have special meanings
		if( stack.isEmpty() )
		{
			if( qName == "definitions" )
				return true;

			// Include another file
			if( qName == "include" )
			{
				QString value = atts.value( "file" );
				impl->imports.push_back( value );
				return true;
			}
		}

		cElement *element = new cElement;
		element->setName( localName.latin1() );
		element->copyAttributes( atts );

		// Child Element ?
		if( stack.count() > 0 )
		{
			cElement *parent = stack.current(); // Pop the potential parent
			parent->addChild( element ); // Add the child to it's parent
			element->setParent( parent );
		}

		stack.push( element ); // Push our element (there may be children)
		return true;
	}

	bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {
		if (stack.isEmpty()) {
			return true;
		}

		cElement *element = stack.pop();

		// Did we complete a parent node?
		if (stack.isEmpty()) {
			// Find a category node
			unsigned int i = 0;

			// Sort it into a category.
			while(categories[i].name != 0) {
				if (element->name() == categories[i].name) {
					QString tagId = element->getAttribute( "id" );

					// If the element has an id,
					if (!tagId.isEmpty()) {
						if (impl->unique[categories[i].key].contains(tagId) && !SrvParams->overwriteDefinitions()) {
							Console::instance()->log(LOG_WARNING, QString("Duplicate %1: %2\n[File: %3, Line: %4]\n").arg(element->name()).arg(tagId).arg(filename).arg(locator->lineNumber()));
							delete element;
						} else {
							impl->unique[categories[i].key].insert(tagId, element);
						}
					} else {
						impl->nonunique[categories[i].key].push_back(element);
					}

					return true;
				}
				++i;
			}

			Console::instance()->log(LOG_WARNING, QString("Unknown element: %1\n[File: %2, Line: %3]\n").arg(element->name()).arg(filename).arg(locator->lineNumber()));
			delete element;
		}

		return true;
	}

	bool characters ( const QString & ch )
	{
		if( !stack.isEmpty() )
		{
			cElement *element = stack.pop();
			element->setText( element->text() + ch );
			stack.push( element );
		}

		return true;
	}

	// error handling
	bool warning ( const QXmlParseException & exception )
	{
		Console::instance()->log( LOG_WARNING, QString("%1\n[File: %2, Line: %3, Column: %4").arg(exception.message(), filename).arg(exception.lineNumber()).arg(exception.columnNumber()));
		return true; // continue
	}
	bool error ( const QXmlParseException & exception )
	{
		Console::instance()->log( LOG_ERROR, QString("%1\n[File: %2, Line: %3, Column: %4").arg(exception.message(), filename).arg(exception.lineNumber()).arg(exception.columnNumber()));
		return true; // continue
	}
	bool fatalError ( const QXmlParseException & exception )
	{
		Console::instance()->log( LOG_ERROR, QString("%1\n[File: %2, Line: %3, Column: %4").arg(exception.message(), filename).arg(exception.lineNumber()).arg(exception.columnNumber()));
		return true; // continue
	}


};

// Recursive Function for Importing Script Sections
bool WPDefManager::ImportSections( const QString& FileName )
{
	QFile File( FileName );

    if ( !File.open( IO_ReadOnly ) )
	{
		Console::instance()->send( "Unable to open " );
		Console::instance()->send( FileName );
		Console::instance()->send( "!\n" );
		return false;
	}

	QXmlInputSource input( &File );
	QXmlSimpleReader reader;
	reader.setFeature( "http://trolltech.com/xml/features/report-whitespace-only-CharData", false );

	cXmlHandler handler( impl, FileName );
	reader.setContentHandler( &handler );
	reader.setErrorHandler( &handler );
	reader.parse( &input, false );

    File.close();
	return true;
}

void WPDefManager::unload( void )
{
	// Clear the nodes
	unsigned int i;

	for( i = 0; i < WPDT_COUNT; ++i )
	{
		QMap< QString, cElement* >::iterator it2;
		for( it2 = impl->unique[i].begin(); it2 != impl->unique[i].end(); ++it2 )
			delete it2.data();

		impl->unique[i].clear();

		QValueVector< cElement* >::iterator it;
		for( it = impl->nonunique[i].begin(); it != impl->nonunique[i].end(); ++it )
			delete *it;

		impl->nonunique[i].clear();
	}

	impl->imports.clear();

	BaseDefManager::instance()->unload();

	listcache_.clear();
}

void WPDefManager::reload( void )
{
	QStringList oldAISections = DefManager->getSections( WPDT_AI );

	unload();
	load();

	// Update all SubSystems associated with this Definition Manager
	KeyManager::instance()->load();
	AIFactory::instance()->checkScriptAI( oldAISections, DefManager->getSections( WPDT_AI ) );

	SpawnRegions::instance()->reload();
	AllTerritories::instance()->reload();
	Resources::instance()->reload();
	MakeMenus::instance()->reload();
	ContextMenus::instance()->reload();
	Skills->reload();

	// Update the Regions
	cCharIterator iter;
	for( P_CHAR pChar = iter.first(); pChar; pChar = iter.next() )
	{
		cTerritory *region = AllTerritories::instance()->region( pChar->pos().x, pChar->pos().y, pChar->pos().map );
		pChar->setRegion( region );
	}

	Network::instance()->reload(); // This will be integrated into the normal definition system soon
}

// Load the Definitions
void WPDefManager::load( void )
{
	unsigned int i = 0;

	Console::instance()->sendProgress( "Loading Definitions" );

	impl->imports = QStringList::split( ";", SrvParams->getString( "General", "Definitions", "definitions/index.xml", true ) );

	while (i < impl->imports.size()) {
		ImportSections(impl->imports[i]);
		++i;
	}

	Console::instance()->sendDone();

	// create a list cache, because reading all the lists on the fly
	// means wasting time
	QMap< QString, cElement* >::iterator it = impl->unique[ WPDT_LIST ].begin();

	while( it != impl->unique[ WPDT_LIST ].end() )
	{
		cElement *DefSection = it.data();

		QStringList list;
		QString data;
		for( unsigned int i = 0; i < DefSection->childCount(); ++i )
		{
			const cElement *childTag = DefSection->getChild( i );

			// Using the nodename is a very very bad habit
			// if the name of the node is "item" then
			// use the node value instead

			if( childTag->name() == "item" )
				data = childTag->text();
			else
				data = childTag->name();

			int mult = childTag->getAttribute( "mult" ).toInt();
			if( mult <= 0 )
				mult = 1;

			int j = 0;
			while( j < mult )
			{
				list.push_back( data );
				++j;
			}
		}

		listcache_.insert( it.key(), list );

		++it;
	}

	Commands::instance()->loadACLs();
	BaseDefManager::instance()->load();
}

// Returns a list of section-names found
QStringList WPDefManager::getSections( eDefCategory Type ) const
{
	// This is a VERY VERY slow function!
	QStringList result;

	QMap< QString, cElement* >::iterator it = impl->unique[ Type ].begin();

	while( it != impl->unique[ Type ].end() )
	{
		result.push_back( it.key() );
		it++;
	}

	return result;
}

QString	WPDefManager::getRandomListEntry( const QString& ListSection )
{
	QStringList *list = NULL;

	QMap< QString, QStringList >::iterator it = listcache_.find( ListSection );
	if( it != listcache_.end() )
		list = &(it.data());

	if( !list || list->isEmpty() )
		return QString();
	else
		return (*list)[ RandomNum( 0, list->size()-1 ) ];
}

QStringList	WPDefManager::getList( const QString& ListSection )
{
	QStringList list;

	QMap< QString, QStringList >::iterator it = listcache_.find( ListSection );
	if( it != listcache_.end() )
		list = it.data();

	return list;
}

QString WPDefManager::getText( const QString& TextSection ) const
{
	/*const QDomElement* DefSection = this->getSection( WPDT_TEXT, TextSection );
	if( DefSection->isNull() )
		return QString();
	else
	{
		QString text = DefSection->text();
		if( text.left( 1 ) == "\n" || text.left( 0 ) == "\r" )
			text = text.right( text.length()-1 );
		text = text.replace( QRegExp( "\\t" ), "" );
		if( text.right( 1 ) == "\n" || text.right( 1 ) == "\r" )
			text = text.left( text.length()-1 );
		return text;
	}*/
	return QString();
}

WPDefManager::WPDefManager()
{
	impl = new cDefManagerPrivate;
}

WPDefManager::~WPDefManager()
{
	unload();
	delete impl;
}


const cElement* WPDefManager::getDefinition( eDefCategory type, const QString& id ) const
{
	QMap< QString, cElement* >::const_iterator it = impl->unique[ type ].find( id );

	if( it == impl->unique[ type ].end() )
		return 0;
	else
		return it.data();
}

const QValueVector< cElement* > &WPDefManager::getDefinitions( eDefCategory type ) const
{
	return impl->nonunique[ type ];
}

/*
 *	Implementation of cElement
 */
void cElement::freeAttributes()
{
	if( attributes != 0 )
	{
		for( unsigned int i = 0; i < attrCount_; ++i )
			delete attributes[i];

		attrCount_ = 0;
		delete [] attributes;
		attributes = 0;
	}
}

void cElement::freeChildren()
{
	if( children != 0 )
	{
		for( unsigned int i = 0; i < childCount_; ++i )
			delete children[i];

		childCount_ = 0;
		delete [] children;
		children = 0;
	}
}

cElement::cElement()
{
	childCount_ = 0;
	attrCount_ = 0;
	name_ = QString::null;
	text_ = QString::null;
	children = 0;
	attributes = 0;
	parent_ = 0;
};

cElement::~cElement()
{
	freeAttributes();
	freeChildren();

	if( parent_ )
		parent_->removeChild( this );
}

void cElement::copyAttributes( const QXmlAttributes &attributes )
{
	freeAttributes();
	attrCount_ = attributes.count();

	if( attrCount_ > 0 )
	{
		this->attributes = new stAttribute*[ attrCount_ ];

		for( unsigned int i = 0; i < attrCount_; ++i )
		{
			this->attributes[i] = new stAttribute;
			this->attributes[i]->name = attributes.localName( i );
			this->attributes[i]->value = attributes.value( i );
		}
	}
}

void cElement::addChild( cElement *element )
{
	if( children == 0 )
	{
		childCount_ = 1;
		children = new cElement*[ 1 ];
		children[0] = element;
	}
	else
	{
		cElement **newChildren = new cElement* [ childCount_ + 1 ];

		// Copy over the old list
		memcpy( newChildren, children, childCount_ * sizeof( cElement* ) );
		newChildren[ childCount_++ ] = element;

		delete [] children;
		children = newChildren;
	}
}

void cElement::removeChild( cElement *element )
{
	for( unsigned int i = 0; i < childCount_; ++i )
	{
		if( children[i] == element )
		{
			// Found the element we want to delete
			unsigned int offset = 0;

			cElement **newChildren = new cElement* [ childCount_ - 1 ];

			for( unsigned int j = 0; j < childCount_; ++j )
				if( children[j] != element )
					newChildren[offset++] = children[j];

			childCount_--;
			delete [] children;
			children = newChildren;
			return;
		}
	}
}

bool cElement::hasAttribute( const QCString &name ) const
{
	for( unsigned int i = 0; i < attrCount_; ++i )
		if( attributes[i]->name == name )
			return true;

	return false;
}

const QString &cElement::getAttribute( const QCString &name, const QString &def ) const
{
	for( unsigned int i = 0; i < attrCount_; ++i )
		if( attributes[i]->name == name )
			return attributes[i]->value;

	return def;
}

void cElement::setName( const QCString &data )
{
	name_ = data;
}

const QCString &cElement::name() const
{
	return name_;
}

void cElement::setText( const QString &data )
{
	text_ = data;
}

const QString &cElement::text() const
{
	return text_;
}

void cElement::setParent( cElement *parent )
{
	parent_ = parent;
}

const cElement *cElement::parent() const
{
	return parent_;
}

const cElement *cElement::getTopmostParent() const
{
	if( parent_ )
	{
		return parent_->getTopmostParent();
	}

	return this;
}

const cElement *cElement::findChild( const QString &name ) const
{
	for( unsigned int i = 0; i < childCount_; ++i )
	{
		if( children[i]->name() == name.latin1() )
			return children[i];
	}

	return 0;
}

const cElement *cElement::getChild( unsigned int index ) const
{
	if( index >= childCount_ )
		return 0;

	return children[index];
}

unsigned int cElement::childCount() const
{
	return childCount_;
}

QString cElement::value() const
{
	QString Value = text_;

	for( unsigned int i = 0; i < childCount(); ++i )
	{
		const cElement *childTag = getChild( i );

		if( childTag->name() == "random" )
		{
			if( childTag->hasAttribute( "min" ) && childTag->hasAttribute( "max" ) )
			{
				QString min = childTag->getAttribute( "min" );
				QString max = childTag->getAttribute( "max" );

				if( min.contains( "." ) || max.contains( "." ) )
					Value += QString::number( RandomNum( min.toFloat(), max.toFloat() ) );
				else
					Value += QString::number( RandomNum( min.toInt(), max.toInt() ) );
			}
			else if( childTag->hasAttribute("valuelist") )
			{
				QStringList RandValues = QStringList::split( ",", childTag->getAttribute( "valuelist" ) );
				Value += RandValues[ RandomNum( 0, RandValues.size() - 1 ) ];
			}
			else if( childTag->hasAttribute( "list" ) )
			{
				Value += DefManager->getRandomListEntry( childTag->getAttribute( "list" ) );
			}
			else if( childTag->hasAttribute( "dice" ) )
			{
				Value += QString::number( rollDice( childTag->getAttribute( "dice" ) ) );
			}
			else if( childTag->hasAttribute( "value" ) )
			{
				QStringList parts = QStringList::split( "-", childTag->getAttribute( "value", "0-0" ) );

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
	}

	return hex2dec( Value );
}

struct wpElement {
    PyObject_HEAD;
	cElement *element;
};

static PyObject *wpElement_getAttr(wpElement *self, char *name);

PyTypeObject wpElementType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "element",
    sizeof(wpElementType),
    0,
	wpDealloc,
    0,
    (getattrfunc)wpElement_getAttr
};

static PyObject* wpElement_hasattribute(wpElement *self, PyObject *args) {
	char *name;

	if (!PyArg_ParseTuple(args, "s:element.hasattribute(name)", &name)) {
		return 0;
	}

	return self->element->hasAttribute(name) ? PyTrue() : PyFalse();
}

static PyObject* wpElement_getattribute(wpElement *self, PyObject *args) {
	char *name;
	char *value = 0;

	if (!PyArg_ParseTuple(args, "s|es:element.getattribute(name, [default])", &name, "utf-8", &value)) {
		return 0;
	}

	QString defvalue = QString::null;

	if (value != 0) {
		defvalue = QString::fromUtf8(value);
		PyMem_Free(value);
	}

	QString result = self->element->getAttribute(name, defvalue);

	if (result == QString::null) {
		return PyUnicode_FromUnicode((Py_UNICODE*)L"", 0);
	} else {
		return PyUnicode_FromUnicode((Py_UNICODE*)result.ucs2(), result.length());
	}
}

static PyObject* wpElement_findchild(wpElement *self, PyObject *args) {
	char *name;

	if (!PyArg_ParseTuple(args, "s:element.findchild(name)", &name)) {
		return 0;
	}

	cElement *result = const_cast<cElement*>(self->element->findChild(name));

	if (result) {
		return result->getPyObject();
	} else {
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static PyObject* wpElement_getchild(wpElement *self, PyObject *args) {
	unsigned int pos;

	if (!PyArg_ParseTuple(args, "I:element.getchild(pos)", &pos)) {
		return 0;
	}

	cElement *result = const_cast<cElement*>(self->element->getChild(pos));

	if (result) {
		return result->getPyObject();
	} else {
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static PyMethodDef methods[] = {
	{"hasattribute", (getattrofunc)wpElement_hasattribute, METH_VARARGS, 0},
	{"getattribute", (getattrofunc)wpElement_getattribute, METH_VARARGS, 0},
	{"findchild", (getattrofunc)wpElement_findchild, METH_VARARGS, 0},
	{"getchild", (getattrofunc)wpElement_getchild, METH_VARARGS, 0},
	{0, 0, 0, 0}
};

static PyObject *wpElement_getAttr(wpElement *self, char *name) {
	cElement *element = self->element;

	if (!strcmp(name, "name")) {
		return PyString_FromString(element->name().data());
	} else if (!strcmp(name, "parent")) {
		if (element->parent()) {
			cElement *parent = const_cast<cElement*>(element->parent());
			return parent->getPyObject();
		} else {
			Py_INCREF(Py_None);
			return Py_None;
		}
	} else if (!strcmp(name, "value")) {
		QString value = element->value();

		if (value == QString::null) {
			return PyUnicode_FromUnicode((Py_UNICODE*)L"", 0);
		} else {
			return PyUnicode_FromUnicode((Py_UNICODE*)value.ucs2(), value.length());
		}
	} else if (!strcmp(name, "text")) {
		QString value = element->text();

		if (value == QString::null) {
			return PyUnicode_FromUnicode((Py_UNICODE*)L"", 0);
		} else {
			return PyUnicode_FromUnicode((Py_UNICODE*)value.ucs2(), value.length());
		}
	} else if (!strcmp(name, "childcount")) {
		return PyInt_FromLong(self->element->childCount());
	}

	return Py_FindMethod(methods, (PyObject*)self, name);
}

// Python Scripting Interface for elements
PyObject *cElement::getPyObject() {
	wpElement *returnVal = PyObject_New(wpElement, &wpElementType);
	returnVal->element = this;
	return (PyObject*)returnVal;
}

const char *cElement::className() const {
	return "element";
}

bool cElement::implements(const QString &name) const {
	if (name == "element") {
		return true;
	} else {
		return cPythonScriptable::implements(name);
	}
}
