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
//	Wolfpack Homepage: http://wpdev.sf.net/
//========================================================================================

// Global Variables

#include "wolfpack.h"
#include "verinfo.h"
#include "walking.h"
#include "wpscriptmanager.h"
#include "wpdefmanager.h"
#include "wptargetrequests.h"
#include "typedefs.h"

#if defined(__unix__)
termios termstate ;
#endif
unsigned int uiCurrentTime;
wp_version_info wp_version;

// Library Includes
#include "qdatetime.h"

//	Instantiate our console object

WPConsole_cl clConsole;

UI32 VersionRecordSize = 20L;
UI32 MultiRecordSize = 12L;
UI32 LandRecordSize = 26L;
UI32 TileRecordSize = 37L;
UI32 MapRecordSize = 3L;
UI32 MultiIndexRecordSize = 12L;
UI32 StaticRecordSize = 7L;
//  Should characters have expiration before delete

unsigned short int doorbase[DOORTYPES]={
0x0675, 0x0685, 0x0695, 0x06A5, 0x06B5, 0x06C5, 0x06D5, 0x06E5, 0x0839, 0x084C, 
0x0866, 0x00E8, 0x0314, 0x0324, 0x0334, 0x0344, 0x0354};

char skillname[SKILLS+1][20]={
"ALCHEMY", "ANATOMY", "ANIMALLORE", "ITEMID", "ARMSLORE", "PARRYING", "BEGGING", "BLACKSMITHING", "BOWCRAFT",
"PEACEMAKING", "CAMPING", "CARPENTRY", "CARTOGRAPHY", "COOKING", "DETECTINGHIDDEN", "ENTICEMENT", "EVALUATINGINTEL",
"HEALING", "FISHING", "FORENSICS", "HERDING", "HIDING", "PROVOCATION", "INSCRIPTION", "LOCKPICKING", "MAGERY",
"MAGICRESISTANCE", "TACTICS", "SNOOPING", "MUSICIANSHIP", "POISONING", "ARCHERY", "SPIRITSPEAK", "STEALING",
"TAILORING", "TAMING", "TASTEID", "TINKERING", "TRACKING", "VETERINARY", "SWORDSMANSHIP", "MACEFIGHTING", "FENCING",
"WRESTLING", "LUMBERJACKING", "MINING", "MEDITATION", "STEALTH", "REMOVETRAPS", "ALLSKILLS", "STR", "DEX", "INT", "FAME", "KARMA"
};

char spellname[71][25]={
        "Clumsy","Create Food","Feeblemind","Heal","Magic Arrow","Night Sight","Reactive Armor","Weaken",
        "Agility","Cunning","Cure","Harm","Magic Trap","Magic Untrap","Protection","Strength",
        "Bless","Fireball","Magic Lock","Poison","Telekinesis","Teleport","Unlock","Wall Of Stone",
        "Arch Cure","Arch Protection","Curse","Fire Field","Greater Heal","Lightning","Mana Drain","Recall",
        "Blade Spirits","Dispel Field","Incognito","Magic Reflection","Mind Blast","Paralyze","Poison Field","Summon Creature",
        "Dispel","Energy Bolt","Explosion","Invisibility","Mark","Mass Curse","Paralyze Field","Reveal",
        "Chain Lightning","Energy Field","Flame Strike","Gate Travel","Mana Vampire","Mass Dispel","Meteor Swarm","Polymorph",
        "Earthquake","Energy Vortex","Resurrection","Summon Air Elemental",
        "Summon Daemon","Summon Earth Elemental","Summon Fire Elemental","Summon Water Elemental",
        "Random", "Necro1", "Necro2", "Necro3", "Necro4", "Necro5"
};

char foodname[enNumberOfFood][20]={
"no food", "raw meat", "raw fish", "eggs", "crops", "fruits", "hay",
"grain", "cooked meat", "cooked fish", "pastries", "leather", "metal" };

unsigned char wppause[3]="\x33\x01";
unsigned char restart[3]="\x33\x00";
unsigned char goxyz[20]="\x20\x00\x05\xA8\x90\x01\x90\x00\x83\xFF\x00\x06\x08\x06\x49\x00\x00\x02\x00";
unsigned char wearitem[16]="\x2E\x40\x0A\x00\x01\x00\x00\x00\x01\x00\x05\xA8\x90\x00\x00";
unsigned char talk[15]="\x1C\x00\x00\x01\x02\x03\x04\x01\x90\x00\x00\x38\x00\x03";
unsigned char sysname[31]="System\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
unsigned char removeitem[6]="\x1D\x00\x00\x00\x00";
unsigned char gmprefix[10]="\x7C\x00\x00\x01\x02\x03\x04\x00\x64";
unsigned char gmmiddle[5]="\x00\x00\x00\x00";
unsigned char sfx[13]="\x54\x01\x12\x34\x00\x00\x06\x40\x05\x9A\x00\x00";
unsigned char doact[15]="\x6E\x01\x02\x03\x04\x01\x02\x00\x05\x00\x01\x00\x00\x01";
unsigned char bpitem[20]="\x40\x0D\x98\xF7\x0F\x4F\x00\x00\x09\x00\x30\x00\x52\x40\x0B\x00\x1A\x00\x00";
unsigned char dyevat[10]="\x95\x40\x01\x02\x03\x00\x00\x0F\xAB";
unsigned char updscroll[11]="\xA6\x01\x02\x02\x00\x00\x00\x00\x01\x02";
unsigned char spc[2]="\x20";
unsigned char extmove[18]="\x77\x01\x02\x03\x04\x01\x90\x01\x02\x01\x02\x0A\x00\xED\x00\x00\x00";
unsigned char particleSystem[49];             
time_t	oldtime, newtime;

unsigned long int updatepctime=0;

unsigned int polyduration=90;

int gatecount=0;
//Time variables
QDateTime uoTime;
int secondsperuominute=5; //Number of seconds for a WOLFPACK minute.
int uotickcount=1;
int openings=0;
unsigned long initialserversec ;
unsigned long initialservermill ;
//int goldamount;
int defaultpriv1;
int defaultpriv2;
unsigned int nextfieldeffecttime=0;
unsigned int nextnpcaitime=0;
unsigned int nextdecaytime=0;

// MSVC fails to compile WOLFPACK if this is unsigned, change it then
int autosaved, saveinterval;
int dosavewarning=0;	
bool heartbeat;

int err, error;
int keeprun;

int now;

FILE *lstfile, *mapfile, *sidxfile, *statfile, *verfile, *tilefile, *multifile, *midxfile;

int secure; // Secure mode

int xycount;

// - the below structure is for looking up items based on serial #
// - item's serial, owner's serial, char's serial, and container's serial

unsigned char w_anim[3]={0x1d,0x95,0}; // flying blood instead of lightnings, LB in a real bad mood

unsigned int raindroptime;

int save_counter;//LB, world backup rate
int gDecayItem=0; //Last item that was checked for decay Tauriel 3/7/99
int gRespawnItem=0; //Last item that was checked for respawn Tauriel 3/7/99


int locationcount;
unsigned int logoutcount;//Instalog

//unsigned int cmem;
int tnum;

unsigned int starttime, endtime, lclock;
bool overflow;
unsigned char globallight;
unsigned char wtype;
int executebatch;
int showlayer;

unsigned char tempflag;

unsigned int fly_p=18; // flying probability = 1/fly_p each step (if it doesnt fly)
unsigned char fly_steps_max=27;

unsigned int shoprestocktime=0;
int shoprestockrate=5;
unsigned int respawntime=0;
unsigned int gatedesttime=0;

//int *spawnedguards;
// Profiling
int networkTime = 0;
int timerTime = 0;
int autoTime = 0;
int loopTime = 0;
int networkTimeCount = 1000;
int timerTimeCount = 1000;
int autoTimeCount = 1000;
int loopTimeCount = 1000;

unsigned long int serverstarttime;

int triggerx;
int triggery;
signed char triggerz;
// Script files that need to be cached
// Crackerjack Jul 31/99

int escortRegions = 0;


unsigned int hungerdamagetimer=0; // For hunger damage

//-=-=-=-=-=-=-Classes Definitions=-=-=-=-=-=//
cSrvParams		*SrvParams;
CWorldMain		*cwmWorldState;
cAllItems		*Items;
Maps			*Map;
cSkills			*Skills;
cFishing        *Fishing;
cWeight			*Weight;
cTargets		*Targ;
cMagic			*Magic;
cAccounts		*Accounts;
cMovement		*Movement;
cSpeech	        *Speech;
cTrade	        *Trade;
cBounty         *Bounty;
cMultisCache	*MultisCache;
WPScriptManager	*ScriptManager;
WPDefManager    *DefManager;
PersistentBroker* persistentBroker;

cCharStuff::cDragonAI	*DragonAI;
cCharStuff::cBankerAI	*BankerAI;	

//////////////////// ARRAYS that didnt get initialized above ////////////////////

/////////////////////////////////////////////
///             MAXCLIENT arrays          ///
/////////////////////////////////////////////
// maximum too cause maxclient = maximum +1

 char noweather[MAXCLIENT+1]; //LB

//char firstpacket[MAXCLIENT+1];

 int newclient[MAXCLIENT];
 char unsigned buffer[MAXCLIENT][MAXBUFFER_REAL];
 char  outbuffer[MAXCLIENT][MAXBUFFER_REAL];
 short int walksequence[MAXCLIENT];
 signed char addid5[MAXCLIENT];
 int acctno[MAXCLIENT];
 P_CHAR currchar[MAXCLIENT];
 make_st itemmake[MAXCLIENT];
 int tempint[MAXCLIENT];
 unsigned char addid1[MAXCLIENT];
 unsigned char addid2[MAXCLIENT];
 unsigned char addid3[MAXCLIENT];
 unsigned char addid4[MAXCLIENT];
 int addx[MAXCLIENT];
 int addy[MAXCLIENT];
 int addx2[MAXCLIENT];
 int addy2[MAXCLIENT];
 signed char addz[MAXCLIENT];
 int addmitem[MAXCLIENT];
 char xtext[MAXCLIENT][31];
 unsigned char perm[MAXCLIENT];
 std::string SocketStrings[MAXCLIENT];

 int clickx[MAXCLIENT];
 int clicky[MAXCLIENT];
 int currentSpellType[MAXCLIENT]; // 0=spellcast, 1=scrollcast, 2=wand cast
 unsigned char targetok[MAXCLIENT];
 unsigned char clientDimension[MAXCLIENT];

//////////////////////////////////////////////
//              MAxBuffer                  //
/////////////////////////////////////////////

 char tbuffer[MAXBUFFER_REAL];
 char xoutbuffer[MAXBUFFER_REAL*3];

/////////////////////////////////////////////
///             hasmax                   ///
/////////////////////////////////////////////


// - the below structure is for looking up items based on serial #
// - item's serial, owner's serial, char's serial, and container's serial

Container_cl stablesp, contsp, ownsp, cspawnsp, spawnsp, glowsp, cownsp;

list<SERIAL> guilds;

/******************************************************/

///////////////////////////////////////////
///   and the rest                        /
///////////////////////////////////////////

 skill_st skill[SKILLS+1];
 int gatex[MAXGATES][2],gatey[MAXGATES][2];
 signed char gatez[MAXGATES][2];
 unitile_st xyblock[XYMAX];
 std::multimap <int, tele_locations_st> tele_locations; // can't use a map here, so using a multimap with x as the key :(
 title_st title[ALLSKILLS+1];

 location_st location[4000];
 logout_st logout[1024];//Instalog
 advance_st wpadvance[1000];
 //spell_st spells[100];
 int validEscortRegion[256];
 creat_st creatures[2048];

 int npcshape[5]; //Stores the coords of the bouding shape for the NPC

 jail_st jails[11];

 std::vector<std::string> clientsAllowed; // client version controll system

/////////////////////////////////////////
///////////// global string vars /////////
/////////////////////////////////////////

 char temp[1024];

