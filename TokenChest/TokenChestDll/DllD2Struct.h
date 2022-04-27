#ifndef D2STRUCT_H
#define D2STRUCT_H

struct ClientData;
struct QuestInfo;
struct Arena;

enum UnitStats {
	STAT_STRENGTH = 0x0,
	STAT_ENERGY = 0x1,
	STAT_DEXTERITY = 0x2,
	STAT_VITALITY = 0x3,
	STAT_STATPTS = 0x4,
	STAT_NEWSKILLS = 0x5,
	STAT_HITPOINTS = 0x6,
	STAT_MAXHP = 0x7,
	STAT_MANA = 0x8,
	STAT_MAXMANA = 0x9,
	STAT_STAMINA = 0xA,
	STAT_MAXSTAMINA = 0xB,
	STAT_LEVEL = 0xC,
	STAT_EXPERIENCE = 0xD,
	STAT_GOLD = 0xE,
	STAT_GOLDBANK = 0xF,
	STAT_ITEM_ARMOR_PERCENT = 0x10,
	STAT_ITEM_MAXDAMAGE_PERCENT = 0x11,
	STAT_ITEM_MINDAMAGE_PERCENT = 0x12,
	STAT_TOHIT = 0x13,
	STAT_TOBLOCK = 0x14,
	STAT_MINDAMAGE = 0x15,
	STAT_MAXDAMAGE = 0x16,
	STAT_SECONDARY_MINDAMAGE = 0x17,
	STAT_SECONDARY_MAXDAMAGE = 0x18,
	STAT_DAMAGEPERCENT = 0x19,
	STAT_MANARECOVERY = 0x1A,
	STAT_MANARECOVERYBONUS = 0x1B,
	STAT_STAMINARECOVERYBONUS = 0x1C,
	STAT_LASTEXP = 0x1D,
	STAT_NEXTEXP = 0x1E,
	STAT_ARMORCLASS = 0x1F,
	STAT_ARMORCLASS_VS_MISSILE = 0x20,
	STAT_ARMORCLASS_VS_HTH = 0x21,
	STAT_NORMAL_DAMAGE_REDUCTION = 0x22,
	STAT_MAGIC_DAMAGE_REDUCTION = 0x23,
	STAT_DAMAGERESIST = 0x24,
	STAT_MAGICRESIST = 0x25,
	STAT_MAXMAGICRESIST = 0x26,
	STAT_FIRERESIST = 0x27,
	STAT_MAXFIRERESIST = 0x28,
	STAT_LIGHTRESIST = 0x29,
	STAT_MAXLIGHTRESIST = 0x2A,
	STAT_COLDRESIST = 0x2B,
	STAT_MAXCOLDRESIST = 0x2C,
	STAT_POISONRESIST = 0x2D,
	STAT_MAXPOISONRESIST = 0x2E,
	STAT_DAMAGEAURA = 0x2F,
	STAT_FIREMINDAM = 0x30,
	STAT_FIREMAXDAM = 0x31,
	STAT_LIGHTMINDAM = 0x32,
	STAT_LIGHTMAXDAM = 0x33,
	STAT_MAGICMINDAM = 0x34,
	STAT_MAGICMAXDAM = 0x35,
	STAT_COLDMINDAM = 0x36,
	STAT_COLDMAXDAM = 0x37,
	STAT_COLDLENGTH = 0x38,
	STAT_POISONMINDAM = 0x39,
	STAT_POISONMAXDAM = 0x3A,
	STAT_POISONLENGTH = 0x3B,
	STAT_LIFEDRAINMINDAM = 0x3C,
	STAT_LIFEDRAINMAXDAM = 0x3D,
	STAT_MANADRAINMINDAM = 0x3E,
	STAT_MANADRAINMAXDAM = 0x3F,
	STAT_STAMDRAINMINDAM = 0x40,
	STAT_STAMDRAINMAXDAM = 0x41,
	STAT_STUNLENGTH = 0x42,
	STAT_VELOCITYPERCENT = 0x43,
	STAT_ATTACKRATE = 0x44,
	STAT_OTHER_ANIMRATE = 0x45,
	STAT_QUANTITY = 0x46,
	STAT_VALUE = 0x47,
	STAT_DURABILITY = 0x48,
	STAT_MAXDURABILITY = 0x49,
	STAT_HPREGEN = 0x4A,
	STAT_ITEM_MAXDURABILITY_PERCENT = 0x4B,
	STAT_ITEM_MAXHP_PERCENT = 0x4C,
	STAT_ITEM_MAXMANA_PERCENT = 0x4D,
	STAT_ITEM_ATTACKERTAKESDAMAGE = 0x4E,
	STAT_ITEM_GOLDBONUS = 0x4F,
	STAT_ITEM_MAGICBONUS = 0x50,
	STAT_ITEM_KNOCKBACK = 0x51,
	STAT_ITEM_TIMEDURATION = 0x52,
	STAT_ITEM_ADDCLASSSKILLS = 0x53,
	STAT_UNSENTPARAM1 = 0x54,
	STAT_ITEM_ADDEXPERIENCE = 0x55,
	STAT_ITEM_HEALAFTERKILL = 0x56,
	STAT_ITEM_REDUCEDPRICES = 0x57,
	STAT_ITEM_DOUBLEHERBDURATION = 0x58,
	STAT_ITEM_LIGHTRADIUS = 0x59,
	STAT_ITEM_LIGHTCOLOR = 0x5A,
	STAT_ITEM_REQ_PERCENT = 0x5B,
	STAT_ITEM_LEVELREQ = 0x5C,
	STAT_ITEM_FASTERATTACKRATE = 0x5D,
	STAT_ITEM_LEVELREQPCT = 0x5E,
	STAT_LASTBLOCKFRAME = 0x5F,
	STAT_ITEM_FASTERMOVEVELOCITY = 0x60,
	STAT_ITEM_NONCLASSSKILL = 0x61,
	STAT_STATE = 0x62,
	STAT_ITEM_FASTERGETHITRATE = 0x63,
	STAT_MONSTER_PLAYERCOUNT = 0x64,
	STAT_SKILL_POISON_OVERRIDE_LENGTH = 0x65,
	STAT_ITEM_FASTERBLOCKRATE = 0x66,
	STAT_SKILL_BYPASS_UNDEAD = 0x67,
	STAT_SKILL_BYPASS_DEMONS = 0x68,
	STAT_ITEM_FASTERCASTRATE = 0x69,
	STAT_SKILL_BYPASS_BEASTS = 0x6A,
	STAT_ITEM_SINGLESKILL = 0x6B,
	STAT_ITEM_RESTINPEACE = 0x6C,
	STAT_CURSE_RESISTANCE = 0x6D,
	STAT_ITEM_POISONLENGTHRESIST = 0x6E,
	STAT_ITEM_NORMALDAMAGE = 0x6F,
	STAT_ITEM_HOWL = 0x70,
	STAT_ITEM_STUPIDITY = 0x71,
	STAT_ITEM_DAMAGETOMANA = 0x72,
	STAT_ITEM_IGNORETARGETAC = 0x73,
	STAT_ITEM_FRACTIONALTARGETAC = 0x74,
	STAT_ITEM_PREVENTHEAL = 0x75,
	STAT_ITEM_HALFFREEZEDURATION = 0x76,
	STAT_ITEM_TOHIT_PERCENT = 0x77,
	STAT_ITEM_DAMAGETARGETAC = 0x78,
	STAT_ITEM_DEMONDAMAGE_PERCENT = 0x79,
	STAT_ITEM_UNDEADDAMAGE_PERCENT = 0x7A,
	STAT_ITEM_DEMON_TOHIT = 0x7B,
	STAT_ITEM_UNDEAD_TOHIT = 0x7C,
	STAT_ITEM_THROWABLE = 0x7D,
	STAT_ITEM_ELEMSKILL = 0x7E,
	STAT_ITEM_ALLSKILLS = 0x7F,
	STAT_ITEM_ATTACKERTAKESLIGHTDAMAGE = 0x80,
	STAT_IRONMAIDEN_LEVEL = 0x81,
	STAT_LIFETAP_LEVEL = 0x82,
	STAT_THORNS_PERCENT = 0x83,
	STAT_BONEARMOR = 0x84,
	STAT_BONEARMORMAX = 0x85,
	STAT_ITEM_FREEZE = 0x86,
	STAT_ITEM_OPENWOUNDS = 0x87,
	STAT_ITEM_CRUSHINGBLOW = 0x88,
	STAT_ITEM_KICKDAMAGE = 0x89,
	STAT_ITEM_MANAAFTERKILL = 0x8A,
	STAT_ITEM_HEALAFTERDEMONKILL = 0x8B,
	STAT_ITEM_EXTRABLOOD = 0x8C,
	STAT_ITEM_DEADLYSTRIKE = 0x8D,
	STAT_ITEM_ABSORBFIRE_PERCENT = 0x8E,
	STAT_ITEM_ABSORBFIRE = 0x8F,
	STAT_ITEM_ABSORBLIGHT_PERCENT = 0x90,
	STAT_ITEM_ABSORBLIGHT = 0x91,
	STAT_ITEM_ABSORBMAGIC_PERCENT = 0x92,
	STAT_ITEM_ABSORBMAGIC = 0x93,
	STAT_ITEM_ABSORBCOLD_PERCENT = 0x94,
	STAT_ITEM_ABSORBCOLD = 0x95,
	STAT_ITEM_SLOW = 0x96,
	STAT_ITEM_AURA = 0x97,
	STAT_ITEM_INDESCTRUCTIBLE = 0x98,
	STAT_ITEM_CANNOTBEFROZEN = 0x99,
	STAT_ITEM_STAMINADRAINPCT = 0x9A,
	STAT_ITEM_REANIMATE = 0x9B,
	STAT_ITEM_PIERCE = 0x9C,
	STAT_ITEM_MAGICARROW = 0x9D,
	STAT_ITEM_EXPLOSIVEARROW = 0x9E,
	STAT_ITEM_THROW_MINDAMAGE = 0x9F,
	STAT_ITEM_THROW_MAXDAMAGE = 0xA0,
	STAT_SKILL_HANDOFATHENA = 0xA1,
	STAT_SKILL_STAMINAPERCENT = 0xA2,
	STAT_SKILL_PASSIVE_STAMINAPERCENT = 0xA3,
	STAT_SKILL_CONCENTRATION = 0xA4,
	STAT_SKILL_ENCHANT = 0xA5,
	STAT_SKILL_PIERCE = 0xA6,
	STAT_SKILL_CONVICTION = 0xA7,
	STAT_SKILL_CHILLINGARMOR = 0xA8,
	STAT_SKILL_FRENZY = 0xA9,
	STAT_SKILL_DECREPIFY = 0xAA,
	STAT_SKILL_ARMOR_PERCENT = 0xAB,
	STAT_ALIGNMENT = 0xAC,
	STAT_TARGET0 = 0xAD,
	STAT_TARGET1 = 0xAE,
	STAT_GOLDLOST = 0xAF,
	STAT_CONVERSION_LEVEL = 0xB0,
	STAT_CONVERSION_MAXHP = 0xB1,
	STAT_UNIT_DOOVERLAY = 0xB2,
	STAT_ATTACK_VS_MONTYPE = 0xB3,
	STAT_DAMAGE_VS_MONTYPE = 0xB4,
	STAT_FADE = 0xB5,
	STAT_ARMOR_OVERRIDE_PERCENT = 0xB6,
	STAT_UNUSED183 = 0xB7,
	STAT_UNUSED184 = 0xB8,
	STAT_UNUSED185 = 0xB9,
	STAT_UNUSED186 = 0xBA,
	STAT_UNUSED187 = 0xBB,
	STAT_ITEM_ADDSKILL_TAB = 0xBC,
	STAT_UNUSED189 = 0xBD,
	STAT_UNUSED190 = 0xBE,
	STAT_UNUSED191 = 0xBF,
	STAT_UNUSED192 = 0xC0,
	STAT_UNUSED193 = 0xC1,
	STAT_ITEM_NUMSOCKETS = 0xC2,
	STAT_ITEM_SKILLONATTACK = 0xC3,
	STAT_ITEM_SKILLONKILL = 0xC4,
	STAT_ITEM_SKILLONDEATH = 0xC5,
	STAT_ITEM_SKILLONHIT = 0xC6,
	STAT_ITEM_SKILLONLEVELUP = 0xC7,
	STAT_UNUSED200 = 0xC8,
	STAT_ITEM_SKILLONGETHIT = 0xC9,
	STAT_UNUSED202 = 0xCA,
	STAT_UNUSED203 = 0xCB,
	STAT_ITEM_CHARGED_SKILL = 0xCC,
	STAT_UNUSED204 = 0xCD,
	STAT_UNUSED205 = 0xCE,
	STAT_UNUSED206 = 0xCF,
	STAT_UNUSED207 = 0xD0,
	STAT_UNUSED208 = 0xD1,
	STAT_UNUSED209 = 0xD2,
	STAT_UNUSED210 = 0xD3,
	STAT_UNUSED211 = 0xD4,
	STAT_UNUSED212 = 0xD5,
	STAT_ITEM_ARMOR_PERLEVEL = 0xD6,
	STAT_ITEM_ARMORPERCENT_PERLEVEL = 0xD7,
	STAT_ITEM_HP_PERLEVEL = 0xD8,
	STAT_ITEM_MANA_PERLEVEL = 0xD9,
	STAT_ITEM_MAXDAMAGE_PERLEVEL = 0xDA,
	STAT_ITEM_MAXDAMAGE_PERCENT_PERLEVEL = 0xDB,
	STAT_ITEM_STRENGTH_PERLEVEL = 0xDC,
	STAT_ITEM_DEXTERITY_PERLEVEL = 0xDD,
	STAT_ITEM_ENERGY_PERLEVEL = 0xDE,
	STAT_ITEM_VITALITY_PERLEVEL = 0xDF,
	STAT_ITEM_TOHIT_PERLEVEL = 0xE0,
	STAT_ITEM_TOHITPERCENT_PERLEVEL = 0xE1,
	STAT_ITEM_COLD_DAMAGEMAX_PERLEVEL = 0xE2,
	STAT_ITEM_FIRE_DAMAGEMAX_PERLEVEL = 0xE3,
	STAT_ITEM_LTNG_DAMAGEMAX_PERLEVEL = 0xE4,
	STAT_ITEM_POIS_DAMAGEMAX_PERLEVEL = 0xE5,
	STAT_ITEM_RESIST_COLD_PERLEVEL = 0xE6,
	STAT_ITEM_RESIST_FIRE_PERLEVEL = 0xE7,
	STAT_ITEM_RESIST_LTNG_PERLEVEL = 0xE8,
	STAT_ITEM_RESIST_POIS_PERLEVEL = 0xE9,
	STAT_ITEM_ABSORB_COLD_PERLEVEL = 0xEA,
	STAT_ITEM_ABSORB_FIRE_PERLEVEL = 0xEB,
	STAT_ITEM_ABSORB_LTNG_PERLEVEL = 0xEC,
	STAT_ITEM_ABSORB_POIS_PERLEVEL = 0xED,
	STAT_ITEM_THORNS_PERLEVEL = 0xEE,
	STAT_ITEM_FIND_GOLD_PERLEVEL = 0xEF,
	STAT_ITEM_FIND_MAGIC_PERLEVEL = 0xF0,
	STAT_ITEM_REGENSTAMINA_PERLEVEL = 0xF1,
	STAT_ITEM_STAMINA_PERLEVEL = 0xF2,
	STAT_ITEM_DAMAGE_DEMON_PERLEVEL = 0xF3,
	STAT_ITEM_DAMAGE_UNDEAD_PERLEVEL = 0xF4,
	STAT_ITEM_TOHIT_DEMON_PERLEVEL = 0xF5,
	STAT_ITEM_TOHIT_UNDEAD_PERLEVEL = 0xF6,
	STAT_ITEM_CRUSHINGBLOW_PERLEVEL = 0xF7,
	STAT_ITEM_OPENWOUNDS_PERLEVEL = 0xF8,
	STAT_ITEM_KICK_DAMAGE_PERLEVEL = 0xF9,
	STAT_ITEM_DEADLYSTRIKE_PERLEVEL = 0xFA,
	STAT_ITEM_FIND_GEMS_PERLEVEL = 0xFB,
	STAT_ITEM_REPLENISH_DURABILITY = 0xFC,
	STAT_ITEM_REPLENISH_QUANTITY = 0xFD,
	STAT_ITEM_EXTRA_STACK = 0xFE,
	STAT_ITEM_FIND_ITEM = 0xFF,
	STAT_ITEM_SLASH_DAMAGE = 0x100,
	STAT_ITEM_SLASH_DAMAGE_PERCENT = 0x101,
	STAT_ITEM_CRUSH_DAMAGE = 0x102,
	STAT_ITEM_CRUSH_DAMAGE_PERCENT = 0x103,
	STAT_ITEM_THRUST_DAMAGE = 0x104,
	STAT_ITEM_THRUST_DAMAGE_PERCENT = 0x105,
	STAT_ITEM_ABSORB_SLASH = 0x106,
	STAT_ITEM_ABSORB_CRUSH = 0x107,
	STAT_ITEM_ABSORB_THRUST = 0x108,
	STAT_ITEM_ABSORB_SLASH_PERCENT = 0x109,
	STAT_ITEM_ABSORB_CRUSH_PERCENT = 0x10A,
	STAT_ITEM_ABSORB_THRUST_PERCENT = 0x10B,
	STAT_ITEM_ARMOR_BYTIME = 0x10C,
	STAT_ITEM_ARMORPERCENT_BYTIME = 0x10D,
	STAT_ITEM_HP_BYTIME = 0x10E,
	STAT_ITEM_MANA_BYTIME = 0x10F,
	STAT_ITEM_MAXDAMAGE_BYTIME = 0x110,
	STAT_ITEM_MAXDAMAGE_PERCENT_BYTIME = 0x111,
	STAT_ITEM_STRENGTH_BYTIME = 0x112,
	STAT_ITEM_DEXTERITY_BYTIME = 0x113,
	STAT_ITEM_ENERGY_BYTIME = 0x114,
	STAT_ITEM_VITALITY_BYTIME = 0x115,
	STAT_ITEM_TOHIT_BYTIME = 0x116,
	STAT_ITEM_TOHITPERCENT_BYTIME = 0x117,
	STAT_ITEM_COLD_DAMAGEMAX_BYTIME = 0x118,
	STAT_ITEM_FIRE_DAMAGEMAX_BYTIME = 0x119,
	STAT_ITEM_LTNG_DAMAGEMAX_BYTIME = 0x11A,
	STAT_ITEM_POIS_DAMAGEMAX_BYTIME = 0x11B,
	STAT_ITEM_RESIST_COLD_BYTIME = 0x11C,
	STAT_ITEM_RESIST_FIRE_BYTIME = 0x11D,
	STAT_ITEM_RESIST_LTNG_BYTIME = 0x11E,
	STAT_ITEM_RESIST_POIS_BYTIME = 0x11F,
	STAT_ITEM_ABSORB_COLD_BYTIME = 0x120,
	STAT_ITEM_ABSORB_FIRE_BYTIME = 0x121,
	STAT_ITEM_ABSORB_LTNG_BYTIME = 0x122,
	STAT_ITEM_ABSORB_POIS_BYTIME = 0x123,
	STAT_ITEM_FIND_GOLD_BYTIME = 0x124,
	STAT_ITEM_FIND_MAGIC_BYTIME = 0x125,
	STAT_ITEM_REGENSTAMINA_BYTIME = 0x126,
	STAT_ITEM_STAMINA_BYTIME = 0x127,
	STAT_ITEM_DAMAGE_DEMON_BYTIME = 0x128,
	STAT_ITEM_DAMAGE_UNDEAD_BYTIME = 0x129,
	STAT_ITEM_TOHIT_DEMON_BYTIME = 0x12A,
	STAT_ITEM_TOHIT_UNDEAD_BYTIME = 0x12B,
	STAT_ITEM_CRUSHINGBLOW_BYTIME = 0x12C
};

struct UnitAny
{
	DWORD dwType;					//0x00
	DWORD dwTxtFileNo;				//0x04
	DWORD _1;						//0x08
	DWORD dwUnitId;					//0x0C
	DWORD dwMode;					//0x10
	DWORD pUnitData;				//0x14 PlayerData* if player, ItemData* if item ...
	DWORD dwAct;					//0x18
	DWORD pAct;						//0x1C
	DWORD dwSeed[2];				//0x20
	DWORD _2;						//0x28
	DWORD pPath;					//0x24 Path*
	DWORD _3[5];					//0x30
	DWORD dwGfxFrame;				//0x44
	DWORD dwFrameRemain;			//0x48
	WORD wFrameRate;				//0x4C
	WORD _4;						//0x4E
	DWORD pGfxUnk;					//0x50 BYTE*
	DWORD pGfxInfo;					//0x54 DWORD*
	DWORD _5;						//0x58
	DWORD pStats;					//0x5C StatList*
	DWORD pInventory;				//0x60 Inventory*
	DWORD ptLight;					//0x64 Light*
	DWORD _6[9];					//0x68
	WORD wX;						//0x8C
	WORD wY;						//0x8E
	DWORD _7;						//0x90
	DWORD dwOwnerType;				//0x94
	DWORD dwOwnerId;				//0x98
	DWORD _8[2];					//0x9C
	DWORD pOMsg;					//0xA4 OverheadMsg*
	DWORD pInfo;					//0xA8 Info*
	DWORD _9[6];					//0xAC
	DWORD dwFlags;					//0xC4
	DWORD dwFlags2;					//0xC8
	DWORD _10[5];					//0xCC
	DWORD pChangedNext;			//0xE0
	DWORD pRoomNext;			//0xE4
	DWORD pListNext;			//0xE8 -> 0xD8
	//CHAR szNameCopy[0x10];			//+0x66
};

struct ItemData
{
	DWORD dwQuality;				//0x00
	DWORD _1[2];					//0x04
	DWORD dwItemFlags;				//0x0C 1 = Owned by player, 0xFFFFFFFF = Not owned
	DWORD _2[2];					//0x10
	DWORD dwFlags;					//0x18
	DWORD _3[3];					//0x1C
	DWORD dwQuality2;				//0x28
	DWORD dwItemLevel;				//0x2C
	WORD wVersion;					//0x30
	WORD wRarePrefix;				//0x32
	WORD wRareSuffix;				//0x34
	WORD _14;						//0x36
	WORD wPrefix[3];				//0x38
	WORD wSuffix[3];				//0x3E
	BYTE BodyLocation;				//0x44
	BYTE ItemLocation;				//0x45 Non-body/belt location (Body/Belt == 0xFF)
	BYTE _7;						//0x46
	BYTE _8;						//0x47
	BYTE EarLevel;					//0x48
	BYTE VarGfx;					//0x49
	CHAR personalizedName[16];		//0x4A
	WORD _10;						//0x5A
	DWORD pOwnerInventory;			//0x5C Inventory*
	DWORD pPrevInvItem;				//0x60 UnitAny*
	DWORD pNextInvItem;				//0x64 UnitAny*
	BYTE _11;						//0x68
	BYTE NodePage;					//0x69 Actual location, this is the most reliable by far
	WORD _12;						//0x6A
	DWORD _13[6];					//0x6C
	DWORD pOwner;					//0x84 UnitAny*
};

struct Waypoint {
	BYTE _1;
	BYTE _2;
	BYTE _act1;
	BYTE _act2;
	BYTE _act3;
	BYTE _act4;
	BYTE _act5;
	BYTE _3[9];

	BYTE _4;
	BYTE _5;	
	BYTE norm_act1;
	BYTE norm_act2;
	BYTE norm_act3;
	BYTE norm_act4;
	BYTE norm_act5;
	BYTE _6[9];

	BYTE _7;
	BYTE _8;
	BYTE nightmare_act1;
	BYTE nightmare_act2;
	BYTE nightmare_act3;
	BYTE nightmare_act4;
	BYTE nightmare_act5;
	BYTE _9[9];

	BYTE _10;
	BYTE _11;
	BYTE hell_act1;
	BYTE hell_act2;
	BYTE hell_act3;
	BYTE hell_act4;
	BYTE hell_act5;
	BYTE _12[9];
};

struct PlayerData { //size 0x16C
	char szName[16];				//0x00
	QuestInfo *pNormalQuest;		//0x10
	QuestInfo *pNightmareQuest;		//0x14
	QuestInfo *pHellQuest;			//0x18
	Waypoint *pNormalWaypoint;		//0x1c
	Waypoint *pNightmareWaypoint;	//0x20
	Waypoint *pHellWaypoint;		//0x24
	DWORD _1[3];					//0x28
	Arena* pArena;					//0x34
	DWORD _1b;						//0x38
	DWORD _2[13];					//0x3C
	DWORD dwRightSkill;				//0x70
	DWORD dwLeftSkill;				//0x74
	DWORD dwRightSkillFlags;		//0x78
	DWORD dwLeftSkillFlags;			//0x7C
	DWORD dwSwitchRightSkill;		//0x80
	DWORD dwSwitchLeftSkill;		//0x84
	DWORD dwSwitchRightSkillFlags;	//0x88
	DWORD dwSwitchLeftSkillFlags;	//0x8C
	DWORD _3[3];					//0x90
	ClientData* pClientData;		//0x9C
	DWORD _6[50];					//0x100
	DWORD GameFrame;				//0x168
};

struct Inventory
{
	DWORD dwSignature;				//0x00 0x1020304
	DWORD bGame1C;					//0x04 BYTE*
	DWORD pOwner;					//0x08 UnitAny*
	DWORD pFirstItem;				//0x0C UnitAny*
	DWORD pLastItem;				//0x10 UnitAny*
	DWORD pStores;					//0x14 InventoryStore* [count]
	DWORD dwStoresCount;			//0x18
	DWORD dwLeftItemUid;			//0x1C
	DWORD pCursorItem;				//0x20 UnitAny*
	DWORD dwOwnerId;				//0x24
	DWORD dwItemCount;				//0x28
};

struct InventoryStore
{
	DWORD pFirstItem;				//0x00
	DWORD pLastItem;				//0x04
	BYTE Width;						//0x08
	BYTE Height;					//0x09
	BYTE unk[2];					//0x0A
	DWORD pArray;					//0x0C UnitAny* [height][width]
};

struct StatList
{
	DWORD _1[9];					//0x00
	DWORD pStat;					//0x24 Stat*
	WORD wStatCount;				//0x28
	WORD wStatSize;					//0x2A
	DWORD pNext;					//0x2C StatList*
	DWORD pPrev;					//0x30 StatList*
	DWORD pParent;					//0x34 StatList*
	DWORD _4;						//0x38
	DWORD pFirstChild;				//0x3C StatList*
};

struct StatArr
{
	DWORD pStat;					// 0 Stat*
	WORD wStatCount;				// 4
	WORD wStatSize;					// 6
};									// 8

struct Stat
{
	WORD wSubIndex;					//0x00
	WORD wStatIndex;				//0x02
	DWORD dwStatValue;				//0x04
};

struct SkillsTxt //size 0x23C
{
	DWORD wSkillId; 			//0x00
	struct {
		BYTE bDecquant : 1;				//0
		BYTE bLob : 1;					//1
		BYTE bProgressive : 1;			//2
		BYTE bFinishing : 1;				//3
		BYTE bPassive : 1;				//4
		BYTE bAura : 1;					//5
		BYTE bPeriodic : 1;				//6
		BYTE bPrgStack : 1;				//7
		BYTE bInTown : 1;					//8
		BYTE bKick : 1;					//9
		BYTE bInGame : 1;					//10
		BYTE bRepeat : 1;					//11
		BYTE bStSuccessOnly : 1;			//12
		BYTE bStSoundDelay : 1;			//13
		BYTE bWeaponSnd : 1;				//14
		BYTE bImmediate : 1;				//15
		BYTE bNoAmmo : 1;					//16
		BYTE bEnhanceable : 1;			//17
		BYTE bDurability : 1;				//18
		BYTE bUseAttackRating : 1;		//19
		BYTE bTargetableOnly : 1;			//20
		BYTE bSearchEnemyXY : 1;			//21
		BYTE bSearchEnemyNear : 1;		//22
		BYTE bSearchOpenXY : 1;			//23
		BYTE bTargetCorpse : 1;			//24
		BYTE bTargetPet : 1;				//25
		BYTE bTargetAlly : 1;				//26
		BYTE bTargetItem : 1;				//27
		BYTE bAttackNoMana : 1;			//28
		BYTE bItemTgtDo : 1;				//29
		BYTE bLeftSkill : 1;				//30
		BYTE bInterrupt : 1;				//31
		BYTE bTgtPlaceCheck : 1;			//32
		BYTE bItemCheckStart : 1;			//33
		BYTE bItemCltCheckStart : 1;		//34
		BYTE bGeneral : 1;				//35
		BYTE bScroll : 1;					//36
		BYTE bUseManaOnDo : 1;			//37
		BYTE bWarp : 1;					//38
	} dwFlags;				//0x04 (5 bytes)
	DWORD bClassId;			//0x0C
	BYTE bAnim;				//0x10
	BYTE bMonAnim;			//0x11
	BYTE bSeqTrans;			//0x12
	BYTE bSeqNum;			//0x13
	BYTE bRange;			//0x14
	BYTE bSelectProc;		//0x15
	BYTE bSeqInput;			//0x16
	BYTE _1;				//0x17
	WORD wITypeA1;			//0x18
	WORD wITypeA2;			//0x1A
	WORD wITypeA3;			//0x1C
	WORD wITypeB1;			//0x1E
	WORD wITypeB2;			//0x20
	WORD wITypeB3;			//0x22
	WORD wETypeA1;			//0x24
	WORD wETypeA2;			//0x26
	WORD wETypeB1;			//0x28
	WORD wETypeB2;			//0x2A
	WORD wSrvStartFunc;		//0x2C
	WORD wSrvDoFunc;		//0x2E
	WORD wSrvPrgFunc1;		//0x30
	WORD wSrvPrgFunc2;		//0x32
	WORD wSrvPrgFunc3;		//0x34
	WORD _2;				//0x36
	DWORD dwPrgCalc1;		//0x38
	DWORD dwPrgCalc2;		//0x3C
	DWORD dwPrgCalc3;		//0x40
	WORD bPrgDamage;		//0x44
	WORD wSrvMissile;		//0x46
	WORD wSrvMissileA;		//0x48
	WORD wSrvMissileB;		//0x4A
	WORD wSrvMissileC;		//0x4C
	WORD wSrvOverlay;		//0x4E
	DWORD dwAuraFilter;		//0x50
	WORD wAuraStat1;		//0x54
	WORD wAuraStat2;		//0x56
	WORD wAuraStat3;		//0x58
	WORD wAuraStat4;		//0x5A
	WORD wAuraStat5;		//0x5C
	WORD wAuraStat6;		//0x5E
	DWORD dwAuraLenCalc;	//0x60
	DWORD dwAuraRangeCalc;	//0x64
	DWORD dwAuraStatCalc1;	//0x68
	DWORD dwAuraStatCalc2;	//0x6C
	DWORD dwAuraStatCalc3;	//0x70
	DWORD dwAuraStatCalc4;	//0x74
	DWORD dwAuraStatCalc5;	//0x78
	DWORD dwAuraStatCalc6;	//0x7C
	WORD wAuraState;		//0x80
	WORD wAuraTargetState;	//0x82
	WORD wAuraEvent1;		//0x84
	WORD wAuraEvent2;		//0x86
	WORD wAuraEvent3;		//0x88
	WORD wAuraEventFunc1;	//0x8A
	WORD wAuraEventFunc2;	//0x8C
	WORD wAuraEventFunc3;	//0x8E
	WORD wAuraTgtEvent;		//0x90
	WORD wAuraTgtEventFunc;	//0x92
	WORD wPassiveState;		//0x94
	WORD wPassiveiType;		//0x96
	WORD wPassiveStat1;		//0x98
	WORD wPassiveStat2;		//0x9A
	WORD wPassiveStat3;		//0x9C
	WORD wPassiveStat4;		//0x9E
	WORD wPassiveStat5;		//0xA0
	WORD _3;				//0xA2
	DWORD dwPassiveCalc1;	//0xA4
	DWORD dwPassiveCalc2;	//0xA8
	DWORD dwPassiveCalc3;	//0xAC
	DWORD dwPassiveCalc4;	//0xB0
	DWORD dwPassiveCalc5;	//0xB4
	WORD wPassiveEvent;		//0xB8
	WORD wPassiveEventFunc; //0xBA
	WORD wSummon;			//0xBC
	BYTE wPetType;			//0xBE
	BYTE bSumMode;			//0xBF
	DWORD dwPetMax;			//0xC0
	WORD wSumSkill1;		//0xC4
	WORD wSumSkill2;		//0xC6
	WORD wSumSkill3;		//0xC8
	WORD wSumSkill4;		//0xCA
	WORD wSumSkill5;		//0xCC
	WORD _4;				//0xCE
	DWORD dwSumSkCalc1;		//0xD0
	DWORD dwSumSkCalc2;		//0xD4
	DWORD dwSumSkCalc3;		//0xD8
	DWORD dwSumSkCalc4;		//0xDC
	DWORD dwSumSkCalc5;		//0xE0
	WORD wSumUMod;			//0xE4
	WORD wSumOverlay;		//0xE6
	WORD wCltMissile;		//0xE8
	WORD wCltMissileA;		//0xEA
	WORD wCltMissileB;		//0xEC
	WORD wCltMissileC;		//0xEE
	WORD wCltMissileD;		//0xF0
	WORD wCltStFunc;		//0xF2
	WORD wCltDoFunc;		//0xF4
	WORD wCltPrgFunc1;		//0xF6
	WORD wCltPrgFunc2;		//0xF8
	WORD wCltPrgFunc3;		//0xFA
	WORD wStSound;			//0xFC
	WORD _5;				//0xFE
	WORD wDoSound;			//0x100
	WORD wDoSoundA;			//0x102
	WORD wDoSoundB;			//0x104
	WORD wCastOverlay;		//0x106
	WORD wTgtOverlay;		//0x108
	WORD wTgtSound;			//0x10A
	WORD wPrgOverlay;		//0x10C
	WORD wPrgSound;			//0x10E
	WORD wCltOverlayA;		//0x110
	WORD wCltOverlayB;		//0x112
	DWORD dwCltCalc1;		//0x114
	DWORD dwCltCalc2;		//0x118
	DWORD dwCltCalc3;		//0x11C
	WORD bItemTarget;		//0x120
	WORD wItemCastSound;	//0x122
	DWORD wItemCastOverlay;	//0x124
	DWORD dwPerDelay;		//0x128
	WORD wMaxLvl;			//0x12C
	WORD wResultFlags;		//0x12E
	DWORD dwHitFlags;		//0x130
	DWORD dwHitClass;		//0x134
	DWORD dwCalc1;			//0x138
	DWORD dwCalc2;			//0x13C
	DWORD dwCalc3;			//0x140
	DWORD dwCalc4;			//0x144
	DWORD dwParam1;			//0x148
	DWORD dwParam2;			//0x14C
	DWORD dwParam3;			//0x150
	DWORD dwParam4;			//0x154
	DWORD dwParam5;			//0x158
	DWORD dwParam6;			//0x15C
	DWORD dwParam7;			//0x160
	DWORD dwParam8;			//0x164
	WORD bWeapSel;			//0x168
	WORD wItemEffect;		//0x16A
	DWORD wItemCltEffect;	//0x16C
	DWORD dwSkPoints;		//0x170
	WORD wReqLevel;			//0x174
	WORD wReqStr;			//0x176
	WORD wReqDex;			//0x178
	WORD wReqInt;			//0x17A
	WORD wReqVit;			//0x17C
	WORD wReqSkill1;		//0x17E
	WORD wReqSkill2;		//0x180
	WORD wReqSkill3;		//0x182
	WORD wStartMana;		//0x184
	WORD wMinMana;			//0x186
	WORD wManaShift;		//0x188
	WORD wMana;				//0x18A
	WORD wLevelMana;		//0x18C
	BYTE bAttackRank;		//0x18E
	BYTE bLineOfSight;		//0x18F
	DWORD dwDelay;			//0x190
	DWORD wSkillDesc;		//0x194
	DWORD dwToHit;			//0x198
	DWORD dwLevToHit;		//0x19C
	DWORD dwToHitCalc;		//0x1A0
	BYTE bToHitShift;		//0x1A4
	BYTE bSrcDam;			//0x1A5
	WORD _6;				//0x1A6
	DWORD dwMinDam;			//0x1A8
	DWORD dwMaxDam;			//0x1AC
	DWORD dwMinLvlDam1;		//0x1B0
	DWORD dwMinLvlDam2;		//0x1B4
	DWORD dwMinLvlDam3;		//0x1B8
	DWORD dwMinLvlDam4;		//0x1BC
	DWORD dwMinLvlDam5;		//0x1C0
	DWORD dwMaxLvlDam1;		//0x1C4
	DWORD dwMaxLvlDam2;		//0x1C8
	DWORD dwMaxLvlDam3;		//0x1CC
	DWORD dwMaxLvlDam4;		//0x1D0
	DWORD dwMaxLvlDam5;		//0x1D4
	DWORD dwDmgSymPerCalc;	//0x1D8
	WORD bEType;			//0x1DC
	WORD _7;				//0x1DE
	DWORD dwEMin;			//0x1E0
	DWORD dwEMax;			//0x1E4
	DWORD dwEMinLev1;		//0x1E8
	DWORD dwEMinLev2;		//0x1EC
	DWORD dwEMinLev3;		//0x1F0
	DWORD dwEMinLev4;		//0x1F4
	DWORD dwEMinLev5;		//0x1F8
	DWORD dwEMaxLev1;		//0x1FC
	DWORD dwEMaxLev2;		//0x200
	DWORD dwEMaxLev3;		//0x204
	DWORD dwEMaxLev4;		//0x208
	DWORD dwEMaxLev5;		//0x20C
	DWORD dwEDmgSymPerCalc;	//0x210
	DWORD dwELen;			//0x214
	DWORD dwELevLen1;		//0x218
	DWORD dwELevLen2;		//0x21C
	DWORD dwELevLen3;		//0x220
	DWORD dwELenSymPerCalc;	//0x224
	WORD bRestrict;			//0x228
	WORD wState1;			//0x22A
	WORD wState2;			//0x22C
	WORD wState3;			//0x22E
	WORD bAiType;			//0x230
	WORD wAiBonus;			//0x232
	DWORD dwCostMult;		//0x234
	DWORD dwCostAdd;		//0x238
};

struct Skill {
	SkillsTxt* pSkillsTxt;			//0x00
	Skill* pNextSkill;				//0x04
	DWORD SkillMode;				//0x08
	DWORD _1[7];					//0x0C
	DWORD dwSkillLevel;				//0x28
	DWORD _2[2];					//0x2C
	DWORD dwFlags;					//0x30
};

struct ItemTxt
{
	CHAR flp[0x20];		// 0x00
	CHAR inv[0x20];		// 0x20
	CHAR invu[0x20];	// 0x40
	CHAR invs[0x20];	// 0x60
	CHAR name[4];		// 0x80
	CHAR normal[4];		// 0x84
	CHAR exceptional[4];// 0x88
	CHAR elite[4];		// 0x8C
	CHAR beta[4];		// 0x90
	DWORD pSpell;		// 0x94
	WORD state;			// 0x98
	WORD cstate1;		// 0x9A
	WORD cstate2;		// 0x9C
	WORD stat1;			// 0x9E
	WORD stat2;			// 0xA0
	WORD stat3;			// 0xA2
	DWORD calc1;		// 0xA4
	DWORD calc2;		// 0xA8
	DWORD calc3;		// 0xAC
	DWORD len;			// 0xB0
	WORD spelldesc;		// 0xB4 (bool)
	WORD spelldescstr;	// 0xB6 (hash)
	DWORD spelldesccalc;// 0xBB
	BYTE bettergem[4];	// 0xBC
	BYTE wclass[4];		// 0xC0
	BYTE w2hclass[4];	// 0xC4
	BYTE TMogType[4];	// 0xC8
	DWORD minac;		// 0xCC
	DWORD maxac;		// 0xD0
	DWORD gamblecost;	// 0xD4
	DWORD speed;		// 0xD8
	DWORD bitfield1;	// 0xDC
	DWORD cost;			// 0xE0
	DWORD minstack;		// 0xE4
	DWORD maxstack;		// 0xE8
	DWORD spawnstack;	// 0xEC
	DWORD _2;			// 0xF0
	WORD nameHash;		// 0xF4
	WORD version;		// 0xF6
	WORD autoprefix;	// 0xF8
	WORD missiletype;	// 0xFA
	BYTE rarity;		// 0xFC
	BYTE level;			// 0xFD
	BYTE mindam;		// 0xFE
	BYTE maxdam;		// 0xFF
	BYTE minmisdam;		// 0x100
	BYTE maxmisdam;		// 0x101
	BYTE mindam2hand;	// 0x102
	BYTE maxdam2hand;	// 0x103
	WORD rangeadder;	// 0x104
	WORD strbonus;		// 0x106
	WORD dexbonus;		// 0x108
	WORD reqstr;		// 0x10A
	WORD reqdex;		// 0x10C
	BYTE absorbs;		// 0x10E
	BYTE invwidth;		// 0x10F
	BYTE invheight;		// 0x110
	BYTE block;			// 0x111
	BYTE durability;	// 0x112
	BYTE nodurability;	// 0x113
	BYTE _3;			// 0x114
	BYTE component;		// 0x115
	BYTE rArm;			// 0x116
	BYTE lArm;			// 0x117
	BYTE Torso;			// 0x118
	BYTE Legs;			// 0x119
	BYTE rSPad;			// 0x11A
	BYTE lSPad;			// 0x11B
	BYTE _2handed;		// 0x11C
	BYTE useable;		// 0x11D
	WORD type;			// 0x11E  (axe/club/mace...)
	WORD type2;			// 0x120  (gem0,gem1,gem2)
	WORD _4;			// 0x122
	WORD dropsound;		// 0x124
	WORD usesound;		// 0x126
	BYTE dropsfxframe;	// 0x128
	BYTE unique;		// 0x129
	BYTE quest;			// 0x12A
	BYTE questdiffcheck;// 0x12B
	BYTE transparent;	// 0x12C
	BYTE transtbl;		// 0x12D
	BYTE _5;			// 0x12E
	BYTE lightradius;	// 0x12F
	BYTE belt;			// 0x130
	BYTE autobelt;		// 0x131
	BYTE stackable;		// 0x132
	BYTE spawnable;		// 0x133
	BYTE spellid;		// 0x134
	BYTE durwarning;	// 0x135
	BYTE qntwarning;	// 0x136
	BYTE hasinv;		// 0x137
	BYTE gemsockets;	// 0x138
	BYTE Transmogrify;	// 0x139
	BYTE TMogMin;		// 0x13A
	BYTE TMogMax;		// 0x13B
	BYTE hitclass;		// 0x13C
	BYTE _1or2handed;	// 0x13D
	BYTE gemapplytype;	// 0x13E
	BYTE levelreq;		// 0x13F
	BYTE magiclvl;		// 0x140
	BYTE Transform;		// 0x141
	BYTE InvTrans;		// 0x142
	BYTE compactsave;	// 0x143
	BYTE SkipName;		// 0x144
	BYTE Nameable;		// 0x145
	BYTE AkaraMin;		// 0x146
	BYTE GheedMin;		// 0x147
	BYTE CharsiMin;		// 0x148
	BYTE FaraMin;		// 0x149
	BYTE LysanderMin;	// 0x14A
	BYTE DrognanMin; 	// 0x14B
	BYTE HraltiMin;		// 0x14C
	BYTE AlkorMin;		// 0x14D
	BYTE OrmusMin;		// 0x14E
	BYTE ElzixMin;		// 0x14F
	BYTE AshearaMin;	// 0x150
	BYTE CainMin;		// 0x151
	BYTE HalbuMin;		// 0x152
	BYTE JamellaMin;	// 0x153
	BYTE MalahMin;		// 0x154
	BYTE LarzukMin;		// 0x155
	BYTE DrehyaMin;		// 0x156
	BYTE AkaraMax;		// 0x157
	BYTE GheedMax;		// 0x158
	BYTE CharsiMax;		// 0x159
	BYTE FaraMax;		// 0x15A
	BYTE LysanderMax;	// 0x15B
	BYTE DrognanMax; 	// 0x15C
	BYTE HraltiMax;		// 0x15D
	BYTE AlkorMax;		// 0x15E
	BYTE OrmusMax;		// 0x15F
	BYTE ElzixMax;		// 0x160
	BYTE AshearaMax;	// 0x161
	BYTE CainMax;		// 0x162
	BYTE HalbuMax;		// 0x163
	BYTE JamellaMax;	// 0x164
	BYTE MalahMax;		// 0x165
	BYTE LarzukMax;		// 0x166
	BYTE DrehyaMax;		// 0x167
	BYTE AkaraMagicMin;	// 0x168
	BYTE GheedMagicMin;	// 0x169
	BYTE CharsiMagicMin;// 0x16A
	BYTE FaraMagicMin;	// 0x16B
	BYTE LysanderMagicMin;	// 0x16C
	BYTE DrognanMagicMin;	// 0x16D
	BYTE HraltiMagicMin;	// 0x16E
	BYTE AlkorMagicMin;		// 0x16F
	BYTE OrmusMagicMin;		// 0x170
	BYTE ElzixMagicMin;		// 0x171
	BYTE AshearaMagicMin;	// 0x172
	BYTE CainMagicMin;		// 0x173
	BYTE HalbuMagicMin;		// 0x174
	BYTE JamellaMagicMin;	// 0x175
	BYTE MalahMagicMin;		// 0x176
	BYTE LarzukMagicMin;	// 0x177
	BYTE DrehyaMagicMin;	// 0x178
	BYTE AkaraMagicMax;		// 0x179
	BYTE GheedMagicMax;		// 0x17A
	BYTE CharsiMagicMax;	// 0x17B
	BYTE FaraMagicMax;		// 0x17C
	BYTE LysanderMagicMax;	// 0x17D
	BYTE DrognanMagicMax; 	// 0x17E
	BYTE HraltiMagicMax;	// 0x17F
	BYTE AlkorMagicMax;		// 0x180
	BYTE OrmusMagicMax;		// 0x181
	BYTE ElzixMagicMax;		// 0x182
	BYTE AshearaMagicMax;	// 0x183
	BYTE CainMagicMax;		// 0x184
	BYTE HalbuMagicMax;		// 0x185
	BYTE JamellaMagicMax;	// 0x186
	BYTE MalahMagicMax;		// 0x187
	BYTE LarzukMagicMax;	// 0x188
	BYTE DrehyaMagicMax;	// 0x189
	BYTE AkaraMagicLvl;		// 0x18A
	BYTE GheedMagicLvl;		// 0x18B
	BYTE CharsiMagicLvl;	// 0x18C
	BYTE FaraMagicLvl;		// 0x18D
	BYTE LysanderMagicLvl;	// 0x18E
	BYTE DrognanMagicLvl; 	// 0x18F
	BYTE HraltiMagicLvl;	// 0x190
	BYTE AlkorMagicLvl;		// 0x191
	BYTE OrmusMagicLvl;		// 0x192
	BYTE ElzixMagicLvl;		// 0x193
	BYTE AshearaMagicLvl;	// 0x194
	BYTE CainMagicLvl;		// 0x195
	BYTE HalbuMagicLvl;		// 0x196
	BYTE JamellaMagicLvl;	// 0x197
	BYTE MalahMagicLvl;		// 0x198
	BYTE LarzukMagicLvl;	// 0x199
	BYTE DrehyaMagicLvl;	// 0x19A
	BYTE _6;				// 0x19B
	BYTE NightmareUpgrade[4];// 0x19C
	BYTE HellUpgrade[4];	// 0x1A0
	BYTE PermStoreItem;		// 0x1A4
	BYTE multibuy;			// 0x1A5
	BYTE _7[2];				// 0x1A6
};							// 0x1A8

struct MagicSuffixTxt
{
	CHAR name[0x20];		// 0x00
	BYTE _1[0x45];			// 0x20
	BYTE levelreq;			// 0x65
	BYTE _2[0x2A];			// 0x66
};							// 0x90

struct RareSuffixTxt
{
	CHAR _1[0x26];			// 0x00
	CHAR name[0x22];		// 0x26
};							// 0x48

struct UniqueStats
{
	DWORD prop;
	DWORD par; // subprop?
	DWORD min;
	DWORD max;
};

struct UniqueTxt
{
	WORD _1; 				// 0x000
	CHAR name[0x22];		// 0x002
	DWORD version;			// 0x024
	CHAR base[4];			// 0x028
	DWORD flags;			// 0x02C 1 = enabled, 2 = ?, 4 = carry1, 8 = ladder 
	DWORD rarity;			// 0x030
	WORD lvl;				// 0x034
	WORD lvlreq;			// 0x036
	BYTE chrtransform;		// 0x038
	BYTE invtransform;		// 0x039
	CHAR flippyfile[0x20];	// 0x03A
	CHAR invfile[0x22];		// 0x05A
	DWORD costmult;			// 0x07C
	DWORD costadd;			// 0x080
	WORD dropsound;			// 0x084
	WORD usesound;			// 0x086
	DWORD dropsfxframe;		// 0x088
	UniqueStats props[12];	// 0x08C
};							// 0x14C

struct SetTxt
{
	WORD _1; 				// 0x000
	CHAR name[0x26];		// 0x002
	CHAR base[4];			// 0x028
	DWORD _2;				// 0x02C
	WORD lvl;				// 0x030
	WORD lvlreq;			// 0x032
	WORD rarity;			// 0x034
	BYTE _3[0x52];			// 0x036
	UniqueStats props[19];	// 0x088
};							// 0x1B8

struct ItemStatCostTxt
{
	BYTE _1[0x18];			// 0x000
	BYTE ValShift;			// 0x018
	BYTE _2[0x1B];			// 0x019
	WORD descpriority;		// 0x034
	BYTE descfunc;			// 0x036
	BYTE descval;			// 0x037
	WORD descstrpos;		// 0x038
	WORD descstrneg;		// 0x03A
	WORD descstr2;			// 0x03C
	WORD dgrp;				// 0x03E
	BYTE dgrpfunc;			// 0x040
	BYTE dgrpval;			// 0x041
	WORD dgrpstrpos;		// 0x042
	WORD dgrpstrneg;		// 0x044
	WORD dgrpstr2;			// 0x046
	BYTE _3[0xC];			// 0x048
	BYTE op;				// 0x054
	BYTE opparam;			// 0x055
	BYTE _4[0xEE];			// 0x056
};							// 0x144

struct ItemTypeTxt
{
	CHAR code[4];			// 0x00
	WORD Equiv1;			// 0x04
	WORD Equiv2;			// 0x06
	CHAR _1[0x19];			// 0x08
	BYTE Class;				// 0x21
	BYTE StorePage;			// 0x22
	BYTE VarInvGfx;			// 0x23
	CHAR InvGfx[6][0x20];	// 0x24
};							// 0xE4

struct SkillTxt
{
	DWORD id;				// 0x000
	BYTE _1[0x8];			// 0x004
	DWORD charclass;		// 0x00C 255 = all
	BYTE _2[0x184];			// 0x010
	WORD skilldesc;			// 0x194
	BYTE _3[0xA6];			// 0x196
};							// 0x23C

struct SkillDescTxt
{
	BYTE _1[8];				// 0x000
	WORD strname;			// 0x008
	WORD strshort;			// 0x00A
	WORD strlong;			// 0x00C
	WORD stralt;			// 0x00E
	BYTE _2[0x110];			// 0x010
};							// 0x120

struct GemTxt
{
	CHAR name[0x20];		// 0x00
	CHAR letter[0x8];		// 0x20
	DWORD code;				// 0x28
	BYTE _1;				// 0x2C
	BYTE _2;				// 0x2D
	BYTE nummods;			// 0x2E
	BYTE transform;			// 0x2F
	UniqueStats prosp[3][3];// 0x30
};							// 0xC0

struct CharStatsTxt
{
	BYTE _1[0x20];			// 0x00
	CHAR name[0x10];		// 0x20
	BYTE str;				// 0x30
	BYTE dex;				// 0x31
	BYTE Int;				// 0x32
	BYTE vit;				// 0x33
	BYTE stamina;			// 0x34
	BYTE hpadd;				// 0x35
	BYTE PercentStr;		// 0x36
	BYTE PercentInt;		// 0x37
	BYTE PercentDex;		// 0x38
	BYTE PercentVit;		// 0x39
	BYTE ManaRegen;			// 0x3A
	BYTE _2;				// 0x3B
	CHAR ToHitFactor;		// 0x3C
	CHAR _3;				// 0x3D
	CHAR _4;				// 0x3E
	CHAR _5;				// 0x3F
	BYTE WalkVelocity;		// 0x40
	BYTE RunVelocity;		// 0x41
	BYTE RunDrain;			// 0x42
	BYTE LifePerLevel;		// 0x43
	BYTE StaminaPerLevel;	// 0x44
	BYTE ManaPerLevel;		// 0x45
	BYTE LifePerVitality;	// 0x46
	BYTE StaminaPerVitality;// 0x47
	BYTE ManaPerMagic;		// 0x48
	BYTE BlockFactor;		// 0x49
	WORD _6;				// 0x4A
	CHAR baseWClass[4];		// 0x4C
	WORD StatPerLevel;		// 0x50
	WORD StrAllSkills;		// 0x52
	WORD StrSkillTab[3];	// 0x54
	WORD StrClassOnly;		// 0x5A
	struct BaseItem
	{
		CHAR code[4];
		BYTE loc;
		BYTE count;
		BYTE _1[2];
	};
	BaseItem items[10];		// 0x5C
	WORD StartSkill;		// 0xAC
	WORD Skill[10];			// 0xAE
	WORD _7;				// 0xC2
};							// 0xC4


struct MonStatsTxt
{
	WORD hcIdx;				// 0x000
	WORD BaseId;			// 0x002
	WORD NextInClass;		// 0x004
	WORD NameStr;			// 0x006
	WORD DescStr;			// 0x008
	BYTE _1[0x19E];			// 0x00A
};							// 0x1A8

struct GameStructInfo
{
	BYTE _1[0x1B];					//0x00
	char szGameName[0x18];			//0x1B
	char szGameServerIp[0x56];		//0x33
	char szAccountName[0x30];		//0x89
	char szCharName[0x18];			//0xB9
	char szRealmName[0x18];			//0xD1
	BYTE _2[0x158];					//0xE9
	char szGamePassword[0x18];		//0x241
};

struct PropertiesTxt {
	CHAR tx[16];
	BYTE _1[4];

};

#endif
