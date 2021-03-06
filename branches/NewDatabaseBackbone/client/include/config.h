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

#if !defined(__CONFIG_H__)
#define __CONFIG_H__

#include <qcstring.h>
#include <qstring.h>

class QDomElement;
class cConfigPrivate;

class cConfig
{
private:
	QString logPath_;
	unsigned int logMask_;
	bool logRotate_;
	unsigned int engineHeight_;
	unsigned int engineWidth_;
	bool engineWindowed_;
	QString uoPath_;
	bool useVerdata_;

	bool encryptionEnableLogin_;
	unsigned int encryptionLoginKey1_;
	unsigned int encryptionLoginKey2_;

	QCString loginHost_;
	unsigned short loginPort_;
public:
	const QString &uoPath();
	void setUoPath(const QString &data) { uoPath_ = data; setString("General", "Ultima Online Path", data); }

	const bool useVerdata() { return useVerdata_; }
	void setUseVerdata(bool data) { useVerdata_ = data; setBool("General", "Use Verdata", data); }

	// Data Getters/Setters
	const QString &logPath() { return logPath_; }
	void setLogPath(const QString &data) { logPath_ = data; setString("Logging", "Path", data); }

	const unsigned int logMask() { return logMask_; }
	void setLogMask(unsigned int data) { logMask_ = data; setNumber("Logging", "Mask", data); }

	const bool logRotate() { return logRotate_; }
	void setLogRotate(bool data) { logRotate_ = data; setBool("Logging", "Rotate", data); }

	const unsigned int engineHeight() { return engineHeight_; }
	void setEngineHeight(unsigned int data) { engineHeight_ = data; setNumber("Engine", "Height", data); }
	
	const unsigned int engineWidth() { return engineWidth_; }
	void setEngineWidth(unsigned int data) { engineWidth_ = data; setNumber("Engine", "Width", data); }

	const bool engineWindowed() { return engineWindowed_; }
	void setEngineWindowed(bool data) { engineWindowed_ = data; setBool("Engine", "Windowed", data); }

	const bool encryptionEnableLogin() { return encryptionEnableLogin_; }
	void setEncryptionEnableLogin(bool data) { encryptionEnableLogin_ = data; setBool("Encryption", "Encrypt Login", data); }

	const unsigned int encryptionLoginKey1() { return encryptionLoginKey1_; }
	void setEncryptionLoginKey1(unsigned int data) { encryptionLoginKey1_ = data; setString("Encryption", "Login Key 1", QString("0x%1").arg(data, 0, 16)); }

	const unsigned int encryptionLoginKey2() { return encryptionLoginKey2_; }
	void setEncryptionLoginKey2(unsigned int data) { encryptionLoginKey2_ = data; setString("Encryption", "Login Key 2", QString("0x%1").arg(data, 0, 16)); }

	const QCString &loginHost() { return loginHost_; }
	void setLoginHost(const QCString data) { loginHost_ = data; setString("Login", "Host", data); }

	const unsigned short loginPort() { return loginPort_; }
	void setLoginPort(unsigned short data) { loginPort_ = data; setNumber("Login", "Port", data); }

	// constructor
	cConfig();
	// destructor
	virtual ~cConfig();

	void load();
	void reload();
	void save();

	// preference file information
	const QString& file();
	void setFile(const QString &name);

	// did file open successfully?
	bool fileState();
	// is this a proper preferences file for format?
	bool formatState();

	// boolean data storage
	bool getBool( const QString& group, const QString& key, bool def = false, bool create = true );
	void setBool( const QString& group, const QString& key, bool value );
	// integer data storage
	long getNumber( const QString& group, const QString& key, long def, bool create = true );
	void setNumber( const QString& group, const QString& key, long value );
	// double data storage
	double getDouble( const QString& group, const QString& key, double def = 0.0, bool create = true );
	void setDouble( const QString& group, const QString& key, double value );
	// string data storage
	QString getString( const QString& group, const QString& key, const QString& def, bool create = true );
	void setString( const QString& group, const QString& key, const QString& value );

	// remove a key/value from the preferences
	void removeKey( const QString& group, const QString& key );
	// remove the current group from the preferences
	void removeGroup( const QString& group );
	bool containKey( const QString& group, const QString& key ) const;
	bool containGroup( const QString& group ) const;

	// flush the preferences out to file
	void flush();
	virtual void clear();

protected:
	// serialization
	void readData();
	void writeData();
	void processGroup( const QDomElement& group );

	virtual QString getGroupDoc( const QString& group );
	virtual QString getEntryDoc( const QString& group, const QString& entry );

private:
	cConfigPrivate* d;
};

extern cConfig *Config;

#endif
