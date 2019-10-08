#include "economyseat.h"

using namespace std;

double EconomySeat::getPrice(int date_purchased) const {
	int fdate = getFlightDate();
	
	double price = base_price;

	if (date_purchased <= fdate - 30)//is 30 or more days away from flight date? 
		price = base_price;
	else if (date_purchased <= fdate - 8 && date_purchased >= fdate - 29)//is between 8 and 29 days from flight date?
		price = 1.1 * base_price;
	else if (date_purchased >= fdate - 7)//is between 0 and 7 days from flight date
		price = 1.2 * base_price;

	return price;
}

