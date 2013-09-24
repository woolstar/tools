
#include <buffer>

using	btl::build_base ;
using	btl::scanner ;

build_base:: ~ build_base() { }

void	build_base::copy(buffer const & abuf)
{
	scanner<sized_storage>	scan( abuf) ;

	while ( scan.hasdata() && ( fill_ < limit_ ) ) { *( fill_ ++)= * (scan ++ ) ; }
}

