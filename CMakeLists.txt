cmake_minimum_required( VERSION 3.20.0 )

project ( tools C CXX )

set ( CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_STANDARD 20 )

enable_testing ()

add_subdirectory( "adapt" )
add_subdirectory( "funct" )
add_subdirectory( "serialz" )
add_subdirectory( "test" )
add_subdirectory( "bench" )

