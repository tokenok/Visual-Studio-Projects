#pragma once

template<class T, class C>
inline void _slowsort(T _First, T _Last, C compare) {
	if (_First < _Last) {
		T _Mid = _First + ((_Last - _First) >> 1);
		_slowsort(_First, _Mid, compare);
		_slowsort(_Mid + 1, _Last, compare);
		if (compare(*_Last, *_Mid))
			iter_swap(_Mid, _Last);
		_slowsort(_First, _Last - 1, compare);
	}
}

template<class T, class C>
inline void slowsort(T _First, T _Last, C compare) {
	_slowsort(_First, _Last - 1, compare);
}

template<class T>
inline void slowsort(T _First, T _Last) {
	slowsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void _sillysort(T _First, T _Last, C compare) {
	if (_First < _Last) {
		T _Mid = _First + ((_Last - _First) >> 1);
		_sillysort(_First, _Mid, compare);
		_sillysort(_Mid + 1, _Last, compare);
		if (!compare(*_First, *(_Mid + 1)))
			iter_swap(_First, (_Mid + 1));
		_sillysort(_First + 1, _Last, compare);
	}
}

template<class T, class C>
inline void sillysort(T _First, T _Last, C compare) {
	_sillysort(_First, _Last - 1, compare);
}

template<class T>
inline void sillysort(T _First, T _Last) {
	sillysort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void _stoogesort(T _First, T _Last, C compare /*i = 0, j = length(L) - 1*/) {
	if (compare(*_Last, *_First))
		iter_swap(_First, _Last);
	if (std::distance(_First, _Last) + 1 > 2) {
		T _Temp = _First + ((_Last - _First + 1) / 3);
		_stoogesort(_First, (_Last - std::distance(_First, _Temp)), compare);
		_stoogesort(_Temp, _Last, compare);
		_stoogesort(_First, (_Last - std::distance(_First, _Temp)), compare);
	}
}

template<class T, class C>
inline void stoogesort(T _First, T _Last, C compare) {
	_stoogesort(_First, _Last - 1, compare);
}

template<class T>
inline void stoogesort(T _First, T _Last) {
	stoogesort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void bogosort(T _First, T _Last, C compare) {
	while (!sorted(_First, _Last, compare)) {
		std::random_shuffle(_First, _Last);
	}
}

template<class T>
inline void bogosort(T _First, T _Last) {
	bogosort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void lexipermsort(T _First, T _Last, C compare) {
	while (!sorted(_First, _Last, compare))
		std::next_permutation(_First, _Last, compare);
}

template<class T>
inline void lexipermsort(T _First, T _Last) {
	lexipermsort(_First, _Last, std::less<>());
}

template<class T, class J, class C>
bool bogo_equal(T first1, T last1, J first2, C compare) {
	for (; first1 != last1; ++first1, ++first2) 
		if ((compare(*first1, *first2) || compare(*first2, *first1))) 
			return false;
	return true;
}

template<class T, class C>
inline void bogobogosort(T _First, T _Last, C compare);

template<class T, class C>
inline bool bogo_sorted(T _First, T _Last, C compare) {
	if (_First == _Last || _First == _Last - 1) return true;
	std::vector<typename T::value_type> _Copy(_First, _Last);
	do {
		std::random_shuffle(_Copy.begin(), _Copy.end());
		bogobogosort(_Copy.begin(), _Copy.end() - 1, compare);
		
	} while (compare(*(_Last - 1), _Copy.back()));

	return bogo_equal(_First, _Last, _Copy.begin(), compare);
}

template<class T, class C>
inline void bogobogosort(T _First, T _Last, C compare) {
	while (!bogo_sorted(_First, _Last, compare)) {
		std::random_shuffle(_First, _Last);
	}
}

template<class T>
inline void bogobogosort(T _First, T _Last) {
	bogobogosort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void bozosort(T _First, T _Last, C compare) {
	auto size = std::distance(_First, _Last);
	while (!sorted(_First, _Last, compare))
		iter_swap(_First + (rand() % size), _First + (rand() % size));
}

template<class T>
inline void bozosort(T _First, T _Last) {
	bozosort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void pancakesort(T _First, T _Last, C compare) {	
	for (T _Pos = _Last; _Pos > _First + 1; --_Pos) {
		T _Max = std::max_element(_First, _Pos, compare);
		if (_Max + 1 != _Pos) {			
			std::reverse(_First, _Max + 1);	
			std::reverse(_First, _Pos);
		}
	}	
}

template<class T>
inline void pancakesort(T _First, T _Last) {
	pancakesort(_First, _Last, std::less<>());
}


























