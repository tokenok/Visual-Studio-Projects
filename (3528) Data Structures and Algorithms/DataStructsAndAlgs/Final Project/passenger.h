#ifndef PASSENGER_H
#define PASSENGER_H

#include "flight.h"

#include <iostream>
#include <string>
#include <vector>

class Flight;

class Passenger {

	public:
	int get_id() const { return id; }
	void set_id(int id) { this->id = id; }

	std::string get_name() const { return name; }
	void set_name(const std::string& name) { this->name = name; }

	const std::vector<Flight*>& get_flights() const { return flights; }
	Flight* get_flight(int index) { return flights[index]; }
	void add_flight(Flight* flight) { flights.push_back(flight); }

	const std::vector<Flight*>& GetFlightCart() const { return flightCart; }
	void addToflighcart(Flight* flight) { flightCart.push_back(flight); }

	private:
	int id;
	std::string name;
	std::vector<Flight*> flights;
	std::vector<Flight*> flightCart;
};
extern std::vector<Passenger*> g_passengers;

#endif