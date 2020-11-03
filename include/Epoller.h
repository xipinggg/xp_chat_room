#ifndef EPOLLER_H
#define EPOLLER_H
//------------------
#include <vector>
#include <map>
//------------------
struct epoll_event;
class Channel;

class Epoller
{ 
public:
    Epoller();
    virtual ~Epoller();
    int epoll(std::vector<Channel*> &active_channels);
    void update_channel(Channel *channel);
    void fill_active_channels(int num_events, std::vector<Channel*> &active_channels);
    int epollfd() const { return epollfd_; }
private:
    std::map<int, Channel*> channels_;
    std::vector<struct epoll_event> events_;
    int epollfd_;
};

#endif
