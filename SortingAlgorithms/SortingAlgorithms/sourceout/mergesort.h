#pragma once

#include "insertionsort.h"

namespace merge_sort {
template<class T, class C>
inline void merge(T _First, T _Mid, T _Last, C compare) {
	std::vector<T::value_type> _LeftCpy(_First, _Mid);
	std::vector<T::value_type> _RightCpy(_Mid, _Last);

	auto _LeftCpyP = _LeftCpy.begin();
	auto _RightCpyP = _RightCpy.begin();

	while (_LeftCpyP != _LeftCpy.end() && _RightCpyP != _RightCpy.end())
		*_First++ = compare(*_RightCpyP, *_LeftCpyP) ? *_RightCpyP++ : *_LeftCpyP++;

	std::copy(_LeftCpyP, _LeftCpy.end(), _First);
	std::copy(_RightCpyP, _RightCpy.end(), _First);
}
}

template<class T, class C>
inline void mergesort(T _First, T _Last, C compare) {
	auto mid = std::distance(_First, _Last) >> 1;
	if (!mid) return;

	T _Mid = _First + mid;
	mergesort(_First, _Mid, compare);
	mergesort(_Mid, _Last, compare);
	merge_sort::merge(_First, _Mid, _Last, compare);
}

template<class T>
inline void mergesort(T _First, T _Last) {
	mergesort(_First, _Last, std::less<>());
}


template<class T, class V, class C>
inline void allocmerge(T _First, T _Mid, T _Last, V _DestFirst, V _DestMid, V _DestLast, C compare) {
	std::copy(_First, _Mid, _DestFirst);
	std::copy(_Mid, _Last, _DestMid);

	V _LeftCpyP = _DestFirst;
	V _RightCpyP = _DestMid;

	while (_LeftCpyP != _DestMid && _RightCpyP != _DestLast)
		*_First++ = compare(*_RightCpyP, *_LeftCpyP) ? *_RightCpyP++ : *_LeftCpyP++;

	std::copy(_LeftCpyP, _DestMid, _First);
	std::copy(_RightCpyP, _DestLast, _First);
}

template<class T, class V, class C>
inline void allocmerge2(T _First, T _Mid, T _Last, V _DestFirst, V _DestMid, V _DestLast, C compare) {
	T _Left = _First;
	T _Right = _Mid;

	while (_Left != _Mid && _Right != _Last)
		*_DestFirst++ = compare(*_Right, *_Left) ? *_Right++ : *_Left++;

	std::copy(_Left, _Mid, _DestFirst);
	std::copy(_Right, _Last, _DestFirst);

	std::copy(_DestFirst, _DestMid, _First);
	std::copy(_DestMid, _DestLast, _Mid);
}

template<class T, class V, class C>
inline void _allocmergesort(T _First, T _Last, V _DestFirst, V _DestLast, C compare) {
	auto mid = std::distance(_First, _Last) >> 1;
	if (!mid) return;

	T _Mid = _First + mid;
	V _DestMid = _DestFirst + mid;
	_allocmergesort(_First, _Mid, _DestFirst, _DestMid, compare);
	_allocmergesort(_Mid, _Last, _DestMid, _DestLast, compare);
	allocmerge(_First, _Mid, _Last, _DestFirst, _DestMid, _DestLast, compare);
}

template<class T, class C>
inline void allocmergesort(T _First, T _Last, C compare) {
	std::vector<T::value_type> _Dest(std::distance(_First, _Last));
	_allocmergesort(_First, _Last, _Dest.begin(), _Dest.end(), compare);
}

template<class T>
inline void allocmergesort(T _First, T _Last) {
	allocmergesort(_First, _Last, std::less<>());
}


template<class T, class C>
inline void weavemerge(T _First, T _Mid, T _Last, C compare) {
	std::vector<T::value_type> _LeftCpy(_First, _Mid);
	std::vector<T::value_type> _RightCpy(_Mid, _Last);

	auto _LeftCpyP = _LeftCpy.begin();
	auto _RightCpyP = _RightCpy.begin();

	bool _BWeave = compare(*_LeftCpyP, *_RightCpyP);

	while (_LeftCpyP != _LeftCpy.end() && _RightCpyP != _RightCpy.end()) {
		*_First++ = _BWeave ? *_LeftCpyP++ : *_RightCpyP++;
		_BWeave = !_BWeave;
	}

	std::copy(_LeftCpyP, _LeftCpy.end(), _First);
	std::copy(_RightCpyP, _RightCpy.end(), _First);
}

template<class T, class C>
inline void weavemergesort(T _First, T _Last, C compare) {
	auto mid = std::distance(_First, _Last) >> 1;
	if (!mid) return;

	T _Mid = _First + mid;
	weavemergesort(_First, _Mid, compare);
	weavemergesort(_Mid, _Last, compare);
	weavemerge(_First, _Mid, _Last, compare);
	insertionsort(_First, _Last, compare);
}

template<class T>
inline void weavemergesort(T _First, T _Last) {
	weavemergesort(_First, _Last, std::less<>());
}

























