cmake_minimum_required( VERSION 3.20.0 )

add_library( funct 
	     funct_job.cpp
	   )

target_link_libraries( funct PUBLIC )

