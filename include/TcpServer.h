#ifndef TCPSERVER_H
#define TCPSERVER_H
//----------------------------------------
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include "../include/xpClass/ObjectPool.hpp"
#include "../include/xpClass/TowBuffer.hpp"
//----------------------------------------
class ListenSocket;
class Channel;
class TcpConnection;
class EventLoop;
struct sockaddr_in;

class TcpServer:public std::enable_shared_from_this<TcpServer>
{
public:
	enum Init:int{IO_THREAD_COUNT = 4};
	TcpServer(int count = IO_THREAD_COUNT);
	virtual ~TcpServer();
	void start();
	//
	virtual void on_new_connection(int fd,sockaddr_in addr);
	virtual void on_message(int fd,std::shared_ptr<TcpConnection> conn,
							EventLoop *loop);
	virtual void on_close(int fd,std::shared_ptr<TcpConnection> conn,
							std::weak_ptr<EventLoop> loop);
	virtual void on_error();
	virtual void onWriteComplete(int fd);
	
	std::shared_ptr<EventLoop> select_loop(int connfd);
	int fd() const;
	int max_fd();
	int io_thread_count() const { return io_thread_count_; }
private:
	const int io_thread_count_;
	int loops_index_;
	std::shared_ptr<ListenSocket> socket_;
	std::shared_ptr<Channel> listen_channel_;
	std::vector<std::shared_ptr<EventLoop>> loops_; 
	std::map<int,std::shared_ptr<TcpConnection>> connections_;
	ObjectPool<TcpConnection> conn_pool_;
	std::shared_mutex conn_pool_mutex_;
};

#endif
