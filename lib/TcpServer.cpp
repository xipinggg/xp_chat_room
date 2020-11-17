#include "TcpServer.h"

#include <cassert>
#include <algorithm>
#include <thread>

#include "ListenChannel.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "ConnChannel.h"
#include "Types.h"

using namespace std;
//using namespace std::ranges;
//using namespace std::placeholders;

TcpServer::TcpServer(int io_thread_count)
	: io_thread_count_(io_thread_count),
	  loops_index_(0),
	  socket_(),
	  listen_channel_(new ListenChannel(this)),
	  loops_(1 + io_thread_count),
	  connections_(),
	  conn_pool_(),
	  conn_pool_mutex_()
{
	debug();

	loops_[LISTEN_LOOP_ID] = make_shared<EventLoop>(LISTEN_LOOP_ID, "listen_loop");
	listen_loop_ = loops_[LISTEN_LOOP_ID];

	int index = 1 + LISTEN_LOOP_ID;
	while (index <= io_thread_count_)
	{
		loops_[index] = make_shared<EventLoop>(index, "io_loop");
		++index;
	}
}

TcpServer::~TcpServer()
{
	debug();
}

void TcpServer::start()
{
	debug();

	Channel *channel = listen_channel_.get();
	listen_loop_->update_channel(channel, listen_loop_->id());
	for (int i = 1 + LISTEN_LOOP_ID; i < loops_.size(); ++i)
	{
		thread t(&EventLoop::loop, loops_[i].get());
		t.detach();
	}
	listen_loop_->loop();
}

void TcpServer::on_connect(int fd, sockaddr_in addr)
{
	debug();

	set_nonblock_fd(fd);
	EventLoop *loop = select_loop(fd);
	auto conn = conn_pool_.get(fd, addr, this, loop);
	if (add_conn(fd, conn))
	{
		loop->update_channel(conn->channel(), listen_loop_->id());
		//20-11-15
		//TODO
		//send login msg to everyone
		string s{};
		Message msg = Message::make(move(s), fd);
		add_msg_to_all(std::move(msg), fd, fd, listen_loop_->id());
	}
}

void TcpServer::on_close(int fd, shared_ptr<TcpConnection> conn, int loop_id)
{
	debug();
	//20-11-12
	//close fd in conn
	//::close(fd);

	//20-11-15
	//TODO
	del_conn(fd);
	conn_pool_.recycle(conn);
	Message msg{};
	add_msg_to_all(move(msg), fd, fd, loop_id);
}

EventLoop *TcpServer::select_loop(int connfd)
{
	debug();

	if (++loops_index_ >= loops_.size())
	{
		loops_index_ = 1 + LISTEN_LOOP_ID;
	}
	return loops_[loops_index_].get();
}

void TcpServer::on_message(int fd, shared_ptr<TcpConnection> conn, Message msg, int loop_id)
{
	debug();

}

void TcpServer::on_write_complete(int fd)
{
	debug();
}

void TcpServer::on_error()
{
	debug();

	/* 20-11-05
	 * TODO : handle error
	 */
}

int TcpServer::max_fd()
{
	shared_lock<shared_mutex> rlock(conn_pool_mutex_);
	return connections_.empty() ? (-1) : (connections_.rbegin()->first);
}
