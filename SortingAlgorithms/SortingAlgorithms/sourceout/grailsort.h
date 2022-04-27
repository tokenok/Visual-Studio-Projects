#pragma once


#include <functional>
#include <string>
#include <type_traits>
#include <utility>

template<typename Derived>
struct three_way_compare_base {
	public:

	template<typename T, typename U>
	constexpr auto operator()(T&& lhs, U&& rhs) const
		-> int {
		auto&& compare = derived().base();
		return compare(std::forward<T>(lhs), std::forward<U>(rhs)) ? -1 :
			compare(std::forward<U>(rhs), std::forward<T>(lhs));
	}

	template<typename T, typename U>
	auto lt(T&& x, U&& y) const
		-> decltype(auto) {
		return derived().base()(std::forward<T>(x), std::forward<U>(y));
	}

	template<typename T, typename U>
	auto le(T&& x, U&& y) const
		-> decltype(auto) {
		return not derived().base()(std::forward<U>(y), std::forward<T>(x));
	}

	template<typename T, typename U>
	auto gt(T&& x, U&& y) const
		-> decltype(auto) {
		return derived().base()(std::forward<U>(y), std::forward<T>(x));
	}

	template<typename T, typename U>
	auto ge(T&& x, U&& y) const
		-> decltype(auto) {
		return not derived().base()(std::forward<T>(x), std::forward<U>(y));
	}

	private:

	auto derived() noexcept
		-> Derived& {
		return static_cast<Derived&>(*this);
	}

	auto derived() const noexcept
		-> const Derived& {
		return static_cast<const Derived&>(*this);
	}
};

template<
	typename Compare,
	bool = std::is_empty<Compare>::value && std::is_default_constructible<Compare>::value
>
struct three_way_compare :
	three_way_compare_base<three_way_compare<Compare>> {
	public:

	constexpr three_way_compare(Compare compare) :
		compare(std::move(compare)) {}

	constexpr auto base() const noexcept
		-> Compare {
		return compare;
	}

	private:

	Compare compare;
};

template<typename Compare>
struct three_way_compare<Compare, true> :
	three_way_compare_base<three_way_compare<Compare>> {
	constexpr three_way_compare(Compare) {}

	using three_way_compare_base<three_way_compare<Compare>>::operator();

	constexpr auto base() const noexcept
		-> Compare {
		// If the comparator is empty, we don't need to store it
		return {};
	}
};

template<>
struct three_way_compare<std::less<>, true> :
	three_way_compare_base<three_way_compare<std::less<>>> {
	constexpr three_way_compare(std::less<>) {}

	using three_way_compare_base<three_way_compare<std::less<>>>::operator();

	template<
		typename CharT,
		typename Traits1, typename Alloc1,
		typename Traits2, typename Alloc2
	>
		auto operator()(const std::basic_string<CharT, Traits1, Alloc1>& lhs,
						const std::basic_string<CharT, Traits2, Alloc2>& rhs) const
		-> int {
		return lhs.compare(0, lhs.size(), rhs.data(), rhs.size());
	}

	constexpr auto base() const noexcept
		-> std::less<> {
		return {};
	}
};

template<>
struct three_way_compare<std::greater<>, true> :
	three_way_compare_base<three_way_compare<std::greater<>>> {
	constexpr three_way_compare(std::greater<>) {}

	using three_way_compare_base<three_way_compare<std::greater<>>>::operator();

	template<
		typename CharT,
		typename Traits1, typename Alloc1,
		typename Traits2, typename Alloc2
	>
		auto operator()(const std::basic_string<CharT, Traits1, Alloc1>& lhs,
						const std::basic_string<CharT, Traits2, Alloc2>& rhs) const
		-> int {
		int res = lhs.compare(0, lhs.size(), rhs.data(), rhs.size());
		return (res < 0) ? 1 : -res;
	}

	constexpr auto base() const noexcept
		-> std::greater<> {
		return {};
	}
};


	// cost: 2 * len + nk^2 / 2
template<typename RandomAccessIterator, typename Compare>
auto find_keys(RandomAccessIterator first, RandomAccessIterator last,
			   std::_Iter_diff_t<RandomAccessIterator> key_count,
			   Compare compare)
	-> std::_Iter_diff_t<RandomAccessIterator> {
	

	std::_Iter_diff_t<RandomAccessIterator> dist = 1;
	auto h0 = first;
	for (auto u = std::next(first); u != last; ++u) {
		if (dist == key_count) break;
		auto r = std::lower_bound(h0, h0 + dist, *u, compare.base()) - h0;
		if (r == dist || compare(*u, h0[r]) != 0) {
			h0 = std::rotate(h0, h0 + dist, u);
			std::rotate(h0 + r, u, std::next(u));
			++dist;
		}
	}
	std::rotate(first, h0, h0 + dist);
	return dist;
}

// cost: min(L1, L2)^2 + max(L1, L2)
template<typename RandomAccessIterator, typename Compare>
auto merge_without_buffer(RandomAccessIterator first, RandomAccessIterator middle,
						  RandomAccessIterator last,
						  Compare compare)
	-> void {
	

	if (std::distance(first, middle) < std::distance(middle, last)) {
		while (first != middle) {
			// Binary search left
			auto it = std::lower_bound(middle, last, *first, compare.base());
			if (it != middle) {
				std::rotate(first, middle, it);
				auto delta = std::distance(middle, it);
				first += delta;
				middle += delta;
			}
			if (middle == last) break;
			do {
				++first;
			} while (first != middle && compare(*first, *middle) <= 0);
		}
	}
	else {
		while (middle != last) {
			// Binary search right
			auto it = std::upper_bound(first, middle, *std::prev(last), compare.base());
			if (it != middle) {
				std::rotate(it, middle, last);
				auto delta = std::distance(it, middle);
				middle -= delta;
				last -= delta;
			}
			if (first == middle) break;
			do {
				--last;
			} while (middle != last && compare(*std::prev(middle), *std::prev(last)) <= 0);
		}
	}
}

// arr[M..-1] - buffer, arr[0,L1-1]++arr[L1,L1+L2-1] -> arr[M,M+L1+L2-1]
template<typename RandomAccessIterator, typename Compare>
auto merge_left(RandomAccessIterator first, RandomAccessIterator middle,
				RandomAccessIterator last, RandomAccessIterator M,
				Compare compare)
	-> void {
	
	

	auto left_it = first;
	auto right_it = middle;

	while (right_it != last) {
		if (left_it == middle || compare(*left_it, *right_it) > 0) {
			iter_swap(M, right_it);
			++right_it;
		}
		else {
			iter_swap(M, left_it);
			++left_it;
		}
		++M;
	}
	if (M != left_it) {
		std::swap_ranges(M, M + std::distance(left_it, middle), left_it);
	}
}

template<typename RandomAccessIterator, typename Compare>
auto merge_right(RandomAccessIterator first, RandomAccessIterator middle,
				 RandomAccessIterator last, RandomAccessIterator M,
				 Compare compare)
	-> void {
	
	

	auto p0 = std::prev(M),
		p1 = std::prev(middle),
		p2 = std::prev(last);

	while (p1 >= first) {
		if (p2 < middle || compare(*p1, *p2) > 0) {
			iter_swap(p0, p1);
			--p1;
		}
		else {
			iter_swap(p0, p2);
			--p2;
		}
		--p0;
	}

	if (p2 != p0 && p2 >= middle) {
		do {
			iter_swap(p0, p2);
			--p0;
			--p2;
		} while (p2 != middle);
	}
}

template<typename RandomAccessIterator, typename Compare>
auto smart_merge_with_buffer(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
							 std::_Iter_diff_t<RandomAccessIterator> block_len, int left_over_frag,
							 Compare compare)
	-> std::pair<std::_Iter_diff_t<RandomAccessIterator>, int> {
	
	

	auto out_it = first - block_len,
		left_it = first,
		right_it = middle;
	int frag_type = 1 - left_over_frag;  // 1 if inverted

	while (left_it != middle && right_it != last) {
		if (compare(*left_it, *right_it) - frag_type < 0) {
			iter_swap(out_it, left_it);
			++left_it;
		}
		else {
			iter_swap(out_it, right_it);
			++right_it;
		}
		++out_it;
	}

	//auto p1 = left_it, q1 = middle, p2 = right_it, q2 = last;
	std::_Iter_diff_t<RandomAccessIterator> len;
	if (left_it < middle) {
		len = std::distance(left_it, middle);
		do {
			iter_swap(--middle, --last);
		} while (left_it != middle);
	}
	else {
		len = std::distance(right_it, last);
		left_over_frag = frag_type;
	}
	return {len, left_over_frag};
}

template<typename RandomAccessIterator, typename Compare>
auto smart_merge_without_buffer(RandomAccessIterator first, RandomAccessIterator middle,
								RandomAccessIterator last, int left_over_frag,
								Compare compare)
	-> std::pair<int, int> {
	

	if (middle == last) {
		return {std::distance(first, middle), left_over_frag};
	}

	int frag_type = 1 - left_over_frag;
	if (first != middle && compare(*std::prev(middle), *middle) - frag_type >= 0) {
		while (first != middle) {
			auto len = frag_type ? (std::lower_bound(middle, last, *first, compare.base()) - middle)
				: (std::upper_bound(middle, last, *first, compare.base()) - middle);
			if (len != 0) {
				std::rotate(first, middle, middle + len);
				first += len;
				middle += len;
			}
			if (middle == last) {
				return {std::distance(first, middle), left_over_frag};
			}
			do {
				++first;
			} while (first != middle && compare(*first, *middle) - frag_type < 0);
		}
	}
	return {std::distance(middle, last), frag_type};
}

// Sort With Extra Buffer

template<typename ForwardIterator, typename Compare>
auto merge_left_with_extra_buffer(ForwardIterator first, ForwardIterator middle,
								  ForwardIterator last, ForwardIterator out,
								  Compare compare)
	-> void {
	
	

	auto it = middle;
	while (it != last) {
		if (first == middle || compare(*first, *it) > 0) {
			*out = std::move(*it);
			++out;
			++it;
		}
		else {
			*out = std::move(*first);
			++out;
			++first;
		}
	}
	if (out != first) {
		std::move(first, middle, out);
	}
}

template<typename RandomAccessIterator, typename Compare>
auto smart_merge_with_extra_buffer(RandomAccessIterator first, RandomAccessIterator middle,
								   RandomAccessIterator last,
								   int left_over_frag, std::_Iter_diff_t<RandomAccessIterator> block_len,
								   Compare compare)
	-> std::pair<int, int> {
	
	

	auto out = first - block_len;
	auto it = middle;

	int frag_type = 1 - left_over_frag;  // 1 if inverted
	while (first < middle && it < last) {
		if (compare(*first, *it) - frag_type < 0) {
			*out = std::move(*first);
			++first;
		}
		else {
			*out = std::move(*it);
			++it;
		}
		++out;
	}

	if (first < middle) {
		auto left_over_len = std::distance(first, middle);
		do {
			*--last = std::move(*--middle);
		} while (first != middle);
		return {left_over_len, left_over_frag};
	}
	return {std::distance(it, last), frag_type};
}

// arr - starting array. arr[-lblock..-1] - buffer (if havebuf).
// lblock - length of regular blocks. First nblocks are stable sorted by 1st elements and key-coded
// keys - arrays of keys, in same order as blocks. key<midkey means stream A
// nblock2 are regular blocks from stream A. llast is length of last (irregular) block from stream B, that should go before nblock2 blocks.
// llast=0 requires nblock2=0 (no irregular blocks). llast>0, nblock2=0 is possible.
template<typename RandomAccessIterator, typename Compare>
auto merge_buffers_left_with_extra_buffer(RandomAccessIterator keys, RandomAccessIterator midkey,
										  RandomAccessIterator arr, int nblock, int lblock, int nblock2,
										  int llast, Compare compare)
	-> void {
	

	if (nblock == 0) {
		auto l = arr + nblock2 * lblock;
		merge_left_with_extra_buffer(arr, l, l + llast, arr - lblock, compare);
		return;
	}

	int lrest = lblock;
	int frest = compare(*keys, *midkey) < 0 ? 0 : 1;
	auto pidx = arr + lblock;
	for (int cidx = 1; cidx < nblock; (void) ++cidx, pidx += lblock) {
		auto prest = pidx - lrest;
		int fnext = compare(keys[cidx], *midkey) < 0 ? 0 : 1;
		if (fnext == frest) {
			std::move(prest, prest + lrest, prest - lblock);
			prest = pidx;
			lrest = lblock;
		}
		else {
			std::tie(lrest, frest) = smart_merge_with_extra_buffer(
				prest, prest + lrest, prest + (lrest + lblock),
				frest, lblock, compare);
		}
	}
	auto prest = pidx - lrest;
	if (llast) {
		if (frest) {
			std::move(prest, prest + lrest, prest - lblock);
			prest = pidx;
			lrest = lblock * nblock2;
		}
		else {
			lrest += lblock * nblock2;
		}
		merge_left_with_extra_buffer(prest, prest + lrest, prest + (lrest + llast), prest - lblock,
									 std::move(compare));
	}
	else {
		std::move(prest, prest + lrest, prest - lblock);
	}
}

/***** End Sort With Extra Buffer *****/

// arr - starting array. arr[-lblock..-1] - buffer (if havebuf).
// lblock - length of regular blocks. First nblocks are stable sorted by 1st elements and key-coded
// keys - arrays of keys, in same order as blocks. key<midkey means stream A
// nblock2 are regular blocks from stream A. llast is length of last (irregular) block from stream B,
// that should go before nblock2 blocks.
// llast=0 requires nblock2=0 (no irregular blocks). llast>0, nblock2=0 is possible.
template<typename RandomAccessIterator, typename Compare>
auto merge_buffers_left(RandomAccessIterator keys, RandomAccessIterator midkey,
						RandomAccessIterator arr, int nblock, int lblock, bool havebuf,
						int nblock2, int llast, Compare compare)
	-> void {


	if (nblock == 0) {
		auto l = arr + nblock2 * lblock;
		if (havebuf) {
			merge_left(arr, l, l + llast, arr - lblock, std::move(compare));
		}
		else {
			merge_without_buffer(arr, l, l + llast, std::move(compare));
		}
		return;
	}

	int lrest = lblock;
	int frest = compare(*keys, *midkey) < 0 ? 0 : 1;
	auto pidx = arr + lblock;
	for (int cidx = 1; cidx < nblock; (void) ++cidx, pidx += lblock) {
		auto prest = pidx - lrest;
		int fnext = compare(keys[cidx], *midkey) < 0 ? 0 : 1;
		if (fnext == frest) {
			if (havebuf) {
				std::swap_ranges(prest - lblock, prest - (lblock - lrest), prest);
			}
			prest = pidx;
			lrest = lblock;
		}
		else {
			if (havebuf) {
				std::tie(lrest, frest) = smart_merge_with_buffer(
					prest, prest + lrest, prest + (lrest + lblock),
					lblock, frest, compare);
			}
			else {
				std::tie(lrest, frest) = smart_merge_without_buffer(
					prest, prest + lrest, prest + (lrest + lblock),
					frest, compare);
			}

		}
	}

	auto prest = pidx - lrest;
	if (llast) {
		if (frest) {
			if (havebuf) {
				std::swap_ranges(prest - lblock, prest - (lblock - lrest), prest);
			}
			prest = pidx;
			lrest = lblock * nblock2;
		}
		else {
			lrest += lblock * nblock2;
		}
		if (havebuf) {
			merge_left(prest, prest + lrest, prest + (lrest + llast), prest - lblock,
					   std::move(compare));
		}
		else {
			merge_without_buffer(prest, prest + lrest, prest + (lrest + llast),
								 std::move(compare));
		}
	}
	else {
		if (havebuf) {
			std::swap_ranges(prest, prest + lrest, prest - lblock);
		}
	}
}

// build blocks of length K
// input: [-K,-1] elements are buffer
// output: first K elements are buffer, blocks 2*K and last subblock sorted
template<typename RandomAccessIterator, typename BufferIterator,
	typename Compare>
	auto build_blocks(RandomAccessIterator first, RandomAccessIterator last,
					  int K, BufferIterator extbuf, int LExtBuf,
					  Compare compare)
	-> void {
	
	
	
	auto size = std::distance(first, last);

	int kbuf = std::min(K, LExtBuf);
	while (kbuf & (kbuf - 1)) {
		kbuf &= kbuf - 1;  // max power or 2 - just in case
	}

	int h;
	if (kbuf) {
		std::move(first - kbuf, first, extbuf);
		for (int m = 1; m < size; m += 2) {
			int u = 0;
			if (compare(first[m - 1], first[m]) > 0) {
				u = 1;
			}
			first[m - 3] = std::move(*(first + (m - 1 + u)));
			first[m - 2] = std::move(*(first + (m - u)));
		}
		if (size % 2) {
			first[size - 3] = std::move(*(first + (size - 1)));
		}
		first -= 2;
		last -= 2;
		for (h = 2; h < kbuf; h *= 2) {
			auto p0 = first;
			auto p1 = last - 2 * h;
			while (p0 <= p1) {
				merge_left_with_extra_buffer(p0, p0 + h, p0 + (h + h), p0 - h, compare);
				p0 += 2 * h;
			}
			int rest = std::distance(p0, last);
			if (rest > h) {
				merge_left_with_extra_buffer(p0, p0 + h, last, p0 - h, compare);
			}
			else {
				for (; p0 < last; ++p0) {
					p0[-h] = std::move(*p0);
				}
			}
			first -= h;
			last -= h;
		}
		std::move(extbuf, extbuf + kbuf, last);
	}
	else {
		for (int m = 1; m < size; m += 2) {
			int u = 0;
			if (compare(first[m - 1], first[m]) > 0) {
				u = 1;
			}
			iter_swap(first + (m - 3), first + (m - 1 + u));
			iter_swap(first + (m - 2), first + (m - u));
		}
		if (size % 2) {
			iter_swap(last - 1, last - 3);
		}
		first -= 2;
		last -= 2;
		h = 2;
	}
	for (; h < K; h *= 2) {
		auto p0 = first;
		auto p1 = last - 2 * h;
		while (p0 <= p1) {
			merge_left(p0, p0 + h, p0 + (h + h), p0 - h, compare);
			p0 += 2 * h;
		}
		int rest = std::distance(p0, last);
		if (rest > h) {
			merge_left(p0, p0 + h, last, p0 - h, compare);
		}
		else {
			std::rotate(p0 - h, p0, last);
		}
		first -= h;
		last -= h;
	}
	int restk = size % (2 * K);
	auto p = last - restk;
	if (restk <= K) {
		std::rotate(p, last, last + K);
	}
	else {
		merge_right(p, p + K, last, last + K, compare);
	}
	while (p > first) {
		p -= 2 * K;
		merge_right(p, p + K, p + (K + K), p + (K + K + K), compare);
	}
}

// keys are on the left of arr. Blocks of length LL combined. We'll combine them in pairs
// LL and nkeys are powers of 2. (2*LL/lblock) keys are guarantied
template<typename RandomAccessIterator, typename BufferIterator,
	typename Compare>
	auto combine_blocks(RandomAccessIterator keys, RandomAccessIterator arr, int len, int LL,
						int lblock, bool havebuf, BufferIterator xbuf, bool usexbuf,
						Compare compare)
	-> void {
	
	
	

	int M = len / (2 * LL);
	int lrest = len % (2 * LL);
	if (lrest <= LL) {
		len -= lrest;
		lrest = 0;
	}
	if (usexbuf) {
		std::move(arr - lblock, arr, xbuf);
	}
	for (int b = 0; b <= M; ++b) {
		if (b == M && lrest == 0) break;
		auto arr1 = arr + b * 2 * LL;
		int NBlk = (b == M ? lrest : 2 * LL) / lblock;
		::insertion_sort(keys, keys + (NBlk + (b == M ? 1 : 0)),
					   compare.base());
		int midkey = LL / lblock;
		for (int u = 1; u < NBlk; ++u) {
			int p = u - 1;
			for (int v = u; v < NBlk; ++v) {
				int kc = compare(arr1[p*lblock], arr1[v*lblock]);
				if (kc > 0 || (kc == 0 && compare(keys[p], keys[v]) > 0)) {
					p = v;
				}
			}
			if (p != u - 1) {
				std::swap_ranges(arr1 + (u - 1)*lblock, arr1 + u * lblock, arr1 + p * lblock);
				iter_swap(keys + (u - 1), keys + p);
				if (midkey == u - 1 || midkey == p) {
					midkey ^= (u - 1) ^ p;
				}
			}
		}
		int nbl2 = 0;
		int llast = 0;
		if (b == M) {
			llast = lrest % lblock;
		}
		if (llast != 0) {
			while (nbl2 < NBlk &&
				   compare(arr1[NBlk*lblock], arr1[(NBlk - nbl2 - 1)*lblock]) < 0) {
				++nbl2;
			}
		}
		if (usexbuf) {
			merge_buffers_left_with_extra_buffer(keys, keys + midkey, arr1, NBlk - nbl2, lblock, nbl2, llast,
												 compare);
		}
		else {
			merge_buffers_left(keys, keys + midkey, arr1, NBlk - nbl2, lblock, havebuf, nbl2, llast,
							   compare);
		}
	}
	if (usexbuf) {
		for (int p = len; --p >= 0;) {
			arr[p] = std::move(*(arr + (p - lblock)));
		}
		std::move(xbuf, xbuf + lblock, arr - lblock);
	}
	else if (havebuf) {
		while (--len >= 0) {
			iter_swap(arr + len, arr + (len - lblock));
		}
	}
}

template<typename RandomAccessIterator, typename Compare>
auto lazy_stable_sort(RandomAccessIterator first, RandomAccessIterator last,
					  Compare compare)
	-> void {
	
	

	for (auto it = std::next(first); it < last; it += 2) {
		if (compare(*std::prev(it), *it) > 0) {
			iter_swap(std::prev(it), it);
		}
	}

	auto size = std::distance(first, last);
	for (int h = 2; h < size; h *= 2) {
		auto p0 = first;
		auto p1 = last - 2 * h;
		while (p0 <= p1) {
			merge_without_buffer(p0, p0 + h, p0 + (h + h), compare);
			p0 += 2 * h;
		}
		int rest = std::distance(p0, last);
		if (rest > h) {
			merge_without_buffer(p0, p0 + h, last, compare);
		}
	}
}

template<typename BidirectionalIterator, typename Compare>
auto insertion_sort(BidirectionalIterator first, BidirectionalIterator last,
					Compare compare)
	-> void {
	if (first == last) return;

	for (BidirectionalIterator cur = std::next(first); cur != last; ++cur) {
		BidirectionalIterator sift = cur;
		BidirectionalIterator sift_1 = std::prev(cur);

		// Compare first so we can avoid 2 moves for
		// an element already positioned correctly.
		if (compare(*sift, *sift_1)) {
			auto tmp = *sift;
			do {
				*sift = *sift_1;
			} while (--sift != first && compare(tmp, *--sift_1));
			*sift = std::move(tmp);
		}
	}
}

template<typename RandomAccessIterator, typename BufferIterator,
	typename Compare>
	auto common_sort(RandomAccessIterator first, RandomAccessIterator last,
					 BufferIterator extbuf, int LExtBuf,
					 Compare compare)
	-> void {
	auto size = std::distance(first, last);
	if (size < 16) {
		insertion_sort(first, last, compare.base());
		return;
	}

	int lblock = 1;
	while (lblock * lblock < size) {
		lblock *= 2;
	}
	int nkeys = (size - 1) / lblock + 1;
	int findkeys = find_keys(first, last, nkeys + lblock, compare);
	bool havebuf = true;
	if (findkeys < nkeys + lblock) {
		if (findkeys < 4) {
			lazy_stable_sort(first, last, std::move(compare));
			return;
		}
		nkeys = lblock;
		while (nkeys > findkeys) {
			nkeys /= 2;
		}
		havebuf = false;
		lblock = 0;
	}
	auto ptr = first + (lblock + nkeys);
	int cbuf = havebuf ? lblock : nkeys;
	if (havebuf) {
		build_blocks(ptr, last, cbuf, extbuf, LExtBuf, compare);
	}
	else {
		using T = std::_Iter_value_t<BufferIterator>;
		build_blocks(ptr, last, cbuf, static_cast<T*>(nullptr), 0, compare);
	}

	// 2*cbuf are built
	while (std::distance(ptr, last) > (cbuf *= 2)) {
		int lb = lblock;
		bool chavebuf = havebuf;
		if (not havebuf) {
			if (nkeys > 4 && nkeys / 8 * nkeys >= cbuf) {
				lb = nkeys / 2;
				chavebuf = true;
			}
			else {
				int nk = 1;
				long long s = (long long)cbuf * findkeys / 2;
				while (nk < nkeys && s != 0) {
					nk *= 2;
					s /= 8;
				}
				lb = (2 * cbuf) / nk;
			}
		}

		combine_blocks(first, ptr, std::distance(ptr, last), cbuf, lb,
					   chavebuf, extbuf, chavebuf && lb <= LExtBuf,
					   compare);
	}
	insertion_sort(first, ptr, compare.base());
	merge_without_buffer(first, ptr, last, std::move(compare));
}

//template<typename BufferProvider, typename RandomAccessIterator,
//	typename Compare>
//	auto grailsort(RandomAccessIterator first, RandomAccessIterator last,
//					Compare compare)
//	-> void {
//	
//	using rvalue_reference = remove_cvref_t<rvalue_reference_t<RandomAccessIterator>>;
//
//	// Allocate temporary buffer
//	auto size = std::distance(first, last);
//	typename BufferProvider::template buffer<rvalue_reference> buffer(size);
//
//	using compare_t = std::remove_reference_t<decltype(utility::as_function(compare))>;
//	common_sort(std::move(first), std::move(last), buffer.begin(), buffer.size(),
//				three_way_compare<compare_t>(utility::as_function(compare)));
//}


struct as_function_fn {
	template<typename T>
	constexpr auto operator()(T && t) const
		noexcept(std::is_nothrow_constructible<T, T>::value) {
		return std::forward<T>(t);
	}
};

template<typename T>
struct static_const {
	static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;

constexpr auto&& as_function = static_const<as_function_fn>::value;


template<class T, class C>
inline void grailsort_nobuffer(T _First, T _Last, C compare) {
	std::vector<T::value_type> buffer(0);

	using compare_t = std::remove_reference_t<decltype(as_function(compare))>;

	common_sort(_First, _Last, buffer.begin(), buffer.size(), three_way_compare<compare_t>(as_function(compare)));
}

template<class T, class C>
inline void grailsort_fixedbuffer(T _First, T _Last, C compare) {
	std::vector<T::value_type> buffer(512);

	using compare_t = std::remove_reference_t<decltype(as_function(compare))>;

	common_sort(_First, _Last, buffer.begin(), buffer.size(), three_way_compare<compare_t>(as_function(compare)));
}

template<class T, class C>
inline void grailsort_fullbuffer(T _First, T _Last, C compare) {
	std::vector<T::value_type> buffer(std::distance(_First, _Last));

	using compare_t = std::remove_reference_t<decltype(as_function(compare))>;

	common_sort(_First, _Last, buffer.begin(), buffer.size(), three_way_compare<compare_t>(as_function(compare)));
}

template<class T>
inline void grailsort(T _First, T _Last) {
	grailsort_nobuffer(_First, _Last, std::less<>());
}





















