#include <http/body.hpp>
#include <unistd.h>

namespace http {
template<>
int64_t file_body::write(std::ostream& os, file_body const& body){
	
	uint64_t fsize = body.content_.size();
	/*
#ifdef DEBUG
		std::cerr<<"read "<<fsize<<" bytes from"<<" fd: "<<body.content.fd()<<'\n';
#endif
*/
	os.write(body.content_.fileaddr(),fsize);
#ifdef DEBUG
		//std::cerr<<"write "<<fsize<<" bytes to"<<" fd: "<<body.content_.fd()<<'\n';
#endif
	return body.size();
}
template<>
int64_t file_body::read(std::istream& is, file_body& body){
	fdostream fds(body.content_.fd());
	fds<<is.rdbuf();
	return file_body::size(body);
}

}
