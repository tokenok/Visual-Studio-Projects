#include "font_image.h"

#include <windowsx.h>

using namespace std;

BYTE* getDCBitmapArray1(HDC hDC, HBITMAP hBmp, int *x, int *y) {
	BITMAP bm = { 0 };
	GetObject(hBmp, sizeof(bm), &bm);
	BITMAPINFO* bmi = (BITMAPINFO*)_alloca(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	::ZeroMemory(bmi, sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BOOL bRes = ::GetDIBits(hDC, hBmp, 0, bm.bmHeight, NULL, bmi, DIB_RGB_COLORS);
	if (!bRes || bmi->bmiHeader.biBitCount != 32)
		return NULL;
	LPBYTE pBitData = (LPBYTE) ::LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
	if (pBitData == NULL)
		return NULL;

	BYTE* img = new BYTE[bm.bmWidth * bm.bmHeight * 3];

	::GetDIBits(hDC, hBmp, 0, bm.bmHeight, pBitData, bmi, DIB_RGB_COLORS);
	for (int y = 0; y < bm.bmHeight; y++) {
		for (int x = 0; x < bm.bmWidth; x++) {
			int newpos = y * 3 * bm.bmWidth + (x * 3);
			int bufpos = (bm.bmHeight - y - 1) * (bm.bmWidth * 4) + (x * 4);

			img[newpos + 0] = pBitData[bufpos + 2];
			img[newpos + 1] = pBitData[bufpos + 1];
			img[newpos + 2] = pBitData[bufpos + 0];
		}
	}

	::LocalFree(pBitData);

	*x = bm.bmWidth;
	*y = bm.bmHeight;

	return img;
}

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
			img = getDCBitmapArray1(memDC, cbmp, &w, &h);

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

