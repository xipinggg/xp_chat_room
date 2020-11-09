#include <atomic>

class SpinLock
{
public:
	SpinLock():locking_(ATOMIC_FLAG_INIT){}
	~SpinLock();
	void lock(){ while(locking_.test_and_set()); }
	bool try_lock(){ return locking_.test_and_set(); }
	void unlock(){ locking_.clear(); }
private:
	std::atomic_flag locking_;
};

class SpinLockGuard
{
public:
	SpinLockGuard(SpinLock &lock) : lock_(lock)
	{
		lock_.lock();
	}
	~SpinLockGuard()
	{
		lock_.unlock();
	}
private:
	SpinLock &lock_;
};
