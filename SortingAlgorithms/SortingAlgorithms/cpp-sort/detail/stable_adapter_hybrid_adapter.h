/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Morwenn
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
#ifndef CPPSORT_DETAIL_STABLE_ADAPTER_HYBRID_ADAPTER_H_
#define CPPSORT_DETAIL_STABLE_ADAPTER_HYBRID_ADAPTER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstddef>
#include <type_traits>
#include <utility>

namespace cppsort
{
    template<typename... Sorters>
    struct stable_adapter<hybrid_adapter<Sorters...>>:
        hybrid_adapter<stable_adapter<Sorters>...>
    {
        private:

            template<std::size_t... Indices>
            constexpr explicit stable_adapter(std::index_sequence<Indices...>, hybrid_adapter<Sorters...>&& sorters):
                hybrid_adapter<stable_adapter<Sorters>...>(
                    (stable_adapter<Sorters>(std::move(sorters).template get<Indices, Sorters>()))...
                )
            {}

        public:

            ////////////////////////////////////////////////////////////
            // Construction

            stable_adapter() = default;

            constexpr explicit stable_adapter(hybrid_adapter<Sorters...> sorters):
                stable_adapter(std::make_index_sequence<sizeof...(Sorters)>{}, std::move(sorters))
            {}

            ////////////////////////////////////////////////////////////
            // Sorter traits

            using is_always_stable = std::true_type;
    };
}

#endif // CPPSORT_DETAIL_STABLE_ADAPTER_HYBRID_ADAPTER_H_
