#include <vector>
#include <atomic>
#include <functional>
#include <iostream>
#include <cassert>
#include <unistd.h>

template<typename T>
class TowBuffer
{
public:
	TowBuffer(std::function<void (T&)> reader);
	~TowBuffer();
	void write(T t);
	void read_all();
	bool empty();
private:
	void lock();
	void unlock();
	void swap();
	
	std::vector<T> write_buf_;
	std::vector<T> read_buf_;
	int read_index_;
	std::atomic_flag working_;
	std::function<void (T)> reader_;
};

template<typename T>
TowBuffer<T>::TowBuffer(std::function<void (T&)> reader)
	:working_(ATOMIC_FLAG_INIT),
	reader_(reader)
{
	
}
	
template<typename T>
TowBuffer<T>::~TowBuffer()
{
	
}

template<typename T>
void TowBuffer<T>::write(T t)
{
	lock();
	write_buf_.push_back(t);
	unlock();
}

template<typename T>
void TowBuffer<T>::read_all()
{
	if(write_buf_.empty())
		return;
		
	swap();
	for(auto &e:read_buf_)
	{
		reader_(e);
	}
	read_buf_.clear();
}

template<typename T>
void TowBuffer<T>::lock()
{
	while(working_.test_and_set());
}

template<typename T>
void TowBuffer<T>::unlock()
{
	working_.clear();
}

template<typename T>
void TowBuffer<T>::swap()
{
	read_index_ = 0;
	read_buf_.clear();
		
	lock();
	read_buf_.swap(write_buf_);
	unlock();
}

template<typename T>
bool TowBuffer<T>::empty()
{	
	if(read_buf_.empty() && write_buf_.empty())
		return true;
	else 
		return false;
}
