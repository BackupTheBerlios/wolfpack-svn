
import wolfpack
from wolfpack.utilities import energydamage
from wolfpack import tr
from wolfpack.consts import *

# The maximum poisonlevel that can be resisted by orange petals
ORANGEPETALS_RESIST_LEVEL = 2

#
# The poison types
#
POISONS = {
#( string name, int min, int max, double percent, double delay, double interval, int count, int messageInterval )
	# ID: ['NAME', MIN, MAX, PERCENT, DELAY, INTERVAL, HOWOFTEN, MESSAGECOUNT]
	0: ["Lesser", 4, 16,  0.075, 3000, 2250, 10, 4],
	1: ["Regular", 8, 18, 0.10, 3000, 3250, 10, 3],
	2: ["Greater", 12, 20, 0.15, 3000, 4250, 10, 2],
	3: ["Deadly", 16, 30, 0.30, 3000, 5250, 15, 2],
	4: ["Lethal", 20, 50, 0.35, 3000, 5250, 20, 2],
}

#
# Execute a poison stroke
#
def stroke(char, arguments):
	strokes = arguments[0]

	# First: Check if the character disconnected
	# If he did, save the strokes and level and restore it later
	if char.player and not char.socket and char.logouttime == 0:
		if not char.hasscript( 'system.poison' ):
			char.settag( 'poison_strokes', strokes )
			char.addscript( 'system.poison' )
		return

	if not char.poison in POISONS:
		if char.poison != -1:
			char.poison = -1
			char.updateflags()
		return

	# Check for orange petals
	if char.hastag('orangepetals') and char.poison <= ORANGEPETALS_RESIST_LEVEL:
		# Cure Poison
		if char.poison != -1:
			char.poison = -1
			char.updateflags()

		char.message(tr("* You feel yourself resisting the effects of the poison *" ), 0x3f)
		
		# Show to everyone else
		listeners = wolfpack.chars(char.pos.x, char.pos.y, char.pos.map, 18)
		text = unicode(tr("* %s seems resistant to the poison *")) % unicode(char.name)

		for listener in listeners:
			if listener != char and listener.socket:
				listener.socket.showspeech(char, text, 0x3f)

		return

	poison = POISONS[char.poison]

	# Show a message if the cycle is ok
	if strokes % poison[7] == 0:
		chars = wolfpack.chars(char.pos.x, char.pos.y, char.pos.map, 18)
		for viewer in chars:
			if viewer.socket and viewer.cansee(char):
				if viewer != char:
					viewer.socket.clilocmessage(1042858 + char.poison * 2, char.name, 34, 3, char)
				else:
					viewer.socket.clilocmessage(1042857 + char.poison * 2, '', 34, 3, char)

	dodamage(char, poison)

	# See if we should add another timer
	strokes += 1
	if strokes == poison[6]:
		if char.poison != -1:
			char.poison = -1
			char.updateflags()
		if char.socket:
			char.socket.clilocmessage(502136)
		return

	char.addtimer(poison[5], stroke, [strokes], 0, 0, "poison_timer")

def dodamage(char, poison):
	damage = 1 + int(char.hitpoints * poison[3])
	damage = min(poison[2], max(poison[1], damage))
	energydamage(char, None, damage, poison=100)

#
# Cure the currently applied poison
#
def cure(char):
	char.dispel(None, 1, "poison_timer", [])
	if char.poison != -1:
		char.poison = -1
		char.updateflags()

#
# Apply poison to a character.
# Apply the required tags and start the timer.
#
def poison(char, level):
	if not level in POISONS:
		return False

	if char.hasscript('magic.evilomen'):
		if (level+1) in POISONS: # check if it's possible to increase the poison
			level += 1

	# Level is smaller than old poison
	if level <= char.poison:
		return False

	# Check for poison immunity
	poison_immunity = char.getintproperty('poison_immunity', -1)
	
	if poison_immunity != -1 and level <= poison_immunity:
		return False # Do nothing. We're immune to that kind of poison

	# Delete current poison
	char.dispel(None, 1, "poison_timer", [])

	poison = POISONS[level]
	if char.poison == -1:
		char.poison = level
		char.updateflags()
	else:
		char.poison = level

	char.addtimer(poison[4], stroke, [0], 0, 0, "poison_timer")
	return True

#
# Reattach the poison timer
#
def onLogin(char):
	char.removescript( 'system.poison' )

	if not char.poison in POISONS:
		if char.poison != -1:
			char.poison = -1
			char.updateflags()
	elif char.hastag('poison_strokes'):
		poison = POISONS[char.poison]
		try:
			strokes = int(char.gettag('poison_strokes'))
			char.addtimer(poison[5], stroke, [strokes])
		except:
			raise

	char.deltag('poison_strokes')
	return False

def poison_target(player, arguments, target):
	if not target.char:
		player.socket.sysmessage(tr('You have to target a character.'))
		return

	poison(target.char, arguments[0])

"""
	\command poison
	\description Afflict a character with poison.
	\usage - <code>poison level</code>
	Level is one of the following values:
	<code>0 - Lesser Poison
	1 - Regular Poison
	2 - Greater Poison
	3 - Deadly Poison
	4 - Lethal Poison</code>
"""
def poison_command(socket, command, arguments):
	try:
		level = int(arguments)
		if level < 0 or level > 4:
			socket.sysmessage(tr('Allowed levels: 0 to 4.'))
		else:
			socket.sysmessage(tr('Select the character you want to poison.'))
			socket.attachtarget("system.poison.poison_target", [level])
	except:
		socket.sysmessage(tr('Usage: poison <level>'))

def onLoad():
	wolfpack.registercommand('poison', poison_command)
