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

#if !defined(__SERXMLFILE_H__)
#define __SERXMLFILE_H__

#include "iserialization.h"

// forward Declarations

class QFile;
class QDomDocument;
class QXmlInputSource;
class QXmlSimpleReader;


#include <qxml.h>
#include <qdom.h>

class cObjectParser : public QXmlDefaultHandler
{
public:
	cObjectParser() : doc_(NULL), objectlevel_(0) {}
	~cObjectParser() {}
/*    bool startDocument()	{ doc_ = new QDomDocument(); return true; }
	bool endDocument()		{ delete doc_; return true; }*/
    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName,
                       const QXmlAttributes& att );
	bool characters( const QString& ch );
    bool endElement( const QString&, const QString& localName, const QString& );

	QDomDocument*	document()	{ return doc_; }
private:
	Q_UINT32	objectlevel_;
	QDomDocument*	doc_;
	QDomElement		currtag_;
};


/*!
CLASS
    

    This class provides an implementation for XML file serialization implementation.


USAGE
    <More detailed description of the class and
        a summary of it's (public) operations>.

*/
class serXmlFile : public ISerialization
{
protected:
	QFile* file;
	QXmlInputSource* source;
	cObjectParser* handler;
	QXmlSimpleReader* reader;
	QDomDocument* document;

	QDomElement node;
	QDomElement root;
	unsigned int _version;
	unsigned int _count;
public:
	serXmlFile() : _version(0), _count(0), file(0), source(0), reader(0), document(0) {}
	virtual ~serXmlFile();

	virtual void prepareReading(std::string ident, int bLevel = 0);
	virtual void prepareWritting(std::string ident);
	virtual void close();	

	virtual unsigned int getVersion();
	virtual unsigned int size();
	virtual void setVersion(unsigned int);

	// Write Methods
	virtual void writeObjectID( const QString& data );

	virtual void write(const char* Key, std::string &data);
	virtual void write(const char* Key, unsigned int data);
	virtual void write(const char* Key, signed int data);
	virtual void write(const char* Key, signed short data);
	virtual void write(const char* Key, unsigned short data);
	virtual void write(const char* Key, unsigned char data);
	virtual void write(const char* Key, signed char data);
	virtual void write(const char* Key, bool data);
	virtual void write(const char* Key, double data);

	virtual void done();

	// Read Methods
	virtual void readObjectID(QString &data);

	virtual void read(const char* Key, std::string    &data);
	virtual void read(const char* Key, unsigned int   &data);
	virtual void read(const char* Key, signed   int   &data);
	virtual void read(const char* Key, unsigned short &data);
	virtual void read(const char* Key, signed short   &data);
	virtual void read(const char* Key, unsigned char  &data);
	virtual void read(const char* Key, signed   char  &data);
	virtual void read(const char* Key, bool           &data);
	virtual void read(const char* Key, double		  &data);
};

#endif // __SERXMLFILE_H__
