#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <future>
#include <queue>
#include <atomic>

enum  _State
{
		NOT_READY = 0,
		READY,
		DEAD
};


class ThreadPool
{
	// copy and assignment not allowed
	void operator=( const ThreadPool&) = delete;
	ThreadPool( const ThreadPool&) = delete;

	const int thread_num;

	typedef std::function<void()> Task;
	typedef std::promise<Task> Promise;
	std::vector<Promise> promises;
	
	std::vector<std::thread> threads;

	std::mutex q_lock;
	std::queue<Task> task_queue;

	std::mutex f_lock;

	std::vector<_State> flags; // 0 - isn't ready, 1 - is ready, 2 - is dead 

	std::promise<void> ready_promise;

	std::thread sched;

	void worker( int i);

	int dead_processes;

	void scheduler();
	bool start;
public:
	ThreadPool( int n);
	~ThreadPool();
	void post( Task task);
};

ThreadPool::ThreadPool( int n) : thread_num( n)
{
	ready_promise = std::promise<void>();

	dead_processes = 0;

	for ( int i = 0; i < n; i++)
	{
		promises.push_back( Promise());
		flags.push_back( READY);
		threads.push_back( std::thread( &ThreadPool::worker, this, i));		
	}

	sched = std::thread( &ThreadPool::scheduler, this);
}

ThreadPool::~ThreadPool()
{
	//std::cerr << "destructor" << std::endl;
	for ( int i = 0; i < thread_num; i++)
		post( nullptr);

	for ( int i = 0; i < thread_num; i++)
		threads[ i].join();


	sched.join();
}

void ThreadPool::post( Task task)
{
	std::lock_guard<std::mutex> lock( q_lock);
	task_queue.push( task);
	//std::cerr << "post ...\n";
	try
	{
		ready_promise.set_value();
	}
	catch( ... ){};
}

void ThreadPool::scheduler()
{
	while( true)
	{
		f_lock.lock();
		q_lock.lock();
		for ( int i = 0; i < thread_num; i++)
		{
			if ( flags[ i] == DEAD)
				dead_processes++;

			if ( flags[ i] && !task_queue.empty())
			{
				//std::cerr << "Scheduler! set ...\n";
				promises[ i].set_value( task_queue.front());
				task_queue.pop();
				flags[ i] = NOT_READY;
			}
			ready_promise = std::promise<void>();
		}
		q_lock.unlock();
		f_lock.unlock();
		//std::cerr << "Scheduler! wait ...\n";
		if ( dead_processes == thread_num)
			break;
		else
			ready_promise.get_future().wait();
	}
}

void ThreadPool::worker( int i)
{
	while( true)
	{	
		//std::cerr << "Worker" << i << "! get ...\n";
		Task task = promises[ i].get_future().get();
		promises[ i] = Promise();
		//std::cerr << "Worker" << i << "! ready ...\n";
		f_lock.lock();
		//std::cerr << "Worker! flag ...\n";
		if ( task == nullptr)
		{
			flags[ i] = DEAD;
		}
		else
		{
			flags[ i] = READY;
			task();
		}

		try
		{
			ready_promise.set_value();
		}
		catch ( ... ){};
		f_lock.unlock();

		if ( task == nullptr)
			break;
	}
}