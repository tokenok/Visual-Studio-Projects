#pragma once

constexpr size_t pow2(size_t e) {
	return (e == 0) ? 1 : 2 * pow2(e - 1);
}

template<class T, class O>
inline void counting_sort(T _First, T _Last, O _Res) {
	constexpr size_t num_c = (size_t)pow2(sizeof(T::value_type) * 8) - 1;
	std::vector<size_t> counts(num_c, 0);

	for (T it = _First; it != _Last; ++it)
		++counts[*it];

	size_t total = 0;
	for (size_t & count : counts) {
		size_t old_count = count;
		count = total;
		total += old_count;
	}

	for (; _First != _Last; ++_First)
		_Res[counts[*_First]++] = *_First;
}

template<class T, class C>
inline void countingsort(T _First, T _Last, C compare) {
	std::vector<T::value_type> _Res(std::distance(_First, _Last));
	counting_sort(_First, _Last, _Res.begin());
	std::copy(_Res.begin(), _Res.end(), _First);
}

template<class T>
inline void countingsort(T _First, T _Last) {
	countingsort(_First, _Last, NULL);
}



















