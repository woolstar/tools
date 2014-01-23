
CC=clang
CXX=clang++
CFLAGS=-std=c++11 -I../include

CC=gcc
CXX=g++
CFLAGS=-std=c++0x -I../include

OD=../obj
LIBPATH=../libs

LIBS=tank_base.cpp tank_iter.cpp tank_range.cpp
OBJS=$(patsubst %.cpp, $(OD)/%.o, $(LIBS))

TARGET=$(LIBPATH)/libpolyco.a

$(TARGET): $(LIBPATH) $(OD) $(OBJS) $(LIBS)
	ar rc $@ $(OBJS)
	ranlib $(LIBPATH)/libpolyco.a


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

