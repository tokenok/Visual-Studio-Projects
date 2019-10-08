#pragma once

#include <iostream>
#include <time.h>
#include <Windows.h>

class Benchmark {
public:
	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER elapsed;

	LARGE_INTEGER Frequency;

	Benchmark();

	void start();

	LONGLONG end();
};