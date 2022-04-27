#pragma once

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
























