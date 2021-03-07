#include <Gateway.hpp>
#include <http/message.hpp>
#include <http/parser.hpp>
#include <http/serializer.hpp>
#include <http/vector_body.hpp>
#include <arpa/inet.h>
#include <cerrno>
#include <ctime>
#include <unistd.h>
#include <sstream>
#include <Logger.hpp>
#include <http/shared_file_body.hpp>

namespace simpleweb{

static const char* getcontent_type(unsigned int hash) {
	switch(hash) {
		case 6640: return "text/plain"; break;
		case 43301: return "text/html"; break;
		case 6539: return "text/css"; break;
		case 5988: return "image/svg+xml"; break;
		case 911: return "text/javascript"; break;
		case 5929: return "image/png"; break;
		case 41168: return "image/jpeg"; break;
		case 5839: return "image/gif"; break;
		case 6237: return "image/x-icon"; break;
		default: return "text/plain";
	}
}

int application::response(
	fdstream& os
	,http::response_header& req_h
   )
{
	return 0;
}
host::host(const hostinfo& info, int cachesnum)
	:info_(info),filecaches_(cachesnum)
{

}

int host::access(const std::string& path) {
	if(::access(path.c_str(),R_OK))
		return errno;
	else return 0;
}
void host::serve(
	std::ostream& logstream,
	fdstream& conn, 
	http::request_parser<http::empty_body>& par
	) 
{
	using namespace http;
	request_message<empty_base>& req_m=par.get();
	time_t rawtime;
	::time(&rawtime);
	char timestr[30];
	::strftime(timestr,30,"%a, %d %b %Y %T GMT", ::gmtime(&rawtime));
	response_header res_h(
		status::internal_server_error, 
		{
			{"Date",timestr},
			{"Server",Conf::ServerVersion()},
		},
		10
	);
	::strftime(timestr,30,"%a, %d %b %Y %T GMT", gmtime(&rawtime));
	//404 and 401
	if(req_m.target()=="/") {
		req_m.target("/index.html");
	}
	if(isDynamic(par)) {
		//using reqs_body = vector_body<reqargs>;

		request_parser<vector_body<reqargs>> par_(std::move(par));
		read_body(conn,par_);
		dynamicapps[req_m.target()].response(conn,res_h);
	} else {
		std::string &&filepath =info_.docroot+req_m.target() ;
		int ec = access(filepath);
		if(ec == 0) {
			res_h.result(status::ok);

			unsigned int hash =0;
			for(int i=filepath.size()-1;i&&filepath[i]!='.';i--) 
				hash = hash*7+filepath[i];

			res_h["Content-type"] = getcontent_type(hash);
			res_h["Connection"] = "keep-alive";
			fileNode filenode;
			if(filecaches_.get(filepath) != filecaches_.end()) {
#ifdef DEBUG
				std::cerr<<"caches hit filename "<<filenode.name<<'\n';
#endif
				filenode = *filecaches_.get(filepath);
			} else {
#ifdef DEBUG
				std::cerr<<"caches missed filename "<<filepath<<'\n';
				std::cerr<<"set caches for file "<<filepath<<'\n';
#endif
				filenode = filecaches_.set(filepath);
			}
			//shared_file_body file_body = { filenode.fptr };
			response_serializer<shared_file_body> res_ser({std::move(res_h), filenode.fptr});
			response_message<std::shared_ptr<file>>& res_m = res_ser.get();
			res_m["Content-Length"] = std::to_string(shared_file_body::size(res_m));
			write(conn,res_ser);
			//defaultapp.response(conn,res_h,pagefile);
		} else {
			res_h["Content-type"]="text/plain; charset=UTF-8";
			//{"Content-Length",std::to_string(file_body::size(fb.body())) },
			switch(ec) {
				case EACCES: res_h.result(status::forbidden);break;
				case ENOENT: res_h.result(status::not_found);break;
			}
			std::string resultstring = res_h.reason();
			response_serializer<string_body> res_ser{
				{ std::move(res_h), resultstring}
			};
			write(conn,res_ser);
		}
	} 
	char logtime[23];
	std::strftime(logtime,23,"[%d/%b/%Y:%T]", std::localtime(&rawtime));
	if(Conf::loglevel() >= Info)
		logstream<<logtime<<' '<<req_m.method_string()<<' '<<req_m.target()<<' '<<res_h.result()<<'\n';
}
bool host::isDynamic(http::request_parser<http::empty_body>& par) {
	return par.get().method()== http::verb::post 
		|| par.get().method()== http::verb::get && par.querystr();
}
Gateway::Gateway(Reactor& reactor, Logger& logger, Conf& conf)
	:defaultHost(conf.defaulthost(),conf.filecachesnum())
{
	using namespace http;
	for(auto& h: conf.hosts()) {
		Hosts.insert({h.servername, host(h,conf.filecachesnum())});
	}
	reactor.register_handler<RW>([&,this](epoll_event& e){
#ifdef DEBUG
		//std::cerr<<"connection fd: "<<e.data.fd<<'\n';
#endif
		fdstream conn(e.data.fd);
		if(conn.peek() == EOF) {
			//std::cerr<<"connection return 0 bytes, remove and close\n";
			reactor.remove_event(e.data.fd);
			::close(e.data.fd);
			return ;
		}
		request_parser<empty_body> req_par{};
		read_header(conn,req_par);
		request_message<empty_base> &req_m = req_par.get();
		if(!req_par.is_header_done()) {
			::shutdown(e.data.fd,SHUT_RDWR);
			return ;
		}

		std::ostringstream oss;
		oss<<::inet_ntoa(fd_ip_map_[e.data.fd])<<' ';
		//Logger::logfor(Info, ::inet_ntoa(fd_ip_map_[e.data.fd]), ' ');
		if(req_m["Host"] == conf.defaulthost().servername)
			defaultHost.serve(oss,conn,req_par);
		else if(Hosts.find(req_m["Host"]) == Hosts.end()){
			Hosts[req_m["Host"]].serve(oss,conn,req_par);
		}else {
			::shutdown(e.data.fd,SHUT_RDWR);
			oss<<"host not found\n";
			return ;
		}
		//::shutdown(e.data.fd,SHUT_RD);
		if(Conf::loglevel() >= Info)
			logger.log(oss.str());
#ifdef DEBUG
		//std::cerr<<"c fd< "<<e.data.fd<<" >";
#endif
		::shutdown(e.data.fd,SHUT_RDWR);
	});
}
}
