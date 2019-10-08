#ifndef BASIC_CONTROL_HEADER
#define BASIC_CONTROL_HEADER

#include <Windows.h>
#include <string>
#include <vector>

class BasicControl {
	public:
	
	LPVOID      lpCreateParams;
	HINSTANCE   hInstance;
	HMENU       hMenu;
	HWND        hwndParent;
	int         cy;
	int         cx;
	int         y;
	int         x;
	LONG        style;
	LPCWSTR     lpszName;
	LPCWSTR     lpszClass;
	DWORD       dwExStyle;

	HFONT font;

	void(*Proc)(WPARAM wParam, LPARAM lParam);


	bool mouse_in_client = false;
	bool focus = false;

	BasicControl();

	HWND Create(HWND parent);

	void Show();
	void Hide();
};

BasicControl* getBasicControl(HWND wnd);

#endif