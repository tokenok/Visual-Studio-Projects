#include "Custom Trackbar.h"


///////////////////////////////////////
////FIX FOCUS AND CUSTOM CUROSR
////MAKE VERTICLE


/*
main:
init_register_custom_trackbar();

WM_CREATE:
for (UINT i = 0; i < custom_trackbars.size(); i++) {
	custom_trackbar* a = custom_trackbars[i];
	a->handle = CreateWindow(custom_trackbar_classname, TEXT(""),
	(a->showstate ? WS_VISIBLE : NULL) | WS_CHILD,
	a->xpos, a->ypos, a->width, a->height,
	hwnd, (HMENU)a->id, NULL, NULL);
}


*/

trackbarcolorscheme c_scheme {
	RGB(10, 10, 10), RGB(50, 50, 50),																// background/border
	RGB(127, 127, 127), RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255), 					// left/right channel left/right highlight
	RGB(10, 10, 10), RGB(10, 10, 10), RGB(0, 118, 255),												// thumb background: idle/hover/selected
	RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb border: idle/hover/selected
	RGB(127, 127, 127), RGB(0, 118, 255), RGB(10, 10, 10)											// thumb text: idle/hover/selected
};

vector<custom_trackbar*> custom_trackbars;

#pragma region custom_trackbar class implementation

custom_trackbar::custom_trackbar(
	int id,
	int xpos, int ypos,
	int width, int height,
	int min_val, int max_val, int start_val,
	int small_step, int large_step,
	int thumb_width, bool smooth_dragging, bool smooth_snap,
	trackbarcolorscheme color_scheme, 
	bool showstate, 
	int client_cur_id, int thumb_cur_id, 
	string group_name){
		handle = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;
		this->id = id;
		this->xpos = xpos; this->ypos = ypos;
		this->width = width; this->height = height;
		this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
		this->small_step = small_step; this->large_step = large_step;
		this->thumb_width = thumb_width; this->smooth_dragging = smooth_dragging; this->smooth_snap = smooth_snap;
		this->color_scheme = color_scheme; this->client_cur_id = client_cur_id; this->thumb_cur_id = thumb_cur_id;
		this->showstate = showstate; this->group_name = group_name;

		if (client_cur_id)
			client_cur = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(client_cur_id));
		if (thumb_cur_id)
			thumb_cur = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(thumb_cur_id));

		custom_trackbars.push_back(this);
}

custom_trackbar::custom_trackbar(const custom_trackbar& other) {
	handle = other.handle; thumb_region = other.thumb_region;
	id = other.id;
	xpos = other.xpos; ypos = other.ypos;
	width = other.width; height = other.height;
	min_val = other.min_val; max_val = other.max_val; start_val = other.start_val;
	small_step = other.small_step; large_step = other.large_step;
	thumb_width = other.thumb_width; smooth_dragging = other.smooth_dragging; smooth_snap = other.smooth_snap;
	color_scheme = other.color_scheme; client_cur = other.client_cur; thumb_cur = other.thumb_cur;
	client_cur_id = other.client_cur_id; thumb_cur_id = other.thumb_cur_id;
	current_val = other.current_val;
	pos = other.pos;
	showstate = other.showstate;
	group_name = other.group_name;
	start = other.start; focus = other.focus;
}

custom_trackbar& custom_trackbar::operator=(const custom_trackbar& other) {
	if (this == &other)
		return *this;
	handle = other.handle; thumb_region = other.thumb_region;
	id = other.id;
	xpos = other.xpos; ypos = other.ypos;
	width = other.width; height = other.height;
	min_val = other.min_val; max_val = other.max_val; start_val = other.start_val;
	small_step = other.small_step; large_step = other.large_step;
	thumb_width = other.thumb_width; smooth_dragging = other.smooth_dragging; smooth_snap = other.smooth_snap;
	color_scheme = other.color_scheme; client_cur = other.client_cur; thumb_cur = other.thumb_cur;
	client_cur_id = other.client_cur_id; thumb_cur_id = other.thumb_cur_id;
	current_val = other.current_val;
	pos = other.pos;
	showstate = other.showstate;
	group_name = other.group_name;
	start = other.start; focus = other.focus;
	return *this;
}

int custom_trackbar::set_pos(int pos) {
	return this->pos = pos <= 0 ? 0 : (width - thumb_width) <= pos ? (width - thumb_width) : pos;
}

int custom_trackbar::set_val(int pos) {
	RECT tr;
	GetClientRect(this->handle, &tr);
	width = tr.right - tr.left;
	return current_val = ((pos * (max_val - min_val)) / (width - thumb_width)) + min_val;
}

int custom_trackbar::set_pos_with_val(int val) {
	RECT tr;
	GetClientRect(this->handle, &tr);
	width = tr.right - tr.left;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	return pos = ((current_val - min_val) * (width - thumb_width)) / (max_val - min_val);
}

int custom_trackbar::move_thumb(int pos) {
	return set_val(set_pos(pos));
}

void custom_trackbar::move_thumb_real(int pos) {
	set_pos_with_val(move_thumb(pos));
}

void custom_trackbar::thumb_hit_test() {
	if (tb_cursor_in_region(thumb_region, tb_getclientcursorpos(handle))) {
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
		//setfocus to new window?
	}
	SendMessage(handle, WM_UPDATE, (WPARAM)handle, (LPARAM)RDW_INVALIDATE);
}

bool custom_trackbar::set_range(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	set_val(pos);
	SendMessage(handle, WM_UPDATE, (WPARAM)handle, (LPARAM)RDW_INVALIDATE);
	return true;
}

#pragma endregion

LRESULT CALLBACK customtrackbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//make vertical
	//tab focus

	custom_trackbar* ctb;

	for (UINT i = 0; i < custom_trackbars.size(); i++) {
		if (custom_trackbars[i]->handle == hwnd) {
			ctb = custom_trackbars[i];
			break;
		}
		if (i == custom_trackbars.size() - 1)
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	switch (message) {
		case WM_PAINT:{
			trackbarcolorscheme cs = ctb->color_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(ctb->handle, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = tb_getclientrect(ctb->handle);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			//colorbackground
			HBRUSH tempbrush0 = CreateSolidBrush(cs.background);
			FillRect(hDC, &rcclient, tempbrush0);

			//calculate thumb position
			if (ctb->start) {
				ctb->set_pos_with_val(ctb->start_val);
				ctb->start = false;
			}
			ctb->thumb_region.left = rcclient.left + ctb->pos;
			ctb->thumb_region.top = rcclient.top + 1;
			ctb->thumb_region.right = rcclient.left + ctb->thumb_width + ctb->pos;
			ctb->thumb_region.bottom = rcclient.bottom - 1;

			//draw channel
			int channel_height = 5;

			RECT rc_channel_left;
			RECT rc_channel_right;

			rc_channel_left.left = rcclient.left;
			rc_channel_left.right = rcclient.left + ctb->pos;
			rc_channel_left.top = rcclient.top + channel_height;
			rc_channel_left.bottom = rcclient.bottom - channel_height;

			rc_channel_right.left = rcclient.left + ctb->thumb_width + ctb->pos;
			rc_channel_right.right = rcclient.right;
			rc_channel_right.top = rcclient.top + channel_height;
			rc_channel_right.bottom = rcclient.bottom - channel_height;

			HBRUSH tempbrush1 = CreateSolidBrush(ctb->mouse_in_client ? cs.left_channel_highlight : cs.left_channel);
			HBRUSH tempbrush2 = CreateSolidBrush(ctb->mouse_in_client ? cs.right_channel_highlight : cs.right_channel);
			FillRect(hDC, &rc_channel_left, tempbrush1);
			FillRect(hDC, &rc_channel_right, tempbrush2);

			//draw focus rect
			SetDCPenColor(hDC, ctb->focus ? cs.border : RGB(10, 10, 10));
			Rectangle(hDC, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);

			//draw thumb
			ctb->thumb_hit_test();
			SIZE size;
			string text = tb_int_to_str(ctb->current_val);
			GetTextExtentPoint32(hDC, tb_str_to_wstr(text).c_str(), text.length(), &size);
			SetTextColor(hDC, ctb->thumb_selected ? cs.thumb_text_selected : ctb->thumb_hover ? cs.thumb_text_hover : cs.thumb_text_idle);
			SetBkColor(hDC, ctb->thumb_selected ? cs.thumb_background_selected : ctb->thumb_hover ? cs.thumb_background_hover : cs.thumb_background_idle);
			ExtTextOut(hDC,
				((ctb->thumb_region.right - ctb->thumb_region.left) - size.cx) / 2 + ctb->pos,
				((ctb->thumb_region.bottom - ctb->thumb_region.top) - size.cy) / 2 + 1,
				ETO_OPAQUE | ETO_CLIPPED, &ctb->thumb_region, tb_str_to_wstr(text).c_str(), text.length(), NULL);
			SetDCPenColor(hDC, ctb->thumb_selected ? cs.thumb_border_selected : ctb->thumb_hover || ctb->dragging ? cs.thumb_border_hover : cs.thumb_border_idle);
			Rectangle(hDC, ctb->thumb_region.left, ctb->thumb_region.top, ctb->thumb_region.right, ctb->thumb_region.bottom);

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
				ctb->move_thumb(tb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));
			else
				ctb->move_thumb_real(tb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));

			customtrackbarProc(ctb->handle, WM_SETCURSOR, (WPARAM)ctb->handle, (LPARAM)MAKELONG(HTTHUMB, NULL));

			SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP: {
			ctb->dragging = false;
			ctb->thumb_selected = false;
			ReleaseCapture();
			if (!ctb->smooth_dragging || ctb->smooth_snap)
				ctb->set_pos_with_val(ctb->current_val);
			SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:{
			if (!ctb->mouse_in_client) {
				SendMessage(ctb->handle, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = ctb->handle;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			ctb->thumb_hit_test();

			if (ctb->dragging) {
				if (ctb->smooth_dragging)
					ctb->move_thumb(tb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));
				else
					ctb->move_thumb_real(tb_getclientcursorpos(ctb->handle).x - (ctb->thumb_width / 2));
				SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN:{
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
					break;
			}
			SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON)
					ctb->set_pos_with_val(ctb->current_val + (((ctb->max_val - ctb->min_val) / 10) == 0 ? 1 : ((ctb->max_val - ctb->min_val) / 10)));
				else
					ctb->set_pos_with_val(ctb->current_val + ctb->small_step);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT || GET_KEYSTATE_WPARAM(wParam) == MK_MBUTTON)
					ctb->set_pos_with_val(ctb->current_val - (((ctb->max_val - ctb->min_val) / 10) == 0 ? 1 : ((ctb->max_val - ctb->min_val) / 10)));
				else
					ctb->set_pos_with_val(ctb->current_val - ctb->small_step);
			}
			else
				break;
			SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);
			break;
		}
		case WM_SETCURSOR:{
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
		case WM_KILLFOCUS:{
			ctb->set_focus(false);

			//kill focus rect on previous
			for (UINT i = 0; i < custom_trackbars.size(); i++) {
				if (custom_trackbars[i]->focus) {
					custom_trackbars[i]->focus = false;
					custom_trackbars[i]->mouse_in_client = false;
					custom_trackbars[i]->dragging = false;
					custom_trackbars[i]->thumb_hover = false;
					custom_trackbars[i]->thumb_selected = false;
					customtrackbarProc(custom_trackbars[i]->handle, WM_UPDATE, (WPARAM)custom_trackbars[i]->handle, (LPARAM)RDW_INVALIDATE);
					SendMessage(custom_trackbars[i]->handle, WM_PAINT, NULL, NULL);
				}
			}
			break;
		}
		case WM_MOUSELEAVE:{
			if (!ctb->dragging) {
				ctb->mouse_in_client = false;
				ctb->thumb_selected = false;
				ctb->thumb_hover = false;
				SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER:{
			ctb->mouse_in_client = true;

			SendMessage(ctb->handle, WM_UPDATE, (WPARAM)ctb->handle, (LPARAM)RDW_INVALIDATE);
			break;
		}
		case WM_UPDATE:{
			RedrawWindow((HWND)wParam, &tb_getclientrect((HWND)wParam), NULL, lParam);
			break;
		}
	}
	return DefWindowProc(ctb->handle, message, wParam, lParam);
}

string tb_int_to_str(int num) {
	cout.clear();
	stringstream out;
	out << num;
	string return_value = out.str();
	return return_value;
}
wstring tb_str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}

POINT tb_getclientcursorpos(HWND hwnd_parent) {
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
RECT tb_getclientrect(HWND wnd) {
	RECT ret;
	GetClientRect(wnd, &ret);
	return ret;
}
bool tb_cursor_in_region(RECT region, POINT cursor_pos) {
	return (cursor_pos.x > region.left && cursor_pos.x < region.right && cursor_pos.y < region.bottom && cursor_pos.y > region.top);
}

ATOM init_register_custom_trackbar() {
	WNDCLASS wnd = {0};
	wnd.lpszClassName = custom_trackbar_classname;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customtrackbarProc;
	return RegisterClass(&wnd);
}