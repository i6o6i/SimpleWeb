#include <http/body.hpp>
namespace http {
template<>
int64_t string_body::write(std::ostream& os, std::string const& body) {
	os<<body;
	return body.size();
}
template<>
int64_t string_body::read(std::istream& is, std::string& body) {
	is>>body;
	return body.size();
}
}
