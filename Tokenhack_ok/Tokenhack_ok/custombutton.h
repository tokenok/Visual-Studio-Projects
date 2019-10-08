#pragma once

#include "basiccontrol.h"

const TCHAR buttonclassName[] = TEXT("normalbuttonclassName");

class Button : public BasicControl {
public:
	Button();
	Button(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT owner_name, UINT group_name);

	Button(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, HWND parent, void(*Proc)(WPARAM wParam, LPARAM lParam) = nullptr);

	static ATOM RegisterCustomClass();
};

