#ifndef CUSTOM_EDIT_CONTROL_HEADER
#define CUSTOM_EDIT_CONTROL_HEADER

#include <Windows.h>

#include "BasicControl.h"

static TCHAR custom_edit_control_classname[] = TEXT("custom edit control");

LRESULT CALLBACK customeditcontrolProc(HWND, UINT, WPARAM, LPARAM);

class CustomEditControl: public BasicControl {
	public:

	/*struct char_info {
		wchar_t c;
		COLORREF color;
	};*/

	bool is_color;

	std::wstring text;
	std::vector<COLORREF> text_colors;
	
	int sel_s = -1, sel_e = -1;
	int caret_pos = -1;

	bool is_dragging = false;

	int scroll_speed = 2;

	int getLineCount();

	void setScrollBounds();

	int UpdateCaretPos(POINTS pt);

};

#endif