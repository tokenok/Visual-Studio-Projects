#ifndef functions_header_guard
#define functions_header_guard

#include <string>
#include <Windows.h>
#include <vector>

#include "window.h"

#pragma comment(lib, "Winmm.lib")

#pragma region owners and groups

enum group_names {
	grp_no_group,
	grp_tokenhack_function,
	grp_function_tab_button,
	grp_stat_tab_button,
	grp_tokenhack_offset,
	grp_config_tab_button
};

#pragma endregion

ATOM init_wnd_class(HINSTANCE hInst, LPCWSTR classname, WNDPROC wndproc, UINT iconid);
void init_notify_icon_data(NOTIFYICONDATA &nid, HWND wnd, UINT msgid, UINT iconid, LPCWSTR icontiptitle);

COLORREF color(int iter, double frequency, double phase = 120, double center = 128, double width = 127);

BOOL ProtectProcess();

std::string changeconfigonstate(std::string name_in_file, int new_val = 0, bool hex = false);
void setbuttonstate(HWND button_handle, int on_state);

#pragma region tokenhack function class

class tokenhackfunction {
	public:
	CheckBox checkbox_button;
	Button hotkey_button;
	StaticControl static_control;

	/*CheckBox config_toggle_button;
	EditControl config_edit_control;
	StaticControl config_static_control;
	CustomComboBox config_custom_combobox;*/

	std::string name_in_file;

	DWORD hotkey_key;

	int on;

	UINT group_name;

	tokenhackfunction();
	tokenhackfunction(HWND parent, std::string static_text, std::string name_in_file,



		std::vector<std::string> ccb_options = {{"On"}, {"Off"}});

	void set_on_state(int state);
};

extern std::vector<tokenhackfunction*> tokenhackfunctions;
extern std::vector<tokenhackfunction*> all_tokenhackfunctions;

#pragma endregion

extern std::string getmessagetext(int msgId);
extern std::string getvktext(int vk);

#endif

