/*
* The MIT License (MIT)
*
* Copyright (c) 2018 Morwenn
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
#ifndef CPPSORT_DETAIL_INTROSELECT_H_
#define CPPSORT_DETAIL_INTROSELECT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iterator>
#include <utility>

namespace {

namespace detail {
namespace isd {
template<typename Iterator, typename Compare>
auto iter_sort3(Iterator a, Iterator b, Iterator c,
				Compare compare)
	-> Iterator {
	iter_swap_if(b, c, compare);
	iter_swap_if(a, c, compare);
	iter_swap_if(a, b, std::move(compare));
	return b; // Return median of 3
}
}

template<typename Predicate, typename ForwardIterator>
auto partition_impl(ForwardIterator first, ForwardIterator last, Predicate pred,
					std::forward_iterator_tag)
	-> ForwardIterator {
	while (true) {
		if (first == last)
			return first;
		if (!pred(*first))
			break;
		++first;
	}
	for (ForwardIterator p = first; ++p != last;) {
		if (pred(*p)) {
			iter_swap(first, p);
			++first;
		}
	}
	return first;
}

template<typename Predicate, typename BidirectionalIterator>
auto partition_impl(BidirectionalIterator first, BidirectionalIterator last, Predicate pred,
					std::bidirectional_iterator_tag)
	-> BidirectionalIterator {
	while (true) {
		while (true) {
			if (first == last)
				return first;
			if (!pred(*first))
				break;
			++first;
		}
		do {
			if (first == --last)
				return first;
		} while (!pred(*last));
		iter_swap(first, last);
		++first;
	}
}

template<typename ForwardIterator, typename Predicate>
auto partition(ForwardIterator first, ForwardIterator last, Predicate pred)
-> ForwardIterator {
	return partition_impl<std::add_lvalue_reference_t<Predicate>>(
		std::move(first), std::move(last), pred,
		typename std::iterator_traits<ForwardIterator>::iterator_category{}
	);
}
}

// Returns floor(log2(n)), assumes n > 0.
template<class T>
inline int fastlog2(T n) {
	int log = 0;
	while (n >>= 1) ++log;
	return log;
}

template<typename Iterator, typename Compare>
auto iter_swap_if(Iterator lhs, Iterator rhs, Compare compare) -> void {
	if (compare(*rhs, *lhs)) {
		iter_swap(lhs, rhs);
	}
}

template<typename BidirectionalIterator, typename Compare>
auto small_sort(BidirectionalIterator first, BidirectionalIterator last,
				typename std::iterator_traits<BidirectionalIterator>::difference_type,
				Compare compare)
	-> void {
	insertionsort(std::move(first), std::move(last),
				   std::move(compare));
}

template<typename ForwardIterator, typename Compare>
auto iter_median_5(ForwardIterator it1, ForwardIterator it2, ForwardIterator it3,
				   ForwardIterator it4, ForwardIterator it5,
				   Compare compare)
	-> ForwardIterator {
	// Median of 5, adapted from https://stackoverflow.com/a/481398/

	iter_swap_if(it1, it2, compare);
	iter_swap_if(it3, it4, compare);

	if (compare(*it1, *it3)) {
		iter_swap(it1, it5);
		iter_swap_if(it1, it2, compare);
	}
	else {
		iter_swap(it3, it5);
		iter_swap_if(it3, it4, compare);
	}

	if (compare(*it1, *it3)) {
		if (compare(*it2, *it3)) {
			return it3;
		}
		return it2;
	}
	else {
		if (compare(*it4, *it1)) {
			return it1;
		}
		return it4;
	}
}

template<typename ForwardIterator, typename Compare>
auto iter_median_rest(ForwardIterator first, typename std::iterator_traits<ForwardIterator>::difference_type size,
					  Compare compare)
	-> ForwardIterator {

	switch (size) {
		case 0:
		case 1:
		case 2:
			return first;
		case 3: {
			auto it1 = first;
			auto it2 = ++first;
			auto it3 = ++first;
			iter_swap_if(it1, it2, compare);
			iter_swap_if(it2, it3, compare);
			iter_swap_if(it1, it2, compare);
			return it2;
		}
		case 4: {
			auto it1 = first;
			auto it2 = ++first;
			auto it3 = ++first;
			auto it4 = ++first;
			iter_swap_if(it1, it2, compare);
			iter_swap_if(it3, it4, compare);
			iter_swap_if(it1, it3, compare);
			iter_swap_if(it2, it4, compare);
			iter_swap_if(it2, it3, compare);
			return it2;
		}
		case 5: {
			auto it1 = first;
			auto it2 = ++first;
			auto it3 = ++first;
			auto it4 = ++first;
			auto it5 = ++first;
			return iter_median_5(it1, it2, it3, it4, it5,
								 std::move(compare));
		}
		default:
			throw std::exception("unreachable");
	}
}

template<typename ForwardIterator, typename Compare>
auto intro_select(ForwardIterator first, ForwardIterator last,
				 typename std::iterator_traits<ForwardIterator>::difference_type nth_pos,
				 typename std::iterator_traits<ForwardIterator>::difference_type size, int bad_allowed,
				 Compare compare)
	->ForwardIterator;

template<typename ForwardIterator, typename Compare>
auto median_of_medians(ForwardIterator first, ForwardIterator last,
					   typename std::iterator_traits<ForwardIterator>::difference_type size,
					   Compare compare)
	-> ForwardIterator {

	if (size <= 5) {
		return iter_median_rest(first, size, std::move(compare));
	}

	// Iterator over the collection
	auto it = first;
	// Points to the next value to replace by a median-of-5
	auto medians_it = first;

	// We handle first the biggest part that can be rounded to a power
	// of 5, then we handle the rest
	auto rounded_size = (size / 5) * 5;

	// Handle elements 5 by 5

	for (typename std::iterator_traits<ForwardIterator>::difference_type i = 0; i < rounded_size / 5; ++i) {
		auto it1 = it;
		auto it2 = ++it;
		auto it3 = ++it;
		auto it4 = ++it;
		auto it5 = ++it;

		auto median = iter_median_5(it1, it2, it3, it4, it5, compare);
		iter_swap(medians_it, median);
		++medians_it;
		++it;
	}

	// Handle remaining elements
	if (rounded_size != size) {
		auto last_median = iter_median_rest(it, size - rounded_size, compare);
		iter_swap(last_median, medians_it);
		++medians_it;
	}

	// Rest variables for the next iteration
	last = medians_it;
	size = rounded_size == size ? size / 5 : size / 5 + 1;

	// Mutual recursion with intro_select
	return intro_select(first, last, size / 2, size, ::fastlog2(size),
					   std::move(compare));
}

////////////////////////////////////////////////////////////
// Get iterator to last element

template<typename Iterator>
auto last_it(Iterator first, Iterator, typename std::iterator_traits<Iterator>::difference_type size,
			 std::forward_iterator_tag)
	-> Iterator {
	return std::next(first, size - 1);
}

template<typename Iterator>
auto last_it(Iterator, Iterator last, typename std::iterator_traits<Iterator>::difference_type,
			 std::bidirectional_iterator_tag)
	-> Iterator {
	return std::prev(last);
}

template<typename Iterator>
auto last_it(Iterator first, Iterator last, typename std::iterator_traits<Iterator>::difference_type size)
-> Iterator {
	using category = typename std::iterator_traits<Iterator>::iterator_category;
	return last_it(first, last, size, category{});
}

////////////////////////////////////////////////////////////
// Pick a pivot for quicksort and quickselect

template<typename ForwardIterator, typename Compare>
auto pick_pivot(ForwardIterator first, ForwardIterator last,
				typename std::iterator_traits<ForwardIterator>::difference_type size, int bad_allowed,
				Compare compare)
	-> std::pair<ForwardIterator, ForwardIterator> {
	if (bad_allowed > 0) {
		auto it1 = std::next(first, size / 8);
		auto it2 = std::next(it1, size / 8);
		auto it3 = std::next(it2, size / 8);
		auto middle = std::next(it3, size / 2 - 3 * (size / 8));
		auto it4 = std::next(middle, size / 8);
		auto it5 = std::next(it4, size / 8);
		auto it6 = std::next(it5, size / 8);
		auto last_1 = last_it(it6, last, size - size / 2 - 3 * (size / 8));

		detail::isd::iter_sort3(first, it1, it2, compare);
		detail::isd::iter_sort3(it3, middle, it4, compare);
		detail::isd::iter_sort3(it5, it6, last_1, compare);
		auto median_it = detail::isd::iter_sort3(it1, middle, it4, std::move(compare));
		return std::make_pair(median_it, last_1);
	}
	else {
		auto last_1 = last_it(first, last, size);
		auto median_it = median_of_medians(first, last, size, compare);
		return std::make_pair(median_it, last_1);
	}
}

////////////////////////////////////////////////////////////
// Forward nth_element based on intro_select

template<typename ForwardIterator, typename Compare>
auto intro_select(ForwardIterator first, ForwardIterator last,
				 typename std::iterator_traits<ForwardIterator>::difference_type nth_pos,
				 typename std::iterator_traits<ForwardIterator>::difference_type size, int bad_allowed,
				 Compare compare)
	-> ForwardIterator {

	if (size <= 32) {
		small_sort(first, last, size, std::move(compare));
		return std::next(first, nth_pos);
	}

	// Choose pivot as either median of 9 or median of medians
	auto temp = pick_pivot(first, last, size, bad_allowed, compare);
	auto median_it = temp.first;
	auto last_1 = temp.second;

	// Put the pivot at position std::prev(last) and partition
	iter_swap(median_it, last_1);
	auto middle1 = detail::partition(
		first, last_1,
		[&](const auto& elem) { return compare(elem, *last_1); }
	);

	// Put the pivot in its final position and partition
	iter_swap(middle1, last_1);
	
	auto middle2 = detail::partition(
		std::next(middle1), last,
		[&](const auto& elem) { return not compare(*middle1, elem); }
	);

	// Recursive call: heuristic trick here: in real world cases,
	// the middle partition is more likely to be smaller than the
	// right one, so computing its size should generally be cheaper
	auto size_left = std::distance(first, middle1);
	auto size_middle = std::distance(middle1, middle2);
	auto size_right = size - size_left - size_middle;

	// TODO: unroll tail recursion
	// We're done if the nth element is in the middle partition
	if (nth_pos < size_left) {
		return intro_select(first, middle1, nth_pos,
						   size_left, --bad_allowed,
						   std::move(compare));
	}
	else if (nth_pos > size_left + size_middle) {
		return intro_select(middle2, last, nth_pos - size_left - size_middle,
						   size_right, --bad_allowed,
						   std::move(compare));
	}
	// Return an iterator to the nth element
	return std::next(middle1, nth_pos - size_left);
}
}

#endif // CPPSORT_DETAIL_INTROSELECT_H_




