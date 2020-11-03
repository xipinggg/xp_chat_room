#include "TcpServer.h"

#include <cassert>
#include <algorithm>
#include <thread>

#include "ListenChannel.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "ConnChannel.h"
#include "Types.h"
#include "ListenSocket.h"
using namespace std;
using namespace std::placeholders;

TcpServer::TcpServer(int ioThreadCount)
	:io_thread_count_(ioThreadCount),
	loops_index_(0),
	socket_(new ListenSocket),
	listen_channel_(new ListenChannel(this)),
	loops_(1 + ioThreadCount),
	connections_(),
	conn_pool_(),
	conn_pool_mutex_()
{
	debug();
   	
   	loops_[0] = make_shared<EventLoop>(0,"listenLoop");
   	
   	int index = 1;
   	while(index <= io_thread_count_)
	{
   	    loops_[index] = make_shared<EventLoop>(index,"ioLoop");
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

	auto channel = listen_channel_.get();
	
	auto listenLoop = loops_[0];
	listenLoop->update_channel(channel,listenLoop->id());

	for(int i = 1;i < loops_.size();++i)
	{
		thread t(&EventLoop::loop,loops_[i].get());
		t.detach();
	}

	listenLoop->loop();
}


void TcpServer::on_close(int fd, shared_ptr<TcpConnection> conn, weak_ptr<EventLoop> loop)
{
	debug();
	
	::close(fd);
	conn_pool_.recycle(conn);
}

void TcpServer::on_new_connection(int fd,sockaddr_in addr)
{
	debug();
	//cout<<connfd<<"  "<<maxFd()<<endl;
	auto loop = select_loop(fd);

	auto conn = conn_pool_.get(fd,addr,
					weak_from_this(),weak_ptr<EventLoop>(loop));
	
	loop->update_channel(conn->channel(),loops_[0]->id());
}

shared_ptr<EventLoop> TcpServer::select_loop(int connfd)
{
    debug();
    if(++loops_index_ >= loops_.size())
        loops_index_ = 1;
        
    return loops_[loops_index_];
}

void TcpServer::on_message(int fd,shared_ptr<TcpConnection> conn,weak_ptr<EventLoop> loop)
{
	debug();
	  
	if(loop.expired())
		return;
}

void TcpServer::onWriteComplete(int fd)
{
	debug();
}

void TcpServer::on_error()
{
	//errno
	debug();
}

int TcpServer::max_fd()
{
	shared_lock<shared_mutex> rlock(conn_pool_mutex_);
	return connections_.empty() ? (-1) : (connections_.rbegin()->first);
}

int TcpServer::fd() const 
{
	return socket_->fd();
}