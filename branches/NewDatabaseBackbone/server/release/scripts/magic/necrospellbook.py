#===============================================================#
#   )      (\_     | WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  | Created by: Radiant                        #
#   )).-' {{ ;'`   | Revised by:                                #
#  ( (  ;._ \\ ctr | Last Modification: Created                 #
#===============================================================#
# Necromancy Spellbook                                          #
#===============================================================#
import wolfpack
from math import floor,ceil

def countspells(item):
	count = 0

	for i in range( 1, 2 ):
		if item.hastag('circle' + str(i)):
			spells = int(item.gettag('circle' + str(i)))
			for j in range(0, 9):
				if (spells >> j) & 0x01:
					count += 1
	return count

def hasspell( item, spell ):
	if item and item.hasscript( 'magic.necrospellbook' ):
		spell = spell - 200

		circle = int( floor( spell / 2 ) ) + 1 # 0 for first circle
		spell = spell % 2

		if item.hastag( 'circle' + str( circle ) ):
			spells = int( item.gettag( 'circle' + str( circle ) ) )

			return spells & ( 0x01 << spell )

	return 0

def addspell( item, spell ):
	if not item or not item.hasscript( 'magic.necrospellbook' ):
		return 0
	circle = int( floor( spell / 2 ) ) + 1 # 0 for first circle
	spell = spell % 2
	spells = 0
	if item.hastag( 'circle' + str( circle ) ):
		spells = int( item.gettag( 'circle' + str( circle ) ) )
	spells |= 0x01 << spell
	item.settag( 'circle' + str( circle ), spells )
	item.resendtooltip()

	return 1


def onUse(char, item):
	if item.getoutmostchar() != char:
		char.socket.sysmessage('The book has to be in your belongings to be used.')
		return 1

	packet = wolfpack.packet( 0x24, 7 )
	packet.setint( 1, item.serial )
	packet.setshort( 5, 0xffff )
	packet.send( char.socket )

	packet = wolfpack.packet( 0xbf, 23 )
	packet.setshort( 1, 23 )	 # Packet length
	packet.setshort( 3, 0x1b )	 # 0xbf subcommand
	packet.setshort( 5, 1	 )	 # Unknown. Maybe it's a subsubcommand ?
	packet.setint( 7, item.serial ) # Spellbook serial
	packet.setshort( 11, item.id ) # Item id
	packet.setshort( 13, 201 ) # Scroll offset (1 = regular, 101 = paladin, 201 = necro)

	for i in range( 0, 2 ):
		if not item.hastag( 'circle' + str( i + 1 ) ):
			packet.setbyte( 15 + i, 0 )
		else:
			packet.setbyte( 15 + i, int( item.gettag( 'circle' + str( i + 1 ) ) ) )

	packet.send( char.socket )
	return 1

def onShowTooltip(viewer, object, tooltip):
	tooltip.add(1042886, str(countspells(object)))
