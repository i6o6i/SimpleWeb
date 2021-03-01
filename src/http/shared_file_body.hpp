#ifndef SHARED_FILE_BODY
#define SHARED_FILE_BODY
#include <http/body.hpp>
#include <file.hpp>
#include <http/file_body.hpp>

namespace http{
template<>
struct basic_body<std::shared_ptr<file>> {
	std::shared_ptr<file>& body;
	typedef typename std::shared_ptr<file> value_type;
	std::shared_ptr<file>& Body() { return body; }
	basic_body(std::shared_ptr<file>& f): body(f) {}
	uint64_t size() const { return body->size(); };
	//T& content() { return *content_; }
	static uint64_t size(const std::shared_ptr<file>& b) { return b->size();}
	// write body to stream
	static int64_t write(std::ostream& os, std::shared_ptr<file> const& b) {
#ifdef DEBUG
		std::cerr<<"invoke write<shared_ptr<file>>()\n";
#endif
		return file_body::write(os, *(b));
	}	
	static int64_t read(std::istream& is, std::shared_ptr<file>& b) {
		std::cerr<<"invoke basic_body<std::shared_ptr<file>> read\n";
		return file_body::read(is, *(b));
;
	}
};
typedef basic_body<std::shared_ptr<file>> shared_file_body;
}
#endif
