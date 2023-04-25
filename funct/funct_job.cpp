// funct_job.cpp			-*-C++-*-

#include "funct_job.h"
#include <time.h>


namespace Wool {
namespace Funct {

Job::Job() : is_active( false ), is_running( false )
{ }

Job:: ~ Job()
{
    stop() ;
    wait() ;
    if ( _thread.joinable() ) { _thread.join() ; }
}

void
Job::wait()
{
    while ( is_running ) {
      struct timespec ns = { 0, 100000 } ;
      nanosleep( & ns, nullptr ) ;
    }
}

void
Job::spinup()
{
    while ( ! is_running ) {
      struct timespec ns = { 0, 100000 } ;
      nanosleep( & ns, nullptr ) ;
    }
}

#if __cplusplus >= 202002L

JobJ:: ~ JobJ()
{
    stop() ;
    wait() ;
}

void
JobJ::wait()
{
    if ( _thread.joinable() ) { _thread.join() ; }
}

#endif

}
}

