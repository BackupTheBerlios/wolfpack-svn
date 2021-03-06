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

/////////////////////
// Name:	Trade.cpp
// Purpose: functions that are related to trade
// History:	cut from wolfpack.cpp by Duke, 26.10.00
// Remarks:	not necessarily ALL those functions
//

#include "Trade.h"

#undef  DBGFILE
#define DBGFILE "Trade.cpp"

void buyaction(int s)
{
	char clearmsg[8];
	int clear, i, j;
	int buyit[512];
	int amount[512];
	int layer[512];
	int playergoldtotal;
	int goldtotal;
	int itemtotal;
	int npc;
	int soldout;
	int p;
	int tmpvalue=0; // Fixed for adv trade system -- Magius(CHE) �
	CHARACTER cc=currchar[s];
	P_CHAR pc_currchar = MAKE_CHARREF_LR(cc);
	p=packitem(DEREF_P_CHAR(pc_currchar));
	if (p==-1) return; //LB no player-pack - no buy action possible - and no crash too ;-)
	npc=calcCharFromSer(buffer[s][3], buffer[s][4], buffer[s][5], buffer[s][6]);

	if (npc <= 0) return;

	clear=0;
	goldtotal=0;
	soldout=0;
	itemtotal=(((256*(buffer[s][1]))+buffer[s][2])-8)/7;
	if (itemtotal>256) return; //LB

	for(i=0;i<itemtotal;i++)
	{
		layer[i]=buffer[s][8+(7*i)];
		buyit[i]=calcItemFromSer(buffer[s][8+(7*i)+1], buffer[s][8+(7*i)+2],
			buffer[s][8+(7*i)+3], buffer[s][8+(7*i)+4]);
		amount[i]=(256*(buffer[s][8+(7*i)+5]))+buffer[s][8+(7*i)+6];

		if (buyit[i]>-1)
		{
			items[buyit[i]].rank=10;
			// Fixed for adv trade system -- Magius(CHE) �
			tmpvalue=items[buyit[i]].value;
			tmpvalue=calcValue(buyit[i], tmpvalue);
			if (SrvParms->trade_system==1) tmpvalue=calcGoodValue(s,buyit[i],tmpvalue,0);
			goldtotal=goldtotal+(amount[i]*tmpvalue);
			// End Fix for adv trade system -- Magius(CHE) �
		}
	}

	bool useBank;
	useBank = (goldtotal >= SrvParms->CheckBank );

	if( useBank )
		playergoldtotal = GetBankCount( DEREF_P_CHAR(pc_currchar), 0x0EED );
	else
		playergoldtotal = pc_currchar->CountGold();

	if ((playergoldtotal>=goldtotal)||(pc_currchar->isGM()))
	{
		for (i=0;i<itemtotal;i++)
		{
			if (buyit[i]>-1)
			{
				if (items[buyit[i]].amount<amount[i])
				{
					soldout=1;
				}
			}
		}
		if (soldout)
		{
			npctalk(s, npc, "Alas, I no longer have all those goods in stock. Let me know if there is something else thou wouldst buy.",0);
			clear=1;
		}
		else
		{
			if (pc_currchar->isGM())
			{
				sprintf((char*)temp, "Here you are, %s. Someone as special as thee will receive my wares for free of course.", pc_currchar->name);
			}
			else
			{
				if(useBank)
				{
					sprintf((char*)temp, "Here you are, %s. %d gold coin%s will be deducted from your bank account.  I thank thee for thy business.",
					pc_currchar->name, goldtotal, (goldtotal==1) ? "" : "s");
				    goldsfx(s, goldtotal);
				}
			    else
				{
				    sprintf((char*)temp, "Here you are, %s.  That will be %d gold coin%s.  I thank thee for thy business.",
					pc_currchar->name, goldtotal, (goldtotal==1) ? "" : "s");
				    goldsfx(s, goldtotal);	// Dupois, SFX for gold movement. Added Oct 08, 1998
				}
			}
			npctalkall(npc, (char*)temp,0);
			npcaction(npc,0x20);		// bow (Duke, 17.3.2001)

			clear=1;
			if( !(pc_currchar->isGM() ) ) 
			{
				if( useBank )
					DeleBankItem( DEREF_P_CHAR(pc_currchar), 0x0EED, 0, goldtotal );
				else
					delequan( DEREF_P_CHAR(pc_currchar), 0x0EED, goldtotal, NULL );
			}
			for (i=0;i<itemtotal;i++)
			{
				P_ITEM pi = MAKE_ITEM_REF(buyit[i]);
				if (pi != NULL)
				{
					if (pi->amount>amount[i])
					{
						if (pi->pileable)
						{
							Commands->DupeItem(s, buyit[i], amount[i]);
						}
						else
						{
							for (j=0;j<amount[i];j++)
							{
								Commands->DupeItem(s, buyit[i], 1);
							}
						}
						pi->amount-=amount[i];
						pi->restock+=amount[i];
					}
					else
					{
						switch(layer[i])
						{
						case 0x1A:
							if (pi->pileable)
							{
								Commands->DupeItem(s, buyit[i], amount[i]);
							}
							else
							{
								for (j=0;j<amount[i];j++)
								{
									Commands->DupeItem(s, buyit[i], 1);
								}
							}
							pi->amount=pi->amount-amount[i];
							pi->restock=pi->restock+amount[i];
							break;
						case 0x1B:
							if (pi->pileable)
							{
								pi->SetContSerial(items[p].serial);
								RefreshItem(buyit[i]);//AntiChrist
							}
							else
							{
								for (j=0;j<amount[i]-1;j++)
								{
									Commands->DupeItem(s, buyit[i], 1);
								}
								pi->SetContSerial(items[p].serial);
								pi->amount=1;
								RefreshItem(buyit[i]);//AntiChrist
							}
							break;
						default:
							clConsole.send("ERROR: Fallout of switch statement without default. wolfpack.cpp, buyaction()\n"); //Morrolan
						}
					}
				}
			}
		}
	}
	else
	{
		npctalkall(npc, "Alas, thou dost not possess sufficient gold for this purchase!",0);
	}

	if (clear)
	{
		clearmsg[0]=0x3B;
		clearmsg[1]=0x00;
		clearmsg[2]=0x08;
		clearmsg[3]=buffer[s][3];
		clearmsg[4]=buffer[s][4];
		clearmsg[5]=buffer[s][5];
		clearmsg[6]=buffer[s][6];
		clearmsg[7]=0x00;
		Xsend(s, clearmsg, 8);
	}
	Weight->NewCalc(DEREF_P_CHAR(pc_currchar));	// Ison 2-20-99
	statwindow(s, DEREF_P_CHAR(pc_currchar));
}

void restock(int s)
{
	int a,serial;
	AllItemsIterator iter_items;
	for (iter_items.Begin(); iter_items.GetData() != iter_items.End(); iter_items++)
	{
		P_ITEM pi = iter_items.GetData();
		// Dupois - added this check to only restock items that ... well ... have a restock value >0
		// Added Oct 25, 1998
		if (pi->restock && isItemSerial(pi->contserial))
		{
			serial = pi->contserial;
			if(serial > 0)
			{
				P_ITEM ci = FindItemBySerial(serial);
				if (ci != NULL)
				{
					if ((ci->layer==0x1A))
					{
						if (s)
						{
							pi->amount += pi->restock;
							pi->restock = 0;
						}
						else
						{
							if (pi->restock > 0)
							{
								a = min(pi->restock, (pi->restock/2)+1);
								pi->amount+=a;
								pi->restock-=a;
							}
						}
					}
				}
			}
		}
		// MAgius(CHE): All items in shopkeeper need a new randomvaluerate.
		if (SrvParms->trade_system==1) StoreItemRandomValue(DEREF_P_ITEM(pi),-1);// Magius(CHE) (2)
	}
}

// this is a q&d fix for 'sell price higher than buy price' bug (Duke, 30.3.2001)
static bool items_match(P_ITEM pi1, P_ITEM pi2)
{
	if (pi1->id()==pi2->id() &&
		pi1->type==pi2->type &&
		!(pi1->id()==0x14F0 && (pi1->morex!=pi2->morex)) &&			// house deeds only differ by morex
		!(IsShield(pi1->id()) && strcmp(pi1->name2,pi2->name2)) &&	// magic shields only differ by name2
		!(IsMetalArmour(pi1->id()) && pi1->color()!=pi2->color()) )	// color checking for armour
		return true;
	return false;
}
 
void sellaction(int s)
{
	int n, npa=0, npb=0, npc=0, i, amt, value=0, totgold=0;

	if (buffer[s][8]!=0)
	{
		n=calcCharFromSer(buffer[s][3], buffer[s][4], buffer[s][5], buffer[s][6]);
		if (n<0 || n>=cmem) return;

		int ci=0,loopexit=0;
		P_ITEM pi;
		while ( ((pi=ContainerSearch(chars[n].serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
		{
			if (pi->layer==0x1A) npa=DEREF_P_ITEM(pi);		// Buy Restock container
			else if (pi->layer==0x1B) npb=DEREF_P_ITEM(pi);	// Buy no restock container
			else if (pi->layer==0x1C) npc=DEREF_P_ITEM(pi);	// Sell container
		}

		// Pre Calculate Total Amount of selling items to STOPS if the items if greater than SELLMAXITEM - Magius(CHE)
		int maxsell=0;
		i=buffer[s][8];
		if (i>256) return;
		for (i=0;i<buffer[s][8];i++)
		{
			amt=ShortFromCharPtr(buffer[s]+9+(6*i)+4);
			maxsell+=amt;
		}
		if (maxsell>SrvParms->sellmaxitem)
		{
			char tmpmsg[256];
			sprintf(tmpmsg,"Sorry %s but i can buy only %i items at time!",chars[currchar[s]].name,SrvParms->sellmaxitem);
			npctalkall(n, tmpmsg,0);
			return;
		}

		for (i=0;i<buffer[s][8];i++)
		{
			P_ITEM pSell=FindItemBySerPtr(buffer[s]+9+(6*i));	// the item to sell
			if (!pSell) continue;
			int sell=DEREF_P_ITEM(pSell);
			amt=ShortFromCharPtr(buffer[s]+9+(6*i)+4);
			
			// player may have taken items out of his bp while the sell menu was up ;-)
			if (pSell->amount<amt)
			{
				npctalkall(n, "Cheating scum! Leave now, before I call the guards!",0);
				return;
			}

			// Search the buy restock Container
			int join=-1;
			ci=0,loopexit=0;
			P_ITEM pi;
			while ( ((pi=ContainerSearch(items[npa].serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
			{
				if (items_match(pi,pSell))
					join=DEREF_P_ITEM(pi);
			}

			// Search the sell Container to determine the price
			ci=0,loopexit=0;
			while ( ((pi=ContainerSearch(items[npc].serial,&ci)) != NULL) && (++loopexit < MAXLOOPS) )
			{
				if (items_match(pi,pSell))
				{
					value=pi->value;
					value=calcValue(sell, value);
					if (SrvParms->trade_system==1)
						value=calcGoodValue(s,sell,value,1); // Fixed for adv trade --- by Magius(CHE) �
					break;	// let's take the first match
				}
			}
			totgold+=(amt*value);	// add to the bill

			if (join!=-1)	// The item goes to the container with restockable items
			{
				items[join].amount+=amt;
				items[join].restock-=amt;
				pSell->ReduceAmount(amt);
			}
			else
			{
				pSell->SetContSerial(items[npb].serial);
				SndRemoveitem(pSell->serial);
				if (pSell->amount!=amt)
					Commands->DupeItem(s, sell, pSell->amount-amt);
			}
		}
		addgold(s, totgold);
		goldsfx(s, totgold);	// Dupois, SFX for gold movement	// Added Oct 08, 1998
	}

	char clearmsg[9];
	clearmsg[0]=0x3B;
	clearmsg[1]=0x00;
	clearmsg[2]=0x08;
	clearmsg[3]=buffer[s][3];
	clearmsg[4]=buffer[s][4];
	clearmsg[5]=buffer[s][5];
	clearmsg[6]=buffer[s][6];
	clearmsg[7]=0x00;
	Xsend(s, clearmsg, 8);
}

int tradestart(int s, int i)
{
	int ps, pi, bps, bpi, s2,c;
	unsigned char msg[90];

	bps=packitem(currchar[s]);
	bpi=packitem(i);
	s2=calcSocketFromChar(i);

	if (bps==-1) //LB
	{
		sysmessage(s,"Time to buy a backpack!");
		sprintf((char*)temp,"%s doesnt have a backpack!",chars[currchar[s]].name);
		sysmessage(s2,(char*)temp);
		return 0;
	}

	if (bpi==-1)
	{
		sysmessage(s2,"Time to buy a backpack!");
		sprintf((char*)temp,"%s doesnt have a backpack!",chars[currchar[s2]].name);
		sysmessage(s,(char*)temp);
		return 0;
	}

	c=Items->SpawnItem(s2,currchar[s],1,"#",0,0x1E,0x5E,0,0,0,0);
	if(c==-1) return 0;//AntiChrist to preview crashes
	items[c].pos.x=26;
	items[c].pos.y=0;
	items[c].pos.z=0;
	items[c].SetContSerial(chars[currchar[s]].serial);
	items[c].layer=0;
	items[c].type=1;
	items[c].dye=0;
	ps=c;
	sendbpitem(s, ps);
	if (s2!=-1) sendbpitem(s2, ps);

	c=Items->SpawnItem(s2,i,1,"#",0,0x1E,0x5E,0,0,0,0);
	if(c==-1) return 0;//AntiChrist to preview crashes
	items[c].pos.x=26;
	items[c].pos.y=0;
	items[c].pos.z=0;
	items[c].SetContSerial(chars[i].serial);
	items[c].layer=0;
	items[c].type=1;
	items[c].dye=0;
	pi=c;
	sendbpitem(s, pi);
	if (s2!=-1) sendbpitem(s2, pi);

	items[pi].moreb1=items[ps].ser1;
	items[pi].moreb2=items[ps].ser2;
	items[pi].moreb3=items[ps].ser3;
	items[pi].moreb4=items[ps].ser4;
	items[ps].moreb1=items[pi].ser1;
	items[ps].moreb2=items[pi].ser2;
	items[ps].moreb3=items[pi].ser3;
	items[ps].moreb4=items[pi].ser4;
	items[pi].morez=0;
	items[ps].morez=0;

	msg[0]=0x6F; // Header Byte
	msg[1]=0; // Size
	msg[2]=47; // Size
	msg[3]=0; // Initiate
	LongToCharPtr(chars[i].serial,msg+4);
	LongToCharPtr(items[ps].serial,msg+8);
	LongToCharPtr(items[pi].serial,msg+12);
	msg[16]=1;
	strcpy((char*)&(msg[17]),chars[i].name);
	Xsend(s, msg, 47);

	if (s2 >= 0)
	{
		msg[0]=0x6F; // Header Byte
		msg[1]=0; // Size
		msg[2]=47; // Size
		msg[3]=0; // Initiate
		LongToCharPtr(chars[currchar[s]].serial,msg+4);
		LongToCharPtr(items[pi].serial,msg+8);
		LongToCharPtr(items[ps].serial,msg+12);
		msg[16]=1;
		strcpy((char*)&(msg[17]),chars[currchar[s]].name);
		Xsend(s2, msg, 47);
	}
	return ps;
}

void clearalltrades()
{
	int i, j, k, p,serial,serhash,ci;
	for (i=0;i<itemcount;i++)
	{
		P_ITEM pi = &items[i];
		if (pi->type==1 && pi->pos.x==26 && pi->pos.y==0 && pi->pos.z==0 &&
			pi->id()==0x1E5E)
		{
			k=calcCharFromSer(pi->contserial);
			p=packitem(k);
			serial=pi->serial;
			serhash=serial%HASHMAX;
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				j=contsp[serhash].pointer[ci];
				if (j!=-1)
					if ((items[j].contserial==serial))
					{
						if(p>-1)
						{
							items[j].SetContSerial(items[p].serial);
						}
					}
			}
			Items->DeleItem(i);
			clConsole.send("Trade cleared\n");
		}
	}
}

void trademsg(int s)
{
	int cont1, cont2;
	switch(buffer[s][3])
	{
	case 0://Start trade - Never happens, sent out by the server only.
		break;
	case 2://Change check marks. Possibly conclude trade
		cont1=calcItemFromSer(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]);
		if (cont1>-1) cont2=calcItemFromSer(items[cont1].moreb1, items[cont1].moreb2, items[cont1].moreb3, items[cont1].moreb4); else cont2=-1;
		if (cont2>-1) // lb crashfix
		{
			items[cont1].morez=buffer[s][11];
			sendtradestatus(cont1, cont2);
			if (items[cont1].morez && items[cont2].morez)
			{
				dotrade(cont1, cont2);
				endtrade(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]);
			}
		}
		break;
	case 1://Cancel trade. Send each person cancel messages, move items.
		endtrade(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]);
		break;
	default:
		clConsole.send("ERROR: Fallout of switch statement without default. wolfpack.cpp, trademsg()\n"); //Morrolan
	}
}

void dotrade(int cont1, int cont2)
{
	int p1, p2, bp1, bp2, s1, s2, i;
	int serial,serhash,ci;

	p1=calcCharFromSer(items[cont1].contserial);
	if(p1<0) return;
	p2=calcCharFromSer(items[cont2].contserial);
	if(p2<0) return;
	if(items[cont1].morez==0 || items[cont2].morez==0)
	{//not checked - give items to previous owners - AntiChrist
		int t;
		t=p1;
		p1=p2;
		p2=t;
	}
	bp1=packitem(p1);
	if(bp1<0) return;
	bp2=packitem(p2);
	if(bp2<0) return;
	s1=calcSocketFromChar(p1);
	s2=calcSocketFromChar(p2);

	serial=items[cont1].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial))
			{
				if (items[i].glow>0) removefromptr(&glowsp[chars[currchar[s2]].serial%HASHMAX],i); // lb, glowing stuff
				items[i].SetContSerial(items[bp2].serial);
				if (items[i].glow>0) setptr(&glowsp[chars[currchar[s1]].serial%HASHMAX],i);
				items[i].pos.x=50+(rand()%80);
				items[i].pos.y=50+(rand()%80);
				items[i].pos.z=9;
				if (s1!=-1)
					RefreshItem(i);//AntiChrist
				if (s2!=-1) sendbpitem(s2, i);
					RefreshItem(i);//AntiChrist
			}
	}
	serial=items[cont2].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial))
			{
				if (items[i].glow>0) removefromptr(&glowsp[chars[currchar[s2]].serial%HASHMAX],i); // lb, glowing stuff
				items[i].SetContSerial(items[bp1].serial);
				if (items[i].glow>0) setptr(&glowsp[chars[currchar[s1]].serial%HASHMAX],i);
				items[i].pos.x=50+(rand()%80);
				items[i].pos.y=50+(rand()%80);
				items[i].pos.z=9;
				if (s2!=-1)
					RefreshItem(i);//AntiChrist
				if (s1!=-1) sendbpitem(s1, i);
					RefreshItem(i);//AntiChrist
			}
	}
}

