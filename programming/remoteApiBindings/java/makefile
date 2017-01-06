CFLAGS = -I../../include -I../../remoteApi -I. -I/usr/lib/jvm/java-7-openjdk-i386/include -I/usr/lib/jvm/java-7-openjdk-amd64/include -I/System/Library/Frameworks/javaVM.framework/Headers -Wall -D_Included_extApiJava=1 -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -fPIC

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__linux -shared
	EXT = so
else
	CFLAGS += -D__APPLE__ -dynamiclib -current_version 1.0
	EXT = dylib
endif

all: 
	@rm -f lib/*.$(EXT)
	@rm -f *.o 
	@rm -f ../../remoteApi/*.o
	g++ $(CFLAGS) -c ../../remoteApi/extApi.c -o ../../remoteApi/extApi.o
	g++ $(CFLAGS) -c ../../remoteApi/extApiPlatform.c -o ../../remoteApi/extApiPlatform.o
	@mkdir -p lib
	g++ ../../remoteApi/extApi.o ../../remoteApi/extApiPlatform.o -o lib/libremoteApiJava.$(EXT) -lpthread -ldl $(CFLAGS)
