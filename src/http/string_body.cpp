#include <http/body.hpp>
namespace http {
template<>
int64_t string_body::write(std::ostream& os, string_body const& body) {
	os<<body.content_;
	return body.size();
}
template<>
int64_t string_body::read(std::istream& is, string_body& body) {
	is>>body.content_;
	return body.size();
}
}
