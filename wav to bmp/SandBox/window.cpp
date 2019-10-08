#include <fstream>
#include <algorithm>
#include <time.h>
#include <iostream>

#include "window.h"
#include "resource.h"
#include "common.h"

#pragma comment(lib, "user32.lib")

using namespace std;

const UINT WM_customtrackbar_VALUE_CHANGED = RegisterWindowMessageA("custom trackbar value changed");
const UINT WM_customedit_TEXT_CHANGED = RegisterWindowMessageA("custom edit text changed");
const UINT WM_KBHOOKKEYDOWN = RegisterWindowMessageA("keyboard hook keydown message");

const TCHAR tabbuttonclassName[] = TEXT("tabbuttonclassName");
const TCHAR autoradiobuttonclassName[] = TEXT("autoradiobuttonclassName");
const TCHAR checkboxbuttonclassName[] = TEXT("checkboxbuttonclassName");
const TCHAR normalbuttonclassName[] = TEXT("normalbuttonclassName");
const TCHAR togglebuttonclassName[] = TEXT("togglebuttonclassName");
const TCHAR staticcontrolclassName[] = TEXT("staticcontrolclassName");
const TCHAR EditControlclassName[] = TEXT("EditControlclassName");
const TCHAR customtrackbarclassName[] = TEXT("customtrackbarclassName");
const TCHAR customcomboboxclassName[] = TEXT("customcomboboxclassName");
const TCHAR CustomContextMenuclassName[] = TEXT("CustomContextMenuclassName");
const TCHAR CustomScrollbarclassName[] = TEXT("CustomScrollbarclassName");

UINT get_unique_id() {
	static int unique_id = 0;
	return ++unique_id;
}

bool CreateChildren(HWND parent) {
	return custom_controls.createChildren(parent);
}

#define WM_MOUSEENTER (WM_USER)

CustomControls custom_controls;

#pragma region BasicControl + colorscheme + hover: implementation
	#pragma region colorscheme
	BasicControl_colorscheme::BasicControl_colorscheme() {};

	//statics
	BasicControl_colorscheme::BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on) {
		this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
		this->background_hover_on = background_idle_on; this->border_hover_on = border_idle_on; this->text_hover_on = text_idle_on;
		//this->background_selected_on = background_idle_on; this->border_selected_on = border_idle_on; this->text_selected_on = text_idle_on;
		this->background_idle_off = background_idle_on; this->border_idle_off = border_idle_on; this->text_idle_off = text_idle_on;
		this->background_hover_off = background_idle_on; this->border_hover_off = border_idle_on; this->text_hover_off = text_idle_on;
		//this->background_selected_off = background_idle_on; this->border_selected_off = border_idle_on; this->text_selected_off = text_idle_on;
	}

	//buttons etc
	BasicControl_colorscheme::BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on)
		//,COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on);
	{

		this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
		this->background_hover_on = background_hover_on; this->border_hover_on = border_hover_on; this->text_hover_on = text_hover_on;
		//this->background_selected_on = background_selected_on; this->border_selected_on = border_selected_on; this->text_selected_on = text_selected_on;
		this->background_idle_off = background_idle_on; this->border_idle_off = border_idle_on; this->text_idle_off = text_idle_on;
		this->background_hover_off = background_hover_on; this->border_hover_off = border_hover_on; this->text_hover_off = text_hover_on;
		//this->background_selected_off = background_selected_on; this->border_selected_off = border_selected_on; this->text_selected_off = text_selected_on;
	}

	//buttons with toggle state
	BasicControl_colorscheme::BasicControl_colorscheme(
		COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on,
		COLORREF background_hover_on, COLORREF border_hover_on, COLORREF text_hover_on,
		//COLORREF background_selected_on, COLORREF border_selected_on, COLORREF text_selected_on,
		COLORREF background_idle_off, COLORREF border_idle_off, COLORREF text_idle_off,
		COLORREF background_hover_off, COLORREF border_hover_off, COLORREF text_hover_off)
		//COLORREF background_selected_off, COLORREF border_selected_off, COLORREF text_selected_off);
	{

		this->background_idle_on = background_idle_on; this->border_idle_on = border_idle_on; this->text_idle_on = text_idle_on;
		this->background_hover_on = background_hover_on; this->border_hover_on = border_hover_on; this->text_hover_on = text_hover_on;
		//this->background_selected_on = background_selected_on; this->border_selected_on = border_selected_on; this->text_selected_on = text_selected_on;
		this->background_idle_off = background_idle_off; this->border_idle_off = border_idle_off; this->text_idle_off = text_idle_off;
		this->background_hover_off = background_hover_off; this->border_hover_off = border_hover_off; this->text_hover_off = text_hover_off;
		//this->background_selected_off = background_selected_off; this->border_selected_off = border_selected_off; this->text_selected_off = text_selected_off;
	}
	#pragma endregion
	#pragma region BasicControl

	BasicControl* getBasicControl(HWND wnd) {
		LONG_PTR ptr = GetWindowLongPtr(wnd, GWL_USERDATA);
		return reinterpret_cast<BasicControl*>(ptr);
	}

	BasicControl::BasicControl() {}

	HWND BasicControl::Create(HWND parent) {
		handle = CreateWindowEx(window_exstyles, str_to_wstr(className).c_str(), str_to_wstr(window_text).c_str(), 
			window_styles, xpos, ypos, width, height,
			parent, (HMENU)id, NULL, this);
	/*	for (auto & a : custom_controls.tabbuttons) {//tabs
			if (a->this_name == owner_name)
				a->page_windows.push_back(handle);
		}	*/	
		return handle;
	}

	void BasicControl::Show() {
		SetWindowLong(handle, GWL_STYLE, GetWindowLongPtr(handle, GWL_STYLE) | WS_VISIBLE);
		ShowWindow(handle, SW_SHOW);
		EnableWindow(handle, 1);
	}

	void BasicControl::Hide() {
		if (GetWindowLongPtr(handle, GWL_STYLE) & WS_VISIBLE)
			SetWindowLong(handle, GWL_STYLE, GetWindowLongPtr(handle, GWL_STYLE) ^ WS_VISIBLE);
		ShowWindow(handle, SW_HIDE);
		EnableWindow(handle, 0);
	}
	#pragma endregion
	#pragma region hover
	BasicControl_hover::BasicControl_hover() {}
	BasicControl_hover::BasicControl_hover(std::string hover_text, DWORD Flags, DWORD HoverTime) {
		this->hover_text = hover_text;
		this->Flags = Flags;
		this->HoverTime = HoverTime;
	}
	#pragma endregion
#pragma endregion

#pragma region color schemes, fonts, styles, hover info
	#pragma region color schemes
	BasicControl_colorscheme togglebutton_scheme(
		RGB(10, 10, 10), RGB(0, 255, 0), RGB(0, 255, 0),			//IDLE ON: background/border/text
		RGB(45, 45, 45), RGB(0, 255, 0), RGB(0, 255, 0),			//HOVER ON: background/border/text
		RGB(10, 10, 10), RGB(255, 0, 0), RGB(255, 0, 0),			//IDLE OFF: background/border/text
		RGB(45, 45, 45), RGB(255, 0, 0), RGB(255, 0, 0)				//HOVER OFF: background/border/text
		);
	BasicControl_colorscheme normalbutton_scheme(
		RGB(10, 10, 10), RGB(0, 117, 255), RGB(0, 117, 255), 		//IDLE: background/border/text
		RGB(25, 25, 25), RGB(0, 117, 255), RGB(0, 117, 255)			//HOVER: background/border/text	
		);
	BasicControl_colorscheme stattab_scheme(
		RGB(0, 118, 255), RGB(0, 118, 255), RGB(10, 10, 10),		//IDLE ON: background/border/text
		RGB(0, 118, 255), RGB(0, 118, 255), RGB(10, 10, 10),		//HOVER ON: background/border/text
		RGB(10, 10, 10), RGB(0, 118, 255), RGB(0, 118, 255),		//IDLE OFF: background/border/text
		RGB(0, 118, 255), RGB(10, 10, 10), RGB(10, 10, 10)			//HOVER OFF: background/border/text
		);
	#pragma endregion
	#pragma region fonts
	HFONT Font_a = NULL;
	HFONT Font_ariel = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("ariel"));
	HFONT Font_consolas = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("consolas"));
	HFONT Font_courier = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("courier"));
	#pragma endregion
	#pragma region hover_info
	BasicControl_hover default_hover("", TME_LEAVE, 0);
	BasicControl_hover tooltip_hover("default hover tooltip", TME_LEAVE | TME_HOVER, 0);
	#pragma endregion
#pragma endregion

///////////

#pragma region TabButton class

TabButton::TabButton() {}
TabButton::TabButton(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT group_name, bool add_to_list) {
	className = wstr_to_str(tabbuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK tabbuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	TabButton* a;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<TabButton*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {		
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<TabButton*>(ptr);
	}
		
	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {			
			break;
		}
		case MY_COMMAND:
		case WM_LBUTTONUP: {
			for (auto & tabbtn : custom_controls.tabbuttons) {
				if (a->group_name == tabbtn->group_name) {
					if (tabbtn->toggle_state) {
						for (auto & b : tabbtn->page_windows) {
							ShowWindow(b, SW_HIDE);
							EnableWindow(b, 0);
							if (GetWindowLongPtr(b, GWL_STYLE) & WS_VISIBLE)
								SetWindowLongPtr(b, GWL_STYLE, GetWindowLongPtr(b, GWL_STYLE) ^ WS_VISIBLE);
							RedrawWindow(b, NULL, NULL, RDW_INVALIDATE);
						}
					}
					tabbtn->toggle_state = 0;
					RedrawWindow(tabbtn->handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
			a->toggle_state = 1;
			for (auto & b : a->page_windows) {
				ShowWindow(b, SW_SHOW);
				EnableWindow(b, 1);
				SetWindowLongPtr(b, GWL_STYLE, GetWindowLongPtr(b, GWL_STYLE) | WS_VISIBLE);
				RedrawWindow(b, NULL, NULL, RDW_INVALIDATE);
			}

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;				
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{			
			a->page_windows.clear();
			a->mouse_in_client = false;
			a->focus = false;

			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_tabbutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = tabbuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = tabbuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM tabbtn = init_register_custom_tabbutton();

#pragma endregion

#pragma region RadioButton class

RadioButton::RadioButton() {}
RadioButton::RadioButton(string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam), int group,
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT group_name, bool add_to_list) {
	className = wstr_to_str(autoradiobuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->group = group;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK autoradiobuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RadioButton* a;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<RadioButton*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<RadioButton*>(ptr);
	}
	
	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {			
			break;
		}
		case WM_LBUTTONUP: {
			int group = a->group;
			UINT group_name = a->group_name;
			HWND parent = a->parent;
			vector<RadioButton*> autoradiobuttons = custom_controls.autoradiobuttons;
			for (auto & b : autoradiobuttons) {
				if (group == 0)
					break;
				if (b->group == group && b->group_name == group_name && b->parent == parent) {
					b->toggle_state = 0;
					RedrawWindow(b->handle, NULL, NULL, RDW_INVALIDATE);
				}
			}
			a->toggle_state = 1;

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;				
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_autoradiobutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = autoradiobuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = autoradiobuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM rbtn = init_register_custom_autoradiobutton();

#pragma endregion

#pragma region CheckBox class

CheckBox::CheckBox() {}
CheckBox::CheckBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, int toggle_state, UINT group_name, bool add_to_list) {
	className = wstr_to_str(checkboxbuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->toggle_state = toggle_state; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK checkboxbuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CheckBox* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CheckBox*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CheckBox*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;			
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->toggle_state)
				a->toggle_state = 0;
			else
				a->toggle_state = 1;

			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_checkboxbutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = checkboxbuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = checkboxbuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM cbtn = init_register_custom_checkboxbutton();

#pragma endregion

#pragma region Button class

Button::Button() {}
Button::Button(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT group_name, bool add_to_list) {
	className = wstr_to_str(normalbuttonclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->group_name = group_name;

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK normalbuttonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Button* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<Button*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<Button*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = (a->window_text);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return FALSE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_normalbutton() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = normalbuttonclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = normalbuttonProc;
	return RegisterClassEx(&wnd);
}
ATOM btn = init_register_custom_normalbutton();

#pragma endregion

#pragma region StaticControl class

StaticControl::StaticControl() {}
StaticControl::StaticControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, DWORD* hover_info, UINT group_name, bool add_to_list) {
	className = wstr_to_str(staticcontrolclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->group_name = group_name;

	//FIX HOVER INFO OK
	hover_info = NULL;//removes warning (all this is here for)
	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);

	if (add_to_list)
		custom_controls.addControl(this);
}

LRESULT CALLBACK staticcontrolProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	StaticControl* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<StaticControl*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<StaticControl*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			if (GetWindowLongPtr(hwnd, GWL_STYLE) & SS_CENTER)
				ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			else
				ExtTextOut(hDC, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);

			if (a->focus) {
				HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
				FrameRect(hDC, &rcclient, framebrush);
				DeleteObject(framebrush);
			}

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = a->hover_info.Flags;
				me.hwndTrack = a->handle;
				me.dwHoverTime = a->hover_info.HoverTime;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSEHOVER:{
			SIZE size;
			HDC hDC = GetDC(hwnd);
			SelectObject(hDC, a->font);
			SetMapMode(hDC, MM_TEXT);
			a->hover_info.hover_text = a->window_text;
			GetTextExtentPoint32(hDC, str_to_wstr(a->hover_info.hover_text).c_str(), a->hover_info.hover_text.length(), &size);
			if (size.cx <= a->width)
				break;

			POINT pt = getclientcursorpos(GetParent(hwnd));

			StaticControl* temp = new StaticControl;
			temp->window_exstyles = WS_EX_TOPMOST;
			temp->className = wstr_to_str(staticcontrolclassName);
			temp->window_text = a->window_text;
			temp->window_styles = WS_CHILD | WS_VISIBLE | SS_CENTER;
			temp->xpos = pt.x + 20;
			temp->ypos = pt.y;
			temp->width = size.cx;
			temp->height = size.cy;
			temp->id = get_unique_id();
			temp->client_cursor = NULL;
			temp->font = Font_a;
			temp->Proc = nullptr;
			temp->color_scheme = a->color_scheme;

			a->hover_info.hover_text = a->window_text;

			a->hover_info.handle = temp->Create(GetParent(hwnd));

			ReleaseDC(hwnd, hDC);
			break;
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			if (a->hover_info.handle)
				DestroyWindow(a->hover_info.handle);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			a->focus = false;
			a->mouse_in_client = false;

			a->handle = NULL;
			
			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_staticcontrol() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = staticcontrolclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = staticcontrolProc;
	return RegisterClassEx(&wnd);
}
ATOM stc = init_register_custom_staticcontrol();

#pragma endregion

#pragma region CustomTrackbar class

#define HTTHUMB (WM_USER + 1)

//TrackbarColorScheme c_scheme {
//	RGB(10, 10, 10), RGB(50, 50, 50), RGB(255, 0, 0), RGB(0, 0, 0),									// background/border/window text idle/highlight
//	RGB(127, 127, 127), RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255), 					// left/right channel left/right highlight
//	RGB(10, 10, 10), RGB(10, 10, 10), RGB(0, 118, 255),												// thumb background: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb border: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(10, 10, 10)											// thumb text: idle/hover/selected
//};

CustomTrackbar::CustomTrackbar(string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
	int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
	TrackbarColorScheme color_scheme, HCURSOR client_cursor, HCURSOR thumb_cursor, UINT group_name/*= 0*/, bool add_to_list) {

	className = wstr_to_str(customtrackbarclassName); handle = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;

	this->window_name = window_name; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id;
	this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
	this->small_step = small_step; this->large_step = large_step; this->thumb_size = thumb_size; this->channel_size = channel_size; this->flags = flags;
	this->tcolor_scheme = color_scheme; this->client_cursor = client_cursor; this->thumb_cursor = thumb_cursor;
	this->group_name = group_name;
	
	if (this->client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (this->thumb_cursor == NULL)
		this->thumb_cursor = LoadCursor(NULL, IDC_HAND);

	if (add_to_list)
		custom_controls.addControl(this);
}

int CustomTrackbar::setPos(int pos) {
	int x = flags & CTB_VERT ? height : width;
	return this->pos = pos <= 0 ? 0 : (int)(x - thumb_size) <= pos ? (x - thumb_size) : pos;
}

int CustomTrackbar::setVal(int pos) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	return current_val = ((pos * (max_val - min_val)) / (x - thumb_size == 0 ? 1 : (x - thumb_size))) + min_val;
}

int CustomTrackbar::setPosWithVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	return pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val);
}

int CustomTrackbar::moveThumb(int pos) {
	return setVal(setPos(pos));
}

void CustomTrackbar::moveThumbReal(int pos) {
	setPosWithVal(moveThumb(pos));
}

void CustomTrackbar::thumbHitText() {
	if (is_cursor_in_region(thumb_region, getclientcursorpos(handle))) {
		if (!thumb_hover) {
			thumb_hover = true;
			InvalidateRect(handle, &thumb_region, true);
		}
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTTHUMB, NULL));
	}
	else if (thumb_hover) {
		thumb_hover = false;
		InvalidateRect(handle, &thumb_region, true);
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTCLIENT, NULL));
	}
}

void CustomTrackbar::setFocus(bool focus) {
	this->focus = focus;
	if (focus) {
		SetFocus(handle);
		if (dragging)
			SetCapture(handle);
	}
	else {
		ReleaseCapture();
		//setfocus to new window here		
	}

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool CustomTrackbar::setRange(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	setVal(pos);

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
	return true;
}

void CustomTrackbar::redrawNewVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val == 0 ? 1 : (max_val - min_val));

	if (scrollbar_owner_handle)
		PostMessage(scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);
	PostMessage(GetParent(handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);
	RedrawWindow(handle, &tr, NULL, RDW_INVALIDATE);
}

LRESULT CALLBACK customtrackbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//tab focus

	CustomTrackbar* a;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomTrackbar*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomTrackbar*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			TrackbarColorScheme cs = a->tcolor_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//colorbackground
			HBRUSH tempbrush0 = CreateSolidBrush(cs.background);
			FillRect(hDC, &rcclient, tempbrush0);

			//calculate thumb position
			if (a->start) {
				a->setPosWithVal(a->start_val);
				a->start = false;
			}
			a->thumb_region.left = (a->flags & CTB_VERT ? rcclient.left + 1: rcclient.left + a->pos);
			a->thumb_region.top = (a->flags & CTB_VERT ? rcclient.top + a->pos : rcclient.top + 1);
			a->thumb_region.right = (a->flags & CTB_VERT ? rcclient.right - 1 : rcclient.left + a->thumb_size + a->pos);
			a->thumb_region.bottom = (a->flags & CTB_VERT ? rcclient.left + a->thumb_size + a->pos : rcclient.bottom - 1);

			//draw channel
			RECT rc_channel_left;
			RECT rc_channel_right;

			//left/top
			rc_channel_left.left = (a->flags & CTB_VERT ? rcclient.left + a->channel_size : rcclient.left);
			rc_channel_left.right = (a->flags & CTB_VERT ? rcclient.right - a->channel_size : rcclient.left + a->pos);
			rc_channel_left.top = (a->flags & CTB_VERT ? rcclient.top : rcclient.top + a->channel_size);
			rc_channel_left.bottom = (a->flags & CTB_VERT ? rcclient.top + a->pos : rcclient.bottom - a->channel_size);
			//right/bottom
			rc_channel_right.left = (a->flags & CTB_VERT ? rcclient.left + a->channel_size : rcclient.left + a->thumb_size + a->pos);
			rc_channel_right.right = (a->flags & CTB_VERT ? rcclient.right - a->channel_size : rcclient.right);
			rc_channel_right.top = (a->flags & CTB_VERT ? rcclient.top + a->pos : rcclient.top + a->channel_size);
			rc_channel_right.bottom = (a->flags & CTB_VERT ? rcclient.bottom : rcclient.bottom - a->channel_size);

			HBRUSH tempbrush1 = CreateSolidBrush(a->mouse_in_client ? cs.left_channel_highlight : cs.left_channel_idle);
			HBRUSH tempbrush2 = CreateSolidBrush(a->mouse_in_client ? cs.right_channel_highlight : cs.right_channel_idle);
			FillRect(hDC, &rc_channel_left, tempbrush1);
			FillRect(hDC, &rc_channel_right, tempbrush2);

			//draw focus rect
			if ((a->focus || a->flags & CTB_BORDER) && !(a->flags & CTB_NOBORDER)) {
				SetDCPenColor(hDC, cs.border);
				Rectangle(hDC, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);
			}

			//draw thumb
			a->thumbHitText();
			SIZE size;
			string text = a->flags & CTB_THUMBVALUE ? int_to_str(a->current_val) : "";
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), (int)text.length(), &size);
			SetTextColor(hDC, a->thumb_selected ? cs.thumb_text_selected : a->thumb_hover ? cs.thumb_text_hover : cs.thumb_text_idle);
			SetBkColor(hDC, a->thumb_selected ? cs.thumb_background_selected : a->thumb_hover ? cs.thumb_background_hover : cs.thumb_background_idle);
			ExtTextOut(hDC,
				((a->thumb_region.right - a->thumb_region.left) - size.cx) / 2 + (a->flags & CTB_VERT ? 0 : a->pos),
				((a->thumb_region.bottom - a->thumb_region.top) - size.cy) / 2 + (a->flags & CTB_VERT ? a->pos : 1),
				ETO_OPAQUE | ETO_CLIPPED, &a->thumb_region, str_to_wstr(text).c_str(), text.length(), NULL);
			SetDCPenColor(hDC, a->thumb_selected ? cs.thumb_border_selected : a->thumb_hover || a->dragging ? cs.thumb_border_hover : cs.thumb_border_idle);			
			Rectangle(hDC, a->thumb_region.left, a->thumb_region.top, a->thumb_region.right, a->thumb_region.bottom);

			//draw window name
			if (a->window_name.size() > 0) {
				SetBkMode(hDC, TRANSPARENT);
				SetTextColor(hDC, a->mouse_in_client ? cs.window_name_highlight : cs.window_name_idle);
				RECT rc = getclientrect(a->handle);
				DrawText(hDC, str_to_wstr(a->window_name).c_str(), a->window_name.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			//apply paint
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(tempbrush0);
			DeleteObject(tempbrush1);
			DeleteObject(tempbrush2);
			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			a->focus = true;
			a->mouse_in_client = true;
			a->dragging = true;
			a->thumb_hover = true;
			a->thumb_selected = true;

			a->setFocus(true);

			POINT pt = getclientcursorpos(a->handle);

			if (a->flags & CTB_SMOOTH)//smooth dragging
				a->moveThumb((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));
			else
				a->moveThumbReal((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));

			customtrackbarProc(a->handle, WM_SETCURSOR, (WPARAM)a->handle, (LPARAM)MAKELONG(HTTHUMB, NULL));

			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP: {
			a->dragging = false;
			a->thumb_selected = false;
			ReleaseCapture();
			if (!(a->flags & CTB_SMOOTH) || !(a->flags & CTB_STAY))//not smooth dragging OR not smooth stay
				a->setPosWithVal(a->current_val);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			a->CustomTrackbar::thumbHitText();

			if (a->dragging) {

				POINT pt = getclientcursorpos(a->handle);

				if (a->flags & CTB_SMOOTH)//smooth dragging
					a->moveThumb((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));
				else
					a->moveThumbReal((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));

				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_LEFT:{
					a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_DOWN:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_RIGHT:{
					a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_UP:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_HOME:{
					a->setPosWithVal(a->min_val);
					break;
				}
				case VK_END:{
					a->setPosWithVal(a->max_val);
					break;
				}
				case VK_PRIOR:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
					break;
				}
				case VK_NEXT:{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
			if (!wrongkey_skip) {
				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
				}
				else {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
				}
				else {
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
				}
			}
			else
				break;
			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_SIZE:{
			a->width = LOWORD(lParam);
			a->height = HIWORD(lParam);

			break;
		}
		case WM_MOVE:{
			a->xpos = LOWORD(lParam);
			a->ypos = HIWORD(lParam);

			break;
		}
		case WM_SETCURSOR:{
			if (a->dragging)
				lParam = MAKELPARAM(HTTHUMB, HIWORD(lParam));
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
				case HTTHUMB:
					SetCursor(a->thumb_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_KILLFOCUS:{
			a->setFocus(false);

			a->mouse_in_client = false;
			a->dragging = false;
			a->thumb_hover = false;
			a->thumb_selected = false;

			break;
		}
		case WM_MOUSELEAVE:{
			if (!a->dragging) {
				a->mouse_in_client = false;
				a->thumb_selected = false;
				a->thumb_hover = false;

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_customtrackbar() {
	WNDCLASS wnd = {0};
	wnd.lpszClassName = customtrackbarclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customtrackbarProc;
	return RegisterClass(&wnd);
}
ATOM ctb = init_register_customtrackbar();

#pragma endregion

#pragma region CustomComboBox class

CustomComboBoxOption::CustomComboBoxOption() {}
CustomComboBoxOption::CustomComboBoxOption(string text) {
	hover = false; region = {0, 0, 0, 0};
	this->text = text;
}

CustomComboBox::CustomComboBox() {}
CustomComboBox::CustomComboBox(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id,
	vector<string> options_text, UINT arrow_region_width, UINT max_options, UINT style, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, UINT group_name, bool add_to_list) {

	className = wstr_to_str(customcomboboxclassName); handle = NULL; toggle_state = true;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->group_name = group_name;

	this->style = style;
	if (style == ccb_style0)
		arrow_region_width = 0;
	this->arrow_region_width = arrow_region_width;
	this->max_options = max_options;

	this->box_region = {0, 0, width - arrow_region_width, height};
	this->arrow_region = {width - arrow_region_width, 0, width, height};
	this->arrow_region_up = {width - arrow_region_width, 0, width, height / 2};
	this->arrow_region_down = {width - arrow_region_width, height / 2, width, height};	
	
	for (UINT i = 0; i < options_text.size(); i++) {
		CustomComboBoxOption option(options_text[i]);
		option.region = {0, (i + 1) * height, width, (i + 2) * height};
		options.push_back(option);
	}

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (add_to_list)
		custom_controls.addControl(this);
}

void CustomComboBox::setFocus(bool focus) {
	if (focus) {
		SetFocus(handle);
		SetCapture(handle);
	}
	else {
		ReleaseCapture();
	}

	this->focus = focus;
	is_expanded = focus;
}

void CustomComboBox::display_options(bool show) {
	is_expanded = show;

	MoveWindow(handle, xpos, ypos, width, show ? height * (options.size() + 1) : height, !show);

	setFocus(show);
}

void CustomComboBox::change_option_hover(int pos_change, bool wrap) {
	int newpos = hovered_option + pos_change;
	hovered_option = pos_change > 0 ? newpos > (int)options.size() - 1 ? wrap ? 0 : options.size() - 1 : newpos : pos_change < 0 ? newpos < 0 ? wrap ? options.size() - 1 : 0 : newpos : selected_option;
	for (UINT i = 0; i < options.size(); i++)
		options[i].hover = false;
	options[hovered_option].hover = true;
	selected_option = hovered_option;
}

LRESULT CALLBACK customcomboboxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//display up
	//scroll bar
	//draw outside client

	CustomComboBox* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomComboBox*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);		
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomComboBox*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//paint box region
			SIZE size;
			a->window_text = a->options.at(a->selected_option).text;
			string text = (a->window_text);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->box_hover || a->is_expanded ? cs.background_hover_on : cs.background_idle_on : a->box_hover || a->is_expanded ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->box_hover || a->is_expanded ? cs.text_hover_on : cs.text_idle_on : a->box_hover || a->is_expanded ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((a->box_region.right - a->box_region.left) - size.cx) / 2, ((a->box_region.bottom - a->box_region.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &a->box_region, str_to_wstr(text).c_str(), text.length(), NULL);

			//paint arrow region
			if (a->style == ccb_style1 || a->style == ccb_style2) {
				HBRUSH tempbrush = CreateSolidBrush(a->toggle_state ? a->arrow_hover || a->is_expanded ? cs.background_hover_on : cs.background_idle_on : a->arrow_hover || a->is_expanded ? cs.background_hover_off : cs.background_idle_off);
				FillRect(hDC, &a->arrow_region, tempbrush);
				DeleteObject(tempbrush);
				SetDCPenColor(hDC, a->toggle_state ? a->arrow_hover || a->is_expanded ? cs.text_hover_on : cs.text_idle_on : a->arrow_hover || a->is_expanded ? cs.text_hover_off : cs.text_idle_off);

				RECT temprect = a->arrow_region;
				temprect.left = a->box_region.right;
				int left = ((temprect.right - temprect.left) - 5) / 2 + a->box_region.right;
				int top = ((temprect.bottom - temprect.top) - 3) / 2;

				POINT points[6];
				points[0] = {left + 0, top + 0};
				points[1] = {left + 2, top + 2};
				points[2] = {left + 4, top + 0};
				points[3] = {left + 3, top + 0};
				points[4] = {left + 2, top + 1};
				points[5] = {left + 1, top + 0};
				Polygon(hDC, points, 6);			
			}
			if (a->style == ccb_style3) {
				HBRUSH arrowup = CreateSolidBrush(a->toggle_state ? a->arrow_hover_up ? cs.background_hover_on : cs.background_idle_on : a->arrow_hover_up ? cs.background_hover_off : cs.background_idle_off);
				HBRUSH arrowdo = CreateSolidBrush(a->toggle_state ? a->arrow_hover_down ? cs.background_hover_on : cs.background_idle_on : a->arrow_hover_down ? cs.background_hover_off : cs.background_idle_off);
				FillRect(hDC, &a->arrow_region_up, arrowup);
				FillRect(hDC, &a->arrow_region_down, arrowdo);
				DeleteObject(arrowup);
				DeleteObject(arrowdo);
				
				SetDCPenColor(hDC, a->toggle_state ? a->arrow_hover_up ? cs.text_hover_on : cs.text_idle_on : a->arrow_hover_up ? cs.text_hover_off : cs.text_idle_off);
				RECT temprect = a->arrow_region_up;
				temprect.left = a->box_region.right;
				int left = ((temprect.right - temprect.left) - 5) / 2 + a->box_region.right;
				int top = ((temprect.bottom - temprect.top) - 3) / 2 + a->arrow_region_up.top;
				POINT points[6];
				points[0] = {left + 0, top + 2};
				points[1] = {left + 2, top + 0};
				points[2] = {left + 4, top + 2};
				points[3] = {left + 3, top + 2};
				points[4] = {left + 2, top + 1};
				points[5] = {left + 1, top + 2};
				Polygon(hDC, points, 6);

				SetDCPenColor(hDC, a->toggle_state ? a->arrow_hover_down ? cs.text_hover_on : cs.text_idle_on : a->arrow_hover_down ? cs.text_hover_off : cs.text_idle_off);
				temprect = a->arrow_region_down;
				temprect.left = a->box_region.right;
				left = ((temprect.right - temprect.left) - 5) / 2 + a->box_region.right;
				top = ((temprect.bottom - temprect.top) - 3) / 2 + a->arrow_region_down.top;
				points[0] = {left + 0, top + 0};
				points[1] = {left + 2, top + 2};
				points[2] = {left + 4, top + 0};
				points[3] = {left + 3, top + 0};
				points[4] = {left + 2, top + 1};
				points[5] = {left + 1, top + 0};
				Polygon(hDC, points, 6);
			}

			//paint options
			if (a->is_expanded) {
				for (UINT i = 0; i < a->options.size(); i++) {
					CustomComboBoxOption* b = &a->options[i];
					string text = (b->text);
					GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
					SetBkColor(hDC, b->toggle_state ? b->hover ? cs.background_hover_on : cs.background_idle_on : b->hover ? cs.background_hover_off : cs.background_idle_off);
					SetTextColor(hDC, b->toggle_state ? b->hover ? cs.text_hover_on : cs.text_idle_on : b->hover ? cs.text_hover_off : cs.text_idle_off);					
					ExtTextOut(hDC, ((b->region.right - b->region.left) - size.cx) / 2, (((b->region.bottom - b->region.top) - size.cy) / 2) + (a->height * (i + 1)) , ETO_OPAQUE | ETO_CLIPPED, &b->region, str_to_wstr(text).c_str(), text.length(), NULL);
				}
			}

			//paint borders
			COLORREF framecol = (a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			HBRUSH framebrush = CreateSolidBrush(RGB(GetRValue(framecol), GetGValue(framecol), GetBValue(framecol)));
			SetDCPenColor(hDC, framecol);
			FrameRect(hDC, &rcclient, framebrush);
			if (a->is_expanded) {//outline box when expanded
				MoveToEx(hDC, 0, a->height - 1, NULL);
				LineTo(hDC, a->width, a->height - 1);
			}
			if (a->style == ccb_style2) {//draw divider line for box and arrow region
				MoveToEx(hDC, a->box_region.right, 3, NULL);
				LineTo(hDC, a->box_region.right, a->height - 3);
			}
			if (a->style == ccb_style3) {//draw divider lines for box and up+down arrows
				MoveToEx(hDC, a->box_region.right, 1, NULL);
				LineTo(hDC, a->box_region.right, a->height - 1);

				MoveToEx(hDC, a->arrow_region.left, a->arrow_region.bottom / 2, NULL);
				LineTo(hDC, a->arrow_region.right, a->arrow_region.bottom / 2);
			}
			DeleteObject(framebrush);

			//bit-block transfer
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			SetFocus(WindowFromPoint(getcursorpos()));
			if (is_cursor_in_region(a->box_region, getclientcursorpos(a->handle))) {
				if (a->style == ccb_style1 || a->style == ccb_style3) {
					a->change_option_hover(0);
					a->display_options(!a->is_expanded);
				}
				if (a->style == ccb_style0 || a->style == ccb_style2) {
					a->change_option_hover(1, true);
				}
			}
			else if (is_cursor_in_region(a->arrow_region, getclientcursorpos(a->handle))) {
				if (a->style == ccb_style1 || a->style == ccb_style2) {
					a->change_option_hover(0);
					a->display_options(!a->is_expanded);
				}
				else if (a->style == ccb_style3) {
					if (is_cursor_in_region(a->arrow_region_up, getclientcursorpos(a->handle))) {
						a->hovered_option = a->selected_option;
						a->change_option_hover(-1);
					}
					else if (is_cursor_in_region(a->arrow_region_down, getclientcursorpos(a->handle))) {
						a->hovered_option = a->selected_option;
						a->change_option_hover(1);
					}
				}
			}
			else if (a->is_expanded) {
				for (UINT i = 0; i < a->options.size(); i++) {
					if (is_cursor_in_region(a->options[i].region, getclientcursorpos(a->handle))) {
						a->selected_option = i;
						a->change_option_hover(0);
						a->display_options(false);
					}
				}				
			}
			if (!is_cursor_in_region(getclientrect(a->handle), getclientcursorpos(a->handle))) {
				a->display_options(false);
			}

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			if (is_cursor_in_region(a->box_region, getclientcursorpos(a->handle))) {
				bool orig_state = a->box_hover;
				a->box_hover = true;
				a->arrow_hover = (a->style == ccb_style1) ? true : false;
				a->arrow_hover_up = false;
				a->arrow_hover_down = false;
				if (!orig_state)
					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else if (is_cursor_in_region(a->arrow_region, getclientcursorpos(a->handle))) {
				bool orig_state = a->arrow_hover;
				a->arrow_hover = true;
				a->box_hover = (a->style == ccb_style1) ? true : false;
				if (a->style == ccb_style3) {
					a->arrow_hover = false;
					if (is_cursor_in_region(a->arrow_region_up, getclientcursorpos(a->handle))) {
						a->arrow_hover_up = true;
						a->arrow_hover_down = false;
					}
					else if (is_cursor_in_region(a->arrow_region_down, getclientcursorpos(a->handle))) {
						a->arrow_hover_down = true;
						a->arrow_hover_up = false;
					}
				}
				if (!orig_state)
					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else if (a->is_expanded && is_cursor_in_region(getclientrect(a->handle), getclientcursorpos(a->handle))) {
				if (a->arrow_hover_up || a->arrow_hover_down) {
					a->arrow_hover_up = false;
					a->arrow_hover_down = false;
					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
				}
				for (UINT i = 0; i < a->options.size(); i++) {
					bool orig_state = a->options[i].hover;
					a->options[i].hover = false;
					if (is_cursor_in_region(a->options[i].region, getclientcursorpos(a->handle)) || getclientcursorpos(a->handle).y == a->options[i].region.bottom) {
						a->options[i].hover = true;
						a->hovered_option = i;
					}
					if (orig_state != a->options[i].hover)
						RedrawWindow(a->handle, &a->options[i].region, NULL, RDW_INVALIDATE);
				}
			}

			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_RETURN:{
					a->selected_option = a->hovered_option;
					//fall through
				}
				case VK_ESCAPE:{
					if (a->is_expanded)
						a->display_options(false);

					RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
					break;
				}
				case VK_RIGHT:
				case VK_DOWN:{
					a->change_option_hover(1);
					break;
				}					
				case VK_LEFT:
				case VK_UP:{
					a->change_option_hover(-1);
					break;
				}
				case VK_HOME:{
					a->selected_option = 0;
					a->change_option_hover(0);
					break;
				}
				case VK_END:{
					a->selected_option = a->options.size() - 1;
					a->change_option_hover(0);
					break;
				}
				case VK_PRIOR:{
					a->change_option_hover(-1);
					break;
				}
				case VK_NEXT:{
					a->change_option_hover(1);
					break;
				}
				default:
					wrongkey_skip = true;
			}
			if (!wrongkey_skip) 
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOVE:
		case WM_SIZE:{
			RECT mappedclient = getmappedclientrect(a->handle, GetParent(a->handle));
			RECT client = getclientrect(a->handle);

			a->xpos = mappedclient.left;
			a->ypos = mappedclient.top;
			a->width = client.right;
			a->height = a->is_expanded ? client.bottom / (a->options.size() + 1) : client.bottom;
			
			a->box_region = {0, 0, a->width - a->arrow_region_width, a->height};
			a->arrow_region = {a->width - a->arrow_region_width, 0, a->width, a->height};
			a->arrow_region_up = {a->width - a->arrow_region_width, 0, a->width, a->height / 2};
			a->arrow_region_down = {a->width - a->arrow_region_width, a->height / 2, a->width, a->height};

			for (UINT i = 0; i < a->options.size(); i++) 
				a->options[i].region = {0, (i + 1) * a->height, a->width, (i + 2) * a->height};

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			a->arrow_hover = false;
			a->box_hover = false;
			a->arrow_hover_up = false;
			a->arrow_hover_down = false;

			for (auto b : a->options) 
				b.hover = false;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			a->display_options(false);

			break;
		}
		case WM_DESTROY:{
			a->display_options(false);

			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_customcombobox() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = customcomboboxclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customcomboboxProc;
	return RegisterClassEx(&wnd);
}
ATOM ccb = init_register_customcombobox();

#pragma endregion

#pragma region CustomScrollbar class

CustomScrollbar::CustomScrollbar(string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
	int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
	ScrollbarColorScheme scolor_scheme, HCURSOR client_cursor, HCURSOR thumb_cursor, UINT group_name/*= 0*/, bool add_to_list) {

	className = wstr_to_str(customtrackbarclassName); handle = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;

	this->window_name = window_name; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id;
	this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
	this->small_step = small_step; this->large_step = large_step; this->thumb_size = thumb_size; this->channel_size = channel_size; this->flags = flags;
	this->scolor_scheme = scolor_scheme; this->client_cursor = client_cursor; this->thumb_cursor = thumb_cursor;
	this->group_name = group_name;

	if (this->client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
	if (this->thumb_cursor == NULL)
		this->thumb_cursor = LoadCursor(NULL, IDC_HAND);

	if (add_to_list)
		custom_controls.addControl(this);
}

int CustomScrollbar::setPos(int pos) {
	int x = !(flags & CSB_HORZ) ? height : width;
	return this->pos = pos <= 0 ? 0 : (int)(x - thumb_size) <= pos ? (x - thumb_size) : pos;
}

int CustomScrollbar::setVal(int pos) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = !(flags & CSB_HORZ) ? height : width;
	return current_val = ((pos * (max_val - min_val)) / (x - thumb_size == 0 ? 1 : (x - thumb_size))) + min_val;
}

int CustomScrollbar::setPosWithVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = !(flags & CSB_HORZ) ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	return pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val);
}

int CustomScrollbar::moveThumb(int pos) {
	return setVal(setPos(pos));
}

void CustomScrollbar::moveThumbReal(int pos) {
	setPosWithVal(moveThumb(pos));
}

void CustomScrollbar::thumbHitText() {
	if (is_cursor_in_region(thumb_region, getclientcursorpos(handle))) {
		if (!thumb_hover) {
			thumb_hover = true;
			InvalidateRect(handle, &thumb_region, true);
		}
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTTHUMB, NULL));
	}
	else if (thumb_hover) {
		thumb_hover = false;
		InvalidateRect(handle, &thumb_region, true);
		SendMessage(handle, WM_SETCURSOR, (WPARAM)handle, (LPARAM)MAKELONG(HTCLIENT, NULL));
	}
}

void CustomScrollbar::setFocus(bool focus) {
	this->focus = focus;
	if (focus) {
		SetFocus(handle);
		if (dragging)
			SetCapture(handle);
	}
	else {
		ReleaseCapture();
		//setfocus to new window here		
	}

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool CustomScrollbar::setRange(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	setVal(pos);

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
	return true;
}

void CustomScrollbar::redrawNewVal(int val) {
	RECT tr = getclientrect(handle);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = !(flags & CSB_HORZ) ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val == 0 ? 1 : (max_val - min_val));

	/*if (scrollbar_owner_handle)
		PostMessage(scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);*/
	PostMessage(GetParent(handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)this);
	RedrawWindow(handle, &tr, NULL, RDW_INVALIDATE);
}

LRESULT CALLBACK CustomScrollbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CustomScrollbar* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomScrollbar*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomScrollbar*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			ScrollbarColorScheme cs = a->scolor_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//colorbackground
			HBRUSH tempbrush0 = CreateSolidBrush(cs.background);
			FillRect(hDC, &rcclient, tempbrush0);

			//calculate thumb position
			if (a->start) {
				a->setPosWithVal(a->start_val);
				a->start = false;
			}
			a->thumb_region.left = (!(a->flags & CSB_HORZ) ? rcclient.left + 1 : rcclient.left + a->pos);
			a->thumb_region.top = (!(a->flags & CSB_HORZ) ? rcclient.top + a->pos : rcclient.top + 1);
			a->thumb_region.right = (!(a->flags & CSB_HORZ) ? rcclient.right - 1 : rcclient.left + a->thumb_size + a->pos);
			a->thumb_region.bottom = (!(a->flags & CSB_HORZ) ? rcclient.left + a->thumb_size + a->pos : rcclient.bottom - 1);

			//draw channel
			RECT rc_channel_left;
			RECT rc_channel_right;

			//left/top
			rc_channel_left.left = (!(a->flags & CSB_HORZ) ? rcclient.left + a->channel_size : rcclient.left);
			rc_channel_left.right = (!(a->flags & CSB_HORZ) ? rcclient.right - a->channel_size : rcclient.left + a->pos);
			rc_channel_left.top = (!(a->flags & CSB_HORZ) ? rcclient.top : rcclient.top + a->channel_size);
			rc_channel_left.bottom = (!(a->flags & CSB_HORZ) ? rcclient.top + a->pos : rcclient.bottom - a->channel_size);
			//right/bottom
			rc_channel_right.left = (!(a->flags & CSB_HORZ) ? rcclient.left + a->channel_size : rcclient.left + a->thumb_size + a->pos);
			rc_channel_right.right = (!(a->flags & CSB_HORZ) ? rcclient.right - a->channel_size : rcclient.right);
			rc_channel_right.top = (!(a->flags & CSB_HORZ) ? rcclient.top + a->pos : rcclient.top + a->channel_size);
			rc_channel_right.bottom = (!(a->flags & CSB_HORZ) ? rcclient.bottom : rcclient.bottom - a->channel_size);

			HBRUSH tempbrush1 = CreateSolidBrush(a->mouse_in_client ? cs.left_channel_highlight : cs.left_channel_idle);
			HBRUSH tempbrush2 = CreateSolidBrush(a->mouse_in_client ? cs.right_channel_highlight : cs.right_channel_idle);
			FillRect(hDC, &rc_channel_left, tempbrush1);
			FillRect(hDC, &rc_channel_right, tempbrush2);

			//draw focus rect
			if ((a->focus || a->flags & CSB_BORDER) && !(a->flags & CSB_NOBORDER)) {
				SetDCPenColor(hDC, cs.border);
				Rectangle(hDC, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);
			}

			//draw thumb
			a->thumbHitText();
			SIZE size;
			GetTextExtentPoint32(hDC, str_to_wstr(a->window_text).c_str(), (int)a->window_text.length(), &size);
			SetTextColor(hDC, a->thumb_selected ? cs.thumb_text_selected : a->thumb_hover ? cs.thumb_text_hover : cs.thumb_text_idle);
			SetBkColor(hDC, a->thumb_selected ? cs.thumb_background_selected : a->thumb_hover ? cs.thumb_background_hover : cs.thumb_background_idle);
			ExtTextOut(hDC,
				((a->thumb_region.right - a->thumb_region.left) - size.cx) / 2 + (!(a->flags & CSB_HORZ) ? 0 : a->pos),
				((a->thumb_region.bottom - a->thumb_region.top) - size.cy) / 2 + (!(a->flags & CSB_HORZ) ? a->pos : 1),
				ETO_OPAQUE | ETO_CLIPPED, &a->thumb_region, str_to_wstr(a->window_text).c_str(), a->window_text.length(), NULL);
			SetDCPenColor(hDC, a->thumb_selected ? cs.thumb_border_selected : a->thumb_hover || a->dragging ? cs.thumb_border_hover : cs.thumb_border_idle);
			Rectangle(hDC, a->thumb_region.left, a->thumb_region.top, a->thumb_region.right, a->thumb_region.bottom);

			//draw window name
			if (a->window_name.size() > 0) {
				SetBkMode(hDC, TRANSPARENT);
				SetTextColor(hDC, a->mouse_in_client ? cs.window_name_highlight : cs.window_name_idle);
				RECT rc = getclientrect(a->handle);
				DrawText(hDC, str_to_wstr(a->window_name).c_str(), a->window_name.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			//apply paint
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(tempbrush0);
			DeleteObject(tempbrush1);
			DeleteObject(tempbrush2);
			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			a->focus = true;
			a->mouse_in_client = true;
			a->dragging = true;
			a->thumb_hover = true;
			a->thumb_selected = true;

			a->setFocus(true);

			POINT pt = getclientcursorpos(a->handle);

			//smooth dragging
			a->moveThumb((!(a->flags & CSB_HORZ) ? pt.y : pt.x) - (a->thumb_size / 2));

			customtrackbarProc(a->handle, WM_SETCURSOR, (WPARAM)a->handle, (LPARAM)MAKELONG(HTTHUMB, NULL));

			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP: {
			a->dragging = false;
			a->thumb_selected = false;
			ReleaseCapture();

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			a->CustomScrollbar::thumbHitText();

			if (a->dragging) {

				POINT pt = getclientcursorpos(a->handle);

				//smooth dragging
				a->moveThumb((!(a->flags & CSB_HORZ) ? pt.y : pt.x) - (a->thumb_size / 2));

				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_LEFT:{
					a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_DOWN:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_RIGHT:{
					a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_UP:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_HOME:{
					a->setPosWithVal(a->min_val);
					break;
				}
				case VK_END:{
					a->setPosWithVal(a->max_val);
					break;
				}
				case VK_PRIOR:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
					break;
				}
				case VK_NEXT:{
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
			if (!wrongkey_skip) {
				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
				}
				else {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON) {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->large_step);
					else
						a->setPosWithVal(a->current_val - a->large_step);
				}
				else {
					if (!(a->flags & CSB_HORZ))
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
				}
			}
			else
				break;
			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			PostMessage(GetParent(a->handle), WM_customtrackbar_VALUE_CHANGED, NULL, (LPARAM)a);
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_SIZE:{
			a->width = LOWORD(lParam);
			a->height = HIWORD(lParam);

			break;
		}
		case WM_MOVE:{
			a->xpos = LOWORD(lParam);
			a->ypos = HIWORD(lParam);

			break;
		}
		case WM_SETCURSOR:{
			if (a->dragging)
				lParam = MAKELPARAM(HTTHUMB, HIWORD(lParam));
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
				case HTTHUMB:
					SetCursor(a->thumb_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_KILLFOCUS:{
			a->setFocus(false);

			a->mouse_in_client = false;
			a->dragging = false;
			a->thumb_hover = false;
			a->thumb_selected = false;

			break;
		}
		case WM_MOUSELEAVE:{
			if (!a->dragging) {
				a->mouse_in_client = false;
				a->thumb_selected = false;
				a->thumb_hover = false;

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_CustomScrollbar() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = CustomScrollbarclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = CustomScrollbarProc;
	return RegisterClassEx(&wnd);
}
ATOM csb = init_register_custom_CustomScrollbar();

#pragma endregion

#pragma region EditControl class

LRESULT CALLBACK EditControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT id, DWORD_PTR ptr);

EditControl::EditControl() {}
EditControl::EditControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, HWND parent, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	UINT flags, BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT group_name,
	bool number_only/* = false*/, int min_number/* = INT_MIN*/, int max_number/* = INT_MAX*/, bool add_to_list) {
	className = wstr_to_str(EditControlclassName); handle = NULL; caret_pos = 0; start = 0; end = 0;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->parent = parent; this->id = id; this->Proc = Proc;
	this->flags = flags; this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->hover_info = hover_info; this->group_name = group_name;
	this->number_only = number_only; this->min_number = min_number; this->max_number = max_number;

	if (flags & EDC_VSCROLL) {
		this->width -= 17;
		BasicControl_colorscheme cs =  color_scheme;
		vscrollbar = new CustomScrollbar("vert", WS_CHILD, 0, 0, 0, 0, get_unique_id(),
			0, 1, 0, 1, 1,
			100, 0, CSB_NOBORDER, {
			cs.background_idle_on, cs.border_idle_on, RGB(0, 0, 0), RGB(0, 0, 0),							// background/border/window name idle/highlight
			cs.background_idle_on, cs.background_idle_on, cs.background_idle_on, cs.background_idle_on,		// left/right channel left/right highlight
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,										// thumb background: idle/hover/selected
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,	 									// thumb border: idle/hover/selected
			RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0)														// thumb text: idle/hover/selected
			}, NULL, NULL, NULL, false);
	}
	if (flags & EDC_HSCROLL) {
		this->height -= 17;
		BasicControl_colorscheme cs =  color_scheme;
		hscrollbar = new CustomScrollbar("horz", WS_CHILD, 0, 0, 0, 0, get_unique_id(),
			0, 1, 0, 1, 1,
			100, 0, CSB_NOBORDER | CSB_HORZ, {
			cs.background_idle_on, cs.border_idle_on, RGB(0, 0, 0), RGB(0, 0, 0),							// background/border/window name idle/highlight
			cs.background_idle_on, cs.background_idle_on, cs.background_idle_on, cs.background_idle_on,		// left/right channel left/right highlight
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,										// thumb background: idle/hover/selected
			cs.border_idle_on, cs.border_idle_on, cs.border_idle_on,	 									// thumb border: idle/hover/selected
			RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0)														// thumb text: idle/hover/selected
			}, NULL, NULL, NULL, false);
	}

	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_IBEAM);
	if (add_to_list)
		custom_controls.addControl(this);
}

vector<string> EditControl::GetLines() {
	vector<string> ret;
	string line = "";
	for (UINT i = 0; i < window_text.size(); i++){
		if (window_text[i] == '\r') {
			ret.push_back(line);
			line = "";
			i++;
			continue;
		}
		if (window_text[i] != '\r' &&window_text[i] != '\n')
			line += window_text[i];
	}
	ret.push_back(line);
	return ret;
}

UINT EditControl::VecPosToStrPos(POINT pt) {
	cout << pt.x << " " << pt.y << '\n';

	int x = 0, y = 0;
	int offset = 0;
	for (UINT i = 0; i < window_text.size(); i++) {
		x++;
		if (window_text[i] == '\n') {
			if (window_text.at(i - 1) == '\n')
				offset++;
			offset++;
			y++;
			x = 0;
		}
		if (pt.x == x && pt.y == y) {
			cout << "i: " << i << '\n';
			return i;
		}
	}

	return 0;
}

POINT EditControl::PosToClient(UINT caret) {//uses text pos to return client pos
	SIZE size;

	vector<string> lines = GetLines();

	UINT total = 0;
	for (UINT i = 0; i < lines.size(); i++) {
		if (total + lines[i].size() >= caret) {
			string letters = window_text.substr(total, caret - total);
			HDC hDC = GetDC(handle);
			GetTextExtentPoint32(hDC, str_to_wstr(letters).c_str(), letters.length(), &size);
			ReleaseDC(handle, hDC);
			return {size.cx + 2, line_height * i};
		}
		total += lines[i].size() + 2;
	}
	return {0, 0};
}

UINT EditControl::ClientToPos(POINT pos) {//uses client pos to return text pos
	if (line_height < 1)
		return 0;
	int line = (pos.y - 2) / line_height;
	if (line < 0)
		line = 0;
	vector<string> text = GetLines();
	if (line > (int)text.size() - 1)
		line = text.size() - 1;
	HDC hDC = GetDC(handle);
	int total = 0;
	POINT pt = {0, line};
	for (UINT i = 0; i < text[line].size(); i++) {
		SIZE size;
		string letter = "";
		letter += text[line][i];
		GetTextExtentPoint32(hDC, str_to_wstr(letter).c_str(), letter.length(), &size);
		total += size.cx;
		pt.x = i;
		if (total - 3 > pos.x)
			break;
	}
	if (total - 3 < pos.x)
		pt.x = text[line].size();
	ReleaseDC(handle, hDC);

	return VecPosToStrPos(pt);
}

bool EditControl::GetHighlightText(EditTextHighlight* eth, int line /*= -1*/) {
	eth->startpos = start > end ? end : start;
	eth->endpos = start > end ? start : end;
	
	bool l = true, m = false, r = false;
	int y = 0;
	for (UINT i = 0; i < window_text.size(); i++) {
		if ((int)i == eth->startpos) {
			l = false;
			m = true;
		}
		if ((int)i == eth->endpos) {
			m = false;
			r = true;
		}
		if (l && (y == line || line < 0)) 
			eth->left += window_text[i];
		if (m && (y == line || line < 0))
			eth->highlight += window_text[i];
		if (r && (y == line || line < 0))
			eth->right += window_text[i];
		if (window_text[i] == '\n') 
			y++;
	}

	return true;
}

void EditControl::EditText(string text, int pos/* = -1*/) {
	if (pos < 0)
		pos = this->caret_pos;

	if (!(flags & EDC_READONLY)) {
		EditTextHighlight eth;
		GetHighlightText(&eth);

		if (eth.startpos != eth.endpos) 
			window_text.erase(window_text.begin() + eth.startpos, window_text.begin() + eth.endpos);
	}

	if (caret_pos > window_text.size() + 1)
		caret_pos = window_text.size();
	if (text.size() > 0)
		window_text.insert(this->caret_pos, text);

	start = end = caret_pos += text.size();

	PostMessage(handle, WM_customedit_TEXT_CHANGED, (WPARAM)id, (LPARAM)handle);
	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool EditControl::IsTextClipped(RECT* r) {
	RECT rc = getclientrect(handle);
	rc.left += this->scroll_xpos; rc.right += this->scroll_xpos;
	rc.top += this->scroll_ypos; rc.bottom += this->scroll_ypos;

	bool lr = true;
	bool tb = true;

	displayrect(rc);

	displayrect(*r);

	(rc.left < r->left && r->left < rc.right) ? r->left = rc.left, lr = false : r->left = r->left;
	(rc.top < r->top && r->top < rc.bottom) ? r->top = rc.top, tb = false : r->top = r->top;
	(rc.left < r->right && r->right < rc.right) ? r->right = rc.right, lr = false : r->right = r->right, lr = true;
	(rc.top < r->bottom && r->bottom < rc.bottom) ? r->bottom = rc.bottom, tb = false : r->bottom = r->bottom, tb = true;

	//cout << tb << " " << lr << " " << (tb && lr) << endl;
	displayrect(*r);
	cout << "------------\n";


	return tb && lr;
}

EditControl& operator<<(EditControl& obj, int a) {
	obj.EditText(int_to_str(a), obj.window_text.size());
	return obj;
}
EditControl& operator<<(EditControl& obj, char a) {
	obj.EditText(char_to_str(a), obj.window_text.size());
	return obj;
}
EditControl& operator<<(EditControl& obj, string a) {
	obj.EditText(a, obj.window_text.size());
	return obj;
}

LRESULT CALLBACK EditControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	EditControl* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<EditControl*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);

		if (a->flags & EDC_VSCROLL && !a->vscrollbar->handle) {
			a->vscrollbar->Create(GetParent(a->handle));
			a->vscrollbar->scrollbar_owner_handle = a->handle;
		}
		if (a->flags & EDC_HSCROLL && !a->hscrollbar->handle) {
			a->hscrollbar->Create(GetParent(a->handle));
			a->hscrollbar->scrollbar_owner_handle = a->handle;
		}


		for (int i = 1; i < 2; i++)
			a->window_text += int_to_str(i) + "the cake is really good You KNow what i § hahahaha there is no soup\r\n";

		PostMessage(a->handle, WM_customedit_TEXT_CHANGED, NULL, NULL);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<EditControl*>(ptr);
	}

	static BYTE click_count = 0;

	if (message == WM_customtrackbar_VALUE_CHANGED) {
		CustomScrollbar csb = *((CustomScrollbar*)lParam);

		if (csb.scrollbar_owner_handle == a->handle) {
			if (csb.handle == a->vscrollbar->handle && csb.thumb_size > 0) {
				a->scroll_ypos = csb.current_val * a->line_height;
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else if (csb.handle == a->hscrollbar->handle && csb.thumb_size > 0) {
				
			}
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	else if (message == WM_customedit_TEXT_CHANGED) {		
		TEXTMETRIC tm;
		HDC hDC = GetDC(a->handle);
		GetTextMetrics(hDC, &tm);
		ReleaseDC(a->handle, hDC);
		a->line_height = (BYTE)tm.tmHeight;
		vector<string> lines = a->GetLines();

		UINT max_lines = a->height / a->line_height;

		if (a->flags & EDC_VSCROLL) {//calculate trackbar thumb size
			a->vscrollbar->max_val = lines.size() - (a->height / a->line_height);
			if (lines.size() > max_lines) {
				a->vscrollbar->thumb_size = (UINT)((double)a->height * ((double)max_lines / (double)(lines.size() == 0 ? 1 : lines.size())));
				RedrawWindow(a->vscrollbar->handle, NULL, NULL, RDW_INVALIDATE);
			}
			else {
				a->vscrollbar->thumb_size = 0;
				RedrawWindow(a->vscrollbar->handle, NULL, NULL, RDW_INVALIDATE);
			}
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//paint background
			HBRUSH hb_background = CreateSolidBrush(RGB(255,0,0)/*a->toggle_state ? cs.background_idle_on : cs.background_idle_off*/);
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			//draw each line of text
			if (a->font)
				SelectObject(hDC, a->font);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			a->line_height = (BYTE)tm.tmHeight;
			vector<string> text = a->GetLines();
			for (UINT i = 0; i < text.size(); i++) {
				EditTextHighlight eth;
				a->GetHighlightText(&eth, i);
				SIZE lsize, msize, rsize;
				lsize = msize = rsize = {0, 0};
				//left
				if (eth.left.size() > 0) {
					GetTextExtentPoint32(hDC, str_to_wstr(eth.left).c_str(), eth.left.length(), &lsize);
					SetBkColor(hDC, RGB(0,255,0)/*a->toggle_state ? cs.background_idle_on : cs.background_idle_off*/);
					SetTextColor(hDC, a->toggle_state ? cs.text_idle_on : cs.text_idle_off);
					RECT tr = {
						2 /*- a->scroll_xpos*/,
						i * a->line_height + 2 /*- a->scroll_ypos*/,
						lsize.cx + 2 /*- a->scroll_xpos*/,
						(i + 1) * a->line_height + 2/* - a->scroll_ypos*/
					};
					cout << "LEFT:\n";
					//if (a->IsTextClipped(&tr))
						ExtTextOut(hDC, 2 - a->scroll_xpos, i * a->line_height + 2 - a->scroll_ypos, ETO_OPAQUE | ETO_CLIPPED, &tr, str_to_wstr(eth.left).c_str(), eth.left.length(), NULL);
				}
				//middle (highlight)
				if (eth.highlight.size() > 0) {
					GetTextExtentPoint32(hDC, str_to_wstr(eth.highlight).c_str(), eth.highlight.length(), &msize);
					SetBkColor(hDC, a->toggle_state ? cs.background_hover_on : cs.background_hover_off);
					SetTextColor(hDC, a->toggle_state ? cs.text_hover_on : cs.text_hover_off);
					RECT tr = {
						lsize.cx + 2 /*- a->scroll_xpos*/,
						i * a->line_height + 2 /*- a->scroll_ypos*/,
						lsize.cx + msize.cx + 2 /*- a->scroll_xpos*/,
						(i + 1) * a->line_height + 2 /*- a->scroll_ypos*/
					};
					if (a->IsTextClipped(&tr))
						ExtTextOut(hDC, lsize.cx + 2 - a->scroll_xpos, i * a->line_height + 2 - a->scroll_ypos, ETO_OPAQUE | ETO_CLIPPED, &tr, str_to_wstr(eth.highlight).c_str(), eth.highlight.length(), NULL);
				}
				//right
				if (eth.right.size() > 0) {
					GetTextExtentPoint32(hDC, str_to_wstr(eth.right).c_str(), eth.right.length(), &rsize);
					SetBkColor(hDC, RGB(0, 0, 255)/*a->toggle_state ? cs.background_idle_on : cs.background_idle_off*/);
					SetTextColor(hDC, a->toggle_state ? cs.text_idle_on : cs.text_idle_off);
					RECT tr = {
						lsize.cx + msize.cx + 2/* - a->scroll_xpos*/,
						i * a->line_height + 2/* - a->scroll_ypos*/,
						lsize.cx + msize.cx + rsize.cx + 2 /*- a->scroll_xpos*/,
						(i + 1) * a->line_height + 2 /*- a->scroll_ypos*/
					};
					cout << "RIGHT:\n";
				//	if (a->IsTextClipped(&tr))
						ExtTextOut(hDC, lsize.cx + msize.cx + 2 - a->scroll_xpos, i * a->line_height + 2 - a->scroll_ypos, ETO_OPAQUE | ETO_CLIPPED, &tr, str_to_wstr(eth.right).c_str(), eth.right.length(), NULL);
				}
			}

			//draw caret			
			POINT pt = a->PosToClient(a->caret_pos);
			SetDCPenColor(hDC, RGB(255, 255, 255));
			MoveToEx(hDC, pt.x - a->scroll_xpos, pt.y + 2 - a->scroll_ypos, NULL);
			LineTo(hDC, pt.x - a->scroll_xpos, pt.y + a->line_height + 2 - a->scroll_ypos);

			//draw frame
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			//bit block transfer
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);
			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case MY_COMMAND:{
			switch (HIWORD(wParam)) {
				case EN_CHANGE:{
					if (a->number_only) {
						string EditText = getwindowtext((HWND)lParam);
						bool contains_char = false;
						for (UINT j = 0; j < EditText.size(); j++) {
							if (EditText[j] < 48 || EditText[j] > 57 && EditText[j] != 45) {
								EditText.erase(j, 1);
								contains_char = true;
							}
						}
						if (contains_char) {
							SetWindowText((HWND)lParam, str_to_wstr(EditText).c_str());
							SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)EditText.size(), (LPARAM)EditText.size());
						}
						int number = str_to_int(EditText);
						if (number < a->min_number) {
							SetWindowText((HWND)lParam, str_to_wstr(int_to_str(a->min_number)).c_str());
							SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
						}
						else if (number > a->max_number) {
							SetWindowText((HWND)lParam, str_to_wstr(int_to_str(a->max_number)).c_str());
							SendMessage((HWND)lParam, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
						}
						a->window_text = getwindowtext(hwnd);
					}
					if (a->Proc != nullptr)
						a->Proc((WPARAM)a->handle, 0);

					break;
				}
				/*case EN_SETFOCUS:{
				PostMessage(hwnd, EM_SETSEL, 0, -1);
				break;
				}*/
			}
			break;
		}
		case WM_LBUTTONDOWN:{/*

			a->vscrollbar->redrawNewVal(0);
			lastwindow = a->vscrollbar->handle;
*/
			SetFocus(a->handle);
			SetCapture(a->handle);

			SetTimer(a->handle, 0, 500, NULL);
			click_count++;

			if (click_count == 2) {

			}
			if (click_count == 3) {
				click_count = 0;

				cout << "trips\n";

				/*vector<string> text = split_str(a->window_text, "\n", 0);
				int i = a->ClientToPos(getclientcursorpos(a->handle));
				a->start = {0, i};
				a->end = {text[i].size(), i};*/

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
				break;
			}

			a->dragging = true;

			a->caret_pos = a->ClientToPos(getclientcursorpos(a->handle));
			if (!(GetKeyState(VK_SHIFT) & 0x8000))
				a->start = a->caret_pos;
			a->end = a->caret_pos;

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_LBUTTONUP:{
			ReleaseCapture();

			a->dragging = false;

			break;
		}
		case WM_CHAR:{
			if (!(a->flags & EDC_READONLY) &&
				!(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000) &&
				wParam != '\b' && wParam != '\n' && wParam != '\r' && wParam != '\t') {

				a->EditText(char_to_str((char)wParam), a->caret_pos);
			}
			else
				break;
			
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') {
				a->start = 0; 
				a->end = a->window_text.size();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'C') {
				EditTextHighlight eth;
				a->GetHighlightText(&eth);
				string line = a->window_text.substr(eth.startpos, eth.endpos) /*eth.highlight*/;
				OpenClipboard(NULL);
				EmptyClipboard();
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, line.size() + 1);
				strcpy_s((char*)hGlob, line.size() + 1, line.c_str());
				SetClipboardData(CF_TEXT, hGlob);
				CloseClipboard();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'V') {
				OpenClipboard(NULL);
				HANDLE hData = GetClipboardData(CF_TEXT);
				char * pszText = static_cast<char*>(GlobalLock(hData));	
				std::string text(pszText);
				a->EditText(text);
				GlobalUnlock(hData);
				CloseClipboard();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'X') {
				EditTextHighlight eth;
				a->GetHighlightText(&eth);
				string line = a->window_text.substr(eth.startpos, eth.endpos);
				a->window_text.erase(eth.startpos, eth.endpos);
				a->start = a->end = a->caret_pos = eth.startpos;
				OpenClipboard(NULL);
				EmptyClipboard();
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, line.size() + 1);
				strcpy_s((char*)hGlob, line.size() + 1, line.c_str());
				SetClipboardData(CF_TEXT, hGlob);
				CloseClipboard();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == VK_RIGHT) {
				UINT save_start = a->start;

				auto is_whitespace = [](char c) -> bool { return (c == ' ' || c == '\t' || c == '\r'); };
				
				for (UINT i = a->caret_pos; i < a->window_text.size() - 1; i++) {
					if (is_whitespace(a->window_text[i]) && !is_whitespace(a->window_text[i + 1])){
						a->start = a->end = a->caret_pos = i;
						break;
					}
				}

				if (GetKeyState(VK_SHIFT) & 0x8000) 
					a->start = save_start;
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == VK_LEFT) {
				UINT save_start = a->start;

				auto is_whitespace = [](char c) -> bool { return (c == ' ' || c == '\t' || c == '\r'); };

				for (UINT i = a->caret_pos; i > 1; i--) {
					if (is_whitespace(a->window_text[i]) && !is_whitespace(a->window_text[i - 1])) {
						a->start = a->end = a->caret_pos = i;
						break;
					}
				}

				if (GetKeyState(VK_SHIFT) & 0x8000)
					a->start = save_start;
				goto skip;
			}
			else if (GetKeyState(VK_SHIFT) & 0x8000 && wParam == VK_UP) {
				/*a->caret_pos.y--;
				if (a->caret_pos.y < 0)
					a->caret_pos.y = 0;
				if (a->caret_pos.x > (int)text[a->caret_pos.y].size())
					a->caret_pos.x = text[a->caret_pos.y].size();
				a->end = a->caret_pos;*/
				goto skip;
			}
			else if (GetKeyState(VK_SHIFT) & 0x8000 && wParam == VK_DOWN) {
				/*a->caret_pos.y++;
				if (a->caret_pos.y >(int)text.size() - 1)
					a->caret_pos.y = text.size() - 1;
				if (a->caret_pos.x > (int)text[a->caret_pos.y].size())
					a->caret_pos.x = text[a->caret_pos.y].size();
				a->end = a->caret_pos;*/
				goto skip;
			}
			
			switch (wParam) {
				case VK_LEFT:{
					a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
					if (a->window_text[a->caret_pos] == '\n')
						a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						a->start = a->caret_pos;
					a->end = a->caret_pos;
					break;
				}
				case VK_DOWN:{
					/*a->caret_pos.y++;
					if (a->caret_pos.y >(int)text.size() - 1)
						a->caret_pos.y = text.size() - 1;
					a->start = a->caret_pos;
					a->end = a->caret_pos;*/
					if (a->flags & EDC_HSCROLL) {
						int t = a->scroll_xpos;
						t -= a->line_height;
						a->scroll_xpos = t < 0 ? 0 : t;
					}
					break;
				}
				case VK_RIGHT:{
					a->caret_pos++;
					if (a->caret_pos < a->window_text.size() && a->window_text[a->caret_pos] == '\n') 
						a->caret_pos++;
					if (a->caret_pos > (int)a->window_text.size())
						a->caret_pos = a->window_text.size();
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						a->start = a->caret_pos;
					a->end = a->caret_pos;
					break;
				}
				case VK_UP:{
					/*a->caret_pos.y--;
					if (a->caret_pos.y < 0)
						a->caret_pos.y = 0;
					a->start = a->caret_pos;
					a->end = a->caret_pos;*/
					if (a->flags & EDC_HSCROLL) {
						a->scroll_xpos += a->line_height;
					}
					break;
				}
				case VK_HOME:{

					break;
				}
				case VK_END:{

					break;
				}
				case VK_PRIOR:{

					break;
				}
				case VK_NEXT:{

					break;
				}
				case VK_RETURN:{
					if (!(a->flags & EDC_READONLY)) 
						a->EditText("\r\n");
					break;
				}
				case VK_TAB:{
					if (!(a->flags & EDC_READONLY)) 
						a->EditText("\t", a->caret_pos);
					break;
				}
				case VK_DELETE:{
					if (!(a->flags & EDC_READONLY)) {
						if (a->start == a->end)
							a->start = a->start == a->window_text.size() ? a->window_text.size() : a->start + 1;
						else 
							a->caret_pos = a->start > a->end ? a->end : a->start;
						if (a->window_text[a->caret_pos] == '\r') {
							a->start = a->start == a->window_text.size() ? a->window_text.size() : a->start + 1;
							a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
						}

						a->EditText("");
					}
					break;
				}
				case VK_BACK:{
					if (!(a->flags & EDC_READONLY)) {
						if (a->start == a->end)//if not highlighted
							a->start = a->start == 0 ? 0 : a->start - 1;//move highlight left one
						else 
							a->caret_pos = a->start > a->end ? a->end + 1 : a->start + 1;//else (is highlighted) put cursor to beginning

						if (a->window_text[a->start] == '\n') {//if newline is found move highlight left again, ajdust caret as well
							a->start = a->start == 0 ? 0 : a->start - 1;
							a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
						}
						
						a->EditText("");//removes all highlighted text
						a->start = a->end = a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;//remove highlighting and move caret left
					}
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
			skip:
			/*for (UINT i = 0; i < a->window_text.size(); i++) {
				if (i == a->caret_pos)
					cout << "|";
				if (a->window_text[i] == '\r')
					cout << "\\r";
				else if (a->window_text[i] == '\n')
					cout << "\\n" << '\n';
				else if (a->window_text[i] == '\t')
					cout << "\\t";
				else
					cout << a->window_text[i];
			}
			cout << "\n/////////////////////////////////\n";*/

			if (!wrongkey_skip)
				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		/*case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (a->flags & EDC_VSCROLL) {
					int t = a->scroll_ypos;
					t -= a->line_height;
					a->scroll_ypos = t < 0 ? 0 : t;
					a->vscrollbar->redrawNewVal(a->scroll_ypos / a->line_height - 1);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (a->flags & EDC_VSCROLL) {
					a->scroll_ypos += a->line_height;
					vector<string> lines = a->GetLines();
					if (lines.size() < a->height / a->line_height)
						a->scroll_ypos = 0;
					else if (a->scroll_ypos > (lines.size() - (a->height / a->line_height)) * a->line_height)
						a->scroll_ypos = (lines.size() - (a->height / a->line_height)) * a->line_height;

					a->vscrollbar->redrawNewVal(a->scroll_ypos / a->line_height + 1);
				}
			}
			else
				break;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}*/
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = a->hover_info.Flags;
				me.hwndTrack = a->handle;
				me.dwHoverTime = a->hover_info.HoverTime;
				TrackMouseEvent(&me);
			}

			if (a->dragging) {
				a->caret_pos = a->ClientToPos(getclientcursorpos(a->handle));
				a->end = a->caret_pos;

				RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_CONTEXTMENU:{
			
			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			break;
		}
		case WM_SIZE:{
			a->width = LOWORD(lParam);
			a->height = HIWORD(lParam);

			if (a->flags & EDC_VSCROLL)
				MoveWindow(a->vscrollbar->handle, a->width + a->xpos, a->ypos, 17, a->height, true);
			if (a->flags & EDC_HSCROLL)
				MoveWindow(a->hscrollbar->handle, a->xpos, a->height + a->ypos, a->width, 17, true);
				
			break;
		}
		case WM_MOVE:{
			a->xpos = LOWORD(lParam);
			a->ypos = HIWORD(lParam);

			if (a->flags & EDC_VSCROLL)
				MoveWindow(a->vscrollbar->handle, a->width + a->xpos, a->ypos, 17, a->height, true);
			if (a->flags & EDC_HSCROLL)
				MoveWindow(a->hscrollbar->handle, a->xpos, a->height + a->ypos, a->width, 17, true);

			break;
		}
		case WM_TIMER:{
			if (wParam == 0) {
				click_count = 0;
				KillTimer(a->handle, 0);
			}
			break;
		}
		case WM_MOUSEHOVER:{
			SIZE size;
			HDC hDC = GetDC(hwnd);
			SelectObject(hDC, a->font);
			SetMapMode(hDC, MM_TEXT);
			GetTextExtentPoint32(hDC, str_to_wstr(a->hover_info.hover_text).c_str(), a->hover_info.hover_text.length(), &size);

			POINT pt = getclientcursorpos(GetParent(hwnd));

			StaticControl* temp = new StaticControl;
			temp->window_exstyles = WS_EX_TOPMOST;
			temp->className = wstr_to_str(staticcontrolclassName);
			temp->window_text = a->window_text;
			temp->window_styles = WS_CHILD | WS_VISIBLE | SS_CENTER;
			temp->xpos = pt.x + 20;
			temp->ypos = pt.y;
			temp->width = size.cx;
			temp->height = size.cy;
			temp->id = get_unique_id();
			temp->client_cursor = NULL;
			temp->font = Font_a;
			temp->Proc = nullptr;
			temp->color_scheme = a->color_scheme;

			a->hover_info.hover_text = a->window_text;

			a->hover_info.handle = temp->Create(GetParent(hwnd));

			ReleaseDC(hwnd, hDC);
			break;
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			if (a->hover_info.handle)
				DestroyWindow(a->hover_info.handle);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

ATOM init_register_custom_EditControl() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = EditControlclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = EditControlProc;
	return RegisterClassEx(&wnd);
}
ATOM edc = init_register_custom_EditControl();

#pragma endregion

#pragma region ContextMenu class

LRESULT CALLBACK CustomContextMenuProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CustomContextMenu* a = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		a = reinterpret_cast<CustomContextMenu*>(pCreate->lpCreateParams);
		a->handle = hwnd;
		SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)a);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWL_USERDATA);
		a = reinterpret_cast<CustomContextMenu*>(ptr);
	}

	switch (message) {
		case WM_PAINT:{
			BasicControl_colorscheme cs = a->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			SIZE size;
			string text = (a->window_text);
			GetTextExtentPoint32(hDC, str_to_wstr(text).c_str(), text.length(), &size);
			SetBkColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
			SetTextColor(hDC, a->toggle_state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
			if (a->font)
				SelectObject(hDC, a->font);
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
			HBRUSH framebrush = CreateSolidBrush(a->toggle_state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
			FrameRect(hDC, &rcclient, framebrush);
			DeleteObject(framebrush);

			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(a->handle, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return FALSE;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			if (a->Proc != nullptr)
				a->Proc((WPARAM)hwnd, 0);

			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			if (!a->mouse_in_client) {
				SendMessage(a->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(a->client_cursor);
					return TRUE;
			}
			return DefWindowProc(a->handle, message, wParam, lParam);
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS:{
			a->focus = true;

			break;
		}
		case WM_KILLFOCUS:{
			a->focus = false;

			break;
		}
		case WM_DESTROY:{
			a->handle = NULL;

			break;
		}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM init_register_custom_CustomContextMenu() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = CustomContextMenuclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = CustomContextMenuProc;
	return RegisterClassEx(&wnd);
}
ATOM ccxm = init_register_custom_CustomContextMenu();

#pragma endregion

#pragma region customcontrols container class

void CustomControls::addControl(TabButton* tabbtn) { tabbuttons.push_back(tabbtn); all_controls.push_back((LONG_PTR)tabbtn); }
void CustomControls::addControl(RadioButton* rbtn) { autoradiobuttons.push_back(rbtn); all_controls.push_back((LONG_PTR)rbtn); }
void CustomControls::addControl(CheckBox* cbtn) { checkboxbuttons.push_back(cbtn); all_controls.push_back((LONG_PTR)cbtn);  }
void CustomControls::addControl(Button* btn) { normalbuttons.push_back(btn); all_controls.push_back((LONG_PTR)btn); }
void CustomControls::addControl(StaticControl* stc) { staticcontrols.push_back(stc); all_controls.push_back((LONG_PTR)stc); }
void CustomControls::addControl(CustomTrackbar* ctb) { customtrackbars.push_back(ctb); all_controls.push_back((LONG_PTR)ctb); }
void CustomControls::addControl(CustomComboBox* ccb) { customcomboboxes.push_back(ccb); all_controls.push_back((LONG_PTR)ccb); }
void CustomControls::addControl(EditControl* edc) { EditControls.push_back(edc); all_controls.push_back((LONG_PTR)edc); }
void CustomControls::addControl(CustomContextMenu* ccxm) { CustomContextMenus.push_back(ccxm); all_controls.push_back((LONG_PTR)ccxm); }
void CustomControls::addControl(CustomScrollbar* csb) { CustomScrollbars.push_back(csb); all_controls.push_back((LONG_PTR)csb); }

BasicControl* CustomControls::getControl(LONG_PTR ptr) {
	BasicControl* bc = (BasicControl*)ptr;
	return bc;
}

bool CustomControls::createChildren(HWND parent) {
	for (auto & c : all_controls) {
		BasicControl* a = getControl(c);
		if (a->parent == parent) 
			if (!a->Create(parent))
				return false;
	}
	return true;
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////

#pragma endregion

