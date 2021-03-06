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
#include "itemid.h"
#include "SndPkg.h"
#include "utilsys.h"

#include "debug.h"
#undef  DBGFILE
#define DBGFILE "combat.cpp"
#define SWINGAT (unsigned int)1.75 * MY_CLOCKS_PER_SEC // changed from signed to unsigned, LB

int cCombat::GetBowType(int c)
{
	int ci=0,loopexit=0;
	P_ITEM pi;
	while ( ((pi=ContainerSearch(chars[c].serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
	{
		if (pi->layer==1 || pi->layer==2)
		{
			switch( pi->id() )
			{
			case 0x13B1:
			case 0x13B2:	return 1;	// bows
			case 0x0F4F:
			case 0x0F50:	return 2;	// crossbow
			case 0x13FC:
			case 0x13FD:	return 3;	// heavy xbow
			}
		}
	}
	return 0;
}

void cCombat::ItemCastSpell(UOXSOCKET s, CHARACTER c, P_ITEM pi)//S=Socket c=Char # Target i=Item //Itemid
{
	if(!pi) return;
//	P_ITEM pi=MAKE_ITEMREF_LR(ii);
	int cc=currchar[s];
	P_CHAR pc_currchar = MAKE_CHARREF_LR(currchar[s]);
	unsigned short int spellnum=((pi->morex*8)-8)+pi->morey;
	unsigned short int tempmana=pc_currchar->mn;//Save their mana so we can give it back.
	unsigned short int tempmage=pc_currchar->skill[MAGERY];//Easier than writing new functions for all these spells


	if(pi->type!=15) return;
	
	if(pi->morez<=0) return;
	
//	pc_currchar->skill[MAGERY] = RandomNum(600, 1000);//For damage
#pragma note("Why are we gambling with his skill?")
	
	switch(spellnum)
	{
	case 1: Magic->ClumsySpell(cc,c); break; //LB
	case 3: Magic->FeebleMindSpell(cc,c); break; //LB
	case 5:	Magic->MagicArrow(cc,c);		break; // lB
	case 8: Magic->WeakenSpell(cc,c); break; //LB
	case 18: Magic->FireballSpell(cc,c); break; //LB
	case 22: Magic->HarmSpell(cc,c); break; //LB
	case 27: Magic->CurseSpell(cc,c); break; //LB
	case 30: Magic->NPCLightningTarget(cc,c); break; //lb
	case 37: Magic->MindBlastSpell(cc,c); break;
	case 38: Magic->ParalyzeSpell(cc,c);	break; //lb
	case 42: Magic->NPCEBoltTarget(cc,c); break;
	case 43: Magic->ExplosionSpell(cc,c); break;
	case 51: Magic->NPCFlameStrikeTarget(cc,c); break;
	default:
		staticeffect(cc, 0x37, 0x35, 0, 30);
		soundeffect2(cc, 0x00, 0x5C);
		break;
	}
	pc_currchar->mn+=tempmana;
	pc_currchar->skill[MAGERY]=tempmage;
	if(pc_currchar->in<pc_currchar->mn) pc_currchar->mn=pc_currchar->in;//Shouldn't happen, but just in case;
	updatestats(s, 1);
	
	pi->morez--;
	if(pi->morez==0)//JUST lost it's charge....
		sysmessage(s, "This item is out of charges.");
}

void CheckPoisoning(UOXSOCKET sd, P_CHAR pc_attacker, P_CHAR pc_defender)
{
	if ((pc_attacker->poison)&&(pc_defender->poisoned<pc_attacker->poison))
	{
		if (rand()%3==0) // || fightskill==FENCING)//0 1 or 2 //fencing always poisons :) - AntiChrist
		{
			pc_defender->poisoned=pc_attacker->poison;
			pc_defender->poisontime=uiCurrentTime+(MY_CLOCKS_PER_SEC*(40/pc_defender->poisoned)); // a lev.1 poison takes effect after 40 secs, a deadly pois.(lev.4) takes 40/4 secs - AntiChrist
			pc_defender->poisonwearofftime=pc_defender->poisontime+(MY_CLOCKS_PER_SEC*SrvParms->poisontimer); //wear off starts after poison takes effect - AntiChrist
			if (sd != -1) 
			{
				impowncreate(sd,DEREF_P_CHAR(pc_defender),1); //Lb, sends the green bar ! 
				sysmessage(sd,"You have been poisoned!");//AntiChrist 
			}
		}
	}
}

// This checks LineOfSight before calling CombatHit (Duke, 10.7.2001)
void cCombat::CombatHitCheckLoS(P_CHAR pAttacker, unsigned int currenttime)
{
	P_CHAR pDefender = MAKE_CHARREF_LR(pAttacker->swingtarg);
	UOXSOCKET s1=calcSocketFromChar(DEREF_P_CHAR(pAttacker));

	unsigned short los=line_of_sight(s1,pAttacker->pos, pDefender->pos,	WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING);

	CombatHit(DEREF_P_CHAR(pAttacker),DEREF_P_CHAR(pDefender),currenttime,los);
}

// CombatHit now expects that LineOfSight has been checked before (Duke, 10.7.2001)
void cCombat::CombatHit(int a, int d, unsigned int currenttime, short los)
{
	if (d==-1) return;
	char hit;

	P_CHAR pc_attacker = MAKE_CHARREF_LR(a);
	P_CHAR pc_deffender = MAKE_CHARREF_LR(d);

	UOXSOCKET s1=calcSocketFromChar(a), s2=calcSocketFromChar(d);
	unsigned short fightskill=Skills->GetCombatSkill(a), bowtype=Combat->GetBowType(a),splitnum,splitcount,hitin;
	unsigned int basedamage;
	int damage; // removed from unsigne by Magius(CHE)
	signed int x;
	// Magius(CHE) - For armour absorbtion system
	char /*t[512],*/debabs[512]; 
	int maxabs, maxnohabs, tmpj;


	P_ITEM pWeapon=pc_attacker->getWeapon();// get the weapon item only once

	if (pWeapon && !(rand()%50)	// a 2 percent chance (Duke, 07.11.2000)
		&& pWeapon->type !=9)	// but not for spellbooks (Duke, 09/10/00)
	{
		pWeapon->hp--; //Take off a hit point
		if(pWeapon->hp<=0)
		{
			sysmessage(s1,"Your weapon has been destroyed");
			if ((pWeapon->trigon==1) && (pWeapon->layer>0))// -Frazurbluu- Trigger Type 2 is my new trigger type *-
			{
				triggerwitem(s1, DEREF_P_ITEM(pWeapon), 1); // trigger is fired when item destroyed
			}				
			Items->DeleItem(pWeapon);
		}
	}

	*debabs='\0'; 
//	*t='\0';
	// End here - Magius(CHE) - For armour absorbtion system



	pc_attacker->swingtarg=-1;

	if((chardist(a,d)>1 && fightskill!=ARCHERY) || !los) return;
	if(pc_deffender->isNpc() && pc_deffender->isInvul()) return; // ripper


	hit=Skills->CheckSkill(a, fightskill, 0, 1000);  // increase fighting skill for attacker and defender
	if (!hit)
	{
		if (pc_attacker->isPlayer())
			doMissedSoundEffect(a);
		if ((fightskill==ARCHERY)&&(los))
		{
			if (rand()%3-1)//-1 0 or 1
			{
				short id=0x1BFB;	// bolts
				if (bowtype==1)
					id=0x0F3F;		// arrows

				P_ITEM pAmmo=Items->SpawnItem(d,1,"#",1,id,0,0);
				if(pAmmo)
				{
					pAmmo->MoveTo(pc_deffender->pos.x,pc_deffender->pos.y,pc_deffender->pos.z);
					pAmmo->priv=1;
					RefreshItem(pAmmo);
				}
			}
		}
	}
	else
	{
		if (!pc_deffender->isInvul())
		{
			if (pc_deffender->xid2==0x91) soundeffect2(d,0x01,0x4b);
			if (pc_deffender->xid2==0x90) soundeffect2(d,0x01,0x56);
			playmonstersound(d, pc_deffender->id1, pc_deffender->id2, SND_DEFEND);
			//AntiChrist -- for poisoned weapons
			CheckPoisoning(s2, pc_attacker, pc_deffender);	// attacker poisons defender
			if (fightskill!=ARCHERY)	// only for melee (Duke,21.4.01)
				CheckPoisoning(s1, pc_deffender, pc_attacker); // and vice versa

			if ((pc_deffender->effDex()>0)) pc_deffender->priv2&=0xFD;	// unfreeze

			if (fightskill!=WRESTLING && los)
				Combat->ItemSpell(a, d);
			
			if (fightskill!=WRESTLING || pc_attacker->isNpc())
				basedamage=Combat->CalcAtt(a); // Calc base damage
			else
			{
				if ((pc_attacker->skill[WRESTLING]/100) > 0) 
				{
					if (pc_attacker->skill[WRESTLING]/100!=0)
						basedamage=rand()%(pc_attacker->skill[WRESTLING]/100);
					else basedamage=0;
				}
				else basedamage=rand()%2;
			}

			if((pc_attacker->isPlayer()) && (fightskill!=WRESTLING))
			{ 
				if (pWeapon->racehate != 0 && pc_deffender->race != 0)//-Fraz- Racehating combat
				{
					if (pWeapon->racehate==pc_deffender->race)
					{
						basedamage *=2;
							if(pc_deffender->isPlayer())
							{
								sysmessage(s2,"You sceam in agony from being hit by the accursed metal!");
								if (pc_deffender->xid2==0x91) soundeffect2(d,0x01,0x52);
								else if (pc_deffender->xid2==0x90) soundeffect2(d,0x01,0x57);
							}// can add a possible effect below here for npc's being hit
					}
							
				}
			}
			Skills->CheckSkill(a, TACTICS, 0, 1000);
			damage=(int)(basedamage*((pc_attacker->skill[TACTICS]+500.0)/1000.0)); // Add Tactical bonus
			damage=damage+(int)((basedamage*(pc_attacker->st/500.0))); // Add Strength bonus

			//Adds a BONUS DAMAGE for ANATOMY
			//Anatomy=100 -> Bonus +20% Damage - AntiChrist (11/10/99)
			if (Skills->CheckSkill(a, ANATOMY, 0, 1000))
			{
				float multiplier=(((pc_attacker->skill[ANATOMY]*20)/1000.0f)/100.0f)+1;
				damage=(int)  (damage * multiplier);
			}
			//Adds a BONUS DEFENCE for TACTICS
			//Tactics=100 -> Bonus -20% Damage - AntiChrist (11/10/99)
			float multiplier=1-(((pc_deffender->skill[TACTICS]*20)/1000.0f)/100.0f);
			damage=(int)  (damage * multiplier);
			P_ITEM pShield=pc_deffender->getShield();
			if(pShield)
			{
				if (Skills->CheckSkill(d, PARRYING, 0, 1000))// chance to block with shield
				{
					if (pShield->def!=0) damage-=rand()%(pShield->def);// damage absorbed by shield
					if(rand()%2) pShield->hp--; //Take off a hit point
					if(pShield->hp<=0)
					{
						sysmessage(s2,"Your shield has been destroyed");
						if ((pShield->trigon==1) && (pShield->layer >0))// -Frazurbluu- Trigger Type 2 is my new trigger type *-
						{
							triggerwitem(s2, DEREF_P_ITEM(pShield), 1); // trigger is fired when item destroyed
						}	
						Items->DeleItem(pShield);
					}
				}
			}
			// Armor destruction and sped up by hitting with maces should go in here somewhere 
			// According to lacation of body hit Id imagine -Frazurbluu- **NEEDS ADDED**
			x=rand()%100;// determine area of body hit
			if (SrvParms->combathitmessage!=1)
			{
				if (x<=44) x=1; // body
				else if (x<=58) x=2; // arms
				else if (x<=72) x=3; // head
				else if (x<=86) x=4; // legs
				else if (x<=93) x=5; // neck
				else x=6; // hands
			}
			if (SrvParms->combathitmessage==1)
			{
				temp[0] = '\0';
				hitin = rand()%2;
				if (x<=44)
				{
					x=1;       // body
					switch (hitin)
					{
					case 1:
						//later take into account dir facing attacker during battle
						if (damage < 10) strcpy(temp, "hits you in your Chest!");
						if (damage >=10) strcpy(temp, "lands a terrible blow to your Chest!");
						break;
					case 2:
						if (damage < 10) strcpy(temp, "lands a blow to your Stomach!");
						if (damage >=10) strcpy(temp, "knocks the wind out of you!");
						break;
					default:
						if (damage < 10) strcpy(temp, "hits you in your Ribs!");
						if (damage >=10) strcpy(temp, "broken your Rib?!");
					}
				}
				else if (x<=58)
				{
					if (damage > 1)
					{
						x=2;  // arms
						switch (hitin)
						{
						case 1:	strcpy(temp, "hits you in Left Arm!");	break;
						case 2:	strcpy(temp, "hits you in Right Arm!");	break;
						default:strcpy(temp, "hits you in Right Arm!");
						}
					}
				}
				else if (x<=72)
				{
					x=3;  // head
					switch (hitin)
					{
					case 1:
						if (damage < 10) strcpy(temp, "hits you you straight in the Face!");
						if (damage >=10) strcpy(temp, "lands a stunning blow to your Head!");
						break;
					case 2:
						if (damage < 10) strcpy(temp, "hits you to your Head!"); //kolours - (09/19/98)
						if (damage >=10) strcpy(temp, "smashed a blow across your Face!");
						break;
					default:
						if (damage < 10) strcpy(temp, "hits you you square in the Jaw!");
						if (damage >=10) strcpy(temp, "lands a terrible hit to your Temple!");
					}
				}
				else if (x<=86) 
				{
					x=4;  // legs
					switch (hitin)
					{
					case 1:	strcpy(temp, "hits you in Left Thigh!");	break;
					case 2:	strcpy(temp, "hits you in Right Thigh!");	break;
					default:strcpy(temp, "hits you in Groin!");
					}
				}
				else if (x<=93)
				{
					x=5;  // neck
					strcpy(temp, "hits you to your Throat!");
				}
				else
				{
					x=6;  // hands
					switch (hitin)
					{
					case 1:
						if (damage > 1) strcpy(temp, "hits you in Left Hand!");
						break;
					case 2:
						if (damage > 1) strcpy(temp, "hits you in Right Hand!");
						break;
					default:
						if (damage > 1) strcpy(temp, "hits you in Right Hand!");
					}
				}

				sprintf((char*)temp2,"%s %s",pc_attacker->name, temp);
				if (pc_deffender->isPlayer() && s2!=-1) sysmessage(s2, (char*)temp2); //kolours -- hit display
			}
			x = CalcDef(pc_deffender,x);
			
			// Magius(CHE) - For armour absorbtion system
			maxabs = 20; //
			           // there are monsters with DEF >20, this makes them undefeatable
			maxnohabs=100;
			if (SrvParms->maxabsorbtion>0) maxabs = SrvParms->maxabsorbtion;
			else {
				clConsole.send("SERVER.SCP:ComabatHit() Error in MAX_ABSORBTION. Reset to Deafult (20).\n");
				SrvParms->maxabsorbtion=maxabs;
			}
			if (SrvParms->maxnohabsorbtion>0) maxnohabs=SrvParms->maxnohabsorbtion;
			else {
				clConsole.send("SERVER.SCP:ComabatHit() Error in MAX_NON_HUMAN_ABSORBTION. Reset to Deafult (100).\n");
				SrvParms->maxnohabsorbtion=maxnohabs;
			}
			if (!ishuman(d)) maxabs=maxnohabs;
			tmpj=(int) (damage*x)/maxabs; // Absorbtion by Magius(CHE)
			damage -= tmpj;
			if (damage<0) damage=0;
			if (pc_deffender->isPlayer()) damage /= SrvParms->npcdamage; // Rate damage against other players
			// End Armour Absorbtion by Magius(CHE) (See alse reactive armour spell damage)

			if (pc_attacker->isPlayer())//Zippy
				ItemCastSpell(s1,d,pWeapon);

			//AntiChrist - 26/10/99
			//when hitten and damage >1, defender fails if casting a spell!
			if(damage>1 && pc_deffender->isPlayer())//only if damage>1 and against a player
			{
				if(pc_deffender->casting && currentSpellType[s2]==0 )
				{//if casting a normal spell (scroll: no concentration loosen)
					currentSpellType[s2]=0;
					pc_deffender->spell=-1;
					pc_deffender->casting=0;
					pc_deffender->spelltime=0;
					pc_deffender->priv2=pc_deffender->priv2&0xfd; // unfreeze, bugfix LB
					Magic->SpellFail(s2);
				}
			}

			if(damage>0)
			{
				if (pc_deffender->ra) // For reactive armor spell
				{
					// -Frazurbluu- RA may need a rewrite to be more OSI standard here
					// Its said 80% deflected 10% to attacker / 10% defender gotta check special effects
					int damage1;
					damage1=(int)( damage*(pc_deffender->skill[MAGERY]/2000.0));
					pc_deffender->hp -= damage-damage1;
					if (pc_deffender->isNpc()) damage1 = damage1 * SrvParms->npcdamage; // by Magius(CHE)
					pc_attacker->hp -= damage1;  // Remove damage from attacker
					staticeffect(d, 0x37, 0x4A, 0, 15);//RA effect - AntiChrist (9/99)
					if ((fightskill==MACEFIGHTING) && (IsSpecialMace(pWeapon->id())))// Stamina Loss -Fraz-
					{ 
						//pc_attacker->stm-=3+(rand()%4);
					}
					if ((fightskill==FENCING) && (IsFencing2H(pWeapon->id())))// Paralyzing -Fraz-
					{ 
						//will call the combat caused paralyzation **NEED TO DO**
					}
					if ((fightskill==SWORDSMANSHIP) && (IsAxe(pWeapon->id())))// Concussion Hit -Fraz-
					{ 
						//will call the combat caused concussion (loss of int for 30 secs) **NEED TO DO**
						// for now make it subtract mana
						//pc_attacker->mn-=(pc_attacker->mn/2);
					}
					updatestats(a, 0);
				}
				else 
				{	// -Fraz- Now needs adjusted to happen on a skill percentage 
					pc_deffender->hp-=damage; // Remove damage from defender only apply special hits to non-npc's
					if ((fightskill==MACEFIGHTING) && (IsSpecialMace(pWeapon->id())) && (pc_deffender->isPlayer()))// Stamina Loss -Fraz-
					{ 
						pc_deffender->stm-=3+(rand()%3);
					}
					if ((fightskill==FENCING) && (IsFencing2H(pWeapon->id())) && (pc_deffender->isPlayer()))// Paralyzing -Fraz-
					{ 
						tempeffect(a, d, 44, 0, 0, 0);
						sysmessage(s1,"You delivered a paralyzing blow");
											}
					if ((fightskill==SWORDSMANSHIP) && (IsAxe(pWeapon->id())) && (pc_deffender->isPlayer()))// Concussion Hit -Fraz-
					{ 
						tempeffect(a, d, 45, 0, 0, 0);
						//pc_attacker->mn-=(pc_attacker->mn/2); //-Fraz- temp use of this for concussion
					}
					updatestats(d, 0);
				}
				// blood shred by blackwind
				if (damage>10)
				{
	               short id=0x122c;	
	               if (damage>20) id=0x122b;
	               if (damage>30) id=0x122e;
				   if (damage>40) id=0x122d;
	               if (damage>50) id=0x122a;
				   P_ITEM pBlood=Items->SpawnItem(d,1,"#",0,id,0,0);
				   if (pBlood)
				   {
					  pBlood->MoveTo(pc_deffender->pos.x, pc_deffender->pos.y, pc_deffender->pos.z);
					  pBlood->priv = 1;
					  pBlood->magic = 2; //Moveable by GM
					  RefreshItem(pBlood);
					  pBlood->decaytime = (SrvParms->decaytimer/2)*MY_CLOCKS_PER_SEC+uiCurrentTime;
				   }
				}

				/////////  For Splitting NPCs ///  McCleod
				if ((pc_deffender->split>0)&&(pc_deffender->hp>=1))
				{
					if (rand()%100<=pc_deffender->splitchnc)
					{
						if (pc_deffender->split==1) splitnum=1;
						else splitnum=rand()%pc_deffender->split+1;
						
						for (splitcount=0;splitcount<splitnum;splitcount++)
							Npcs->Split(d);
					}
				}
				////////      End of spliting NPCs
			}
			if (pc_attacker->isPlayer())
				if((fightskill==ARCHERY && los)|| fightskill!=ARCHERY)
					doSoundEffect(a, fightskill, pWeapon);
			if (pc_deffender->hp<0) pc_deffender->hp=0;
			updatestats(d, 0);
			x=(pc_deffender->id1<<8)+pc_deffender->id2;
			if (x>=0x0190)
			{
				if (!pc_deffender->onhorse) npcaction(d, 0x14);
			}
		}
	}
}

static void NpcSpellAttack(P_CHAR pc_attacker, P_CHAR pc_defender, unsigned int currenttime, int los)
{
	if (pc_attacker->spatimer<=currenttime)
	{
		int a = DEREF_P_CHAR(pc_attacker);
		int d = DEREF_P_CHAR(pc_defender);
		int spattacks = numbitsset( pc_attacker->spattack );

		if (!pc_defender->dead && chardist(a,d)<server_data.attack_distance && spattacks > 0 )
		{
			if (los)
			{																	
				int spattackbit=rand()%(spattacks) + 1;
				switch(whichbit(pc_attacker->spattack, spattackbit))
				{
				case 1:
					if (pc_attacker->mn>=4)
					{
						npcaction(a, 6);
						Magic->MagicArrow(a,d);
					}
					break; 
				case 2:
					if (pc_attacker->mn>=6)
					{
						npcaction(a, 6);
						Magic->HarmSpell(a,d);
					}
					break; //lb
				case 3:
					if (pc_attacker->mn>=4)
					{
						npcaction(a, 6);
						Magic->ClumsySpell(a,d);										
					}
					break; //LB
				case 4:
					if (pc_attacker->mn>=4)
					{
						npcaction(a, 6);
						Magic->FeebleMindSpell(a,d);
					}
					break; //LB
				case 5:
					if (pc_attacker->mn>=4)
					{
						npcaction(a, 6);
						Magic->WeakenSpell(a,d);
					}
					break; //LB
				case 6:
					if (pc_attacker->mn>=9)
					{
						npcaction(a, 6);
						Magic->FireballSpell(a,d);
					}
					break; //LB
				case 7:
					if (pc_attacker->mn>=11)
					{
						npcaction(a, 6);
						Magic->CurseSpell(a,d);
					}
					break; //LB
				case 8:
					if (pc_attacker->mn>=11)
					{
						npcaction(a, 6);
						Magic->NPCLightningTarget(a,d);
					}
					break; //lb
				case 9:
					if (pc_attacker->mn>=14)
					{
						npcaction(a, 6);
						Magic->ParalyzeSpell(a,d);
					}
					break; //lb
				case 10:
					if (pc_attacker->mn>=14)
					{
						npcaction(a, 6);
						Magic->MindBlastSpell(a,d);
					}
					break;
				case 11:
					if (pc_attacker->mn>=20)
					{
						npcaction(a, 6);
						Magic->NPCEBoltTarget(a,d);
					}
					break;
				case 12:
					if (pc_attacker->mn>=20)
					{
						npcaction(a, 6);
						Magic->ExplosionSpell(a,d);
					}
					break;
				case 13:
					if (pc_attacker->mn>=40)
					{
						npcaction(a, 6);
						Magic->NPCFlameStrikeTarget(a,d);
					}
					break;
				case 14:
					npcaction(a, 6);
					Magic->PFireballTarget(a, d, 10);
					break;
				case 15:
					npcaction(a, 6);
					Magic->PFireballTarget(a, d, 20);
					break;
				case 16:
					npcaction(a, 6);
					Magic->PFireballTarget(a, d, 40);
					break;
				}
			}			
		}
		pc_attacker->spatimer=currenttime+(pc_attacker->spadelay*MY_CLOCKS_PER_SEC); //LB bugkilling
	}
}				

// Formulas take from OSI's combat formulas
// attack speed should be determined here.
// attack speed = 15000 / ((DEX+100) * weapon speed)
static void SetWeaponTimeout(P_CHAR pc_attacker, P_ITEM pWeapon)
{
	int x,j;
	
	if (pWeapon) 
	{ 
		if (pWeapon->spd==0) pWeapon->spd=35;
		x = (15000 / ((pc_attacker->effDex()+100) * pWeapon->spd)*MY_CLOCKS_PER_SEC); //Calculate combat delay
	}
	else 
	{
		if(pc_attacker->skill[WRESTLING]>200) j = 35;
		else if(pc_attacker->skill[WRESTLING]>400) j = 40;
		else if(pc_attacker->skill[WRESTLING]>600) j = 45;
		else if(pc_attacker->skill[WRESTLING]>800) j = 50;
		else j = 30;
		x = (15000 / ((pc_attacker->effDex()+100) * j)*MY_CLOCKS_PER_SEC);
	}
	pc_attacker->timeout=uiCurrentTime+x;
}


void cCombat::DoCombatAnimations(P_CHAR pc_attacker, P_CHAR pc_defender, int fightskill, int bowtype, int los)
{
	short id = (pc_attacker->id1<<8)+pc_attacker->id2;
	CHARACTER a=DEREF_P_CHAR(pc_attacker);
	int cc,aa;
	if (id<0x0190)
	{
		aa=4+(rand()%3); // bugfix, LB, some creatures dont have animation #4
		cc=(creatures[id].who_am_i)&0x2; // anti blink bit set ?
		if (cc==2)
		{
			aa++;
			if (id==5) // eagles need special treatment
			{
				switch (RandomNum(0, 2))
				{
				case 0: aa=0x1;  break;
				case 1: aa=0x14; break;
				case 2: aa=0x4;  break;
				}
			}
		}
		npcaction(a,aa); 
		playmonstersound(a, pc_attacker->id1, pc_attacker->id2, SND_ATTACK);
	}
	else if (pc_attacker->onhorse)
	{
		CombatOnHorse(a);	// determines weapon in hand and runs animation kolours (09/19/98)
	}
	else
	{
		CombatOnFoot(a);	// determines weapon in hand and runs animation kolours (09/19/98)
	}

	if (fightskill==ARCHERY)
	{
		if (los)
		{
			CHARACTER d=DEREF_P_CHAR(pc_defender);
			if (bowtype==1)
			{
				delequan(a, 0x0F3F, 1, NULL);
				movingeffect3(a, d, 0x0F, 0x42, 0x08, 0x00, 0x00,0,0,0,0);
			}
			else
			{
				delequan(a, 0x1BFB, 1, NULL);
				movingeffect3(a, d, 0x1B, 0xFE, 0x08, 0x00, 0x00,0,0,0,0);
			}
		}
	}
}

void cCombat::DoCombat(int a, unsigned int currenttime)
{
	int x, bowtype=0;// spamanachecking, tmp;

	P_CHAR pc_attacker = MAKE_CHARREF_LR(a);
	if (pc_attacker->free) return;
	P_ITEM pWeapon=pc_attacker->getWeapon();

	int d = pc_attacker->targ;
	P_CHAR pc_defender = MAKE_CHAR_REF(d);

	if (d==-1 || (pc_defender->isPlayer() && !online(d) || pc_defender->isHidden()) && pc_attacker->war)
	{
		pc_attacker->war=0; // LB
		pc_attacker->timeout=0;
		pc_attacker->attacker=-1;
		pc_attacker->resetAttackFirst();
		return;
	}
	if (( pc_attacker->isNpc() || online(a) ) && d != -1 )
	{			
		if (d<=-1 || d>=cmem) return;
		if (pc_defender->free) return;
		
		if ((pc_defender->isNpc() && pc_defender->npcaitype!=17) || (online(d) && !pc_defender->dead) ) // ripper		
		{
			if (chardist(a,d) > SrvParms->attack_distance)
			{
				if (pc_attacker->npcaitype==4 && pc_attacker->inGuardedArea()) // changed from 0x40 to 4, LB
				{
					pc_attacker->MoveTo(pc_defender->pos.x,pc_defender->pos.y,pc_defender->pos.z);
					
					teleport(a);
					soundeffect2(a, 0x01, 0xFE); // crashfix, LB
					staticeffect(a, 0x37, 0x2A, 0x09, 0x06);
					npctalkall(a,"Halt, scoundrel!",1);
				}
				else 
				{ // else -> npcaityes != 4
					pc_attacker->targ=-1;
					pc_attacker->timeout=0;
					if (pc_attacker->attacker>=0 && pc_attacker->attacker<cmem)
					{
						chars[pc_attacker->attacker].resetAttackFirst();
						chars[pc_attacker->attacker].attacker=-1; // lb crashfix
					}
					pc_attacker->attacker=-1;
					pc_attacker->resetAttackFirst();
					if (pc_attacker->isNpc() && pc_attacker->npcaitype!=17 && !pc_attacker->dead && pc_attacker->war)
						npcToggleCombat(a); // ripper
				}
			}
			else
			{
				if (pc_attacker->targ==-1)
				{
					npcsimpleattacktarget(a,d);
					x=(((100-pc_attacker->effDex())*MY_CLOCKS_PER_SEC)/25)+(1*MY_CLOCKS_PER_SEC); //Yet another attempt.
					pc_attacker->timeout=currenttime+x;
					return;
				}
				if (Combat->TimerOk(a))
				{
					int los = line_of_sight(-1, pc_attacker->pos, pc_defender->pos, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING);
					UOXSOCKET s1 = calcSocketFromChar(a);
					int fightskill=Skills->GetCombatSkill(a);
					x=0;
					if (fightskill==ARCHERY)
					{
						if (los)
						{
							int arrowsquant;
							bowtype=Combat->GetBowType(a);
							if (bowtype==1) arrowsquant=getamount(a, 0x0F3F);
							else arrowsquant=getamount(a, 0x1BFB);
							if (arrowsquant>0)
								x=1;
							//else
							//	sysmessage(s1, "You are out of ammunitions!"); //-Fraz- this message can cause problems removed
						}
					}
					if ( chardist(a,d)<2 && fightskill!=ARCHERY ) x=1;
					if (x)
					{
						// - Do stamina maths - AntiChrist (6) -
						if(abs(SrvParms->attackstamina)>0 && !pc_attacker->isGM())
						{
							if((SrvParms->attackstamina<0)&&(pc_attacker->stm<abs(SrvParms->attackstamina)))
							{
								sysmessage(s1,"You are too tired to attack.");
								SetWeaponTimeout(pc_attacker, pWeapon);
								return;
							}
							pc_attacker->stm += SrvParms->attackstamina;
							if (pc_attacker->stm>pc_attacker->effDex()) pc_attacker->stm=pc_attacker->effDex();
							if (pc_attacker->stm<0) pc_attacker->stm=0;
							updatestats(a,2); //LB, crashfix, was currchar[a]
							// --------ATTACK STAMINA END ------
						}
						
						DoCombatAnimations( pc_attacker, pc_defender, fightskill, bowtype, los);

						if (((chardist(a,d)<2)||(fightskill==ARCHERY))&&!(pc_attacker->npcaitype==4)) // changed from 0x40 to 4
                        {
							if (los)
							{
								npcsimpleattacktarget(a,d);
							}
						}
						if (pc_attacker->timeout2 > uiCurrentTime) return; //check shotclock memory-BackStab
						if (fightskill==ARCHERY)
							Combat->CombatHit(a,d,currenttime,los);
						else
							pc_attacker->swingtarg=d;
					}

					SetWeaponTimeout(pc_attacker, pWeapon);
					pc_attacker->timeout2=pc_attacker->timeout; // set shotclock memory-BackStab
					if (!pc_defender->isInvul())
					{
						NpcSpellAttack(pc_attacker,pc_defender,currenttime,los);
					}
					if (fightskill!=ARCHERY)
					{
						Combat->CombatHit(a,d,currenttime,los); // LB !!!
						//return; // Ripper	???? (Duke)
					}
				}
			}			
			if (pc_defender->hp<1)//Highlight //Repsys
			{
				if(pc_attacker->npcaitype==4 && pc_defender->isNpc())
				{
					npcaction(d, 0x15);
					
					PlayDeathSound(d);
					
					Npcs->DeleteChar(d);//Guards, don't give body
				}
				else
				{
					deathstuff(d);
				}
				//murder count \/
				
				if ((pc_attacker->isPlayer())&&(pc_defender->isPlayer()))//Player vs Player
				{
					if(pc_defender->isInnocent() && Guilds->Compare(a,d)==0 )
					{
						pc_attacker->kills++;
						sprintf((char*)temp, "You have killed %i innocent people.", pc_attacker->kills);
						sysmessage(calcSocketFromChar(a),(char*)temp);
						//clConsole.send("DEBUG %s's kills are now -> %i\n",pc_attacker->name,pc_attacker->kills);
						if (pc_attacker->kills==repsys.maxkills+1)
							sysmessage(calcSocketFromChar(a),"You are now a murderer!");
					}
					
					if (SrvParms->pvp_log)
					{
						sprintf((char*)temp,"%s was killed by %s!\n",pc_defender->name,pc_attacker->name);
						savelog((char*)temp,"PvP.log");
					}
					
				}
				npcToggleCombat(a);
				return; // LB
			}
		}
	}
}

int cCombat::CalcAtt(int p) // Calculate total attack powerer
{
	int total = 0, j;
	
	P_CHAR pc_p = MAKE_CHARREF_LRV(p,0);

	if(pc_p->isNpc()) 
	{
		//Compatibility for use with older scripts
		if((pc_p->lodamage==0)&&(pc_p->hidamage==0)) return(pc_p->att);
		
		//Code for new script commands
		if(pc_p->lodamage==pc_p->hidamage) //Avoids divide by zero error
		{
			return(pc_p->lodamage);
		}
		if(!(pc_p->hidamage<=2))
		{
			j=pc_p->hidamage-pc_p->lodamage; 
			if (j!=0) total=(rand()%(j))+pc_p->lodamage; else total=pc_p->lodamage;
		}
		
		if(total<1) total=1;
		return(total);
	}
	
	int ci=0,loopexit=0;
	P_ITEM pi;
	while ( ((pi=ContainerSearch(pc_p->serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
	{
		if (pi->layer==1 || pi->layer==2)
		{
			if (pi->lodamage == pi->hidamage) total+=pi->lodamage;
			else if ((pi->lodamage>0)&&(pi->hidamage>0))
			{
				if (pi->hidamage-pi->lodamage!=0) total+=(rand()%(pi->hidamage-pi->lodamage))+(pi->lodamage); 
				else total+=pi->lodamage;
			}
			else if ((pi->lodamage==0)&&(pi->hidamage==0)) total+=pi->att;
		}
	}
	if (total<1) total=1;
	return total;
}

int cCombat::CalcDef(P_CHAR pc,int x) // Calculate total defense power 
{ 
	P_ITEM pj=NULL; 
	P_ITEM pShield = pc->getShield(); 
	unsigned int total=pc->def; 
	if (x==0) // -Fraz- added parrying skill bonuses
	{ 
		if (pShield) 
			total+=1+(((pc->skill[PARRYING]/10) * pShield->def) / 200); // -Fraz- These values may need tweaked to OSI
	} 
	if (pc->skill[PARRYING]==1000) 
		total+=5; // gm parry bonus. 
	if (ishuman(DEREF_P_CHAR(pc))) // Added by Magius(CHE) 
	{ 
		int ci=0,loopexit=0; 
		P_ITEM pi; 
		while ( ((pi=ContainerSearch(pc->serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
		{ 
			if (pi->layer>1 && pi->layer<25) 
			{ 
				//blackwinds new stuff 
				int armordef=0; 
				if (pi->def>0) 
				{ 
					float armorhpperc=((float)pi->hp/((float)pi->maxhp/100)); 
					armordef=(int)(((float)pi->def/100)*armorhpperc); 
				} 

				if (x==0) 
					total+=pi->def; 
				else 
				{ 
					switch (pi->layer) 
					{ 
					case 5: 
					case 13: 
					case 17: 
					case 20: 
					case 22: 
						if (x==1 && pi->def>total) 
						{ 
							total=pi->def; 
							pj=pi; 
						} 
						break; 
					case 19: 
						if (x==2 && pi->def>total) 
						{ 
							total=pi->def; 
							pj=pi; 
						} 
						break; 
					case 6: 
						if (x==3 && pi->def>total) 
						{ 
							total=pi->def; 
							pj=pi; 
						} 
						break; 
					case 3: 
					case 4: 
					case 12: 
					case 23: 
					case 24: 
						if (x==4 && pi->def>total) 
						{ 
							total=pi->def; 
							pj=pi; 
						} 
						break; 
					case 10: 
						if (x==5 && pi->def>total) 
						{ 
							total=pi->def; 
							pj=pi; 
						} 
						break; 
					case 7: 
						if (x==6 && pi->def>total) 
						{ 
							total=pi->def; 
							pj=pi; 
						} 
						break; 
					default: 
						break; 
					} 
				} 
			} 
		} 
	} else pj=NULL; 

	UOXSOCKET k=calcSocketFromChar(pc); 
	if (pj && k!=-1) 
	{ 
		//AntiChrist - don't damage hairs, beard and backpack 
		//important! this sometimes cause backpack destroy! 
		// 
		if(pj->layer!=0x0B && pj->layer!=0x10 && pj->layer!=0x15) // bugfix lB,was 0x15, 0x15,0x15 !! 
		{ 
			if((rand()%2)==0) 
				pj->hp--; //Take off a hit point 
			if(pj->hp<=0) 
			{ 
				sprintf((char*)temp,"Your %s has been destroyed", pj->getName().c_str());
				pc->removeItemBonus(pj);	// remove BONUS STATS given by equipped special items
				//-Frazurbluu-  need to have tactics bonus removed also
				if ((pj->trigon==1) && (pj->layer >0))// -Frazurbluu- Trigger Type 2 is my new trigger type *-
				{
					triggerwitem(k, DEREF_P_ITEM(pj), 1); // trigger is fired when item destroyed
				}
				sysmessage(k,(char*)temp);
				Items->DeleItem(pj);		 
			}
			statwindow(k,currchar[k]);
		}
	}
	if (total < 2) total = 2;
	return total;
}

int cCombat::TimerOk(int c)
{
	int d=0;
	P_CHAR pc = MAKE_CHARREF_LRV(c,0);
	//if (chars[c].timeout<uiCurrentTime) d=1;
	//---BackStab-- Tab-Machinegun stopper- meet both timeout requirements---------- 
    if(( pc->timeout < uiCurrentTime )&&(pc->timeout2 < uiCurrentTime)) d=1;
	if (overflow) d=1;
	return d;
}

//play animation for weapon in hand during combat on a horse //kolours - modularized (09/20/98)
void cCombat::CombatOnHorse(CHARACTER c)
{
	P_CHAR pc = MAKE_CHARREF_LR(c);
	P_ITEM pWeapon=pc->getWeapon();
	if (pWeapon)
	{
		short id = pWeapon->id();

		if (IsBow(id))
		{
			npcaction(c, 0x1B);
			return;
		}
		if (IsCrossbow(id) || IsHeavyCrossbow(id))
		{
			npcaction(c, 0x1C);
			return;
		}

		if(  IsSword(id) || IsMaceType(id) ||
			(id ==0x0FB4 || id ==0x0FB5) || // sledge hammer
			IsFencing1H(id) )
		{
			npcaction(c, 0x1A);
			return;
		}
		if ( IsAxe(id) || IsFencing2H(id) )
		{
			npcaction(c, 0x1D); //2Handed
			return;
		}
	} // end of !=-1
	else
	{
		npcaction(c, 0x1A); //fist fighting
		return;
	}
}

//play animation for weapon in hand for combat on foot //kolours - (09/20/98)
void cCombat::CombatOnFoot(int c)
{
	 P_CHAR pc = MAKE_CHARREF_LR(c);
	 P_ITEM pWeapon=pc->getWeapon();
	int m = rand()%4; //randomize the animation sequence

	if (pWeapon)
	{
		short id = pWeapon->id();

		if (IsBow(id))
		{
			npcaction(c, 0x12); //bow
			return;
		}
		if (IsCrossbow(id) || IsHeavyCrossbow(id))
		{
			npcaction(c, 0x13); //crossbow - regular
			return;
		}
		if (IsSword(id))
		{
			switch (m) //swords
			{
			case 0:		npcaction(c, 0x0D);	return; //side swing
			case 1:		npcaction(c, 0x0A);	return; //poke
			default:	npcaction(c, 0x09);	return; //top-down swing
			}
		}
		if (IsMace1H(id))
		{
			switch (m) //maces
			{
			case 0:		npcaction(c, 0x0D);	return;	//side swing
			default:	npcaction(c, 0x09);	return; //top-down swing
			}
		}
		if (IsMace2H(id) || IsAxe(id))
		{
			switch (m)
			{
			case 0:		npcaction(c, 0x0D);	return; //2H top-down
			case 1:		npcaction(c, 0x0C);	return; //2H swing
			default:	npcaction(c, 0x0D);	return; //2H top-down
			}
		}
		if (IsFencing1H(id))	// one handed fencing
		{
			switch (m) //fencing
			{
			case 0:		npcaction(c, 0x09);	return; //top-down
			case 1:		npcaction(c, 0x0D);	return; //side-swipe
			default:	npcaction(c, 0x0A);	return; //default: poke
			}
		}
		if (IsFencing2H(id))	//pitchfork & spear
		{
			switch (m) //pitchfork
			{
			case 0:		npcaction(c, 0x0D);	return; //top-down
			default:	npcaction(c, 0x0E);	return; //default: 2-handed poke
			}
		}
	} // end of !=-1
	else
	{
		switch (m) //fist fighting
		{
		case 0:		npcaction(c, 0x0A);	return; //fist straight-punch
		case 1:		npcaction(c, 0x09);	return; //fist top-down
		default:	npcaction(c, 0x1F);	return; //default: //fist over-head
		}
	}
}


//s: char#
void cCombat::SpawnGuard(CHARACTER s, CHARACTER i, int x, int y, signed char z)
{
	int t;
//	if (i < 0 || i >= cmem || s < 0 || s >= cmem)
//		return;
	
	P_CHAR pc_offender = MAKE_CHARREF_LR(s);
	P_CHAR pc_caller   = MAKE_CHARREF_LR(i);
	
	if (!pc_caller->inGuardedArea())
		return;
	
	if (pc_offender->dead || pc_caller->dead)
		return; // AntiChrist
	
	if (SrvParms->guardsactive && !pc_offender->isInvul())
	{
        t = region[pc_caller->region].guardnum[(rand()%10)];
		CHARACTER c = Npcs->AddNPCxyz(calcSocketFromChar(s), t, 0, x, y, z);
		P_CHAR pc_guard = MAKE_CHARREF_LR(c);
		
		pc_guard->npcaitype = 4; // CITY GUARD, LB, bugfix, was 0x40 -> not existing
		pc_guard->setAttackFirst();
		pc_guard->attacker = s;
		pc_guard->targ = s;
		pc_guard->npcWander = 2;  // set wander mode Tauriel
		npcToggleCombat(DEREF_P_CHAR(pc_guard));
		pc_guard->npcmovetime =(unsigned int)(getNormalizedTime() +(double)(NPCSPEED*MY_CLOCKS_PER_SEC));
		pc_guard->summontimer =(getNormalizedTime() +(MY_CLOCKS_PER_SEC*25));    
		
		soundeffect2(DEREF_P_CHAR(pc_guard), 0x01, 0xFE);  // Tauriel 1-9-99 changed to stop crashing used to call soundeffect (expeted socket)
		staticeffect(DEREF_P_CHAR(pc_guard), 0x37, 0x2A, 0x09, 0x06);
		
		updatechar(DEREF_P_CHAR(pc_guard));
		switch (RandomNum(0,1))
		{
		case 0:		npctalkall(DEREF_P_CHAR(pc_guard), "Thou shalt regret thine actions, swine!", 1);	break;
		case 1:		npctalkall(DEREF_P_CHAR(pc_guard), "Death to all Evil!", 1);							break;
		}
	}
}

void cCombat::ItemSpell(int attaker, int defender)
{
	currentSpellType[attaker]=2;
	int ci=0,loopexit=0;
	P_ITEM pi;
	while ( ((pi=ContainerSearch(chars[attaker].serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
	{
		if (((pi->layer==1 && pi->type!=9) || (pi->layer==2)))
		{
			if (pi->offspell && (pi->att||pi->hidamage) && pi->type == 15)
			{
				switch(pi->offspell)
				{
				case 1:	Magic->ClumsySpell(attaker,defender, false);		break;
				case 2:	Magic->FeebleMindSpell(attaker, defender, false);	break;
				case 3:	Magic->MagicArrow(attaker,defender, false);			break;
				case 4:	Magic->WeakenSpell(attaker,defender, false);		break;
				case 5:	Magic->HarmSpell(attaker,defender, false);			break;
				case 6:	Magic->FireballSpell(attaker,defender, false);		break;
				case 8:	Magic->CurseSpell(attaker,defender, false);			break;
				case 9:	Magic->LightningSpellItem(attaker,defender);		break;
				case 11:Magic->MindBlastSpell(attaker,defender, false);		break;
				case 12:Magic->ParalyzeSpell(attaker,defender, false);		break;
				case 14:Magic->ExplosionSpell(attaker,defender, false);		break;
				case 15:Magic->FlameStrikeSpellItem(attaker,defender);		break;
				default:
					LogErrorVar("invalid offspell value %i",pi->offspell);
				}
				pi->morez--;
				if (pi->morez == 0)
				{
					pi->type = pi->type2;
					pi->morex = 0;
					pi->morey = 0;
					pi->offspell = 0;
				}
			}
			return;
		}
    }
}

//AntiChrist - do the sound effect ( only if HITTEN! )
void cCombat::doSoundEffect(CHARACTER p, int fightskill, P_ITEM pWeapon)
{
	bool heavy=false;
	int a=rand()%4;

	//check if a heavy weapon
	if (pWeapon && IsAxe(pWeapon->id()))
		heavy=true;

	if(heavy)
	{
		if (a==0 || a==1) soundeffect2(p, 0x02, 0x36);
		else soundeffect2(p, 0x02, 0x37);
		return;
	}		

	switch(fightskill)
	{
	case ARCHERY:
		soundeffect2(p, 0x02, 0x34);
		break;
	case FENCING:
	case SWORDSMANSHIP:
		if (a==0 || a==1) soundeffect2(p, 0x02, 0x3B);
		else soundeffect2(p, 0x02, 0x3C);
		break;
	case MACEFIGHTING:
		if (a==0 || a==1) soundeffect2(p, 0x02, 0x32);
		else if (a==2) soundeffect2(p, 0x01, 0x39);
		else soundeffect2(p, 0x02, 0x33);
		break;
	case WRESTLING:
		if (a==0) soundeffect2(p, 0x01, 0x35);
		else if (a==1) soundeffect2(p, 0x01, 0x37);
		else if (a==2) soundeffect2(p, 0x01, 0x3D);
		else soundeffect2(p, 0x01, 0x3B);
		break;
	default:
		soundeffect2(p, 0x01, 0x3D);
	}
}


//AntiChrist - do the "MISSED" sound effect
void cCombat::doMissedSoundEffect(CHARACTER p)
{
	int a=rand()%3;

	if (a==0) soundeffect2(p, 0x02, 0x38);
	else if (a==1) soundeffect2(p, 0x02, 0x39);
	else soundeffect2(p, 0x02, 0x3A);
}
