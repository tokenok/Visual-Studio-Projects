#pragma once

#include <Windows.h>
#include <string>

#include "C:\CPPlibs\common\common.h"

struct BasicControl_colorscheme {
	COLORREF background_idle_on;
	COLORREF border_idle_on;
	COLORREF text_idle_on;
	COLORREF background_hover_on;
	COLORREF border_hover_on;
	COLORREF text_hover_on;
	//COLORREF background_selected_on;
	//COLORREF border_selected_on;
	//COLORREF text_selected_on;

	COLORREF background_idle_off;
	COLORREF border_idle_off;
	COLORREF text_idle_off;
	COLORREF background_hover_off;
	COLORREF border_hover_off;
	COLORREF text_hover_off;
	//COLORREF background_selected_off;
	//COLORREF border_selected_off;	
	//COLORREF text_selected_off;

	BasicControl_colorscheme() {}
	BasicControl_colorscheme(COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on);

	//buttons etc
	BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on);
		//,COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on);

	//buttons with toggle state
	BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on,
		//COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on,
		COLORREF background_idle_off, COLORREF border_idle_off, COLORREF text_idle_off,
		COLORREF background_hover_off, COLORREF border_hover_off, COLORREF text_hover_off);
		//COLORREF background_selected_off, COLORREF border_selected_off, COLORREF text_selected_off);
};

struct BasicControl_hover {
	HWND handle = NULL;
	std::string hover_text = "...";
	DWORD Flags = TME_LEAVE;
	DWORD HoverTime = 0;

	BasicControl_hover() {}

	BasicControl_hover(std::string hover_text, DWORD Flags, DWORD HoverTime);
};

class BasicControl {
public:
	std::string className;
	HWND handle;
	HWND parent;
	std::string window_text;
	UINT window_exstyles = NULL;
	UINT window_styles;
	int xpos;
	int ypos;
	int width;
	int height;
	UINT id;
	void(*Proc)(WPARAM wParam, LPARAM lParam);
	BasicControl_colorscheme color_scheme;
	BasicControl_hover hover_info;
	bool state;
	HFONT font;
	HCURSOR client_cursor;
	UINT owner_name;
	UINT group_name = 0;

	bool mouse_in_client = false;
	bool focus = false;

	BasicControl() {}

	HWND Create(HWND parent);

	void Show();

	void Hide();
};

BasicControl* getBasicControl(HWND wnd);

