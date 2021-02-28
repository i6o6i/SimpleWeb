#ifndef EMPTY_BODY_HPP
#define EMPTY_BODY_HPP
#include <http/body.hpp>
namespace http {
struct empty_base {
};
typedef basic_body<empty_base> empty_body;
template<>
class basic_body<empty_base> {
public:
    static uint64_t size(empty_base const& body);
	// write body to stream
	static int64_t write(std::ostream& os, basic_body<empty_base> const& body) ;
	static int64_t read(std::istream& is, basic_body<empty_base>& body) ;
};
}
#endif
