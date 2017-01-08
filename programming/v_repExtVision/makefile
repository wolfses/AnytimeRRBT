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
	g++ $(CFLAGS) -c visionTransf.cpp -o visionTransf.o
	g++ $(CFLAGS) -c visionTransfCont.cpp -o visionTransfCont.o
	g++ $(CFLAGS) -c visionVelodyne.cpp -o visionVelodyne.o
	g++ $(CFLAGS) -c visionVelodyneCont.cpp -o visionVelodyneCont.o
	g++ $(CFLAGS) -c v_repExtVision.cpp -o v_repExtVision.o
	g++ $(CFLAGS) -c ../common/v_repLib.cpp -o ../common/v_repLib.o
	@mkdir -p lib
	g++ visionTransf.o visionTransfCont.o visionVelodyne.o visionVelodyneCont.o v_repExtVision.o ../common/v_repLib.o -o lib/libv_repExtVision.$(EXT) -lpthread -ldl -shared 