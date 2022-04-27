/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Morwenn
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
#ifndef CPPSORT_ADAPTERS_COUNTING_ADAPTER_H_
#define CPPSORT_ADAPTERS_COUNTING_ADAPTER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <utility>
#include <cpp-sort/fwd.h>
#include <cpp-sort/sorter_facade.h>
#include <cpp-sort/sorter_traits.h>
#include <cpp-sort/utility/adapter_storage.h>
#include "../detail/checkers.h"
#include "../detail/comparison_counter.h"

namespace cppsort
{
    ////////////////////////////////////////////////////////////
    // Adapter

    namespace detail
    {
        template<typename Sorter, typename CountType>
        struct counting_adapter_impl:
            utility::adapter_storage<Sorter>,
            check_iterator_category<Sorter>,
            check_is_always_stable<Sorter>
        {
            counting_adapter_impl() = default;

            constexpr explicit counting_adapter_impl(Sorter&& sorter):
                utility::adapter_storage<Sorter>(std::move(sorter))
            {}

            template<
                typename Iterable,
                typename Compare = std::less<>,
                typename = std::enable_if_t<
                    not is_projection_v<Compare, Iterable>
                >
            >
            auto operator()(Iterable&& iterable, Compare compare={}) const
                -> CountType
            {
                CountType count(0);
                comparison_counter<Compare, CountType> cmp(std::move(compare), count);
                this->get()(std::forward<Iterable>(iterable), std::move(cmp));
                return count;
            }

            template<
                typename Iterator,
                typename Compare = std::less<>,
                typename = std::enable_if_t<
                    not is_projection_iterator_v<Compare, Iterator>
                >
            >
            auto operator()(Iterator first, Iterator last, Compare compare={}) const
                -> CountType
            {
                CountType count(0);
                comparison_counter<Compare, CountType> cmp(std::move(compare), count);
                this->get()(std::move(first), std::move(last), std::move(cmp));
                return count;
            }

            template<
                typename Iterable,
                typename Compare,
                typename Projection,
                typename = std::enable_if_t<
                    is_projection_v<Projection, Iterable, Compare>
                >
            >
            auto operator()(Iterable&& iterable, Compare compare={},
                            Projection projection={}) const
                -> CountType
            {
                CountType count(0);
                comparison_counter<Compare, CountType> cmp(std::move(compare), count);
                this->get()(std::forward<Iterable>(iterable), std::move(cmp), projection);
                return count;
            }

            template<
                typename Iterator,
                typename Compare,
                typename Projection,
                typename = std::enable_if_t<
                    is_projection_iterator_v<Projection, Iterator, Compare>
                >
            >
            auto operator()(Iterator first, Iterator last,
                            Compare compare, Projection projection) const
                -> CountType
            {
                CountType count(0);
                comparison_counter<Compare, CountType> cmp(std::move(compare), count);
                this->get()(std::move(first), std::move(last), std::move(cmp), std::move(projection));
                return count;
            }
        };
    }

    template<typename Sorter, typename CountType>
    struct counting_adapter:
        sorter_facade<detail::counting_adapter_impl<
            Sorter,
            CountType
        >>
    {
        counting_adapter() = default;

        constexpr explicit counting_adapter(Sorter sorter):
            sorter_facade<detail::counting_adapter_impl<Sorter, CountType>>(std::move(sorter))
        {}
    };

    ////////////////////////////////////////////////////////////
    // is_stable specialization

    template<typename Sorter, typename CountType, typename... Args>
    struct is_stable<counting_adapter<Sorter, CountType>(Args...)>:
        is_stable<Sorter(Args...)>
    {};
}

#endif // CPPSORT_ADAPTERS_COUNTING_ADAPTER_H_
