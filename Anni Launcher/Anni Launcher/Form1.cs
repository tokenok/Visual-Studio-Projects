using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Deployment.Application;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Anni_Launcher {
	public partial class Form1 : Form {
		private const string version_url = "http://launcher.annihilus.net/annihilusversion.txt";
		private const string updater_url = "http://launcher.annihilus.net/AnnihilusUpdater.exe";
		private const string registry_key = "HKEY_CURRENT_USER\\Software\\Blizzard Entertainment\\Diablo II";

		Stopwatch sw = new Stopwatch();

		bool g_updating = false;

		string current_version = "";

		System.Net.WebClient g_webClient = new System.Net.WebClient();

		//Microsoft.WindowsAPICodePack.Taskbar.TaskbarManager taskbar_progressbar = null;

		public Form1() {
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e) {
			string FullScreen = (string)Registry.GetValue(registry_key, "FullScreen", 0);
			cb_fullscreen.CheckState = FullScreen == "1" ? CheckState.Checked : CheckState.Unchecked;

			string PreferredRealm = (string)Registry.GetValue(registry_key, "Preferred Realm", "Annihilus");
			if (PreferredRealm == "Annihilus") {
				btn_annihilus_Click(this.btn_annihilus, null);
			}
			else if (PreferredRealm == "D2PK") {
				btn_d2pk_Click(this.btn_d2pk, null);
			}

			this.FormClosing += new FormClosingEventHandler(OnApplicationExit);

			//try {
			//	taskbar_progressbar = Microsoft.WindowsAPICodePack.Taskbar.TaskbarManager.Instance;
			//}
			//catch (Exception ex) {

			//}
		}

		private void Form1_Shown(object sender, EventArgs e) {
			string local_version = ((string)Registry.GetValue(registry_key, "AnnihilusVersion", "0")).Trim();

			System.Net.WebClient wc = new System.Net.WebClient();
			try {
				current_version = System.Text.Encoding.UTF8.GetString(wc.DownloadData(version_url));

				if (current_version != local_version) {
					lbl_updatestatus.Text = "Annihilus requires an update.";
					DownloadUpdater();
				}
				else {
					lbl_updatestatus.Text = "Annihilus is up to date.";
				}
			}
			catch (Exception ex) {
				MessageBox.Show(ex.ToString(), "Error");
			}
		}

		private void DownloadUpdater() {
			var res = MessageBox.Show("Annihilus Update is Available! Please close all instances of Diablo II and click OK to update now.", "Annihilus Launcher", MessageBoxButtons.OKCancel);

			if (res == System.Windows.Forms.DialogResult.OK) {
				g_updating = true;
				btn_launch.Image = Anni_Launcher.Properties.Resources.launchdisabled;

				lbl_downloadinfo.Show();
				progressBar1.Show();

				//if (taskbar_progressbar != null) {
				//	taskbar_progressbar.SetProgressValue(0, 100);
				//	taskbar_progressbar.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.Normal);
				//}

				lbl_updatestatus.Text = "Update found! Downloading...";

				g_webClient.DownloadFileCompleted += new AsyncCompletedEventHandler(DownloadCompleted);
				g_webClient.DownloadProgressChanged += new DownloadProgressChangedEventHandler(ProgressChanged);

				sw.Start();
				try {
					g_webClient.DownloadFileAsync(new Uri(updater_url), AppDomain.CurrentDomain.BaseDirectory + "AnnihilusUpdater.exe");
				}
				catch (Exception ex) {
					sw.Stop();
					MessageBox.Show(ex.Message);
				}
			}
		}

		private void ProgressChanged(object sender, System.Net.DownloadProgressChangedEventArgs e) {
			progressBar1.Value = e.ProgressPercentage;
			//if (taskbar_progressbar != null)
			//	taskbar_progressbar.SetProgressValue(e.ProgressPercentage, 100);

			string speed = string.Format("{0} kb/s", (e.BytesReceived / 1024d / sw.Elapsed.TotalSeconds).ToString("0.00"));

			lbl_downloadinfo.Text = string.Format("{0} Mb of {1} Mb ({2}) {3}",
				(e.BytesReceived / 1024 / 1024).ToString("0.00"),
				(e.TotalBytesToReceive / 1024 / 1024).ToString("0.00"),
				e.ProgressPercentage.ToString() + "%",
				speed);
		}
		private void DownloadCompleted(object sender, AsyncCompletedEventArgs e) {
			sw.Reset();

			g_updating = false;
			btn_launch.Image = Anni_Launcher.Properties.Resources.launch;
			
			if (e.Error == null) {
				if (e.Cancelled == true) {
					lbl_downloadinfo.Text = "Download canceled";
				}
				else {
					lbl_downloadinfo.Text = "Download completed!";
					progressBar1.Hide();
					//if (taskbar_progressbar != null)
					//	taskbar_progressbar.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.NoProgress);

					lbl_updatestatus.Text = "Annihilus is up to date.";

					try {
						//System.IO.File.Delete(AppDomain.CurrentDomain.BaseDirectory + "AnnihilusUpdater.exe");
						//System.IO.File.Move(AppDomain.CurrentDomain.BaseDirectory + "AnnihilusUpdater(temp).exe", AppDomain.CurrentDomain.BaseDirectory + "AnnihilusUpdater.exe");
						Process.Start(AppDomain.CurrentDomain.BaseDirectory + "AnnihilusUpdater.exe");
					}
					catch (Exception ex) {
						MessageBox.Show(ex.ToString(), "error");
					}

					Registry.SetValue(registry_key, "AnnihilusVersion", current_version);

					this.Close();
				}
			}
			else {
				MessageBox.Show(e.Error.Message, "error");

				lbl_downloadinfo.Text = "Download failed!";
				progressBar1.Hide();
				//if (taskbar_progressbar != null)
				//	taskbar_progressbar.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.NoProgress);

				lbl_updatestatus.Text = "Annihilus requires an date.";
			}
		}

		private void OnApplicationExit(object sender, FormClosingEventArgs e) {
			if (g_updating) {
				var res = MessageBox.Show("Stop program update and Exit?\n\nTo resume update click Cancel\nTo continue without the patch update click OK", "Annihilus Launcher", MessageBoxButtons.OKCancel);
				if (res == DialogResult.OK) {
					g_webClient.CancelAsync();
					//try {
					//	System.IO.File.Delete(AppDomain.CurrentDomain.BaseDirectory + "AnnihilusUpdater(temp).exe");
					//}
					//catch (Exception ex) {
					//	MessageBox.Show(ex.ToString(), "error");
					//}
				}
				e.Cancel = res == DialogResult.Cancel;
			}			
		}

		private void btn_launch_Click(object sender, EventArgs e) {
			if (!g_updating) {
				ProcessStartInfo psi = new ProcessStartInfo();
				string base_dir = AppDomain.CurrentDomain.BaseDirectory;

				psi.FileName = base_dir + "Diablo II.exe";
				psi.Arguments = "-w -nohide";

				string PreferredRealm = (string)Registry.GetValue(registry_key, "Preferred Realm", "Annihilus");
				if (PreferredRealm == "D2PK") {
					psi.WorkingDirectory = base_dir + "D2PK";
				}
				else {
					psi.WorkingDirectory = base_dir;
				}

				try {
					Process.Start(psi);
				}
				catch (Exception ex) {
					MessageBox.Show(ex.ToString(), "Error");
				}
			}
		}

		private void btn_reupdate_Click(object sender, EventArgs e) {
			if (!g_updating) {
				DownloadUpdater();
			}
		}

		private void btn_d2pk_Click(object sender, EventArgs e) {
			btn_d2pk.BackgroundImage = Anni_Launcher.Properties.Resources.d2pkselected;
			btn_annihilus.BackgroundImage = Anni_Launcher.Properties.Resources.anniunselected;

			Registry.SetValue(registry_key, "DisableHighRes", 1);
			Registry.SetValue(registry_key, "Preferred Realm", "D2PK");
		}


		private void btn_annihilus_Click(object sender, EventArgs e) {
			btn_d2pk.BackgroundImage = Anni_Launcher.Properties.Resources.d2pkunselected;
			btn_annihilus.BackgroundImage = Anni_Launcher.Properties.Resources.anniselected;

			Registry.SetValue(registry_key, "DisableHighRes", 0);
			Registry.SetValue(registry_key, "Preferred Realm", "Annihilus");
		}

		private void cb_fullscreen_CheckedChanged(object sender, EventArgs e) {
			Registry.SetValue(registry_key, "FullScreen", cb_fullscreen.CheckState == CheckState.Checked ? "1" : "0");
		}
		
		private void btn_close_Click(object sender, EventArgs e) {
			this.Close();
		}

		private void pictureBox1_MouseDown(object sender, MouseEventArgs e) {
			btn_close.BackgroundImage = Anni_Launcher.Properties.Resources.close2;
		}
		private void btn_close_MouseUp(object sender, MouseEventArgs e) {
			btn_close.BackgroundImage = Anni_Launcher.Properties.Resources.close;
		}
		private void btn_launch_MouseDown(object sender, MouseEventArgs e) {
			btn_launch.BackgroundImage = Anni_Launcher.Properties.Resources.launchdown;
		}
		private void btn_launch_MouseUp(object sender, MouseEventArgs e) {
			btn_launch.BackgroundImage = Anni_Launcher.Properties.Resources.launch;
		}
		private void btn_reupdate_MouseDown(object sender, MouseEventArgs e) {
			btn_reupdate.BackgroundImage = Anni_Launcher.Properties.Resources.reupdatedown;
		}
		private void btn_reupdate_MouseUp(object sender, MouseEventArgs e) {
			btn_reupdate.BackgroundImage = Anni_Launcher.Properties.Resources.reupdate;
		}
	}
}
