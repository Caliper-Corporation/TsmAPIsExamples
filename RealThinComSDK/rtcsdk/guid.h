#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

namespace rtcsdk {

namespace details {

constexpr const size_t normal_guid_size = 36;	//  XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
constexpr const size_t braced_guid_size = 38;	// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}

constexpr int parse_hex_digit(const char c)
{
    using namespace std::string_literals;

    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return 10 + c - 'a';
    else if ('A' <= c && c <= 'F')
        return 10 + c - 'A';
    else
        throw std::domain_error{ "Invalid character in GUID"s };
}

template<typename T>
constexpr T parse_hex(const char* ptr)
{
    constexpr size_t digits = sizeof(T) * 2;
    T result{};

    for (size_t i = 0; i < digits; ++i) {
        result |= parse_hex_digit(ptr[i]) << (4 * (digits - i - 1));
    }

    return result;
}

constexpr GUID make_guid_helper(const char* begin)
{
    GUID result{};
    result.Data1 = parse_hex<uint32_t>(begin);
    begin += 8 + 1;
    result.Data2 = parse_hex<uint16_t>(begin);
    begin += 4 + 1;
    result.Data3 = parse_hex<uint16_t>(begin);
    begin += 4 + 1;
    result.Data4[0] = parse_hex<uint8_t>(begin);
    begin += 2;
    result.Data4[1] = parse_hex<uint8_t>(begin);
    begin += 2 + 1;

    for (size_t i = 0; i < 6; ++i) {
        result.Data4[i + 2] = parse_hex<uint8_t>(begin + i * 2);
    }

    return result;
}

template<size_t N>
constexpr GUID make_guid(const char(&str)[N])
{
    using namespace std::string_literals;
    static_assert(N == (braced_guid_size + 1) || N == (normal_guid_size + 1),
        "String GUID of form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX expected");

    if constexpr (N == (braced_guid_size + 1)) {
        if (str[0] != '{' || str[braced_guid_size - 1] != '}') {
            throw std::domain_error{ "Missing opening or closing brace"s };
        }
    }

    return make_guid_helper(str + (N == (braced_guid_size + 1) ? 1 : 0));
}

//
template<typename T, typename>
struct has_get_guid_impl : std::false_type
{
};

template<typename T>
struct has_get_guid_impl<T, decltype(void(T::get_guid()))> : std::true_type
{
};

template<typename T>
using has_get_guid = has_get_guid_impl<T, void>;

template<typename T, typename>
struct has_free_get_guid_impl : std::false_type
{
};

template<typename T>
struct has_free_get_guid_impl<T, decltype(void(get_guid(std::declval<T*>())))> : std::true_type
{
};

template<typename T>
using has_free_get_guid = has_free_get_guid_impl<T, void>;

//
template<typename T>
struct interface_wrapper
{
    using type = T;
};

template<typename T>
constexpr const auto msvc_get_guid_workaround = T::get_guid();

//
template<typename T>
constexpr GUID get_interface_guid_impl(std::true_type, std::false_type)
{
    return msvc_get_guid_workaround<T>;
}

template<typename T, typename Any>
constexpr GUID get_interface_guid_impl(Any, std::true_type) noexcept
{
    return get_guid(static_cast<T*>(nullptr));
}

template<typename T>
constexpr GUID get_interface_guid_impl(std::false_type, std::false_type) noexcept
{
    return __uuidof(T);
}

//
template<typename T>
constexpr GUID get_interface_guid(T);

template<typename Interface>
constexpr GUID get_interface_guid(interface_wrapper<Interface>) noexcept
{
    return get_interface_guid_impl<Interface>(has_get_guid<Interface>{}, has_free_get_guid<Interface>{});
}

} // end of namespace rtcsdk::details

using details::make_guid;

namespace literals {

constexpr GUID operator "" _guid(const char* str, size_t N)
{
    using namespace details;
    using namespace std::string_literals;

    if (!(N == normal_guid_size || N == braced_guid_size)) {
        throw std::domain_error{
            "String GUID of the form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX is expected"s
        };
    }

    if (N == braced_guid_size && (str[0] != '{' || str[braced_guid_size - 1] != '}')) {
        throw std::domain_error{ "Missing opening or closing brace"s };
    }
    // Skip the first opening brace if this is a braced guid.
    return details::make_guid_helper(str + (N == braced_guid_size ? 1 : 0));
}

} // end of namespace rtcsdk::literals

} // end of namespace rtcsdk

using namespace rtcsdk::literals;
