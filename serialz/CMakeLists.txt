cmake_minimum_required( VERSION 3.20.0 )

cmake_minimum_required( VERSION 3.20.0 )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library( serialz
	     ser_limit.cpp
	     ser_buildsz.h
	     ser_buildbuf.h
	     ser_buildtools.h
	     ser_ialize.h
	     deser_limit.h
	     deser_ialize.h
	   )

target_link_libraries( serialz INTERFACE )

