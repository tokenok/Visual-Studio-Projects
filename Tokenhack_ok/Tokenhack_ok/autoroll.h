#ifndef ocr_header_guard
#define ocr_header_guard

#include <Windows.h>
#include <vector>
#include <string>
#include <map>

extern bool ocr_spaces_on;

extern int layered_window_fix;
extern bool kill_all_OCR_auto_roll;

extern std::map<HWND, std::string> original_window_titles;

// ascii value, number of pixels in character+2 (size-2), width, height, [grid values...](zero based)
extern std::vector<std::vector<BYTE>> font16;

bool get_transmute_pos(HWND wnd, LONG* x, LONG* y);
bool get_transmute_pos(HWND wnd, POINT* pt);

std::vector<std::string> load_stats();
bool compare_item_stats_to_file_stats(std::vector<std::string>& ocr_stats, std::vector<std::string>& file_stats, const bool& fast_skip = true);
void OCR_auto_roll(HWND wnd, std::vector<std::vector<BYTE>> &font, POINT &item_pt);

bool process(BYTE *newbuf, int x, int y, std::string outfile = "process.bmp", bool OCR_visualization = false);

std::vector<int> find_lines(BYTE* newbuf, int x, int y);
std::vector<int> isolate_letters(BYTE* newbuf, int width, int top, int bottom);
bool match(std::vector<int> a, std::vector<std::vector<BYTE>> &charset);
std::vector<std::string> ocr(BYTE* newbuf, int x, int y, std::vector<std::vector<BYTE>> &charset, bool OCR_visualization = false);

#endif

