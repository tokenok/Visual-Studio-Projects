#ifndef image_header_guard
#define image_header_guard

#include <string>
#include <windows.h>
#include <vector>

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

struct rgb_color;

struct hsv_color;

//http://msdn.microsoft.com/en-us/library/dd183402(v=vs.85).aspx
BYTE* windowcapture(HWND hWnd, int* width = 0, int* height = 0, bool skip_save = true, std::string filename = "default.bmp");
BYTE* windowcapture(HWND hWnd, std::string filename, int* width = 0, int* height = 0);
//http://www.runicsoft.com/bmp.cpp
//http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html#chapter5
BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile);
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height);
BYTE* ConvertRGBToBMPBuffer(BYTE* Buffer, int width, int height, long* newsize);
BYTE* bmp_to_array(std::string file, int &x, int &y, HWND capture = NULL);
void array_to_bmp(std::string file, BYTE* buffer, int x, int y);
bool LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC);

hsv_color RGB_to_HSV(rgb_color RGB);
rgb_color HSV_to_RGB(hsv_color HSV);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

class process_image {
	public:
	BYTE* buffer;
	int x, y;
	std::string filename;
	std::string directory;
	std::string algs;

	process_image();
	process_image(const process_image& other);
	process_image& operator=(const process_image& other);
	~process_image();

	void load_bmp(std::string file);
};

extern std::vector<process_image*> process_images;

void bmp_to_gif(std::string file_directory, std::string filename, std::string output_directory, std::string output_filename = "");
void animate_gif(std::string file_directory, std::string output_directory, std::string output_filename, int delay = 0, int loop_count = 0);

void grayscale(BYTE *newbuf, int x, int y, UINT gray_algorithm, int extra = 0, std::string outfile = "");
BYTE* cropimage(BYTE *buf, int &x, int &y, int startx, int stopx, int starty, int stopy, std::string outfile = "");
BYTE* growimage(BYTE *buf, int &x, int &y, int startx, int stopx, int starty, int stopy, COLORREF background = RGB(0, 0, 0), std::string outfile = "");
void brightness(BYTE *newbuf, int x, int y, UINT brightness_algorithm, double val, std::string outfile = "");

BYTE* flatcoloradjust(BYTE *buf, int x, int y, INT red, INT green, INT blue, std::string outfile = "");


#endif



///directory set
//convert to gif
//create animated gif