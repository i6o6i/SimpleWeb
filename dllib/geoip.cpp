#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <http/message.hpp>
#include <http/vector_body.hpp>
#include <http/serializer.hpp>
#include <errno.h>
#include <cstdarg>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <maxminddb.h>
#include <iostream>
#include <iomanip>

class Mmdbcpp{
	MMDB_s mmdb;
	MMDB_lookup_result_s lookup_result;
	MMDB_entry_data_list_s *entry_data_list;
	public:
	Mmdbcpp(const std::string filename):entry_data_list(nullptr)
	{
		using namespace std;
		int status=MMDB_open(filename.c_str(),MMDB_MODE_MMAP,&mmdb);

		ostringstream oss;
		if(status != MMDB_SUCCESS)
		{
			oss<<"Can't open"<<filename<<" - "<<MMDB_strerror(status)<<endl;
			if(status == MMDB_IO_ERROR )
				oss<<"   IO error: "<<strerror(errno)<<endl;
			throw runtime_error(oss.str());
		}
	}
	void find_entry(std::string ip_address){
		using namespace std;
		int gai_error,mmdb_error;
		lookup_result=MMDB_lookup_string(&mmdb,ip_address.c_str(),&gai_error, &mmdb_error);

		ostringstream oss;
		if(gai_error)
			oss<<"Error from getaddrinfo for "<<ip_address<<" - "<<gai_strerror(gai_error)<<endl;
		if(mmdb_error != MMDB_SUCCESS)
			oss<<"Got an error from libmaxminddb: "<<MMDB_strerror(mmdb_error)<<endl;
		if(oss.tellp())
			throw runtime_error(oss.str());

		if(lookup_result.found_entry){
			int status = MMDB_get_entry_data_list(&lookup_result.entry,&entry_data_list);

			if(status != MMDB_SUCCESS)
			{
				oss<<"Got an error looking up the entry data - "<<MMDB_strerror(status)<<endl;
				throw runtime_error(oss.str());
			}
		}else{
			oss<<"No entry for this IP address ("<<ip_address<<") was found\n";
			throw runtime_error(oss.str());
		}
	}

	std::string get(std::string key,const char *lang="en")
	{
		using namespace std;

		map<string, vector<const char*> > key2pathmap{
			{"continent",    {"continent","names",lang,NULL}        },
				{"country",      {"country","names",lang,NULL}          },
				{"countryCode",  {"country","ios_code",NULL}            },
				{"region",       {"subdivisions","0","names",lang,NULL} },
				{"regionCode",   {"subdivisions","0","ios_code",NULL}   },
				{"city",         {"city","names",lang,NULL}             },
				{"latitude",     {"location","latitude",NULL}           },
				{"longitude",    {"location","longitude",NULL}          }
		};
		auto path=key2pathmap[key];
		string value;
		MMDB_entry_data_s entry_data;
		int status = MMDB_aget_value(&lookup_result.entry,&entry_data,&path[0]);
		if(status != MMDB_SUCCESS)
		{
			if(status == MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR)
				value = "unknown";
			else throw runtime_error(MMDB_strerror(status));
		}else if(entry_data.has_data)
		{
			switch(entry_data.type){
				case MMDB_DATA_TYPE_UTF8_STRING: value=string(entry_data.utf8_string,entry_data.data_size); break;
				case MMDB_DATA_TYPE_DOUBLE: value=to_string(entry_data.double_value); break;
				case MMDB_DATA_TYPE_UINT16: value=to_string(entry_data.uint16); break;
				case MMDB_DATA_TYPE_UINT32: value=to_string(entry_data.uint32); break;
				default: throw runtime_error("unknown type");
			}
		}else throw runtime_error("cannot find entry");
		return value;
	}
	std::vector<std::pair<std::string,std::string>> get()
	{
		using namespace std;
		vector<pair<string,string>> res;
		vector<string> keyarr={ "continent", "country", "region", "city", "latitude", "longitude" };
		char* lang= strtok(getenv("LANG"),".");
		if(strncmp(lang,"zh_CN",5)==0||strncmp(lang,"pt_BR",5)==0)
			strchr(lang,'_')[0]='-';
		else lang=strtok(lang,"_");
		if(!strcmp(lang,"en")&&!strcmp(lang,"fr")&&!strcmp(lang,"zh"))
			throw runtime_error("not supported language");
		string value;
		for(auto key:keyarr){
			if((value = get(key,lang))!= "unknown")
				res.emplace_back(key,value);
		}
		return res;
	} void dump_data(){
		int status=MMDB_get_entry_data_list(&lookup_result.entry,&entry_data_list);
		if(status != MMDB_SUCCESS)
			throw std::runtime_error(MMDB_strerror(status));
		MMDB_dump_entry_data_list(stdout,entry_data_list,0);
	}
	virtual ~Mmdbcpp(){
		MMDB_free_entry_data_list(entry_data_list);
		MMDB_close(&mmdb);
	}
};
using namespace http;
extern "C" void get(
	request_message<std::vector<reqargs>>& req,
	response_message<std::string>& res
)
{
	using namespace std;
	std::string filename("/var/lib/GeoIP/GeoLite2-City.mmdb");
	std::map<std::string,std::string> amap;
	for(auto &i: static_cast<vector<reqargs>>(req)) {
		amap[i.key]= i.value;
	}
	std::string ip_address(amap["ip"]);

	try{
		Mmdbcpp handler(filename);
		handler.find_entry(ip_address);
		//handler.dump_data();
		auto info=handler.get();
		cout.setf(ios::left);
		for(auto item:info) {
			res+=item.first+" : "+ item.second+'\n';
		}
	}catch(std::runtime_error &e){
		res+="Error";
		res+=e.what();
	}
}
