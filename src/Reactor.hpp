#ifndef REACTOR_HPP
#define REACTOR_HPP
#include <ThreadPool.hpp>
#include <functional>
#include <sys/epoll.h>
#include <Logger.hpp>
#define MAX_EVENT 1024

namespace simpleweb {
typedef threadpool::ThreadPool ThreadPool;
enum EventType {CONNECT, RW, LOG, UNKNOWN};
//typedef int (*handler_type)(int fd);
//typedef int (*demultiplexer_type)(epoll_event ev);
typedef std::function<void(epoll_event&)> handler_type;
typedef std::function<int(epoll_event&)> demultiplexer_type;
class Reactor {
private: 
	int handlefd_;
	handler_type handler[3];
	demultiplexer_type demultiplexer_;
	//ThreadPool& _tp_ref;
	void init();
public:
	Reactor();
	int handlefd() {return handlefd_;}
	//Reactor(ThreadPool& tp);
	int add_event(int fd, uint32_t flag);
	int modify_event(int fd, uint32_t flag);
	int remove_event(int fd);
	void handle_events(Logger& logger, ThreadPool& tp);
	void handle_events();
	demultiplexer_type register_demultiplexer(demultiplexer_type demultiplexer);
	template<EventType E>
	void register_handler(handler_type handler);
};
template<EventType E>
void Reactor::register_handler(handler_type h) {
	handler[static_cast<int>(E)] = h;
}
}
#endif
