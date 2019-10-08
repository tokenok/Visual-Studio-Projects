#pragma once

#include "basiccontrol.h"

const TCHAR radiobuttonclassName[] = TEXT("radiobuttonclassName");

class RadioButton : public BasicControl {
public:
	int group;
	int toggle_state;
	RadioButton();
	RadioButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam), int group,
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name);

	static ATOM RegisterCustomClass();

	static vector<RadioButton*> RadioButtons;
};

