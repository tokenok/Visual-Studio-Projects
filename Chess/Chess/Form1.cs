using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Chess {
	public partial class Form1 : Form {
		public Point g_selectedPoint = new Point(-1, -1);
		public List<Point> g_selected_valid_moves = new List<Point>();
		public Tuple<Point, Point> g_last_move = new Tuple<Point, Point>(new Point(-1, -1), new Point(-1, -1));
		public IntPtr g_hwnd;
		public Board g_board;
		public PlayerColor g_current_player = PlayerColor.White;

		[DllImport("user32.dll")]
		static extern bool GetClientRect(IntPtr hwnd, out RECT lpRect);
		[DllImport("user32.dll")]
		static extern IntPtr GetActiveWindow();
		[DllImport("user32.dll")]
		static extern bool PtInRect(out RECT lprc, POINT pt);

		[StructLayout(LayoutKind.Sequential)]
		public struct RECT {
			public int left;
			public int top;
			public int right;
			public int bottom;
		}
		[StructLayout(LayoutKind.Sequential)]
		public struct POINT {
			public int x;
			public int y;

			public POINT(int x, int y) {
				this.x = x;
				this.y = y;
			}
		}

		public static RECT getclientrect(IntPtr wnd) {
			RECT ret;
			GetClientRect(wnd, out ret);
			return ret;
		}

		public Form1() {
			InitializeComponent();
		}

		public enum PlayerColor {
			None, White, Black
		}

		public class PieceData {
			public PieceType type;
			public PlayerColor color;

			public static bool testRange(Point pt) {
				return !(pt.X < 0 || pt.X > 7 || pt.Y < 0 || pt.Y > 7);
			}

			public void Move(Board b, Point pos, Point mv) {
				if (b.squares[mv.Y][mv.X].piece.color != this.color && b.squares[mv.Y][mv.X].piece.color != PlayerColor.None) {//capture
					Console.WriteLine("captured: " + b.squares[mv.Y][mv.X].piece.getPieceText());
				}
								
				b.squares[mv.Y][mv.X].setPiece(b.squares[pos.Y][pos.X].piece);
				b.squares[pos.Y][pos.X].setPiece(new Piece(PieceType.None, PlayerColor.None));
			}
		}
		public interface IPieceType {
			void Move(Board b, Point pos, Point mv);
			List<Point> getMoves(Board b, Point pos);
		}
		public class King : PieceData, IPieceType {
			public King(PlayerColor color) {
				this.color = color;
			}

			//public void Move(Board b, Point pos, Point mv) { 
				
			//}
			public List<Point> getMoves(Board b, Point pos) {
				List<Point> ret = new List<Point>();

				SquarePos sqpos = SquarePos.getSquarePosFromIndex(pos.Y, pos.X);
				Point testpt = new Point();

				List<Point> directions = new List<Point> { 
					new Point(1, 1), new Point(1, -1), new Point(-1, 1), new Point(-1, -1),
					new Point(0, 1), new Point(0, -1), new Point(-1, 0), new Point(1, 0) 
				};
				for (int d = 0; d < directions.Count; d++) {
					for (int i = 1; i <= 1; i++) {
						testpt = SquarePos.getPointFromSquarePos(sqpos.rank + (i * directions[d].X), (char)((int)sqpos.file + (i * directions[d].Y)));//
						if (!PieceData.testRange(testpt))
							break;
						if (b.squares[testpt.Y][testpt.X].piece.color != PlayerColor.None
							&& b.squares[testpt.Y][testpt.X].piece.color != this.color) {

							if (b.squares[testpt.Y][testpt.X].piece.type != PieceType.King)
								ret.Add(testpt);
							break;
						}
						if (b.squares[testpt.Y][testpt.X].piece.color == this.color)
							break;

						this.Move(b, pos, testpt);
						if (isKingInCheck(b, pos, this.color) == 0) {
							ret.Add(testpt);
						}
						this.Move(b, testpt, pos);
					}
				}

				return ret;
			}

			public static int isKingInCheck(Board b, PlayerColor kingcolor, ref List<Point> attackers) {
				for (int i = 0; i < b.squares.Count; i++) 
					for (int j = 0; j < b.squares[i].Count; j++) 
						if (b.squares[i][j].piece.type == PieceType.King && b.squares[i][j].piece.color == kingcolor) 
							return isKingInCheck(b, new Point(j, i), kingcolor, ref attackers);
				
				return 0;
			}

			public static int isKingInCheck(Board b, Point kingpos, PlayerColor kingcolor) {
				List<Point> pts = new List<Point>();
				return isKingInCheck(b, kingpos, kingcolor, ref pts);
			}

			public static int isKingInCheck(Board b, Point kingpos, PlayerColor kingcolor, ref List<Point> attackers) {
				PlayerColor enemycolor = kingcolor == PlayerColor.White ? PlayerColor.Black : PlayerColor.White;

				SquarePos sqKingPos = SquarePos.getSquarePosFromIndex(kingpos.Y, kingpos.X);
				Point testpt = new Point();

				int checkcount = 0;

				//diagonals
				List<Point> directions = new List<Point> { new Point(1, 1), new Point(1, -1), new Point(-1, 1), new Point(-1, -1) };
				for (int d = 0; d < directions.Count; d++) {
					for (int i = 1; ; i++) {
						testpt = SquarePos.getPointFromSquarePos(sqKingPos.rank + (i * directions[d].X), (char)((int)sqKingPos.file + (i * directions[d].Y)));//
						if (!PieceData.testRange(testpt))
							break;
						if (b.squares[testpt.Y][testpt.X].piece.color == enemycolor) {
							if (b.squares[testpt.Y][testpt.X].piece.type == PieceType.Bishop
								|| b.squares[testpt.Y][testpt.X].piece.type == PieceType.Queen) {
								checkcount++;
								attackers.Add(testpt);
							}
						}
						if (b.squares[testpt.Y][testpt.X].piece.color == kingcolor)
							break;
					}
				}

				// orthogonals
				directions = new List<Point> { new Point(0, 1), new Point(0, -1), new Point(1, 0), new Point(-1, 0) };
				for (int d = 0; d < directions.Count; d++) {
					for (int i = 1; ; i++) {
						testpt = SquarePos.getPointFromSquarePos(sqKingPos.rank + (i * directions[d].X), (char)((int)sqKingPos.file + (i * directions[d].Y)));//
						if (!PieceData.testRange(testpt))
							break;
						if (b.squares[testpt.Y][testpt.X].piece.color == enemycolor) {
							if (b.squares[testpt.Y][testpt.X].piece.type == PieceType.Rook
								|| b.squares[testpt.Y][testpt.X].piece.type == PieceType.Queen) {
								checkcount++;
								attackers.Add(testpt);
							}
						}
						if (b.squares[testpt.Y][testpt.X].piece.color == kingcolor)
							break;
					}
				}

				//knights
				directions = new List<Point> { new Point(2, 1), new Point(2, -1), new Point(-2, 1), new Point(-2, -1), new Point(1, 2), new Point(1, -2), new Point(-1, 2), new Point(-1, -2) };
				for (int d = 0; d < directions.Count; d++) {
					testpt = SquarePos.getPointFromSquarePos(sqKingPos.rank + directions[d].X, (char)((int)sqKingPos.file + directions[d].Y));//
					if (!PieceData.testRange(testpt))
						continue;
					if (b.squares[testpt.Y][testpt.X].piece.color == enemycolor) {
						if (b.squares[testpt.Y][testpt.X].piece.type == PieceType.Knight) {
							checkcount++;
							attackers.Add(testpt);
						}
					}
				}

				//pawns
				if (kingcolor == PlayerColor.White)
					directions = new List<Point> { new Point(1, 1), new Point(1, -1) };
				else
					directions = new List<Point> { new Point(-1, 1), new Point(-1, -1) };
				for (int d = 0; d < directions.Count; d++) {
					testpt = SquarePos.getPointFromSquarePos(sqKingPos.rank + directions[d].X, (char)((int)sqKingPos.file + directions[d].Y));//
					if (!PieceData.testRange(testpt))
						continue;
					if (b.squares[testpt.Y][testpt.X].piece.color == enemycolor) {
						if (b.squares[testpt.Y][testpt.X].piece.type == PieceType.Pawn) {
							checkcount++;
							attackers.Add(testpt);
						}
					}
				}

				return checkcount;
			}
		}
		public class Queen : PieceData, IPieceType {
			public Queen(PlayerColor color) {
				this.color = color;
			}
			public List<Point> getMoves(Board b, Point pos) {
				List<Point> ret = new List<Point>();

				SquarePos sqpos = SquarePos.getSquarePosFromIndex(pos.Y, pos.X);
				Point testpt = new Point();

				List<Point> directions = new List<Point> { 
					new Point(1, 1), new Point(1, -1), new Point(-1, 1), new Point(-1, -1),
					new Point(0, 1), new Point(0, -1), new Point(-1, 0), new Point(1, 0) 
				};
				for (int d = 0; d < directions.Count; d++) {
					for (int i = 1; ; i++) {
						testpt = SquarePos.getPointFromSquarePos(sqpos.rank + (i * directions[d].X), (char)((int)sqpos.file + (i * directions[d].Y)));//
						if (!PieceData.testRange(testpt))
							break;
						if (b.squares[testpt.Y][testpt.X].piece.color != PlayerColor.None
							&& b.squares[testpt.Y][testpt.X].piece.color != this.color) {

							if (b.squares[testpt.Y][testpt.X].piece.type != PieceType.King)
								ret.Add(testpt);
							break;
						}
						if (b.squares[testpt.Y][testpt.X].piece.color == this.color)
							break;

						ret.Add(testpt);
					}
				}

				return ret;
			}
		}
		public class Rook : PieceData, IPieceType {
			public Rook(PlayerColor color) {
				this.color = color;
			}

			public List<Point> getMoves(Board b, Point pos) {
				List<Point> ret = new List<Point>();

				SquarePos sqpos = SquarePos.getSquarePosFromIndex(pos.Y, pos.X);
				Point testpt = new Point();

				List<Point> directions = new List<Point> { new Point(0, 1), new Point(0, -1), new Point(-1, 0), new Point(1, 0) };
				for (int d = 0; d < directions.Count; d++) {
					for (int i = 1; ; i++) {
						testpt = SquarePos.getPointFromSquarePos(sqpos.rank + (i * directions[d].X), (char)((int)sqpos.file + (i * directions[d].Y)));//
						if (!PieceData.testRange(testpt))
							break;
						if (b.squares[testpt.Y][testpt.X].piece.color != PlayerColor.None
							&& b.squares[testpt.Y][testpt.X].piece.color != this.color) {

							if (b.squares[testpt.Y][testpt.X].piece.type != PieceType.King)
								ret.Add(testpt);
							break;
						}
						if (b.squares[testpt.Y][testpt.X].piece.color == this.color)
							break;

						ret.Add(testpt);
					}
				}

				return ret;
			}
		}
		public class Bishop : PieceData, IPieceType {
			public Bishop(PlayerColor color) {
				this.color = color;
			}

			public List<Point> getMoves(Board b, Point pos) {
				List<Point> ret = new List<Point>();

				SquarePos sqpos = SquarePos.getSquarePosFromIndex(pos.Y, pos.X);
				Point testpt = new Point();

				List<Point> directions = new List<Point>{new Point(1,1),new Point(1,-1),new Point(-1,1),new Point(-1,-1)};
				for (int d = 0; d < directions.Count; d++) {
					for (int i = 1; ; i++) {
						testpt = SquarePos.getPointFromSquarePos(sqpos.rank + (i * directions[d].X), (char)((int)sqpos.file + (i * directions[d].Y)));//
						if (!PieceData.testRange(testpt))
							break;
						if (b.squares[testpt.Y][testpt.X].piece.color != PlayerColor.None
							&& b.squares[testpt.Y][testpt.X].piece.color != this.color) {

							if (b.squares[testpt.Y][testpt.X].piece.type != PieceType.King)
								ret.Add(testpt);
							break;
						}
						if (b.squares[testpt.Y][testpt.X].piece.color == this.color)
							break;

						ret.Add(testpt);
					}
				}

				return ret;
			}
		}
		public class Knight : PieceData, IPieceType {
			public Knight(PlayerColor color) {
				this.color = color;
			}

			public List<Point> getMoves(Board b, Point pos) {
				List<Point> ret = new List<Point>();

				SquarePos sqpos = SquarePos.getSquarePosFromIndex(pos.Y, pos.X);
				Point testpt = new Point();

				List<Point> directions = new List<Point> { new Point(2, 1), new Point(2, -1), new Point(-2, 1), new Point(-2, -1), new Point(1, 2), new Point(1, -2), new Point(-1, 2), new Point(-1, -2) };
				for (int d = 0; d < directions.Count; d++) {
					testpt = SquarePos.getPointFromSquarePos(sqpos.rank + directions[d].X, (char)((int)sqpos.file + directions[d].Y));//
					if (PieceData.testRange(testpt) && b.squares[testpt.Y][testpt.X].piece.color != this.color
						&& b.squares[testpt.Y][testpt.X].piece.type != PieceType.King)
						ret.Add(testpt);
				}

				//TODO check if own king would be checked

				return ret;
			}
		}
		public class Pawn : PieceData, IPieceType {
			public Pawn(PlayerColor color) {
				this.color = color;
			}

			public void Move(Board b, Point pos, Point mv) {
				if (b.squares[mv.Y][mv.X].piece.color != this.color && b.squares[mv.Y][mv.X].piece.color != PlayerColor.None) {//capture
					Console.WriteLine("captured: " + b.squares[mv.Y][mv.X].piece.getPieceText());
				}

				if ((this.color == PlayerColor.White && mv.Y == 0) || (this.color == PlayerColor.Black && mv.Y == 7))
					b.squares[mv.Y][mv.X].setPiece(new Piece(PieceType.Queen, this.color));
				else
					b.squares[mv.Y][mv.X].setPiece(b.squares[pos.Y][pos.X].piece);
				
				b.squares[pos.Y][pos.X].setPiece(new Piece(PieceType.None, PlayerColor.None));
			}
			public List<Point> getMoves(Board b, Point pos) {
				List<Point> ret = new List<Point>();

				SquarePos sqpos = SquarePos.getSquarePosFromIndex(pos.Y, pos.X);
				Point testpt = new Point();

				int direction = this.color == PlayerColor.Black ? -1 : 1;
				
				testpt = SquarePos.getPointFromSquarePos(sqpos.rank + direction, sqpos.file);//single move
				if (PieceData.testRange(testpt) && b.squares[testpt.Y][testpt.X].piece.type == PieceType.None) {
					ret.Add(testpt);

					if (sqpos.rank == (PlayerColor.Black == this.color ? 7 : 2)) { //double move
						testpt = SquarePos.getPointFromSquarePos(sqpos.rank + (2 * direction), sqpos.file);//double move
						if (PieceData.testRange(testpt) && b.squares[testpt.Y][testpt.X].piece.type == PieceType.None) {
							ret.Add(testpt);
						}
					}
				}

				testpt = SquarePos.getPointFromSquarePos(sqpos.rank + direction, (char)((int)sqpos.file - 1));//capture left
				if (PieceData.testRange(testpt) && b.squares[testpt.Y][testpt.X].piece.color != this.color && b.squares[testpt.Y][testpt.X].piece.color != PlayerColor.None && b.squares[testpt.Y][testpt.X].piece.type != PieceType.King) 
					ret.Add(testpt);
					
				testpt = SquarePos.getPointFromSquarePos(sqpos.rank + direction, (char)((int)sqpos.file + 1));//capture right
				if (PieceData.testRange(testpt) && b.squares[testpt.Y][testpt.X].piece.color != this.color && b.squares[testpt.Y][testpt.X].piece.color != PlayerColor.None && b.squares[testpt.Y][testpt.X].piece.type != PieceType.King) 
					ret.Add(testpt);

				//TODO check if own king would be checked

				return ret;
			}
		}
		public class NonePiece : IPieceType {
			public void Move(Board b, Point pos, Point mv) {
				
			}
			public bool CanMove() {
				return false;
			}
			public List<Point> getMoves(Board b, Point pos) {
				return new List<Point>();
			}
		}

		public enum PieceType {
			None,
			Pawn,
			Knight,
			Bishop,
			Rook,
			Queen,
			King
		}

		public class Piece {
			public PieceType type;
			public PlayerColor color;
			public IPieceType pieceActions;

			public static Bitmap ChessPieces = Chess.Properties.Resources.ChessPiecesSprite;
			public static Bitmap bmp_white_pawn = null;
			public static Bitmap bmp_white_knight = null;
			public static Bitmap bmp_white_bishop = null;
			public static Bitmap bmp_white_rook = null;
			public static Bitmap bmp_white_queen = null;
			public static Bitmap bmp_white_king = null;
			public static Bitmap bmp_black_pawn = null;
			public static Bitmap bmp_black_knight = null;
			public static Bitmap bmp_black_bishop = null;
			public static Bitmap bmp_black_rook = null;
			public static Bitmap bmp_black_queen = null;
			public static Bitmap bmp_black_king = null;


			public Piece() {
				this.type = PieceType.None;
				this.color = PlayerColor.None;
				this.pieceActions = getIPieceType(type, color);
			}

			public Piece(PieceType type, PlayerColor color) {
				this.type = type;
				this.color = color;
				this.pieceActions = getIPieceType(type, color);
			}

			public static IPieceType getIPieceType(PieceType type, PlayerColor color) {
				switch (type) {
					case PieceType.King: return new King(color);
					case PieceType.Queen: return new Queen(color);
					case PieceType.Rook: return new Rook(color);
					case PieceType.Bishop: return new Bishop(color);
					case PieceType.Knight: return new Knight(color);
					case PieceType.Pawn: return new Pawn(color);
					default: return new NonePiece();
				}
			}

			public void Move(Board b, Point pos, Point mv) {
				pieceActions.Move(b, pos, mv);
			}

			public String getPieceText() {
				switch (type) {
					case PieceType.None: return "";
					case PieceType.Pawn: return "Pawn";
					case PieceType.Knight: return "Knight";
					case PieceType.Bishop: return "Bishop";
					case PieceType.Rook: return "Rook";
					case PieceType.Queen: return "Queen";
					case PieceType.King: return "King";
					default: return "ErrorPiece";
				}
			}

			public static Bitmap getPieceImage(Piece piece) {
				Rectangle rc = new Rectangle(
					piece.type == PieceType.King ? 0 :
					piece.type == PieceType.Queen ? 333 :
					piece.type == PieceType.Bishop ? 666 :
					piece.type == PieceType.Knight ? 999 :
					piece.type == PieceType.Rook ? 1322 :
					piece.type == PieceType.Pawn ? 1665 : 
					-1,					
					piece.color == PlayerColor.White ? 0 : 333, 333, 333);

				if (piece.color == PlayerColor.White) {
					if (piece.type == PieceType.King) {
						if (Piece.bmp_white_king == null)
							Piece.bmp_white_king = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_white_king;
					}
					if (piece.type == PieceType.Queen) {
						if (Piece.bmp_white_queen == null)
							Piece.bmp_white_queen = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_white_queen;
					}
					if (piece.type == PieceType.Rook) {
						if (Piece.bmp_white_rook == null)
							Piece.bmp_white_rook = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_white_rook;
					}
					if (piece.type == PieceType.Bishop) {
						if (Piece.bmp_white_bishop == null)
							Piece.bmp_white_bishop = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_white_bishop;
					}
					if (piece.type == PieceType.Knight) {
						if (Piece.bmp_white_knight == null)
							Piece.bmp_white_knight = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_white_knight;
					}
					if (piece.type == PieceType.Pawn) {
						if (Piece.bmp_white_pawn == null)
							Piece.bmp_white_pawn = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_white_pawn;
					}
				}
				else {
					if (piece.type == PieceType.King) {
						if (Piece.bmp_black_king == null)
							Piece.bmp_black_king = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_black_king;
					}
					if (piece.type == PieceType.Queen) {
						if (Piece.bmp_black_queen == null)
							Piece.bmp_black_queen = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_black_queen;
					}
					if (piece.type == PieceType.Rook) {
						if (Piece.bmp_black_rook == null)
							Piece.bmp_black_rook = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_black_rook;
					}
					if (piece.type == PieceType.Bishop) {
						if (Piece.bmp_black_bishop == null)
							Piece.bmp_black_bishop = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_black_bishop;
					}
					if (piece.type == PieceType.Knight) {
						if (Piece.bmp_black_knight == null)
							Piece.bmp_black_knight = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_black_knight;
					}
					if (piece.type == PieceType.Pawn) {
						if (Piece.bmp_black_pawn == null)
							Piece.bmp_black_pawn = (Bitmap)ChessPieces.Clone(rc, ChessPieces.PixelFormat);
						return bmp_black_pawn;
					}
				}

				return null;
			}
		}

		public struct SquarePos {
			public int rank;
			public char file;

			public SquarePos(int rank, char file) {
				this.rank = rank;
				this.file = file;
			}

			public static SquarePos getSquarePosFromIndex(int i, int j) {
				return new SquarePos(8 - i, (char)(j + 'a'));
			}
			public static Point getPointFromSquarePos(int rank, char file) {
				return new Point(file - 'a', 8 - rank);
			}
			public static Point getPointFromSquarePos(SquarePos sqpos) {
				return getPointFromSquarePos(sqpos.rank, sqpos.file);
			}
		}

		public class Square {
			public RECT rc;
			public SquarePos pos;
			public Piece piece;
			public PlayerColor squareColor;

			public Square(SquarePos pos, Piece piece, PlayerColor squareColor) {
				this.pos = pos;
				this.piece = piece;
				this.squareColor = squareColor;
				setRectFromPos(pos);			
			}

			public void setPiece(Piece piece) {
				this.piece = piece;
			}

			public void setRectFromPos(SquarePos pos) {
				int w = getclientrect(GetActiveWindow()).right;
				int h = getclientrect(GetActiveWindow()).bottom;

				if (w > 0 && h > 0) { 
					int sqw = w / 8;
					int sqh = h / 8;

					this.rc.left = sqw * (int)(pos.file - 'a');
					this.rc.right = sqw * ((int)(pos.file - 'a') + 1) - 1;
					this.rc.top = sqh * (8 - pos.rank);
					this.rc.bottom = sqh * (8 - pos.rank + 1) - 1;
				}
			}

			public void updateRect() {
				setRectFromPos(pos);
			}
		}

		public class Board {
			public List<List<Square>> squares = new List<List<Square>>();

			public Board() {
				squares = new List<List<Square>>{
					new List<Square> {
						new Square(new SquarePos(8, 'a'), new Piece(PieceType.Rook, PlayerColor.Black), PlayerColor.White),
						new Square(new SquarePos(8, 'b'), new Piece(PieceType.Knight, PlayerColor.Black), PlayerColor.Black),
						new Square(new SquarePos(8, 'c'), new Piece(PieceType.Bishop, PlayerColor.Black), PlayerColor.White), 
						new Square(new SquarePos(8, 'd'), new Piece(PieceType.Queen, PlayerColor.Black), PlayerColor.Black), 
						new Square(new SquarePos(8, 'e'), new Piece(PieceType.King, PlayerColor.Black), PlayerColor.White),
						new Square(new SquarePos(8, 'f'),new Piece(PieceType.Bishop, PlayerColor.Black), PlayerColor.Black), 
						new Square(new SquarePos(8, 'g'), new Piece(PieceType.Knight, PlayerColor.Black), PlayerColor.White), 
						new Square(new SquarePos(8, 'h'), new Piece(PieceType.Rook, PlayerColor.Black), PlayerColor.Black)
					}, 



					new List<Square> {
						new Square(new SquarePos(7, 'a'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.Black),
						new Square(new SquarePos(7, 'b'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.White), 
						new Square(new SquarePos(7, 'c'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.Black), 
						new Square(new SquarePos(7, 'd'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.White),
						new Square(new SquarePos(7, 'e'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.Black), 
						new Square(new SquarePos(7, 'f'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.White),
						new Square(new SquarePos(7, 'g'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.Black),
						new Square(new SquarePos(7, 'h'), new Piece(PieceType.Pawn, PlayerColor.Black), PlayerColor.White)
					}, 
				
					
					
					new List<Square> {
						new Square(new SquarePos(6, 'a'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(6, 'b'), new Piece(), PlayerColor.Black), 
						new Square(new SquarePos(6, 'c'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(6, 'd'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(6, 'e'), new Piece(), PlayerColor.White), 
						new Square(new SquarePos(6, 'f'), new Piece(), PlayerColor.Black), 
						new Square(new SquarePos(6, 'g'), new Piece(), PlayerColor.White), 
						new Square(new SquarePos(6, 'h'), new Piece(), PlayerColor.Black)
					}, 
					
					
									
					new List<Square> {
						new Square(new SquarePos(5, 'a'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(5, 'b'), new Piece(), PlayerColor.White), 
						new Square(new SquarePos(5, 'c'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(5, 'd'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(5, 'e'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(5, 'f'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(5, 'g'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(5, 'h'), new Piece(), PlayerColor.White)
					}, 
										
					
					
					new List<Square> {
						new Square(new SquarePos(4, 'a'), new Piece(), PlayerColor.White), 
						new Square(new SquarePos(4, 'b'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(4, 'c'), new Piece(), PlayerColor.White), 
						new Square(new SquarePos(4, 'd'), new Piece(), PlayerColor.Black), 
						new Square(new SquarePos(4, 'e'), new Piece(), PlayerColor.White), 
						new Square(new SquarePos(4, 'f'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(4, 'g'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(4, 'h'), new Piece(), PlayerColor.Black)
					}, 
					
					
					
					new List<Square> {new Square(
						new SquarePos(3, 'a'), new Piece(), PlayerColor.Black), 
						new Square(new SquarePos(3, 'b'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(3, 'c'), new Piece(), PlayerColor.Black), 
						new Square(new SquarePos(3, 'd'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(3, 'e'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(3, 'f'), new Piece(), PlayerColor.White),
						new Square(new SquarePos(3, 'g'), new Piece(), PlayerColor.Black),
						new Square(new SquarePos(3, 'h'), new Piece(), PlayerColor.White)
					}, 
					
					
					
					new List<Square> {
						new Square(new SquarePos(2, 'a'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.White),
						new Square(new SquarePos(2, 'b'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.Black),
						new Square(new SquarePos(2, 'c'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.White), 
						new Square(new SquarePos(2, 'd'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.Black), 
						new Square(new SquarePos(2, 'e'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.White),
						new Square(new SquarePos(2, 'f'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.Black), 
						new Square(new SquarePos(2, 'g'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.White),
						new Square(new SquarePos(2, 'h'), new Piece(PieceType.Pawn, PlayerColor.White), PlayerColor.Black)
					}, 
					
					
					
					new List<Square> {
						new Square(new SquarePos(1, 'a'), new Piece(PieceType.Rook, PlayerColor.White), PlayerColor.Black), 
						new Square(new SquarePos(1, 'b'), new Piece(PieceType.Knight, PlayerColor.White), PlayerColor.White), 
						new Square(new SquarePos(1, 'c'),new Piece(PieceType.Bishop, PlayerColor.White), PlayerColor.Black), 
						new Square(new SquarePos(1, 'd'), new Piece(PieceType.Queen, PlayerColor.White), PlayerColor.White),
						new Square(new SquarePos(1, 'e'), new Piece(PieceType.King, PlayerColor.White), PlayerColor.Black),
						new Square(new SquarePos(1, 'f'),new Piece(PieceType.Bishop, PlayerColor.White), PlayerColor.White),
						new Square(new SquarePos(1, 'g'), new Piece(PieceType.Knight, PlayerColor.White), PlayerColor.Black),
						new Square(new SquarePos(1, 'h'), new Piece(PieceType.Rook, PlayerColor.White), PlayerColor.White)
					}, 
				};
			}

			public void Draw(Form1 sender) {
				for (int i = 0; i < squares.Count; i++) {
					for (int j = 0; j < squares[i].Count; j++) {
						squares[i][j].updateRect();
					}
				}

				sender.Invalidate();
			}

			public List<Point> getSquareMoves(Point pos) {
				if (!PieceData.testRange(pos)) return new List<Point>();
				return squares[pos.Y][pos.X].piece.pieceActions.getMoves(this, pos);
			}
		}

		private void Form1_Load(object sender, EventArgs e) {
			g_hwnd = this.Handle;

			g_board = new Board();

			this.DoubleBuffered = true;
		}

		private void Form1_Paint(object sender, PaintEventArgs e) {
			if (g_board != null) {
				//SolidBrush whiteBrush = new SolidBrush(Color.White); 
				//SolidBrush blackBrush = new SolidBrush(Color.Black);
				Pen bluePen = new Pen(Color.Blue, 2);
				Pen redPen = new Pen(Color.Red, 2);

				SolidBrush grayBrush = new SolidBrush(Color.Gray);
				SolidBrush lightgrayBrush = new SolidBrush(Color.LightGray);
				SolidBrush cyanBrush = new SolidBrush(Color.Cyan);
				SolidBrush redBrush = new SolidBrush(Color.Red);
				SolidBrush violetBrush = new SolidBrush(Color.Purple);

				Font font = new Font("Arial", 10, FontStyle.Bold);

				Graphics g = e.Graphics;

				List<Point> attackers = new List<Point>();
				int checkcount = King.isKingInCheck(g_board, g_current_player, ref attackers);

				for (int i = 0; i < g_board.squares.Count; i++) {
					for (int j = 0; j < g_board.squares[i].Count; j++) {
						RECT r = g_board.squares[i][j].rc;
						Rectangle rc = new Rectangle(r.left, r.top, r.right - r.left, r.bottom - r.top);

						SolidBrush selectedBrush = null;

						if (g_selectedPoint.X == j && g_selectedPoint.Y == i && g_board.squares[i][j].piece.type != PieceType.None){
							if (checkcount > 0 && g_board.squares[i][j].piece.type == PieceType.King && g_board.squares[i][j].piece.color == g_current_player) 
								selectedBrush = violetBrush;
							else
								selectedBrush = cyanBrush; 
						}
						else {
							if (checkcount > 0 && g_board.squares[i][j].piece.type == PieceType.King && g_board.squares[i][j].piece.color == g_current_player)
								selectedBrush = redBrush;
							else if (g_board.squares[i][j].squareColor == PlayerColor.White)
								selectedBrush = lightgrayBrush;
							else 
								selectedBrush = grayBrush;
						}

						g.FillRectangle(selectedBrush, rc);

						Rectangle borderrc = rc;
						if (attackers.IndexOf(new Point(j, i)) >= 0) {
							g.DrawRectangle(redPen, borderrc);
							borderrc.X += 2;
							borderrc.Y += 2;
							borderrc.Width -= 4;
							borderrc.Height -= 4;
						}

						if (g_selected_valid_moves.IndexOf(new Point(j, i)) >= 0) {
							g.DrawRectangle(bluePen, borderrc);
						}

						if (g_board.squares[i][j].piece.type != PieceType.None) {
							g.DrawImage(Piece.getPieceImage(g_board.squares[i][j].piece), rc);
						//	g.DrawString(g_board.squares[i][j].piece.getPieceText(), font, g_board.squares[i][j].color == PlayerColor.Black ? grayBrush : lightgrayBrush, new Point(rc.X, rc.Y));
						}
					}
				}

				if (PieceData.testRange(g_last_move.Item1) && PieceData.testRange(g_last_move.Item2)) {					
					int sw = g_board.squares[0][0].rc.right - g_board.squares[0][0].rc.left;
					int sh = g_board.squares[0][0].rc.bottom - g_board.squares[0][0].rc.top;
				
					AdjustableArrowCap myArrow = new AdjustableArrowCap(6, 6);
					Pen capPen = new Pen(Color.Yellow, 3);
					capPen.CustomEndCap = myArrow;
					e.Graphics.DrawLine(capPen,
						g_board.squares[g_last_move.Item2.Y][g_last_move.Item2.X].rc.left + (sw / 2),
						g_board.squares[g_last_move.Item2.Y][g_last_move.Item2.X].rc.top + (sh / 2),
						g_board.squares[g_last_move.Item1.Y][g_last_move.Item1.X].rc.left + (sw / 2),
						g_board.squares[g_last_move.Item1.Y][g_last_move.Item1.X].rc.top + (sh / 2));					
				}
			}
		}

		private void Form1_Shown(object sender, EventArgs e) {
			g_board.Draw(this);
		}

		private void Form1_Resize(object sender, EventArgs e) {
			g_board.Draw(this);
		}

		private void Form1_MouseClick(object sender, MouseEventArgs e) {
			POINT pt = new POINT(e.Location.X, e.Location.Y);

			Point testpt = new Point(-1, -1);
			for (int i = 0; i < g_board.squares.Count; i++) {
				for (int j = 0; j < g_board.squares[i].Count; j++) {
					if (PtInRect(out g_board.squares[i][j].rc, pt)) {
						testpt = new Point(j, i);
					}
				}
			}

			if (PieceData.testRange(testpt) && g_selected_valid_moves.IndexOf(testpt) >= 0) {//Move piece
				g_board.squares[g_selectedPoint.Y][g_selectedPoint.X].piece.pieceActions.Move(g_board, g_selectedPoint, testpt);

				g_last_move = new Tuple<Point, Point>(testpt, g_selectedPoint);

				g_current_player = g_current_player == PlayerColor.Black ? PlayerColor.White : PlayerColor.Black;

				int checkcount = 0;
				for (int i = 0; i < g_board.squares.Count; i++) {
					for (int j = 0; j < g_board.squares[i].Count; j++) {
						if (g_board.squares[i][j].piece.type == PieceType.King && g_current_player != g_board.squares[i][j].piece.color)
							checkcount += King.isKingInCheck(g_board, SquarePos.getPointFromSquarePos(g_board.squares[i][j].pos), g_board.squares[i][j].piece.color);
					}
				}

				if (checkcount > 0)
					Console.WriteLine("check");
			}
			else {
				if (g_selectedPoint.Equals(testpt))
					g_selectedPoint = new Point(-1, -1);
				else
					g_selectedPoint = testpt;
			}

			if (PieceData.testRange(testpt))
				g_selected_valid_moves = g_board.getSquareMoves((g_current_player == g_board.squares[testpt.Y][testpt.X].piece.color) ? g_selectedPoint : new Point(-1,-1));
			
			g_board.Draw(this);
		}

		private Point pt_last_hover = new Point(-1, -1);
		private void Form1_MouseMove(object sender, MouseEventArgs e) {
			POINT pt = new POINT(e.Location.X, e.Location.Y);

			if (PieceData.testRange(g_selectedPoint) && g_board.squares[g_selectedPoint.Y][g_selectedPoint.X].piece.color != g_current_player) {

				Point testpt = new Point(-1, -1);
				for (int i = 0; i < g_board.squares.Count; i++) {
					for (int j = 0; j < g_board.squares[i].Count; j++) {
						if (PtInRect(out g_board.squares[i][j].rc, pt)) {
							testpt = new Point(j, i);
						}
					}
				}

				if (!pt_last_hover.Equals(testpt)) {
					if (PieceData.testRange(testpt))
						g_selected_valid_moves = g_board.getSquareMoves((g_current_player == g_board.squares[testpt.Y][testpt.X].piece.color) ? testpt : new Point(-1, -1));
					g_board.Draw(this);
				}

				pt_last_hover = testpt;
			}
		}
	}
}
