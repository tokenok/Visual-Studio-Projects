#pragma once

#include "customcontrols.h"

const TCHAR customtrackbarclassName[] = TEXT("customtrackbarclassName");

const UINT WM_customtrackbar_VALUE_CHANGED = RegisterWindowMessageA("custom trackbar value changed");

#define WM_MOUSEENTER (WM_USER)

enum CUSTOM_TRACKBAR_FLAGS {
	CTB_BORDER = 1 << 0,		//always show border, default shows when focused hides when not
	CTB_NOBORDER = 1 << 1,		//never show border, default shows when focused hides when not

	CTB_SMOOTH = 1 << 2,		//trackbar moves smoothly, default trackbar moves on value
	CTB_STAY = 1 << 3,			//trackbar doesn't jump to real value on release, default does

	CTB_VERT = 1 << 4,			//trackbar verticle, default horizontal

	CTB_THUMBVALUE = 1 << 5,	//display current value on thumb, default don't
};

struct TrackbarColorScheme {
	COLORREF background;
	COLORREF border;

	COLORREF window_name_idle;
	COLORREF window_name_highlight;

	COLORREF left_channel_idle;
	COLORREF right_channel_idle;
	COLORREF left_channel_highlight;
	COLORREF right_channel_highlight;

	COLORREF thumb_background_idle;
	COLORREF thumb_background_hover;
	COLORREF thumb_background_selected;

	COLORREF thumb_border_idle;
	COLORREF thumb_border_hover;
	COLORREF thumb_border_selected;

	COLORREF thumb_text_idle;
	COLORREF thumb_text_hover;
	COLORREF thumb_text_selected;
};

class CustomTrackbar : public BasicControl {
public:
	RECT thumb_region;
	std::string window_name;
	UINT thumb_size;
	int start_val, current_val, pos;
	int min_val, max_val;
	int small_step, large_step;
	UINT channel_size;
	UINT flags;
	TrackbarColorScheme tcolor_scheme;
	HCURSOR thumb_cursor;

	bool start;
	bool mouse_in_client = false;
	bool dragging = false;
	bool thumb_hover = false;
	bool thumb_selected = false;

	HWND scrollbar_owner_handle;

	CustomTrackbar() {}
	~CustomTrackbar() {}

	CustomTrackbar(std::string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
		int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
		TrackbarColorScheme color_scheme, HCURSOR client_cursor, HCURSOR thumb_cur, UINT owner_name, UINT group_name = 0);

	int setPos(int pos);

	int setVal(int pos);

	int setPosWithVal(int val);

	int moveThumb(int pos);
	void moveThumbReal(int pos);

	void thumbHitText();

	void setFocus(bool focus);

	bool setRange(int min, int max);

	void redrawNewVal(int val);

	static ATOM RegisterCustomClass();
};