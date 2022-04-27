#pragma once

#define NOMINMAX
#undef min
#undef max

#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <map>

//restores old val to var when ScopedVar goes out of scope
template<class T>
class ScopedVar {
	T* var;
	T oldval;
public:
	ScopedVar(T& v, const T& newval) {
		var = &v;
		oldval = v;
		v = newval;
	}
	~ScopedVar() {
		*var = oldval;
	}
};

template<class T>
struct Range {
	T min, max;

	Range() {}
	Range(T min, T max) {
		this->min = std::min(min, max);
		this->max = std::max(min, max);
	}

	inline T MapTo(const T& value, const Range& to) const {
		return Map(value, *this, to);
	}

	inline T MapFrom(const T& value, const Range& from) const {
		return Map(value, from, *this);
	}

	static inline T Map(const T& value, const Range& from, const Range& to) {
		T val = value;
		if (value < from.min) val = from.min;
		else if (from.max < value) val = from.max;
		return (((val - from.min) * (to.max - to.min)) / (from.max - from.min)) + to.min;
	}
};

template<class A, class B, class C> 
struct trio {
	A first;
	B mid;
	C last;
	trio(A a, B b, C c) : first(a), mid(b), last(c) {}
};

template<class T, class C>
inline trio<T, T, T> Sort3(T a, T b, T c, C compare) {
	if (compare(b, a)) 
		std::swap(b, a);
	if (compare(c, b)) {
		std::swap(c, b);
		if (compare(b, a)) 
			std::swap(b, a);
	}
	return trio<T, T, T>(a, b, c);
}
template<class T>
inline trio<T, T, T> Sort3(T a, T b, T c) {
	return Sort3(a, b, c, std::less<>());
}

template<class T, class C>
bool sorted(T _First, T _Last, C compare) {
	for (T _Pos = _First + 1; _Pos != _Last; ++_Pos) {
		T _Prev = _Pos - 1;
		if (compare(*_Pos, *_Prev))
			return false;
	}
	return true;
}

inline std::vector<std::wstring> split_wstr(std::wstring str, const std::wstring& delimiter, int minlen = -1) {
	std::vector<std::wstring> ret;
	size_t pos = 0;
	std::wstring token;
	while ((pos = str.find(delimiter)) != std::wstring::npos) {
		token = str.substr(0, pos);
		if ((int)token.size() >= minlen)
			ret.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	if ((int)str.size() > 0)
		ret.push_back(str);
	return ret;
}

#define STRINGIFY_ENUM_DEC(e) extern std::map<e, std::wstring> e##Names;
#define STRINGIFY_ENUM(e, ...) std::map<e, std::wstring> e##Names; \
namespace { \
	bool e##SetEnumNames() { \
		std::vector<std::wstring> names = split_wstr(L#__VA_ARGS__, L","); \
		std::vector<e> vals = {__VA_ARGS__}; \
		for (unsigned i = 0; i < vals.size(); i++) \
			e##Names[vals[i]] = names[i]; \
		return true; \
	} \
	bool e##bbbb = e##SetEnumNames(); \
} \

//hue[0 - 360] saturation[0.0 - 1.0] value[0.0 - 1.0]
inline COLORREF HSVtoRGB(double h, double s, double v) {
	int i;
	double f, p, q, t, r, g, b;
	if (s == 0) {
		// achromatic (grey)
		r = g = b = v;
		return RGB(r * 255.0, g * 255.0, b * 255.0);
	}
	h /= 60;			// sector 0 to 5
	i = (int)floor(h);
	f = h - i;			// factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
	return RGB(r * 255.0, g * 255.0, b * 255.0);
}

inline unsigned int uint_diff(unsigned a, unsigned b) {
	return a < b ? b - a : a - b;
}

inline POINT getclientcursorpos(HWND hwnd) {
	RECT temprect;
	GetClientRect(hwnd, &temprect);
	MapWindowPoints(hwnd, NULL, (LPPOINT)&temprect, 2);
	POINT temppoint;
	GetCursorPos(&temppoint);
	POINT client_cursor_pos;
	client_cursor_pos.x = temppoint.x - temprect.left;
	client_cursor_pos.y = temppoint.y - temprect.top;
	return client_cursor_pos;
}

inline std::string getexedir() {
	CHAR path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string dir(path);
	return dir.substr(0, dir.rfind('\\'));
}
inline void GetFilesInDirectory(const std::string &directory, const std::string &filetype, std::vector<std::string> *out) {
	using namespace std;
	HANDLE dir;
	WIN32_FIND_DATAA file_data;

	string path = (directory + (directory.back() != '\\' ? "\\*." : "") + filetype);
	if ((dir = FindFirstFileA(path.c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = file_data.cFileName;
		const string full_file_name = directory + "\\" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out->push_back(full_file_name);
	} while (FindNextFileA(dir, &file_data));

	FindClose(dir);
}
inline void SaveSourceToFile(std::string dir) {
	std::vector<std::string> files;
	GetFilesInDirectory(dir, "h", &files);
	GetFilesInDirectory(dir, "cpp", &files);
	GetFilesInDirectory(dir, "rc", &files);

	std::string text = "";
	for (unsigned i = 0; i < files.size(); i++) {
		std::ifstream file;
		file.open(files[i]);
		if (file.is_open()) {
			text += "~-----------------------------------------------~\n" + files[i] + "\n~-----------------------------------------------~\n";

			while (file.good()) {
				std::string line;
				std::getline(file, line);

				text += line + "\n";
			}

		//	text.erase(text.end() - 1);

			file.close();
		}
	}

	std::ofstream file;
	file.open("output.txt");

	if (file.is_open()) {
		file << text;
	}

	file.close();
}
inline void LoadSourceFromFile(std::string dir) {
	std::map<std::wstring, std::wstring> files;

	std::wifstream infile;
	infile.open(dir + "\\input.txt");
	if (infile.is_open()) {
		std::wstring line;
	//	std::getline(infile, line);//eat first empty line
		std::getline(infile, line);//eat line of ~----...~
		while (infile.good()) {
			std::getline(infile, line);
			std::wstring filename = line.substr(line.rfind(L"\\") + 1, line.size());
			std::getline(infile, line);//eat next line of ~----------------...~

			std::wstring filetext = L"";
			while (infile.good()) {
				std::getline(infile, line);
				if (line == L"~-----------------------------------------------~") {
					break;
				}
				filetext += line + L"\n";
			}

			files[filename] = filetext;
		}
	}
	infile.close();

	for (auto & file : files) {
		std::wofstream outfile;
		outfile.open(L"sourceout\\" + file.first);
		if (outfile.is_open()) {
			outfile << file.second;
			outfile.close();
		}
	}
}

#pragma region getmessagetext

#define X(x) { x, #x },
struct WindowsMessage {
	int msgid;
	const char* pname;
} static WindowsMessages[] =
{
	X(WM_NULL)
	X(WM_CREATE)
	X(WM_DESTROY)
	X(WM_MOVE)
	X(WM_SIZE)
	X(WM_ACTIVATE)
	X(WM_SETFOCUS)
	X(WM_KILLFOCUS)
	X(WM_ENABLE)
	X(WM_SETREDRAW)
	X(WM_SETTEXT)
	X(WM_GETTEXT)
	X(WM_GETTEXTLENGTH)
	X(WM_PAINT)
	X(WM_CLOSE)
	X(WM_QUERYENDSESSION)
	X(WM_QUERYOPEN)
	X(WM_ENDSESSION)
	X(WM_QUIT)
	X(WM_ERASEBKGND)
	X(WM_SYSCOLORCHANGE)
	X(WM_SHOWWINDOW)
	X(WM_WININICHANGE)
	X(WM_DEVMODECHANGE)
	X(WM_ACTIVATEAPP)
	X(WM_FONTCHANGE)
	X(WM_TIMECHANGE)
	X(WM_CANCELMODE)
	X(WM_SETCURSOR)
	X(WM_MOUSEACTIVATE)
	X(WM_CHILDACTIVATE)
	X(WM_QUEUESYNC)
	X(WM_GETMINMAXINFO)
	X(WM_PAINTICON)
	X(WM_ICONERASEBKGND)
	X(WM_NEXTDLGCTL)
	X(WM_SPOOLERSTATUS)
	X(WM_DRAWITEM)
	X(WM_MEASUREITEM)
	X(WM_DELETEITEM)
	X(WM_VKEYTOITEM)
	X(WM_CHARTOITEM)
	X(WM_SETFONT)
	X(WM_GETFONT)
	X(WM_SETHOTKEY)
	X(WM_GETHOTKEY)
	X(WM_QUERYDRAGICON)
	X(WM_COMPAREITEM)
	X(WM_GETOBJECT)
	X(WM_COMPACTING)
	X(WM_COMMNOTIFY)
	X(WM_WINDOWPOSCHANGING)
	X(WM_WINDOWPOSCHANGED)
	X(WM_POWER)
	X(WM_COPYDATA)
	X(WM_CANCELJOURNAL)
	X(WM_NOTIFY)
	X(WM_INPUTLANGCHANGEREQUEST)
	X(WM_INPUTLANGCHANGE)
	X(WM_TCARD)
	X(WM_HELP)
	X(WM_USERCHANGED)
	X(WM_NOTIFYFORMAT)
	X(WM_CONTEXTMENU)
	X(WM_STYLECHANGING)
	X(WM_STYLECHANGED)
	X(WM_DISPLAYCHANGE)
	X(WM_GETICON)
	X(WM_SETICON)
	X(WM_NCCREATE)
	X(WM_NCDESTROY)
	X(WM_NCCALCSIZE)
	X(WM_NCHITTEST)
	X(WM_NCPAINT)
	X(WM_NCACTIVATE)
	X(WM_GETDLGCODE)
	X(WM_SYNCPAINT)
	X(WM_NCMOUSEMOVE)
	X(WM_NCLBUTTONDOWN)
	X(WM_NCLBUTTONUP)
	X(WM_NCLBUTTONDBLCLK)
	X(WM_NCRBUTTONDOWN)
	X(WM_NCRBUTTONUP)
	X(WM_NCRBUTTONDBLCLK)
	X(WM_NCMBUTTONDOWN)
	X(WM_NCMBUTTONUP)
	X(WM_NCMBUTTONDBLCLK)
	X(WM_NCXBUTTONDOWN)
	X(WM_NCXBUTTONUP)
	X(WM_NCXBUTTONDBLCLK)
	X(WM_INPUT_DEVICE_CHANGE)
	X(WM_INPUT)
	X(WM_KEYDOWN)
	X(WM_KEYUP)
	X(WM_CHAR)
	X(WM_DEADCHAR)
	X(WM_SYSKEYDOWN)
	X(WM_SYSKEYUP)
	X(WM_SYSCHAR)
	X(WM_SYSDEADCHAR)
	X(WM_UNICHAR)
	X(WM_KEYLAST)
	X(WM_KEYLAST)
	X(WM_IME_STARTCOMPOSITION)
	X(WM_IME_ENDCOMPOSITION)
	X(WM_IME_COMPOSITION)
	X(WM_IME_KEYLAST)
	X(WM_INITDIALOG)
	X(WM_COMMAND)
	X(WM_SYSCOMMAND)
	X(WM_TIMER)
	X(WM_HSCROLL)
	X(WM_VSCROLL)
	X(WM_INITMENU)
	X(WM_INITMENUPOPUP)
	X(WM_GESTURE)
	X(WM_GESTURENOTIFY)
	X(WM_MENUSELECT)
	X(WM_MENUCHAR)
	X(WM_ENTERIDLE)
	X(WM_MENURBUTTONUP)
	X(WM_MENUDRAG)
	X(WM_MENUGETOBJECT)
	X(WM_UNINITMENUPOPUP)
	X(WM_MENUCOMMAND)
	X(WM_CHANGEUISTATE)
	X(WM_UPDATEUISTATE)
	X(WM_QUERYUISTATE)
	X(WM_CTLCOLORMSGBOX)
	X(WM_CTLCOLOREDIT)
	X(WM_CTLCOLORLISTBOX)
	X(WM_CTLCOLORBTN)
	X(WM_CTLCOLORDLG)
	X(WM_CTLCOLORSCROLLBAR)
	X(WM_CTLCOLORSTATIC)
	X(WM_MOUSEMOVE)
	X(WM_LBUTTONDOWN)
	X(WM_LBUTTONUP)
	X(WM_LBUTTONDBLCLK)
	X(WM_RBUTTONDOWN)
	X(WM_RBUTTONUP)
	X(WM_RBUTTONDBLCLK)
	X(WM_MBUTTONDOWN)
	X(WM_MBUTTONUP)
	X(WM_MBUTTONDBLCLK)
	X(WM_MOUSEWHEEL)
	X(WM_XBUTTONDOWN)
	X(WM_XBUTTONUP)
	X(WM_XBUTTONDBLCLK)
	X(WM_MOUSEHWHEEL)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_PARENTNOTIFY)
	X(WM_ENTERMENULOOP)
	X(WM_EXITMENULOOP)
	X(WM_NEXTMENU)
	X(WM_SIZING)
	X(WM_CAPTURECHANGED)
	X(WM_MOVING)
	X(WM_POWERBROADCAST)
	X(WM_MDICREATE)
	X(WM_MDIDESTROY)
	X(WM_MDIACTIVATE)
	X(WM_MDIRESTORE)
	X(WM_MDINEXT)
	X(WM_MDIMAXIMIZE)
	X(WM_MDITILE)
	X(WM_MDICASCADE)
	X(WM_MDIICONARRANGE)
	X(WM_MDIGETACTIVE)
	X(WM_MDISETMENU)
	X(WM_ENTERSIZEMOVE)
	X(WM_EXITSIZEMOVE)
	X(WM_DROPFILES)
	X(WM_MDIREFRESHMENU)
	X(WM_POINTERDEVICECHANGE)
	X(WM_POINTERDEVICEINRANGE)
	X(WM_POINTERDEVICEOUTOFRANGE)
	X(WM_TOUCH)
	X(WM_NCPOINTERUPDATE)
	X(WM_NCPOINTERDOWN)
	X(WM_NCPOINTERUP)
	X(WM_POINTERUPDATE)
	X(WM_POINTERDOWN)
	X(WM_POINTERUP)
	X(WM_POINTERENTER)
	X(WM_POINTERLEAVE)
	X(WM_POINTERACTIVATE)
	X(WM_POINTERCAPTURECHANGED)
	X(WM_TOUCHHITTESTING)
	X(WM_POINTERWHEEL)
	X(WM_POINTERHWHEEL)
	X(WM_IME_SETCONTEXT)
	X(WM_IME_NOTIFY)
	X(WM_IME_CONTROL)
	X(WM_IME_COMPOSITIONFULL)
	X(WM_IME_SELECT)
	X(WM_IME_CHAR)
	X(WM_IME_REQUEST)
	X(WM_IME_KEYDOWN)
	X(WM_IME_KEYUP)
	X(WM_MOUSEHOVER)
	X(WM_MOUSELEAVE)
	X(WM_NCMOUSEHOVER)
	X(WM_NCMOUSELEAVE)
	X(WM_WTSSESSION_CHANGE)
	X(WM_TABLET_FIRST)
	X(WM_TABLET_LAST)
	X(WM_DPICHANGED)
	X(WM_CUT)
	X(WM_COPY)
	X(WM_PASTE)
	X(WM_CLEAR)
	X(WM_UNDO)
	X(WM_RENDERFORMAT)
	X(WM_RENDERALLFORMATS)
	X(WM_DESTROYCLIPBOARD)
	X(WM_DRAWCLIPBOARD)
	X(WM_PAINTCLIPBOARD)
	X(WM_VSCROLLCLIPBOARD)
	X(WM_SIZECLIPBOARD)
	X(WM_ASKCBFORMATNAME)
	X(WM_CHANGECBCHAIN)
	X(WM_HSCROLLCLIPBOARD)
	X(WM_QUERYNEWPALETTE)
	X(WM_PALETTEISCHANGING)
	X(WM_PALETTECHANGED)
	X(WM_HOTKEY)
	X(WM_PRINT)
	X(WM_PRINTCLIENT)
	X(WM_APPCOMMAND)
	X(WM_THEMECHANGED)
	X(WM_CLIPBOARDUPDATE)
	X(WM_DWMCOMPOSITIONCHANGED)
	X(WM_DWMNCRENDERINGCHANGED)
	X(WM_DWMCOLORIZATIONCOLORCHANGED)
	X(WM_DWMWINDOWMAXIMIZEDCHANGE)
	X(WM_DWMSENDICONICTHUMBNAIL)
	X(WM_DWMSENDICONICLIVEPREVIEWBITMAP)
	X(WM_GETTITLEBARINFOEX)
	X(WM_HANDHELDFIRST)
	X(WM_HANDHELDLAST)
	X(WM_AFXFIRST)
	X(WM_AFXLAST)
	X(WM_PENWINFIRST)
	X(WM_PENWINLAST)
	X(WM_APP)
	X(WM_USER)
};
#undef X

inline std::string getMessageText(int msgId) {
	int size = (sizeof(WindowsMessages) / sizeof(*WindowsMessages));
	for (int i = 0; i < size; i++) {
		if (msgId == WindowsMessages[i].msgid)
			return WindowsMessages[i].pname;
	}
	return "";
}

#pragma endregion


























