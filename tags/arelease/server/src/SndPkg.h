//==================================================================================
//
//      Wolfpack Emu (WP)
//	UO Server Emulation Program
//
//	Copyright 1997, 98 by Marcus Rating (Cironian)
//  Copyright 2001 by holders identified in authors.txt
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
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//	* In addition to that license, if you are running this program or modified
//	* versions of it on a public system you HAVE TO make the complete source of
//	* the version used by you available or provide people with a location to
//	* download it.
//
//
//
//	Wolfpack Homepage: http://www.wpdev.com/
//========================================================================================

////////////////////////
// SndPkg.h: interface for SndPkg.cpp
//

#if !defined(__SNDPKG_H__)
#define __SNDPKG_H__

// Platform specifics
#include "platform.h"

// System includes

#include <iostream>

using namespace std ;

// Forward class Declaration


// Wolfpack includes 
#include "defines.h"
#include "structs.h"
#include "typedefs.h"		// UOXSOCKET and others
/*
#if _MSC_VER >= 1000
#pragma once
#endif
*/
void SndAttackOK(UOXSOCKET s, int serial);
void SndDyevat(UOXSOCKET s, int serial, short id);
void SndUpdscroll(UOXSOCKET s, short txtlen, char* txt);
void SndRemoveitem(int serial);
void SndShopgumpopen(UOXSOCKET s, int serial);

void soundeffect(int s, unsigned char a, unsigned char b); // Play sound effect for player
void soundeffect2(PC_CHAR pc, short sound);
void soundeffect2(CHARACTER p, unsigned char a, unsigned char b);
void soundeffect3(P_ITEM pi, short sound);
void soundeffect4(int p, UOXSOCKET s, unsigned char a, unsigned char b);
void soundeffect5(UOXSOCKET s, unsigned char a, unsigned char b);
void action(int s, int x); // Character does a certain action
void npcaction(int npc, int x); // NPC character does a certain action
void sysbroadcast(const char *txt); // System broadcast in bold text
void sysmessage(UOXSOCKET s, char *txt, ...); // System message (In lower left corner);
void sysmessage(UOXSOCKET s, short color, char *txt, ...);
void itemmessage(UOXSOCKET s, char *txt, int serial, short color=0x0000);
void wearIt(const UOXSOCKET s, const P_ITEM pi);
void backpack(UOXSOCKET s, SERIAL serial); // Send Backpack (with items);
void backpack2(int s, int a1, int a2, int a3, int a4); // Send corpse stuff
void sendbpitem(UOXSOCKET s, ITEM i); // Update single item in backpack
void tileeffect(int x, int y, int z, char eff1, char eff2, char speed, char loop);
void senditem(UOXSOCKET s, ITEM i); // Send items (on ground) OLD interface
void senditem(UOXSOCKET s, P_ITEM pi); // Send items (on ground);
void senditem_lsd(UOXSOCKET s, ITEM i,char color1, char color2, int x, int y, signed char z);
void sendperson_lsd(UOXSOCKET s, CHARACTER c, char color1, char color2);
void chardel (UOXSOCKET s); // Deletion of character
void textflags (int s, int i, char *name);
void teleport(CHARACTER s); // Teleports character to its current set coordinates
void teleport2(CHARACTER s); // used for /RESEND only - Morrolan, so people can find their corpses
void updatechar(CHARACTER c); // If character status has been changed (Polymorph);, resend him
void target(UOXSOCKET s, int a1, int a2, int a3, int a4, char *txt); // Send targetting cursor to client
void skillwindow(int s); // Opens the skills list, updated for client 1.26.2b by LB
void updatestats(CHARACTER c, char x);
void statwindow(int s, int i); // Opens the status window
void updates(UOXSOCKET s); // Update Window
void tips(int s, int i); // Tip of the day window
void deny(UOXSOCKET k, CHARACTER s, int sequence);
void weblaunch(int s, char *txt); // Direct client to a web page
void broadcast(int s); // GM Broadcast (Done if a GM yells something);
void itemtalk(int s, int item, char *txt); // Item "speech"
void npctalk(int s, cChar* pNpc, char *txt,char antispam); // NPC speech
void npctalk(int s, int npc, char *txt,char antispam); // NPC speech
void npctalkall(cChar* pNpc, char *txt,char antispam); // NPC speech to all in range.
void npctalkall(int npc, char *txt,char antispam); // NPC speech to all in range.
void npctalk_runic(int s, int npc, char *txt,char antispam); // NPC speech
void npcemote(int s, int npc, char *txt, char antispam); // NPC speech


void staticeffect (CHARACTER player, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, bool UO3DonlyEffekt=false, stat_st *str=NULL, bool skip_old=false );
void movingeffect(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt=false, move_st *str=NULL, bool skip_old=false);
void bolteffect(CHARACTER player, bool UO3DonlyEffekt=false, bool skip_old=false);
void staticeffect2(P_ITEM pi, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt=false, stat_st *str=NULL, bool skip_old=false);

void staticeffect3(UI16 x, UI16 y, SI08 z, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode);
void movingeffect3(CHARACTER source, unsigned short x, unsigned short y, signed char z, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode);
void movingeffect3(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode,unsigned char unk1,unsigned char unk2,unsigned char ajust,unsigned char type);
void movingeffect2(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode);
void bolteffect2(CHARACTER player,char a1,char a2);	// experimenatal, lb

void staticeffectUO3D(CHARACTER player, stat_st *sta);
void movingeffectUO3D(CHARACTER source, CHARACTER dest, move_st *sta);
void bolteffectUO3D(CHARACTER player);
void itemeffectUO3D(P_ITEM pi, stat_st *sta);

void dolight(int s, char level);
void updateskill(int s, int skillnum); // updated for client 1.26.2b by LB
void deathaction(int s, int x); // Character does a certain action
void deathmenu(int s); // Character sees death menu
void impowncreate(int s, int i, int z); //socket, player to send
void sendshopinfo(int s, int c, P_ITEM pi);
int sellstuff(int s, int i);
void playmidi(int s, char num1, char num2);
void sendtradestatus(int cont1, int cont2);
void endtrade(int b1, int b2, int b3, int b4);
void tellmessage(int i, int s, char *txt);
void PlayDeathSound( CHARACTER i );

#endif
