#ifndef image_header_guard
#define image_header_guard

#include <string>
#include <windows.h>
#include <gdiplus.h>

#pragma comment(lib, "Gdiplus.lib")

//http://msdn.microsoft.com/en-us/library/dd183402(v=vs.85).aspx
BYTE* windowcapture(HWND hWnd, int* width = 0, int* height = 0, bool skip_save = true, std::string filename = "default.bmp");
BYTE* windowcapture(HWND hWnd, std::string filename, int* width, int* height);
BYTE* windowcapture(HWND hWnd, std::string filename);
//http://www.runicsoft.com/bmp.cpp
//http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html#chapter5
BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile);
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height);
BYTE* ConvertRGBToBMPBuffer(BYTE* Buffer, int width, int height, long* newsize);
BYTE* bmp_to_array(std::string file, int &x, int &y, HWND capture = NULL);
void array_to_bmp(std::string file, BYTE* buffer, int x, int y);
bool LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

void bmp_to_gif(std::string file_directory);
void animate_gif(std::string file_directory, std::string output, int delay = 0, int loop_count = 0);

bool get_inventory_layout(BYTE *newbuf, int x, int y, std::string outfile = "inventory_processed.bmp");

#endif



///directory set
//convert to gif
//create animated gif