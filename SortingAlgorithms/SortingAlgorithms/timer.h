#pragma once

#include <windows.h>
#include <iostream>

class BenchmarkTimer {
public:

	BenchmarkTimer() {
		QueryPerformanceFrequency(&frequency);
	}

	LONGLONG start() {
		stopped = false;
		QueryPerformanceCounter(&start_time);
		return start_time.QuadPart;
	}

	LONGLONG stop() {
		QueryPerformanceCounter(&end_time);
		if (stopped) start_time = end_time;
		else stopped = true;
		elapsed.QuadPart = end_time.QuadPart - start_time.QuadPart;
		elapsed.QuadPart *= 1000000;
		elapsed.QuadPart /= frequency.QuadPart;
		return elapsed.QuadPart;
	}

	void reset() {
		elapsed.QuadPart = 0;
		stopped = true;
		paused = false;
		stop();
	}

	void pause() {
		if (!stopped && !paused) {
			QueryPerformanceCounter(&paused_time);
		}
		paused = true;
	}

	void resume() {
		if (!stopped && paused) {
			QueryPerformanceCounter(&resume_time);
			start_time.QuadPart += (resume_time.QuadPart - paused_time.QuadPart);
		}
		paused_time = { 0 };
		paused = false;
	}

	LONGLONG getElapsed() { 
		if (!stopped) {
			stop();
			stopped = false;
		}
		
		return elapsed.QuadPart; 
	}

	double getElapsedMS() { return (double)getElapsed() / 1000.0; }

	friend std::ostream& operator<<(std::ostream& os, BenchmarkTimer& bm) {
		bm.stop();
		os << bm.elapsed.QuadPart;
		return os;
	}

private:
	bool stopped = true;
	bool paused = false;
	LARGE_INTEGER paused_time;
	LARGE_INTEGER resume_time;

	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER elapsed;

	LARGE_INTEGER frequency;
};

