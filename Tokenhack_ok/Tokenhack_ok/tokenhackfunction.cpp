#include "tokenhackfunction.h"
#include "Tokenhack.h"
#include "autoroll.h"

#include "resource.h"

#include "C:\CPPlibs\common\common.h"

BasicControl_colorscheme tokenhackfunctioncheckbox_scheme{
		RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
		RGB(45, 45, 45), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
		RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
		RGB(45, 45, 45), RGB(254, 33, 61), RGB(45, 45, 45)			//HOVER OFF: background/border/text
};
BasicControl_colorscheme tokenhackfunctionstatic_scheme{
	RGB(10, 10, 10), RGB(148, 0, 211), RGB(148, 0, 211)			//IDLE ON: background/border/text
};
BasicControl_colorscheme tokenhackfunctiontab_scheme{
	RGB(100, 100, 100), RGB(100, 100, 100), RGB(10, 10, 10),	//IDLE ON: background/border/text
	RGB(100, 100, 100), RGB(100, 100, 100), RGB(10, 10, 10),	//HOVER ON: background/border/text
	//															//SELECTED ON: background/border/text
	RGB(10, 10, 10), RGB(100, 100, 100), RGB(100, 100, 100),	//IDLE OFF: background/border/text
	RGB(100, 100, 100), RGB(10, 10, 10), RGB(10, 10, 10)		//HOVER OFF: background/border/text
	//															//SELECTED OFF: background/border/text
};
BasicControl_colorscheme tokenhackfunctionhotkey_scheme{
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(254, 33, 61),		//IDLE ON: background/border/text
	RGB(45, 45, 45), RGB(254, 33, 61), RGB(254, 33, 61),		//HOVER ON: background/border/text
	//															//SELECTED ON: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10),			//IDLE OFF: background/border/text
	RGB(10, 10, 10), RGB(254, 33, 61), RGB(10, 10, 10)			//HOVER OFF: background/border/text
	//															//SELECTED OFF: background/border/text
};

void TOKENHACK_CHECKBOX_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	for (auto a : tokenhackfunction::tokenhackfunctions) {
		if (b->handle == a->checkbox_button.handle) {
			if (a->on) {//if on, turn off
				a->set_on_state(0);
				EnableWindow(a->hotkey_button.handle, false);
			}
			else {//if off, turn on					
				a->set_on_state(1);
				EnableWindow(a->hotkey_button.handle, true);
			}
			RedrawWindow(a->hotkey_button.handle, NULL, NULL, RDW_INVALIDATE);


			if (a->checkbox_button.id == READ.checkbox_button.id || a->checkbox_button.id == AUTOROLL.checkbox_button.id) {
				if (g_statbox) {
					if (!READ.on && !AUTOROLL.on) {
						DestroyWindow(g_statbox);
						//DestroyWindow(STAT_TAB_BTN_OUTPUT.handle);
						g_statbox = NULL;
						Resize();
					}
				}
				else {
					statbox_display(g_hwnd);
					Resize();
				}
				if (a->checkbox_button.id == AUTOROLL.checkbox_button.id) {
					if (AUTOROLL.on)
						statbox_display(g_hwnd, true);
					else
						SetWindowText(g_statbox, L"");
				}
			}
			break;
		}
	}
}
void TOKENHACK_HOTKEYBUTTON_PROC(WPARAM wParam, LPARAM /*lParam*/) {
	CheckBox* b = (CheckBox*)GetWindowLongPtr((HWND)wParam, GWLP_USERDATA);
	for (auto a : tokenhackfunction::tokenhackfunctions) {
		if (b->handle == a->hotkey_button.handle) {
			DWORD hotkey = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(HOTKEYBOX), g_hwnd, (DLGPROC)hotkeydlgProc);
			if (hotkey != 0) {
				if (hotkey == 1)
					hotkey = 0;
				a->change_hotkey(hotkey);
			}
			break;
		}
	}
}

vector<tokenhackfunction*> tokenhackfunction::tokenhackfunctions;

tokenhackfunction::tokenhackfunction(string static_text, DWORD default_hotkey, void(*Proc)()) :
	checkbox_button("T", WS_VISIBLE | WS_CHILD, 5, tokenhackfunctions.size() * static_control_height, checkbox_width, checkbox_height, window_id++, TOKENHACK_CHECKBOX_PROC, tokenhackfunctioncheckbox_scheme, font_a, press_cursor, 0, 1, 1),
	hotkey_button("", WS_VISIBLE | WS_CHILD, 21, tokenhackfunctions.size() * static_control_height, hotkey_button_width, hotkey_button_height, window_id++, TOKENHACK_HOTKEYBUTTON_PROC, tokenhackfunctionhotkey_scheme, font_a, press_cursor, 1, 1),
	static_control(static_text, WS_VISIBLE | WS_CHILD, 185, tokenhackfunctions.size() * static_control_height, static_control_width, static_control_height, window_id++, nullptr, tokenhackfunctionstatic_scheme, font_a, idle_cursor, BasicControl_hover(), 1, 1)
{
	checkbox_button.handle = NULL; hotkey_button.handle = NULL; static_control.handle = NULL; hotkey = 0; on = 0;
	this->Proc = Proc; this->key_name = static_text; this->default_hotkey = default_hotkey; this->owner_name = 0; this->group_name = 1;

	read();
	set_hotkey_btn_text(hotkey);

	tokenhackfunctions.push_back(this);
}

void tokenhackfunction::read_all() {
	for (UINT i = 0; i < tokenhackfunctions.size(); i++) {
		tokenhackfunctions[i]->read();
	}
}

void tokenhackfunction::read() {
	wchar_t value[255];
	GetPrivateProfileString(L"Tokenhack function", STW(key_name), STW(int_to_str(default_hotkey) + "," + "1"), value, 255, g_config_path.c_str());
	string svalue = wstr_to_str(value);
	vector<string> split = split_str(svalue, ",");
	hotkey = str_to_int(split[0]);
	on = str_to_int(split[1]);
}

void tokenhackfunction::set_on_state(int on_state) {
	WritePrivateProfileString(L"Tokenhack function", STW(key_name), STW(int_to_str(hotkey) + "," + int_to_str(on_state)), g_config_path.c_str());
	on = on_state;
	checkbox_button.toggle_state = on_state;
	hotkey_button.state = on_state;
	//	static_control.state = on_state;
}

void tokenhackfunction::change_hotkey(int newhotkey) {
	WritePrivateProfileString(L"Tokenhack function", STW(key_name), STW(int_to_str(newhotkey) + "," + int_to_str(on)), g_config_path.c_str());
	this->hotkey = newhotkey;
	set_hotkey_btn_text(hotkey);
}

void tokenhackfunction::Do() {
	if (Proc != nullptr)
		Proc();
}

void tokenhackfunction::set_hotkey_btn_text(DWORD hk) {
	if (hk == 0) {
		hotkey_button.window_text = "NULL";
	//	SetWindowText(ptr->handle, str_to_wstr(ptr->window_text).c_str());
		RedrawWindow(hotkey_button.handle, NULL, NULL, RDW_INVALIDATE);
		return;
	}

	string text = "";
	int count = 0;
	while (hk > 256) {
		count++;
		hk -= 256;
	}
	switch (count) {
		case 1:	 text += "ALT+"; break;
		case 2:	 text += "CTRL+"; break;
		case 3:	 text += "CTRL+ALT+"; break;
		case 4:	 text += "SHIFT+"; break;
		case 5:	 text += "SHIFT+ALT+"; break;
		case 6:	 text += "CTRL+SHIFT+"; break;
		case 7:	 text += "CTRL+SHIFT+ALT+"; break;
		case 8:	 text += "WND+"; break;
		case 9:	 text += "WND+ALT+"; break;
		case 10: text += "WND+CTRL+"; break;
		case 11: text += "WND+CTRL+ALT+"; break;
		case 12: text += "WND+SHIFT+"; break;
		case 13: text += "WND+SHIFT+ALT+"; break;
		case 14: text += "WND+CTRL+SHIFT+"; break;
		case 15: text += "WND+CTRL+SHIFT+ALT+"; break;
	}

	UINT scanCode = MapVirtualKey(hk, MAPVK_VK_TO_VSC);
	switch (hk) {
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
		case VK_APPS: {
			scanCode |= 0x100; // set extended bit
			break;
		}
	}

	wchar_t keyName[256];
	if (GetKeyNameText((LONG)scanCode << 16, keyName, sizeof(keyName)) != 0)
		text += wstr_to_str((wstring)keyName);
	else
		text += (getvktext((int)hk) == "" ? "Undefined" : getvktext((int)hk));
	hotkey_button.window_text = text;
	//SetWindowText(hotkey_button.handle, str_to_wstr(ptr->window_text).c_str());
	RedrawWindow(hotkey_button.handle, NULL, NULL, RDW_INVALIDATE);
}