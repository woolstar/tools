#include <io>

using btl::io ;

btl::iobasic	btl::ioin { 0 } ;
btl::iobasic	btl::ioout { 1 } ;
btl::iobasic	btl::ioerr { 2 } ;

io::Init::Init()
{
	if ( refcount_ ) { return ; }

	refcount_ ++ ;
}

io::Init:: ~ Init()
{
	if ( refcount_ )
	{
		// flush ioout
	}
}

io::Init	__init ;

