#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include "resource.h"
#include "C:/CPPlibs/common/f/common.h"

using namespace std;

HWND g_hwnd;
HHOOK mhook;

POINT ptclicked = {};

BOOL CALLBACK MainDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*args*/, int /*iCmdShow*/) {
	SHOW_CONSOLE();

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)MainDialogProc, 0);

	return 0;
}

enum Face {
	FRONT		= 0,
	BACK		= 1 << 1,
	TOP			= 1 << 2,
	DOWN		= 1 << 3,
	LEFT		= 1 << 4,
	RIGHT		= 1 << 5
};

enum Color {
	RED			= 0, 
	ORANGE		= 1 << 1,
	WHITE		= 1 << 2,
	YELLOW		= 1 << 3, 
	GREEN		= 1 << 4,
	BLUE		= 1 << 5
};

enum class RotationType {
	COUNTERCLOCKWISE, CLOCKWISE, DOUBLE
};

class Cube {
	public:
	vector<vector<vector<int>>> cube;
	int dimension;

	double yaw = 0, pitch = 0;

	Cube(int dimension) {
		this->dimension = dimension;

		cube.resize(6);
		for (int i = 0; i < cube.size(); i++) {
			cube[i].resize(dimension);
			for (int j = 0; j < cube[i].size(); j++) {
				cube[i][j].resize(dimension);
			}
		}

		cout << "";
		
	}

	void setCubeParams(double yaw, double pitch) {
		this->yaw = yaw;
		this->pitch = pitch;
	}

	void DrawCube(HDC hdc) {



		POINT pts[5] = {{10, 10}, {10, 20}, {20, 20}, {20, 10}, {10, 10}};
		Polygon(hdc, pts, 5);
	}

	void RotateFace(Face face, RotationType rotation) {
		switch (face) {
			case Face::FRONT:{

				break;
			}
			case Face::BACK:{

				break;
			}
			case Face::TOP:{

				break;
			}
			case Face::DOWN:{

				break;
			}
			case Face::LEFT:{

				break;
			}
			case Face::RIGHT:{

				break;
			}
		}
	}
};

Cube cube(3);

BOOL CALLBACK MainDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			g_hwnd = hwnd;

			mhook = SetWindowsHookEx(WH_MOUSE_LL, hookProc, NULL, NULL);

			break;
		}
		case WM_PAINT:{
			RECT rcclient = getclientrect(hwnd);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			//////////////////////////////////////////////////	

			HBRUSH brush = CreateSolidBrush(RGB(240, 240, 240));
			FillRect(hDCmem, &rcclient, brush);
			DeleteObject(brush);

			/*HPEN pen = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
			HPEN oldpen = (HPEN)SelectObject(hDCmem, pen);
			MoveToEx(hDCmem, 0, 0, NULL);
			LineTo(hDCmem, 10, 10);
			DeleteObject(SelectObject(hDCmem, oldpen));*/

			cube.DrawCube(hDCmem);

			//////////////////////////////////////////////////////
			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			break;
		}
		case WM_COMMAND:{

			break;
		}
		case WM_MOUSEMOVE:{

			break;
		}
		case WM_LBUTTONDOWN:{
			
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}

	return FALSE;
}

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONDOWN) {
		ptclicked = getcursorpos();
	}
	if (wParam == WM_MOUSEMOVE) {
		if (GetAsyncKeyState(VK_LBUTTON) < 0) {
			RECT clientrc = getmappedclientrect(g_hwnd);

			if (PtInRect(&clientrc, ptclicked)) {
				static POINT pos = getcursorpos();

				double difx = (pos.x - (1366 >> 1)) * .2;
				double dify = (pos.y - (768 >> 1)) * .2;

				cout << difx << " " << dify << '\n';

				cube.setCubeParams(difx, dify);

				RedrawWindow(g_hwnd, NULL, NULL, RDW_INVALIDATE);
			}			
		}
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}