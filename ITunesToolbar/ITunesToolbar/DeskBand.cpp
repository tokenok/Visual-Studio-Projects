#include <windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <deque>
#include <map>
#include <thread>
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
#include <Mmdeviceapi.h>

#include "DeskBand.h"

#include "C:\CPPlibs\common\f\common.h"

#include "resource.h"

#include "Custom Trackbar.h"
#include "iTunes.h"

using namespace std;
using namespace Gdiplus;

#define RECTWIDTH(x)   ((x).right - (x).left)
#define RECTHEIGHT(x)  ((x).bottom - (x).top)

extern ULONG        g_cDllRef;
extern HINSTANCE    g_hInst;

extern CLSID CLSID_DeskBand;

static const WCHAR g_szITunesToolbarClass[] = L"ITunesToolbarClassname";

RECT g_rcprev = {3, 2, 3 + 17, 2 + 19};
RECT g_rcplay = {g_rcprev.right, g_rcprev.top, g_rcprev.right + 21, g_rcprev.bottom};
RECT g_rcnext = {g_rcplay.right, g_rcprev.top, g_rcplay.right + 17, g_rcprev.bottom};

custom_trackbar CTB_VOLUME("", WS_VISIBLE | WS_CHILD, 64, 7, 55, 9, 1,
	0, 100, 0, 1, 5,
	9, CTB_DRAGGING_VALUE, CTB_DRAGGING_SMOOTH_VALUE_SNAP, {
		RGB(10, 10, 10), RGB(50, 50, 50), RGB(80, 80, 80), RGB(127, 127, 127),							// background/border/window name idle/highlight
		RGB(0, 127, 255), RGB(0, 127, 255), RGB(0, 127, 255), RGB(0, 127, 255),							// left/right channel, left/right highlight
		RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127), 									// thumb background: idle/hover/selected
		RGB(10, 10, 10), RGB(10, 10, 10), RGB(10, 10, 10),			 									// thumb border: idle/hover/selected
		RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127)										// thumb text: idle/hover/selected
	},
	CTB_SHOW_FOCUS_RECT, 0
);

HWND g_songinfo = NULL;
CComPtr<CDeskBand> g_db;

HFONT font_rainbow = CreateFont(12, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");
HFONT font = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");

struct COLOR {
	BYTE R, G, B;
	COLOR(BYTE r, BYTE g, BYTE b) { R = r; G = g; B = b; }
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
deque<COLOR> g_info_colors = g_colors;

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
		rcArtist.top = 11;
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

enum {
	TIMER_HIDE_SONGINFO = 1,
	TIMER_DESKBAND_RAINBOW_ROTATE,
	TIMER_ARTIST_ROTATE,
	TIMER_SONG_ROTATE,
	TIMER_SONGINFO_ROTATE_DELAY,

};

ULONG_PTR gdiplusToken;
map<string, HBITMAP> btnImageFiles;

HHOOK kbhook;
LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam);

void cleanup() {
	if (g_songinfo) {
		DeleteObject(font);
		DestroyWindow(g_songinfo);
		g_songinfo = NULL;
	}

	g_colors.clear();

	if (g_db) {
		//cout << "g_db->Release() " << g_db->Release() << '\n';
		cout << "g_db->CloseDW(0) " << hex << g_db->CloseDW(0) << '\n';
		g_db = NULL;
	}

	Icleanup();
	
	for (auto a : btnImageFiles) {
		DeleteObject(a.second);
		a.second = NULL;
	}
	if (btnImageFiles.size())
		btnImageFiles.clear();

	UnhookWindowsHookEx(kbhook);
}

CDeskBand::CDeskBand():
	m_cRef(1), m_pSite(NULL), m_fHasFocus(FALSE), m_fIsDirty(FALSE), m_dwBandID(0), m_hwnd(NULL), m_hwndParent(NULL) {}

CDeskBand::~CDeskBand() {
	if (m_pSite) {
	//	cout << "m_pSite->Release() " << m_pSite->Release() << '\n';
	}
}

//
// IUnknown
//
STDMETHODIMP CDeskBand::QueryInterface(REFIID riid, void **ppv) {
	HRESULT hr = S_OK;

	if (IsEqualIID(IID_IUnknown, riid) ||
		IsEqualIID(IID_IOleWindow, riid) ||
		IsEqualIID(IID_IDockingWindow, riid) ||
		IsEqualIID(IID_IDeskBand, riid) ||
		IsEqualIID(IID_IDeskBand2, riid)) {
		*ppv = static_cast<IOleWindow *>(this);
	}
	else if (IsEqualIID(IID_IPersist, riid) ||
		IsEqualIID(IID_IPersistStream, riid)) {
		*ppv = static_cast<IPersist *>(this);
	}
	else if (IsEqualIID(IID_IObjectWithSite, riid)) {
		*ppv = static_cast<IObjectWithSite *>(this);
	}
	else if (IsEqualIID(IID_IInputObject, riid)) {
		*ppv = static_cast<IInputObject *>(this);
	}
	else {
		hr = E_NOINTERFACE;
		*ppv = NULL;
	}

	if (*ppv) {
		AddRef();
	}

	return hr;
}

STDMETHODIMP_(ULONG) CDeskBand::AddRef() {
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CDeskBand::Release() {
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (0 == cRef) {
		delete this;
	}

	return cRef;
}

//
// IOleWindow
//
STDMETHODIMP CDeskBand::GetWindow(HWND *phwnd) {
	*phwnd = m_hwnd;
	return S_OK;
}

STDMETHODIMP CDeskBand::ContextSensitiveHelp(BOOL) {
	return E_NOTIMPL;
}

//
// IDockingWindow
//
STDMETHODIMP CDeskBand::ShowDW(BOOL fShow) {
	if (m_hwnd) {
		ShowWindow(m_hwnd, fShow ? SW_SHOW : SW_HIDE);
	}

	return S_OK;
}

STDMETHODIMP CDeskBand::CloseDW(DWORD) {
	if (m_hwnd) {
		ShowWindow(m_hwnd, SW_HIDE);
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;
	}

	return S_OK;
}

STDMETHODIMP CDeskBand::ResizeBorderDW(const RECT *, IUnknown *, BOOL) {
	return E_NOTIMPL;
}

//
// IDeskBand
//
STDMETHODIMP CDeskBand::GetBandInfo(DWORD dwBandID, DWORD, DESKBANDINFO *pdbi) {
	HRESULT hr = E_INVALIDARG;

	if (pdbi) {
		m_dwBandID = dwBandID;

		if (pdbi->dwMask & DBIM_MINSIZE) {
			pdbi->ptMinSize.x = 142;
			pdbi->ptMinSize.y = 23;
		}

		if (pdbi->dwMask & DBIM_MAXSIZE) {
			pdbi->ptMaxSize.y = -1;
		}

		if (pdbi->dwMask & DBIM_INTEGRAL) {
			pdbi->ptIntegral.y = 1;
		}

		if (pdbi->dwMask & DBIM_ACTUAL) {
			pdbi->ptActual.x = 142;
			pdbi->ptActual.y = 23;
		}

		if (pdbi->dwMask & DBIM_TITLE) {
			// Don't show title by removing this flag.
			//pdbi->dwMask &= ~DBIM_TITLE;
		}

		if (pdbi->dwMask & DBIM_MODEFLAGS) {
			pdbi->dwModeFlags = DBIMF_NORMAL;
		}

		if (pdbi->dwMask & DBIM_BKCOLOR) {
			// Use the default background color by removing this flag.
			pdbi->dwMask &= ~DBIM_BKCOLOR;
		}

		hr = S_OK;
	}

	return hr;
}

//
// IDeskBand2
//
STDMETHODIMP CDeskBand::CanRenderComposited(BOOL *pfCanRenderComposited) {
	*pfCanRenderComposited = TRUE;

	return S_OK;
}

STDMETHODIMP CDeskBand::SetCompositionState(BOOL fCompositionEnabled) {
	m_fCompositionEnabled = fCompositionEnabled;

	InvalidateRect(m_hwnd, NULL, TRUE);
	UpdateWindow(m_hwnd);

	return S_OK;
}

STDMETHODIMP CDeskBand::GetCompositionState(BOOL *pfCompositionEnabled) {
	*pfCompositionEnabled = m_fCompositionEnabled;

	return S_OK;
}

//
// IPersist
//
STDMETHODIMP CDeskBand::GetClassID(CLSID *pclsid) {
	*pclsid = CLSID_DeskBand;
	return S_OK;
}

//
// IPersistStream
//
STDMETHODIMP CDeskBand::IsDirty() {
	return m_fIsDirty ? S_OK : S_FALSE;
}

STDMETHODIMP CDeskBand::Load(IStream * /*pStm*/) {
	return S_OK;
}

STDMETHODIMP CDeskBand::Save(IStream * /*pStm*/, BOOL fClearDirty) {
	if (fClearDirty) {
		m_fIsDirty = FALSE;
	}

	return S_OK;
}

STDMETHODIMP CDeskBand::GetSizeMax(ULARGE_INTEGER * /*pcbSize*/) {
	return E_NOTIMPL;
}

//
// IObjectWithSite
//
STDMETHODIMP CDeskBand::SetSite(IUnknown *pUnkSite) {
	HRESULT hr = S_OK;

	m_hwndParent = NULL;

	if (m_pSite) {
		//m_pSite->Release();
	}

	if (pUnkSite) {
		CComPtr<IOleWindow> pow;
		hr = pUnkSite->QueryInterface(IID_IOleWindow, reinterpret_cast<void **>(&pow));
		if (SUCCEEDED(hr)) {
			hr = pow->GetWindow(&m_hwndParent);
			if (SUCCEEDED(hr)) {
				WNDCLASSW wc = {0};
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.hCursor = LoadCursor(NULL, IDC_ARROW);
				wc.hInstance = g_hInst;
				wc.lpfnWndProc = WndProc;
				wc.lpszClassName = g_szITunesToolbarClass;

				RegisterClassW(&wc);
				
				CreateWindowExW(0, g_szITunesToolbarClass, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_hwndParent, NULL, g_hInst, this);
				
				if (!m_hwnd) {					
					hr = E_FAIL;
				}
			}

			//pow->Release();
		}

		hr = pUnkSite->QueryInterface(IID_IInputObjectSite, reinterpret_cast<void **>(&m_pSite));
	}

	return hr;
}

STDMETHODIMP CDeskBand::GetSite(REFIID riid, void **ppv) {
	HRESULT hr = E_FAIL;

	if (m_pSite) {
		hr = m_pSite->QueryInterface(riid, ppv);
	}
	else {
		*ppv = NULL;
	}

	return hr;
}

//
// IInputObject
//
STDMETHODIMP CDeskBand::UIActivateIO(BOOL fActivate, MSG *) {
	if (fActivate) {
		SetFocus(m_hwnd);
	}

	return S_OK;
}

STDMETHODIMP CDeskBand::HasFocusIO() {
	return m_fHasFocus ? S_OK : S_FALSE;
}

STDMETHODIMP CDeskBand::TranslateAcceleratorIO(MSG *) {
	return S_FALSE;
};

void CDeskBand::OnFocus(const BOOL fFocus) {
	m_fHasFocus = fFocus;

	if (m_pSite) {
		m_pSite->OnFocusChangeIS(static_cast<IOleWindow*>(this), m_fHasFocus);
	}
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
			if (Iisplay()) {
				for (int i = 0; i < 10; i++) {
					COLOR last = g_colors[g_colors.size() - 1];
					g_colors.pop_back();
					g_colors.push_front(last);
				}
			}			
			InvalidateRect(hwnd, &getclientrect(hwnd), FALSE);

			if (g_songinfo) {
				for (int i = 0; i < 10; i++) {
					COLOR last = g_info_colors[g_info_colors.size() - 1];
					g_info_colors.pop_back();
					g_info_colors.push_front(last);
				}
				InvalidateRect(g_songinfo, &getclientrect(g_songinfo), TRUE);
			}

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
void HandleHeadphoneJackStateChange() {
	HRESULT hr = S_OK;
	CCoInitialize ci(NULL, &hr);
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
				cout << guid << " " << state << '\n';
				if (guid == "{0.0.1.00000000}.{913e4018-4d7b-45d2-a694-0cfe9145da95}") {
					static bool resume = false;
					if (state == 8) {
						if (Iisplay()) {
							Iplaypause();
							resume = true;
						}
						else
							resume = false;
					}
					else if (resume) {
						if (!Iisplay()) {
							Iplaypause();
						}
					}
				}
			}

		}
	}
	else
		printf("CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
}
LRESULT CALLBACK StaticProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE: {
			_tzset();
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

			HBRUSH backbrush = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDCmem, &rcclient, backbrush);

			int volume = Igetvol();

			long song_progress, song_total_time;
			Igetsongprogress(&song_progress, &song_total_time);
			
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);

			if (!Iisplay()) {
				SetTextColor(hDCmem, RGB(0, 127, 255));
				SetBkColor(hDCmem, RGB(10, 10, 10));

				DeleteObject(SelectObject(hDCmem, font_rainbow));

				ExtTextOut(hDCmem, songinfo.rcArtist.left, songinfo.rcArtist.top, ETO_CLIPPED, &songinfo.rcArtist, STW(songinfo.artist), songinfo.artist.size(), NULL);
				ExtTextOut(hDCmem, songinfo.rcSong.left, songinfo.rcSong.top, ETO_CLIPPED, &songinfo.rcSong, STW(songinfo.song), songinfo.song.size(), NULL);

				if (songinfo.bArtistPart2)
					ExtTextOut(hDCmem, songinfo.rcArtist2.left, songinfo.rcArtist2.top, ETO_CLIPPED, &songinfo.rcArtist2, STW(songinfo.artist), songinfo.artist.size(), NULL);
				if (songinfo.bSongPart2)
					ExtTextOut(hDCmem, songinfo.rcSong2.left, songinfo.rcSong2.top, ETO_CLIPPED, &songinfo.rcSong2, STW(songinfo.song), songinfo.song.size(), NULL);

				//draw date and time
				HFONT dt_font = CreateFont(11, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");
				HFONT oldfont = (HFONT)SelectObject(hDCmem, dt_font);
				SIZE sz;
				//draw date
				std::wstring ws_date(80, '\0');
				ws_date.resize(wcsftime(&ws_date[0], ws_date.size(), L"%m/%d (%a)", &tm));	//%#x - long date
				GetTextExtentPoint32(hDCmem, ws_date.c_str(), ws_date.size(), &sz);
				RECT rcdate;
				rcdate.left = 5;
				rcdate.top = 0;
				rcdate.right = rcdate.left + sz.cx;
				rcdate.bottom = rcdate.bottom + sz.cy;
				ExtTextOut(hDCmem, 5, 0, ETO_CLIPPED, &rcdate, ws_date.c_str(), ws_date.size(), NULL);
				//draw time
				std::wstring ws_time(80, '\0');
				ws_time.resize(wcsftime(&ws_time[0], ws_time.size(), L"%I:%M %p", &tm)); //%a - short weekday							
				GetTextExtentPoint32(hDCmem, ws_time.c_str(), ws_time.size(), &sz);	
				RECT rctime;
				rctime.left = rcclient.right - sz.cx - 5;
				rctime.top = 0;
				rctime.right = rctime.left + sz.cx;
				rctime.bottom = rctime.top + sz.cy;
				ExtTextOut(hDCmem, rctime.left, rctime.top, ETO_CLIPPED, &rctime, ws_time.c_str(), ws_time.size(), NULL);				
				SelectObject(hDCmem, oldfont);
				DeleteFont(dt_font);

				//draw progress bar
				RECT rcprogress = {rcclient.left, rcclient.bottom - 2, (int)((double)(song_progress * rcclient.right) / (double)song_total_time), rcclient.bottom};
				HBRUSH progress_brush = CreateSolidBrush(RGB(0, 127, 255));
				FillRect(hDCmem, &rcprogress, progress_brush);
				DeleteBrush(progress_brush);
				//draw progress text
				HFONT progress_font = CreateFont(11, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");
				oldfont = (HFONT)SelectObject(hDCmem, progress_font);
				std::string sprogress = milliseconds_to_hms(song_progress * 1000) + "/" + milliseconds_to_hms(song_total_time * 1000);
				std::wstring ws_progress(sprogress.begin(), sprogress.end());
				GetTextExtentPoint32(hDCmem, ws_progress.c_str(), ws_progress.size(), &sz);
				RECT rcprogresstext;
				rcprogresstext.left = 5;
				rcprogresstext.top = rcclient.bottom - 13;
				rcprogresstext.right = rcprogresstext.left + sz.cx;
				rcprogresstext.bottom = rcprogresstext.top + sz.cy;
				ExtTextOut(hDCmem, rcprogresstext.left, rcprogresstext.top, ETO_CLIPPED, &rcprogresstext, ws_progress.c_str(), ws_progress.size(), NULL);
				//draw volume
				std::wstring ws_vol = int_to_wstr(volume) + L"%";
				GetTextExtentPoint32(hDCmem, ws_vol.c_str(), ws_vol.size(), &sz);
				RECT rcvol;
				rcvol.left = rcclient.right - sz.cx - 5;
				rcvol.top = rcclient.bottom - 13;
				rcvol.right = rcvol.left + sz.cx;
				rcvol.bottom = rcvol.top + sz.cy;
				ExtTextOut(hDCmem, rcvol.left, rcvol.top, ETO_CLIPPED, &rcvol, ws_vol.c_str(), ws_vol.size(), NULL);

				SelectObject(hDCmem, oldfont);
				DeleteFont(progress_font);

				
				
			}
			else {
				auto draw_songinfo_text_back = [](int w, int h, int contrast) {
					HBITMAP bitmap = CreateBitmap(w, h, 1, 32, NULL);

					HDC hdcMem = CreateCompatibleDC(0);

					HBITMAP old = SelectBitmap(hdcMem, bitmap);

					RECT rc = {0, 0, 0, h};
					for (int i = 0; i < w; i++) {
						rc.left = i;
						rc.right = (i + 1);
						COLORREF col = COLOR::rgb(g_info_colors[i % g_info_colors.size()]);
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

					DeleteObject(SelectObject(hdcMem, font_rainbow));

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

				auto draw_rainbow_text = [](HDC hdc, HFONT font, const std::wstring& text, int x, int y, COLORREF backcol, UINT rainbow_start = 0, COLORREF transparent = 0) {
					HDC hdcMem = CreateCompatibleDC(0);

					DeleteObject(SelectObject(hdcMem, font));

					SIZE sz;
					GetTextExtentPoint32(hdcMem, text.c_str(), text.size(), &sz);
					RECT rctext = {0, 0, sz.cx, sz.cy};

					int text_width = rctext.right - rctext.left;
					int text_height = rctext.bottom - rctext.top;

					//draw rainbow background
					HBITMAP bmp_back = CreateBitmap(text_width, text_height, 1, 32, NULL);
					HBITMAP old_bmp_back = SelectBitmap(hdcMem, bmp_back);
					double step = 360.0 / (double)text_width;
					RECT rc = {0, 0, 0, text_height};
					for (int i = 0; i < text_width; i++) {
						rc.left = i;
						rc.right = (i + 1);
						double hue = (double)((int)((i + rainbow_start) * step) % 361);
					//	COLORREF col = HSVtoRGB(hue, 1.0, 1.0);
						COLORREF col = COLOR::rgb(g_info_colors[(i + rainbow_start) % g_info_colors.size()]);
						HBRUSH brush = CreateSolidBrush(col);
						FillRect(hdcMem, &rc, brush);
						DeleteObject(brush);
					}
					bmp_back = SelectBitmap(hdcMem, old_bmp_back);
					HBITMAPBlitToHdc(hdc, bmp_back, x, y, text_width, text_height);

					//get text bitmap
					HBITMAP bmp_text = CreateBitmap(text_width, text_height, 1, 32, NULL);
					SetTextColor(hdcMem, transparent);
					SetBkColor(hdcMem, backcol);
					HBITMAP old_bmp_text = SelectBitmap(hdcMem, bmp_text);
					ExtTextOut(hdcMem, 0, 0, ETO_CLIPPED, &rctext, text.c_str(), text.size(), NULL);
					bmp_text = SelectBitmap(hdcMem, old_bmp_text);
					TransparentBlitToHdc(hdc, bmp_text, x, y, rctext.right - rctext.left, rctext.bottom - rctext.top, transparent);

					//cleanup
					DeleteObject(bmp_back);
					DeleteObject(bmp_text);

					DeleteDC(hdcMem);
				};

				int height = getRectHeight(songinfo.rcArtist);

				//draw artist
				HBITMAP bmpartist_back = draw_songinfo_text_back(rcclient.right, height, 0);
				HBITMAPBlitToHdc(hDCmem, bmpartist_back, 0, songinfo.rcArtist.top, rcclient.right, height);
				HBITMAP bmpartist = draw_songinfo_text(songinfo.getArtistWidth(), height, songinfo.artist, songinfo.bArtistPart2);
				RECT rclart = {0, songinfo.rcArtist.top, songinfo.rcArtist.left, songinfo.rcArtist.bottom};
				FillRect(hDCmem, &rclart, backbrush);
				if (!songinfo.bArtistPart2) {
					RECT rcrart = {songinfo.rcArtist.right, songinfo.rcArtist.top, rcclient.right, songinfo.rcArtist.bottom};
					FillRect(hDCmem, &rcrart, backbrush);
				}
				TransparentBlitToHdc(hDCmem, bmpartist, songinfo.rcArtist.left, songinfo.rcArtist.top, songinfo.getArtistWidth(), height, RGB(0, 0, 0));

				//draw song
				HBITMAP bmpsong_back = draw_songinfo_text_back(rcclient.right, height, 80);
				HBITMAPBlitToHdc(hDCmem, bmpsong_back, 0, songinfo.rcSong.top, rcclient.right, height);
				HBITMAP bmpsong = draw_songinfo_text(songinfo.getSongWidth(), height, songinfo.song, songinfo.bSongPart2);
				RECT rclsong = {0, songinfo.rcSong.top, songinfo.rcSong.left, songinfo.rcSong.bottom};
				FillRect(hDCmem, &rclsong, backbrush);
				if (!songinfo.bSongPart2) {
					RECT rcrsong = {songinfo.rcSong.right, songinfo.rcSong.top, rcclient.right, songinfo.rcSong.bottom};
					FillRect(hDCmem, &rcrsong, backbrush);
				}
				TransparentBlitToHdc(hDCmem, bmpsong, songinfo.rcSong.left, songinfo.rcSong.top, songinfo.getSongWidth(), height, RGB(0, 0, 0));

				SIZE sz;

				//draw progress bar
				HBITMAP bmp_progress = draw_songinfo_text_back(rcclient.right, 2, 0);
				HBITMAPBlitToHdc(hDCmem, bmp_progress, 0, rcclient.bottom - 2, (int)((double)(song_progress * rcclient.right) / (double)song_total_time), 2);
				HFONT progress_font = CreateFont(11, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");
				HFONT oldfont = (HFONT)SelectObject(hDCmem, progress_font);
				std::string sprogress = milliseconds_to_hms(song_progress * 1000) + "/" + milliseconds_to_hms(song_total_time * 1000);
				std::wstring ws_progress(sprogress.begin(), sprogress.end());
				draw_rainbow_text(hDCmem, progress_font, ws_progress, 5, rcclient.bottom - 13, RGB(10, 10, 10));
				//draw volume
				std::wstring ws_vol = int_to_wstr(volume) + L"%";
				GetTextExtentPoint32(hDCmem, ws_vol.c_str(), ws_vol.size(), &sz);
				draw_rainbow_text(hDCmem, progress_font, ws_vol, rcclient.right - sz.cx - 5, rcclient.bottom - 13, RGB(10, 10, 10));
				SelectObject(hDCmem, oldfont);
				DeleteFont(progress_font);

				//draw date and time
				HFONT dt_font = CreateFont(11, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");
				//draw date
				std::wstring ws_date(80, '\0');
				ws_date.resize(wcsftime(&ws_date[0], ws_date.size(), L"%m/%d (%a)", &tm));	//%#x - long date //%A, %B %d, %Y
				draw_rainbow_text(hDCmem, dt_font, ws_date, 5, 0, RGB(10, 10, 10));
				//draw time
				std::wstring ws_time(80, '\0');
				ws_time.resize(wcsftime(&ws_time[0], ws_time.size(), L"%I:%M %p", &tm));				
				oldfont = (HFONT)SelectObject(hDCmem, dt_font);
				GetTextExtentPoint32(hDCmem, ws_time.c_str(), ws_time.size(), &sz);
				SelectObject(hDCmem, oldfont);
				draw_rainbow_text(hDCmem, dt_font, ws_time, rcclient.right - sz.cx - 5, 0, RGB(10, 10, 10), rcclient.right - sz.cx - 10);
				DeleteFont(dt_font);

				DeleteObject(bmp_progress);
				DeleteObject(bmpsong);
				DeleteObject(bmpartist);
				DeleteObject(bmpsong_back);
				DeleteObject(bmpartist_back);
			}

			BitBlt(hdc, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

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
		case WM_DEVICECHANGE: {
			switch (wParam) {
				case 7: { 
					thread a(HandleHeadphoneJackStateChange);
					a.detach();

					break;
				}
			}
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
		wc.lpfnWndProc = StaticProc;
		wc.style = NULL;
		RegisterClassEx(&wc);

		g_songinfo = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"CurrentSongClassName", L"", WS_VISIBLE, 0, 0, 200, 50, NULL, NULL, NULL, NULL);
		SetWindowLong(g_songinfo, GWL_STYLE, 0);
	}

	//if (Iisplay()) {
		songinfo.artist = Igetcurartist();
		songinfo.song = Igetcursong();
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

		SetTimer(g_songinfo, TIMER_HIDE_SONGINFO, 7000, TimerProc);

		SetTimer(g_songinfo, TIMER_SONGINFO_ROTATE_DELAY, 1000, TimerProc);

		SetTimer(g_db->getHWND(), TIMER_DESKBAND_RAINBOW_ROTATE, 100, TimerProc);
	/*}
	else {
		ShowWindow(g_songinfo, SW_HIDE);
	}*/
}

HRESULT CiTunesEventHandler::OnSoundVolumeChangedEvent() {
	CTB_VOLUME.redraw_newval(Igetvol(), false);

	DisplaySongInfo();

	return S_OK;
}

HRESULT CiTunesEventHandler::OnAboutToPromptUserToQuitEvent() {
	cleanup();

	return S_OK;
}

HRESULT CiTunesEventHandler::OnPlayerPlayEvent() {
	DisplaySongInfo();

	RedrawWindow(g_db->getHWND(), &getwindowrect(g_db->getHWND()), NULL, RDW_INVALIDATE);

	return S_OK;
}

HRESULT CiTunesEventHandler::OnPlayerStopEvent() {
//	KillTimer(g_db->getHWND(), TIMER_DESKBAND_RAINBOW_ROTATE);

//	RedrawWindow(g_db->getHWND(), &getwindowrect(g_db->getHWND()), NULL, RDW_INVALIDATE);

	return S_OK;
}

Image* LoadPicture(LPCWSTR lpszFileName) {
	return Image::FromFile(lpszFileName, FALSE);
}

LRESULT CALLBACK CDeskBand::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CDeskBand *db = reinterpret_cast<CDeskBand *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (message == WM_CUSTOM_TRACKBAR_VALUE_CHANGED) {
		custom_trackbar ctb = *((custom_trackbar*)lParam);
		if (ctb.handle == CTB_VOLUME.handle) {
			Isetvol(ctb.current_val);
		}
	}
	switch (message) {
		case WM_CREATE: {
			db = reinterpret_cast<CDeskBand *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
			db->m_hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(db));

			g_db = db;

			custom_trackbar* a = &CTB_VOLUME;
			a->handle = CreateWindow(custom_trackbar_classname, str_to_wstr(a->window_name).c_str(),
				a->window_styles,
				a->xpos, a->ypos,
				a->width, a->height,
				hwnd, (HMENU)a->id, NULL, a);

			CreateWindow(L"BUTTON", L"", BS_OWNERDRAW | WS_VISIBLE | WS_CHILD, 129, 2, 11, 9, hwnd, (HMENU)IDC_BTN_MAX, NULL, NULL);

			init_itunes_com();

			CTB_VOLUME.redraw_newval(Igetvol(), false);

			g_colors = getGradient(g_colors, (142.0 * 1) / 12);
			g_info_colors = getGradient(g_info_colors, 20);

			if (Iisplay())
				DisplaySongInfo();

			GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

			break;
		}
		case WM_PAINT: {
			auto DrawBtnImage = [&](HDC hDC, RECT rc, string filename) {
				if (btnImageFiles[filename] == NULL) {
					Image* img = LoadPicture(STW(filename));
					if (img) {
						Bitmap* pBitmap = static_cast<Bitmap*>(img->Clone());
						HBITMAP bmp = NULL;
						pBitmap->GetHBITMAP(Color(0, 0, 0), &bmp);
						btnImageFiles[filename] = bmp;
						delete pBitmap;
						delete img;
					}
				}

				HBITMAP* hitembmp = &btnImageFiles[filename];

				if (hitembmp) {
					TransparentBlitToHdc(hDC, *hitembmp, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, RGB(0, 0, 0));
				}
			};

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rcclient = getclientrect(hwnd);

			HDC hDCmem = CreateCompatibleDC(hdc);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hdc, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			
			//paint background (rainbow)
			RECT trc = rcclient;
			for (int i = 0; i < rcclient.right; i++) {
				trc.left = i;
				trc.right = (i + 1);
				HBRUSH brush = CreateSolidBrush(COLOR::rgb(g_colors[i % g_colors.size()]));
				FillRect(hDCmem, &trc, brush);
				DeleteObject(brush);
			}

			//paint buttons
			string dir = "C:\\Users\\Josh\\Documents\\Visual Studio 2017\\Projects\\ITunesToolbar\\ITunesToolbar\\res\\";
			DrawBtnImage(hDCmem, g_rcprev, dir + "prev.png");
			DrawBtnImage(hDCmem, g_rcplay, dir + (Iisplay() ? "pause.png" : "play.png"));
			DrawBtnImage(hDCmem, g_rcnext, dir + "next.png");

			BitBlt(hdc, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND: {
			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_BTN_PREV: {
					Iprev();
					break;
				}
				case IDC_BTN_PLAY: {
					Iplaypause();
					break;
				}
				case IDC_BTN_NEXT: {
					Inext();
					break;
				}
				case IDC_BTN_MAX: {
					Imaximize();
					break;
				}
				case IDC_BTN_VOLU: {
					Ivolu(lParam);
					break;
				}
				case IDC_BTN_VOLD: {
					Ivold(lParam);
					break;
				}
				case IDC_BTN_SHOW_PLAYING: {
					DisplaySongInfo();
					break;
				}
			}
			break;
		}
		case WM_LBUTTONUP: {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			POINT pt = {xPos, yPos};

			if (PtInRect(&g_rcprev, pt))
				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_PREV, 0), NULL);
			else if (PtInRect(&g_rcplay, pt))
				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_PLAY, 0), NULL);
			else if (PtInRect(&g_rcnext, pt))
				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_NEXT, 0), NULL);

			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_SETFOCUS: {
			db->OnFocus(TRUE);
			break;
		}
		case WM_KILLFOCUS: {
			db->OnFocus(FALSE);
			break;
		}
		case WM_DESTROY: {
			static bool is_destroy = false;
			if (!is_destroy) {
				is_destroy = true;
				cleanup();
				Gdiplus::GdiplusShutdown(gdiplusToken);
			}
			break;
		}
	}
	
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int GetKeyFromKBDLLHOOKSTRUCT(const KBDLLHOOKSTRUCT& key) {
	int ret = key.vkCode;

	wchar_t name[0x100] = {0};
	DWORD lp = 1;
	lp += key.scanCode << 16;
	lp += key.flags << 24;
	GetKeyNameText(lp, (LPTSTR)name, 255);
	string sname = wstr_to_str((wstring)name);
	if ((GetKeyState(VK_MENU) & 0x8000) != 0) {
		ret += 256;
		sname.insert(0, "ALT+");
	}
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) {
		ret += 256 * 4;
		sname.insert(0, "SHIFT+");
	}
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
		ret += 256 * 2;
		sname.insert(0, "CTRL+");
	}
	if ((GetKeyState(VK_LWIN) & 0x8000) != 0) {
		ret += 256 * 8;
		sname.insert(0, "LEFT WINDOWKEY+");
	}
	if ((GetKeyState(VK_RWIN) & 0x8000) != 0) {
		ret += 256 * 8;
		sname.insert(0, "RIGHT WINDOWKEY+");
	}

	return ret;
}

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

		int keyCode = GetKeyFromKBDLLHOOKSTRUCT(key);
#pragma region virtual keycode processing	
		
		enum {
			ALT = 256,
			CTRL = 512,
			SHIFT = 1024
		};
#pragma endregion	
		if (VK_F9 == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_PLAY, 0), NULL);
		}
		else if (SHIFT + VK_F10 == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_NEXT, 0), NULL);
		}
		else if (SHIFT + VK_F11 == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_PREV, 0), NULL);
		}
		else if (SHIFT + VK_DOWN == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_VOLD, 0), VOL_CHANGE_INCREMENT);
		}
		else if (SHIFT + VK_UP == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_VOLU, 0), VOL_CHANGE_INCREMENT);
		}
		else if (SHIFT + CTRL + VK_DOWN == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_VOLD, 0), VOL_CHANGE_INCREMENT_LARGE);
		}
		else if (SHIFT + CTRL + VK_UP == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_VOLU, 0), VOL_CHANGE_INCREMENT_LARGE);
		}
		else if (VK_F10 == keyCode) {
			PostMessage(g_db->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_BTN_SHOW_PLAYING, 0), NULL);
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

