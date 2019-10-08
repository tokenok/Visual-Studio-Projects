#include "include.h"
#include "window.h"
#include "hotkey.h"
#include "image.h"
#include "keypress.h"
#include "resource.h"
#include "Custom Trackbar.h"
#include "Tokenhackdll.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow) {
	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, (LPCWSTR)"MyMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}
	filein.open("config.txt");
	if (!filein.is_open()) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		return 0;
	}
	filein.close();

	WORD left_vol = 0x4444;
	WORD right_vol = 0x4444;
	waveOutSetVolume(0, MAKELPARAM(right_vol, left_vol));

	srand((unsigned)time(NULL));

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_HOTKEY_CLASS;
	InitCommonControlsEx(&icex);

	init_register_class(hInstance);
	init_register_custom_trackbar();

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	g_hwnd = CreateWindow(className, WVERSION.c_str(), WS_OVERLAPPEDWINDOW /*WS_POPUP|*/  | WS_CLIPCHILDREN, 350, 150, windowwidth, functionnameheight*totalonfunctions + 130, NULL, NULL, hInstance, NULL);

	for (UINT i = 0; i < functiontabbuttons.size(); i++) {
		if (0 == i){
			functiontabbuttons[i]->toggle_state = 1;
			SendMessage(g_hwnd, WM_COMMAND, (WPARAM)functiontabbuttons[i]->id, NULL);
		}
	}	

	if (TBTN_SHOWONSTART.toggle_state)	ShowWindow(g_hwnd, SW_SHOW);

	InitNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	if (TBTN_SOUND.toggle_state)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(mhook);
	UnhookWindowsHookEx(kbhook);
	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

	ClipCursor(NULL);
	
	ShowWindow(g_hwnd, SW_HIDE);
	CloseHandle(Mutex_handle);

	DeleteObject(Font_a);

	cout << "EXITING" << '\n';
	if (TBTN_SOUND.toggle_state)
		PlaySound(MAKEINTRESOURCE(BYE), NULL, SND_SYNC | SND_RESOURCE);
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HINSTANCE hInst;
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		Minimize();
		return 0;
	}
	else if (message == WM_DLL_HOOKPROC) {
		cout << wParam << " " << lParam << '\n';
		HWND wnd = (HWND)wParam;
		RECT rccl = getwindowrect(wnd);
		MoveWindow(wnd, 0, 0, rccl.right - rccl.left, rccl.bottom - rccl.top, true);
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	switch (message) {
		case WM_CREATE:{
			static bool firstrun = true;
			if (firstrun) {
				press_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESS));
				idle_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(IDLE));
				loading_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(WAITCURSOR));
				token_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(TOKENCURSOR));

				Font_a = CreateFont(14, 0, 0, 0, 400, 0, 0, 0, 1, 8, 0, 5, 2, TEXT("ariel"));
				
				hInst = ((LPCREATESTRUCT)lParam)->hInstance;
				if (!TBTN_SHOWONSTART.toggle_state) {
					ShowWindow(g_hwnd, SW_FORCEMINIMIZE);
					Minimize();
				}
				firstrun = false;
			}
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
					var_name = var_name.substr(0, var_name.find('('));
					string key = line.substr(line.find('(') + 1, line.find(')') - line.find('(') - 1);
					if (var_name.size() == 0 || val.size() == 0)
						continue;
					//set toggle buttons states
					for (UINT i = 0; i < togglebuttons.size(); i++) {
						if (var_name == togglebuttons[i]->string_in_file)
							togglebuttons[i]->toggle_state = on;
					}
					for (UINT i = 0; i < statcolors.size(); i++) {
						if (var_name == statcolors[i].text_in_file) {
							statcolors[i].red = str_to_int(val.substr(0, val.find(',')));
							statcolors[i].green = str_to_int(val.substr(val.find(',') + 1, val.rfind(',') - val.find(',') - 1));
							statcolors[i].blue = str_to_int(val.substr(val.rfind(',') + 1, val.size()));
						}
					}
					if (var_name == "gamepath") {
						gamepath = val;
					}
					else if (var_name == "targetlines") targetlines = val;
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
					else if (var_name == "goldbuttonpos") {
						if (val == "auto") {
							goldbuttonx = -1;
							goldbuttony = -1;
						}
						else {
							goldbuttonx = str_to_int(val.substr(0, val.find(',')));
							goldbuttony = str_to_int(val.substr(val.find(',') + 1, val.size()));
						}
					}
					else if (var_name == "weaponswapkey") {
						if (val.find("0x") != string::npos)
							weapsonswitchkey = str_to_hex(val);
						else
							weapsonswitchkey = str_to_int(val);
					}
					else if (line[0] == '>') {
						line.erase(0, 1);
						if (line.size() == 0)
							continue;
						stats_in_file.push_back(line);
					}
					else if (val[0] == '#') {						
						val.erase(0, 1);
						if (val.size() == 0)
							continue;
						spam_text_in_file.push_back(val);					
					}
					else {
						for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
							if (var_name == tokenhackfunctions[i]->name_in_file) {
								functionnameyoffsum += functionnameheight;
								tokenhackfunctions[i]->on = on;
								tokenhackfunctions[i]->hotkey_key = (WORD)str_to_int(key);
								tokenhackfunctions[i]->temp_hotkey = tokenhackfunctions[i]->hotkey_key;
								tokenhackfunctions[i]->function_handle = CreateWindow(TEXT("STATIC"), (LPCWSTR)str_to_wstr(tokenhackfunctions[i]->window_text).c_str(), 
									WS_CHILD | WS_VISIBLE,
									functionnamexoff, functionnameyoffsum, 
									functionnamewidth, functionnameheight, 
									hwnd, 0, NULL, NULL);
								tokenhackfunctions[i]->button_handle = CreateWindow(TEXT("BUTTON"), TEXT(""), 
									WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 
									buttonxoff, functionnameyoffsum, 
									buttonwidth, buttonheight, 
									hwnd, (HMENU)tokenhackfunctions[i]->button_id, NULL, NULL);
								tokenhackfunctions[i]->checkbox_handle = CreateWindow(TEXT("BUTTON"), TEXT("T"), 
									WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 
									checkboxxoff, functionnameyoffsum, 
									checkboxwidth, checkboxheight,
									hwnd, (HMENU)tokenhackfunctions[i]->checkbox_id, NULL, NULL);
								changebuttontext(tokenhackfunctions[i]->button_handle, tokenhackfunctions[i]->hotkey_key);
								setbuttonstate(hwnd, tokenhackfunctions[i]->button_handle, tokenhackfunctions[i]->checkbox_id, tokenhackfunctions[i]->on);
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
			
			if (READ.on || AUTOROLL.on) {
				statbox = CreateWindow(TEXT("EDIT"), TEXT(""),
					WS_CHILD | WS_VISIBLE | WS_BORDER /*| ES_WANTRETURN*/ | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_READONLY,
					0, 0,
					0, 0,
					hwnd, (HMENU)statboxid, NULL, NULL);
				for (UINT i = 0; i < statboxtabbuttons.size(); i++) {
					statboxtabbuttons[i]->handle = CreateWindow(TEXT("BUTTON"), (LPCWSTR)str_to_wstr(statboxtabbuttons[i]->window_text).c_str(),
						WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
						statboxtabbuttons[i]->xpos, statboxtabbuttons[i]->ypos,
						statboxtabbuttons[i]->width, statboxtabbuttons[i]->height,
						hwnd, (HMENU)statboxtabbuttons[i]->id, NULL, NULL);
				}
				if (AUTOROLL.on) {
					string searching = "Stats in file:\r\n";
					for (int i = 0; i < (int)stats_in_file.size(); i++)
						searching += '>' + stats_in_file[i] + "\r\n";
					searching += '\0';
					SendMessage(statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());
				}
			}
			
			for (UINT i = 0; i < functiontabbuttons.size(); i++) {
				functiontabbuttons[i]->handle = CreateWindow(TEXT("BUTTON"), (LPCWSTR)str_to_wstr(functiontabbuttons[i]->window_text).c_str(),
					WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
					functiontabbuttons[i]->xpos, functiontabbuttons[i]->ypos,
					functiontabbuttons[i]->width, functiontabbuttons[i]->height,
					hwnd, (HMENU)functiontabbuttons[i]->id, NULL, NULL);
			}
			for (UINT i = 0; i < normalbuttons.size(); i++) {
				normalbutton a = *normalbuttons[i];
				normalbuttons[i]->handle = CreateWindow(TEXT("BUTTON"), (LPCWSTR)str_to_wstr(a.window_text).c_str(), 
					(a.show_state ? WS_VISIBLE : NULL) | WS_CHILD | BS_OWNERDRAW, 
					a.xpos, (a.yoffset != nullptr) ? *a.yoffset + a.ypos : a.ypos,
					a.width, a.height,
					hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font) 
					SendMessage(GetDlgItem(g_hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < togglebuttons.size(); i++) {
				togglebutton a = *togglebuttons[i];
				a.handle = CreateWindow(TEXT("BUTTON"), (LPCWSTR)str_to_wstr(a.window_text).c_str(),
					WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
					a.xpos, (a.yoffset != nullptr) ? *a.yoffset + a.ypos : a.ypos,
					a.width, a.height,
					hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font) 
					SendMessage(GetDlgItem(g_hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < autoradiobuttons.size(); i++) {
				autoradiobutton a = *autoradiobuttons[i];
				autoradiobuttons[i]->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a.window_text).c_str(),
					(a.show_state) ? WS_VISIBLE | WS_CHILD | BS_OWNERDRAW : WS_CHILD | BS_OWNERDRAW,
					a.xpos, a.ypos,
					a.width, a.height,
					hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font)
					SendMessage(GetDlgItem(g_hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < checkboxbuttons.size(); i++) {
				checkboxbutton a = *checkboxbuttons[i];
				checkboxbuttons[i]->handle = CreateWindow(TEXT("BUTTON"), str_to_wstr(a.window_text).c_str(),
					(a.show_state) ? WS_VISIBLE | WS_CHILD | BS_OWNERDRAW : WS_CHILD | BS_OWNERDRAW,
					a.xpos, a.ypos, 
					a.width, a.height,
					g_hwnd,	(HMENU)a.id, NULL, NULL);
				if (a.font)
					SendMessage(GetDlgItem(g_hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < editcontrols.size(); i++) {
				editcontrol a = *editcontrols[i];
				editcontrols[i]->handle = CreateWindow(TEXT("EDIT"), str_to_wstr(a.window_text).c_str(),
					WS_CHILD | (a.show_state ? WS_VISIBLE : NULL) | ES_LEFT,
					a.xpos, a.ypos,
					a.width, a.height, 
					hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font)
					SendMessage(GetDlgItem(hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < staticcontrols.size(); i++) {
				staticcontrol a = *staticcontrols[i];
				staticcontrols[i]->handle = CreateWindow(TEXT("STATIC"), str_to_wstr(a.window_text).c_str(),
					(a.show_state) ? WS_VISIBLE | WS_CHILD : WS_CHILD,
					a.xpos, a.ypos,
					a.width, a.height,
					hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font) 
					SendMessage(GetDlgItem(hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < custom_trackbars.size(); i++) {
				custom_trackbar* a = custom_trackbars[i];
				a->handle = CreateWindow(custom_trackbar_classname, TEXT(""),
					(a->showstate ? WS_VISIBLE : NULL) | WS_CHILD,
					a->xpos, a->ypos, a->width, a->height,
					hwnd, (HMENU)a->id, NULL, NULL);
			}

			SetWindowPos(hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250/*180*/, functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);
		
			g_menu = CreatePopupMenu();
			if (TBTN_SOUND.toggle_state)
				for (int freshmeatx = 0; freshmeatx < 31; freshmeatx++)
					AppendMenu(g_menu, MF_STRING, 3003, TEXT("FRESHMEAT"));
			if (filein || fileout) AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
			AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
			AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			break;
		}
		case WM_COMMAND:{
			DRAWITEMSTRUCT dis;
			RECT temprect;
			static bool tog = false;
			bool is_function = false;
			for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
				if (tokenhackfunctions[i]->button_id == LOWORD(wParam) && tokenhackfunctions[i]->on) {
					is_function = true;
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(HOTKEYBOX), hwnd, DlgProc);
					if (dw != 0)
						hotkeychange(tokenhackfunctions[i]->button_handle, tokenhackfunctions[i]->button_id, tokenhackfunctions[i]->hotkey_key, dw);
					break;
				}
				if (tokenhackfunctions[i]->checkbox_id == LOWORD(wParam)) {
					if (tokenhackfunctions[i]->on) {//if on, turn off				
						tokenhackfunctions[i]->on = 0;
						EnableWindow(tokenhackfunctions[i]->button_handle, false);
					}
					else {//if off, turn on					
						tokenhackfunctions[i]->on = 1;
						EnableWindow(tokenhackfunctions[i]->button_handle, true);
					}
					changeconfigonstate(tokenhackfunctions[i]->name_in_file);
				}
			}
			for (UINT i = 0; i < functiontabbuttons.size(); i++) {
				if (functiontabbuttons[i]->id == LOWORD(wParam)) {
					/*if (functiontabbuttons[i]->toggle_state == 1)
						break;*/
					for (UINT j = 0; j < functiontabbuttons.size(); j++) {
						functiontabbuttons[j]->toggle_state = 0;
						HWND thandle = functiontabbuttons[j]->handle;
						GetClientRect(thandle, &temprect);
						dis.CtlID = GetDlgCtrlID(thandle);
						dis.CtlType = ODT_BUTTON;
						dis.hDC = GetDC(thandle);
						dis.itemState = ODS_INACTIVE;
						dis.rcItem = temprect;
						SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);
						ReleaseDC(thandle, dis.hDC);
					}
					functiontabbuttons[i]->toggle_state = 1;
					SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(BTN_RELOAD.id, 0x01), NULL);
					break;
				}
			}
			for (UINT i = 0; i < statboxtabbuttons.size(); i++) {
				if (statboxtabbuttons[i]->id == LOWORD(wParam)) {
					if (statboxtabbuttons[i]->toggle_state == 1)
						break;
					for (UINT j = 0; j < statboxtabbuttons.size(); j++) {
						statboxtabbuttons[j]->toggle_state = 0;
						HWND thandle = statboxtabbuttons[j]->handle;
						GetClientRect(thandle, &temprect);
						dis.CtlID = GetDlgCtrlID(thandle);
						dis.CtlType = ODT_BUTTON;
						dis.hDC = GetDC(thandle);
						dis.itemState = ODS_INACTIVE;
						dis.rcItem = temprect;
						SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);
						ReleaseDC(thandle, dis.hDC);
					}
					statboxtabbuttons[i]->toggle_state = 1;
					if (statboxtabbuttons[i]->id == STAT_TAB_BTN_OUTPUT.id) {

					}
				}
			}
			//
			for (UINT i = 0; i < autoradiobuttons.size(); i++) {
				if (autoradiobuttons[i]->id == LOWORD(wParam)) {
					int group = autoradiobuttons[i]->group;
					for (UINT j = 0; j < autoradiobuttons.size(); j++) {
						if (group == 0)
							break;
						if (autoradiobuttons[j]->group == group) {
							autoradiobuttons[j]->toggle_state = 0;
							HWND thandle = autoradiobuttons[j]->handle;
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
					autoradiobuttons[i]->toggle_state = 1;
				}
			}
			for (UINT i = 0; i < checkboxbuttons.size(); i++) {
				if (checkboxbuttons[i]->id == LOWORD(wParam)) {
					if (checkboxbuttons[i]->toggle_state)
						checkboxbuttons[i]->toggle_state = 0;
					else
						checkboxbuttons[i]->toggle_state = 1;
					if (LOWORD(wParam) == CHK_BTN_TOGGLEALL.id) {
						tog = true;
						if (toggleall) {
							toggleall = false;
							for (int i = 0; i < (int)tokenhackfunctions.size(); i++)
								if (tokenhackfunctions[i]->on)
									SendMessage(hwnd, WM_COMMAND, (WPARAM)tokenhackfunctions[i]->checkbox_id, NULL);
						}
						else {
							toggleall = true;
							for (int i = 0; i < (int)tokenhackfunctions.size(); i++)
								if (!tokenhackfunctions[i]->on)
									SendMessage(hwnd, WM_COMMAND, (WPARAM)tokenhackfunctions[i]->checkbox_id, NULL);
						}
						tog = false;
						break;
					}
				}
			}
			for (UINT i = 0; i < normalbuttons.size(); i++) {
				if (normalbuttons[i]->id == LOWORD(wParam)) {
					if (BTN_RELOAD.id == normalbuttons[i]->id) {	
						EnumChildWindows(hwnd, EnumChildProc, DESTROY_WINDOWS);
						turn_all_functions_off();
						stats_in_file.erase(stats_in_file.begin(), stats_in_file.end());
						spam_text_in_file.erase(spam_text_in_file.begin(), spam_text_in_file.end());
						gamepath = "";
						reset_color_text_RGB();
						ClipCursor(NULL);
						InvalidateRect(g_hwnd, &getclientrect(g_hwnd), true);
						SendMessage(hwnd, WM_CREATE, NULL, NULL);
						SendMessage(hwnd, WM_SIZE, NULL, NULL);
						if (HIWORD(wParam) == 0) {
							cout << "\n-------------------RELOAD-------------------" << '\n';
							if (TBTN_SOUND.toggle_state)
								PlaySound(MAKEINTRESOURCE(RELOAD), NULL, SND_ASYNC | SND_RESOURCE);
						}
					}
					else if (BTN_CONFIG.id == normalbuttons[i]->id) {
						if (ShellExecute(hwnd, TEXT("open"), TEXT("config.txt"), NULL, NULL, SW_SHOW) == (HINSTANCE)ERROR_FILE_NOT_FOUND) {
							MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
							PostQuitMessage(0);
						}
					}
					else if (BTN_LAUNCH.id == normalbuttons[i]->id) {
						if (gamepath.size() == 0) {
							OPENFILENAME ofn;
							char File[MAX_PATH];
							ZeroMemory(&ofn, sizeof(ofn));
							ofn.lStructSize = sizeof(ofn);
							ofn.hwndOwner = hwnd;
							ofn.lpstrFile = (LPWSTR)File;
							ofn.lpstrFile[0] = '\0';
							ofn.nMaxFile = MAX_PATH;
							ofn.lpstrFilter = TEXT("exe\0*.exe*\0All\0*.*\0Shortcut\0*.lnk*\0Text\0*.TXT\0");							
							ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS;

							GetOpenFileName(&ofn);

							gamepath = wstr_to_str(ofn.lpstrFile);

							vector<string> text;
							filein.open("config.txt");
							if (!filein)
								break;
							string line;
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
								if (var_name == "gamepath")
									text[i] = "gamepath\t\t:\t" + gamepath;
							}

							fileout.open("config.txt");
							if (!fileout)
								break;
							for (unsigned int i = 0; i<text.size(); i++)
								fileout << text[i] << '\n';
							fileout.close();
						}
						gamedir = gamepath.substr(0, gamepath.rfind('\\'));
						string gamename = gamepath.substr(gamepath.rfind('\\') + 1, gamepath.size());

						if (gamepath.size() > 0)
							ShellExecute(hwnd, TEXT("open"), str_to_wstr(gamename).c_str(), str_to_wstr(targetlines).c_str(), str_to_wstr(gamedir).c_str(), SW_SHOW);
					}
					else if (BTN_COW.id == normalbuttons[i]->id) {
						static int cowc = 0;
						if (TBTN_SOUND.toggle_state) {
							if (cowc == 2)
								cowc = 0;
							if (cowc == 0)
								PlaySound(MAKEINTRESOURCE(COW1), NULL, SND_ASYNC | SND_RESOURCE);
							if (cowc == 1)
								PlaySound(MAKEINTRESOURCE(COW2), NULL, SND_ASYNC | SND_RESOURCE);
							cowc++;
						}
					}
					else if (BTN_MUSHROOM.id == normalbuttons[i]->id) {
						static int mushroomc = 508;
						if (TBTN_SOUND.toggle_state) {
							if (mushroomc == 517)
								mushroomc = 508;							
							PlaySound(MAKEINTRESOURCE(mushroomc), NULL, SND_ASYNC | SND_RESOURCE);							
							mushroomc++;
						}
					}
					break;
				}
			}
			for (UINT i = 0; i < togglebuttons.size(); i++) {
				if (togglebuttons[i]->id == LOWORD(wParam)) {	// is it a toggle button? if so, which one?				
					if (togglebuttons[i]->id == TBTN_CONSOLE.id) {
						if (TBTN_CONSOLE.toggle_state) {
							FreeConsole();
						}
						else {
							cout.clear();
							CONSOLE_ON();
							SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
							//SetActiveWindow(hwnd);
							//SetFocus(g_hwnd);
						}
					}
					else if (togglebuttons[i]->id == TBTN_RAINBOW.id) {
						for (unsigned int i = 0; i < tokenhackfunctions.size(); i++) {
							InvalidateRect(tokenhackfunctions[i]->function_handle, &getclientrect(tokenhackfunctions[i]->function_handle), TRUE);
						}
					}

					//always flip toggle state
					if (togglebuttons[i]->toggle_state) togglebuttons[i]->toggle_state = 0;
					else togglebuttons[i]->toggle_state = 1;
					changeconfigonstate(togglebuttons[i]->string_in_file);
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

			//SetFocus(NULL);//remove focus from button

			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(g_hwnd, &ps);

			//paint background
			FillRect(hDC, &ps.rcPaint, (blackback) ? hbblack : hbnormal);

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
		case WM_SYSCOMMAND:{
			switch (wParam) {
				case SC_MINIMIZE:
					Minimize();
					return 0;
					break;
				case SC_CLOSE:
					Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
					PostQuitMessage(0);
					return 0;
					break;
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
			for (UINT i = 0; i < statboxtabbuttons.size(); i++) {
				minwidth += statboxtabbuttons[i]->width;
			}
			MoveWindow(statbox, statboxxoff, tabbuttonheight - 1, ((curwidth < minwidth) ? minwidth : curwidth), ((curheight < minheight) ? minheight : curheight), true);
			if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
				is_drawing_static = true;
				for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
					GetClientRect(tokenhackfunctions[i]->function_handle, &r);
					RedrawWindow(tokenhackfunctions[i]->function_handle, &r, NULL, RDW_INVALIDATE);
				}
			}
			break;
		}
		case WM_CTLCOLORSTATIC:{
			if (color_static_brush != NULL) {
				DeleteObject(color_static_brush);
				color_static_brush = NULL;
			}
			COLORREF blue = RGB(0, 118, 255);
			COLORREF black = RGB(10, 10, 10);
			COLORREF red = RGB(254, 33, 61);
			COLORREF gray = RGB(239, 239, 239);
			//COLORREF purple = RGB(148, 0, 211);
			static HBRUSH hbnormal = CreateSolidBrush(gray);
			static HBRUSH hbblack = CreateSolidBrush(black);
			static HBRUSH hbred = CreateSolidBrush(red);
			static HBRUSH hbblue = CreateSolidBrush(blue);
			HDC hdcStatic = (HDC)wParam;
			if ((HWND)lParam == statbox) {
				RECT rec;
				HDC whydoineedthis = GetDC(statbox);
				GetClientRect(statbox, &rec);
				SetTextColor(hdcStatic, blue);
				SetBkColor(hdcStatic, (blackback) ? black : gray);
				FrameRect(whydoineedthis, &rec, hbblue);
				ReleaseDC(statbox, whydoineedthis);
				return (INT_PTR)((blackback) ? hbblack : hbnormal);
			}
			if (get_window_class_name((HWND)lParam) != "Static")
				break;
			static int count = 0;
			count++;
			static BYTE red1 = 255, gre1 = 0, blu1 = 105;
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
					if ((HWND)lParam == tokenhackfunctions[tokenhackfunctions.size() - 1]->function_handle)
						count = 0;
					count = 0;
					is_drawing_static = false;
					rainbowc = rainbowc1;
					rainbowc1--;
					if (rainbowc1 == 0)
						rainbowc1 = 12;
				}
			}
			else
				red1 = 148, gre1 = 0, blu1 = 211;
			//bool is_function = false;
			for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
				if ((HWND)lParam == tokenhackfunctions[i]->function_handle) {
					//is_function = true;
					SetTextColor(hdcStatic, RGB(red1, gre1, blu1));
					SetBkColor(hdcStatic, (blackback) ? black : gray);
					return (INT_PTR)((blackback) ? hbblack : hbnormal);
				}
			}
			SetTextColor(hdcStatic, RGB(100, 100, 100));
			SetBkColor(hdcStatic, (blackback) ? black : gray);
			return (INT_PTR)((blackback) ? hbred/*hbblack */: hbnormal);
		}
		case WM_CTLCOLOREDIT:{
			if (get_window_class_name((HWND)lParam) != "Edit")
				break;
			COLORREF blue = RGB(0, 118, 255);
			COLORREF black = RGB(10, 10, 10);
			COLORREF red = RGB(254, 33, 61);
			COLORREF gray = RGB(239, 239, 239);
			HDC hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(255, 0, 0));
			SetBkColor(hdc, (blackback) ? black : gray);
			return (INT_PTR)GetSysColorBrush(COLOR_3DHILIGHT); 
		}
		case WM_DRAWITEM:{
			#pragma region colors			
			static int count = 2;
			static BYTE red1, gre1, blu1;				
			if (TBTN_RAINBOW.toggle_state>1) {
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

			COLORREF highlight = RGB(red1, gre1, blu1);
			COLORREF normal_back = RGB(239, 239, 239);
			COLORREF black_back = RGB(10, 10, 10);
			COLORREF tokenhack_function_button = RGB(254, 33, 61);
			COLORREF normal_button = RGB(255, 206, 61);
			COLORREF toggle_on = RGB(0, 255, 0);
			COLORREF toggle_off = RGB(255, 0, 0);
			COLORREF statbox_tab = RGB(0, 118, 255);
			COLORREF image_button = RGB(0, 118, 255);
			static HBRUSH hb_highlight = CreateSolidBrush(RGB(100, 100, 100));
			static HBRUSH hb_tokenhack_function_button = CreateSolidBrush(tokenhack_function_button);
			static HBRUSH hb_normal_back = CreateSolidBrush(normal_back);
			static HBRUSH hb_black_back = CreateSolidBrush(black_back);
			static HBRUSH hb_normal_button = CreateSolidBrush(normal_button);
			static HBRUSH hb_toggle_on = CreateSolidBrush(toggle_on);
			static HBRUSH hb_toggle_off = CreateSolidBrush(toggle_off);
			static HBRUSH hb_statbox_tab = CreateSolidBrush(statbox_tab);
			static HBRUSH hb_image_button = CreateSolidBrush(image_button);
#pragma endregion
			
			LPDRAWITEMSTRUCT pdis = (DRAWITEMSTRUCT*)lParam;
			SIZE size;

			//draw hotkey buttons
			for (UINT i = 0; i < tokenhackfunctions.size(); i++) {	
				//draw buttons
				if (pdis->CtlID == tokenhackfunctions[i]->button_id) {
					wchar_t ttext[256];
					GetWindowText(GetDlgItem(hwnd, tokenhackfunctions[i]->button_id), ttext, 256);
					string text = wstr_to_str((wstring)ttext);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, tokenhackfunctions[i]->on ? tokenhack_function_button : (blackback ? black_back : normal_back));
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : blackback ? black_back : normal_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);

					//DrawEdge(pdis->hDC, &pdis->rcItem, (pdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT);
					FrameRect(pdis->hDC, &pdis->rcItem, hb_tokenhack_function_button);
					return TRUE;
				}
				//draw checkboxes
				if (pdis->CtlID == tokenhackfunctions[i]->checkbox_id) {
					wchar_t ttext[2];					
					GetWindowText(GetDlgItem(hwnd, tokenhackfunctions[i]->checkbox_id), ttext, 2);
					string text = wstr_to_str((wstring)ttext);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, tokenhackfunctions[i]->on ? tokenhack_function_button : pdis->itemState & ODS_SELECTED ? highlight : (blackback ? black_back : normal_back));
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : (blackback ? black_back : normal_back));
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);

					//DrawEdge(pdis->hDC, &pdis->rcItem, (pdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT);
					FrameRect(pdis->hDC, &pdis->rcItem, hb_tokenhack_function_button);
					return TRUE;
				}
			}
			//draw function tab buttons
			for (UINT i = 0; i < functiontabbuttons.size(); i++) {
				functiontabbutton a = *functiontabbuttons[i];
				if (pdis->CtlID == a.id) {					
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL/*FW_BOLD*/, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("SYSTEM") /*TEXT("MS PMINCHO")*//*str_to_wstr(fonts[currfont]).c_str()*/);
					SelectObject(pdis->hDC, hFont);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, a.toggle_state ? black_back : (pdis->itemState & ODS_SELECTED) ? blackback ? black_back : normal_back : highlight);
					SetBkColor(pdis->hDC, a.toggle_state ? highlight : (pdis->itemState & ODS_SELECTED) ? highlight : blackback ? black_back : normal_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, a.toggle_state ? hb_highlight : pdis->itemState & ODS_SELECTED ? blackback ? hb_black_back : hb_normal_back : hb_highlight);

					DeleteObject(hFont);
					return TRUE;
				}
			}
			//draw statbox tab buttons
			for (UINT i = 0; i < statboxtabbuttons.size(); i++) {
				statboxtabbutton a = *statboxtabbuttons[i];
				if (pdis->CtlID == a.id) {
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, a.toggle_state ? blackback ? black_back : normal_back : pdis->itemState & ODS_SELECTED ? blackback ? black_back : normal_back : statbox_tab);
					SetBkColor(pdis->hDC, a.toggle_state ? statbox_tab : pdis->itemState & ODS_SELECTED ? statbox_tab : blackback ? black_back : normal_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, a.toggle_state ? hb_statbox_tab : pdis->itemState & ODS_SELECTED ? blackback ? hb_black_back : hb_normal_back : hb_statbox_tab);
										
					return TRUE;
				}
			}
			//
			//draw radio buttons
			for (UINT i = 0; i < autoradiobuttons.size(); i++) {
				autoradiobutton a = *autoradiobuttons[i];
				if (pdis->CtlID == a.id) {
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					HFONT hFont = CreateFont(14, 0, 0, 0, 400, 0, 0, 0, 1, 8, 0, 5, 2, TEXT("ariel"));
					SelectObject(pdis->hDC, hFont);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);					
					SetTextColor(pdis->hDC, a.toggle_state ? ((blackback ? black_back : normal_back)) : pdis->itemState & ODS_SELECTED ? ((blackback ? black_back : normal_back)) : a.color);
					SetBkColor(pdis->hDC, a.toggle_state ? a.color : pdis->itemState & ODS_SELECTED ? highlight : ((blackback ? black_back : normal_back)));
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH temp = CreateSolidBrush(a.color);
					FrameRect(pdis->hDC, &pdis->rcItem, temp);
					DeleteObject(temp);
					DeleteObject(hFont);
					return TRUE;
				}
			}
			//draw checkbox buttons
			for (UINT i = 0; i < checkboxbuttons.size(); i++) {
				checkboxbutton a = *checkboxbuttons[i];
				if (pdis->CtlID == a.id) {
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, a.id), ttext, 2);
					string text = wstr_to_str((wstring)ttext);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, a.toggle_state ? a.color : pdis->itemState & ODS_SELECTED ? highlight : (blackback ? black_back : normal_back));
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : (blackback ? black_back : normal_back));
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH temp = CreateSolidBrush(a.color);
					FrameRect(pdis->hDC, &pdis->rcItem, temp);
					DeleteObject(temp);
					return TRUE;
				}
			}
			//draw normal buttons
			for (UINT i = 0; i < normalbuttons.size(); i++) {
				normalbutton a = *normalbuttons[i];
				if (pdis->CtlID == a.id) {
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, a.color);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : blackback ? black_back : normal_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					HBRUSH temp = CreateSolidBrush(a.color);
					FrameRect(pdis->hDC, &pdis->rcItem, temp);
					DeleteObject(temp);
					return TRUE;
				}
			}
			//draw toggle buttons
			for (UINT i = 0; i < togglebuttons.size(); i++) {
				if (pdis->CtlID == togglebuttons[i]->id) {
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, togglebuttons[i]->toggle_state ? toggle_on : toggle_off);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : blackback ? black_back : normal_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, togglebuttons[i]->toggle_state ? hb_toggle_on : hb_toggle_off);
					return TRUE;
				}
			}
			return TRUE;
		}
		case WM_SETCURSOR:{
			if (get_window_class_name((HWND)wParam) == "Button") {
				SetCursor(press_cursor);
				//SetCursor(token_cursor);
				return TRUE;
			}
			if (get_window_class_name((HWND)wParam) == "Edit") 				
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
						if (TBTN_SOUND.toggle_state)
							PlaySound(MAKEINTRESOURCE(CAIN), NULL, SND_ASYNC | SND_RESOURCE);
						MessageBox(NULL, TEXT("Let Token Know"), TEXT("NOOOOOOOB"), MB_OK);
					}
					if (clicked == 3002) {
						ShellExecute(hwnd, TEXT("open"), TEXT("config.txt"), NULL, NULL, SW_SHOW);
					}
					if (clicked == 3003) {
						ShowWindow(BTN_MUSHROOM.handle, SW_SHOW);
						ShowWindow(BTN_COW.handle, SW_SHOW);
						/*ShowWindow(hconfig, SW_HIDE);
						ShowWindow(hlaunch, SW_HIDE);*/
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
		case WM_UPDATE_WND:{
			RedrawWindow((HWND)wParam, &getclientrect((HWND)wParam), NULL, lParam);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			ShowWindow(hwnd, SW_HIDE);
			Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
			if (TBTN_SOUND.toggle_state)
				PlaySound(MAKEINTRESOURCE(BYE), NULL, SND_RESOURCE);
			PostQuitMessage(0);
		}
		case WM_TEST:{
			static bool kill_dll = false;
		
			HWND wnd = GetForegroundWindow();
			static HHOOK callwndhook;
			static HHOOK callwndrethook;
			static HHOOK getmessagehook;

			static RECT rcstartpos;
			static RECT rcdesktop;
			
			rcdesktop = getclientrect(GetDesktopWindow());

			if (kill_dll) {
				kill_dll = false;
				//MoveWindow(wnd, rcstartpos.left, rcstartpos.top, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, true);
				UnhookWindowsHookEx(callwndhook);
				UnhookWindowsHookEx(callwndrethook);
				UnhookWindowsHookEx(getmessagehook);

			}
			else {
				kill_dll = true;
				rcstartpos = getwindowrect(wnd);
				//MoveWindow(wnd, rcdesktop.right, rcdesktop.bottom, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, true);
				if (!InjectDll(DLL_NAME, wnd, &callwndhook, &callwndrethook, &getmessagehook)) {
					MessageBox(NULL, L"Dll injection failed", L"Error", MB_OK);
					PostQuitMessage(0);
				}
			}
			

			

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	ClipCursor(NULL);
	for (int i = 10000; i < (int)(10000 + hotkeys.size()); i++)
		UnregisterHotKey(NULL, i);
	update_hotkeykeys();
	for (int i = 10000; i < (int)(10000 + hotkeys.size()); i++)
		RegisterHotKey(NULL, i, HIBYTE(hotkeys[i - 10000]), LOBYTE(hotkeys[i - 10000]));
	switch (message) {	
		case WM_INITDIALOG:{
			is_hotkey_dialog_visible = true;
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
			return FALSE;
		}
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case BTN_OK:{
					dw = (WORD)SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_GETHOTKEY, 0, 0);
					if ((dw - (int)LOBYTE(dw)) / 256 == 1)
						dw += 768;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 4)
						dw -= 768;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 3)
						dw += 768;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 6)
						dw -= 768;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 6)
						dw -= 768;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 12)
						dw -= 256 * 11;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 14)
						dw -= 256 * 11;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 9)
						dw -= 256 * 5;
					else if ((dw - (int)LOBYTE(dw)) / 256 == 11)
						dw -= 256 * 5;
					if (dw > ((256 * 7) + 255))
						dw -= 256 * 8;
					for (int i = 10000; i < (int)(10000 + hotkeys.size()); i++)
						UnregisterHotKey(NULL, i);
					is_hotkey_dialog_visible = false;
					EndDialog(hwnd, KEYCONTROL);
					break;
				}
			}
			break;
		}
		case WM_CLOSE:{
			dw = 0;
			for (int i = 10000; i < (int)(10000 + hotkeys.size()); i++)
				UnregisterHotKey(NULL, i);
			is_hotkey_dialog_visible = false;
			EndDialog(hwnd, BTN_OK);
			break;
		}
		case WM_DESTROY:{
			for (int i = 10000; i < (int)(10000 + hotkeys.size()); i++)
				UnregisterHotKey(NULL, i);
			is_hotkey_dialog_visible = false;
			EndDialog(hwnd, BTN_OK);
			break;
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
VOID CALLBACK timerProc(HWND hwnd, UINT message, UINT_PTR id, DWORD time) {
	if (id == timer2) {
		if (click_toggle) {
			post_left_click();
		}
		else
			KillTimer(0, timer2);
	}
}
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);

	if (wParam == WM_MOUSEMOVE || wParam == WM_LBUTTONDOWN) {
		static int count = 0;	
		if (TBTN_RAINBOW.toggle_state > 1 && count++ % TBTN_RAINBOW.toggle_state == 0 && !is_drawing_static) {
			is_drawing_static = true;
			for (unsigned int i = 0; i < tokenhackfunctions.size(); i++) {
				RedrawWindow(tokenhackfunctions[i]->function_handle, &getclientrect(tokenhackfunctions[i]->function_handle), NULL, RDW_INVALIDATE);
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

		//cout << temppoint.x << "      " << temppoint.y << '\n';

		current_handle = ChildWindowFromPointEx(g_hwnd, client_cursor_pos, CWP_SKIPDISABLED | CWP_SKIPINVISIBLE);//get window handle under cursor

		if (wParam == WM_LBUTTONDOWN) {
			SetFocus(current_handle);
		}

		if (get_window_class_name(current_handle) == "Button") {		//only hightlight buttons 
			for (unsigned int i = 0; i < tokenhackfunctions.size(); i++) {
				if (current_handle == tokenhackfunctions[i]->button_handle) {
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
		else if (get_window_class_name(previous_handle) == "Button") {//current is not a button, but previous was
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
		//return CallNextHookEx(mhook, code, wParam, lParam);
	}
	if (wParam == WM_MOUSEWHEEL) {
		if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
			is_drawing_static = true;
			for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
				RedrawWindow(tokenhackfunctions[i]->function_handle, &getclientrect(tokenhackfunctions[i]->function_handle), NULL, RDW_INVALIDATE);
			}
		}
		//return CallNextHookEx(mhook, code, wParam, lParam);
	}
	
	return CallNextHookEx(mhook, code, wParam, lParam);
}
LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN ||	wParam == WM_SYSKEYDOWN){
	#pragma region virtual keycode processing
		/*if (is_hotkey_dialog_visible)
			return CallNextHookEx(kbhook, code, wParam, lParam);*/
		wchar_t name[0x100] = {0};
		DWORD dwMsg = 1;
		dwMsg += key.scanCode << 16;
		dwMsg += key.flags << 24;
		GetKeyNameText(dwMsg, (LPTSTR)name, 255);
		string sname = wstr_to_str((wstring)name);
		if ((GetKeyState(VK_MENU) & 0x8000) != 0) {
			key.vkCode += 256;
			sname.insert(0, "ALT+");
		}
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) {
			key.vkCode += 256 * 4;
			sname.insert(0, "SHIFT+");
		}
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
			key.vkCode += 256 * 2;
			sname.insert(0, "CTRL+");
		}
		if ((GetKeyState(VK_LWIN) & 0x8000) != 0) {
			key.vkCode += 256 * 8;
			sname.insert(0, "LEFT WINDOWKEY+");
		}
		if ((GetKeyState(VK_RWIN) & 0x8000) != 0) {
			key.vkCode += 256 * 8;
			sname.insert(0, "RIGHT WINDOWKEY+");
		}

		if (!is_hotkey_dialog_visible && TBTN_HOOKOUTPUT.toggle_state) {
			cout << key.vkCode << " " << sname << '\n';
			for (int i = 10000; i < (int)(10000 + hotkeys.size()); i++)
				UnregisterHotKey(NULL, i);
		}
	#pragma endregion	
	#pragma region skill bug input processing
		static bool trap = false;
		static bool pause = false;
		static bool type = false;
		static bool skillbugsetgetinputon = false;
		static int skillbugsetcount = 0;
		static WORD firstkey = NULL;
		static WORD secondkey = NULL;
		if (skillbugsetgetinputon) {
			if (skillbugsetcount) {
				if (key.vkCode != SKILLBUGSET.hotkey_key) {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
					secondkey = (WORD)key.vkCode;
				}
				else {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
					firstkey = NULL;
					secondkey = NULL;
				}
				skillbugsetcount = 0;
				skillbugsetgetinputon = false;
			}
			else {
				skillbugsetcount++;
				if (key.vkCode != SKILLBUGSET.hotkey_key) {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
					firstkey = (WORD)key.vkCode;
				}
				else {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
					firstkey = NULL;
					secondkey = NULL;
					skillbugsetcount = 0;
					skillbugsetgetinputon = false;
				}
			}
			return CallNextHookEx(kbhook, code, wParam, lParam);
		}
#pragma endregion
	#pragma region functions
		if (is_hotkey_dialog_visible) {
			if (TBTN_HOOKOUTPUT.toggle_state)
				cout << "hotkey box: " << key.vkCode << " " << sname << '\n';
		}		
		else if (key.vkCode == WMC.hotkey_key			&&WMC.on		&&!pause	&&!type) {
			if (!trap) {
				trap = true;
				HWND wnd = GetForegroundWindow();
				if (getwindowtext(GetForegroundWindow()) == "Program Manager") {
					ClipCursor(&getwindowrect(GetDesktopWindow()));
				}
				else
					ClipCursor(&getmappedclientrect(wnd));
			}
			else {
				trap = false;
				ClipCursor(NULL);
			}
		}
		else if (key.vkCode == CLOSE.hotkey_key			&&CLOSE.on		/*&&!pause*/&&!type) {
			PostQuitMessage(0);
		}
		else if (key.vkCode == GAMBLE.hotkey_key || key.vkCode == GAMBLE.temp_hotkey	&&GAMBLE.on		&&!pause	&&!type) {
			type = true;
			static int gcount = 0;
			static POINT a, b; 
			HWND wnd = GetForegroundWindow();
			if (key.vkCode == GAMBLE.temp_hotkey && gcount > 0) {
				if (TBTN_SOUND.toggle_state)
					PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
				GAMBLE.hotkey_key = GAMBLE.temp_hotkey;
				gcount = -1;
			}
			else if (gcount == 0) {
				if (TBTN_SOUND.toggle_state)
					PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
				GAMBLE.hotkey_key = VK_F3;
			}
			else if (gcount == 1) {
				if (TBTN_SOUND.toggle_state)
					PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
				a = getclientcursorpos(wnd);
			}
			else if (gcount == 2) {
				if (TBTN_SOUND.toggle_state)
					PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
				b = getclientcursorpos(wnd);
				post_left_click();
			}
			else {
				keydownup(VK_ESCAPE);
				post_timer_click(a, 250, 0, -1, wnd);
				post_timer_click(b, 500, 0, -1, wnd);
			}
			gcount++;
			type = false;
		}
		else if (key.vkCode == GOLD.hotkey_key			&&GOLD.on		&&!pause	&&!type) {
			type = true;

			HWND wnd = GetForegroundWindow();
			POINT origpt = getclientcursorpos(wnd);
			POINT goldpt;
			if (!get_gold_pos(wnd, &goldpt)) {
				cout << "Invalid window resolution: cound not find gold button" << '\n';
				cout << "Compatible resolutions: (640, 480) | (800, 600) " << '\n';//| (1024, 768) | (1280, 1024)
				cout << "open confix.txt to manually set the gold button pos" << '\n';
			}
			
			post_left_click(goldpt, 0, wnd);
			string s = int_to_str((rand() % 20 + 5000));
			sendkeys(s, wnd);
			keydownup(VK_RETURN, 0, wnd);
			if (GOLD.on == 2) {
				//keydownup(VK_SHIFT, -1, wnd);??
				keybd_event(VK_SHIFT, 0x10, KEYEVENTF_EXTENDEDKEY | 0, 0);
				post_timer_click(origpt, 5, 0, -1, wnd);
				//keydownup(VK_SHIFT, 1, wnd);??
				keybd_event(VK_SHIFT, 0x10, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
				
			}
			else
				post_timer_click(origpt, 5, 0, -1, wnd);

			type = false;
		}
		else if (key.vkCode == WSGCAST.hotkey_key		&&WSGCAST.on	&&!pause	&&!type) {
			switch_weapons();
			post_right_click();
		}
		else if (key.vkCode == SKILLBUG.hotkey_key		&&SKILLBUG.on	&&!pause	&&!type) {
			type = true;
			//shift_press();	
			//keybd_event('Z',0xac,KEYEVENTF_EXTENDEDKEY | 0,0);
			//keybd_event('Z',0xac,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
			//Sleep(70);//70
			//keybd_event('A',0x9e,KEYEVENTF_EXTENDEDKEY | 0,0);
			//keybd_event('A',0x9e,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
			//Sleep(60);//60
			//right_click();
			//shift_release();
			if (secondkey != NULL && firstkey != NULL) {
				keydownup(secondkey);
				switch_weapons();
				keydownup(secondkey);
				switch_weapons();
				keydownup(firstkey);
			}
			type = false;
		}
		else if (key.vkCode == SKILLBUGSET.hotkey_key	&&SKILLBUG.on	&&!pause	&&!type) {
			type = true;
			if (TBTN_SOUND.toggle_state)
				PlaySound(MAKEINTRESOURCE(SKILLBUGSOUND), NULL, SND_ASYNC | SND_RESOURCE);
			skillbugsetgetinputon = true;
			type = false;
		}
		else if (key.vkCode == WSG.hotkey_key			&&WSG.on		&&!pause	&&!type) {
			switch_weapons();
		}
		else if (key.vkCode == NOPICKUP.hotkey_key		&&NOPICKUP.on	&&!pause	&&!type) {
			type = true;
			d2type("/nopickup");
			type = false;
		}
		else if (key.vkCode == LEFTCLICK.hotkey_key		&&LEFTCLICK.on	&&!pause	&&!type) {
			if (LEFTCLICK.on == 1)
				post_left_click();
			if (LEFTCLICK.on == 2) {
				if (!click_toggle) {
					click_toggle = 1;
					timer2 = SetTimer(NULL, 0, 0, timerProc);
				}
				else {
					click_toggle = 0;
				}
			}
		}
		else if (key.vkCode == PAUSE.hotkey_key			&&PAUSE.on					&&!type) {
			if (!pause) {
				memcpy(g_notifyIconData.szInfoTitle, TEXT("PAUSED"), 64);
				memcpy(g_notifyIconData.szInfo, TEXT(" "), 256);
				memcpy(g_notifyIconData.szTip, TEXT("PAUSED"), 128);
				g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENREDICO), IMAGE_ICON, 0, 0, LR_SHARED);
				Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
				pause = true;
				trap = false;
				click_toggle = 0;
				KillTimer(0, timer2);
				ClipCursor(NULL);
			}
			else {
				memcpy(g_notifyIconData.szInfoTitle, TEXT("RESUMED"), 64);
				memcpy(g_notifyIconData.szInfo, TEXT(" "), 256);
				memcpy(g_notifyIconData.szTip, WVERSION.c_str(), 128);
				g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENICO), IMAGE_ICON, 0, 0, LR_SHARED);
				Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
				pause = false;
				SendMessage(g_hwnd, WM_COMMAND, BTN_RELOAD.id, NULL);
			}
		}
		else if (key.vkCode == ROLL.hotkey_key			&&ROLL.on		&&!pause	&&!type) {
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
		}
		else if (key.vkCode == DROP.hotkey_key			&&DROP.on		&&!pause	&&!type) {
			type = true;
			HWND wnd = GetForegroundWindow();
			d2type(".drop", wnd);
			keydownup(VK_MENU, -1);	
			keydownup(VK_F4, 0, wnd, true, true);
			keydownup(VK_MENU, 1);
			PostQuitMessage(0);
			type = false;
		}
		else if (key.vkCode == AUTOROLL.hotkey_key		&&AUTOROLL.on	&&!pause	&&!type) {
			type = true;

			HWND wnd = GetForegroundWindow();

			stats_in_file.erase(stats_in_file.begin(), stats_in_file.end());

			filein.open("config.txt");
			if (!filein) {
				MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
				PostQuitMessage(0);
			}
			while (filein.is_open()) {
				while (filein.good()) {
					string line;
					getline(filein, line);
					line = line.substr(0, line.find('*'));
					line.erase(remove(line.begin(), line.end(), '\t'), line.end());
					string val = line.substr(line.find(':') + 1, line.size());
					line.erase(remove(line.begin(), line.end(), ' '), line.end());
					transform(line.begin(), line.end(), line.begin(), ::tolower);
					string var_name = line.substr(0, line.find(':'));
					if (line.find('>') != string::npos) {
						line.erase(0, 1);
						if (line.size() == 0)
							continue;
						stats_in_file.push_back(line);
					}
					for (UINT i = 0; i < statcolors.size(); i++) {
						if (var_name == statcolors[i].text_in_file) {
							statcolors[i].red = str_to_int(val.substr(0, val.find(',')));
							statcolors[i].green = str_to_int(val.substr(val.find(',') + 1, val.rfind(',') - val.find(',') - 1));
							statcolors[i].blue = str_to_int(val.substr(val.rfind(',') + 1, val.size()));
							break;
						}
					}
				}
				filein.close();
			}
			if (stats_in_file.size() == 0) {
				SetWindowText(statbox, TEXT("No stats found in file"));
				if (TBTN_SOUND.toggle_state)
					PlaySound(MAKEINTRESOURCE(NOTHING), NULL, SND_ASYNC | SND_RESOURCE);
				return CallNextHookEx(kbhook, code, wParam, lParam);
			}

			string searching = "Searching for:\r\n";
			for (int i = 0; i < (int)stats_in_file.size(); i++)
				searching += '>' + stats_in_file[i] + "\r\n";
			searching += '\0';

			SendMessage(statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());

			POINT pt = getclientcursorpos(wnd);

			if (AUTOROLL.on != 2) {
				/*RECT rc = getwindowrect(wnd);
				SetCursorPos((rc.right - rc.left) / 2 + rc.left, rc.top + 5);*/
				SetCursorPos(0, 0);
			}

			thread roll(OCR_auto_roll, wnd, pt, font16);
			roll.detach();

			type = false;
		}
		else if (key.vkCode == READ.hotkey_key			&&READ.on		&&!pause	&&!type) {
			type = true;

			int x, y;
			BYTE* newbuf = bmp_to_array("capture.bmp", x, y, GetForegroundWindow());
			vector<string> item_stats = ocr(newbuf, x, y, font16);
			process(newbuf, x, y, "capture.bmp");
			delete[] newbuf;
			string all = "";
			if (TBTN_SOUND.toggle_state && item_stats.size() == 0)
				PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
			for (int i = 0; i<(int)item_stats.size(); i++)
				all += '>' + item_stats[i] + "\r\n";
			if (item_stats.size()>0)
				SetWindowText(statbox, str_to_wstr(all).c_str());
			else
				SetWindowText(statbox, TEXT("COULD NOT FIND"));			
			for (UINT i = 0; i < item_stats.size(); i++) {
				if (READ.on == 2) 
					d2type(item_stats[i]);
				cout << "stats: " << item_stats[i] << '\n';
			}
			type = false;
		}
		else if (key.vkCode == TEXTSPAM.hotkey_key		&&READ.on		&&!pause	&&!type) {
			type = true;

			d2type(spam_text_in_file);
			
			type = false;
		}
		else if (key.vkCode == TEST.hotkey_key			&&TEST.on		&&!pause	&&!type) {
			type = true;
		
			/*int x, y;//cursor based ocr
			BYTE* newbuf = bmp_to_array("capture.bmp", x, y, 1);
			POINT pt;
			RECT rc;
			GetCursorPos(&pt);
			HWND foreground = GetForegroundWindow();
			GetClientRect(foreground, &rc);
			MapWindowPoints(foreground, NULL, (LPPOINT)&rc, 2);
			pt.x = pt.x - rc.left;
			pt.y = pt.y - rc.top;

			process(newbuf, x, y, "");
			find_box(newbuf, x, y, pt, "capture.bmp");

			cout << "done" << '\n';*/

			SendMessage(g_hwnd, WM_TEST, NULL, NULL);

			type = false;
			return CallNextHookEx(kbhook, code, wParam, lParam);
			////////////////////////////////////////
			string file_directory = "C:\\Users\\Josh_2.Josh-PC\\Desktop\\New folder (2)\\";

			GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			EncoderParameters encoderParameters;
			ULONG parameterValue;
			CLSID encoderClsid;
			Status stat;
			
			//image/bmp
			//image/jpeg
			//image/gif
			//image/tiff
			//image/png
			
			GetEncoderClsid(L"image/gif", &encoderClsid);

			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = EncoderSaveFlag;
			encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;
			encoderParameters.Parameter[0].Value = &parameterValue;

			Image* multi = new Image(str_to_wstr(file_directory + "nvn0000.gif").c_str());

			// Save the first page (frame).
			parameterValue = EncoderValueMultiFrame;
			stat = multi->Save(L"MultiFrame.gif", &encoderClsid, &encoderParameters);
			if (stat == Ok)
				printf("Page 1 saved successfully.\n");
			else
				cout << "Failure: stat = " << stat << " " << '\n';



			vector<string> files;
			GetFilesInDirectory(files, file_directory);
			for (UINT i = 0; i < files.size(); i++) {
				if (files[i].substr(files[i].rfind('.'), files[i].size()) == ".bmp") {
					string filename = files[i].substr(files[i].rfind('/') + 1, files[i].size());					
					string filenamenotype = files[i].substr(files[i].rfind('/') + 1, files[i].rfind('.') - files[i].rfind('/') - 1);
					Image* image = new Image(str_to_wstr(file_directory + filename).c_str());
					// Get the CLSID of the GIF encoder.

					stat = image->Save(str_to_wstr(file_directory + filenamenotype + ".gif").c_str(), &encoderClsid, NULL);
					if (stat == Ok) {
						parameterValue = EncoderValueFrameDimensionPage;
						Image* temp = new Image(str_to_wstr(file_directory + filenamenotype + ".gif").c_str());
						stat = multi->SaveAdd(temp, &encoderParameters);
						if (stat != Ok)
							cout << "Failure: " << stat << " " << '\n';
					}
					else 
						cout << "Failure: " << filenamenotype << ".gif was not saved successfully. stat = " << stat << " " << '\n';
					delete image;
				}
			}
			parameterValue = EncoderValueFlush;
			stat = multi->SaveAdd(&encoderParameters);
			if (stat == Ok)
				printf("File closed successfully.\n");
			GdiplusShutdown(gdiplusToken);

			_getch();
			cout << "done" << '\n';
			

			SendMessage(g_hwnd, WM_TEST, NULL, NULL);

			type = false;
		}
	
#pragma endregion
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}

