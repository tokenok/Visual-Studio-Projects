#pragma once

//#include "Visualizer.h"

#include "trace.h"

template<class T, class C>
inline void bubblesort(T _First, T _Last, C compare) {
	T _Start = _First + 1;
	T _End = _Last;

	while (_Start < _End) {
		T _LastSwap = _Start;
		for (T _Pos = _Start; _Pos < _End; ++_Pos) {
			T _Prev = _Pos - 1;
			if (compare(*_Pos, *_Prev)) {
				iter_swap(_Prev, _Pos);
				_LastSwap = _Pos;
			}
		}
		_End = _LastSwap;

		if (_End == _Start) break;
	}
}

template<class T>
inline void bubblesort(T _First, T _Last) {
	bubblesort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void cocktailshakersort(T _First, T _Last, C compare) {
	T _Start = _First + 1;
	T _End = _Last;

	while (_Start < _End) {
		T _Left = _End;
		T _Right = _Start;
		for (T _Pos = _Start; _Pos < _End; ++_Pos) {
			T _Prev = _Pos - 1;
			if (compare(*_Pos, *_Prev)) {
				iter_swap(_Prev, _Pos);
				_Right = _Pos;
			}
		}
		_End = _Right;

		for (T _Pos = _End; _Pos >= _Start; --_Pos) {
			T _Prev = _Pos - 1;
			if (compare(*_Pos, *_Prev)) {
				iter_swap(_Prev, _Pos);
				_Left = _Pos;
			}
		}
		_Start = _Left;
	}
}

template<class T>
inline void cocktailshakersort(T _First, T _Last) {
	cocktailshakersort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void combsort(T _First, T _Last, C compare) {
	auto _Gap = std::distance(_First, _Last);
	auto _Done = false;
	while (!_Done) {
		_Gap *= 10;
		_Gap /= 13;
		if (_Gap <= 1) {
			_Gap = 1;
			_Done = true;
		//	cocktailshakersort(_First, _Last, compare);
		//	break;
		}
		for (T _Pos = _First; _Last - _Pos > _Gap; ++_Pos) {
			T _Next = _Pos + _Gap;
			if (compare(*_Next, *_Pos)) {
				iter_swap(_Next, _Pos);
				_Done = false;
			}
		}
	}
}

template<class T>
inline void combsort(T _First, T _Last) {
	combsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void evenoddsort(T _First, T _Last, C compare) {
	T _End = _Last - 1;
	auto sorted = false;
	while (!sorted) {
		sorted = true;
		for (T _Pos = _First + 1; _Pos < _End; _Pos += 2) {
			T _Next = _Pos + 1;
			if (compare(*_Next, *_Pos)) {
				iter_swap(_Pos, _Next);
				sorted = false;
			}
		}
		for (T _Pos = _First; _Pos < _End; _Pos += 2) {
			T _Next = _Pos + 1;
			if (compare(*_Next, *_Pos)) {
				iter_swap(_Pos, _Next);
				sorted = false;
			}
		}
	}
}

template<class T>
inline void evenoddsort(T _First, T _Last) {
	evenoddsort(_First, _Last, std::less<>());
}








