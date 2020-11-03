#include <memory>
#include <vector>
#include <concepts>
#include "SpinLock.hpp"

namespace
{
template<typename T>
concept Setable = requires(T t)
{
	&T::set;
	//std::is_same<decltype(&T::set),void>::value;
};
}

template<Setable T>
class ObjectPool
{
public:
	enum {INITSIZE = 32};
	ObjectPool();
	~ObjectPool() = default;
	
	template<typename ...Args>
	std::shared_ptr<T> get(Args... args);

	void recycle(std::shared_ptr<T> &t);
	bool empty() const { return pool_.empty(); }
	size_t size() const { return pool_.size(); }
	size_t total_size() const { return total_size_; }
	
private:
	std::vector<std::shared_ptr<T>> pool_;
	size_t total_size_;
	SpinLock lock_;
};

template<Setable T>
ObjectPool<T>::ObjectPool()
	:pool_(INITSIZE),
	total_size_(0),
	lock_()
{
	
}

template<Setable T> template<typename ...Args>
std::shared_ptr<T> ObjectPool<T>::get(Args... args)
{
	if(empty())
	{
		++total_size_;
		return std::make_shared<T>(args...);
	}
	else
	{
		lock_.lock();
		auto t = pool_.back();
		pool_.erase(--pool_.end());
		lock_.unlock();
		
		t->set(args...);
		return t;
	}
}

template<Setable T>
void ObjectPool<T>::recycle(std::shared_ptr<T> &t)
{
	lock_.lock();
	pool_.emplace_back(std::move(t));
	lock_.unlock();
}
