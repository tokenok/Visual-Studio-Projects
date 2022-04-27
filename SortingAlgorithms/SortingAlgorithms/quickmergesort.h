#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace {
// Number of elements to sort under which we perform insertion sort
static constexpr int insertion_limit = 32;

template<
	typename BidirectionalIterator,
	typename Compare = std::less<>
>
auto insertion_sort(BidirectionalIterator first, BidirectionalIterator last,
					Compare compare = {})
	-> void {
	if (first == last) return;

	for (BidirectionalIterator cur = std::next(first); cur != last; ++cur) {
		BidirectionalIterator sift = cur;
		BidirectionalIterator sift_1 = std::prev(cur);

		// Compare first so we can avoid 2 moves for
		// an element already positioned correctly.
		if (compare(*sift, *sift_1)) {
			auto tmp = std::move(*sift);
			do {
				*sift-- = std::move(*sift_1);
			} while (sift != first && compare(tmp, *--sift_1));
			*sift = std::move(tmp);
		}
	}
}

template<
	typename InputIterator1,
	typename InputIterator2,
	typename OutputIterator,
	typename Compare = std::less<>
>
auto half_inplace_merge(InputIterator1 first1, InputIterator1 last1,
						InputIterator2 first2, InputIterator2 last2,
						OutputIterator result, Compare compare = {})
	-> void {
	for (; first1 != last1; ++result) {
		if (first2 == last2) {
			std::swap_ranges(first1, last1, result);
			return;
		}

		if (compare(*first2, *first1)) {
			std::iter_swap(result, first2);
			++first2;
		}
		else {
			std::iter_swap(result, first1);
			++first1;
		}
	}
	// first2 through last2 are already in the right spot
}

template<
	typename BidirectionalIterator,
	typename Compare = std::less<>
>
auto buffered_inplace_merge(BidirectionalIterator first, BidirectionalIterator middle,
							BidirectionalIterator last, BidirectionalIterator buffer,
							Compare compare = {})
	-> void {
	auto buffer_end = std::swap_ranges(first, middle, buffer);
	half_inplace_merge(buffer, buffer_end,
					   middle, last,
					   first, compare);
}

template<
	typename BidirectionalIterator,
	typename Compare = std::less<>
>
auto internal_mergesort(BidirectionalIterator first, BidirectionalIterator last,
						BidirectionalIterator buffer, Compare compare = {})
	-> void {
	if (std::distance(first, last) <= insertion_limit) {
		::insertion_sort(first, last, compare);
		return;
	}

	auto middle = first + (last - first) / 2; // make sure left is smaller
	internal_mergesort(first, middle, buffer, compare);
	internal_mergesort(middle, last, buffer, compare);

	while (first != middle && not compare(*middle, *first)) {
		++first;
	}
	if (first == middle) return;

	buffered_inplace_merge(first, middle, last, buffer, compare);
}

template<
	typename RandomAccessIterator,
	typename Compare = std::less<>
>
auto quickmergesort(RandomAccessIterator first, RandomAccessIterator last,
					Compare compare = {})
	-> void {
	auto size = std::distance(first, last);
	while (size > insertion_limit) {
		auto pivot = first + (2 * (size / 3) - 2);
		std::nth_element(first, pivot, last, compare);
		internal_mergesort(first, pivot, pivot, compare);

		first = pivot;
		size = std::distance(first, last);
	}
	::insertion_sort(first, last, compare);
}
}




