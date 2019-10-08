#include <iostream>
#include <string>
#include <vector>

#include "seats.h"

using namespace std;

int main() {
	//creat instances of different types of seats
	FirstClassSeat firstclass;
	EconomyPlusSeat economyplus;
	EconomySeat economy;

	//use polymorphism
	Seat* a = &firstclass;
	Seat* b = &economyplus;
	Seat* c = &economy;

	//initialize seats flight info
	a->setFlightDate(140);
	b->setFlightDate(140);
	c->setFlightDate(140);
	b->setFlightMiles(2000);

	//show that polymorphism was successful//
	/////////////////////////////////////////
	cout << "base price: " << a->getBasePrice() << '\n';

	//show prices over 36 days for first class seats
	for (int i = 105; i <= 140; i++) {
		cout << "first class seat price (" 
			<< 140 - i 
			<< " days prior): $" 
			<< a->getPrice(i)
			<< '\n';
	}
	//show prices over 36 days for economy+ seats
	for (int i = 105; i <= 140; i++) {
		cout << "economy+ seat price ("
			<< 140 - i
			<< " days prior): $"
			<< b->getPrice(i)
			<< '\n';
	}
	//show prices over 36 days for economy seats
	for (int i = 105; i <= 140; i++) {
		cout << "economy seat price ("
			<< 140 - i
			<< " days prior): $"
			<< c->getPrice(i)
			<< '\n';
	}

	a = b;//because why not
	a->setFlightMiles(600);
	cout << "price of economy+ seat (600 mile flight, 30 days before flight): $"
		<< a->getPrice(110) 
		<< '\n';

	a->setFlightMiles(1000);
	int* d = (int*)a;//cast Seat* to int*
	cout << "price of economy+ seat (1000 mile flight, 30 days before flight): $" 
		<< ((Seat*)d)->getPrice(110)
		<< '\n';

	Seat* e = new EconomySeat;
	e->setFlightDate(40);
	e->setFlightMiles(99);
	e->setBasePrice(100);
	cout << "new base price: $" << e->getBasePrice() << '\n';
	cout << "testing new operator for EconomySeat as Seat* (5 days prior): $" << e->getPrice(35) << '\n';
	delete e;

	cin.get();
	return 0;
}

