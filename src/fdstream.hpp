#ifndef FDSTREAM_HPP
#define FDSTREAM_HPP
#include <istream>
#include <errno.h>
#include <ostream>
#include <streambuf>
#include <cstdio>
#include <iostream>
#include <cstring>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

class fdbuf: public std::streambuf 
{
private:
	static const int pbSize=4;
	static const int bufSize=1024;
	char buffer[bufSize+pbSize];
	int fd_;
public:
	fdbuf(int fd):fd_(fd){
		setg(buffer+pbSize, buffer+pbSize, buffer+pbSize);
	}
	int fd() {
		return fd_;
	}
	int buffersize() {
		return bufSize;
	}
protected:
	virtual int_type underflow(){
#ifndef _MSC_VER
		using std::memcpy;
#endif
		if(gptr()<egptr()){
			return *gptr();
		}
		int numPutback;
		numPutback = gptr()- eback();
		if(numPutback > pbSize){
			numPutback = pbSize;
		}

		memcpy(buffer+pbSize-numPutback,gptr()-numPutback,numPutback);
		int num;
#ifdef DEBUG
		//std::cerr<<"call read() for fd: "<<fd_<<'\n';
#endif
		num= read(fd_,buffer+pbSize,bufSize);
		if(num<=0){
#ifdef DEBUG
			//std::cerr<<"read() for fd:"<<fd_<<" return "<<num<<'\n';
			//std::cerr<<fd_<<::strerror(errno)<<'\n';
			//std::cerr<<fd_<<" "<<"fdstream buffer content:" << buffer+pbSize<<'\n';
#endif
			return EOF;
		}
#ifdef DEBUG
		//std::cerr<<"read() for fd:"<<fd_<<" complete\n";
#endif

		setg(buffer+pbSize-numPutback,
				buffer+pbSize,
				buffer+pbSize+num);
		return *gptr();
	}
	/**
     *  @note  off must not exceed the buffer range
	*/
	virtual pos_type seekoff(off_type off, std::ios_base::seekdir way,
								   std::ios_base::openmode which=std::ios_base::in)
	{
		char *ptr=0;
		switch(static_cast<int>(way)){
			case std::ios_base::beg: ptr=eback() +off;break;
			case std::ios_base::cur: ptr=gptr() +off;break;
			case std::ios_base::end: ptr=egptr() +off;break;
		}
		if(ptr !=nullptr){
			setg(eback(),ptr,egptr());
			return ptr - buffer -pbSize ; 
		} else return -1;
	}
	virtual int_type overflow(int_type c){
		if(c != EOF){
			char z=c;
			if(write(fd_,&z,1)!=1){
				return EOF;
			}
		}
		return c;
	}
	virtual std::streamsize xsputn(const char *s, std::streamsize num){
#ifdef DEBUG
		std::cerr<<"write "<<num<<" bytes to "<<"fd : "<<fd_<<'\n';
#endif
		return write(fd_,s,num);
	}
};
class fdostream: virtual public std::ostream 
{
protected:
	fdbuf buf;
public:
	fdostream(int fd):std::ostream(0),buf(fd){
		rdbuf(&buf);
	}
};
class fdistream : public std::istream
{
protected:
	fdbuf buf;
public:
	fdistream(int fd): std::istream(0), buf(fd){
		rdbuf(&buf);
	}
};
class fdstream : public fdistream, public fdostream {
public:
	fdstream(int fd): fdistream(fd),fdostream(fd) {
		rdbuf(static_cast<fdostream *>(this)->rdbuf());
	}
};
#endif
