
#include <qdatetime.h>
#include <qdir.h>
#include <qfile.h>

#include "log.h"
#include "config.h"
#include "utilities.h"

cLog::cLog() {
	currentday = 0xFF;
	logfile = new QFile;
}

cLog::~cLog()
{
	if ( logfile->isOpen() ) {
		logfile->close();
	}
}

bool cLog::checkLogFile()
{
	QDate today = QDate::currentDate();

	/*
		Try to open the logfile for today if:
		a) Our filedescriptor is invalid
		b) We don't have today anymore
		EXCEPT if the user sets LogRotate to false
	*/
	if ( !logfile->isOpen() || currentday != today.day() )
	{
		currentday = today.day();

		logfile->close(); // Just to be sure

		QString path = Config->logPath();

		if ( !path.endsWith( QChar( QDir::separator() ) ) )
			path.append( QDir::separator() );

		QDir d;
		if ( !d.exists( path ) ) {
			d.mkdir( path );
		}

		QString filename;
		if ( Config->logRotate() )
			filename.sprintf( "client-%04u-%02u-%02u.log", today.year(), today.month(), today.day() );
		else
			filename = QString( "client.log" );

		logfile->setName( path + filename );

		if ( !logfile->open( IO_WriteOnly | IO_Append | IO_Translate ) )
		{
			// This is problematic
			Utilities::messageBox( tr( "Couldn't open logfile '%1'\n" ).arg( path + filename ) );
			return false;
		}
	}

	return true;
}

void cLog::print( eLogLevel loglevel, const QString& string, bool timestamp )
{
	if ( ( Config->logMask() & loglevel ) == 0 )
	{
		return;
	}

	if ( !checkLogFile() )
		return;

	// Timestamp the data
	QTime now = QTime::currentTime();

	QCString utfdata = string.utf8();
	QCString prelude;

	if ( timestamp )
	{
		prelude.sprintf( "%02u:%02u:%02u:", now.hour(), now.minute(), now.second() );

		// LogLevel
		switch ( loglevel )
		{
		case LOG_ERROR:
			prelude.append( "ERROR: " );
			break;

		case LOG_WARNING:
			prelude.append( "WARNING: " );
			break;

		default:
			prelude.append( " " );
		}
	}

	utfdata.prepend( prelude );

	logfile->writeBlock( utfdata.data(), utfdata.length() );
	logfile->flush();
}

cLog *Log = 0; // Global cLog Instance
