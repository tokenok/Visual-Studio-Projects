#include "statview.h"

#include <windowsx.h>
#include <Richedit.h>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
#include <assert.h>
#include <Richole.h>

#include "common.h"
#include "include.h"
#include "invview.h"

#pragma warning(disable: 4100)

using namespace std;
using namespace Gdiplus;

ATOM init_register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = {0};
	wnd.hInstance = hInstance;
	wnd.lpszClassName = newstatwndclassname;
	wnd.lpfnWndProc = (WNDPROC)newstatProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	//wnd.hIconSm = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(TOKENICO));
	return RegisterClassEx(&wnd);
}
int asdkfjaeslkfjsdf() {
	init_register_class(g_hInst);
	return 1;
}
int asdkfjaeslkfjsdf1 = asdkfjaeslkfjsdf();

class CImageDataObject: IDataObject {
	public:
	// This static function accepts a pointer to IRochEditOle 
	//   and the bitmap handle.
	// After that the function insert the image in the current 
	//   position of the RichEdit
	//
	static void InsertBitmap(IRichEditOle* pRichEditOle,
		HBITMAP hBitmap);

	private:
	ULONG m_ulRefCnt;
	BOOL  m_bRelease;

	// The data being bassed to the richedit
	//
	STGMEDIUM m_stgmed;
	FORMATETC m_fromat;

	public:
	CImageDataObject(): m_ulRefCnt(0) {
		m_bRelease = FALSE;
	}

	~CImageDataObject() {
		if (m_bRelease)
			::ReleaseStgMedium(&m_stgmed);
	}

	// Methods of the IUnknown interface
	// 
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) {
		if (iid == IID_IUnknown || iid == IID_IDataObject) {
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		else
			return E_NOINTERFACE;
	}
	STDMETHOD_(ULONG, AddRef)(void) {
		m_ulRefCnt++;
		return m_ulRefCnt;
	}
	STDMETHOD_(ULONG, Release)(void) {
		if (--m_ulRefCnt == 0) {
			delete this;
		}

		return m_ulRefCnt;
	}

	// Methods of the IDataObject Interface
	//
	STDMETHOD(GetData)(FORMATETC *pformatetcIn,
		STGMEDIUM *pmedium) {
		HANDLE hDst;
		hDst = ::OleDuplicateData(m_stgmed.hBitmap,
			CF_BITMAP, NULL);
		if (hDst == NULL) {
			return E_HANDLE;
		}

		pmedium->tymed = TYMED_GDI;
		pmedium->hBitmap = (HBITMAP)hDst;
		pmedium->pUnkForRelease = NULL;

		return S_OK;
	}
	STDMETHOD(GetDataHere)(FORMATETC* pformatetc,
		STGMEDIUM*  pmedium) {
		return E_NOTIMPL;
	}
	STDMETHOD(QueryGetData)(FORMATETC*  pformatetc) {
		return E_NOTIMPL;
	}
	STDMETHOD(GetCanonicalFormatEtc)(FORMATETC*  pformatectIn,
		FORMATETC* pformatetcOut) {
		return E_NOTIMPL;
	}
	STDMETHOD(SetData)(FORMATETC* pformatetc,
		STGMEDIUM*  pmedium,
		BOOL  fRelease) {
		m_fromat = *pformatetc;
		m_stgmed = *pmedium;

		return S_OK;
	}
	STDMETHOD(EnumFormatEtc)(DWORD  dwDirection,
		IEnumFORMATETC**  ppenumFormatEtc) {
		return E_NOTIMPL;
	}
	STDMETHOD(DAdvise)(FORMATETC *pformatetc,
		DWORD advf,
		IAdviseSink *pAdvSink,
		DWORD *pdwConnection) {
		return E_NOTIMPL;
	}
	STDMETHOD(DUnadvise)(DWORD dwConnection) {
		return E_NOTIMPL;
	}
	STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise) {
		return E_NOTIMPL;
	}
	// Some Other helper functions
	//
	void SetBitmap(HBITMAP hBitmap);
	IOleObject *GetOleObject(IOleClientSite *pOleClientSite,
		IStorage *pStorage);
};
void CImageDataObject::InsertBitmap(IRichEditOle* pRichEditOle, HBITMAP hBitmap) {
	SCODE sc;

	// Get the image data object
	//
	CImageDataObject *pods = new CImageDataObject;
	LPDATAOBJECT lpDataObject;
	pods->QueryInterface(IID_IDataObject, (void **)&lpDataObject);

	pods->SetBitmap(hBitmap);

	// Get the RichEdit container site
	//
	IOleClientSite *pOleClientSite;
	pRichEditOle->GetClientSite(&pOleClientSite);

	// Initialize a Storage Object
	//
	IStorage *pStorage;

	LPLOCKBYTES lpLockBytes = NULL;
	sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (sc != S_OK)
		throw sc;
	assert(lpLockBytes != NULL);

	sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);
	if (sc != S_OK) {
		assert(lpLockBytes->Release() == 0); 
		lpLockBytes = NULL;
		throw sc;
	}
	assert(pStorage != NULL);

	// The final ole object which will be inserted in the richedit control
	//
	IOleObject *pOleObject;
	pOleObject = pods->GetOleObject(pOleClientSite, pStorage);

	// all items are "contained" -- this makes our reference to this object
	//  weak -- which is needed for links to embedding silent update.
	OleSetContainedObject(pOleObject, TRUE);

	// Now Add the object to the RichEdit 
	//
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(REOBJECT);

	CLSID clsid;
	sc = pOleObject->GetUserClassID(&clsid);
	if (sc != S_OK)
		throw sc;

	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.poleobj = pOleObject;
	reobject.polesite = pOleClientSite;
	reobject.pstg = pStorage;

	// Insert the bitmap at the current location in the richedit control
	//
	pRichEditOle->InsertObject(&reobject);

	// Release all unnecessary interfaces
	//
	pOleObject->Release();
	pOleClientSite->Release();
	pStorage->Release();
	lpDataObject->Release();
}
void CImageDataObject::SetBitmap(HBITMAP hBitmap) {
	assert(hBitmap);

	STGMEDIUM stgm;
	stgm.tymed = TYMED_GDI;					// Storage medium = HBITMAP handle		
	stgm.hBitmap = hBitmap;
	stgm.pUnkForRelease = NULL;				// Use ReleaseStgMedium

	FORMATETC fm;
	fm.cfFormat = CF_BITMAP;				// Clipboard format = CF_BITMAP
	fm.ptd = NULL;							// Target Device = Screen
	fm.dwAspect = DVASPECT_CONTENT;			// Level of detail = Full content
	fm.lindex = -1;							// Index = Not applicaple
	fm.tymed = TYMED_GDI;					// Storage medium = HBITMAP handle

	this->SetData(&fm, &stgm, TRUE);
}
IOleObject *CImageDataObject::GetOleObject(IOleClientSite *pOleClientSite, IStorage *pStorage) {
	assert(m_stgmed.hBitmap);

	SCODE sc;
	IOleObject *pOleObject;
	sc = ::OleCreateStaticFromData(this, IID_IOleObject, OLERENDER_FORMAT,
		&m_fromat, pOleClientSite, pStorage, (void **)&pOleObject);
	if (sc != S_OK)
		throw sc;
	return pOleObject;
}

COLORREF getitemqualitycolor(std::string quality) {
	if (quality == "invalid")
		return RGB(0xff, 0x0, 0x0);
	else if (quality == "low quality")
		return RGB(0x50, 0x50, 0x50);
	else if (quality == "superior")
		return RGB(0x50, 0x50, 0x50);
	else if (quality == "ethereal/socketed")
		return RGB(0x50, 0x50, 0x50);
	else if (quality == "normal")
		return RGB(0xc6, 0xc4, 0xc5);
	else if (quality == "magic")
		return RGB(0x78, 0x7c, 0xe7);
	else if (quality == "set")
		return RGB(24, 252, 0);
	else if (quality == "rare")
		return RGB(0xf5, 0xf7, 0x92);
	else if (quality == "unique")
		return RGB(0xAF, 0xA6, 0x69);
	else if (quality == "runeword")
		return RGB(0xAF, 0xA6, 0x69);
	else if (quality == "quest")
		return RGB(0xAF, 0xA6, 0x69);
	else if (quality == "crafted")
		return RGB(0xc8, 0x88, 0x31);
	else if (quality == "error")
		return RGB(0xff, 0xff, 0xff);
	else
		return RGB(0, 0, 0);
}
void SetRichTextColor(HWND rich, COLORREF text_col, int start, int end) {
	CHARRANGE cr = {start, end};
	SendMessage(rich, EM_EXSETSEL, 0, (LPARAM)&cr);
	CHARFORMAT2 cf2;
	cf2.cbSize = sizeof(cf2);
	cf2.dwMask = CFM_COLOR | CFM_BOLD;
	cf2.dwEffects = CFE_BOLD;
	if (text_col != NULL) cf2.crTextColor = text_col;
	SendMessage(rich, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
}
BOOL HighlightTextInSelection(HWND rich, const vector<string>& targets, int start, int end) {
	FINDTEXTEX ftex;
	ftex.chrg.cpMax = end;
	for (UINT i = 0; i < targets.size(); i++) {
		vector<string> ors = split_str(targets[i], "|");
		for (UINT j = 0; j < ors.size(); j++) {
			while (ors[j].size() && ors[j][0] == ' ') ors[j].erase(0, 1);
			ftex.lpstrText = str_to_LPWSTR(ors[j]);
			ftex.chrg.cpMin = start;
			ftex.chrgText.cpMax = 0;
			while (end >= 0 && ftex.chrgText.cpMax < end) {
				LRESULT lr = SendMessage(rich, EM_FINDTEXTEXW, (WPARAM)FR_DOWN, (LPARAM)&ftex);
				ftex.chrg.cpMin = ftex.chrgText.cpMax;
				if (lr >= 0) {
					SendMessage(rich, EM_EXSETSEL, 0, (LPARAM)&ftex.chrgText);
					CHARFORMAT2 cf2;
					cf2.cbSize = sizeof(cf2);
					cf2.dwMask = CFM_COLOR | CFM_BACKCOLOR;
					cf2.crTextColor = RGB(10, 10, 10);
					cf2.crBackColor = RGB(255, 70, 70);
					SendMessage(rich, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
				}
				else
					break;
			}
			delete[] ftex.lpstrText;
		}
	}
	SendMessage(rich, EM_HIDESELECTION, (LPARAM)TRUE, 0);
	return TRUE;
}
int get_newline_count(string s) {
	if (!s.size()) return 0;
	int count = 0;
	for (UINT i = 0; i < s.size() - 1; i++) {
		if (s[i] == '\r' && s[i + 1] == '\n')
			count++;
	}
	return count;
}
HWND open_item_in_new_wnd(const ItemData* item) {
	if (!item) return NULL;
	HWND newwnd = CreateWindow(newstatwndclassname, str_to_wstr(item->name).c_str(), WS_VISIBLE | WS_OVERLAPPEDWINDOW, 100, 100, 400, 300, NULL, NULL, g_hInst, NULL);
	HWND statview = CreateWindow(L"RichEdit20W", L"",
		ES_CENTER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_BORDER | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | WS_VISIBLE | WS_CHILD,
		0, 0, 400, 300, newwnd, NULL, NULL, NULL);

	SendMessage(statview, EM_SETBKGNDCOLOR, NULL, g_cust_color);
	SendMessage(statview, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);
	SetWindowFont(statview, g_boldfont, NULL);

	update_stat_view(statview, item);

	return newwnd;
}
void update_stat_view(HWND statview, const ItemData* item, int *start, int *end) {
	if (!item) {
		SetWindowText(statview, L"");
		return;
	}

	/*static int count = 0;
	printf("%d: draw\n", count++);*/
	int carstart = 0;
	string text = item->character + "\r\n";
	int carend = text.size() - 1;
	text += "*" + item->account + "\r\n(" + item->realm + ")\r\n" + (item->store == "body" || item->store == "mercenary" ? "on " : "in ") + item->store + "\r\n\r\n";

	int searchstart = text.size() - get_newline_count(text);//5
	text += item->name + "\r\n";
	int itemend = text.size() - get_newline_count(text);//6
	text += item->icode.type + "\r\n";

	int whitestart = itemend;//7
	for (UINT i = 0; i < item->basestats.size(); i++)
		text += item->basestats[i] + "\r\n";
	int whiteend = text.size() - get_newline_count(text);

	int bluestart = whiteend;//8
	for (UINT i = 0; i < item->stats.size(); i++)
		text += item->stats[i] + "\r\n";
	int blueend = text.size() + item->stats.size();

	//add text to item stat view
	SetWindowText(statview, str_to_wstr(text).c_str());
	//BOLD the text and set default color
	SetRichTextColor(statview, RGB(127, 127, 127));

	//color stat view
	SetRichTextColor(statview, RGB(0, 127, 255), carstart, carend);
	SetRichTextColor(statview, getitemqualitycolor(item->quality), searchstart, itemend);
	SetRichTextColor(statview, getitemqualitycolor("normal"), whitestart, whiteend);
	SetRichTextColor(statview, getitemqualitycolor("magic"), bluestart, blueend);

	if (start) *start = searchstart;
	if (end) *end = text.size();

	/*if (1){
		string invfile = item->quality == "unique" && item->invdata.invu.size() > 0 ? item->invdata.invu
			: item->quality == "set" && item->invdata.invs.size() > 0 ? item->invdata.invs
			: item->invdata.inv;
		Image* img = Image::FromFile(str_to_wstr(getexedir() + "\\inv\\" + invfile + ".png").c_str(), FALSE);
		Bitmap* pBitmap = static_cast<Bitmap*>(img->Clone());
		HBITMAP bmp = NULL;
		pBitmap->GetHBITMAP(Color(0, 0, 0), &bmp);
		IRichEditOle* m_pRichEditOle;
		SendMessage(statview, EM_GETOLEINTERFACE, 0, (LPARAM)&m_pRichEditOle);
		CImageDataObject::InsertBitmap(m_pRichEditOle, bmp);
		DeleteObject(bmp);
		delete pBitmap;
		delete img; 
		m_pRichEditOle->Release();
	}*/
}
void update_charstat_view(HWND statview, CharacterStats* stats) {
	string text;
	COLORREF blue = RGB(80, 80, 172);
	COLORREF white = RGB(196, 196, 196);
	COLORREF orange = RGB(208, 132, 32);
	COLORREF yellow = RGB(216, 184, 100);
	COLORREF green = RGB(24, 252, 0);
	COLORREF red = RGB(176, 68, 52);
//	COLORREF gray = RGB(80, 80, 80);
	COLORREF gold = RGB(148, 128, 100);
	vector<pair<int, int>> stat_lengths;
	vector<pair<int, COLORREF>> ids = {
		{STAT_DAMAGERESIST, white},
		{STAT_MAGICRESIST, orange},
		{STAT_FIRERESIST, red},
		{STAT_COLDRESIST, blue},
		{STAT_LIGHTRESIST, yellow},
		{STAT_POISONRESIST, green},
		{STAT_MAXMAGICRESIST, orange},
		{STAT_MAXFIRERESIST, red},
		{STAT_MAXCOLDRESIST, blue},
		{STAT_MAXLIGHTRESIST, yellow},
		{STAT_MAXPOISONRESIST, green},
		{STAT_NORMAL_DAMAGE_REDUCTION, white},
		{STAT_MAGIC_DAMAGE_REDUCTION, white},
		{STAT_ITEM_ABSORBFIRE_PERCENT, red},
		{STAT_ITEM_ABSORBCOLD_PERCENT, blue},
		{STAT_ITEM_ABSORBLIGHT_PERCENT, yellow},
		{STAT_ITEM_ABSORBMAGIC_PERCENT, orange},
		{STAT_ITEM_ABSORBFIRE, red},
		{STAT_ITEM_ABSORBCOLD, blue},
		{STAT_ITEM_ABSORBLIGHT, yellow},
		{STAT_ITEM_ABSORBMAGIC, orange},
		{STAT_ITEM_MAGICBONUS, white},
		{STAT_TOBLOCK, white},
		{STAT_ITEM_FASTERCASTRATE, gold},
		{STAT_ITEM_FASTERATTACKRATE, gold},
		{STAT_ITEM_FASTERMOVEVELOCITY, gold},
		{STAT_ITEM_FASTERGETHITRATE, gold},
		{STAT_ITEM_ADDEXPERIENCE, white},
		{STAT_HPREGEN, white},
		{STAT_ITEM_OPENWOUNDS, white},
		{STAT_ITEM_CRUSHINGBLOW, white},
		{STAT_ITEM_DEADLYSTRIKE, white}
	};
	for (UINT i = 0; i < ids.size(); i++) {
		string s = stats->getStatText(ids[i].first);
		if (stats->getStat(ids[i].first) == 0)
			ids[i].second = 0xFFFFFFFF;
		stat_lengths.push_back(make_pair(text.size(), text.size() + s.size()));
		text += s + '\n';		
	}
	SetWindowText(statview, str_to_wstr(text).c_str());

	for (UINT i = 0; i < ids.size(); i++) {
		SetRichTextColor(statview, ids[i].second, stat_lengths[i].first, stat_lengths[i].second);
	}
}
LRESULT newstatProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_SIZE:{
			HWND edit = ChildWindowFromPoint(hwnd, {0, 0});
			if (edit == hwnd || !edit) break;
			SetWindowPos(edit, NULL, 0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), SWP_NOMOVE);

			break;
		}
		case WM_NOTIFY:{
			LPNMHDR nmhdr = (LPNMHDR)lParam;
			switch (nmhdr->code) {
				case EN_REQUESTRESIZE:{
					REQRESIZE* rqs = (REQRESIZE*)lParam;

					HWND edit = ChildWindowFromPoint(hwnd, {0, 0});
					PostMessage(edit, EM_SETEVENTMASK, 0, NULL);

					RECT clientrc = getclientrect(hwnd);
					RECT wndrc = getwindowrect(hwnd);

					SetWindowPos(hwnd, NULL, 0, 0, rqs->rc.right + (wndrc.right - wndrc.left - clientrc.right) + 10, rqs->rc.bottom + (wndrc.bottom - wndrc.top - clientrc.bottom) + 10, SWP_NOMOVE);
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}