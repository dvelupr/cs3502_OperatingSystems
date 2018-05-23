#pragma once

#include <mutex>
#include <queue>
#include <thread>

template<class T>
class SafeQueue
{
public:
	SafeQueue()
	{}

	~SafeQueue()
	{}

	// Copy constructor
	SafeQueue(const SafeQueue &rhs)
	{
		std::lock_guard<std::mutex> lock(mut);
		q = rhs.getQueue();
	}

	SafeQueue& operator=(const SafeQueue &rhs)
	{
		std::lock_guard<std::mutex> lock(mut);
		q = rhs.getQueue();

		return *this;
	}

	bool empty() const
	{
		return q.empty();
	}

	T pop()
	{
		T retval;

		std::unique_lock<std::mutex> locker(mut);
		mut_condition.wait(locker, [&]() { return !q.empty(); });
		retval = q.front();
		q.pop();
		locker.unlock();

		return retval;
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> locker(mut);
		q.push(item);
		locker.unlock();

		mut_condition.notify_all();
	}

	int size() const
	{
		return q.size();
	}
	
	std::queue<T> getQueue() const
	{
		return q;
	}

private:
	std::queue<T> q;
	std::mutex mut;
	std::condition_variable mut_condition;
};
