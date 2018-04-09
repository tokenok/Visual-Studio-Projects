using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GeoComp {
	public partial class Form1 : Form {
		public class data_endry {
			public string last, first, cls, schoolID, answers;
			public int score, correct;
		}

		public Form1() {
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e) {
			button1.Enabled = false;			

			string key = "", tie = "";
			List<data_endry> entries = new List<data_endry>();

			try {
				FileStream file = new FileStream(richTextBox1.Text, FileMode.Open, FileAccess.Read);
				StreamReader reader = new StreamReader(file);

				string line;
				int count = 0;

				while ((line = reader.ReadLine()) != null) {
					string[] data = line.Split(',');

					if (count == 0) {
						key = data[data.Length - 1];
					}
					else if (count == 1) {
						tie = data[data.Length - 1];
					}
					else {
						if (data.Length < 5)
							continue;
						data_endry de = new data_endry();
						de.last = data[0];
						de.first = data[1];
						de.cls = data[2];
						de.schoolID = data[3];
						de.answers = data[4];

						entries.Add(de);
					}
					count++;
				}
			} catch (Exception ex) {
				MessageBox.Show("Error: " + ex.Message);
			}

			for (int i = 0; i < entries.Count; i++) {
				if (entries[i].answers.Length != key.Length)
					continue;

				int t1 = 0, t2 = 0;
				entries[i].correct = 0;
				for (int j = 0; j < entries[i].answers.Length; j++) {
					if (entries[i].answers[j] == key[j]) {
						entries[i].correct++;
						if (tie[j] == '1')
							t1++;
						if (tie[j] == '2')
							t2++;
					}
				}
				entries[i].score = entries[i].correct * 10000 + t1 * 100 + t2;
			}

			entries = entries.OrderBy(x => x.score).ToList();
			entries.Reverse();
			
			//Student Scores output
			using (StreamWriter writer = new StreamWriter(System.IO.Directory.GetCurrentDirectory() + "\\Student Scores.txt")) {
				writer.Write("Northwoods University\r\nJunior Division Team Scores\r\n\r\n");

				writer.WriteLine("Key: {0}", key);
				writer.WriteLine("Ties: {0}\r\n", tie);

				for (int i = 0; i < entries.Count; i++) {
					writer.WriteLine("{0,-6} {1,-36} {2,-2} {3,-6} {4,-40}", entries[i].score, entries[i].last + ", " + entries[i].first, entries[i].cls, entries[i].schoolID, entries[i].answers);
				}
			}

			entries = entries.OrderBy(x => x.correct).ToList();
			entries.Reverse();
			entries = entries.OrderBy(x => x.schoolID).ToList();
			string currentId = "";
			int c = 0;
			int sum3 = 0;

			//School Team Scores
			using (StreamWriter writer = new StreamWriter(System.IO.Directory.GetCurrentDirectory() + "\\School Team Scores.txt")) {
				writer.Write("Northwoods University\r\nJunior Division Team Scores\r\n\r\n");

				for (int i = 0; i < entries.Count; ++i) {
					if (currentId != entries[i].schoolID) {
						if (currentId != "") {
							writer.Write("\r\nTeam Score: +{0}\r\n\r\n\r\n", sum3);
						}

						writer.WriteLine(entries[i].schoolID);
						writer.WriteLine("=====");

						c = 0;
						sum3 = 0;
					}

					if (c < 3)
						sum3 += entries[i].correct;

					writer.WriteLine("{0,-6} {1,-36}", entries[i].correct, entries[i].last + ", " + entries[i].first, entries[i].cls);

					c++;
					currentId = entries[i].schoolID;
				}
				writer.Write("\r\nTeam Score: {0}\r\n\r\n\r\n", sum3);
			}

			//populate listview control
			listView1.Items.Clear();
			for (int i = 0; i < entries.Count; i++){
				string[] row = { entries[i].score.ToString(), entries[i].correct.ToString(), entries[i].last, entries[i].first, entries[i].cls, entries[i].schoolID, entries[i].answers};
				var listViewItem = new ListViewItem(row);
				listView1.Items.Add(listViewItem);
			}

			button1.Enabled = true;
		}

		private void button2_Click(object sender, EventArgs e) {
			OpenFileDialog ofn = new OpenFileDialog();

			if (ofn.ShowDialog() == DialogResult.OK) {
				try {
					if (ofn.OpenFile() != null) {
						richTextBox1.Text = ofn.FileName;
					}
				}
				catch (Exception ex) {
					MessageBox.Show("Error: " + ex.Message);
				}
			}
		}
	}
}

