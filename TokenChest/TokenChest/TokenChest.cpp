//#include "vld.h"

//TODO weapon swap select
///DONE drag rect client area
///DONE ctrl + drag inversion
///DONE context menu item name search

//TODO trade depth finder
//TODO iso files
//TODO skill tree
//TODO quests
//TODO base stats/base res

//TODO color items (rares, uniques etc)
//TODO color eth items
//TODO draw sockets + gems in sockets on hover / selection

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <algorithm>
#include <Richedit.h>
#include <shellapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <ctime>
#include <functional>
#include <conio.h>

#include "include.h"
#include "Registry.h"
#include "detours.h"
#include "../TokenChestDll/Dll.h"
#include "common.h"
#include "resource.h"
#include "tabcontrolhandler.h"
#include "d2data.h"
#include "invview.h"
#include "listview.h"
#include "treeview.h"
#include "statview.h"
#include "inject.h"
#include "trade.h"
#include "iso.h"

#ifdef _DEBUG
#include <conio.h>
#define GETCH() _getch();
#else
#define GETCH()
#endif

#define STW(s) str_to_wstr(s).c_str()

//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WIN32_LEAN_AND_MEAN

using namespace std;
using namespace Gdiplus;

VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

HINSTANCE g_hInst;

HHOOK kbhook;

HWND g_finder = NULL;

TabClass g_TAB;
TabClass g_Tradetab;
HWND g_ttab3;
HWND g_ttab3_1;

HMENU g_menu;

NOTIFYICONDATA g_notifyIconData;

int g_is_auto_log = false;

HFONT g_boldfont = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"MS Shell Dlg");

std::vector<ItemCode> g_itemcodes;
std::vector<CharacterData> g_characters;
//realm     //account   //character //store     //items
map<string, map<string, map<string, map<string, vector<ItemData*>>>>> g_realms;//generates Warning: C4503

namespace itemcodes {
	std::vector<ItemCode> itemcodes = {
		{"Armor", "Helms", "Cap", "cap", 1}, {"Armor", "Helms", "War Hat", "xap", 2}, {"Armor", "Helms", "Shako", "uap", 3},
		{"Armor", "Helms", "Skull Cap", "skp", 1}, {"Armor", "Helms", "Sallet", "xkp", 2}, {"Armor", "Helms", "Hydraskull", "ukp", 3},
		{"Armor", "Helms", "Helm", "hlm", 1}, {"Armor", "Helms", "Casque", "xlm", 2}, {"Armor", "Helms", "Armet", "ulm", 3},
		{"Armor", "Helms", "Full Helm", "fhl", 1}, {"Armor", "Helms", "Basinet", "xhl", 2}, {"Armor", "Helms", "Giant Conch", "uhl", 3},
		{"Armor", "Helms", "Great Helm", "ghm", 1}, {"Armor", "Helms", "Winged Helm", "xhm", 2}, {"Armor", "Helms", "Spired Helm", "uhm", 3},
		{"Armor", "Helms", "Crown", "crn", 1}, {"Armor", "Helms", "Grand Crown", "xrn", 2}, {"Armor", "Helms", "Corona", "urn", 3},
		{"Armor", "Helms", "Mask", "msk", 1}, {"Armor", "Helms", "Death Mask", "xsk", 2}, {"Armor", "Helms", "Demonhead", "usk", 3},
		{"Armor", "Helms", "Bone Helm", "bhm", 1}, {"Armor", "Helms", "Grim Helm", "xh9", 2}, {"Armor", "Helms", "Bone Visage", "uh9", 3},
		{"Armor", "Helms", "Circlet", "ci0", 1}, {"Armor", "Helms", "Coronet", "ci1", 1}, {"Armor", "Helms", "Tiara", "ci2", 2}, {"Armor", "Helms", "Diadem", "ci3", 3},
		{"Armor", "Body Armor", "Quilted Armor", "qui", 1}, {"Armor", "Body Armor", "Ghost Armor", "xui", 2}, {"Armor", "Body Armor", "Dusk Shroud", "uui", 3},
		{"Armor", "Body Armor", "Leather Armor", "lea", 1}, {"Armor", "Body Armor", "Serpentskin", "xea", 2}, {"Armor", "Body Armor", "Wyrmhide", "uea", 3},
		{"Armor", "Body Armor", "Hard Leather", "hla", 1}, {"Armor", "Body Armor", "Demonhide Armor", "xla", 2}, {"Armor", "Body Armor", "Scarab Husk", "ula", 3},
		{"Armor", "Body Armor", "Studded Leather", "stu", 1}, {"Armor", "Body Armor", "Trellised Armor", "xtu", 2}, {"Armor", "Body Armor", "Wire Fleece", "utu", 3},
		{"Armor", "Body Armor", "Ring Mail", "rng", 1}, {"Armor", "Body Armor", "Linked Mail", "xng", 2}, {"Armor", "Body Armor", "Diamond Mail", "ung", 3},
		{"Armor", "Body Armor", "Scale Mail", "scl", 1}, {"Armor", "Body Armor", "Tigulated Mail", "xcl", 2}, {"Armor", "Body Armor", "Loricated Mail", "ucl", 3},
		{"Armor", "Body Armor", "Chain Mail", "chn", 1}, {"Armor", "Body Armor", "Mesh Armor", "xhn", 2}, {"Armor", "Body Armor", "Boneweave", "uhn", 3},
		{"Armor", "Body Armor", "Breast Plate", "brs", 1}, {"Armor", "Body Armor", "Cuirass", "xrs", 2}, {"Armor", "Body Armor", "Great Hauberk", "urs", 3},
		{"Armor", "Body Armor", "Splint Mail", "spl", 1}, {"Armor", "Body Armor", "Russet Armor", "xpl", 2}, {"Armor", "Body Armor", "Balrog Skin", "upl", 3},
		{"Armor", "Body Armor", "Plate Mail", "plt", 1}, {"Armor", "Body Armor", "Templar Coat", "xlt", 2}, {"Armor", "Body Armor", "Hellforge Plate", "ult", 3},
		{"Armor", "Body Armor", "Field Plate", "fld", 1}, {"Armor", "Body Armor", "Sharktooth", "xld", 2}, {"Armor", "Body Armor", "Kraken Shell", "uld", 3},
		{"Armor", "Body Armor", "Gothic Plate", "gth", 1}, {"Armor", "Body Armor", "Embossed Plate", "xth", 2}, {"Armor", "Body Armor", "Lacquered Plate", "uth", 3},
		{"Armor", "Body Armor", "Full Plate Mail", "ful", 1}, {"Armor", "Body Armor", "Chaos Armor", "xul", 2}, {"Armor", "Body Armor", "Shadow Plate", "uul", 3},
		{"Armor", "Body Armor", "Ancient Armor", "aar", 1}, {"Armor", "Body Armor", "Ornate Armor", "xar", 2}, {"Armor", "Body Armor", "Sacred Armor", "uar", 3},
		{"Armor", "Body Armor", "Light Plate", "ltp", 1}, {"Armor", "Body Armor", "Mage Plate", "xtp", 2}, {"Armor", "Body Armor", "Archon Plate", "utp", 3},
		{"Armor", "Shields", "Buckler", "buc", 1}, {"Armor", "Shields", "Defender", "xuc", 2}, {"Armor", "Shields", "Heater", "uuc", 3},
		{"Armor", "Shields", "Small Shield", "sml", 1}, {"Armor", "Shields", "Round Shield", "xml", 2}, {"Armor", "Shields", "Luna", "uml", 3},
		{"Armor", "Shields", "Large Shield", "lrg", 1}, {"Armor", "Shields", "Scutum", "xrg", 2}, {"Armor", "Shields", "Hyperion", "urg", 3},
		{"Armor", "Shields", "Kite Shield", "kit", 1}, {"Armor", "Shields", "Dragon Shield", "xit", 2}, {"Armor", "Shields", "Monarch", "uit", 3},
		{"Armor", "Shields", "Tower Shield", "tow", 1}, {"Armor", "Shields", "Pavise", "xow", 2}, {"Armor", "Shields", "Aegis", "uow", 3},
		{"Armor", "Shields", "Gothic Shield", "gts", 1}, {"Armor", "Shields", "Ancient Shield", "xts", 2}, {"Armor", "Shields", "Ward", "uts", 3},
		{"Armor", "Shields", "Bone Shield", "bsh", 1}, {"Armor", "Shields", "Grim Shield", "xsh", 2}, {"Armor", "Shields", "Troll Nest", "ush", 3},
		{"Armor", "Shields", "Spiked Shield", "spk", 1}, {"Armor", "Shields", "Barbed Shield", "xpk", 2}, {"Armor", "Shields", "Blade Barrier", "upk", 3},
		{"Armor", "Gloves", "Leather Gloves", "lgl", 1}, {"Armor", "Gloves", "Demonhide Gloves", "xlg", 2}, {"Armor", "Gloves", "Bramble Mitts", "ulg", 3},
		{"Armor", "Gloves", "Heavy Gloves", "vgl", 1}, {"Armor", "Gloves", "Sharkskin Gloves", "xvg", 2}, {"Armor", "Gloves", "Vampirebone Gloves", "uvg", 3},
		{"Armor", "Gloves", "Chain Gloves", "mgl", 1}, {"Armor", "Gloves", "Heavy Bracers", "xmg", 2}, {"Armor", "Gloves", "Vambraces", "umg", 3},
		{"Armor", "Gloves", "Light Gauntlets", "tgl", 1}, {"Armor", "Gloves", "Battle Gauntlet", "xtg", 2}, {"Armor", "Gloves", "Crusader Gauntlets", "utg", 3},
		{"Armor", "Gloves", "Gauntlets", "hgl", 1}, {"Armor", "Gloves", "War Gauntlets", "xhg", 2}, {"Armor", "Gloves", "Ogre Gauntlets", "uhg", 3},
		{"Armor", "Boots", "Boots", "lbt", 1}, {"Armor", "Boots", "Demonhide Boots", "xlb", 2}, {"Armor", "Boots", "Wyrmhide Boots", "ulb", 3},
		{"Armor", "Boots", "Heavy Boots", "vbt", 1}, {"Armor", "Boots", "Sharkskin Boots", "xvb", 2}, {"Armor", "Boots", "Scarabshell Boots", "uvb", 3},
		{"Armor", "Boots", "Chain Boots", "mbt", 1}, {"Armor", "Boots", "Mesh Boots", "xmb", 2}, {"Armor", "Boots", "Boneweave Boots", "umb", 3},
		{"Armor", "Boots", "Light Plate", "tbt", 1}, {"Armor", "Boots", "Battle Boots", "xtb", 2}, {"Armor", "Boots", "Mirrored Boots", "utb", 3},
		{"Armor", "Boots", "Greaves", "hbt", 1}, {"Armor", "Boots", "War Boots", "xhb", 2}, {"Armor", "Boots", "Myrmidon Greave", "uhb", 3},
		{"Armor", "Belts", "Sash", "lbl", 1}, {"Armor", "Belts", "Demonhide Sash", "zlb", 2}, {"Armor", "Belts", "Spiderweb Sash", "ulc", 3},
		{"Armor", "Belts", "Light Belt", "vbl", 1}, {"Armor", "Belts", "Sharkskin Belt", "zvb", 2}, {"Armor", "Belts", "Vampirefang Blt", "uvc", 3},
		{"Armor", "Belts", "Belt", "mbl", 1}, {"Armor", "Belts", "Mesh Belt", "zmb", 2}, {"Armor", "Belts", "Mithril Coil", "umc", 3},
		{"Armor", "Belts", "Heavy Belt", "tbl", 1}, {"Armor", "Belts", "Battle Belt", "ztb", 2}, {"Armor", "Belts", "Troll Belt", "utc", 3},
		{"Armor", "Belts", "Plated Belt", "hbl", 1}, {"Armor", "Belts", "War Belt", "zhb", 2}, {"Armor", "Belts", "Colossus Girdle", "uhc", 3},
		{"Armor", "Druid Pelts", "Wolf Head", "dr1", 1}, {"Armor", "Druid Pelts", "Alpha Helm", "dr6", 2}, {"Armor", "Druid Pelts", "Blood Spirit", "drb", 3},
		{"Armor", "Druid Pelts", "Hawk Helm", "dr2", 1}, {"Armor", "Druid Pelts", "Griffon Headress", "dr7", 2}, {"Armor", "Druid Pelts", "Sun Spirit", "drc", 3},
		{"Armor", "Druid Pelts", "Antlers", "dr3", 1}, {"Armor", "Druid Pelts", "Hunter's Guise", "dr8", 2}, {"Armor", "Druid Pelts", "Earth Spirit", "drd", 3},
		{"Armor", "Druid Pelts", "Falcon Mask", "dr4", 1}, {"Armor", "Druid Pelts", "Sacred Feathers", "dr9", 2}, {"Armor", "Druid Pelts", "Sky Spirit", "dre", 3},
		{"Armor", "Druid Pelts", "Spirit Mask", "dr5", 1}, {"Armor", "Druid Pelts", "Totemic Mask", "dra", 2}, {"Armor", "Druid Pelts", "Dream Spirit", "drf", 3},
		{"Armor", "Barbarian Helms", "Jawbone Cap", "ba1", 1}, {"Armor", "Barbarian Helms", "Jawbone Visor", "ba6", 2}, {"Armor", "Barbarian Helms", "Carnage Helm", "bab", 3},
		{"Armor", "Barbarian Helms", "Fanged Helm", "ba2", 1}, {"Armor", "Barbarian Helms", "Lion Helm", "ba7", 2}, {"Armor", "Barbarian Helms", "Fury Visor", "bac", 3},
		{"Armor", "Barbarian Helms", "Horned Helm", "ba3", 1}, {"Armor", "Barbarian Helms", "Rage Mask", "ba8", 2}, {"Armor", "Barbarian Helms", "Destroyer Helm", "bad", 3},
		{"Armor", "Barbarian Helms", "Assualt Helmet", "ba4", 1}, {"Armor", "Barbarian Helms", "Savage Helmet", "ba9", 2}, {"Armor", "Barbarian Helms", "Conqueror Crown", "bae", 3},
		{"Armor", "Barbarian Helms", "Avenger Guard", "ba5", 1}, {"Armor", "Barbarian Helms", "Slayer Guard", "baa", 2}, {"Armor", "Barbarian Helms", "Guardian Crown", "baf", 3},
		{"Armor", "Paladin Shields", "Targe", "pa1", 1}, {"Armor", "Paladin Shields", "Akaran Targe", "pa6", 2}, {"Armor", "Paladin Shields", "Sacred Targe", "pab", 3},
		{"Armor", "Paladin Shields", "Rondache", "pa2", 1}, {"Armor", "Paladin Shields", "Akaran Rondache", "pa7", 2}, {"Armor", "Paladin Shields", "Sacred Rondache", "pac", 3},
		{"Armor", "Paladin Shields", "Heraldic Shield", "pa3", 1}, {"Armor", "Paladin Shields", "Protector Shld", "pa8", 2}, {"Armor", "Paladin Shields", "Ancient Shield", "pad", 3},
		{"Armor", "Paladin Shields", "Aerin Shield", "pa4", 1}, {"Armor", "Paladin Shields", "Guilded Shield", "pa9", 2}, {"Armor", "Paladin Shields", "Zakarum Shield", "pae", 3},
		{"Armor", "Paladin Shields", "Crown Shield", "pa5", 1}, {"Armor", "Paladin Shields", "Royal Shield", "paa", 2}, {"Armor", "Paladin Shields", "Vortex Shield", "paf", 3},
		{"Armor", "Necromancer Shrunken Heads", "Preserved Head", "ne1", 1}, {"Armor", "Necromancer Shrunken Heads", "Mummified Trphy", "ne6", 2}, {"Armor", "Necromancer Shrunken Heads", "Minion Skull", "neb", 3},
		{"Armor", "Necromancer Shrunken Heads", "Zombie Head", "ne2", 1}, {"Armor", "Necromancer Shrunken Heads", "Fetish Trophy", "ne7", 2}, {"Armor", "Necromancer Shrunken Heads", "Hellspawn Skull", "neg", 3},
		{"Armor", "Necromancer Shrunken Heads", "Unraveller Head", "ne3", 1}, {"Armor", "Necromancer Shrunken Heads", "Sexton Trophy", "ne8", 2}, {"Armor", "Necromancer Shrunken Heads", "Overseer Skull", "ned", 3},
		{"Armor", "Necromancer Shrunken Heads", "Gargoyle Head", "ne4", 1}, {"Armor", "Necromancer Shrunken Heads", "Cantor Trophy", "ne9", 2}, {"Armor", "Necromancer Shrunken Heads", "Succubae Skull", "nee", 3},
		{"Armor", "Necromancer Shrunken Heads", "Demon Head", "ne5", 1}, {"Armor", "Necromancer Shrunken Heads", "Heirophant Trphy", "nea", 2}, {"Armor", "Necromancer Shrunken Heads", "Bloodlord Skull", "nef", 3},
		{"Weapons", "Axes", "Hand Axe", "hax", 1}, {"Weapons", "Axes", "Hatchet", "9ha", 2}, {"Weapons", "Axes", "Tomahawk", "7ha", 3},
		{"Weapons", "Axes", "Axe", "axe", 1}, {"Weapons", "Axes", "Cleaver", "9ax", 2}, {"Weapons", "Axes", "Small Crescent", "7ax", 3},
		{"Weapons", "Axes", "Double Axe", "2ax", 1}, {"Weapons", "Axes", "Twin Axe", "92a", 2}, {"Weapons", "Axes", "Ettin Axe", "72a", 3},
		{"Weapons", "Axes", "Military Pick", "mpi", 1}, {"Weapons", "Axes", "Crowbill", "9mp", 2}, {"Weapons", "Axes", "War Spike", "7mp", 3},
		{"Weapons", "Axes", "War Axe", "wax", 1}, {"Weapons", "Axes", "Naga", "9wa", 2}, {"Weapons", "Axes", "Berserker Axe", "7wa", 3},
		{"Weapons", "Axes", "Large Axe", "lax", 1}, {"Weapons", "Axes", "Military Axe", "9la", 2}, {"Weapons", "Axes", "Feral Axe", "7la", 3},
		{"Weapons", "Axes", "Broad Axe", "bax", 1}, {"Weapons", "Axes", "Bearded Axe", "9ba", 2}, {"Weapons", "Axes", "Silver Edged Ax", "7ba", 3},
		{"Weapons", "Axes", "Battle Axe", "btx", 1}, {"Weapons", "Axes", "Tabar", "9bt", 2}, {"Weapons", "Axes", "Decapitator", "7bt", 3},
		{"Weapons", "Axes", "Great Axe", "gax", 1}, {"Weapons", "Axes", "Gothic Axe", "9ga", 2}, {"Weapons", "Axes", "Champion Axe", "7ga", 3},
		{"Weapons", "Axes", "Giant Axe", "gix", 1}, {"Weapons", "Axes", "Ancient Axe", "9gi", 2}, {"Weapons", "Axes", "Glorious Axe", "7gi", 3},
		{"Weapons", "Maces", "Club", "clb", 1}, {"Weapons", "Maces", "Cudgel", "9cl", 2}, {"Weapons", "Maces", "Truncheon", "7cl", 3},
		{"Weapons", "Maces", "Spiked Club", "spc", 1}, {"Weapons", "Maces", "Barbed Club", "9sp", 2}, {"Weapons", "Maces", "Tyrant Club", "7sp", 3},
		{"Weapons", "Maces", "Mace", "mac", 1}, {"Weapons", "Maces", "Flanged Mace", "9ma", 2}, {"Weapons", "Maces", "Reinforced Mace", "7ma", 3},
		{"Weapons", "Maces", "Morning Star", "mst", 1}, {"Weapons", "Maces", "Jagged Star", "9mt", 2}, {"Weapons", "Maces", "Devil Star", "7mt", 3},
		{"Weapons", "Maces", "Flail", "fla", 1}, {"Weapons", "Maces", "Knout", "9fl", 2}, {"Weapons", "Maces", "Scourge", "7fl", 3},
		{"Weapons", "Maces", "War Hammer", "whm", 1}, {"Weapons", "Maces", "Battle Hammer", "9wh", 2}, {"Weapons", "Maces", "Legendary Mallt", "7wh", 3},
		{"Weapons", "Maces", "Maul", "mau", 1}, {"Weapons", "Maces", "War Club", "9m9", 2}, {"Weapons", "Maces", "Ogre Maul", "7m7", 3},
		{"Weapons", "Maces", "Great Maul", "gma", 1}, {"Weapons", "Maces", "Martelde Fer", "9gm", 2}, {"Weapons", "Maces", "Thunder Maul", "7gm", 3},
		{"Weapons", "Swords", "Short Swrod", "ssd", 1}, {"Weapons", "Swords", "Gladius", "9ss", 2}, {"Weapons", "Swords", "Falcata", "7ss", 3},
		{"Weapons", "Swords", "Scimitar", "scm", 1}, {"Weapons", "Swords", "Cutlass", "9sm", 2}, {"Weapons", "Swords", "Ataghan", "7sm", 3},
		{"Weapons", "Swords", "Saber", "sbr", 1}, {"Weapons", "Swords", "Shamshir", "9sb", 2}, {"Weapons", "Swords", "Elegant Blade", "7sb", 3},
		{"Weapons", "Swords", "Falchion", "flc", 1}, {"Weapons", "Swords", "Tulwar", "9fc", 2}, {"Weapons", "Swords", "Hydra Edge", "7fc", 3},
		{"Weapons", "Swords", "Crystal Sword", "crs", 1}, {"Weapons", "Swords", "Dimensional Blade", "9cr", 2}, {"Weapons", "Swords", "Phase Blade", "7cr", 3},
		{"Weapons", "Swords", "Broad Sword", "bsd", 1}, {"Weapons", "Swords", "Battle Sword", "9bs", 2}, {"Weapons", "Swords", "Conquest Sword", "7bs", 3},
		{"Weapons", "Swords", "Long Sword", "lsd", 1}, {"Weapons", "Swords", "Rune Sword", "9ls", 2}, {"Weapons", "Swords", "Cryptic Sword", "7ls", 3},
		{"Weapons", "Swords", "War Sword", "wsd", 1}, {"Weapons", "Swords", "Ancient Sword", "9wd", 2}, {"Weapons", "Swords", "Mythical Sword", "7wd", 3},
		{"Weapons", "Swords", "Two-handed Swrd", "2hs", 1}, {"Weapons", "Swords", "Espadon", "92h", 2}, {"Weapons", "Swords", "Legend Sword", "72h", 3},
		{"Weapons", "Swords", "Claymore", "clm", 1}, {"Weapons", "Swords", "Dacian Falx", "9cm", 2}, {"Weapons", "Swords", "Highland Blade", "7cm", 3},
		{"Weapons", "Swords", "Giant Sword", "gis", 1}, {"Weapons", "Swords", "Tusk Sword", "9gs", 2}, {"Weapons", "Swords", "Balrog Blade", "7gs", 3},
		{"Weapons", "Swords", "Bastard Sword", "bsw", 1}, {"Weapons", "Swords", "Gothic Sword", "9b9", 2}, {"Weapons", "Swords", "Champion Sword", "7b7", 3},
		{"Weapons", "Swords", "Flamberge", "flb", 1}, {"Weapons", "Swords", "Zweihander", "9fb", 2}, {"Weapons", "Swords", "Colossal Sword", "7fb", 3},
		{"Weapons", "Swords", "Great Sword", "gsd", 1}, {"Weapons", "Swords", "Executioner Sword", "9gd", 2}, {"Weapons", "Swords", "Colossus Blade", "7gd", 3},
		{"Weapons", "Daggers", "Dagger", "dgr", 1}, {"Weapons", "Daggers", "Poignard", "9dg", 2}, {"Weapons", "Daggers", "Bone Knife", "7dg", 3},
		{"Weapons", "Daggers", "Dirk", "dir", 1}, {"Weapons", "Daggers", "Rondel", "9di", 2}, {"Weapons", "Daggers", "Mithral Point", "7di", 3},
		{"Weapons", "Daggers", "Kriss", "kri", 1}, {"Weapons", "Daggers", "Cinquedeas", "9kr", 2}, {"Weapons", "Daggers", "Fanged Knife", "7kr", 3},
		{"Weapons", "Daggers", "Blade", "bld", 1}, {"Weapons", "Daggers", "Stilleto", "9bl", 2}, {"Weapons", "Daggers", "Legend Spike", "7bl", 3},
		{"Weapons", "Throwing", "Throwing Knife", "tkf", 1}, {"Weapons", "Throwing", "Battle Dart", "9tk", 2}, {"Weapons", "Throwing", "Flying Knife", "7tk", 3},
		{"Weapons", "Throwing", "Throwing Axe", "tax", 1}, {"Weapons", "Throwing", "Francisca", "9ta", 2}, {"Weapons", "Throwing", "Flying Axe", "7ta", 3},
		{"Weapons", "Throwing", "Strangling Gas Potion", "gpl", 1}, {"Weapons", "Throwing", "Choking Gas Potion", "gpm", 2}, {"Weapons", "Throwing", "Rancid Gas Potion", "gps", 3},
		{"Weapons", "Throwing", "Fulminating Potion", "opl", 1}, {"Weapons", "Throwing", "Exploding Potion", "opm", 2}, {"Weapons", "Throwing", "Oil Potion", "ops", 3},
		{"Weapons", "Throwing", "Balanced Knife", "bkf", 1}, {"Weapons", "Throwing", "War Dart", "9bk", 2}, {"Weapons", "Throwing", "Winged Knife", "7bk", 3},
		{"Weapons", "Throwing", "Balanced Axe", "bal", 1}, {"Weapons", "Throwing", "Hurlbat", "9b8", 2}, {"Weapons", "Throwing", "Winged Axe", "7b8", 3},
		{"Weapons", "Javelins", "Javelin", "jav", 1}, {"Weapons", "Javelins", "War Javelin", "9ja", 2}, {"Weapons", "Javelins", "Hyperion Javeln", "7ja", 3},
		{"Weapons", "Javelins", "Pilum", "pil", 1}, {"Weapons", "Javelins", "Great Pilum", "9pi", 2}, {"Weapons", "Javelins", "Stygian Pilum", "7pi", 3},
		{"Weapons", "Javelins", "Short Spear", "ssp", 1}, {"Weapons", "Javelins", "Simbilan", "9s9", 2}, {"Weapons", "Javelins", "Balrog Spear", "7s7", 3},
		{"Weapons", "Javelins", "Glaive", "glv", 1}, {"Weapons", "Javelins", "Spiculum", "9gl", 2}, {"Weapons", "Javelins", "Ghost Glaive", "7gl", 3},
		{"Weapons", "Javelins", "Throwing Spear", "tsp", 1}, {"Weapons", "Javelins", "Harpoon", "9ts", 2}, {"Weapons", "Javelins", "Winged Harpoon", "7ts", 3},
		{"Weapons", "Spears", "Spear", "spr", 1}, {"Weapons", "Spears", "War Spear", "9sr", 2}, {"Weapons", "Spears", "Hyperion Spear", "7sr", 3},
		{"Weapons", "Spears", "Trident", "tri", 1}, {"Weapons", "Spears", "Fuscina", "9tr", 2}, {"Weapons", "Spears", "Stygian Pike", "7tr", 3},
		{"Weapons", "Spears", "Brandistock", "brn", 1}, {"Weapons", "Spears", "War Fork", "9br", 2}, {"Weapons", "Spears", "Mancatcher", "7br", 3},
		{"Weapons", "Spears", "Spetum", "spt", 1}, {"Weapons", "Spears", "Yari", "9st", 2}, {"Weapons", "Spears", "Ghost Spear", "7st", 3},
		{"Weapons", "Spears", "Pike", "pik", 1}, {"Weapons", "Spears", "Lance", "9p9", 2}, {"Weapons", "Spears", "War Pike", "7p7", 3},
		{"Weapons", "Polearms", "Bardiche", "bar", 1}, {"Weapons", "Polearms", "Lochaber Axe", "9b7", 2}, {"Weapons", "Polearms", "Ogre Axe", "7o7", 3},
		{"Weapons", "Polearms", "Voulge", "vou", 1}, {"Weapons", "Polearms", "Bill", "9vo", 2}, {"Weapons", "Polearms", "Colossus Voulge", "7vo", 3},
		{"Weapons", "Polearms", "Scythe", "scy", 1}, {"Weapons", "Polearms", "Battle Scythe", "9s8", 2}, {"Weapons", "Polearms", "Thresher", "7s8", 3},
		{"Weapons", "Polearms", "Poleaxe", "pax", 1}, {"Weapons", "Polearms", "Partizan", "9pa", 2}, {"Weapons", "Polearms", "Cryptic Axe", "7pa", 3},
		{"Weapons", "Polearms", "Halberd", "hal", 1}, {"Weapons", "Polearms", "Bec-de-Corbin", "9h9", 2}, {"Weapons", "Polearms", "Great Poleaxe", "7h7", 3},
		{"Weapons", "Polearms", "War Scythe", "wsc", 1}, {"Weapons", "Polearms", "Grim Scythe", "9wc", 2}, {"Weapons", "Polearms", "Giant Thresher", "7wc", 3},
		{"Weapons", "Bows", "Short Bow", "sbw", 1}, {"Weapons", "Bows", "Edge Bow", "8sb", 2}, {"Weapons", "Bows", "Spider Bow", "6sb", 3},
		{"Weapons", "Bows", "Hunter's Bow", "hbw", 1}, {"Weapons", "Bows", "Razor Bow", "8hb", 2}, {"Weapons", "Bows", "Blade Bow", "6hb", 3},
		{"Weapons", "Bows", "Long Bow", "lbw", 1}, {"Weapons", "Bows", "Cedar Bow", "8lb", 2}, {"Weapons", "Bows", "Shadow Bow", "6lb", 3},
		{"Weapons", "Bows", "Composite Bow", "cbw", 1}, {"Weapons", "Bows", "Double Bow", "8cb", 2}, {"Weapons", "Bows", "Great Bow", "6cb", 3},
		{"Weapons", "Bows", "Short Battle Bow", "sbb", 1}, {"Weapons", "Bows", "Short Siege Bow", "8s8", 2}, {"Weapons", "Bows", "Diamond Bow", "6s7", 3},
		{"Weapons", "Bows", "Long Battle Bow", "lbb", 1}, {"Weapons", "Bows", "Long Siege Bow", "8l8", 2}, {"Weapons", "Bows", "Crusader Bow", "6l7", 3},
		{"Weapons", "Bows", "Short War Bow", "swb", 1}, {"Weapons", "Bows", "Rune Bow", "8sw", 2}, {"Weapons", "Bows", "Ward Bow", "6sw", 3},
		{"Weapons", "Bows", "Long War Bow", "lwb", 1}, {"Weapons", "Bows", "Gothic Bow", "8lw", 2}, {"Weapons", "Bows", "Hydra Bow", "6lw", 3},
		{"Weapons", "Crossbows", "Light Crossbow", "lxb", 1}, {"Weapons", "Crossbows", "Arbalest", "8lx", 2}, {"Weapons", "Crossbows", "Pellet Bow", "6lx", 3},
		{"Weapons", "Crossbows", "Crossbow", "mxb", 1}, {"Weapons", "Crossbows", "Siege Crossbow", "8mx", 2}, {"Weapons", "Crossbows", "Gorgon Crossbow", "6mx", 3},
		{"Weapons", "Crossbows", "Heavy Crossbow", "hxb", 1}, {"Weapons", "Crossbows", "Ballista", "8hx", 2}, {"Weapons", "Crossbows", "Colossus Crossbow", "6hx", 3},
		{"Weapons", "Crossbows", "Repeating Crossbow", "rxb", 1}, {"Weapons", "Crossbows", "Chu-Ko-Nu", "8rx", 2}, {"Weapons", "Crossbows", "Demon Crossbow", "6rx", 3},
		{"Weapons", "Staves", "Short Staff", "sst", 1}, {"Weapons", "Staves", "Jo Staff", "8ss", 2}, {"Weapons", "Staves", "Walking Stick", "6ss", 3},
		{"Weapons", "Staves", "Long Staff", "lst", 1}, {"Weapons", "Staves", "Quarterstaff", "8ls", 2}, {"Weapons", "Staves", "Stalagmite", "6ls", 3},
		{"Weapons", "Staves", "Gnarled Staff", "cst", 1}, {"Weapons", "Staves", "Cedar Staff", "8cs", 2}, {"Weapons", "Staves", "Elder Staff", "6cs", 3},
		{"Weapons", "Staves", "Battle Staff", "bst", 1}, {"Weapons", "Staves", "Gothic Staff", "8bs", 2}, {"Weapons", "Staves", "Shillelagh", "6bs", 3},
		{"Weapons", "Staves", "War Staff", "wst", 1}, {"Weapons", "Staves", "Rune Staff", "8ws", 2}, {"Weapons", "Staves", "Archon Staff", "6ws", 3},
		{"Weapons", "Wands", "Wand", "wnd", 1}, {"Weapons", "Wands", "Burnt Wand", "9wn", 2}, {"Weapons", "Wands", "Polished Wand", "7wn", 3},
		{"Weapons", "Wands", "Yew Wand", "ywn", 1}, {"Weapons", "Wands", "Petrified Wand", "9yw", 2}, {"Weapons", "Wands", "Ghost Wand", "7yw", 3},
		{"Weapons", "Wands", "Bone Wand", "bwn", 1}, {"Weapons", "Wands", "Tomb Wand", "9bw", 2}, {"Weapons", "Wands", "Lich Wand", "7bw", 3},
		{"Weapons", "Wands", "Grim Wand", "gwn", 1}, {"Weapons", "Wands", "Grave Wand", "9gw", 2}, {"Weapons", "Wands", "Unearthed Wand", "7gw", 3},
		{"Weapons", "Scepters", "Scepter", "scp", 1}, {"Weapons", "Scepters", "Rune Scepter", "9sc", 2}, {"Weapons", "Scepters", "Mighty Scepter", "7sc", 3},
		{"Weapons", "Scepters", "Grand Scepter", "gsc", 1}, {"Weapons", "Scepters", "Holy Water Sprinkler", "9qs", 2}, {"Weapons", "Scepters", "Seraph Rod", "7qs", 3},
		{"Weapons", "Scepters", "War Scepter", "wsp", 1}, {"Weapons", "Scepters", "Divine Scepter", "9ws", 2}, {"Weapons", "Scepters", "Caduceus", "7ws", 3},
		{"Weapons", "Assassin Katars", "Katar", "ktr", 1}, {"Weapons", "Assassin Katars", "Quhab", "9ar", 2}, {"Weapons", "Assassin Katars", "Suwayyah", "7ar", 3},
		{"Weapons", "Assassin Katars", "Wrist Blade", "wrb", 1}, {"Weapons", "Assassin Katars", "Wrist Spike", "9wb", 2}, {"Weapons", "Assassin Katars", "Wrist Sword", "7wb", 3},
		{"Weapons", "Assassin Katars", "Hatchet Hands", "axf", 1}, {"Weapons", "Assassin Katars", "Fascia", "9xf", 2}, {"Weapons", "Assassin Katars", "War Fist", "7xf", 3},
		{"Weapons", "Assassin Katars", "Cestus", "ces", 1}, {"Weapons", "Assassin Katars", "Hand Scythe", "9cs", 2}, {"Weapons", "Assassin Katars", "Battle Cestus", "7cs", 3},
		{"Weapons", "Assassin Katars", "Claws", "clw", 1}, {"Weapons", "Assassin Katars", "Greater Claws", "9lw", 2}, {"Weapons", "Assassin Katars", "Feral Claws", "7lw", 3},
		{"Weapons", "Assassin Katars", "Blade Talons", "btl", 1}, {"Weapons", "Assassin Katars", "Greater Talons", "9tw", 2}, {"Weapons", "Assassin Katars", "Runic Talons", "7tw", 3},
		{"Weapons", "Assassin Katars", "Scissors Katar", "skr", 1}, {"Weapons", "Assassin Katars", "Scissors Quhab", "9qr", 2}, {"Weapons", "Assassin Katars", "Scissors Suwayyah", "7qr", 3},
		{"Weapons", "Sorceress Orbs", "Eagle Orb", "ob1", 1}, {"Weapons", "Sorceress Orbs", "Glowing Orb", "ob6", 2}, {"Weapons", "Sorceress Orbs", "Heavenly Stone", "obb", 3},
		{"Weapons", "Sorceress Orbs", "Sacred Globe", "ob2", 1}, {"Weapons", "Sorceress Orbs", "Crystalline Globe", "ob7", 2}, {"Weapons", "Sorceress Orbs", "Eldritch Orb", "obc", 3},
		{"Weapons", "Sorceress Orbs", "Smoked Sphere", "ob3", 1}, {"Weapons", "Sorceress Orbs", "Cloudy Sphere", "ob8", 2}, {"Weapons", "Sorceress Orbs", "Demon Heart", "obd", 3},
		{"Weapons", "Sorceress Orbs", "Clasped Orb", "ob4", 1}, {"Weapons", "Sorceress Orbs", "Sparkling Ball", "ob9", 2}, {"Weapons", "Sorceress Orbs", "Vortex Orb", "obe", 3},
		{"Weapons", "Sorceress Orbs", "Dragon Stone", "ob5", 1}, {"Weapons", "Sorceress Orbs", "Swirling Crystal", "oba", 2}, {"Weapons", "Sorceress Orbs", "Dimensional Shard", "obf", 3},
		{"Weapons", "Amazon Weapons", "Stag Bow", "am1", 1}, {"Weapons", "Amazon Weapons", "Ashwood Bow", "am6", 2}, {"Weapons", "Amazon Weapons", "Matriarchal Bow", "amb", 3},
		{"Weapons", "Amazon Weapons", "Reflex Bow", "am2", 1}, {"Weapons", "Amazon Weapons", "Ceremonial Bow", "am7", 2}, {"Weapons", "Amazon Weapons", "Grand Matron Bow", "amc", 3},
		{"Weapons", "Amazon Weapons", "Maiden Spear", "am3", 1}, {"Weapons", "Amazon Weapons", "Ceremonial Spear", "am8", 2}, {"Weapons", "Amazon Weapons", "Matriarchal Spear", "amd", 3},
		{"Weapons", "Amazon Weapons", "Maiden Pike", "am4", 1}, {"Weapons", "Amazon Weapons", "Ceremonial Pike", "am9", 2}, {"Weapons", "Amazon Weapons", "Matriarchal Pike", "ame", 3},
		{"Weapons", "Amazon Weapons", "Maiden Javelin", "am5", 1}, {"Weapons", "Amazon Weapons", "Ceremonial Javalin", "ama", 2}, {"Weapons", "Amazon Weapons", "Matriarchal Javalin", "amf", 3},
		{"Misc", "Potion", "Healing Potion", "hpo", 0},
		{"Misc", "Potion", "Mana Potion", "mpo", 0},
		{"Misc", "Potion", "Full Healing Potion", "hpf", 0},
		{"Misc", "Potion", "Full Mana Potion", "mpf", 0},
		{"Misc", "Potion", "Stamina Potion", "vps", 0},
		{"Misc", "Potion", "Antidote Potion", "yps", 0},
		{"Misc", "Potion", "Rejuv Potion", "rvs", 0},
		{"Misc", "Potion", "Full Rejuv Potion", "rvl", 0},
		{"Misc", "Potion", "Thawing Potion", "wms", 0},
		{"Misc", "Potion", "Lesser Healing Potion", "hp1", 0}, {"Misc", "Potion", "Light Healing Potion", "hp2", 0}, {"Misc", "Potion", "Healing Potion", "hp3", 0}, {"Misc", "Potion", "Strong Healing Potion", "hp4", 0}, {"Misc", "Potion", "Greater Healing Potion", "hp5", 0},
		{"Misc", "Potion", "Lesser Mana Potion", "mp1", 0},	{"Misc", "Potion", "Light Mana Potion", "mp2", 0}, {"Misc", "Potion", "Mana Potion", "mp3", 0},	{"Misc", "Potion", "Strong Mana Potion", "mp4", 0},	{"Misc", "Potion", "Greater Mana Potion", "mp5", 0},
		{"Misc", "Potion", "elixir", "elx", 0},
		{"Misc", "Other", "ring", "rin", 0},
		{"Misc", "Other", "amulet", "amu", 0},
		{"Misc", "Other", "viperamulet", "vip", 0},
		{"Misc", "Other", "Town Portal Book", "tbk", 0},
		{"Misc", "Other", "Identify Book", "ibk", 0},
		{"Misc", "Other", "Torch", "tch", 0},
		{"Misc", "Other", "Scroll", "0sc", 0},
		{"Misc", "Other", "Scroll of Town Portal", "tsc", 0},
		{"Misc", "Other", "Scroll of Identify", "isc", 0},
		{"Misc", "Other", "Arrows", "aqv", 0},
		{"Misc", "Other", "Bolts", "cqv", 0},
		{"Misc", "Other", "herb", "hrb", 0},
		{"Misc", "Other", "Jewel", "jew", 0},
		{"Misc", "Other", "Charm Small", "cm1", 0}, {"Misc", "Other", "Charm Medium", "cm2", 0}, {"Misc", "Other", "Charm Large", "cm3", 0},
		{"Misc", "Other", "Player Ear", "ear", 0},
		{"Misc", "Other", "Skeleton Key", "key", 0},
		{"Misc", "Other", "Mephisto Key", "luv", 0},
		{"Misc", "Gems", "Chipped Amethyst", "gcv", 0},	{"Misc", "Gems", "Flawed Amethyst", "gfv", 0},	{"Misc", "Gems", "Amethyst", "gsv", 0},	{"Misc", "Gems", "Flawless Amethyst", "gzv", 0}, {"Misc", "Gems", "Perfect Amethyst", "gpv", 0},
		{"Misc", "Gems", "Chipped Topaz", "gcy", 0}, {"Misc", "Gems", "Flawed Topaz", "gfy", 0}, {"Misc", "Gems", "Topaz", "gsy", 0}, {"Misc", "Gems", "Flawless Topaz", "gly", 0},	{"Misc", "Gems", "Perfect Topaz", "gpy", 0},
		{"Misc", "Gems", "Chipped Saphire", "gcb", 0}, {"Misc", "Gems", "Flawed Saphire", "gfb", 0}, {"Misc", "Gems", "Saphire", "gsb", 0}, {"Misc", "Gems", "Flawless Saphire", "glb", 0}, {"Misc", "Gems", "Perfect Saphire", "gpb", 0},
		{"Misc", "Gems", "Chipped Emerald", "gcg", 0}, {"Misc", "Gems", "Flawed Emerald", "gfg", 0}, {"Misc", "Gems", "Emerald", "gsg", 0}, {"Misc", "Gems", "Flawless Emerald", "glg", 0}, {"Misc", "Gems", "Perfect Emerald", "gpg", 0},
		{"Misc", "Gems", "Chipped Ruby", "gcr", 0}, {"Misc", "Gems", "Flawed Ruby", "gfr", 0}, {"Misc", "Gems", "Ruby", "gsr", 0},	{"Misc", "Gems", "Flawless Ruby", "glr", 0}, {"Misc", "Gems", "Perfect Ruby", "gpr", 0},
		{"Misc", "Gems", "Chipped Diamond", "gcw", 0}, {"Misc", "Gems", "Flawed Diamond", "gfw", 0}, {"Misc", "Gems", "Diamond", "gsw", 0}, {"Misc", "Gems", "Flawless Diamond", "glw", 0}, {"Misc", "Gems", "Perfect Diamond", "gpw", 0},
		{"Misc", "Gems", "Chipped Skull", "skc", 0}, {"Misc", "Gems", "Flawed Skull", "skf", 0}, {"Misc", "Gems", "Skull", "sku", 0}, {"Misc", "Gems", "Flawless Skull", "skl", 0}, {"Misc", "Gems", "Perfect Skull", "skz", 0},
		{"Misc", "Quest", "Bark Scroll", "bks", 0},
		{"Misc", "Quest", "deciphered Bark Scroll", "bkd", 0},
		{"Misc", "Quest", "scroll of self resurrect", "xyz", 0},
		{"Misc", "Quest", "jade figurine", "j34", 0},
		{"Misc", "Quest", "gold bird", "g34", 0},
		{"Misc", "Quest", "Maguffin(ice)", "ice", 0},
		{"Misc", "Quest", "Scroll of Malah", "tr2", 0},
		{"Misc", "Quest", "lamesen'stome", "bbb", 0},
		{"Misc", "Quest", "Horadric Cube", "box", 0},
		{"Misc", "Quest", "Scroll of Horadric Quest Info", "tr1", 0},
		{"Misc", "Quest", "Mephisto Soul Stone", "mss", 0},
		{"Misc", "Quest", "Book of Skill", "ass", 0},
		{"Misc", "Quest", "Decoy Gidbinn", "d33", 0},
		{"Misc", "Quest", "The Gidbinn", "g33", 0},
		{"Misc", "Quest", "Horadric Malus", "hdm", 0},
		{"Misc", "Quest", "Hell Forge Hammer", "hfh", 0},
		{"Misc", "Quest", "Horadric Staff", "hst", 0},
		{"Misc", "Quest", "Shaft of the Horadric Staff", "msf", 0},
		{"Misc", "Quest", "Khalim's Flail", "qf1", 0},
		{"Misc", "Quest", "Khalim's Will", "qf2", 0},
		{"Misc", "Quest", "Wirt's Leg", "leg", 0},
		{"Misc", "Quest", "Khalim Eye", "qey", 0},	{"Misc", "Quest", "Khalim Heart", "qhr", 0}, {"Misc", "Quest", "Khalim Brain", "qbr", 0},
		{"Misc", "Not Used", "Heart", "hrt", 0}, {"Misc", "Not Used", "Brain", "brz", 0}, {"Misc", "Not Used", "Jawbone", "jaw", 0}, {"Misc", "Not Used", "Eye", "eyz", 0}, {"Misc", "Not Used", "Horn", "hrn", 0}, {"Misc", "Not Used", "Tail", "tal", 0}, {"Misc", "Not Used", "Flag", "flg", 0}, {"Misc", "Not Used", "Fang", "fng", 0}, {"Misc", "Not Used", "Quill", "qll", 0}, {"Misc", "Not Used", "Soul", "sol", 0}, {"Misc", "Not Used", "Scalp", "scz", 0}, {"Misc", "Not Used", "Spleen", "spe", 0},
		{"Misc", "Runes", "El Rune", "r01", 0},
		{"Misc", "Runes", "Eld Rune", "r02", 0},
		{"Misc", "Runes", "Tir Rune", "r03", 0},
		{"Misc", "Runes", "Nef Rune", "r04", 0},
		{"Misc", "Runes", "Eth Rune", "r05", 0},
		{"Misc", "Runes", "Ith Rune", "r06", 0},
		{"Misc", "Runes", "Tal Rune", "r07", 0},
		{"Misc", "Runes", "Ral Rune", "r08", 0},
		{"Misc", "Runes", "Ort Rune", "r09", 0},
		{"Misc", "Runes", "Thul Rune", "r10", 0},
		{"Misc", "Runes", "Amn Rune", "r11", 0},
		{"Misc", "Runes", "Sol Rune", "r12", 0},
		{"Misc", "Runes", "Shael Rune", "r13", 0},
		{"Misc", "Runes", "Dol Rune", "r14", 0},
		{"Misc", "Runes", "Hel Rune", "r15", 0},
		{"Misc", "Runes", "Io Rune", "r16", 0},
		{"Misc", "Runes", "Lum Rune", "r17", 0},
		{"Misc", "Runes", "Ko Rune", "r18", 0},
		{"Misc", "Runes", "Fal Rune", "r19", 0},
		{"Misc", "Runes", "Lem Rune", "r20", 0},
		{"Misc", "Runes", "Pul Rune", "r21", 0},
		{"Misc", "Runes", "Um Rune", "r22", 0},
		{"Misc", "Runes", "Mal Rune", "r23", 0},
		{"Misc", "Runes", "Ist Rune", "r24", 0},
		{"Misc", "Runes", "Gul Rune", "r25", 0},
		{"Misc", "Runes", "Vex Rune", "r26", 0},
		{"Misc", "Runes", "Ohm Rune", "r27", 0},
		{"Misc", "Runes", "Lo Rune", "r28", 0},
		{"Misc", "Runes", "Sur Rune", "r29", 0},
		{"Misc", "Runes", "Ber Rune", "r30", 0},
		{"Misc", "Runes", "Jah Rune", "r31", 0},
		{"Misc", "Runes", "Cham Rune", "r32", 0},
		{"Misc", "Runes", "Zod Rune", "r33", 0},
		{"Misc", "Runes", "Tor Rune", "r34", 0},
		{"Misc", "Runes", "Xer Rune", "r35", 0},
		{"Misc", "Quests V1.11", "Key of Terror", "pk1", 0}, {"Misc", "Quests V1.11", "Key of Hate", "pk2", 0},	{"Misc", "Quests V1.11", "Key of Destruction", "pk3", 0},
		{"Misc", "Quests V1.11", "Diablo's Horn", "dhn", 0}, {"Misc", "Quests V1.11", "Baal's Eye", "bey", 0},	{"Misc", "Quests V1.11", "Mephisto's Brain", "mbr", 0},
		{"Misc", "Quests V1.11", "Standard of Heroes", "std", 0},
		{"Misc", "Quests V1.11", "Burning Essence of Terror", "bet", 0}, {"Misc", "Quests V1.11", "Twisted Essence of Suffering", "tes", 0}, {"Misc", "Quests V1.11", "Charged Essence of Hatred", "ceh", 0}, {"Misc", "Quests V1.11", "Festering Essence of Destruction", "fed", 0},
		{"Misc", "Quests V1.11", "Token of Absolution", "toa", 0},		
		{"Annihilus", "Misc", "Runic Arrows", "aqv2", 0},
		{"Annihilus", "Misc", "Hellfire Arrows", "aqv3", 0},
		{"Annihilus", "Misc", "Razorspine Bolts", "cqv2", 0},
		{"Annihilus", "Misc", "Hallowed Bolts", "cqv3", 0},
		{"Annihilus", "Misc", "Scroll of Weapon Upgrade", "scwu", 0},
		{"Annihilus", "Misc", "Scroll of Armor Upgrade", "scau", 0},
		{"Annihilus", "Misc", "Sroll of Reroll", "rrsc", 0},
		{"Annihilus", "Misc", "Lucky Small Charm", "cm4", 0},
		{"Annihilus", "Misc", "White Dye", "dy1", 0}, 
		{"Annihilus", "Misc", "Black Dye", "dy2", 0},
		{"Annihilus", "Misc", "Blue Dye", "dy3", 0},
		{"Annihilus", "Misc", "Gold Dye", "dy4", 0},
		{"Annihilus", "Misc", "Green Dye", "dy5", 0},
		{"Annihilus", "Misc", "Orange Dye", "dy6", 0},
		{"Annihilus", "Misc", "Purple Dye", "dy7", 0},
		{"Annihilus", "Misc", "Red Dye", "dy8", 0},
		{"Annihilus", "Misc", "Bleach", "blc", 0},
		{"Annihilus", "Misc", "Orb of Alchemy", "orba", 0},
		{"Annihilus", "Misc", "Infernal Key", "ikey", 0},
		{"Annihilus", "Misc", "Infernal Chest (ring)", "inc1", 0}, 
		{"Annihilus", "Misc", "Infernal Chest (amulet)", "inc2", 0},
		{"Annihilus", "Misc", "Infernal Chest (rune)", "inc3", 0},
		{"Annihilus", "Misc", "Infernal Chest (???)", "inc4", 0},
		{"Annihilus", "Misc", "Unlocked Infernal Chest", "inc5", 0},
		{"Annihilus", "Misc", "Knarst's Amulet", "knam", 0},
		{"Annihilus", "Misc", "Diablo's Soulstone", "dias", 0},
		{"Annihilus", "Misc", "Jewel of the Dragonslayer", "jedr", 0},
		{"Annihilus", "Misc", "Othuyeg's Eyeball", "otey", 0},
		{"Annihilus", "Misc", "Puzzle Box", "puz1", 0},
		{"Annihilus", "Misc", "Puzzle Box", "puz2", 0},
		{"Annihilus", "Misc", "Puzzle Box", "puz3", 0},
		{"Annihilus", "Misc", "Puzzle Box", "puz4", 0},
		{"Annihilus", "Misc", "Puzzle Box", "puz5", 0},
		{"Annihilus", "Misc", "Madness Caster", "s1r1", 0},
		{"Annihilus", "Misc", "Nightmare Caster", "s1r2", 0},
		{"Annihilus", "Misc", "Horror Caster", "s1am", 0},
		{"Annihilus", "Misc", "Madness Physical", "s2r1", 0},
		{"Annihilus", "Misc", "Nightmare Physical", "s2r2", 0},
		{"Annihilus", "Misc", "Horror Physical", "s2am", 0},
		{"Annihilus", "Misc", "Puzzle Amulet 1", "pam1", 0},
		{"Annihilus", "Misc", "Puzzle Amulet 2", "pam2", 0},
		{"Annihilus", "Misc", "Puzzle Amulet 3", "pam3", 0},
		{"Annihilus", "Misc", "Puzzle Amulet 4", "pam4", 0},
		{"Annihilus", "Misc", "Key to Rlyeh", "kery", 0},
		{"Annihilus", "Misc", "Cthulhu Amulet", "ctam", 0},
		{"Annihilus", "Misc", "nv Blue Glow", "eff1", 0},
		{"Annihilus", "Misc", "nv Baal Shield", "eff2", 0},
		{"Annihilus", "Misc", "nv Red Shock", "eff3", 0},
		{"Annihilus", "Misc", "nv Dark Tyrael Wings", "eff4", 0},
		{"Annihilus", "Misc", "nv Big Shock", "eff5", 0},
		{"Annihilus", "Misc", "nv Valkyrie Glow", "eff6", 0},
		{"Annihilus", "Misc", "nv Green Glow", "eff7", 0},
		{"Annihilus", "Misc", "nv Small Shock", "eff8", 0},
		{"Annihilus", "Misc", "nv Noob Saibot", "eff9", 0},
		{"Annihilus", "Misc", "nv Pacman", "ef10", 0},
		{"Annihilus", "Misc", "nv Pikachu", "ef11", 0},
		{"Annihilus", "Misc", "nv Meph", "ef12", 0},
		{"Annihilus", "Misc", "nv Frozen Solid", "ef13", 0},
		{"Annihilus", "Misc", "nv Squelch", "ef14", 0},
		{"Annihilus", "Misc", "Altar Amulet", "pam5", 0},
		{"Annihilus", "Misc", "Cow Queen Amulet", "cow1", 0},
		{"Annihilus", "Misc", "Dead King Amulet", "pam6", 0},
		{"Annihilus", "Misc", "Cache of R'lyeh", "cct1", 0},
		{"Annihilus", "Misc", "Key of the Old Ones", "okey", 0},
		{"Annihilus", "Misc", "Key of Madness", "mke1", 0},
		{"Annihilus", "Misc", "Key of Nightmares", "mke2", 0},
		{"Annihilus", "Misc", "Key of Horror", "mke3", 0},
		{"Annihilus", "Misc", "Key of Insanity", "mke4", 0},
		{"Annihilus", "Misc", "The Cow King's Heart", "kiht", 0},
		{"Annihilus", "Misc", "The Cow King's Horn", "kihn", 0},
		{"Annihilus", "Misc", "The Cow King's Brain", "kibr", 0},
		{"Annihilus", "Misc", "The Cow King's Eye", "kiey", 0},
		{"Annihilus", "Misc", "The Cow King's Tail", "kita", 0},
		{"Annihilus", "Misc", "Azathoth's Heart", "azht", 0},
		{"Annihilus", "Misc", "Azathoth's Eye", "azey", 0},
		{"Annihilus", "Misc", "Azathoth's Brain", "azbr", 0},
		{"Annihilus", "Misc", "Green Worldstone Fragment", "wsf1", 0},
		{"Annihilus", "Misc", "Yellow Worldstone Fragment", "wsf2", 0},
		{"Annihilus", "Misc", "Blue Worldstone Fragment", "wsf3", 0},
		{"Annihilus", "Misc", "Red Worldstone Fragment", "wsf4", 0},
		{"Annihilus", "Misc", "Gold Worldstone Fragment", "wsf5", 0},
		{"Annihilus", "Misc", "Green Worldstone Crystal", "wsc1", 0},
		{"Annihilus", "Misc", "Yellow Worldstone Crystal", "wsc2", 0},
		{"Annihilus", "Misc", "Blue Worldstone Crystal", "wsc3", 0},
		{"Annihilus", "Misc", "Red Worldstone Crystal", "wsc4", 0},
		{"Annihilus", "Misc", "Gold Worldstone Crystal", "wsc5", 0},
		{"Annihilus", "Misc", "Pale Worldstone Crystal", "wsc6", 0},
		{"Annihilus", "Misc", "Orb of Transmogrification", "orbt", 0},
		{"Annihilus", "Misc", "Prime Charm 1 (Amazon)", "aby1", 0},
		{"Annihilus", "Misc", "Prime Charm 2 (Assassin)", "aby2", 0},
		{"Annihilus", "Misc", "Prime Charm 3 (Barbarian)", "aby3", 0},
		{"Annihilus", "Misc", "Prime Charm 4 (Druid)", "aby4", 0},
		{"Annihilus", "Misc", "Prime Charm 5 (Necromancer)", "aby5", 0},
		{"Annihilus", "Misc", "Prime Charm 6 (Paladin)", "aby6", 0},
		{"Annihilus", "Misc", "Prime Charm 7 (Sorceress)", "aby7", 0},
		{"Annihilus", "Misc", "Book of the Abyss", "aby8", 0},
		{"Annihilus", "Misc", "Belial's Tome", "aby9", 0},
		{"Annihilus", "Misc", "Basket of Organs", "baog", 0},
		{"Annihilus", "Misc", "Divine Amethyst", "gdv", 0},
		{"Annihilus", "Misc", "Divine Topaz", "gdy", 0},
		{"Annihilus", "Misc", "Divine Sapphire", "gdb", 0},
		{"Annihilus", "Misc", "Divine Emerald", "gdg", 0},
		{"Annihilus", "Misc", "Divine Ruby", "gdr", 0},
		{"Annihilus", "Misc", "Divine Diamond", "gdw", 0},
		{"Annihilus", "Misc", "Divine Skull", "dskz", 0},
		{"Annihilus", "Misc", "Scroll of Faster Cast Rate", "enc1", 0},
		{"Annihilus", "Misc", "Scroll of Faster Run Walk", "enc2", 0},
		{"Annihilus", "Misc", "Scroll of Faster Hit Recovery", "enc3", 0},
		{"Annihilus", "Misc", "Scroll of Increased Attack Speed", "enc4", 0},
		{"Annihilus", "Misc", "Scroll of Attack Rating", "enc5", 0},
		{"Annihilus", "Misc", "Scroll of Maximum Life", "enc6", 0},
		{"Annihilus", "Misc", "Scroll of Maximum Mana", "enc7", 0},
		{"Annihilus", "Misc", "Scroll of Ease", "enc8", 0},
		{"Annihilus", "Misc", "Scroll of Thawing", "enc9", 0},
		{"Annihilus", "Misc", "Scroll of Fire Damage", "en10", 0},
		{"Annihilus", "Misc", "Scroll of Cold Damage", "en11", 0},
		{"Annihilus", "Misc", "Scroll of Lightning Damage", "en12", 0},
		{"Annihilus", "Misc", "Scroll of Poison Damage", "en13", 0},
		{"Annihilus", "Misc", "Scroll of Trap Damage", "en14", 0},
		{"Annihilus", "Misc", "Scroll of Strength", "en15", 0},
		{"Annihilus", "Misc", "Scroll of Crushing Blow", "en16", 0},
		{"Annihilus", "Misc", "Scroll of Deadly Strike", "en17", 0},
		{"Annihilus", "Misc", "Scroll of Open Wounds", "en18", 0},
		{"Annihilus", "Misc", "Scroll of Luck", "en19", 0},
		{"Annihilus", "Misc", "Scroll of Enlightenment", "en20", 0},
		{"Annihilus", "Misc", "Scroll of the Amazon", "en21", 0},
		{"Annihilus", "Misc", "Scroll of the Assassin", "en22", 0},
		{"Annihilus", "Misc", "Scroll of the Barbarian", "en23", 0},
		{"Annihilus", "Misc", "Scroll of the Druid", "en24", 0},
		{"Annihilus", "Misc", "Scroll of the Necromancer", "en25", 0},
		{"Annihilus", "Misc", "Scroll of the Paladin", "en26", 0},
		{"Annihilus", "Misc", "Scroll of the Sorceress", "en27", 0},
		{"Annihilus", "Misc", "Scroll of Skill", "en28", 0},
		{"Annihilus", "Misc", "Scroll of Damage Reduction", "en29", 0},
		{"Annihilus", "Misc", "Scroll of Blood", "en30", 0},
		{"Annihilus", "Misc", "Scroll of the Valkyrie", "en31", 0},
		{"Annihilus", "Misc", "Scroll of the Golems", "en32", 0},
		{"Annihilus", "Misc", "Scroll of Massive Life", "en33", 0},
		{"Annihilus", "Misc", "Scroll of Massive Mana", "en34", 0},
		{"Annihilus", "Misc", "Scroll of Frostfire", "en35", 0},
		{"Annihilus", "Misc", "Scroll of the Old One", "en36", 0},
		{"Annihilus", "Misc", "Scroll of Cleaving", "en37", 0},
		{"Annihilus", "Misc", "Book of Enchantments", "scbk", 0},
		{"Annihilus", "Misc", "Key to the Courts of Justice", "cojk", 0},
		{"Annihilus", "Misc", "Trick or Treat Bag", "tktr", 0},
		{"Annihilus", "Misc", "Candy Bar", "hac1", 0},
		{"Annihilus", "Misc", "Spooky Ghost", "hac2", 0},
		{"Annihilus", "Misc", "Jack O' Lantern", "hac3", 0},
		{"Annihilus", "Misc", "Candy Corn", "hac4", 0},
		{"Annihilus", "Misc", "Infernal Pearl", "inpe", 0},
		{"Annihilus", "Misc", "nv Frozen Wings", "ef15", 0},
		{"Annihilus", "Misc", "nv Angel Wings", "ef16", 0},
		{"Annihilus", "Misc", "nv Demon Wings", "ef17", 0},
		{"Annihilus", "Misc", "nv Iridescent Wings", "ef18", 0},
		{"Annihilus", "Misc", "Scroll of Rune Upgrade", "scru", 0},
		{"Annihilus", "Misc", "Scroll of Rune Downgrade", "scrd", 0},
		{"Annihilus", "Misc", "nv Warped", "ef20", 0},
		{"Annihilus", "Misc", "Scroll of Magic Damage", "en38", 0},
		{"Annihilus", "Misc", "Scroll of the Horadrim", "en39", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 1", "cru1", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 2", "cru2", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 3", "cru3", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 4", "cru4", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 5", "cru5", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 6", "cru6", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 7", "cru7", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 8", "cru8", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 9", "cru9", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 10", "cr10", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 11", "cr11", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 12", "cr12", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 13", "cr13", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 14", "cr14", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 15", "cr15", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 16", "cr16", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 17", "cr17", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 18", "cr18", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 19", "cr19", 0},
		{"Annihilus", "Misc", "Crucible Admission Level 20", "cr20", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 2", "crk2", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 3", "crk3", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 4", "crk4", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 5", "crk5", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 6", "crk6", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 7", "crk7", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 8", "crk8", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 9", "crk9", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 10", "ck10", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 11", "ck11", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 12", "ck12", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 13", "ck13", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 14", "ck14", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 15", "ck15", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 16", "ck16", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 17", "ck17", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 18", "ck18", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 19", "ck19", 0},
		{"Annihilus", "Misc", "Key to The Crucible Level 20", "ck20", 0},
		{"Annihilus", "Misc", "The Dark Wanderer", "cai1", 0},
		{"Annihilus", "Misc", "Horadric Jewel", "cai3", 0},
		{"Annihilus", "Misc", "Nephalem's Birthright", "uld3", 0},
		{"Annihilus", "Misc", "nv Conquerer of Crucible", "ef19", 0},
		{"Annihilus", "Misc", "Mark of Trag'Oul", "puz6", 0},
		{"Annihilus", "Misc", "The Oculus Ring", "spe4", 0},
		{"Annihilus", "Misc", "Alkor's Shrunken Head", "spe5", 0},
		{"Annihilus", "Misc", "Diablo's Rebirth", "sp10", 0},
		{"Annihilus", "Misc", "Neo Annihilus", "sp11", 0},
		{"Annihilus", "Misc", "Wrapped Gift", "chg1", 0},
		{"Annihilus", "Misc", "Wrapped Gift", "chg2", 0},
		{"Annihilus", "Misc", "Wrapped Gift", "chg3", 0},
		{"Annihilus", "Misc", "Wrapped Gift", "chg4", 0},
		{"Annihilus", "Misc", "Wrapped Gift", "chg5", 0},
		{"Annihilus", "Misc", "Rudolph's Nose", "sp15", 0},
		{"Annihilus", "Misc", "Mark of Sin", "puz7", 0},
		{"Annihilus", "Misc", "Band of Sin", "sp17", 0},
		{"Annihilus", "Misc", "Dark Matter", "ef21", 0},
		{"Annihilus", "Misc", "Beach Ball", "ef22", 0},
		{"Annihilus", "Misc", "Exploding Cow", "ef23", 0},
		{"Annihilus", "Misc", "Cainnado", "ef25", 0},
		{"Annihilus", "Misc", "Guts Throw", "ef27", 0},
		{"Annihilus", "Misc", "Disco Maelstrom", "ef24", 0},
		{"Annihilus", "Misc", "Snowy Vengeance", "ef28", 0},
		{"Annihilus", "Misc", "Raise Santa", "ef26", 0},
		{"Annihilus", "Misc", "Snow Effect", "ef29", 0},
		{"Annihilus", "Misc", "Scroll of Fire Pierce", "en40", 0},
		{"Annihilus", "Misc", "Scroll of Cold Pierce", "en41", 0},
		{"Annihilus", "Misc", "Scroll of Lightning Pierce", "en42", 0},
		{"Annihilus", "Misc", "Scroll of Poison Pierce", "en43", 0},
		{"Annihilus", "Misc", "Scroll of Magic Pierce", "en44", 0},
		{"Annihilus", "Misc", "Scroll of Physical Pierce", "en45", 0},
		{"Annihilus", "Misc", "Scroll of Mortality", "en46", 0},
		{"Annihilus", "Misc", "Scroll of Paradox", "en47", 0},
		{"Annihilus", "Misc", "Bag of Fragments", "frbg", 0},
		{"Annihilus", "Misc", "Scroll of Tera", "en48", 0},
		{"Annihilus", "Misc", "Black Soulstone", "puz8", 0},
		{"Annihilus", "Misc", "Standard of Hell", "std2", 0},
		{"Annihilus", "Misc", "Mark of Lies", "puz9", 0},
		{"Annihilus", "Misc", "Xephirian Amulet", "sp22", 0},
		{"Annihilus", "Misc", "Obsidian Ring", "sp25", 0},
		{"Annihilus", "Armor", "Knarst's Ribcage", "knrc", 0},
		{"Annihilus", "Armor", "Moon of R'lyeh", "ctpe", 0},
		{"Annihilus", "Armor", "The Husk of Kaalut", "huka", 0},
		{"Annihilus", "Armor", "Cow Queen's Horns", "coq2", 0},
		{"Annihilus", "Armor", "Cow Queen's Hide", "coq3", 0},
		{"Annihilus", "Armor", "Cow Queen's Hooves", "coq1", 0},
		{"Annihilus", "Armor", "Treads of Valusia", "trva", 0},
		{"Annihilus", "Armor", "The Shrieking Lich ", "shrl", 0},
		{"Annihilus", "Armor", "Dragon's Bulwark", "drbu", 0},
		{"Annihilus", "Armor", "Purifying Flame", "pufl", 0},
		{"Annihilus", "Armor", "The Horseman's Head", "pmkn", 0},
		{"Annihilus", "Armor", "Scourgelord's Girdle", "lic1", 0},
		{"Annihilus", "Armor", "Crown of the High Heavens", "tyr1", 0},
		{"Annihilus", "Armor", "Divinity", "tyr2", 0},
		{"Annihilus", "Armor", "Heaven's Champion", "tyr3", 0},
		{"Annihilus", "Armor", "Death's Advance", "lic3", 0},
		{"Annihilus", "Armor", "Hysteria", "twi2", 0},
		{"Annihilus", "Armor", "Devil's Bargain", "twi3", 0},
		{"Annihilus", "Armor", "Elder of Tristram", "cai2", 0},
		{"Annihilus", "Armor", "Emblem of the Edyrem", "uld4", 0},
		{"Annihilus", "Armor", "Warmonger", "tra1", 0},
		{"Annihilus", "Armor", "Trag-Oul's True Guise", "tra2", 0},
		{"Annihilus", "Armor", "Sanctuary's Balance", "tra3", 0},
		{"Annihilus", "Armor", "Corrupted Stormshield", "cto2", 0},
		{"Annihilus", "Armor", "Corrupted Harlequin Crest", "cto3", 0},
		{"Annihilus", "Armor", "Short Circlet", "spe1", 0},
		{"Annihilus", "Armor", "Skull of the Unclean", "spe2", 0},
		{"Annihilus", "Armor", "Leoric's Folly", "spe6", 0},
		{"Annihilus", "Armor", "Crown of the Skeleton King", "spe7", 0},
		{"Annihilus", "Armor", "Elf Slippers", "sp13", 0},
		{"Annihilus", "Armor", "Azmodan's Shell", "sp16", 0},
		{"Annihilus", "Armor", "Serpent's Deception", "sp21", 0},
		{"Annihilus", "Armor", "Robes of the Reaper", "sp24", 0},
		{"Annihilus", "Armor", "Wisdom's End", "sp26", 0},
		{"Annihilus", "Weapon", "Short Circuit", "shci", 0},
		{"Annihilus", "Weapon", "Frostmourne", "rnbl", 0},
		{"Annihilus", "Weapon", "Ssshaa's Primordial Fang", "sfan", 0},
		{"Annihilus", "Weapon", "Frostfire", "frbo", 0},
		{"Annihilus", "Weapon", "Acid Injector", "acij", 0},
		{"Annihilus", "Weapon", "Insanity Caster", "s1we", 0},
		{"Annihilus", "Weapon", "Insanity Physical", "s2we", 0},
		{"Annihilus", "Weapon", "Trickster's Mischief", "trms", 0},
		{"Annihilus", "Weapon", "Goka Uln", "goul", 0},
		{"Annihilus", "Weapon", "Heaven's Fury", "hefu", 0},
		{"Annihilus", "Weapon", "Gae Bolg", "gabo", 0},	
		{"Annihilus", "Weapon", "Eldruin", "anbl", 0},
		{"Annihilus", "Weapon", "The Plaguebringer", "lic2", 0},
		{"Annihilus", "Weapon", "Flaming Pearl", "chi1", 0},
		{"Annihilus", "Weapon", "Buckshot", "chi2", 0},
		{"Annihilus", "Weapon", "The Twins Anger", "twi1", 0},
		{"Annihilus", "Weapon", "Thunderfury", "uld1", 0},
		{"Annihilus", "Weapon", "Khalim's Hope", "uld2", 0},
		{"Annihilus", "Weapon", "Corrupted Lightsabre", "cto1", 0},
		{"Annihilus", "Weapon", "Buster Sword", "spe3", 0},
		{"Annihilus", "Weapon", "The Butcher's Cleaver", "spe8", 0},
		{"Annihilus", "Weapon", "Touch of Death", "spe9", 0},
		{"Annihilus", "Weapon", "Flame Horizon", "sp12", 0},
		{"Annihilus", "Weapon", "North Pole Marker", "sp14", 0},
		{"Annihilus", "Weapon", "Heart of Hell", "sp18", 0},
		{"Annihilus", "Weapon", "The Deadly Sin", "sp19", 0},
		{"Annihilus", "Weapon", "Zephyr", "sp20", 0},
		{"Annihilus", "Weapon", "Oblivion", "sp23", 0},
	};


	//get real item codes on new patch
	void PrintPatchItemCodes() {
		vector<ItemCode> realcodes;
		std::ifstream codein;
		codein.open("realitemcodes.txt");
		while (codein.good()) {
			string ln;
			getline(codein, ln);
			string name, code;
			name = ln.substr(0, ln.rfind(","));
			code = ln.substr(ln.rfind(",") + 1, string::npos);
			ItemCode cd;
			cd.code = code;
			cd.type = name;
			if (code.size() < 5)
				realcodes.push_back(cd);
		}
		codein.close();
		for (UINT i = 0; i < realcodes.size(); i++) {
			bool found = false;
			for (UINT j = 0; j < itemcodes::itemcodes.size(); j++) {
				if (itemcodes::itemcodes[j].code == realcodes[i].code) {
					found = true;
					break;
				}
			}
			if (!found) {
				printf("NOT FOUND: \"%s\", \"%s\", 0},\n", realcodes[i].type.c_str(), realcodes[i].code.c_str());
			}
		}
		GETCH()
	}	

	//item codes for item filter
	void PrintBaseFilterCodes() {
		vector<string> normalsubtypes = {"Boots", "Gloves", "Belts", "Daggers", "Throwing", "Javelins", "Crossbows", "Potion"};
		vector<string> magicsubtypes = {"Helms", "Body Armor", "Shields", "Gloves", "Boots", "Belts", "Druid Pelts", "Barbarian Helms", "Paladin Shields", "Necromancer Shrunken Heads", "Axes",
			"Maces", "Swords", "Daggers", "Throwing", "Javelins", "Spears", "Polearms", "Bows", "Crossbows", "Staves", "Wands", "Scepters", "Assassin Katars", "Sorceress Orbs", "Amazon Weapons"};

		int c = 1;
		printf("%d=dy1,whatever,purple\n", c++);
		printf("%d=dy2,whatever,purple\n", c++);
		printf("%d=dy3,whatever,purple\n", c++);
		printf("%d=dy4,whatever,purple\n", c++);
		printf("%d=dy5,whatever,purple\n", c++);
		printf("%d=dy6,whatever,purple\n", c++);
		printf("%d=dy7,whatever,purple\n", c++);
		printf("%d=dy8,whatever,purple\n", c++);
										
		for (auto i : itemcodes::itemcodes) {
			string adj = i.code.size() == 3 ? i.code + ' ' : i.code;
			if (find(normalsubtypes.begin(), normalsubtypes.end(), i.subtype) != normalsubtypes.end()) {
				if (i.type != "Full Rejuv Potion" && i.type != "elixir") {
					printf("%d=%s,low,hide\n", c++, adj.c_str());
					printf("%d=%s,normal,hide\n", c++, adj.c_str());
					printf("%d=%s,superior,hide\n", c++, adj.c_str());
				}
			}
			if (find(magicsubtypes.begin(), magicsubtypes.end(), i.subtype) != magicsubtypes.end()) {
				if (i.type != "Circlet" && i.type != "Coronet" && i.type != "Tiara" && i.type != "Diadem"
					&& i.type != "Maiden Javelin" && i.type != "Ceremonial Javalin" && i.type != "Matriarchal Javalin"
					&& i.type != "Monarch") {

					printf("%d=%s,magic,hide\n", c++, adj.c_str());
				}
			}
		}

		printf("%d=cqv ,magic,hide\n", c++);
		printf("%d=cqv2,magic,hide\n", c++);
		printf("%d=cqv3,magic,hide\n", c++);
		printf("%d=aqv ,magic,hide\n", c++);
		printf("%d=aqv2,magic,hide\n", c++);
		printf("%d=aqv3,magic,hide\n", c++);
		printf("%d=tsc ,normal,hide\n", c++);
		printf("%d=isc ,normal,hide\n", c++);

		GETCH()
	}	
}

void read_itemcodes() {
#ifndef _DEBUG
	g_itemcodes.clear();
	ifstream file;
	file.open("bin//item codes.txt");
	if (file.is_open()) {
		while (file.good()) {
			string line;
			getline(file, line);
			while (line.find("!") == 0 && file.good()) {
				string basetype = line.substr(1, line.size() - 1);
				getline(file, line);
				while (line.find("@") == 0 && file.good()) {
					string subtype = line.substr(1, line.size() - 1);
					getline(file, line);
					while (line.size() > 0
						&& line[0] != '!'
						&& line[0] != '@'
						&& file.good()) {
						ItemCode code;
						code.basetype = basetype;
						code.subtype = subtype;
						vector<string> cod = split_str(line, ",");
						code.type = cod[0];
						code.code = cod[1];
						code.tier = str_to_int(cod[2]);
						g_itemcodes.push_back(code);
						getline(file, line);
					}
				}
			}
		}
	}
	else
		MessageBox(NULL, L"Unable to open item codes.txt", L"ERROR", MB_OK);

	file.close();
#else
	g_itemcodes = itemcodes::itemcodes;
#endif
}
void write_itemcodes(vector<ItemCode> &itemcodes) {
	//basetype  //subtype 
	map<string, map<string, vector<ItemCode*>>> item_types;//generates Warning: C4503
	for (UINT i = 0; i < itemcodes.size(); i++)
		item_types[itemcodes[i].basetype][itemcodes[i].subtype].push_back(&itemcodes[i]);

	ofstream file;
	file.open("bin//item codes.txt");
	for (auto basetype : item_types) {
		file << "!" << basetype.first << '\n';
		for (auto & subtype : basetype.second) {
			file << "@" << subtype.first << '\n';
			for (UINT i = 0; i < subtype.second.size(); i++) {
				file << subtype.second[i]->type << "," << subtype.second[i]->code << "," << subtype.second[i]->tier << '\n';
			}		
		}		
	}
	file.close();
}

bool operator==(const ItemCode &lhs, const ItemCode &rhs) {
	if (lhs.code == rhs.code)
		return true;
	return false;
}

void get_itemcode_diff() {
	printf("%s", "scanning for duplicats...\n");

	for (UINT i = 0; i < itemcodes::itemcodes.size(); i++) {
		for (UINT j = 0; j < itemcodes::itemcodes.size(); j++) {
			if (itemcodes::itemcodes[i].code == itemcodes::itemcodes[j].code && i != j) {
				printf("dupe!: %s\n", itemcodes::itemcodes[i].code.c_str());
			}
		}
	}

	std::vector<ItemCode> t_itemcodes;
	ifstream file;
	file.open("realitemcodes.txt");
	if (file.is_open()) {
		while (file.good()) {
			string line;
			getline(file, line);
			vector<string> sline = split_str(line, ",");
			if (sline.size() == 2) {
				ItemCode code;

				static vector<string> exclude = {
					"gld",
					"rps",
					"rpl",
					"bps",
					"bpl",
				};

				if (find(exclude.begin(), exclude.end(), sline[1]) != exclude.end() || sline[1][0] == ' ')
					continue;

				code.code = sline[1];
				t_itemcodes.push_back(code);
			}
		}
	}

	vector<ItemCode> res;
	
	printf("%s", "Searching for missing item codes...\n");
	for (int i = 0; i < t_itemcodes.size(); i++) {
		if (find(itemcodes::itemcodes.begin(), itemcodes::itemcodes.end(), t_itemcodes[i]) == itemcodes::itemcodes.end()) {
			printf("%s\n", t_itemcodes[i].code.c_str());
		}
	}
	printf("%s", "Done\nPress any key to continue\n");
	_getch();
}

void init_item_name_list(HWND hlist) {
	int castart, caend;
	SendMessage(hlist, CB_GETEDITSEL, (WPARAM)&castart, (LPARAM)&caend);
	wchar_t text[240];
	ComboBox_GetText(hlist, text, 239);
	ComboBox_ResetContent(hlist);
	ComboBox_SetText(hlist, text);
	SendMessage(hlist, CB_SETEDITSEL, 0, MAKELPARAM(castart, caend));

	std::map<std::string, int> itemslist;
	for (UINT i = 0; i < g_characters.size(); i++) {
		int size = g_characters[i].getItemsCount();
		for (INT j = 0; j < size; j++) {
			ItemData* item = g_characters[i].getItem(j);
			if (itemslist.find(item->name) == itemslist.end())
				itemslist.insert(make_pair(item->name, 1));
			else
				itemslist[item->name]++;
		}
	}
	for (auto a : itemslist) {
		SendMessage(hlist, CB_ADDSTRING, 0, (LPARAM)str_to_wstr(a.first + " (x" + int_to_str(a.second) + ")").c_str());
	}
}

int(__stdcall * DumpGames)();

#pragma region load character data

void SkipBOM(std::ifstream &in) {
	char test[3] = {0};
	in.read(test, 3);
	if ((unsigned char)test[0] == 0xEF &&
		(unsigned char)test[1] == 0xBB &&
		(unsigned char)test[2] == 0xBF) {
		return;
	}
	in.seekg(0);
}
BOOL CALLBACK LoadingProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{
			RECT rc = getclientrect(hwnd);
			SetWindowPos(hwnd, HWND_TOP, (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2, rc.right, rc.bottom, NULL);
			break;
		}
		case WM_LBUTTONDOWN:{
			SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			break;
		}
	}
	return FALSE;
}
void load_CharacterData() {
	g_characters.clear();
	g_realms.clear();

	vector<string> files;
	GetFilesInDirectory(getexedir() + "\\char data", "char", &files);

	HWND loadingwindow = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_LOADSCREEN), NULL, LoadingProc);
	HWND progressbar1 = GetDlgItem(loadingwindow, IDC_PROGRESS1);
	SendMessage(progressbar1, PBM_SETRANGE, 0, MAKELPARAM(0, files.size()));
	SendMessage(progressbar1, PBM_SETSTEP, (WPARAM)1, 0);

	for (UINT i = 0; i < files.size(); i++) {
		CharacterData cdata;

		std::ifstream file;
		file.open(files[i]);
		if (!file.is_open()) {
			MessageBox(NULL, L"unable to open file\n", L"ERROR", MB_OK);
			continue;
		}
		SkipBOM(file);
		enum {
			charstats = 0,
			stash = 1 << 0,
			body = 1 << 1,
			inventory = 1 << 2,
			cube = 1 << 3,
			mercenary = 1 << 4
		};
		vector<string> item_stats;
		int capture = 0;
		while (file.good()) {
			string str;
			std::getline(file, str);
			if (str.find("Realm: ") == 0)
				cdata.realm = str.substr(7, str.length() - 1);
			else if (str.find("Account: ") == 0)
				cdata.account = str.substr(9, str.length() - 1);
			else if (str.find("Character name: ") == 0)
				cdata.character = str.substr(16, str.length() - 1);
			else if (str == "#stash#")
				capture = stash;
			else if (str == "#body#")
				capture = body;
			else if (str == "#inventory#")
				capture = inventory;
			else if (str == "#cube#")
				capture = cube;
			else if (str == "#mercenary#")
				capture = mercenary;
			else if (str == "#character stats#")
				capture = charstats;
			else {
				if (str.size() > 0)
					item_stats.push_back(str);
			}
			if (str.size() == 0) {
				if (item_stats.size() >= 3 && capture && capture != charstats) {
					ItemData idata;
					//item_stats -> first line is storage data, second line is item item name, third line is item type, then base stats, then magic stats
					vector<string> inf = split_str(item_stats[0], ",");
					if (inf.size() == 7) {
						for (UINT i = 0; i < inf.size(); i++) {
							inf[i].pop_back();
							if (i == 0)	idata.invdata.x = str_to_int(inf[i]);
							if (i == 1) idata.invdata.y = str_to_int(inf[i]);
							if (i == 2) idata.invdata.w = str_to_int(inf[i]);
							if (i == 3) idata.invdata.h = str_to_int(inf[i]);
							if (i == 4) idata.invdata.inv = inf[i];
							if (i == 5) idata.invdata.invu = inf[i];
							if (i == 6) idata.invdata.invs = inf[i];
						}
					}
					idata.name = item_stats[1].substr(item_stats[1].find('>') + 1, item_stats[1].length());
					idata.quality = item_stats[1].substr(1, item_stats[1].find('>') - 1);
					idata.icode.type = item_stats[2].substr(item_stats[2].find('>') + 1, item_stats[2].length());
					if (idata.name == "Bad Unique ID") idata.name = idata.icode.type;
					idata.icode.code = item_stats[2].substr(1, item_stats[2].find('>') - 1);
					getItemDataFromCode(idata.icode.code, &idata);
					idata.realm = cdata.realm;
					idata.account = cdata.account;
					idata.character = cdata.character;
					for (UINT j = 2; j < item_stats.size(); j++) {
						string t = item_stats[j];
						if (t.find("<b>") == 0) {
							t.erase(0, 3);
							idata.basestats.push_back(t);
						}
						else if (t.find("<m>") == 0) {
							t.erase(0, 3);
							idata.stats.push_back(t);
						}
					}
					switch (capture) {
						case stash:{
							idata.store = "stash";
							cdata.stash_items.push_back(idata);
							break;
						}
						case body:{
							idata.store = "body";
							cdata.body_items.push_back(idata);
							break;
						}
						case inventory:{
							idata.store = "inventory";
							cdata.inv_items.push_back(idata);
							break;
						}
						case cube:{
							idata.store = "cube";
							cdata.cube_items.push_back(idata);
							break;
						}
						case mercenary:{
							idata.store = "mercenary";
							cdata.merc_items.push_back(idata);
							break;
						}
						default:
							break;
					}
				}
				else if (capture == charstats) {
					for (UINT j = 0; j < item_stats.size(); j++) {
						string s = item_stats[j];
						string id = s.substr(0, s.find(":"));
						if (id == "wp") {
							std::string val = s.substr(s.find(":") + 1, string::npos);
							if (val.size() < 120) continue;
							cdata.wp_norm = val.substr(0, 39);
							cdata.wp_night = val.substr(40, 79);
							cdata.wp_hell = val.substr(80, 119);
						}
						else if (id == "qu") {
							std::string val = s.substr(s.find(":") + 1, string::npos);
							if (val.size() < 90) continue;
							cdata.quest_norm = val.substr(0, 29);
							cdata.quest_night = val.substr(30, 59);
							cdata.quest_hell = val.substr(60, 89);
						}
						else {
							int val = str_to_int(s.substr(s.find(":") + 1, s.find("//") - 1));
							string str = s.substr(s.find("//") + 2, string::npos);
							cdata.character_stats.addStat(str_to_int(id), val, str);
						}
					}
				}
				item_stats.clear();
			}
		}
		SendMessage(progressbar1, PBM_STEPIT, 0, 0);

		file.close();
		g_characters.push_back(cdata);
	}
	for (UINT i = 0; i < g_characters.size(); i++) {
		UINT count = g_characters[i].getItemsCount();
		for (UINT j = 0; j < count; j++) {
			ItemData* item = g_characters[i].getItem(j);
			g_realms[item->realm][item->account][item->character][item->store].push_back(item);
		}
	}
	DestroyWindow(loadingwindow);
}

#pragma endregion
#pragma region CONTROL COLORING

BOOL g_use_cust_color = FALSE;
const COLORREF g_cust_color = RGB(25, 25, 25);
HBRUSH g_h_cust_color = CreateSolidBrush(g_cust_color);

typedef DWORD(__stdcall * pGetSysColor)(int nIndex);
pGetSysColor oGetSysColor;
DWORD __stdcall hGetSysColor(int nIndex) {
	if (!g_use_cust_color) return oGetSysColor(nIndex);
	switch (nIndex) {
		case COLOR_BACKGROUND:
		case COLOR_BTNFACE:
		case COLOR_WINDOW:{
			return g_cust_color;
		}
	}
	return oGetSysColor(nIndex);
};

typedef HBRUSH(__stdcall * pGetSysColorBrush)(int nIndex);
pGetSysColorBrush oGetSysColorBrush;
HBRUSH __stdcall hGetSysColorBrush(int nIndex) {
	if (!g_use_cust_color) return oGetSysColorBrush(nIndex);
	return g_h_cust_color;
};

WNDPROC oldComboProc;
LRESULT WINAPI SubComboProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) { 
	switch (message) {
		case WM_CTLCOLORLISTBOX:{
		//	if (!g_use_cust_color) break;
			SetBkColor((HDC)wParam, g_cust_color);
			SetTextColor((HDC)wParam, RGB(150, 150, 150));
			return (LRESULT)g_h_cust_color;
		}
	}
	return CallWindowProc(oldComboProc, hwnd, message, wParam, lParam);
}
void WMDrawItem(LPARAM lParam) {
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	switch (lpdis->CtlType) {
		case ODT_BUTTON:{
			HDC memhDC = CreateCompatibleDC(lpdis->hDC);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(lpdis->hDC, lpdis->rcItem.right, lpdis->rcItem.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(memhDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(memhDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(memhDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = getwindowtext(lpdis->hwndItem);
			HFONT hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, L"MS Shell Dlg");
			SelectObject(memhDC, hFont);
			GetTextExtentPoint32(memhDC, str_to_wstr(text).c_str(), text.size(), &size);
			SetBkColor(memhDC, g_cust_color);
			RECT trc = {(lpdis->rcItem.right - lpdis->rcItem.left - size.cx) / 2, (lpdis->rcItem.bottom - lpdis->rcItem.top - size.cy) / 2, size.cx, size.cy};
			if (lpdis->itemState & ODS_DISABLED) {
				SetTextColor(memhDC, RGB(40, 30, 30));				
				ExtTextOut(memhDC, trc.left + 1, trc.top + 1, ETO_OPAQUE | ETO_CLIPPED, &lpdis->rcItem, str_to_wstr(text).c_str(), text.size(), NULL);
				SetTextColor(memhDC, RGB(160, 0, 0));
				SetBkMode(memhDC, TRANSPARENT);
				DrawText(memhDC, str_to_wstr(text).c_str(), text.size(), &trc, DT_NOCLIP);
				SetBkMode(memhDC, OPAQUE);
			}
			else {
				SetTextColor(memhDC, RGB(240, 0, 0));
				ExtTextOut(memhDC,
					trc.left + (lpdis->itemState & ODS_SELECTED ? 1 : 0),
					trc.top + (lpdis->itemState & ODS_SELECTED ? 1 : 0),
					ETO_OPAQUE | ETO_CLIPPED, &lpdis->rcItem, str_to_wstr(text).c_str(), text.size(), NULL);
			}

			HPEN outerhighlight = CreatePen(PS_SOLID, 1, RGB(135, 135, 135));
			HPEN innerhighlight = CreatePen(PS_SOLID, 1, RGB(61, 61, 61));
			HPEN innershadow = CreatePen(PS_SOLID, 1, RGB(30, 30, 30));
			HPEN outershadow = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

			if (lpdis->itemState & ODS_FOCUS) {
				if (lpdis->itemState & ODS_SELECTED) {
					SelectObject(memhDC, innershadow);
					Rectangle(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.top + 1, lpdis->rcItem.right - 1, lpdis->rcItem.bottom - 1);

					SelectObject(memhDC, outershadow);
					Rectangle(memhDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
				}
				else {
					SelectObject(memhDC, outerhighlight);
					MoveToEx(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.top + 1, NULL);
					LineTo(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.top + 1);
					MoveToEx(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.top + 2, NULL);
					LineTo(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.bottom - 2);

					SelectObject(memhDC, innerhighlight);
					MoveToEx(memhDC, lpdis->rcItem.left + 2, lpdis->rcItem.top + 2, NULL);
					LineTo(memhDC, lpdis->rcItem.right - 3, lpdis->rcItem.top + 2);
					MoveToEx(memhDC, lpdis->rcItem.left + 2, lpdis->rcItem.top + 3, NULL);
					LineTo(memhDC, lpdis->rcItem.left + 2, lpdis->rcItem.bottom - 3);

					SelectObject(memhDC, innershadow);
					MoveToEx(memhDC, lpdis->rcItem.left + 2, lpdis->rcItem.bottom - 3, NULL);
					LineTo(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.bottom - 3);
					MoveToEx(memhDC, lpdis->rcItem.right - 3, lpdis->rcItem.top + 2, NULL);
					LineTo(memhDC, lpdis->rcItem.right - 3, lpdis->rcItem.bottom - 3);

					SelectObject(memhDC, outershadow);
					MoveToEx(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.bottom - 2, NULL);
					LineTo(memhDC, lpdis->rcItem.right - 1, lpdis->rcItem.bottom - 2);
					MoveToEx(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.top + 1, NULL);
					LineTo(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.bottom - 2);
					Rectangle(memhDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
				}
				if (~lpdis->itemState & ODS_NOFOCUSRECT) {
					SelectObject(memhDC, outershadow);
					Rectangle(memhDC, lpdis->rcItem.left + 4, lpdis->rcItem.top + 4, lpdis->rcItem.right - 4, lpdis->rcItem.bottom - 4);
				}
			}
			else {
				SelectObject(memhDC, outerhighlight);
				MoveToEx(memhDC, lpdis->rcItem.left, lpdis->rcItem.top, NULL);
				LineTo(memhDC, lpdis->rcItem.right - 1, lpdis->rcItem.top);
				MoveToEx(memhDC, lpdis->rcItem.left, lpdis->rcItem.top + 1, NULL);
				LineTo(memhDC, lpdis->rcItem.left, lpdis->rcItem.bottom - 1);

				SelectObject(memhDC, innerhighlight);
				MoveToEx(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.top + 1, NULL);
				LineTo(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.top + 1);
				MoveToEx(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.top + 2, NULL);
				LineTo(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.bottom - 2);

				SelectObject(memhDC, innershadow);
				MoveToEx(memhDC, lpdis->rcItem.left + 1, lpdis->rcItem.bottom - 2, NULL);
				LineTo(memhDC, lpdis->rcItem.right - 1, lpdis->rcItem.bottom - 2);
				MoveToEx(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.top + 1, NULL);
				LineTo(memhDC, lpdis->rcItem.right - 2, lpdis->rcItem.bottom - 2);

				SelectObject(memhDC, outershadow);
				MoveToEx(memhDC, lpdis->rcItem.left, lpdis->rcItem.bottom - 1, NULL);
				LineTo(memhDC, lpdis->rcItem.right, lpdis->rcItem.bottom - 1);
				MoveToEx(memhDC, lpdis->rcItem.right - 1, lpdis->rcItem.top, NULL);
				LineTo(memhDC, lpdis->rcItem.right - 1, lpdis->rcItem.bottom - 1);
			}

			BitBlt(lpdis->hDC, 0, 0, lpdis->rcItem.right, lpdis->rcItem.bottom, memhDC, 0, 0, SRCCOPY);

			SelectObject(memhDC, hbmOldBitmap);
			SelectObject(memhDC, hpenOld);
			SelectObject(memhDC, hbrushOld);

			DeleteObject(hFont);
			DeleteObject(outerhighlight);
			DeleteObject(innerhighlight);
			DeleteObject(innershadow);
			DeleteObject(outershadow);
			DeleteObject(hbmScreen);

			DeleteDC(memhDC);

			break;
		}
	}
}

#pragma endregion
#pragma region system tray

const UINT WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");
#define WM_TRAYICON (WM_USER + 1)
void Minimize() {
	ShowWindow(g_finder, SW_HIDE);
}
void Restore() {
	SetWindowPos(g_finder, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	SetForegroundWindow(g_finder);
}
void InitNotifyIconData(HWND hwnd) {
	g_notifyIconData = {0};
	g_notifyIconData.cbSize = sizeof(g_notifyIconData);
	g_notifyIconData.hWnd = hwnd;
	g_notifyIconData.uID = 5000;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED);
	g_notifyIconData.uTimeout = 0;
	g_notifyIconData.uVersion = NOTIFYICON_VERSION;
	memcpy(g_notifyIconData.szInfoTitle, L"TokenChest", 64);
	memcpy(g_notifyIconData.szInfo, L"TokenChest", 256);
	memcpy(g_notifyIconData.szTip, L"TokenChest", 128);
	g_notifyIconData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
}

#pragma endregion
#pragma region registry handling

template<typename T>
void edit_reg_config(LPCWSTR creg_key, T t, HKEY creg_rootkey = CREG_ROOTKEY, const wchar_t* creg_path = CREG_PATH) {
	CRegistry hregkey;
	if (!hregkey.Open(creg_path, creg_rootkey))
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);

	hregkey[creg_key] = t;

	hregkey.Close();
}

#pragma endregion
#pragma region subclass procs

INT_PTR CALLBACK DllWarnProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{
			CRegistry hregkey;
			if (!hregkey.Open(CREG_PATH, CREG_ROOTKEY))
				MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);
			bool warn = hregkey[CREG_KEY_DLLWARN];
			hregkey.Close();
			if (warn) {
				Button_SetCheck(GetDlgItem(hwnd, IDC_DLLWARNSHOW), TRUE);
				EndDialog(hwnd, 1);
			}
			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDOK:{
					EndDialog(hwnd, 1);
					break;
				}
				case IDCANCEL:{
					EndDialog(hwnd, 0);
					break;
				}
			}
			break;
		}
		case WM_DESTROY:{
			edit_reg_config(CREG_KEY_DLLWARN, Button_GetCheck(GetDlgItem(hwnd, IDC_DLLWARNSHOW)));
			break;
		}
	}
	return FALSE;
}

LRESULT CALLBACK itemlistProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR /*uIdSubclass*/, DWORD_PTR /*dwRefData*/) {
	switch (message) {
		case WM_KEYDOWN:{
			switch (wParam) {
				case 'A':{
					if (GetAsyncKeyState(VK_CONTROL) < 0) {
						int pos = ListView_GetNextItem(hwnd, -1, LVNI_ALL);
						while (pos != -1) {
							ListView_SetItemState(hwnd, pos, LVIS_SELECTED, LVIS_SELECTED);
							pos = ListView_GetNextItem(hwnd, pos, LVNI_ALL);
						}
					}
					break;
				}
				case VK_DELETE:{
					vector<ItemData*> selectlist = ListView_GetSelectedItems(hwnd);
					trade::remove(selectlist);
					break;
				}
			}
			break;
		}
	}
	return DefSubclassProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK stateditproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR /*uIdSubclass*/, DWORD_PTR /*dwRefData*/) {
	switch (message) {
		case WM_CHAR:{
			switch (wParam) {
				case 1:{
					if (GetAsyncKeyState(VK_CONTROL) < 0) {
						Edit_SetSel(hwnd, 0, -1);
						return 1;
					}
					break;
				}
			}
			break;
		}
	}
	return DefSubclassProc(hwnd, message, wParam, lParam);
};

#pragma endregion
#pragma region Display Status

UINT timer1;
BYTE g_status_col_red = 0;
BYTE g_status_col_green = 0;
BYTE g_status_col_blue = 0;
WORD g_status_count = 0;
VOID CALLBACK TimerProc(HWND hwnd, UINT /*msg*/, UINT_PTR idEvent, DWORD /*dwTime*/) {
	switch (idEvent) {
		case 1:{
			static BYTE stopcol = GetRValue(COLORREF(GetSysColor(COLOR_3DFACE)));
			static bool flash = false;
			
			WORD delay = 50;
			WORD rate = 1;
			delay *= rate;

			if (g_status_count <= 7 * rate) {				
				g_status_col_red = 0;
				g_status_col_green = 255;
				g_status_col_blue = 255;
				if (!flash)
					RedrawWindow(GetDlgItem(g_finder, IDC_STATUS), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				flash = true;
			}
			else if (flash && g_status_count > 7 * rate && g_status_count <= delay) {
				flash = false;
				g_status_col_red = g_status_col_green = g_status_col_blue = 0;
				RedrawWindow(GetDlgItem(g_finder, IDC_STATUS), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			}

			g_status_count += rate;
			if (g_status_count > delay) {
				g_status_col_red = g_status_col_green = g_status_col_blue = (BYTE)(g_status_count - delay);
				if (g_status_col_red >= stopcol) {
					g_status_col_red = g_status_col_green = g_status_col_blue = stopcol;
					g_status_count = 0;
					KillTimer(hwnd, timer1);
				}
				RedrawWindow(GetDlgItem(g_finder, IDC_STATUS), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			}		

			break;
		}
	}
}
void ShowStatus(string status) {
	SetWindowText(GetDlgItem(g_finder, IDC_STATUS), str_to_wstr(status).c_str());
	KillTimer(g_finder, timer1);
	g_status_count = 0;
	g_status_col_red = g_status_col_green = g_status_col_blue = 0;
	timer1 = SetTimer(g_finder, 1, 10, TimerProc);
	RedrawWindow(GetDlgItem(g_finder, IDC_STATUS), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

#pragma endregion

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
	g_hInst = hInstance;
#ifndef _DEBUG
	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, (LPCWSTR)"TokenChest");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}
#endif
#ifdef _DEBUG
	SHOW_CONSOLE();
	write_itemcodes(itemcodes::itemcodes);
#endif
		
	read_itemcodes();
	
//	DWORD addr = (DWORD)GetProcAddress(GetModuleHandle(L"User32.dll"), "GetSysColor");
//	oGetSysColor = (pGetSysColor)DetourFunction((PBYTE)addr, (PBYTE)hGetSysColor);
//	addr = (DWORD)GetProcAddress(GetModuleHandle(L"User32.dll"), "GetSysColorBrush");
//	oGetSysColorBrush = (pGetSysColorBrush)DetourFunction((PBYTE)addr, (PBYTE)hGetSysColorBrush);

	*(FARPROC*)&DumpGames = GetProcAddress(LoadLibrary(DLL_NAME), "DUMP");

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	EnableDebugPriv();

	CRegistry hregkey;
	if (!hregkey.Open(CREG_PATH, CREG_ROOTKEY))
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);
	hregkey[CREG_KEY_TOKENCHEST_PATH] = str_to_wstr(getexedir()).c_str();	
	g_is_auto_log = hregkey[CREG_KEY_AUTOLOG];
	hregkey.Close();
	if (g_is_auto_log) {
		EnumWindows(EnumWindowsProc, INJECT_DLL);
		AutoInjectOn(true);
	}

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);	

	LoadLibrary(L"riched20.dll");
	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_FINDER), NULL, (DLGPROC)FinderDialogProc, 0);

	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

	DestroyMenu(g_menu);

	Gdiplus::GdiplusShutdown(gdiplusToken);

	AutoInjectOn(false);

	FreeLibrary(GetModuleHandle(DLL_NAME));

	DeleteFont(g_boldfont);
	DeleteObject(g_h_cust_color);	

#ifndef _DEBUG
	CloseHandle(Mutex_handle);	
#endif

	return 0;
}
BOOL CALLBACK FinderDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_finder)) {
		Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);
		Restore();
		return FALSE;
	}
	switch (message) {
		case WM_INITDIALOG:	{
			g_finder = hwnd;

			InitCommonControls();
			
			g_menu = CreatePopupMenu();

			InitNotifyIconData(hwnd);
			Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

			DragAcceptFiles(hwnd, TRUE);
			ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
			ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
			ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);

			load_CharacterData();

			HWND hDummyButton = CreateWindow(L"button", L"", WS_CHILD | BS_OWNERDRAW, 0, 0, 0, 0, hwnd, (HMENU)0, NULL, NULL);
			SetClassLong(hDummyButton, GCL_STYLE, GetClassLong(hDummyButton, GCL_STYLE) & ~CS_DBLCLKS);
			DestroyWindow(hDummyButton);

			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1)));		

			g_TAB.setParent(GetDlgItem(hwnd, IDC_TAB1));
			g_TAB.addTab("Viewer", CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_TABPAGE1), g_TAB.getParent(), TabPage1Proc));
			g_TAB.addTab("Search", CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_TABPAGE2), g_TAB.getParent(), TabPage2Proc));
			g_TAB.addTab("Trade", CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_TABPAGE3), g_TAB.getParent(), TabPage3Proc));
			g_TAB.selectTab(0);

			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hwnd, NULL, NULL, NULL);

			//auto dump
			Button_SetCheck(GetDlgItem(hwnd, IDC_AUTODUMP), g_is_auto_log > 0);
			
			break;
		}
		case WM_CTLCOLORDLG:{
			if (!g_use_cust_color) break;
			HDC hDC = (HDC)wParam;

			RECT rc = getclientrect(hwnd);
			HBRUSH background = CreateSolidBrush(g_cust_color);
			FillRect(hDC, &rc, background);
			DeleteObject(background);

			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_CTLCOLORSTATIC:{
			switch (GetDlgCtrlID((HWND)lParam)) {
				case IDC_STATUS:{
					SetBkColor((HDC)wParam, COLORREF(GetSysColor(COLOR_3DFACE)));
					SetTextColor((HDC)wParam, RGB(g_status_col_red, g_status_col_green, g_status_col_blue));
					return (INT_PTR)GetSysColorBrush(COLOR_3DFACE);
				}
			}
			break;
		}
		case WM_DRAWITEM:{
			WMDrawItem(lParam);
			break;
		}
		case WM_ERASEBKGND:{
			return 0;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDC_DUMPBTN:{
					int ret = DumpGames();
#ifdef _DEBUG
					get_itemcode_diff();
#endif
			//		itemcodes::PrintPatchItemCodes();
					if (ret != ERROR_SUCCESS)
						break;
					//fallthrough to refresh button implementation
				}
				case IDC_REFRESHBTN:{
					EnableWindow(GetDlgItem(hwnd, IDC_REFRESHBTN), FALSE);

					//save current treeview selection
					HWND tree = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
					vector<string> treehier;
					HTREEITEM current = TreeView_GetSelection(tree);
					while (current != NULL) {
						treehier.push_back(TreeView_GetItemText(tree, current));
						current = TreeView_GetParent(tree, current);
					}
					TreeView_DeleteAllItems(tree);
					TreeView_DeleteAllItems(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCHARFILTER));
					ListView_DeleteColumn(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), 0);
					ListView_DeleteColumn(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), 0);
					ListView_DeleteColumn(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), 0);
					ListView_DeleteColumn(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), 0);
					ListView_DeleteAllItems(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS));
					ComboBox_ResetContent(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME));
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSHOWTRADEONLY), BST_UNCHECKED);
					SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDBTN, NULL);
					SetWindowText(GetDlgItem(g_TAB.getTab(0).wnd, IDC_STATVIEW), L"");
					SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_RESULTSTATS), L"");
					SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSTATS), L"");

					if ((HWND)lParam == GetDlgItem(hwnd, IDC_REFRESHBTN) || (HWND)lParam == GetDlgItem(hwnd, IDC_DUMPBTN)) {
						load_CharacterData();

						trade::load(trade::deffilename);
						trade::update();
					}

					TabPage1Proc(g_TAB.getTab(0).wnd, WM_INITDIALOG, NULL, NULL);
					TabPage2Proc(g_TAB.getTab(1).wnd, WM_INITDIALOG, NULL, NULL);

					//restore previous treeview selection if it still exists
					current = TreeView_GetRoot(tree);
					for (UINT i = treehier.size() - 2; (int)i > -1; i--) {
						current = TreeView_GetChild(tree, current);
						current = TreeView_FindSibling(tree, current, treehier[i]);
						TreeView_SelectItem(tree, current);
					}

					/*ListView_SortItems(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), ListViewCompareProc, 0);
					ListView_SortItems(GetDlgItem(g_Tradetab.getTab(0).wnd, IDC_TRADELIST), ListViewCompareProc, 0);*/

					RECT rc = getclientrect(GetDlgItem(g_TAB.getTab(0).wnd, IDC_INVDISPLAY));
					RedrawWindow(GetDlgItem(g_TAB.getTab(0).wnd, IDC_INVDISPLAY), &rc, NULL, RDW_INVALIDATE);

					EnableWindow(GetDlgItem(hwnd, IDC_REFRESHBTN), TRUE);

					break;
				}
				case IDC_AUTODUMP:{
					EnableWindow(GetDlgItem(hwnd, IDC_AUTODUMP), FALSE);
					bool state = Button_GetCheck(GetDlgItem(hwnd, IDC_AUTODUMP)) & BST_CHECKED;
					if (state) {
						if (DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLLWARN), hwnd, DllWarnProc) == IDOK) {
							EnumWindows(EnumWindowsProc, INJECT_DLL);
							AutoInjectOn(true);
							if (FAILED(WMIstart->hres)) {
								Button_SetCheck(GetDlgItem(hwnd, IDC_AUTODUMP), !state);
							}
							else {
								edit_reg_config(CREG_KEY_AUTOLOG, 1);
							}
						}
						else {
							Button_SetCheck(GetDlgItem(hwnd, IDC_AUTODUMP), 0);
						}
					}
					else {
						AutoInjectOn(false);
						edit_reg_config(CREG_KEY_AUTOLOG, 0);
					}
					EnableWindow(GetDlgItem(hwnd, IDC_AUTODUMP), TRUE);
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_TAB1:{
					switch (lpnmh->code) {
						case TCN_SELCHANGE:{
							g_TAB.selectTab(TabCtrl_GetCurFocus(g_TAB.getParent()));
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_SYSCOMMAND:{
			switch (wParam) {
				case SC_MINIMIZE:{
					Minimize();
					return TRUE;
				}
			}
			break;
		}
		case WM_TRAYICON:{
			switch (lParam) {
				case WM_LBUTTONUP:{
					if (ShowWindow(hwnd, SW_SHOW))
						Minimize();
					else
						Restore();
					break;
				}
				case WM_RBUTTONDOWN:{
					enum {
						close = 1,
						dump,
						auto_dump
					};
					bool autodumpstate = Button_GetCheck(GetDlgItem(hwnd, IDC_AUTODUMP)) & BST_CHECKED;
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED | (autodumpstate ? MF_CHECKED : MF_UNCHECKED), auto_dump, TEXT("Auto Dump"));
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, dump, TEXT("Manual Dump"));
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, close, TEXT("Exit"));
					POINT pt = getcursorpos();
					UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, NULL, hwnd, NULL);
					switch (clicked) {
						case close: {
							PostQuitMessage(0);
						}
						case dump: {
							PostMessage(hwnd, WM_COMMAND, IDC_DUMPBTN, NULL);
							break;
						}
						case auto_dump: {
							Button_SetCheck(GetDlgItem(hwnd, IDC_AUTODUMP), !autodumpstate);
							PostMessage(hwnd, WM_COMMAND, IDC_AUTODUMP, NULL);
							break;
						}
					}					
					DeleteMenu(g_menu, 0, MF_BYPOSITION);
					DeleteMenu(g_menu, 0, MF_BYPOSITION);
					DeleteMenu(g_menu, 0, MF_BYPOSITION);

					break;
				}
			}
			break;
		}
		case WM_COPYDATA:{
			COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
			switch (cds->dwData) {
				case WM_DUMPED:{
					GameStructInfo* gameinfo = (GameStructInfo*)(cds->lpData);
					memcpy(g_notifyIconData.szInfoTitle, L"Character Dumped", 64);
					memcpy(g_notifyIconData.szInfo, str_to_wstr(string("\'") + gameinfo->szCharName + string("\' was successfully dumped.")).c_str(), 256);
					Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
					break;
				}
			}
			break;
		}
		case WM_DROPFILES:{
			wchar_t text[MAX_PATH];
			DragQueryFile((HDROP)wParam, 0, text, MAX_PATH);
			string filename = wstr_to_str(text);
			if (filename.substr(filename.rfind("."), string::npos) == ".trade") {
				trade::load(filename);
				trade::update();
			}

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK TabPage1Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:	{
			InitCommonControls();

			//set font
			SetWindowFont(GetDlgItem(hwnd, IDC_TREE1), g_boldfont, false);

			//subclass invdisplay 
			static bool first = true;
			if (first) {
				first = false;
				SetWindowLongPtr(GetDlgItem(hwnd, IDC_INVDISPLAY), GWLP_WNDPROC, (LONG_PTR)invProc);
			}

			SetFocus(GetDlgItem(hwnd, IDC_TREE1));

			//color controls
			SendMessage(GetDlgItem(hwnd, IDC_STATVIEW), EM_SETBKGNDCOLOR, NULL, g_cust_color);

			/*HWND loadingwindow = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_LOADSCREEN), NULL, LoadingProc);
			HWND progressbar1 = GetDlgItem(loadingwindow, IDC_PROGRESS1);
			SendMessage(progressbar1, PBM_SETRANGE, 0, MAKELPARAM(0, g_characters.size()));
			SendMessage(progressbar1, PBM_SETSTEP, (WPARAM)1, 0);*/

			//////////////////////////////////////////////////////////////////////////
			//*******************Initialize tab page1 controls************************
			//////////////////////////////////////////////////////////////////////////
			//-----init finder tree
			static HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR16, 3, 10);
			static HBITMAP hBitMap = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1));
			ImageList_Add(hImageList, hBitMap, NULL);
			DeleteObject(hBitMap);
			SendDlgItemMessage(hwnd, IDC_TREE1, TVM_SETIMAGELIST, 0, (LPARAM)hImageList);

			TV_INSERTSTRUCT tvinsert;
			tvinsert.hParent = NULL;
			tvinsert.hInsertAfter = TVI_ROOT;
			tvinsert.item.pszText = L"Realms";
			tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvinsert.item.iImage = 0;
			tvinsert.item.iSelectedImage = 1;
			tvinsert.item.lParam = 0;
			tvinsert.hInsertAfter = TVI_SORT;
			HTREEITEM TRoot = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			HTREEITEM TRealm;
			HTREEITEM TAccount;
			HTREEITEM TChar;
			HTREEITEM TStore;
			for (auto realm : g_realms) {
				tvinsert.hParent = TRoot;
				tvinsert.item.pszText = str_to_LPWSTR(realm.first);
				TRealm = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto & account : realm.second) {
					tvinsert.hParent = TRealm;
					tvinsert.item.pszText = str_to_LPWSTR(account.first);
					TAccount = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & character : account.second) {
						tvinsert.hParent = TAccount;
						tvinsert.item.pszText = str_to_LPWSTR(character.first);
						TChar = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
						delete[] tvinsert.item.pszText;
						CharacterData* chardata = NULL;
						bool is_getchar = false;
						for (auto & store : character.second) {
							tvinsert.hParent = TChar;
							tvinsert.item.pszText = str_to_LPWSTR(store.first);
						//	store.second[0]->account;
							TStore = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
							delete[] tvinsert.item.pszText;
							tvinsert.item.iImage = 2;
							tvinsert.item.iSelectedImage = 2;
							ItemData* firstitem = NULL;
							bool is_getstore = false;
							for (auto & item : store.second) {
								tvinsert.hParent = TStore;
								tvinsert.item.pszText = str_to_LPWSTR(item->name);
								tvinsert.item.lParam = (LPARAM)item;
								SendDlgItemMessage(hwnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
								delete[] tvinsert.item.pszText;
								if (!is_getstore) {
									is_getstore = true;
									firstitem = item;
								}
								if (!is_getchar) {
									is_getchar = true;
									chardata = get_character(item);
								}
							}
							TVITEM tvi;
							tvi.mask = TVIF_PARAM;
							tvi.hItem = TStore;
							tvi.lParam = (LPARAM)firstitem;
							TreeView_SetItem(GetDlgItem(hwnd, IDC_TREE1), &tvi);

							tvinsert.item.lParam = 0;
							tvinsert.item.iImage = 0;
							tvinsert.item.iSelectedImage = 1;
						}
						TVITEM tvi;
						tvi.mask = TVIF_PARAM;
						tvi.hItem = TChar;
						tvi.lParam = (LPARAM)chardata;
						TreeView_SetItem(GetDlgItem(hwnd, IDC_TREE1), &tvi);

						tvinsert.item.lParam = 0;
		//				SendMessage(progressbar1, PBM_STEPIT, 0, 0);
					}
				}
			}
		//	DestroyWindow(loadingwindow);
			break;
		}
		case WM_CTLCOLORDLG:{
			if (!g_use_cust_color) break;
			HDC hDC = (HDC)wParam;

			RECT rc = getclientrect(hwnd);
			HBRUSH background = CreateSolidBrush(g_cust_color);
			FillRect(hDC, &rc, background);
			DeleteObject(background);

			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_ERASEBKGND:{
			return 0;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_TREE1:{
					switch (lpnmh->code) {
						case NM_CUSTOMDRAW:{
							LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
							HWND hWndTreeView = lpnmh->hwndFrom;
							SetWindowLong(hwnd, DWL_MSGRESULT, FinderTreeCustomDraw(hWndTreeView, pNMTVCD));
							break;
						}
						case TVN_SELCHANGED:{
							LPNMTREEVIEW tv = (LPNMTREEVIEW)lParam;
							HWND statview = GetDlgItem(hwnd, IDC_STATVIEW);
							int childcount = TreeView_GetChildCount(lpnmh->hwndFrom, tv->itemNew.hItem);
							if (tv->itemNew.lParam) {
								switch (childcount) {
									case 0:
									case 1:{										
										ItemData* newitem = (ItemData*)tv->itemNew.lParam;

										HWND display = GetDlgItem(hwnd, IDC_INVDISPLAY);

										//update item stat view	
										if (!childcount) {
											update_stat_view(statview, newitem);
											SendMessage(display, WM_SELECINVTITEM, NULL, (LPARAM)newitem);
											RedrawWindow(display, NULL, NULL, RDW_INVALIDATE);
										}
										else {
											SendMessage(display, WM_PAINT, 0, 0);
											SetWindowText(statview, L"");
										}
										break;
									}
									case 2:{
										CharacterData* chardata = (CharacterData*)tv->itemNew.lParam;
										if (!chardata) break;
										CharacterStats* newstats = &chardata->character_stats;
										if (!newstats) break;
										update_charstat_view(statview, newstats);

										RedrawWindow(GetDlgItem(hwnd, IDC_INVDISPLAY), NULL, NULL, RDW_INVALIDATE/* | RDW_UPDATENOW*/);
										break;
									}
								}
							}
							else {
								SendMessage(GetDlgItem(hwnd, IDC_INVDISPLAY), WM_PAINT, 0, 0);
								SetWindowText(statview, L"");
							}
							break;
						}
						case NM_RCLICK:{
							HTREEITEM hItem = TreeView_GetNextItem(lpnmh->hwndFrom, 0, TVGN_DROPHILITE);
							if (hItem)
								TreeView_SelectItem(lpnmh->hwndFrom, hItem);
							break;
						}
					}
					return TRUE;
				}
			}
			break;
		}
		case WM_CONTEXTMENU:{
			if ((HWND)wParam == GetDlgItem(hwnd, IDC_TREE1)) {
				HWND tree = GetDlgItem(hwnd, IDC_TREE1);
				HTREEITEM hItem = TreeView_GetSelection(tree);
				ItemData* item = (ItemData*)TreeView_GetItemParam(tree, hItem);

				int ccount = TreeView_GetChildCount(tree, hItem);

				std::function<HTREEITEM(HWND, HTREEITEM, vector<ItemData*> *)> TreeView_GetNodeItems = [&](HWND tree, HTREEITEM hItem, vector<ItemData*> *list) -> HTREEITEM {
					HTREEITEM current = hItem;
					HTREEITEM sibling = NULL;
					while (current != NULL && sibling == NULL) {
						if (!TreeView_GetChild(tree, current))
							list->push_back((ItemData*)TreeView_GetItemParam(tree, current));
						sibling = TreeView_GetNodeItems(tree, TreeView_GetChild(tree, current), list);
						current = TreeView_GetNextSibling(tree, current);
					}
					return sibling;
				};

				HMENU menu = CreatePopupMenu();
				enum {
					search_character = 2,
					search_account = 3,
					search_realm = 4,
					add_to_trade_file,
					remove_from_trade_file,
					open_stats_new_wnd
				};
				if (trade::is_mytradefile()) {
					std::string sea = ccount == 2 ? "Character"
						: ccount == 3 ? "Account"
						: ccount == 4 ? "Realm" : "";
					if (sea.size()) {
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ccount, str_to_wstr("Search 'this' " + sea + " (" + TreeView_GetParentText(tree, hItem, 0) + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
					}
					if (item && !ccount) {
						if (!trade::is_itemcountmax(item))
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, add_to_trade_file, str_to_wstr("Add 'this' Item to trade file (" + item->name + ")").c_str());
						if (trade::findtradeitem(item))
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove_from_trade_file, str_to_wstr("Remove 'this' Item from trade file (" + item->name + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, str_to_wstr("Show 'this' Item in new window (" + item->name + ")").c_str());
					}
					else {
						std::string text = TreeView_GetItemText(tree, hItem);
						std::string m = ccount == 1 ? "'this' Store's items"
							: ccount == 2 ? "'this' Character's items"
							: ccount == 3 ? "'this' Account's items"
							: ccount == 4 ? "'this' Realm's items" : "All Items";				
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, add_to_trade_file, str_to_wstr("Add " + m + " to trade file (" + text + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove_from_trade_file, str_to_wstr("Remove " + m + " from trade file (" + text + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, str_to_wstr("Show " + m + " in new window (" + text + ")").c_str());
					}
				}
				UINT clicked = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, LOWORD(lParam), HIWORD(lParam), NULL, hwnd, NULL);
				switch (clicked) {
					case search_character:{
						g_TAB.selectTab(1);
						ComboBox_SetText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME), L"");
						SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSTATS), L"");

						if (Button_GetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN)) == BST_UNCHECKED) {
							Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN), BST_CHECKED);
							SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDBTN, NULL);
						}
						SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDALLQUALITY, NULL);

						HWND tree = GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCHARFILTER);
						HTREEITEM hti = TreeView_GetRoot(tree);
						TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
						TreeView_SetCheckState(tree, hti, FALSE);

						hti = TreeView_GetChild(tree, hti);
						hti = TreeView_FindSibling(tree, hti, TreeView_GetParentText(tree, hItem, 3));
						hti = TreeView_GetChild(tree, hti);
						hti = TreeView_FindSibling(tree, hti, TreeView_GetParentText(tree, hItem, 2));
						hti = TreeView_GetChild(tree, hti);
						hti = TreeView_FindSibling(tree, hti, TreeView_GetParentText(tree, hItem, 1));
						TreeView_SetCheckState(tree, hti, TRUE);
						TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

						hti = TreeView_GetRoot(tree);
						hti = TreeView_GetNextSibling(tree, hti);
						TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
						TreeView_SetCheckState(tree, hti, TRUE);
						TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

						SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), NULL);
						SetFocus(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS));
						break;
					}
					case search_account:{
						g_TAB.selectTab(1);
						ComboBox_SetText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME), L"");
						SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSTATS), L"");

						if (Button_GetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN)) == BST_UNCHECKED) {
							Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN), BST_CHECKED);
							SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDBTN, NULL);
						}
						SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDALLQUALITY, NULL);

						HWND tree = GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCHARFILTER);
						HTREEITEM hti = TreeView_GetRoot(tree);
						TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
						TreeView_SetCheckState(tree, hti, FALSE);

						hti = TreeView_GetChild(tree, hti);
						hti = TreeView_FindSibling(tree, hti, TreeView_GetParentText(tree, hItem, 2));
						hti = TreeView_GetChild(tree, hti);
						hti = TreeView_FindSibling(tree, hti, TreeView_GetParentText(tree, hItem, 1));
						TreeView_SetCheckState(tree, hti, TRUE);
						TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

						hti = TreeView_GetRoot(tree);
						hti = TreeView_GetNextSibling(tree, hti);
						TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
						TreeView_SetCheckState(tree, hti, TRUE);
						TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

						SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), NULL);
						SetFocus(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS));
						break;
					}
					case search_realm:{
						g_TAB.selectTab(1);
						ComboBox_SetText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME), L"");
						SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSTATS), L"");

						if (Button_GetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN)) == BST_UNCHECKED) {
							Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN), BST_CHECKED);
							SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDBTN, NULL);
						}
						SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDALLQUALITY, NULL);

						HWND tree = GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCHARFILTER);
						HTREEITEM hti = TreeView_GetRoot(tree);
						TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
						TreeView_SetCheckState(tree, hti, FALSE);

						hti = TreeView_GetChild(tree, hti);
						hti = TreeView_FindSibling(tree, hti, TreeView_GetParentText(tree, hItem, 1));
						TreeView_SetCheckState(tree, hti, TRUE);
						TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

						hti = TreeView_GetRoot(tree);
						hti = TreeView_GetNextSibling(tree, hti);
						TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
						TreeView_SetCheckState(tree, hti, TRUE);
						TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

						SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), NULL);
						SetFocus(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS));
						break;
					}
					case add_to_trade_file:{
						if (ccount) {
							vector<ItemData*> list;
							TreeView_GetNodeItems(tree, TreeView_GetChild(tree, hItem), &list);
							trade::add(list);
						}
						else
							trade::add(item);
						break;
					}
					case remove_from_trade_file:{
						if (ccount) {
							vector<ItemData*> list;
							TreeView_GetNodeItems(tree, TreeView_GetChild(tree, hItem), &list);
							trade::remove(list);
						}
						else
							trade::remove(item);

						break;
					}
					case open_stats_new_wnd:{
						std::function<HTREEITEM(HWND, HTREEITEM, vector<HWND>*)> TreeView_OpenNodeInNewWindow = [&](HWND tree, HTREEITEM hItem, vector<HWND>* hwnds) -> HTREEITEM {
							HTREEITEM current = hItem;
							HTREEITEM sibling = NULL;
							while (current != NULL && sibling == NULL) {
								if (!TreeView_GetChild(tree, current)) {
									HWND newwnd = open_item_in_new_wnd((ItemData*)TreeView_GetItemParam(tree, current));
									hwnds->push_back(newwnd);
								}
								sibling = TreeView_OpenNodeInNewWindow(tree, TreeView_GetChild(tree, current), hwnds);
								current = TreeView_GetNextSibling(tree, current);
							}
							return sibling;
						};
						if (ccount) {
							vector<HWND> hwnds;
							TreeView_OpenNodeInNewWindow(tree, TreeView_GetChild(tree, hItem), &hwnds);
							RECT workarea;
							SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
							PackingTreeNode node(workarea);
							for (UINT i = 0; i < hwnds.size(); i++) {
								RECT newrect = node.insert(getclientrect(hwnds[i]));
								if (newrect.left != 0 && newrect.right != 0 && newrect.top != 0 && newrect.bottom != 0) {
									SetWindowPos(hwnds[i], NULL, newrect.left, newrect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
								}
							}
						}
						else
							open_item_in_new_wnd(item);
						break;
					}
				}
				DestroyMenu(menu);
			}
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK TabPage2Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	const UINT WM_POSTCONSTRUCTION = WM_USER + 1;
	const UINT PC_SETCHECKSTATEFORPARENTS = 0;
	switch (message) {
		case WM_INITDIALOG:{
			InitCommonControls();

			//-----color controls
			static bool first = true;
			if (first) {
				first = false;
				oldComboProc = (WNDPROC)SetWindowLong(GetDlgItem(hwnd, IDC_SEARCHNAME), GWL_WNDPROC, (LONG)SubComboProc);
				SendMessage(GetDlgItem(hwnd, IDC_RESULTSTATS), EM_SETBKGNDCOLOR, NULL, g_cust_color);

				SetWindowSubclass(GetDlgItem(hwnd, IDC_SEARCHRESULTS), itemlistProc, 0, 0);

				SetWindowSubclass(GetDlgItem(hwnd, IDC_SEARCHSTATS), stateditproc, 0, 0);
			}

			//////////////////////////////////////////////////////////////////////////
			//*******************Initialize tab page2 controls************************
			//////////////////////////////////////////////////////////////////////////

			//-----initialize results list columns
			HWND resultslist = GetDlgItem(hwnd, IDC_SEARCHRESULTS);

			SetWindowFont(resultslist, g_boldfont, false);

			SendMessage(resultslist, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 188;
			lvc.pszText = L"Item";
			SendMessage(resultslist, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);
			lvc.cx = 90;
			lvc.pszText = L"Character";
			SendMessage(resultslist, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);
			lvc.cx = 90;
			lvc.pszText = L"Account";
			SendMessage(resultslist, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);
			lvc.cx = 66;
			lvc.pszText = L"Realm";
			SendMessage(resultslist, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);

			//-----all qualities initialized to ON
			PostMessage(hwnd, WM_COMMAND, IDC_ADVANCEDALLQUALITY, NULL);

			//-----populate item names list and set focus
			HWND itemnamelist = GetDlgItem(hwnd, IDC_SEARCHNAME);
			init_item_name_list(itemnamelist);

			//-----populate intial results list (posts update_results_list() call)
			PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), (LPARAM)itemnamelist);

			//----------init advanced search filtering tree
			//-----init character filter tree
			TV_INSERTSTRUCT tvinsert = {0};
			tvinsert.hParent = NULL;
			tvinsert.hInsertAfter = TVI_ROOT;
			tvinsert.item.pszText = L"All Characters";
			tvinsert.item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
			tvinsert.item.stateMask = TVIS_STATEIMAGEMASK;
			tvinsert.hInsertAfter = TVI_SORT;
			HTREEITEM TRoot = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			HTREEITEM TRealm;
			HTREEITEM TAccount;			
			for (auto realm : g_realms) {
				tvinsert.hParent = TRoot;
				tvinsert.item.pszText = str_to_LPWSTR(realm.first);
				TRealm = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto & account : realm.second) {
					tvinsert.hParent = TRealm;
					tvinsert.item.pszText = str_to_LPWSTR(account.first);
					TAccount = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & character : account.second) {
						tvinsert.hParent = TAccount;
						tvinsert.item.pszText = str_to_LPWSTR(character.first);
						SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
						delete[] tvinsert.item.pszText;
					}
				}
			}

			//-----init item filter tree
			map<string, map<string, map<int, map<string, ItemCode*>>>> item_types;//generates Warning: C4503
			for (UINT i = 0; i < g_itemcodes.size(); i++)
				item_types[g_itemcodes[i].basetype][g_itemcodes[i].subtype][g_itemcodes[i].tier][g_itemcodes[i].type] = &g_itemcodes[i];

			tvinsert.hParent = NULL;
			tvinsert.hInsertAfter = TVI_ROOT;
			tvinsert.item.pszText = L"All Items";
			TRoot = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			HTREEITEM TBasetype;
			HTREEITEM TSubtype;
			HTREEITEM TTier;
			for (auto basetype : item_types) {
				tvinsert.hParent = TRoot;
				tvinsert.item.pszText = str_to_LPWSTR(basetype.first);
				TBasetype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto & subtype : basetype.second) {
					tvinsert.hParent = TBasetype;
					tvinsert.item.pszText = str_to_LPWSTR(subtype.first);
					TSubtype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & tier : subtype.second) {
						tvinsert.hParent = TSubtype;
						tvinsert.item.pszText = tier.first == 1 ? L"Normal" : tier.first == 2 ? L"Exceptional" : tier.first == 3 ? L"Elite" : L"";
						TTier = tier.first > 0 ? (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert) : TSubtype;
						for (auto & type : tier.second) {
							tvinsert.hParent = TTier;
							tvinsert.item.pszText = str_to_LPWSTR(type.first);
							tvinsert.item.lParam = (LPARAM)type.second;
							SendDlgItemMessage(hwnd, IDC_ADVANCEDCHARFILTER, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
							delete[] tvinsert.item.pszText;
						}
						tvinsert.item.lParam = 0;
					}
				}
			}

			break;
		}
		case WM_CTLCOLORDLG:{
			if (!g_use_cust_color) break;
			HDC hDC = (HDC)wParam;

			RECT rc = getclientrect(hwnd);
			HBRUSH background = CreateSolidBrush(g_cust_color);
			FillRect(hDC, &rc, background);
			DeleteObject(background);

			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_DRAWITEM:{
			WMDrawItem(lParam);
			break;
		}
		case WM_CTLCOLORSTATIC:{
			if (!g_use_cust_color) break;
			HDC hdc = (HDC)wParam;			
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(150, 150, 150));
			return (LRESULT)g_h_cust_color;
		}
		case WM_ERASEBKGND:{
			return 0;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDC_SEARCHSHOWTRADEONLY:{
					if ((HWND)lParam == GetDlgItem(hwnd, IDC_SEARCHSHOWTRADEONLY))
						update_result_list(hwnd);
					break;
				}
				case IDC_ADVANCEDALLQUALITY: {
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QLOW), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QNORMAL), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QSUPERIOR), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QMAGIC), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QRARE), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QUNIQUE), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QSET), BST_CHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QCRAFTED), BST_CHECKED);
					if ((HWND)lParam == GetDlgItem(hwnd, IDC_ADVANCEDALLQUALITY))
						update_result_list(g_TAB.getTab(1).wnd);
					break;
				}
				case IDC_ADVANCEDCLEARQUALITY: {
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QLOW), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QNORMAL), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QSUPERIOR), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QMAGIC), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QRARE), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QUNIQUE), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QSET), BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QCRAFTED), BST_UNCHECKED);
					if ((HWND)lParam == GetDlgItem(hwnd, IDC_ADVANCEDCLEARQUALITY))
						update_result_list(g_TAB.getTab(1).wnd);
					break;
				}
				case IDC_ADVANCEDBTN:{
					bool state = Button_GetCheck(GetDlgItem(hwnd, IDC_ADVANCEDBTN)) & BST_CHECKED;
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCHARFILTER), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QLOW), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QNORMAL), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QSUPERIOR), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QMAGIC), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QRARE), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QUNIQUE), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QSET), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_QCRAFTED), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDALLQUALITY), state);
					EnableWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCLEARQUALITY), state);
					//restore item name list
					if (!state) {
						init_item_name_list(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME));
					}
					if ((HWND)lParam == GetDlgItem(hwnd, IDC_ADVANCEDBTN))
						update_result_list(g_TAB.getTab(1).wnd);
					break;
				}
				case IDC_QCRAFTED:
				case IDC_QLOW:
				case IDC_QMAGIC:
				case IDC_QNORMAL:
				case IDC_QRARE:
				case IDC_QSET:
				case IDC_QSUPERIOR:
				case IDC_QUNIQUE: {
					update_result_list(hwnd);
					break;
				}
			}
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:{
					switch (LOWORD(wParam)) {
						case IDC_SEARCHNAME: {
							//update stats list  when item name is selected from dropdown list
							PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), lParam);
							break;
						}
					}
					break;
				}
				case CBN_CLOSEUP:{
					switch (LOWORD(wParam)) {
						case IDC_SEARCHNAME:{
							//update stats list when item name autocompletes (tab is pressed when drop down is active)
							if (GetAsyncKeyState(VK_TAB)) {
								SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), lParam);
							}
							break;
						}
					}
					break;
				}
				case CBN_EDITCHANGE:{
					switch (LOWORD(wParam)) {
						case IDC_SEARCHNAME: {
							//update results list when item name is entered
							update_result_list(hwnd);
							break;
						}
					}
					break;
				}
				case EN_CHANGE:{
					switch (LOWORD(wParam)) {
						case IDC_SEARCHSTATS:{
							//update stats list when stats are entered
							update_result_list(hwnd);

							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_ADVANCEDCHARFILTER:{
					switch (lpnmh->code) {
						case TVN_KEYDOWN:{
							LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN)lParam;
							if (ptvkd->wVKey == VK_SPACE) {
								HTREEITEM ht = TreeView_GetSelection(ptvkd->hdr.hwndFrom);
								TVITEM tvi;
								tvi.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
								tvi.hItem = (HTREEITEM)ht;
								tvi.stateMask = TVIS_STATEIMAGEMASK;
								TreeView_GetItem(ptvkd->hdr.hwndFrom, &tvi);

								TreeView_SetCheckStateForAllChildren(lpnmh->hwndFrom, TreeView_GetChild(lpnmh->hwndFrom, tvi.hItem), !TreeView_GetCheckState(lpnmh->hwndFrom, tvi.hItem));
								
								RECT* SetCheckStateForAllParents = new RECT;
								SetCheckStateForAllParents->left = (LONG)lpnmh->hwndFrom;
								SetCheckStateForAllParents->right = (LONG)tvi.hItem;
								PostMessage(hwnd, WM_POSTCONSTRUCTION, PC_SETCHECKSTATEFORPARENTS, (LPARAM)SetCheckStateForAllParents);
								
								PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), lParam);
							}
							return 0L;  // see the documentation for TVN_KEYDOWN
						}
						case NM_CLICK: {
							TVHITTESTINFO ht = {0};
							DWORD dwpos = GetMessagePos();
							ht.pt.x = GET_X_LPARAM(dwpos);
							ht.pt.y = GET_Y_LPARAM(dwpos);
							MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);
							TreeView_HitTest(lpnmh->hwndFrom, &ht);
							if (TVHT_ONITEMSTATEICON & ht.flags) {
								TVITEM tvi;
								tvi.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
								tvi.hItem = (HTREEITEM)ht.hItem;
								tvi.stateMask = TVIS_STATEIMAGEMASK;

								TreeView_SetCheckStateForAllChildren(lpnmh->hwndFrom, TreeView_GetChild(lpnmh->hwndFrom, tvi.hItem), !TreeView_GetCheckState(lpnmh->hwndFrom, tvi.hItem));
																
								RECT* SetCheckStateForAllParents = new RECT;
								SetCheckStateForAllParents->left = (LONG)lpnmh->hwndFrom;
								SetCheckStateForAllParents->right = (LONG)tvi.hItem;
								PostMessage(hwnd, WM_POSTCONSTRUCTION, PC_SETCHECKSTATEFORPARENTS, (LPARAM)SetCheckStateForAllParents);
								
								PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), lParam);
							}
							break;
						}
					}
					break;
				}
				case IDC_SEARCHRESULTS:{
					switch (lpnmh->code) {
						case LVN_ITEMCHANGED:{
							NMLISTVIEW *VAL_notify = (NMLISTVIEW*)lParam;
							if (VAL_notify->uNewState & LVIS_SELECTED) {
								HWND results = GetDlgItem(hwnd, IDC_SEARCHRESULTS);
								ItemData* item = (ItemData*)ListView_GetItemParam(results, VAL_notify->iItem);
								if ((LPARAM)item == 0xffffffff || !item) {
									SetWindowText(GetDlgItem(hwnd, IDC_RESULTSTATEX), L"");
									SetWindowText(GetDlgItem(hwnd, IDC_RESULTSTATS), L"");
									break;
								}

								//update item stat view							
								HWND statview = GetDlgItem(hwnd, IDC_RESULTSTATS);
								int start, end;
								update_stat_view(statview, item, &start, &end);

								//	IDC_RESULTSTATEX
								string invfile = item->quality == "unique" && item->invdata.invu.size() > 0 ? item->invdata.invu
									: item->quality == "set" && item->invdata.invs.size() > 0 ? item->invdata.invs
									: item->invdata.inv;
								string resultex = "";
								resultex += "Code: " + item->icode.code;
								resultex += "\r\ninvpos: (" + int_to_str(item->invdata.x) + ", " + int_to_str(item->invdata.y);
								resultex += ") [" + int_to_str(item->invdata.w) + "x" + int_to_str(item->invdata.h);
								resultex += "] \r\ninvfile: " + invfile;
								resultex += "\r\nQuality: " + item->quality;
								resultex += "\r\nBaseType: " + item->icode.basetype;
								resultex += "\r\nSubType: " + item->icode.subtype;
								resultex += "\r\nType: " + item->icode.type;
								SetWindowText(GetDlgItem(hwnd, IDC_RESULTSTATEX), str_to_wstr(resultex).c_str());

								//get text from stat search and highlight the matches
								string fulltext = getwindowtext(GetDlgItem(hwnd, IDC_SEARCHSTATS));
								vector<string> searchstats = split_str(fulltext, "\r\n", 1);
								HighlightTextInSelection(statview, searchstats, start, end);
							}
							else {
								SetWindowText(GetDlgItem(hwnd, IDC_RESULTSTATEX), L"");
								SetWindowText(GetDlgItem(hwnd, IDC_RESULTSTATS), L"");
							}
							break;
						}
						case LVN_COLUMNCLICK:{
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;
							if (last == pListView->iSubItem)
								flip = !flip;
							ListView_SortItems(GetDlgItem(hwnd, IDC_SEARCHRESULTS), ListViewCompareProc, flip ? pListView->iSubItem - 5 : pListView->iSubItem);
							last = pListView->iSubItem;
							break;
						}
						case NM_CUSTOMDRAW:{
							SetWindowLong(hwnd, DWL_MSGRESULT, (LONG)SearchResultsCustomDraw(lpnmh->hwndFrom, lParam));
							return TRUE;
						}
						case NM_DBLCLK:{
							find_item_in_tree((ItemData*)ListView_GetItemParam(GetDlgItem(hwnd, IDC_SEARCHRESULTS), ListView_GetCurSel(GetDlgItem(hwnd, IDC_SEARCHRESULTS))));
							
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CONTEXTMENU:{
			HWND listbox = GetDlgItem(hwnd, IDC_SEARCHRESULTS);
			if ((HWND)wParam == listbox) {
				ItemData* item = (ItemData*)ListView_GetItemParam(listbox, ListView_GetCurSel(listbox));
				vector<ItemData*> selectlist = ListView_GetSelectedItems(listbox);

				HMENU menu = CreatePopupMenu();
				enum {
					to_finder = 1,
					add_to_trade_file,
					remove_from_trade_file,
					open_stats_new_wnd,
					listview_selectall
				};
				if (selectlist.size() == 1) {
					if (trade::is_mytradefile()) {
						if (!trade::is_itemcountmax(item))
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, add_to_trade_file, STW("Add 'this' Item to trade file (" + item->name + ")"));
						if (trade::findtradeitem(item))
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove_from_trade_file, STW("Remove 'this' Item from trade file (" + item->name + ")"));
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
					}
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, to_finder, STW("Show 'this' Item in Viewer (" + item->name + ")"));
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, STW("Show 'this' Item in New Window(" + item->name + ")"));
				}
				else {
					LVHITTESTINFO lvhti = {0};
					lvhti.pt = getclientcursorpos(listbox);
					lvhti.flags = LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON;
					ListView_HitTest(listbox, &lvhti);
					item = (ItemData*)ListView_GetItemParam(listbox, lvhti.iItem);
					if (trade::is_mytradefile()) {
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, add_to_trade_file, STW("Add Selected Items to trade file (" + int_to_str(selectlist.size()) + ")"));
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove_from_trade_file, STW("Remove Selected Items from trade file (" + int_to_str(selectlist.size()) + ")"));
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
					}
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, to_finder, STW("Show 'this' Item in Viewer (" + item->name + ")"));
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, STW("Show Selected Items in New Window (" + int_to_str(selectlist.size()) + ")"));
				}
				InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
				InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, listview_selectall, L"Select All");
				UINT clicked = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, LOWORD(lParam), HIWORD(lParam), NULL, hwnd, NULL);
				switch (clicked) {
					case to_finder:{
						find_item_in_tree(item);
						break;
					}
					case add_to_trade_file:{
						trade::add(selectlist);
						break;
					}
					case remove_from_trade_file:{
						trade::remove(selectlist);
						break;
					}
					case open_stats_new_wnd:{
						for (auto item1 : selectlist)
							open_item_in_new_wnd(item1);
						break;
					}
					case listview_selectall:{
						int pos = ListView_GetNextItem((HWND)wParam, -1, LVNI_ALL);
						while (pos != -1) {
							ListView_SetItemState((HWND)wParam, pos, LVIS_SELECTED, LVIS_SELECTED);
							pos = ListView_GetNextItem((HWND)wParam, pos, LVNI_ALL);
						}
						break;
					}
				}
				DestroyMenu(menu);
			}
			break;
		}
		case WM_POSTCONSTRUCTION:{
			switch (wParam) {
				case PC_SETCHECKSTATEFORPARENTS:{
					RECT* hack = (RECT*)lParam;
					TreeView_SetCheckStateForAllParents((HWND)hack->left, (HTREEITEM)hack->right);
					delete hack;
					break;
				}
			}
			
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK TabPage3Proc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			InitCommonControls();

			g_ttab3 = hwnd;

			//-----color controls
			static bool first = true;
			if (first) {
				first = false;
				SendMessage(GetDlgItem(hwnd, IDC_TRADESTATS), EM_SETBKGNDCOLOR, NULL, g_cust_color);
			}

			g_Tradetab.setParent(GetDlgItem(hwnd, IDC_TRADETAB));
			g_Tradetab.addTab("For Trade", CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_TRADETABPAGE1), g_Tradetab.getParent(), TradeTabPage1Proc));
			g_Tradetab.addTab("In Search of", CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_TRADETABPAGE2), g_Tradetab.getParent(), TradeTabPage2Proc));
			g_Tradetab.selectTab(0);

			trade::update();

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_TRADETAB:{
					switch (lpnmh->code) {
						case TCN_SELCHANGE:{
							g_Tradetab.selectTab(TabCtrl_GetCurFocus(g_Tradetab.getParent()));
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK TradeTabPage1Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			g_ttab3_1 = hwnd;

			//-----initialize tradelist
			HWND tradelist = GetDlgItem(hwnd, IDC_TRADELIST);

			SetWindowFont(tradelist, g_boldfont, false);

			static bool first = true;
			if (first) {
				first = false;
				SetWindowSubclass(tradelist, itemlistProc, 0, 0);
			}
			
			SendMessage(tradelist, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 250;
			lvc.pszText = L"Item";
			lvc.cchTextMax = 4;
			SendMessage(tradelist, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);
			lvc.cx = 130;
			lvc.pszText = L"Character";
			lvc.cchTextMax = 10;
			SendMessage(tradelist, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);
			lvc.cx = 130;
			lvc.pszText = L"Account";
			lvc.cchTextMax = 7;
			SendMessage(tradelist, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);
			lvc.cx = 66;
			lvc.pszText = L"Realm";
			lvc.cchTextMax = 5;
			SendMessage(tradelist, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);

			trade::load();

			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDC_TRADEREMOVEBTN:{
					if (trade::is_mytradefile()) {
						Button_Enable(GetDlgItem(hwnd, IDC_TRADEREMOVEBTN), FALSE);

						vector<ItemData*> selectlist = ListView_GetSelectedItems(GetDlgItem(hwnd, IDC_TRADELIST));
						trade::remove(selectlist);

						Button_Enable(GetDlgItem(hwnd, IDC_TRADEREMOVEBTN), TRUE);
					}
					break;
				}
				case IDC_TRADELOADBTN:{
					trade::load(browseforfile(g_finder, true, L"Select Trade File to Open", L"Trade Files\0*.trade*\0"));
					trade::update();
					break;
				}
				case IDC_TRADESAVEBTN:{
					if (trade::is_mytradefile()) {
						string path = browseforfile(g_finder, false, L"Save Trade File", L"Trade File\0*.trade*\0");
						if (!path.size()) break;
						auto slash = path.rfind("\\");
						auto dot = path.rfind(".");
						string dir = path.substr(0, slash + 1);
						string filename = path.substr(slash + 1, (path.size() - slash) - (path.size() - dot) - 1);
						string filetype = path.substr(dot, string::npos);
						/*time_t t = time(0);
						struct tm* now = localtime(&t);*/
						path = dir + filename + /*"_" +
							int_to_str(now->tm_mon + 1) + "-" +
							int_to_str(now->tm_mday) + "-" +
							int_to_str(now->tm_year + 1900) + "__" +
							int_to_str(now->tm_hour) + ";" +
							int_to_str(now->tm_min) + ";;" +
							int_to_str(now->tm_sec) +*/
							filetype;
						ifstream source(trade::tradefilename, ios::binary);
						ofstream dest(path, ios::binary);
						dest << source.rdbuf();
						source.close();
						dest.close();
					}
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_TRADELIST:{
					switch (lpnmh->code) {
						case LVN_ITEMCHANGED:{
							NMLISTVIEW* nmlv = (NMLISTVIEW*)lParam;
							HWND tradelist = GetDlgItem(hwnd, IDC_TRADELIST);
							HWND tradestats = GetDlgItem(g_ttab3/*g_TAB.getTab(2).wnd*/, IDC_TRADESTATS);
							if (nmlv->uNewState & LVIS_SELECTED) {
								ItemData* item = (ItemData*)ListView_GetItemParam(tradelist, nmlv->iItem);
								if ((LPARAM)item == 0xffffffff || !item) {
									SetWindowText(tradestats, L"");
									break;
								}

								//update item stat view
								update_stat_view(tradestats, item);
							}
							else {
								SetWindowText(tradestats, L"");
							}
							break;
						}
						case LVN_COLUMNCLICK:{
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;
							if (last == pListView->iSubItem)
								flip = !flip;
							ListView_SortItems(GetDlgItem(hwnd, IDC_TRADELIST), ListViewCompareProc, flip ? pListView->iSubItem - 5 : pListView->iSubItem);
							last = pListView->iSubItem;
							break;
						}
						case NM_CUSTOMDRAW:{
							SetWindowLong(hwnd, DWL_MSGRESULT, (LONG)SearchResultsCustomDraw(lpnmh->hwndFrom, lParam));
							return TRUE;
						}
						case NM_DBLCLK:{	
							ItemData* item = (ItemData*)ListView_GetItemParam(GetDlgItem(hwnd, IDC_TRADELIST), ListView_GetCurSel(GetDlgItem(hwnd, IDC_TRADELIST)));
							find_item_in_tree(item);
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CONTEXTMENU:{
			if ((HWND)wParam == GetDlgItem(hwnd, IDC_TRADELIST)) {
				HWND tradelist = GetDlgItem(hwnd, IDC_TRADELIST);
				ItemData* item = (ItemData*)ListView_GetItemParam(tradelist, ListView_GetCurSel(tradelist));
				vector<ItemData*> selectlist = ListView_GetSelectedItems(tradelist);

				HMENU menu = CreatePopupMenu();
				enum {
					to_finder = 1,
					remove,
					open_stats_new_wnd,
					listview_selectall,
					undo_op,
					redo_op
				};
				if (item) {
					if (selectlist.size() == 1) {
						if (trade::is_mytradefile()) {
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove, STW("Remove 'this' Item from Trade File (" + item->name + ")"));
							InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						}
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, to_finder, STW("Show Item in Viewer (" + item->name + ")"));
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, STW("Show Item in New Window (" + item->name + ")"));
					}
					else {
						if (trade::is_mytradefile())
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove, STW("Remove 'these' Items from Trade File (" + int_to_str(selectlist.size()) + ")"));
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, STW("Show 'these' Items in New Window (" + int_to_str(selectlist.size()) + ")"));
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
					}
					InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
				}
				if (ListView_GetItemCount(tradelist))
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, listview_selectall, L"Select All");
				if (trade::undo.can_undo())
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, undo_op, L"Undo");
				if (trade::undo.can_redo())
					InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, redo_op, L"Redo");

				UINT clicked = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, LOWORD(lParam), HIWORD(lParam), NULL, hwnd, NULL);
				switch (clicked) {
					case remove:{
						trade::remove(selectlist);
						break;
					}
					case to_finder:{
						find_item_in_tree(item);
						break;
					}
					case open_stats_new_wnd:{
						for (auto item1 : selectlist)
							open_item_in_new_wnd(item1);
						break;
					}
					case listview_selectall:{
						int pos = ListView_GetNextItem((HWND)wParam, -1, LVNI_ALL);
						while (pos != -1) {
							ListView_SetItemState((HWND)wParam, pos, LVIS_SELECTED, LVIS_SELECTED);
							pos = ListView_GetNextItem((HWND)wParam, pos, LVNI_ALL);
						}
						break;
					}
					case undo_op:{
						trade::undo.undo();
						break;
					}
					case redo_op:{
						trade::undo.redo();
						break;
					}
				}
				DestroyMenu(menu);
			}
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK TradeTabPage2Proc(HWND /*hwnd*/, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{

			break;
		}
	}
	return FALSE;
}

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT kbhs = *((KBDLLHOOKSTRUCT*)lParam);
	DWORD key = kbhs.vkCode;
	if ((GetKeyState(VK_MENU) & 0x8000) != 0) key += 256;
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) key += 256 * 4;
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) key += 256 * 2;
	if ((GetKeyState(VK_LWIN) & 0x8000) != 0) key += 256 * 8;
	if ((GetKeyState(VK_RWIN) & 0x8000) != 0) key += 256 * 8;
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		if (key == 602 && GetForegroundWindow() == g_finder) {//ctrl + Z
			trade::undo.undo();
		}
		else if (key == 601 && GetForegroundWindow() == g_finder) {//ctrl + y
			trade::undo.redo();
		}
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}

