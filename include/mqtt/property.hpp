// Copyright Takatoshi Kondo 2018
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(MQTT_PROPERTY_HPP)
#define MQTT_PROPERTY_HPP

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <boost/asio/buffer.hpp>
#include <boost/optional.hpp>
#include <boost/container/static_vector.hpp>

#include <mqtt/two_byte_util.hpp>
#include <mqtt/fixed_header.hpp>
#include <mqtt/remaining_length.hpp>
#include <mqtt/qos.hpp>
#include <mqtt/const_buffer_util.hpp>
#include <mqtt/will.hpp>
#include <mqtt/connect_flags.hpp>
#include <mqtt/publish.hpp>
#include <mqtt/exception.hpp>
#include <mqtt/string_check.hpp>
#include <mqtt/property_id.hpp>
#include <mqtt/four_byte_util.hpp>
#include <mqtt/utf8encoded_strings.hpp>

namespace mqtt {

namespace as = boost::asio;

namespace v5 {

namespace property {

namespace detail {

template <std::size_t N>
struct n_bytes_property {
    explicit n_bytes_property(std::uint8_t id)
        :id_(static_cast<char>(id)) {}
    template <typename It>
    n_bytes_property(std::uint8_t id, It b, It e)
        :id_(static_cast<char>(id)), buf_(b, e) {}
    n_bytes_property(std::uint8_t id, std::initializer_list<char> il)
        :id_(static_cast<char>(id)), buf_(std::move(il)) {}

    /**
     * @brief Create const buffer sequence
     *        it is for boost asio APIs
     * @return const buffer sequence
     */
    std::vector<as::const_buffer> const_buffer_sequence() const {
        return
            {
                as::buffer(&id_, 1),
                as::buffer(buf_.data(), buf_.size())
            };
    }

    /**
     * @brief Copy the internal information to the range between b and e
     *        it is for boost asio APIs
     * @param b begin of the range to fill
     * @param e end of the range to fill
     */
    template <typename It>
    void fill(It b, It e) const {
        BOOST_ASSERT(static_cast<std::size_t>(std::distance(b, e)) >= size());
        *b++ = id_;
        std::copy(buf_.begin(), buf_.end(), b);
    }

    /**
     * @brief Get whole size of sequence
     * @return whole size
     */
    std::size_t size() const {
        return 1 + buf_.size();
    }

    char const id_;
    boost::container::static_vector<char, N> buf_;
};

struct string_property {
    string_property(std::uint8_t id, string_view sv)
        :id_(static_cast<char>(id)),
         length_{MQTT_16BITNUM_TO_BYTE_SEQ(sv.size())},
         buf_(sv.begin(), sv.end()) {
             if (utf8string::is_valid_length(sv)) throw utf8string_length_error();
             auto r = utf8string::validate_contents(sv);
             if (r != utf8string::validation::well_formed) throw utf8string_contents_error(r);
         }

    /**
     * @brief Create const buffer sequence
     *        it is for boost asio APIs
     * @return const buffer sequence
     */
    std::vector<as::const_buffer> const_buffer_sequence() const {
        return
            {
                as::buffer(&id_, 1),
                as::buffer(length_.data(), length_.size()),
                as::buffer(buf_.data(), buf_.size())
            };
    }

    /**
     * @brief Copy the internal information to the range between b and e
     *        it is for boost asio APIs
     * @param b begin of the range to fill
     * @param e end of the range to fill
     */
    template <typename It>
    void fill(It b, It e) const {
        BOOST_ASSERT(static_cast<std::size_t>(std::distance(b, e)) >= size());
        *b++ = id_;
        std::copy(length_.begin(), length_.end(), b);
        b += length_.size();
        std::copy(buf_.begin(), buf_.end(), b);
    }

    /**
     * @brief Get whole size of sequence
     * @return whole size
     */
    std::size_t size() const {
        return 1 + length_.size() + buf_.size();
    }

    char const id_;
    boost::container::static_vector<char, 2> length_;
    std::string buf_;
};

struct string_property_ref {
    string_property_ref(std::uint8_t id, string_view sv)
        :id_(static_cast<char>(id)),
         length_{MQTT_16BITNUM_TO_BYTE_SEQ(sv.size())},
         buf_(sv.data(), sv.size()) {
             if (utf8string::is_valid_length(sv)) throw utf8string_length_error();
             auto r = utf8string::validate_contents(sv);
             if (r != utf8string::validation::well_formed) throw utf8string_contents_error(r);
         }

    /**
     * @brief Create const buffer sequence
     *        it is for boost asio APIs
     * @return const buffer sequence
     */
    std::vector<as::const_buffer> const_buffer_sequence() const {
        return
            {
                as::buffer(&id_, 1),
                as::buffer(length_.data(), length_.size()),
                as::buffer(get_pointer(buf_), get_size(buf_))
            };
    }

    /**
     * @brief Copy the internal information to the range between b and e
     *        it is for boost asio APIs
     * @param b begin of the range to fill
     * @param e end of the range to fill
     */
    template <typename It>
    void fill(It b, It e) const {
        BOOST_ASSERT(static_cast<std::size_t>(std::distance(b, e)) >= size());
        *b++ = id_;
        std::copy(length_.begin(), length_.end(), b);
        b += length_.size();
        std::copy(get_pointer(buf_), get_pointer(buf_) + get_size(buf_), b);
    }

    /**
     * @brief Get whole size of sequence
     * @return whole size
     */
    std::size_t size() const {
        return 1 + length_.size() + get_size(buf_);
    }

    char const id_;
    boost::container::static_vector<char, 2> length_;
    as::const_buffer buf_;
};

} // namespace detail

class payload_format_indicator : public detail::n_bytes_property<1> {
public:
    enum payload_format {
        binary,
        string
    };

    payload_format_indicator(bool binary = true)
        : detail::n_bytes_property<1>(id::payload_format_indicator, { binary ? char(0) : char(1) } ) {}

    template <typename It>
    payload_format_indicator(It& b, It e)
        : detail::n_bytes_property<1>(id::payload_format_indicator) {
        if (b == e) throw property_parse_error();
        if (*b != 0 && *b != 1) throw property_parse_error();
        buf_[0] = *b;
        ++b;
    }

    payload_format payload_format() const {
        return
            [this] {
                if (buf_[0] == 0) return binary;
                else return string;
            }();
    }

};

class message_expiry_interval : public detail::n_bytes_property<4> {
public:
    message_expiry_interval(std::uint32_t val)
        : detail::n_bytes_property<4>(id::message_expiry_interval, { MQTT_32BITNUM_TO_BYTE_SEQ(val) } ) {}
    std::uint32_t val() const {
        return make_uint32_t(buf_.begin(), buf_.end());
    }
};

class content_type : public detail::string_property {
public:
    content_type(string_view type)
        : detail::string_property(id::message_expiry_interval, type) {}
    std::uint32_t val() const {
        return make_uint32_t(buf_.begin(), buf_.end());
    }
};

class user_property {
    struct len_str {
        explicit len_str(string_view v)
            : len{MQTT_16BITNUM_TO_BYTE_SEQ(v.size())}
            , str(as::buffer(v.data(), v.size()))
        {}
        explicit len_str(as::const_buffer v)
            : len{MQTT_16BITNUM_TO_BYTE_SEQ(get_size(v))}
            , str(v)
        {}
        std::size_t size() const {
            return len.size() + get_size(str);
        }
        boost::container::static_vector<char, 2> len;
        as::const_buffer str;
    };
public:
    user_property(string_view key, string_view val)
        : key_(key), val_(val) {}

    /**
     * @brief Create const buffer sequence
     *        it is for boost asio APIs
     * @return const buffer sequence
     */
    std::vector<as::const_buffer> const_buffer_sequence() const {
        std::vector<as::const_buffer> ret;
        ret.reserve(
            1 + // header
            2 + // key (len, str)
            2   // val (len, str)
        );

        ret.emplace_back(as::buffer(&id_, 1));
        ret.emplace_back(as::buffer(key_.len.data(), key_.len.size()));
        ret.emplace_back(key_.str);
        ret.emplace_back(as::buffer(val_.len.data(), val_.len.size()));
        ret.emplace_back(val_.str);

        return ret;
    }

    template <typename It>
    void fill(It b, It e) const {
        BOOST_ASSERT(static_cast<std::size_t>(std::distance(b, e)) >= size());

        *b++ = id_;
        {
            std::copy(key_.len.begin(), key_.len.end(), b);
            b += key_.len.size();
            auto ptr = get_pointer(key_.str);
            auto size = get_size(key_.str);
            std::copy(ptr, ptr + size, b);
            b += size;
        }
        {
            std::copy(val_.len.begin(), val_.len.end(), b);
            b += val_.len.size();
            auto ptr = get_pointer(val_.str);
            auto size = get_size(val_.str);
            std::copy(ptr, ptr + size, b);
            b += size;
        }
    }

    /**
     * @brief Get whole size of sequence
     * @return whole size
     */
    std::size_t size() const {
        return
            1 + // id_
            key_.size() +
            val_.size();
    }

private:
    char const id_ = id::user_property;
    len_str key_;
    len_str val_;
};

} // namespace property
} // namespace v5
} // namespace mqtt

#endif // MQTT_PROPERTY_HPP
