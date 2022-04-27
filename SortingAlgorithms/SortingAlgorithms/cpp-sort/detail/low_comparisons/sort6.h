/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2017 Morwenn
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
#ifndef CPPSORT_DETAIL_LOW_COMPARISONS_SORT6_H_
#define CPPSORT_DETAIL_LOW_COMPARISONS_SORT6_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <utility>
#include <cpp-sort/sorter_traits.h>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/functional.h>
#include <cpp-sort/utility/iter_move.h>
#include "../rotate_right.h"
#include "../swap_if.h"

namespace cppsort
{
namespace detail
{
    template<>
    struct low_comparisons_sorter_impl<6u>
    {
        template<
            typename RandomAccessIterator,
            typename Compare = std::less<>,
            typename Projection = utility::identity,
            typename = std::enable_if_t<is_projection_iterator_v<
                Projection, RandomAccessIterator, Compare
            >>
        >
        auto operator()(RandomAccessIterator first, RandomAccessIterator,
                        Compare compare={}, Projection projection={}) const
            -> void
        {
            using utility::iter_move;
            using utility::iter_swap;
            auto&& comp = utility::as_function(compare);
            auto&& proj = utility::as_function(projection);

            // Order elements pair-wise

            iter_swap_if(first, first + 1u, compare, projection);
            iter_swap_if(first + 2u, first + 3u, compare, projection);
            iter_swap_if(first + 4u, first + 5u, compare, projection);

            // Order pairs of elements by max value

            if (comp(proj(first[3u]), proj(first[1u]))) {
                iter_swap(first, first + 2u);
                iter_swap(first + 1u, first + 3u);
            }

            if (comp(proj(first[5u]), proj(first[3u]))) {
                if (comp(proj(first[5u]), proj(first[1u]))) {
                    {
                        auto tmp = iter_move(first + 5u);
                        first[5u] = iter_move(first + 3u);
                        first[3u] = iter_move(first + 1u);
                        first[1u] = std::move(tmp);
                    }
                    {
                        auto tmp = iter_move(first + 4u);
                        first[4u] = iter_move(first + 2u);
                        first[2u] = iter_move(first + 0u);
                        first[0u] = std::move(tmp);
                    }
                } else {
                    iter_swap(first + 2u, first + 4u);
                    iter_swap(first + 3u, first + 5u);
                }
            }

            // Merge-insert minimal elements

            if (comp(proj(first[4u]), proj(first[1u]))) {
                // Insert last element in [0, 1, 3]
                if (comp(proj(first[4u]), proj(first[0u]))) {
                    rotate_right<5u>(first);
                } else {
                    rotate_right<4u>(first + 1u);
                }
            } else {
                if (comp(proj(first[4u]), proj(first[3u]))) {
                    rotate_right<3u>(first + 2u);
                } else {
                    // Insert 2 in [0, 1, 3]
                    if (comp(proj(first[2u]), proj(first[0u]))) {
                        rotate_right<3u>(first);
                    } else {
                        iter_swap_if(first + 1u, first + 2u,
                                     std::move(compare), std::move(projection));
                    }
                    return;
                }
            }

            // Insert 3 in [0, 1, 2]
            if (comp(proj(first[3u]), proj(first[1u]))) {
                if (comp(proj(first[3u]), proj(first[0u]))) {
                    rotate_right<4u>(first);
                } else {
                    rotate_right<3u>(first + 1u);
                }
            } else {
                iter_swap_if(first + 2u, first + 3u,
                             std::move(compare), std::move(projection));
            }
        }
    };
}}

#endif // CPPSORT_DETAIL_LOW_COMPARISONS_SORT6_H_
