#pragma once

#include <queue>

template<class T, class C>
void radixsort(T _First, T _Last, C compare, const int BASE) {
	std::vector<std::queue<T::value_type>> bucket(BASE);

	typename std::iterator_traits<T>::value_type _Max = *_First;
	for (T _Pos = _First; _Pos != _Last; ++_Pos)
		if (compare(_Max, *_Pos))
			_Max = *_Pos;


	for (unsigned power = 1; _Max != 0; _Max /= BASE, power *= BASE) {
		for (T _Pos = _First; _Pos != _Last; ++_Pos) {
			unsigned bucketNumber = (unsigned)((*_Pos / power) % BASE);
			bucket[bucketNumber].push(*_Pos);
		}

		auto _Pos = _First;
		for (int i = 0; i < BASE; i++) {
			for (int j = 0; !bucket[i].empty(); j++, ++_Pos) {
				*_Pos = bucket[i].front();
				bucket[i].pop();
			}
		}
	}
}

template<class T, class C>
void radixsort_LSD_10(T _First, T _Last, C compare) {
	radixsort(_First, _Last, compare, 10);
}

template<class T>
void radixsort_LSD_10(T _First, T _Last) {
	radixsort_LSD_10(_First, _Last, std::less<>(), 10);
}

template<class T, class C>
void radixsort_LSD_2(T _First, T _Last, C compare) {
	radixsort(_First, _Last, compare, 2);
}

template<class T>
void radixsort_LSD_2(T _First, T _Last) {
	radixsort_LSD_2(_First, _Last, std::less<>(), 2);
}

template<class T, class C>
void radixsort_LSD_4(T _First, T _Last, C compare) {
	radixsort(_First, _Last, compare, 4);
}

template<class T>
void radixsort_LSD_4(T _First, T _Last) {
	radixsort_LSD_4(_First, _Last, std::less<>(), 4);
}

template<class T, class C>
void radixsort_LSD_16(T _First, T _Last, C compare) {
	radixsort(_First, _Last, compare, 16);
}

template<class T>
void radixsort_LSD_16(T _First, T _Last) {
	radixsort_LSD_16(_First, _Last, std::less<>(), 16);
}

template<class T, class C>
void radixsort_LSD_32(T _First, T _Last, C compare) {
	radixsort(_First, _Last, compare, 32);
}

template<class T>
void radixsort_LSD_32(T _First, T _Last) {
	radixsort_LSD_32(_First, _Last, std::less<>(), 32);
}

template<class T>
inline int getMaxLog(T _First, T _Last, unsigned base) {
	int max = 0;
	auto _Pos = _First;
	double logbase = log(base);
	for (; _Pos != _Last; ++_Pos) 
		max = std::max((int)(log((unsigned)*_Pos) / logbase), max);
	return max;
}

template<class T>
inline int getDigit(T v, unsigned power, unsigned radix) {
	return (int)(v / pow(radix, power)) % radix;
}

template<class T>
inline void multiSwap(T _First, T _Last, unsigned pos, unsigned to) {	
	for (unsigned i = pos; i < to; i++) {
		iter_swap((_First + i), (_First + i + 1));
	}	
}

template<class T>
inline void inPlaceRadixLSDSort(T _First, T _Last, unsigned radix) {
	auto length = std::distance(_First, _Last);
	std::vector<typename T::value_type> vregs(radix - 1);

	unsigned maxpower = getMaxLog(_First, _Last, radix);

	int pos = 0;
	for (unsigned p = 0; p <= maxpower; p++) {
		for (unsigned i = 0; i < vregs.size(); i++) 
			vregs[i] = length - 1;

		pos = 0;
		for (int i = 0; i < length; i++) {
			int digit = getDigit(_First[pos], p, radix);

			if (digit == 0) {
				pos++;
			}
			else {
				multiSwap(_First, _Last, pos, vregs[digit - 1]);

				for (int j = digit - 1; j > 0; j--) {
					--vregs[j - 1];
				}
			}
		}
	}
}

template<class T, class C>
inline void inPlaceRadixLSDSortBase10(T _First, T _Last, C compare) {
	inPlaceRadixLSDSort(_First, _Last, 10);
}

template<class T, class C>
inline void inPlaceRadixLSDSortBase2(T _First, T _Last, C compare) {
	inPlaceRadixLSDSort(_First, _Last, 2);
}

template<class T, class C>
inline void inPlaceRadixLSDSortBase4(T _First, T _Last, C compare) {
	inPlaceRadixLSDSort(_First, _Last, 4);
}

template<class T, class C>
inline void inPlaceRadixLSDSortBase16(T _First, T _Last, C compare) {
	inPlaceRadixLSDSort(_First, _Last, 16);
}

//TODO implement https://cs.stackexchange.com/questions/93563/fast-stable-almost-in-place-radix-and-merge-sorts




