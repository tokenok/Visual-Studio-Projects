#include "font_image.h"
#include "C:/CPPlibs/common/common.h"

#include <windowsx.h>

using namespace std;

vector<FontImage> GetWindowFontGlyphsAsArray(HWND hwnd) {
	vector<FontImage> glyphs;

	GLYPHSET* gs;
	HDC hDC = GetDC(hwnd);
	HDC memDC = CreateCompatibleDC(hDC);
	DWORD GlyphSize = GetFontUnicodeRanges(memDC, NULL);
	gs = (GLYPHSET*)new unsigned char[GlyphSize];
	GetFontUnicodeRanges(memDC, gs);
	HFONT font = GetWindowFont(hwnd);
	SetWindowFont(hwnd, font, TRUE);
	SelectObject(memDC, font);

	wstring ws;
	for (int i = 0; i < gs->cRanges; i++) {
		for (int j = 0; j < gs->ranges[i].cGlyphs; j++) {
			ws.push_back(gs->ranges[i].wcLow + j);
			ws.push_back('\n');

			SIZE size;
			wstring wtxt(ws.end() - 2, ws.end() - 1);
			GetTextExtentPoint32(memDC, wtxt.c_str(), wtxt.length(), &size);

			HBITMAP cbmp = CreateCompatibleBitmap(hDC, size.cx, size.cy);
			HBITMAP hbmOldBitmap = (HBITMAP)SelectObject(memDC, cbmp);

			RECT tr = {
				2,
				2,
				size.cx + 2,
				size.cy + 2
			};

			SetBkColor(memDC, RGB(255, 255, 255));
			SetTextColor(memDC, RGB(0, 0, 0));
			ExtTextOut(memDC, 0, 0, /*ETO_CLIPPED |*/ ETO_OPAQUE, &tr, wtxt.c_str(), wtxt.size(), NULL);

			BYTE* img;
			int w, h;
			img = getDCBitmapArray(memDC, cbmp, &w, &h);

			//array_to_bmp(uint_to_str((UINT)wtxt[0]) + ".bmp", img, w, h);

			if (w && h) {
				FontImage im;
				im.buf = img;
				im.w = w;
				im.h = h;
				im.c = wtxt[0];
				glyphs.push_back(im);
			}

			DeleteObject(cbmp);

			SelectObject(hDC, hbmOldBitmap);
		}
	}

	ReleaseDC(hwnd, hDC);
	DeleteDC(memDC);

	return glyphs;
}

