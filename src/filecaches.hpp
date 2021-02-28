#ifndef FILE_CACHES_HPP
#include <unordered_map>
#include <file.hpp>
#include <list>

namespace simpleweb {
struct fileNode {
	std::string name;
	int fd;
	fileNode(const std::string& filename, int fd):name(filename), fd(fd) { }
};
class filecaches {
	std::unordered_map<std::string, std::list<fileNode>::iterator> cachemap_;
	std::list<fileNode> cachelist_;
	int capacity;
public:
	filecaches(int c):capacity(c){}
	std::list<fileNode>::iterator get(const std::string& filepath) {
		if(cachemap_.find(filepath)==cachemap_.end()) {
			return cachelist_.end();
		}
		cachelist_.splice(cachelist_.begin(),cachelist_,cachemap_[filepath]);
		cachemap_[filepath] = cachelist_.begin();
		return cachemap_[filepath];
	}
	void set(std::string const& filepath,int fd) {
		if( cachemap_.find(filepath) == cachemap_.end() ) {
			if(cachelist_.size() == capacity) {
				cachemap_.erase(cachelist_.back().name);
				cachelist_.pop_back();
			}
			cachelist_.push_front(fileNode(filepath,fd));
		}else {
			cachemap_[filepath]->fd = fd;
			cachelist_.splice(cachelist_.begin(),cachelist_, cachemap_[filepath]);
		}
		cachemap_[filepath] = cachelist_.begin();
	}
	std::list<fileNode>::iterator end() {
		return cachelist_.end();
	}
};

}
#endif
