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

  // for whatever dictionary under test, construct N for testing
template<typename CONTAINER, int N>
struct test_block
{
      // populate dictionaries with *sz* keys from source *labels*
    test_block( const vector<string> & labels, int sz )
    {
        vector<string>  local ;
        vector<string>  thrash ;
        mt19937   ubb(Gen::random()) ;

        thrash.reserve( N * sz) ;

        for ( auto & ele : block )
        {
            // grab labels into local vector
          local.assign( labels.begin(), labels.end() );
            // randomize
          shuffle( local.begin(), local.end(), ubb ) ;
            // keep *sz* labels (discard rest, assumption labels.size() > sz
          local.resize( sz ) ;
            
          for ( auto & s : local )
          {
            ele.emplace( std::move( s ) ) ;
              // after each insert, thrash some memory
            thrash.emplace_back( Gen::block() ) ;
          }
        }
    }

      // actual test
      // take first N *keys* ( keys.size() > N ) and lookup one key from each dictionary
    int   lookup( const vector<string> & keys )
    {
        int match= 0 ;
        auto ikey= keys.begin() ;

        for ( auto & ele : block )
        {
          if ( ikey == keys.end() )  // should not happen
            { exit(1) ; }
          auto it= ele.find( * (ikey ++ )) ;
          if ( it != ele.end() ) { match ++ ; }
        }

        return match ;
    }

    CONTAINER   block[ N ] ;
} ;

    // do a relatively small population of keys, with 60% used in each dictionary
const static int  kKeySize = 500 ;
const static int  kSetSize = 300 ;

    // test enough to blow out most caches
const static int  kConstelationSize = 2000 ;

}

    // TEST std::set
    //
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

    // TEST boost::container::flat_set
    // in the future will test std::flat_set once c++23 is available
    //
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

