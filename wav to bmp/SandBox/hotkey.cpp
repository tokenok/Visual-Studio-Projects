#include <algorithm>
#include <fstream>

#include <fstream>

#include "hotkey.h"
#include "common.h"
#include "window.h"
#include "functions.h"

using namespace std;

vector<DWORD> hotkeys;
DWORD global_change_hotkey;

string GetModName(int key) {
	switch (key) {
		case 1:
			return "ALT";
		case 2:
			return "CTRL";
		case 3:
			return "CTRL+ALT";
		case 4:
			return "SHIFT";
		case 5:
			return "SHIFT+ALT";
		case 6:
			return "CTRL+SHIFT";
		case 7:
			return "CTRL+SHIFT+ALT";
		case 8:
			return "WND";
		case 9:
			return "WND+ALT";
		case 10:
			return "WND+CTRL";
		case 11:
			return "WND+CTRL+ALT";
		case 12:
			return "WND+SHIFT";
		case 13:
			return "WND+SHIFT+ALT";
		case 14:
			return "WND+CTRL+SHIFT";
		case 15:
			return "WND+CTRL+SHIFT+ALT";
		default:
			return "";
	}
}
void changebuttontext(HWND btn, DWORD w) {
	BasicControl* ptr = ((BasicControl*)GetWindowLongPtr(btn, GWL_USERDATA));
	
	if (w == 0) {
		ptr->window_text = "NULL";
		SetWindowText(ptr->handle, str_to_wstr(ptr->window_text).c_str());
		RedrawWindow(btn, NULL, NULL, RDW_INVALIDATE);
		return;
	}

	string text = "";
	int count = 0;
	while (w > 256) {
		count++;
		w -= 256;
	}
	text += GetModName(count);
	if (text.size() > 0)
		text += "+";
	UINT scanCode = MapVirtualKey(w, MAPVK_VK_TO_VSC);

	switch (w) {
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
		case VK_APPS:{
			scanCode |= 0x100; // set extended bit
			break;
		}
	}

	wchar_t keyName[256];
	if (GetKeyNameText((LONG)scanCode << 16, keyName, sizeof(keyName)) != 0)
		text += wstr_to_str((wstring)keyName);
	else
		text += (getvktext((int)w) == "" ? "Undefined" : getvktext((int)w));
	ptr->window_text = text;
	SetWindowText(ptr->handle, str_to_wstr(ptr->window_text).c_str());
	RedrawWindow(ptr->handle, NULL, NULL, RDW_INVALIDATE);
}
void fileedit(UINT btnid, DWORD hotkeykey) {
	vector<string> text;
	string line;

	ifstream filein;
	ofstream fileout;

	filein.open("config.txt");
	if (!filein) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	while (getline(filein, line))
		text.push_back(line);
	filein.close();

	for (auto & a: text) {
		string line = a;
		line = line.substr(0, line.find('*'));
		line.erase(remove(line.begin(), line.end(), '\t'), line.end());
		line.erase(remove(line.begin(), line.end(), ' '), line.end());
		transform(line.begin(), line.end(), line.begin(), ::tolower);
		string var_name = line.substr(0, line.find(':'));
		var_name = var_name.substr(0, var_name.find('['));
		var_name = var_name.substr(0, var_name.find('('));
		if (var_name.size() == 0)
			continue;
		for (auto & b: tokenhackfunctions) {			
			if (var_name == b->name_in_file && b->hotkey_button.id == btnid) {
				a = a.erase(a.find('(') + 1, a.find(')') - a.find('(') - 1);
				a.insert(a.find('(') + 1, int_to_str((int)hotkeykey));
				break;
			}
		}
	}
	fileout.open("config.txt");
	if (!fileout) {
		MessageBox(NULL, TEXT("Could not open config.txt"), TEXT("Error"), MB_OK);
		PostQuitMessage(0);
	}
	for (auto & a: text)
		fileout << a << '\n';
	fileout.close();
}
void hotkeychange(HWND btn, UINT btnid, DWORD &hotkeykey, DWORD w) {
	hotkeykey = w;
	changebuttontext(btn, hotkeykey);
	fileedit(btnid, hotkeykey);
}

void update_hotkeykeys() {
	hotkeys.erase(hotkeys.begin(), hotkeys.end());
	for (auto & a : tokenhackfunctions)
		if (a->on && a->group_name == grp_tokenhack_function)
			hotkeys.push_back(a->hotkey_key);
}

