
#
# Skill constants directly taken from Wolfpack
#
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
NECROMANCY = 49
FOCUS = 50
CHIVALRY = 51

ALLSKILLS = 52 # skills + 1

SKILLNAMES = \
{
	'ALCHEMY' : ALCHEMY,
	'ANATOMY' : ANATOMY,
	'ANIMALLORE' : ANIMALLORE,
	'ITEMID' : ITEMID,
	'ARMSLORE' : ARMSLORE,
	'PARRYING' : PARRYING,
	'BEGGING' : BEGGING,
	'BLACKSMITHING' : BLACKSMITHING,
	'BOWCRAFT' : BOWCRAFT,
	'PEACEMAKING' : PEACEMAKING,
	'CAMPING' : CAMPING,
	'CARPENTRY' : CARPENTRY,
	'CARTOGRAPHY' : CARTOGRAPHY,
	'COOKING' : COOKING,
	'DETECTINGHIDDEN' : DETECTINGHIDDEN,
	'ENTICEMENT' : ENTICEMENT,
	'EVALUATINGINTEL' : EVALUATINGINTEL,
	'HEALING' : HEALING,
	'FISHING' : FISHING,
	'FORENSICS' : FORENSICS,
	'HERDING' : HERDING,
	'HIDING' : HIDING,
	'PROVOCATION' : PROVOCATION,
	'INSCRIPTION' : INSCRIPTION,
	'LOCKPICKING' : LOCKPICKING,
	'MAGERY' : MAGERY,
	'MAGICRESISTANCE' : MAGICRESISTANCE,
	'TACTICS' : TACTICS,
	'SNOOPING' : SNOOPING,
	'MUSICIANSHIP' : MUSICIANSHIP,
	'POISONING' : POISONING,
	'ARCHERY' : ARCHERY,
	'SPIRITSPEAK' : SPIRITSPEAK,
	'STEALING' : STEALING,
	'TAILORING' : TAILORING,
	'TAMING' : TAMING,
	'TASTEID' : TASTEID,
	'TINKERING' : TINKERING,
	'TRACKING' : TRACKING,
	'VETERINARY' : VETERINARY,
	'SWORDSMANSHIP' : SWORDSMANSHIP,
	'MACEFIGHTING' : MACEFIGHTING,
	'FENCING' : FENCING,
	'WRESTLING' : WRESTLING,
	'LUMBERJACKING' : LUMBERJACKING,
	'MINING' : MINING,
	'MEDITATION' : MEDITATION,
	'STEALTH' : STEALTH,
	'REMOVETRAPS' : REMOVETRAPS,
}

#
# Type constants
#
T_NORMAL = 0
T_CONTAINER = 1 # ANY UNLOCKED CONTAINER OR CORPSE. CCONTAINER BASED
T_CONTAINER_LOCKED = 2
T_DOOR = 3 # 3 = DOOR CAN BE OPENED
T_DOOR_LOCKED = 4 # A LOCKED DOOR.
T_KEY = 5
T_LIGHT_LIT = 6 # LOCAL LIGHT GIVING OBJECT
T_LIGHT_OUT = 7 # CAN BE LIT.
T_FOOD = 8 # EDIBLE FOOD. (POISONED FOOD ?)
T_FOOD_RAW = 9 # MUST COOK RAW FOOD UNLESS YOUR AN ANIMAL.
T_ARMOR = 10 # SOME TYPE OF ARMOR. (NO REAL ACTION)
T_WEAPON_MACE_SMITH = 11 # CAN BE USED FOR SMITHING
T_WEAPON_MACE_SHARP = 12 # WAR AXE CAN BE USED TO CUT/CHOP TREES.
T_WEAPON_SWORD = 13 #
T_WEAPON_FENCE = 14 # CAN'T BE USED TO CHOP TREES. (MAKE KINDLING)
T_WEAPON_BOW = 15 # BOW OR XBOW
T_WAND = 16 # A MAGIC STORAGE ITEM
T_TELEPAD = 17 # WALK ON TELEPORT
T_SWITCH = 18 # THIS IS A SWITCH WHICH EFFECTS SOME OTHER OBJECT IN THE WORLD.
T_BOOK = 19 # READ THIS BOOK. (STATIC OR DYNAMIC TEXT)
T_RUNE = 20 # CAN BE MARKED AND RENAMED AS A RECALL RUNE.
T_BOOZE = 21 # BOOZE = (DRUNK EFFECT)
T_POTION = 22 # SOME MAGIC EFFECT.
T_FIRE = 23 # IT WILL BURN YOU.
T_CLOCK = 24 # OR A WRISTWATCH
T_TRAP = 25 # WALK ON TRAP.
T_TRAP_ACTIVE = 26	# = SOME ANIMATION
T_MUSICAL = 27	# = A MUSICAL INSTRUMENT.
T_SPELL = 28	# = MAGIC SPELL EFFECT.
T_GEM = 29	# = NO USE YET
T_WATER = 30	# = THIS IS WATER (FISHABLE) (NOT A GLASS OF WATER)
T_CLOTHING = 31	# = ALL CLOTH BASED WEARABLE STUFF
T_SCROLL = 32	# = MAGIC SCROLL
T_CARPENTRY = 33	# = TOOL OF SOME SORT.
T_SPAWN_CHAR = 34	# = SPAWN OBJECT. SHOULD BE INVIS ALSO.
T_GAME_PIECE = 35	# = CAN'T BE REMOVED FROM GAME.
T_PORTCULIS = 36	# = Z DELTA MOVING GATE. (OPEN)
T_FIGURINE = 37	# = MAGIC FIGURE THAT TURNS INTO A CREATURE WHEN ACTIVATED.
T_SHRINE = 38	# = CAN RES YOU
T_MOONGATE = 39	# = LINKED TO OTHER MOON GATES (HARD CODED LOCATIONS)
T_CHAIR = 40	# = ANY SORT OF A CHAIR ITEM. WE CAN SIT ON.
T_FORGE = 41	# = USED TO SMELT ORE BLACKSMITHY ETC.
T_ORE = 42	# = SMELT TO INGOTS.
T_LOG = 43	# = MAKE INTO FURNITURE ETC. LUMBERLOGS
T_TREE = 44	# = CAN BE CHOPPED.
T_ROCK = 45	# = CAN BE MINED FOR ORE.
T_CARPENTRY_CHOP = 46	# = TOOL OF SOME SORT.
T_MULTI = 47	# = MULTI PART OBJECT LIKE HOUSE OR SHIP.
T_REAGENT = 48	# = ALCHEMY WHEN CLICKED ?
T_SHIP = 49	# = THIS IS A SHIP MULTI
T_SHIP_PLANK = 50 #
T_SHIP_SIDE = 51	# = SHOULD EXTEND TO MAKE A PLANK
T_SHIP_SIDE_LOCKED = 52 #
T_SHIP_TILLER = 53	# = TILLER MAN ON THE SHIP.
T_EQ_TRADE_WINDOW = 54	# = CONTAINER FOR THE TRADE WINDOW.
T_FISH = 55	# = FISH CAN BE CUT UP.
T_SIGN_GUMP = 56	# = THINGS LIKE GRAVE STONES AND SIGN PLAQUES
T_STONE_GUILD = 57	# = GUILD STONES
T_ANIM_ACTIVE = 58	# = ACTIVE ANIUM THAT WILL RECYCLE WHEN DONE.
T_ADVANCE_GATE = 59	# = ADVANCE GATE. M_ADVANCEGATETYPE = CREID_TYPE
T_CLOTH = 60	# = BOLT OR FOLDED CLOTH
T_HAIR = 61 #
T_BEARD = 62	# = JUST FOR GROUPING PURPOSES.
T_INGOT = 63	# = INGOT.
T_COIN = 64	# = COIN OF SOME SORT. GOLD OR OTHERWISE.
T_CROPS = 65	# = A PLANT THAT WILL REGROW. PICKED TYPE.
T_DRINK = 66	# = SOME SORT OF DRINK (NON BOOZE)
T_ANVIL = 67	# = FOR REPAIR.
T_PORT_LOCKED = 68	# = THIS PORTCULLIS MUST BE TRIGGERED.
T_SPAWN_ITEM = 69	# = SPAWN OTHER ITEMS.
T_TELESCOPE = 70	# = BIG TELESCOPE PIC.
T_BED = 71	# = BED.
T_GOLD = 72	# = GOLD COIN
T_MAP = 73	# = MAP OBJECT WITH PINS.
T_EQ_MEMORY_OBJ = 74	# = A CHAR HAS A MEMORY LINK TO SOME OBJECT. (I AM FIGHTING WITH SOMEONE. THIS RECORDS THE FIGHT.)
T_WEAPON_MACE_STAFF = 75	# = STAFF TYPE OF MACE. OR JUST OTHER TYPE OF MACE.
T_EQ_HORSE = 76	# = EQUIPPED HORSE OBJECT REPRESENTS A RIDING HORSE TO THE CLIENT.
T_COMM_CRYSTAL = 77	# = COMMUNICATION CRYSTAL.
T_GAME_BOARD = 78	# = THIS IS A CONTAINER OF PIECES.
T_TRASH_CAN = 79	# = DELETE ANY OBJECT DROPPED ON IT.
T_CANNON_MUZZLE = 80	# = CANNON MUZZLE. NOT THE OTHER CANNON PARTS.
T_CANNON = 81	# = THE REST OF THE CANNON.
T_CANNON_BALL = 82 #
T_ARMOR_LEATHER = 83	# = NON METALLIC ARMOR (T_CLOTHING)
T_SEED = 84	# = FRUIT TO SEED
T_JUNK = 85	# = NEVER USED
T_CRYSTAL_BALL = 86 #
T_MESSAGE = 88	# = USER WRITTEN MESSAGE ITEM. (FOR BBOARD USSUALLY)
T_REAGENT_RAW = 89	# = FRESHLY GROWN REAGENTS...NOT PROCESSED YET.
T_EQ_CLIENT_LINGER = 90	# = CHANGE PLAYER TO NPC FOR A WHILE.
T_DREAM_GATE = 91	# = PUSH YOU TO ANOTHER SERVER. (NO ITEMS TRANSFERED CLIENT INSTA LOGGED OUT)
T_IT_STONE = 92	# = DOUBLE CLICK FOR ITEMS
T_METRONOME = 93	# = TICKS ONCE EVERY N SECS.
T_EXPLOSION = 94	# = ASYNC EXPLOSION.
T_EQ_NPC_SCRIPT = 95	# = SCRIPT NPC ACTIONS IN THE FORM OF A BOOK. (GET RID OF THIS IN FAVOR OF WAITING ON M_EVENTS)
T_WEB = 96	# = WALK ON THIS AND TRANSFORM INTO SOME OTHER OBJECT.
T_GRASS = 97	# = CAN BE EATEN BY GRAZING ANIMALS
T_AROCK = 98	# = A ROCK OR BOULDER. CAN BE THROWN BY GIANTS.
T_TRACKER = 99	# = POINTS TO A LINKED OBJECT.
T_SOUND = 100 # = THIS IS A SOUND SOURCE.
T_STONE_TOWN = 101 # = TOWN STONES. EVERYONE FREE TO JOIN.
T_WEAPON_MACE_CROOK = 102 #
T_WEAPON_MACE_PICK = 103 #
T_LEATHER = 104 # = LEATHER OR SKINS OF SOME SORT.(NOT WEARABLE)
T_SHIP_OTHER = 105 # = SOME OTHER PART OF A SHIP.
T_BBOARD = 106 # = A CONTAINER AND BBOARD OBJECT.
T_SPELLBOOK = 107 # = SPELLBOOK (WITH SPELLS)
T_CORPSE = 108 # = SPECIAL TYPE OF ITEM.
T_TRACK_ITEM = 109 # - TRACK A ID OR TYPE OF ITEM.
T_TRACK_CHAR = 110 # = TRACK A CHAR OR RANGE OF CHAR ID'S
T_WEAPON_ARROW = 111 #
T_WEAPON_BOLT = 112 #
T_EQ_VENDOR_BOX = 113 # = AN EQUIPPED VENDOR .
T_EQ_BANK_BOX = 114 # = AN EQUIPPED BANK BOX
T_DEED = 115 #
T_LOOM = 116 #
T_BEE_HIVE = 117 #
T_ARCHERY_BUTTE = 118 #
T_EQ_MURDER_COUNT = 119 # = MY MURDER COUNT FLAG.
T_EQ_STUCK = 120 # WE ARE STUCK IN A WEB
T_TRAP_INACTIVE = 121 #	= A SAFE TRAP.
T_STONE_ROOM = 122 #	= HOUSE TYPE STONE. (FOR MAPPED HOUSE REGIONS)
T_BANDAGE = 123 #	= CAN BE USED FOR HEALING.
T_CAMPFIRE = 124 #	= THIS IS A FIRE BUT A SMALL ONE.
T_MAP_BLANK = 125
T_SPY_GLASS = 126
T_SEXTANT = 127
T_SCROLL_BLANK = 128
T_FRUIT = 129
T_WATER_WASH = 130 # WATER THAT WILL NOT CONTAIN FISH. (FOR WASHING OR DRINKING)
T_WEAPON_AXE = 131 # NOT THE SAME AS A SWORD. BUT USES SWORDSMANSHIP SKILL
T_WEAPON_XBOW = 132
T_SPELLICON = 133
T_DOOR_OPEN = 134
T_MEAT_RAW = 135 # JUST A MEATY PART OF A CORPSE. (UNCOOKED MEAT)
T_GARBAGE = 136
T_KEYRING = 137
T_TABLE = 138 # DOESN'T REALLY DO ANYTHING.
T_FLOOR = 139
T_ROOF = 140
T_FEATHER = 141 # A BIRDS FEATHER
T_WOOL = 142 # WOOL CUT FRM A SHEEP.
T_FUR = 143
T_BLOOD = 144 # BLOOD OF SOME CREATURE
T_FOLIAGE = 145 # DOES NOT GO INVIS WHEN REAPED. BUT WILL IF EATEN
T_GRAIN = 146
T_SCISSORS = 147
T_THREAD = 148
T_YARN = 149
T_SPINWHEEL = 150
T_BANDAGE_BLOOD = 151 #	= CAN'T BE USED FOR HEALING.
T_FISH_POLE = 152
T_SHAFT = 153 # BOLT OR ARROW.
T_LOCKPICK = 154
T_KINDLING = 155
T_TRAIN_DUMMY = 156
T_TRAIN_PICKPOCKET = 157
T_BEDROLL = 158
T_BELLOWS = 159
T_HIDE = 160 # MADE INTO LEATHER.
T_CLOTH_BOLT = 161
T_BOARD = 162 # = LOGS ARE PLAINED INTO DECENT LUMBER
T_PITCHER = 163
T_PITCHER_EMPTY = 164
T_DYE_VAT = 165
T_DYE = 166
T_POTION_EMPTY = 167 # EMPTY BOTTLE.
T_MORTAR = 168
T_HAIR_DYE = 169
T_SEWING_KIT = 170
T_TINKER_TOOLS = 171
T_WALL = 172 # WALL OF A STRUCTURE.
T_WINDOW = 173 # WINDOW FOR A STRUCTURE.
T_COTTON = 174 # COTTON FROM THE PLANT
T_BONE = 175
T_EQ_SCRIPT = 176 # PURE SCRIPTABLE ITEM EQUIPPED.
T_SHIP_HOLD = 177 # SHIPS HOLD.
T_SHIP_HOLD_LOCK = 178
T_LAVA = 179
T_SHIELD = 180 # EQUIPPABLE ARMOR.
T_JEWELRY = 181
T_DIRT = 182 # A PATCH OF DIRT WHERE I CAN PLANT SOMETHING
T_SCRIPT = 183

#
# Color values for memory items
#
MEMORY_SAWCRIME = 0x0001
MEMORY_IPET = 0x0002
MEMORY_FIGHT = 0x0004
MEMORY_IAGGRESSOR = 0x0008
MEMORY_HARMEDBY = 0x0010
MEMORY_IRRITATEDBY = 0x0020
MEMORY_SPEAK = 0x0040
MEMORY_AGGREIVED = 0x0080
MEMORY_GUARD = 0x0100
MEMORY_ISPAWNED = 0x0200
MEMORY_GUILD = 0x0400
MEMORY_TOWN = 0x0800
MEMORY_FOLLOW = 0x1000
MEMORY_WAR_TARG = 0x2000
MEMORY_FRIEND = 0x4000

#
# Item Attributes
#
ATTR_IDENTIFIED	 = 0x0001
ATTR_DECAY		 = 0x0002
ATTR_NEWBIE		 = 0x0004
ATTR_MOVE_ALWAYS = 0x0008
ATTR_MOVE_NEVER	 = 0x0010
ATTR_MAGIC		 = 0x0020
ATTR_OWNED		 = 0x0040
ATTR_INVIS		 = 0x0080
ATTR_CURSED		 = 0x0100
ATTR_CURSED2	 = 0x0200
ATTR_BLESSED	 = 0x0400
ATTR_BLESSED2	 = 0x0800
ATTR_FORSALE	 = 0x1000
ATTR_STOLEN		 = 0x2000
ATTR_CAN_DECAY = 0x4000
ATTR_STATIC		 = 0x8000
