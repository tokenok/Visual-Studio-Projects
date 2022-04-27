/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2019 Morwenn
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
#ifndef CPPSORT_COMPARATORS_NATURAL_LESS_H_
#define CPPSORT_COMPARATORS_NATURAL_LESS_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cctype>
#include <iterator>
#include <utility>
#include <cpp-sort/utility/static_const.h>

namespace cppsort
{
    namespace detail
    {
        ////////////////////////////////////////////////////////////
        // Natural order for char sequences

        template<typename ForwardIterator1, typename ForwardIterator2>
        auto natural_less_impl(ForwardIterator1 begin1, ForwardIterator1 end1,
                               ForwardIterator2 begin2, ForwardIterator2 end2)
            -> bool
        {
            while (begin1 != end1 && begin2 != end2) {
                auto last1 = begin1;
                auto last2 = begin2;
                do {
                    if (not std::isdigit(*last1)) break;
                    ++last1;
                } while (last1 != end1);
                do {
                    if (not std::isdigit(*last2)) break;
                    ++last2;
                } while (last2 != end2);

                if (last1 != begin1 && last2 != begin2) {
                    // Skip leading zeros
                    do {
                        if (*begin1 != '0') break;
                        ++begin1;
                    } while (begin1 != last1);
                    do {
                        if (*begin2 != '0') break;
                        ++begin2;
                    } while (begin2 != last2);

                    // Compare numbers
                    auto size1 = std::distance(begin1, last1);
                    auto size2 = std::distance(begin2, last2);
                    if (size1 != size2) {
                        return size1 < size2;
                    }
                    if (size1 == 0) {
                        return false;
                    }

                    // Sizes are equal and not 0
                    while (begin1 != end1) {
                        if (*begin1 != *begin2) {
                            return *begin1 < *begin2;
                        }
                        ++begin1;
                        ++begin2;
                    }
                }

                if (begin1 == end1) {
                    return begin2 != end2;
                }
                if (begin2 == end2) {
                    return false;
                }

                if (*begin1 != *begin2) {
                    return *begin1 < *begin2;
                }

                ++begin1;
                ++begin2;
            }

            return begin1 == end1 && begin2 != end2;
        }

        template<typename T, typename U>
        auto natural_less(const T& lhs, const U& rhs)
            -> bool
        {
            return natural_less_impl(std::begin(lhs), std::end(lhs),
                                     std::begin(rhs), std::end(rhs));
        }

        ////////////////////////////////////////////////////////////
        // Customization point

        struct natural_less_fn
        {
            template<typename T, typename U>
            constexpr auto operator()(T&& lhs, U&& rhs) const
                noexcept(noexcept(natural_less(std::forward<T>(lhs), std::forward<U>(rhs))))
                -> decltype(natural_less(std::forward<T>(lhs), std::forward<U>(rhs)))
            {
                return natural_less(std::forward<T>(lhs), std::forward<U>(rhs));
            }

            using is_transparent = void;
        };
    }

    using natural_less_t = detail::natural_less_fn;

    namespace
    {
        constexpr auto&& natural_less = utility::static_const<
            detail::natural_less_fn
        >::value;
    }
}

#endif // CPPSORT_COMPARATORS_NATURAL_LESS_H_
