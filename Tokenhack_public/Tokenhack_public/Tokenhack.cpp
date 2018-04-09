#include "include.h"

#include <conio.h>

using namespace std;

HWND rainbowtest;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*args*/, int /*iCmdShow*/) {
	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, L"THE MUTEX OF POWER");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Error: Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}
	ifstream filein;
	filein.open("config.txt");
	if (!filein.is_open()) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		return 0;
	}
	filein.close();
	
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_HOTKEY_CLASS;
	InitCommonControlsEx(&icex);

	init_register_class(hInstance);

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);
	//mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);

	g_hwnd = CreateWindow(className, VERSION, WS_OVERLAPPEDWINDOW /*WS_POPUP|*/  | WS_CLIPCHILDREN, 350, 150, windowwidth, functionnameheight*totalonfunctions + 130, NULL, NULL, hInstance, NULL);

	vector<TabButton*> tabbuttons = custom_controls.tabbuttons;
	for (UINT i = 0; i < tabbuttons.size(); i++) {
		if (tabbuttons[i]->group_name == grp_function_tab_button) {
			if (0 == i) {
				tabbuttons[i]->toggle_state = 1;
				SendMessage(g_hwnd, WM_COMMAND, (WPARAM)tabbuttons[i]->id, (LPARAM)tabbuttons[i]->handle);
			}
		}
	}

	if (TBTN_SHOWONSTART.toggle_state)	ShowWindow(g_hwnd, SW_SHOW);

	InitNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	EnumChildWindows(g_hwnd, EnumChildProc, DESTROY_WINDOWS);

	DeleteObject(Font_a);

	UnhookWindowsHookEx(mhook);
	UnhookWindowsHookEx(kbhook);
	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

	ClipCursor(NULL);
	
	ShowWindow(g_hwnd, SW_HIDE);
	CloseHandle(Mutex_handle);

	kill_all_OCR_auto_roll = true;
	for (auto a : original_window_titles) 
		SetWindowText(a.first, str_to_wstr(a.second).c_str());
	if (original_window_titles.size() > 0)
		this_thread::sleep_for(chrono::milliseconds(100));	

	cout << "EXITING" << '\n';
	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(BYE), NULL, SND_SYNC | SND_RESOURCE);
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		Minimize();
		return 0;
	}
	switch (message) {
		case WM_CREATE:{
			static bool firstrun = true;
			if (firstrun) {
				if (TBTN_SHOWONSTART.toggle_state) {
					ShowWindow(g_hwnd, SW_FORCEMINIMIZE);
					Minimize();
				}

				CTB_VOLUME.start = false;

				firstrun = false;
			}
			ifstream filein;
			filein.open("config.txt");
			if (!filein) {
				MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
				PostQuitMessage(0);
			}
			while (filein.is_open()) {
				totalonfunctions = 0;
				functionnameyoffsum = tokenhackfunctionwindowxposoffset + (tabbuttonheight - functionnameheight) + 1;
				while (filein.good()) {
					string line;
					getline(filein, line);
					line = line.substr(0, line.find('*'));
					line.erase(remove(line.begin(), line.end(), '\t'), line.end());
					string val = line.substr(line.find(':') + 1, line.size());
					line.erase(remove(line.begin(), line.end(), ' '), line.end());
					transform(line.begin(), line.end(), line.begin(), ::tolower);
					int on = str_to_int(val);
					string var_name = line.substr(0, line.find(':'));
					var_name = var_name.substr(0, var_name.find('['));
					var_name = var_name.substr(0, var_name.find('('));
					string key = line.substr(line.find('(') + 1, line.find(')') - line.find('(') - 1);
					if (var_name.size() == 0)
						continue;
					//set toggle buttons states
					vector<ToggleButton*> togglebuttons = custom_controls.togglebuttons;
					for (auto & a: togglebuttons) {
						if (var_name == a->string_in_file) 
							a->toggle_state = on;
					}
					for (auto & a: statcolors) {
						if (var_name == a->text_in_file) {
							a->red = str_to_int(val.substr(0, val.find(',')));
							a->green = str_to_int(val.substr(val.find(',') + 1, val.rfind(',') - val.find(',') - 1));
							a->blue = str_to_int(val.substr(val.rfind(',') + 1, val.size()));
						}
					}
					if (var_name == "gamepath") gamepath = val;
					else if (var_name == "targetlines") targetlines = val;
					else if (var_name == "volume") {
						volume = (WORD)str_to_int(val);
						set_volume((WORD)volume);
					}
					else if (var_name == "transmutebuttonpos") {
						if (val == "auto") {
							transmutebuttonx = -1;
							transmutebuttony = -1;
						}
						else {
							transmutebuttonx = str_to_int(val.substr(0, val.find(',')));
							transmutebuttony = str_to_int(val.substr(val.find(',') + 1, val.size()));
						}
					}
					else if (var_name == "layeredwindowfix") layered_window_fix = str_to_int(val);
					else if (var_name == "ocrtitledisplay") ocr_title_display = str_to_int(val);
					else if (var_name == "ocrsamestatstimeout") ocr_same_stats_timout = str_to_int(val);
					else if (var_name == "ocrnostatstimeout") ocr_no_stats_timeout = str_to_int(val);
					else if (var_name == "layeredwindowdelay") layered_window_delay = str_to_int(val);
					else if (line.size() > 0 && line[0] == '>') {
						line.erase(0, 1);
						if (line.size() == 0)
							continue;
						stats_in_file.push_back(line);
					}
					else {
						for (auto & a: tokenhackfunctions) {
							if (var_name == a->name_in_file) {
								functionnameyoffsum += functionnameheight;

								a->set_on_state(on);
								a->hotkey_key = key.size() == 0 ? 0 : (WORD)str_to_int(key);
								a->config_edit_control.window_text = key;
								on > 1 ? a->checkbox_button.window_text = int_to_str(on) : a->checkbox_button.window_text = "T";

								a->checkbox_button.ypos = functionnameyoffsum;
								a->hotkey_button.ypos = functionnameyoffsum;
								a->static_control.ypos = functionnameyoffsum;

								a->checkbox_button.Create(hwnd);
								a->hotkey_button.Create(hwnd);
								a->static_control.Create(hwnd);

								if (a->hotkey_key)
									changebuttontext(a->hotkey_button.handle, a->hotkey_key);
								else
									a->hotkey_button.window_text = "Click to set hotkey";

								a->config_static_control.window_text = a->hotkey_button.window_text;
								setbuttonstate(a->hotkey_button.handle, a->on);
								totalonfunctions++;
								break;
							}
						}
					}
				}
				filein.close();
			}
			
			TBTN_CONSOLE.toggle_state ? CONSOLE_ON() : FreeConsole();

			statbox_display(hwnd);
			
			auto cond = [](BasicControl* a) -> bool {
				if (a->className == wstr_to_str(normalbuttonclassName)) {
					if (a->group_name == grp_tokenhack_offset)
						a->ypos = (totalonfunctions * functionnameheight) + 6 + 35;
				}
				if (a->className == wstr_to_str(tabbuttonclassName))
					if (a->group_name == grp_stat_tab_button)
						return false;
				return (((a->owner_name > own_BEGIN_tokenhack && 
					a->owner_name < own_END_tokenhack) || a->owner_name == own_root_window) &&
					a->group_name != grp_tokenhack_function);
			};
			custom_controls.createChildren(hwnd, cond);

			SetWindowPos(hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250, functionnameheight * totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);

			rainbowtest = CreateWindow(TEXT("STATIC"), TEXT("DIE WEEBS"), WS_VISIBLE | WS_CHILD, 250, 0, 100, 15, g_hwnd, (HMENU)NULL, NULL, NULL);
			
			DestroyMenu(g_menu);
			g_menu = CreatePopupMenu();
			if (CTB_VOLUME.current_val > 0)
				for (int freshmeatx = 0; freshmeatx < 31; freshmeatx++)
					AppendMenu(g_menu, MF_STRING, 3003, TEXT("FRESHMEAT"));
			AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
			AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
			AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			break;
		}
		case WM_COMMAND:{
			if (lParam != 0)
				SendMessage((HWND)lParam, MY_COMMAND, wParam, lParam);
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

			//border of tokenhackfunction tab
			RECT tfb;
			tfb.right = functionnamexoff + functionnamewidth + 1;
			tfb.left = 0;
			tfb.top = tabbuttonheight - 1;
			tfb.bottom = (functionnameheight * totalonfunctions) + tabbuttonheight + (tokenhackfunctionwindowxposoffset * 2) + 1 + 1;
			SetDCPenColor(hDC, RGB(100, 100, 100));
			Rectangle(hDC, tfb.left + 1, tfb.top, tfb.right, tfb.bottom);
			Rectangle(hDC, tfb.left, tfb.top + 1, tfb.right + 1, tfb.bottom - 1);

			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);

			EndPaint(g_hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return 1;
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
		case WM_SIZE:{
			RECT t;
			RECT r;
			GetClientRect(hwnd, &t);
			GetWindowRect(hwnd, &r);
			int curheight = t.bottom - bottomheight + (r.bottom - r.top - t.bottom) + (tokenhackfunctionwindowxposoffset) + 1 + 1 + 1;
			int curwidth = t.right - windowwidth + 25 - tokenhackfunctionwindowxposoffset;
			int minheight = totalonfunctions * functionnameheight + (tokenhackfunctionwindowxposoffset * 2) + 1 + 1 + 1;
			int minwidth = 0 - tokenhackfunctionwindowxposoffset + 5;
			vector<TabButton*> tabbuttons = custom_controls.tabbuttons;
			for (auto & a: tabbuttons) {
				if (a->group_name == grp_stat_tab_button)
					minwidth += a->width;
			}
			MoveWindow(statbox, statboxxoff, tabbuttonheight - 1, ((curwidth < minwidth) ? minwidth : curwidth), ((curheight < minheight) ? minheight : curheight), true);
			if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
				is_drawing_static = true;
				for (auto & a: tokenhackfunctions) {
					RedrawWindow(a->static_control.handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
			break;
		}
		case WM_CTLCOLORSTATIC:{
			if (g_tempbrush != NULL) {
				DeleteObject(g_tempbrush);
				g_tempbrush = NULL;
			}	
			HDC hdcStatic = (HDC)wParam;
			if ((HWND)lParam == statbox) {				
				SetTextColor(hdcStatic, RGB(0, 118, 255));
				SetBkColor(hdcStatic, RGB(10, 10, 10));

				HDC whydoineedthis = GetDC(statbox);
				RECT rec = getclientrect(statbox);
				g_tempbrush = CreateSolidBrush(RGB(0, 118, 255));
				FrameRect(whydoineedthis, &rec, g_tempbrush);
				DeleteObject(g_tempbrush);
				g_tempbrush = NULL;
				ReleaseDC(statbox, whydoineedthis);

				g_tempbrush = CreateSolidBrush(RGB(10, 10, 10));
				return (INT_PTR)g_tempbrush;
			}
			if (getwindowclassname((HWND)lParam) != "Static")
				break;
			static int count = 0;
			count++;
			BYTE red1 = 255, gre1 = 0, blu1 = 105;
			if (TBTN_RAINBOW.toggle_state) {
				static int rainbowc = 1;
				static int rainbowc1 = 12;
				if (rainbowc > 12)
					rainbowc = 1;
				if (rainbowc == 1) { red1 = 255; gre1 = 0; blu1 = 0; /*cout << "\tred1"; */ }
				if (rainbowc == 2) { red1 = 255; gre1 = 150; blu1 = 0; /*cout << "\torange"; */ }
				if (rainbowc == 3) { red1 = 255; gre1 = 255; blu1 = 0; /*cout << "\tyellow"; */ }
				if (rainbowc == 4) { red1 = 105; gre1 = 255; blu1 = 0; /*cout << "\tlight gre1en"; */ }
				if (rainbowc == 5) { red1 = 0; gre1 = 255; blu1 = 0; /*cout << "\tgre1en"; */ }
				if (rainbowc == 6) { red1 = 0; gre1 = 255; blu1 = 150; /*cout << "\tturquoise"; */ }
				if (rainbowc == 7) { red1 = 0; gre1 = 255; blu1 = 255; /*cout << "\tlight blu1e"; */ }
				if (rainbowc == 8) { red1 = 0; gre1 = 105; blu1 = 255; /*cout << "\tblu1e"; */ }
				if (rainbowc == 9) { red1 = 0; gre1 = 0; blu1 = 255; /*cout << "\tdark blu1e"; */ }
				if (rainbowc == 10) { red1 = 150; gre1 = 0; blu1 = 255; /*cout << "\tpurple"; */ }
				if (rainbowc == 11) { red1 = 255; gre1 = 0; blu1 = 255; /*cout << "\tmagenta"; */ }
				if (rainbowc == 12) { red1 = 255; gre1 = 0; blu1 = 105; /*cout << "\tpink"; */ }				
				rainbowc++;
				if (count % totalonfunctions == 0) {
					if ((HWND)lParam == tokenhackfunctions[tokenhackfunctions.size() - 1]->static_control.handle)
						count = 0;
					count = 0;
					is_drawing_static = false;
					rainbowc = rainbowc1;
					rainbowc1--;
					if (rainbowc1 == 0)
						rainbowc1 = 12;
				}
			}


			if ((HWND)lParam == rainbowtest) {
				static int rainbowco = 0;
				if (rainbowco == 1) { red1 = 255; gre1 = 0; blu1 = 0; /*cout << "\tred1"; */ }
				if (rainbowco == 2) { red1 = 255; gre1 = 150; blu1 = 0; /*cout << "\torange"; */ }
				if (rainbowco == 3) { red1 = 255; gre1 = 255; blu1 = 0; /*cout << "\tyellow"; */ }
				if (rainbowco == 4) { red1 = 105; gre1 = 255; blu1 = 0; /*cout << "\tlight gre1en"; */ }
				if (rainbowco == 5) { red1 = 0; gre1 = 255; blu1 = 0; /*cout << "\tgre1en"; */ }
				if (rainbowco == 6) { red1 = 0; gre1 = 255; blu1 = 150; /*cout << "\tturquoise"; */ }
				if (rainbowco == 7) { red1 = 0; gre1 = 255; blu1 = 255; /*cout << "\tlight blu1e"; */ }
				if (rainbowco == 8) { red1 = 0; gre1 = 105; blu1 = 255; /*cout << "\tblu1e"; */ }
				if (rainbowco == 9) { red1 = 0; gre1 = 0; blu1 = 255; /*cout << "\tdark blu1e"; */ }
				if (rainbowco == 10) { red1 = 150; gre1 = 0; blu1 = 255; /*cout << "\tpurple"; */ }
				if (rainbowco == 11) { red1 = 255; gre1 = 0; blu1 = 255; /*cout << "\tmagenta"; */ }
				if (rainbowco == 12) { red1 = 255; gre1 = 0; blu1 = 105; /*cout << "\tpink"; */ }
				rainbowco++;
				if (rainbowco > 12)
					rainbowco = 1;
				SetTextColor(hdcStatic, RGB(red1, gre1, blu1));
				SetBkColor(hdcStatic, RGB(0, 0, 0));
				g_tempbrush = CreateSolidBrush(RGB(0, 0, 0));
				return (INT_PTR)g_tempbrush;
			}


			for (auto & a: tokenhackfunctions) {
				if ((HWND)lParam == a->static_control.handle) {
					SetTextColor(hdcStatic, TBTN_RAINBOW.toggle_state ? RGB(red1, gre1, blu1) : a->static_control.color_scheme.text_idle_on);
					SetBkColor(hdcStatic, a->static_control.color_scheme.background_idle_on);
					g_tempbrush = CreateSolidBrush(a->static_control.color_scheme.border_idle_on);
					return (INT_PTR)g_tempbrush;
				}
			}
			vector<StaticControl*> staticcontrols = custom_controls.staticcontrols;
			for (auto & a: staticcontrols) {
				if ((HWND)lParam == a->handle) {
					SetTextColor(hdcStatic, TBTN_RAINBOW.toggle_state ? RGB(red1, gre1, blu1) : a->color_scheme.text_idle_on);
					SetBkColor(hdcStatic, a->color_scheme.background_idle_on);
					g_tempbrush = CreateSolidBrush(a->color_scheme.border_idle_on);
					return (INT_PTR)g_tempbrush;
				}
			}
			SetTextColor(hdcStatic, RGB(100, 100, 100));
			SetBkColor(hdcStatic, RGB(10, 10, 10));
			g_tempbrush = CreateSolidBrush(RGB(254, 33, 61));
			return (INT_PTR)g_tempbrush;
		}
		case WM_SETCURSOR:{			
			if (getwindowclassname((HWND)wParam) == "Edit") 				
				return FALSE;
			//cout << LOWORD(lParam) << '\n';
			switch (LOWORD(lParam)) {
				case HTSYSMENU:
				case HTREDUCE:
				case HTZOOM:
				case HTCLOSE:
				case HTMENU:
				case HTHSCROLL:
				case HTVSCROLL:
					SetCursor(press_cursor);
					return TRUE;
				case HTNOWHERE:
				case HTCLIENT:
				case HTCAPTION:
				case HTSIZE:
				case HTOBJECT:
				case HTHELP:
					SetCursor(idle_cursor);
					return TRUE;
				case CURSOR_LOADING:
					SetCursor(loading_cursor);
					return TRUE;
				default:
					return FALSE;
			}
			break;
		}
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
						if (CTB_VOLUME.current_val > 0)
							PlaySound(MAKEINTRESOURCE(CAIN), NULL, SND_ASYNC | SND_RESOURCE);
						MessageBox(NULL, TEXT("Let Token Know"), TEXT("NOOOOOOOB"), MB_OK);
					}
					if (clicked == 3002) {
						ShellExecute(hwnd, TEXT("open"), TEXT("config.txt"), NULL, NULL, SW_SHOW);
					}
					if (clicked == 3003) {
						ShowWindow(BTN_MUSHROOM.handle, SW_SHOW);
						ShowWindow(BTN_COW.handle, SW_SHOW);
						PlaySound(MAKEINTRESOURCE(FRESHMEAT), NULL, SND_ASYNC | SND_RESOURCE);
					}
					break;
			}
			break;
		}
		/*case WM_NCACTIVATE:{
			if (wParam)
				mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);
			else
				UnhookWindowsHookEx(mhook);
			break;
		}*/
		/*case WM_NCHITTEST:{
			UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
			if (uHitTest == HTCLIENT)
				return HTCAPTION;
			else
				return uHitTest;
		}*/
		case WM_CLOSE:
		case WM_DESTROY:{
			PostQuitMessage(0);
		}
		case WM_TEST:{
			cout << "test 1 -> screenshot\n";
			windowcapture(GetForegroundWindow(), "screenshot.bmp");


			//if (TBTN_RAINBOW.toggle_state) {
			//	double t1 = (double)clock();
			//	int speed = 13;
			//	for (int i = 0; i < functionnamewidth; i += (functionnamewidth / speed)) {
			//		for (auto & a : tokenhackfunctions) {
			//			if (IsWindow(a->static_control.handle)) {
			//				RECT rc = getclientrect(a->static_control.handle);
			//				rc.right = rc.left + i + (functionnamewidth / speed);
			//				rc.left += i;

			//				static int count = 0;
			//				count++;
			//				BYTE red1 = 255, gre1 = 0, blu1 = 105;
			//				if (TBTN_RAINBOW.toggle_state) {
			//					static int rainbowc = 1;
			//					static int rainbowc1 = 12;
			//					if (rainbowc > 12)
			//						rainbowc = 1;
			//					if (rainbowc == 1) { red1 = 255; gre1 = 0; blu1 = 0; /*cout << "\tred1"; */ }
			//					if (rainbowc == 2) { red1 = 255; gre1 = 150; blu1 = 0; /*cout << "\torange"; */ }
			//					if (rainbowc == 3) { red1 = 255; gre1 = 255; blu1 = 0; /*cout << "\tyellow"; */ }
			//					if (rainbowc == 4) { red1 = 105; gre1 = 255; blu1 = 0; /*cout << "\tlight gre1en"; */ }
			//					if (rainbowc == 5) { red1 = 0; gre1 = 255; blu1 = 0; /*cout << "\tgre1en"; */ }
			//					if (rainbowc == 6) { red1 = 0; gre1 = 255; blu1 = 150; /*cout << "\tturquoise"; */ }
			//					if (rainbowc == 7) { red1 = 0; gre1 = 255; blu1 = 255; /*cout << "\tlight blu1e"; */ }
			//					if (rainbowc == 8) { red1 = 0; gre1 = 105; blu1 = 255; /*cout << "\tblu1e"; */ }
			//					if (rainbowc == 9) { red1 = 0; gre1 = 0; blu1 = 255; /*cout << "\tdark blu1e"; */ }
			//					if (rainbowc == 10) { red1 = 150; gre1 = 0; blu1 = 255; /*cout << "\tpurple"; */ }
			//					if (rainbowc == 11) { red1 = 255; gre1 = 0; blu1 = 255; /*cout << "\tmagenta"; */ }
			//					if (rainbowc == 12) { red1 = 255; gre1 = 0; blu1 = 105; /*cout << "\tpink"; */ }
			//					rainbowc++;
			//					if (count % totalonfunctions == 0) {
			//						if ((HWND)lParam == tokenhackfunctions[tokenhackfunctions.size() - 1]->static_control.handle)
			//							count = 0;
			//						count = 0;
			//						is_drawing_static = false;
			//						rainbowc = rainbowc1;
			//						rainbowc1--;
			//						if (rainbowc1 == 0)
			//							rainbowc1 = 12;
			//					}
			//				}
			//				a->static_control.color_scheme.text_idle_on = COLORREF(RGB(red1, gre1, blu1));
			//				RedrawWindow(a->static_control.handle, &rc, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			//			}
			//			else
			//				cout << "window not found\n";
			//		}
			//	}
			//	cout << (double)clock() - t1 << '\n';
			//}
			break;
		}
		case WM_TEST2:{
			cout << "test 2 -> transparent toggle\n";
			HWND wnd = WindowFromPoint(getcursorpos());
			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
			SetLayeredWindowAttributes(wnd, NULL, 200, LWA_ALPHA);
			break;
		}
		case WM_TEST3:{
			cout << "test 3 -> layered window screenshot test\n";
			HWND wnd = WindowFromPoint(getcursorpos());

			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
			SetLayeredWindowAttributes(wnd, NULL, 255, LWA_ALPHA);
			this_thread::sleep_for(chrono::milliseconds(500));
			windowcapture(wnd, "layered_toggle.bmp");

			break;
		}
		case WM_TEST4:{
			HWND wnd = GetForegroundWindow();

			if (!InjectDll(DLL_NAME, wnd)) {
				MessageBox(NULL, L"Dll injection failed", L"Error", MB_OK);
				PostQuitMessage(0);
			}

			break;
		}
		case WM_RAINBOW:{
			SIZE size;
			HDC hDC = GetDC(rainbowtest);

			SetWindowText(rainbowtest, TEXT("kill all weeaboos"));

			string text = getwindowtext(rainbowtest);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.size(), &size);
			SetWindowPos(rainbowtest, NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
			ReleaseDC(rainbowtest, hDC);

			UINT reps = 6;
			UINT speed = 8;
			for (UINT j = 0; j < reps; j++) {				
				for (int i = 0; i < size.cx; i += speed) {
					RECT rc = getclientrect(rainbowtest);
					rc.right = rc.left + i + speed;
					rc.left += i;
					static int count = 0;
					count++;
					BYTE red1 = 255, gre1 = 0, blu1 = 105;

					static int rainbowc = 1;
					static int rainbowc1 = 12;
					if (rainbowc > 12)
						rainbowc = 1;
					if (rainbowc == 1) { red1 = 255; gre1 = 0; blu1 = 0; /*cout << "\tred1"; */ }
					if (rainbowc == 2) { red1 = 255; gre1 = 150; blu1 = 0; /*cout << "\torange"; */ }
					if (rainbowc == 3) { red1 = 255; gre1 = 255; blu1 = 0; /*cout << "\tyellow"; */ }
					if (rainbowc == 4) { red1 = 105; gre1 = 255; blu1 = 0; /*cout << "\tlight gre1en"; */ }
					if (rainbowc == 5) { red1 = 0; gre1 = 255; blu1 = 0; /*cout << "\tgre1en"; */ }
					if (rainbowc == 6) { red1 = 0; gre1 = 255; blu1 = 150; /*cout << "\tturquoise"; */ }
					if (rainbowc == 7) { red1 = 0; gre1 = 255; blu1 = 255; /*cout << "\tlight blu1e"; */ }
					if (rainbowc == 8) { red1 = 0; gre1 = 105; blu1 = 255; /*cout << "\tblu1e"; */ }
					if (rainbowc == 9) { red1 = 0; gre1 = 0; blu1 = 255; /*cout << "\tdark blu1e"; */ }
					if (rainbowc == 10) { red1 = 150; gre1 = 0; blu1 = 255; /*cout << "\tpurple"; */ }
					if (rainbowc == 11) { red1 = 255; gre1 = 0; blu1 = 255; /*cout << "\tmagenta"; */ }
					if (rainbowc == 12) { red1 = 255; gre1 = 0; blu1 = 105; /*cout << "\tpink"; */ }
					rainbowc++;

					RedrawWindow(rainbowtest, &rc, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				windowcapture(rainbowtest, "C:\\Users\\Josh_2.Josh-PC\\Desktop\\RAINBOW\\rainbow" + int_to_str(j) + ".bmp");
			}
			bmp_to_gif("C:\\Users\\Josh_2.Josh-PC\\Desktop\\RAINBOW\\");
			animate_gif("C:\\Users\\Josh_2.Josh-PC\\Desktop\\RAINBOW\\", "output\\candy.gif");

			vector<string> files;
			GetFilesInDirectory(files, "C:\\Users\\Josh_2.Josh-PC\\Desktop\\RAINBOW\\", "gif");
			GetFilesInDirectory(files, "C:\\Users\\Josh_2.Josh-PC\\Desktop\\RAINBOW\\", "bmp");

			for (UINT i = 0; i < files.size(); i++) {
				cout << files[i] << '\n';
				if (!DeleteFile(str_to_wstr(files[i]).c_str())) {
					cout << "unable to delete file" << error_code_to_text(GetLastError()) << '\n';
				}
			}

			//if (TBTN_RAINBOW.toggle_state) {
			//	double t1 = (double)clock();
			//	int speed = 13;
			//	for (int i = 0; i < functionnamewidth; i += (functionnamewidth / speed)) {
			//		for (auto & a : tokenhackfunctions) {
			//			if (IsWindow(a->static_control.handle)) {
			//				RECT rc = getclientrect(a->static_control.handle);
			//				rc.right = rc.left + i + (functionnamewidth / speed);
			//				rc.left += i;
			//				static int count = 0;
			//				count++;
			//				BYTE red1 = 255, gre1 = 0, blu1 = 105;
			//				
			//				static int rainbowc = 1;
			//				static int rainbowc1 = 12;
			//				if (rainbowc > 12)
			//					rainbowc = 1;
			//				if (rainbowc == 1) { red1 = 255; gre1 = 0; blu1 = 0; /*cout << "\tred1"; */ }
			//				if (rainbowc == 2) { red1 = 255; gre1 = 150; blu1 = 0; /*cout << "\torange"; */ }
			//				if (rainbowc == 3) { red1 = 255; gre1 = 255; blu1 = 0; /*cout << "\tyellow"; */ }
			//				if (rainbowc == 4) { red1 = 105; gre1 = 255; blu1 = 0; /*cout << "\tlight gre1en"; */ }
			//				if (rainbowc == 5) { red1 = 0; gre1 = 255; blu1 = 0; /*cout << "\tgre1en"; */ }
			//				if (rainbowc == 6) { red1 = 0; gre1 = 255; blu1 = 150; /*cout << "\tturquoise"; */ }
			//				if (rainbowc == 7) { red1 = 0; gre1 = 255; blu1 = 255; /*cout << "\tlight blu1e"; */ }
			//				if (rainbowc == 8) { red1 = 0; gre1 = 105; blu1 = 255; /*cout << "\tblu1e"; */ }
			//				if (rainbowc == 9) { red1 = 0; gre1 = 0; blu1 = 255; /*cout << "\tdark blu1e"; */ }
			//				if (rainbowc == 10) { red1 = 150; gre1 = 0; blu1 = 255; /*cout << "\tpurple"; */ }
			//				if (rainbowc == 11) { red1 = 255; gre1 = 0; blu1 = 255; /*cout << "\tmagenta"; */ }
			//				if (rainbowc == 12) { red1 = 255; gre1 = 0; blu1 = 105; /*cout << "\tpink"; */ }
			//				rainbowc++;
			//				if (count % totalonfunctions == 0) {
			//					if ((HWND)lParam == tokenhackfunctions[tokenhackfunctions.size() - 1]->static_control.handle)
			//						count = 0;
			//					count = 0;
			//					is_drawing_static = false;
			//					rainbowc = rainbowc1;
			//					rainbowc1--;
			//					if (rainbowc1 == 0)
			//						rainbowc1 = 12;
			//				}
			//				
			//				a->static_control.color_scheme.text_idle_on = COLORREF(RGB(red1, gre1, blu1));
			//				a->static_control.color_scheme.text_hover_on = COLORREF(RGB(red1, gre1, blu1));
			//				RedrawWindow(a->static_control.handle, &rc, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			//			}
			//			else
			//				cout << "window not found\n";
			//		}
			//	}
			//	cout << (double)clock() - t1 << '\n';
			//}

			cout << "candy\n";
			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK hotkeydlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			is_hotkey_dialog_visible = true;
			ClipCursor(NULL);
			update_hotkeykeys();
			SetWindowLongPtr(GetDlgItem(hwnd, HOTKEYBOX), DWLP_DLGPROC, (LONG_PTR)hotkeydlgProc);
			return TRUE;
		}
		case WM_SETCURSOR:{
			if (GetDlgCtrlID((HWND)wParam) == BTN_OK)
				break;
			else if (LOWORD(lParam) == HTCLOSE || LOWORD(lParam) == HTSYSMENU) {
				SetCursor(press_cursor);
				return TRUE;
			}
			else {
				SetCursor(idle_cursor);
				return TRUE;
			}
			break;
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
					hotkeys;
					for (auto & a: hotkeys) {
						if (global_change_hotkey == a) {
							newkey = false;
							cout << "Key already being used as hotkey" << '\n';
							if (CTB_VOLUME.current_val > 0)
								PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
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
			is_hotkey_dialog_visible = false;
			EndDialog(hwnd, BTN_OK);
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK configdlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_customtrackbar_VALUE_CHANGED) {
		CustomTrackbar ctb = *((CustomTrackbar*)lParam);
		if (ctb.handle == CTB_VOLUME.handle) {
			set_volume((WORD)ctb.current_val);

			static bool change = false;
			if (CTB_VOLUME.current_val == 0) {
				change = true;
				CTB_VOLUME.tcolor_scheme.border = RGB(255, 0, 0);
				CTB_VOLUME.tcolor_scheme.window_name_idle = RGB(255, 0, 0);
				CTB_VOLUME.tcolor_scheme.window_name_highlight = RGB(200, 0, 0);

				DestroyMenu(g_menu);
				g_menu = CreatePopupMenu();
				AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
				AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
				AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			}
			else if (change) {
				change = false;
				CTB_VOLUME.tcolor_scheme.border = RGB(0, 255, 0);
				CTB_VOLUME.tcolor_scheme.window_name_idle = RGB(80, 80, 80);
				CTB_VOLUME.tcolor_scheme.window_name_highlight = RGB(127, 127, 127);

				DestroyMenu(g_menu);
				g_menu = CreatePopupMenu();
				for (int freshmeatx = 0; freshmeatx < 31; freshmeatx++)
					AppendMenu(g_menu, MF_STRING, 3003, TEXT("FRESHMEAT"));
				AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
				AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
				AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));
			}
			changeconfigonstate("volume", CTB_VOLUME.current_val, true);
		}
	}
	switch (message) {
		case WM_INITDIALOG:{
			is_config_dialog_visible = true;

			int tokenhack_config_offset = 8;
			for (UINT i = 0; i < tokenhackfunctions.size() - 15; i++) {
				tokenhackfunction* a = tokenhackfunctions[i];
				if (a->config_edit_control.owner_name != own_config_dialog_tab_functions)
					continue;
				tokenhack_config_offset += 24;

				a->config_toggle_button.ypos = tokenhack_config_offset;
				a->config_edit_control.ypos = tokenhack_config_offset;
				a->config_static_control.ypos = tokenhack_config_offset;
				a->config_custom_combobox.ypos = tokenhack_config_offset;

				a->config_toggle_button.Create(hwnd);
				a->config_edit_control.Create(hwnd);
				a->config_static_control.Create(hwnd);
				a->config_custom_combobox.Create(hwnd);
			}		

			auto cond = [](BasicControl* a) -> bool {return (a->owner_name > own_BEGIN_config_dialog_box && a->owner_name < own_END_config_dialog_box && a->group_name != grp_tokenhack_function); };

			custom_controls.createChildren(hwnd, cond);
			
			set_volume((WORD)volume);
			CTB_VOLUME.redrawNewVal(volume);
			if (CTB_VOLUME.current_val == 0) {
				CTB_VOLUME.tcolor_scheme.border = RGB(255, 0, 0);
				CTB_VOLUME.tcolor_scheme.window_name_idle = RGB(255, 0, 0);
				CTB_VOLUME.tcolor_scheme.window_name_highlight = RGB(200, 0, 0);
			}
			else {
				CTB_VOLUME.tcolor_scheme.border = RGB(0, 255, 0);
				CTB_VOLUME.tcolor_scheme.window_name_idle = RGB(80, 80, 80);
				CTB_VOLUME.tcolor_scheme.window_name_highlight = RGB(127, 127, 127);
			}

			SendMessage(TAB_BTN_CONFIG_TAB_BTN_FUNCTIONS.handle, WM_LBUTTONUP, NULL, NULL);

			SetWindowLongPtr(GetDlgItem(hwnd, CONFIGBOX), DWLP_DLGPROC, (LONG_PTR)configdlgProc);
			break;
		}
		case WM_COMMAND:{
			if (lParam != 0)
				SendMessage((HWND)lParam, MY_COMMAND, wParam, lParam);
			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			HBRUSH hb_background = CreateSolidBrush(RGB(10, 10, 10));

			//paint background
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			//paint tab rect
			SetDCPenColor(hDC, RGB(100, 100, 100));
			Rectangle(hDC, 5, 18, 535, 314);
			Rectangle(hDC, 6, 19, 536, 315);

			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTSYSMENU:
				case HTREDUCE:
				case HTZOOM:
				case HTCLOSE:
				case HTMENU:
				case HTHSCROLL:
				case HTVSCROLL:
					SetCursor(press_cursor);
					return TRUE;
				case HTNOWHERE:
				case HTCLIENT:
				case HTCAPTION:
				case HTSIZE:
				case HTOBJECT:
				case HTHELP:
					SetCursor(idle_cursor);
					return TRUE;
				case CURSOR_LOADING:
					SetCursor(loading_cursor);
					return TRUE;
				default:
					return FALSE;
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			is_config_dialog_visible = false;
			EnumChildWindows(hwnd, EnumChildProc, DESTROY_WINDOWS);
			EnableWindow(g_hwnd, TRUE);
			EndDialog(hwnd, 1);
		}
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	if (lParam == DESTROY_WINDOWS) {
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else if (lParam == ECW_WM_KBHOOKKEYDOWN) {
		PostMessage(hwnd, WM_KBHOOKKEYDOWN, NULL, NULL);
	}
	return TRUE;
}
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	//MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);
	
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
			"\tcursor: " << crclient.x << ", " << crclient.y <<	'\n';
	}
	if (wParam == WM_MOUSEMOVE || wParam == WM_LBUTTONDOWN) {
		static int count = 0;	
		if (TBTN_RAINBOW.toggle_state > 1 && count++ % TBTN_RAINBOW.toggle_state == 0 && !is_drawing_static) {
			is_drawing_static = true;
			for (unsigned int i = 0; i < tokenhackfunctions.size(); i++) {
				if (IsWindow(tokenhackfunctions[i]->hotkey_button.handle)) {
					RedrawWindow(tokenhackfunctions[i]->static_control.handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
		}
		//cout << mouse.pt.x << "	" << mouse.pt.y << '\n';
	}
	if (wParam == WM_MOUSEWHEEL) {
		if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
			is_drawing_static = true;
			for (auto & a: tokenhackfunctions) {
				if (IsWindow(a->hotkey_button.handle)) {
					RedrawWindow(a->static_control.handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
		}
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

		EnumChildWindows(g_hwnd, EnumChildProc, ECW_WM_KBHOOKKEYDOWN);
		EnumChildWindows(config, EnumChildProc, ECW_WM_KBHOOKKEYDOWN);

		global_hook_key = key.vkCode;
		if (TBTN_HOOKOUTPUT.toggle_state)
			cout << key.vkCode << " " << sname << '\n';
		#pragma endregion	
		#pragma region functions
		static bool trap = false;
		static bool pause = false;
		if (move_window && lastwindow != NULL &&
			((key.vkCode >= 1061 && key.vkCode <= 1064) || //shift + arrows
			(key.vkCode >= 549 && key.vkCode <= 552) || //ctrl + arrows
			(key.vkCode >= 37 && key.vkCode <= 40) || // arrows
			(key.vkCode >= 1573 && key.vkCode <= 1576))) //ctrl + shift + arrows
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
		if (is_hotkey_dialog_visible || is_config_dialog_visible || (key.vkCode != PAUSE.hotkey_key && pause)) {
			
		}
		else if (key.vkCode == WMC.hotkey_key			&&WMC.on) {
			if (!trap) {
				trap = true;
				HWND wnd = GetForegroundWindow();
				if (getwindowtext(wnd) == "Program Manager") {
					RECT rcwindow = getwindowrect(GetDesktopWindow());
					ClipCursor(&rcwindow);
				}
				else {
					RECT rcmappedclient = getmappedclientrect(wnd);
					ClipCursor(&rcmappedclient);
				}
			}
			else {
				trap = false;
				ClipCursor(NULL);
			}
		}
		else if (key.vkCode == CLOSE.hotkey_key			&&CLOSE.on) {
			PostQuitMessage(0);
		}
		else if (key.vkCode == NOPICKUP.hotkey_key		&&NOPICKUP.on) {
			d2type("/nopickup");
		}
		else if (key.vkCode == PAUSE.hotkey_key			&&PAUSE.on) {
			if (!pause) {
				memcpy(g_notifyIconData.szInfoTitle, TEXT("PAUSED"), 64);
				memcpy(g_notifyIconData.szInfo, TEXT(" "), 256);
				memcpy(g_notifyIconData.szTip, TEXT("PAUSED"), 128);
				g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENREDICO), IMAGE_ICON, 0, 0, LR_SHARED);
				Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
				pause = true;
				trap = false;
				ClipCursor(NULL);
				kill_all_OCR_auto_roll = true;
			}
			else {
				memcpy(g_notifyIconData.szInfoTitle, TEXT("RESUMED"), 64);
				memcpy(g_notifyIconData.szInfo, TEXT(" "), 256);
				memcpy(g_notifyIconData.szTip, VERSION, 128);
				g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENICO), IMAGE_ICON, 0, 0, LR_SHARED);
				Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
				pause = false;
				BTN_RELOAD_PROC(0, 0);
			}
		}
		else if (key.vkCode == ROLL.hotkey_key			&&ROLL.on) {
			HWND wnd = GetForegroundWindow();
			POINT transmute_btn;
			if (!get_transmute_pos(wnd, &transmute_btn)) {
				cout << "Invalid window resolution: cound not find transmute button" << '\n';
				cout << "Compatible resolutions: (640, 480) | (800, 600) | (1024, 768) | (1280, 1024)" << '\n';
				cout << "open confix.txt to manually set the transmute button pos" << '\n';
				return CallNextHookEx(kbhook, code, wParam, lParam);
			}
			SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));
			SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));

			if (ROLL.on == 2) {

				this_thread::sleep_for(chrono::milliseconds(200));

				int x, y;
				BYTE* newbuf = windowcapture(GetForegroundWindow(), "capture.bmp", &x, &y);
				BYTE* newbuf_ocr = new BYTE[x * 3 * y];
				for (int i = 0; i < y; i++) {
					for (int j = 0; j < 3 * x; j += 3) {
						int pos = i * x * 3 + j;
						newbuf_ocr[pos + 0] = newbuf[pos + 0];
						newbuf_ocr[pos + 1] = newbuf[pos + 1];
						newbuf_ocr[pos + 2] = newbuf[pos + 2];
					}
				}
				vector<string> item_stats = ocr(newbuf_ocr, x, y, font16, true);
				process(newbuf_ocr, x, y, "(ocr)(processed)capture.bmp", true);
				process(newbuf, x, y, "(processed)capture.bmp", true);
				delete[] newbuf;
				delete[] newbuf_ocr;

				if (CTB_VOLUME.current_val > 0 && item_stats.size() == 0)
					PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);

				load_stats_and_colors_from_file();
				vector<string> item_stats_cpy = item_stats;
				compare_item_stats_to_file_stats(item_stats_cpy, stats_in_file, false);
				for (UINT i = 0; i < item_stats_cpy.size(); i++) {
					if (item_stats_cpy[i][0] == '#') {
						item_stats[i].insert(0, "## ");
						item_stats[i] += " ##";
					}
				}

				string statbox_text = "";
				for (auto & i : item_stats) {
					statbox_text += '>' + i + "\r\n";
					cout << "stats: " << i << '\n';
				}

				if (item_stats.size() > 0) {
					SetWindowText(statbox, str_to_wstr(statbox_text).c_str());
					cout << "-----------------------------------------------" << '\n';
				}
			}
		}
		else if (key.vkCode == AUTOROLL.hotkey_key		&&AUTOROLL.on) {
			HWND wnd = GetForegroundWindow();

			load_stats_and_colors_from_file();

			if (stats_in_file.size() == 0) {
				SetWindowText(statbox, TEXT("No stats found in file"));
				cout << "No stats found in file" << '\n';
				if (CTB_VOLUME.current_val > 0)
					PlaySound(MAKEINTRESOURCE(NOTHING), NULL, SND_ASYNC | SND_RESOURCE);
				return CallNextHookEx(kbhook, code, wParam, lParam);
			}

			string searching = "Searching for:\r\n";
			for (auto & a : stats_in_file)
				searching += '>' + a + "\r\n";
			searching += '\0';

			SendMessage(statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());

			POINT item_pt = getclientcursorpos(wnd);

			if (layered_window_fix) {
				if (!(GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYERED))
					SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) | WS_EX_LAYERED);
				SetLayeredWindowAttributes(wnd, NULL, 255, LWA_ALPHA);
			}

			kill_all_OCR_auto_roll = false;

			thread roll(OCR_auto_roll, wnd, font16, item_pt);
			roll.detach();
		}
		else if (key.vkCode == READ.hotkey_key			&&READ.on) {
			int x, y;
			BYTE* newbuf = windowcapture(GetForegroundWindow(), "capture.bmp", &x, &y);
			BYTE* newbuf_ocr = new BYTE[x * 3 * y];
			for (int i = 0; i < y; i++) {
				for (int j = 0; j < 3 * x; j += 3) {
					int pos = i * x * 3 + j;
					newbuf_ocr[pos + 0] = newbuf[pos + 0];
					newbuf_ocr[pos + 1] = newbuf[pos + 1];
					newbuf_ocr[pos + 2] = newbuf[pos + 2];
				}
			}
			vector<string> item_stats = ocr(newbuf_ocr, x, y, font16, true);
			process(newbuf_ocr, x, y, "(ocr)(processed)capture.bmp", true);
			process(newbuf, x, y, "(processed)capture.bmp", true);
			delete[] newbuf;
			delete[] newbuf_ocr;

			if (CTB_VOLUME.current_val > 0 && item_stats.size() == 0)
				PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);

			if (READ.on == 2) {
				load_stats_and_colors_from_file();
				vector<string> item_stats_cpy = item_stats;
				compare_item_stats_to_file_stats(item_stats_cpy, stats_in_file, false);
				for (auto & a : item_stats_cpy) {
					if (a[0] == '#') {
						a.insert(0, "## ");
						a += " ##";
					}
				}
			}

			string statbox_text = "";
			for (auto & a : item_stats) {
				statbox_text += '>' + a + "\r\n";
				cout << "stats: " << a << '\n';
			}

			if (item_stats.size() > 0) {
				SetWindowText(statbox, str_to_wstr(statbox_text).c_str());
				cout << "-----------------------------------------------" << '\n';
			}
			else {
				if (CTB_VOLUME.current_val > 0)
					PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
				//	SetWindowText(statbox, L"no stats found");
				std::cout << "no stats found" << '\n';
			}
		}
		else if (key.vkCode == MOVEITEM.hotkey_key		&&MOVEITEM.on) {
			/*int startw = 432;
			int starth = 328;
			for (int h = 0; h < 4; h++) {
			for (int i = 0; i < 10; i++) {
			cout << "{" << startw << ", " << starth << "}, ";
			POINT pt;
			pt.x = startw;
			pt.y = starth;
			ClientToScreen(GetForegroundWindow(), &pt);
			SetCursorPos(pt.x, pt.y);
			this_thread::sleep_for(chrono::milliseconds(500));
			startw += 29;
			}
			startw = 432;
			starth += 29;
			}
			_getch();*/
			static UINT count = 0;

			vector<int> inv_pos = {
				/*432, 328, 461, 328,*/ 490, 328, 519, 328, 548, 328, 577, 328, 606, 328, 635, 328, 664, 328, 693, 328, 
				/*432, 357, 461, 357,*/ 490, 357, 519, 357, 548, 357, 577, 357, 606, 357, 635, 357, 664, 357, 693, 357,
				432, 386, 461, 386, 490, 386, 519, 386, 548, 386, 577, 386, 606, 386, 635, 386, 664, 386, 693, 386,
				432, 415, 461, 415, 490, 415, 519, 415, 548, 415, 577, 415, 606, 415, 635, 415, 664, 415, 693, 415	
			};

			if (count >= inv_pos.size())
				count = 0;
			HWND wnd = GetForegroundWindow();

			for (int i = 0; count < inv_pos.size() && i < 3; i++, count+=2) {
				int xpos = inv_pos[count];
				int ypos = inv_pos[count + 1];
				this_thread::sleep_for(chrono::milliseconds(500));
				SendMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(xpos, ypos));
				SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(xpos, ypos));
				SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(xpos, ypos));
				this_thread::sleep_for(chrono::milliseconds(500));
				SendMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(450, 350));
				SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(450, 350));
				SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(450, 350));
			}
			POINT transmute_btn;
			if (!get_transmute_pos(wnd, &transmute_btn)) {
				cout << "Invalid window resolution: cound not find transmute button" << '\n';
				cout << "Compatible resolutions: (640, 480) | (800, 600) | (1024, 768) | (1280, 1024)" << '\n';
				cout << "open confix.txt to manually set the transmute button pos" << '\n';
				return CallNextHookEx(kbhook, code, wParam, lParam);
			}
			this_thread::sleep_for(chrono::milliseconds(500));
			SendMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));
			SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));
			SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));
			SendMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(100, 100));
		}
		else if (key.vkCode == VK_CANCEL				/*&&AUTOROLL.on*/) {
			kill_all_OCR_auto_roll = true;
			lastwindow = NULL;
		}
		else if (key.vkCode == TEST.hotkey_key			&&TEST.on) {
			SendMessage(g_hwnd, WM_TEST, 0, 0);
		}
		else if (key.vkCode == TEST2.hotkey_key			&&TEST2.on) {
			SendMessage(g_hwnd, WM_TEST2, 0, 0);
		}
		else if (key.vkCode == TEST3.hotkey_key			&&TEST3.on) {
			SendMessage(g_hwnd, WM_TEST3, 0, 0);
		}
		else if (key.vkCode == RAINBOW.hotkey_key		&&RAINBOW.on) {
			SendMessage(g_hwnd, WM_RAINBOW, 0, 0);
		}
		else if (key.vkCode == TEST4.hotkey_key			&&TEST4.on) {
			SendMessage(g_hwnd, WM_TEST4, 0, 0);
		}
#pragma endregion
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}

