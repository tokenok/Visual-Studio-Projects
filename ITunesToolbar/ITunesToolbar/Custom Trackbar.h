#ifndef custom_trackbar_header_guard
#define custom_trackbar_header_guard

#include <Windows.h>
#include <string>
#include <vector>

static TCHAR custom_trackbar_classname[] = TEXT("Custom Trackbar");

extern UINT WM_CUSTOM_TRACKBAR_VALUE_CHANGED;

LRESULT CALLBACK customtrackbarProc(HWND, UINT, WPARAM, LPARAM);

#define CTB_HIDE_FOUCS_RECT				0
#define CTB_SHOW_FOCUS_RECT				1

#define CTB_DRAGGING_VALUE				0
#define CTB_DRAGGING_SMOOTH				1

#define CTB_DRAGGING_SMOOTH_STAY		0
#define CTB_DRAGGING_SMOOTH_VALUE_SNAP	1

struct trackbarcolorscheme {
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

class custom_trackbar {
	public:
	HWND handle;
	RECT thumb_region;
	std::string window_name;
	UINT window_styles;
	int xpos, ypos;
	UINT width, height, thumb_width;
	int id;
	int start_val, current_val, pos;
	int min_val, max_val;
	int small_step, large_step;
	UINT channel_size;
	trackbarcolorscheme color_scheme;
	HCURSOR client_cur, thumb_cur;
	bool always_show_border;
	std::string group_name;
	bool start, focus;
	bool smooth_dragging;
	bool smooth_snap;

	bool mouse_in_client = false;
	bool dragging = false;
	bool thumb_hover = false;
	bool thumb_selected = false;

	custom_trackbar() {}
	~custom_trackbar() {}

	custom_trackbar(const custom_trackbar& other);

	custom_trackbar& operator=(const custom_trackbar& other);

	custom_trackbar(std::string window_name, UINT window_styles,
		int xpos, int ypos,
		UINT width, UINT height,
		int id,
		int min_val, int max_val, int start_val,
		int small_step, int large_step,
		UINT thumb_width, bool smooth_dragging, bool smooth_snap,
		trackbarcolorscheme color_scheme,
		bool always_show_border = false, UINT channel_size = 5,
		HCURSOR client_cur = LoadCursor(NULL, IDC_ARROW), HCURSOR thumb_cur = LoadCursor(NULL, IDC_HAND),
		std::string group_name = "");

	int set_thumb_pos(int pos);

	int set_val_from_pos(int pos);

	int set_pos_with_val(int val);

	int move_thumb(int pos);
	void move_thumb_real(int pos);

	void thumb_hit_test();

	void set_focus(bool focus);

	bool set_range(int min, int max);

	void redraw_newval(int val, bool notify = true);

	void PostValueChanged();
};

ATOM init_register_custom_trackbar();
bool ctb_cursor_in_region(RECT, POINT);
RECT ctb_getclientrect(HWND);
POINT ctb_getclientcursorpos(HWND);
std::string ctb_int_to_str(int);
std::wstring ctb_str_to_wstr(std::string);

#endif

