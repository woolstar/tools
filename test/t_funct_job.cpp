#include "funct_job.h"

#include <string>
#include <gtest/gtest.h>


namespace Wool {
namespace Funct {
namespace test {

using namespace ::testing ;
using namespace std ;


TEST( Tokenize, example_usage )
{
    Job   runner ;
    
    runner.launch( [](auto & go) { int i ;  while ( go ) { i ++ ; } } ) ;
    EXPECT_TRUE( runner ) ;

    runner.stop() ;
    runner.wait() ;
    EXPECT_FALSE( runner ) ;
}

TEST( Tokenize, setup )
{
    Job   runner ;
}

}
}
}
