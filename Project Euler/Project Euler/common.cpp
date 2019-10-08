#include "common.h"

#include <math.h>

bool is_prime(ull num) {
	if (num < 2) return false;
	if (num == 2) return true;
	if (num % 2 == 0) return false;
	int max = sqrt((double)num);
	for (int i = 3; i <= max; i += 2) 
		if (num % i == 0)
			return false;
	return true;
}