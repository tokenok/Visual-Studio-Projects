#pragma once

#include "basiccontrol.h"

const TCHAR staticcontrolclassName[] = TEXT("staticcontrolclassName");

class StaticControl : public BasicControl {
public:
	StaticControl();
	StaticControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT owner_name, UINT group_name);
	
	static ATOM RegisterCustomClass();
};

