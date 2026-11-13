// Traditional fixed-size thread pool: jobs always run on one of its workers.
class ThreadPool : NoCopy {
	struct Job : Moveable<Job> {
		Function<void ()> fn;
	};

	Array<Thread>      threads;
	Vector<Job>        queue;
	Mutex              lock;
	ConditionVariable  waitforjob;
	ConditionVariable  waitfordone;
	int                active = 0;
	bool               quit = false;
	std::exception_ptr exc = nullptr;
	static thread_local int worker_index;
	int64 seed_base;

	void ThreadRun(int index);

public:
	void Do(Function<void ()>&& fn);
	void Do(const Function<void ()>& fn)  { Do(clone(fn)); }
	void Finish();

	int         GetCount() const          { return threads.GetCount(); }
	static bool IsWorker()                { return worker_index >= 0; }
	static int  GetWorkerIndex()          { return worker_index; }

	// A nonnegative seed gives each worker a deterministic RNG stream.
	ThreadPool(int nthreads, int64 seed_base = -1);
	~ThreadPool();
};
