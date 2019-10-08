using Microsoft.VisualBasic.PowerPacks;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace HW5 {
	public partial class Form1 : Form {
		private int x, y, radius;       //Circle Cordiantes, Dimenseion, and Horizontal/Vertical Velocity
		private Random rand = new Random();     //Random number generator
		private int maxCount = 4;              //Max number of Circles
		private ShapeContainer shapeContainer;  //Container that you're gonna place into your form
		private Ball[] shapes;                 //Contains all the shapes you wanna display

		public class Ball {
			public OvalShape ball;
			public double speed = 20;
			public double angle = 190;
			public double mass = 1;
			
			public Ball(){
				ball = new OvalShape();
			}

			public void UpdatePos() {
				ball.Left += (int)(speed * Math.Cos(angle * Math.PI / 180));
				ball.Top += (int)(speed * Math.Sin(angle * Math.PI / 180));
			}
		}

		public Form1() {
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e) {
			AllocConsole();
		}
		[DllImport("kernel32.dll", SetLastError = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		static extern bool AllocConsole();

		private void init() {
			shapes = new Ball[maxCount];
			shapeContainer = new ShapeContainer();
			StopBtn.Enabled = false;

			for (int i = 0; i < maxCount; i++)              //Randomly sets 20 circles
            {
				shapes[i] = new Ball();
				shapes[i].angle = rand.Next(360);
				shapes[i].speed = 3;

				x = rand.Next(this.ClientSize.Width);
				y = rand.Next(this.ClientSize.Height);
				radius = 75;
				OvalShape ovalShape = new OvalShape(x,y, radius, radius);      //Create the shape you want with it's properties
				shapes[i].ball = ovalShape;                      //Add the shape to the array

				double center = 128;
				double width = 127;
				double phase = 128;
				double frequency = Math.PI * 2 / 75;
				int red = (int)(Math.Sin(frequency * ovalShape.Size.Width + 0 + phase) * width + center);
				int green = (int)(Math.Sin(frequency * ovalShape.Size.Width + 2 + phase) * width + center);
				int blue = (int)(Math.Sin(frequency * ovalShape.Size.Width + 4 + phase) * width + center);
				ovalShape.FillStyle = FillStyle.Solid;
				ovalShape.FillColor = Color.FromArgb(red, green, blue);
			}

			for (int i = 0; i < shapes.Length; i++) {
				shapeContainer.Shapes.Add(shapes[i].ball); //Add the array of shapes to the ShapeContainer
			}

			this.Controls.Add(shapeContainer);              //Add the ShapeContainer to your form
		}

		private void PlayBtn_Click(object sender, EventArgs e) {
			timer1.Start();
			StopBtn.Enabled = true;
			PlayBtn.Enabled = false;
		}

		private void StartBtn_Click(object sender, EventArgs e) {
			init();
			//timer1.Start();
			StartBtn.Enabled = false;
			PlayBtn.Enabled = true;
		}

		private void StopBtn_Click(object sender, EventArgs e) {
		//	shapeContainer.Shapes.Clear();			
			StartBtn.Enabled = true;
			StopBtn.Enabled = false;
			PlayBtn.Enabled = true;
			timer1.Stop();
		//	this.Invalidate();
	
		}

		private bool isColliding(OvalShape shape1, OvalShape shape2) {
			double x1, y1, x2, y2, r1, r2;
			double d;

			x1 = shape1.Location.X;
			y1 = shape1.Location.Y;
			r1 = shape1.Size.Width / 2;
			x2 = shape2.Location.X;
			y2 = shape2.Location.Y;
			r2 = shape2.Size.Width / 2;
			d = Math.Sqrt(Math.Pow(x1 - x2, 2.0) + Math.Pow(y1 - y2, 2.0)) - r1 - r2;

			return d <= 0;
		}

		private void CheckBounds(Ball b) {
			double prev = b.angle;

			if (b.angle < 0)
				b.angle = 360 + b.angle;

			if (b.ball.Left < 0 && (b.angle >= 90 && b.angle <= 270)) {
				b.angle = 180 - b.angle;
			}				
			if (b.ball.Right > shapeContainer.Size.Width && (b.angle <= 90 || b.angle >= 270 || b.angle == 0)){
				b.angle = 180 - b.angle;
			}
			if (b.ball.Top <= 0 && (b.angle <= 360 && b.angle >= 180)){
				b.angle = 360 - b.angle;
			}
			if (b.ball.Bottom >= shapeContainer.Size.Height - 75 && (b.angle >= 0 && b.angle <= 180)){
				b.angle = 360 - b.angle;
			}
			if (b.angle >= 360)
				b.angle -= 360;

			if (b.angle < 0)
				b.angle = 360 + b.angle;
		}

		private void timer1_Tick(object sender, EventArgs e) {
			for (int i = 0; i < shapes.Length; i++) 
				shapes[i].UpdatePos();
			for (int i = 0; i < shapes.Length; i++) {
				CheckBounds(shapes[i]);
				for (int j = i + 1; j < shapes.Length; j++) {
					if (isColliding(shapes[i].ball, shapes[j].ball)) {
						if (shapes[i].ball.Width > shapes[j].ball.Width) {
							shapes[i].ball.Size = new Size(shapes[i].ball.Size.Width + 1, shapes[i].ball.Size.Width - 1);
							shapes[j].ball.Size = new Size(shapes[j].ball.Size.Width - 1, shapes[j].ball.Size.Width - 1);
						}
						else {

						}

						double center = 128;
						double width = 127;
						double phase = 128;
						double frequency = Math.PI * 2 / 75;
						int red = (int)(Math.Sin(frequency * shapes[i].ball.Size.Width + 0 + phase) * width + center);
						int green = (int)(Math.Sin(frequency * shapes[i].ball.Size.Width + 2 + phase) * width + center);
						int blue = (int)(Math.Sin(frequency * shapes[i].ball.Size.Width + 4 + phase) * width + center);
						shapes[i].ball.FillStyle = FillStyle.Solid;
						shapes[i].ball.FillColor = Color.FromArgb(red, green, blue);

						red = (int)(Math.Sin(frequency * shapes[j].ball.Size.Width + 0 + phase) * width + center);
						green = (int)(Math.Sin(frequency * shapes[j].ball.Size.Width + 2 + phase) * width + center);
						blue = (int)(Math.Sin(frequency * shapes[j].ball.Size.Width + 4 + phase) * width + center);
						shapes[j].ball.FillStyle = FillStyle.Solid;
						shapes[j].ball.FillColor = Color.FromArgb(red, green, blue);
					}
				}
			}
		}
	}
}