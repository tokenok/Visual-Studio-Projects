#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

#include "resource.h"

HWND g_hwnd;
HINSTANCE g_hInst;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void Resize() {

}

class Game {
public:
	RECT GetGridRect() {
		RECT ret;

		return ret;
	}

	void Update() {
		std::vector<POINT> killed;
		std::vector<POINT> birthed;

		//check for isolated nodes
		for (auto & a : nodes) {			
			if (a.second) {
				int count = count_live_neighbors(a.first);
				if (count < 3) killed.push_back(a.first);
			}
		}

		//check for birthed nodes
		for (auto & a : nodes) {
			if (!a.second) {
				int count = count_live_neighbors(a.first);
				if (count >= 3) birthed.push_back(a.first);
			}
		}

		for (auto &a : killed) {
			nodes[a] = false;
		}

		for (auto &a : birthed) {
			nodes[a] = false;
		}
	}

	void AddNode(POINT pt) {
		nodes[pt] = true;
	}

	int GetGridWidth() {
		return width;
	}

	int GetGridHeight() {
		return height;
	}

private:
	std::map<POINT, bool> nodes;
	int width, height;

	void do_births() {

	}

	std::vector<POINT> get_neighbors(POINT pt) {
		return {
			{ pt.x - 1, pt.y - 1	},		{ pt.x, pt.y - 1	},		{ pt.x + 1, pt.y - 1	},
			{ pt.x - 1, pt.y		},									{ pt.x + 1, pt.y		},
			{ pt.x - 1, pt.y + 1	},		{ pt.x, pt.y + 1	},		{ pt.x + 1, pt.y + 1	},
		};
	}

	int count_live_neighbors(POINT pt) {
		int count = 0;
		std::vector<POINT> neighbors = get_neighbors(pt);
		for (int i = 0; i < neighbors.size(); i++)
			count += nodes[neighbors[i]];
		return count;
	}
};

Game g_game;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
	g_hInst = hInstance;

	DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc);

	return 0;
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static bool IsCursorInClient = false;
	static bool IsDragging = false;
	static int DragCount = 0;
	static POINT ClickDownPoint = {-1, -1};
	static POINT ClickUpPoint = {-1, -1};
	static RECT SelectionRect;

	switch (message) {
		case WM_INITDIALOG: {
			g_hwnd = hwnd;

			g_game.AddNode({0, 0});

			break;
		}
		case WM_PAINT: {
			RECT rcclient;
			GetClientRect(hwnd, &rcclient);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			HBITMAP hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);

			/////////////////////////////////////////////////////////////

			HBRUSH backbrush = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDCmem, &rcclient, backbrush);
			DeleteObject(backbrush);

			/////////////////////////////////////////////////////////////

			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			//cleanup
			SelectObject(hDCmem, hbmOldBitmap);
			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_ERASEBKGND: {
			return TRUE;
		}
		case WM_COMMAND: {
			HWND ihwnd = (HWND)lParam;
			UINT iid = LOWORD(wParam);
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (iid) {

					}
					break;
				}
			}
			break;
		}
		case WM_SIZE: {
			Resize();

			break;
		}
		case WM_LBUTTONDOWN: {
			IsDragging = true;
			SetCapture(hwnd);
			DragCount = 0;
			ClickDownPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			break;
		}
		case WM_LBUTTONUP: {
			if (IsDragging) {
				ReleaseCapture();
				IsDragging = false;
				ClickUpPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				SelectionRect = {0, 0, 0, 0};

				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_RBUTTONUP: {

			
			break;
		}
		case WM_MOUSEMOVE: {
			if (!IsCursorInClient) {
				IsCursorInClient = true;
				TRACKMOUSEEVENT tme = {0};
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				TrackMouseEvent(&tme);
			}

			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if (IsDragging) {
				DragCount++;

				RECT rcclient;
				GetClientRect(hwnd, &rcclient);

				SelectionRect.left = std::min(ClickDownPoint.x, pt.x); SelectionRect.left = rcclient.left > SelectionRect.left ? rcclient.left : SelectionRect.left;
				SelectionRect.right = std::max(ClickDownPoint.x, pt.x); SelectionRect.right = rcclient.right < SelectionRect.right ? rcclient.right : SelectionRect.right;
				SelectionRect.top = std::min(ClickDownPoint.y, pt.y); SelectionRect.top = rcclient.top > SelectionRect.top ? rcclient.top : SelectionRect.top;
				SelectionRect.bottom = std::max(ClickDownPoint.y, pt.y); SelectionRect.bottom = rcclient.bottom < SelectionRect.bottom ? rcclient.bottom : SelectionRect.bottom;

				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}

