#ifndef window_header_guard
#define window_header_guard

#include <Windows.h>
#include <vector>
#include <string>
#include <Shellapi.h>

#include "Custom Trackbar.h"

#define VERSION L"TokenHack_v2.176"

#define WM_TRAYICON (WM_USER + 0)
#define WM_TEST (WM_USER + 1)
#define WM_TEST2 (WM_USER + 2)
#define WM_TEST3 (WM_USER + 3)

#define DESTROY_WINDOWS (WM_USER + 0)

#define CURSOR_IDLE (WM_USER + 1)
#define CURSOR_PRESS (WM_USER + 2)
#define CURSOR_LOADING (WM_USER + 3)

extern TCHAR className[];
extern UINT WM_TASKBARCREATED;

extern NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);
BOOL CALLBACK EnumChildProc(HWND, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK configProc(HWND, UINT, WPARAM, LPARAM);

extern HWND g_hwnd;
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
extern bool toggleall;
extern bool is_drawing_static;
extern DWORD global_change_hotkey;

extern std::vector<DWORD> hotkeys;

extern HBRUSH g_tempbrush;

extern bool move_window;
extern HWND lastwindow;

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

#pragma region basic_control + colorscheme

struct basic_control_colorscheme {
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

	basic_control_colorscheme();
	basic_control_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on
		);
	basic_control_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on
		//,COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on);
		);
	basic_control_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on,
		//COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on,
		COLORREF background_idle_off, COLORREF border_idle_off, COLORREF text_idle_off,
		COLORREF background_hover_off, COLORREF border_hover_off, COLORREF text_hover_off
		//COLORREF background_selected_off, COLORREF border_selected_off, COLORREF text_selected_off);
		);
};

class basic_control {
	public:
	HWND handle;
	std::string window_text;
	UINT window_styles;
	int xpos;
	int ypos;
	int width;
	int height;
	UINT id;
	basic_control_colorscheme color_scheme;
	HFONT font;
	std::string group_name;
	basic_control();
};


#pragma endregion

///////////////////

#pragma region tokenhack function class

class tokenhackfunction {
	public:
	HWND checkbox_handle;
	HWND hotkeybutton_handle;
	HWND static_handle;	
	
	std::string checkbox_text;
	std::string static_text;
	
	UINT checkbox_styles;	
	UINT hotkeybutton_styles;
	UINT static_styles;

	UINT checkbox_id;
	UINT hotkeybutton_id;

	std::string name_in_file;

	basic_control_colorscheme checkbox_scheme;
	basic_control_colorscheme hotkeybutton_scheme;
	basic_control_colorscheme static_scheme;

	HFONT checkbox_font;
	HFONT hotkeybutton_font;
	HFONT static_font;
	
	DWORD hotkey_key;
	
	int on;
	//gamble + skillbug?
	int temp_hotkey;

	std::string group_name;

	tokenhackfunction();
	tokenhackfunction(std::string static_text, std::string name_in_file,
		std::string group_name = "tokenhackfunction");
	tokenhackfunction(std::string static_text, std::string name_in_file, UINT checkbox_id, UINT hotkeybutton_id, 
		std::string group_name = "tokenhackfunction");
	tokenhackfunction(std::string checkbox_text, std::string static_text,  
		UINT checkbox_styles, UINT hotkeybutton_styles, UINT static_styles,
		UINT checkbox_id, UINT hotkeybutton_id, std::string name_in_file,
		basic_control_colorscheme checkbox_scheme, basic_control_colorscheme hotkeybutton_scheme, basic_control_colorscheme static_scheme,
		HFONT checkbox_font, HFONT hotkeybutton_font, HFONT static_font, 
		std::string group_name = "tokenhackfunction");
};

extern std::vector<tokenhackfunction*> tokenhackfunctions;
extern std::vector<tokenhackfunction*> all_tokenhackfunctions;

#pragma endregion

#pragma region tabbutton class

class tabbutton: public basic_control{
	public:
	int toggle_state;
	tabbutton();
	tabbutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, 
		basic_control_colorscheme color_scheme, HFONT font, int toggle_state, std::string group_name);
	
};

extern std::vector<tabbutton*> tabbuttons;

#pragma endregion

#pragma region autoradiobutton class

class autoradiobutton: public basic_control {
	public:
	int group;
	int toggle_state;
	autoradiobutton();
	autoradiobutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, int group, 
		basic_control_colorscheme color_scheme, HFONT font, int toggle_state, std::string group_name = "");
};

extern std::vector<autoradiobutton*> autoradiobuttons;

#pragma endregion

#pragma region checkboxbutton class

class checkboxbutton: public basic_control {
	public:
	int toggle_state;
	checkboxbutton();
	checkboxbutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, 
		basic_control_colorscheme color_scheme, HFONT font, int toggle_state, std::string group_name = "");
};

extern std::vector<checkboxbutton*> checkboxbuttons;

#pragma endregion

#pragma region normalbutton class

class normalbutton: public basic_control {
	public:
	normalbutton();
	normalbutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
		basic_control_colorscheme color_scheme, HFONT font, std::string group_name = "");
};

extern std::vector<normalbutton*> normalbuttons;

#pragma endregion

#pragma region togglebutton class

class togglebutton: public basic_control {
	public:
	int toggle_state;
	std::string string_in_file;
	togglebutton();
	togglebutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, 
		std::string string_in_file,
		basic_control_colorscheme color_scheme, HFONT font, int toggle_state, std::string group_name = "");
};

extern std::vector<togglebutton*> togglebuttons;

#pragma endregion

#pragma region editcontrol class

class editcontrol: public basic_control{	
	public:
	bool number_only;
	int min_number;
	int max_number;
	editcontrol();	
	editcontrol(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
		basic_control_colorscheme color_scheme, HFONT font, std::string group_name = "", 
		bool number_only = false, int min_number = INT_MIN, int max_number = INT_MAX);
};

extern std::vector<editcontrol*> editcontrols;

#pragma endregion

#pragma region staticcontrol class

class staticcontrol: public basic_control {
	public:
	staticcontrol();
	staticcontrol(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
		basic_control_colorscheme color_scheme, HFONT font, std::string group_name = "");
};

extern std::vector<staticcontrol*> staticcontrols;

#pragma endregion

#pragma region controlgroup class

class controlgroup {
	public:
	std::vector<autoradiobutton*> group_radiobuttons;
	std::vector<checkboxbutton*> group_checkboxbuttons;
	std::vector<normalbutton*> group_buttons;
	std::vector<togglebutton*> group_togglebuttons;
	std::vector<custom_trackbar*> group_customtrackbars;

	std::vector<staticcontrol*> group_staticcontrols;
	std::vector<editcontrol*> group_editcontrols;

	controlgroup();
	controlgroup(std::string group_name);

	void show_group(bool show);
};

extern std::vector<controlgroup> all_groups;

#pragma endregion

#pragma region window creation declaration

extern tabbutton TAB_BTN_ALL;

extern tabbutton STAT_TAB_BTN_OUTPUT;
//extern tabbutton STAT_TAB_BTN_INPUT;

///////////////////////////////////////////

extern checkboxbutton CBTN_TOGGLEALL;

extern tokenhackfunction WMC;
extern tokenhackfunction CLOSE;
extern tokenhackfunction NOPICKUP;
extern tokenhackfunction PAUSE;
extern tokenhackfunction ROLL;
extern tokenhackfunction AUTOROLL;
extern tokenhackfunction READ;
extern tokenhackfunction LAUNCH;
extern tokenhackfunction TEST;
extern tokenhackfunction TEST2;
extern tokenhackfunction TEST3;

///////////////////////////////////////////

extern normalbutton BTN_CONFIG;
extern normalbutton BTN_RELOAD;
extern normalbutton BTN_LAUNCH;
extern normalbutton BTN_LAUNCH_DIRECTORY_SET;
extern normalbutton BTN_COW;
extern normalbutton BTN_MUSHROOM;

///////////////////////////////////////////

extern custom_trackbar CTB_VOLUME;
extern togglebutton TBTN_RAINBOW;
extern togglebutton TBTN_CONSOLE;
extern togglebutton TBTN_SHOWONSTART;
extern togglebutton TBTN_HOOKOUTPUT;

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

