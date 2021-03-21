#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <Conf.hpp>
#include <Logger.hpp>
#include <functional>
#include <cstring>

#ifdef __linux__
#include <unistd.h>
#endif

namespace simpleweb{
level Conf::loglevel_=
#ifdef DEBUG
	Debug;
#else
	Info;
#endif

Conf::Conf() {
	ip_ = "127.0.0.1";
	port_ = 8080;
	threadcnt_ = 4;
	defaulthost_ = { "", "." };
	logfilename_ = "";
	filecachesnum_ = 32;
}
int Conf::init(Logger& logger) {
	if(logfilename_.length()) {
		logger.logfile(logfilename_);
	}
	return 0;
}
int Conf::global_parser(std::istream& is) 
{
	std::string line;
	Logger::logfor(Debug, "Read global conf\n");
	while(is.good()&&is.peek() != '['&&getline(is,line)) {
		int i=0;
		while(i<line.length() && line[i] != '=') {
			i++;
		}
		if(i<line.length()) {
			std::string && key = line.substr(0,i);
			std::string && value = line.substr(i+1);
			switch(i) {
				case 2: 
					if(key == "ip") {
						ip_=line.substr(i+1); 
						Logger::logfor(Debug, "set ip to ", ip_, '\n');
					}
					break;
				case 4: 
					if(key == "port") {
						port_=stoi(line.substr(i+1)); 
						Logger::logfor(Debug, "set port to ", port_, '\n');
					}
					break;
				case 8:
					if(key == "loglevel") {
						switch(value.length()) {
							case 5:
								if(value == "debug")
									loglevel_ = Debug;
								break;
							case 4:
								if(value == "info")
									loglevel_ = Info;
								else if(value == "none")
									loglevel_ = None;
								break;
						}
					}
					break;
				case 7:
					if(key == "logfile") {
						if( ::access(value.c_str(),R_OK) == 0 )
							logfilename_ = line.substr(i+1);
						else {
							Logger::logfor(Info, ::strerror(errno));
							return -1;
						}
					}
					if( key == "docroot" ) {
						if( ::access(value.c_str(),R_OK) == 0 ) {
							defaulthost_.docroot = line.substr(i+1);
							Logger::logfor(Debug, "set default docroot to ", defaulthost_.docroot, '\n');
						} else {
							Logger::logfor(Info, ::strerror(errno));
							return -1;
						}
					}
					break;
				case 9:
					if(key == "threadcnt") {
						threadcnt_ = stoi(value);
						Logger::logfor(Debug, "boost with ", threadcnt_," threads\n");
					}
					if(key == "cachesnum") {
						filecachesnum_ = stoi(value);
						Logger::logfor(Debug, "boost with ", threadcnt_," filecaches\n");
					}
					break;
				case 11:
					if( key == "defaulthost" ) {
						defaulthost_.servername= value;
						Logger::logfor(Debug, "set default host to ", defaulthost_.servername, '\n');
					}
					break;
			}
		}
	}
	return 0;
}

int Conf::host_parser(std::istream& is) {
	std::string line;
	Logger::logfor(Debug, "Read host conf\n");
	hostinfo host;
	while(is.good()&&is.peek() != '['&&getline(is,line)) {
		int i=0;
		while(i<line.length() && line[i] != '=') {
			i++;
		}
		if(i<line.length()) {
			host.docroot=".";
			std::string &&key = line.substr(0,i);
			switch(i) {
				//servername
				case 10: 
					if(key == "servername") {
						host.servername = line.substr(i+1);
					}
					break;
				case 7: 
					if(key == "docroot") {
						host.docroot = line.substr(i+1);
					}
					break;
			}
		}
	}
	Logger::logfor(Debug, "add server ( ", 
				   host.servername, ' ', host.docroot, " )\n");
	hosts_.push_back(host);
	return 0;
}

int Conf::read(const std::string& confile)
{
	using namespace std;
	if(::access(confile.c_str(),R_OK) < 0) {
		Logger::logfor(Info, "configuration file not found\n");
		Logger::logfor(Info, "set to default ip and port\n" 
					   ,ip_ ,':' ,port_, '\n');
		return 0;
	}
	ifstream conf_istrm(confile);
	std::string line;

	Logger::logfor(Info, "Read conf file from ", confile, '\n');
	while(getline(conf_istrm,line)) {
		int i=0;
		if(line[0] != '[') {
			Logger::logfor(Info, line, '\n');
			Logger::logfor(Info, "key must be preceeded with '[' \n");
			return -1;
		}
		while(i+1<line.length() && line[i+1] != ']') {
			i++;
		}
		if(i<line.length()) {
			switch(i) {
				case 6:
					if(line.substr(1,i) == "global")
						if(global_parser(conf_istrm) < 0) {
							Logger::logfor(Info, "parse global configuration failed\n");
							return -1;
						}
					break;
				case 4:
					if(line.substr(1,i) == "host")
						if(host_parser(conf_istrm) < 0) {
							Logger::logfor(Info, "parse host conf failed\n");
							return -1;
						}
					break;
			}
		}
	}
	return 0;
}

}
