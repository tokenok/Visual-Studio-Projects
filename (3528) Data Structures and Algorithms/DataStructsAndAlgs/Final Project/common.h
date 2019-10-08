#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <string>
#include <vector>

#include "flight.h"

#define STW(s) str_to_wstr(s).c_str()
#define ITW(i) str_to_wstr(int_to_str(i)).c_str()

int str_to_int(std::string str);
std::string int_to_str(int num);
std::string wstr_to_str(std::wstring ws);
std::wstring str_to_wstr(std::string s);
LPWSTR str_to_LPWSTR(std::string s);

std::vector<std::string> split_str(std::string str, const std::string& delimiter, int minlen = -1);

std::string getwindowtext(HWND wnd);

void SHOW_CONSOLE(bool show, bool noclose = false);

std::tm string_time_to_struct_time(std::string s);
std::string struct_time_to_string_time(std::tm time);
bool compare_flight_time(const Flight* a, const Flight* b);
bool compare_time(const tm* ta, const tm* tb);

#endif

