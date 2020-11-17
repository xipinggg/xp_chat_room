#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>
#include <vector>
#include <memory>
#include <fcntl.h>

//#define NODEBUG

enum lenth : int
{
	MSG_HEAD = 4
};

#ifndef NODEBUG
#define debug()                    \
	std::cout << __FILE__ << " : " \
			  << __func__ << "---------" << __LINE__ << "\n"
#else
#define debug()
#endif //#ifndef NODEBUG

static inline int set_nonblock_fd(int fd)
{
	//20-11-11
	//TODO
	//test flags
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1 or flags & O_NONBLOCK)
		return flags;
	else
		return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

struct Message
{
	std::shared_ptr<std::vector<char>> str;
	int left;
	int right;

	static Message &make(string s, int fd_send);
	int str_size() { return right - left; }
	char *data() { return str->data() + left; }
	void reset()
	{
		str.reset();
		left = 0;
		right = 0;
	}
};

#endif //#ifndef _TYPES_H_