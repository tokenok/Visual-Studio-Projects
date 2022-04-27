#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <utility>
#include <vector>

template<
	typename InputIterator1,
	typename InputIterator2,
	typename OutputIterator,
	typename Compare = std::less<>
>
auto merge_move(InputIterator1 first1, InputIterator1 last1,
				InputIterator2 first2, InputIterator2 last2,
				OutputIterator result, Compare compare = {})
	-> OutputIterator {
	for (; first1 != last1; ++result) {
		if (first2 == last2) {
			return std::move(first1, last1, result);
		}

		if (compare(*first2, *first1)) {
			*result = std::move(*first2);
			++first2;
		}
		else {
			*result = std::move(*first1);
			++first1;
		}
	}
	return std::move(first2, last2, result);
}

template<
	typename ForwardIterator,
	typename Compare = std::less<>
>
auto melsort(ForwardIterator first, ForwardIterator last, Compare compare = {})
-> void {
	using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
	std::vector<std::list<value_type>> lists;

	////////////////////////////////////////////////////////////
	// Create encroaching lists

	for (auto it = first; it != last; ++it) {
		bool found = false;
		for (auto& list : lists) {
			if (not compare(list.front(), *it)) {
				list.emplace_front(std::move(*it));
				found = true;
				break;
			}
			if (not compare(*it, list.back())) {
				list.emplace_back(std::move(*it));
				found = true;
				break;
			}
		}

		if (not found) {
			lists.emplace_back();
			lists.back().emplace_back(std::move(*it));
		}
	}

	////////////////////////////////////////////////////////////
	// Merge encroaching lists

	while (lists.size() > 2) {
		if (lists.size() % 2 != 0) {
			auto last_it = std::prev(std::end(lists));
			auto last_1_it = std::prev(last_it);
			last_1_it->merge(*last_it, compare);
			lists.pop_back();
		}

		auto first_it = std::begin(lists);
		auto half_it = first_it + lists.size() / 2;
		while (half_it != std::end(lists)) {
			first_it->merge(*half_it, compare);
			++first_it;
			++half_it;
		}

		lists.erase(std::begin(lists) + lists.size() / 2, std::end(lists));
	}

	// Merge lists back into the original collection

	if (lists.size() == 2) {
		merge_move(
			std::begin(lists.front()), std::end(lists.front()),
			std::begin(lists.back()), std::end(lists.back()),
			first, compare
		);
	}
	else if (lists.size() == 1) {
		std::move(std::begin(lists.front()), std::end(lists.front()), first);
	}
}




