#ifndef window_header_guard
#define window_header_guard

#include <Windows.h>
#include <vector>
#include <string>

#include "Custom Trackbar.h"

#define VERSION L"TokenHack_v2.127"

#define WM_TEST (WM_USER)
#define WM_TRAYICON (WM_USER+1)
#define DESTROY_WINDOWS (WM_USER+2)

#define WM_PLAYPAUSE (WM_USER+30)
#define WM_PREV	(WM_USER+31)
#define WM_NEXT (WM_USER+32)
#define WM_VOLU (WM_USER+33)
#define WM_VOLD (WM_USER+34)

#define WM_UPDATE_WND (WM_USER + 40)

#define WM_VOICE (WM_USER+50)

#define CURSOR_IDLE (WM_USER + 1)
#define CURSOR_PRESS (WM_USER + 2)
#define CURSOR_LOADING (WM_USER + 3)

extern TCHAR className[];
extern UINT WM_TASKBARCREATED;

extern NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);
VOID CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD);
BOOL CALLBACK EnumChildProc(HWND, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK imageDlgProc(HWND, UINT, WPARAM, LPARAM);

extern HWND g_hwnd;
extern HHOOK kbhook;
extern HHOOK mhook;
extern HHOOK msghook;
extern HWND hworkout;
extern HWND icursong;
extern HWND statbox;
extern HWND image_display_box;
extern HWND image_input_edit;
extern HWND image_output_edit;
extern HWND image_input_static;
extern HWND image_output_static;
extern HCURSOR idle_cursor;
extern HCURSOR press_cursor;
extern HCURSOR loading_cursor;
extern HCURSOR token_cursor;
extern HFONT Font_a;
extern HFONT Font_itunes;
extern HFONT Font_workout;
extern HMENU g_menu;
extern MSG msg;
extern UINT timer1;
extern UINT timer2;
extern UINT timer3;
extern UINT timer4;

extern double starttime;
extern int click_toggle;
extern int click_toggle2;
extern int timer_on;

extern bool cursongvisstate;
extern bool is_hotkey_dialog_visible;
extern bool toggleall;
extern bool is_drawing_static;
extern DWORD global_change_hotkey;

extern std::vector<DWORD> hotkeys;

extern std::vector<std::string> workout;
extern UINT curworkout;
extern int totalsets;

extern bool move_window;
extern HWND lastwindow;

////

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

extern enum { statboxid = 2000/*, toggleallcb*/ };//other ids

#pragma region tokenhack functions struct

struct tokenhackfunction {
	HWND function_handle;
	HWND button_handle;
	HWND checkbox_handle;
	int checkbox_id;
	int button_id;
	DWORD hotkey_key;
	std::string name_in_file;
	std::string window_text;
	int on;
	//gamble + skillbug?
	int temp_hotkey;

	tokenhackfunction();
	tokenhackfunction(int abutton_id, int acheckbox_id, std::string aname_in_file, std::string awindow_text);
};

extern std::vector<tokenhackfunction*> tokenhackfunctions;
extern std::vector<tokenhackfunction*> all_tokenhackfunctions;

#pragma endregion

#pragma region imagetabbuttons struct

struct imagetabbutton {
	HWND handle;
	int id;
	std::string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	int row;
	int true_row;
	bool showstate;
	bool toggle_state;
	imagetabbutton(int aid, std::string awindow_text, int axpos, int aypos, int awidth, int aheight, int arow);
};

extern std::vector<imagetabbutton*> imagetabbuttons;

#pragma endregion

#pragma region functiontabbuttons struct

struct functiontabbutton {
	HWND handle;
	int id;
	std::string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	bool toggle_state;
	functiontabbutton();
	functiontabbutton(int id, std::string window_text, int xpos, int ypos, int width, int height, bool toggle_state);

};

extern std::vector<functiontabbutton*> functiontabbuttons;

#pragma endregion

#pragma region statboxtabbuttons struct

struct statboxtabbutton {
	HWND handle;
	int id;
	std::string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	bool toggle_state;
	statboxtabbutton();
	statboxtabbutton(int id, std::string window_text, int xpos, int ypos, int width, int height, bool toggle_state);
};

extern std::vector<statboxtabbutton*> statboxtabbuttons;

#pragma endregion

//

#pragma region ownerdrawn autoradiobuttons struct

struct autoradiobutton {
	HWND handle;
	int id;
	int xpos;
	int ypos;
	int width;
	int height;
	int group;
	std::string window_text;
	bool show_state;
	bool toggle_state;
	COLORREF color;
	std::string group_name;
	bool font;
	autoradiobutton();
	autoradiobutton(int id, std::string window_text, int xpos, int ypos, int width, int height, int group, COLORREF color, bool show_state, bool font = false, std::string group_name = "");
};

extern std::vector<autoradiobutton*> autoradiobuttons;

#pragma endregion

#pragma region ownerdrawn checkboxbuttons struct

struct checkboxbutton {
	HWND handle;
	int id;
	int xpos;
	int ypos;
	int width;
	int height;
	std::string window_text;
	bool show_state;
	bool toggle_state;
	COLORREF color;
	std::string group_name;
	bool font;
	checkboxbutton();
	checkboxbutton(int id, std::string window_text, int xpos, int ypos, int width, int height, COLORREF color, bool show_state, bool font = false, std::string group_name = "");
};

extern std::vector<checkboxbutton*> checkboxbuttons;

#pragma endregion

#pragma region normalbuttons struct

struct normalbutton {
	HWND handle;
	int id;
	std::string window_text;
	int xpos;
	int ypos;
	int* yoffset;
	int width;
	int height;
	COLORREF color;
	bool show_state;
	std::string group_name;
	bool font;
	normalbutton();
	normalbutton(int aid, std::string awindow_text, int axpos, int* ayoffset, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool font = false, std::string group_name = "");
};

extern std::vector<normalbutton*> normalbuttons;

#pragma endregion

#pragma region togglebutton struct

struct togglebutton {
	HWND handle;
	int id;
	std::string window_text;
	std::string string_in_file;
	int xpos;
	int ypos;
	int* yoffset;
	int width;
	int height;
	int toggle_state;
	bool show_state;
	std::string group_name;
	bool font;
	togglebutton();
	togglebutton(int id, std::string window_text, std::string string_in_file, int xpos, int* yoffset, int ypos, int width, int height, int toggle_state, bool font = false, std::string group_name = "");
};

extern std::vector<togglebutton*> togglebuttons;

#pragma endregion

#pragma region editcontrol struct

struct editcontrol {
	HWND handle;
	int id;
	std::string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	COLORREF color;
	bool show_state;
	bool number_only;
	int min_number;
	int max_number;
	std::string group_name;
	bool font;
	editcontrol();
	editcontrol(int aid, std::string awindow_text, int axpos, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool number_only = false, int min_number = INT_MIN, int max_number = INT_MAX, bool font = false, std::string group_name = "");
};

extern std::vector<editcontrol*> editcontrols;

#pragma endregion

#pragma region staticcontrol struct

struct staticcontrol {
	HWND handle;
	int id;
	std::string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	COLORREF color;
	bool show_state;
	std::string group_name;
	bool font;
	staticcontrol();
	staticcontrol(int aid, std::string awindow_text, int axpos, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool font = false, std::string group_name = "");
};

extern std::vector<staticcontrol*> staticcontrols;

#pragma endregion

#pragma region controlgroup

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


#pragma region window declarations


extern functiontabbutton FUNC_TAB_BTN_ALL;
extern functiontabbutton FUNC_TAB_BTN_D2;
extern functiontabbutton FUNC_TAB_BTN_ITUNES;
extern functiontabbutton FUNC_TAB_BTN_IMAGE;

///////////////////////////////////////////

extern checkboxbutton CHK_BTN_TOGGLEALL;

extern tokenhackfunction WMC;
extern tokenhackfunction CLOSE;
extern tokenhackfunction GAMBLE;
extern tokenhackfunction GOLD;
extern tokenhackfunction WSGCAST;
extern tokenhackfunction SKILLBUG;
extern tokenhackfunction NOPICKUP;
extern tokenhackfunction LEFTCLICK;
extern tokenhackfunction RIGHTCLICK;
extern tokenhackfunction PAUSE;
extern tokenhackfunction ROLL;
extern tokenhackfunction DROP;
extern tokenhackfunction WSG;
extern tokenhackfunction AUTOROLL;
extern tokenhackfunction READ;
extern tokenhackfunction SKILLBUGSET;
extern tokenhackfunction TEXTSPAM;
extern tokenhackfunction LAUNCH;
extern tokenhackfunction CHARMROLL;
extern tokenhackfunction IPLAY;
extern tokenhackfunction IPREV;
extern tokenhackfunction INEXT;
extern tokenhackfunction IVOLU;
extern tokenhackfunction IVOLD;
extern tokenhackfunction TIMER;
extern tokenhackfunction TEST;
extern tokenhackfunction SELL;
extern tokenhackfunction DUPE;
extern tokenhackfunction BARBWW;
extern tokenhackfunction BARBLEAP;

extern normalbutton BTN_CONFIG;
extern normalbutton BTN_RELOAD;
extern normalbutton BTN_LAUNCH;
extern normalbutton BTN_COW;
extern normalbutton BTN_MUSHROOM;


extern togglebutton TBTN_SOUND;
extern togglebutton TBTN_RAINBOW;
extern togglebutton TBTN_CONSOLE;
extern togglebutton TBTN_SHOWONSTART;
extern togglebutton TBTN_HOOKOUTPUT;

/////////////////////////////////////////////

extern statboxtabbutton STAT_TAB_BTN_OUTPUT;
extern statboxtabbutton STAT_TAB_BTN_INPUT;

extern imagetabbutton IMG_TAB_BTN_CROP;
extern imagetabbutton IMG_TAB_BTN_GROW;
extern imagetabbutton IMG_TAB_BTN_GRAY;
extern imagetabbutton IMG_TAB_BTN_BRIGHTNESS;
extern imagetabbutton IMG_TAB_BTN_CONTRAST;
extern imagetabbutton IMG_TAB_BTN_INVERT;

extern imagetabbutton IMG_TAB_BTN_CONVERT;
extern imagetabbutton IMG_TAB_BTN_COMPARE;
extern imagetabbutton IMG_TAB_BTN_ANIMATE;

extern imagetabbutton IMG_BTN_INPUT;
extern imagetabbutton IMG_BTN_OUTPUT;

//////////////////////////////////////////////

extern normalbutton BTN_IMAGE_PROCESS;
//clear btn
//out btn
#pragma region crop tab
extern staticcontrol STATIC_CROP_LEFT;
extern staticcontrol STATIC_CROP_RIGHT;
extern staticcontrol STATIC_CROP_TOP;
extern staticcontrol STATIC_CROP_BOTTOM;
extern editcontrol EDIT_CROP_LEFT;
extern editcontrol EDIT_CROP_RIGHT;
extern editcontrol EDIT_CROP_TOP;
extern editcontrol EDIT_CROP_BOTTOM;
extern controlgroup GROUP_CROP;
#pragma endregion
#pragma region grow tab
extern staticcontrol STATIC_GROW_LEFT;
extern staticcontrol STATIC_GROW_RIGHT;
extern staticcontrol STATIC_GROW_TOP;
extern staticcontrol STATIC_GROW_BOTTOM;
extern editcontrol EDIT_GROW_LEFT;
extern editcontrol EDIT_GROW_RIGHT;
extern editcontrol EDIT_GROW_TOP;
extern editcontrol EDIT_GROW_BOTTOM;
extern controlgroup GROUP_GROW;
#pragma endregion
#pragma region gray tab
extern autoradiobutton RBTN_GRAY_AVGERAGE;
extern autoradiobutton RBTN_GRAY_LUMA;
extern autoradiobutton RBTN_GRAY_DESATURATE;
extern autoradiobutton RBTN_GRAY_DECOMP_MIN;
extern autoradiobutton RBTN_GRAY_DECOMP_MAX;
extern autoradiobutton RBTN_GRAY_COLOR_CHANNEL_RED;
extern autoradiobutton RBTN_GRAY_COLOR_CHANNEL_GREEN;
extern autoradiobutton RBTN_GRAY_COLOR_CHANNEL_BLUE;
extern staticcontrol STATIC_GRAY_DECOMPOSITION;
extern staticcontrol STATIC_GRAY_COLOR_CHANNEL;
extern staticcontrol STATIC_GRAY_COLORS;
extern staticcontrol STATIC_GRAY_DITHER;
extern editcontrol EDIT_GRAY_COLORS;
extern checkboxbutton CHBOX_GRAY_DITHER;
extern controlgroup GROUP_GRAY;
#pragma endregion
#pragma region brightness tab
extern custom_trackbar TRACKBAR_BRIGHTNESS_SET;
extern autoradiobutton RBTN_BRIGHTNESS_HSV;
extern autoradiobutton RBTN_BRIGHTNESS_FLAT;
extern controlgroup GROUP_BRIGHTNESS;
#pragma endregion
#pragma region contrast tab
extern controlgroup GROUP_CONTRAST;
#pragma endregion
#pragma region convert tab
extern controlgroup GROUP_CONVERT;
#pragma endregion

#pragma endregion


void InitNotifyIconData();
ATOM init_register_class(HINSTANCE hInstance);

void Minimize();
void Restore();

void update_hotkeykeys();
void turn_all_functions_off();

void changeconfigonstate(std::string name_in_file);
void setbuttonstate(HWND hwnd, HWND button_handle, int checkbox_id, int on_state);


#endif

