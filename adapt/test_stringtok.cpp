
#include "adapt_stringtok.h"

#include <string>
#include <gtest/gtest.h>

namespace Wool {
namespace Adapt {
namespace test {

using namespace ::testing ;

TEST( Tokenize, example_usage )
{
    int   counttok = 0 ;
    int   countperiod = 0 ;

    const std::string   teststring = "a b cd. e" ;
      // sequence should be "a" "b" "cd" "." "e"

    for ( auto s : StringTokenize( teststring ) )
    {
        counttok ++ ;
        if ( s == "." ) { countperiod ++ ; }
    }

    EXPECT_EQ( counttok, 5 ) ;
    EXPECT_EQ( countperiod, 1 ) ;
}

//

TEST( Tokenize, empty )
{
    StringTokenize  test( "" ) ;

    EXPECT_EQ( * test, "" ) ;
    EXPECT_FALSE( test ) ;

    test ++ ;
    EXPECT_FALSE( test ) ;
}

TEST( Tokenize, also_empty )
{
    StringTokenize  test ;

    EXPECT_EQ( * test, "" ) ;
    EXPECT_FALSE( test ) ;

    test= "" ;
      // still empty
    EXPECT_EQ( * test, "" ) ;
    EXPECT_FALSE( test ) ;
}

TEST( Tokenize, word )
{
    static const char *   teststr= "abc" ;
    StringTokenize        test( teststr ) ;

    EXPECT_TRUE( test ) ;
    EXPECT_EQ( * test, teststr ) ;
    EXPECT_EQ( test.remaining(), 0 ) ;

    test ++ ;
    EXPECT_FALSE( test ) ;

      // no op
    test ++ ;
    EXPECT_FALSE( test ) ;
}

TEST( Tokenize, words )
{
    static const char *   teststr= "ab de" ;
    StringTokenize        test( teststr ) ;

    EXPECT_TRUE( test ) ;
    EXPECT_EQ( * test, "ab" ) ;

    test ++ ;
    EXPECT_EQ( * test, "de" ) ;
    EXPECT_EQ( test.remaining(), 0 ) ;

    test ++ ;

    EXPECT_FALSE( test ) ;
}

TEST( Tokenize, punctuation )
{
    static const char *   teststr= ".,; ." ;
    StringTokenize        test( teststr ) ;

    EXPECT_TRUE( test ) ;
    EXPECT_EQ( (*test).front(), '.' ) ;

    test ++ ;
    EXPECT_EQ( (*test).front(), ',' ) ;

    test ++ ;
    EXPECT_EQ( (*test).front(), ';' ) ;

    test ++ ;
    EXPECT_EQ( (*test).front(), '.' ) ;

    test ++ ;
    EXPECT_FALSE( test ) ;
}

}
}
}

