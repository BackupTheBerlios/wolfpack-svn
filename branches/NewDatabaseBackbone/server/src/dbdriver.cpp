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

// Wolfpack Includes
#include "dbdriver.h"

// Library Includes
#include <qglobal.h>

#if defined(Q_OS_WIN32) // Required by mysql.h
# include <winsock.h>
#endif

#include <qstring.h>
#include <qptrlist.h>
#include <stdlib.h>
#include <sqlite.h>

#ifdef MYSQL_DRIVER
# ifdef _MSC_VER
#  pragma comment(lib,"libmysql.lib")
# endif
# include <mysql.h>
# include <errmsg.h>
#endif

/*****************************************************************************
  cDBDriver member functions
 *****************************************************************************/

/*!
	\class cDBDriver dbdriver.h

	\brief The cDBDriver class provides an abstraction for the different DBMS
	supported by Wolfpack.

	\ingroup database
	\ingroup mainclass
*/

/* dummy functions */
bool cDBDriver::exec( const QString& /*query*/ )
{
	return true;
}
void cDBDriver::lockTable( const QString& /*table*/ )
{
}

void cDBDriver::unlockTable( const QString& /*table*/ )
{
}

QString cDBDriver::error()
{
	return QString::null;
}

void cDBDriver::setActiveConnection( int )
{
}

/*****************************************************************************
  cDBResult member functions
 *****************************************************************************/

// Fetchs a new row, returns false if there is no new row
bool cDBResult::fetchrow()
{
	if ( !_result )
		return false;

	if ( mysql_type )
	{
#ifdef MYSQL_DRIVER
		_row = mysql_fetch_row( ( st_mysql_res * ) _result );
		return ( _row != 0 );
#endif
	}
	else
	{
		int count;
		const char** columns;

		return ( sqlite_step( ( sqlite_vm * ) _result, &count, ( const char * ** ) &_row, &columns ) == SQLITE_ROW );
	}
	return false;
}

// Call this to free the query
void cDBResult::free()
{
	if ( mysql_type )
	{
#ifdef MYSQL_DRIVER
		mysql_free_result( ( st_mysql_res * ) _result );
#endif
	}
	else
	{
		char* error;
		if ( sqlite_finalize( ( sqlite_vm * ) _result, &error ) != SQLITE_OK )
		{
			if ( error )
			{
				QString err( error );
				sqlite_freemem( error );
				throw err;
			}
			else
			{
				throw QString( "Unknown SQLite error while finalizing query." );
			}
		}
	}

	_result = 0;
	_row = 0;
	_connection = 0;
}

// Get the data for the current row
char** cDBResult::data() const
{
	return _row;
}

// Get an integer with a specific offset
Q_INT32 cDBResult::getInt( Q_UINT32 offset ) const
{
	if ( !_row )
		throw QString( "Trying to access a non valid result!" );

	return atoi( _row[offset] );
}

// Get a string with a specific offset
QString cDBResult::getString( Q_UINT32 offset ) const
{
	if ( !_row )
		throw QString( "Trying to access a non valid result!" );

	return QString::fromUtf8( _row[offset] );
}

struct wpDbResult
{
	PyObject_HEAD;
	cDBResult* result;
};

static void wpDeallocDbResult( PyObject* object )
{
	wpDbResult* result = ( wpDbResult* ) object;
	delete result->result;
	PyObject_Del( object );
}

static PyObject* wpDbResult_getAttr( wpDbResult* self, char* name );

/*
	\object dbresult
	\description This object type represents the response from the database to a query.
*/
PyTypeObject wpDbResultType =
{
PyObject_HEAD_INIT( NULL )
0,
"dbresult",
sizeof( wpDbResultType ),
0,
wpDeallocDbResult,
0,
( getattrfunc ) wpDbResult_getAttr,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

/*
	\method dbresult.free
	\description This function frees the resources allocated by this dbresult object. Always call this
	when you no longer need this object.
*/
static PyObject* wpDbResult_free( wpDbResult* self, PyObject* args )
{
	Q_UNUSED( args );
	self->result->free();
	Py_RETURN_NONE;
}

/*
	\method dbresult.fetchrow
	\return A boolean value.
	\description Fetch a new row from the database and return false if the end of the result set was reached.
*/
static PyObject* wpDbResult_fetchrow( wpDbResult* self, PyObject* args )
{
	Q_UNUSED( args );
	bool result = self->result->fetchrow();

	if ( result )
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/*
	\method dbresult.getint
	\param position The position of the integer value you want to get.
	\return An integer value.
	\description Get an integer value from the row at the given position.
	Please be careful with this function. If you specify an invalid position
	it can lead to a crash of your server.
*/
static PyObject* wpDbResult_getint( wpDbResult* self, PyObject* args )
{
	unsigned int pos;
	if ( !PyArg_ParseTuple( args, "I:dbresult.getint(position)", &pos ) )
	{
		return 0;
	}
	return PyInt_FromLong( self->result->getInt( pos ) );
}

/*
	\method dbresult.getstring
	\param position The position of the string value you want to get.
	\return A string value.
	\description Get a string value from the row at the given position.
	Please be careful with this function. If you specify an invalid position
	it can lead to a crash of your server.
*/
static PyObject* wpDbResult_getstring( wpDbResult* self, PyObject* args )
{
	unsigned int pos;
	if ( !PyArg_ParseTuple( args, "I:dbresult.getstring(position)", &pos ) )
	{
		return 0;
	}

	QString value = self->result->getString( pos );

	return QString2Python( value );
}

static PyMethodDef wpDbResultMethods[] =
{
{ "free", ( getattrofunc ) wpDbResult_free, METH_VARARGS, 0 },
{ "fetchrow", ( getattrofunc ) wpDbResult_fetchrow, METH_VARARGS, 0 },
{ "getint", ( getattrofunc ) wpDbResult_getint, METH_VARARGS, 0 },
{ "getstring", ( getattrofunc ) wpDbResult_getstring, METH_VARARGS, 0 },
{ 0, 0, 0, 0 }
};

static PyObject* wpDbResult_getAttr( wpDbResult* self, char* name )
{
	return Py_FindMethod( wpDbResultMethods, ( PyObject * ) self, name );
}

PyObject* cDBResult::getPyObject()
{
	wpDbResult* returnVal = PyObject_New( wpDbResult, &wpDbResultType );
	returnVal->result = this;
	return ( PyObject * ) returnVal;
}

bool cDBResult::implements( const QString& name )
{
	if ( name == "dbresult" )
	{
		return true;
	}
	else
	{
		return cPythonScriptable::implements( name );
	}
}

const char* cDBResult::className() const
{
	return "dbresult";
}

/*****************************************************************************
  cSQLiteDriver member functions
 *****************************************************************************/

int cSQLiteDriver::lastInsertId()
{
	return sqlite_last_insert_rowid( ( sqlite * ) connection );
}

bool cSQLiteDriver::open( int )
{
	char* error = NULL;

	close();

	connection = sqlite_open( _dbname.latin1(), 0, &error );

	if ( !connection )
	{
		if ( error )
		{
			QString err( error );
			sqlite_freemem( error );
			throw err;
		}
		else
		{
			throw QString( "Unknown SQLite error while opening database." );
		}
	}

	exec( "PRAGMA synchronous = OFF;" );
	exec( "PRAGMA default_synchronous = OFF;" );
	exec( "PRAGMA full_column_names = OFF;" );
	exec( "PRAGMA show_datatypes = OFF;" );
	exec( "PRAGMA parser_trace = OFF;" );

	return true;
}

void cSQLiteDriver::close()
{
	if ( connection != 0 )
	{
		sqlite_close( ( sqlite * ) connection );
		connection = 0;
	}
}

bool cSQLiteDriver::exec( const QString& query )
{
	char* error;

	if ( sqlite_exec( ( sqlite * ) connection, query.local8Bit(), NULL, NULL, &error ) != SQLITE_OK )
	{
		if ( error )
		{
			QString err( QString( error ) + " (" + query + ")" );
			sqlite_freemem( error );
			throw err;
		}
		else
		{
			throw QString( "Unknown SQLite error while executing: %1" ).arg( query );
		}
	}

	return true;
}

cDBResult cSQLiteDriver::query( const QString& query )
{
	char* error = NULL;
	sqlite_vm* result;

	// Compile a VM and pass it to cSQLiteResult
	if ( sqlite_compile( ( sqlite * ) connection, query.local8Bit(), NULL, &result, &error ) != SQLITE_OK )
	{
		if ( error )
		{
			QString err( QString( error ) + " (" + query + ")" );
			sqlite_freemem( error );
			throw err;
		}
		else
		{
			throw QString( "Unknown SQLite error while querying: %1" ).arg( query );
		}
	}

	return cDBResult( result, connection, false );
}

bool cSQLiteDriver::tableExists( const QString& table )
{
	cDBResult result = query( QString( "PRAGMA table_info('%1');" ).arg( table ) );

	bool res = result.fetchrow(); // Every table has at least one field

	result.free();

	return res;
}

/*****************************************************************************
  cMySQLDriver member functions
 *****************************************************************************/

#ifdef MYSQL_DRIVER

bool cMySQLDriver::open( int id )
{
	if ( connection )
		return true;

	connection = mysql_init( 0 );
	if ( !connection )
		throw QString( "mysql_init(): insufficient memory to allocate a new object" );

	( ( MYSQL * ) connection )->reconnect = 1;

	if ( !mysql_real_connect( ( MYSQL * ) connection, _host.latin1(), _username.latin1(), _password.latin1(), _dbname.latin1(), 0, 0, CLIENT_COMPRESS ) )
	{
		// Named pipes are acctually slower :(
		throw QString( "Connection to DB failed: %1" ).arg( mysql_error( ( MYSQL * ) connection ) );
	}
	connections[id] = ( MYSQL * ) connection;

	return true;
}

void cMySQLDriver::close()
{
	mysql_close( ( MYSQL * ) connection );
	connection = 0;
}

cDBResult cMySQLDriver::query( const QString& query )
{
	MYSQL* mysql = ( MYSQL* ) connection;

	if ( !mysql )
		throw QString( "Not connected to mysql server. Unable to execute query." );

	if ( mysql_query( mysql, query.local8Bit() ) )
	{
		return cDBResult(); // Return invalid result
	}

	MYSQL_RES* result = mysql_use_result( mysql );
	return cDBResult( result, mysql );
}

bool cMySQLDriver::exec( const QString& query )
{
	if ( !connection )
		throw QString( "Not connected to mysql server. Unable to execute query." );

	bool ok = !mysql_query( ( MYSQL* ) connection, query.local8Bit() );
	return ok;
}

void cMySQLDriver::lockTable( const QString& table )
{
	exec( QString( "LOCK TABLES %1 WRITE;" ).arg( table ) );
}

void cMySQLDriver::unlockTable( const QString& table )
{
	Q_UNUSED( table );
	exec( QString( "UNLOCK TABLES;" ) );
}

QString cMySQLDriver::error()
{
	const char* error = mysql_error( ( MYSQL* ) connection );

	if ( error != 0 )
	{
		return QString( error );
	}
	else
	{
		return QString::null;
	}
}

void cMySQLDriver::setActiveConnection( int id )
{
	if ( connections.find( id ) == connections.end() )
	{
		connection = NULL;
		open( id );
	}
	else
	{
		connection = connections[id];
	}
}

/*!
	No checking done for MySQL yet.
 */
bool cMySQLDriver::tableExists( const QString& table )
{
	Q_UNUSED( table );
	return true;
}

#endif
