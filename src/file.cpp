#include <file.hpp>
#include <sys/mman.h>

file::file():fd_(-1),fsize(0){}
file::file(int fd, const char* path):fd_(fd) {
	struct stat st;
	::stat(path,&st);
	fsize = st.st_size;
	fileaddr_ = (char *)mmap(nullptr, fsize, PROT_READ, MAP_SHARED, fd_, 0);
}
file::file(file&& f) {
	fd_ = f.fd_;
	f.fd_ = -1;
	fileaddr_ = f.fileaddr_;
	fsize = f.fsize;
}
file& file::operator=(file&& f) {
	fd_ = f.fd_;
	f.fd_ = -1;
	fileaddr_ = f.fileaddr_;
	fsize = f.fsize;
	return *this;
}
void file::openfile(const char* path, std::ios_base::openmode m) {
	int flag = 0;
	switch((int)m){
		case (std::ios_base::in |std::ios_base::out): flag = O_RDWR; break;
		case std::ios_base::in : flag = O_RDONLY; break;
		case std::ios_base::out : flag = O_WRONLY; break;
		default:break;
	}

	fd_ = open(path,flag);
#ifdef DEBUG
	std::cerr<<"open file: "<<path<<" fd:"<<fd_<<std::endl;
#endif
	if( fd_ < 0 ) {
		std::cerr<<::strerror(errno);
	}
	struct stat st;
	::stat(path,&st);
	fsize = st.st_size;
	fileaddr_ = (char *)mmap(nullptr, fsize, PROT_READ, MAP_SHARED, fd_, 0);
	if(fileaddr_ == MAP_FAILED ) {
		std::cerr <<"mmap\n";
		std::cerr<<::strerror(errno);
	}
}

file::file(const char* fstr, std::ios_base::openmode m){
	openfile(fstr,m);
}
file::file(const std::string& fstr, std::ios_base::openmode m){
	openfile(fstr.c_str(),m);
}

int file::close(){
	return ::close(fd_);
}
file::~file() {
	if(fd_ != -1) {
		if(munmap(fileaddr_,fsize) < 0)
			std::cerr<<"unmap failed for file fd:"<<fd_;
		close();
	}
}

uint64_t file::size() const{
	return fsize;
}
int file::fd() const{
	return fd_;
}
