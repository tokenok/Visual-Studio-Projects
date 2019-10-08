#include "basiccontrol.h"

BasicControl_colorscheme::BasicControl_colorscheme(COLORREF background_idle_on, COLORREF border_idle_on, COLORREF text_idle_on) {
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

BasicControl_hover::BasicControl_hover(std::string hover_text, DWORD Flags, DWORD HoverTime) {
	this->hover_text = hover_text;
	this->Flags = Flags;
	this->HoverTime = HoverTime;
}

HWND BasicControl::Create(HWND parent) {
	this->parent = parent;
	handle = CreateWindowEx(window_exstyles, STW(className), STW(window_text), window_styles, xpos, ypos, width, height, parent, (HMENU)id, NULL, this);
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

BasicControl* getBasicControl(HWND wnd) {
	LONG_PTR ptr = GetWindowLongPtr(wnd, GWL_USERDATA);
	return reinterpret_cast<BasicControl*>(ptr);
}