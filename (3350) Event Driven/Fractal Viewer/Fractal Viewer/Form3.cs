using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Fractal_Viewer {
	public partial class Form3 : Form {
		public Form3() {
			InitializeComponent();
		}

		private void Form3_Load(object sender, EventArgs e) {
			pictureBox1.Focus();
		}

		private void pictureBox1_MouseHover(object sender, EventArgs e) {
			pictureBox1.Focus();
		}
		private void Form3_Scroll(object sender, MouseEventArgs e) {
			Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

			double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((Form1.G.xmax - Form1.G.xmin) / pictureBox1.Width) + ((Form1.G.xmax + Form1.G.xmin) / 2);
			double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((Form1.G.ymax - Form1.G.ymin) / pictureBox1.Height) + ((Form1.G.ymax + Form1.G.ymin) / 2);

			if (e.Delta > 0) {
				Form1.G.xmin += xoff;
				Form1.G.xmax += xoff;
				Form1.G.ymin += yoff;
				Form1.G.ymax += yoff;

				Form1.G.xmin /= 2;
				Form1.G.xmax /= 2;
				Form1.G.ymin /= 2;
				Form1.G.ymax /= 2;
			}
			else {
				Form1.G.xmin *= 2;
				Form1.G.xmax *= 2;
				Form1.G.ymin *= 2;
				Form1.G.ymax *= 2;

				Form1.G.xmin -= xoff;
				Form1.G.xmax -= xoff;
				Form1.G.ymin -= yoff;
				Form1.G.ymax -= yoff;
			}

			f1.MandelBrotDraw();
		}
		private void pictureBox1_MouseDown(object sender, MouseEventArgs e) {
			pictureBox1.Focus();

			Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

			double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((Form1.G.xmax - Form1.G.xmin) / pictureBox1.Width);
			double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((Form1.G.ymax - Form1.G.ymin) / pictureBox1.Height);

			if (e.Button == MouseButtons.Left) {
				int reduction = Convert.ToInt32(f1.ud_reduction.Value);
				Form1.G.spanx = e.Location.X / reduction;
				Form1.G.spany = e.Location.Y / reduction;
				Form1.G.panx = 0;
				Form1.G.pany = 0;

				Form1.G.sxoff = xoff;
				Form1.G.syoff = yoff;
			}
		}
		private void pictureBox1_MouseUp(object sender, MouseEventArgs e) {
			Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((Form1.G.xmax - Form1.G.xmin) / pictureBox1.Width);
				double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((Form1.G.ymax - Form1.G.ymin) / pictureBox1.Height);

				double xoff1 = xoff - Form1.G.sxoff;
				double yoff1 = yoff - Form1.G.syoff;

				Form1.G.xmin -= xoff1;
				Form1.G.xmax -= xoff1;
				Form1.G.ymin -= yoff1;
				Form1.G.ymax -= yoff1;

				double reduction = Convert.ToDouble(f1.ud_reduction.Value);
				int w = (int)((double)pictureBox1.Width / reduction),
					h = (int)((double)pictureBox1.Height / reduction);

				if (Math.Abs(Form1.G.panx) >= w || Math.Abs(Form1.G.pany) >= h) {
					f1.MandelBrotDraw();
					Form1.G.spanx = 0;
					Form1.G.spany = 0;
					Form1.G.panx = 0;
					Form1.G.pany = 0;
					return;
				}

				double xzoom = (Form1.G.xmax - Form1.G.xmin) / (double)w;
				double yzoom = (Form1.G.ymax - Form1.G.ymin) / (double)h;

				List<List<int>> g = new List<List<int>>();
				for (int i = 0; i < Form1.G.MandelbrotActiveCountgrid.Count; i++) {
					List<int> c = new List<int>();
					for (int j = 0; j < Form1.G.MandelbrotActiveCountgrid[i].Count; j++) {
						c.Add(-1);
					}
					g.Add(c);
				}

				if (Form1.G.panx <= 0 && Form1.G.pany <= 0) {
					for (int i = 0; i < h + Form1.G.pany; i++) {
						for (int j = 0; j < w + Form1.G.panx; j++) {
							g[i][j] = Form1.G.MandelbrotActiveCountgrid[i - Form1.G.pany][j - Form1.G.panx];
						}
					}
					Form1.G.MandelbrotCountgrid = g;

					f1.MandelbrotGetCount(xzoom, yzoom, h + Form1.G.pany, h, 0, (w + Form1.G.panx) * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, h + Form1.G.pany, h, (w + Form1.G.panx) * 4, w * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, 0, h + Form1.G.pany, (w + Form1.G.panx) * 4, w * 4);
				}
				else if (Form1.G.panx > 0 && Form1.G.pany < 0) {
					for (int i = 0; i < h + Form1.G.pany; i++) {
						for (int j = Form1.G.panx; j < w; j++) {
							g[i][j] = Form1.G.MandelbrotActiveCountgrid[i - Form1.G.pany][j - Form1.G.panx];
						}
					}
					Form1.G.MandelbrotCountgrid = g;

					f1.MandelbrotGetCount(xzoom, yzoom, 0, h + Form1.G.pany, 0, Form1.G.panx * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, h + Form1.G.pany, h, 0, Form1.G.panx * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, h + Form1.G.pany, h, Form1.G.panx * 4, w * 4);
				}
				else if (Form1.G.panx < 0 && Form1.G.pany > 0) {
					for (int i = Form1.G.pany; i < h; i++) {
						for (int j = 0; j < w + Form1.G.panx; j++) {
							g[i][j] = Form1.G.MandelbrotActiveCountgrid[i - Form1.G.pany][j - Form1.G.panx];
						}
					}
					Form1.G.MandelbrotCountgrid = g;

					f1.MandelbrotGetCount(xzoom, yzoom, 0, Form1.G.pany, 0, (w + Form1.G.panx) * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, 0, Form1.G.pany, (w + Form1.G.panx) * 4, w * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, Form1.G.pany, h, (w + Form1.G.panx) * 4, w * 4);
				}
				else {
					for (int i = Form1.G.pany; i < h; i++) {
						for (int j = Form1.G.panx; j < w; j++) {
							g[i][j] = Form1.G.MandelbrotActiveCountgrid[i - Form1.G.pany][j - Form1.G.panx];
						}
					}
					Form1.G.MandelbrotCountgrid = g;

					f1.MandelbrotGetCount(xzoom, yzoom, Form1.G.pany, h, 0, Form1.G.panx * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, 0, Form1.G.pany, 0, Form1.G.panx * 4);
					f1.MandelbrotGetCount(xzoom, yzoom, 0, Form1.G.pany, Form1.G.panx * 4, w * 4);
				}

				Form1.G.MandelbrotActiveCountgrid = Form1.G.MandelbrotCountgrid;
			}
			Form1.G.spanx = 0;
			Form1.G.spany = 0;
			Form1.G.panx = 0;
			Form1.G.pany = 0;
		}
		private void pictureBox1_MouseMove(object sender, MouseEventArgs e) {
			Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

			double xoff = (e.Location.X - (pictureBox1.Width / 2)) * ((Form1.G.xmax - Form1.G.xmin) / pictureBox1.Width);// +((G.xmax + G.xmin) / 2);
			double yoff = ((pictureBox1.Height / 2) - e.Location.Y) * ((Form1.G.ymax - Form1.G.ymin) / pictureBox1.Height);// +((G.ymax + G.ymin) / 2);
			double xoffoff = xoff + ((Form1.G.xmax + Form1.G.xmin) / 2);
			double yoffoff = yoff + ((Form1.G.ymax + Form1.G.ymin) / 2);

			if (e.Button == System.Windows.Forms.MouseButtons.Left) {
				int reduction = Convert.ToInt32(f1.ud_reduction.Value);

				Form1.G.panx = (e.Location.X / reduction) - Form1.G.spanx;
				Form1.G.pany = (e.Location.Y / reduction) - Form1.G.spany;
			}
		}

		private void Form3_MouseLeave(object sender, EventArgs e) {
			this.Close();
		}

		private void pictureBox1_LostFocus(object sender, EventArgs e) {
			this.Close();
		}

		private void Form3_FormClosed(object sender, FormClosedEventArgs e) {
			Form1 f1 = System.Windows.Forms.Application.OpenForms["Form1"] as Form1;

			Form1.G.MandelbrotCountgrid.Clear();
			for (int i = 0; i < f1.pictureBox1.Height; i++) {
				List<int> t = new List<int>();
				for (int j = 0; j < f1.pictureBox1.Width; j++) {
					t.Add(0);
				}
				Form1.G.MandelbrotCountgrid.Add(t);
			}

			f1.MandelBrotDraw();
		}
	}
}
