#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP
#include <utility>
#include <type_traits>
#include <iostream>
#include <http/fields.hpp>
#include <http/status.hpp>
#include <http/body.hpp>

namespace http{
enum class verb{
	unknown = 0,
	delete_,
	get,
	head,
	post,
	put
};
inline std::string to_string(verb v){
	static std::string methodTable[6]={
		"UNKNOWN",
		"DELETE",
		"GET",
		"HEAD",
		"POST",
		"PUT",
	};
	return methodTable[static_cast<int>(v)];
}

template<bool isRequest, class Fields = fields>
class header;

template<class Fields>
class header<true, Fields> :public Fields{
public:
	void method(const verb &m){ method_=m; }
	int method(std::string &m){
		switch(m[0]){
			case 'D': method_=verb::delete_;break;
			case 'G': method_=verb::get;break;
			case 'H': method_=verb::head;break;
			case 'P': method_=m[1]=='O'?verb::post
					  :verb::put;break;
			default: method_=verb::unknown;
		}
		if(method_!=verb::unknown&&to_string(method_)!=m){
			method_=verb::unknown;
			return -1;
		} else return 0;
	}
	verb method() { return method_; }
	std::string method_string() { return to_string(method_); }
	void target(const std::string & t) { target_=t; }
	const std::string& target() { return target_; }
	void version(unsigned v) { version_=v; }
	unsigned version() { return version_; }
	std::string version_string() {
		switch(version_){
			case 11: return "HTTP/1.1";
			case 10: return "HTTP/1.0";
			default: return "unknown";
		}
	}
	header(
			verb method,
			const std::string& target_,
			unsigned version_,
			const std::initializer_list<typename Fields::value_type>&& fields
		  ): Fields(std::move(fields)),method_(method)
	{
		version(version_);
		target(target_);
	}
	header() = default;
private:
	verb method_;
	unsigned version_;
	std::string target_;
};

template<class Fields>
class header<false, Fields>:public Fields{
public:
	void result(status s){ status_=s; }
	status result(){ return status_; }
	unsigned result_int() { return static_cast<int>(status_); }
	const std::string reason() { return to_string(status_); }
	header() = default;
	void version(unsigned v) { version_=v; }
	unsigned version() { return version_; }
	std::string version_string() {
		switch(version_){
			case 11: return "HTTP/1.1";
			case 10: return "HTTP/1.0";
			default: return "unknown";
		}
	}
	template<class... FieldsArgs>
	header(
			status status,
			const std::initializer_list<typename Fields::value_type>&& fields,
			unsigned version
		  ): Fields(std::move(fields)),status_(status),version_(version)
	{ }
private:
	status status_;
	unsigned version_;
};
using request_header = header<true>;
using response_header = header<false>;

template<bool isRequest, class Body, class Fields = fields>
class message: public header<isRequest, Fields>, public Body{
public:
	using header_type = header<isRequest, Fields>;
	//using body_type = typename Body::value_type;
	message() = default;
	template<class U = Body>
	message(header_type &&h, typename std::enable_if<std::is_empty<U>::value>::type* = 0): header_type(h) {}
	template<class U>
	message(message<isRequest,U>&& m):header_type(m){}
	template<class... BodyArgs>
	message(header_type &&h, BodyArgs&&... body_args)
	:header_type(h), Body(std::forward<BodyArgs>(body_args)...){
	}
	/*
	template<class... BodyArgs>
	message(header_type &h, BodyArgs &... body_args)
	//std::forward will remove const
	:header_type(h), Body(std::forward<BodyArgs>(body_args)...){
	}
	*/
	//constructor for other body message
	template<class OtherBody, class... BodyArgs>
	message(message<isRequest, OtherBody> && m, BodyArgs &&... body_args)
	:header_type(m), Body(std::forward<BodyArgs>(body_args)...){
	}
};
template<class Body , class Fields = fields>
using request_message = message<true,Body,Fields>;

template<class Body , class Fields = fields>
using response_message = message<false,Body,Fields>;
}
#endif
