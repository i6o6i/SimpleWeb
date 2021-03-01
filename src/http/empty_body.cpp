#include <http/empty_body.hpp>
namespace http {
uint64_t empty_body::size(empty_base const& b){
	return 0;
}

int64_t empty_body::write(std::ostream& os, empty_base const& f){
	return 0;
}

int64_t empty_body::read(std::istream& is, empty_base& f){
	return 0;
}
}
