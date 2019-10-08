#include "economyplusseat.h"

double EconomyPlusSeat::getPrice(int date_purchased) const {
	int fdate = getFlightDate();
	int fmiles = getFlightMiles();

	double price = base_price;

	if (date_purchased <= fdate - 30)//is 30 or more days away from flight date? 
		price = base_price;
	else if (date_purchased <= fdate - 8 && date_purchased >= fdate - 29)//is between 8 and 29 days from flight date?
		price = 1.1 * base_price;
	else if (date_purchased >= fdate - 7)//is between 0 and 7 days from flight date
		price = 1.2 * base_price;

	//calculate upcharge
	if (fmiles < 800)
		price += 75;
	else
		price += 125;
	if (date_purchased >= fdate - 29)
		price += 10;

	return price;
}