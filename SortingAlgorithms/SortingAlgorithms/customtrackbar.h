#ifndef CustomTrackbar_header_guard
#define CustomTrackbar_header_guard

#include <Windows.h>
#include <string>
#include <vector>

static TCHAR CustomTrackbar_classname[] = TEXT("custom trackbar");

#define CTB_CHANGE (WM_USER + 3)

//const UINT WM_customtrackbar_VALUE_CHANGED = RegisterWindowMessageA("custom trackbar value changed");

ATOM register_customtrackbar();

#define WM_MOUSEENTER (WM_USER)
#define WM_UPDATE (WM_USER + 1)
#define HTTHUMB (WM_USER + 2)

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

	BasicControl_colorscheme();
	BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on
	);
	BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on
		//,COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on);
	);
	BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on,
		//COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on,
		COLORREF background_idle_off, COLORREF border_idle_off, COLORREF text_idle_off,
		COLORREF background_hover_off, COLORREF border_hover_off, COLORREF text_hover_off
		//COLORREF background_selected_off, COLORREF border_selected_off, COLORREF text_selected_off);
	);
};

class BasicControl {
public:
	std::string className;
	HWND hwnd;
	std::string window_text;
	UINT window_exstyles = NULL;
	UINT window_styles;
	int xpos;
	int ypos;
	int width;
	int height;
	UINT id;
	void(*Proc)(WPARAM wParam, LPARAM lParam);
	HFONT font;
	HCURSOR client_cursor;
	int toggle_state;

	bool mouse_in_client = false;
	bool focus = false;

	BasicControl();

	HWND Create(HWND parent);

	void Show();
	void Hide();
};

BasicControl* getBasicControl(HWND wnd);



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
				   TrackbarColorScheme color_scheme, HCURSOR client_cursor = NULL, HCURSOR thumb_cur = NULL);

	int setPos(int pos);

	int setVal(int pos);

	int setPosWithVal(int val);

	int moveThumb(int pos);
	void moveThumbReal(int pos);

	void thumbHitText();

	void setFocus(bool focus, HWND hwnd_focus = NULL);

	bool setRange(int min, int max);

	void redrawNewVal(int val);

	static LRESULT CALLBACK  customtrackbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif

























