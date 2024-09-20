#include <benchmark/benchmark.h>

#include "longest.h"
#include "gen_string.h"

#include <iostream>

using namespace std ;

/*****
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
BM_cpp_longest          814101 ns       814101 ns          858
BM_cpp_longest_tune     783420 ns       783410 ns          908
BM_c_longest             54906 ns        54906 ns        12791
BM_c_clever_longest      61980 ns        61979 ns        11218

*/

namespace
{
    // do a small number of short strings
const static int  kSetSize = 512 ;
const static int  kKeySize = 64 ;

  // for whatever dictionary under test, construct N for testing
template<typename ALGO>
struct test_block
{
      // populate dictionaries with *sz* keys from source *labels*
    test_block( ALGO search ) : searcher_( search )
    { }

      // actual test
    int   count( const vector<string> & strings )
    {
        int lengths = 0 ;

        for ( auto & s : strings ) {
          lengths += searcher_( s ) ; 
        }
        return lengths ;
    }

    void   bench( benchmark::State& bs )
    {
        auto  keys = Gen::alpha( kSetSize, kKeySize ) ;
        int total ;

        for ( auto _ : bs ) {
          total= count( keys ) ;
        }
    }

    ALGO  searcher_ ;
} ;

}

    // TEST c++ longest algo
    //
static void   BM_cpp_longest( benchmark::State& bs )
{
    test_block  test( Find::longest ) ;
    test.bench( bs ) ;
}

static void   BM_cpp_longest_tune( benchmark::State& bs )
{
    test_block  test( Find::longest_tune ) ;
    test.bench( bs ) ;
}

    // TEST c longest algo
    //
static void   BM_c_longest( benchmark::State& bs )
{
    test_block  test( []( auto & str ) { return Find::longest_c( str.c_str() ) ; } ) ;
    test.bench( bs ) ;
}

    // TEST c longest algo
    //
static void   BM_c_clever_longest( benchmark::State& bs )
{
    test_block  test( []( auto & str ) { return Find::longest_c_clever( str.c_str() ) ; } ) ;
    test.bench( bs ) ;
}

BENCHMARK( BM_cpp_longest ) ;
BENCHMARK( BM_cpp_longest_tune ) ;
BENCHMARK( BM_c_longest ) ;
BENCHMARK( BM_c_clever_longest ) ;

BENCHMARK_MAIN() ;

