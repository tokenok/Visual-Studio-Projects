#pragma once

#include <list>

template<typename T, typename C>
inline void insertionsort(T _First, T _Last, C compare) {
	if (_First == _Last) return;
	for (T _Pos = std::next(_First); _Pos != _Last; ++_Pos) {
		T _Hole = _Pos;
		T _Prev = std::prev(_Pos);

		if (compare(*_Hole, *_Prev)) {
			typename T::value_type _Key = *_Pos;
			do {
				*_Hole = *_Prev;
			} while (--_Hole != _First && compare(_Key, *--_Prev));
			*_Hole = _Key;
		}
	}
}

template<class T>
inline void insertionsort(T _First, T _Last) {
	insertionsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void binaryinsertionsort(T _First, T _Last, C compare) {
	for (T _Pos = _First + 1; _Pos != _Last; ++_Pos) {
		T _Lo = _First, _Hi = _Pos;
		T _Mid = _First + ((_Pos - _First) >> 1);		
		
		do {
			//not stable and has more comparisons, but doesn't swap value in place
			/*if (compare(*_Pos, *_Mid))
				_Hi = _Mid;
			else if (compare(*_Mid, *_Pos))
				_Lo = _Mid + 1;
			else
				break;
			_Mid = _Lo + ((_Hi - _Lo) >> 1);*/

			//stable and less comparisons, but swaps value already in place
			if (compare(*_Pos, *_Mid))
				 _Hi = _Mid;
			else 
				_Lo = _Mid + 1;
			_Mid = _Lo + ((_Hi - _Lo) >> 1);

		} while (_Lo < _Hi);

		if (_Mid < _Pos) {
			typename T::value_type key = *_Pos;
			for (int _TPos = std::distance(_Mid, _Pos - 1); _TPos >= 0; --_TPos)
				*(_Mid + _TPos + 1) = *(_Mid + _TPos);
			*_Mid = key;
		}
	}
}

template<class T>
inline void binaryinsertionsort(T _First, T _Last) {
	binaryinsertionsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void shellsort(T _First, T _Last, C compare) {
	//ceil(.2 * (9 * pow(2.25, i - 1) - 4));//inverse = (ln((5x + 4) / 9) / ln(2.25)) + 1
	static std::list<int> gaps = { 1750, 701, 301, 132, 57, 23, 10, 4, 1 };

	int len = std::distance(_First, _Last);

	while (gaps.front() < len) gaps.push_front((int)((double)gaps.front() * 2.25));

	for (auto gap : gaps) {
		if (gap > len) continue;
		for (T _Pos = _First + gap; _Pos != _Last; ++_Pos) {
			for (T _Prev = _Pos; _Prev - _First >= gap && compare(*_Prev, *(_Prev - gap)); _Prev -= gap) {
				T _PrevPos = _Prev - gap;
				iter_swap(_Prev, _PrevPos);
			}
		}
	}
}

template<class T>
inline void shellsort(T _First, T _Last) {
	shellsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void stdinsertionsort(T _First, T _Last, C compare) {
	std::_Insertion_sort_unchecked(_First, _Last, compare);
}

template<class T>
inline void stdinsertionsort(T _First, T _Last) {
	std::_Insertion_sort_unchecked(_First, _Last);
}


























