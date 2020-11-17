#ifndef BUFFER_H
#define BUFFER_H
//--------------------
#include <string_view>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <shared_mutex>
#include <cerrno>
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
	//fd or conn -> buffer
	void add_msg(Message msg);
	//buffer write to fd
	ssize_t write_to_fd(int fd);

	//fd -> buffer -> conn
	//buffer -> conn
	Message msg();
	//fd -> buffer
	ssize_t read_from_fd(int fd, int num);
	ssize_t read_from_fd(int fd);
	
	int msg_num() { return msg_num_; }
	bool empty() { return msg_num_ == 0; }
	void clear();

private:
	Message &back_msg_() { return messages_.back(); }
	Message &front_msg_() { return messages_.front(); }
	char *back_msg_data_()
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
