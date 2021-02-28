#ifndef GATEWAY_HPP
#define GATEWAY_HPP
#include <map>
#include <unordered_map>
#include <Reactor.hpp>
#include <fdstream.hpp>
#include <http/body.hpp>
#include <http/message.hpp>
#include <http/parser.hpp>
#include <http/empty_body.hpp>
#include <arpa/inet.h>
#include <Logger.hpp>
#include <filecaches.hpp>
/*
namespace std
{
template<>
struct hash<struct in_addr>
{
	std::size_t operator()(struct in_addr const& ip) const noexcept {
		hash<unsigned int> h;
		return h(ip.s_addr);
	}
};
}
*/
namespace simpleweb{

class application{
public:
	int response(fdstream& os, http::response_header& req_h);
	static const char* getcontent_type(std::string& filepath);
};

class host{
	hostinfo info_;
	std::map<std::string,application> dynamicapps;
	filecaches filecaches_;
	int access(const std::string& path);
public:
	host(): filecaches_(0){}
	host(const hostinfo& info, int cachesnum);
	void serve(std::ostream& logstream, fdstream& ios, http::request_parser<http::empty_body>& par);
	bool isDynamic(http::request_parser<http::empty_body>& par);
};
class Gateway{
	private:
		std::unordered_map<int, struct in_addr> fd_ip_map_;
		std::map<std::string, host> Hosts;
		host defaultHost;
	public:
		Gateway(Reactor& reactor, Logger& logger, Conf& conf);
		std::unordered_map<int, struct in_addr> &fd_ip_map() {return fd_ip_map_;}
		void handle(int fd, unsigned int ip);
};
}
#endif
