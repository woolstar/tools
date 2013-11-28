
CC=gcc
CXX=g++
CFLAGS=-std=c++0x -Iinc

CC=clang
CXX=clang++
CFLAGS=-std=c++11 -Iinc

OD=../obj
LIBPATH=../libs

IOLIBS=io_core.cpp io_basic.cpp io_init.cpp io_manage.cpp \
	channel_base.cpp channel_scan.cpp
SOCKLIBS=socket.cpp socket_base.cpp socket_connect.cpp
IOBJS=$(patsubst %.cpp, $(OD)/%.o, $(IOLIBS))
SOBJS=$(patsubst %.cpp, $(OD)/%.o, $(SOCKLIBS))

ITARGET=$(LIBPATH)/libio.a
STARGET=$(LIBPATH)/libsocket.a

all: $(ITARGET) $(STARGET)

$(ITARGET): $(LIBPATH) $(OD) $(IOBJS) $(IOLIBS)
	rm -f $@
	ar rc $@ $(IOBJS)
	ranlib $@

$(STARGET): $(LIBPATH) $(OD) $(SOBJS) $(SOCKLIBS)
	rm -f $@
	ar rc $@ $(SOBJS)
	ranlib $@


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
	rm -fr $(OD) $(ITARGET) $(STARGET)

