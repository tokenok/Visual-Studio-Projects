/*
Written by r57shell from Blizzhackers
http://www.blizzhackers.cc/viewtopic.php?f=182&t=499536

modified by token
*/

#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <cstdio>
#include <algorithm>
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>

#include "DllDump.h"
#include "Dll.h"

#include "common.h"

#ifdef _DEBUG
#include <conio.h>
#endif

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

//#define SE_DEBUG_PRIVILEGE 20

DWORD(__stdcall * RtlAdjustPrivilege)(DWORD, DWORD, DWORD, PVOID);

HANDLE ProcessHandle;
std::vector<DWORD> ItemsProcessed;
std::vector<int> StatsOrder;

struct PR // process read
{
	static void read(void *buff, unsigned int ptr, size_t size) {		
		if (ProcessHandle){
			SIZE_T readed;
			ReadProcessMemory(ProcessHandle, (LPCVOID)ptr, buff, size, &readed);
			if (readed != size) {
				MessageBox(NULL, str_to_wstr("Error: not full read " + int_to_str(ptr, true) + " - " + int_to_str(ptr + size, true) + ":" + int_to_str(readed, true) + " of " + int_to_str(size, true)).c_str(), L"ERROR", MB_OK);
				printf("Error: not full read %08X-%08X:%X of %X\n", ptr, ptr + size, readed, size);
			}
		}
		else {
			memcpy(buff, (LPCVOID)ptr, size);
		}
	}
};

template<typename T>
T* ReadProcess(DWORD ptr, T** data, int count = 1) {
	*data = (T*)RemoteData<PR>::read(ptr, sizeof(T) * count);
	return *data;
}

template<typename T>
struct Txt {
	static std::vector<T> data;
	static const T* Get(DWORD id) {
		if (id >= data.size())
			return NULL;
		return &data[id];
	}

	static size_t Count() {
		return data.size();
	}

	static int Load(const char *filename) {
		FILE *f = fopen(filename, "rb");
		if (!f) {
			fprintf(stderr, "Can't open %s\n", filename);
			return 1;
		}
		size_t count;
		fread(&count, 1, 4, f);
		fseek(f, 0, SEEK_END);
		size_t size = (size_t)ftell(f);
		if (count * sizeof(T) + 4 != size) {
			fclose(f);
			fprintf(stderr, "Wrong %s\n", filename);
			return 2;
		}
		fseek(f, 4, SEEK_SET);
		size = 0;
		for (;;) {
			T txt;
			size_t r = fread(&txt, 1u, sizeof(T), f);
			if (r != sizeof(T))
				break;
			data.push_back(txt);
			++size;
		}
		fclose(f);
		if (size != count) {
			fprintf(stderr, "Not full read %s\n", filename);
			return 3;
		}
		return 0;
	}
};

template<typename T> std::vector<T> Txt<T>::data;

template<typename T>
const void TxtGet(T const ** txt, DWORD id) {
	*txt = Txt<T>::Get(id);
}

int AdjustPrivilege() {
	HINSTANCE nt = GetModuleHandleA("ntdll.dll"); // always loaded
	if (!nt)
		return 0;

	*(FARPROC *)&RtlAdjustPrivilege = GetProcAddress(nt, "RtlAdjustPrivilege");

	DWORD prevState;

	if (RtlAdjustPrivilege(20, 1, 0, &prevState))
		return 0;

	return 1;
}

UINT initStatOrder() {
	StatsOrder.clear();
	for (size_t i = 0; i < Txt<ItemStatCostTxt>::Count(); ++i) {
		const ItemStatCostTxt *txt;
		TxtGet(&txt, (DWORD)i);
		if (!txt)
			continue;
		if (txt->descfunc) {
			StatsOrder.push_back((int)i);
		}
	}
	qsort(StatsOrder.data(), StatsOrder.size(), sizeof(int), desc_priority_cmp);
	return StatsOrder.size();
}

bool initBins() {
	if (Txt<ItemTxt>::Count())
		return 1;
	int txt = 0;
	txt |= Txt<ItemTxt>::Load((g_TokenChestPath + "\\bin\\weapons.bin").c_str());
	txt |= Txt<ItemTxt>::Load((g_TokenChestPath + "\\bin\\armor.bin").c_str());
	txt |= Txt<ItemTxt>::Load((g_TokenChestPath + "\\bin\\misc.bin").c_str());
	txt |= Txt<UniqueTxt>::Load((g_TokenChestPath + "\\bin\\uniqueitems.bin").c_str());
	txt |= Txt<SetTxt>::Load((g_TokenChestPath + "\\bin\\setitems.bin").c_str());
	txt |= Txt<ItemStatCostTxt>::Load((g_TokenChestPath + "\\bin\\itemstatcost.bin").c_str());
	txt |= Txt<SkillDescTxt>::Load((g_TokenChestPath + "\\bin\\skilldesc.bin").c_str());
	txt |= Txt<SkillTxt>::Load((g_TokenChestPath + "\\bin\\skills.bin").c_str());
	txt |= Txt<ItemTypeTxt>::Load((g_TokenChestPath + "\\bin\\itemtypes.bin").c_str());
	txt |= Txt<MagicSuffixTxt>::Load((g_TokenChestPath + "\\bin\\magicsuffix.bin").c_str());
	txt |= Txt<MagicSuffixTxt>::Load((g_TokenChestPath + "\\bin\\magicprefix.bin").c_str());
	txt |= Txt<RareSuffixTxt>::Load((g_TokenChestPath + "\\bin\\raresuffix.bin").c_str());
	txt |= Txt<RareSuffixTxt>::Load((g_TokenChestPath + "\\bin\\rareprefix.bin").c_str());
	txt |= Txt<GemTxt>::Load((g_TokenChestPath + "\\bin\\gems.bin").c_str());
	txt |= Txt<CharStatsTxt>::Load((g_TokenChestPath + "\\bin\\charstats.bin").c_str());
	txt |= Txt<MonStatsTxt>::Load((g_TokenChestPath + "\\bin\\monstats.bin").c_str());
	//txt |= Txt<PropertiesTxt>::Load((g_TokenChestPath + "\\bin\\properties.bin").c_str());

	//txt |= Txt<CubeBinField>::Load("C:\\Users\\Josh_2.Josh-PC\\Desktop\\Work\\data\\global\\excel\\cubemain.bin");

	if (txt)
		return 0;
	return 1;
}

#define UNIT_TYPE_ITEM 4

//#define ITEM_FLAG_WAS_USED (1<<0)
#define ITEM_FLAG_IDENTIFIED (1<<4)
#define ITEM_FLAG_BROKEN (1<<8)
#define ITEM_FLAG_SOCKETED (1<<11)
#define ITEM_FLAG_ILLEGALEQ (1<<14)
#define ITEM_FLAG_EAR (1<<16)
#define ITEM_FLAG_SIMPLE_STRUCT (1<<21)
#define ITEM_FLAG_ETHEREAL (1<<22)
#define ITEM_FLAG_PERSONALIZED (1<<24)
#define ITEM_FLAG_RUNEWORD (1<<26)

#define ITEM_QUALITY_INVALID 0
#define ITEM_QUALITY_LOW     1
#define ITEM_QUALITY_NORMAL  2
#define ITEM_QUALITY_HIGH    3
#define ITEM_QUALITY_MAGIC   4
#define ITEM_QUALITY_SET     5
#define ITEM_QUALITY_RARE    6
#define ITEM_QUALITY_UNIQUE  7
#define ITEM_QUALITY_CRAFT   8

const char *GetItemName(const UnitAny *item);

const ItemData *GetItemData(const UnitAny *item) {
	if (item->dwType != UNIT_TYPE_ITEM)
		return NULL;
	const ItemData *p = ReadProcess(item->pUnitData, &p);
	return p;
}
const ItemTxt *GetItemTxt(const UnitAny *item) {
	if (item->dwType != UNIT_TYPE_ITEM)
		return NULL;
	const ItemTxt *txt;
	TxtGet(&txt, item->dwTxtFileNo);
	return txt;
}
const char *GetItemFlagsStr(const UnitAny *item) {
	const char *flags[] = {
		"Unknown 0", //0
		"Unknown 1", //1
		"Unknown 2", //2
		"Unknown 3", //3
		"Identified", //4
		"Unknown 5", //5
		"Unknown 6", //6
		"Unknown 7", //7
		"Broken", //8
		"Unknown 9", //9
		"Unknown 10", //10
		"Socketed", //11
		"Unknown 12", //12
		"Unknown 13", //13
		"Illegal Equip", //14 (on body but red)
		"Unknown 15", //15
		"Ear", //16
		"Unknown 17", //17
		"Unknown 18", //18
		"Unknown 19", //19
		"Unknown 20", //20
		"Simple Struct", //21
		"Ethereal", //22
		"Unknown 23", //23
		"Personalized", //24
		"Unknown 25", //25
		"Runeword", //26
		"Unknown 27", //27
		"Unknown 28", //28
		"Unknown 29", //29
		"Unknown 30", //30
		"Unknown 31", //31
	};
	static char buff[300];
	buff[0] = 0;
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return buff;
	for (int i = 0; i<32; ++i)
		if (item_data->dwFlags&(1 << i)) {
			strcat_s(buff, flags[i]);
			strcat_s(buff, " ");
		}
	return buff;
}
const char *GetItemInvFile(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return "";
	const char *inv = NULL;
	const ItemTxt *txt = GetItemTxt(item);
	if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED) {
		if (item_data->dwQuality == ITEM_QUALITY_UNIQUE) {
			const UniqueTxt *unique;
			TxtGet(&unique, item_data->dwQuality2);
			if (unique) {
				inv = unique->invfile;
				if (inv[0] != 0)
					return inv;
			}
			if (txt) {
				inv = txt->invu;
				if (inv[0] != 0)
					return inv;
			}
		}
		if (txt && item_data->dwQuality == ITEM_QUALITY_SET) {
			const char* temp = GetItemName(item);
			std::string tstr(temp, strnlen(temp, 120));
			if (tstr.size() > 4 && tstr[0] == -61) {
				tstr = tstr.substr(4, tstr.size() - 4);
			}
			if (tstr == "Insanity")
				return "insanity";
			else if (tstr == "Horror")
				return "horror";
			else if (tstr == "Nightmare")
				return "cthulhuring";
			else if (tstr == "Madness")
				return "cthulhuring";
			else
				inv = txt->invs;			
			if (inv[0] != 0)
				return inv;
		}
	}
	if (txt) {
		const ItemTypeTxt *type;
		TxtGet(&type, txt->type);
		if (type) {
			int gfx = item_data->VarGfx;
			if (type->VarInvGfx && gfx<6) {
				return type->InvGfx[gfx];
			}
		}
		return txt->inv;
	}
	return "";
}
const char *GetItemSetName(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return "It's not item";
	if (item_data->dwQuality != ITEM_QUALITY_SET)
		return "It's not set";
	const SetTxt *set;
	TxtGet(&set, item_data->dwQuality2);
	if (set)
		return getStringByName(set->name);
	return "Bad Set ID";
}
const char *GetItemUniqueName(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return "ERROR: not item";
	if (item_data->dwQuality != ITEM_QUALITY_UNIQUE)
		return "ERROR: not unique";
	const UniqueTxt *unique;
	TxtGet(&unique, item_data->dwQuality2);
	if (unique)
		return getStringByName(unique->name);
	return "Bad Unique ID";
}
const char *GetItemRuneWordName(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return NULL;
	if (!(item_data->dwFlags & ITEM_FLAG_RUNEWORD))
		return NULL;
	return getStringByNum(item_data->wPrefix[0]);
}
const char *GetItemBaseName(const UnitAny *item) {
	const ItemTxt *txt = GetItemTxt(item);
	if (txt)
		return getStringByNum(txt->nameHash);
	return "Bad Item";
}
const char *GetItemNameColor(const UnitAny *item) {
	char *colors[] = {"<invalid>", "<low quality>", "<normal>", "<superior>", "<magic>", "<set>", "<rare>", "<unique>", "<crafted>"};

	const ItemTxt *txt;
	TxtGet(&txt, item->dwTxtFileNo);
	if (txt && txt->unique)
		return "<quest>";

	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return "<error>";

	if (item_data->dwFlags & ITEM_FLAG_BROKEN)
		return "<error>";

	if (item_data->dwQuality < ITEM_QUALITY_MAGIC) {
		if (item_data->dwFlags & (ITEM_FLAG_SOCKETED | ITEM_FLAG_ETHEREAL))
			return "<ethereal/socketed>";
	}


	char *c = colors[item_data->dwQuality];
	return c;
}
const char *GetMagicSuffix(DWORD id) {
	const MagicSuffixTxt *txt;
	TxtGet(&txt, id - 1);
	if (txt)
		return getStringByName(txt->name);
	return "";
}
int GetMagicSuffixLevelReq(DWORD id) {
	const MagicSuffixTxt *txt;
	TxtGet(&txt, id - 1);
	if (txt)
		return txt->levelreq;
	return 0;
}
const char *GetRareSuffix(DWORD id) {
	const RareSuffixTxt *txt;
	TxtGet(&txt, id - 1);
	if (!txt)
		return "";
	return getStringByName(txt->name);
}
const char *GetMonsterName(DWORD id) {
	const MonStatsTxt *txt;
	TxtGet(&txt, id);
	if (!txt)
		return getStringByNum(5382);
	return getStringByNum(txt->NameStr);
}
const char *GetItemName(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return "Not Item";

	static std::string name;
	name = GetItemBaseName(item);

	const char * space = getStringByNum(3995);
	//const char * newline = "\n";//getStringByNum(3998);

	const char * lq[] = {"Crude", "Cracked", "Damaged", "Low Quality"};

	switch (item_data->dwQuality) {
		case ITEM_QUALITY_LOW:
			name = std::string(getStringByName(lq[item_data->dwQuality2])) + space + name;
			break;
		case ITEM_QUALITY_NORMAL:
			break;
		case ITEM_QUALITY_HIGH:
			name = std::string(getStringByNum(1727)) + space + name;
			break;
		case ITEM_QUALITY_MAGIC:
			if (item_data->wPrefix[0])
				name = std::string(GetMagicSuffix(item_data->wPrefix[0])) + space + name;
			if (item_data->wSuffix[0])
				name = name + space + GetMagicSuffix(item_data->wSuffix[0]);
			break;
		case ITEM_QUALITY_SET:
			if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED)
				name = std::string(GetItemSetName(item));
			break;
		case ITEM_QUALITY_RARE:
		case ITEM_QUALITY_CRAFT:
			if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED) {
				if (item_data->wRareSuffix) {
					name = std::string(GetRareSuffix(item_data->wRareSuffix));
				}
				if (item_data->wRarePrefix) {
					name = std::string(GetRareSuffix(item_data->wRarePrefix)) + space + name;
				}
			}
			break;
		case ITEM_QUALITY_UNIQUE:
			if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED)
				name = std::string(GetItemUniqueName(item));
			break;
	}
	if (item_data->dwFlags & (ITEM_FLAG_PERSONALIZED | ITEM_FLAG_EAR))
		name = std::string(item_data->personalizedName) + "'s" + space + name;
	return name.c_str();
}
bool ItemHasClass(const UnitAny *item, DWORD _id) {
	const ItemTypeTxt *type;
	TxtGet(&type, _id);
	if (!type)
		return NULL;
	static char was[255];
	static WORD q[255];
	memset(was, 0, sizeof(was));
	const ItemTxt *txt = GetItemTxt(item);
	int l = 0;
	int r = 1;
	DWORD id = txt->type;
	q[l] = (WORD)id;
	while (l<r) {
		id = (DWORD)q[l++];
		if (was[id])
			continue;
		if (id == _id)
			return true;
		was[(BYTE)id] = 1;
		TxtGet(&type, id);
		if (type) {
			q[r++] = type->Equiv1;
			q[r++] = type->Equiv2;
		}
	}
	return false;
}
const char *GetSkillName(DWORD id) {
	const SkillTxt *txt;
	TxtGet(&txt, id);
	if (!txt)
		return getStringByNum(5382);

	const SkillDescTxt *desc;
	TxtGet(&desc, txt->skilldesc);
	if (desc)
		return getStringByNum(desc->strname);
	return getStringByNum(5382);
}
DWORD GetSkillCharClass(DWORD id) {
	const SkillTxt *txt;
	TxtGet(&txt, id);
	if (txt)
		return txt->charclass;
	return 255;
}
int FindInStats(const Stat * stats, int len, int id) {
	for (int i = 0; i < len; ++i) {
		if (stats[i].wStatIndex == id)
			return i;
	}
	return -1;
}
DWORD GetItemStatValue(const StatArr &arr, WORD index, WORD subindex) {
	if (!arr.wStatCount)
		return 0;
	const Stat *s = ReadProcess(arr.pStat, &s, arr.wStatCount);
	int i = FindInStats(s, arr.wStatCount, index);
	if (i < 0)
		return 0;
	for (; i < arr.wStatCount; ++i) {
		if (s[i].wStatIndex == index
			&& s[i].wSubIndex == subindex)
			return s[i].dwStatValue;
	}
	return 0;
}
DWORD GetItemStatValue(const std::vector<Stat> &arr, WORD index, WORD subindex) {
	if (!arr.size())
		return 0;
	int i = FindInStats(arr.data(), (int)arr.size(), index);
	if (i < 0)
		return 0;
	for (; i < (int)arr.size(); ++i) {
		if (arr[i].wStatIndex == index
			&& arr[i].wSubIndex == subindex)
			return arr[i].dwStatValue;
	}
	return 0;
}
int GetItemStatGrouped(const std::vector<Stat> &arr, WORD index, WORD subindex, DWORD value) {
	const ItemStatCostTxt *txt, *t;

	TxtGet(&txt, index);
	if (!txt)
		return 0;
	if (!txt->dgrp)
		return 0;

	int res = 1;
	for (UINT i = 0; i < Txt<ItemStatCostTxt>::Count(); ++i) {
		TxtGet(&t, (DWORD)i);
		if (!t)
			continue;
		if (t->dgrp == txt->dgrp) {
			if (GetItemStatValue(arr, (WORD)i, subindex) != value)
				return 0;
			if (i == index)
				res = 2;
			else
				res = 1;
		}
	}
	return res;
}
const char *GetStatDesc(WORD index, WORD subindex, DWORD value, int grouped) {
	long val = (long)value;
	static std::string desc;
	static char buff[200];
	int descfunc = -1;
	int descval = 4;
	int descstr2 = 0;
	bool error = false;
	int op = 0;
	int opparam = 0;
	const ItemStatCostTxt *txt;
	TxtGet(&txt, index);
	if (txt) {
		if (val >= 0)
			desc = getStringByNum(grouped ? txt->dgrpstrpos : txt->descstrpos);
		else
			desc = getStringByNum(grouped ? txt->dgrpstrneg : txt->descstrneg);
		descfunc = (grouped ? txt->dgrpfunc : txt->descfunc);
		descval = (grouped ? txt->dgrpval : txt->descval);
		descstr2 = (grouped ? txt->dgrpstr2 : txt->descstr2);
		val = val >> txt->ValShift;
		if (txt->op >= 2 && txt->op <= 5) {
			op = 2;
			opparam = txt->opparam;
		}
	}

	switch (descfunc) {
		case 1:
		case 12: // freeze target, hit blinds target
			sprintf_s(buff, "%+d", val);
			break;
		case 2:
			sprintf_s(buff, "%d%s", val, getStringByName("percent"));
			break;
		case 3:
			sprintf_s(buff, "%d", val);
			break;
		case 4:
			sprintf_s(buff, "%+d%s", val, getStringByName("percent"));
			break;
		case 5: // hit causes monster to flee
			sprintf_s(buff, "%d%s", val * 100 / 128, getStringByName("percent"));
			break;
		case 6:
			sprintf_s(buff, "%+.2lf", double(val) / (1 << opparam));
			break;
		case 7:
			sprintf_s(buff, "%.2lf%s", double(val) / (1 << opparam), getStringByName("percent"));
			break;
		case 8:
			sprintf_s(buff, "%+.2lf%s", double(val) / (1 << opparam), getStringByName("percent"));
			break;
		case 9: // attacker takes damage of (based on level)
			sprintf_s(buff, "%.2lf", double(val) / (1 << opparam));
			break;
		case 11:
			if (val >= 81)
				sprintf_s(buff, getStringByNum(21241), 1); // repairs 1 durability in second
			else
				sprintf_s(buff, getStringByNum(21242), 1, int(100.0 / val + 0.48)); // repairs 1 durability in %d seconds
			return buff;
		case 13: // class skills
		{
			int c = subindex;
			const CharStatsTxt* cs_txt;
			TxtGet(&cs_txt, c);
			if (cs_txt)
				sprintf_s(buff, "%+d %s", val, getStringByNum(cs_txt->StrAllSkills));
			else
				sprintf_s(buff, "%+d to Bad Class Skills(%d)", val, c);
			return buff;
		}
		case 14: // skills tab
		{
			int c = subindex / 8;
			int t = subindex & 7;
			const CharStatsTxt* cs_txt;
			TxtGet(&cs_txt, c);
			if (cs_txt && t < 3) {
				sprintf_s(buff, getStringByNum(cs_txt->StrSkillTab[t]), val);
				desc = std::string(buff) + " " + getStringByNum(cs_txt->StrClassOnly);
				return desc.c_str();
			}
			else {
				sprintf_s(buff, "%+d to Bad Skills Tab(%d,%d)", val, c, t);
				return buff;
			}
		}
		case 15: // skill on hit/struck... (trigger)
			sprintf_s(buff, desc.c_str(), val, subindex & 63, GetSkillName(subindex >> 6));
			return buff;
			// case 17,18 - by time
		case 16: // aura
			sprintf_s(buff, desc.c_str(), val, GetSkillName(subindex));
			return buff;
		case 19: // all res
			sprintf_s(buff, desc.c_str(), val);
			return buff;
		case 20:
			sprintf_s(buff, "%d%s", -val, getStringByName("percent"));
			break;
		case 21:
			sprintf_s(buff, "%d", -val);
			break;
			//case 22: // attack vs monster
		case 23: // reanimate as monster
		{
			const MonStatsTxt *txt;
			TxtGet(&txt, subindex);
			if (txt)
				descstr2 = txt->NameStr;
			sprintf_s(buff, "%d%s", val, getStringByName("percent"));
			break;
		}
		case 24: // charged skill
			sprintf_s(buff, desc.c_str(), val & 0xFF, val >> 8);
			desc = buff;
			sprintf_s(buff, "Level %d %s ", subindex & 63, GetSkillName(subindex >> 6));
			desc = std::string(buff) + desc;
			return desc.c_str();
			// case 25: unknown
			// case 26: unknown
		case 27: // single skill
		{
			int c = GetSkillCharClass(subindex);
			const CharStatsTxt* cs_txt;
			TxtGet(&cs_txt, c);
			if (cs_txt)
				sprintf_s(buff, "%+d to %s %s", val, GetSkillName(subindex), getStringByNum(cs_txt->StrClassOnly));
			else
				sprintf_s(buff, "%+d to %s (Bad Class Only)", val, GetSkillName(subindex));
			desc = std::string(buff);
			return desc.c_str();
		}
		case 28: // non-class skill
			sprintf_s(buff, "%+d to %s", val, GetSkillName(subindex));
			desc = std::string(buff);
			return desc.c_str();
		default:
			sprintf_s(buff, "%d(p:%d f:%d)", val, index, descfunc);
			error = true;
			break;
	}

	switch (descval) {
		case 0: // none
			if (error)
				desc = desc + getStringByName("space") + buff;
			break;
		case 1: // left
			desc = std::string(buff) + getStringByName("space") + desc;
			break;
		case 2: // right
			desc = desc + getStringByName("space") + buff;
			break;
		default: // unknown
			desc = desc + getStringByName("space") + buff;
			error = true;
			break;
	}
	if (error && subindex) {
		static char sub[20];
		sprintf_s(sub, "(%d)", subindex);
		desc = std::string(sub) + desc;
	}
	if (descstr2 != 5382 || descfunc == 23) {
		desc = desc + getStringByName("space") + getStringByNum(descstr2);
	}
	return desc.c_str();
}
const char *GetItemStatDesc(const UnitAny *item, const std::vector<Stat> &arr, WORD index, WORD subindex, DWORD value) {
	long val = (long)value;
	static std::string desc;
	static char buff[200];
	desc = "invalid";
	DWORD x;
	switch (index) {
		case 17: // enhanced max damage %
			x = GetItemStatValue(arr, 18, 0);
			if (x)
				return "";
			break;
		case 18: // enhanced min damage %
			x = GetItemStatValue(arr, 17, 0);
			if (x) {
				sprintf_s(buff, "%+d%s %s", val, getStringByNum(4001), getStringByNum(10023));
				return buff;
			}
			break;
		case 21: // min dmg
		case 22: // max dmg
		case 23: // min second dmg
		case 24: // max second dmg
		{
			int idx[4];
			for (int i = 0; i<4; ++i)
				idx[i] = -1;
			for (UINT i = 0; i<arr.size(); ++i)
				if (arr[i].wStatIndex >= 21
					&& arr[i].wStatIndex <= 24)
					idx[arr[i].wStatIndex - 21] = i;
			DWORD min = 0;
			DWORD max = 0;
			if (idx[0] >= 0) min = arr[idx[0]].dwStatValue;
			if (idx[1] >= 0) max = arr[idx[1]].dwStatValue;
			if (!min && idx[2] >= 0) min = arr[idx[2]].dwStatValue;
			if (!max && idx[3] >= 0) max = arr[idx[3]].dwStatValue;
			if (min && max && min < max) {
				int i;
				for (i = 0; i<4; ++i)
					if (idx[i])
						break;
				if (index == i + 21) {
					sprintf_s(buff, getStringByNum(3623), min, max);
					return buff;
				}
				else
					return "";
			}
			if (index == 23 && idx[0] >= 0 && arr[idx[0]].dwStatValue)
				return "";
			if (index == 24 && idx[1] >= 0 && arr[idx[1]].dwStatValue)
				return "";
			break;
		}
		case 48: // fire min
			x = GetItemStatValue(arr, 49, 0);
			if (x) {
				if (val < (long)x)
					sprintf_s(buff, getStringByNum(3613), val, x);
				else
					sprintf_s(buff, getStringByNum(3612), x);
				return buff;
			}
			break;
		case 49: // fire max
			x = GetItemStatValue(arr, 48, 0);
			if (x)
				return "";
			break;
		case 50: // lightning min
			x = GetItemStatValue(arr, 51, 0);
			if (x) {
				if (val < (long)x)
					sprintf_s(buff, getStringByNum(3613), val, x);
				else
					sprintf_s(buff, getStringByNum(3612), x);
				return buff;
			}
			break;
		case 51: // lightning max
			x = GetItemStatValue(arr, 50, 0);
			if (x)
				return "";
			break;
		case 52: // magic min
			x = GetItemStatValue(arr, 53, 0);
			if (x) {
				if (val < (long)x)
					sprintf_s(buff, getStringByNum(3619), val, x);
				else
					sprintf_s(buff, getStringByNum(3618), x);
				return buff;
			}
			break;
		case 53: // magic max
			x = GetItemStatValue(arr, 52, 0);
			if (x)
				return "";
			break;
		case 54: // cold min
			x = GetItemStatValue(arr, 55, 0);
			if (x) {
				if (val < (long)x)
					sprintf_s(buff, getStringByNum(3615), val, x);
				else
					sprintf_s(buff, getStringByNum(3614), x);
				return buff;
			}
			break;
		case 55: // cold max
			x = GetItemStatValue(arr, 54, 0);
			if (x)
				return "";
			break;
		case 57: // poison min
			x = GetItemStatValue(arr, 58, 0);
			if (x) {
				int len = GetItemStatValue(arr, 59, 0);
				int count = GetItemStatValue(arr, 326, 0);
				if (!count)
					count = 1;
				len = len / count;
				val = (val * len + 0x80) >> 8;
				x = (x * len + 0x80) >> 8;
				len = len / 25;
				if (val < (long)x)
					sprintf_s(buff, getStringByNum(3621), val, x, len);
				else
					sprintf_s(buff, getStringByNum(3620), x, len);
				return buff;
			}
			break;
		case 58: // poison max
			x = GetItemStatValue(arr, 57, 0);
			if (x)
				return "";
			break;
	}
	int gr = GetItemStatGrouped(arr, index, subindex, value);
	if (gr == 1)
		return "";
	if (index == 122) {
		if (ItemHasClass(item, 57))
			val += 50;
	}
	return GetStatDesc(index, subindex, (DWORD)val, gr);
}
const char * GetInvisibleStatDesc(WORD index, WORD subindex, DWORD value) {
	static char buff[200];
	bool vis = true;
	const ItemStatCostTxt *txt;
	TxtGet(&txt, index);
	if (txt) {
		if (!txt->descfunc)
			vis = false;
	}
	else
		vis = false;
	if (vis)
		return "";
	int val = (int)value >> txt->ValShift;
	switch (index) {
		case 4: // New Stats
			sprintf_s(buff, "%s: %d", getStringByNum(3986), val);
			break;
		case 5: // New Skill
			sprintf_s(buff, "%s: %d", getStringByNum(3987), val);
			break;
		case 6:
			sprintf_s(buff, "Current Life: %d", val);
			break;
		case 8:
			sprintf_s(buff, "Current Mana: %d", val);
			break;
		case 10:
			sprintf_s(buff, "Current Stamina: %d", val);
			break;
		case 12: // Level
			sprintf_s(buff, "%s: %d", getStringByNum(4057), val);
			break;
		case 13: // Experience
			sprintf_s(buff, "%s: %d", getStringByNum(4058), val);
			break;
		case 14: // Gold
			sprintf_s(buff, "%s: %d", getStringByNum(2215), val);
			break;
		case 15: // Gold in stash
			sprintf_s(buff, "%s %d", getStringByNum(4128), val);
			break;
		case 56: // cold length
			sprintf_s(buff, "%s %d", getStringByNum(4265), val);
			break;
		case 59: // poison length
			sprintf_s(buff, "%s %d", getStringByNum(4266), val);
			break;
		case 73:
			sprintf_s(buff, "Maximum Durability: %+d", val);
			break;
		case 92:
			sprintf_s(buff, "Level Requirement: %+d", val);
			break;
		case 94:
			sprintf_s(buff, "Level Requirement: %+d%%", val);
			break;
		case 159:
			sprintf_s(buff, "%+d to Minimum Throw Damage", val);
			break;
		case 160:
			sprintf_s(buff, "%+d to Maximum Throw Damage", val);
			break;
		case 326:
			sprintf_s(buff, "Poison Count: %d", val);
			break;
		case 356:
			sprintf_s(buff, "Quest Item Difficulty: %d", val);
			break;
		default:
			if (subindex)
				sprintf_s(buff, "Unknown property %d: (%d) %+d", index, subindex, val);
			else
				sprintf_s(buff, "Unknown property %d: %+d", index, val);
			break;
	}
	return buff;
}
const char *GetItemWeaponClassName(int id) {
	const ItemTypeTxt *type;
	TxtGet(&type, id);
	if (!type)
		return NULL;
	int item_classes[] = {
		0x1A, 0xFF5,
		0x1C, 0xFEE,
		0x1E, 0xFEF,
		0x20, 0xFF0,
		0x26, 0xFF1,
		0x2C, 0xFF2,
		0x21, 0xFF3,
		0x1B, 0xFF4,
		0x22, 0xFF6,
		0x23, 0xFF7,
		0x43, 0x530A,
		0x58, 0x530A,
		0x44, 0xFF5,
		0x19, 0xFF5,
		0x39, 0xFED};
	static char was[255];
	static char q[255];
	memset(was, 0, sizeof(was));
	int l = 0;
	int r = 1;
	q[l] = (char)id;
	while (l<r) {
		id = q[l++];
		if (was[id])
			continue;
		was[id] = 1;
		TxtGet(&type, id);
		q[r++] = (char)type->Equiv1;
		q[r++] = (char)type->Equiv2;
		for (int i = 0; i<sizeof(item_classes) / sizeof(int) / 2; ++i) {
			if (id == item_classes[i * 2])
				return getStringByNum(item_classes[i * 2 + 1]);
		}
	}
	return NULL;
}
const char * GetItemClassOnly(const UnitAny *item) {
	const ItemTxt *txt = GetItemTxt(item);
	if (!txt)
		return NULL;

	const ItemTypeTxt *type;
	TxtGet(&type, txt->type);
	if (!type)
		return NULL;

	int c = type->Class;
	if (c != 255) {
		const CharStatsTxt *s;
		TxtGet(&s, c);
		if (s)
			return getStringByNum(s->StrClassOnly);
	}
	return NULL;
}
int GetItemRequiredDexterity(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return 0;
	const ItemTxt *txt = GetItemTxt(item);

	int res = 0;
	if (txt)
		res = txt->reqdex;

	if (item_data->dwFlags & ITEM_FLAG_ETHEREAL)
		res -= 10;

	const StatArr *sb = ReadProcess((DWORD)item->pStats + 0x48, &sb);
	int val = (signed long)GetItemStatValue(*sb, 91, 0); // Requirements
	if (val)
		res = (res * (100 + val)) / 100;
	if (res < 0)
		res = 0;
	return res;
}
int GetItemRequiredStrength(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return 0;
	const ItemTxt *txt = GetItemTxt(item);

	int res = 0;
	if (txt)
		res = txt->reqstr;

	if (item_data->dwFlags & ITEM_FLAG_ETHEREAL)
		res -= 10;

	const StatArr *sb = ReadProcess((DWORD)item->pStats + 0x48, &sb);
	int val = (signed long)GetItemStatValue(*sb, 91, 0); // Requirements
	if (val)
		res = (res * (100 + val)) / 100;
	if (res < 0)
		res = 0;
	return res;
}
int GetItemRequiredLevel(const UnitAny *item) {
	const ItemData *item_data = GetItemData(item);
	if (!item_data)
		return 0;
	if (!(item_data->dwFlags & ITEM_FLAG_IDENTIFIED))
		return 0;
	const ItemTxt *txt = GetItemTxt(item);

	int res = 0;
	if (txt)
		res = txt->levelreq;

	switch (item_data->dwQuality) {
		case ITEM_QUALITY_MAGIC:
			if (item_data->wPrefix[0]) {
				int v = GetMagicSuffixLevelReq(item_data->wPrefix[0]);
				if (res < v)
					res = v;
			}
			if (item_data->wSuffix[0]) {
				int v = GetMagicSuffixLevelReq(item_data->wSuffix[0]);
				if (res < v)
					res = v;
			}
			break;
		case ITEM_QUALITY_SET:
			if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED) {
				const SetTxt *set;
				TxtGet(&set, item_data->dwQuality2);
				if (set) {
					int v = set->lvlreq;
					if (res < v)
						res = v;
				}
			}
			break;
		case ITEM_QUALITY_RARE:
		case ITEM_QUALITY_CRAFT:
			if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED) {
				int v, max = 0, c = 10;
				for (int i = 0; i<3; ++i) {
					if (item_data->wSuffix[i])
						c += 3;
					if (item_data->wPrefix[i])
						c += 3;
					v = GetMagicSuffixLevelReq(item_data->wSuffix[i]);
					if (max < v)
						max = v;
					v = GetMagicSuffixLevelReq(item_data->wPrefix[i]);
					if (max < v)
						max = v;
				}
				if (item_data->dwQuality == ITEM_QUALITY_CRAFT)
					max += c;
				if (max > 98)
					max = 98;
				if (res < max)
					res = max;
			}
			break;
		case ITEM_QUALITY_UNIQUE:
			if (item_data->dwFlags & ITEM_FLAG_IDENTIFIED) {
				const UniqueTxt *unique;
				TxtGet(&unique, item_data->dwQuality2);
				if (unique) {
					int v = unique->lvlreq;
					if (res < v)
						res = v;
				}
			}
			break;
	}
	if (item->pInventory) {
		const Inventory *inv = ReadProcess(item->pInventory, &inv);
		if (inv->dwSignature == 0x1020304) {
			DWORD next = inv->pFirstItem;
			while (next) {
				const UnitAny *item = ReadProcess(next, &item);
				const ItemData *item_data = ReadProcess(item->pUnitData, &item_data);
				next = item_data->pNextInvItem;
				int v = GetItemRequiredLevel(item);
				if (res < v)
					res = v;
			}
		}
	}
	const StatArr *_arr = ReadProcess((DWORD)item->pStats + 0x48, &_arr);
	res += GetItemStatValue(*_arr, 92, 0);
	return res;
}
const GemTxt * GetItemGem(const UnitAny *item) {
	int n = Txt<GemTxt>::Count();
	for (int i = 0; i<n; ++i) {
		const GemTxt *txt;
		TxtGet(&txt, i);
		if (txt->code == item->dwTxtFileNo)
			return txt;
	}
	return NULL;
}
BYTE GetItemStackable(const UnitAny *item) {
	const ItemTxt *txt = GetItemTxt(item);
	if (txt)
		return txt->stackable;
	else
		return 0;
}
const char *GetItemRunes(const UnitAny *item) {
	static char buff[100];
	char *p = buff;
	if (item->pInventory) {
		const Inventory *inv = ReadProcess(item->pInventory, &inv);
		if (inv->dwSignature == 0x1020304) {
			DWORD next = inv->pFirstItem;
			while (next) {
				const UnitAny *item = ReadProcess(next, &item);
				const ItemData *item_data = ReadProcess(item->pUnitData, &item_data);
				next = item_data->pNextInvItem;
				const GemTxt *gem = GetItemGem(item);
				if (gem) {
					strcpy(p, gem->letter);
					p += strlen(p);
				}
			}
		}
	}
	if (p == buff)
		return NULL;
	*p = 0;
	return buff;
}
std::vector<Stat> StatArrToVector(const StatArr &arr) {
	const Stat *s = ReadProcess(arr.pStat, &s, arr.wStatCount);
	std::vector<Stat> res;
	res.resize(arr.wStatCount);
	memcpy(res.data(), s, sizeof(Stat)*arr.wStatCount);
	return res;
}
bool StatOrderCmp(const Stat &a, const Stat &b) {
	if (a.wStatIndex != b.wStatIndex)
		return a.wStatIndex < b.wStatIndex;
	return a.wSubIndex < b.wSubIndex;
}
void DumpItem(DWORD itemp, int x, int y, int width = 0, int height = 0, int ida = 0) {
	ida = x = y = 0;//removes compiler warning (UNUSED VAR)
	const UnitAny *item = ReadProcess(itemp, &item);
	const ItemData &item_data = *GetItemData(item);
	if (!(&item_data))
		return;
	const ItemTxt *txt = GetItemTxt(item);
	if (!txt)
		return;

	//char newinvfile[30];
	printf("\n%d;,%d;,%d;,%d;,%s;,%s;,%s;\n", width, height, txt->invwidth, txt->invheight, GetItemInvFile(item), txt->invu, txt->invs);
	// ======== TITLE
	const char *rune_word_name = GetItemRuneWordName(item);
	if (rune_word_name)
		printf("<runeword>%s\n", rune_word_name);
	else {
		printf("%s", GetItemNameColor(item));
		const char* temp = GetItemName(item);
		std::string tstr(temp, strnlen(temp, 240));
		auto it = tstr.find(-61);
		while (it != std::string::npos) {
			tstr.replace(tstr.begin() + it, tstr.begin() + it + 4, "");
			it = tstr.find(-61);
		}
		tstr = tstr.substr(tstr.rfind('\n') + 1, std::string::npos);
		printf("%s\n", tstr.c_str());
	}
	const char* temp = GetItemBaseName(item);
	std::string tstr(temp, strnlen(temp, 120));
	auto it = tstr.find(-61);
	while (it != std::string::npos) {
		tstr.replace(tstr.begin() + it, tstr.begin() + it + 4, "");
		it = tstr.find(-61);
	}
	tstr = tstr.substr(tstr.rfind('\n') + 1, std::string::npos);
	std::string tstr2 = std::string(txt->name).substr(0, 4);
	if (tstr2.size() > 3 && tstr2[3] == 32) {
		tstr2 = tstr2.substr(0, 3);
	}
	printf("<%s>%s\n", tstr2.c_str(), tstr.c_str());//<item code> + type

	const char *runes = GetItemRunes(item);
	if (runes)
		printf("<b>%s\n", runes);

	#pragma region invisible stats
	//============ Invisible Stats

	if (item_data._1[0] != 1)
		printf("%d\n", item_data._1[0]);
	if (item_data._1[1] != 666)
		printf("_1[1]: %d\n", item_data._1[1]);
	if (item_data._2[0] != 0)
		printf("_2[0]: %d\n", item_data._2[0]);
	if (item_data._2[1] != 0)
		printf("_2[1]: %d\n", item_data._2[1]);
	if (item_data._3[0] != 0)
		printf("_3[0]: %d\n", item_data._3[0]);
	if (item_data._3[1] != 0)
		printf("_3[1]: %d\n", item_data._3[1]);
	if (item_data._3[2] != 0)
		printf("_3[1]: %d\n", item_data._3[2]);
	if (item_data.dwQuality != ITEM_QUALITY_LOW
		&& item_data.dwQuality != ITEM_QUALITY_HIGH
		&& item_data.dwQuality != ITEM_QUALITY_SET
		&& item_data.dwQuality != ITEM_QUALITY_UNIQUE
		&& !(item_data.dwFlags & ITEM_FLAG_EAR)
		&& item_data.dwQuality2 != 0)
		printf("Quality2: %d\n", item_data.dwQuality2);
	if (item_data.dwQuality != 6 && item_data.dwQuality != 8 && item_data.wRarePrefix)
		printf("RarePrefix: %d\n", item_data.wRarePrefix);
	if (item_data.dwQuality != 6 && item_data.dwQuality != 8 && item_data.wRareSuffix)
		printf("RareSuffix: %d\n", item_data.wRareSuffix);
	if (item_data._14)
		printf("_14: %d\n", item_data._14);
	/*if ((item_data.ItemLocation != 255 && item_data.ItemLocation > 4)
	|| (item_data.ItemLocation == 255 && (item_data.BodyLocation > 12 || item_data.BodyLocation == 0))
	|| (item_data.ItemLocation != 255 && item_data.BodyLocation != 0)) {
	printf("BodyLocation: %d\n", item_data.BodyLocation);
	printf("ItemLocation: %d\n", item_data.ItemLocation);
	}*/
	if (item_data._7)
		printf("_7: %d\n", item_data._7);
	if (item_data._8)
		printf("_8: %d\n", item_data._8);

	if (!(item_data.dwFlags & (ITEM_FLAG_EAR | ITEM_FLAG_PERSONALIZED))
		&& item_data.personalizedName[0] != 0)
		printf("Personalized: %s\n", item_data.personalizedName);
	if (item_data._10)
		printf("_10: %d\n", item_data._10);

	#pragma endregion

	// =========== Base Stats
	if (item_data.dwFlags & ITEM_FLAG_EAR) {
		int c = item_data.dwQuality2;
		const CharStatsTxt *cs_txt;
		TxtGet(&cs_txt, c);
		if (cs_txt)
			printf("<b>%s\n", getStringByName(cs_txt->name));
		else
			printf("<b>Bad Character Class(%d)\n", c);
		printf("<b>Level: %d\n", item_data.EarLevel);
	}
	else if (item_data.EarLevel)
		printf("<b>EarLevel: %d\n", item_data.EarLevel);

	const StatList *sl = ReadProcess(item->pStats, &sl);
	const StatArr *sa = (StatArr*)&sl->pStat;
	const StatArr *sb = ReadProcess((DWORD)item->pStats + 0x48, &sb);

	#pragma region base stats

	// Throw Damage
	int val = GetItemStatValue(*sa, 160, 0);
	if (val) {
		int v2 = GetItemStatValue(*sb, 160, 0);
		int v3 = GetItemStatValue(*sa, 159, 0);
		int v4 = GetItemStatValue(*sb, 159, 0);
		if (val == v2 && v3 == v4)
			printf("<b>%s %d %s %d\n", getStringByNum(3467), v4, getStringByNum(3464), v2);
		else
			printf("<b>%s %d %s %d\n", getStringByNum(3467), v4, getStringByNum(3464), v2);
	}

	// One-Hand Damage
	val = GetItemStatValue(*sa, 22, 0);
	if (val) {
		int v2 = GetItemStatValue(*sb, 22, 0);
		int v3 = GetItemStatValue(*sa, 21, 0);
		int v4 = GetItemStatValue(*sb, 21, 0);
		if (val == v2 && v3 == v4)
			printf("<b>%s %d %s %d\n", getStringByName("ItemStats1l"), v4, getStringByNum(3464), v2);
		else
			printf("<b>%s %d %s %d\n", getStringByName("ItemStats1l"), v4, getStringByNum(3464), v2);
	}

	// Two-Hand Damage
	val = GetItemStatValue(*sa, 24, 0);
	if (val) {
		int v2 = GetItemStatValue(*sb, 24, 0);
		int v3 = GetItemStatValue(*sa, 23, 0);
		int v4 = GetItemStatValue(*sb, 23, 0);
		if (val == v2 && v3 == v4)
			printf("<b>%s %d %s %d\n", getStringByName("ItemStats1m"), v4, getStringByNum(3464), v2);
		else
			printf("<b>%s %d %s %d\n", getStringByName("ItemStats1m"), v4, getStringByNum(3464), v2);
	}

	BYTE quantity = GetItemStackable(item);
	if (quantity) {
		val = GetItemStatValue(*sa, 70, 0);
		printf("<b>%s %d\n", getStringByNum(3462), val);
	}

	// Defense
	val = GetItemStatValue(*sa, 31, 0);
	if (val) {
		int v2 = GetItemStatValue(*sb, 31, 0);
		if (val == v2)
			printf("<b>%s %d\n", getStringByName("ItemStats1h"), v2);
		else
			printf("<b>%s %d\n", getStringByName("ItemStats1h"), v2);
	}

	// Chance to Block
	val = GetItemStatValue(*sa, 20, 0);
	if (val) {
		int v2 = GetItemStatValue(*sb, 20, 0);
		if (val == v2)
			printf("<b>%s %d%%\n", getStringByNum(11018), v2);
		else
			printf("<b>%s %d%%\n", getStringByNum(11018), v2);
	}

	// Durability
	if (!quantity) {
		val = GetItemStatValue(*sa, 73, 0);
		if (val) {
			int v2 = GetItemStatValue(*sb, 73, 0);
			int v3 = GetItemStatValue(*sb, 72, 0);
			if (val == v2)
				printf("<b>%s %d %s %d\n", getStringByName("ItemStats1d"), v3, getStringByName("ItemStats1j"), v2);
			else
				printf("<b>%s %d %s %d\n", getStringByName("ItemStats1h"), v3, getStringByName("ItemStats1j"), v2);
		}
	}

	// Class Only
	const char *classonly = GetItemClassOnly(item);
	if (classonly) {
		printf("<b>%s\n", classonly);
	}

	// Dexterity
	val = GetItemRequiredDexterity(item);
	if (val) {
		printf("<b>%s %d\n", getStringByName("ItemStats1f"), val);
	}

	// Strength
	val = GetItemRequiredStrength(item);
	if (val) {
		printf("<b>%s %d\n", getStringByName("ItemStats1e"), val);
	}

	// Level
	val = GetItemRequiredLevel(item);
	if (val) {
		printf("<b>%s %d\n", getStringByName("ItemStats1p"), val);
	}

	// Class [Speed]
	const char *wpncls = (txt ? GetItemWeaponClassName(txt->type) : NULL);
	if (wpncls) {
		//int val = GetItemStatValue(*sa, 68, 0);
		int v2 = GetItemStatValue(*sb, 68, 0);
		//if (val == v2)
		printf("<b>%s [%d]\n", wpncls, v2 * -1);
		//else
		//printf("%s %d\n", wpncls, v2);
	}
	//item level
	printf("<b>Item Level: %d\n", item_data.dwItemLevel);

	// Velocity
	val = GetItemStatValue(*sa, 67, 0);
	if (val) {
		int v2 = GetItemStatValue(*sb, 67, 0);
		if (val == v2)
			printf("<b>%s %d\n", getStringByNum(4324), v2);
		else
			printf("<b>%s %d\n", getStringByNum(4324), v2);
	}
	if (sl->wStatCount) {
		Stat *s = ReadProcess(sl->pStat, &s, sl->wStatCount);
		for (int j = 0; j<sl->wStatCount; ++j) {
			int id = s[j].wStatIndex;
			if (id != 20 // chance to block
				&& id != 21 // min dmg
				&& id != 22 // max dmg
				&& id != 23 // 2h min dmg
				&& id != 24 // 2h max dmg
				&& id != 31 // defense
				&& id != 67 // velocity
				&& id != 68 // speed
				&& id != 70 // quantity
				&& id != 72 // durability current
				&& id != 73 // durability max
				&& id != 159// min throw
				&& id != 160// max throw
				&& id != 194// sockets
				)
				printf("<b>%d %s\n", id, GetItemStatDesc(item, StatArrToVector(*(StatArr*)&sl->pStat), s[j].wStatIndex, s[j].wSubIndex, s[j].dwStatValue));
		}
	}

	#pragma endregion

	// ======= Magic Stats
	std::vector<Stat> stats;
	{
		DWORD next = sl->pFirstChild;
		while (next) {
			sl = ReadProcess(next, &sl);
			next = sl->pNext;
			if (sl->wStatCount) {
				Stat *s = ReadProcess(sl->pStat, &s, sl->wStatCount);
				for (int j = 0; j<sl->wStatCount; ++j)
					stats.push_back(s[j]);
			}
			if (!(sl->_1[4] & 0x40)) {
				sl = ReadProcess(sl->pFirstChild, &sl);
				if (sl->wStatCount) {
					Stat *s = ReadProcess(sl->pStat, &s, sl->wStatCount);
					for (int j = 0; j<sl->wStatCount; ++j)
						stats.push_back(s[j]);
				}
			}
		}
	}
	std::sort(stats.begin(), stats.end(), StatOrderCmp);
	for (UINT i = 0; i<stats.size(); ++i)
		if ((UINT)(i + 1) < stats.size())
			if (stats[i].wStatIndex == stats[i + 1].wStatIndex
				&& stats[i].wSubIndex == stats[i + 1].wSubIndex) {
				stats[i].dwStatValue += stats[i + 1].dwStatValue;
				stats.erase(stats.begin() + i + 1);
				--i;
			}
	for (int j = StatsOrder.size() - 1; j >= 0; --j) {
		int k = FindInStats(stats.data(), stats.size(), StatsOrder[j]);
		if (k >= 0) {
			for (; k < (int)stats.size() && stats[k].wStatIndex == StatsOrder[j]; ++k);
			for (--k; k >= 0 && stats[k].wStatIndex == StatsOrder[j]; --k) {
				const char *str = GetItemStatDesc(item, stats, stats[k].wStatIndex, stats[k].wSubIndex, stats[k].dwStatValue);
				if (str[0] && strlen(str) > 0) {
					if (str[strlen(str) - 1] == '\n') {
						std::string tstr(str);
						tstr.pop_back();
						printf("<m>%s\n", tstr.c_str());
					}
					else
						printf("<m>%s\n", str);
				}
			}
		}
	}

	// ========== Damage to Undead
	if (item_data.dwFlags & ITEM_FLAG_IDENTIFIED) {
		val = GetItemStatValue(*sb, 122, 0);
		if (!val && ItemHasClass(item, 57)) {
			printf("<m>%s\n", GetItemStatDesc(item, StatArrToVector(*sa), 122, 0, 0));
		}
	}
	else {
		// =========== Unidentified
		printf("<m>%s\n", getStringByNum(3455));
	}

	// ========== Ethereal, Socketed
	int ethereal = item_data.dwFlags & ITEM_FLAG_ETHEREAL;
	int sockets = GetItemStatValue(*sa, 194, 0);
	if (ethereal || sockets) {
		if (ethereal)
			printf("<m>%s", getStringByNum(22745)); // ethereal
		if (sockets) {
			if (ethereal)
				printf(", %s (%d)", getStringByNum(3453), sockets); // socketed");
			else
				printf("<m>%s (%d)", getStringByNum(3453), sockets); // socketed
		}
		printf("\n");
	}
	for (UINT k = 0; k < stats.size(); ++k) {
		const char *str = GetInvisibleStatDesc(stats[k].wStatIndex, stats[k].wSubIndex, stats[k].dwStatValue);
		if (str[0]) {
			printf("<m>%s\n", str);
		}
	}
}
void DumpBody(const InventoryStore &is) {
	if (!is.Width || !is.Height || !is.pArray)
		return;
	DWORD *Array = ReadProcess(is.pArray, &Array, is.Width*is.Height);
	struct { int x, y, w, h; } coords[] =
	{
		0, -50, 2, 4, // not equiped
		116, -251, 2, 2, // helm
		188, -225, 1, 1, // amulet
		116, -180, 2, 3, // armor
		1, -209, 2, 4, // weapon1
		231, -209, 2, 4, // shield1
		74, -80, 1, 1, // ring1
		188, -80, 1, 1, // ring2
		117, -80, 2, 1, // sash
		234, -80, 2, 2, // boots
		2, -77, 2, 2, // gloves
		294, -209, 2, 4, // weapon2
		352, -209, 2, 4, // shield2
	};

	for (int y = 0; y<is.Height; ++y) {
		for (int x = 0; x<is.Width; ++x) {
			DWORD item = Array[x + y*is.Width];
			if (item) {
				UINT id;
				for (id = 0; id < ItemsProcessed.size(); ++id)
					if (ItemsProcessed[id] == item)
						break;
				if (id != ItemsProcessed.size())
					continue;
				ItemsProcessed.push_back(item);
				DumpItem(item,
					coords[x].x,
					coords[x].y,
					x,
					-1);
			}
		}
	}
}
void DumpInventory(const InventoryStore &is, int id = 0) {
	id = 0;//removes compiler warning (UNUSED VAR)
	if (!is.Width || !is.Height || !is.pArray)
		return;
	DWORD *Array = ReadProcess(is.pArray, &Array, is.Width*is.Height);
	for (int y = 0; y<is.Height; ++y) {
		for (int x = 0; x<is.Width; ++x) {
			DWORD item = Array[x + y*is.Width];
			if (item) {
				UINT id;
				for (id = 0; id < ItemsProcessed.size(); ++id)
					if (ItemsProcessed[id] == item)
						break;
				if (id != ItemsProcessed.size())
					continue;
				ItemsProcessed.push_back(item);
				DumpItem(item, 0, 0, x, y, id);
			}
		}
	}
}
const char *GetNPCName(const UnitAny *monster) {
	if (monster->dwType != 1)
		return "";
	DWORD *ptr = ReadProcess(monster->pUnitData + 0x2C, &ptr);
	const char *name = ReadProcess(*ptr, &name, 28);
	return name;
}
std::string GetBits(size_t const size, void const * const ptr) {
	std::string str = "";
	unsigned char *b = (unsigned char*)ptr;
	unsigned char byte;
	int i, j;

	for (i = size - 1; i >= 0; i--) {
		for (j = 7; j >= 0; j--) {
			byte = b[i] & (1 << j);
			byte >>= j;
			str = int_to_str((int)byte) + str;
		}
	}
	return str;
}
std::string GetNormalWps(const Waypoint *wp) {
	std::string bits = "";
	bits += GetBits(1, &wp->norm_act1);
	bits += GetBits(1, &wp->norm_act2);
	bits += GetBits(1, &wp->norm_act3);
	bits += GetBits(1, &wp->norm_act4);
	bits += GetBits(1, &wp->norm_act5);
	return bits;
}
std::string GetNightmareWps(const Waypoint *wp) {
	std::string bits = "";
	bits += GetBits(1, &wp->nightmare_act1);
	bits += GetBits(1, &wp->nightmare_act2);
	bits += GetBits(1, &wp->nightmare_act3);
	bits += GetBits(1, &wp->nightmare_act4);
	bits += GetBits(1, &wp->nightmare_act5);
	return bits;
}
std::string GetHellWps(const Waypoint *wp) {
	std::string bits = "";
	bits += GetBits(1, &wp->hell_act1);
	bits += GetBits(1, &wp->hell_act2);
	bits += GetBits(1, &wp->hell_act3);
	bits += GetBits(1, &wp->hell_act4);
	bits += GetBits(1, &wp->hell_act5);
	return bits;
}
bool DumpPlayer(DWORD D2Client_base, const UnitAny *PlayerUnit) {
	ItemsProcessed.clear();
	InventoryStore *is;
	const Inventory *inv = ReadProcess(PlayerUnit->pInventory, &inv);
	if (inv->dwSignature != 0x01020304) {
		printf("Wrong signature: %08X", inv->dwSignature);
		return false;
	}
	else {
		const DWORD *gi_p = ReadProcess(D2Client_base + 0x109738, &gi_p);
		const GameStructInfo *gameinfo = ReadProcess(*gi_p, &gameinfo);

		std::string tbasepath = g_TokenChestPath;
		if (tbasepath.size() == 0) tbasepath = getexedir();
		char basepath[MAX_PATH];
		strncpy(basepath, tbasepath.c_str(), sizeof(basepath));
		basepath[sizeof(basepath) - 1] = 0;

		char filename[MAX_PATH];
		if (gameinfo->szRealmName[0]) {
			if (createfolder(tbasepath + "\\char data") == 0)
				sprintf_s(filename, "%s\\char data\\%s_%s_%s.char", basepath, gameinfo->szRealmName, gameinfo->szAccountName, gameinfo->szCharName);
			else {
				MessageBox(NULL, str_to_wstr("Unable to create folder.\n" + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
				return false;
			}
		}
		else {
			if (createfolder(tbasepath + "\\char data") == 0)
				sprintf_s(filename, "%s\\char data\\Single_Player_%s.char", basepath, gameinfo->szCharName);
			else {
				MessageBox(NULL, str_to_wstr("Unable to create folder.\n" + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
				return false;
			}
		}
		freopen(filename, "w+", stdout);

		if (gameinfo->szRealmName[0]) {
			printf("Realm: %s\n", gameinfo->szRealmName);
			std::string account_name(gameinfo->szAccountName);
			std::transform(account_name.begin(), account_name.end(), account_name.begin(), tolower);
			printf("Account: %s\n", account_name.c_str());
		}
		else {
			printf("Realm: Single_Player\n");
			printf("Account: Single_Player\n");
		}
		const char *name = ReadProcess(PlayerUnit->pUnitData, &name, 0x10);
		printf("Character name: %s\n", name);

		is = ReadProcess(inv->pStores, &is, inv->dwStoresCount);
		// Stash
		printf("\n#stash#\n");
		if (6 < inv->dwStoresCount)
			DumpInventory(is[6], 6);
		// Body
		printf("\n#body#\n");
		if (0 < inv->dwStoresCount)
			DumpBody(is[0]);
		// Backpack
		printf("\n#inventory#\n");
		if (2 < inv->dwStoresCount)
			DumpInventory(is[2]);
		// Cube
		printf("\n#cube#\n");
		if (5 < inv->dwStoresCount)
			DumpInventory(is[5]);

		printf("\n#mercenary#\n");
		// Hireling
		DWORD MercID = (DWORD)-1;
		DWORD *tmp = ReadProcess(D2Client_base + 0x11CE30, &tmp);
		DWORD pRoster = *tmp;
		while (pRoster) {
			const DWORD *Roster = ReadProcess(pRoster, &Roster, 13);
			if (Roster[1] == 7
				&& Roster[3] == PlayerUnit->dwUnitId
				&& !Roster[8]) {
				MercID = Roster[2];
				break;
			}
			pRoster = Roster[12];
		}
		DWORD pMerc = 0;
		const DWORD *pUnits = ReadProcess(D2Client_base + 0x1047B8 + 0x200, &pUnits, 128);
		for (int i = 0; i<128; ++i) {
			DWORD next = pUnits[i];
			while (next) {
				const UnitAny *unit = ReadProcess(next, &unit);
				//printf("%08X %d %08X %ls<br>",unit.dwUnitId,unit.dwType,next,GetNPCName(unit));
				if (unit->dwType == 1
					&& unit->dwUnitId == MercID)
					pMerc = next;
				next = unit->pRoomNext;
			}
		}
		if (pMerc) {
			const UnitAny *MercenaryUnit = ReadProcess(pMerc, &MercenaryUnit);
			const Inventory *inv = ReadProcess(MercenaryUnit->pInventory, &inv);
			if (inv->dwSignature != 0x01020304 || inv->dwStoresCount != 1) {
				fprintf(stderr, "Wrong signature: %08X %d\n", inv->dwSignature, inv->dwStoresCount);
				return false;
			}
			else {
				is = ReadProcess(inv->pStores, &is, inv->dwStoresCount);
				DumpBody(is[0]);
			}
		}

		//player
		printf("\n#character stats#\n\n");

		//TODO dump waypoint info
		const DWORD *wp = ReadProcess(D2Client_base + 0x1088FD, &wp);
		const Waypoint *PlayerWaypoint = ReadProcess(*wp, &PlayerWaypoint);
		std::string norm = GetNormalWps(PlayerWaypoint);
		std::string nightmare = GetNightmareWps(PlayerWaypoint);
		std::string hell = GetHellWps(PlayerWaypoint);
		printf("wp:%s%s%s\n", norm.c_str(), nightmare.c_str(), hell.c_str());

		//TODO dump skills
		/*
		[6-35]
		[36-65]
		[66-95]
		[96-125] 
		[126-155]
		[221-250]
		[251-280]
		*/		

		//TODO dump quests

		//dump character stats
		const StatArr *hm = ReadProcess(PlayerUnit->pStats + 0x48, &hm);
		const Stat *hz = ReadProcess(hm->pStat, &hz, hm->wStatCount);
		printf("-1:%d//character class\n", PlayerUnit->dwTxtFileNo);

		static std::vector<int> stat_ids = 
			{STAT_TOBLOCK, STAT_DAMAGERESIST, STAT_MAGICRESIST, 
			STAT_FIRERESIST, STAT_COLDRESIST, STAT_LIGHTRESIST, STAT_POISONRESIST,
			STAT_MAXMAGICRESIST, STAT_MAXFIRERESIST, STAT_MAXCOLDRESIST, STAT_MAXLIGHTRESIST, STAT_MAXPOISONRESIST, 
			STAT_NORMAL_DAMAGE_REDUCTION, STAT_MAGIC_DAMAGE_REDUCTION, /*STAT_ITEM_POISONLENGTHRESIST,*/
			STAT_ITEM_ABSORBFIRE_PERCENT, STAT_ITEM_ABSORBCOLD_PERCENT, STAT_ITEM_ABSORBLIGHT_PERCENT, STAT_ITEM_ABSORBMAGIC_PERCENT,
			STAT_ITEM_ABSORBFIRE, STAT_ITEM_ABSORBLIGHT, STAT_ITEM_ABSORBCOLD, STAT_ITEM_ABSORBMAGIC,
			STAT_ITEM_MAGICBONUS,
			STAT_ITEM_FASTERCASTRATE, STAT_ITEM_FASTERATTACKRATE, STAT_ITEM_FASTERMOVEVELOCITY, STAT_ITEM_FASTERGETHITRATE,
			STAT_ITEM_ADDEXPERIENCE, /*STAT_MANARECOVERYBONUS,*/
			STAT_HPREGEN, 
			STAT_ITEM_OPENWOUNDS, STAT_ITEM_CRUSHINGBLOW, STAT_ITEM_DEADLYSTRIKE
		};
		
		int dex = 0, lvl = 0;

		for (int i = 0; i < hm->wStatCount; ++i) {
		/*	const ItemStatCostTxt *txt;
			TxtGet(&txt, hz[i].wStatIndex);*/
			if (hz[i].wStatIndex == STAT_DEXTERITY)
				dex = hz[i].dwStatValue;
			else if (hz[i].wStatIndex == STAT_LEVEL)
				lvl = hz[i].dwStatValue;
			if (hz[i].wStatIndex == STAT_TOBLOCK) {//block chance			
				int cls = PlayerUnit->dwTxtFileNo;
				int block = hz[i].dwStatValue + (cls == 3 ? 30 : cls == 0 || cls == 4 || cls == 6 ? 25 : 20);
				int totalblock = (block * (dex - 15)) / (lvl * 2);
				totalblock = totalblock > 75 ? 75 : totalblock;
				printf("%d:%d//%s\n", hz[i].wStatIndex, totalblock, ("Chance to Block: " + int_to_str(totalblock) + "%").c_str());
			}
			else if (strcmp(gameinfo->szRealmName, "D2PK") == 0 && 
				(hz[i].wStatIndex == STAT_FIRERESIST 
				|| hz[i].wStatIndex == STAT_COLDRESIST 
				|| hz[i].wStatIndex == STAT_LIGHTRESIST 
				|| hz[i].wStatIndex == STAT_POISONRESIST)) {
				const char *desc = GetInvisibleStatDesc(hz[i].wStatIndex, hz[i].wSubIndex, hz[i].dwStatValue);
				if (desc[0] == 0)
					desc = GetStatDesc(hz[i].wStatIndex, hz[i].wSubIndex, hz[i].dwStatValue, 0);
				printf("%d:%d//%s\n", hz[i].wStatIndex, hz[i].dwStatValue + 30, desc);
			}
			else {
				const char *desc = GetInvisibleStatDesc(hz[i].wStatIndex, hz[i].wSubIndex, hz[i].dwStatValue);
				if (desc[0] == 0)
					desc = GetStatDesc(hz[i].wStatIndex, hz[i].wSubIndex, hz[i].dwStatValue, 0);
				printf("%d:%d//%s\n", hz[i].wStatIndex, (hz[i].wStatIndex >= 6 && hz[i].wStatIndex <= 11 ? hz[i].dwStatValue / 256 : hz[i].dwStatValue), desc);
			}
			auto it = std::find(stat_ids.begin(), stat_ids.end(), hz[i].wStatIndex);
			if (it != stat_ids.end())
				stat_ids.erase(it);
		}
		for (UINT i = 0; i < stat_ids.size(); i++) {
			const char *desc = GetInvisibleStatDesc((WORD)stat_ids[i], (WORD)stat_ids[i], 0);
			if (desc[0] == 0)
				desc = GetStatDesc((WORD)stat_ids[i], (WORD)stat_ids[i], 0, 0);
			printf("%d:%d//%s\n", stat_ids[i], 0, desc);
		}

		fclose(stdout);

		GameStructInfo tginfo = *(GameStructInfo*)gameinfo;
		COPYDATASTRUCT cds = {0};
		cds.dwData = WM_DUMPED;
		cds.cbData = sizeof(tginfo);
		cds.lpData = &tginfo;
		if (!g_TokenChest)
			g_TokenChest = FindWindow(NULL, L"TokenChest");
		SendMessage(g_TokenChest, WM_COPYDATA, (WPARAM)g_info.hwnd, (LPARAM)(LPVOID)&cds);
	}
	
	return true;
}
bool DumpGame(HANDLE hProcess, DWORD D2Client_base) {
	ProcessHandle = hProcess;

	RemoteData<PR>::Release();
	DWORD* PlayerUnit_p = ReadProcess(D2Client_base + 0x11D050, &PlayerUnit_p);
	if (!(*PlayerUnit_p))
		return false;
	const UnitAny *PlayerUnit = ReadProcess(*PlayerUnit_p, &PlayerUnit);
	return DumpPlayer(D2Client_base, PlayerUnit);
}
DWORD getDumps() {
	if (!AdjustPrivilege()) {
		MessageBox(NULL, str_to_wstr("Can't adjust priveledge, try to run with administrator rights.\n").c_str(), L"ERROR", MB_OK);
		return 1;
	}
	if (int ret = initStringTables()) {
		MessageBox(NULL, str_to_wstr("Can't init string tables (locale strings) [" + int_to_str(ret) + "].\n").c_str(), L"ERROR", MB_OK);
		return 2;
	}
	if (!initBins()) {
		MessageBox(NULL, str_to_wstr("Error with one or more .bin files.\nAbort.\n").c_str(), L"ERROR", MB_OK);
		return 3;
	}
	if (!initStatOrder()) {
		MessageBox(NULL, str_to_wstr("Can't init Stat Order.\n").c_str(), L"ERROR", MB_OK);
		return 4;
	}

#define TEST
#define WRITEITEMS
#define WRITEITEMCODES
//#define PRINTITEMS
#ifdef _DEBUG
#ifdef TEST

	std::vector<std::string> g_props;
	std::ifstream file;
	file.open("unique props.txt");
	if (file.is_open()) {
		while (file.good()) {
			std::string s;
			std::getline(file, s);
			if (!s.size()) continue;
			std::string text = s.substr(s.find(".") + 2, s.rfind("//") - s.find(".") - 2);
			g_props.push_back(text);
		}
	}
	file.close();

	auto getproptext = [&](UniqueStats us) -> std::string {
		const ItemStatCostTxt *stat;
		TxtGet(&stat, us.prop);
		if (!stat)
			return "";

		char out[240];
		std::string range = us.min == us.max ? int_to_str(us.min) : "[" + int_to_str(us.min) + "-" + int_to_str(us.max) + "]";

		if (g_props.size() <= us.prop) {
			printf("add more nums to unique props.txt");
			return "-1";
		}

		if (g_props[us.prop].size() == 0) {
			sprintf(out, "--------------Unknown %d, %d%s, %d, %d--------------", us.prop, us.par, us.par ? std::string("(" + std::string(GetSkillName(us.par)) + ")").c_str() : "", us.min, us.max);
			return std::string(out);
		}
		else if (us.prop <= 16
			|| us.prop >= 27 && us.prop <= 116
			|| us.prop >= 118 && us.prop <= 120
			|| us.prop == 138
			|| us.prop == 139
			|| us.prop >= 180 && us.prop <= 181
			|| us.prop >= 232 && us.prop <= 235
			|| us.prop == 242
			|| us.prop >= 244 && us.prop <= 247
			|| us.prop >= 251 && us.prop <= 255
			|| us.prop == 258
			|| us.prop == 264
			|| us.prop == 268
			|| us.prop >= 270 && us.prop <= 275
			|| us.prop == 280
			|| us.prop == 293
			|| us.prop == 294
			|| us.prop >= 296 && us.prop <= 298
			|| us.prop == 301
			|| us.prop >= 322 && us.prop <= 327
			|| us.prop == 332
			|| us.prop == 336
			|| us.prop == 337
			|| us.prop >= 354 && us.prop <= 356
			|| us.prop == 359
			|| us.prop == 377
			|| us.prop == 379
			)
		{
			sprintf(out, g_props[us.prop].c_str(), range.c_str());
		}
		else if (us.prop >= 17 && us.prop <= 26) {//min - max c/f/l/p damage + duration
			if (us.prop == 19 || us.prop == 26) {//freeze/poison duration
				if (range.find("-") != std::string::npos) {
					range = "[" 
						+ int_to_str(str_to_int(range.substr(range.find("[") + 1, range.find("-"))) / 25) 
						+ "-" 
						+ int_to_str(str_to_int(range.substr(range.find("-") + 1, range.find("]"))) / 25)
						+ "]";
				}
				else
					range = int_to_str(str_to_int(range) / 25);
			}
			if (us.prop == 24 || us.prop == 25) {//poison damage
				if (range.find("-") != std::string::npos) {
					range = "["
						+ int_to_str((int)((float)str_to_int(range.substr(range.find("[") + 1, range.find("-"))) / 3.415))
						+ "-"
						+ int_to_str((int)((float)str_to_int(range.substr(range.find("-") + 1, range.find("]"))) / 3.415))
						+ "]";
				}
				else
					range = int_to_str((int)((float)str_to_int(range) / 3.415));
			}
			sprintf(out, g_props[us.prop].c_str(), range.c_str());
		//	std::string ret(out);
		//	return ret + (us.prop == 19 || us.prop == 21 || us.prop == 23 || us.prop == 26 ? "\n" : "");
		}
		else if (us.prop == 117) {//animate as _____
			sprintf(out, g_props[us.prop].c_str(), range.c_str(), GetMonsterName(us.par));
		}
		else if (us.prop == 123) {//class skill
			const CharStatsTxt* cs_txt;
			TxtGet(&cs_txt, GetSkillCharClass(us.par));
			if (cs_txt)
				sprintf(out, g_props[us.prop].c_str(), range.c_str(), GetSkillName(us.par), getStringByNum(cs_txt->StrClassOnly));
			/*else
				sprintf(out, "error");*/
		}
		else if (us.prop == 126 || us.prop == 128) {//%s %s //+ tab skill class only
			const CharStatsTxt* cs_txt;
			TxtGet(&cs_txt, us.par / 3);
			char* s1 = getStringByNum(cs_txt->StrSkillTab[us.par % 3]);
			std::string s2(s1);
			char c[100];
			if (s2.find("%d") != std::string::npos) {
				s2.replace(s2.find("%d"), s2.find("%d") + 1, "%s");
				sprintf(c, s2.c_str(), range.c_str());
			}
			sprintf(out, g_props[us.prop].c_str(), c, getStringByNum(cs_txt->StrClassOnly));
		}
		else if (us.prop == 125 || us.prop == 129 || us.prop == 265) {//Level %s %s Aura When Equipped OR oskill
			sprintf(out, g_props[us.prop].c_str(), range.c_str(), GetSkillName(us.par));
		}
		else if (us.prop >= 131 && us.prop <= 133 
			|| us.prop >= 259 && us.prop <= 260 
			|| us.prop == 333 || us.prop == 341) {//%s%% Chance to cast level %s %s on ____
			sprintf(out, g_props[us.prop].c_str(), int_to_str(us.min).c_str(), int_to_str(us.max).c_str(), GetSkillName(us.par));
		}
		else if (us.prop >= 134 && us.prop <= 137 || us.prop == 140) {//Adds %s - %s f/l/m/c/_ damage
			sprintf(out, g_props[us.prop].c_str(), int_to_str(us.min).c_str(), int_to_str(us.max).c_str());
		}
		else if (us.prop == 138) {//+%s poison damage over %s seconds //par = duration in frames, min & max = damage (4000 == 999?)
			sprintf(out, g_props[us.prop].c_str(), range.c_str(), int_to_str(us.par / 25).c_str());
			//TODO poison damage fix
		}
		else if (us.prop >= 141 && us.prop <= 177) {//stat based on character level
			std::string perlvl = std::to_string((float)us.par / (us.prop == 151 || us.prop == 152 || us.prop == 172 || us.prop == 173 ? 2 : 8));
			sprintf(out, g_props[us.prop].c_str(), perlvl.substr(0, perlvl.find(".") + 3).c_str());
		}
		else if (us.prop == 179) {//Repairs 1 durability in %s seconds 
			sprintf(out, g_props[us.prop].c_str(), int_to_str(us.par != 0 ? 100 / us.par : 12).c_str());
		}
		else if (us.prop == 197 || us.prop == 198) {//stat based on time
			sprintf(out, g_props[us.prop].c_str(), range.c_str(), us.par == 0 ? "During Daytime" : us.par == 1 ? "Near Dusk" : us.par == 2 ? "During Nighttime" : "Near Dawn");
		}
		else if (us.prop == 243) {// Level %s %s (%s/%s Charges) //par = skill, min = max charge, max = skill level
			sprintf(out, g_props[us.prop].c_str(), int_to_str(us.max).c_str(), GetSkillName(us.par), int_to_str(us.min).c_str(), int_to_str(us.min).c_str());
		}
		else if (us.prop == 248) {//+%s-%s to Elemental Damage //adds min to max fire, cold, lite dmg
			char* temp = "+%s-%s Fire Damage\n+%s-%s Cold Damage\n+%s-%s Lightning Damage";
			sprintf(out, temp, int_to_str(us.min).c_str(), int_to_str(us.max).c_str(), int_to_str(us.min).c_str(), int_to_str(us.max).c_str(), int_to_str(us.min).c_str(), int_to_str(us.max).c_str());
		}
		else if (us.prop == 261) {//+%s to %s %s //random skill par = level, min = start range, max = end range
			const CharStatsTxt* cs_txt;
			TxtGet(&cs_txt, GetSkillCharClass(us.min));
			sprintf(out, g_props[us.prop].c_str(), int_to_str(us.par).c_str(), ("Random Skill " + range + "{" + GetSkillName(us.min) + "-" +GetSkillName(us.max) + "}").c_str(), getStringByNum(cs_txt->StrClassOnly));
		}
		else if (us.prop == 266) {//nv state
			sprintf(out, g_props[us.prop].c_str(), int_to_str(us.par).c_str());
		}
		else if (us.prop == 267 || us.prop == 269 || us.prop == 358) {//+x to %s Skill Levels //+ rand class skill levels
			sprintf(out, g_props[us.prop].c_str(), ("(Random Class" + range + ")").c_str());
		}
		else if (us.prop == 268) {//?????
			sprintf(out, "+%s%% to Cold Skill Damage\n+%s%% to Lightning Skill Damage\n+%s%% to Fire Skilll Damage\n", range.c_str(), range.c_str(), range.c_str());
		}
		else if (us.prop == 276) {//Level %s Armor Penetration Aura When Equipped\n(Nearby Enemies have %s%% Reduced Physical Resistance)
			sprintf(out, "Level %s Armor Penetration Aura When Equipped\n(Nearby Enemies have %s%% Reduced Physical Resistance)", range.c_str(), range.c_str());
		}
		else if (us.prop >= 277 && us.prop <= 279 || (us.prop >= 281 && us.prop <= 283) 
			|| (us.prop >= 285 && us.prop <= 288) || us.prop == 295 || us.prop == 318
			|| us.prop == 319 || us.prop == 320 || us.prop == 322 || us.prop == 357) {//%s%% x (Based on xstat)
			char perlvlmin[8];
			sprintf(perlvlmin, "%.2lf", (float)us.min * .03125);
			char perlvlmax[8];
			sprintf(perlvlmin, "%.2lf", (float)us.max * .03125);
			range = us.min == us.max ? perlvlmin : "[" + std::string(perlvlmin) + "-" + std::string(perlvlmax) + "]";
			sprintf(out, g_props[us.prop].c_str(), range.c_str());
		}
		else if (us.prop == 284) {
			sprintf(out, (g_props[us.prop] + "\n50%% chance to Parry an enemy Attack or Spell").c_str(), range.c_str(), GetSkillName(us.par));			
		}
		else if (us.prop == 291) {//Level %s Bulwark Aura When Equipped\n(Nearby allies have %s%% Increased Damage Reduction and Block Chance)
			sprintf(out, "Level %s Bulwark Aura When Equipped\n(Nearby allies have %s%% Increased Damage Reduction and Block Chance)", range.c_str(), range.c_str());
		}
		else if (us.prop == 249 || us.prop == 289 || us.prop == 292 || us.prop == 299
			|| us.prop == 302 || us.prop == 329 || us.prop == 330
			|| us.prop == 331 || us.prop == 382 || us.prop == 383) {//no parameter stats (plain text)
			sprintf(out, g_props[us.prop].c_str());
		}
		else if (us.prop == 300) {
			sprintf(out, "Fist of the Heavens' Holy Shock synergy is replaced with Fanaticism\n100%% of your Fist of the Heavens Skill Damage is converted to Physical Damage");
		}
		else if (us.prop == 303) {
			sprintf(out, "(Death Spear gains a 10%% Damage Synergy from Shout)\n(Death Spear requires a Shield equipped)");
		}
		else if (us.prop == 334) {
			sprintf(out, "+%s%% to Lightning Skill Damage\n+%s%% to Fire Skill Damage\n+%s%% to Cold Skill Damage\n+%s%% to Poison Skill Damage", range.c_str(), range.c_str(), range.c_str(), range.c_str());
		}
		else if (us.prop == 346) {
			sprintf(out, "%s%% Increased Strength\n%s%% Increased Dexterity\n%s%% Increased Vitality\n%s%% Increased Energy", range.c_str(), range.c_str(), range.c_str(), range.c_str());
		}
		else if (us.prop == 347) {
			sprintf(out, "-%s%% to Lightning Skill Damage\n-%s%% to Fire Skill Damage\n-%s%% to Cold Skill Damage\n-%s%% to Poison Skill Damage", range.c_str(), range.c_str(), range.c_str(), range.c_str());
		}
		else if (us.prop == 406) {
			sprintf(out, "%s to Life\n%s to Mana", range.c_str(), range.c_str());
		}
		else if (us.prop == 407) {
			sprintf(out, "%s%% Chance of Crushing Blow\n%s%%Deadly Strike\n%s%%Chance of Open Wounds", range.c_str(), range.c_str(), range.c_str());
		}
		else if (us.prop == 429) {
			sprintf(out, "Increase Maximum Life %s%%\nIncrease Maximum Mana %s%%", range.c_str(), range.c_str());
		}
		else {
			sprintf(out, "--------------%d, %s, %d, %d, %d--------------", us.prop, g_props[us.prop].c_str(), us.par, us.min, us.max);
		}
		return std::string(out);
	};

	auto isnewline = [](UniqueStats us) -> bool {
		bool ret = true;
		switch (us.prop) {
			case 20:
			case 22:
			case 24:
			case 25:{
				ret = false;
				break;
			}
		}
		return ret;
	};

#ifdef WRITEITEMS
	std::ofstream setnamecodes;
	setnamecodes.open("setnamecodes.txt");
	std::ofstream uniquenamecodes;
	uniquenamecodes.open("uniquenamecodes.txt");
	std::ofstream statout;
	statout.open("candy.txt");
	if (statout.is_open()) {
#endif
#ifdef WRITEITEMCODES
		std::map<int, std::string> types = {
			{2, "Shields"},
			{3, "Body Armor"},
			{15, "Boots"},
			{16, "Gloves"},
			{19, "Belts"},
			{24, "Scepters"},
			{25, "Wands"},
			{26, "Staves"},
			{27, "Bows"},
			{28, "Axes"},
			{29, "Maces"},
			{30, "Swords"},
			{31, "Maces"},
			{32, "Daggers"},
			{33, "Spears"},
			{34, "Polearms"},
			{35, "Crossbows"},
			{36, "Maces"},
			{37, "Helms"},
			{38, "Throwing"},
			{39, "Quest"},
			{40, "Not Used"},
			{42, "Throwing"},
			{43, "Throwing"},
			{44, "Throwing"},
			{67, "Assassin Katars"},
			{68, "Sorceress Orbs"},
			{69, "Necromancer Shrunken Heads"},
			{70, "Paladin Shields"},
			{71, "Barbarian Helms"},
			{72, "Druid Pelts"},
			{74, "Runes"},
			{75, "Helms"},
			{85, "Amazon Weapons"},
			{86, "Amazon Weapons"},
			{87, "Amazon Weapons"},
			{88, "Assassin Katars"},
			{96, "Gems"}, 
			{97, "Gems"},
			{98, "Gems"},
			{99, "Gems"},
			{100, "Gems"},
			{101, "Gems"},
			{102, "Gems"},			
		};
		std::ofstream codeout;
		codeout.open("realitemcodes.txt");
		for (size_t i = 0; i < Txt<ItemTxt>::Count(); ++i) {
			const ItemTxt *txt;
			TxtGet(&txt, (DWORD)i);
			if (!txt)
				continue;

			const char* text = getStringByNum(txt->nameHash);
			std::string tstr(text);
			auto it = tstr.find(-61);
			while (it != std::string::npos) {
				tstr.replace(tstr.begin() + it, tstr.begin() + it + 4, "");
				it = tstr.find(-61);
			}

			std::string tstr2 = std::string(txt->name).substr(0, 4);
			if (tstr2.size() > 3 && tstr2[3] == 32) {
				tstr2 = tstr2.substr(0, 3);
			}

			codeout << tstr << "," << tstr2 << '\n';

		//	if (types[txt->type].size() == 0)
		//		printf("%d: \"%s\", \"%s\" invfile:%s type:%d:%s\n", i, text, tstr2.c_str(), txt->inv, txt->type, types[txt->type].c_str());
			printf("%d{Place: %d:\"%s\", \"%s\", \"%s\", 0},\n", i, txt->type, types[txt->type].size() == 0 ? "Other" : types[txt->type].c_str(), tstr.c_str(), tstr2.c_str());
		}
		codeout.close();
		_getch();
#endif
		
		/*size_t sisis = Txt<PropertiesTxt>::Count();
		for (size_t i = 0; i < Txt<PropertiesTxt>::Count(); ++i) {
			const PropertiesTxt *txt;
			TxtGet(&txt, (DWORD)i);

			if (!txt)
				continue;

			printf("");
		}*/

		for (size_t i = 0; i < Txt<SetTxt>::Count(); ++i) {
			const SetTxt *txt;
			TxtGet(&txt, (DWORD)i);
			if (!txt)
				continue;

			char tte[60];
			sprintf(tte, "%s\n%.4s\n", txt->name, txt->base);
#ifdef PRINTITEMS
			printf(tte);
#endif
#ifdef WRITEITEMS
			statout << tte;
			char tte2[60];
			sprintf(tte2, "%s|%.4s\n", txt->name, txt->base);
			setnamecodes << tte2;
#endif	

			/*std::string s0(tte);
			if (s0.find("Queen") != std::string::npos)
				std::cout << "break\n";*/

			for (UINT j = 0; j < 19; j++) {
				std::string s = getproptext(txt->props[j]);
				if (s.size()) {
#ifdef PRINTITEMS
					printf("%s", s.c_str());

					if (j > 8)
						printf(" (%d item set bonus)", ((j - 9) / 2) + 2);
					if (isnewline(txt->props[j]))
						printf("\n");
#endif
#ifdef WRITEITEMS
					statout << s;
					if (j > 8) {
						statout << " (" << ((j - 9) / 2) + 2 << " item set bonus)";
					}
					if (isnewline(txt->props[j]))
						statout << '\n';
#endif
				}

#ifdef PRINTITEMS
				UniqueStats us = txt->props[j];
				if (us.prop == 279) {
					printf("");
			//		getch();
				}

				const ItemStatCostTxt *stat;
				TxtGet(&stat, us.prop);
				if (stat && g_props[us.prop].size() == 0){
					printf("");
//					getch();
				}
#endif
			}
#ifdef PRINTITEMS
			printf("\n");
#endif
#ifdef WRITEITEMS
			statout << '\n';
#endif
		}
	//	_getch();
		for (size_t i = 0; i < Txt<UniqueTxt>::Count(); ++i) {
			const UniqueTxt *txt;
			TxtGet(&txt, (DWORD)i);
			if (!txt)
				continue;
		
			char tte[60];
			sprintf(tte, "%s\n%.4s,%s\n", txt->name, txt->base, txt->invfile);
#ifdef PRINTITEMS
			printf(tte);
#endif
			if (std::string(txt->name).find("Hellrack") != std::string::npos)
				printf("");
#ifdef WRITEITEMS
			statout << tte;
			char tte2[60];
			sprintf(tte2, "%s|%.4s\n", txt->name, txt->base);
			uniquenamecodes << tte2;
#endif			

			for (UINT j = 0; j < 12; j++) {
				std::string s = getproptext(txt->props[j]);
				if (s.size()) {
#ifdef PRINTITEMS
					printf("%s", s.c_str());
					if (isnewline(txt->props[j]))
						printf("\n");
#endif
#ifdef WRITEITEMS					
					statout << s;
					if (isnewline(txt->props[j]))
						statout << '\n';
#endif
				}
#ifdef PRINTITEMS
				UniqueStats us = txt->props[j];
				if (us.prop == 300) {
					printf("");
//					getch();
				}
				const ItemStatCostTxt *stat;
				TxtGet(&stat, us.prop);
				if (stat && g_props[us.prop].size() == 0) {
					printf("");
//					getch();
				}
#endif
			}
#ifdef PRINTITEMS
			printf("\n");
#endif
#ifdef WRITEITEMS
			statout << '\n';
#endif
		}
	//	_getch();
#ifdef WRITEITEMS
	}
	statout.close();
	setnamecodes.close();
	uniquenamecodes.close();
#endif	

#endif
#endif

	size_t j, count;
	DWORD sizeD;
	HMODULE *hModules;
	TCHAR name[MAX_PATH];
	PROCESSENTRY32 pe32;

	std::vector<DWORD> pids;

	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Can't create process snapshot\n", L"ERROR", MB_OK);
		return 5;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snap, &pe32)) {
		CloseHandle(snap);
		MessageBox(NULL, L"Can't get first process\n", L"ERROR", MB_OK);
		return 6;
	}
	do {
		if (!strcmp(wstr_to_str(pe32.szExeFile).c_str(), "Game.exe")) {
			pids.push_back(pe32.th32ProcessID);
		}
	} while (Process32Next(snap, &pe32));
	CloseHandle(snap);

	if (pids.empty()) {
		MessageBox(NULL, L"Can't find running Diablo II\n", L"ERROR", MB_OK);
		return 7;
	}
	int dump_game_ret = 7;
	for (size_t i = 0; i < pids.size(); ++i) {
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS/*PROCESS_QUERY_INFORMATION | PROCESS_VM_READ*/, 0, pids[i]);
		if (!hProcess) {
			MessageBox(NULL, str_to_wstr("Can't open process PID: " + int_to_str(pids[i])).c_str(), L"ERROR", MB_OK);
			continue;
		}

		DWORD D2Client_base = 0;
		bool got_base = false;

		if (EnumProcessModulesEx(hProcess, 0, 0, &sizeD, LIST_MODULES_ALL)) {
			hModules = (HMODULE *)malloc(sizeD);
			if (hModules) {
				if (EnumProcessModulesEx(hProcess, hModules, sizeD, &sizeD, LIST_MODULES_ALL)) {
					count = sizeD / sizeof(HMODULE);
					for (j = 0; j < count; j++) {
						if (!GetModuleFileNameEx(hProcess, hModules[j], name, sizeof(name))) {
							MessageBox(NULL, str_to_wstr("Can't ged module name PID: " + int_to_str(pids[i]) + "MODULE : " + int_to_str((int)hModules[j], true)).c_str(), L"ERROR", MB_OK);
							continue;
						}
						const char *dll = "D2Client.dll";
						int len1 = strlen(dll);
						int len2 = strlen(wstr_to_str(name).c_str());
						if (len1 > len2)
							continue;
						if (!_stricmp(wstr_to_str(name + len2 - len1).c_str(), dll)) {
							MODULEINFO modInfo;
							if (GetModuleInformation(hProcess, hModules[j], &modInfo, sizeof(modInfo))) {
								got_base = true;
								D2Client_base = (DWORD)modInfo.lpBaseOfDll;
							}
						}
					}
				}
				else {
					MessageBox(NULL, str_to_wstr("Can't enum modules PID: " + int_to_str(pids[i])).c_str(), L"ERROR", MB_OK);
				}
				free(hModules);
			}
			else {
				MessageBox(NULL, str_to_wstr("Can't callocate memory for Modules PID: " + int_to_str(pids[i])).c_str(), L"ERROR", MB_OK);
			}
		}
		if (got_base) {
			if (!DumpGame(hProcess, D2Client_base)) {
				dump_game_ret++;
			}
		}

		CloseHandle(hProcess);
	}
	return dump_game_ret == 7 ? 0 : dump_game_ret;
}
int desc_priority_cmp(const void *aa, const void *bb) {
	DWORD a = *(DWORD*)aa;
	DWORD b = *(DWORD*)bb;
	const ItemStatCostTxt *txta, *txtb;
	TxtGet(&txta, a);
	TxtGet(&txtb, b);
	int _a = txta->descpriority;
	int _b = txtb->descpriority;
	return _a - _b;
}

