#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#include "d2data.h"

LPARAM TreeView_GetItemParam(HWND hwnd, HTREEITEM htItem);
std::string TreeView_GetItemText(HWND hwnd, HTREEITEM htItem);
std::string TreeView_GetParentText(HWND hwnd, HTREEITEM htItem, int depth = 1);
HTREEITEM TreeView_FindSibling(HWND hwnd, HTREEITEM start, std::string match);
HTREEITEM TreeView_FindSibling(HWND hwnd, HTREEITEM start, LPARAM ptr);
HTREEITEM TreeView_FindSiblingItemCode(HWND hwnd, HTREEITEM start, std::string code);
bool TreeView_IsSelectedD2Item();
bool TreeView_IsSelectedD2Store();
bool TreeView_IsSelectedD2Char();
HTREEITEM TreeView_GetHTVofD2item(const ItemData* item);
int TreeView_GetChildCount(HWND tree, HTREEITEM hItem);
void TreeView_SetCheckStateForAllParents(HWND tree, HTREEITEM child);
HTREEITEM TreeView_SetCheckStateForAllChildren(HWND tree, HTREEITEM hItem, BOOL checkstate);
HTREEITEM getfiltercharacters(HWND tree, HTREEITEM hItem, std::map<std::string, std::vector<std::string>> *list);
HTREEITEM getfilteritems(HWND tree, HTREEITEM hItem, std::vector<std::string>* list);
LRESULT FinderTreeCustomDraw(HWND hWndTreeView, LPNMTVCUSTOMDRAW pNMTVCD);

