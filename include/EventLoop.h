#ifndef EVENTLOOP_H
#define EVENTLOOP_H
//-------------------------------
#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include "xpClass/TowBuffer.hpp"
//-------------------------------
class Channel;
class Epoller;

class EventLoop
{
public:
	EventLoop(int id = -1, std::string name = "");
	virtual ~EventLoop();
	virtual void loop();
	void stop();
	//
	void add_task(std::function<void()> func, int id = -1);
	void wakeup();
	void update_channel(Channel *channel, int id = -1);
	//get
	int id() const { return id_; }
	std::string name() const { return name_; }
	int iteration() const { return iteration_; }
	bool looping() const { return looping_; }
	bool event_handling() const { return event_handling_; }
private:
	void do_task();
private:
	const int id_;
	std::string name_;
	int iteration_;
	int wakeup_fd_;
	std::atomic<bool> looping_;
	std::atomic<bool> event_handling_;
	std::shared_ptr<Channel> wakeup_channel_;
	std::vector<Channel*> active_channels_;
	std::shared_ptr<Epoller> epoller_;

	TowBuffer<std::function<void()>> tasks_;
};

#endif
