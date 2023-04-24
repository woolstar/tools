// funct_job.h			-*-C++-*-
#ifndef  INCLUDED_FUNCT_JOB
#define  INCLUDED_FUNCT_JOB 1

#include <functional>
#include <thread>
#include <atomic>


namespace Wool {
namespace Funct {

class	Job
{
    public:
      Job() ;
      ~ Job() ;

      void    launch( auto func )
      {
        auto runner= [func, this]
                     {
                       is_active= true ;
                       is_running= true ;

                       func( is_active ) ;

                       is_running= false ;
                     } ;

        _thread = std::thread( runner ) ;
      }

      explicit operator bool() const { return is_running ; }
      bool  active() const { return is_active ; }
      void  stop() { is_active= false ; }

      void  wait() ;
        
    protected:
      std::thread         _thread ;
      std::atomic<bool>   is_active, is_running ;

      void    spinup() ;
} ;

}
}

#endif
