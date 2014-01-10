
CC=gcc-4.8
CFLAGS=-std=c++0x
CC=clang
CFLAGS=-std=c++11 -g

LIBPATH=-L../btl/libs

TARGETS=xbuffer xio xlist xsock xecho xvec

demos: $(TARGETS)

xbuffer: demo_buffer.cpp
	$(CC) $(CFLAGS) -Ibtl $< $(LIBPATH) -ldata -lstdc++ -o $@

xio: demo_io.cpp
	$(CC) $(CFLAGS) -Ibtl $< $(LIBPATH) -lio -ldata -lstdc++ -o $@

xlist: demo_list.cpp
	$(CC) $(CFLAGS) -Ibtl $< $(LIBPATH) -ldata -lstdc++ -o $@

xsock: demo_socket.cpp
	$(CC) $(CFLAGS) -Ibtl $< $(LIBPATH) -lsocket -lio --ldata lstdc++ -o $@

xecho: demo_echo.cpp
	$(CC) $(CFLAGS) -Ibtl $< $(LIBPATH) -lio -ldata -lsocket -lstdc++ -o $@

xvec: demo_cvec.cpp
	$(CC) $(CFLAGS) -Ictl -Ibtl $< -L../btl/libs -L../ctl/libs -lio -ldata -lpolyco -lstdc++ -o $@

clean:
	rm -fr *.o $(TARGETS)

setup:
	ln -s ../btl/include btl
	ln -s ../ctl/include ctl

