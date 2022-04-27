#pragma once

#define NOMINMAX

#include <Windows.h>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "initializer.h"
#include "chart.h"
#include "table.h"
#include "sorting.h"
#include "trace.h"
#include "benchmark.h"
#pragma comment(lib, "winmm.lib")
#include "sounds.h"
#include "customtrackbar.h"

#define VISUALIZERCLASSNAME L"VisualizerControlClassName"

ATOM register_visualizer_control();

typedef TraceVector<TraceInt>::iterator tvit;
typedef std::less<TraceInt>/*TraceLess<TraceInt>*/ tvcmp;

typedef std::vector<unsigned>::iterator vit;
typedef std::less<unsigned> vcmp;

template<typename T, typename C>
struct SortAlg { typedef std::function<void(T, T, C)> Alg; };

typedef void(*TVSortAlgPtr)(tvit, tvit, tvcmp);
typedef void(*VSortAlgPtr)(vit, vit, vcmp);

class SortingAlgorithm {
public:
	SortAlg<tvit, tvcmp>::Alg sort;
	SortAlg<vit, vcmp>::Alg vsort;
	std::wstring name;
	std::wstring best_case, average_case, worst_case, space;	
	bool is_stable;
	std::wstring desc;
	void operator()(tvit _First, tvit _Last, tvcmp compare) {
		sort(_First, _Last, compare);
	}
	void operator()(vit _First, vit _Last, vcmp compare) {
		vsort(_First, _Last, compare);
	}

	SortingAlgorithm() {}

	SortingAlgorithm(SortAlg<tvit, tvcmp>::Alg sort, SortAlg<vit, vcmp>::Alg vsort, std::wstring name,
					 std::wstring best_case, std::wstring average_case, std::wstring worst_case, std::wstring space, bool is_stable,
					 std::wstring desc)
		: sort(sort), vsort(vsort), name(name), 
		best_case(best_case), average_case(average_case), worst_case(worst_case), space(space), is_stable(is_stable),
		desc(desc) { }

	SortingAlgorithm(SortAlg<vit, vcmp>::Alg vsort, std::wstring name) {
		this->vsort = vsort;
		this->name = name;
	}
};

extern std::map<std::wstring, SortingAlgorithm> SortingAlgorithms;

enum class SortState {
	Unknown, Unsorted, Sorted, Inorder, Sorting
};

enum VisualizationType {
	BarGraph,
	FullBarGraph,
	ScatterPlot,
	HorizontalPyramid,
	ColorWheel,
	DisparityDots,
	Squares,
};
STRINGIFY_ENUM_DEC(VisualizationType)

enum ColorMode {
	Rainbow,
	RainbowHSV,
	BlackAndWhite,
	RedWhiteBlue, 
	Gradient,
	HeatMap,
	Exploding,
	Heap
};
STRINGIFY_ENUM_DEC(ColorMode)

enum DrawOption {
	None = 0, 
	Accesses = 1 << 0,
	Comparisons = 1 << 1,
	Assignments = 1 << 2,
	PreSwaps = 1 << 3,
	PostSwaps = 1 << 4
}; 
STRINGIFY_ENUM_DEC(DrawOption)

STRINGIFY_ENUM_DEC(InitTypeValue)
STRINGIFY_ENUM_DEC(InitTypeOrder)

template<class T>
struct Range;

class Visualizer {
public:
	enum class DrawEventType {
		NoDraw, Redraw, DrawSkipped, Access, Comparison, Assignment, Swap
	};

	enum class ScreenMode {
		SortVisualizer, Chart
	};
	
	bool isDrawEventOn(DrawEventType det) {
		switch (det) {
			case DrawEventType::Assignment: return options.draw_options & DrawOption::Assignments;
			case DrawEventType::Access: return options.draw_options & DrawOption::Accesses;
			case DrawEventType::Comparison: return options.draw_options & DrawOption::Comparisons;
			case DrawEventType::Swap: return options.draw_options & DrawOption::PostSwaps || options.draw_options & DrawOption::PreSwaps;
			default: return false;
		}
	}
	static bool isDrawEventSingle(DrawEventType det) { return (det == DrawEventType::Assignment || det == DrawEventType::Access || det == DrawEventType::Swap || det == DrawEventType::Comparison); }
	static bool isDrawEventChange(DrawEventType det) { return (det == DrawEventType::Assignment || det == DrawEventType::Swap); }
	static bool isDrawEventDouble(DrawEventType det) { return (det == DrawEventType::Swap || det == DrawEventType::Comparison); }

	struct DrawEventInfo {
		unsigned ptr1, ptr2;
		DrawEventType draw_event_type;

		/*bool operator==(const DrawEventInfo& rhs) {
			if (rhs.draw_event_type == DrawEventType::Redraw || draw_event_type == DrawEventType::Redraw) return true;
			return ptr1 == rhs.ptr1 && ptr2 == rhs.ptr2;
		}*/
	};

	static MIDISoundPlayer midi_sound_player;

	Visualizer(HWND parent, HINSTANCE hinst, HMENU id, unsigned size, VisualizationType visualization_type, ColorMode color_mode, InitTypeValue init_type_value, InitTypeOrder init_type_order, DrawOption draw_options, const SortingAlgorithm& sort);

	HWND GetHWND() { return hwnd; }

	static Visualizer* getVisualizer(HWND hwnd);

	void SetSort(const SortingAlgorithm& sort);

	std::wstring GetSortName() { return sort.name; }

	static std::wstring GetSortNameKey(std::wstring sort_name) { for (auto a : SortingAlgorithms) if (a.second.name == sort_name) return a.first; return L"NOT FOUND"; }

	std::wstring GetSortNameKey() { return GetSortNameKey(sort.name); }

	unsigned GetSize() { return options.size; }

	void SetSize(unsigned size) { options.size = size; Resize(); }

	VisualizationType GetVisualizationType() { return options.visualization_type; }

	ColorMode GetColorMode() { return options.color_mode; }

	InitTypeValue GetInitTypeValue() { return options.init_type_value; }

	InitTypeOrder GetInitTypeOrder() { return options.init_type_order; }

	DrawOption GetDrawOptions() { return options.draw_options; }

	double GetRuntime() { return timer.getElapsedMS(); }

	void RunSort();

	void DrawScreen();

	double GetDrawSpeed();

	void SetDrawSpeed(double new_speed);

	std::vector<unsigned>* GetInitializedList();

	static void AddSelectionHWND(HWND hwnd) { Options::hwnds_MultiSelected.insert(hwnd); getVisualizer(hwnd)->DrawScreen(); }

	class Options {
	public:
		Options();

		std::wstring GetInitTypeValueName();
		std::wstring GetInitTypeOrderName();
		void SetInitType(InitTypeValue init_type_value, InitTypeOrder init_type_order);
		void SetInitType(InitTypeValue init_type_value) { SetInitType(init_type_value, (InitTypeOrder)this->init_type_order); }
		void SetInitType(InitTypeOrder init_type_order) { SetInitType((InitTypeValue)this->init_type_value, init_type_order); }

		void SetDrawOptions(DrawOption draw_option);

		double GetDefaultDrawSpeed() { return default_draw_speed; }
		double GetClickedDrawSpeed() { return clicked_draw_speed; }

		void SetSleepTime(int time) { sleep_time = time; }

		static void InitOptions(Visualizer* vis);

		static HWND GetHWND() { return hwnd; }
		static void ClearHWND() { DestroyWindow(hwnd); hwnd = NULL; }

	private:
		static bool is_initializing;
		static HWND hwnd;
		static std::set<HWND> hwnds_MultiSelected;
		static HWND hwnd_SortingAlgorithm;
		static HWND hwnd_VisualizationType;
		static HWND hwnd_ColorMode;
		static HWND hwnd_Instrument;
		static std::map<UINT, HWND> hwnd_DrawOptions;//map control id to control hwnd
		static std::map<UINT, DrawOption> id_DrawOptions;//map control id to drawoption enum val
		static HWND hwnd_InitTypeValues;//map control id to control hwnd
		static std::map<UINT, InitTypeValue>id_InitTypeValue;//map control id to InitTypeValue enum val
		static HWND hwnd_InitTypeOrdering;//map control id to control hwnd
		static std::map<UINT, InitTypeOrder>id_InitTypeOrder;//map control id to InitTypeOrder enum val
		static HWND hwnd_Size;
		static HWND hwnd_FastDraw;
		static HWND hwnd_DrawSpeed;
		static HWND hwnd_SleepTime;
		static HWND hwnd_Complexity;
		static HWND hwnd_Counts;
		static HWND hwnd_Runtime;
		static HWND hwnd_Sound;
		static HWND hwnd_NoteMinFrequency;
		static HWND hwnd_NoteMaxFrequency;
		static HWND hwnd_NoteOnVelocity;
		static HWND hwnd_NoteOffVelocity;
		static HWND hwnd_NoteDuration;
		static CustomTrackbar ctb_Volume;

		static BOOL CALLBACK Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		unsigned size;
		VisualizationType visualization_type;
		ColorMode color_mode;
		InitTypeValue init_type_value;
		InitTypeOrder init_type_order;
		DrawOption draw_options;
		int sleep_time = 0;
		BYTE show_complexity = BST_INDETERMINATE;
		BYTE show_counts = BST_INDETERMINATE;
		BYTE show_runtime = BST_INDETERMINATE;
		bool bBenchmark = true;
		bool bdraw_initialization = true;

		std::vector<size_t> access_ages;

		bool bfast_draw = true;
		double draw_speed_factor = 10.0;
		double default_draw_speed = draw_speed_factor;
		double clicked_draw_speed = 100.0;

		static bool sound_on;
		static unsigned char note_min_frequency;
		static unsigned char note_max_frequency;
		static unsigned char note_on_velocity;
		static unsigned char note_off_velocity;
		static int note_duration;

		friend class Visualizer;
		friend class TraceInt;
		template <class T, class A = std::allocator<T>> friend class TraceVector;
		friend void swap(TraceInt& lhs, TraceInt& rhs);
		friend void iter_swap(const TraceVector<TraceInt>::iterator& _Left, const TraceVector<TraceInt>::iterator& _Right);
	};

	Options options;

	class SortRunData {
	public:
		enum class ColumnId {
			name, num_elements, runtime, compares, assigns, swaps, accesses, values, ordering
		};

		SortRunData() {}

		SortRunData(std::wstring name, unsigned num_elements, double runtime, 
					unsigned compares, unsigned assigns, unsigned swaps, unsigned accesses, 
					std::wstring init_values, std::wstring init_ordering) :
			name(name), num_elements(num_elements), runtime(runtime),
			compares(compares), assigns(assigns), swaps(swaps), accesses(accesses),
			init_values(init_values), init_ordering(init_ordering) {}

		SortRunData(std::wstring name, unsigned num_elements, double runtime) :
			name(name), num_elements(num_elements), runtime(runtime),
			compares(0), assigns(0), swaps(0), accesses(0),
			init_values(L"NULL"), init_ordering(L"NULL") {}

		void SetData(Visualizer* vis) {
			SetData(vis->GetSortName(), vis->options.size, vis->GetRuntime(),
				vis->tv.compare_count, vis->tv.assignment_count, vis->tv.swap_count, vis->tv.access_count,
				vis->options.GetInitTypeValueName(), vis->options.GetInitTypeOrderName());
		}

		void SetData(std::wstring name, unsigned num_elements, double runtime,
					 unsigned compares, unsigned assigns, unsigned swaps, unsigned accesses,
					 std::wstring init_values, std::wstring init_ordering) {
			this->name = name;	this->num_elements = num_elements; this->runtime = runtime;
			this->compares = compares; this->assigns = assigns;	this->swaps = swaps; this->accesses = accesses;
			this->init_values = init_values; this->init_ordering = init_ordering;
		}

		static std::wstring GetColumnName(ColumnId col_id) {
			return ColumnNames[col_id];
		}

		static int GetColumnCount() { return ColumnNames.size(); }

		static bool CompareByColumn(const SortRunData& a, const SortRunData& b, ColumnId col_id, bool is_ascending) {
			switch (col_id) {
				case ColumnId::name: { return is_ascending ? a.name < b.name : b.name < a.name; }
				case ColumnId::num_elements: { return is_ascending ? a.num_elements < b.num_elements : b.num_elements < a.num_elements; }
				case ColumnId::runtime: { return is_ascending ? a.runtime < b.runtime : b.runtime < a.runtime; }
				case ColumnId::compares: { return is_ascending ? a.compares < b.compares : b.compares < a.compares; }
				case ColumnId::assigns: { return is_ascending ? a.assigns < b.assigns : b.assigns < a.assigns; }
				case ColumnId::swaps: { return is_ascending ? a.swaps < b.swaps : b.swaps < a.swaps; }
				case ColumnId::accesses: { return is_ascending ? a.accesses < b.accesses : b.accesses < a.accesses; }
				case ColumnId::values: { return is_ascending ? a.init_values < b.init_values : b.init_values < a.init_values; }
				case ColumnId::ordering: { return is_ascending ? a.init_ordering < b.init_ordering : b.init_ordering < a.init_ordering; }
				default: throw(std::exception("Invalid column id"));
			}
		}

		static void PrintColumnNames() {
			for (int i = 0; i < ColumnNames.size(); i++) {
				std::wcout << ColumnNames[ColumnId(i)] << '\t';
			}
		}

		static void Print(const SortRunData& row) {
			std::wcout << row.name << '\t' << row.num_elements << '\t' << row.runtime << '\t' 
				<< row.compares << '\t' << row.assigns << '\t' << row.swaps << '\t' << row.accesses << '\t' 
				<< row.init_values << '\t' << row.init_ordering << '\n';
		}

		static std::wstring GetColumnDataAsString(const SortRunData& row, ColumnId col_id) {
			switch (col_id) {
				case ColumnId::name: { return row.name; }
				case ColumnId::num_elements: { return std::to_wstring(row.num_elements); }
				case ColumnId::runtime: { return std::to_wstring(row.runtime); }
				case ColumnId::compares: { return std::to_wstring(row.compares); }
				case ColumnId::assigns: { return std::to_wstring(row.assigns); }
				case ColumnId::swaps: { return std::to_wstring(row.swaps); }
				case ColumnId::accesses: { return std::to_wstring(row.accesses); }
				case ColumnId::values: { return row.init_values; }
				case ColumnId::ordering: { return row.init_ordering; }
			}
			return L"";
		}

		std::wstring GetColumnDataAsString(ColumnId col_id) {
			return GetColumnDataAsString(*this, col_id);
		}

		void Print() {
			Print(*this);
		}

		static BOOL CALLBACK Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		std::wstring name;
		unsigned num_elements;
		double runtime;
		unsigned compares, assigns, swaps, accesses;
		std::wstring init_values, init_ordering;

		static std::map<ColumnId, std::wstring> ColumnNames;

		friend class Table<SortRunData>;

		static void RowAddedProc(const SortRunData& row);

		static void SortByColumnProc(ColumnId col_id, bool is_ascending = true);
	};
	static Table<SortRunData> SortTable;

private:	
	HWND hwnd;
	HMENU id;
	HINSTANCE hinst;
	RECT rcclient;

	static HWND hwnd_Table;
	static HWND hwnd_LVSortTable;

	bool is_initializing = false;
	Benchmark timer;

	SortState tv_state = SortState::Unknown;

	TraceVector<TraceInt> tv;
	std::vector<unsigned> v;

	SortingAlgorithm sort;

	void init();

	void test_sort();

	ScreenMode active_screen_mode;

	class Screen {
	public:
		template <class T>
		class ReadOnly {
			friend class Screen;
		private:
			T data;
			T operator=(const T& arg) { data = arg; return data; }
		public:
			operator const T&() const { return data; }
			/*operator T() const { return data; }
			operator T() { return data; }*/
		};

		ReadOnly<unsigned> w;
		ReadOnly<unsigned> h;
		ReadOnly<unsigned> stride;
		ReadOnly<COLORREF> back_color;
		ReadOnly<unsigned> bmp_size;

		~Screen() {
			FreeData();
		}

		void Resize(unsigned w, unsigned h) {
			ScopedVar<bool> scoped_is_resizing(is_resizing, true);
			FreeData();
			this->w = w, this->h = h;
			stride = w * 4;
			bmp_size = (h + 3) * (stride + 8);
			data = new BYTE[bmp_size];
		}

		void SetData(unsigned index, BYTE value) {
			if (is_resizing) return;
			data[index] = value;
		}

		BYTE* GetData() const { return data; }

		void FreeData() {
			if (data) {
				delete[] data;
				data = 0;
			}
		}

		bool IsResizing() { return is_resizing; }

	private:
		BYTE* data = NULL;
		bool is_resizing = false;
	};
	Screen screen;

	bool mouse_in_client = false;

	unsigned num_draw_skips;
	unsigned num_skipped = 0;
	std::vector<DrawEventInfo> skipped_draws;
	DrawEventInfo last_draw = {0, 0, DrawEventType::NoDraw};
	DrawEventInfo last_draw_value = {0, 0, DrawEventType::NoDraw};
	void reset_last_draw();

	static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Draw(const DrawEventInfo& draw_event_info);

	void Draw(unsigned ptr1, DrawEventType draw_event_type) { Draw({ ptr1, 0, draw_event_type }); }

	void Draw(unsigned ptr1, unsigned ptr2, DrawEventType draw_event_type) { Draw({ ptr1, ptr2, draw_event_type }); }

	COLORREF GetColor(const std::vector<TraceInt>* arr, unsigned i, const DrawEventInfo* draw_event_info, void* param = nullptr);

	void FillBackground();

	bool isCursorInClient();

	void Resize();

	static void Play(unsigned v, const Range<unsigned>& range);
	static void Play(std::vector<unsigned>& values, const Range<unsigned>& range);

	static void DrawScreenText(Visualizer* vis, HWND hwnd, HDC hDCmem);
	static void DrawScreen(Visualizer* vis, DrawEventInfo* draw_event_info);

	static void DrawBarGraph(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info);
	static void DrawScatterPlot(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info);
	static void DrawColorWheel(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info);
	static void DrawDisparityDots(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info);
	static void DrawSquares(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info);

	friend class TraceInt;
	template <class T, class A = std::allocator<T>> friend class TraceVector;
	friend void swap(TraceInt& lhs, TraceInt& rhs);
	friend void iter_swap(const TraceVector<TraceInt>::iterator& _Left, const TraceVector<TraceInt>::iterator& _Right);
	friend ATOM register_visualizer_control();
};

struct SortRunLVCompareProcInfo {
	Visualizer::SortRunData::ColumnId col_id;
	bool is_ascending;
};

int CALLBACK SortRunLVCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

//template<typename T>
//class ConsoleVisualizer {
//	public:
//
//	static int sleep_time;
//	static short line_pos;
//
//	static int Write(std::wstring screen) {
//		DWORD dwBytesWritten = 0;
//		WriteConsoleOutputCharacter(ConH, screen.c_str(), screen.length(), {0, line_pos}, &dwBytesWritten);
//		if (sleep_time)
//			this_thread::sleep_for(chrono::milliseconds(sleep_time));
//		return dwBytesWritten;
//	}
//
//	static int WriteVec(const std::vector<T>& arr, int ptr = -1) {
//		std::vector<std::wstring> screenbuffer(25, std::wstring(120, ' '));
//		for (int i = 0; i < arr.size(); i++) {
//			for (int j = 0; j < arr[i]; j++) {
//				screenbuffer[j][i] = i == ptr ? '@' : '#';
//			}
//		}
//
//		std::wstring screen = L"";
//		for (int i = 0; i < screenbuffer.size(); i++) {
//			screen += screenbuffer[i];
//		}
//
//		return Write(screen);
//	}
//	private:
//	static HANDLE ConH;
//};
//
//template<typename T> HANDLE ConsoleVisualizer<T>::ConH = GetStdHandle(STD_OUTPUT_HANDLE);
//template<typename T> int ConsoleVisualizer<T>::sleep_time = 0;
//template<typename T> short ConsoleVisualizer<T>::line_pos = 0;
//
//template <typename T>
//std::wostream& operator<<(std::wostream& os, const std::vector<T>& ti) {
//	static std::vector<std::wstring> screen(25, std::wstring(120, ' '));
//
//	for (int i = 0; i < ti.size(); i++) {
//		for (int j = 0; j < ti[i].value; j++) {
//			screen[j][i] = '#';
//		}
//	}
//	for (int i = 0; i < screen.size(); i++) {
//		os << screen[i] << '\n';
//	}
//
//	return os;
//}


























