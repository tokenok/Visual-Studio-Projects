#include "game.h"

#include <windowsx.h>
#define _USE_MATH_DEFINES//PI
#include <math.h>
#include <map>

//#include "image.h"
#include "C:/CPPlibs/common/common.h"
#include "resource.h"

using namespace std;

HWND g_hwnd;
Game* g_game;

HHOOK kbhook;

double starttime;
double endtime;

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam);

bool PtInTri(POINT tri[3], POINT pt) {
	auto area = [](int x1, int y1, int x2, int y2, int x3, int y3) {
		return abs((x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2)) / 2.0);
	};
	double a = area(tri[0].x, tri[0].y, tri[1].x, tri[1].y, tri[2].x, tri[2].y);
	double a1 = area(pt.x, pt.y, tri[1].x, tri[1].y, tri[2].x, tri[2].y);
	double a2 = area(tri[0].x, tri[0].y, pt.x, pt.y, tri[2].x, tri[2].y);
	double a3 = area(tri[0].x, tri[0].y, tri[1].x, tri[1].y, pt.x, pt.y);

	return (a == a1 + a2 + a3);
}
int Board::Square::hit_test(POINT pt) const {
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	int ycenter = h / 2 + rc.top;
	int xcenter = w / 2 + rc.left;

	POINT UPTRI[3] = {
		{rc.left, rc.top},
		{xcenter, ycenter},
		{rc.right, rc.top}
	};
	if (PtInTri(UPTRI, pt) && props & MOVE_UP)
		return MOVE_UP;

	POINT RIGHTTRI[3] = {
		{rc.right, rc.top},
		{xcenter, ycenter},
		{rc.right, rc.bottom}
	};
	if (PtInTri(RIGHTTRI, pt) && props & MOVE_RIGHT)
		return MOVE_RIGHT;

	POINT LEFTTRI[3] = {
		{rc.left, rc.top},
		{xcenter, ycenter},
		{rc.left, rc.bottom}
	};
	if (PtInTri(LEFTTRI, pt) && props & MOVE_LEFT)
		return MOVE_LEFT;

	POINT DOWNTRI[3] = {
		{rc.left, rc.bottom},
		{xcenter, ycenter},
		{rc.right, rc.bottom}
	};
	if (PtInTri(DOWNTRI, pt) && props & MOVE_DOWN)
		return MOVE_DOWN;

	if (PtInRect(&rc, pt))
		return props;

	return -1;
}

void printBoard(vector<vector<Board::Square>>& board) {
	for (UINT i = 0; i < board.size(); i++) {
		for (UINT j = 0; j < board[i].size(); j++) {
			cout << board[i][j].type;
		}
		cout << '\n';
	}
	cout << '\n';
}
void printBoard(vector<Board::Square> board, int w, int h) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			cout << board[i * w + j].type;
		}
		cout << '\n';
	}
	cout << '\n';
}

void Board::undoLastMove() {
	if (undo_stack.size()) {
		Board::move move = undo_stack[undo_stack.size() - 1];
		stacklock = true;
		switch (move.direction) {
			case MOVE_UP:{
				MovePiece(move.piece_id, MOVE_DOWN, move.pivot);
				move_count--;
				break;
			}
			case MOVE_DOWN:{
				MovePiece(move.piece_id, MOVE_UP, move.pivot);
				move_count--;
				break;
			}
			case MOVE_RIGHT:{
				MovePiece(move.piece_id, MOVE_LEFT, move.pivot);
				move_count--;
				break;
			}
			case MOVE_LEFT:{
				MovePiece(move.piece_id, MOVE_RIGHT, move.pivot);
				move_count--;
				break;
			}
			case MOVE_COUNTERCLOCKWISE:{
				MovePiece(move.piece_id, MOVE_CLOCKWISE, move.pivot);
				move_count--;
				break;
			}
			case MOVE_CLOCKWISE:{
				MovePiece(move.piece_id, MOVE_COUNTERCLOCKWISE, move.pivot);
				move_count--;
				break;
			}
			case -1:{
				Board::move mv;
				mv = move;
				mv.build_props.id = LevelGrid[move.pivot.x][move.pivot.y].id;
				mv.build_props.props = LevelGrid[move.pivot.x][move.pivot.y].props;
				mv.build_props.type = LevelGrid[move.pivot.x][move.pivot.y].type;

				LevelGrid[move.pivot.x][move.pivot.y].props = move.build_props.props;
				LevelGrid[move.pivot.x][move.pivot.y].id = move.build_props.id;
				LevelGrid[move.pivot.x][move.pivot.y].type = move.build_props.type;

				move = mv;

				break;
			}
		}
		stacklock = false;
		redo_stack.push_back(move);
		undo_stack.pop_back();
	}
}
void Board::redoLastMove() {
	if (redo_stack.size()) {
		Board::move move = redo_stack[redo_stack.size() - 1];

		if (move.direction == -1) {
			Board::move mv;
			mv = move;
			mv.build_props.id = LevelGrid[move.pivot.x][move.pivot.y].id;
			mv.build_props.props = LevelGrid[move.pivot.x][move.pivot.y].props;
			mv.build_props.type = LevelGrid[move.pivot.x][move.pivot.y].type;

			LevelGrid[move.pivot.x][move.pivot.y].props = move.build_props.props;
			LevelGrid[move.pivot.x][move.pivot.y].id = move.build_props.id;
			LevelGrid[move.pivot.x][move.pivot.y].type = move.build_props.type;		

			move = mv;
		}
		else {
			stacklock = true;
			MovePiece(move.piece_id, move.direction, move.pivot);
			move_count++;
			stacklock = false;
		}

		redo_stack.pop_back();
		undo_stack.push_back(move);
	}
}

bool Board::isdestsafe(const Board::Square& sq, int piece_id) {
	if (sq.type & TYPE_PIECE && sq.id != piece_id)
		return false;
	if (sq.type & TYPE_WALL)
		return false;
	return true;
};

template<class RandomIterator>
void transpose(RandomIterator first, RandomIterator last, int m) {
	const int mn1 = (last - first - 1);
	const int n = (last - first) / m;
	vector<bool> visited(last - first);
	RandomIterator cycle = first;
	while (++cycle != last) {
		if (visited[cycle - first])
			continue;
		int a = cycle - first;
		do {
			a = a == mn1 ? mn1 : (n * a) % mn1;
			swap(*(first + a), *cycle);
			visited[a] = true;
		} while ((first + a) != cycle);
	}
}
bool Board::rotate_piece(vector<vector<Board::Square>>& ret, int rotation, int piece_id, POINT pivot) {
	vector<Board::Square> cpy;
	int w = LevelGrid[0].size();
	int h = LevelGrid.size();

	int oldtype;

	for (UINT i = 0; i < LevelGrid.size(); i++) {
		for (UINT j = 0; j < LevelGrid[i].size(); j++) {
			if (LevelGrid[i][j].id == piece_id && LevelGrid[i][j].type & TYPE_PIECE) {
				cpy.push_back(LevelGrid[i][j]);
				if (i == pivot.y && j == pivot.x) {
					oldtype = cpy[i * w + j].type;
					cpy[i * w + j].type = -1;
				}
			}
			else {
				Board::Square sq;
				sq.id = 0;
				cpy.push_back(sq);
			}
		}
	}

	transpose(cpy.begin(), cpy.end(), w);
	swap(w, h);

	if (rotation == MOVE_CLOCKWISE) {//reverse rows
		for (int i = 0; i < h; i++)
			reverse(cpy.begin() + i * w, cpy.begin() + (i + 1) * w);
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				int newprop = MOVE_NONE;
				if (cpy[i * w + j].props & MOVE_DOWN) {
					cpy[i * w + j].props ^= MOVE_DOWN; 
					newprop |= MOVE_LEFT;
				}
				if (cpy[i * w + j].props & MOVE_LEFT) {
					cpy[i * w + j].props ^= MOVE_LEFT;
					newprop |= MOVE_UP;
				}
				if (cpy[i * w + j].props & MOVE_UP) {
					cpy[i * w + j].props ^= MOVE_UP;
					newprop |= MOVE_RIGHT;
				}
				if (cpy[i * w + j].props & MOVE_RIGHT) {
					cpy[i * w + j].props ^= MOVE_RIGHT;
					newprop |= MOVE_DOWN;
				}
				
				if (newprop)
					cpy[i * w + j].props = newprop;
			}
		}
	}
	else if (rotation == MOVE_COUNTERCLOCKWISE) {//revers columns
		for (int i = 0; i < h / 2; i++) {
			for (int j = 0; j < w; j++) {
				swap(cpy[i * w + j], cpy[(h - i - 1) * w + j]);
			}
		}
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {				
				int newprop = MOVE_NONE;
				if (cpy[i * w + j].props & MOVE_DOWN) {
					cpy[i * w + j].props ^= MOVE_DOWN;
					newprop |= MOVE_RIGHT;
				}
				if (cpy[i * w + j].props & MOVE_RIGHT) {
					cpy[i * w + j].props ^= MOVE_RIGHT;
					newprop |= MOVE_UP;
				}
				if (cpy[i * w + j].props & MOVE_UP) {
					cpy[i * w + j].props ^= MOVE_UP;
					newprop |= MOVE_LEFT;
				}
				if (cpy[i * w + j].props & MOVE_LEFT) {
					cpy[i * w + j].props ^= MOVE_LEFT;
					newprop |= MOVE_DOWN;
				}
				if (newprop)
					cpy[i * w + j].props = newprop;
			}			
		}
	}
	else
		return false;

	//find pivot
	POINT p = {-1, -1};
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (cpy[i * w + j].type == -1) {
				p = {j, i};
				cpy[i * w + j].type = oldtype;
				break;
			}
		}
	}

	//shift piece back to original pivot
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (cpy[i * w + j].id) {
				int ni = i + pivot.y - p.y;
				int nj = j + pivot.x - p.x;

				if (ni < 0 || ni >= w) //out of range
					return false;
				if (nj < 0 || nj >= h) //out of range
					return false;

				if (!isdestsafe(LevelGrid[ni][nj], piece_id))//wall collision
					return false;

				ret[ni][nj] = cpy[i * w + j];
			}
		}
	}

	return true;
}

bool Board::hittest(POINT p, int* row, int* column) {
	for (UINT r = 0; r < LevelGrid.size(); r++) {
		for (UINT c = 0; c < LevelGrid[r].size(); c++) {
			if (PtInRect(&LevelGrid[r][c].rc, p)) {
				*row = r;
				*column = c;
				return true;
			}
		}
	}
	return false;
}
bool Board::MovePiece(int piece_id, int direction, POINT selected_POINT, bool do_move) {
	vector<vector<Square>> NewLevelGrid_Empty;
	NewLevelGrid_Empty.resize(LevelGrid.size());
	for (UINT i = 0; i < LevelGrid.size(); i++) 
		NewLevelGrid_Empty[i].resize(LevelGrid[i].size());

#pragma region get no_piece mask

	vector<vector<Square>> LG_no_piece_id;
	LG_no_piece_id.resize(LevelGrid.size());
	for (UINT i = 0; i < LevelGrid.size(); i++) 
		LG_no_piece_id[i].resize(LevelGrid[i].size());
	for (UINT i = 0; i < LevelGrid.size(); i++) {
		for (UINT j = 0; j < LevelGrid[i].size(); j++) {			
			LG_no_piece_id[i][j] = LevelGrid[i][j];
			if (LevelGrid[i][j].type & TYPE_PIECE)
				LG_no_piece_id[i][j].type ^= TYPE_PIECE;
		}
	}

#pragma endregion

	if (direction == MOVE_CLOCKWISE || direction == MOVE_COUNTERCLOCKWISE) {		
		if (!rotate_piece(NewLevelGrid_Empty, direction, piece_id, selected_POINT)) {
			LevelGrid[selected_POINT.y][selected_POINT.x].can_move = false;
			return false;
		}

		for (UINT i = 0; i < LevelGrid.size(); i++) {
			for (UINT j = 0; j < LevelGrid[i].size(); j++) {
				//skip old piece_id
				if (LevelGrid[i][j].id == piece_id && LevelGrid[i][j].type & TYPE_PIECE) 					
					continue;
				//skip new piece_id
				if (NewLevelGrid_Empty[i][j].id == piece_id && NewLevelGrid_Empty[i][j].type & TYPE_PIECE) {
					Square lg = LevelGrid[i][j];
					Square nlg = NewLevelGrid_Empty[i][j];
					NewLevelGrid_Empty[i][j].type |= LevelGrid[i][j].type;
					nlg = NewLevelGrid_Empty[i][j];
					continue;
				}
				//set other pieces
				NewLevelGrid_Empty[i][j] = LevelGrid[i][j];
			}
		}
	}
	else {
		for (UINT i = 0; i < LevelGrid.size(); i++) {
			for (UINT j = 0; j < LevelGrid[i].size(); j++) {
				if (LevelGrid[i][j].id == piece_id && LevelGrid[i][j].type & TYPE_PIECE) {
					switch (direction) {
						case MOVE_UP:{
							if (i == 0 || !isdestsafe(LevelGrid[i - 1][j], piece_id)) {
								if (LevelGrid[selected_POINT.y][selected_POINT.x].can_move & MOVE_UP)
									LevelGrid[selected_POINT.y][selected_POINT.x].can_move ^= MOVE_UP;
								return false;
							}
							NewLevelGrid_Empty[i - 1][j] = LevelGrid[i][j];
							
							break;
						}
						case MOVE_DOWN:{
							if (i == LevelGrid.size() - 1 || !isdestsafe(LevelGrid[i + 1][j], piece_id)) {
								if (LevelGrid[selected_POINT.y][selected_POINT.x].can_move & MOVE_DOWN)
									LevelGrid[selected_POINT.y][selected_POINT.x].can_move ^= MOVE_DOWN;
								return false;
							}
							NewLevelGrid_Empty[i + 1][j] = LevelGrid[i][j];

							break;
						}
						case MOVE_RIGHT:{
							if (j == LevelGrid[i].size() - 1 || !isdestsafe(LevelGrid[i][j + 1], piece_id)) {
								if (LevelGrid[selected_POINT.y][selected_POINT.x].can_move & MOVE_RIGHT)
									LevelGrid[selected_POINT.y][selected_POINT.x].can_move ^= MOVE_RIGHT;
								return false;
							}
							NewLevelGrid_Empty[i][j + 1] = LevelGrid[i][j];

							break;
						}
						case MOVE_LEFT:{
							if (j == 0 || !isdestsafe(LevelGrid[i][j - 1], piece_id)) {
								if (LevelGrid[selected_POINT.y][selected_POINT.x].can_move & MOVE_LEFT)
									LevelGrid[selected_POINT.y][selected_POINT.x].can_move ^= MOVE_LEFT;
								return false;
							}
							NewLevelGrid_Empty[i][j - 1] = LevelGrid[i][j];

							break;
						}
						default:{
							NewLevelGrid_Empty[i][j] = LevelGrid[i][j];
						}
					}
				}
				else if (!NewLevelGrid_Empty[i][j].id) {
					NewLevelGrid_Empty[i][j] = LevelGrid[i][j];
				}
			}
		}
	}

	//remove none piece mask, then reapply
	for (UINT i = 0; i < NewLevelGrid_Empty.size(); i++) {
		for (UINT j = 0; j < NewLevelGrid_Empty[i].size(); j++) {
			if (NewLevelGrid_Empty[i][j].type & TYPE_PIECE)
				NewLevelGrid_Empty[i][j].type = TYPE_PIECE;
		}
	}
	for (UINT i = 0; i < NewLevelGrid_Empty.size(); i++) {
		for (UINT j = 0; j < NewLevelGrid_Empty[i].size(); j++) {
			NewLevelGrid_Empty[i][j].type |= LG_no_piece_id[i][j].type;
		}
	}

	if (do_move) {
		if (!stacklock) {
			Board::move move;
			move.piece_id = piece_id;
			move.direction = direction;
			move.pivot = selected_POINT;
			undo_stack.push_back(move);
			move_count++;
		}

		selected_piece_id = piece_id;
		LevelGrid = NewLevelGrid_Empty;
		SendMessage(g_hwnd, WM_SIZE, NULL, NULL);

		for (UINT i = 0; i < LevelGrid.size(); i++) {
			for (UINT j = 0; j < LevelGrid[i].size(); j++) {
				if (LevelGrid[i][j].type & TYPE_PIECE) {
					LevelGrid[i][j].move_checked = false;
				}
			}
		}		
		
		checkWin();
	}
	else
		LevelGrid[selected_POINT.y][selected_POINT.x].can_move |= direction;

	return true;
}
bool Board::checkWin() {
	if (g_game->getMode() != MODE_LEVEL)
		return false;

	for (UINT i = 0; i < LevelGrid.size(); i++) {
		for (UINT j = 0; j < LevelGrid[i].size(); j++) {
			if ((LevelGrid[i][j].type & TYPE_GOAL) && LevelGrid[i][j].type != (TYPE_GOAL | TYPE_PIECE)) {
				return false;
			}
		}
	}

	endtime = clock();
	g_game->setMode(MODE_POST_LEVEL);
	
	return true;
}

void Board::save_custom_build() {
	if (g_game->getMode() != MODE_CREATE_LEVEL) return;

	BYTE* newbuf = new BYTE[height * width * 3];

	for (UINT i = 0; i < (UINT)height; i++) {
		for (UINT j = 0; j < (UINT)width * 3; j += 3) {
			if (LevelGrid[i][j / 3].type) {
				newbuf[i * 3 * width + j + 0] = LevelGrid[i][j / 3].id;
				newbuf[i * 3 * width + j + 1] = LevelGrid[i][j / 3].type;
				newbuf[i * 3 * width + j + 2] = LevelGrid[i][j / 3].props;
			}
			else {			
				newbuf[i * 3 * width + j + 0] = 255;
				newbuf[i * 3 * width + j + 1] = 255;
				newbuf[i * 3 * width + j + 2] = 255;
			}
		}
	}
	
	string path;
	if (!g_game->packs[g_game->currentPack].name.size() || !g_game->packs[g_game->currentPack].levels[g_game->currentLevel].name.size()) {
		path = browseforfile(g_hwnd, false, L"Save Custom Level", L"bmp\0*.bmp\0");
		if (path.size()) {
			vector<string> directories = split_str(path, "\\");
			if (directories.size() >= 2) {
				g_game->packs[g_game->currentPack].levels[g_game->currentLevel].name = directories[directories.size() - 1];
				g_game->packs[g_game->currentPack].name = directories[directories.size() - 2];
			}
		}
	}
	else
		path = getexedir() + "\\Levels\\" + g_game->packs[g_game->currentPack].name + "\\" + g_game->packs[g_game->currentPack].levels[g_game->currentLevel].name + ".bmp";
	
	if (path.size())
		array_to_bmp(path, newbuf, width, height);
}

bool Level::LoadLevelFromBitmap(string levelname) {
	BYTE* newbuf = bmp_to_array(levelname, board.width, board.height);
	if (!newbuf)
		return false;

	board.LevelGrid.resize(board.height);
	for (UINT i = 0; i < board.LevelGrid.size(); i++) {
		board.LevelGrid[i].resize(board.width);
	}

	map<int, int> piececount;

	for (int i = 0; i < board.height; i++) {
		for (int j = 0; j < 3 * board.width; j += 3) {
			int pos = i * 3 * board.width + j;

			int r = newbuf[pos];
			int g = newbuf[pos + 1];
			int b = newbuf[pos + 2];

			board.LevelGrid[i][j / 3].id = r > 0 && r != 255 ? r : 0;		
			board.LevelGrid[i][j / 3].type = g > 0 && g != 255 ? g : 0;
			board.LevelGrid[i][j / 3].props = b > 0 && b != 255 ? b : 0;

			if (board.LevelGrid[i][j / 3].id && board.LevelGrid[i][j / 3].type & TYPE_PIECE) {
				piececount[board.LevelGrid[i][j / 3].id] = 1;
			}
		}
	}
	delete[] newbuf;

	board.numPieces = piececount.size();

	return true;
}

void Game::nextLevel() {
	reset_level(currentPack, currentLevel);

	currentLevel++;
	
	if (currentLevel > (int)packs[currentPack].levels.size() - 1) {
	//	int ret = MessageBox(NULL, L"You win!\nWould you like to return to the Main Menu?", L"Game Over", MB_ICONASTERISK | MB_YESNO | MB_SYSTEMMODAL);
		//if (ret == IDYES) {
			currentLevel = 0;
			setMode(MODE_MAIN_MENU);

			return;
	//	}
		//else
			//exit(0);
	}

	reset_level(currentPack, currentLevel);

	starttime = clock();

	SendMessage(g_hwnd, WM_SIZE, NULL, NULL);
}
void Game::reset_level(int packi, int leveli) {
	packs[packi].levels[leveli].getBoard()->undo_stack.clear();
	packs[packi].levels[leveli].getBoard()->move_count = 0;
	for (UINT i = 0; i < packs[packi].levels[leveli].getBoard()->LevelGrid.size(); i++) {
		for (UINT j = 0; j < packs[packi].levels[leveli].getBoard()->LevelGrid[i].size(); j++) {
			packs[packi].levels[leveli].getBoard()->LevelGrid[i][j].move_checked = false;
		}
	}
	packs[packi].levels[leveli].LoadLevelFromBitmap(
		getexedir() + "\\Levels\\"
		+ packs[packi].name + "\\"
		+ packs[packi].levels[leveli].name + ".bmp");
}

BOOL CALLBACK Game::StaticGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->GameProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::StaticGameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->GameDisplayProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::StaticGameOutputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->GameOutputProc(hwnd, message, wParam, lParam);
}

VOID CALLBACK Game::TimerProc(HWND hwnd, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/) {
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

BOOL CALLBACK Game::GameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			g_hwnd = hwnd;

			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

			Game* this_game = reinterpret_cast<Game*>(lParam);

			g_game = this_game;

			vector<string> packs;
			GetFoldersInDirectory(getexedir() + "\\Levels", &packs);
			if (!packs.size()) {
				MessageBox(NULL, L"Unable to load levels", L"Error", MB_OK | MB_ICONWARNING);
				EndDialog(hwnd, 1);
			}
			
			if (packs.size() > 1) {
				packs.erase(find(packs.begin(), packs.end(), getexedir() + "\\Levels\\(NULL)"));
			}

			for (UINT p = 0; p < packs.size(); p++) {
				vector<string> files;
				GetFilesInDirectory(packs[p], "bmp", &files);

				if (!files.size()) {
					MessageBox(NULL, L"Unable to load levels", L"Error", MB_OK | MB_ICONWARNING);
					EndDialog(hwnd, 1);
				}
				
				level_pack lp;
				lp.name = packs[p].substr(packs[p].rfind("\\") + 1, packs[p].size());

				for (UINT i = 0; i < files.size(); i++) {
					Level lvl;
					lvl.name = files[i].substr(files[i].rfind("\\") + 1, files[i].rfind(".") - files[i].rfind("\\") - 1);
					lvl.LoadLevelFromBitmap(files[i]);
					lp.levels.push_back(lvl);
				}
				this_game->packs.push_back(lp);
			}

			this_game->main_menu = {
				{MMO_STARTGAME, "Start Game", {0}, RGB(255, 0, 0), RGB(10, 10, 10)},
				{MMO_SELECTPACK, "Select Level Pack", {0}, RGB(0, 128, 255), RGB(10, 10, 10)},
				{MMO_CREATELEVEL, "Create Level", {0}, RGB(0, 255, 0), RGB(10,10,10)},
				{MMO_EXIT, "Exit", {0}, RGB(255, 255, 0), RGB(10, 10, 10)}
			};

			this_game->post_level_menu = {
				{PLMO_NEXTLEVEL, "Play Next Level", {0}, RGB(0, 222, 0), RGB(10, 10, 10)},
				{PLMO_REPLAY, "Replay Level", {0}, RGB(222, 222, 0), RGB(10, 10, 10)},
				{PLMO_GOTOMAINMENU, "Return to Main Menu", {0}, RGB(255, 0, 0), RGB(10, 10, 10)}
			};

			this_game->build_menu = {
				{BLMO_WIDTH, "Width: 12", {0}, RGB(255, 0, 0), RGB(10, 10, 10)},
				{BLMO_HEIGHT, "Height: 8", {0}, RGB(255, 0, 0), RGB(10, 10, 10)},
				{BLMO_TYPE_PIECE, "Piece ID: 1", {0}, RGB(222, 222, 0), RGB(10, 10, 10)},
				{BLMO_TYPE_GOAL, "Goal", {0}, RGB(0, 222, 0), RGB(10, 10, 10)},
				{BLMO_TYPE_WALL, "Wall", {0}, RGB(222, 222, 222), RGB(10, 10, 10)},
				{BLMO_SAVE, "Save", {0}, RGB(0, 128, 255), RGB(10, 10, 10)}
			};

			this_game->hwnd = GetDlgItem(hwnd, IDC_GAMEDISPLAY);

			//subclass game window 
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_GAMEDISPLAY), GWLP_USERDATA, (LONG_PTR)this_game);
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_GAMEDISPLAY), GWLP_WNDPROC, (LONG_PTR)Game::StaticGameDisplayProc);

			//subclass output window
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_GAMEOUTPUT), GWLP_USERDATA, (LONG_PTR)this_game);
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_GAMEOUTPUT), GWLP_WNDPROC, (LONG_PTR)Game::StaticGameOutputProc);

			SetMenu(hwnd, LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1)));

			//redraw game window on a timer
			SetTimer(GetDlgItem(hwnd, IDC_GAMEDISPLAY), 0, 10, Game::TimerProc);
			SetTimer(GetDlgItem(hwnd, IDC_GAMEOUTPUT), 0, 10, Game::TimerProc);

			kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case ID_GAME_GOTOMAINMENU:{
					setMode(MODE_MAIN_MENU);
					packs[currentPack].levels[currentLevel].getBoard()->undo_stack.clear();
					reset_level(currentPack, currentLevel);
					currentLevel = 0;
					reset_level(currentPack, currentLevel);

					break;
				}
			}
			break;
		}
		case WM_SIZE:{
			SendMessage(GetDlgItem(hwnd, IDC_GAMEDISPLAY), WM_SIZE, wParam, lParam);//send size message to gamewindow and let that window handle resizing
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			KillTimer(GetDlgItem(hwnd, IDC_GAMEDISPLAY), 0);

			EndDialog(hwnd, 0);
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK Game::GameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_PAINT:{
			lock_guard<mutex> lock(paintmutex);

			RECT rcclient = getclientrect(hwnd);
			POINT pt = getclientcursorpos(hwnd);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			/////////////////////////////////////////////////////////////////////

			switch (mode) {
				case MODE_MAIN_MENU:{					
					int h = (rcclient.bottom - rcclient.top);
					int topoffset = h / main_menu.size();
					h -= topoffset;

					//color background
					HBRUSH background = CreateSolidBrush(RGB(10, 10, 10));
					FillRect(hDCmem, &rcclient, background);
					DeleteObject(background);

					//draw splash screen (temp)
					RECT splashrc = {rcclient.left, rcclient.top, rcclient.right, topoffset};
					string t = "[splash goes here]";
					HFONT font = CreateFont(splashrc.bottom - splashrc.top - 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"System");
					HFONT hFontOld = (HFONT)SelectObject(hDCmem, font);
					DrawText(hDCmem, STW(t), t.size(), &splashrc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					DeleteObject(SelectObject(hDCmem, hFontOld));

					for (UINT i = 0; i < main_menu.size(); i++) {
						SetTextColor(hDCmem, main_menu[i].text_col);
						SetBkColor(hDCmem, main_menu[i].back_col);

						HFONT font = CreateFont(main_menu[i].rc.bottom - main_menu[i].rc.top - 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"System");
						HFONT hFontOld = (HFONT)SelectObject(hDCmem, font);
						DrawText(hDCmem, STW(main_menu[i].text), main_menu[i].text.size(), &main_menu[i].rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						DeleteObject(SelectObject(hDCmem, hFontOld));

						if (PtInRect(&main_menu[i].rc, pt)) {
							HBRUSH frame = CreateSolidBrush(main_menu[i].text_col);
							FrameRect(hDCmem, &main_menu[i].rc, frame);
							DeleteObject(frame);
						}
					}		

					break;
				}
				case MODE_SELECT_PACK:{		//color background
					HBRUSH background = CreateSolidBrush(RGB(10, 10, 10));
					FillRect(hDCmem, &rcclient, background);
					DeleteObject(background);

					for (UINT i = 0; i < packs.size(); i++){
						SetTextColor(hDCmem, RGB(0, 128, 255));
						SetBkColor(hDCmem, RGB(10, 10, 10));

						HFONT font = CreateFont(packs[i].rc.bottom - packs[i].rc.top - 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"System");
						HFONT hFontOld = (HFONT)SelectObject(hDCmem, font);
						DrawText(hDCmem, STW(packs[i].name), packs[i].name.size(), &packs[i].rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						DeleteObject(SelectObject(hDCmem, hFontOld));

						if (i == currentPack) {
							HBRUSH frame = CreateSolidBrush(RGB(0, 128, 255));
							FrameRect(hDCmem, &packs[i].rc, frame);
							DeleteObject(frame);
						}
					}

					break;
				}
				case MODE_PRE_LEVEL:{

					break;
				}
				case MODE_CREATE_LEVEL:
				case MODE_POST_LEVEL:
				case MODE_LEVEL:{
					Board* board = getCurrentLevelBoard();
					if (board) {
						//draw board
						for (UINT r = 0; r < board->LevelGrid.size(); r++) {
							for (UINT c = 0; c < board->LevelGrid[r].size(); c++) {
								const Board::Square sq = *(&board->LevelGrid[r][c]);
								RECT rc = sq.rc;

								COLORREF backc = 0;
								//draw background/////////////////////////////////////////////////////////////////////////////////////						
								switch (sq.type) {
									case TYPE_NONE:{

										break;
									}
									case TYPE_PIECE | TYPE_GOAL:
									case TYPE_PIECE: {
#pragma region generate a unique color based on pices id and color them
										double center = 128;
										double width = 127;
										double phase = 128;
										double frequency = M_PI * 2 / board->numPieces;
										int red = (int)(sin(frequency * (board->numPieces - sq.id) + 0 + phase) * width + center);
										int green = (int)(sin(frequency * (board->numPieces - sq.id) + 2 + phase) * width + center);
										int blue = (int)(sin(frequency * (board->numPieces - sq.id) + 4 + phase) * width + center);

										backc = RGB(red, green, blue);

										HBRUSH bbrush = CreateSolidBrush(backc);
										FillRect(hDCmem, &rc, bbrush);
										DeleteObject(bbrush);

#pragma endregion

										//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region w, h, and dists

										int w = rc.right - rc.left;
										int h = rc.bottom - rc.top;
										int aw = w / 8;

										int ytop = h / 4 + rc.top;
										int ybottom = 3 * h / 4 + rc.top;
										int ycenter = h / 2 + rc.top;

										int xleft = w / 4 + rc.left;
										int xright = 3 * w / 4 + rc.left;
										int xcenter = w / 2 + rc.left;

										int oydist = (rc.bottom - rc.top) / 10;
										int oxdist = (rc.right - rc.left) / 10;

#pragma endregion

										if (sq.props & MOVE_CLOCKWISE) {
											COLORREF pcol = RGB(255, 255, 255);
											if (mode == MODE_POST_LEVEL || (sq.hit_test(pt) == MOVE_CLOCKWISE))
												pcol = 0;

											if (!sq.move_checked) {
												board->LevelGrid[r][c].move_checked = true;
												if (!board->MovePiece(sq.id, MOVE_CLOCKWISE, {c, r}, false)) {
													pcol = RGB(50, 50, 50);
												}
											}
											else {
												if (!(sq.can_move & MOVE_CLOCKWISE)) {
													pcol = RGB(50, 50, 50);
												}
											}

											HPEN uborder = CreatePen(PS_SOLID, 3, pcol);
											HBRUSH uback = CreateSolidBrush(backc);

											HPEN oldpen = (HPEN)SelectObject(hDCmem, uborder);
											HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, uback);

											Ellipse(hDCmem, oxdist + rc.left, oydist + rc.top, rc.right - oxdist, rc.bottom - oydist);

											//draw hiding rect
											HPEN hide = CreatePen(PS_SOLID, 2, backc);
											uborder = (HPEN)SelectObject(hDCmem, hide);
											Rectangle(hDCmem, rc.left + 1, ycenter, xcenter, ycenter - 3 * (h / 8));
											DeleteObject(SelectObject(hDCmem, uborder));

											//draw up triangle
											POINT tri[4] = {
												{rc.left + oxdist + 2, ycenter - aw},
												{rc.left + 2, ycenter},
												{rc.left + 2 * aw, ycenter},
												{rc.left + oxdist + 2, ycenter - aw}
											};
											Polygon(hDCmem, tri, 4);

											//cleanup
											DeleteObject(SelectObject(hDCmem, oldbrush));
											DeleteObject(SelectObject(hDCmem, oldpen));
										}
										if (sq.props & MOVE_COUNTERCLOCKWISE) {
											COLORREF pcol = RGB(255, 255, 255);
											if (mode == MODE_POST_LEVEL || (sq.hit_test(pt) == MOVE_COUNTERCLOCKWISE))
												pcol = 0;

											if (!sq.move_checked) {
												board->LevelGrid[r][c].move_checked = true;
												if (!board->MovePiece(sq.id, MOVE_COUNTERCLOCKWISE, {c, r}, false)) {
													pcol = RGB(50, 50, 50);
												}
											}
											else {
												if (!(sq.can_move & MOVE_COUNTERCLOCKWISE)) {
													pcol = RGB(50, 50, 50);
												}
											}

											HPEN uborder = CreatePen(PS_SOLID, 3, pcol);
											HBRUSH uback = CreateSolidBrush(backc);

											HPEN oldpen = (HPEN)SelectObject(hDCmem, uborder);
											HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, uback);

											Ellipse(hDCmem, oxdist + rc.left, oydist + rc.top, rc.right - oxdist, rc.bottom - oydist);

											//draw hiding rect
											HPEN hide = CreatePen(PS_SOLID, 2, backc);
											uborder = (HPEN)SelectObject(hDCmem, hide);
											Rectangle(hDCmem, rc.left + 1, ycenter, xcenter, ycenter + 3 * (h / 8));
											DeleteObject(SelectObject(hDCmem, uborder));

											//draw down triangle
											POINT tri[4] = {
												{rc.left + oxdist + 2, ycenter + aw},
												{rc.left + 2, ycenter},
												{rc.left + 2 * aw - 2, ycenter},
												{rc.left + oxdist + 2, ycenter + aw}
											};
											Polygon(hDCmem, tri, 4);

											//cleanup
											DeleteObject(SelectObject(hDCmem, oldbrush));
											DeleteObject(SelectObject(hDCmem, oldpen));
										}
										if (sq.props & MOVE_UP) {
											COLORREF bcol = RGB(255, 255, 255);;
											if (mode == MODE_POST_LEVEL || sq.hit_test(pt) == MOVE_UP)
												bcol = 0;

											COLORREF pcol = 0;
											if (!sq.move_checked) {
												board->LevelGrid[r][c].move_checked = true;
												if (!board->MovePiece(sq.id, MOVE_UP, {c, r}, false)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}
											else {
												if (!(sq.can_move & MOVE_UP)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}

											HPEN uborder = CreatePen(PS_SOLID, 2, pcol);
											HBRUSH uback = CreateSolidBrush(bcol);

											HPEN oldpen = (HPEN)SelectObject(hDCmem, uborder);
											HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, uback);

											POINT tri[4] = {
												{xcenter, rc.top + oydist},
												{xcenter + aw, rc.top + (3 * oydist)},
												{xcenter - aw, rc.top + (3 * oydist)},
												{xcenter, rc.top + oydist}
											};
											Polygon(hDCmem, tri, 4);

											DeleteObject(SelectObject(hDCmem, oldbrush));
											DeleteObject(SelectObject(hDCmem, oldpen));
										}
										if (sq.props & MOVE_LEFT) {
											COLORREF bcol = RGB(255, 255, 255);;
											if (mode == MODE_POST_LEVEL || sq.hit_test(pt) == MOVE_LEFT)
												bcol = 0;

											COLORREF pcol = 0;
											if (!sq.move_checked){
												board->LevelGrid[r][c].move_checked = true;
												if (!board->MovePiece(sq.id, MOVE_LEFT, {c, r}, false)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}
											else {
												if (!(sq.can_move & MOVE_LEFT)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}

											HPEN uborder = CreatePen(PS_SOLID, 2, pcol);
											HBRUSH uback = CreateSolidBrush(bcol);

											HPEN oldpen = (HPEN)SelectObject(hDCmem, uborder);
											HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, uback);

											POINT tri[4] = {
												{rc.left + oxdist, ycenter},
												{rc.left + (3 * oxdist), ycenter + aw},
												{rc.left + (3 * oxdist), ycenter - aw},
												{rc.left + oxdist, ycenter}
											};
											Polygon(hDCmem, tri, 4);

											DeleteObject(SelectObject(hDCmem, oldbrush));
											DeleteObject(SelectObject(hDCmem, oldpen));
										}
										if (sq.props & MOVE_DOWN) {
											COLORREF bcol = RGB(255, 255, 255);
											if (mode == MODE_POST_LEVEL || sq.hit_test(pt) == MOVE_DOWN)
												bcol = 0;

											COLORREF pcol = 0;
											if (!sq.move_checked) {
												board->LevelGrid[r][c].move_checked = true;
												if (!board->MovePiece(sq.id, MOVE_DOWN, {c, r}, false)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}
											else {
												if (!(sq.can_move & MOVE_DOWN)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}

											HPEN uborder = CreatePen(PS_SOLID, 2, pcol);
											HBRUSH uback = CreateSolidBrush(bcol);

											HPEN oldpen = (HPEN)SelectObject(hDCmem, uborder);
											HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, uback);

											POINT tri[4] = {
												{xcenter, rc.bottom - oydist},
												{xcenter - aw, rc.bottom - (3 * oydist)},
												{xcenter + aw, rc.bottom - (3 * oydist)},
												{xcenter, rc.bottom - oydist}
											};
											Polygon(hDCmem, tri, 4);

											DeleteObject(SelectObject(hDCmem, oldbrush));
											DeleteObject(SelectObject(hDCmem, oldpen));
										}
										if (sq.props & MOVE_RIGHT) {
											COLORREF bcol = RGB(255, 255, 255);
											if (mode == MODE_POST_LEVEL || sq.hit_test(pt) == MOVE_RIGHT)
												bcol = 0;

											COLORREF pcol = 0;
											if (!sq.move_checked) {
												board->LevelGrid[r][c].move_checked = true;
												if (!board->MovePiece(sq.id, MOVE_RIGHT, {c, r}, false)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}
											else {
												if (!(sq.can_move & MOVE_RIGHT)) {
													bcol = RGB(50, 50, 50);
													pcol = bcol;
												}
											}

											HPEN uborder = CreatePen(PS_SOLID, 2, pcol);
											HBRUSH uback = CreateSolidBrush(bcol);

											HPEN oldpen = (HPEN)SelectObject(hDCmem, uborder);
											HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, uback);

											POINT tri[4] = {
												{rc.right - oxdist, ycenter},
												{rc.right - (3 * oxdist), ycenter + aw},
												{rc.right - (3 * oxdist), ycenter - aw},
												{rc.right - oxdist, ycenter}
											};
											Polygon(hDCmem, tri, 4);

											DeleteObject(SelectObject(hDCmem, oldbrush));
											DeleteObject(SelectObject(hDCmem, oldpen));
										}

										break;
									}
									case TYPE_WALL:{
										backc = RGB(255, 0, 0);
										break;
									}
								}

								if (!backc) {
									backc = RGB(100, 100, 100);
									HBRUSH bbrush = CreateSolidBrush(backc);
									FillRect(hDCmem, &rc, bbrush);
									DeleteObject(bbrush);
								}

								//draw id of piece on square
								if (sq.type == TYPE_PIECE) {
									string text = int_to_str(sq.id);
									POINT midp = {(rc.right - rc.left) / 2 + rc.left, (rc.bottom - rc.top) / 2 + rc.top};
									int numw = ((rc.right - rc.left) / 5);
									int numh = ((rc.bottom - rc.top) / 5);
									RECT numrc = {midp.x - numw, midp.y - numh, midp.x + numw, midp.y + numh};
									SetBkColor(hDCmem, backc);
									SetTextColor(hDCmem, RGB(255, 255, 255));
									DrawText(hDCmem, STW(text), text.size(), &numrc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
								}

								//draw board square frames and highlight when hovered////////////////////////////////////////////////////////////////////////////////////
								COLORREF frame = 0;
								if (mode != MODE_CREATE_LEVEL && board->LevelGrid[r][c].type & TYPE_PIECE && board->LevelGrid[r][c].id == board->selected_piece_id)
									frame = RGB(255, 255, 255);
								else if (mode == MODE_CREATE_LEVEL && selected_build_square.y == r && selected_build_square.x == c)
									frame = RGB(255, 255, 255);
								if (board->LevelGrid[r][c].type & TYPE_GOAL) {
									frame = RGB(0, 255, 0);
								}

								HBRUSH tbrush = CreateSolidBrush(frame);
								FrameRect(hDCmem, &board->LevelGrid[r][c].rc, tbrush);
								RECT inner = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
								FrameRect(hDCmem, &inner, tbrush);
								DeleteObject(tbrush);
							}
						}
					}					

					if (MODE_POST_LEVEL == mode) {
						int sw = (rcclient.right - rcclient.left);
						int sh = (rcclient.bottom - rcclient.top);

						int bw = sw / 2;
						int bh = sh / 2;

						RECT rc = {rcclient.left + bw / 2, rcclient.top + bh / 2, rcclient.right - bw / 2, rcclient.bottom - bh / 2};

						HBRUSH backc = CreateSolidBrush(RGB(10, 10, 10));
						FillRect(hDCmem, &rc, backc);
						DeleteObject(backc);

						int h = (rc.bottom - rc.top);
						int topoffset = h / post_level_menu.size();
						h -= topoffset;

						for (UINT i = 0; i < post_level_menu.size(); i++) {
							SetTextColor(hDCmem, post_level_menu[i].text_col);
							SetBkColor(hDCmem, post_level_menu[i].back_col);

							HFONT font = CreateFont(post_level_menu[i].rc.bottom - post_level_menu[i].rc.top - 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"System");
							HFONT hFontOld = (HFONT)SelectObject(hDCmem, font);
							DrawText(hDCmem, STW(post_level_menu[i].text), post_level_menu[i].text.size(), &post_level_menu[i].rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							DeleteObject(SelectObject(hDCmem, hFontOld));

							if (PtInRect(&post_level_menu[i].rc, pt)) {
								HBRUSH frame = CreateSolidBrush(post_level_menu[i].text_col);
								FrameRect(hDCmem, &post_level_menu[i].rc, frame);
								DeleteObject(frame);
							}
						}

						double score = (((0.001 / ((double)(endtime - starttime) / 1000.0)) * 1234) * (1.0 / (double)board->move_count)) * 10000.0;
						string count = "Score: " + int_to_str((int)score);
						SetTextColor(hDCmem, RGB(0, 128, 255));
						SetBkColor(hDCmem, RGB(10, 10, 10));

						HFONT font = CreateFont(post_level_menu[0].rc.bottom - post_level_menu[0].rc.top - 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"System");
						HFONT hFontOld = (HFONT)SelectObject(hDCmem, font);
						RECT rc1 = {rc.left, rc.top, rc.right, rc.top + (bh / post_level_menu.size())};
						DrawText(hDCmem, STW(count), count.size(), &rc1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						DeleteObject(SelectObject(hDCmem, hFontOld));

						HBRUSH frame = CreateSolidBrush(RGB(255, 255, 255));
						FrameRect(hDCmem, &rc, frame);
						rc = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
						FrameRect(hDCmem, &rc, frame);
						DeleteObject(frame);
					}
					break;
				}						
			}			

			/////////////////////////////////////////////////////////////////////
			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:{
			SetFocus(hwnd);

			POINT pt = getclientcursorpos(hwnd);

			Board* board = getCurrentLevelBoard();

			switch (mode) {
				case MODE_MAIN_MENU:{
					for (UINT i = 0; i < main_menu.size(); i++) {
						if (PtInRect(&main_menu[i].rc, pt)) {
							switch (main_menu[i].option) {
								case MMO_STARTGAME:{
									setMode(MODE_LEVEL);
									starttime = clock();

									break;
								}
								case MMO_SELECTPACK:{
									setMode(MODE_SELECT_PACK);

									break;
								}
								case MMO_CREATELEVEL:{
									level_pack lp;
									Level lv;
									Board b;

									b.width = 12;
									b.height = 8;
									b.move_count = 0;
									b.selected_piece_id = 0;
									b.numPieces = 10;
									b.LevelGrid.resize(b.height);
									for (UINT i = 0; i < b.LevelGrid.size(); i++)
										b.LevelGrid[i].resize(b.width);

									lv.setBoard(b);
									lp.levels.push_back(lv);
									packs.push_back(lp);	

									currentPack = packs.size() - 1;
									currentLevel = 0;

									packs[currentPack].name = "(temp)";

									setMode(MODE_CREATE_LEVEL);

									break;
								}
								case MMO_EXIT:{
									EndDialog(g_hwnd, 0);
									break;
								}
							}

							break;
						}
					}

					break;
				}
				case MODE_SELECT_PACK:{
					for (UINT i = 0; i < packs.size(); i++) {
						if (PtInRect(&packs[i].rc, pt)) {
							currentPack = i;
							setMode(MODE_MAIN_MENU);

							break;
						}
					}

					break;
				}
				case MODE_POST_LEVEL:{
					for (UINT i = 0; i < post_level_menu.size(); i++) {
						if (PtInRect(&post_level_menu[i].rc, pt)) {
							switch (post_level_menu[i].option) {
								case PLMO_GOTOMAINMENU:{
									setMode(MODE_MAIN_MENU);
									packs[currentPack].levels[currentLevel].getBoard()->undo_stack.clear();
									reset_level(currentPack, currentLevel);
									currentLevel = 0;
									reset_level(currentPack, currentLevel);
									
									break;
								}
								case PLMO_NEXTLEVEL:{
									setMode(MODE_LEVEL);
									starttime = clock();

									nextLevel();
									
									break;
								}
								case PLMO_REPLAY:{
									setMode(MODE_LEVEL);
									reset_level(currentPack, currentLevel);
									starttime = clock();

									break;
								}
							}
							

							break;
						}
					}

					break;
				}
				case MODE_CREATE_LEVEL:{
					bool fallthrough = false;

					bool control = (GetAsyncKeyState(VK_CONTROL) < 0);

					int hoverr, hoverc;
					if (board->hittest(pt, &hoverr, &hoverc)) { 
						if (selected_build_square.x == hoverc && selected_build_square.y == hoverr)
							fallthrough = true;//clicked twice so actually move piece
						else {
							//piece was placed
							Board::move undoinfo;
							undoinfo.direction = -1;
							undoinfo.pivot = {hoverr, hoverc};
							undoinfo.build_props.id = board->LevelGrid[hoverr][hoverc].id;
							undoinfo.build_props.props = board->LevelGrid[hoverr][hoverc].props;
							undoinfo.build_props.type = board->LevelGrid[hoverr][hoverc].type;
							board->undo_stack.push_back(undoinfo);
							board->redo_stack.clear();
						}

						Board::Square* sq = &board->LevelGrid[hoverr][hoverc];
						
						selected_build_square = {hoverc, hoverr};

						if (control) {
							selected_build_id = sq->id;
							selected_build_type = sq->type;
							for (UINT i = 0; i < build_menu.size(); i++) {
								if (build_menu[i].option == BLMO_TYPE_PIECE)
									build_menu[i].text = "Piece ID: " + int_to_str(sq->id);
							}							
						}
						
						sq->id = sq->id ? sq->id : selected_build_type == TYPE_PIECE ? selected_build_id : 0;
						sq->type = sq->type ? sq->type : selected_build_type;

						for (UINT i = 0; i < board->LevelGrid.size(); i++)
							for (UINT j = 0; j < board->LevelGrid[i].size(); j++)
								board->LevelGrid[i][j].move_checked = false;
					}

					if (!fallthrough)
						break;
				}
				case MODE_LEVEL:{
					int hoverr, hoverc;
					if (board->hittest(pt, &hoverr, &hoverc)) {
						Board::Square sq = board->LevelGrid[hoverr][hoverc];

						if (sq.type & TYPE_PIECE) {
							int move = sq.hit_test(pt);
							if (sq.props & move) {	
								if (board->MovePiece(sq.id, move, {hoverc, hoverr}) && mode == MODE_CREATE_LEVEL) {
									if (move == MOVE_UP)
										selected_build_square.y--;
									else if (move == MOVE_DOWN)
										selected_build_square.y++;
									else if (move == MOVE_LEFT)
										selected_build_square.x--;
									else if (move == MOVE_RIGHT)
										selected_build_square.x++;

									board->redo_stack.clear();
								}
							}
						}
					}

					break;
				}
			}			

			break;
		}
		case WM_RBUTTONDOWN:{
			SetFocus(hwnd);

			Board* board = getCurrentLevelBoard();

			POINT pt = getclientcursorpos(hwnd);

			int hoverr, hoverc;
			if (board->hittest(pt, &hoverr, &hoverc)) {
				Board::Square sq = board->LevelGrid[hoverr][hoverc];
				
				if (mode == MODE_CREATE_LEVEL) {
					selected_build_square = {hoverc, hoverr};

					Board::move undoinfo;
					undoinfo.direction = -1;
					undoinfo.pivot = {hoverr, hoverc};
					undoinfo.build_props.id = board->LevelGrid[hoverr][hoverc].id;
					undoinfo.build_props.props = board->LevelGrid[hoverr][hoverc].props;
					undoinfo.build_props.type = board->LevelGrid[hoverr][hoverc].type;
					board->undo_stack.push_back(undoinfo);
					board->redo_stack.clear();

					board->LevelGrid[hoverr][hoverc].id = 0;
					board->LevelGrid[hoverr][hoverc].props = 0;
					board->LevelGrid[hoverr][hoverc].type = 0;
					
					for (UINT i = 0; i < board->LevelGrid.size(); i++)
						for (UINT j = 0; j < board->LevelGrid[i].size(); j++)
							board->LevelGrid[i][j].move_checked = false;
				}
				
				if (sq.type & TYPE_PIECE)
					board->selected_piece_id = sq.id;
			}

			break;
		}
		case WM_SIZE:{
			RECT prc = getclientrect(GetParent(hwnd));
			RECT wdestrc;
			RECT odestrc;
			int pw = prc.right - prc.left;
			int ph = prc.bottom - prc.top;

			switch (mode) {
				case MODE_POST_LEVEL:
				case MODE_LEVEL:{
					wdestrc = {prc.left, prc.top, prc.right, prc.bottom - 60};
					odestrc = {prc.left, prc.bottom - 60, prc.right, prc.bottom};
					break;
				}
				case MODE_CREATE_LEVEL:{
					wdestrc = {((prc.right - prc.left) + prc.left) / 5, prc.top, prc.right, prc.bottom};
					odestrc = {prc.left, prc.top, ((prc.right - prc.left) + prc.left) / 5, prc.bottom};
					break;
				}
				default:{
					wdestrc = prc;
					odestrc = {0, 0, 0, 0};
				}
			}

			SetWindowPos(hwnd, NULL, wdestrc.left, wdestrc.top, wdestrc.right - wdestrc.left, wdestrc.bottom - wdestrc.top, NULL);//change game window size
			SetWindowPos(GetDlgItem(GetParent(hwnd), IDC_GAMEOUTPUT), NULL, odestrc.left, odestrc.top, odestrc.right - odestrc.left, odestrc.bottom - odestrc.top, NULL);//set position for game output
			
			//set rect for all squares on board when window size is changed
			RECT rc = getclientrect(hwnd);
			Board* board = getCurrentLevelBoard();
			if (!board)	break;

			if (mode == MODE_LEVEL || mode == MODE_CREATE_LEVEL || mode == MODE_POST_LEVEL) {
				int h = rc.bottom / board->height;
				int w = rc.right / board->width;
				for (UINT r = 0; r < board->LevelGrid.size(); r++) {
					for (UINT c = 0; c < board->LevelGrid[r].size(); c++) {
						board->LevelGrid[r][c].rc = {c * w, r * h, (c + 1) * w, (r + 1) * h};
					}
				}
			}
			if (mode == MODE_MAIN_MENU) {
				int h = (rc.bottom - rc.top);
				int topoffset = h / main_menu.size();
				h -= topoffset;
				for (UINT i = 0; i < main_menu.size(); i++) {
					main_menu[i].rc.left = rc.left + 10;
					main_menu[i].rc.right = rc.right - 10;
					main_menu[i].rc.top = i * (h / main_menu.size()) + topoffset;
					main_menu[i].rc.bottom = (i + 1) * (h / main_menu.size()) + topoffset;
				}
			}
			if (mode == MODE_SELECT_PACK) {
				for (UINT i = 0; i < packs.size(); i++) {
					packs[i].rc.left = rc.left + 10;
					packs[i].rc.right = rc.right - 10;
					packs[i].rc.top = i * ((rc.bottom - rc.top) / packs.size());
					packs[i].rc.bottom = (i + 1) * ((rc.bottom - rc.top) / packs.size());
				}
			}
			if (mode == MODE_POST_LEVEL) {
				int sw = (rc.right - rc.left);
				int sh = (rc.bottom - rc.top);

				int bw = sw / 2;
				int bh = sh / 2;

				RECT rc1 = {rc.left + bw / 2, rc.top + bh / 2, rc.right - bw / 2, rc.bottom - bh / 2};

				int h = (rc1.bottom - rc1.top);
				int topoffset = h / post_level_menu.size();
				h -= topoffset;
				for (UINT i = 0; i < post_level_menu.size(); i++) {
					post_level_menu[i].rc.left = rc1.left + 10;
					post_level_menu[i].rc.right = rc1.right - 10;
					post_level_menu[i].rc.top = (i + 2) * (h / post_level_menu.size()) + topoffset;
					post_level_menu[i].rc.bottom = (i + 3)* (h / post_level_menu.size()) + topoffset;
				}
			}
			if (mode == MODE_CREATE_LEVEL) {
				RECT rc = getclientrect(GetDlgItem(g_hwnd, IDC_GAMEOUTPUT));
				for (UINT i = 0; i < build_menu.size(); i++) {
					build_menu[i].rc.left = rc.left + 3;
					build_menu[i].rc.right = rc.right - 3;
					build_menu[i].rc.top = i * ((rc.bottom - rc.top) / build_menu.size());
					build_menu[i].rc.bottom = (i + 1) * ((rc.bottom - rc.top) / build_menu.size());
				}			
			}

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::GameOutputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_PAINT:{
			lock_guard<mutex> lock(outputme);

			RECT rcclient = getclientrect(hwnd);
			POINT pt = getclientcursorpos(hwnd);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			/////////////////////////////////////////////////////////////////////

			Board* board = getCurrentLevelBoard();

			HBRUSH background = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDCmem, &rcclient, background);
			DeleteObject(background);

			switch (mode) {
				case MODE_POST_LEVEL:
				case MODE_LEVEL:{
					string time = milliseconds_to_hms((mode == MODE_POST_LEVEL ? endtime : clock()) - starttime);
					string count = "Time: " + time + " Total Moves: " + int_to_str(board->move_count); 
					SetTextColor(hDCmem, RGB(127, 0, 0));
					SetBkColor(hDCmem, RGB(10, 10, 10));

					HFONT font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"System");
					HFONT hFontOld = (HFONT)SelectObject(hDCmem, font);
					DrawText(hDCmem, STW(count), count.size(), &rcclient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					DeleteObject(SelectObject(hDCmem, hFontOld));
					
					break;
				}
				case MODE_CREATE_LEVEL:{
					for (UINT i = 0; i < build_menu.size(); i++) {
						double center = 128;
						double width = 127;
						double phase = 128;
						double frequency = M_PI * 2 / board->numPieces;
						int red = (int)(sin(frequency * (board->numPieces - selected_build_id) + 0 + phase) * width + center);
						int green = (int)(sin(frequency * (board->numPieces - selected_build_id) + 2 + phase) * width + center);
						int blue = (int)(sin(frequency * (board->numPieces - selected_build_id) + 4 + phase) * width + center);
						COLORREF idcol = RGB(red, green, blue);
						COLORREF col = build_menu[i].option == BLMO_TYPE_PIECE ? idcol : build_menu[i].text_col;

						SetTextColor(hDCmem, col);
						SetBkColor(hDCmem, build_menu[i].back_col);

						RECT temprc = {build_menu[i].rc.left - (build_menu[i].rc.right - build_menu[i].rc.left) / 10, build_menu[i].rc.top, build_menu[i].rc.right - (build_menu[i].rc.right - build_menu[i].rc.left) / 10, build_menu[i].rc.bottom};
						RECT textrc = ((build_menu[i].option == BLMO_TYPE_PIECE 
							|| build_menu[i].option == BLMO_HEIGHT
							|| build_menu[i].option == BLMO_WIDTH)
							&& PtInRect(&build_menu[i].rc, pt)) ? temprc : build_menu[i].rc;
						DrawText(hDCmem, STW(build_menu[i].text), build_menu[i].text.size(), &textrc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP | DT_INTERNAL);

						if (PtInRect(&build_menu[i].rc, pt) || build_menu[i].option == selected_build_type) {
							HBRUSH frame = CreateSolidBrush(col);
							FrameRect(hDCmem, &build_menu[i].rc, frame);

							if ((build_menu[i].option == BLMO_TYPE_PIECE
								|| build_menu[i].option == BLMO_HEIGHT
								|| build_menu[i].option == BLMO_WIDTH)
								&& PtInRect(&build_menu[i].rc, pt)) {

								RECT btnarearc = {build_menu[i].rc.right - ((build_menu[i].rc.right - build_menu[i].rc.left) / 5), build_menu[i].rc.top, build_menu[i].rc.right, build_menu[i].rc.bottom};					
								RECT upper = {btnarearc.left, btnarearc.top, btnarearc.right, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top};
								RECT lower = {btnarearc.left, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top, btnarearc.right, btnarearc.bottom};
								FrameRect(hDCmem, &upper, frame);
								FrameRect(hDCmem, &lower, frame);
							}
							DeleteObject(frame);					
						}
					}

					break;
				}
			}

			/////////////////////////////////////////////////////////////////////
			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:{
			switch (mode) {
				case MODE_CREATE_LEVEL:{
					for (UINT i = 0; i < build_menu.size(); i++) {
						POINT pt = getclientcursorpos(hwnd);
						if (PtInRect(&build_menu[i].rc, pt)) {
							switch (build_menu[i].option) {
								case BLMO_HEIGHT:{
									RECT btnarearc = {build_menu[i].rc.right - ((build_menu[i].rc.right - build_menu[i].rc.left) / 5), build_menu[i].rc.top, build_menu[i].rc.right, build_menu[i].rc.bottom};
									RECT upper = {btnarearc.left, btnarearc.top, btnarearc.right, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top};
									RECT lower = {btnarearc.left, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top, btnarearc.right, btnarearc.bottom};

									int val = str_to_int(build_menu[i].text.substr(build_menu[i].text.find(": ") + 1, build_menu[i].text.size()));

									if (PtInRect(&upper, pt))
										val++;
									else if (PtInRect(&lower, pt))
										val--;

									if (val <= 1)
										val = 1;

									Board* b = getCurrentLevelBoard();
									b->LevelGrid.resize(val);
									for (UINT j = 0; j < b->LevelGrid.size(); j++)
										b->LevelGrid[j].resize(b->width);
									b->height = val;

									build_menu[i].text = "Height: " + int_to_str(val);

									SendMessage(GetDlgItem(g_hwnd, IDC_GAMEDISPLAY), WM_SIZE, NULL, NULL);

									break;
								}
								case BLMO_WIDTH:{
									RECT btnarearc = {build_menu[i].rc.right - ((build_menu[i].rc.right - build_menu[i].rc.left) / 5), build_menu[i].rc.top, build_menu[i].rc.right, build_menu[i].rc.bottom};
									RECT upper = {btnarearc.left, btnarearc.top, btnarearc.right, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top};
									RECT lower = {btnarearc.left, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top, btnarearc.right, btnarearc.bottom};

									int val = str_to_int(build_menu[i].text.substr(build_menu[i].text.find(": ") + 1, build_menu[i].text.size()));

									if (PtInRect(&upper, pt))
										val++;
									else if (PtInRect(&lower, pt))
										val--;

									if (val <= 1)
										val = 1;

									Board* b = getCurrentLevelBoard();
									for (UINT j = 0; j < b->LevelGrid.size(); j++)
										b->LevelGrid[j].resize(val);
									b->width = val;

									build_menu[i].text = "Width: " + int_to_str(val);

									SendMessage(GetDlgItem(g_hwnd, IDC_GAMEDISPLAY), WM_SIZE, NULL, NULL);

									break;
								}
								case BLMO_TYPE_PIECE:{
									RECT btnarearc = {build_menu[i].rc.right - ((build_menu[i].rc.right - build_menu[i].rc.left) / 5), build_menu[i].rc.top, build_menu[i].rc.right, build_menu[i].rc.bottom};
									RECT upper = {btnarearc.left, btnarearc.top, btnarearc.right, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top};
									RECT lower = {btnarearc.left, (btnarearc.bottom - btnarearc.top) / 2 + btnarearc.top, btnarearc.right, btnarearc.bottom};

									int val = str_to_int(build_menu[i].text.substr(build_menu[i].text.find(": ") + 1, build_menu[i].text.size()));

									if (PtInRect(&upper, pt))
										val++;
									else if (PtInRect(&lower, pt))
										val--;

									if (val >= g_game->getCurrentLevelBoard()->numPieces) {
										val++;
										g_game->getCurrentLevelBoard()->numPieces = val;
									}

									if (val <= 1)
										val = 1;

									selected_build_id = val;
									selected_build_type = TYPE_PIECE;
									build_menu[i].text = "Piece ID: " + int_to_str(val);

									break;
								}
								case BLMO_TYPE_GOAL:{
									selected_build_type = TYPE_GOAL;
							//		selected_build_id = 0;
									break;
								}
								case BLMO_TYPE_WALL:{
									selected_build_type = TYPE_WALL;
							//		selected_build_id = 0;
									break;
								}
								case BLMO_SAVE:{									
									getCurrentLevelBoard()->save_custom_build();
									break;
								}
							}

							break;
						}
					}

					break;
				}
			}

			break;
		}
		case WM_RBUTTONDOWN:{
			switch (mode) {
				case MODE_CREATE_LEVEL:{
					for (UINT i = 0; i < build_menu.size(); i++) {
						POINT pt = getclientcursorpos(hwnd);
						if (PtInRect(&build_menu[i].rc, pt)) {
							getCurrentLevelBoard()->numPieces++;
						}
					}
					break;
				}
			}

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT kbhs = *((KBDLLHOOKSTRUCT*)lParam);
	DWORD key = kbhs.vkCode;
	if ((GetKeyState(VK_MENU) & 0x8000) != 0) key += 256;
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) key += 256 * 4;
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) key += 256 * 2;
	if ((GetKeyState(VK_LWIN) & 0x8000) != 0) key += 256 * 8;
	if ((GetKeyState(VK_RWIN) & 0x8000) != 0) key += 256 * 8;
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		Board* board = g_game->getCurrentLevelBoard();
		if (GetForegroundWindow() == g_hwnd) {
			if (key == 602) {//ctrl + z
				if (g_game->getMode() == MODE_LEVEL || g_game->getMode() == MODE_CREATE_LEVEL) {
					board->undoLastMove();
				}
			}
			else if (key == 601) {//ctrl + y
				if (g_game->getMode() == MODE_LEVEL || g_game->getMode() == MODE_CREATE_LEVEL) {
					board->redoLastMove();
				}
			}
			else if (key == VK_SPACE) {
				if (g_game->getMode() == MODE_LEVEL) {
					g_game->nextLevel();
					starttime = clock();
				}
			}
			else if (key == 1838) {
				g_game->setMode(MODE_CREATE_LEVEL);
			}
			else {
				int possiblemoves = MOVE_NONE;
				for (UINT i = 0; i < board->LevelGrid.size(); i++) {
					for (UINT j = 0; j < board->LevelGrid[i].size(); j++) {
						if (board->LevelGrid[i][j].id == board->selected_piece_id && board->LevelGrid[i][j].type & TYPE_PIECE) {
							possiblemoves |= board->LevelGrid[i][j].props;
						}
					}
				}
				if (key == VK_UP) {
					if (g_game->getMode() == MODE_LEVEL && possiblemoves & MOVE_UP) {
						if (board->selected_piece_id) {
							if (board->MovePiece(board->selected_piece_id, MOVE_UP, {0})) {

							}
						}
					}
					else if (g_game->getMode() == MODE_CREATE_LEVEL) {
						if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].type == TYPE_PIECE) {
							board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_UP;

							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_COUNTERCLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_COUNTERCLOCKWISE;
							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_CLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_CLOCKWISE;

							for (int i = 0; i < board->height; i++)
								for (int j = 0; j < board->width; j++)
									board->LevelGrid[i][j].move_checked = false;
						}
					}
				}
				else if (key == VK_RIGHT) {
					if (g_game->getMode() == MODE_LEVEL && possiblemoves & MOVE_RIGHT) {
						if (board->selected_piece_id) {
							if (board->MovePiece(board->selected_piece_id, MOVE_RIGHT, {0})) {

							}
						}
					}
					else if (g_game->getMode() == MODE_CREATE_LEVEL) {
						if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].type == TYPE_PIECE) {
							board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_RIGHT;

							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_COUNTERCLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_COUNTERCLOCKWISE;
							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_CLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_CLOCKWISE;

							for (int i = 0; i < board->height; i++)
								for (int j = 0; j < board->width; j++)
									board->LevelGrid[i][j].move_checked = false;
						}
					}
				}
				else if (key == VK_DOWN) {
					if (g_game->getMode() == MODE_LEVEL && possiblemoves & MOVE_DOWN) {
						if (board->selected_piece_id) {
							if (board->MovePiece(board->selected_piece_id, MOVE_DOWN, {0})) {

							}
						}
					}
					else if (g_game->getMode() == MODE_CREATE_LEVEL) {
						if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].type == TYPE_PIECE) {
							board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_DOWN;

							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_COUNTERCLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_COUNTERCLOCKWISE;
							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_CLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_CLOCKWISE;

							for (int i = 0; i < board->height; i++)
								for (int j = 0; j < board->width; j++)
									board->LevelGrid[i][j].move_checked = false;
						}
					}
				}
				else if (key == VK_LEFT) {
					if (g_game->getMode() == MODE_LEVEL && possiblemoves & MOVE_LEFT) {
						if (board->selected_piece_id) {
							if (board->MovePiece(board->selected_piece_id, MOVE_LEFT, {0})) {

							}
						}
					}
					else if (g_game->getMode() == MODE_CREATE_LEVEL) {
						if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].type == TYPE_PIECE) {
							board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_LEFT;

							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_COUNTERCLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_COUNTERCLOCKWISE;
							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_CLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_CLOCKWISE;

							for (int i = 0; i < board->height; i++)
								for (int j = 0; j < board->width; j++)
									board->LevelGrid[i][j].move_checked = false;
						}
					}
				}
				else if (key == VK_LEFT + 1024) {
					if (g_game->getMode() == MODE_CREATE_LEVEL) {
						if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].type == TYPE_PIECE) {
							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_COUNTERCLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props ^= MOVE_COUNTERCLOCKWISE;
							else
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props = MOVE_COUNTERCLOCKWISE;

							for (int i = 0; i < board->height; i++)
								for (int j = 0; j < board->width; j++)
									board->LevelGrid[i][j].move_checked = false;
						}
					}
				}
				else if (key == VK_RIGHT + 1024) {
					if (g_game->getMode() == MODE_CREATE_LEVEL) {
						if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].type == TYPE_PIECE) {
							if (board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props & MOVE_CLOCKWISE)
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props = 0;
							else
								board->LevelGrid[g_game->selected_build_square.y][g_game->selected_build_square.x].props = MOVE_CLOCKWISE;

							for (int i = 0; i < board->height; i++)
								for (int j = 0; j < board->width; j++)
									board->LevelGrid[i][j].move_checked = false;
						}
					}
				}
			}
		}
	}
	return CallNextHookEx(kbhook, code, wParam, lParam);
}

