#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <fstream>

using namespace std;

extern HWND g_hwnd;

enum moves {
	/*0*/MOVE_NONE					= 0,
	/*1*/MOVE_UP					= 1,
	/*2*/MOVE_RIGHT					= 1 << 1,
	/*4*/MOVE_DOWN					= 1 << 2, 
	/*8*/MOVE_LEFT					= 1 << 3,
	/*16*/MOVE_COUNTERCLOCKWISE		= 1 << 4, 
	/*32*/MOVE_CLOCKWISE			= 1 << 5
};

enum types {
	/*0*/TYPE_NONE						= 0,
	/*1*/TYPE_PIECE						= 1,
	/*2*/TYPE_GOAL						= 1 << 1,
	/*4*/TYPE_WALL						= 1 << 2
};

enum modes {
	MODE_MAIN_MENU = 0,
	MODE_SELECT_PACK,
	MODE_PRE_LEVEL,
	MODE_LEVEL,
	MODE_POST_LEVEL, 
	MODE_CREATE_LEVEL
};

enum main_menu_options {
	MMO_STARTGAME,
	MMO_SELECTPACK,
	MMO_CREATELEVEL,
	MMO_EXIT
};
enum post_level_menu_options {
	PLMO_NEXTLEVEL,
	PLMO_GOTOMAINMENU,
	PLMO_REPLAY
};
enum create_level_menu_options {
	BLMO_TYPE_PIECE = TYPE_PIECE,
	BLMO_TYPE_GOAL = TYPE_GOAL,
	BLMO_TYPE_WALL = TYPE_WALL,
	BLMO_WIDTH,
	BLMO_HEIGHT,
	BLMO_SAVE
};

class Board {
	public:
	struct Square {
		int id = 0;
		int type = 0;
		int props = 0;
		RECT rc;

		int can_move = MOVE_NONE;
		bool move_checked = false;

		int hit_test(POINT pt) const;
	};

	int move_count = 0;

	int width, height;
	int numPieces;
	vector<vector<Square>> LevelGrid;

	bool isdestsafe(const Board::Square& sq, int piece_id);

	bool hittest(POINT p, int* row, int* column);
	bool MovePiece(int piece_id, int direction, POINT selected_POINT, bool do_move = true);
	bool rotate_piece(vector<vector<Board::Square>>& ret, int rotation, int piece_id, POINT pivot);
	bool checkWin();

	void save_custom_build();
		
	int selected_piece_id;
	struct move {
		int piece_id;
		int direction;
		POINT pivot;
		struct build {
			int props;
			int id;
			int type;
		};
		build build_props;
	};

	bool stacklock = 0;
	vector<move> undo_stack;
	vector<move> redo_stack;
	void undoLastMove();
	void redoLastMove();
};

class Level {
	public:
	string name;

	bool LoadLevelFromBitmap(string levelname);

	Board* getBoard() { return &board; }
	void setBoard(Board board) { this->board = board; }

	private:
	Board board;
};

struct level_pack {
	string name;
	vector<Level> levels;
	RECT rc;
};
struct menu_option {
	int option;
	string text;
	RECT rc;
	COLORREF text_col;
	COLORREF back_col;
};

class Game {
	public:

	Game(): currentLevel(0), currentPack(0), mode(0), hwnd(0) {
	}
	~Game() {}

	void nextLevel();

	Board* getCurrentLevelBoard() {
		return (currentLevel > (int)packs[currentPack].levels.size() - 1) ? NULL : packs[currentPack].levels[currentLevel].getBoard();
	}

	void setMode(int mode) {
		this->mode = mode;
		
		SendMessage(hwnd, WM_SIZE, 0, 0);
	}
	int getMode() { return mode; }

	void reset_level(int packi, int leveli);

	POINT selected_build_square;
	int selected_build_id = 1;
	int selected_build_type = 1;

	static VOID CALLBACK TimerProc(HWND hwnd, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/);

	static BOOL CALLBACK StaticGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticGameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StaticGameOutputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	
	vector<level_pack> packs;
	int currentLevel;
	int currentPack;


	private:

	HWND hwnd;

	int mode;

	vector<menu_option> main_menu;
	vector<menu_option> post_level_menu;
	vector<menu_option> build_menu;

	mutex paintmutex;
	mutex outputme;

	BOOL CALLBACK GameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK GameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK GameOutputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif 

