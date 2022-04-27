#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include "d2data.h"

LPARAM ListView_GetItemParam(HWND hList, int item);
int ListView_GetCurSel(HWND hList);
void find_item_in_tree(const ItemData* item);
bool compare_stats(std::vector<std::string>& item_stats, std::vector<std::string>& search_stats);
void update_result_list(HWND hwnd_parent);
std::vector<ItemData*> ListView_GetSelectedItems(HWND tradelist);
int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int CALLBACK ListViewTradeCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
LRESULT SearchResultsCustomDraw(HWND listbox, LPARAM lParam);

