
#include "serz_buildsz.h"
#include "serz_buildtools.h"

#include <gtest/gtest.h>


namespace Wool {
namespace Serialize {
namespace test {

using namespace ::testing ;
using namespace std ;

namespace {

}

TEST( SerzSZ, basic )
{
    BuildSz	test ;

    EXPECT_EQ( test.size(), 0 ) ;

    test.add( "abcd", 4 ) ;
    EXPECT_EQ( test.size(), 4 ) ;
}

TEST( SerzSZ, tools )
{
    BuildTools<BuildSz>  test ;

    test.add( 4L ) ;
    EXPECT_EQ( test.size(), sizeof( long )) ;
}

}
}
}

