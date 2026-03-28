
#pragma once

/**************************************************************
 * @file   Types.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  utility-types.
 **************************************************************/

#include <cstdint>
#include <concepts>
#include <string>

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

    /**************************************************************
     * @brief convenience vec2- and vec3-casts because
     *        converting between sf::VectorX and glm::vecX is
     *        annoying.
     **************************************************************/

    template <typename T>
    concept is_vec2 = std::same_as<decltype(T::x), decltype(T::y)>;

    template <typename T>
    concept is_vec3 = is_vec2<T> && std::same_as<decltype(T::x), decltype(T::z)>;

    template <typename T>
    concept is_vec = is_vec2<T> || is_vec3<T>;

    template <typename T, typename V>
        requires is_vec2<T> && is_vec2<V>
    constexpr
    T vec2_cast(const V& v)
    {
        using target_type = decltype(T::x);
        return T { 
            static_cast<target_type>(v.x), 
            static_cast<target_type>(v.y) 
        };
    }

    template <typename T, typename V>
        requires is_vec3<T> && is_vec3<V>
    constexpr
    T vec3_cast(const V& v)
    {
        using target_type = decltype(T::x);
        return T { 
            static_cast<target_type>(v.x), 
            static_cast<target_type>(v.y), 
            static_cast<target_type>(v.z)
        };
    }

    template <typename T>
        requires is_vec2<T>
    std::string to_string(const T& v)
    { return std::to_string(v.x) + std::to_string(v.y); }

    template <typename T>
        requires is_vec3<T>
    std::string to_string(const T& v)
    { return std::to_string(v.x) + std::to_string(v.y) + std::to_string(v.z); }

}