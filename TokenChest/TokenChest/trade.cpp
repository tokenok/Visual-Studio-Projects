#include "trade.h"

#include <Windows.h>
#include <windowsx.h>
#include <map>
#include <fstream>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#include "include.h"
#include "common.h"
#include "resource.h"
#include "listview.h"
#include "statview.h"
#include "treeview.h"

#pragma warning(disable: 4503)

using namespace std;

namespace trade {
	const std::string deffilename = getexedir() + string("\\char data\\FT.trade");
	string tradefilename = deffilename;

	vector<CharacterData> cdata;
	//realm     //account   //character //store     //items
	map<string, map<string, map<string, map<string, vector<ItemData*>>>>> data;//generates Warning: C4503

	Undo undo;

	bool findtradeitem(const ItemData* item) {
		vector<ItemData*>& titems = data[item->realm][item->account][item->character][item->store];
		for (UINT i = 0; i < titems.size(); i++)
			if (*titems[i] == *item)
				return true;
		return false;		
	}
	bool is_itemcountmax(const ItemData* item, int cmp_depth/* = CMP_CHARACTER*/) {
		int realcount = 0;
		for (UINT i = 0; i < g_characters.size(); i++) {
			UINT count = g_characters[i].getItemsCount();
			for (UINT j = 0; j < count; j++) {
				ItemData* myitem = g_characters[i].getItem(j);
				if (itemcmp(*myitem, *item, cmp_depth))
					realcount++;
			}
		}
		vector<ItemData*>& titems = data[item->realm][item->account][item->character][item->store];
		int tradecount = 0;
		for (UINT i = 0; i < titems.size(); i++) {
			if (*titems[i] == *item)
				tradecount++;
		}
		return realcount <= tradecount;
	}
	ItemData* findrealitem(ItemData* item, int cmp_depth = CMP_CHARACTER) {
		for (UINT l = CMP_INVPOS; l >= (UINT)cmp_depth; l /= 2) {
			for (UINT i = 0; i < g_characters.size(); i++) {
				UINT count = g_characters[i].getItemsCount();
				for (UINT j = 0; j < count; j++) {
					ItemData* myitem = g_characters[i].getItem(j);
					if (itemcmp(*myitem, *item, l))
						return myitem;
				}
			}
		}
		return NULL;
	}
	int displayret(int ret) {
		switch (ret) {
			case ADD_OK:{
				ShowStatus("Successfully added new item!");
				break;
			}
			case REMOVE_OK:{
				ShowStatus("Successfully removed item!");
				break;
			}
			case NOT_MYFILE:{
				ShowStatus("Cannot modify unowned trade file.");
				break;
			}
			case ALREADY_ADDED:{
				ShowStatus("Add failed: item already added.");
				break;
			}
			case NOT_IN_TRADE_FILE:{
				ShowStatus("Remove failed: item not found.");
				break;
			}
			case NOT_AN_ITEM:{
				ShowStatus("Failure: not an item.");
				break;
			}
		}
		return ret;
	}
	bool is_mytradefile() {
		return trade::deffilename == trade::tradefilename;
	}
	bool load(string filename/* = tradefilename*/) {
		tradefilename = filename;
		ShowWindow(GetDlgItem(g_ttab3, IDC_LOADEDTRADEDISPLAY), SW_SHOW);
		ShowWindow(GetDlgItem(g_ttab3, IDC_LOADEDISODISPLAY), SW_HIDE);
		SetWindowText(GetDlgItem(g_ttab3, IDC_LOADEDTRADEDISPLAY), str_to_wstr("Loaded file: " + filename).c_str());
		SendMessage(GetDlgItem(g_ttab3_1, IDC_TRADELIST), LVM_DELETEALLITEMS, 0, 0);
		cdata.clear();
		data.clear();
		undo.clear();

		if (is_mytradefile()) {
			Button_Enable(GetDlgItem(g_ttab3_1, IDC_TRADELOADBTN), TRUE);
			Button_Enable(GetDlgItem(g_ttab3_1, IDC_TRADESAVEBTN), TRUE);
			Button_Enable(GetDlgItem(g_ttab3_1, IDC_TRADEREMOVEBTN), TRUE);
		}
		else {
			g_characters.clear();
			Button_Enable(GetDlgItem(g_ttab3_1, IDC_TRADELOADBTN), FALSE);
			Button_Enable(GetDlgItem(g_ttab3_1, IDC_TRADESAVEBTN), FALSE);
			Button_Enable(GetDlgItem(g_ttab3_1, IDC_TRADEREMOVEBTN), FALSE);
		}

		std::ifstream file;
		file.open(filename);
		if (!file.is_open()) return false;
		while (file.good()) {
			string str, realm, account, character, store, item;
			getline(file, str, '\n');
			while (str.find("!") == 0 && file.good()) {
				realm = str.substr(1, str.size() - 1);
				getline(file, str, '\n');
				while (str.find("@") == 0 && file.good()) {
					account = str.substr(1, str.size() - 1);
					getline(file, str, '\n');
					while (str.find("#") == 0 && file.good()) {
						character = str.substr(1, str.size() - 1);
						getline(file, str, '\n');
						CharacterData FTitems;
						while (str.find("$") == 0 && file.good()) {
							store = str.substr(1, str.size() - 1);
							getline(file, str, '\n');
							while (str.find("%") == 0 && file.good()) {
								item = str.substr(1, str.size() - 1);
								getline(file, str, '\n');
								FTitems.realm = realm;
								FTitems.account = account;
								FTitems.character = character;

								ItemData idata;
								idata.is_trade = true;
								idata.realm = realm;
								idata.account = account;
								idata.character = character;
								idata.store = store;
								idata.name = item;
								vector<string> dat = split_str(str, ",");
								if (dat.size() < 8)
									continue;
								idata.quality = dat[0];								
								idata.icode.code = dat[1];
								getItemDataFromCode(idata.icode.code, &idata, true);
								idata.invdata.inv = dat[2];
								idata.invdata.x = str_to_int(dat[3]);
								idata.invdata.y = str_to_int(dat[4]);
								idata.invdata.w = str_to_int(dat[5]);
								idata.invdata.h = str_to_int(dat[6]);
								idata.trade_depth = str_to_int(dat[7]);
																   
								while (str.size() > 0 
									&& str[0] != '!'
									&& str[0] != '@'
									&& str[0] != '#'
									&& str[0] != '$'
									&& str[0] != '%'
									&& file.good()) {
									getline(file, str, '\n');
									if (str.find("<b>") == 0)
										idata.basestats.push_back(str.substr(3, str.size() - 1));
									if (str.find("<m>") == 0)
										idata.stats.push_back(str.substr(3, str.size() - 1));
								}

								if (store == "stash")
									FTitems.stash_items.push_back(idata);
								else if (store == "body")
									FTitems.body_items.push_back(idata);
								else if (store == "inventory")
									FTitems.inv_items.push_back(idata);
								else if (store == "cube")
									FTitems.cube_items.push_back(idata);
								else if (store == "mercenary")
									FTitems.merc_items.push_back(idata);
							}
						}	
						trade::cdata.push_back(FTitems);
					}					
				}				
			}			
		}
		file.close();

		if (!is_mytradefile())
			g_characters = cdata;
		g_realms.clear();
		for (UINT i = 0; i < g_characters.size(); i++) {
			UINT count = g_characters[i].getItemsCount();
			for (UINT j = 0; j < count; j++) {
				ItemData* item = g_characters[i].getItem(j);
				g_realms[item->realm][item->account][item->character][item->store].push_back(item);
			}
		}

		int items_not_found = 0;
		int real_items_not_found = 0;
		int item_pos_change = 0;
		for (UINT i = 0; i < trade::cdata.size(); i++) {
			UINT count = cdata[i].getItemsCount();
			for (UINT j = 0; j < count; j++) {
				ItemData* item = cdata[i].getItem(j);
				ItemData* myitem = findrealitem(item, item->trade_depth);
				if (!is_itemcountmax(item, item->trade_depth)) {
					if (myitem) {
						if (!(*myitem == *item))
							item_pos_change++;
						myitem->is_trade = true;
						data[item->realm][item->account][item->character][item->store].push_back(myitem);
					}
					else
						items_not_found++;
				}
				else
					real_items_not_found++;				
			}
		}
		if (is_mytradefile()) {
			if (items_not_found || real_items_not_found || item_pos_change)
				writefile();
		}
		else {
			SendMessage(g_finder, WM_COMMAND, IDC_REFRESHBTN, 0);
		}

		return true;
	}
	bool writefile() {
		std::ofstream file;
		file.open(tradefilename);
		if (!file.is_open()) return false;
		for (auto realm : data) {
			file << "!" << realm.first << '\n';
			for (auto & account : realm.second) {
				file << "@" << account.first << '\n';
				for (auto & character : account.second) {
					file << "#" << character.first << '\n';
					for (auto & store : character.second) {
						file << "$" << store.first << '\n';
						for (auto & item : store.second) {
							file << "%" << item->name << '\n';
							string invfile = item->quality == "unique" && item->invdata.invu.size() > 0 ? item->invdata.invu
								: item->quality == "set" && item->invdata.invs.size() > 0 ? item->invdata.invs
								: item->invdata.inv;
							file << item->quality.c_str() << "," << item->icode.code.c_str() << "," << invfile.c_str() << ","
								<< item->invdata.x << "," << item->invdata.y << "," << item->invdata.w << "," << item->invdata.h 
								<< "," << item->trade_depth << '\n';
							for (UINT i = 0; i < item->basestats.size(); i++)
								file << "<b>" << item->basestats[i] << '\n';
							for (UINT i = 0; i < item->stats.size(); i++)
								file << "<m>" << item->stats[i] << '\n';
						}
					}
				}
			}
		}
		file.close();
		return true;
	}
	bool update() {
		HWND tradelist = GetDlgItem(g_Tradetab.getTab(0).wnd, IDC_TRADELIST);

		int pos = ListView_GetCurSel(tradelist);
		SendMessage(tradelist, LVM_DELETEALLITEMS, 0, 0);

		for (auto realm : trade::data) {			
			for (auto & account : realm.second) {				
				for (auto & character : account.second) {				
					for (auto & store : character.second) {					
						for (auto & item : store.second) {
							LVITEM lvi = {0};
							lvi.mask = LVIF_TEXT | LVIF_PARAM;
							lvi.cchTextMax = 240;
							lvi.iItem = 0;
							lvi.iSubItem = 0;
							lvi.lParam = (LPARAM)item;
							lvi.pszText = str_to_LPWSTR(item->name);
							int pos = SendMessage(tradelist, LVM_INSERTITEM, 0, (LPARAM)&lvi);
							delete[] lvi.pszText;
							lvi.pszText = str_to_LPWSTR(item->character);
							ListView_SetItemText(tradelist, pos, 1, lvi.pszText);
							delete[] lvi.pszText;
							lvi.pszText = str_to_LPWSTR(item->account);
							ListView_SetItemText(tradelist, pos, 2, lvi.pszText);
							delete[] lvi.pszText;
							lvi.pszText = str_to_LPWSTR(item->realm);
							ListView_SetItemText(tradelist, pos, 3, lvi.pszText);
							delete[] lvi.pszText;
						}
					}
				}
			}
		}
		int count = ListView_GetItemCount(tradelist);
		if (count) {
			ListView_SetItemState(tradelist, count <= pos ? count - 1 : pos < 0 ? 0 : pos, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
			ListView_SortItems(tradelist, ListViewTradeCompareProc, -1);
		}
		else {
			SetWindowText(GetDlgItem(g_ttab3, IDC_TRADESTATS), L"");
		}

		//show item count in header
		wchar_t txt[50];
		LVCOLUMN lvc = {0};
		lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		lvc.pszText = txt;
		lvc.cchTextMax = 50;
		ListView_GetColumn(tradelist, 0, &lvc);
		std::string te = LPWSTR_to_str(lvc.pszText, lvc.cchTextMax);
		std::string text = "";
		for (UINT i = 0; i < te.size(); i++) {
			if (te[i] == '\0')
				break;
			text += te[i];
		}
		text = text.substr(0, te.rfind(" ("));
		text += std::string(" (" + int_to_str(count) + ")");
		lvc.pszText = str_to_LPWSTR(text);
		lvc.cchTextMax = text.length();
		ListView_SetColumn(tradelist, 0, &lvc);
		delete[] lvc.pszText;

		return true;
	}
	int add(ItemData* item, bool write) {
		return add(vector<ItemData*>{item}, write);
	}
	int remove(ItemData* item, bool write) {
		return remove(vector<ItemData*>{item}, write);
	}
	int add(vector<ItemData*> items, bool write/* = true*/) {
		if (!is_mytradefile())
			return displayret(NOT_MYFILE);

		vector<ItemData*> addeditems;
		
		for (UINT k = 0; k < items.size(); k++) {
			ItemData* item = items[k];
			if (item) {
				if (is_itemcountmax(item, CMP_INVPOS)) {
					displayret(ALREADY_ADDED);
					continue;//return displayret(ALREADY_ADDED);
				}

				item->is_trade = true;
				trade::data[item->realm][item->account][item->character][item->store].push_back(item);

				addeditems.push_back(item);

				HWND tradelist = GetDlgItem(g_Tradetab.getTab(0).wnd, IDC_TRADELIST);

				int pos = ListView_GetCurSel(tradelist);

				LVITEM lvi = {0};
				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.cchTextMax = 240;
				lvi.iItem = 0;
				lvi.iSubItem = 0;
				lvi.lParam = (LPARAM)item;
				lvi.pszText = str_to_LPWSTR(item->name);
				int ipos = SendMessage(tradelist, LVM_INSERTITEM, 0, (LPARAM)&lvi);
				delete[] lvi.pszText;
				lvi.pszText = str_to_LPWSTR(item->character);
				ListView_SetItemText(tradelist, ipos, 1, lvi.pszText);
				delete[] lvi.pszText;
				lvi.pszText = str_to_LPWSTR(item->account);
				ListView_SetItemText(tradelist, ipos, 2, lvi.pszText);
				delete[] lvi.pszText;
				lvi.pszText = str_to_LPWSTR(item->realm);
				ListView_SetItemText(tradelist, ipos, 3, lvi.pszText);
				delete[] lvi.pszText;
				ListView_SetItemState(tradelist, pos > ipos ? pos + 1 : pos, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
				ListView_SortItems(tradelist, ListViewTradeCompareProc, -1);

				//show item count in header
				wchar_t txt[50];
				LVCOLUMN lvc = {0};
				lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
				lvc.pszText = txt;
				lvc.cchTextMax = 50;
				ListView_GetColumn(tradelist, 0, &lvc);
				std::string te = LPWSTR_to_str(lvc.pszText, lvc.cchTextMax);
				std::string text = "";
				for (UINT i = 0; i < te.size(); i++) {
					if (te[i] == '\0')
						break;
					text += te[i];
				}
				text = text.substr(0, te.rfind(" ("));
				text += std::string(" (" + int_to_str(ListView_GetItemCount(tradelist)) + ")");
				lvc.pszText = str_to_LPWSTR(text);
				lvc.cchTextMax = text.length();
				ListView_SetColumn(tradelist, 0, &lvc);
				delete[] lvc.pszText;

				//update tree rect and invview rect
				HWND tree = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
				RECT rc;
				TreeView_GetItemRect(tree, TreeView_GetHTVofD2item(item), &rc, TRUE);
				RedrawWindow(tree, &rc, NULL, RDW_INVALIDATE);
				RedrawWindow(GetDlgItem(g_TAB.getTab(0).wnd, IDC_INVDISPLAY), NULL, NULL, RDW_INVALIDATE);
			}
			else
				/*return */displayret(NOT_AN_ITEM);
		}
		if (write)
			writefile();

		if (!undo.is_stacklocked())
			undo.push(addeditems, UNDO_ADD);

		if (Button_GetState(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSHOWTRADEONLY)) & BST_CHECKED)
			update_result_list(g_TAB.getTab(1).wnd);
		RedrawWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), NULL, NULL, RDW_INVALIDATE);

		return displayret(ADD_OK);
	}
	int remove(vector<ItemData*> items, bool write/* = true*/) {
		if (!is_mytradefile())
			return displayret(NOT_MYFILE);

		vector<ItemData*> removeditems;

		int ret = NOT_IN_TRADE_FILE;
		for (UINT k = 0; k < items.size(); k++) {
			ItemData* item = items[k];
			if (item) {				
				vector<ItemData*>& titems = trade::data[item->realm][item->account][item->character][item->store];
				vector<ItemData*> proxy = titems;
				for (UINT i = 0; i < proxy.size(); i++) {
					if (itemcmp(*proxy[i], *item, CMP_INVPOS)) {
						titems.erase(find(titems.begin(), titems.end(), item));
						
						removeditems.push_back(item);
						if (!titems.size()) {
							data[item->realm][item->account][item->character].erase(item->store);
							if (!data[item->realm][item->account][item->character].size()) {
								data[item->realm][item->account].erase(item->character);
								if (!data[item->realm][item->account].size()) 
									data[item->realm].erase(item->account);
							}
						}

						ret = REMOVE_OK;
						item->is_trade = false;

						HWND tradelist = GetDlgItem(g_Tradetab.getTab(0).wnd, IDC_TRADELIST);

						//save current selection pos
						int pos = ListView_GetCurSel(tradelist);

						//delete selected item from list
						LVFINDINFO lvfi = {0};
						lvfi.flags = LVFI_PARAM;
						lvfi.lParam = (LPARAM)item;
						int pso = ListView_FindItem(tradelist, -1, &lvfi);
						ListView_DeleteItem(tradelist, pso);

						//restore highlight to new item (item below removed)
						int count = ListView_GetItemCount(tradelist);
						if (count) {
							ListView_SetItemState(tradelist, count <= pos ? count - 1 : pos < 0 ? 0 : pos, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
						}
						else
							SetWindowText(GetDlgItem(g_ttab3, IDC_TRADESTATS), L"");

						//show item count in header
						wchar_t txt[50];
						LVCOLUMN lvc = {0};
						lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
						lvc.pszText = txt;
						lvc.cchTextMax = 50;
						ListView_GetColumn(tradelist, 0, &lvc);
						std::string te = LPWSTR_to_str(lvc.pszText, lvc.cchTextMax);
						std::string text = "";
						for (UINT i = 0; i < te.size(); i++) {
							if (te[i] == '\0')
								break;
							text += te[i];
						}
						text = text.substr(0, te.rfind(" ("));
						text += std::string(" (" + int_to_str(count) + ")");
						lvc.pszText = str_to_LPWSTR(text);
						lvc.cchTextMax = text.length();
						ListView_SetColumn(tradelist, 0, &lvc);
						delete[] lvc.pszText;

						//update tree rect and invview rect
						HWND tree = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
						RECT rc;
						TreeView_GetItemRect(tree, TreeView_GetHTVofD2item(item), &rc, TRUE);
						RedrawWindow(tree, &rc, NULL, RDW_INVALIDATE);
						RedrawWindow(GetDlgItem(g_TAB.getTab(0).wnd, IDC_INVDISPLAY), NULL, NULL, RDW_INVALIDATE);
					}
				}
			}
			else
				/*return */displayret(NOT_AN_ITEM);			
		}
		if (write)
			writefile();

		if (!undo.is_stacklocked())
			undo.push(removeditems, UNDO_REMOVE);

		if (Button_GetState(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSHOWTRADEONLY)) & BST_CHECKED)
			update_result_list(g_TAB.getTab(1).wnd);
		RedrawWindow(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS), NULL, NULL, RDW_INVALIDATE);

		return displayret(ret);
	}

	void Undo::push(ItemData* item, int action) {
		undostack.push(std::make_pair(vector<ItemData*>{item}, action));
	}
	void Undo::push(vector<ItemData*> items, int action) {
		undostack.push(std::make_pair(items, action));
	}
	bool Undo::undo() {
		if (!undostack.empty()) {
			switch (undostack.top().second) {
				case UNDO_ADD:{
					stacklock = true;
					trade::remove(undostack.top().first);
					stacklock = false;
					redostack.push(std::make_pair(undostack.top().first, REDO_ADD));
					undostack.pop();					
					break;
				}
				case UNDO_REMOVE:{
					stacklock = true;
					trade::add(undostack.top().first);
					stacklock = false;
					redostack.push(std::make_pair(undostack.top().first, REDO_REMOVE));
					undostack.pop();
					break;
				}
				default:
					return false;
			}
			return true;
		}
		return false;
	}
	bool Undo::redo() {
		if (!redostack.empty()) {
			switch (redostack.top().second) {
				case REDO_ADD:{
					stacklock = true;
					trade::add(redostack.top().first);
					stacklock = false;
					undostack.push(std::make_pair(redostack.top().first, UNDO_ADD));
					redostack.pop();
					break;
				}
				case REDO_REMOVE:{
					stacklock = true;
					trade::remove(redostack.top().first);
					stacklock = false;
					undostack.push(std::make_pair(redostack.top().first, UNDO_REMOVE));
					redostack.pop();
					break;
				}
				default:
					return false;
			}
			return true;
		}
		return false;
	}
	void Undo::clear() {
		while (!undostack.empty())
			undostack.pop();
		while (!redostack.empty())
			redostack.pop();
	}
}

