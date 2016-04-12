
CFLAGS=-std=c++11 -g -I../btl/include $(LOCAL_CFLAGS)
LIBS=$(LOCAL_LIBS)

LIBPATH=-L../btl/libs

TARGETS=xbuffer xio xlist xsock xecho xtnk xvek

demos: $(TARGETS)

xbuffer: demo_buffer.cpp
	$(CXX) $(CFLAGS) $< $(LIBPATH) -ldata $(LIBS) -o $@

xio: demo_io.cpp
	$(CC) $(CFLAGS) $< $(LIBPATH) -lio -ldata $(LIBS) -o $@

xlist: demo_list.cpp
	$(CC) $(CFLAGS) $< $(LIBPATH) -ldata $(LIBS) -o $@

xsock: demo_socket.cpp
	$(CC) $(CFLAGS) $< $(LIBPATH) -lsocket -lio -ldata $(LIBS) -o $@

xecho: demo_echo.cpp
	$(CC) $(CFLAGS) $< $(LIBPATH) -lio -ldata -lsocket $(LIBS) -o $@

xtnk: demo_ctank.cpp
	$(CC) $(CFLAGS) $< -L../btl/libs -L../ctl/libs -lio -ldata -lpolyco $(LIBS) -o $@

xvek: demo_cvek.cpp
	$(CC) $(CFLAGS) $< -L../btl/libs -L../ctl/libs -lio -ldata -lpolyco $(LIBS) -o $@

clean:
	rm -fr *.o $(TARGETS)

setup:
	ln -s ../btl/include btl
	ln -s ../ctl/include ctl

