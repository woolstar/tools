
#include <buffer>
#include <stdarg.h>

using	btl::build_base ;
using	btl::scanner ;

build_base:: ~ build_base() { }

void	build_base::copy(buffer const & abuf)
{
	scanner<sized_storage>	scan( abuf) ;
	sized_storage * xptr= ( sized_storage *) far_ ;

	while ( scan && ( xptr != limit_ ) ) { *( xptr ++)= * (scan ++ ) ; }
	far_= xptr ;
}

void	build_base::copy(scanner<> & ascan)
{
	sized_storage * xptr= ( sized_storage *) far_ ;

	while ( ascan && ( xptr != limit_ ) ) { *( xptr ++)= * (ascan ++ ) ; }
	far_= xptr ;
}

void	build_base::copy(scanner<> & ascan, size_t asz)
{
	sized_storage * xptr= ( sized_storage *) far_ ;

	while ( ascan && asz && ( xptr != limit_ ) ) { *( xptr ++)= * (ascan ++ ) ;  -- asz ; }
	far_= xptr ;
}

void	build_base::print(const char * afmt, ...)
{
	va_list vl ;
	int iret ;

	va_start(vl, afmt) ;
	iret= vsnprintf((char *) far_, remaining(), afmt, vl) ;
	va_end( vl) ;

	if ( iret > 0 ) { jump( iret ) ; }
}

