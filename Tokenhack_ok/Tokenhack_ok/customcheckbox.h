#pragma once

#include "basiccontrol.h"

const TCHAR checkboxbuttonclassName[] = TEXT("checkboxbuttonclassName");

class CheckBox : public BasicControl {
public:
	int toggle_state;
	CheckBox();
	CheckBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name);

	static ATOM RegisterCustomClass();
};

