#pragma once

#include <Windows.h>
#include <string>
#include <map>

#include "d2data.h"
#include "tabcontrolhandler.h"

#pragma warning(disable: 4503)

const TCHAR className[] = TEXT("TokenChest");

BOOL CALLBACK FinderDialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TabPage1Proc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TabPage2Proc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TabPage3Proc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TradeTabPage1Proc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TradeTabPage2Proc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);

extern HINSTANCE g_hInst;

extern HHOOK kbhook;

extern TabClass g_TAB;
extern TabClass g_Tradetab;
extern HWND g_finder;
extern HWND g_ttab3;
extern HWND g_ttab3_1;

extern int g_is_auto_log;

extern BOOL g_use_cust_color;
extern const COLORREF g_cust_color;
extern HBRUSH g_h_cust_color;

extern HFONT g_boldfont;
 
extern std::vector<ItemCode> g_itemcodes;
extern std::vector<CharacterData> g_characters;
extern std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, std::vector<ItemData*>>>>> g_realms;

void write_itemcodes(std::vector<ItemCode> &itemcodes);

void ShowStatus(std::string status);


