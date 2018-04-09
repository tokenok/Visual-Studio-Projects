using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Data.OleDb;

namespace DB {
	public partial class Form1 : Form {
		public OleDbConnection conn;
		public OleDbDataReader reader;
		public struct Row {
			public string date;
			public double u, v;
		};

		public Form1() {
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e) {
			var DB = new List<Row>();

			while (reader.Read()) {
				string u = reader[1].ToString();
				string v = reader[2].ToString();

				Row row = new Row();
				row.date = reader[0].ToString();
				double.TryParse(u, out row.u);
				double.TryParse(v, out row.v);
				DB.Add(row);
			}

			//3-point prediction
			int point = 3;

			var uavg = new List<double>();
			var vavg = new List<double>();
			double sumx = 0, sumu = 0, sumv = 0;
			double sumxu = 0, sumxv = 0, sumxx = 0;

			for (int i = 0; i < DB.Count; i++) {
				if (i == DB.Count - point - 1)
					break;
				double u = 0;
				double v = 0;

				sumu += DB[i].u;
				sumv += DB[i].v;
				sumx += i;

				double innersumx = 0;
				double innersumxx = 0;
				double innersumxu = 0;
				double innersumxv = 0;

				output.AppendText((i + 1).ToString() + ": ");

				for (int j = i + 1; j < i + point + 1; j++) {
					u += DB[j - 1].u;
					v += DB[j - 1].v;

					innersumx += j;
					innersumxx += j * j;
					innersumxu += j * DB[j - 1].u;
					innersumxv += j * DB[j - 1].v;

					output.AppendText(DB[j - 1].u.ToString() + ", ");
				}

				double inneruslope = (innersumxu - ((innersumx * u) / point)) / (innersumxx - ((innersumx * innersumx) / point));
				double inneruinter = (u / point) - (inneruslope * (innersumx / point));

				double prediction = inneruslope * (point + 1) + inneruinter;
				if (prediction > 60)
					output.AppendText("");

				output.AppendText(/*DB[i].date + " " + */" | " + inneruslope + "x + " + inneruinter + " = " + (inneruslope * (point + 1) + inneruinter) + "\r\n");
				

				double innervslope = (innersumxv - ((innersumx * v) / point)) / (innersumxx - ((innersumx * innersumx) / point));
				double innervinter = (v / point) - (innervslope * (innersumx / point));








				uavg.Add(u / point);
				vavg.Add(v / point);
			}

			for (int i = 1; i < uavg.Count - 1; i++) {
				int index = i - 1;
				sumxu += i * uavg[index];
				sumxv += i * vavg[index];
				sumxx += i * i;
			}

			//u
			double uslope = (sumxu - ((sumx * sumu) / DB.Count)) / (sumxx - ((sumx * sumx) / DB.Count));
			double uinter = (sumu / DB.Count) - (uslope * (sumx / DB.Count));
			for (int i = 0; i < DB.Count; i++) {
			//	output.AppendText(/*DB[i].date + " " + */uslope * i + uinter + "\r\n");
			}
			//v
			double vslope = (sumxv - ((sumx * sumv) / DB.Count)) / (sumxx - ((sumx * sumx) / DB.Count));
			double vinter = (sumv / DB.Count) - (vslope * (sumx / DB.Count));
			for (int i = 0; i < DB.Count; i++) {
			//	output.AppendText(/*DB[i].date + " " + */vslope * i + vinter + "\r\n");
			}










		}

		private void Form1_Load(object sender, EventArgs e) {
			string connectionString = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\\Users\\Josh_2.Josh-PC\\Documents\\School\\DataBases\\WindsDB.accdb";

			conn = new OleDbConnection(connectionString);

			conn.Open();

			string selectSQL = "SELECT dat, u, v FROM Windat WHERE dat>=#7/2/2012# and dat<=#7/4/2012# And wban='14958' ORDER BY dat, longitude, latitude";
			OleDbCommand cmd = new OleDbCommand(selectSQL, conn);

			reader = cmd.ExecuteReader(CommandBehavior.CloseConnection);
		}
	}
}
