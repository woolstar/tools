
#include "serz_limit.h"

#include <gtest/gtest.h>
#include <stdexcept>

namespace Wool {
namespace Serialize {
namespace test {

using namespace ::testing ;
using namespace std ;

namespace {

class	LimitAssert : public LimitOps<LimitTrack>
{
    public:
      LimitAssert( size_t n ) : LimitOps( n ) {}

      void fail(size_t) override
      {
	throw out_of_range( "reached limit" ) ;
      }
} ;

}

TEST( SerzLim, smoke )
{
    {
      LimitOps<LimitNone>	 test ;

      test ++ ;
    }

    {
      LimitOps<LimitTrack>  test( 4 ) ;

      test ++ ;
    }
}

TEST(SerzLim, track )
{
    LimitAssert   test( 4 ) ;

    test += 4 ;
    EXPECT_ANY_THROW( test ++ ) ;
}

}
}
}

