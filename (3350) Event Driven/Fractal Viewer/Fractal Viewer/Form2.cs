using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Fractal_Viewer {
	public partial class Form2 : Form {
		public Form2() {
			InitializeComponent();
		}

		Form1 f1;

		private void Form2_Load(object sender, EventArgs e) {
			f1 = (System.Windows.Forms.Application.OpenForms["Form1"] as Form1);

			switch (Form1.G.ColorMode) {
				case Form1.ColorModes.BlackAndWhite: {
					rb_blackwhite.CheckedChanged -= rb_blackwhite_CheckedChanged;
					rb_blackwhite.Checked = true;
					rb_blackwhite.CheckedChanged += rb_blackwhite_CheckedChanged;
					break;
				}
				case Form1.ColorModes.Rainbow: {
					rb_rainbow.CheckedChanged -= rb_rainbow_CheckedChanged;
					rb_rainbow.Checked = true;
					rb_rainbow.CheckedChanged += rb_rainbow_CheckedChanged;
					break;
				}
				case Form1.ColorModes.Palette: {
					rb_default.CheckedChanged -= rb_default_CheckedChanged;
					rb_default.Checked = true;
					rb_default.CheckedChanged += rb_default_CheckedChanged;
					break;
				}
				case Form1.ColorModes.Random: {
					rb_random.CheckedChanged -= rb_random_CheckedChanged;
					rb_random.Checked = true;
					rb_random.CheckedChanged += rb_random_CheckedChanged;
					break;
				}
			}

			cb_cyclecolors.CheckedChanged -= cb_cyclecolors_CheckedChanged;
			cb_cyclecolors.Checked = Form1.G.cycle;
			cb_cyclecolors.CheckedChanged += cb_cyclecolors_CheckedChanged;

			cb_edgedetect.CheckedChanged -= cb_edgedetect_CheckedChanged;
			cb_edgedetect.Checked = Form1.G.edge;
			cb_edgedetect.CheckedChanged += cb_edgedetect_CheckedChanged;

			//init icon list
			lb_colorbox.View = View.SmallIcon;			

			imageList1.ImageSize = new Size(16, 16);

			ud_colors.Value = Form1.G.colors;
			ud_gradients.Value = Form1.G.gradients;

			imageList1.Images.Clear();
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
				imageList1.Images.Add(bmp);
			}
			lb_colorbox.SmallImageList = imageList1;

			lb_colorbox.Clear();
			for (int i = 0; i < Form1.G.palettecolors.Count; i++) {
				ListViewItem lvi = new ListViewItem("");

				lvi.ImageIndex = i;
				lvi.BackColor = Form1.G.palettecolors[i];
				lb_colorbox.Items.Add(lvi);
			}

			lb_colorbox.HideSelection = true;
		}

		private void lb_colorbox_ItemActivate(object sender, EventArgs e) {
			ListView lv = (ListView)sender;

			colorDialog1.CustomColors = new int[] { ColorTranslator.ToOle((lv).SelectedItems[0].SubItems[0].BackColor),
			ColorTranslator.ToOle((lv).SelectedItems[0].SubItems[0].BackColor),
			ColorTranslator.ToOle((lv).SelectedItems[0].SubItems[0].BackColor),
			ColorTranslator.ToOle((lv).SelectedItems[0].SubItems[0].BackColor),
			ColorTranslator.ToOle((lv).SelectedItems[0].SubItems[0].BackColor)};
			
			DialogResult result = colorDialog1.ShowDialog();
			if (result == System.Windows.Forms.DialogResult.OK) {
				Bitmap bmp = new Bitmap(16, 16);
				for (int y = 0; y < 16; y++) {
					for (int x = 0; x < 16; x++) {
						if (y == 0 || y == 15 || x == 0 || x == 15) 
							bmp.SetPixel(x, y, Color.Black);
						else
							bmp.SetPixel(x, y, colorDialog1.Color);
					}
				}

				imageList1.Images.Add(bmp);		

				lv.SelectedItems[0].ImageIndex = imageList1.Images.Count - 1;
				lv.SelectedItems[0].BackColor = colorDialog1.Color;

				Form1.G.palettecolors.Clear();

				for (int i = 0; i < lb_colorbox.Items.Count; i++) {
					Form1.G.palettecolors.Add(lb_colorbox.Items[i].BackColor);
					Form1.G.knowncolors[i] = lb_colorbox.Items[i].BackColor;
				}

				Form1.G.gradientcolors = f1.getGradient(Form1.G.palettecolors, Form1.G.gradients);
			}
		}

		private void button1_Click(object sender, EventArgs e) {
			Form1.G.palettecolors.Clear();

			for (int i = 0; i < lb_colorbox.Items.Count; i++) {
				Form1.G.palettecolors.Add(lb_colorbox.Items[i].BackColor);
				Form1.G.knowncolors[i] = lb_colorbox.Items[i].BackColor;
			}

			this.DialogResult = DialogResult.OK;
			
			this.Close();
		}

		private void ud_gradients_ValueChanged(object sender, EventArgs e) {
			Form1.G.gradients = Convert.ToInt32(ud_gradients.Value);

			f1.changeColorMode(Form1.G.ColorMode);
		}
		private void ud_colors_ValueChanged(object sender, EventArgs e) {
			Form1.G.colors = Convert.ToInt32(ud_colors.Value);
			
			f1.changeColorMode(Form1.G.ColorMode);
		}

		private void button2_Click(object sender, EventArgs e) {
			this.Close();
		}

		private void button3_Click(object sender, EventArgs e) {
			colorDialog1.CustomColors = new int[16];
			
			DialogResult result = colorDialog1.ShowDialog();
			if (result == System.Windows.Forms.DialogResult.OK) {
				Bitmap bmp = new Bitmap(16, 16);
				for (int y = 0; y < 16; y++) {
					for (int x = 0; x < 16; x++) {
						if (y == 0 || y == 15 || x == 0 || x == 15)
							bmp.SetPixel(x, y, Color.Black);
						else
							bmp.SetPixel(x, y, colorDialog1.Color);
					}
				}

				imageList1.Images.Add(bmp);

				ListViewItem lvi = new ListViewItem("");

				lvi.ImageIndex = imageList1.Images.Count - 1;
				lvi.BackColor = colorDialog1.Color;
				lb_colorbox.Items.Add(lvi);
			}			
		}

		private void rb_default_CheckedChanged(object sender, EventArgs e) {
			if (rb_default.Checked) {
				Type colorType = typeof(System.Drawing.Color);
				PropertyInfo[] picolors = colorType.GetProperties(BindingFlags.Static | BindingFlags.DeclaredOnly | BindingFlags.Public);
				Array.Reverse(picolors);
				Form1.G.knowncolors = new Color[picolors.Length];
				for (int i = 0; i < Form1.G.knowncolors.Length; i++) {
					Form1.G.knowncolors[i] = (Color)picolors[i].GetValue(null);
				}
				f1.changeColorMode(Fractal_Viewer.Form1.ColorModes.Random);
			}
		}

		private void rb_rainbow_CheckedChanged(object sender, EventArgs e) {
			if (rb_rainbow.Checked)
				f1.changeColorMode(Fractal_Viewer.Form1.ColorModes.Rainbow);
		}

		private void rb_random_CheckedChanged(object sender, EventArgs e) {
			if (rb_random.Checked)
				f1.changeColorMode(Fractal_Viewer.Form1.ColorModes.Random, true);
		}

		private void rb_random_MouseClick(object sender, MouseEventArgs e) {
			if (rb_random.Checked)
				f1.changeColorMode(Fractal_Viewer.Form1.ColorModes.Random, true);
		}

		private void rb_blackwhite_CheckedChanged(object sender, EventArgs e) {
			f1.changeColorMode(Fractal_Viewer.Form1.ColorModes.BlackAndWhite);
		}

		private void cb_edgedetect_CheckedChanged(object sender, EventArgs e) {
			Form1.G.edge = !Form1.G.edge;
		}

		private void cb_cyclecolors_CheckedChanged(object sender, EventArgs e) {
			Form1.G.cycle = !Form1.G.cycle;
		}

		private void btn_cycledirection_Click(object sender, EventArgs e) {
			Form1.G.cycledown = !Form1.G.cycledown;
		}

		private void btn_savescheme_Click(object sender, EventArgs e) {
			Stream myStream = null;

			string path = Form1.getexedir() + "\\palletes";
			System.IO.Directory.CreateDirectory(path);
			string filename = "color scheme " + DateTime.Now.ToString(@"MM_dd_yyyy h_mm_ss tt") + ".scheme";

			SaveFileDialog dlg = new SaveFileDialog();
			dlg.Filter = "color scheme files (*.scheme)|*.scheme";

			if (dlg.ShowDialog() == DialogResult.OK) {
				if ((myStream = dlg.OpenFile()) != null) {
					StreamWriter s = new StreamWriter(myStream);
					for (int i = 0; i < Form1.G.palettecolors.Count; i++) {
						s.WriteLine("{0},{1},{2}", Form1.G.palettecolors[i].R, Form1.G.palettecolors[i].G, Form1.G.palettecolors[i].B);
					}
					s.Close();
					myStream.Close();
				}
			}
		}

		private void btn_loadscheme_Click(object sender, EventArgs e) {
			Stream myStream = null;

			OpenFileDialog dlg = new OpenFileDialog();
			dlg.InitialDirectory = Form1.getexedir() + "\\palletes";
			dlg.Filter = "color scheme files (*.scheme)|*.scheme";

			dlg.RestoreDirectory = true;

			if (dlg.ShowDialog() == DialogResult.OK) {
				try {
					if ((myStream = dlg.OpenFile()) != null) {
						using (myStream) {
							Form1.G.palettecolors.Clear();

							StreamReader r = new StreamReader(myStream);
							string line;								
							while ((line = r.ReadLine()) != null) {								
								string[] rgb = line.Split(',');
								if (rgb.Length != 3)
									continue;

								int red = Convert.ToInt32(rgb[0]);
								int green = Convert.ToInt32(rgb[1]);
								int blue = Convert.ToInt32(rgb[2]);

								Form1.G.palettecolors.Add(Color.FromArgb(red, green, blue));
							}
						}
					}
				}
				catch (Exception ex) {
					MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
				}
			}

			f1.changeColorMode(Form1.ColorModes.Palette);
		}
	}
}
