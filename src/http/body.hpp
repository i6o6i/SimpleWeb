#ifndef HTTP_BODY_HPP
#define HTTP_BODY_HPP
#include <string>
#include <file.hpp>
#include <vector>
#include <type_traits>

namespace http{
template<class T>
class basic_body {
	T content_;
public:
	typedef typename T::value_type value_type;
	/*
	basic_body(basic_body&) =delete;
	basic_body(basic_body&&) =delete;
	basic_body(T& content):content_(content){}
	basic_body(T&& content):content_(content){}
	*/
	template<class ... ContentArgs>
	basic_body(ContentArgs&&... content_args)
	:content_(std::forward<ContentArgs>(content_args)...)
	{};
	uint64_t size() const { return content_.size(); };
	T& content() { return content_; }
	static uint64_t size(const basic_body<T>& body) { return body.content_.size();}
	// write body to stream
	static int64_t write(std::ostream& os, basic_body<T> const& body);	
	static int64_t read(std::istream& is, basic_body<T>& body);
};

typedef basic_body<file> file_body;
typedef basic_body<std::string> string_body;
}
#endif
