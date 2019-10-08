#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>

#include "image.h"
#include "autoroll.h"
#include "resource.h"
#include "keypress.h"
#include "Tokenhack.h"
#include "config.h"

#include "C:\CPPlibs\common\common.h"

using namespace std;

bool ocr_spaces_on = true;

// ascii value, number of pixels in character+2 (size-2), width, height, [grid values...](zero based)
vector<vector<BYTE>> font16 = {
	{32, 2, 0, 0},
	{37, 47, 10, 9, 1, 2, 10, 11, 12, 13, 14, 16, 20, 23, 24, 25, 26, 30, 31, 32, 33, 35, 40, 41, 42, 43, 44, 45, 46, 47, 48, 54, 55, 56, 58, 59, 63, 65, 66, 67, 69, 73, 75, 76, 77, 82, 86, 87, 88},//%
	{39, 5, 1, 3, 0, 1, 2},//'
	{40, 15, 3, 12, 2, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 31, 35},//(
	{41, 14, 2, 11, 0, 2, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21},//)
	{43, 15, 5, 5, 2, 5, 7, 10, 11, 12, 13, 14, 15, 17, 21, 22, 23},//+
	{44, 6, 2, 3, 1, 2, 3, 4},//,
	{45, 4, 2, 1, 0, 1},//- (PROBLEM CHAR) - right side of gap
	{47, 16, 6, 12, 5, 11, 16, 22, 27, 32, 33, 38, 39, 44, 49, 55, 60, 66},//'/'
	{48, 37, 9, 10, 4, 5, 11, 12, 13, 14, 15, 16, 19, 24, 25, 27, 28, 32, 33, 34, 35, 36, 40, 44, 45, 48, 53, 54, 56, 61, 62, 64, 65, 70, 74, 75, 76, 78, 85},//0
	{49, 11, 2, 9, 1, 2, 4, 6, 8, 10, 12, 14, 16},//1
	{50, 27, 6, 9, 2, 7, 8, 10, 12, 13, 16, 17, 18, 19, 22, 23, 28, 33, 34, 38, 39, 43, 44, 48, 49, 50, 51, 52, 53},//2
	{51, 26, 6, 9, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 16, 21, 26, 27, 28, 34, 35, 37, 41, 43, 47, 49, 50, 52},//3
	{52, 20, 7, 8, 4, 10, 11, 17, 18, 23, 25, 29, 32, 35, 36, 37, 38, 39, 40, 41, 46, 53},//4
	{53, 26, 6, 8, 1, 2, 4, 7, 13, 18, 19, 20, 21, 22, 25, 28, 29, 30, 31, 35, 36, 37, 40, 41, 43, 44, 45, 46},//5
	{54, 24, 6, 10, 5, 9, 10, 15, 20, 21, 25, 26, 27, 28, 31, 35, 36, 37, 41, 43, 47, 49, 50, 51, 52, 57},//6
	{55, 16, 6, 9, 2, 3, 6, 7, 8, 10, 11, 16, 22, 28, 33, 39, 44, 50},//7
	{56, 31, 6, 10, 2, 3, 7, 8, 9, 10, 12, 13, 16, 19, 21, 22, 25, 26, 27, 30, 31, 34, 36, 40, 41, 42, 43, 46, 49, 50, 51, 52, 56},//8
	{57, 23, 6, 9, 3, 7, 8, 9, 10, 11, 12, 13, 17, 18, 19, 23, 25, 28, 29, 32, 33, 34, 39, 45, 50},//9
	{58, 8, 2, 7, 0, 2, 3, 10, 12, 13},//:
	{65, 25, 8, 9, 4, 11, 12, 19, 21, 26, 29, 34, 38, 41, 46, 49, 50, 51, 52, 54, 55, 56, 57, 63, 64, 65, 71},//A
	{66, 35, 5, 9, 0, 1, 2, 3, 4, 5, 6, 9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 24, 25, 26, 29, 30, 31, 34, 35, 36, 37, 38, 39, 40, 41},//B
	{67, 26, 7, 10, 4, 9, 10, 11, 12, 13, 15, 20, 22, 28, 35, 36, 42, 43, 50, 51, 58, 59, 60, 61, 62, 66, 67, 68},//C
	{68, 16, 4, 8, 0, 1, 2, 6, 7, 11, 15, 19, 23, 26, 27, 28, 29, 30},//D (PROBLEM CHAR) - right side of gap
	{69, 33, 5, 9, 1, 2, 3, 4, 6, 10, 11, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 30, 31, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44},//E
	{70, 23, 6, 8, 0, 1, 2, 3, 4, 7, 12, 13, 18, 19, 20, 22, 23, 24, 25, 26, 28, 30, 31, 37, 43},//F
	{71, 28, 7, 9, 1, 2, 3, 4, 5, 6, 8, 9, 14, 21, 26, 27, 28, 33, 34, 35, 41, 43, 48, 51, 52, 53, 54, 55, 59, 60},//G
	{72, 37, 7, 10, 6, 7, 8, 12, 15, 19, 22, 26, 28, 29, 30, 31, 32, 33, 35, 36, 37, 38, 39, 40, 41, 42, 43, 47, 49, 50, 54, 55, 56, 57, 61, 62, 63, 64, 68},//H
	{73, 16, 2, 8, 0, 1, 2, 3, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15},//I
	{74, 21, 2, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18},//J
	{75, 24, 7, 9, 1, 4, 8, 10, 11, 15, 16, 17, 22, 23, 29, 30, 36, 38, 43, 46, 50, 54, 55, 56, 57, 62},//K
	{76, 17, 5, 9, 0, 5, 10, 15, 20, 25, 30, 35, 36, 37, 38, 40, 42, 43, 44},//L
	{77, 53, 9, 10, 3, 4, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 22, 25, 26, 27, 30, 31, 34, 35, 36, 39, 40, 43, 44, 45, 48, 49, 52, 53, 54, 57, 58, 61, 62, 63, 66, 67, 70, 71, 72, 75, 76, 79, 80, 81, 84, 85, 88, 89},//M
	{78, 34, 8, 9, 0, 1, 6, 7, 9, 14, 17, 18, 19, 22, 24, 25, 27, 30, 32, 33, 36, 38, 40, 41, 45, 46, 48, 49, 54, 56, 57, 63, 64, 65, 70, 71},//N
	{79, 53, 9, 10, 4, 10, 11, 12, 13, 14, 15, 16, 19, 20, 22, 25, 27, 28, 31, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 58, 61, 62, 64, 67, 69, 70, 74, 75, 76, 77, 78, 84, 85},//O
	{80, 25, 6, 8, 0, 1, 2, 3, 4, 6, 7, 10, 12, 13, 16, 17, 19, 22, 25, 26, 27, 30, 31, 36, 37, 42, 43},//P
	{81, 37, 9, 10, 2, 3, 4, 5, 6, 10, 15, 16, 18, 19, 25, 26, 27, 34, 35, 36, 44, 45, 49, 52, 53, 54, 55, 58, 61, 64, 65, 66, 67, 68, 69, 75, 76, 77, 85},//Q
	{82, 25, 7, 9, 0, 1, 2, 3, 7, 11, 14, 18, 21, 24, 25, 29, 30, 31, 38, 43, 46, 49, 50, 54, 55, 56, 62},//R
	{83, 24, 5, 8, 1, 2, 3, 4, 5, 6, 9, 10, 11, 16, 17, 23, 24, 29, 30, 31, 34, 35, 36, 37, 38, 39},//S
	{84, 24, 9, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 14, 22, 23, 32, 40, 41, 49, 50, 59, 67, 68, 76, 77},//T
	{85, 36, 10, 9, 0, 1, 8, 9, 10, 11, 18, 19, 20, 21, 28, 29, 30, 31, 38, 39, 40, 41, 48, 49, 51, 52, 58, 59, 61, 68, 72, 73, 74, 75, 76, 77, 84, 85},//U
	{86, 22, 9, 8, 0, 1, 8, 10, 16, 17, 19, 20, 25, 29, 33, 34, 39, 42, 48, 50, 51, 58, 59, 67},//V
	{87, 43, 13, 9, 0, 1, 4, 5, 8, 11, 12, 14, 18, 20, 21, 24, 27, 28, 31, 32, 33, 37, 41, 45, 46, 49, 54, 55, 58, 62, 68, 70, 71, 72, 74, 81, 82, 83, 85, 86, 87, 95, 96, 99, 108},//W
	{88, 20, 8, 9, 0, 7, 9, 14, 15, 18, 21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63, 64},//X
	{89, 25, 9, 10, 7, 9, 10, 16, 17, 19, 20, 24, 25, 29, 30, 32, 33, 38, 39, 40, 41, 49, 58, 67, 76, 84, 85},//Y
	{90, 26, 6, 9, 0, 1, 2, 3, 4, 5, 10, 11, 16, 21, 22, 27, 32, 37, 38, 43, 44, 45, 46, 47, 49, 50, 51, 53},//Z
	{97, 15, 6, 7, 3, 9, 14, 16, 19, 22, 25, 29, 30, 31, 32, 35, 36},//a
	{98, 16, 4, 6, 0, 1, 2, 4, 8, 9, 10, 12, 14, 15, 16, 19, 20, 22},//b
	{99, 16, 6, 8, 3, 4, 5, 8, 10, 11, 13, 19, 24, 25, 31, 38, 45, 47},//c
	{100, 24, 6, 7, 0, 1, 2, 4, 6, 7, 11, 12, 13, 17, 18, 19, 23, 24, 25, 29, 30, 31, 34, 37, 38, 39},//d
	{101, 17, 4, 7, 0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 20, 27},//e
	{102, 19, 4, 6, 0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 21},//f
	{103, 21, 6, 8, 3, 4, 7, 8, 9, 10, 11, 18, 24, 27, 28, 29, 31, 35, 37, 41, 44, 45, 46},//g
	{104, 22, 5, 8, 4, 9, 10, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 29, 30, 31, 34, 35, 39},//h
	{105, 9, 1, 7, 0, 1, 2, 3, 4, 5, 6},//i
	// no lowercase j (PROBLEM CHAR) - identical to lowercase i (105)
	{107, 15, 4, 7, 0, 2, 3, 4, 5, 8, 12, 13, 16, 18, 20, 23, 24},//k
	{108, 11, 3, 7, 0, 3, 6, 9, 12, 15, 18, 19, 20},//l
	{109, 29, 7, 8, 2, 3, 7, 8, 9, 10, 11, 12, 13, 14, 17, 20, 21, 24, 27, 28, 31, 34, 35, 38, 41, 42, 45, 48, 49, 52, 55},//m
	{110, 27, 6, 7, 0, 4, 5, 6, 7, 10, 11, 12, 14, 16, 17, 18, 20, 21, 22, 23, 24, 27, 28, 29, 30, 34, 35, 36, 41},//n
	{111, 33, 7, 8, 3, 8, 9, 10, 11, 12, 14, 17, 20, 21, 24, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 41, 43, 44, 45, 46, 47, 51, 52, 53},//o
	{112, 12, 4, 6, 0, 1, 2, 3, 7, 11, 12, 13, 14, 20},//p
	{113, 25, 7, 9, 3, 8, 9, 10, 11, 12, 14, 19, 20, 21, 27, 28, 34, 35, 38, 41, 43, 45, 46, 47, 51, 52, 59},//q
	{114, 14, 5, 6, 0, 2, 3, 8, 10, 11, 13, 15, 16, 17, 23, 29},//r
	{115, 16, 5, 7, 1, 2, 3, 5, 6, 11, 12, 18, 24, 25, 26, 31, 32, 33},//s
	{116, 15, 7, 7, 0, 7, 8, 9, 10, 11, 12, 13, 17, 24, 31, 38, 45},//t
	{117, 10, 4, 7, 3, 7, 11, 15, 19, 22, 24, 25},//u (PROBLEM CHAR) - right side of gap
	{118, 16, 8, 8, 7, 8, 14, 16, 17, 21, 25, 29, 34, 36, 42, 44, 51, 59},//v
	{119, 29, 10, 7, 0, 10, 15, 16, 18, 19, 20, 23, 25, 28, 31, 33, 34, 35, 37, 38, 41, 44, 47, 52, 53, 54, 55, 62, 63, 65, 66},//w
	{120, 15, 7, 6, 0, 5, 6, 8, 11, 16, 17, 23, 24, 29, 32, 35, 40},//x
	{121, 13, 8, 7, 1, 8, 9, 14, 15, 18, 21, 27, 28, 43, 51},//y
	{122, 17, 6, 7, 0, 1, 2, 4, 5, 10, 15, 20, 26, 31, 37, 38, 39, 40, 41}//z
};

bool kill_all_OCR_auto_roll = false;
int ocr_title_display = true;
int layered_window_fix = true;
UINT ocr_no_stats_timeout = 30;
UINT ocr_same_stats_timout = 30;
UINT layered_window_delay = 100;
int thread_count = 0;

map<HWND, string> original_window_titles;

vector<HWND> auto_roll_windows;
class auto_roll_data {
	HWND wnd;
public:
	bool kill;
	int pos;
	vector<string> prev_item_stats;
	auto_roll_data() = default;
	auto_roll_data(HWND wnd) {
		this->wnd = wnd;
		kill = false;
		auto it = find(auto_roll_windows.begin(), auto_roll_windows.end(), wnd);
		if (it != auto_roll_windows.end()) {
			auto_roll_windows.erase(it);
			kill = true;
		}
		else
			auto_roll_windows.push_back(wnd);
	}
	~auto_roll_data() {
		auto it = find(auto_roll_windows.begin(), auto_roll_windows.end(), wnd);
		if (it != auto_roll_windows.end())
			auto_roll_windows.erase(it);
	}
	int calculate_minimize_pos() {
		RECT rcDesktop;
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcDesktop, NULL);
		if (rcDesktop.right < (pos - 1) * 100)
			pos = 1;
		return (pos - 1) * 100;
	}
};

vector<string> load_stats() {
	vector<string> ret;

	ifstream filein;
	filein.open(g_config_path);
	if (!filein) {
		MessageBox(NULL, TEXT("Could not open config.ini"), TEXT("Error"), MB_OK);
	//	PostQuitMessage(0);
	}
	while (filein.is_open()) {
		while (filein.good()) {
			string line;
			getline(filein, line);
			if (line.size() > 0 && line[0] == '>') {
				line.erase(0, 1);
				if (line.size() == 0)
					continue;
				ret.push_back(line);
			}
		}
		filein.close();
	}
	return ret;
}

bool get_transmute_pos(HWND wnd, LONG* x, LONG* y) {
	RECT client = getclientrect(wnd);
	if (Config::showconsole) {
		POINT client_cursor = getclientcursorpos(wnd);
		std::cout << "current client cursor pos: " << client_cursor.x << " " << client_cursor.y << '\n';
		std::cout << "current screen resolution: " << client.right << " " << client.bottom << '\n';
	}
	if (Config::transmute_btn_pos.x > 0 && Config::transmute_btn_pos.y > 0) {
		*x = Config::transmute_btn_pos.x; *y = Config::transmute_btn_pos.y;
	}
	if (client.right == 640 && client.bottom == 480) {
		*x = 160; *y = 275;
	}
	else if (client.right == 800 && client.bottom == 600) {
		*x = 240; *y = 335;
	}
	else if (client.right == 1024 && client.bottom == 768) {
		*x = 160; *y = 445;
	}
	else if (client.right == 1280 && client.bottom == 1024) {
		*x = 165; *y = 560;
	}
	else {
		return false;
	}
	return true;
}
bool get_transmute_pos(HWND wnd, POINT* pt) {
	return get_transmute_pos(wnd, &pt->x, &pt->y);
}

bool compare_item_stats_to_file_stats(vector<string>& ocr_stats, vector<string>& file_stats, const bool& fast_skip/* = true*/) {
	for (UINT j = 0; j < ocr_stats.size(); j++) {
		ocr_stats[j].erase(remove(ocr_stats[j].begin(), ocr_stats[j].end(), ' '), ocr_stats[j].end());
		transform(ocr_stats[j].begin(), ocr_stats[j].end(), ocr_stats[j].begin(), tolower);
	}

	vector<vector<string>> items_in_file = split_vec(file_stats, ">");

	for (auto current_item : items_in_file) {
		UINT match_count = 0;
		for (auto current_line : current_item) {
			bool match = false;
			vector<string> ORs_of_current_line = split_str(current_line, "|");
			for (auto current_OR : ORs_of_current_line) {
				for (auto & j : ocr_stats) {//wtf is this coloring
					if (j.find(current_OR) != string::npos) {
						match = true;
						match_count++;
						if (fast_skip)
							break;
						else {
							j.insert(0, "#");
							break;
						}
					}
				}
				if (match && fast_skip)
					break;
			}
			if (match_count == current_item.size())
				return true;
		}
	}
	return false;
}

void OCR_auto_roll(HWND wnd, vector<vector<BYTE>> &font, POINT &item_pt) {
	auto_roll_data data(wnd);
	if (data.kill)
		return;

	original_window_titles.insert(make_pair(wnd, getwindowtext(wnd)));

	thread_count++;
	data.pos = thread_count;
	if (layered_window_fix)
		this_thread::sleep_for(chrono::milliseconds(layered_window_delay));

	POINT transmutept;
	if (!get_transmute_pos(wnd, &transmutept)) {
		std::cout << "transmute button could not be located" << '\n';
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
		if ((GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYERED) && layered_window_fix)
			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
		return;
	}

	string windowtext = wstr_to_str(VERSION);

	int x, y;
	BYTE* newbuf = windowcapture(wnd, &x, &y);
	vector<string> item_stats = ocr(newbuf, x, y, font);
	delete[] newbuf;
	if (item_stats.size() == 0) {
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
		std::cout << "no stats found" << '\n';

		if (ocr_title_display) {
			string txt = getwindowtext(wnd);
			if (txt.find(windowtext) != string::npos)
				SetWindowText(wnd, str_to_wstr("#> -NO STATS FOUND- " + windowtext + " <#").c_str());
		}
		if ((GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYERED) && layered_window_fix)
			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
		return;
	}

	RECT rcstartpos = getwindowrect(wnd);
	RECT rcmap = getmappedclientrect(wnd);
	RECT rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcDesktop, NULL);

	if (AUTOROLL.on == 2)
		ShowWindow(wnd, SW_FORCEMINIMIZE);

	vector<string> stats_in_file_copy = load_stats();

	double start_time = (double)clock();
	UINT roll_count = 0;
	UINT no_stats_count = 0;
	UINT same_stats_count = 0;
	bool match = false;
	while (!match) {
		if (kill_all_OCR_auto_roll || find(auto_roll_windows.begin(), auto_roll_windows.end(), wnd) == auto_roll_windows.end() || !IsWindow(wnd)) {
			if (CTB_VOLUME.current_val > 0)
				PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
			std::cout << "exited rolling" << '\n';
			if (ocr_title_display)
				SetWindowText(wnd, str_to_wstr("#> -KILLED- " + windowtext + " Elapsed time: " + milliseconds_to_hms((int)((double)clock() - start_time)) + " Approx rolls: " + int_to_str((int)roll_count) + " <#").c_str());
			break;
		}

		int x, y;
		BYTE* newbuf = windowcapture(wnd, &x, &y);
		vector<string> item_stats = ocr(newbuf, x, y, font);
		delete[] newbuf;

		if (item_stats.size() == 0) {
			no_stats_count++;
			if (no_stats_count > ocr_no_stats_timeout * 5) {
				if (ocr_title_display)
					SetWindowText(wnd, str_to_wstr("#> -EMERGENCY STOP- (NO STATS FOUND) " + windowtext + " Elapsed time: " + milliseconds_to_hms((int)((double)clock() - start_time)) + " Approx rolls: " + int_to_str((int)roll_count) + " <#").c_str());
				break;
			}
			WINDOWPLACEMENT wp;
			GetWindowPlacement(wnd, &wp);
			if (wp.showCmd == SW_MINIMIZE || wp.showCmd == SW_FORCEMINIMIZE || wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) {
				ShowWindow(wnd, SW_SHOWNOACTIVATE);
				SetWindowPos(wnd, HWND_NOTOPMOST, data.calculate_minimize_pos(), rcDesktop.bottom - (rcmap.top - rcstartpos.top), 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_SHOWWINDOW);
				continue;
			}
			else if (wp.showCmd == SW_MAXIMIZE || wp.showCmd == SW_SHOWMAXIMIZED) {
				ShowWindow(wnd, SW_SHOWNORMAL);
				continue;
			}
			std::cout << "no stats found (" << no_stats_count << "/" << ocr_no_stats_timeout * 5 << ")" << '\n';
			if (ocr_title_display)
				SetWindowText(wnd, str_to_wstr("#> -NO STATS FOUND- " + windowtext + " Elapsed time: " + milliseconds_to_hms((int)((double)clock() - start_time)) + " Approx rolls: " + int_to_str((int)roll_count) + " <#").c_str());

			this_thread::sleep_for(chrono::milliseconds(200));
			send_timer_click(item_pt, 0, 0, 0, wnd);
			continue;
		}

		if (data.prev_item_stats.size() == item_stats.size()) {
			if (compare_item_stats_to_file_stats(item_stats, data.prev_item_stats)) {
				same_stats_count++;
				if (same_stats_count > ocr_same_stats_timout * 5) {
					if (ocr_title_display)
						SetWindowText(wnd, str_to_wstr("#> -EMERGENCY STOP- (SAME STATS FOUND) " + windowtext + " Elapsed time: " + milliseconds_to_hms((int)((double)clock() - start_time)) + " Approx rolls: " + int_to_str((int)roll_count) + " <#").c_str());
					break;
				}
				cout << "same stats found (" << same_stats_count << "/" << ocr_same_stats_timout * 5 << ")" << '\n';
			}
			else {
				same_stats_count = 0;
			}
		}
		else {
			same_stats_count = 0;
		}
		data.prev_item_stats = item_stats;

		match = compare_item_stats_to_file_stats(item_stats, stats_in_file_copy);

		if (!match) {
			no_stats_count = 0;
			roll_count++;
			send_left_click(transmutept, 0, wnd);
			if (ocr_title_display)
				SetWindowText(wnd, str_to_wstr("#> -ROLLING- " + windowtext + " Elapsed time: " + milliseconds_to_hms((int)((double)clock() - start_time)) + " Approx rolls: " + int_to_str((int)roll_count) + " <#").c_str());
			this_thread::sleep_for(chrono::milliseconds(200));
			send_timer_click(item_pt, 0, 0, 0, wnd);
		}
	}
	double total_time = (double)clock() - start_time;
	if (match) {
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(WTF), NULL, SND_ASYNC | SND_RESOURCE);
		std::cout << "----------------------------------\n";
		std::cout << "match found!" << '\n';
		for (auto a : stats_in_file_copy)
			cout << "stats in file: " << a << '\n';
		if (ocr_title_display)
			SetWindowText(wnd, str_to_wstr("#> -MATCH FOUND- " + windowtext + " Elapsed time: " + milliseconds_to_hms((int)total_time) + " Approx rolls: " + int_to_str((int)roll_count) + " <#").c_str());
	}
	else {
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
	}
	std::cout << "Time spent rolling: " << milliseconds_to_hms((int)total_time) << "\nAppox # of rolls: " << roll_count << '\n';
	std::cout << "----------------------------------\n";

	if (AUTOROLL.on != 1)
		MoveWindow(wnd, rcstartpos.left, rcstartpos.top, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, false);

	if ((GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYERED) && layered_window_fix)
		SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
}

bool process(BYTE *newbuf, int x, int y, string outfile /* = "process.bmp"*/, bool OCR_visualization/* = false*/) {
	static int brightness = 0;
	static int contrast = 225;
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			int red = pos;
			int green = pos + 1;
			int blue = pos + 2;
			if (!(newbuf[red] == 80, newbuf[green] == 80, newbuf[blue] == 172) && ((OCR_visualization) ?
				!(newbuf[red] == 255 && newbuf[green] == 0 && newbuf[blue] == 0) &&
				!(newbuf[red] == 0 && newbuf[green] == 255 && newbuf[blue] == 0) &&
				!(newbuf[red] == 255 && newbuf[green] == 255 && newbuf[blue] == 0) :
				true)) {
				newbuf[red] = 0;
				newbuf[green] = 0;
				newbuf[blue] = 0;
			}
			if ((j == (3 * x - 3)) || (i == y - 1)) {
				newbuf[pos] = 0;
				newbuf[pos + 1] = 0;
				newbuf[pos + 2] = 0;
			}
		}
	}
	if (outfile.size() > 0) {
		array_to_bmp(outfile, newbuf, x, y);
	}
	return true;
}

vector<int> find_lines(BYTE* newbuf, int x, int y) {
	vector<int> lines;
	int top = 1000000;
	int height = 0;
	for (int i = 0; i < y; i++) {
		if (top < 1000000)
			height++;
		int gap = 0;
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			int red = (int)newbuf[pos];
			int green = (int)newbuf[pos + 1];
			int blue = (int)newbuf[pos + 2];
			if (height >= 16) {
				top = 1000000;
				lines.push_back(i - 1);
				height = 0;
			}
			if (!(red == 80 && green == 80 && blue == 172)) {
				gap++;
				if (gap >= x - 20 && i > top) {
					top = 1000000;
					lines.push_back(i - 1);
					height = 0;
				}
			}
			else {
				if (i < top) {
					top = i;
					lines.push_back(top);
				}
			}
		}
	}
	return lines;
}
vector<int> isolate_letters(BYTE* newbuf, int width, int top, int bottom) {
	vector<int> gaps;
	int l = 1000000, r = 0, t = 1000000, b = 0;
	int gap_count = 0;
	for (int i = 0; i < width * 3; i += 3) {//crops to left right top and bottom
		int gap = 0;
		for (int j = top * width * 3 + i; j < (width * 3 * (bottom + 1)); j += (width * 3)) {
			int red = (int)newbuf[j];
			int green = (int)newbuf[j + 1];
			int blue = (int)newbuf[j + 2];
			if (red == 80 && green == 80 && blue == 172 && red != 171) {
				if (i / 3 < l)
					l = i / 3;
				r = i / 3 + 1;
				if ((j - i) / (width * 3) < t)
					t = (j - i) / (width * 3);
				if ((j - i) / (width * 3) + 1 > b)
					b = (j - i) / (width * 3) + 1;
			}
			else
				gap++;
			if (gap == bottom + 1 - top && ocr_spaces_on) {
				gap_count++;
				if (gap_count > 8) {
					gaps.push_back(-1); gaps.push_back(-1); gaps.push_back(-1); gaps.push_back(-1);
					gap_count = 0;
				}
			}
			if (gap == bottom + 1 - top && r) {
				gaps.push_back(l); gaps.push_back(r); gaps.push_back(t); gaps.push_back(b);
				l = 1000000, r = 0, t = 1000000, b = 0;
				gap_count = 0;
			}
		}
	}
	if (gaps.size() > 0 && ocr_spaces_on) {
		while (gaps[0] == -1)
			gaps.erase(gaps.begin(), gaps.begin() + 1);
		while (gaps[gaps.size() - 1] == -1)
			gaps.erase(gaps.end() - 1, gaps.end());
	}
	return gaps;
}
bool match(vector<int> a, vector<BYTE> &charset) {
	UINT size = charset[1];
	if (size != a.size())
		return false;
	UINT size1 = a.size() - 1;
	for (UINT i = 0; i < size1; i++)
		if (a[i] != charset[i + 2])
			return false;
	return true;
}
vector<string> ocr(BYTE* newbuf, int x, int y, vector<vector<BYTE>> &charset, bool OCR_visualization/* = false*/) {
	vector<string> item_stats;
	vector<int> lines = find_lines(newbuf, x, y);//finds top and bottom row coordinates for each line of text
	UINT lines_size = lines.size();
	for (UINT l = 0; l < lines_size; l += 2) {//go through each line and isolate the letters
		vector<int> letters = isolate_letters(newbuf, x, lines[l], lines[l + 1]);
		vector<vector<int>> let;
		vector<int> temp;
		if (letters.size() == 0)
			continue;
		UINT letters_size = letters.size();
		for (UINT a = 0; a < letters_size; a += 4) {//puts each isolated letter into its own array. ex. (Capital H)
			int left = letters[a];						//0, x, x, x, 4,
			int right = letters[a + 1];					//5, x, x, x, 9,
			int top = letters[a + 2];					//10,11,12,13,14,
			int bottom = letters[a + 3];				//15,x, x, x, 19,
			temp.push_back(right - left);				//20,x, x, x, 24
			temp.push_back(bottom - top);				//in array it would be: {5(width), 5(height), 0,4,5,9,10,11,12,13,14,15,19,20,24}
			for (int i = top; i < bottom; i++) {
				for (int j = left * 3; j < 3 * right; j += 3) {
					int pos = i * 3 * x + j;
					int red = (int)newbuf[pos];
					int green = (int)newbuf[pos + 1];
					int blue = (int)newbuf[pos + 2];

					if (OCR_visualization) {
						int newpos;
						if (i == top && i > 0) {
							newpos = (i - 1) * 3 * x + j;
							newbuf[newpos + 0] = 255;
							newbuf[newpos + 1] = 255;
							newbuf[newpos + 2] = 0;
						}
						if (i == bottom - 1 && i < y) {
							newpos = (i + 1) * 3 * x + j;
							newbuf[newpos + 0] = 255;
							newbuf[newpos + 1] = 255;
							newbuf[newpos + 2] = 0;
						}
						if (j == left * 3 && j > 0) {
							newpos = i * 3 * x + (j - 3);
							newbuf[newpos + 0] = 255;
							newbuf[newpos + 1] = 255;
							newbuf[newpos + 2] = 0;
						}
						if (j == 3 * right - 3 && j < x * 3) {
							newpos = i * 3 * x + (j + 3);
							newbuf[newpos + 0] = 255;
							newbuf[newpos + 1] = 255;
							newbuf[newpos + 2] = 0;
						}
					}

					if (red == 80 && green == 80 && blue == 172) {
						temp.push_back((pos / 3) - (x * i) - left + ((right - left) * (i - top)));
					}
				}
			}
			let.push_back(temp);
			temp.erase(temp.begin(), temp.end());
		}
		string tline = "";
		UINT size1 = let.size();
		UINT size2 = charset.size();
		for (UINT i = 0; i < size1; i++) {
			for (UINT j = 0; j < size2; j++) {
				if (match(let[i], charset[j])) {
					tline += (char)charset[j][0];
					break;
				}
			}
		}
		if (tline.size() > 0) {
			char start = tline[0];
			char end = tline[tline.size() - 1];
			while (tline.size() > 0 && (start == ' ' || start == '-' || start == ':' || start == ',' || start == '\'')) {
				tline.erase(tline.begin(), tline.begin() + 1);
				start = tline[0];
			}
			while (tline.size() > 0 && (end == ' ' || end == '-' || end == ':' || end == ',' || end == '\'')) {
				tline.erase(tline.end() - 1, tline.end());
				end = tline[tline.size() - 1];
			}
			if (tline.size() > 1)
				item_stats.push_back(tline);
		}


		if (OCR_visualization) {
			for (int ln = 0; ln < 3 * x; ln += 3) {
				int ps1 = (lines[l + 0] - 1) * 3 * x + ln;
				if (lines[l] > 0) {
					newbuf[ps1 + 0] = 0;
					newbuf[ps1 + 1] = 255;
					newbuf[ps1 + 2] = 0;
				}
				int ps2 = (lines[l + 1] + 1) * 3 * x + ln;
				if (lines[l + 1] < y) {
					newbuf[ps2 + 0] = 255;
					newbuf[ps2 + 1] = 0;
					newbuf[ps2 + 2] = 0;
				}
			}
		}
	}
	if (OCR_visualization)
		array_to_bmp("(ocr)capture.bmp", newbuf, x, y);
	return item_stats;
}