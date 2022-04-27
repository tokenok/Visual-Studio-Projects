#pragma once

#define NOMINMAX

#include <Windows.h>

#include <vector>
#include <unordered_set>

#include "Die.h"

#define GAMECLASSNAME L"GameClassName"

ATOM registergamewindowclassname(HINSTANCE hInstance);

struct Word {
public:
	std::string word;
	std::vector<POINT> letter_pts;

	Word() { }

	Word(const Word& w): Word(w.word, w.letter_pts) { }

	Word(const std::string& word, const std::vector<POINT>& letter_pts) {
		this->word = word;
		for (int i = 0; i < letter_pts.size(); i++)
			this->letter_pts.push_back(letter_pts[i]);
	}
};

class Game {
public:
	Game(HINSTANCE hinst);

	~Game();

	static Game* getGame(HWND hwnd);

	HWND GetHWND() { return hwnd; }

	void resize();

	void newGame();

	void resetDicePositions();

	void resetGame();

	bool MoveSelectedDice(POINT to, bool checkwords = true);

	Die * GetDieAtPOINT(POINT pt);

	RECT getBoxRECT(int y, int x);

	void Solve();

private:
	std::vector<std::vector<Die*>> board;
	std::vector<Die*> dice;

	std::vector<POINT> selected_boxs;

	std::unordered_set<std::string> words;

	std::vector<Word> correct_words;
	std::vector<Word> incorrect_words;

	HWND hwnd;
	HINSTANCE hinst;
	RECT rcclient;

	bool isdragging = false;
	int dragcount = 0;
	RECT selectionrc;
	POINT dragstartpoint;
	bool _is_cursor_in_client;
	const int maxdragcount = 15;

	static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void DrawScreen(Game* game);

	void LoadWords();

	bool checkWords();

	void rollDice();
	
	bool checkBoardBounds(POINT pt);

	void clearWords();

	int getCorrectLetterCount();

	bool isDieAtPOINTCorrect(POINT pt);
	bool isDieAtPOINTIncorrect(POINT pt);

	friend ATOM registergamewindowclassname(HINSTANCE hinst);
};

