
#include <io>

using btl::io ;
using btl::IO_Port ;
using btl::manage ;

	static bool operator<( const pollfd & a, const pollfd & b) { return a.fd < b.fd ; }

	////
	//

void	manage::destroy( void)
{
	poll_.clear() ;
	stor_.clear() ;
}

	////
	//	watch & watch to time
	//

void	manage::watch( void)
{
	struct timespec t= { -1, 0 } ;	// poll indefinitely
	int iret ;

	iret= ppoll( & (poll_[0] ), poll_.size(), & t, NULL ) ;
	if ( iret > 0 ) { scan( iret) ; }
}

btl::manage::Results	manage::watch( int amsec )
{
	struct timespec t= { amsec / 1000, ( amsec % 1000 ) * 1000000 } ;
	int iret ;

	iret= ppoll( & (poll_[0] ), poll_.size(), & t, NULL ) ;
	if ( iret > 0 ) { scan( iret) ;  return eActivity ; }

	return ( iret < 0 ) ? eError : eIdle ;
}

	////
	//	scan
	//	go through pollfd and pull out active items

void	manage::scan( int aactive)
{
	struct {
		int mask ;
		int io ;
	}
		actlist[ aactive],
		* actstep = actlist,
		* actfar = actlist + aactive ;

		// build up a temperary list from active records
	for ( const auto & p : poll_ ) { 
		if ( p.revents ) {
			actstep-> mask= p.revents ,
			actstep-> io= p.fd ;
			++ actstep ;
			if ( actstep == actfar ) { break ; }
		}
	}

#ifdef TEST
	assert( actstep == actfar ) ;
#endif

		// call out activity from temp list
	for ( auto i : actlist ) { activity( i.mask, i.io) ; }
}

	////
	//	activity
	//	deal with activity on one IO

void	manage::activity( int amask, int aport )
{
	const concept_t * aptr= stor_[aport].get() ;

	if ( amask & ( POLLIN | POLLHUP )) { aptr-> doread_() ; }
	if ( amask & POLLOUT ) { aptr-> dowrite_() ; }

	if ( ! aptr-> isactive_() || ( amask & POLLERR ))
	{
		auto p= std::find_if( poll_.begin(), poll_.end(), [aport](const pollfd& rec){ return rec.fd == aport ; }) ;
		if ( p != poll_.end()) { poll_.erase( p) ; }
		stor_.erase( aport) ;
	}
}

	//
	//

