
from wolfpack.consts import *
from math import floor
from combat.weaponinfo import WEAPONINFO, WEAPON_RESNAME_BONI
from combat.armorinfo import ARMORINFO, ARMOR_RESNAME_BONI

#
# Get the delay for the next swing from this attacker and his weapon.
# The return value is in miliseconds.
#
def getdelay(attacker, weapon):
  speed = fromitem(weapon, SPEED)
  value = max(1, (attacker.stamina + 100) * speed)

  # Scale value according to bonus
  # value += bonus * value / 100

  return floor(40000.0 / value) * 500

#
# Known item properties and the tags they are stored in.
# The default value is saved here too (Wrestling values).
# The third value specifies whether the value is cumulative
# and can be retrieved from characters.
#
PROPERTIES = {
  # Resistances
  RESISTANCE_PHYSICAL: ['res_physical', 0, 1],
  RESISTANCE_FIRE: ['res_fire', 0, 1],
  RESISTANCE_COLD: ['res_cold', 0, 1],
  RESISTANCE_POISON: ['res_poison', 0, 1],
  RESISTANCE_ENERGY: ['res_energy', 0, 1],

  # Damage Distribution (only weapons)
  DAMAGE_PHYSICAL: ['dmg_physical', 100, 0],
  DAMAGE_FIRE: ['dmg_fire', 0, 0],
  DAMAGE_COLD: ['dmg_cold', 0, 0],
  DAMAGE_POISON: ['dmg_poison', 0, 0],
  DAMAGE_ENERGY: ['dmg_energy', 0, 0],

  # % Boni
  DAMAGEBONUS: ['aos_boni_damage', 0, 1],
  SPEEDBONUS: ['aos_boni_speed', 0, 1],
  HITBONUS: ['aos_boni_hit_chance', 0, 1],
  DEFENSEBONUS: ['aos_defense_chance', 0, 1],

  # Regular Combat Properties
  MINDAMAGE: ['mindamage', 1, 0],
  MAXDAMAGE: ['maxdamage', 4, 0],
  SPEED: ['speed', 50, 0],
  HITSOUND: ['hitsound', [0x135, 0x137, 0x13b], 0],
  MISSSOUND: ['misssound', [0x238, 0x23a], 0],
  AMMUNITION: ['ammunition', '', 0],
  PROJECTILE: ['projectile', 0, 0],
  PROJECTILEHUE: ['projectilehue', 0, 0],
}

#
# Get a certain property for an item.
#
def fromitem(item, property):
  if not PROPERTIES.has_key(property):
    raise Exception, "Unknown property value %u" % property

  info = PROPERTIES[property]

  # Fists are no items.
  if not item:
    return info[1]

  # Tag overrides properties
  if item.hastag(info[0]):
    # HITSOUND and MISSSOUND need special treatment beacuse of
    # the list character.
    if property == HITSOUND or property == MISSSOUND:
      return str(item.gettag(info[0])).split(',')
    else:
      return int(item.gettag(info[0]))
  else:
    # See if our weapon info has anything for the
    # requested item. Otherwise return the default value.
    if itemcheck(item, ITEM_WEAPON):
      if WEAPONINFO.has_key(item.baseid):
        weaponinfo = WEAPONINFO[item.baseid]

        if weaponinfo.has_key(property):
          return weaponinfo[property]
    elif itemcheck(item, ITEM_ARMOR) or itemcheck(item, ITEM_SHIELD):
      if ARMORINFO.has_key(item.baseid):
        armorinfo = ARMORINFO[item.baseid]

        if armorinfo.has_key(property):
          return armorinfo[property]

    return info[1]

#
# Calculates a certain property for the character by
# recursing trough all of his equipment.
#
def fromchar(char, property):
  # Check for unknown property
  if not PROPERTIES.has_key(property):
    raise Exception, "Unknown property value %u" % property

  info = PROPERTIES[property]

  value = 0 # Default

  # See if the character is a npc and has the value
  # Then it overrides any items
  if char.hastag(info[0]):
    return int(char.gettag(info[0]))

  for layer in range(LAYER_RIGHTHAND, LAYER_MOUNT):
    item = char.itemonlayer(layer)

    if item:
      value += fromitem(item, property)

  return value

#
# Computes the damage a certain character
# deals.
#
def getdamage(char):
  weapon = char.getweapon()

  # See if it's a npc with special min and maxdamage settings
  if char.npc:
    # If the npc has mindamage and maxdamage tags, they
    # override all other settings
    if char.hastag('mindamage') and char.hastag('maxdamage'):
      mindamage = int(char.gettag('mindamage'))
      maxdamage = int(char.gettag('maxdamage'))
      return (mindamage, maxdamage)

    # Special treatment for fists.
    if not weapon:
      mindamage = char.strength / 28
      maxdamage = mindamage + 7
    else:
      mindamage = fromitem(weapon, MINDAMAGE)
      maxdamage = fromitem(weapon, MAXDAMAGE)
  else:
    mindamage = fromitem(weapon, MINDAMAGE)
    maxdamage = fromitem(weapon, MAXDAMAGE)

  return (mindamage, maxdamage)

#
# See if the given item has a specific property
# This is used more as a type check.
# To see if an item is an armor, a bashing weapon, a ranged weapon. etc.
#
def itemcheck(item, check):
  if not item:
    return 0

  # No real check for this yet
  if check == ITEM_ARMOR:
    return item.type == 1009

  # Only type check yet.
  if check == ITEM_SHIELD:
    return item.type == 1008

  # Only type check yet.
  if check == ITEM_WEAPON:
    return item.type >= 1001 and item.type <= 1007

  # Only type check yet.
  if check == ITEM_MELEE:
    return item.type >= 1001 and item.type <= 1005

  # Only type check yet.
  if check == ITEM_RANGED:
    return item.type == 1006 or item.type == 1007

  return 0
