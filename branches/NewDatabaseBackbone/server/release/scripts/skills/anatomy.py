#################################################################
#   )      (\_     # WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  # Created by: DarkStorm                      #
#   )).-' {{ ;'`   # Revised by:                                #
#  ( (  ;._ \\ ctr # Last Modification: Created                 #
#################################################################

import wolfpack
import wolfpack.time
import skills
from wolfpack.consts import ANATOMY
from math import floor

ANATOMY_DELAY = 1000
MINSKILL = 0
MAXSKILL = 1200

# Button for Anatomy pressed on skill gump
def anatomy( char, skill ):
	# We only handle anatomy
	if skill != ANATOMY:
		return False

	socket = char.socket

	if socket.hastag( 'skill_delay' ):
		if wolfpack.time.currenttime() < socket.gettag( 'skill_delay' ):
			socket.clilocmessage( 500118, "", 0x3b2, 3 )
			return True
		else:
			socket.deltag( 'skill_delay' )

	# Assign the target request
	socket.clilocmessage( 0x7A261, "", 0x3b2, 3 ) # Whom shall I examine?
	socket.attachtarget( "skills.anatomy.response" )

	return True

def response( char, args, target ):
	if skills.skilltable[ ANATOMY ][ skills.UNHIDE ] and char.hidden:
		char.removefromview()
		char.hidden = False
		char.update()

	socket = char.socket
	# Check for a valid target
	if not target.char:
		# Only living things have anatomies
		if target.item:
			socket.clilocmessage( 0x7A263, "", 0x3b2, 3, target.item )
			return
		else:
			socket.clilocmessage( 0x7A263, "", 0x3b2, 3, char )
		return

	if target.char == char:
		# You know yourself quite well enough already.
		socket.clilocmessage( 0x7A264, "", 0x3b2, 3, char )
		return

	# You can't reach that (too far away, no los, wrong map)
	if char.cansee( target.char ) and not char.canreach( target.char, 10 ):
		# I am too far away to do that.
		socket.clilocmessage( 0x7A265, "", 0x3b2, 3 )
		return

	# Turn toward the char we want to look at
	char.turnto( target.char )

	# Invulnerable Characters cannot be examined
	if target.char.dead:
		socket.clilocmessage( 0x7A266, "", 0x3b2, 3, target.char ) # That cannot be inspected.
		return

	char.socket.settag( 'skill_delay', int( wolfpack.time.currenttime() + ANATOMY_DELAY ) )

	# Make a skillcheck and display the fail or success message above the targets head
	if not char.checkskill( ANATOMY, MINSKILL, MAXSKILL ):
		socket.clilocmessage( 0xFE8EA, "", 0x3b2, 3, target.char ) # You cannot quite get a sense of their physical characteristics.
		return

	# Anatomy Messages start: 0xFD6DD
	strId = min( 10, int( target.char.strength / 10 ) )
	dexId = min( 10, int( target.char.dexterity / 10 ) )
	msgId = int( 0xFD6DD + strId * 11 + dexId )
	dexRatio = float( 100.0 / target.char.dexterity )
	StamId = floor( ( target.char.stamina * dexRatio ) / 10 )
	msgId2 = int( 0xFD7DF + StamId )

	socket.clilocmessage( msgId, "", 0x3b2, 3, target.char, "", True )

	if char.skill[ANATOMY] >= 650:
		socket.clilocmessage( msgId2, "", 0x3b2, 3, target.char, "", True )

# Register as a global script
def onLoad():
	skills.register( ANATOMY, anatomy )
