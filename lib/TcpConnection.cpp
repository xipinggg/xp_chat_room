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
      current_msg_{nullptr, 0, 0},
      is_read_msg_head_(false)
{
    debug();
    msg_head_[lenth::MSG_HEAD] = '/0';
    conn_channel_ = make_shared<ConnChannel>(this);
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

void TcpConnection::on_msg_complete()
{
    Message msg = input_buffer_.msg();
    server_->on_message(fd_, shared_from_this(), move(msg), loop_->id());
}
void TcpConnection::handle_read()
{
    //20-11-09
    //TODO
    debug();

    if (!is_get_msg_head())
    {
        if (get_msg_head() == 0)
            return;
    }

    int read_result = input_buffer_.read_from_fd(fd_);
    if (read_result > 0)
    {
        current_msg_.left += read_result;
        if (is_msg_complete())
            on_msg_complete();
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

void TcpConnection::add_msg_to_buffer()
{
    //init current_msg_
    int total_size = lenth::MSG_HEAD + std::atoi(msg_head_);
    current_msg_.str = make_shared<vector<char>>(0, total_size);
    memcpy(current_msg_.str->data(), msg_head_, lenth::MSG_HEAD);
    current_msg_.right = total_size;

    input_buffer_.add_msg(std::move(current_msg_));
}

int TcpConnection::get_msg_head()
{
    debug();

    int read_num = lenth::MSG_HEAD - current_msg_.left;
    int res = ::recv(fd_, msg_head_, read_num, 0);
    if (res > 0)
    {
        current_msg_.left += res;
        if (is_get_msg_head())
        {
            add_new_msg_to_buffer();
            return 1;
        }
    }
    else if (res == 0) //close
    {
        handle_close();
    }
    else if (res < 0) //error
    {
        handle_error();
    }
    return 0;
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
    //20-11-09
    //TODO
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
    //20-11-09
    //TODO
    debug();
    ::close(fd_);
    conn_channel_->set_update_type(EPOLL_CTL_DEL);

    loop_->update_channel(conn_channel_.get(), loop_->id());
    reset();

    server_->on_close(fd_, shared_from_this(), loop_->id());
}

void TcpConnection::reset()
{
    //20-11-09
    //TODO
    debug();

    set_connecting(false);
    input_buffer_.clear();
    output_buffer_.clear();
    conn_channel_->reset();
}

void TcpConnection::restart(sockaddr_in addr, EventLoop *loop)
{
    //20-11-09
    //TODO
    debug();

    set_connecting(true);
    addr_ = addr;
    loop_ = loop;
}

void TcpConnection::handle_error()
{
    //20-11-09
    //TODO
    debug();
    //errno
    switch (errno)
    {
    case EAGAIN:
        /* code */
        break;
    
    default:
        break;
    }
    //server_->on_error();
}


