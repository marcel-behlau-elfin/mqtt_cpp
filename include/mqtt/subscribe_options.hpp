// Copyright Takatoshi Kondo 2015
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(MQTT_SUBSCRIBE_OPTIONS_HPP)
#define MQTT_SUBSCRIBE_OPTIONS_HPP

#include <cstdint>
#include <mqtt/namespace.hpp>

namespace MQTT_NS {

enum class retain_handling : std::uint8_t
{
    send = 0b00000000,
    send_only_new_subscription = 0b00010000,
    not_send = 0b00100000,
};
enum class rap : std::uint8_t
{
    dont = 0b00000000,
    retain = 0b00001000,
};
enum class nl : std::uint8_t
{
    no = 0b00000000,
    yes = 0b00000100,
};
enum class qos : std::uint8_t
{
    at_most_once = 0b00000000,
    at_least_once = 0b00000001,
    exactly_once = 0b00000010,
};

struct subscribe_options
{
    constexpr subscribe_options(void) = delete;
    ~subscribe_options(void) = default;
    constexpr subscribe_options(subscribe_options &&) = default;
    constexpr subscribe_options(subscribe_options const&) = default;
    constexpr subscribe_options& operator=(subscribe_options &&) = default;
    constexpr subscribe_options& operator=(subscribe_options const&) = default;

    explicit constexpr subscribe_options(std::uint8_t value) : data_(value) { }

    constexpr subscribe_options(retain_handling value) : data_(static_cast<std::uint8_t>(value)) { }
    constexpr subscribe_options(rap value) : data_(static_cast<std::uint8_t>(value)) { }
    constexpr subscribe_options(nl value) : data_(static_cast<std::uint8_t>(value)) { }
    constexpr subscribe_options(qos value) : data_(static_cast<std::uint8_t>(value)) { }

    constexpr subscribe_options& operator|=(retain_handling value) { data_ |= static_cast<std::uint8_t>(value); return *this; }
    constexpr subscribe_options& operator|=(rap value) { data_ |= static_cast<std::uint8_t>(value); return *this; }
    constexpr subscribe_options& operator|=(nl value) { data_ |= static_cast<std::uint8_t>(value); return *this; }
    constexpr subscribe_options& operator|=(qos value) { data_ |= static_cast<std::uint8_t>(value); return *this; }

    constexpr retain_handling get_retain_handling() const
    { return static_cast<retain_handling>(data_ & 0b00110000); }
    constexpr rap get_rap() const
    { return static_cast<rap>(data_ & 0b00001000); }
    constexpr nl get_nl() const
    { return static_cast<nl>(data_ & 0b00000100); }
    constexpr qos get_qos() const
    { return static_cast<qos>(data_ & 0b00000011); }

    explicit constexpr operator std::uint8_t() const { return data_; }
private:
    std::uint8_t data_;
};

constexpr subscribe_options operator|(subscribe_options lhs, retain_handling rhs) { lhs |= rhs; return lhs; }
constexpr subscribe_options operator|(subscribe_options lhs, rap rhs) { lhs |= rhs; return lhs; }
constexpr subscribe_options operator|(subscribe_options lhs, nl rhs) { lhs |= rhs; return lhs; }
constexpr subscribe_options operator|(subscribe_options lhs, qos rhs) { lhs |= rhs; return lhs; }

constexpr subscribe_options operator|(retain_handling lhs, rap rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(retain_handling lhs, nl rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(retain_handling lhs, qos rhs) { return subscribe_options(lhs) | rhs; }

constexpr subscribe_options operator|(rap lhs, retain_handling rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(rap lhs, nl rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(rap lhs, qos rhs) { return subscribe_options(lhs) | rhs; }

constexpr subscribe_options operator|(nl lhs, retain_handling rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(nl lhs, rap rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(nl lhs, qos rhs) { return subscribe_options(lhs) | rhs; }

constexpr subscribe_options operator|(qos lhs, retain_handling rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(qos lhs, rap rhs) { return subscribe_options(lhs) | rhs; }
constexpr subscribe_options operator|(qos lhs, nl rhs) { return subscribe_options(lhs) | rhs; }

constexpr char const* qos_to_str(qos v) {
    char const * const str[] = {
        "at_most_once",
        "at_least_once",
        "exactly_once"
    };
    if (static_cast<std::uint8_t>(v) < sizeof(str)) return str[static_cast<std::uint8_t>(v)];
    return "invalid_qos";
}

template<typename Stream>
Stream & operator<<(Stream & os, qos val)
{
    os << qos_to_str(val);
    return os;
}

} // namespace MQTT_NS

#endif // MQTT_SUBSCRIBE_OPTIONS_HPP
