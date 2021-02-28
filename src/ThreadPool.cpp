#include <future>
#include <cerrno>
#include <exception>
#include <iostream>
#include <system_error>
#include <type_traits>
#include <ThreadPool.hpp>

namespace threadpool {
ThreadPool::ThreadPool(int workercnt_):
	workercnt(workercnt_),mvec(workercnt_),cv_jobaddvec(workercnt_),
	list(workercnt_,std::list<std::function<void()>>())
{
	taskcnt.store(0);
	for(int i = 0; i < workercnt; i++) {
		worker.emplace_back([&, i]()noexcept {
			do {
				try {
					std::unique_lock<std::mutex> l(mvec[i]);
					if(list[i].empty()){
						cv_jobaddvec[i].wait(l, [&]{ return !list[i].empty(); });
					}
					l.unlock();
					if(taskcnt.load()!=-1){
						list[i].front()();
						std::unique_lock<std::mutex> l(mvec[i]);
						if(list[i].size() <= 1) {
							list[i].pop_front();
							l.unlock();
						}else {
							l.unlock();
							list[i].pop_front();
						}
						taskcnt--;
					}else {
						workercnt--;
						break; //exit thread
					}
				}catch(...) {
					//throw std::current_exception();
					std::cout<<"exception caught\n";
					std::terminate();
				}
			} while(true);
		});
	}
	setcleaner();

}

void ThreadPool::setcleaner(bool soft) {
	if(soft) {
		cleaner = [](std::thread & t) {
			t.join();
		};
	} else {
		cleaner = [](std::thread & t) {
			t.~thread();
		};
	}
}
ThreadPool::~ThreadPool() {
	taskcnt.store(-1);
	int maxwokercnt=workercnt.load();
	while(workercnt.load()){
		for(int i=0;i<maxwokercnt;i++)
			cv_jobaddvec[i].notify_one();
	}
	for(auto &it : worker) {
		cleaner(it);
	}
}
}
