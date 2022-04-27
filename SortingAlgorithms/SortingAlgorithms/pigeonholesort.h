#pragma once

//template<class T, class C>
//void pigeonholesort(T _First, T _Last, C compare) {
//	std::pair<T, T> minmax = std::minmax_element(_First, _Last, compare);
//	int min = *minmax.first, max = *minmax.second;
//	const int size = max - min + 1;
//	std::vector<int> holes(size, 0);
//
//	for (T _Pos = _First; _Pos < _Last; ++_Pos)
//		holes[*_Pos - min] += 1;
//
//	int count = 0;
//	for (T _Pos = _First; count < size; ++count) {
//		while (0 < holes[count]) {
//			*_Pos++ = count + min;
//			--holes[count];
//		}
//	}
//}

template<class T, class C>
void pigeonholesort(T _First, T _Last, C compare) {
	std::pair<T, T> minmax = std::minmax_element(_First, _Last, compare);
	typename T::value_type min = *minmax.first, max = *minmax.second;
	const int size = max - min + 1;
	typename T::value_type zero = *_First - *_First;
	std::vector<std::deque<typename T::value_type>> holes(size);

	for (T _Pos = _First; _Pos < _Last; ++_Pos) {
		typename T::value_type one = (*_Pos - *_Pos + 1);
		holes[*_Pos - min].push_back(*_Pos);
	}

	int count = 0;
	for (T _Pos = _First; count < size; ++count) {
		while (holes[count].size()) {
			*_Pos++ = holes[count].front();
			holes[count].pop_front();
		}
	}
}

template<class T>
void pigeonholesort(T _First, T _Last) {
	pigeonholesort(_First, _Last, std::less<>());
}




















