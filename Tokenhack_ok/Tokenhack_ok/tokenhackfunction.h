#pragma once

#include <vector>

#include "customcontrols.h"

extern BasicControl_colorscheme tokenhackfunctioncheckbox_scheme;
extern BasicControl_colorscheme tokenhackfunctionstatic_scheme;
extern BasicControl_colorscheme tokenhackfunctiontab_scheme;
extern BasicControl_colorscheme tokenhackfunctionhotkey_scheme;

class tokenhackfunction {
public:
	static std::vector<tokenhackfunction*> tokenhackfunctions;

	static const int static_control_height = 16;
	static const int static_control_width = 300;

	static const int hotkey_button_height = 16;
	static const int hotkey_button_width = 150;

	static const int checkbox_width = 15;
	static const int checkbox_height = 15;

	CheckBox checkbox_button;
	Button hotkey_button;
	StaticControl static_control;

	void(*Proc)();

	string key_name;
	DWORD default_hotkey;

	DWORD hotkey;

	int on;

	UINT owner_name;
	UINT group_name;

	tokenhackfunction() {}
	tokenhackfunction(string static_text, DWORD default_hotkey, void(*Proc)());

	static void read_all();

	void read();

	void set_on_state(int on_state);

	void change_hotkey(int newhotkey);

	void Do();

private:
	int window_id = 10000;

	void set_hotkey_btn_text(DWORD hk);
};