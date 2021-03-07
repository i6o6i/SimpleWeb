#ifndef CONF_H
#define CONF_H
#include <iostream>
#include <string>
#include <vector>

namespace simpleweb{
class Logger;
enum  level {None, Info, Debug} ;
struct hostinfo {
	std::string servername;
	std::string docroot;
};
class Conf{
		std::string ip_;
		int port_;
		static level loglevel_;
		std::string logfilename_;
		std::vector<hostinfo> hosts_;
		hostinfo defaulthost_;
		int threadcnt_;
		int filecachesnum_;
		int global_parser(std::istream& is);
		int host_parser(std::istream& is);
	public:
		Conf();
		int read(const std::string& confile = "sw.conf");
		int init(Logger& logger);
		//Conf(char **argv) = delete;
		const std::string & ip() const {return ip_;}
		int port() {return port_;}
		int threadcnt() {return threadcnt_;};
		int filecachesnum() {return filecachesnum_; }
		const std::string& logfilename() {return logfilename_;}
		const hostinfo& defaulthost() {return defaulthost_;}
		const std::vector<hostinfo>& hosts() {return hosts_;}
		static level loglevel() {return loglevel_;}
		static std::string ServerVersion() { return "SimpleWeb/0.1"; }
		static void loglevel(level loglevel) { loglevel_ = loglevel; }
};
}
#endif
