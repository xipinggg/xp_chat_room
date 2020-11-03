#include "WakeupChannel.h"
#include <unistd.h>
#include "Types.h"
using namespace std;

WakeupChannel::WakeupChannel(int fd)
	:Channel(fd)
{
	debug();
}

void WakeupChannel::handle_read()
{
	debug();
	
	constexpr int SIZE = 4 * sizeof(uint64_t);
	uint64_t buf[SIZE];
	ssize_t n = SIZE;
	while(n == SIZE)
	{
		n = ::read(fd_,buf,SIZE);
	}
}
