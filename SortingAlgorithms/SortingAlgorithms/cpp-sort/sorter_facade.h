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
#ifndef CPPSORT_SORTER_FACADE_H_
#define CPPSORT_SORTER_FACADE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <utility>
#include <cpp-sort/refined.h>
#include <cpp-sort/sorter_traits.h>
#include <cpp-sort/utility/functional.h>
#include "detail/config.h"
#include "detail/projection_compare.h"
#include "detail/type_traits.h"

namespace cppsort
{
    namespace detail
    {
        // Helper class to allow to convert a sorter_facade into a variety
        // of function pointers, but only if the wrapped sorter is empty:
        // conversion to function pointer does not make sense when state
        // is involved

        template<typename Sorter, bool IsEmpty>
        class sorter_facade_fptr;

        template<typename Sorter>
        class sorter_facade_fptr<Sorter, true>
        {
            protected:

                // Function pointer types, a type alias is required
                // for the function pointer conversion operator syntax
                // to be valid

                template<typename Iterable>
                using fptr_t = detail::invoke_result_t<Sorter, Iterable&>(*)(Iterable&);

                template<typename Iterable>
                using fptr_rvalue_t = detail::invoke_result_t<Sorter, Iterable&&>(*)(Iterable&&);

                template<typename Iterable, typename Func>
                using fptr_func_t = detail::invoke_result_t<Sorter, Iterable&, Func>(*)(Iterable&, Func);

                template<typename Iterable, typename Func>
                using fptr_rvalue_func_t = detail::invoke_result_t<Sorter, Iterable&&, Func>(*)(Iterable&&, Func);

                template<typename Iterable, typename Func1, typename Func2>
                using fptr_func2_t
                    = detail::invoke_result_t<Sorter, Iterable&, Func1, Func2>(*)(Iterable&, Func1, Func2);

                template<typename Iterable, typename Func1, typename Func2>
                using fptr_rvalue_func2_t
                    = detail::invoke_result_t<Sorter, Iterable&&, Func1, Func2>(*)(Iterable&&, Func1, Func2);

                template<typename Iterator>
                using fptr_it_t = detail::invoke_result_t<Sorter, Iterator, Iterator>(*)(Iterator, Iterator);

                template<typename Iterator, typename Func>
                using fptr_func_it_t
                    = detail::invoke_result_t<Sorter, Iterator, Iterator, Func>(*)(Iterator, Iterator, Func);

                template<typename Iterator, typename Func1, typename Func2>
                using fptr_func2_it_t
                    = detail::invoke_result_t<Sorter, Iterator, Iterator, Func1, Func2>(*)(Iterator, Iterator, Func1, Func2);

            public:

                ////////////////////////////////////////////////////////////
                // Conversion to function pointers

                template<typename Iterable>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_t<Iterable>() const
                {
                    return [](Iterable& iterable) {
                        return Sorter{}(iterable);
                    };
                }

                template<typename Iterable>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_rvalue_t<Iterable>() const
                {
                    return [](Iterable&& iterable) {
                        return Sorter{}(std::move(iterable));
                    };
                }

                template<typename Iterable, typename Func>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_func_t<Iterable, Func>() const
                {
                    return [](Iterable& iterable, Func func) {
                        return Sorter{}(iterable, func);
                    };
                }

                template<typename Iterable, typename Func>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_rvalue_func_t<Iterable, Func>() const
                {
                    return [](Iterable&& iterable, Func func) {
                        return Sorter{}(std::move(iterable), func);
                    };
                }

                template<typename Iterable, typename Func1, typename Func2>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_func2_t<Iterable, Func1, Func2>() const
                {
                    return [](Iterable& iterable, Func1 func1, Func2 func2) {
                        return Sorter{}(iterable, func1, func2);
                    };
                }

                template<typename Iterable, typename Func1, typename Func2>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_rvalue_func2_t<Iterable, Func1, Func2>() const
                {
                    return [](Iterable&& iterable, Func1 func1, Func2 func2) {
                        return Sorter{}(std::move(iterable), func1, func2);
                    };
                }

                template<typename Iterator>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_it_t<Iterator>() const
                {
                    return [](Iterator first, Iterator last) {
                        return Sorter{}(first, last);
                    };
                }

                template<typename Iterator, typename Func>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_func_it_t<Iterator, Func>() const
                {
                    return [](Iterator first, Iterator last, Func func) {
                        return Sorter{}(first, last, func);
                    };
                }

                template<typename Iterator, typename Func1, typename Func2>
                CPPSORT_CONSTEXPR_AFTER_CXX14
                operator fptr_func2_it_t<Iterator, Func1, Func2>() const
                {
                    return [](Iterator first, Iterator last, Func1 func1, Func2 func2) {
                        return Sorter{}(first, last, func1, func2);
                    };
                }
        };

        template<typename Sorter>
        class sorter_facade_fptr<Sorter, false> {};
    }

    // This class takes an incomplete sorter, analyses it and creates
    // all the methods needed to complete it: additional overloads to
    // operator() and conversions to function pointers

    template<typename Sorter>
    struct sorter_facade:
        Sorter,
        detail::sorter_facade_fptr<
            sorter_facade<Sorter>,
            std::is_empty<Sorter>::value
        >
    {
        ////////////////////////////////////////////////////////////
        // Constructors

        sorter_facade() = default;

        template<typename... Args>
        constexpr sorter_facade(Args&&... args):
            Sorter(std::forward<Args>(args)...)
        {}

        ////////////////////////////////////////////////////////////
        // Non-comparison overloads

        template<typename Iterator>
        auto operator()(Iterator first, Iterator last) const
            -> decltype(Sorter::operator()(std::move(first), std::move(last)))
        {
            return Sorter::operator()(std::move(first), std::move(last));
        }

        template<typename Iterable>
        auto operator()(Iterable&& iterable) const
            -> std::enable_if_t<
                detail::has_sort<Sorter, Iterable>::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable)))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable));
        }

        template<typename Iterable>
        auto operator()(Iterable&& iterable) const
            -> std::enable_if_t<
                not detail::has_sort<Sorter, Iterable>::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable)))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable));
        }

        ////////////////////////////////////////////////////////////
        // Comparison overloads

        template<typename Iterator, typename Compare>
        auto operator()(Iterator first, Iterator last, Compare compare) const
            -> std::enable_if_t<
                detail::has_comparison_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Compare>
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last),
                                            refined<decltype(*first)>(std::move(compare))))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last),
                                      refined<decltype(*first)>(std::move(compare)));
        }

        template<typename Iterable, typename Compare>
        auto operator()(Iterable&& iterable, Compare compare) const
            -> std::enable_if_t<
                detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Compare>
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(compare))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(compare)));
        }

        template<typename Iterable, typename Compare>
        auto operator()(Iterable&& iterable, Compare compare) const
            -> std::enable_if_t<
                not detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Compare>
                >::value &&
                detail::has_comparison_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Compare>
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(compare))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(compare)));
        }

        ////////////////////////////////////////////////////////////
        // Projection overloads

        template<typename Iterator, typename Projection>
        auto operator()(Iterator first, Iterator last, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Projection>
                >::value &&
                detail::has_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Projection>
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last),
                                            refined<decltype(*first)>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last),
                                      refined<decltype(*first)>(std::move(projection)));
        }

        template<typename Iterator, typename Projection>
        auto operator()(Iterator first, Iterator last, Projection projection) const
            -> std::enable_if_t<
                not detail::has_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Projection>
                >::value &&
                detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    std::less<>,
                    refined_t<decltype(*first), Projection>
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last), std::less<>{},
                                            refined<decltype(*first)>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last), std::less<>{},
                                      refined<decltype(*first)>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable), std::less<>{},
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable), std::less<>{},
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, Projection projection) const
            -> std::enable_if_t<
                not detail::has_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable), std::less<>{},
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable), std::less<>{},
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        ////////////////////////////////////////////////////////////
        // std::less<> overloads

        template<typename Iterator>
        auto operator()(Iterator first, Iterator last, std::less<>) const
            -> std::enable_if_t<
                not detail::has_comparison_sort_iterator<Sorter, Iterator, std::less<>>::value,
                decltype(Sorter::operator()(std::move(first), std::move(last)))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last));
        }

        template<typename Iterable>
        auto operator()(Iterable&& iterable, std::less<>) const
            -> std::enable_if_t<
                not detail::has_comparison_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>
                >::value,
                decltype(operator()(std::forward<Iterable>(iterable)))
            >
        {
            return operator()(std::forward<Iterable>(iterable));
        }

        ////////////////////////////////////////////////////////////
        // utility::identity overloads

        template<typename Iterator>
        auto operator()(Iterator first, Iterator last, utility::identity) const
            -> std::enable_if_t<
                not detail::has_projection_sort_iterator<Sorter, Iterator, utility::identity>::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    std::less<>,
                    utility::identity
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last)))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last));
        }

        template<typename Iterable>
        auto operator()(Iterable&& iterable, utility::identity) const
            -> std::enable_if_t<
                not detail::has_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                utility::identity
                >::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    utility::identity
                >::value,
                decltype(operator()(std::forward<Iterable>(iterable)))
            >
        {
            return operator()(std::forward<Iterable>(iterable));
        }

        ////////////////////////////////////////////////////////////
        // Fused comparison-projection overloads

        template<typename Iterator, typename Compare, typename Projection>
        auto operator()(Iterator first, Iterator last,
                        Compare compare, Projection projection) const
            -> std::enable_if_t<
                detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Compare>,
                    refined_t<decltype(*first), Projection>
                >::value,
                decltype(Sorter::operator()(first, last,
                                            refined<decltype(*first)>(std::move(compare)),
                                            refined<decltype(*first)>(std::move(projection))))
            >
        {
            return Sorter::operator()(first, last,
                                      refined<decltype(*first)>(std::move(compare)),
                                      refined<decltype(*first)>(std::move(projection)));
        }

        template<typename Iterable, typename Compare, typename Projection>
        auto operator()(Iterable&& iterable, Compare compare, Projection projection) const
            -> std::enable_if_t<
                detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Compare>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(compare)),
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(compare)),
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Compare, typename Projection>
        auto operator()(Iterable&& iterable, Compare compare, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Compare>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Compare>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable),
                                            refined<decltype(*std::begin(iterable))>(compare),
                                            refined<decltype(*std::begin(iterable))>(projection)))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(compare)),
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        ////////////////////////////////////////////////////////////
        // std::less<> and utility::identity overloads

        template<typename Iterator>
        auto operator()(Iterator first, Iterator last, std::less<>, utility::identity) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    std::less<>,
                    utility::identity
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last)))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last));
        }

        template<typename Iterable>
        auto operator()(Iterable&& iterable, std::less<>, utility::identity) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    utility::identity
                >::value,
                decltype(operator()(std::forward<Iterable>(iterable)))
            >
        {
            return operator()(std::forward<Iterable>(iterable));
        }

        template<typename Iterator, typename Projection>
        auto operator()(Iterator first, Iterator last, std::less<>, Projection projection) const
            -> std::enable_if_t<
                detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    std::less<>,
                    refined_t<decltype(*first), Projection>
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last), std::less<>{},
                                            refined<decltype(*first)>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last), std::less<>{},
                                      refined<decltype(*first)>(std::move(projection)));
        }

        template<typename Iterator, typename Projection>
        auto operator()(Iterator first, Iterator last, std::less<>, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    std::less<>,
                    refined_t<decltype(*first), Projection>
                >::value &&
                detail::has_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Projection>
                >::value,
                decltype(Sorter::operator()(std::move(first), std::move(last),
                                            refined<decltype(*first)>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::move(first), std::move(last),
                                      refined<decltype(*first)>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, std::less<>, Projection projection) const
            -> std::enable_if_t<
                detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable), std::less<>{},
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable), std::less<>{},
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, std::less<>, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable), std::less<>{},
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable), std::less<>{},
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, std::less<>, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, std::less<>, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort<
                    Sorter,
                    Iterable,
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_projection_sort<
                    Sorter,
                    Iterable,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable),
                                            refined<decltype(*std::begin(iterable))>(std::move(projection))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable),
                                      refined<decltype(*std::begin(iterable))>(std::move(projection)));
        }

        ////////////////////////////////////////////////////////////
        // Embed projection in comparison

        template<typename Iterator, typename Projection>
        auto operator()(Iterator first, Iterator last, Projection projection) const
            -> std::enable_if_t<
                not detail::has_projection_sort_iterator<Sorter, Iterator, Projection>::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    std::less<>,
                    refined_t<decltype(*first), Projection>
                >::value &&
                detail::has_comparison_sort_iterator<
                    Sorter,
                    Iterator,
                    detail::projection_compare<std::less<>, refined_t<decltype(*first), Projection>>
                >::value,
                decltype(Sorter::operator()(first, last,
                                            detail::make_projection_compare(std::less<>{},
                                                                            refined<decltype(*first)>(std::move(projection)))))
            >
        {
            return Sorter::operator()(first, last,
                                      detail::make_projection_compare(std::less<>{},
                                                                      refined<decltype(*first)>(std::move(projection))));
        }

        template<typename Iterator, typename Compare, typename Projection>
        auto operator()(Iterator first, Iterator last,
                        Compare compare, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    Iterator,
                    refined_t<decltype(*first), Compare>,
                    refined_t<decltype(*first), Projection>
                >::value &&
                detail::has_comparison_sort_iterator<
                    Sorter,
                    Iterator,
                    detail::projection_compare<
                        refined_t<decltype(*first), Compare>,
                        refined_t<decltype(*first), Projection>
                    >
                >::value,
                decltype(Sorter::operator()(first, last, detail::make_projection_compare(
                    refined<decltype(*first)>(std::move(compare)),
                    refined<decltype(*first)>(std::move(projection)))))
            >
        {
            return Sorter::operator()(first, last, detail::make_projection_compare(
                refined<decltype(*first)>(std::move(compare)),
                refined<decltype(*first)>(std::move(projection))));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, Projection projection) const
            -> std::enable_if_t<
                not detail::has_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    detail::projection_compare<
                        std::less<>,
                        refined_t<decltype(*std::begin(iterable)), Projection>
                    >
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable), detail::make_projection_compare(
                    std::less<>{}, refined<decltype(*std::begin(iterable))>(std::move(projection)))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable), detail::make_projection_compare(
                std::less<>{}, refined<decltype(*std::begin(iterable))>(std::move(projection))));
        }

        template<typename Iterable, typename Projection>
        auto operator()(Iterable&& iterable, Projection projection) const
            -> std::enable_if_t<
                not detail::has_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    std::less<>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    detail::projection_compare<
                        std::less<>,
                        refined_t<decltype(*std::begin(iterable)), Projection>
                    >
                >::value &&
                detail::has_comparison_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    detail::projection_compare<
                        std::less<>,
                        refined_t<decltype(*std::begin(iterable)), Projection>
                    >
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable),
                                            detail::make_projection_compare(std::less<>{},
                                                                            refined<decltype(*std::begin(iterable))>(std::move(projection)))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable),
                                      detail::make_projection_compare(std::less<>{},
                                                                      refined<decltype(*std::begin(iterable))>(std::move(projection))));
        }

        template<typename Iterable, typename Compare, typename Projection>
        auto operator()(Iterable&& iterable, Compare compare, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Compare>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    detail::projection_compare<
                        refined_t<decltype(*std::begin(iterable)), Compare>,
                        refined_t<decltype(*std::begin(iterable)), Projection>
                    >
                >::value,
                decltype(Sorter::operator()(std::forward<Iterable>(iterable), detail::make_projection_compare(
                    refined<decltype(*std::begin(iterable))>(std::move(compare)),
                    refined<decltype(*std::begin(iterable))>(std::move(projection)))))
            >
        {
            return Sorter::operator()(std::forward<Iterable>(iterable), detail::make_projection_compare(
                refined<decltype(*std::begin(iterable))>(std::move(compare)),
                refined<decltype(*std::begin(iterable))>(std::move(projection))));
        }

        template<typename Iterable, typename Compare, typename Projection>
        auto operator()(Iterable&& iterable, Compare compare, Projection projection) const
            -> std::enable_if_t<
                not detail::has_comparison_projection_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    refined_t<decltype(*std::begin(iterable)), Compare>,
                    refined_t<decltype(*std::begin(iterable)), Projection>
                >::value &&
                not detail::has_comparison_sort<
                    Sorter,
                    Iterable,
                    detail::projection_compare<
                        refined_t<decltype(*std::begin(iterable)), Compare>,
                        refined_t<decltype(*std::begin(iterable)), Projection>
                    >
                >::value &&
                detail::has_comparison_sort_iterator<
                    Sorter,
                    decltype(std::begin(iterable)),
                    detail::projection_compare<
                        refined_t<decltype(*std::begin(iterable)), Compare>,
                        refined_t<decltype(*std::begin(iterable)), Projection>
                    >
                >::value,
                decltype(Sorter::operator()(std::begin(iterable), std::end(iterable), detail::make_projection_compare(
                    refined<decltype(*std::begin(iterable))>(std::move(compare)),
                    refined<decltype(*std::begin(iterable))>(std::move(projection)))))
            >
        {
            return Sorter::operator()(std::begin(iterable), std::end(iterable), detail::make_projection_compare(
                refined<decltype(*std::begin(iterable))>(std::move(compare)),
                refined<decltype(*std::begin(iterable))>(std::move(projection))));
        }
    };
}

#endif // CPPSORT_SORTER_FACADE_H_
