#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
//-----------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <functional>
#include <memory>
#include "Buffer.h"
//-----------------------------------------
class TcpServer;
class ConnChannel;
class Channel;
class EventLoop;

class TcpConnection:public std::enable_shared_from_this<TcpConnection>
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
	//set and get
	int fd() const { return fd_; }
	void set_fd(sockaddr_in addr){ peer_addr_ = addr; }
	sockaddr_in addr() const { return peer_addr_; }
	Channel* channel() const { return (Channel*)conn_channel_.get(); }
	std::weak_ptr<ConnChannel> weak_channel() const { return conn_channel_; }
	void set_nonblock_fd();
	bool connecting() const { return connecting_; }
	void set_connecting(bool status){ connecting_ = status; }
	void reset();
	void restart(sockaddr_in addr, EventLoop *loop);
	void init_conn_channel();
protected:
	int fd_;
	sockaddr_in peer_addr_;
public:
	Buffer input_buffer_;//read
	Buffer output_buffer_;//write
protected:
	bool connecting_;
	std::shared_ptr<ConnChannel> conn_channel_;
	TcpServer *server_;
	EventLoop *loop_;
	
};
#endif
