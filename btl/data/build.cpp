
#include <buffer>
#include <stdarg.h>

using	btl::build_base ;
using	btl::scanner ;

build_base:: ~ build_base() { }

void	build_base::copy(buffer const & abuf)
{
	scanner<sized_storage>	scan( abuf) ;

	while ( scan && ( fill_ != limit_ ) ) { datasize_ ++ ;  *( fill_ ++)= * (scan ++ ) ; }
}

void	build_base::copy(scanner<sized_storage> & ascan)
{
	while ( ascan && ( fill_ != limit_ ) ) { datasize_ ++ ;  *( fill_ ++)= * (ascan ++ ) ; }
}

void	build_base::print(const char * afmt, ...)
{
	va_list vl ;
	int iret ;

	va_start(vl, afmt) ;
	iret= vsnprintf(reinterpret_cast<char *>( fill_), remaining(), afmt, vl) ;
	va_end( vl) ;

	if ( iret > 0 ) { jump( iret ) ; }
}

