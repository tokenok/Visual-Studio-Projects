#include <fstream>
#include <algorithm>
#include <time.h>
#include <iostream>

#include "window.h"
#include "resource.h"
#include "common.h"
#include "ocr.h"
#include "d2funcs.h"
#include "hotkey.h"

#pragma comment(lib, "user32.lib")

using namespace std;

const TCHAR className[] = TEXT("Tokenhack");
const UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");
const UINT WM_customtrackbar_VALUE_CHANGED = RegisterWindowMessageA("custom trackbar value changed");
const UINT WM_customedit_TEXT_CHANGED = RegisterWindowMessageA("custom edit text changed");
const UINT WM_KBHOOKKEYDOWN = RegisterWindowMessageA("keyboard hook keydown message");

const TCHAR tabbuttonclassName[] = TEXT("tabbuttonclassName");
const TCHAR autoradiobuttonclassName[] = TEXT("autoradiobuttonclassName");
const TCHAR checkboxbuttonclassName[] = TEXT("checkboxbuttonclassName");
const TCHAR normalbuttonclassName[] = TEXT("normalbuttonclassName");
const TCHAR togglebuttonclassName[] = TEXT("togglebuttonclassName");
const TCHAR staticcontrolclassName[] = TEXT("staticcontrolclassName");
const TCHAR EditControlclassName[] = TEXT("EditControlclassName");
const TCHAR customtrackbarclassName[] = TEXT("customtrackbarclassName");
const TCHAR customcomboboxclassName[] = TEXT("customcomboboxclassName");
const TCHAR CustomContextMenuclassName[] = TEXT("CustomContextMenuclassName");
const TCHAR CustomScrollbarclassName[] = TEXT("CustomScrollbarclassName");

NOTIFYICONDATA g_notifyIconData;

#define WM_MOUSEENTER (WM_USER)

HWND g_hwnd;
HWND config;
HHOOK kbhook;
HHOOK mhook;
HWND statbox;
HCURSOR idle_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(IDLECURSOR));
HCURSOR press_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESSCURSOR));
HCURSOR loading_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(WAITCURSOR));
HCURSOR token_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(TOKENCURSOR));
HMENU g_menu;
MSG msg;
bool is_hotkey_dialog_visible = false;
bool is_config_dialog_visible = false;
bool toggleall = false;
bool is_drawing_static = false;
UINT volume = 0;
DWORD global_change_hotkey;
DWORD global_hook_key = 0;

vector<DWORD> hotkeys;

HBRUSH g_tempbrush = NULL;

bool move_window = false; //right click on control then use arrow keys to move, shift makes it move faster, ctrl + arrow keys resize the window
HWND lastwindow = NULL;

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

CustomControls custom_controls;

#pragma region BasicControl + colorscheme + hover: implementation
	#pragma region colorscheme
	BasicControl_colorscheme::BasicControl_colorscheme() {};

	//statics
	BasicControl_colorscheme::BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on) {
		this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
		this->background_hover_on = background_idle_on; this->border_hover_on = border_idle_on; this->text_hover_on = text_idle_on;
		//this->background_selected_on = background_idle_on; this->border_selected_on = border_idle_on; this->text_selected_on = text_idle_on;
		this->background_idle_off = background_idle_on; this->border_idle_off = border_idle_on; this->text_idle_off = text_idle_on;
		this->background_hover_off = background_idle_on; this->border_hover_off = border_idle_on; this->text_hover_off = text_idle_on;
		//this->background_selected_off = background_idle_on; this->border_selected_off = border_idle_on; this->text_selected_off = text_idle_on;
	}

	//buttons etc
	BasicControl_colorscheme::BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on)
		//,COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on);
	{

		this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
		this->background_hover_on = background_hover_on; this->border_hover_on = border_hover_on; this->text_hover_on = text_hover_on;
		//this->background_selected_on = background_selected_on; this->border_selected_on = border_selected_on; this->text_selected_on = text_selected_on;
		this->background_idle_off = background_idle_on; this->border_idle_off = border_idle_on; this->text_idle_off = text_idle_on;
		this->background_hover_off = background_hover_on; this->border_hover_off = border_hover_on; this->text_hover_off = text_hover_on;
		//this->background_selected_off = background_selected_on; this->border_selected_off = border_selected_on; this->text_selected_off = text_selected_on;
	}

	//buttons with toggle state
	BasicControl_colorscheme::BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on,
		//COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on,
		COLORREF background_idle_off, COLORREF border_idle_off, COLORREF text_idle_off,
		COLORREF background_hover_off, COLORREF border_hover_off, COLORREF text_hover_off)
		//COLORREF background_selected_off, COLORREF border_selected_off, COLORREF text_selected_off);
	{

		this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
		this->background_hover_on = background_hover_on; this->border_hover_on = border_hover_on; this->text_hover_on = text_hover_on;
		//this->background_selected_on = background_selected_on; this->border_selected_on = border_selected_on; this->text_selected_on = text_selected_on;
		this->background_idle_off = background_idle_off; this->border_idle_off = border_idle_off; this->text_idle_off = text_idle_off;
		this->background_hover_off = background_hover_off; this->border_hover_off = border_hover_off; this->text_hover_off = text_hover_off;
		//this->background_selected_off = background_selected_off; this->border_selected_off = border_selected_off; this->text_selected_off = text_selected_off;
	}
	#pragma endregion
	#pragma region BasicControl

	BasicControl* getBasicControl(HWND wnd) {
		LONG_PTR ptr = GetWindowLongPtr(wnd, GWL_USERDATA);
		return reinterpret_cast<BasicControl*>(ptr);
	}

	BasicControl::BasicControl() {}

	HWND BasicControl::Create(HWND parent) {
		handle = CreateWindowEx(window_exstyles, str_to_wstr(className).c_str(), str_to_wstr(window_text).c_str(), 
			window_styles, xpos, ypos, width, height,
			parent, (HMENU)id, NULL, this);
		if (owner_name > own_root_window) {
			for (auto & a : custom_controls.tabbuttons) {
				if (a->this_name == owner_name)
					a->page_windows.push_back(handle);
			}
		}		
		return handle;
	}

	void BasicControl::Show() {
		SetWindowLong(handle, GWL_STYLE, GetWindowLongPtr(handle, GWL_STYLE) | WS_VISIBLE);
		ShowWindow(handle, SW_SHOW);
		EnableWindow(handle, 1);
	}

	void BasicControl::Hide() {
		if (GetWindowLongPtr(handle, GWL_STYLE) & WS_VISIBLE)
			SetWindowLong(handle, GWL_STYLE, GetWindowLongPtr(handle, GWL_STYLE) ^ WS_VISIBLE);
		ShowWindow(handle, SW_HIDE);
		EnableWindow(handle, 0);
	}
	#pragma endregion
	#pragma region hover
	BasicControl_hover::BasicControl_hover() {}
	BasicControl_hover::BasicControl_hover(std::string hover_text, DWORD Flags, DWORD HoverTime) {
		this->hover_text = hover_text;
		this->Flags = Flags;
		this->HoverTime = HoverTime;
	}
	#pragma endregion
#pragma endregion

#pragma region color schemes, fonts, styles, hover info, ids
	#pragma region color schemes
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
	BasicControl_colorscheme togglebutton_scheme(
		RGB(10, 10, 10), RGB(0, 255, 0), RGB(0, 255, 0),			//IDLE ON: background/border/text
		RGB(45, 45, 45), RGB(0, 255, 0), RGB(0, 255, 0),			//HOVER ON: background/border/text
		RGB(10, 10, 10), RGB(255, 0, 0), RGB(255, 0, 0),			//IDLE OFF: background/border/text
		RGB(45, 45, 45), RGB(255, 0, 0), RGB(255, 0, 0)				//HOVER OFF: background/border/text
		);
	BasicControl_colorscheme normalbutton_scheme(
		RGB(10, 10, 10), RGB(0, 117, 255), RGB(0, 117, 255), 		//IDLE: background/border/text
		RGB(25, 25, 25), RGB(0, 117, 255), RGB(0, 117, 255)			//HOVER: background/border/text	
		);
	BasicControl_colorscheme stattab_scheme(
		RGB(0, 118, 255), RGB(0, 118, 255), RGB(10, 10, 10),		//IDLE ON: background/border/text
		RGB(0, 118, 255), RGB(0, 118, 255), RGB(10, 10, 10),		//HOVER ON: background/border/text
		RGB(10, 10, 10), RGB(0, 118, 255), RGB(0, 118, 255),		//IDLE OFF: background/border/text
		RGB(0, 118, 255), RGB(10, 10, 10), RGB(10, 10, 10)			//HOVER OFF: background/border/text
		);
	#pragma endregion
	#pragma region fonts
	HFONT Font_a = NULL;
	HFONT Font_ariel = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("ariel"));
	HFONT Font_consolas = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("consolas"));
	HFONT Font_courier = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("courier"));
	#pragma endregion
	#pragma region styles
	LONG button_styles = WS_VISIBLE | WS_CHILD;
	#pragma endregion
	#pragma region hover_info
	BasicControl_hover default_hover("", TME_LEAVE, 0);
	BasicControl_hover tooltip_hover("default hover tooltip", TME_LEAVE | TME_HOVER, 0);
	#pragma endregion
	#pragma region ids
	int window_id = 1000;
	int cb_id = 2000;
	int st_id = 3000;
	int con_tbtn_id = 4000;
	int con_e_id = 5000;
	int con_s_id = 6000;
	int con_c_id = 7000;
	#pragma endregion
#pragma endregion

#pragma region tokenhack function window position and size info

int totalonfunctions = 0;

int tokenhackfunctionwindowxposoffset = 5;

int windowwidth = 400;
int functionnameyoffsum = 0;

int functionnameheight = 16;
int functionnamewidth = 180;

int tabbuttonheight = functionnameheight;

int 15 = 15;
int checkboxheight = 15;

int buttonwidth = 150;
int buttonheight = functionnameheight;

int checkboxxoff = 0 + tokenhackfunctionwindowxposoffset;
int buttonxoff = checkboxwidth + tokenhackfunctionwindowxposoffset + 1;
int functionnamexoff = 180 + tokenhackfunctionwindowxposoffset;
int statboxxoff = 370 + tokenhackfunctionwindowxposoffset;

int bottomheight = 135;

#pragma endregion

///////////

#pragma region TabButton class

TabButton::TabButton() {}
TabButton::TabButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT this_name, UINT group_name, bool add_to_list) {
	className = wstr_to_str(tabbuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->owner_name = owner_name; this->this_name = this_name; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK tabbuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	TabButton* a;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<TabButton*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {		
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<TabButton*>(ptr);
	}
		
	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {			
			break;
		}
		case MY_COMMAND:
		case WM_LBUTTONUP: {
			for (auto & tabbtn : custom_controls.tabbuttons) {
				if (a->group_name == tabbtn->group_name) {
					if (tabbtn->toggle_state) {
						for (auto & b : tabbtn->page_windows) {
							ShowWindow(b, SW_HIDE);
							EnableWindow(b, 0);
							if (GetWindowLongPtr(b, GWL_STYLE) & WS_VISIBLE)
								SetWindowLongPtr(b, GWL_STYLE, GetWindowLongPtr(b, GWL_STYLE) ^ WS_VISIBLE);
							RedrawWindow(b, NULL, NULL, RDW_INVALIDATE);
						}
					}
					tabbtn->toggle_state = 0;
					RedrawWindow(tabbtn->handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
			a->toggle_state = 1;
			for (auto & b : a->page_windows) {
				ShowWindow(b, SW_SHOW);
				EnableWindow(b, 1);
				SetWindowLongPtr(b, GWL_STYLE, GetWindowLongPtr(b, GWL_STYLE) | WS_VISIBLE);
				RedrawWindow(b, NULL, NULL, RDW_INVALIDATE);
			}

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;				
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{			
			a->page_windows.clear();
			a->mouse_in_client = false;
			a->focus = false;

			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_tabbutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = tabbuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = tabbuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM tabbtn = init_register_custom_tabbutton();

#pragma endregion

#pragma region RadioButton class

RadioButton::RadioButton() {}
RadioButton::RadioButton(string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam), int group,
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name, bool add_to_list) {
	className = wstr_to_str(autoradiobuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->group = group;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->owner_name = owner_name; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK autoradiobuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RadioButton* a;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<RadioButton*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<RadioButton*>(ptr);
	}
	
	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {			
			break;
		}
		case WM_LBUTTONUP: {
			int group = a->group;
			UINT group_name = a->group_name;
			UINT owner_name = a->owner_name;
			vector<RadioButton*> autoradiobuttons = custom_controls.autoradiobuttons;
			for (auto & b : autoradiobuttons) {
				if (group == 0)
					break;
				if (b->group == group && b->group_name == group_name && b->owner_name == owner_name) {
					b->toggle_state = 0;
					RedrawWindow(b->handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
			a->toggle_state = 1;

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;				
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_autoradiobutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = autoradiobuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = autoradiobuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM rbtn = init_register_custom_autoradiobutton();

#pragma endregion

#pragma region CheckBox class

CheckBox::CheckBox() {}
CheckBox::CheckBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name, bool add_to_list) {
	className = wstr_to_str(checkboxbuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->owner_name = owner_name; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK checkboxbuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CheckBox* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CheckBox*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CheckBox*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;			
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->toggle_state)
				a->toggle_state = 0;
			else
				a->toggle_state = 1;

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_checkboxbutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = checkboxbuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = checkboxbuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM cbtn = init_register_custom_checkboxbutton();

#pragma endregion

#pragma region Button class

Button::Button() {}
Button::Button(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT owner_name, UINT group_name, bool add_to_list) {
	className = wstr_to_str(normalbuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->owner_name = owner_name; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK normalbuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Button* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<Button*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<Button*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = (a->window_text);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return FALSE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_normalbutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = normalbuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = normalbuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM btn = init_register_custom_normalbutton();

#pragma endregion

#pragma region ToggleButton class

ToggleButton::ToggleButton() {}
ToggleButton::ToggleButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	std::string string_in_file,
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT owner_name, UINT group_name, bool add_to_list) {
	className = wstr_to_str(togglebuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->string_in_file = string_in_file;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->owner_name = owner_name; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK togglebuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	ToggleButton* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<ToggleButton*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<ToggleButton*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;			
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			int state;
			string str = changeconfigonstate(a->string_in_file);
			if (str.size() == 0) 
				state = a->toggle_state ? 0 : 1;
			else
				state = str_to_int(str);
			a->toggle_state = state;

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_togglebutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = togglebuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = togglebuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM tbtn = init_register_custom_togglebutton();

#pragma endregion

#pragma region StaticControl class

StaticControl::StaticControl() {}
StaticControl::StaticControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT owner_name, UINT group_name, bool add_to_list) {
	className = wstr_to_str(staticcontrolclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->hover_info = hover_info; this->owner_name = owner_name; this->group_name = group_name;
	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);

	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK staticcontrolProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	StaticControl* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<StaticControl*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<StaticControl*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			if (GetWindowLongPtr(hwnd, GWL_STYLE) & SS_CENTER)
				ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			else
				ExtTextOut(hDC, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);

			if (a->focus) {
				HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
				FrameRect(hDC, &rcclient, framebrush);
				DeleteObject(framebrush);
			}

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = a->hover_info.Flags;
				me.hwndTrack = a->handle;
				me.dwHoverTime = a->hover_info.HoverTime;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSEHOVER:{
			SIZE size;
			HDC hDC = GetDC(hwnd);
			SelectObject(hDC, a->font);
			SetMapMode(hDC, MM_TEXT);
			a->hover_info.hover_text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(a->hover_info.hover_text).c_str(), a->hover_info.hover_text.length(), &size);
			if (size.cx <= a->width)
				break;

			POINT pt = getclientcursorpos(GetParent(hwnd));

			StaticControl* temp = new StaticControl;
			temp->window_exstyles = WS_EX_TOPMOST;
			temp->className = wstr_to_str(staticcontrolclassName);
			temp->window_text = a->window_text;
			temp->window_styles = WS_CHILD | WS_VISIBLE | SS_CENTER;
			temp->xpos = pt.x + 20;
			temp->ypos = pt.y;
			temp->width = size.cx;
			temp->height = size.cy;
			temp->id = window_id++;
			temp->client_cursor = NULL;
			temp->font = Font_a;
			temp->Proc = nullptr;
			temp->color_scheme = tokenhackfunctionstatic_scheme;

			a->hover_info.hover_text = a->window_text;

			a->hover_info.handle = temp->Create(GetParent(hwnd));

			ReleaseDC(hwnd, hDC);
			break;
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			if (a->hover_info.handle)
				DestroyWindow(a->hover_info.handle);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			a->focus = false;
			a->mouse_in_client = false;

			a->handle = NULL;
			
			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_staticcontrol() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = staticcontrolclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = staticcontrolProc;
	return RegisterClassEx(&wnd);
}
ATOM stc = init_register_custom_staticcontrol();

#pragma endregion

#pragma region CustomTrackbar class

#define HTTHUMB (WM_USER + 1)

//TrackbarColorScheme c_scheme {
//	RGB(10, 10, 10), RGB(50, 50, 50), RGB(255, 0, 0), RGB(0, 0, 0),									// background/border/window text idle/highlight
//	RGB(127, 127, 127), RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255), 					// left/right channel left/right highlight
//	RGB(10, 10, 10), RGB(10, 10, 10), RGB(0, 118, 255),												// thumb background: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb border: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(10, 10, 10)											// thumb text: idle/hover/selected
//};

CustomTrackbar::CustomTrackbar(string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
	int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
	TrackbarColorScheme color_scheme, HCURSOR client_cursor, HCURSOR thumb_cursor, UINT owner_name, UINT group_name/*= 0*/, bool add_to_list) {

	className = wstr_to_str(customtrackbarclassName); handle = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;

	this->window_name = window_name; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
	this->small_step = small_step; this->large_step = large_step; this->thumb_size = thumb_size; this->channel_size = channel_size; this->flags = flags;
	this->tcolor_scheme = color_scheme; this->client_cursor = client_cursor; this->thumb_cursor = thumb_cursor;
	this->owner_name = owner_name; this->group_name = group_name;
	
	if (this->client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (this->thumb_cursor == NULL)
		this->thumb_cursor = LoadCursor(NULL, IDC_HAND);

	if (add_to_list)
		custom_controls.addControl(this);
}

int CustomTrackbar::setPos(int pos) {
	int x = flags & CTB_VERT ? height : width;
	return this->pos = pos <= 0 ? 0 : (int)(x - thumb_size) <= pos ? (x - thumb_size) : pos;
}

int CustomTrackbar::setVal(int pos) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	return current_val = ((pos * (max_val - min_val)) / (x - thumb_size == 0 ? 1 : (x - thumb_size))) + min_val;
}

int CustomTrackbar::setPosWithVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	return pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val);
}

int CustomTrackbar::moveThumb(int pos) {
	return setVal(setPos(pos));
}

void CustomTrackbar::moveThumbReal(int pos) {
	setPosWithVal(moveThumb(pos));
}

void CustomTrackbar::thumbHitText() {
	if (is_cursor_in_region(thumb_region, getclientcursorpos(handle))) {
		if (!thumb_hover) {
			thumb_hover = true;
			InvalidateRect(handle, &thumb_region, true);
		}
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTTHUMB, NULL));
	}
	else if (thumb_hover) {
		thumb_hover = false;
		InvalidateRect(handle, &thumb_region, true);
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTCLIENT, NULL));
	}
}

void CustomTrackbar::setFocus(bool focus) {
	this->focus = focus;
	if (focus) {
		SetFocus(handle);
		if (dragging)
			SetCapture(handle);
	}
	else {
		ReleaseCapture();
		//setfocus to new window here		
	}

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool CustomTrackbar::setRange(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	setVal(pos);

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
	return true;
}

void CustomTrackbar::redrawNewVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val == 0 ? 1 : (max_val - min_val));

	if (scrollbar_owner_handle)
		PostMessage(scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);
	PostMessage(GetParent(handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);
	RedrawWindow(handle, &tr, NULL, RDW_INVALIDATE);
}

LRESULT CALLBACK customtrackbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//tab focus

	CustomTrackbar* a;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomTrackbar*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomTrackbar*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			TrackbarColorScheme cs = a->tcolor_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//colorbackground
			HBRUSH tempbrush0 = CreateSolidBrush(cs.background);
			FillRect(hDC, &rcclient, tempbrush0);

			//calculate thumb position
			if (a->start) {
				a->setPosWithVal(a->start_val);
				a->start = false;
			}
			a->thumb_region.left = (a->flags & CTB_VERT ? rcclient.left + 1: rcclient.left + a->pos);
			a->thumb_region.top = (a->flags & CTB_VERT ? rcclient.top + a->pos : rcclient.top + 1);
			a->thumb_region.right = (a->flags & CTB_VERT ? rcclient.right - 1 : rcclient.left + a->thumb_size + a->pos);
			a->thumb_region.bottom = (a->flags & CTB_VERT ? rcclient.left + a->thumb_size + a->pos : rcclient.bottom - 1);

			//draw channel
			RECT rc_channel_left;
			RECT rc_channel_right;

			//left/top
			rc_channel_left.left = (a->flags & CTB_VERT ? rcclient.left + a->channel_size : rcclient.left);
			rc_channel_left.right = (a->flags & CTB_VERT ? rcclient.right - a->channel_size : rcclient.left + a->pos);
			rc_channel_left.top = (a->flags & CTB_VERT ? rcclient.top : rcclient.top + a->channel_size);
			rc_channel_left.bottom = (a->flags & CTB_VERT ? rcclient.top + a->pos : rcclient.bottom - a->channel_size);
			//right/bottom
			rc_channel_right.left = (a->flags & CTB_VERT ? rcclient.left + a->channel_size : rcclient.left + a->thumb_size + a->pos);
			rc_channel_right.right = (a->flags & CTB_VERT ? rcclient.right - a->channel_size : rcclient.right);
			rc_channel_right.top = (a->flags & CTB_VERT ? rcclient.top + a->pos : rcclient.top + a->channel_size);
			rc_channel_right.bottom = (a->flags & CTB_VERT ? rcclient.bottom : rcclient.bottom - a->channel_size);

			HBRUSH tempbrush1 = CreateSolidBrush(a->mouse_in_client ? cs.left_channel_highlight : cs.left_channel_idle);
			HBRUSH tempbrush2 = CreateSolidBrush(a->mouse_in_client ? cs.right_channel_highlight : cs.right_channel_idle);
			FillRect(hDC, &rc_channel_left, tempbrush1);
			FillRect(hDC, &rc_channel_right, tempbrush2);

			//draw focus rect
			if ((a->focus || a->flags & CTB_BORDER) && !(a->flags & CTB_NOBORDER)) {
				SetDCPenColor(hDC, cs.border);
				Rectangle(hDC, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);
			}

			//draw thumb
			a->thumbHitText();
			SIZE size;
			string text = a->flags & CTB_THUMBVALUE ? int_to_str(a->current_val) : "";
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), (int)text.length(), &size);
			SetTextColor(hDC, a->thumb_selected ? cs.thumb_text_selected : a->thumb_hover ? cs.thumb_text_hover : cs.thumb_text_idle);
			SetBkColor(hDC, a->thumb_selected ? cs.thumb_background_selected : a->thumb_hover ? cs.thumb_background_hover : cs.thumb_background_idle);
			ExtTextOut(hDC,
				((a->thumb_region.right - a->thumb_region.left) - size.cx) / 2 + (a->flags & CTB_VERT ? 0 : a->pos),
				((a->thumb_region.bottom - a->thumb_region.top) - size.cy) / 2 + (a->flags & CTB_VERT ? a->pos : 1),
				ETO_OPAQUE | ETO_CLIPPED, &a->thumb_region, str_to_wstr(text).c_str(), text.length(), NULL);
			SetDCPenColor(hDC, a->thumb_selected ? cs.thumb_border_selected : a->thumb_hover || a->dragging ? cs.thumb_border_hover : cs.thumb_border_idle);			
			Rectangle(hDC, a->thumb_region.left, a->thumb_region.top, a->thumb_region.right, a->thumb_region.bottom);

			//draw window name
			if (a->window_name.size() > 0) {
				SetBkMode(hDC, TRANSPARENT);
				SetTextColor(hDC, a->mouse_in_client ? cs.window_name_highlight : cs.window_name_idle);
				RECT rc = getclientrect(a->handle);
				DrawText(hDC, str_to_wstr(a->window_name).c_str(), a->window_name.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			//apply paint
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(tempbrush0);
			DeleteObject(tempbrush1);
			DeleteObject(tempbrush2);
			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			a->focus = true;
			a->mouse_in_client = true;
			a->dragging = true;
			a->thumb_hover = true;
			a->thumb_selected = true;

			a->setFocus(true);

			POINT pt = getclientcursorpos(a->handle);

			if (a->flags & CTB_SMOOTH)//smooth dragging
				a->moveThumb((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));
			else
				a->moveThumbReal((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));

			customtrackbarProc(a->handle, WM_SETCURSOR, (WPARAM)a->handle, (LPARAM)MAKELONG(HTTHUMB, NULL));

			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP: {
			a->dragging = false;
			a->thumb_selected = false;
			ReleaseCapture();
			if (!(a->flags & CTB_SMOOTH) || !(a->flags & CTB_STAY))//not smooth dragging OR not smooth stay
				a->setPosWithVal(a->current_val);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			a->CustomTrackbar::thumbHitText();

			if (a->dragging) {

				POINT pt = getclientcursorpos(a->handle);

				if (a->flags & CTB_SMOOTH)//smooth dragging
					a->moveThumb((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));
				else
					a->moveThumbReal((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));

				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_LEFT:{
					a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_DOWN:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_RIGHT:{
					a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_UP:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_HOME:{
					a->setPosWithVal(a->min_val);
					break;
				}
				case VK_END:{
					a->setPosWithVal(a->max_val);
					break;
				}
				case VK_PRIOR:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
					break;
				}
				case VK_NEXT:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
			if (!wrongkey_skip) {
				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
				}
				else {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
				}
				else {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
				}
			}
			else
				break;
			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_SIZE:{
			a->width = LOWORD(lParam);
			a->height = HIWORD(lParam);

			break;
		}
		case WM_MOVE:{
			a->xpos = LOWORD(lParam);
			a->ypos = HIWORD(lParam);

			break;
		}
		case WM_SETCURSOR:{
			if (a->dragging)
				lParam = MAKELPARAM(HTTHUMB, HIWORD(lParam));
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
				case HTTHUMB:
					SetCursor(a->thumb_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_KILLFOCUS:{
			a->setFocus(false);

			a->mouse_in_client = false;
			a->dragging = false;
			a->thumb_hover = false;
			a->thumb_selected = false;

			break;
		}
		case WM_MOUSELEAVE:{
			if (!a->dragging) {
				a->mouse_in_client = false;
				a->thumb_selected = false;
				a->thumb_hover = false;

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_customtrackbar() {
	WNDCLASS wnd = {0};
	wnd.lpszClassName = customtrackbarclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customtrackbarProc;
	return RegisterClass(&wnd);
}
ATOM ctb = init_register_customtrackbar();

#pragma endregion

#pragma region CustomComboBox class

CustomComboBoxOption::CustomComboBoxOption() {}
CustomComboBoxOption::CustomComboBoxOption(string text) {
	hover = false; region = {0, 0, 0, 0};
	this->text = text;
}

CustomComboBox::CustomComboBox() {}
CustomComboBox::CustomComboBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id,
	vector<string> options_text, UINT arrow_region_width, UINT max_options, UINT style, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT owner_name, UINT group_name, bool add_to_list) {

	className = wstr_to_str(customcomboboxclassName); handle = NULL; toggle_state = true;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->owner_name = owner_name; this->group_name = group_name;

	this->style = style;
	if (style == ccb_style0)
		arrow_region_width = 0;
	this->arrow_region_width = arrow_region_width;
	this->max_options = max_options;

	this->box_region = {0, 0, (LONG)width - (LONG)arrow_region_width, (LONG)height};
	this->arrow_region = { (LONG)width - (LONG)arrow_region_width, 0, (LONG)width, (LONG)height};
	this->arrow_region_up = { (LONG)width - (LONG)arrow_region_width, 0, (LONG)width, (LONG)height / 2};
	this->arrow_region_down = { (LONG)width - (LONG)arrow_region_width, (LONG)height / 2, width, (LONG)height};
	
	for (UINT i = 0; i < options_text.size(); i++) {
		CustomComboBoxOption option(options_text[i]);
		option.region = {0, (LONG)(i + 1) * (LONG)height, (LONG)width, (LONG)(i + 2) * (LONG)height};
		options.push_back(option);
	}

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

void CustomComboBox::setFocus(bool focus) {
	if (focus) {
		SetFocus(handle);
		SetCapture(handle);
	}
	else {
		ReleaseCapture();
	}

	this->focus = focus;
	is_expanded = focus;
}

void CustomComboBox::display_options(bool show) {
	is_expanded = show;

	MoveWindow(handle, xpos, ypos, width, show ? height * (options.size() + 1) : height, !show);

	setFocus(show);
}

void CustomComboBox::change_option_hover(int pos_change, bool wrap) {
	int newpos = hovered_option + pos_change;
	hovered_option = pos_change > 0 ? newpos > (int)options.size() - 1 ? wrap ? 0 : options.size() - 1 : newpos : pos_change < 0 ? newpos < 0 ? wrap ? options.size() - 1 : 0 : newpos : selected_option;
	for (UINT i = 0; i < options.size(); i++)
		options[i].hover = false;
	options[hovered_option].hover = true;
	selected_option = hovered_option;
}

LRESULT CALLBACK customcomboboxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//display up
	//scroll bar
	//draw outside client

	CustomComboBox* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomComboBox*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);		
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomComboBox*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//paint box region
			SIZE size;
			a->window_text = a->options.at(a->selected_option).text;
			string text = (a->window_text);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->box_hover || a->is_expanded ? cs.background_hover_on : cs.background_idle_on : a->box_hover || a->is_expanded ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->box_hover || a->is_expanded ? cs.text_hover_on : cs.text_idle_on : a->box_hover || a->is_expanded ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((a->box_region.right - a->box_region.left) - size.cx) / 2, ((a->box_region.bottom - a->box_region.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &a->box_region, str_to_wstr(text).c_str(), text.length(), NULL);

			//paint arrow region
			if (a->style == ccb_style1 || a->style == ccb_style2) {
				HBRUSH tempbrush = CreateSolidBrush(a->toggle_state ? a->arrow_hover || a->is_expanded ? cs.background_hover_on : cs.background_idle_on : a->arrow_hover || a->is_expanded ? cs.background_hover_off : cs.background_idle_off);
				FillRect(hDC, &a->arrow_region, tempbrush);
				DeleteObject(tempbrush);
				SetDCPenColor(hDC, a->toggle_state ? a->arrow_hover || a->is_expanded ? cs.text_hover_on : cs.text_idle_on : a->arrow_hover || a->is_expanded ? cs.text_hover_off : cs.text_idle_off);

				RECT temprect = a->arrow_region;
				temprect.left = a->box_region.right;
				int left = ((temprect.right - temprect.left) - 5) / 2 + a->box_region.right;
				int top = ((temprect.bottom - temprect.top) - 3) / 2;

				POINT points[6];
				points[0] = {left + 0, top + 0};
				points[1] = {left + 2, top + 2};
				points[2] = {left + 4, top + 0};
				points[3] = {left + 3, top + 0};
				points[4] = {left + 2, top + 1};
				points[5] = {left + 1, top + 0};
				Polygon(hDC, points, 6);			
			}
			if (a->style == ccb_style3) {
				HBRUSH arrowup = CreateSolidBrush(a->toggle_state ? a->arrow_hover_up ? cs.background_hover_on : cs.background_idle_on : a->arrow_hover_up ? cs.background_hover_off : cs.background_idle_off);
				HBRUSH arrowdo = CreateSolidBrush(a->toggle_state ? a->arrow_hover_down ? cs.background_hover_on : cs.background_idle_on : a->arrow_hover_down ? cs.background_hover_off : cs.background_idle_off);
				FillRect(hDC, &a->arrow_region_up, arrowup);
				FillRect(hDC, &a->arrow_region_down, arrowdo);
				DeleteObject(arrowup);
				DeleteObject(arrowdo);
				
				SetDCPenColor(hDC, a->toggle_state ? a->arrow_hover_up ? cs.text_hover_on : cs.text_idle_on : a->arrow_hover_up ? cs.text_hover_off : cs.text_idle_off);
				RECT temprect = a->arrow_region_up;
				temprect.left = a->box_region.right;
				int left = ((temprect.right - temprect.left) - 5) / 2 + a->box_region.right;
				int top = ((temprect.bottom - temprect.top) - 3) / 2 + a->arrow_region_up.top;
				POINT points[6];
				points[0] = {left + 0, top + 2};
				points[1] = {left + 2, top + 0};
				points[2] = {left + 4, top + 2};
				points[3] = {left + 3, top + 2};
				points[4] = {left + 2, top + 1};
				points[5] = {left + 1, top + 2};
				Polygon(hDC, points, 6);

				SetDCPenColor(hDC, a->toggle_state ? a->arrow_hover_down ? cs.text_hover_on : cs.text_idle_on : a->arrow_hover_down ? cs.text_hover_off : cs.text_idle_off);
				temprect = a->arrow_region_down;
				temprect.left = a->box_region.right;
				left = ((temprect.right - temprect.left) - 5) / 2 + a->box_region.right;
				top = ((temprect.bottom - temprect.top) - 3) / 2 + a->arrow_region_down.top;
				points[0] = {left + 0, top + 0};
				points[1] = {left + 2, top + 2};
				points[2] = {left + 4, top + 0};
				points[3] = {left + 3, top + 0};
				points[4] = {left + 2, top + 1};
				points[5] = {left + 1, top + 0};
				Polygon(hDC, points, 6);
			}

			//paint options
			if (a->is_expanded) {
				for (UINT i = 0; i < a->options.size(); i++) {
					CustomComboBoxOption* b = &a->options[i];
					string text = (b->text);
					GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(hDC, b->toggle_state ? b->hover ? cs.background_hover_on : cs.background_idle_on : b->hover ? cs.background_hover_off : cs.background_idle_off);
					SetTextColor(hDC, b->toggle_state ? b->hover ? cs.text_hover_on : cs.text_idle_on : b->hover ? cs.text_hover_off : cs.text_idle_off);					
					ExtTextOut(hDC, ((b->region.right - b->region.left) - size.cx) / 2, (((b->region.bottom - b->region.top) - size.cy) / 2) + (a->height * (i + 1)) , ETO_OPAQUE | ETO_CLIPPED, &b->region, str_to_wstr(text).c_str(), text.length(), NULL);
				}
			}

			//paint borders
			COLORREF framecol = (a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			HBRUSH framebrush = CreateSolidBrush(RGB(GetRValue(framecol), GetGValue(framecol), GetBValue(framecol)));
			SetDCPenColor(hDC, framecol);
			FrameRect(hDC, &rcclient, framebrush);
			if (a->is_expanded) {//outline box when expanded
				MoveToEx(hDC, 0, a->height - 1, NULL);
				LineTo(hDC, a->width, a->height - 1);
			}
			if (a->style == ccb_style2) {//draw divider line for box and arrow region
				MoveToEx(hDC, a->box_region.right, 3, NULL);
				LineTo(hDC, a->box_region.right, a->height - 3);
			}
			if (a->style == ccb_style3) {//draw divider lines for box and up+down arrows
				MoveToEx(hDC, a->box_region.right, 1, NULL);
				LineTo(hDC, a->box_region.right, a->height - 1);

				MoveToEx(hDC, a->arrow_region.left, a->arrow_region.bottom / 2, NULL);
				LineTo(hDC, a->arrow_region.right, a->arrow_region.bottom / 2);
			}
			DeleteObject(framebrush);

			//bit-block transfer
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			SetFocus(WindowFromPoint(getcursorpos()));
			if (is_cursor_in_region(a->box_region, getclientcursorpos(a->handle))) {
				if (a->style == ccb_style1 || a->style == ccb_style3) {
					a->change_option_hover(0);
					a->display_options(!a->is_expanded);
				}
				if (a->style == ccb_style0 || a->style == ccb_style2) {
					a->change_option_hover(1, true);
				}
			}
			else if (is_cursor_in_region(a->arrow_region, getclientcursorpos(a->handle))) {
				if (a->style == ccb_style1 || a->style == ccb_style2) {
					a->change_option_hover(0);
					a->display_options(!a->is_expanded);
				}
				else if (a->style == ccb_style3) {
					if (is_cursor_in_region(a->arrow_region_up, getclientcursorpos(a->handle))) {
						a->hovered_option = a->selected_option;
						a->change_option_hover(-1);
					}
					else if (is_cursor_in_region(a->arrow_region_down, getclientcursorpos(a->handle))) {
						a->hovered_option = a->selected_option;
						a->change_option_hover(1);
					}
				}
			}
			else if (a->is_expanded) {
				for (UINT i = 0; i < a->options.size(); i++) {
					if (is_cursor_in_region(a->options[i].region, getclientcursorpos(a->handle))) {
						a->selected_option = i;
						a->change_option_hover(0);
						a->display_options(false);
					}
				}				
			}
			if (!is_cursor_in_region(getclientrect(a->handle), getclientcursorpos(a->handle))) {
				a->display_options(false);
			}

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			if (is_cursor_in_region(a->box_region, getclientcursorpos(a->handle))) {
				bool orig_state = a->box_hover;
				a->box_hover = true;
				a->arrow_hover = (a->style == ccb_style1) ? true : false;
				a->arrow_hover_up = false;
				a->arrow_hover_down = false;
				if (!orig_state)
					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else if (is_cursor_in_region(a->arrow_region, getclientcursorpos(a->handle))) {
				bool orig_state = a->arrow_hover;
				a->arrow_hover = true;
				a->box_hover = (a->style == ccb_style1) ? true : false;
				if (a->style == ccb_style3) {
					a->arrow_hover = false;
					if (is_cursor_in_region(a->arrow_region_up, getclientcursorpos(a->handle))) {
						a->arrow_hover_up = true;
						a->arrow_hover_down = false;
					}
					else if (is_cursor_in_region(a->arrow_region_down, getclientcursorpos(a->handle))) {
						a->arrow_hover_down = true;
						a->arrow_hover_up = false;
					}
				}
				if (!orig_state)
					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else if (a->is_expanded && is_cursor_in_region(getclientrect(a->handle), getclientcursorpos(a->handle))) {
				if (a->arrow_hover_up || a->arrow_hover_down) {
					a->arrow_hover_up = false;
					a->arrow_hover_down = false;
					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
				}
				for (UINT i = 0; i < a->options.size(); i++) {
					bool orig_state = a->options[i].hover;
					a->options[i].hover = false;
					if (is_cursor_in_region(a->options[i].region, getclientcursorpos(a->handle)) || getclientcursorpos(a->handle).y == a->options[i].region.bottom) {
						a->options[i].hover = true;
						a->hovered_option = i;
					}
					if (orig_state != a->options[i].hover)
						RedrawWindow(a->handle, &a->options[i].region, NULL, RDW_INVALIDATE);
				}
			}

			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_RETURN:{
					a->selected_option = a->hovered_option;
					//fall through
				}
				case VK_ESCAPE:{
					if (a->is_expanded)
						a->display_options(false);

					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
					break;
				}
				case VK_RIGHT:
				case VK_DOWN:{
					a->change_option_hover(1);
					break;
				}					
				case VK_LEFT:
				case VK_UP:{
					a->change_option_hover(-1);
					break;
				}
				case VK_HOME:{
					a->selected_option = 0;
					a->change_option_hover(0);
					break;
				}
				case VK_END:{
					a->selected_option = a->options.size() - 1;
					a->change_option_hover(0);
					break;
				}
				case VK_PRIOR:{
					a->change_option_hover(-1);
					break;
				}
				case VK_NEXT:{
					a->change_option_hover(1);
					break;
				}
				default:
					wrongkey_skip = true;
			}
			if (!wrongkey_skip) 
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOVE:
		case WM_SIZE:{
			RECT mappedclient = getmappedclientrect(a->handle, GetParent(a->handle));
			RECT client = getclientrect(a->handle);

			a->xpos = mappedclient.left;
			a->ypos = mappedclient.top;
			a->width = client.right;
			a->height = a->is_expanded ? client.bottom / (a->options.size() + 1) : client.bottom;
			
			a->box_region = {0, 0, (LONG)a->width - (LONG)a->arrow_region_width, (LONG)a->height};
			a->arrow_region = { (LONG)a->width - (LONG)a->arrow_region_width, 0, (LONG)a->width, (LONG)a->height};
			a->arrow_region_up = {a->width - (LONG)a->arrow_region_width, 0, a->width, a->height / 2};
			a->arrow_region_down = {a->width - (LONG)a->arrow_region_width, a->height / 2, a->width, a->height};

			for (UINT i = 0; i < a->options.size(); i++) 
				a->options[i].region = {0, (LONG)(i + 1) * (LONG)a->height, (LONG)a->width, (LONG)(i + 2) * (LONG)a->height};

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			a->arrow_hover = false;
			a->box_hover = false;
			a->arrow_hover_up = false;
			a->arrow_hover_down = false;

			for (auto b : a->options) 
				b.hover = false;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			a->display_options(false);

			break;
		}
		case WM_DESTROY:{
			a->display_options(false);

			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_customcombobox() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = customcomboboxclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customcomboboxProc;
	return RegisterClassEx(&wnd);
}
ATOM ccb = init_register_customcombobox();

#pragma endregion

#pragma region CustomScrollbar class

CustomScrollbar::CustomScrollbar(string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
	int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
	ScrollbarColorScheme scolor_scheme, HCURSOR client_cursor, HCURSOR thumb_cursor, UINT owner_name, UINT group_name/*= 0*/, bool add_to_list) {

	className = wstr_to_str(customtrackbarclassName); handle = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;

	this->window_name = window_name; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
	this->small_step = small_step; this->large_step = large_step; this->thumb_size = thumb_size; this->channel_size = channel_size; this->flags = flags;
	this->scolor_scheme = scolor_scheme; this->client_cursor = client_cursor; this->thumb_cursor = thumb_cursor;
	this->owner_name = owner_name; this->group_name = group_name;

	if (this->client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (this->thumb_cursor == NULL)
		this->thumb_cursor = LoadCursor(NULL, IDC_HAND);

	if (add_to_list)
		custom_controls.addControl(this);
}

int CustomScrollbar::setPos(int pos) {
	int x = !(flags & CSB_HORZ) ? height : width;
	return this->pos = pos <= 0 ? 0 : (int)(x - thumb_size) <= pos ? (x - thumb_size) : pos;
}

int CustomScrollbar::setVal(int pos) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = !(flags & CSB_HORZ) ? height : width;
	return current_val = ((pos * (max_val - min_val)) / (x - thumb_size == 0 ? 1 : (x - thumb_size))) + min_val;
}

int CustomScrollbar::setPosWithVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = !(flags & CSB_HORZ) ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	return pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val);
}

int CustomScrollbar::moveThumb(int pos) {
	return setVal(setPos(pos));
}

void CustomScrollbar::moveThumbReal(int pos) {
	setPosWithVal(moveThumb(pos));
}

void CustomScrollbar::thumbHitText() {
	if (is_cursor_in_region(thumb_region, getclientcursorpos(handle))) {
		if (!thumb_hover) {
			thumb_hover = true;
			InvalidateRect(handle, &thumb_region, true);
		}
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTTHUMB, NULL));
	}
	else if (thumb_hover) {
		thumb_hover = false;
		InvalidateRect(handle, &thumb_region, true);
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTCLIENT, NULL));
	}
}

void CustomScrollbar::setFocus(bool focus) {
	this->focus = focus;
	if (focus) {
		SetFocus(handle);
		if (dragging)
			SetCapture(handle);
	}
	else {
		ReleaseCapture();
		//setfocus to new window here		
	}

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool CustomScrollbar::setRange(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	setVal(pos);

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
	return true;
}

void CustomScrollbar::redrawNewVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = !(flags & CSB_HORZ) ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val == 0 ? 1 : (max_val - min_val));

	/*if (scrollbar_owner_handle)
		PostMessage(scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);*/
	PostMessage(GetParent(handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);
	RedrawWindow(handle, &tr, NULL, RDW_INVALIDATE);
}

LRESULT CALLBACK CustomScrollbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CustomScrollbar* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomScrollbar*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomScrollbar*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			ScrollbarColorScheme cs = a->scolor_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//colorbackground
			HBRUSH tempbrush0 = CreateSolidBrush(cs.background);
			FillRect(hDC, &rcclient, tempbrush0);

			//calculate thumb position
			if (a->start) {
				a->setPosWithVal(a->start_val);
				a->start = false;
			}
			a->thumb_region.left = (!(a->flags & CSB_HORZ) ? rcclient.left + 1 : rcclient.left + a->pos);
			a->thumb_region.top = (!(a->flags & CSB_HORZ) ? rcclient.top + a->pos : rcclient.top + 1);
			a->thumb_region.right = (!(a->flags & CSB_HORZ) ? rcclient.right - 1 : rcclient.left + a->thumb_size + a->pos);
			a->thumb_region.bottom = (!(a->flags & CSB_HORZ) ? rcclient.left + a->thumb_size + a->pos : rcclient.bottom - 1);

			//draw channel
			RECT rc_channel_left;
			RECT rc_channel_right;

			//left/top
			rc_channel_left.left = (!(a->flags & CSB_HORZ) ? rcclient.left + a->channel_size : rcclient.left);
			rc_channel_left.right = (!(a->flags & CSB_HORZ) ? rcclient.right - a->channel_size : rcclient.left + a->pos);
			rc_channel_left.top = (!(a->flags & CSB_HORZ) ? rcclient.top : rcclient.top + a->channel_size);
			rc_channel_left.bottom = (!(a->flags & CSB_HORZ) ? rcclient.top + a->pos : rcclient.bottom - a->channel_size);
			//right/bottom
			rc_channel_right.left = (!(a->flags & CSB_HORZ) ? rcclient.left + a->channel_size : rcclient.left + a->thumb_size + a->pos);
			rc_channel_right.right = (!(a->flags & CSB_HORZ) ? rcclient.right - a->channel_size : rcclient.right);
			rc_channel_right.top = (!(a->flags & CSB_HORZ) ? rcclient.top + a->pos : rcclient.top + a->channel_size);
			rc_channel_right.bottom = (!(a->flags & CSB_HORZ) ? rcclient.bottom : rcclient.bottom - a->channel_size);

			HBRUSH tempbrush1 = CreateSolidBrush(a->mouse_in_client ? cs.left_channel_highlight : cs.left_channel_idle);
			HBRUSH tempbrush2 = CreateSolidBrush(a->mouse_in_client ? cs.right_channel_highlight : cs.right_channel_idle);
			FillRect(hDC, &rc_channel_left, tempbrush1);
			FillRect(hDC, &rc_channel_right, tempbrush2);

			//draw focus rect
			if ((a->focus || a->flags & CSB_BORDER) && !(a->flags & CSB_NOBORDER)) {
				SetDCPenColor(hDC, cs.border);
				Rectangle(hDC, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);
			}

			//draw thumb
			a->thumbHitText();
			SIZE size;
			GetTextExtentPoint32(hDC, str_to_wstr(a->window_text).c_str(), (int)a->window_text.length(), &size);
			SetTextColor(hDC, a->thumb_selected ? cs.thumb_text_selected : a->thumb_hover ? cs.thumb_text_hover : cs.thumb_text_idle);
			SetBkColor(hDC, a->thumb_selected ? cs.thumb_background_selected : a->thumb_hover ? cs.thumb_background_hover : cs.thumb_background_idle);
			ExtTextOut(hDC,
				((a->thumb_region.right - a->thumb_region.left) - size.cx) / 2 + (!(a->flags & CSB_HORZ) ? 0 : a->pos),
				((a->thumb_region.bottom - a->thumb_region.top) - size.cy) / 2 + (!(a->flags & CSB_HORZ) ? a->pos : 1),
				ETO_OPAQUE | ETO_CLIPPED, &a->thumb_region, str_to_wstr(a->window_text).c_str(), a->window_text.length(), NULL);
			SetDCPenColor(hDC, a->thumb_selected ? cs.thumb_border_selected : a->thumb_hover || a->dragging ? cs.thumb_border_hover : cs.thumb_border_idle);
			Rectangle(hDC, a->thumb_region.left, a->thumb_region.top, a->thumb_region.right, a->thumb_region.bottom);

			//draw window name
			if (a->window_name.size() > 0) {
				SetBkMode(hDC, TRANSPARENT);
				SetTextColor(hDC, a->mouse_in_client ? cs.window_name_highlight : cs.window_name_idle);
				RECT rc = getclientrect(a->handle);
				DrawText(hDC, str_to_wstr(a->window_name).c_str(), a->window_name.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			//apply paint
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(tempbrush0);
			DeleteObject(tempbrush1);
			DeleteObject(tempbrush2);
			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			a->focus = true;
			a->mouse_in_client = true;
			a->dragging = true;
			a->thumb_hover = true;
			a->thumb_selected = true;

			a->setFocus(true);

			POINT pt = getclientcursorpos(a->handle);

			//smooth dragging
			a->moveThumb((!(a->flags & CSB_HORZ) ? pt.y : pt.x) - (a->thumb_size / 2));

			customtrackbarProc(a->handle, WM_SETCURSOR, (WPARAM)a->handle, (LPARAM)MAKELONG(HTTHUMB, NULL));

			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP: {
			a->dragging = false;
			a->thumb_selected = false;
			ReleaseCapture();

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			a->CustomScrollbar::thumbHitText();

			if (a->dragging) {

				POINT pt = getclientcursorpos(a->handle);

				//smooth dragging
				a->moveThumb((!(a->flags & CSB_HORZ) ? pt.y : pt.x) - (a->thumb_size / 2));

				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_LEFT:{
					a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_DOWN:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_RIGHT:{
					a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_UP:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_HOME:{
					a->setPosWithVal(a->min_val);
					break;
				}
				case VK_END:{
					a->setPosWithVal(a->max_val);
					break;
				}
				case VK_PRIOR:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
					break;
				}
				case VK_NEXT:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
			if (!wrongkey_skip) {
				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
				}
				else {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
				}
				else {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
				}
			}
			else
				break;
			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_SIZE:{
			a->width = LOWORD(lParam);
			a->height = HIWORD(lParam);

			break;
		}
		case WM_MOVE:{
			a->xpos = LOWORD(lParam);
			a->ypos = HIWORD(lParam);

			break;
		}
		case WM_SETCURSOR:{
			if (a->dragging)
				lParam = MAKELPARAM(HTTHUMB, HIWORD(lParam));
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
				case HTTHUMB:
					SetCursor(a->thumb_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_KILLFOCUS:{
			a->setFocus(false);

			a->mouse_in_client = false;
			a->dragging = false;
			a->thumb_hover = false;
			a->thumb_selected = false;

			break;
		}
		case WM_MOUSELEAVE:{
			if (!a->dragging) {
				a->mouse_in_client = false;
				a->thumb_selected = false;
				a->thumb_hover = false;

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_CustomScrollbar() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = CustomScrollbarclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = CustomScrollbarProc;
	return RegisterClassEx(&wnd);
}
ATOM csb = init_register_custom_CustomScrollbar();

#pragma endregion

#pragma region EditControl class

LRESULT CALLBACK EditControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT id, DWORD_PTR ptr);

EditControl::EditControl() {}
EditControl::EditControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	UINT flags, BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT owner_name, UINT group_name,
	bool number_only/* = false*/, int min_number/* = INT_MIN*/, int max_number/* = INT_MAX*/, bool add_to_list) {
	className = wstr_to_str(EditControlclassName); handle = NULL; caret_pos = 0; start = 0; end = 0;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->flags = flags; this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->hover_info = hover_info; this->owner_name = owner_name; this->group_name = group_name;
	this->number_only = number_only; this->min_number = min_number; this->max_number = max_number;

	if (flags & EDC_VSCROLL) {
		this->width -= 17;
		BasicControl_colorscheme cs =  color_scheme;
		vscrollbar = new CustomScrollbar("vert", WS_CHILD, 0, 0, 0, 0, window_id++,
			0, 1, 0, 1, 1,
			100, 0, CSB_NOBORDER, {
			cs.background_idle_on, cs.border_idle_on, RGB(0, 0, 0), RGB(0, 0, 0),							// background/border/window name idle/highlight
			cs.background_idle_on, cs.background_idle_on, cs.background_idle_on, cs.background_idle_on,		// left/right channel left/right highlight
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,										// thumb background: idle/hover/selected
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,	 									// thumb border: idle/hover/selected
			RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0)														// thumb text: idle/hover/selected
			}, press_cursor, press_cursor,  owner_name, grp_no_group, false);
	}
	if (flags & EDC_HSCROLL) {
		this->height -= 17;
		BasicControl_colorscheme cs =  color_scheme;
		hscrollbar = new CustomScrollbar("horz", WS_CHILD, 0, 0, 0, 0, window_id++,
			0, 1, 0, 1, 1,
			100, 0, CSB_NOBORDER | CSB_HORZ, {
			cs.background_idle_on, cs.border_idle_on, RGB(0, 0, 0), RGB(0, 0, 0),							// background/border/window name idle/highlight
			cs.background_idle_on, cs.background_idle_on, cs.background_idle_on, cs.background_idle_on,		// left/right channel left/right highlight
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,										// thumb background: idle/hover/selected
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,	 									// thumb border: idle/hover/selected
			RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0)														// thumb text: idle/hover/selected
			}, press_cursor, press_cursor, owner_name, grp_no_group, false);
	}

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_IBEAM);
	if (add_to_list)
		custom_controls.addControl(this);
}

vector<string> EditControl::GetLines() {
	vector<string> ret;
	string line = "";
	for (UINT i = 0; i < window_text.size(); i++){
		if (window_text[i] == '\r') {
			ret.push_back(line);
			line = "";
			i++;
			continue;
		}
		if (window_text[i] != '\r' &&window_text[i] != '\n')
			line += window_text[i];
	}
	ret.push_back(line);
	return ret;
}

UINT EditControl::VecPosToStrPos(POINT pt) {
	cout << pt.x << " " << pt.y << '\n';

	int x = 0, y = 0;
	int offset = 0;
	for (UINT i = 0; i < window_text.size(); i++) {
		x++;
		if (window_text[i] == '\n') {
			if (window_text.at(i - 1) == '\n')
				offset++;
			offset++;
			y++;
			x = 0;
		}
		if (pt.x == x && pt.y == y) {
			cout << "i: " << i << '\n';
			return i;
		}
	}

	return 0;
}

POINT EditControl::PosToClient(UINT caret) {//uses text pos to return client pos
	SIZE size;

	vector<string> lines = GetLines();

	UINT total = 0;
	for (UINT i = 0; i < lines.size(); i++) {
		if (total + lines[i].size() >= caret) {
			string letters = window_text.substr(total, caret - total);
			HDC hDC = GetDC(handle);
			GetTextExtentPoint32(hDC, str_to_wstr(letters).c_str(), letters.length(), &size);
			ReleaseDC(handle, hDC);
			return {size.cx + 2, (LONG)line_height * (LONG)i};
		}
		total += lines[i].size() + 2;
	}
	return {0, 0};
}

UINT EditControl::ClientToPos(POINT pos) {//uses client pos to return text pos
	if (line_height < 1)
		return 0;
	int line = (pos.y - 2) / line_height;
	if (line < 0)
		line = 0;
	vector<string> text = GetLines();
	if (line > (int)text.size() - 1)
		line = text.size() - 1;
	HDC hDC = GetDC(handle);
	int total = 0;
	POINT pt = {0, line};
	for (UINT i = 0; i < text[line].size(); i++) {
		SIZE size;
		string letter = "";
		letter += text[line][i];
		GetTextExtentPoint32(hDC, str_to_wstr(letter).c_str(), letter.length(), &size);
		total += size.cx;
		pt.x = i;
		if (total - 3 > pos.x)
			break;
	}
	if (total - 3 < pos.x)
		pt.x = text[line].size();
	ReleaseDC(handle, hDC);

	return VecPosToStrPos(pt);
}

bool EditControl::GetHighlightText(EditTextHighlight* eth, int line /*= -1*/) {
	eth->startpos = start > end ? end : start;
	eth->endpos = start > end ? start : end;
	
	bool l = true, m = false, r = false;
	int y = 0;
	for (UINT i = 0; i < window_text.size(); i++) {
		if ((int)i == eth->startpos) {
			l = false;
			m = true;
		}
		if ((int)i == eth->endpos) {
			m = false;
			r = true;
		}
		if (l && (y == line || line < 0)) 
			eth->left += window_text[i];
		if (m && (y == line || line < 0))
			eth->highlight += window_text[i];
		if (r && (y == line || line < 0))
			eth->right += window_text[i];
		if (window_text[i] == '\n') 
			y++;
	}

	return true;
}

void EditControl::EditText(string text, int pos/* = -1*/) {
	if (pos < 0)
		pos = this->caret_pos;

	if (!(flags & EDC_READONLY)) {
		EditTextHighlight eth;
		GetHighlightText(&eth);

		if (eth.startpos != eth.endpos) 
			window_text.erase(window_text.begin() + eth.startpos, window_text.begin() + eth.endpos);
	}

	if (caret_pos > window_text.size() + 1)
		caret_pos = window_text.size();
	if (text.size() > 0)
		window_text.insert(this->caret_pos, text);

	start = end = caret_pos += text.size();

	PostMessage(handle, WM_customedit_TEXT_CHANGED, (WPARAM)id, (LPARAM)handle);
	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool EditControl::IsTextClipped(RECT* r) {
	RECT rc = getclientrect(handle);
	rc.left += this->scroll_xpos; rc.right += this->scroll_xpos;
	rc.top += this->scroll_ypos; rc.bottom += this->scroll_ypos;

	bool lr = true;
	bool tb = true;

	displayrect(rc);

	displayrect(*r);

	(rc.left < r->left && r->left < rc.right) ? r->left = rc.left, lr = false : r->left = r->left;
	(rc.top < r->top && r->top < rc.bottom) ? r->top = rc.top, tb = false : r->top = r->top;
	(rc.left < r->right && r->right < rc.right) ? r->right = rc.right, lr = false : r->right = r->right, lr = true;
	(rc.top < r->bottom && r->bottom < rc.bottom) ? r->bottom = rc.bottom, tb = false : r->bottom = r->bottom, tb = true;

	//cout << tb << " " << lr << " " << (tb && lr) << endl;
	displayrect(*r);
	cout << "------------\n";


	return tb && lr;
}

EditControl& operator<<(EditControl& obj, int a) {
	obj.EditText(int_to_str(a), obj.window_text.size());
	return obj;
}
EditControl& operator<<(EditControl& obj, char a) {
	obj.EditText(char_to_str(a), obj.window_text.size());
	return obj;
}
EditControl& operator<<(EditControl& obj, string a) {
	obj.EditText(a, obj.window_text.size());
	return obj;
}

LRESULT CALLBACK EditControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	EditControl* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<EditControl*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);

		if (a->flags & EDC_VSCROLL && !a->vscrollbar->handle) {
			a->vscrollbar->Create(GetParent(a->handle));
			a->vscrollbar->scrollbar_owner_handle = a->handle;
		}
		if (a->flags & EDC_HSCROLL && !a->hscrollbar->handle) {
			a->hscrollbar->Create(GetParent(a->handle));
			a->hscrollbar->scrollbar_owner_handle = a->handle;
		}


		for (int i = 1; i < 2; i++)
			a->window_text += int_to_str(i) + "the cake is really good You KNow what i § hahahaha there is no soup\r\n";

		PostMessage(a->handle, WM_customedit_TEXT_CHANGED, NULL, NULL);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<EditControl*>(ptr);
	}

	static BYTE click_count = 0;

	if (message == WM_customtrackbar_VALUE_CHANGED) {
		CustomScrollbar csb = *((CustomScrollbar*)lParam);

		if (csb.scrollbar_owner_handle == a->handle) {
			if (csb.handle == a->vscrollbar->handle && csb.thumb_size > 0) {
				a->scroll_ypos = csb.current_val * a->line_height;
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else if (csb.handle == a->hscrollbar->handle && csb.thumb_size > 0) {
				
			}
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	else if (message == WM_customedit_TEXT_CHANGED) {		
		TEXTMETRIC tm;
		HDC hDC = GetDC(a->handle);
		GetTextMetrics(hDC, &tm);
		ReleaseDC(a->handle, hDC);
		a->line_height = (BYTE)tm.tmHeight;
		vector<string> lines = a->GetLines();

		UINT max_lines = a->height / a->line_height;

		if (a->flags & EDC_VSCROLL) {//calculate trackbar thumb size
			a->vscrollbar->max_val = lines.size() - (a->height / a->line_height);
			if (lines.size() > max_lines) {
				a->vscrollbar->thumb_size = (UINT)((double)a->height * ((double)max_lines / (double)(lines.size() == 0 ? 1 : lines.size())));
				RedrawWindow(a->vscrollbar->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else {
				a->vscrollbar->thumb_size = 0;
				RedrawWindow(a->vscrollbar->handle, NULL, NULL, RDW_INVALIDATE);
			}
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//paint background
			HBRUSH hb_background = CreateSolidBrush(RGB(255,0,0)/*a->toggle_state ? cs.background_idle_on : cs.background_idle_off*/);
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			//draw each line of text
			if (a->font)
				SelectObject(hDC, a->font);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			a->line_height = (BYTE)tm.tmHeight;
			vector<string> text = a->GetLines();
			for (UINT i = 0; i < text.size(); i++) {
				EditTextHighlight eth;
				a->GetHighlightText(&eth, i);
				SIZE lsize, msize, rsize;
				lsize = msize = rsize = {0, 0};
				//left
				if (eth.left.size() > 0) {
					GetTextExtentPoint32(hDC, str_to_wstr(eth.left).c_str(), eth.left.length(), &lsize);
					SetBkColor(hDC, RGB(0,255,0)/*a->toggle_state ? cs.background_idle_on : cs.background_idle_off*/);
					SetTextColor(hDC, a->toggle_state ? cs.text_idle_on : cs.text_idle_off);
					RECT tr = {
						2 /*- a->scroll_xpos*/,
						i * a->line_height + 2 /*- a->scroll_ypos*/,
						lsize.cx + 2 /*- a->scroll_xpos*/,
						(i + 1) * a->line_height + 2/* - a->scroll_ypos*/
					};
					cout << "LEFT:\n";
					//if (a->IsTextClipped(&tr))
						ExtTextOut(hDC, 2 - a->scroll_xpos, i * a->line_height + 2 - a->scroll_ypos, ETO_OPAQUE | ETO_CLIPPED, &tr, str_to_wstr(eth.left).c_str(), eth.left.length(), NULL);
				}
				//middle (highlight)
				if (eth.highlight.size() > 0) {
					GetTextExtentPoint32(hDC, str_to_wstr(eth.highlight).c_str(), eth.highlight.length(), &msize);
					SetBkColor(hDC, a->toggle_state ? cs.background_hover_on : cs.background_hover_off);
					SetTextColor(hDC, a->toggle_state ? cs.text_hover_on : cs.text_hover_off);
					RECT tr = {
						lsize.cx + 2 /*- a->scroll_xpos*/,
						i * a->line_height + 2 /*- a->scroll_ypos*/,
						lsize.cx + msize.cx + 2 /*- a->scroll_xpos*/,
						(i + 1) * a->line_height + 2 /*- a->scroll_ypos*/
					};
					if (a->IsTextClipped(&tr))
						ExtTextOut(hDC, lsize.cx + 2 - a->scroll_xpos, i * a->line_height + 2 - a->scroll_ypos, ETO_OPAQUE | ETO_CLIPPED, &tr, str_to_wstr(eth.highlight).c_str(), eth.highlight.length(), NULL);
				}
				//right
				if (eth.right.size() > 0) {
					GetTextExtentPoint32(hDC, str_to_wstr(eth.right).c_str(), eth.right.length(), &rsize);
					SetBkColor(hDC, RGB(0, 0, 255)/*a->toggle_state ? cs.background_idle_on : cs.background_idle_off*/);
					SetTextColor(hDC, a->toggle_state ? cs.text_idle_on : cs.text_idle_off);
					RECT tr = {
						lsize.cx + msize.cx + 2/* - a->scroll_xpos*/,
						i * a->line_height + 2/* - a->scroll_ypos*/,
						lsize.cx + msize.cx + rsize.cx + 2 /*- a->scroll_xpos*/,
						(i + 1) * a->line_height + 2 /*- a->scroll_ypos*/
					};
					cout << "RIGHT:\n";
				//	if (a->IsTextClipped(&tr))
						ExtTextOut(hDC, lsize.cx + msize.cx + 2 - a->scroll_xpos, i * a->line_height + 2 - a->scroll_ypos, ETO_OPAQUE | ETO_CLIPPED, &tr, str_to_wstr(eth.right).c_str(), eth.right.length(), NULL);
				}
			}

			//draw caret			
			POINT pt = a->PosToClient(a->caret_pos);
			SetDCPenColor(hDC, RGB(255, 255, 255));
			MoveToEx(hDC, pt.x - a->scroll_xpos, pt.y + 2 - a->scroll_ypos, NULL);
			LineTo(hDC, pt.x - a->scroll_xpos, pt.y + a->line_height + 2 - a->scroll_ypos);

			//draw frame
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			//bit block transfer
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);
			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case MY_COMMAND:{
			switch (HIWORD(wParam)) {
				case EN_CHANGE:{
					if (a->number_only) {
						string EditText = getwindowtext((HWND)lParam);
						bool contains_char = false;
						for (UINT j = 0; j < EditText.size(); j++) {
							if (EditText[j] < 48 || EditText[j] > 57 && EditText[j] != 45) {
								EditText.erase(j, 1);
								contains_char = true;
							}
						}
						if (contains_char) {
							SetWindowText((HWND)lParam, str_to_wstr(EditText).c_str());
							SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)EditText.size(), (LPARAM)EditText.size());
						}
						int number = str_to_int(EditText);
						if (number < a->min_number) {
							SetWindowText((HWND)lParam, str_to_wstr(int_to_str(a->min_number)).c_str());
							SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
						}
						else if (number > a->max_number) {
							SetWindowText((HWND)lParam, str_to_wstr(int_to_str(a->max_number)).c_str());
							SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
						}
						a->window_text = getwindowtext(hwnd);
					}
					if (a->Proc != nullptr)
						a->Proc((WPARAM)a->handle, 0);

					break;
				}
				/*case EN_SETFOCUS:{
				PostMessage(hwnd, EM_SETSEL, 0, -1);
				break;
				}*/
			}
			break;
		}
		case WM_LBUTTONDOWN:{/*

			a->vscrollbar->redrawNewVal(0);
			lastwindow = a->vscrollbar->handle;
*/
			SetFocus(a->handle);
			SetCapture(a->handle);

			SetTimer(a->handle, 0, 500, NULL);
			click_count++;

			if (click_count == 2) {

			}
			if (click_count == 3) {
				click_count = 0;

				cout << "trips\n";

				/*vector<string> text = split_str(a->window_text, "\n", 0);
				int i = a->ClientToPos(getclientcursorpos(a->handle));
				a->start = {0, i};
				a->end = {text[i].size(), i};*/

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
				break;
			}

			a->dragging = true;

			a->caret_pos = a->ClientToPos(getclientcursorpos(a->handle));
			if (!(GetKeyState(VK_SHIFT) & 0x8000))
				a->start = a->caret_pos;
			a->end = a->caret_pos;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_LBUTTONUP:{
			ReleaseCapture();

			a->dragging = false;

			break;
		}
		case WM_CHAR:{
			if (!(a->flags & EDC_READONLY) &&
				!(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000) &&
				wParam != '\b' && wParam != '\n' && wParam != '\r' && wParam != '\t') {

				a->EditText(char_to_str((char)wParam), a->caret_pos);
			}
			else
				break;
			
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') {
				a->start = 0; 
				a->end = a->window_text.size();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'C') {
				EditTextHighlight eth;
				a->GetHighlightText(&eth);
				string line = a->window_text.substr(eth.startpos, eth.endpos) /*eth.highlight*/;
				OpenClipboard(NULL);
				EmptyClipboard();
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, line.size() + 1);
				strcpy_s((char*)hGlob, line.size() + 1, line.c_str());
				SetClipboardData(CF_TEXT, hGlob);
				CloseClipboard();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'V') {
				OpenClipboard(NULL);
				HANDLE hData = GetClipboardData(CF_TEXT);
				char * pszText = static_cast<char*>(GlobalLock(hData));	
				std::string text(pszText);
				a->EditText(text);
				GlobalUnlock(hData);
				CloseClipboard();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'X') {
				EditTextHighlight eth;
				a->GetHighlightText(&eth);
				string line = a->window_text.substr(eth.startpos, eth.endpos);
				a->window_text.erase(eth.startpos, eth.endpos);
				a->start = a->end = a->caret_pos = eth.startpos;
				OpenClipboard(NULL);
				EmptyClipboard();
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, line.size() + 1);
				strcpy_s((char*)hGlob, line.size() + 1, line.c_str());
				SetClipboardData(CF_TEXT, hGlob);
				CloseClipboard();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == VK_RIGHT) {
				UINT save_start = a->start;

				auto is_whitespace = [](char c) -> bool { return (c == ' ' || c == '\t' || c == '\r'); };
				
				for (UINT i = a->caret_pos; i < a->window_text.size() - 1; i++) {
					if (is_whitespace(a->window_text[i]) && !is_whitespace(a->window_text[i + 1])){
						a->start = a->end = a->caret_pos = i;
						break;
					}
				}

				if (GetKeyState(VK_SHIFT) & 0x8000) 
					a->start = save_start;
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == VK_LEFT) {
				UINT save_start = a->start;

				auto is_whitespace = [](char c) -> bool { return (c == ' ' || c == '\t' || c == '\r'); };

				for (UINT i = a->caret_pos; i > 1; i--) {
					if (is_whitespace(a->window_text[i]) && !is_whitespace(a->window_text[i - 1])) {
						a->start = a->end = a->caret_pos = i;
						break;
					}
				}

				if (GetKeyState(VK_SHIFT) & 0x8000)
					a->start = save_start;
				goto skip;
			}
			else if (GetKeyState(VK_SHIFT) & 0x8000 && wParam == VK_UP) {
				/*a->caret_pos.y--;
				if (a->caret_pos.y < 0)
					a->caret_pos.y = 0;
				if (a->caret_pos.x > (int)text[a->caret_pos.y].size())
					a->caret_pos.x = text[a->caret_pos.y].size();
				a->end = a->caret_pos;*/
				goto skip;
			}
			else if (GetKeyState(VK_SHIFT) & 0x8000 && wParam == VK_DOWN) {
				/*a->caret_pos.y++;
				if (a->caret_pos.y >(int)text.size() - 1)
					a->caret_pos.y = text.size() - 1;
				if (a->caret_pos.x > (int)text[a->caret_pos.y].size())
					a->caret_pos.x = text[a->caret_pos.y].size();
				a->end = a->caret_pos;*/
				goto skip;
			}
			
			switch (wParam) {
				case VK_LEFT:{
					a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
					if (a->window_text[a->caret_pos] == '\n')
						a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						a->start = a->caret_pos;
					a->end = a->caret_pos;
					break;
				}
				case VK_DOWN:{
					/*a->caret_pos.y++;
					if (a->caret_pos.y >(int)text.size() - 1)
						a->caret_pos.y = text.size() - 1;
					a->start = a->caret_pos;
					a->end = a->caret_pos;*/
					if (a->flags & EDC_HSCROLL) {
						int t = a->scroll_xpos;
						t -= a->line_height;
						a->scroll_xpos = t < 0 ? 0 : t;
					}
					break;
				}
				case VK_RIGHT:{
					a->caret_pos++;
					if (a->caret_pos < a->window_text.size() && a->window_text[a->caret_pos] == '\n') 
						a->caret_pos++;
					if (a->caret_pos > (int)a->window_text.size())
						a->caret_pos = a->window_text.size();
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						a->start = a->caret_pos;
					a->end = a->caret_pos;
					break;
				}
				case VK_UP:{
					/*a->caret_pos.y--;
					if (a->caret_pos.y < 0)
						a->caret_pos.y = 0;
					a->start = a->caret_pos;
					a->end = a->caret_pos;*/
					if (a->flags & EDC_HSCROLL) {
						a->scroll_xpos += a->line_height;
					}
					break;
				}
				case VK_HOME:{

					break;
				}
				case VK_END:{

					break;
				}
				case VK_PRIOR:{

					break;
				}
				case VK_NEXT:{

					break;
				}
				case VK_RETURN:{
					if (!(a->flags & EDC_READONLY)) 
						a->EditText("\r\n");
					break;
				}
				case VK_TAB:{
					if (!(a->flags & EDC_READONLY)) 
						a->EditText("\t", a->caret_pos);
					break;
				}
				case VK_DELETE:{
					if (!(a->flags & EDC_READONLY)) {
						if (a->start == a->end)
							a->start = a->start == a->window_text.size() ? a->window_text.size() : a->start + 1;
						else 
							a->caret_pos = a->start > a->end ? a->end : a->start;
						if (a->window_text[a->caret_pos] == '\r') {
							a->start = a->start == a->window_text.size() ? a->window_text.size() : a->start + 1;
							a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
						}

						a->EditText("");
					}
					break;
				}
				case VK_BACK:{
					if (!(a->flags & EDC_READONLY)) {
						if (a->start == a->end)//if not highlighted
							a->start = a->start == 0 ? 0 : a->start - 1;//move highlight left one
						else 
							a->caret_pos = a->start > a->end ? a->end + 1 : a->start + 1;//else (is highlighted) put cursor to beginning

						if (a->window_text[a->start] == '\n') {//if newline is found move highlight left again, ajdust caret as well
							a->start = a->start == 0 ? 0 : a->start - 1;
							a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
						}
						
						a->EditText("");//removes all highlighted text
						a->start = a->end = a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;//remove highlighting and move caret left
					}
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
			skip:
			/*for (UINT i = 0; i < a->window_text.size(); i++) {
				if (i == a->caret_pos)
					cout << "|";
				if (a->window_text[i] == '\r')
					cout << "\\r";
				else if (a->window_text[i] == '\n')
					cout << "\\n" << '\n';
				else if (a->window_text[i] == '\t')
					cout << "\\t";
				else
					cout << a->window_text[i];
			}
			cout << "\n/////////////////////////////////\n";*/

			if (!wrongkey_skip)
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		/*case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (a->flags & EDC_VSCROLL) {
					int t = a->scroll_ypos;
					t -= a->line_height;
					a->scroll_ypos = t < 0 ? 0 : t;
					a->vscrollbar->redrawNewVal(a->scroll_ypos / a->line_height - 1);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (a->flags & EDC_VSCROLL) {
					a->scroll_ypos += a->line_height;
					vector<string> lines = a->GetLines();
					if (lines.size() < a->height / a->line_height)
						a->scroll_ypos = 0;
					else if (a->scroll_ypos > (lines.size() - (a->height / a->line_height)) * a->line_height)
						a->scroll_ypos = (lines.size() - (a->height / a->line_height)) * a->line_height;

					a->vscrollbar->redrawNewVal(a->scroll_ypos / a->line_height + 1);
				}
			}
			else
				break;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}*/
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = a->hover_info.Flags;
				me.hwndTrack = a->handle;
				me.dwHoverTime = a->hover_info.HoverTime;
				TrackMouseEvent(&me);
			}

			if (a->dragging) {
				a->caret_pos = a->ClientToPos(getclientcursorpos(a->handle));
				a->end = a->caret_pos;

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_CONTEXTMENU:{
			
			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			break;
		}
		case WM_SIZE:{
			a->width = LOWORD(lParam);
			a->height = HIWORD(lParam);

			if (a->flags & EDC_VSCROLL)
				MoveWindow(a->vscrollbar->handle, a->width + a->xpos, a->ypos, 17, a->height, true);
			if (a->flags & EDC_HSCROLL)
				MoveWindow(a->hscrollbar->handle, a->xpos, a->height + a->ypos, a->width, 17, true);
				
			break;
		}
		case WM_MOVE:{
			a->xpos = LOWORD(lParam);
			a->ypos = HIWORD(lParam);

			if (a->flags & EDC_VSCROLL)
				MoveWindow(a->vscrollbar->handle, a->width + a->xpos, a->ypos, 17, a->height, true);
			if (a->flags & EDC_HSCROLL)
				MoveWindow(a->hscrollbar->handle, a->xpos, a->height + a->ypos, a->width, 17, true);

			break;
		}
		case WM_TIMER:{
			if (wParam == 0) {
				click_count = 0;
				KillTimer(a->handle, 0);
			}
			break;
		}
		case WM_MOUSEHOVER:{
			SIZE size;
			HDC hDC = GetDC(hwnd);
			SelectObject(hDC, a->font);
			SetMapMode(hDC, MM_TEXT);
			GetTextExtentPoint32(hDC, str_to_wstr(a->hover_info.hover_text).c_str(), a->hover_info.hover_text.length(), &size);

			POINT pt = getclientcursorpos(GetParent(hwnd));

			StaticControl* temp = new StaticControl;
			temp->window_exstyles = WS_EX_TOPMOST;
			temp->className = wstr_to_str(staticcontrolclassName);
			temp->window_text = a->window_text;
			temp->window_styles = WS_CHILD | WS_VISIBLE | SS_CENTER;
			temp->xpos = pt.x + 20;
			temp->ypos = pt.y;
			temp->width = size.cx;
			temp->height = size.cy;
			temp->id = window_id++;
			temp->client_cursor = NULL;
			temp->font = Font_a;
			temp->Proc = nullptr;
			temp->color_scheme = tokenhackfunctionstatic_scheme;

			a->hover_info.hover_text = a->window_text;

			a->hover_info.handle = temp->Create(GetParent(hwnd));

			ReleaseDC(hwnd, hDC);
			break;
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			if (a->hover_info.handle)
				DestroyWindow(a->hover_info.handle);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

ATOM init_register_custom_EditControl() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = EditControlclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = EditControlProc;
	return RegisterClassEx(&wnd);
}
ATOM edc = init_register_custom_EditControl();

#pragma endregion

#pragma region ContextMenu class

LRESULT CALLBACK CustomContextMenuProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CustomContextMenu* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomContextMenu*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomContextMenu*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = (a->window_text);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return FALSE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_CustomContextMenu() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = CustomContextMenuclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = CustomContextMenuProc;
	return RegisterClassEx(&wnd);
}
ATOM ccxm = init_register_custom_CustomContextMenu();

#pragma endregion

#pragma region tokenhack function class

vector<tokenhackfunction*> tokenhackfunctions;
vector<tokenhackfunction*> all_tokenhackfunctions;

void TOKENHACK_CHECKBOX_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	for (auto a : tokenhackfunctions) {
		if (b->handle == a->checkbox_button.handle) {
			if (a->on) {//if on, turn off
				a->set_on_state(str_to_int(changeconfigonstate(a->name_in_file)));
				EnableWindow(a->hotkey_button.handle, false);
			}
			else {//if off, turn on					
				a->set_on_state(str_to_int(changeconfigonstate(a->name_in_file)));
				EnableWindow(a->hotkey_button.handle, true);
			}
			RedrawWindow(a->hotkey_button.handle, NULL, NULL, RDW_INVALIDATE);


			if (a->checkbox_button.id == READ.checkbox_button.id || a->checkbox_button.id == AUTOROLL.checkbox_button.id) {
				if (statbox) {
					if (!READ.on && !AUTOROLL.on) {
						DestroyWindow(statbox);
						DestroyWindow(STAT_TAB_BTN_OUTPUT.handle);
						statbox = NULL;
						SetWindowPos(g_hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250, functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);
					}
				}
				else {
					statbox_display(g_hwnd);
					SetWindowPos(g_hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250, functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);
				}
				if (a->checkbox_button.id == AUTOROLL.checkbox_button.id) {
					if (AUTOROLL.on)
						statbox_display(g_hwnd, true);
					else
						SetWindowText(statbox, L"");
				}
			}
			break;
		}
	}	
}
void TOKENHACK_HOTKEYBUTTON_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	for (auto a : tokenhackfunctions) {
		if (b->handle == a->hotkey_button.handle) {
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(HOTKEYBOX), g_hwnd, hotkeydlgProc);
			if (global_change_hotkey != 0) {
				if (global_change_hotkey == 1)
					global_change_hotkey = 0;
				hotkeychange(a->hotkey_button.handle, a->hotkey_button.id, a->hotkey_key, global_change_hotkey);
			}
			break;
		}
	}
}

void TOKENHACK_CONFIG_FUNCTIONS_TOGGLE_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	ToggleButton* b = (ToggleButton*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	cout << b->window_text << '\n';
	b->toggle_state = 1;
	cout << b->toggle_state << '\n';
}
void TOKENHACK_CONFIG_FUNCTIONS_EDIT_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	EditControl* b = (EditControl*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);	
	string EditText = getwindowtext(b->handle);

	for (UINT i = 0; i < tokenhackfunctions.size() - 15; i++) {
		tokenhackfunction* a = tokenhackfunctions[i];
		if (a->config_edit_control.handle != NULL && a->config_edit_control.handle == b->handle) {
			changebuttontext(a->config_static_control.handle, str_to_int(EditText));
			RedrawWindow(a->config_static_control.handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
	}
}
LRESULT CALLBACK tokenhackconfigstaticProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT/* id*/, DWORD_PTR/* ptr*/) {
	StaticControl* a = (StaticControl*)GetWindowLongPtr(hwnd, GWL_USERDATA);
	if (message == WM_KBHOOKKEYDOWN) {
		changebuttontext(a->handle, global_hook_key);

		for (auto b : tokenhackfunctions) {
			if (b->config_static_control.handle == a->handle) {
				SetWindowText(b->config_edit_control.handle, str_to_wstr(int_to_str(global_hook_key)).c_str());
				cout << b->config_edit_control.window_text << '\n';
				break;
			}
		}
	}
	switch (message) {
		case WM_SETFOCUS:{
			a->focus = true;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			return FALSE;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			RemoveWindowSubclass(a->handle, tokenhackconfigstaticProc, 2);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			return FALSE;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			a->focus = false;
			a->mouse_in_client = false;

			RemoveWindowSubclass(a->handle, tokenhackconfigstaticProc, 2);

			break;
		}
	}
	return DefSubclassProc(hwnd, message, wParam, lParam);
}
void TOKENHACK_CONFIG_FUNCTIONS_STATIC_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	StaticControl* a = (StaticControl*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);

	SetWindowSubclass(a->handle, tokenhackconfigstaticProc, 2, (DWORD_PTR)a);

	SetFocus(a->handle);
}

tokenhackfunction::tokenhackfunction() {}
tokenhackfunction::tokenhackfunction(string static_text, string name_in_file, vector<string> ccb_options):
checkbox_button("T", button_styles, checkboxxoff, 0, checkboxwidth, checkboxheight, cb_id++, TOKENHACK_CHECKBOX_PROC, tokenhackfunctioncheckbox_scheme, Font_a, press_cursor, 0, own_tokenhack_tab_D2, grp_tokenhack_function),
hotkey_button("", button_styles, buttonxoff, 0, buttonwidth, buttonheight, window_id++, TOKENHACK_HOTKEYBUTTON_PROC, tokenhackfunctionhotkey_scheme, Font_a, press_cursor, own_tokenhack_tab_D2, grp_tokenhack_function),
static_control(static_text, button_styles, functionnamexoff, 0, functionnamewidth, functionnameheight, st_id++, nullptr, tokenhackfunctionstatic_scheme, Font_a, idle_cursor, default_hover, own_tokenhack_tab_D2, grp_tokenhack_function),

config_toggle_button(static_text, WS_CHILD, 15, 0, 132, 23, con_tbtn_id++, TOKENHACK_CONFIG_FUNCTIONS_TOGGLE_PROC, "", togglebutton_scheme, Font_a, press_cursor, 1, own_config_dialog_tab_functions, grp_tokenhack_function),
config_edit_control("", WS_CHILD | ES_LEFT, 149, 0, 33, 23, con_e_id++, TOKENHACK_CONFIG_FUNCTIONS_EDIT_PROC, 0, togglebutton_scheme, Font_a, NULL, default_hover, own_config_dialog_tab_functions, grp_tokenhack_function, true, 0, 4094),
config_static_control("", WS_CHILD | SS_CENTER, 184, 0, 266, 23, con_s_id++, TOKENHACK_CONFIG_FUNCTIONS_STATIC_PROC, tokenhackfunctionconfigstatic_scheme, Font_a, idle_cursor, default_hover, own_config_dialog_tab_functions, grp_tokenhack_function),
config_custom_combobox("", WS_CHILD, 452, 0, 72, 23, con_c_id++, ccb_options, 15, (UINT)-1, ccb_style2, nullptr, normalbutton_scheme, Font_a, press_cursor, own_config_dialog_tab_functions, grp_tokenhack_function)
{	
	checkbox_button.handle = NULL; hotkey_button.handle = NULL; static_control.handle = NULL; hotkey_key = 0; on = 0;
	this->name_in_file = name_in_file; this->owner_name = own_root_window; this->group_name = grp_tokenhack_function;
	config_toggle_button.string_in_file = name_in_file;

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

#pragma region customcontrols container class

void CustomControls::addControl(TabButton* tabbtn) { tabbuttons.push_back(tabbtn); all_controls.push_back((LONG_PTR)tabbtn); }
void CustomControls::addControl(RadioButton* rbtn) { autoradiobuttons.push_back(rbtn); all_controls.push_back((LONG_PTR)rbtn); }
void CustomControls::addControl(CheckBox* cbtn) { checkboxbuttons.push_back(cbtn); all_controls.push_back((LONG_PTR)cbtn);  }
void CustomControls::addControl(Button* btn) { normalbuttons.push_back(btn); all_controls.push_back((LONG_PTR)btn); }
void CustomControls::addControl(ToggleButton* tbtn) { togglebuttons.push_back(tbtn); all_controls.push_back((LONG_PTR)tbtn); }
void CustomControls::addControl(StaticControl* stc) { staticcontrols.push_back(stc); all_controls.push_back((LONG_PTR)stc); }
void CustomControls::addControl(CustomTrackbar* ctb) { customtrackbars.push_back(ctb); all_controls.push_back((LONG_PTR)ctb); }
void CustomControls::addControl(CustomComboBox* ccb) { customcomboboxes.push_back(ccb); all_controls.push_back((LONG_PTR)ccb); }
void CustomControls::addControl(EditControl* edc) { EditControls.push_back(edc); all_controls.push_back((LONG_PTR)edc); }
void CustomControls::addControl(CustomContextMenu* ccxm) { CustomContextMenus.push_back(ccxm); all_controls.push_back((LONG_PTR)ccxm); }
void CustomControls::addControl(CustomScrollbar* csb) { CustomScrollbars.push_back(csb); all_controls.push_back((LONG_PTR)csb); }

BasicControl* CustomControls::getControl(LONG_PTR ptr) {
	BasicControl* bc = (BasicControl*)ptr;
	return bc;
}

bool CustomControls::createChildren(HWND parent, bool(*control_condition)(BasicControl*)) {
	for (auto & c : all_controls) {
		BasicControl* a = getControl(c);
		if (control_condition(a) && a->handle == NULL)
			a->Create(parent);
	}
	return true;
}

#pragma endregion

#pragma region window creation
	#pragma region tokenhack window
		#pragma region tokenhack tabs
		void TAB_BTN_ALL_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			EnumChildWindows(g_hwnd, EnumChildProc, DESTROY_WINDOWS);
			turn_all_functions_off();
			tokenhackfunctions.erase(tokenhackfunctions.begin(), tokenhackfunctions.end());
			tokenhackfunctions = all_tokenhackfunctions;
			BTN_RELOAD_PROC(0, MAKELPARAM(0, 1));
		}
		TabButton TAB_BTN_ALL("D2", button_styles, 15, 0, 45, tabbuttonheight, window_id++, TAB_BTN_ALL_PROC, tokenhackfunctiontab_scheme, Font_a, press_cursor, 1, own_root_window, own_tokenhack_tab_D2, grp_function_tab_button);
		TabButton TAB_BTN_CONSOLE("Console", button_styles, 60, 0, 72, tabbuttonheight, window_id++, nullptr, tokenhackfunctiontab_scheme, Font_a, press_cursor, 1, own_root_window, own_tokenhack_tab_console, grp_function_tab_button);

		TabButton STAT_TAB_BTN_OUTPUT("Output", button_styles, statboxxoff, 0, 60, tabbuttonheight, window_id++, nullptr, stattab_scheme, Font_a, press_cursor, 1, own_root_window, own_stat_tab_output, grp_stat_tab_button);
		TabButton STAT_TAB_BTN_INPUT("Input", button_styles, statboxxoff + 60, 0, 60, tabbuttonheight, window_id++, nullptr, stattab_scheme, Font_a, press_cursor, 0, own_root_window, own_stat_tab_input, grp_stat_tab_button);
		#pragma endregion
			#pragma region tokenhack functions
			//////////////////////////////////////////////////////////////////////////////////////
			void CBTN_TOGGLEALL_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
				if (toggleall) {
					toggleall = false;
					for (auto & b : tokenhackfunctions) {
						if (b->on) {
							SendMessage(g_hwnd, WM_COMMAND, (WPARAM)b->checkbox_button.id, NULL);
							RedrawWindow(b->checkbox_button.handle, NULL, NULL, RDW_INVALIDATE);
						}
					}
				}
				else {
					toggleall = true;
					for (auto & b : tokenhackfunctions) {
						if (!b->on) {
							SendMessage(g_hwnd, WM_COMMAND, (WPARAM)b->checkbox_button.id, NULL);
							RedrawWindow(b->checkbox_button.handle, NULL, NULL, RDW_INVALIDATE);
						}
					}
				}
			}
			CheckBox CBTN_TOGGLEALL("T", button_styles, 0, 0, 14, 14, window_id++, CBTN_TOGGLEALL_PROC, {
				RGB(10, 10, 10), RGB(255, 206, 61), RGB(255, 206, 61),		//IDLE ON: background/border/text
				RGB(100, 100, 100), RGB(255, 206, 61), RGB(255, 206, 61),		//HOVER ON: background/border/text
				RGB(10, 10, 10), RGB(255, 206, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
				RGB(100, 100, 100), RGB(255, 206, 61), RGB(100, 100, 100)	//HOVER OFF: background/border/text
			}, Font_a, press_cursor, 0, own_root_window, grp_no_group);

			tokenhackfunction WMC("Toggle Cursor Lock", "wmc", {"off", "on"});
			tokenhackfunction CLOSE("Close", "close", {"off", "on"});
			tokenhackfunction NOPICKUP("\"/nopickup\"", "\"/nopickup\"", {"off", "on"});
			tokenhackfunction PAUSE("Pause/Resume", "pause" , {"off", "on"});
			tokenhackfunction ROLL("Transmute", "transmute", {"off", "on", "2", "3", "4", "5", "6", "7", "8"});
			tokenhackfunction AUTOROLL("Auto Roll (OCR)", "auto_roll", {"off", "on", "2", "3", "4", "5", "6", "7", "8"});
			tokenhackfunction READ("Read Stats (OCR)", "read_stats", {"off", "on", "2", "3", "4", "5", "6", "7", "8"});
			tokenhackfunction MOVEITEM("Move Item", "move_item", {"off", "on"});
			tokenhackfunction TEST("test", "test", {"off", "on"});
			tokenhackfunction TEST2("test2", "test2", {"off", "on"});
			tokenhackfunction TEST3("test3", "test3", {"off", "on"});
			tokenhackfunction RAINBOW("Rainbow", "rainbowtest", {"off", "on"});
			tokenhackfunction TEST4("test4", "test4", {"off", "on"});

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
			tokenhackfunction DONOTHINGFILLER1(token_func_rand_text(), "i", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER2(token_func_rand_text(), "put", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER3(token_func_rand_text(), "these", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER4(token_func_rand_text(), "here", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER5(token_func_rand_text(), "so", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER6(token_func_rand_text(), "\"rainbow\"", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER7(token_func_rand_text(), "looks", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER8(token_func_rand_text(), "much", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER9(token_func_rand_text(), "better", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER10(token_func_rand_text(), "than", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER11(token_func_rand_text(), "it", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER12(token_func_rand_text(), "does", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER13(token_func_rand_text(), "with", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER14(token_func_rand_text(), "fewer", {"off", "on"});
			tokenhackfunction DONOTHINGFILLER15(token_func_rand_text(), "things", {"off", "on"});
			#pragma endregion
			#pragma region console tab
			BasicControl_colorscheme EditControl_scheme(RGB(10, 10, 10), RGB(100, 100, 100), RGB(255, 255, 100), RGB(104, 10, 10), RGB(100, 100, 100), RGB(255, 255, 100));
			EditControl console("",
				WS_CHILD | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
				2, 17, 363, 409, window_id++, nullptr, NULL | EDC_SCROLL, EditControl_scheme, NULL/*Font_ariel*/, NULL, default_hover, own_tokenhack_tab_console, grp_no_group);
			#pragma endregion
		#pragma region tokenhack normal buttons
		//////////////////////////////////////////////////////////////////////////////////////
		void BTN_CONFIG_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			config = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(CONFIGBOX), g_hwnd, configdlgProc);
			EnableWindow(g_hwnd, FALSE);
		}
		Button BTN_CONFIG("Open Config", button_styles, 14, 35, 100, 30, window_id++, BTN_CONFIG_PROC, normalbutton_scheme, Font_a, press_cursor, own_root_window, grp_tokenhack_offset);
		void BTN_RELOAD_PROC(WPARAM /*wParam*/, LPARAM lParam) {
			EnumChildWindows(g_hwnd, EnumChildProc, DESTROY_WINDOWS);
			turn_all_functions_off();
			stats_in_file.erase(stats_in_file.begin(), stats_in_file.end());
			reset_color_text_RGB();
			ClipCursor(NULL);
			RECT rcclient = getclientrect(g_hwnd);
			InvalidateRect(g_hwnd, &rcclient, true);
			SendMessage(g_hwnd, WM_CREATE, NULL, NULL);
			SendMessage(g_hwnd, WM_SIZE, NULL, NULL);
			if (HIWORD(lParam) == 0) {
				cout << "\n-------------------RELOAD-------------------" << '\n';
				if (CTB_VOLUME.current_val > 0)
					PlaySound(MAKEINTRESOURCE(RELOAD), NULL, SND_ASYNC | SND_RESOURCE);
				BTN_RELOAD.mouse_in_client = false;
				RedrawWindow(BTN_RELOAD.handle, NULL, NULL, RDW_INVALIDATE);
			}
		}
		Button BTN_RELOAD("Reload", button_styles, 140, 35, 80, 30, window_id++, BTN_RELOAD_PROC, normalbutton_scheme, Font_a, press_cursor, own_root_window, grp_tokenhack_offset);
		void BTN_LAUNCH_PROC(WPARAM /*wParam*/, LPARAM lParam) {
			if (gamepath.size() == 0 || lParam != 0) {
				OPENFILENAME ofn;
				char File[MAX_PATH];
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = g_hwnd;
				ofn.lpstrFile = (LPWSTR)File;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter = TEXT("exe\0*.exe*\0All\0*.*\0Shortcut\0*.lnk*\0Text\0*.TXT\0");
				ofn.lpstrTitle = L"Select executable to be launched";
				ofn.nFilterIndex = 0;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS;

				GetOpenFileName(&ofn);

				if (wcslen(ofn.lpstrFile) > 0)
					gamepath = wstr_to_str(ofn.lpstrFile);
				else
					return;

				vector<string> text;
				ifstream filein;
				filein.open("config.txt");
				if (!filein)
					return;
				string line;
				while (getline(filein, line))
					text.push_back(line);
				filein.close();

				for (auto & a : text) {
					line = a;
					line = line.substr(0, line.find('*'));
					line.erase(remove(line.begin(), line.end(), '\t'), line.end());
					string val = line.substr(line.find(':') + 1, line.size());
					line.erase(remove(line.begin(), line.end(), ' '), line.end());
					transform(line.begin(), line.end(), line.begin(), ::tolower);
					string var_name = line.substr(0, line.find(':'));
					var_name = var_name.substr(0, var_name.find('['));
					var_name = var_name.substr(0, var_name.find('('));
					if (var_name == "gamepath")
						a = "gamepath\t\t:\t" + gamepath;
				}
				ofstream fileout;
				fileout.open("config.txt");
				if (!fileout)
					return;
				for (auto & a : text)
					fileout << a << '\n';
				fileout.close();
			}
			gamedir = gamepath.substr(0, gamepath.rfind('\\'));
			string gamename = gamepath.substr(gamepath.rfind('\\') + 1, gamepath.size());

			if (gamepath.size() > 0 && lParam == 0) 
				ShellExecute(g_hwnd, TEXT("open"), str_to_wstr(gamename).c_str(), str_to_wstr(targetlines).c_str(), str_to_wstr(gamedir).c_str(), SW_SHOW);
		}
		Button BTN_LAUNCH("Launch", button_styles, 246, 35, 100, 30, window_id++, BTN_LAUNCH_PROC, normalbutton_scheme, Font_a, press_cursor, own_root_window, grp_tokenhack_offset);
		void BTN_LAUNCH_DIRECTORY_SET_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			BTN_LAUNCH_PROC(0, 1);
		}			
		Button BTN_LAUNCH_DIRECTORY_SET("...", button_styles, 345, 35, 17, 30, window_id++, BTN_LAUNCH_DIRECTORY_SET_PROC, normalbutton_scheme, Font_a, press_cursor, own_root_window, grp_tokenhack_offset);
		void BTN_COW_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			static int cowc = 0;
			if (CTB_VOLUME.current_val > 0) {
				if (cowc == 2)
					cowc = 0;
				if (cowc == 0)
					PlaySound(MAKEINTRESOURCE(COW1), NULL, SND_ASYNC | SND_RESOURCE);
				if (cowc == 1)
					PlaySound(MAKEINTRESOURCE(COW2), NULL, SND_ASYNC | SND_RESOURCE);
				cowc++;
			}
		}
		Button BTN_COW("C", WS_CHILD | BS_OWNERDRAW, 113, 35, 28, 30, window_id++, BTN_COW_PROC, normalbutton_scheme, Font_a, press_cursor, own_root_window, grp_tokenhack_offset);
		void BTN_MUSHROOM_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			static int mushroomc = 508;
			if (CTB_VOLUME.current_val > 0) {
				if (mushroomc == 517)
					mushroomc = 508;
				PlaySound(MAKEINTRESOURCE(mushroomc), NULL, SND_ASYNC | SND_RESOURCE);
				mushroomc++;
			}
		}
		Button BTN_MUSHROOM("M", WS_CHILD | BS_OWNERDRAW, 219, 35, 28, 30, window_id++, BTN_MUSHROOM_PROC, normalbutton_scheme, Font_a, press_cursor, own_root_window, grp_tokenhack_offset);
		//////////////////////////////////////////////////////////////////////////////////////
		#pragma endregion
	#pragma endregion
	#pragma region config dialog
		#pragma region config dialog buttons
		void CONFIG_OK_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			
		}
		Button CONFIG_OK("Ok", button_styles, 20, 329, 100, 35, window_id++, CONFIG_OK_PROC, normalbutton_scheme, Font_a, press_cursor, own_config_dialog_box, grp_no_group);
		void CONFIG_CANCEL_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			
		}
		Button CONFIG_CANCEL("Cancel", button_styles, 219, 329, 100, 35, window_id++, CONFIG_CANCEL_PROC, normalbutton_scheme, Font_a, press_cursor, own_config_dialog_box, grp_no_group);
		void CONFIG_APPLY_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
			
		}
		Button CONFIG_APPLY("Apply", button_styles, 420, 329, 100, 35, window_id++, CONFIG_APPLY_PROC, normalbutton_scheme, Font_a, press_cursor, own_config_dialog_box, grp_no_group);

		#pragma endregion
		#pragma region config dialog tabs
		TabButton TAB_BTN_CONFIG_TAB_BTN_FUNCTIONS("Functions", button_styles, 5, 3, 75, 16, window_id++, nullptr, tokenhackfunctiontab_scheme, Font_a, press_cursor, 1, own_config_dialog_box, own_config_dialog_tab_functions, grp_config_tab_button);
		TabButton TAB_BTN_CONFIG_TAB_BTN_MISC("Misc", button_styles, 80, 3, 52, 16, window_id++, nullptr, tokenhackfunctiontab_scheme, Font_a, press_cursor, 0, own_config_dialog_box, own_config_dialog_tab_misc, grp_config_tab_button);
		TabButton TAB_BTN_CONFIG_TAB_BTN_TEST("Test", button_styles, 132, 3, 50, 16, window_id++, nullptr, tokenhackfunctiontab_scheme, Font_a, press_cursor, 0, own_config_dialog_box, own_config_dialog_tab_test, grp_config_tab_button);
		
		#pragma endregion
			#pragma region config dialog tab: misc
			CustomTrackbar CTB_VOLUME("volume", WS_CHILD, 16, 70, 68, 14, window_id++,
				0x0000, 0xFFFF, 0x4444, 0x0CCF, 0x199A,
				0, 0, CTB_BORDER | CTB_SMOOTH | CTB_STAY, {
				RGB(10, 10, 10), RGB(50, 50, 50), RGB(80, 80, 80), RGB(127, 127, 127),							// background/border/window name idle/highlight
				RGB(0, 255, 0), RGB(10, 10, 10), RGB(0, 255, 0), RGB(10, 10, 10),								// left/right channel left/right highlight
				RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127), 									// thumb background: idle/hover/selected
				RGB(10, 10, 10), RGB(10, 10, 10), RGB(10, 10, 10),			 									// thumb border: idle/hover/selected
				RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127)										// thumb text: idle/hover/selected
			}, press_cursor, press_cursor, own_config_dialog_tab_misc, grp_no_group);
			void TBTN_RAINBOW_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
				for (auto & a : tokenhackfunctions) {
					if (IsWindow(a->hotkey_button.handle)) {
						RECT rcclient = getclientrect(a->static_control.handle);
						InvalidateRect(a->static_control.handle, &rcclient, TRUE);
					}
				}
			}
			ToggleButton TBTN_RAINBOW("rainbow", WS_CHILD, 91, 70, 72, 14, window_id++, TBTN_RAINBOW_PROC, "rainbow", togglebutton_scheme, Font_a, press_cursor, false, own_config_dialog_tab_misc, grp_no_group);
			void TBTN_CONSOLE_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
				if (!TBTN_CONSOLE.toggle_state) {
					FreeConsole();
				}
				else {
					CONSOLE_ON();
					SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			ToggleButton TBTN_CONSOLE("console", WS_CHILD, 170, 70, 64, 14, window_id++, TBTN_CONSOLE_PROC, "console", togglebutton_scheme, Font_a, press_cursor, false, own_config_dialog_tab_misc, grp_no_group);
			ToggleButton TBTN_HOOKOUTPUT("kb", WS_CHILD, 240, 70, 17, 14, window_id++, nullptr, "keyboardhookoutput", togglebutton_scheme, Font_a, press_cursor, false, own_config_dialog_tab_misc, grp_no_group);
			ToggleButton TBTN_SHOWONSTART("show on start", WS_CHILD, 263, 70, 98, 14, window_id++, nullptr, "showtokenhackonstart", togglebutton_scheme, Font_a, press_cursor, false, own_config_dialog_tab_misc, grp_no_group);
			#pragma endregion
			#pragma region config dialog tab: test

			#pragma endregion
		#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////

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
		if (a->on && a->group_name == grp_tokenhack_function)
			hotkeys.push_back(a->hotkey_key);
}
void turn_all_functions_off() {
	for (auto & a: tokenhackfunctions)
		a->set_on_state(0);
	totalonfunctions = 0;
}

void statbox_display(HWND hwnd, bool skiptostatsinfile /* = false*/) {
	if (READ.on || AUTOROLL.on) {
		if (!skiptostatsinfile) {
			statbox = CreateWindow(TEXT("EDIT"), TEXT(""),
				WS_CHILD | WS_VISIBLE | WS_BORDER /*| ES_WANTRETURN*/ | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL/* | ES_READONLY*/ | WS_VSCROLL | WS_HSCROLL,
				0, 0,
				0, 0,
				hwnd, (HMENU)statboxid, NULL, NULL);
			for (auto & a: custom_controls.tabbuttons) {
				if (a->owner_name != own_root_window)
					continue;
				if (a->group_name == grp_stat_tab_button) 
					a->Create(hwnd);
			}
		}
		if (AUTOROLL.on) {
			load_stats_and_colors_from_file();
			string searching = "Stats in file:                                         asdf\r\n";
			for (UINT i = 0; i < 10; i++)
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
