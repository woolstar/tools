cmake_minimum_required( VERSION 3.20.0 )

add_executable( gtest-unit
		t_adapt_stringtok.cpp
                t_funct_job.cpp
		t_static.cpp t_static2.cpp
	      )

target_link_libraries( gtest-unit PUBLIC gtest gtest_main adapt funct )
target_include_directories (
      gtest-unit PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/adapt" 
	"${PROJECT_SOURCE_DIR}/funct" 
      )

add_test ( NAME gtest-unit COMMAND $<TARGET_FILE:gtest-unit> )

