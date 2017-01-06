CFLAGS = -I../include -Wall -fPIC -static

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__linux -DLINUX -std=c++0x
	EXT = so
else
	CFLAGS += -D__APPLE__
	EXT = dylib
endif

# CHAI3D (make sure to compile the CHAI3D library with -fPIC)
CHAI3D_DIR = /home/marc/Documents/chai3d-3.0.0
CFLAGS    += -I$(CHAI3D_DIR)/src
CFLAGS    += -I$(CHAI3D_DIR)/external/Eigen
CFLAGS    += -I$(CHAI3D_DIR)/external/glew/include
LDFLAGS   += -L$(CHAI3D_DIR)/lib/release/lin-x86_64-gcc
LDLIBS    += -lchai3d

# FDSDK (make sure to compile the Force Dimension library with -fPIC)
FDSDK_DIR = /home/marc/Documents/sdk-3.5.1
CFLAGS   += -I$(FDSDK_DIR)/include
LDFLAGS  += -L$(FDSDK_DIR)/lib/release/lin-x86_64-gcc
LDLIBS   += -ldrd -lusb-1.0

all: 
	@rm -f lib/*.$(EXT)
	@rm -f *.o 
	@rm -f ../common/*.o
	g++ $(CFLAGS) -c v_repExtCHAI3D.cpp -o v_repExtCHAI3D.o
	g++ $(CFLAGS) -c ../common/v_repLib.cpp -o ../common/v_repLib.o
	@mkdir -p lib
	g++ v_repExtCHAI3D.o ../common/v_repLib.o -o lib/libv_repExtCHAI3D.$(EXT) $(LDFLAGS) $(LDLIBS) -lpthread -ldl -shared 
