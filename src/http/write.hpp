#ifndef HTTP_WRITE_HPP
#define HTTP_WRITE_HPP
#include <http/serializer.hpp>
#include <type_traits>

namespace http {
template<bool isRequest, class Body, class Fields>
class serializer;

template<bool isRequest,class Body, class Fields >
typename std::enable_if<isRequest,uint64_t>::type
//uint64_t
write_header(std::ostream &os, serializer<isRequest, Body, Fields>& sr)
{
	std::size_t bytes = 0;
	message<true,Body,Fields> & m = sr.message_;
	os<<m.method_string()+" "+m.target()+" "+m.version_string()<<"\r\n";
	bytes +=m.method_string().length()
		+ m.target().length()
		+ m.version_string().length() + 4;
	bytes +=write(os,static_cast<fields&>(sr.message_));
	sr.header_done_ = true;
	return bytes;
}
template<bool isRequest,class Body, class Fields >
typename std::enable_if<!isRequest,uint64_t>::type
//uint64_t
write_header(std::ostream &os, serializer<isRequest, Body, Fields>& sr)
{
	std::size_t bytes = 0;
	message<false,Body,Fields> & m = sr.message_;
	os<<m.version_string()+" "+m.reason()<<"\r\n";
	bytes +=m.version_string().length()
		+ m.reason().length() + 3;
	bytes +=write(os,static_cast<fields&>(sr.message_));
	os<<"\r\n";
	bytes += 2;
	sr.header_done_ = true;
	return bytes;
}
template<bool isRequest,class Body, class Fields >
uint64_t
write(std::ostream &os, serializer<isRequest, Body, Fields>& sr)
{
	uint64_t bytes = 0;
	bytes+=write_header<isRequest>(os,sr);
	bytes+=Body::write(os,sr.message_);
	sr.done_ =true;
	return bytes;
};

}
#endif
