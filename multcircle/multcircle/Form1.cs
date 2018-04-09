using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace multcircle {
	public partial class Form1 : Form {
		public Form1() {
			InitializeComponent();
		}

		public int g_points = 200;
		public double g_mult = 0, g_inc = 0.1, radius = 217;

		private void Form1_Load(object sender, EventArgs e) {
			timer1.Interval = 110;
			label2.Text = g_mult.ToString();
			label3.Text = g_points.ToString();
			label5.Text = g_inc.ToString();

			tb_multiple.Value = (int)(g_mult * 100);
			tb_nodes.Value = g_points;
			tb_rate.Value = (int)(g_inc * 100);
		}

		private void panel1_Paint(object sender, PaintEventArgs e) {
			Graphics g = e.Graphics;
			
			Pen pen = new Pen(Color.FromArgb(0, 128, 255), 1);

			double inc = 360.0 / (double)g_points;

			List<Point> pts = new List<Point>();

			for (double i = 0.0; i < 360.0; i += inc) {
				double angle = i * System.Math.PI / 180;
				int x = (int)((panel1.Width / 2) + radius * System.Math.Cos(angle));
				int y = (int)((panel1.Width / 2) + radius * System.Math.Sin(angle));

				pts.Add(new Point(x, y));
			}

			for (int i = 0; i < pts.Count / 2; i++) {
				Point tpt = pts[0];
				pts.RemoveAt(0);
				pts.Add(tpt);
			}

			for (int i = 0; i < pts.Count; i++) {
				g.FillEllipse(Brushes.Black, pts[i].X - 3, pts[i].Y - 3, 6, 6);
				g.DrawLine(pen, pts[i], pts[(int)(i * g_mult) % g_points]);
			}
		}

		private void timer1_Tick(object sender, EventArgs e) {
			panel1.Invalidate();
			g_mult += g_inc;
			label2.Text = g_mult.ToString();
			tb_multiple.Value = (int)(g_mult * 100);
		}

		private void button1_Click(object sender, EventArgs e) {
			if (button1.Text == "Play" || button1.Text == "Start") {
				timer1.Start();
				button1.Text = "Pause";
			}
			else {
				timer1.Stop();
				button1.Text = "Play";
			}
		}

		private void tb_multiple_Scroll(object sender, EventArgs e) {
			g_mult = (double)tb_multiple.Value / 100.0;
			label2.Text = g_mult.ToString();
			panel1.Invalidate();
		}

		private void trackBar1_Scroll(object sender, EventArgs e) {
			g_points = tb_nodes.Value;
			label3.Text = g_points.ToString();
			panel1.Invalidate();
		}

		private void trackBar1_Scroll_1(object sender, EventArgs e) {
			g_inc = (double)tb_rate.Value / 100.0;
			label5.Text = g_inc.ToString();
			panel1.Invalidate();
		}
	}
}
