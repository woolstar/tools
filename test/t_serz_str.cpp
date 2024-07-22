
#include "serz_buildstr.h"
#include "serz_buildtools.h"

#include <gtest/gtest.h>


namespace Wool {
namespace Serialize {
namespace test {

using namespace ::testing ;
using namespace std ;

TEST( SerzString, basic )
{
    BuildString  test ;

    EXPECT_EQ( test.size(), 0 ) ;

    test.add( "abc", 3 ) ;
    EXPECT_EQ( test.size(), 3 ) ;
    EXPECT_EQ( ** test, 'a' ) ;
}

TEST( SerzString, reserve )
{
    {
	BuildTools<BuildString>	 test( 32 ) ;

	test.add( 5000ULL ) ;
    }
    {
	BuildTools<BuildString>	 test ;

	test.reserve( 32 ) ;
	test.add( 100 ) ;
	test.add( 200 ) ;
	test.add( 300 ) ;
	test.add( 400 ) ;

	EXPECT_EQ( test.size(), 4 * sizeof(int) ) ;
    }
}

}
}
}

