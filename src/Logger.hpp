#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <iostream>
//#include <sharedqueue.hpp>
#include <Conf.hpp>
#include <queue>
#include <mutex>

namespace simpleweb{
class Logger{
	std::queue<std::string> mess_list;
	std::mutex que_mutex;
	std::string logfile_;
	public:
		template<class ... Ts>
		static void logfor(level loglevel, Ts ... Args)
		{
			if(loglevel <= Conf::loglevel() )
				int dummy[sizeof...(Ts)] = { (std::cerr << Args, 0)... };
		}
		//static void perror();
		void log(std::string mess);
		void logfile(std::string& filepath) { logfile_ = filepath; }
		void write();
		//std::mutex& que_mutex() { return que_mutex_; }
};
}
#endif
