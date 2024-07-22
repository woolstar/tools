
#include "serz_limit.h"

#include <gtest/gtest.h>


namespace Wool {
namespace Serialize {
namespace test {

using namespace ::testing ;
using namespace std ;

namespace {

}

TEST( SerzLim, smoke )
{
    {
      LimitNone  test ;

      test( 1 ) ;
    }

    {
      LimitTrack   test( 4 ) ;

      test( 1 ) ;
    }
}

TEST(SerzLim, track )
{
    LimitThrow<>  test( 4 ) ;

    test( 4 ) ;
    EXPECT_ANY_THROW( test(1) ) ;
}

}
}
}

