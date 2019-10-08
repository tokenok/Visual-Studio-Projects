#ifndef SEAT_H
#define SEAT_H

#include <iostream>

class Seat {
	public:

	//get and set flight's date
	void setFlightDate(int date) { flight_date = date; }
	int getFlightDate() const { return flight_date; }

	//get and set flight's miles
	void setFlightMiles(int miles) { flight_miles = miles; }
	double getFlightMiles() const { return flight_miles; }

	//get base price with public access
	double getBasePrice() const { return base_price; }
	void  setBasePrice(double price) { base_price = price; }

	//calulates price of seat based on date purchased, and number of miles of flight
	//must be implemented by all child classes
	virtual double getPrice(int date_purchased) const = 0;	

	private:
	int flight_miles;
	int flight_date;
	int purchased_date;

	protected:
	static double base_price;	
};

#endif

