#ifndef ECONOMYPLUSSEAT_H
#define ECONOMYPLUSSEAT_H

#include "seat.h"

class EconomyPlusSeat: public Seat {
	public:
	double getPrice(int date_purchased) const;
};

#endif

