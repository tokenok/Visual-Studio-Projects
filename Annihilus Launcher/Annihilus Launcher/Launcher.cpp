#include <Windows.h>
#include <iostream>
#include <string>
#include <regex>
#include <thread>
#include <Shlobj.h>
#include <atlbase.h>
#include <direct.h>
#include "Shlwapi.h"

#include "Registry.h"
#include <zlib.h>

#include "window.h"
#include "resource.h"
#include "common.h"
#include "console.h"
#include "update.h"
#include "include.h"

#pragma comment(lib, "Shlwapi.lib")

#include <conio.h>

using std::cout;
using std::string;
using std::vector;
using namespace console_class;

Console console;

void install_launcher() {
	CRegistry hregkey;
	if (!hregkey.Open(CREG_PATH, CREG_ROOTKEY))
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);

	hregkey[CREG_KEY_VERSION] = L"1.101d";

	TCHAR wpath[MAX_PATH];
	GetModuleFileName(NULL, wpath, MAX_PATH);
	string path = wstr_to_str(wpath);
	path = path.substr(0, path.rfind('\\'));
	path = path.substr(0, path.rfind('\\'));

	hregkey[CREG_KEY_ANNIHILUS_PATH] = str_to_wstr(path).c_str();

	hregkey[CREG_KEY_CB_SKIPTOBNET] = 1;
	hregkey[CREG_KEY_CB_WINDOWMODE] = 1;
	hregkey[CREG_KEY_CB_SHOWCONSOLE] = 0;

	hregkey.Close();
}

void set_button_states() {
	CRegistry hregkey;
	if (!hregkey.Open(CREG_PATH, CREG_ROOTKEY))
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);

	SendMessage(skiptobnet_btn, BM_SETCHECK, str_to_int(wstr_to_str(hregkey[CREG_KEY_CB_SKIPTOBNET])), NULL);
	SendMessage(windowmode_btn, BM_SETCHECK, str_to_int(wstr_to_str(hregkey[CREG_KEY_CB_WINDOWMODE])), NULL);
	SendMessage(showconsole_btn, BM_SETCHECK, str_to_int(wstr_to_str(hregkey[CREG_KEY_CB_SHOWCONSOLE])), NULL);

	hregkey.Close();
}

template<typename T>
void edit_reg_config(LPCWSTR key, T t, HKEY root = CREG_ROOTKEY, const wchar_t* path = CREG_PATH);

template<typename T>
void edit_reg_config(LPCWSTR key, T t, HKEY root, const wchar_t* path) {
	CRegistry hregkey;
	if (!hregkey.Open(path, root))
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);

	hregkey[key] = t;

	hregkey.Close();
}







static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}






int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR args, int /*iCmdShow*/) {
	console.show();
	console.setTitle("Console");
	//console.setFont("CONSOLE", 14, 700);
	console.setConsoleColor(b::black + t::white);

	//get_status_info();


	string cmdargs = args;
	if (cmdargs.find("-install") != string::npos)
		install_launcher();

//	extract_folder("annihilus.zip", "C:\\Program Files (x86)\\Diablo II");

	//_getch();

	install_launcher();

//	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	register_class(hInstance);
	g_hwnd = CreateWindow(className, VERSION, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 350, 150, 500, 400, NULL, NULL, hInstance, NULL);

	update();

	if (!SendMessage(showconsole_btn, BM_GETCHECK, NULL, NULL))
		console.hide();

	ShowWindow(g_hwnd, SW_SHOW);

	InitNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
	
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		Minimize();
		return 0;
	}
	switch (message) {
		case WM_CREATE:{
			launch_btn = CreateWindow(L"BUTTON", L"Launch", WS_CHILD | WS_VISIBLE, 0, 0, 100, 25, hwnd, (HMENU)launch_btn_id, NULL, NULL);
		//	settings_btn = CreateWindow(L"BUTTON", L"Settings", WS_CHILD | WS_VISIBLE, 100, 0, 100, 25, hwnd, (HMENU)settings_btn_id, NULL, NULL);
			close_btn = CreateWindow(L"BUTTON", L"Close", WS_CHILD | WS_VISIBLE, 200, 0, 100, 25, hwnd, (HMENU)close_btn_id, NULL, NULL);
			skiptobnet_btn = CreateWindow(L"BUTTON", L"skiptobnet", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 25, 100, 25, hwnd, (HMENU)skiptobnet_id, NULL, NULL);
			windowmode_btn = CreateWindow(L"BUTTON", L"window mode", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 200, 25, 100, 25, hwnd, (HMENU)windowmode_id, NULL, NULL);
			showconsole_btn = CreateWindow(L"BUTTON", L"Show Console", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 100, 0, 100, 25, hwnd, (HMENU)showconsole_id, NULL, NULL);

			set_button_states();

			g_menu = CreatePopupMenu();
			AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
			AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			press_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESS));
			idle_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(IDLE));
			loading_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(WAITCURSOR));

			//SendMessage(g_hwnd, WM_SETCURSOR, (WPARAM)g_hwnd, (LPARAM)CURSOR_LOADING);

			break;
		}
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case launch_btn_id:{
					if (int error = (int)ShellExecute(g_hwnd, TEXT("open"), L"Diablo II.exe", str_to_wstr("-w -skiptobnet").c_str(), NULL, SW_SHOW) <= 32) {
						MessageBox(NULL, str_to_wstr("Unable to open Diablo II.exe\n\nShellExecute() error code: (" + int_to_str(error) + ")\n" + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
						break;
					}

					int state = SendMessage(windowmode_btn, BM_GETCHECK, NULL, NULL);
					if (state == BST_UNCHECKED) {
						old_d2windowlist.clear();
						new_d2windowlist.clear();
						EnumWindows(EnumWindowsProc, 1);
						HWND wnd = NULL;
						int count = 0;
						while (wnd == NULL) {
							this_thread::sleep_for(chrono::milliseconds(100));
							new_d2windowlist.clear();
							EnumWindows(EnumWindowsProc, 2);
							if (old_d2windowlist.size() != new_d2windowlist.size()) {
								std::sort(old_d2windowlist.begin(), old_d2windowlist.end());
								std::sort(new_d2windowlist.begin(), new_d2windowlist.end());
								std::vector<HWND> difference;
								std::set_difference(
									new_d2windowlist.begin(), new_d2windowlist.end(),
									old_d2windowlist.begin(), old_d2windowlist.end(),
									std::back_inserter(difference));
								if (difference.size() > 0)
									wnd = difference[0];
							}
							
							count++;
							if (count > 30)
								break;
						}					
						if (wnd) {
							DWORD PID;
							GetWindowThreadProcessId(wnd, &PID);
							if (int error = (int)ShellExecute(NULL, L"open", L"D2FS.exe", str_to_wstr(int_to_str(PID)).c_str(), NULL, SW_SHOW) <= 32)
								MessageBox(NULL, str_to_wstr("Unable to open D2FS.exe\n\nShellExecute() error code: " + int_to_str(error) + "\n" + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
						}
						else {
							MessageBox(NULL, L"Could not find Diablo II.exe", L"ERROR", MB_OK);
						}
					}

					break;
				}
				case close_btn_id:{
					PostQuitMessage(0);
					break;
				}
				case settings_btn_id:{
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(SETTINGSDLG), g_hwnd, SettingsDlgProc);

					break;
				}				
				case skiptobnet_id:{
					int state = SendMessage(skiptobnet_btn, BM_GETCHECK, NULL, NULL);
					edit_reg_config(CREG_KEY_CB_SKIPTOBNET, state);

					break;
				}
				case windowmode_id:{
					int state = SendMessage(windowmode_btn, BM_GETCHECK, NULL, NULL);
					edit_reg_config(CREG_KEY_CB_WINDOWMODE, state);

					break;
				}				
				case showconsole_id:{
					int state = SendMessage(showconsole_btn, BM_GETCHECK, NULL, NULL);
					edit_reg_config(CREG_KEY_CB_SHOWCONSOLE, state);

					if (state) {
						console.show();
						SetForegroundWindow(hwnd);
					}
					else
						console.hide();

					break;
				}
			}
			break;
		}
		case WM_SYSCOMMAND:{
			switch (wParam) {
				case SC_MINIMIZE:
					Minimize();
					return 0;
				case SC_CLOSE:
					Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
					PostQuitMessage(0);
return 0;
			}
			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(g_hwnd, &ps);

			//paint background
			HBRUSH hb_background = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);

			EndPaint(g_hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return 1;
		}
		case WM_SETCURSOR:{
			string classname = getwindowclassname((HWND)wParam);
			if (classname == "Button") {
				SetCursor(press_cursor);
				//SetCursor(token_cursor);
				return TRUE;
			}
			if (classname == "Edit")
				return FALSE;
			switch (LOWORD(lParam)) {
				case HTSYSMENU:
				case HTREDUCE:
				case HTZOOM:
				case HTCLOSE:
				case HTMENU:
				case HTHSCROLL:
				case HTVSCROLL:
					SetCursor(press_cursor);
					//SetCursor(token_cursor);
					return TRUE;
				case HTNOWHERE:
				case HTCLIENT:
				case HTCAPTION:
				case HTSIZE:
				case HTOBJECT:
				case HTHELP:
					SetCursor(idle_cursor);
					//SetCursor(token_cursor);
					return TRUE;
				case CURSOR_LOADING:
					SetCursor(loading_cursor);
					return TRUE;
			}
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		//case WM_NCACTIVATE:{
		//	if (wParam)
		//		mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);
		//	/*else
		//		UnhookWindowsHookEx(mhook);*/
		//	break;
		//}
		case WM_TRAYICON:{
			switch (lParam) {
				case WM_LBUTTONUP:
					if (ShowWindow(g_hwnd, SW_SHOW))
						Minimize();
					else
						Restore();
					break;
				case WM_RBUTTONDOWN:
					POINT pt;
					GetCursorPos(&pt);
					SetForegroundWindow(hwnd);
					UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, 0, hwnd, NULL);
					if (clicked == 3000) {
						Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
						PostQuitMessage(0);
					}
					if (clicked == 3001) {
						MessageBox(NULL, TEXT("Let Token Know"), TEXT("NOOOOOOOB"), MB_OK);
					}
					break;
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			PostQuitMessage(0);
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	switch (lParam) {
		case 1:{
			if (getwindowtext(hwnd) == "Diablo II") {
				old_d2windowlist.push_back(hwnd);
			}
			break;
		}
		case 2:{
			if (getwindowtext(hwnd) == "Diablo II") {
				new_d2windowlist.push_back(hwnd);
			}
			break;
		}
			
	}

	return TRUE;
}
BOOL CALLBACK SettingsDlgProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{
			SetWindowLongPtr(GetDlgItem(hwnd, SETTINGSDLG), DWLP_DLGPROC, (LONG_PTR)SettingsDlgProc);
			return TRUE;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	//MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);

	HWND wnd = GetForegroundWindow();
	POINT rc = getclientcursorpos(wnd);

	cout << rc.x << ", " << rc.y << "\t\t\t\t\t\t\r";
	if (wParam == WM_RBUTTONDOWN && move_window) {
		POINT client_cursor_pos = getclientcursorpos(GetForegroundWindow());
		lastwindow = ChildWindowFromPointEx(GetForegroundWindow(), client_cursor_pos, CWP_SKIPDISABLED | CWP_SKIPINVISIBLE);

		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(lastwindow, &wp);
		RECT move = wp.rcNormalPosition;

		RECT rclient = getclientrect(lastwindow);
		POINT crclient = getclientcursorpos(lastwindow);

		cout << lastwindow << " " << getwindowtext(lastwindow) << ": " << move.left << ", " << move.top << ", " << move.right - move.left << ", " << move.bottom - move.top <<
			"\tclient: " << rclient.left << " " << rclient.right << " " << rclient.top << " " << rclient.bottom <<
			"\tcursor: " << crclient.x << ", " << crclient.y << '\n';
	}
	return CallNextHookEx(mhook, code, wParam, lParam);
}
LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		#pragma region virtual keycode processing
		wchar_t name[256];
		LPARAM lparam = 1;
		lparam += key.scanCode << 16;
		lparam += key.flags << 24;
		GetKeyNameText(lparam, (LPTSTR)name, 256);
		string sname = wstr_to_str((wstring)name);
		if ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0 && key.vkCode != VK_LMENU && key.vkCode != VK_RMENU) {
			key.vkCode += 256;
			sname.insert(0, "ALT+");
		}
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 && key.vkCode != VK_LCONTROL && key.vkCode != VK_RCONTROL) {
			key.vkCode += 256 * 2;
			sname.insert(0, "CTRL+");
		}
		if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 && key.vkCode != VK_LSHIFT && key.vkCode != VK_RSHIFT) {
			key.vkCode += 256 * 4;
			sname.insert(0, "SHIFT+");
		}
		if ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 && key.vkCode != VK_LWIN) {
			key.vkCode += 256 * 8;
			sname.insert(0, "LEFT WINDOWKEY+");
		}
		if ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0 && key.vkCode != VK_RWIN) {
			key.vkCode += 256 * 8;
			sname.insert(0, "RIGHT WINDOWKEY+");
		}

		//cout << key.vkCode << " " << sname << '\n';
#pragma endregion	
		#pragma region functions
		if (move_window && lastwindow != NULL &&
			((key.vkCode >= 1061 && key.vkCode <= 1064) || /*shift + arrows*/
			(key.vkCode >= 549 && key.vkCode <= 552) || /*ctrl + arrows*/
			(key.vkCode >= 37 && key.vkCode <= 40) || /*arrows*/
			(key.vkCode >= 1573 && key.vkCode <= 1576))) /*ctrl + shift + arrows*/
		{
			WINDOWPLACEMENT wp;
			wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(lastwindow, &wp);
			RECT r;
			r = wp.rcNormalPosition;
			MoveWindow(lastwindow,
				key.vkCode == 1061 ? r.left - 5 : key.vkCode == 37 ? r.left - 1 : key.vkCode == 1063 ? r.left + 5 : key.vkCode == 39 ? r.left + 1 : r.left,
				key.vkCode == 1062 || key.vkCode == 1574 ? r.top - 5 : key.vkCode == 38 || key.vkCode == 550 ? r.top - 1 : key.vkCode == 1064 || key.vkCode == 1576 ? r.top + 5 : key.vkCode == 40 || key.vkCode == 552 ? r.top + 1 : r.top,
				key.vkCode == 549 ? r.right - r.left - 1 : key.vkCode == 1573 ? r.right - r.left - 5 : key.vkCode == 551 ? r.right - r.left + 1 : key.vkCode == 1575 ? r.right - r.left + 5 : r.right - r.left,
				key.vkCode == 550 ? r.bottom - r.top + 1 : key.vkCode == 1574 ? r.bottom - r.top + 5 : key.vkCode == 552 ? r.bottom - r.top - 1 : key.vkCode == 1576 ? r.bottom - r.top - 5 : r.bottom - r.top,
				true);
			GetWindowPlacement(lastwindow, &wp);
			r = wp.rcNormalPosition;
			RedrawWindow((HWND)lastwindow, NULL, NULL, RDW_INVALIDATE);

			RECT rclient = getclientrect(lastwindow);
			POINT crclient = getclientcursorpos(lastwindow);

			cout << getwindowtext(lastwindow) << ": " << r.left << ", " << r.top << ", " << r.right - r.left << ", " << r.bottom - r.top <<
				"\tclient: " << rclient.left << " " << rclient.right << " " << rclient.top << " " << rclient.bottom <<
				"\tcursor: " << crclient.x << ", " << crclient.y << '\n';
		}
		else if (key.vkCode >= 37 && key.vkCode <= 40) {
			HWND wnd = GetForegroundWindow();
			POINT pt = getclientcursorpos(wnd);
			if (key.vkCode == 37)//left
				pt.x--;
			if (key.vkCode == 38)//up
				pt.y--;
			if (key.vkCode == 39)//right
				pt.x++;
			if (key.vkCode == 40)//down
				pt.y++;
			cout << pt.x << ", " << pt.y << "\t\t\t\t\t\t\t\r";
			cout.flush();
			ClientToScreen(wnd, &pt);
			SetCursorPos(pt.x, pt.y);
		}
	/*	if (is_hotkey_dialog_visible) {

		}*/
		if (key.vkCode == WMC_HOTKEY) {
			/*static bool trap = false;
			if (!trap) {
				trap = true;
				HWND wnd = GetForegroundWindow();
				RECT rcmappedclient = getmappedclientrect(wnd);
				ClipCursor(&rcmappedclient);				
			}
			else {
				trap = false;
				ClipCursor(NULL);
			}*/
			/*POINT pt;
			pt.x = 532;
			pt.y = 469;
			ClientToScreen(GetForegroundWindow(), &pt);
			SetCursorPos(pt.x, pt.y);*/
		}
					
#pragma endregion
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}