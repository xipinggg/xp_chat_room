#include "TcpConnection.h"
#include <fcntl.h>
#include <cassert>

#include "TcpServer.h"
#include "ConnChannel.h"
#include "EventLoop.h"
#include "Types.h"
using namespace std;

TcpConnection::TcpConnection(int fd, sockaddr_in addr, TcpServer *server, EventLoop *loop)
	: fd_(fd),
	  peer_addr_(addr),
	  connecting_(true),
	  conn_channel_(nullptr),
	  server_(server),
	  loop_(loop)
{
	debug();

	conn_channel_ = make_shared<ConnChannel>(this);
	set_nonblock_fd();
}

void TcpConnection::set(int fd, sockaddr_in addr, TcpServer *server, EventLoop *loop)
{
	fd_ = fd;
	peer_addr_ = addr;
	connecting_ = true;
	server_ = server;
	loop_ = loop;
}
TcpConnection::TcpConnection()
	:fd_(-1)
{
	debug();
}

TcpConnection::~TcpConnection()
{
    debug();
	
    //::close(fd_);
}


void TcpConnection::handle_read()
{
	debug();
	cout << "TcpConnection::handleRead()\n";
	ssize_t read_result = input_buffer_.write(fd_);
	
    if(read_result > 0)
    {
		server_->on_message(fd_, shared_from_this(), loop_);
	}
    else if(read_result == 0)//close
    {
		handle_close();
	}
	else if(read_result < 0)//error
	{
		handle_error();
	}
}

void TcpConnection::handle_write()
{
	debug();
	ssize_t send_result = output_buffer_.write(fd_);
	
	if(send_result > 0)
	{
		output_buffer_.readBytes(send_result);

		if(output_buffer_.empty())
		{
			int ev=conn_channel_->events();
			conn_channel_->set_events(ev & (~EPOLLOUT));
			conn_channel_->set_update_type(Channel::UPDATE_NO::MOD);
			
	    	loop_->update_channel(ChannelWPtr(conn_channel_));
		}
	}
}

void TcpConnection::handle_close()
{
	debug();
	
	conn_channel_->set_update_type(Channel::UPDATE_NO::DEL);
	
	if(!loop_.expired())
	{
		loop_.lock()->update_channel(ChannelWPtr(conn_channel_));
	}
	
	reset();
	
	if(!server_.expired())
	{
		server_.lock()->onClose(fd_,loop_);
	}
}

void TcpConnection::reset()
{
	debug();
	
	setConnecting(false);
	inputBuffer_.reset();
	outputBuffer_.reset();
	conn_channel_->reset();
}

void TcpConnection::restart(sockaddr_in addr,weak_ptr<EventLoop> loop)
{
	debug();
	
	setConnecting(true);
	peer_addr_ = addr;
	loop_ = loop;
}

void TcpConnection::handle_error()
{
	debug();
	//errno
	if(!server_.expired())
	{
		server_.lock()->onError();
	}
	
}

void TcpConnection::set_nonblock_fd()
{
	debug();

	int flags = fcntl(fd_, F_GETFL, 0);
	fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
}



