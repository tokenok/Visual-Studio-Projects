#pragma once

template <class T, class C>
void gnomesort(T _First, T _Last, C compare) {
	T _Pos = _First;

	while (_Pos != _Last) {
		if (_Pos == _First || !compare(*_Pos, *(_Pos - 1))) {
			++_Pos;
		}
		else {
			iter_swap(_Pos, (_Pos - 1));
			--_Pos;
		}
	}
}

template <class T>
inline void gnomesort(T _First, T _Last) {
	gnomesort(_First, _Last, std::less<>());
}

template <class T, class C>
inline void optimizedgnomesort(T _First, T _Last, C compare) {
	auto _Pos = _First + 1;
	auto _JmpPos = _First + 2;

	while (_Pos < _Last) {
		if (!(compare(*_Pos, *(_Pos - 1)))) {
			_Pos = _JmpPos;
			if (_JmpPos != _Last)
				++_JmpPos;
		}
		else {
			iter_swap(_Pos - 1, _Pos);
			--_Pos;
			if (_Pos == _First) {
				_Pos = _JmpPos;
				if (_JmpPos != _Last)
					++_JmpPos;
			}
		}
	}
}

template <class T>
inline void optimizedgnomesort(T _First, T _Last) {
	optimizedgnomesort(_First, _Last, std::less<>());
}
















