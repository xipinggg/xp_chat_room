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
}
