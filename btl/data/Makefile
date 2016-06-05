
CFLAGS=-std=c++11 -I../include $(LOCAL_CFLAGS)

OD=../obj
LIBPATH=../libs

LIBS=buffer.cpp build.cpp build_methods.cpp expand_traits.cpp
OBJS=$(patsubst %.cpp, $(OD)/%.o, $(LIBS))

TARGET=$(LIBPATH)/libdata.a

$(TARGET): $(LIBPATH) $(OD) $(OBJS) $(LIBS)
	ar rc $(LIBPATH)/libdata.a $(OBJS)
	ranlib $(LIBPATH)/libdata.a


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

