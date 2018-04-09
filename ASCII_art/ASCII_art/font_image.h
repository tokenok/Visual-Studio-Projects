#include <Windows.h>
#include <vector>
#include <string>

struct FontImage {
	BYTE* buf;
	int w, h;

	wchar_t c;
};

std::vector<FontImage> GetWindowFontGlyphsAsArray(HWND hwnd);
