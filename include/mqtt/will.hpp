// Copyright Takatoshi Kondo 2015
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(MQTT_WILL_HPP)
#define MQTT_WILL_HPP

#include <string>

#include <mqtt/namespace.hpp>

#include <mqtt/move.hpp>
#include <mqtt/publish.hpp>
#include <mqtt/property_variant.hpp>

namespace MQTT_NS {

class will {
public:
    /**
     * @brief constructor
     * @param topic
     *        A topic name to publish as a will
     * @param message
     *        The contents to publish as a will
     * @param pubopts
     *        Qos and retain flag
     * @param qos
     *        qos
     */
    will(buffer topic,
         buffer message,
         publish_options pubopts = {},
         v5::properties props = {})
        :topic_(force_move(topic)),
         message_(force_move(message)),
         pubopts_(pubopts),
         props_(force_move(props))
    {}

    buffer const& topic() const {
        return topic_;
    }
    buffer& topic() {
        return topic_;
    }
    buffer const& message() const {
        return message_;
    }
    buffer& message() {
        return message_;
    }
    retain get_retain() const {
        return pubopts_.get_retain();
    }
    qos get_qos() const {
        return pubopts_.get_qos();
    }
    v5::properties const& props() const {
        return props_;
    }
    v5::properties& props() {
        return props_;
    }

private:
    buffer topic_;
    buffer message_;
    publish_options pubopts_;
    v5::properties props_;
};

} // namespace MQTT_NS

#endif // MQTT_WILL_HPP
