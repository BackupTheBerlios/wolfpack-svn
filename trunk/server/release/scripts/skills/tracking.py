#################################################################
#   )      (\_     # WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  # Created by: Correa                         #
#   )).-' {{ ;'`   # Revised by:                                #
#  ( (  ;._ \\ ctr # Last Modification: Created                 #
#################################################################

from wolfpack.consts import *
from wolfpack.utilities import *
from wolfpack.time import *
from wolfpack.gumps import *
import wolfpack

STEALTH_DELAY = 5000
# the hiding skill before you can use the stealth skill
MIN_HIDING = 800

def onLoad():
	wolfpack.registerglobal( HOOK_CHAR, EVENT_SKILLUSE, "skills.tracking" )

def onSkillUse( char, skill ):
	if skill != TRACKING:
		return 0

	if char.hastag( 'skill_delay' ):
		cur_time = servertime()
		if cur_time < char.gettag( 'skill_delay' ):
			char.socket.clilocmessage( 500118, "", 0x3b2, 3 )
			return 1
		else:
			char.deltag( 'skill_delay' )

        char.socket.cloclocmessage( 1011350 ) # What do you wish to track?
        char.socket.closegump( 0x87651592 ) # What to track
        gump = cGump( x = 20, y = 30, callback="skills.tracking.trackWhatResponse", type=0x87651592 )

        gump.startPage( 0 )
        gump.addBackground( 5054, 440, 135 )
        gump.addResizeGump( 10, 10, 2620, 420, 75 )
        gump.addResizeGump( 10, 85, 3000, 420, 25 )

        gump.addTilePic( 20, 20, 9682 )
        gump.addButton( 20, 110, 4005, 4007, 1 )
        gump.addXmfHtmlGump( 20, 90, 100, 20, 1018087 ) # Animals

        gump.addTilePic( 120, 20, 9607 )
        gump.addButton( 120, 110, 4005, 4007, 2 )
        gump.addXmfHtmlGump( 120, 90, 100, 20, 1018088 ) # Monsters

        gump.addTilePic( 220, 20, 8454 )
        gump.addButton( 220, 110, 4005, 4007, 3 )
        gump.addXmfHtmlGump( 220, 90, 100, 20, 1018089 ) # Human NPCs
        
        gump.addTilePic( 320, 20, 8455 )
        gump.addButton( 320, 110, 4005, 4007, 4 )
        gump.addXmfHtmlGump( 320, 90, 100, 20, 1018090 ) # Players

        gump.send( char )
	
	cur_time = servertime()
	char.settag( 'skill_delay', cur_time + STEALTH_DELAY )

	return 1

def trackWhatResponse( char, args, target ):
    return 1

