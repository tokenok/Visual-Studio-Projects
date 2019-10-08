#include <iostream>

#include "image.h"
#include "C:\CPPlibs\common\common.h"

using namespace std;
using namespace Gdiplus;

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

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

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

void bmp_to_gif(string file_directory) {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CLSID encoderClsid;
	Gdiplus::Status stat;
	vector<string> files;
	GetFilesInDirectory(file_directory, "bmp", &files);
	for (UINT i = 0; i < files.size(); i++) {
		if (files[i].substr(files[i].rfind('.'), files[i].size()) == ".bmp") {
			string filename = files[i].substr(files[i].rfind('\\') + 1, files[i].size());
			string filenamenotype = files[i].substr(files[i].rfind('\\') + 1, files[i].rfind('.') - files[i].rfind('\\') - 1);
			Gdiplus::Image* image = new Gdiplus::Image(str_to_wstr(file_directory + filename).c_str());
			// Get the CLSID of the PNG encoder.
			GetEncoderClsid(L"image/gif", &encoderClsid);

			stat = image->Save(str_to_wstr(file_directory + filenamenotype + ".gif").c_str(), &encoderClsid, NULL);
			if (stat == Gdiplus::Ok)
				cout << filenamenotype + ".gif" << " saved successfully\n";
			else
				printf("Failure: stat = %d\n", stat);
			delete image;
		}
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);

	////image/bmp
	////image/jpeg
	////image/gif
	////image/tiff
	////image/png

	//GetEncoderClsid(L"image/gif", &encoderClsid);
	//encoderParameters.Count = 1;
	//encoderParameters.Parameter[0].Guid = EncoderSaveFlag;
	//encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	//encoderParameters.Parameter[0].NumberOfValues = 1;
	//encoderParameters.Parameter[0].Value = &parameterValue;
	//Image* multi = new Image(str_to_wstr(file_directory + "nvn0000.gif").c_str());
	//// Save the first page (frame).
	//parameterValue = EncoderValueMultiFrame;
	//stat = multi->Save(L"MultiFrame.gif", &encoderClsid, &encoderParameters);
	//if (stat == Ok)
	//	printf("Page 1 saved successfully.\n");
	//else
	//	cout << "Failure: stat = " << stat << " " << '\n';
	//vector<string> files;
	//GetFilesInDirectory(files, file_directory);
	//for (UINT i = 0; i < files.size(); i++) {
	//	if (files[i].substr(files[i].rfind('.'), files[i].size()) == ".bmp") {
	//		string filename = files[i].substr(files[i].rfind('/') + 1, files[i].size());
	//		string filenamenotype = files[i].substr(files[i].rfind('/') + 1, files[i].rfind('.') - files[i].rfind('/') - 1);
	//		Image* image = new Image(str_to_wstr(file_directory + filename).c_str());
	//		// Get the CLSID of the GIF encoder.
	//		
	//		stat = image->Save(str_to_wstr(file_directory + filenamenotype + ".gif").c_str(), &encoderClsid, NULL);
	//		if (stat == Ok) {
	//			parameterValue = EncoderValueFrameDimensionPage;
	//			Image* temp = new Image(str_to_wstr(file_directory + filenamenotype + ".gif").c_str());
	//			stat = multi->SaveAdd(temp, &encoderParameters);
	//			if (stat != Ok)
	//				cout << "Failure: " << stat << " " << '\n';
	//		}
	//		else
	//			cout << "Failure: " << filenamenotype << ".gif was not saved successfully. stat = " << stat << " " << '\n';
	//		delete image;
	//	}
	//}
	//parameterValue = EncoderValueFlush;
	//stat = multi->SaveAdd(&encoderParameters);
	//if (stat == Ok)
	//	printf("File closed successfully.\n");
	//GdiplusShutdown(gdiplusToken);
	//getchar();	
}
void animate_gif(string file_directory, string output, int delay/* = 0*/, int loop_count/* = 0*/) {
	string cmdline = "";

	vector<string> files;
	GetFilesInDirectory(file_directory, "gif", &files);
	if (files.size() == 0) {
		cout << "no files found" << '\n';
		return;
	}

	cmdline += "cd " + file_directory + " & convert -delay " + int_to_str(delay);

	for (UINT i = 0; i < files.size(); i++) {
		string filename = files[i].substr(files[i].rfind('\\') + 1, files[i].size());
		cmdline += " -page +0+0 " + filename;
	}

	cmdline += " -loop " + int_to_str(loop_count) + " " + output;
	system(cmdline.c_str());
}


bool get_inventory_layout(BYTE *newbuf, int x, int y, string outfile /* = "inventory_processed.bmp"*/) {
	struct color {
		int red;
		int green;
		int blue;
	};
	vector<color> colors = {
			{12, 12, 40}/*blue*/,
			{16, 20, 8}/*green*/,
			{44, 16, 8}/*red*/,
			{20, 20, 20}/*gray*/
	};
	auto color_match_test = [&](int r, int g, int b) -> bool {
		for (UINT i = 0; i < colors.size(); i++)
			if (r == colors[i].red && g == colors[i].green && b == colors[i].blue)
				return true;
		return false;
	};
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			int red = pos;
			int green = pos + 1;
			int blue = pos + 2;
			if (!color_match_test(newbuf[red], newbuf[green], newbuf[blue])) {
				newbuf[red] = 0;
				newbuf[green] = 0;
				newbuf[blue] = 0;
			}
			else {
				newbuf[red] += 150;
				newbuf[green] += 150;
				newbuf[blue] += 150;
			}
		}
	}
	if (outfile.size() > 0) {
		array_to_bmp(outfile, newbuf, x, y);
	}
	return true;
}