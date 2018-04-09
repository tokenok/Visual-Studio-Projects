#include <fstream>
#include <algorithm>
#include <time.h>

#include "window.h"
#include "resource.h"
#include "common.h"
#include "ocr.h"
#include "d2funcs.h"

using namespace std;

TCHAR className[] = TEXT("Tokenhack");
UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

NOTIFYICONDATA g_notifyIconData;

HWND g_hwnd;
HHOOK kbhook;
HHOOK mhook;
HWND statbox;
HCURSOR idle_cursor;
HCURSOR press_cursor;
HCURSOR loading_cursor;
HCURSOR token_cursor;
HMENU g_menu;
MSG msg;
bool is_hotkey_dialog_visible = false;
bool toggleall = false;
bool is_drawing_static = false;//mutex emulation?
DWORD global_change_hotkey;

vector<DWORD> hotkeys;

HBRUSH g_tempbrush = NULL;

bool move_window = false; //right click on control then use arrow keys to move, shift makes it move faster, ctrl + arrow keys resize the window
HWND lastwindow = NULL;

#pragma region color schemes, fonts and styles

basic_control_colorscheme tokenhackfunctionhotkey_scheme(
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
	//															//SELECTED ON: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10)			//HOVER OFF: background/border/text
	//															//SELECTED OFF: background/border/text
	);
basic_control_colorscheme tokenhackfunctioncheckbox_scheme(
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
	RGB(100, 100, 100), RGB(254, 33, 61), RGB(100, 100, 100)	//HOVER OFF: background/border/text
	);
basic_control_colorscheme tokenhackfunctionstatic_scheme(
	RGB(10, 10, 10), RGB(10, 10, 10), RGB(148, 0, 211)
	);
basic_control_colorscheme tokenhackfunctiontab_scheme(
	RGB(100, 100, 100), RGB(100, 100, 100), RGB(10, 10, 10),	//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(100, 100, 100), RGB(10, 10, 10),	//HOVER ON: background/border/text
	//															//SELECTED ON: background/border/text
	RGB(10, 10, 10), RGB(100, 100, 100), RGB(100, 100, 100),	//IDLE OFF: background/border/text
	RGB(100, 100, 100), RGB(10, 10, 10), RGB(10, 10, 10)		//HOVER OFF: background/border/text
	//															//SELECTED OFF: background/border/text
	);
basic_control_colorscheme togglebutton_scheme(
	RGB(10, 10, 10), RGB(0, 255, 0), RGB(0, 255, 0),			//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(0, 255, 0), RGB(0, 255, 0),			//HOVER ON: background/border/text
	RGB(10, 10, 10), RGB(255, 0, 0), RGB(255, 0, 0),			//IDLE OFF: background/border/text
	RGB(100, 100, 100), RGB(255, 0, 0), RGB(255, 0, 0)			//HOVER OFF: background/border/text
	);
basic_control_colorscheme normalbutton_scheme(
	RGB(10, 10, 10), RGB(0, 117, 255), RGB(0, 117, 255), 		//IDLE: background/border/text
	RGB(100, 100, 100), RGB(0, 117, 255), RGB(0, 117, 255)		//HOVER: background/border/text	
	);
basic_control_colorscheme stattab_scheme(
	RGB(0, 118, 255), RGB(0, 118, 255), RGB(10, 10, 10),		//IDLE ON: background/border/text
	RGB(0, 118, 255), RGB(0, 118, 255), RGB(10, 10, 10),		//HOVER ON: background/border/text
	RGB(10, 10, 10), RGB(0, 118, 255), RGB(0, 118, 255),		//IDLE OFF: background/border/text
	RGB(0, 118, 255), RGB(10, 10, 10), RGB(10, 10, 10)			//HOVER OFF: background/border/text
	);

HFONT Font_a = NULL;

LONG button_styles = WS_VISIBLE | WS_CHILD | BS_OWNERDRAW;

int window_id = 1000;
int cb_id = 1500;

#pragma endregion

#pragma region tokenhack function window position and size info

int totalonfunctions = 0;

int tokenhackfunctionwindowxposoffset = 5;

int windowwidth = 400;
int functionnameyoffsum = 0;

int functionnameheight = 16;
int functionnamewidth = 180;

int tabbuttonheight = functionnameheight;

int checkboxwidth = 15;
int checkboxheight = 15;

int buttonwidth = 150;
int buttonheight = functionnameheight;

int checkboxxoff = 0 + tokenhackfunctionwindowxposoffset;
int buttonxoff = checkboxwidth + tokenhackfunctionwindowxposoffset + 1;
int functionnamexoff = 180 + tokenhackfunctionwindowxposoffset;
int statboxxoff = 370 + tokenhackfunctionwindowxposoffset;

int bottomheight = 135;

#pragma endregion

#pragma region basic_control + colorscheme implementation

basic_control::basic_control() {};

basic_control_colorscheme::basic_control_colorscheme() {};

//statics
basic_control_colorscheme::basic_control_colorscheme(
	COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on) {
	this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
}

//buttons etc
basic_control_colorscheme::basic_control_colorscheme(
	COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
	COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on)
	//,COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on);
{

	this->background_idle_on = background_idle_on; this->background_hover_on = background_hover_on; //this->background_selected_on = background_selected_on;
	this->border_idle_on = border_idle_on; this->border_hover_on = border_hover_on; //this->border_selected_on = border_selected_on;
	this->text_idle_on = text_idle_on; this->text_hover_on = text_hover_on; //this->text_selected_on = text_selected_on;
}

//buttons with toggle state
basic_control_colorscheme::basic_control_colorscheme(
	COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
	COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on,
	//COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on,
	COLORREF background_idle_off, COLORREF border_idle_off, COLORREF text_idle_off,
	COLORREF background_hover_off, COLORREF border_hover_off, COLORREF text_hover_off)
	//COLORREF background_selected_off, COLORREF border_selected_off, COLORREF text_selected_off);
{

	this->background_idle_on = background_idle_on; this->background_hover_on = background_hover_on; //this->background_selected_on = background_selected_on;
	this->background_idle_off = background_idle_off; this->background_hover_off = background_hover_off; //this->background_selected_off = background_selected_off;
	this->border_idle_on = border_idle_on; this->border_hover_on = border_hover_on; //this->border_selected_on = border_selected_on;
	this->border_idle_off = border_idle_off; this->border_hover_off = border_hover_off; //this->border_selected_off = border_selected_off;
	this->text_idle_on = text_idle_on; this->text_hover_on = text_hover_on; //this->text_selected_on = text_selected_on;
	this->text_idle_off = text_idle_off; this->text_hover_off = text_hover_off;// this->text_selected_off = text_selected_off;
}

#pragma endregion

///////////

#pragma region tokenhack function class

vector<tokenhackfunction*> tokenhackfunctions;
vector<tokenhackfunction*> all_tokenhackfunctions;

tokenhackfunction::tokenhackfunction() {}
tokenhackfunction::tokenhackfunction(std::string static_text, std::string name_in_file,
	std::string group_name/* = "tokenhackfunction"*/) {
	checkbox_handle = NULL; hotkeybutton_handle = NULL; static_handle = NULL; hotkey_key = 0; temp_hotkey = 0; on = 0;
	this->checkbox_text = "T"; this->static_text = static_text;
	this->checkbox_styles = button_styles; this->hotkeybutton_styles = button_styles; this->static_styles = WS_CHILD | WS_VISIBLE;
	this->checkbox_id = cb_id++; this->hotkeybutton_id = window_id++; this->name_in_file = name_in_file;
	this->checkbox_scheme = tokenhackfunctioncheckbox_scheme; this->hotkeybutton_scheme = tokenhackfunctionhotkey_scheme; this->static_scheme = tokenhackfunctionstatic_scheme;
	this->checkbox_font = Font_a; this->hotkeybutton_font = Font_a; this->static_font = Font_a;
	this->group_name = group_name;

	tokenhackfunctions.push_back(this);
	all_tokenhackfunctions.push_back(this);
}
tokenhackfunction::tokenhackfunction(std::string static_text, std::string name_in_file, UINT checkbox_id, UINT hotkeybutton_id, 
	std::string group_name/* = "tokenhackfunction"*/) {
	checkbox_handle = NULL; hotkeybutton_handle = NULL; static_handle = NULL; hotkey_key = 0; temp_hotkey = 0; on = 0;
	this->checkbox_text = "T"; this->static_text = static_text;
	this->checkbox_styles = button_styles; this->hotkeybutton_styles = button_styles; this->static_styles = WS_CHILD | WS_VISIBLE;
	this->checkbox_id = checkbox_id; this->hotkeybutton_id = hotkeybutton_id; this->name_in_file = name_in_file;
	this->checkbox_scheme = tokenhackfunctioncheckbox_scheme; this->hotkeybutton_scheme = tokenhackfunctionhotkey_scheme; this->static_scheme = tokenhackfunctionstatic_scheme;
	this->checkbox_font = Font_a; this->hotkeybutton_font = Font_a; this->static_font = Font_a;
	this->group_name = group_name;

	tokenhackfunctions.push_back(this);
	all_tokenhackfunctions.push_back(this);
}
tokenhackfunction::tokenhackfunction(std::string checkbox_text, std::string static_text,
	UINT checkbox_styles, UINT hotkeybutton_styles, UINT static_styles,
	UINT checkbox_id, UINT hotkeybutton_id, std::string name_in_file,
	basic_control_colorscheme checkbox_scheme, basic_control_colorscheme hotkeybutton_scheme, basic_control_colorscheme static_scheme,
	HFONT checkbox_font, HFONT hotkeybutton_font, HFONT static_font, 
	std::string group_name/* = "tokenhackfunction"*/) {
	
	checkbox_handle = NULL; hotkeybutton_handle = NULL; static_handle = NULL; hotkey_key = 0; temp_hotkey = 0; on = 0;
	this->checkbox_text = checkbox_text; this->static_text = static_text;
	this->checkbox_styles = checkbox_styles; this->hotkeybutton_styles = hotkeybutton_styles; this->static_styles = static_styles;
	this->checkbox_id = checkbox_id; this->hotkeybutton_id = hotkeybutton_id; this->name_in_file = name_in_file;
	this->checkbox_scheme = checkbox_scheme; this->hotkeybutton_scheme = hotkeybutton_scheme; this->static_scheme = static_scheme;
	this->checkbox_font = checkbox_font; this->hotkeybutton_font = hotkeybutton_font; this->static_font = static_font;
	this->group_name = group_name;

	tokenhackfunctions.push_back(this);
	all_tokenhackfunctions.push_back(this);
}

#pragma endregion

#pragma region functiontabbutton class

vector<tabbutton*> tabbuttons;

tabbutton::tabbutton() {}
tabbutton::tabbutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
	basic_control_colorscheme color_scheme, HFONT font, int toggle_state, string group_name/* = ""*/) {
	handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->color_scheme = color_scheme; this->font = font; this->toggle_state = toggle_state; this->group_name = group_name;
	
	tabbuttons.push_back(this);
}

#pragma endregion

#pragma region autoradiobutton class

vector<autoradiobutton*> autoradiobuttons;

autoradiobutton::autoradiobutton() {}
autoradiobutton::autoradiobutton(string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, int group, 
	basic_control_colorscheme color_scheme, HFONT font, int toggle_state, string group_name/* = ""*/) {
	handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; 
	this->group = group;
	this->color_scheme = color_scheme; this->font = font; this->toggle_state = toggle_state; this->group_name = group_name;

	autoradiobuttons.push_back(this);
}

#pragma endregion

#pragma region checkboxbutton class

vector<checkboxbutton*> checkboxbuttons;

checkboxbutton::checkboxbutton() {}
checkboxbutton::checkboxbutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
	basic_control_colorscheme color_scheme, HFONT font, int toggle_state, std::string group_name/* = ""*/) {
	handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->color_scheme = color_scheme; this->font = font; this->toggle_state = toggle_state; this->group_name = group_name;

	checkboxbuttons.push_back(this);
}

#pragma endregion

#pragma region normalbutton class

vector<normalbutton*> normalbuttons;

normalbutton::normalbutton() {}
normalbutton::normalbutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
	basic_control_colorscheme color_scheme, HFONT font, std::string group_name/* = ""*/) {
	handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->color_scheme = color_scheme; this->font = font; this->group_name = group_name;

	normalbuttons.push_back(this);
}

#pragma endregion

#pragma region togglebutton class

vector<togglebutton*> togglebuttons;

togglebutton::togglebutton() {}
togglebutton::togglebutton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, 
	std::string string_in_file,
	basic_control_colorscheme color_scheme, HFONT font, int toggle_state, std::string group_name/* = ""*/) {

	handle = NULL;
	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->string_in_file = string_in_file;
	this->color_scheme = color_scheme; this->font = font; this->toggle_state = toggle_state; this->group_name = group_name;

	togglebuttons.push_back(this);
}

#pragma endregion

#pragma region editcontrol class

vector<editcontrol*> editcontrols;

editcontrol::editcontrol() {}
editcontrol::editcontrol(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
	basic_control_colorscheme color_scheme, HFONT font, std::string group_name/* = ""*/,
	bool number_only/* = false*/, int min_number/* = INT_MIN*/, int max_number/* = INT_MAX*/){

	handle = NULL;
	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->color_scheme = color_scheme; this->font = font; this->group_name = group_name;
	this->number_only = number_only; this->min_number = min_number; this->max_number = max_number;

	editcontrols.push_back(this);
}

#pragma endregion

#pragma region staticcontrol class

vector<staticcontrol*> staticcontrols;

staticcontrol::staticcontrol() {}
staticcontrol::staticcontrol(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, 
	basic_control_colorscheme color_scheme, HFONT font, std::string group_name/* = ""*/) {

	handle = NULL;
	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->color_scheme = color_scheme; this->font = font; this->group_name = group_name;

	staticcontrols.push_back(this);
}

#pragma endregion

#pragma region controlgroup class

vector<controlgroup> all_groups;

controlgroup::controlgroup():
	group_radiobuttons(NULL), group_checkboxbuttons(NULL), group_buttons(NULL), group_togglebuttons(NULL), group_staticcontrols(NULL), group_editcontrols(NULL), group_customtrackbars(NULL) {}
controlgroup::controlgroup(string group_name) :
	group_radiobuttons(NULL), group_checkboxbuttons(NULL), group_buttons(NULL), group_togglebuttons(NULL), group_staticcontrols(NULL), group_editcontrols(NULL) {
	for (auto & a: autoradiobuttons) {
		if (group_name == a->group_name)
			group_radiobuttons.push_back(a);
	}
	for (auto & a: checkboxbuttons) {
		if (group_name == a->group_name)
			group_checkboxbuttons.push_back(a);
	}
	for (auto & a: normalbuttons) {
		if (group_name == a->group_name)
			group_buttons.push_back(a);
	}
	for (auto & a: togglebuttons) {
		if (group_name == a->group_name)
			group_togglebuttons.push_back(a);
	}
	for (auto & a: editcontrols) {
		if (group_name == a->group_name)
			group_editcontrols.push_back(a);
	}
	for (auto & a: staticcontrols) {
		if (group_name == a->group_name)
			group_staticcontrols.push_back(a);
	}
	for (auto & a: custom_trackbars) {
		if (group_name == a->group_name)
			group_customtrackbars.push_back(a);
	}
	all_groups.push_back(*this);
}

void controlgroup::show_group(bool show) {
	show = true;//remove warning
}

#pragma endregion

#pragma region window creation

tabbutton TAB_BTN_ALL("D2", button_styles, 15, 0, 45, tabbuttonheight, window_id++, tokenhackfunctiontab_scheme, Font_a, 0, "function tab button");

tabbutton STAT_TAB_BTN_OUTPUT("Output", button_styles, statboxxoff, 0, 60, tabbuttonheight, window_id++, stattab_scheme, Font_a, 1, "stat tab button");
//tabbutton STAT_TAB_BTN_INPUT("Input", button_styles, statboxxoff + 60, 0, 60, tabbuttonheight, window_id++, stattab_scheme, Font_a, 0, "stat tab button");

//////////////////////////////////////////////////////////////////////////////////////

checkboxbutton CBTN_TOGGLEALL("T", button_styles, 0, 0, 14, 14, window_id++, {
	RGB(10, 10, 10), RGB(255, 206, 61), RGB(255, 206, 61),		//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(255, 206, 61), RGB(255, 206, 61),		//HOVER ON: background/border/text
	RGB(10, 10, 10), RGB(255, 206, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
	RGB(100, 100, 100), RGB(255, 206, 61), RGB(100, 100, 100)	//HOVER OFF: background/border/text
}, Font_a, 0);

tokenhackfunction WMC("Toggle Cursor Lock", "wmc");
tokenhackfunction CLOSE("Close", "close");
tokenhackfunction NOPICKUP("\"/nopickup\"", "\"/nopickup\"");
tokenhackfunction PAUSE("Pause/Resume", "pause");
tokenhackfunction ROLL("Transmute", "transmute");
tokenhackfunction AUTOROLL("Auto Roll (OCR)", "auto_roll");
tokenhackfunction READ("Read Stats (OCR)", "read_stats");
tokenhackfunction LAUNCH("Launch", "launch");
tokenhackfunction TEST("test", "test");
tokenhackfunction TEST2("test2", "test2");
tokenhackfunction TEST3("test3", "test3");

///////////////////////////////////////////////////////////////////////////////////////
int seed_token_func_rand() {
	srand((UINT)time(NULL));
	return 0;
}
string token_func_rand_text() {
	string ret = "";
	for (int i = 0; i < 17; i++) {
		int randd = rand() % 1000;
		if (randd % 2)
			ret += (char)rand() % 2 + 88;
		else
			ret += (char)8;
	}
	return ret;
}

int xasdflkjasdlksaldasdkjflkasjdflkajsdfl = seed_token_func_rand();
tokenhackfunction DONOTHINGFILLER1(token_func_rand_text(), "i", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER2(token_func_rand_text(), "put", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER3(token_func_rand_text(), "these", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER4(token_func_rand_text(), "here", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER5(token_func_rand_text(), "so", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER6(token_func_rand_text(), "\"rainbow\"", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER7(token_func_rand_text(), "looks", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER8(token_func_rand_text(), "much", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER9(token_func_rand_text(), "better", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER10(token_func_rand_text(), "than", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER11(token_func_rand_text(), "it", cb_id, window_id++, "skip");;
tokenhackfunction DONOTHINGFILLER12(token_func_rand_text(), "does", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER13(token_func_rand_text(), "with", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER14(token_func_rand_text(), "fewer", cb_id, window_id++, "skip");
tokenhackfunction DONOTHINGFILLER15(token_func_rand_text(), "things", cb_id++, window_id++, "skip");

//////////////////////////////////////////////////////////////////////////////////////

normalbutton BTN_CONFIG("Open Config", button_styles, 14, 35, 100, 30, window_id++, normalbutton_scheme, Font_a, "tokenhackoffset");
normalbutton BTN_RELOAD("Reload", button_styles, 140, 35, 80, 30, window_id++, normalbutton_scheme, Font_a, "tokenhackoffset");
normalbutton BTN_LAUNCH("Launch", button_styles, 246, 35, 100, 30, window_id++, normalbutton_scheme, Font_a, "tokenhackoffset");
normalbutton BTN_LAUNCH_DIRECTORY_SET("...", button_styles, 345, 35, 17, 30, window_id++, normalbutton_scheme, Font_a, "tokenhackoffset");
normalbutton BTN_COW("C", WS_CHILD | BS_OWNERDRAW, 113, 35, 28, 30, window_id++, normalbutton_scheme, Font_a, "tokenhackoffset");
normalbutton BTN_MUSHROOM("M", WS_CHILD | BS_OWNERDRAW, 219, 35, 28, 30, window_id++, normalbutton_scheme, Font_a, "tokenhackoffset");

//////////////////////////////////////////////////////////////////////////////////////

custom_trackbar CTB_VOLUME("volume", WS_VISIBLE | WS_CHILD, 16, 70, 68, 14, window_id++,
	0x0000, 0xFFFF, 0x4444, 0x0CCF, 0x199A,
	0, CTB_DRAGGING_VALUE, CTB_DRAGGING_SMOOTH_STAY, {
	RGB(10, 10, 10), RGB(50, 50, 50), RGB(80, 80, 80), RGB(127, 127, 127),							// background/border/window name idle/highlight
	RGB(0, 255, 0), RGB(10, 10, 10), RGB(0, 255, 0), RGB(10, 10, 10),								// left/right channel left/right highlight
	RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127), 									// thumb background: idle/hover/selected
	RGB(10, 10, 10), RGB(10, 10, 10), RGB(10, 10, 10),			 									// thumb border: idle/hover/selected
	RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127)										// thumb text: idle/hover/selected
},
CTB_SHOW_FOCUS_RECT, 0,
LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESSCURSOR)), LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESSCURSOR)),
"tokenhackoffset"
);
togglebutton TBTN_RAINBOW("rainbow", button_styles, 91, 70, 72, 14, window_id++, "rainbow", togglebutton_scheme, Font_a, false, "tokenhackoffset");
togglebutton TBTN_CONSOLE("console", button_styles, 170, 70, 64, 14, window_id++, "console", togglebutton_scheme, Font_a, false, "tokenhackoffset");
togglebutton TBTN_HOOKOUTPUT("kb", button_styles, 240, 70, 17, 14, window_id++, "keyboardhookoutput", togglebutton_scheme, Font_a, false, "tokenhackoffset");
togglebutton TBTN_SHOWONSTART("show on start", button_styles, 263, 70, 98, 14, window_id++, "showtokenhackonstart", togglebutton_scheme, Font_a, false, "tokenhackoffset");

#pragma endregion

void InitNotifyIconData() {
	memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hwnd;
	g_notifyIconData.uID = 5000;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENICO), IMAGE_ICON, 0, 0, LR_SHARED);
	g_notifyIconData.uTimeout = 0;
	g_notifyIconData.uVersion = NOTIFYICON_VERSION;
	memcpy(g_notifyIconData.szInfoTitle, VERSION, 64);
	memcpy(g_notifyIconData.szInfo, TEXT("Click icon to see menu"), 256);
	memcpy(g_notifyIconData.szTip, VERSION, 128);
	g_notifyIconData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
}
ATOM init_register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = {0};
	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof (WNDCLASSEX);
	wnd.hIconSm = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(TOKENICO));
//	wnd.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
	//wnd.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
	//else wnd.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;	
	//wnd.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(TOKENBACK)));
	return RegisterClassEx(&wnd);
}

void Minimize() {
	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore() {
	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
	SetWindowPos(g_hwnd, HWND_TOP, 0, 0,
		(!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250/*180*/,
		functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset,
		SWP_NOMOVE);
	SetForegroundWindow(g_hwnd);
}

void set_volume(WORD volume, WORD left /*= 0*/, WORD right /*= 0*/) {
	if (!left)
		left = volume;
	if (!right)
		right = volume;
	waveOutSetVolume(0, MAKELPARAM(left, right));
}

void update_hotkeykeys() {
	hotkeys.erase(hotkeys.begin(), hotkeys.end());
	for (auto & a: tokenhackfunctions)
		if (a->on && a->group_name == "tokenhackfunction")
			hotkeys.push_back(a->hotkey_key);
}
void turn_all_functions_off() {
	for (auto & a: tokenhackfunctions)
		a->on = 0;
	totalonfunctions = 0;
}

void statbox_display(HWND hwnd, bool skiptostatsinfile /* = false*/) {
	if (READ.on || AUTOROLL.on) {
		if (!skiptostatsinfile) {
			statbox = CreateWindow(TEXT("EDIT"), TEXT(""),
				WS_CHILD | WS_VISIBLE | WS_BORDER /*| ES_WANTRETURN*/ | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_READONLY,
				0, 0,
				0, 0,
				hwnd, (HMENU)statboxid, NULL, NULL);
			for (auto & a: tabbuttons) {
				if (a->group_name == "stat tab button") {
					a->handle = CreateWindow(TEXT("BUTTON"), (LPCWSTR)str_to_wstr(a->window_text).c_str(),
						WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
						a->xpos, a->ypos,
						a->width, a->height,
						hwnd, (HMENU)a->id, NULL, NULL);
				}
			}
		}
		if (AUTOROLL.on) {
			load_stats_and_colors_from_file();
			string searching = "Stats in file:\r\n";
			for (auto & a: stats_in_file)
				searching += '>' + a + "\r\n";
			searching += '\0';
			SendMessage(statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());
		}
	}
}

string changeconfigonstate(string name_in_file, int new_val /*= 0*/, bool hex /*= false*/) {
	if (name_in_file.size() == 0)
		return "";
	string ret = "";
	vector<string> text;
	string line;
	ifstream filein;
	filein.open("config.txt");
	if (!filein) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	while (getline(filein, line))
		text.push_back(line);
	filein.close();
	for (UINT i = 0; i < text.size(); i++) {
		line = text[i];
		line = line.substr(0, line.find('*'));
		line.erase(remove(line.begin(), line.end(), '\t'), line.end());
		line.erase(remove(line.begin(), line.end(), ' '), line.end());
		transform(line.begin(), line.end(), line.begin(), ::tolower);
		string var_name = line.substr(0, line.find(':'));
		var_name = var_name.substr(0, var_name.find('['));
		var_name = var_name.substr(0, var_name.find('('));
		string val = line.substr(line.find(':') + 1, line.size());
		string prev = (line.find('[') != string::npos) ? line.substr(line.find('[') + 1, line.find(']') - line.find('[') - 1) : "";
		if ((line.find('[') != string::npos && prev.size() == 0) || (prev == int_to_str(new_val) && new_val == 0))
			prev = "1";
		ret = prev;
		if (var_name.size() == 0)
			continue;
		if (var_name == name_in_file) {
			text[i] = text[i].erase(text[i].find(':') + 1, text[i].find('*') - text[i].find(':') - 1);
			text[i].insert(text[i].find(':') + 1, "\t" + ((val != "0" ? int_to_str(new_val, hex), ret = int_to_str(new_val, hex) : (prev.size() > 0 ? prev : "1")) + "\t"));
			if (prev.size() > 0) {
				text[i] = text[i].erase(text[i].find('[') + 1, text[i].find(']') - text[i].find('[') - 1);
				text[i].insert(text[i].find('[') + 1, val);
			}
			break;
		}
	}
	ofstream fileout;
	fileout.open("config.txt");
	if (!fileout) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	for (UINT i = 0; i < text.size(); i++)
		fileout << text[i] << '\n';
	fileout.close();

	return ret;
}
void setbuttonstate(HWND button_handle, int on_state) {
	if (!on_state) {
		EnableWindow(button_handle, false);
	}
	else {
		EnableWindow(button_handle, true);
	}
}

