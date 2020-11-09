#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>
#include <vector>
#include <memory>
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

struct Message
{
	std::shared_ptr<std::vector<char>> str;
	int left;
	int right;
	
	int str_size()
	{
		return right - left;
	}

	char *data()
	{
		return str.get()->data() + left;
	}
};

#endif //#ifndef _TYPES_H_