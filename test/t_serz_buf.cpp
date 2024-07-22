
#include "serz_buildbuf.h"
#include "serz_buildtools.h"
#include "serz_limit.h"

#include <gtest/gtest.h>


namespace Wool {
namespace Serialize {
namespace test {

using namespace ::testing ;
using namespace std ;

TEST( SerzBuffer, basic )
{
    char tmp[100] ;
    BuildBuf< LimitNone >   test( tmp, sizeof(tmp) ) ;

    EXPECT_EQ( test.size(), 0 ) ;

    test.add( "abc", 3 ) ;
    EXPECT_EQ( test.size(), 3 ) ;
    EXPECT_EQ( ** test, 'a' ) ;
}

TEST( SerzBuffer, limit )
{
    char tmp[100] ;
    {
        BuildBuf< LimitTrack >  test( tmp, 4 ) ;

        test.add( "abc", 3 ) ;
        EXPECT_EQ( test.size(), 3 ) ;

        test.add( "ef", 2 ) ;
        EXPECT_EQ( test.size(), 3 ) ;
    }
    {
        BuildTools< BuildBuf< LimitThrow<> >>  test( tmp, 4 ) ;
        test.add( 300U ) ;

        EXPECT_EQ( test.size(), sizeof( int ) ) ;
        EXPECT_ANY_THROW( test.add( 1 ) ) ;
    }
}

TEST( SerzBuffer, endian )
{
    char tmp[32] ;

    BuildEndian< BuildBuf< LimitThrow<> > >   test( tmp, 4 ) ;

    uint16_t  tmpv = 0x0201 ;
    test.add( tmpv ) ;

    EXPECT_EQ( ** test, 0x02 ) ;
}

}
}
}

