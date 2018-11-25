// Copyright Takatoshi Kondo 2018
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(MQTT_MESSAGE_VARIANT_HPP)
#define MQTT_MESSAGE_VARIANT_HPP

#include <mqtt/message.hpp>
#include <mqtt/v5_message.hpp>
#include <mqtt/variant.hpp>

namespace mqtt {

//  message_variant

using message_variant = variant<
    v3_1_1::connect_message,
    v3_1_1::connack_message,
    v3_1_1::publish_message,
    v3_1_1::puback_message,
    v3_1_1::pubrec_message,
    v3_1_1::pubrel_message,
    v3_1_1::pubcomp_message,
    v3_1_1::subscribe_message,
    v3_1_1::suback_message,
    v3_1_1::unsubscribe_message,
    v3_1_1::unsuback_message,
    v3_1_1::pingreq_message,
    v3_1_1::pingresp_message,
    v3_1_1::disconnect_message,
    v5::connect_message,
    v5::connack_message,
    v5::publish_message,
    v5::puback_message,
    v5::pubrec_message,
    v5::pubrel_message,
    v5::pubcomp_message,
    v5::subscribe_message,
    v5::suback_message,
    v5::unsubscribe_message,
    v5::unsuback_message,
    v5::pingreq_message,
    v5::pingresp_message,
    v5::disconnect_message
>;

namespace message {

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

struct continuous_buffer_visitor

#if !defined(MQTT_STD_VARIANT)
    : boost::static_visitor<std::string>
#endif // !defined(MQTT_STD_VARIANT)

{
    template <typename T>
    std::string operator()(T&& t) const {
        return t.continuous_buffer();
    }
};

} // namespace detail

} // namespace message

inline std::vector<as::const_buffer> const_buffer_sequence(message_variant const& mv) {
    return visit(message::detail::const_buffer_sequence_visitor(), mv);
}

inline std::size_t size(message_variant const& mv) {
    return visit(message::detail::size_visitor(), mv);
}

inline std::string continuous_buffer(message_variant const& mv) {
    return visit(message::detail::continuous_buffer_visitor(), mv);
}


//  store_message_variant

using store_message_variant = variant<
    publish_message,
    pubrel_message
>;

namespace detail {

struct message_variant_visitor

#if !defined(MQTT_STD_VARIANT)
    : boost::static_visitor<message_variant>
#endif // !defined(MQTT_STD_VARIANT)

{
    template <typename T>
    message_variant operator()(T&& t) const {
        return t;
    }
};

} // detail

inline message_variant get_message_variant(store_message_variant const& smv) {
    return visit(mqtt::detail::message_variant_visitor(), smv);
}

} // namespace mqtt

#endif // MQTT_MESSAGE_VARIANT_HPP
