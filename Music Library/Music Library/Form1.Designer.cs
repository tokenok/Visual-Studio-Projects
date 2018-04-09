namespace Music_Library {
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
			this.richTextBox1 = new System.Windows.Forms.RichTextBox();
			this.button1 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.rdio_SD = new System.Windows.Forms.RadioButton();
			this.rdio_USB = new System.Windows.Forms.RadioButton();
			this.rdio_FTP = new System.Windows.Forms.RadioButton();
			this.SuspendLayout();
			// 
			// richTextBox1
			// 
			this.richTextBox1.HideSelection = false;
			this.richTextBox1.Location = new System.Drawing.Point(13, 13);
			this.richTextBox1.Name = "richTextBox1";
			this.richTextBox1.ReadOnly = true;
			this.richTextBox1.Size = new System.Drawing.Size(805, 420);
			this.richTextBox1.TabIndex = 0;
			this.richTextBox1.Text = "";
			// 
			// button1
			// 
			this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.button1.Location = new System.Drawing.Point(13, 439);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(321, 23);
			this.button1.TabIndex = 1;
			this.button1.Text = "Apply";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// button2
			// 
			this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.button2.Location = new System.Drawing.Point(340, 439);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(309, 23);
			this.button2.TabIndex = 2;
			this.button2.Text = "Scan";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// progressBar1
			// 
			this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.progressBar1.Location = new System.Drawing.Point(340, 439);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(309, 23);
			this.progressBar1.TabIndex = 3;
			// 
			// rdio_SD
			// 
			this.rdio_SD.AutoSize = true;
			this.rdio_SD.Checked = true;
			this.rdio_SD.Location = new System.Drawing.Point(655, 442);
			this.rdio_SD.Name = "rdio_SD";
			this.rdio_SD.Size = new System.Drawing.Size(65, 17);
			this.rdio_SD.TabIndex = 4;
			this.rdio_SD.TabStop = true;
			this.rdio_SD.Text = "SD Card";
			this.rdio_SD.UseVisualStyleBackColor = true;
			// 
			// rdio_USB
			// 
			this.rdio_USB.AutoSize = true;
			this.rdio_USB.Location = new System.Drawing.Point(726, 442);
			this.rdio_USB.Name = "rdio_USB";
			this.rdio_USB.Size = new System.Drawing.Size(47, 17);
			this.rdio_USB.TabIndex = 5;
			this.rdio_USB.Text = "USB";
			this.rdio_USB.UseVisualStyleBackColor = true;
			// 
			// rdio_FTP
			// 
			this.rdio_FTP.AutoSize = true;
			this.rdio_FTP.Location = new System.Drawing.Point(779, 442);
			this.rdio_FTP.Name = "rdio_FTP";
			this.rdio_FTP.Size = new System.Drawing.Size(45, 17);
			this.rdio_FTP.TabIndex = 6;
			this.rdio_FTP.Text = "FTP";
			this.rdio_FTP.UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(830, 475);
			this.Controls.Add(this.rdio_FTP);
			this.Controls.Add(this.rdio_USB);
			this.Controls.Add(this.rdio_SD);
			this.Controls.Add(this.progressBar1);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.richTextBox1);
			this.Name = "Form1";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Form1";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.RichTextBox richTextBox1;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.ProgressBar progressBar1;
		private System.Windows.Forms.RadioButton rdio_SD;
		private System.Windows.Forms.RadioButton rdio_USB;
		private System.Windows.Forms.RadioButton rdio_FTP;
	}
}

