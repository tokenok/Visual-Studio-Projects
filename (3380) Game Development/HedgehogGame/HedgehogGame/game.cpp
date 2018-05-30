#include "game.h"

#include <windowsx.h>
#include <Richedit.h>
#include <random>
#define _USE_MATH_DEFINES//PI
#include <math.h>
#include <thread>
#include <map>

#include "C:/CPPlibs/common/common.h"
#include "resource.h"

//TODO control animation speed in-game
//TODO move loading and board sizing to builder window
//TODO fix black holes

using namespace std;

HWND g_hwnd;

std::map<int, COLORREF> g_obstacles = {
	{OBS_NONE, RGB(25, 25, 25)},
	{OBS_PIT, RGB(101, 67, 33)},
	{OBS_BLACK_HOLE, RGB(0, 0, 0)},
	{OBS_SHALLOW_PIT, RGB(205, 133, 63)},
	{OBS_TRAMPOLINE, RGB(0, 255, 0)}
};

RECT Board::getHogRect(int r, int c) {
	RECT rc = squares[r][c].rc;
	return {rc.left + 4 + (int)((rc.right - rc.left) * (double)(g_hedgehog_stack_width / 100)), rc.top + 4, rc.right - 4, rc.bottom - 4};
}

////////////////////////////////////////////////////

Board::Board() {
	vector<vector<int>> board {
		{0, 0, 0, OBS_PIT, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, OBS_PIT, 0, 0},
		{0, 0, 0, 0, OBS_PIT, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, OBS_PIT, 0, 0, 0},
		{0, 0, OBS_PIT, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, OBS_PIT, 0}
	};
	init(board.size(), board[0].size(), board);
}

bool Board::init(int r, int c, const vector<vector<int>>& board) {
	rows = r;
	columns = c;

	squares.clear();

	squares.resize(r);
	for (UINT i = 0; i < squares.size(); i++)
		squares[i].resize(c);

	for (UINT i = 0; i < board.size(); i++) {
		for (UINT j = 0; j < board[i].size(); j++) {
			squares[i][j].obstacle_type = board[i][j];
		}
	}
	return true;
}
bool Board::SetBoard(const vector<vector<int>>& layout) {
	if (!layout.size() || !layout[0].size())
		return false;

	return init(layout.size(), layout[0].size(), layout);
}
bool Board::SetBoard(ifstream& file) {
	vector<vector<int>> layout;
	while (file.good()) {
		string line;
		getline(file, line);
		vector<int> row = split_stri(line, " ");
		if (row.size())
			layout.push_back(row);
	}

	return SetBoard(layout);
}

bool Board::hittest(POINT p, int* row, int* column) {
	for (UINT r = 0; r < squares.size(); r++) {
		for (UINT c = 0; c < squares[r].size(); c++) {
			if (PtInRect(&squares[r][c].rc, p)) {
				*row = r;
				*column = c;
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////

Game::~Game() {
	for (UINT i = 0; i < players.size(); i++) {
		delete players[i];
	}
}

int Game::rolldie() {
	random_device rd;
	mt19937 gen(rd());
	return gen() % board.rows + 1;
}

void Game::promptNewGame(string mb_text, string caption) { 
	int response = MessageBox(g_hwnd, STW(mb_text), STW(caption), MB_YESNO | MB_APPLMODAL | MB_DEFBUTTON1);	

	if (response == IDYES) {//user selected yes so start new game
		EnableWindow(g_hwnd, FALSE);//disable game window 
		block_input = true;
		gameover = true;//end game loop
		signal();
		startNewGame();//open new game dialog
		EnableWindow(g_hwnd, TRUE);//reenable game window
		block_input = false;
		SendMessage(g_hwnd, WM_SIZE, NULL, NULL);
		SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void Game::startNewGame() {
	while (pause) this_thread::sleep_for(chrono::milliseconds(10));

	init_game = true;

	SetWindowText(GetDlgItem(g_hwnd, IDC_OUTPUT), L"");
	outbuffer.clear();

	//clean old game object
	for (UINT i = 0; i < players.size(); i++) {
		delete players[i];
	}
	players.clear();
	board.squares.clear();

	//get new game info from dialog
	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_INITGAME), NULL, (DLGPROC)StaticNewGameProc, (LPARAM)this);

	//add players to the game, giving them their own unique id and color
	for (int i = 0; i < numPlayers; i++) {
		Player* p = new Player;
		p->id = i;
		p->hasMoved = false;

		//generate a unique color based on i
		double center = 128;
		double width = 127;
		double phase = 128;
		double frequency = M_PI * 2 / numPlayers;
		int r = (int)(sin(frequency * (numPlayers - i) + 0 + phase) * width + center);
		int g = (int)(sin(frequency * (numPlayers - i) + 2 + phase) * width + center);
		int b = (int)(sin(frequency * (numPlayers - i) + 4 + phase) * width + center);

		p->color = RGB(r, g, b);
		p->numFinished = 0;
		p->piece_count = numHogs;
		players.push_back(p);
	}

	auto a = [&]() {
		init_game = true;

		//have players select their starting rows and place their pieces
		for (int j = 0; j < numHogs; j++) {
			for (int i = (int)players.size() - 1; i >= 0; --i) {
				if (gameover)//prevents crash
					break;

				turn = i;

				gameOutput(i + 1, "choose a row to place your " + int_to_str(j + 1)
					+ (j == 0 ? "st" : j == 1 ? "nd" : j == 2 ? "rd" : "th")
					+ " hedgehog\r\n", players[turn]->color);

				if (isAImove()) {				
					int random;
					while (!canPlace(random = rolldie() - 1));
					board.squares[random][0].stack.push_back(players[turn]);
				}
				else {
					block_input = false;
					wait();//wait for input
				}
			}
			if (gameover)//prevents crash
				break;
		}

		gameOutput("-----------------------------------\r\n", 0);

		init_game = false;

		//start game loop
		thread c(&Game::gameLoop, this);
		c.detach();

	};
	thread b(a);
	b.detach();
}

void Game::gameLoop() {
	while (!gameover) {
		for (UINT i = 0; i < players.size(); i++) {
			if (gameover)
				break;

			while (moving)
				this_thread::sleep_for(chrono::milliseconds(10));//wait for move to finish

			if (gameover)
				break;

			turn = i;

			roll = rolldie();
			gameOutput(i + 1, "you rolled " + int_to_str(roll) + "\r\n", players[turn]->color);

			//move player up/down
			//find a stack with the current players hedgehog on top, if not found skip move/up down step
			bool found = false;
			for (UINT j = 0; j < board.squares.size(); j++) {
				for (UINT k = 0; k < board.squares[j].size() - 2; k++) {
					if (board.squares[j][k].stack.size() && board.squares[j][k].stack.back()->id == turn && (canMoveUp(j, k) || canMoveDown(j, k))) {
						found = true;
						break;
					}
				}
				if (found) break;
			}
			if (found || (varients & VARIENT_ANARCHOHEDGEHOG)) {
				gameOutput(i + 1, "move up or down. Right click to skip\r\n", players[turn]->color);
				if (isAImove()) {
					while (moving) this_thread::sleep_for(chrono::milliseconds(10));//wait for move to finish
					
					int fr, fc, tr, tc;
					int move_dir = AI_moveUpDown(fr, fc, tr, tc);
					switch (move_dir) {
						case MOVE_DOWN:
						case MOVE_UP:{
							bool can_move = false;
							if (move_dir == MOVE_UP)
								can_move = canMoveUp(fr, fc);
							else if (move_dir == MOVE_DOWN)
								can_move = canMoveDown(fr, fc);
							
							if (can_move) {
								PostMessage(GetDlgItem(g_hwnd, IDC_GAMEWINDOW), WM_SEND_MOVE, MAKEWPARAM(fc, fr), MAKELPARAM(tc, tr));
								wait();
							}
							else if (!gameover) {
								gameOutput(i + 1, "none of your hedgehogs can move!\r\n", players[turn]->color);
							}

							break;
						}
					}		
				}
				else {
					block_input = false;
					wait();//wait for player input
				}
			}
			else if (!gameover) {
				gameOutput(i + 1, "none of your hedgehogs can move!\r\n", players[turn]->color);
			}	

			if (gameover)//prevents crash
				break;

			players[turn]->hasMoved = true;

			///////////////////////////////////////////////////////////////////////

			//move headhog right (forward)
			//search for a hedgehog in rolled row, if none are found skip to next players turn
			found = false;
			for (UINT j = 0; j < board.squares[roll - 1].size() - 1; j++) {
				if (board.squares[roll - 1][j].stack.size() && canMoveForward(roll - 1, j)) {
					found = true;
					break;
				}
			}
			if (found) {
				gameOutput(i + 1, "move a hedgehog in row #" + int_to_str(roll) + "\r\n", players[turn]->color);

				if (isAImove()) {					
					while (moving) this_thread::sleep_for(chrono::milliseconds(10));//wait for move to finish
					
					int fr, fc;
					AI_moveForward(fr, fc);
					PostMessage(GetDlgItem(g_hwnd, IDC_GAMEWINDOW), WM_SEND_MOVE, MAKEWPARAM(fc, fr), MAKELPARAM(fc + 1, fr));
					wait();
				}
				else {
					block_input = false;
					wait();//wait for player input
				}
			}
			else if (!gameover) {
				gameOutput(i + 1, "No Valid moves\r\n", players[turn]->color);
			}
			
			if (gameover)//prevents crash
				break;

			///////////////////////////////////////////////////////////////////////

			gameOutput("-----------------------------------\r\n", 0);

			//reset player for their next turn
			players[turn]->hasMoved = false;
		}
	}
}

void Game::wait() {
	pause = true;
	while (pause) {
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}
void Game::signal() {
	pause = false;
}

void Game::checkWin(Player* p, int r, int c) {
	if (c + 1 == board.columns) {//got to finish line
		static bool first_doping = false;
		if (!first_doping && (varients & VARIENT_DOPING)) {
			first_doping = true;
			auto a = [&](Player* p, int r, int c) {
				p->piece_count--;
				while (moving) this_thread::sleep_for(chrono::milliseconds(10));
				board.squares[r][c].stack.pop_back();
				gameOutput(p->id + 1, "Your Hedgehog is obviously doping and has been disqualified\r\n", p->color);
				if (p->piece_count < numWin)
					gameOutput(p->id + 1, "You no long have enough hedgehogs to win!\r\n", p->color);
			};
			thread t(a, p, r, c);
			t.detach();
		}
		else {
			if (board.squares[r][c].stack.size()) {
				p->numFinished++;//add one to players score
				if (board.squares[r][c].stack.back()->numFinished == numWin) {//if score is equal to win condtion count they win
					promptNewGame("Player " + int_to_str(board.squares[r][c].stack.back()->id + 1)
						+ " won!\n\nWould you like to play again?", "Game Over");
				}
			}
		}
	}
}

bool Game::canPlace(int row) {
	UINT total_placed = 0;
	UINT min = (UINT)-1;//max value for unsigned int
	UINT minindex = (UINT)-1;
	for (UINT i = 0; i < board.squares.size(); i++) {
		UINT size = board.squares[i][0].stack.size();
		total_placed += size;
		if (size < min) {
			min = size;
			minindex = i;
		}
	}
	UINT max = (total_placed / board.rows) + 1;

	if ((int)min <= (int)max - 2) 
		return (UINT)row == minindex;

	bool cover_exception = true;
	for (UINT i = 0; i < board.squares.size(); i++) {
		if (!board.squares[i][0].stack.size()) {//all empty rows must be filled first
			cover_exception = false;
			break;
		}
		if (board.squares[i][0].stack.size() < max && board.squares[i][0].stack.back()->id != turn) {
			cover_exception = false;
			break;
		}
	}

	return (board.squares[row][0].stack.size() < max || cover_exception);
}

bool Game::obstacleLogic(int row, int column) {
	int r = row, c = column;
	switch (board.squares[r][c].obstacle_type) {
		case OBS_PIT:{
			for (UINT i = 0; i < (UINT)board.rows; i++) {
				for (UINT j = 0; j < (UINT)c; j++) {//loop through all squares behind selected hedgehog, if one is found return false
					if (board.squares[i][j].stack.size() > 0) {
						return false;
					}
				}
			}
			return true;
		}
		case OBS_BLACK_HOLE:{
			return false;//rekt
		}
		case OBS_SHALLOW_PIT:{
			if (board.squares[row][column].stack.size() == 1) {//if only hedgehog in shallow pit
				for (UINT i = 0; i < (UINT)c; i++) {//loop through all squares behind selected hedgehog, if one is found return false
					if (board.squares[r][i].stack.size() > 0) {
						return false;
					}
				}
			}
			return true;
		}
	}
	return true;
}

bool Game::canMoveUp(int row, int column) {
	int r = row, c = column;//temp vars for easier typing

	if (!board.squares[r][c].stack.size())
		return false;
	if (players[turn]->hasMoved)
		return false;
	if (row == 0 && !(varients & VARIENT_TUBE))
		return false;

	return obstacleLogic(r, c);
}	 
bool Game::canMoveDown(int row, int column) {
	int r = row, c = column;//temp vars for easier typing

	if (!board.squares[r][c].stack.size())
		return false;
	if (players[turn]->hasMoved)
		return false;
	if (row == board.rows - 1 && !(varients & VARIENT_TUBE))
		return false;

	return obstacleLogic(r, c);
}
bool Game::canMoveForward(int row, int column) {
	int r = row, c = column;//temp vars for easier typing
	
	if (!board.squares[r][c].stack.size())
		return false;
	if (c + 1 >= board.columns)//can't move off the end of the board
		return false;
	if (players[turn]->hasMoved && row != roll - 1)//a player has moved up/down, so if piece isn't in rolled row return false
		return false;
	else if ((!players[turn]->hasMoved && players[turn]->id != board.squares[r][c].stack.back()->id) && !(varients & VARIENT_ANARCHOHEDGEHOG))//player hasn't moved up/down yet, but its not your turn, return false
		return false;
	
	return obstacleLogic(r, c);
}

bool Game::move(Player* p, int fromr, int fromc, int tor, int toc) {
	bool ret = false;

	if (board.squares[fromr][fromc].stack.size()) {
		ret = true;
		if (animation_speed)
			moving = true;
		this->fromr = fromr;
		this->fromc = fromc;
		this->tor = tor;
		this->toc = toc;
		board.squares[tor][toc].stack.push_back(board.squares[fromr][fromc].stack.back());//move foward one square
		board.squares[fromr][fromc].stack.pop_back();
	}

	obstacleEvent(p, tor, toc);//check if landed in another obstacle
	checkWin(p, tor, toc);//check if game ended

	return ret;
}

void Game::obstacleEvent(Player* p, int r, int c) {
	auto a = [&](Player* p, int r, int c) {
		while (moving) this_thread::sleep_for(chrono::milliseconds(10));
		switch (board.squares[r][c].obstacle_type) {
			case OBS_PIT:{
				gameOutput(p->id + 1, "one of your hedgehogs fell into a Pit! It can leave when all other hedgehogs have caught up\r\n", p->color);
				break;
			}
			case OBS_BLACK_HOLE:{
				gameOutput(p->id + 1, "one of your hedgehogs fell into a Black Hole! There is no escapse.\r\n", p->color);
				p->piece_count--;

				//find and remove the hedge from the game (it fell into a black hole) cya

				if (board.squares[r][c].stack.size())
					board.squares[r][c].stack.pop_back();

				if (p->piece_count < numWin) {
					gameOutput(p->id + 1, "You no longer have enough hedgehogs to win!\r\n", p->color);

					//check if game can still end by a player getting enough of their hedgehogs to finish
					vector<int> found;
					for (UINT j = 0; j < players.size(); j++) {
						if (players[j]->piece_count >= numWin) {
							found.push_back(players[j]->id);
						}
					}
					if (found.size() == 1) {
						gameOutput("-----------------------------------\r\n", RGB(255, 255, 255));
						gameOutput(found[0] + 1, "You Probably are going to Win! No other player has enough hedgehogs remaining to win!\r\n", players[found[0]]->color);
						//			promptNewGame("Player " + int_to_str((players[found[0]]->id + 1)) + ": Wins!\nNo other player has enough hedgehogs remaining to win!\nWould you like to play again ? ");
					}
					else if (found.size() == 0) {
						//TODO fix blank newgame bug
						gameOutput("-----------------------------------\r\n", RGB(255, 255, 255));
						gameOutput("No Player can possibly win.\r\n", RGB(255, 255, 255));
						gameOutput("Game Over.\r\n", RGB(255, 255, 255));
						gameover = true;
						signal();
						promptNewGame("No player has enough hedgehogs left to win.\nWould you like to play again?", "Game Over");
					}
				}
				break;
			}
			case OBS_SHALLOW_PIT:{
				if (board.squares[r][c].stack.size() == 1)
					gameOutput(p->id + 1, "one of your hedgehogs fell into a Shallow Pit! It can leave when all other hedgehogs have caught up\r\n", p->color);

				break;
			}
			case OBS_TRAMPOLINE:{
				if (board.squares[r][c].stack.size()) {
					gameOutput(p->id + 1, "one of your hedgehogs jumped onto a trampoline and bounced forward one square!\r\n", p->color);
					if (c < board.columns - 1) {//dont bounce off of board
						int tc = c + 1;
						while (tc < board.columns - 1 && board.squares[r][tc].obstacle_type == OBS_TRAMPOLINE)
							tc++;
						move(p, r, c, r, tc);
					}
				}
				break;
			}
		}
		if (c == board.columns - 1 && board.squares[r][c].obstacle_type != OBS_BLACK_HOLE) {
			gameOutput(p->id + 1, "one of your hedgehogs got to the finish! your score is now: " + int_to_str(p->numFinished) + "\r\n", p->color);
			bool found = false;
			for (UINT i = 0; i < board.squares.size(); i++) {
				for (UINT j = 0; j < board.squares[i].size() - 1; j++) {
					if (board.squares[i][j].stack.size()) {
						found = true;
						break;
					}
				}
			}
			if (!found) gameover = true;
		}
	};
	thread t(a, p, r, c);
	t.detach();
}

void Game::gameOutput(int playerid, string output, COLORREF color) {
	if (gameover) return;

	lock_guard<mutex> lock(outputmutex);
	
	HWND out = GetDlgItem(g_hwnd, IDC_OUTPUT);
	string text = (playerid >= 0 ? "Player " + int_to_str(playerid) + ": " : "") + output;

	outbuffer.push_front(make_pair(text, color));

	text = "";
	int len = 0;
	if (outbuffer.size())
		text += ">" + outbuffer[0].first + "\r\n";

	for (UINT i = 1; i < outbuffer.size(); i++) {
		text += outbuffer[i].first;
	}

	SetWindowText(out, STW(text));

	for (UINT i = 0; i < outbuffer.size() && i < 50; i++) {
		SetRichTextColor(out, outbuffer[i].second, len, len + outbuffer[i].first.size() - 1);
		len += outbuffer[i].first.size() - (i == 0 ? 0 : 1);
	}
	if (outbuffer.size() > 250)
		outbuffer.resize(250);
}
void Game::gameOutput(string output, COLORREF color) {
	gameOutput(-1, output, color);
}

bool Game::AI_isMoveSafe(int fr, int fc, int tr, int tc) {
	if (tr < 0 || tr > board.rows - 1 || tc < 0 || tc > board.columns - 1)//out of range
		return false;
	switch (board.squares[tr][tc].obstacle_type) {
		case OBS_NONE:
		case OBS_SHALLOW_PIT:
		case OBS_PIT:{
			board.squares[tr][tc].stack.push_back(board.squares[fr][fc].stack.back());
			board.squares[fr][fc].stack.pop_back();
			bool move = canMoveForward(tr, tc) || tc == board.columns - 1;
			board.squares[fr][fc].stack.push_back(board.squares[tr][tc].stack.back());
			board.squares[tr][tc].stack.pop_back();

			return move;
		}
		case OBS_TRAMPOLINE:{
			bool is_safe = AI_isMoveSafe(fr, fc, tr, tc + 1);
			return is_safe;
		}
		case OBS_BLACK_HOLE:
			return false;
	}
	return false;
};
int Game::AI_moveUpDown(int& fr, int& fc, int& tr, int& tc) {
	int ret = MOVE_DONT;

	//find all AI hedgehogs capable of moving
	for (int j = board.columns - 2; j >= 0; j--) {
		for (int i = board.squares.size() - 1; i >= 0; i--) {
			if (!board.squares[i][j].stack.size())
				continue;
			if (players[turn]->id != board.squares[i][j].stack.back()->id) //is top of stack current AI
				continue;

			int ui = (varients & VARIENT_TUBE) && i == 0 ? board.rows - 1 : i - 1;
			int di = ((varients & VARIENT_TUBE) && i == board.rows - 1) ? 0 : i + 1;

			if (i == roll - 1) {//in rolled row
				if (AI_isMoveSafe(i, j, i, j + 1)) {//try to move forward
					cout << "stay - ";
					ret = MOVE_DONT;
				}
				else {//in rolled row, but moving forward isn't safe
					if (canMoveUp(i, j) && AI_isMoveSafe(i, j, ui, j)) {//try to move up to dodge obstacle
						cout << "up dodge - ";
						ret = MOVE_UP;
					}
					else if (canMoveDown(i, j) && AI_isMoveSafe(i, j, di, j)) {//try to move down to dodge obstacle
						cout << "down dodge - ";
						ret = MOVE_DOWN;
					}
				}
			}
			else if ((i - 1 == roll - 1) || ((varients & VARIENT_TUBE) && i == 0 && roll - 1 == board.rows - 1)) {//below rolled row
				if (canMoveUp(i, j) && AI_isMoveSafe(i, j, ui, j) && AI_isMoveSafe(i, j, ui, j + 1)) {//try to move up
					cout << "up rolled - ";
					ret = MOVE_UP;
				}
				else
					ret = MOVE_DONT;
			}
			else if ((i + 1 == roll - 1) || ((varients & VARIENT_TUBE) && i == board.rows - 1 && roll - 1 == 0)) {//above rolled row
				if (canMoveDown(i, j) && AI_isMoveSafe(i, j, di, j) && AI_isMoveSafe(i, j, di, j + 1)) {//try to move down
					cout << "down rolled - ";
					ret = MOVE_DOWN;
				}
				else
					ret = MOVE_DONT;
			}

			switch (ret) {
				case MOVE_UP:
					fr = i;
					fc = j;
					tr = ui;
					tc = j;
					break;
				case MOVE_DOWN:
					fr = i;
					fc = j;
					tr = di;
					tc = j;
					break;
			}

			if (ret)
				break;
		}
		if (ret)
			break;
	}
	if (ret) return ret;

	//if none of AI hedgehogs can safetly move forward try to cover other players hedgehogs or uncover own
	for (int j = board.columns - 2; j >= 0; j--) {
		for (int i = board.squares.size() - 1; i >= 0; i--) {
			if (!board.squares[i][j].stack.size())
				continue;
			if (players[turn]->id != board.squares[i][j].stack.back()->id) //is top of stack current AI
				continue;

			int ui = (varients & VARIENT_TUBE) && i == 0 ? board.rows - 1 : i - 1;
			int di = ((varients & VARIENT_TUBE) && i == board.rows - 1) ? 0 : i + 1;
			auto countstack = [&](int r, int c, int id) -> int {
				if (varients & VARIENT_TUBE) {
					if (r == -1) r = board.rows - 1;
					else if (r = board.rows - 1) r = 0;
				}
				if (r < 0 || r > board.rows - 1 || c < 0 || c > board.columns - 1)//out of range
					return false;

				int found = 0;
				for (UINT k = 0; k < board.squares[r][c].stack.size(); k++) {
					if (board.squares[r][c].stack[k]->id == id)
						found++;
				}
				return found;
			};

			int playeru = countstack(ui, j, board.squares[i][j].stack.back()->id);
			int playerc = countstack(i, j, board.squares[i][j].stack.back()->id) - 1;
			int playerd = countstack(di, j, board.squares[i][j].stack.back()->id);

			if (playerc) {//current ai is covering its own peices in stack so try to move off
				if (playerd < playerc && canMoveDown(i, j) && AI_isMoveSafe(i, j, di, j)) {//try to move down to free own pieces below top
					cout << "down free - ";
					ret = MOVE_DOWN;
				}
				else if (playeru < playerc && canMoveUp(i, j) && AI_isMoveSafe(i, j, ui, j)) {//try to move up to free own pieces below top
					cout << "up free - ";
					ret = MOVE_UP;
				}
				else
					ret = MOVE_DONT;
			}
			else {//try to cover other stacks
				int uid = ui >= 0 && board.squares[ui][j].stack.size() ? board.squares[ui][j].stack.back()->id : -1;
				int pid = board.squares[i][j].stack.back()->id;
				int did = di <= board.rows - 1 && board.squares[di][j].stack.size() ? board.squares[di][j].stack.back()->id : -1;

				//dont let another player win
				if (j == board.columns - 2) {//stack onto other players about to get to finish
					if (uid > -1 && uid != pid && canMoveUp(i, j) && players[uid]->numFinished == numWin - 1) {//emergency! another player is about to win, stack on them
						cout << "emergency up prevent win - ";
						ret = MOVE_UP;
					}
					else if (did > -1 && did != pid && canMoveDown(i, j) && players[did]->numFinished == numWin - 1) {//emergency! another player is about to win, stack on them
						cout << "emergency down prevent win - ";
						ret = MOVE_DOWN;
					}
					else {//just check if another player is about to reach finish
						if (uid > -1 && uid != pid && canMoveUp(i, j) && AI_isMoveSafe(i, j, ui, j) && players[uid]->numFinished < numWin) {//dont let player score until you have
							cout << "up prevent win - ";
							ret = MOVE_UP;
						}
						else if (did > -1 && did != pid && canMoveDown(i, j) && AI_isMoveSafe(i, j, di, j) && players[did]->numFinished < numWin) {//dont let player score until you have
							cout << "down prevent win - ";
							ret = MOVE_DOWN;
						}
						else
							ret = MOVE_DONT;
					}
				}
				else if (playeru == 0 && playerd == 0) {//try to cover a stack of other players (biggest)
					int us = ui >= 0 ? board.squares[ui][j].stack.size() : 0;
					int ps = board.squares[i][j].stack.size();
					int ds = di <= board.rows - 1 ? board.squares[di][j].stack.size() : 0;
					int m = ((us > ps) ? ((us > ds) ? us : ds) : ((ps > ds) ? ps : ds));
					if (m == ps || m == 0) {
						ret = MOVE_DONT;
					}
					else if (canMoveUp(i, j) && AI_isMoveSafe(i, j, ui, j) && us > ds) {
						cout << "cover up - ";
						ret = MOVE_UP;
					}
					else if (canMoveDown(i, j) && AI_isMoveSafe(i, j, di, j)) {
						cout << "cover down - ";
						ret = MOVE_DOWN;
					}
					else
						ret = MOVE_DONT;
				}
				else
					ret = MOVE_DONT;
			}

			switch (ret) {
				case MOVE_UP:
					fr = i;
					fc = j;
					tr = ui;
					tc = j;
					break;
				case MOVE_DOWN:
					fr = i;
					fc = j;
					tr = di;
					tc = j;
					break;
			}

			if (ret)
				break;
		}
		if (ret)
			break;
	}

	return ret;
}
int Game::AI_moveForward(int& fr, int& fc) {
	int r = roll - 1;
	for (int c = board.squares[r].size() - 2; c >= 0; c--) {//loop right to left (find most furthest out hedgehog and move it first
		if (!board.squares[r][c].stack.size())
			continue;

		if (board.squares[r][c].stack.back()->id == turn) {//is ai piece
			if (canMoveForward(r, c) && AI_isMoveSafe(r, c, r, c + 1)) {//move it forward
				cout << "advance\n";
				fr = r;
				fc = c;
				return 1;
			}
		}
	}
	for (int c = board.squares[r].size() - 3; c >= 0; c--) {//right to left to move piece that isn't yours into an obstacle (never move into finish)
		if (!board.squares[r][c].stack.size())
			continue;
		
		if (canMoveForward(r, c) && !AI_isMoveSafe(r, c, r, c + 1)) {//move a
			cout << "move other into obstacle\n";
			fr = r;
			fc = c;
			return 1;
		}		
	}
	for (UINT c = 0; c < board.squares[r].size() - 1; c++) {//loop left to right and move furthest back piece forward
		if (!board.squares[r][c].stack.size())
			continue;

		if (canMoveForward(r, c)) {
			cout << "move other in back\n";
			fr = r;
			fc = c;
			return 1;
		}
	}

	return 0;
}

BOOL CALLBACK Game::StaticGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->GameProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::StaticNewGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->NewGameProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::StaticGameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->GameDisplayProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::StaticLegendProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWL_USERDATA))->LegendProc(hwnd, message, wParam, lParam);
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

			//subclass game window 
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_GAMEWINDOW), GWLP_USERDATA, (LONG_PTR)this_game);
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_GAMEWINDOW), GWLP_WNDPROC, (LONG_PTR)Game::StaticGameDisplayProc);

			//subclass legend window
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_LEGEND), GWLP_USERDATA, (LONG_PTR)this_game);
			SetWindowLongPtr(GetDlgItem(hwnd, IDC_LEGEND), GWLP_WNDPROC, (LONG_PTR)Game::StaticLegendProc);

			//set up display
			SendMessage(GetDlgItem(hwnd, IDC_OUTPUT), EM_SETBKGNDCOLOR, NULL, RGB(25, 25, 25));
			SendMessage(GetDlgItem(hwnd, IDC_OUTPUT), EM_SETWORDWRAPMODE, WBF_WORDWRAP | WBF_WORDBREAK, 0);

			if (!this_game->is_building_board) {
				this_game->startNewGame();
				SetMenu(hwnd, LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1)));
			}
			else {
				SetWindowText(hwnd, L"Custom Board Creator");
				this_game->gameOutput("Select an obstacle type below, then click where you want the selected obstacle on the board.\r\n", RGB(255, 255, 255));
			}

			//redraw game window on a timer
			SetTimer(GetDlgItem(hwnd, IDC_GAMEWINDOW), 0, 10, Game::TimerProc);
			//redraw legend on a timer
			SetTimer(GetDlgItem(hwnd, IDC_LEGEND), 0, 100, Game::TimerProc);

			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case ID_GAME_NEWGAME:{
					promptNewGame("Are you sure you want to start a new game?", "New Game");
					break;
				}
			}
			switch (LOWORD(wParam)) {
				case IDC_OUTPUT:{//hide caret and never set focus to output window
					switch (HIWORD(wParam)) {
						case EN_SETFOCUS: {
							HideCaret(GetDlgItem(hwnd, LOWORD(wParam)));
							SetFocus(NULL);
							break;
						}
						case EN_KILLFOCUS:{
							ShowCaret(GetDlgItem(hwnd, LOWORD(wParam)));
							break;
						}
					}

					break;
				}
			}
			break;
		}
		case WM_SIZE:{
			SendMessage(GetDlgItem(hwnd, IDC_GAMEWINDOW), WM_SIZE, wParam, lParam);//send size message to gamewindow and let that window handle resizing
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			KillTimer(GetDlgItem(hwnd, IDC_GAMEWINDOW), 0);
			KillTimer(GetDlgItem(hwnd, IDC_LEGEND), 0);

			random_device rd;
			mt19937 gen(rd());
			if (!is_building_board && gen() % 1000 == 1)
				MessageBox(hwnd, L"No, this game is really fun. Keep playing", L"0.1%", MB_OK | MB_ICONHAND);
			else
				EndDialog(hwnd, 0);

			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK Game::NewGameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int bcustom = false;//saves custom board checkbox state for future games
	static int bvarients = false;//saves varient checkbox state for future games
	switch (message) {
		case WM_INITDIALOG:{
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

			Game* this_game = reinterpret_cast<Game*>(lParam);//lParam (Game*) is already construced with default parameters, or maintains old ones

			//initialize game parameter inputs to default values
			SetWindowText(GetDlgItem(hwnd, IDC_EDCNUMPLAYERS), STW(int_to_str(this_game->numPlayers)));
			SetWindowText(GetDlgItem(hwnd, IDC_EDCPLAYERHOGS), STW(int_to_str(this_game->numHogs)));
			SetWindowText(GetDlgItem(hwnd, IDC_EDCNUMAI), STW(int_to_str(this_game->numAI)));
			SetWindowText(GetDlgItem(hwnd, IDC_EDCWINHOGS), STW(int_to_str(this_game->numWin)));
			SetWindowText(GetDlgItem(hwnd, IDC_EDCROW), STW(int_to_str(this_game->board.rows)));
			SetWindowText(GetDlgItem(hwnd, IDC_EDCCOLUMN), STW(int_to_str(this_game->board.columns)));

			//set controls to previous settings
			Button_SetCheck(GetDlgItem(hwnd, IDC_CUSTOMBOARD), bcustom);
			Button_SetCheck(GetDlgItem(hwnd, IDC_VARIENTS), bvarients);

			//resize window to fit based on previous settings
			SendMessage(hwnd, WM_COMMAND, IDC_CUSTOMBOARD, NULL);
		
			break;
		}
		case WM_SETCURSOR:{//hack (easier than tracking mouse events for every control)
			POINT p = getclientcursorpos(hwnd);

			HWND desc = GetDlgItem(hwnd, IDC_VARIENT_DESCRIPTION);

			//get rects for varient check boxes
			RECT rcanarch = getmappedclientrect(GetDlgItem(hwnd, IDC_VARIENT_ANARCHOHEDGEHOG), hwnd);
			RECT rcdoping = getmappedclientrect(GetDlgItem(hwnd, IDC_VARIENT_DOPING), hwnd);
			RECT rctube = getmappedclientrect(GetDlgItem(hwnd, IDC_VARIENT_TUBE), hwnd);

			//show description of each varient on mouse hover
			static int i = 0;
			if (PtInRect(&rcanarch, p)) {
				if (i != 1) {
					i = 1;
					SetWindowText(desc, L"Any hedgehog can be moved sideways; sideways moves are no longer restricted to hedgehogs of the players color");
				}
			}
			else if (PtInRect(&rcdoping, p)) {
				if (i != 2) {
					i = 2;
					SetWindowText(desc, L"First hedgehog to finish gets removed from game and doesn't count towards players score");
				}
			}
			else if (PtInRect(&rctube, p)) {
				if (i != 3) {
					i = 3;
					SetWindowText(desc, L"Top and bottom rows are connected allowing hedgehogs to travel between them");
				}
			}
			else {
				i = 0;
				SetWindowText(desc, L"");
			}

			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDOK:{
					int playerc = str_to_int(getwindowtext(GetDlgItem(hwnd, IDC_EDCNUMPLAYERS)));
					int aic = str_to_int(getwindowtext(GetDlgItem(hwnd, IDC_EDCNUMAI)));
					int hogc = str_to_int(getwindowtext(GetDlgItem(hwnd, IDC_EDCPLAYERHOGS)));
					int winc = str_to_int(getwindowtext(GetDlgItem(hwnd, IDC_EDCWINHOGS)));
					int rowc = str_to_int(getwindowtext(GetDlgItem(hwnd, IDC_EDCROW)));
					int colc = str_to_int(getwindowtext(GetDlgItem(hwnd, IDC_EDCCOLUMN)));

					//get varient states from checkboxes
					if (Button_GetCheck(GetDlgItem(hwnd, IDC_VARIENTS)) & BST_CHECKED) {
						varients = VARIENT_NONE;
						if (Button_GetCheck(GetDlgItem(hwnd, IDC_VARIENT_TUBE)))
							varients |= VARIENT_TUBE;
						if (Button_GetCheck(GetDlgItem(hwnd, IDC_VARIENT_DOPING)))
							varients |= VARIENT_DOPING;
						if (Button_GetCheck(GetDlgItem(hwnd, IDC_VARIENT_ANARCHOHEDGEHOG)))
							varients |= VARIENT_ANARCHOHEDGEHOG;
					}

					//check input validity
					if (colc <= 1) {
						MessageBox(NULL, L"# of columns must be greater than one", L"Input Error!", MB_OK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_EDCCOLUMN), 0, -1);
						SetFocus(GetDlgItem(hwnd, IDC_EDCCOLUMN));
						break;
					}
					if (rowc <= 0) {
						MessageBox(NULL, L"# of rows must be greater than zero", L"Input Error!", MB_OK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_EDCROW), 0, -1);
						SetFocus(GetDlgItem(hwnd, IDC_EDCROW));
						break;
					}
					if (hogc <= 0) {
						MessageBox(NULL, L"# of hogs per player must be greater than zero", L"Input Error!", MB_OK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_EDCPLAYERHOGS), 0, -1);
						SetFocus(GetDlgItem(hwnd, IDC_EDCPLAYERHOGS));
						break;
					}
					if (playerc <= 0) {
						MessageBox(NULL, L"# of players must be greater than zero", L"Input Error!", MB_OK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_EDCNUMPLAYERS), 0, -1);
						SetFocus(GetDlgItem(hwnd, IDC_EDCNUMPLAYERS));
						break;
					}
					if (aic > playerc || aic < 0) {
						MessageBox(NULL, L"Number of AI cannot exceed number of players!", L"Input Error!", MB_OK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_EDCWINHOGS), 0, -1);
						SetFocus(GetDlgItem(hwnd, IDC_EDCWINHOGS));
						break;
					}
					if (hogc < winc || winc == 0) {
						MessageBox(NULL, L"# of hogs required to win must be less than or equal\nto the total # of hogs per player! (also can't be zero)", L"Input Error!", MB_OK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_EDCWINHOGS), 0, -1);
						SetFocus(GetDlgItem(hwnd, IDC_EDCWINHOGS));
						break;
					}

					//set member vars from input
					board.rows = rowc;
					board.columns = colc;
					numWin = winc;
					numHogs = hogc;
					turn = 0;
					gameover = false;
					roll = -1;
					numPlayers = playerc;
					numAI = aic;

					Board b;//default constructor initialzes with default board
					if (Button_GetCheck(GetDlgItem(hwnd, IDC_CUSTOMBOARD))) {
						string filename = getwindowtext(GetDlgItem(hwnd, IDC_EDC_CUSTOMBOARD));
						if (filename.size()) {//load from file
							auto it = filename.rfind(".");
							if (it != string::npos && filename.substr(it, string::npos) == ".board") {//filetype must be .board
								ifstream file;
								file.open(filename);
								if (file.is_open())
									b.SetBoard(file);//intialize board object with custom board from file
								else {
									MessageBox(NULL, STW("Error opening file\n" + error_code_to_text(GetLastError())), L"Error loading file", MB_OK);
									break;
								}
								file.close();
							}
							else{
								MessageBox(NULL, STW("Invalid File."), L"Error", MB_OK);
								break;
							}
						}
						else {//use board editor
							is_building_board = true;
							init_game = false;
							selected_obstacle = 0;

							//preset size of board
							board.squares.resize(rowc);
							for (UINT i = 0; i < board.squares.size(); i++)
								board.squares[i].resize(colc);

							//set all square types on board to be normal squares
							for (UINT i = 0; i < (UINT)rowc; i++) {
								for (UINT j = 0; j < (UINT)colc; j++) {
									board.squares[i][j].obstacle_type = 0;
								}
							}

							EnableWindow(hwnd, FALSE);//disable 'new game' window

							HWND old = g_hwnd;//save old g_hwnd

							//startup board editor (recycles GameProc, so g_hwnd gets changed, thus the need to save and restore it)
							DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)Game::StaticGameProc, (LPARAM)this);

							g_hwnd = old;//restore g_hwnd
							
							//reset game vars, end building state and start initializing game (players placing pieces)
							outbuffer.clear();
							is_building_board = false;
							init_game = true;

							//board builder directly builds the board, but SetBoard method takes 2-d vector of int, not 2-d vector of Square, so move board into int vector
							vector<vector<int>> obs;
							for (UINT i = 0; i < board.squares.size(); i++) {
								vector<int> t;
								for (UINT j = 0; j < board.squares[i].size(); j++) {
									t.push_back(board.squares[i][j].obstacle_type);
								}
								obs.push_back(t);
							}

							b.SetBoard(obs);							
						}
					}
					board = b;

					EndDialog(hwnd, 0);
					break;
				}
				case IDCANCEL:{
					exit(0);
					break;
				}
				case IDC_VARIENTS:
				case IDC_CUSTOMBOARD:{
					//turn on extra controls for advanced board creation//
					 ////////////////////////////////////////////////////

					bcustom = Button_GetCheck(GetDlgItem(hwnd, IDC_CUSTOMBOARD));
					bvarients = Button_GetCheck(GetDlgItem(hwnd, IDC_VARIENTS));

					//toggle extra controls visibility
					ShowWindow(GetDlgItem(hwnd, IDC_EDCROW), bcustom);
					ShowWindow(GetDlgItem(hwnd, IDC_EDCCOLUMN), bcustom);
					ShowWindow(GetDlgItem(hwnd, IDC_EDC_CUSTOMBOARD), bcustom);
					ShowWindow(GetDlgItem(hwnd, IDC_FILEBROWSE), bcustom);
					ShowWindow(GetDlgItem(hwnd, IDC_STATIC1), bcustom);
					ShowWindow(GetDlgItem(hwnd, IDC_STATIC2), bcustom);

					//resize window to fit extra controls
					SetWindowPos(hwnd, NULL, 0, 0, bvarients ? 520 : 270, bcustom ? 310 + 20 : 231 + 20, SWP_NOMOVE | SWP_NOZORDER);

					//move statics for row and column into place
					SetWindowPos(GetDlgItem(hwnd, IDC_STATIC2), NULL, 22, 158 + 20, 0, 0, SWP_NOSIZE);
					SetWindowPos(GetDlgItem(hwnd, IDC_STATIC1), NULL, 127, 158 + 20, 0, 0, SWP_NOSIZE);

					//move edits for row and column into place
					SetWindowPos(GetDlgItem(hwnd, IDC_EDCCOLUMN), NULL, 182, 157 + 20, 0, 0, SWP_NOSIZE);
					SetWindowPos(GetDlgItem(hwnd, IDC_EDCROW), NULL, 57, 157 + 20, 0, 0, SWP_NOSIZE);

					//move file edit and browser button into place
					SetWindowPos(GetDlgItem(hwnd, IDC_EDC_CUSTOMBOARD), NULL, 23, 195 + 20, 0, 0, SWP_NOSIZE);
					SetWindowPos(GetDlgItem(hwnd, IDC_FILEBROWSE), NULL, 197, 195 + 20, 0, 0, SWP_NOSIZE);
					
					//move ok and cancel button to bottom
					SetWindowPos(GetDlgItem(hwnd, IDOK), NULL, 48, bcustom ? 232 + 20 : 152 + 20, 0, 0, SWP_NOSIZE);
					SetWindowPos(GetDlgItem(hwnd, IDCANCEL), NULL, 135, bcustom ? 232 + 20 : 152 + 20, 0, 0, SWP_NOSIZE);
					
					break;
				}
				case IDC_FILEBROWSE:{
					//browse for .board file
					string filename = browseforfile(hwnd, true, L"Browse for file", L"Boards\0*.board\0");
					SetWindowText(GetDlgItem(hwnd, IDC_EDC_CUSTOMBOARD), STW(filename));

					break;
				}
			}
			break;
		}
		case WM_DROPFILES:{
			//handle drag and drop of .board file
			wchar_t text[MAX_PATH];
			DragQueryFile((HDROP)wParam, 0, text, MAX_PATH);
			string filename = wstr_to_str(text);
			SetWindowText(GetDlgItem(hwnd, IDC_EDC_CUSTOMBOARD), STW(filename));

			break;
		}
		case WM_CLOSE:{
			exit(0);
		}
		case WM_DESTROY:{
			break;
		}
	}
	return FALSE;
}
BOOL CALLBACK Game::GameDisplayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_PAINT:{
			mutex mu;

			mu.lock();

			RECT rcclient = getclientrect(hwnd);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			/////////////////////////////////////////////////////////////////////
			POINT p = getclientcursorpos(hwnd);

			//draw board
			for (UINT r = 0; r < board.squares.size(); r++) {
				for (UINT c = 0; c < board.squares[r].size(); c++) {
					RECT rc = board.squares[r][c].rc;

					//draw background/////////////////////////////////////////////////////////////////////////////////////
					COLORREF backc = 0;
					if (init_game && c == 0) {//is game being set up and is the current column the first one?
						//set background color to gray if valid square to place a hedgehog
						if (canPlace(r))
							backc = RGB(230, 230, 230);
						else
							backc = RGB(25, 25, 25);
					}
					else {
						if (is_building_board && c == 0) {
							backc = RGB(255, 0, 0);
						}
						/*else if ((UINT)roll - 1 == r && !board.squares[r][c].obstacle_type) {
							backc = RGB(230, 230, 230);
						}*/
						else
							backc = g_obstacles[board.squares[r][c].obstacle_type];
					}
					HBRUSH bbrush = CreateSolidBrush(backc);
					FillRect(hDCmem, &rc, bbrush);
					DeleteObject(bbrush);

					//draw board square frames and highlight when hovered////////////////////////////////////////////////////////////////////////////////////
					COLORREF frame = 0;
					if (PtInRect(&board.squares[r][c].rc, p))
						frame = RGB(128, 0, 0);
					HBRUSH tbrush = CreateSolidBrush(frame);
					FrameRect(hDCmem, &board.squares[r][c].rc, tbrush);
					RECT inner = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
					FrameRect(hDCmem, &inner, tbrush);
					DeleteObject(tbrush);

					//draw lines for rolled row////////////////////////////////////////////////////////////////////////////////////
					if (!init_game && (UINT)roll - 1 == r) {
						HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
						RECT rolledrow = {rc.left, board.squares[r][c].rc.top, rc.right, board.squares[r][c].rc.bottom};
						FrameRect(hDCmem, &rolledrow, brush);
						DeleteObject(brush);
					}
				}
			}
			//draw hedgehogs and stack
			for (UINT r = 0; r < board.squares.size(); r++) {
				for (UINT c = 0; c < board.squares[r].size(); c++) {
					RECT rc = board.squares[r][c].rc;

					//draw hedgehog stack/////////////////////////////////////////////////////////////////////////////////////
					UINT max = (moving && r == (UINT)tor && c == (UINT)toc) ? board.squares[r][c].stack.size() - 1 : board.squares[r][c].stack.size();
					for (UINT i = 0; i < max; i++) {
						int h = (rc.bottom - rc.top) / ((UINT)numHogs > board.squares[r][c].stack.size() ? numHogs : board.squares[r][c].stack.size());//get height of each piece in stack (default is based on # of hogs per player, but pieces shrink to fit)
						int top = rc.bottom - (h * (i + 1));//get position for the top of the piece based on its position in stack
						RECT src = {rc.left, top, (int)((rc.right - rc.left) * (double)(g_hedgehog_stack_width / 100)) + rc.left, top + h};//set up drawing area (% of the total square on left)
						HPEN pen = CreatePen(PS_SOLID, 2, board.squares[r][c].stack[i]->id == turn ? RGB(255, 255, 255) : canMoveForward(r, c) ? 0 : RGB(200, 200, 200));//use black as border, white if current player
						HBRUSH brush = CreateSolidBrush(board.squares[r][c].stack[i]->color);//use player color as background
						HPEN oldpen = (HPEN)SelectObject(hDCmem, pen);
						HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, brush);
						//draw stack piece
						Rectangle(hDCmem, src.left + 2, src.top, src.right, src.bottom);
						//restore old GDI objects
						pen = (HPEN)SelectObject(hDCmem, oldpen);
						brush = (HBRUSH)SelectObject(hDCmem, oldbrush);
						//clean up GDI objects
						DeleteObject(brush);
						DeleteObject(pen);
					}

					//draw hedgehogs/////////////////////////////////////////////////////////////////////////////////////
					if (board.squares[r][c].stack.size()) {
						//color border of current players hedgehogs white, otherwise player color

						//set border color of all valid moves black unless they belong to player, then color them white, otherwise color them the default color
						COLORREF border = 0;
						if (moving)
							border = board.squares[r][c].stack.back()->color;//use default color
						else if (!init_game && c != (UINT)board.columns - 1) {//only color border after board has been set up and if piece hasn't finished
							if (board.squares[r][c].stack.back()->id == turn) {//is current players piece?
								if (canMoveUp(r, c) || canMoveDown(r, c) || canMoveForward(r, c)) //hasn't moved up/down and has ability to or can move forward?
									border = RGB(255, 255, 255);//color white
								else
									border = board.squares[r][c].stack.back()->color;//use default color
							}
							else {//not players piece
								if (canMoveForward(r, c))//can the piece move?
									border = 0;//color black
								else
									border = board.squares[r][c].stack.back()->color;//use default color
							}
						}
						else
							border = board.squares[r][c].stack.back()->color;//use default color when initilizing board

						COLORREF back = 0;
						if (moving && r == (UINT)tor && c == (UINT)toc && board.squares[r][c].stack.size() > 1) {
							back = board.squares[r][c].stack[board.squares[r][c].stack.size() - 2]->color;
							border = back;
						}
						else
							back = board.squares[r][c].stack.back()->color;

						HPEN pen = CreatePen(PS_SOLID, 4, border);
						HBRUSH brush = CreateSolidBrush(back);
						HPEN oldpen = (HPEN)SelectObject(hDCmem, pen);
						HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, brush);

						if (!(moving && r == (UINT)tor && c == (UINT)toc && board.squares[r][c].stack.size() == 1)) {//skip animated hedgehog and do it last so its always on top
							RECT hogrc = board.getHogRect(r, c);
							//draw the hedgehog (fancy graphics)
							Ellipse(hDCmem, hogrc.left, hogrc.top, hogrc.right, hogrc.bottom);;
						}								

						//restore old GDI objects
						pen = (HPEN)SelectObject(hDCmem, oldpen);
						brush = (HBRUSH)SelectObject(hDCmem, oldbrush);
						//clean up GDI objects
						DeleteObject(brush);
						DeleteObject(pen);

						//draw up and down movement 'buttons'
						if (!moving 
							&& (board.squares[r][c].stack.back()->id == turn || (varients & VARIENT_ANARCHOHEDGEHOG))//current turn or VARIENT_ARARCHHEDGEHOG(any player can move any other play up/down)
							&& !players[turn]->hasMoved //player hasn't moved yet
							&& !init_game //game is running, not being initialized
							&& c != (UINT)board.columns - 1) {//dont allow finished hedgehogs to move up/down

							RECT hogrc = board.getHogRect(r, c);
							RECT tophalf = {hogrc.left, hogrc.top, hogrc.right, (hogrc.bottom - hogrc.top) / 2 + hogrc.top};
							RECT bottomhalf = {hogrc.left, (hogrc.bottom - hogrc.top) / 2 + hogrc.top, hogrc.right, hogrc.bottom};

							if (g_hedgehog_stack_width >= 50) {
								tophalf.left = rc.left + 1;
								bottomhalf.left = rc.left + 1;
							}

							int centerx = (tophalf.right - tophalf.left) / 2 + tophalf.left;
							int quarterx = (int)((double)((centerx - tophalf.left) / 2) / 1.5);
							int tcentery = (tophalf.bottom - tophalf.top) / 2 + tophalf.top + 2;
							int tquatery = (tcentery - tophalf.top) / 2;
							int bcentery = (bottomhalf.bottom - bottomhalf.top) / 2 + bottomhalf.top - 2;
							int bquatery = (bcentery - bottomhalf.bottom) / 2;

							//draw up button
							HPEN border = CreatePen(PS_SOLID, 4, 0);

							COLORREF ucol;
							if (!canMoveUp(r, c))
								ucol = RGB(100, 100, 100);
							else if (PtInRect(&tophalf, p))
								ucol = 0;
							else
								ucol = RGB(255, 255, 255);
							HBRUSH ubrush = CreateSolidBrush(ucol);
							HPEN oldpen = (HPEN)SelectObject(hDCmem, border);
							HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, ubrush);

							POINT upper[4] = {
								{centerx, tcentery - tquatery},
								{centerx + quarterx, tcentery + tquatery},
								{centerx - quarterx, tcentery + tquatery},
								{centerx, tcentery - tquatery}
							};
							//draw up triangle
							Polygon(hDCmem, upper, 4);

							//draw down button
							COLORREF lcol;
							if (!canMoveDown(r, c))
								lcol = RGB(100, 100, 100);
							else if (PtInRect(&bottomhalf, p))
								lcol = 0;
							else
								lcol = RGB(255, 255, 255);
							HBRUSH lbrush = CreateSolidBrush(lcol);
							ubrush = (HBRUSH)SelectObject(hDCmem, lbrush);

							POINT lower[4] = {
								{centerx, bcentery - bquatery},
								{centerx + quarterx, bcentery + bquatery},
								{centerx - quarterx, bcentery + bquatery},
								{centerx, bcentery - bquatery}
							};
							//draw down triangle
							Polygon(hDCmem, lower, 4);

							//clean up
							border = (HPEN)SelectObject(hDCmem, oldpen);
							lbrush = (HBRUSH)SelectObject(hDCmem, oldbrush);
							DeleteObject(lbrush);
							DeleteObject(ubrush);
							DeleteObject(border);
						}
					}
				}
			}
			if (moving) {//draw animated hedgehog (last)
				RECT hogrc = board.getHogRect(tor, toc);
				struct DOULEPOINT {
					double x, y;
				};
				static bool first = true;
				static double dist, dirx, diry;
				static DOULEPOINT start, end, temp;
				int width = hogrc.right - hogrc.left;
				int height = hogrc.bottom - hogrc.top;

				if (first) {//guard these vars (only change when previous animation is done)
					first = false;
					RECT frc = board.getHogRect(fromr, fromc);
					RECT trc = board.getHogRect(tor, toc);
					start = {(double)frc.left, (double)frc.top};//starting point
					temp = start;//copy of starting point (used to store changes)
					end = {(double)trc.left, (double)trc.top};//ending pos

					dist = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));//total distance from start to end
					dirx = (end.x - start.x) / dist;//angle in x direction
					diry = (end.y - start.y) / dist;//angle in y direction
				}

				//move the hedgehog in direction at a certain rate (animation_speed)
				temp.x += dirx * animation_speed;
				temp.y += diry * animation_speed;

				//get distance traveled so far, if its more than the total distance you're done
				if (sqrt(pow(temp.x - start.x, 2) + pow(temp.y - start.y, 2)) >= dist) {
					//reset for next movement animation
					first = true;
					hogrc.left = (LONG)end.x;
					hogrc.top = (LONG)end.y;
					moving = false;
				}
				//update actual position of hedgehog
				if (moving) {
					hogrc.left = (LONG)temp.x;
					hogrc.top = (LONG)temp.y;
				}
				hogrc.right = hogrc.left + width;
				hogrc.bottom = hogrc.top + height;

				HPEN pen = CreatePen(PS_SOLID, 4, board.squares[tor][toc].stack.back()->color);
				HBRUSH brush = CreateSolidBrush(board.squares[tor][toc].stack.back()->color);
				HPEN oldpen = (HPEN)SelectObject(hDCmem, pen);
				HBRUSH oldbrush = (HBRUSH)SelectObject(hDCmem, brush);

				Ellipse(hDCmem, hogrc.left, hogrc.top, hogrc.right, hogrc.bottom);

				//restore old GDI objects
				pen = (HPEN)SelectObject(hDCmem, oldpen);
				brush = (HBRUSH)SelectObject(hDCmem, oldbrush);
				//clean up GDI objects
				DeleteObject(brush);
				DeleteObject(pen);
			}

			/////////////////////////////////////////////////////////////////////
			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			SelectObject(hDCmem, hbmOldBitmap);

			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			mu.unlock();

			break;
		}
		case WM_MOUSEMOVE:{
			POINT p = getclientcursorpos(hwnd);

			//allow "drawing" of obstacles by holding down mouse and dragging (only when control is pressed)
			if (GetAsyncKeyState(VK_CONTROL)) {
				int hoverr, hoverc;
				if (board.hittest(p, &hoverr, &hoverc)) {
					if (wParam & MK_LBUTTON) {
						if (hoverc != 0)//dont allow obstacles to be placed on first column
							board.squares[hoverr][hoverc].obstacle_type = selected_obstacle;//set obstacle for square
					}
					else if (wParam & MK_RBUTTON) {
						if (hoverc != 0)//dont allow obstacles to be placed on first column
							board.squares[hoverr][hoverc].obstacle_type = OBS_NONE;
					}
				}
			}

			break;
		}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:{
			POINT p = getclientcursorpos(hwnd);

			SetFocus(hwnd);

			if (!block_input && !moving || is_building_board) {
				int hoverr, hoverc;
				if (board.hittest(p, &hoverr, &hoverc)) {
					if (init_game) {//if game is being set up
						//keep number of hedgehogs placed even across all squares
						if (canPlace(hoverr)) {
							board.squares[hoverr][0].stack.push_back(players[turn]);//add current players hedgehog to board on the row they selected
							signal();//continue game init loop (unblock)
							block_input = true;
						}
					}
					else if (is_building_board) {
						if (hoverc != 0)//dont allow obstacles to be placed on first column
							board.squares[hoverr][hoverc].obstacle_type = selected_obstacle;//set obstacle for square
					}
					else {//gameplay processing
						if (board.squares[hoverr][hoverc].stack.size()) {
							Player* player = board.squares[hoverr][hoverc].stack.back();							
							int destr, destc;
							if (!players[turn]->hasMoved && hoverc != board.columns - 1) {//player hasn't moved yet and piece hasn't finished -> move up/down
								if (canMoveDown(hoverr, hoverc) || canMoveUp(hoverr, hoverc)) {
									if (board.squares[hoverr][hoverc].stack.back()->id == turn || (varients & VARIENT_ANARCHOHEDGEHOG)) {//is correct player, unless anarchohedgehog varient is on, then move anything up/down
										RECT rc = board.getHogRect(hoverr, hoverc);
										RECT tophalf = {rc.left, rc.top, rc.right, (rc.bottom - rc.top) / 2 + rc.top};
										RECT bottomhalf = {rc.left, (rc.bottom - rc.top) / 2 + rc.top, rc.right, rc.bottom};
										if (g_hedgehog_stack_width >= 50) {//if stack is too big, move up down arrows to center of square instead of center of hedgehog
											tophalf.left = bottomhalf.left = board.squares[hoverr][hoverc].rc.left;
										}

										destc = hoverc;//moving up/down so always in same column
										if (PtInRect(&tophalf, p) && (hoverr != 0 || varients & VARIENT_TUBE)) {//clicked top half so move up											
											if (hoverr == 0 && varients & VARIENT_TUBE) //enable ability to move up, and wrap around to bottom
												destr = board.rows - 1;//loop to bottom									
											else 
												destr = hoverr - 1;											
										}
										else if (PtInRect(&bottomhalf, p) && (hoverr != board.rows - 1 || varients & VARIENT_TUBE)) {//clicked bottom half so move down
											if (hoverr == board.rows - 1 && varients & VARIENT_TUBE) //enable ability to move down, and wrap around to top
												destr = 0;//loop to top								
											else 
												destr = hoverr + 1;
										}
										else
											break;//dont allow game state to continue until player makes a valid decision

										//continue game
										players[turn]->hasMoved = true;
										move(player, hoverr, hoverc, destr, destc);//move up/down
										signal();
										block_input = true;
									}
								}
							}
							else if (roll - 1 == hoverr) {//is row the rolled row? yes? -> move right
								if (canMoveForward(hoverr, hoverc)) {
									destr = hoverr;
									destc = hoverc + 1;
									move(player, hoverr, hoverc, destr, destc);//move forward
									
									signal();//continue game loop
									block_input = true;
								}
							}
						}						
					}
				}
			}
			break;
		}
		case WM_RBUTTONDOWN:{
			POINT p = getclientcursorpos(hwnd);

			if (!block_input && !moving) {//skip move up/down step
				if (!init_game && !is_building_board) {
					if (!players[turn]->hasMoved) {
						players[turn]->hasMoved = true;
						signal();
						block_input = true;
					}
				}
				else if (is_building_board) {//set hovered square to normal when building
					int hoverr, hoverc;
					if (board.hittest(p, &hoverr, &hoverc)) {
						board.squares[hoverr][hoverc].obstacle_type = 0;
					}
				}
			}

			break;
		}
		case WM_SIZE:{
			RECT prc = getmappedclientrect(GetParent(hwnd));
			int pw = prc.right - prc.left;
			int ph = prc.bottom - prc.top;

			double window_display_ratio = 2.0 / 7.0;

			int owh = static_cast<int>(window_display_ratio * ph);//output window height
			int lwh = static_cast<int>((1 - window_display_ratio) * ph);//legend window height

			int oww = static_cast<int>(window_display_ratio * pw);//output window width
			int gww = static_cast<int>((1 - window_display_ratio) * pw);//game window width

			SetWindowPos(GetDlgItem(GetParent(hwnd), IDC_OUTPUT), NULL, 0, 0, oww, owh, NULL);//change output window size
			SetWindowPos(GetDlgItem(GetParent(hwnd), IDC_LEGEND), NULL, 0, owh, oww, lwh, NULL);//legend pos
			
			SetWindowPos(hwnd, NULL, oww, 0, gww, ph, NULL);//change game window size

			//set rect for all squares on board when window size is changed
			RECT rc = getclientrect(hwnd);
			LONG h = rc.bottom / board.rows;
			LONG w = rc.right / board.columns;
			for (LONG r = 0; r < board.squares.size(); r++) {
				for (LONG c = 0; c < board.squares[r].size(); c++) {
					board.squares[r][c].rc = {c * w, r * h, (c + 1) * w, (r + 1) * h};
				}
			}

			break;
		}
		case WM_SEND_MOVE:{
			int fr = HIWORD(wParam);
			int fc = LOWORD(wParam);
			int tr = HIWORD(lParam);
			int tc = LOWORD(lParam);
		
			if (board.squares[fr][fc].stack.size())
				move(board.squares[fr][fc].stack.back(), fr, fc, tr, tc);
			
			while (!pause && !init_game) this_thread::sleep_for(chrono::milliseconds(10));
			signal();

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK Game::LegendProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	struct Legend {
		RECT rc;
		int data;
		COLORREF color;
	};
	static vector<Legend> keyrects;
	static int xscroll = 0;
	static vector<pair<string, RECT>> btns;

	switch (message) {
		case WM_PAINT:{
			RECT rcclient = getclientrect(hwnd);

			HDC hDC = GetDC(hwnd);
			
			HDC hDCmem = CreateCompatibleDC(hDC);//double buffer

			HBITMAP hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			HBITMAP hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			/////////////////////////////////////////////////////////////////////

			//paint background
			HBRUSH background = CreateSolidBrush(RGB(25, 25, 25));
			FillRect(hDCmem, &rcclient, background);
			DeleteObject(background);

			if (is_building_board) {
				//paint legend rects
				for (UINT i = 0; i < keyrects.size(); i++) {
					//color background color
					HBRUSH brush = CreateSolidBrush(keyrects[i].color);
					FillRect(hDCmem, &keyrects[i].rc, brush);
					DeleteObject(brush);

					//color frames (highlight active)
					COLORREF frame = 0;
					if (keyrects[i].data == selected_obstacle) {
						frame = RGB(200, 0, 0);
						//draw obstacle type info when selected
						RECT rcinfo = {rcclient.left, rcclient.bottom * 2 / 3, rcclient.right, rcclient.bottom - (rcclient.bottom / 10)};

						string info = "";
						switch (selected_obstacle) {
							case OBS_NONE:{
								info = "Regular Square.";
								break;
							}
							case OBS_PIT:{
								info = "Deep Pit: Hedgehogs that fall into a deep pit cannot leave until all other hedgehogs have caught up in every row";
								break;
							}
							case OBS_BLACK_HOLE:{
								info = "Black Hole: Hedgehogs that fall into a black hole are removed from the game forever";
								break;
							}
							case OBS_SHALLOW_PIT:{
								info = "Shallow Pit: Only the first hedgehog to fall into the pit is stuck. It can leave when all other hedgehogs have caught up in every row";
								break;
							}
							case OBS_TRAMPOLINE:{
								info = "Trampoline: hedgehogs bounce off the trampoline moving forward one square";
								break;
							}
						}
						
						SetTextColor(hDCmem, RGB(255, 255, 255));
						SetBkColor(hDCmem, RGB(25, 25, 25));
						DrawText(hDCmem, STW(info), info.size(), &rcinfo, DT_NOCLIP | DT_WORDBREAK);
					}
					brush = CreateSolidBrush(frame);
					FrameRect(hDCmem, &keyrects[i].rc, brush);
					DeleteObject(brush);
				}				

				for (UINT i = 0; i < btns.size(); i++) {//draw ok and save buttons
					SetTextColor(hDCmem, RGB(255, 255, 0));
					SetBkColor(hDCmem, RGB(25, 25, 25));
					DrawText(hDCmem, STW(btns[i].first), btns[i].first.size(), &btns[i].second, DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					HBRUSH frame = CreateSolidBrush(RGB(255, 255, 0));
					FrameRect(hDCmem, &btns[i].second, frame);
					DeleteObject(frame);
				}
			}
			else {//display player list with scores and total hedgehogs remaining
				TEXTMETRIC tm = {0};
				GetTextMetrics(hDCmem, &tm);
				int h = tm.tmHeight;

				static int prevturn;
				
				for (UINT i = 0; i < players.size(); i++) {					
					string s = "Player " + int_to_str(players[i]->id + 1) + "\'s score: " + int_to_str(players[i]->numFinished) + "/" + int_to_str(numWin) + " (" + int_to_str(players[i]->piece_count) + ")";
					RECT rc = {0, i * h - xscroll, rcclient.right, (i + 1) * h - xscroll};
					SetTextColor(hDCmem, players[i]->color);
					SetBkColor(hDCmem, RGB(25, 25, 25));

					bool in_ybound = (rc.bottom >= 0 && rc.top <= rcclient.bottom);

					if (in_ybound) {//only draw text in view
						DrawText(hDCmem, STW(s), s.size(), &rc, DT_NOCLIP | DT_VCENTER | DT_CENTER);
					}
					
					if (turn == players[i]->id) {//draw frame around current player
						//move player list window to show current turn (only once and when player turn changes)
						if (prevturn != turn && !(rc.top >= 0 && rc.bottom <= rcclient.bottom)) {
							int mn, mx;							
							GetScrollRange(hwnd, SB_VERT, &mn, &mx);
							xscroll = turn * h > mx ? mx : turn * h;
							SetScrollPos(hwnd, SB_VERT, xscroll, TRUE);
						}
					
						HBRUSH brush = CreateSolidBrush(RGB(200, 0, 0));
						FrameRect(hDCmem, &rc, brush);
						DeleteObject(brush);
					}
				}
				prevturn = turn;
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

			POINT p = getclientcursorpos(hwnd);

			if (is_building_board) {
				//process obstacle btn press
				for (UINT i = 0; i < keyrects.size(); i++) {
					if (PtInRect(&keyrects[i].rc, p)) {
						selected_obstacle = keyrects[i].data;
						break;
					}
				}
				//process button press
				for (UINT i = 0; i < btns.size(); i++) {
					if (PtInRect(&btns[i].second, p)) {						
						if (btns[i].first == "Ok") {//ok button pressed
							EndDialog(GetParent(hwnd), 0);
						}
						else if (btns[i].first == "Save") {//save button pressed
							string path = browseforfile(g_hwnd, false, L"Save Board", L"Board\0*.board*\0");
							if (!path.size()) break;
							auto slash = path.rfind("\\");
							auto dot = path.rfind(".");
							string dir = path.substr(0, slash + 1);
							string filename = path.substr(slash + 1, (path.size() - slash) - (path.size() - dot) - 1);
							string filetype = path.substr(dot, string::npos);
							path = dir + filename +	filetype;
							ofstream dest(path, ios::binary);//open full path for write

							//write the board in grid form columns delimited by spaces, rows by newlines
							for (UINT i = 0; i < board.squares.size(); i++) {
								for (UINT j = 0; j < board.squares[i].size(); j++) {
									dest << board.squares[i][j].obstacle_type << " ";
								}
								dest << "\r\n";
							}

							dest.close();
						}
					}
				}
			}

			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

			break;
		}
		case WM_RBUTTONDBLCLK:
		case WM_MOUSEWHEEL:{
			SetFocus(hwnd);
			
			//handles scrolling through player list
			int min, max;
			GetScrollRange(hwnd, SB_VERT, &min, &max);

			HDC hDC = GetDC(hwnd);
			TEXTMETRIC tm = {0};
			GetTextMetrics(hDC, &tm);
			int h = tm.tmHeight;
			ReleaseDC(hwnd, hDC);

			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {//up 
				xscroll = xscroll - h < min ? min : xscroll - h;
			}
			else//down
				xscroll = xscroll + h > max ? max : xscroll + h;

			SetScrollPos(hwnd, SB_VERT, xscroll, TRUE);

			break;
		}
		case WM_VSCROLL:{
			switch (LOWORD(wParam)) {
				case SB_THUMBTRACK:{//playerlist thumb is being dragged, so update window
					xscroll = HIWORD(wParam);
					SetScrollPos(hwnd, SB_VERT, xscroll, TRUE);
					
					break;
				}
			}

			break;
		}
		case WM_SIZE:{
			RECT rcclient = getclientrect(hwnd);
			int bottom = rcclient.bottom * 2 / 3;

			keyrects.clear();
			btns.clear();
			
			//get rects for obstacle buttons and ok/save buttons, based on window size
			if (is_building_board) {
				int i = 0;
				for (auto a : g_obstacles) {
					Legend l;
					l.color = a.second;
					l.data = a.first;
				
					int cx = rcclient.right / 2;
					int h = (int)ceil((double)g_obstacles.size() / 2.0);
					RECT rc = {i % 2 == 0 ? 0 : cx,
						(i / 2) * (bottom / h),
						i % 2 == 0 ? cx : 2 * cx,
						(i / 2) * (bottom / h) + (bottom / h)
					};
					l.rc = rc;

					keyrects.push_back(l);

					i++;
				}

				//add buttons
				RECT rccontrol = {rcclient.left, rcclient.bottom - (rcclient.bottom / 10), rcclient.right, rcclient.bottom};
				RECT rcbtnok = rccontrol;
				RECT rcbtnsave = rccontrol;
				rcbtnok.right /= 2;
				rcbtnsave.left = rccontrol.right / 2;
				btns.push_back(make_pair("Ok", rcbtnok));
				btns.push_back(make_pair("Save", rcbtnsave));
			}
			else {
				//resize scrollbar
				HDC hDC = GetDC(hwnd);
				TEXTMETRIC tm = {0};
				GetTextMetrics(hDC, &tm);
				int h = tm.tmHeight;
				ReleaseDC(hwnd, hDC);

				int max = 0;
				if (rcclient.bottom <= numPlayers * h)
					max = numPlayers * h - rcclient.bottom;;
				//windowheight*nMax)/graphic height
				SCROLLINFO si;
				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_RANGE;// | SIF_PAGE;
				si.nMin = 0;
				si.nMax = max;
			//	si.nPage = rcclient.bottom / h;
				SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			}
			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

