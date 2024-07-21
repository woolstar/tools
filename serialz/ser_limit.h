// ser_limit.h

#ifndef SER_LIMIT_DEF
#define SER_LIMIT_DEF 1

#include <utility>
#include <stddef.h>

namespace Wool {
namespace Serialize {

class	LimitNone
{
    public:
	LimitNone() ;
	LimitNone(size_t) ;
	~ LimitNone() ;

	bool test(size_t) { return true ; }

} ;

class	LimitTrack
{
    public:
	LimitTrack( size_t lim) : limit_( lim ) { }

    protected:
	bool test( size_t ) ;
	virtual void fail(size_t) {}

	size_t  limit_, current_ = 0 ;
} ;

template<typename BASELIMIT>
class	LimitOps : public BASELIMIT
{
    public:
	LimitOps() {}

	template <typename ...ARGS>
	LimitOps(ARGS&& ...args) : BASELIMIT( std::forward<ARGS>( args )... ) {}

	void operator++() { BASELIMIT::test( 1 ) ; }
	void operator++(int) { BASELIMIT::test( 1 ) ; }
	LimitOps & operator+=(size_t t) { BASELIMIT::test( t ) ;  return * this ; }
} ;

////

}
}

#endif
