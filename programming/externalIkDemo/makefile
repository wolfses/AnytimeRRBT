CFLAGS = -I../remoteApi -I../externalIk -I../include -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__linux
else
	CFLAGS += -D__APPLE__
endif

all: 
	@rm -f bin/*.*
	@rm -f *.o 
	@rm -f ../remoteApi/*.o 
	@rm -f ../externalIk/*.o 
	g++ $(CFLAGS) -c externalIkDemo.cpp -o externalIkDemo.o
	gcc $(CFLAGS) -c ../remoteApi/extApi.c -o ../remoteApi/extApi.o
	gcc $(CFLAGS) -c ../remoteApi/extApiPlatform.c -o ../remoteApi/extApiPlatform.o
	g++ $(CFLAGS) -c ../externalIk/Vector.cpp -o ../externalIk/Vector.o
	g++ $(CFLAGS) -c ../externalIk/Tt.cpp -o ../externalIk/Tt.o
	g++ $(CFLAGS) -c ../externalIk/Registerediks.cpp -o ../externalIk/Registerediks.o
	g++ $(CFLAGS) -c ../externalIk/ObjCont.cpp -o ../externalIk/ObjCont.o
	g++ $(CFLAGS) -c ../externalIk/MyMath.cpp -o ../externalIk/MyMath.o
	g++ $(CFLAGS) -c ../externalIk/MMatrix.cpp -o ../externalIk/MMatrix.o
	g++ $(CFLAGS) -c ../externalIk/MainCont.cpp -o ../externalIk/MainCont.o
	g++ $(CFLAGS) -c ../externalIk/ikRoutine.cpp -o ../externalIk/ikRoutine.o
	g++ $(CFLAGS) -c ../externalIk/ikGroup.cpp -o ../externalIk/ikGroup.o
	g++ $(CFLAGS) -c ../externalIk/ikEl.cpp -o ../externalIk/ikEl.o
	g++ $(CFLAGS) -c ../externalIk/extIkSer.cpp -o ../externalIk/extIkSer.o
	g++ $(CFLAGS) -c ../externalIk/extIk.cpp -o ../externalIk/extIk.o
	g++ $(CFLAGS) -c ../externalIk/Dummy.cpp -o ../externalIk/Dummy.o
	g++ $(CFLAGS) -c ../externalIk/Ct.cpp -o ../externalIk/Ct.o
	g++ $(CFLAGS) -c ../externalIk/jointObject.cpp -o ../externalIk/jointObject.o
	g++ $(CFLAGS) -c ../externalIk/7Vector.cpp -o ../externalIk/7Vector.o
	g++ $(CFLAGS) -c ../externalIk/6X6Matrix.cpp -o ../externalIk/6X6Matrix.o
	g++ $(CFLAGS) -c ../externalIk/6Vector.cpp -o ../externalIk/6Vector.o
	g++ $(CFLAGS) -c ../externalIk/4X4Matrix.cpp -o ../externalIk/4X4Matrix.o
	g++ $(CFLAGS) -c ../externalIk/4X4FullMatrix.cpp -o ../externalIk/4X4FullMatrix.o
	g++ $(CFLAGS) -c ../externalIk/4Vector.cpp -o ../externalIk/4Vector.o
	g++ $(CFLAGS) -c ../externalIk/3X3Matrix.cpp -o ../externalIk/3X3Matrix.o
	g++ $(CFLAGS) -c ../externalIk/3Vector.cpp -o ../externalIk/3Vector.o
	g++ $(CFLAGS) -c ../externalIk/3DObject.cpp -o ../externalIk/3DObject.o
	@mkdir -p bin
	g++ externalIkDemo.o ../remoteApi/extApi.o ../remoteApi/extApiPlatform.o ../externalIk/Vector.o ../externalIk/Tt.o ../externalIk/Registerediks.o ../externalIk/ObjCont.o ../externalIk/MyMath.o ../externalIk/MMatrix.o ../externalIk/MainCont.o ../externalIk/ikRoutine.o ../externalIk/ikGroup.o ../externalIk/ikEl.o ../externalIk/extIkSer.o ../externalIk/extIk.o ../externalIk/Dummy.o ../externalIk/Ct.o ../externalIk/jointObject.o ../externalIk/7Vector.o ../externalIk/6X6Matrix.o ../externalIk/6Vector.o ../externalIk/4X4Matrix.o ../externalIk/4X4FullMatrix.o ../externalIk/4Vector.o ../externalIk/3X3Matrix.o ../externalIk/3Vector.o ../externalIk/3DObject.o -o bin/externalIkDemo -lpthread
