#ifndef HTTP_FILE_HPP
#define HTTP_FILE_HPP

#include <ios>

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fdstream.hpp>
#endif

class file {
	int fd_;
	uint64_t fsize;
	char* fileaddr_;
	void openfile(const char* path, std::ios_base::openmode m);
public:
	typedef file value_type;
	file();
	file(int fd, const char* path);
	file(const char* fstr, std::ios_base::openmode m = std::ios_base::in |std::ios_base::out);
	file(const std::string& fstr, std::ios_base::openmode m = std::ios_base::in |std::ios_base::out);
	file(file&& f);
	file & operator=(file&& file);
	~file();
	int close();
	std::size_t size() const;
	char* fileaddr() const{ return fileaddr_; }
	int fd() const;
	void invalid() { fd_ = -1; }
};
#endif
