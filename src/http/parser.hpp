#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP
#include <iostream>
#include <memory>
#include <http/message.hpp>
#include <http/fields.hpp>
#include <http/read.hpp>

namespace http{

template<bool isRequest,class Body,class Fields>
class parser{
	using message_type = message<isRequest, Body, Fields>;
	message_type message_;
	std::unique_ptr<std::string> querystr_ptr;
	bool header_done_;
	bool done_;
	parser_ec ec_;
public:
	parser(): message_(), querystr_ptr(nullptr), done_(false),header_done_(false),ec_(parser_ec::ok) {}
	template<class Emptybody>
	parser( parser<isRequest, Emptybody, Fields> && par)
	: message_(std::move(par.get())), 
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
	read_header_line<>(std::istream &is, parser<isRequest, Body, Fields>& par);
	friend parser_ec
	read_header<>(std::istream &is, parser<isRequest, Body, Fields>& par);
	friend int
	read_body<>(std::istream &is, parser<isRequest, Body, Fields>& par);
	friend int
	read<>(std::istream &is, parser<isRequest, Body, Fields>& par);
};

template<class Body , class Fields = fields>
using request_parser = parser<true, Body, Fields>;

template<class Body , class Fields = fields>
using response_parser = parser<false, Body, Fields>;

}
#endif
