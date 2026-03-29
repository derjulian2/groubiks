
#pragma once

/**************************************************************
 * @file   Types.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  utility-types.
 **************************************************************/

#include <cstdint>
#include <cstring>
#include <cassert>
#include <string>
#include <concepts>

#include <glm/glm.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Glsl.hpp>

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
     * @brief vector- and matrix-conversions to easily
     *        convert between glm and SFML-types.
     *
     *        maybe overkill to make such a generic inteface,
     *        but hey, maybe i've learned something about traits.
     **************************************************************/

    template <typename T>
    struct vector_traits { };

    template <typename T>
    struct matrix_traits { };

    template <typename T>
    concept vector_like = requires (T& t, const T& ct)
    {
        typename vector_traits<T>::value_type;
        { vector_traits<T>::null() } -> std::same_as<T>;
        { vector_traits<T>::length() } -> std::convertible_to<std::size_t>;
        { vector_traits<T>::get(t, 0) } -> std::same_as<typename vector_traits<T>::value_type&>;
        { vector_traits<T>::get(ct, 0) } -> std::same_as<const typename vector_traits<T>::value_type&>;
    };

    template <typename T>
    concept matrix_like = requires (T& t, const T& ct)
    {
        typename matrix_traits<T>::value_type;
        { matrix_traits<T>::null() } -> std::same_as<T>;
        { matrix_traits<T>::col_count() } -> std::convertible_to<std::size_t>;
        { matrix_traits<T>::row_count() } -> std::convertible_to<std::size_t>;
        { matrix_traits<T>::get(t, 0, 0) } -> std::same_as<typename matrix_traits<T>::value_type&>;
        { matrix_traits<T>::get(ct, 0, 0) } -> std::same_as<const typename matrix_traits<T>::value_type&>;
    };

    /*
     * type-generic cast for vector-types.
     */
    template <typename T, typename U>
        requires vector_like<T> && vector_like<U>
    constexpr
    T vector_cast(const U& v)
    {
        using target_traits = vector_traits<T>;
        using source_traits = vector_traits<U>;
        using target_type   = typename target_traits::value_type;

        T res = target_traits::null();
        for (std::size_t i = 0; i < source_traits::length(); ++i)
            target_traits::get(res, i) = static_cast<target_type>(source_traits::get(v, i));
        return res;
    }

    /*
     * type-generic cast for matrix-types.
     */
    template <typename T, typename U>
        requires matrix_like<T> && matrix_like<U>
    constexpr
    T matrix_cast(const U& m)
    {
        using target_traits = matrix_traits<T>;
        using source_traits = matrix_traits<U>;
        using target_type   = typename target_traits::value_type;

        T res = target_traits::null();
        for (std::size_t i = 0; i < source_traits::col_count(); ++i)
            for (std::size_t j = 0; j < source_traits::row_count(); ++j)
                target_traits::get(res, i, j) = static_cast<target_type>(source_traits::get(m, i, j));
        return res;
    }


    /*
     * vector-to-string conversion.
     */
    template <typename T>
        requires vector_like<T>
    constexpr
    std::string to_string(const T& v,
                          std::string_view left_parens = "(",
                          std::string_view right_parens = ")",
                          std::string_view delim = ", ")
    { 
        using traits = vector_traits<T>;

        std::string res; 
        res += left_parens.data();
        for (std::size_t i = 0; i < traits::length() - 1; ++i)
            res += std::to_string(traits::get(v, i)) + delim.data();
        return res + std::to_string(traits::get(v, traits::length() - 1)) + right_parens.data();
    }

    /*
     * matrix-to-string column-major conversion.
     */
    template <typename T>
        requires matrix_like<T>
    constexpr
    std::string to_string(const T& m,
                          std::string_view left_parens = "[",
                          std::string_view right_parens = "]",
                          std::string_view delim = ", ")
    {
        using traits = matrix_traits<T>;

        std::string res;
        for (std::size_t i = 0; i < traits::col_count(); ++i)
        {
            std::string column;
            column += left_parens.data();
            for (std::size_t j = 0; j < traits::row_count() - 1; ++j)
                column += std::to_string(traits::get(m, i, j)) + delim.data();
            column += std::to_string(traits::get(m, i, traits::row_count() - 1)) + right_parens.data() + '\n';
            res += column;
        }
        return res;
    }

}

/*
 * traits-specializations for glm-types
 */

template <glm::length_t L, typename T, glm::qualifier Q>
struct ng::vector_traits<glm::vec<L, T, Q>>
{
    using vector_type = typename glm::vec<L, T, Q>;
    using value_type  = typename vector_type::value_type;

    static constexpr 
    std::size_t
    length()
    { return vector_type::length(); }

    /*
     * should return a zeroed vector-object.
     */
    static constexpr
    vector_type
    null()
    { return vector_type(static_cast<value_type>(0)); }

    static constexpr
    value_type& get(vector_type& v, std::size_t i)
    { 
        assert(i >= 0 && i < length());
        return v[i]; 
    }

    static constexpr
    const value_type& get(const vector_type& v, std::size_t i)
    { 
        assert(i >= 0 && i < length());
        return v[i]; 
    }
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct ng::matrix_traits<glm::mat<C, R, T, Q>>
{
    using matrix_type = typename glm::mat<C, R, T, Q>;
    using value_type  = typename matrix_type::value_type;
    using row_type    = typename matrix_type::row_type;
    using col_type    = typename matrix_type::col_type;

    static constexpr
    std::size_t
    row_count()
    { return R; }

    static constexpr
    std::size_t
    col_count()
    { return C; }

    /*
     * should return a zeroed matrix object.
     */
    static constexpr
    matrix_type
    null()
    { return matrix_type(static_cast<value_type>(0)); }

    /*
     * returns the element in the ith column and jth row.
     */
    static constexpr
    value_type& get(matrix_type& v, std::size_t i, std::size_t j)
    { 
        assert(i >= 0 && i < col_count());
        assert(j >= 0 && j < row_count());
        return v[i][j]; 
    }

    /*
     * returns the element in the ith column and jth row.
     */
    static constexpr
    const value_type& get(const matrix_type& v, std::size_t i, std::size_t j)
    { 
        assert(i >= 0 && i < col_count());
        assert(j >= 0 && j < row_count());
        return v[i][j]; 
    }
    
};

/*
 * traits-specializations for sfml-types
 */

template <typename T>
struct ng::vector_traits<sf::Vector2<T>>
{
    using vector_type = sf::Vector2<T>;
    using value_type  = T;

    static constexpr 
    std::size_t
    length()
    { return 2; }

    static constexpr
    vector_type
    null()
    { return sf::Vector2<T>(); }


    static constexpr
    value_type& get(vector_type& v, std::size_t i)
    { 
        assert(i >= 0 && i < length());
        switch (i)
        {
        default:
        case (0):
            return v.x;
        case (1):
            return v.y;
        }
    }

    static constexpr
    const value_type& get(const vector_type& v, std::size_t i)
    { 
        assert(i >= 0 && i < length());
        switch (i)
        {
        default:
        case (0):
            return v.x;
        case (1):
            return v.y;
        }
    }

};

template <typename T>
struct ng::vector_traits<sf::Vector3<T>>
{
    using vector_type = sf::Vector3<T>;
    using value_type = T;

    static constexpr 
    std::size_t
    length()
    { return 3; }

    static constexpr
    vector_type
    null()
    { return sf::Vector3<T>(); }

    static constexpr
    value_type& get(vector_type& v, std::size_t i)
    { 
        assert(i >= 0 && i < length());
        switch (i)
        {
        default:
        case (0):
            return v.x;
        case (1):
            return v.y;
        case (2):
            return v.z;
        }
    }

    static constexpr
    const value_type& get(const vector_type& v, std::size_t i)
    { 
        assert(i >= 0 && i < length());
        switch (i)
        {
        default:
        case (0):
            return v.x;
        case (1):
            return v.y;
        case (2):
            return v.z;
        }
    }

};


template <>
struct ng::matrix_traits<sf::Glsl::Mat3>
{
    using matrix_type = sf::Glsl::Mat3;
    using value_type = float;
    using col_type    = sf::Glsl::Vec3;
    using row_type    = sf::Glsl::Vec3;

    static constexpr
    std::size_t
    row_count()
    { return 3; }

    static constexpr
    std::size_t
    col_count()
    { return 3; }

    static constexpr
    matrix_type
    null()
    { 
        float data[9] = { };
        return sf::Glsl::Mat3(&data[0]); 
    }

    static constexpr
    value_type& get(matrix_type& v, std::size_t i, std::size_t j)
    { 
        assert(i >= 0 && i < col_count());
        assert(j >= 0 && j < row_count());
        return v.array[i * col_count() + j];
    }

    static constexpr
    const value_type& get(const matrix_type& v, std::size_t i, std::size_t j)
    { 
        assert(i >= 0 && i < col_count());
        assert(j >= 0 && j < row_count());
        return v.array[i * col_count() + j];
    }
    
};


template <>
struct ng::matrix_traits<sf::Glsl::Mat4>
{
    using matrix_type = sf::Glsl::Mat4;
    using value_type  = float;
    using col_type    = sf::Glsl::Vec4;
    using row_type    = sf::Glsl::Vec4;

    static constexpr
    std::size_t
    row_count()
    { return 4; }

    static constexpr
    std::size_t
    col_count()
    { return 4; }

    static constexpr
    matrix_type
    null()
    { 
        float data[16] = { };
        return sf::Glsl::Mat4(&data[0]); 
    }

    static constexpr
    value_type& get(matrix_type& v, std::size_t i, std::size_t j)
    { 
        assert(i >= 0 && i < col_count());
        assert(j >= 0 && j < row_count());
        return v.array[i * col_count() + j];
    }

    static constexpr
    const value_type& get(const matrix_type& v, std::size_t i, std::size_t j)
    { 
        assert(i >= 0 && i < col_count());
        assert(j >= 0 && j < row_count());
        return v.array[i * col_count() + j];
    }
    
};

static_assert(ng::vector_like<glm::vec2>);
static_assert(ng::vector_like<sf::Vector2f>);
static_assert(ng::vector_like<sf::Glsl::Vec2>);
static_assert(ng::matrix_like<glm::mat4>);
static_assert(ng::matrix_like<sf::Glsl::Mat4>);

static_assert(!ng::vector_like<glm::mat4>);
static_assert(!ng::vector_like<sf::Glsl::Mat4>);
static_assert(!ng::matrix_like<glm::vec2>);
static_assert(!ng::matrix_like<sf::Vector2f>);