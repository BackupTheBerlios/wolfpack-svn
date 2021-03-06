#===============================================================#
#   )      (\_     | WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  | Created by: Correa                         #
#   )).-' {{ ;'`   | Revised by:                                #
#  ( (  ;._ \\ ctr | Last Modification: Created                 #
#===============================================================#

import wolfpack
from wolfpack.time import servertime
from skills.animaltaming import dotame

def onContextCheckVisible(char, target, tag):
	# Return false if already owned.
	return not target.owner	

def onContextEntry(char, target, tag):
	if tag == 1:
		if char.hastag('skill_delay'):
			cur_time = servertime()
			if cur_time < char.gettag('skill_delay'):
				char.socket.clilocmessage(500118, "", 0x3b2, 3)
				return 1
			else:
				char.deltag('skill_delay')
		dotame(char, target)
	return 1
