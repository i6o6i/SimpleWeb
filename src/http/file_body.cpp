#include <http/file_body.hpp>
#include <unistd.h>

namespace http {
int64_t file_body::write(std::ostream& os, file const& f){
	/*
#ifdef DEBUG
		std::cerr<<"read "<<fsize<<" bytes from"<<" fd: "<<body.content.fd()<<'\n';
#endif
*/
	int nums=f.size();
	int res=0;
	uint64_t totalwrited = 0;
	while(totalwrited < f.size()) {
		if((res = os.rdbuf()->sputn(f.fileaddr()+totalwrited,nums) )< 0) {
			std::cerr<<::strerror(errno)<<'\n'; 
			if(errno == EFAULT )  {
				std::cerr<<"EFAULT\n";
				break;
			}
			if(nums > 4096 ) {
				nums =nums>>1;
#ifdef DEBUG
			std::cerr<<"nums shrink\n";
#endif
			}
		}else {
			totalwrited+=res;
			nums=nums<<1;
		}
	}
#ifdef DEBUG
		//std::cerr<<"write "<<fsize<<" bytes to"<<" fd: "<<body.content_.fd()<<'\n';
#endif
	return f.size();

}

int64_t file_body::read(std::istream& is, file& f){
	fdostream fds(f.fd());
	fds<<is.rdbuf();
	return f.size();
}
}
