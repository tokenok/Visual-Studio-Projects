#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include "d2data.h"

const TCHAR newstatwndclassname[] = TEXT("free stats");

class PackingTreeNode {
	public:
	RECT binRectangle;
	PackingTreeNode * leftChild;
	PackingTreeNode * rightChild;
	bool filled;

	PackingTreeNode(RECT binRectangle) {
		this->binRectangle = binRectangle;
		leftChild = NULL;
		rightChild = NULL;
		filled = false;
	}

	RECT insert(RECT inputRectangle) {
		if (leftChild != NULL || rightChild != NULL) {
			RECT leftRectangle = leftChild->insert(inputRectangle);
			if (leftRectangle.left == 0 && leftRectangle.right == 0 && leftRectangle.top == 0 && leftRectangle.bottom == 0) {
				return rightChild->insert(inputRectangle);
			}
			return leftRectangle;
		}
		else {
			int inWidth = inputRectangle.right - inputRectangle.left;
			int inHeight = inputRectangle.bottom - inputRectangle.top;
			int binWidth = binRectangle.right - binRectangle.left;
			int binHeight = binRectangle.bottom - binRectangle.top;
			if (filled || inWidth > binWidth || inHeight > binHeight) {
				return {0, 0, 0, 0};
			}

			if (inWidth == binWidth && inHeight == binHeight) {
				filled = true;
				return binRectangle;
			}

			int widthDifference = binWidth - inWidth;
			int heightDifference = binHeight - inHeight;

			RECT leftRectangle = binRectangle;
			RECT rightRectangle = binRectangle;

			if (widthDifference > heightDifference) {
				leftRectangle.right = inputRectangle.right;
				rightRectangle.left += inWidth;
				rightRectangle.right -= inputRectangle.right;
			}
			else {
				leftRectangle.bottom = inputRectangle.bottom;
				rightRectangle.top += inputRectangle.bottom;
				rightRectangle.bottom -= inputRectangle.bottom;
			}

			leftChild = new PackingTreeNode(leftRectangle);
			rightChild = new PackingTreeNode(rightRectangle);

			return leftChild->insert(inputRectangle);
		}
	}
};

ATOM init_register_class(HINSTANCE hInstance);
COLORREF getitemqualitycolor(std::string quality);
void SetRichTextColor(HWND rich, COLORREF text_col, int start = 0, int end = -1);
BOOL HighlightTextInSelection(HWND rich, const std::vector<std::string>& targets, int start = 0, int end = -1);
int get_newline_count(std::string s);
LRESULT newstatProc(HWND, UINT, WPARAM, LPARAM);
HWND open_item_in_new_wnd(const ItemData* item);
void update_stat_view(HWND statview, const ItemData* item, int *start = 0, int *end = 0);
void update_charstat_view(HWND statview, CharacterStats* stats);

