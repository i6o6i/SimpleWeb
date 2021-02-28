#include <http/parser.hpp>
#include <http/serializer.hpp>
#include <http/message.hpp>
#include <http/empty_body.hpp>
#include <http/vector_body.hpp>
#include <initializer_list>
#include <sstream>

using namespace std;
using namespace http;

int main() {
	stringstream post(R"END(
POST /text HTTP/1.0
content-type: application/json; charset='utf-8'
origin: https://yqfk.dgut.edu.cn
user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36'

ip=1.1.1.1&name=bkb
)END");
	stringstream get(R"END(
GET /text?ip=1.1.1.1&name=bkb HTTP/1.0
content-type: application/json; charset='utf-8'
origin: https://yqfk.dgut.edu.cn
user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36'

)END");

	{
		request_parser<empty_body> req_par_headeronly{};
		read_header(post,req_par_headeronly);

		request_parser<vector_body<reqargs>> req_par{std::move(req_par_headeronly)};
		read_body(post,req_par);
		request_serializer<vector_body<reqargs>> req_ser{std::move(req_par.get())};
		cout<<"result:\n";
		write(cout,req_ser);
		cout<<endl;
	}
	{
		request_parser<empty_body> req_par_headeronly{};
		read_header(get,req_par_headeronly);

		request_parser<vector_body<reqargs>> req_par{std::move(req_par_headeronly)};
		read_body(get,req_par);
		request_serializer<vector_body<reqargs>> req_ser{std::move(req_par.get())};
		cout<<"result:\n";
		write(cout,req_ser);
		cout<<endl;
	}
	return 0;
}
