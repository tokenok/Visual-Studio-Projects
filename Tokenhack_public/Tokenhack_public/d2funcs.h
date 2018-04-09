#ifndef d2funcs_header_guard
#define d2funcs_header_guard

#include <Windows.h>
#include <vector>
#include <string>
#include <map>

extern int transmutebuttonx;
extern int transmutebuttony;

extern std::string gamedir;
extern std::string gamepath;
extern std::string targetlines;

extern UINT ocr_no_stats_timeout;
extern UINT ocr_same_stats_timout;
extern UINT layered_window_delay;

void d2type(std::string s, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0);
void d2type(std::vector<std::string> &v, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0);

bool get_transmute_pos(HWND wnd, LONG* x, LONG* y);
bool get_transmute_pos(HWND wnd, POINT* pt);

void load_stats_and_colors_from_file();

bool compare_item_stats_to_file_stats(std::vector<std::string>& ocr_stats, std::vector<std::string>& file_stats, const bool& fast_skip = true);

extern bool kill_all_OCR_auto_roll;
extern int layered_window_fix;
extern int ocr_title_display;
extern std::map<HWND, std::string> original_window_titles;

void OCR_auto_roll(HWND wnd, std::vector<std::vector<BYTE>> &font, POINT &item_pt);


#endif

