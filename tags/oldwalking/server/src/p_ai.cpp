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

#include "wolfpack.h"
#include "SndPkg.h"
#include "debug.h"

#undef  DBGFILE
#define DBGFILE "p_ai.cpp"

void cCharStuff::CheckAI(unsigned int currenttime, int i) //Lag Fix -- Zippy
{
	int d, onl;
	unsigned int chance;
	//unsigned int StartGrid, getcell, increment, a, checkgrid;
	P_CHAR pc_i = MAKE_CHARREF_LR(i);
	if (i<0 || i>cmem) return;

	if (!(nextnpcaitime<=currenttime||(overflow))) return;
    // in npc.scp add script # for npc`s ai
	// case - script - case - script -   case - script -  case - script
	//   0   -  0     -  4    -  4     -  8    -  8     -  12   -  C
	//   1   -  1     -  5    -  5     -  9    -  9     -  13   -  D
	//   2   -  2     -  6    -  6     -  10   -  A     -  14   -  E
	//   3   -  3     -  7    -  7     -  11   -  B     -  15   -  F
	// case - script - case - script
	//   16  -  10    -  20   -  14
	//   17  -  11    -  21   -  15
	//   18  -  12    -  22   -  16
	//   19  -  13    -  23   -  17  ...this is just a guide...Ripper
	switch(pc_i->npcaitype)
	{
	case 0: // Shopkeepers greet players..Ripper
		if(server_data.VendorGreet==1 && pc_i->isNpc() && pc_i->shop==1 && pc_i->id1==0x01 && (pc_i->id2==0x90 || pc_i->id2==0x91))
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 3)
						continue;
					if (pc->isInvul() || pc->isNpc() || pc->dead || !pc->isInnocent() || !onl)
					    continue;
						sprintf((char*)temp,"Hello %s, Welcome to my shop, How may i help thee?.", pc->name);
						npctalkall(i,(char*)temp,1);
						pc_i->antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*30;
				}
			}
		}
		break;
	case 1: // good healers
		if(!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (!pc->dead || d > 3 || pc->isNpc() || !onl)
						continue;
					if (pc->isMurderer()) 
					{
						npctalkall(i, "I will nay give life to a scoundrel like thee!",1);
						return;
					}
					else if (pc->isCriminal()) 
					{
						npctalkall(i, "I will nay give life to thee for thou art a criminal!",1);
						return;
					}
					else if (pc->isInnocent())
					{
						npcaction(i, 0x10);
						Targ->NpcResurrectTarget(DEREF_P_CHAR(pc));
						staticeffect(DEREF_P_CHAR(pc), 0x37, 0x6A, 0x09, 0x06);
						switch(RandomNum(0, 4)) 
						{
						case 0: npctalkall(i, "Thou art dead, but 'tis within my power to resurrect thee.  Live!",1);			break;
						case 1: npctalkall(i, "Allow me to resurrect thee ghost.  Thy time of true death has not yet come.",1);	break;
						case 2: npctalkall(i, "Perhaps thou shouldst be more careful.  Here, I shall resurrect thee.",1);		break;
						case 3: npctalkall(i, "Live again, ghost!  Thy time in this world is not yet done.",1);					break;
						case 4: npctalkall(i, "I shall attempt to resurrect thee.",1);											break;
						}
					}
				}
			}
		}
		break;
	case 2 : // Monsters, PK's - (stupid NPCs)
		if (!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					chance = RandomNum(1, 100);
					if (DEREF_P_CHAR(pc) == i)
						continue;
					if (d>SrvParms->attack_distance)
						continue;
					if (onl &&(pc->isInvul() || pc->isHidden() || pc->dead))
						continue;
					if (pc->isNpc() &&(pc->npcaitype == 2) || pc->npcaitype == 1)
						continue;
					if (server_data.monsters_vs_animals == 0 && (strlen(pc->title) > 0 || !onl))
						continue;
					if (server_data.monsters_vs_animals == 1 && chance > server_data.animals_attack_chance)
						continue;
					if (pc_i->baseskill[MAGERY]>400)
					{
						if (pc_i->hp < (pc_i->st/2))
						{
							npctalkall(i, "In Vas Mani", 0);
							Magic->NPCHeal(i);
						}
						if (pc_i->poisoned)
						{
							npctalkall(i, "An Nox", 0);
							Magic->NPCCure(i);
						}
						if (pc->priv2&0x20)
						{
							npctalkall(i, "An Ort", 0);
							Magic->NPCDispel(i, DEREF_P_CHAR(pc));
						}
					}
					npcattacktarget(i, DEREF_P_CHAR(pc));
					return;
				}
			}
		}
		break;
	case 3 : //Evil Healers
		if(!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (!pc->dead || d > 3 || pc->isNpc() || !onl)
						continue;
					if (pc->isInnocent())
					{
						npctalkall(i, "I dispise all things good. I shall not give thee another chance!",1);
						return;
					}
					else
					{
						npcaction(i, 0x10);
						Targ->NpcResurrectTarget(DEREF_P_CHAR(pc));
						staticeffect(DEREF_P_CHAR(pc), 0x37, 0x09, 0x09, 0x19); //Flamestrike effect
						switch(RandomNum(0, 4)) 
						{
						   case 0: npctalkall(i, "Fellow minion of Mondain, Live!!",1); break;
						   case 1: npctalkall(i, "Thou has evil flowing through your vains, so I will bring you back to life.",1); break;
						   case 2: npctalkall(i, "If I res thee, promise to raise more hell!.",1); break;
						   case 3: npctalkall(i, "From hell to Britannia, come alive!.",1); break;
						   case 4: npctalkall(i, "Since you are Evil, I will bring you back to consciouness.",1); break;
						}
					}
				}
			}
		}
		break;
	case 4 : // Guards
		if (!pc_i->war	// guard isnt busy 
			&& pc_i->inGuardedArea())	// this region is guarded
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 10 || pc->isInvul() || pc->dead || !onl)
						continue;
						npcattacktarget(i, DEREF_P_CHAR(pc));
						npctalkall(i, "Thou shalt regret thine actions, swine!",1); // ANTISPAM !!! LB
				}
			}
		}
		break;
	case 5: // npc beggars
		if (!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 3 || pc->isInvul() || pc->isNpc() || pc->dead || !onl || !pc->isInnocent())
					    continue;
						int beg= RandomNum (0,2);
						{
							switch (beg)
							{
								case 0: npctalkall(i,"Could thou spare a few coins?",1);
										pc_i->antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*30;
										break;
								case 1: npctalkall(i,"Hey buddy can you spare some gold?",1);
										pc_i->antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*30;
										break;
								case 2: npctalkall(i,"I have a family to feed, think of the children.",1);
										pc_i->antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*30;
										break;
								default:
										break;
							}
						}
				}
			}
		}
		break;
	case 6: break; // Ripper- chaos guards.
	case 7: break; // Ripper- order guards.
	case 8: break; //morrolan - old banker
	case 9 : // in world guards, they dont teleport out...Ripper
		if (!pc_i->war	// guard isnt busy 
			&& pc_i->inGuardedArea())	// this region is guarded
		{	// this bracket just to keep compiler happy
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (i==DEREF_P_CHAR(pc) || d > 3 || pc->isInvul() || pc->dead || !onl)
						continue;
					if ( pc->isPlayer() && pc->crimflag > 0 )
					{
						sprintf((char*)temp,"You better watch your step %s, I am watching thee!!", pc->name);
						npctalkall(i,(char*)temp,1);
						pc_i->antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*30;
					}
					else if ( pc->isPlayer() && pc->isInnocent() && !pc->dead )
					{
						sprintf((char*)temp,"%s is an upstanding citizen, I will protect thee in %s.", pc->name, region[pc->region].name);
						npctalkall(i,(char*)temp,1);
						pc_i->antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*30;
					}
					else if ( d<=10 && (
							(pc->isNpc() && (pc->npcaitype==2))	// evil npc
							|| (pc->isPlayer() && !(pc->isInnocent()) && !(pc->isCriminal()))	// a player, not grey or blue
							|| (pc->attackfirst==1)))	// any agressor
					{
						pc_i->pos.x=pc->pos.x; //Ripper..guards teleport to enemies.
						pc_i->pos.y=pc->pos.y;
						pc_i->pos.z=pc->pos.z;
						soundeffect2(i, 0x01, 0xFE); // crashfix, LB
						staticeffect(i, 0x37, 0x2A, 0x09, 0x06);
						npcattacktarget(i, DEREF_P_CHAR(pc));
						npctalkall(i, "Thou shalt regret thine actions, swine!",1); // ANTISPAM !!! LB
						return;
					}
				}
			}
		}
		break;
	case 10: // Tamed Dragons ..not white wyrm..Ripper
		// so regular dragons attack reds on sight while tamed.
		if(pc_i->isNpc() && pc_i->tamed)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if(d > 10 || pc->isPlayer() || pc->npcaitype!=2)
						continue;
						npcattacktarget(i,DEREF_P_CHAR(pc));
						return;
				}
			}
		}
		break;
	case 11 : // add NPCAI B in scripts to make them attack reds. (Ripper)
		if (!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 10 || pc->isInvul() || pc->dead || !onl)
						continue;
					if (pc->npcaitype!=2 || !pc->isCriminal() || !pc->isMurderer())
					    continue;
						npcattacktarget(i, DEREF_P_CHAR(pc));
				}
			}
		}
		break;
	case 17: break; //Zippy Player Vendors.
	case 30: // why is this the same as case 50???..Ripper
		if (!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 10 || pc->isInvul() || pc->dead || pc->npcaitype!=2 || !onl)
						continue;
						npcattacktarget(i, DEREF_P_CHAR(pc));
						return;
				}
			}
		}
		break;
	case 32: // Pets Guarding..Ripper
		if(pc_i->isNpc() && pc_i->tamed)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 10 || pc->isNpc() || pc->dead || !pc->guarded || !onl)
						continue;
						if(pc->Owns(pc_i))
						{
							npcattacktarget(i,pc->attacker);
							return;
						}
				}
			}
		}
		break;
	case 50://EV/BS
		if (!pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (d > 10 || pc->isInvul() || pc->dead || !onl)
						continue;
						npcattacktarget(i, DEREF_P_CHAR(pc));
						return;
				}
			}
		}
		break;
		// Case 60-70 is Skyfires new AI
	case 96:
	case 60: //Skyfire - Dragon AI
		if (pc_i->war)
		{
			cRegion::RegionIterator4Chars ri(pc_i->pos);
			for (ri.Begin(); ri.GetData() != ri.End(); ri++)
			{
				P_CHAR pc = ri.GetData();
				if (pc != NULL)
				{
					onl = online(DEREF_P_CHAR(pc));
					d = chardist(i, DEREF_P_CHAR(pc));
					if (pc->isNpc() || pc->dead || !onl)
						continue;
						if (d > 4 && pc_i->attacker==DEREF_P_CHAR(pc))
						{
							if (Combat->GetBowType(DEREF_P_CHAR(pc))!=0)
							{
								pc_i->pos.x=pc->pos.x;
								pc_i->pos.y=pc->pos.y;
								pc_i->pos.z=pc->pos.z;
								pc_i->dir=pc->dir;
								teleport(i);
								npctalkall(i,"Foolish Mortal!",0);
							}
						}
						else
						{
							switch(RandomNum(0, 4))
							{
							case 0:	Magic->NPCEBoltTarget(i,DEREF_P_CHAR(pc));			break;
							case 1:	Magic->NPCFlameStrikeTarget(i,DEREF_P_CHAR(pc));	break;
							case 2:	Magic->ParalyzeSpell(i,DEREF_P_CHAR(pc));			break;
							case 3:	Magic->NPCLightningTarget(i,DEREF_P_CHAR(pc));		break;
							case 4:	Magic->ParalyzeSpell(i,DEREF_P_CHAR(pc));			break;
							}
						}
					}
					if (pc_i->hp<(pc_i->st/2))
						Magic->NPCHeal(i);
					if (pc_i->poisoned)
						Magic->NPCCure(i);
					if (pc->priv2&0x20)
						Magic->NPCDispel(i,DEREF_P_CHAR(pc));
					npcattacktarget(i, DEREF_P_CHAR(pc));
					return;
			}
		}
		else
		{
			if (pc_i->hp<(pc_i->st/2))
				Magic->NPCHeal(i);
			if (pc_i->poisoned)
				Magic->NPCCure(i);
		}
		break;
	default:
		clConsole.send("ERROR: cCharStuff::CheckAI-> Error npc %i (%x %x %x %x) has invalid AI type %i\n",i,pc_i->ser1,pc_i->ser2,pc_i->ser3,pc_i->ser4,pc_i->npcaitype); //Morrolan
		return;
	}// switch
}// void checknpcai



