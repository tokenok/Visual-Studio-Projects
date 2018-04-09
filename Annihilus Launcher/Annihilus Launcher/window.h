#ifndef window_header_guard
#define window_header_guard

#define VERSION L"Annihilus Launcher_v1.0 by token"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

extern const TCHAR className[];
extern const UINT WM_TASKBARCREATED;

#define WM_TRAYICON (WM_USER + 0)

#define CURSOR_LOADING (WM_USER + 0)

extern HWND g_hwnd;

extern HWND launch_btn;
extern HWND settings_btn;
extern HWND close_btn;
extern HWND skiptobnet_btn;
extern HWND windowmode_btn;
extern HWND showconsole_btn;

extern HHOOK kbhook;
extern HHOOK mhook;
extern HCURSOR idle_cursor;
extern HCURSOR press_cursor;
extern HCURSOR loading_cursor;
extern HMENU g_menu;

enum btn_ids {
	launch_btn_id,
	settings_btn_id,
	close_btn_id,
	skiptobnet_id, 
	windowmode_id,
	showconsole_id
};

extern NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
BOOL CALLBACK SettingsDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);

extern bool move_window;
extern HWND lastwindow;

extern std::vector<HWND> old_d2windowlist;
extern std::vector<HWND> new_d2windowlist;

extern DWORD WMC_HOTKEY;

ATOM register_class(HINSTANCE hInstance);
void InitNotifyIconData();

void Minimize();
void Restore();

#endif

