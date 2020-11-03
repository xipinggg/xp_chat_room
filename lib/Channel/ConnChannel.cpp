#include "ConnChannel.h"
#include "TcpConnection.h"
#include "Types.h"
using namespace std;

ConnChannel::ConnChannel(TcpConnection *conn)
	:Channel(conn->fd()),conn_(conn)
{
	debug();
}

void ConnChannel::handle_read()
{
	debug();
	
	conn_->handle_read();
}

void ConnChannel::handle_write()
{
	debug();
	
	conn_->handle_write();
}

void ConnChannel::handle_close()
{
	debug();
	
	conn_->handle_close();
}

void ConnChannel::handle_error()
{
	debug();

	conn_->handle_error();
}
