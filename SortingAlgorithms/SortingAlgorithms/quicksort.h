#pragma once

template<class T, class C>
inline void MedianOfThree(T _First, T _Mid, T _Last, C compare) {
	using std::iter_swap;
	if (compare(*_Last, *_First))
		iter_swap(_First, _Last);
	if (compare(*_Mid, *_First))
		iter_swap(_Mid, _First);
	if (compare(*_Last, *_Mid))
		iter_swap(_Last, _Mid);
}

template<class T, class C>
inline T part(T _First, T _Last, C compare) {
	T _Mid = _First + ((_Last - _First) >> 1);
	MedianOfThree(_First, _Mid, _Last, compare);
	++_First; --_Last;
	auto p = *_Mid;

	for (;;) {
		while (compare(*_First, p)) ++_First;
		while (compare(p, *_Last)) --_Last;

		if (_First >= _Last) return _Last;

		iter_swap(_First, _Last);
		++_First; --_Last;
	}
}

template<class T, class C>
inline void _quicksort(const T _First, const T _Last, C compare) {
	if (_Last > _First) {
		T _Med = part(_First, _Last, compare);
		_quicksort(_First, _Med, compare);
		_quicksort(_Med + 1, _Last, compare);
	}
}

template<class T>
inline void quicksort(const T _First, const T _Last) {
	_quicksort(_First, _Last - 1, std::less<>());
}

template<class T, class C>
inline void quicksort(const T _First, const T _Last, C compare) {
	_quicksort(_First, _Last - 1, compare);
}


























