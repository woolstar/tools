
CC=gcc
CXX=g++
CFLAGS=-std=c++0x -Iinc

CC=clang
CXX=clang++
CFLAGS=-std=c++11 -Iinc

OD=../obj
LIBPATH=../libs

LIBS=io_core.cpp io_basic.cpp io_init.cpp io_manage.cpp channel_scan.cpp socket.cpp
OBJS=$(patsubst %.cpp, $(OD)/%.o, $(LIBS))

TARGET=$(LIBPATH)/libio.a

$(TARGET): $(LIBPATH) $(OD) $(OBJS) $(LIBS)
	rm -f $(LIBPATH)/libio.a
	ar rc $(LIBPATH)/libio.a $(OBJS)
	ranlib $(LIBPATH)/libio.a


$(OD):
	mkdir -p $(OD)

$(LIBPATH):
	mkdir -p $(LIBPATH)

.SUFFIXES: .c .cpp

.c.o:
	$(CXX) $(CFLAGS) -c $<

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

$(OD)/%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	rm -fr $(OD) $(TARGET)

