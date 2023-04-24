cmake_minimum_required( VERSION 3.20.0 )

add_library( adapt
	     adapt_stringtok.cpp
	   )

target_link_libraries( adapt PUBLIC )

