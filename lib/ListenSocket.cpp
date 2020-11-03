#include "ListenSocket.h"

#include <unistd.h>
#include <cstring>
#include <cassert>

#include "Types.h"

using namespace std;

ListenSocket::ListenSocket(int port)
	:port_(port),
	fd_(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)),
	addr_()
{
	debug();
	
	assert(fd_ >= 0);
}

ListenSocket::~ListenSocket()
{
	debug();

	::close(fd_);
}

int ListenSocket::listen(int backlog)
{
	debug();

	int ret = ::listen(fd_,backlog);
	return ret;
}

int ListenSocket::bind()
{
	debug();

	bzero(&addr_,sizeof(struct sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port_);
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = ::bind(fd_,(struct sockaddr *)&addr_,sizeof(addr_));

	assert(ret >= 0);
	return ret;
}
