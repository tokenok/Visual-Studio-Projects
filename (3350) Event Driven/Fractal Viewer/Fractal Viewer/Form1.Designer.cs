namespace Fractal_Viewer {
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
			this.components = new System.ComponentModel.Container();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.tb_xmin = new System.Windows.Forms.TextBox();
			this.tb_xmax = new System.Windows.Forms.TextBox();
			this.tb_ymin = new System.Windows.Forms.TextBox();
			this.tb_ymax = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.ud_iterations = new System.Windows.Forms.NumericUpDown();
			this.pb_julia = new System.Windows.Forms.PictureBox();
			this.ud_reduction = new System.Windows.Forms.NumericUpDown();
			this.label9 = new System.Windows.Forms.Label();
			this.lb_mandelbrotxpos = new System.Windows.Forms.Label();
			this.lb_mandelbrotypos = new System.Windows.Forms.Label();
			this.label12 = new System.Windows.Forms.Label();
			this.label13 = new System.Windows.Forms.Label();
			this.label14 = new System.Windows.Forms.Label();
			this.label15 = new System.Windows.Forms.Label();
			this.lb_juliaypos = new System.Windows.Forms.Label();
			this.lb_juliaxpos = new System.Windows.Forms.Label();
			this.label18 = new System.Windows.Forms.Label();
			this.label19 = new System.Windows.Forms.Label();
			this.label20 = new System.Windows.Forms.Label();
			this.label21 = new System.Windows.Forms.Label();
			this.tb_jymax = new System.Windows.Forms.TextBox();
			this.tb_jymin = new System.Windows.Forms.TextBox();
			this.tb_jxmax = new System.Windows.Forms.TextBox();
			this.tb_jxmin = new System.Windows.Forms.TextBox();
			this.label22 = new System.Windows.Forms.Label();
			this.lb_julia_c = new System.Windows.Forms.Label();
			this.btn_showcontrols = new System.Windows.Forms.Button();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.btn_palette = new System.Windows.Forms.Button();
			this.label8 = new System.Windows.Forms.Label();
			this.ud_updateinterval = new System.Windows.Forms.NumericUpDown();
			this.button1 = new System.Windows.Forms.Button();
			this.btn_record = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_iterations)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.pb_julia)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_reduction)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_updateinterval)).BeginInit();
			this.SuspendLayout();
			// 
			// pictureBox1
			// 
			this.pictureBox1.Location = new System.Drawing.Point(12, 12);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(800, 600);
			this.pictureBox1.TabIndex = 0;
			this.pictureBox1.TabStop = false;
			this.pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
			this.pictureBox1.MouseEnter += new System.EventHandler(this.pictureBox1_MouseHover);
			this.pictureBox1.MouseHover += new System.EventHandler(this.pictureBox1_MouseHover);
			this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
			this.pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
			this.pictureBox1.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseWheel);
			// 
			// tb_xmin
			// 
			this.tb_xmin.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_xmin.Location = new System.Drawing.Point(868, 10);
			this.tb_xmin.Name = "tb_xmin";
			this.tb_xmin.ReadOnly = true;
			this.tb_xmin.Size = new System.Drawing.Size(202, 20);
			this.tb_xmin.TabIndex = 1;
			this.tb_xmin.TabStop = false;
			// 
			// tb_xmax
			// 
			this.tb_xmax.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_xmax.Location = new System.Drawing.Point(868, 30);
			this.tb_xmax.Name = "tb_xmax";
			this.tb_xmax.ReadOnly = true;
			this.tb_xmax.Size = new System.Drawing.Size(202, 20);
			this.tb_xmax.TabIndex = 2;
			this.tb_xmax.TabStop = false;
			// 
			// tb_ymin
			// 
			this.tb_ymin.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_ymin.Location = new System.Drawing.Point(868, 50);
			this.tb_ymin.Name = "tb_ymin";
			this.tb_ymin.ReadOnly = true;
			this.tb_ymin.Size = new System.Drawing.Size(202, 20);
			this.tb_ymin.TabIndex = 3;
			this.tb_ymin.TabStop = false;
			// 
			// tb_ymax
			// 
			this.tb_ymax.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_ymax.Location = new System.Drawing.Point(868, 70);
			this.tb_ymax.Name = "tb_ymax";
			this.tb_ymax.ReadOnly = true;
			this.tb_ymax.Size = new System.Drawing.Size(202, 20);
			this.tb_ymax.TabIndex = 4;
			this.tb_ymax.TabStop = false;
			// 
			// label1
			// 
			this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(828, 13);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(33, 13);
			this.label1.TabIndex = 5;
			this.label1.Text = "Xmin:";
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(828, 33);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(36, 13);
			this.label2.TabIndex = 6;
			this.label2.Text = "Xmax:";
			// 
			// label3
			// 
			this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(828, 53);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(33, 13);
			this.label3.TabIndex = 7;
			this.label3.Text = "Ymin:";
			// 
			// label4
			// 
			this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(828, 73);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(36, 13);
			this.label4.TabIndex = 8;
			this.label4.Text = "Ymax:";
			// 
			// label6
			// 
			this.label6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(827, 204);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(53, 13);
			this.label6.TabIndex = 11;
			this.label6.Text = "Iterations:";
			// 
			// ud_iterations
			// 
			this.ud_iterations.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.ud_iterations.Location = new System.Drawing.Point(895, 200);
			this.ud_iterations.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
			this.ud_iterations.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.ud_iterations.Name = "ud_iterations";
			this.ud_iterations.Size = new System.Drawing.Size(58, 20);
			this.ud_iterations.TabIndex = 12;
			this.ud_iterations.TabStop = false;
			this.ud_iterations.ThousandsSeparator = true;
			this.ud_iterations.Value = new decimal(new int[] {
            500,
            0,
            0,
            0});
			this.ud_iterations.ValueChanged += new System.EventHandler(this.ud_iterations_ValueChanged);
			this.ud_iterations.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ud_reduction_KeyDown);
			// 
			// pb_julia
			// 
			this.pb_julia.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.pb_julia.BackColor = System.Drawing.SystemColors.ControlDark;
			this.pb_julia.Location = new System.Drawing.Point(831, 409);
			this.pb_julia.Name = "pb_julia";
			this.pb_julia.Size = new System.Drawing.Size(246, 203);
			this.pb_julia.TabIndex = 19;
			this.pb_julia.TabStop = false;
			this.pb_julia.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pb_julia_MouseDown);
			this.pb_julia.MouseEnter += new System.EventHandler(this.pb_julia_MouseHover);
			this.pb_julia.MouseHover += new System.EventHandler(this.pb_julia_MouseHover);
			this.pb_julia.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pb_julia_MouseMove);
			this.pb_julia.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pb_julia_MouseUp);
			this.pb_julia.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.pb_julia_MouseWheel);
			// 
			// ud_reduction
			// 
			this.ud_reduction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.ud_reduction.DecimalPlaces = 2;
			this.ud_reduction.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
			this.ud_reduction.Location = new System.Drawing.Point(895, 174);
			this.ud_reduction.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.ud_reduction.Name = "ud_reduction";
			this.ud_reduction.Size = new System.Drawing.Size(58, 20);
			this.ud_reduction.TabIndex = 20;
			this.ud_reduction.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.ud_reduction.ValueChanged += new System.EventHandler(this.ud_reduction_ValueChanged);
			this.ud_reduction.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ud_reduction_KeyDown);
			// 
			// label9
			// 
			this.label9.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(827, 177);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(60, 13);
			this.label9.TabIndex = 21;
			this.label9.Text = "Resolution:";
			// 
			// lb_mandelbrotxpos
			// 
			this.lb_mandelbrotxpos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_mandelbrotxpos.AutoSize = true;
			this.lb_mandelbrotxpos.Location = new System.Drawing.Point(845, 98);
			this.lb_mandelbrotxpos.Name = "lb_mandelbrotxpos";
			this.lb_mandelbrotxpos.Size = new System.Drawing.Size(23, 13);
			this.lb_mandelbrotxpos.TabIndex = 22;
			this.lb_mandelbrotxpos.Text = "null";
			// 
			// lb_mandelbrotypos
			// 
			this.lb_mandelbrotypos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_mandelbrotypos.AutoSize = true;
			this.lb_mandelbrotypos.Location = new System.Drawing.Point(845, 121);
			this.lb_mandelbrotypos.Name = "lb_mandelbrotypos";
			this.lb_mandelbrotypos.Size = new System.Drawing.Size(23, 13);
			this.lb_mandelbrotypos.TabIndex = 23;
			this.lb_mandelbrotypos.Text = "null";
			// 
			// label12
			// 
			this.label12.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label12.AutoSize = true;
			this.label12.Location = new System.Drawing.Point(830, 98);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(13, 13);
			this.label12.TabIndex = 24;
			this.label12.Text = "r:";
			// 
			// label13
			// 
			this.label13.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label13.AutoSize = true;
			this.label13.Location = new System.Drawing.Point(830, 120);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(12, 13);
			this.label13.TabIndex = 25;
			this.label13.Text = "i:";
			// 
			// label14
			// 
			this.label14.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label14.AutoSize = true;
			this.label14.Location = new System.Drawing.Point(829, 381);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(12, 13);
			this.label14.TabIndex = 29;
			this.label14.Text = "i:";
			// 
			// label15
			// 
			this.label15.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label15.AutoSize = true;
			this.label15.Location = new System.Drawing.Point(829, 359);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(13, 13);
			this.label15.TabIndex = 28;
			this.label15.Text = "r:";
			// 
			// lb_juliaypos
			// 
			this.lb_juliaypos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_juliaypos.AutoSize = true;
			this.lb_juliaypos.Location = new System.Drawing.Point(844, 382);
			this.lb_juliaypos.Name = "lb_juliaypos";
			this.lb_juliaypos.Size = new System.Drawing.Size(23, 13);
			this.lb_juliaypos.TabIndex = 27;
			this.lb_juliaypos.Text = "null";
			// 
			// lb_juliaxpos
			// 
			this.lb_juliaxpos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_juliaxpos.AutoSize = true;
			this.lb_juliaxpos.Location = new System.Drawing.Point(844, 359);
			this.lb_juliaxpos.Name = "lb_juliaxpos";
			this.lb_juliaxpos.Size = new System.Drawing.Size(23, 13);
			this.lb_juliaxpos.TabIndex = 26;
			this.lb_juliaxpos.Text = "null";
			// 
			// label18
			// 
			this.label18.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label18.AutoSize = true;
			this.label18.Location = new System.Drawing.Point(828, 338);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(41, 13);
			this.label18.TabIndex = 37;
			this.label18.Text = "JYmax:";
			// 
			// label19
			// 
			this.label19.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label19.AutoSize = true;
			this.label19.Location = new System.Drawing.Point(828, 318);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(38, 13);
			this.label19.TabIndex = 36;
			this.label19.Text = "JYmin:";
			// 
			// label20
			// 
			this.label20.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label20.AutoSize = true;
			this.label20.Location = new System.Drawing.Point(828, 298);
			this.label20.Name = "label20";
			this.label20.Size = new System.Drawing.Size(41, 13);
			this.label20.TabIndex = 35;
			this.label20.Text = "JXmax:";
			// 
			// label21
			// 
			this.label21.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label21.AutoSize = true;
			this.label21.Location = new System.Drawing.Point(828, 278);
			this.label21.Name = "label21";
			this.label21.Size = new System.Drawing.Size(38, 13);
			this.label21.TabIndex = 34;
			this.label21.Text = "JXmin:";
			// 
			// tb_jymax
			// 
			this.tb_jymax.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_jymax.Location = new System.Drawing.Point(868, 335);
			this.tb_jymax.Name = "tb_jymax";
			this.tb_jymax.ReadOnly = true;
			this.tb_jymax.Size = new System.Drawing.Size(202, 20);
			this.tb_jymax.TabIndex = 33;
			this.tb_jymax.TabStop = false;
			// 
			// tb_jymin
			// 
			this.tb_jymin.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_jymin.Location = new System.Drawing.Point(868, 315);
			this.tb_jymin.Name = "tb_jymin";
			this.tb_jymin.ReadOnly = true;
			this.tb_jymin.Size = new System.Drawing.Size(202, 20);
			this.tb_jymin.TabIndex = 32;
			this.tb_jymin.TabStop = false;
			// 
			// tb_jxmax
			// 
			this.tb_jxmax.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_jxmax.Location = new System.Drawing.Point(868, 295);
			this.tb_jxmax.Name = "tb_jxmax";
			this.tb_jxmax.ReadOnly = true;
			this.tb_jxmax.Size = new System.Drawing.Size(202, 20);
			this.tb_jxmax.TabIndex = 31;
			this.tb_jxmax.TabStop = false;
			// 
			// tb_jxmin
			// 
			this.tb_jxmin.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_jxmin.Location = new System.Drawing.Point(868, 275);
			this.tb_jxmin.Name = "tb_jxmin";
			this.tb_jxmin.ReadOnly = true;
			this.tb_jxmin.Size = new System.Drawing.Size(202, 20);
			this.tb_jxmin.TabIndex = 30;
			this.tb_jxmin.TabStop = false;
			// 
			// label22
			// 
			this.label22.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label22.AutoSize = true;
			this.label22.Location = new System.Drawing.Point(828, 254);
			this.label22.Name = "label22";
			this.label22.Size = new System.Drawing.Size(26, 13);
			this.label22.TabIndex = 38;
			this.label22.Text = "C = ";
			// 
			// lb_julia_c
			// 
			this.lb_julia_c.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_julia_c.AutoSize = true;
			this.lb_julia_c.Location = new System.Drawing.Point(851, 254);
			this.lb_julia_c.Name = "lb_julia_c";
			this.lb_julia_c.Size = new System.Drawing.Size(23, 13);
			this.lb_julia_c.TabIndex = 39;
			this.lb_julia_c.Text = "null";
			// 
			// btn_showcontrols
			// 
			this.btn_showcontrols.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_showcontrols.Location = new System.Drawing.Point(962, 226);
			this.btn_showcontrols.Name = "btn_showcontrols";
			this.btn_showcontrols.Size = new System.Drawing.Size(116, 21);
			this.btn_showcontrols.TabIndex = 40;
			this.btn_showcontrols.Text = "Show Controls";
			this.btn_showcontrols.UseVisualStyleBackColor = true;
			this.btn_showcontrols.Click += new System.EventHandler(this.btn_showcontrols_Click);
			// 
			// timer1
			// 
			this.timer1.Interval = 10;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// btn_palette
			// 
			this.btn_palette.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_palette.Location = new System.Drawing.Point(962, 172);
			this.btn_palette.Name = "btn_palette";
			this.btn_palette.Size = new System.Drawing.Size(116, 21);
			this.btn_palette.TabIndex = 41;
			this.btn_palette.Text = "Change Colors";
			this.btn_palette.UseVisualStyleBackColor = true;
			this.btn_palette.Click += new System.EventHandler(this.btn_palette_Click);
			// 
			// label8
			// 
			this.label8.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(827, 228);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(65, 13);
			this.label8.TabIndex = 43;
			this.label8.Text = "Draw Delay:";
			// 
			// ud_updateinterval
			// 
			this.ud_updateinterval.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.ud_updateinterval.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.ud_updateinterval.Location = new System.Drawing.Point(895, 226);
			this.ud_updateinterval.Maximum = new decimal(new int[] {
            1000000,
            0,
            0,
            0});
			this.ud_updateinterval.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.ud_updateinterval.Name = "ud_updateinterval";
			this.ud_updateinterval.Size = new System.Drawing.Size(58, 20);
			this.ud_updateinterval.TabIndex = 42;
			this.ud_updateinterval.TabStop = false;
			this.ud_updateinterval.ThousandsSeparator = true;
			this.ud_updateinterval.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
			this.ud_updateinterval.ValueChanged += new System.EventHandler(this.ud_updateinterval_ValueChanged);
			this.ud_updateinterval.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ud_reduction_KeyDown);
			// 
			// button1
			// 
			this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.button1.Location = new System.Drawing.Point(962, 199);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(116, 21);
			this.button1.TabIndex = 44;
			this.button1.Text = "Fullscreen";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// btn_record
			// 
			this.btn_record.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_record.Location = new System.Drawing.Point(830, 142);
			this.btn_record.Name = "btn_record";
			this.btn_record.Size = new System.Drawing.Size(247, 23);
			this.btn_record.TabIndex = 45;
			this.btn_record.Text = "Start Recording";
			this.btn_record.UseVisualStyleBackColor = true;
			this.btn_record.Click += new System.EventHandler(this.Record_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1089, 625);
			this.Controls.Add(this.btn_record);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.label18);
			this.Controls.Add(this.label19);
			this.Controls.Add(this.label20);
			this.Controls.Add(this.label21);
			this.Controls.Add(this.tb_jymax);
			this.Controls.Add(this.tb_jymin);
			this.Controls.Add(this.tb_jxmax);
			this.Controls.Add(this.tb_jxmin);
			this.Controls.Add(this.label14);
			this.Controls.Add(this.label15);
			this.Controls.Add(this.lb_juliaypos);
			this.Controls.Add(this.lb_juliaxpos);
			this.Controls.Add(this.pb_julia);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.ud_updateinterval);
			this.Controls.Add(this.btn_palette);
			this.Controls.Add(this.btn_showcontrols);
			this.Controls.Add(this.lb_julia_c);
			this.Controls.Add(this.label22);
			this.Controls.Add(this.label13);
			this.Controls.Add(this.label12);
			this.Controls.Add(this.lb_mandelbrotypos);
			this.Controls.Add(this.lb_mandelbrotxpos);
			this.Controls.Add(this.label9);
			this.Controls.Add(this.ud_reduction);
			this.Controls.Add(this.ud_iterations);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.tb_ymax);
			this.Controls.Add(this.tb_ymin);
			this.Controls.Add(this.tb_xmax);
			this.Controls.Add(this.tb_xmin);
			this.Controls.Add(this.pictureBox1);
			this.KeyPreview = true;
			this.Name = "Form1";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Mandelbrot and Julia Set Viewer";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
			this.Load += new System.EventHandler(this.Form1_Load);
			this.SizeChanged += new System.EventHandler(this.Form1_SizeChanged);
			this.Click += new System.EventHandler(this.Form1_Click);
			this.DoubleClick += new System.EventHandler(this.Form1_Click);
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ud_reduction_KeyDown);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_iterations)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.pb_julia)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_reduction)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ud_updateinterval)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox tb_xmin;
		private System.Windows.Forms.TextBox tb_xmax;
		private System.Windows.Forms.TextBox tb_ymin;
		private System.Windows.Forms.TextBox tb_ymax;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.PictureBox pb_julia;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label lb_mandelbrotxpos;
		private System.Windows.Forms.Label lb_mandelbrotypos;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.Label label14;
		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.Label lb_juliaypos;
		private System.Windows.Forms.Label lb_juliaxpos;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.Label label21;
		private System.Windows.Forms.TextBox tb_jymax;
		private System.Windows.Forms.TextBox tb_jymin;
		private System.Windows.Forms.TextBox tb_jxmax;
		private System.Windows.Forms.TextBox tb_jxmin;
		private System.Windows.Forms.Label label22;
		private System.Windows.Forms.Label lb_julia_c;
		private System.Windows.Forms.Button btn_showcontrols;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.Button btn_palette;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.NumericUpDown ud_updateinterval;
		private System.Windows.Forms.Button button1;
		public System.Windows.Forms.NumericUpDown ud_iterations;
		public System.Windows.Forms.NumericUpDown ud_reduction;
		public System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.Button btn_record;
	}
}

