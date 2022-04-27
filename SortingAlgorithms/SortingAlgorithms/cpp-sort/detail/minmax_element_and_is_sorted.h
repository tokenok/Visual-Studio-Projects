/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Morwenn
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
#ifndef CPPSORT_DETAIL_MINMAX_ELEMENT_AND_IS_SORTED_H_
#define CPPSORT_DETAIL_MINMAX_ELEMENT_AND_IS_SORTED_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <iterator>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/functional.h>
#include "minmax_element.h"

namespace cppsort
{
namespace detail
{
    template<
        typename ForwardIterator,
        typename Compare = std::less<>,
        typename Projection = utility::identity
    >
    auto minmax_element_and_is_sorted(ForwardIterator first, ForwardIterator last,
                                      Compare compare={}, Projection projection={})
        -> decltype(auto)
    {
        auto&& comp = utility::as_function(compare);
        auto&& proj = utility::as_function(projection);

        // Function-local result type, only the names of the
        // data members matter
        struct result_type
        {
            ForwardIterator min;
            ForwardIterator max;
            bool is_sorted;
        } result = { first, first, true };

        // 0 or 1 elements
        if (first == last) return result;
        auto next = std::next(first);
        if (next == last) return result;

        // While it is sorted, the min and max are obvious
        auto current = first;
        while (not comp(proj(*next), proj(*current)))
        {
            ++current;
            ++next;

            // The range is fully sorted
            if (next == last)
            {
                result.max = current;
                return result;
            }
        }

        // The range is not sorted, use a regular minmax_element algorithm
        result.is_sorted = false;
        result.min = first;
        result.max = current;

        auto tmp = minmax_element(next, last, compare, projection);
        if (comp(proj(*tmp.first), proj(*result.min)))
        {
            result.min = tmp.first;
        }
        if (not comp(proj(*tmp.second), proj(*result.max)))
        {
            result.max = tmp.second;
        }
        return result;
    }
}}

#endif // CPPSORT_DETAIL_MINMAX_ELEMENT_AND_IS_SORTED_H_
