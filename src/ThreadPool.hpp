#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP
#include <list>
#include <thread>
#include <memory>
#include <vector>
#include <future>
#include <functional>
#include <condition_variable>
#ifdef DEBUG
#include <iostream>
#endif

namespace threadpool {
template< class T >
using result_of_t = typename std::result_of<T>::type;
class ThreadPool {
public:
	ThreadPool(int workercnt_ = 5);
	template<class F, class ...Args>
	//std::future<result_of_t<F(Args...)>> 
	auto emplace_back(F&& f, Args&&... args) 
	-> std::future< result_of_t<decltype(f)(Args...)>>;
	void setcleaner(bool soft = true);
	~ThreadPool();
private:
	std::atomic<int> workercnt;
	std::vector<std::mutex> mvec;
	std::list<std::thread> worker;
	std::vector<std::condition_variable> cv_jobaddvec;
	std::atomic<int> taskcnt;
	std::vector<std::list<std::function<void()>>> list;
	std::function<void(std::thread &)> cleaner;
};
template<class F, class ...Args>
//std::future<result_of_t<F(Args...)>> 
auto ThreadPool::emplace_back(F&& f, Args&&... args) 
	-> std::future<result_of_t<decltype(f)(Args...)> >
{
    using RetType=typename std::result_of<decltype(f)(Args...)>::type;
	std::shared_ptr<std::packaged_task<RetType()>> tp=std::make_shared<std::packaged_task<RetType()>>(std::bind(f,args...));

	int thread_id = taskcnt.load()%workercnt;
	std::unique_lock<std::mutex> l(mvec[thread_id]);
	if(list[thread_id].size() <= 1) {
		list[thread_id].push_back([tp]{ (*tp)(); });
		l.unlock();
	} else {
		l.unlock();
		list[thread_id].push_back([tp]{ (*tp)(); });
	}

	cv_jobaddvec[thread_id].notify_one();
	taskcnt++;
	return tp->get_future();
}
}
#endif
