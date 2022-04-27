#include "customstaticcontrol.h"

#define WM_MOUSEENTER (WM_USER)

StaticControl::StaticControl() {}
StaticControl::StaticControl(std::string window_text, UINT window_styles, int xpos, int ypos, int width, int height, UINT id, void(*Proc)(WPARAM wParam, LPARAM lParam),
	BasicControl_colorscheme color_scheme, HFONT font, HCURSOR client_cursor, BasicControl_hover hover_info, UINT owner_name, UINT group_name) {
	className = wstr_to_str(staticcontrolclassName); handle = NULL;

	this->window_text = window_text; this->window_styles = window_styles; this->xpos = xpos; this->ypos = ypos; this->width = width; this->height = height; this->id = id; this->Proc = Proc;
	this->color_scheme = color_scheme; this->font = font; this->client_cursor = client_cursor; this->hover_info = hover_info; this->owner_name = owner_name; this->group_name = group_name;
	if (client_cursor == NULL)
		this->client_cursor = LoadCursor(NULL, IDC_ARROW);
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
	case WM_PAINT: {
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
		SetBkColor(hDC, a->state ? a->mouse_in_client ? cs.background_hover_on : cs.background_idle_on : a->mouse_in_client ? cs.background_hover_off : cs.background_idle_off);
		SetTextColor(hDC, a->state ? a->mouse_in_client ? cs.text_hover_on : cs.text_idle_on : a->mouse_in_client ? cs.text_hover_off : cs.text_idle_off);
		if (a->font)
			SelectObject(hDC, a->font);
		if (GetWindowLongPtr(hwnd, GWL_STYLE) & SS_CENTER)
			ExtTextOut(hDC, ((rcclient.right - rcclient.left) - size.cx) / 2, ((rcclient.bottom - rcclient.top) - size.cy) / 2, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);
		else
			ExtTextOut(hDC, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rcclient, str_to_wstr(text).c_str(), text.length(), NULL);

		if (a->focus) {
			HBRUSH framebrush = CreateSolidBrush(a->state ? a->mouse_in_client ? cs.border_hover_on : cs.border_idle_on : a->mouse_in_client ? cs.border_hover_off : cs.border_idle_off);
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
	case WM_ERASEBKGND: {
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
	case WM_MOUSEMOVE: {
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
	case WM_SETCURSOR: {
		switch (LOWORD(lParam)) {
		case HTCLIENT:
			SetCursor(a->client_cursor);
			return TRUE;
		}
		return DefWindowProc(a->handle, message, wParam, lParam);
	}
	case WM_MOUSEHOVER: {
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
		temp->id = 0;
		temp->client_cursor = NULL;
		temp->font = NULL;
		temp->Proc = nullptr;
		temp->color_scheme = BasicControl_colorscheme(RGB(10, 10, 10), RGB(148, 0, 211), RGB(148, 0, 211));

		a->hover_info.hover_text = a->window_text;

		a->hover_info.handle = temp->Create(GetParent(hwnd));

		ReleaseDC(hwnd, hDC);
		break;
	}
	case WM_MOUSELEAVE: {
		a->mouse_in_client = false;
		RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

		if (a->hover_info.handle)
			DestroyWindow(a->hover_info.handle);

		break;
	}
	case WM_MOUSEENTER: {
		a->mouse_in_client = true;
		RedrawWindow(a->handle, NULL, NULL, RDW_INVALIDATE);

		break;
	}
	case WM_CLOSE:
	case WM_DESTROY: {
		a->focus = false;
		a->mouse_in_client = false;

		a->handle = NULL;

		break;
	}
	}
	return DefWindowProc(a->handle, message, wParam, lParam);
}

ATOM StaticControl::RegisterCustomClass() {
	WNDCLASSEX wnd = { 0 };
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = staticcontrolclassName;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = staticcontrolProc;
	return RegisterClassEx(&wnd);
}
