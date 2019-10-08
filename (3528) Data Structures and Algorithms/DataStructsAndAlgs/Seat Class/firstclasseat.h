#ifndef FIRSTCLASSSEAT_H
#define FIRSTCLASSSEAT_H

#include "seat.h"

class FirstClassSeat: public Seat {
	public:
	double getPrice(int date_purchased) const;
};

#endif

