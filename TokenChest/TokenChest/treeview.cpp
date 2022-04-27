#include "treeview.h"

#include "statview.h"
#include "include.h"
#include "common.h"
#include "resource.h"
#include "trade.h"

#pragma warning(disable: 4706)

using namespace std;

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
string TreeView_GetParentText(HWND hwnd, HTREEITEM htItem, int depth/* = 1*/) {
	for (int i = 1; i < depth; i++) 
		htItem = TreeView_GetParent(hwnd, htItem);
	return TreeView_GetItemText(hwnd, htItem);
}
HTREEITEM TreeView_FindSibling(HWND hwnd, HTREEITEM start, string match) {
	HTREEITEM current = start;
	do {
		if (TreeView_GetItemText(hwnd, current) == match)
			return current;
	} while ((current = TreeView_GetNextSibling(hwnd, current)) != NULL);
	return NULL;
}
HTREEITEM TreeView_FindSibling(HWND hwnd, HTREEITEM start, LPARAM ptr) {
	HTREEITEM current = start;
	do {
		if (TreeView_GetItemParam(hwnd, current) == ptr)
			return current;
	} while ((current = TreeView_GetNextSibling(hwnd, current)) != NULL);
	return NULL;
}
HTREEITEM TreeView_FindSiblingItemCode(HWND hwnd, HTREEITEM start, string code) {
	HTREEITEM current = start;
	do {
		ItemCode* item = (ItemCode*)TreeView_GetItemParam(hwnd, current);
		if (item && item->code == code)
			return current;
	} while ((current = TreeView_GetNextSibling(hwnd, current)) != NULL);
	return NULL;
}
bool TreeView_IsSelectedD2Item() {
	return TreeView_GetChild(GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1), TreeView_GetSelection(GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1))) == NULL;
}
bool TreeView_IsSelectedD2Store() {
	HWND tree = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
	HTREEITEM cur = TreeView_GetChild(tree, TreeView_GetSelection(tree));
	if (!cur) return false;
	return TreeView_GetChild(tree, cur) == NULL;
}
bool TreeView_IsSelectedD2Char() {
	HWND tree = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
	HTREEITEM cur = TreeView_GetChild(tree, TreeView_GetSelection(tree));
	if (!cur) return false;
	cur = TreeView_GetChild(tree, TreeView_GetSelection(tree));
	if (!cur) return false;
	return TreeView_GetChild(tree, cur) == NULL;
}
HTREEITEM TreeView_GetHTVofD2item(const ItemData* item) {
	vector<string> traverse = {item->realm, item->account, item->character, item->store};

	HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
	HTREEITEM current = TreeView_GetRoot(htv);
	for (UINT i = 0; i < traverse.size(); i++) {
		current = TreeView_GetChild(htv, current);
		current = TreeView_FindSibling(htv, current, traverse[i]);
	}
	current = TreeView_GetChild(htv, current);
	return TreeView_FindSibling(htv, current, (LPARAM)*&item);
}
int TreeView_GetChildCount(HWND tree, HTREEITEM hItem) {
	int count = 0;
	while (hItem = TreeView_GetChild(tree, hItem))//generates compiler warning C4706
		count++;
	return count;
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
HTREEITEM TreeView_SetCheckStateForAllChildren(HWND tree, HTREEITEM hItem, BOOL checkstate) {
	HTREEITEM current = hItem;
	HTREEITEM sibling = NULL;
	while (current != NULL && sibling == NULL) {
		TreeView_SetCheckState(tree, current, checkstate);
		sibling = TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, current), checkstate);
		current = TreeView_GetNextSibling(tree, current);
	}
	return sibling;
}
HTREEITEM getfiltercharacters(HWND tree, HTREEITEM hItem, map<string, vector<string>> *list) {
	HTREEITEM current = hItem;
	HTREEITEM child = NULL;
	while (current != NULL && child == NULL) {
		child = getfiltercharacters(tree, TreeView_GetChild(tree, current), list);
		if ((TreeView_GetChild(tree, current) == NULL) && (TreeView_GetCheckState(tree, current) == 1)) {
			string character = TreeView_GetItemText(tree, current);
			string realm = TreeView_GetItemText(tree, TreeView_GetParent(tree, TreeView_GetParent(tree, current)));
			list->operator[](realm).push_back(character);
		}
		current = TreeView_GetNextSibling(tree, current);
	}
	return child;
}
HTREEITEM getfilteritems(HWND tree, HTREEITEM hItem, vector<string>* list) {
	HTREEITEM current = hItem;
	HTREEITEM sibling = NULL;
	while (current != NULL && sibling == NULL) {
		sibling = getfilteritems(tree, TreeView_GetChild(tree, current), list);
		if ((TreeView_GetChild(tree, current) == NULL) && (TreeView_GetCheckState(tree, current) == 1)) {
			ItemCode* ic = (ItemCode*)TreeView_GetItemParam(tree, current);
			list->push_back(ic->code);
		}
		current = TreeView_GetNextSibling(tree, current);
	}
	return sibling;
}
LRESULT FinderTreeCustomDraw(HWND tree, LPNMTVCUSTOMDRAW pNMTVCD) {
	if (pNMTVCD == NULL) return -1;
	switch (pNMTVCD->nmcd.dwDrawStage) {
		case CDDS_PREPAINT:{
			/*HBRUSH background = CreateSolidBrush(RGB(30, 30, 30));
			HBRUSH border = CreateSolidBrush(RGB(0, 0, 0));
			RECT rc = getclientrect(tree);
			FillRect(pNMTVCD->nmcd.hdc, &rc, background);
			FrameRect(pNMTVCD->nmcd.hdc, &rc, border);
			DeleteObject(background);
			DeleteObject(border);*/

			return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
		}
		case CDDS_ITEMPREPAINT: {			
			TV_ITEM tvi = {0};
			tvi.mask = TVIF_TEXT | TVIF_PARAM;
			tvi.hItem = (HTREEITEM)pNMTVCD->nmcd.dwItemSpec;
			TreeView_GetItem(tree, &tvi);
			if (tvi.lParam > 0 && TreeView_GetChild(tree, tvi.hItem) == NULL) {
				ItemData* pItemData = (ItemData*)tvi.lParam;
				if (pItemData) {
					SetTextColor(pNMTVCD->nmcd.hdc, getitemqualitycolor(pItemData->quality));
					if (pNMTVCD->nmcd.uItemState & CDIS_SELECTED)
						SetBkColor(pNMTVCD->nmcd.hdc, RGB(128, 0, 0));
					else {
						if (TreeView_GetSelection(tree) == tvi.hItem)
							SetBkColor(pNMTVCD->nmcd.hdc, RGB(80, 80, 200));//selection no focus
						else
							SetBkColor(pNMTVCD->nmcd.hdc, g_cust_color);
					}
				}
			}
			/*else
				SetBkColor(pNMTVCD->nmcd.hdc, RGB(100, 0, 0));*/
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);
		}
		case CDDS_ITEMPOSTPAINT: {
			if (trade::is_mytradefile()) {
				TV_ITEM tvi = {0};
				tvi.mask = TVIF_TEXT | TVIF_PARAM;
				tvi.hItem = (HTREEITEM)pNMTVCD->nmcd.dwItemSpec;
				TreeView_GetItem(tree, &tvi);
				ItemData* pItemData = (ItemData*)tvi.lParam;
				if (pItemData && TreeView_GetChild(tree, tvi.hItem) == NULL && pItemData->is_trade) {
					RECT rc;
					TreeView_GetItemRect(tree, tvi.hItem, &rc, TRUE);
					HBRUSH highlight = CreateSolidBrush(RGB(255, 0, 255));
					FrameRect(pNMTVCD->nmcd.hdc, &rc, highlight);
					DeleteObject(highlight);
				}
			}

			return CDRF_DODEFAULT;
		}
	}
	return CDRF_DODEFAULT;
}

