#include "functions.h"

#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "common.h"
#include "window.h"
#include "resource.h"
#include "hotkey.h"

using namespace std;

#define PI       3.14159265358979323846

ATOM init_wnd_class(HINSTANCE hInst, LPCWSTR classname, WNDPROC wndproc, UINT iconid) {
	WNDCLASSEX wnd = {0};
	wnd.hInstance = hInst;
	wnd.lpszClassName = classname;
	wnd.lpfnWndProc = wndproc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.hIconSm = (HICON)LoadIcon(hInst, MAKEINTRESOURCE(iconid));
	return RegisterClassEx(&wnd);
}
void init_notify_icon_data(NOTIFYICONDATA &nid, HWND wnd, UINT msgid, UINT iconid, LPCWSTR icontiptitle) {
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = wnd;
	nid.uID = 5000;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = msgid;
	nid.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(iconid), IMAGE_ICON, 0, 0, LR_SHARED);
	nid.uTimeout = 0;
	nid.uVersion = NOTIFYICON_VERSION;
	memcpy(nid.szInfoTitle, icontiptitle, 64);
	memcpy(nid.szInfo, TEXT("Click icon to see menu"), 256);
	memcpy(nid.szTip, icontiptitle, 128);
	nid.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

COLORREF color(int iter, double frequency, double phase/* = 120*/, double center/* = 128*/, double width/* = 127*/) {
	double frequency_val = PI * 2 / frequency;
	double red = sin(frequency_val * iter + 2 + phase) * width + center;
	double green = sin(frequency_val * iter + 0 + phase) * width + center;
	double blue = sin(frequency_val * iter + 4 + phase) * width + center;
	return RGB(red, green, blue);
}

BOOL ProtectProcess() {
	HANDLE hProcess = GetCurrentProcess();
	EXPLICIT_ACCESS denyAccess = {0};
	DWORD dwAccessPermissions = GENERIC_WRITE | PROCESS_ALL_ACCESS | WRITE_DAC | DELETE | WRITE_OWNER | READ_CONTROL;
	BuildExplicitAccessWithName(&denyAccess, TEXT("CURRENT_USER"), dwAccessPermissions, DENY_ACCESS, NO_INHERITANCE);
	PACL pTempDacl = NULL;
	DWORD dwErr = 0;
	dwErr = SetEntriesInAcl(1, &denyAccess, NULL, &pTempDacl);
	dwErr = SetSecurityInfo(hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pTempDacl, NULL);
	LocalFree(pTempDacl);
	CloseHandle(hProcess);
	return dwErr == ERROR_SUCCESS;
}

void set_volume(WORD volume, WORD left /*= 0*/, WORD right /*= 0*/) {
	if (!left)
		left = volume;
	if (!right)
		right = volume;
	waveOutSetVolume(0, MAKELPARAM(left, right));
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

#pragma region tokenhack function class

BasicControl_colorscheme tokenhackfunctionhotkey_scheme(
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
	RGB(45, 45, 45), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
	//															//SELECTED ON: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10)			//HOVER OFF: background/border/text
	//															//SELECTED OFF: background/border/text
	);
BasicControl_colorscheme tokenhackfunctioncheckbox_scheme(
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
	RGB(45, 45, 45), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
	RGB(45, 45, 45), RGB(254, 33, 61), RGB(45, 45, 45)			//HOVER OFF: background/border/text
	);
BasicControl_colorscheme tokenhackfunctionstatic_scheme(
	RGB(10, 10, 10), RGB(148, 0, 211), RGB(148, 0, 211)			//IDLE ON: background/border/text
	);
BasicControl_colorscheme tokenhackfunctiontab_scheme(
	RGB(100, 100, 100), RGB(100, 100, 100), RGB(10, 10, 10),	//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(100, 100, 100), RGB(10, 10, 10),	//HOVER ON: background/border/text
	//															//SELECTED ON: background/border/text
	RGB(10, 10, 10), RGB(100, 100, 100), RGB(100, 100, 100),	//IDLE OFF: background/border/text
	RGB(100, 100, 100), RGB(10, 10, 10), RGB(10, 10, 10)		//HOVER OFF: background/border/text
	//															//SELECTED OFF: background/border/text
	);
BasicControl_colorscheme tokenhackfunctionconfigstatic_scheme(
	RGB(20, 20, 20), RGB(0, 118, 255), RGB(0, 118, 255)			//IDLE ON: background/border/text
	);

vector<tokenhackfunction*> tokenhackfunctions;
vector<tokenhackfunction*> all_tokenhackfunctions;

BOOL CALLBACK hotkeydlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void TOKENHACK_CHECKBOX_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	for (auto a : tokenhackfunctions) {
		if (b->handle == a->checkbox_button.handle) {
			if (a->on) {//if on, turn off
				//a->set_on_state(str_to_int(changeconfigonstate(a->name_in_file)));
				EnableWindow(a->hotkey_button.handle, false);
			}
			else {//if off, turn on					
				//a->set_on_state(str_to_int(changeconfigonstate(a->name_in_file)));
				EnableWindow(a->hotkey_button.handle, true);
			}
			RedrawWindow(a->hotkey_button.handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
	}
}
void TOKENHACK_HOTKEYBUTTON_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	for (auto a : tokenhackfunctions) {
		if (b->handle == a->hotkey_button.handle) {
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(HOTKEYBOX), GetForegroundWindow(), hotkeydlgProc);
			if (global_change_hotkey != 0) {
				if (global_change_hotkey == 1)
					global_change_hotkey = 0;
				hotkeychange(a->hotkey_button.handle, a->hotkey_button.id, a->hotkey_key, global_change_hotkey);
			}
			break;
		}
	}
}

//void TOKENHACK_CONFIG_FUNCTIONS_TOGGLE_PROC(WPARAM wParam, LPARAM /*lParam*/) {
//	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
//	cout << b->window_text << '\n';
//	b->toggle_state = 1;
//	cout << b->toggle_state << '\n';
//}
//void TOKENHACK_CONFIG_FUNCTIONS_EDIT_PROC(WPARAM wParam, LPARAM /*lParam*/) {
//	EditControl* b = (EditControl*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);	
//	string EditText = getwindowtext(b->handle);
//
//	for (UINT i = 0; i < tokenhackfunctions.size() - 15; i++) {
//		tokenhackfunction* a = tokenhackfunctions[i];
//		if (a->config_edit_control.handle != NULL && a->config_edit_control.handle == b->handle) {
//			changebuttontext(a->config_static_control.handle, str_to_int(EditText));
//			RedrawWindow(a->config_static_control.handle, NULL, NULL, RDW_INVALIDATE);
//			break;
//		}
//	}
//}

tokenhackfunction::tokenhackfunction() {}
tokenhackfunction::tokenhackfunction(HWND parent, string static_text, string name_in_file, vector<string> ccb_options):
checkbox_button("T", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, parent, get_unique_id(), TOKENHACK_CHECKBOX_PROC, tokenhackfunctioncheckbox_scheme, NULL, NULL, 0, grp_tokenhack_function),
hotkey_button("", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, parent, get_unique_id(), TOKENHACK_HOTKEYBUTTON_PROC, tokenhackfunctionhotkey_scheme, NULL, NULL, grp_tokenhack_function),
static_control(static_text, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, parent, get_unique_id(), nullptr, tokenhackfunctionstatic_scheme, NULL, NULL, NULL, grp_tokenhack_function)
//,

//config_toggle_button(static_text, WS_CHILD, 15, 0, 132, 23, con_tbtn_id++, TOKENHACK_CONFIG_FUNCTIONS_TOGGLE_PROC, "", togglebutton_scheme, Font_a, NULL, 1, own_config_dialog_tab_functions, grp_tokenhack_function),
//config_edit_control("", WS_CHILD | ES_LEFT, 149, 0, 33, 23, con_e_id++, TOKENHACK_CONFIG_FUNCTIONS_EDIT_PROC, 0, togglebutton_scheme, Font_a, NULL, default_hover, own_config_dialog_tab_functions, grp_tokenhack_function, true, 0, 4094),
//config_static_control("", WS_CHILD | SS_CENTER, 184, 0, 266, 23, con_s_id++, nullptr, tokenhackfunctionconfigstatic_scheme, Font_a, NULL, default_hover, own_config_dialog_tab_functions, grp_tokenhack_function),
//config_custom_combobox("", WS_CHILD, 452, 0, 72, 23, con_c_id++, ccb_options, 15, (UINT)-1, ccb_style2, nullptr, normalbutton_scheme, Font_a, NULL, own_config_dialog_tab_functions, grp_tokenhack_function)
{
	checkbox_button.handle = NULL; hotkey_button.handle = NULL; static_control.handle = NULL; hotkey_key = 0; on = 0;
	this->name_in_file = name_in_file; this->group_name = grp_tokenhack_function;
	//config_toggle_button.string_in_file = name_in_file;

	tokenhackfunctions.push_back(this);
	all_tokenhackfunctions.push_back(this);
}

void tokenhackfunction::set_on_state(int state) {
	on = state;
	checkbox_button.toggle_state = state;
	hotkey_button.toggle_state = state;
	static_control.toggle_state = state;
}

#pragma endregion

#pragma region getmessagetext

#define X(x) { x, #x },
struct WindowsMessage {
	int msgid;
	char* pname;
} WindowsMessages[] =
{
	X(WM_NULL)
	X(WM_CREATE)
	X(WM_DESTROY)
	X(WM_MOVE)
	X(WM_SIZE)
	X(WM_ACTIVATE)
	X(WM_SETFOCUS)
	X(WM_KILLFOCUS)
	X(WM_ENABLE)
	X(WM_SETREDRAW)
	X(WM_SETTEXT)
	X(WM_GETTEXT)
	X(WM_GETTEXTLENGTH)
	X(WM_PAINT)
	X(WM_CLOSE)
	X(WM_QUERYENDSESSION)
	X(WM_QUERYOPEN)
	X(WM_ENDSESSION)
	X(WM_QUIT)
	X(WM_ERASEBKGND)
	X(WM_SYSCOLORCHANGE)
	X(WM_SHOWWINDOW)
	X(WM_WININICHANGE)
	X(WM_DEVMODECHANGE)
	X(WM_ACTIVATEAPP)
	X(WM_FONTCHANGE)
	X(WM_TIMECHANGE)
	X(WM_CANCELMODE)
	X(WM_SETCURSOR)
	X(WM_MOUSEACTIVATE)
	X(WM_CHILDACTIVATE)
	X(WM_QUEUESYNC)
	X(WM_GETMINMAXINFO)
	X(WM_PAINTICON)
	X(WM_ICONERASEBKGND)
	X(WM_NEXTDLGCTL)
	X(WM_SPOOLERSTATUS)
	X(WM_DRAWITEM)
	X(WM_MEASUREITEM)
	X(WM_DELETEITEM)
	X(WM_VKEYTOITEM)
	X(WM_CHARTOITEM)
	X(WM_SETFONT)
	X(WM_GETFONT)
	X(WM_SETHOTKEY)
	X(WM_GETHOTKEY)
	X(WM_QUERYDRAGICON)
	X(WM_COMPAREITEM)
	X(WM_GETOBJECT)
	X(WM_COMPACTING)
	X(WM_COMMNOTIFY)
	X(WM_WINDOWPOSCHANGING)
	X(WM_WINDOWPOSCHANGED)
	X(WM_POWER)
	X(WM_COPYDATA)
	X(WM_CANCELJOURNAL)
	X(WM_NOTIFY)
	X(WM_INPUTLANGCHANGEREQUEST)
	X(WM_INPUTLANGCHANGE)
	X(WM_TCARD)
	X(WM_HELP)
	X(WM_USERCHANGED)
	X(WM_NOTIFYFORMAT)
	X(WM_CONTEXTMENU)
	X(WM_STYLECHANGING)
	X(WM_STYLECHANGED)
	X(WM_DISPLAYCHANGE)
	X(WM_GETICON)
	X(WM_SETICON)
	X(WM_NCCREATE)
	X(WM_NCDESTROY)
	X(WM_NCCALCSIZE)
	X(WM_NCHITTEST)
	X(WM_NCPAINT)
	X(WM_NCACTIVATE)
	X(WM_GETDLGCODE)
	X(WM_SYNCPAINT)
	X(WM_NCMOUSEMOVE)
	X(WM_NCLBUTTONDOWN)
	X(WM_NCLBUTTONUP)
	X(WM_NCLBUTTONDBLCLK)
	X(WM_NCRBUTTONDOWN)
	X(WM_NCRBUTTONUP)
	X(WM_NCRBUTTONDBLCLK)
	X(WM_NCMBUTTONDOWN)
	X(WM_NCMBUTTONUP)
	X(WM_NCMBUTTONDBLCLK)
	X(WM_NCXBUTTONDOWN)
	X(WM_NCXBUTTONUP)
	X(WM_NCXBUTTONDBLCLK)
	X(WM_INPUT_DEVICE_CHANGE)
	X(WM_INPUT)
	X(WM_KEYDOWN)
	X(WM_KEYUP)
	X(WM_CHAR)
	X(WM_DEADCHAR)
	X(WM_SYSKEYDOWN)
	X(WM_SYSKEYUP)
	X(WM_SYSCHAR)
	X(WM_SYSDEADCHAR)
	X(WM_UNICHAR)
	X(WM_KEYLAST)
	X(WM_KEYLAST)
	X(WM_IME_STARTCOMPOSITION)
	X(WM_IME_ENDCOMPOSITION)
	X(WM_IME_COMPOSITION)
	X(WM_IME_KEYLAST)
	X(WM_INITDIALOG)
	X(WM_COMMAND)
	X(WM_SYSCOMMAND)
	X(WM_TIMER)
	X(WM_HSCROLL)
	X(WM_VSCROLL)
	X(WM_INITMENU)
	X(WM_INITMENUPOPUP)
	X(WM_GESTURE)
	X(WM_GESTURENOTIFY)
	X(WM_MENUSELECT)
	X(WM_MENUCHAR)
	X(WM_ENTERIDLE)
	X(WM_MENURBUTTONUP)
	X(WM_MENUDRAG)
	X(WM_MENUGETOBJECT)
	X(WM_UNINITMENUPOPUP)
	X(WM_MENUCOMMAND)
	X(WM_CHANGEUISTATE)
	X(WM_UPDATEUISTATE)
	X(WM_QUERYUISTATE)
	X(WM_CTLCOLORMSGBOX)
	X(WM_CTLCOLOREDIT)
	X(WM_CTLCOLORLISTBOX)
	X(WM_CTLCOLORBTN)
	X(WM_CTLCOLORDLG)
	X(WM_CTLCOLORSCROLLBAR)
	X(WM_CTLCOLORSTATIC)
	X(WM_MOUSEMOVE)
	X(WM_LBUTTONDOWN)
	X(WM_LBUTTONUP)
	X(WM_LBUTTONDBLCLK)
	X(WM_RBUTTONDOWN)
	X(WM_RBUTTONUP)
	X(WM_RBUTTONDBLCLK)
	X(WM_MBUTTONDOWN)
	X(WM_MBUTTONUP)
	X(WM_MBUTTONDBLCLK)
	X(WM_MOUSEWHEEL)
	X(WM_XBUTTONDOWN)
	X(WM_XBUTTONUP)
	X(WM_XBUTTONDBLCLK)
	X(WM_MOUSEHWHEEL)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_PARENTNOTIFY)
	X(WM_ENTERMENULOOP)
	X(WM_EXITMENULOOP)
	X(WM_NEXTMENU)
	X(WM_SIZING)
	X(WM_CAPTURECHANGED)
	X(WM_MOVING)
	X(WM_POWERBROADCAST)
	X(WM_MDICREATE)
	X(WM_MDIDESTROY)
	X(WM_MDIACTIVATE)
	X(WM_MDIRESTORE)
	X(WM_MDINEXT)
	X(WM_MDIMAXIMIZE)
	X(WM_MDITILE)
	X(WM_MDICASCADE)
	X(WM_MDIICONARRANGE)
	X(WM_MDIGETACTIVE)
	X(WM_MDISETMENU)
	X(WM_ENTERSIZEMOVE)
	X(WM_EXITSIZEMOVE)
	X(WM_DROPFILES)
	X(WM_MDIREFRESHMENU)
	X(WM_POINTERDEVICECHANGE)
	X(WM_POINTERDEVICEINRANGE)
	X(WM_POINTERDEVICEOUTOFRANGE)
	X(WM_TOUCH)
	X(WM_NCPOINTERUPDATE)
	X(WM_NCPOINTERDOWN)
	X(WM_NCPOINTERUP)
	X(WM_POINTERUPDATE)
	X(WM_POINTERDOWN)
	X(WM_POINTERUP)
	X(WM_POINTERENTER)
	X(WM_POINTERLEAVE)
	X(WM_POINTERACTIVATE)
	X(WM_POINTERCAPTURECHANGED)
	X(WM_TOUCHHITTESTING)
	X(WM_POINTERWHEEL)
	X(WM_POINTERHWHEEL)
	X(WM_IME_SETCONTEXT)
	X(WM_IME_NOTIFY)
	X(WM_IME_CONTROL)
	X(WM_IME_COMPOSITIONFULL)
	X(WM_IME_SELECT)
	X(WM_IME_CHAR)
	X(WM_IME_REQUEST)
	X(WM_IME_KEYDOWN)
	X(WM_IME_KEYUP)
	X(WM_MOUSEHOVER)
	X(WM_MOUSELEAVE)
	X(WM_NCMOUSEHOVER)
	X(WM_NCMOUSELEAVE)
	X(WM_WTSSESSION_CHANGE)
	X(WM_TABLET_FIRST)
	X(WM_TABLET_LAST)
	X(WM_DPICHANGED)
	X(WM_CUT)
	X(WM_COPY)
	X(WM_PASTE)
	X(WM_CLEAR)
	X(WM_UNDO)
	X(WM_RENDERFORMAT)
	X(WM_RENDERALLFORMATS)
	X(WM_DESTROYCLIPBOARD)
	X(WM_DRAWCLIPBOARD)
	X(WM_PAINTCLIPBOARD)
	X(WM_VSCROLLCLIPBOARD)
	X(WM_SIZECLIPBOARD)
	X(WM_ASKCBFORMATNAME)
	X(WM_CHANGECBCHAIN)
	X(WM_HSCROLLCLIPBOARD)
	X(WM_QUERYNEWPALETTE)
	X(WM_PALETTEISCHANGING)
	X(WM_PALETTECHANGED)
	X(WM_HOTKEY)
	X(WM_PRINT)
	X(WM_PRINTCLIENT)
	X(WM_APPCOMMAND)
	X(WM_THEMECHANGED)
	X(WM_CLIPBOARDUPDATE)
	X(WM_DWMCOMPOSITIONCHANGED)
	X(WM_DWMNCRENDERINGCHANGED)
	X(WM_DWMCOLORIZATIONCOLORCHANGED)
	X(WM_DWMWINDOWMAXIMIZEDCHANGE)
	X(WM_DWMSENDICONICTHUMBNAIL)
	X(WM_DWMSENDICONICLIVEPREVIEWBITMAP)
	X(WM_GETTITLEBARINFOEX)
	X(WM_HANDHELDFIRST)
	X(WM_HANDHELDLAST)
	X(WM_AFXFIRST)
	X(WM_AFXLAST)
	X(WM_PENWINFIRST)
	X(WM_PENWINLAST)
	X(WM_APP)
	X(WM_USER)
};
#undef X

string getmessagetext(int msgId) {
	int size = (sizeof(WindowsMessages) / sizeof(*WindowsMessages));
	for (int i = 0; i < size; i++) {
		if (msgId == WindowsMessages[i].msgid)
			return WindowsMessages[i].pname;
	}
	return "";
}

#pragma endregion

#pragma region getvktext

#define X(x) {x, #x},
struct VirtualKeyName {
	int vkcode;
	char* vkname;
} VirtualKeyNames[] =
{
	X(VK_LBUTTON)
	X(VK_RBUTTON)
	X(VK_CANCEL)
	X(VK_MBUTTON)
	X(VK_XBUTTON1)
	X(VK_XBUTTON2)
	X(VK_BACK)
	X(VK_TAB)
	X(VK_CLEAR)
	X(VK_RETURN)
	X(VK_SHIFT)
	X(VK_CONTROL)
	X(VK_MENU)
	X(VK_PAUSE)
	X(VK_CAPITAL)
	X(VK_KANA)
	X(VK_HANGUL)
	X(VK_JUNJA)
	X(VK_FINAL)
	X(VK_HANJA)
	X(VK_KANJI)
	X(VK_ESCAPE)
	X(VK_CONVERT)
	X(VK_NONCONVERT)
	X(VK_ACCEPT)
	X(VK_MODECHANGE)
	X(VK_SPACE)
	X(VK_PRIOR)
	X(VK_NEXT)
	X(VK_END)
	X(VK_HOME)
	X(VK_LEFT)
	X(VK_UP)
	X(VK_RIGHT)
	X(VK_DOWN)
	X(VK_SELECT)
	X(VK_PRINT)
	X(VK_EXECUTE)
	X(VK_SNAPSHOT)
	X(VK_INSERT)
	X(VK_DELETE)
	X(VK_HELP)
	X(VK_LWIN)
	X(VK_RWIN)
	X(VK_APPS)
	X(VK_SLEEP)
	X(VK_NUMPAD0)
	X(VK_NUMPAD1)
	X(VK_NUMPAD2)
	X(VK_NUMPAD3)
	X(VK_NUMPAD4)
	X(VK_NUMPAD5)
	X(VK_NUMPAD6)
	X(VK_NUMPAD7)
	X(VK_NUMPAD8)
	X(VK_NUMPAD9)
	X(VK_MULTIPLY)
	X(VK_ADD)
	X(VK_SEPARATOR)
	X(VK_SUBTRACT)
	X(VK_DECIMAL)
	X(VK_DIVIDE)
	X(VK_F1)
	X(VK_F2)
	X(VK_F3)
	X(VK_F4)
	X(VK_F5)
	X(VK_F6)
	X(VK_F7)
	X(VK_F8)
	X(VK_F9)
	X(VK_F10)
	X(VK_F11)
	X(VK_F12)
	X(VK_F13)
	X(VK_F14)
	X(VK_F15)
	X(VK_F16)
	X(VK_F17)
	X(VK_F18)
	X(VK_F19)
	X(VK_F20)
	X(VK_F21)
	X(VK_F22)
	X(VK_F23)
	X(VK_F24)
	X(VK_NUMLOCK)
	X(VK_SCROLL)
	X(VK_OEM_NEC_EQUAL)
	X(VK_OEM_FJ_JISHO)
	X(VK_OEM_FJ_MASSHOU)
	X(VK_OEM_FJ_TOUROKU)
	X(VK_OEM_FJ_LOYA)
	X(VK_OEM_FJ_ROYA)
	X(VK_LSHIFT)
	X(VK_RSHIFT)
	X(VK_LCONTROL)
	X(VK_RCONTROL)
	X(VK_LMENU)
	X(VK_RMENU)
	X(VK_BROWSER_BACK)
	X(VK_BROWSER_FORWARD)
	X(VK_BROWSER_REFRESH)
	X(VK_BROWSER_STOP)
	X(VK_BROWSER_SEARCH)
	X(VK_BROWSER_FAVORITES)
	X(VK_BROWSER_HOME)
	X(VK_VOLUME_MUTE)
	X(VK_VOLUME_DOWN)
	X(VK_VOLUME_UP)
	X(VK_MEDIA_NEXT_TRACK)
	X(VK_MEDIA_PREV_TRACK)
	X(VK_MEDIA_STOP)
	X(VK_MEDIA_PLAY_PAUSE)
	X(VK_LAUNCH_MAIL)
	X(VK_LAUNCH_MEDIA_SELECT)
	X(VK_LAUNCH_APP1)
	X(VK_LAUNCH_APP2)
	X(VK_OEM_1)
	X(VK_OEM_PLUS)
	X(VK_OEM_COMMA)
	X(VK_OEM_MINUS)
	X(VK_OEM_PERIOD)
	X(VK_OEM_2)
	X(VK_OEM_3)
	X(VK_OEM_4)
	X(VK_OEM_5)
	X(VK_OEM_6)
	X(VK_OEM_7)
	X(VK_OEM_8)
	X(VK_OEM_AX)
	X(VK_OEM_102)
	X(VK_ICO_HELP)
	X(VK_ICO_00)
	X(VK_PROCESSKEY)
	X(VK_ICO_CLEAR)
	X(VK_PACKET)
	X(VK_OEM_RESET)
	X(VK_OEM_JUMP)
	X(VK_OEM_PA1)
	X(VK_OEM_PA2)
	X(VK_OEM_PA3)
	X(VK_OEM_WSCTRL)
	X(VK_OEM_CUSEL)
	X(VK_OEM_ATTN)
	X(VK_OEM_FINISH)
	X(VK_OEM_COPY)
	X(VK_OEM_AUTO)
	X(VK_OEM_ENLW)
	X(VK_OEM_BACKTAB)
	X(VK_ATTN)
	X(VK_CRSEL)
	X(VK_EXSEL)
	X(VK_EREOF)
	X(VK_PLAY)
	X(VK_ZOOM)
	X(VK_NONAME)
	X(VK_PA1)
	X(VK_OEM_CLEAR)
};
#undef X

string getvktext(int vk) {
	int size = (sizeof(VirtualKeyNames) / sizeof(*VirtualKeyNames));
	for (int i = 0; i < size; i++) {
		if (vk == VirtualKeyNames[i].vkcode)
			return VirtualKeyNames[i].vkname;
	}
	return "";
}

#pragma endregion
