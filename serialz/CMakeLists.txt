cmake_minimum_required( VERSION 3.20.0 )

cmake_minimum_required( VERSION 3.20.0 )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library( serialz
	     serz_limit.cpp
	     serz_buildsz.h
	     serz_buildbuf.h
	     serz_buildstr.cpp
	     serz_buildtools.h
	     serz_variable.h
	     serialize.h
	     deserz_limit.h
	     deserialize.h
	   )

target_link_libraries( serialz INTERFACE )

