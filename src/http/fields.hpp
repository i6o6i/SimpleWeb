#ifndef HTTP_FIELDS_HPP
#define HTTP_FIELDS_HPP

#include <iostream>
#include <initializer_list>
#include <map>
#include <string>

namespace http{
class fields{
private:
	std::map<std::string,std::string> fields_;
public:
	typedef std::pair<const std::string,std::string> value_type;
	fields(const std::initializer_list<std::pair<const std::string,std::string>>&& fs):fields_(fs){}
	std::string &operator[](const std::string & key){
		return fields_[key];
	}
	fields() = default;
	/*
	friend std::size_t read(std::istream &is, fields & f){
		std::size_t bytes = 0;
		std::string field_str;
		int idx;

		while(is>>field_str){
			idx=field_str.find(':');
			f.fields_[field_str.substr(0,idx)]=field_str.substr(idx);
			bytes += field_str.length() +2;
		}
		return bytes;
	}
	*/
	friend std::size_t write(std::ostream &os, fields &f){
		std::size_t bytes = 0;
		for(auto const &field:f.fields_){
			os<<field.first<<": "<<field.second<<"\r\n";
			bytes += field.first.length() + field.second.length() + 2;
		}
		return bytes;
	}
};
}
#endif
