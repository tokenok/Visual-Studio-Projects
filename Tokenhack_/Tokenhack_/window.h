#ifndef window
#define window

#include <Windows.h>
#include <limits.h>
#include <string>
#include <vector>
#include <objidl.h>
#include "shlobj.h"

#include "resource.h"
#include "Custom Trackbar.h"

#define WM_TEST (WM_USER)
#define WM_TRAYICON (WM_USER+1)
#define DESTROY_WINDOWS (WM_USER+2)

#define WM_UPDATE_WND (WM_USER + 40)

#define CURSOR_IDLE (WM_USER + 1)
#define CURSOR_PRESS (WM_USER + 2)
#define CURSOR_LOADING (WM_USER + 3)

TCHAR className[] = TEXT("Tokenhack");
UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);
VOID CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD);
BOOL CALLBACK EnumChildProc(HWND, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//
HWND g_hwnd;
HHOOK kbhook;
HHOOK mhook;
HHOOK msghook;
HWND statbox;
HCURSOR idle_cursor;
HCURSOR press_cursor;
HCURSOR loading_cursor;
HCURSOR token_cursor;
HFONT Font_a;
HMENU g_menu;
MSG msg;
UINT timer2;
int click_toggle = 0;
int timer_on = 0;
bool cursongvisstate = false;
bool is_hotkey_dialog_visible = false;
bool toggleall = false;
bool is_drawing_static = false;
DWORD dw;

vector<DWORD> hotkeys;

static HBRUSH hbnormal = CreateSolidBrush(RGB(239, 239, 239));
static HBRUSH hbblack = CreateSolidBrush(RGB(10, 10, 10));

int blackback = 1;

enum {statboxid = 1500/*, toggleallcb*/};//other ids

#pragma region tokenhack functions struct

struct tokenhackfunction;
vector<tokenhackfunction*> tokenhackfunctions;
vector<tokenhackfunction*> all_tokenhackfunctions;

struct tokenhackfunction{
	HWND function_handle;
	HWND button_handle;
	HWND checkbox_handle;
	int checkbox_id;
	int button_id;
	DWORD hotkey_key;
	string name_in_file;
	string window_text;
	int on;
	//gamble + skillbug?
	int temp_hotkey;
	
	tokenhackfunction() :
		function_handle(NULL), button_handle(NULL), checkbox_handle(NULL), checkbox_id(0), button_id(0), hotkey_key(0), name_in_file(""), window_text(""), on(0), temp_hotkey(0) {
	}
	tokenhackfunction(int abutton_id, int acheckbox_id, string aname_in_file, string awindow_text) :
		function_handle(NULL), button_handle(NULL), checkbox_handle(NULL), hotkey_key(0), on(0), temp_hotkey(0),
		button_id(abutton_id), checkbox_id(acheckbox_id), name_in_file(aname_in_file), window_text(awindow_text) {
		tokenhackfunctions.push_back(this);
		all_tokenhackfunctions.push_back(this);
	}
};

int function_id = 1000;
int checkbox_id = 1100;

tokenhackfunction WMC(function_id++, checkbox_id++, "~wmc", "Toggle Cursor Lock");
tokenhackfunction CLOSE(function_id++, checkbox_id++, "~close", "Close");
tokenhackfunction GAMBLE(function_id++, checkbox_id++, "~gamble", "Gamble");
tokenhackfunction GOLD(function_id++, checkbox_id++, "~gold_drop", "Drop Gold(open inventory)");
tokenhackfunction WSGCAST(function_id++, checkbox_id++, "~wsg_cast", "wsg cast (+left click)");
tokenhackfunction SKILLBUG(function_id++, checkbox_id++, "~skill_bug", "Skillbug");
tokenhackfunction NOPICKUP(function_id++, checkbox_id++, "~\"/nopickup\"", "\"/nopickup\"");
tokenhackfunction LEFTCLICK(function_id++, checkbox_id++, "~left_click", "Left Click");
tokenhackfunction PAUSE(function_id++, checkbox_id++, "~pause", "Pause/Resume");
tokenhackfunction ROLL(function_id++, checkbox_id++, "~transmute", "Transmute");
tokenhackfunction DROP(function_id++, checkbox_id++, "~\".drop\"", "\".drop\"");
tokenhackfunction WSG(function_id++, checkbox_id++, "~switch_weapons", "switch weapons(scroll)");
tokenhackfunction AUTOROLL(function_id++, checkbox_id++, "~auto_roll", "Auto Roll (OCR)");
tokenhackfunction READ(function_id++, checkbox_id++, "~read_stats", "Read Stats (OCR)");
tokenhackfunction SKILLBUGSET(function_id++, checkbox_id++, "~skill_bug_set", "Skillbug Set key");
tokenhackfunction TEXTSPAM(function_id++, checkbox_id++, "~text_spam", "Spam Text(#)");
tokenhackfunction TEST(function_id++, checkbox_id++, "~test", "test");

#pragma endregion

#pragma region functiontabbuttons struct

struct functiontabbutton;
vector<functiontabbutton*> functiontabbuttons;

struct functiontabbutton {
	HWND handle;
	int id;
	string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	bool toggle_state;
	functiontabbutton():
		handle(NULL), id(NULL), window_text(NULL), xpos(NULL), ypos(NULL), width(NULL), height(NULL), toggle_state(NULL) {}
	functiontabbutton(int id, string window_text, int xpos, int ypos, int width, int height, bool toggle_state):
		handle(NULL),
		id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), toggle_state(toggle_state) {
		functiontabbuttons.push_back(this);
	}

};

functiontabbutton FUNC_TAB_BTN_ALL(checkbox_id++, "D2", 15, 0, 45, tabbuttonheight, 0);

#pragma endregion

#pragma region statboxtabbuttons struct

struct statboxtabbutton;
vector<statboxtabbutton*> statboxtabbuttons;

struct statboxtabbutton {
	HWND handle;
	int id;
	string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	bool toggle_state;
	statboxtabbutton():
		handle(NULL), id(NULL), window_text(NULL), xpos(NULL), ypos(NULL), width(NULL), height(NULL), toggle_state(NULL) {}
	statboxtabbutton(int id, string window_text, int xpos, int ypos, int width, int height, bool toggle_state):
		handle(NULL),
		id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), toggle_state(toggle_state) {
		statboxtabbuttons.push_back(this);
	}

};

statboxtabbutton STAT_TAB_BTN_OUTPUT(checkbox_id++, "Output", statboxxoff, 0, 60, tabbuttonheight, 1);
//statboxtabbutton STAT_TAB_BTN_INPUT(checkbox_id++, "Input", statboxxoff + 60, 0, 60, tabbuttonheight, 0);

#pragma endregion

//

#pragma region ownerdrawn autoradiobuttons struct

struct autoradiobutton;
vector<autoradiobutton*> autoradiobuttons;

struct autoradiobutton {
	HWND handle;
	int id;
	int xpos;
	int ypos;
	int width;
	int height;
	int group;
	string window_text;
	bool show_state;
	bool toggle_state;
	COLORREF color;	
	string group_name;
	bool font;
	autoradiobutton(){}
	autoradiobutton(int id, string window_text, int xpos, int ypos, int width, int height, int group, COLORREF color, bool show_state, bool font = false, string group_name=""):
		handle(NULL), toggle_state(0),
		id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), group(group), color(color), show_state(show_state), font(font), group_name(group_name) {

		autoradiobuttons.push_back(this);
	}
};

#pragma endregion

#pragma region ownerdrawn checkboxbuttons struct

struct checkboxbutton;
vector<checkboxbutton*> checkboxbuttons;

struct checkboxbutton {
	HWND handle;
	int id;
	int xpos;
	int ypos;
	int width;
	int height;
	string window_text;
	bool show_state;
	bool toggle_state;
	COLORREF color;
	string group_name;
	bool font;
	checkboxbutton():handle(NULL) {}
	checkboxbutton(int id, string window_text, int xpos, int ypos, int width, int height, COLORREF color, bool show_state, bool font = false, string group_name = ""):
		handle(NULL), toggle_state(0),
		id(id), window_text(window_text), xpos(xpos), ypos(ypos), width(width), height(height), color(color), show_state(show_state), font(font), group_name(group_name) {
		checkboxbuttons.push_back(this);
	}
};

checkboxbutton CHK_BTN_TOGGLEALL(checkbox_id++, "T", 0, 0, 14, 14, RGB(255, 206, 61), true);

#pragma endregion

#pragma region normalbuttons struct

struct normalbutton;
vector<normalbutton*> normalbuttons;

struct normalbutton {
	HWND handle;
	int id;
	string window_text;
	int xpos;
	int ypos;
	int* yoffset;
	int width;
	int height;
	COLORREF color;
	bool show_state;
	string group_name;
	bool font;
	normalbutton(){}
	normalbutton(int aid, string awindow_text, int axpos, int* ayoffset, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool font = false, string group_name = ""):
		handle(NULL),
		id(aid), window_text(awindow_text), xpos(axpos), yoffset(ayoffset), ypos(aypos), width(awidth), height(aheight), color(color), show_state(ashowstate), font(font), group_name(group_name) {
		normalbuttons.push_back(this);
	}
};

normalbutton BTN_CONFIG(checkbox_id++, "Open Config", 24, &functionnameyoffsum, 35, 100, 30, RGB(254, 33, 61), true);
normalbutton BTN_RELOAD(checkbox_id++, "Reload", 152, &functionnameyoffsum, 35, 80, 30, RGB(254, 33, 61), true);
normalbutton BTN_LAUNCH(checkbox_id++, "Launch", 260, &functionnameyoffsum, 35, 100, 30, RGB(254, 33, 61), true);
normalbutton BTN_COW(checkbox_id++, "C", 126, &functionnameyoffsum, 37, 25, 25, RGB(254, 33, 61), false);
normalbutton BTN_MUSHROOM(checkbox_id++, "M", 234, &functionnameyoffsum, 37, 25, 25, RGB(254, 33, 61), false);

#pragma endregion

#pragma region togglebutton struct

struct togglebutton;
vector<togglebutton*> togglebuttons;

struct togglebutton {
	HWND handle;
	int id;
	string window_text;
	string string_in_file;
	int xpos;
	int ypos;
	int* yoffset;
	int width;
	int height;
	int toggle_state;
	bool show_state;
	string group_name;
	bool font;
	togglebutton(){}
	togglebutton(int id, string window_text, string string_in_file, int xpos, int* yoffset, int ypos, int width, int height, int toggle_state, bool font = false, string group_name = ""):
		handle(NULL),
		id(id), window_text(window_text), string_in_file(string_in_file), xpos(xpos), yoffset(yoffset), ypos(ypos), width(width), height(height), font(font), toggle_state(toggle_state), group_name(group_name) {
		togglebuttons.push_back(this);
	}
};

togglebutton TBTN_SOUND(checkbox_id++, "sound", "sound", 24, &functionnameyoffsum, 70, 61, 14, 0);

//custom_trackbar CTB_VOLUME(checkbox_id++, 100, 0, 75, 14, 0, 100, 100, 1, 10, 1, true, true, {
//	RGB(10, 10, 10), RGB(50, 50, 50),																// background/border
//	RGB(0, 118, 255), RGB(0, 118, 255), RGB(0, 118, 255), RGB(0, 118, 255), 						// left/right channel left/right highlight
//	RGB(0, 118, 255), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb background: idle/hover/selected
//	RGB(0, 118, 255), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb border: idle/hover/selected
//	RGB(0, 118, 255), RGB(0, 118, 255), RGB(0, 118, 255)											// thumb text: idle/hover/selected
//}, 
//true);
togglebutton TBTN_RAINBOW(checkbox_id++, "rainbow", "rainbow", 90, &functionnameyoffsum, 70, 75, 14, 0);
togglebutton TBTN_CONSOLE(checkbox_id++, "console", "console", 170, &functionnameyoffsum, 70, 75, 14, 0);
togglebutton TBTN_SHOWONSTART(checkbox_id++, "show on start", "showtokenhackonstart", 250, &functionnameyoffsum, 70, 110, 14, 0);
togglebutton TBTN_HOOKOUTPUT(checkbox_id++, "kb", "keyboardhookoutput", 4, &functionnameyoffsum, 70, 16, 14, 0);

#pragma endregion

#pragma region editcontrol struct

struct editcontrol;
vector<editcontrol*> editcontrols;

struct editcontrol {
	HWND handle;
	int id;
	string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	COLORREF color;
	bool show_state;
	bool number_only;
	int min_number;
	int max_number;
	string group_name;
	bool font;
	editcontrol(){}
	editcontrol(int aid, string awindow_text, int axpos, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool number_only = false, int min_number = INT_MIN, int max_number = INT_MAX, bool font = false, string group_name = ""):
		handle(NULL),
		id(aid), window_text(awindow_text), xpos(axpos), ypos(aypos), width(awidth), height(aheight), color(color), show_state(ashowstate), number_only(number_only), min_number(min_number), max_number(max_number), font(font), group_name(group_name) {
		editcontrols.push_back(this);
	}
};

#pragma endregion

#pragma region staticcontrol struct

struct staticcontrol;
vector<staticcontrol*> staticcontrols;

struct staticcontrol {
	HWND handle;
	int id;
	string window_text;
	int xpos;
	int ypos;
	int width;
	int height;
	COLORREF color;
	bool show_state;
	string group_name;
	bool font;
	staticcontrol():
		handle(NULL), id(NULL), window_text(NULL), xpos(NULL), ypos(NULL), width(NULL), height(NULL), color(NULL), show_state(NULL) {}
	staticcontrol(int aid, string awindow_text, int axpos, int aypos, int awidth, int aheight, COLORREF color, bool ashowstate, bool font = false, string group_name = ""):
		handle(NULL),
		id(aid), window_text(awindow_text), xpos(axpos), ypos(aypos), width(awidth), height(aheight), color(color), show_state(ashowstate), font(font), group_name(group_name) {
		staticcontrols.push_back(this);
	}
};

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
	memcpy(g_notifyIconData.szInfoTitle, WVERSION.c_str(), 64);
	memcpy(g_notifyIconData.szInfo, TEXT("Click icon to see menu"), 256);
	memcpy(g_notifyIconData.szTip, WVERSION.c_str(), 128);
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
	wnd.hbrBackground = CreateSolidBrush(RGB(239, 239, 239));
	if (blackback)wnd.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
	//wnd.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
	//else wnd.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;	
	//wnd.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(TOKENBACK)));
	return RegisterClassEx(&wnd);
}

void Minimize(){
	if (TBTN_SOUND.toggle_state)
		PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore(){
	if (TBTN_SOUND.toggle_state)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
	SetWindowPos(g_hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250/*180*/, functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);
	SetFocus(g_hwnd);
	//SetWindowPos(g_hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 170, functionnameheight*totalonfunctions + 135, SWP_NOMOVE | SWP_SHOWWINDOW);
}
void CONSOLE_ON(){
	FILE *stream;
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&stream, "CONOUT$", "w", stdout);
}

void update_hotkeykeys(){
	hotkeys.erase(hotkeys.begin(), hotkeys.end());
	for (UINT i = 0; i < tokenhackfunctions.size(); i++)
		if (tokenhackfunctions[i]->on)
			hotkeys.push_back(tokenhackfunctions[i]->hotkey_key);
}
void turn_all_functions_off(){
	for (int i = 0; i<(int)tokenhackfunctions.size(); i++)
		tokenhackfunctions[i]->on = 0;
	totalonfunctions = 0;
}

string get_window_class_name(HWND hwnd){
	size_t i;
	TCHAR clsss[15];
	char a[15];
	GetClassName((HWND)hwnd, clsss, 15);
	wcstombs_s(&i, a, (size_t)15, clsss, 15);
	return string(a);
}
string getwindowtext(HWND wnd) {
	wchar_t text[256];
	GetWindowText(wnd, text, 256);
	return wstr_to_str(text);
}
RECT getwindowrect(HWND wnd) {
	RECT ret;
	GetWindowRect(wnd, &ret);
	return ret;
}
RECT getclientrect(HWND wnd) {
	RECT ret;
	GetClientRect(wnd, &ret);
	return ret;
}
RECT getmappedclientrect(HWND wnd, HWND wndTo = NULL) {
	RECT ret;
	GetClientRect(wnd, &ret);
	MapWindowPoints(wnd, wndTo, (LPPOINT)&ret, 2);
	return ret;
}
POINT getclientcursorpos(HWND hwnd_parent) {
	RECT temprect;
	GetClientRect(hwnd_parent, &temprect);
	MapWindowPoints(hwnd_parent, NULL, (LPPOINT)&temprect, 2);
	POINT temppoint;
	GetCursorPos(&temppoint);
	POINT client_cursor_pos;
	client_cursor_pos.x = temppoint.x - temprect.left;
	client_cursor_pos.y = temppoint.y - temprect.top;
	return client_cursor_pos;
}
bool cursor_in_region(RECT region, POINT cursor_pos) {
	return (cursor_pos.x > region.left && cursor_pos.x < region.right && cursor_pos.y < region.bottom && cursor_pos.y > region.top);
}

bool GetFolder(std::string& folderpath, const wchar_t* szCaption = NULL, HWND hOwner = NULL) {
	bool retVal = false;

	// The BROWSEINFO struct tells the shell 
	// how it should display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags = BIF_USENEWUI;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = szCaption;

	// must call this if using BIF_USENEWUI
	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the 
	// selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if (pIDL != NULL) {
		// Create a buffer to store the path, then 
		// get the path.
		wchar_t buffer[_MAX_PATH] = {'\0'};
		if (::SHGetPathFromIDList(pIDL, buffer) != 0) {
			// Set the string value.
			folderpath = wstr_to_str((wstring)buffer);
			retVal = true;
		}

		// free the item id list
		CoTaskMemFree(pIDL);
	}

	::OleUninitialize();

	return retVal;
}
void GetFilesInDirectory(std::vector<string> &out, const string &directory) {
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile(str_to_wstr((directory + "/*.bmp")).c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = wstr_to_str(file_data.cFileName);
		const string full_file_name = directory + "\\" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	}
	while (FindNextFile(dir, &file_data));

	FindClose(dir);

}

void changeconfigonstate(string name_in_file){
	if (name_in_file.size() == 0)
		return;
	vector<string> text;
	string line;
	filein.open("config.txt");
	if (!filein){
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	while (getline(filein, line))
		text.push_back(line);
	filein.close();

	for (UINT i = 0; i < text.size(); i++){
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
			if (var_name == name_in_file){
				text[i] = text[i].erase(text[i].find(':') + 1, text[i].find('*') - text[i].find(':') - 1);
				text[i].insert(text[i].find(':') + 1, ((val != "0") ? "\t0\t" : "\t1\t"));
				break;
			}
		//}
	}
	fileout.open("config.txt");
	if (!fileout){
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	for (unsigned int i = 0; i<text.size(); i++)
		fileout << text[i] << '\n';
	fileout.close();
}
void setbuttonstate(HWND hwnd, HWND button_handle, int checkbox_id, int on_state){
	if (!on_state){
		EnableWindow(button_handle, false);
	}
	else{
		EnableWindow(button_handle, true);
	}
}


#endif

