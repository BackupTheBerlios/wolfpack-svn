
import wolfpack
import wolfpack.time
import whrandom
import skills
from wolfpack.consts import *
from wolfpack.utilities import *
from random import randint
#import weapons.blades

woodrespawndelay = randint( LUMBERJACKING_REFILLTIME[0], LUMBERJACKING_REFILLTIME[1] )
woodspawnamount = randint( LUMBERJACKING_LOGS[0], LUMBERJACKING_LOGS[1] )
chopdistance = LUMBERJACKING_MAX_DISTANCE
nextchopdelay = 3000

# Lumberjacking Harvest Table Items
REQSKILL = 0
MINSKILL = 1
COLORID = 2
RESOURCENAME = 3
# resname, reqSkill, maxSkill, color
woodtable = \
{
	'plainwood': [ 0, 10, 0x0 ]
}
yewtree = [ 4789, 4790, 4791, 4792, 4793, 4794, 4795, 4796, 4797 ]

def response( args ):
	target = args[0]
	tool = args[1]
	char = args[2]
	socket = char.socket
	pos = target.pos

	if not socket:
		return False

	if socket.hastag('is_lumberjacking') and ( socket.gettag( 'is_lumberjacking' ) > wolfpack.time.currenttime() ):
		socket.clilocmessage( 500119, "", GRAY )
		return False
	else:
		socket.deltag('is_lumberjacking')

	# Player can reach that ?
	if char.pos.map != pos.map or char.pos.distance( pos ) > chopdistance:
		# That is too far away
		socket.clilocmessage( 500446, "", GRAY )
		return True

	#Player also can't lumberjack when riding, polymorphed and dead.
	if char.itemonlayer( LAYER_MOUNT ):
		# You can't use this while on a mount!
		char.socket.clilocmessage( 1049627, "", GRAY )
		return False

	##########
	if socket.hastag( 'wood_gem' ):
		veingem = wolfpack.finditem( socket.gettag( 'wood_gem' ) )
		if not veingem:
			veingem = getvein( socket, pos, target )
			if not veingem:
				socket.deltag( 'wood_gem' )
				veingem = createwoodgem( target, pos )
				socket.settag( 'wood_gem', int(veingem.serial) )
	else:
		veingem = getvein( socket, pos, target )
		# OK, we still don't have a veingem to get our wood, lets create it here.
		if not veingem:
			veingem = createwoodgem( target, pos )
		else:
			socket.settag( 'wood_gem', veingem.serial )

	if not veingem:
		veingem = createwoodgem( target, pos )

	if not veingem.hastag( 'resname' ) or not veingem.hastag( 'resourcecount' ):
		return False

	elif veingem.hastag( 'resname' ):
		resname = veingem.gettag( 'resname' )

	socket.settag( 'is_lumberjacking', int( wolfpack.time.currenttime() + nextchopdelay ) )
	hack_logs( char, target, tool, veingem )

	return True

def createwoodgem( target, pos ):
	gem = wolfpack.additem( 'wood_gem' )
	gem.settag( 'resourcecount', int( woodspawnamount ) ) # 10 - 34 ore
	#if ( target.model in yewtree ) or ( target.id in yewtree ):
	#	resource.settag( 'resname', 'yewwood' )
	#else:
	gem.settag( 'resname', 'plainwood' )
	gem.moveto( pos )
	gem.visible = 0
	gem.update()
	return gem

def getvein( socket, pos, target ):
	#Check if we have wood_gems near ( range = 4)
	gems = wolfpack.items( pos.x, pos.y, pos.map, 4 )
	for gem in gems:
		if wolfpack.finditem( gem.serial ):
			if gem.hastag('resource') and gem.gettag('resource') == 'wood' and gem.hastag('resname') and gem.id == hex2dec( 0x1ea7 ):
				return gem
				break
	if not gems:
		gem = createwoodgem(target, pos)
		return gem

# Delay for chopping trees and getting the logs
# Animation Sequence
def chop_tree( char, args ):
	pos = args[0]
	socket = char.socket

	if char.pos.map != pos.map or char.pos.distance( pos ) > chopdistance:
		socket.deltag( 'is_lumberjacking' )
		return False

	# Turn to our lumberjacking position
	direction = char.directionto( pos )
	if char.direction != direction:
		char.direction = direction
		char.updateflags()

	# Let him hack
	char.action( 0xd )
	char.soundeffect( 0x13e )
	return True

# HACK LOGS
def hack_logs( char, target, tool, resource ):
	socket = char.socket
	pos = target.pos

	resname = resource.gettag( 'resname' )
	amount = resource.gettag( 'resourcecount' )

	resourcecount = resource.gettag( 'resourcecount' )

	# No resource left to harvest?
	if resourcecount <= 0:
		socket.clilocmessage( 500488, '', GRAY )
		return False

	# Turn to our lumberjacking position
	direction = char.directionto( pos )
	if char.direction != direction:
		char.direction = direction
		char.updateflags()

	# Let him hack
	char.action( 0xd )
	char.soundeffect( 0x13e )
	char.addtimer( 2000, "skills.lumberjacking.chop_tree", [pos] )
	char.addtimer( 3500, "skills.lumberjacking.chop_tree", [pos] )
	char.addtimer( 4000, "skills.lumberjacking.successlumberjacking", [pos, resource, amount, tool, resname, woodtable ] )
	return

# HACK KINDLINGS
def hack_kindling( char, pos ):
	socket = char.socket

	if pos.distance( char.pos ) > 3:
		char.socket.clilocmessage( 0x7A258 ) # You cannot reach that
		return 1

	direction = char.directionto( pos )
	if char.direction != direction:
		char.direction = direction
		char.updateflags()

	# Let's add some Kindling
	item = wolfpack.additem( "de1" )
	if not wolfpack.utilities.tobackpack( item, char ):
		item.update()

	# Let him hack
	char.action( 0x9 )
	char.soundeffect( 0x13e )
	char.socket.clilocmessage( 0x7A30B ) # You put some kindlings in your pack

def successlumberjacking( char, args ):
	socket = char.socket
	pos = args[0] # Target POS
	resource = args[1]
	amount = args[2]
	tool = args[3]
	resname = args[4]
	table = args[5]

	# Lets make sure we stayed next to the tree
	# Player can reach that ?
	if char.pos.map != pos.map or char.pos.distance( pos ) > chopdistance:
		socket.sysmessage("You have moved too far away to gather any wood.")
		socket.deltag( 'is_lumberjacking' )
		return False

	if not resource.hastag( 'resourcecount' ):
		return False

	if int( resource.gettag( 'resourcecount' ) ) <= 0:
		char.socket.clilocmessage( 500488, '', GRAY )
		return False

	reqskill = woodtable[ resname ][ REQSKILL ]
	chance = int( ( char.skill[ LUMBERJACKING ] - woodtable[ resname ][ MINSKILL ] ) / 10 )

	if char.skill[ LUMBERJACKING ] < reqskill:
		char.socket.clilocmessage( 500298 ) # You are not skilled enough...
		return False
	else:
		# Skill Check against LUMBERJACKING
		if not char.checkskill( LUMBERJACKING, reqskill, 1200 ):
			char.socket.clilocmessage( 500495 ) # You hack at the tree for a while but fail to produce...
			success = 0
			return False
		elif chance >= randint(1, 100):
			char.socket.clilocmessage( 500498 ) # You put some logs into your backpack
			if tool.gettag( 'remaining_uses' ) > 1:
				tool.settag( 'remaining_uses', int( int( tool.gettag( 'remaining_uses' ) ) - 1 ) )
				tool.resendtooltip()
			elif tool.gettag( 'remaining_uses' ) == 1:
				tool.delete()
				# You broke your axe!
				socket.clilocmessage( 500499, '', GRAY )
			char.socket.deltag( 'wood_gem' )
			char.socket.deltag( 'is_lumberjacking' )
			success = 1
		else:
			char.socket.clilocmessage( 500495 ) # You hack at the tree for a while but fail to produce...
			success = 0
			return False

	if success == 1:
		# Check for a backpack
		backpack = char.getbackpack()
		if not backpack:
			return False
		# Create an item in my pack (logs to be specific)
		resourceitem = wolfpack.additem( "1bdd" )
		if ( FELUCIA2XRESGAIN == TRUE ) and ( char.pos.map  == 0 ):
			resourceitem.amount = 20
		else:
			resourceitem.amount = 10
		resourceitem.settag( 'resname', resname ) # Used when crafting
		if not wolfpack.utilities.tobackpack( resourceitem, char ):
			resourceitem.update()

		if resource.gettag( 'resourcecount' ) >= 1:
			resource.settag( 'resourcecount', int( amount - 1 ) )
		elif resource.gettag( 'resourcecount' ) == 0:
			if not resource.hastag ('resource_empty') and int( resource.gettag( 'resourcecount' ) ) == 0:
				resource.settag( 'resource_empty', 'true' )
				resource.addtimer( woodrespawndelay, "skills.lumberjacking.respawnvein", [], True )
		return True

def respawnvein( vein, args ):
	if vein.hastag ('resource_empty') and int(vein.gettag( 'resourcecount' )) == 0:
		vein.settag( 'resourcecount', int( woodspawnamount ) )
		vein.deltag( 'resource_empty' )
		return True
	else:
		return False
