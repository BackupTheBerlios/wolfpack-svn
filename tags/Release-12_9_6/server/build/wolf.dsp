# Microsoft Developer Studio Project File - Name="wolf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wolf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wolf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wolf.mak" CFG="wolf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wolf - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wolf - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "wolf"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /GR /GX /O2 /I "lib/Python/PC" /I "sqlite" /I "lib/Python/include" /I "lib\ZThread\include" /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "QT_NO_STL" /D "QT_THREAD_SUPPORT" /D "MYSQL_DRIVER" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ole32.lib advapi32.lib ws2_32.lib comctl32.lib $(QTDIR)\lib\qt-mt331.lib shell32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\wolfpack.exe" /libpath:"lib\ZThread\lib" /libpath:"lib\Python\lib" /libpath:"lib\bugreport\lib" /libpath:"flatstore\Release" /opt:ref /opt:nowin98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /Gm /GR /GX /Zi /Od /I "sqlite" /I "lib\bugreport" /I "$(QTDIR)\include" /D "_CONSOLE" /D "_MBCS" /D "_DEBUG" /D "WIN32" /D "QT_DLL" /D "QT_NO_STL" /D "QT_THREAD_SUPPORT" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ole32.lib advapi32.lib ws2_32.lib $(QTDIR)\lib\qt-mt331.lib shell32.lib /nologo /version:12.9 /subsystem:windows /incremental:no /debug /machine:I386 /out:"..\wolfpack.exe" /pdbtype:sept /fixed:no
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "wolf - Win32 Release"
# Name "wolf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\accounts.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\ai.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\ai_animals.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\ai_humans.cpp
# End Source File
# Begin Source File

SOURCE=.\ai\ai_monsters.cpp
# End Source File
# Begin Source File

SOURCE=.\basechar.cpp
# End Source File
# Begin Source File

SOURCE=.\basedef.cpp
# End Source File
# Begin Source File

SOURCE=.\basics.cpp
# End Source File
# Begin Source File

SOURCE=.\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\commands.cpp
# End Source File
# Begin Source File

SOURCE=.\console.cpp
# End Source File
# Begin Source File

SOURCE=.\win\console_win.cpp
# End Source File
# Begin Source File

SOURCE=.\contextmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\coord.cpp
# End Source File
# Begin Source File

SOURCE=.\corpse.cpp
# End Source File
# Begin Source File

SOURCE=.\customtags.cpp
# End Source File
# Begin Source File

SOURCE=.\dbdriver.cpp
# End Source File
# Begin Source File

SOURCE=.\dbl_single_click.cpp
# End Source File
# Begin Source File

SOURCE=.\definable.cpp
# End Source File
# Begin Source File

SOURCE=.\dragdrop.cpp
# End Source File
# Begin Source File

SOURCE=.\encryption.cpp
# End Source File
# Begin Source File

SOURCE=.\getopts.cpp
# End Source File
# Begin Source File

SOURCE=.\win\getopts_win.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\guilds.cpp
# End Source File
# Begin Source File

SOURCE=.\gumps.cpp
# End Source File
# Begin Source File

SOURCE=.\itemid.cpp
# End Source File
# Begin Source File

SOURCE=.\items.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\makemenus.cpp
# End Source File
# Begin Source File

SOURCE=.\maps.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\multi.cpp
# End Source File
# Begin Source File

SOURCE=.\multiscache.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.cpp
# End Source File
# Begin Source File

SOURCE=.\party.cpp
# End Source File
# Begin Source File

SOURCE=.\persistentbroker.cpp
# End Source File
# Begin Source File

SOURCE=.\persistentobject.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=.\preferences.cpp
# End Source File
# Begin Source File

SOURCE=.\pythonscript.cpp
# End Source File
# Begin Source File

SOURCE=.\resources.cpp
# End Source File
# Begin Source File

SOURCE=.\scriptmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sectors.cpp
# End Source File
# Begin Source File

SOURCE=.\skills.cpp
# End Source File
# Begin Source File

SOURCE=.\spawnregions.cpp
# End Source File
# Begin Source File

SOURCE=.\speech.cpp
# End Source File
# Begin Source File

SOURCE=.\srvparams.cpp
# End Source File
# Begin Source File

SOURCE=.\win\srvparams_win.cpp
# End Source File
# Begin Source File

SOURCE=.\targetrequests.cpp
# End Source File
# Begin Source File

SOURCE=.\territories.cpp
# End Source File
# Begin Source File

SOURCE=.\tilecache.cpp
# End Source File
# Begin Source File

SOURCE=.\Timing.cpp
# End Source File
# Begin Source File

SOURCE=.\TmpEff.cpp
# End Source File
# Begin Source File

SOURCE=.\tracking.cpp
# End Source File
# Begin Source File

SOURCE=.\Trade.cpp
# End Source File
# Begin Source File

SOURCE=.\twofish\TWOFISH2.C
# End Source File
# Begin Source File

SOURCE=.\uobject.cpp
# End Source File
# Begin Source File

SOURCE=.\walking.cpp
# End Source File
# Begin Source File

SOURCE=.\wolfpack.cpp
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# Begin Source File

SOURCE=.\wpdefmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\wptargetrequests.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\accounts.h
# End Source File
# Begin Source File

SOURCE=.\ai\ai.h
# End Source File
# Begin Source File

SOURCE=.\basechar.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\basedef.h
# End Source File
# Begin Source File

SOURCE=.\baseregion.h
# End Source File
# Begin Source File

SOURCE=.\basics.h
# End Source File
# Begin Source File

SOURCE=.\chars.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\combat.h
# End Source File
# Begin Source File

SOURCE=.\commands.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\console.h
# End Source File
# Begin Source File

SOURCE=.\contextmenu.h
# End Source File
# Begin Source File

SOURCE=.\coord.h
# End Source File
# Begin Source File

SOURCE=.\corpse.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\customtags.h
# End Source File
# Begin Source File

SOURCE=.\dbdriver.h
# End Source File
# Begin Source File

SOURCE=.\dbl_single_click.h
# End Source File
# Begin Source File

SOURCE=.\definable.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\dragdrop.h
# End Source File
# Begin Source File

SOURCE=.\encryption.h
# End Source File
# Begin Source File

SOURCE=.\exceptions.h
# End Source File
# Begin Source File

SOURCE=.\factory.h
# End Source File
# Begin Source File

SOURCE=.\getopts.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\guilds.h
# End Source File
# Begin Source File

SOURCE=.\gumps.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\inlines.h
# End Source File
# Begin Source File

SOURCE=.\itemid.h
# End Source File
# Begin Source File

SOURCE=.\items.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\makemenus.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\maps.h
# End Source File
# Begin Source File

SOURCE=.\mulstructs.h
# End Source File
# Begin Source File

SOURCE=.\multi.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\multiscache.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# Begin Source File

SOURCE=.\npc.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\python\objectcache.h
# End Source File
# Begin Source File

SOURCE=.\pagesystem.h
# End Source File
# Begin Source File

SOURCE=.\party.h
# End Source File
# Begin Source File

SOURCE=.\persistentbroker.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\persistentobject.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\platform.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\preferences.h
# End Source File
# Begin Source File

SOURCE=.\pythonscript.h
# End Source File
# Begin Source File

SOURCE=.\resources.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\scriptmanager.h
# End Source File
# Begin Source File

SOURCE=.\sectors.h
# End Source File
# Begin Source File

SOURCE=.\singleton.h
# End Source File
# Begin Source File

SOURCE=.\skills.h
# End Source File
# Begin Source File

SOURCE=.\spawnregions.h
# End Source File
# Begin Source File

SOURCE=.\speech.h
# End Source File
# Begin Source File

SOURCE=.\srvparams.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\targetrequests.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\territories.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\tilecache.h
# End Source File
# Begin Source File

SOURCE=.\tileflags.h
# End Source File
# Begin Source File

SOURCE=.\Timing.h
# End Source File
# Begin Source File

SOURCE=.\TmpEff.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\tracking.h
# End Source File
# Begin Source File

SOURCE=.\Trade.h
# End Source File
# Begin Source File

SOURCE=.\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\uobject.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\verinfo.h
# End Source File
# Begin Source File

SOURCE=.\walking.h
# End Source File
# Begin Source File

SOURCE=.\wolfpack.h
# End Source File
# Begin Source File

SOURCE=.\world.h
# End Source File
# Begin Source File

SOURCE=.\wpdefmanager.h
# End Source File
# Begin Source File

SOURCE=.\wptargetrequests.h
# PROP Ignore_Default_Tool 1
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res.rc
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\network\asyncnetio.cpp
# End Source File
# Begin Source File

SOURCE=.\network\asyncnetio.h
# End Source File
# Begin Source File

SOURCE=.\network\listener.cpp
# End Source File
# Begin Source File

SOURCE=.\network\listener.h
# End Source File
# Begin Source File

SOURCE=.\network\uopacket.cpp
# End Source File
# Begin Source File

SOURCE=.\network\uopacket.h
# End Source File
# Begin Source File

SOURCE=.\network\uorxpackets.cpp
# End Source File
# Begin Source File

SOURCE=.\network\uorxpackets.h
# End Source File
# Begin Source File

SOURCE=.\network\uosocket.cpp
# End Source File
# Begin Source File

SOURCE=.\network\uosocket.h

!IF  "$(CFG)" == "wolf - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing uosocket.h...
InputDir=.\network
InputPath=.\network\uosocket.h
InputName=uosocket

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "wolf - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\network\uotxpackets.cpp
# End Source File
# Begin Source File

SOURCE=.\network\uotxpackets.h
# End Source File
# End Group
# Begin Group "Python"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\python\char.cpp
# End Source File
# Begin Source File

SOURCE=.\python\content.h
# End Source File
# Begin Source File

SOURCE=.\python\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\python\engine.h
# End Source File
# Begin Source File

SOURCE=.\python\global.cpp
# End Source File
# Begin Source File

SOURCE=.\python\gump.h
# End Source File
# Begin Source File

SOURCE=.\python\item.cpp
# End Source File
# Begin Source File

SOURCE=.\python\pyaccount.cpp
# End Source File
# Begin Source File

SOURCE=.\python\pyai.cpp
# End Source File
# Begin Source File

SOURCE=.\python\pycoord.cpp
# End Source File
# Begin Source File

SOURCE=.\python\pypacket.cpp
# End Source File
# Begin Source File

SOURCE=.\python\pypacket.h
# End Source File
# Begin Source File

SOURCE=.\python\pyregion.cpp
# End Source File
# Begin Source File

SOURCE=.\python\pytooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\python\regioniterator.h
# End Source File
# Begin Source File

SOURCE=.\python\skills.h
# End Source File
# Begin Source File

SOURCE=.\python\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\python\target.h
# End Source File
# Begin Source File

SOURCE=.\python\tempeffect.h
# End Source File
# Begin Source File

SOURCE=.\python\utilities.h
# End Source File
# Begin Source File

SOURCE=.\python\worlditerator.cpp
# End Source File
# Begin Source File

SOURCE=.\python\worlditerator.h
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# End Group
# Begin Group "SQLite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sqlite\attach.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\auth.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\btree.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\btree.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\btree_rb.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\build.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\config.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\copy.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\date.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\delete.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\expr.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\func.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\hash.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\hash.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\insert.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\main.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\opcodes.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\opcodes.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\os.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\os.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\pager.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\pager.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\parse.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\parse.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\pragma.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\printf.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\random.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\select.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\sqlite.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\sqliteInt.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\table.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\tokenize.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\trigger.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\update.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\util.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\vacuum.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\vdbe.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\vdbe.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\vdbeaux.c
# End Source File
# Begin Source File

SOURCE=.\sqlite\vdbeInt.h
# End Source File
# Begin Source File

SOURCE=.\sqlite\where.c
# End Source File
# End Group
# End Target
# End Project
