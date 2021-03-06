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
 * Wolfpack Homepage: http://developer.berlios.de/projects/wolfpack/
 */

#if !defined( __BASEDEF_H__ )
#define __BASEDEF_H__

#include "definitions.h"
#include "definable.h"
#include "singleton.h"
#include "pythonscript.h"
#include <qmap.h>
#include <qptrlist.h>
#include <qcstring.h>
#include <qstring.h>

class cElement;
class cPythonScript;

class cBaseDef : public cDefinable, public cPythonScriptable
{
protected:
	// Our id
	QCString id_;

	QMap<QString, unsigned int> intproperties;
	QMap<QString, QString> properties;

	QPtrList<cPythonScript> baseScripts_;
	QCString baseScriptList_;
	QCString bindmenu_;

	bool loaded;
	virtual void reset();
	virtual void load() = 0;
	void refreshScripts();

	eDefCategory definitionType;
public:
	void processNode( const cElement* node );

	inline unsigned int getIntProperty( const QString& name, unsigned int def = 0 )
	{
		load();
		QMap<QString, unsigned int>::const_iterator it = intproperties.find( name );
		if ( it == intproperties.end() )
		{
			return def;
		}
		else
		{
			return *it;
		}
	}

	inline bool hasIntProperty( const QString& name )
	{
		load();
		return intproperties.contains( name );
	}

	inline const QString& getStrProperty( const QString& name, const QString& def = QString::null )
	{
		load();
		QMap<QString, QString>::const_iterator it = properties.find( name );
		if ( it == properties.end() )
		{
			return def;
		}
		else
		{
			return *it;
		}
	}

	inline bool hasStrProperty( const QString& name )
	{
		load();
		return properties.contains( name );
	}

	inline const QCString& id() const
	{
		return id_;
	}

	inline const QCString& baseScriptList()
	{
		load();
		return baseScriptList_;
	}

	inline const QPtrList<cPythonScript>& baseScripts()
	{
		load();
		return baseScripts_;
	}

	inline const QCString& bindmenu()
	{
		load();
		return bindmenu_;
	}

	// Python Scriptable
	const char* className() const;
	PyObject* getPyObject();
	bool implements( const QString& name ) const;
	PyObject* getProperty( const QString& name );
};

class cCharBaseDef : public cBaseDef
{
	friend class cCharBaseDefs;
protected:
	// Sounds
	unsigned short basesound_;
	unsigned char type_;
	unsigned short figurine_;
	unsigned short minDamage_;
	unsigned short maxDamage_;
	short minTaming_;
	QCString carve_;
	QCString lootPacks_;
	unsigned char controlSlots_;
	unsigned char criticalHealth_;
	unsigned int actionSpeed_; // ms between moves
	unsigned int wanderSpeed_; // ms between moves

	// Override the override... ;)
	QValueVector<unsigned short> attackSound_;
	QValueVector<unsigned short> idleSound_;
	QValueVector<unsigned short> hitSound_;
	QValueVector<unsigned short> gethitSound_;
	QValueVector<unsigned short> deathSound_;

	// Misc Properties
	void load();
	void reset();
public:
	cCharBaseDef( const QCString& id );
	~cCharBaseDef();

	void processNode( const cElement* node );

	inline unsigned char controlSlots()
	{
		load();
		return controlSlots_;
	}

	inline const QValueVector<unsigned short>& attackSound()
	{
		load();
		return attackSound_;
	}

	inline const QValueVector<unsigned short>& idleSound()
	{
		load();
		return idleSound_;
	}

	inline const QValueVector<unsigned short>& hitSound()
	{
		load();
		return hitSound_;
	}

	inline const QValueVector<unsigned short>& gethitSound()
	{
		load();
		return gethitSound_;
	}

	inline const QValueVector<unsigned short>& deathSound()
	{
		load();
		return deathSound_;
	}

	inline unsigned int wanderSpeed()
	{
		load();
		return wanderSpeed_;
	}

	inline unsigned int actionSpeed()
	{
		load();
		return actionSpeed_;
	}

	inline unsigned char criticalHealth()
	{
		load();
		return criticalHealth_;
	}

	inline unsigned short basesound()
	{
		load();
		return basesound_;
	}

	inline unsigned short figurine()
	{
		load();
		return figurine_;
	}

	inline unsigned short maxDamage()
	{
		load();
		return maxDamage_;
	}

	inline unsigned short minDamage()
	{
		load();
		return minDamage_;
	}

	inline short minTaming()
	{
		load();
		return minTaming_;
	}

	inline const QCString& carve()
	{
		load();
		return carve_;
	}

	inline const QCString& lootPacks()
	{
		load();
		return lootPacks_;
	}

	PyObject* getProperty( const QString& name );
};

#define BODY_UNKNOWN 0
#define BODY_MONSTER 1
#define BODY_SEA 2
#define BODY_ANIMAL 3
#define BODY_HUMAN 4
#define BODY_EQUIPMENT 5

struct stBodyInfo
{
	unsigned short body;
	unsigned short basesound;
	unsigned short figurine;
	unsigned char flags;
	unsigned char type;
	unsigned short mountid;

	// These sounds override the basesound + soundtype if neccesary
	int attackSound;
	int idleSound;
	int hitSound;
	int gethitSound;
	int deathSound;
};

class cCharBaseDefs
{
	friend class cCharBaseDef;

protected:
	typedef QMap<QCString, cCharBaseDef*> Container;
	typedef Container::iterator Iterator;
	Container definitions;
	stBodyInfo bodyinfo[0x400];

public:
	cCharBaseDefs();
	~cCharBaseDefs();

	// Get a base definition
	// This is guaranteed to return a basedef. Even if uninitialized.
	cCharBaseDef* get( const QCString& id );

	inline const stBodyInfo& getBodyInfo( unsigned short body )
	{
		if ( body < 0x400 )
		{
			return bodyinfo[body];
		}
		else
		{
			return bodyinfo[0];
		}
	}

	// When reset is called, all loaded basedefs are unflagged.
	void reset();
	void refreshScripts();
	void loadBodyInfo();
};

typedef Singleton<cCharBaseDefs> CharBaseDefs;

/*
	This class represents an item definition and it's
	static properties.
*/
class cItemBaseDef : public cBaseDef
{
	friend class cItemBaseDefs;
protected:
	float weight_;
	unsigned int sellprice_;
	unsigned int buyprice_;
	unsigned short type_;
	unsigned char lightsource_;
	unsigned int decaydelay_;
	unsigned int flags_;
	unsigned int clilocName_; // This is the default name of this type of item

	// Misc Properties
	void load();
	void reset();

	inline void setWaterSource( bool data )
	{
		if ( data )
		{
			flags_ |= 0x01;
		}
		else
		{
			flags_ &= ~0x01;
		}
	}
public:
	cItemBaseDef( const QCString& id );
	~cItemBaseDef();

	void processNode( const cElement* node );

	inline unsigned int decaydelay()
	{
		load();
		return decaydelay_;
	}

	inline unsigned int clilocName()
	{
		load();
		return clilocName_;
	}

	inline float weight()
	{
		load();
		return weight_;
	}

	inline unsigned int sellprice()
	{
		load();
		return sellprice_;
	}

	inline unsigned int buyprice()
	{
		load();
		return buyprice_;
	}

	inline unsigned short type()
	{
		load();
		return type_;
	}

	inline unsigned char lightsource()
	{
		load();
		return lightsource_;
	}

	inline bool isWaterSource()
	{
		load();
		return ( flags_ & 0x01 ) != 0;
	}

	PyObject* getProperty( const QString& name );
};

class cItemBaseDefs
{
protected:
	typedef QMap<QCString, cItemBaseDef*> Container;
	typedef Container::iterator Iterator;
	Container definitions;
public:
	cItemBaseDefs();
	~cItemBaseDefs();

	// Get a base definition
	// This is guaranteed to return a basedef. Even if uninitialized.
	cItemBaseDef* get( const QCString& id );

	// When reset is called, all loaded basedefs are unflagged.
	void reset();
	void refreshScripts();
};

class cMultiBaseDef : public cItemBaseDef
{
public:
	cMultiBaseDef( const QCString& id );
};

typedef Singleton<cItemBaseDefs> ItemBaseDefs;

#endif
