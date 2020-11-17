#ifndef TCPSERVER_H
#define TCPSERVER_H
//----------------------------------------
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
//#include <ranges>
#include "../include/ListenSocket.h"
#include "../include/xpClass/ObjectPool.hpp"
#include "../include/xpClass/TowBuffer.hpp"
//----------------------------------------
class Channel;
class TcpConnection;
class EventLoop;
struct sockaddr_in;

class TcpServer : public std::enable_shared_from_this<TcpServer>
{
public:
	enum Init : int
	{
		IO_THREAD_COUNT = 4,
		LISTEN_LOOP_ID = 0
	};
	TcpServer(int count = IO_THREAD_COUNT);
	virtual ~TcpServer();
	void start();
	//
	virtual void on_connect(int fd, sockaddr_in addr);
	virtual void on_message(int fd, std::shared_ptr<TcpConnection> conn, Message msg, int loop_id);
	virtual void on_close(int fd, std::shared_ptr<TcpConnection> conn, int loop_id);
	virtual void on_error(int fd, std::shared_ptr<TcpConnection> conn, int loop_id);
	virtual void on_error();
	virtual void on_write_complete(int fd);
	//20-11-15 未实现
	void add_msg_to_all(Message msg, int fd_from, int fd_not_in, int loop_id);
	//20-11-15 未实现
	void add_msg_to_one(Message msg, int fd_from, int fd_to, int loop_id);

	int fd() const { return socket_.fd(); }
	int max_fd();
	int io_thread_count() const { return io_thread_count_; }

private:
	EventLoop *select_loop(int connfd);
	//未实现
	bool add_conn(const int fd, std::shared_ptr<TcpConnection>);
	//未实现
	bool del_conn(const int fd);

private:
	const int io_thread_count_;
	int loops_index_;
	ListenSocket socket_;
	std::shared_ptr<Channel> listen_channel_;
	std::vector<std::shared_ptr<EventLoop>> loops_;
	std::map<int, std::shared_ptr<TcpConnection>> connections_;
	ObjectPool<TcpConnection> conn_pool_;
	std::shared_mutex conn_pool_mutex_;
	std::shared_ptr<EventLoop> listen_loop_;
};

#endif
