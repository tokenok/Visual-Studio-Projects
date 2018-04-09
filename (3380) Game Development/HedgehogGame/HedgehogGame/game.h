#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <fstream>

#define DEFAULT_PLAYER_COUNT			6
#define DEFAULT_AI_COUNT				5
#define DEFAULT_HEDGEHOG_COUNT			4
#define DEFAULT_WIN_COUNT				3
#define DEFAULT_COLUMNS					9
#define DEFAULT_ROWS					6

const double g_hedgehog_stack_width = 20;//percentage of the square the hedgehog stack uses
const double animation_speed = 10;

extern HWND g_hwnd;

#define WM_SEND_MOVE (WM_USER + 1)//WPARAM(HIWORD = fromr, LOWORD = fromc) | LPARAM(HIWORD = tor, LOWORD = toc)

enum OBSTACLE_IDS {
	OBS_NONE = 0,
	OBS_PIT,
	OBS_BLACK_HOLE,
	OBS_SHALLOW_PIT,
	OBS_TRAMPOLINE	
};

enum VARIENT_FLAGS {
	VARIENT_NONE = 0,
	VARIENT_TUBE = 1,
	VARIENT_DOPING = 1 << 1,
	VARIENT_ANARCHOHEDGEHOG = 1 << 2
};

enum AI_MOVE_UP_DOWN {
	MOVE_DONT = 0,
	MOVE_UP,
	MOVE_DOWN
};

struct Player {
	int id;
	int piece_count;
	int numFinished;
	DWORD color;//RGB
	bool hasMoved;
};

struct Square {
	RECT rc;
	int obstacle_type;
	std::vector<Player*> stack;
};

class Board {
	public:
	int columns, rows;
	std::vector<std::vector<Square>> squares;

	Board();
	Board(int w, int h, std::vector<std::vector<int>>& board);

	bool SetBoard(std::ifstream& file);
	bool SetBoard(const std::vector<std::vector<int>>& layout);

	bool hittest(POINT p, int* row, int* column);

	RECT getHogRect(int r, int c);

	private:
	bool init(int r, int c, const std::vector<std::vector<int>>& board);
};

class Game {
	public:

	bool is_building_board = false;

	Game(): numPlayers(DEFAULT_PLAYER_COUNT), numAI(DEFAULT_AI_COUNT), numHogs(DEFAULT_HEDGEHOG_COUNT), numWin(DEFAULT_WIN_COUNT) {
		board.columns = DEFAULT_COLUMNS; board.rows = DEFAULT_ROWS;
	}
	~Game();

	int rolldie();

	void promptNewGame(std::string mb_text, std::string caption);

	void startNewGame();

	void gameLoop();

	void wait();
	void signal();

	void checkWin(Player* p, int r, int c);

	bool canPlace(int row);

	bool obstacleLogic(int row, int column);

	bool canMoveUp(int row, int column);
	bool canMoveDown(int row, int column);
	bool canMoveForward(int row, int column);

	bool move(Player* p, int fromr, int fromc, int tor, int toc);

	void obstacleEvent(Player* p, int r, int c);

	void gameOutput(int playerid, std::string output, COLORREF color);
	void gameOutput(std::string output, COLORREF color);

	bool isAImove() { return (turn >= numPlayers - numAI); }
	bool AI_isMoveSafe(int fr, int fc, int tr, int tc);
	int AI_moveUpDown(int& fr, int& fc, int& tr, int& tc);
	int AI_moveForward(int& fr, int& fc);

	static BOOL CALLBACK StaticGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticNewGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticGameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticLegendProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	static VOID CALLBACK TimerProc(HWND hwnd, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/);

	private:

	bool init_game;
	bool gameover = false;
	int numPlayers;	
	int numHogs;
	int numWin;
	int turn;
	int roll;

	int numAI;

	//animation data
	int fromr, fromc, tor, toc;
	bool moving = false;

	DWORD varients;

	int selected_obstacle = 0;

	Board board;
	std::vector<Player*> players;

	bool pause = false;
	bool block_input = false;

	std::mutex outputmutex;
	std::deque<std::pair<std::string, COLORREF>> outbuffer;

	BOOL CALLBACK GameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK NewGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK GameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK LegendProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif 

