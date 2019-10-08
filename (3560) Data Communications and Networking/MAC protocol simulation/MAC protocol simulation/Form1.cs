using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MAC_protocol_simulation {
	public partial class Form1 : Form {
		private const int T = 86400;
		private int N = 0;
		public static Random rng = new Random();

		private class transmission_station {
			public List<int> queue = new List<int>();
			public int wait = 0;
		}

		public Form1() {
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e) {
			(new Thread(() => {	NonPersistentP(); })).Start();	
		}

		private void NonPersistentP() {
			for (N = 1; N < 1000; N++) {
				bool in_use = false;
				int talker_id = -1;

				List<transmission_station> ts = new List<transmission_station>();
				for (int i = 0; i < N; i++) ts.Add(new transmission_station());

				int collisions = 0;
				int throughput = 0;

				for (int f = 0; f < T; f++) {
					for (int i = 0; i < ts.Count; i++) {
						if (rng.Next(1, 100) < rng.Next(1, 4))
							ts[i].queue.Add(10);
					}

					for (int i = 0; i < ts.Count; i++) {
						if (ts[i].queue.Count > 0 && (!in_use || talker_id == i)) {
							if (ts[i].queue[0] == 0) {
								ts[i].queue.RemoveAt(0);
								in_use = false;
								talker_id = -1;
								continue;
							}
							in_use = true;
							talker_id = i;
							ts[i].queue[0]--;
							throughput++;
						}
						else if (ts[i].queue.Count > 0) {
							if (ts[i].wait == 0) {
								ts[i].wait = rng.Next(1, 10);
								collisions++;
							}
							else
								ts[i].wait--;
						}
					}
				}

				int unsent = 0;
				for (int i = 0; i < ts.Count; i++) {
					for (int j = 0; j < ts[i].queue.Count; j++) {
						unsent += ts[i].queue[j];
					}
				}

				Console.WriteLine(N + " " + throughput + " " + unsent + " " + ((double)throughput / (double)unsent) + " " + collisions);
			}
		}

		private void numericUpDown1_ValueChanged(object sender, EventArgs e) {
			trackBar1.Value = (int)numericUpDown1.Value;
			N = trackBar1.Value;
		}

		private void trackBar1_Scroll(object sender, EventArgs e) {
			numericUpDown1.Value = trackBar1.Value;
			N = trackBar1.Value;
		}
	}
}
