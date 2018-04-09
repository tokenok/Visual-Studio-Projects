using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;   // Must include this!!!!
using System.Threading;

namespace Genetic_Tour {
	public partial class Form1 : Form {
		public Random rng = new Random();

		public class origin {
			public string orig;
			public List<destination> destinations = new List<destination>();
		}

		public class destination {
			public string dest;
			public int dist;
		}

		public class route {
			public List<int> gene = new List<int>();
			public int total_dist;

			public route() {

			}

			public route(route r) {
				this.gene = r.gene.ToList();
				this.total_dist = r.total_dist;
			}
		}

		public void Shuffle(List<int> list) {
			int n = list.Count;

			while (n > 1) {
				n--;
				int k = rng.Next(n + 1);
				int value = list[k];
				list[k] = list[n];
				list[n] = value;
			}
		}

		List<origin> origins = new List<origin>();

		public Form1() {
			InitializeComponent();
		}

		// Google Requirements
		private String gmaps_API = "AIzaSyBJbA3Sp9cfBIkjY_f50trS_EgaySPKuhA";   // This is a private key that each person must acquire from the Google Developers website.
		private String Distance_Matrix_URL = "https://maps.googleapis.com/maps/api/distancematrix/json?";    // This is the URL that we will make requests to.
		private string filename = "candy1.txt";

		// Step One
		// Capture Input, we need starting locations, as well as potential destinations
		public List<String> my_destinations = new List<String>();

		private void Form1_Load(object sender, EventArgs e) {
			Add_Cities();
		//	InvokeGoogleAPI();

			string text = "";
			using (System.IO.StreamReader file = new System.IO.StreamReader(filename)) {
				text += file.ReadToEnd();
			}

			ParseResponse(text);

			Genetics();
		}

		private void Add_Cities() {
			//filename = "small.txt";
			//my_destinations.Add("Bemidji, MN");
			//my_destinations.Add("Lincoln, IN 46994");
			//my_destinations.Add("Lincoln, WI");
			//my_destinations.Add("Lincoln, IA");
			//my_destinations.Add("Lincoln, MI");
			//my_destinations.Add("Lincoln, NE");
			//my_destinations.Add("Lincoln, IL");
			//my_destinations.Add("Lincoln, KS");
			//my_destinations.Add("Sault Ste. Marie, MI");

			filename = "big.txt";
			my_destinations.Add("Bemidji, MN");
			my_destinations.Add("Lincoln, UT");
			my_destinations.Add("Phoenix, AZ");
			my_destinations.Add("Lincoln City, OR");
			my_destinations.Add("Lincoln, TX");
			my_destinations.Add("Lincoln, Montana");
			my_destinations.Add("Lincoln, ND");
			my_destinations.Add("Lincoln, IL");
			my_destinations.Add("Lincoln, CA");
			my_destinations.Add("Lincoln, WI");
			my_destinations.Add("Sault Ste. Marie, MI");
			my_destinations.Add("Lincoln, NE");
			my_destinations.Add("Lincoln, NH");
			my_destinations.Add("Lincoln, VA");
			my_destinations.Add("Lincoln, MA");
			my_destinations.Add("Lincoln, AL");
			my_destinations.Add("Lincolnton, GA");
			my_destinations.Add("Lincoln, KS");
			my_destinations.Add("Lincoln Village, OH");
			my_destinations.Add("Greeley, CO");
			my_destinations.Add("Orlando, FL");
			my_destinations.Add("Lincoln, MI");
			my_destinations.Add("Lincoln 46994, IN");
			my_destinations.Add("Lincoln, ME");
			my_destinations.Add("Lincoln, PA");
			my_destinations.Add("Lincoln, NY");
			my_destinations.Add("Lincoln, NM");
			my_destinations.Add("Lincoln, MD");
			my_destinations.Add("Buffalo, WY");
			my_destinations.Add("Lincoln, IA");

			//filename = "custom.txt";
			//my_destinations.Add("Alabama,Montgomery");
			//my_destinations.Add("Arizona,Phoenix");
			//my_destinations.Add("Arkansas,Little Rock");
			//my_destinations.Add("California,Sacramento");
			//my_destinations.Add("Colorado,Denver");
			//my_destinations.Add("Connecticut,Hartford");
			//my_destinations.Add("Delaware,Dover");
			//my_destinations.Add("Florida,Tallahassee");
			//my_destinations.Add("Georgia,Atlanta");
			//my_destinations.Add("Idaho,Boise");
			//my_destinations.Add("Illinois,Springfield");
			//my_destinations.Add("Indiana,Indianapolis");
			//my_destinations.Add("Iowa,Des Moines");
			//my_destinations.Add("Kansas,Topeka");
			//my_destinations.Add("Kentucky,Frankfort");
			//my_destinations.Add("Louisiana,Baton Rouge");
			//my_destinations.Add("Maine,Augusta");
			//my_destinations.Add("Maryland,Annapolis");
			//my_destinations.Add("Massachusetts,Boston");
			//my_destinations.Add("Michigan,Lansing");
			//my_destinations.Add("Minnesota,Saint Paul");
			//my_destinations.Add("Mississippi,Jackson");
			//my_destinations.Add("Missouri,Jefferson City");
			//my_destinations.Add("Montana,Helena");
			//my_destinations.Add("Nebraska,Lincoln");
			//my_destinations.Add("Nevada,Carson City");
			//my_destinations.Add("New Hampshire,Concord");
			//my_destinations.Add("New Jersey,Trenton");
			//my_destinations.Add("New Mexico,Santa Fe");
			//my_destinations.Add("New York,Albany");
			//my_destinations.Add("North Carolina,Raleigh");
			//my_destinations.Add("North Dakota,Bismarck");
			//my_destinations.Add("Ohio,Columbus");
			//my_destinations.Add("Oklahoma,Oklahoma City");
			//my_destinations.Add("Oregon,Salem");
			//my_destinations.Add("Pennsylvania,Harrisburg");
			//my_destinations.Add("Rhode Island,Providence");
			//my_destinations.Add("South Carolina,Columbia");
			//my_destinations.Add("South Dakota,Pierre");
			//my_destinations.Add("Tennessee,Nashville");
			//my_destinations.Add("Texas,Austin");
			//my_destinations.Add("Utah,Salt Lake City");
			//my_destinations.Add("Vermont,Montpelier");
			//my_destinations.Add("Virginia,Richmond");
			//my_destinations.Add("Washington,Olympia");
			//my_destinations.Add("West Virginia,Charleston");
			//my_destinations.Add("Wisconsin,Madison");
			//my_destinations.Add("Wyoming,Cheyenne");
		}

		// This is the function that will create the URL, and send for the request to the Google API
		private void InvokeGoogleAPI() {
			using (System.IO.StreamWriter file = new System.IO.StreamWriter(filename)) {
				file.WriteLine("");
			}

			String request_URL = "";

			String origins = "origins=";
			String destinations = "destinations=";
			for (int i = 0; i < my_destinations.Count - 1; i++) {
				string origin = origins + my_destinations[i].Replace(" ", "+");
				for (int j = i + 1; j < my_destinations.Count; j++) {
					string destination = destinations + my_destinations[j].Replace(" ", "+");

					request_URL = Distance_Matrix_URL + origin + "&" + destination + "&units=metric&key=" + gmaps_API;

					Console.WriteLine(request_URL);

					Send_Request(request_URL);

					System.Threading.Thread.Sleep(10);
				}
			}

			//String request_URL = Distance_Matrix_URL;

			//String origins = "origins=";
			//String destinations = "destinations=";
			//for (int i = 0; i < my_destinations.Count; i++) {
			//	origins = origins + my_destinations[i].Replace(" ", "+") + "|";  // make origins, replace spaces with + sign
			//	destinations = destinations + my_destinations[i].Replace(" ", "+") + "|";  // make destinations, replace spaces with + sign
			//}

			//request_URL = request_URL + origins + "&" + destinations + "&units=metric&key=" + gmaps_API;

			//Send_Request(request_URL);
		}

		// Send Request!
		private void Send_Request(String url) {
			try {
				var req = (HttpWebRequest)WebRequest.Create(url);
				var res = (HttpWebResponse)req.GetResponse();

				GetResponse(res);
			}
			catch (Exception e) {
				Console.WriteLine(e);
			}
		}

		// Read in text!
		private void GetResponse(HttpWebResponse response) {
			String responeText = "";

			var encoding = ASCIIEncoding.ASCII;
			using (var reader = new System.IO.StreamReader(response.GetResponseStream(), encoding)) {
				responeText = reader.ReadToEnd();
			}

			using (System.IO.StreamWriter file = new System.IO.StreamWriter(filename, true)) {
				file.WriteLine(responeText);
			}

			ParseResponse(responeText);
		}

		private void ParseResponse(string r) {
			string[] lines = r.Split('{');

			int o = 0, d = 0;

			origin or = new origin();
			string val = "";

			for (int i = 0; i < lines.Length; i++) {
				int pos = lines[i].IndexOf("value");

				if (pos >= 0 && lines[i - 1].IndexOf("distance") >= 0) {
					string sub = lines[i].Substring(pos + 9);
					val = sub.Substring(0, sub.IndexOf("\n"));

					destination e = new destination();
					e.dest = my_destinations[d + 1];
					e.dist = Convert.ToInt32(val);
					or.destinations.Add(e);

					d++;

					if (d > my_destinations.Count - 2) {
						or.orig = my_destinations[o];

						o++;
						d = o;

						origins.Add(or);
						or = new origin();
					}
				}
			}

			for (int i = 0; i < origins.Count; i++) {
				while (origins[i].destinations.Count < my_destinations.Count) {
					origins[i].destinations.Insert(0, new destination());
				}
			}
		}

		private int get_dist(List<int> pop, bool loop = true) {
			if (pop.Count < 2)
				return 0;
			try {
				int dist = 0;
				for (int i = 0; i < pop.Count - 1; i++) {
					int o = pop[i] > pop[i + 1] ? pop[i + 1] : pop[i];
					int d = pop[i] > pop[i + 1] ? pop[i] : pop[i + 1];
					dist += origins[o].destinations[d].dist;
				}
				if (loop) {
					int o1 = pop[0] > pop[pop.Count - 1] ? pop[pop.Count - 1] : pop[0];
					int d1 = pop[0] > pop[pop.Count - 1] ? pop[0] : pop[pop.Count - 1];
					dist += origins[o1].destinations[d1].dist;
				}

				return dist;
			}
			catch (Exception e) {
				Console.WriteLine(e);
			}
			return -1;
		}

		private int find(List<int> a, int n) {
			for (int i = 0; i < a.Count; i++) {
				if (a[i] == n)
					return i;
			}
			return -1;
		}

		private void Crossover(ref List<int> a1, ref List<int> b1) {
			List<int> a = new List<int>(a1);
			List<int> b = new List<int>(b1);

			int c = rng.Next(a.Count / 10, a.Count - 1);
		//	int amin = int.MaxValue;
		//	int ami = -1;
		//	int bmin = int.MaxValue;
		//	int bmi = -1;
			//for (int i = 0; i < a.Count - c; i++) {
			//	int asum = get_dist(new List<int>(a.GetRange(i, c)), false);
			//	int bsum = get_dist(new List<int>(b.GetRange(i, c)), false);
			//	if (asum < amin) {
			//		amin = asum;
			//		ami = i;
			//	}
			//	if (bsum < bmin) {
			//		bmin = bsum;
			//		bmi = i;
			//	}
			//}

			int ami = rng.Next(0, a.Count - c);
			int bmi = rng.Next(0, a.Count - c);

			List<int> ac = a.GetRange(ami, c).ToList();
			a.RemoveRange(ami, c);
			List<int> bc = b.GetRange(bmi, c).ToList();
			b.RemoveRange(bmi, c);

			List<int> a2 = new List<int>(a1);
			for (int i = 0; i < a.Count / 2; i++) {
				int k = find(a, b[i]);
				if (k == -1)
					continue;
				int t = a[i];
				a[i] = a[k];
				a[k] = t;
			}
			for (int i = 0; i < b.Count / 2; i++) {
				int k = find(b, a2[i]);
				if (k == -1)
					continue;
				int t = b[i];
				b[i] = b[k];
				b[k] = t;
			}

			a.InsertRange(rng.Next(a.Count), ac);
			b.InsertRange(rng.Next(b.Count), bc);
			a1 = a;
			b1 = b;
		}

		private void mutate(ref List<int> pop) {
			Random rnd = new Random();
			int a = rnd.Next(0, pop.Count);
			int b = rnd.Next(0, pop.Count);

			int t = pop[a];
			pop[a] = pop[b];
			pop[b] = t;
		}

		private void Genetics() {
			List<route> population = new List<route>();
			int pop_size = 1000;
			int min_dist = int.MaxValue;

			//create population
			for (int p = 0; p < pop_size; p++) {
				route r = new route();
				for (int i = 0; i < my_destinations.Count; i++)
					r.gene.Add(i);
				Shuffle(r.gene);

				population.Add(r);
			}

			for (int i = 0; i < population.Count; i++)
				population[i].total_dist = get_dist(population[i].gene);

			while (true) {
				population = population.OrderBy(p => p.total_dist).ToList();

				int k = 10;
				int m = pop_size / k;

				for (int j = m - 1; j >= 0; j--) {
					for (int l = j * k; l < (j + 1) * k - 1; l++) {
						population[l] = new route(population[j]);
					}
				}

				Console.WriteLine(population[0].total_dist);

				if (population[0].total_dist < min_dist) {
					min_dist = population[0].total_dist;

					string rr = "";
					for (int j = 0; j < population[0].gene.Count; j++) {
						rr += "\'" + my_destinations[population[0].gene[j]].ToString() + "\', ";
					}
					rr += "\'" + my_destinations[population[0].gene[0]] + "\'";
					Console.WriteLine("{0} <----- best", min_dist);

					Generate_Map(min_dist, rr);
				}

				if (population.Count % 2 == 1) 
					population.Remove(population[population.Count - 1]);

				for (int i = 0; i < population.Count; i += 2) {
					Crossover(ref population[i].gene, ref population[i + 1].gene);

					if (rng.Next(0, 100) < 25)
						mutate(ref population[i].gene);

					if (rng.Next(0, 100) < 35)
						mutate(ref population[i + 1].gene);

					population[i].total_dist = get_dist(population[i].gene);
					population[i + 1].total_dist = get_dist(population[i + 1].gene);
				}
			}
		}

		private void Generate_Map(int dist, string r) {
			string first_html = @"
    <!DOCTYPE html>
    <html lang=""en"">
      <head>
        <meta charset=""utf-8"">
        <meta name=""viewport"" content=""initial-scale=1.0, user-scalable=no"">

        <title>MPN's optimal GA road trip #1</title>
        <style>
          html, body, #map-canvas {
            height: 100%;
            margin: 0px;
            padding: 0px
          }
          #panel {
            position: absolute;
            top: 5px;
            left: 50%;
            margin-left: -180px;
            z-index: 5;
            background-color: #fff;
            padding: 10px;
            border: 1px solid #999;
          }
        </style>
        <script src=""https://maps.googleapis.com/maps/api/js?v=3.exp&signed_in=true""></script>
        <script>
            var routes_list = []
            var markerOptions = {icon: ""http://maps.gstatic.com/mapfiles/markers2/marker.png""};
            var directionsDisplayOptions = {preserveViewport: true,
                                            markerOptions: markerOptions};
            var directionsService = new google.maps.DirectionsService();
            var map;

            function initialize() {
              var center = new google.maps.LatLng(39, -96);
              var mapOptions = {
                zoom: 5,
                center: center
              };
              map = new google.maps.Map(document.getElementById('map-canvas'), mapOptions);
              for (i=0; i<routes_list.length; i++) {
                routes_list[i].setMap(map);
              }
            }

            function calcRoute(start, end, routes) {

              var directionsDisplay = new google.maps.DirectionsRenderer(directionsDisplayOptions);

              var waypts = [];
              for (var i = 0; i < routes.length; i++) {
                waypts.push({
                  location:routes[i],
                  stopover:true});
                }

              var request = {
                  origin: start,
                  destination: end,
                  waypoints: waypts,
                  optimizeWaypoints: false,
                  travelMode: google.maps.TravelMode.DRIVING
              };

              directionsService.route(request, function(response, status) {
                if (status == google.maps.DirectionsStatus.OK) {
                    directionsDisplay.setDirections(response);
                }
              });

              routes_list.push(directionsDisplay);
            }

            function createRoutes(route) {
                // Google's free map API is limited to 10 waypoints so need to break into batches
                route.push(route[0]);
                var subset = 0;
                while (subset < route.length) {
                    var waypointSubset = route.slice(subset, subset + 10);

                    var startPoint = waypointSubset[0];
                    var midPoints = waypointSubset.slice(1, waypointSubset.length - 1);
                    var endPoint = waypointSubset[waypointSubset.length - 1];

                    calcRoute(startPoint, endPoint, midPoints);

                    subset += 9;
                }
            }
    ";
			string middle_html = "optimal_route = [" + r + "]";

			string last_html = @"			

            createRoutes(optimal_route);

            google.maps.event.addDomListener(window, 'load', initialize);

        </script>
      </head>
      <body>
        <div id=""map-canvas""></div>
      </body>
    </html>";

			using (System.IO.StreamWriter file = new System.IO.StreamWriter(dist.ToString() + ".html")) {
				file.WriteLine(first_html + middle_html	+ last_html);
			}
		}
	}
}




