CFLAGS = -I../include -Wall -fPIC -static

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__linux
	EXT = so
else
	CFLAGS += -D__APPLE__
	EXT = dylib
endif

all: 
	@rm -f lib/*.$(EXT)
	@rm -f *.o 
	@rm -f ../common/*.o
	g++ $(CFLAGS) -c access.cpp -o access.o
	g++ $(CFLAGS) -c bubbleRob.cpp -o bubbleRob.o
	g++ $(CFLAGS) -c bubbleRobContainer.cpp -o bubbleRobContainer.o
	g++ $(CFLAGS) -c porting.cpp -o porting.o
	g++ $(CFLAGS) -c v_repExtBubbleRob.cpp -o v_repExtBubbleRob.o
	g++ $(CFLAGS) -c ../common/v_repLib.cpp -o ../common/v_repLib.o
	@mkdir -p lib
	g++ access.o bubbleRob.o porting.o bubbleRobContainer.o v_repExtBubbleRob.o ../common/v_repLib.o -o lib/libv_repExtBubbleRob.$(EXT) -lpthread -ldl -shared 
