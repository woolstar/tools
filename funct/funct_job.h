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
	spinup() ;
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

#if __cplusplus >= 202002L

class   JobJ
{
    public:
      ~ JobJ() ;

      void    launch( auto function )
      {
        _thread= std::jthread( function ) ;
        _token= _thread.get_stop_token() ;
      }

      explicit operator bool() const { return _thread.joinable() ; }
      bool              active() const { return _token.stop_possible() ; }

      void              stop() { _thread.request_stop() ; }
      void              wait() ;

    protected:
      std::jthread      _thread ;
      std::stop_token   _token ;
} ;

#endif

}
}

#endif
