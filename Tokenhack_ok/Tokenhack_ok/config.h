#pragma once

#include <Windows.h>

#include "tokenhackfunction.h"

#include "C:\CPPlibs\common\common.h"

const wstring g_config_path = str_to_wstr(getexedir() + "\\config.ini");

class Config {
	public:
	static wstring path;

	static POINT transmute_btn_pos;
	static POINT read_transmute_btn_pos();
	static void write_transmute_btn_pos(POINT pt);

	static string gamepath;
	static string read_gamepath();
	static void write_gamepath(string gamepath);

	static string targetlines;
	static string read_targetlines();
	static void write_targetlines(string tarread);

	static bool showconsole;
	static bool read_showconsole();
	static void write_showconsole(bool show);

	static bool showonstart;
	static bool read_showonstart();
	static void write_showonstart(bool show);

	Config(wstring path);

	static bool Load();	

private:
	static bool Write(string app_name, string key_name, string value);

	static string Read(string app_name, string key_name, string default_value);
};