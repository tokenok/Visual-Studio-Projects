#include <iostream>
#include <algorithm>
#include <time.h>
#include <thread>
#include <fstream>

#include "d2funcs.h"
#include "window.h"
#include "keypress.h"
#include "image.h"
#include "common.h"
#include "resource.h"
#include "ocr.h"

using namespace std;

int transmutebuttonx = -1;
int transmutebuttony = -1;

string gamedir;
string gamepath;
string targetlines = "";

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

void d2type(string s, HWND wnd, UINT entertime, UINT keytime) {
	if (entertime)
		this_thread::sleep_for(chrono::milliseconds(entertime));
	keydownup(VK_RETURN);
	if (entertime)
		this_thread::sleep_for(chrono::milliseconds(entertime));
	sendkeys(s, wnd, keytime);
	keydownup(VK_RETURN);
}
void d2type(vector<string> &v, HWND wnd, UINT entertime, UINT keytime) {
	for (auto a: v) {
		d2type(a, wnd, entertime, keytime);
	}
}

bool get_transmute_pos(HWND wnd, LONG* x, LONG* y) {
	RECT client = getclientrect(wnd);
	if (TBTN_HOOKOUTPUT.toggle_state) {
		POINT client_cursor = getclientcursorpos(wnd);
		std::cout << "current client cursor pos: " << client_cursor.x << " " << client_cursor.y << '\n';
		std::cout << "current screen resolution: " << client.right << " " << client.bottom << '\n';
	}
	if (transmutebuttonx >= 0 && transmutebuttony >= 0) {
		*x = transmutebuttonx; *y = transmutebuttony;
	}
	else if (client.right == 640 && client.bottom == 480) {
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
	else
		return false;
	return true;
}
bool get_transmute_pos(HWND wnd, POINT* pt) {
	return get_transmute_pos(wnd, &pt->x, &pt->y);
}

void load_stats_and_colors_from_file() {
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
			line.erase(remove(line.begin(), line.end(), '#'), line.end());
			transform(line.begin(), line.end(), line.begin(), ::tolower);
			string var_name = line.substr(0, line.find(':'));
			var_name = var_name.substr(0, var_name.find('['));
			var_name = var_name.substr(0, var_name.find('('));
			if (line.size() > 0 && line[0] == '>') {
				line.erase(0, 1);
				if (line.size() == 0)
					continue;
				stats_in_file.push_back(line);
			}
			/*if (val.size() > 0 && val[0] == '>') {
				val.erase(0, 1);
				if (val.size() == 0)
					continue;
				stats_in_file.push_back(val);
			}*/
			for (UINT i = 0; i < statcolors.size();i++) {
				if (var_name == statcolors[i]->text_in_file) {
					statcolors[i]->red = str_to_int(val.substr(0, val.find(',')));
					statcolors[i]->green = str_to_int(val.substr(val.find(',') + 1, val.rfind(',') - val.find(',') - 1));
					statcolors[i]->blue = str_to_int(val.substr(val.rfind(',') + 1, val.size()));
					break;
				}
			}
		}
		filein.close();
	}
}


/*
>cold $ 11						*wildcard sequence ($)
>cold resist +1?%				*wildcard (?)
>cold | lightning				*OR (|)
>resist ! poison !fire			*NOT (!)
>>								*new item
<xyz>							*set of chars
>^								*regex
*/

bool compare_item_stats_to_file_stats(vector<string>& ocr_stats, vector<string>& file_stats, const bool& fast_skip/* = true*/) {
	for (UINT j = 0; j < ocr_stats.size(); j++) {
		ocr_stats[j].erase(remove(ocr_stats[j].begin(), ocr_stats[j].end(), ' '), ocr_stats[j].end());
		transform(ocr_stats[j].begin(), ocr_stats[j].end(), ocr_stats[j].begin(), tolower);
	}
	
	vector<vector<string>> items_in_file = split_vec(file_stats, ">");

	for (auto current_item: items_in_file) {
		UINT match_count = 0;
		for (auto current_line: current_item) {
			bool match = false;
			vector<string> ORs_of_current_line = split_str(current_line, "|");
			for (auto current_OR: ORs_of_current_line) {				
				for (auto & j: ocr_stats) {//wtf is this coloring
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

	vector<string> stats_in_file_copy = stats_in_file;

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

