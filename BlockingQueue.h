#ifndef BLOCKING_QUEUE
#define BLOCKING_QUEUE

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdio.h>

template <class T>
class BlockingQueue {
private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c_empty;
	std::condition_variable c_full;
	int max_queue_size;
	bool myIsDone;
	T null_value;
public:
	BlockingQueue(int size, T nv)
		: q()
		, m()
		, c_empty()
		, c_full()
		, max_queue_size(size)
		, myIsDone(false)
		, null_value(nv)
	{}

	~BlockingQueue(void)
	{}

	void push(T t) {
		std::unique_lock<std::mutex> lock(m);
		while(q.size() >= max_queue_size){
			c_full.wait(lock);
		}
		q.push(t);
		c_empty.notify_one();
	}


	T pop(void) {
		std::unique_lock<std::mutex> lock(m);
		while(q.empty() && !myIsDone) {
			c_empty.wait(lock);
		}

		T val;
		if (!q.empty()) {
			val = q.front();
			q.pop();
			c_full.notify_one();
		}
		else {
			val = null_value;
		}
		return val;   
	}

	void close() {
		std::unique_lock<std::mutex> lock(m);
		myIsDone = true;
		c_empty.notify_all();
	}
	
	int get_size() {
		return q.size();
	}
};
#endif
