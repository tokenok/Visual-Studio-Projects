#pragma once

#include <Windows.h>
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
#include <Windowsx.h>
#include <string>
#include <vector>
#include <functional>

#include "d2data.h"

#define WM_SELECINVTITEM (WM_USER)

Gdiplus::Image* LoadPicture(LPCWSTR lpszFileName);
BOOL DrawPicture(HDC hDC, Gdiplus::Image* pImage, LPRECT lpRect);
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
void TransparentBlitToHdc(HDC hdcDst, HBITMAP hbmSrc, int x, int y, int w, int h, COLORREF transparent, void(*transfunc)(BYTE*, BYTE*, BYTE*) = NULL);
std::string GetStorePNG(std::string itemstore, const InvData* data);
std::vector<ItemData>* get_item_store(std::string realm, std::string account, std::string character, std::string store);
POINT get_item_inv_loc_from_pt(std::string store, POINT pt, const InvData& invdata);
ItemData* get_item_from_loc(std::string realm, std::string account, std::string character, std::string store, POINT pt);
RECT get_item_inv_rect(const ItemData* item);
class inv_active_bmp_files;
BOOL CALLBACK invProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

