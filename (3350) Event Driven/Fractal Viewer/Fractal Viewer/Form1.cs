using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
using System.Reflection;
using System.Drawing.Imaging;
using System.IO;
using AviFile;
using Microsoft.Win32.SafeHandles;
using System.Diagnostics;

namespace Fractal_Viewer {
	public partial class Form1 : Form {
		#region dll imports

		[DllImport("User32.dll")]
		static extern short GetAsyncKeyState(int a);

		[DllImport("User32.dll")]
		static extern short GetKeyState(int a);

		[DllImport("kernel32.dll")]
		static extern bool AllocConsole();

        [DllImport("kernel32.dll")]
		static extern IntPtr GetConsoleWindow();

		[DllImport("user32.dll")]
		static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

		[DllImport("user32.dll")]
		static extern IntPtr GetForegroundWindow();

		[DllImport("user32.dll")]
		static extern IntPtr GetFocus();

		[DllImport("user32.dll")]
		static extern bool GetWindowRect(IntPtr hwnd, out RECT lpRect);

		[DllImport("user32.dll")]
		static extern bool GetClientRect(IntPtr hwnd, out RECT lpRect);

		[DllImport("user32.dll")]
		static extern int MapWindowPoints(IntPtr hWndFrom, IntPtr hWndTo, ref RECT lpPoints, int cPoints);

		[DllImport("user32.dll")]
		static extern bool PtInRect(out RECT lprc, POINT pt);

		[DllImport("user32.dll")]
		static extern bool GetCursorPos(out POINT lpPoint);

		[DllImport("user32.dll")]
		static extern bool SetCursorPos(int x, int y);

		[DllImport("kernel32.dll")]
		static extern uint GetModuleFileName(IntPtr hModule, [Out] StringBuilder lpFilename, int nSize);

		[DllImport("kernel32.dll")]
		static extern IntPtr GetStdHandle(int nStdHandle);

		[DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
		static extern bool GetCurrentConsoleFontEx(IntPtr hConsoleOutput, bool bMaximumWindow, ref CONSOLE_FONT_INFOEX lpConsoleCurrentFontEx);

		[DllImport("kernel32.dll")]
		static extern bool SetCurrentConsoleFontEx(IntPtr hConsoleOutput, bool bMaximumWindow, ref CONSOLE_FONT_INFOEX lpConsoleCurrentFontEx);

		[DllImport("user32.dll")]
		static extern bool SystemParametersInfo(uint uiAction, uint uiParam, ref RECT pvParam, uint fWinIni);

		static class DisableConsoleQuickEdit {

			const uint ENABLE_QUICK_EDIT = 0x0040;

			// STD_INPUT_HANDLE (DWORD): -10 is the standard input device.
			const int STD_INPUT_HANDLE = -10;

			[DllImport("kernel32.dll", SetLastError = true)]
			static extern IntPtr GetStdHandle(int nStdHandle);

			[DllImport("kernel32.dll")]
			static extern bool GetConsoleMode(IntPtr hConsoleHandle, out uint lpMode);

			[DllImport("kernel32.dll")]
			static extern bool SetConsoleMode(IntPtr hConsoleHandle, uint dwMode);

			internal static bool Go() {

				IntPtr consoleHandle = GetStdHandle(STD_INPUT_HANDLE);

				// get current console mode
				uint consoleMode;
				if (!GetConsoleMode(consoleHandle, out consoleMode)) {
					// ERROR: Unable to get console mode.
					return false;
				}

				// Clear the quick edit bit in the mode flags
				consoleMode &= ~ENABLE_QUICK_EDIT;

				// set the new mode
				if (!SetConsoleMode(consoleHandle, consoleMode)) {
					// ERROR: Unable to set console mode
					return false;
				}

				return true;
			}
		}

		#endregion

		#region SetWindowsHookEx import

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

		public const int SW_HIDE = 0;
		public const int SW_MAXIMIZE = 3;
		public const int SW_SHOW = 5;

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

		#endregion

		[StructLayout(LayoutKind.Sequential)]
		public struct RECT {
			public int left;
			public int top;
			public int right;
			public int bottom;
		}

		[StructLayout(LayoutKind.Sequential)]
		public struct POINT{
			public int x;
			public int y;
		}

		[StructLayout(LayoutKind.Sequential)]
		public struct COORD{
			public short X, Y;
		}

		[StructLayout(LayoutKind.Sequential)]
		public unsafe struct CONSOLE_FONT_INFOEX {
			public uint cbSize;
			public uint nFont;
			public COORD dwFontSize;
			public int FontFamily;
			public int FontWeight;
			public fixed char FaceName[32];
		} 

		public static RECT getwindowrect(IntPtr wnd) {
			RECT ret;
			GetWindowRect(wnd, out ret);
			return ret;
		}

		public static POINT getcursorpos() {
			POINT ret;
			GetCursorPos(out ret);
			return ret;
		}
		
		public static RECT getclientrect(IntPtr wnd) {
			RECT ret;
			GetClientRect(wnd, out ret);
			return ret;
		}

		public static RECT getmappedclientrect(IntPtr wndFrom, IntPtr wndTo = new IntPtr()) {
			RECT ret = getclientrect(wndFrom);
			MapWindowPoints(wndFrom, wndTo, ref ret, 2);
			return ret;
		}

		public static POINT getclientcursorpos(IntPtr hwnd_parent) {
			RECT temprect;
			GetClientRect(hwnd_parent, out temprect);
			MapWindowPoints(hwnd_parent, new IntPtr(), ref temprect, 2);
			POINT temppoint;
			GetCursorPos(out temppoint);
			POINT client_cursor_pos;
			client_cursor_pos.x = temppoint.x - temprect.left;
			client_cursor_pos.y = temppoint.y - temprect.top;
			return client_cursor_pos;
		}

		public static string getexedir() {
			StringBuilder filename = new StringBuilder(1024);
			GetModuleFileName(IntPtr.Zero, filename, 1024);
			string dir = filename.ToString();
			return dir.Substring(0, dir.LastIndexOf('\\'));
		}

		public class Complex {
			public double r, i;

			public Complex(double r, double i){
				this.r = r;
				this.i = i;
			}
		}

		public class CaptureImage {
			public Image img;
			public string name = "";
		}

		public enum ColorModes {
			Palette,
			BlackAndWhite,
			Rainbow,
			Random
		}

		public Random rng = new Random();

		public static class G {
			public static double xmin = -3, xmax = 3, ymin = -2, ymax = 2;
			public static double jxmin = -3, jxmax = 3, jymin = -2, jymax = 2, jxpos, jypos;
			public static double cxmin = -3, cxmax = 3, cymin = -2, cymax = 2;
			public static double sxoff, syoff;
			public static int spanx = 0, spany = 0, panx = 0, pany = 0;
			public static double jsxoff, jsyoff;
			public static int jspanx = 0, jspany = 0, jpanx = 0, jpany = 0;
			public static int maxloop = 500, gradients, colors;
			public static ColorModes ColorMode = 0;
			public static List<Color> palettecolors = new List<Color>();
			public static List<Color> gradientcolors = new List<Color>();
			public static bool edge = false, cycle = false, cycledown = false, protectdraw = false;
			public static List<List<int>> MandelbrotCountgrid = new List<List<int>>();
			public static List<List<int>> MandelbrotActiveCountgrid = new List<List<int>>();
			public static List<List<int>> JuliaCountgrid = new List<List<int>>();
			public static List<List<int>> JuliaActiveCountgrid = new List<List<int>>();
			public static List<List<int>> ConsolebrotCountgrid = new List<List<int>>();
			public static List<List<int>> ConsolebrotActiveCountgrid = new List<List<int>>();
			public static List<Complex> MandelbrotEscapePoints = new List<Complex>();
			public static List<Complex> JuliaEscapePoints = new List<Complex>();
			public static Color[] knowncolors;
			public static bool console = false;
			public static int imagecounter = 0;
			public static bool recording = false, recording_block = false;
			public static List<CaptureImage> capturelist = new List<CaptureImage>();
			public static string recording_dir = "";
		}

		public Form1() {
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e) {
            AllocConsole();
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			
			RECT rcd = new RECT();
			SystemParametersInfo(0x0030, 0, ref rcd, 0);

			IntPtr outcon = GetStdHandle(-11);

			CONSOLE_FONT_INFOEX cfi = new CONSOLE_FONT_INFOEX();
			cfi.cbSize = (uint)Marshal.SizeOf(cfi);
			GetCurrentConsoleFontEx(outcon, false, ref cfi);

			//	cfi.dwFontSize.X = 4; cfi.dwFontSize.Y = 6;
			//	cfi.dwFontSize.X = 8; cfi.dwFontSize.Y = 12;
			cfi.dwFontSize.X = 20; cfi.dwFontSize.Y = 30;

            SetCurrentConsoleFontEx(outcon, false, ref cfi);

			DisableConsoleQuickEdit.Go();

			int w = rcd.right - (2 * cfi.dwFontSize.X);
			int h = rcd.bottom - 30 - (2 * cfi.dwFontSize.Y);

            Console.WindowWidth = w / cfi.dwFontSize.X;
            Console.WindowHeight = h / cfi.dwFontSize.Y;
            Console.BufferWidth = Console.WindowWidth;
            Console.BufferHeight = Console.WindowHeight;

            //////////////////////////////////////////////////////////////////////////////////////////

            kbProc = new HookProc(kbproc);
			kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbProc, (IntPtr)0, 0);

			pb_julia.Parent.Controls.SetChildIndex(pb_julia, 0);

			G.gradients = 12;
			G.colors = 32;

			G.MandelbrotCountgrid.Clear();
			for (int i = 0; i < pictureBox1.Height; i++) {
				List<int> t = new List<int>();
				for (int j = 0; j < pictureBox1.Width; j++) {
					t.Add(0);
				}
				G.MandelbrotCountgrid.Add(t);
			}
			G.MandelbrotActiveCountgrid = G.MandelbrotCountgrid;

			G.JuliaCountgrid.Clear();
			for (int i = 0; i < pb_julia.Height; i++) {
				List<int> t = new List<int>();
				for (int j = 0; j < pb_julia.Width; j++) {
					t.Add(0);
				}
				G.JuliaCountgrid.Add(t);
			}
			G.JuliaActiveCountgrid = G.JuliaCountgrid;

			G.ConsolebrotCountgrid.Clear();
			for (int i = 0; i < Console.WindowHeight; i++) {
				List<int> t = new List<int>();
				for (int j = 0; j < Console.WindowWidth; j++) {
					t.Add(0);
				}
				G.ConsolebrotCountgrid.Add(t);
			}
			G.ConsolebrotActiveCountgrid = G.ConsolebrotCountgrid;

			Type colorType = typeof(System.Drawing.Color);
			PropertyInfo[] picolors = colorType.GetProperties(BindingFlags.Static | BindingFlags.DeclaredOnly | BindingFlags.Public);
			Array.Reverse(picolors);
			G.knowncolors = new Color[picolors.Length];
			for (int i = 0; i < G.knowncolors.Length; i++) {
				G.knowncolors[i] = (Color)picolors[i].GetValue(null);
			}

			changeColorMode(ColorModes.Random);

			lb_julia_c.Text = "0 + 0i";

			MandelBrotDraw();
			JuliaDraw();

			timer1.Interval = (int)ud_updateinterval.Value;
			timer1.Start();

			pictureBox1.Focus();
		}

		///////////////////////////////////////////// 

		public List<Color> getGradient(List<Color> l, int n) {
			List<Color> ret = new List<Color>();

			for (int i = 1; i < l.Count; i++) {
				ret.Add(l[i - 1]);
				for (double j = n; j > 0; j--) {
					byte r = Convert.ToByte(l[i - 1].R * (j / (n + 1)) + l[i].R * (1 - (j / (n + 1))));
					byte g = Convert.ToByte(l[i - 1].G * (j / (n + 1)) + l[i].G * (1 - (j / (n + 1))));
					byte b = Convert.ToByte(l[i - 1].B * (j / (n + 1)) + l[i].B * (1 - (j / (n + 1))));
					ret.Add(Color.FromArgb(r, g, b));
				}
			}
			ret.Add(l[l.Count - 1]);

			for (double j = n; j > 0; j--) {
				byte r = Convert.ToByte(l[l.Count - 1].R * (j / (n + 1)) + l[0].R * (1 - (j / (n + 1))));
				byte g = Convert.ToByte(l[l.Count - 1].G * (j / (n + 1)) + l[0].G * (1 - (j / (n + 1))));
				byte b = Convert.ToByte(l[l.Count - 1].B * (j / (n + 1)) + l[0].B * (1 - (j / (n + 1))));
				ret.Add(Color.FromArgb(r, g, b));
			}

			return ret;
		}

		public Color getColor(int count) {
			if (count < 0)
				return Color.Black;

			switch (G.ColorMode) {
				case ColorModes.BlackAndWhite:
				case ColorModes.Rainbow:
				case ColorModes.Random:
				case ColorModes.Palette: {
					if (G.gradientcolors.Count > 0)
						return G.gradientcolors[count % G.gradientcolors.Count];
					break;
				}
				default: {
					return Color.White;
				}
			}

			return Color.Red;
		}

		public void changeColorMode(ColorModes cm, bool shuffle = false) {
			G.ColorMode = cm;
			switch (G.ColorMode) {
				case ColorModes.BlackAndWhite: {
					if (G.gradientcolors.Count > 0)
						G.gradientcolors.Clear();

					G.gradients = 0;
					G.palettecolors = new List<Color>(){Color.Black, Color.White};
					G.gradientcolors = getGradient(G.palettecolors, G.gradients);

					break;
				}
				case ColorModes.Rainbow: {
					if (G.gradientcolors.Count > 0)
						G.gradientcolors.Clear();

					if (G.palettecolors.Count > 0)
						G.palettecolors.Clear();

					G.palettecolors = new List<Color>() {
						Color.FromArgb(255, 0, 0), 		 						
						Color.FromArgb(255, 150, 0), 	 
						Color.FromArgb(255, 255, 0), 	 
						Color.FromArgb(105, 255, 0), 	 
						Color.FromArgb(0, 255, 0), 		 
						Color.FromArgb(0, 255, 150), 	 
						Color.FromArgb(0, 255, 255), 	 
						Color.FromArgb(0, 105, 255), 	 
						Color.FromArgb(0, 0, 255), 		 
						Color.FromArgb(150, 0, 255), 	 
						Color.FromArgb(255, 0, 255), 	 
						Color.FromArgb(255, 0, 105)
					};

					G.gradientcolors = getGradient(G.palettecolors, G.gradients);

					break;
				}
				case ColorModes.Random: {
					if (G.gradientcolors.Count > 0)
						G.gradientcolors.Clear();

					if (shuffle) {
						Type colorType = typeof(System.Drawing.Color);
						PropertyInfo[] picolors = colorType.GetProperties(BindingFlags.Static | BindingFlags.DeclaredOnly | BindingFlags.Public);
						Array.Reverse(picolors);
						G.knowncolors = new Color[picolors.Length];
						for (int i = 0; i < G.knowncolors.Length; i++) {
							G.knowncolors[i] = (Color)picolors[i].GetValue(null);
						}

						int n = G.knowncolors.Length;
						while (n > 1) {
							n--;
							int k = rng.Next(n + 1);
							Color c = G.knowncolors[k];
							G.knowncolors[k] = G.knowncolors[n];
							G.knowncolors[n] = c;
						}
					}

					if (G.palettecolors.Count > 0)
						G.palettecolors.Clear();

					for (int i = 0; i < G.colors && i < G.knowncolors.Length; i++) {
						G.palettecolors.Add(G.knowncolors[i]);
					}

					G.gradientcolors = getGradient(G.palettecolors, G.gradients);

					break;
				}
				case ColorModes.Palette: {
					G.gradientcolors = getGradient(G.palettecolors, G.gradients);

					break;
				}
			}

			G.colors = G.palettecolors.Count;

			if (System.Windows.Forms.Application.OpenForms["Form2"] != null) {
				Form2 f2 = System.Windows.Forms.Application.OpenForms["Form2"] as Form2; 
				f2.imageList1.Images.Clear();
				f2.ud_colors.Value = G.colors;

				for (int i = 0; i < Form1.G.palettecolors.Count; i++) {
					Bitmap bmp = new Bitmap(16, 16);
					for (int y = 0; y < 16; y++) {
						for (int x = 0; x < 16; x++) {
							if (y == 0 || y == 15 || x == 0 || x == 15)
								bmp.SetPixel(x, y, Color.Black);
							else
								bmp.SetPixel(x, y, Form1.G.palettecolors[i]);
						}
					}
					f2.imageList1.Images.Add(bmp);
				}
				f2.lb_colorbox.SmallImageList = f2.imageList1;

				f2.lb_colorbox.Clear();
				for (int i = 0; i < Form1.G.palettecolors.Count; i++) {
					ListViewItem lvi = new ListViewItem("");

					lvi.ImageIndex = i;
					lvi.BackColor = Form1.G.palettecolors[i];
					f2.lb_colorbox.Items.Add(lvi);
				}
			}

		}

		public bool MandelbrotGetCount(double xzoom, double yzoom, int ystart, int yend, int xstart, int xend) {
			for (int y = ystart; y < yend; y++) {
				for (int x = xstart; x < xend; x += 4) {
					double realoff = G.xmin + (double)((x / 4) * xzoom);
					double complexoff = G.ymax - ((double)y * yzoom);

					int count = 0;
					double r = 0, i = 0;
					for (; count < G.maxloop; ++count) {
						double a = r * r, b = i * i;
						if (a + b <= 4) {
							i = 2 * r * i + complexoff;
							r = a - b + realoff;
						}
						else
							break;
					}
					try {//?????
						G.MandelbrotCountgrid[y][x / 4] = (count == G.maxloop) ? -1 : count;
					}
					catch {

					}
				}
			}
			return true;
		}
		public void MandelBrotDraw() {
			tb_xmin.Text = G.xmin.ToString();
			tb_xmax.Text = G.xmax.ToString();
			tb_ymin.Text = G.ymin.ToString();
			tb_ymax.Text = G.ymax.ToString();

			var watch = System.Diagnostics.Stopwatch.StartNew();

			double reduction = Convert.ToDouble(ud_reduction.Value);

			int w = (int)((double)pictureBox1.Width / reduction),
				h = (int)((double)pictureBox1.Height / reduction);

			if (System.Windows.Forms.Application.OpenForms["Form3"] != null) {
				Form3 f3 = System.Windows.Forms.Application.OpenForms["Form3"] as Form3;

				w = (int)((double)f3.pictureBox1.Width / reduction);
				h = (int)((double)f3.pictureBox1.Height / reduction);
			}

			double xzoom = (G.xmax - G.xmin) / (double)w;
			double yzoom = (G.ymax - G.ymin) / (double)h;

			bool[] finished = new bool[12];

			for (int tyt = 0; tyt < finished.GetLength(0); tyt++) {
				int yt = tyt;
				(new Thread(() => {
					finished[yt] = MandelbrotGetCount(xzoom, yzoom,
						yt * h / finished.GetLength(0), (yt + 1) * h / finished.GetLength(0),
						0, w * 4);
				})).Start();
			}

			if (G.protectdraw || G.recording) {
				bool done = false;
				while (!done) {
					done = true;
					for (int i = 0; i < finished.GetLength(0); i++) {
						if (!finished[i]) {
							done = false;
							break;
						}
					}
				}
			}

			G.MandelbrotActiveCountgrid = G.MandelbrotCountgrid;

			watch.Stop();

			if (G.recording)
				G.recording_block = false;

			//	Console.WriteLine("time: {0}\n\n", watch.ElapsedMilliseconds);
		}

		public bool JuliaGetCount(double realoff, double complexoff, double xzoom, double yzoom, int ystart, int yend, int xstart, int xend) {
			for (int y = ystart; y < yend; y++) {
				for (int x = xstart; x < xend; x += 4) {
					int count = 0;
					double r = G.jxmin + (double)((x / 4) * xzoom);
					double i = G.jymax - ((double)y * yzoom);
					for (; count < G.maxloop; ++count) {
						double a = r * r, b = i * i;
						if (a + b <= 4) {
							i = 2 * r * i + complexoff;
							r = a - b + realoff;
						}
						else
							break;
					}

					G.JuliaCountgrid[y][x / 4] = (count == G.maxloop) ? -1 : count;
				}
			}
			return true;
		}
		public void JuliaDraw() {
			tb_jxmin.Text = G.jxmin.ToString();
			tb_jxmax.Text = G.jxmax.ToString();
			tb_jymin.Text = G.jymin.ToString();
			tb_jymax.Text = G.jymax.ToString();

			double reduction = Convert.ToDouble(ud_reduction.Value);

			int w = (int)((double)pb_julia.Width / reduction),
				h = (int)((double)pb_julia.Height / reduction);

			double xzoom = (G.jxmax - G.jxmin) / (double)w;
			double yzoom = (G.jymax - G.jymin) / (double)h;

			byte[] buf = new byte[h * w * 4];

			bool[] finished = new bool[4];

			for (int tyt = 0; tyt < finished.GetLength(0); tyt++) {
				int yt = tyt;
				(new Thread(() => {
					finished[yt] = JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom,
						yt * h / finished.GetLength(0), (yt + 1) * h / finished.GetLength(0),
						0, w * 4);
				})).Start();
			}

			bool done = false;
			while (!done) {
				done = true;
				for (int i = 0; i < finished.GetLength(0); i++) {
					if (!finished[i]) {
						done = false;
						break;
					}
				}
			}

			G.JuliaActiveCountgrid = G.JuliaCountgrid;	
		}

		public bool ConsolebrotGetCount(double xzoom, double yzoom, int ystart, int yend, int xstart, int xend) {
			for (int y = ystart; y < yend; y++) {
				for (int x = xstart; x < xend; x += 4) {
					double realoff = G.cxmin + (double)((x / 4) * xzoom);
					double complexoff = G.cymax - ((double)y * yzoom);

					int count = 0;
					double r = 0, i = 0;
					for (; count < G.maxloop; ++count) {
						double a = r * r, b = i * i;
						if (a + b <= 4) {
							i = 2 * r * i + complexoff;
							r = a - b + realoff;
						}
						else
							break;
					}
					try {//?????
						G.ConsolebrotCountgrid[y][x / 4] = (count == G.maxloop) ? -1 : count;
					}
					catch {

					}
				}
			}
			return true;
		}
		public void ConsoleBrotDraw() {
			int w = Console.WindowWidth,
				h = Console.WindowHeight;

			double xzoom = (G.cxmax - G.cxmin) / (double)w;
			double yzoom = (G.cymax - G.cymin) / (double)h;

			bool[] finished = new bool[4];

			for (int tyt = 0; tyt < finished.GetLength(0); tyt++) {
				int yt = tyt;
				(new Thread(() => {
					finished[yt] = ConsolebrotGetCount(xzoom, yzoom,
						yt * h / finished.GetLength(0), (yt + 1) * h / finished.GetLength(0),
						0, w * 4);
				})).Start();
			}

			
			bool done = false;
			while (!done) {
				done = true;
				for (int i = 0; i < finished.GetLength(0); i++) {
					if (!finished[i]) {
						done = false;
						break;
					}
				}
			}

			G.ConsolebrotActiveCountgrid = G.ConsolebrotCountgrid;

			Console.Clear();
			for (int i = 0; i < G.ConsolebrotActiveCountgrid.Count; i++) {
				for (int j = 0; j < G.ConsolebrotActiveCountgrid[i].Count; j++) {
					if (G.ConsolebrotActiveCountgrid[i][j] == -1) 
						Console.Write(" ");
					else {
						List<ConsoleColor> cols = new List<ConsoleColor> {							
							ConsoleColor.DarkCyan,
							ConsoleColor.DarkBlue,							
							ConsoleColor.DarkGreen,
							ConsoleColor.DarkMagenta,
							ConsoleColor.Red,
							ConsoleColor.Yellow,
							ConsoleColor.Green,
							ConsoleColor.Cyan,
							ConsoleColor.Blue,
							ConsoleColor.Magenta,
							ConsoleColor.White,
							ConsoleColor.Gray,
							ConsoleColor.DarkGray,
							ConsoleColor.DarkRed,
							ConsoleColor.DarkYellow,							
						};

						Console.ForegroundColor = G.ConsolebrotActiveCountgrid[i][j] == 0 ? ConsoleColor.Black : cols[G.ConsolebrotActiveCountgrid[i][j] % cols.Count];
						Console.Write((char)(G.ConsolebrotActiveCountgrid[i][j] % 94 + 33));						
					}
				}
			}
		}

		//////////////////////////////////////////////

		private void ud_iterations_ValueChanged(object sender, EventArgs e) {
			G.maxloop = Convert.ToInt32(ud_iterations.Value);
			MandelBrotDraw();
			JuliaDraw();
		}
		private void ud_reduction_ValueChanged(object sender, EventArgs e) {
			MandelBrotDraw();
			JuliaDraw();
		}
		private void ud_updateinterval_ValueChanged(object sender, EventArgs e) {
			timer1.Interval = (int)ud_updateinterval.Value;
		}

		private void ud_reduction_KeyDown(object sender, KeyEventArgs e) {
			if (e.KeyCode == System.Windows.Forms.Keys.Left || e.KeyCode == System.Windows.Forms.Keys.Right
				|| e.KeyCode == System.Windows.Forms.Keys.Up || e.KeyCode == System.Windows.Forms.Keys.Down)
				e.SuppressKeyPress = true;
		}

		private void Form1_SizeChanged(object sender, EventArgs e) {
			Control control = (Control)sender;

			pictureBox1.Width = control.ClientSize.Width - 287;
			pictureBox1.Height = control.ClientSize.Height - 25;

			G.MandelbrotCountgrid.Clear();
			for (int i = 0; i < pictureBox1.Height; i++) {
				List<int> t = new List<int>();
				for (int j = 0; j < pictureBox1.Width; j++) {
					t.Add(0);
				}
				G.MandelbrotCountgrid.Add(t);
			}

			G.protectdraw = true;

			MandelBrotDraw();

			G.protectdraw = false;
		}

		///////////////////////////////////////////////

		private void pictureBox1_MouseHover(object sender, EventArgs e) {
			if (this.Handle == GetForegroundWindow())
				pictureBox1.Focus();
		}
		private void pictureBox1_MouseWheel(object sender, MouseEventArgs e) {
			double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((G.xmax - G.xmin) / pictureBox1.Width) + ((G.xmax + G.xmin) / 2);
			double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((G.ymax - G.ymin) / pictureBox1.Height) + ((G.ymax + G.ymin) / 2);

			if (e.Delta > 0) {
				G.xmin += xoff;
				G.xmax += xoff;
				G.ymin += yoff;
				G.ymax += yoff;

				G.xmin /= 2;
				G.xmax /= 2;
				G.ymin /= 2;
				G.ymax /= 2;
			}
			else {
				G.xmin *= 2;
				G.xmax *= 2;
				G.ymin *= 2;
				G.ymax *= 2;

				G.xmin -= xoff;
				G.xmax -= xoff;
				G.ymin -= yoff;
				G.ymax -= yoff;
			}

			MandelBrotDraw();
		}
		private void pictureBox1_MouseDown(object sender, MouseEventArgs e) {
			pictureBox1.Focus();

			double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((G.xmax - G.xmin) / pictureBox1.Width);
			double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((G.ymax - G.ymin) / pictureBox1.Height);

			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				double xoffoff = xoff + ((G.xmax + G.xmin) / 2);
				double yoffoff = yoff + ((G.ymax + G.ymin) / 2);

				G.jxpos = xoffoff;
				G.jypos = yoffoff;

				lb_julia_c.Text = G.jxpos.ToString() + " + " + G.jypos.ToString() + "i";

				G.jxmin = -3; G.jxmax = 3;
				G.jymin = -2; G.jymax = 2;
				JuliaDraw();
			}

			if (e.Button == MouseButtons.Left) {
				int reduction = Convert.ToInt32(ud_reduction.Value);
				G.spanx = e.Location.X / reduction;
				G.spany = e.Location.Y / reduction;
				G.panx = 0;
				G.pany = 0;

				G.sxoff = xoff;
				G.syoff = yoff;
			}
		}
		private void pictureBox1_MouseUp(object sender, MouseEventArgs e) {
			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((G.xmax - G.xmin) / pictureBox1.Width);
				double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((G.ymax - G.ymin) / pictureBox1.Height);

				double xoff1 = xoff - G.sxoff;
				double yoff1 = yoff - G.syoff;

				G.xmin -= xoff1;
				G.xmax -= xoff1;
				G.ymin -= yoff1;
				G.ymax -= yoff1;

				tb_xmin.Text = G.xmin.ToString();
				tb_xmax.Text = G.xmax.ToString();
				tb_ymin.Text = G.ymin.ToString();
				tb_ymax.Text = G.ymax.ToString();

				double reduction = Convert.ToDouble(ud_reduction.Value);
				int w = (int)((double)pictureBox1.Width / reduction),
					h = (int)((double)pictureBox1.Height / reduction);

				if (Math.Abs(G.panx) >= w || Math.Abs(G.pany) >= h) {
					MandelBrotDraw();
					G.spanx = 0;
					G.spany = 0;
					G.panx = 0;
					G.pany = 0;
					return;
				}

				double xzoom = (G.xmax - G.xmin) / (double)w;
				double yzoom = (G.ymax - G.ymin) / (double)h;

				List<List<int>> g = new List<List<int>>();
				for (int i = 0; i < G.MandelbrotActiveCountgrid.Count; i++) {
					List<int> c = new List<int>();
					for (int j = 0; j < G.MandelbrotActiveCountgrid[i].Count; j++) {
						c.Add(-1);
					}
					g.Add(c);
				}

				if (G.panx <= 0 && G.pany <= 0) {
					for (int i = 0; i < h + G.pany; i++) {
						for (int j = 0; j < w + G.panx; j++) {
							g[i][j] = G.MandelbrotActiveCountgrid[i - G.pany][j - G.panx];
						}
					}
					G.MandelbrotCountgrid = g;

					MandelbrotGetCount(xzoom, yzoom, h + G.pany, h, 0, (w + G.panx) * 4);
					MandelbrotGetCount(xzoom, yzoom, h + G.pany, h, (w + G.panx) * 4, w * 4);
					MandelbrotGetCount(xzoom, yzoom, 0, h + G.pany, (w + G.panx) * 4, w * 4);
				}
				else if (G.panx > 0 && G.pany < 0) {
					for (int i = 0; i < h + G.pany; i++) {
						for (int j = G.panx; j < w; j++) {
							g[i][j] = G.MandelbrotActiveCountgrid[i - G.pany][j - G.panx];
						}
					}
					G.MandelbrotCountgrid = g;

					MandelbrotGetCount(xzoom, yzoom, 0, h + G.pany, 0, G.panx * 4); 
					MandelbrotGetCount(xzoom, yzoom, h + G.pany, h, 0, G.panx * 4);
					MandelbrotGetCount(xzoom, yzoom, h + G.pany, h, G.panx * 4, w * 4);
				}
				else if (G.panx < 0 && G.pany > 0) {
					for (int i = G.pany; i < h; i++) {
						for (int j = 0; j < w + G.panx; j++) {
							g[i][j] = G.MandelbrotActiveCountgrid[i - G.pany][j - G.panx];
						}
					}
					G.MandelbrotCountgrid = g;

					MandelbrotGetCount(xzoom, yzoom, 0, G.pany, 0, (w + G.panx) * 4);
					MandelbrotGetCount(xzoom, yzoom, 0, G.pany, (w + G.panx) * 4, w * 4);
					MandelbrotGetCount(xzoom, yzoom, G.pany, h, (w + G.panx) * 4, w * 4);
				}
				else {
					for (int i = G.pany; i < h; i++) {
						for (int j = G.panx; j < w; j++) {
							g[i][j] = G.MandelbrotActiveCountgrid[i - G.pany][j - G.panx];
						}
					}
					G.MandelbrotCountgrid = g;

					MandelbrotGetCount(xzoom, yzoom, G.pany, h, 0, G.panx * 4);
					MandelbrotGetCount(xzoom, yzoom, 0, G.pany, 0, G.panx * 4);
					MandelbrotGetCount(xzoom, yzoom, 0, G.pany, G.panx * 4, w * 4);
				}

				G.MandelbrotActiveCountgrid = G.MandelbrotCountgrid;
			}

			if (G.recording) {
				G.recording_block = false;
			}

			G.spanx = 0;
			G.spany = 0;
			G.panx = 0;
			G.pany = 0;
		}
		private void pictureBox1_MouseMove(object sender, MouseEventArgs e) {
			double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((G.xmax - G.xmin) / pictureBox1.Width);// +((G.xmax + G.xmin) / 2);
			double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((G.ymax - G.ymin) / pictureBox1.Height);// +((G.ymax + G.ymin) / 2);
			double xoffoff = xoff + ((G.xmax + G.xmin) / 2);
			double yoffoff = yoff + ((G.ymax + G.ymin) / 2);

			lb_mandelbrotxpos.Text = xoffoff.ToString();
			lb_mandelbrotypos.Text = yoffoff.ToString();

			//lb_mandelbrotxpos.Text = e.Location.X.ToString();
			//lb_mandelbrotypos.Text = e.Location.Y.ToString();

			if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_MENU) < 0) {
				G.MandelbrotEscapePoints.Clear();
				G.MandelbrotEscapePoints.Add(new Complex(xoffoff, yoffoff));

				double reductionf = Convert.ToDouble(ud_reduction.Value);

				int w = (int)((double)pictureBox1.Width / reductionf),
					h = (int)((double)pictureBox1.Height / reductionf);

				if (System.Windows.Forms.Application.OpenForms["Form3"] != null) {
					Form3 f3 = System.Windows.Forms.Application.OpenForms["Form3"] as Form3;

					w = (int)((double)f3.pictureBox1.Width / reductionf);
					h = (int)((double)f3.pictureBox1.Height / reductionf);
				}

				double xzoom = (G.xmax - G.xmin) / (double)w;
				double yzoom = (G.ymax - G.ymin) / (double)h;

				double realoff = xoffoff;
				double complexoff = yoffoff;

				int count = 0;
				double r = 0, i = 0;
				for (; count < G.maxloop; ++count) {
					double a = r * r, b = i * i;
					if (a + b <= 4) {
						i = 2 * r * i + complexoff;
						r = a - b + realoff;

						G.MandelbrotEscapePoints.Add(new Complex(r, i));
					}
					else
						break;
				}

				//if (count == G.maxloop) {
				//	G.MandelbrotEscapePoints.Clear();
				//}
			}


			//if (e.Button == System.Windows.Forms.MouseButtons.Right) {
			//	G.jxpos = xoffoff;
			//	G.jypos = yoffoff;

			//	lb_julia_c.Text = G.jxpos.ToString() + " + " + G.jypos.ToString() + "i";

			//	G.jxmin = -3; G.jxmax = 3;
			//	G.jymin = -2; G.jymax = 2;

			//	JuliaDraw();
			//}

			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				int reduction = Convert.ToInt32(ud_reduction.Value);

				G.panx = (e.Location.X / reduction) - G.spanx;
				G.pany = (e.Location.Y / reduction) - G.spany;
			}

			if (this.Handle == GetForegroundWindow() && GetFocus() != pictureBox1.Handle) {
				pictureBox1.Focus();
			}
		}

		private void pb_julia_MouseHover(object sender, EventArgs e) {
			if (this.Handle == GetForegroundWindow())
				pb_julia.Focus();
		}
		private void pb_julia_MouseWheel(object sender, MouseEventArgs e) {
			double xoff = (e.Location.X - (pb_julia.Width / 2)) * ((G.jxmax - G.jxmin) / pb_julia.Width) + ((G.jxmax + G.jxmin) / 2);
			double yoff = ((pb_julia.Height / 2) - e.Location.Y) * ((G.jymax - G.jymin) / pb_julia.Height) + ((G.jymax + G.jymin) / 2);

			if (e.Delta > 0) {
				G.jxmin += xoff;
				G.jxmax += xoff;
				G.jymin += yoff;
				G.jymax += yoff;

				G.jxmin /= 2;
				G.jxmax /= 2;
				G.jymin /= 2;
				G.jymax /= 2;
			}
			else {
				G.jxmin *= 2;
				G.jxmax *= 2;
				G.jymin *= 2;
				G.jymax *= 2;

				G.jxmin -= xoff;
				G.jxmax -= xoff;
				G.jymin -= yoff;
				G.jymax -= yoff;
			}

			JuliaDraw();
		}
		private void pb_julia_MouseDown(object sender, MouseEventArgs e) {
			pb_julia.Focus();

			double xoff = (e.Location.X - (pb_julia.Width / 2)) * ((G.jxmax - G.jxmin) / pb_julia.Width);
			double yoff = ((pb_julia.Height / 2) - e.Location.Y) * ((G.jymax - G.jymin) / pb_julia.Height);

			if (e.Button == MouseButtons.Left) {
				int reduction = Convert.ToInt32(ud_reduction.Value);
				G.jspanx = e.Location.X / reduction;
				G.jspany = e.Location.Y / reduction;
				G.jpanx = 0;
				G.jpany = 0;

				G.jsxoff = xoff;
				G.jsyoff = yoff;
			}
		}
		private void pb_julia_MouseUp(object sender, MouseEventArgs e) {
			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				double xoff = (e.Location.X - (pb_julia.Width / 2)) * ((G.jxmax - G.jxmin) / pb_julia.Width);
				double yoff = ((pb_julia.Height / 2) - e.Location.Y) * ((G.jymax - G.jymin) / pb_julia.Height);

				double xoff1 = xoff - G.jsxoff;
				double yoff1 = yoff - G.jsyoff;

				G.jxmin -= xoff1;
				G.jxmax -= xoff1;
				G.jymin -= yoff1;
				G.jymax -= yoff1;
			
				tb_jxmin.Text = G.jxmin.ToString();
				tb_jxmax.Text = G.jxmax.ToString();
				tb_jymin.Text = G.jymin.ToString();
				tb_jymax.Text = G.jymax.ToString();

				double reduction = Convert.ToDouble(ud_reduction.Value);
				int w = (int)((double)pb_julia.Width / reduction),
					h = (int)((double)pb_julia.Height / reduction);

				if (Math.Abs(G.jpanx) >= w || Math.Abs(G.jpany) >= h) {
					JuliaDraw();
					G.jspanx = 0;
					G.jspany = 0;
					G.jpanx = 0;
					G.jpany = 0;
					return;
				}

				double xzoom = (G.jxmax - G.jxmin) / (double)w;
				double yzoom = (G.jymax - G.jymin) / (double)h;

				List<List<int>> g = new List<List<int>>();
				for (int i = 0; i < G.JuliaActiveCountgrid.Count; i++) {
					List<int> c = new List<int>();
					for (int j = 0; j < G.JuliaActiveCountgrid[i].Count; j++) {
						c.Add(-1);
					}
					g.Add(c);
				}

				if (G.jpanx <= 0 && G.jpany <= 0) {
					for (int i = 0; i < h + G.jpany; i++) {
						for (int j = 0; j < w + G.jpanx; j++) {
							g[i][j] = G.JuliaActiveCountgrid[i - G.jpany][j - G.jpanx];
						}
					}
					G.JuliaCountgrid = g;

					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, h + G.jpany, h, 0, (w + G.jpanx) * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, h + G.jpany, h, (w + G.jpanx) * 4, w * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, 0, h + G.jpany, (w + G.jpanx) * 4, w * 4);
				}
				else if (G.jpanx > 0 && G.jpany < 0) {
					for (int i = 0; i < h + G.jpany; i++) {
						for (int j = G.jpanx; j < w; j++) {
							g[i][j] = G.JuliaActiveCountgrid[i - G.jpany][j - G.jpanx];
						}
					}
					G.JuliaCountgrid = g;

					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, 0, h + G.jpany, 0, G.jpanx * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, h + G.jpany, h, 0, G.jpanx * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, h + G.jpany, h, G.jpanx * 4, w * 4);
				}
				else if (G.jpanx < 0 && G.jpany > 0) {
					for (int i = G.jpany; i < h; i++) {
						for (int j = 0; j < w + G.jpanx; j++) {
							g[i][j] = G.JuliaActiveCountgrid[i - G.jpany][j - G.jpanx];
						}
					}
					G.JuliaCountgrid = g;

					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, 0, G.jpany, 0, (w + G.jpanx) * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, 0, G.jpany, (w + G.jpanx) * 4, w * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, G.jpany, h, (w + G.jpanx) * 4, w * 4);
				}
				else {
					for (int i = G.jpany; i < h; i++) {
						for (int j = G.jpanx; j < w; j++) {
							g[i][j] = G.JuliaActiveCountgrid[i - G.jpany][j - G.jpanx];
						}
					}
					G.JuliaCountgrid = g;

					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, G.jpany, h, 0, G.jpanx * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, 0, G.jpany, 0, G.jpanx * 4);
					JuliaGetCount(G.jxpos, G.jypos, xzoom, yzoom, 0, G.jpany, G.jpanx * 4, w * 4);
				}

				G.JuliaActiveCountgrid = G.JuliaCountgrid;
			}
			G.jspanx = 0;
			G.jspany = 0;
			G.jpanx = 0;
			G.jpany = 0;
		}
		private void pb_julia_MouseMove(object sender, MouseEventArgs e) {
			double xoff = (e.Location.X - (pb_julia.Width / 2)) * ((G.jxmax - G.jxmin) / pb_julia.Width);
			double yoff = ((pb_julia.Height / 2) - e.Location.Y) * ((G.jymax - G.jymin) / pb_julia.Height);
			double xoffoff = xoff + ((G.jxmax + G.jxmin) / 2);
			double yoffoff = yoff + ((G.jymax + G.jymin) / 2);

			lb_juliaxpos.Text = xoffoff.ToString();
			lb_juliaypos.Text = yoffoff.ToString();

			if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_MENU) < 0) {
				G.JuliaEscapePoints.Clear();
				G.JuliaEscapePoints.Add(new Complex(xoffoff, yoffoff));

				double reductionf = Convert.ToDouble(ud_reduction.Value);

				int w = (int)((double)pb_julia.Width / reductionf),
					h = (int)((double)pb_julia.Height / reductionf);

				double xzoom = (G.jxmax - G.jxmin) / (double)w;
				double yzoom = (G.jymax - G.jymin) / (double)h;

				double realoff = G.jxpos;
				double complexoff = G.jypos;
				
				int count = 0;
				double r = xoff;
				double i = yoff;
				for (; count < G.maxloop; ++count) {
					double a = r * r, b = i * i;
					if (a + b <= 4) {
						i = 2 * r * i + complexoff;
						r = a - b + realoff;

						G.JuliaEscapePoints.Add(new Complex(r, i));
					}
					else
						break;
				}

				//if (count == G.maxloop) {
				//	G.JuliaEscapePoints.Clear();
				//}
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				int reduction = Convert.ToInt32(ud_reduction.Value);

				G.jpanx = (e.Location.X / reduction) - G.jspanx;
				G.jpany = (e.Location.Y / reduction) - G.jspany;
			}
		}

		private void timer1_Tick(object sender, EventArgs e) {
			if (GetKeyState(2) < 0 && pictureBox1.ClientRectangle.Contains(PointToClient(Control.MousePosition))) {
				Point ptt = pictureBox1.PointToClient(Cursor.Position);

				double xoff = (ptt.X - (pictureBox1.Width / 2)) * ((G.xmax - G.xmin) / pictureBox1.Width);// +((G.xmax + G.xmin) / 2);
				double yoff = ((pictureBox1.Height / 2) - ptt.Y) * ((G.ymax - G.ymin) / pictureBox1.Height);// +((G.ymax + G.ymin) / 2);
				double xoffoff = xoff + ((G.xmax + G.xmin) / 2);
				double yoffoff = yoff + ((G.ymax + G.ymin) / 2);

				lb_mandelbrotxpos.Text = xoffoff.ToString();
				lb_mandelbrotypos.Text = yoffoff.ToString();

				G.jxpos = xoffoff;
				G.jypos = yoffoff;

				lb_julia_c.Text = G.jxpos.ToString() + " + " + G.jypos.ToString() + "i";

				JuliaDraw();
			}

			updatemandelbrot();
			updatejuliaset();

			if (G.recording && !G.recording_block) {
				G.recording_block = true;
				string path = getexedir() + "\\recordings";
				System.IO.Directory.CreateDirectory(path);
				string s = " ___" + G.xmin.ToString() + " " + G.xmax.ToString() + " " + G.ymin.ToString() + " " + G.ymax.ToString();

				CaptureImage im = new CaptureImage();
				im.img = (Image)pictureBox1.Image.Clone();
				im.name = path + "\\" + (G.imagecounter++).ToString() + s + ".bmp";

				G.capturelist.Add(im);				
			}
		}

		private void updatemandelbrot() {
			double reduction = Convert.ToDouble(ud_reduction.Value);

			int h = (int)((double)G.MandelbrotActiveCountgrid.Count / reduction);
			int w;
			if (h > 0)
				w = (int)((double)G.MandelbrotActiveCountgrid[0].Count / reduction);
			else return;

			byte[] buf = new byte[w * h * 4];

			if (G.cycle) {
				if (G.cycledown) {
					Color end = G.gradientcolors[G.gradientcolors.Count - 1];
					G.gradientcolors.RemoveAt(G.gradientcolors.Count - 1);
					G.gradientcolors.Insert(0, end);
				}
				else {
					Color first = G.gradientcolors[0];
					G.gradientcolors.RemoveAt(0);
					G.gradientcolors.Add(first);
				}
			}

			int px = G.panx * 4;
			int lj = -px;
			int uj = (w * 4) - px;

			for (int i = 0; i < h; i++) {
				for (int j = lj; j < uj; j += 4) {
					int pos = (i + G.pany) * 4 * w + j + px;
					if (pos >= w * h * 4 || pos < 0 || j >= w * 4 || j < 0)
						continue;
					int count = G.MandelbrotActiveCountgrid[i][j / 4];
					Color c = new Color();

					if (G.edge) {
						if (i != h - 1 && w != j / 4 + 1
							&& i != 0 && j != 0) {
							if (count != G.MandelbrotActiveCountgrid[i][j / 4 + 1]
								|| count != G.MandelbrotActiveCountgrid[i + 1][j / 4]
								|| count != G.MandelbrotActiveCountgrid[i][j / 4 - 1]
								|| count != G.MandelbrotActiveCountgrid[i - 1][j / 4])
								c = getColor(count);
							else
								c = Color.Black;
						}
						else
							c = getColor(count);
					}
					else
						c = getColor(count);

					buf[pos + 2] = c.R;
					buf[pos + 1] = c.G;
					buf[pos] = c.B;
				}
			}

			unsafe {
				fixed (byte* ptr = buf) {
					IntPtr iptr = new IntPtr(ptr);
					Bitmap bmp = new Bitmap(w, h, w * 4, System.Drawing.Imaging.PixelFormat.Format32bppRgb, iptr);
					Bitmap act = new Bitmap(bmp, (int)((double)w * reduction), (int)((double)h * reduction));

					RECT rc = getwindowrect(pictureBox1.Handle);
					if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_MENU) < 0 && PtInRect(out rc, getcursorpos())) {
						using (var graphics = Graphics.FromImage(act)) {
							Pen whitePen = new Pen(Color.White, 1);

							for (int i = 0; i < G.MandelbrotEscapePoints.Count - 1; i++) {

								int r1 = (int)(((w * G.MandelbrotEscapePoints[i].r) / (G.xmax - G.xmin)) + (w / 2) - ((G.xmax + G.xmin) / 2));
								int i1 = (int)-(((h * G.MandelbrotEscapePoints[i].i) / (G.ymax - G.ymin)) - (h / 2));
								int r2 = (int)(((w * G.MandelbrotEscapePoints[i + 1].r) / (G.xmax - G.xmin)) + (w / 2) - ((G.xmax + G.xmin) / 2));
								int i2 = (int)-(((h * G.MandelbrotEscapePoints[i + 1].i) / (G.ymax - G.ymin)) - (h / 2));

								graphics.DrawLine(whitePen, r1, i1, r2, i2);
							}
						}
					}

					if (System.Windows.Forms.Application.OpenForms["Form3"] != null) {
						Form3 f3 = System.Windows.Forms.Application.OpenForms["Form3"] as Form3;
						f3.pictureBox1.Image = act;
					}
					else
						pictureBox1.Image = act;
				}
			}
		}

		private void updatejuliaset() {
			double reduction = Convert.ToDouble(ud_reduction.Value);

			int h = (int)((double)G.JuliaActiveCountgrid.Count / reduction);
			int w;
			if (h > 0)
				w = (int)((double)G.JuliaActiveCountgrid[0].Count);
			else return;
			w = (int)((double)G.JuliaActiveCountgrid[0].Count);


			byte[] buf = new byte[w * h * 4];

			int px = G.jpanx * 4;
			int lj = -px;
			int uj = (w * 4) - px;

			for (int i = 0; i < h; i++) {
				for (int j = lj; j < uj; j += 4) {
					int pos = (i + G.jpany) * 4 * w + j + px;
					if (pos >= w * h * 4 || pos < 0 || j >= w * 4 || j < 0)
						continue;
					int count = G.JuliaActiveCountgrid[i][j / 4];
					Color c = new Color();

					if (G.edge) {
						if (i != h - 1 && w != j / 4 + 1
							&& i != 0 && j != 0) {
							if (count != G.JuliaActiveCountgrid[i][j / 4 + 1]
								|| count != G.JuliaActiveCountgrid[i + 1][j / 4]
								|| count != G.JuliaActiveCountgrid[i][j / 4 - 1]
								|| count != G.JuliaActiveCountgrid[i - 1][j / 4])
								c = getColor(count);
							else
								c = Color.Black;
						}
					}
					else
						c = getColor(count);

					buf[pos + 2] = c.R;
					buf[pos + 1] = c.G;
					buf[pos] = c.B;
				}
			}

			unsafe {
				fixed (byte* ptr = buf) {
					IntPtr iptr = new IntPtr(ptr);
					Bitmap bmp = new Bitmap(w, h, w * 4, System.Drawing.Imaging.PixelFormat.Format32bppRgb, iptr);
					Bitmap act = new Bitmap(bmp, (int)((double)w * reduction), (int)((double)h * reduction));

					RECT rc = getwindowrect(pb_julia.Handle);
					if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_MENU) < 0 && PtInRect(out rc, getcursorpos())) {
						using (var graphics = Graphics.FromImage(act)) {
							Pen whitePen = new Pen(Color.White, 1);

							for (int i = 0; i < G.JuliaEscapePoints.Count - 1; i++) {

								int r1 = (int)(((w * G.JuliaEscapePoints[i].r) / (G.jxmax - G.jxmin)) + (w / 2) - ((G.jxmax + G.jxmin) / 2));
								int i1 = (int)-(((h * G.JuliaEscapePoints[i].i) / (G.jymax - G.jymin)) - (h / 2));
								int r2 = (int)(((w * G.JuliaEscapePoints[i + 1].r) / (G.jxmax - G.jxmin)) + (w / 2) - ((G.jxmax + G.jxmin) / 2));
								int i2 = (int)-(((h * G.JuliaEscapePoints[i + 1].i) / (G.jymax - G.jymin)) - (h / 2));

								graphics.DrawLine(whitePen, r1, i1, r2, i2);
							}
						}
					}

					pb_julia.Image = act;
				}
			}
		}

		private void btn_showcontrols_Click(object sender, EventArgs e)  {
			string str = 
@"Scroll Wheel - Zoom in/out of Mandelbrot/Julia set

Left Click - Sets C value for Julia set and resets its veiwing window
	(hold down and move to pan Mandelbrot/Julia set)

Right Click - Sets C value for Julia set without reseting veiwing window 
	(hold down and move mouse for fast julia drawing)

Shift + Alt - Display fate lines at cursor position on Mandelbrot/Julia set

Space - reset mandelbrot and julia set view window

N - Set coloring mode to default
B - Set coloring mode to black and white
R - Set coloring mode to rainbow
X - Generate a random color palette
				
C - Toggle color cycling
D - Change color cycling direction
E - Toggle edge detection

F - Toggle Full Screen

J - Show/Hide ConsoleBrot (left click to zoom)

ALT + Enter - Start/Stop recording actions (pans and zooms)

Up/Down Arrow - increase/decrease # of max iterations
Righ/Left Arrow - increase/decrease resolution
CTRL + Up/Down Arrow - increase/decrease # of gradients
CTRL + Right/Left Arrow - increase/decrease # of colors
SHIFT + Up/Down Arrow - increase/decrease # of ms to update display
";

			MessageBox.Show(str, "Controls and Features");
		}

		private void toggle_recording() {
			G.recording = !G.recording;
			if (!G.recording) {
				btn_record.Enabled = false;
				btn_record.Text = "Start Recording";

				AviManager aviManager = new AviManager("recordings\\" + G.recording_dir + ".avi", false);
				VideoStream aviStream = aviManager.AddVideoStream(true, 2, (Bitmap)G.capturelist[0].img);

				for (int i = 0; i < G.capturelist.Count; i++) {
					aviStream.AddFrame((Bitmap)G.capturelist[i].img);
					G.capturelist[i].img.Dispose();
				}
				aviManager.Close();

				G.capturelist.Clear();
				btn_record.Enabled = true;
			}
			else {
				G.capturelist.Clear();
				btn_record.Text = "Stop Recording";
				G.recording_dir = DateTime.Now.ToString(@"MM_dd_yyyy h_mm_ss tt");
				G.imagecounter = 0;
				MandelBrotDraw();
				G.recording_block = false;
			}
		}
		private void Record_Click(object sender, EventArgs e) {
			toggle_recording();
		}

		private void button1_Click(object sender, EventArgs e) {
			Form3 f3 = new Form3();
			f3.Show();

			G.MandelbrotCountgrid.Clear();
			for (int i = 0; i < f3.pictureBox1.Height; i++) {
				List<int> t = new List<int>();
				for (int j = 0; j < f3.pictureBox1.Width; j++) {
					t.Add(0);
				}
				G.MandelbrotCountgrid.Add(t);
			}

			MandelBrotDraw();
		}

		private void btn_palette_Click(object sender, EventArgs e) {
			if (System.Windows.Forms.Application.OpenForms["Form2"] == null) {
				Form2 f2 = new Form2();
				f2.Show();
			}
			else
				(System.Windows.Forms.Application.OpenForms["Form2"] as Form2).Focus();
		}

		private void Form1_Click(object sender, EventArgs e) {
			G.maxloop = Convert.ToInt32(ud_iterations.Value);
			timer1.Interval = (int)ud_updateinterval.Value;
		
			pictureBox1.Focus();
		}

		private void Form1_FormClosed(object sender, FormClosedEventArgs e) {
			UnhookWindowsHookEx(kbhook);
		}
		
		public static int mproc(int nCode, IntPtr wParam, IntPtr lParam) {
			MSLLHOOKSTRUCT mouse = (MSLLHOOKSTRUCT)Marshal.PtrToStructure(lParam, typeof(MSLLHOOKSTRUCT));

			Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

			//if ((int)wParam == WM_MOUSEMOVE) {
			//	IntPtr handle = GetConsoleWindow();
			//	RECT rcclient = getmappedclientrect(handle);
			//	POINT pt = getclientcursorpos(handle);
			//	int xpos = pt.x / ((rcclient.right - rcclient.left) / Console.WindowWidth) + 1;
			//	int ypos = pt.y / ((rcclient.bottom - rcclient.top) / Console.WindowHeight) + 1;

			//	//f1.lb_mandelbrotxpos.Text = xpos.ToString();
			//	//f1.lb_mandelbrotypos.Text = ypos.ToString();
			//}

			if (((int)wParam == WM_LBUTTONUP || (int)wParam == WM_RBUTTONUP) && (GetConsoleWindow() == GetForegroundWindow())) {
				if (G.console) {
					IntPtr handle = GetConsoleWindow();
					RECT rcclient = getmappedclientrect(handle);

					if (PtInRect(out rcclient, mouse.pt)) {
						POINT pt = getclientcursorpos(handle);

						double xoff = ((double)pt.x - ((rcclient.right - rcclient.left) / 2)) * ((G.cxmax - G.cxmin) / (rcclient.right - rcclient.left)) + ((G.cxmax + G.cxmin) / 2);
						double yoff = (((rcclient.bottom - rcclient.top) / 2) - (double)pt.y) * ((G.cymax - G.cymin) / (rcclient.bottom - rcclient.top)) + ((G.cymax + G.cymin) / 2);

						if ((int)wParam == WM_LBUTTONUP) {
							G.cxmin += xoff;
							G.cxmax += xoff;
							G.cymin += yoff;
							G.cymax += yoff;

							G.cxmin /= 2;
							G.cxmax /= 2;
							G.cymin /= 2;
							G.cymax /= 2;
						}
						if ((int)wParam == WM_RBUTTONUP) {
							G.cxmin *= 2;
							G.cxmax *= 2;
							G.cymin *= 2;
							G.cymax *= 2;

							G.cxmin -= xoff;
							G.cxmax -= xoff;
							G.cymin -= yoff;
							G.cymax -= yoff;
						}

						f1.ConsoleBrotDraw();
					}
				}
			}

			return CallNextHookEx(kbhook, nCode, wParam, lParam);
		}

		public static int kbproc(int nCode, IntPtr wParam, IntPtr lParam) {
			KBDLLHOOKSTRUCT kb = (KBDLLHOOKSTRUCT)Marshal.PtrToStructure(lParam, typeof(KBDLLHOOKSTRUCT));
			if (nCode >= 0) {
				IntPtr foreground = GetForegroundWindow();
				Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;
				Form2 f2 = null;
				if (System.Windows.Forms.Application.OpenForms["Form2"] != null) 
					f2 = System.Windows.Forms.Application.OpenForms["Form2"] as Form2;
				Form3 f3 = null;
				if (System.Windows.Forms.Application.OpenForms["Form3"] != null)
					f3 = System.Windows.Forms.Application.OpenForms["Form3"] as Form3;
				
				if (f1.Handle != foreground 
					&& (f2 == null ? true : f2.Handle != foreground) 
					&& (f3 == null ? true : f3.Handle != foreground)
					&& (GetConsoleWindow() == null ? true : GetConsoleWindow() != foreground))
					return CallNextHookEx(kbhook, nCode, wParam, lParam);

				if ((int)wParam == WM_KEYDOWN || (int)wParam == WM_SYSKEYDOWN) {
					switch ((Keys)kb.vkCode) {
						case Keys.A: {
							G.xmin = -.372156600952148;
							G.xmax = -.371424179077148;
							G.ymin = .665984293619792;
							G.ymax = .666472574869792;
							f1.MandelBrotDraw();

							if (G.console) {
								G.cxmin = -.372156600952148;
								G.cxmax = -.371424179077148;
								G.cymin = .665984293619792;
								G.cymax = .666472574869792;
								f1.ConsoleBrotDraw();
							}

							break;
						}
						case Keys.S: {
							G.xmin = -1.74988889452702;
							G.xmax = -1.74988889417774;
							G.ymin = 1.45436418946456e-05;
							G.ymax = 1.45438723969832e-05;
							f1.MandelBrotDraw();

							if (G.console) {
								G.cxmin = -1.74988889452702;
								G.cxmax = -1.74988889417774;
								G.cymin = 1.45436418946456e-05;
								G.cymax = 1.45438723969832e-05;
								f1.ConsoleBrotDraw();
							}

							break;
						}
						case Keys.Space: {
							G.xmin = -3; G.xmax = 3; G.ymin = -2; G.ymax = 2;
							G.jxmin = -3; G.jxmax = 3; G.jymin = -2; G.jymax = 2;
							f1.JuliaDraw();
							f1.MandelBrotDraw();

							if (G.console) {
								G.cxmin = -3; G.cxmax = 3; G.cymin = -2; G.cymax = 2;
								f1.ConsoleBrotDraw();
							}
							
							break;
						}
						case Keys.N: {
							Type colorType = typeof(System.Drawing.Color);
							PropertyInfo[] picolors = colorType.GetProperties(BindingFlags.Static | BindingFlags.DeclaredOnly | BindingFlags.Public);
							Array.Reverse(picolors);
							G.knowncolors = new Color[picolors.Length];
							for (int i = 0; i < G.knowncolors.Length; i++) {
								G.knowncolors[i] = (Color)picolors[i].GetValue(null);
							}
							f1.changeColorMode(ColorModes.Random);

							break;
						}
						case Keys.B: {
							if (f2 != null) {
								f2.ud_gradients.Value = 0;
								f2.rb_blackwhite.Select();
							}
							else
								f1.changeColorMode(ColorModes.BlackAndWhite);

							break;
						}
						case Keys.R: {
							if (f2 != null) {
								f2.rb_rainbow.Select();
							}
							else
								f1.changeColorMode(ColorModes.Rainbow);

							break;
						}
						case Keys.X: {
							if (f2 != null) {
								f2.rb_default.Select();
								f2.rb_random.Select();
							}
							else
								f1.changeColorMode(ColorModes.Random, true);

							break;
						}
						case Keys.E: {
							if (f2 != null) {
								f2.cb_edgedetect.CheckState = G.edge ? CheckState.Unchecked : CheckState.Checked;
							}
							else
								G.edge = !G.edge;

							break;
						}
						case Keys.D: {
							G.cycledown = !G.cycledown;

							break;
						}
						case Keys.C: {
							if (f2 != null) {
								f2.cb_cyclecolors.CheckState = G.cycle ? CheckState.Unchecked : CheckState.Checked;
							}
							else
								G.cycle = !G.cycle;

							break;
						}
						case Keys.F: {
							if (f3 != null) {
								f3.Close();
							}
							else {
								Form3 nf3 = new Form3();
								nf3.Show();

								G.MandelbrotCountgrid.Clear();
								for (int i = 0; i < nf3.pictureBox1.Height; i++) {
									List<int> t = new List<int>();
									for (int j = 0; j < nf3.pictureBox1.Width; j++) {
										t.Add(0);
									}
									G.MandelbrotCountgrid.Add(t);
								}

								f1.MandelBrotDraw();
							}
							break;
						}
						case Keys.Escape: {
							if (f3 != null) {
								f3.Close();
							}

							break;
						}
						case Keys.J: {
							G.console = !G.console;

							ShowWindow(GetConsoleWindow(), G.console ? SW_MAXIMIZE : SW_HIDE);
					//		if (G.console) ShowWindow(GetConsoleWindow(), 1);

							if (G.console) {
								f1.mProc = new HookProc(mproc);
								mhook = SetWindowsHookEx(WH_MOUSE_LL, f1.mProc, (IntPtr)0, 0);
								f1.ConsoleBrotDraw();
							}
							else 
								UnhookWindowsHookEx(mhook);

							break;
						}
						case Keys.Q: {
							RECT rc = getmappedclientrect(f1.pictureBox1.Handle);
							SetCursorPos((rc.right - rc.left) / 2 + rc.left, (rc.bottom - rc.top) / 2 + rc.top);
							break;
						}
						case Keys.Enter: {
							if (GetAsyncKeyState(VK_MENU) < 0)
								f1.toggle_recording();
							break;
						}
						case Keys.LShiftKey:
						case Keys.LMenu: {
							if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_MENU) < 0) {
								if (G.xmin != -3 || G.xmax != 3 || G.ymin != -2 || G.ymax != 2) {
									G.xmin = -3; G.xmax = 3; G.ymin = -2; G.ymax = 2;
									f1.MandelBrotDraw();
								}
								if (G.jxmin != -3 || G.jxmax != 3 || G.jymin != -2 || G.jymax != 2) {
									G.jxmin = -3; G.jxmax = 3; G.jymin = -2; G.jymax = 2;
									f1.JuliaDraw();
								}
							}
							break;
						}
					}
					
					if (GetKeyState(VK_CONTROL) < 0) {
						if (kb.vkCode == (int)Keys.Up || kb.vkCode == (int)Keys.Down) {
							//gradients
							G.gradients = kb.vkCode == (int)Keys.Up ? G.gradients + 1 : G.gradients - 1 < 0 ? 0 : G.gradients - 1;

							if (f2 != null)
								f2.ud_gradients.Value = G.gradients;

							f1.changeColorMode(G.ColorMode);
						}
						else if (kb.vkCode == (int)Keys.Left || kb.vkCode == (int)Keys.Right) {
							//colors
							G.colors = kb.vkCode == (int)Keys.Right ? G.colors + 1 > 141 ? 141 : G.colors + 1 : G.colors - 1 < 1 ? 1 : G.colors - 1;

							if (f2 != null)
								f2.ud_colors.Value = G.colors;

							f1.changeColorMode(G.ColorMode);
						}
					}
					else if (GetKeyState(VK_SHIFT) < 0) {
						if (kb.vkCode == (int)Keys.Up || kb.vkCode == (int)Keys.Down) {
							//speed
							f1.ud_updateinterval.Value = kb.vkCode == (int)Keys.Down ? f1.ud_updateinterval.Value - 10 < 1 ? 1 : f1.ud_updateinterval.Value - 10 : f1.ud_updateinterval.Value + 10;
						}
					}
					else if (GetKeyState(VK_MENU) < 0) {

					}
					else {						
						if (kb.vkCode == (int)Keys.Up || kb.vkCode == (int)Keys.Down) {
							//iterations
							G.maxloop = kb.vkCode == (int)Keys.Up ? G.maxloop + 1 : G.maxloop - 1 < 0 ? 0 : G.maxloop - 1;

							f1.ud_iterations.ValueChanged -= f1.ud_iterations_ValueChanged;
							f1.ud_iterations.Value = G.maxloop;
							f1.ud_iterations.ValueChanged += f1.ud_iterations_ValueChanged;
						}
						else if (kb.vkCode == (int)Keys.Left || kb.vkCode == (int)Keys.Right) {
							//resolution
							f1.ud_reduction.ValueChanged -= f1.ud_reduction_ValueChanged;
							f1.ud_reduction.Value = Convert.ToDecimal(kb.vkCode == (int)Keys.Left ? (double)f1.ud_reduction.Value - .1 < 1 ? 1 : (double)f1.ud_reduction.Value - .1 : (double)f1.ud_reduction.Value + .1 > 100 ? 100 : (double)f1.ud_reduction.Value + .1);
							f1.ud_reduction.ValueChanged += f1.ud_reduction_ValueChanged;
						}
					}
				}
				if ((int)wParam == WM_KEYUP || (int)wParam == WM_SYSKEYUP) {
					if ((kb.vkCode == (int)Keys.Up || kb.vkCode == (int)Keys.Down || kb.vkCode == (int)Keys.Left || kb.vkCode == (int)Keys.Right)
						&& (GetKeyState(VK_MENU) >= 0 && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0)) {
						f1.MandelBrotDraw();
						f1.JuliaDraw();
					}
				}
			}

			return CallNextHookEx(kbhook, nCode, wParam, lParam);
		}		
	}
}
