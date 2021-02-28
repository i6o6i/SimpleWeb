#ifndef VECTOR_BODY_HPP
#define VECTOR_BODY_HPP
#include <http/body.hpp>
namespace http {
struct reqargs {
	std::string key;
	std::string value;
};
template<typename T>
using vector_body=basic_body<std::vector<T>>;
using post_params = std::vector<reqargs>;

template<class T>
class basic_body<std::vector<T>> {
	std::vector<T> content_;
public:
	typedef T value_type;
    uint64_t size();
	// write body to stream
	static int64_t write(std::ostream& os, basic_body<std::vector<T>> const& body) ;
	static int64_t read(std::istream& is, basic_body<std::vector<T>>& body) ;
};
}
#endif
