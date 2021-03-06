#################################################################
#   )      (\_     # WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  # Created by: khpae                          #
#   )).-' {{ ;'`   # Revised by:                                #
#  ( (  ;._ \\ ctr # Last Modification: Created                 #
#################################################################

import wolfpack
from wolfpack.consts import *
from wolfpack.utilities import *
import wolfpack.time
import skills

PEACE_DELAY = 5000

def peacemaking( char, skill ):
	if skill != PEACEMAKING:
		return False

	if char.socket.hastag( 'skill_delay' ):
		if wolfpack.time.currenttime() < char.socket.gettag( 'skill_delay' ):
			char.socket.clilocmessage( 500118, "", 0x3b2, 3 )
			return True
		else:
			char.socket.deltag( 'skill_delay' )

	# check instrument in backpack
	backpack = char.getbackpack()
	if not backpack:
		return False
	contents = backpack.content
	items = []
	for item in contents:
		if isinstrument( item ):
			items.append( item )

	# if we don't have an instrument in backpack, send target cursor to select an item - will be added
	if not len( items ):
		char.socket.clilocmessage( 500617, "", 0x3b2, 3 )
		char.socket.attachtarget( "skills.peacemaking.selectinstrument", [items] )
		return True
	selecttarget( char, items )

def selecttarget( char, items ):
	# if there are more than one instruments, select last used one
	last_time = 0
	if len( items ) > 1:
		instrument = items[ 0 ]
		for item in items:
			if item.hastag( 'last_musicianship_use' ):
				use_time = item.gettag( 'last_musicianship_use' )
				if use_time > last_time:
					last_time = use_time
					instrument = item
	else:
		instrument = items[ 0 ]

	char.socket.settag( 'peacemaking_instrument', instrument.serial )
	char.socket.clilocmessage( 1049525, "", 0x3b2, 3 )
	char.socket.attachtarget( "skills.peacemaking.response" )
	return True

def response( char, args, target ):
	if not char.socket.hastag( 'peacemaking_instrument' ):
		return False
	# you can only target chars
	if not target.char:
		return True
	instrument = wolfpack.finditem( char.socket.gettag( 'peacemaking_instrument' ) )
	if not instrument:
		return False

	if skills.skilltable[ PEACEMAKING ][ skills.UNHIDE ] and char.hidden:
		char.removefromview()
		char.hidden = False
		char.update()

	char.socket.deltag( 'peacemaking_instrument' )
	char.socket.settag( 'skill_delay', int( wolfpack.time.currenttime() + PEACE_DELAY ) )

	# if target him/her self : standard (regional) mode
	# anyone including npcs can re-target and start fight
	peace_range = skills.musicianship.bard_range( char )
	if char == target.char:
		result = char.checkskill( MUSICIANSHIP, 0, 1000 )
		skills.musicianship.play_instrument( char, instrument, result )
		# fail to play well
		if not result:
			char.socket.clilocmessage( 500612, "", 0x3b2, 3 )
			return True
		result = char.checkskill( PEACEMAKING, 0, 1000 )
		# fail on peacemaking
		if not result:
			char.socket.clilocmessage( 500613, "", 0x3b2, 3 )
			return True
		char.socket.clilocmessage( 500615, "", 0x3b2, 3 )
		creatures = wolfpack.chars( char.pos.x, char.pos.y, char.pos.map, peace_range )
		for creature in creatures:
			if char.canreach( creature, peace_range ):
				# stop combat
				# player chars
				if creature.socket:
					creature.socket.clilocmessage( 500616, "", 0x3b2, 3 )
	# target on an npc - effect will go some duration
	else:
		if char.canreach( target.char, peace_range ):
			char.socket.clilocmessage( 500618, "", 0x3b2, 3 )
			return True
		if not target.char.npc:
			return True
		# bard difficulty - later, we should get these from the xml defs.
		loskill = 0
		hiskill = 1000
		result = char.checkskill( MUSICIANSHIP, loskill, hiskill )
		skills.musicianship.play_instrument( char, instrument, result )
		if not result:
			char.socket.clilocmessage( 500612, "", 0x3b2, 3 )
			return True
		result = char.checkskill( PEACEMAKING, loskill, hiskill )
		if not result:
			char.socket.clilocmessage( 500613, "", 0x3b2, 3 )
			return True
		# FIXME : duration ( 5 sec ~ 65 sec )
		duration = 5000 + char.skill[ PEACEMAKING ] * 60
		# stop combat
		# if npc, do not start combat for the duration while not attacked
		creature.settag( 'peacemaking', 1 )
		creature.addtimer( duration, release, [] )
	return True

def selectinstrument( char, args, target ):
	items = args[0]
	if target.item:
		if not char.gm and not char.canreach( target.item, 2 ):
			char.socket.clilocmessage( 500295 )
			return True
		if isinstrument( target.item ):
			items.append( target.item )
			selecttarget( char, items )
		else:
			char.socket.sysmessage( "You need an instrument." )
	else:
		char.socket.sysmessage( "You need an instrument." )
	return True
	
def release( char, args ):
	if char.socket.hastag( 'peacemaking' ):
		char.socket.deltag( 'peacemaking' )

def onLoad():
	skills.register( PEACEMAKING, peacemaking )
