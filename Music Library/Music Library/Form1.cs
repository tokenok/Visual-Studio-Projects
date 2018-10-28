using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Net;
using System.Threading;
using System.Web;
using System.Reflection;

namespace Music_Library {
	public partial class Form1 : Form {

		[DllImport("kernel32.dll")]
		static extern uint GetModuleFileName(IntPtr hModule, [Out] StringBuilder lpFilename, int nSize);
		public static string getexedir() {
			StringBuilder filename = new StringBuilder(1024);
			GetModuleFileName(IntPtr.Zero, filename, 1024);
			string dir = filename.ToString();
			return dir.Substring(0, dir.LastIndexOf('\\'));
		}

		BackgroundWorker backgroundWorkerScanSongData = new BackgroundWorker();
		BackgroundWorker backgroundWorkerApplySongData = new BackgroundWorker();

		delegate void PrintCallback(string text);
		delegate void EnableScanButtonCallback(bool enabled);
		delegate void EnableApplyButtonCallback(bool enabled);
		delegate void ProgressBarShowCallback(bool show);
		delegate void ProgressBarSetMaximumCallback(int max);
		delegate void ProgressBarSetValueCallback(int val);

		public string g_pc_dir = "";//@"C:\Music\Music";
		//public string g_sd_dir = @"Z:";
		public string g_sd_dir = "";//@"P:\Music";
		public string g_itunes_xml_path = "";// @"C:\Users\Josh_2.Josh-PC\Music\iTunes\iTunes Music Library.xml";

		public class entry {
			public string dir, title, album, artist, album_artist, genre, year, track_num;

			public int compare(entry e) {
				int m = 0;
				if (e.artist != this.artist)
					m += 1;
				if (e.album_artist != this.album_artist)
					m += 2;
				if (e.album != this.album)
					m += 4;
				if (e.title != this.title)
					m += 8;
				if (e.genre != this.genre)
					m += 16;
				if (e.track_num != this.track_num)
					m += 32;
				if (e.year != this.year)
					m += 64;

				return m;
			}
		}

		class IniFile {
			string Path;
			string EXE = Assembly.GetExecutingAssembly().GetName().Name;

			[DllImport("kernel32", CharSet = CharSet.Unicode)]
			static extern long WritePrivateProfileString(string Section, string Key, string Value, string FilePath);

			[DllImport("kernel32", CharSet = CharSet.Unicode)]
			static extern int GetPrivateProfileString(string Section, string Key, string Default, StringBuilder RetVal, int Size, string FilePath);

			public IniFile(string IniPath = null) {
				Path = new FileInfo(IniPath ?? EXE + ".ini").FullName.ToString();
			}

			public string Read(string Key, string Section = null) {
				var RetVal = new StringBuilder(255);
				GetPrivateProfileString(Section ?? EXE, Key, "", RetVal, 255, Path);
				return RetVal.ToString();
			}

			public void Write(string Key, string Value, string Section = null) {
				WritePrivateProfileString(Section ?? EXE, Key, Value, Path);
			}

			public void DeleteKey(string Key, string Section = null) {
				Write(Key, null, Section ?? EXE);
			}

			public void DeleteSection(string Section = null) {
				Write(null, null, Section ?? EXE);
			}

			public bool KeyExists(string Key, string Section = null) {
				return Read(Key, Section).Length > 0;
			}
		}

		List<entry> g_pc_data = new List<entry>();
		List<entry> g_phone_data = new List<entry>();

		List<entry> g_added = new List<entry>();
		List<entry> g_changed = new List<entry>();
		List<entry> g_removed = new List<entry>();

		public Form1() {
			InitializeComponent();

			TagLib.Id3v2.Tag.UseNumericGenres = false;

			progressBar1.Hide();

			backgroundWorkerScanSongData.DoWork += ScanSongData;
			backgroundWorkerApplySongData.DoWork += ApplyChanges;
		}

		private string ParseItunesXMLTag(string line) {
			string ret = line;

			ret = ret.Substring(ret.IndexOf('>') + 1);
			ret = ret.Substring(ret.IndexOf('>') + 1);
			ret = ret.Substring(ret.IndexOf('>') + 1);
			ret = ret.Substring(0, ret.LastIndexOf('<'));			
				
			ret = WebUtility.HtmlDecode(ret);				
			ret = System.Uri.UnescapeDataString(ret);			

			return ret;
		}

		private void LoadSongData(string dir, ref List<entry> data, string outfile) {
			string[] dirs = Directory.GetFiles(dir, "*", SearchOption.AllDirectories);
			data.Capacity = dirs.Length;

			ProgressBarSetMaximum(dirs.Length);
			ProgressBarShow(true);

			int max = dirs.Length;
			var prog = Microsoft.WindowsAPICodePack.Taskbar.TaskbarManager.Instance;
			prog.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.Normal);			

			for (int i = 0; i < dirs.Length; i++) {
				string ext = dirs[i].Substring(dirs[i].LastIndexOf('.') + 1).ToLower();

				ProgressBarSetValue(i);
				prog.SetProgressValue(i, max);

				List<string> valid_extensions = new List<string> { "mp3", "mp4", "m4a", "m4p", "wma" };
				List<string> extra_extensions = new List<string> { "jpg", "jpeg", "bmp", "png", "plist" };

				if (!valid_extensions.Contains(ext) && !extra_extensions.Contains(ext))
					//Console.WriteLine("{0}  {1}", dirs[i].Substring(dirs[i].LastIndexOf('.')), dirs[i]);
					//Print(dirs[i] + "\n");
					//File.Delete(dirs[i]);
					;//wipe useless files
				else if (valid_extensions.Contains(ext)) {
					entry d = new entry();

					int pos = dirs[i].Length - 1;
					for (int pc = 0; pos >= 0; pos--) {
						if (dirs[i][pos] == '\\') {
							if (++pc == 3)
								break;
						}
					}
					d.dir = dirs[i].Substring(pos + 1);

					try {
						TagLib.File file = TagLib.File.Create(dirs[i]);

						d.artist = file.Tag.FirstPerformer;
						d.album_artist = file.Tag.FirstAlbumArtist;
						d.album = file.Tag.Album;
						d.title = file.Tag.Title;
						d.year = file.Tag.Year.ToString();

						d.genre = file.Tag.FirstGenre;
						for (int j = 1; j < file.Tag.Genres.Length; j++) {
							d.genre += "/" + file.Tag.Genres[j];
						}

						d.track_num = file.Tag.Track.ToString();
						

						//if (d.album_artist == null && dir == g_pc_dir) {
						//	if (d.artist != null) {
						//		file.Tag.AlbumArtists = null;
						//		file.Tag.AlbumArtists = new[] { d.artist };
						//		file.Save();
						//		d.album_artist = file.Tag.FirstAlbumArtist;
						//	}
						//}

						if (dir == g_pc_dir) {
							int m = 0;
							if (d.artist == null)
								m += 1;
							if (d.album_artist == null)
								m += 2;
							if (d.album == null)
								m += 4;
							if (d.title == null)
								m += 8;
							if (d.genre == null)
								m += 16;
							if (d.track_num == null)
								m += 32;
							if (d.year == null || d.year == "0")
								m += 64;

							if (m > 0) {
								Print("Missing info found in: " + dirs[i] + "\r\n\tmissing info: ");
								if ((m & 1) > 0)
									Print("Artist. ");
								if ((m & 2) > 0)
									Print("Album Artist. ");
								if ((m & 4) > 0)
									Print("Album. ");
								if ((m & 8) > 0)
									Print("Title. ");
								if ((m & 16) > 0)
									Print("Genre. ");
								if ((m & 32) > 0)
									Print("Track #. ");
								if ((m & 64) > 0)
									Print("Year. ");
								Print("\r\n");
							}
						}

						data.Add(d);
					}
					catch (Exception ex) {
						Print("error " + ex + "\r\n--" + dirs[i] + "\r\n");

						if (ex.GetType().IsAssignableFrom(typeof(TagLib.CorruptFileException))){
							if (dir != g_pc_dir) {
								data.Add(d);
							}
						}

						continue;
					}
				}
			}

			ProgressBarShow(false);
			prog.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.NoProgress);

			using (System.IO.StreamWriter file = new System.IO.StreamWriter(getexedir() + "\\" + outfile)) {
				for (int i = 0; i < data.Count; i++) {
					file.Write(
						data[i].dir + "\r\n" +
						data[i].album_artist + "\r\n" +
						data[i].artist + "\r\n" + 
						data[i].album + "\r\n" +
						data[i].title + "\r\n" +
						data[i].genre + "\r\n" + 
						data[i].track_num + "\r\n" +
						data[i].year + "\r\n" +
						"\r\n");
				}
			}
			if (dir == g_sd_dir)
				File.Copy(getexedir() + "\\" + outfile, g_sd_dir + "\\data.txt");
		}
		private void FetchItunesData(string dir, ref List<entry> data, string outfile) {
			string[] lines = System.IO.File.ReadAllLines(dir);

			entry d = new entry();
			int artcount = 0;
			int dict_count = 0;

			bool disabled = false;

			ProgressBarSetMaximum(lines.Length / 1000);
			ProgressBarShow(true);

			int max = lines.Length / 1000;
			var prog = Microsoft.WindowsAPICodePack.Taskbar.TaskbarManager.Instance;
			prog.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.Normal);			

			for (int i = 0; i < lines.Length; i++) {
				if (i % 1000 == 0) {
					ProgressBarSetValue(i / 1000);
					prog.SetProgressValue(i / 1000, max);
				}

				if (lines[i] == "\t<key>Playlists</key>")
					break;

				if (dict_count == 3 && lines[i].IndexOf("</dict>") < 0 && lines[i].IndexOf("<dic>") < 0) {
					if (lines[i].IndexOf("\t\t\t<key>Artist</key>") >= 0) d.artist = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Album Artist</key>") >= 0) d.album_artist = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Album</key>") >= 0) d.album = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Name</key>") >= 0) d.title = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Year</key>") >= 0) d.year = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Genre</key>") >= 0) d.genre = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Track Number</key>") >= 0) d.track_num = ParseItunesXMLTag(lines[i]);
					if (lines[i].IndexOf("\t\t\t<key>Location</key>") >= 0) {
						d.dir = ParseItunesXMLTag(lines[i]); 

						d.dir = d.dir.Remove(0, "file://localhost/".Length);

						int pos = d.dir.Length - 1;
						for (int pc = 0; pos >= 0; pos--) {
							if (d.dir[pos] == '/') {
								if (++pc == 3)
									break;
							}
						}
						d.dir = d.dir.Substring(pos + 1);

						for (int j = 0, pc = 0; pc < 2; j++) {
							if (d.dir[j] == '/') {
								pc++;
								StringBuilder t = new StringBuilder(d.dir);
								t[j] = '\\';
								d.dir = t.ToString();
							}
						}
					}
					if (lines[i].IndexOf("\t\t\t<key>Artwork Count</key>") >= 0) artcount = int.Parse(ParseItunesXMLTag(lines[i]));

					if (lines[i].IndexOf("\t\t\t<key>Disabled</key>") >= 0) 
						disabled = true;
				}
				else {
					if (d.dir != null) {
						if (disabled)
							disabled = false;
						else
							data.Add(d);

						int m = 0;
						if (d.artist == null)
							m += 1;
						if (d.album_artist == null)
							m += 2;
						if (d.album == null)
							m += 4;
						if (d.title == null)
							m += 8;
						if (d.genre == null)
							m += 16;
						if (d.track_num == null)
							m += 32;
						if (d.year == null || d.year == "0")
							m += 64;
						//if (artcount == 0)
						//	m += 128;

						if (m > 0) {
							Print("Missing info found in: " + d.dir + "\r\n\tmissing info: ");
							if ((m & 1) > 0)
								Print("Artist. ");
							if ((m & 2) > 0)
								Print("Album Artist. ");
							if ((m & 4) > 0)
								Print("Album. ");
							if ((m & 8) > 0)
								Print("Title. ");
							if ((m & 16) > 0)
								Print("Genre. ");
							if ((m & 32) > 0)
								Print("Track #. ");
							if ((m & 64) > 0)
								Print("Year. ");
							if ((m & 128) > 0)
								Print("Artwork. ");
							Print("\r\n");
						}
					}					
				}

				if (lines[i].IndexOf("<dict>") >= 0)
					dict_count++;

				if (lines[i].IndexOf("</dict>") >= 0) {
					if (dict_count == 3) {
						d = new entry();
						artcount = 0;
					}
					dict_count--;
				}
			}

			ProgressBarShow(false);
			prog.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.NoProgress);

			data = data.OrderBy(a => a.dir).ToList();

			using (System.IO.StreamWriter file = new System.IO.StreamWriter(getexedir() + "\\" + outfile)) {
				for (int i = 0; i < data.Count; i++) {
					file.Write(
						data[i].dir + "\r\n" +
						data[i].album_artist + "\r\n" +
						data[i].artist + "\r\n" +
						data[i].album + "\r\n" +
						data[i].title + "\r\n" +
						data[i].genre + "\r\n" +
						data[i].track_num + "\r\n" +
						data[i].year + "\r\n" +
						"\r\n");
				}
			}
			if (dir == g_sd_dir)
				File.Copy(getexedir() + "\\" + outfile, g_sd_dir + "\\data.txt");
		}
		private void LoadSongDataFromText(string[] song_data, ref List<entry> data) {
			for (int i = 0; i < song_data.Length; i += 9) {
				try {
					entry d = new entry();
					d.dir = song_data[i + 0];
					d.album_artist = song_data[i + 1];
					d.artist = song_data[i + 2];
					d.album = song_data[i + 3];
					d.title = song_data[i + 4];
					d.genre = song_data[i + 5];
					d.track_num = song_data[i + 6];
					d.year = song_data[i + 7];

					//if (dir.Substring(dir.LastIndexOf('\\') + 1) == "pcdata.txt") {
					//	int m = 0;
					//	if (d.artist == "")
					//		m += 1;
					//	if (d.album_artist == "")
					//		m += 2;
					//	if (d.album == "")
					//		m += 4;
					//	if (d.title == "")
					//		m += 8;
					//	if (d.genre == "")
					//		m += 16;
					//	if (m > 0) {
					//		Print("Missing info found in: " + d.dir + "\r\n\tmissing info: ");
					//		if ((m & 1) > 0)
					//			Print("Artist. ");
					//		if ((m & 2) > 0)
					//			Print("Album Artist. ");
					//		if ((m & 4) > 0)
					//			Print("Album. ");
					//		if ((m & 8) > 0)
					//			Print("Title. ");
					//		if ((m & 16) > 0)
					//			Print("Genre. ");
					//		Print("\r\n");
					//	}
					//}

					data.Add(d);
				}
				catch (Exception ex) {
					//Print(ex + "\r\n");
				}
			}
		}
		private void LoadSongDataFromFile(string dir, ref List<entry> data) {
			string[] lines = System.IO.File.ReadAllLines(dir);
			LoadSongDataFromText(lines, ref data);
		}
		private void ScanSongData(object sender, System.ComponentModel.DoWorkEventArgs e) {
			EnableScanButton(false);

			g_pc_data.Clear();
			g_phone_data.Clear();
			g_added.Clear();
			g_removed.Clear();
			g_changed.Clear();


			List<entry> backup_phone_data = new List<entry>();
			if (File.Exists(getexedir() + "\\phonedata.txt"))
				LoadSongDataFromFile(getexedir() + "\\phonedata.txt", ref backup_phone_data);

			Print("Fetching PC data...\r\n");
		//	LoadSongData(g_pc_dir, ref g_pc_data, "pcdata.txt");
			FetchItunesData(g_itunes_xml_path, ref g_pc_data, "pcdata.txt");

			Print("Fetching phone data...\r\n");

			if (rdio_SD.Checked && Directory.Exists(g_sd_dir)) {
				if (File.Exists(g_sd_dir + "\\data.txt"))
					LoadSongDataFromFile(g_sd_dir + "\\data.txt", ref g_phone_data);
				else
					LoadSongData(g_sd_dir, ref g_phone_data, "data.txt");
			}
			else {
				g_phone_data = backup_phone_data;
			}

			Print("Scanning for differences...\r\n");

			for (int i = 0; i < g_pc_data.Count; i++) {
				var pos = g_phone_data.Find(a => a.dir.Substring(0, a.dir.LastIndexOf('.')) == g_pc_data[i].dir.Substring(0, g_pc_data[i].dir.LastIndexOf('.')));
				if (pos != null) {
					int res = pos.compare(g_pc_data[i]);
					if (res > 0) {
						g_changed.Add(g_pc_data[i]);

						string missing = "";
						if (res > 0) {
							if ((res & 1) > 0)
								missing = "Artist. ";
							if ((res & 2) > 0)
								missing = "Album Artist. ";
							if ((res & 4) > 0)
								missing = "Album. ";
							if ((res & 8) > 0)
								missing = "Title. ";
							if ((res & 16) > 0)
								missing = "Genre. ";
							if ((res & 32) > 0)
								missing = "Track #. ";
							if ((res & 64) > 0)
								missing = "Year. ";
						}

						Print("change detected: " + g_pc_data[i].dir + ": " + missing + "\r\n");
					}
				}
			}

			for (int i = 0; i < g_pc_data.Count; i++) {
				var pos = g_phone_data.Find(a => a.dir.Substring(0, a.dir.LastIndexOf('.')) == g_pc_data[i].dir.Substring(0, g_pc_data[i].dir.LastIndexOf('.')));
				if (pos == null) {
					g_added.Add(g_pc_data[i]);
					Print("new song detected: " + g_pc_data[i].dir + "\r\n");
				}
			}

			for (int i = 0; i < g_phone_data.Count; i++) {
				string pd = g_phone_data[i].dir.Substring(0, g_phone_data[i].dir.LastIndexOf('.'));
				var pos = g_pc_data.Find(a => a.dir.Substring(0, a.dir.LastIndexOf('.')) == g_phone_data[i].dir.Substring(0, g_phone_data[i].dir.LastIndexOf('.')));
				if (pos == null) {
					g_removed.Add(g_phone_data[i]);
					Print("removal detected: " + g_phone_data[i].dir + "\r\n");
				}
			}

			Print("Scan complete.\r\n");

			if (g_removed.Count == 0 && g_added.Count == 0 && g_changed.Count == 0)
				Print("No differences detected.\r\n");
			else
				EnableApplyButton(true);

			EnableScanButton(true);			
		}

		private int SetSongMetadata(string path, entry data) {
			try {
				TagLib.File file = TagLib.File.Create(path);

				//file.Tag.Performers = null;
				file.Tag.Performers = new[] { data.artist };

				//file.Tag.AlbumArtists = null;
				file.Tag.AlbumArtists = new[] { data.album_artist };

				file.Tag.Album = data.album;

				file.Tag.Title = data.title;

				//file.Tag.Genres = null;
				string[] genres = data.genre.Split('/');
				file.Tag.Genres = genres;

				file.Tag.Track = (uint)Convert.ToInt32(data.track_num);

				file.Tag.Year = (uint)Convert.ToInt32(data.year);

				file.Save();

				Print("changed: " + path + "\r\n");
			}
			catch (Exception ex) {
				if (ex.GetType().IsAssignableFrom(typeof(System.IO.FileNotFoundException))) {
					return 2;
				}
				else {
					Print("error " + ex + "\r\n");
					return 1;
				}
			}
			return 0;
		}
		private void ApplyChanges(object sender, System.ComponentModel.DoWorkEventArgs e) {
			EnableApplyButton(false);

			ProgressBarSetMaximum(g_changed.Count + g_added.Count + g_removed.Count);
			ProgressBarSetValue(0);
			ProgressBarShow(true);

			int max = g_changed.Count + g_added.Count + g_removed.Count;
			var prog = Microsoft.WindowsAPICodePack.Taskbar.TaskbarManager.Instance;
			prog.SetProgressValue(0, max);
			prog.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.Normal);

			if (g_changed.Count > 0) {
				Print("Applying changes...\r\n");				
				for (int i = 0; i < g_changed.Count; i++) {
					string dir = g_sd_dir + "\\" + g_changed[i].dir;
					int res = SetSongMetadata(dir, g_changed[i]);
					if (res == 2) {
						string pdir = dir.Substring(0, dir.LastIndexOf('\\'));
						string[] dirs = Directory.GetFiles(pdir, "*", SearchOption.AllDirectories);
						int start = g_changed[i].dir.LastIndexOf('\\') + 1;
						string songname = g_changed[i].dir.Substring(start, g_changed[i].dir.LastIndexOf('.') - start);
						for (int j = 0; j < dirs.Length; j++) {
							start = dirs[j].LastIndexOf('\\') + 1;
							if (dirs[j].Substring(start, dirs[j].LastIndexOf('.') - start) == songname) {
								res = SetSongMetadata(dirs[j], g_changed[i]);
								ProgressBarSetValue(i);
								prog.SetProgressValue(i, max);
								break;
							}
						}
					}
				}				
			}

			if (g_removed.Count > 0) {
				Print("Removing songs...\r\n");
				for (int i = 0; i < g_removed.Count; i++) {
					string dir = g_sd_dir + "\\" + g_removed[i].dir;
					Print("Deleting " + dir + "\r\n");
					try {
						File.Delete(dir);

						string directory = g_sd_dir + "\\" + (g_removed[i].dir.Substring(0, g_removed[i].dir.LastIndexOf("\\")));
						List<string> files = Directory.GetFiles(directory).ToList();
						if (files.Count == 0 &&
							Directory.GetDirectories(directory).Length == 0) {
							Directory.Delete(directory, false);
							Print("Deleted Directory: " + directory + "\r\n");
						}

						ProgressBarSetValue(i + g_changed.Count);
						prog.SetProgressValue(i + g_changed.Count, max);
					}
					catch (Exception ex) {
						Print(ex + "\r\n");
					}
				}
			//	Print("Cleaning...\r\n");
			//	cleanDirectory(g_sd_dir);
			}

			if (g_added.Count > 0) {
				Print("Adding songs...\r\n");
				for (int i = 0; i < g_added.Count; i++) {
					int pos = g_added[i].dir.Length - 1;
					for (int pc = 0; pos >= 0; pos--) {
						if (g_added[i].dir[pos] == '\\') {
							if (++pc == 3)
								break;
						}
					}
					string dir = g_added[i].dir.Substring(pos + 1);

					string	src = g_pc_dir + "\\" + g_added[i].dir,
							dest = g_sd_dir + "\\" + dir;
					Print("Copying " + src + "\r\n\tto " + dest + "\r\n");
					try {
						File.Copy(src, dest, true);
						ProgressBarSetValue(i + g_changed.Count + g_removed.Count);
						prog.SetProgressValue(i + g_changed.Count + g_removed.Count, max);
					}
					catch (Exception ex) {
						if (ex.GetType().IsAssignableFrom(typeof(System.IO.DirectoryNotFoundException))) {
							try {
								System.IO.Directory.CreateDirectory(g_sd_dir + "\\" + dir.Substring(0, dir.LastIndexOf('\\')));
							}
							catch (Exception ex1) {
								Print(ex1 + "\r\n");
							}
							try {
								File.Copy(src, dest, true);
							}
							catch (Exception ex1) {
								Print(ex1 + "\r\n");
							}
						}
						else {
							Print(ex + "\r\n");
						}
					}
				}
			}

			try {
				File.Copy(getexedir() + "\\" + "pcdata.txt", g_sd_dir + "\\" + "data.txt", true);
				File.Copy(getexedir() + "\\" + "pcdata.txt", getexedir() + "\\" + "phonedata.txt", true);
			}
			catch (Exception ex) {
				Print(ex + "\r\n");
			}

			ProgressBarShow(false);
			prog.SetProgressState(Microsoft.WindowsAPICodePack.Taskbar.TaskbarProgressBarState.NoProgress);

			g_added.Clear();
			g_changed.Clear();
			g_removed.Clear();

			Print("Done\r\n");
		}

		private void Form1_Load(object sender, EventArgs e) {
			IniFile ini = new IniFile("MusicLibPaths.ini");

			//public string g_pc_dir = @"C:\Music\Music";
			////public string g_sd_dir = @"Z:";
			//public string g_sd_dir = @"P:\Music";
			//public string g_itunes_xml_path = @"C:\Users\Josh_2.Josh-PC\Music\iTunes\iTunes Music Library.xml";


			if (!ini.KeyExists("PCLibraryDir", "Directories")) {
				ini.Write("PCLibraryDir", "", "Directories");
				MessageBox.Show("Missing PC library directory. Set the directory in MusicLibPaths.ini");
			}
			else {
				g_pc_dir = ini.Read("PCLibraryDir", "Directories");
			}
			if (!ini.KeyExists("SDCardDir", "Directories")) {
				ini.Write("SDCardDir", "", "Directories");
				MessageBox.Show("Missing SD Card directory. Set the directory in MusicLibPaths.ini");
			}
			else {
				g_sd_dir = ini.Read("SDCardDir", "Directories");
			}
			if (!ini.KeyExists("itunesXMLDir", "Directories")) {
				ini.Write("itunesXMLDir", "", "Directories");
				MessageBox.Show("Missing itunes XML directory. Set the directory in MusicLibPaths.ini");
			}
			else {
				g_itunes_xml_path = ini.Read("itunesXMLDir", "Directories");
			}

			if (g_pc_dir.Length == 0 || g_sd_dir.Length == 0  || g_itunes_xml_path.Length == 0) {
				Application.Exit();
			}

			EnableApplyButton(false);
		}

		private void button1_Click(object sender, EventArgs e) {
			backgroundWorkerApplySongData.RunWorkerAsync();		
		}

		private void button2_Click(object sender, EventArgs e) {
			backgroundWorkerScanSongData.RunWorkerAsync();
		}

		private void cleanDirectory(string startLocation) {
			foreach (var directory in Directory.GetDirectories(startLocation)) {
				cleanDirectory(directory);
				List<string> files = Directory.GetFiles(directory).ToList();
				for (int i = 0; i < files.Count; i++) {
					if (files[i].IndexOf(".jpg") >= 0) {						
						try {
							File.Delete(files[i]);
							Print("Deleted: " + files[i] + "\r\n");
						}
						catch (Exception ex) {
							Print(ex + "\r\n");
						}

						files.Remove(files[i]);

						if (files.Count == 0)
							Console.Write("");
					}
				}
				if (files.Count == 0 &&
					Directory.GetDirectories(directory).Length == 0) {
					Directory.Delete(directory, false);
					Print("Deleted Directory: " + directory + "\r\n");
				}
			}
		}

		private void Print(string text) {
			if (this.richTextBox1.InvokeRequired) {
				PrintCallback d = new PrintCallback(Print);
				this.Invoke(d, new object[] { text });
			}
			else {
				this.richTextBox1.AppendText(text);
			}
		}
		private void EnableScanButton(bool enabled) {
			if (this.button2.InvokeRequired) {
				EnableScanButtonCallback d = new EnableScanButtonCallback(EnableScanButton);
				this.Invoke(d, new object[] { enabled });
			}
			else {
				this.button2.Enabled = enabled;
			}
		}
		private void EnableApplyButton(bool enabled) {
			if (this.button1.InvokeRequired) {
				EnableApplyButtonCallback d = new EnableApplyButtonCallback(EnableApplyButton);
				this.Invoke(d, new object[] { enabled });
			}
			else {
				this.button1.Enabled = enabled;
			}
		}
		private void ProgressBarShow(bool show) {
			if (this.progressBar1.InvokeRequired) {
				ProgressBarShowCallback d = new ProgressBarShowCallback(ProgressBarShow);
				this.Invoke(d, new object[] { show });
			}
			else {
				if (show)
					progressBar1.Show();
				else
					progressBar1.Hide();
			}
		}
		private void ProgressBarSetMaximum(int max) {
			if (this.progressBar1.InvokeRequired) {
				ProgressBarSetMaximumCallback d = new ProgressBarSetMaximumCallback(ProgressBarSetMaximum);
				this.Invoke(d, new object[] { max });
			}
			else {
				progressBar1.Maximum = max;
			}
		}
		private void ProgressBarSetValue(int val) {
			if (this.progressBar1.InvokeRequired) {
				ProgressBarSetValueCallback d = new ProgressBarSetValueCallback(ProgressBarSetValue);
				this.Invoke(d, new object[] { val });
			}
			else {
				progressBar1.Value = val;
			}
		}

	}
}
