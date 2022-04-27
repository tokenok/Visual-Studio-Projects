#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include "visualizer.h"
#include "sorting.h"
#include "timer.h"

enum class ChartType {
	Pie, Graph, Line
};

template<class T>
struct Point {
	T x, y;
};

template<class T>
class Data {
public:
	void AddPoint(Point<T> point) {
		points.push_back(point);
	}

	size_t GetNumPoints() {
		return points.size();
	}

	Point<T> GetPoint(size_t i) {
		return points[i];
	}

private:
	std::vector<Point<T>> points;
};

template<class T>
class Chart {
public:
	Chart() { }

	void AddDataPoints(size_t data_i, Data<T>& data) {
		size_t numPoints = data.GetNumPoints();
		for (unsigned i = 0; i < numPoints; ++i) {
			AddDataPoint(data_i, data.GetPoint(i));
		}
	}
	void AddDataPoints(Data<T>& data) {
		AddDataPoints(0, data);
	}

	void AddDataPoints(size_t data_i, const std::vector<Point<T>>& points) {
		for (unsigned i = 0; i < points.size(); ++i)
			AddDataPoint(data_i, points[i]);
	}
	void AddDataPoints(const std::vector<Point<T>>& points) {
		AddDataPoints(0, points);
	}

	void AddDataPoint(const Point<T>& dataPoint) {
		AddDataPoint(0, dataPoint);
	}

	void AddDataPoint(size_t data_i, const Point<T>& dataPoint) {
		add_data_point(data_i, dataPoint);
	}

	static Chart<T>* getChart(HWND hwnd);

	HWND GetHWND() { return hwnd; }

private:
	inline void add_data_point(size_t data_i, const Point<T>& dataPoint) {
		if (dataPoint.x < x_range.min) x_range.min = dataPoint.x;
		else if (dataPoint.x > x_range.max) x_range.max = dataPoint.x;

		if (dataPoint.y < y_range.min) y_range.min = dataPoint.y;
		else if (dataPoint.y > y_range.max) y_range.max = dataPoint.y;

		dataPoints[data_i].AddPoint(dataPoint);
	}

	ChartType type;

	std::wstring title, label_x_axis, label_y_axis;

	Range<T> x_range, y_range;

	std::vector<Data<T>> dataPoints;

	HWND hwnd;

	static LRESULT CALLBACK Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void DrawScreen(Chart<T>* chart);

	friend class Visualizer;
};

template<class T>
Chart<T>* Chart<T>::getChart(HWND hwnd) {
	return (Chart<T>*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

template<class T>
void Chart<T>::DrawScreen(Chart<T>* chart) {
	HWND hwnd = chart->GetHWND();

	BenchmarkTimer bm;
	bm.start();

	RECT rcclient;
	GetClientRect(hwnd, &rcclient);

	if (!rcclient.right || !rcclient.bottom) return;

	HDC hdc = GetDC(hwnd);

	HDC hdcmem = CreateCompatibleDC(hdc);

	HBITMAP hbmScreen, hbmOldBitmap;
	hbmScreen = CreateCompatibleBitmap(hdc, rcclient.right, rcclient.bottom);
	hbmOldBitmap = (HBITMAP)SelectObject(hdcmem, hbmScreen);

	//////////////////////////////////////////////////////////////////////////////////////


	FillRect(hdcmem, &rcclient, (HBRUSH)GetStockObject(WHITE_BRUSH));


	//////////////////////////////////////////////////////////////////////////////////////

	//draw the screen
	BitBlt(hdc, 0, 0, rcclient.right, rcclient.bottom, hdcmem, 0, 0, SRCCOPY);

	//cleanup
	SelectObject(hdcmem, hbmOldBitmap);
	DeleteObject(hbmScreen);

	DeleteDC(hdcmem);
	ReleaseDC(hwnd, hdc);

	bm.stop();
}

template<class T>
LRESULT CALLBACK Chart<T>::Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Chart<T>* chart = Chart<T>::getChart(hwnd);

	switch (message) {
		case WM_NCCREATE: {
			chart = (Chart<T>*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			chart->hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)chart);

			break;
		}
		case WM_PAINT: {
			DrawScreen(chart);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			/*if (chart) {
				delete chart;
				chart = 0;
			}*/
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

















