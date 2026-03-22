
#pragma once

/**************************************************************
 * @file   Types.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  utility-types.
 **************************************************************/

#include <cstdint>

namespace ng 
{
    /**************************************************************
     * @brief abbreviated integer and floating-point types.
     **************************************************************/
    
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32  = float;
    using f64  = double;
    using f128 = long double;

    static_assert(sizeof(f32)  ==  32 / 8);
    static_assert(sizeof(f64)  ==  64 / 8);
    static_assert(sizeof(f128) == 128 / 8);
}