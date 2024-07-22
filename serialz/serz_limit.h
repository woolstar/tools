// serz_limit.h

#ifndef SERZ_LIMIT_DEF
#define SERZ_LIMIT_DEF 1

#include <utility>
#include <stddef.h>
#include <stdexcept>


namespace Wool {
namespace Serialize {

class	LimitNone
{
    public:
	LimitNone() ;
	LimitNone(size_t) ;
	~ LimitNone() ;

        bool operator()(size_t) { return true ; }

} ;

class	LimitTrack
{
    public:
	LimitTrack( size_t lim) : limit_( lim ) { }

	bool operator()( size_t ) ;

    protected:
	virtual void fail(size_t) {}

	size_t  limit_, current_ = 0 ;
} ;

template < typename EXCEPT = std::out_of_range >
class	LimitThrow : public LimitTrack
{
    public:
      using LimitTrack::LimitTrack ;

      void  fail(size_t) override {
              throw EXCEPT( "exceeded buffer limit" ) ;
            }
} ;

}
}

#endif
