#include "include.h"

#ifdef TYPECHEAT
#include <regex>
HWND testedit;
#endif

using namespace std;
using namespace Gdiplus;

/*

///auto start file setting dont start on reload
///config button
///show or minimize on start -file
///launch pd button
///file comments
///reload button
///OCR
///>matchlines----8
///HOOKS
///directory set for launch


///**features**

//imagefile tray menu / button
//README
//tray icon lock
//force destroy icon
//OR(|), NOT(!), and wildcard(?)/ wildcard sequence (\?), logic for match lines
//multiple items
//itunes gui
//custom window and dialog
//update use function (simplify)
//input stats with edit control
//keeping old values on uncheck [prev]
//roll count
//new ocr
//settings dialog
//start with last active tab
//

///**functionality**

//find box

///*image
//previous image input path
//crop
//grow
//gray
//brightness
//contrast
//invert
//convert to type (gif, png, bmp, tiff, jpg)
//animate gif
//compare two images
//dither
//video to image sequence
//type text in file (use) #typethisline
//inventory to stash vise versa
//gheed finder

//fixes
//timer display fix + escape

*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow) {
	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, (LPCWSTR)"MyMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}

	ifstream filein;
	filein.open("config.txt");
	if (!filein.is_open()) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		return 0;
	}
	filein.close();

	WORD left_vol = 0x4444;
	WORD right_vol = 0x4444;
	waveOutSetVolume(0, MAKELPARAM(right_vol, left_vol));

	HKEY hRegKey = OpenRegKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Tokenhack\\"));
	SetRegVal(hRegKey, TEXT("Last tab"), GetRegVal(hRegKey, TEXT("Last tab")));

	srand((unsigned)time(NULL));

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_HOTKEY_CLASS;
	InitCommonControlsEx(&icex);

	init_itunes_com();
	init_voice_com();
	init_register_class(hInstance);
	init_register_custom_trackbar();

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	g_hwnd = CreateWindow(className, VERSION, WS_OVERLAPPEDWINDOW /*WS_POPUP|*/  | WS_CLIPCHILDREN, 350, 150, windowwidth, functionnameheight*totalonfunctions + 130, NULL, NULL, hInstance, NULL);

	DWORD reg_tab = GetRegVal(hRegKey, TEXT("Last tab"));
	for (UINT i = 0; i < functiontabbuttons.size(); i++) {
		if (reg_tab == i){
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
	UnregisterClass(custom_trackbar_classname, NULL);
	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

	itunes_release();
	voice_release();

	for (UINT i = 0; i < functiontabbuttons.size(); i++) {
		if (functiontabbuttons[i]->toggle_state) {
			SetRegVal(hRegKey, TEXT("Last tab"), i);
		}
	}
	RegCloseKey(hRegKey);

	ClipCursor(NULL);
	
	ShowWindow(g_hwnd, SW_HIDE);
	CloseHandle(Mutex_handle);

	DeleteObject(Font_a);
	DeleteObject(Font_itunes);
	DeleteObject(Font_workout);

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
				Font_itunes = CreateFont(13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Microsoft Sans Serif"));
				Font_workout = CreateFont(80, 0, 0, 0, FW_NORMAL/*FW_BOLD*/, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("SYSTEM"));


				hInst = ((LPCREATESTRUCT)lParam)->hInstance;
				if (!TBTN_SHOWONSTART.toggle_state) {
					ShowWindow(g_hwnd, SW_FORCEMINIMIZE);
					Minimize();
				}

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
					if (var_name == "gamepath") gamepath = val;
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
						weapsonswitchkey = str_to_int(val);
					}
					else if (var_name == "clickdelay") {
						g_clickdelay = str_to_int(val);
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
			
			HWND hdesktop = GetDesktopWindow();
			RECT rd;
			GetClientRect(hdesktop, &rd);
			int xscreen = rd.right /*- 800*/;
			int yscreen = rd.bottom /*- 300*/;

			//CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, TEXT("STATIC"), TEXT("FU"), WS_VISIBLE | WS_POPUP | SS_CENTER, 400, 200, 400, 300, hwnd, (HMENU)NULL, NULL, NULL);

#ifdef TYPECHEAT
			testedit = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL, 0, 0, 100, 100, hwnd, (HMENU)NULL, NULL, NULL);
#endif

			hworkout = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE /*| WS_EX_TOPMOST*/, TEXT("STATIC"), TEXT(""), WS_POPUP | SS_CENTER, 0, 0, xscreen, yscreen, hwnd, NULL, NULL, NULL);

			icursong = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, TEXT("STATIC"), TEXT(""), WS_POPUP | SS_CENTER, 0, 0, 200, 45, hwnd, NULL, NULL, NULL);
			SendMessage(icursong, WM_SETFONT, (WPARAM)Font_itunes, TRUE);

			if (FUNC_TAB_BTN_IMAGE.toggle_state) {
				totalonfunctions = 10;
				functionnameyoffsum = 10 * functionnameheight;

				image_input_static = CreateWindow(TEXT("STATIC"), TEXT("Input Folder:"), WS_CHILD | WS_VISIBLE, 14, 21, 81, 15, g_hwnd, (HMENU)NULL, NULL, NULL);
				image_input_edit = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL, 14, 40, 315, 14, g_hwnd, (HMENU)NULL, NULL, NULL);
				image_output_static = CreateWindow(TEXT("STATIC"), TEXT("Output Folder:"), WS_CHILD | WS_VISIBLE, 14, 61, 91, 15, g_hwnd, (HMENU)NULL, NULL, NULL);
				image_output_edit = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL, 14, 80, 315, 14, g_hwnd, (HMENU)NULL, NULL, NULL);

				HFONT hFont = CreateFont(14, 0, 0, 0, 400, 0, 0, 0, 1, 8, 0, 5, 2, TEXT("ariel"));
				SendMessage(image_input_edit, WM_SETFONT, (WPARAM)hFont, 0);
				SendMessage(image_output_edit, WM_SETFONT, (WPARAM)hFont, 0);
				string previous_image_input_path = "C:\\Users\\Josh_2.Josh-PC\\Desktop\\New folder (2)";
				SendMessage(image_input_edit, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(previous_image_input_path).c_str());
				string default_image_output_path = previous_image_input_path + "\\output";
				SendMessage(image_output_edit, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(default_image_output_path).c_str());

				//if output folder does not exist, create it
				if (PathFileExists(str_to_wstr(previous_image_input_path).c_str())){
					if (!PathFileExists(str_to_wstr(default_image_output_path).c_str())) {
						CreateDirectory(str_to_wstr(default_image_output_path).c_str(), NULL);
					}
				}

				for (UINT i = 0; i < imagetabbuttons.size(); i++) {
					imagetabbuttons[i]->handle = CreateWindow(TEXT("BUTTON"), (LPCWSTR)str_to_wstr(imagetabbuttons[i]->window_text).c_str(),
						(imagetabbuttons[i]->showstate) ? WS_VISIBLE | WS_CHILD | BS_OWNERDRAW : WS_CHILD | BS_OWNERDRAW,
						imagetabbuttons[i]->xpos, imagetabbuttons[i]->ypos,
						imagetabbuttons[i]->width, imagetabbuttons[i]->height,
						hwnd, (HMENU)imagetabbuttons[i]->id, NULL, NULL);
				}
			}
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
					g_hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font)
					SendMessage(GetDlgItem(g_hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < staticcontrols.size(); i++) {
				staticcontrol a = *staticcontrols[i];
				staticcontrols[i]->handle = CreateWindow(TEXT("STATIC"), str_to_wstr(a.window_text).c_str(),
					(a.show_state) ? WS_VISIBLE | WS_CHILD : WS_CHILD,
					a.xpos, a.ypos,
					a.width, a.height,
					g_hwnd, (HMENU)a.id, NULL, NULL);
				if (a.font) 
					SendMessage(GetDlgItem(g_hwnd, a.id), WM_SETFONT, (WPARAM)Font_a, 0);
			}
			for (UINT i = 0; i < custom_trackbars.size(); i++) {
				custom_trackbar* a = custom_trackbars[i];
				a->handle = CreateWindow(custom_trackbar_classname, TEXT(""),
					(a->showstate ? WS_VISIBLE : NULL) | WS_CHILD,
					a->xpos, a->ypos, a->width, a->height,
					g_hwnd, (HMENU)a->id, NULL, NULL);
			}

			SetWindowPos(hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? (!FUNC_TAB_BTN_IMAGE.toggle_state ? windowwidth : windowwidth - 17) : windowwidth + 250/*180*/, functionnameheight*totalonfunctions + (!FUNC_TAB_BTN_IMAGE.toggle_state ? bottomheight : 61) + tokenhackfunctionwindowxposoffset, SWP_NOMOVE);
		
			g_menu = CreatePopupMenu();
			if (TBTN_SOUND.toggle_state)
				for (int freshmeatx = 0; freshmeatx < 31; freshmeatx++)
					AppendMenu(g_menu, MF_STRING, 3003, TEXT("FRESHMEAT"));
			AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
			AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
			AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			break;
		}
		case WM_COMMAND:{
			case EN_CHANGE:{
				for (UINT i = 0; i < editcontrols.size(); i++) {
					if ((HWND)lParam == editcontrols[i]->handle) {
						if (HIWORD(wParam) == EN_CHANGE) {
							if (editcontrols[i]->number_only) {
								wchar_t buffer[256];
								GetWindowText((HWND)lParam, buffer, 256);
								string edit_text = wstr_to_str(buffer);
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
								if (number < editcontrols[i]->min_number) {
									SetWindowText((HWND)lParam, str_to_wstr(int_to_str(editcontrols[i]->min_number)).c_str());
									SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
								}
								else if (number > editcontrols[i]->max_number) {
									SetWindowText((HWND)lParam, str_to_wstr(int_to_str(editcontrols[i]->max_number)).c_str());
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
			for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
				if (tokenhackfunctions[i]->button_id == LOWORD(wParam) && tokenhackfunctions[i]->on) {
					is_function = true;
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(HOTKEYBOX), hwnd, DlgProc);
					if (global_change_hotkey != 0)
						hotkeychange(tokenhackfunctions[i]->button_handle, tokenhackfunctions[i]->button_id, tokenhackfunctions[i]->hotkey_key, global_change_hotkey);
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
			for (UINT i = 0; i < imagetabbuttons.size(); i++) {
				if (imagetabbuttons[i]->id == LOWORD(wParam)) {
					//get paths from edit control
					wchar_t buffer[MAX_PATH];
					GetWindowText(image_input_edit, buffer, MAX_PATH);
					string input_path = wstr_to_str(buffer);

					GetWindowText(image_output_edit, buffer, MAX_PATH);
					string output_path = wstr_to_str(buffer);

					//if output is empty set it to input
					if (output_path.size() == 0 || output_path == input_path)
						output_path = input_path /*+ "\\output"*/;

					//get path with browser
					if (imagetabbuttons[i]->id == IMG_BTN_INPUT.id) {
						string image_path;
						GetFolder(image_path);
						if (image_path.size() == 0 && input_path.size() != 0)
							image_path = input_path;
						input_path = image_path;
						SendMessage(image_input_edit, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(image_path).c_str());
						GetClientRect(imagetabbuttons[i]->handle, &temprect);
						InvalidateRect(imagetabbuttons[i]->handle, &temprect, false);
						//change output folder too
						SendMessage(image_output_edit, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(image_path + "\\output").c_str());
						GetClientRect(IMG_BTN_OUTPUT.handle, &temprect);						
						InvalidateRect(IMG_BTN_OUTPUT.handle, &temprect, false);
						if (PathFileExists(str_to_wstr(image_path).c_str())) {
							if (!PathFileExists(str_to_wstr(image_path + "\\output").c_str()))
								if (CreateDirectory(str_to_wstr(image_path + "\\output").c_str(), NULL))
									output_path = image_path + "\\output";
						}
						break;
					}
					else if (imagetabbuttons[i]->id == IMG_BTN_OUTPUT.id) {
						string image_path;
						GetFolder(image_path);
						if (image_path.size() == 0 && output_path.size() != 0)
							image_path = output_path;
						output_path = image_path;
						SendMessage(image_output_edit, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(image_path).c_str());						
						GetClientRect(imagetabbuttons[i]->handle, &temprect);
						InvalidateRect(imagetabbuttons[i]->handle, &temprect, false);
						break;
					}

					//make sure path exists
					if (input_path.size() == 0) {
						cout << "Error: no path specified" << '\n';
						break;
					} 
					if (PathFileExists(str_to_wstr(input_path).c_str())) {
						if (!PathFileExists(str_to_wstr(output_path).c_str()))
							if (!CreateDirectory(str_to_wstr(output_path).c_str(), NULL))
								cout << GetLastError() << '\n';
					}
					if (!PathFileExists(str_to_wstr(input_path).c_str()) || !PathFileExists(str_to_wstr(output_path).c_str())) {						
						cout << "Error: Invalid path" << '\n';
						break;
					}

					//handle toggling
					for (UINT j = 0; j < imagetabbuttons.size(); j++) {
						imagetabbuttons[j]->toggle_state = 0;
						HWND thandle = imagetabbuttons[j]->handle;
						GetClientRect(thandle, &temprect);
						dis.CtlID = GetDlgCtrlID(thandle);
						dis.CtlType = ODT_BUTTON;
						dis.hDC = GetDC(thandle);
						dis.itemState = ODS_INACTIVE;
						dis.rcItem = temprect;
						SendMessage(g_hwnd, WM_DRAWITEM, NULL, (LPARAM)&dis);
						ReleaseDC(thandle, dis.hDC);
					}
					imagetabbuttons[i]->toggle_state = 1;

					//hide_all_groups(all_groups);
					for (UINT j = 0; j < all_groups.size(); j++) {
						all_groups[j].show_group(false);
					}

					//flip tab rows
					if (imagetabbuttons[i]->row == 1) {
						for (UINT j = 0; j < imagetabbuttons.size(); j++) {
							imagetabbutton a = *imagetabbuttons[j];
							if (a.row == 1) {
								MoveWindow(a.handle, a.xpos, 120, a.width, a.height, false);
								imagetabbuttons[j]->row = 2;
							}
							else if (a.row == 2) {
								MoveWindow(a.handle, a.xpos, 105, a.width, a.height, false);
								imagetabbuttons[j]->row = 1;
							}
							GetClientRect(a.handle, &temprect);
							InvalidateRect(a.handle, &temprect, false);
						}
					}

					//tab button pressed
					if (IMG_TAB_BTN_CROP.id == imagetabbuttons[i]->id) { 
						GROUP_CROP.show_group(true); 
					}
					else if (IMG_TAB_BTN_GROW.id == imagetabbuttons[i]->id) {
						GROUP_GROW.show_group(true);
					}
					else if (IMG_TAB_BTN_GRAY.id == imagetabbuttons[i]->id) {
						GROUP_GRAY.show_group(true);
					}
					else if (IMG_TAB_BTN_BRIGHTNESS.id == imagetabbuttons[i]->id) {
						GROUP_BRIGHTNESS.show_group(true);
						TRACKBAR_BRIGHTNESS_SET.set_focus(true);
					}
					else if (IMG_TAB_BTN_CONTRAST.id == imagetabbuttons[i]->id) {
						GROUP_CONTRAST.show_group(true);
					}
					else if (IMG_TAB_BTN_CONVERT.id == imagetabbuttons[i]->id) {
						GROUP_CONVERT.show_group(true);
					}

					break;
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

					for (UINT j = 0; j < all_groups.size(); j++) {
						all_groups[j].show_group(false);
					}
					BTN_IMAGE_PROCESS.show_state = false;

					EnumChildWindows(g_hwnd, EnumChildProc, DESTROY_WINDOWS);
					turn_all_functions_off();
					tokenhackfunctions.erase(tokenhackfunctions.begin(), tokenhackfunctions.end());
					
					if (functiontabbuttons[i]->id == FUNC_TAB_BTN_ALL.id) {
						tokenhackfunctions = all_tokenhackfunctions;
					}
					if (functiontabbuttons[i]->id == FUNC_TAB_BTN_D2.id) {
						tokenhackfunctions.push_back(&WMC);
						tokenhackfunctions.push_back(&CLOSE);
						tokenhackfunctions.push_back(&GAMBLE);
						tokenhackfunctions.push_back(&GOLD);
						tokenhackfunctions.push_back(&WSGCAST);
						tokenhackfunctions.push_back(&SKILLBUG);
						tokenhackfunctions.push_back(&NOPICKUP);
						tokenhackfunctions.push_back(&LEFTCLICK);
						tokenhackfunctions.push_back(&PAUSE);
						tokenhackfunctions.push_back(&ROLL);
						tokenhackfunctions.push_back(&DROP);
						tokenhackfunctions.push_back(&WSG);
						tokenhackfunctions.push_back(&AUTOROLL);
						tokenhackfunctions.push_back(&READ);
						tokenhackfunctions.push_back(&SKILLBUGSET);
						tokenhackfunctions.push_back(&TEXTSPAM);
					}
					if (functiontabbuttons[i]->id == FUNC_TAB_BTN_ITUNES.id) {
						tokenhackfunctions.push_back(&IPLAY);
						tokenhackfunctions.push_back(&IPREV);
						tokenhackfunctions.push_back(&INEXT);
						tokenhackfunctions.push_back(&IVOLU);
						tokenhackfunctions.push_back(&IVOLD);
					}
					if (functiontabbuttons[i]->id == FUNC_TAB_BTN_IMAGE.id) {
						BTN_IMAGE_PROCESS.show_state = true;
						for (UINT g = 0; g < imagetabbuttons.size(); g++) {
							imagetabbuttons[g]->showstate = true;
							imagetabbuttons[g]->toggle_state = false;

							//fix tab flip
							if (imagetabbuttons[g]->row != imagetabbuttons[g]->true_row) {
								
								imagetabbutton a = *imagetabbuttons[g];
								if (a.row == 1) {
									MoveWindow(a.handle, a.xpos, 120, a.width, a.height, false);
									imagetabbuttons[g]->row = 2;
								}
								else if (a.row == 2) {
									MoveWindow(a.handle, a.xpos, 105, a.width, a.height, false);
									imagetabbuttons[g]->row = 1;
								}
								GetClientRect(a.handle, &temprect);
								InvalidateRect(a.handle, &temprect, false);								
							}
						}
					}
					
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
					if (statboxtabbuttons[i]->id == STAT_TAB_BTN_INPUT.id) {
						
					}
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


					if (RBTN_BRIGHTNESS_FLAT.id == autoradiobuttons[i]->id) {
						TRACKBAR_BRIGHTNESS_SET.set_range(-255, 255);
					}
					else if (RBTN_BRIGHTNESS_HSV.id == autoradiobuttons[i]->id) {
						TRACKBAR_BRIGHTNESS_SET.set_range(0, 100);
					}
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
						for (UINT g = 0; g < imagetabbuttons.size(); g++) {
							imagetabbuttons[g]->showstate = true;
							imagetabbuttons[g]->toggle_state = false;

							//fix tab flip
							if (imagetabbuttons[g]->row != imagetabbuttons[g]->true_row) {

								imagetabbutton a = *imagetabbuttons[g];
								if (a.row == 1) {
									MoveWindow(a.handle, a.xpos, 120, a.width, a.height, false);
									imagetabbuttons[g]->row = 2;
								}
								else if (a.row == 2) {
									MoveWindow(a.handle, a.xpos, 105, a.width, a.height, false);
									imagetabbuttons[g]->row = 1;
								}
								GetClientRect(a.handle, &temprect);
								InvalidateRect(a.handle, &temprect, false);
							}
						}
						for (UINT j = 0; j < all_groups.size(); j++) {
							all_groups[j].show_group(false);
						}
						stats_in_file.erase(stats_in_file.begin(), stats_in_file.end());
						spam_text_in_file.erase(spam_text_in_file.begin(), spam_text_in_file.end());
						workout.erase(workout.begin(), workout.end());
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

							for (int i = 0; i < (int)text.size(); i++) {
								line = text[i];
								line = line.substr(0, line.find('*'));
								line.erase(remove(line.begin(), line.end(), '\t'), line.end());
								string val = line.substr(line.find(':') + 1, line.size());
								line.erase(remove(line.begin(), line.end(), ' '), line.end());
								transform(line.begin(), line.end(), line.begin(), ::tolower);
								string var_name = line.substr(0, line.find(':'));
								if (var_name == "gamepath")
									text[i] = "gamepath\t\t:\t" + gamepath;
							}
							ofstream fileout;
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
					else if (BTN_IMAGE_PROCESS.id ==  normalbuttons[i]->id && FUNC_TAB_BTN_IMAGE.toggle_state) {
						//get paths from edit control
						string input_path = getwindowtext(image_input_edit);
						string output_path = getwindowtext(image_output_edit);
						//if output is empty set it to input
						if (output_path.size() == 0 || output_path == input_path)
							output_path = input_path /*+ "\\output"*/;

						//make sure path exists
						if (input_path.size() == 0) {
							cout << "Error: no path specified" << '\n';
							break;
						}
						if (PathFileExists(str_to_wstr(input_path).c_str())) {
							if (!PathFileExists(str_to_wstr(output_path).c_str()))
								if (!CreateDirectory(str_to_wstr(output_path).c_str(), NULL))
									cout << GetLastError() << '\n';
						}
						if (!PathFileExists(str_to_wstr(input_path).c_str()) || !PathFileExists(str_to_wstr(output_path).c_str())) {
							cout << "Error: Invalid path" << '\n';
							break;
						}

						vector<string> files;
						GetFilesInDirectory(files, input_path, "bmp");


						HWND progress_bar = CreateWindow(PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE, 107, 61, 223, 15, g_hwnd, (HMENU)NULL, NULL, NULL);
						SendMessage(progress_bar, PBM_SETRANGE, 0, MAKELPARAM(0, files.size()));
						SendMessage(progress_bar, PBM_SETSTEP, (WPARAM)1, 0);
						SendMessage(g_hwnd, WM_SETCURSOR, (WPARAM)g_hwnd, (LPARAM)CURSOR_LOADING);
						UnhookWindowsHookEx(mhook);
						EnableWindow(BTN_IMAGE_PROCESS.handle, false);

						//load base images
						if (process_images.size() == 0) {
							for (UINT i = 0; i < files.size(); i++) {
								process_image* pi = new process_image();
								pi->load_bmp(files[i]);
								process_images.push_back(pi);
							}
						}

						if (IMG_TAB_BTN_ANIMATE.toggle_state) {
							animate_gif(input_path, output_path, "bananza.gif");
						}
						else {
							//apply algorithm and save
							for (UINT i = 0; i < process_images.size(); i++) {
								process_image* pi = process_images[i];

								//rainbows
								/*double center = 170;
								double width = 50;
								double phase = 20;
								int iter = 12;
								UINT len = int_to_str(iter).size();
								double frequency = M_PI * 2 / iter;
								for (int i = 0, count = 0; i < iter; ++i, count++) {
								int r = (int)(sin(frequency * i + 0 + phase) * width + center);
								int g = (int)(sin(frequency * i + 2 + phase) * width + center);
								int b = (int)(sin(frequency * i + 4 + phase) * width + center);
								pi->algs = int_to_str(count);
								while (pi->algs.size() < len)
								pi->algs = "0" + pi->algs;
								BYTE* temp = flatcoloradjust(pi->buffer, pi->x, pi->y, r, g, b);
								string newoutput_path = output_path + '\\' + pi->algs + pi->filename;
								array_to_bmp(newoutput_path, temp, pi->x, pi->y);
								delete[] temp;
								}
								bmp_to_gif(output_path);
								animate_gif(output_path, output_path + "\\guns n rims.gif");*/

								if (IMG_TAB_BTN_CROP.toggle_state) {
									pi->algs += "(crop)";

									int left = str_to_int(getwindowtext(EDIT_CROP_LEFT.handle));
									int right = str_to_int(getwindowtext(EDIT_CROP_RIGHT.handle));
									int top = str_to_int(getwindowtext(EDIT_CROP_TOP.handle));
									int bottom = str_to_int(getwindowtext(EDIT_CROP_BOTTOM.handle));

									pi->buffer = cropimage(pi->buffer, pi->x, pi->y, left, right, top, bottom);
								}
								else if (IMG_TAB_BTN_GROW.toggle_state) {
									pi->algs += "(grow)";

									int left = str_to_int(getwindowtext(EDIT_GROW_LEFT.handle));
									int right = str_to_int(getwindowtext(EDIT_GROW_RIGHT.handle));
									int top = str_to_int(getwindowtext(EDIT_GROW_TOP.handle));
									int bottom = str_to_int(getwindowtext(EDIT_GROW_BOTTOM.handle));

									pi->buffer = growimage(pi->buffer, pi->x, pi->y, left, right, top, bottom, RGB(255, 255, 0));
								}
								else if (IMG_TAB_BTN_GRAY.toggle_state) {
									if (RBTN_GRAY_AVGERAGE.toggle_state) {
										pi->algs += "(gray_avg)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_AVGERAGE);
									}
									else if (RBTN_GRAY_LUMA.toggle_state) {
										pi->algs += "(gray_luma)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_LUMINOSITY);
									}
									else if (RBTN_GRAY_DESATURATE.toggle_state) {
										pi->algs += "(gray_desat)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_DESATURATION);
									}
									else if (RBTN_GRAY_COLOR_CHANNEL_RED.toggle_state) {
										pi->algs += "(gray_channel_red)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_COLOR_CHANNEL_RED);
									}
									else if (RBTN_GRAY_COLOR_CHANNEL_GREEN.toggle_state) {
										pi->algs += "(gray_channel_green)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_COLOR_CHANNEL_GREEN);
									}
									else if (RBTN_GRAY_COLOR_CHANNEL_BLUE.toggle_state) {
										pi->algs += "(gray_channel_blue)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_COLOR_CHANNEL_BLUE);
									}
									else if (RBTN_GRAY_DECOMP_MIN.toggle_state) {
										pi->algs += "(gray_decomp_min)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_DECOMPOSITION_MIN);
									}
									else if (RBTN_GRAY_DECOMP_MAX.toggle_state) {
										pi->algs += "(gray_decomp_max)";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCALE_DECOMPOSITION_MAX);
									}

									//color count
									int color_count = str_to_int(getwindowtext(EDIT_GRAY_COLORS.handle));
									if (!(color_count >= 256)) {
										pi->algs += "(gray_colors[" + int_to_str(color_count) + "])";
										grayscale(pi->buffer, pi->x, pi->y, GRAYSCLAE_COLOR_COUNT, color_count);
									}
								}
								else if (IMG_TAB_BTN_BRIGHTNESS.toggle_state) {
									if (RBTN_BRIGHTNESS_FLAT.toggle_state) {
										pi->algs += "(bright_flat[" + int_to_str(TRACKBAR_BRIGHTNESS_SET.current_val) + "])";
										brightness(pi->buffer, pi->x, pi->y, BRIGHTNESS_FLAT, TRACKBAR_BRIGHTNESS_SET.current_val);
									}
									else if (RBTN_BRIGHTNESS_HSV.toggle_state) {
										pi->algs += "(bright_HSV[" + int_to_str(TRACKBAR_BRIGHTNESS_SET.current_val) + "])";
										brightness(pi->buffer, pi->x, pi->y, BRIGHTNESS_HSV, TRACKBAR_BRIGHTNESS_SET.current_val);
									}
								}

								else if (IMG_TAB_BTN_CONTRAST.toggle_state) {

								}
								else if (IMG_TAB_BTN_CONVERT.toggle_state) {
									bmp_to_gif(input_path, pi->filename, output_path);
									SendMessage(progress_bar, PBM_STEPIT, 0, 0);
									continue;
								}

								string newoutput_path = output_path + '\\' + pi->algs + pi->filename;
								array_to_bmp(newoutput_path, pi->buffer, pi->x, pi->y);
								SendMessage(progress_bar, PBM_STEPIT, 0, 0);
							}
						}

						if (GetForegroundWindow() == g_hwnd)
							mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);
						DestroyWindow(progress_bar);
						EnableWindow(BTN_IMAGE_PROCESS.handle, true);

						//if (!image_display_box)
						//image_display_box = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IMAGEDISPLAYBOX), hwnd, imageDlgProc);
						if (files.size() > 0) {
							int x, y;
							LONG s;
							LoadBMP(&x, &y, &s, str_to_wstr(files[0]).c_str());
							imageDlgProc(image_display_box, (WM_USER + 1), reinterpret_cast<WPARAM>(str_to_wstr(files[0]).c_str()), (LPARAM)MAKELONG(x, y));
						}
						//delete process stack
						process_images.clear();

						cout << "done" << '\n';
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

			HBRUSH hb_background = CreateSolidBrush(RGB(10, 10, 10));

			//paint background
			FillRect(hDC, &ps.rcPaint, hb_background);

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

			if (FUNC_TAB_BTN_IMAGE.toggle_state) {
				//border of image button tab
				SetDCPenColor(hDC, RGB(0, 118, 255));
				Rectangle(hDC, 14, 134, 329, 180);

				//borders for file directory edit controls
				SetDCPenColor(hDC, RGB(100, 100, 100));
				RECT fileb = getwindowrect(image_input_edit);
				Rectangle(hDC, fileb.left - 1, fileb.top - 1, fileb.left + (fileb.right - fileb.left) + 1, fileb.top + (fileb.bottom - fileb.top) + 1);
				
				fileb = getwindowrect(image_output_edit);
				Rectangle(hDC, fileb.left - 1, fileb.top - 1, fileb.left + (fileb.right - fileb.left) + 1, fileb.top + (fileb.bottom - fileb.top) + 1);
			}

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
			COLORREF blue = RGB(0, 118, 255);
			COLORREF black = RGB(10, 10, 10);
			COLORREF red = RGB(254, 33, 61);
			//COLORREF purple = RGB(148, 0, 211);
			static HBRUSH hbblack = CreateSolidBrush(black);
			static HBRUSH hbred = CreateSolidBrush(red);
			static HBRUSH hbblue = CreateSolidBrush(blue);
			HDC hdcStatic = (HDC)wParam;
			if ((HWND)lParam == icursong) {
				SetWindowText(icursong, str_to_wstr((Igetcursong() + "\n" + Igetcurartist())).c_str());
				//SetTextColor(hdcStatic, RGB(148,0,211));
				SetBkColor(hdcStatic, RGB(254, 33, 61));
				return (INT_PTR)hbred;
			}
			if ((HWND)lParam == statbox) {
				RECT rec;
				HDC whydoineedthis = GetDC(statbox);
				GetClientRect(statbox, &rec);
				SetTextColor(hdcStatic, blue);
				SetBkColor(hdcStatic, black);
				FrameRect(whydoineedthis, &rec, hbblue);
				ReleaseDC(statbox, whydoineedthis);
				return (INT_PTR)hbblack;
			}
			if ((HWND)lParam == hworkout) {
				SIZE size;
				RECT rcItem;
				wchar_t ttext[256];
				HDC hDC = (HDC)wParam;
				GetClientRect(hworkout, &rcItem);				
				
				GetWindowText(hworkout, ttext, 256);
				string text = wstr_to_str(ttext);

				SelectObject(hDC, Font_workout);

				GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
				SetBkColor(hdcStatic, RGB(254, 33, 61));	
				//SetTextColor(hdcStatic, RGB(148, 0, 211));		
				ExtTextOut(hDC,	((rcItem.right - rcItem.left) - size.cx) / 2, ((rcItem.bottom - rcItem.top) - size.cy) / 2,	ETO_OPAQUE | ETO_CLIPPED, &rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
				
				ReleaseDC((HWND)lParam, hDC);
				return (INT_PTR)hbred;
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
				//cout << count << " ";
				/*
				//255 0 0
				//255 127 0
				//255 255 0
				//127 255 0
				//0 255 0
				//0 255 127
				//0 255 255
				//0 127 255
				//0 0 255
				//127 0 255
				//255 0 127
				CONSOLE_ON();
				BYTE r = 255, g = 0, b = 127;
				int colorspeed = 127;
				int count = 0;
				for (;;) {
					count++;
					if (r == 255 && g<255 && b == 0)
						(g + colorspeed>255) ? g = 255 : g += colorspeed;
					else if (g == 255 && r <= 255 && r != 0)
						(r - colorspeed < 0) ? r = 0 : r -= colorspeed;
					else if (r == 0 && b<255)
						(b + colorspeed>255) ? b = 255 : b += colorspeed;
					else if (b == 255 && g <= 255 && g != 0)
						(g - colorspeed < 0) ? g = 0 : g -= colorspeed;
					else if (g == 0 && r<255 && b == 255)
						(r + colorspeed>255) ? r = 255 : r += colorspeed;
					else if (r == 255 && g == 0 && b <= 255 && b != 0)
						(b - colorspeed < 0) ? b = 0 : b -= colorspeed;
					(r == 1) ? r = 0 : (r == 254) ? r = 255 : (r == 128) ? r = 127 : r = r;
					(g == 1) ? g = 0 : (g == 254) ? g = 255 : (g == 128) ? g = 127 : g = g;
					(b == 1) ? b = 0 : (b == 254) ? b = 255 : (b == 128) ? b = 127 : b = b;
					cout << "if (rainbowc==" << count << "){red1=" << (int)r << ";gre1=" << (int)g << ";blu1=" << (int)b << ";}" << '\n';
					if (r == 255 && g == 0 && b == 127)
						_getch();
				}				
				*/
			}
			else
				red1 = 148, gre1 = 0, blu1 = 211;
			//bool is_function = false;
			for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
				if ((HWND)lParam == tokenhackfunctions[i]->function_handle) {
					//is_function = true;
					SetTextColor(hdcStatic, RGB(red1, gre1, blu1));
					SetBkColor(hdcStatic, black);
					return (INT_PTR)hbblack;
				}
			}
			SetTextColor(hdcStatic, RGB(100, 100, 100));
			SetBkColor(hdcStatic, black);
			return (INT_PTR)hbred;
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
			COLORREF black_back = RGB(10, 10, 10);
			COLORREF tokenhack_function_button = RGB(254, 33, 61);
			COLORREF normal_button = RGB(255, 206, 61);
			COLORREF toggle_on = RGB(0, 255, 0);
			COLORREF toggle_off = RGB(255, 0, 0);
			COLORREF statbox_tab = RGB(0, 118, 255);
			COLORREF image_button = RGB(0, 118, 255);
			static HBRUSH hb_highlight = CreateSolidBrush(RGB(100, 100, 100));
			static HBRUSH hb_tokenhack_function_button = CreateSolidBrush(tokenhack_function_button);
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
					SetTextColor(pdis->hDC, tokenhackfunctions[i]->on ? tokenhack_function_button : black_back);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : black_back);
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
					SetTextColor(pdis->hDC, tokenhackfunctions[i]->on ? tokenhack_function_button : pdis->itemState & ODS_SELECTED ? highlight : black_back);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : black_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);

					//DrawEdge(pdis->hDC, &pdis->rcItem, (pdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT);
					FrameRect(pdis->hDC, &pdis->rcItem, hb_tokenhack_function_button);
					return TRUE;
				}
			}
			//draw image tab buttons
			for (UINT i = 0; i < imagetabbuttons.size(); i++) {
				imagetabbutton a = *imagetabbuttons[i];
				if (pdis->CtlID == a.id && (a.handle != IMG_BTN_INPUT.handle && a.handle != IMG_BTN_OUTPUT.handle)) {
					TCHAR ttext[256];
					//GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					GetWindowText(a.handle, ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL/*FW_BOLD*/, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("SYSTEM") /*TEXT("MS PMINCHO")*//*str_to_wstr(fonts[currfont]).c_str()*/);
					SelectObject(pdis->hDC, hFont);
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, a.toggle_state ? black_back : (pdis->itemState & ODS_SELECTED) ? black_back : image_button);
					SetBkColor(pdis->hDC, a.toggle_state ? image_button : (pdis->itemState & ODS_SELECTED) ? image_button : black_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, a.toggle_state ? hb_image_button : pdis->itemState & ODS_SELECTED ? hb_black_back : hb_image_button);

					DeleteObject(hFont);
					return TRUE;
				}
				//draw browser btns seperately
				else if (pdis->CtlID == a.id && (a.handle == IMG_BTN_INPUT.handle || a.handle == IMG_BTN_OUTPUT.handle)) {
					TCHAR ttext[256];
					GetWindowText(GetDlgItem(hwnd, pdis->CtlID), ttext, 256);
					string text = (wstr_to_str((wstring)ttext));
					GetTextExtentPoint32(pdis->hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetTextColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? black_back : highlight);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : black_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, hb_highlight);
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
					SetTextColor(pdis->hDC, a.toggle_state ? black_back : (pdis->itemState & ODS_SELECTED) ? black_back : highlight);
					SetBkColor(pdis->hDC, a.toggle_state ? highlight : (pdis->itemState & ODS_SELECTED) ? highlight : black_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, a.toggle_state ? hb_highlight : pdis->itemState & ODS_SELECTED ? hb_black_back : hb_highlight);

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
					SetTextColor(pdis->hDC, a.toggle_state ? black_back : pdis->itemState & ODS_SELECTED ? black_back : statbox_tab);
					SetBkColor(pdis->hDC, a.toggle_state ? statbox_tab : pdis->itemState & ODS_SELECTED ? statbox_tab : black_back);
					ExtTextOut(pdis->hDC,
						((pdis->rcItem.right - pdis->rcItem.left) - size.cx) / 2,
						((pdis->rcItem.bottom - pdis->rcItem.top) - size.cy) / 2,
						ETO_OPAQUE | ETO_CLIPPED, &pdis->rcItem, str_to_wstr(text).c_str(), text.length(), NULL);
					FrameRect(pdis->hDC, &pdis->rcItem, a.toggle_state ? hb_statbox_tab : pdis->itemState & ODS_SELECTED ? hb_black_back : hb_statbox_tab);
										
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
					SetTextColor(pdis->hDC, a.toggle_state ? black_back : pdis->itemState & ODS_SELECTED ? black_back : a.color);
					SetBkColor(pdis->hDC, a.toggle_state ? a.color : pdis->itemState & ODS_SELECTED ? highlight : black_back);
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
					SetTextColor(pdis->hDC, a.toggle_state ? a.color : pdis->itemState & ODS_SELECTED ? highlight : black_back);
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : black_back);
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
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : black_back);
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
					SetBkColor(pdis->hDC, pdis->itemState & ODS_SELECTED ? highlight : black_back);
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
		case WM_PLAYPAUSE:{
			Iplaypause();
			ShowWindow(icursong, SW_SHOW);
			SendMessage(g_hwnd, WM_CTLCOLORSTATIC, NULL, (LPARAM)icursong);
			timer1 = SetTimer(NULL, 0, 5000, timerProc);
			break;
		}
		case WM_NEXT:{
			Inext();
			ShowWindow(icursong, SW_SHOW);
			SendMessage(g_hwnd, WM_CTLCOLORSTATIC, NULL, (LPARAM)icursong);
			timer1 = SetTimer(NULL, 0, 5000, timerProc);
			break;
		}
		case WM_PREV:{
			Iprev();
			ShowWindow(icursong, SW_SHOW);
			SendMessage(g_hwnd, WM_CTLCOLORSTATIC, NULL, (LPARAM)icursong);
			timer1 = SetTimer(NULL, 0, 5000, timerProc);
			break;
		}
		case WM_VOLU:{
			Ivolu();
			break;
		}
		case WM_VOLD:{
			Ivold();
			break;
		}
		case WM_VOICE:{
			//if (TBTN_SOUND.toggle_state)
		//	Voice->Speak((wchar_t *)wParam, (DWORD)lParam, 0);

			std::wstring name(L"Let Token know");
			const wchar_t* szName = name.c_str();
			Voice->Speak(szName, (DWORD)1, 0);

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
#ifdef TYPECHEAT
			string source;
			int len = GetWindowTextLength(testedit);
			wchar_t* text = new wchar_t[len];
			GetWindowText(testedit, text, len + 1);
			source = wstr_to_str(text);
			
			string result;
			regex_replace(back_inserter(result), source.begin(), source.end(), regex("<.+?>"), " ");
			source = result;

			vector<string> words;
			UINT pos = 0;
			string token;
			string delimiter = "  ";
			while ((pos = source.find(delimiter)) != std::string::npos) {
				token = source.substr(0, pos);
				if ((int)token.size() >= 1)
					words.push_back(token);
				source.erase(0, pos + delimiter.length());
			}
			if ((int)source.size() > 0)
				words.push_back(source);
			
#define TIME 850
#if TIME > 0
#define DELAY this_thread::sleep_for(chrono::microseconds(TIME));
#else 
#define DELAY
#endif
			HWND wnd = GetForegroundWindow();
			for (UINT i = 0; i < words.size(); i++) {
				for (UINT j = 0; j < words[i].size(); j++) {
					DELAY
					PostMessage(wnd, WM_CHAR, words[i][j], 0x1 | (LPARAM)((OemKeyScan(words[i][j])) << 16));					
				}				
				DELAY
				keybd_event(VK_SPACE, 0x39, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(VK_SPACE, 0x39, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
#endif
			/*HWND target_wnd = GetForegroundWindow();
			wchar_t item_text[700];

			HANDLE mainToken;
			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &mainToken)) {
				if (GetLastError() == ERROR_NO_TOKEN) {
					if (!ImpersonateSelf(SecurityImpersonation)) {
						cout << "error 1\n";
						return 1;
					}

					if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &mainToken)) {
						cout << GetLastError();
						return 1;
					}
				}
				else {
					cout << "error 2\n";
					return 1;
				}
			}

			if (!SetPrivilege(mainToken, SE_DEBUG_NAME, true)) {
				CloseHandle(mainToken);
				cout << "Couldn't set DEBUG MODE: " << error_code_to_text(GetLastError()) << endl;
				return 1;
			};

			DWORD processId;
			GetWindowThreadProcessId(target_wnd, &processId);
			HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId);
			if (process == NULL)
				cout << "failed to open process\n";
			else {
				if (!ReadProcessMemory(process, (LPVOID)(0x6F8E0000 + 0x62C70), &item_text, 700, NULL))
					cout << "failed to read memory\n";
			}

			CloseHandle(process);

			wcout << item_text[700];

			cout << "Done\n";*/
			
			//HWND wnd = GetForegroundWindow();
			//static HHOOK callwndhook;
			//static HHOOK callwndrethook;
			//static HHOOK getmessagehook;

			//RECT rcstartpos = getwindowrect(wnd);
			//RECT rcdesktop = getclientrect(GetDesktopWindow());
			//MoveWindow(wnd, rcdesktop.right, rcdesktop.bottom, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, true);
			//
			////SetActiveWindow(NULL);
			///*HMODULE dll_handle = LoadLibrary(DLL_NAME);
			//BOOL(*InjectDll)(wchar_t*, HWND, HHOOK*, HHOOK*, HHOOK*);
			//InjectDll = (BOOL(*)(wchar_t*, HWND, HHOOK*, HHOOK*, HHOOK*))GetProcAddress(dll_handle, "InjectDll");*/
			//
			//if (!InjectDll(DLL_NAME, wnd, &callwndhook, &callwndrethook, &getmessagehook)) {
			//	MessageBox(NULL, L"Dll injection failed", L"Error", MB_OK);
			//	PostQuitMessage(0);
			//}

			////MoveWindow(wnd, rcstartpos.left, rcstartpos.top, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, true);

			break;
		}
		//case WM_DEVICECHANGE: {
		//	 //
		//	 // This is the actual message from the interface via Windows messaging.
		//	 // This code includes some additional decoding for this particular device type
		//	 // and some common validation checks.
		//	 //
		//	 // Note that not all devices utilize these optional parameters in the same
		//	 // way. Refer to the extended information for your particular device type 
		//	 // specified by your GUID.
		//	 //
		//	 PDEV_BROADCAST_DEVICEINTERFACE b = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
		//	 TCHAR strBuff[256];
		//	 static int msgCount = 0;
		//	 // Output some messages to the window.
		//	 switch (wParam) {
		//		 case DBT_DEVICEARRIVAL:
		//			 msgCount++;
		//			 StringCchPrintf(
		//				 strBuff, 256,
		//				 TEXT("Message %d: DBT_DEVICEARRIVAL\n"), msgCount);
		//			 break;
		//		 case DBT_DEVICEREMOVECOMPLETE:
		//			 msgCount++;
		//			 StringCchPrintf(
		//				 strBuff, 256,
		//				 TEXT("Message %d: DBT_DEVICEREMOVECOMPLETE\n"), msgCount);
		//			 break;
		//		 case DBT_DEVNODES_CHANGED:
		//			 msgCount++;
		//			 StringCchPrintf(
		//				 strBuff, 256,
		//				 TEXT("Message %d: DBT_DEVNODES_CHANGED\n"), msgCount);
		//			 break;
		//		 default:
		//			 msgCount++;
		//			 StringCchPrintf(
		//				 strBuff, 256,
		//				 TEXT("Message %d: WM_DEVICECHANGE message received, value %d unhandled.\n"),
		//				 msgCount, wParam);
		//			 break;
		//	 }
		////	 OutputMessage(hEditWnd, wParam, (LPARAM)strBuff);
		//	 wcout << strBuff << '\n';
		//	 break;
		// }
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
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
			return FALSE;
		}
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case BTN_OK:{
					global_change_hotkey = (WORD)SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_GETHOTKEY, 0, 0);
					if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 1)
						global_change_hotkey += 768;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 4)
						global_change_hotkey -= 768;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 3)
						global_change_hotkey += 768;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 6)
						global_change_hotkey -= 768;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 6)
						global_change_hotkey -= 768;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 12)
						global_change_hotkey -= 256 * 11;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 14)
						global_change_hotkey -= 256 * 11;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 9)
						global_change_hotkey -= 256 * 5;
					else if ((global_change_hotkey - (int)LOBYTE(global_change_hotkey)) / 256 == 11)
						global_change_hotkey -= 256 * 5;
					if (global_change_hotkey > ((256 * 7) + 255))
						global_change_hotkey -= 256 * 8;

					bool newkey = true;
					for (UINT i = 0; i < hotkeys.size(); i++) {
						if (global_change_hotkey == hotkeys[i]) {
							newkey = false;
							cout << "Key already being used as hotkey" << '\n';
							if (TBTN_SOUND.toggle_state)
								PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
							global_change_hotkey = 0;
							SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_SETHOTKEY, 0, 0);
							SetFocus(GetDlgItem(hwnd, KEYCONTROL));
							break;
						}
					}
					if (newkey)
						EndDialog(hwnd, KEYCONTROL);
					break;
				}
			}
			break;
		}
		case WM_CLOSE:		
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
BOOL CALLBACK imageDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	const int WM_LOADIMAGE = (WM_USER + 1);

	static int x = 0, y = 0;
	static string file = "";

	if (!image_display_box)
		return FALSE;

	switch (message) {
		case WM_INITDIALOG:{
			return true;
		}
		/*case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				default:
					return FALSE;
			}
			break;
		}*/
		case WM_PAINT:{
			
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);

			if (file.size() > 0) 
				LoadAndBlitBitmap(str_to_wstr(file).c_str(), hDC);

			EndPaint(hwnd, &ps);
			
			break;
		}
		case WM_LOADIMAGE:{
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			HDC hDC = GetDC(hwnd);
			wchar_t* tf = (wchar_t*)wParam;
			file = wstr_to_str(tf);
			
			MoveWindow(hwnd, 0, 0, x + 16, y + 38, true);
			//cout << getclientrect(hwnd).right << " " << getclientrect(hwnd).bottom << '\n';
			LoadAndBlitBitmap(str_to_wstr(file).c_str(), hDC);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			image_display_box = NULL;
			EndDialog(hwnd, 0);
		}
		default:{
			return FALSE;
		}
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
	if (id == timer1) {
		KillTimer(0, timer1);
		//AnimateWindow(icursong, 0, AW_HIDE | AW_BLEND);
		ShowWindow(icursong, SW_HIDE);
		cursongvisstate = false;
	}
	if (id == timer2) {
		if (LEFTCLICK.on == 2) {
			if (click_toggle) {
				post_left_click();
			}
			else
				KillTimer(0, timer2);
		}
		else if (LEFTCLICK.on == 3) {
			if (click_toggle) {
				post_left_click(g_lclickpt, 0, g_lclicktarget);
			}
			else
				KillTimer(0, timer2);
		}
	}
	if (id == timer3) {
		static ULONG total_time = 0;
		static double curtime;
		if (timer_on) {
			total_time++;
			curtime = ((double)clock() / 1000);
			cout << "timer: "
				<< total_time
				<< (total_time == 1 ? " second " : " seconds elapsed ")
				<< "(real: "
				<< curtime - starttime
				<< " ["
				<< (curtime - starttime) - total_time
				<< "]) "
				<< TIMER.on - (total_time % TIMER.on)
				<< '\n';
			
			/*if ((total_time % TIMER.on) - 2 == 0) {
				
			}*/
			if (total_time % TIMER.on == 0) {
				if (TBTN_SOUND.toggle_state) {
					PlaySound(MAKEINTRESOURCE(TIMERSOUND), NULL, SND_RESOURCE | SND_ASYNC);
				}
				/*if (TBTN_SOUND.toggle_state) {
					if (ifstream("res\\workout\\" + workout[curworkout]))
						PlaySound(str_to_wstr("res\\workout\\" + workout[curworkout]).c_str(), NULL, SND_FILENAME | SND_ASYNC);
					else
						Voice->Speak(str_to_wstr(workout[curworkout]).c_str(), SPF_ASYNC | SPF_PURGEBEFORESPEAK, 0);
				}*/
				cout << (workout.size()>  0 ? workout[curworkout] : "DING") << '\n';
				curworkout++;
				if (curworkout > workout.size() - 1) {
					curworkout = 0;
					totalsets++;
				}
			}
			SetWindowText(hworkout, str_to_wstr("sets completed: " + int_to_str(totalsets) + '\n' + ((workout.size()>0) ? workout[curworkout] + "\r\n " : "") + int_to_str((TIMER.on - (total_time % TIMER.on)))).c_str());		
		}
		else {
			total_time = 0;
			totalsets = 0;
			KillTimer(0, timer3);
		}
	}
	if (id == timer4) {		
		if (click_toggle2) {
			post_right_click();
		}
		else
			KillTimer(0, timer4);
	}
}
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);

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
				RedrawWindow(tokenhackfunctions[i]->function_handle, &getclientrect(tokenhackfunctions[i]->function_handle), NULL, RDW_INVALIDATE);
			}
		}
		//cout << mouse.pt.x << "	" << mouse.pt.y << '\n';
	#pragma region current song hover [off]
		/*static bool tempstate = false;
		RECT cursongrect;
		GetClientRect(icursong, &cursongrect);
		if (wParam == WM_LBUTTONDOWN && cursongvisstate && mouse.pt.x <= cursongrect.right && mouse.pt.y <= cursongrect.bottom) {
			ShowWindow(icursong, SW_HIDE);
			cursongvisstate = false;
		}
		if (!trap) {
			if (mouse.pt.x <= 0 && mouse.pt.y <= 0 && !cursongvisstate) {
				ShowWindow(icursong, SW_SHOW);
				cursongvisstate = true;
				tempstate = true;
			}
			if (!(mouse.pt.x <= cursongrect.right && mouse.pt.y <= cursongrect.bottom) && cursongvisstate && tempstate) {
				timer1 = SetTimer(NULL, 0, 5000, timerProc);
				tempstate = false;
			}
		}
		else {
			if (mouse.pt.x <= r.left && mouse.pt.y <= r.top && !cursongvisstate) {
				ShowWindow(icursong, SW_SHOW);
				cursongvisstate = true;
				tempstate = true;
			}
			if (!(mouse.pt.x <= r.left + cursongrect.right && mouse.pt.y <= r.top + cursongrect.bottom) && cursongvisstate && tempstate) {
				timer1 = SetTimer(NULL, 0, 5000, timerProc);
				tempstate = false;
			}
		}*/
#pragma endregion
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
		DWORD lp = 1;
		lp += key.scanCode << 16;
		lp += key.flags << 24;
		GetKeyNameText(lp, (LPTSTR)name, 255);
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
		static bool hotkeyset = false;
		static bool skillbugsetgetinputon = false;
		static int skillbugsetcount = 0;
		static WORD firstkey = NULL;
		static WORD secondkey = NULL;
		static BYTE firstscancode = NULL;
		static BYTE secondscancode = NULL;
		if (skillbugsetgetinputon) {
			if (skillbugsetcount) {
				if (key.vkCode != SKILLBUGSET.hotkey_key) {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
					secondkey = (WORD)key.vkCode;
					secondscancode = (BYTE)key.scanCode + 128;
				}
				else {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
					firstkey = NULL;
					secondkey = NULL;
					firstscancode = NULL;
					secondscancode = NULL;
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
					firstscancode = (BYTE)key.scanCode + 128;
				}
				else {
					if (TBTN_SOUND.toggle_state)
						PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
					firstkey = NULL;
					secondkey = NULL;
					firstscancode = NULL;
					secondscancode = NULL;
					skillbugsetcount = 0;
					skillbugsetgetinputon = false;
				}
			}
			return CallNextHookEx(kbhook, code, wParam, lParam);
		}
#pragma endregion
	#pragma region functions

		static int barbwwcountpi = 0;

#define check_d2foreground() {\
			string s = getwindowtext(GetForegroundWindow());\
			if (getwindowtext(GetForegroundWindow()).find("Diablo II") == string::npos) {\
				type = false;\
				return CallNextHookEx(kbhook, code, wParam, lParam); \
						}}

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

			check_d2foreground();

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

			check_d2foreground();

			HWND wnd = GetForegroundWindow();
			POINT origpt = getclientcursorpos(wnd);
			cout << origpt.x << ", " << origpt.y << '\n';

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
			check_d2foreground();

			switch_weapons();
			post_right_click();
		}
		else if (key.vkCode == SKILLBUG.hotkey_key		&&SKILLBUG.on	&&!pause	&&!type) {
			type = true;

			check_d2foreground();

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

			check_d2foreground();

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

			check_d2foreground();

			d2type("/nopickup");
			type = false;
		}
		else if (key.vkCode == LEFTCLICK.hotkey_key		&&LEFTCLICK.on	&&!pause	&&!type) {
			if (LEFTCLICK.on == 1)
				post_left_click();
			if (LEFTCLICK.on == 2 || LEFTCLICK.on == 3) {
				g_lclicktarget = GetForegroundWindow();
				g_lclickpt = getclientcursorpos(g_lclicktarget);
				if (!click_toggle) {
					click_toggle = 1;

					timer2 = SetTimer(NULL, 0, rand() % 21 - 10 + g_clickdelay, timerProc);
				}
				else {
					click_toggle = 0;
				}
			}
		}
		else if (key.vkCode == RIGHTCLICK.hotkey_key	&&RIGHTCLICK.on	&&!pause	&&!type) {
			if (RIGHTCLICK.on == 1)
				post_right_click();
			if (RIGHTCLICK.on == 2) {
				if (!click_toggle2) {
					click_toggle2 = 1;
					timer4 = SetTimer(NULL, 0, g_clickdelay, timerProc);
				}
				else {
					click_toggle2 = 0;
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
				memcpy(g_notifyIconData.szTip, VERSION, 128);
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

			ifstream filein;
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
				type = false;
				return CallNextHookEx(kbhook, code, wParam, lParam);
			}

			string searching = "Searching for:\r\n";
			for (UINT i = 0; i < stats_in_file.size(); i++)
				searching += '>' + stats_in_file[i] + "\r\n";
			searching += '\0';

			SendMessage(statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());

			POINT pt = getclientcursorpos(wnd);

			if (AUTOROLL.on != 2)
				SetCursorPos(0, 0);

			kill_all_OCR_auto_roll = false;

			thread roll(OCR_auto_roll, wnd, pt, font16);
			roll.detach();

			type = false;
		}
		else if (key.vkCode == READ.hotkey_key			&&READ.on		&&!pause	&&!type) {
			type = true;

			int x, y;
			BYTE* newbuf = windowcapture(GetForegroundWindow(), "capture.bmp", &x, &y);
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
			cout << "-----------------------------------------------" << '\n';

			type = false;
		}
		else if (key.vkCode == TEXTSPAM.hotkey_key		&&READ.on		&&!pause	&&!type) {
			type = true;

			check_d2foreground();

			d2type(spam_text_in_file);

			type = false;
		}
		else if (key.vkCode == LAUNCH.hotkey_key		&&LAUNCH.on		&&!pause	&&!type) {
			type = true;

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
					return CallNextHookEx(kbhook, code, wParam, lParam);
				string line;
				while (getline(filein, line))
					text.push_back(line);
				filein.close();

				for (int i = 0; i < (int)text.size(); i++) {
					line = text[i];
					line = line.substr(0, line.find('*'));
					line.erase(remove(line.begin(), line.end(), '\t'), line.end());
					string val = line.substr(line.find(':') + 1, line.size());
					line.erase(remove(line.begin(), line.end(), ' '), line.end());
					transform(line.begin(), line.end(), line.begin(), ::tolower);
					string var_name = line.substr(0, line.find(':'));
					if (var_name == "gamepath")
text[i] = "gamepath\t\t:\t" + gamepath;
				}
				ofstream fileout;
				fileout.open("config.txt");
				if (!fileout)
					return CallNextHookEx(kbhook, code, wParam, lParam);
				for (unsigned int i = 0; i < text.size(); i++)
					fileout << text[i] << '\n';
				fileout.close();
			}
			gamedir = gamepath.substr(0, gamepath.rfind('\\'));
			string gamename = gamepath.substr(gamepath.rfind('\\') + 1, gamepath.size());

			if (gamepath.size() > 0)
				ShellExecute(g_hwnd, TEXT("open"), str_to_wstr(gamename).c_str(), str_to_wstr(targetlines).c_str(), str_to_wstr(gamedir).c_str(), SW_SHOW);

			type = false;
		}
		else if (key.vkCode == CHARMROLL.hotkey_key		&&CHARMROLL.on) {
			check_d2foreground();
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

			for (int i = 0; count < inv_pos.size() && i < 3; i++, count += 2) {
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
		else if (key.vkCode == TIMER.hotkey_key			&&TIMER.on		&&!pause	&&!type) {
			type = true;
			if (!timer_on) {
				timer_on = 1;
				curworkout = 0;
				cout << "timer started - timer interval: " << TIMER.on << (TIMER.on == 1 ? " second" : " seconds") << '\n';
				ShowWindow(hworkout, SW_SHOW);
				SetWindowText(hworkout, str_to_wstr("sets completed: " + int_to_str(totalsets) + '\n' + ((workout.size() > 0) ? (workout[curworkout] + "\r\n ") : "") + int_to_str(TIMER.on)).c_str());
				timer3 = SetTimer(NULL, 0, 998, timerProc);
				starttime = ((double)clock() / 1000);
			}
			else {
				ShowWindow(hworkout, SW_HIDE);
				cout << "timer killed" << '\n';
				timer_on = 0;
			}
			type = false;
		}
		else if (key.vkCode == TEST.hotkey_key			&&TEST.on		&&!pause	&&!type) {
			type = true;

			SendMessage(g_hwnd, WM_TEST, NULL, NULL);

			type = false;
		}
		else if (key.vkCode == BARBWW.hotkey_key		&&BARBWW.on		&&!pause	&&!type){
			type = true;

			check_d2foreground();

			RECT rc = getmappedclientrect(GetForegroundWindow());

			vector<POINT> tri = {
				{(rc.right - rc.left) / 2 + rc.left + 25, (rc.bottom - rc.top) / 2 - 2 + rc.top},
				{(rc.right - rc.left) / 2 + rc.left - 55, (rc.bottom - rc.top) / 2 - 18 + rc.top},
				{(rc.right - rc.left) / 2 + rc.left + 32, (rc.bottom - rc.top) / 2 - 39 + rc.top}
			};
			
			keydownup('A');
			SetCursorPos(tri[barbwwcountpi % 3].x, tri[barbwwcountpi % 3].y);
			post_right_click(0);

			barbwwcountpi++;

			type = false;
		}
		else if (key.vkCode == BARBLEAP.hotkey_key		&&BARBLEAP.on	&&!pause	&&!type) {
			type = true;

			check_d2foreground();

			RECT rc = getmappedclientrect(GetForegroundWindow());
			POINT pt;
			pt.x = (rc.right - rc.left) / 2 + rc.left;
			pt.y = (rc.bottom - rc.top) / 2 - 2 + rc.top;

			SetCursorPos(pt.x, pt.y);

			type = false;
		}
		else if (key.vkCode == IPLAY.hotkey_key			&&IPLAY.on		&&!pause	&&!type) {
			type = true;
			PostMessage(g_hwnd, WM_PLAYPAUSE, NULL, NULL);
			type = false;
		}
		else if (key.vkCode == IPREV.hotkey_key			&&IPREV.on		&&!pause	&&!type) {
			type = true;
			PostMessage(g_hwnd, WM_PREV, NULL, NULL);
			type = false;
		}
		else if (key.vkCode == INEXT.hotkey_key			&&INEXT.on		&&!pause	&&!type) {
			type = true;
			PostMessage(g_hwnd, WM_NEXT, NULL, NULL);
			type = false;
		}
		else if (key.vkCode == IVOLU.hotkey_key			&&IVOLU.on		&&!pause	&&!type) {
			type = true;
			PostMessage(g_hwnd, WM_VOLU, NULL, NULL);
			type = false;
		}
		else if (key.vkCode == IVOLD.hotkey_key			&&IVOLD.on		&&!pause	&&!type) {
			type = true;
			PostMessage(g_hwnd, WM_VOLD, NULL, NULL);
			type = false;
		}
		else if (key.vkCode == VK_CANCEL				/*&&AUTOROLL.on	&&!pause	&&!type*/) {
			kill_all_OCR_auto_roll = true;
		}
		else if (key.vkCode == SELL.hotkey_key) {
			/*GetCursorPos(&q);
			h = GetForegroundWindow();
			GetWindowRect(h, &r);
			Sleep(300);
			left_click();
			Sleep(50);
			SetCursorPos((int)(r.left + .30*(r.right - r.left)), (int)(r.top + .38*(r.bottom - r.top)));
			Sleep(100);
			left_click();
			Sleep(50);
			SetCursorPos(q.x, q.y);*/
		}
		else if (key.vkCode == DUPE.hotkey_key) {
			POINT pt;
			GetCursorPos(&pt);
			post_left_click();
			SetCursorPos(224, 374);
			Sleep(14);
			post_left_click();
			Sleep(250);
			post_left_click();
			Sleep(150);
			SetCursorPos(pt.x, pt.y);
		}
		else if (((key.vkCode >= 1061 && key.vkCode <= 1064)  || (key.vkCode >= 549 && key.vkCode <= 552) || (key.vkCode >= 37 && key.vkCode <= 40)) && move_window && lastwindow != NULL) {
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
			SendMessage(g_hwnd, WM_UPDATE_WND, (WPARAM)lastwindow, (LPARAM)RDW_INVALIDATE);
			cout << lastwindow << " " << getwindowtext(lastwindow) << ": " << r.left << ", " << r.top << ", " << r.right - r.left << ", " << r.bottom - r.top << '\n';	
		}
		/*else if (key.vkCode == 579 && move_window) {
			cout << "Press any key to continue" << '\n';
			UnhookWindowsHookEx(mhook);
			_getch();
			if (GetForegroundWindow() == g_hwnd)
				mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);
		}*/

		if (key.vkCode != BARBWW.hotkey_key) {
			barbwwcountpi = 0;
		}
#pragma endregion
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}

/*

*/