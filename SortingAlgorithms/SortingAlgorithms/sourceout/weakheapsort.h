#pragma once

#include <vector>

#define GETFLAG(r, x) ((r[(x) >> 3] >> ((x) & 7)) & 1)
#define TOGGLEFLAG(r, x) (r[(x) >> 3] ^= 1 << ((x) & 7))

template<class T, class C>
inline void weakheapmerge(T _First, unsigned char *r, int i, int j, C compare) {
	if (compare(*(_First + i), *(_First + j))) {
		TOGGLEFLAG(r, j);
		iter_swap((_First + i), (_First + j));
	}
}

template<class T, class C>
void weakheapsort_(T _First, unsigned n, C compare) {
	using std::swap;
	if (n > 1) {
		unsigned i;
		int j, x, y, Gparent;
		int s = (n + 7) / 8;
		unsigned char * r = new unsigned char[s];
		for (i = 0; i < n / 8; ++i)
			r[i] = 0;
		for (i = n - 1; i > 0; --i) {
			j = i;
			while ((j & 1) == GETFLAG(r, j >> 1))
				j >>= 1;
			Gparent = j >> 1;
			weakheapmerge(_First, r, Gparent, i, compare);
		}
		for (i = n - 1; i >= 2; --i) {
			iter_swap(_First, (_First + i));
			x = 1;
			while ((unsigned)(y = 2 * x + GETFLAG(r, x)) < i)
				x = y;
			while (x > 0) {
				weakheapmerge(_First, r, 0, x, compare);
				x >>= 1;
			}
		}
		iter_swap(_First, (_First + 1));
		delete[] r;
	}
}

#undef GETFLAG
#undef TOGGLEFLAG

template<class T, class C>
inline void weakheapsort(T _First, T _Last, C compare) {
	weakheapsort_(_First, std::distance(_First, _Last), compare);
}

template<class T>
inline void weakheapsort(T _First, T _Last) {
	weakheapsort_(_First, std::distance(_First, _Last), std::less<>());
}


























