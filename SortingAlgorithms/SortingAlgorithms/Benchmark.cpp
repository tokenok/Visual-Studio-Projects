#include "Benchmark.h"

Benchmark::Benchmark() {
	QueryPerformanceFrequency(&Frequency);
	start();
}

void Benchmark::start() {
	QueryPerformanceCounter(&start_time);
}

LONGLONG Benchmark::end() {
	QueryPerformanceCounter(&end_time);
	elapsed.QuadPart = end_time.QuadPart - start_time.QuadPart;

	elapsed.QuadPart *= 1000000;
	elapsed.QuadPart /= Frequency.QuadPart;

	return elapsed.QuadPart;
}