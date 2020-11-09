#ifndef WAKEUPCHANNEL_H
#define WAKEUPCHANNEL_H

#include "Channel.h"

class WakeupChannel final : public Channel
{
public:
	WakeupChannel(int fd = FD);
	void handle_read() override;
};

#endif