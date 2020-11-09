#ifndef _CHANNEL_H
#define _CHANNEL_H
//---------------------------------------
#include <sys/epoll.h>
//---------------------------------------
class Channel
{
public:
	enum INIT : int
	{
		FD = -1,
		EVENTS = EPOLLIN | EPOLLPRI | EPOLLHUP,
		UPDATE_TYPE = EPOLL_CTL_ADD
	};

	Channel(int fd = FD);
	virtual ~Channel();

	void handle_event();
	virtual void handle_read() {};
	virtual void handle_write() {};
	virtual void handle_close() {};
	virtual void handle_error() {};
	//set and get
	void set(int events = EVENTS, int update_type = UPDATE_TYPE);
	void reset();
	void set_fd(int fd) { fd_ = fd; }
	int fd() const { return fd_; }
	void set_events(int events) { events_ = events; }
	int events() const { return events_; }
	void set_revents(int revt) { revents_ = revt; }
	int revents() const { return revents_; }
	void set_update_type(int no) { update_type_ = no; }
	int update_type() { return update_type_; }

protected:
	int fd_;
	int events_;
	int update_type_;
	int revents_;
};

#endif
