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

/////////////////////
// Name:	teleport.cpp
// Purpose: functions that handle the teleporting
// History:	cut from wolfpack.cpp by Duke, 26.10.00
// Remarks:	not necessarily ALL those functions
//

#include "telport.h"
#include "regions.h"
#include "network.h"
#include "classes.h"
#include "wpdefmanager.h"

#undef  DBGFILE
#define DBGFILE "teleport.cpp"

void teleporters(P_CHAR pc_s)
{

	if ( pc_s == NULL ) return;

	multimap<int, tele_locations_st>::iterator iter_tele_locations;
	iter_tele_locations = tele_locations.find(pc_s->pos.x);
	
	while  (pc_s->pos.x == iter_tele_locations->second.origem.x && iter_tele_locations != tele_locations.end())
	{
		if(pc_s->pos.y == iter_tele_locations->second.origem.y)
		{
			if((iter_tele_locations->second.origem.z == illegal_z)||(pc_s->pos.z == iter_tele_locations->second.origem.z))
			{
				pc_s->moveTo(iter_tele_locations->second.destination);
				teleport(pc_s);
				return;
			}
		}
		iter_tele_locations++;
	}
}

void read_in_teleport(void)
{
	clConsole.PrepareProgress( "Loading Teleporters" );

	FILE *fp;
	char text[256];
	char seps[]	= " ,\t\n";
	char *token;

	fp = fopen("teleport.scp","r");

	if(fp==NULL)
	{
		clConsole.ProgressFail();
		clConsole.send( "teleport.scp not found!" );
		error = 1;
		return;
	}

	while(!feof(fp))
	{
		fgets(text,255,fp);

		if(text[0]!=';')
		{
			tele_locations_st dummy;

			token = strtok( text, seps );
			
			dummy.origem.x = atoi(token);
			token = strtok( NULL, seps );
			dummy.origem.y = atoi(token);
			token = strtok( NULL, seps );
			if (token[0] == 'A')
				dummy.origem.z = illegal_z;
			else
				dummy.origem.z = atoi(token);

			token = strtok( NULL, seps );
			dummy.destination.x = atoi(token);
			token = strtok(NULL, seps );
			dummy.destination.y = atoi(token);
			token = strtok(NULL, seps);
			dummy.destination.z = atoi(token);

			tele_locations.insert(make_pair(dummy.origem.x, dummy));
			
		}
	}
	fclose(fp);

	clConsole.ProgressDone();
}

int validtelepos(int s)
{
	int z;

	z=-1;
	if ((currchar[s]->pos.x>=1397)&&(currchar[s]->pos.x<=1400)&&
		(currchar[s]->pos.y>=1622)&&(currchar[s]->pos.y<=1630))
		z=28;
	if ((currchar[s]->pos.x>=1510)&&(currchar[s]->pos.x<=1537)&&
		(currchar[s]->pos.y>=1455)&&(currchar[s]->pos.y<=1456))
		z=15;
	return z;
}

#pragma note("whole telport.cpp should be done with python, commented out")
/*
void advancementobjects(P_CHAR pc_s, int x, int allways)
{
	char sect[512];
	unsigned long loopexit=0;

	if ( pc_s == NULL ) return;

	int pos;
	if ((pc_s->advobj()==0)||(allways==1))
	{
		staticeffect(pc_s, 0x37, 0x3A, 0, 15);
		soundeffect2(pc_s, 0x01E9);
		pc_s->setAdvobj(x);
		openscript("advance.scp");
		sprintf(sect, "ADVANCEMENT %i", x);
		if (!i_scripts[advance_script]->find(sect))
		{
			closescript();
			clConsole.send("ADVANCEMENT OBJECT: Script section not found. Aborting.\n");
			pc_s->setAdvobj(0);
			return;
		}
		else			
			do
			{
				read2();
				if (script1[0]!='}')
				{
					if ((!(strcmp("STR",(char*)script1)))||(!(strcmp("STRENGTH",(char*)script1))))
					{
						pc_s->st= getstatskillvalue((char*)script2);
						pc_s->st2 = pc_s->st;
					}
					if ((!(strcmp("DEX",(char*)script1)))||(!(strcmp("DEXTERITY",(char*)script1))))
					{
						pc_s->setDex(getstatskillvalue((char*)script2));
					}
					if ((!(strcmp("INT",(char*)script1)))||(!(strcmp("INTELLIGENCE",(char*)script1))))
					{
						pc_s->in= getstatskillvalue((char*)script2);
						pc_s->in2 = pc_s->in;
					}
					if ((!(strcmp("ALCHEMY",(char*)script1)))||(!(strcmp("SKILL0",(char*)script1)))) pc_s->setBaseSkill(ALCHEMY, getstatskillvalue((char*)script2));
					if ((!(strcmp("ANATOMY",(char*)script1)))||(!(strcmp("SKILL1",(char*)script1)))) pc_s->setBaseSkill(ANATOMY, getstatskillvalue((char*)script2));
					if ((!(strcmp("ANIMALLORE",(char*)script1)))||(!(strcmp("SKILL2",(char*)script1)))) pc_s->setBaseSkill(ANIMALLORE, getstatskillvalue((char*)script2));
					if ((!(strcmp("ITEMID",(char*)script1)))||(!(strcmp("SKILL3",(char*)script1)))) pc_s->setBaseSkill(ITEMID, getstatskillvalue((char*)script2));
					if ((!(strcmp("ARMSLORE",(char*)script1)))||(!(strcmp("SKILL4",(char*)script1)))) pc_s->setBaseSkill(ARMSLORE, getstatskillvalue((char*)script2));
					if ((!(strcmp("PARRYING",(char*)script1)))||(!(strcmp("SKILL5",(char*)script1)))) pc_s->setBaseSkill(PARRYING, getstatskillvalue((char*)script2));
					if ((!(strcmp("BEGGING",(char*)script1)))||(!(strcmp("SKILL6",(char*)script1)))) pc_s->setBaseSkill(BEGGING, getstatskillvalue((char*)script2));
					if ((!(strcmp("BLACKSMITHING",(char*)script1)))||(!(strcmp("SKILL7",(char*)script1)))) pc_s->setBaseSkill(BLACKSMITHING, getstatskillvalue((char*)script2));
					if ((!(strcmp("BOWCRAFT",(char*)script1)))||(!(strcmp("SKILL8",(char*)script1)))) pc_s->setBaseSkill(BOWCRAFT, getstatskillvalue((char*)script2));
					if ((!(strcmp("PEACEMAKING",(char*)script1)))||(!(strcmp("SKILL9",(char*)script1)))) pc_s->setBaseSkill(PEACEMAKING, getstatskillvalue((char*)script2));
					if ((!(strcmp("CAMPING",(char*)script1)))||(!(strcmp("SKILL10",(char*)script1)))) pc_s->setBaseSkill(CAMPING, getstatskillvalue((char*)script2));
					if ((!(strcmp("CARPENTRY",(char*)script1)))||(!(strcmp("SKILL11",(char*)script1)))) pc_s->setBaseSkill(CARPENTRY, getstatskillvalue((char*)script2));
					if ((!(strcmp("CARTOGRAPHY",(char*)script1)))||(!(strcmp("SKILL12",(char*)script1)))) pc_s->setBaseSkill(CARTOGRAPHY, getstatskillvalue((char*)script2));
					if ((!(strcmp("COOKING",(char*)script1)))||(!(strcmp("SKILL13",(char*)script1)))) pc_s->setBaseSkill(COOKING, getstatskillvalue((char*)script2));
					if ((!(strcmp("DETECTINGHIDDEN",(char*)script1)))||(!(strcmp("SKILL14",(char*)script1)))) pc_s->setBaseSkill(DETECTINGHIDDEN, getstatskillvalue((char*)script2));
					if ((!(strcmp("ENTICEMENT",(char*)script1)))||(!(strcmp("SKILL15",(char*)script1)))) pc_s->setBaseSkill(ENTICEMENT, getstatskillvalue((char*)script2));
					if ((!(strcmp("EVALUATINGINTEL",(char*)script1)))||(!(strcmp("SKILL16",(char*)script1)))) pc_s->setBaseSkill(EVALUATINGINTEL, getstatskillvalue((char*)script2));
					if ((!(strcmp("HEALING",(char*)script1)))||(!(strcmp("SKILL17",(char*)script1)))) pc_s->setBaseSkill(HEALING, getstatskillvalue((char*)script2));
					if ((!(strcmp("FISHING",(char*)script1)))||(!(strcmp("SKILL18",(char*)script1)))) pc_s->setBaseSkill(FISHING, getstatskillvalue((char*)script2));
					if ((!(strcmp("FORENSICS",(char*)script1)))||(!(strcmp("SKILL19",(char*)script1)))) pc_s->setBaseSkill(FORENSICS, getstatskillvalue((char*)script2));
					if ((!(strcmp("HERDING",(char*)script1)))||(!(strcmp("SKILL20",(char*)script1)))) pc_s->setBaseSkill(HERDING, getstatskillvalue((char*)script2));
					if ((!(strcmp("HIDING",(char*)script1)))||(!(strcmp("SKILL21",(char*)script1)))) pc_s->setBaseSkill(HIDING, getstatskillvalue((char*)script2));
					if ((!(strcmp("PROVOCATION",(char*)script1)))||(!(strcmp("SKILL22",(char*)script1)))) pc_s->setBaseSkill(PROVOCATION, getstatskillvalue((char*)script2));
					if ((!(strcmp("INSCRIPTION",(char*)script1)))||(!(strcmp("SKILL23",(char*)script1)))) pc_s->setBaseSkill(INSCRIPTION, getstatskillvalue((char*)script2));
					if ((!(strcmp("LOCKPICKING",(char*)script1)))||(!(strcmp("SKILL24",(char*)script1)))) pc_s->setBaseSkill(LOCKPICKING, getstatskillvalue((char*)script2));
					if ((!(strcmp("MAGERY",(char*)script1)))||(!(strcmp("SKILL25",(char*)script1)))) pc_s->setBaseSkill(MAGERY, getstatskillvalue((char*)script2));
					if ((!(strcmp("MAGICRESISTANCE",(char*)script1)))||(!(strcmp("RESIST",(char*)script1)))||(!(strcmp("SKILL26",(char*)script1)))) pc_s->setBaseSkill(MAGICRESISTANCE, getstatskillvalue((char*)script2));
					if ((!(strcmp("TACTICS",(char*)script1)))||(!(strcmp("SKILL27",(char*)script1)))) pc_s->setBaseSkill(TACTICS, getstatskillvalue((char*)script2));
					if ((!(strcmp("SNOOPING",(char*)script1)))||(!(strcmp("SKILL28",(char*)script1)))) pc_s->setBaseSkill(SNOOPING, getstatskillvalue((char*)script2));
					if ((!(strcmp("MUSICIANSHIP",(char*)script1)))||(!(strcmp("SKILL29",(char*)script1)))) pc_s->setBaseSkill(MUSICIANSHIP, getstatskillvalue((char*)script2));
					if ((!(strcmp("POISONING",(char*)script1)))||(!(strcmp("SKILL30",(char*)script1)))) pc_s->setBaseSkill(POISONING, getstatskillvalue((char*)script2));
					if ((!(strcmp("ARCHERY",(char*)script1)))||(!(strcmp("SKILL31",(char*)script1)))) pc_s->setBaseSkill(ARCHERY, getstatskillvalue((char*)script2));
					if ((!(strcmp("SPIRITSPEAK",(char*)script1)))||(!(strcmp("SKILL32",(char*)script1)))) pc_s->setBaseSkill(SPIRITSPEAK, getstatskillvalue((char*)script2));
					if ((!(strcmp("STEALING",(char*)script1)))||(!(strcmp("SKILL33",(char*)script1)))) pc_s->setBaseSkill(STEALING, getstatskillvalue((char*)script2));
					if ((!(strcmp("TAILORING",(char*)script1)))||(!(strcmp("SKILL34",(char*)script1)))) pc_s->setBaseSkill(TAILORING, getstatskillvalue((char*)script2));
					if ((!(strcmp("TAMING",(char*)script1)))||(!(strcmp("SKILL35",(char*)script1)))) pc_s->setBaseSkill(TAMING, getstatskillvalue((char*)script2));
					if ((!(strcmp("TASTEID",(char*)script1)))||(!(strcmp("SKILL36",(char*)script1)))) pc_s->setBaseSkill(TASTEID, getstatskillvalue((char*)script2));
					if ((!(strcmp("TINKERING",(char*)script1)))||(!(strcmp("SKILL37",(char*)script1)))) pc_s->setBaseSkill(TINKERING, getstatskillvalue((char*)script2));
					if ((!(strcmp("TRACKING",(char*)script1)))||(!(strcmp("SKILL38",(char*)script1)))) pc_s->setBaseSkill(TRACKING, getstatskillvalue((char*)script2));
					if ((!(strcmp("VETERINARY",(char*)script1)))||(!(strcmp("SKILL39",(char*)script1)))) pc_s->setBaseSkill(VETERINARY, getstatskillvalue((char*)script2));
					if ((!(strcmp("SWORDSMANSHIP",(char*)script1)))||(!(strcmp("SKILL40",(char*)script1)))) pc_s->setBaseSkill(SWORDSMANSHIP, getstatskillvalue((char*)script2));
					if ((!(strcmp("MACEFIGHTING",(char*)script1)))||(!(strcmp("SKILL41",(char*)script1)))) pc_s->setBaseSkill(MACEFIGHTING, getstatskillvalue((char*)script2));
					if ((!(strcmp("FENCING",(char*)script1)))||(!(strcmp("SKILL42",(char*)script1)))) pc_s->setBaseSkill(FENCING, getstatskillvalue((char*)script2));
					if ((!(strcmp("WRESTLING",(char*)script1)))||(!(strcmp("SKILL43",(char*)script1)))) pc_s->setBaseSkill(WRESTLING, getstatskillvalue((char*)script2));
					if ((!(strcmp("LUMBERJACKING",(char*)script1)))||(!(strcmp("SKILL44",(char*)script1)))) pc_s->setBaseSkill(LUMBERJACKING, getstatskillvalue((char*)script2));
					if ((!(strcmp("MINING",(char*)script1)))||(!(strcmp("SKILL45",(char*)script1)))) pc_s->setBaseSkill(MINING, getstatskillvalue((char*)script2));

					// lb, new skills
					if ((!(strcmp("MEDITATION",(char*)script1)))||(!(strcmp("SKILL46",(char*)script1)))) pc_s->setBaseSkill(MEDITATION, getstatskillvalue((char*)script2));
					if ((!(strcmp("STEALTH",(char*)script1)))||(!(strcmp("SKILL47",(char*)script1)))) pc_s->setBaseSkill(STEALTH, getstatskillvalue((char*)script2));
					if ((!(strcmp("REMOVETRAPS",(char*)script1)))||(!(strcmp("SKILL48",(char*)script1)))) pc_s->setBaseSkill(REMOVETRAPS, getstatskillvalue((char*)script2));

					if ((!(strcmp("DYEHAIR",(char*)script1))))
					{
						unsigned int ci;
						vector<SERIAL> vecContainer = contsp.getData(pc_s->serial);
						P_ITEM pi_hair = NULL;
						for (ci = 0; ci < vecContainer.size(); ci++)
						{
							P_ITEM pi_temp = FindItemBySerial(vecContainer[ci]);
							if (pi_temp != NULL)
								if( pi_temp->layer() == 0x0B )
								{
									pi_hair = pi_temp;
									break;
								}
						}
						if (pi_hair != NULL)
						{
							x=hex2num(script2);
							pi_hair->setColor( x );
							RefreshItem(pi_hair);//AntiChrist
							teleport(pc_s);
						}
					}

					if ((!(strcmp("DYEBEARD",(char*)script1))))
					{
						unsigned int ci;
						P_ITEM pi_beard = NULL;
						vector<SERIAL> vecContainer = contsp.getData(pc_s->serial);
						for (ci = 0; ci < vecContainer.size();ci++)
						{
							P_ITEM pi_temp = FindItemBySerial(vecContainer[ci]);
							if (pi_temp != NULL)
								if (pi_temp->layer() == 0x10)
								{
									pi_beard = pi_temp;
									break;
								}
						}
						if (pi_beard != NULL)
						{
							x=hex2num(script2);
							pi_beard->setColor( x );
							RefreshItem(pi_beard);//AntiChrist
							teleport(pc_s);
						}
					}

					if (!(strcmp("KILLHAIR",(char*)script1)))
					{
						unsigned int ci;
						vector<SERIAL> vecContainer = contsp.getData(pc_s->serial);
						for (ci=0;ci<vecContainer.size();ci++)
						{
							P_ITEM pi_temp = FindItemBySerial(vecContainer[ci]);
							if (pi_temp != NULL)
								if (pi_temp->layer() == 0x0B)
								{
									Items->DeleItem(pi_temp);
									break;
								}
						}
					}

					if (!(strcmp("KILLBEARD",(char*)script1)))
					{
						unsigned int ci;
						vector<SERIAL> vecContainer = contsp.getData(pc_s->serial);
						for (ci=0;ci<vecContainer.size();ci++)
						{
							P_ITEM pi_temp = FindItemBySerial(vecContainer[ci]);
							if (pi_temp != NULL)
								if (pi_temp->layer()==0x10)
								{
									Items->DeleItem(pi_temp);
									break;
								}
						}
					}

					if (!(strcmp("KILLPACK",(char*)script1)))
					{
						unsigned int ci;
						vector<SERIAL> vecContainer = contsp.getData(pc_s->serial);
						for (ci=0;ci<vecContainer.size();ci++)
						{
							P_ITEM pi = FindItemBySerial(vecContainer[ci]);
							if (pi != NULL)
								if (pi->layer()==0x15)
								{
									Items->DeleItem(pi);
									break;
								}
						}
					}
					if (!(strcmp("ITEM",(char*)script1)))
					{
						x=str2num(script2);
						pos=ftell(scpfile);
						closescript();	// lord binary 
						P_ITEM retitem = Targ->AddMenuTarget(-1, 0, x);
						openscript("advance.scp");
						fseek(scpfile, pos, SEEK_SET);
						strcpy((char*)script1, "DUMMY");
						P_ITEM pPack = Packitem(pc_s);
						if (retitem != NULL)
						{
							retitem->pos.x=50+(rand()%80);
							retitem->pos.y=50+(rand()%80);
							retitem->pos.z=9;
							if(retitem->layer()==0x0b || retitem->layer()==0x10)
							{
								retitem->setContSerial(pc_s->serial);
							}
							else
							{
								if(pPack != NULL) 
									retitem->setContSerial(pPack->serial);
							}
							RefreshItem(retitem);//AntiChrist
							teleport(pc_s);
						}
					}

					if (!(strcmp((char*)script1,"SKIN")))
					{
						pc_s->setSkin(static_cast<UI16>(hex2num(script2)));
						pc_s->setXSkin(static_cast<UI16>(hex2num(script2)));
						teleport(pc_s);
					}

					if (!(strcmp("POLY",(char*)script1)))
					{
						x=hex2num(script2);
						pc_s->setId(x);
						pc_s->xid = x;
						teleport(pc_s);
					}

					if (!(strcmp("ADVOBJ",(char*)script1)))
					{
						x=str2num(script2);
						pc_s->setAdvobj(x);
					}
				}
			}
			while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );
			closescript();
	}
	else sysmessage(calcSocketFromChar(pc_s),"You have already used an advancement object with this character.");
}
*/
/*
void monstergate(P_CHAR pc_s, int x)
{
	int tmp, z, lovalue, hivalue;
	char sect[512];
	
	if ( pc_s == NULL ) return;
	P_ITEM pBackpack = NULL;
	P_ITEM pRetitem = NULL;
	
	if (pc_s->isNpc()) return;

	QDomElement* npcSect = DefManager->getSection( WPDT_NPC, QString("%1").arg(x) );
	if( !npcSect->isNull() )
	{
		AllItemsIterator iterItem;   
        for(iterItem.Begin(); !iterItem.atEnd(); iterItem++)   
        {   
			P_ITEM pi = iterItem.GetData();   
			if (pc_s->Wears(pi) &&   
				pi->layer()!=0x15 && pi->layer()!=0x1D &&   
				pi->layer()!=0x10 && pi->layer()!=0x0B && (!pi->free))   
				{   
				if (pBackpack == NULL)   
				{   
					pBackpack = Packitem(pc_s);   
				}   
				if (pBackpack == NULL)   
				{   
					pBackpack = Items->SpawnItem(calcSocketFromChar(pc_s),pc_s,1,"#",0,0x0E,0x75,0,0,0);   
                    if (pBackpack == NULL)   
						return;   
                    pc_s->packitem = pBackpack->serial;   
    
                    pBackpack->setContSerial(pc_s->serial);   
                    pBackpack->setLayer(0x15);   
                    pBackpack->setType( 1 );   
                    pBackpack->dye=1;   
					pRetitem = pBackpack;   
                }   
                pi->pos.x = RandomNum(50, 130);   
                pi->pos.y = RandomNum(50, 130);   
                pi->pos.z=9;   
                pi->setContSerial(pBackpack->serial);   
                pi->setLayer(0x00);   
    
				SndRemoveitem(pi->serial);   
				RefreshItem(pi);//AntiChrist   
			}   
			else if (pc_s->Wears(pi) &&   
				(pi->layer() == 0x0B || pi->layer() == 0x10))   
			{   
				Items->DeleItem(pi);   
			}   
        } 

		pc_s->applyDefinition( *npcSect );
	}
	
	pc_s->setTitle(QString::null);

	//Now find real 'skill' based on 'baseskill' (stat modifiers)
	for(z=0;z<TRUESKILLS;z++)
	{
		Skills->updateSkillLevel(pc_s,z);
	}
	updatechar(pc_s);
	staticeffect(pc_s, 0x37, 0x3A, 0, 15);
	soundeffect2(pc_s, 0x01E9);
}
*/
// new function 
////////////////////////////////// 
// This will be just a body type switching 
// and coloring item. 
// Uses the integer number of the monster type 
// from the npc file. 
// Gate has to be of type 84. 
// 
// 
/*
void polycolorgate(P_CHAR pc_s, int x) 
{ 
	int tmp; 
	char sect[256]; 

	if ( pc_s == NULL ) return; 

	if (pc_s->isNpc())
		return; 

	sprintf(sect, "NPC %i", x); 
	Script *pScpBase = i_scripts[npc_script]; 
	Script *pScp = pScpBase->Select(sect, custom_npc_script); 

	if (!pScp)
		return; 

	unsigned long loopexit = 0; 
	do 
	{ 
		pScp->NextLineSplitted(); 
		if (script1[0] != '}') 
		{ 
			if (!(strcmp("ID", (char*)script1))) 
			{ 
				tmp = hex2num(script2); 
				pc_s->setId(tmp); 
				pc_s->xid = tmp; 
			} 
			else if (!(strcmp("SKIN", (char*)script1))) 
			{ 
				pc_s->setSkin(static_cast<UI16>(hex2num(script2)));
				pc_s->setXSkin(static_cast<UI16>(hex2num(script2)));
			} 
		} 
	} 
	while ((script1[0] != '}') &&(++loopexit < MAXLOOPS)); 
	pScp->Close(); 
  
	updatechar(pc_s); 
	staticeffect(pc_s, 0x37, 0x3A, 0, 15); 
	soundeffect2(pc_s, 0x01E9); 
} 
*/
// 
// 
// Aldur 
////////////////////////////////// 
/*
void objTeleporters(P_CHAR pc_s)
{
	if (pc_s == NULL) return;

	int x = pc_s->pos.x, y = pc_s->pos.y;
	
	int	StartGrid = mapRegions->StartGrid(pc_s->pos);
	//	int	getcell=mapRegions->GetCell(pc_s->x,pc_s->y);
#pragma note("replace by item region iterator")	
	unsigned int increment = 0;
	for (unsigned int checkgrid = StartGrid + (increment*mapRegions->GetColSize()); increment < 3; increment++, checkgrid = StartGrid + (increment*mapRegions->GetColSize()))
	{
		for (int a = 0; a < 3; a++)
		{
			cRegion::raw vecEntries = mapRegions->GetCellEntries(checkgrid + a);
			cRegion::rawIterator it = vecEntries.begin();
			for ( ; it != vecEntries.end(); ++it )
			{
				P_ITEM pmi = FindItemBySerial(*it);
				if (pmi != NULL)
				{
					if (pmi->pos.x == x && pmi->pos.y == y &&
						((abs(pmi->pos.z) + 10) >= abs(pc_s->pos.z)) &&((abs(pmi->pos.z) - 10) <= abs(pc_s->pos.z)))
					{
						if ((pmi->type() == 60) && (pmi->morex + pmi->morey + pmi->morez >0))
						{
							Coord_cl pos(pc_s->pos);
							pos.x = pmi->morex;
							pos.y = pmi->morey;
							pos.z = pmi->morez;
							pc_s->moveTo(pos);
							teleport(pc_s);
						}
						
						// advancement objects
						if ((pmi->type() == 80)&&(pc_s->isPlayer()))
							if (pmi->more1 != 0 || pmi->more2 != 0 || pmi->more3 != 0 || pmi->more4 != 0)
							{
								if (pc_s->serial == calcserial( pmi->more1, pmi->more2, pmi->more3, pmi->more4 ))
									advancementobjects(pc_s, pmi->morex, 0);
							}
							else
								advancementobjects(pc_s, pmi->morex, 0);
							
							if ((pmi->type() == 81)&&(pc_s->isPlayer()))
								if (pmi->more1 != 0 || pmi->more2 != 0 || pmi->more3 != 0 || pmi->more4 != 0)
								{
									if (pc_s->serial == calcserial(pmi->more1, pmi->more2, pmi->more3, pmi->more4))
										advancementobjects(pc_s, pmi->morex, 1);
								}
								else
									advancementobjects(pc_s, pmi->morex, 1);
								// The above code lets you restrict a gate's use by setting its MORE values to a char's
								// serial #
								
								// damage objects
								if (!(pc_s->isInvul()) && (pmi->type() == 85))
								{
									pc_s->hp = pc_s->hp - (pmi->morex + RandomNum(pmi->morey, pmi->morez));
									if (pc_s->hp < 1)
										pc_s->hp = 0;
									updatestats(pc_s, 0);
									if (pc_s->hp <= 0)
										deathstuff(pc_s);
								}
								// monster gates
								if (pmi->type() == 82)
									monstergate(pc_s, pmi->morex);
								////////////////////////////////// 
								// This will be just a body type switching 
								// item. 
								// 
								// 
								// 
								if (pmi->type() == 84) 
									polycolorgate(pc_s, pmi->morex); 
								// 
								// 
								// Aldur 
								//////////////////////////////////
								
								// sound objects
								if (pmi->type() == 86)
								{
									if (RandomNum(1, 100) <= pmi->morez)
										soundeffect3(pmi, (pmi->morex << 8) + pmi->morey);
								}
					}
				}
			}
		}
	}
}
*/