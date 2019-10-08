#include <string>
#include <Windows.h>
#include <windowsx.h>

#include "Tokenhackdll.h"

using namespace std;

HWND tokenhackwnd = FindWindow(L"Tokenhack", VERSION);

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved) {
	switch (Reason) {
		case DLL_PROCESS_ATTACH:{
			DisableThreadLibraryCalls(hDLL);

			if (!tokenhackwnd) {
				MessageBox(NULL, L"Tokenhack not found", TEXT("Error"), MB_OK);
				return TRUE;
			}			

			CONSOLE_ON();

			std::cout << "DLL attach function called" << '\n';
			break;
		}
		case DLL_PROCESS_DETACH:{
			std::cout << "DLL detach function called" << '\n';
			FreeConsole();
			break;
		}
		case DLL_THREAD_ATTACH:{
			std::cout << "DLL thread attach function called" << '\n';
			break;
		}
		case DLL_THREAD_DETACH:{
			std::cout << "DLL thread detach function called" << '\n';
			break;
		}
	}
	return TRUE;
}

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

string getMessageText(int msgId) {										
	int size = (sizeof(WindowsMessages) / sizeof(*WindowsMessages));			
	for (int i = 0; i < size; i++) {
		if (msgId == WindowsMessages[i].msgid)
			return WindowsMessages[i].pname;
	}																																		
	return "";																	
}																				
		
#pragma endregion

#define X(x) { x, #x },
struct WindowStyles {
	int style;
	char* name;
} WindowStyles[] =
{
	X(WS_OVERLAPPED)
	X(WS_POPUP)
	X(WS_CHILD)
	X(WS_MINIMIZE)
	X(WS_VISIBLE)
	X(WS_DISABLED)
	X(WS_CLIPSIBLINGS)
	X(WS_CLIPCHILDREN)
	X(WS_MAXIMIZE)
	X(WS_CAPTION)
	X(WS_BORDER)
	X(WS_DLGFRAME)
	X(WS_VSCROLL)
	X(WS_HSCROLL)
	X(WS_SYSMENU)
	X(WS_THICKFRAME)
	X(WS_GROUP)
	X(WS_TABSTOP)
	X(WS_MINIMIZEBOX)
	X(WS_MAXIMIZEBOX)
	X(WS_TILED)
	X(WS_ICONIC)
	X(WS_SIZEBOX)
	X(WS_TILEDWINDOW)
	X(WS_EX_DLGMODALFRAME)
	X(WS_EX_NOPARENTNOTIFY)
	X(WS_EX_TOPMOST)
	X(WS_EX_ACCEPTFILES)
	X(WS_EX_TRANSPARENT)
	X(WS_EX_MDICHILD)
	X(WS_EX_TOOLWINDOW)
	X(WS_EX_WINDOWEDGE)
	X(WS_EX_CLIENTEDGE)
	X(WS_EX_CONTEXTHELP)
	X(WS_EX_RIGHT)
	X(WS_EX_LEFT)
	X(WS_EX_RTLREADING)
	X(WS_EX_LTRREADING)
	X(WS_EX_LEFTSCROLLBAR)
	X(WS_EX_RIGHTSCROLLBAR)
	X(WS_EX_CONTROLPARENT)
	X(WS_EX_STATICEDGE)
	X(WS_EX_APPWINDOW)
};
#undef X

string getwindowstyletext(int msgId) {
	int size = (sizeof(WindowStyles) / sizeof(*WindowStyles));
	string ret = "";
	for (int i = 0; i < size; i++) {
		if (msgId & WindowStyles[i].style) {
			ret += WindowStyles[i].name;
			ret += '\n';
		}
	}
	return ret;
}

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
	LUID luid;
	BOOL bRet = FALSE;

	if (LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
		TOKEN_PRIVILEGES tp;

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (int hatfat = AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, NULL, NULL)) {
			bRet = (GetLastError() == ERROR_SUCCESS);
		}
	}

	return bRet;
}

__declspec(dllexport) LRESULT CALLBACK dllcallwndProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0) {
		CWPSTRUCT cwp = *((CWPSTRUCT*)lParam);
		//cout << "callwndproc msg: " << getMessageText(cwp.message) << " " << cwp.wParam << " " << cwp.lParam << '\n';
		//switch (cwp.message) {
		//	/*case WM_GETICON:{
		//		PostMessage(tokenhackwnd, WM_DLL_HOOKPROC, (WPARAM)cwp.hwnd, NULL);
		//	}*/
		//}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

__declspec(dllexport) LRESULT CALLBACK dllcallwndretProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0) {
		CWPRETSTRUCT cwpr = *((CWPRETSTRUCT*)lParam);
		//cout << "ret: " << getMessageText(cwpr.message) << " " << cwpr.wParam << " " << cwpr.lParam << " " << cwpr.lResult << '\n';
		/*switch (cwpr.message) {
			
		}*/
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

__declspec(dllexport) LRESULT CALLBACK dllgetmessageProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0) {
		MSG dllmsg = *((MSG*)lParam);
		/*if (dllmsg.wParam != 0 || dllmsg.lParam != 0)
			cout << "getmsgproc msg: " << getMessageText(dllmsg.message) << " " << dllmsg.wParam << " " << dllmsg.lParam << '\n';*/
		switch (dllmsg.message) {
			case WM_KEYDOWN:{
				//cout << "getmsgproc msg: " << getMessageText(dllmsg.message) << " " << dllmsg.wParam << " " << dllmsg.lParam << '\n';
				if (dllmsg.wParam == VK_RETURN) {
					cout << "getmsgproc msg: " << getMessageText(dllmsg.message) << " " << dllmsg.wParam << " " << dllmsg.lParam << '\n';

					TCHAR item_text[1000];
					
					while (wcslen(item_text) < 400) {
						cout << "len: " << wcslen(item_text) << '\n';
						if (!memcpy(item_text, (LPVOID)(GetModuleHandle(L"D2COMMON.dll") + 0x62C70), 1000))
							cout << "failed to read memory\n";
						else {
							cout << "Sucess!" << '\n';
						}
					}
					
					cout << '\n';
					wcout << "text: " << item_text << '\n';
					cout << "text: " << item_text << '\n';

				}
				break;
			}
			case WM_CHAR:{		
				//cout << "getmsgproc msg: " << getMessageText(dllmsg.message) << " " << dllmsg.wParam << " " << dllmsg.lParam << '\n';
				break;				
			}
			case WM_KEYUP:{
				//cout << "getmsgproc msg: " << getMessageText(dllmsg.message) << " " << dllmsg.wParam << " " << dllmsg.lParam << '\n';
				if (dllmsg.wParam == VK_RETURN) {					
					//return 1;
				}
				break;
			}			
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}


