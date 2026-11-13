#include "Ext.h"

NAMESPACE_UPP

thread_local int ThreadPool::worker_index = -1;

ThreadPool::ThreadPool(int nthreads, int64 seed_base_)
{
	seed_base = seed_base_;
	for(int i = 0; i < nthreads; i++)
		CHECK(threads.Add().RunNice([=] {
			worker_index = i;
			if(seed_base >= 0)
				SeedRandom((dword)(seed_base + i));
			ThreadRun(i);
		}, true));
}

ThreadPool::~ThreadPool()
{
	lock.Enter();
	quit = true;
	lock.Leave();
	waitforjob.Broadcast();
	for(int i = 0; i < threads.GetCount(); i++)
		threads[i].Wait();
}

void ThreadPool::ThreadRun(int index)
{
	lock.Enter();
	for(;;) {
		while(queue.IsEmpty() && !quit)
			waitforjob.Wait(lock);
		if(queue.IsEmpty()) {
			lock.Leave();
			return;
		}
		Function<void ()> fn = pick(queue[0].fn);
		queue.Remove(0);
		lock.Leave();

		try {
			fn();
		}
		catch(...) {
			lock.Enter();
			if(!exc)
				exc = std::current_exception();
			lock.Leave();
		}

		lock.Enter();
		if(--active == 0)
			waitfordone.Broadcast();
	}
}

void ThreadPool::Do(Function<void ()>&& fn)
{
	Mutex::Lock __(lock);
	Job& job = queue.Add();
	job.fn = pick(fn);
	active++;
	waitforjob.Signal();
}

void ThreadPool::Finish()
{
	lock.Enter();
	while(active)
		waitfordone.Wait(lock);
	std::exception_ptr e = pick(exc);
	lock.Leave();
	if(e)
		std::rethrow_exception(e);
}

END_UPP_NAMESPACE
