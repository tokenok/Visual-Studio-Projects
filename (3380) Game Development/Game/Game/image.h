#ifndef IMAGE_H
#define IMAGE_H

#include <Windows.h>
#include <string>

BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile);
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height);
BYTE* ConvertRGBToBMPBuffer(BYTE* Buffer, int width, int height, long* newsize);
BYTE* bmp_to_array(std::string file, int &x, int &y);
void array_to_bmp(std::string file, BYTE* buffer, int x, int y);

#endif

