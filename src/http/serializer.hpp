#ifndef HTTP_SERIALIZER_HPP
#define HTTP_SERIALIZER_HPP
#include <iostream>
#include <http/message.hpp>
#include <http/fields.hpp>
#include <http/write.hpp>

namespace http {

template<bool isRequest, class Body, class Fields >
class serializer {
	using message_type = message<isRequest, Body, Fields>;
	message_type message_;
	bool done_;
	bool header_done_;
public:
	serializer(message_type&& m): message_(std::move(m)) {}
	message_type &get() { return message_; }
	friend uint64_t
	write_header<>(std::ostream &os, serializer<isRequest, Body, Fields>& sr);
	friend uint64_t
	write<>(std::ostream &os, serializer<isRequest, Body, Fields>& sr);
};

template<class Body , class Fields = fields>
using request_serializer = serializer<true, Body, Fields>;

template<class Body , class Fields = fields>
using response_serializer = serializer<false, Body, Fields>;
}
#endif
