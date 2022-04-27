#include "listview.h"

#include <windowsx.h>
#include <algorithm>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <map>
#include <regex>

#include "include.h"
#include "common.h"
#include "resource.h"
#include "treeview.h"
#include "statview.h"
#include "trade.h"

using namespace std;

LPARAM ListView_GetItemParam(HWND hList, int item) {
	LVITEM lvi = {0};
	lvi.iItem = item;
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	ListView_GetItem(hList, &lvi);
	return lvi.lParam;
}
int ListView_GetCurSel(HWND hList) {
	int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
	return iPos;
	//while (iPos != -1) {
	//	// iPos is the index of a selected item
	//	// do whatever you want with it

	//	// Get the next selected item
	//	iPos = ListView_GetNextItem(hList, iPos, LVNI_SELECTED);
	//}
}
void find_item_in_tree(const ItemData* item) {
	if (!item)
		return;
	g_TAB.selectTab(0);

	HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
	SetFocus(htv);
	TreeView_SelectItem(htv, TreeView_GetHTVofD2item(item));
}
bool compare_stats(vector<string>& item_stats, vector<string>& search_stats) {
	for (UINT j = 0; j < item_stats.size(); j++) {
		item_stats[j].erase(remove(item_stats[j].begin(), item_stats[j].end(), ' '), item_stats[j].end());
		transform(item_stats[j].begin(), item_stats[j].end(), item_stats[j].begin(), tolower);
	}

	UINT match_count = 0;
	for (UINT current_line = 0; current_line < search_stats.size(); current_line++) {
		bool match = false;
		vector<string> ORs_of_current_line = split_str(search_stats[current_line], "|");
		for (auto current_OR : ORs_of_current_line) {
			for (auto & j : item_stats) {
				if (j.find(current_OR) != string::npos) {
					match = true;
					match_count++;
					break;
				}
			}
			if (match)
				break;
		}
		if (match_count == search_stats.size())
			return true;
	}

	return false;
}
void update_result_list(HWND hwnd_parent) {
	HWND hregbtn = GetDlgItem(hwnd_parent, IDC_REGEXBUTTON);
	bool useregex = Button_GetState(hregbtn) & BST_CHECKED;
	string expression = "";

	bool advanced = Button_GetState(GetDlgItem(hwnd_parent, IDC_ADVANCEDBTN)) & BST_CHECKED;
	bool showtradeitemsonly = Button_GetState(GetDlgItem(hwnd_parent, IDC_SEARCHSHOWTRADEONLY)) & BST_CHECKED;

	//get item name search field and prepare for comparison
	HWND hsearchname = GetDlgItem(hwnd_parent, IDC_SEARCHNAME);
	string nametext = getwindowtext(hsearchname);
	nametext.erase(remove(nametext.begin(), nametext.end(), ' '), nametext.end());
	transform(nametext.begin(), nametext.end(), nametext.begin(), tolower);
	vector<string> searchnamelist = split_str(nametext, "|");
	//remove xNNN(item count) from end of item name if it exists
	for (UINT i = 0; i < searchnamelist.size(); i++) {
		if (searchnamelist[i].rfind("(x") != string::npos)
			searchnamelist[i].erase(searchnamelist[i].rfind("(x"), searchnamelist[i].size() - searchnamelist[i].rfind("(x"));
	}

	//get item stats search field and prepare for comparison (adds item name)
	HWND hedcstats = GetDlgItem(hwnd_parent, IDC_SEARCHSTATS);
	string fulltext = getwindowtext(hedcstats);
	if (!useregex) {
		fulltext.erase(remove(fulltext.begin(), fulltext.end(), '\t'), fulltext.end());
		fulltext.erase(remove(fulltext.begin(), fulltext.end(), ' '), fulltext.end());
		transform(fulltext.begin(), fulltext.end(), fulltext.begin(), tolower);
	}
	vector<string> searchstats = split_str(fulltext, "\r\n", 1);
	if (useregex && searchstats.size()) {//only use first line for regex
		expression = searchstats[0];
	}

	//get current selected item
	HWND listbox = GetDlgItem(hwnd_parent, IDC_SEARCHRESULTS);
	LPARAM previtemdata = ListView_GetItemParam(listbox, ListView_GetCurSel(listbox));

	//get original listbox contents
	vector<ItemData*> oitemlist;
	int lpos = ListView_GetNextItem(listbox, -1, LVNI_ALL);
	while (lpos != -1) {
		oitemlist.push_back((ItemData*)ListView_GetItemParam(listbox, lpos));
		lpos = ListView_GetNextItem(listbox, lpos, LVNI_ALL);
	}

	map<string, vector<string>> filter_characters;
	vector<string> filter_itemtypes;
	vector<string> filter_qualities;
	if (advanced) {
		HWND filtertree = GetDlgItem(hwnd_parent, IDC_ADVANCEDCHARFILTER);
		//get valid character list (search)		
		getfiltercharacters(filtertree, TreeView_GetChild(filtertree, TreeView_GetRoot(filtertree)), &filter_characters);

		//get valid item list (search)		
		getfilteritems(filtertree, TreeView_GetChild(filtertree, TreeView_GetNextSibling(filtertree, TreeView_GetRoot(filtertree))), &filter_itemtypes);

		//get valid item quality list (search)		
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QLOW))) {
			filter_qualities.push_back("low quality");
			filter_qualities.push_back("runeword");
			filter_qualities.push_back("ethereal/socketed");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QNORMAL))) {
			filter_qualities.push_back("normal");
			filter_qualities.push_back("runeword");
			filter_qualities.push_back("ethereal/socketed");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QSUPERIOR))) {
			filter_qualities.push_back("superior");
			filter_qualities.push_back("runeword");
			filter_qualities.push_back("ethereal/socketed");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QMAGIC))) {
			filter_qualities.push_back("magic");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QCRAFTED))) {
			filter_qualities.push_back("crafted");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QSET))) {
			filter_qualities.push_back("set");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QUNIQUE))) {
			filter_qualities.push_back("unique");
			filter_qualities.push_back("quest");
		}
		if (Button_GetCheck(GetDlgItem(hwnd_parent, IDC_QRARE))) {
			filter_qualities.push_back("rare");
		}

		//update item names list (combo box)
		int castart, caend;
		SendMessage(hsearchname, CB_GETEDITSEL, (WPARAM)&castart, (LPARAM)&caend);
		string nametext = getwindowtext(hsearchname);
		ComboBox_ResetContent(hsearchname);
		ComboBox_SetText(hsearchname, str_to_wstr(nametext).c_str());
		SendMessage(hsearchname, CB_SETEDITSEL, 0, MAKELPARAM(castart, caend));
		std::map<std::string, int> itemslist;
		for (UINT i = 0; i < g_characters.size(); i++) {
			int size = g_characters[i].getItemsCount();
			for (INT j = 0; j < size; j++) {
				ItemData* item = g_characters[i].getItem(j);
				if (find(filter_characters[item->realm].begin(), filter_characters[item->realm].end(), item->character) != filter_characters[item->realm].end() &&
					find(filter_itemtypes.begin(), filter_itemtypes.end(), item->icode.code) != filter_itemtypes.end() &&
					find(filter_qualities.begin(), filter_qualities.end(), item->quality) != filter_qualities.end()) {
					if (itemslist.find(item->name) == itemslist.end())
						itemslist.insert(make_pair(item->name, 1));
					else
						itemslist[item->name]++;
				}
			}
		}
		for (auto a : itemslist) {
			SendMessage(hsearchname, CB_ADDSTRING, 0, (LPARAM)str_to_wstr(a.first + " (x" + int_to_str(a.second) + ")").c_str());
		}		
	}

	//loop throught items for comparison
	vector<ItemData*> nitemlist;
	UINT result_count = 0;
	for (UINT i = 0; i < g_characters.size(); i++) {
		int total_items = g_characters[i].getItemsCount();
		for (INT j = 0; j < total_items; j++) {
			ItemData* temp = g_characters[i].getItem(j);
			ItemData item = *temp;

			//apply filters
			if (advanced) {
				if ((find(filter_characters[item.realm].begin(), filter_characters[item.realm].end(), item.character) == filter_characters[item.realm].end()) ||
					(find(filter_itemtypes.begin(), filter_itemtypes.end(), item.icode.code) == filter_itemtypes.end()) ||
					(find(filter_qualities.begin(), filter_qualities.end(), item.quality) == filter_qualities.end())) {
				//	printf("%s | %s | %s | %s\n", item.character.c_str(), item.name.c_str(), item.icode.type.c_str(), item.icode.code.c_str());
					continue;
				}
			}
			if (showtradeitemsonly && !item.is_trade) {
				continue;
			}

			//add things to item to be searched for (name, type, base stats)
			vector<string> to_prepare = {item.name, item.icode.type};
			for (UINT j = 0; j < to_prepare.size(); j++) {
				to_prepare[j].erase(remove(to_prepare[j].begin(), to_prepare[j].end(), ' '), to_prepare[j].end());
				transform(to_prepare[j].begin(), to_prepare[j].end(), to_prepare[j].begin(), tolower);
			}
			to_prepare.insert(to_prepare.end(), item.basestats.begin(), item.basestats.end());
			item.stats.insert(item.stats.begin(), to_prepare.begin(), to_prepare.end());
			for (UINT j = 2; j < item.stats.size(); j++) {
				item.stats[j].erase(remove(item.stats[j].begin(), item.stats[j].end(), ' '), item.stats[j].end());
				transform(item.stats[j].begin(), item.stats[j].end(), item.stats[j].begin(), tolower);
			}

			//TODO regex matching
			bool regmatch = false;
			if (useregex && expression.size()) {
				for (UINT k = 0; k < item.stats.size(); k++) {
					std::smatch sm;
					std::regex_search(item.stats[k], sm, std::regex(expression));
					if (sm.size() > 0) {
						regmatch = true;
						break;
					}

				}
			}

			bool nt = searchnamelist.size() > 0 ? true : false;
			bool st = searchstats.size() > 0 ? true : false;
			bool nm = (nt && to_prepare.size() 
				&& [&]() -> bool {
					for (UINT j = 0; j < searchnamelist.size(); j++) {
						if (searchnamelist[j].size() && to_prepare[0].find(searchnamelist[j]) == 0)
							return true;
					}
					return false;
				}()
				);
			bool sm = (st && (useregex ? regmatch : compare_stats(item.stats, searchstats)));
			if ((!nt && !st) || (!nt && sm) || (!st && nm) || (sm && nm)) {
				nitemlist.push_back(temp);
				result_count++;				
			}
		}
	}
	SetWindowText(GetDlgItem(hwnd_parent, IDC_RESULTSTATIC), str_to_wstr("Results: (" + int_to_str(result_count) + ")").c_str());

	//remove elements from tradelist
	std::sort(oitemlist.begin(), oitemlist.end());
	std::sort(nitemlist.begin(), nitemlist.end());
	vector<ItemData*> odifference;
	auto it = std::set_difference(oitemlist.begin(), oitemlist.end(), nitemlist.begin(), nitemlist.end(), std::inserter(odifference, odifference.end()));
	for (auto &tItem : odifference) {
		LVFINDINFO lvfi = {0};
		lvfi.flags = LVFI_PARAM;
		lvfi.lParam = (LPARAM)tItem;
		ListView_DeleteItem(listbox, ListView_FindItem(listbox, -1, &lvfi));
	}

	//add elements to tradelist
	vector<ItemData*> ndifference;
	it = std::set_difference(nitemlist.begin(), nitemlist.end(), oitemlist.begin(), oitemlist.end(), std::inserter(ndifference, ndifference.end()));
	for (auto tItem : ndifference) {
		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.cchTextMax = 240;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)tItem;
		lvi.pszText = str_to_LPWSTR(tItem->name);
		int pos = SendMessage(listbox, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		delete[] lvi.pszText;
		lvi.pszText = str_to_LPWSTR(tItem->character);
		ListView_SetItemText(listbox, pos, 1, lvi.pszText);
		delete[] lvi.pszText;
		lvi.pszText = str_to_LPWSTR(tItem->account);
		ListView_SetItemText(listbox, pos, 2, lvi.pszText);
		delete[] lvi.pszText;
		lvi.pszText = str_to_LPWSTR(tItem->realm);
		ListView_SetItemText(listbox, pos, 3, lvi.pszText);
		delete[] lvi.pszText;		
	}
	ListView_SortItems(listbox, ListViewCompareProc, -1); 

	//keep current selection as long as it is still in list, otherwise select first item
	LVFINDINFO lvfi = {0};
	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = previtemdata;
	int pos = ListView_FindItem(listbox, -1, &lvfi);	
	ListView_SetItemState(listbox, pos > -1 ? pos : 0, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
	if (ListView_GetItemCount(listbox) == 0) {
		SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_RESULTSTATEX), L"");
		SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_RESULTSTATS), L"");
	}	

	NMLISTVIEW nmlv = {};
	nmlv.hdr.code = LVN_ITEMCHANGED;
	nmlv.hdr.idFrom = IDC_SEARCHRESULTS;
	nmlv.iItem = pos > -1 ? pos : 0;
	nmlv.uNewState = LVIS_SELECTED;
	SendMessage(hwnd_parent, WM_NOTIFY, NULL, (LPARAM)&nmlv);
}
vector<ItemData*> ListView_GetSelectedItems(HWND tradelist) {
	vector<ItemData*> selectlist;
	int pos = ListView_GetNextItem(tradelist, -1, LVNI_SELECTED);
	while (pos != -1) {
		selectlist.push_back((ItemData*)ListView_GetItemParam(tradelist, pos));
		pos = ListView_GetNextItem(tradelist, pos, LVNI_SELECTED);
	}
	return selectlist;
}
int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	static LPARAM lastsort;
	if (lParamSort == -1)
		lParamSort = lastsort;
	lastsort = lParamSort;
	int ret = 0;
	ItemData* item1 = (ItemData*)lParam1;
	ItemData* item2 = (ItemData*)lParam2;
	if (!item1 || !item2)
		return 0;
	std::string s1, s2;
	switch (lParamSort) {
		case -5:
		case 0:
			ret = item1->name.compare(item2->name);
			break;
		case -4:
		case 1:
			ret = item1->character.compare(item2->character);
			break;
		case -3:
		case 2:
			ret = item1->account.compare(item2->account);
			break;
		case -2:
		case 3:
			ret = item1->realm.compare(item2->realm);
			break;
	}
	return lParamSort < -1 ? ret * -1 : ret;
}
int CALLBACK ListViewTradeCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	static LPARAM lastsort;
	if (lParamSort == -1)
		lParamSort = lastsort;
	lastsort = lParamSort;
	int ret = 0;
	ItemData* item1 = (ItemData*)lParam1;
	ItemData* item2 = (ItemData*)lParam2;
	if (!item1 || !item2)
		return 0;
	std::string s1, s2;
	switch (lParamSort) {
		case -5:
		case 0:
			ret = item1->name.compare(item2->name);
			break;
		case -4:
		case 1:
			ret = item1->character.compare(item2->character);
			break;
		case -3:
		case 2:
			ret = item1->account.compare(item2->account);
			break;
		case -2:
		case 3:
			ret = item1->realm.compare(item2->realm);
			break;
	}
	return lParamSort < -1 ? ret * -1 : ret;
}
LRESULT SearchResultsCustomDraw(HWND listbox, LPARAM lParam) {
	NMLVCUSTOMDRAW* lvcd = (NMLVCUSTOMDRAW*)lParam;
	static bool ishighlighted = false;

	switch (lvcd->nmcd.dwDrawStage) {
		case CDDS_PREPAINT: {
			return CDRF_NOTIFYITEMDRAW;
		}
		case CDDS_ITEMPREPAINT: {
			ishighlighted = ListView_GetItemState(listbox, lvcd->nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED;

			if (ishighlighted) {
				ListView_SetItemState(listbox, lvcd->nmcd.dwItemSpec, 0, LVIS_SELECTED);
			}

			return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		}
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:{
			ItemData* itemdata = (ItemData*)lvcd->nmcd.lItemlParam;
			if (!itemdata) break;

			lvcd->clrText = lvcd->iSubItem == 0 ? getitemqualitycolor(itemdata->quality) : RGB(150, 150, 150);
			lvcd->clrTextBk = ishighlighted ? RGB(128, 0, 0) : g_cust_color;

			return CDRF_NOTIFYPOSTPAINT;
		}
		case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:{
			if (trade::is_mytradefile()) {
				ItemData* itemdata = (ItemData*)lvcd->nmcd.lItemlParam;
				if (!itemdata) break;

				if (lvcd->iSubItem == 0 && itemdata->is_trade && listbox == GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS)) {
					RECT itemrc;
					ListView_GetSubItemRect(listbox, lvcd->nmcd.dwItemSpec, 1, LVIR_BOUNDS, &itemrc);
					itemrc.right = itemrc.left - 1;
					itemrc.left = 2;
					HBRUSH framebrush = CreateSolidBrush(RGB(255, 0, 255));
					FrameRect(lvcd->nmcd.hdc, &itemrc, framebrush);
					DeleteObject(framebrush);
				}
			}
			return CDRF_DODEFAULT;
		}
		case CDDS_ITEMPOSTPAINT:{
			if (ishighlighted)
				ListView_SetItemState(listbox, lvcd->nmcd.dwItemSpec, LVIS_SELECTED, LVIS_SELECTED);

			return CDRF_DODEFAULT;
		}
	}
	return CDRF_DODEFAULT;
}


