#include "EventLoop.h"
#include <cassert>
#include <unistd.h>
#include <sys/eventfd.h>
#include "Epoller.h"
#include "WakeupChannel.h"
#include "Types.h"
using namespace std;

int creat_eventfd();

EventLoop::EventLoop(int id,string name)
	:id_(id),
	name_(name),
	wakeup_fd_(creat_eventfd()),
	wakeup_channel_(new WakeupChannel(wakeup_fd_)),
	epoller_(new Epoller),
	tasks_([](std::function<void ()> &t){ t(); })
{
	debug();

	update_channel(wakeup_channel_.get());
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
	while(looping_)
	{
		active_channels_.clear();
		int num_events = epoller_->epoll(active_channels_);
		
		event_handling_ = true;
		//cout<<"activeChannels.size:  "<<activeChannels_.size()<<endl;
		for(auto channel:active_channels_)
		{
			channel->handle_event();//fd?
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

void EventLoop::add_task(std::function<void()> func, int id = -1)
{
	debug();

	tasks_.write(func);
	wakeup();
}

void EventLoop::do_task()
{
	debug();
	
	tasks_.read_all();
}

int creat_eventfd()
{
	debug();
	
	int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	//cout<<"  eventfd:   "<<fd<<endl;
	assert(fd >= 0);
	return fd;
}

void EventLoop::update_channel(Channel *channel, int id)
{
	debug();

	if(!channel)
		return;
	
	auto func = [channel,this]
	{
		this->epoller_->update_channel(channel);
	};

	if(id == this->id() || id == -1)
	{
		func();
	}
	else
	{
		add_task(func);
	}
}

void EventLoop::stop()
{
	debug();
	
	looping_ = false;
}
