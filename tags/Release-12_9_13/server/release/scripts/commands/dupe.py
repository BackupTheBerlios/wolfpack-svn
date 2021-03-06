#===============================================================#
#   )      (\_     | WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  | Created by: DarkStorm                      #
#   )).-' {{ ;'`   | Revised by:                                #
#  ( (  ;._ \\ ctr | Last Modification: Created                 #
#===============================================================#
# .dupe Command                                                 #
#===============================================================#

"""
	\command dupe
	\description Duplicates an item 1 or more times.
	\usage - <code>dupe</code>
	- <code>dupe amount</code>
	If no amount is given, only one item will be duplicated. Otherwise the
	item will be duplicated according to the given parameter. Please note
	that amount is not the new amount of the item, but rather how often
	the item should be duplicated.
"""

import wolfpack
from wolfpack.utilities import hex2dec
from wolfpack import console
from wolfpack.consts import *

def dupe(socket, command, arguments):
	arguments = arguments.strip() # Remove trailing and leading whitespaces
	amount = 1

	if len( arguments ) > 0:
		try:
			amount = hex2dec(arguments)
		except:
			socket.sysmessage( 'Usage: dupe <amount>' )
			return False

	socket.sysmessage( 'Please choose an item to dupe.' )
	socket.attachtarget( "commands.dupe.callback", [amount] )

def callback(char, args, target):
	if not target.item:
		char.socket.sysmessage('You have to target an item.')
		return False

	#char.log(LOG_MESSAGE, "Duping item 0x%x.\n" % target.item.serial)

	for i in range(0, args[0]):
		created = target.item.dupe()
		created.update()
		
		char.log(LOG_MESSAGE, "Duping item 0x%x. New serial is 0x%x.\n" % (target.item.serial, created.serial))				

def onLoad():
	wolfpack.registercommand("dupe", dupe)
