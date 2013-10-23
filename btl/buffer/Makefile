
CC=gcc-4.8
CXX=g++-4.8
CFLAGS=-std=c++0x -Iinc

CC=clang
CXX=clang++
CFLAGS=-std=c++11 -Iinc

OD=../obj
LIBPATH=../libs

LIBS=buffer.cpp build.cpp build_methods.cpp expand_traits.cpp
OBJS=$(patsubst %.cpp, $(OD)/%.o, $(LIBS))

TARGET=$(LIBPATH)/libbuffer.a

$(TARGET): $(LIBPATH) $(OD) $(OBJS) $(LIBS)
	ar rc $(LIBPATH)/libbuffer.a $(OBJS)
	ranlib $(LIBPATH)/libbuffer.a


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

