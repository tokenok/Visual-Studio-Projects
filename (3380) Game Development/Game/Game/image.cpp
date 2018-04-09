//#include "image.h"

//#include <iostream>
//#include "C:\CPPlibs\common\common.h"
//
//using namespace std;
//
////http://www.runicsoft.com/bmp.cpp
////http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html#chapter5
//BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile) {
//	// declare bitmap structures
//	BITMAPFILEHEADER bmpheader;
//	BITMAPINFOHEADER bmpinfo;
//	// value to be used in ReadFile funcs
//	DWORD bytesread;
//	// open file to read from
//	HANDLE file = CreateFile(bmpfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
//	if (NULL == file) {
//		cout << "ERROR: could not open BMP file: " << wstr_to_str(bmpfile) << '\n';
//		return NULL; // coudn't open file	
//	}
//	if (GetLastError() == ERROR_FILE_NOT_FOUND) {
//		cout << "ERROR: could not open BMP file: " << wstr_to_str(bmpfile) << '\n';
//		return NULL; // coudn't open file	
//	}
//	if (GetLastError() == ERROR_PATH_NOT_FOUND) {
//		cout << "ERROR: path not found: " << wstr_to_str(bmpfile) << '\n';
//		return NULL;
//	}
//	// read file header
//	if (ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false) {
//		cout << "ERROR: could not read bmp file header: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return NULL;
//	}
//
//	//read bitmap info
//
//	if (ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false) {
//		cout << "ERROR: could not read bmp file info: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return NULL;
//	}
//
//	// check if file is actually a bmp
//	if (bmpheader.bfType != 'MB') {
//		cout << "ERROR: file is not a bmp: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return NULL;
//	}
//
//	// get image measurements
//	*width = bmpinfo.biWidth;
//	*height = abs(bmpinfo.biHeight);
//
//	// check if bmp is uncompressed
//	if (bmpinfo.biCompression != BI_RGB) {
//		CloseHandle(file);
//		return NULL;
//	}
//
//	// check if we have 24 bit bmp
//	if (bmpinfo.biBitCount != 24) {
//		cout << "ERROR: file is does not have 24 bits per pixel: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return NULL;
//	}
//
//	// create buffer to hold the data
//	*size = bmpheader.bfSize - bmpheader.bfOffBits;
//	BYTE* Buffer = new BYTE[*size];
//	// move file pointer to start of bitmap data
//	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
//	// read bmp data
//	if (ReadFile(file, Buffer, *size, &bytesread, NULL) == false) {
//		delete[] Buffer;
//		CloseHandle(file);
//		return NULL;
//	}
//
//	// everything successful here: close file and return buffer
//
//	CloseHandle(file);
//
//	return Buffer;
//}
//bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile) {
//	// declare bmp structures 
//	BITMAPFILEHEADER bmfh;
//	BITMAPINFOHEADER info;
//
//	// andinitialize them to zero
//	memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
//	memset(&info, 0, sizeof(BITMAPINFOHEADER));
//
//	// fill the fileheader with data
//	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
//	bmfh.bfReserved1 = 0;
//	bmfh.bfReserved2 = 0;
//	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
//	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
//
//	// fill the infoheader
//
//	info.biSize = sizeof(BITMAPINFOHEADER);
//	info.biWidth = width;
//	info.biHeight = height;
//	info.biPlanes = 1;			// we only have one bitplane
//	info.biBitCount = 24;		// RGB mode is 24 bits
//	info.biCompression = BI_RGB;
//	info.biSizeImage = 0;		// can be 0 for 24 bit images
//	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
//	info.biYPelsPerMeter = 0x0ec4;
//	info.biClrUsed = 0;			// we are in RGB mode and have no palette
//	info.biClrImportant = 0;    // all colors are important
//
//	// now we open the file to write to
//	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (file == NULL) {
//		cout << "ERROR: could not create bmp file: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return false;
//	}
//	if (GetLastError() == ERROR_INVALID_NAME) {
//		cout << "ERROR: invalid name: " << wstr_to_str(bmpfile) << '\n';
//		return false;
//	}
//	// write file header
//	unsigned long bwritten;
//	if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false) {
//		cout << "ERROR: could not write bmp file header: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return false;
//	}
//	// write infoheader
//	if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false) {
//		cout << "ERROR: could not write bmp file infoheader: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return false;
//	}
//	// write image data
//	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false) {
//		cout << "ERROR: could not write bmp file image data: " << wstr_to_str(bmpfile) << '\n';
//		CloseHandle(file);
//		return false;
//	}
//	// and close file
//	CloseHandle(file);
//
//	return true;
//}
//BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height) {
//	// first make sure the parameters are valid
//	if ((NULL == Buffer) || (width == 0) || (height == 0))
//		return NULL;
//
//	// find the number of padding bytes
//
//	int padding = 0;
//	int scanlinebytes = width * 3;
//	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
//		padding++;
//	// get the padded scanline width
//	int psw = scanlinebytes + padding;
//
//	// create new buffer
//	BYTE* newbuf = new BYTE[width * height * 3];
//
//	// now we loop trough all bytes of the original buffer, 
//	// swap the R and B bytes and the scanlines
//	long bufpos = 0;
//	long newpos = 0;
//	for (int y = 0; y<height; y++) {
//		for (int x = 0; x<3 * width; x += 3) {
//			newpos = y * 3 * width + x;
//			bufpos = (height - y - 1)*psw + x;
//
//			newbuf[newpos] = Buffer[bufpos + 2];
//			newbuf[newpos + 1] = Buffer[bufpos + 1];
//			newbuf[newpos + 2] = Buffer[bufpos];
//		}
//	}
//	return newbuf;
//}
//BYTE* ConvertRGBToBMPBuffer(BYTE* Buffer, int width, int height, long* newsize) {
//
//	// first make sure the parameters are valid
//	if ((NULL == Buffer) || (width == 0) || (height == 0))
//		return NULL;
//
//	// now we have to find with how many bytes
//	// we have to pad for the next DWORD boundary	
//
//	int padding = 0;
//	int scanlinebytes = width * 3;
//	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
//		padding++;
//	// get the padded scanline width
//	int psw = scanlinebytes + padding;
//
//	// we can already store the size of the new padded buffer
//	*newsize = height * psw;
//
//	// and create new buffer
//	BYTE* newbuf = new BYTE[*newsize];
//
//	// fill the buffer with zero bytes then we dont have to add
//	// extra padding zero bytes later on
//	memset(newbuf, 0, *newsize);
//
//	// now we loop trough all bytes of the original buffer, 
//	// swap the R and B bytes and the scanlines
//	long bufpos = 0;
//	long newpos = 0;
//	for (int y = 0; y < height; y++) {
//		for (int x = 0; x < 3 * width; x += 3) {
//			bufpos = y * 3 * width + x;     // position in original buffer
//			newpos = (height - y - 1) * psw + x;           // position in padded buffer
//
//			newbuf[newpos + 0] = Buffer[bufpos + 2];// swap r and b
//			newbuf[newpos + 1] = Buffer[bufpos + 1];// g stays
//			newbuf[newpos + 2] = Buffer[bufpos + 0];// swap b and r
//		}
//	}
//	return newbuf;
//}
//BYTE* bmp_to_array(string file, int &x, int &y) {
//	long s;
//	BYTE* a = LoadBMP(&x, &y, &s, str_to_wstr(file).c_str());
//	BYTE* newbuf = ConvertBMPToRGBBuffer(a, x, y);
//	delete[] a;
//	return newbuf;
//}
//void array_to_bmp(string file, BYTE* buffer, int x, int y) {
//	long s;
//	BYTE* b = ConvertRGBToBMPBuffer(buffer, x, y, &s);
//	SaveBMP(b, x, y, s, (str_to_wstr(file).c_str()));
//	delete[] b;
//}
//
