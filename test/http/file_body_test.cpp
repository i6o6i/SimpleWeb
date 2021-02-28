#include <iostream>
#include <http/body.hpp>

using namespace std;
using namespace http;
int main(){
	file_body fb("index.html");

	int fs = file_body::write(cout, fb);
	cout<<endl<<fs<<endl;
	cout<<file_body::size(fb);
}
