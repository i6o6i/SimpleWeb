#ifndef SIMPLEWEB_H
#define SIMPLEWEB_H
#include <string>
#include <functional>
#include <queue>
#include <sstream>
#include <sys/epoll.h>
#include <fdstream.hpp>
#include <ThreadPool.hpp>
#include <Conf.hpp>
#include <Logger.hpp>
namespace simpleweb{
namespace utils{
int getlasterr();
}
#ifdef __linux__
using SOCKET=int;
#elif _WIN32
#endif
class SimpleWeb{
	public:
		SimpleWeb(const std::string& ip="127.0.0.1", int port=8080); //read conf and listen port
		SimpleWeb(SimpleWeb & simpleweb) = delete ;
		~SimpleWeb();
		void run();
	private:
		int openport(int port=80);
		Conf conf;
		Logger logger;
};
}
#endif
