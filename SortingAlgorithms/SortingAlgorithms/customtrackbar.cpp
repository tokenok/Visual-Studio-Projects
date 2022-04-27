#include "customtrackbar.h"

using namespace std;


//TrackbarColorScheme c_scheme {
//	RGB(10, 10, 10), RGB(50, 50, 50), RGB(255, 0, 0), RGB(0, 0, 0),									// background/border/window text idle/highlight
//	RGB(127, 127, 127), RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255), 					// left/right channel left/right highlight
//	RGB(10, 10, 10), RGB(10, 10, 10), RGB(0, 118, 255),												// thumb background: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(0, 118, 255),											// thumb border: idle/hover/selected
//	RGB(127, 127, 127), RGB(0, 118, 255), RGB(10, 10, 10)											// thumb text: idle/hover/selected
//};

inline std::string wstr_to_str(std::wstring wstr) {
	return std::string(wstr.begin(), wstr.end());
}

inline std::wstring str_to_wstr(std::string str) {
	return std::wstring(str.begin(), str.end());
}

inline RECT getclientrect(HWND hwnd) {
	RECT ret;
	GetClientRect(hwnd, &ret);
	return ret;
}

inline POINT getclientcursorpos(HWND hwnd_parent) {
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

inline bool is_cursor_in_region(RECT region, POINT cursor_pos) {
	return (cursor_pos.x > region.left && cursor_pos.x < region.right && cursor_pos.y < region.bottom && cursor_pos.y > region.top);
}

inline std::string int_to_str(int i) {
	return std::to_string(i);
}

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


BasicControl* getBasicControl(HWND wnd) {
	LONG_PTR ptr = GetWindowLongPtr(wnd, GWLP_USERDATA);
	return reinterpret_cast<BasicControl*>(ptr);
}

BasicControl::BasicControl() {}

HWND BasicControl::Create(HWND parent) {
	hwnd = CreateWindowEx(window_exstyles, str_to_wstr(className).c_str(), str_to_wstr(window_text).c_str(),
							window_styles, xpos, ypos, width, height,
							parent, (HMENU)id, NULL, this);	
	return hwnd;
}

void BasicControl::Show() {
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) | WS_VISIBLE);
	ShowWindow(hwnd, SW_SHOW);
	EnableWindow(hwnd, 1);
}

void BasicControl::Hide() {
	if (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_VISIBLE)
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) ^ WS_VISIBLE);
	ShowWindow(hwnd, SW_HIDE);
	EnableWindow(hwnd, 0);
}



CustomTrackbar::CustomTrackbar(string window_name, UINT window_styles, int xpos, int ypos, UINT width, UINT height, int id,
							   int min_val, int max_val, int start_val, int small_step, int large_step, UINT thumb_size, UINT channel_size, UINT flags,
							   TrackbarColorScheme color_scheme, HCURSOR client_cursor, HCURSOR thumb_cursor) {

	className = wstr_to_str(CustomTrackbar_classname); hwnd = NULL; thumb_region = {0, 0, 0, 0}; pos = NULL; start = true; focus = false;

	this->window_name = window_name; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id;
	this->min_val = min_val; this->max_val = max_val; this->start_val = start_val; this->current_val = start_val;
	this->small_step = small_step; this->large_step = large_step; this->thumb_size = thumb_size; this->channel_size = channel_size; this->flags = flags;
	this->tcolor_scheme = color_scheme; this->client_cursor = client_cursor; this->thumb_cursor = thumb_cursor;

	if (this->client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_HAND);
	if (this->thumb_cursor == NULL)
		this->thumb_cursor = LoadCursor(NULL, IDC_HAND);
}

int CustomTrackbar::setPos(int pos) {
	int x = flags & CTB_VERT ? height : width;
	return this->pos = pos <= 0 ? 0 : (int)(x - thumb_size) <= pos ? (x - thumb_size) : pos;
}

int CustomTrackbar::setVal(int pos) {
	RECT tr = getclientrect(hwnd);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	return current_val = ((pos * (max_val - min_val)) / (x - thumb_size == 0 ? 1 : (x - thumb_size))) + min_val;
}

int CustomTrackbar::setPosWithVal(int val) {
	RECT tr = getclientrect(hwnd);
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
	if (is_cursor_in_region(thumb_region, getclientcursorpos(hwnd))) {
		if (!thumb_hover) {
			thumb_hover = true;
			InvalidateRect(hwnd, &thumb_region, true);
		}
		SendMessage(hwnd, WM_SETCURSOR, (WPARAM)hwnd, (LPARAM)MAKELONG(HTTHUMB, NULL));
	}
	else if (thumb_hover) {
		thumb_hover = false;
		InvalidateRect(hwnd, &thumb_region, true);
		SendMessage(hwnd, WM_SETCURSOR, (WPARAM)hwnd, (LPARAM)MAKELONG(HTCLIENT, NULL));
	}
}

void CustomTrackbar::setFocus(bool focus, HWND hwnd_focus) {
	this->focus = focus;
	if (focus) {
		//SetFocus(hwnd);
		if (dragging)
			SetCapture(hwnd);
	}
	else {
		ReleaseCapture();
	//	SetFocus(hwnd_focus);
		//setfocus to new window here		
	}

	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

bool CustomTrackbar::setRange(int min, int max) {
	if (min > max)
		return false;
	min_val = min;
	max_val = max;
	setVal(pos);

	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	return true;
}

void CustomTrackbar::redrawNewVal(int val) {
	RECT tr = getclientrect(hwnd);
	width = tr.right - tr.left;
	height = tr.bottom - tr.top;
	int x = flags & CTB_VERT ? height : width;
	current_val = val <= min_val ? min_val : val >= max_val ? max_val : val;
	pos = ((current_val - min_val) * (x - thumb_size)) / (max_val - min_val == 0 ? 1 : (max_val - min_val));

	if (scrollbar_owner_handle)
		PostMessage(scrollbar_owner_handle, WM_COMMAND, MAKEWPARAM(this->id, CTB_CHANGE), (LPARAM)this->hwnd);
	PostMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(this->id, CTB_CHANGE), (LPARAM)this->hwnd);
	RedrawWindow(hwnd, &tr, NULL, RDW_INVALIDATE);
}

LRESULT CALLBACK CustomTrackbar::customtrackbarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CustomTrackbar* a = (CustomTrackbar*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (message) {
		case WM_NCCREATE: {
			a = (CustomTrackbar*)((CREATESTRUCT*)lParam)->lpCreateParams;
			a->hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)a);

			break;
		}
		case WM_PAINT:{
			TrackbarColorScheme cs = a->tcolor_scheme;

			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(a->hwnd, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient = getclientrect(a->hwnd);

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
			a->thumb_region.left = (a->flags & CTB_VERT ? rcclient.left + 1 : rcclient.left + a->pos);
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
				RECT rc = getclientrect(a->hwnd);
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

			EndPaint(a->hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND: {
			return TRUE;
		}
		case WM_LBUTTONDOWN:{
			a->focus = true;
			a->mouse_in_client = true;
			a->dragging = true;
			a->thumb_hover = true;
			a->thumb_selected = true;

			a->setFocus(true);

			POINT pt = getclientcursorpos(a->hwnd);

			if (a->flags & CTB_SMOOTH)//smooth dragging
				a->moveThumb((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));
			else
				a->moveThumbReal((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));

			customtrackbarProc(a->hwnd, WM_SETCURSOR, (WPARAM)a->hwnd, (LPARAM)MAKELONG(HTTHUMB, NULL));

			if (a->scrollbar_owner_handle)
				PostMessage(a->scrollbar_owner_handle, WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);
			PostMessage(GetParent(a->hwnd), WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);

			RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_LBUTTONUP:{
			a->dragging = false;
			a->thumb_selected = false;
			ReleaseCapture();
			if (!(a->flags & CTB_SMOOTH) || !(a->flags & CTB_STAY))//not smooth dragging OR not smooth stay
				a->setPosWithVal(a->current_val);

			RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_MOUSEMOVE:	{
			if (!a->mouse_in_client) {
				SendMessage(a->hwnd, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = a->hwnd;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);
			}

			a->CustomTrackbar::thumbHitText();

			if (a->dragging) {

				POINT pt = getclientcursorpos(a->hwnd);

				if (a->flags & CTB_SMOOTH)//smooth dragging
					a->moveThumb((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));
				else
					a->moveThumbReal((a->flags & CTB_VERT ? pt.y : pt.x) - (a->thumb_size / 2));

				if (a->scrollbar_owner_handle)
					PostMessage(a->scrollbar_owner_handle, WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);
				PostMessage(GetParent(a->hwnd), WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);

				RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			switch (wParam) {
				case VK_LEFT:
				{
					a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_DOWN:
				{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val + a->small_step);
					else
						a->setPosWithVal(a->current_val - a->small_step);
					break;
				}
				case VK_RIGHT:
				{
					a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_UP:
				{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->small_step);
					else
						a->setPosWithVal(a->current_val + a->small_step);
					break;
				}
				case VK_HOME:
				{
					a->setPosWithVal(a->min_val);
					break;
				}
				case VK_END:
				{
					a->setPosWithVal(a->max_val);
					break;
				}
				case VK_PRIOR:
				{
					if (a->flags & CTB_VERT)
						a->setPosWithVal(a->current_val - a->large_step);
					else
						a->setPosWithVal(a->current_val + a->large_step);
					break;
				}
				case VK_NEXT:
				{
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
					PostMessage(a->scrollbar_owner_handle, WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);
				PostMessage(GetParent(a->hwnd), WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);
				RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);
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
				PostMessage(a->scrollbar_owner_handle, WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);
			PostMessage(GetParent(a->hwnd), WM_COMMAND, MAKEWPARAM(a->id, CTB_CHANGE), (LPARAM)a->hwnd);
			RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);
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
			return DefWindowProc(a->hwnd, message, wParam, lParam);
		}
		case WM_KILLFOCUS:{
			a->setFocus(false, (HWND)wParam);

			a->mouse_in_client = false;
			a->dragging = false;
			a->thumb_hover = false;
			a->thumb_selected = false;

			break;
		}
		case WM_MOUSELEAVE:	{
			if (!a->dragging) {
				a->mouse_in_client = false;
				a->thumb_selected = false;
				a->thumb_hover = false;

				RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;

			RedrawWindow(a->hwnd, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_DESTROY:{
			a->hwnd = NULL;

			break;
		}
	}
	return DefWindowProc(a->hwnd, message, wParam, lParam);
}

ATOM register_customtrackbar() {
	WNDCLASS wnd = {0};
	wnd.lpszClassName = CustomTrackbar_classname;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = CustomTrackbar::customtrackbarProc;
	return RegisterClass(&wnd);
}






































