#include <http/serializer.hpp>
#include <http/empty_body.hpp>
#include <initializer_list>

using namespace std;
using namespace http;
int main(){
	request_header req_h(verb::get, "./text", 10,{
						 {"user-agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36' "},
						 {"content-type","application/json; charset=utf-8'"},
						 {"origin","https://yqfk.dgut.edu.cn"}
	});
	response_header res_h(status::forbidden,{
		{"Date","Tue, 23 Feb 2021 07:11:31 GMT"},
		{"Server","SimpleWeb/0.1"},
		{"Content-Length","4006" },
		{"Content-type","text/html; charset=UTF-8"}
	},10);

	request_message<file_body> req_m(req_h,"index.html");
	request_serializer<file_body> sr{std::move(req_m)};
	
	request_serializer<file_body> sr2{
		{
			{
				verb::get, "./text", 10,
				{
					{"user-agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36' "},
					{"content-type","application/json; charset=utf-8'"},
					{"origin","https://yqfk.dgut.edu.cn"}
				}
			}, "index.html"
		}};

	response_message<empty_body> res_m(res_h);

	write(cout,sr);
	return 0;
}
