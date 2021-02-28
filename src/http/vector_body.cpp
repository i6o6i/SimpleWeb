#include <http/vector_body.hpp>
#include <http/utils.hpp>
#include <cctype>

namespace http{

template<>
uint64_t vector_body<reqargs>::size() {
	uint64_t size_;
	if(size_ < 0) {
		size_ = 0;
		for(auto i:content_) {
			size_+=i.key.size()+i.value.size();
		}
	}
	return size_;
}

template<>
int64_t vector_body<reqargs>::read(std::istream& is, vector_body<reqargs>& body_handler) {
	int64_t totalsize=0;
	char name[MAXFIELDNAME];
	char value[MAXFIELDVALUE];
	enum {
		s_name,
		s_value,
		s_almost_done,
		//s_done,
	} state;
	u_char ch,*p, *end;
	state = s_name;

	int i=0;
	for(;totalsize<MAXTRUNK&&is.get((char&)ch);) {
		totalsize++;
		switch(state) {
			case s_name:
				if(usual[ch >> 5] & (1U << (ch & 0x1f))&&ch!='&'&&ch!='=') {
					name[i++] = ch;
					break;
				}
				switch(ch) {
					case '=':
						name[i] ='\0';
						state = s_value; 
						i=0;
						break;
					case '&':
						name[i] ='\0';
						body_handler.content_.push_back({
								std::move(std::string(name)),
								std::move(std::string())
							});
						i=0;
						break;
					default:
						return -1;
				}
				break;
			case s_value:
				if(isprint(ch)&&ch!='&'&&ch!=' ') {
					value[i++] = ch;
					break;
				}
				switch(ch) {
					case '&':
						value[i]='\0';
						body_handler.content_.push_back({
								std::move(std::string(name)),
								std::move(std::string(value))
							});
						state =s_name;
						i=0;
						break;
					//case space for GET
					case ' ':
						value[i]='\0';
						body_handler.content_.push_back({
								std::move(std::string(name)),
								std::move(std::string(value))
							});
						return totalsize;
					case CR:
						value[i]='\0';
						body_handler.content_.push_back({
								std::move(std::string(name)),
								std::move(std::string(value))
							});
						state = s_almost_done;
						break;
					case LF:
						value[i]='\0';
						body_handler.content_.push_back({
								std::move(std::string(name)),
								std::move(std::string(value))
							});
						state = s_name;
						break;
					default: return -1;
				}
			case s_almost_done:
				if(ch == CR)
					break;
				if(ch == LF)
					state = s_name;
				break;
		}
	}
	return totalsize;
}
template<>
int64_t vector_body<reqargs>::write(std::ostream& os, vector_body<reqargs> const& vb) {
	if(vb.content_.size())
		os<<vb.content_[0].key<<'='<<vb.content_[0].value;
	for(int i=1;i<vb.content_.size();i++) {
		os<<'&'<<vb.content_[i].key<<'='<<vb.content_[i].value;
	}
	return 0;
}
}
