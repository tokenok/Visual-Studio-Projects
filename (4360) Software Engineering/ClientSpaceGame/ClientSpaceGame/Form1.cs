using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ClientSpaceGame {
	public partial class Form1 : Form {
		public static Random rng = new Random();

		public int turn = 0;
		public int annualfunds = 25;
		
		public enum Team {
			TEAM_RUSSIA, TEAM_USA, TEAM_FRANCE, TEAM_CHINA
		}

		public enum ComponentType {
			COMP_CAPSULE_EAGLE, COMP_CAPSULE_VOSTOK,COMP_CAPSULE_ALDRIN, COMP_CAPSULE_APOLLO,
			COMP_ROCKET_JUNO, COMP_ROCKET_ATLAAS, COMP_ROCKET_SOYUZ, COMP_ROCKET_SATURN, 
			COMP_PROBE
		}
		public enum ComponentState {
			STATE_OK, STATE_DAMAGED, STATE_DESTROYED
		}
		public enum BugType {
			BUG_SUCCESS, BUG_MINOR, BUG_MAJOR
		}
		public class Bug {
			public BugType type;

			public Bug() {
				this.type = (BugType)rng.Next(0, 2);
			}
		}
		public class Component {
			public ComponentType type;
			public List<Bug> bugs = new List<Bug>();
			public ComponentState state;

			public Component(ComponentType type) {
				for (int i = 0; i < 3; i++)
					bugs.Add(new Bug());
				state = ComponentState.STATE_OK;
				this.type = type;
			}
		}

		public class Spacecraft {
			public List<Component> components = new List<Component>();

			public bool PerformManeuver(){
				for (int i = 0; i < components.Count; i++) {
					if (components[i].bugs.Count > 0) {
						switch (components[i].bugs[rng.Next(0, components[i].bugs.Count)].type) {
							case BugType.BUG_SUCCESS: {
								//prompt removal
								break;
							}
							case BugType.BUG_MINOR: {
								//prompt removal
								break;
							}
							case BugType.BUG_MAJOR: {
								//prompt removal
								//destroy everything if engine failure, etc
								break;
							}
						}
					}
				}
				return true;
			}
		}

		public class Player{
			public int score = 0;
			public Team team;
			public int funds = 0;
			public List<Spacecraft> rockets = new List<Spacecraft>();

			public Player(Team team){
				this.team = team;
			}

			public void doTurn() {
				bool endturn = false;
				while (funds > 0 && !endturn) {

				}
			}
		}
		
		public Form1() {
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e) {
			for (int i = 0; i < locationCards.Count; i++) {
				Console.WriteLine(locationCards[i].name);
				for (int j = 0; j < locationCards[i].toLocations.Count; j++) {
					LocationCard.Location to = locationCards[i].toLocations[j];
					Console.WriteLine("\t" + locationCards[to.id].name + " " + to.difficulty + " " + to.duration + " " + to.landing + " " + to.reentry);
				}
			}

			List<Player> players = new List<Player>();
			players.Add(new Player(Team.TEAM_RUSSIA));
			players.Add(new Player(Team.TEAM_CHINA));
			players.Add(new Player(Team.TEAM_FRANCE));
			players.Add(new Player(Team.TEAM_USA));

			//shuffle players
			int n = players.Count;
			while (n > 1) {
				n--;
				int k = rng.Next(n + 1);
				Player value = players[k];
				players[k] = players[n];
				players[n] = value;
			}

			int year = 0;
			while (true) {
				for (int i = 0; i < players.Count; i++) {
					players[i].doTurn();
				}
				year++;
			}
		}

		private void btncomponents_Click(object sender, EventArgs e) {

		}

		private void btntech_Click(object sender, EventArgs e) {

		}

		private void btnmanuever_Click(object sender, EventArgs e) {

		}

















		private static int id = 0;

		public List<LocationCard> locationCards = new List<LocationCard> { 
			new LocationCard(id, "Earth", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(1, 3, 0, false, false), 
					new LocationCard.Location(3, 8, 0, false, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(1, 0, 0, true, false),
					new LocationCard.Location(3, 0, 0, true, true)
				}
			),
			new LocationCard(++id, "Suborbital Flight", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(0, 3, 0, false, false), 
					new LocationCard.Location(3, 8, 0, false, false)}, 
				new List<LocationCard.Location>{}
			),
			new LocationCard(++id, "Lunar Fly-By", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(3, 1, -1, false, false), 
					new LocationCard.Location(4, 2, -1, false, false),
					new LocationCard.Location(5, 4, 0, true, false)},
				new List<LocationCard.Location>{
					new LocationCard.Location(3, 1, 1, false, false),
				}
			),
			new LocationCard(++id, "Earth Orbit",
				new List<LocationCard.Location>{ 
					new LocationCard.Location(7, 5, 3, false, false), 
					new LocationCard.Location(9, 3, 3, false, false),
					new LocationCard.Location(8, 3, 1, false, false),
					new LocationCard.Location(4, 3, -1, false, false),
					new LocationCard.Location(2, 1, -1, false, false),
					new LocationCard.Location(0, 0, 0, true, true)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(7, 5, 3, false, false),
					new LocationCard.Location(8, 3, 1, false, false),
					new LocationCard.Location(4, 3, -1, false, false),
					new LocationCard.Location(2, 1, -1, false, false),
					new LocationCard.Location(1, 5, 0, false, false),
					new LocationCard.Location(0, 8, 0, false, false)
				}
			),
			new LocationCard(++id, "Lunar Orbit", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(3, 3, -1, false, false), 
					new LocationCard.Location(5, 2, 0, true, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(2, 2, 1, false, false),
					new LocationCard.Location(3, 3, -1, false, false),
					new LocationCard.Location(5, 2, 0, false, false)
				}
			),
			new LocationCard(++id, "Moon", 
				new List<LocationCard.Location>{  
					new LocationCard.Location(4, 2, 0, false, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(4, 2, 0, true, false),
					new LocationCard.Location(2, 4, 0, true, false)
				}
			),
			new LocationCard(++id, "Mars", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(7, 3, 0, false, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(7, 0, 0, true, true),
					new LocationCard.Location(9, 3, 0, true, true)
				}
			),
			new LocationCard(++id, "Mars Orbit", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(6, 0, 0, true, true), 
					new LocationCard.Location(8, 4, 2, false, false), 
					new LocationCard.Location(3, 5, 3, false, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(3, 5, 3, false, false),
					new LocationCard.Location(6, 3, 0, false, false),
					new LocationCard.Location(8, 4, 2, false, false),
					new LocationCard.Location(9, 3, 1, false, false)
				}
			),
			new LocationCard(++id, "Inner Planets Transfer", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(3, 3, 1, false, false), 
					new LocationCard.Location(7, 4, 2, false, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(3, 3, 1, false, false),
					new LocationCard.Location(7, 4, 2, false, false)
				}
			),
			new LocationCard(++id, "Mars Fly-By", 
				new List<LocationCard.Location>{ 
					new LocationCard.Location(6, 3, 0, true, true), 
					new LocationCard.Location(7, 3, -1, false, false)}, 
				new List<LocationCard.Location>{
					new LocationCard.Location(3, 3, 3, false, false)
				}
			),
		};
		public List<ComponentCard> componentCards = new List<ComponentCard> {
			new ComponentCard((id = 0), ComponentCard.ComponentType.Rocket, "Juno", 1, 1, 4),
			new ComponentCard(++id, ComponentCard.ComponentType.Rocket, "Atlas", 4, 5, 27),
			new ComponentCard(++id, ComponentCard.ComponentType.Rocket, "Soyus", 9, 8, 80),
			new ComponentCard(++id, ComponentCard.ComponentType.Rocket, "Saturn", 20, 15, 200),			
			new ComponentCard(++id, ComponentCard.ComponentType.Capsule, "Vostok", 2, 2, 1),
			new ComponentCard(++id, ComponentCard.ComponentType.Capsule, "Eagle", 1, 4, 2),
			new ComponentCard(++id, ComponentCard.ComponentType.Capsule, "Apollo", 3, 4, 3),
			new ComponentCard(++id, ComponentCard.ComponentType.Capsule, "Aldrin", 3, 4, 8),
			new ComponentCard(++id, ComponentCard.ComponentType.Probe, "Probe", 1, 2, 0),
			new ComponentCard(++id, ComponentCard.ComponentType.Astronaut, "Astronaut", 0, 5, 0)
		};
		public List<TechnologyCard> technologyCards = new List<TechnologyCard> {
			new TechnologyCard((id = 0), "Juno Rockets", 10, 3, "Rocket discarded, provides thrust", "Rocket damaged, provides no thrust", "Explosion destroys spacecraft"),
			new TechnologyCard(++id, "Atlas Rockets", 10, 3, "Rocket discarded, provides thrust", "Rocket damaged, provides no thrust", "Explosion destroys spacecraft"),
			new TechnologyCard(++id, "Soyuz Rockets", 10, 3, "Rocket discarded, provides thrust", "Rocket damaged, provides no thrust", "Explosion destroys spacecraft"),
			new TechnologyCard(++id, "Saturn Rockets", 10, 3, "Rocket discarded, provides thrust", "Rocket damaged, provides no thrust", "Explosion destroys spacecraft"),
			new TechnologyCard(++id, "Life Supoort", 10, 3, "Occupants survive", "Occupants die", "Occupants die"),
			new TechnologyCard(++id, "Re-Entry", 10, 3, "Atmosphere re-entry successful", "Capsule is damaged, occupants survive", "Capsule is destroyed, occupants die"),
			new TechnologyCard(++id, "Landing", 10, 3, "Landing successful", "Rough landing, damage chosen component", "Impact with surface, spacecraft is destroyed"),
			new TechnologyCard(++id, "Life Supoort", 10, 3, "Occupants survive", "Occupants die", "Occupants die"),
	//		new TechnologyCard(++id, "Surveying", 10, 1, "Look at other side of location card", "Surveying fails", "Surveying fails")
		};
		public List<MissionCard> missionCards = new List<MissionCard>{
			new MissionCard((id = 0), "Sounding Rocket", 1, "Working probe/capsule in space", "Easy"),
			new MissionCard(++id, "Man in Space", 2, "Man to space and back", "Easy"),
			new MissionCard(++id, "Artifical Satellite", 2, "Working probe/capsule in Earth orbit", "Easy"),
			new MissionCard(++id, "Lunar Survey", 4, "Reveal Moon location", "Easy"),
			new MissionCard(++id, "Man in Orbit", 4, "Man to earth orbit and back", "Easy"),
			new MissionCard(++id, "Mars Survey", 5, "Reveal Mars location", "Easy"),
			new MissionCard(++id, "Lunar Lander", 6, "Working probe/capsule on the moon", "Medium"),
			new MissionCard(++id, "Space Station", 6, "Man in space at start of year", "Medium"),
			new MissionCard(++id, "Mars Lander", 7, "Working probe/capsule on mars", "Medium"),
			new MissionCard(++id, "Man on the Moon", 12, "Man on the Moon and back", "Medium"),
			new MissionCard(++id, "Lunar Station", 15, "Man on the Moon at start of year", "Hard"),
			new MissionCard(++id, "Mars Station", 20, "Man on Mars at start of year", "Hard"),
			new MissionCard(++id, "Man on Mars", 24, "Man on Mars and back", "Hard")
		};

		public class LocationCard {
			public int id;
			public string name;

			public struct Location {
				public int id;
				public int difficulty;
				public int duration;//-1 for duration indicates optional time counters (can place as many as they want)
				public bool reentry, landing;

				public Location(int id, int difficulty, int duration, bool landing = false, bool reentry = false) {
					this.id = id;
					this.difficulty = difficulty;
					this.duration = duration;
					this.reentry = reentry;
					this.landing = landing;
				}
			}

			public List<Location> fromLocations = new List<Location>();
			public List<Location> toLocations = new List<Location>();

			public LocationCard(int id, string name, List<Location> tolocations, List<Location> fromlocations) {
				this.id = id;
				this.name = name;
				this.fromLocations = fromlocations;
				this.toLocations = tolocations;
			}
		}

		public class ComponentCard {
			public string name;
			public int id, weight, cost;

			public enum ComponentType { Rocket, Capsule, Probe, Astronaut }
			public ComponentType type;

			public int val;//val is thrust if type is rocket, or capacity if type is capsule, otherwise unused

			public ComponentCard(int id, ComponentType type, string name, int weight, int cost, int val) {
				this.id = id;
				this.type = type;
				this.name = name;
				this.weight = weight;
				this.cost = cost;
				this.val = val;
			}

			//public bool hasHeatShield() {
			//	return name == "Vostok" || name == "Apollo";
			//}
		}

		public class TechnologyCard {
			public int id, cost, numOutcomes;
			public string name, outcomeSuccess, outcomeMinor, outcomeMajor;
			public int removeOutcomeSuccessCost = 10, removeOutcomeMinorCost = 5, removeOutcomeMajorCost = 5;

			public TechnologyCard(int id, string name, int cost, int numoutcomes, string success, string minor, string major) {
				this.id = id;
				this.name = name;
				this.cost = cost;
				this.numOutcomes = numoutcomes;
				this.outcomeSuccess = success;
				this.outcomeMinor = minor;
				this.outcomeMajor = major;
			}
		}

		public class MissionCard {
			public int id, points;
			public string name, description, difficulty;

			public MissionCard(int id, string name, int points, string description, string difficulty) {
				this.id = id;
				this.name = name;
				this.points = points;
				this.description = description;
				this.difficulty = difficulty;
			}
		}










	}
}
