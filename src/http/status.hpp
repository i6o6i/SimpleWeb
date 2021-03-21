#ifndef HTTP_STATUS_HPP
#define HTTP_STATUS_HPP
#include <string>

enum status : unsigned {
	unknown = 0,
	ok = 200,
	bad_request = 400,
	forbidden = 403,
	not_found = 404,
	internal_server_error = 500,
	bad_gateway = 502,

};

inline const std::string to_string(const status &s){
	static std::string statusTable[599];
	if(statusTable[200].length() == 0){
		auto idxOf= [](status s){return static_cast<int>(s);};
		//statusTable[idxOf(status::unknown)]="400 Bad Request";
		statusTable[idxOf(status::ok)]="200 Ok";
		statusTable[idxOf(status::bad_request)]="400 Bad Request";
		statusTable[idxOf(status::forbidden)] = "403 Forbidden";
		statusTable[idxOf(status::not_found)] = "404 Not Found";
		statusTable[idxOf(status::internal_server_error)] = "500 Internal Server Error";
		statusTable[idxOf(status::bad_gateway)] = "502 Bad Gateway";
	}
	return statusTable[static_cast<int>(s)];
}
#endif
