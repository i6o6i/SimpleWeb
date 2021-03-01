#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <exception>
#include <system_error>
#include <SimpleWeb.hpp>
#include <Reactor.hpp>
#include <Gateway.hpp>
#include <Logger.hpp>

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#elif _WIN32
#include <Winsock2.h>
#endif

namespace simpleweb{
	namespace utils{
	int getlasterr()
	{
#ifdef __linux__
		return errno;
#elif _WIN32
		return WSAGetLastError();
#endif
	}
	}
SimpleWeb::SimpleWeb(const std::string& ip, int port)
{
	if(conf.read() < 0 ) {
		exit(-1);
	}
	conf.init(logger);

	Logger::logfor(Debug,"Debug mode enable\n");
#ifdef DEBUG
	std::cerr<<"DEBUG flag enable\n";
#endif
}

void SimpleWeb::run(){
	ThreadPool tp(conf.threadcnt());
	Reactor reactor;
	Gateway gateway(reactor,logger,conf);

	SOCKET listener_fd = openport(conf.port());
	Logger::logfor(Info,"bind to ",conf.ip(),".",conf.port(),"\n");
	fcntl(listener_fd, F_SETFL, 
		  fcntl(listener_fd,F_GETFL,0)|O_NONBLOCK
		  );
	reactor.add_event(listener_fd,EPOLLIN|EPOLLET);

	reactor.register_demultiplexer([&](epoll_event& ev)->int{
		if(ev.data.fd == listener_fd)
			return static_cast<int>(CONNECT);
		else if(ev.events&EPOLLIN) {
			return static_cast<int>(RW);

		} else return static_cast<int>(UNKNOWN);
	});

	/*
	tp.emplace_back([&]{
		if(conf.logfilename().length() == 0) {
			std::fstream os(conf.logfilename());
				logger.write(os);
		}else {
				logger.write(std::cerr);
		}
	});
	*/
#ifdef DEBUG
	std::cerr<<"listener_fd is "<<listener_fd<<'\n';
#endif


	auto& fd_ip_map = gateway.fd_ip_map();
	reactor.register_handler<CONNECT>([&](epoll_event& ev){
		sockaddr_in client_addr;
		socklen_t client_addr_len=sizeof(sockaddr_in);
		int connfd;
		while(1) {
			if( (connfd = accept(listener_fd,(struct sockaddr *)&client_addr,&client_addr_len)) < 0 ) {
				if(errno == EWOULDBLOCK) {
					break;
				}else if(errno == ETOOMANYREFS ){
					std::cerr<<"ETOOMANYREFS\n";
					continue;
				}else {
					//std::cerr<<"accept "<<connfd<<" failed\n";
					//std::cerr<<::strerror(errno)<<'\n';
					break;
				}
			}
			fd_ip_map[connfd]=client_addr.sin_addr;
#ifdef DEBUG
			//std::string mess;
			//mess +="Connection from ";
			//mess +=inet_ntoa(client_addr.sin_addr);
			//mess +='\n';
			//std::cerr<<mess;
			std::cerr<<"reactor.add_event "<<connfd<<'\n';
#endif
			reactor.add_event(connfd,EPOLLIN|EPOLLET );
			//reactor.modify_event(connfd, EPOLLONESHOT);
		}
	});
	reactor.handle_events(logger,tp);
	//reactor.handle_events();

}

SimpleWeb::~SimpleWeb() {
}

SOCKET SimpleWeb::openport(int port)
{
#ifdef __linux__
	const int INVALID_SOCKET=-1;
	const int SOCKET_ERROR=-1;
#elif _WIN32
#endif
	SOCKET sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
#ifdef DEBUG
	int optval = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&optval, sizeof(optval));
#endif

	if(INVALID_SOCKET == sockfd)
		throw std::system_error(utils::getlasterr(),std::system_category());

	struct sockaddr_in service;
	service.sin_family=AF_INET;
	service.sin_addr.s_addr = inet_addr(conf.ip().c_str());
	service.sin_port=htons(port);
	int s=bind(sockfd,(struct sockaddr*)&service,sizeof(struct sockaddr));
	if(SOCKET_ERROR == s) {
		throw std::system_error(utils::getlasterr(),std::system_category());
	}
	s=listen(sockfd,SOMAXCONN);
	if(SOCKET_ERROR == s){
		throw std::system_error(utils::getlasterr(),std::system_category());
	}
	return sockfd;
}
}
