#ifndef ocr_header_guard
#define ocr_header_guard

extern std::vector<std::string> stats_in_file;

extern bool ocr_spaces_on;

// ascii value, number of pixels in character+2 (size-2), width, height, [grid values...](zero based)
extern std::vector<std::vector<BYTE>> font16;

#pragma region statfile color struct

struct my_RGB {
	int red;
	int green;
	int blue;
	std::string text_in_file;
	my_RGB();
	my_RGB(std::string text_in_file);
	my_RGB(int red, int green, int blue);
};

extern std::vector<my_RGB> statcolors;

extern my_RGB blue_text_RGB;
extern my_RGB white_text_RGB;
extern my_RGB yellow_text_RGB;
extern my_RGB gray_text_RGB;
extern my_RGB gold_text_RGB;
extern my_RGB green_text_RGB;
extern my_RGB orange_text_RGB;
extern my_RGB red_text_RGB;
extern my_RGB magenta_text_RGB;

void reset_color_text_RGB();

#pragma endregion


bool rgbt(int red, int green, int blue);
bool process(BYTE *newbuf, int x, int y, std::string outfile = "process.bmp");

void find_box(BYTE* newbuf, int x, int y, POINT pt, std::string outfile = "find box.bmp");
std::vector<int> find_lines(BYTE* newbuf, int x, int y);
std::vector<int> isolate_letters(BYTE* newbuf, int width, int top, int bottom);
bool match(std::vector<int> a, int b, std::vector<std::vector<BYTE>> &charset);
std::vector<std::string> ocr(BYTE* newbuf, int x, int y, std::vector<std::vector<BYTE>> &charset);

#endif

