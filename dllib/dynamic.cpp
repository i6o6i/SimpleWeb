#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <http/message.hpp>
#include <http/vector_body.hpp>
#include <http/serializer.hpp>


using namespace http;
extern "C" void get(
	request_message<std::vector<reqargs>>& req,
	response_message<std::string>& res
)
{
	using namespace std;
	for(auto &i: static_cast<vector<reqargs>>(req)) {
		res+=i.key;
		res+='=';
		res+=i.value;
		res+='\n';
	}
}
