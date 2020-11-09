#ifndef BUFFER_H
#define BUFFER_H
//--------------------
#include <string_view>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <shared_mutex>

#include "Types.h"
#include "xpClass/SpinLock.hpp"

#include <jsoncpp/json/json.h>

class Buffer
{
public:
	enum Init : int
	{
		kInitialSize = 1024
	};
	Buffer();
	~Buffer();
	//conn -> buffer -> fd
	void add_msg(Message msg);//fd or conn -> buffer
	ssize_t write_to_fd(int fd);//buffer write to fd

	//fd -> buffer -> conn
	Message msg();//buffer -> conn
	ssize_t read_from_fd(int fd, int num);//fd -> buffer

	int msg_num() { return msg_num_; }
	bool empty() { return msg_num_ == 0; }
	void clear();

	//ssize_t write(Buf buf, int threadId);
	//ssize_t read(int fd, size_t num);

	//void reset();
	//void swap(Buffer &rhs);

private:
	Message &back_msg() { return messages_.back(); }
	Message &front_msg() { return messages_.front(); }
	char *back_msg_data()
	{
		char *data = messages_.back().data();
		return data;
	}

private:
	std::list<Message> messages_;
	std::shared_mutex mtx_;
	SpinLock messages_lock_;
	int msg_num_;
};

#endif
