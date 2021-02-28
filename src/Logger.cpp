#include <cstring>
#include <cerrno>
#include <Logger.hpp>
#include <fstream>

namespace simpleweb{
/*
void Logger::perror() {
	std::cerr<<::strerror(errno)<<'\n';
}
*/
void Logger::log(std::string mess) {
	std::unique_lock<std::mutex> que_mutex_lock(que_mutex);
	mess_list.push(mess);
	que_mutex_lock.unlock();
}
void Logger::write() {
	std::unique_lock<std::mutex> que_mutex_lock(que_mutex);
	while(!mess_list.empty()) {
		std::string& mess=mess_list.front();
		if(logfile_.length() ) {
			std::ofstream fs(logfile_);
			fs<<mess;
		}else std::cerr<<mess;
		mess_list.pop();
	}
	que_mutex_lock.unlock();
#ifdef DEBUG
	std::cerr<<"mess_list.empty() == "<<mess_list.empty()<<'\n';
#endif
}
}
