/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Morwenn
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
#ifndef CPPSORT_DETAIL_FLOATING_POINT_WEIGHT_H_
#define CPPSORT_DETAIL_FLOATING_POINT_WEIGHT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cmath>

namespace cppsort
{
namespace detail
{
    //
    // Helpers for floating point comparison, mostly
    // to implement simple comparators from P0100 by
    // Lawrence Crowl
    //

    template<typename FloatingPoint>
    auto weak_weight(FloatingPoint value)
        -> int
    {
        // Weak order for floating point numbers proposed by Lawrence Crowl:
        // * all positive NaNs equivalent
        // * positive infinity
        // * positive reals
        // * all zeros equivalent
        // * negative reals
        // * negative infinity
        // * all negative NaNs equivalent

        // Only matters for NaN and infinity
        int res = std::signbit(value) ? -1 : 1;
        switch (std::fpclassify(value))
        {
            case FP_NAN:
                return res * 2;
            case FP_INFINITE:
                return res;
            default:
                return 0;
        }
    }

    template<typename FloatingPoint>
    auto total_weight(FloatingPoint value)
        -> int
    {
        // IEEE 754 totalOrder for floating point numbers:
        // * positive quiet NaNs
        // * positive signaling NaNs
        // * positive infinity
        // * positive reals
        // * positive zero
        // * negative zero
        // * negative reals
        // * negative infinity
        // * negative signaling NaNs
        // * negative quiet NaNs

        // Only matters for NaN and infinity
        int res = std::signbit(value) ? -1 : 1;
        switch (std::fpclassify(value))
        {
            case FP_NAN:
                // TODO: discriminate quiet and signaling NaNs
                return res * 2;
            case FP_INFINITE:
                return res;
            default:
                return 0;
        }
    }
}}

#endif // CPPSORT_DETAIL_FLOATING_POINT_WEIGHT_H_
