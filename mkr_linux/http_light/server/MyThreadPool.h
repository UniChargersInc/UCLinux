#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#if 0
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};
 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
        );
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
#else
#include <pthread.h>
//using _HttpServer = httplib::HttpServer;


class MyThreadPool
{
public:
	MyThreadPool(int m_threads)
	{
		terminate=false;
		this->m_threads = m_threads;
		//this->owner = owner;
		for (int i = 0; i < m_threads; i++) {
			pthread_t tid;
			pthread_create(&tid, NULL, workerFunc, this);
			
			tidV.push_back(tid);
		}

	}
	static void *workerFunc(void *this_)
	{
		MyThreadPool *_this = (MyThreadPool *)this_;
		for (;;)
		{
			int sock = -1;
			{
				std::unique_lock<std::mutex> lock(_this->queue_mutex);

				_this->condition.wait(lock,
					[_this] { return _this->terminate || !_this->socketsV.empty(); });

				if (_this->terminate && _this->socketsV.empty())
					return NULL;
				sock = _this->socketsV.front();
				_this->socketsV.pop();
			}
			/*httplib::HttpServer* svr = (httplib::HttpServer*)_this->owner;
			if (svr && sock!=-1)
			{
				bool rv = svr->read_and_close_socket(sock);
			}*/
			bool execute_thread_function(int sock);
			execute_thread_function(sock);
		}
		return NULL;
	}
	void submit(int sock)
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			socketsV.emplace(sock);
		}
		condition.notify_one();
	}
	void shutdown()
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			terminate = true;
		}
		condition.notify_all();
		for (auto tid : tidV)
			pthread_join(tid, NULL);
	}
private:
	bool terminate;
	int m_threads;
	//void *owner;
	std::vector<pthread_t> tidV;
	std::queue<int > socketsV;

	std::mutex queue_mutex;
	std::condition_variable condition;
};
#endif
#endif
