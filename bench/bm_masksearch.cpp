#include <benchmark/benchmark.h>

#include <algorithm>
#include <iostream>

#include <cstdint>
#include <string.h>

#include "gen_string.h"

using namespace std ;

namespace
{

const static int  kKeySize = 16 ;

struct	Block
{
    Block() {}
    Block( const vector<uint8_t> & v )
    {
      memmove( data, v.data(), kKeySize ) ;
    }

    uint8_t data[ kKeySize ] ;
} ;

class   OffsetMatch
{
    public:
      OffsetMatch( int offset, vector<uint8_t> && key ) : offset_( offset ), key_( std::move( key ) )
      { }

      bool  operator()( const Block & element ) 
      {
          auto ptr= element.data + offset_ ;
          for ( auto & k : key_ ) {
            if ( k != * ptr ) { return false ; }
            ptr ++ ;
          }
          return true ;
      }
        
    protected:
      int offset_ ;
      vector<uint8_t>  key_ ;
} ;

template <int N>
class   MaskMatch
{
    public:
      MaskMatch( int offset, const vector<uint8_t> & key )
      {
          mask_.reserve( N / 2 ) ;
          while ( offset >= 4 ) {
            mask_.push_back( 0 ) ;
            mask_.push_back( 0 ) ;
            offset -= 4 ;
          }

          uint32_t  tmpmsk, tmpval  ;
          int i, tmpshft ;
          auto itk = key.begin() ;
          for ( i = key.size(), tmpshft = 0 ; ( i -- ) ; tmpshft += 8 )
          {
            if ( tmpshft == 32 ) {
              mask_.push_back( tmpmsk ) ;  tmpmsk = 0 ;
              mask_.push_back( tmpval ) ;  tmpval = 0 ;
              tmpshft = 0 ;
            }

            tmpmsk |= ( 0xff << tmpshft ) ;
            tmpval |= ( * (itk ++) << tmpshft ) ;
          }

          if ( tmpshft ) {
            mask_.push_back( tmpmsk ) ;  tmpmsk = 0 ;
            mask_.push_back( tmpval ) ;  tmpval = 0 ;
          }
      }

      bool  operator()( const Block & element )
      {
          auto ptr= element.data ;
          auto ptrend= ptr + sizeof( element.data ) ;
          auto it = mask_.begin() ;

          uint32_t  val ;

          while ( (it < mask_.end() ) && ptr < ptrend ) {
            memcpy( & val, ptr, sizeof( val ) ) ;  ptr += sizeof( val ) ;
            val &= * (it ++ ) ;
            if ( val != *( it ++ ) ) { return false ; }
          }
          return true ;
      }

    protected:
      vector< uint32_t >  mask_ ;
} ;

template <int N>
struct Test
{
    Test()
    {
        world.reserve( N ) ;
        for ( int i = N ; ( i -- ) ; ) {
          world.emplace_back( Gen::bytes( kKeySize ) ) ;
        }
    }

    int   match( auto matcher )
    {
	int ct = 0 ;
	for ( auto & ele : world ) {
          if ( matcher( ele ) ) { ct ++ ; }
	}
        return ct ;
    }

    vector<Block>   world ;
} ;

const static int  kWorldSize = 1024 * 1024 ;

}

    // TEST offset.compare

static void   BM_offset_test( benchmark::State& bs )
{
    int match ;
    Test< kWorldSize >  test ;

    for ( auto _ : bs ) {
      OffsetMatch   matcher( 8, Gen::bytes( 2 ) ) ;
      match = test.match( matcher ) ;
    }
}

    // TEST long.mask

static void   BM_mask_test( benchmark::State& bs )
{
    int match ;
    Test< kWorldSize >  test ;

    for ( auto _ : bs ) {
      MaskMatch< kKeySize > matcher( 8, Gen::bytes( 2 ) ) ;
      match = test.match( matcher ) ;
    }
}

BENCHMARK( BM_offset_test ) ;
BENCHMARK( BM_mask_test ) ;

BENCHMARK_MAIN() ;

