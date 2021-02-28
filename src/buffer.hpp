class Buffer{
	static int use_count_;
	static char *buffer_;
public:
	Buffer(){
		use_count_++;
		if(buffer_ == nullptr){
			buffer_ = new char[1024];
		}
	}
	~Buffer(){
		use_count_--;
	}
};
