
import wolfpack.time

def onRegenMana(char, points):
	bonus = 13
	return bonus

def dispel(char, args, source, dispelargs):
	char.removescript('magic.lichform')

def expire(char, args):
	char.hitpoints -= 1
	char.updatehealth()
	char.addtimer( 2500, expire, [], True, False, 'LICHFORM', dispel )
