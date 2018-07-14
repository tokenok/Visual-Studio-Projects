#include "CustomEditControl.h"

#include <Strsafe.h>
#include <windowsx.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <Richedit.h>

#include "font_image.h"

#include "C:/CPPlibs/common/common.h"

#define WM_MOUSEENTER (WM_USER)

using namespace std;

int CustomEditControl::getLineCount() {
	int line_count = 0;
	for (UINT i = 0; i < this->text.size(); i++) {
		if (this->text[i] == '\n') {
			line_count++;
		}
	}
	return line_count;
}

void CustomEditControl::setScrollBounds() {
	int line_count = this->getLineCount();

	HWND hwnd = GetDlgItem(hwndParent, (int)this->hMenu);

	HFONT font = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
	LOGFONT lf = {0};
	GetObject(font, sizeof(lf), &lf);
	int line_height = lf.lfHeight;
	if (line_height == 0)
		return;

	RECT rcclient;
	GetClientRect(hwnd, &rcclient);

	int page_size = rcclient.bottom / line_height + 1;

	if (page_size >= line_count) {
		ShowScrollBar(hwnd, SB_VERT, FALSE);
	}
	else {
		ShowScrollBar(hwnd, SB_VERT, TRUE);

		SCROLLINFO vsi;
		vsi.cbSize = sizeof(SCROLLINFO);
		vsi.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &vsi);
		vsi.nMin = 0;
		vsi.nMax = line_count /*- (page_size - 1)*/;
		vsi.nPage = page_size;
		SetScrollInfo(hwnd, SB_VERT, &vsi, TRUE);
	}	

	HDC hDC = GetDC(hwnd);
	SIZE sz;
	int max_line_width = 0;	
	HFONT tempfont = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);
	HFONT oldFont = (HFONT)SelectObject(hDC, tempfont);

	wstring line = L"";
	for (int i = 0; i < this->text.size(); i++) {
		line += this->text[i];

		if (this->text[i] == '\n' || i == this->text.size() - 1) {
			if (this->text[i] == '\n') line.pop_back();

			GetTextExtentPoint32(hDC, line.c_str(), line.size(), &sz);

			if (sz.cx > max_line_width) {
				max_line_width = sz.cx;
			}

			line = L"";
		}	
	}

	DeleteObject(SelectObject(hDC, oldFont));
	ReleaseDC(hwnd, hDC);

	if (rcclient.right >= max_line_width) {
		ShowScrollBar(hwnd, SB_HORZ, FALSE);
	}
	else {
		ShowScrollBar(hwnd, SB_HORZ, TRUE);

		SCROLLINFO hsi;
		hsi.cbSize = sizeof(SCROLLINFO);
		hsi.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ, &hsi);
		hsi.nMin = 0;
		hsi.nMax = max_line_width;
		hsi.nPage = rcclient.right;
		SetScrollInfo(hwnd, SB_HORZ, &hsi, TRUE);
	}	
}

int CustomEditControl::UpdateCaretPos(POINTS pt) {
	int ret = -1;

	HWND hwnd = GetDlgItem(this->hwndParent, (int)this->hMenu);

	SCROLLINFO vsi;
	vsi.cbSize = sizeof(SCROLLINFO);
	vsi.fMask = SIF_ALL;
	GetScrollInfo(hwnd, SB_VERT, &vsi);

	SCROLLINFO hsi;
	hsi.cbSize = sizeof(SCROLLINFO);
	hsi.fMask = SIF_ALL;
	GetScrollInfo(hwnd, SB_HORZ, &hsi);

	HDC hDC = GetDC(hwnd);
	HFONT font = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
	LOGFONT lf = {0};
	GetObject(font, sizeof(lf), &lf);
	HFONT tempfont = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);
	HFONT oldFont = (HFONT)SelectObject(hDC, tempfont);

	int clicked_line_number = (int)((double)((vsi.nPos * lf.lfHeight) + pt.y) / (double)lf.lfHeight);
	int xpos = hsi.nPos + pt.x;

	wstring line = L"";
	int line_count = 0;
	for (int i = 0; i < this->text.size(); i++) {
		line += this->text[i];

		if (this->text[i] == '\n' || i == this->text.size() - 1) {
			if (this->text[i] == '\n') {
				if (line_count == clicked_line_number) {
					ret = i - 1;
					break;
				}

				line.pop_back();
				line_count++;
			}
			line = L"";
		}

		if (line_count == clicked_line_number) {
			SIZE sz;
			GetTextExtentPoint32(hDC, line.c_str(), line.size(), &sz);

			SIZE last_char = {0, 0};
			if (line.size() > 0) {
				GetTextExtentPoint32(hDC, line.c_str(), line.size() - 1, &last_char);
				last_char.cx = sz.cx - last_char.cx;
			}

			if (sz.cx - ((last_char.cx / 2) + 1) >= xpos) {
				ret = i;
				break;
			}
		}
	}

	DeleteObject(SelectObject(hDC, oldFont));
	ReleaseDC(hwnd, hDC);

	this->caret_pos = ret;

	return ret;
}

vector<wstring> g_font_names;
int g_font_index = 0;

int CALLBACK enumFontProc(ENUMLOGFONTW *lpelf, NEWTEXTMETRICW *lpntm, DWORD FontType, LPARAM lParam) {
	g_font_names.push_back(lpelf->elfFullName);

	return TRUE;
}

LRESULT CALLBACK customeditcontrolProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CustomEditControl* a = (CustomEditControl*)GetWindowLongPtr(hwnd, 0);

	static HCURSOR clientCursor = LoadCursor(NULL, IDC_IBEAM);

	switch (message) {
		case WM_NCCREATE: {
			CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);

			a = new CustomEditControl();// (CustomEditControl*)malloc(sizeof(CustomEditControl));
			if (a == NULL)
				return FALSE;

			a->cx = pCreate->cx;
			a->cy = pCreate->cy;
			a->dwExStyle = pCreate->dwExStyle;
			a->hInstance = pCreate->hInstance;
			a->hMenu = pCreate->hMenu;
			a->hwndParent = pCreate->hwndParent;
			a->lpCreateParams = pCreate->lpCreateParams;
			a->lpszClass = pCreate->lpszClass;
			a->lpszName = pCreate->lpszName;
			a->style = pCreate->style;
			a->x = pCreate->x;
			a->y = pCreate->y;

			size_t sz;
			if (pCreate->lpszName != NULL)
				StringCchLength(pCreate->lpszName, STRSAFE_MAX_CCH, &sz);
			else
				sz = 0;

			if (a->text.size()) {
				a->text.clear();
				a->text_colors.clear();
			}

			a->text.reserve(sz);
			a->text_colors.reserve(sz);
			for (int i = 0; i < sz; i++) {
				a->text += a->lpszName[i]; 
				a->text_colors.push_back(RGB(0, 0, 0));
			}

			a->setScrollBounds();

			a->caret_pos = a->sel_s = a->sel_e = -1;

			SetWindowLongPtr(hwnd, 0, (LONG_PTR)a);

			HDC hDC = GetDC(hwnd);
			g_font_names.clear();
			EnumFontFamilies(hDC, NULL, (FONTENUMPROC)enumFontProc, NULL);
			ReleaseDC(hwnd, hDC);

			return TRUE;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC temphDC = BeginPaint(hwnd, &ps);

			HDC hDC = CreateCompatibleDC(temphDC);
			RECT rcclient;
			GetClientRect(hwnd, &rcclient);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(temphDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDC, hbmScreen);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			///////////////////////////////////////////////////////

			//paint background
			HBRUSH hb_background = CreateSolidBrush(RGB(255, 255, 255));
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			//select font
			SelectObject(hDC, a->font);

			//draw text
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			int line_height = (BYTE)tm.tmHeight;
			int line_count = 0, lp = 0;

			SCROLLINFO vsi;
			vsi.cbSize = sizeof(SCROLLINFO);
			vsi.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_VERT, &vsi);
			
			SCROLLINFO hsi;
			hsi.cbSize = sizeof(SCROLLINFO);
			hsi.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_HORZ, &hsi);

			POINT caret_pt = {-1, -1};

			if (a->is_color) {
				///single character
				for (UINT i = 0; i < a->text.size(); i++) {
					if (a->text[i] == '\n') {
						line_count++;
						lp = 0;
						continue;
					}

					if (line_count < vsi.nPos)
						continue;

					if (line_count > (vsi.nPos + vsi.nPage))
						break;

					SIZE size;
					wstring wtxt(L"");
					wtxt += (a->text[i]);
					GetTextExtentPoint32(hDC, wtxt.c_str(), wtxt.length(), &size);

					RECT tr = {
						2 + lp,
						line_count * line_height + 2,
						lp + size.cx + 2,
						(line_count + 1) * line_height + 2
					};

					RECT ttr = tr;
					ttr.top -= (vsi.nPos * line_height);
					ttr.bottom -= (vsi.nPos * line_height);
					ttr.right -= hsi.nPos;
					ttr.left -= hsi.nPos;

					if ((a->sel_s >= 0 && a->sel_e >= 0)
						&& (((a->sel_s < a->sel_e) && (i >= a->sel_s && i < a->sel_e))
						|| ((a->sel_s >= a->sel_e) && i >= a->sel_e && i < a->sel_s))) {
						SetBkColor(hDC, RGB(0, 255, 0));
					}
					else {
						SetBkColor(hDC, RGB(255, 255, 255));
					}

					RECT dummy;
					if (IntersectRect(&dummy, &rcclient, &ttr)) {
						SetTextColor(hDC, a->text_colors[i]);

						int xpos = lp + 2 - hsi.nPos;
						int ypos = line_count * line_height + 2 - (vsi.nPos * line_height);

						ExtTextOut(hDC, xpos, ypos, ETO_OPAQUE, &tr, wtxt.c_str(), wtxt.size(), NULL);

						if (i == a->caret_pos) {
							SIZE size = {0, 0};
							caret_pt.x = xpos + size.cx;
							caret_pt.y = ypos;
						}
					}

					lp += size.cx;
				}
			}
			else {
				///print whole lines
				wstring line = L"";
				for (UINT i = 0; i < a->text.size(); i++) {
					if (a->text[i] == '\n') {
						line_count++;

						SIZE size;
						GetTextExtentPoint32(hDC, line.c_str(), line.length(), &size);

						RECT tr = {
							2,
							line_count * line_height + 2,
							size.cx + 2,
							(line_count + 1) * line_height + 2
						};

						RECT ttr = tr;
						ttr.top -= (vsi.nPos * line_height);
						ttr.bottom -= (vsi.nPos * line_height);
						ttr.right -= hsi.nPos;
						ttr.left -= hsi.nPos;

						if ((a->sel_s >= 0 && a->sel_e >= 0)
							&& (((a->sel_s < a->sel_e) && (i >= a->sel_s && i < a->sel_e))
							|| ((a->sel_s >= a->sel_e) && i >= a->sel_e && i < a->sel_s))) {
							SetBkColor(hDC, RGB(0, 255, 0));
						}
						else {
							SetBkColor(hDC, RGB(255, 255, 255));
						}

						RECT dummy;
						if (IntersectRect(&dummy, &rcclient, &ttr)) {
							SetTextColor(hDC, a->text_colors[i]);

							int xpos = 2 - hsi.nPos;
							int ypos = line_count * line_height + 2 - (vsi.nPos * line_height);

							ExtTextOut(hDC, xpos, ypos, ETO_OPAQUE, &tr, line.c_str(), line.size(), NULL);

							if (i == a->caret_pos) {
								SIZE size = {0, 0};
								caret_pt.x = xpos + size.cx;
								caret_pt.y = ypos;
							}
						}

						line = L"";

						continue;
					}
					else
						line += a->text[i];

					if (line_count < vsi.nPos)
						continue;

					if (line_count >(vsi.nPos + vsi.nPage))
						break;
				}
			}

			if (caret_pt.y > -1) {
				HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
				HPEN oldpen = (HPEN)SelectObject(hDC, pen);
				MoveToEx(hDC, caret_pt.x, caret_pt.y, NULL);
				LineTo(hDC, caret_pt.x, caret_pt.y + line_height);
				DeleteObject(SelectObject(hDC, oldpen));
			}

			/////////////////////////////////////////////////////////////////////
			
			//bit block transfer
			BitBlt(temphDC, 0, 0, rcclient.right, rcclient.bottom, hDC, 0, 0, SRCCOPY);

			//clean up			
			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);
			SelectObject(hDC, hbmOldBitmap);

			DeleteObject(hbmScreen);
			DeleteDC(hDC);

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		/*case WM_KEYDOWN: {
			switch (wParam) {
				case 'A': {
					g_font_index = g_font_index + 1 >= g_font_names.size() ? g_font_names.size() : g_font_index + 1;

					cout << g_font_index << " ";
					wcout << g_font_names[g_font_index] << '\n';

					HFONT font = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, g_font_names[g_font_index].c_str());
					SetWindowFont(hwnd, font, false);

					RECT rcclient;
					GetClientRect(hwnd, &rcclient);
					InvalidateRect(hwnd, &rcclient, FALSE);

					return FALSE;
				}
				case 'S': {
					g_font_index = g_font_index - 1 < 0 ? 0 : g_font_index - 1;

					cout << g_font_index << " ";
					wcout << g_font_names[g_font_index] << '\n';

					HFONT font = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, g_font_names[g_font_index].c_str());
					SetWindowFont(hwnd, font, false);

					RECT rcclient;
					GetClientRect(hwnd, &rcclient);
					InvalidateRect(hwnd, &rcclient, FALSE);

					return FALSE;
				}
			}
			break;
		}*/


		case WM_KEYDOWN:{
			bool wrongkey_skip = false;
			if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') {
				a->sel_s = 0;
				a->sel_e = a->text.size();
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'C') {
				int start = min(a->sel_s, a->sel_e);
				int end = max(a->sel_s, a->sel_e);

				wstring line = L"";
				line.reserve(end - start);
				for (int i = start; i < end; i++) {
					if (a->text[i] == '\n')
						line += '\r';
					line += a->text[i];					
				}

				OpenClipboard(NULL);
				EmptyClipboard();
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, 2 * (line.size() + 1));
				wcscpy_s((wchar_t*)hGlob, line.size() + 1, line.c_str());
				GlobalUnlock(hGlob);
				SetClipboardData(CF_UNICODETEXT, hGlob);
				CloseClipboard();

				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'V') {
				OpenClipboard(NULL);
				HANDLE hData = GetClipboardData(CF_UNICODETEXT);
				wchar_t * pszText = static_cast<wchar_t*>(GlobalLock(hData));
				std::wstring text(pszText);

				remove(text.begin(), text.end(), '\r');

				a->text.insert(a->caret_pos, text);
				GlobalUnlock(hData);
				CloseClipboard();

				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'X') {
				int start = min(a->sel_s, a->sel_e);
				int end = max(a->sel_s, a->sel_e);
				
				wstring line = L"";
				line.reserve(end - start);
				for (int i = start; i < end; i++) {
					if (a->text[i] == '\n')
						line += '\r';
					line += a->text[i];
				}

				a->text.erase(start, end);
				a->sel_s = a->sel_e = a->caret_pos = start;				

				OpenClipboard(NULL);
				EmptyClipboard();
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, 2 * (line.size() + 1));
				wcscpy_s((wchar_t*)hGlob, line.size() + 1, line.c_str());
				GlobalUnlock(hGlob);
				SetClipboardData(CF_UNICODETEXT, hGlob);
				CloseClipboard();

				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == VK_RIGHT) {
				UINT save_start = a->sel_s;

				auto is_whitespace = [](wchar_t c) -> bool { return (c == ' ' || c == '\t' || c == '\r'); };

				for (UINT i = a->caret_pos; i < a->text.size() - 1; i++) {
					if (is_whitespace(a->text[i]) && !is_whitespace(a->text[i + 1])) {
						a->sel_s = a->sel_e = a->caret_pos = i;
						break;
					}
				}

				if (GetKeyState(VK_SHIFT) & 0x8000)
					a->sel_s = save_start;
				goto skip;
			}
			else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == VK_LEFT) {
				UINT save_start = a->sel_s;

				auto is_whitespace = [](wchar_t c) -> bool { return (c == ' ' || c == '\t' || c == '\r'); };

				for (UINT i = a->caret_pos; i > 1; i--) {
					if (is_whitespace(a->text[i]) && !is_whitespace(a->text[i - 1])) {
						a->sel_s = a->sel_e = a->caret_pos = i;
						break;
					}
				}

				if (GetKeyState(VK_SHIFT) & 0x8000)
					a->sel_s = save_start;
				goto skip;
			}
			else if (GetKeyState(VK_SHIFT) & 0x8000 && wParam == VK_UP) {
				/*a->caret_pos.y--;
				if (a->caret_pos.y < 0)
				a->caret_pos.y = 0;
				if (a->caret_pos.x > (int)text[a->caret_pos.y].size())
				a->caret_pos.x = text[a->caret_pos.y].size();
				a->sel_e = a->caret_pos;*/
				goto skip;
			}
			else if (GetKeyState(VK_SHIFT) & 0x8000 && wParam == VK_DOWN) {
				/*a->caret_pos.y++;
				if (a->caret_pos.y >(int)text.size() - 1)
				a->caret_pos.y = text.size() - 1;
				if (a->caret_pos.x > (int)text[a->caret_pos.y].size())
				a->caret_pos.x = text[a->caret_pos.y].size();
				a->sel_e = a->caret_pos;*/
				goto skip;
			}

			switch (wParam) {
				case VK_LEFT:{
					a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
					if (a->text[a->caret_pos] == '\n')
						a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						a->sel_s = a->caret_pos;
					a->sel_e = a->caret_pos;
					break;
				}
				case VK_DOWN:{
					/*a->caret_pos.y++;
					if (a->caret_pos.y >(int)text.size() - 1)
					a->caret_pos.y = text.size() - 1;
					a->sel_s = a->caret_pos;
					a->sel_e = a->caret_pos;*/
					/*if (a->flags & EDC_HSCROLL) {
						int t = a->scroll_xpos;
						t -= a->line_height;
						a->scroll_xpos = t < 0 ? 0 : t;
					}*/
					break;
				}
				case VK_RIGHT:{
					a->caret_pos++;
					if (a->caret_pos < a->text.size() && a->text[a->caret_pos] == '\n')
						a->caret_pos++;
					if (a->caret_pos >(int)a->text.size())
						a->caret_pos = a->text.size();
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						a->sel_s = a->caret_pos;
					a->sel_e = a->caret_pos;
					break;
				}
				case VK_UP:{
					/*a->caret_pos.y--;
					if (a->caret_pos.y < 0)
					a->caret_pos.y = 0;
					a->sel_s = a->caret_pos;
					a->sel_e = a->caret_pos;*/
					/*if (a->flags & EDC_HSCROLL) {
						a->scroll_xpos += a->line_height;
					}*/
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
					if (!(a->style & ES_READONLY))
						a->text.insert(a->text.begin() + a->caret_pos, '\n');
					break;
				}
				case VK_TAB:{
					if (!(a->style & ES_READONLY))
						a->text.insert(a->text.begin() + a->caret_pos, '\t');
					break;
				}
				case VK_DELETE:{
					if (!(a->style & ES_READONLY)) {
						if (a->sel_s == a->sel_e)
							a->sel_s = a->sel_s == a->text.size() ? a->text.size() : a->sel_s + 1;
						else
							a->caret_pos = a->sel_s > a->sel_e ? a->sel_e : a->sel_s;
						if (a->text[a->caret_pos] == '\r') {
							a->sel_s = a->sel_s == a->text.size() ? a->text.size() : a->sel_s + 1;
							a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
						}

						//a->EditText("");
					}
					break;
				}
				case VK_BACK:{
					if (!(a->style & ES_READONLY)) {
						if (a->sel_s == a->sel_e)//if not highlighted
							a->sel_s = a->sel_s == 0 ? 0 : a->sel_s - 1;//move highlight left one
						else
							a->caret_pos = a->sel_s > a->sel_e ? a->sel_e + 1 : a->sel_s + 1;//else (is highlighted) put cursor to beginning

						if (a->text[a->sel_s] == '\n') {//if newline is found move highlight left again, ajdust caret as well
							a->sel_s = a->sel_s == 0 ? 0 : a->sel_s - 1;
							a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;
						}

					//	a->EditText("");//removes all highlighted text
						a->sel_s = a->sel_e = a->caret_pos = a->caret_pos == 0 ? 0 : a->caret_pos - 1;//remove highlighting and move caret left
					}
					break;
				}
				default:
					wrongkey_skip = true;
					break;
			}
skip:
			/*for (UINT i = 0; i < a->text.size(); i++) {
			if (i == a->caret_pos)
			cout << "|";
			if (a->text[i] == '\r')
			cout << "\\r";
			else if (a->text[i] == '\n')
			cout << "\\n" << '\n';
			else if (a->text[i] == '\t')
			cout << "\\t";
			else
			cout << a->text[i];
			}
			cout << "\n/////////////////////////////////\n";*/

			if (!wrongkey_skip)
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			break;
		}

		case WM_LBUTTONDOWN: {
			SetCapture(hwnd);

			POINTS pt = MAKEPOINTS(lParam);

			a->is_dragging = true;

			a->UpdateCaretPos(pt);

			if (!(GetKeyState(VK_SHIFT) & 0x8000))
				a->sel_s = a->caret_pos;
			a->sel_e = a->caret_pos;

			RECT client;
			GetClientRect(hwnd, &client);
			InvalidateRect(hwnd, &client, false);

			break;
		}
		case WM_LBUTTONUP: {
			ReleaseCapture();

			a->is_dragging = false;

			break;
		}
		case WM_RBUTTONUP: {
			a->sel_s = -1;
			a->sel_e = -1;
			a->caret_pos = -1;

			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEMOVE:{
			POINTS pt = MAKEPOINTS(lParam);

			/*if (!a->mouse_in_client) {
				SendMessage(hwnd, WM_MOUSEENTER, NULL, NULL);
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_HOVER | TME_LEAVE | TME_NONCLIENT;
				me.hwndTrack = hwnd;
				me.dwHoverTime = HOVER_DEFAULT;
				TrackMouseEvent(&me);
			}*/

			if (a->is_dragging) {
				a->UpdateCaretPos(pt);
				a->sel_e = a->caret_pos;

				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_MOUSELEAVE:{
			a->mouse_in_client = false;
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_MOUSEENTER:{
			a->mouse_in_client = true;
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETCURSOR:{
			switch (LOWORD(lParam)) {
				case HTCLIENT:
					SetCursor(clientCursor);
					return TRUE;
			}
			break;
		}
		case WM_SETTEXT:{
			TCHAR* lpszText = (TCHAR*)lParam;
			size_t sz;

			if (lpszText != NULL)
				StringCchLength(lpszText, STRSAFE_MAX_CCH, &sz);
			else
				sz = 0;

			if (a->text.size()) {
				a->text.clear();
				a->text_colors.clear();
			}

			a->text.reserve(sz);
			a->text_colors.reserve(sz);
			for (int i = 0; i < sz; i++) {
				a->text += lpszText[i];
				a->text_colors.push_back(RGB(0, 0, 0));
			}

			a->setScrollBounds();

			a->sel_s = -1;
			a->sel_e = -1;
			a->caret_pos = -1;

			RECT client;
			GetClientRect(hwnd, &client);
			InvalidateRect(hwnd, &client, false);

			return TRUE;
		}
		case WM_GETTEXT:{
			size_t sz = (size_t)wParam;
			TCHAR* lpBuffer = (TCHAR*)lParam;

			size_t max = sz > a->text.size() ? a->text.size() : sz < a->text.size() ? sz - 1 : sz - 1;
			for (int i = 0; i < max; i++) {
				lpBuffer[i] = a->text[i];
			}
			lpBuffer[sz - 1] = '\0';

			return max;
		}
		case WM_GETTEXTLENGTH: {
			return a->text.size();
		}
		case WM_GETFONT: {
			return (LRESULT)a->font;

			break;
		}
		case WM_SETFONT: {
			a->font = (HFONT)wParam;

			/*wstring txt = L"";

			static bool block = false;
			if (!block) {
				block = true;
				vector<FontImage> glyphs = GetWindowFontGlyphsAsArray(hwnd);

				for (int i = 0; i < glyphs.size(); i++) {
					for (int j = 0; j < glyphs.size(); j++) {
						if (glyphs[j].c == '\0' || glyphs[j].c == 0)
							continue;
						txt += glyphs[j].c;
					}
					txt += '\n';
				}

				for (int i = 0; i < glyphs.size(); i++) {
					delete[] glyphs[i].buf;
				}

				SetWindowText(hwnd, txt.c_str());
				block = false;
			}			
*/
			if (LOWORD(lParam) == TRUE) {
				RECT rcclient;
				GetClientRect(hwnd, &rcclient);
				InvalidateRect(hwnd, &rcclient, FALSE);
			}

			break;
		}
		case EM_SETSEL: {
			a->sel_s = wParam;
			a->sel_e = lParam;

			break;
		}
		case EM_EXSETSEL: {
			CHARRANGE cr = *((CHARRANGE*)lParam);
		
			a->sel_s = cr.cpMin;
			a->sel_e = cr.cpMax;

			break;
		}
		case EM_SETCHARFORMAT:{
			CHARFORMAT2* p = (CHARFORMAT2*)lParam;
		
			if ((wParam & SCF_SELECTION) > 0) {
				for (int i = a->sel_s; i < a->sel_e; i++) {
					if ((p->dwMask & CFM_COLOR) > 0) {
						a->text_colors[i] = p->crTextColor;
					}
				}
			}

			return 0;
		}
		case WM_SYSCOMMAND: {
			switch (wParam) {
				case SC_MAXIMIZE: {
					a->setScrollBounds();

					RECT rcclient;
					GetClientRect(hwnd, &rcclient);
					InvalidateRect(hwnd, &rcclient, FALSE);

					break;
				}
			}
			break;
		}
		case WM_WINDOWPOSCHANGED: {
			a->setScrollBounds();

			RECT rcclient;
			GetClientRect(hwnd, &rcclient);
			InvalidateRect(hwnd, &rcclient, FALSE);

			break;
		}
		case WM_MOUSEWHEEL:{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (GetKeyState(VK_CONTROL) & 0x8000) {
					HFONT font = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
					LOGFONT lf = {0};
					GetObject(font, sizeof(lf), &lf);
					lf.lfHeight++;
					DeleteObject(font);
					SetWindowFont(hwnd, CreateFontIndirect(&lf), false);
					a->setScrollBounds();
				}
				else {
					SCROLLINFO vsi;
					vsi.cbSize = sizeof(SCROLLINFO);
					vsi.fMask = SIF_ALL;
					GetScrollInfo(hwnd, SB_VERT, &vsi);
					vsi.nPos = vsi.nPos - a->scroll_speed <= 0 ? 0 : vsi.nPos - a->scroll_speed;
					SetScrollInfo(hwnd, SB_VERT, &vsi, TRUE);

					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				}
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				if (GetKeyState(VK_CONTROL) & 0x8000) {
					HFONT font = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
					LOGFONT lf = {0};
					GetObject(font, sizeof(lf), &lf);					
					lf.lfHeight = lf.lfHeight < 2 ? 1 : lf.lfHeight - 1;
					DeleteObject(font);
					SetWindowFont(hwnd, CreateFontIndirect(&lf), false);
					a->setScrollBounds();
				}
				else {
					SCROLLINFO vsi;
					vsi.cbSize = sizeof(SCROLLINFO);
					vsi.fMask = SIF_ALL;
					GetScrollInfo(hwnd, SB_VERT, &vsi);
					vsi.nPos = vsi.nPos + a->scroll_speed >= vsi.nMax ? vsi.nMax : vsi.nPos + a->scroll_speed;
					SetScrollInfo(hwnd, SB_VERT, &vsi, TRUE);

					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				}
			}
			else
				break;

			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case WM_VSCROLL: {
			switch (LOWORD(wParam)) {
				case SB_THUMBTRACK: {
					int pos = HIWORD(wParam);

					SCROLLINFO vsi;
					vsi.cbSize = sizeof(SCROLLINFO);
					vsi.fMask = SIF_ALL;
					GetScrollInfo(hwnd, SB_VERT, &vsi);
					vsi.nPos = pos;
					SetScrollInfo(hwnd, SB_VERT, &vsi, TRUE);

					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

					break;
				}
				case SB_LINEDOWN: {

					break;
				}
				case SB_LINEUP: {

					break;
				}
			}
			
			break;
		}
		case WM_HSCROLL: {
			switch (LOWORD(wParam)) {
				case SB_THUMBTRACK: {
					int pos = HIWORD(wParam);

					SCROLLINFO hsi;
					hsi.cbSize = sizeof(SCROLLINFO);
					hsi.fMask = SIF_ALL;
					GetScrollInfo(hwnd, SB_HORZ, &hsi);
					hsi.nPos = pos;
					SetScrollInfo(hwnd, SB_HORZ, &hsi, TRUE);

					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

					break;
				}
				case SB_LINELEFT: {

					break;
				}
				case SB_LINERIGHT: {

					break;
				}
			}

			break;
		}
		/*case SBM_SETPOS: {

			break;
		}
		case SBM_SETRANGE: {
			
			break;
		}
		case SBM_SETSCROLLINFO: {

			break;
		}*/
		case WM_NCDESTROY: {
			if (a != NULL) {
				free(a);
			}
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

ATOM init_register_custom_EditControl() {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.lpszClassName = custom_edit_control_classname;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = customeditcontrolProc;
	wnd.cbWndExtra = sizeof(CustomEditControl*);
	return RegisterClassEx(&wnd);
}
ATOM edc = init_register_custom_EditControl();

