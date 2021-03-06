#===============================================================#
#   )      (\_     | WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  | Created by: Radiant                        #
#   )).-' {{ ;'`   | Revised by:                                #
#  ( (  ;._ \\ ctr | Last Modification: Created                 #
#===============================================================#
# Necromancy basic stuff                                        #
#===============================================================#

# http://uo.stratics.com/content/guides/necromancy1.shtml

from magic.spell import CharEffectSpell, Spell, DelayedDamageSpell
from wolfpack.consts import *
from magic.utilities import *

class CurseWeapon(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_PIGIRON: 1}
		self.mantra = 'An Sanct Gra Char'

class PainSpike(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_PIGIRON: 1, REAGENT_GRAVEDUST: 1}
		self.mantra = 'In Sar'

class CorpseSkin(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_GRAVEDUST: 1, REAGENT_BATWING: 1}
		self.mantra = 'In Agle Corp Ylem'

class EvilOmen(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_BATWING: 1, REAGENT_NOXCRYSTAL: 1}
		self.mantra = 'Pas Tym An Sanct'

class BloodOath(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_DAEMONBLOOD: 1}
		self.mantra = 'In Jux Mani Xen'

class WraithForm(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_NOXCRYSTAL: 1, REAGENT_PIGIRON: 1}
		self.mantra = 'Rel Xen Um'

class MindRot(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_BATWING: 1, REAGENT_DAEMONBLOOD: 1, REAGENT_PIGIRON: 1}
		self.mantra = 'Wis An Ben'

class HorrificBeast(Spell):
	def __init__(self):
		Spell.__init__(self, 0)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_BATWING: 1, REAGENT_DAEMONBLOOD: 1}
		self.mantra = 'Rel Xen Vas Bal'

class AnimateDead(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_GRAVEDUST: 1, REAGENT_DAEMONBLOOD: 1}
		self.mantra = 'Uus Corp'

	def target(self, char, mode, targettype, target, args, item):
		char.turnto(target)
		if target.id != 0x2006:
			char.socket.clilocmessage(1042600)
			return

		if char.player and char.controlslots >= 5:
			char.socket.clilocmessage(1049645)
			return

		if target.owner:
			char.socket.sysmessage('You may not use this on players!')
			return

		if not self.consumerequirements(char, mode, args, target, item):
			return

		char.socket.sysmessage('Not yet implented')

class PoisonStrike(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_NOXCRYSTAL: 1}
		self.mantra = 'In Vas Nox'

class Strangle(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_NOXCRYSTAL: 1, REAGENT_DAEMONBLOOD: 1}
		self.mantra = 'In Bal Nox'

class SummonFamiliar(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_BATWING: 1, REAGENT_GRAVEDUST: 1, REAGENT_DAEMONBLOOD: 1}
		self.mantra = 'Kal Xen Bal'

class Wither(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_GRAVEDUST: 1, REAGENT_NOXCRYSTAL: 1, REAGENT_PIGIRON: 1}
		self.mantra = 'Kal Vas An Flam'

class LichForm(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_GRAVEDUST: 1, REAGENT_DAEMONBLOOD: 1, REAGENT_NOXCRYSTAL: 1}
		self.mantra = 'Rel Xen Corp Ort'

class VengefulSpirit(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_BATWING: 1, REAGENT_GRAVEDUST: 1, REAGENT_PIGIRON: 1}
		self.mantra = 'Kal Xen Bal Beh'

class VampiricEmbrace(Spell):
	def __init__(self):
		Spell.__init__(self, 1)
		self.skill = NECROMANCY
		self.damageskill = SPIRITSPEAK
		self.mana = 0
		self.reagents = {REAGENT_BATWING: 1, REAGENT_NOXCRYSTAL: 1, REAGENT_PIGIRON: 1}
		self.mantra = 'Rel Xen An Sanct'

def onLoad():
	CurseWeapon().register(201)
	PainSpike().register(202)
	CorpseSkin().register(203)
	EvilOmen().register(204)
	BloodOath().register(205)
	WraithForm().register(206)
	MindRot().register(207)
	HorrificBeast().register(208)
	AnimateDead().register(209)
	PoisonStrike().register(210)
	Strangle().register(211)
	SummonFamiliar().register(212)
	Wither().register(213)
	LichForm().register(214)
	VengefulSpirit().register(215)
	VampiricEmbrace().register(216)
