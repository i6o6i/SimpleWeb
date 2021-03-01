#ifndef FILE_BODY_HPP
#define FILE_BODY_HPP
//#include <http/body.hpp>
#include <file.hpp>

namespace http {
struct file_body
{
	file& file_;
	file& Body() { return file_; }
	template<class ... ContentArgs>
	file_body(ContentArgs&&... content_args)
	:file_(std::forward<ContentArgs>(content_args)...)
	{};
	static uint64_t size(file const& f) { return f.size();}
	// write body to stream
	static int64_t write(std::ostream& os, file const& body);	
	static int64_t read(std::istream& is, file& body);
};
}
#endif
