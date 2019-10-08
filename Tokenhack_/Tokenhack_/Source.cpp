#include <vector>
#include <Windows.h>

vector<vector<BYTE>> font16 = {
	{32, 2, 0, 0},
	{37, 47, 10, 9, 1, 2, 10, 11, 12, 13, 14, 16, 20, 23, 24, 25, 26, 30, 31, 32, 33, 35, 40, 41, 42, 43, 44, 45, 46, 47, 48, 54, 55, 56, 58, 59, 63, 65, 66, 67, 69, 73, 75, 76, 77, 82, 86, 87, 88},//%
	{39, 5, 1, 3, 0, 1, 2},//'
	{40, 15, 3, 12, 2, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 31, 35},//(
	{41, 14, 2, 11, 0, 2, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21},//)
	{43, 15, 5, 5, 2, 5, 7, 10, 11, 12, 13, 14, 15, 17, 21, 22, 23},//+
	{44, 6, 2, 3, 1, 2, 3, 4},//,
	{45, 4, 2, 1, 0, 1},//- (PROBLEM CHAR) - right side of gap
	{47, 16, 6, 12, 5, 11, 16, 22, 27, 32, 33, 38, 39, 44, 49, 55, 60, 66},//'/'
	{48, 37, 9, 10, 4, 5, 11, 12, 13, 14, 15, 16, 19, 24, 25, 27, 28, 32, 33, 34, 35, 36, 40, 44, 45, 48, 53, 54, 56, 61, 62, 64, 65, 70, 74, 75, 76, 78, 85},//0
	{49, 11, 2, 9, 1, 2, 4, 6, 8, 10, 12, 14, 16},//1
	{50, 27, 6, 9, 2, 7, 8, 10, 12, 13, 16, 17, 18, 19, 22, 23, 28, 33, 34, 38, 39, 43, 44, 48, 49, 50, 51, 52, 53},//2
	{51, 26, 6, 9, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 16, 21, 26, 27, 28, 34, 35, 37, 41, 43, 47, 49, 50, 52},//3
	{52, 20, 7, 8, 4, 10, 11, 17, 18, 23, 25, 29, 32, 35, 36, 37, 38, 39, 40, 41, 46, 53},//4
	{53, 26, 6, 8, 1, 2, 4, 7, 13, 18, 19, 20, 21, 22, 25, 28, 29, 30, 31, 35, 36, 37, 40, 41, 43, 44, 45, 46},//5
	{54, 24, 6, 10, 5, 9, 10, 15, 20, 21, 25, 26, 27, 28, 31, 35, 36, 37, 41, 43, 47, 49, 50, 51, 52, 57},//6
	{55, 16, 6, 9, 2, 3, 6, 7, 8, 10, 11, 16, 22, 28, 33, 39, 44, 50},//7
	{56, 31, 6, 10, 2, 3, 7, 8, 9, 10, 12, 13, 16, 19, 21, 22, 25, 26, 27, 30, 31, 34, 36, 40, 41, 42, 43, 46, 49, 50, 51, 52, 56},//8
	{57, 23, 6, 9, 3, 7, 8, 9, 10, 11, 12, 13, 17, 18, 19, 23, 25, 28, 29, 32, 33, 34, 39, 45, 50},//9
	{58, 8, 2, 7, 0, 2, 3, 10, 12, 13},//:
	{65, 25, 8, 9, 4, 11, 12, 19, 21, 26, 29, 34, 38, 41, 46, 49, 50, 51, 52, 54, 55, 56, 57, 63, 64, 65, 71},//A
	{66, 35, 5, 9, 0, 1, 2, 3, 4, 5, 6, 9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 24, 25, 26, 29, 30, 31, 34, 35, 36, 37, 38, 39, 40, 41},//B
	{67, 26, 7, 10, 4, 9, 10, 11, 12, 13, 15, 20, 22, 28, 35, 36, 42, 43, 50, 51, 58, 59, 60, 61, 62, 66, 67, 68},//C
	{68, 16, 4, 8, 0, 1, 2, 6, 7, 11, 15, 19, 23, 26, 27, 28, 29, 30},//D (PROBLEM CHAR) - right side of gap
	{69, 33, 5, 9, 1, 2, 3, 4, 6, 10, 11, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 30, 31, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44},//E
	{70, 23, 6, 8, 0, 1, 2, 3, 4, 7, 12, 13, 18, 19, 20, 22, 23, 24, 25, 26, 28, 30, 31, 37, 43},//F
	{71, 28, 7, 9, 1, 2, 3, 4, 5, 6, 8, 9, 14, 21, 26, 27, 28, 33, 34, 35, 41, 43, 48, 51, 52, 53, 54, 55, 59, 60},//G
	{72, 37, 7, 10, 6, 7, 8, 12, 15, 19, 22, 26, 28, 29, 30, 31, 32, 33, 35, 36, 37, 38, 39, 40, 41, 42, 43, 47, 49, 50, 54, 55, 56, 57, 61, 62, 63, 64, 68},//H
	{73, 16, 2, 8, 0, 1, 2, 3, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15},//I
	{74, 21, 2, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18},//J
	{75, 24, 7, 9, 1, 4, 8, 10, 11, 15, 16, 17, 22, 23, 29, 30, 36, 38, 43, 46, 50, 54, 55, 56, 57, 62},//K
	{76, 17, 5, 9, 0, 5, 10, 15, 20, 25, 30, 35, 36, 37, 38, 40, 42, 43, 44},//L
	{77, 53, 9, 10, 3, 4, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 22, 25, 26, 27, 30, 31, 34, 35, 36, 39, 40, 43, 44, 45, 48, 49, 52, 53, 54, 57, 58, 61, 62, 63, 66, 67, 70, 71, 72, 75, 76, 79, 80, 81, 84, 85, 88, 89},//M
	{78, 34, 8, 9, 0, 1, 6, 7, 9, 14, 17, 18, 19, 22, 24, 25, 27, 30, 32, 33, 36, 38, 40, 41, 45, 46, 48, 49, 54, 56, 57, 63, 64, 65, 70, 71},//N
	{79, 53, 9, 10, 4, 10, 11, 12, 13, 14, 15, 16, 19, 20, 22, 25, 27, 28, 31, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 58, 61, 62, 64, 67, 69, 70, 74, 75, 76, 77, 78, 84, 85},//O
	{80, 25, 6, 8, 0, 1, 2, 3, 4, 6, 7, 10, 12, 13, 16, 17, 19, 22, 25, 26, 27, 30, 31, 36, 37, 42, 43},//P
	{81, 37, 9, 10, 2, 3, 4, 5, 6, 10, 15, 16, 18, 19, 25, 26, 27, 34, 35, 36, 44, 45, 49, 52, 53, 54, 55, 58, 61, 64, 65, 66, 67, 68, 69, 75, 76, 77, 85},//Q
	{82, 25, 7, 9, 0, 1, 2, 3, 7, 11, 14, 18, 21, 24, 25, 29, 30, 31, 38, 43, 46, 49, 50, 54, 55, 56, 62},//R
	{83, 24, 5, 8, 1, 2, 3, 4, 5, 6, 9, 10, 11, 16, 17, 23, 24, 29, 30, 31, 34, 35, 36, 37, 38, 39},//S
	{84, 24, 9, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 14, 22, 23, 32, 40, 41, 49, 50, 59, 67, 68, 76, 77},//T
	{85, 36, 10, 9, 0, 1, 8, 9, 10, 11, 18, 19, 20, 21, 28, 29, 30, 31, 38, 39, 40, 41, 48, 49, 51, 52, 58, 59, 61, 68, 72, 73, 74, 75, 76, 77, 84, 85},//U
	{86, 22, 9, 8, 0, 1, 8, 10, 16, 17, 19, 20, 25, 29, 33, 34, 39, 42, 48, 50, 51, 58, 59, 67},//V
	{87, 43, 13, 9, 0, 1, 4, 5, 8, 11, 12, 14, 18, 20, 21, 24, 27, 28, 31, 32, 33, 37, 41, 45, 46, 49, 54, 55, 58, 62, 68, 70, 71, 72, 74, 81, 82, 83, 85, 86, 87, 95, 96, 99, 108},//W
	{88, 20, 8, 9, 0, 7, 9, 14, 15, 18, 21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63, 64},//X
	{89, 25, 9, 10, 7, 9, 10, 16, 17, 19, 20, 24, 25, 29, 30, 32, 33, 38, 39, 40, 41, 49, 58, 67, 76, 84, 85},//Y
	{90, 26, 6, 9, 0, 1, 2, 3, 4, 5, 10, 11, 16, 21, 22, 27, 32, 37, 38, 43, 44, 45, 46, 47, 49, 50, 51, 53},//Z
	{97, 15, 6, 7, 3, 9, 14, 16, 19, 22, 25, 29, 30, 31, 32, 35, 36},//a
	{98, 16, 4, 6, 0, 1, 2, 4, 8, 9, 10, 12, 14, 15, 16, 19, 20, 22},//b
	{99, 16, 6, 8, 3, 4, 5, 8, 10, 11, 13, 19, 24, 25, 31, 38, 45, 47},//c
	{100, 24, 6, 7, 0, 1, 2, 4, 6, 7, 11, 12, 13, 17, 18, 19, 23, 24, 25, 29, 30, 31, 34, 37, 38, 39},//d
	{101, 17, 4, 7, 0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 20, 27},//e
	{102, 19, 4, 6, 0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 21},//f
	{103, 21, 6, 8, 3, 4, 7, 8, 9, 10, 11, 18, 24, 27, 28, 29, 31, 35, 37, 41, 44, 45, 46},//g
	{104, 22, 5, 8, 4, 9, 10, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 29, 30, 31, 34, 35, 39},//h
	{105, 9, 1, 7, 0, 1, 2, 3, 4, 5, 6},//i
	// no lowercase j (PROBLEM CHAR) - identical to lowercase i (105)
	{107, 15, 4, 7, 0, 2, 3, 4, 5, 8, 12, 13, 16, 18, 20, 23, 24},//k
	{108, 11, 3, 7, 0, 3, 6, 9, 12, 15, 18, 19, 20},//l
	{109, 29, 7, 8, 2, 3, 7, 8, 9, 10, 11, 12, 13, 14, 17, 20, 21, 24, 27, 28, 31, 34, 35, 38, 41, 42, 45, 48, 49, 52, 55},//m
	{110, 27, 6, 7, 0, 4, 5, 6, 7, 10, 11, 12, 14, 16, 17, 18, 20, 21, 22, 23, 24, 27, 28, 29, 30, 34, 35, 36, 41},//n
	{111, 33, 7, 8, 3, 8, 9, 10, 11, 12, 14, 17, 20, 21, 24, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 41, 43, 44, 45, 46, 47, 51, 52, 53},//o
	{112, 12, 4, 6, 0, 1, 2, 3, 7, 11, 12, 13, 14, 20},//p
	{113, 25, 7, 9, 3, 8, 9, 10, 11, 12, 14, 19, 20, 21, 27, 28, 34, 35, 38, 41, 43, 45, 46, 47, 51, 52, 59},//q
	{114, 14, 5, 6, 0, 2, 3, 8, 10, 11, 13, 15, 16, 17, 23, 29},//r
	{115, 16, 5, 7, 1, 2, 3, 5, 6, 11, 12, 18, 24, 25, 26, 31, 32, 33},//s
	{116, 15, 7, 7, 0, 7, 8, 9, 10, 11, 12, 13, 17, 24, 31, 38, 45},//t
	{117, 10, 4, 7, 3, 7, 11, 15, 19, 22, 24, 25},//u (PROBLEM CHAR) - right side of gap
	{118, 16, 8, 8, 7, 8, 14, 16, 17, 21, 25, 29, 34, 36, 42, 44, 51, 59},//v
	{119, 29, 10, 7, 0, 10, 15, 16, 18, 19, 20, 23, 25, 28, 31, 33, 34, 35, 37, 38, 41, 44, 47, 52, 53, 54, 55, 62, 63, 65, 66},//w
	{120, 15, 7, 6, 0, 5, 6, 8, 11, 16, 17, 23, 24, 29, 32, 35, 40},//x
	{121, 13, 8, 7, 1, 8, 9, 14, 15, 18, 21, 27, 28, 43, 51},//y
	{122, 17, 6, 7, 0, 1, 2, 4, 5, 10, 15, 20, 26, 31, 37, 38, 39, 40, 41}//z
};




struct my_RGB {
	int red;
	int green;
	int blue;
	my_RGB(): red(-1), green(-1), blue(-1) {}
};

my_RGB blue_text_RGB;
my_RGB white_text_RGB;
my_RGB yellow_text_RGB;
my_RGB grey_text_RGB;
my_RGB gold_text_RGB;
my_RGB green_text_RGB;
my_RGB orange_text_RGB;
my_RGB red_text_RGB;
my_RGB magenta_text_RGB;


bool rgbt(int red, int green, int blue) {
	if (red == blue_text_RGB.red && green == blue_text_RGB.green && blue == blue_text_RGB.blue)
		return true;
	else if (red == white_text_RGB.red && green == white_text_RGB.green && blue == white_text_RGB.blue)
		return true;
	else if (red == yellow_text_RGB.red && green == yellow_text_RGB.green && blue == yellow_text_RGB.blue)
		return true;
	else if (red == grey_text_RGB.red && green == grey_text_RGB.green && blue == grey_text_RGB.blue)
		return true;
	else if (red == gold_text_RGB.red && green == gold_text_RGB.green && blue == gold_text_RGB.blue)
		return true;
	else if (red == green_text_RGB.red && green == green_text_RGB.green && blue == green_text_RGB.blue)
		return true;
	else if (red == orange_text_RGB.red && green == orange_text_RGB.green && blue == orange_text_RGB.blue)
		return true;
	else if (red == red_text_RGB.red && green == red_text_RGB.green && blue == red_text_RGB.blue)
		return true;
	else if (red == magenta_text_RGB.red && green == magenta_text_RGB.green && blue == magenta_text_RGB.blue)
		return true;
	return false;

}





vector<int> find_lines(BYTE* newbuf, int x, int y) {
	vector<int> lines;
	int top = 1000000;
	int gap_count = 0;
	for (int i = 0; i < y; i++) {
		int gap = 0;
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * 3 * x + j;
			int red = (int)newbuf[pos];
			int green = (int)newbuf[pos + 1];
			int blue = (int)newbuf[pos + 2];
			if (!rgbt(red, green, blue)) {
				gap++;
				if (gap == x && i>top) {
					top = 1000000;
					lines.push_back(i - 1);
				}
			}
			else {
				if (i<top) {
					top = i;
					i += 6;
					lines.push_back(top);
				}
			}
		}
	}
	return lines;
}






vector<int> isolate_letters(BYTE* newbuf, int width, int top, int bottom) {
	vector<int> gaps;
	int l = 1000000, r = 0, t = 1000000, b = 0;
	int gap_count = 0;
	for (int i = 0; i<width * 3; i += 3) {//crops to left right top and bottom
		int gap = 0;
		for (int j = top*width * 3 + i; j<(width * 3 * (bottom + 1)); j += (width * 3)) {
			int red = (int)newbuf[j];
			int green = (int)newbuf[j + 1];
			int blue = (int)newbuf[j + 2];
			if (rgbt(red, green, blue) && red != 171) {
				if (i / 3<l)
					l = i / 3;
				r = i / 3 + 1;
				if ((j - i) / (width * 3)<t)
					t = (j - i) / (width * 3);
				if ((j - i) / (width * 3) + 1>b)
					b = (j - i) / (width * 3) + 1;
			}
			else
				gap++;
			if (gap == bottom + 1 - top && ocr_spaces_on) {
				gap_count++;
				if (gap_count>8) {
					gaps.push_back(-1); gaps.push_back(-1); gaps.push_back(-1); gaps.push_back(-1);
					gap_count = 0;
				}
			}
			if (gap == bottom + 1 - top && r) {
				gaps.push_back(l); gaps.push_back(r); gaps.push_back(t); gaps.push_back(b);
				l = 1000000, r = 0, t = 1000000, b = 0;
				gap_count = 0;
			}
		}
	}
	if (gaps.size()>0 && ocr_spaces_on) {
		while (gaps[0] == -1)
			gaps.erase(gaps.begin(), gaps.begin() + 1);
		while (gaps[gaps.size() - 1] == -1)
			gaps.erase(gaps.end() - 1, gaps.end());
	}
	return gaps;
}




bool match(vector<int> a, int b, vector<vector<BYTE>> &charset) {
	int size = charset[b][1];
	if (size != a.size())
		return false;
	for (int i = 0; i<(int)a.size() - 1; i++)
		if (a[i] != charset[b][i + 2])
			return false;
	return true;
}





vector<string> ocr(BYTE* newbuf, int x, int y, vector<vector<BYTE>> &charset) {
	vector<string> item_stats;
	vector<int> lines = find_lines(newbuf, x, y);//finds top and bottom row values for each line of text
	for (int l = 0; l<(int)lines.size(); l += 2) {
		vector<int> letters = isolate_letters(newbuf, x, lines[l], lines[l + 1]);
		if (letters.size() == 0)
			continue;
		vector<vector<int>> let;
		vector<int> temp;
		for (int a = 0; a<(int)letters.size(); a += 4) {//puts each isolated letter into its own array. ex. (Capital H)
			int left = letters[a];                                          //0, x, x, x, 4,
			int right = letters[a + 1];                                     //5, x, x, x, 9,
			int top = letters[a + 2];                                       //10,11,12,13,14,
			int bottom = letters[a + 3];                            //15,x, x, x, 19,
			temp.push_back(right - left);                           //20,x, x, x, 24
			temp.push_back(bottom - top);                           //in array it would be: {5(width), 5(height), 0,4,5,9,10,11,12,13,14,15,19,20,24}
			for (int i = top; i<bottom; i++) {
				for (int j = left * 3; j<3 * right; j += 3) {
					int pos = i * 3 * x + j;
					int red = (int)newbuf[pos];
					int green = (int)newbuf[pos + 1];
					int blue = (int)newbuf[pos + 2];
					if (rgbt(red, green, blue)) {
						temp.push_back((pos / 3) - (x*i) - left + ((right - left)*(i - top)));
					}
				}
			}
			let.push_back(temp);
			temp.erase(temp.begin(), temp.end());
		}
		int size = charset.size();
		string tline = "";
		for (int i = 0; i<(int)let.size(); i++) {
			for (int a = 0; a<size; a++) {
				if (match(let[i], a, charset)) {
					tline += (char)charset[a][0];
					break;
				}
			}
		}
		if (tline.size()>0) {
			char start = tline[0];
			char end = tline[tline.size() - 1];
			while (tline.size()>0 && (start == ' ' || start == '-' || start == ':' || start == ',' || start == '\'')) {
				tline.erase(tline.begin(), tline.begin() + 1);
				start = tline[0];
			}
			while (tline.size()>0 && (end == ' ' || end == '-' || end == ':' || end == ',' || end == '\'')) {
				tline.erase(tline.end() - 1, tline.end());
				end = tline[tline.size() - 1];
			}
			if (tline.size()>1)
				item_stats.push_back(tline);
		}
	}
	return item_stats;
}