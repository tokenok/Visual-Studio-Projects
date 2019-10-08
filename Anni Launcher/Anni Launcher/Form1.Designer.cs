namespace Anni_Launcher {
	partial class Form1 {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing) {
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
			this.btn_annihilus = new System.Windows.Forms.PictureBox();
			this.btn_d2pk = new System.Windows.Forms.PictureBox();
			this.btn_launch = new System.Windows.Forms.PictureBox();
			this.tb_anni_info = new System.Windows.Forms.RichTextBox();
			this.tb_d2pk_info = new System.Windows.Forms.RichTextBox();
			this.btn_close = new System.Windows.Forms.PictureBox();
			this.btn_reupdate = new System.Windows.Forms.PictureBox();
			this.cb_fullscreen = new System.Windows.Forms.CheckBox();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.lbl_downloadinfo = new System.Windows.Forms.Label();
			this.lbl_updatestatus = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.btn_annihilus)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_d2pk)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_launch)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_close)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_reupdate)).BeginInit();
			this.SuspendLayout();
			// 
			// btn_annihilus
			// 
			this.btn_annihilus.BackgroundImage = global::Anni_Launcher.Properties.Resources.anniunselected;
			this.btn_annihilus.InitialImage = null;
			this.btn_annihilus.Location = new System.Drawing.Point(24, 64);
			this.btn_annihilus.Name = "btn_annihilus";
			this.btn_annihilus.Size = new System.Drawing.Size(265, 153);
			this.btn_annihilus.TabIndex = 0;
			this.btn_annihilus.TabStop = false;
			this.btn_annihilus.Click += new System.EventHandler(this.btn_annihilus_Click);
			// 
			// btn_d2pk
			// 
			this.btn_d2pk.BackgroundImage = global::Anni_Launcher.Properties.Resources.d2pkunselected;
			this.btn_d2pk.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
			this.btn_d2pk.Location = new System.Drawing.Point(528, 72);
			this.btn_d2pk.Name = "btn_d2pk";
			this.btn_d2pk.Size = new System.Drawing.Size(209, 145);
			this.btn_d2pk.TabIndex = 1;
			this.btn_d2pk.TabStop = false;
			this.btn_d2pk.Click += new System.EventHandler(this.btn_d2pk_Click);
			// 
			// btn_launch
			// 
			this.btn_launch.BackgroundImage = global::Anni_Launcher.Properties.Resources.launch;
			this.btn_launch.Location = new System.Drawing.Point(448, 376);
			this.btn_launch.Name = "btn_launch";
			this.btn_launch.Size = new System.Drawing.Size(281, 41);
			this.btn_launch.TabIndex = 2;
			this.btn_launch.TabStop = false;
			this.btn_launch.Click += new System.EventHandler(this.btn_launch_Click);
			this.btn_launch.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_launch_MouseDown);
			this.btn_launch.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_launch_MouseUp);
			// 
			// tb_anni_info
			// 
			this.tb_anni_info.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(1)))), ((int)(((byte)(1)))), ((int)(((byte)(1)))));
			this.tb_anni_info.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.tb_anni_info.Cursor = System.Windows.Forms.Cursors.Arrow;
			this.tb_anni_info.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tb_anni_info.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(255)))));
			this.tb_anni_info.Location = new System.Drawing.Point(68, 250);
			this.tb_anni_info.Name = "tb_anni_info";
			this.tb_anni_info.ReadOnly = true;
			this.tb_anni_info.Size = new System.Drawing.Size(234, 125);
			this.tb_anni_info.TabIndex = 3;
			this.tb_anni_info.TabStop = false;
			this.tb_anni_info.Text = "- 64 Player Games\n- Increased Experience Gain\n- Hours of new Endgame Content\n- Re" +
    "vamped Sets / Uniques / Rares\n- New Items / Stats\n- Higher Resolutions";
			// 
			// tb_d2pk_info
			// 
			this.tb_d2pk_info.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(1)))), ((int)(((byte)(1)))), ((int)(((byte)(1)))));
			this.tb_d2pk_info.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.tb_d2pk_info.Cursor = System.Windows.Forms.Cursors.Arrow;
			this.tb_d2pk_info.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tb_d2pk_info.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
			this.tb_d2pk_info.Location = new System.Drawing.Point(540, 241);
			this.tb_d2pk_info.Name = "tb_d2pk_info";
			this.tb_d2pk_info.ReadOnly = true;
			this.tb_d2pk_info.Size = new System.Drawing.Size(197, 102);
			this.tb_d2pk_info.TabIndex = 4;
			this.tb_d2pk_info.TabStop = false;
			this.tb_d2pk_info.Text = "- 64 Player Games\n- Instant Level 99\n- Gear Vendors in Town\n- Spectator Mode\n- K/" +
    "D/A Scoreboard\n- Cosmetic Effects (NVs)";
			// 
			// btn_close
			// 
			this.btn_close.BackgroundImage = global::Anni_Launcher.Properties.Resources.close;
			this.btn_close.Location = new System.Drawing.Point(696, 0);
			this.btn_close.Name = "btn_close";
			this.btn_close.Size = new System.Drawing.Size(38, 38);
			this.btn_close.TabIndex = 5;
			this.btn_close.TabStop = false;
			this.btn_close.Click += new System.EventHandler(this.btn_close_Click);
			this.btn_close.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
			this.btn_close.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_close_MouseUp);
			// 
			// btn_reupdate
			// 
			this.btn_reupdate.BackgroundImage = global::Anni_Launcher.Properties.Resources.reupdate;
			this.btn_reupdate.Location = new System.Drawing.Point(16, 424);
			this.btn_reupdate.Name = "btn_reupdate";
			this.btn_reupdate.Size = new System.Drawing.Size(81, 25);
			this.btn_reupdate.TabIndex = 6;
			this.btn_reupdate.TabStop = false;
			this.btn_reupdate.Click += new System.EventHandler(this.btn_reupdate_Click);
			this.btn_reupdate.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_reupdate_MouseDown);
			this.btn_reupdate.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_reupdate_MouseUp);
			// 
			// cb_fullscreen
			// 
			this.cb_fullscreen.AutoSize = true;
			this.cb_fullscreen.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(128)))));
			this.cb_fullscreen.Location = new System.Drawing.Point(528, 424);
			this.cb_fullscreen.Name = "cb_fullscreen";
			this.cb_fullscreen.Size = new System.Drawing.Size(109, 17);
			this.cb_fullscreen.TabIndex = 7;
			this.cb_fullscreen.Text = "Full Screen Mode";
			this.cb_fullscreen.UseVisualStyleBackColor = true;
			this.cb_fullscreen.CheckedChanged += new System.EventHandler(this.cb_fullscreen_CheckedChanged);
			// 
			// progressBar1
			// 
			this.progressBar1.Location = new System.Drawing.Point(16, 391);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(397, 23);
			this.progressBar1.TabIndex = 8;
			this.progressBar1.UseWaitCursor = true;
			this.progressBar1.Visible = false;
			// 
			// lbl_downloadinfo
			// 
			this.lbl_downloadinfo.AutoSize = true;
			this.lbl_downloadinfo.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl_downloadinfo.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(128)))));
			this.lbl_downloadinfo.Location = new System.Drawing.Point(106, 420);
			this.lbl_downloadinfo.Name = "lbl_downloadinfo";
			this.lbl_downloadinfo.Size = new System.Drawing.Size(0, 13);
			this.lbl_downloadinfo.TabIndex = 9;
			this.lbl_downloadinfo.Visible = false;
			// 
			// lbl_updatestatus
			// 
			this.lbl_updatestatus.AutoSize = true;
			this.lbl_updatestatus.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl_updatestatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(128)))));
			this.lbl_updatestatus.Location = new System.Drawing.Point(16, 367);
			this.lbl_updatestatus.Name = "lbl_updatestatus";
			this.lbl_updatestatus.Size = new System.Drawing.Size(0, 16);
			this.lbl_updatestatus.TabIndex = 10;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(1)))), ((int)(((byte)(1)))), ((int)(((byte)(1)))));
			this.BackgroundImage = global::Anni_Launcher.Properties.Resources.launchformbg;
			this.ClientSize = new System.Drawing.Size(760, 491);
			this.Controls.Add(this.lbl_updatestatus);
			this.Controls.Add(this.lbl_downloadinfo);
			this.Controls.Add(this.progressBar1);
			this.Controls.Add(this.cb_fullscreen);
			this.Controls.Add(this.btn_reupdate);
			this.Controls.Add(this.btn_close);
			this.Controls.Add(this.tb_d2pk_info);
			this.Controls.Add(this.tb_anni_info);
			this.Controls.Add(this.btn_launch);
			this.Controls.Add(this.btn_d2pk);
			this.Controls.Add(this.btn_annihilus);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "Form1";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Annihilus Launcher";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.Shown += new System.EventHandler(this.Form1_Shown);
			((System.ComponentModel.ISupportInitialize)(this.btn_annihilus)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_d2pk)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_launch)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_close)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.btn_reupdate)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.PictureBox btn_annihilus;
		private System.Windows.Forms.PictureBox btn_d2pk;
		private System.Windows.Forms.PictureBox btn_launch;
		private System.Windows.Forms.RichTextBox tb_anni_info;
		private System.Windows.Forms.RichTextBox tb_d2pk_info;
		private System.Windows.Forms.PictureBox btn_close;
		private System.Windows.Forms.PictureBox btn_reupdate;
		private System.Windows.Forms.CheckBox cb_fullscreen;
		private System.Windows.Forms.ProgressBar progressBar1;
		private System.Windows.Forms.Label lbl_downloadinfo;
		private System.Windows.Forms.Label lbl_updatestatus;

	}
}

