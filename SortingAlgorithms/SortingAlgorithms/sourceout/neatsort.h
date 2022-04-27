/*
* The MIT License (MIT)
*
* Copyright (c) 2016 Morwenn
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
#ifndef CPPSORT_DETAIL_NEATSORT_H_
#define CPPSORT_DETAIL_NEATSORT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>
#include <list>

namespace {

template<class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
	auto merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp = {}) -> OutputIterator {
	for (; first1 != last1; ++result) {
		if (first2 == last2)
			return std::copy(first1, last1, result);
		if (comp(*first2, *first1)) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
	}
	return std::copy(first2, last2, result);
}

template<typename ForwardIterator, typename Compare>
auto inplace_merge(ForwardIterator first, ForwardIterator middle, ForwardIterator last, Compare compare) -> void {
	using value_type = typename std::iterator_traits<ForwardIterator>::value_type;

	// Shrink the problem size on the left side
	while (compare(*first, *middle)) {
		++first;
	}

	// Try to allocate a temporary buffer and make sure
	// it will always be properly deleted
	auto size = std::distance(first, last);
	std::unique_ptr<value_type[]> buffer(new (std::nothrow) value_type[size]);

	// If there is enough memory, use the buffer for the merge
	if (buffer != nullptr) {
		::merge(first, middle, middle, last, buffer.get(), compare);
		std::move(buffer.get(), buffer.get() + size, first);
	}
	else // Hand-made in-place merge algorithm
	{
		ForwardIterator insertion_point = middle;
		for (; first != middle; ++first) {
			if (compare(*middle, *first)) {
				// *first should be in the right partition
				value_type tmp = std::move(*first);

				// Put *right in place
				*first = std::move(*middle);

				// Look for the place where to insert tmp
				ForwardIterator next = std::next(insertion_point);

				// Move everything smaller than tmp to the left
				std::move(std::next(middle), next, middle);
				while (next != last && compare(*next, tmp)) {
					*insertion_point++ = std::move(*next++);
				}

				// Insert tmp in the right place
				*insertion_point = std::move(tmp);
			}
		}
	}
}

template<typename RandomAccessIterator, typename Compare>
auto neatsort(RandomAccessIterator first, RandomAccessIterator last, Compare compare) -> void {
	// Some heuristic constant, see the original paper
	static constexpr double p = 1.3;

	if (std::distance(first, last) < 2) return;

	// Vector to store the beginning of each run
	std::list<RandomAccessIterator> runs = {first};

	auto current = first;
	auto next = std::next(first);

	////////////////////////////////////////////////////////////
	// Create runs

	while (true) {
		// Look for ascending run
		while (next != last && not compare(*next, *current)) {
			++current;
			++next;
		}

		// If the run is too small
		if (std::distance(runs.back(), next) == 1) {
			// Look for a descending run and reverse
			while (next != last && not compare(*current, *next)) {
				++current;
				++next;
			}
			std::reverse(runs.back(), next);

			// Check whether we can make a bigger run with
			// the following elements too
			while (next != last && not compare(*next, *current)) {
				++current;
				++next;
			}

			// If the beginning of the run is greater than the
			// end of the previous run, make that a big run
			if (runs.size() > 1 && compare(*std::prev(runs.back()), *runs.back())) {
				runs.pop_back();
			}
		}

		// Store the beginning of the next run
		runs.push_back(next);
		if (next == last) break;

		++current;
		++next;
	}

	////////////////////////////////////////////////////////////
	// Merge runs

	while (runs.size() > 3) {
		auto j = std::begin(runs);
		while (j != std::end(runs)) {
			// Beginning of the runs
			auto&& one = j++;
			if (j == std::end(runs)) break;
			auto&& two = j++;
			if (j == std::end(runs)) break;
			auto&& three = j++;
			if (j == std::end(runs)) break;

			if (std::distance(*one, *two) < p * (std::distance(*two, *three) + std::distance(*three, *j))) {
				::inplace_merge(*one, *two, *three, compare);
				runs.erase(two);
				--j;
			}
			else {
				::inplace_merge(*two, *three, *j, compare);
				runs.erase(three);
			}
		}
	}

	// Merge potential remaining runs
	if (runs.size() == 3) {
		::inplace_merge(
			*std::begin(runs),
			*std::next(std::begin(runs)),
			*std::next(std::next(std::begin(runs))),
			compare
		);
	}
}
template<class T>
inline void neatsort(T _First, T _Last) {
	neatsort(_First, _Last, std::less<>());
}
}

#endif // CPPSORT_DETAIL_NEATSORT_H_


