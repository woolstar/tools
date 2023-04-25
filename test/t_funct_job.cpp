#include "funct_job.h"

#include <string>
#include <gtest/gtest.h>


namespace Wool {
namespace Funct {
namespace test {

using namespace ::testing ;
using namespace std ;


TEST( Job, example_usage )
{
    Job   runner ;
    
    runner.launch( [](auto & go) { int i ;  while ( go ) { i ++ ; } } ) ;
    EXPECT_TRUE( runner ) ;

    runner.stop() ;
    runner.wait() ;
    EXPECT_FALSE( runner ) ;
}

#if __cplusplus >= 202002L

TEST(JobJ, example_usage )
{
    JobJ  jrunner ;

    jrunner.launch( [](auto token){ int i ;  while ( ! token.stop_requested() ) { i ++ ; } } ) ;
    EXPECT_TRUE( jrunner ) ;

    jrunner.stop() ;
    jrunner.wait() ;
    EXPECT_FALSE( jrunner ) ;
}

#endif

TEST( Job, setup )
{
    Job   runner ;
}

#if __cplusplus >= 202002L

TEST( JobJ, setup )
{
    JobJ  jrunner ;
}

#endif

}
}
}
