#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
#include <conio.h>

#include <atlbase.h>
#include <mmdeviceapi.h>
#include <devicetopology.h>

#include "C:\CPPlibs\common\f\common.h"

using namespace std;
using namespace Gdiplus;

HWND g;
HWND g_songinfo;

HFONT font = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");

VOID CALLBACK TimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);

ULONG_PTR gdiplusToken;
map<string, HBITMAP> btnImageFiles;

TCHAR className[] = TEXT("testasdfasdfasd");

enum {
	TIMER_HIDE_SONGINFO = 1,
	TIMER_DESKBAND_RAINBOW_ROTATE,
	TIMER_ARTIST_ROTATE,
	TIMER_SONG_ROTATE,
	TIMER_SONGINFO_ROTATE_DELAY,
	
};

struct COLOR {
	BYTE R, G, B;
	COLOR(BYTE r, BYTE g, BYTE b) { R = r; G = g; B = b; }
	COLOR(COLORREF c) {
		R = GetRValue(c);
		G = GetGValue(c);
		B = GetBValue(c);
	}
	static COLOR FromArgb(BYTE r, BYTE g, BYTE b) { return COLOR(r, g, b); }
	static COLORREF rgb(BYTE r, BYTE g, BYTE b) {
		return RGB(r, g, b);
	}
	static COLORREF rgb(const COLOR &c) {
		return RGB(c.R, c.G, c.B);
	}
};
deque<COLOR> getGradient(deque<COLOR> l, int n) {
	deque<COLOR> ret;

	for (int i = 1; i < l.size(); i++) {
		ret.push_back(l[i - 1]);
		for (double j = n; j > 0; j--) {
			byte r = (BYTE)(l[i - 1].R * (j / (n + 1)) + l[i].R * (1 - (j / (n + 1))));
			byte g = (BYTE)(l[i - 1].G * (j / (n + 1)) + l[i].G * (1 - (j / (n + 1))));
			byte b = (BYTE)(l[i - 1].B * (j / (n + 1)) + l[i].B * (1 - (j / (n + 1))));
			ret.push_back(COLOR::FromArgb(r, g, b));
		}
	}
	ret.push_back(l[l.size() - 1]);

	for (double j = n; j > 0; j--) {
		byte r = BYTE(l[l.size() - 1].R * (j / (n + 1)) + l[0].R * (1 - (j / (n + 1))));
		byte g = BYTE(l[l.size() - 1].G * (j / (n + 1)) + l[0].G * (1 - (j / (n + 1))));
		byte b = BYTE(l[l.size() - 1].B * (j / (n + 1)) + l[0].B * (1 - (j / (n + 1))));
		ret.push_back(COLOR::FromArgb(r, g, b));
	}

	return ret;
}
deque<COLOR> g_colors = {
	{255, 0, 0},
	{255, 150, 0},
	{255, 255, 0},
	{105, 255, 0},
	{0, 255, 0},
	{0, 255, 150},
	{0, 255, 255},
	{0, 105, 255},
	{0, 0, 255},
	{150, 0, 255},
	{255, 0, 255},
	{255, 0, 105},
};

class CCoInitialize {
	private:
	HRESULT m_hr;
	public:
	CCoInitialize(PVOID pReserved, HRESULT &hr)
		: m_hr(E_UNEXPECTED) {
		hr = m_hr = CoInitialize(pReserved);
	}
	~CCoInitialize() { if (SUCCEEDED(m_hr)) { cout << "CoUninitialize()\n"; CoUninitialize(); } }
};

void getHeadphoneJackState() {
	
	HRESULT hr = S_OK;
	CCoInitialize ci(NULL, hr);
	CComPtr<IMMDeviceEnumerator> de;
	hr = de.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (!FAILED(hr)) {
		if (de) {
			CComPtr<IMMDeviceCollection> devc;
			de->EnumAudioEndpoints(eCapture, DEVICE_STATEMASK_ALL, &devc);

			UINT count;
			devc->GetCount(&count);
			
			for (int i = 0; i < count; i++) {
				CComPtr<IMMDevice> dev;
				devc->Item(i, &dev);
				LPWSTR id;
				dev->GetId(&id);
				DWORD state;
				dev->GetState(&state);
				string guid = wstr_to_str(id);
				if (guid == "{0.0.1.00000000}.{3e06c837-231b-4326-a308-766dc4e53dbb}") {
					if (state == 8)
						cout << "pause\n";
					else cout << "play\n";
					cout << wstr_to_str(id) << " " << state << '\n';
				}
			}

		}
	}
	else
		printf("CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
}

class SongInfo {
	public:
	string artist, song;
	SIZE szArtist, szSong;
	RECT rcArtist, rcSong;
	RECT rcArtist2, rcSong2;
	bool brotateArtist, brotateSong;
	bool bArtistPart2, bSongPart2;

	void initArtist(RECT rcclient, SIZE sz) {
		szArtist = sz;
		brotateArtist = rcclient.right - szArtist.cx - 10 <= 0;
		rcArtist.left = !brotateArtist ? (rcclient.right - szArtist.cx) / 2 : 0;
		rcArtist.right = rcArtist.left + szArtist.cx;
		rcArtist.top = 5;
		rcArtist.bottom = rcArtist.top + szArtist.cy;
		bArtistPart2 = false;
		rcArtist2 = {0,0,0,0};
	}

	void initSong(RECT rcclient, SIZE sz) {
		szSong = sz;
		brotateSong = rcclient.right - szSong.cx - 10 <= 0;
		rcSong.left = !brotateSong ? (rcclient.right - szSong.cx) / 2 : 0;
		rcSong.right = rcSong.left + szSong.cx;
		rcSong.top = 24;
		rcSong.bottom = rcSong.top + szSong.cy;
		bSongPart2 = false;
		rcSong2 = {0,0,0,0};
	}

	void rotateArtist(int speed, int distance, RECT rcclient) {
		rcArtist.left -= speed;
		rcArtist.right -= speed;

		if (rcArtist.right + distance < rcclient.right) {
			bArtistPart2 = true;
			rcArtist2.left = rcArtist.right + distance;
			rcArtist2.right = rcArtist.right + distance + rcArtist2.left + rcclient.right;
			rcArtist2.top = rcArtist.top;
			rcArtist2.bottom = rcArtist.bottom;
		}

		if (bArtistPart2 && rcArtist2.left <= 0) {
			initArtist(rcclient, szArtist);
//			brotateArtist = false;
		}
	}

	void rotateSong(int speed, int distance, RECT rcclient) {
		rcSong.left -= speed;
		rcSong.right -= speed;

		if (rcSong.right + distance < rcclient.right) {
			bSongPart2 = true;
			rcSong2.left = rcSong.right + distance;
			rcSong2.right = rcSong.right + distance + rcSong2.left + rcclient.right;
			rcSong2.top = rcSong.top;
			rcSong2.bottom = rcSong.bottom;
		}

		if (bSongPart2 && rcSong2.left <= 0) {
			initSong(rcclient, szSong);
	//		brotateSong = false;
		}
	}

	int getArtistWidth() {
		return getRectWidth(rcArtist) + (bArtistPart2 ? getRectWidth(rcArtist) : 0);
	}

	int getSongWidth() {
		return getRectWidth(rcSong) + (bSongPart2 ? getRectWidth(rcSong) : 0);
	}
};

SongInfo songinfo;

LRESULT CALLBACK StaticProc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE: {
			break;
		}
		case WM_NCPAINT:
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rcclient = getclientrect(hwnd);

			HDC hDCmem = CreateCompatibleDC(hdc);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hdc, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);

			//paint background
			HBRUSH backbrush = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDCmem, &rcclient, backbrush);
			
			///////////////////////////////////////////////////

			//paint text	
			auto draw_songinfo_text_back = [](int w, int h, int contrast) {
				HBITMAP bitmap = CreateBitmap(w, h, 1, 32, NULL);

				HDC hdcMem = CreateCompatibleDC(0);

				HBITMAP old = SelectBitmap(hdcMem, bitmap);

				RECT rc = {0, 0, 0, h};
				for (int i = 0; i < w; i++) {
					rc.left = i;
					rc.right = (i + 1);
					COLORREF col = COLOR::rgb(g_colors[i % g_colors.size()]);
					if (contrast)
						applyContrast(&col, contrast);
					HBRUSH brush = CreateSolidBrush(col);
					FillRect(hdcMem, &rc, brush);
					DeleteObject(brush);
				}

				bitmap = SelectBitmap(hdcMem, old);

				DeleteDC(hdcMem);

				return bitmap;
			};

			auto draw_songinfo_text = [](int w, int h, string txt, bool bpart2) {				
				HBITMAP bitmap = CreateBitmap(w, h, 1, 32, NULL);

				HDC hdcMem = CreateCompatibleDC(0);

				SetTextColor(hdcMem, RGB(0, 0, 0));
				SetBkColor(hdcMem, RGB(10, 10, 10));

				DeleteObject(SelectObject(hdcMem, font));

				HBITMAP old = SelectBitmap(hdcMem, bitmap);

				RECT rcinfocorrected = {0, 0, w, h};
				ExtTextOut(hdcMem, 0, 0, ETO_CLIPPED, &rcinfocorrected, STW(txt), txt.size(), NULL);
			
				if (bpart2) {
					RECT space = {w / 2, 0, w / 2 + 10, h};
					HBRUSH brush = CreateSolidBrush(RGB(10, 10, 10));
					FillRect(hdcMem, &space, brush);
					DeleteObject(brush);

					RECT rcinfo2corrected = {0, 0, w + 10, h};
					ExtTextOut(hdcMem, w / 2 + 10, 0, ETO_CLIPPED, &rcinfo2corrected, STW(txt), txt.size(), NULL);
				}

				bitmap = SelectBitmap(hdcMem, old);

				DeleteDC(hdcMem);

				return bitmap;
			};

			int height = getRectHeight(songinfo.rcArtist);

			HBITMAP bmpartist_back = draw_songinfo_text_back(rcclient.right, height, 0);
			HBITMAPBlitToHdc(hDCmem, bmpartist_back, 0, songinfo.rcArtist.top, rcclient.right, height);
			HBITMAP bmpartist = draw_songinfo_text(songinfo.getArtistWidth(), height, songinfo.artist, songinfo.bArtistPart2);
			RECT rclart = {0, songinfo.rcArtist.top, songinfo.rcArtist.left + 5, songinfo.rcArtist.bottom};
			FillRect(hDCmem, &rclart, backbrush);
			if (!songinfo.bArtistPart2) {
				RECT rcrart = {songinfo.rcArtist.right + 5, songinfo.rcArtist.top, rcclient.right, songinfo.rcArtist.bottom};
				FillRect(hDCmem, &rcrart, backbrush);
			}
			TransparentBlitToHdc(hDCmem, bmpartist, songinfo.rcArtist.left + 5, songinfo.rcArtist.top, songinfo.getArtistWidth(), height, RGB(0,0,0));

			HBITMAP bmpsong_back = draw_songinfo_text_back(rcclient.right, height, 80);
			HBITMAPBlitToHdc(hDCmem, bmpsong_back, 0, songinfo.rcSong.top, rcclient.right, height);
			HBITMAP bmpsong = draw_songinfo_text(songinfo.getSongWidth(), height, songinfo.song, songinfo.bSongPart2);
			RECT rclsong = {0, songinfo.rcSong.top, songinfo.rcSong.left + 5, songinfo.rcSong.bottom};
			FillRect(hDCmem, &rclsong, backbrush);
			if (!songinfo.bSongPart2) {
				RECT rcrsong = {songinfo.rcSong.right + 5, songinfo.rcSong.top, rcclient.right, songinfo.rcSong.bottom};
				FillRect(hDCmem, &rcrsong, backbrush);
			}
			TransparentBlitToHdc(hDCmem, bmpsong, songinfo.rcSong.left + 5, songinfo.rcSong.top, songinfo.getSongWidth(), height, RGB(0, 0, 0));

			DeleteObject(bmpsong);
			DeleteObject(bmpartist);			
			DeleteObject(bmpsong_back);
			DeleteObject(bmpartist_back);

			//////////////////////////////////////////////////////////

			BitBlt(hdc, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			//cleanup
			DeleteObject(backbrush);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND: {
			return FALSE;
		}
		case WM_LBUTTONDOWN: {
			ShowWindow(hwnd, SW_HIDE);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void DisplaySongInfo() {
	if (!g_songinfo) {
		WNDCLASSEX wc = {0};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = L"CurrentSongClassName";
		wc.lpfnWndProc = StaticProc2;
		wc.style = NULL;
		RegisterClassEx(&wc);

		g_songinfo = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"CurrentSongClassName", L"", WS_VISIBLE, 0, 0, 200, 46, NULL, NULL, NULL, NULL);
		SetWindowLong(g_songinfo, GWL_STYLE, 0);
	}

	songinfo.artist = "Banana State University";
	songinfo.song = "My dog is on fire you have to put him out omg save my dog";

	RECT clientrc = getclientrect(g_songinfo);
	HDC hdc = GetDC(g_songinfo);
	DeleteObject(SelectObject(hdc, font));
	SIZE sz;
	GetTextExtentPoint32(hdc, STW(songinfo.artist), songinfo.artist.size(), &sz);
	songinfo.initArtist(clientrc, sz);
	GetTextExtentPoint32(hdc, STW(songinfo.song), songinfo.song.size(), &sz);
	songinfo.initSong(clientrc, sz);
	ReleaseDC(g_songinfo, hdc);

	ShowWindow(g_songinfo, SW_SHOW);

	InvalidateRect(g_songinfo, &getwindowrect(g_songinfo), FALSE);	

	//SetTimer(g_songinfo, TIMER_HIDE_SONGINFO, 4000, TimerProc);

	SetTimer(g_songinfo, TIMER_SONGINFO_ROTATE_DELAY, 1000, TimerProc);
}

VOID CALLBACK TimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time) {
	switch (id) {
		case TIMER_HIDE_SONGINFO: {
			if (g_songinfo) {
				ShowWindow(g_songinfo, SW_HIDE);
			}
			KillTimer(hwnd, TIMER_HIDE_SONGINFO);
			KillTimer(hwnd, TIMER_ARTIST_ROTATE);
			KillTimer(hwnd, TIMER_SONG_ROTATE);
			KillTimer(hwnd, TIMER_SONGINFO_ROTATE_DELAY);
			break;
		}
		case TIMER_DESKBAND_RAINBOW_ROTATE: {
			vector<int> cc = {43,32,23,16,10,6,4,2,2,2,1,1};

			for (int i = 0; i < 10; i++) {
				COLOR last = g_colors[g_colors.size() - 1];
				g_colors.pop_back();
				g_colors.push_front(last);
			}

			InvalidateRect(hwnd, &getclientrect(hwnd), FALSE);

			if (g_songinfo) 
				InvalidateRect(g_songinfo, &getclientrect(g_songinfo), TRUE);

			break;
		}
		case TIMER_ARTIST_ROTATE: {
			if (songinfo.brotateArtist)
				songinfo.rotateArtist(2, 10, getclientrect(g_songinfo));

			InvalidateRect(hwnd, &getclientrect(hwnd), FALSE);
			break;
		}
		case TIMER_SONG_ROTATE: {
			if (songinfo.brotateSong)
				songinfo.rotateSong(2, 10, getclientrect(g_songinfo));

			InvalidateRect(hwnd, &getclientrect(hwnd), FALSE);
			break;
		}
		case TIMER_SONGINFO_ROTATE_DELAY: {
			if (songinfo.brotateArtist)
				SetTimer(g_songinfo, TIMER_ARTIST_ROTATE, 25, TimerProc);
			if (songinfo.brotateSong)
				SetTimer(g_songinfo, TIMER_SONG_ROTATE, 25, TimerProc);
			break;
		}
	}
}

LRESULT CALLBACK StaticProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE: {
			GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			g_colors = getGradient(g_colors, (getclientrect(hwnd).right * 1) / 12);

			g_colors.clear();
			int len = 200;
			for (int i = 0; i < len; i++) {
				g_colors.push_back(COLOR(getRainbowColor(i, len, 128, 127)));
			}

			SetTimer(hwnd, TIMER_DESKBAND_RAINBOW_ROTATE, 100, TimerProc);

			DisplaySongInfo();

			break;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rcclient = getclientrect(hwnd);

			HDC hDCmem = CreateCompatibleDC(hdc);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hdc, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);

			//paint background (rainbow)
			RECT rc = rcclient;
			for (int i = 0; i < rcclient.right; i++) {
				rc.left = i;
				rc.right = (i + 1);
				HBRUSH brush = CreateSolidBrush(COLOR::rgb(g_colors[i % g_colors.size()]));
				FillRect(hDCmem, &rc, brush);
				DeleteObject(brush);
			}		

			BitBlt(hdc, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_DEVICECHANGE: {
			switch (wParam) {
				case 7: {
					cout << "DBT_DEVNODES_CHANGED " << '\n';

					getHeadphoneJackState();

					break;
				}
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			g_colors.clear();

			static int center = 40;
			static int width = 12;
			width++;
			for (int i = 0; i < 142; i++) {
				g_colors.push_back(COLOR(getRainbowColor(i, 142, center, width)));
			}

			cout << "center: " << center << " width: " << width << '\n';


			DisplaySongInfo();
			break;
		}
		case WM_DESTROY:
		case WM_CLOSE: {
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	SHOW_CONSOLE();

	/*vector<int> n = {1,1,1,2,2,2,3,3,4,5,5,5,6,7,8,9};

	auto get_mode = [](const vector<int>& n) {
		map<int, int> modecounts;
		map<int, vector<int>> modelists;
		for (int i = 0; i < n.size(); i++)
			modecounts[n[i]]++;
		for (auto a : modecounts)
			modelists[a.second].push_back(a.first);
		auto modes = modelists.rbegin();
		pair<int, vector<int>> ret;
		if (modes->second.size() != n.size() && modes->second.size() <= 4) 
			ret = {modes->first, modes->second};
		return ret;
	};

	auto get_median = [](const vector<int>& n) {		
		vector<int> t = n;
		std::sort(t.begin(), t.end());
		if (t.size() % 2 == 0) 
			return (static_cast<double>(t[t.size() / 2 - 1]) + static_cast<double>(t[t.size() / 2])) / 2.0;
		else 
			return (double)t[t.size() / 2 - 1];
	};

	auto get_mean = [](const vector<int>& n) {
		double mean = 0;
		for (int i = 0; i < n.size(); i++) 
			mean += n[i];
		return mean / (double)n.size();
	};

	auto get_range = [](const vector<int>& n) {
		vector<int> t = n;
		sort(t.begin(), t.end());
		return t[t.size() - 1] - t[0];
	};

	auto modes = get_mode(n);
	auto median = get_median(n);
	auto mean = get_mean(n);
	auto range = get_range(n);

	cout << "modes: ";
	for (int i = 0; i < modes.second.size(); i++)
		cout << modes.second[i] << " ";
	cout << '\n';
	cout << "median " << median << '\n';
	cout << "mean " << mean << '\n';
	cout << "range " << range << '\n';

	_getch();*/

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	wc.lpfnWndProc = StaticProc;
	wc.style = NULL;
	RegisterClassEx(&wc);

	HWND g = CreateWindowEx(0, className, L"", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 600, 400, 142 + 16, 23 + 100, NULL, NULL, NULL, NULL);

	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return FALSE;
}