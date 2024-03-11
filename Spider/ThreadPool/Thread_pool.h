#pragma once

#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include "Safe_queue.hpp"

using task_t = std::function<void()>;
class Thread_pool
{
private:
	enum class thread_mode { free, busy };
	struct Status {
		thread_mode mode;
		std::chrono::steady_clock::time_point start;
	};
	std::vector<std::thread> pool;
	std::vector<Status> status;
	Safe_queue<task_t> squeue;	
	std::chrono::seconds _timeout;

	void work(const unsigned idx);
	bool isBusy();

public:
	Thread_pool(const unsigned numThr);
	~Thread_pool();

	void add(const task_t& task);

	void setTimeout(const std::chrono::seconds timeout);

	void wait();
};