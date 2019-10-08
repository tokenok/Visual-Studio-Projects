using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TicTacToe {
	enum GameStates{
		Normal = 0,
		Player1Wins = 1,
		Player2Wins = 2,
		Cat = 3,
		Restart = 4,
	};

	public class Board {		
		int[,] board;
		int turn = 1;
		int numWin;
		int moveCount = 0;

		public int AI = 0;

		public Board() {
			board = new int[3, 3];
			numWin = 3;
		}

		char toXO(int t) {
			return t == 1 ? 'X' : t == 2 ? 'O' : ' ';
		}

		int[] shuffle(int[] array) {
			Random r = new Random();
			for (int i = array.Length; i > 0; i--) {
				int j = r.Next(i);
				int k = array[j];
				array[j] = array[i - 1];
				array[i - 1] = k;
			}
			return array;
		}

		public void PrintBoard() {
			Console.Clear();

			for (int i = 0; i < board.GetLength(0); i++) {
				for (int j = 0; j < board.GetLength(1); j++) {
					Console.Write(toXO(board[i, j]));
					if (j != board.GetLength(1) - 1)
						Console.Write("|");
					else
						Console.Write("\n");
				}
				if (i != board.GetLength(0) - 1)
					for (int j = 0; j < board.GetLength(1) * 2 - 1; j++) 
						Console.Write("-");
				Console.Write("\n");
			}
			Console.Write("\n");
		}

		public int getGameSate() {
			int catcount = 0;
			for (int i = 0; i < board.GetLength(0); i++) {
				int count = 0;
				for (int j = 0; j < board.GetLength(1); j++) {
					if (board[i, j] == turn)
						count++;
					else
						count = 0;
					if (board[i, j] != 0)
						catcount++;
				}
				if (count == numWin)
					return turn;
			}
			if (catcount == board.GetLength(0) * board.GetLength(1))
				return (int)GameStates.Cat;

			for (int i = 0; i < board.GetLength(1); i++) {
				int count = 0;
				for (int j = 0; j < board.GetLength(0); j++) {
					if (board[j, i] == turn)
						count++;
					else 
						count = 0;
				}
				if (count == numWin)
					return turn;
			}
			if ((board[0, 0] == turn && board[1, 1] == turn && board[2, 2] == turn)
				|| (board[0, 2] == turn && board[1, 1] == turn && board[2, 0] == turn))
				return turn;

			return (int)GameStates.Normal;
		}

		int boardCordToNum(int y, int x) {
			if (y == 0 && x == 0) return 7;
			if (y == 0 && x == 1) return 8;
			if (y == 0 && x == 2) return 9;
			if (y == 1 && x == 0) return 4;
			if (y == 1 && x == 1) return 5;
			if (y == 1 && x == 2) return 6;
			if (y == 2 && x == 0) return 1;
			if (y == 2 && x == 1) return 2;
			if (y == 2 && x == 2) return 3;
			return 0;
		}

		int AIcheckwin() {
			for (int i = 0; i < board.GetLength(0); i++) {
				int ucount = 0;
				int ecount = 0;
				for (int j = 0; j < board.GetLength(1); j++) {
					if (board[i, j] == turn)
						ucount++;
					else if (board[i, j] != 0)
						ecount++;
				}
				if (ucount == numWin - 1 && ecount == 0) {
					for (int j = 0; j < board.GetLength(1); j++) {
						if (board[i, j] == 0)
							return boardCordToNum(i, j);
					}
				}
			}

			for (int i = 0; i < board.GetLength(1); i++) {
				int ucount = 0;
				int ecount = 0;
				for (int j = 0; j < board.GetLength(0); j++) {
					if (board[j, i] == turn)
						ucount++;
					else if (board[j, i] != 0)
						ecount++;
				}
				if (ucount == numWin - 1 && ecount == 0) {
					for (int j = 0; j < board.GetLength(0); j++) {
						if (board[j, i] == 0)
							return boardCordToNum(j, i);
					}
				}
			}

			int eturn = turn == 1 ? 2 : 1;
			int[] counts1 = new int[3];
			counts1[board[0, 0]]++;
			counts1[board[1, 1]]++;
			counts1[board[2, 2]]++;
			if (counts1[turn] == numWin - 1 && counts1[eturn] == 0) {
				if (board[0, 0] == 0)
					return boardCordToNum(0, 0);
				else if (board[1, 1] == 0)
					return boardCordToNum(1, 1);
				else if (board[2, 2] == 0)
					return boardCordToNum(2, 2);
			}

			int[] counts2 = new int[3];
			counts2[board[0, 2]]++;
			counts2[board[1, 1]]++;
			counts2[board[2, 0]]++;
			if (counts2[turn] == numWin - 1 && counts2[eturn] == 0) {
				if (board[0, 2] == 0)
					return boardCordToNum(0, 2);
				else if (board[1, 1] == 0)
					return boardCordToNum(1, 1);
				else if (board[2, 0] == 0)
					return boardCordToNum(2, 0);
			}

			return 0;
		}
		int AIcheckblock() {
			for (int i = 0; i < board.GetLength(0); i++) {
				int ucount = 0;
				int ecount = 0;
				for (int j = 0; j < board.GetLength(1); j++) {
					if (board[i, j] == turn)
						ucount++;
					else if (board[i, j] != 0)
						ecount++;
				}
				if (ecount == numWin - 1 && ucount == 0) {
					for (int j = 0; j < board.GetLength(1); j++) {
						if (board[i, j] == 0)
							return boardCordToNum(i, j);
					}
				}
			}

			for (int i = 0; i < board.GetLength(1); i++) {
				int ucount = 0;
				int ecount = 0;
				for (int j = 0; j < board.GetLength(0); j++) {
					if (board[j, i] == turn)
						ucount++;
					else if (board[j, i] != 0)
						ecount++;
				}
				if (ecount == numWin - 1 && ucount == 0) {
					for (int j = 0; j < board.GetLength(0); j++) {
						if (board[j, i] == 0)
							return boardCordToNum(j, i);
					}
				}
			}

			int eturn = turn == 1 ? 2 : 1;
			int[] counts1 = new int[3];
			counts1[board[0, 0]]++;
			counts1[board[1, 1]]++;
			counts1[board[2, 2]]++;
			if (counts1[eturn] == numWin - 1 && counts1[turn] == 0) {
				if (board[0, 0] == 0)
					return boardCordToNum(0, 0);
				else if (board[1, 1] == 0)
					return boardCordToNum(1, 1);
				else if (board[2, 2] == 0)
					return boardCordToNum(2, 2);
			}

			int[] counts2 = new int[3];
			counts2[board[0, 2]]++;
			counts2[board[1, 1]]++;
			counts2[board[2, 0]]++;
			if (counts2[eturn] == numWin - 1 && counts2[turn] == 0) {
				if (board[0, 2] == 0)
					return boardCordToNum(0, 2);
				else if (board[1, 1] == 0)
					return boardCordToNum(1, 1);
				else if (board[2, 0] == 0)
					return boardCordToNum(2, 0);
			}

			return 0;
		}
		int AIfindfork() {
			int[][,] r = new int[][,] {
				new int[,] {{0,0},{0,1},{0,2},{1,1},{2,2},{1,0},{2,0}},//7
				new int[,] {{2,0},{1,0},{0,0},{1,1},{0,2},{2,1},{2,2}},//9
				new int[,] {{0,2},{0,0},{0,1},{1,1},{2,0},{1,2},{2,2}},//1
				new int[,] {{2,2},{2,1},{2,0},{1,1},{0,0},{1,2},{0,2}},//3
				new int[,] {{1,1},{0,0},{2,2},{0,2},{2,0},{1,0},{1,2},{0,1},{2,1}},//5
				new int[,] {{0,1},{0,0},{0,2},{1,1},{2,1}},//8
				new int[,] {{1,0},{1,1},{1,2},{0,0},{2,0}},//4
				new int[,] {{1,2},{0,2},{2,2},{1,0},{1,1}},//6
				new int[,] {{2,1},{1,1},{0,1},{2,0},{2,2}},//2
			};

			int eturn = turn == 2 ? 1 : 2;
			for (int i = 0; i < r.Length; i++) {
				if (board[r[i][0, 0], r[i][0, 1]] != 0)
					continue;
				int[,] a = new int[(r[i].GetLength(0) - 1) / 2, 2];//[x, 0] = player, [x, 1] = opponent
				for (int j = 1; j < r[i].GetLength(0); j += 2) {
					for (int k = 0; k < r[i].GetLength(1); k++) {
						if (board[r[i][j + k, 0], r[i][j + k, 1]] == turn)
							a[(j - 1) / 2, 0]++;
						if (board[r[i][j + k, 0], r[i][j + k, 1]] == eturn)
							a[(j - 1) / 2, 1]++;
					}
				}
				int fork = 0;
				for (int j = 0; j < a.GetLength(0); j++) {
					if (a[j, 1] == 0 && a[j, 0] == 1)
						fork++;
				}
				if (fork == 2) {
					Console.WriteLine("Fork found at: (" + r[i][0, 0] + ", " + r[i][0, 1] + ")");
					return boardCordToNum(r[i][0, 0], r[i][0, 1]);
				}
			}
			
			return 0;
		}

		int playCorner() {
			int[,] corners = new int[,] { { 0, 0, 7 }, { 0, 2, 9 }, { 2, 0, 1 }, { 2, 2, 3 } };
			int[] s = new int[] { 0, 1, 2, 3 };
			s = shuffle(s);
			for (int i = 0; i < corners.GetLength(0); i++) {
				if (board[corners[s[i], 0], corners[s[i], 1]] == 0) 
					return corners[s[i], 2];
			}
			return 0;
		}
		int playEdge() {
			int[,] edges = new int[,] { { 0, 1, 8 }, { 1, 0, 4 }, { 1, 2, 6 }, { 2, 1, 2 } };
			int[] s = new int[] { 0, 1, 2, 3 };
			s = shuffle(s);
			for (int i = 0; i < edges.GetLength(0); i++) {
				if (board[edges[s[i], 0], edges[s[i], 1]] == 0) 
					return edges[s[i], 2];
			}
			return 0;
		}

		int AImove() {
			if (moveCount == 0) {
				int[] c = new int[] { 7, 9, 1, 3 };
				c = shuffle(c);
				return c[0];
			}
			else if (moveCount == 1) {
				//check edge
				int eturn = turn == 1 ? 2 : 1;
				if (board[0, 1] == eturn)//8
					return 9;
				if (board[1, 0] == eturn)//4
					return 7;
				if (board[1, 2] == eturn)//6
					return 3;
				if (board[2, 1] == eturn)//2
					return 1;
				//check center
				if (board[1, 1] == eturn) {
					return playCorner();
				}
			}
			if (moveCount == 2) {
				int eturn = turn == 1 ? 2 : 1;

				//check center
				if (board[1, 1] == eturn) {
					//play opposite corner
					Console.WriteLine("2nd move: play opposite corner");
					if (board[0, 0] == turn) return 3;
					if (board[0, 2] == turn) return 1;
					if (board[2, 0] == turn) return 9;
					if (board[2, 2] == turn) return 7;
				}
				else {
					//check corners
					if (board[0, 0] == eturn || board[0, 2] == eturn || board[2, 0] == eturn || board[2, 2] == eturn) {
						return playCorner();
					}
					//edge was played by opponent so play center
					return 5;
				}
			}
			else if (moveCount == 3) {
				int eturn = turn == 1 ? 2 : 1;
				if (board[0, 0] == eturn && board[1, 1] == turn && board[2, 2] == eturn) {
					return playEdge();
				}
				if (board[0, 2] == eturn && board[1, 1] == turn && board[2, 0] == eturn) {
					return playEdge();
				}

				int cornercount = (isValidMove(7) ? 1 : 0) + (isValidMove(9) ? 1 : 0) + (isValidMove(1) ? 1 : 0) + (isValidMove(3) ? 1 : 0);
				if (cornercount == 3 && !isValidMove(5)) {
					List<int> corners = new List<int> { 1, 3, 7, 9 };
					for (int i = 0; i < corners.Count; i++) {
						if (isValidMove(corners[i])) {
							doMove(corners[i]);
							int fork = AIfindfork();
							undoMove(corners[i]);
							if (fork == 0)
								return corners[i];
						}
					}
				}
			}

			int move = 0;
			move = AIcheckwin();
			if (move > 0) {
				Console.WriteLine("Win");
				return move;
			}
			move = AIcheckblock();
			if (move > 0) {
				Console.WriteLine("Block win");
				return move;
			}

			move = AIfindfork();
			if (move > 0) return move;
			
			//play center
			if (board[1, 1] == 0) {
				Console.WriteLine("play center");
				return 5;
			}

			//play corner
			move = playCorner();
			if (move > 0) {
				Console.WriteLine("play corner");
				return move;
			}

			//play edge (should never be called but idk)
			move = playEdge();
			if (move > 0) {
				Console.WriteLine("play edge");
				return move;
			}

			return 0;
		}

		public int getMove(){
			if (Convert.ToBoolean(AI & turn)) {
				return AImove();
			}

			Console.WriteLine("Player " + turn.ToString() + " Enter your move. [1-9]");
			ConsoleKeyInfo input = Console.ReadKey(true);
			while (!char.IsDigit(input.KeyChar)) {
				if (input.Key == ConsoleKey.R) {
					reset();
					return 0;
				}
				Console.WriteLine("Invalid input. Enter a number 1-9");
				input = Console.ReadKey(true);
			}
			return int.Parse(input.KeyChar.ToString());
		}

		public bool isValidMove(int move) {
			switch (move){
				case 7: return board[0, 0] == 0;
				case 8: return board[0, 1] == 0;
				case 9: return board[0, 2] == 0;
				case 4: return board[1, 0] == 0;
				case 5: return board[1, 1] == 0;
				case 6: return board[1, 2] == 0;
				case 1: return board[2, 0] == 0;
				case 2: return board[2, 1] == 0;
				case 3: return board[2, 2] == 0;
				default: return false;
			}
		}

		public void doMove(int move) {
			moveCount++;
			switch (move) {
				case 7: board[0, 0] = turn;	break;
				case 8: board[0, 1] = turn;	break;
				case 9: board[0, 2] = turn;	break;
				case 4: board[1, 0] = turn;	break;
				case 5: board[1, 1] = turn;	break;
				case 6: board[1, 2] = turn;	break;
				case 1: board[2, 0] = turn;	break;
				case 2: board[2, 1] = turn;	break;
				case 3: board[2, 2] = turn; break;
			}			
		}

		public void undoMove(int move) {
			moveCount--;
			switch (move) {
				case 7: board[0, 0] = 0; break;
				case 8: board[0, 1] = 0; break;
				case 9: board[0, 2] = 0; break;
				case 4: board[1, 0] = 0; break;
				case 5: board[1, 1] = 0; break;
				case 6: board[1, 2] = 0; break;
				case 1: board[2, 0] = 0; break;
				case 2: board[2, 1] = 0; break;
				case 3: board[2, 2] = 0; break;
			}
		}

		public void nextTurn() {
			turn = turn == 2 ? 1 : 2;
		}

		public bool playAgain(int gamestate) {
			if (gamestate == (int)GameStates.Cat)
				Console.WriteLine("Cat Game, no player can win!\nWould you like to play again? (y/n)");
			else 
				Console.WriteLine("Player " + turn + ((Convert.ToBoolean(AI & turn)) ? "(AI)" : "") + " wins!\n\nWould you like to play again? (y/n)");

			ConsoleKeyInfo input = Console.ReadKey(true);
			while (input.Key != ConsoleKey.Y && input.Key != ConsoleKey.N) {
				input = Console.ReadKey(true);
			}
			return input.Key == ConsoleKey.Y;
		}

		public void reset() {
			for (int i = 0; i < board.GetLength(0); i++) {
				for (int j = 0; j < board.GetLength(1); j++) {
					board[i, j] = 0;
				}
			}
			turn = 1;
			moveCount = 0;
			PrintBoard();
		}
	}

	public class MenuItem {
		public string text;
		public int val, min, max;
		public bool isbool = false;

		public void Print() {
			ConsoleColor temp = Console.ForegroundColor;
			Console.ForegroundColor = ConsoleColor.White;
			Console.Write(text);
			Console.ForegroundColor = ConsoleColor.Red;
			if (isbool) {
				if (val == 1)
					Console.ForegroundColor = ConsoleColor.Green;
				Console.Write(Convert.ToBoolean(val));
			}
			else
				Console.Write(val);
			Console.Write("                            \r");
			Console.ForegroundColor = temp;
		}
	}

	class Program {
		static void Main(string[] args) {
			start:
			Console.ForegroundColor = ConsoleColor.White;
			Console.WriteLine("Left/Right Arrow to change player\nUp/Down Arrow to change AI state\nPress Enter to play\n\n");

			MenuItem[] MainMenu = new MenuItem[] { 
				new MenuItem { text = "Is Player 1 ai? ", val = 0, min = 0, max = 1, isbool = true},
				new MenuItem { text = "Is Player 2 ai? ", val = 0, min = 0, max = 1, isbool = true}	
			};

			int selection = 0;

			MainMenu[selection].Print();

			ConsoleKeyInfo input;
			do {
				input = Console.ReadKey(true);

				switch (input.Key) {
					case ConsoleKey.UpArrow: {
							MainMenu[selection].val++;

							if (MainMenu[selection].val > MainMenu[selection].max) {
								MainMenu[selection].val = MainMenu[selection].min;
							}

							MainMenu[selection].Print();

							break;
						}
					case ConsoleKey.DownArrow: {
							MainMenu[selection].val--;

							if (MainMenu[selection].val < MainMenu[selection].min) {
								MainMenu[selection].val = MainMenu[selection].max;
							}

							MainMenu[selection].Print();

							break;
						}
					case ConsoleKey.RightArrow: {
							selection++;
							if (selection > MainMenu.Length - 1) {
								selection = 0;
							}

							MainMenu[selection].Print();

							break;
						}
					case ConsoleKey.LeftArrow: {
							selection--;
							if (selection < 0)
								selection = MainMenu.Length - 1;

							MainMenu[selection].Print();

							break;
						}
				}
			} while (input.Key != ConsoleKey.Enter);
			Console.ResetColor();

			Board b = new Board();
			b.AI = (MainMenu[0].val == 1 ? 1 : 0) + (MainMenu[1].val == 1 ? 2 : 0); 
			b.PrintBoard();
			
			while (true) {
				int move = b.getMove();
				if (b.isValidMove(move)) {
					b.doMove(move);
					b.PrintBoard();
					int gameState = b.getGameSate();
					if (gameState == (int)GameStates.Normal)
						b.nextTurn();
					else {
						if (b.playAgain((int)gameState))
							b.reset();
						else
							break;
					}
				}
				else 
					Console.WriteLine("Invalid move!");
			}
			Console.Clear();
			goto start;
		}
	}
}
