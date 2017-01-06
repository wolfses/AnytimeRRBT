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
	g++ $(CFLAGS) -c mtbServer.cpp -o mtbServer.o
	g++ $(CFLAGS) -c robotLanguageInterpreter.cpp -o robotLanguageInterpreter.o
	g++ $(CFLAGS) -c simpleInConnection.cpp -o simpleInConnection.o
	g++ $(CFLAGS) -c simplePorting.cpp -o simplePorting.o
	@mkdir -p bin
	g++ robotLanguageInterpreter.o simpleInConnection.o simplePorting.o mtbServer.o -o bin/mtbServer -lpthread 
