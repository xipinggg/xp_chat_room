#ifndef BUFFER_H
#define BUFFER_H
//------------------------------
#include <vector>
#include <list>
#include <memory>

struct Buf
{
	std::shared_ptr<std::vector<char>> buf;
	int left;
	int right;
};

class Buffer
{
public:
	using BufList = std::list<Buf>;
	enum{kInitialSize = 1024};
	Buffer();
	~Buffer();
	//p
	ssize_t write(int fd);
	ssize_t read(int fd,size_t num);
	
	ssize_t write(Buf buf,int threadId);
	ssize_t read(int fd);
	
	void reset();
	void swap(Buffer &rhs);
	
	//get
	ssize_t messageLenth(int fd);
	char* beginRead(){ return begin() + readerIndex_; }
	char* beginWrite(){ return begin() + writerIndex_; }
	size_t readableBytes() const { return writerIndex_ - readerIndex_; }
	size_t writableBytes() const { return buffer_.size() - writerIndex_; }
	bool empty() const { return !(bool)readableBytes(); }
	//set
	void writeBytes(size_t num);
	void readBytes(size_t num);
	void resize(size_t size){ if(size>0) buffer_.resize(size+buffer_.size());}
private:
	char* begin(){ return &((char)(*buffer_.begin())); }
	void updateIndex();

private:
	std::vector<BufList> buffer_;
	
	size_t readerIndex_;
	size_t writerIndex_;
	std::shared_mutex mtx_;
	ssize_t lenth_;
	
	bool getLenth_;
};

#endif
