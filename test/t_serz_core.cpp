
#include "serialize.h"
#include "serz_buildsz.h"
#include "serz_buildstr.h"
#include "serz_buildtools.h"
#include "serz_variable.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


namespace Wool {
namespace Serialize {
namespace test {

using namespace ::testing ;
using namespace std ;

namespace {

struct point
{
  uint16_t  x, y ;
} ;

}

TEST( Serialize, basic )
{
    Serialize< BuildTools< BuildString >>  test ;

    EXPECT_EQ( test.size(), 0 ) ;

    uint16_t  val = 3 ;
    uint32_t  val2 = 100 ;
    test << val
         << val2 ;

    EXPECT_EQ( test.size(), ( sizeof( uint16_t ) + sizeof( uint32_t ) )) ;
    EXPECT_EQ( ** test, 3 ) ;
}

TEST( Serialize, wrap )
{
    Serialize< BuildTools< BuildString >>  test ;

    uint32_t  val = 1 ;
    point  pt( 1, 2 ) ;

    test << val
         << VariableStruct<uint16_t>( pt ) ;

    EXPECT_EQ( test.size(), ( sizeof( val ) + sizeof( uint16_t ) + sizeof( pt ) ) ) ;
    EXPECT_EQ( ** test, val ) ;
    EXPECT_EQ( *( * test + sizeof(val)), sizeof( pt ) ) ;
}

TEST( Serialize, repeatable )
{
    int val = 20 ;
    point pt( 100, 200 ) ;

    auto pack = [&](auto & ser) 
               {
                  ser << val
                      << VariableStruct<uint16_t>( pt ) ;
               } ;

    Serialize< BuildTools< BuildSz >>  testsz ;
    pack( testsz ) ;

    Serialize< BuildTools< BuildString >> test ;
    pack( test ) ;

    ASSERT_EQ( testsz.size(), test.size() );
}

}
}
}

