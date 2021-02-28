#ifndef REACTOR_CPP
#define REACTOR_CPP
#include <sys/epoll.h>
#include <type_traits>
#include <Reactor.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <Logger.hpp>
#ifdef DEBUG
#include <cerrno>
#include <cstring>
#endif

namespace simpleweb {

void Reactor::init() {
	handlefd_  = epoll_create(MAX_EVENT);
}
Reactor::Reactor() {
	register_handler<UNKNOWN>([](epoll_event&){
		std::cerr<<"Unknown event\n";
	});
	init();
}
/*
Reactor::Reactor(ThreadPool& tp)
	:_tp_ref(tp)
{

}
*/
int Reactor::add_event(int fd, uint32_t flag) {
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag;
	int res;
	res=epoll_ctl(handlefd_,EPOLL_CTL_ADD,fd,&ev);
#ifdef DEBUG
	//std::cerr<<"EPOLL_CTL_ADD "<<fd<<" return "<<res<<'\n';
#endif
	if(res < 0) {
#ifdef DEBUG
		std::cerr<<::strerror(errno);
#endif
	}
	return res;
}
int Reactor::modify_event(int fd, uint32_t flag) {
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag;
	return epoll_ctl(handlefd_,EPOLL_CTL_MOD,fd,&ev);
}

int Reactor::remove_event(int fd) {
	int res;
	res=epoll_ctl(handlefd_,EPOLL_CTL_DEL, fd,nullptr);
#ifdef DEBUG
	//std::cerr<<"EPOLL_CTL_DEL "<<fd<<" return "<<res<<'\n';
#endif
	return res;
}

demultiplexer_type Reactor::register_demultiplexer(demultiplexer_type demultiplexer) {
	demultiplexer_type tmp = demultiplexer_;
	demultiplexer_ = demultiplexer;
	return tmp;
}

void Reactor::handle_events(Logger& logger, ThreadPool& tp) {
	int nfds;
	epoll_event evs[MAX_EVENT];
	std::future<void> fut[MAX_EVENT];
	while(1) {
		nfds = ::epoll_wait(handlefd_, evs, MAX_EVENT, -1);
		if(nfds<0) {
			std::cerr<<"epoll_wait\n";
		}
		for(int i=0;i<nfds;i++){
			fut[i] = tp.emplace_back(handler[demultiplexer_(evs[i])], evs[i]);
#ifdef DEBUG
			//Logger::logfor(Debug, "tp.emplace_back\n");
#endif
		}
		for(int i=0;i<nfds; i++) {
			fut[i].get();
		}
		if(Conf::loglevel() >= Info)
			logger.write();
	}
}
void Reactor::handle_events() {
	int nfds;
	epoll_event evs[MAX_EVENT];
	while(1) {
		nfds = ::epoll_wait(handlefd_, evs, MAX_EVENT, -1);
		if(nfds<0) {
			std::cerr<<"epoll_wait\n";
		}
		for(int i=0;i<nfds;i++){
			handler[demultiplexer_(evs[i])](evs[i]);
		}
	}
}
};
#endif
