#include "common.h"

#include <sstream>
#include <iostream>

using namespace std;

//convert string to int
int str_to_int(string str) {
	stringstream out(str);
	int num;
	out >> (str.find("0x") != string::npos ? hex : dec) >> num;
	return num;
}
//convert int to string
string int_to_str(int num) {
	stringstream out;
	out << num;
	string return_value = out.str();
	return return_value;
}
//convert wide string to string
string wstr_to_str(wstring ws) {
	string ret(ws.begin(), ws.end());
	return ret;
}
//convert string to wide string
wstring str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}
//LEAKS + warning
LPWSTR str_to_LPWSTR(std::string s) {
	LPWSTR m = new wchar_t[s.size() + 1];
	copy(s.begin(), s.end(), m);
	m[s.size()] = '\0';
	return m;
}
//gets the text of a window
string getwindowtext(HWND wnd) {
	wchar_t text[256];
	GetWindowText(wnd, text, 256);
	return wstr_to_str(text);
}
//takes a string and a delimiter and finds all the strings septerated by the delimiter and places them into a vector
vector<string> split_str(string str, const string& delimiter, int minlen/* = -1*/) {
	vector<string> ret;
	unsigned int pos = 0;
	string token;
	while ((pos = str.find(delimiter)) != std::string::npos && str.size()) {
		token = str.substr(0, pos);
		if ((int)token.size() >= minlen)
			ret.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	if ((int)str.size() > 0)
		ret.push_back(str);
	return ret;
}
//attaches console to current process (allows cin and cout in win32 environment)
void SHOW_CONSOLE(bool show, bool noclose) {
	static bool show_state = false;
	if (show && !show_state) {
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONIN$", "r", stdin);
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
		if (noclose)
			EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}
	if (!show)
		FreeConsole();
	show_state = show;
}
//converts input string into time structure (input format: MM/DD/YYYY HH:MM:SS)
tm string_time_to_struct_time(string s) {
	//split date and time (should be separated by space)
	vector<string> Space = split_str(s, " ");
	if (Space.size() < 2)
		return {0};

	//get the date and time parts
	vector<string> date = split_str(Space[0], "/");	
	vector<string> time = split_str(Space[1], ":");

	//check if date and time have all the valid numbers (date has month, day, and year; time has hours and minutes, may also have seconds)
	if (date.size() < 3 || time.size() < 2)
		return {0};

	tm t;
	//set date
	t.tm_mon = str_to_int(date[0]);
	t.tm_mday = str_to_int(date[1]);
	t.tm_year = str_to_int(date[2]);
	//set time
	t.tm_hour = str_to_int(time[0]);
	t.tm_min = str_to_int(time[1]);

	//check if input time includes seconds, if it does assing them, otherwise use 0
	if (time.size() == 3)
		t.tm_sec = str_to_int(time[2]);
	else
		t.tm_sec = 0;

	//if year is input as '15' instead of '2015', just add 2000 to get '2015'
	if (t.tm_year < 1000)
		t.tm_year += 2000;

	return t;
}

//foramts time struct info into a string for display  (output format: MM/DD/YYYY HH:MM:SS)
string struct_time_to_string_time(tm time) {
	string a1 = int_to_str(time.tm_mon);
	string a2 = int_to_str(time.tm_mday);
	string a3 = int_to_str(time.tm_year);
	string a4 = int_to_str(time.tm_hour);
	string a5 = int_to_str(time.tm_min); 
	string a6 = int_to_str(time.tm_sec);

	if (a1.size() < 2)
		a1 = "0" + a1;
	if (a2.size() < 2)
		a2 = "0" + a2;
	if (a3.size() == 2)
		a3 = "20" + a3;
	if (a3.size() == 1)
		a3 = "200" + a3;
	if (a4.size() < 2)
		a4 = "0" + a4;
	if (a5.size() < 2)
		a5 = "0" + a5;
	if (a6.size() < 2)
		a6 = "0" + a6;

	return a1 + "/" + a2 + "/" + a3 + " " + a4 + ":" + a5 + ":" + a6;
}
//compares two flights departure dates, returns true if left is earlier than right (used in std::sort as callback)
bool compare_flight_time(const Flight* a, const Flight* b) {
	tm ta = a->get_depTime();
	tm tb = b->get_depTime();
	return compare_time(&ta, &tb);
}
//compares two time structures, returns true if left is earlier than right
bool compare_time(const tm* ta, const tm* tb) {
	//check year first
	if (ta->tm_year != tb->tm_year) {
		return ta->tm_year < tb->tm_year;
	}
	//then check month
	if (ta->tm_mon != tb->tm_mon) {
		return ta->tm_mon < tb->tm_mon;
	}
	//then check day
	if (ta->tm_mday != tb->tm_mday) {
		return ta->tm_mday < tb->tm_mday;
	}
	//then hour
	if (ta->tm_hour != tb->tm_hour) {
		return ta->tm_hour < tb->tm_hour;
	}
	//then minutes
	if (ta->tm_min != tb->tm_min) {
		return ta->tm_min < tb->tm_min;
	}
	//then seconds
	if (ta->tm_sec != tb->tm_sec) {
		return ta->tm_sec < tb->tm_sec;
	}

	//they are basically the same time (dont care about ms)
	return false;
}