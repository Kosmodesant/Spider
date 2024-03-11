#include "Thread_pool.h"

Thread_pool::Thread_pool(const unsigned numThr) : _timeout(std::chrono::seconds(2))
{
	pool.resize(numThr);
	status.assign(numThr, { thread_mode::free, {} });
	
	for (unsigned i(0); i < numThr; ++i) {
		pool[i] = std::thread([this, i] { work(i); });
		pool[i].detach();
	}
}

Thread_pool::~Thread_pool()
{
	wait();
}

void Thread_pool::work(const unsigned idx)
{
	while (true)
	{
		const task_t task = squeue.pop();
		
		status[idx].start = std::chrono::steady_clock::now();
		status[idx].mode = thread_mode::busy;
		task();
		status[idx].mode = thread_mode::free;
	}
}

void Thread_pool::add(const task_t& task)
{
	if (task)
		squeue.push(task);
}

void Thread_pool::setTimeout(const std::chrono::seconds timeout)
{
	_timeout = timeout;
}

bool Thread_pool::isBusy()
{
	int hangsCount(0);
	for (auto& [mode, start] : status) {
		if (mode == thread_mode::busy) {
			auto diff = std::chrono::steady_clock::now() - start;
			if (diff < _timeout) {
				return true;
			}
			++hangsCount;
		}
	}

	const bool queueempty(squeue.empty());
	const bool hangs_f(hangsCount == status.size());
	const std::wstring queueStat_str = queueempty ? L"Очередь пуста" : L"Очередь занята";
	if (hangs_f) {
		std::wcout << L"Все потоки висят! (" + queueStat_str + L")\n";
	}
	else {
		if (queueempty) {
			if (hangsCount) {
				std::wcout << L"Висит потоков: " << hangsCount
					<< L", остальные простаивают ("
					+ queueStat_str + L")\n";
			}
			else std::wcout << L"Работа в потоках закончена!\n";
		}
		else {
			std::wcout << queueStat_str + L" при свободном потоке, подождем...\n";
		}
	}

	return (queueempty || hangs_f) ? false : true;
}
void Thread_pool::wait()
{
	while (isBusy()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	};
}