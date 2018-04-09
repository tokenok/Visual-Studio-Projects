#include "include.h"


#include <fstream>
#include <intrin.h>
#include <bitset>
#include <conio.h>


using namespace std;


Button testbtn("test button", WS_CHILD | WS_VISIBLE, 0, 30, 100, 25, NULL, get_unique_id(),
	[](WPARAM /*wParam*/, LPARAM /*lParam*/) -> void {cout << "brutal\n"; return; },
	{
		RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
		RGB(45, 45, 45), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
		RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//SELECTED ON: background/border/text						
		RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10)			//IDLE OFF: background/border/text
	},
	NULL, NULL, NULL);

		StaticControl teststat("test static", WS_CHILD | WS_VISIBLE, 0, 0, 100, 25, NULL, get_unique_id(),
			nullptr, {RGB(10, 10, 10), RGB(148, 0, 211), RGB(148, 0, 211)}, NULL, NULL, NULL, NULL);

void wave_to_bmp(LPSTR wavfile, LPSTR output) {
	CWave twave;
	twave.Load(wavfile);
	BYTE* data = twave.GetData();
	int x, y;
	get_rect_from_area(twave.GetSize() / 3, &x, &y);
	array_to_bmp(output, data, y, x);
}

void bmp_to_wave() {

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgument*/, int /*cmdShow*/) {
	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, (LPCWSTR)"unique mutex name ok");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}

	CONSOLE(true);

	//CWave cwave;
	//cwave.Load("yee.wav");
	//cwave.Play();

	//cwave.print_info();


	/*BYTE* datas = cwave.GetData();

	int x, y;
	get_rect_from_area(cwave.GetSize(), &x, &y);

	array_to_bmp("shirt.bmp", datas, y, x);*/
	/*int x, y;
	BYTE* buffer = bmp_to_array("shirt.bmp", y, x);
	cwave.SetData(buffer);
	cwave.Play();
	cwave.Save("shirt.wav");*/


	//wave_to_bmp("stinkfist.wav", "stinkfist.bmp");

	/*int x, y;
	BYTE* dat = bmp_to_array("stinkfist.bmp", x, y);
	CWave wav;
	wav.Load("stinkfist.wav");
	wav.SetData(dat);
	wav.Save("stinkfist2.wav");*/

	//wave_to_bmp("shirt.wav", "shirt2.bmp");
	//bmp_to_wave();





	if (FindWindow(NULL, TEXT("LoL Patcher")) == NULL && FindWindow(NULL, TEXT("PVP.net Client")) == NULL && FindWindow(NULL, TEXT("League of Legends (TM) Client")) == NULL) {
		//ShellExecute(NULL, TEXT("open"), str_to_wstr(league_of_legends_directory).c_str(), NULL, NULL, SW_SHOW);
	}

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	MSG msg;

	init_wnd_class(hInstance, szClassName, WindowProcedure, TOKENICON);

	g_hwnd = CreateWindowEx(NULL, szClassName, TEXT("Disable Key"), WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, HWND_DESKTOP, NULL, hInstance, NULL);
	
	if (showonstart) ShowWindow(g_hwnd, SW_SHOW);

	init_notify_icon_data(notifyicondata, g_hwnd, WM_TRAYICON, TOKENICON, VERSION);
	Shell_NotifyIcon(NIM_ADD, &notifyicondata);

	ProtectProcess();

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ReleaseMutex(Mutex_handle);
	Shell_NotifyIcon(NIM_DELETE, &notifyicondata);
	return msg.wParam;
}
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		ShowWindow(g_hwnd, SW_HIDE);
		return 0;
	}
	switch (message) {
		case WM_CREATE:{
			static bool firstrun = true;
			if (firstrun) {
				firstrun = false;
				if (!showonstart) {
					ShowWindow(g_hwnd, SW_FORCEMINIMIZE);
					ShowWindow(g_hwnd, SW_HIDE);
				}
			}

			

			testbtn.parent = hwnd;
			teststat.parent = hwnd;

			auto btnproc = [](WPARAM /*wParam*/, LPARAM /*lParam*/) -> void {
				cout << "brutal\n";
				return;
			};
			
			CreateChildren(hwnd);

			tog_stat = CreateWindow(TEXT("STATIC"), TEXT("Enter key is currently: DISABLED"), WS_CHILD | WS_VISIBLE, 88, 85, 220, 15, hwnd, NULL, NULL, NULL);
			tog_btn = CreateWindow(TEXT("BUTTON"), TEXT("ENABLE"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 148, 135, 100, 50, hwnd, (HMENU)1000, 0, 0);
			SetWindowSubclass(tog_btn, MyButtonWndProc, 0, 0);

			DestroyMenu(g_menu);
			g_menu = CreatePopupMenu();
			for (int i = 0; i < 30; i++)
				AppendMenu(g_menu, MF_STRING, 2, TEXT("DIE WEEB"));
			AppendMenu(g_menu, MF_STRING, 1, TEXT("Exit"));
			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(g_hwnd, &ps);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			static int i = 0;
			if (i > 120) 
				i = 0;
			i++;

			COLORREF background = color(i, 120, 180);
			HBRUSH hb_background = CreateSolidBrush(background);
			RECT rc = getclientrect(g_hwnd);
			FillRect(hDC, &rc, hb_background);
			DeleteObject(hb_background);

			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);

			EndPaint(g_hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return 0;
		}
		case WM_CTLCOLORSTATIC:{
			if (g_tempbrush != NULL) {
				DeleteObject(g_tempbrush);
				g_tempbrush = NULL;
			}
			HDC hdcStatic = (HDC)wParam;
			if ((HWND)lParam == tog_stat) {	
				static int j = 0;
				if (j > 16)
					j = 0;
				j++;

				COLORREF col = color(j, 16, 180);

				SetTextColor(hdcStatic, col);
				SetBkColor(hdcStatic, RGB(10, 10, 10));

				g_tempbrush = CreateSolidBrush(RGB(10, 10, 10));
				return (INT_PTR)g_tempbrush;
			}
			break;
		}
		case WM_DRAWITEM:{
			LPDRAWITEMSTRUCT pdis = (DRAWITEMSTRUCT*)lParam;
			SIZE size;

			if (pdis->hwndItem == tog_btn) {
				static int j = 0;
				if (clickblock)
					j -= 3;
				int x = pdis->rcItem.right < pdis->rcItem.bottom ? pdis->rcItem.right : pdis->rcItem.bottom;
				for (int i = 0; i < x / 2; i++) {
					if (j > 10)
						j = 0;					
					j++;
					HBRUSH tbrush = CreateSolidBrush(color(j, 10, 180));
					RECT trc = {i, i, pdis->rcItem.right - i, pdis->rcItem.bottom - i};
					FrameRect(pdis->hDC, &trc, tbrush);
					DeleteObject(tbrush);
				}
				clickblock = false;

				std::string text = getwindowtext(pdis->hwndItem);
				GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
				SetTextColor(pdis->hDC, RGB(0, 0, 0));
				SetBkMode(pdis->hDC, TRANSPARENT);
				//SetBkColor(pdis->hDC, RGB(255, 255, 0));
				TextOut(pdis->hDC, 
					((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
					((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2, 
					str_to_wstr(text).c_str(), text.length());

				return TRUE;
			}
			break;
		}
		case WM_SYSCOMMAND:{
			switch (wParam) {
				case SC_MINIMIZE:
					ShowWindow(g_hwnd, SW_HIDE);
					return 0;
				/*case SC_CLOSE:
					Shell_NotifyIcon(NIM_DELETE, &notifyicondata);
					PostQuitMessage(0);
					return 0;*/
			}
			break;
		}
		case WM_TRAYICON:{
			switch (lParam) {
				case WM_LBUTTONUP:{
					if (ShowWindow(g_hwnd, SW_SHOW))
						ShowWindow(g_hwnd, SW_HIDE);
					else {
						ShowWindow(g_hwnd, SW_SHOW);
						SetForegroundWindow(g_hwnd);
					}
					break;
				}
				case WM_RBUTTONDOWN:{
					POINT pt;
					GetCursorPos(&pt);
					SetForegroundWindow(hwnd);
					UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, 0, hwnd, NULL);
					if (clicked == 1) {
						Shell_NotifyIcon(NIM_DELETE, &notifyicondata);
						PostQuitMessage(0);
					}
					if (clicked == 2) {
						dieweeb = true;

						mhook = SetWindowsHookEx(WH_MOUSE_LL, kbhookProc, NULL, 0);

						SetTimer(g_hwnd, timer4, 0, timerProc);
						HMENU hmenu = GetSystemMenu(g_hwnd, FALSE);
						EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
						SetWindowLong(g_hwnd, GWL_STYLE, GetWindowLong(g_hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
						SetWindowLong(g_hwnd, GWL_STYLE, GetWindowLong(g_hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);

						HWND hdesktop = GetDesktopWindow();
						RECT rd;
						GetClientRect(hdesktop, &rd);
						int width = rd.right, height = rd.bottom;
						diw = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, 
							TEXT("STATIC"), TEXT("DIE WEEB"),
							WS_CHILD | WS_POPUP | WS_VISIBLE | SS_CENTER, 
							(rd.right / 2) - (width / 2), 
							(rd.bottom / 2) - (height / 2),
							width,
							height, 
							NULL, NULL, NULL, NULL);
						SetWindowSubclass(diw, MyStaticWndProc, 0, 0);

						SetWindowLongPtr(g_hwnd, GWL_EXSTYLE, GetWindowLongPtr(g_hwnd, GWL_EXSTYLE) | WS_EX_TOPMOST);
						SetWindowPos(g_hwnd, diw, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}
			}
			break;
		}
		case WM_NCACTIVATE:{
			if (wParam) {
				SetTimer(g_hwnd, timer1, 0, timerProc);
				SetTimer(g_hwnd, timer2, 100, timerProc);
				SetTimer(g_hwnd, timer3, 0, timerProc);
			}
			else {
				KillTimer(g_hwnd, timer1);
				KillTimer(g_hwnd, timer2);
				KillTimer(g_hwnd, timer3);
			}
			break;
		}
		case WM_CLOSE:{
			if (FindWindow(NULL, str_to_wstr(targetwindow).c_str()) != NULL)
				return 0;
		}
		case WM_DESTROY:{
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK hotkeydlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{
			update_hotkeykeys();
			SetWindowLongPtr(GetDlgItem(hwnd, HOTKEYBOX), DWLP_DLGPROC, (LONG_PTR)hotkeydlgProc);
			return TRUE;
		}
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case BTN_OK:{
					global_change_hotkey = (WORD)SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_GETHOTKEY, 0, 0);
					if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 1)
						global_change_hotkey += 768;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 4)
						global_change_hotkey -= 768;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 3)
						global_change_hotkey += 768;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 6)
						global_change_hotkey -= 768;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 6)
						global_change_hotkey -= 768;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 12)
						global_change_hotkey -= 256 * 11;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 14)
						global_change_hotkey -= 256 * 11;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 9)
						global_change_hotkey -= 256 * 5;
					else if ((global_change_hotkey - (DWORD)LOBYTE(global_change_hotkey)) / 256 == 11)
						global_change_hotkey -= 256 * 5;
					if (global_change_hotkey > ((256 * 7) + 255))
						global_change_hotkey -= 256 * 8;

					bool newkey = true;
					for (auto & a : hotkeys) {
						if (global_change_hotkey == a) {
							newkey = false;
							std::cout << "Key already being used as hotkey" << '\n';
							global_change_hotkey = 0;
							SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_SETHOTKEY, 0, 0);
							SetFocus(GetDlgItem(hwnd, KEYCONTROL));
							break;
						}
					}
					if (!global_change_hotkey)
						global_change_hotkey = 1;
					if (newkey)
						EndDialog(hwnd, KEYCONTROL);
					break;
				}
			}
			break;
		}
		case WM_CLOSE:{
			global_change_hotkey = 0;
			//fall through
		}
		case WM_DESTROY:{
			/*is_hotkey_dialog_visible = false;*/
			EndDialog(hwnd, BTN_OK);
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
LRESULT CALLBACK MyButtonWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR /*uIdSubclass*/, DWORD_PTR/* dwRefData*/) {
	static bool clicked = false;
	switch (msg) {
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:{
			clicked = true;
			return 0;
		}
		case WM_LBUTTONUP:{
			if (clicked) {
				clickblock = true;
				clicked = false;
				if (state) {
					state = false;
					SetWindowText(tog_btn, TEXT("DISABLE"));
					SetWindowText(tog_stat, TEXT("Enter key is currently: ENABLED"));
				}
				else {
					state = true;
					SetWindowText(tog_btn, TEXT("ENABLE"));
					SetWindowText(tog_stat, TEXT("Enter key is currently: DISABLED"));
				}
			}
			return 0;
		}
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK MyStaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR /*uIdSubclass*/, DWORD_PTR /*dwRefData*/) {
	switch (msg) {
		case WM_PAINT:{
			RECT rc;
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rc);
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(255, 0, 0));
			std::string text = getwindowtext(hwnd);

			HWND hdesktop = GetDesktopWindow();
			RECT rd;
			GetClientRect(hdesktop, &rd);
			HFONT font_a = CreateFont(rd.bottom, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("SYSTEM"));
			SelectObject(hDC, font_a);

			DrawText(hDC, str_to_wstr(text).c_str(), text.length(), &rc, DT_CENTER | DT_VCENTER);
			EndPaint(hwnd, &ps);
			return 0;
		}
	}  

	return DefSubclassProc(hwnd, msg, wParam, lParam);
}
VOID CALLBACK timerProc(HWND hwnd, UINT /*message*/, UINT_PTR id, DWORD /*time*/) {
	if (id == timer1) {
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}
	if (id == timer2) {
		RedrawWindow(tog_btn, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE);
	}
	if (id == timer3) {
		RECT rc = getclientrect(tog_stat);
		int rate = 15;
		for (int i = 0; i < rc.right; i += rc.right / rate) {
			RECT r = {i, 0, i + (rc.right / rate), rc.bottom};
			RedrawWindow(tog_stat, &r, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
		}
	}
	if (id == timer4) {
		static int time = 0;
		time++;
		if (time > 1000) {
			time = 0;
			KillTimer(g_hwnd, timer4);
			dieweeb = false;
			BlockInput(false);
			UnhookWindowsHookEx(mhook);
			DestroyWindow(diw);
		}
		static std::string str = "(_(_)=";
		str += "=";
		if (str.size() > 25)
			str = "(_(_)=";
		SetWindowText(g_hwnd, str_to_wstr(str + "D").c_str());

		WINDOWPLACEMENT wp;
		GetWindowPlacement(g_hwnd, &wp);
		ShowWindow(g_hwnd, SW_SHOW);
		SetForegroundWindow(g_hwnd);

		RedrawWindow(diw, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

		BlockInput(true);

		RECT rd = getclientrect(GetDesktopWindow());
		int x = rand() % rd.right;
		int y = rand() % rd.bottom;
		SetWindowPos(g_hwnd, diw, x, y, 0, 0, SWP_NOSIZE);
		x = rand() % rd.right;
		y = rand() % rd.bottom;
		SetCursorPos(x, y);
	}
}
LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN) {
		if (key.vkCode == 369 - 256) {
			std::cout << "yea" << '\n';
			keydownup(VK_RETURN, 0);
			sendkeys("gl hf", GetForegroundWindow(), 10);
			keydownup(VK_RETURN, 0);

			keydownup(VK_RETURN, 0);
			sendkeys("gg wp", GetForegroundWindow(), 10);
			keydownup(VK_RETURN, 0);
		}
	}
	/*if (key.vkCode == disabled_key && state && (targetwindow != "" ? FindWindow(NULL, str_to_wstr(targetwindow).c_str()) == GetForegroundWindow() : true) || dieweeb)
		return 1;*/
	
	return CallNextHookEx(NULL, code, wParam, lParam);
}
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	if (dieweeb)
		return 1;
	return CallNextHookEx(NULL, code, wParam, lParam);
}

