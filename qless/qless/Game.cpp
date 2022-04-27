#include "Game.h"

#define CONSOLE //console is active if in debug mode (define NOCONSOLE to turn off), release mode turns off console unless CONSOLE is defined

#ifndef NOCONSOLE
#ifdef _DEBUG
#define CONSOLE
#endif
#endif

#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>

#ifdef CONSOLE
#include <iostream>
#endif

#include "Die.h"

#include "resource.h"

const std::vector<std::string> g_dice = {
	"MMLLBY",
	"VFGKPP",
	"HHNNRR",
	"DFRLLW",
	"RRDLGG",
	"XKBSZN",
	"WHHTTP",
	"CCBTJD",
	"CCMTTS",
	"OIINNY",
	"AEIOUU",
	"AAEEOO"
};

//const std::vector<std::string> g_dice = {
//	"L",
//	"R",
//	"K",
//	"W",
//	"D",
//	"S",
//	"W",
//	"J",
//	"T",
//	"N",
//	"O",
//	"O"
//};

#ifdef CONSOLE
void SHOW_CONSOLE(bool show/* = true*/, bool noclose /*= false*/) {
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
#endif

std::string int_to_str(int num, bool is_hex = false) {
	std::stringstream out;
	out << (is_hex ? std::hex : std::dec) << num;
	std::string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}

std::string wstr_to_str(std::wstring ws) {
	std::string ret(ws.begin(), ws.end());
	return ret;
}

std::string error_code_to_text(DWORD error_code) {
	std::string ret;
	LPTSTR buf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0, NULL);
	return wstr_to_str(buf) + '(' + int_to_str((int)error_code) + ')';
}

ATOM registergamewindowclassname(HINSTANCE hInstance) {
	WNDCLASSEX wnd = {0};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.hInstance = hInstance;
	wnd.lpszClassName = GAMECLASSNAME;
	wnd.style = NULL; // CS_GLOBALCLASS;
	wnd.lpfnWndProc = (WNDPROC)Game::Proc;
	return RegisterClassEx(&wnd);
}

Game::Game(HINSTANCE hinst) {
	static bool once = false;
	if (once) return;
	once = true;

	registergamewindowclassname(hinst);

#ifdef CONSOLE
	SHOW_CONSOLE(true, false);
#endif

	this->hinst = hinst;

	for (int i = 0; i < g_dice.size(); ++i) 
		this->dice.push_back(new Die(g_dice[i]));

	this->selected_boxs.clear();

	int board_size = g_dice.size();
	for (int i = 0; i < board_size; i++) {
		std::vector<Die*> t(board_size);
		board.push_back(t);
	}

	this->LoadWords();

	RECT rcmonitor;
	GetWindowRect(GetDesktopWindow(), &rcmonitor);
	int window_height = rcmonitor.bottom * (2.0 / 3.0);
	int window_width = window_height;
	int window_x = (rcmonitor.right - window_width) / 2;
	int window_y = (rcmonitor.bottom - window_height) / 2;
	RECT rcw = {0, 0, window_width, window_height};
	AdjustWindowRect(&rcw, WS_OVERLAPPEDWINDOW, FALSE);
	window_width = rcw.right - rcw.left;
	window_height = rcw.bottom - rcw.top;
	
	this->hwnd = CreateWindow(GAMECLASSNAME, L"Q-LESS", WS_OVERLAPPEDWINDOW | WS_VISIBLE, window_x, window_y, window_width, window_height, NULL, NULL, this->hinst, this);

	this->newGame();

	ShowWindow(this->hwnd, SW_SHOW);

	
}

Game::~Game() {
	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, 0);

	for (int i = 0; i < this->board.size(); i++) {
		for (int j = 0; j < this->board.size(); j++) {
			delete this->board[i][j];
			this->board[i][j] = nullptr;
		}
		this->board[i].clear();
	}
	this->board.clear();

	this->dice.clear();
}

void Game::resize() {
	GetClientRect(GetParent(this->hwnd), &this->rcclient);

	DrawScreen(this);
}

void Game::newGame() {
	this->selected_boxs.clear();
	resetDicePositions();
	rollDice();
	clearWords();
	this->Solve();
	this->DrawScreen(this);
}

void Game::resetGame() {
	this->selected_boxs.clear();
	resetDicePositions();
	clearWords();
	this->DrawScreen(this);
}

Game* Game::getGame(HWND hwnd) {
	return (Game*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

LRESULT Game::Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Game* game = Game::getGame(hwnd);

	switch (message) {
		case WM_NCCREATE: {
			game = (Game*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			game->hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)game);

			break;
		}
		case WM_PAINT: {
			DrawScreen(game);

			break;
		}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN: {
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			game->isdragging = true;
			SetCapture(hwnd);
			game->dragcount = 0;
			game->dragstartpoint = pt;

			break;
		}
		case WM_LBUTTONUP: {
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if (game->isdragging) {
				ReleaseCapture();
				game->isdragging = false;
				game->dragstartpoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};				
			}

			//get drag selections else do single click
			if (game->dragcount >= game->maxdragcount) {
				game->selected_boxs.clear();
				for (int i = 0; i < game->board.size(); i++) {
					for (int j = 0; j < game->board[i].size(); j++) {
						RECT rcbox = game->getBoxRECT(i, j);
						if (game->board[i][j]) {//only get boxes with letters
							if (rcbox.left < game->selectionrc.right && rcbox.right > game->selectionrc.left 
								&& rcbox.top < game->selectionrc.bottom && rcbox.bottom > game->selectionrc.top) {//boxes that overlap with selection rc								
								game->selected_boxs.push_back({j, i});
							}
						}
					}
				}
			}
			else {
				//not dragging so find box we clicked on
				for (int i = 0; i < game->board.size(); i++) {
					for (int j = 0; j < game->board[i].size(); j++) {
						RECT rcbox = game->getBoxRECT(i, j);
						POINT loc = {j, i};
						if (PtInRect(&rcbox, pt)) {//is this the box?
							if (game->board[i][j]) {//found it! is it a box with a letter?
								if (game->selected_boxs.size() != 1) {//select one thing if nothing is selected or if multiple are selected
									game->selected_boxs.clear();
									game->selected_boxs.push_back(loc);
								}
								else {
									game->selected_boxs.clear();//unselect the thing we have
								}
							}
							else { //empty box
								if (game->selected_boxs.size() > 0) {//we didn't click on a box with a letter, so move the selection (if we have one) to empty space (where we clicked)
									game->MoveSelectedDice({j, i});
								}
								game->selected_boxs.clear();
							}
							goto exit;//we found the box we clicked on, there isn't going to be another pt to find so exit
						}
					}
				}
			}

			exit:
			if (game->isdragging) {
				game->selectionrc = {0, 0, 0, 0};
			}

			game->DrawScreen(game);

			break;
		}
		case WM_RBUTTONUP: {
			game->resetGame();
			break;
		}
		case WM_MOUSEMOVE: {
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if (game->isdragging) {
				game->dragcount = std::min(game->dragcount + 1, game->maxdragcount);
				if (game->dragcount >= game->maxdragcount) {
					RECT rcclient;
					GetClientRect(hwnd, &rcclient);
					game->selectionrc.left = std::min(game->dragstartpoint.x, pt.x); game->selectionrc.left = rcclient.left > game->selectionrc.left ? rcclient.left : game->selectionrc.left; //max
					game->selectionrc.right = std::max(game->dragstartpoint.x, pt.x); game->selectionrc.right = rcclient.right < game->selectionrc.right ? rcclient.right : game->selectionrc.right; //min
					game->selectionrc.top = std::min(game->dragstartpoint.y, pt.y); game->selectionrc.top = rcclient.top > game->selectionrc.top ? rcclient.top : game->selectionrc.top; //max
					game->selectionrc.bottom = std::max(game->dragstartpoint.y, pt.y); game->selectionrc.bottom = rcclient.bottom < game->selectionrc.bottom ? rcclient.bottom : game->selectionrc.bottom; //min
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				}
			}

			if (game->isdragging && game->dragcount >= game->maxdragcount) {
				game->selected_boxs.clear();
				for (int i = 0; i < game->board.size(); i++) {
					for (int j = 0; j < game->board[i].size(); j++) {
						RECT rcbox = game->getBoxRECT(i, j);
						if (game->board[i][j]) {//only get boxes with letters
							if (rcbox.left < game->selectionrc.right && rcbox.right > game->selectionrc.left
								&& rcbox.top < game->selectionrc.bottom && rcbox.bottom > game->selectionrc.top) {//boxes that overlap with selection rc								
								game->selected_boxs.push_back({j, i});
							}
						}
					}
				}
			}

			if (!game->_is_cursor_in_client) {
				game->_is_cursor_in_client = true;
				TRACKMOUSEEVENT tme = {0};
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				TrackMouseEvent(&tme);
			}
			
			break;
		}
		case WM_MOUSELEAVE: {
			game->_is_cursor_in_client = false;
			break;
		}
		case WM_KEYDOWN: {
			SetFocus(hwnd);
			switch (wParam) {
				case VK_SPACE: {
					game->newGame();
					break;
				}
				case 'Q': {
					game->Solve();
					break;
				}
			}
			break;
		}
		case WM_SETCURSOR: {
			//SetCursor(LoadCursor(game->hinst, MAKEINTRESOURCE(IDC_CURSOR1)));
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
		}
		case WM_SIZE: {
			game->resize();

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			if (game) {
				delete game;
				game = nullptr;
			}
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Game::DrawScreen(Game* game) {
	HWND hwnd = game->GetHWND();

	GetClientRect(hwnd, &game->rcclient);
	//std::cout << game->rcclient.left << " " << game->rcclient.top << " " << game->rcclient.right << " " << game->rcclient.bottom << '\n';

	if (!game->rcclient.right || !game->rcclient.bottom) return;

	HDC hdc = GetDC(hwnd);

	HDC hdcmem = CreateCompatibleDC(hdc);

	HBITMAP hbmScreen, hbmOldBitmap;
	hbmScreen = CreateCompatibleBitmap(hdc, game->rcclient.right, game->rcclient.bottom);
	hbmOldBitmap = (HBITMAP)SelectObject(hdcmem, hbmScreen);

	//////////////////////////////////////////////////////////

	//create font
	int font_height = game->getBoxRECT(0, 0).bottom - game->getBoxRECT(0, 0).top;
	HFONT font = CreateFont(font_height, font_height / 2, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, L"Consolas");
	auto oldfont = SelectObject(hdcmem, font);

	//draw board
	SetBkMode(hdcmem, TRANSPARENT);
	SetTextColor(hdcmem, RGB(0, 127, 255));
	HBRUSH boxbrush = CreateSolidBrush(RGB(30, 30, 30));
	HBRUSH boxhighlightbrush = CreateSolidBrush(RGB(200, 200, 200));
	HBRUSH correctletterbrush = CreateSolidBrush(RGB(0, 127, 255));
	HBRUSH incorrectletterbrush = CreateSolidBrush(RGB(255, 27, 0));
	for (int i = 0; i < game->board.size(); i++) {
		for (int j = 0; j < game->board[i].size(); j++) {
			RECT rcbox = game->getBoxRECT(i, j);
			//draw box rect outline			
			POINT loc = {j, i};
			//draw selected box (highlighted)
			if (std::find(game->selected_boxs.begin(), game->selected_boxs.end(), loc) != game->selected_boxs.end()) {
				FrameRect(hdcmem, &rcbox, boxhighlightbrush);
			}
			else if (game->isDieAtPOINTIncorrect({j, i})) {
				FrameRect(hdcmem, &rcbox, incorrectletterbrush);
			}
			else if (game->isDieAtPOINTCorrect({j, i})) {
				FrameRect(hdcmem, &rcbox, correctletterbrush);
			}
			else {
				FrameRect(hdcmem, &rcbox, boxbrush);
			}
			
			if (game->board[i][j] && game->board[i][j]->getFace()) {
				//draw letter text centered in rect
				char l = game->board[i][j]->getFace();
				std::string temp(1, (char)toupper(l));
				std::wstring c = std::wstring(temp.begin(), temp.end());
				SIZE sz;
				GetTextExtentPoint32(hdcmem, c.c_str(), c.size(), &sz);
				int cx = ((rcbox.right - rcbox.left) - sz.cx) / 2 + rcbox.left;
				int cy = ((rcbox.bottom - rcbox.top) - sz.cy) / 2 + rcbox.top;
				ExtTextOut(hdcmem, cx, cy, ETO_CLIPPED, NULL, c.c_str(), c.size(), NULL);
			}
		}
	}

	if (game->isdragging) {
		FrameRect(hdcmem, &game->selectionrc, boxhighlightbrush);
	}
	
	DeleteObject(incorrectletterbrush);
	DeleteObject(correctletterbrush);
	DeleteObject(boxhighlightbrush);
	DeleteObject(boxbrush);

	DeleteObject(SelectObject(hdcmem, oldfont));

	//////////////////////////////////////////////////////////

	//actually draw the screen
	BitBlt(hdc, 0, 0, game->rcclient.right, game->rcclient.bottom, hdcmem, 0, 0, SRCCOPY);

	//cleanup
	SelectObject(hdcmem, hbmOldBitmap);
	DeleteObject(hbmScreen);

	DeleteDC(hdcmem);
	ReleaseDC(hwnd, hdc);
}

/*

"MMLLBY",
"VFGKPP",
"HHNNRR",
"DFRLLW",
"RRDLGG",
"XKBSZN",
"WHHTTP",
"CCBTJD",
"CCMTTS",
"OIINNY",
"AEIOUU",
"AAEEOO"

*/

void Game::LoadWords() {
	std::ifstream file;
	file.open("qlesswords.txt");
	if (file.is_open()) {
		while (file.good()) {
			std::string line;
			std::getline(file, line);			
			this->words.insert(line);
		}
	}
	file.close();
		
/* keep this commented out stuff
	std::unordered_set<char> possible_letters;
	for (auto a : g_dice) for (auto b : a) possible_letters.insert(b);

	std::ifstream file;
	file.open("words.txt");
	if (file.is_open()) {
		while (file.good()) {
			std::string line;
			std::getline(file, line);
			if (line.size() > 2 && line.size() < 13) {
				bool validword = true;
				for (int i = 0; i < line.size(); i++) {
					if (!possible_letters.count(line[i])) {
						validword = false;
						break;
					}
				}
				if (validword)
					this->words.insert(line);
			}
		}
	}
	file.close();

	std::ofstream file2;
	file2.open("qlesswords.txt");
	if (file2.is_open()) {
		for (auto a : this->words) {
			file2 << a << '\n';
		}
	}
	file2.close();*/
}

bool operator==(const POINT& lhs, const POINT& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool isSafe(const std::vector<std::vector<Die*>>& board, int row, int col, Die* die, std::vector<std::vector<bool>>& visited) {
	return (row >= 0 && row < board.size())
		&& (col >= 0 && col < board[0].size())
		&& ((board[row][col] && die) && !visited[row][col]);
}

void DFS(const std::vector<std::vector<Die*>>& board, int row, int col, Die* die, std::vector<std::vector<bool>>& visited) {
	static int rowNbr[] = {-1, 1, 0, 0};
	static int colNbr[] = {0, 0, 1, -1};

	visited[row][col] = true;

	for (int k = 0; k < 4; ++k)
		if (isSafe(board, row + rowNbr[k], col + colNbr[k], die, visited))
			DFS(board, row + rowNbr[k],	col + colNbr[k], die, visited);
}

int connectedComponents(const std::vector<std::vector<Die*>>& board) {
	int connectedComp = 0;

	std::vector<std::vector<bool>> visited = {board.size(), std::vector<bool>(board[0].size(), false)};

	for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board[i].size(); j++) {
			if (!visited[i][j]) {
				Die* die = board[i][j];
				if (!die) continue;
				DFS(board, i, j, die, visited);
				connectedComp++;
			}
		}
	}

	return connectedComp;
}

void checkWordsHelper(const std::vector<std::vector<Die*>>& board, int i, int j, std::string* word, std::vector<POINT>* letter_pts, std::vector<Word>* found_words, bool horz) {
	if (board[i][j]) {
		*word += board[i][j]->getFace();
		letter_pts->push_back({j, i});
		if (((!horz && i >= board.size() - 1) || (horz && j >= board.size() - 1))
			|| !board[i + (!horz ? 1 : 0)][j + (horz ? 1 : 0)]) {
			if (word->size() >= 2) {
				Word fw(*word, *letter_pts);
				found_words->push_back(fw);
			}
			word->clear();
			letter_pts->clear();
		}
	}
}

bool Game::checkWords() {
	this->clearWords();

	//search for possible words in board
	std::vector<Word> found_words;
	for (int i = 0; i < this->board.size(); i++) {
		std::string word1 = "";
		std::string word2 = "";
		std::vector<POINT> word1_letter_pts;
		std::vector<POINT> word2_letter_pts;
		for (int j = 0; j < this->board[i].size(); j++) {
			checkWordsHelper(this->board, i, j, &word1, &word1_letter_pts, &found_words, true);
			checkWordsHelper(this->board, j, i, &word2, &word2_letter_pts, &found_words, false);
		}
	}

	for (int i = 0; i < found_words.size(); i++) {
		Word w(found_words[i]);
		auto val = this->words.find(found_words[i].word);
		if (found_words[i].word.length() > 2 && this->words.count(found_words[i].word)) {			
			this->correct_words.push_back(w);
		}
		else {
			this->incorrect_words.push_back(w);
		}
	}

	int n = connectedComponents(this->board);

	if (found_words.size() == this->correct_words.size()          //all the found words are correct
		&& this->getCorrectLetterCount() == this->dice.size()     //all the letters are used
		&& connectedComponents(this->board) == 1)                 //all the letters are connected
		return true;

	return false;
}

void Game::rollDice() {
	for (auto a : this->dice) 
		a->roll();
}

bool Game::checkBoardBounds(POINT pt) {
	if (pt.y < 0 || pt.y >= this->board.size())
		return false;
	else if (pt.x < 0 || pt.x >= this->board.size())
		return false;
	return true;
}

void Game::clearWords() {
	this->correct_words.clear();
	this->incorrect_words.clear();
}

int Game::getCorrectLetterCount() {
	std::unordered_set<int> count;
	for (int i = 0; i < this->correct_words.size(); i++) {
		for (int j = 0; j < this->correct_words[i].letter_pts.size(); j++) {
			POINT& pt = this->correct_words[i].letter_pts[j];
			count.insert(pt.x * this->dice.size() + pt.y);
		}
	}
	return count.size();
}

bool Game::isDieAtPOINTIncorrect(POINT pt) {
	for (int i = 0; i < incorrect_words.size(); i++) {
		for (int j = 0; j < incorrect_words[i].letter_pts.size(); j++) {
			if (incorrect_words[i].letter_pts[j].x == pt.x && incorrect_words[i].letter_pts[j].y == pt.y)
				return true;
		}
	}
	return false;
}

bool Game::isDieAtPOINTCorrect(POINT pt) {
	for (int i = 0; i < correct_words.size(); i++) {
		for (int j = 0; j < correct_words[i].letter_pts.size(); j++) {
			if (correct_words[i].letter_pts[j].x == pt.x && correct_words[i].letter_pts[j].y == pt.y)
				return true;
		}
	}
	return false;
}

void Game::resetDicePositions() {
	//clear board completely
	for (int i = 0, dpos = 0; i < this->board.size(); i++) 
		for (int j = 0; j < this->board[i].size(); j++) 
			this->board[i][j] = nullptr;
		
	//put dice back on bottom
	for (int i = 0; i < this->dice.size(); i++) {
		this->board[this->board.size() - 1][i] = this->dice[i];
	}
}

bool Game::MoveSelectedDice(POINT to, bool checkwords) {
	if (!this->selected_boxs.size() || !checkBoardBounds(to))
		return false;	

	std::swap(this->board[to.y][to.x], this->board[this->selected_boxs[0].y][this->selected_boxs[0].x]);

	bool win = checkwords ? this->checkWords() : false;

	if (win) {		
		this->selected_boxs.clear();
		this->DrawScreen(this);
		MessageBox(GetParent(this->hwnd), L"you win!", L"you win", MB_OK);
	}

	this->DrawScreen(this);

	return win;
}

Die* Game::GetDieAtPOINT(POINT pt) {
	if (!checkBoardBounds(pt))
		return nullptr;

	return this->board[pt.y][pt.x];
}

RECT Game::getBoxRECT(int y, int x) {
	double boxh = (double)(this->rcclient.bottom - this->rcclient.top) / (double)this->board.size();
	double boxw = (double)(this->rcclient.right - this->rcclient.left) / (double)this->board.size();
	RECT ret;
	ret.left = x * boxw;
	ret.right = (x + 1) * boxw;
	ret.top = y * boxh;
	ret.bottom = (y + 1) * boxh;
	return ret;
}

void Game::Solve() {
//	auto solve = [&]() {
	std::vector<std::string> valid_words;

	std::string letters = "";
	for (int i = 0; i < this->dice.size(); i++) {
		letters += this->dice[i]->getFace();
	}

	//this code is supposed to find longest possible word, but doesn't care if the roll is actually possible
	/*for (auto a : this->words) {
		if (a.size() >= 11) {
			std::string t = a;
			for (auto l : letters) {
				auto p = t.find(l);
				if (p != std::string::npos) {
					t.erase(p, 1);
	//this code is supposed to find longest possible word, but doesn't care if the roll is actually possible
					if (!t.size())
						break;
				}
			}
			if (t.size() == 0) {
				std::cout << a << '\n';
			}
		}
	}
	return;*/

	//remove letters that are being used by correct words
	auto hash = [](const POINT& p) { return ((uint64_t)p.x) << 32 | (uint64_t)p.y; };
	auto equal = [](const POINT& p1, const POINT& p2) { return p1.x == p2.x && p1.y == p2.y; };
	std::unordered_set<POINT, decltype(hash), decltype(equal)> used_letter_pos(0, hash, equal);
	for (auto cw : this->correct_words) {
		for (auto pt : cw.letter_pts) {
			used_letter_pos.insert(pt);
		}
	}
	for (auto ulpt : used_letter_pos) {
		letters.erase(letters.find(this->board[ulpt.y][ulpt.x]->getFace()), 1);
	}		

	for (auto w : this->words) {
		//if there are correct words the remaining letters must use one letter from one of those words
		bool cont = true;									//				
		if (this->correct_words.size()) {					//
			cont = false;									//
			for (auto cw : this->correct_words) {			//
				for (auto c : w) {							//
					if (cw.word.find(c)) {					//
						cont = true;						//
						break;								//
					}										//
				}											//
				if (cont) break;							//
			}												//
		}													//
		if (!cont) continue;								//

		//remove rolled letters from copy of word (if there are no characters left that means we can make the word from the rolled letters)
		//one letter left can still check against any correct words (we removed 'correct word' letters from the list)
		std::string t = w;
		for (auto l : letters) {
			auto p = t.find(l);
			if (p != std::string::npos) {
				t.erase(p, 1);

				if (!t.size())
					break;
			}
		}
		if (this->correct_words.size() == 0 && t.size() == 0) {//no letters left means the word was made up of rolled letters
			valid_words.push_back(w);
		}
		else if (t.size() == 1) {//need to test if remaining letter is in a correct word (word can be crossed with existing word)
			for (auto cw : this->correct_words) {
				for (int l = 0; l < cw.word.size(); l++) {
					if (cw.word[l] == t[0]) {//does word have the remaining letter in it?
						//make sure that character in correct word isn't already being used (crossed words)
						int c = 0;
						for (auto cwp : this->correct_words) {//need to check against all the words
							for (auto pos : cwp.letter_pts) {//all the points of each word
								if (pos == cw.letter_pts[l]) {
									c++;//count number of times a letter is used
								}
							}
						}
						if (c == 1) {//letter is only used by one word 
							valid_words.push_back(w);
							break;
						}
					}
				}
			}
		}
	}

	//print words alphabetically but also by size
	std::stable_sort(valid_words.begin(), valid_words.end());
	std::stable_sort(valid_words.begin(), valid_words.end(), [](const std::string& l, const std::string& r) ->bool { return l.size() < r.size(); });
	for (auto w : valid_words) {
		std::cout << w << '\n';
	}
	std::cout << "\n=================================\n\n";


	return;
	//do solve
	for (auto word : valid_words) {
		std::string t = word;
		if (this->correct_words.size() == 0) {//first word			
			POINT pt = {(this->board.size() - word.size()) / 2, this->board.size() / 2 - 1};
			//get letters for valid word from bottom row and move up to middle for first word (horizontal)
			for (int i = 0; i < this->board[this->board.size() - 1].size(); i++) {
				auto p = t.find(this->board[this->board.size() - 1][i]->getFace());
				if (p != std::string::npos) {
					t[p] = '\0';
					this->selected_boxs.clear();
					this->selected_boxs.push_back({i, (LONG)this->board.size() - 1});
					POINT ptto = pt;
					ptto.x += p;
					this->MoveSelectedDice(ptto, false);
					this->selected_boxs.clear();
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
			this->checkWords();
			this->DrawScreen(this);
			this->Solve();
			break;
		}
		else if (this->correct_words.size() == 1) {//second word
			int remaining_letters = this->dice.size() - this->correct_words[0].word.size();
			if (remaining_letters - word.size() + 1 == 1)
				continue;

			std::vector<std::vector<Die*>> test;
			for (int i = 0; i < this->board.size(); i++) {
				std::vector<Die*> t1;
				for (int j = 0; j < this->board[i].size(); j++) {
					t1.push_back(this->board[i][j]);
				}
				test.push_back(t1);
			}

			for (int i = 0; i < this->board.back().size(); i++) {
				if (!this->board.back()[i])
					continue;
				auto p = t.find(this->board.back()[i]->getFace());
				if (p != std::string::npos) 
					t[p] = '\0';
			}
			auto p = t.find_first_not_of('\0');
			if (p == std::string::npos)
				continue;
			//for (int i = 0; i < this->correct_words.size(); i++) {
			for (int j = 0; j < this->correct_words[0].word.size(); j++) {
				if (this->correct_words[0].word[j] != t[p])
					continue;
				//word crosses here
				POINT pt = this->correct_words[0].letter_pts[j];
				//get letters for valid word from bottom row and move up to first word and place vertically
				t = word;
				t[p] = '\0';
				for (int l = 0; l < this->board.back().size(); l++) {
					if (!this->board.back()[l])
						continue;
					auto pos = t.find(this->board.back()[l]->getFace());
					if (pos != std::string::npos) {
						t[pos] = '\0';
						this->selected_boxs.clear();
						this->selected_boxs.push_back({l, (LONG)this->board.size() - 1});
						POINT ptto = pt;
						ptto.y += l - pos;
						this->MoveSelectedDice(ptto, false);
						this->selected_boxs.clear();
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
				}					
			}
			//}			
		}
	}
//	};

	//std::thread a(solve);
	//a.detach();
	//solve();
}
