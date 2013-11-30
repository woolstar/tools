#include <io>

using btl::socket ;

socket:: ~ socket()
{
	if ( active_ )
	{
		active_ = false ;
		dispose( port_ ) ;
	}
}

int	socket::read(btl::build_base & zdest) const
{
	int iret, istruc, isz ;
	void * bufptr ;

	if ( ! active_ ) return 0 ;

	isz= zdest.remaining() ;
	if ( isz < 1 ) return 0 ;

	bufptr= fill( zdest) ;
	iret= ::recv( port_, bufptr, isz, 0) ;

	if ( iret < 0 ) { return iret ; }
	if ( 0 == iret ) {
		active_= false ;
		return 0 ;
	}

	jump( zdest, iret) ; 	// advance fill pointer in buffer
	return iret ;
}

int	socket::read_from(btl::build_base & zdest, struct sockaddr & zaddr) const
{
	int iret, istruc, isz ;
	socklen_t iaddrsz ;
	void * bufptr ;

	if ( ! active_ ) return 0 ;

	isz= zdest.remaining() ;
	if ( isz < 1 ) return 0 ;

	bufptr= fill( zdest) ;
	iaddrsz= sizeof( zaddr) ;
	iret= ::recvfrom( port_, bufptr, isz, 0, & zaddr, & iaddrsz) ;

	if ( iret < 0 ) { return iret ; }
	if ( 0 == iret ) {
		active_= false ;
		return 0 ;
	}

	jump( zdest, iret) ; 	// advance fill pointer in buffer
	return iret ;
}

int	socket::print(const btl::buffer & abuf) const
{
	if ( ! active_ ) return -1 ;
	return ::send(port_, * abuf, abuf.size(), 0) ;
}

int socket::ctrl(int, void *) const
{
	return -1 ;
}


