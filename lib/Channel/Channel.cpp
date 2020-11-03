#include "Channel.h"
#include <sys/poll.h>
#include "Types.h"
using namespace std;

Channel::Channel(int fd)
	:fd_(fd),events_(EVENTS),update_type_(UPDATE_TYPE),revents_(0)
{
	debug();
}

Channel::~Channel()
{
	debug();
}

void Channel::set(int events,int update_type)
{
	events_ = events;
	update_type_ = update_type;
	revents_ = 0;
}

void Channel::reset()
{
	debug();
	
	set_events(EVENTS);
	set_update_type(UPDATE_TYPE);
	set_revents(0);
}

void Channel::handle_event()
{
	debug();
	
	if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
	{
		handle_close();
	}
	
	if(revents_ & (EPOLLERR | POLLNVAL))
	{
		handle_error();
	}
	
	if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		handle_read();
	}
	
	if(revents_ & EPOLLOUT)
	{
		handle_write();
	}
}