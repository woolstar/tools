#include <io>

using btl::io ;
using btl::iobasic ;

//
//	base interface
//	basic posix IO calls
//

iobasic::iobasic( btl::IO_Port aport ) : io( aport) { }

int		iobasic::read( btl::build_base & zdest ) const
{
	int iret, isz ;
	void * bufptr ;

	if ( ! active_ ) return 0 ;

	isz= zdest.remaining() ;
	if ( isz < 1 ) return 0 ;

	bufptr= fill(  zdest) ;
	iret= ::read( port_, bufptr, isz) ;

	if ( iret < 0 ) { return iret ; }
	if ( 0 == iret ) {
		active_= false ;
		return 0 ;
	}

	jump( zdest, iret) ; 	// advance fill pointer in buffer
	return iret ;
}

int		iobasic::print( const buffer & abuf ) const
{
	int iret, isz ;

	isz= abuf.size() ;
	if ( isz ) { return ::write( port_, * abuf, isz) ; }

	return 0 ;
}

int		iobasic::ctrl( int acode, void * aptr ) const
{
	return -1 ;
}

