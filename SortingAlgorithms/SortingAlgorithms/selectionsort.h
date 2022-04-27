#pragma once

template<class T, class C>
inline void selectionsort(T _First, T _Last, C compare) {
	T _End = _Last - 1;
	while (_First < _End) {
		T _Pos = _First;
		T _Max = _First;
		while (_Pos <= _End) {
			if (compare(*_Max, *_Pos))
				_Max = _Pos;
			++_Pos;
		}
		iter_swap(_End, _Max);
		--_End;
	}
}

template<class T>
inline void selectionsort(T _First, T _Last) {
	selectionsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void doubleselectionsort(T _First, T _Last, C compare) {
	T _Left = _First, _Right = _Last;
	while (_Left < _Right) {
		std::pair<T, T> minmax = std::minmax_element(_Left, _Right--, compare);

		iter_swap(_Left, minmax.first);

		if (_Left == minmax.second)
			iter_swap(_Right, minmax.first);
		else
			iter_swap(_Right, minmax.second);

		++_Left;
	}
}

template<class T>
inline void doubleselectionsort(T _First, T _Last) {
	doubleselectionsort(_First, _Last, std::less<>());
}


























