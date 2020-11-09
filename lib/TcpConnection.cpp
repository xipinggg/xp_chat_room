#include "TcpConnection.h"
#include <fcntl.h>
#include <cassert>

#include "TcpServer.h"
#include "ConnChannel.h"
#include "EventLoop.h"
#include "Types.h"
using namespace std;

TcpConnection::TcpConnection()
    : fd_(-1)
{
    debug();
}

TcpConnection::TcpConnection(int fd, sockaddr_in addr, TcpServer *server, EventLoop *loop)
    : fd_(fd),
      addr_(addr),
      connecting_(true),
      conn_channel_(nullptr),
      server_(server),
      loop_(loop),
      msg_head_(),
      num_need_read_(0)
{
    debug();

    conn_channel_ = make_shared<ConnChannel>(this);
    set_nonblock_fd();
}

void TcpConnection::set(int fd, sockaddr_in addr, TcpServer *server, EventLoop *loop)
{
    fd_ = fd;
    addr_ = addr;
    connecting_ = true;
    server_ = server;
    loop_ = loop;
}

TcpConnection::~TcpConnection()
{
    debug();

    //::close(fd_);
}

void TcpConnection::handle_read()
{
    debug();

    if (input_buffer_.empty())
        return;

    auto read_result = input_buffer_.read_from_fd(fd_, num_need_read_);
    if (read_result > 0)
    {
        num_need_read_ -= read_result;
        Message msg = input_buffer_.msg();
        server_->on_message(fd_, shared_from_this(), loop_->id());
    }
    else if (read_result == 0) //close
    {
        handle_close();
    }
    else if (read_result < 0) //error
    {
        handle_error();
    }
}

/*
void TcpConnection::handle_read()
{
    debug();
    //cout << "TcpConnection::handleRead()\n";
    ssize_t read_result = input_buffer_.read_from_fd(fd_);
    
    if(read_result > 0)
    {
        server_->on_message(fd_, shared_from_this(), loop_->id());
    }
    else if(read_result == 0)//close
    {
        handle_close();
    }
    else if(read_result < 0)//error
    {
        handle_error();
    }
}*/

void TcpConnection::handle_write()
{
    debug();
    ssize_t send_result = output_buffer_.write_to_fd(fd_);

    if (send_result > 0)
    {
        if (output_buffer_.empty())
        {
            int ev = conn_channel_->events();
            conn_channel_->set_events(ev & (~EPOLLOUT));
            conn_channel_->set_update_type(EPOLL_CTL_MOD);
            loop_->update_channel(conn_channel_.get(), loop_->id());
        }
    }
    if (send_result < 0)
    {
        handle_error();
    }
}

void TcpConnection::handle_close()
{
    debug();

    conn_channel_->set_update_type(EPOLL_CTL_DEL);

    loop_->update_channel(conn_channel_.get(), loop_->id());
    reset();

    server_->on_close(fd_, shared_from_this(), loop_->id());
}

void TcpConnection::reset()
{
    debug();

    set_connecting(false);
    input_buffer_.clear();
    output_buffer_.clear();
    conn_channel_->reset();
}

void TcpConnection::restart(sockaddr_in addr, EventLoop *loop)
{
    debug();

    set_connecting(true);
    addr_ = addr;
    loop_ = loop;
}

void TcpConnection::handle_error()
{
    debug();
    //errno
    server_->on_error();
}

bool TcpConnection::set_nonblock_fd()
{
    debug();

    int flags = fcntl(fd_, F_GETFL, 0);
    return fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
}
