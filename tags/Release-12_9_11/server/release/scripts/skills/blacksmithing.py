#################################################################
#	 )			(\_		 # WOLFPACK 13.0.0 Scripts										#
#	((		_/{	"-;	# Created by: DarkStorm											#
#	 )).-' {{ ;'`	 # Revised by:																#
#	( (	;._ \\ ctr # Last Modification: Created								 #
#################################################################

from wolfpack import console
from wolfpack.consts import *
import math
import wolfpack
from system.makemenus import CraftItemAction, MakeMenu, findmenu
from wolfpack.utilities import hex2dec, tobackpack
from wolfpack.properties import itemcheck, fromitem
import random

# Metals used by Blacksmithing
# The first four values are requied by the makemenu system.
# The last value is the resname to use for newly crafted items
METALS = [
		['Iron',				BLACKSMITHING, 0, ['iron_ingot'], 0x0, 'iron'],
		['Dull Copper', BLACKSMITHING, 650, ['dullcopper_ingot'], 0x973, 'dullcopper'],
		['Shadow Iron', BLACKSMITHING, 700, ['shadowiron_ingot'], 0x966, 'shadowiron'],
		['Copper',			BLACKSMITHING, 750, ['copper_ingot'], 0x96d, 'copper'],
		['Bronze',			BLACKSMITHING, 800, ['bronze_ingot'], 0x972, 'bronze'],
		['Gold',				BLACKSMITHING, 850, ['gold_ingot'], 0x8a5, 'gold'],
		['Agapite',		 BLACKSMITHING, 900, ['agapite_ingot'], 0x979, 'agapite'],
		['Verite',			BLACKSMITHING, 950, ['verite_ingot'], 0x89f, 'verite'],
		['Valorite',		BLACKSMITHING, 990, ['valorite_ingot'], 0x8ab, 'valorite'],
]

#
# A list of scales used by the blacksmithing menu.
#
SCALES = [
	['Red Scales', 0, 0, ['red_scales'], 0x66D, 'red_scales'],
	['Yellow Scales', 0, 0, ['yellow_scales'], 0x8A8, 'yellow_scales'],
	['Black Scales', 0, 0, ['black_scales'], 0x455, 'black_scales'],
	['Green Scales', 0, 0, ['green_scales'], 0x851, 'green_scales'],
	['White Scales', 0, 0, ['white_scales'], 0x8FD, 'white_scales'],
	['Blue Scales', 0, 0, ['blue_scales'], 0x8B0, 'blue_scales'],
]

#
# Check for anvil and forge
#
def checkanvilandforge(char):
	# Check dynamic items.
	# We should later check for statics too
	forge = 0
	anvil = 0
	items = wolfpack.items(char.pos.x, char.pos.y, char.pos.map, 5)
	for item in items:
		if item.id == 0xFAF or item.id == 0xFB0:
			anvil = 1
		elif item.id == 0xFB1 or (item.id >= 0x197A and item.id <= 0x19A9):
			forge = 1

		if anvil and forge:
			return True

	return False

#
# Check if the character is using the right tool
#
def checktool(char, item, wearout = 0):
	if not item:
		return False

	# Has to be in our posession
	if item.getoutmostchar() != char:
		char.socket.clilocmessage(500364)
		return False

	# We do not allow "invulnerable" tools.
	if not item.hastag('remaining_uses'):
		char.socket.clilocmessage(1044038)
		item.delete()
		return False

	# See if we have another tool equipped
	equipped = char.itemonlayer(LAYER_RIGHTHAND)
	if equipped and equipped != item:
		char.socket.clilocmessage(1048146)
		return False

	if wearout:
		uses = int(item.gettag('remaining_uses'))
		if uses <= 1:
			char.socket.clilocmessage(1044038)
			item.delete()
			return False
		else:
			item.settag('remaining_uses', uses - 1)

	return True

#
# Bring up the blacksmithing menu
#
def onUse(char, item):
	if not checktool(char, item):
		return True

	if not checkanvilandforge(char):
		char.socket.clilocmessage(1044267)
		return True

	menu = findmenu('BLACKSMITHING')
	if menu:
		menu.send(char, [item.serial])
	return True

#
# Smith an item.
# Used for scales + ingots
#
class SmithItemAction(CraftItemAction):
	def __init__(self, parent, title, itemid, definition):
		CraftItemAction.__init__(self, parent, title, itemid, definition)
		self.markable = 1 # All blacksmith items are markable

	#
	# Check if we did an exceptional job.
	#
	def getexceptionalchance(self, player, arguments):
		# Only works if this item requires blacksmithing
		if not self.skills.has_key(BLACKSMITHING):
			return False

		minskill = self.skills[BLACKSMITHING][0]
		maxskill = self.skills[BLACKSMITHING][1]
		penalty = self.skills[BLACKSMITHING][2]

		if not penalty:
			penalty = 250

		minskill += penalty
		maxskill += penalty

		chance = ( player.skill[BLACKSMITHING] - minskill ) / 10

		# chance = 0 - 100
		if chance > 100:
			chance = 100
		elif chance < 0:
			chance = chance * -1

		# chance range 0.00 - 1.00
		chance = chance * .01
		return chance

	#
	# Apply resname and color to the item.
	#
	def applyproperties(self, player, arguments, item, exceptional):
		# See if special ingots were used in the creation of
		# this item. All items crafted by blacksmiths gain the
		# color!
		if self.submaterial1 > 0:
			material = self.parent.getsubmaterial1used(player, arguments)
			material = self.parent.submaterials1[material]
			item.color = material[4]
			item.settag('resname', material[5])

		# Apply properties of secondary material
		if self.submaterial2 > 0:
			material = self.parent.getsubmaterial2used(player, arguments)
			material = self.parent.submaterials2[material]
			item.color = material[4]
			item.settag('resname2', material[5])

		# Apply one-time boni
		healthbonus = fromitem(item, DURABILITYBONUS)
		if healthbonus != 0:
			bonus = int(math.ceil(item.maxhealth * (healthbonus / 100.0)))
			item.maxhealth = max(1, item.maxhealth + bonus)
			item.health = item.maxhealth

		#weightbonus = fromitem(item, WEIGHTBONUS)
		#if weightbonus != 0:
			#bonus = int(math.ceil(item.weight * (weightbonus / 100.0)))
			#item.weight = max(0, item.weight + bonus)

		# Distribute another 6 points randomly between the resistances this armor already has
		if exceptional:
			if itemcheck(item, ITEM_ARMOR) or itemcheck(item, ITEM_SHIELD):
				# Copy all the values to tags
				boni = [0, 0, 0, 0, 0]

				for i in range(0, 6):
					boni[random.randint(0,4)] += 1

				item.settag('res_physical', fromitem(item, RESISTANCE_PHYSICAL) + boni[0])
				item.settag('res_fire', fromitem(item, RESISTANCE_FIRE) + boni[1])
				item.settag('res_cold', fromitem(item, RESISTANCE_COLD) + boni[2])
				item.settag('res_energy', fromitem(item, RESISTANCE_ENERGY) + boni[3])
				item.settag('res_poison', fromitem(item, RESISTANCE_POISON) + boni[4])
			elif itemcheck(item, ITEM_WEAPON):
				# Increase the damage bonus by 20%
				bonus = fromitem(item, DAMAGEBONUS)
				bonus += 20
				item.settag('aos_boni_damage', bonus)

		# Reduce the uses remain count
		checktool(player, wolfpack.finditem(arguments[0]), 1)

	#
	# First check if we are near an anvil and forge.
	# Then play a blacksmithing sound.
	#
	def make(self, player, arguments, nodelay=0):
		assert(len(arguments) > 0, 'Arguments has to contain a tool reference.')

		# Look for forge and anvil
		if not checkanvilandforge(player):
			player.socket.clilocmessage(1044267)
			return False

		if not checktool(player, wolfpack.finditem(arguments[0])):
			return False

		return CraftItemAction.make(self, player, arguments, nodelay)

	#
	# Play a simple soundeffect
	#
	def playcrafteffect(self, player, arguments):
		player.soundeffect(0x2a)

#
# A blacksmith menu. The most notable difference is the
# button for selecting another ore.
#
class BlacksmithingMenu(MakeMenu):
	def __init__(self, id, parent, title):
		MakeMenu.__init__(self, id, parent, title)
		self.allowmark = 1
		self.allowrepair = 1
		#self.allowenhance = 1
		self.allowsmelt = 1
		self.submaterials1 = METALS
		self.submaterials2 = SCALES
		self.submaterial2missing = 1060884
		self.submaterial1missing = 1044037
		self.submaterial1noskill = 1044268
		self.gumptype = 0x4f1ba410 # This should be unique

	#
	# Repair an item
	#
	def repair(self, player, arguments, target):
		if not checkanvilandforge(player):
			player.socket.clilocmessage(1044282)
			return

		if not checktool(player, wolfpack.finditem(arguments[0])):
			return

		if not target.item:
			player.socket.clilocmessage(500426)
			return

		if target.item.container != player.getbackpack():
			player.socket.clilocmessage(1044275)
			return

		item = target.item
		weapon = itemcheck(item, ITEM_WEAPON)
		shield = itemcheck(item, ITEM_SHIELD)
		armor = itemcheck(item, ITEM_ARMOR)

		if weapon or armor or shield:
			# Item in full repair
			if item.maxhealth <= 0 or item.health >= item.maxhealth:
				player.socket.clilocmessage(500423)
				return

			skill = player.skill[BLACKSMITHING]
			if skill >= 900:
				weaken = 1
			elif skill >= 700:
				weaken = 2
			else:
				weaken = 3

			action = self.findcraftitem(item.baseid)

			# We can't craft it, so we can't repair it.
			if not action:
				player.socket.clilocmessage(1044277)
				return

			# We will either destroy or repair it from here on
			# So we can play the craft effect.
			player.soundeffect(0x2a)

			if item.maxhealth <= weaken:
				player.socket.clilocmessage(500424)
				item.delete()
			elif player.checkskill(BLACKSMITHING, 0, 1000):
				player.socket.clilocmessage(1044279)
				item.maxhealth -= weaken
				item.health = item.maxhealth
				item.resendtooltip()
			else:
				player.socket.clilocmessage(1044280)
				item.maxhealth -= weaken
				item.health = max(0, item.health - weaken)
				item.resendtooltip()

			# Warn the user if we'll break the item next time
			if item.maxhealth <= weaken:
				player.socket.clilocmessage(1044278)

			return

		player.socket.clilocmessage(1044277)

	#
	# Smelt an item and create new ingots from it.
	#
	def smelt(self, player, arguments, target):
		if not checkanvilandforge(player):
			player.socket.clilocmessage(1042678)
			return

		tool = wolfpack.finditem(arguments[0])

		if not checktool(player, tool):
			return

		if tool == target.item:
			player.socket.clilocmessage(1044271)
			return

		# Smelt a weapon
		item = target.item
		weapon = itemcheck(item, ITEM_WEAPON)
		shield = itemcheck(item, ITEM_SHIELD)
		armor = itemcheck(item, ITEM_ARMOR)

		# See if it's in our ore list.
		if weapon or shield or armor:
			if item.container != player.getbackpack():
				player.socket.clilocmessage(1044274)
				return

			if item.hastag('resname'):
				resname = str(item.gettag('resname'))
			else:
				resname = None

			for metal in METALS:
				if metal[5] == resname:
					# Try to find out how many ingots this item would need
					menu = findmenu('BLACKSMITHING')
					action = menu.findcraftitem(item.baseid)
					returned = 0.25 + min(1000, player.skill[MINING]) / 2000

					if action and action.submaterial1 > 0:
						amount = int(math.floor(action.submaterial1 * returned))
					else:
						amount = 1

					if amount > 0:
						# Randomly select one of the resources required by that metal
						item.delete()
						ingots = wolfpack.additem(random.choice(metal[3]))
						ingots.amount = amount
						if not tobackpack(ingots, player):
							ingots.update()

						player.soundeffect(0x2a)
						player.soundeffect(0x240)

						player.socket.clilocmessage(1044270)
						self.send(player, arguments)
						return

		player.socket.clilocmessage(1044272)
		self.send(player, arguments)

	#
	# Get the material used by the character from the tags
	#
	def getsubmaterial1used(self, player, arguments):
		if not player.hastag('blacksmithing_ore'):
			return False
		else:
			material = int(player.gettag('blacksmithing_ore'))
			if material < len(self.submaterials1):
				return material
			else:
				return False

	#
	# Get the material used by the character from the tags
	#
	def getsubmaterial2used(self, player, arguments):
		if not player.hastag('blacksmithing_scales'):
			return False
		else:
			material = int(player.gettag('blacksmithing_scales'))
			if material < len(self.submaterials2):
				return material
			else:
				return False

	#
	# Save the material preferred by the user in a tag
	#
	def setsubmaterial2used(self, player, arguments):
		player.settag('blacksmithing_scales', material)

	#
	# Save the material preferred by the user in a tag
	#
	def setsubmaterial1used(self, player, arguments, material):
		player.settag('blacksmithing_ore', material)

	#
	# Save the material preferred by the user in a tag
	#
	def setsubmaterial2used(self, player, arguments, material):
		player.settag('blacksmithing_scales', material)

#
# Load a menu with a given id and
# append it to the parents submenus.
#
def loadMenu(id, parent = None):
	definition = wolfpack.getdefinition(WPDT_MENU, id)
	if not definition:
		if parent:
			console.log(LOG_ERROR, "Unknown submenu %s in menu %s.\n" % (id, parent.id))
		else:
			console.log(LOG_ERROR, "Unknown menu: %s.\n" % id)
		return

	name = definition.getattribute('name', '')
	menu = BlacksmithingMenu(id, parent, name)

	# See if we have any submenus
	for i in range(0, definition.childcount):
		child = definition.getchild(i)
		# Submenu
		if child.name == 'menu':
			if not child.hasattribute('id'):
				console.log(LOG_ERROR, "Submenu with missing id attribute in menu %s.\n" % menu.id)
			else:
				loadMenu(child.getattribute('id'), menu)

		# Craft an item
		elif child.name == 'smith':
			if not child.hasattribute('definition') or not child.hasattribute('name'):
				console.log(LOG_ERROR, "Smith action without definition or name in menu %s.\n" % menu.id)
			else:
				itemdef = child.getattribute('definition')
				name = child.getattribute('name')
				try:
					# See if we can find an item id if it's not given
					if not child.hasattribute('itemid'):
						item = wolfpack.getdefinition(WPDT_ITEM, itemdef)
						itemid = 0
						if item:
							itemchild = item.findchild('id')
							if itemchild:
								itemid = itemchild.value
					else:
						itemid = hex2dec(child.getattribute('itemid', '0'))
					action = SmithItemAction(menu, name, int(itemid), itemdef)
				except:
					console.log(LOG_ERROR, "Smith action with invalid item id in menu %s.\n" % menu.id)

				# Process subitems
				for j in range(0, child.childcount):
					subchild = child.getchild(j)

					# How much of the primary resource should be consumed
					if subchild.name == 'ingots':
						action.submaterial1 = hex2dec(subchild.getattribute('amount', '0'))

					# How much of the secondary resource should be consumed
					elif subchild.name == 'scales':
						action.submaterial2 = hex2dec(subchild.getattribute('amount', '0'))

					# Normal Material
					elif subchild.name == 'material':
						if not subchild.hasattribute('id'):
							console.log(LOG_ERROR, "Material element without id list in menu %s.\n" % menu.id)
							break
						else:
							ids = subchild.getattribute('id').split(';')
							try:
								amount = hex2dec(subchild.getattribute('amount', '1'))
							except:
								console.log(LOG_ERROR, "Material element with invalid id list in menu %s.\n" % menu.id)
								break
							action.materials.append([ids, amount])

					# Skill requirement
					elif subchild.name in skillnamesids:
						skill = skillnamesids[subchild.name]
						try:
							minimum = hex2dec(subchild.getattribute('min', '0'))
						except:
							console.log(LOG_ERROR, "%s element with invalid min value in menu %s.\n" % (subchild.name, menu.id))

						try:
							maximum = hex2dec(subchild.getattribute('max', '1200'))
						except:
							console.log(LOG_ERROR, "%s element with invalid max value in menu %s.\n" % (subchild.name, menu.id))

						try:
							penalty = hex2dec(subchild.getattribute('penalty','0'))
						except:
							console.log(LOG_ERROR, "%s element with invalid max value in menu %s.\n" % (subchild.name, menu.id))

						action.skills[skill] = [minimum, maximum, penalty]

	# Sort the menu. This is important for the makehistory to make.
	menu.sort()

#
# Load the blacksmithing menu.
#
def onLoad():
	loadMenu('BLACKSMITHING')
