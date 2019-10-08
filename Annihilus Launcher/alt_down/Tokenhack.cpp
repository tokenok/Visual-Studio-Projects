#include "include.h"

using namespace std;

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
	init_register_custom_trackbar();

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	g_hwnd = CreateWindow(className, VERSION, WS_OVERLAPPEDWINDOW /*WS_POPUP|*/  | WS_CLIPCHILDREN, 350, 150, windowwidth, functionnameheight*totalonfunctions + 130, NULL, NULL, hInstance, NULL);

	for (UINT i = 0; i < tabbuttons.size(); i++) {
		if (tabbuttons[i]->group_name == "function tab button") {
			if (0 == i) {
				tabbuttons[i]->toggle_state = 1;
				SendMessage(g_hwnd, WM_COMMAND, (WPARAM)tabbuttons[i]->id, NULL);
				break;
			}
		}
	}

	if (TBTN_SHOWONSTART.toggle_state)	ShowWindow(g_hwnd, SW_SHOW);

	InitNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(Font_a);

	UnhookWindowsHookEx(mhook);
	UnhookWindowsHookEx(kbhook);
	UnregisterClass(custom_trackbar_classname, NULL);
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
	PlaySound(MAKEINTRESOURCE(BYE), NULL, SND_SYNC | SND_RESOURCE);
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		Minimize();
		return 0;
	}
	if (message == WM_CUSTOM_TRACKBAR_VALUE_CHANGED) {
		custom_trackbar ctb = *((custom_trackbar*)lParam);
		if (ctb.handle == CTB_VOLUME.handle) {
			set_volume((WORD)ctb.current_val);

			static bool change = false;
			if (CTB_VOLUME.current_val == 0) {
				change = true;
				CTB_VOLUME.color_scheme.border = RGB(255, 0, 0);
				CTB_VOLUME.color_scheme.window_name_idle = RGB(255, 0, 0);
				CTB_VOLUME.color_scheme.window_name_highlight = RGB(200, 0, 0);

				DestroyMenu(g_menu);
				g_menu = CreatePopupMenu();
				AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
				AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
				AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			}
			else if (change) {
				change = false;
				CTB_VOLUME.color_scheme.border = RGB(0, 255, 0);
				CTB_VOLUME.color_scheme.window_name_idle = RGB(80, 80, 80);	
				CTB_VOLUME.color_scheme.window_name_highlight = RGB(127, 127, 127);

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
		case WM_CREATE:{
			UINT volume = 0;
			static bool firstrun = true;
			if (firstrun) {
				press_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESSCURSOR));
				idle_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(IDLECURSOR));
				loading_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(WAITCURSOR));
				token_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(TOKENCURSOR));

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
					else if (var_name == "volume") volume = (WORD)str_to_int(val);
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
								a->on = on;
								a->hotkey_key = key.size() == 0 ? 0 : (WORD)str_to_int(key);
								a->temp_hotkey = a->hotkey_key;
								a->static_handle = CreateWindow(TEXT("STATIC"), str_to_wstr(a->static_text).c_str(), 
									a->static_styles,
									functionnamexoff, functionnameyoffsum, 
									functionnamewidth, functionnameheight, 
									hwnd, 0, NULL, NULL);
								a->hotkeybutton_handle = CreateWindow(TEXT("BUTTON"), TEXT(""),
									a->hotkeybutton_styles,
									buttonxoff, functionnameyoffsum, 
									buttonwidth, buttonheight, 
									hwnd, (HMENU)a->hotkeybutton_id, NULL, NULL);
								if (on > 1)
									a->checkbox_text = int_to_str(on);
								else
									a->checkbox_text = "T";
								a->checkbox_handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a->checkbox_text).c_str(), 
									a->checkbox_styles, 
									checkboxxoff, functionnameyoffsum, 
									checkboxwidth, checkboxheight,
									hwnd, (HMENU)a->checkbox_id, NULL, NULL);
								changebuttontext(a->hotkeybutton_handle, a->hotkey_key);
								setbuttonstate(a->hotkeybutton_handle, a->on);
								totalonfunctions++;
								break;
							}
						}
					}
				}
				filein.close();
			}

			if (TBTN_CONSOLE.toggle_state) {
				cout.clear();
				CONSOLE_ON();
			}
			else FreeConsole();

			statbox_display(hwnd);
			
			for (auto & a: tabbuttons) {
				a->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a->window_text).c_str(),
					a->window_styles,
					a->xpos, a->ypos,
					a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: normalbuttons) {
				a->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a->window_text).c_str(), 
					a->window_styles, 
					a->xpos, (a->group_name == "tokenhackoffset") ? a->ypos + (totalonfunctions * functionnameheight) + 6 : a->ypos,
					a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: togglebuttons) {
				a->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a->window_text).c_str(),
					a->window_styles,
					a->xpos, (a->group_name == "tokenhackoffset") ? a->ypos + (totalonfunctions * functionnameheight) + 6 : a->ypos,
					a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: autoradiobuttons) {
				a->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a->window_text).c_str(),
					a->window_styles,
					a->xpos, a->ypos,
					a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: checkboxbuttons) {
				a->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a->window_text).c_str(),
					a->window_styles,
					a->xpos, a->ypos, 
					a->width, a->height,
					hwnd,	(HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: editcontrols) {
				a->handle = CreateWindow(TEXT("EDIT"), str_to_wstr(a->window_text).c_str(),
					a->window_styles,
					a->xpos, a->ypos,
					a->width, a->height, 
					hwnd, (HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: staticcontrols) {
				a->handle = CreateWindow(TEXT("STATIC"), str_to_wstr(a->window_text).c_str(),
					a->window_styles,
					a->xpos, a->ypos,
					a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
				if (a->font)
					SendMessage(a->handle, WM_SETFONT, (WPARAM)a->font, 0);
			}
			for (auto & a: custom_trackbars) {
				a->handle = CreateWindow(custom_trackbar_classname, str_to_wstr(a->window_name).c_str(),
					a->window_styles,
					a->xpos, (a->group_name == "tokenhackoffset") ? a->ypos + (totalonfunctions * functionnameheight) + 6 : a->ypos,
					a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
			}

			set_volume((WORD)volume);
			CTB_VOLUME.redraw_newval(volume);
			if (CTB_VOLUME.current_val == 0) {
				CTB_VOLUME.color_scheme.border = RGB(255, 0, 0);
				CTB_VOLUME.color_scheme.window_name_idle = RGB(255, 0, 0);
				CTB_VOLUME.color_scheme.window_name_highlight = RGB(200, 0, 0);
			}
			else {
				CTB_VOLUME.color_scheme.border = RGB(0, 255, 0);
				CTB_VOLUME.color_scheme.window_name_idle = RGB(80, 80, 80);
				CTB_VOLUME.color_scheme.window_name_highlight = RGB(127, 127, 127);
			}

			SetWindowPos(hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250/*180*/, functionnameheight * totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);

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
			case EN_CHANGE:{
				for (auto & a: editcontrols) {
					if ((HWND)lParam == a->handle) {
						if (HIWORD(wParam) == EN_CHANGE) {
							if (a->number_only) {
								string edit_text = getwindowtext((HWND)lParam);
								bool contains_char = false;
								for (UINT j = 0; j < edit_text.size(); j++) {
									if (edit_text[j] < 48 || edit_text[j] > 57 && edit_text[j] != 45) {
										edit_text.erase(j, 1);
										contains_char = true;
									}
								}
								if (contains_char) {
									SetWindowText((HWND)lParam, str_to_wstr(edit_text).c_str());
									SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)edit_text.size(), (LPARAM)edit_text.size());
								}
								int number = str_to_int(edit_text);
								if (number < a->min_number) {
									SetWindowText((HWND)lParam, str_to_wstr(int_to_str(a->min_number)).c_str());
									SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
								}
								else if (number > a->max_number) {
									SetWindowText((HWND)lParam, str_to_wstr(int_to_str(a->max_number)).c_str());
									SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
								}
							}
						}
					}
				}
			}
			DRAWITEMSTRUCT dis;
			RECT temprect;
			static bool tog = false;
			bool is_function = false;
			for (auto & a: tokenhackfunctions) {
				if (a->hotkeybutton_id == LOWORD(wParam) && a->on) {
					is_function = true;
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(HOTKEYBOX), hwnd, DlgProc);
					if (global_change_hotkey != 0) {
						if (global_change_hotkey == 1)
							global_change_hotkey = 0;
						hotkeychange(a->hotkeybutton_handle, a->hotkeybutton_id, a->hotkey_key, global_change_hotkey);
					}
					break;
				}
				if (a->checkbox_id == LOWORD(wParam)) {
					if (a->on) {//if on, turn off				
						a->on = str_to_int(changeconfigonstate(a->name_in_file));
						EnableWindow(a->hotkeybutton_handle, false);
					}
					else {//if off, turn on					
						a->on = str_to_int(changeconfigonstate(a->name_in_file));
						EnableWindow(a->hotkeybutton_handle, true);
					}					

					if (a->checkbox_id == READ.checkbox_id || a->checkbox_id == AUTOROLL.checkbox_id) {
						if (statbox) {
							if (!READ.on && !AUTOROLL.on) {
								DestroyWindow(statbox);
								DestroyWindow(STAT_TAB_BTN_OUTPUT.handle);
								statbox = NULL;
								SetWindowPos(hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250, functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);

							}
						}
						else {
							statbox_display(hwnd);
							SetWindowPos(hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250, functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);
						}
						if (a->checkbox_id == AUTOROLL.checkbox_id) {
							if (AUTOROLL.on)
								statbox_display(hwnd, true);
							else
								SetWindowText(statbox, L"");
						}
					}
				}
			}		
			for (auto & a: tabbuttons) {
				if (a->id == LOWORD(wParam)) {
					for (auto & b: tabbuttons) {
						if (a->group_name == b->group_name) {
							b->toggle_state = 0;
							HWND thandle = b->handle;
							GetClientRect(thandle, &temprect);
							dis.CtlID = GetDlgCtrlID(thandle);
							dis.CtlType = ODT_BUTTON;
							dis.hDC = GetDC(thandle);
							dis.itemState = ODS_INACTIVE;
							dis.rcItem = temprect;
							SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);
							ReleaseDC(thandle, dis.hDC);
						}
					}
					a->toggle_state = 1;
				/*	for (auto & b: all_groups) {
						b.show_group(false);
					}*/

					if (a->group_name == "function tab button") {
						EnumChildWindows(g_hwnd, EnumChildProc, DESTROY_WINDOWS);
						turn_all_functions_off();
						tokenhackfunctions.erase(tokenhackfunctions.begin(), tokenhackfunctions.end());

						if (a->id == TAB_BTN_ALL.id) {
							tokenhackfunctions = all_tokenhackfunctions;
						}

						SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(BTN_RELOAD.id, 0x01), NULL);
					}

					else if (a->group_name == "stat tab button") {
						/*if (a->id == STAT_TAB_BTN_INPUT.id) {
							
						}*/
						if (a->id == STAT_TAB_BTN_OUTPUT.id) {
							
						}
					}

					break;
				}
			}
			for (auto & a: autoradiobuttons) {
				if (a->id == LOWORD(wParam)) {
					int group = a->group;
					for (auto & b: autoradiobuttons) {
						if (group == 0)
							break;
						if (b->group == group) {
							b->toggle_state = 0;
							HWND thandle = b->handle;
							GetClientRect(thandle, &temprect);
							dis.CtlID = GetDlgCtrlID(thandle);
							dis.CtlType = ODT_BUTTON;
							dis.hDC = GetDC(thandle);
							dis.itemState = ODS_INACTIVE;
							dis.rcItem = temprect;
							SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);
							ReleaseDC(thandle, dis.hDC);
						}
					}
					a->toggle_state = 1;
				}
			}
			for (auto & a: checkboxbuttons) {
				if (a->id == LOWORD(wParam)) {
					if (a->toggle_state)
						a->toggle_state = 0;
					else
						a->toggle_state = 1;
					if (LOWORD(wParam) == CBTN_TOGGLEALL.id) {
						tog = true;
						if (toggleall) {
							toggleall = false;
							for (auto & b: tokenhackfunctions)
								if (b->on)
									SendMessage(hwnd, WM_COMMAND, (WPARAM)b->checkbox_id, NULL);
						}
						else {
							toggleall = true;
							for (auto & b: tokenhackfunctions)
								if (!b->on)
									SendMessage(hwnd, WM_COMMAND, (WPARAM)b->checkbox_id, NULL);
						}
						tog = false;
						break;
					}
				}
			}
			for (auto & a: normalbuttons) {
				if (a->id == LOWORD(wParam)) {
					if (BTN_RELOAD.id == a->id) {
						EnumChildWindows(hwnd, EnumChildProc, DESTROY_WINDOWS);
						turn_all_functions_off();
						for (auto & b: all_groups) {
							b.show_group(false);
						}
						stats_in_file.erase(stats_in_file.begin(), stats_in_file.end());
						reset_color_text_RGB();
						ClipCursor(NULL);
						RECT rcclient = getclientrect(hwnd);
						InvalidateRect(g_hwnd, &rcclient, true);
						SendMessage(hwnd, WM_CREATE, NULL, NULL);
						SendMessage(hwnd, WM_SIZE, NULL, NULL);
						if (HIWORD(wParam) == 0) {
							cout << "\n-------------------RELOAD-------------------" << '\n';
							if (CTB_VOLUME.current_val > 0)
								PlaySound(MAKEINTRESOURCE(RELOAD), NULL, SND_ASYNC | SND_RESOURCE);
						}
					}
					else if (BTN_CONFIG.id == a->id) {
						//image_display_box = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IMAGEDISPLAYBOX), hwnd, imageDlgProc);
						//DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(CONFIGBOX), hwnd, configProc);
						if (ShellExecute(hwnd, TEXT("open"), TEXT("config.txt"), NULL, NULL, SW_SHOW) == (HINSTANCE)ERROR_FILE_NOT_FOUND) {
							MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
							PostQuitMessage(0);
						}
					}
					else if (BTN_LAUNCH.id == a->id || BTN_LAUNCH_DIRECTORY_SET.id == a->id) {
						if (BTN_LAUNCH_DIRECTORY_SET.id == a->id)
							gamepath.erase(gamepath.begin(), gamepath.end());
						if (gamepath.size() == 0) {
							OPENFILENAME ofn;
							char File[MAX_PATH];
							ZeroMemory(&ofn, sizeof(ofn));
							ofn.lStructSize = sizeof(ofn);
							ofn.hwndOwner = g_hwnd;
							ofn.lpstrFile = (LPWSTR)File;
							ofn.lpstrFile[0] = '\0';
							ofn.nMaxFile = MAX_PATH;
							ofn.lpstrFilter = TEXT("exe\0*.exe*\0All\0*.*\0Shortcut\0*.lnk*\0Text\0*.TXT\0");
							ofn.nFilterIndex = 0;
							ofn.lpstrFileTitle = NULL;
							ofn.nMaxFileTitle = 0;
							ofn.lpstrInitialDir = NULL;
							ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS;

							GetOpenFileName(&ofn);

							gamepath = wstr_to_str(ofn.lpstrFile);

							vector<string> text;
							ifstream filein;
							filein.open("config.txt");
							if (!filein)
								break;
							string line;
							while (getline(filein, line))
								text.push_back(line);
							filein.close();

							for (auto & a: text) {
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
								break;
							for (auto & a: text)
								fileout << a << '\n';
							fileout.close();
						}
						gamedir = gamepath.substr(0, gamepath.rfind('\\'));
						string gamename = gamepath.substr(gamepath.rfind('\\') + 1, gamepath.size());

						if (gamepath.size() > 0 && BTN_LAUNCH.id == a->id)
							ShellExecute(hwnd, TEXT("open"), str_to_wstr(gamename).c_str(), str_to_wstr(targetlines).c_str(), str_to_wstr(gamedir).c_str(), SW_SHOW);
					}
					else if (BTN_COW.id == a->id) {
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
					else if (BTN_MUSHROOM.id == a->id) {
						static int mushroomc = 508;
						if (CTB_VOLUME.current_val > 0) {
							if (mushroomc == 517)
								mushroomc = 508;							
							PlaySound(MAKEINTRESOURCE(mushroomc), NULL, SND_ASYNC | SND_RESOURCE);							
							mushroomc++;
						}
					}
					break;
				}
			}
			for (auto & a: togglebuttons) {
				if (a->id == LOWORD(wParam)) {		
					if (a->id == TBTN_CONSOLE.id) {
						if (TBTN_CONSOLE.toggle_state) {
							FreeConsole();
						}
						else {
							cout.clear();
							CONSOLE_ON();
							SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						}
					}
					else if (a->id == TBTN_RAINBOW.id) {
						for (auto & a: tokenhackfunctions) {
							if (IsWindow(a->hotkeybutton_handle)) {
								RECT rcclient = getclientrect(a->static_handle);
								InvalidateRect(a->static_handle, &rcclient, TRUE);
							}
						}
						for (auto & a: tabbuttons) {
							//if (a->group_name == "function tab buttons")
							if (IsWindow(a->handle)) {
								RECT rcclient = getclientrect(a->handle);
								InvalidateRect(a->handle, &rcclient, TRUE);
							}
						}
					}

					//always flip toggle state
					a->toggle_state = str_to_int(changeconfigonstate(a->string_in_file));
					break;
				}
			}
			if (is_function) break;
			HWND thandle = GetDlgItem(hwnd, LOWORD(wParam));
			GetClientRect(thandle, &temprect);
			dis.CtlID = GetDlgCtrlID(thandle);
			dis.CtlType = ODT_BUTTON;
			dis.hDC = GetDC(thandle);
			dis.itemState = tog ? ODS_INACTIVE : ODS_SELECTED;
			dis.rcItem = temprect;
			SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);
			ReleaseDC(thandle, dis.hDC);

			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(g_hwnd, &ps);

			HBRUSH hb_background = CreateSolidBrush(RGB(10, 10, 10));

			//paint background
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
			for (auto & a: tabbuttons) {
				if (a->group_name == "stat tab buttons")
					minwidth += a->width;
			}
			MoveWindow(statbox, statboxxoff, tabbuttonheight - 1, ((curwidth < minwidth) ? minwidth : curwidth), ((curheight < minheight) ? minheight : curheight), true);
			if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
				is_drawing_static = true;
				for (auto & a: tokenhackfunctions) {
					RedrawWindow(a->static_handle, NULL, NULL, RDW_INVALIDATE);
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
					if ((HWND)lParam == tokenhackfunctions[tokenhackfunctions.size() - 1]->static_handle)
						count = 0;
					count = 0;
					is_drawing_static = false;
					rainbowc = rainbowc1;
					rainbowc1--;
					if (rainbowc1 == 0)
						rainbowc1 = 12;
				}
			}
			for (auto & a: tokenhackfunctions) {
				if ((HWND)lParam == a->static_handle) {
					SetTextColor(hdcStatic, TBTN_RAINBOW.toggle_state ? RGB(red1, gre1, blu1) : a->static_scheme.text_idle_on);
					SetBkColor(hdcStatic, a->static_scheme.background_idle_on);
					g_tempbrush = CreateSolidBrush(a->static_scheme.border_idle_on);
					return (INT_PTR)g_tempbrush;
				}
			}
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
		case WM_DRAWITEM:{		
			static int count = 2;
			static BYTE red1, gre1, blu1;				
			if (TBTN_RAINBOW.toggle_state > 1) {
				if (count > 12)
					count = 1;
				if (count == 1) { red1 = 255; gre1 = 0; blu1 = 0; /*cout << "\tred1"; */ }
				if (count == 2) { red1 = 255; gre1 = 150; blu1 = 0; /*cout << "\torange"; */ }
				if (count == 3) { red1 = 255; gre1 = 255; blu1 = 0; /*cout << "\tyellow"; */ }
				if (count == 4) { red1 = 105; gre1 = 255; blu1 = 0; /*cout << "\tlight gre1en"; */ }
				if (count == 5) { red1 = 0; gre1 = 255; blu1 = 0; /*cout << "\tgre1en"; */ }
				if (count == 6) { red1 = 0; gre1 = 255; blu1 = 150; /*cout << "\tturquoise"; */ }
				if (count == 7) { red1 = 0; gre1 = 255; blu1 = 255; /*cout << "\tlight blu1e"; */ }
				if (count == 8) { red1 = 0; gre1 = 105; blu1 = 255; /*cout << "\tblu1e"; */ }
				if (count == 9) { red1 = 0; gre1 = 0; blu1 = 255; /*cout << "\tdark blu1e"; */ }
				if (count == 10) { red1 = 150; gre1 = 0; blu1 = 255; /*cout << "\tpurple"; */ }
				if (count == 11) { red1 = 255; gre1 = 0; blu1 = 255; /*cout << "\tmagenta"; */ }
				if (count == 12) { red1 = 255; gre1 = 0; blu1 = 105; /*cout << "\tpink"; */ }
				count++;
			}
			else
				red1 = gre1 = blu1 = 100;
			
			LPDRAWITEMSTRUCT pdis = (DRAWITEMSTRUCT*)lParam;
			SIZE size;
		
			for (auto & a: tokenhackfunctions){
				//draw buttons
				if (pdis->CtlID == a->hotkeybutton_id) {
					basic_control_colorscheme cs = a->hotkeybutton_scheme;
					wchar_t ttext[256];
					GetWindowText(GetDlgItem(hwnd, a->hotkeybutton_id), ttext, 256);
					string text = wstr_to_str((wstring)ttext);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(pdis->hDC,
						a->on ?
							pdis->itemState & ODS_SELECTED ?
								cs.background_hover_on :
							cs.background_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.background_hover_off :
						cs.background_idle_off);
					SetTextColor(pdis->hDC,
						a->on ?
							pdis->itemState & ODS_SELECTED ?
								cs.text_hover_on :
							cs.text_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.text_hover_off :
						cs.text_idle_off);
					if (a->hotkeybutton_font)
						SelectObject(pdis->hDC, a->hotkeybutton_font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);

					//DrawEdge(pdis->hDC, &pdis->rcItem, (pdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT);
					HBRUSH framebrush = CreateSolidBrush(
						a->on ?
							pdis->itemState & ODS_SELECTED ?
								cs.border_hover_on :
							cs.border_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.border_hover_off :
						cs.border_idle_off);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
				//draw checkboxes
				if (pdis->CtlID == a->checkbox_id) {
					basic_control_colorscheme cs = a->checkbox_scheme;
					wchar_t ttext[2];					
					GetWindowText(GetDlgItem(hwnd, a->checkbox_id), ttext, 2);
					string text = wstr_to_str((wstring)ttext);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(pdis->hDC,
						a->on ?
							pdis->itemState & ODS_SELECTED ?
								cs.background_hover_on :
							cs.background_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.background_hover_off :
						cs.background_idle_off);
					SetTextColor(pdis->hDC,
						a->on ?
							pdis->itemState & ODS_SELECTED ?
								cs.text_hover_on :
							cs.text_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.text_hover_off :
						cs.text_idle_off);
					if (a->checkbox_font)
						SelectObject(pdis->hDC, a->checkbox_font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);

					//DrawEdge(pdis->hDC, &pdis->rcItem, (pdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT);
					HBRUSH framebrush = CreateSolidBrush(
						a->on ?
							pdis->itemState & ODS_SELECTED ?
								cs.border_hover_on :
							cs.border_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.border_hover_off :
						cs.border_idle_off);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
			}
			for (auto & a: tabbuttons) {
				if (pdis->CtlID == a->id) {		
					basic_control_colorscheme cs = a->color_scheme;
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(pdis->hDC, 
						a->toggle_state ?
							pdis->itemState & ODS_SELECTED ?
								cs.background_hover_on : 
							cs.background_idle_on :
						pdis->itemState & ODS_SELECTED ? 
							cs.background_hover_off : 
						cs.background_idle_off);
					SetTextColor(pdis->hDC, 
						a->toggle_state ?
							pdis->itemState & ODS_SELECTED ? 
								cs.text_hover_on : 
							cs.text_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.text_hover_off : 
						cs.text_idle_off);
					if (a->font)
						SelectObject(pdis->hDC, a->font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH framebrush = CreateSolidBrush(
						a->toggle_state ? 
							pdis->itemState & ODS_SELECTED ? 
								cs.border_hover_on : 
							cs.border_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.border_hover_off :
						cs.border_idle_off);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
			}
			for (auto & a: autoradiobuttons) {
				if (pdis->CtlID == a->id) {
					basic_control_colorscheme cs = a->color_scheme;
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);					
					SetBkColor(pdis->hDC, 
						a->toggle_state ? 
							pdis->itemState & ODS_SELECTED ?
								cs.background_hover_on : 
							cs.background_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.background_hover_off : 
						cs.background_idle_off);
					SetTextColor(pdis->hDC, 
						a->toggle_state ? 
							pdis->itemState & ODS_SELECTED ? 
								cs.text_hover_on : 
							cs.text_idle_on : 
						pdis->itemState & ODS_SELECTED ?
							cs.text_hover_off : 
						cs.text_idle_off);
					if (a->font)
						SelectObject(pdis->hDC, a->font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH framebrush = CreateSolidBrush(
						a->toggle_state ?
							pdis->itemState & ODS_SELECTED ? 
								cs.border_hover_on : 
							cs.border_idle_on : 
						pdis->itemState & ODS_SELECTED ?
							cs.border_hover_off :
						cs.border_idle_off);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
			}
			for (auto & a: checkboxbuttons) {
				if (pdis->CtlID == a->id) {
					basic_control_colorscheme cs = a->color_scheme;
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, a->id), ttext, 2);
					string text = wstr_to_str((wstring)ttext);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(pdis->hDC, 
						a->toggle_state ? 
							pdis->itemState & ODS_SELECTED ? 
								cs.background_hover_on :
							cs.background_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.background_hover_off :
						cs.background_idle_off);
					SetTextColor(pdis->hDC, 
						a->toggle_state ?
							pdis->itemState & ODS_SELECTED ?
								cs.text_hover_on :
							cs.text_idle_on :
						pdis->itemState & ODS_SELECTED ? 
							cs.text_hover_off : 
						cs.text_idle_off);
					if (a->font)
						SelectObject(pdis->hDC, a->font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH framebrush = CreateSolidBrush(
						a->toggle_state ? 
							pdis->itemState & ODS_SELECTED ? 
								cs.border_hover_on :
							cs.border_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.border_hover_off : 
						cs.border_idle_off);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
			}
			for (auto & a: normalbuttons) {
				if (pdis->CtlID == a->id) {
					basic_control_colorscheme cs = a->color_scheme;
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? cs.background_hover_on : cs.background_idle_on);
					SetTextColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? cs.text_hover_on : cs.text_idle_on);
					if (a->font)
						SelectObject(pdis->hDC, a->font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH framebrush = CreateSolidBrush(pdis->itemState & ODS_SELECTED ? cs.border_hover_on : cs.border_idle_on);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
			}
			for (auto & a: togglebuttons) {
				if (pdis->CtlID == a->id) {
					basic_control_colorscheme cs = a->color_scheme;
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(pdis->hDC, 
						a->toggle_state ?
							pdis->itemState & ODS_SELECTED ?
								cs.background_hover_on :
							cs.background_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.background_hover_off :
						cs.background_idle_off);
					SetTextColor(pdis->hDC, 
						a->toggle_state ? 
							pdis->itemState & ODS_SELECTED ?
								cs.text_hover_on : 
							cs.text_idle_on : 
						pdis->itemState & ODS_SELECTED ? 
							cs.text_hover_off : 
						cs.text_idle_off);
					if (a->font)
						SelectObject(pdis->hDC, a->font);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH framebrush = CreateSolidBrush(
						a->toggle_state ?
							pdis->itemState & ODS_SELECTED ? 
								cs.border_hover_on : 
							cs.border_idle_on :
						pdis->itemState & ODS_SELECTED ?
							cs.border_hover_off :
						cs.border_idle_off);
					FrameRect(pdis->hDC, &pdis->rcItem, framebrush);
					DeleteObject(framebrush);

					return TRUE;
				}
			}

			return TRUE;
		}
		case WM_SETCURSOR:{
			if (getwindowclassname((HWND)wParam) == "Button") {
				SetCursor(press_cursor);
				return TRUE;
			}
			if (getwindowclassname((HWND)wParam) == "Edit") 				
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
			}
			return DefWindowProc(hwnd, message, wParam, lParam);
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
		case WM_NCACTIVATE:{
			if (wParam)
				mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);
			else
				UnhookWindowsHookEx(mhook);
			break;
		}
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
			if (TBTN_RAINBOW.toggle_state) {
				double t1 = (double)clock();
				int speed = 13;
				for (int i = 0; i < functionnamewidth; i += (functionnamewidth / speed)) {
					for (auto & a : tokenhackfunctions) {
						if (IsWindow(a->static_handle)) {
							RECT rc = getclientrect(a->static_handle);
							rc.right = rc.left + i + (functionnamewidth / speed);
							rc.left += i;
							RedrawWindow(a->static_handle, &rc, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
						}
					}
				}
				cout << (double)clock() - t1 << '\n';
			}
			break;
		}
		case WM_TEST2:{
			HWND wnd = WindowFromPoint(getcursorpos());
			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLong(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
			SetLayeredWindowAttributes(wnd, NULL, 200, LWA_ALPHA);
			break;
		}
		case WM_TEST3:{
			HWND wnd = WindowFromPoint(getcursorpos());

			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLong(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
			SetLayeredWindowAttributes(wnd, NULL, 255, LWA_ALPHA);
			this_thread::sleep_for(chrono::milliseconds(500));
			windowcapture(wnd, "layered_toggle.bmp");

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
VOID CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	if (idEvent == timer1){		
		WINDOWPLACEMENT wp;
		GetWindowPlacement(g_lockedwindow, &wp);
		if (!(wp.rcNormalPosition.right == g_lockwindow.rcNormalPosition.right &&
			wp.rcNormalPosition.top == g_lockwindow.rcNormalPosition.top &&
			wp.rcNormalPosition.bottom == g_lockwindow.rcNormalPosition.bottom &&
			wp.rcNormalPosition.left == g_lockwindow.rcNormalPosition.left)) {
			SetWindowPos(g_lockedwindow, NULL, g_lockwindow.rcNormalPosition.left, g_lockwindow.rcNormalPosition.top, 0, 0, SWP_NOSIZE);
		}
	}
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			is_hotkey_dialog_visible = true;
			ClipCursor(NULL);
			update_hotkeykeys();
			SetWindowLongPtr(GetDlgItem(hwnd, HOTKEYBOX), DWLP_DLGPROC, (LONG_PTR)DlgProc);
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
		case WM_CLOSE:
			global_change_hotkey = 0;
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
BOOL CALLBACK configProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			cout << "CREATE\n";
			SetWindowLongPtr(GetDlgItem(hwnd, CONFIGBOX), DWLP_DLGPROC, (LONG_PTR)configProc);
			break;
		}
		case WM_PAINT:{
			cout << "paint\n";
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			HBRUSH hb_background = CreateSolidBrush(RGB(10, 10, 10));

			//paint background
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
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
	}
	return TRUE;
}
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	//MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);

	if (wParam == WM_RBUTTONDOWN && move_window) {
		POINT client_cursor_pos = getclientcursorpos(g_hwnd);

		//SetFocus(g_hwnd);//why?
		lastwindow = ChildWindowFromPointEx(g_hwnd, client_cursor_pos, CWP_SKIPDISABLED | CWP_SKIPINVISIBLE);
		if (lastwindow == g_hwnd) {
			lastwindow = NULL;
			return CallNextHookEx(mhook, code, wParam, lParam);
		}

		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(lastwindow, &wp);
		RECT move = wp.rcNormalPosition;

		cout << getwindowtext(lastwindow) << ": " << move.left << ", " << move.top << ", " << move.right - move.left << ", " << move.bottom - move.top << '\n';
	}
	if (wParam == WM_MOUSEMOVE || wParam == WM_LBUTTONDOWN) {
		static int count = 0;	
		if (TBTN_RAINBOW.toggle_state > 1 && count++ % TBTN_RAINBOW.toggle_state == 0 && !is_drawing_static) {
			is_drawing_static = true;
			for (unsigned int i = 0; i < tokenhackfunctions.size(); i++) {
				if (IsWindow(tokenhackfunctions[i]->hotkeybutton_handle)) {
					RedrawWindow(tokenhackfunctions[i]->static_handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
		}
		//cout << mouse.pt.x << "	" << mouse.pt.y << '\n';
	#pragma region highlight on hover
		static HWND previous_handle = NULL;
		HWND current_handle;

		DRAWITEMSTRUCT dis;
		POINT client_cursor_pos;
		RECT temprect;		
		BOOL tempon = true;

		client_cursor_pos = getclientcursorpos(g_hwnd);

		current_handle = ChildWindowFromPointEx(g_hwnd, client_cursor_pos, CWP_SKIPDISABLED | CWP_SKIPINVISIBLE);//get window handle under cursor

		if (wParam == WM_LBUTTONDOWN) {
			SetFocus(current_handle);
		}

		if (getwindowclassname(current_handle) == "Button") {		//only hightlight buttons 
			for (unsigned int i = 0; i < tokenhackfunctions.size(); i++) {
				if (current_handle == tokenhackfunctions[i]->hotkeybutton_handle) {
					tempon = tokenhackfunctions[i]->on;//dont hightlight buttons that are turned off
					break;
				}
			}

			if (current_handle != previous_handle /*&& GetForegroundWindow() == g_hwnd*/) {
				//highlight current button under cursor
				if (tempon) {
					GetClientRect(current_handle, &temprect);

					dis.CtlID = GetDlgCtrlID(current_handle);
					dis.CtlType = ODT_BUTTON;
					dis.hDC = GetDC(current_handle);
					dis.itemState = ODS_SELECTED;
					dis.rcItem = temprect;

					SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);

					ReleaseDC(current_handle, dis.hDC);
				}

				/*if (sound)
					PlaySound(MAKEINTRESOURCE(BUTTONHOVER), NULL, SND_ASYNC | SND_RESOURCE);*/
				if (previous_handle != NULL) {
					//dehighlight previous button
					GetClientRect(previous_handle, &temprect);

					dis.CtlID = GetDlgCtrlID(previous_handle);
					dis.CtlType = ODT_BUTTON;
					dis.hDC = GetDC(previous_handle);
					dis.itemState = ODS_INACTIVE;
					dis.rcItem = temprect;

					SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);

					ReleaseDC(previous_handle, dis.hDC);
				}
				previous_handle = current_handle;
			}
		}
		else if (getwindowclassname(previous_handle) == "Button") {//current is not a button, but previous was
			//dehighlight button when cursor taken off buttons
			dis.CtlID = GetDlgCtrlID(previous_handle);
			dis.hDC = GetDC(previous_handle);
			dis.itemState = NULL;
			GetClientRect(previous_handle, &temprect);
			dis.rcItem = temprect;

			SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);

			ReleaseDC(previous_handle, dis.hDC);

			previous_handle = NULL;
		}
#pragma endregion
	}
	if (wParam == WM_MOUSEWHEEL) {
		if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
			is_drawing_static = true;
			for (auto & a: tokenhackfunctions) {
				if (IsWindow(a->hotkeybutton_handle)) {
					RedrawWindow(a->static_handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
		}
	}
	
	return CallNextHookEx(mhook, code, wParam, lParam);
}
LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN ||	wParam == WM_SYSKEYDOWN){
	#pragma region virtual keycode processing
		wchar_t name[0x100];
		LPARAM lparam = 1;
		lparam += key.scanCode << 16;
		lparam += key.flags << 24;
		GetKeyNameText(lparam, (LPTSTR)name, 255);
		string sname = wstr_to_str((wstring)name);
		if ((GetKeyState(VK_MENU) & 0x8000) != 0) {
			key.vkCode += 256;
			sname.insert(0, "ALT+");
		}
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
			key.vkCode += 256 * 2;
			sname.insert(0, "CTRL+");
		}
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) {
			key.vkCode += 256 * 4;
			sname.insert(0, "SHIFT+");
		}		
		if ((GetKeyState(VK_LWIN) & 0x8000) != 0) {
			key.vkCode += 256 * 8;
			sname.insert(0, "LEFT WINDOWKEY+");
		}
		if ((GetKeyState(VK_RWIN) & 0x8000) != 0) {
			key.vkCode += 256 * 8;
			sname.insert(0, "RIGHT WINDOWKEY+");
		}

		if (!is_hotkey_dialog_visible && TBTN_HOOKOUTPUT.toggle_state) 
			cout << key.vkCode << " " << sname << '\n';
	#pragma endregion	
	#pragma region functions
		static bool pause = false;
		static bool type = false;
		static bool trap = false;
		static bool LOCK = false;
		if (is_hotkey_dialog_visible) {
			if (TBTN_HOOKOUTPUT.toggle_state)
				cout << "hotkey box: " << key.vkCode << " " << sname << '\n';
		}		
		else if (key.vkCode == WMC.hotkey_key			&&WMC.on		&&!pause	&&!type) {
			type = true;

			if (!trap) {
				trap = true;
				HWND wnd = GetForegroundWindow();
				RECT rcmappedclient = getmappedclientrect(wnd);
				ClipCursor(&rcmappedclient);
			}
			else {
				trap = false;
				ClipCursor(NULL);
			}

			type = false;
		}
		else if (key.vkCode == WINDOWLOCK.hotkey_key	&&WINDOWLOCK.on	&&!pause	&&!type) {
			type = true;

			if (!LOCK) {
				LOCK = true;
				g_lockedwindow = GetForegroundWindow();
				GetWindowPlacement(g_lockedwindow, &g_lockwindow);
				SetTimer(g_hwnd, timer1, 50, timerProc);
			}
			else {
				LOCK = false;
				KillTimer(g_hwnd, timer1);
			}

			type = false;
		}
		else if (key.vkCode == CLOSE.hotkey_key			&&CLOSE.on		/*&&!pause*/&&!type) {
			type = true;

			PostQuitMessage(0);

			type = false;
		}
		else if (key.vkCode == NOPICKUP.hotkey_key		&&NOPICKUP.on	&&!pause	&&!type) {
			type = true;

			d2type("/nopickup");

			type = false;
		}
		else if (key.vkCode == PAUSE.hotkey_key			&&PAUSE.on					&&!type) {
			type = true;

			if (!pause) {
				memcpy(g_notifyIconData.szInfoTitle, TEXT("PAUSED"), 64);
				memcpy(g_notifyIconData.szInfo, TEXT(" "), 256);
				memcpy(g_notifyIconData.szTip, TEXT("PAUSED"), 128);
				g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENREDICO), IMAGE_ICON, 0, 0, LR_SHARED);
				Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
				pause = true;
				trap = false;
				ClipCursor(NULL);
			}
			else {
				memcpy(g_notifyIconData.szInfoTitle, TEXT("RESUMED"), 64);
				memcpy(g_notifyIconData.szInfo, TEXT(" "), 256);
				memcpy(g_notifyIconData.szTip, VERSION, 128);
				g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENICO), IMAGE_ICON, 0, 0, LR_SHARED);
				Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
				pause = false;
				SendMessage(g_hwnd, WM_COMMAND, BTN_RELOAD.id, NULL);
			}

			type = false;
		}
		else if (key.vkCode == ROLL.hotkey_key			&&ROLL.on		&&!pause	&&!type) {
			type = true;

			HWND wnd = GetForegroundWindow();
			POINT transmute_btn;
			if (!get_transmute_pos(wnd, &transmute_btn)) {
				cout << "Invalid window resolution: cound not find transmute button" << '\n';
				cout << "Compatible resolutions: (640, 480) | (800, 600) | (1024, 768) | (1280, 1024)" << '\n';
				cout << "open confix.txt to manually set the transmute button pos" << '\n';
				type = false;
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

			type = false;
		}
		else if (key.vkCode == AUTOROLL.hotkey_key		&&AUTOROLL.on	&&!pause	&&!type) {
			type = true;

			HWND wnd = GetForegroundWindow();

			load_stats_and_colors_from_file();

			if (stats_in_file.size() == 0) {
				SetWindowText(statbox, TEXT("No stats found in file"));
				cout << "No stats found in file" << '\n';
				if (CTB_VOLUME.current_val > 0)
					PlaySound(MAKEINTRESOURCE(NOTHING), NULL, SND_ASYNC | SND_RESOURCE);
				type = false;
				return CallNextHookEx(kbhook, code, wParam, lParam);
			}

			string searching = "Searching for:\r\n";
			for (auto & a: stats_in_file)
				searching += '>' + a + "\r\n";
			searching += '\0';

			SendMessage(statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());

			POINT item_pt = getclientcursorpos(wnd);

			if (layered_window_fix) {
				if (!(GetWindowLong(wnd, GWL_EXSTYLE) & WS_EX_LAYERED))
					SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLong(wnd, GWL_EXSTYLE) | WS_EX_LAYERED);
				SetLayeredWindowAttributes(wnd, NULL, 255, LWA_ALPHA);
			}

			kill_all_OCR_auto_roll = false;

			thread roll(OCR_auto_roll, wnd, font16, item_pt);
			roll.detach();

			type = false;
		}
		else if (key.vkCode == READ.hotkey_key			&&READ.on		&&!pause	&&!type) {
			type = true;

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
				for (auto & a: item_stats_cpy){
					if (a[0] == '#') {
						a.insert(0, "## ");
						a += " ##";
					}
				}
			}

			string statbox_text = "";
			for (auto & a: item_stats) {
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

			type = false;
		}
		else if (key.vkCode == VK_CANCEL				/*&&AUTOROLL.on	&&!pause	&&!type*/) {
			kill_all_OCR_auto_roll = true;
		}
		else if (move_window && lastwindow != NULL && 
			((key.vkCode >= 1061 && key.vkCode <= 1064) ||
			(key.vkCode >= 549 && key.vkCode <= 552) || 
			(key.vkCode >= 37 && key.vkCode <= 40))) {
			WINDOWPLACEMENT wp;
			wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(lastwindow, &wp);
			RECT r;
			r = wp.rcNormalPosition;
			MoveWindow(lastwindow,
				key.vkCode == 1061 ? r.left - 5 : key.vkCode == 37 ? r.left - 1 : key.vkCode == 1063 ? r.left + 5 : key.vkCode == 39 ? r.left + 1 : r.left,
				key.vkCode == 1062 ? r.top - 5 : key.vkCode == 38 || key.vkCode == 550 ? r.top - 1 : key.vkCode == 1064 ? r.top + 5 : key.vkCode == 40 || key.vkCode == 552 ? r.top + 1 : r.top,
				key.vkCode == 549 ? r.right - r.left - 1 : key.vkCode == 551 ? r.right - r.left + 1 : r.right - r.left,
				key.vkCode == 550 ? r.bottom - r.top + 1 : key.vkCode == 552 ? r.bottom - r.top - 1 : r.bottom - r.top,
				true);
			GetWindowPlacement(lastwindow, &wp);
			r = wp.rcNormalPosition;
			RedrawWindow((HWND)lastwindow, NULL, NULL, RDW_INVALIDATE);
			cout << lastwindow << " " << getwindowtext(lastwindow) << ": " << r.left << ", " << r.top << ", " << r.right - r.left << ", " << r.bottom - r.top << '\n';	
		}
		else if (key.vkCode == TEST.hotkey_key			&&TEST.on		&&!pause	&&!type) {
			type = true;

			SendMessage(g_hwnd, WM_TEST, 0, 0);

			type = false;
		}
		else if (key.vkCode == TEST2.hotkey_key			&&TEST2.on		&&!pause	&&!type) {
			type = true;

			SendMessage(g_hwnd, WM_TEST2, 0, 0);

			type = false;
		}
		else if (key.vkCode == TEST3.hotkey_key			&&TEST3.on		&&!pause	&&!type) {
			type = true;

			SendMessage(g_hwnd, WM_TEST3, 0, 0);

			type = false;
		}
#pragma endregion
	}
	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		if (key.vkCode == VK_LMENU || key.vkCode == VK_RMENU) {
			static bool toggled = false;
			if (!toggled) {
				toggled = true;
				PostMessage(GetForegroundWindow(), WM_SYSKEYDOWN, (WPARAM)0x12, (LPARAM)0x20380001);
				return 1;
			}
			else 
				toggled = false;
		
			
		}
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}

