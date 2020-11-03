std = -std=c++20
OBJS = ListenChannel.o ConnectionChannel.o WakeupChannel.o Channel.o
#------------------------------------
ListenChannel.o:ListenChannel.cpp ListenChannel.h Channel.h ./TcpServer.h ./Types.h
	g++ $(std) -c ListenChannel.cpp -g
	
ConnectionChannel.o:ConnectionChannel.cpp ConnectionChannel.h Channel.h ./TcpConnection.h ./Types.h
	g++ $(std) -c ConnectionChannel.cpp -g
	
WakeupChannel.o:WakeupChannel.cpp WakeupChannel.h Channel.h ./Types.h
	g++ $(std) -c WakeupChannel.cpp -g
	
Channel.o:Channel.cpp Channel.h ./Types.h
	g++ $(std) -c Channel.cpp -g
#------------------------------------
clean:
	rm -f $(OBJS)
