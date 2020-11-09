#include "../include/Buffer.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

#include "../include/Types.h"
using namespace std;

constexpr int subThreadCount = 4;

Buffer::Buffer()
    : msg_num_(0)
{
    
}

Buffer::~Buffer()
{

}

void Buffer::add_msg(Message msg)
{
    messages_lock_.lock();
    messages_.emplace_back(move(msg.str), msg.left, msg.right);
    messages_lock_.unlock();
}

ssize_t Buffer::write_to_fd(int fd)
{
    auto &msg = front_msg();
    const size_t str_size = msg.str_size();
    char *buf = msg.data();
    ssize_t result = ::send(fd, buf, str_size, 0);
    if (result == str_size)
    {
        SpinLockGuard slg(messages_lock_);
        messages_.pop_front();
    }
    else if (result > 0 && result < str_size)
    {
        msg.left += result;
    }
    return result;
}

Message Buffer::msg()
{
    auto msg = messages_.front();
    SpinLockGuard slg(messages_lock_);
    messages_.pop_front();
    --msg_num_;
    return msg;
}

ssize_t Buffer::read_from_fd(int fd, int num)
{
    auto &msg = back_msg();
    char *buf = msg.data();
    auto result = ::recv(fd, buf, num, 0);
    if (result > 0)
    {
        msg.left += result;
        if (result == num)
        {
            ++msg_num_;
        }
    }
    return result;
}

void Buffer::clear()
{
    messages_.clear();
    msg_num_ = 0;
}
/*
ssize_t Buffer::write(int fd, int str_size)
{

}

ssize_t Buffer::write(int fd, char *buf, int num)
{
    ssize_t result = ::recv(fd, buf, num, 0);
    return result;
}

ssize_t Buffer::write(int fd)
{
    debug();

    shared_ptr<vector<char>> buf(4 + lenth);
    size_t maxNum = 0;
    ssize_t readResult = 0;
    do
    {
        maxNum = writableBytes();
        char *buf = beginWrite();
        readResult = ::recv(fd,buf,maxNum,0);
    
        if(readResult > 0)
        {
            writeBytes(readResult);
        }
    }while(readResult == maxNum);
    
    return readResult;
}

ssize_t Buffer::read(int fd, size_t num)
{
    debug();

    size_t maxNum = readableBytes();
    char *buf = beginRead();
    ssize_t sendResult = ::send(fd, buf, maxNum, 0);

    return sendResult;
}

ssize_t Buffer::messageLenth(int fd)
{

}

void Buffer::writeBytes(size_t num)
{
    debug();

    writerIndex_ += num;
    updateIndex();
}

void Buffer::readBytes(size_t num)
{
    debug();

    readerIndex_ += num;
    updateIndex();
}

void Buffer::reset()
{
    debug();

    //buffer_.clear();
    readerIndex_ = 0;
    writerIndex_ = 0;
}

void Buffer::updateIndex()
{
    debug();
    //readerIndex  writerIndex_
    if (readerIndex_ >= buffer_.size() / 2)
    {
        memcpy(begin(), &buffer_[readerIndex_], readableBytes());

        readerIndex_ = 0;
        writerIndex_ = readableBytes();
    }
    else if (readableBytes() >= buffer_.size() / 2)
    {
        buffer_.resize(buffer_.size() * 2);
    }
}

void Buffer::swap(Buffer &rhs)
{
    debug();

    buffer_.swap(rhs.buffer_);
    std::swap(readerIndex_, rhs.readerIndex_);
    std::swap(writerIndex_, rhs.writerIndex_);
}*/