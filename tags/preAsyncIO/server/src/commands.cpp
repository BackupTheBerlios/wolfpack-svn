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

#include "wolfpack.h"
#include "debug.h"
#include "cmdtable.h"
#include "spawnregions.h"
#include "SndPkg.h"
#include "classes.h"
#include "utilsys.h"
#include "network.h"
#include "mapstuff.h"
#include "wpdefmanager.h"
#include "scriptc.h"

#undef  DBGFILE
#define DBGFILE "commands.cpp"

// inline command to do targeting - coulda made this a
// macro but this is pretty much as fast...
static inline void _do_target(int s, TARGET_S *ts) {
	target(s, ts->a1, ts->a2, ts->a3, ts->a4, ts->txt);
	return;
}

/* extensively modified 8/2/99 crackerjack@crackerjack.net -
 * see cmdtable.cpp for more details */
void cCommands::Command(UOXSOCKET s, string speech) // Client entred a '/' command like /ADD
{
	int i=9;
	unsigned char *comm;
	unsigned char nonuni[512];
	int y,loopexit=0;

	P_CHAR pc_currchar = currchar[s];

	this->command_line = speech.c_str();
	this->params = QStringList::split( " ", this->command_line );
	
	strcpy((char*)nonuni, speech.c_str());
	strcpy((char*)tbuffer, (char*)nonuni);

	strupr((char*)nonuni);
	cline = (char*) &nonuni[0];
	splitline();
	if (tnum<1)
		return;
	// Let's ignore the command prefix;
	comm = &nonuni[1];

	i=0; y=-1;loopexit=0;
	while((command_table[i].cmd_name)&&(y==-1) && (++loopexit < MAXLOOPS)) {
		if(!(strcmp((char*)command_table[i].cmd_name, (char*)comm))) y=i;
		i++;
	}

	if(y==-1) {
		sysmessage(s, tr("Unrecognized command."));
		return;
	} else {
		if( ( pc_currchar->isTrueGM() && !pc_currchar->isGM() ) ||		// a restricted GM outside his region(s)
			( pc_currchar->account() != 0 && !Commands->containsCmd(pc_currchar->privlvl(), command_table[y].cmd_name) ) )
		{
			sysmessage(s, tr("Access denied."));
			return;
		}

		switch(command_table[y].cmd_type) {
		case CMD_FUNC:
			(*((CMD_EXEC)command_table[y].cmd_extra)) (s);
			break;
		case CMD_ITEMMENU:
			ShowMenu(s, (int)command_table[y].cmd_extra);
			break;
		case CMD_TARGET:
			SocketStrings[ s ] = speech;
			_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			break;
		case CMD_TARGETX:
			if(tnum==2) {
				addx[s]=makenumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes one number as an argument."));
			}
			break;
		case CMD_TARGETXY:
			if(tnum==3) {
				addx[s]=makenumber(1);
				addy[s]=makenumber(2);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes two numbers as arguments."));
			}
			break;
		case CMD_TARGETXYZ:
			if(tnum==4) {
				addx[s]=makenumber(1);
				addy[s]=makenumber(2);
				addz[s]=makenumber(3);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes three numbers as arguments."));
			}
			break;
		case CMD_TARGETHX:
			if(tnum==2) {
				addx[s]=hexnumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes one hex number as an argument."));
			}
			break;
		case CMD_TARGETHXY:
			if(tnum==3) {
				addx[s]=hexnumber(1);
				addy[s]=hexnumber(2);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes two hex numbers as arguments."));
			}
			break;
		case CMD_TARGETHXYZ:
			if(tnum==4) {
				addx[s]=hexnumber(1);
				addy[s]=hexnumber(2);
				addz[s]=hexnumber(3);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes three hex numbers as arguments."));
			}
			break;
		case CMD_TARGETID1:
			if(tnum==2) {
				addid1[s]=makenumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes one number as an argument."));
			}
			break;
		case CMD_TARGETID2:
			if(tnum==3) {
				addid1[s] = static_cast<unsigned char>(makenumber(1));
				addid2[s] = static_cast<unsigned char>(makenumber(2));
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes two numbers as arguments."));
			}
			break;
		case CMD_TARGETID3:
			if(tnum==4) {
				addid1[s] = static_cast<unsigned char>(makenumber(1));
				addid2[s] = static_cast<unsigned char>(makenumber(2));
				addid3[s] = static_cast<unsigned char>(makenumber(3));
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes three numbers as arguments."));
			}
			break;
		case CMD_TARGETID4:
			if(tnum==5) {
				addid1[s] = static_cast<unsigned char>(makenumber(1));
				addid2[s] = static_cast<unsigned char>(makenumber(2));
				addid3[s] = static_cast<unsigned char>(makenumber(3));
				addid4[s] = static_cast<unsigned char>(makenumber(4));
				//clConsole.send("1: %i 2: %i 3: %i 4: %i\n",addid1[s],addid2[s],addid2[s],addid3[s],addid4[s]);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes four numbers as arguments."));
			}
			break;
		case CMD_TARGETHID1:
			if(tnum==2) {
				addid1[s] = static_cast<unsigned char>(hexnumber(1));
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes one hex number as an argument."));
			}
			break;
		case CMD_TARGETHID2:
			if(tnum==3) {
				addid1[s] = static_cast<unsigned char>(hexnumber(1));
				addid2[s] = static_cast<unsigned char>(hexnumber(2));
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes two hex numbers as arguments."));
			}
			break;
		case CMD_TARGETHID3:
			if(tnum==4) {
				addid1[s] = static_cast<unsigned char>(hexnumber(1));
				addid2[s] = static_cast<unsigned char>(hexnumber(2));
				addid3[s] = static_cast<unsigned char>(hexnumber(3));
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes three hex numbers as arguments."));
			}
			break;
		case CMD_TARGETHID4:
			if(tnum==5) {
				addid1[s] = static_cast<unsigned char>(hexnumber(1));
				addid2[s] = static_cast<unsigned char>(hexnumber(2));
				addid3[s] = static_cast<unsigned char>(hexnumber(3));
				addid4[s] = static_cast<unsigned char>(hexnumber(4));
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes four hex numbers as arguments."));
			}
			break;
		case CMD_TARGETTMP:
			if(tnum==2) {
				tempint[s]=makenumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes a number as an argument."));
			}
			break;
		case CMD_TARGETHTMP:
			if(tnum==2) {
				tempint[s]=hexnumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} else {
				sysmessage(s, tr("This command takes a hex number as an argument."));
			}
			break;
		default:
			sysmessage(s, tr("BUG: Command has a bad command type set!"));
			break;
		}
		return;
	}

	sysmessage(s, "BUG: Should never reach end of command() function!");
}

QString cCommands::GetAllParams(void)
{
	if( !command_line.contains( " " ) )
		return "";

	return command_line.right( command_line.length() - command_line.find( " " ) ).stripWhiteSpace();
}

void cCommands::MakeShop(P_CHAR pc_c)
{
	if ( pc_c == NULL ) return;
	pc_c->shop=true;
	if (pc_c->GetItemOnLayer(0x1A) == NULL)
	{
		P_ITEM p1A = Items->SpawnItem(pc_c,1,"#",0,0x2AF8,0,0);
		if(p1A)
		{
			p1A->setContSerial(pc_c->serial);
			p1A->setLayer( 0x1A );
			p1A->setType( 1 );
			p1A->priv |= 0x02;
		}
	}
	
	if (pc_c->GetItemOnLayer(0x1B) == NULL)
	{
		P_ITEM p1B = Items->SpawnItem(pc_c,1,"#",0,0x2AF8,0,0);
		if(p1B)
		{
			p1B->setContSerial(pc_c->serial);
			p1B->setLayer( 0x1B );
			p1B->setType( 1 );
			p1B->priv |= 0x02;
		}
	}
	
	if (pc_c->GetItemOnLayer(0x1C) == NULL)
	{
		P_ITEM p1C=Items->SpawnItem(pc_c,1,"#",0,0x2AF8,0,0);
		if(p1C)
		{
			p1C->setContSerial(pc_c->serial);
			p1C->setLayer( 0x1C );
			p1C->setType( 1 );
			p1C->priv |= 0x02;
		}
	}
}

void cCommands::NextCall(int s, int type)
{
	// Type is the same as it is in showgmqueue()
	
	int i, serial;
	int x = 0;
	
	P_CHAR pc_currchar = currchar[s];
	
	if (pc_currchar->callnum() != 0)
	{
		donewithcall(s, type);
	}
	if (type == 1) // Player is a GM
	{
		for (i = 1; i < MAXPAGES; i++)
		{
			if (gmpages[i].handled == 0)
			{
				serial = gmpages[i].serial;
				P_CHAR pc_player = FindCharBySerial(serial);
				if (pc_player != NULL)
				{
					sysmessage(s, "");
					sysmessage(s, tr("Transporting to next call: %1").arg(gmpages[i].name.c_str()));
					sysmessage(s, tr("Problem: %1.").arg(gmpages[i].reason.c_str()));
					sysmessage(s, tr("Serial number %1").arg(gmpages[i].serial, 16));
					sysmessage(s, tr("Paged at %1.").arg((char*)gmpages[i].timeofcall));
					gmpages[i].handled = 1;
					pc_currchar->moveTo(pc_player->pos);
					pc_currchar->setCallNum( i );
					teleport((pc_currchar));
					x++;
				}// if
				if (x > 0)
					break;
			}// if
		}// for
		if (x == 0)
			sysmessage(s, tr("The GM queue is currently empty"));
	} // end first IF
	else // Player is only a counselor
	{
		x = 0;
		for (i = 1; i < MAXPAGES; i++)
		{
			if (counspages[i].handled == 0)
			{
				serial = counspages[i].serial;
				P_CHAR pc_player = FindCharBySerial(serial);
				if (pc_player != NULL)
				{
					sysmessage(s, "");
					sysmessage(s, tr("Transporting to next call: %1").arg(counspages[i].name.c_str()));
					sysmessage(s, tr("Problem: %1.").arg(counspages[i].reason.c_str()));
					sysmessage(s, tr("Serial number %1").arg(counspages[i].serial, 16));
					sysmessage(s, tr("Paged at %1.").arg(counspages[i].timeofcall));
					gmpages[i].handled = 1;
					pc_currchar->moveTo(pc_player->pos);
					pc_currchar->setCallNum( i );
					teleport((pc_currchar));
					x++;
					break;
				}// if
			}// else
			if (x > 0)
				break;
		}// for
	    if (x == 0)
			sysmessage(s, tr("The Counselor queue is currently empty"));
	}// if
}

void cCommands::KillSpawn(int s, QString spawnRegion )  // rewrite sereg
{
	cSpawnRegion* Region = cAllSpawnRegions::getInstance()->region( spawnRegion );

	if( Region == NULL )
		return;
	else
	{
		sysmessage(s, tr("Killing spawn, this may cause lag..."));
		Region->deSpawn();
	}

	gcollect();
	sysmessage(s, tr("Done."));
}

void cCommands::RegSpawnMax (int s, QString spawnRegion) // rewrite sereg
{
	cSpawnRegion* Region = cAllSpawnRegions::getInstance()->region( spawnRegion );

	if( Region == NULL )
		return;
	else
	{
		sysmessage(s, tr("Respawning to max, this may cause lag..."));
		Region->reSpawnToMax();
	}
}

void cCommands::RegSpawnNum (int s, QString spawnRegion, int n) // rewrite by sereg
{
	cSpawnRegion* Region = cAllSpawnRegions::getInstance()->region( spawnRegion );

	if( Region == NULL )
		return;
	else
	{
		sysmessage(s, tr("Respawning to max, this may cause lag..."));
		int i = 0;

		while( i < n )
		{
			Region->reSpawn();
			i++;
		}
	}
}//regspawnnum

void cCommands::KillAll(int s, int percent, const char* sysmsg)
{
	sysmessage(s, tr("Killing all characters, this may cause some lag..."));
	sysbroadcast(sysmsg);
	AllCharsIterator iter_char;
	for (iter_char.Begin(); !iter_char.atEnd(); iter_char++)
	{
		P_CHAR pc = iter_char.GetData();
		if(!pc->isGM())
		{
			if(rand()%100+1<=percent)
			{
				bolteffect(pc, true);
				soundeffect2(pc, 0x0029);
				deathstuff(pc);
			}
		}
	}
	sysmessage(s, tr("Done."));
}

//o---------------------------------------------------------------------------o
//|   Function -  void cpage(int s,char *reason)
//|   Date     -  UnKnown
//|   Programmer  -  UnKnown 
//o---------------------------------------------------------------------------o
//|   Purpose     -  
//o---------------------------------------------------------------------------o
void cCommands::CPage(int s, char *reason) // Help button (Calls Counselor Call Menus up)
{
	int i, x;
	int x2=0;

	P_CHAR pc_currchar = currchar[s];
	
	x=0;
	
	for(i=1;i<MAXPAGES;i++)
	{
		if(counspages[i].handled==1)
		{
			counspages[i].handled=0;
			counspages[i].name = pc_currchar->name;
			counspages[i].reason = reason;
			counspages[i].serial = pc_currchar->serial;
			time_t current_time = time(0);
			struct tm *local = localtime(&current_time);
			sprintf(counspages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
			sprintf((char*)temp,"%s [%8x] called at %s, %s",counspages[i].name.c_str(),pc_currchar->serial,counspages[i].timeofcall,counspages[i].reason.c_str());
			pc_currchar->setPlayerCallNum( i );
			pc_currchar->inputmode = cChar::enPageCouns;
			x2++;
			break;
		}
	}
	if(x2==0)
	{
		sysmessage(s,tr("The Counselor Queue is currently full. Contact the shard operator"));
		sysmessage(s,tr("and ask them to increase the size of the queue."));
	}
	else
	{
		if(strcmp(reason,"OTHER"))
		{
			pc_currchar->inputmode = cChar::enNone;
			QString temp = tr("Counselor Page from %1 [%2]: %3").arg(pc_currchar->name.c_str()).arg(pc_currchar->serial, 16).arg(reason);
			for (i=0;i<now;i++)
				if (currchar[i]->isCounselor() && perm[i])
				{
					x = 1;
					sysmessage(i, temp);
				}
			if (x==1)
			{
				sysmessage(s, tr("Available Counselors have been notified of your request."));
			}
			else sysmessage(s, tr("There was no Counselor available to take your call."));
		}
		else sysmessage(s, tr("Please enter the reason for your Counselor request"));
	}
}

//o---------------------------------------------------------------------------o
//|   Function :  void gmpage(int s,char *reason)
//|   Date     :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Help button (Calls GM Call Menus up)
//o---------------------------------------------------------------------------o
void cCommands::GMPage(int s, char *reason)
{
	int i, x = 0;
	int x2=0;
	
	P_CHAR pc_currchar = currchar[s];

	for(i=1;i<MAXPAGES;i++)
	{
		if(gmpages[i].handled==1)
		{
			gmpages[i].handled=0;
			gmpages[i].name = pc_currchar->name;
			gmpages[i].reason = reason;
			gmpages[i].serial = pc_currchar->serial;
			time_t current_time = time(0);
			struct tm *local = localtime(&current_time);
			sprintf(gmpages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
			sprintf((char*)temp,"%s [%8x] called at %s, %s",gmpages[i].name.c_str(),pc_currchar->serial,gmpages[i].timeofcall,gmpages[i].reason.c_str());
			pc_currchar->setPlayerCallNum( i );
			pc_currchar->inputmode = cChar::enPageCouns;
			x2++;
			break;
		}
	}
	if (x2==0)
	{
		sysmessage(s, tr("The GM Queue is currently full. Contact the shard operator"));
		sysmessage(s, tr("and ask them to increase the size of the queue."));
	}
	else
	{
		if(strcmp(reason,"OTHER"))
		{
			pc_currchar->inputmode = cChar::enNone;
			QString temp = tr("Page from %1 [%2]: %3").arg(pc_currchar->name.c_str()).arg(pc_currchar->serial, 16).arg(reason);
			for (i=0;i<now; ++i) 
			{
				if (currchar[i]->isGM() && perm[i])
				{
					x=1;
					sysmessage(i, temp);
				}
			}
			if (x==1)
			{
				sysmessage(s, tr("Available Game Masters have been notified of your request."));
			}
			else sysmessage(s, tr("There was no Game Master available to take your call."));
		}
		else sysmessage(s, tr("Please enter the reason for your GM request"));
	}
}

void cCommands::DyeItem(int s) // Rehue an item
{
	int body,c1,c2,b,k;
	int serial=calcserial(buffer[s][1],buffer[s][2],buffer[s][3],buffer[s][4]);
	P_ITEM pi = FindItemBySerial(serial);
	if (pi != NULL)
	{
			c1=buffer[s][7];
			c2=buffer[s][8];
			
               
			   if(!(dyeall[s]))
               {
				 if ((((c1<<8)+c2)<0x0002) ||
				    	(((c1<<8)+c2)>0x03E9))
				 {
					c1=0x03;
					c2=0xE9;
				 }
			   }
		
           	b=((((c1<<8)+c2)&0x4000)>>14)+((((c1<<8)+c2)&0x8000)>>15);	       
			if (!b)
            {
              pi->setColor( static_cast<unsigned short>(c1<<8) + c2 );
			}

			if (((c1<<8)+c2)==17969)
			{
				pi->setColor( static_cast<unsigned short>(c1<<8) + c2 );
			}
			RefreshItem(pi);//AntiChrist
			
			soundeffect( s, 0x02, 0x3e ); // plays the dye sound, LB
			return;
	}

	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	P_CHAR pc = FindCharBySerial(serial);
	if (pc != NULL)
	{
		P_CHAR pc_currchar = currchar[s];
		if( !(pc_currchar->isGM() ) ) return; // Only gms dye characters
		k=(buffer[s][7]<<8)+buffer[s][8];


		 body=(pc->id1<<8)+pc->id2;
         b=k&0x4000; 

		 if( ( ( k>>8 ) < 0x80 ) && body >= 0x0190 && body <= 0x0193 ) k+= 0x8000;

		 if (b==16384 && (body >=0x0190 && body<=0x03e1)) k=0xf000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

         if (k!=0x8000) 
		 {	
		 
			pc->setSkin(k);
			pc->setXSkin(k);
			updatechar(pc);
         }
	}
	soundeffect( s, 0x02, 0x3e ); // plays the dye sound, LB
}

void cCommands::SetItemTrigger(int s)
{
	int serial;
	 
	
  serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  P_ITEM pi = FindItemBySerial(serial);
  if (pi != NULL)
  {
		sysmessage(s, tr("Item triggered"));
		pi->trigger=addx[s];
  }
}

void cCommands::SetTriggerType(int s)
{
	int serial;
	 
	
  serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  P_ITEM pi = FindItemBySerial(serial);
  if (pi != NULL)
  {
		sysmessage(s, tr("Trigger type set"));
		pi->trigtype=addx[s];
  }
}

void cCommands::SetTriggerWord(int s)
{
	int serial;
	 
	
  serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  P_CHAR pc = FindCharBySerial(serial);
  if (pc != NULL)
  {
		sysmessage(s, tr("Trigger word set"));
		pc->setTrigword( xtext[s] );
  }
}

void cCommands::AddHere(int s, char z)
{
	bool pileable = false;
	short id = (addid1[s]<<8)+addid2[s];
	tile_st tile;
	
	Map->SeekTile(id, &tile);
	if (tile.flag2&0x08) pileable=true;
	
	P_ITEM pi = Items->SpawnItem(currchar[s], 1, "#", pileable, id, 0, 0);
	if(pi)
	{
		P_CHAR pc_currchar = currchar[s];
		Coord_cl pos(pc_currchar->pos);
		pos.z = z;
		pi->moveTo(pos);
		pi->doordir=0;
		pi->priv=0;		

		RefreshItem(pi);//AntiChrist
	}
	addid1[s]=0;
	addid2[s]=0;
}

void cCommands::SetNPCTrigger(int s)
{
	SERIAL serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	P_CHAR pc = FindCharBySerial( serial );
	if (pc != NULL)
	{
		//   if (chars[i].npc)
		//   {
		sysmessage(s, tr("NPC triggered") );
		pc->setTrigger( addx[s] );
		//   }else{
		//    sysmessage(s,"You can not trigger Player Characters");
		//   }
	}
}

void cCommands::WhoCommand(int s, int type,int buttonnum)
{
	char sect[512];
	short int length, length2, textlines;
	int k;
	unsigned int line, i;
	char menuarray[7*(MAXCLIENT)+50][50];  /** lord binary **/
	char menuarray1[7*(MAXCLIENT)+50][50]; /** the ( IS important !!! **/
	unsigned int linecount=0;
	unsigned int linecount1=0;
	int serial;
	
	
	k=buttonnum;
	
	serial = whomenudata[buttonnum];
	P_CHAR pc_c = FindCharBySerial( serial ); // find selected char ...
	if (pc_c == NULL) 
	{
		sysmessage(s, tr("selected character not found"));
		return;
	}
//	P_CHAR pc_c = MAKE_CHARREF_LR(c);
	
	//--static pages
	strcpy(menuarray[linecount++], "nomove");
	strcpy(menuarray[linecount++], "noclose");
	strcpy(menuarray[linecount++], "page 0");
	strcpy(menuarray[linecount++], "resizepic 0 0 5120 260 280");    //The background
	strcpy(menuarray[linecount++], "button 20 240 2130 2129 1 0 1"); //OKAY
	strcpy(menuarray[linecount++], "text 20 10 300 0");           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	strcpy(menuarray[linecount++], "text 20 30 300 1");
	
	//--Command Button Page 
	strcpy(menuarray[linecount++], "page 1");
	strcpy(menuarray[linecount++], "text 20 60 300 2");	//goto text
	strcpy(menuarray[linecount++], "button 150 60 1209 1210 1 0 200"); //goto button
	strcpy(menuarray[linecount++], "text 20 80 300 3");	//gettext
	strcpy(menuarray[linecount++], "button 150 80 1209 1210 1 0 201"); //get button
	strcpy(menuarray[linecount++], "text 20 100 300 4");	//Jail text
	strcpy(menuarray[linecount++], "button 150 100 1209 1210 1 0 202"); //Jail button
	strcpy(menuarray[linecount++], "text 20 120 300 5");	//Release text
	strcpy(menuarray[linecount++], "button 150 120 1209 1210 1 0 203"); //Release button
	strcpy(menuarray[linecount++], "text 20 140 300 6");	//Kick user text
	strcpy(menuarray[linecount++], "button 150 140 1209 1210 1 0 204"); //kick button
	strcpy(menuarray[linecount++], "text 20 180 300 7");

	length=21;
	length2=1;
	
	for(line=0;line<linecount;line++)
	{
		if (strlen(menuarray[line])==0)
			break;
		{
			length+=strlen(menuarray[line])+4;
			length2+=strlen(menuarray[line])+4;
		}
	}
	
	length+=3;
	textlines=0;
	line=0;
	
	sprintf(menuarray1[linecount1++], "User %i selected (account %i)",buttonnum,pc_c->account());
	sprintf(menuarray1[linecount1++], "Name: %s",pc_c->name.c_str());   
	sprintf(menuarray1[linecount1++], "Goto Character:");
	sprintf(menuarray1[linecount1++], "Get Character:");
	sprintf(menuarray1[linecount1++], "Jail Character:");
	sprintf(menuarray1[linecount1++], "Release Character:");
	sprintf(menuarray1[linecount1++], "Kick Character:");
	sprintf(menuarray1[linecount1++], "Serial#[%8x]",pc_c->serial);   
	
	
	for(line=0;line<linecount1;line++)
	{
		if (strlen(menuarray1[line])==0)
			break;
		length+=strlen(menuarray1[line])*2 +2;
		textlines++;
	}
	
	gump1[1]=length>>8;
	gump1[2]=length%256;
	gump1[7]=0;
	gump1[8]=0;
	gump1[9]=0;
	gump1[10]=type; // Gump Number
	gump1[19]=length2>>8;
	gump1[20]=length2%256;
	Xsend(s, gump1, 21);
	
	for(line=0;line<linecount;line++)
	{
		sprintf(sect, "{ %s }", menuarray[line]);
		Xsend(s, sect, strlen(sect));
	}
	
	gump2[1]=textlines>>8;
	gump2[2]=textlines%256;
	
	Xsend(s, gump2, 3);
	
	for(line=0;line<linecount1;line++)
	{
		if (strlen(menuarray1[line])==0)
			break;
		{
			gump3[0]=strlen(menuarray1[line])>>8;
			gump3[1]=strlen(menuarray1[line])%256;
			Xsend(s, gump3, 2);
			gump3[0]=0;
			for (i=0;i<strlen(menuarray1[line]);i++)
			{
				gump3[1]=menuarray1[line][i];
				Xsend(s, gump3, 2);
			}
		}
	}
}

void cCommands::MakePlace(int s, int i) // Decode a teleport location number into X/Y/Z
{
	int x = 0, y = 0, z = 0, loopexit = 0;
	
	openscript("location.scp");
	sprintf((char*)temp, "LOCATION %i", i);
	if (i_scripts[location_script]->find((char*)temp))
	{
		do
		{
			read2();
			if (!strcmp((char*)script1,"X"))
			{
				x = str2num(script2);
			}
			else if (!strcmp((char*)script1,"Y"))
			{
				y = str2num(script2);
			}
			else if (!strcmp((char*)script1,"Z"))
			{
				z = str2num(script2);
			}
		}
		while ( (strcmp((char*)script1,"}")) && (++loopexit < MAXLOOPS) );
	}
	addx[s] = x;
	addy[s] = y;
	addz[s] = z;
	closescript();
}

void cCommands::DupeItem(int s, P_ITEM pi_target, int amount)
{
	P_CHAR pc_currchar = currchar[s];
	if( !pi_target || pi_target->corpse() ) 
		return;

	P_ITEM pPack = NULL;
	if( s != -1 && pc_currchar != NULL )
		pPack = Packitem( pc_currchar );
	else
		pPack = FindItemBySerial(pi_target->contserial);

	if(pPack == NULL) 
		return;//AntiChrist

//	P_ITEM pi_c = Items->MemItemFree();
//	pi_c->Init(0);
#pragma note("Replace by a copy constructor before finishing items[]")
	//memcpy(pi_c, pi_target, sizeof(cItem));
	P_ITEM pi_c = new cItem(*pi_target);
	pi_c->SetSerial(cItemsManager::getInstance()->getUnusedSerial());
	
	pi_c->setContSerial(pPack->serial);
	pi_c->SetOwnSerial(pi_target->ownserial);
	pi_c->SetSpawnSerial(pi_target->spawnserial);
	pi_c->setLayer( 0 ); // it's created in a backpack
	pi_c->setAmount( amount );
	
	RefreshItem(pi_c);//AntiChrist
}

void cCommands::ShowGMQue(int s, int type) // Shows next unhandled call in the GM queue
{
	
	// Type is 0 if it is a Counselor doing the command (or a GM doing /cq) and 1 if it is a GM
	
	int i;
	int x=0;
	
	if(type==1) //Player is a GM
	{
		for(i=1;i<MAXPAGES;i++)
		{
			if (gmpages[i].handled==0)
			{
				if(x==0)
				{
					sysmessage(s,"");
					sysmessage(s, tr("Next unhandled page from %1").arg(gmpages[i].name.c_str()));
					sysmessage(s, tr("Problem: %1.").arg(gmpages[i].reason.c_str()));
					sysmessage(s, tr("Serial number %1").arg(gmpages[i].serial, 16));
					sysmessage(s, tr("Paged at %1.").arg(gmpages[i].timeofcall, 16));
				}
				x++;
			}
		}
		if (x>0)
		{
			sysmessage(s,"");
			sysmessage(s, tr("Total pages in queue: %1").arg(x));
		}
		else sysmessage(s, tr("The GM queue is currently empty"));
	} //end of first if
	else //Player is a counselor so show counselor queue
	{
		for(i=1;i<MAXPAGES;i++)
		{
			if (counspages[i].handled==0)
			{
				if(x==0)
				{
					sysmessage(s,"");
					sysmessage(s, tr("Next unhandled page from %1").arg(counspages[i].name.c_str()));
					sysmessage(s, tr("Problem: %1.").arg(counspages[i].reason.c_str()));
					sysmessage(s, tr("Serial number %1").arg(counspages[i].serial, 16));
					sysmessage(s, tr("Paged at %1.").arg(counspages[i].timeofcall));
				}
				x++;
			}
		}
		if (x>0)
		{
			sysmessage(s,"");
			sysmessage(s, tr("Total pages in queue: %1").arg(x));
		}
		else sysmessage(s, tr("The Counselor queue is currently empty"));
	}
}
// new wipe function, basically it prints output on the console when someone wipes so that
// if a malicious GM wipes the world you know who to blame
void cCommands::Wipe(int s)
{
	P_CHAR pc_currchar = currchar[s];
	
	clConsole.send("WOLFPACK: %s has initiated an item wipe\n",pc_currchar->name.c_str());
	
	P_ITEM pi;
	AllItemsIterator iterItems;
	for (iterItems.Begin(); !iterItems.atEnd(); iterItems++)
	{
		pi = iterItems.GetData();
		if( pi->isInWorld() && !pi->wipe() )
		{
			iterItems--; // Iterator will became invalid when we delete it.
			Items->DeleItem(pi);
		}
	}
	sysbroadcast("All items have been wiped."); 
}

void cCommands::Possess(int s) 
{
/*	P_CHAR pPos = FindCharBySerPtr(buffer[s]+7);	// char to posess
	if (!pPos) return;
	
	if (pPos->shop)
	{
		sysmessage(s,"You cannot use shopkeepers.");
		return;
	}
	if (pPos->isPlayer())
	{
		sysmessage( s, "You can only possess NPCs." );
		return;
	}

	unsigned char tmp;
	P_CHAR pc_currchar  = currchar[s];

	if( pPos->npc == 17 ) // Char's old body
	{
		tmp = pPos->getPriv();
		pPos->setPriv(pc_currchar->getPriv());
		pc_currchar->setPriv(tmp);
		
		tmp = pPos->priv2;
		pPos->priv2 = pc_currchar->priv2;
		pc_currchar->priv2 = tmp;

		tmp = pPos->commandLevel;
		pPos->commandLevel = pc_currchar->commandLevel;
		pc_currchar->commandLevel = tmp;

		for(int i = 0; i < 7; i++)
		{ 
           int tempi = pPos->priv3[i]; 
           pPos->priv3[i] = pc_currchar->priv3[i]; 
           pc_currchar->priv3[i] = tempi;
		}

		pPos->npc = 0;
		pc_currchar->npc = 1;
		pc_currchar->account = -1;
		currchar[s] = pPos;
		Network->startchar( s );
		sysmessage( s, "Welcome back to your old body." );
	}
	else if( pPos->isNpc() )
	{
		tmp = pPos->getPriv();
		pPos->setPriv(pc_currchar->getPriv());
		pc_currchar->setPriv(tmp);
		
		tmp = pPos->priv2;
		pPos->priv2 = pc_currchar->priv2;
		pc_currchar->priv2 = tmp;
		
		tmp = pPos->commandLevel;
		pPos->commandLevel = pc_currchar->commandLevel;
		pc_currchar->commandLevel = tmp;

		for(int i = 0; i < 7; i++)
		{ 
           int tempi = pPos->priv3[i]; 
           pPos->priv3[i] = pc_currchar->priv3[i]; 
           pc_currchar->priv3[i] = tempi;
		}
		
		pPos->npc = 0;
		pPos->account = pc_currchar->account;
		pc_currchar->npc = 17;
		pc_currchar->npcWander = 0;
		currchar[s] = pPos;
		Network->startchar( s );
		sprintf((char*)temp,"Welcome to %s's body!", pPos->name.c_str() );
		sysmessage(s, (char*)temp);
	}
	else
		sysmessage( s, "Possession error %s." );
*/
	sysmessage(s, "This command has been temporary disabled");
}

// adds a command (string) into the stringlist of the privlvl if it's not in yet
void cCommands::addCmdToPrivLvl( QString privlvl, QString command )
{
	if( !privlvl_commands[ privlvl ].commands.contains( command ) )
		privlvl_commands[ privlvl ].commands.push_back( command );
}

// rmvs a command (sting) from the stringlist of the privlvl
void cCommands::rmvCmdFromPrivLvl( QString privlvl, QString command )
{
	if( privlvl_commands[ privlvl ].commands.contains( command ) )
		privlvl_commands[ privlvl ].commands.remove( command );
}

// explains itself :)
bool cCommands::containsCmd( QString privlvl, QString command )
{
	return ( ( privlvl_commands[ privlvl ].commands.contains( command ) && privlvl_commands[ privlvl ].implicit ) ||
		   ( !privlvl_commands[ privlvl ].commands.contains( command ) && !privlvl_commands[ privlvl ].implicit ) );
}

void cCommands::loadPrivLvlCmds( void )
{
	clConsole.PrepareProgress( "Loading PrivLvl Command Lists." );

	QStringList ScriptSections = DefManager->getSections( WPDT_PRIVLEVEL );
	
	if( ScriptSections.isEmpty() )
	{
		clConsole.ProgressFail();
		clConsole.ChangeColor( WPC_RED );
		clConsole.send("WARNING: Privlvls for admins, gms, counselors and players undefined!\n");
		clConsole.ChangeColor( WPC_NORMAL );
		return;
	}
	
	for(QStringList::iterator it = ScriptSections.begin(); it != ScriptSections.end(); ++it )
	{
		QDomElement *Tag = DefManager->getSection( WPDT_PRIVLEVEL, *it );

		if( Tag->isNull() || !Tag->attributes().contains( "id" ) )
			continue;
		
		QString privlvl = Tag->attribute( "id" );
		privlvl_commands[privlvl].implicit = !( Tag->attributes().contains( "type" ) && Tag->attribute( "type" ) == "explicit" );

		QDomNode childNode = Tag->firstChild();
		while( !childNode.isNull() )
		{
			this->addCmdToPrivLvl( privlvl, childNode.nodeName() );
			childNode = childNode.nextSibling();
		}
	}
	clConsole.ProgressDone();
}

