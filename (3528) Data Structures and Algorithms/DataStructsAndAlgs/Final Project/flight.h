#ifndef FLIGHT_H
#define FLIGHT_H

#include "passenger.h"
#include "plane.h"

#include <vector>
#include <string>

class Passenger;

class Flight {
	private:
	int flightNumber;
	std::string destination;
	std::string departureLocation;
	std::tm depTime;
	std::tm arrivalTime;
	Plane* plane;
	int miles;

	std::vector<Passenger*> passengers;

	public:
	//getters and setters
	int get_flightNumber() const { return flightNumber; }
	void set_flightNumber(int flightNumber) { this->flightNumber = flightNumber; }

	const std::string& get_destination() const { return destination; }
	void set_destination(const std::string& destination) { this->destination = destination; }

	const std::string& get_departureLocation() const { return departureLocation; }
	void set_departureLocation(const std::string& departureLocation) { this->departureLocation = departureLocation; }

	const std::tm& get_depTime() const { return depTime; }
	void set_depTime(const std::tm& depTime) { this->depTime = depTime; }

	const std::tm& get_arrivalTime() const { return arrivalTime; }
	void set_arrivalTime(const std::tm& arrivalTime) { this->arrivalTime = arrivalTime; }

	const Plane* get_plane() const { return plane; }
	void set_plane(Plane* plane) { this->plane = plane; }

	int get_miles() const { return miles; }
	void set_miles(int miles) { this->miles = miles; }

	const std::vector<Passenger*>& get_passengers() { return passengers; }
	void add_passenger(Passenger* passenger) { passengers.push_back(passenger); }
	const Passenger* get_passenger(int index) { return passengers[index]; }

};
extern std::vector<Flight*> g_flights;

#endif

