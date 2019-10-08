#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <algorithm>

#pragma comment(lib, "Gdiplus.lib")

#include "img_convert.h"

using namespace std;
using namespace Gdiplus;

#define STW(s) wstring(s.begin(), s.end()).c_str()

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

string img_convert(string input_path, string output_path, ImageFormat output_type/* = ASSUME_FROM_OUTPUT_FILENAME*/) {
	string final_output_path;
	string soutput_type;

	string out_full_filename = output_path.substr(output_path.rfind('\\') + 1, output_path.size());

	if (output_type == ASSUME_FROM_OUTPUT_FILENAME) {
		
		//string t = output_filename.substr(output_filename.rfind('\\') + 1, output_filename.size());

		final_output_path = output_path;

		
		soutput_type = out_full_filename.substr(out_full_filename.rfind('.') + 1, out_full_filename.size());
		transform(soutput_type.begin(), soutput_type.end(), soutput_type.begin(), tolower);
	}
	else {
		string output_dir = output_path.substr(0, output_path.rfind("\\") + 1);
		string filenamenotype = out_full_filename.substr(out_full_filename.rfind('\\') + 1, out_full_filename.rfind('.') - out_full_filename.rfind('\\') - 1);

		cout << '\n';

		switch (output_type) {
			case BMP:{
				soutput_type = "bmp";
				break;
			}
			case JPEG:{
				soutput_type = "jpeg";
				break;
			}
			case GIF: {
				soutput_type = "gif";
				break;
			}
			case TIFF:{
				soutput_type = "tiff";
				break;
			}
			case PNG: {
				soutput_type = "png";
				break;
			}
			default: {
				return "";
			}
		}

		final_output_path = output_dir + filenamenotype + "." + soutput_type;
	}

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID encoderClsid;
	string tgecl = "image/" + soutput_type;
	GetEncoderClsid(wstring(tgecl.begin(), tgecl.end()).c_str(), &encoderClsid);

	Gdiplus::Image* image = new Gdiplus::Image(STW(input_path));
	
	Gdiplus::Status stat = image->Save(STW(final_output_path), &encoderClsid, NULL);
	if (stat == Gdiplus::Ok)
		printf("%s saved successfully\n", final_output_path.c_str());
	else
		printf("Failure: stat = %d\n", stat);

	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return final_output_path;
}

