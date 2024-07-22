
#include "serz_buildstr.h"
#include "serz_buildtools.h"
#include "serz_variable.h"

#include "serz_limit.h"

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

struct FakeBlock
{
    MOCK_METHOD( void, add, (size_t) ) ;
    MOCK_METHOD( void, add, (const char *, size_t) ) ;
} ;


}

TEST( SerzVar, basic )
{
    Variable<uint8_t>   test( "abcd", 4) ;
    FakeBlock mockblock ;

    EXPECT_CALL( mockblock, add( 4 ) ).WillOnce( Return() ) ;
    EXPECT_CALL( mockblock, add( _, 4 ) ).WillOnce( Return() ) ;
    test.serialize( mockblock ) ;
}

TEST( SerzVar, adapt )
{
    {
      Variable<uint8_t>   test( "abcd" ) ;
      BuildTools< FakeBlock > mockblock ;

      EXPECT_CALL( mockblock, add( _, 1 )).WillOnce( Return() ) ;
      EXPECT_CALL( mockblock, add( _, 4 )).WillOnce( Return() ) ;
      test.serialize( mockblock ) ;
    }
    {
      point pt( 100, 20 ) ;
      VariableStruct<uint16_t>  test( pt ) ;
      BuildTools< FakeBlock > mockblock ;
      
      EXPECT_CALL( mockblock, add( _, sizeof( uint16_t )) ).WillOnce( Return() ) ;
      EXPECT_CALL( mockblock, add( _, sizeof( point )) ).WillOnce( Return() ) ;
      test.serialize( mockblock ) ;
    }
}

}
}
}

