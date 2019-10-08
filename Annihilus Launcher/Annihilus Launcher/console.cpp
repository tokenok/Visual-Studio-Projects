#include <Windows.h>
#include <string>
#include <iostream>

#include "console.h"

void console_class::Console::setConsoleColor(BYTE attribute) {
	text_attribute = attribute;

	setConsoleColor();
}
void console_class::Console::setConsoleColor() {
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	//save original attributes
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	if (!original_text_attributes) 		
		original_text_attributes = csbi.wAttributes;

	previous_text_attributes = csbi.wAttributes;
	
	SetConsoleTextAttribute(hstdout, text_attribute);

	FlushConsoleInputBuffer(hstdout);
}

void console_class::Console::restoreOriginalAttributes() {
	if (original_text_attributes) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), original_text_attributes);
		original_text_attributes = 0;
	}
}
void console_class::Console::restorePreviousAttributes() {
	if (previous_text_attributes) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), previous_text_attributes);
		previous_text_attributes = 0;
	}
}

void console_class::Console::show(bool no_close) {
	if (!show_state && !on_state) {
		std::cout.clear();
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONOUT$", "w", stdout);
		if (no_close) {
			HWND hwnd = GetConsoleWindow();
			HMENU hmenu = GetSystemMenu(hwnd, FALSE);
			EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
		}
	}
	else if (on_state && !show_state) {
		HWND hcon = GetConsoleWindow();
		ShowWindow(hcon, SW_SHOW);
	}
	show_state = true;
	on_state = true;
}
void console_class::Console::hide() {
	show_state = false;
	HWND hcon = GetConsoleWindow();
	ShowWindow(hcon, SW_HIDE);
}
void console_class::Console::close() {
	on_state = false;
	show_state = false;
	FreeConsole();
}

void console_class::Console::setFont(std::string font_face_name, SHORT size/* = 12*/, UINT weight/* = FW_NORMAL*/) {
	CONSOLE_FONT_INFOEX font = {0};
	font.cbSize = sizeof(font);
	font.dwFontSize.Y = size;
	font.FontWeight = weight;
	wcscpy(font.FaceName, std::wstring(font_face_name.begin(), font_face_name.end()).c_str());

	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &font);
}
void console_class::Console::setTitle(std::string text) {
	SetConsoleTitle(std::wstring(text.begin(), text.end()).c_str());
}

