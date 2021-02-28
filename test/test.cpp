#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fdstream.hpp>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
int main(){
	cout<<"test for fdstream::tellg\n";

	int fd=open("text",O_RDONLY);
	cout<<"fd : "<<fd<<endl;
	//fdistream fs(fd);
	ifstream fs("text");
	char buffer[1025]={0};

	/*
	fs.seekg(3);
	fs.read(buffer,1023);
	cout<<"fs.beg "<<fs.beg<<endl;
	cout<<"fs.tellg "<<fs.tellg()<<endl;
	cout<<"fs.tellg "<<fs.rdbuf()->pubseekoff(0,ios_base::cur,ios_base::in)<<endl;
	cout<<"fs.end "<<fs.end<<endl;
	cout<<buffer;
	*/
	cin.rdbuf(fs.rdbuf());
	cin.read(buffer,1024);
	cout<<endl<<fs.tellg()<<endl;
	cout<<cin.tellg()<<endl;
	cout<<buffer;
	/*
	while(fs.read(buffer,1024)){
		cin>>buffer;
		cout<<endl<<fs.tellg()<<endl;
		//cout<<buffer;
	}
	*/
}
