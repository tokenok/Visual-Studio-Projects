#ifndef window_header_guard
#define window_header_guard

#include <Windows.h>
#include <vector>
#include <string>
#include <Shellapi.h>
#include <Commdlg.h>
#include <Commctrl.h>

#pragma comment(lib, "Comctl32.lib")

#define MY_COMMAND  (WM_COMMAND + WM_USER)

#define DESTROY_WINDOWS (WM_USER + 0)
#define ECW_WM_KBHOOKKEYDOWN (WM_USER + 1)

#define CURSOR_IDLE (WM_USER + 1)
#define CURSOR_PRESS (WM_USER + 2)
#define CURSOR_LOADING (WM_USER + 3)

extern const UINT WM_customtrackbar_VALUE_CHANGED;
extern const UINT WM_customedit_TEXT_CHANGED;
extern const UINT WM_KBHOOKKEYDOWN;

extern const TCHAR tabbuttonclassName[];
extern const TCHAR autoradiobuttonclassName[];
extern const TCHAR checkboxbuttonclassName[];
extern const TCHAR normalbuttonclassName[];
extern const TCHAR staticcontrolclassName[];
extern const TCHAR EditControlclassName[];
extern const TCHAR customtrackbarclassName[];
extern const TCHAR customcomboboxclassName[];
extern const TCHAR CustomContextMenuclassName[];
extern const TCHAR CustomScrollbarclassName[];

UINT get_unique_id();

bool CreateChildren(HWND parent);

////

#pragma region BasicControl + colorscheme /*+ hover*/

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

struct BasicControl_hover {
	HWND handle = NULL;
	std::string hover_text = "default hover text";
	DWORD Flags = TME_LEAVE;
	DWORD HoverTime = 0;

	BasicControl_hover();
	BasicControl_hover(std::string hover_text, DWORD Flags, DWORD HoverTime);
};

class BasicControl {	
	public:
	std::string className;
	HWND handle;
	std::string window_text;
	UINT window_exstyles = NULL;
	UINT window_styles;
	int xpos;
	int ypos;
	int width;
	int height;
	HWND parent;
	UINT id;
	void(*Proc)(WPARAM wParam, LPARAM lParam);
	BasicControl_colorscheme color_scheme;
	BasicControl_hover hover_info;
	HFONT font;
	HCURSOR client_cursor;
	int toggle_state;
	UINT group_name = NULL;	

	bool mouse_in_client = false;
	bool focus = false;

	BasicControl();

	HWND Create(HWND parent);

	void Show();
	void Hide();
};

BasicControl* getBasicControl(HWND wnd);

#pragma endregion

///////////////////

#pragma region TabButton class

class TabButton: public BasicControl{
	public:
	UINT this_name;
	std::vector<HWND> page_windows;
	TabButton();
	TabButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT group_name, bool add_to_list = true);
	
};

#pragma endregion

#pragma region RadioButton class

class RadioButton: public BasicControl {
	public:
	int group;
	RadioButton();
	RadioButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam), int group,
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region CheckBox class

class CheckBox: public BasicControl {
	public:
	CheckBox();
	CheckBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region Button class

class Button: public BasicControl {
	public:
	Button();
	Button(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region StaticControl class

class StaticControl: public BasicControl {
	public:
	StaticControl();
	StaticControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, DWORD* hover_info, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region CustomTrackbar class

enum CUSTOM_TRACKBAR_FLAGS{
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
		TrackbarColorScheme color_scheme, HCURSOR client_cursor, HCURSOR thumb_cur, UINT group_name = 0, bool add_to_list = true);

	int setPos(int pos);

	int setVal(int pos);

	int setPosWithVal(int val);

	int moveThumb(int pos);
	void moveThumbReal(int pos);

	void thumbHitText();

	void setFocus(bool focus);

	bool setRange(int min, int max);

	void redrawNewVal(int val);
};

#pragma endregion

#pragma region CustomComboBox class

struct CustomComboBoxOption {
	std::string text;
	RECT region;

	bool hover = false;
	bool toggle_state = false;

	CustomComboBoxOption();
	CustomComboBoxOption(std::string text);
};

enum CUSTOM_COMBOBOX_STYLES {
	ccb_style0,
	ccb_style1,
	ccb_style2,
	ccb_style3
};

class CustomComboBox : public BasicControl {
	public:
	std::vector<CustomComboBoxOption> options;
	UINT selected_option = 0;
	UINT hovered_option = 0;
	UINT arrow_region_width = 15;
	UINT max_options = (UINT)-1;

	RECT box_region;
	RECT arrow_region;
	RECT arrow_region_up;
	RECT arrow_region_down;

	UINT style;

	bool is_expanded = false;
	bool box_hover = false;
	bool arrow_hover = false;
	bool arrow_hover_up = false;
	bool arrow_hover_down = false;

	CustomComboBox();
	CustomComboBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, 
		std::vector<std::string> options_text, UINT arrow_region_width, UINT max_options, UINT style, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT group_name = NULL, bool add_to_list = true);

	void setFocus(bool focus);

	void display_options(bool show);

	void change_option_hover(int pos_change, bool wrap = false);

};

#pragma endregion

#pragma region Scrollbar class

enum CUSTOM_SCROLLBAR_FLAGS {
	CSB_BORDER = 1 << 0,		//always show border, default shows when focused hides when not
	CSB_NOBORDER = 1 << 1,		//never show border, default shows when focused hides when not

	CSB_HORZ = 1 << 2,			//scrollbar horizontal, default verticle
};

struct ScrollbarColorScheme {
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

class CustomScrollbar: public BasicControl {
	public:
	RECT thumb_region;
	std::string window_name;
	UINT thumb_size;
	int start_val, current_val, pos;
	int min_val, max_val;
	int small_step, large_step;
	UINT channel_size;
	UINT flags;
	ScrollbarColorScheme scolor_scheme;
	HCURSOR thumb_cursor;

	bool start;
	bool mouse_in_client = false;
	bool dragging = false;
	bool thumb_hover = false;
	bool thumb_selected = false;

	HWND scrollbar_owner_handle;

	CustomScrollbar();
	CustomScrollbar(std::string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
		int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
		ScrollbarColorScheme scolor_scheme, HCURSOR client_cursor, HCURSOR thumb_cur, UINT group_name = 0, bool add_to_list = true);

	int setPos(int pos);

	int setVal(int pos);

	int setPosWithVal(int val);

	int moveThumb(int pos);
	void moveThumbReal(int pos);

	void thumbHitText();

	void setFocus(bool focus);

	bool setRange(int min, int max);

	void redrawNewVal(int val);
};

#pragma endregion

#pragma region EditControl class

enum EDIT_CONTROL_FLAGS {
	EDC_NUMBERONLY = 1 << 0,
	EDC_READONLY = 1 << 1,
	EDC_MULTILINE = 1 << 2,
	EDC_VSCROLL = 1 << 3,
	EDC_HSCROLL = 1 << 4,
	EDC_AUTOVSCROLL = 1 << 5,
	EDC_AUTOHSCROLL = 1 << 6
};
#define EDC_VHSCROLL EDC_VSCROLL | EDC_HSCROLL
#define EDC_AUTOVHSCROLL EDC_AUTOVSCROLL | EDC_AUTOHSCROLL
#define EDC_SCROLL EDC_VHSCROLL | EDC_AUTOVHSCROLL

struct EditTextHighlight {
	std::string left = "", highlight = "", right = "";
	UINT startpos = 0, endpos = 0;
};

class EditControl: public BasicControl {
	public:
	bool number_only;
	int min_number;
	int max_number;

	UINT scroll_ypos = 0;
	UINT scroll_xpos = 0;
	CustomScrollbar* vscrollbar;
	CustomScrollbar* hscrollbar;

	UINT tab_size = 4;

	BYTE line_height;
	UINT caret_pos;
	UINT start;
	UINT end;

	UINT flags;

	bool dragging = false;

	EditControl();
	EditControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		UINT flags, BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT group_name,
		bool number_only = false, int min_number = INT_MIN, int max_number = INT_MAX, bool add_to_list = true);

	std::vector<std::string> GetLines();

	UINT VecPosToStrPos(POINT pt);
	
	POINT PosToClient(UINT pos);

	UINT ClientToPos(POINT cursor);

	bool GetHighlightText(EditTextHighlight* eth, int line = -1);

	void EditText(std::string newtext, int pos = -1);

	bool IsTextClipped(RECT* rc);
};

extern EditControl& operator<<(EditControl& obj, int a);
extern EditControl& operator<<(EditControl& obj, char a);
extern EditControl& operator<<(EditControl& obj, std::string a);

#pragma endregion

#pragma region ContextMenu class

class CustomContextMenu: public BasicControl {
	public:
	CustomContextMenu();
	CustomContextMenu(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region customcontrols container class

class CustomControls {
	public:
	std::vector<LONG_PTR> all_controls;

	std::vector<TabButton*> tabbuttons;
	std::vector<RadioButton*> autoradiobuttons;
	std::vector<CheckBox*> checkboxbuttons;
	std::vector<Button*> normalbuttons;
	std::vector<StaticControl*> staticcontrols;
	std::vector<CustomTrackbar*> customtrackbars;
	std::vector<CustomComboBox*> customcomboboxes;
	std::vector<EditControl*> EditControls;
	std::vector<CustomContextMenu*> CustomContextMenus;
	std::vector<CustomScrollbar*> CustomScrollbars;

	CustomControls() = default;

	void addControl(TabButton* tabbtn);
	void addControl(RadioButton* rbtn);
	void addControl(CheckBox* cbtn);
	void addControl(Button* btn);
	void addControl(StaticControl* stc);
	void addControl(CustomTrackbar* ctb);
	void addControl(CustomComboBox* ccb);
	void addControl(EditControl* edc);
	void addControl(CustomContextMenu* ccxm);
	void addControl(CustomScrollbar* csb);

	BasicControl* getControl(LONG_PTR ptr);

	bool createChildren(HWND parent);
};

extern CustomControls custom_controls;

#pragma endregion

#endif

