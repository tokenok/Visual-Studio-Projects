using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TestGame {
	public partial class Form1 : Form {
		#region a
		public const int SW_HIDE = 0;
		public const int SW_MAXIMIZE = 3;
		public const int SW_SHOW = 5;

		[DllImport("kernel32.dll")]
		static extern bool AllocConsole();

		[DllImport("kernel32.dll")]
		static extern IntPtr GetConsoleWindow();

		[DllImport("User32.dll")]
		static extern IntPtr GetForegroundWindow();

		[DllImport("user32.dll")]
		static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);		

		public const int WH_KEYBOARD_LL = 13;
		public const int WH_MOUSE_LL = 14;

		public const int WM_KEYDOWN = 0x100;
		public const int WM_KEYUP = 0x101;
		public const int WM_SYSKEYDOWN = 0x104;
		public const int WM_SYSKEYUP = 0x105;
		public const int WM_MOUSEMOVE = 0x0200;
		public const int WM_LBUTTONDOWN = 0x0201;
		public const int WM_LBUTTONUP = 0x0202;
		public const int WM_RBUTTONDOWN = 0x0204;
		public const int WM_RBUTTONUP = 0x0205;

		public const int VK_SHIFT = 0x10;
		public const int VK_CONTROL = 0x11;
		public const int VK_MENU = 0x12;

		public delegate int HookProc(int nCode, IntPtr wParam, IntPtr lParam);
		HookProc kbProc;
		HookProc mProc;

		static int kbhook = 0;
		static int mhook = 0;

		[DllImport("user32.dll")]
		public static extern int SetWindowsHookEx(int idHook, HookProc lpfn, IntPtr hInstance, int threadId);

		[DllImport("user32.dll")]
		public static extern bool UnhookWindowsHookEx(int idHook);

		[DllImport("user32.dll")]
		public static extern int CallNextHookEx(int idHook, int nCode, IntPtr wParam, IntPtr lParam);

		[StructLayout(LayoutKind.Sequential)]
		public class KBDLLHOOKSTRUCT {
			public int vkCode;
			public int scanCode;
			public int flags;
			public int time;
			public int dwExtraInfo;
		}

		[StructLayout(LayoutKind.Sequential)]
		public class MSLLHOOKSTRUCT {
			public POINT pt;
			public int mouseData;
			public int flags;
			public int time;
			public int dwExtraInfo;
		}

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
		}

		#endregion

		public enum MoveDirection {
			Right, Left, Up, Down
		}

		public enum CellType {
			None = 0,
			Door = 1,
			Item = 2, 
			Monster = 3,
			Player = 4, 
			Wall = 5, 
			Hole = 6, 
			Ladder = 7,
			Projectile = 8, 
			Trap = 9
		}

		public class Cell {
			public ConsoleColor color;
			public CellType type;
			public char symbol;

			public Item item_info;
			public Door door_info;
				
			public ConsoleColor getColorFromType() {
				ConsoleColor ret = ConsoleColor.White;



				color = ret;
				return ret;
			}
		}

		public enum ItemType {
			Unknown = 0,
			Sword = 1,
			Shield = 2, 
			Armor = 3, 
			Potion = 4,
			Key = 5
		}

		public class Item {
			public string name;
			public int count;
			public ItemType type;
			public int val;
		}

		public class Door {
			public bool is_locked;
			public int to_lvl;
			public int door_id;
		}

		public class Game {
			public int level;
			public List<List<Cell>> board;
			public List<Item> inventory;

			public bool is_using_item;
			public bool is_using_inventory;
			public int selected_inventory_item;

			public Game() {
				board = new List<List<Cell>>();
				inventory = new List<Item>();
			}

			public void loadCurrentLevel() {
				board.Clear();
				try {
					string[] lines = File.ReadAllLines("level " + level + ".csv");
					for (int i = 0; i < lines.Length; i++) {
						List<Cell> line = new List<Cell>();
						string[] slines = lines[i].Split(',');
						for (int j = 0; j < slines.Length; j++) {
							Cell c = new Cell();

							string[] data = slines[j].Split('`');

							if (data[0].Length == 0) {
								c.symbol = ' ';
								c.color = ConsoleColor.Black;
								c.type = CellType.None;
							}
							else {
								c.symbol = data[0].Length == 0 || (data[0][0] >= '0' && data[0][0] <= '9') ? ' ' : data[0][0];
								c.type = (CellType)int.Parse(data[1]);
								c.color = (ConsoleColor)int.Parse(data[2]);
								if (c.type == CellType.Item) {
									Item item = new Item();
									item.name = data[3];
									item.count = int.Parse(data[4]);
									item.type = (ItemType)int.Parse(data[5]);
									item.val = int.Parse(data[6]);
									c.item_info = item;
								}
							}
							line.Add(c);
						}
						board.Add(line);
					}
				}
				catch(Exception e) {
					MessageBox.Show(e.ToString());
				}
			}

			bool checkBounds(int i, int j) {
				if (i >= board.Count || i < 0)
					return false;
				if (board[0].Count > 0 && (j >= board[0].Count || j < 0))
					return false;

				return true;
			}

			public void printBoard() {
				//Console.Clear();
				Console.SetCursorPosition(0, 0);
				for (int i = 0; i < board.Count; i++) {
					for (int j = 0; j < board[i].Count; j++) {
						Console.ForegroundColor = board[i][j].color;
						Console.Write(board[i][j].symbol);
					}					
					Console.WriteLine();
				}
				Console.WriteLine();

				printInventory();
			}

			public void printInventory() {
				Console.WriteLine(); Console.WriteLine(); Console.WriteLine(); Console.WriteLine();
				Console.WriteLine("Inventory");
				for (int i = 0; i < inventory.Count; i++) {
					ConsoleColor old = Console.ForegroundColor;
					if (i == selected_inventory_item)
						Console.ForegroundColor = ConsoleColor.Green;
					Console.WriteLine(inventory[i].name + " " + inventory[i].count + " " + Enum.GetName(typeof(ItemType), inventory[i].type));
					Console.ForegroundColor = old;
				}
			}

			public Point getPlayerPos() {
				Point ret = new Point(-1, -1);
				for (int i = 0; i < board.Count; i++) {
					for (int j = 0; j < board[i].Count; j++) {
						if (board[i][j].symbol == 'T') {
							ret.X = j;
							ret.Y = i;
							return ret;
						}
					}					
				}

				return ret;
			}

			public bool can_move(Point oldpt, Point newpt) {
				if (!checkBounds(newpt.Y, newpt.X))
					return false;

				Cell t = board[newpt.Y][newpt.X];

				if (t.type == CellType.Wall)
					return false;

				return true;
			}

			public bool move(MoveDirection d) {
				Point pt = getPlayerPos();
				if (!checkBounds(pt.Y, pt.X))
					return false;
				Point newpt = pt;

				switch (d) {
					case MoveDirection.Down: 
						newpt.Y++;
						break;
					case MoveDirection.Up:
						newpt.Y--;
						break;
					case MoveDirection.Right:
						newpt.X++;
						break;
					case MoveDirection.Left:
						newpt.X--;
						break;
				}

				if (can_move(pt, newpt)) {
					try {
						if (board[newpt.Y][newpt.X].type == CellType.Item) {
							inventory.Add(board[newpt.Y][newpt.X].item_info);
						}
					}
					catch (Exception e) {
						MessageBox.Show(e.ToString());
					}

					Cell t = board[pt.Y][pt.X];
					board[newpt.Y][newpt.X] = t;
					board[pt.Y][pt.X] = new Cell();

					
				}				

				return true;
			}
		}

		Game G;
		IntPtr g_hwnd;

		public Form1() {
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e) {
		//	Visible = false; 
		//	ShowInTaskbar = false; 
		//	Opacity = 0;

			AllocConsole();
			ShowWindow(GetConsoleWindow(), SW_SHOW);

			Console.CursorVisible = false;

			kbProc = new HookProc(kbproc);
			kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbProc, (IntPtr)0, 0);

			G = new Game();

			G.level = 1;
			G.loadCurrentLevel();

			G.printBoard();

			//DoubleBuffered = true;

			timer1.Start();
		}

		//public class point {
		//	public int X, Y;
		//	public point() { }
		//	public point(int x, int y) {
		//		X = x;
		//		Y = y;
		//	}
		//}

		//public class Triplet<T1, T2, T3> {
		//	public T1 First { get; set; }
		//	public T2 Second { get; set; }
		//	public T3 Third { get; set; }

		//	public Triplet(T1 a, T2 b, T3 c) {
		//		First = a;
		//		Second = b;
		//		Third = c;
		//	}
		//}
		//public List<Triplet<double, double, point>> g_pts = new List<Triplet<double, double, point>>();

		private void Form1_Shown(object sender, EventArgs e) {
			g_hwnd = this.Handle;

			//int res = 100;
			//for (int i = 0; i < res; i++) {
			//	double t = 2 * Math.PI * i / (res - 1);
			//	point pt = new point();

			//	pt.Y = 100 - (int)(13 * Math.Cos(t) - 5 * Math.Cos(2 * t) - 2 * Math.Cos(3 * t) - Math.Cos(4 * t));
			//	pt.X = (int)(16 * Math.Pow(1 * Math.Sin(t), 3)) + 100;

			//	g_pts.Add(new Triplet<double, double, point>(45.0, 30.0, pt));
			//}

			ShowWindow(g_hwnd, SW_HIDE);
		}

		

		private void timer1_Tick(object sender, EventArgs e) {
			G.printBoard();
			
			//System.Drawing.Graphics graphics = this.CreateGraphics();			
			//Pen pen = new Pen(Color.Red);
			//graphics.FillRectangle(new SolidBrush(Color.Purple), 0, 0, this.Width, this.Height);			
			//for (int i = 0; i < g_pts.Count; i++) {
			//	g_pts[i].Third.X += (int)(g_pts[i].Second * Math.Cos(g_pts[i].First * Math.PI / 180.0));
			//	g_pts[i].Third.Y += (int)(g_pts[i].Second * Math.Sin(g_pts[i].First * Math.PI / 180.0));
			//	if (g_pts[i].Third.X < 0 || g_pts[i].Third.X > this.Width) 
			//		g_pts[i].First = 180.0 - g_pts[i].First;				
			//	if (g_pts[i].Third.Y < 0 || g_pts[i].Third.Y > this.Height - 25) 
			//		g_pts[i].First = 360.0 - g_pts[i].First;
			//	g_pts[i].First %= 360.0;
			//	graphics.DrawEllipse(pen, g_pts[i].Third.X, g_pts[i].Third.Y, 2, 2);
			//}
		}

		public static int kbproc(int nCode, IntPtr wParam, IntPtr lParam) {
			KBDLLHOOKSTRUCT kb = (KBDLLHOOKSTRUCT)Marshal.PtrToStructure(lParam, typeof(KBDLLHOOKSTRUCT));
			if (nCode >= 0) {
				IntPtr foreground = GetForegroundWindow();
								
				if (GetConsoleWindow() != foreground)
					return CallNextHookEx(kbhook, nCode, wParam, lParam);

				Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

				if ((int)wParam == WM_KEYDOWN || (int)wParam == WM_SYSKEYDOWN) {
					switch ((Keys)kb.vkCode) {
						case Keys.Up: {
							if (!f1.G.is_using_item)
								f1.G.move(MoveDirection.Up);
							f1.G.printBoard();
							break;
						}
						case Keys.Down: {
							if (!f1.G.is_using_item)
								f1.G.move(MoveDirection.Down);
							f1.G.printBoard();
							break;
						}
						case Keys.Left: {
							if (!f1.G.is_using_item)
								f1.G.move(MoveDirection.Left);
							f1.G.printBoard();
							break;
						}
						case Keys.Right: {
							if (!f1.G.is_using_item) {
								f1.G.move(MoveDirection.Right);
							}
							f1.G.printBoard();
							break;
						}
						case Keys.Space: {
							f1.G.is_using_item = true;
							break;
						}
						case Keys.Tab: {
							f1.G.selected_inventory_item = f1.G.selected_inventory_item >= f1.G.inventory.Count - 1 ? 0 : f1.G.selected_inventory_item + 1;
							f1.G.printBoard();
							break;
						}
					}					
				}
				
				if ((int)wParam == WM_KEYUP) {
					switch ((Keys)kb.vkCode) {
						case Keys.Space: {
							f1.G.is_using_item = false;

							break;
						}
					}
				}
			}

			return CallNextHookEx(kbhook, nCode, wParam, lParam);
		}

			
	}
}
