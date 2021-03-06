#
# Wolfpack TMake project
#
#
#

PROJECT         = wolfpack
TARGET          = wolfpack
TEMPLATE        = app
CONFIG          = console debug thread
INCLUDEPATH     = lib/ZThread/include lib/wrl/include lib/Python/include
DEFINES         = REENTRANT ZTHREAD_STATIC NDEBUG
win32:DEFINES  += WIN32 
win32:OBJECTS_DIR = obj
win32-msvc:DEFINES +=  _CONSOLE _MBCS
win32:INCLUDEPATH += lib/Python/PC
unix:INCLUDEPATH += lib/Python

unix:LIBS       = -Llib/ZThread/lib -Llib/wrl/lib -Llib/Python -ldl -lZThread -lwrl -lpython2.2



unix:TMAKE_CXXFLAGS = -funsigned-char
win32-g++:TMAKE_CXXFLAGS = -funsigned-char
win32-g++:LIBS= -Llib/ZThread/lib/ -Llib/wrl/lib -lwsock32 -lZThread -lwrl
win32-msvc:RC_FILE         = res.rc
win32-msvc:LIBS      = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comdlg32.lib ws2_32.lib ZThread.lib wrl.lib
win32-msvc:TMAKE_CXXFLAGS = /J /nologo /ML /W3 /GX /O2 /YX /FD /c

win32-borland:TMAKE_CXXFLAGS =  -K -6 -q -x -WM -w-8057 -w-8066 -w-8060 -w-8027 -w-8059 -w-8004 -w-8012
win32-borland:LIBS = ws2_32.lib lib/ZThread/lib/ZThread.lib lib/wrl/lib/wrl.lib

HEADERS         = Client.h \
		  SndPkg.h \
		  Timing.h \
		  TmpEff.h \
		  Trade.h \
		  accounts.h \
		  basics.h \
		  boats.h \
		  books.h \
		  bounty.h \
		  chars.h \
                  charsmgr.h \
		  classes.h \
		  cmdtable.h \
		  coord.h \
		  combat.h \
		  cweather.h \
		  dbl_single_click.h \
		  debug.h \
		  definable.h \
		  defines.h \
		  dragdrop.h \
		  globals.h \
		  guildstones.h \
		  gumps.h \
		  hCache.h \
		  im.h \
		  inlines.h \
                  iserialization.h \
		  itemid.h \
		  items.h \
                  itemsmgr.h \
		  junk.h \
                  magic.h \
		  mapstuff.h \
		  msgboard.h \
		  netsys.h \
		  network.h \
                  pfactory.h \
		  platform.h \
		  prototypes.h \
                  rcvpkg.h \
		  regions.h \
		  remadmin.h \
		  resource.h \
		  scriptc.h \
		  spawnregions.h \
		  speech.h \
		  sregions.h \
                  srvparams.h \
                  serxmlfile.h \
                  serbinfile.h \
		  stream.h \
		  structs.h \
		  targeting.h \
                  targetrequests.h \
		  telport.h \
                  territories.h \
		  trigger.h \
		  typedefs.h \
		  uobject.h \
		  utilsys.h \
		  verinfo.h \
		  weight.h \
		  whitespace.h \
		  wip.h \
		  wolfpack.h \
		  worldmain.h \
		  wpconsole.h \
		  tilecache.h \
		  walking2.h \
		  packlen.h \
		  extract.h \
		  wpdefaultscript.h \
		  wpdefmanager.h \
		  wppythonscript.h \
		  wpscriptmanager.h \
		  menuactions.h \
		  wptargetrequests.h 
		  
SOURCES         = Client.cpp \
		  LoS.cpp \
		  SkiTarg.cpp \
		  SndPkg.cpp \
		  Timing.cpp \
		  TmpEff.cpp \
		  Trade.cpp \
		  accounts.cpp \
		  basics.cpp \
		  boats.cpp \
		  books.cpp \
		  bounty.cpp \
		  cache.cpp \
                  chars.cpp \
                  charsmgr.cpp \
		  cmdtable.cpp \
		  combat.cpp \
		  commands.cpp \
		  coord.cpp \
		  cWeather.cpp \
		  dbl_single_click.cpp \
		  debug.cpp \
		  definable.cpp \
		  dragdrop.cpp \
		  fishing.cpp \
		  globals.cpp \
		  guildstones.cpp \
		  gumps.cpp \
		  house.cpp \
		  html.cpp \
		  im.cpp \
                  iserialization.cpp \
		  itemid.cpp \
		  items.cpp \
                  itemsmgr.cpp \
		  magic.cpp \
		  mapstuff.cpp \
		  msgboard.cpp \
		  necro.cpp \
		  netsys.cpp \
		  network.cpp \
		  newbie.cpp \
		  npcs.cpp \
		  p_ai.cpp \
                  pfactory.cpp \
		  pointer.cpp \
		  qsf.cpp \
                  rcvpkg.cpp \
		  regions.cpp \
		  remadmin.cpp \
		  scriptc.cpp \
		  scripts.cpp \
                  serxmlfile.cpp \
                  serbinfile.cpp \
		  skills.cpp \
		  speech.cpp \
		  spawnregions.cpp \
		  sregions.cpp \
                  srvparams.cpp \
		  storage.cpp \
		  targeting.cpp \
		  targetrequests.cpp \
		  telport.cpp \
                  territories.cpp \
		  trigger.cpp \
		  uobject.cpp \
		  utilsys.cpp \
		  weight.cpp \
		  wip.cpp \
		  wolfpack.cpp \
		  worldmain.cpp \
		  wpconsole.cpp \
		  tilecache.cpp \
		  walking2.cpp \
		  packlen.cpp \
		  wpdefmanager.cpp \
		  wppythonscript.cpp \
		  wpscriptmanager.cpp \
		  menuactions.cpp \
		  wptargetrequests.cpp 

INTERFACES	=
TRANSLATIONS    = \
                  languages/wolfpack_pt-BR.ts \
                  languages/wolfpack_it.ts \
                  languages/wolfpack_nl.ts
