#pragma once

template<class T, class C>
inline void heapsort(T _First, T _Last, C compare) {
	//	using std::iter_swap;
	T _PFirst = _First + 1;
	T _Pos(_PFirst);
	T _Parent(_Pos);
	while (_PFirst < _Last) {
		while ((_Parent = _First + ((std::distance(_First, _PFirst - 1)) >> 1)) >= _First && compare(*_Parent, *_PFirst)) {
			iter_swap(_Parent, _PFirst);
			if (_Parent == _First)
				break;
			_PFirst = _Parent;
		}
		_PFirst = ++_Pos;
	}
	static int c = 0;
	while (--_Pos > _First) {
		iter_swap(_Pos, _First);
		_PFirst = _First;
		for (;;) {
			auto _ChildOff = std::distance(_First, _PFirst) << 1;
			if (++_ChildOff >= std::distance(_First, _Pos)) break;

			T _Left = _First + _ChildOff;

			T _Big = _Left;
			if (((_First + ++_ChildOff) < _Pos)) {
				T _Right = _First + _ChildOff;
				if (compare(*_Left, *_Right))
					_Big = _Right;
			}

			if (!compare(*_PFirst, *_Big)) break;

			iter_swap(_PFirst, _Big);
			_PFirst = _Big;
		}
	}
}

template<class T>
inline void heapsort(T _First, T _Last) {
	heapsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void minheapsort(T _First, T _Last, C compare) {
	//	using std::iter_swap;
	//heapify
	T _PFirst = _First + 1;
	T _Pos(_PFirst);
	T _Parent(_Pos);
	while (_PFirst < _Last) {
		while ((_Parent = _First + ((std::distance(_First, _PFirst - 1)) >> 1)) >= _First && compare(*_PFirst, *_Parent)) {
			iter_swap(_Parent, _PFirst);
			if (_Parent == _First)
				break;
			_PFirst = _Parent;
		}
		_PFirst = ++_Pos;
	}
	
	//deheapify
	while (--_Pos > _First) {
		iter_swap(_Pos, _First);
		_PFirst = _First;
		for (;;) {
			auto _ChildOff = std::distance(_First, _PFirst) << 1;
			if (++_ChildOff >= std::distance(_First, _Pos)) break;

			T _Left = _First + _ChildOff;

			T _Small = _Left;
			if (((_First + ++_ChildOff) < _Pos)) {
				T _Right = _First + _ChildOff;
				if (compare(*_Right, *_Left))
					_Small = _Right;
			}

			if (!compare(*_Small, *_PFirst)) break;

			iter_swap(_PFirst, _Small);
			_PFirst = _Small;
		}
	}
	std::reverse(_First, _Last);
}

template<class T>
inline void minheapsort(T _First, T _Last) {
	minheapsort(_First, _Last, std::less<>());
}

template<class T, class C>
inline void stdheapsort(T _First, T _Last, C compare) {
	std::make_heap(_First, _Last, compare);
	std::sort_heap(_First, _Last, compare);
}

template<class T>
inline void stdheapsort(T _First, T _Last) {
	std::make_heap(_First, _Last, std::less<>());
	std::sort_heap(_First, _Last, std::less<>());
}


























