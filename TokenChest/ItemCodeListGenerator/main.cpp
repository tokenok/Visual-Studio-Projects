#include <Windows.h>
#include <Windowsx.h>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <fstream>
#include <iostream>
#include <sstream>
#include <conio.h>
#include <Commctrl.h>
#include <Richedit.h>
#pragma comment(lib, "Comctl32.lib")

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "C:\CPPlibs\common\f\common.h"
#include "resource.h"

#pragma warning(disable: 4503)

using namespace std;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hwnd;
string ConfigIni;

const COLORREF g_cust_color = RGB(25, 25, 25);

enum quality {
	QUAL_WHATEVER,
	QUAL_LOW,
	QUAL_NORMAL,
	QUAL_SUPERIOR,
	QUAL_MAGIC,
	QUAL_SET,
	QUAL_RARE,
	QUAL_UNIQUE,
	QUAL_CRAFTED,
	QUAL_TEMPERED
};
enum colors {
	COL_WHITE,
	COL_RED,
	COL_LIGHTGREEN,
	COL_BLUE,
	COL_DARKGOLD,
	COL_GRAY,
	COL_BLACK,
	COL_GOLD,
	COL_ORANGE,
	COL_YELLOW,
	COL_DARKGREEN,
	COL_PURPLE,
	COL_GREEN,
	COL_WHITE2,
	COL_BLACK2,
	COL_DARKWHITE,
	COL_HIDE
};

string TransColor(int col) {
	if (col == 0) return "white";
	else if (col == 1) return "red";
	else if (col == 2) return "lightgreen";
	else if (col == 3) return "blue";
	else if (col == 4) return "darkgold";
	else if (col == 5) return "grey";
	else if (col == 6) return "black";
	else if (col == 7) return "gold";
	else if (col == 8) return "orange";
	else if (col == 9) return "yellow";
	else if (col == 10) return "darkgreen";
	else if (col == 11) return "purple";
	else if (col == 12) return "green";
	else if (col == 13) return "white2";
	else if (col == 14) return "black2";
	else if (col == 15) return "darkwhite";
	else if (col == 16) return "hide";
	return "";
}
int TransColor(string str) {
	if (str == "white") return 0;
	else if (str == "red") return 1;
	else if (str == "lightgreen") return 2;
	else if (str == "blue") return 3;
	else if (str == "darkgold") return 4;
	else if (str == "grey") return 5;
	else if (str == "black") return 6;
	else if (str == "gold") return 7;
	else if (str == "orange") return 8;
	else if (str == "yellow") return 9;
	else if (str == "darkgreen") return 10;
	else if (str == "purple") return 11;
	else if (str == "green") return 12;
	else if (str == "white2") return 13;
	else if (str == "black2") return 14;
	else if (str == "darkwhite") return 15;
	else if (str == "hide") return 16;
	return 0;
}
string TransQuality(int qual) {
	if (qual == 0) return "whatever";
	else if (qual == 1) return "low";
	else if (qual == 2) return "normal";
	else if (qual == 3) return "superior";
	else if (qual == 4) return "magic";
	else if (qual == 5) return "set";
	else if (qual == 6) return "rare";
	else if (qual == 7) return "unique";
	else if (qual == 8) return "crafted";
	else if (qual == 9) return "tempered";
	return "";
}
int TransQuality(string str) {
	if (str == "whatever") return 0;
	else if (str == "low") return 1;
	else if (str == "normal") return 2;
	else if (str == "superior") return 3;
	else if (str == "magic") return 4;
	else if (str == "set") return 5;
	else if (str == "rare") return 6;
	else if (str == "unique") return 7;
	else if (str == "crafted") return 8;
	else if (str == "tempered") return 9;
	return 0;
}
DWORD TransCode(const char* ptCode) // taken from afj666
{
	DWORD ItemCode = 0;

	if (strlen(ptCode) == 3) {
		char NewStr[5];
		sprintf_s(NewStr, 5, "%s ", ptCode);
		ItemCode = TransCode(NewStr);
	}
	else {
		ItemCode = (DWORD)(ptCode[3] << 24) + (DWORD)(ptCode[2] << 16) +
			(DWORD)(ptCode[1] << 8) + (DWORD)(ptCode[0]);
	}

	return ItemCode;
}
string TransCode(DWORD dwCode) {
	BYTE* aCode = (BYTE*)&dwCode;
	char ItemCode[5] = {0};

	ItemCode[0] = aCode[0];
	ItemCode[1] = aCode[1];
	ItemCode[2] = aCode[2];
	ItemCode[3] = aCode[3];
	ItemCode[4] = 0;

	return string(ItemCode);
}
int get_selected_color_mode() {
	return IsDlgButtonChecked(g_hwnd, IDC_RADIO1) ? 0 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO2) ? 1 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO3) ? 2 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO4) ? 3 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO5) ? 4 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO6) ? 5 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO7) ? 6 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO8) ? 7 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO9) ? 8 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO10) ? 9 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO11) ? 10 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO12) ? 11 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO13) ? 12 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO14) ? 13 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO15) ? 14 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO16) ? 15 :
		IsDlgButtonChecked(g_hwnd, IDC_RADIO17) ? 16 : 16;
}

struct ItemCode {
	std::string basetype;//weapon, armor...
	std::string subtype;//axe, shield...
	std::string type;//battle axe, buckler...
	std::string code;//btx, buc...
	int tier;//normal, exceptional, or elite, otherwise zero

	int col = -1;
	int def_col = -1;
};
struct ItemConfig {
	int Code;
	int	Quality;
	int Color;
	int Type;
};

vector<ItemCode> g_itemcodes;
vector<ItemCode> g_acodenames;
map<string, map<string, int>> g_ucodenames;
map<string, map<string, int>> g_scodenames;
vector<ItemConfig> g_ItemArray;

COLORREF getitemqualitycolor(int col) {
	if (col == COL_WHITE) return RGB(0xC4, 0xC4, 0xC4);
	else if (col == COL_RED) return RGB(0xff, 0, 0); 
	else if (col == COL_LIGHTGREEN) return RGB(0x18, 0xFC, 0x00);
	else if (col == COL_BLUE) return RGB(0x78, 0x7c, 0xe7);
	else if (col == COL_DARKGOLD) return RGB(0xAF, 0xA6, 0x69);
	else if (col == COL_GRAY) return RGB(0x50, 0x50, 0x50);
	else if (col == COL_BLACK) return RGB(0, 0, 0);
	else if (col == COL_GOLD) return RGB(0xAC, 0x9C, 0x64);
	else if (col == COL_ORANGE) return RGB(0xD0,0x84,0x20); 
	else if (col == COL_YELLOW) return RGB(0xf5, 0xf7, 0x92);
	else if (col == COL_DARKGREEN) return RGB(0x18, 0x64, 0x08);
	else if (col == COL_PURPLE) return RGB(0xA4, 0x20, 0xFC);
	else if (col == COL_GREEN) return RGB(0x28, 0x7C, 0x14);
	else if (col == COL_WHITE2) return RGB(0xC4, 0xC4, 0xC4);
	else if (col == COL_BLACK2) return RGB(0xC4, 0xC4, 0xC4);
	else if (col == COL_DARKWHITE) return RGB(0xC4, 0xC4, 0xC4);
	else if (col == COL_HIDE) return 0;
	
	return 0;
}
LRESULT TreeCustomDraw(HWND tree, LPNMTVCUSTOMDRAW pNMTVCD) {
	if (pNMTVCD == NULL) 
		return -1;
	switch (pNMTVCD->nmcd.dwDrawStage) {
		case CDDS_PREPAINT:{
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
		}
		case CDDS_ITEMPREPAINT: {
			TV_ITEM tvi = {0};
			tvi.mask = TVIF_TEXT | TVIF_PARAM;
			tvi.hItem = (HTREEITEM)pNMTVCD->nmcd.dwItemSpec;
			TreeView_GetItem(tree, &tvi);
			if (tvi.lParam > 0 && TreeView_GetChild(tree, tvi.hItem) == NULL) {
				ItemCode* pItemData = (ItemCode*)tvi.lParam;
				if (pItemData) {
					//set text color
					SetTextColor(pNMTVCD->nmcd.hdc, getitemqualitycolor(TreeView_GetCheckState(tree, tvi.hItem) ? pItemData->col : pItemData->def_col));

					//set background color
					if (pNMTVCD->nmcd.uItemState & CDIS_SELECTED)
						SetBkColor(pNMTVCD->nmcd.hdc, RGB(128, 0, 0));
					else {
						if (TreeView_GetSelection(tree) == tvi.hItem)
							SetBkColor(pNMTVCD->nmcd.hdc, RGB(80, 80, 200));//selection no focus
						else if (pItemData->col != COL_HIDE)
							SetBkColor(pNMTVCD->nmcd.hdc, g_cust_color);
						else; //default
					}
				}
			}
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);
		}
		case CDDS_ITEMPOSTPAINT: {
			return CDRF_DODEFAULT;
		}
	}
	return CDRF_DODEFAULT;
}
LPARAM TreeView_GetItemParam(HWND hwnd, HTREEITEM htItem) {
	TVITEMW tvi = {0};
	tvi.hItem = htItem;
	tvi.mask = TVIF_PARAM;
	TreeView_GetItem(hwnd, &tvi);
	return tvi.lParam;
}
string TreeView_GetItemText(HWND hwnd, HTREEITEM htItem) {
	static const size_t len = 240;
	WCHAR buffer[len + 1];

	TVITEMW tvi = {0};
	tvi.hItem = htItem;
	tvi.mask = TVIF_TEXT;
	tvi.cchTextMax = len;
	tvi.pszText = &buffer[0];
	TreeView_GetItem(hwnd, &tvi);
	return wastr_to_str(tvi.pszText, len);
}
HTREEITEM TreeView_FindSibling(HWND hwnd, HTREEITEM start, string match) {
	HTREEITEM current = start;
	do {
		string text = TreeView_GetItemText(hwnd, current);
		text = text.substr(0, text.find(" ("));
		if (text == match)
			return current;
	} while ((current = TreeView_GetNextSibling(hwnd, current)) != NULL);
	return NULL;
}
HTREEITEM TreeView_SetCheckStateForAllChildren(HWND tree, HTREEITEM hItem, BOOL checkstate) {
	HTREEITEM current = hItem;
	HTREEITEM sibling = NULL;
	while (current != NULL && sibling == NULL) {
		TreeView_SetCheckState(tree, current, checkstate);
		ItemCode* ic = (ItemCode*)TreeView_GetItemParam(tree, current);
		if (ic) {
			ic->col = checkstate != 0 ? get_selected_color_mode() : -1;
		}
		sibling = TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, current), checkstate);
		current = TreeView_GetNextSibling(tree, current);
	}
	return sibling;
}
void TreeView_SetCheckStateForAllParents(HWND tree, HTREEITEM child) {
	while (child != NULL) {
		int check = 0;
		int total = 1;
		HTREEITEM current = TreeView_GetChild(tree, TreeView_GetParent(tree, child));
		check = TreeView_GetCheckState(tree, current) ? check + 1 : check;
		while (current != NULL) {
			current = TreeView_GetNextSibling(tree, current);
			check = TreeView_GetCheckState(tree, current) ? check + 1 : check;
			total++;
		}
		TreeView_SetCheckState(tree, TreeView_GetParent(tree, child), total == check);

		child = TreeView_GetParent(tree, child);
	}
}
HTREEITEM getfilteritems(HWND tree, HTREEITEM hItem, vector<HTREEITEM>* list) {
	HTREEITEM current = hItem;
	HTREEITEM child = NULL;
	while (current != NULL && child == NULL) {
		child = getfilteritems(tree, TreeView_GetChild(tree, current), list);
		if ((TreeView_GetChild(tree, current) == NULL) && (TreeView_GetCheckState(tree, current) == BST_CHECKED))
			list->push_back(current);
		current = TreeView_GetNextSibling(tree, current);
	}
	return child;
}

void read_itemcodes() {
	g_itemcodes.clear();
	ifstream file;
	file.open("item codes.txt");
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

	ifstream annihiluscodes;
	annihiluscodes.open("annihiluscodes.txt");
	if (annihiluscodes.is_open()) {
		while (annihiluscodes.good()) {
			string line;
			getline(annihiluscodes, line);

			while (line.find("!") == 0 && annihiluscodes.good()) {
				string basetype = line.substr(1, line.size() - 1);
				getline(annihiluscodes, line);
				while (line.find("@") == 0 && annihiluscodes.good()) {
					string subtype = line.substr(1, line.size() - 1);
					getline(annihiluscodes, line);
					while (line.size() > 0
						&& line[0] != '!'
						&& line[0] != '@'
						&& annihiluscodes.good()) {

						ItemCode code;
						code.basetype = basetype;
						code.subtype = subtype;
						vector<string> cod = split_str(line, ",");
						code.type = cod[0];
						code.code = cod[1];
						code.tier = str_to_int(cod[2]);
						g_acodenames.push_back(code);
						getline(annihiluscodes, line);
					}
				}
			}
		}
	}
	annihiluscodes.close();

	ifstream uniquenamecodes;
	uniquenamecodes.open("uniquenamecodes.txt");
	if (uniquenamecodes.is_open()) {
		while (uniquenamecodes.good()) {
			string line;
			getline(uniquenamecodes, line);

			vector<string> item = split_str(line, "|");

			if (item.size() != 2)
				continue;

			if (item[1][3] == ' ')
				item[1].pop_back();

			g_ucodenames[item[1]].insert(make_pair(item[0], 1));
		}
	}
	uniquenamecodes.close();

	ifstream setnamecodes;
	setnamecodes.open("setnamecodes.txt");
	if (setnamecodes.is_open()) {
		while (setnamecodes.good()) {
			string line;
			getline(setnamecodes, line);

			vector<string> item = split_str(line, "|");

			if (item.size() != 2)
				continue;

			if (item[1][3] == ' ')
				item[1].pop_back();

			g_scodenames[item[1]].insert(make_pair(item[0], 1));
		}
	}
	setnamecodes.close();
}
void LoadItemConfig() {
	char szConfig[200];

	ifstream infile;
	infile.open("itemfilter.txt");

	int linenum = 1;
	while (infile.good()) {
		string Config;
		getline(infile, Config);

		if (Config.size() == 0)
			continue;

		string::size_type i = Config.find(',');
		if (i == string::npos) { printf("Error in configuration at line: %d", linenum); continue; }
		string ItemCode(Config.substr(0, i));

		string::size_type i2 = Config.find(',', i + 1);
		if (i2 == string::npos) { printf("Error in configuration at line: %d", linenum); continue; }
		string ItemQuality(Config.substr(i + 1, i2 - i - 1));

		string ItemColor(Config.substr(i2 + 1));

		boost::trim(ItemCode); boost::trim(ItemQuality); boost::trim(ItemColor);
		boost::to_lower(ItemCode); boost::to_lower(ItemQuality); boost::to_lower(ItemColor);

		ItemConfig hConfig;
		hConfig.Code = TransCode(ItemCode.c_str());
		hConfig.Quality = TransQuality(ItemQuality.c_str());
		hConfig.Color = TransColor(ItemColor.c_str());
		//Log("%x, %d, %d",hConfig.Code,hConfig.Quality,hConfig.Color);
		g_ItemArray.push_back(hConfig);

		linenum++;
	}

	infile.close();
}
void SaveItemConfig(int start = 1) {
	ofstream outfile;
	outfile.open("itemfilter.txt");

	HWND edit = GetDlgItem(g_hwnd, IDC_RICHEDIT21);
	int len = GetWindowTextLength(edit);
	if (len == 0) return;
	wchar_t* wtext = new wchar_t[GetWindowTextLength(edit)];
	GetWindowText(edit, wtext, len);
	wstring wt = wstring(wtext);
	string text = wstr_to_str(wt);
	vector<string> lines = split_str(text, "\r\n");
	for (UINT i = 0; i < lines.size(); i++) {//write items back into config
		string code = lines[i].substr(lines[i].find('=') + 1, lines[i].find(',') - lines[i].find('=') - 1);
		string qual = lines[i].substr(lines[i].find(',') + 1, lines[i].rfind(',') - lines[i].find(',') - 1);
		string col = lines[i].substr(lines[i].rfind(',') + 1, string::npos);

		boost::trim(code); boost::trim(qual); boost::trim(col);
		boost::to_lower(code); boost::to_lower(qual); boost::to_lower(col);

		string configline = code + ", " + qual + ", " + col;		

		outfile << configline << '\n';
	}

	outfile.close();
}

void PrintBaseFilterCodes(HWND tree) {
	SetWindowText(GetDlgItem(GetParent(tree), IDC_RICHEDIT21), L"");

	string output = "";

	if (GetDlgCtrlID(tree) == IDC_ITEMCODEFILTERTREE) {
		vector<HTREEITEM> filter_itemtypes;
		getfilteritems(tree, TreeView_GetRoot(tree), &filter_itemtypes);

		for (UINT i = 0; i < filter_itemtypes.size(); i++) {
			ItemCode* ic = (ItemCode*)TreeView_GetItemParam(tree, filter_itemtypes[i]);

			HTREEITEM parent = TreeView_GetParent(tree, filter_itemtypes[i]);
			HTREEITEM cur;
			do {
				cur = parent;
				parent = TreeView_GetParent(tree, parent);
			} while (TreeView_GetParent(tree, parent));

			string quality = TreeView_GetItemText(tree, cur);
			boost::to_lower(quality);
			if (quality == "annihilus")
				quality = "whatever";
			
			if (filter_itemtypes[i])
				output += ic->code + ", " + quality + ", " + TransColor(ic->col) + "\r\n";
		}
	}

	SetWindowText(GetDlgItem(GetParent(tree), IDC_RICHEDIT21), str_to_wstr(output).c_str());
}
HTREEITEM findcodeintree(HWND tree, HTREEITEM start, ItemCode* icode) {
	start = TreeView_FindSibling(tree, TreeView_GetChild(tree, start), icode->basetype);
	start = TreeView_FindSibling(tree, TreeView_GetChild(tree, start), icode->subtype);
	if (icode->tier)
		start = TreeView_FindSibling(tree, TreeView_GetChild(tree, start), icode->tier == 1 ? "Normal" : icode->tier == 2 ? "Exceptional" : icode->tier == 3 ? "Elite" : "");
	start = TreeView_FindSibling(tree, TreeView_GetChild(tree, start), icode->type);
	//start = TreeView_GetChild(tree, start);
	if (TreeView_GetItemParam(tree, start))
		return start;
	return NULL;
}
void InitTree(HWND tree, vector<string>& configlines) {
	static bool first = true;
	if (!first) {
		first = false;
		TreeView_SetCheckStateForAllChildren(tree, TreeView_GetRoot(tree), FALSE);
	}	

	for (auto line : configlines) {
		string code = line.substr(line.find('=') + 1, line.find(',') - line.find('=') - 1);
		string qual = line.substr(line.find(',') + 1, line.rfind(',') - line.find(',') - 1);
		string col = line.substr(line.rfind(',') + 1, string::npos);
		boost::trim(code); boost::trim(qual); boost::trim(col);
		boost::to_lower(code); boost::to_lower(qual); boost::to_lower(col);

		HTREEITEM current = TreeView_GetRoot(tree);
		if (qual != "whatever") {				
			current = TreeView_GetChild(tree, current);
			do {
				string match = TreeView_GetItemText(tree, current);
				boost::to_lower(match);
				if (qual == match)
					break;
			} while ((current = TreeView_GetNextSibling(tree, current)) != NULL);
			if (!current) 
				continue;

			ItemConfig ic;
			ic.Code = TransCode(code.c_str());
			ic.Quality = TransQuality(qual);
			ic.Color = TransColor(col);
			int pos = -1;
			for (UINT i = 0; i < g_ItemArray.size(); i++) {
				if (g_ItemArray[i].Code == ic.Code && g_ItemArray[i].Quality == ic.Quality) {
					if (true || g_ItemArray[i].Color == COL_HIDE) {
						pos = (int)i;
						break;
					}
				}
			}
			if (pos == -1) g_ItemArray.push_back(ic);
			else g_ItemArray.erase(g_ItemArray.begin() + pos);

			ItemCode* item = 0;
			for (UINT i = 0; i < g_itemcodes.size(); i++) {
				if (g_itemcodes[i].code == code) {
					item = &g_itemcodes[i];
					break;
				}
			}
			if (!item) {
				for (UINT i = 0; i < g_acodenames.size(); i++) {
					if (g_acodenames[i].code == code) {
						item = &g_acodenames[i];
						break;
					}
				}
				if (!item)
					continue;
			}

			HTREEITEM found = findcodeintree(tree, current, item);
			if (found) {
				((ItemCode*)TreeView_GetItemParam(tree, found))->col = TransColor(col);
				TreeView_SetCheckState(tree, found, TRUE);
				TreeView_SetCheckStateForAllParents(tree, found);
			}
			else
				printf("item not found in tree!\n");
		}
		else {
			function<HTREEITEM(HWND, HTREEITEM, string, vector<HTREEITEM>*)> rec = [&](HWND tree, HTREEITEM hItem, string code, vector<HTREEITEM>* list) -> HTREEITEM {
				HTREEITEM current = hItem;
				HTREEITEM child = NULL;
				while (current != NULL && child == NULL) {
					child = rec(tree, TreeView_GetChild(tree, current), code, list);
					ItemCode* ic = ((ItemCode*)TreeView_GetItemParam(tree, current));
					if ((TreeView_GetChild(tree, current) == NULL) && (ItemCode*)TreeView_GetItemParam(tree, current) && (((ItemCode*)TreeView_GetItemParam(tree, current))->code == code))
						list->push_back(current);
					current = TreeView_GetNextSibling(tree, current);
				}
				return child;
			};

			vector<HTREEITEM> nodes;
			rec(tree, TreeView_GetRoot(tree), code, &nodes);

			for (UINT i = 0; i < nodes.size(); i++) {
				ItemCode* icd = (ItemCode*)TreeView_GetItemParam(tree, nodes[i]);

				icd->col = TransColor(col);

				//if (icd->col == COL_HIDE) {						
				TreeView_SetCheckState(tree, nodes[i], TRUE);
				TreeView_SetCheckStateForAllParents(tree, nodes[i]);
				//}
			}
		}		
	}
	PostMessage(GetParent(tree), WM_COMMAND, IDC_BUTTON1, GetDlgCtrlID(tree));//PrintBaseFilterCodes(tree);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
#ifdef _DEBUG
	SHOW_CONSOLE(true);
#endif
	ConfigIni = getexedir() + "\\itemfilter.txt";

	InitCommonControls();

	LoadLibrary(L"riched20.dll");
	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)DialogProc, 0);
}
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			g_hwnd = hwnd;

			LoadItemConfig();
			read_itemcodes();

			map<string, map<string, map<int, map<string, ItemCode*>>>> item_tree;//generates Warning: C4503
			for (UINT i = 0; i < g_itemcodes.size(); i++) {
				item_tree[g_itemcodes[i].basetype][g_itemcodes[i].subtype][g_itemcodes[i].tier][g_itemcodes[i].type] = &g_itemcodes[i];
			}

			map<string, map<string, map<int, map<string, ItemCode*>>>> annihilus_tree;
			for (UINT i = 0; i < g_acodenames.size(); i++) {
				annihilus_tree[g_acodenames[i].basetype][g_acodenames[i].subtype][g_acodenames[i].tier][g_acodenames[i].type] = &g_acodenames[i];
			}

			map<string, map<string, map<int, map<string, ItemCode*>>>> set_tree;
			for (auto a : g_scodenames) {
				for (UINT i = 0; i < g_itemcodes.size(); i++) {
					if (g_itemcodes[i].code == a.first) {
						set_tree[g_itemcodes[i].basetype][g_itemcodes[i].subtype][g_itemcodes[i].tier][g_itemcodes[i].type] = &g_itemcodes[i];
						break;
					}
				}
			}

			map<string, map<string, map<int, map<string, ItemCode*>>>> unique_tree;
			for (auto a : g_ucodenames) {
				for (UINT i = 0; i < g_itemcodes.size(); i++) {
					if (g_itemcodes[i].code == a.first) {
						unique_tree[g_itemcodes[i].basetype][g_itemcodes[i].subtype][g_itemcodes[i].tier][g_itemcodes[i].type] = &g_itemcodes[i];
						break;
					}
				}
			}

			vector<string> qualities = {"Low", "Normal", "Superior", "Magic", "Rare"};

			static HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR16, 3, 10);
			static HBITMAP hBitMap = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1));
			ImageList_Add(hImageList, hBitMap, NULL);
			DeleteObject(hBitMap);
			SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_SETIMAGELIST, 0, (LPARAM)hImageList);

			TV_INSERTSTRUCT tvinsert = {0};
			tvinsert.hParent = NULL;
			tvinsert.hInsertAfter = TVI_ROOT;
			tvinsert.item.pszText = L"All Items";
			tvinsert.item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE;
			tvinsert.item.stateMask = TVIS_STATEIMAGEMASK;
			tvinsert.item.iImage = 0;
			tvinsert.item.iSelectedImage = 1;
			tvinsert.hInsertAfter = TVI_LAST;
			HTREEITEM TRoot = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			HTREEITEM TQuality;
			HTREEITEM TBasetype;
			HTREEITEM TSubtype;
			HTREEITEM TTier;
			for (auto quality : qualities) {
				tvinsert.hParent = TRoot;
				tvinsert.item.pszText = str_to_LPWSTR(quality);
				TQuality = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto basetype : item_tree) {
					if ((quality == "Low" || quality == "Superior") && basetype.first == "Misc")
						continue;
					tvinsert.hParent = TQuality;
					tvinsert.item.pszText = str_to_LPWSTR(basetype.first);
					TBasetype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & subtype : basetype.second) {
						if ((quality == "Magic" || quality == "Rare") && basetype.first == "Misc" && subtype.first != "Other")
							continue;
						tvinsert.hParent = TBasetype;
						tvinsert.item.pszText = str_to_LPWSTR(subtype.first);
						TSubtype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
						delete[] tvinsert.item.pszText;
						for (auto & tier : subtype.second) {
							tvinsert.hParent = TSubtype;
							tvinsert.item.pszText = tier.first == 1 ? L"Normal" : tier.first == 2 ? L"Exceptional" : tier.first == 3 ? L"Elite" : L"";
							TTier = tier.first > 0 ? (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert) : TSubtype;
							tvinsert.item.iImage = 2;
							tvinsert.item.iSelectedImage = 2;
							for (auto & type : tier.second) {
								if ((quality == "Normal" && basetype.first == "Misc" && subtype.first == "Other")
									&& (type.first == "Charm Large" || type.first == "Charm Medium" || type.first == "Charm Small"
									|| type.first == "Jewel" || type.first == "amulet" || type.first == "ring"))
									continue;
								if ((quality == "Magic" || quality == "Rare") && basetype.first == "Misc" && subtype.first == "Other"
									&& type.first != "Arrows" && type.first != "Bolts"
									&& type.first != "Hallowed Bolts" && type.first != "Hellfire Arrows"
									&& type.first != "Razorspine Bolts" && type.first != "Runic Arrows"
									&& type.first != "Charm Large" && type.first != "Charm Medium" && type.first != "Charm Small"
									&& type.first != "Jewel" && type.first != "amulet" && type.first != "ring")
									continue;

								if (quality == "Rare" && basetype.first == "Misc" && subtype.first == "Other"
									&& (type.first == "Charm Large" || type.first == "Charm Medium" || type.first == "Charm Small"))
									continue;

								ItemCode* ic = new ItemCode;
								ic->basetype = type.second->basetype;
								ic->code = type.second->code;
								ic->subtype = type.second->subtype;
								ic->tier = type.second->tier;
								ic->type = type.second->type;
								ic->def_col = 
									quality == "Low" ? COL_GRAY 
									: quality == "Normal" ? COL_WHITE 
									: quality == "Superior" ? COL_GRAY 
									: quality == "Magic" ? COL_BLUE 
									: quality == "Rare" ? COL_YELLOW
									: COL_RED;

								tvinsert.hParent = TTier;
								tvinsert.item.pszText = str_to_LPWSTR(type.first + " (" + type.second->code + ")");
								tvinsert.item.lParam = (LPARAM)ic;
								SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
								delete[] tvinsert.item.pszText;
							}
							tvinsert.item.lParam = 0;
							tvinsert.item.iImage = 0;
							tvinsert.item.iSelectedImage = 1;
						}
					}
				}
			}

			tvinsert.hParent = TRoot;
			tvinsert.item.pszText = L"Annihilus";
			TQuality = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			for (auto basetype : annihilus_tree) {
				tvinsert.hParent = TQuality;
				tvinsert.item.pszText = str_to_LPWSTR(basetype.first);
				TBasetype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto & subtype : basetype.second) {
					tvinsert.hParent = TBasetype;
					tvinsert.item.pszText = str_to_LPWSTR(subtype.first);
					TSubtype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & tier : subtype.second) {
						tvinsert.hParent = TSubtype;
						tvinsert.item.pszText = tier.first == 1 ? L"Normal" : tier.first == 2 ? L"Exceptional" : tier.first == 3 ? L"Elite" : L"";
						TTier = tier.first > 0 ? (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert) : TSubtype;
						for (auto & type : tier.second) {
							ItemCode* ic = new ItemCode;
							ic->basetype = type.second->basetype;
							ic->code = type.second->code;
							ic->subtype = type.second->subtype;
							ic->tier = type.second->tier;
							ic->type = type.second->type;
							ic->def_col = COL_DARKGOLD;

							tvinsert.hParent = TTier;
							tvinsert.item.pszText = str_to_LPWSTR(type.first + " (" + type.second->code + ")");
							tvinsert.item.lParam = (LPARAM)ic;
							SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
							delete[] tvinsert.item.pszText;
						}
						tvinsert.item.lParam = 0;
					}
				}
			}

			tvinsert.hParent = TRoot;
			tvinsert.item.pszText = L"Set";
			TQuality = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			for (auto basetype : set_tree) {
				tvinsert.hParent = TQuality;
				tvinsert.item.pszText = str_to_LPWSTR(basetype.first);
				TBasetype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto & subtype : basetype.second) {
					tvinsert.hParent = TBasetype;
					tvinsert.item.pszText = str_to_LPWSTR(subtype.first);
					TSubtype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & tier : subtype.second) {
						tvinsert.hParent = TSubtype;
						tvinsert.item.pszText = tier.first == 1 ? L"Normal" : tier.first == 2 ? L"Exceptional" : tier.first == 3 ? L"Elite" : L"";
						TTier = tier.first > 0 ? (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert) : TSubtype;
						for (auto & type : tier.second) {
							if (g_scodenames[type.second->code].size()) {
								tvinsert.hParent = TTier;
								string text = type.first + " (" + type.second->code + ") [";

								for (auto a : g_scodenames[type.second->code]) {
									if (a.second == 1) {
										text += a.first + ", ";
									}
								}
								text.pop_back();
								text.pop_back();
								text += "]";

								ItemCode* ic = new ItemCode;
								ic->basetype = type.second->basetype;
								ic->code = type.second->code;
								ic->subtype = type.second->subtype;
								ic->tier = type.second->tier;
								ic->type = type.second->type;
								ic->def_col = COL_LIGHTGREEN;

								tvinsert.item.pszText = str_to_LPWSTR(text);
								tvinsert.item.lParam = (LPARAM)ic;
								SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
								delete[] tvinsert.item.pszText;
							}
						}
						tvinsert.item.lParam = 0;
					}
				}
			}

			tvinsert.hParent = TRoot;
			tvinsert.item.pszText = L"Unique";
			TQuality = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
			for (auto basetype : unique_tree) {
				tvinsert.hParent = TQuality;
				tvinsert.item.pszText = str_to_LPWSTR(basetype.first);
				TBasetype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				delete[] tvinsert.item.pszText;
				for (auto & subtype : basetype.second) {
					tvinsert.hParent = TBasetype;
					tvinsert.item.pszText = str_to_LPWSTR(subtype.first);
					TSubtype = (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
					delete[] tvinsert.item.pszText;
					for (auto & tier : subtype.second) {
						tvinsert.hParent = TSubtype;
						tvinsert.item.pszText = tier.first == 1 ? L"Normal" : tier.first == 2 ? L"Exceptional" : tier.first == 3 ? L"Elite" : L"";
						TTier = tier.first > 0 ? (HTREEITEM)SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert) : TSubtype;
						for (auto & type : tier.second) {
							if (g_ucodenames[type.second->code].size()) {
								tvinsert.hParent = TTier;
								string text = type.first + " (" + type.second->code + ") [";

								for (auto a : g_ucodenames[type.second->code]) {
									if (a.second == 1) {
										text += a.first + ", ";
									}
								}
								text.pop_back();
								text.pop_back();
								text += "]";

								ItemCode* ic = new ItemCode;
								ic->basetype = type.second->basetype;
								ic->code = type.second->code;
								ic->subtype = type.second->subtype;
								ic->tier = type.second->tier;
								ic->type = type.second->type;
								ic->def_col = COL_DARKGOLD;

								tvinsert.item.pszText = str_to_LPWSTR(text);
								tvinsert.item.lParam = (LPARAM)ic;
								SendDlgItemMessage(hwnd, IDC_ITEMCODEFILTERTREE, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
								delete[] tvinsert.item.pszText;
							}
						}
						tvinsert.item.lParam = 0;
					}
				}
			}

			PostMessage(hwnd, WM_COMMAND, IDC_BUTTON1, NULL);

			break;
		}
		/*case WM_CTLCOLORBTN:{
			int col = -1;
			switch (GetDlgCtrlID((HWND)lParam)) {
				case IDC_RADIO1:
					col = 0; break;
				case IDC_RADIO2:
					col = 1; break;
				case IDC_RADIO3:
					col = 2; break;
				case IDC_RADIO4:
					col = 3; break;
				case IDC_RADIO5:
					col = 4; break;
				case IDC_RADIO6:
					col = 5; break;
				case IDC_RADIO7:
					col = 6; break;
				case IDC_RADIO8:
					col = 7; break;
				case IDC_RADIO9:
					col = 8; break;
				case IDC_RADIO10:
					col = 9; break;
				case IDC_RADIO11:
					col = 10; break;
				case IDC_RADIO12:
					col = 11; break;
				case IDC_RADIO13:
					col = 12; break;
				case IDC_RADIO14:
					col = 13; break;
				case IDC_RADIO15:
					col = 14; break;
				case IDC_RADIO16:
					col = 15; break;
				case IDC_RADIO17:
					col = 16; break;
			}

			if (col != -1) {
				HDC hDC = (HDC)wParam;

				SetTextColor(hDC, getitemqualitycolor(col));

				
			}

			break;
		}*/
		case WM_COMMAND:{
			switch (wParam) {
				case IDC_BUTTON1:{//hack
					if (lParam == NULL) {
						vector<string> lines;
						for (UINT i = 0; i < g_ItemArray.size(); i++) {
							string configline = TransCode(g_ItemArray[i].Code) + ", " + TransQuality(g_ItemArray[i].Quality) + ", " + TransColor(g_ItemArray[i].Color);
							lines.push_back(configline);
						}
						InitTree(GetDlgItem(hwnd, IDC_ITEMCODEFILTERTREE), lines);

						auto t = [](HWND hwnd) {
							this_thread::sleep_for(chrono::milliseconds(100));
							SendMessage(hwnd, WM_COMMAND, IDC_RADIO1, (LPARAM)GetDlgItem(hwnd, IDC_RADIO1));
							PrintBaseFilterCodes(GetDlgItem(hwnd, IDC_ITEMCODEFILTERTREE));
						};
						thread b(t, hwnd);
						b.detach();
					}
					PrintBaseFilterCodes(GetDlgItem(hwnd, IDC_ITEMCODEFILTERTREE));
					break;
				}
				case IDC_SAVEBTN:{
					SaveItemConfig();
					break;
				}
				case 10:{
					RECT* hack = (RECT*)lParam;
					TreeView_SetCheckStateForAllParents((HWND)hack->left, (HTREEITEM)hack->right);
					delete hack;
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_ITEMCODEFILTERTREE:{
					switch (lpnmh->code) {
						case NM_CUSTOMDRAW:{
							LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
							HWND hWndTreeView = lpnmh->hwndFrom;
							SetWindowLong(hwnd, DWL_MSGRESULT, TreeCustomDraw(hWndTreeView, pNMTVCD));
							break;
						}
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
								RECT* hack = new RECT;
								hack->left = (LONG)lpnmh->hwndFrom;
								hack->right = (LONG)tvi.hItem;
								PostMessage(hwnd, WM_COMMAND, 10, (LPARAM)hack);
								PostMessage(hwnd, WM_COMMAND, IDC_BUTTON1, lpnmh->idFrom);
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
							if (ht.flags & TVHT_ONITEMSTATEICON) {
								TVITEM tvi;
								tvi.mask = TVIF_HANDLE | TVIF_STATE | TVIF_TEXT;
								tvi.hItem = (HTREEITEM)ht.hItem;
								tvi.stateMask = TVIS_STATEIMAGEMASK;
								TreeView_SetCheckStateForAllChildren(lpnmh->hwndFrom, TreeView_GetChild(lpnmh->hwndFrom, tvi.hItem), !TreeView_GetCheckState(lpnmh->hwndFrom, tvi.hItem));
																
								ItemCode* ic = (ItemCode*)TreeView_GetItemParam(lpnmh->hwndFrom, tvi.hItem);
								if (ic)
									ic->col = TreeView_GetCheckState(lpnmh->hwndFrom, tvi.hItem) == 0 ? get_selected_color_mode() : -1;
								
								RECT* hack = new RECT;
								hack->left = (LONG)lpnmh->hwndFrom;
								hack->right = (LONG)tvi.hItem;
								PostMessage(hwnd, WM_COMMAND, 10, (LPARAM)hack);
								PostMessage(hwnd, WM_COMMAND, IDC_BUTTON1, lpnmh->idFrom);

								RedrawWindow(lpnmh->hwndFrom, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);

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
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
			break;
		}
	}
	return FALSE;
}

