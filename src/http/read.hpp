#ifndef HTTP_READ_HPP
#define HTTP_READ_HPP
#include <cstring>
#include <sstream>
#include <http/body.hpp>
#include <http/utils.hpp>
#include <http/parser.hpp>

namespace http {
template<bool isRequest,class BodyHandler, class Fields >
class parser;

enum class parser_ec {
	ok=0,
	invalid_method,
	invalid_request,
	invalid_header
};

template<bool isRequest,class BodyHandler, class Fields >
typename std::enable_if<isRequest,parser_ec>::type
read_header_line(std::istream &is, parser<isRequest, BodyHandler, Fields>& par)
{
	enum {
		s_start,
		s_method,
		s_spaces_before_uri,
		s_after_slash_in_uri,
		s_check_uri,
		s_params,
		s_http,
		s_http_major,
		s_http_minor,
		s_done,
	} state;
	char buffer[1024];
	u_char ch;
	uint16_t i;
	unsigned int http_version=0;
	bool header_line_is_done=false;
	//sr.message_.method(verb::unknown);

	state = s_start	;

	for(;!header_line_is_done&&is.get((char&)ch);){
		switch(state){
			case s_start: 
				if( ch == CR || ch == LF )
					break;
				if(ch < 'A' || ch > 'Z'){
					return parser_ec::invalid_method;
				}
				state = s_method;
				i=0;
				buffer[i++]=ch;
				break;
			case s_method:
				buffer[i++]=ch;
				if(ch == ' '){
#ifdef DEBUG
					//buffer[i]= 0;
					//std::cerr<<buffer;
#endif
					switch(i-1){
						case 3:
							if(std::strncmp(buffer,"GET",3) == 0){
								par.message_.method(verb::get);
								break;
							}
							if(std::strncmp(buffer,"PUT",3) == 0){
								par.message_.method(verb::put);
								break;
							}
						case 4:
							if(std::strncmp(buffer,"HEAD",4) == 0){
								par.message_.method(verb::head);
								break;
							}
							if(std::strncmp(buffer,"POST",4) == 0){
								par.message_.method(verb::post);
								break;
							}
						case 6:
							if(std::strncmp(buffer,"DELETE",6) == 0){
								par.message_.method(verb::delete_);
								break;
							}
						default: {
									 return parser_ec::invalid_method;
							 }
					}
					state = s_spaces_before_uri;
					i=0;
					break;
				}

				if(ch < 'A' || ch > 'Z') {
					return parser_ec::invalid_method;
				}

				break;
			case s_spaces_before_uri:
				if(ch == '/'){
					buffer[i++]=ch;
					state = s_after_slash_in_uri;
					break;
				}
				if(ch == ' '){
					break;
				} else {
					return parser_ec::invalid_request;
				}
			case s_after_slash_in_uri:
				if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
					buffer[i++]=ch;
					break;
				}
				switch(ch) {
					case '?':
						buffer[i]='\0';
						par.message_.target(buffer);
						state = s_params;
						i=0;
						break;
					case '/':
						buffer[i++]=ch;
						break;
					case ' ':
						buffer[i]='\0';
#ifdef DEBUG
					//std::cerr<<buffer<<" ";
#endif
						par.message_.target(buffer);
						state = s_http;
						i=0;
						break;
					case '.':
						buffer[i++]=ch;
						break;
					default:
						return parser_ec::invalid_request;
				}
				break;
			case s_params:
				if (isprint(ch)) {
					buffer[i++]=ch;
				}
				switch(ch) {
					case ' ': 
						buffer[i]='\0';
						if(i)
							par.querystr_ptr=std::make_unique<std::string>(buffer);
						state  =s_http;
						i=0;
						break;
					case '\0':
						return parser_ec::invalid_request;
				}
				break;
			case s_http:
				if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
#ifdef DEBUG
					//std::cerr<<ch;
#endif
					buffer[i++]=ch;
				}
				if(i == 4) {
#ifdef DEBUG
					//buffer[i]='\0';
					//std::cerr<<buffer;
#endif
					if(std::strncmp(buffer,"HTTP",4))
						return parser_ec::invalid_request;
				}
				if( ch == '/' ){
					state = s_http_major;
				}
				break;
			case s_http_major:
				if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
					if(ch >= '0' && ch <='9')
						http_version =  http_version*10+ ch - '0';
					if(http_version > 1) {
						return parser_ec::invalid_request;
					}
					break;
				}
				if(ch == '.') {
					state=s_http_minor;
					break;
				}
				if(ch == ' ') {
					break;
				}
				if(ch == LF)
					header_line_is_done =true;
				return parser_ec::invalid_request;
				break;
			case s_http_minor:
				if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
					if(ch >= '0' && ch <='9')
						http_version =  http_version*10+ ch - '0';
					if(http_version > 11) {
						return parser_ec::invalid_request;
					}
					break;
				}
				if(ch == ' ') {
					break;
				}
				if(ch == LF)
					header_line_is_done =true;
					break;
				if(ch == CR) {
					state = s_done;
					break;
				}
				return parser_ec::invalid_request;
			case s_done:
				if(ch == CR)
					break;
				if(ch== LF)
					header_line_is_done =true;
		}
	}

	par.message_.version(http_version);
	return parser_ec::ok;
}

template<bool isRequest,class BodyHandler, class Fields >
typename std::enable_if<isRequest,parser_ec>::type
read_header(std::istream &is, parser<isRequest, BodyHandler, Fields>& par)
{
	enum {
		s_start,
		s_name,
		s_spaces_before_value,
		s_value,
		s_spaces_after_value,
		s_almost_done,
		s_done,
		s_header_almost_done,
		s_header_done,
	} state ;
	state = s_start;
	parser_ec ec = read_header_line(is,par);
	if(ec!=parser_ec::ok)
		return ec;
	u_char ch;

	char fieldname[MAXFIELDNAME], fieldvalue[MAXFIELDVALUE];
	int i=0;
	for(;!par.header_done_&&is.get((char &)ch);) {
		switch(state) {
			case s_start:
				switch(ch){
					case CR: 
						state = s_header_almost_done; 
						break;
					case LF: 
						par.header_done_ =true;
						break;
					default: 
						if(lowcase[ch]) {
							i=0;
							fieldname[i++] = ch;
							state = s_name;
							break;
						}
				}
				break;
			case s_name:
				if(lowcase[ch]&& i <MAXFIELDNAME) {
					fieldname[i++] = ch;
					break;
				}
				if(ch == ':'&&i <MAXFIELDNAME) {
					fieldname[i] = '\0';
					state = s_spaces_before_value;
					break;
				}
				return parser_ec::invalid_request;
			case s_spaces_before_value:
				switch(ch) {
					case ' ': break;
					case CR: 
							  par.message_[std::string(fieldname)] = '\0';
							  state = s_almost_done; 
							  break;
					case LF: 
							  par.message_[std::string(fieldname)] = '\0';
							  state = s_start; 
							 break;
					default: 
							 i=0;
							 fieldvalue[i++] = ch;
							 state = s_value; 
							 break;
				}
				break;
			case s_value:
				switch(ch) {
					case ' ':
						fieldvalue[i++] = ch;
						break;
					case CR:
						fieldvalue[i]='\0';
						par.message_[std::string(fieldname)] = std::string(fieldvalue);
						state = s_almost_done;
						break;
					case LF:
						fieldvalue[i]='\0';
						par.message_[std::string(fieldname)] = std::string(fieldvalue);
						state = s_start;
						break;
					case '\0':
						return parser_ec::invalid_header;
					default: 
						fieldvalue[i++] = ch;
						break;
				}
				break;
			case s_almost_done:
				if(ch == CR) {
					state = s_done;
					break;
				}
				if(ch == LF) {
					state = s_start;
				}
				break;
			case s_done:
				if(ch == LF) {
					state = s_start;
				}
				break;
			case s_header_almost_done:
				if(ch == LF) {
					par.header_done_ = true ;
					state = s_header_done;
					break;
				}
			default: 
				return parser_ec::invalid_header;
		}
	}

	return parser_ec::ok;
}

template<bool isRequest,class BodyHandler, class Fields >
typename std::enable_if<isRequest,int>::type
read_body(std::istream &is, parser<isRequest, BodyHandler, Fields>& par) {
	if(!par.querystr_ptr.get()) {
		BodyHandler::read(is,par.message_);
	}
	else {
		std::stringstream ss(*par.querystr_ptr);
		BodyHandler::read(ss,par.message_);
	}
	return 0;
}
template<bool isRequest,class BodyHandler, class Fields >
typename std::enable_if<isRequest,int>::type
read(std::istream &is, parser<isRequest, BodyHandler, Fields>& par)
{
	parser_ec _ec = read_header(is,par);
	if(_ec != parser_ec::ok)
		return -1;
	read_body(is,par);
	par.done_ = true;
	return 0;
}
}
#endif
