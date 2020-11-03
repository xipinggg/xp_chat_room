#include "Buffer.h"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include "Types.h"
using namespace std;

constexpr int subThreadCount = 4;

Buffer::Buffer()
	:buffer_(1 + subThreadCount),
	readerIndex_(0),
	writerIndex_(0),
	getLenth_(false)
{


}

Buffer::~Buffer()
{

	
}

ssize_t Buffer::write(Buf buf,int threadId)
{
	buffer_[threadId].emplace_back(buf);
}

ssize_t Buffer::read(int fd)
{
	
}

ssize_t Buffer::write(int fd)
{
	debug();
	
	if(!getLenth_)
	ssize_t lenth = messageLenth(fd);
	if(lenth == 0)
	{
		return 0;
	}
	
	shared_ptr<vector<char>> buf(4+lenth);
	
	/*size_t maxNum = 0;
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
	
	return readResult;*/
}

ssize_t Buffer::read(int fd,size_t num)
{
	debug();
	
	size_t maxNum = readableBytes();
	char *buf = beginRead();
	ssize_t sendResult = ::send(fd,buf,maxNum,0);
	
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
	if(readerIndex_ >= buffer_.size()/2)
	{
		memcpy(begin(),&buffer_[readerIndex_],readableBytes());

		readerIndex_ = 0;
		writerIndex_ = readableBytes();
	}
	else if(readableBytes() >= buffer_.size() / 2)
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
}

