#ifndef LISTENSOCKET_H
#define LISTENSOCKET_H
//---------------------------------------
#include <sys/types.h>
#include <netinet/in.h>
//----------------------------------------
class ListenSocket
{
public:
	enum Init:int{PORT = 8888,BACKLOG = 21};
	
	ListenSocket(int port = PORT);
	~ListenSocket();
	int bind();
	int listen(int backlog = BACKLOG);
	//get and set
	int fd() const { return fd_; }
	int port() const { return port_; }
	sockaddr_in addr() const { return addr_; }

private:
	int port_;
	int fd_;
	sockaddr_in addr_;
};

#endif
