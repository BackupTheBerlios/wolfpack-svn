#===============================================================#
#   )      (\_     | WOLFPACK 13.0.0 Scripts                    #
#  ((    _/{  "-;  | Created by: DarkStorm                      #
#   )).-' {{ ;'`   | Revised by:                                #
#  ( (  ;._ \\ ctr | Last Modification: Created                 #
#===============================================================#
# Constants used in Python scripts                              #
#===============================================================#

# Which port should the remote admin run on
REMOTEADMIN_PORT = 2594

# Spell Settings
CLUMSY_DURATION = 60000

# Potions
POTION_LESSERHEAL_RANGE = [ 1, 16 ]
POTION_HEAL_RANGE = [ 1, 16 ]
POTION_GREATERHEAL_RANGE = [ 1, 16 ]

# Fishing Settings
FISHING_MIN_FISH = 25	 # Minimum fish in a 8x8 block
FISHING_MAX_FISH = 30	 # Maximum fish in a 8x8 block
FISHING_REFILLTIME = 60 * 5 # 5 Minute refresh time
FISHING_MAX_DISTANCE = 6 # Maximum Distance to fishing ground
FISHING_BLOCK_RANGE = 13 # How many Z-Units of free space need to be above the fishing ground

# Lumberjacking Settings
LUMBERJACKING_MIN_LOGS = 10
LUMBERJACKING_MAX_LOGS = 15
LUMBERJACKING_MIN_SKILL = 0
LUMBERJACKING_MAX_SKILL = 600
LUMBERJACKING_REFILLTIME = 60 * 5 # 5 Minutes Refresh time for trees

# Skill Constants
ALCHEMY = 0
ANATOMY = 1
ANIMALLORE = 2
ITEMID = 3
ARMSLORE = 4
PARRYING = 5
BEGGING = 6
BLACKSMITHING = 7
BOWCRAFT = 8
PEACEMAKING = 9
CAMPING = 10
CARPENTRY = 11
CARTOGRAPHY = 12
COOKING = 13
DETECTINGHIDDEN = 14
ENTICEMENT = 15
EVALUATINGINTEL = 16
HEALING = 17
FISHING = 18
FORENSICS = 19
HERDING = 20
HIDING = 21
PROVOCATION = 22
INSCRIPTION = 23
LOCKPICKING = 24
MAGERY = 25
MAGICRESISTANCE = 26
TACTICS = 27
SNOOPING = 28
MUSICIANSHIP = 29
POISONING = 30
ARCHERY = 31
SPIRITSPEAK = 32
STEALING = 33
TAILORING = 34
TAMING = 35
TASTEID = 36
TINKERING = 37
TRACKING = 38
VETERINARY = 39
SWORDSMANSHIP = 40
MACEFIGHTING = 41
FENCING = 42
WRESTLING = 43
LUMBERJACKING = 44
MINING = 45
MEDITATION = 46
STEALTH = 47
REMOVETRAPS = 48

# Constants for char.sound
SND_STARTATTACK = 0
SND_IDLE = 1
SND_ATTACK = 2
SND_DEFEND = 3
SND_DIE = 4

# Layer
LAYER_RIGHTHAND = 1
LAYER_LEFTHAND = 2
LAYER_SHOES = 3
LAYER_PANTS = 4
LAYER_SHIRT = 5
LAYER_HELM = 6
LAYER_GLOVES = 7
LAYER_RING = 8
LAYER_UNUSED1 = 9
LAYER_NECK = 10
LAYER_HAIR = 11
LAYER_WAIST = 12
LAYER_CHEST = 13
LAYER_BRACELET = 14
LAYER_UNUSED2 = 15
LAYER_BEARD = 16
LAYER_TORSO = 17
LAYER_EARRINGS = 18
LAYER_ARMS = 19
LAYER_CLOAK = 20
LAYER_BACKPACK = 21
LAYER_ROBE = 22
LAYER_SKIRT = 23
LAYER_LEGS = 24
LAYER_MOUNT = 25
LAYER_NPCRESTOCK = 26
LAYER_NPCNORESTOCK = 27
LAYER_NPCSELL = 28
LAYER_BANKBOX = 29
LAYER_DRAGGING = 30
LAYER_TRADING = 31

# Several Hook Constants
HOOK_OBJECT = 1
HOOK_ITEM = 2
HOOK_CHAR = 3

# Constants for the Events we can hook
EVENT_USE					= 0x00000001
EVENT_SINGLECLICK			= 0x00000002
EVENT_COLLIDEITEM			= 0x00000004
EVENT_COLLIDECHAR			= 0x00000008
EVENT_WALK					= 0x00000010
EVENT_CREATE				= 0x00000020
EVENT_LOGIN					= 0x00000040
EVENT_LOGOUT				= 0x00000080
EVENT_TALK					= 0x00000100
EVENT_WARMODETOGGLE			= 0x00000200
EVENT_HELP					= 0x00000400
EVENT_CHAT					= 0x00000800
EVENT_SKILLUSE				= 0x00001000
EVENT_SELECTCONTEXTMENU		= 0x00002000
EVENT_REQUESTCONTEXTMENU	= 0x00004000
EVENT_DROPONCHAR			= 0x00008000
EVENT_DROPONITEM			= 0x00010000
EVENT_DROPONGROUND			= 0x00020000
EVENT_PICKUP				= 0x00040000
EVENT_SPEECH				= 0x00080000
EVENT_BEGINCAST				= 0x00100000
EVENT_ENDCAST				= 0x00200000
EVENT_SPELLCHECKTARGET		= 0x00400000
EVENT_SPELLSUCCESS			= 0x00800000
EVENT_SPELLFAILURE			= 0x01000000

# Effect Constants
EFFECT_MOVING        = 0
EFFECT_LIGHTNING     = 1
EFFECT_STAYSOURCEPOS = 2
EFFECT_STAYSOURCESER = 3
