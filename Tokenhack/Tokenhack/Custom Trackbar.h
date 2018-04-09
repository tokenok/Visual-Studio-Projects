#ifndef custom_trackbar_header_guard
#define custom_trackbar_header_guard

#include <Windows.h>
#include <string>
#include <vector>

static TCHAR custom_trackbar_classname[] = TEXT("custom trackbar");

#define WM_MOUSEENTER (WM_USER)
#define WM_UPDATE (WM_USER + 1)
#define HTTHUMB (WM_USER + 2)

LRESULT CALLBACK customtrackbarProc(HWND, UINT, WPARAM, LPARAM);

struct trackbarcolorscheme {
	COLORREF background;
	COLORREF border;

	COLORREF left_channel;
	COLORREF right_channel;
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
	int id;
	RECT thumb_region;
	int xpos, ypos;
	int width, height, thumb_width;
	int start_val, current_val, pos;
	int min_val, max_val;
	int small_step, large_step;
	trackbarcolorscheme color_scheme;
	int client_cur_id, thumb_cur_id;
	HCURSOR client_cur, thumb_cur;
	bool showstate;
	std::string group_name;
	bool start, focus;
	bool smooth_dragging;
	bool smooth_snap;
	bool mouse_in_client = false;
	bool dragging = false;
	bool thumb_hover = false;
	bool thumb_selected = false;

	custom_trackbar(){}
	~custom_trackbar() {}	

	custom_trackbar(const custom_trackbar& other);

	custom_trackbar& operator=(const custom_trackbar& other);

	custom_trackbar(int id,
		int xpos, int ypos,
		int width, int height,
		int min_val, int max_val, int start_val,
		int small_step, int large_step,
		int thumb_width, bool smooth_dragging, bool smooth_snap,
		trackbarcolorscheme color_scheme, 
		bool showstate, 
		int client_cur_id = 0, int thumb_cur_id = 0,
		std::string group_name = "");

	int set_pos(int pos);

	int set_val(int pos);

	int set_pos_with_val(int val);

	int move_thumb(int pos);
	void move_thumb_real(int pos);

	void thumb_hit_test();

	void set_focus(bool focus);

	bool set_range(int min, int max);
};

extern std::vector<custom_trackbar*> custom_trackbars;
extern trackbarcolorscheme c_scheme;

ATOM init_register_custom_trackbar();
bool tb_cursor_in_region(RECT, POINT);
RECT tb_getclientrect(HWND);
POINT tb_getclientcursorpos(HWND);
std::string tb_int_to_str(int);
std::wstring tb_str_to_wstr(std::string);

#endif

