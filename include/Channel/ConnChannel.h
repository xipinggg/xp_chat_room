#ifndef CONNECTIONCHANNEL_H
#define CONNECTIONCHANNEL_H
//----------------------------------------------
#include "Channel.h"
#include <memory>

//----------------------------------------------
class TcpConnection;
class ConnChannel final : public Channel
{
public:
	ConnChannel(TcpConnection *conn);
	void handle_read() override;
	void handle_write() override;
	void handle_close() override;
	void handle_error() override;
private:
	TcpConnection* conn_;
};

#endif
