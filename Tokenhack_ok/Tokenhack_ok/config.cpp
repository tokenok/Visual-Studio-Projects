#include "config.h"

wstring Config::path;



Config::Config(wstring path) {
	this->path = path;
}

/////////////////////////////////////////////////////

POINT Config::transmute_btn_pos;
POINT Config::read_transmute_btn_pos() {
	string svalue = Read("Auto Roll", "Transmute Button Position", "0,0");
	vector<string> split = split_str(svalue, ",");
	transmute_btn_pos.x = str_to_int(split[0]);
	transmute_btn_pos.y = str_to_int(split[1]);
	return transmute_btn_pos;
}
void Config::write_transmute_btn_pos(POINT pt) {
	Write("Auto Roll", "Transmute Button Position", int_to_str(pt.x) + "," + int_to_str(pt.y));
	transmute_btn_pos = pt;
}

/////////////////////////////////////////////////////

string Config::gamepath;
string Config::read_gamepath() {
	gamepath = Read("Run Game", "Game Path", "");
	return gamepath;
}
void Config::write_gamepath(string path) {
	Write("Run Game", "Game Path", path);
	gamepath = path;
}

/////////////////////////////////////////////////////

string Config::targetlines;
string Config::read_targetlines() {
	targetlines = Read("Run Game", "target lines", "");
	return targetlines;
}
void Config::write_targetlines(string target) {
	Write("Run Game", "target lines", target);
	targetlines = target;
}

/////////////////////////////////////////////////////

bool Config::showconsole;
bool Config::read_showconsole() {
	showconsole = (bool)str_to_int(Read("Misc", "Show Console", "0"));
	return showconsole;
}
void Config::write_showconsole(bool show) {
	Write("Misc", "Show Console", int_to_str((int)show));
	showconsole = show;
}

/////////////////////////////////////////////////////

bool Config::showonstart;
bool Config::read_showonstart() {
	showonstart = (bool)str_to_int(Read("Misc", "Visible on Startup", "1"));
	return showonstart;
}
void Config::write_showonstart(bool show) {
	Write("Misc", "Visible on Startup", int_to_str((int)show));
	showonstart = show;
}

/////////////////////////////////////////////////////

/////////////////////////////////////////////////////

bool Config::Load() {
	tokenhackfunction::read_all();

	read_transmute_btn_pos();
	read_gamepath();
	read_targetlines();
	read_showconsole();
	read_showonstart();

	return true;
}

string Config::Read(string app_name, string key_name, string default_value) {
	wchar_t value[255];
	GetPrivateProfileString(STW(app_name), STW(key_name), STW(default_value), value, 255, g_config_path.c_str());
	string svalue = wstr_to_str(value);
	if (svalue == default_value) Write(app_name, key_name, default_value);
	return svalue;
}

bool Config::Write(string app_name, string key_name, string value) {
	return WritePrivateProfileString(STW(app_name), STW(key_name), STW(value), g_config_path.c_str());
}
