#include <iostream>
#include <sstream>

#include "Custom Trackbar.h"

using namespace std;

UINT WM_CUSTOM_TRACKBAR_VALUE_CHANGED = RegisterWindowMessageA("custom trackbar value changed");

#define WM_MOUSEENTER (WM_USER)
#define HTTHUMB (WM_USER + 1)

//trackbarcolorscheme c_scheme {
//	RGB(10, 10, 10), RGB(50, 50, 50), RGB(255, 0, 0)												// background/border/window text
//	RGB(127, 127, 127), RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255), 					// left/right channel left/right highlight
//	RGB(10, 10, 10), RGB(10, 10, 10), RGB(0, 118, 255),												// thumb background: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb border: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(10, 10, 10)											// thumb text: idle/hover/selected
//};

#pragma region custom_trackbar class implementation

custom_trackbar::custom_trackbar(
	string window_name,
	UINT window_styles,
	int xpos, int ypos,
	UINT width, UINT height,
	int id,
	int min_val, int max_val, int start_val,
	int small_step, int large_step,
	UINT thumb_width, bool smooth_dragging, bool smooth_snap,
	trackbarcolorscheme color_scheme,
	bool always_show_border /*= false*/, UINT channel_size /* = 5*/,
	HCURSOR client_cur/* = LoadCursor(NULL, IDC_ARROW)*/, HCURSOR thumb_cur/* = LoadCursor(NULL, IDC_HAND)*/,
	string group_name/*= ""*/) {
	handle = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;
	this->window_name = window_name; this->window_styles = window_styles;
	this->xpos = xpos; this->ypos = ypos;
	this->width = width; this->height = height;
	this->id = id;
	this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
	this->small_step = small_step; this->large_step = large_step;
	this->thumb_width = thumb_width; this->smooth_dragging = smooth_dragging; this->smooth_snap = smooth_snap;
	this->color_scheme = color_scheme;
	this->always_show_border = always_show_border; this->channel_size = channel_size;
	this->client_cur = client_cur; this->thumb_cur = thumb_cur;
	this->group_name = group_name;
}

custom_trackbar::custom_trackbar(const custom_trackbar& other) {
	handle = other.handle; thumb_region = other.thumb_region;
	window_name = other.window_name; window_styles = other.window_styles;
	xpos = other.xpos; ypos = other.ypos;
	width = other.width; height = other.height;
	id = other.id;
	min_val = other.min_val; max_val = other.max_val; start_val = other.start_val;
	small_step = other.small_step; large_step = other.large_step;
	thumb_width = other.thumb_width; smooth_dragging = other.smooth_dragging; smooth_snap = other.smooth_snap;
	color_scheme = other.color_scheme; client_cur = other.client_cur; thumb_cur = other.thumb_cur;
	current_val = other.current_val;
	pos = other.pos;
	always_show_border = other.always_show_border;
	group_name = other.group_name;
	start = other.start; focus = other.focus;
}

custom_trackbar& custom_trackbar::operator=(const custom_trackbar& other) {
	if (this == &other)
		return *this;
	handle = other.handle; thumb_region = other.thumb_region;
	window_name = other.window_name; window_styles = other.window_styles;
	xpos = other.xpos; ypos = other.ypos;
	width = other.width; height = other.height;
	id = other.id;
	min_val = other.min_val; max_val = other.max_val; start_val = other.start_val;
	small_step = other.small_step; large_step = other.large_step;
	thumb_width = other.thumb_width; smooth_dragging = other.smooth_dragging; smooth_snap = other.smooth_snap;
	color_scheme = other.color_scheme; client_cur = other.client_cur; thumb_cur = other.thumb_cur;
	current_val = other.current_val;
	pos = other.pos;
	always_show_border = other.always_show_border;
	group_name = other.group_name;
	start = other.start; focus = other.focus;
	return *this;
}

int custom_trackbar::set_thumb_pos(int pos) {
	return this->pos = pos <= 0 ? 0 : (int)(width - thumb_width) <= pos ? (width - thumb_width) : pos;
}

int custom_trackbar::set_val_from_pos(int pos) {
	RECT tr = ctb_getclientrect(handle);
	width = tr.right - tr.left;
	return current_val = ((pos * (max_val - min_val)) / (width - thumb_width)) + min_val;
}

int custom_trackbar::set_pos_with_val(int val) {
	RECT tr = ctb_getclientrect(handle);
	width = tr.right - tr.left;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	return pos = ((current_val - min_val) * (width - thumb_width)) / (max_val - min_val);
}

int custom_trackbar::move_thumb(int pos) {
	return set_val_from_pos(set_thumb_pos(pos));
}

void custom_trackbar::move_thumb_real(int pos) {
	set_pos_with_val(move_thumb(pos));
}

void custom_trackbar::thumb_hit_test() {
	if (ctb_cursor_in_region(thumb_region, ctb_getclientcursorpos(handle))) {
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

void custom_trackbar::set_focus(bool focus) {
	this->focus = focus;
	if (focus) {
		SetFocus(handle);
		if (dragging)
			SetCapture(handle);
	}
	else {
		ReleaseCapture();
		SetFocus(NULL);
		//setfocus to new window
		//DO IT
	}

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
}

bool custom_trackbar::set_range(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	set_val_from_pos(pos);

	RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE);
	return true;
}

void custom_trackbar::redraw_newval(int val, bool notify) {
	RECT tr = ctb_getclientrect(handle);
	width = tr.right - tr.left;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	pos = ((current_val - min_val) * (width - thumb_width)) / (max_val - min_val);

	if (notify)
		PostValueChanged();
	RedrawWindow(handle, &tr, NULL, RDW_INVALIDATE);
}

void custom_trackbar::PostValueChanged() {
	PostMessage(GetParent(handle), WM_CUSTOM_TRACKBAR_VALUE_CHANGED, NULL, (LPARAM)this);
}

#pragma endregion

LRESULT CALLBACK customtrackbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//make vertical
	//tab focus
	//fix focus

	custom_trackbar* ctb = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		ctb = reinterpret_cast<custom_trackbar*>(pCreate->lpCreateParams);
		ctb->handle = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ctb);
	}
	else {
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		ctb = reinterpret_cast<custom_trackbar*>(ptr);
	}

	switch (message) {
		case WM_PAINT: {
			trackbarcolorscheme cs = ctb->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(ctb->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = ctb_getclientrect(ctb->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//colorbackground
			HBRUSH tempbrush0 = CreateSolidBrush(cs.background);
			FillRect(hDC, &rcclient, tempbrush0);

			//calculate thumb position
			ctb->thumb_region.left = rcclient.left + ctb->pos;
			ctb->thumb_region.top = rcclient.top + 1;
			ctb->thumb_region.right = rcclient.left + ctb->thumb_width + ctb->pos;
			ctb->thumb_region.bottom = rcclient.bottom - 1;

			//draw channel
			RECT rc_channel_left;
			RECT rc_channel_right;

			rc_channel_left.left = rcclient.left;
			rc_channel_left.right = rcclient.left + ctb->pos;
			rc_channel_left.top = rcclient.top + ctb->channel_size;
			rc_channel_left.bottom = rcclient.bottom - ctb->channel_size;

			rc_channel_right.left = rcclient.left + ctb->thumb_width + ctb->pos;
			rc_channel_right.right = rcclient.right;
			rc_channel_right.top = rcclient.top + ctb->channel_size;
			rc_channel_right.bottom = rcclient.bottom - ctb->channel_size;

			HBRUSH tempbrush1 = CreateSolidBrush(ctb->mouse_in_client ? cs.left_channel_highlight : cs.left_channel_idle);
			HBRUSH tempbrush2 = CreateSolidBrush(ctb->mouse_in_client ? cs.right_channel_highlight : cs.right_channel_idle);
			FillRect(hDC, &rc_channel_left, tempbrush1);
			FillRect(hDC, &rc_channel_right, tempbrush2);

			//draw focus rect
			if (ctb->focus || ctb->always_show_border) {
				SetDCPenColor(hDC, cs.border);//make transparent
				Rectangle(hDC, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);
			}

			//draw thumb
			ctb->thumb_hit_test();
			SIZE size;
			string text = ctb_int_to_str(ctb->current_val);
			GetTextExtentPoint32(hDC, ctb_str_to_wstr(text).c_str(), (int)text.length(), &size);
			SetTextColor(hDC, ctb->thumb_selected ? cs.thumb_text_selected : ctb->thumb_hover ? cs.thumb_text_hover : cs.thumb_text_idle);
			SetBkColor(hDC, ctb->thumb_selected ? cs.thumb_background_selected : ctb->thumb_hover ? cs.thumb_background_hover : cs.thumb_background_idle);
			ExtTextOut(hDC,
				((ctb->thumb_region.right - ctb->thumb_region.left) - size.cx) / 2 + ctb->pos,
				((ctb->thumb_region.bottom - ctb->thumb_region.top) - size.cy) / 2 + 1,
				ETO_OPAQUE | ETO_CLIPPED, &ctb->thumb_region, ctb_str_to_wstr(text).c_str(), text.length(), NULL);
			SetDCPenColor(hDC, ctb->thumb_selected ? cs.thumb_border_selected : ctb->thumb_hover || ctb->dragging ? cs.thumb_border_hover : cs.thumb_border_idle);
			Rectangle(hDC, ctb->thumb_region.left, ctb->thumb_region.top, ctb->thumb_region.right, ctb->thumb_region.bottom);

			//draw window name
			if (ctb->window_name.size() > 0) {
				SetBkMode(hDC, TRANSPARENT);
				SetTextColor(hDC, ctb->mouse_in_client ? cs.window_name_highlight : cs.window_name_idle);
				RECT rc = ctb_getclientrect(ctb->handle);
				DrawText(hDC, ctb_str_to_wstr(ctb->window_name).c_str(), ctb->window_name.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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

			EndPaint(ctb->handle, &ps);

			break;
		}
		case WM_LBUTTONDOWN: {
			ctb->focus = true;
			ctb->mouse_in_client = true;
			ctb->dragging = true;
			ctb->thumb_hover = true;
			ctb->thumb_selected = true;

			ctb->set_focus(true);

			if (ctb->smooth_dragging)
				ctb->move_thumb(ctb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));
			else
				ctb->move_thumb_real(ctb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));

			customtrackbarProc(ctb->handle, WM_SETCURSOR, (WPARAM)ctb->handle, (LPARAM)MAKELONG(HTTHUMB, NULL));

			ctb->PostValueChanged();

			RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP: {
			ctb->dragging = false;
			ctb->thumb_selected = false;
			ReleaseCapture();
			if (!ctb->smooth_dragging || ctb->smooth_snap)
				ctb->set_pos_with_val(ctb->current_val);

			RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE: {
			if (!ctb->mouse_in_client) {
				SendMessage(ctb->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = ctb->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			ctb->custom_trackbar::thumb_hit_test();

			if (ctb->dragging) {
				if (ctb->smooth_dragging)
					ctb->move_thumb(ctb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));
				else
					ctb->move_thumb_real(ctb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));

				ctb->PostValueChanged();

				RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN: {
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_LEFT:
				case VK_DOWN:
					ctb->set_pos_with_val(ctb->current_val - ctb->small_step);
					break;
				case VK_RIGHT:
				case VK_UP:
					ctb->set_pos_with_val(ctb->current_val + ctb->small_step);
					break;
				case VK_HOME:
					ctb->set_pos_with_val(ctb->min_val);
					break;
				case VK_END:
					ctb->set_pos_with_val(ctb->max_val);
					break;
				case VK_PRIOR:
					ctb->set_pos_with_val(ctb->current_val + ctb->large_step);
					break;
				case VK_NEXT:
					ctb->set_pos_with_val(ctb->current_val - ctb->large_step);
					break;
				default:
					wrongkey_skip = true;
					break;
			}
			if (!wrongkey_skip) {
				ctb->PostValueChanged();
				RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEWHEEL: {
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON)
					ctb->set_pos_with_val(ctb->current_val + ctb->large_step);
				else
					ctb->set_pos_with_val(ctb->current_val + ctb->small_step);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON)
					ctb->set_pos_with_val(ctb->current_val - ctb->large_step);
				else
					ctb->set_pos_with_val(ctb->current_val - ctb->small_step);
			}
			else
				break;
			ctb->PostValueChanged();
			RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_SETCURSOR: {
			if (ctb->dragging)
				lParam = MAKELPARAM(HTTHUMB, HIWORD(lParam));
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(ctb->client_cur);
					return TRUE;
				case HTTHUMB:
					SetCursor(ctb->thumb_cur);
					return TRUE;
			}
			return DefWindowProc(ctb->handle, message, wParam, lParam);
		}
		case WM_KILLFOCUS: {
			ctb->set_focus(false);

			//kill focus rect on previous
			
			if (ctb->focus) {
				ctb->focus = false;
				ctb->mouse_in_client = false;
				ctb->dragging = false;
				ctb->thumb_hover = false;
				ctb->thumb_selected = false;

				RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
				SendMessage(ctb->handle, WM_PAINT, NULL, NULL);
			}
			
			break;
		}
		case WM_MOUSELEAVE: {
			if (!ctb->dragging) {
				ctb->mouse_in_client = false;
				ctb->thumb_selected = false;
				ctb->thumb_hover = false;

				RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER: {
			ctb->mouse_in_client = true;

			RedrawWindow(ctb->handle, NULL, NULL, RDW_INVALIDATE);
			break;
		}
	}
	return DefWindowProc(ctb->handle, message, wParam, lParam);
}

string ctb_int_to_str(int num) {
	cout.clear();
	stringstream out;
	out << num;
	string return_value = out.str();
	return return_value;
}
wstring ctb_str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}

POINT ctb_getclientcursorpos(HWND hwnd_parent) {
	RECT temprect;
	GetClientRect(hwnd_parent, &temprect);
	MapWindowPoints(hwnd_parent, NULL, (LPPOINT)&temprect, 2);
	POINT temppoint;
	GetCursorPos(&temppoint);
	POINT client_cursor_pos;
	client_cursor_pos.x = temppoint.x - temprect.left;
	client_cursor_pos.y = temppoint.y - temprect.top;
	return client_cursor_pos;
}
RECT ctb_getclientrect(HWND wnd) {
	RECT ret;
	GetClientRect(wnd, &ret);
	return ret;
}
bool ctb_cursor_in_region(RECT region, POINT cursor_pos) {
	return (cursor_pos.x > region.left && cursor_pos.x < region.right && cursor_pos.y < region.bottom && cursor_pos.y > region.top);
}

ATOM init_register_custom_trackbar() {
	WNDCLASS wnd = {0};
	wnd.lpszClassName = custom_trackbar_classname;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customtrackbarProc;
	return RegisterClass(&wnd);
}

ATOM a = init_register_custom_trackbar();

