namespace ClientSpaceGame {
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
			this.btncomponents = new System.Windows.Forms.Button();
			this.btntech = new System.Windows.Forms.Button();
			this.btnmanuever = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// btncomponents
			// 
			this.btncomponents.Location = new System.Drawing.Point(114, 119);
			this.btncomponents.Name = "btncomponents";
			this.btncomponents.Size = new System.Drawing.Size(75, 23);
			this.btncomponents.TabIndex = 0;
			this.btncomponents.Text = "Components";
			this.btncomponents.UseVisualStyleBackColor = true;
			this.btncomponents.Click += new System.EventHandler(this.btncomponents_Click);
			// 
			// btntech
			// 
			this.btntech.Location = new System.Drawing.Point(114, 180);
			this.btntech.Name = "btntech";
			this.btntech.Size = new System.Drawing.Size(75, 23);
			this.btntech.TabIndex = 1;
			this.btntech.Text = "tech";
			this.btntech.UseVisualStyleBackColor = true;
			this.btntech.Click += new System.EventHandler(this.btntech_Click);
			// 
			// btnmanuever
			// 
			this.btnmanuever.Location = new System.Drawing.Point(114, 247);
			this.btnmanuever.Name = "btnmanuever";
			this.btnmanuever.Size = new System.Drawing.Size(75, 23);
			this.btnmanuever.TabIndex = 2;
			this.btnmanuever.Text = "maneuvers";
			this.btnmanuever.UseVisualStyleBackColor = true;
			this.btnmanuever.Click += new System.EventHandler(this.btnmanuever_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(788, 471);
			this.Controls.Add(this.btnmanuever);
			this.Controls.Add(this.btntech);
			this.Controls.Add(this.btncomponents);
			this.Name = "Form1";
			this.Text = "Form1";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button btncomponents;
		private System.Windows.Forms.Button btntech;
		private System.Windows.Forms.Button btnmanuever;
	}
}

