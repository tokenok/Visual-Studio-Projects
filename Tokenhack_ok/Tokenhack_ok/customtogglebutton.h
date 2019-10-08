#pragma once

#include "basiccontrol.h"

const TCHAR togglebuttonclassName[] = TEXT("togglebuttonclassName");

class ToggleButton : public BasicControl {
public:
	int toggle_state;
	ToggleButton();
	ToggleButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name);

	static ATOM RegisterCustomClass();
};

