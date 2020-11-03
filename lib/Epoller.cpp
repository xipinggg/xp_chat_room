#include "../include/Epoller.h"
#include <cstring>
#include <cassert>
#include <unistd.h>
#include "Channel.h"
#include "Types.h"
using namespace std;

Epoller::Epoller()
    :epollfd_(epoll_create1(EPOLL_CLOEXEC))
{
	debug();
	
	assert(epollfd_!=-1);
	events_.resize(10);
}

Epoller::~Epoller()
{
	debug();
	
	::close(epollfd_);
}

int Epoller::epoll(std::vector<Channel*> &active_channels)
{
	debug();
	
	//cout<<"channels_.size(): "<<channels_.size()<<endl;
	constexpr int TIMEOUT = -1;
	assert(epollfd_ > 0);
	assert(&*events_.begin() != nullptr);
	
	int num_events = ::epoll_wait(epollfd_, &(*events_.begin()), 
	static_cast<int>(events_.size()),TIMEOUT);
	constexpr int K = 1.5;
	if (num_events == events_.size())
	{
		events_.resize(K * events_.size());
	}
    //cout<<"numEvents : "<<numEvents<<endl;
   
    if(num_events >= 0)
    {
		fill_active_channels(num_events, active_channels);
	}
	
    return num_events;
}
    
void Epoller::update_channel(Channel *channel)
{
	debug();
	
	if(!channel) 
		return;

	int fd = channel->fd();
	if(fd < 0) 
		return;	

	int update_type = channel->update_type();
	if(update_type == EPOLL_CTL_ADD)
	{
		channels_.insert({fd,channel});
		struct epoll_event event;
		bzero(&event, sizeof(event));
		event.events = channel->events();
		event.data.ptr = channel;
		
		::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
	}
	else if(update_type == EPOLL_CTL_MOD)
	{
		struct epoll_event event;
		bzero(&event, sizeof(event));
		event.events = channel->events();
		event.data.ptr = channel;
		
		::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event);
	}
	else if(update_type == EPOLL_CTL_DEL)
	{
		auto iter = channels_.find(fd);
		if(iter != channels_.end())
		{
			channels_.erase(iter);
		}
		::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
	}
}

void Epoller::fill_active_channels(int num_events, std::vector<Channel*> &active_channels)
{
	debug();
	 
	for(int i = 0; i < num_events; ++i)
	{
		Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->set_revents(events_[i].events);
		active_channels.push_back(channel);
	}
}
