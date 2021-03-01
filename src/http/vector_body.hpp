#ifndef VECTOR_BODY_HPP
#define VECTOR_BODY_HPP
#include <http/body.hpp>
#include <vector>
#include <string>
namespace http {
struct reqargs {
	std::string key;
	std::string value;
};
//template<typename T>
//using vector_body=basic_body<std::vector<T>>;
using post_params = std::vector<reqargs>;

template<class T>
struct vector_body{
	typedef std::vector<T> value_type;
	value_type& body;
	value_type& Body() {return body;}
    static uint64_t size(const value_type & body);
	// write body to stream
	static int64_t write(std::ostream& os, value_type const& body) ;
	static int64_t read(std::istream& is, value_type & body) ;
};

}
#endif
