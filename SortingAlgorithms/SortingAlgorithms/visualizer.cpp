#include "Visualizer.h"

//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windowsx.h>
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <map>
#include <algorithm>
#include <thread>
#include <mutex> 
#include <assert.h>
#include <fstream>
#include <ctime>
#include <iomanip>

#include "resource.h"
#include "cyclesort.h"

#define PI      3.14159265358979323846

#define CTB_VOLUME 1000

////////////////////////////////////////////////////////////////////////////////////////////////////////
//if you add an enum value you need to add it to its correct STRINGIFY_ENUM macro to add it to the gui//
////////////////////////////////////////////////////////////////////////////////////////////////////////

STRINGIFY_ENUM(DrawOption,None,Accesses,Comparisons,Assignments,PreSwaps,PostSwaps)
STRINGIFY_ENUM(InitTypeValue,Regular,FewUnique,VeryFewUnique,NoUnique,Random,Normal)
STRINGIFY_ENUM(InitTypeOrder,Shuffled,FrayedAscending,FrayedDescending,SawtoothAscending,SawtoothDescending,VeryNearSorted,Reversed,Sorted,PipeOrgan,FrayedFront,FrayedMiddle,FrayedEnd,RandomFront,RandomMiddle,RandomEnd,Alternating,WorstCaseQuickSortMedianOf3)

STRINGIFY_ENUM(VisualizationType,BarGraph,FullBarGraph,ScatterPlot,ColorWheel,HorizontalPyramid,DisparityDots,Squares)
STRINGIFY_ENUM(ColorMode,Rainbow,RainbowHSV,BlackAndWhite,RedWhiteBlue,Gradient,HeatMap,Exploding,Heap)

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


MIDISoundPlayer Visualizer::midi_sound_player(Instrument::Voice_Oohs);

ATOM register_visualizer_control() {
	WNDCLASS wnd = { 0 };
	wnd.lpszClassName = VISUALIZERCLASSNAME;
	wnd.style = CS_GLOBALCLASS;
	wnd.lpfnWndProc = (WNDPROC)Visualizer::Proc;
	return RegisterClass(&wnd);
}

COLORREF getRainbowColor(unsigned i, unsigned steps, double center = 128, double width = 127, double phase = 220/*120*/) {
	if (phase == NULL) phase = 0;
	static double pi = 3.14159265358979323846;
	double frequency = 2 * pi / (double)steps;

	double red = sin(frequency * i + (2 * pi / 3) + phase) * width + center;
	double green = sin(frequency * i + 0 + phase) * width + center;
	double blue = sin(frequency * i + (4 * pi / 3) + phase) * width + center;

	/*red += 55;f
	green -= 55;
	blue -= 55;*/

	red = red > 255 ? 255 : red < 0 ? 0 : red;
	green = green > 255 ? 255 : green < 0 ? 0 : green;
	blue = blue > 255 ? 255 : blue < 0 ? 0 : blue;

	return RGB((BYTE)red, (BYTE)green, (BYTE)blue);
}

void HBITMAPBlitToHdc(HDC hdcDst, HBITMAP hbmSrc, int x, int y, int w, int h) {
	HDC hdcScreen = GetDC(NULL);
	HDC hdcSrc = CreateCompatibleDC(hdcScreen);

	HBITMAP hbmOld = static_cast<HBITMAP>(SelectObject(hdcSrc, hbmSrc));
	BitBlt(hdcDst, x, y, w, h, hdcSrc, 0, 0, SRCCOPY);

	if (hbmOld) SelectObject(hdcSrc, hbmOld);
	DeleteDC(hdcSrc);
	ReleaseDC(NULL, hdcScreen);
}

void DrawBorderedText(HDC hdc, const RECT& rc, const std::wstring& wstr, COLORREF bordercol, COLORREF textcol) {
	SetTextColor(hdc, bordercol);
	SetBkMode(hdc, TRANSPARENT);

	int height = rc.bottom - rc.top;

	const std::vector<std::pair<int, int>> shifts = { {-1, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, 0}, {1, 0}, {0, 1}, {0, -1},
													  {-2, -2}, {2, 2}, {-2, 2}, {2, -2}, {-2, 0}, {2, 0}, {0, 2}, {0, -2} };
	for (int i = 0; i < (height > 26 ? 16 : 8); i++) {
		RECT trc = { rc.left + shifts[i].first, rc.top + shifts[i].second, rc.right + shifts[i].first, rc.bottom + shifts[i].second };
		ExtTextOut(hdc, rc.left + shifts[i].first, rc.top + shifts[i].second, ETO_CLIPPED, &trc, wstr.c_str(), wstr.size(), NULL);
	}

	SetTextColor(hdc, textcol);

	ExtTextOut(hdc, rc.left, rc.top, ETO_CLIPPED, &rc, wstr.c_str(), wstr.size(), NULL);
}

//justify left: -1, center: 0, right: 1
void DrawBorderedTextOnNextLine(HDC hdc, const std::wstring& wtext, RECT* rc_last_line, COLORREF bordercol, COLORREF textcol, int justify = -1) {
	SIZE sz;
	GetTextExtentPoint32(hdc, wtext.c_str(), wtext.size(), &sz);
	if (justify < 0) *rc_last_line = { rc_last_line->left, rc_last_line->bottom, rc_last_line->left + sz.cx, rc_last_line->bottom + sz.cy };
	else if (justify > 0) *rc_last_line = { rc_last_line->right - sz.cx, rc_last_line->bottom, rc_last_line->right, rc_last_line->bottom + sz.cy };
	else *rc_last_line = { rc_last_line->left, rc_last_line->bottom, rc_last_line->left + sz.cx, rc_last_line->bottom + sz.cy };
	DrawBorderedText(hdc, *rc_last_line, wtext, bordercol, textcol);
}

void get_centered_text_rect(HDC hdc, const std::wstring& wstr, RECT& rc, RECT* outrect) {
	SIZE sz;
	GetTextExtentPoint32(hdc, wstr.c_str(), wstr.size(), &sz);
	outrect->left = rc.left + (((rc.right - rc.left) - sz.cx) / 2);
	outrect->right = outrect->left + sz.cx;
	outrect->top = rc.top + (((rc.bottom - rc.top) - sz.cy) / 2);
	outrect->bottom = outrect->top + sz.cy;
}

Visualizer::Visualizer(HWND parent, HINSTANCE hinst, HMENU id, 
					   unsigned size, VisualizationType visualization_type, ColorMode color_mode, 
					   InitTypeValue init_type_value, InitTypeOrder init_type_order, DrawOption draw_options, 
					   const SortingAlgorithm& sort) {
	this->hinst = hinst;
	this->id = id;
	this->options.size = size;

	options.visualization_type = visualization_type;
	options.color_mode = color_mode;
	options.init_type_value = init_type_value;	
	options.init_type_order = init_type_order;
	options.draw_options = draw_options;

	this->sort = sort;

	active_screen_mode = ScreenMode::SortVisualizer;

	tv.setVisualizer(this);

	hwnd = CreateWindow(VISUALIZERCLASSNAME, L"", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, parent, (HMENU)id, hinst, this);

	ScopedVar<InitTypeOrder> scoped_init_type(options.init_type_order, InitTypeOrder::Sorted);
	init();

	DrawScreen();
}

void Visualizer::SetSort(const SortingAlgorithm& sort) {
	if (!sort.sort) throw std::exception("Invalid sort");
	if (is_initializing || this->tv_state == SortState::Sorting) return;
	this->sort = sort;
}

void Visualizer::RunSort() {
	if (is_initializing || this->tv_state == SortState::Sorting) return;
	std::thread a{ [](Visualizer* vis) {
		vis->init();
		vis->test_sort();

		while (vis->tv_state != SortState::Sorted) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		SortRunData run_data(vis->GetSortName(), vis->options.size, vis->GetRuntime(), 
						vis->tv.compare_count, vis->tv.assignment_count, vis->tv.swap_count, vis->tv.access_count, 
						vis->options.GetInitTypeValueName(), vis->options.GetInitTypeOrderName());
		
		static std::mutex SortMutex;
		std::lock_guard<std::mutex> lock(SortMutex);

		Visualizer::SortTable.AddRow(run_data);		
		
		std::wofstream file;
		file.open("sorts.xls", std::ios::app);

		file << vis->GetSortName() << '\t';

		file << L"best:" << vis->sort.best_case << ',';
		file << L"avg:" << vis->sort.average_case << ',';
		file << L"worst:" << vis->sort.worst_case << ',';
		file << L"space:" << vis->sort.space << ',';
		file << L"stable:" << (vis->sort.is_stable ? L"Yes" : L"No") << '\t';

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		file << std::put_time(&tm, L"%d-%m-%Y %H:%M:%S") << '\t';

		file << vis->GetSize() << '\t';
		file << (std::to_wstring(vis->timer.getElapsedMS()) + L"ms") << '\t';

		file << vis->tv.compare_count << '\t';
		file << vis->tv.assignment_count << '\t';
		file << vis->tv.swap_count << '\t';
		file << vis->tv.access_count << '\n';

		file.close();

		vis->DrawScreen();
	}, this };
	a.detach();
}

void Visualizer::DrawScreen() {
	DrawScreen(this, 0);
}

std::vector<unsigned>* Visualizer::GetInitializedList()  {
	init();
	return &v;
}

void Visualizer::init() {
	if (is_initializing || tv_state == SortState::Sorting) return;
	ScopedVar<bool> scoped_is_initializing(is_initializing, true);

	srand(clock());

	ScopedVar<DrawOption> scoped_draw_options(options.draw_options, (DrawOption)(options.bdraw_initialization ? (options.draw_options == DrawOption::None ? DrawOption::None : DrawOption::Assignments | DrawOption::PostSwaps) : DrawOption::None));

	v.clear();
	tv.clear();

	v.reserve((size_t)options.size);
	tv.reserve((size_t)options.size);

	timer.reset();
	tv.reset_counts();

	tv_state = (options.init_type_order & InitTypeOrder::Sorted) ? SortState::Inorder : SortState::Unsorted;

	//intialize elements (this stage doesn't get drawn (push_back not access/swap))
	Initializer::InitValues(&tv, options.size, options.init_type_value);
	DrawScreen();	

	Initializer::InitOrder(&tv, options.size, options.init_type_order);

	//sort sorted_pointers for elements with same value (stable sort test / tv has elements with equal values) 
	if (options.init_type_value != InitTypeValue::Regular) {
		TraceVector<TraceInt> temp(tv.size());
		{
			ScopedVar<DrawOption> scoped_draw_options(this->options.draw_options, DrawOption::None);
			std::map<unsigned, std::vector<TraceInt>> values;
			for (unsigned i = 0; i < tv.size(); i++)
				values[tv[i].value].push_back(tv[i]);
			
			for (auto &val : values) {
				std::sort(val.second.begin(), val.second.end(), [](const TraceInt& a, const TraceInt& b) { return a.sorted_index_ < b.sorted_index_; });
				for (unsigned i = 0; i < val.second.size(); i++) {
					temp[val.second[i].index_] = val.second[i];
				}
			}
		}
		tv = temp;
	}

	//copy into vector<int>
	for (unsigned i = 0; i < tv.size(); i++)
		v.push_back(tv[i]);

	tv.reset_counts();

	//draw any pending skipped draws
	if (options.bfast_draw) {
		if (num_skipped < num_draw_skips) {
			ScopedVar<unsigned> scoped_num_draw_skips(num_draw_skips, num_skipped);
			Draw(0, 0, DrawEventType::DrawSkipped);
		}
	}
}

void Visualizer::test_sort() {
	if (tv_state == SortState::Sorting || is_initializing) return;
	tv_state = SortState::Sorting;
	
	DrawScreen();

	std::this_thread::sleep_for(std::chrono::milliseconds(700));

	tv.reset_counts();

	if (options.bBenchmark) {
		std::thread a([&]() {
			timer.reset();
			timer.start();
			sort(v.begin(), v.end(), vcmp());
			timer.stop();
			if (!sorted(v.begin(), v.end(), vcmp())) printf("SORT FAILED!!!!!\n");
			//	MessageBox(NULL, L"Sort Failed", L"Failure", MB_OK);
		});
		a.detach();
	}

	std::thread b([&]() {
		sort(tv.begin(), tv.end(), tvcmp());
		tv_state = SortState::Sorted;
		midi_sound_player.StopAllNotes();
		DrawScreen();
	});
	b.detach();

	/*ScopedVar<unsigned long long> scoped_access_count(this->tv.access_count, this->tv.access_count);
	ScopedVar<unsigned long long> scoped_assignment_count(this->tv.assignment_count, this->tv.assignment_count);
	ScopedVar<unsigned long long> scoped_compare_count(this->tv.compare_count, this->tv.compare_count);
	ScopedVar<unsigned long long> scoped_swap_count(this->tv.swap_count, this->tv.swap_count);
	if (!sorted(tv.begin(), tv.end(), tvcmp())) printf("SORT FAILED!!!!!\n");*/	
}

void Visualizer::Draw(const DrawEventInfo& draw_event_info) {
	while (screen.IsResizing()) 
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	if (options.draw_options && screen.w > 0 && screen.h > 0) {
		if (options.bfast_draw) {
			if (num_skipped < num_draw_skips) {
				//keep track of the changes to draw them later				
				if (isDrawEventOn(draw_event_info.draw_event_type) && isDrawEventSingle(draw_event_info.draw_event_type) && isDrawEventChange(draw_event_info.draw_event_type))
					skipped_draws[num_skipped] = draw_event_info;

				++num_skipped;
				return;//must return to skip DrawScreen call
			}
		}

		//play sound
		if (options.sound_on) {
			Range<unsigned> nums(0, (unsigned)tv.vec_.size() - 1);

			/*if (options.bfast_draw) {
				std::vector<unsigned> skipped_values;
				for (auto i = 0; i < skipped_draws.size() && i < 10; i++) {
					if (isDrawEventSingle(skipped_draws[i].draw_event_type)) {
						skipped_values.push_back(tv.vec_[skipped_draws[i].ptr1]);
						if (isDrawEventDouble(skipped_draws[i].draw_event_type)) {
							skipped_values.push_back(tv.vec_[skipped_draws[i].ptr2]);
						}
					}
				}
				Visualizer::Play(skipped_values, nums);
			}*/			

			if (isDrawEventSingle(draw_event_info.draw_event_type)) {
				Visualizer::Play(tv.vec_[draw_event_info.ptr1], nums);
				if (isDrawEventDouble(draw_event_info.draw_event_type)) {
					Visualizer::Play(tv.vec_[(size_t)draw_event_info.ptr2], nums);
				}
			}
		}

		DrawEventInfo cpy_draw_event_info = draw_event_info;
		DrawScreen(this, &cpy_draw_event_info);		

		//we drew everything, so reset num_skipped
		if (options.bfast_draw) {
			num_skipped = 0;
		}
	}
}

double Visualizer::GetDrawSpeed() { return options.draw_speed_factor; };

void Visualizer::SetDrawSpeed(double new_speed_factor) {
	options.draw_speed_factor = new_speed_factor;	
	Resize();
}

void Visualizer::Resize() {
	GetClientRect(hwnd, &rcclient);

	screen.Resize(rcclient.right, rcclient.bottom);
//	FillBackground();

	if (screen.w && screen.h) {
		if (options.bfast_draw) {
			int num_skips = (int)((double)tv.size() *  options.draw_speed_factor / (double)screen.w);
			skipped_draws.resize(num_skips);
			num_draw_skips = num_skips;
		}

		DrawScreen();
	}
	else {
		skipped_draws.clear();
		num_draw_skips = 0;
		num_skipped = 0;
	}
}

void Visualizer::Play(unsigned v, const Range<unsigned>& range) {
	static Range<unsigned> freqrange(Options::note_min_frequency, Options::note_max_frequency);
	if (v < range.min || v > range.max)
		return;
	int note = range.MapTo(v, freqrange);
	midi_sound_player.PlayNote(note, Options::note_on_velocity, Options::note_off_velocity, Options::note_duration);
}

void Visualizer::Play(std::vector<unsigned>& values, const Range<unsigned>& range) {
	static Range<unsigned> freqrange(Options::note_min_frequency, Options::note_max_frequency);
	std::vector<char> notes;
	for (auto & v : values) {
		if (v < range.min || v > range.max)
			continue;
		int note = range.MapTo(v, freqrange);
		notes.push_back(note);
	}
	midi_sound_player.PlayNotes(notes, Options::note_on_velocity, Options::note_off_velocity, Options::note_duration);
}

bool Visualizer::Options::is_initializing = false;
HWND Visualizer::Options::hwnd = NULL;
std::set<HWND> Visualizer::Options::hwnds_MultiSelected;
HWND Visualizer::Options::hwnd_SortingAlgorithm = NULL;
HWND Visualizer::Options::hwnd_VisualizationType = NULL;
HWND Visualizer::Options::hwnd_ColorMode = NULL;
HWND Visualizer::Options::hwnd_Instrument = NULL;
std::map<UINT, HWND> Visualizer::Options::hwnd_DrawOptions;//map control id to control hwnd
std::map<UINT, DrawOption> Visualizer::Options::id_DrawOptions;//map control id to drawoption enum val
HWND Visualizer::Options::hwnd_InitTypeValues;//map control id to control hwnd
std::map<UINT, InitTypeValue> Visualizer::Options::id_InitTypeValue;//map control id to InitTypeValue enum val
HWND Visualizer::Options::hwnd_InitTypeOrdering;//map control id to control hwnd
std::map<UINT, InitTypeOrder> Visualizer::Options::id_InitTypeOrder;//map control id to InitTypeOrder enum val
HWND Visualizer::Options::hwnd_Size = NULL;
HWND Visualizer::Options::hwnd_FastDraw = NULL;
HWND Visualizer::Options::hwnd_DrawSpeed = NULL;
HWND Visualizer::Options::hwnd_SleepTime = NULL;
HWND Visualizer::Options::hwnd_Complexity = NULL;
HWND Visualizer::Options::hwnd_Counts = NULL;
HWND Visualizer::Options::hwnd_Runtime = NULL;
HWND Visualizer::Options::hwnd_Sound = NULL;
HWND Visualizer::Options::hwnd_NoteMinFrequency = NULL;
HWND Visualizer::Options::hwnd_NoteMaxFrequency = NULL;
HWND Visualizer::Options::hwnd_NoteOnVelocity = NULL;
HWND Visualizer::Options::hwnd_NoteOffVelocity = NULL;
HWND Visualizer::Options::hwnd_NoteDuration = NULL;
CustomTrackbar Visualizer::Options::ctb_Volume("volume", WS_VISIBLE | WS_CHILD, 199, 280, 64, 20, CTB_VOLUME,
	0, 127, 0, 1, 3,
	0, 0, CTB_BORDER | CTB_SMOOTH | CTB_STAY, {
		RGB(10, 10, 10), RGB(50, 50, 50), RGB(0, 0, 0), RGB(20, 20, 20),								// background/border/window name idle/highlight
		RGB(0, 255, 0), RGB(240, 240, 240), RGB(0, 255, 0), RGB(240, 240, 240),							// left/right channel left/right highlight
		RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127), 									// thumb background: idle/hover/selected
		RGB(10, 10, 10), RGB(10, 10, 10), RGB(10, 10, 10),			 									// thumb border: idle/hover/selected
		RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127)},									// thumb text: idle/hover/selected							
		NULL, NULL);

bool Visualizer::Options::sound_on = false;
unsigned char Visualizer::Options::note_min_frequency = 50;
unsigned char Visualizer::Options::note_max_frequency = 120;
unsigned char Visualizer::Options::note_on_velocity = 70;
unsigned char Visualizer::Options::note_off_velocity = 0;
int Visualizer::Options::note_duration = 60;

HWND Visualizer::hwnd_Table;
HWND Visualizer::hwnd_LVSortTable;
Table<Visualizer::SortRunData> Visualizer::SortTable;
std::map<Visualizer::SortRunData::ColumnId, std::wstring> Visualizer::SortRunData::ColumnNames = {
	{ Visualizer::SortRunData::ColumnId::name, L"Sort" },
	{ Visualizer::SortRunData::ColumnId::num_elements, L"Elements" },
	{ Visualizer::SortRunData::ColumnId::runtime, L"Runtime" },
	{ Visualizer::SortRunData::ColumnId::compares, L"Comparisons" },
	{ Visualizer::SortRunData::ColumnId::assigns, L"Assignments" },
	{ Visualizer::SortRunData::ColumnId::swaps, L"Swaps" },
	{ Visualizer::SortRunData::ColumnId::accesses, L"Accesses" },
	{ Visualizer::SortRunData::ColumnId::values, L"Values" },
	{ Visualizer::SortRunData::ColumnId::ordering, L"Ordering" }
};

Visualizer::Options::Options() {}

void Visualizer::Options::SetDrawOptions(DrawOption draw_option) {
	draw_options = draw_option;
}

void  Visualizer::Options::SetInitType(InitTypeValue init_type_value, InitTypeOrder init_type_order) {
	this->init_type_value = init_type_value;
	this->init_type_order = init_type_order;
}

std::wstring Visualizer::Options::GetInitTypeValueName() {
	return InitTypeValueNames[init_type_value];
}

std::wstring Visualizer::Options::GetInitTypeOrderName() {
	return InitTypeOrderNames[init_type_order];
}

Visualizer* Visualizer::getVisualizer(HWND hwnd) {
	return (Visualizer*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

void Visualizer::Options::InitOptions(Visualizer* vis) {	
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)vis);

	SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	////////////////////INITIALIZE CONTROLS//////////////////////////////

	bool all_selected_are_same = true;

	//init sorting algorithm
	all_selected_are_same = true;
	for (auto & a : Visualizer::Options::hwnds_MultiSelected) 
		if (getVisualizer(a)->GetSortName() != vis->GetSortName()) 
			all_selected_are_same = false;
	ComboBox_SetCurSel(vis->options.hwnd_SortingAlgorithm, all_selected_are_same ? ComboBox_FindString(vis->options.hwnd_SortingAlgorithm, 0, vis->GetSortName().c_str()) : -1);

	//init visualization type
	all_selected_are_same = true;
	for (auto & a : Visualizer::Options::hwnds_MultiSelected)
		if (getVisualizer(a)->options.visualization_type != vis->options.visualization_type)
			all_selected_are_same = false;
	ComboBox_SetCurSel(vis->options.hwnd_VisualizationType, all_selected_are_same ? vis->options.visualization_type : -1);

	//init color mode
	all_selected_are_same = true;
	for (auto & a : Visualizer::Options::hwnds_MultiSelected)
		if (getVisualizer(a)->options.color_mode != vis->options.color_mode) 
			all_selected_are_same = false;
	ComboBox_SetCurSel(vis->options.hwnd_ColorMode, all_selected_are_same ? (int)vis->options.color_mode : -1);

	//init visualization options (values)
	all_selected_are_same = true;
	for (auto & a : Visualizer::Options::hwnds_MultiSelected)
		if (getVisualizer(a)->options.init_type_value != vis->options.init_type_value) 
			all_selected_are_same = false;
	ComboBox_SetCurSel(vis->options.hwnd_InitTypeValues, all_selected_are_same ? (int)vis->options.init_type_value : -1);
	
	//init visualization options (ordering)
	all_selected_are_same = true;
	for (auto & a : Visualizer::Options::hwnds_MultiSelected)
		if (getVisualizer(a)->options.init_type_order != vis->options.init_type_order) 
			all_selected_are_same = false;
	ComboBox_SetCurSel(vis->options.hwnd_InitTypeOrdering, all_selected_are_same ? (int)vis->options.init_type_order : -1);
	

	/////////////////init draw options////////////////////////////////
	auto init_draw_options = [&](UINT id, DrawOption opt) {
		vis->options.id_DrawOptions[id] = opt;
		vis->options.hwnd_DrawOptions[id] = GetDlgItem(hwnd, id);
		Button_SetCheck(vis->options.hwnd_DrawOptions[id], vis->options.draw_options & opt ? BST_CHECKED : BST_UNCHECKED);
	};
	init_draw_options(CBTN_ACCESSES, DrawOption::Accesses);
	init_draw_options(CBTN_COMPARISONS, DrawOption::Comparisons);
	init_draw_options(CBTN_ASSIGNMENTS, DrawOption::Assignments);
	init_draw_options(CBTN_PRE_SWAP, DrawOption::PreSwaps);
	init_draw_options(CBTN_POST_SWAP, DrawOption::PostSwaps);
	/////////////////end init draw options////////////////////////////////

	//init size edit
	//TODO handle selections with different elements count options (size)
	vis->options.hwnd_Size = GetDlgItem(hwnd, EDC_SIZE);
	all_selected_are_same = true;
	for (auto & a : Visualizer::Options::hwnds_MultiSelected)
		if (getVisualizer(a)->options.size != vis->options.size)
			all_selected_are_same = false;
	all_selected_are_same = true;
	Edit_SetText(vis->options.hwnd_Size, all_selected_are_same ? std::to_wstring(vis->options.size).c_str() : L"");

	//init fast draw checkbox
	vis->options.hwnd_FastDraw = GetDlgItem(hwnd, CBTN_FAST_DRAW);
	Button_SetCheck(vis->options.hwnd_FastDraw, vis->options.bfast_draw ? BST_CHECKED : BST_UNCHECKED);
	//init draw speed edit
	vis->options.hwnd_DrawSpeed = GetDlgItem(hwnd, EDC_DRAW_SPEED_FACTOR);
	Edit_SetText(vis->options.hwnd_DrawSpeed, std::to_wstring((int)vis->options.draw_speed_factor).c_str());
	Edit_Enable(vis->options.hwnd_DrawSpeed, vis->options.bfast_draw);
	//init sleep time edit
	vis->options.hwnd_SleepTime = GetDlgItem(hwnd, EDC_SLEEP_TIME);
	Edit_SetText(vis->options.hwnd_SleepTime, std::to_wstring(vis->options.sleep_time).c_str());

	//init complexity checkbox
	vis->options.hwnd_Complexity = GetDlgItem(hwnd, CBTN_COMPLEXITY);
	Button_SetCheck(vis->options.hwnd_Complexity, vis->options.show_complexity);

	//init counts checkbox
	vis->options.hwnd_Counts = GetDlgItem(hwnd, CBTN_COUNTS);
	Button_SetCheck(vis->options.hwnd_Counts, vis->options.show_counts);

	//init runtime checkbox
	vis->options.hwnd_Runtime = GetDlgItem(hwnd, CBTN_RUNTIME);
	Button_SetCheck(vis->options.hwnd_Runtime, vis->options.show_runtime);

	//init sound checkbox
	vis->options.hwnd_Sound = GetDlgItem(hwnd, CBTN_SOUND);
	Button_SetCheck(vis->options.hwnd_Sound, vis->options.sound_on);

	//init instrument name
	SendMessage(vis->options.hwnd_Instrument, CB_SETCURSEL, vis->midi_sound_player.GetInstrument() - 1, NULL);

	//init min frequency
	vis->options.hwnd_NoteMinFrequency = GetDlgItem(hwnd, EDC_MIN_FREQ);
	Edit_SetText(vis->options.hwnd_NoteMinFrequency, std::to_wstring(vis->options.note_min_frequency).c_str());

	//init max frequency
	vis->options.hwnd_NoteMaxFrequency = GetDlgItem(hwnd, EDC_MAX_FREQ);
	Edit_SetText(vis->options.hwnd_NoteMaxFrequency, std::to_wstring(vis->options.note_max_frequency).c_str());

	//init note on velocity
	vis->options.hwnd_NoteOnVelocity = GetDlgItem(hwnd, EDC_ON_VELOCITY);
	Edit_SetText(vis->options.hwnd_NoteOnVelocity, std::to_wstring(vis->options.note_on_velocity).c_str());

	//init note off velocity
	vis->options.hwnd_NoteOffVelocity = GetDlgItem(hwnd, EDC_OFF_VELOCITY);
	Edit_SetText(vis->options.hwnd_NoteOffVelocity, std::to_wstring(vis->options.note_off_velocity).c_str());

	//init note duration
	vis->options.hwnd_NoteDuration = GetDlgItem(hwnd, EDC_NOTE_DURATION);
	Edit_SetText(vis->options.hwnd_NoteDuration, std::to_wstring(vis->options.note_duration).c_str());
}

BOOL CALLBACK Visualizer::Options::Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Visualizer* vis = getVisualizer(hwnd);

	switch (message) {
		case WM_INITDIALOG: {
			Visualizer::Options::hwnd = hwnd;

			Visualizer* vis = (Visualizer*)lParam;

			//add sorting algorithms to combobox
			vis->options.hwnd_SortingAlgorithm = GetDlgItem(hwnd, CB_ALGORITHMS);
			for (auto alg : SortingAlgorithms)
				SendMessage(vis->options.hwnd_SortingAlgorithm, CB_ADDSTRING, NULL, (LPARAM)alg.second.name.c_str());

			//add InitTypeValues to combobox
			vis->options.hwnd_InitTypeValues = GetDlgItem(hwnd, CB_INIT_VALUES);
			for (auto name : InitTypeValueNames)
				SendMessage(vis->options.hwnd_InitTypeValues, CB_ADDSTRING, NULL, (LPARAM)name.second.c_str());

			//add InitTypeOrders to combobox
			vis->options.hwnd_InitTypeOrdering = GetDlgItem(hwnd, CB_INIT_ORDER);
			for (auto name : InitTypeOrderNames)
				SendMessage(vis->options.hwnd_InitTypeOrdering, CB_ADDSTRING, NULL, (LPARAM)name.second.c_str());

			//add visualization types to combobox
			vis->options.hwnd_VisualizationType = GetDlgItem(hwnd, CB_VISUALIZATION_TYPE);
			for (auto name : VisualizationTypeNames)
				SendMessage(vis->options.hwnd_VisualizationType, CB_ADDSTRING, NULL, (LPARAM)name.second.c_str());

			//add color modes to combobox
			vis->options.hwnd_ColorMode = GetDlgItem(hwnd, CB_COLOR_MODE);
			for (auto name : ColorModeNames)
				SendMessage(vis->options.hwnd_ColorMode, CB_ADDSTRING, NULL, (LPARAM)name.second.c_str());

			//add instruments to combobox
			vis->options.hwnd_Instrument = GetDlgItem(hwnd, CB_INSTRUMENTS);
			for (auto name : InstrumentNames)
				SendMessage(vis->options.hwnd_Instrument, CB_ADDSTRING, NULL, (LPARAM)name.second.c_str());

			//create volume trackbar
			ctb_Volume.Create(hwnd);
			Visualizer::midi_sound_player.SetVolume((char)ctb_Volume.current_val);

			ScopedVar<bool> scoped_is_initializing(is_initializing, true);
			InitOptions(vis);

			break;
		}
		case WM_COMMAND: {
			HWND ihwnd = (HWND)lParam;
			UINT iid = LOWORD(wParam);
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (iid) {
						case CBTN_ACCESSES:
						case CBTN_COMPARISONS:
						case CBTN_ASSIGNMENTS:
						case CBTN_PRE_SWAP: 
						case CBTN_POST_SWAP: {
							vis->options.SetDrawOptions((DrawOption)(vis->options.draw_options ^ vis->options.id_DrawOptions[iid]));
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.SetDrawOptions(vis->options.draw_options);
							break;
						}
						case CBTN_FAST_DRAW: {
							vis->options.bfast_draw = Button_GetCheck(vis->options.hwnd_FastDraw);
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.bfast_draw = vis->options.bfast_draw;
							vis->SetDrawSpeed(vis->GetDrawSpeed());
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->SetDrawSpeed(vis->GetDrawSpeed());
							
							Edit_Enable(vis->options.hwnd_DrawSpeed, vis->options.bfast_draw);
							break;
						}
						case CBTN_COMPLEXITY: {
							vis->options.show_complexity = Button_GetCheck(vis->options.hwnd_Complexity);
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.show_complexity = vis->options.show_complexity;
							break;
						}
						case CBTN_COUNTS: {
							vis->options.show_counts = Button_GetCheck(vis->options.hwnd_Counts);
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.show_counts = vis->options.show_counts;
							break;
						}
						case CBTN_RUNTIME: {
							vis->options.show_runtime = Button_GetCheck(vis->options.hwnd_Runtime);
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.show_runtime = vis->options.show_runtime;
							break;
						}
						case CBTN_SOUND: {
							vis->options.sound_on = Button_GetCheck(vis->options.hwnd_Sound);
							break;
						}
					}
					break;
				}
				case CBN_SELCHANGE: {
					int iindex = SendMessage(ihwnd, CB_GETCURSEL, NULL, NULL);
					std::wstring itext(255, L'\0');
					SendMessage(ihwnd, CB_GETLBTEXT, iindex, (LPARAM)&itext[0]);
					itext.erase(itext.find(L'\0'));
					switch (iid) {
						case CB_ALGORITHMS: {
							if (vis->is_initializing || vis->tv_state == SortState::Sorting) {
								int pos = ComboBox_FindString(ihwnd, 0, vis->GetSortName().c_str());
								if (pos == CB_ERR) throw(std::exception("Sort not found!"));
								ComboBox_SetCurSel(ihwnd, pos);
								MessageBeep(MB_OK);
							}
							else {
								vis->SetSort(SortingAlgorithms[GetSortNameKey(itext)]);
								for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->SetSort(SortingAlgorithms[GetSortNameKey(itext)]);
							}
							break;
						}
						case CB_INIT_VALUES: {
							if (vis->is_initializing || vis->tv_state == SortState::Sorting) {
								MessageBeep(MB_OK);
								ComboBox_SetCurSel(ihwnd, vis->options.init_type_value);
							}
							else {
								vis->options.SetInitType((InitTypeValue)iindex);
								for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.SetInitType(vis->GetInitTypeValue());
								
								ScopedVar<DrawOption> scoped_draw_options(vis->options.draw_options, DrawOption::None);
								vis->init();
								for (auto & a : Visualizer::Options::hwnds_MultiSelected) {//thread this???
									auto v = getVisualizer(a);
									ScopedVar<DrawOption> scoped(v->options.draw_options, DrawOption::None);
									v->init();
								}
							}
							break;
						}
						case CB_INIT_ORDER: {
							if (vis->is_initializing || vis->tv_state == SortState::Sorting) {
								MessageBeep(MB_OK);
								ComboBox_SetCurSel(ihwnd, vis->options.init_type_order);
							}
							else {
								vis->options.SetInitType((InitTypeOrder)iindex);
								for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.SetInitType(vis->GetInitTypeOrder());

								ScopedVar<DrawOption> scoped_draw_options(vis->options.draw_options, DrawOption::None);
								vis->init();
								for (auto & a : Visualizer::Options::hwnds_MultiSelected) {//thread this???
									auto v = getVisualizer(a);
									ScopedVar<DrawOption> scoped(v->options.draw_options, DrawOption::None);
									v->init();
								}
							}
							break;
						}
						case CB_VISUALIZATION_TYPE: {
							vis->options.visualization_type = (VisualizationType)iindex;
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.visualization_type = vis->options.visualization_type;
							break;
						}
						case CB_COLOR_MODE: {
							vis->options.color_mode = (ColorMode)iindex;
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.color_mode = vis->options.color_mode;
							break;
						}
						case CB_INSTRUMENTS: {
							vis->midi_sound_player.SetInstrument((unsigned char)iindex);
							break;
						}
					}
					break;
				}
				case EN_CHANGE: {
					static bool block_change = false;
					if (block_change) {
					//	block_change = false;
						break;
					}
					std::wstring text(12, L'\0');
					text.resize(Edit_GetText(ihwnd, &text[0], 12));
					switch (iid) {
						case EDC_SIZE: {					
							if (vis->is_initializing || vis->tv_state == SortState::Sorting) {
								ScopedVar<bool> scoped_block_change(block_change, true);
								int start, end;
								SendMessage(ihwnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);								
								SetWindowText(ihwnd, std::to_wstring(vis->options.size).c_str());
								SendMessage(ihwnd, EM_SETSEL, (WPARAM)start - 1, (LPARAM)end - 1);							
								MessageBeep(MB_OK);

								break;
							}

							static int min_val = 5;
							static int max_val = 10000000;
							int n = _wtoi(text.c_str());
							if (n < min_val) n = min_val;
							if (n > max_val) n = max_val;
							if (n != _wtoi(text.c_str())) {
								/*ScopedVar<bool> scoped_block_change(block_change, true);
								int start, end;
								SendMessage(ihwnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
								SetWindowText(ihwnd, std::to_wstring(n).c_str());
								SendMessage(ihwnd, EM_SETSEL, (WPARAM)start - 1, (LPARAM)end - 1);*/
								MessageBeep(MB_OK);
							}
							if (n != vis->options.size) {
								vis->options.size = n;
								for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.size = vis->options.size;
						
								ScopedVar<DrawOption> scoped_draw_options(vis->options.draw_options, DrawOption::None);
								vis->init();
								vis->Resize();

								for (auto & a : Visualizer::Options::hwnds_MultiSelected) { 
									auto v = getVisualizer(a);
									ScopedVar<DrawOption> scoped(v->options.draw_options, DrawOption::None);
									v->init();
									v->Resize();
								}

							}

							break;
						}
						case EDC_DRAW_SPEED_FACTOR: {
							vis->options.draw_speed_factor = vis->options.default_draw_speed = _wtoi(text.c_str());
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.draw_speed_factor = vis->options.draw_speed_factor;

							if (is_initializing) break;

							vis->Resize();
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->Resize();

							break;
						}
						case EDC_SLEEP_TIME: {
							vis->options.sleep_time = _wtoi(text.c_str());
							for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->options.sleep_time = vis->options.sleep_time;
							break;
						}
						case EDC_MIN_FREQ: { 
							vis->options.note_min_frequency = _wtoi(text.c_str());
							break; 
						}
						case EDC_MAX_FREQ: { 
							vis->options.note_max_frequency = _wtoi(text.c_str());
							break; 
						}
						case EDC_ON_VELOCITY: {
							vis->options.note_on_velocity = _wtoi(text.c_str());
							break;
						}
						case EDC_OFF_VELOCITY: {
							vis->options.note_off_velocity = _wtoi(text.c_str());
							break; 
						}
						case EDC_NOTE_DURATION: { 
							vis->options.note_duration = _wtoi(text.c_str());
							break; 
						}
					}
					break;
				}
				case CTB_CHANGE: {
					switch (iid) {
						case CTB_VOLUME: {
							Visualizer::midi_sound_player.SetVolume((char)ctb_Volume.current_val);

							vis->options.sound_on = (bool)ctb_Volume.current_val;

							if (ctb_Volume.current_val == 0) {								
								ctb_Volume.tcolor_scheme.border = RGB(255, 0, 0);
								ctb_Volume.tcolor_scheme.window_name_idle = RGB(255, 0, 0);
								ctb_Volume.tcolor_scheme.window_name_highlight = RGB(200, 0, 0);
							}
							else {								
								double min_deg = 0.0; //0 is red
								double max_deg = 120.0; //120 is green
								double value = (double)ctb_Volume.current_val / (double)ctb_Volume.max_val;
								double H = value * (max_deg - min_deg) + min_deg;

								COLORREF color = HSVtoRGB(H, 1.0, 1.0);

								ctb_Volume.tcolor_scheme.left_channel_idle = color;
								ctb_Volume.tcolor_scheme.left_channel_highlight = color;
								ctb_Volume.tcolor_scheme.border = RGB(0, 0, 0);
								ctb_Volume.tcolor_scheme.window_name_idle = RGB(0, 0, 0);
								ctb_Volume.tcolor_scheme.window_name_highlight = RGB(20, 20, 20);
							}
							break;
						}
					}
				}
			}
			if (is_initializing) break;

			vis->DrawScreen();
			for (auto & a : Visualizer::Options::hwnds_MultiSelected) getVisualizer(a)->DrawScreen();

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			//save handles of selected visualizer
			HWND oldvis = Visualizer::Options::hwnd;		

			//save handles of multi selected visualizers
			std::vector<HWND> multi_hwnds;
			multi_hwnds.reserve(Visualizer::Options::hwnds_MultiSelected.size());
			for (auto & a : Visualizer::Options::hwnds_MultiSelected)
				multi_hwnds.push_back(a);

			//empty out selections
			Visualizer::Options::hwnd = NULL;
			Visualizer::Options::hwnds_MultiSelected.clear();

			//redraw selected screens (now marked as unselected)
			getVisualizer(oldvis)->DrawScreen();
			for (auto & a : multi_hwnds)
				getVisualizer(a)->DrawScreen();

			EndDialog(hwnd, 0);
		}
		default: return FALSE;
	}
	return TRUE;
}

void Visualizer::SortRunData::SortByColumnProc(ColumnId col_id, bool is_ascending) {
	SortRunLVCompareProcInfo srlvcpi = {col_id, is_ascending};
	ListView_SortItems(hwnd_LVSortTable, SortRunLVCompareProc, &srlvcpi);
}

void Visualizer::SortRunData::RowAddedProc(const Visualizer::SortRunData& row) {
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.cchTextMax = 240;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.lParam = (LPARAM)&row;

	unsigned col_id = 0;

	std::wstring ws = GetColumnDataAsString(row, SortRunData::ColumnId(col_id));
	lvi.pszText = const_cast<LPWSTR>(&ws[0]);
	int pos = ListView_InsertItem(hwnd_LVSortTable, &lvi);

	for (col_id = 1; col_id < row.GetColumnCount(); ++col_id) {
		ws = GetColumnDataAsString(row, SortRunData::ColumnId(col_id));
		lvi.pszText = const_cast<LPWSTR>(&ws[0]);
		ListView_SetItemText(hwnd_LVSortTable, pos, col_id, lvi.pszText);
	}	

	//Print(row);
}

int CALLBACK SortRunLVCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	Visualizer::SortRunData* a = (Visualizer::SortRunData*)lParam1;
	Visualizer::SortRunData* b = (Visualizer::SortRunData*)lParam2;
	SortRunLVCompareProcInfo* info = (SortRunLVCompareProcInfo*)lParamSort;

	if (!a || !b || !info) return 0;

	bool less = Visualizer::SortRunData::CompareByColumn(*a, *b, info->col_id, info->is_ascending);

	return less ? -1 : 1;
}

BOOL CALLBACK Visualizer::SortRunData::Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			hwnd_Table = hwnd;

			hwnd_LVSortTable = GetDlgItem(hwnd, LV_SORT_TABLE);

			SendMessage(hwnd_LVSortTable, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

			LVCOLUMN col;
			col.mask = LVCF_TEXT | LVCF_WIDTH;

			auto insert_listview_column = [&col](HWND hwnd_ListView, ColumnId col_id, int width) {
				col.cx = width;

				std::wstring ws = SortRunData::GetColumnName(col_id);
				LPWSTR lpwstr = new wchar_t[ws.size() + 1];
				std::copy(ws.begin(), ws.end(), lpwstr);
				lpwstr[ws.size()] = '\0';
				col.pszText = lpwstr;

				ListView_InsertColumn(hwnd_ListView, (int)col_id, &col);
			};

			insert_listview_column(hwnd_LVSortTable, ColumnId::name, 220);
			insert_listview_column(hwnd_LVSortTable, ColumnId::num_elements, 70);
			insert_listview_column(hwnd_LVSortTable, ColumnId::runtime, 90);
			insert_listview_column(hwnd_LVSortTable, ColumnId::compares, 70);
			insert_listview_column(hwnd_LVSortTable, ColumnId::swaps, 70);
			insert_listview_column(hwnd_LVSortTable, ColumnId::assigns, 70);
			insert_listview_column(hwnd_LVSortTable, ColumnId::accesses, 70);
			insert_listview_column(hwnd_LVSortTable, ColumnId::values, 100);
			insert_listview_column(hwnd_LVSortTable, ColumnId::ordering, 100);

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR hdr = (LPNMHDR)lParam;
			switch (hdr->code) {
				case LVN_COLUMNCLICK: {
					NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;

					static int last_col = -1;
					static bool is_ascending = true;
					if (last_col != pListView->iSubItem) 
						is_ascending = true;
					else 
						is_ascending = !is_ascending;

					SortRunLVCompareProcInfo info;
					info.col_id = Visualizer::SortRunData::ColumnId(pListView->iSubItem);
					info.is_ascending = is_ascending;

					ListView_SortItems(hwnd_LVSortTable, SortRunLVCompareProc, &info);

					last_col = pListView->iSubItem;

					break;
				}
			}

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
		}
		default: return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK Visualizer::Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Visualizer* vis = Visualizer::getVisualizer(hwnd);
	
	switch (message) {
		case WM_NCCREATE: {
			vis = (Visualizer*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			vis->hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)vis);

			break;
		}
		case WM_PAINT: {
			DrawScreen(vis, (DrawEventInfo*)wParam);

			break;
		}
	/*	case WM_ERASEBKGND: {
			return TRUE;
		}*/
		case WM_SYSCOMMAND: {
			switch (wParam) {
				case SC_MAXIMIZE:
				case SC_MINIMIZE: {
					vis->Resize();
					vis->DrawScreen();
					break;
				}
			}

			break;
		}
		case WM_MOUSEMOVE: {
			if (!vis->mouse_in_client) {
				vis->mouse_in_client = true;
				TRACKMOUSEEVENT me;
				me.cbSize = sizeof(TRACKMOUSEEVENT);
				me.dwFlags = TME_LEAVE;
				me.hwndTrack = hwnd;
				me.dwHoverTime = 0;
				TrackMouseEvent(&me);

				vis->DrawScreen();
			}
			break;
		}
		case WM_MOUSELEAVE: 
		case WM_LBUTTONDOWN: 
		case WM_LBUTTONUP: {
			if (message == WM_MOUSELEAVE) {				
				vis->mouse_in_client = false;

				if (vis->options.draw_speed_factor != vis->options.GetDefaultDrawSpeed())
					vis->SetDrawSpeed(vis->options.GetDefaultDrawSpeed());

				vis->DrawScreen();				
			}
			else if (message == WM_LBUTTONDOWN) {
				if (vis->active_screen_mode == ScreenMode::SortVisualizer) {
					if (vis->is_initializing || vis->tv_state == SortState::Sorting) {
						vis->SetDrawSpeed(vis->options.GetClickedDrawSpeed());
					}
					else if (vis->options.draw_speed_factor != vis->options.GetDefaultDrawSpeed())
						vis->SetDrawSpeed(vis->options.GetDefaultDrawSpeed());
				}
			}
			else if (message == WM_LBUTTONUP) {
				if (vis->active_screen_mode == ScreenMode::SortVisualizer) {
					if (vis->is_initializing || vis->tv_state == SortState::Sorting) {
						vis->SetDrawSpeed(vis->options.GetDefaultDrawSpeed());
					}
					else {
						if (vis->options.draw_speed_factor != vis->options.GetDefaultDrawSpeed())
							vis->SetDrawSpeed(vis->options.GetDefaultDrawSpeed());
						else {
						//	vis->init();
							vis->RunSort();
						}
					}
				}
			}
			break;
		}
		case WM_RBUTTONUP: {
			if (vis->active_screen_mode == ScreenMode::SortVisualizer) {
				/*SetWindowLongPtr(vis->GetHWND(), GWLP_WNDPROC, (LONG)vis->chart.Proc);
				LPCREATESTRUCT lpcreate = new CREATESTRUCT();
				lpcreate->lpCreateParams = &vis->chart;
				SendMessage(hwnd, WM_NCCREATE, NULL, (LPARAM)&lpcreate);				
				delete lpcreate;*/
				
				if (Visualizer::Options::hwnd == NULL) {
					CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPTIONS), GetParent(hwnd), (DLGPROC)Options::Proc, (LPARAM)vis);
				}
				else {
					if (Visualizer::Options::hwnd != hwnd && GetAsyncKeyState(VK_CONTROL) < 0) {
						if (Visualizer::Options::hwnds_MultiSelected.find(hwnd) == Visualizer::Options::hwnds_MultiSelected.end())
							Visualizer::Options::hwnds_MultiSelected.insert(hwnd);
						else
							Visualizer::Options::hwnds_MultiSelected.erase(hwnd);

						Visualizer::Options::InitOptions(getVisualizer(Visualizer::Options::hwnd));

						vis->DrawScreen();
					}
					else {
						//save handles of multi selected visualizers (to redraw them later)
						std::vector<HWND> multi_hwnds;
						for (auto & a : Visualizer::Options::hwnds_MultiSelected)
							multi_hwnds.push_back(a);

						Visualizer::Options::hwnds_MultiSelected.clear();

						//redraw selected screens (now marked as unselected)
						for (auto & a : multi_hwnds)
							getVisualizer(a)->DrawScreen();

						Visualizer* oldvis = getVisualizer(Options::hwnd);
						Options::InitOptions(vis);
						if (oldvis) oldvis->DrawScreen();
						ShowWindow(oldvis->options.hwnd, SW_SHOWNORMAL);
					}
				}
			}
			
			break;
		}
		case WM_SIZE: {
			vis->Resize();

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {				
			if (vis) {
				delete vis;
				vis = 0;
			}
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Visualizer::reset_last_draw() {
	last_draw.ptr1 = 0;
	last_draw.ptr2 = 0;
	last_draw.draw_event_type = DrawEventType::NoDraw;
	last_draw_value.ptr1 = 0;
	last_draw_value.ptr2 = 0;
}

bool Visualizer::isCursorInClient() {
	RECT rcclient;
	GetClientRect(hwnd, &rcclient);
	return PtInRect(&rcclient, getclientcursorpos(hwnd));
}

void Visualizer::FillBackground() {
	COLORREF* p = (COLORREF*)screen.GetData(), i, size = screen.bmp_size / sizeof(*p);
	for (i = 0; i < size; ++i) p[i] = screen.back_color;
}

COLORREF Visualizer::GetColor(const std::vector<TraceInt>* arr, unsigned i, const DrawEventInfo* draw_event_info, void* param) {
	COLORREF col = RGB(0,0,0);

	bool is_leftptr = isDrawEventSingle(draw_event_info->draw_event_type) ? i == draw_event_info->ptr1 : false;
	bool is_rightptr = isDrawEventDouble(draw_event_info->draw_event_type) ? i == draw_event_info->ptr2 : false;

	if (draw_event_info->draw_event_type == DrawEventType::NoDraw) return screen.back_color;

	if (options.color_mode == ColorMode::Rainbow) {
		if (is_leftptr || is_rightptr)
			return RGB(255, 255, 255);//white
		else {
			return getRainbowColor((unsigned)arr->at(i).sorted_index_, (unsigned)arr->size());//rainbow
		}
	}
	if (options.color_mode == ColorMode::RainbowHSV) {
		if (is_leftptr || is_rightptr)
			return RGB(255, 255, 255);//white
		else {
			double min_deg = 0.0;
			double max_deg = 360.0;
			double value = (double)arr->at(i).sorted_index_ / (double)arr->size();
			double H = value * (max_deg - min_deg) + min_deg;
			return HSVtoRGB(H, 1.0, 1.0);			
		}
	}
	else if (options.color_mode == ColorMode::BlackAndWhite) {
		if (is_leftptr || is_rightptr)
			return RGB(127, 127, 127);//gray
		else 
			return RGB(255, 255, 255);//white
	}
	else if (options.color_mode == ColorMode::RedWhiteBlue) {
		COLORREF leftcol, rightcol, defcol;
		leftcol = rightcol = defcol = RGB(255, 255, 255);//white
		if (draw_event_info->draw_event_type == DrawEventType::Swap) {
			leftcol = RGB(0, 255, 0);//green
			rightcol = RGB(0, 255, 0);//green
		}
		else if (draw_event_info->draw_event_type == DrawEventType::Assignment) {
			leftcol = RGB(255, 255, 0);//yellow
			rightcol = RGB(255, 255, 0);//yellow
		}
		else if (draw_event_info->draw_event_type == DrawEventType::Comparison) {
			leftcol = RGB(0, 0, 255);//blue
			rightcol = RGB(255, 0, 0);//red
		}
		else if (draw_event_info->draw_event_type == DrawEventType::Access) {
			leftcol = RGB(127, 127, 127);//gray
			rightcol = RGB(127, 127, 127);//gray
		}
		return is_leftptr ? leftcol : is_rightptr ? rightcol : defcol;
	}
	else if (options.color_mode == ColorMode::Gradient) {
		if (is_leftptr || is_rightptr)
			return RGB(200, 200, 200);//light gray
		else {
			Range<int> range = { 0, (int)arr->size() - 1 };
			unsigned dist = uint_diff(arr->at(i).sorted_index_, arr->at(i).index_);

			//if element is in place color it a special color
			if (dist == 0)
				return RGB(0, 127, 255);//"pure" blue (Azure)
			
			//calc color gradient based on distance from sorted position (blue[close] -> red[far])
			double min_deg = 240.0; //240 is blue hue
			double max_deg = 360.0; //360 is red		
			double value = (double)dist / (double)arr->size();
			double H = value * (max_deg - min_deg) + min_deg;

			return HSVtoRGB(H, 1.0, 1.0);
		}
	}
	else if (options.color_mode == ColorMode::Exploding) {		
		static std::map<size_t, size_t> access;

		if (is_leftptr || is_rightptr) {
			const int decay_rate = 2;
			for (auto & a : access)
				a.second -= a.second < decay_rate ? a.second : decay_rate;
			access[i] = arr->size();
		}

		double min_deg = 0.0;
		double max_deg = 360.0;
		double value = (double)access[i] / (double)arr->size();
		if (value) {
			double H = value * (max_deg - min_deg) + min_deg;
			return HSVtoRGB(H, 1.0, 1.0);
		}
		else 
			return HSVtoRGB(0, 0.0, 1.0);
	}
	else if (options.color_mode == ColorMode::Heap) {
		int height = floor(log2(i + 1)) + 1;
		int max_height = floor(log2(arr->size())) + 1;

		double min_deg = 0.0;
		double max_deg = 360.0;
		double value = (double)height / (double)max_height;
		double H = value * (max_deg - min_deg) + min_deg;

		return HSVtoRGB(H, 1.0, 1.0);
	}
	else if (options.color_mode == ColorMode::HeatMap) {	
		static std::map<size_t, size_t> access;

		if (is_leftptr || is_rightptr) {
			const int decay_rate = 1;
			for (auto & a : access)
				a.second -= a.second < decay_rate ? a.second : decay_rate;
			access[i] = std::min(access[i] + arr->size() / 5, arr->size());
		}

		double min_deg = 0.0;
		double max_deg = 120.0;
		double value = 1.0 - (double)access[i] / (double)arr->size();
		double H = value * (max_deg - min_deg) + min_deg;
		return HSVtoRGB(H, 1.0, 1.0);
	}

	throw std::exception("Unhandled/unimplemented ColorMode case");

	return col;
}

void Visualizer::DrawScreenText(Visualizer* vis, HWND hwnd, HDC hDCmem) {
	RECT rcclient;
	GetClientRect(hwnd, &rcclient);

	//draw initializing text while initializing
	if (vis->is_initializing) {
		int font_size = std::max((int)rcclient.bottom / 15, 14);
		HFONT initializing_font = CreateFont(font_size, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, 0, L"Arial");
		DeleteObject(SelectObject(hDCmem, initializing_font));

		std::wstring wstr = L"initializing...";

		RECT rc;
		get_centered_text_rect(hDCmem, wstr, rcclient, &rc);

		DrawBorderedText(hDCmem, rc, wstr, RGB(0, 0, 0), RGB(255, 255, 255));

		DeleteFont(initializing_font);
	}

	//draw info text to screen
	{
		int font_size = std::max((int)rcclient.bottom / 30, 12);
		HFONT screen_font = CreateFont(font_size, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, L"Verdana");
		DeleteObject(SelectObject(hDCmem, screen_font));

		std::wstring wtext;
		RECT rctextlocation;
		SIZE sz;

		int xoff = 5;
		int yoff = 3;

		COLORREF textcolor = RGB(255, 255, 255);
		COLORREF bordercolor = RGB(0, 0, 0);

		/////////////////////////////////////////////////////////////////
		auto format_num_with_commas = [](std::wstring num) -> std::wstring {
			int pos = num.length() - 3;
			while (pos > 0) {
				num.insert(pos, L",");
				pos -= 3;
			}
			return num;
		};

		//draw sort name
		wtext = vis->sort.name
			+ L" (" + format_num_with_commas(std::to_wstring(vis->options.size)) + L" Elements ["
			+ vis->options.GetInitTypeValueName() + L" Values, "
			+ vis->options.GetInitTypeOrderName() + L" List])";
		GetTextExtentPoint32(hDCmem, wtext.c_str(), wtext.size(), &sz);
		rctextlocation = {xoff, yoff, sz.cx + xoff, sz.cy + yoff};
		DrawBorderedText(hDCmem, rctextlocation, wtext, bordercolor, textcolor);

		bool BST_INDETERMINATE_COND = vis->tv_state == SortState::Sorted || vis->isCursorInClient();
		if (vis->options.show_counts) {
			if (vis->options.show_counts == BST_CHECKED || BST_INDETERMINATE_COND) {
				//draw comparison count
				wtext = L"Comparisons: " + format_num_with_commas(std::to_wstring(vis->tv.compare_count));
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);

				//draw access count
				wtext = L"Accesses: " + format_num_with_commas(std::to_wstring(vis->tv.access_count));
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);

				//draw assignment count
				wtext = L"Assignments: " + format_num_with_commas(std::to_wstring(vis->tv.assignment_count));
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);

				//draw swap count
				wtext = L"Swaps: " + format_num_with_commas(std::to_wstring(vis->tv.swap_count));
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);
			}
		}

		//draw runtime
		if (vis->options.show_runtime) {
			if (vis->options.show_runtime == BST_CHECKED || BST_INDETERMINATE_COND) {
				if (true || vis->tv_state == SortState::Sorted) {
					double runtimems = vis->timer.getElapsedMS();
					wtext = (runtimems > 0.0 ? L"Runtime: " + std::to_wstring(runtimems) + L"ms" : vis->tv_state == SortState::Sorting ? L"Sorting..." : L"");
					DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);
				}
				else if (vis->tv_state == SortState::Sorting) {
					wtext = L"Sorting...";
					DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);
				}
				else if (vis->is_initializing) {
					wtext = L"Initializing...";
					DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, textcolor);
				}
			}
		}

		//////////////////////////////////////////////////////////////////

		//Draw Complexity (on right size, right justified)
		if (vis->options.show_complexity) {
			if (vis->options.show_complexity == BST_CHECKED || BST_INDETERMINATE_COND) {
				wtext = L"Complexity";
				GetTextExtentPoint32(hDCmem, wtext.c_str(), wtext.size(), &sz);
				rctextlocation = {rcclient.right - xoff - sz.cx, yoff, rcclient.right - xoff, sz.cy + yoff};
				DrawBorderedText(hDCmem, rctextlocation, wtext, RGB(0, 0, 0), RGB(255, 255, 255));

				static std::vector<std::wstring> TimeComplexityRanks = {L"O(n)", L"O(n log n)", L"O(n log\u00B2 n)", L"colorfiller", L"O(n\u00B2)"};
				static std::vector<std::wstring> SpaceComplexityRanks = {L"O(1)", L"O(log n)", L"O(n)"};

				//red to green
				auto GetComplexityColor = [](std::wstring complexity, const std::vector<std::wstring>& ComplexityRanks, COLORREF defaultcolor) -> COLORREF {
					//check if its one of the badsorts and just make the color red
					if (complexity == L"O(n^(log n))" || complexity == L"O(n!)" || complexity == L"O(n^2.7095)" ||
						complexity == L"O((n + 1)!)" || complexity == L"O(\u221E)")
						return RGB(255, 0, 0);

					//space complexity is n^2 make color red
					if (ComplexityRanks.size() == 3 && complexity == L"O(n\u00B2)") return RGB(255, 0, 0);

					auto pos = std::find(ComplexityRanks.begin(), ComplexityRanks.end(), complexity);
					if (pos == ComplexityRanks.end())
						return defaultcolor;

					double value = 1.0 - ((double)std::distance(ComplexityRanks.begin(), pos)) / (double)(ComplexityRanks.size() - 1);
					//value[0.0 - 1.0] * 120 picks a hue between 0.0(red) and 120.0(green)
					double H = value * 120, S = 1.0, V = 1.0;
					return HSVtoRGB(H, S, V);
				};

				std::wstring complexityStr = L"";
				COLORREF ComplexityColor;

				//draw best case
				complexityStr = vis->sort.best_case;
				wtext = L" Best: " + complexityStr;
				ComplexityColor = GetComplexityColor(complexityStr, TimeComplexityRanks, textcolor);
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, ComplexityColor, 1);

				//draw average case
				complexityStr = vis->sort.average_case;
				wtext = L" Average: " + complexityStr;
				ComplexityColor = GetComplexityColor(complexityStr, TimeComplexityRanks, textcolor);
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, ComplexityColor, 1);

				//draw worst case
				complexityStr = vis->sort.worst_case;
				wtext = L" Worst: " + complexityStr;
				ComplexityColor = GetComplexityColor(complexityStr, TimeComplexityRanks, textcolor);
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, ComplexityColor, 1);

				//draw space
				complexityStr = vis->sort.space;
				wtext = L" Space: " + complexityStr;
				ComplexityColor = GetComplexityColor(complexityStr, SpaceComplexityRanks, textcolor);
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, ComplexityColor, 1);

				//draw stable
				wtext = std::wstring(L" Stable: ") + (vis->sort.is_stable ? L"Yes" : L"No");
				DrawBorderedTextOnNextLine(hDCmem, wtext, &rctextlocation, bordercolor, (vis->sort.is_stable ? RGB(0, 255, 0) : RGB(255, 0, 0)), 1);
			}
		}

		DeleteFont(screen_font);
	}
}

void Visualizer::DrawScreen(Visualizer* vis, DrawEventInfo* draw_event_info) {
	//We access the internal tracevector array directly, so we don't have to worry about changing any of our counts during drawing stage
	/*ScopedVar<unsigned int> scoped_compare_count(vis->tv.compare_count, vis->tv.compare_count);
	ScopedVar<unsigned int> scoped_access_count(vis->tv.access_count, vis->tv.access_count);
	ScopedVar<unsigned int> scoped_assignment_count(vis->tv.assignment_count, vis->tv.assignment_count);
	ScopedVar<unsigned int> scoped_swap_count(vis->tv.swap_count, vis->tv.swap_count);*/

	HWND hwnd = vis->GetHWND();

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

	//draw screen
	if (vis->active_screen_mode == ScreenMode::SortVisualizer) {

		bool no_draw_event_info = draw_event_info ? false : true;
		if (no_draw_event_info) draw_event_info = new DrawEventInfo{0, 0, DrawEventType::Redraw};
		if (vis->tv.vec_.size()) {
			switch (vis->options.visualization_type) {
				case VisualizationType::BarGraph:
				case VisualizationType::FullBarGraph:
				case VisualizationType::HorizontalPyramid: DrawBarGraph(vis, hwnd, hdcmem, draw_event_info); break;
				case VisualizationType::ScatterPlot: DrawScatterPlot(vis, hwnd, hdcmem, draw_event_info); break;
				case VisualizationType::DisparityDots: DrawDisparityDots(vis, hwnd, hdcmem, draw_event_info); break;
				case VisualizationType::ColorWheel: DrawColorWheel(vis, hwnd, hdcmem, draw_event_info); break;
				case VisualizationType::Squares: DrawSquares(vis, hwnd, hdcmem, draw_event_info); break;
			}
		}
		if (no_draw_event_info) delete draw_event_info;

		DrawScreenText(vis, hwnd, hdcmem);
	}

	//highlight options selection
	if (vis == getVisualizer(Options::hwnd)) {
		COLORREF framecolor = RGB(0, 255, 0);
		HBRUSH framebrush = CreateSolidBrush(framecolor);
		RECT framerect = rcclient;
		FrameRect(hdcmem, &framerect, framebrush);
		InflateRect(&framerect, -1, -1);
		FrameRect(hdcmem, &framerect, framebrush);
		InflateRect(&framerect, -1, -1);
		FrameRect(hdcmem, &framerect, framebrush);
		DeleteObject(framebrush);
	}
	else if (Visualizer::Options::hwnds_MultiSelected.find(hwnd) != Visualizer::Options::hwnds_MultiSelected.end()) {
		COLORREF framecolor = RGB(0, 0, 255);
		HBRUSH framebrush = CreateSolidBrush(framecolor);
		RECT framerect = rcclient;
		FrameRect(hdcmem, &framerect, framebrush);
		InflateRect(&framerect, -1, -1);
		FrameRect(hdcmem, &framerect, framebrush);
		InflateRect(&framerect, -1, -1);
		FrameRect(hdcmem, &framerect, framebrush);
		DeleteObject(framebrush);
	}

	//draw the screen
	BitBlt(hdc, 0, 0, rcclient.right, rcclient.bottom, hdcmem, 0, 0, SRCCOPY);

	//cleanup
	SelectObject(hdcmem, hbmOldBitmap);
	DeleteObject(hbmScreen);

	DeleteDC(hdcmem);
	ReleaseDC(hwnd, hdc);

	bm.stop();
//	printf("%d           \r", bm.getElapsed());
}

void Visualizer::DrawBarGraph(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info) {
	std::vector<TraceInt>* arr = &vis->tv.vec_;

	double wf = (double)vis->screen.w / (double)arr->size();
	double hf = ((double)vis->screen.h / (double)arr->size());

	auto DrawBar = [&](unsigned i) {
		if (i >= arr->size()) return;
		unsigned bar_height = (unsigned)(hf * (double)(arr->at(i)));

		unsigned x_start = (unsigned)std::ceil(wf * (double)i);
		unsigned x_end = std::max((unsigned)std::ceil(wf * (double)(i + 1)), x_start + 1);

		unsigned y_start = 0;//bottom
		unsigned y_end = 0;//top
		if (vis->options.visualization_type == VisualizationType::BarGraph) {
			y_start = vis->screen.h - 1;
			y_end = vis->screen.h - bar_height;
		}
		else if (vis->options.visualization_type == VisualizationType::HorizontalPyramid) {
			y_end = (vis->screen.h - bar_height + 1) / 2;
			y_start = y_end + bar_height;
		}
		else if (vis->options.visualization_type == VisualizationType::FullBarGraph) {
			y_start = vis->screen.h - 1;
			y_end = 0;
		}

		COLORREF bar_color = vis->GetColor(arr, i, draw_event_info);

		/*RECT bar_rect = { x_start, y_end, x_end, y_start };
		RECT back_rect = { x_start, 0, x_end, y_end};
		HBRUSH back_brush = CreateSolidBrush(vis->screen.back_color);
		HBRUSH bar_brush = CreateSolidBrush(bar_color);
		FillRect(hdc, &back_rect, back_brush);		
		FillRect(hdc, &bar_rect, bar_brush);
		DeleteObject(back_brush);
		DeleteObject(bar_brush);*/
		
		for (unsigned y = 0; y < vis->screen.h; ++y) {
			for (unsigned x = x_start; x < x_end && x < vis->screen.w; ++x) {
				int pos = y * vis->screen.stride + x * 4;

				COLORREF col = (y <= y_start && y > y_end) ? bar_color : vis->screen.back_color;
				
				if (vis->screen.GetData()) {
					vis->screen.SetData(pos + 2, GetRValue(col));
					vis->screen.SetData(pos + 1, GetGValue(col));
					vis->screen.SetData(pos + 0, GetBValue(col));
				}
			}
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////
	//nothing specific to draw so draw the whole thing (no highlights)
	if (true || draw_event_info->draw_event_type == DrawEventType::Redraw) {
		vis->reset_last_draw();
		vis->num_skipped = 0;

		//only paint once per column of pixels (step over the rest, they would just get painted over anyways)
		double step = std::max((double)arr->size() / (double)vis->screen.w, 1.0);
		unsigned count = std::min(arr->size(), (unsigned)vis->screen.w);
		for (unsigned i = 0; i < count; ++i) {
			DrawBar((unsigned)((double)i * step));			
		}
	}
	else { //only draw the changes
		//draw anything that was skipped and draw only once per pixel column
		if (vis->options.bfast_draw && vis->num_skipped > 0) {
			ScopedVar<Visualizer::DrawEventType> scoped_draw_event_type(draw_event_info->draw_event_type, DrawEventType::DrawSkipped);
	
			//number of skipped draws exceeds the number of pixel columns?
			if (vis->num_skipped >= vis->screen.w) {
				//TODO fix draw gaps
				long double step = (double)arr->size() / ((double)vis->screen.w * 1.0);// * 1.5 seems to fix draw gaps (?caused by precision error?)
				
				//crush each each index into its pixel column(s)
				std::set<unsigned> cols;
				for (unsigned i = 0; i < vis->skipped_draws.size(); i++) {
					cols.insert((unsigned)((long double)vis->skipped_draws[i].ptr1 / step));
					if (isDrawEventDouble(vis->skipped_draws[i].draw_event_type))
						cols.insert((unsigned)((long double)vis->skipped_draws[i].ptr2 / step));
				}
				//draw each pixel column(s)
				for (auto a : cols)
					DrawBar((unsigned)(step * (long double)a));
			}
			else {
				for (unsigned i = 0; i < vis->skipped_draws.size(); i++) {
					DrawBar(vis->skipped_draws[i].ptr1);
					if (isDrawEventDouble(vis->skipped_draws[i].draw_event_type))
						DrawBar(vis->skipped_draws[i].ptr2);
				}
			}
		}

		//draw current change
		if (isDrawEventSingle(draw_event_info->draw_event_type)) {
			DrawBar(draw_event_info->ptr1);
			if (isDrawEventDouble(draw_event_info->draw_event_type)) {
				DrawBar(draw_event_info->ptr2);
			}
		}
		
		//redraw last change
		if (isDrawEventSingle(vis->last_draw.draw_event_type)) {
			{
				ScopedVar<DrawEventType> scoped_last_draw_draw_event_type(vis->last_draw.draw_event_type, DrawEventType::DrawSkipped);
				DrawBar(vis->last_draw.ptr1);
			}
			if (isDrawEventDouble(vis->last_draw.draw_event_type)) {
				ScopedVar<DrawEventType> scoped_last_draw_draw_event_type(vis->last_draw.draw_event_type, DrawEventType::DrawSkipped);
				DrawBar(vis->last_draw.ptr2);
			}
		}	

		vis->last_draw.ptr1 = draw_event_info->ptr1;
		vis->last_draw.ptr2 = draw_event_info->ptr2;
		vis->last_draw.draw_event_type = draw_event_info->draw_event_type;
	}

	///////////////////////////////////////////////////////////////////////////////////////

	BITMAPINFOHEADER bmih = { sizeof(BITMAPINFOHEADER), (int)((long long)vis->screen.w), (int)(-(long long)vis->screen.h), 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	BITMAPINFO bmi = { bmih, 0, 0, 0, 0 };
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, vis->screen.GetData(), &bmi, DIB_RGB_COLORS);
	HBITMAPBlitToHdc(hdc, hbmp, 0, 0, vis->screen.w, vis->screen.h);
	DeleteObject(hbmp);

	if (vis->options.sleep_time) std::this_thread::sleep_for(std::chrono::milliseconds(vis->options.sleep_time));
}

void Visualizer::DrawScatterPlot(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info) {
	std::vector<TraceInt>* arr = &vis->tv.vec_;

	///////////////////////////////////////////////////////////////////////////////////////

	double wf = (double)vis->screen.w / (double)arr->size();
	double hf = (double)vis->screen.h / (double)arr->size();

	const double dot_w = 2.0;
	static unsigned floor_dot_w = (unsigned)floor(dot_w / 2.0);
	static unsigned ceil_dot_w = (unsigned)ceil(dot_w / 2.0);

	auto DrawDot = [&](unsigned i) {
		unsigned ypos = (unsigned)(hf * (double)arr->at(i));		

		long long x_start = (int)(wf * (double)i) - floor_dot_w;
		long long x_end = (int)(wf * (double)i) + ceil_dot_w;

		long long y_start = vis->screen.h - ypos - floor_dot_w - 1;//top
		long long y_end = vis->screen.h - ypos + ceil_dot_w;//bottom

		x_start = x_start < 0 ? 0 : x_start;
		x_end = x_end > vis->screen.w ? vis->screen.w: x_end;

		y_start = y_start < 0 ? 0 : y_start;
		y_end = y_end > vis->screen.h ? vis->screen.h: y_end;

		COLORREF dot_color = vis->GetColor(arr, i, draw_event_info);

		for (unsigned y = y_start; y < y_end; ++y) {
			for (unsigned x = x_start; x <= x_end; ++x) {
				int pos = y * vis->screen.stride + x * 4 + vis->screen.stride + 4;
				
			//	COLORREF col = (y <= y_start && y > y_end) ? dot_color : vis->back_color;
				
				vis->screen.SetData(pos + 2, GetRValue(dot_color));
				vis->screen.SetData(pos + 1, GetGValue(dot_color));
				vis->screen.SetData(pos + 0, GetBValue(dot_color));
			}
		}
	};

	auto RedrawDot = [&](unsigned i, unsigned v) {		
		unsigned ypos = (unsigned)(hf * (double)v);

		unsigned x_start = std::max((int)(wf * (double)i) - floor_dot_w, 0u);
		unsigned x_end = std::min((int)(wf * (double)i) + ceil_dot_w, (unsigned)vis->screen.w);

		unsigned y_start = std::max(vis->screen.h - ypos - floor_dot_w - 1, 0u);//top
		unsigned y_end = std::min(vis->screen.h - ypos + ceil_dot_w, (unsigned)vis->screen.h);//bottom

		COLORREF dot_color = RGB(255, 0, 0);//vis->back_color;// vis->GetColor(arr, i, draw_event_info);

		for (unsigned y = y_start; y < y_end; ++y) {
			for (unsigned x = x_start; x <= x_end; ++x) {
				int pos = y * vis->screen.stride + x * 4 + vis->screen.stride + 4;

				//	COLORREF col = (y <= y_start && y > y_end) ? dot_color : vis->back_color;

				vis->screen.SetData(pos + 2, GetRValue(dot_color));
				vis->screen.SetData(pos + 1, GetGValue(dot_color));
				vis->screen.SetData(pos + 0, GetBValue(dot_color));
			}
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////
	//nothing specific to draw so draw the whole thing (no highlights)
	if (draw_event_info->draw_event_type == DrawEventType::Redraw) {
		vis->reset_last_draw();

		vis->FillBackground();

		double step = (double)arr->size() / (double)vis->screen.w;
		double size = (double)arr->size();
		//for (double i = 0.0; i < size; i += step)
		for (unsigned i = 0; i < arr->size(); i++)
			DrawDot((unsigned)i);
	}
	else { //only draw changes
		if (vis->options.bfast_draw && vis->num_skipped > 0) {
			ScopedVar<Visualizer::DrawEventType> scoped_draw_event_type(draw_event_info->draw_event_type, DrawEventType::DrawSkipped);

			double step = std::max((double)arr->size() / (double)vis->screen.w, 1.0);
			//crush each each index into its pixel column(s)
			std::set<unsigned> cols;
			for (unsigned i = 0; i < vis->skipped_draws.size(); i++) {
				cols.insert((unsigned)((double)vis->skipped_draws[i].ptr1 / step));
				if (isDrawEventDouble(vis->skipped_draws[i].draw_event_type))
					cols.insert((unsigned)((double)vis->skipped_draws[i].ptr2 / step));
			}
			
			for (auto a : cols) {
				unsigned i = (unsigned)(step * (double)a);
				DrawDot(i);
			}
		}

		//draw current change
		if (isDrawEventSingle(draw_event_info->draw_event_type)) {
			DrawDot(draw_event_info->ptr1);
			if (isDrawEventDouble(draw_event_info->draw_event_type)) {
				DrawDot(draw_event_info->ptr2);
			}
		}

		//redraw last change
		if (isDrawEventSingle(vis->last_draw.draw_event_type)) {				
			RedrawDot(vis->last_draw.ptr1, vis->last_draw_value.ptr1);			
			if (isDrawEventDouble(vis->last_draw.draw_event_type)) {				
				RedrawDot(vis->last_draw.ptr2, vis->last_draw_value.ptr2);
			}
		}

		vis->last_draw.ptr1 = draw_event_info->ptr1;
		vis->last_draw.ptr2 = draw_event_info->ptr2;
		vis->last_draw.draw_event_type = draw_event_info->draw_event_type;
		vis->last_draw_value.ptr1 = arr->at(draw_event_info->ptr1);
		vis->last_draw_value.ptr2 = arr->at(draw_event_info->ptr2);	
	}

	///////////////////////////////////////////////////////////////////////////////////////

	BITMAPINFOHEADER bmih = { sizeof(BITMAPINFOHEADER), (int)((long long)vis->screen.w), (int)(-(long long)vis->screen.h), 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	BITMAPINFO bmi = { bmih, 0, 0, 0, 0 };
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, vis->screen.GetData(), &bmi, DIB_RGB_COLORS);
	HBITMAPBlitToHdc(hdc, hbmp, 0, 0, vis->screen.w, vis->screen.h);
	DeleteObject(hbmp);

	if (vis->options.sleep_time) std::this_thread::sleep_for(std::chrono::milliseconds(vis->options.sleep_time));
}

void Visualizer::DrawColorWheel(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info) {
	std::vector<TraceInt>* arr = &vis->tv.vec_;

	///////////////////////////////////////////////////////////////////////////////////////

	double wf = (double)vis->screen.w / (double)arr->size();
	double hf = (double)vis->screen.h / (double)arr->size();

	double diameter = (double)arr->size();
	double radius = diameter / 2.1;
	double center = diameter / 2.0;

	double wr = (double)vis->screen.w / 2.0, hr = (double)vis->screen.h / 2.0;
	double perimeter = (2.0 * PI) * (3.0 * (wr + hr) - sqrt((3.0 * wr + hr) * (wr + 3.0 * hr)));

	Range<double> indexes(0.0, (double)arr->size());
	Range<double> arcs(0.0, perimeter);
	static Range<double> degrees(0.0, 360.0);
	static Range<double> radians(0.0, 2.0 * PI);

	auto DrawArc = [&](unsigned i) {
		double theta_s;
		double theta_e;

		if (indexes.max > arcs.max) {
			unsigned arc = (unsigned)indexes.MapTo((double)i, arcs);
			theta_s = arcs.MapTo((double)arc, radians);
			theta_e = arcs.MapTo((double)(arc + 1.0), radians);
		}
		else {
			theta_s = indexes.MapTo((double)i, radians);
			theta_e = indexes.MapTo((double)(i + 1.0), radians);
		}

		POINT c = {center * wf, center * hf};
		POINT p1, p2;

		p1.x = (long)(wf * (center + radius * cos(theta_s)));
		p1.y = vis->screen.h - (long)(hf * (center + radius * sin(theta_s)));

		p2.x = (long)(wf * (center + radius * cos(theta_e)));
		p2.y = vis->screen.h - (long)(hf * (center + radius * sin(theta_e)));

		/*auto lrfml = Sort3(c.x, p1.x, p2.x);
		auto tbfml = Sort3(c.y, p1.y, p2.y);*/

		COLORREF arc_color = vis->GetColor(arr, i, draw_event_info);

		HPEN hPen = CreatePen(PS_SOLID, 1, arc_color);
		HPEN hOldPen = SelectPen(hdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(arc_color);
		HBRUSH hOldBrush = SelectBrush(hdc, hBrush);	
		POINT vertices[] = {p1, p2, c};
		Polygon(hdc, vertices, 3);	
		SelectBrush(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectPen(hdc, hOldPen);
		DeleteObject(hPen);	
	};

	if (true || draw_event_info->draw_event_type == DrawEventType::Redraw) {
		vis->reset_last_draw();

	//	vis->FillBackground();

		double dsize = (double)arr->size();
		double step = dsize / perimeter;
	
		if (dsize > perimeter) 
			for (double i = 0.0; i < dsize; i += step) 
				DrawArc((unsigned)i);
		else 
			for (unsigned i = 0; i < arr->size(); ++i) 
				DrawArc(i);	
	}
	else {
		//draw anything that was skipped and draw only once per arc
		if (vis->options.bfast_draw && vis->num_skipped > 0) {
			ScopedVar<Visualizer::DrawEventType> scoped_draw_event_type(draw_event_info->draw_event_type, DrawEventType::DrawSkipped);

			//number is skipped draws exceeds the number of pixel columns
			//if (vis->num_skipped >= vis->w) {
			//	long double step = (double)arr->size() / ((double)vis->w * 1.5);// * 1.5 seems to fix draw gaps (?caused by precision error?)

			//	//crush each each index into its pixel column(s)
			//	std::set<unsigned> cols;
			//	for (unsigned i = 0; i < vis->skipped_draws.size(); i++) {
			//		cols.insert((unsigned)((long double)vis->skipped_draws[i].ptr1 / step));
			//		if (isDrawEventDouble(vis->skipped_draws[i].draw_event_type))
			//			cols.insert((unsigned)((long double)vis->skipped_draws[i].ptr2 / step));
			//	}
			//	//draw each pixel column(s)
			//	for (auto a : cols)
			//		DrawBar((unsigned)(step * (long double)a));
			//}
			//else {
				for (unsigned i = 0; i < vis->skipped_draws.size(); i++) {
					DrawArc(vis->skipped_draws[i].ptr1);
					if (isDrawEventDouble(vis->skipped_draws[i].draw_event_type))
						DrawArc(vis->skipped_draws[i].ptr2);
				}
			/*}*/
		}


		//draw current change
		if (isDrawEventSingle(draw_event_info->draw_event_type)) {
			DrawArc(draw_event_info->ptr1);
			if (isDrawEventDouble(draw_event_info->draw_event_type)) {
				DrawArc(draw_event_info->ptr2);
			}
		}

		//redraw last change
		if (isDrawEventSingle(vis->last_draw.draw_event_type)) {
			{
				ScopedVar<DrawEventType> scoped_last_draw_draw_event_type(vis->last_draw.draw_event_type, DrawEventType::DrawSkipped);
				DrawArc(vis->last_draw.ptr1);
			}
			if (isDrawEventDouble(vis->last_draw.draw_event_type)) {
				ScopedVar<DrawEventType> scoped_last_draw_draw_event_type(vis->last_draw.draw_event_type, DrawEventType::DrawSkipped);
				DrawArc(vis->last_draw.ptr2);
			}
		}

		vis->last_draw.ptr1 = draw_event_info->ptr1;
		vis->last_draw.ptr2 = draw_event_info->ptr2;
		vis->last_draw.draw_event_type = draw_event_info->draw_event_type;
		vis->last_draw_value.ptr1 = arr->at(draw_event_info->ptr1);
		vis->last_draw_value.ptr2 = arr->at(draw_event_info->ptr2);
	}

	///////////////////////////////////////////////////////////////////////////////////////

	/*BITMAPINFOHEADER bmih = {sizeof(BITMAPINFOHEADER), (int)((long long)vis->w), (int)(-(long long)vis->h), 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	BITMAPINFO bmi = {bmih, 0, 0, 0, 0};
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, vis->bmp_screen, &bmi, DIB_RGB_COLORS);
	HBITMAPBlitToHdc(hdc, hbmp, 0, 0, vis->w, vis->h);
	DeleteObject(hbmp);*/

	if (vis->options.sleep_time) std::this_thread::sleep_for(std::chrono::milliseconds(vis->options.sleep_time));
}

void Visualizer::DrawDisparityDots(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info) {
	std::vector<TraceInt>* arr = &vis->tv.vec_;

	///////////////////////////////////////////////////////////////////////////////////////

	double wf = (double)vis->screen.w / (double)arr->size();
	double hf = (double)vis->screen.h / (double)arr->size();

	Range<double> nums(1.0, (double)arr->size());
	static Range<double> radians(0.0, 2.0 * PI);

	double diameter = (double)arr->size();
	double center = diameter / 2.0;

	auto DrawDot = [&](unsigned i) {	
		double dist = (double)uint_diff(arr->at(i).sorted_index_, i);
		dist = std::max(dist, diameter - dist);
		double radius = dist / 2.1;		
		double theta = nums.MapTo((double)i, radians);

		unsigned x = (unsigned)(wf * (center + radius * cos(theta)));
		unsigned y = vis->screen.h - (unsigned)(hf * (center + radius * sin(theta)));

		unsigned x_start = x - 2;//left
		unsigned x_end = x + 1;//right

		unsigned y_start = vis->screen.h - y + 1;//bottom
		unsigned y_end = vis->screen.h - y - 2;//top

		COLORREF dot_color = vis->GetColor(arr, i, draw_event_info);

		for (unsigned y = y_start; y > y_end; --y) {
			for (unsigned x = x_start; x <= x_end; ++x) {
				unsigned pos = y * vis->screen.stride + x * 4 + vis->screen.stride + 4;

				vis->screen.SetData(pos + 2, GetRValue(dot_color));
				vis->screen.SetData(pos + 1, GetGValue(dot_color));
				vis->screen.SetData(pos + 0, GetBValue(dot_color));
			}
		}
	};

	auto ReDrawDotArea = [&](unsigned i/*, unsigned v*/) {
		double t = nums.MapTo((double)i, radians);

		double dist = (double)uint_diff(arr->at(i).sorted_index_, arr->at(i).index_) / 2.0;
		double d = (double)(arr->size() - 1);
		double r = (d - dist) / 2.1;//may be negative
		double center = d / 2.0;
		unsigned x = (unsigned)(wf * (center + r * cos(t)));
		unsigned y = vis->screen.h - (unsigned)(hf * (center + r * sin(t)));

		unsigned x_start = x - 2;
		unsigned x_end = x + 1;

		unsigned y_start = vis->screen.h - y + 1;//bottom
		unsigned y_end = vis->screen.h - y - 2;//top

		for (unsigned y = y_start; y > y_end; --y) {
			for (unsigned x = x_start; x <= x_end; ++x) {
				unsigned pos = y * vis->screen.stride + x * 4 + vis->screen.stride + 4;

				/*vis->bmp_screen[pos + 2] = GetRValue(vis->back_color);
				vis->bmp_screen[pos + 1] = GetGValue(vis->back_color);
				vis->bmp_screen[pos + 0] = GetBValue(vis->back_color);*/

				vis->screen.SetData(pos + 2, GetRValue(RGB(255, 0, 0)));
				vis->screen.SetData(pos + 1, GetGValue(RGB(255, 0, 0)));
				vis->screen.SetData(pos + 0, GetBValue(RGB(255, 0, 0)));
			}
		}
	};

	if (true || draw_event_info->draw_event_type == DrawEventType::Redraw) {
		vis->reset_last_draw();

		vis->FillBackground();

		for (unsigned i = 0; i < arr->size(); i++) {
			DrawDot(i);
		}
	}
	else {
		//draw current change
		if (isDrawEventSingle(draw_event_info->draw_event_type)) {
			DrawDot(draw_event_info->ptr1);
			if (isDrawEventDouble(draw_event_info->draw_event_type)) {
				DrawDot(draw_event_info->ptr2);
			}
		}	

		//redraw last change
		if (isDrawEventSingle(vis->last_draw.draw_event_type)) {
			{
				ScopedVar<DrawEventType> scoped_last_draw_draw_event_type(vis->last_draw.draw_event_type, DrawEventType::DrawSkipped);
				ReDrawDotArea(vis->last_draw.ptr1);
			}
			if (isDrawEventDouble(vis->last_draw.draw_event_type)) {
				ScopedVar<DrawEventType> scoped_last_draw_draw_event_type(vis->last_draw.draw_event_type, DrawEventType::DrawSkipped);
				ReDrawDotArea(vis->last_draw.ptr2);
			}
		}

		vis->last_draw.ptr1 = draw_event_info->ptr1;
		vis->last_draw.ptr2 = draw_event_info->ptr2;
		vis->last_draw.draw_event_type = draw_event_info->draw_event_type;
		vis->last_draw_value.ptr1 = arr->at(draw_event_info->ptr1);
		vis->last_draw_value.ptr2 = arr->at(draw_event_info->ptr2);
	}

	///////////////////////////////////////////////////////////////////////////////////////

	BITMAPINFOHEADER bmih = { sizeof(BITMAPINFOHEADER), (int)((long long)vis->screen.w), (int)(-(long long)vis->screen.h), 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	BITMAPINFO bmi = { bmih, 0, 0, 0, 0 };
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, vis->screen.GetData(), &bmi, DIB_RGB_COLORS);
	HBITMAPBlitToHdc(hdc, hbmp, 0, 0, vis->screen.w, vis->screen.h);
	DeleteObject(hbmp);

	if (vis->options.sleep_time) std::this_thread::sleep_for(std::chrono::milliseconds(vis->options.sleep_time));
}

void Visualizer::DrawSquares(Visualizer* vis, HWND hwnd, HDC hdc, DrawEventInfo* draw_event_info) {
	std::vector<TraceInt>* arr = &vis->tv.vec_;

	///////////////////////////////////////////////////////////////////////////////////////

	double wf = (double)vis->screen.w / (double)arr->size();
	double hf = (double)vis->screen.h / (double)arr->size();

	const double dot_w = 2.0;
	static unsigned floor_dot_w = (unsigned)floor(dot_w / 2.0);
	static unsigned ceil_dot_w = (unsigned)ceil(dot_w / 2.0);

	auto DrawDot = [&](unsigned i) {
		unsigned ypos = (unsigned)(hf * (double)arr->at(i));

		long long x_start = (int)(wf * (double)i) - floor_dot_w;
		long long x_end = (int)(wf * (double)i) + ceil_dot_w;

		long long y_start = vis->screen.h - ypos - floor_dot_w - 1;//top
		long long y_end = vis->screen.h - ypos + ceil_dot_w;//bottom

		x_start = x_start < 0 ? 0 : x_start;
		x_end = x_end > vis->screen.w ? vis->screen.w : x_end;

		y_start = y_start < 0 ? 0 : y_start;
		y_end = y_end > vis->screen.h ? vis->screen.h : y_end;

		COLORREF dot_color = vis->GetColor(arr, i, draw_event_info);

		for (unsigned y = y_start; y < y_end; ++y) {
			for (unsigned x = x_start; x <= x_end; ++x) {
				int pos = y * vis->screen.stride + x * 4 + vis->screen.stride + 4;

				//	COLORREF col = (y <= y_start && y > y_end) ? dot_color : vis->back_color;

				vis->screen.SetData(pos + 2, GetRValue(dot_color));
				vis->screen.SetData(pos + 1, GetGValue(dot_color));
				vis->screen.SetData(pos + 0, GetBValue(dot_color));
			}
		}
	};

	auto DrawSquare = [&](unsigned i) {

	};

	if (true || draw_event_info->draw_event_type == DrawEventType::Redraw) {
		vis->reset_last_draw();
	
		if (arr->size() > vis->screen.w * vis->screen.h) {
			double dsize = (double)arr->size();
			double step = dsize / (double)(vis->screen.w * vis->screen.h);
			for (double i = 0.0; i < dsize; i += step)
				DrawSquare((unsigned)i);
		}
		else
			for (unsigned i = 0; i < arr->size(); ++i)
				DrawSquare(i);
	}
	else { //only draw changes
		//if (vis->options.bfast_draw && vis->num_skipped > 0) {
		//	ScopedVar<Visualizer::DrawEventType> scoped_draw_event_type(draw_event_info->draw_event_type, DrawEventType::DrawSkipped);

		//	double step = std::max((double)arr->size() / (double)vis->w, 1.0);
		//	//crush each each index into its pixel column(s)
		//	std::set<unsigned> cols;
		//	for (unsigned i = 0; i < vis->skipped_draws.size(); i++) {
		//		cols.insert((unsigned)((double)vis->skipped_draws[i].ptr1 / step));
		//		if (isDrawEventDouble(vis->skipped_draws[i].draw_event_type))
		//			cols.insert((unsigned)((double)vis->skipped_draws[i].ptr2 / step));
		//	}

		//	for (auto a : cols) {
		//		unsigned i = (unsigned)(step * (double)a);
		//		DrawDot(i);
		//	}
		//}

		////draw current change
		//if (isDrawEventSingle(draw_event_info->draw_event_type)) {
		//	DrawDot(draw_event_info->ptr1);
		//	if (isDrawEventDouble(draw_event_info->draw_event_type)) {
		//		DrawDot(draw_event_info->ptr2);
		//	}
		//}

		////redraw last change
		//if (isDrawEventSingle(vis->last_draw.draw_event_type)) {
		//	RedrawDot(vis->last_draw.ptr1, vis->last_draw_value.ptr1);
		//	if (isDrawEventDouble(vis->last_draw.draw_event_type)) {
		//		RedrawDot(vis->last_draw.ptr2, vis->last_draw_value.ptr2);
		//	}
		//}

		vis->last_draw.ptr1 = draw_event_info->ptr1;
		vis->last_draw.ptr2 = draw_event_info->ptr2;
		vis->last_draw.draw_event_type = draw_event_info->draw_event_type;
		vis->last_draw_value.ptr1 = arr->at(draw_event_info->ptr1);
		vis->last_draw_value.ptr2 = arr->at(draw_event_info->ptr2);
	}

	///////////////////////////////////////////////////////////////////////////////////////

	BITMAPINFOHEADER bmih = {sizeof(BITMAPINFOHEADER), (int)((long long)vis->screen.w), (int)(-(long long)vis->screen.h), 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	BITMAPINFO bmi = {bmih, 0, 0, 0, 0};
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, vis->screen.GetData(), &bmi, DIB_RGB_COLORS);
	HBITMAPBlitToHdc(hdc, hbmp, 0, 0, vis->screen.w, vis->screen.h);
	DeleteObject(hbmp);

	if (vis->options.sleep_time) std::this_thread::sleep_for(std::chrono::milliseconds(vis->options.sleep_time));
}


