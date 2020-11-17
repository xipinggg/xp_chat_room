#include "ListenChannel.h"

#include <netinet/in.h>
#include <sys/types.h>
#include "../TcpServer.h"
#include "../Types.h"

using namespace std;

ListenChannel::ListenChannel(TcpServer *server)
	:Channel(server->fd()),
	server_(server)
{
	debug();
}

void ListenChannel::handle_read()
{
	debug();
	
	sockaddr_in peer_addr;
	socklen_t len = sizeof(peer_addr);
	int conn_fd = ::accept(fd_,(sockaddr*)&peer_addr,&len);
	
	//cout<<"connfd:  "<<connfd<<endl;
	if(conn_fd > 0)
	{
		server_->on_connect(conn_fd,peer_addr);
	}
	else
	{

	}
}