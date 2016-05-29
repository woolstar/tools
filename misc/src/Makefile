CXXFLAGS=-std=c++11 -I../include
CXX=clang++

OD=../obj
LIBPATH=../libs

LIBS=safety_once.cpp
OBJS=$(patsubst %.cpp, $(OD)/%.o, $(LIBS))

TARGET=$(LIBPATH)/libmisc.a

$(TARGET): $(LIBPATH) $(OD) $(OBJS) $(LIBS)
	ar rc $(LIBPATH)/libmisc.a $(OBJS)
	ranlib $(LIBPATH)/libmisc.a

$(OD):
	mkdir -p $(OD)

$(LIBPATH):
	mkdir -p $(LIBPATH)

.SUFFIXES: .c .cpp

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

$(OD)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -fr $(OD) $(TARGET)

