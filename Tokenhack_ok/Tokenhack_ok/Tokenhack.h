#pragma once

#include <Windows.h>

#include "C:\CPPlibs\common\common.h"

#include "config.h"
#include "customcontrols.h"
#include "tokenhackfunction.h"
#include "resource.h"

#define VERSION L"TokenHack_v3.0"

const TCHAR className[] = TEXT("Tokenhack");
const UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

extern tokenhackfunction READ;
extern tokenhackfunction AUTOROLL;
extern CustomTrackbar CTB_VOLUME;
extern ToggleButton TBTN_HOOKOUTPUT;

extern HWND g_hwnd;

extern HWND g_statbox;

void statbox_display(HWND hwnd, bool skiptostatsinfile  = false);

extern HFONT font_a;

extern HCURSOR idle_cursor;
extern HCURSOR press_cursor;
extern HCURSOR loading_cursor;
extern HCURSOR token_cursor;

#define WM_TRAYICON (WM_USER + 0)
#define CURSOR_IDLE (WM_USER + 1)
#define CURSOR_PRESS (WM_USER + 2)
#define CURSOR_LOADING (WM_USER + 3)

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hotkeydlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void TOKENHACK_CHECKBOX_PROC(WPARAM wParam, LPARAM /*lParam*/);
void TOKENHACK_HOTKEYBUTTON_PROC(WPARAM wParam, LPARAM /*lParam*/);

void d2type(std::string s, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0);
void d2type(std::vector<std::string> &v, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0);

void Resize();
