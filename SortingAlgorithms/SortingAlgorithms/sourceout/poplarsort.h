/*
* The MIT License (MIT)
*
* Copyright (c) 2016-2018 Morwenn
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef CPPSORT_DETAIL_POPLAR_SORT_H_
#define CPPSORT_DETAIL_POPLAR_SORT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace detail {

template<typename Unsigned>
constexpr auto hyperfloor(Unsigned n) -> Unsigned {
	constexpr auto bound = std::numeric_limits<Unsigned>::digits / 2;
	for (std::size_t i = 1; i <= bound; i <<= 1) {
		n |= (n >> i);
	}
	return n & ~(n >> 1);
}

template<typename RandomAccessIterator>
struct poplar {
	RandomAccessIterator begin, end;
	typename std::make_unsigned_t<typename RandomAccessIterator::difference_type> size;
	auto root() const -> RandomAccessIterator {
		auto res = end;
		return --res;
	}
};

template<typename RandomAccessIterator, typename Size, typename Compare>
auto sift(RandomAccessIterator first, Size size, Compare compare) -> void {
	if (size < 2) return;

	auto root = first + (size - 1);
	auto child_root1 = root - 1;
	auto child_root2 = first + (size / 2 - 1);

	while (true) {
		auto max_root = root;
		if (compare((*max_root), (*child_root1))) {
			max_root = child_root1;
		}
		if (compare((*max_root), (*child_root2))) {
			max_root = child_root2;
		}
		if (max_root == root) return;

		iter_swap(root, max_root);

		size /= 2;
		if (size < 2) return;

		root = max_root;
		child_root1 = root - 1;
		child_root2 = max_root - (size - size / 2);
	}
}

template<typename RandomAccessIterator, typename Compare>
auto relocate(const std::vector<poplar<RandomAccessIterator>>& poplars, Compare compare) -> void {
	// Find the poplar with the bigger root
	// We can assume that there is always at least one poplar
	auto last = std::prev(std::end(poplars));
	auto bigger = last;
	for (auto it = std::begin(poplars); it != last; ++it) {
		if (compare((*bigger->root()), (*it->root()))) {
			bigger = it;
		}
	}

	if (bigger != last) {
		iter_swap(bigger->root(), last->root());
		sift(bigger->begin, bigger->size, std::move(compare));
	}
}

template<typename RandomAccessIterator, typename Compare>
auto make_poplar(RandomAccessIterator first, RandomAccessIterator last, Compare compare) -> void {
	using poplar_size_t = std::make_unsigned_t<RandomAccessIterator::difference_type>;
	poplar_size_t size = std::distance(first, last);
	if (size < 16) {
		// A sorted collection is a valid poplar heap;
		// when the heap is small, using insertion sort
		// should be faster
		insertion_sort(std::move(first), std::move(last), std::move(compare));
		return;
	}

	auto middle = first + size / 2;
	make_poplar(first, middle, compare);
	make_poplar(middle, std::prev(last), compare);
	sift(std::move(first), size, std::move(compare));
}

template<typename RandomAccessIterator, typename Compare>
auto poplar_sort(RandomAccessIterator first, RandomAccessIterator last, Compare compare) -> void {
	using poplar_size_t = std::make_unsigned_t<RandomAccessIterator::difference_type>;

	// Size of the unsorted subsequence
	poplar_size_t size = std::distance(first, last);
	if (size < 2) return;

	std::vector<poplar<RandomAccessIterator>> poplars;
	poplars.reserve((unsigned)log2(size));

	//
	// Size of the biggest poplar in the array, which always is a number
	// of the form 2^n - 1
	//
	// It's worth noting that the +1 never causes problems: we're only
	// using unsigned integers, so when size is the biggest representable
	// value for its type, size + 1 == 0 thanks to the behaviour of
	// unsigned overflow; hyperfloor(0) == 0, and subtracting 1 to that
	// gives back the biggest representable value, which happens to be
	// a number of the form 2^n - 1
	//
	poplar_size_t poplar_size = hyperfloor(size + 1u) - 1u;

	// Make the poplar heap
	auto it = first;
	do {
		if (poplar_size_t(std::distance(it, last)) >= poplar_size) {
			auto begin = it;
			auto end = it + poplar_size;
			make_poplar(begin, end, compare);
			poplars.push_back({begin, end, poplar_size});
			it = end;
		}
		else {
			poplar_size = (poplar_size + 1) / 2 - 1;
		}
	} while (poplar_size > 0);

	// Sort the poplar heap
	do {
		// Find the greatest element, put it in place
		relocate(poplars, compare);
		// If the last poplar had one element, destroy it
		if (poplars.back().size == 1) {
			poplars.pop_back();
			if (poplars.size() == 0) return;
			if (poplars.size() == 1) {
				if (poplars.back().size == 1) return;
				auto& back = poplars.back();
				auto old_end = back.end;
				auto new_size = (back.size - 1) / 2;
				auto middle = back.begin + new_size;
				back.end = middle;
				back.size = new_size;
				poplars.push_back({middle, --old_end, new_size});
			}
		}
		else {
			auto& back = poplars.back();
			auto old_end = back.end;
			auto new_size = (back.size - 1) / 2;
			auto middle = back.begin + new_size;
			back.end = middle;
			back.size = new_size;
			poplars.push_back({middle, --old_end, new_size});
		}

	} while (poplars.size() > 1);
}
}

template<class T, class C>
inline void poplarsort(T _First, T _Last, C compare) {
	::detail::poplar_sort(_First, _Last, compare);
}

#endif // CPPSORT_DETAIL_POPLAR_SORT_H_





















