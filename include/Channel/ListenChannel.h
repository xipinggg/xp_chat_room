#ifndef LISTENCHANNEL_H
#define LISTENCHANNEL_H
//---------------------------------------
#include <memory>
#include "Channel.h"
//---------------------------------------
class TcpServer;

class ListenChannel final : public Channel
{
public:
	ListenChannel(TcpServer *server);
	void handle_read() override;
private:
	TcpServer *server_;
};

#endif
