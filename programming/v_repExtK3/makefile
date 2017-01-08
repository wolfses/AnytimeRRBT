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
	g++ $(CFLAGS) -c k3.cpp -o k3.o
	g++ $(CFLAGS) -c k3Container.cpp -o k3Container.o
	g++ $(CFLAGS) -c porting.cpp -o porting.o
	g++ $(CFLAGS) -c stdafx.cpp -o stdafx.o
	g++ $(CFLAGS) -c v_repExtK3.cpp -o v_repExtK3.o
	g++ $(CFLAGS) -c ../common/v_repLib.cpp -o ../common/v_repLib.o
	@mkdir -p lib
	g++ access.o k3.o porting.o k3Container.o stdafx.o v_repExtK3.o ../common/v_repLib.o -o lib/libv_repExtK3.$(EXT) -lpthread -ldl -shared 
