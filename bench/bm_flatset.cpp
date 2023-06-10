#include <benchmark/benchmark.h>

#include <set>
#include <boost/container/flat_set.hpp>

#include <random>
#include <algorithm>
#include <iostream>

#include "gen_string.h"

using namespace std ;

namespace
{

template<typename CONTAINER, int N>
struct test_block
{
    test_block( const vector<string> & labels, int sz )
    {
        vector<string>  local ;
        vector<string>  thrash ;
        mt19937   ubb(Gen::random()) ;

        thrash.reserve( N * sz) ;

        for ( auto & ele : block )
        {
          local.assign( labels.begin(), labels.end() );
          shuffle( local.begin(), local.end(), ubb ) ;
          local.resize( sz ) ;
            
          for ( auto & s : local )
          {
            ele.emplace( move( s ) ) ;
            thrash.emplace_back( Gen::block() ) ;
          }
        }
    }

    int   lookup( const vector<string> & keys )
    {
        int match= 0 ;
        auto ikey= keys.begin() ;

        for ( auto & ele : block )
        {
          if ( ikey == keys.end() )
            { exit(1) ; }
          auto it= ele.find( * (ikey ++ )) ;
          if ( it != ele.end() ) { match ++ ; }
        }

        return match ;
    }

    CONTAINER   block[ N ] ;
} ;

const static int  kKeySize = 500 ;
const static int  kSetSize = 300 ;

const static int  kConstelationSize = 2000 ;

}

static void   BM_set_test( benchmark::State& bs )
{
    auto  keys = Gen::group( kKeySize ) ;

    test_block< std::set<string>, kConstelationSize >   test( keys, kSetSize ) ;
    auto searches = Gen::searches( keys, kConstelationSize ) ;
    int match ;

    for ( auto _ : bs )
    {
      match= test.lookup( searches ) ;
    }
}

static void   BM_flat_test( benchmark::State& bs )
{
    auto  keys = Gen::group( kKeySize ) ;

    test_block< boost::container::flat_set<string>, kConstelationSize >   test( keys, kSetSize ) ;
    auto searches = Gen::searches( keys, kConstelationSize ) ;
    int match ;

    for ( auto _ : bs )
    {
      match= test.lookup( searches ) ;
    }
}

BENCHMARK( BM_set_test ) ;
BENCHMARK( BM_flat_test ) ;

BENCHMARK_MAIN() ;

