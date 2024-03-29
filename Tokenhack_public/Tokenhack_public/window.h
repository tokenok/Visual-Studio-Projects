#ifndef window_header_guard
#define window_header_guard

#include <Windows.h>
#include <vector>
#include <string>
#include <Shellapi.h>
#include <Commdlg.h>

#pragma comment(lib, "Comctl32.lib")

#define VERSION L"TokenHack_v2.176"

#define WM_TRAYICON (WM_USER + 0)
#define WM_TEST (WM_USER + 1)
#define WM_TEST2 (WM_USER + 2)
#define WM_TEST3 (WM_USER + 3)
#define WM_RAINBOW (WM_USER + 4)
#define WM_TEST4 (WM_USER + 5)

#define MY_COMMAND  (WM_COMMAND + WM_USER)

#define DESTROY_WINDOWS (WM_USER + 0)
#define ECW_WM_KBHOOKKEYDOWN (WM_USER + 1)

#define CURSOR_IDLE (WM_USER + 1)
#define CURSOR_PRESS (WM_USER + 2)
#define CURSOR_LOADING (WM_USER + 3)

extern const TCHAR className[];
extern const UINT WM_TASKBARCREATED;
extern const UINT WM_customtrackbar_VALUE_CHANGED;
extern const UINT WM_customedit_TEXT_CHANGED;
extern const UINT WM_KBHOOKKEYDOWN;

extern const TCHAR tabbuttonclassName[];
extern const TCHAR autoradiobuttonclassName[];
extern const TCHAR checkboxbuttonclassName[];
extern const TCHAR normalbuttonclassName[];
extern const TCHAR togglebuttonclassName[];
extern const TCHAR staticcontrolclassName[];
extern const TCHAR EditControlclassName[];
extern const TCHAR customtrackbarclassName[];
extern const TCHAR customcomboboxclassName[];
extern const TCHAR CustomContextMenuclassName[];
extern const TCHAR CustomScrollbarclassName[];

extern NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);
BOOL CALLBACK EnumChildProc(HWND, LPARAM);
BOOL CALLBACK hotkeydlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK configdlgProc(HWND, UINT, WPARAM, LPARAM);

extern HWND g_hwnd;
extern HWND config;
extern HHOOK kbhook;
extern HHOOK mhook;
extern HWND statbox;
extern HCURSOR idle_cursor;
extern HCURSOR press_cursor;
extern HCURSOR loading_cursor;
extern HCURSOR token_cursor;
extern HFONT Font_a;
extern HMENU g_menu;
extern MSG msg;

extern bool is_hotkey_dialog_visible;
extern bool is_config_dialog_visible;
extern bool toggleall;
extern bool is_drawing_static;
extern UINT volume;
extern DWORD global_change_hotkey;
extern DWORD global_hook_key;

extern std::vector<DWORD> hotkeys;

extern HBRUSH g_tempbrush;

extern bool move_window;
extern HWND lastwindow;

extern std::string getvktext(int vk);

////

extern enum { statboxid = 2000 };//other ids

#pragma region tokenhack function window position and size info

extern int totalonfunctions;

extern int tokenhackfunctionwindowxposoffset;

extern int windowwidth;
extern int functionnameyoffsum;

extern int functionnameheight;
extern int functionnamewidth;

extern int tabbuttonheight;

extern int checkboxwidth;
extern int checkboxheight;

extern int buttonwidth;
extern int buttonheight;

extern int checkboxxoff;
extern int buttonxoff;
extern int functionnamexoff;
extern int statboxxoff;

extern int bottomheight;

#pragma endregion

#pragma region owners and groups

enum owner_names {
	own_no_owner,
	own_root_window,

	own_BEGIN_tokenhack,
	own_tokenhack_tab_D2,
	own_tokenhack_tab_console,
	own_tokenhack_tab_box,
	own_END_tokenhack,

	own_stat_tab_input,
	own_stat_tab_output,	

	own_hotkey_dialog_box,

	own_BEGIN_config_dialog_box,
	own_config_dialog_box,
	own_config_dialog_tab_functions,
	own_config_dialog_tab_misc,
	own_config_dialog_tab_test,
	own_END_config_dialog_box
};

enum group_names {
	grp_no_group,
	grp_tokenhack_function,
	grp_function_tab_button,
	grp_stat_tab_button,
	grp_tokenhack_offset,
	grp_config_tab_button
};

#pragma endregion

#pragma region BasicControl + colorscheme + hover

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
	UINT id;
	void(*Proc)(WPARAM wParam, LPARAM lParam);
	BasicControl_colorscheme color_scheme;
	BasicControl_hover hover_info;
	HFONT font;
	HCURSOR client_cursor;
	int toggle_state;
	UINT owner_name;
	UINT group_name = grp_no_group;	

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
	TabButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT this_name, UINT group_name, bool add_to_list = true);
	
};

#pragma endregion

#pragma region RadioButton class

class RadioButton: public BasicControl {
	public:
	int group;
	RadioButton();
	RadioButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam), int group,
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region CheckBox class

class CheckBox: public BasicControl {
	public:
	CheckBox();
	CheckBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region Button class

class Button: public BasicControl {
	public:
	Button();
	Button(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT owner_name, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region ToggleButton class

class ToggleButton: public BasicControl {
	public:
	std::string string_in_file;
	ToggleButton();
	ToggleButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		std::string string_in_file,
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name, bool add_to_list = true);
};

#pragma endregion

#pragma region StaticControl class

class StaticControl: public BasicControl {
	public:
	StaticControl();
	StaticControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT owner_name, UINT group_name, bool add_to_list = true);
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
		TrackbarColorScheme color_scheme, HCURSOR client_cursor, HCURSOR thumb_cur, UINT owner_name, UINT group_name = 0, bool add_to_list = true);

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
	CustomComboBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, 
		std::vector<std::string> options_text, UINT arrow_region_width, UINT max_options, UINT style, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT owner_name, UINT group_name = grp_no_group, bool add_to_list = true);

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
		ScrollbarColorScheme scolor_scheme, HCURSOR client_cursor, HCURSOR thumb_cur, UINT owner_name, UINT group_name = 0, bool add_to_list = true);

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
	EditControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		UINT flags, BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT owner_name, UINT group_name,
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
	CustomContextMenu(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
		BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT owner_name, UINT group_name, bool add_to_list = true);
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
	std::vector<ToggleButton*> togglebuttons;
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
	void addControl(ToggleButton* tbtn);
	void addControl(StaticControl* stc);
	void addControl(CustomTrackbar* ctb);
	void addControl(CustomComboBox* ccb);
	void addControl(EditControl* edc);
	void addControl(CustomContextMenu* ccxm);
	void addControl(CustomScrollbar* csb);

	BasicControl* getControl(LONG_PTR ptr);

	bool createChildren(HWND parent, bool(*control_condition)(BasicControl*));
};

extern CustomControls custom_controls;

#pragma endregion

#pragma region tokenhack function class

class tokenhackfunction {
	public:
	CheckBox checkbox_button;
	Button hotkey_button;
	StaticControl static_control;

	ToggleButton config_toggle_button;
	EditControl config_edit_control;
	StaticControl config_static_control;
	CustomComboBox config_custom_combobox;

	std::string name_in_file;

	DWORD hotkey_key;

	int on;

	UINT owner_name;
	UINT group_name;

	tokenhackfunction();
	tokenhackfunction(std::string static_text, std::string name_in_file, 
		
		
		
		std::vector<std::string> ccb_options = {{"On"}, {"Off"}});

	void set_on_state(int state);
};

extern std::vector<tokenhackfunction*> tokenhackfunctions;
extern std::vector<tokenhackfunction*> all_tokenhackfunctions;

#pragma endregion

#pragma region window creation declaration

extern TabButton TAB_BTN_ALL;

extern TabButton STAT_TAB_BTN_OUTPUT;
extern TabButton STAT_TAB_BTN_INPUT;

///////////////////////////////////////////

extern CheckBox CBTN_TOGGLEALL;

extern tokenhackfunction WMC;
extern tokenhackfunction CLOSE;
extern tokenhackfunction NOPICKUP;
extern tokenhackfunction PAUSE;
extern tokenhackfunction ROLL;
extern tokenhackfunction AUTOROLL;
extern tokenhackfunction READ;
extern tokenhackfunction MOVEITEM;
extern tokenhackfunction TEST;
extern tokenhackfunction TEST2;
extern tokenhackfunction TEST3;
extern tokenhackfunction RAINBOW;
extern tokenhackfunction TEST4;

///////////////////////////////////////////

extern void BTN_RELOAD_PROC(WPARAM wParam, LPARAM lParam);

extern Button BTN_CONFIG;
extern Button BTN_RELOAD;
extern Button BTN_LAUNCH;
extern Button BTN_LAUNCH_DIRECTORY_SET;
extern Button BTN_COW;
extern Button BTN_MUSHROOM;

///////////////////////////////////////////

extern CustomTrackbar CTB_VOLUME;
extern ToggleButton TBTN_RAINBOW;
extern ToggleButton TBTN_CONSOLE;
extern ToggleButton TBTN_SHOWONSTART;
extern ToggleButton TBTN_HOOKOUTPUT;

///////////////////////////////////////////

extern TabButton TAB_BTN_CONFIG_TAB_BTN_FUNCTIONS;
extern TabButton TAB_BTN_CONFIG_TAB_BTN_MISC;

extern EditControl console;

///////////////////////////////////////////

#pragma endregion

void InitNotifyIconData();
ATOM init_register_class(HINSTANCE hInstance);

void Minimize();
void Restore();

void set_volume(WORD volume, WORD left = 0, WORD right = 0);

void update_hotkeykeys();
void turn_all_functions_off();

void statbox_display(HWND hwnd, bool skiptostatsinfile = false);

std::string changeconfigonstate(std::string name_in_file, int new_val = 0, bool hex = false);
void setbuttonstate(HWND button_handle, int on_state);

#endif

