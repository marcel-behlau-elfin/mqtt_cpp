// Copyright Takatoshi Kondo 2018
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(MQTT_PROPERTY_VARIANT_HPP)
#define MQTT_PROPERTY_VARIANT_HPP

#include <mqtt/property.hpp>
#include <mqtt/variant.hpp>

namespace mqtt {

namespace v5 {
//  property_variant

using property_variant = variant<
    property::payload_format_indicator,
    property::user_property
>;

namespace property {

namespace detail {

struct const_buffer_sequence_visitor

#if !defined(MQTT_STD_VARIANT)
    : boost::static_visitor<std::vector<as::const_buffer>>
#endif // !defined(MQTT_STD_VARIANT)

{
    template <typename T>
    std::vector<as::const_buffer> operator()(T&& t) const {
        return t.const_buffer_sequence();
    }
};

struct size_visitor

#if !defined(MQTT_STD_VARIANT)
    : boost::static_visitor<std::size_t>
#endif // !defined(MQTT_STD_VARIANT)

{
    template <typename T>
    std::size_t operator()(T&& t) const {
        return t.size();
    }
};

template <typename Iterator>
struct fill_visitor

#if !defined(MQTT_STD_VARIANT)
    : boost::static_visitor<std::string>
#endif // !defined(MQTT_STD_VARIANT)

{
    fill_visitor(Iterator b, Iterator e):b(b), e(e) {}

    template <typename T>
    std::string operator()(T&& t) {
        return t.fill(b, e);
    }

    Iterator b;
    Iterator e;
};

template <typename Iterator>
inline fill_visitor<Iterator> make_fill_visitor(Iterator b, Iterator e) {
    return fill_visitor<Iterator>(b, e);
}

} // namespace detail

} // namespace property

inline std::vector<as::const_buffer> const_buffer_sequence(property_variant const& pv) {
    return visit(property::detail::const_buffer_sequence_visitor(), pv);
}

inline std::size_t size(property_variant const& pv) {
    return visit(property::detail::size_visitor(), pv);
}

template <typename Iterator>
inline std::string fill(property_variant& pv, Iterator b, Iterator e) {
    return visit(property::detail::make_fill_visitor(b, e), pv);
}

} // namespace v5

} // namespace mqtt

#endif // MQTT_PROPERTY_VARIANT_HPP
