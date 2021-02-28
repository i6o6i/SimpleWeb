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
	//os.write(body.content_.fileaddr(),fsize);
	int nums=1024;
	uint64_t writednums = 0;
	while(writednums < fsize) {
		if(os.rdbuf()->sputn(body.content_.fileaddr()+writednums,nums) < 0) {
#ifdef DEBUG
			std::cerr<<::strerror(errno)<<'\n'; 
			std::cerr<<writednums<<'\n'; 
#endif
			if(errno == EFAULT ) break;
			if(nums > 1024 ) {
				nums =nums>>1;
			std::cerr<<"nums shrink\n";
			}
		}else {
			writednums+=nums;
			nums=nums<<1;
		}
	}
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
