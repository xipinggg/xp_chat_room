#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
//-----------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <functional>
#include <memory>

#include "Buffer.h"
#include "Types.h"
//-----------------------------------------
class TcpServer;
class ConnChannel;
class Channel;
class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection();
	TcpConnection(int fd, sockaddr_in addr, TcpServer *server, EventLoop *loop);
	~TcpConnection();
	void set(int fd, sockaddr_in addr, TcpServer *server, EventLoop *loop);
	//
	virtual void handle_read();
	virtual void handle_write();
	virtual void handle_close();
	virtual void handle_error();
	bool add_msg(Message msg);
	//set and get
	int fd() const { return fd_; }
	sockaddr_in addr() const { return addr_; }
	Channel *channel() const { return conn_channel_.get(); }
	std::weak_ptr<ConnChannel> weak_channel() const { return conn_channel_; }
	bool connecting() const { return connecting_; }
	void set_connecting(bool status) { connecting_ = status; }
	void reset();
	void restart(sockaddr_in addr, EventLoop *loop);
	void init_conn_channel();

protected:
	bool is_get_msg_head() { return current_msg_.left >= lenth::MSG_HEAD; }
	int get_msg_head();
	void add_new_msg_to_buffer();
	bool is_msg_complete() { return current_msg_.left + 1 == current_msg_.right; }
	void on_msg_complete();

protected:
	int fd_;
	sockaddr_in addr_;
	Buffer input_buffer_;  //read
	Buffer output_buffer_; //write
	bool connecting_;
	std::shared_ptr<ConnChannel> conn_channel_;
	TcpServer *server_;
	EventLoop *loop_;
	char msg_head_[lenth::MSG_HEAD + 1];//20-11-10 TODO char [4]
	Message current_msg_;
	bool is_read_msg_head_;
};
#endif