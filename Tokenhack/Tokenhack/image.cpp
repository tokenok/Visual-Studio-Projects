#include <iostream>
#include <Windows.h>
#include <gdiplus.h>

#include "image.h"
#include "common.h"

#pragma comment(lib, "Gdiplus.lib")

using namespace std;
using namespace Gdiplus;

#define GRAYSCALE_AVGERAGE				1
#define GRAYSCALE_LUMINOSITY			2
#define GRAYSCALE_DESATURATION			3
#define GRAYSCALE_DECOMPOSITION_MIN		4
#define GRAYSCALE_DECOMPOSITION_MAX		5
#define GRAYSCALE_COLOR_CHANNEL_RED		6
#define GRAYSCALE_COLOR_CHANNEL_GREEN	7
#define GRAYSCALE_COLOR_CHANNEL_BLUE	8
#define GRAYSCLAE_COLOR_COUNT			9
#define GRAYSCLAE_COLOR_COUNT_DITHER	10
#define GRAYSCALE_BINARY				11
#define GRAYSCALE_INVERSE_BINARY		12

#define BRIGHTNESS_FLAT					1
#define BRIGHTNESS_HSV					2

#define MIN3(x,y,z)  ((y) <= (z) ? ((x) <= (y) ? (x) : (y)) : ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? ((x) >= (y) ? (x) : (y)) : ((x) >= (z) ? (x) : (z)))

struct rgb_color {
	double r, g, b;
	rgb_color() {}
	rgb_color(double r, double g, double b):r(r / 255), g(g / 255), b(b / 255) {}
};

struct hsv_color {
	double hue;
	double sat;
	double val;
};

//http://msdn.microsoft.com/en-us/library/dd183402(v=vs.85).aspx
BYTE* windowcapture(HWND hWnd, int* width, int* height, bool skip_save, string filename) {
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	// Retrieve the handle to a display device context for the client 
	// area of the window. 
	hdcWindow = GetDC(hWnd);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC)
		goto done;

	// Get the client area for size calculation
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	//This is the best stretch mode
	SetStretchBltMode(hdcWindow, HALFTONE);

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	if (!hbmScreen)
		goto done;

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcWindow, 0, 0, SRCCOPY))
		goto done;

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 24) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	BYTE* lpbitmap = (BYTE*)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	*width = bmpScreen.bmWidth;	
	*height = bmpScreen.bmHeight;

	BYTE* ret = ConvertBMPToRGBBuffer(lpbitmap, *width, *height);

	//skip file save and return bmp data buffer
	if (skip_save) {
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		goto done;
	}

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFile(str_to_wstr(filename).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	//Clean up
done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(hWnd, hdcWindow);

	return ret;
}
BYTE* windowcapture(HWND hWnd, string filename, int* width, int* height) {
	return windowcapture(hWnd, &*width, &*height, false, filename);
}
//http://www.runicsoft.com/bmp.cpp
//http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html#chapter5
BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile) {
	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	// value to be used in ReadFile funcs
	DWORD bytesread;
	// open file to read from
	HANDLE file = CreateFile(bmpfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (NULL == file) {
		cout << "ERROR: could not open BMP file: " << wstr_to_str(bmpfile) << '\n';
		return NULL; // coudn't open file	
	}
	if (GetLastError() == ERROR_FILE_NOT_FOUND) {
		cout << "ERROR: could not open BMP file: " << wstr_to_str(bmpfile) << '\n';
		return NULL; // coudn't open file	
	}
	if (GetLastError() == ERROR_PATH_NOT_FOUND) {
		cout << "ERROR: path not found: " << wstr_to_str(bmpfile) << '\n';
		return NULL;
	}
	// read file header
	if (ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false) {
		cout << "ERROR: could not read bmp file header: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	//read bitmap info

	if (ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false) {
		cout << "ERROR: could not read bmp file info: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	// check if file is actually a bmp
	if (bmpheader.bfType != 'MB') {
		cout << "ERROR: file is not a bmp: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	// get image measurements
	*width = bmpinfo.biWidth;
	*height = abs(bmpinfo.biHeight);

	// check if bmp is uncompressed
	if (bmpinfo.biCompression != BI_RGB) {
		CloseHandle(file);
		return NULL;
	}

	// check if we have 24 bit bmp
	if (bmpinfo.biBitCount != 24) {
		cout << "ERROR: file is does not have 24 bits per pixel: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	// create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;
	BYTE* Buffer = new BYTE[*size];
	// move file pointer to start of bitmap data
	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
	// read bmp data
	if (ReadFile(file, Buffer, *size, &bytesread, NULL) == false) {
		delete[] Buffer;
		CloseHandle(file);
		return NULL;
	}

	// everything successful here: close file and return buffer

	CloseHandle(file);

	return Buffer;
}
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile) {
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	// andinitialize them to zero
	memset(&bmfh, 0, sizeof (BITMAPFILEHEADER));
	memset(&info, 0, sizeof (BITMAPINFOHEADER));

	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == NULL) {
		cout << "ERROR: could not create bmp file: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	if (GetLastError() == ERROR_INVALID_NAME) {
		cout << "ERROR: invalid name: " << wstr_to_str(bmpfile) << '\n';
		return false;
	}
	// write file header
	unsigned long bwritten;
	if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false) {
		cout << "ERROR: could not write bmp file header: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	// write infoheader
	if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false) {
		cout << "ERROR: could not write bmp file infoheader: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	// write image data
	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false) {
		cout << "ERROR: could not write bmp file image data: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	// and close file
	CloseHandle(file);

	return true;
}
BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height) {
	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf = new BYTE[width * height * 3];

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int y = 0; y<height; y++) {
		for (int x = 0; x<3 * width; x += 3) {
			newpos = y * 3 * width + x;
			bufpos = (height - y - 1)*psw + x;

			newbuf[newpos] = Buffer[bufpos + 2];
			newbuf[newpos + 1] = Buffer[bufpos + 1];
			newbuf[newpos + 2] = Buffer[bufpos];
		}
	}
	return newbuf;
}
BYTE* ConvertRGBToBMPBuffer(BYTE* Buffer, int width, int height, long* newsize) {

	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// we can already store the size of the new padded buffer
	*newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[*newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, *newsize);

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < 3 * width; x += 3) {
			bufpos = y * 3 * width + x;     // position in original buffer
			newpos = (height - y - 1) * psw + x;           // position in padded buffer

			newbuf[newpos + 0] = Buffer[bufpos + 2];// swap r and b
			newbuf[newpos + 1] = Buffer[bufpos + 1];// g stays
			newbuf[newpos + 2] = Buffer[bufpos + 0];// swap b and r
		}
	}
	return newbuf;
}
BYTE* bmp_to_array(string file, int &x, int &y, HWND capture) {
	if (capture)
		windowcapture(capture, file);
	long s;
	BYTE* a = LoadBMP(&x, &y, &s, str_to_wstr(file).c_str());
	BYTE* newbuf = ConvertBMPToRGBBuffer(a, x, y);
	delete[] a;
	return newbuf;
}
void array_to_bmp(string file, BYTE* buffer, int x, int y) {
	long s;
	BYTE* b = ConvertRGBToBMPBuffer(buffer, x, y, &s);
	SaveBMP(b, x, y, s, (str_to_wstr(file).c_str()));
	delete[] b;
}
bool LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC) {
	// Load the bitmap image file
	HBITMAP hBitmap;
	hBitmap = (HBITMAP)::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE);
	// Verify that the image was loaded
	if (hBitmap == NULL) {
		::MessageBox(NULL, TEXT("LoadImage Failed"), TEXT("Error"), MB_OK);
		return false;
	}

	// Create a device context that is compatible with the window
	HDC hLocalDC;
	hLocalDC = ::CreateCompatibleDC(hWinDC);
	// Verify that the device context was created
	if (hLocalDC == NULL) {
		::MessageBox(NULL, TEXT("CreateCompatibleDC Failed"), TEXT("Error"), MB_OK);
		return false;
	}

	// Get the bitmap's parameters and verify the get
	BITMAP qBitmap;
	int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),
		reinterpret_cast<LPVOID>(&qBitmap));
	if (!iReturn) {
		::MessageBox(NULL, TEXT("GetObject Failed"), TEXT("Error"), MB_OK);
		return false;
	}

	// Select the loaded bitmap into the device context
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hLocalDC, hBitmap);
	if (hOldBmp == NULL) {
		::MessageBox(NULL, TEXT("SelectObject Failed"), TEXT("Error"), MB_OK);
		return false;
	}

	// Blit the dc which holds the bitmap onto the window's dc
	BOOL qRetBlit = ::BitBlt(hWinDC, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight,
		hLocalDC, 0, 0, SRCCOPY);
	if (!qRetBlit) {
		::MessageBox(NULL, TEXT("Blit Failed"), TEXT("Error"), MB_OK);
		return false;
	}

	// Unitialize and deallocate resources
	::SelectObject(hLocalDC, hOldBmp);
	::DeleteDC(hLocalDC);
	::DeleteObject(hBitmap);
	return true;
}

hsv_color RGB_to_HSV(rgb_color RGB) {
	hsv_color HSV;

	double R = RGB.r;
	double G = RGB.g;
	double B = RGB.b;

	double M = MAX3(R, G, B);
	double m = MIN3(R, G, B);
	double C = M - m;

	double H = 0;
	double S = 0;
	double V = 0;

	//calculate value
	V = M;
	//calculate hue
	if (M == R) {
		H = ((G - B) / C) * 60;
		if (H < 0)
			H += 360;
	}
	else if (M == G)
		H = (((B - R) / C) + 2) * 60;
	else if (M == B)
		H = (((R - G) / C) + 4) * 60;
	//calculate saturation
	if (V == 0)
		S = 0;
	else
		S = (C / M);

	HSV.hue = H;
	HSV.sat = S;
	HSV.val = V;
	return HSV;
}
rgb_color HSV_to_RGB(hsv_color HSV) {
	rgb_color RGB;

	double H = HSV.hue;
	double S = HSV.sat;
	double V = HSV.val;

	double C = V * S * 255;
	double X = C * (1 - abs(fmod((H / 60), 2) - 1));
	double m = V * 255 - C;

	double R = 0;
	double G = 0;
	double B = 0;

	int c = 0;
	for (; H >= 0; H -= 60, c++);

	R = (c == 1 || c == 6) ? C + m : (c == 2 || c == 5) ? X + m : (c == 3 || c == 4) ? m : 0;
	G = (c == 2 || c == 3) ? C + m : (c == 1 || c == 4) ? X + m : (c == 5 || c == 6) ? m : 0;
	B = (c == 4 || c == 5) ? C + m : (c == 3 || c == 6) ? X + m : (c == 1 || c == 2) ? m : 0;

	RGB.r = R;
	RGB.g = G;
	RGB.b = B;
	return RGB;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

vector<process_image*> process_images;

process_image::process_image() {
	buffer = NULL;
	x = y = NULL;
	filename = directory = algs = "";
}

process_image::process_image(const process_image& other) {
	buffer = other.buffer;
	x = other.x;
	y = other.y;
	filename = other.filename;
	directory = other.directory;
	algs = other.algs;
};

process_image& process_image::operator=(const process_image& other) {
	if (this == &other)
		return *this;
	buffer = other.buffer;
	x = other.x;
	y = other.y;
	filename = other.filename;
	directory = other.directory;
	algs = other.algs;
	return *this;
}

process_image::~process_image() {
	delete[] buffer;
}

void process_image::load_bmp(string file) {
	directory = file.substr(0, file.rfind('\\') + 1);
	filename = file.substr(file.rfind('\\') + 1, file.size());
	long s;
	BYTE* a = LoadBMP(&x, &y, &s, str_to_wstr(file).c_str());
	buffer = ConvertBMPToRGBBuffer(a, x, y);
	delete[] a;
}

void bmp_to_gif(string file_directory, string filename, string output_directory, string output_filename/* = ""*/) {
	if (file_directory.size() > 0 && file_directory[file_directory.size() - 1] != '\\')
		file_directory += '\\';
	if (output_directory.size() > 0 && output_directory[output_directory.size() - 1] != '\\')
		output_directory += '\\';
	if (output_filename == "")
		output_filename = filename;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID encoderClsid;
	GetEncoderClsid(L"image/gif", &encoderClsid);

	Gdiplus::Image* image = new Gdiplus::Image(str_to_wstr(file_directory + filename).c_str());

	string filenamenotype = output_filename.substr(output_filename.rfind('\\') + 1, output_filename.rfind('.') - output_filename.rfind('\\') - 1);

	Gdiplus::Status stat = image->Save(str_to_wstr(output_directory + filenamenotype + ".gif").c_str(), &encoderClsid, NULL);
	if (stat == Gdiplus::Ok)
		cout << filenamenotype + ".gif" << " saved successfully\n";
	else
		printf("Failure: stat = %d\n", stat);

	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);

	////image/bmp
	////image/jpeg
	////image/gif
	////image/tiff
	////image/png	
}
void animate_gif(string file_directory, string output_directory, string output_filename, int delay/* = 0*/, int loop_count/* = 0*/) {
	if (file_directory.size() > 0 && file_directory[file_directory.size() - 1] != '\\')
		file_directory += '\\';
	if (output_directory.size() > 0 && output_directory[output_directory.size() - 1] != '\\')
		output_directory += '\\';
	string cmdline = "";

	vector<string> files;
	GetFilesInDirectory(files, file_directory, "gif");
	if (files.size() == 0) {
		cout << "no files found" << '\n';
		return;
	}

	cmdline += "cd " + file_directory + " & convert -delay " + int_to_str(delay);

	for (UINT i = 0; i < files.size(); i++) {
		string filename = files[i].substr(files[i].rfind('\\') + 1, files[i].size());
		cmdline += " -page +0+0 " + filename;
	}

	cmdline += " -loop " + int_to_str(loop_count) + " \"" + output_directory + output_filename + "\"";
	system(cmdline.c_str());
}

void grayscale(BYTE *newbuf, int x, int y, UINT gray_algorithm, int extra, string outfile/* = ""*/) {
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			int red = newbuf[pos];
			int green = newbuf[pos + 1];
			int blue = newbuf[pos + 2];

			BYTE gray = 255;

			if (gray_algorithm == GRAYSCALE_AVGERAGE) {
				gray = (red + green + blue) / 3;
			}

			else if (gray_algorithm == GRAYSCALE_LUMINOSITY) {
				gray = (BYTE)(((double)red * 0.2125) + ((double)green * 0.7154) + ((double)blue * 0.0721));
			}

			else if (gray_algorithm == GRAYSCALE_DESATURATION) {
				gray = (MAX3(red, green, blue) + MIN3(red, green, blue)) / 2;
			}

			else if (gray_algorithm == GRAYSCALE_DECOMPOSITION_MIN) {
				gray = MIN3(red, green, blue);
			}
			else if (gray_algorithm == GRAYSCALE_DECOMPOSITION_MAX) {
				gray = MAX3(red, green, blue);
			}

			else if (gray_algorithm == GRAYSCALE_COLOR_CHANNEL_RED) {
				gray = red;
			}
			else if (gray_algorithm == GRAYSCALE_COLOR_CHANNEL_GREEN) {
				gray = green;
			}
			else if (gray_algorithm == GRAYSCALE_COLOR_CHANNEL_BLUE) {
				gray = blue;
			}

			else if (gray_algorithm == GRAYSCLAE_COLOR_COUNT) {
				if (extra < 2)
					extra = 2;
				BYTE ConversionFactor = 255 / (extra - 1);
				BYTE AverageValue = (red + green + blue) / 3;
				gray = (BYTE)((AverageValue / ConversionFactor) + 0.5) * ConversionFactor;
			}

			else if (gray_algorithm == GRAYSCLAE_COLOR_COUNT_DITHER) {

			}

			if (gray_algorithm == GRAYSCALE_BINARY) {
				gray = (((red + green + blue) / 3 > extra) ? 255 : 0);
			}
			else if (gray_algorithm == GRAYSCALE_INVERSE_BINARY) {
				gray = ((red + green + blue) / 3 > extra) ? 0 : 255;
			}


			newbuf[pos + 0] = gray;
			newbuf[pos + 1] = gray;
			newbuf[pos + 2] = gray;
		}
	}
	if (outfile.size() > 0)
		array_to_bmp(outfile, newbuf, x, y);
}
BYTE* cropimage(BYTE *buf, int &x, int &y, int startx, int stopx, int starty, int stopy, string outfile/* = ""*/) {
	BYTE* newbuf = new BYTE[((x - (startx + stopx)) * ((y - (starty + stopy)) * 3))];
	int newpos = 0;
	for (int i = starty; i < y - stopy; i++) {
		for (int j = startx * 3; j < 3 * (x - stopx); j += 3) {
			int pos = i * 3 * x + j;
			int red = pos;
			int green = pos + 1;
			int blue = pos + 2;
			newbuf[newpos + 0] = buf[red];
			newbuf[newpos + 1] = buf[green];
			newbuf[newpos + 2] = buf[blue];
			newpos += 3;
		}
	}
	if (outfile.size() > 0)
		array_to_bmp(outfile, newbuf, x - (startx + stopx), y - (starty + stopy));
	x -= (startx + stopx);
	y -= (starty + stopy);
	return newbuf;
}
BYTE* growimage(BYTE *buf, int &x, int &y, int startx, int stopx, int starty, int stopy, COLORREF background, string outfile/* = ""*/) {
	x = x + startx + stopx;
	y = y + starty + stopy;
	BYTE* newbuf = new BYTE[x * y * 3];

	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			newbuf[pos + 0] = GetRValue(background);
			newbuf[pos + 1] = GetGValue(background);
			newbuf[pos + 2] = GetBValue(background);
		}
	}

	int newpos = 0;
	for (int i = starty; i < y - stopy; i++) {
		for (int j = startx * 3; j < 3 * (x - stopx); j += 3) {
			int pos = i * 3 * x + j;
			int red = pos;
			int green = pos + 1;
			int blue = pos + 2;
			newbuf[red] = buf[newpos];
			newbuf[green] = buf[newpos + 1];
			newbuf[blue] = buf[newpos + 2];
			newpos += 3;
		}
	}
	if (outfile.size() > 0)
		array_to_bmp(outfile, newbuf, x, y);
	return newbuf;
}
void brightness(BYTE *newbuf, int x, int y, UINT brightness_algorithm, double val, string outfile/* = ""*/) {
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			int red = newbuf[pos];
			int green = newbuf[pos + 1];
			int blue = newbuf[pos + 2];

			if (brightness_algorithm == BRIGHTNESS_FLAT) {
				newbuf[pos + 0] = (BYTE)(red + val > 255 ? 255 : red + val < 0 ? 0 : red + val);
				newbuf[pos + 1] = (BYTE)(green + val > 255 ? 255 : green + val < 0 ? 0 : green + val);
				newbuf[pos + 2] = (BYTE)(blue + val > 255 ? 255 : blue + val < 0 ? 0 : blue + val);
			}
			else if (brightness_algorithm == BRIGHTNESS_HSV) {
				hsv_color hsv = RGB_to_HSV(rgb_color(red, green, blue));
				hsv.val += val / 100 > 1 ? 1 : val / 100 < 0 ? 0 : val / 100;
				rgb_color rgb = HSV_to_RGB(hsv);
				newbuf[pos + 0] = (BYTE)rgb.r;
				newbuf[pos + 1] = (BYTE)rgb.g;
				newbuf[pos + 2] = (BYTE)rgb.b;
			}
		}
	}
	if (outfile.size() > 0)
		array_to_bmp(outfile, newbuf, x, y);
}
BYTE* flatcoloradjust(BYTE *buf, int x, int y, INT red, INT green, INT blue, string outfile/* = ""*/) {
	BYTE* newbuf = new BYTE[x * y * 3];
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			
			newbuf[pos + 0] = (BYTE)(buf[pos + 0] + red > 255 ? 255 : buf[pos + 0] + red < 0 ? 0 : buf[pos + 0] + red);
			newbuf[pos + 1] = (BYTE)(buf[pos + 1] + green > 255 ? 255 : buf[pos + 1] + green < 0 ? 0 : buf[pos + 1] + green);
			newbuf[pos + 2] = (BYTE)(buf[pos + 2] + blue > 255 ? 255 : buf[pos + 2] + blue < 0 ? 0 : buf[pos + 2] + blue);
		}
	}
	if (outfile.size() > 0)
		array_to_bmp(outfile, buf, x, y);
	return newbuf;
}

//not in use
#define M_PI       3.14159265358979323846
void colorText(string str, double phase) {
	if (phase == NULL) phase = 0;
	double center = 128;
	double width = 127;
	double frequency = M_PI * 2 / str.length();
	for (UINT i = 0; i < str.length(); ++i) {
		double red = sin(frequency * i + 2 + phase) * width + center;
		double green = sin(frequency * i + 0 + phase) * width + center;
		double blue = sin(frequency * i + 4 + phase) * width + center;
		cout << (int)(BYTE)red << " " << (int)(BYTE)green << " " << (int)(BYTE)blue << '\n';
	}
}

///directory set
//convert to gif
//create animated gif