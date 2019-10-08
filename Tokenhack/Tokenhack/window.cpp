#include <fstream>
#include <algorithm>

#include "resource.h"
#include "Custom Trackbar.h"
#include "window.h"

using namespace std;

TCHAR className[] = TEXT("Tokenhack");
UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

NOTIFYICONDATA g_notifyIconData;

//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
//LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);
//VOID CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD);
//BOOL CALLBACK EnumChildProc(HWND, LPARAM);
//BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//BOOL CALLBACK imageDlgProc(HWND, UINT, WPARAM, LPARAM);
//

HWND g_hwnd;
HHOOK kbhook;
HHOOK mhook;
HHOOK msghook;
HWND hworkout;
HWND icursong;
HWND statbox;
HWND image_display_box;
HWND image_input_edit;
HWND image_output_edit;
HWND image_input_static;
HWND image_output_static;
HCURSOR idle_cursor;
HCURSOR press_cursor;
HCURSOR loading_cursor;
HCURSOR token_cursor;
HFONT Font_a;
HFONT Font_itunes;
HFONT Font_workout;
HMENU g_menu;
MSG msg;
UINT timer1;
UINT timer2;
UINT timer3;
UINT timer4;
double starttime;
int click_toggle = 0;
int click_toggle2 = 0;
int timer_on = 0;
bool cursongvisstate = false;
bool is_hotkey_dialog_visible = false;
bool toggleall = false;
bool is_drawing_static = false;
DWORD global_change_hotkey;

vector<DWORD> hotkeys;

std::vector<std::string> workout;
UINT curworkout = 0;
int totalsets = 0;

bool move_window = true;
HWND lastwindow = NULL;

int window_id = 1000;

#pragma region window creation

functiontabbutton FUNC_TAB_BTN_ALL(window_id++, "All", 15, 0, 45, tabbuttonheight, 0);
functiontabbutton FUNC_TAB_BTN_D2(window_id++, "D2", 60, 0, 45, tabbuttonheight, 0);
functiontabbutton FUNC_TAB_BTN_ITUNES(window_id++, "Itunes", 105, 0, 65, tabbuttonheight, 0);
functiontabbutton FUNC_TAB_BTN_IMAGE(window_id++, "Image", 170, 0, 65, tabbuttonheight, 0);

//////////////////////////////////////////////////////////////////////////////////////

checkboxbutton CHK_BTN_TOGGLEALL(window_id++, "T", 0, 0, 14, 14, RGB(255, 206, 61), true);

int cb_id = 1500;
tokenhackfunction WMC(window_id++, cb_id++, "~wmc", "Toggle Cursor Lock");
tokenhackfunction CLOSE(window_id++, cb_id++, "~close", "Close");
tokenhackfunction GAMBLE(window_id++, cb_id++, "~gamble", "Gamble");
tokenhackfunction GOLD(window_id++, cb_id++, "~gold_drop", "Drop Gold(open inventory)");
tokenhackfunction WSGCAST(window_id++, cb_id++, "~wsg_cast", "wsg cast (+left click)");
tokenhackfunction SKILLBUG(window_id++, cb_id++, "~skill_bug", "Skillbug");
tokenhackfunction NOPICKUP(window_id++, cb_id++, "~\"/nopickup\"", "\"/nopickup\"");
tokenhackfunction LEFTCLICK(window_id++, cb_id++, "~left_click", "Left Click");
tokenhackfunction RIGHTCLICK(window_id++, cb_id++, "~right_click", "Right Click");
tokenhackfunction PAUSE(window_id++, cb_id++, "~pause", "Pause/Resume");
tokenhackfunction ROLL(window_id++, cb_id++, "~transmute", "Transmute");
tokenhackfunction DROP(window_id++, cb_id++, "~\".drop\"", "\".drop\"");
tokenhackfunction WSG(window_id++, cb_id++, "~switch_weapons", "switch weapons(scroll)");
tokenhackfunction AUTOROLL(window_id++, cb_id++, "~auto_roll", "Auto Roll (OCR)");
tokenhackfunction READ(window_id++, cb_id++, "~read_stats", "Read Stats (OCR)");
tokenhackfunction SKILLBUGSET(window_id++, cb_id++, "~skill_bug_set", "Skillbug Set key");
tokenhackfunction LAUNCH(window_id++, cb_id++, "~launch", "Launch");
tokenhackfunction CHARMROLL(window_id++, cb_id++, "~roll_charm", "Roll Charm");
tokenhackfunction TEXTSPAM(window_id++, cb_id++, "~text_spam", "Spam Text(#)");
tokenhackfunction IPLAY(window_id++, cb_id++, "~itunes_play", "play hotkey");
tokenhackfunction IPREV(window_id++, cb_id++, "~itunes_prev", "prev hotkey");
tokenhackfunction INEXT(window_id++, cb_id++, "~itunes_next", "next hotkey");
tokenhackfunction IVOLU(window_id++, cb_id++, "~itunes_volume_up", "volume up");
tokenhackfunction IVOLD(window_id++, cb_id++, "~itunes_volume_down", "volume down");
tokenhackfunction TIMER(window_id++, cb_id++, "~timer", "timer");
tokenhackfunction TEST(window_id++, cb_id++, "~test", "test");
tokenhackfunction BARBWW(window_id++, cb_id++, "~barbww", "barb ww");
tokenhackfunction BARBLEAP(window_id++, cb_id++, "~barbleap", "barb leap");
tokenhackfunction SELL;
tokenhackfunction DUPE;

//////////////////////////////////////////////////////////////////////////////////////

normalbutton BTN_CONFIG(window_id++, "Open Config", 24, &functionnameyoffsum, 35, 100, 30, RGB(255, 206, 61), true);
normalbutton BTN_RELOAD(window_id++, "Reload", 152, &functionnameyoffsum, 35, 80, 30, RGB(255, 206, 61), true);
normalbutton BTN_LAUNCH(window_id++, "Launch", 260, &functionnameyoffsum, 35, 100, 30, RGB(255, 206, 61), true);
normalbutton BTN_COW(window_id++, "C", 126, &functionnameyoffsum, 37, 25, 25, RGB(255, 206, 61), false);
normalbutton BTN_MUSHROOM(window_id++, "M", 234, &functionnameyoffsum, 37, 25, 25, RGB(255, 206, 61), false);

//////////////////////////////////////////////////////////////////////////////////////

togglebutton TBTN_SOUND(window_id++, "sound", "sound", 24, &functionnameyoffsum, 70, 61, 14, 0);
togglebutton TBTN_RAINBOW(window_id++, "rainbow", "rainbow", 90, &functionnameyoffsum, 70, 75, 14, 0);
togglebutton TBTN_CONSOLE(window_id++, "console", "console", 170, &functionnameyoffsum, 70, 75, 14, 0);
togglebutton TBTN_SHOWONSTART(window_id++, "show on start", "showtokenhackonstart", 250, &functionnameyoffsum, 70, 110, 14, 0);
togglebutton TBTN_HOOKOUTPUT(window_id++, "kb", "keyboardhookoutput", 4, &functionnameyoffsum, 70, 16, 14, 0);

//////////////////////////////////////////////////////////////////////////////////////

statboxtabbutton STAT_TAB_BTN_OUTPUT(window_id++, "Output", statboxxoff, 0, 60, tabbuttonheight, 1);
statboxtabbutton STAT_TAB_BTN_INPUT(window_id++, "Input", statboxxoff + 60, 0, 60, tabbuttonheight, 0);

//////////////////////////////////////////////////////////////////////////////////////

imagetabbutton IMG_TAB_BTN_CROP(window_id++, "Crop", 14, 105, 40, 15, 1);
imagetabbutton IMG_TAB_BTN_GROW(window_id++, "Grow", 54, 105, 40, 15, 1);
imagetabbutton IMG_TAB_BTN_GRAY(window_id++, "Gray", 94, 105, 40, 15, 1);
imagetabbutton IMG_TAB_BTN_BRIGHTNESS(window_id++, "Brightness", 134, 105, 80, 15, 1);
imagetabbutton IMG_TAB_BTN_CONTRAST(window_id++, "Contrast", 214, 105, 65, 15, 1);
imagetabbutton IMG_TAB_BTN_INVERT(window_id++, "Invert", 279, 105, 50, 15, 1);

imagetabbutton IMG_TAB_BTN_CONVERT(window_id++, "Convert", 14, 120, 105, 15, 2);
imagetabbutton IMG_TAB_BTN_COMPARE(window_id++, "Compare", 119, 120, 105, 15, 2);
imagetabbutton IMG_TAB_BTN_ANIMATE(window_id++, "Animate", 224, 120, 105, 15, 2);

imagetabbutton IMG_BTN_INPUT(window_id++, "...", 330, 39, 25, 16, 0);
imagetabbutton IMG_BTN_OUTPUT(window_id++, "...", 330, 79, 25, 16, 0);

//////////////////////////////////////////////////////////////////////////////////////

normalbutton BTN_IMAGE_PROCESS(window_id++, "Go", 334, nullptr, 137, 27, 40, RGB(255, 0, 0), false);
//clear btn
//out btn
#pragma region crop tab
string GROUP_CROP_STR = "crop tab";
staticcontrol STATIC_CROP_LEFT(window_id++, "Left:", 25, 151, 21, 14, NULL, false, true, GROUP_CROP_STR);
staticcontrol STATIC_CROP_RIGHT(window_id++, "Right:", 94, 151, 28, 14, NULL, false, true, GROUP_CROP_STR);
staticcontrol STATIC_CROP_TOP(window_id++, "Top:", 178, 151, 19, 14, NULL, false, true, GROUP_CROP_STR);
staticcontrol STATIC_CROP_BOTTOM(window_id++, "Bottom:", 245, 151, 37, 14, NULL, false, true, GROUP_CROP_STR);
editcontrol EDIT_CROP_LEFT(window_id++, "0", 53, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_CROP_STR);
editcontrol EDIT_CROP_RIGHT(window_id++, "0", 132, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_CROP_STR);
editcontrol EDIT_CROP_TOP(window_id++, "0", 211, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_CROP_STR);
editcontrol EDIT_CROP_BOTTOM(window_id++, "0", 290, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_CROP_STR);
controlgroup GROUP_CROP(GROUP_CROP_STR);
#pragma endregion
#pragma region grow tab
string GROUP_GROW_STR = "grow tab";
staticcontrol STATIC_GROW_LEFT(window_id++, "Left:", 25, 151, 21, 14, NULL, false, true, GROUP_GROW_STR);
staticcontrol STATIC_GROW_RIGHT(window_id++, "Right:", 94, 151, 28, 14, NULL, false, true, GROUP_GROW_STR);
staticcontrol STATIC_GROW_TOP(window_id++, "Top:", 178, 151, 19, 14, NULL, false, true, GROUP_GROW_STR);
staticcontrol STATIC_GROW_BOTTOM(window_id++, "Bottom:", 245, 151, 37, 14, NULL, false, true, GROUP_GROW_STR);
editcontrol EDIT_GROW_LEFT(window_id++, "0", 53, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_GROW_STR);
editcontrol EDIT_GROW_RIGHT(window_id++, "0", 132, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_GROW_STR);
editcontrol EDIT_GROW_TOP(window_id++, "0", 211, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_GROW_STR);
editcontrol EDIT_GROW_BOTTOM(window_id++, "0", 290, 151, 25, 14, NULL, false, true, INT_MIN, INT_MAX, true, GROUP_GROW_STR);
controlgroup GROUP_GROW(GROUP_GROW_STR);
#pragma endregion
#pragma region gray tab
string GROUP_GRAY_STR = "gray tab";
autoradiobutton RBTN_GRAY_AVGERAGE(window_id++, "Avg", 18, 140, 29, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_LUMA(window_id++, "Luma", 49, 140, 37, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_DESATURATE(window_id++, "Desaturate", 18, 159, 68, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_DECOMP_MIN(window_id++, "Min", 168, 140, 25, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_DECOMP_MAX(window_id++, "Max", 193, 140, 25, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_COLOR_CHANNEL_RED(window_id++, "R", 168, 158, 16, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_COLOR_CHANNEL_GREEN(window_id++, "G", 184, 158, 17, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
autoradiobutton RBTN_GRAY_COLOR_CHANNEL_BLUE(window_id++, "B", 201, 158, 17, 15, 1, RGB(255, 0, 0), false, true, GROUP_GRAY_STR);
staticcontrol STATIC_GRAY_DECOMPOSITION(window_id++, "Decomposition:", 90, 141, 74, 14, NULL, false, true, GROUP_GRAY_STR);
staticcontrol STATIC_GRAY_COLOR_CHANNEL(window_id++, "Color Channel:", 90, 159, 71, 11, NULL, false, true, GROUP_GRAY_STR);
staticcontrol STATIC_GRAY_COLORS(window_id++, "Colors:", 222, 141, 35, 14, NULL, false, true, GROUP_GRAY_STR);
staticcontrol STATIC_GRAY_DITHER(window_id++, "Dither:", 222, 160, 32, 14, NULL, false, true, GROUP_GRAY_STR);
editcontrol EDIT_GRAY_COLORS(window_id++, "256", 257, 141, 27, 14, NULL, false, true, 2, 256, true, GROUP_GRAY_STR);
checkboxbutton CHBOX_GRAY_DITHER(window_id++, "T", 263, 160, 14, 14, RGB(255, 0, 0), false, false, GROUP_GRAY_STR);
controlgroup GROUP_GRAY(GROUP_GRAY_STR);
#pragma endregion
#pragma region brightness tab
string GROUP_BRIGHTNESS_STR = "brightness tab";
custom_trackbar TRACKBAR_BRIGHTNESS_SET(window_id++, 17, 144, 249, 27, 0, 100, 100, 1, 5, 30, true, true, c_scheme, 0, IDLE, PRESS, GROUP_BRIGHTNESS_STR);
autoradiobutton RBTN_BRIGHTNESS_HSV(window_id++, "HSV", 282, 140, 31, 15, 1, RGB(255, 0, 0), false, true, GROUP_BRIGHTNESS_STR);
autoradiobutton RBTN_BRIGHTNESS_FLAT(window_id++, "Flat", 282, 159, 31, 15, 1, RGB(255, 0, 0), false, true, GROUP_BRIGHTNESS_STR);
controlgroup GROUP_BRIGHTNESS(GROUP_BRIGHTNESS_STR);
#pragma endregion
#pragma region contrast tab
string GROUP_CONTRAST_STR = "contrast tab";
controlgroup GROUP_CONTRAST(GROUP_CONTRAST_STR);
#pragma endregion
#pragma region convert tab
string GROUP_CONVERT_STR = "convert tab";
controlgroup GROUP_CONVERT(GROUP_CONVERT_STR);
#pragma endregion

#pragma endregion

#pragma region tokenhack window placement

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

#pragma region tokenhack functions struct

vector<tokenhackfunction*> tokenhackfunctions;
vector<tokenhackfunction*> all_tokenhackfunctions;

tokenhackfunction::tokenhackfunction():
	function_handle(NULL), button_handle(NULL), checkbox_handle(NULL), checkbox_id(0), button_id(0), hotkey_key(0), name_in_file(""), window_text(""), on(0), temp_hotkey(0) {}
tokenhackfunction::tokenhackfunction(int abutton_id, int acheckbox_id, string aname_in_file, string awindow_text):
	function_handle(NULL), button_handle(NULL), checkbox_handle(NULL), hotkey_key(0), on(0),
	button_id(abutton_id), checkbox_id(acheckbox_id), name_in_file(aname_in_file), window_text(awindow_text) {
	tokenhackfunctions.push_back(this);
	all_tokenhackfunctions.push_back(this);
}

#pragma endregion

#pragma region imagetabbuttons struct

std::vector<imagetabbutton*> imagetabbuttons;

imagetabbutton::imagetabbutton(int aid, string awindow_text, int axpos, int aypos, int awidth, int aheight, int arow):
	handle(NULL), showstate(false), toggle_state(false), true_row(arow),
	id(aid), window_text(awindow_text), xpos(axpos), ypos(aypos), width(awidth), height(aheight), row(arow) {
	imagetabbuttons.push_back(this);
}
#pragma endregion

#pragma region functiontabbuttons struct

vector<functiontabbutton*> functiontabbuttons;

functiontabbutton::functiontabbutton():
	handle(NULL), id(NULL), window_text(NULL), xpos(NULL), ypos(NULL), width(NULL), height(NULL), toggle_state(NULL) {}
functiontabbutton::functiontabbutton(int id, string window_text, int xpos, int ypos, int width, int height, bool toggle_state) :
	handle(NULL),
	id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), toggle_state(toggle_state) {
	functiontabbuttons.push_back(this);
}

#pragma endregion

#pragma region statboxtabbuttons struct

vector<statboxtabbutton*> statboxtabbuttons;

statboxtabbutton::statboxtabbutton():
	handle(NULL), id(NULL), window_text(NULL), xpos(NULL), ypos(NULL), width(NULL), height(NULL), toggle_state(NULL) {}
statboxtabbutton::statboxtabbutton(int id, string window_text, int xpos, int ypos, int width, int height, bool toggle_state):
	handle(NULL),
	id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), toggle_state(toggle_state) {
	statboxtabbuttons.push_back(this);
}
#pragma endregion

//

#pragma region ownerdrawn autoradiobuttons struct

vector<autoradiobutton*> autoradiobuttons;


autoradiobutton::autoradiobutton() {}
autoradiobutton::autoradiobutton(int id, string window_text, int xpos, int ypos, int width, int height, int group, COLORREF color, bool show_state, bool font, string group_name):
	handle(NULL), toggle_state(0),
	id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), group(group), color(color), show_state(show_state), font(font), group_name(group_name) {
	autoradiobuttons.push_back(this);
}

#pragma endregion

#pragma region ownerdrawn checkboxbuttons struct

vector<checkboxbutton*> checkboxbuttons;

checkboxbutton::checkboxbutton() {}
checkboxbutton::checkboxbutton(int id, string window_text, int xpos, int ypos, int width, int height, COLORREF color, bool show_state, bool font, string group_name) :
	handle(NULL), toggle_state(0),
	id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), color(color), show_state(show_state), font(font), group_name(group_name) {
	checkboxbuttons.push_back(this);
}

#pragma endregion

#pragma region normalbuttons struct

vector<normalbutton*> normalbuttons;

normalbutton::normalbutton() {}
normalbutton::normalbutton(int aid, string awindow_text, int axpos, int* ayoffset, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool font, string group_name):
	handle(NULL),
	id(aid), window_text(awindow_text), xpos(axpos), yoffset(ayoffset), ypos(aypos), width(awidth), height(aheight), color(color), show_state(ashowstate), font(font), group_name(group_name) {
	normalbuttons.push_back(this);
}

#pragma endregion

#pragma region togglebutton struct

vector<togglebutton*> togglebuttons;

togglebutton::togglebutton() {}
togglebutton::togglebutton(int id, string window_text, string string_in_file, int xpos, int* yoffset, int ypos, int width, int height, int toggle_state, bool font, string group_name):
	handle(NULL),
	id(id), window_text(window_text), string_in_file(string_in_file), xpos(xpos), yoffset(yoffset), ypos(ypos), width(width), height(height), font(font), toggle_state(toggle_state), group_name(group_name) {
	togglebuttons.push_back(this);
}

#pragma endregion

#pragma region editcontrol struct

vector<editcontrol*> editcontrols;

editcontrol::editcontrol() {}
editcontrol::editcontrol(int aid, string awindow_text, int axpos, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool number_only, int min_number, int max_number, bool font, string group_name):
	handle(NULL),
	id(aid), window_text(awindow_text), xpos(axpos), ypos(aypos), width(awidth), height(aheight), color(color), show_state(ashowstate), number_only(number_only), min_number(min_number), max_number(max_number), font(font), group_name(group_name) {
	editcontrols.push_back(this);
}

#pragma endregion

#pragma region staticcontrol struct

vector<staticcontrol*> staticcontrols;

staticcontrol::staticcontrol():
	handle(NULL), id(NULL), window_text(NULL), xpos(NULL), ypos(NULL), width(NULL), height(NULL), color(NULL), show_state(NULL) {}
staticcontrol::staticcontrol(int aid, string awindow_text, int axpos, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool font, string group_name) :
	handle(NULL),
	id(aid), window_text(awindow_text), xpos(axpos), ypos(aypos), width(awidth), height(aheight), color(color), show_state(ashowstate), font(font), group_name(group_name) {
	staticcontrols.push_back(this);
}

#pragma endregion

#pragma region controlgroup

vector<controlgroup> all_groups;

controlgroup::controlgroup():
	group_radiobuttons(NULL), group_checkboxbuttons(NULL), group_buttons(NULL), group_togglebuttons(NULL), group_staticcontrols(NULL), group_editcontrols(NULL), group_customtrackbars(NULL) {}
controlgroup::controlgroup(string group_name) :
	group_radiobuttons(NULL), group_checkboxbuttons(NULL), group_buttons(NULL), group_togglebuttons(NULL), group_staticcontrols(NULL), group_editcontrols(NULL) {
	for (UINT i = 0; i < autoradiobuttons.size(); i++) {
		if (group_name == autoradiobuttons[i]->group_name)
			group_radiobuttons.push_back(autoradiobuttons[i]);
	}
	for (UINT i = 0; i < checkboxbuttons.size(); i++) {
		if (group_name == checkboxbuttons[i]->group_name)
			group_checkboxbuttons.push_back(checkboxbuttons[i]);
	}
	for (UINT i = 0; i < normalbuttons.size(); i++) {
		if (group_name == normalbuttons[i]->group_name)
			group_buttons.push_back(normalbuttons[i]);
	}
	for (UINT i = 0; i < togglebuttons.size(); i++) {
		if (group_name == togglebuttons[i]->group_name)
			group_togglebuttons.push_back(togglebuttons[i]);
	}
	for (UINT i = 0; i < editcontrols.size(); i++) {
		if (group_name == editcontrols[i]->group_name)
			group_editcontrols.push_back(editcontrols[i]);
	}
	for (UINT i = 0; i < staticcontrols.size(); i++) {
		if (group_name == staticcontrols[i]->group_name)
			group_staticcontrols.push_back(staticcontrols[i]);
	}
	for (UINT i = 0; i < custom_trackbars.size(); i++) {
		if (group_name == custom_trackbars[i]->group_name)
			group_customtrackbars.push_back(custom_trackbars[i]);
	}
	all_groups.push_back(*this);
}

void controlgroup::show_group(bool show) {
	for (UINT i = 0; i < this->group_radiobuttons.size(); i++) {
		this->group_radiobuttons[i]->show_state = show;
		ShowWindow(this->group_radiobuttons[i]->handle, show ? SW_SHOW : SW_HIDE);
		EnableWindow(this->group_radiobuttons[i]->handle, show);
	}
	for (UINT i = 0; i < this->group_checkboxbuttons.size(); i++) {
		this->group_checkboxbuttons[i]->show_state = show;
		ShowWindow(this->group_checkboxbuttons[i]->handle, show ? SW_SHOW : SW_HIDE);
		EnableWindow(this->group_checkboxbuttons[i]->handle, show);
	}
	for (UINT i = 0; i < this->group_buttons.size(); i++) {
		this->group_buttons[i]->show_state = show;
		ShowWindow(this->group_buttons[i]->handle, show ? SW_SHOW : SW_HIDE);
		EnableWindow(this->group_buttons[i]->handle, show);
	}
	for (UINT i = 0; i < this->group_togglebuttons.size(); i++) {
		this->group_togglebuttons[i]->show_state = show;
		ShowWindow(this->group_togglebuttons[i]->handle, show ? SW_SHOW : SW_HIDE);
		EnableWindow(this->group_togglebuttons[i]->handle, show);
	}
	for (UINT i = 0; i < this->group_editcontrols.size(); i++) {
		this->group_editcontrols[i]->show_state = show;
		ShowWindow(this->group_editcontrols[i]->handle, show ? SW_SHOW : SW_HIDE);
		EnableWindow(this->group_editcontrols[i]->handle, show);
	}
	for (UINT i = 0; i < this->group_staticcontrols.size(); i++) {
		this->group_staticcontrols[i]->show_state = show;
		ShowWindow(this->group_staticcontrols[i]->handle, show ? SW_SHOW : SW_HIDE);
	}
	for (UINT i = 0; i < this->group_customtrackbars.size(); i++) {
		this->group_customtrackbars[i]->showstate = show;
		ShowWindow(this->group_customtrackbars[i]->handle, show ? SW_SHOW : SW_HIDE);
		EnableWindow(this->group_customtrackbars[i]->handle, show);
	}
}

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
	if (TBTN_SOUND.toggle_state)
		PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore() {
	if (TBTN_SOUND.toggle_state)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
	SetWindowPos(g_hwnd, HWND_TOP, 0, 0,
		(!READ.on && !AUTOROLL.on) ? (!FUNC_TAB_BTN_IMAGE.toggle_state ? windowwidth : windowwidth - 17) : windowwidth + 250/*180*/,
		functionnameheight*totalonfunctions + (!FUNC_TAB_BTN_IMAGE.toggle_state ? bottomheight : 61) + tokenhackfunctionwindowxposoffset,
		SWP_NOMOVE);
	SetForegroundWindow(g_hwnd);
}

void update_hotkeykeys() {
	hotkeys.erase(hotkeys.begin(), hotkeys.end());
	for (UINT i = 0; i < tokenhackfunctions.size(); i++)
		if (tokenhackfunctions[i]->on)
			hotkeys.push_back(tokenhackfunctions[i]->hotkey_key);
}
void turn_all_functions_off() {
	for (int i = 0; i<(int)tokenhackfunctions.size(); i++)
		tokenhackfunctions[i]->on = 0;
	totalonfunctions = 0;
}

void changeconfigonstate(string name_in_file) {
	if (name_in_file.size() == 0)
		return;
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
		var_name = var_name.substr(0, var_name.find('('));
		string val = line.substr(line.find(':') + 1, line.size());
		if (var_name.size() == 0)
			continue;
		//for (int j = 0; j<(int)tokenhackfunctions.size(); j++){
		if (var_name == name_in_file) {
			text[i] = text[i].erase(text[i].find(':') + 1, text[i].find('*') - text[i].find(':') - 1);
			text[i].insert(text[i].find(':') + 1, ((val != "0") ? "\t0\t" : "\t1\t"));
			break;
		}
		//}
	}
	ofstream fileout;
	fileout.open("config.txt");
	if (!fileout) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	for (unsigned int i = 0; i<text.size(); i++)
		fileout << text[i] << '\n';
	fileout.close();
}
void setbuttonstate(HWND hwnd, HWND button_handle, int checkbox_id, int on_state) {
	if (!on_state) {
		EnableWindow(button_handle, false);
	}
	else {
		EnableWindow(button_handle, true);
	}
}

