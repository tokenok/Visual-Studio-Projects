#include <Windows.h>
#include <windowsx.h>
#include <opencv2/opencv.hpp>
#include <Commctrl.h>
#include <string>
#include <Strsafe.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <functional>
#ifdef _DEBUG
#include <conio.h>
#define _GETCH(); _getch();
#else
#define _GETCH();
#endif
//junimo plush (blue)
#pragma comment(lib, "Comctl32.lib")


#include "resource.h"
#include "img_convert.h"
#include "CustomEditControl.h"
#include "font_image.h"

#include "C:/CPPlibs/common/common.h"

#define WIN32_LEAN_AND_MEAN

using namespace std;
using namespace cv;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OptionsProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/);
BOOL CALLBACK ChangeFontProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/);
LRESULT CALLBACK kbHookProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ContrastEdcProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

HWND g_hwnd;
HWND g_options;

CustomEditControl edc;

string g_last_converted_image_filename = "";

string g_test_filename = "trish2.bmp";

double
	dx1 = 1.0 / 4.0,
	dx2 = 3.0 / 4.0,
	dy1 = 1.0 / 4.0,
	dy2 = 3.0 / 4.0;

int
	g_corner_weight = 10,
	g_edge_weight = 10,
	g_center_weight = 48,
	g_brightness_weight = 1000;

struct ImageCoverageSection {
	double d;

	ImageCoverageSection() {
		d = 0.0;
		c = 0;
	}

	void update(double d) {
		this->d = ((this->c * this->d) + d) / (this->c + 1);
		++this->c;
	}

	private:
	int c;
};

struct GlyphInfo {
	double cover;
	wchar_t c;

	int w, h;

	ImageCoverageSection cover_9[9];

	static bool orderByCoverage(GlyphInfo& a, GlyphInfo& b) {
		return a.cover < b.cover;
	}
};

struct Options_info {
	int result;
	string output_filename;
	bool mask, color;
	COLORREF mask_color;
	int x, y;
	wstring character_set;
	bool open_html;
	double gamma_correction, brightness, contrast;
};

struct font_info {
	NEWTEXTMETRIC ntm;
	LOGFONT lf;
	DWORD fontType;
};

void rgb_to_hsv(double r, double g, double b, double& h, double& s, double& v) {
	double min, max, delta;

	min = r < g ? r : g;
	min = min  < b ? min : b;

	max = r > g ? r : g;
	max = max  > b ? max : b;

	v = max;                                // v
	delta = max - min;
	if (delta < 0.00001) {
		s = 0;
		h = 0; // undefined, maybe nan?
		return;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		s = 0.0;
		h = NAN;                            // its now undefined
		return;
	}
	if (r >= max)                           // > is bogus, just keeps compilor happy
		h = (g - b) / delta;        // between yellow & magenta
	else
		if (g >= max)
			h = 2.0 + (b - r) / delta;  // between cyan & yellow
		else
			h = 4.0 + (r - g) / delta;  // between magenta & cyan

	h *= 60.0;                              // degrees

	if (h < 0.0)
		h += 360.0;

	return;
}

void hsv_to_rgb(double h, double s, double v, double& r, double& g, double& b) {
	double hh, p, q, t, ff;
	long i;

	if (s <= 0.0) {
		r = v;
		g = v;
		b = v;
		return;
	}

	hh = h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
		default:
			r = v;
			g = p;
			b = q;
			break;
	}
	return;
}

GlyphInfo getGlyphCoverage(FontImage& glyph) {
	GlyphInfo ret = {};

	if (!glyph.h || !glyph.w)
		return ret;

	if (glyph.buf) {
		double count = 0;

		int x1, x2, y1, y2;

		x1 = (int)ceil((double)glyph.w * dx1) - 1;
		x2 = (int)floor((double)glyph.w * dx2) - 1;

		y1 = (int)floor((double)glyph.h * dy1) - 1;
		y2 = (int)ceil((double)glyph.h * dy2) - 1;

		for (int y = 0; y < glyph.h; y++) {
			for (int x = 0; x < glyph.w; x++) {
				int pos = y * 3 * glyph.w + (x * 3);

				if (glyph.buf[pos + 0] != 255) {
					count++;
				}

				int gray = (int)(((double)glyph.buf[pos] * 0.2125) + ((double)glyph.buf[pos + 1] * 0.7154) + ((double)glyph.buf[pos + 2] * 0.0721));

				if (y <= y1) {
					if (x <= x1)
						ret.cover_9[0].update(gray);
					else if (x > x1 && x <= x2)
						ret.cover_9[1].update(gray);
					else
						ret.cover_9[2].update(gray);
				}
				else if (y > y1 && y <= y2) {
					if (x <= x1)
						ret.cover_9[3].update(gray);
					else if (x > x1 && x <= x2)
						ret.cover_9[4].update(gray);
					else
						ret.cover_9[5].update(gray);
				}
				else {
					if (x <= x1)
						ret.cover_9[6].update(gray);
					else if (x > x1 && x <= x2)
						ret.cover_9[7].update(gray);
					else
						ret.cover_9[8].update(gray);
				}
			}
		}

		count /= ((double)glyph.h * (double)glyph.w);

		ret.cover = count;
		ret.c = glyph.c;
		ret.w = glyph.w;
		ret.h = glyph.h;
	}

	return ret;
}

wstring getDistributedShadingCharacters(vector<GlyphInfo>& glyphs, unsigned int num_c) {
	if (num_c > glyphs.size())
		num_c = glyphs.size();

	sort(glyphs.begin(), glyphs.end(), GlyphInfo::orderByCoverage);

	double inc = (double)glyphs.size() / (double)num_c;

	wstring ws1(L"");

	ws1 += glyphs[0].c;

	for (int i = 1; i < num_c - 1; i++) {
		int val = (int)round((double)i * inc);

		if (val > glyphs.size()) {
			val = glyphs.size();
		}

		ws1 += glyphs[val].c;
	}

	ws1 += glyphs[glyphs.size() - 1].c;

	reverse(ws1.begin(), ws1.end());

	return ws1;
}

vector<GlyphInfo> getFontGlyphInfo(HWND hwnd, const wstring& shades = L"") {
	vector<GlyphInfo> glyphs;

	vector<FontImage> glyph_arrs = GetWindowFontGlyphsAsArray(hwnd);

	for (int i = 0; i < glyph_arrs.size(); i++) {
		if (shades != L"")
			if (find(shades.begin(), shades.end(), glyph_arrs[i].c) == shades.end())
				continue;

		GlyphInfo gi = getGlyphCoverage(glyph_arrs[i]);
		if (gi.c != 0)
			glyphs.push_back(gi);
		delete[] glyph_arrs[i].buf;
	}

	return glyphs;
}

double getScore(const ImageCoverageSection q[9], int gray, const GlyphInfo &gi) {
	double score = 0;

	/*vector<ImageCoverageSection> q1;
	vector<ImageCoverageSection> g1;
	for (int i = 0; i < 9; i++) {
		q1.push_back(q[i]);
		g1.push_back(g[i]);
	}*/
	if (g_brightness_weight)
		score += (abs(gray - (255 - (double)gi.cover * 255)) * g_brightness_weight);

	score += (abs(gi.cover_9[0].d - q[0].d) * g_corner_weight);
	score += (abs(gi.cover_9[1].d - q[1].d) * g_edge_weight);
	score += (abs(gi.cover_9[2].d - q[2].d) * g_corner_weight);
	score += (abs(gi.cover_9[3].d - q[3].d) * g_edge_weight);
	score += (abs(gi.cover_9[4].d - q[4].d) * g_center_weight);
	score += (abs(gi.cover_9[5].d - q[5].d) * g_edge_weight);
	score += (abs(gi.cover_9[6].d - q[6].d) * g_corner_weight);
	score += (abs(gi.cover_9[7].d - q[7].d) * g_edge_weight);
	score += (abs(gi.cover_9[8].d - q[8].d) * g_corner_weight);

	return score;
}

GlyphInfo getBestFittingChar(const ImageCoverageSection q[9], int gray, const vector<GlyphInfo> &glyphs) {
	GlyphInfo best;
	double best_score = MAXDWORD64;

	/*vector<ImageCoverageSection> q1;
	for (int i = 0; i < 9; i++)
		q1.push_back(q[i]);*/

	for (int i = 0; i < glyphs.size(); i++) {
		double score = getScore(q, gray, glyphs[i]);

		if (score < best_score) {
			best = glyphs[i];
			best_score = score;
		}
	}

	return best;
}

BYTE boundColor(double& a) {
	a = a > 255 ? 255 : a < 0 ? 0 : a;
	return (BYTE)a;
}

void applyFilters(const string &filename, BYTE* img, int x, int y, double gamma_correction, double contrast, double brightness) {
	if (contrast == 0 && brightness == 100 && gamma_correction == 0) return;

	double factor = (259 * (contrast + 255)) / (255 * (259 - contrast));

	ImageCoverageSection avgcov[1];

	int x3 = x * 3;

	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x3; j += 3) {
			int pos = i * x3 + j;

			double r = img[pos + 0];
			double g = img[pos + 1];
			double b = img[pos + 2];

			if (gamma_correction)
				avgcov[0].update((int)((r * 0.2125) + (g * 0.7154) + (b * 0.0721)));

			if (brightness != 100) {
				double h, s, v;
				rgb_to_hsv(r, g, b, h, s, v);
				double new_b = (double)v * (brightness / 100);
				v = boundColor(new_b);
				hsv_to_rgb(h, s, v, r, g, b);

				/*r += brightness;
				g += brightness;
				b += brightness;*/

				boundColor(r);
				boundColor(g);
				boundColor(b);
			}

			if (contrast != 0) {
				r = (int)trunc(factor * (r - 128) + 128);
				g = (int)trunc(factor * (g - 128) + 128);
				b = (int)trunc(factor * (b - 128) + 128);

				boundColor(r);
				boundColor(g);
				boundColor(b);
			}

			img[pos + 0] = r;
			img[pos + 1] = g;
			img[pos + 2] = b;
		}
	}

	if (gamma_correction) {
		//ImageCoverageSection avgcov[1];
		int osz = x3 * y;
		//for (int i = 0; i < osz; i += 3)
		//	avgcov[0].update((int)(((double)img[i] * 0.2125) + ((double)img[i + 1] * 0.7154) + ((double)img[i + 2] * 0.0721)));
		double avg = avgcov->d/* / 255.0 * 1.35*/;

		//ImageCoverageSection avgcov[1];

		//for (int i = 0; i < osz; i += 3)
		//	avgcov[0].update((int)(((double)img[i] * 0.2125) + ((double)img[i + 1] * 0.7154) + ((double)img[i + 2] * 0.0721)));
		//double avg = avgcov->d / 255.0 * 1.35;//1.35

		double gamma = avg / 255.0 + (gamma_correction / 100.0);

	//	gamma = gamma_correction / 100.0;
		
		for (int i = 0; i < osz; i++) {
			img[i] = 255 * pow(((double)img[i] / 255.0), gamma);
		}
	}

	//array_to_bmp(filename, img, x, y);
}

vector<vector<pair<wchar_t, COLORREF>>> convert_image_to_ascii2(HWND txtbox, string filename, int sx, int sy, wstring shades, bool bmask, COLORREF mask, double gamma_correction, double brightness, double contrast) {
	vector<vector<pair<wchar_t, COLORREF>>> ret;

	int iw, ih;
	BYTE* img = bmp_to_array(filename, iw, ih);

	if (!img)
		return ret;

	applyFilters("conout.bmp", img, iw, ih, gamma_correction, contrast, brightness);

	vector<GlyphInfo> glyphs = getFontGlyphInfo(txtbox, shades);

	/*HDC hDC = GetDC(txtbox);
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);	
	ReleaseDC(txtbox, hDC);
	sx = (double)iw / (double)sx;
	sy = sx * ((double)tm.tmHeight / (double)tm.tmAveCharWidth);
*/
	if (sx == 0 || sy == 0) {
		sx = sx == 0 ? 1 : sx;
		sy = sy == 0 ? 2 : sy;
	}

	int x1, x2, y1, y2;
	x1 = (int)ceil((double)sx * dx1) - 1;
	x2 = (int)floor((double)sx * dx2) - 1;
	y1 = (int)floor((double)sy * dy1) - 1;
	y2 = (int)ceil((double)sy * dy2) - 1;

	int w = 3 * iw, h = ih;
	for (int i = 0; i <= h - sy; i += sy) {
		vector<pair<wchar_t, COLORREF>> row;
		for (int j = 0; j <= w - sx * 3; j += sx * 3) {
			int r, g, b, gray; r = g = b = gray = 0;
			int n = sx * sy;

			ImageCoverageSection q[9];

			for (int k = i; k < i + sy; k++) {
				for (int l = j; l < j + sx * 3; l += 3) {
					int pos = k * w + l;

					int y = k - i;
					int x = (l - j) / 3;

					if (bmask) {
						COLORREF col = RGB(img[pos], img[pos + 1], img[pos + 2]);
						if (col == mask) {
							n--;
							continue;
						}
					}

					gray = (int)(((double)img[pos] * 0.2125) + ((double)img[pos + 1] * 0.7154) + ((double)img[pos + 2] * 0.0721));

					if (y <= y1) {
						if (x <= x1)
							q[0].update(gray);
						else if (x > x1 && x <= x2)
							q[1].update(gray);
						else
							q[2].update(gray);
					}
					else if (y > y1 && y <= y2) {
						if (x <= x1)
							q[3].update(gray);
						else if (x > x1 && x <= x2)
							q[4].update(gray);
						else
							q[5].update(gray);
					}
					else {
						if (x <= x1)
							q[6].update(gray);
						else if (x > x1 && x <= x2)
							q[7].update(gray);
						else
							q[8].update(gray);
					}					

					r += img[pos];
					g += img[pos + 1];
					b += img[pos + 2];
				}
			}

			if (n == 0) {
				r = 255;
				g = 255;
				b = 255;
			}
			else {
				r /= n;
				g /= n;
				b /= n;
			}

			gray = (int)(((double)r * 0.2125) + ((double)g * 0.7154) + ((double)b * 0.0721));

			GlyphInfo best = getBestFittingChar(q, gray, glyphs);
			row.push_back(std::make_pair(best.c, RGB(r, g, b)));

			//int ind = (int)(((double)gray / 255.0) * (shades.size() - 1));
			//row.push_back(std::make_pair(shades[ind], RGB(r, g, b)));
		}
		ret.push_back(row);
	}

	return ret;
}

vector<vector<pair<wchar_t, COLORREF>>> convert_image_to_ascii3(HWND txtbox, string filename, int sx, int sy, wstring shades, bool bmask, COLORREF mask, double gamma_correction, double brightness, double contrast) {
	vector<vector<pair<wchar_t, COLORREF>>> ret;

	int iw, ih;
	BYTE* img = bmp_to_array(filename, iw, ih);

	if (!img) return ret;
	
	applyFilters("conout.bmp", img, iw, ih, gamma_correction, contrast, brightness);

	vector<GlyphInfo> glyphs = getFontGlyphInfo(txtbox, shades);
	if (!glyphs.size()) return ret;

	sx = iw * sy / ih;

	map<int, vector<GlyphInfo>> glyphs_by_width;
	for (int i = 0; i < glyphs.size(); i++)
		glyphs_by_width[glyphs[i].w].push_back(glyphs[i]);

	for (auto & a : glyphs_by_width) {
		sort(a.second.begin(), a.second.end(), GlyphInfo::orderByCoverage);
	}

	srand(clock());

	vector<vector<int>> rows;
	vector<int> row_lens;
	int num_rows = ih / sy;
	int ph = num_rows * glyphs[0].h;
	int pw = ph * iw / ih;
	for (int i = 0; i < num_rows; i++) {
		int rw = 0;
		int tries = 0;
		vector<int> row;
		do {
			int w = glyphs[rand() % glyphs.size()].w;
			rw += w;
			row.push_back(w);

			if (rw > pw && tries < 1000) {
				tries++;
				for (int i = 0; i < 5; i++) {
					int rem = rand() % row.size();
					rw -= row[rem];
					row.erase(row.begin() + rem);
				}
			}
		} while (rw != pw && tries < 1000);
		row_lens.push_back(rw);
		rows.push_back(row);
	}

	if (sx == 0 || sy == 0) {
		sx = sx == 0 ? 1 : sx;
		sy = sy == 0 ? 2 : sy;
	}
	
	int w = 3 * iw, h = ih;
	for (int i = 0, ri = 0; i <= h - sy; i += sy, ri++) {
		vector<pair<wchar_t, COLORREF>> row;
		int rj = 0;

		int ccx = (rows[ri][rj] * iw / (pw));

		for (int j = 0; j <= w - ccx * 3; j += ccx * 3, rj++) {		
			if (ri >= rows.size() || rj >= rows[ri].size()) {
				cout << rj << " " << rows[ri].size() << '\n';
				continue;
			}

			ccx = (rows[ri][rj] * iw / (pw));

			int r, g, b, gray; 
			r = g = b = gray = 0;
			int n = ccx * sy;

			ImageCoverageSection q[9];

			for (int k = i; k < i + sy; k++) {
				for (int l = j; l < j + ccx * 3; l += 3) {
					int pos = k * w + l;

					int y = k - i;
					int x = (l - j) / 3;

					if (bmask) {
						if (RGB(img[pos], img[pos + 1], img[pos + 2]) == mask) {
							n--; 
							continue;					
						}
					}

					gray = (int)(((double)img[pos] * 0.2125) + ((double)img[pos + 1] * 0.7154) + ((double)img[pos + 2] * 0.0721));

					int x1, x2, y1, y2;
					x1 = (int)ceil((double)ccx * dx1) - 1;
					x2 = (int)floor((double)ccx * dx2) - 1;
					y1 = (int)floor((double)sy * dy1) - 1;
					y2 = (int)ceil((double)sy * dy2) - 1;

					if (y <= y1) {
						if (x <= x1)
							q[0].update(gray);
						else if (x > x1 && x <= x2)
							q[1].update(gray);
						else
							q[2].update(gray);
					}
					else if (y > y1 && y <= y2) {
						if (x <= x1)
							q[3].update(gray);
						else if (x > x1 && x <= x2)
							q[4].update(gray);
						else
							q[5].update(gray);
					}
					else {
						if (x <= x1)
							q[6].update(gray);
						else if (x > x1 && x <= x2)
							q[7].update(gray);
						else
							q[8].update(gray);
					}

					r += img[pos];
					g += img[pos + 1];
					b += img[pos + 2];
				}				
			}
		
			if (n == 0) {
				r = 255;
				g = 255;
				b = 255;
			}
			else {
				r /= n;
				g /= n;
				b /= n;
			}

			gray = (int)(((double)r * 0.2125) + ((double)g * 0.7154) + ((double)b * 0.0721));

			GlyphInfo best = getBestFittingChar(q, gray, glyphs_by_width[rows[ri][rj]]);
			row.push_back(std::make_pair(best.c, RGB(r, g, b)));
			
			/*
			int ind = (int)(((double)gray / 255.0) * (shades.size() - 1));
			row.push_back(std::make_pair(shades[ind], RGB(r, g, b)));*/
		}
		ret.push_back(row);
	}
	
	return ret;
}

vector<vector<pair<wchar_t, COLORREF>>> convert_image_to_ascii(HWND txtbox, BYTE* img, int iw, int ih, int sx, int sy, const wstring &shades, bool bmask, COLORREF mask, double gamma_correction, double brightness, double contrast) {
	vector<vector<pair<wchar_t, COLORREF>>> ret;

	if (!img) return ret;

	Mat frame = Mat(ih, iw, CV_8UC3, img, 0).clone();
	cvtColor(frame, frame, COLOR_RGB2BGR);

	resizeWindow("pre processing", 300, 400);
	resizeWindow("post processing", 300, 400);

	imshow("pre processing", frame);

	applyFilters("conout.bmp", img, iw, ih, gamma_correction, contrast, brightness);
	
	frame = Mat(ih, iw, CV_8UC3, img, 0).clone();
	cvtColor(frame, frame, COLOR_RGB2BGR);

	imshow("post processing", frame);

	vector<GlyphInfo> glyphs = getFontGlyphInfo(txtbox, shades);
	if (!glyphs.size()) return ret;

	sx = iw * sy / ih;

	int num_rows = ih / sy;
	int ph = num_rows * glyphs[0].h;
	int pw = ph * iw / ih;

	if (sx == 0 || sy == 0) {
		sx = sx == 0 ? 1 : sx;
		sy = sy == 0 ? 2 : sy;
	}

	int w = 3 * iw, h = ih;
	for (int i = 0, ri = 0; i <= h - sy; i += sy, ri++) {
		vector<pair<wchar_t, COLORREF>> row;
		int rj = 0;

		int ccx = 0;

		for (int j = 0; j <= w - ccx * 3; j += ccx * 3, rj++) {
			int r, g, b, gray;
			r = g = b = gray = 0;
			int n = ccx * sy;

			ImageCoverageSection q[9];

			for (int k = i; k < i + sy; k++) {
				for (int l = j; l < j + ccx * 3; l += 3) {
					int pos = k * w + l;

					int y = k - i;
					int x = (l - j) / 3;

					if (bmask) {
						if (RGB(img[pos], img[pos + 1], img[pos + 2]) == mask) {
							n--;
							continue;
						}
					}

					gray = (int)(((double)img[pos] * 0.2125) + ((double)img[pos + 1] * 0.7154) + ((double)img[pos + 2] * 0.0721));

					int x1, x2, y1, y2;
					x1 = (int)ceil((double)ccx * dx1) - 1;
					x2 = (int)floor((double)ccx * dx2) - 1;
					y1 = (int)floor((double)sy * dy1) - 1;
					y2 = (int)ceil((double)sy * dy2) - 1;

					if (y <= y1) {
						if (x <= x1)
							q[0].update(gray);
						else if (x > x1 && x <= x2)
							q[1].update(gray);
						else
							q[2].update(gray);
					}
					else if (y > y1 && y <= y2) {
						if (x <= x1)
							q[3].update(gray);
						else if (x > x1 && x <= x2)
							q[4].update(gray);
						else
							q[5].update(gray);
					}
					else {
						if (x <= x1)
							q[6].update(gray);
						else if (x > x1 && x <= x2)
							q[7].update(gray);
						else
							q[8].update(gray);
					}

					r += img[pos];
					g += img[pos + 1];
					b += img[pos + 2];
				}
			}

			if (n == 0) {
				r = 255;
				g = 255;
				b = 255;
			}
			else {
				r /= n;
				g /= n;
				b /= n;
			}

			/*for (int qi = 0; qi < 9; qi++) {
				q[qi].d = pow(q[qi].d, 1.0 + (avg - .5));
			}
*/
			gray = (int)(((double)r * 0.2125) + ((double)g * 0.7154) + ((double)b * 0.0721));

			GlyphInfo best = getBestFittingChar(q, gray, glyphs);
			ccx = (best.w * iw / (pw));

			row.push_back(std::make_pair(best.c, RGB(r, g, b)));

//			row.push_back(std::make_pair(shades[(int)(((double)gray / 255.0) * (shades.size() - 1))], RGB(r, g, b)));
		}
		ret.push_back(row);
	}

	return ret;
}

vector<vector<pair<wchar_t, COLORREF>>> convert_image_to_ascii(HWND txtbox, const string &filename, int sx, int sy, const wstring &shades, bool bmask, COLORREF mask, double gamma_correction, double brightness, double contrast) {
	int iw, ih;
	BYTE* img = bmp_to_array(filename, iw, ih);

	return convert_image_to_ascii(txtbox, img, iw, ih, sx, sy, shades, bmask, mask, gamma_correction, brightness, contrast);
}

wstring get_ascii_as_string(const vector<vector<pair<wchar_t, COLORREF>>> &img) {
	wstring ret(L"");

	for (int i = 0; i < img.size(); i++) {
		for (int j = 0; j < img[i].size(); j++) {			
			ret += img[i][j].first;
		}
		ret += '\n';
	}
	ret += '\n';

	return ret;
}

void print_art(const vector<vector<pair<wchar_t, COLORREF>>>& art, HWND hwnd, bool color) {
	if (hwnd != NULL) {
		wstring ws = get_ascii_as_string(art);

		SetWindowText(hwnd, ws.c_str());

		CustomEditControl* a = (CustomEditControl*)GetWindowLongPtr(hwnd, 0);
		if (!a)
			return;

		if (color) {
			a->is_color = true;
			int offset = 0;
			for (int i = 0; i < art.size(); i++) {
				for (int j = 0; j < art[i].size(); j++) {
					if (a->text[i * art[i].size() + j + offset] == '\n')
						offset++;
					a->text_colors[i * art[i].size() + j + offset] = art[i][j].second;
				}
			}
		}
		else
			a->is_color = false;
	}
}

void save_art(string output_filename, const vector<vector<pair<wchar_t, COLORREF>>>& art, bool color) {
	FILE* out = fopen(output_filename.c_str(), "w+,ccs=UTF-8");
	
	fwprintf(out, LR"(
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="utf-8" />
	<title></title>
	<style>
		.text{
			font-family: Consolas,monaco,monospace; 
			font-size: 4px;
			font-weight: bold;
		}
	</style>
</head>
<body>	
	<div class="text">
)");

	for (int i = 0; i < art.size(); i++) {
		for (int j = 0; j < art[i].size(); j++) {
			if (art[i][j].first == ' ') {
				fwprintf(out, L"&nbsp;");
				continue;
			}

			if (color) {
				fwprintf(out, L"<span style=\"color:rgb(");
				fwprintf(out, L"%d, ", (int)GetRValue(art[i][j].second));
				fwprintf(out, L"%d, ", (int)GetGValue(art[i][j].second));
				fwprintf(out, L"%d", (int)GetBValue(art[i][j].second));
				fwprintf(out, L")\">");
			}

			fwrite(&art[i][j].first, sizeof(wchar_t), 1, out);
			
			if (color)
				fwprintf(out, L"</span>");
		}
		fwprintf(out, L"<br>\n");
	}
	fwprintf(out, L"<br>\n");

	fwprintf(out, LR"(
	</div>
</body>
</html>)");

	fclose(out);
}

Options_info* getInfo(HWND hwnd) {
	Options_info* info = new Options_info;

	info->result = 0;
	info->output_filename = getwindowtext(GetDlgItem(hwnd, IDC_EDC_OUTPUTFILE));
	info->mask = IsDlgButtonChecked(hwnd, IDC_CB_MASK);
	info->color = IsDlgButtonChecked(hwnd, IDC_CB_COLOR);

	//get RGB mask
	string sr, sg, sb;
	sr = getwindowtext(GetDlgItem(hwnd, IDC_EDC_MASK_R));
	sg = getwindowtext(GetDlgItem(hwnd, IDC_EDC_MASK_G));
	sb = getwindowtext(GetDlgItem(hwnd, IDC_EDC_MASK_B));
	if (info->mask && (!sr.size() || !sg.size() || !sb.size())) {
		MessageBox(NULL, L"Must fill out mask colors correctly (0 - 255)", L"error", MB_OK);
		return 0;
	}
	int r, g, b;
	r = str_to_int(sr);
	g = str_to_int(sg);
	b = str_to_int(sb);
	int max3 = max(max(r, g), b);
	int min3 = min(min(r, b), b);
	if (info->mask && (max3 > 255 || min3 < 0)) {
		MessageBox(NULL, L"Must fill out mask colors correctly (0 - 255)", L"error", MB_OK);
		return 0;
	}
	info->mask_color = RGB(r, g, b);

	//get x and y 
	string sx = getwindowtext(GetDlgItem(hwnd, IDC_EDC_IMGX));
	string sy = getwindowtext(GetDlgItem(hwnd, IDC_EDC_IMGY));
	if (!sx.size() || !sy.size()) {
		MessageBox(NULL, L"Enter x or y value", L"error", MB_OK);
		return 0;
	}
	int x = str_to_int(sx);
	int y = str_to_int(sy);
	if (x <= 2 || y <= 2) {
		MessageBox(NULL, L"Value must be greater than 2", L"error", MB_OK);
		return 0;
	}
	info->x = x;
	info->y = y;

	//character set (glyphs)
	wstring character_set = L"";
	int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_EDC_CHARACTER_SET)) + 1;
	wchar_t* text = new wchar_t[len];
	GetWindowText(GetDlgItem(hwnd, IDC_EDC_CHARACTER_SET), text, len);
	character_set = text;
	if (character_set.size() == 0) {
		character_set = L"X";
	}
	info->character_set = character_set;

	//open html
	info->open_html = IsDlgButtonChecked(hwnd, IDC_CB_OPENHTML);

	//gamma correction
	string sgamma_correction = getwindowtext(GetDlgItem(hwnd, IDC_EDC_GAMMA_CORRECT));
	double gamma_correction;
	gamma_correction = str_to_double(sgamma_correction);
	if (gamma_correction < 0) {
		MessageBox(NULL, L"Must fill out gamma correction correctly (0 or more)", L"error", MB_OK);
		return 0;
	}
	if (sgamma_correction.size() == 0)
		gamma_correction = 0;
	info->gamma_correction = gamma_correction;

	//brightness
	string sbrightness = getwindowtext(GetDlgItem(hwnd, IDC_EDC_BRIGHTNESS));
	double brightness;
	brightness = str_to_double(sbrightness);
	if (brightness < 0) {
		MessageBox(NULL, L"Must fill out brightness correctly (0 or more)", L"error", MB_OK);
		return 0;
	}
	if (sbrightness.size() == 0)
		brightness = 0;
	info->brightness = brightness;

	//contrast
	string scontrast = getwindowtext(GetDlgItem(hwnd, IDC_EDC_CONTRAST));
	double contrast;
	contrast = str_to_double(scontrast);
	if (contrast < -255 || contrast > 255) {
		MessageBox(NULL, L"Must fill out contrast correctly (-255 to 255)", L"error", MB_OK);
		return 0;
	}
	if (!scontrast.size())
		contrast = 0;
	info->contrast = contrast;

	g_corner_weight = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CORNER)));
	g_center_weight = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CENTER)));
	g_edge_weight = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_EDGE)));
	g_brightness_weight = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_BRIGHTNESS)));

	return info;
}

void openOptionsDialog(HWND parent) {
	//Options_info* info = (Options_info*)DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_OPTIONS), GetParent(txtbox), OptionsProc, NULL);

	HWND optionsdlg = FindWindow(NULL, L"Ascii Art Options");
	if (optionsdlg == NULL)
		optionsdlg = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_OPTIONS), parent, OptionsProc, NULL);

	SendMessage(optionsdlg, WM_COMMAND, IDC_BTN_OK, NULL);
}

void file_to_ascii(string filepath, HWND txtbox) {
	string extension = filepath.substr(filepath.rfind(".") + 1, filepath.size());

	transform(extension.begin(), extension.end(), extension.begin(), toupper);

	if (extension != "BMP") {
		filepath = img_convert(filepath, filepath, BMP);

		if (filepath.size() == 0) {
			MessageBox(NULL, STW("invalid file " + filepath), L"error", MB_OK);
			return;
		}
	}

	g_last_converted_image_filename = filepath;	

	openOptionsDialog(GetParent(txtbox));
}

BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID lpv) {
	vector<font_info>* lfs = static_cast<vector<font_info>*>(lpv);

	LOGFONT lf = *((LPLOGFONT)lplf);
	NEWTEXTMETRIC ntm = *((LPNEWTEXTMETRIC)lpntm);

	font_info fi;
	fi.fontType = FontType;
	fi.lf = lf;
	fi.ntm = ntm;

	lfs->push_back(fi);

	return TRUE;
}

LPARAM ListView_GetItemParam(HWND hList, int item) {
	LVITEM lvi = {0};
	lvi.iItem = item;
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	ListView_GetItem(hList, &lvi);
	return lvi.lParam;
}
int ListView_GetCurSel(HWND hList) {
	return ListView_GetNextItem(hList, -1, LVNI_SELECTED);
}
int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	font_info* fi1 = (font_info*)lParam1;
	font_info* fi2 = (font_info*)lParam2;
	
	wstring w1(fi1->lf.lfFaceName);
	wstring w2(fi2->lf.lfFaceName);

	transform(w1.begin(), w1.end(), w1.begin(), tolower);
	transform(w2.begin(), w2.end(), w2.begin(), tolower);

	return w1 > w2;
}
LRESULT DrawFontListView(HWND listbox, LPARAM lParam) {
	NMLVCUSTOMDRAW* lvcd = (NMLVCUSTOMDRAW*)lParam;
	static bool ishighlighted = false;
	static HFONT oldfont = NULL, font = NULL;


	switch (lvcd->nmcd.dwDrawStage) {
		case CDDS_PREPAINT: {
			return CDRF_NOTIFYITEMDRAW;
		}
		case CDDS_ITEMPREPAINT: {
			ishighlighted = ListView_GetItemState(listbox, lvcd->nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED;

			if (ishighlighted) {
				ListView_SetItemState(listbox, lvcd->nmcd.dwItemSpec, 0, LVIS_SELECTED);
			}

			return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		}
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:{
			font_info* fi = (font_info*)lvcd->nmcd.lItemlParam;
			if (!fi) break;

			font = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, fi->lf.lfFaceName);
			oldfont = (HFONT)SelectObject(lvcd->nmcd.hdc, font);

			lvcd->clrText = RGB(0, 0, 0);
			lvcd->clrTextBk = ishighlighted ? RGB(128, 0, 0) : RGB(255,255,255);

			return CDRF_NOTIFYPOSTPAINT;
		}
		case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:{
			DeleteObject(SelectObject(lvcd->nmcd.hdc, oldfont));
			/*if (trade::is_mytradefile()) {
				ItemData* itemdata = (ItemData*)lvcd->nmcd.lItemlParam;
				if (!itemdata) break;

				if (lvcd->iSubItem == 0 && itemdata->is_trade && listbox == GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS)) {
					RECT itemrc;
					ListView_GetSubItemRect(listbox, lvcd->nmcd.dwItemSpec, 1, LVIR_BOUNDS, &itemrc);
					itemrc.right = itemrc.left - 1;
					itemrc.left = 2;
					HBRUSH framebrush = CreateSolidBrush(RGB(255, 0, 255));
					FrameRect(lvcd->nmcd.hdc, &itemrc, framebrush);
					DeleteObject(framebrush);
				}
			}*/
			return CDRF_DODEFAULT;
		}
		case CDDS_ITEMPOSTPAINT:{
			if (ishighlighted)
				ListView_SetItemState(listbox, lvcd->nmcd.dwItemSpec, LVIS_SELECTED, LVIS_SELECTED);

			return CDRF_DODEFAULT;
		}
	}
	return CDRF_DODEFAULT;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow) {
//	SHOW_CONSOLE(true, false);

	namedWindow("pre processing", WINDOW_NORMAL);
	namedWindow("post processing", WINDOW_NORMAL);

	HHOOK kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)kbHookProc, NULL, NULL);

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, 0);

	destroyAllWindows();

	UnhookWindowsHookEx(kbhook);

	return 0;
}
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			g_hwnd = hwnd;

			InitCommonControls();

			HWND textbox = CreateWindow(custom_edit_control_classname, L"",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_HSCROLL,
				0, 0, 10, 10, hwnd, (HMENU)IDC_CUSTOM_EDIT, NULL, (LPVOID)&edc);

			//HFONT font = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Chiller");
			HFONT font = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Consolas");
			SetWindowFont(textbox, font, false);

			RECT rc;
			GetClientRect(hwnd, &rc);
			SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));

			file_to_ascii("C:\\Users\\Josh\\Desktop\\test images\\" + g_test_filename, textbox);
			
			break;
		}
		case WM_DROPFILES: {
			wchar_t text[MAX_PATH];
			DragQueryFile((HDROP)wParam, 0, text, MAX_PATH);
			string filepath = wstr_to_str(text);

			if (filepath.find(".mp4") != string::npos) {
				HWND options = FindWindow(NULL, L"Ascii Art Options");

				if (options) {
					VideoCapture capture(filepath);
					Mat frame;

					double fps = capture.get(CV_CAP_PROP_FPS);

					if (!capture.isOpened())
						cout << "Error when reading steam_avi\n";
					else {
						for (;;) {
							Options_info* info = getInfo(options);

							if (info) {
								capture >> frame;
								if (frame.empty())
									break;

								HWND txtbox = GetDlgItem(hwnd, IDC_CUSTOM_EDIT);

								vector<vector<pair<wchar_t, COLORREF>>> art = convert_image_to_ascii(txtbox, frame.data, frame.cols, frame.rows,
									info->x, info->y, info->character_set, info->mask, info->mask_color, info->gamma_correction, info->brightness, info->contrast);

								print_art(art, txtbox, info->color);
							}

							waitKey(1); // waits to display frame
						}

						capture.release();
					}
				}
			}

			else
				file_to_ascii(filepath, GetDlgItem(hwnd, IDC_CUSTOM_EDIT));

			HWND optionsdlg = FindWindow(NULL, L"Ascii Art Options");
			if (optionsdlg)
				SetWindowText(GetDlgItem(optionsdlg, IDC_STATIC_OPENFILENAME), STW(g_last_converted_image_filename));

			break;
		}
		case WM_SIZE: {
			SetWindowPos(GetDlgItem(hwnd, IDC_CUSTOM_EDIT), NULL, 0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), NULL);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			DestroyWindow(hwnd);
			g_hwnd = NULL;
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK OptionsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			g_options = hwnd;

			SetWindowText(GetDlgItem(hwnd, IDC_STATIC_OPENFILENAME), STW(g_last_converted_image_filename));

			SetWindowText(GetDlgItem(hwnd, IDC_EDC_IMGX), L"12");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_IMGY), L"8");

			SetWindowText(GetDlgItem(hwnd, IDC_EDC_CONTRAST), L"0");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_BRIGHTNESS), L"100");
			SetWindowSubclass(GetDlgItem(hwnd, IDC_EDC_CONTRAST), ContrastEdcProc, 0, 0);

			SetWindowText(GetDlgItem(hwnd, IDC_EDC_MASK_R), L"255");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_MASK_G), L"255");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_MASK_B), L"255");

			SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CORNER), L"10");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CENTER), L"48");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_EDGE), L"10");
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_BRIGHTNESS), L"1000");

			HWND idc_edc_outputfile = GetDlgItem(hwnd, IDC_EDC_OUTPUTFILE);
			SetWindowText(idc_edc_outputfile, STW(getexedir() + "\\" + "output.html"));
			int outLength = GetWindowTextLength(idc_edc_outputfile);
			SendMessage(idc_edc_outputfile, EM_SETSEL, outLength, outLength);

			HWND txtbox = GetDlgItem(GetParent(hwnd), IDC_CUSTOM_EDIT);
			vector<GlyphInfo> glyphs = getFontGlyphInfo(txtbox);

			//	glyphs.erase(remove_if(glyphs.begin(), glyphs.end(), [](GlyphInfo a) { return ((unsigned int)a.c) > 127; }), glyphs.end());

			wstring wtxt = getDistributedShadingCharacters(glyphs, 300);
			wstring def = L"@%#Oo*^+=-;:.'\" ";
			wstring nfixed = L"XAVO\":,'.` ";
			wstring noalphanum = L"";
			wstring online = L"@#+:;',. ";

			for (int i = glyphs.size() - 1; i >= 0; i--) {
				wchar_t t = glyphs[i].c;
				if ((t < 'a' || t > 'z') && (t < 'A' || t > 'Z') && (t < '0' || t > '9') && t != '\0' && t != 0)
					noalphanum.push_back(t);
			}

			SetWindowText(GetDlgItem(hwnd, IDC_EDC_CHARACTER_SET), def.c_str());

			SendMessage(GetDlgItem(hwnd, IDC_SPIN_CORNER_WEIGHT), UDM_SETBUDDY, (WPARAM)GetDlgItem(hwnd, IDC_EDC_WEIGHT_CORNER), 0);
			SendMessage(GetDlgItem(hwnd, IDC_SPIN_CORNER_WEIGHT), UDM_SETRANGE, 0, MAKELPARAM(100, 0));

			SendMessage(GetDlgItem(hwnd, IDC_SPIN_CENTER_WEIGHT), UDM_SETBUDDY, (WPARAM)GetDlgItem(hwnd, IDC_EDC_WEIGHT_CENTER), 0);
			SendMessage(GetDlgItem(hwnd, IDC_SPIN_CENTER_WEIGHT), UDM_SETRANGE, 0, MAKELPARAM(100, 0));

			SendMessage(GetDlgItem(hwnd, IDC_SPIN_EDGE_WEIGHT), UDM_SETBUDDY, (WPARAM)GetDlgItem(hwnd, IDC_EDC_WEIGHT_EDGE), 0);
			SendMessage(GetDlgItem(hwnd, IDC_SPIN_EDGE_WEIGHT), UDM_SETRANGE, 0, MAKELPARAM(100, 0));

			SendMessage(GetDlgItem(hwnd, IDC_SPIN_BRIGHTNESS_WEIGHT), UDM_SETBUDDY, (WPARAM)GetDlgItem(hwnd, IDC_EDC_WEIGHT_BRIGHTNESS), 0);
			SendMessage(GetDlgItem(hwnd, IDC_SPIN_BRIGHTNESS_WEIGHT), UDM_SETRANGE, 0, MAKELPARAM(100, 0));

			break;
		}
		case WM_COMMAND: {
			switch (wParam) {
				case IDC_BTN_OK: {
					Options_info* info = getInfo(hwnd);

					HWND txtbox = GetDlgItem(GetParent(hwnd), IDC_CUSTOM_EDIT);
					
					vector<vector<pair<wchar_t, COLORREF>>> art = convert_image_to_ascii(txtbox, g_last_converted_image_filename,
						info->x, info->y, info->character_set, info->mask, info->mask_color, info->gamma_correction, info->brightness, info->contrast);

					print_art(art, txtbox, info->color);
					
					break;
				}
				case IDC_CB_MASK: {
					bool active = IsDlgButtonChecked(hwnd, IDC_CB_MASK);
					EnableWindow(GetDlgItem(hwnd, IDC_EDC_MASK_R), active);
					EnableWindow(GetDlgItem(hwnd, IDC_EDC_MASK_G), active);
					EnableWindow(GetDlgItem(hwnd, IDC_EDC_MASK_B), active);

					break;
				}
				case IDC_BTN_CHANGE_FONT: {
					HFONT newfont = (HFONT)DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLG_FONT), hwnd, (DLGPROC)ChangeFontProc, NULL);

					if (newfont)
						SetWindowFont(GetDlgItem(GetParent(hwnd), IDC_CUSTOM_EDIT), newfont, TRUE);

					break;
				}
				case IDC_BTN_CANCEL: {
					SendMessage(hwnd, WM_CLOSE, NULL, NULL);
					break;
				}
			}
			break;
		}
		case WM_NOTIFY:{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;

			switch (((LPNMHDR)lParam)->code) {
				case UDN_DELTAPOS: {
					LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;

					switch (lpnmud->hdr.idFrom) {
						case IDC_SPIN_CORNER_WEIGHT: {
							int val = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CORNER)));
							int newval = val + lpnmud->iDelta;
							newval = newval < 0 ? 0 : newval;
							SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CORNER), int_to_wstr(newval).c_str());

						//	SendMessage(hwnd, WM_COMMAND, IDC_BTN_OK, NULL);

							break;
						}
						case IDC_SPIN_CENTER_WEIGHT: {
							int val = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CENTER)));
							int newval = val + lpnmud->iDelta;
							newval = newval < 0 ? 0 : newval;
							SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_CENTER), int_to_wstr(newval).c_str());

						//	SendMessage(hwnd, WM_COMMAND, IDC_BTN_OK, NULL);

							break;
						}
						case IDC_SPIN_EDGE_WEIGHT: {
							int val = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_EDGE)));
							int newval = val + lpnmud->iDelta;
							newval = newval < 0 ? 0 : newval;
							SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_EDGE), int_to_wstr(newval).c_str());

						//	SendMessage(hwnd, WM_COMMAND, IDC_BTN_OK, NULL);

							break;
						}
						case IDC_SPIN_BRIGHTNESS_WEIGHT: {
							int val = wstr_to_int(getwindowtextw(GetDlgItem(hwnd, IDC_EDC_WEIGHT_BRIGHTNESS)));
							int newval = val + lpnmud->iDelta;
							newval = newval < 0 ? 0 : newval;
							SetWindowText(GetDlgItem(hwnd, IDC_EDC_WEIGHT_BRIGHTNESS), int_to_wstr(newval).c_str());

						//	SendMessage(hwnd, WM_COMMAND, IDC_BTN_OK, NULL);

							break;
						}
					}

					break;
				}
			}
			break;
		}
		case WM_DESTROY:
		case WM_CLOSE: {
			Options_info* info = new Options_info;
			info->result = 1;
			EndDialog(hwnd, (INT_PTR)info);
			DestroyWindow(hwnd);
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK ChangeFontProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static vector<font_info> lfs;

	switch (message) {
		case WM_INITDIALOG: {
			HDC hdc = GetDC(hwnd);	
			lfs.clear();
			EnumFontFamilies(hdc, (LPCTSTR)NULL, (FONTENUMPROC)EnumFamCallBack, (LPARAM)&lfs);
			ReleaseDC(hwnd, hdc);

			HWND list = GetDlgItem(hwnd, IDC_LIST_FONTS);

			SendMessage(list, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

			LVCOLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH;
			lvc.cx = getclientrect(list).right - 20;
			lvc.pszText = L"Fonts";
			ListView_InsertColumn(list, 0, &lvc);

			LVITEM lvi = {0};
			lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
			lvi.stateMask = LVIS_SELECTED;
			lvi.iSubItem = 0;
			lvi.state = 0;

			for (int i = 0; i < lfs.size(); i++) {
				lvi.pszText = lfs[i].lf.lfFaceName;
				lvi.lParam = (LPARAM)&lfs[i];
			
				ListView_InsertItem(list, (LPARAM)&lvi);
			}
			
			SetFocus(list);		

			ListView_SortItems(list, ListViewCompareProc, 0);

			HFONT font = (HFONT)SendMessage(GetDlgItem(GetParent(GetParent(hwnd)), IDC_CUSTOM_EDIT), WM_GETFONT, 0, 0);
			LOGFONT lf = {0};
			GetObject(font, sizeof(lf), &lf);

			LVFINDINFO lvfi;
			lvfi.flags = LVFI_STRING;
			lvfi.psz = lf.lfFaceName;
			int pos = ListView_FindItem(list, 0, &lvfi);

			ListView_SetItemState(list, pos < 0 ? 0 : pos, LVIS_SELECTED, LVIS_SELECTED);

			break;
		}
		case WM_COMMAND: {
			switch (wParam) {
				case IDOK:{
					HWND list = GetDlgItem(hwnd, IDC_LIST_FONTS);
					
					font_info* fi = (font_info*)ListView_GetItemParam(list, ListView_GetCurSel(list));

					if (!fi) {
						EndDialog(hwnd, (INT_PTR)0);
						break;
					}

					LOGFONT lf = fi->lf;
					HFONT tempfont = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lf.lfFaceName);
					
					EndDialog(hwnd, (INT_PTR)tempfont);

					break;
				}
				case IDCANCEL: {
					SendMessage(hwnd, WM_CLOSE, NULL, NULL);
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_LIST_FONTS:{
					switch (lpnmh->code) {						
						case NM_CUSTOMDRAW:{
							SetWindowLong(hwnd, DWL_MSGRESULT, DrawFontListView(lpnmh->hwndFrom, lParam));
							return TRUE;
						}
						case LVN_ITEMCHANGED:{
							NMLISTVIEW *VAL_notify = (NMLISTVIEW*)lParam;
							if (VAL_notify->uNewState & LVIS_SELECTED) {
								HWND selected_fontname = GetDlgItem(hwnd, IDC_EDC_SELECTED_FONTNAME);
								HWND font_sample = GetDlgItem(hwnd, IDC_EDC_FONT_SAMPLE);
								HWND list = GetDlgItem(hwnd, IDC_LIST_FONTS);

								font_info* fi = (font_info*)ListView_GetItemParam(list, ListView_GetCurSel(list));

								SetWindowText(selected_fontname, fi->lf.lfFaceName);
								SetWindowText(font_sample, L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\nabcdefghijklmnopqrstuvwxyz\r\n0123456789!@#$%^&*()-_=+;:\'\",<.>/?\\");
						
								HFONT font = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, fi->lf.lfFaceName);	
								SetWindowFont(font_sample, font, TRUE);
							}
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_DESTROY:
		case WM_CLOSE: {
			EndDialog(hwnd, (INT_PTR)0);
			break;
		}
	}

	return FALSE;
}
LRESULT CALLBACK ContrastEdcProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (uMsg) {
		case WM_CHAR: {
			/*wchar_t nc = (wchar_t)wParam;

			if (true);
			else if (nc < 32 || nc > 127) {
				return TRUE;
			}

			wstring txt = getwindowtextw(hwnd) + nc;

			wstring ntxt = L"";

			for (int i = 0; i < txt.size(); i++) {
				if (txt[i] >= 48 && txt[i] <= 57 || (txt[i] == '-' && i == 0))
					ntxt += txt[i];
			}

			SetWindowText(hwnd, ntxt.c_str());
			SendMessage(hwnd, EM_SETSEL, ntxt.size(), ntxt.size());

			return TRUE;*/
		}
	}
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK kbHookProc(int code, WPARAM wParam, LPARAM lParam) {
	static bool is_cap = false;

  	if (code < 1) {
		LPKBDLLHOOKSTRUCT key = (LPKBDLLHOOKSTRUCT)lParam;

		HWND hwnd = GetActiveWindow();

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			if (key->vkCode == VK_UP) {
			//	file_to_ascii(g_last_converted_image_filename, GetDlgItem(hwnd, IDC_CUSTOM_EDIT));
				is_cap = false;
			}
			if (key->vkCode == VK_DOWN) {
				if (!g_options) {
					openOptionsDialog(g_hwnd);
				}

				HWND txtbox = GetDlgItem(g_hwnd, IDC_CUSTOM_EDIT);

				if (g_options) {
					VideoCapture cap(0);
					if (!cap.isOpened())
						cout << "Cam failed\n";
					else {
						is_cap = true;

						while (is_cap && g_hwnd && cap.isOpened()) {
							Options_info* info = getInfo(g_options);

							if (info) {
								Mat frame;
								cap >> frame;	

								cvtColor(frame, frame, COLOR_RGB2BGR);

								vector<vector<pair<wchar_t, COLORREF>>> art = convert_image_to_ascii(txtbox, frame.data, frame.cols, frame.rows,
									info->x, info->y, info->character_set, info->mask, info->mask_color, info->gamma_correction, info->brightness, info->contrast);

								print_art(art, txtbox, info->color);
							}

							waitKey(1);
						}
					}
				}
			}
		}
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

