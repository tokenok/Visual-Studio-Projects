#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <conio.h>
#include <ctime>
#include <functional>
#include <thread>

#include "sorting.h"
#include "resource.h"

#include "benchmark.h"
#include "trace.h"
#include "visualizer.h"
#include "chart.h"

#include "timsort.h"
#include "quicksort.h"
#include "mergesort.h"
#include "heapsort.h"
#include "softheapsort.h"
#include "weakheapsort.h"
#include "insertionsort.h"
#include "bubblesort.h"
#include "gnomesort.h"
#include "selectionsort.h"
#include "cyclesort.h"
#include "radixsort.h"
#include "pigeonholesort.h"
#include "smoothsort.h"
#include "wikisort.h"
#include "grailsort.h"
#include "binarytreesort.h"
#include "badsorts.h"
#include "vergesort.h"
#include "pdqsort.h"
#include "gravitysort.h"
#include "skasort.h"
#include "poplarsort.h"
#include "dropmergesort.h"
#include "countingsort.h"
#include "regionssort.h"
#include "patiencesort.h"
#include "tim\timsort.h"
#include "sqrtsort.h"
#include "neatsort.h"
#include "melsort.h"
#include "quickmergesort.h"
#include "mergeinsertionsort.h"
#define BOOST_INSTALLED true
#if BOOST_INSTALLED
#include "C:/boost_1_72_0/boost/sort/sort.hpp"
#endif

//TODO:
//quicksort variations
//soft heap sort fix
//weave merge complexity
//weak heap sort complexity
//shatter sort
//spread sort
//bogobogosort
//fix bogobogosort
//fix fast draw
//fix scatter plot (optimized (crush horz and vert))
//line plot
//fix comparison draw (not redrawing prev?)
//square plot
//region sort!!!!!!!!!!!!
//multi select options
//benchmark chart
//benchmark table??

#define SORTING_ALGORITHM(alg, alg_name, bc, ac, wc, sp, st, desc) { L#alg, {(TVSortAlgPtr)alg<tvit, tvcmp>, (VSortAlgPtr)alg<vit, vcmp>, alg_name, bc, ac, wc, sp, st, desc} }

#define TEST_SORT(alg, alg_name) { (VSortAlgPtr)alg<vit, vcmp>, alg_name }

int g_vis_id_counter = 0;
#define CREATE_VISUALIZER(sz, vis_type, col_mode, init_type_val, init_type_ord, draw_opts, alg) \
																						{Visualizer* visualizer = new Visualizer(hwnd, g_hInst, (HMENU)g_vis_id_counter++, \
																						sz, \
																						vis_type, \
																						col_mode, \
																						init_type_val, \
																						init_type_ord, \
																						DrawOption(draw_opts), \
																						SortingAlgorithms[L#alg]); \
																						Visualizer_HWNDS.push_back(visualizer->GetHWND());}

HWND g_hwnd;
HINSTANCE g_hInst;

std::vector<HWND> Visualizer_HWNDS;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void SHOW_CONSOLE(bool show = true, bool noclose = false) {
	static bool show_state = false;
	if (show && !show_state) {
		std::cout.clear();
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONOUT$", "w", stdout);
		if (noclose)
			EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}
	if (!show)
		FreeConsole();
	show_state = show;
}

struct Rect {
	int x, y, w, h;
	Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) { }
};
std::vector<Rect> calc_window_rects(unsigned num_wnd, const RECT& rcclient) {
	std::vector<Rect> ret;
	if (!num_wnd) return ret;	
	unsigned cols = num_wnd < 5 ? 1 : (int)log2(num_wnd - 3);
	unsigned rows = num_wnd < 5 ? num_wnd : (unsigned)ceil((double)num_wnd / (double)cols);
	double w = ((double)rcclient.right / (double)cols) - 4.0;
	double h = ((double)rcclient.bottom / (double)rows) - 4.0;
	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0, pos = row * cols + col; col < cols && (pos = row * cols + col) < num_wnd; col++) {
			unsigned x = 2 + (4 * col) + (unsigned)((double)col * w), y = 2 + (4 * row) + (unsigned)((double)row * h);
			Rect rect(x, y, (int)w, (int)h);
			ret.push_back(rect);
		}
	}
	return ret;
}

void Resize() {
	RECT rcclient;
	GetClientRect(g_hwnd, &rcclient);

	rcclient.right -= 80;

	SetWindowPos(GetDlgItem(g_hwnd, BTN_ADD), NULL, rcclient.right + 5, rcclient.bottom - 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(GetDlgItem(g_hwnd, BTN_REMOVE), NULL, rcclient.right + 5, rcclient.bottom - 70, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(GetDlgItem(g_hwnd, BTN_RUN_ALL), NULL, rcclient.right + 5, rcclient.bottom - 40, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	std::vector<Rect> r = calc_window_rects(Visualizer_HWNDS.size(), rcclient); 
	for (unsigned i = 0; i < r.size(); i++) {
		SetWindowPos(Visualizer_HWNDS[i], NULL, r[i].x, r[i].y, r[i].w, r[i].h, SWP_NOZORDER);
	}
}

//void Redraw() {
//	for (unsigned i = 0; i < Visualizer_HWNDS.size(); i++) {
//		Visualizer* vis = Visualizer::getVisualizer(Visualizer_HWNDS[i]);
//		vis->DrawScreen();
//	}
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
	g_hInst = hInstance;

	SHOW_CONSOLE();
	
	srand(clock());

	register_visualizer_control();
	register_customtrackbar();

	//std::string filepath = "C:\\Users\\Josh\\Documents\\Visual Studio 2017\\Projects\\SortingAlgorithms\\SortingAlgorithms";//pc
	//std::string filepath = "C:\\Users\\JO20026780\\source\\repos\\Sorting\\Sorting";//work
	std::string filepath = "C:\\Users\\Josh\\Documents\\Visual Studio 2017\\Projects\\Sorting\\Sorting";//laptop

#ifndef _DEBUG
	SaveSourceToFile(filepath);
	LoadSourceFromFile(filepath);
#endif

	InitCommonControls();

	DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)DialogProc);

	return 0;
}

template<class T, class C>
inline void rotater(T _First, T _Last, C compare) {
	while (true) {
		typename T::value_type val = *_First;
		T _Ptr = _First + 1;
		for (; _Ptr != _Last; ++_Ptr) {
			*(_Ptr - 1) = *_Ptr;
		}
		*(_Ptr - 1) = val;
	}
}

template <class RandomAccessIter, class Compare>
inline void integer_sort(RandomAccessIter first, RandomAccessIter last, Compare compare) {
  // Don't sort if it's too small to optimize.
	if (last - first < boost::sort::spreadsort::detail::min_sort_size)
		boost::sort::pdqsort(first, last);
	else
		boost::sort::spreadsort::detail::integer_sort(first, last, *first >> 0);
}

std::map<std::wstring, SortingAlgorithm> SortingAlgorithms = {
	///............|  alg_name					|				best case		|	average case		|	worst case			|	space		|	stable		|
//	SORTING_ALGORITHM(bozosort,									L"O(n)",			L"O(n)",				L"O(\u221E)",			L"O(1)",		false),
////	SORTING_ALGORITHM(bogobogosort,								L"O(n\u00B2)",		L"O(n*(n!)\u207F)",		L"O(\u221E)",			L"O(1)",		false),
//	SORTING_ALGORITHM(lexipermsort,								L"O(n)",			L"O(n!)",				L"O(n!)",				L"O(1)",		false),
//	SORTING_ALGORITHM(bogosort,									L"O(n)",			L"O((n + 1)!)",			L"O(\u221E)",			L"O(1)",		false),
//	SORTING_ALGORITHM(slowsort,									L"O(n^(log n))",	L"O(n^(log n))",		L"O(n^(log n))",		L"O(n)",		false),
//	SORTING_ALGORITHM(stoogesort,								L"O(n^2.7095)",		L"O(n^2.7095)",			L"O(n^2.7095)",			L"O(n)",		false),
//	SORTING_ALGORITHM(sillysort,								L"O(n^(log n))",	L"O(n^(log n))",		L"O(n^(log n))",		L"O(n)",		false), //this is the same things a slow sort, but with min vals instead of max
	SORTING_ALGORITHM(pancakesort,								L"Pancake Sort",										L"O(n\u00B2)",		L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				false,	L"Pancake sort"),
	SORTING_ALGORITHM(selectionsort,							L"Selection Sort",										L"O(n\u00B2)",		L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				false,	L"Select the largest value in the list, then swap it with the last unselected value in the list, repeat until last unselected value is the first element."),
	SORTING_ALGORITHM(doubleselectionsort,						L"Double Selection Sort",								L"O(n\u00B2)",		L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				false,	L"Select the smallest and largest value, then swap the smallest value to the front, and the biggest value to the end, repeat until unselected values meet in the center of the list."),
	SORTING_ALGORITHM(bubblesort,								L"Bubble Sort",											L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L"Check each adjacent pair in the list (left to right), swap them if the right element is greater than the left element, repeat until an entire pass is made without a swap. This Quickly moves large elements to the end of the list, but slowly moves small values to the front. These small values are sometimes called \"turtles\"."),
	SORTING_ALGORITHM(cocktailshakersort,						L"Cocktail Shaker Sort",								L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L"Bubble sort from left to right, then right to left, repeat until no swaps are made. This is an attempt to move bubble sorts small value \"turtles\" to the left as quickly as large values move right."),
	SORTING_ALGORITHM(evenoddsort,								L"Even Odd Sort",										L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(combsort,									L"Comb Sort",											L"O(n log n)",		L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				false,	L"Bubble sort, but instead of comparing adjacent elments it compares elements with increasingly smaller gaps until the gap size is 1. Then do passes of gap size 1 until no swaps are made."),
	SORTING_ALGORITHM(cyclesort,								L"Cycle Sort",											L"O(n\u00B2)",		L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				false,	L""),
	SORTING_ALGORITHM(gnomesort,								L"Gnome Sort",											L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(optimizedgnomesort,						L"Gnome Sort (Optimized)",								L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(insertionsort,							L"Insertion Sort",										L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(binaryinsertionsort,						L"Binary Insertion Sort",								L"O(n log n)",		L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(stdinsertionsort,							L"std::insertion_sort",									L"O(n)",			L"O(n\u00B2)",			L"O(n\u00B2)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(shellsort,								L"Shell Sort",											L"O(n log n)",		L"O(n log\u00B2 n)",	L"Unknown",				L"O(1)",				false,	L""),
	SORTING_ALGORITHM(weavemergesort,							L"Weave Merge Sort",									L"Unknown",			L"Unknown",				L"Unknown",				L"O(n)",				false,	L""),
	SORTING_ALGORITHM(mergesort,								L"Merge Sort",											L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
	SORTING_ALGORITHM(timsort,									L"Tim Sort",											L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
	SORTING_ALGORITHM(tim::timsort,								L"Tim Sort (Optimized)",								L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
	SORTING_ALGORITHM(heapsort,									L"Heap Sort (Max)",										L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				false,	L""),
	SORTING_ALGORITHM(minheapsort,								L"Heap Sort (Min)",										L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				false,	L""),
	SORTING_ALGORITHM(stdheapsort,								L"std::heap_sort",										L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				false,	L""),
	SORTING_ALGORITHM(smoothsort,								L"Smooth Sort",											L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(1)",				false,	L""),
	SORTING_ALGORITHM(weakheapsort,								L"Weak Heap Sort",										L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(n)",				false,	L""),
	SORTING_ALGORITHM(binarytreesort,							L"Binary Tree Sort",									L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
//	SORTING_ALGORITHM(softheapsort,								L"Soft Heap Sort",										L"Unknown",			L"Unknown",				L"Unknown",				L"Unknown",				false,	L""),//doesn't sort in place :(
	SORTING_ALGORITHM(radixsort_LSD_2,							L"Radix Sort (LSD Base 2)",								L"O(w * n)",		L"O(w * n)",			L"O(w * n)",			L"O(w + n)",			true,	L""),
	SORTING_ALGORITHM(radixsort_LSD_4,							L"Radix Sort (LSD Base 4)",								L"O(w * n)",		L"O(w * n)",			L"O(w * n)",			L"O(w + n)",			true,	L""),
	SORTING_ALGORITHM(radixsort_LSD_10,							L"Radix Sort (LSD Base 10)",							L"O(w * n)",		L"O(w * n)",			L"O(w * n)",			L"O(w + n)",			true,	L""),
	SORTING_ALGORITHM(radixsort_LSD_16,							L"Radix Sort (LSD Base 16)",							L"O(w * n)",		L"O(w * n)",			L"O(w * n)",			L"O(w + n)",			true,	L""),
	SORTING_ALGORITHM(radixsort_LSD_32,							L"Radix Sort (LSD Base 32)",							L"O(w * n)",		L"O(w * n)",			L"O(w * n)",			L"O(w + n)",			true,	L""),
	SORTING_ALGORITHM(inPlaceRadixLSDSortBase2,					L"In-Place Radix Sort (LSD Base 2)",					L"Unknown",			L"Unknown",				L"Unknown",				L"O(b)",				true,	L""),
	SORTING_ALGORITHM(inPlaceRadixLSDSortBase4,					L"In-Place Radix Sort (LSD Base 4)",					L"Unknown",			L"Unknown",				L"Unknown",				L"O(b)",				true,	L""),
	SORTING_ALGORITHM(inPlaceRadixLSDSortBase10,				L"In-Place Radix Sort (LSD Base 10)",					L"Unknown",			L"Unknown",				L"Unknown",				L"O(b)",				true,	L""),
	SORTING_ALGORITHM(inPlaceRadixLSDSortBase16,				L"In-Place Radix Sort (LSD Base 16)",					L"Unknown",			L"Unknown",				L"Unknown",				L"O(b)",				true,	L""),
	SORTING_ALGORITHM(pigeonholesort,							L"Pigeon Hole Sort",									L"O(N + n)",		L"O(N + n)",			L"O(N + n)",			L"O(N + n)",			true,	L""),
	SORTING_ALGORITHM(quicksort,								L"Quick Sort (Median of Three, Dual Pointer)",			L"O(n log n)",		L"O(n log n)",			L"O(n\u00B2)",			L"O(log n)",			false,	L""),
	SORTING_ALGORITHM(std::stable_sort,							L"std::stable_sort",									L"O(n log n)",		L"O(n log n)",			L"O(n log\u00B2 n)",	L"O(n)",				true,	L""),
	SORTING_ALGORITHM(std::sort,								L"std::sort",											L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(log n)",			false,	L""),
	SORTING_ALGORITHM(wikisort_nobuffer,						L"Wiki Sort (Buffer Size: 0)",							L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				true,	L"Block sort implementation (stable in-place merge sort"),
	SORTING_ALGORITHM(wikisort_fixedbuffer,						L"Wiki Sort (Buffer Size: 512)",						L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(wikisort_sqrtbuffer,						L"Wiki Sort (Buffer Size: sqrt(N / 2))",				L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(sqrt(n))",			true,	L""),
	SORTING_ALGORITHM(wikisort_fullbuffer,						L"Wiki Sort (Buffer Size: (N / 2))",					L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
	SORTING_ALGORITHM(grailsort_nobuffer,						L"Grail Sort (Buffer Size: 0)",							L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(grailsort_fixedbuffer,					L"Grail Sort (Buffer Size: 512)",						L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				true,	L""),
	SORTING_ALGORITHM(grailsort_fullbuffer,						L"Grail Sort (Buffer Size: N)",							L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
	SORTING_ALGORITHM(vergesort,								L"Verge Sort",											L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				false,	L""),	
	SORTING_ALGORITHM(gravitysort,								L"Gravity Sort (Bead Sort)",							L"O(S)",			L"O(S)",				L"O(S)",				L"O(n\u00B2)",			false,	L""),
	SORTING_ALGORITHM(skasort,									L"Ska Sort",											L"O(b * n)",		L"O(b * n)",			L"O(n log n)",			L"O(b)",				false,	L"Ska sort is an optimized and generalized american flag sort."),
	SORTING_ALGORITHM(vergeskasort,								L"Ska Verge Sort",										L"O(n)",			L"Unknown",				L"Unknown",				L"Unknown",				false,	L""),
	SORTING_ALGORITHM(americanflagsort,							L"American Flag Sort (Serial In-place MSD Radix Sort)",	L"O(b * n)",		L"O(b * n)",			L"O(n log n)",			L"O(b)",				false,	L"Serial In-place MSD Radix sort with 256 buckets"),
	SORTING_ALGORITHM(poplarsort,								L"Poplar Heap Sort",									L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(1)",				false,	L""),
	SORTING_ALGORITHM(dropmergesort,							L"Drop Merge Sort",										L"O(n)",			L"O(n + k log k)",		L"O(n + k log k)",		L"O(k)",				false,	L""),
	SORTING_ALGORITHM(patiencesort, 							L"Patience Sort",										L"Unknown",			L"Unknown",				L"Unknown",				L"Unknown",				false,	L""),

	SORTING_ALGORITHM(sqrtsort, 								L"Sqrtsort",											L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(sqrt(n))",			true,	L"Similar to Grail sort, but allocates sqrt(n) size buffer on the heap"),
	SORTING_ALGORITHM(neatsort, 								L"NeatSort",											L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				false,	L"Adaptive mergesort that finds natural non-decreasing runs and merges them together."),
	SORTING_ALGORITHM(melsort, 									L"Melsort",												L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				false,	L"Builds a set of encroaching lists, then merges them until there is only one sorted list left"),
	SORTING_ALGORITHM(quickmergesort, 							L"Quick Merge Sort",									L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(log n)",			false,	L""),
//	SORTING_ALGORITHM(merge_insertion_sort, 					L"Merge Insertion Sort",								L"Unknown",			L"Unknown",				L"Unknown",				L"Unknown",				false,	L""),

	//	SORTING_ALGORITHM(AVLTreeSort,								L"AVL Tree Sort",										L"O(n log n)",		L"O(n log n)",			L"O(n log n)",			L"O(n)",				false,	L""),
#if BOOST_INSTALLED	
	SORTING_ALGORITHM(boost::sort::block_indirect_sort, 		L"boost::block_indirect_sort (parallel)",				L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(block_size * T)",	false,	L""),
	SORTING_ALGORITHM(boost::sort::parallel_stable_sort, 		L"boost::parallel_stable_sort (parallel)",				L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),//O(n / 2) space
	SORTING_ALGORITHM(boost::sort::sample_sort, 				L"boost::sample_sort (parallel)",						L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),
	SORTING_ALGORITHM(integer_sort, 							L"boost::spread_sort",									L"O(n)",			L"Unknown",				L"O(k)",				L"Unknown",				false,	L""),
	SORTING_ALGORITHM(boost::sort::flat_stable_sort, 			L"boost::flat_stable_sort",								L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"Unknown",				true,	L""),
	SORTING_ALGORITHM(boost::sort::indirect_flat_stable_sort, 	L"boost::indirect_flat_stable_sort",					L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"Unknown",				true,	L""),
	SORTING_ALGORITHM(boost::sort::spinsort, 					L"boost::spinsort",										L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),//O(n / 2) space
	SORTING_ALGORITHM(boost::sort::indirect_spinsort, 			L"boost::indirect_spinsort",							L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(n)",				true,	L""),//O(n / 2) space
	SORTING_ALGORITHM(boost::sort::pdqsort_branchless, 			L"boost::pdqsort_branchless",							L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(log n)",			false,	L""),
	SORTING_ALGORITHM(boost::sort::pdqsort,						L"boost::pdqsort",										L"O(n)",			L"O(n log n)",			L"O(n log n)",			L"O(log n)",			false,	L""),
#endif
	//	SORTING_ALGORITHM(regionssort,					L"Regions Sort",									L"Unknown",			L"Unknown",				L"Unknown",				L"Unknown",		false,	L""),
//big crash (BIG ALLOCATE)	SORTING_ALGORITHM(countingsort,					L"Counting Sort",			L"Unknown",			L"Unknown",				L"Unknown",				L"Unknown",		false,	L""),
	//	SORTING_ALGORITHM(rotater, L"", L"", L"", L"", false, L"")
};

double runSortN(const SortingAlgorithm& sort_alg, const std::vector<unsigned>& list, unsigned test_runs) {
	Benchmark timer;
	
	//SortingAlgorithm alg((VSortAlgPtr)merge_insertion_sort<vit, vit, vcmp>, L"Merge insertion sort");
	//Visualizer v(1,2,3,4,VisualizationType::BarGraph,ColorMode::RainbowHSV,InitTypeValue::Regular,InitTypeOrder::Shuffled,DrawOption::Accesses,)


	double total_time = 0.0;

	int runs = 1;
	for (; runs <= test_runs; runs++) {
		std::vector<unsigned> l(list);

		timer.start();
		sort_alg.vsort(l.begin(), l.end(), vcmp());
		timer.stop();

		double runtime = timer.getElapsedMS();

		total_time += runtime;
	}

	return total_time / (double)runs;
}

void BenchmarkOrdering(unsigned n, InitTypeValue values, int test_runs, const std::vector<SortingAlgorithm>& test_sorts) {
	std::wofstream outfile;
	outfile.open(L"ordering (" + (std::to_wstring(n)) + L" elements).xls");

	//=B2=MIN($B2:$AD2)

	//write all the sort names
	outfile << '\t';
	for (auto & sort : test_sorts) {
		outfile << sort.name << '\t';
	}
	outfile << '\n';

	for (auto a : InitTypeOrderNames) {
		outfile << a.second << '\t';

		InitTypeOrder order = a.first;

		std::vector<unsigned> list;
		Initializer::Init(&list, n, values, order);

		std::wcout << a.second << '\n';

		for (auto & sort : test_sorts) {
			std::wcout << '\t' << sort.name << ' ';

			double runtime = runSortN(sort, list, test_runs);

			Visualizer::SortRunData srd(sort.name, n, runtime, 0, 0, 0, 0, InitTypeValueNames[values], a.second);
			Visualizer::SortTable.AddRow(srd);
			Visualizer::SortTable.SortByColumn(Visualizer::SortRunData::ColumnId::runtime, true);

			std::wcout << runtime << '\n';

			outfile << runtime << '\t';
		}

		outfile << '\n';
	}

	outfile.close();
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static bool IsCursorInClient = false;
	static bool IsDragging = false;
	static int DragCount = 0;
	static POINT ClickDownPoint = {-1, -1};
	static POINT ClickUpPoint = {-1, -1};
	static RECT SelectionRect;

	switch (message) {
		case WM_INITDIALOG: {
			g_hwnd = hwnd;
		
			RECT rcclient;
			GetClientRect(hwnd, &rcclient);			
			
			int def_opts = DrawOption::Comparisons | DrawOption::Assignments;

#ifdef BENCHMARK
			InitTypeValue values = Regular;
			InitTypeOrder order = Shuffled;
			Table results;
			int max_power = 20;//20;
			int test_runs = 5;
			double max_time = 100.0;
			Benchmark timer;

			std::vector<unsigned> sizes;
			for (int i = 1; i <= max_power; i++)
				sizes.push_back((unsigned)pow(2, i));

			for (unsigned i = 0; i < sizes.size(); ++i) {
				unsigned n = sizes[i];
				std::vector<unsigned> list;
				Initializer::Init(&list, n, values, order);

				printf("size: %d %d of %d                                                                       \n", n, i + 1, sizes.size());

				int sai = 0;
				for (auto & sort : SortingAlgorithms) {
					if (sort.second.desc == L"skip")
						continue;

					double total_time = 0.0;

				//	dummy->SetSize(n);
				//	dummy->SetSort(a.second);

					int runs = 1;
					for (; runs <= test_runs; runs++) {
						std::vector<unsigned> l(list);
						
						printf("%ls(%d of %d) run %d of %d elapsed: %f                                               \r", sort.first.c_str(), sai, SortingAlgorithms.size(), runs, test_runs, total_time);
						timer.start();
						sort.second.vsort(l.begin(), l.end(), vcmp());
						timer.stop();						

						double runtime = timer.getElapsedMS();

						if (runtime * 2.0 > max_time)
							sort.second.desc = L"skip";

						total_time += runtime;
						if (total_time > max_time)
							break;
					}
					
					Row row;
					row.data = {sort.second.name, n, timer.getElapsedMS(),
						0, 0, 0, 0,
						InitTypeValueNames[values], InitTypeOrderNames[order]};
					results.AddRow(row);

					sai++;
				}				
			}
			/*dummy->SortTableBy(ColumnId::runtime, true);
			dummy->SortTableBy(ColumnId::num_elements, false);
			dummy->PrintTableRows();*/

			auto rows = results.GetRows();
			std::map<std::wstring, std::map<unsigned, double>> sorts;
			for (int i = 0; i < rows.size(); i++) {
				auto sort_name = GET_COLUMN_DATA(ColumnId::name, rows[i].data);
				auto num_elements = GET_COLUMN_DATA(ColumnId::num_elements, rows[i].data);
				auto runtime = GET_COLUMN_DATA(ColumnId::runtime, rows[i].data);
				sorts[Visualizer::GetSortNameKey(sort_name)][num_elements] = runtime;
			}

			std::wofstream outfile;
			outfile.open("benchmark.txt");
			if (outfile.is_open()) {
			//write headers
				outfile << L"Number Of Elements" << '\t';
				for (auto & a : SortingAlgorithms)
					outfile << a.second.name << '\t';
				outfile << '\n';

				//write data
				for (int i = 0; i < sizes.size(); i++) {
					outfile << sizes[i] << '\t';
					for (auto & a : SortingAlgorithms) {
						auto pos = sorts[a.first].find(sizes[i]);
						if (pos != sorts[a.first].end()) {
							outfile << (*pos).second;
						}
						outfile << '\t';
					}
					outfile << '\n';
				}				
			}
			outfile.close();

#endif

		/*	for (auto & alg : SortingAlgorithms) {
				Visualizer* visualizer = new Visualizer(hwnd, g_hInst, (HMENU)g_vis_id_counter++,
					500,
					BarGraph,
					RainbowHSV,
					Regular,
					Shuffled,
					def_opts,
					SortingAlgorithms[alg.first]);
				Visualizer_HWNDS.push_back(visualizer->GetHWND());
			}*/

			CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TABLEVIEW), GetParent(hwnd), (DLGPROC)Visualizer::SortRunData::Proc, (LPARAM)&Visualizer::SortTable);


		//	CREATE_VISUALIZER(1000, BarGraph, Gradient, Random, Shuffled, def_opts, sqrtsort);
		//	CREATE_VISUALIZER(1000, BarGraph, Gradient, Random, Shuffled, def_opts, neatsort);
		//	CREATE_VISUALIZER(1000, BarGraph, Gradient, Random, Shuffled, def_opts, melsort);
		//	CREATE_VISUALIZER(1000, BarGraph, Gradient, Random, Shuffled, def_opts, quickmergesort);
		//	CREATE_VISUALIZER(1000, BarGraph, Gradient, Random, Shuffled, def_opts, merge_insertion_sort);

			//CREATE_VISUALIZER(1000, BarGraph, HeatMap, Regular, Shuffled, def_opts, quickmergesort);
			CREATE_VISUALIZER(100, ScatterPlot, Exploding, Regular, Shuffled, def_opts, insertionsort);
			

	//		CREATE_VISUALIZER(100, DisparityDots, Rainbow, FewUnique, def_opts, radixsort_LSD_2);
		//	CREATE_VISUALIZER(10, DisparityDots, Rainbow, Random, def_opts, radixsort_LSD_2);

		//	CREATE_VISUALIZER(10, DisparityDots, Gradient, Random, def_opts, quicksort);
		//	CREATE_VISUALIZER(100, DisparityDots, Gradient, Random, def_opts, insertionsort);
			//CREATE_VISUALIZER(1000, DisparityDots, Rainbow, Regular, Random, def_opts, pancakesort);
		//	CREATE_VISUALIZER(13, ColorWheel, RainbowHSV, Regular, Random, def_opts, quicksort);
		//	CREATE_VISUALIZER(100, ColorWheel, RainbowHSV, Regular, Random, def_opts, quicksort);
		//	CREATE_VISUALIZER(1000, ColorWheel, RainbowHSV, Regular, Random, def_opts, quicksort);
		//	CREATE_VISUALIZER(10000, ColorWheel, RainbowHSV, Regular, Random, def_opts, quicksort);


		//	CREATE_VISUALIZER(1000, ColorWheel, RainbowHSV, Regular, Sorted, def_opts, rotater);


		//	CREATE_VISUALIZER(5, BarGraph, Gradient,  Regular, Random, def_opts, bozosort);
		//	CREATE_VISUALIZER(10, BarGraph, Gradient,  Regular, Random, def_opts, lexipermsort);
		//	CREATE_VISUALIZER(6, BarGraph, Gradient,  Regular, Random, def_opts, bogosort);
		//	CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, FewUnique, def_opts, slowsort);
		//	CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, FewUnique, def_opts, stoogesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, pancakesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, selectionsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient, Regular, Random, def_opts, doubleselectionsort);
			//CREATE_VISUALIZER(10000, BarGraph, Gradient,  Regular, Random, def_opts, bubblesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, cocktailshakersort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, evenoddsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, cyclesort);
			//CREATE_VISUALIZER(100, BarGraph, RedWhiteBlue,  Regular, Random, Comparisons | PreSwaps | PostSwaps | Assignments, combsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, gnomesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, optimizedgnomesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, insertionsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, binaryinsertionsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, stdinsertionsort);
			//CREATE_VISUALIZER(100, BarGraph, Rainbow,  Regular, Random, def_opts, shellsort);
			//CREATE_VISUALIZER(10, BarGraph, Gradient,  Regular, Random, def_opts, weavemergesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, mergesort);
			//CREATE_VISUALIZER(10000, BarGraph, Gradient,  Regular, Random, def_opts, heapsort);
			//CREATE_VISUALIZER(100000, BarGraph, Gradient,  Regular, Random, def_opts, minheapsort);
			//CREATE_VISUALIZER(1000000, BarGraph, Gradient,  Regular, Random, def_opts, stdheapsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, binarytreesort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, smoothsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, weakheapsort);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, radixsort_LSD_32);
			//CREATE_VISUALIZER(100, BarGraph, Gradient,  Regular, Random, def_opts, typedpigeonholesort);
			//CREATE_VISUALIZER(100, ScatterPlot, RainbowHSV,  Regular, Random, def_opts, quicksort);
//bad		//CREATE_VISUALIZER(1000, BarGraph, Gradient, Regular, Random, def_opts, gravitysort);

		//	CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts | Accesses, regionssort);

			//CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, vergeskasort);
			/*CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Shuffled, 0, vergeskasort);
			CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Shuffled, 0, vergeskasort);
			CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Shuffled, 0, vergeskasort);
			CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Shuffled, 0, vergeskasort);
			CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Shuffled, 0, vergeskasort);*/


			/*CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts | Accesses, skasort);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts | Accesses, vergeskasort);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, std::sort);		
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, vergesort);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, pdqsort);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, wikisort_fixedbuffer);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, grailsort_fixedbuffer);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, std::stable_sort);			
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, timsort);
			CREATE_VISUALIZER(1000000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, dropmergesort);*/

			//CREATE_VISUALIZER(1000, BarGraph, Gradient, Regular, Random, def_opts, poplarsort);
			//CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Random, def_opts, inPlaceRadixLSDSortBase10);

		//	CREATE_VISUALIZER(100000, BarGraph, RainbowHSV, Regular, Shuffled, def_opts, skasort);
		//	CREATE_VISUALIZER(1000, Squares, RainbowHSV, FewUnique, PipeOrgan, def_opts, timsort);
			//CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, FrayedStart, def_opts, timsort);
			//CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, FrayedEnd, def_opts, timsort);
			//CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Random, def_opts, timsort);

			//NOT WORKING
			//			CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Random, def_opts, countingsort);
			//				CREATE_VISUALIZER(4, BarGraph, Gradient, Random, def_opts, bogobogosort);
				
				//CREATE_VISUALIZER(1000, BarGraph, RainbowHSV, Regular, Random, def_opts, softheapsort);

			break;
		}
		case WM_PAINT: {
			RECT rcclient;
			GetClientRect(hwnd, &rcclient);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			HBITMAP hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);

			/////////////////////////////////////////////////////////////

			HBRUSH backbrush = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDCmem, &rcclient, backbrush);
			DeleteObject(backbrush);

			if (IsDragging) {
				FrameRect(hDCmem, &SelectionRect, GetStockBrush(WHITE_BRUSH));
			}

			/////////////////////////////////////////////////////////////

			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			//cleanup
			SelectObject(hDCmem, hbmOldBitmap);
			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_ERASEBKGND:{
			return TRUE;
		}
		case WM_COMMAND: {
			HWND ihwnd = (HWND)lParam;
			UINT iid = LOWORD(wParam);
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (iid) {
						case BTN_ADD: {
							Visualizer* activeVis = Visualizer::getVisualizer(Visualizer::Options::GetHWND());
							if (activeVis) {
								Visualizer* visualizer = new Visualizer(hwnd, g_hInst, (HMENU)g_vis_id_counter++, 
																		activeVis->GetSize(),
																		activeVis->GetVisualizationType(), activeVis->GetColorMode(),
																		activeVis->GetInitTypeValue(), activeVis->GetInitTypeOrder(),
																		activeVis->GetDrawOptions(),
																		SortingAlgorithms[activeVis->GetSortNameKey()]);
								visualizer->SetDrawSpeed(activeVis->GetDrawSpeed());
								Visualizer_HWNDS.push_back(visualizer->GetHWND());
							}
							else {
								CREATE_VISUALIZER(100, BarGraph, Gradient, Regular, Shuffled, DrawOption(Comparisons | Assignments), selectionsort);
							}

							Resize();
//							Redraw();

							break;
						}
						case BTN_REMOVE: {
							if (Visualizer_HWNDS.size() > 1) {
								HWND removeHWND = NULL;

								Visualizer* activeVis = Visualizer::getVisualizer(Visualizer::Options::GetHWND());
								if (activeVis)
									removeHWND = activeVis->GetHWND();
								else
									removeHWND = Visualizer_HWNDS[Visualizer_HWNDS.size() - 1];

								auto pos = find(Visualizer_HWNDS.begin(), Visualizer_HWNDS.end(), removeHWND);
								DestroyWindow(*pos);
								Visualizer_HWNDS.erase(pos);

								if (activeVis) {
									Visualizer::Options::ClearHWND();
								}

								Resize();
//								Redraw();
							}

							break;
						}
						case BTN_RUN_ALL: {
							for (unsigned i = 0; i < Visualizer_HWNDS.size(); i++) {
								Visualizer* vis = Visualizer::getVisualizer(Visualizer_HWNDS[i]);
								vis->RunSort();
							}

							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CHAR: {
			switch (wParam) {
				case VK_RETURN: {
					for (unsigned i = 0; i < Visualizer_HWNDS.size(); i++) {
						Visualizer* vis = Visualizer::getVisualizer(Visualizer_HWNDS[i]);
						vis->RunSort();
					}
					break;
				}
				case 'A': {
					if (GetAsyncKeyState(VK_CONTROL) < 0) {
						
					}

					break;
				}
			}
			break;
		}
		case WM_SIZE: {
			Resize();
//			Redraw();

			break;
		}
		case WM_LBUTTONDOWN: {
			IsDragging = true;
			SetCapture(hwnd);
			DragCount = 0;
			ClickDownPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			
			break;
		}
		case WM_LBUTTONUP: {
			if (IsDragging) {
				ReleaseCapture();
				IsDragging = false;
				ClickUpPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				SelectionRect = {0, 0, 0, 0};
				
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_RBUTTONUP: {

			std::vector<SortingAlgorithm> test_sorts = {
				//				TEST_SORT(pancakesort,								L"Pancake Sort"),
				//				TEST_SORT(selectionsort,							L"Selection Sort"),
				//				TEST_SORT(doubleselectionsort,						L"Double Selection Sort"),
				//				TEST_SORT(bubblesort,								L"Bubble Sort"),
				//				TEST_SORT(cocktailshakersort,						L"Cocktail Shaker Sort"),
				//				TEST_SORT(evenoddsort,								L"Even Odd Sort"),
				//				TEST_SORT(combsort,									L"Comb Sort"),
				//				TEST_SORT(cyclesort,								L"Cycle Sort"),
				//				TEST_SORT(gnomesort,								L"Gnome Sort"),
				//				TEST_SORT(optimizedgnomesort,						L"Gnome Sort (Optimized)"),
				//				TEST_SORT(insertionsort,							L"Insertion Sort"),
				//				TEST_SORT(binaryinsertionsort,						L"Binary Insertion Sort"),
				//				TEST_SORT(stdinsertionsort,							L"std::insertion_sort"),
				//			TEST_SORT(shellsort,								L"Shell Sort"),
				//				TEST_SORT(weavemergesort,							L"Weave Merge Sort"),
				//			TEST_SORT(mergesort,								L"Merge Sort"),
				TEST_SORT(timsort, L"Tim Sort"),
					TEST_SORT(tim::timsort, L"Tim Sort (Optimized)"),
					//			TEST_SORT(heapsort,									L"Heap Sort (Max)"),
					//			TEST_SORT(minheapsort,								L"Heap Sort (Min)"),
					//			TEST_SORT(stdheapsort,								L"std::heap_sort"),
					//			TEST_SORT(smoothsort,								L"Smooth Sort"),
					//			TEST_SORT(weakheapsort,								L"Weak Heap Sort"),
					//			TEST_SORT(binarytreesort,							L"Binary Tree Sort"),										
					//				TEST_SORT(radixsort_LSD_2,							L"Radix Sort (LSD Base 2)"),
					//				TEST_SORT(radixsort_LSD_4,							L"Radix Sort (LSD Base 4)"),
					//				TEST_SORT(radixsort_LSD_10,							L"Radix Sort (LSD Base 10)"),
					//				TEST_SORT(radixsort_LSD_16,							L"Radix Sort (LSD Base 16)"),
					//				TEST_SORT(radixsort_LSD_32,							L"Radix Sort (LSD Base 32)"),
					//				TEST_SORT(inPlaceRadixLSDSortBase2,					L"In-Place Radix Sort (LSD Base 2)"),
					//				TEST_SORT(inPlaceRadixLSDSortBase4,					L"In-Place Radix Sort (LSD Base 4)"),
					//				TEST_SORT(inPlaceRadixLSDSortBase10,				L"In-Place Radix Sort (LSD Base 10)"),
					//				TEST_SORT(inPlaceRadixLSDSortBase16,				L"In-Place Radix Sort (LSD Base 16)"),
					//				TEST_SORT(pigeonholesort,							L"Pigeon Hole Sort"),
					//			TEST_SORT(quicksort,								L"Quick Sort (Median of Three, Dual Pointer)"),
					//			TEST_SORT(std::stable_sort,							L"std::stable_sort"),
					TEST_SORT(std::sort, L"std::sort"),
					//			TEST_SORT(wikisort_nobuffer,						L"Wiki Sort (Buffer Size: 0)"),
					TEST_SORT(wikisort_fixedbuffer, L"Wiki Sort (Buffer Size: 512)"),
					//			TEST_SORT(wikisort_sqrtbuffer,						L"Wiki Sort (Buffer Size: sqrt(N / 2))"),
					//			TEST_SORT(wikisort_fullbuffer,						L"Wiki Sort (Buffer Size: (N / 2))"),
					//			TEST_SORT(grailsort_nobuffer,						L"Grail Sort (Buffer Size: 0)"),
					TEST_SORT(grailsort_fixedbuffer, L"Grail Sort (Buffer Size: 512)"),
					//			TEST_SORT(grailsort_fullbuffer,						L"Grail Sort (Buffer Size: N)"),
					TEST_SORT(vergesort, L"Verge Sort"),
					//				TEST_SORT(gravitysort,								L"Gravity Sort (Bead Sort)"),
					TEST_SORT(skasort, L"Ska Sort"),
					TEST_SORT(vergeskasort, L"Ska Verge Sort"),
					//			TEST_SORT(americanflagsort,							L"American Flag Sort (Serial In-place MSD Radix Sort)"),
					//			TEST_SORT(poplarsort,								L"Poplar Heap Sort"),
					TEST_SORT(dropmergesort, L"Drop Merge Sort"),
					//			TEST_SORT(patiencesort, 							L"Patience Sort"),
					//			TEST_SORT(boost::sort::block_indirect_sort, 		L"boost::block_indirect_sort (parallel)"),
					//			TEST_SORT(boost::sort::parallel_stable_sort, 		L"boost::parallel_stable_sort (parallel)"),
					//			TEST_SORT(boost::sort::sample_sort, 				L"boost::sample_sort (parallel)"),
					//			TEST_SORT(integer_sort, 							L"boost::spread_sort"),
					//			TEST_SORT(boost::sort::flat_stable_sort, 			L"boost::flat_stable_sort"),
					//			TEST_SORT(boost::sort::indirect_flat_stable_sort, 	L"boost::indirect_flat_stable_sort"),
					//			TEST_SORT(boost::sort::spinsort, 					L"boost::spinsort"),
					//			TEST_SORT(boost::sort::indirect_spinsort, 			L"boost::indirect_spinsort"),
					//			TEST_SORT(boost::sort::pdqsort_branchless, 			L"boost::pdqsort_branchless"),
					//			TEST_SORT(boost::sort::pdqsort,						L"boost::pdqsort"),						
			};

			std::thread a(BenchmarkOrdering, 1000000, InitTypeValue::Normal, 10, test_sorts);
			a.detach();

			for (unsigned i = 0; i < Visualizer_HWNDS.size(); i++) {
				Visualizer::AddSelectionHWND(Visualizer_HWNDS[i]);
			}
			break;
		}
		case WM_MOUSEMOVE: {
			if (!IsCursorInClient) {
				IsCursorInClient = true;
				TRACKMOUSEEVENT tme = {0};
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				TrackMouseEvent(&tme);
			}

			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if (IsDragging) {
				DragCount++;

				RECT rcclient;
				GetClientRect(hwnd, &rcclient);

				SelectionRect.left = std::min(ClickDownPoint.x, pt.x); SelectionRect.left = rcclient.left > SelectionRect.left ? rcclient.left : SelectionRect.left;
				SelectionRect.right = std::max(ClickDownPoint.x, pt.x); SelectionRect.right = rcclient.right < SelectionRect.right ? rcclient.right : SelectionRect.right;
				SelectionRect.top = std::min(ClickDownPoint.y, pt.y); SelectionRect.top = rcclient.top > SelectionRect.top ? rcclient.top : SelectionRect.top;
				SelectionRect.bottom = std::max(ClickDownPoint.y, pt.y); SelectionRect.bottom = rcclient.bottom < SelectionRect.bottom ? rcclient.bottom : SelectionRect.bottom;
				
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}


