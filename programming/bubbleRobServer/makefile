CFLAGS =

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__linux
else
	CFLAGS += -D__APPLE__
endif

all: 
	@rm -f bin/*.*
	@rm -f *.o 
	g++ $(CFLAGS) -c bubbleRobServer.cpp -o bubbleRobServer.o
	g++ $(CFLAGS) -c simpleInConnection.cpp -o simpleInConnection.o
	g++ $(CFLAGS) -c simplePorting.cpp -o simplePorting.o
	@mkdir -p bin
	g++ simpleInConnection.o simplePorting.o bubbleRobServer.o -o bin/bubbleRobServer
