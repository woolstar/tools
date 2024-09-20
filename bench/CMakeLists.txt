
add_library( gen gen_string.cpp longest.cpp )

target_link_libraries( gen PUBLIC )
target_compile_options( gen PUBLIC -O3 )

find_package( benchmark REQUIRED )

add_executable( bench-struct 
		bm_flatset.cpp
	      )

target_link_libraries( bench-struct PUBLIC gen benchmark::benchmark )
target_compile_options( bench-struct PUBLIC -O2 )

add_executable( bench-mask
                bm_masksearch.cpp
              )

target_link_libraries( bench-mask PUBLIC gen benchmark::benchmark )
target_compile_options( bench-mask PUBLIC -O2 )

add_executable( bench-longest
                bm_longest.cpp
              )

target_link_libraries( bench-longest PUBLIC gen benchmark::benchmark )
target_compile_options( bench-longest PUBLIC -O2 )

