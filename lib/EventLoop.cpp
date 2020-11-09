#include "EventLoop.h"
#include <cassert>
#include <unistd.h>
#include <sys/eventfd.h>
#include "Epoller.h"
#include "WakeupChannel.h"
#include "Types.h"
using namespace std;

int creat_eventfd();

EventLoop::EventLoop(int id, string name)
	: id_(id),
	  name_(name),
	  wakeup_fd_(creat_eventfd()),
	  wakeup_channel_(new WakeupChannel(wakeup_fd_)),
	  epoller_(new Epoller),
	  tasks_([](std::function<void()> t) { t(); })
{
	debug();

	update_channel(wakeup_channel_.get(), this->id());
}

EventLoop::~EventLoop()
{
	debug();
}

void EventLoop::loop()
{
	debug();
	//cout<<"thread  id:  "<<id_<<endl;
	looping_ = true;
	while (looping_)
	{
		active_channels_.clear();
		int num_events = epoller_->epoll(active_channels_);

		event_handling_ = true;
		//cout<<"activeChannels.size:  "<<activeChannels_.size()<<endl;
		for (auto channel : active_channels_)
		{
			channel->handle_event(); //fd?
		}
		event_handling_ = false;

		do_task();

		++iteration_;
//cout<<"-----------iteration: "<<iteration_<<" ----thread  id:  "<<id_<<endl;
#ifndef NODEBUG
		::sleep(1);
#endif
	}
}

void EventLoop::wakeup()
{
	debug();

	uint64_t one = 1;
	::write(wakeup_fd_, &one, sizeof one);
}

bool EventLoop::add_task(std::function<void()> func)
{
	debug();

	tasks_.write(func);
	wakeup();
	return true;
}

int EventLoop::do_task()
{
	debug();

	/* 20-11-05
	 * TODO: do tasks and return the num of tasks have done
	 */
	tasks_.read_all();
	return 0;
}

int creat_eventfd()
{
	debug();

	int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	assert(fd >= 0);
	return fd;
}

int EventLoop::update_channel(Channel *channel, const int loop_id)
{
	debug();

	if (!channel)
		return -2;

	int ret = 0;
	auto func = [channel, this, &ret]() {
		ret = this->epoller_->update_channel(channel);
	};

	if (loop_id == this->id())
	{
		func();
		return ret;
	}
	else
	{
		add_task(func);
		return 1;
	}
}

void EventLoop::stop()
{
	debug();

	looping_ = false;
}
