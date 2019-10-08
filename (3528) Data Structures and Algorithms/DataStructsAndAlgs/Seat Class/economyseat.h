#ifndef ECONOMYSEAT_H
#define ECONOMYSEAT_H

#include "seat.h"

class EconomySeat: public Seat {
	public:
	double getPrice(int date_purchased) const;
};

#endif 

