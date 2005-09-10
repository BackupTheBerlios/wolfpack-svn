
#include <qdatetime.h>
#include <qdir.h>
#include <qmessagebox.h>

#include "client.h"
#include "utilities.h"
#include "config.h"
#include "log.h"
#include "exceptions.h"
#include "texture.h"
#include "sound.h"
#include "scripts.h"

#include "gui/cursor.h"
#include "gui/gui.h"
#include "gui/worldview.h"
#include "gui/contextmenu.h"
#include "gui/tooltip.h"

#include "game/mobile.h"
#include "game/world.h"
#include "game/tooltips.h"

#include "muls/animations.h"
#include "muls/gumpart.h"
#include "muls/hues.h"
#include "muls/verdata.h"
#include "muls/art.h"
#include "muls/maps.h"
#include "muls/asciifonts.h"
#include "muls/unicodefonts.h"
#include "muls/sounds.h"
#include "muls/multis.h"
#include "muls/speech.h"
#include "muls/tiledata.h"
#include "muls/textures.h"
#include "muls/localization.h"

#include "dialogs/login.h"

#include "network/uosocket.h"

#include <QErrorMessage>

#if defined(Q_OS_WIN32)
#include "windows/gmtoolwnd.h"
#endif

void myMessageOutput( QtMsgType type, const char *msg )
{
    switch ( type ) {
        case QtDebugMsg:
			// This is crazy...
			// Log->print(LOG_DEBUG, tr("QT Debug: %1\n").arg(msg));
            break;
        case QtWarningMsg:
			Log->print(LOG_WARNING, tr("QT Warning: %1\n").arg(msg));
            break;
        case QtFatalMsg:
			Log->print(LOG_ERROR, tr("QT Fatal: %1\n").arg(msg));
			break;
		default:
			Log->print(LOG_NOTICE, tr("%1\n").arg(msg));
			break;
    }
}

cUoClient::cUoClient() {
	setObjectName("Client");

	Config = new cConfig();
	Log = new cLog();

	UoSocket = new cUoSocket();
	Random = new MTRand();
	Tooltips = new cTooltips;
	Gui = new cGui();
	Cursor = new cCursor();

	// Mul Readers
	Animations = new cAnimations;
	Gumpart = new cGumpart;
	Hues = new cHues;
	Verdata = new cVerdata;
	Art = new cArt;
	AsciiFonts = new cAsciiFonts;
	UnicodeFonts = new cUnicodeFonts;
	Tiledata = new cTiledata;
	Maps = new cMaps;
	Textures = new cTextures;
	Sounds = new cSounds;
	Speech = new cSpeech;
	Sound = new cSound;
	Localization = new cLocalization;
	Multis = new cMultis;

	World = new cWorld;	

	ContextMenu = new cContextMenu;
	LoginDialog = new cLoginDialog;

	Scripts = new cScripts;
}

cUoClient::~cUoClient() {
	delete Scripts;

	delete UoSocket;

	delete World;

	delete LoginDialog;
	delete Cursor;

	delete Gui;
	delete Tooltip;
	delete Tooltips;

	delete Multis;
	delete Localization;
	delete Speech;
	delete Animations;
	delete Textures;
	delete Maps;
	delete Tiledata;
	delete UnicodeFonts;
	delete AsciiFonts;
	delete Art;
	delete Verdata;
	delete Hues;
	delete Gumpart;

	delete Log;
	delete Sound;
	delete Sounds;
	delete Config;

	delete Random;
}

void cUoClient::load() {
	Random->seed(); // Initialize Random Number Generator

	// Check UO installation
	if (Config->uoPath().isEmpty() || !QDir(Config->uoPath()).exists()) {
		errorMessage(tr("The client was unable to detect your Ultima Online installation.\nIf you are certain, that Ultima Online is installed on your computer, please check the file config.xml\n in your clients directory and set the Ultima Online Path value manually."), tr("Ultima Online Installation Is Missing"));
		throw SilentException();
	}

	Log->print(LOG_MESSAGE, tr("Using Ultima Online at: %1\n").arg(Config->uoPath()));

	// Load MulReaders
	Localization->load();
	Verdata->load();
	Gumpart->load();
	Hues->load();
	Art->load();
	AsciiFonts->load();
	UnicodeFonts->load();
	Tiledata->load();
	Maps->load();
	Textures->load();
	Animations->load();
	Sounds->load();
	Speech->load();
	Sound->load();
	Multis->load();

	Cursor->load(); // The cursor requires the mulreader classes

	Scripts->load();
	Gui->load();
}

void cUoClient::unload() {
	Gui->unload();
	Scripts->unload();

	Cursor->unload();

	// Unload MulReaders
	Multis->unload();
	Sound->unload();
	Sounds->unload();
	Animations->unload();
	Textures->unload();
	Maps->unload();
	Tiledata->unload();
	UnicodeFonts->unload();
	AsciiFonts->unload();
	Art->unload();
	Speech->unload();
	Hues->unload();
	Gumpart->unload();
	Verdata->unload();
	Localization->unload();

	Config->save(); // Save Configuration

	Log->print("\n-----------------------------------------------------------------------------\n", false);
	Log->print(tr("Stopped Session (%1)\n").arg(QDateTime::currentDateTime().toString()), false);
	Log->print("-----------------------------------------------------------------------------\n\n", false);
}

// Main Widget
#include "mainwindow.h"

void cUoClient::run()
{
	Config->setFile("config.xml"); // Default Config File

	// Check for configuration options
	int argc = QApplication::instance()->argc();
	char **argv = QApplication::instance()->argv();
	for (int i = 0; i < argc; ++i) {
		QString arg = argv[i];
		if (arg.startsWith("-c")) {
			if (i + 1 < argc) {
				Config->setFile(argv[i+1]);
				++i; // Skip next option
			}
		}
	}

	Config->load(); // Load configuration (has to come first)

	qInstallMsgHandler(myMessageOutput); // Install handler
	
	Log->print("-----------------------------------------------------------------------------\n", false);
	Log->print(tr("Starting Session (%1)\n").arg(QDateTime::currentDateTime().toString()), false);
	Log->print("-----------------------------------------------------------------------------\n\n", false);

	// INITIALIZE WINDOW - OPENGL INTIAILZATION
	MainWindow = new cMainWindow();
	MainWindow->show();
	// END WINDOW INITIALIZATION

	try {
		load();
	} catch(const Exception &e) {
		Log->print(LOG_ERROR, e.message() + "\n");
		errorMessage(e.message());
		unload();
		return;
	} catch(const SilentException &) {
		unload();
		return;
	}
	
	LoginDialog->show(PAGE_LOGIN); // Set up the login screen

	//World->changeFacet(ILSHENAR);
	World->moveCenter(0, 0, 0, true);
	//World->moveCenter(0, 301, -7, true);
	//Player = new cMobile(1715, 301, -7, ILSHENAR, 1);
	Player = new cMobile(0, 0, 0, TRAMMEL, 1);
	Player->incref(); // Always keep one reference
	World->addEntity(Player);

	// add the tooltip instance
	Tooltip = new cTooltip;

	// add the world view
	WorldView = new cWorldView(800, 600);
	WorldView->setVisible(false);
	Gui->addControl(WorldView);

#if defined(Q_OS_WIN32)
	enableGmToolWnd();
#endif

	QTimer networkTimer;
	QObject::connect(&networkTimer, SIGNAL(timeout()), UoSocket, SLOT(poll()));
	networkTimer.setSingleShot(false);
	networkTimer.start(10); // Poll the network socket every 10 ms
	qApp->exec(); // Enter the main event loop
	networkTimer.stop(); // Stop the network timer

#if defined(Q_OS_WIN32)
	disableGmToolWnd();
#endif

	try {
		unload();
	} catch(const Exception &e) {
		Log->print(LOG_ERROR, e.message() + "\n");
	}
}

void cUoClient::errorMessage(const QString &message, const QString &title) {
	QMessageBox box(title, message, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	box.setModal(true);
	box.show();
	while (box.isVisible()) {
		qApp->processEvents();
	}
}

void cUoClient::quit() {
	MainWindow->close();
}

cUoClient *Client = 0; // Global Client Instance