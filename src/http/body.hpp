#ifndef HTTP_BODY_HPP
#define HTTP_BODY_HPP
#include <string>
#include <memory>
#include <vector>
#include <type_traits>
#include <file.hpp>

namespace http{

template<typename T>
struct basic_body
{
	T& content_;
	typedef T value_type;
	/*
	basic_body(basic_body&) =delete;
	basic_body(basic_body&&) =delete;
	basic_body(T& content):content_(content){}
	basic_body(T&& content):content_(content){}
	*/
	T& Body() { return content_; }
	uint64_t size() const { return content_.size(); };
	T& get() { return content_; }
	static uint64_t size(const basic_body<T>& body) { return body.content_.size();}
	// write body to stream
	static int64_t write(std::ostream& os, T const& body);	
	static int64_t read(std::istream& is, T& body);
};

typedef basic_body<std::string> string_body;

//typedef basic_body<file> file_body;
}
#endif
