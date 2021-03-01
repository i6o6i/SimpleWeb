#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP
#include <iostream>
#include <memory>
#include <http/message.hpp>
#include <http/fields.hpp>
#include <http/read.hpp>

namespace http{

template<bool isRequest,class BodyHandler,class Fields>
class parser{
	using message_type = message<isRequest, typename BodyHandler::value_type, Fields>;
	message_type message_;
	std::unique_ptr<std::string> querystr_ptr;
	bool header_done_;
	bool done_;
	parser_ec ec_;
public:
	parser(): message_(), querystr_ptr(nullptr), done_(false),header_done_(false),ec_(parser_ec::ok) {}
	template<class Emptybody, class... BodyArgs>
	parser( parser<isRequest, Emptybody, Fields> && par, BodyArgs &&... body_args)
	: message_(std::move(par.get()), std::forward<BodyArgs>(body_args)...), 
	querystr_ptr(std::move(par.querystr())),
	header_done_(par.is_header_done()),
	done_(par.is_done()), 
	ec_(par.ec()) {}
	message_type &get() { return message_; }
	std::unique_ptr<std::string> &querystr() { return querystr_ptr; }
	bool is_header_done() { return header_done_; }
	bool is_done() { return done_; }
	parser_ec ec() { return ec_; }
	friend parser_ec
	read_header_line<>(std::istream &is, parser<isRequest, BodyHandler, Fields>& par);
	friend parser_ec
	read_header<>(std::istream &is, parser<isRequest, BodyHandler, Fields>& par);
	friend int
	read_body<>(std::istream &is, parser<isRequest, BodyHandler, Fields>& par);
	friend int
	read<>(std::istream &is, parser<isRequest, BodyHandler, Fields>& par);
};

template<class BodyHandler , class Fields = fields>
using request_parser = parser<true, BodyHandler, Fields>;

template<class BodyHandler , class Fields = fields>
using response_parser = parser<false, BodyHandler, Fields>;

}
#endif
