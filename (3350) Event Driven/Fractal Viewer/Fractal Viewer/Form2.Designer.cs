namespace Fractal_Viewer {
	partial class Form2 {
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
			this.components = new System.ComponentModel.Container();
			this.lb_colorbox = new System.Windows.Forms.ListView();
			this.colorDialog1 = new System.Windows.Forms.ColorDialog();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.button2 = new System.Windows.Forms.Button();
			this.label7 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.ud_colors = new System.Windows.Forms.NumericUpDown();
			this.ud_gradients = new System.Windows.Forms.NumericUpDown();
			this.rb_default = new System.Windows.Forms.RadioButton();
			this.rb_rainbow = new System.Windows.Forms.RadioButton();
			this.rb_blackwhite = new System.Windows.Forms.RadioButton();
			this.rb_random = new System.Windows.Forms.RadioButton();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.cb_edgedetect = new System.Windows.Forms.CheckBox();
			this.cb_cyclecolors = new System.Windows.Forms.CheckBox();
			this.btn_cycledirection = new System.Windows.Forms.Button();
			this.btn_savescheme = new System.Windows.Forms.Button();
			this.btn_loadscheme = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.ud_colors)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_gradients)).BeginInit();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// lb_colorbox
			// 
			this.lb_colorbox.Location = new System.Drawing.Point(12, 12);
			this.lb_colorbox.MultiSelect = false;
			this.lb_colorbox.Name = "lb_colorbox";
			this.lb_colorbox.Size = new System.Drawing.Size(37, 356);
			this.lb_colorbox.TabIndex = 0;
			this.lb_colorbox.TileSize = new System.Drawing.Size(30, 30);
			this.lb_colorbox.UseCompatibleStateImageBehavior = false;
			this.lb_colorbox.View = System.Windows.Forms.View.Tile;
			this.lb_colorbox.ItemActivate += new System.EventHandler(this.lb_colorbox_ItemActivate);
			// 
			// imageList1
			// 
			this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(91, 345);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(75, 23);
			this.button2.TabIndex = 2;
			this.button2.Text = "Close";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// label7
			// 
			this.label7.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(65, 40);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(39, 13);
			this.label7.TabIndex = 21;
			this.label7.Text = "Colors:";
			// 
			// label5
			// 
			this.label5.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(65, 15);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(55, 13);
			this.label5.TabIndex = 20;
			this.label5.Text = "Gradients:";
			// 
			// ud_colors
			// 
			this.ud_colors.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.ud_colors.Location = new System.Drawing.Point(127, 38);
			this.ud_colors.Maximum = new decimal(new int[] {
            141,
            0,
            0,
            0});
			this.ud_colors.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.ud_colors.Name = "ud_colors";
			this.ud_colors.Size = new System.Drawing.Size(48, 20);
			this.ud_colors.TabIndex = 19;
			this.ud_colors.TabStop = false;
			this.ud_colors.ThousandsSeparator = true;
			this.ud_colors.Value = new decimal(new int[] {
            32,
            0,
            0,
            0});
			this.ud_colors.ValueChanged += new System.EventHandler(this.ud_colors_ValueChanged);
			// 
			// ud_gradients
			// 
			this.ud_gradients.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.ud_gradients.Location = new System.Drawing.Point(127, 12);
			this.ud_gradients.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
			this.ud_gradients.Name = "ud_gradients";
			this.ud_gradients.Size = new System.Drawing.Size(48, 20);
			this.ud_gradients.TabIndex = 18;
			this.ud_gradients.TabStop = false;
			this.ud_gradients.ThousandsSeparator = true;
			this.ud_gradients.Value = new decimal(new int[] {
            12,
            0,
            0,
            0});
			this.ud_gradients.ValueChanged += new System.EventHandler(this.ud_gradients_ValueChanged);
			// 
			// rb_default
			// 
			this.rb_default.AutoSize = true;
			this.rb_default.Checked = true;
			this.rb_default.Location = new System.Drawing.Point(6, 19);
			this.rb_default.Name = "rb_default";
			this.rb_default.Size = new System.Drawing.Size(59, 17);
			this.rb_default.TabIndex = 22;
			this.rb_default.TabStop = true;
			this.rb_default.Text = "Default";
			this.rb_default.UseVisualStyleBackColor = true;
			this.rb_default.CheckedChanged += new System.EventHandler(this.rb_default_CheckedChanged);
			// 
			// rb_rainbow
			// 
			this.rb_rainbow.AutoSize = true;
			this.rb_rainbow.Location = new System.Drawing.Point(6, 42);
			this.rb_rainbow.Name = "rb_rainbow";
			this.rb_rainbow.Size = new System.Drawing.Size(67, 17);
			this.rb_rainbow.TabIndex = 23;
			this.rb_rainbow.Text = "Rainbow";
			this.rb_rainbow.UseVisualStyleBackColor = true;
			this.rb_rainbow.CheckedChanged += new System.EventHandler(this.rb_rainbow_CheckedChanged);
			// 
			// rb_blackwhite
			// 
			this.rb_blackwhite.AutoSize = true;
			this.rb_blackwhite.Location = new System.Drawing.Point(6, 88);
			this.rb_blackwhite.Name = "rb_blackwhite";
			this.rb_blackwhite.Size = new System.Drawing.Size(92, 17);
			this.rb_blackwhite.TabIndex = 24;
			this.rb_blackwhite.Text = "Black && White";
			this.rb_blackwhite.UseVisualStyleBackColor = true;
			this.rb_blackwhite.CheckedChanged += new System.EventHandler(this.rb_blackwhite_CheckedChanged);
			// 
			// rb_random
			// 
			this.rb_random.AutoSize = true;
			this.rb_random.Location = new System.Drawing.Point(6, 65);
			this.rb_random.Name = "rb_random";
			this.rb_random.Size = new System.Drawing.Size(65, 17);
			this.rb_random.TabIndex = 25;
			this.rb_random.Text = "Random";
			this.rb_random.UseVisualStyleBackColor = true;
			this.rb_random.CheckedChanged += new System.EventHandler(this.rb_random_CheckedChanged);
			this.rb_random.MouseClick += new System.Windows.Forms.MouseEventHandler(this.rb_random_MouseClick);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.rb_default);
			this.groupBox1.Controls.Add(this.rb_random);
			this.groupBox1.Controls.Add(this.rb_rainbow);
			this.groupBox1.Controls.Add(this.rb_blackwhite);
			this.groupBox1.Location = new System.Drawing.Point(68, 83);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(107, 119);
			this.groupBox1.TabIndex = 26;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Coloring Modes";
			// 
			// cb_edgedetect
			// 
			this.cb_edgedetect.AutoSize = true;
			this.cb_edgedetect.Location = new System.Drawing.Point(74, 222);
			this.cb_edgedetect.Name = "cb_edgedetect";
			this.cb_edgedetect.Size = new System.Drawing.Size(91, 17);
			this.cb_edgedetect.TabIndex = 27;
			this.cb_edgedetect.Text = "Detect Edges";
			this.cb_edgedetect.UseVisualStyleBackColor = true;
			this.cb_edgedetect.CheckedChanged += new System.EventHandler(this.cb_edgedetect_CheckedChanged);
			// 
			// cb_cyclecolors
			// 
			this.cb_cyclecolors.AutoSize = true;
			this.cb_cyclecolors.Location = new System.Drawing.Point(74, 246);
			this.cb_cyclecolors.Name = "cb_cyclecolors";
			this.cb_cyclecolors.Size = new System.Drawing.Size(84, 17);
			this.cb_cyclecolors.TabIndex = 28;
			this.cb_cyclecolors.Text = "Cycle Colors";
			this.cb_cyclecolors.UseVisualStyleBackColor = true;
			this.cb_cyclecolors.CheckedChanged += new System.EventHandler(this.cb_cyclecolors_CheckedChanged);
			// 
			// btn_cycledirection
			// 
			this.btn_cycledirection.Location = new System.Drawing.Point(157, 242);
			this.btn_cycledirection.Name = "btn_cycledirection";
			this.btn_cycledirection.Size = new System.Drawing.Size(57, 23);
			this.btn_cycledirection.TabIndex = 29;
			this.btn_cycledirection.Text = "Direction";
			this.btn_cycledirection.UseVisualStyleBackColor = true;
			this.btn_cycledirection.Click += new System.EventHandler(this.btn_cycledirection_Click);
			// 
			// btn_savescheme
			// 
			this.btn_savescheme.Location = new System.Drawing.Point(68, 279);
			this.btn_savescheme.Name = "btn_savescheme";
			this.btn_savescheme.Size = new System.Drawing.Size(132, 23);
			this.btn_savescheme.TabIndex = 30;
			this.btn_savescheme.Text = "Save Scheme";
			this.btn_savescheme.UseVisualStyleBackColor = true;
			this.btn_savescheme.Click += new System.EventHandler(this.btn_savescheme_Click);
			// 
			// btn_loadscheme
			// 
			this.btn_loadscheme.Location = new System.Drawing.Point(68, 308);
			this.btn_loadscheme.Name = "btn_loadscheme";
			this.btn_loadscheme.Size = new System.Drawing.Size(132, 23);
			this.btn_loadscheme.TabIndex = 31;
			this.btn_loadscheme.Text = "Load Scheme";
			this.btn_loadscheme.UseVisualStyleBackColor = true;
			this.btn_loadscheme.Click += new System.EventHandler(this.btn_loadscheme_Click);
			// 
			// Form2
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(220, 383);
			this.Controls.Add(this.btn_loadscheme);
			this.Controls.Add(this.btn_savescheme);
			this.Controls.Add(this.btn_cycledirection);
			this.Controls.Add(this.cb_cyclecolors);
			this.Controls.Add(this.cb_edgedetect);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.ud_colors);
			this.Controls.Add(this.ud_gradients);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.lb_colorbox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Name = "Form2";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Color Options";
			this.Load += new System.EventHandler(this.Form2_Load);
			((System.ComponentModel.ISupportInitialize)(this.ud_colors)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_gradients)).EndInit();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ColorDialog colorDialog1;
		private System.Windows.Forms.Button button2;
		public System.Windows.Forms.Label label7;
		public System.Windows.Forms.Label label5;
		public System.Windows.Forms.NumericUpDown ud_colors;
		public System.Windows.Forms.NumericUpDown ud_gradients;
		public System.Windows.Forms.ListView lb_colorbox;
		public System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button btn_cycledirection;
		public System.Windows.Forms.RadioButton rb_default;
		public System.Windows.Forms.RadioButton rb_rainbow;
		public System.Windows.Forms.RadioButton rb_blackwhite;
		public System.Windows.Forms.RadioButton rb_random;
		public System.Windows.Forms.CheckBox cb_edgedetect;
		public System.Windows.Forms.CheckBox cb_cyclecolors;
		private System.Windows.Forms.Button btn_savescheme;
		private System.Windows.Forms.Button btn_loadscheme;

	}
}